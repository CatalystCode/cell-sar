/** sar_config.js: in-memory configuration for the javascript sar engine
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

#require "utils.js"

var config = {
   'testing': true,
   'loud_sms': false,
   'poll_phyinfo': true
};

// intervals (in ms) between different runs
var intervals = {
   // 'master': null, // calculated when loadConfiguration is run to be the gcd of all other timers
   'heartbeat': 10000,
   'phyinfoPolling': 1000,
   'subscriberExpire': 60000,
   'onInterval': 10000,
   'sendSMS': 500
};

var helloText = "phone detected";  // sent to a phone when they join the network

var allowedImsis = {};     // key: IMSI, value: true if allowed
var forbiddenImsis = {};   // key: IMSI, value: true if forbidden

var country_code = 1;
var nnsf_bits = 8;
var nnsf_node = 123;

var droneRootImsi = "sar_imsi";
var droneRootMsisdn = "1234";

var nnsf_node_mask, nnsf_node_shift, nnsf_local_mask;

function loadConfiguration() {
   var conf = new ConfigFile(Engine.configFile('sar'), true);
   if (!conf) return;

   var imsiPattern = new RegExp(/^[0-9]{15}$/);
   var anyAllowedImsis = false;

   for (var section in conf.sections()) {
      if (section === 'general') {
         var general = conf.getSection(section);

         config.testing = general.getBoolValue('testing', config.testing);
         config.loud_sms = general.getBoolValue('loud_sms', config.loud_sms);
         config.poll_phyinfo = general.getBoolValue('poll_phyinfo', config.poll_phyinfo);

         country_code = general.getIntValue('country_code', country_code);

         droneRootMsisdn = general.getValue('sensor_phone_number', droneRootMsisdn);

         if (droneRootMsisdn === '911') {
            Engine.alarm(4, "911 is not a permitted phone number for the sensor due to restrictions on use by law enforcement");
         }

         helloText = general.getValue('helloText', helloText);

      } else if (section === 'intervals') {
         var times = [];
         var intervalSection = conf.getSection(section);

         for (var timer in intervals) {
            intervals[timer] = intervalSection.getIntValue(timer, intervals[timer]);
            times.push(intervals[timer]);
         }

         intervals['master'] = gcdNums(times);

      } else if (imsiPattern.test(section)) {
         var imsi = section;
         var imsiConfig = conf.getSection(imsi);

         allowedImsis[imsi] = imsiConfig.getBoolValue('allowed');
         forbiddenImsis[imsi] = imsiConfig.getBoolValue('forbidden');

         anyAllowedImsis = anyAllowedImsis || allowedImsis[imsi];
      }
   }

   if (config.testing && !anyAllowedImsis) {
      Engine.alarm(4, "Testing without any Allowed IMSIs");
   }
}

function initializeNNSF() {
   if (nnsf_bits > 0 && nnsf_bits <= 10) {
      nnsf_node &= 0x03ff >> (10 - nnsf_bits);
      nnsf_node_mask = (0xffc000 << (10 - nnsf_bits)) & 0xffc000;
      nnsf_node_shift = nnsf_node << (24 - nnsf_bits);
      nnsf_local_mask = 0xffffff >> nnsf_bits;
   } else {
      nnsf_bits = 0;
      nnsf_node = 0;
      nnsf_node_mask = 0;
      nnsf_node_shift = 0;
      nnsf_local_mask = 0xffffff;
   }
}


