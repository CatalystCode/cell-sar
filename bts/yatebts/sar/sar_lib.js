/** sar_lib.js: engine utility functions for the searchandrescue.js automation
  *
  * Cellular Search and Rescue - Cellular Sensor BTS
  *   Copyright (C) 2017 Microsoft
  * Yet Another Telephony Engine - Base Transceiver Station
  *   Copyright (C) 2013-2014 Null Team Impex SRL
  *   Copyright (C) 2014 Legba, Inc
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

#require "javascript.js"

#require "utils.js"
#require "sar_config.js"

#require "subscribers.js"
#require "sms.js"

/* ############### Engine State ############### */

Engine.debugName("searchandrescue");
var radioStopped = false;
var radioStoppedReason = null;

/* ############### SAR SDK ############### */

var sar = SearchAndRescue();

function writeToOCP(data, type) {
   var now = Date.now();
   var obj = {
      'type': type,
      'time': now,
      'data': data
   };

   var objStr = JSON.stringify(obj);
   sar.writeToOCP(objStr);
};

/* ############### SAR Initialization ############### */

function initializeSAR() {
   
   initializeNNSF();

   Engine.debug(Engine.DebugInfo, "Loading configuration from sar.conf");
   loadConfiguration();

   // install message handlers and callbacks
   Engine.debug(Engine.DebugInfo, "Installing SAR Listeners");
   Message.install(onHandsetRegister, "user.register", 80, 'driver', 'ybts');
   Message.install(onHandsetUnregister, "user.unregister", 80);
   Message.install(onPhyinfo, "phyinfo", 80);
   Message.install(onRouteSMS, "call.route", 80, 'route_type', 'msg'); // onRouteSMS in sms.js
   Message.install(onSMS, "msg.execute", 80, "callto", droneRootImsi);
   Message.install(onSMSFailed, "sms.failed", 100);
   Message.install(onEngineStatus, "engine.status", 75);
   Message.install(onYBTSStatus, "ybts.status", 75);

   Engine.setInterval(onIntervalMaster, intervals.master);

   // setup periodic actions
   if (onInterval) installPeriodicAction(onInterval, intervals.onInterval, 'client');
   installPeriodicAction(heartbeat, intervals.heartbeat, 'heartbeat');
   installPeriodicAction(expireSubscribers, intervals.subscriberExpire, 'expireSubscribers');
   if (config.poll_phyinfo) installPeriodicAction(sendSilentSMSs, intervals.phyinfoPolling, 'pollPhyinfo');
   installPeriodicAction(sendNextSMS, intervals.sendSMS, 'sendSMS');
   installPeriodicAction(checkOCPQueue, intervals.checkOCPQueue, 'checkOCPQueue');

   // Ready!
   Engine.debug(Engine.DebugInfo, "Search and Rescue Cell Site is UP! Let's go save some lives.");
}

var periodicActions = [];

function installPeriodicAction(action, delay, name) {
   if (delay <= 0) {
      Engine.alarm(4, "A Periodic Action must have a positive delay (action=" + name + ")");
      return;
   } else if (delay < 100) {
      Engine.debug(Engine.DebugInfo, "WARNING: periodic actions are untested with delays less than 100");
   }

   var when = Date.now() + delay;
   var periodicAction = {
      'action': action,
      'delay': delay,
      'name': name,
      'next': when
   };
   periodicActions.push(periodicAction);

   Message.install(action, "idle.execute", 80, 'module', name);
}

/* ############### Event Handlers and Hooks ############### */

var onInterval, onPhoneDetected, onPhoneLost, onSendSMS, onSMSDetected, onSignalReceived;

function onEngineStatus(msg) {
   log("engine.status detected. Message contents:");
   for (var key in msg) {
      log(" - " + key + ": " + msg[key]);
   }
}

function onYBTSStatus(msg) {
   var data = {'type': msg.type};

   switch (msg.type) {
      case "stopnotification":
         data.restarting = msg.restarting;
         data.reason = msg.reason;
         if (!msg.restarting) {
            radioStopped = true;
            radioStoppedReason = msg.reason;
         }
         break;
      case "timeout":
         data['for'] = msg['for'];
         break;
      default:
         break;
   }

   writeToOCP(data, "ybts");
}

