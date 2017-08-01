<?php

/**
 * Copyright (C) 2012-2013 Null Team
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

require_once("debug.php");

function make_request($out, $request=null, $response_is_array=true, $recursive=true)
{
	Debug::func_start(__FUNCTION__,func_get_args(),"ansql");

	global $send_request_function;
	global $path_api;

	$func = (isset($send_request_function) && $send_request_function && function_exists($send_request_function)) ? $send_request_function : "make_direct_request";

	if (isset($send_request_function) && $send_request_function && function_exists($send_request_function))
		$func = $send_request_function;
	elseif (isset($path_api))
		$func = "make_direct_request";
	else
		$func = "make_curl_request";

	if (isset($out["params"]) && is_callable("transform_request_params"))
		transform_request_params($out["params"]);

	return $func($out,$request,$response_is_array,$recursive);
}

function make_direct_request($out, $request=null, $response_is_array=true, $recursive=true)
{
	Debug::func_start(__FUNCTION__,func_get_args(),"ansql");

	global $path_api;

	$func = $request;
	if (substr($func,-4)!=".php") {
		$page = "$func.php";
	} else {
		$page = $func;
		$func = substr($func,0,strlen($func)-4);
	}

	require_once($page);
	// handle_direct_request must be implemented separately
	// and it's application specific
	$inp = handle_direct_request($func,$out);

	if ($func == "get_captcha")
		return;

	check_errors($inp);
	if (($inp["code"]=="215" || $inp["code"]=="226") && $recursive) {
		$res = make_curl_request(array(),"get_user",true,false);
		if ($res["code"]=="0" && isset($res["user"])) 
			$_SESSION["site_user"] = $res["user"];
		else
			Debug::trigger_report("critical","Could not handle direct request nor curl request for $request");
	}
	return $inp;
}

function build_request_url(&$out,&$request)
{
	Debug::func_start(__FUNCTION__,func_get_args(),"ansql");

	global $server_name,$request_protocol;

	if (!isset($request_protocol))
		$request_protocol = "http";
	$url = "$request_protocol://$server_name/json_api.php";
	$request_full = $request; //(substr($request,-4)!=".php") ? $request.".php" : $request;
	$out = array("req_func"=>$request_full,"req_params"=>$out);
	return $url;
}

function make_curl_request($out, $request=null, $response_is_array=true, $recursive=true)
{
	Debug::func_start(__FUNCTION__,func_get_args(),"ansql");

	global $method;
	global $conn_error;
	global $parse_errors;
	global $json_api_secret;
	global $func_build_request_url;
	global $request_timeout;

	if (substr($request,0,7)!="http://" && substr($request,0,8)!="https://") {
		if (!isset($func_build_request_url) || !$func_build_request_url)
			$func_build_request_url = "build_request_url";

		$url = $func_build_request_url($out,$request);
		if (!$url || (is_array($url) && !$url[0])) {
			$resp = array("code"=>"-104", "message"=>_("Please specify url where to send request."));
			write_error($request, $out, "", "", $url, $resp);
			return $resp;
		}
	} else
		$url = $request;

	$curl = curl_init($url);
	if ($curl === false) {
		$resp = array("code"=>"-103", "message"=>_("Could not initialize curl request."));
		write_error($request, $out, "", "", $url, $resp);
		return $resp;
	}

	if (isset($_SESSION["cookie"])) {
		$cookie = $_SESSION["cookie"];
	}

	$timeout = 20;
	if (count($request_timeout) && in_array($out["request"], array_keys($request_timeout))) {
		$timeout = $request_timeout[$out["request"]];
	}

	curl_setopt($curl,CURLOPT_POST,true);
	curl_setopt($curl,CURLOPT_SSL_VERIFYPEER, 0); # Equivalent to -k or --insecure 
	curl_setopt($curl,CURLOPT_POSTFIELDS,json_encode($out));
	curl_setopt($curl,CURLOPT_HTTPHEADER,array(
	    "X-Authentication: ".$json_api_secret,
	    "Content-Type: application/json",
	    "Accept: application/json,text/x-json,application/x-httpd-php",
	    "Accept-Encoding: gzip, deflate"
	));
	curl_setopt($curl,CURLOPT_RETURNTRANSFER,true);
	curl_setopt($curl,CURLOPT_CONNECTTIMEOUT,5);
	curl_setopt($curl,CURLOPT_TIMEOUT,$timeout);
	curl_setopt($curl,CURLOPT_HEADER, true);

	if (isset($cookie)) 
		curl_setopt($curl,CURLOPT_COOKIE,$cookie);

	$http_code = "-";
	$ret = curl_exec($curl);

	if ($ret === false) {
		$error = curl_error($curl);
		// if no response from api / request times out this will be received
		$resp = array("code"=>"-100", "message"=>_("Could not send request. Please try again later. Error: $error."));
		write_error($request, $out, $ret, "CURL exec error: $error", $url, $resp);
		curl_close($curl);
		return $resp;
	} else {
		$info = curl_getinfo($curl);

		$http_code = curl_getinfo($curl,CURLINFO_HTTP_CODE);
		$raw_headers = explode("\n", substr($ret, 0, $info['header_size']) );

		$gzip = false;
		if (!isset($_SESSION["all_cookies"]))
			$_SESSION["all_cookies"] = array();
		foreach ($raw_headers as $header) {
			if( preg_match('/^(.*?)\\:\\s+(.*?)$/m', $header, $header_parts) ){
				$headers[$header_parts[1]] = $header_parts[2];

				if ($header_parts[1] == "Set-Cookie") {
					$cookie = $header_parts[2];
					$cookie = explode("=", $cookie);
					if (count($cookie)<2)
						continue;
					$value = explode(";",$cookie[1]);
					$value = $value[0];
					$_SESSION["all_cookies"][$cookie[0]] = $value;
				}
				if ($header_parts[1] == "Content-Encoding" && substr(trim($header_parts[2]),0,4)=="gzip")
					$gzip = true;
			}
		}

		$_SESSION["cookie"] = "";
		foreach ($_SESSION["all_cookies"] as $name=>$value) {
			if ($_SESSION["cookie"]!="")
				$_SESSION["cookie"] .= "; ";
			$_SESSION["cookie"] .= "$name=$value";
		}
		// test to check that server doesn't return gziped captcha
		//if ($gzip && $request!="get_captcha" ) {
		if ($gzip) {
			$zipped_ret = substr($ret,$info['header_size']);
			$ret = gzinflate(substr($zipped_ret,10));
		} else
			$ret = substr($ret,$info['header_size']);

		$code = curl_getinfo($curl,CURLINFO_HTTP_CODE);
		$type = curl_getinfo($curl,CURLINFO_CONTENT_TYPE);
		if ($type == "application/json") {
			$inp = json_decode($ret,true);
			if (!$inp || $inp==$ret) {
				$resp = array("code"=>"-101", "message"=>_("Could not parse JSON response."));
				write_error($request, $out, $ret, $http_code, $url, $resp);
				curl_close($curl);
				return $resp;
			}
			check_errors($inp);
			curl_close($curl);
			/*if (($inp["code"]=="215" || $inp["code"]=="226") && $recursive) {
				$res = make_curl_request(array(),"get_user",$response_is_array,false);
				if ($res["code"]=="0" && isset($res["user"])) 
					$_SESSION["site_user"] = $res["user"];
				// else
				//       bad luck, maybe submit bug report
			}*/
			return $inp;
		} elseif ($type == "image/jpeg") {
			if ($response_is_array) {
				$resp = array("code"=>"-102", "message"=>_("Invalid content type image/jpeg for response."));
				write_error($request, $out, $ret, $http_code, $url, $resp);
				return $resp;
			
			}
			curl_close($curl);
			print $ret;
		} elseif ($type == "application/octet-stream" || substr($type,0,10) == "text/plain") {
			curl_close($curl);
			return $ret;
		} else {
			//print $ret;
			$resp = array("code"=>"-101", "message"=>_("Could not parse response from API."));
			write_error($request, $out, $ret, $http_code, $url, $resp);
			curl_close($curl);
			return $resp;
			//return $ret;
		}
	}
}

