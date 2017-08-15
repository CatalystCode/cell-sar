<?php 
/**
 * check_validity_fields_enb.php
 *
 * This file is part of the Yate-LMI Project http://www.yatebts.com
 *
 * Copyright (C) 2016 Null Team
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

/* validate CellIdentity from [basic] section */
function validate_cell_identity($field_name, $field_value)
{
	if (strlen($field_value)!=7 || !ctype_xdigit($field_value))
		return array(false, "Field '" . $field_name . "' is not valid: " . $field_value .". Must be 7 hex digits.");
	
	return array(true);
}

/* validate EARFCN */
function validate_earfcn_band($field_name, $field_value, $restricted_value)
{
        // the first value is the band and the array is the range of the permitted EARFCNs for each band
	$permitted_values = array( 
		"1"  => array(0, 599), "2" => array(600, 1199),
        "3"  => array(1200, 1949), "4" => array(1950, 2399),
		"5"  => array(2400, 2649), "6" => array(2650, 2749), 
		"7"  => array(2750, 3449), "8" => array(3450, 3799), 
		"9"  => array(3780, 4149), "10" => array(4150, 4749),
		"11" => array(4750, 4949), "12" => array(4950, 5179),
		"13" => array(5180, 5279), "14" => array(5280, 5379),
		"17" => array(5380, 5849), "18" => array(5850, 5999),
		"19" => array(6000, 6149), "20" => array(6150, 6449),
                "21" => array(6450, 6599), "22" => array(6600, 7399),
                "23" => array(7400, 7699), "24" => array(7700, 8039),
                "25" => array(8040, 8689), "26" => array(8690, 9039),
                "27" => array(9040, 9209), "28" => array(9210, 9659),
		"29" => array(9660, 9769), "30" => array(9770, 9869),
                "31" => array(9870, 9919), "32" => array(9920, 10359),
                "33" => array(36000, 36199), "34" => array(36200, 36349),
		"35" => array(36350, 36949), "36" => array(36950, 37549),
                "37" => array(37550, 37749), "38" => array(37750, 38249),
                "39" => array(38250, 38649), "40" => array(38650, 39649),
		"41" => array(39650, 41589), "42" => array(41590, 43589),
                "43" => array(43590, 45589), "44" => array(45590, 46589)
	);
	
	if (!$permitted_values[$restricted_value]) 
		return array(false, "The field Band value: '". $restricted_value . "' is not a permitted");
	
	$field_value = (int)$field_value;
        $earfcn = $permitted_values[$restricted_value];
	$min = $earfcn[0];
	$max = $earfcn[1];
	if ($field_value<$min || $field_value>$max) 
		return array(false, "Field '". $field_name ."' is not in the right range for the Band chosen.");

	return array(true);
}

/**
 * Validate Broadcast format IP:port, IP:port, ....
 */
function check_broadcast($field_name, $field_value)
{
	$expl = explode(",", $field_value);
	if (!count($expl)) {
		return array(false, "Invalid format for '" . $field_name . "'. The IPs must be splitted by comma: ','.");
	}
	foreach ($expl as $k=>$bc) {
		$val = explode(":", $bc);
		$ip = trim($val[0]);
		$port = trim($val[1]);
		$valid_ip = check_valid_ipaddress($field_name,$ip);
		if (!$valid_ip)
			return $valid_ip;

		if (!$port) {
			return array(false, "Field '" . $field_name . "' doesn't contain a port.");
		}

		if (ctype_digit($port) || strlen($port)>5) {
			return array(false, "Field '" . $field_name . "' doesn't contain a valid port:  '". $port);
		}
	}

	return array(true);
}

/* validate AuxRoutingTable field */
function check_auxrouting_table($field_name, $field_value)
{
	if (!ctype_digit($field_value)) { 
		$valid = check_field_validity($field_name, $field_value, null, null, "^[a-zA-Z0-9_-]+)$");
		if (!$valid[0])
			return $valid;
	} else {
		$valid = check_field_validity($field_name, $field_value, 0, 255);
		if (!$valid[0])
			return $valid;
	}
	return array(true);
} 

/* UplinkRbs + UplinkRbStartIndex must be less than the total number of RBs specified in the system bandwidth configuration */
function check_uplink_rbs($field_name, $field_value, $ulrbsstartind, $rbs)
{
	$rbs_value = getparam($rbs);
	$ulrbsstartind_value = getparam($ulrbsstartind);
	if ($field_value + $ulrbsstartind_value > $rbs_value) {
		return array(false, "UplinkRbs + UplinkRbStartIndex must be less than the total number of RBs specified by 'Bandwidth' parameter in 'Radio' section.");
	}
	return array(true);
}

