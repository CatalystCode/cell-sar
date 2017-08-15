<?php
/**
 * enb_tabbed_settings.php
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
require_once("ansql/sdr_config/enb_fields.php");
require_once("ansql/sdr_config/check_validity_fields_enb.php");

class EnbTabbedSettings extends TabbedSettings
{
	protected $allow_empty_params = array("addr4", "addr6", "reportingPath", "radio_driver", "address", "port", "pci", "earfcn", "broadcast", "max_pdu", "AddrSubnet", "AuxRoutingTable", "Name", "streams", "dscp", "UplinkRbs", "UplinkRbStartIndex", "PrachResponseDelay","mme_address_2", "local_2", "streams_2", "dscp_2","mme_address_3", "local_3", "streams_3", "dscp_3","mme_address_4", "local_4", "streams_4", "dscp_4","mme_address_5", "local_5", "streams_5", "dscp_5", "antenna_type", "antenna_serial_number", "antenna_cable_type", "antenna_cable_length", "power_suply_type", "power_suply_serial_number", "location", "siteName", "antennaDirection", "mode", "custom_parameters");

	protected $trigger_names      = array("mme"=>"add_mme_");
	protected $default_section    = "radio";
	protected $default_subsection = "enodeb";
	protected $title              = "ENB";
	protected $menu_css           = "menu menu_enb";

	function __construct()
	{
		$this->current_section    = $this->default_section;
		$this->current_subsection = $this->default_subsection;
		$this->open_tabs = 3;
	}

	function getMenuStructure()
	{
		global $developers_tab;

		Debug::func_start(__METHOD__, func_get_args(), "tabs_enb");

		if (!$developers_tab)
			$developers_tab = false;

		//The key is the MENU alias the sections from $fields 
		//and for each menu is an array that has the submenu data with subsections 
		$structure = array(
			"Radio" => array("EnodeB", "Calibration"/*, "Bearers"*/),
			"Core" => array("GTP", "MME"/*, "S1AP"*/),
			"Access channels" => array("PRACH",/* "PDSCH",*/ "PUSCH", "PUCCH", "PDCCH"),

			"Hardware" => array("Site info", "Site equipment", "Shutdown"),
			"System" => array("System information", "Scheduler"/*, "Advanced", "RadioHardware", "Measurements"*/),
		);

		if ($developers_tab)
			$structure["Developers"] = array("Radio", "General", "Uu-simulator", "Uu-loopback", "Test-Enb", "Test-scheduler");

		return $structure;
	}

	function getSectionsDescription()
	{
		global $developers_tab;
		Debug::func_start(__METHOD__, func_get_args(), "tabs_enb");

		if (!$developers_tab)
	        $developers_tab = false;

		$desc = array(
			"enodeb" => "These are the most basic configuration parameters and the ones most likely to be changed.<br/><br/>
They are gathered in this section to make them easy to find.", //basic section from file
		//	"bearers" => "Bearer parameters (RLC and PDCP layers)",

			"calibration" => "Parameters used in configuration of radio calibration module.", 
			"gtp" => "S1-U interface parameters",
			
			"mme" => "Hand-configured MME. \n
		    The eNodeB normally selects an MME using DNS,
		    but explicit MME selection is also possible.  \n
		    This is an example of an explicit MME configuration:  

		    address = 192.168.56.62 
		    local = 192.168.56.1 
		    streams = 5 
		    dscp = expedited",

			"scheduler" => "Parameters related to the MAC scheduler",
		/*	"advanced" => "Here, \"advanced\" just means parameters that are not normally changed.  \n
		    It is not a reference to LTE-Advanced features.",*/
		//	"measurements" => "KPI-related performance measurements",
		//	"radiohardware" => "Control parameters for the lower PHY",

			"site_info" => "Site specific information.",
			"site_equipment" => "This is an area for customer-specific parameters for other site equipment,
like antennas, cables, and anything else too \"dumb\" to carry its own
configuration and identifying information. \n
No defaults are provided.",
			"shutdown" => "Parameters for safety shutdown of SatSite components. \n
Raising these parameters above their default values may result in damage to the eNodeB hardware or reduced equipment life.  \n
This parameters are ignored in Labkit units."
		);

		if ($developers_tab) {
			$desc["radio"] = "These are parameters for configuring the radio device";
			$desc["general"] = "Global configuration for the ENB Yate Module";
			$desc["uu-simulator"] = "Configuration parameters for the Uu interface simulator";
			$desc["uu-loopback"] = "Configuration parameters for the UeConnection test fixture";
			$desc["test-enb"] = "This section controls special test modes of the eNodeB.";
			$desc["test-scheduler"] = "This section controls special test modes of the eNodeB.";
		}

		return $desc;
	}

	// Retrieve settings using get_enb_node and build array with renamed sections to match interface fields
	function getApiFields()
	{
		global $developers_tab;
		Debug::func_start(__METHOD__, func_get_args(), "tabs_enb");

		$response_fields = request_api(array(), "get_enb_node", "node");
		if (!isset($response_fields["yateenb"])) {
			Debug::xdebug("tabs_enb", "Could not retrieve yateenb fields in " . __METHOD__);
			return null;
		}

		$res = $response_fields["yateenb"];
		if (isset($response_fields["gtp"]["ran_u"])) {
			$gtp_settings = $response_fields["gtp"]["ran_u"];
			if (isset($gtp_settings["addr4"]) && ($gtp_settings["addr4"]=="false" || !$gtp_settings["addr4"]))
				unset($gtp_settings["addr4"]);
			if (isset($gtp_settings["addr6"]) && ($gtp_settings["addr6"]=="false" || !$gtp_settings["addr6"]))
				unset($gtp_settings["addr6"]);
		} else {
			$gtp_settings = array();
			Debug::xdebug("tabs_enb", "Could not retrieve gtp fields in " . __METHOD__);
		}
		$res["gtp"] = $gtp_settings;

		if (isset($response_fields["sdr"])) {
			$hardware_settings = $response_fields["sdr"];
			if (isset($hardware_settings["basic"])) {
				$hardware_settings["site_info"] = $hardware_settings["basic"];
				unset($hardware_settings["basic"]);
			}
			foreach ($hardware_settings as $section=>$section_def)
				$res[$section] = $section_def;
		}

		// set mme fields
		foreach ($response_fields["yateenb"] as $section_name=>$section_def) {
			if (substr($section_name,0,3)!="mme")
				continue;
			$index = substr($section_name, 3);
			if ($index=="1") {
				$res["mme"] = $section_def;
				$res["mme"]["mme_address"] = $res["mme"]["address"];
				unset($res["mme"]["address"]);
			} else {
				foreach ($section_def as $param_name=>$param_value)
					$res["mme"][$param_name."_$index"] = $param_value;
				$res["mme"]["mme_address"."_$index"] = $res["mme"]["address_$index"];
				unset($res["mme"]["address_$index"]);
			}
		}

		// rename some of the sections the parameters appear under: basic->enodeb, basic-> all subsections under access channels, basic-> system_information
		$aggregated_subsections = array("enodeb","system_information",/*"pdsch",*/"pusch","pucch","prach","pdcch");
		foreach ($aggregated_subsections as $subsection)
			if (isset($res["basic"]))
				$res[$subsection] = $res["basic"];

		if (!$developers_tab)
			$developers_tab = false;

		if ($developers_tab) {
			if (isset($res["test-enb"]))
				$res["test-scheduler"] = $res["test-enb"];
			if (!isset($res["general"]["mode"]))
				$res["general"]["mode"] = "";
		}

		$this->storeCalibrationFields($response_fields, $res);

		return $res;
	}

	function getDefaultFields()
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs_enb");

		return get_default_fields_enb();
	}
	
	function buildParticularParam($data=null, $param=null, $request_fields=array())
	{
		return $data;
	}

	/**
	 * Build form fields by applying response fields over default fields
	 * @param $request_fields Array. Fields retrived using getApiFields
	 * @param $exists_in_response Bool. Verify if field exists in $request_fields otherwise add special marker. Default false
	 * @return Array
	 */
	function applyRequestFields($request_fields=null,$exists_in_response=null)
	{
		global $developers_tab;
		Debug::func_start(__METHOD__, func_get_args(), "tabs_enb");

		$structure = $this->buildConfSections();
		$fields    = $this->getDefaultFields();

		if (!$request_fields)
			return $fields;
		if (!$developers_tab)
			$developers_tab = false;

		$custom_site_equipment = "";
		foreach ($structure as $section=>$data) {
			foreach ($data as $key=>$subsection) {
				if (isset($request_fields[$subsection])) {
					foreach ($request_fields[$subsection] as $param=>$data) {
						
						if ($subsection=="measurements" && $param=="reportingPeriod") {
							$param = "kpi_reportingPeriod";
						}
						
						if (!isset($fields[$section][$subsection]))
							continue;

						if (!isset($fields[$section][$subsection][$param])) {
							if ($subsection!="site_equipment")
								continue;
							if (strlen($custom_site_equipment))
								$custom_site_equipment .= "\n";
							$custom_site_equipment .= "$param=$data";
							continue;
						}

						if (isset($fields[$section][$subsection][$param]["display"]) && ($fields[$section][$subsection][$param]["display"]=="select" || $fields[$section][$subsection][$param]["display"]=="select_without_non_selected")) {

							if ($data=="" && in_array("Factory calibrated", $fields[$section][$subsection][$param][0]))
								$data = "Factory calibrated";
							$fields[$section][$subsection][$param][0]["selected"] = $data;

						} elseif (isset($fields[$section][$subsection][$param]["display"]) && $fields[$section][$subsection][$param]["display"] == "checkbox")

							$fields[$section][$subsection][$param]["value"] = ($data=="yes" || $data=="on" || $data=="1")  ? "on" : "off";

						else 
							$fields[$section][$subsection][$param]["value"] = $data; 

						// unmark triggered fields if they are set
						if ((strlen($data) || getparam($param)) && isset($this->trigger_names[$subsection])) {
							$trigger_name = $this->trigger_names[$subsection];
							if (isset($fields[$section][$subsection][$param]["triggered_by"]) && ctype_digit($fields[$section][$subsection][$param]["triggered_by"])) {
								$triggered_by   = $fields[$section][$subsection][$param]["triggered_by"];
								$former_trigger = $triggered_by - 1;

								if (isset($fields[$section][$subsection][$trigger_name.$former_trigger]))
									unset($fields[$section][$subsection][$trigger_name.$former_trigger]);

								$fld = $fields[$section][$subsection];

								foreach ($fld as $fldn=>$fldd) {
									if (isset($fldd["triggered_by"]) && $fldd["triggered_by"]==$triggered_by) {
										unset($fields[$section][$subsection][$fldn]["triggered_by"]);
									}
								}

								unset($fields[$section][$subsection][$param]["triggered_by"]);
							}
						}
					}
				}
			}
		}

		// SRB fields
		$ack_fields = array("rlcTPollRetransmit", "rlcTReordering", "rlcTStatusProhibit", "rlcMaxRetxThreshold", "rlcPollPdu", "rlcPollByte");
		$unack_fields = array("rlcSnFieldLength" ,"rlcTReordering");

		// unset triggered_by for SRB depending on selected mode
/*		for ($i=1; $i<3; $i++) {
			if (strlen(getparam("Srb$i.mode")))
				$srb_mode = getparam("Srb$i.mode");
			elseif (isset($fields["radio"]["bearers"]["Srb$i.mode"][0]["selected"]))
				$srb_mode = $fields["radio"]["bearers"]["Srb$i.mode"][0]["selected"];
			else 
				$srb_mode = '';

			$trigger = array();
			if ($srb_mode=="acknowledged") 
				$trigger = $ack_fields;
			elseif ($srb_mode=="unacknowledged")
				$trigger = $unack_fields;
			for ($j=0; $j<count($trigger); $j++) {
				unset($fields["radio"]["bearers"]["Srb$i.".$trigger[$j]]["triggered_by"]);
			}
		}*/

		if (strlen($custom_site_equipment))
			$fields["hardware"]["site_equipment"]["custom_parameters"]["value"] = $custom_site_equipment;


		return $fields;
	}

	function validateFields($section, $subsection)
	{
		global $developers_tab;
		Debug::func_start(__METHOD__, func_get_args(), "tabs_enb");

		$parent_res = parent::validateFields($section, $subsection);
		if (!$parent_res[0]) 
			return $parent_res;
		
		$extra_validations = array(
			array("subsection"=>"gtp",  "cb"=>"validate_gtp_params"),
			array("subsection"=>"mme",  "cb"=>"validate_mme_params")
		);

		foreach ($extra_validations as $validation) {
			if ($subsection==$validation["subsection"]) {
				$res = call_user_func($validation["cb"]);
				if (!$res[0])
					$this->error_field[]   = array($res[1], $res[2][0]);
				elseif ($res[0] && isset($res[1]))
					$this->warning_field[] = array($res[1], $res[2][0]);
			}
		}

		if (count($this->error_field))
			return array(false, "fields"=>$parent_res["fields"], "request_fields"=>$parent_res["request_fields"]);
		return $parent_res;
	}

	// Send API request to save ENB configuration
	// Break error message in case of error and mark section/subsection to be opened
	function storeFormResult(array $fields)
	{
		global $developers_tab;
		//if no errors encountered on validate data fields then send API request
		Debug::func_start(__METHOD__, func_get_args(), "tabs_enb");
		
		if (!$developers_tab)
			$developers_tab = false;

		$request_fields = array("yateenb"=>array("basic"=>array()));//, "gtp"=>array());
		$request_fields["calibrate"] = $this->setCalibrationFields($fields);

		$basic_sections = array("enodeb","system_information"/*,"pdsch"*/,"pusch","pucch","prach","pdcch");
		foreach ($basic_sections as $basic_section) {
			$request_fields["yateenb"]["basic"] = array_merge($request_fields["yateenb"]["basic"], $fields[$basic_section]);
			unset($fields[$basic_section]);
		}
		
		$gtp = $fields["gtp"];
		unset($fields["gtp"]);

		$satsite = array();
		$satsite_sections = array("site_info", "site_equipment", "shutdown");
		foreach ($satsite_sections as $satsite_section) {
			$section_name = ($satsite_section!="site_info") ? $satsite_section : "basic";
			$satsite[$section_name] = $fields[$satsite_section];
			unset($fields[$satsite_section]);
		}
		if (count($satsite))
			$request_fields["sdr"] = $satsite;
		
		if (isset($fields["measurements"]["kpi_reportingPeriod"])) {
			$fields["measurements"]["reportingPeriod"] = $fields["measurements"]["kpi_reportingPeriod"];
			unset($fields["measurements"]["kpi_reportingPeriod"]);
		}

		if (strlen($fields["mme"]["mme_address"])) {

			$index = 1;
			$mme_fields = array("mme_address"=>"address","local","streams","dscp");

			while(true) {
				$suffix = ($index>1) ? "_$index" : "";
				$custom_mme = array();
				if (isset($fields["mme"]["mme_address$suffix"]) && strlen($fields["mme"]["mme_address$suffix"])) {
					foreach ($mme_fields as $field_index=>$field_name) {
						$form_name = (is_numeric($field_index)) ? $field_name.$suffix : $field_index.$suffix;
						$custom_mme[$field_name] = $fields["mme"][$form_name];
					}
					$fields["mme$index"] = $custom_mme;
					$index++;
				} else
					break;
			}
		}
		unset($fields["mme"]);

		if (isset($request_fields["sdr"]["site_equipment"]["custom_parameters"])) {
			if (strlen($request_fields["sdr"]["site_equipment"]["custom_parameters"])) {
				$custom = explode("\r\n",$request_fields["sdr"]["site_equipment"]["custom_parameters"]);
				foreach ($custom as $custom_param) {
					$custom_param = explode("=",$custom_param);
					if (count($custom_param)!=2)
						continue;
					$request_fields["sdr"]["site_equipment"][trim($custom_param[0])] = trim($custom_param[1]);
				}

			}
			unset($request_fields["sdr"]["site_equipment"]["custom_parameters"]);
		}

		if (isset($fields["bearers"]))
			$fields["bearers"] = $this->setBearers($fields["bearers"]);

		$request_fields["yateenb"] = array_merge($request_fields["yateenb"],$fields);
	
		if ($developers_tab) {
			foreach ($fields["test-scheduler"] as $p_name=>$p_value)
				$request_fields["yateenb"]["test-enb"][$p_name] = $p_value;

			unset($request_fields["yateenb"]["test-scheduler"]);
		}

		$request_fields["gtp"]["ran_u"] = $gtp;

		$res = make_request($request_fields, "set_enb_node");

		if (!isset($res["code"]) || $res["code"]!=0) {
			// find subsection where error was detected so it can be opened
			$mess        = substr($res["message"],-15);
			$pos_section = strrpos($mess,"'",-5);
			$subsection  = substr($mess,$pos_section+1);
			$subsection  = substr($subsection,0,strpos($subsection,"'"));
			if (substr($subsection,0,3)=="mme")
				$subsection = "mme";

			$section = $this->findSection($subsection);
			if (!$section) {
				Debug::output('enb tabs', "Could not find section for subsection '$subsection'");
				$section    = $this->default_section;
				$subsection = $this->default_subsection;
			}

			$this->current_subsection = $subsection;
			$this->current_section    = $section;
			$_SESSION[$this->title]["subsection"]   = $subsection;
			$_SESSION[$this->title]["section"]      = $section;

			$code = "";
			if (isset($res["code"]))
				$code = "[API: ".$res["code"]."] ";

			return array(false, $code.$res["message"]);
		} else {
			unset($_SESSION[$this->title]["section"], $_SESSION[$this->title]["subsection"]);
			return array(true);
		}
	}

	function setBearers($form_data)
	{
		$bearers = array();

		// SRB 
		$ack_fields   = array("rlcTPollRetransmit", "rlcTReordering", "rlcTStatusProhibit", "rlcMaxRetxThreshold", "rlcPollPdu", "rlcPollByte");
		$unack_fields = array("rlcSnFieldLength" ,"rlcTReordering");

		for ($i=1 ; $i<3 ; $i++) {
			$mode = $form_data["Srb$i.mode"];
			if ($mode=="acknowledged") 
				$set = $ack_fields;
			elseif ($mode=="unacknowledged") 
				$set = $unack_fields;
			else
				$set = array();

			$bearers["Srb$i.mode"] = $mode;
			foreach ($set as $key) {
				$value = (isset($form_data["Srb$i.$key"])) ? $form_data["Srb$i.$key"] : "";
				$bearers["Srb$i.$key"] = $value;
			}
		}

		// DBR
		// set all drb params
		foreach ($form_data as $key=>$value) {
			if (substr($key,0,3)=="Drb")
				$bearers[$key] = $value;
		}

		return $bearers;
	}
}
?>