function onHandsetRegister(msg) {
   
   // Make sure the subscriber gets registered properly
   var ret = registerSubscriber(msg.imsi, msg.tmsi);
   if (ret !== REGISTERED && ret !== DUPLICATE) {
      if (ret === NO_IMSI) {
         Engine.debug(Engine.DebugInfo, "Asking for new IMSI");
         msg.askimsi = true;
         msg.askimei = true;
      }
      return false;
   }

   // update the yate msg
   var subscriber = getSubscriber(msg.imsi, msg.tmsi);
   msg.imsi = subscriber.imsi;
   msg.tmsi = subscriber.tmsi;
   msg.msisdn = subscriber.msisdn;

   // greet the user on first register
   if (ret === REGISTERED) {
      var next_try = Date.now() / 1000 + 2;
      var options = {'tries': 3, 'next_try': next_try};

      if (config.testing) {
         var msgText = "Hello " + subscriber.imsi + " (" + subscriber.tmsi + "), Your phone # is : '" 
            + subscriber.msisdn + "'";
         var detailedHello = newSMS(subscriber, null, msgText, options);
         pendingSMSs.push(detailedHello);
      } else {
         var hello = newSMS(subscriber, null, helloText, options);
         pendingSMSs.push(hello);
      }
   }

   // ===================================================================================== //
   // No need to authenticate! This is the weakness in GSM that makes this strategy viable! //
   // ===================================================================================== //

   if (onPhoneDetected) onPhoneDetected(subscriber);
   return true;
}

function onHandsetUnregister(msg) {
   var sub = unregisterSubscriber(msg.imsi, msg.tmsi);
   if (sub && onPhoneLost)
      onPhoneLost(sub);
}

function onPhyinfo(msg) {

   // check if a valid subscriber
   var subscriber = getSubscriber(msg.IMSI, msg.TMSI);
   if (subscriber === null) {
      Engine.debug(Engine.DebugInfo, "Unknown subscriber with IMSI: " + msg.IMSI + " and TMSI " + msg.TMSI);
      return true;
   }

   // call the searchandrescue C++ module
   var phyinfo = {
      'IMSI': subscriber.imsi,
      'TMSI': subscriber.tmsi,
      'TA': msg.TA,
      'TE': msg.TE,
      'UpRSSI': msg.UpRSSI,
      'DnRSSIdBm': msg.DnRSSIdBm,
      'time': msg.time
   };
   writeToOCP(phyinfo, 'phy');

   // update the subscriber's phyinfo
   subscriber.lastPhyinfo = phyinfo;

   if (onSignalReceived) 
      onSignalReceived(subscriber, msg.TA, msg.TE, msg.UpRSSI, msg.TxPwr, msg.DnRSSIdBm, msg.time);

   return true;
}

function onSMS(msg) {
   Engine.debug(Engine.DebugInfo, "ATTEMPTING to send SMS: \"" + msg.text + "\"");

   // make sure we have the requisite info
   if (!msg.caller || !msg.called || (!msg.imsi && !msg.tmsi)) {
      Engine.debug(Engine.DebugInfo, "FAILED: protocol error");
      msg.error = "111";
      return false;
   }

   // get the 'from' subscriber
   var from = getSubscriber(msg.imsi, msg.tmsi);
   if (!from || !from.msisdn) {
      Engine.debug(Engine.DebugInfo, 
         "FAILED: Unknown 'from' subscriber or incomplete subscriber registration.");
      msg.error = "28";
      return false;
   }

   // just in case either one is missing
   msg.imsi = from.imsi;
   msg.tmsi = from.tmsi;

   // get the 'to' subscriber
   var dest = msg['sms.called'];
   var to = getSubscriberByMSISDN(dest);
   if (!to || !to.imsi) {
      
      // special handling to route to OCP
      if (dest == droneRootMsisdn) {
         Engine.debug(Engine.DebugInfo, "SUCCESSFUL sending SMS to OCP");

         var simpleSub = simpleSubscriber(from);
         var data = {
            'subscriber': simpleSub,
            'msg': msg.text
         };
         writeToOCP(data, 'sms');

         if (onSMSDetected) onSMSDetected({'imsi': from.imsi, 'dest_imsi': droneRootImsi, 'msg': msg.text});
         return true;
      }

      Engine.debug(Engine.DebugInfo, "FAILED: Unknown 'to' subscriber. (MSISDN=" + dest + ")");
      msg.error = "1";
      return false;
   }

   // build and send the sms
   var next_try = Date.now() / 1000;
   var options = {'tries': 3, 'next_try': next_try};
   var sms = newSMS(to, from, msg.text, options);
   pendingSMSs.push(sms);

   // finished!
   Engine.debug(Engine.DebugInfo, "SUCCESSFUL sending SMS");
   if (onSMSDetected) onSMSDetected({'imsi': from.imsi, 'dest_imsi': to.imsi, 'msg': msg.text});

   return true;
}

