<?php
/**
 * bts_tabbed_settings.php
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

require_once("ansql/tabbed_settings.php");
require_once("ansql/sdr_config/ybts_fields.php");
require_once("ansql/sdr_config/check_validity_fields_ybts.php");
require_once("ansql/sdr_config/create_radio_band_select_array.php");

class BtsTabbedSettings extends TabbedSettings
{
	protected $allow_empty_params = array("Args", "DNS", "ShellScript", "MS.IP.Route", "Logfile.Name", "peer_arg", "RadioFrequencyOffset", "TxAttenOffset", "Radio.RxGain", "my_sip", "reg_sip", "nodes_sip", "gstn_location", "neighbors", "gprs_nnsf_bits", "nnsf_dns", "network_map", "local_breakout", "antenna_type", "antenna_serial_number", "antenna_cable_type", "antenna_cable_length", "power_suply_type", "power_suply_serial_number", "location", "siteName", "antennaDirection", "custom_parameters", "networkname", "networkname.full", "sos_sip", "extra_yatepeer_args", "peer_abort");

	protected $default_section    = "radio";
	protected $default_subsection = "gsm";
	protected $title = "BTS";
	protected $menu_css = "menu menu_bts";
	protected $skip_special_params = array("print_msg");

	function __construct($open_tabs=2)
	{
		$this->current_section = $this->default_section;
		$this->current_subsection = $this->default_subsection;
		$this->open_tabs = $open_tabs;
		$this->subsections_advanced = array("GSM advanced", "GPRS advanced", "Control");
	}

	function getMenuStructure()
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs_bts");
		//The key is the MENU alias the sections from $fields 
		//and for each menu is an array that has the submenu data with subsections 

		$structure = array(
			"Radio" => array("GSM","GPRS"/*TBI: ,"Bearers"*/, "GSM advanced", "GPRS advanced", "Control", "Calibration"),
			"Core" => array(),

			"Hardware" => array("Site info", "Site equipment", "Shutdown"),
			"System" => array("YBTS", "Security", "Transceiver"),
			"Test" => array("Test", "Tapping")
		);

		if (!isset($_SESSION["sdr_mode"])) {
			Debug::xdebug("tabs_bts", "Could not set 'Core' in structure menu, working mode not set.");
			return $structure;
		}

		$sdr_mode = $_SESSION["sdr_mode"];

		if ($sdr_mode == "dataroam") {
			$structure["Core"] = array("Roaming", "GPRS roaming", "Handover", "SGSN", "GGSN");
		}
		elseif ($sdr_mode == "roaming") {
			$structure["Core"] = array("Roaming", "Handover", "SGSN", "GGSN");
		}
		else
			$structure["Core"] = array("SGSN", "GGSN");

		return $structure;
	}

	function getSectionsDescription()
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs_bts");

		return array(
			"gsm" => "Section [gsm] controls basic GSM operation. \n
			You MUST set and review all parameters here before starting the BTS!",
			"gsm_advanced" => "Section [gsm_advanced] controls more advanced GSM features.\n
			You normally don't need to make changes in this section",
			"gprs" => "Section [gprs] controls basic GPRS operation.\n
			You should review all parameters in this section.",
			"gprs_advanced" => "Section [gprs_advanced] controls more advanced GPRS features.\n
			You normally don't need to make changes in this section.",
			"calibration" => "Parameters used in configuration of radio calibration module.",
			"ggsn" => "Section [ggsn] has internal GGSN function configuration.\n
Used in 'GPRS with Local breakout setup'.\n
Additionally, you need to have  IP Forwarding enabled and to define NAT rule that forwards the data coming from your phones to local internet connection.",
			"sgsn" => "Section [sgsn] has internal SGSN function configuration.\n
Used in 'GPRS with Local breakout setup'.",
			"control" => "Section [control] - configuration for various MBTS controls.\n
			You normally don't need to change anything in this section.",
			"transceiver" => "Section [transceiver] controls how the radio transceiver is started and operates.",
			"tapping" => "Section [tapping] - settings control if radio layer GSM and GPRS packets are tapped to Wireshark.",
			"test" => "Section [test] has special parameters used to simulate errors.",
			"ybts" => "Section [ybts] configures ybts related parameters.",
			"security" => "Section [security] configures security related parameters.",
			"roaming" => "Section [roaming] controls parameters used by roaming.js when connecting YateBTS to a core network.",
			"handover" => "Section [handover] controls handover parameters used by roaming.js.",
			"gprs_roaming" => "Section [gprs_roaming] controls parameters used by dataroam.js when connecting YateBTS to a core data network.",

			"site_info" => "Site specific information.",
			"site_equipment" => "This is an area for customer-specific parameters for other site equipment,
like antennas, cables, and anything else too \"dumb\" to carry its own
configuration and identifying information.\n
No defaults are provided.",
			"shutdown" => "Parameters for safety shutdown of SatSite components.\n
Raising these parameters above their default values may result in damage to the eNodeB hardware or reduced equipment life. \n
This parameters are ignored in Labkit units."
		);
	}

	function getApiFields()
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs_bts");

		$response_fields = request_api(array(), "get_bts_node", "node");
		if (!isset($response_fields["ybts"])) {
			Debug::xdebug("tabs_bts", "Could not retrieve ybts fields in " . __METHOD__); 
			return null;
		}
		$res = $response_fields["ybts"];

		if (isset($response_fields["sdr"])) {
			$hardware_settings = $response_fields["sdr"];
			if (isset($hardware_settings["basic"])) {
				$hardware_settings["site_info"] = $hardware_settings["basic"];
				unset($hardware_settings["basic"]);
			}
			foreach ($hardware_settings as $section=>$section_def)
				$res[$section] = $section_def;
		}

		if (isset($res["gsm"]["Radio.Band"]))
			$_SESSION["Radio.Band"] = $res["gsm"]["Radio.Band"];
		
		$this->storeCalibrationFields($response_fields, $res);
		return $res;
	}

	function getDefaultFields()
	{
		return get_default_fields_ybts();
	}

	function buildParticularParam($data = null, $param = null, $request_fields = array())
	{
	 	//there is a particular case for Radio.C0 that is build from value_radio.band."-".value_radio.c0, 
	    $new_data = $data;
	    if ($param == "Radio.C0")
			$new_data = $request_fields['gsm']['Radio.Band']."-".$data;

		return $new_data;
	}

	/**
	 * Build form fields by applying response fields over default fields
	 * @param $request_fields Array. Fields retrived using getApiFields
	 * @param $exists_in_response Bool. Verify if field exists in $request_fields otherwise add special marker. Default false
	 * @return Array
	 */
	function applyRequestFields($request_fields=null,$exists_in_response = false)
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs_bts");

		$structure = $this->buildConfSections(); //get_fields_structure_from_menu();
		$fields = $this->getDefaultFields();

		if (!$request_fields)
			return $fields;

		foreach ($structure as $section=>$data) {
			foreach ($data as $key=>$subsection) {
				if ($exists_in_response) {
					if (!isset($request_fields[$subsection])) {
						$fields["not_in_response"] = true;
						break 2;
					}

					foreach ($fields[$section][$subsection] as $paramname=>$data_fields) {
						$allow_empty_params = array("Args", "DNS", "ShellScript", "MS.IP.Route", "Logfile.Name", "peer_arg");
						if (!in_array($paramname, $allow_empty_params) && !isset($request_fields[$subsection][$paramname])) {
							$fields["not_in_response"] = true;	
							break 3;
						}
					}
				}
			}
		}

		$custom_site_equipment = "";
		$network_map = "";
		foreach ($structure as $section=>$data) {
			foreach ($data as $key=>$subsection) {
				if (isset($request_fields[$subsection])) {
					foreach ($request_fields[$subsection] as $param=>$data) {
						if (!isset($fields[$section][$subsection]))
							continue;
						if ($subsection=="gprs_roaming" && Numerify($param)!="NULL") {
							if (strlen($network_map))
								$network_map .= "\r\n";
							$network_map .= "$param=$data";
							continue;
						}
						if ($subsection=="gprs_roaming" && $param=="nnsf_bits")
							$param = "gprs_nnsf_bits";
						if ($subsection=="sgsn" && $param=="Debug")
							$param = "sgsn_debug";
						if (!isset($fields[$section][$subsection][$param])) {
							if ($subsection!="site_equipment")
								continue;
							if (strlen($custom_site_equipment))
								$custom_site_equipment .= "\n";
							$custom_site_equipment .= "$param=$data";
							continue;
						}
						
						if (isset($fields[$section][$subsection][$param]["display"]) && ($fields[$section][$subsection][$param]["display"] == "select" || $fields[$section][$subsection][$param]["display"] == "select_without_non_selected")) {
							if ($data=="" && in_array("Factory calibrated", $fields[$section][$subsection][$param][0]))
								$data = "Factory calibrated";
							$fields[$section][$subsection][$param][0]["selected"] = $data;

							if ($subsection == "roaming") {
								if (isset($_SESSION["ybts_fields"]["interfaces_ips"]["both"]) && 
									!in_array($data,$_SESSION["ybts_fields"]["interfaces_ips"]["both"])) {
									$fields[$section][$subsection][$param][0]["selected"] = "Custom";
									$fields[$section][$subsection]["custom_".$param]["value"] = $data;
									$fields[$section][$subsection]["custom_".$param]["column_name"] = "";
								}
							}
						} elseif (isset($fields[$section][$subsection][$param]["display"]) && $fields[$section][$subsection][$param]["display"] == "checkbox") 
							$fields[$section][$subsection][$param]["value"] = ($data == "yes" || $data=="on" || $data=="1")  ? "on" : "off";
						else 
							$fields[$section][$subsection][$param]["value"] = $data; 
					}
				}
			}
		}
		if (strlen($network_map)) 
			$fields["core"]["gprs_roaming"]["network_map"]["value"] = $network_map;
		if (strlen($custom_site_equipment))
			$fields["hardware"]["site_equipment"]["custom_parameters"]["value"] = $custom_site_equipment;

		if (isset($fields['radio']['gsm']['Radio.Band'][0]["selected"]) && isset($fields['radio']['gsm']["Radio.C0"][0]["selected"])) {
			$particle = $fields['radio']['gsm']['Radio.Band'][0]["selected"];
			$fields['radio']['gsm']["Radio.C0"][0]["selected"] = "$particle-".$fields['radio']['gsm']["Radio.C0"][0]["selected"];
		}
		return $fields;
	}

	function validateFields($section, $subsection)
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs_bts");

		$parent_res = parent::validateFields($section, $subsection);
		if (!$parent_res[0])
			return $parent_res;

		$extra_validations = array(
			array("mode"=>"roaming", "subsection"=>"roaming",      "cb"=>"validate_roaming_params"),
			array("mode"=>"dataroam","subsection"=>"gprs_roaming", "cb"=>"validate_dataroam_params"),
			array("mode"=>"dataroam","subsection"=>"roaming",      "cb"=>"validate_piece_roaming")
		);

		foreach ($extra_validations as $validation) {
			if (getparam("mode")==$validation["mode"] && $subsection==$validation["subsection"]) {
				$res = $validation["cb"]();
				if (!$res[0])
					$this->error_field[] = array($res[1], $res[2][0]);
				elseif ($res[0] && isset($res[1]))
					$this->warning_field[] = array($res[1], $res[2][0]);
			}
		}

		if (count($this->error_field))
			return array(false, "fields"=>$parent_res["fields"], "request_fields"=>$parent_res["request_fields"]);
		return $parent_res;
	}

	// Send API request to save BTS configuration
	// Break error message in case of error and mark section/subsection tobe opened 
	function storeFormResult(array $fields)
	{
		//if no errors encountered on validate data fields then send API request
		Debug::func_start(__METHOD__, func_get_args(), "tabs_bts");

		$c0 = $fields['gsm']['Radio.C0'];
		$c0 = explode("-",$c0);
		$c0 = $c0[1];
		$fields['gsm']['Radio.C0'] = $c0;

		if (isset($fields['gprs_roaming'])) {

			$fields['gprs_roaming']['nnsf_bits'] = $fields['gprs_roaming']['gprs_nnsf_bits'];
			unset($fields['gprs_roaming']['gprs_nnsf_bits']);

			$network_map = $fields['gprs_roaming']['network_map'];
			$network_map = explode("\r\n",$network_map);
			unset($fields['gprs_roaming']['network_map']);
			foreach ($network_map as $assoc) {
				$assoc = explode("=",$assoc);
				if (count($assoc)!=2)
					continue;
				$fields['gprs_roaming'][$assoc[0]] = trim($assoc[1]);
			}
		}

		$fields['sgsn']['Debug'] = $fields['sgsn']['sgsn_debug'];
		unset($fields['sgsn']['sgsn_debug']);

		if (getparam("my_sip")=="Custom")
			$fields["roaming"]["my_sip"] = getparam("custom_my_sip");

		$satsite = array();
		$satsite_sections = array("site_info", "site_equipment", "shutdown");
		foreach ($satsite_sections as $satsite_section) {
			$section_name = ($satsite_section!="site_info") ? $satsite_section : "basic";
			$satsite[$section_name] = $fields[$satsite_section];
			unset($fields[$satsite_section]);
		}

		$fields = array("ybts"=>$fields);
		if (count($satsite))
			$fields["sdr"] = $satsite;

		if (isset($fields["sdr"]["site_equipment"]["custom_parameters"])) {
			if (strlen($fields["sdr"]["site_equipment"]["custom_parameters"])) {
				$custom = explode("\r\n",$fields["sdr"]["site_equipment"]["custom_parameters"]);
				foreach ($custom as $custom_param) {
					$custom_param = explode("=",$custom_param);
					if (count($custom_param)!=2)
						continue;
					$fields["sdr"]["site_equipment"][trim($custom_param[0])] = trim($custom_param[1]);
				}

			}
			unset($fields["sdr"]["site_equipment"]["custom_parameters"]);
		}
		
		$fields["calibrate"] = $this->setCalibrationFields($fields["ybts"]);

		$res = make_request($fields, "set_bts_node");

		if (!isset($res["code"]) || $res["code"]!=0) {
			// find subsection where error was detected so it can be opened
			$mess        = substr($res["message"],-15);
			$pos_section = strrpos($mess,"'",-5);
			$subsection  = substr($mess,$pos_section+1);
			$subsection  = substr($subsection,0,strpos($subsection,"'"));
			$section = $this->findSection($subsection);

			if (!$section) {
				Debug::output('bts tabs', "Could not find section for subsection '$subsection'");
				$section = $this->default_section;
				$subsection = $this->default_subsection;
			}

			$this->current_subsection = $subsection;
			$this->current_section = $section;
			$_SESSION[$this->title]["subsection"] = $subsection;
			$_SESSION[$this->title]["section"] = $section;

			$code = "";
			if (isset($res["code"]))
				$code = "[API: ".$res["code"]."] ";

			return array(false, $code.$res["message"]);
		} else {
			unset($_SESSION[$this->title]["section"], $_SESSION[$this->title]["subsection"]);
			return array(true);
		}
	}

	function cleanSession()
	{
		unset($_SESSION['Radio.Band']);
	}
	
}

?>