function write_error($request, $out, $ret, $http_code, $url, $displayed_response=null)
{
	Debug::func_start(__FUNCTION__,func_get_args(),"ansql");

	global $parse_errors, $display_parse_error, $error_type_func;

	$user_id = (isset($_SESSION["user_id"])) ? $_SESSION["user_id"] : "-";
	$text = "------".date("Y-m-d H:i:s").",request=$request,url=$url,user_id=$user_id\n"
		."Sent: ".json_encode($out)."\n"
		."Received HTTP CODE=$http_code : ".$ret."\n";
	if ($displayed_response)
		$text .= "Displayed: ".json_encode($displayed_response)."\n";

	// keep writing errors separately but also write them to common logs file
	if ($displayed_response["code"]) {
		$code = $displayed_response["code"];
		if (is_callable("get_type_error") && !isset($error_type_func)) {
			if (get_type_error($code) == "fatal")
				Debug::trigger_report('ansql_json', $text);
		} elseif (isset($error_type_func) && is_callable($error_type_func)) {
			if (call_user_func_array($error_type_func,array($code)))
				Debug::trigger_report('ansql_json', $text);
		} else {
			Debug::trigger_report('ansql_json', '$error_type_func not set. The received code must be translated into an error type.');
		}
	}

	if (isset($parse_errors) && strlen($parse_errors)) {
		$fh = fopen($parse_errors, "a");
		if ($fh) {
			fwrite($fh,$text);
			fclose($fh);
		}
	}
	if ((isset($display_parse_error) && $display_parse_error) || (isset($_SESSION["debug_all"]) && $_SESSION["debug_all"]=="on")) {
		$text = str_replace("\n", "<br/>", $text);
		print $text;
	}
}

