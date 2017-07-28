<?php 
/**
 * check_validity_fields_ybts.php
 * This file is part of the Yate-LMI Project http://www.yatebts.com
 *
 * Copyright (C) 2014 Null Team
 *
 * This software is distributed under multiple licenses;
 * see the COPYING file in the main directory for licensing
 * information for this specific distribution.
 *
 * This use of this software may be subject to additional restrictions.
 * See the LEGAL file in the main directory for details.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


/** 
 * Test the validity of [gsm] Radio.C0 param to be the right value for the chosen Radio.Band
 */ 
function check_radio_band($field_name, $field_value, $restricted_value)
{
	$permitted_values = array(
		"850" => array("128","251"),
		"900" => array("0","124","975","1023"), 
		"1800" => array("512","885"),
		"1900" => array("512","810")
	);

	foreach ($permitted_values as $radio_band_val => $radio_c0_val) {
		if ($restricted_value == $radio_band_val) {
			$field_value = explode("-",$field_value);
			if (!isset($field_value[1]))
				$field_value = null;
			else
				$field_value = $field_value[1];
			$int_value = (int)$field_value;
			$min = $radio_c0_val[0];
			$max = $radio_c0_val[1];

			if (!isset($radio_c0_val[2])) {

				if ($int_value<$min || $int_value>$max)
					return array(false, "Field $field_name selected, is not in the right range for the Radio.Band chosen.");
				return array(true);

			} elseif (isset($radio_c0_val[2])) {
				$min2 = $radio_c0_val[2];
				$max2 = $radio_c0_val[3];
				if (($int_value>=$min && $int_value<=$max) || ($int_value>=$min2 && $int_value<=$max2))
					return array(true);
				return array(false, "Field $field_name selected, is not in the right range for the Radio.Band chosen.");
			}

		}
	}

	return array(false, "The given value for $field_name is not a permitted one.");
}

/** 
 * Test if Radio.PowerManager.MinAttenDB is less or equal to Radio.PowerManager.MaxAttenDB.
 *
 * $field_value = getparam("Radio.PowerManager.MinAttenD");
 * $restricted_value = getparam("Radio.PowerManager.MaxAttenDB");
 */
function check_radio_powermanager($field_name, $field_value, $restricted_value)
{
	$valid = check_valid_number($field_name, $field_value);
	if (!$valid[0])
	    return $valid;

	if ((int)$field_value > $restricted_value)
		return array(false, "Radio.PowerManager.MinAttenDB, must be less or equal to Radio.PowerManager.MaxAttenDB");
	
	return array(true);
}

function validate_gsm_time_sync_check($field_name, $field_value)
{
	$valid = check_valid_number($field_name, $field_value);
	if (!$valid[0])
		return $valid;

	if ($field_value!=0 && $field_value<2000 && $field_value>10000)
		return array(false, "Field $field_name is not valid. The value must be 0 or in interval [2000,10000].");

	return array(true);
}

/**
 * VEA ([control]) field value depends on CellSelection.NECI ([gsm_advanced]) 
 * this fields are in different section 
 * if one of the interest params was written in config file 
 * validate the other in form
 * This has to be valid:
 * If VEA is selected, [gsm_advanced] CellSelection.NECI should be set to 1.
 */
function validate_neci_vea($field_name, $field_value)
{
	if ($field_name == "VEA" && ( $field_value == "off" || $field_value == "no" || $field_value == "0")) {
		$neci = getparam("CellSelection_NECI");
		if ($neci=="1")
			return array(true, "Field $field_name from Control Tab doesn't have a recommended value. It has to be checked because CellSelection.NECI param from Radio tab in GSM Advanced subsection was set to 1.");
	} 

	if ($field_name == "CellSelection.NECI" && $field_value != "1") {
		$vea = getparam("VEA");
		if ($vea=='on')
			return array(true, "Field $field_name doesn't have a recommended value. It has to be selected the 'New establisment causes are supported'  because VEA parameter from Control Tab was selected.");
	}

	return array(true);
}

