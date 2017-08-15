/** searchandrescue.js: javascript automation file for Search and Rescue functionality.
  *    
  * Cellular Search and Rescue - Cellular Sensor BTS
  *   Copyright (C) 2017 Microsoft
  * 
  * This file is part of cell-sar
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

#require "sar_lib.js"

function onInterval() {

   // TODO: Code here is run once every second.
   Engine.debug(Engine.DebugInfo, "EXECUTING searchandrescue.onInterval");

}

function onPhoneDetected(subscriber) {
   
   // TODO: Code here is run when a phone's signal is initially detected
   Engine.debug(Engine.DebugInfo, "EXECUTING searchandrescue.onPhoneDetected "
      + "{IMSI: " + subscriber.imsi + ", TMSI: " + subscriber.tmsi + "}");

   // sendSMSMessage(subscriber.imsi, "Search and Rescue is currently running a mission. If you are lost and in need of assistance, please keep your phone on and stand by for rescue crews.")
}

function onPhoneLost(subscriber) {

   // TODO: Code here is run when a phone's signal is lost 
   Engine.debug(Engine.DebugInfo, "EXECUTING searchandrescue.onPhoneLost "
      + "{IMSI: " + subscriber.imsi + ", TMSI: " + subscriber.tmsi + "}");

}

function onSendSMS(sms) {

   // TODO: Code here is run when an SMS is sent to a phone
   Engine.debug(Engine.DebugInfo, "EXECUTING searchandrescue.onSendSMS "
      + "{dest_imsi: " + sms.dest_imsi + ", msg: " + sms.msg + "}");

}

function onSMSDetected(sms) {

   // TODO: Code here is run when an SMS is passed through the network
   Engine.debug(Engine.DebugInfo, "EXECUTING searchandrescue.onSMSDetected "
      + "to '" + sms.dest_imsi + "', from '" + sms.imsi + "', text: \"" + sms.msg + "\"");

}

function onSignalReceived(subscriber, TA, TE, UpRSSI, TxPwr, DnRSSIdBm, time) {

   // TODO: Code here is run when a phone's signal strength info is detected.
   Engine.debug(Engine.DebugInfo, "EXECUTING searchandrescue.onSignalReceived " 
      + "{IMSI: " + subscriber['imsi'] + ", TA: " + TA + ", TE: " + TE + ", UpRSSI: " 
      + UpRSSI + ", TxPwr: " + TxPwr + ", DnRSSIdBm: " + DnRSSIdBm + ", time: " 
      + time + "}");

}

// Initialize the SAR engine
Engine.debug(Engine.DebugInfo, "initializing SAR");
initializeSAR();

