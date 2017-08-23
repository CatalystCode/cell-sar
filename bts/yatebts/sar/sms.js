/** sms.js: utility functions for sending sms messages
  * 
  * Cellular Search and Rescue - Cellular Sensor BTS
  *   Copyright (C) 2017 Microsoft
  * 
  * This file is part of cell-sar/the Yate-BTS Project http://www.yatebts.com
  * 
  * cell-sar is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  * 
  * cell-sar is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with cell-sar.  If not, see <http://www.gnu.org/licenses/>.
  */

#require "subscribers.js"
#require "sar_config.js"
#require "utils.js"

var pendingSMSs = [];            // a queue of SMS messages that need to be handled
var silentSMSCount = {};         // IMSI-># of silent SMSs sent

function newSMS(to, from, text, options, silent) {
   var sms = {};
   sms.to = to;
   if (from)
      sms.from = from;
   else {
      sms.from = {
         'imsi': droneRootImsi,
         'msisdn': droneRootMsisdn
      };
   }
   sms.text = text;

   sms.tries = 0;
   sms.next_try = Date.now() / 1000;
   sms.maxpdd = 5000;

   sms.silent = false;
   if (silent)
      sms.silent = true;

   if (options) {
      if (options.tries && options.tries > 0)
         sms.tries = options.tries;
      if (options.next_try)
         sms.next_try = options.next_try;
      if (options.maxpdd && options.maxpdd > 0)
         sms.maxpdd = options.maxpdd;
      if (options.rpdu)
         sms.rpdu = options.rpdu;
   }

   return sms;
}

function smsToString(sms) {
   var result = "SMS from '" + sms.from.imsi + "' to '" + sms.to.imsi + "'";
   if (sms.silent)
      result += " (silent)";
   else 
      result += " \"" + sms.text + "\"";
   return result;
}

function dispatchSMS(sms) {
   if (!sms.to || !sms.to.location) return false;

   var m = new Message("msg.execute");

   // for both loud and silent SMSs
   m['caller'] = droneRootMsisdn;
   m['sms.caller'] = sms.from.msisdn;
   m['callto'] = sms.to.location;
   m['oimsi'] = sms.to.imsi;
   m['otmsi'] = sms.to.tmsi;
   
   if (sms.silent) {
      // only for silent SMSs
      m['rpdu'] = sms.rpdu;
   } else {
      // only for loud SMSs
      if (sms.from.msisdn.substr(0, 1) === '+') {
         m['sms.caller.nature'] = 'international';
         m['sms.caller'] = sms.from.msisdn.substr(1);
      }
      m['called'] = sms.to.msisdn;
      m['text'] = sms.text;
      m['maxpdd'] = sms.maxpdd;
   }


   var result = m.dispatch(true);
   if (!result) notifySMSFailed(sms, m);
   return result;
}

function notifySMSFailed(sms, msg) {
   var m = new Message("sms.failed");

   m.error = msg.error;
   m.reason = msg.reason;
   m.maxpdd = sms.maxpdd;

   m.silent = sms.silent;
   m.to_imsi = sms.to.imsi;
   if (sms.silent) {
      m.silentSMSCount = silentSMSCount[sms.to.imsi];
   } else {
      m.from_imsi = sms.from.imsi;
      m.text = sms.text;
   }

   m.enqueue();
}

function buildSilentSMS(subscriber) {
   var number = subscriber["msisdn"];
   var numberLen = number.length;
   var numberLenHex = numberLen.toString(16);
   if (numberLenHex.length < 2)
     numberLenHex = "0"+numberLenHex;

   // This message is very particular and exploits the "SilentSMS" Class0 'bug' that exists in the SMS signalling protocol
   // 00|01|00|0c|91|214365870921|00|C0|1e|005300650061007200630068002000260020005200650073006300750065
   //     ^     ^               ^     ^  ^                                     `- Message (this is "Search & Rescue")
   //     |     `- Num length   |     |  `- Message length
   //     |                     |     `- Class0 SMS
   // No delivery receipt      Target phone number, pair-swapped

   var pduMessage = "000100" + numberLenHex + "91" + generatePduNumber(number) +
   "00C01e005300650061007200630068002000260020005200650073006300750065";

   var options = {'rpdu': pduMessage};
   var sms = newSMS(subscriber, null, null, options, true);
   return sms;
};

function buildLoudSMS(subscriber) {

   var text = "Silent SMS #" + silentSMSCount[subscriber.imsi] + ".";
   var phy = subscriber.lastPhyinfo;
   if (phy)
      text += " Last Phyinfo: {TA: " + phy.TA + ", TE: " + phy.TE
         + ", UpRSSI: " + phy.UpRSSI + ", TxPwr: " + phy.TxPwr
         + ", DnRSSIdBm: " + phy.DnRSSIdBm + "}";

   var sms = newSMS(subscriber, null, text);
   return sms;
};

// try to send the next SMS in the pendingSMSs queue.
function sendNextSMS() {
   var now = Date.now() / 1000;
   var sms;

   // check if we have any SMSs to send
   for (var i = 0; !sms && i < pendingSMSs.length; ++i) {
      if (pendingSMSs[i].next_try <= now) {
         sms = pendingSMSs[i];
         pendingSMSs.splice(i, 1);
      }
   }

   // nothing to send
   if (!sms) return;
   
   Engine.debug(Engine.DebugInfo, "Sending '" + sms.text + "' to IMSI: '" + sms.to.imsi + "'");

   // try to send, if it fails, try again later
   if (dispatchSMS(sms)) {
      if (onSendSMS) onSendSMS(sms);
   } else {
      Engine.debug(Engine.DebugInfo, "Failed to dispatch SMS to IMSI " + subscriber.imsi);

      if (sms.tries) {
         Engine.debug(Engine.DebugInfo, "  Trying again...");
         sms.tries -= 1;
         sms.next_try = now + 2;
         pendingSMSs.push(sms);
      }
   }
}

function sendSilentSMSs() {
   for (var i = 0; i < activeSubscribers.length; ++i) {
      var subscriber = activeSubscribers[i];
      if (!silentSMSCount[subscriber.imsi])
         silentSMSCount[subscriber.imsi] = 0;

      var sms;
      if (config.loud_sms)
         sms = buildLoudSMS(subscriber);
      else 
         sms = buildSilentSMS(subscriber);

      if (!dispatchSMS(sms))
         Engine.debug(Engine.DebugInfo, "Failed to dispatch SilentSMS to IMSI " 
            + subscriber.imsi);

      silentSMSCount[subscriber.imsi] += 1;
   }
}

function onRouteSMS(msg) {
   if (!msg.caller || !msg.called || !msg['sms.called'])
      return false;

   msg.retValue(droneRootImsi);
   return true;
}