/**
 * Timer.ImplicitDetach : should be at least 240 seconds greater than SGSN.Timer.RAUpdate. 
 * Interval allowed 2000:4000(10).
 *
 * $field_value => is the value of Timer.ImplicitDetach from form
 * $restricted_value => is the value of Timer.RAUpdate from form
 */
function check_timer_implicitdetach($field_name, $field_value, $restricted_value)
{
	$valid = check_valid_number($field_name, $field_value);
	if (!$valid[0]) {
		return $valid;
	}

	//first test if $field_value is in the allowed interval
	if ($field_value<2000 || $field_value>4000 || $field_value%10!=0)
		return array(false, "Field $field_name is not valid. The value must be in interval [2000,4000] and should be a factor of 10.");

	if ($field_value-$restricted_value < 240)
		return array(true, "Field $field_name doesn't have a recommended value. It should be at least 240 seconds greater than Timer.RAUpdate.");

	return array(true);
}

/**
 * [gprs_advanced] param: Timer.RAUpdate
 * Setting to 0 or >12000 deactivates entirely, i.e., sets the timer to effective infinity.
 * Note: to prevent GPRS Routing Area Updates you must set both this and GSM.Timer.T3212 to 0.
 * Interval allowed 0:11160(2). Defaults to 3240.
 */
function check_timer_raupdate($field_name, $field_value)
{
	$valid = check_valid_number($field_name, $field_value);
	if (!$valid[0])
		return $valid;

	if (($field_value<0 || $field_value>11160 || $field_value%2!=0) && $field_value<12000)
		return array(false, "Field $field_name is not valid. The value must be in interval [0,11160] and should be a factor of 2 or greater than 12000.");

	$timerT3212 = getparam("Timer_T3212");
	if ($field_value!=0 && $timerT3212=="0") {
		return array(true, "To prevent GPRS Routing Area Updates it is recommended you set $field_name also to 0, setting field Timer.T3212 from Radio Tab set to 0 is not enough.");
	}

	return array(true);
}

/**
 * [gprs_advanced] param:  Uplink.Persist
 * If non-zero, must be greater than GPRS.Uplink.KeepAlive.
 * This is broadcast in the beacon and it cannot be changed once BTS is started.
 * Allowed interval 0:6000(100).
 */
function check_uplink_persistent($field_name, $field_value, $restricted_value)
{
	$valid = check_valid_number($field_name, $field_value);
	if (!$valid[0])
		return $valid;

	$field_value = (int)$field_value;
	if ($field_value<0 || $field_value>6000 || $field_value%100!=0)
		return array(false, "Field $field_name is not valid. The value must be in interval [0,6000] and should be a factor of 100.");

	if ($field_value != 0 || $field_value != "0") {
		if ($field_value < (int)$restricted_value)
			return array(true, "Field $field_name doesn't have a recommended value. This value must be greater than Uplink.KeepAlive value.");
	}

	return array(true);
}

/**
 * [gprs_advanced] param: Downlink.Persist
 * If non-zero, must be greater than GPRS.Downlink.KeepAlive.
 */
function check_downlink_persistent($field_name, $field_value, $restricted_value)
{
	$valid = check_valid_number($field_name, $field_value);
	if (!$valid[0])
		return $valid;

	$field_value = (int) $field_value;
	$restricted_value = (int)$restricted_value;

	if ($field_value < 0 || $field_value > 10000)
		return array(false, "Field $field_name is not valid. It has to be smaller than 10000.");
	if ($field_value != 0) {
		if ($field_value < $restricted_value)
			return array(true, "Field $field_name doesn't have a recommended value. This value must be greater than Downlink.KeepAlive value.");
	}

	return array(true);
}

/**
 * Test for [gprs_advanced] param: ChannelCodingControl.RSS.
 * This value should normally be GSM.Radio.RSSITarget + 10 dB.
 * Interval allowed -65:-15.
 */