/** 
 * DEPRECATED. Use Debug::output() or Debug::xdebug('progress',$message) instead
 */
function write_text_error($message)
{
	Debug::func_start(__FUNCTION__,func_get_args(),"ansql");
	Debug::output('critical',$message);

	global $parse_errors;
	if (isset($parse_errors) && strlen($parse_errors)) {
		$fh = fopen($parse_errors, "a");
		if ($fh) {
			$user_id = (isset($_SESSION["user_id"])) ? $_SESSION["user_id"] : "-";
			fwrite($fh,"------".date("Y-m-d H:i:s").",user_id=$user_id\n");
			fwrite($fh,"Text: $message"."\n");
			fclose($fh);
		}
	}
}

function not_auth($res)
{
	Debug::func_start(__FUNCTION__,func_get_args(),"ansql");

	if ($res["code"]=="43")
		return true;
	return false;
}

function check_errors(&$res)
{
	Debug::func_start(__FUNCTION__,func_get_args(),"ansql");

	global $error_codes;

	if ($res!=null && (!isset($res["message"]) || (!strlen($res["message"]))) && @array_key_exists('code',$res)) {
		if (isset($error_codes[$res["code"]]))
			$res["message"] = $error_codes[$res["code"]];
		foreach ($res as $name => $val) {
			if (is_array($val) && !isset($val["message"]) && isset($val['error_code'])) {
				$res[$name]["message"] = $error_codes[$val["error_code"]];
			}
		}
	}

	// later on we might choose to hide error code < 200
	//if ($res["code"]<200)
	//	$res["message"] = "Problem with your request. Try again later.";
	
	if (not_auth($res)) {
		session_unset();
		if ($_SERVER["PHP_SELF"]=="/main.php") {
			on_automatic_signout($res);
			exit();
		}
	}
}

function response($response)
{
	Debug::func_start(__FUNCTION__,func_get_args(),"ansql");

	print json_encode($response,JSON_HEX_TAG | JSON_HEX_APOS | JSON_HEX_QUOT | JSON_HEX_AMP);
	return;
}