function check_n1PucchAn($field_name, $field_value, $bandwidth)
{
	$valid = check_valid_number($field_name, $field_value);
	if (!$valid[0])
		return $valid;	
	$bandwidth_value = (int) $bandwidth;
	$max_n1PucchAn =  floor (2047 * $bandwidth_value / 110);
	if ($field_value<0 || $field_value>$max_n1PucchAn)
		return array(false, "$field_name should be between 0 and 2047 * Bandwidth / 110 ($max_n1PucchAn)");
	return array(true);
}

function check_valid_enodebid($field_name, $field_value)
{
	if ((strlen($field_value)!=5 && strlen($field_value)!=7) || !ctype_xdigit($field_value)) 
		return array(false, "Field '" . $field_name . "' is not valid: " . $field_value .". Must be 5 or 7 hex digits.");

	return array(true);
}

function validate_gtp_params()
{
	$addr4 = getparam("addr4");
	$addr6 = getparam("addr6");

	if (!valid_param($addr4) && !valid_param($addr6)) {
		return array(false,"Please set 'addr4' or 'addr6' when setting 'GTP'.", array("addr4", "addr6"));
	}

	if (valid_param($addr4)) {
		$res = check_valid_ipaddress("Addr4", $addr4);
		$res[2] = array("addr4");
		return $res;
	}

	if (valid_param($addr6)) {
		$res = check_valid_ipaddress("Addr6", $addr6);
		$res[2] = array("addr6");
		return $res;
	}

	return array(true);
}

function validate_mme_params()
{
	$mme_validations = array(
		"address" => array("callback"=>"check_valid_ipaddress", "type"=>"required"),
		"local"   => array("callback"=>"check_valid_ipaddress", "type"=>"required"),
		"streams" => array("callback"=>"check_valid_integer"),
		"dscp"	  => array("valid_values"=> array("expedited")));

	// see if we have any defined MMEs set
	$index = 0;
	while ($index<=5) {
		$index++;
		if ($index==1)
			$mme_index = "";
		else
			$mme_index = "_" . $index;

		$mme_address = "mme_address" . $mme_index;
		$mme_local = "local" . $mme_index;
		if (!(getparam($mme_address) || getparam($mme_local)))
			break;
		$mme_addresses[] = getparam($mme_address);
		foreach ($mme_validations as $param=>$param_validation) {
			$suffix = "";
			if ($param == "address")
				$suffix = "mme_";
			$name = $suffix. $param . $mme_index;
			$field_name  = ucfirst($param);
			$field_value = getparam($name);

			if (isset($param_validation["type"]) && !valid_param($field_value))
				return array(false, "The '".$name."' can't be empty in 'mme".$index."'.", array($name));

			if (isset($param_validation["callback"]) && $field_value) {
				$res = call_user_func($param_validation["callback"], $field_name, $field_value);
			} elseif (isset($param_validation["valid_values"]) && $field_value) {
				$args = array($field_name, $field_value, $param_validation["valid_values"]);
				$res = call_user_func_array("check_valid_values_for_select_field", $args);
			}

			if (!$res[0]) {
				$res[2] = array($name);
				return $res;
			}
		}
	}

	if (count(array_unique($mme_addresses))<count($mme_addresses)) { 
		$err = "Found duplicated MME Addresses. ";
		$dup = array_count_values($mme_addresses);
		foreach ($dup as $ip=>$how_many) 
			$err .= "The Address: ".$ip. " is duplicated. ";

		return array(false, $err, array("mme_address"));
	}

	return array(true);
}

function check_output_level_validity($field_name, $field_value, $restricted_value)
{
	$valid = check_field_validity($field_name, $field_value, 0, 43);
	if (!$valid[0])
		return $valid;

	if ($field_value > $restricted_value)
		return array(false, "The '".$field_name. "' should not exceed the 'MaximumOutput' parameter from 'RadioHardware' section.");

	return array(true);
}

function check_SpecialRntiCodingEfficiency($field_name, $field_value)
{
	if ($field_value < 0.0625 || $field_value > 4)
		return array(false, "The '".$field_name. "' should be in the range 0.0625 - 4.0");

	return array(true);
}

function check_valid_tac($field_name, $field_value)
{
	if (!ctype_xdigit($field_value))
		return array(false, "Invalid '$field_name' value $field_value. Values should be hex format. Ex: 1A0C");
	if (strlen($field_value)!=4)
		return array(false, "Invalid '$field_name' value $field_value. Value should be 4 digits long. Ex: 1A0C");
	
	return array(true);
}
?>
