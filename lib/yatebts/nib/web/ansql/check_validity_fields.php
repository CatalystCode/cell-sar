<?php
/**
 * check_validity_fields.php
 * This file is part of the Yate-SDR Project http://www.yatebts.com
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

/* test the given param to not be empty string or null */
function valid_param($param)
{
	return ($param !== NULL && $param !== "");
}

/* test the given param to be a valid number */
function is_valid_number($field_value)
{
	//the allowed values are ^-?[1-9][0-9]*|0$
	if (!is_numeric($field_value))
		return false;
	if (strcmp($field_value,(string)(int)$field_value)!== 0)
		return false;
	return true;
}

function check_valid_number($field_name, $field_value)
{
	if (!is_valid_number($field_value))
		return array(false, "Field '".$field_name."' is not a valid number: $field_value.");
	return array(true);
}

function check_valid_integer($field_name, $field_value)
{
	$valid = check_valid_number($field_name, $field_value);
	if (!$valid[0])
		return $valid;

	$field_value = (int) $field_value;
	if (filter_var($field_value, FILTER_VALIDATE_INT) === false)
		return array(false, "Field '".$field_name."' is not a valid integer: $field_value.");	
	return array(true);
}

function check_valid_float($field_name, $field_value)
{
	if (filter_var($field_value, FILTER_VALIDATE_FLOAT) === false)
		return array(false, "Field '".$field_name."' is not a valid float: $field_value.");	
	return array(true);
}

/**
 * validate a field value to be in a given array 
 * [Used for SELECT FIELDS]
 */
function check_valid_values_for_select_field($field_name, $field_value, $select_array)
{
	foreach ($select_array as $key => $values) {
		if ($key === "selected" || $key === "SELECTED")
			continue;
		if (is_array($values))
			$selectable_data[] = $values[$field_name."_id"];
		else 
			$selectable_data[] = $values;	
	}

	if (!in_array($field_value, $selectable_data, true)) 
		return array(false, "The field $field_name is not valid, is different from the allowed values.");

	return array(true);
}

/**
 * Generic test field function
 * Test field to be in an interval [min,max] or
 * compare the field against a given regex
 */
function check_field_validity($field_name, $field_value, $min=false, $max=false, $regex=false, $fixed=false)
{
	if ($min !== false && $max !== false)  {
		$valid = check_valid_number($field_name, $field_value);
		if (!$valid[0])
		    return $valid;
		if ((int)$field_value<$min || (int)$field_value>$max) 
			return array(false, "Field '".$field_name."' is not valid. It has to be greater than $min and smaller than $max.");
	} 

	if ($regex) {
		if (!filter_var($field_value, FILTER_VALIDATE_REGEXP, array("options"=>array("regexp"=>"/".$regex."/"))))
			return array(false, "Field '".$field_name."' is not valid.");
	}

	if ($fixed) {
		if ($field_value != $fixed)
			return array(false, "Field '". $field_name ."' is not valid. It must be: ".$fixed);
	}

	return array(true);
}

/* validate an IP address*/
function check_valid_ipaddress($field_name, $field_value)
{
	if (!filter_var($field_value, FILTER_VALIDATE_IP)) 
		  return array(false, "Field $field_name is not valid. $field_value is not a valid IP address!");
	
	return array(true);
}

/* Validate a space-separated list of the DNS servers, in IP dotted notation, eg: 1.2.3.4 5.6.7.8. */
function check_valid_dns($field_name, $field_value)
{
	//this field can be empty
	if (!$field_value)
		return array(true);

	//validate DNS that are separed by space
	$dns_addresses = explode(" ", $field_value);
	$total = count($dns_addresses);
	for ($i=0; $i<$total; $i++)
		$res[] = check_valid_ipaddress($field_name, $dns_addresses[$i]);

	for ($i=0; $i<count($res); $i++) {
		if (!$res[$i][0])
			return array(false, $res[$i][1]);
	}

	return array(true);
}

function check_valid_geolocation($field_name, $field_value)
{
	if (preg_match('/^[-]?(([0-8]?[0-9])\.(\d+))|(90(\.0+)?),[-]?((((1[0-7][0-9])|([0-9]?[0-9]))\.(\d+))|180(\.0+)?)$/', $field_value)!=1)
		return array(false, "Field $field_name is not valid. $field_value doesn't match format dd.dddddd,ddd.dddddd!");
	return array(true);
}
?>