function check_channelcodingcontrol_rssi($field_name, $field_value)
{
	$valid = check_valid_number($field_name, $field_value);
	if (!$valid[0])
		return $valid;

	$field_value = (int) $field_value;

	if ($field_value<-65 || $field_value>-15)
		return array(false, "Field $field_name is not valid. The value must be in interval [-65,-15].");

	$radio_RSSITarget = (int)getparam("Radio_RSSITarget");

	if ($radio_RSSITarget+10 != $field_value)
		return array(true, "Field $field_name doesn't have a recommended value. This value should normally be Radio.RSSITarget + 10 dB, value added from tab Radio, from GSM Advanced that has the value: $radio_RSSITarget.");

	return array(true);
}

/**
 * validate field Radio.RSSITarget to be ChannelCodingControl.RSS-10 dB.
 */ 
function check_radio_rssitarget($field_name, $field_value)
{
	$valid = check_valid_number($field_name, $field_value);
	if (!$valid[0])
		return $valid;

	$field_value = (int) $field_value;

	if ($field_value<-75 || $field_value>-25)
		return array(false, "Field $field_name is not valid. The value must be in interval [-75,-25].");

	$chancontrol_rss = (int) getparam("ChannelCodingControl_RSSI");
	if ($chancontrol_rss -10 < $field_value)
		return array(true, "Field $field_name doesn't have a recommended value. This value should be the value of ChannelCodingControl.RSSI -10dB, parameter already added from submenu GPRS Advanced with the value: $chancontrol_rss.");

	return array(true);
}

/**
 * validate t3260
 */
function check_t3260($field_name, $field_value)
{
	$valid = check_valid_number($field_name, $field_value);
	if (!$valid[0])
		return $valid;

	$field_value = (int) $field_value;

	if ($field_value<5000 || $field_value>3600000)
		return array(false, "Field $field_name is not valid. Interval allowed: 5000..3600000.");

	return array(true);
}

/**
 * Validate reg_sip from [roaming] section in ybts.conf: ip:port or just ip
 */
function valid_reg_sip($field_name, $field_value)
{
	if (!strlen($field_value))
		return array(true);

	$expl = explode(":",$field_value);
	$ip = $expl[0];
	if (!filter_var($ip, FILTER_VALIDATE_IP))
		return array(false, "Field $field_name '$field_value' doesn't contain a valid IP address.");

	$port = (isset($expl[1])) ? $expl[1] : null;
	if ($port && !filter_var($port,FILTER_VALIDATE_INT))
		return array(false, "Field $field_name '$field_value' doesn't contain a valid port.");

	return array(true);
}

/**
 * Validate nodes_sip from [roaming] section in ybts.conf
 */
function valid_nodes_sip($field_name, $field_value)
{
	if (!strlen($field_value))
		return array(true);

	$field_value = htmlspecialchars_decode($field_value);
	$decoded = json_decode($field_value);

	if ($decoded===null)
		return array(false, "Field $field_name '$field_value' isn't a valid JSON object.");

	return array(true);
}

function check_rach_ac($field_name, $field_value)
{
	if (!strlen($field_value))
		return array(true);

	if (substr($field_value,0,2)!="0x") {
		$res = check_field_validity($field_name, $field_value, 0, 65535);
		if (!$res[0])
			return $res;
		$emergency_disabled = $field_value & 0x0400;
	} else {
		$hex = substr($field_value,2);
		if (!ctype_xdigit($hex))
			return array(false, "Invalid hex value '$field_value' for $field_name.");
		if (strcasecmp($hex,"ffff")>0)
			return array(false, "Value '$field_value' for $field_name exceeds limit 0xffff.");
		$hex = base_convert($hex,16,10);
		$emergency_disabled = $hex & 0x0400;
	}
	if (!$emergency_disabled) {
		/*if (is_file($yate_conf_dir."/subscribers.conf")) {
			$conf = new ConfFile($yate_conf_dir."/subscribers.conf");
			if (!isset($conf->sections["general"]["gw_sos"]) || !strlen($conf->sections["general"]["gw_sos"]))
				return array(false, "You enabled emergency calls in RACH.CA. <font class='error'>DON'T MODIFY THIS UNLESS YOU ARE A REAL OPERATOR</font>. You might catch real emergency calls than won't ever be answered. If you really want to enable them, first set 'Gateway SOS' in Subscribers>Country code and SMSC.");
		}

		warning_note("You enabled emergency calls in RACH.CA. <font class='error'>DON'T MODIFY THIS UNLESS YOU ARE A REAL OPERATOR</font>. You might catch real emergency calls than won't ever be answered.");*/
	}

	return array(true);
}

