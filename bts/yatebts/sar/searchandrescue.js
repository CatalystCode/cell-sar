/**
 * Search and Rescue
 * Saving Lives with Mobile Cellular Sensors
 * Microsoft, Summer 2017
 *
 * Project Team:
 * Anthony Turner, Cellular Sensor Design
 * Stefan Gordon, Drone Integration
 * Cameron Taylor, DevOps and Scripting
**/

#require "sar_lib.js"

function onInterval() {

   // TODO: Code here is run on an interval.
   // Engine.debug(Engine.DebugInfo, "EXECUTING searchandrescue.onInterval");

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

function onSMSReceived(sms) {

   // TODO: Code here is run when an SMS is received from a phone
   Engine.debug(Engine.DebugInfo, "EXECUTING searchandrescue.onSMSReceived "
      + "{imsi: " + sms.imsi + ", msg: " + sms.msg + "}");

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