function check_session_on_request()
{
	Debug::func_start(__FUNCTION__,func_get_args(),"ansql");

	if (!isset($_SESSION["user_id"])) {
		response(array("code"=>"43", "message"=>"Not authenticated."));
		return false;
	}
	return true;
}

function logout_from_api()
{
	Debug::func_start(__FUNCTION__,func_get_args(),"ansql");

	make_curl_request(array(),"logout");
}

// REQUEST working mode from equipment with SDR: bts/enb 
// Additionally it initializes available nodes in $_SESSION["node_types"]
function get_working_mode()
{
	if (!isset($_SESSION["available_sdr_modes"])) {
		// nib,roaming,dataroam,enb
		$installed_modes = request_api(array(), "get_available_modes", "modes");
		$modes = clean_sdr_modes($installed_modes);
		$_SESSION["available_sdr_modes"] = $modes;
	}
	
	if (!isset($_SESSION["sdr_mode"])) {
		// bts, enb
		$installed_nodes = request_api(array(), "get_node_type", "node_type");
		$node_types = clean_node_types($installed_nodes);

		if (count($node_types) && isset($node_types[0]["sdr_mode"])) {
			$_SESSION["node_types"] = $node_types;
			$sdr_mode = $node_types[0]["sdr_mode"];
			
			if (!in_array($sdr_mode, $_SESSION["available_sdr_modes"])) {
				$sdr_mode = "";
			}
			
			$_SESSION["sdr_mode"] = $sdr_mode;
			return $sdr_mode;
		}
		return "";
	}
	
	return $_SESSION["sdr_mode"];
}

function clean_sdr_modes($installed_modes)
{
	$modes = array();
	$accepted_modes = array("nib","roaming","dataroam","enb");
	foreach ($installed_modes as $mode) {
		if (in_array($mode, $accepted_modes))
			$modes[] = $mode;
	}
	return $modes;
}

function clean_node_types($installed_nodes)
{
	// Ex: [{"type":"smsc","version":"unknown"},{"type":"eir","version":"unknown"},{"type":"ucn","version":"unknown"},{"type":"bts","version":"0.1-1_r74_r289.mga5","sdr_mode":"enb"},
	// {"type":"enb","version":"0.1-1_r74_r289.mga5","sdr_mode":"enb"},{"type":"stp","version":"unknown"},{"type":"dra","version":"unknown"},{"type":"pcrf","version":"unknown"},{"type":"hss","version":"unknown"}]

	$nodes = array();
	$accepted_nodes = array("enb", "bts");
	foreach ($installed_nodes as $node) {
		if (!isset($node["type"]) || !in_array($node["type"], $accepted_nodes))
			continue;
		$nodes[] = $node;
	}
	return $nodes;
}
/**
 * Uses get_node_status API request to get the node status. 
 * Returns array("state":text,"color":red/green/yellow,"details":true/false)
 */ 
function node_status($out=array(), $url="get_node_status")
{
	$res = make_request($out,$url);

	if ($res["code"] != 0)
		return array("state"=>html_entity_decode(nl2br($res["message"])), "color"=>"red", "details"=>false);

	$node_status = array(
		"details"=>true,
		"state"=> html_entity_decode(nl2br($res["status"]["state"]))
	);

	if ($res["status"]["level"]) {
		$all_colors = array(
			"green"  => array("NOTE"), 
			"red"    => array("WARN","FAIL","CRIT"), 
			"yellow" => array("MILD")
		);
		foreach ($all_colors as $color=>$levels) {
			if (!in_array($res["status"]["level"],$levels))
				continue;
			$node_status["color"] = $color;
		}

	} elseif ($res["status"]["operational"]) {
		$node_status["color"] = "green";
	} else {
		$node_status["color"] = "red";
		if ($res["status"]["level"] == "MILD")
			$node_status["color"] = "yellow";
	}

	return $node_status;
}
?>