/**
 * Validate sos_sip from [roaming] section 
 * String: host:port where SIP emergency calls are sent.
 * If not set any emergency calls will be delivered to reg_sip or nodes_sip.
 * Ex: sos_sip=192.168.1.215:5059
 */
function valid_sos_sip($field_name, $field_value)
{
	if (!strlen($field_value))
		return array(true);

	$expl = explode(":",$field_value);
	$ip = $expl[0];
	if (!filter_var($ip, FILTER_VALIDATE_IP))
		return array(false, "Field $field_name '$field_value' doesn't contain a valid IP address.");

	$port = (isset($expl[1])) ? $expl[1] : null;

	if (!$port || !filter_var($port,FILTER_VALIDATE_INT))
		return array(false, "Field $field_name '$field_value' doesn't contain a valid port.");

	return array(true);
}

function validate_roaming_params()
{
	$required = array("nnsf_bits", "gstn_location");

	foreach ($required as $field_name)
		if (!getparam($field_name))
			return array(false, "Field $field_name is required in roaming mode.", array($field_name));

	$reg_sip = getparam("reg_sip");
	$nodes_sip = getparam("nodes_sip");
	if (!$reg_sip && !$nodes_sip)
		return array(false, "You need to set 'Reg sip' or 'Nodes sip' in roaming mode.", array("reg_sip", "nodes_sip"));

	$nnsf_bits = getparam("nnsf_bits");
	$expires = getparam("expires");
	if (valid_param($expires) && !is_valid_number($expires))
		return array(false, "Field 'Expires' field should be numeric.", array("expires"));
	if (valid_param($nnsf_bits) && (!is_valid_number($nnsf_bits) || $nnsf_bits<0))
		return array(false, "Field 'NNSF bits' should be a positive int.", array("nnsf_bits"));

	return array(true);
}

function validate_piece_roaming()
{
	// verify fields from roaming as well, if set
	$nnsf_bits = getparam("nnsf_bits");
	$expires = getparam("expires");
	if (valid_param($expires) && !is_valid_number($expires))
		return array(false, "Field 'Expires' field should be numeric.", array("expires"));
	if (valid_param($nnsf_bits) && (!is_valid_number($nnsf_bits) || $nnsf_bits<0))
		return array(false, "Field 'NNSF bits' should be a positive int.", array("nnsf_bits"));

	$reg_sip = getparam("reg_sip");
	$nodes_sip = getparam("nodes_sip");
	if (!$reg_sip && !$nodes_sip)
		return array(false, "You need to set 'Reg sip' or 'Nodes sip' in dataroam mode.", array("reg_sip", "nodes_sip"));

	return array(true);
}

function validate_dataroam_params()
{
	$nnsf_bits = getparam("gprs_nnsf_bits");
	if (valid_param($nnsf_bits) && (!is_valid_number($nnsf_bits) || $nnsf_bits<0))
		return array(false, "NNSF bits must be a positive int, if set.", array("gprs_nnsf_bits"));

	$map = getparam("network_map");
	if (!strlen($map))
		return array(false, "You need to set at least one 'Explicitly map network nodes to IP addresses' in dataroam mode.", array("network_map"));

	$map = explode("\n", $map);
	foreach ($map as $map_entry) {
		$entry = explode("=",$map_entry);
		if (count($entry)!=2)
			return array(false, "Invalid format for 'Network map'.", array("network_map"));
		if (Numerify($entry[0])=="NULL")
			return array(false, "Invalid value '".$entry[0]."'. Should be numeric.", array("network_map"));
		if (filter_var(trim($entry[1]),FILTER_VALIDATE_IP)===false)
			return array(false, "Invalid value '".$entry[1]."'. Should be a valid IP address.", array("network_map"));
	}

	return array(true);
}

?>
