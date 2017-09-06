/** subscribers.js: manages subscriber logic including registration, 
  * authorization, unregistration, and inspection.
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

#require "sar_config.js"


var activeSubscribers = [];

var REGISTERED = 0;
var NO_IMSI_OR_TMSI = 1;
var NO_IMSI = 2;
var NOT_ALLOWED = 3;
var DUPLICATE = 4;

function simpleSubscriber(sub) {
   var imsi = '' + sub.imsi;
   var tmsi = '' + sub.tmsi;
   var msisdn = '' + sub.msisdn;
   var result = {
      'imsi': imsi,
      'tmsi': tmsi,
      'msisdn': msisdn
   };
   return result;
}

function registerSubscriber(imsi, tmsi) {
   // TODO: trigger authentication

   Engine.debug(Engine.DebugInfo, "ATTEMPTING handset registration for IMSI: " 
      + imsi + ", TMSI: " + tmsi);

   // make sure we have at least one
   if (!imsi && !tmsi) {
      Engine.debug(Engine.DebugInfo, "FAILED handset registration");
      return NO_IMSI_OR_TMSI;
   }

   // we at least need the imsi, so ask for it
   if (!msg.imsi) {
      Engine.debug(Engine.DebugInfo, "FAILED handset registration, no IMSI");
      return NO_IMSI; 
   }

   // check if the subscriber already exists
   var subscriber = getSubscriber(imsi); // purposely ignoring tmsi, if its a duplicate, we'll reset it
   if (subscriber) {
      Engine.debug(Engine.DebugInfo, "DUPLICATE handset registration.");
      return DUPLICATE;
   }

   // the specific imsi must be allowed
   if (!imsiPermitted(imsi)) {
      Engine.debug(Engine.DebugInfo, "IMSI not allowed");
      return NOT_ALLOWED;
   }

   // make sure they have a unique TMSI
   if (!tmsi || !tmsiAvailable(tmsi))
      tmsi = allocateTmsi();

   // add the subscriber!
   var msisdn = allocatePhoneNumber(imsi);
   var expiry = Date.now() / 1000 + 3600 * 24; // 1 day
   var loc = "ybts/TMSI" + tmsi;

   var subscriber = {
      'imsi': imsi,
      'tmsi': tmsi,
      'msisdn': msisdn,
      'expires': expiry,
      'location': loc
   };
   activeSubscribers.push(subscriber);

   Engine.debug(Engine.DebugInfo, "SUCCESSFUL handset registration for IMSI: " + imsi);
   return REGISTERED;
}

function unregisterSubscriber(imsi, tmsi) {
   Engine.debug(Engine.DebugInfo, "ATTEMPTING to unregister IMSI: " 
      + imsi + ", TMSI: " + tmsi);

   // make sure it is a registered subscriber
   var subscriber = getSubscriber(imsi, tmsi);
   if (!subscriber) {
      Engine.debug(Engine.DebugInfo, "NOT FOUND handset during unregistration");
      return null;
   }

   var idx = activeSubscribers.indexOf(subscriber);
   activeSubscribers.splice(idx, 1);
   
   Engine.debug(Engine.DebugInfo, "SUCCESSFUL unregistration");
   return subscriber;
}

function imsiPermitted(imsi) {
   return (config.testing && allowedImsis[imsi]) 
      || (!config.testing && !forbiddenImsis[imsi]);
}

function getSubscriberByMSISDN(msisdn) {
   for (var i = 0; i < activeSubscribers.length; ++i) {
      if (activeSubscribers[i].msisdn == msisdn)
         return activeSubscribers[i];
   }
   return null;
}

function getSubscriber(imsi, tmsi) {
   for (var i = 0; i < activeSubscribers.length; ++i) {
      if ((!imsi && activeSubscribers[i]["tmsi"] == tmsi) || // Given TMSI but no IMSI
            (!tmsi && activeSubscribers[i]["imsi"] == imsi) || // Given IMSI but no TMSI
            activeSubscribers[i]["imsi"] == imsi) // Given both, use IMSI
      {
         return activeSubscribers[i];
      }
   }

   return null;
}

function allocatePhoneNumber(imsi) {
   if (!imsi)
      var val = country_code + generatePhoneNumber();
   else
      // create number based on IMSI. Try to always generate same number for same IMSI
      var val = country_code + imsi.substr(-7);

   while (!numberAvailable(val))
      val = country_code + generatePhoneNumber();

   return val;
}

function allocateTmsi() {
   var tmsi;

   while (true) {
      if (tmsiAvailable(tmsi = createTmsi(nnsf_bits, nnsf_local_mask, nnsf_node_shift)))
         break;
   }

   return tmsi;
}

function tmsiAvailable(tmsi) {
   for (var i = 0; i < activeSubscribers.length; ++i) {
      if (activeSubscribers[i]["tmsi"] === tmsi)
         return false;
   }

   return true;
}

function numberAvailable(number) {
   for (var i = 0; i < activeSubscribers.length; ++i) {
      if (activeSubscribers[i]["msisdn"] === number)
         return false;
   }
   return true;
}

function expireSubscriptions(now) {
   // see if we should expire TMSIs
   for (var i = 0; i < activeSubscribers.length; i++)
   {
      if (now >= activeSubscribers[i]["expires"])
      {
         Engine.debug(Engine.DebugInfo, "EXPIRING handset with IMSI '" + activeSubscribers[i]["imsi"] +
            "'. (Expired " + activeSubscribers[i]["expires"] + ")");
         activeSubscribers.splice(i, 1);
         i--;
      }
   }
}

function expireSubscribers() {
   var now = Date.now() / 1000;
   if (now % 100 < 5) {
      expireSubscriptions(now);
   }
}
