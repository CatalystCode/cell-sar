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

var pendingSMSs = [];
var silentSMSCount = {};      // key: IMSI, value: # of silent SMSs sent

function sendHelloSMS(subscriber) {
   var try_time = Date.now() / 1000 + 5; // 5s delay before sending to allow the call to settle

   var imsi = subscriber.imsi;
   var msisdn = subscriber.msisdn;
   var sms = {
      'imsi': droneRootImsi,
      'msisdn': droneRootMsisdn,
      'smsc': droneRootMsisdn,
      'dest': msisdn,
      'dest_imsi': imsi,
      'next_try': try_time,
      'tries': 3,
      'msg': helloText
   };

   if (!sendSMS(sms)) {
      Engine.debug(Engine.DebugInfo, "Failed to send HelloSMS to IMSI: " + subscriber.imsi);
   }
}

function sendSilentSMS(imsi) {
    var subscriber = getSubscriber(imsi);
    if (!subscriber) return false;
    
    Engine.debug(Engine.DebugInfo, "Sending SilentSMS to IMSI " + imsi);

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

    var m = new Message("msg.execute");
    m.caller = droneRootImsi;
    m["sms.caller"] = droneRootMsisdn;
    m["rpdu"] = pduMessage;
    m.callto = subscriber["location"];
    m.oimsi = imsi;
    m.otmsi = subscriber["tmsi"];

    m.enqueue();
    return true;
}

function sendSMSMessage(imsi, messageText)
{
   var subscriber = getSubscriber(imsi);
   var sms = {
      'imsi': droneRootImsi,
      'msisdn': droneRootMsisdn,
      'smsc': droneRootMsisdn,
      'dest': subscriber.msisdn,
      'dest_imsi': subscriber.imsi,
      'msg': messageText
   };

   if (!sendSMS(sms)) {
      Engine.debug(Engine.DebugInfo, "Failed to SMS message to IMSI: " + subscriber["imsi"] + ", message was '" + messageText + "'");
   }
}

function sendSMS(sms) {
   // Assume all devices are always online, so we never end up pushing undeliverable messages to the end of the queue
   // May want to change this later, but for now it's a sane approach.

   Engine.debug(Engine.DebugInfo, "Sending SMS to IMSI: " + sms.dest_imsi + " '" + sms.msg + "'");

   var destSubscriber = getSubscriber(sms.dest_imsi);
   if (!destSubscriber) {
      Engine.debug(Engine.DebugInfo, "Did not deliver sms. Unknown dest_imsi: " + sms.dest_imsi);
      return false;
   }
   if (!destSubscriber.location) {
      Engine.debug(Engine.DebugInfo, "Did not deliver sms because destination is offline.");
      return false;
   }

   var m = new Message("msg.execute");
   m.caller = sms.smsc;
   m.called = sms.dest;
   m["sms.caller"] = sms.msisdn;
   if (sms.msisdn.substr(0, 1) === "+") {
      m["sms.caller.nature"] = "international";
      m["sms.caller"] = sms.msisdn.substr(1);
   }
   m.text = sms.msg;
   m.callto = destSubscriber["location"];
   m.oimsi = sms.dest_imsi;
   m.otmsi = destSubscriber["tmsi"];
   m.maxpdd = "5000";
   
   return m.enqueue();
}

function trySendingLater(sms) {
   var now = Date.now() / 1000;
   var later = now + 3; // 3 seconds later
   
   sms.next_try = later;
   if (sms.tries)
      sms.tries -= 1;

   if (sms.tries) {
      pendingSMSs.push(sms);
      return true;
   }

   return false;
}

function sendSilentSMSs() {
   for (var i = 0; i < activeSubscribers.length; i++) {
      var subscriber = activeSubscribers[i];
      if (!silentSMSCount[subscriber.imsi]) 
         silentSMSCount[subscriber.imsi] = 0;

      if (config.loud_sms) {
         var text = "Silent SMS #" + silentSMSCount[subscriber.imsi]  + ". ";
         if (subscriber.lastPhyinfo) {
            var phy = subscriber.lastPhyinfo;
            text += "Last Phyinfo: {TA: " + phy.TA + ", TE: " + phy.TE 
               + ", UpRSSI: " + phy.UpRSSI + ", TxPwr: " + phy.TxPwr 
               + ", DnRSSIdBm: " + phy.DnRSSIdBm + ", time: " + phy.time + "}";
         }

         sendSMSMessage(subscriber.imsi, text);

      } else if (!sendSilentSMS(subscriber["imsi"])) {
         Engine.debug(Engine.DebugInfo, "Failed to dispatch StealthSMS to IMSI: " + subscriber["imsi"]);
      }

      silentSMSCount[subscriber.imsi] += 1;
   }
}

function onRouteSMS(msg) {
   if (!msg.caller || !msg.called || !msg['sms.called'])
      return false;

   msg.retValue(droneRootImsi);
   return true;
}

function sendNextSMS() {
   var now = Date.now()/1000;
   var sms;

   // check if we have any SMSs to send
   for (var i=0; i<pendingSMSs.length; i++) {
      if (pendingSMSs[i].next_try <= now) {
         sms = pendingSMSs[i];
         pendingSMSs.splice(i,1);
         break;
      }
   }

   // if there is one, attempt to send it
   if (sms) {
      var sms_sent = sendSMS(sms);
      if (sms_sent) {
         Engine.debug(Engine.DebugInfo, "Successfully sent SMS to IMSI " + sms.dest_imsi);
         if (onSendSMS) onSendSMS(sms);
      } else {
         if (trySendingLater(sms))
            Engine.debug(Engine.DebugInfo, "Failed to send SMS to IMSI: " + sms.dest_imsi + " - " + sms.msg);
         else
            Engine.debug(Engine.DebugInfo, "Gave up trying to send SMS to IMSI " + sms.dest_imsi + ". Message was '" 
               + sms.msg + "'");
      }
   }
}