function onSMSFailed(msg) {
   var data = {
      'to_imsi': msg.to_imsi,
      'error': msg.error,
      'reason': msg.reason,
      'silent': msg.silent
   };

   if (msg.silent) {
      data.silentSMSCount = msg.silentSMSCount;
   } else {
      data.from_imsi = msg.from_imsi;
      data.text = msg.text;
   }

   writeToOCP(data, "sms_failed");
}

// TODO: include the current PLMN here 
function heartbeat() {
   var subs = activeSubscribers.map(function(sub) { 
      return simpleSubscriber(sub);
   });

   var radio = {
      'stopped': radioStopped,
      'stoppedReason': radioStoppedReason
   };

   var started = Engine.started();
   var info = {
      'ybtsTimersState': ybtsTimersState,
      'engineStarted': started,
      'radio': radio,
      'subscribers': subs,
      'pendingSMSs': pendingSMSs.length
   };
   writeToOCP(info, 'status');   
}

function onIntervalMaster() {
   var now = Date.now();

   for (var i = 0; i < periodicActions.length; ++i) {
      var periodicAction = periodicActions[i];

      if (periodicAction.next && periodicAction.next < now) {
         var m = new Message("idle.execute");
         m.module = periodicAction.name;
         m.enqueue();

         periodicAction.next = now + periodicAction.delay;
      }
   }
}

function checkOCPQueue() {
   var msg = sar.readFromOCP();
   if (!msg) return false;
   
   var msgObj = JSON.parse(msg);
   if (!msgObj) return false;

   if (msgObj.type === "plmn") {
      return handleOCP_plmn(msgObj.data);
   } else if (msgObj.type === "sms") {
      return handleOCP_sms(msgObj.data);
   }
   return false;
}

function handleOCP_plmn(data) {
   Engine.debug(Engine.DebugInfo, "Changing PLMN. MCC='" + data.MCC + "', MNC='" + data.MNC + "'");
   if (!data.MCC || !data.MNC) {
      Engine.debug(Engine.DebugInfo, "Can't change PLMN without both MCC and MNC.");
      return false;
   }

   sar.changePLMN(data.MCC, data.MNC);
   Engine.debug(Engine.DebugInfo, "PLMN changed successfully");
   return true;
}

function handleOCP_sms(data) {
   Engine.debug(Engine.DebugInfo, "Sending SMS to subscriber from OCP. imsi='" 
      + data.imsi + "' tmsi='" + data.tmsi + "' msisdn='" + data.msisdn + "'");
   var sub = null;
   if (data.imsi || data.tmsi)
      sub = getSubscriber(data.imsi, data.tmsi);
   else if (data.msisdn)
      sub = getSubscriberByMSISDN(data.msisdn);

   if (sub === null) {
      Engine.debug(Engine.DebugInfo, "Could not find subscriber... please provide imsi, tmsi, or msisdn");
      return false;
   } else {
      log("Got Subscriber");
      for (var key in sub)
         log("  - " + key + ": " + sub[key]);
   }

   var options = {
      'tries': 3
   };
   if (data.tries && data.tries > 0)
      options.tries = data.tries;

   var sms = newSMS(sub, null, data.msg, options);
   pendingSMSs.push(sms);

   Engine.debug(Engine.DebugInfo, "SMS enqueued successfully");
   return true;
}

