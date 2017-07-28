<?php
/**
 * tabbed_settings.php
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

require_once("lib.php");
require_once("check_validity_fields.php");

/**
 * Class used to build tabbed based editing form when you have many settings grouped in sections with/without subsections
 * To use it, derive from it and reimplement at least @ref getMenuStructure
 */
abstract class TabbedSettings 
{
	protected $error                = "";
	protected $warnings             = "";
	protected $current_section      = "";
	protected $current_subsection   = "";
	protected $open_tabs            = 1;
	protected $subsections_advanced = array();
	protected $menu_css             = "menu";
	protected $skip_special_params = array();
	protected $detected_invalidities_message = "";
	protected $validated_fields = false;

	protected $title;

	function __construct()
	{
	}

	// Should return Array
	abstract function getMenuStructure();

	// Should return Array
	abstract function getSectionsDescription();

	// Should return Array
	abstract function getDefaultFields();

	// Should return Array
	abstract function getApiFields();

	// Should return Array
	abstract function applyRequestFields($request_fields=null,$exists_in_response=null);

	// returns Array(true/false, "error")
	abstract function storeFormResult(array $fields);

	//returns the modified value of the fields
	abstract function buildParticularParam($data=null,$param=null,$request_fields=array());

	//clean session data
	function cleanSession() 
	{
	}

	// Validate form result. Use field definition to do this: select/checkbox/Factory calibrated param/or call function to make callback specified in "validity" in field format
	// Returns array(true/false, "fields"=>.. (fields to build form), "request_fields"=> .. (fields to be sent to API or written to file)
	// "fields" has a section,subsection structure, while "request_fields" has only the subsections(specified in interface)  (that should be sent to API or written to file )
	function validateFields($section, $subsection)
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs");

		$allow_empty_params = $this->allow_empty_params;
		if (!is_array($allow_empty_params)) {
			Debug::trigger_report("critical", "Class variable allow_empty_params is not array.");
			return array(false);
		}

		$fields = $this->getDefaultFields();

		$new_fields           = array();
		$request_fields       = array();
		$this->error_field    = array();
		$this->warning_field  = array();
		
		if (!isset($fields[$section][$subsection]))
			return array(true, "fields"=>array(), "request_fields"=>array());

		foreach ($fields[$section][$subsection] as $param_name=>$data) {

			if (isset($data["display"]) && in_array($data["display"],array("objtitle","message")))
				continue;

			// start for default fields
			$paramname = str_replace(".", "_", $param_name);
			$new_fields[$section][$subsection][$param_name] = $data;

			$field_param = getparam($paramname);
			if (isset($data["display"]) && $data["display"]=="checkbox" && $field_param==NULL) 
				$field_param = "off";
			$field_param = htmlspecialchars_decode($field_param);

			if (!valid_param($field_param)) {
				if (!in_array($param_name, $allow_empty_params)) {
					$this->error_field[] = array("Field $param_name can't be empty.", $param_name);
					continue;
				}
			}

			$res = array(true);
			if (isset($data["display"]) && ($data["display"]=="select" ||  $data["display"]=="select_without_non_selected") && !isset($data["validity"]) && !in_array($param_name, $allow_empty_params)) 
				$res = $this->validSelectField($param_name, $field_param, $data[0]);
			elseif (isset($data["validity"]) && valid_param($field_param)) 
				$res = $this->cbValidityField($data["validity"], $param_name, $field_param);

			if (!$res[0])
				$this->error_field[]   = array($res[1], $param_name);
			elseif ($res[0] && isset($res[1]))
				$this->warning_field[] = array($res[1], $param_name);

			// set to "" parameters that are not set or are set to "Factory calibrated"
			// In the end (after API request/other method) they will be written commented in ybts.conf/etc..
			if (!valid_param($field_param) || $field_param=="Factory calibrated")
				$field_param = "";

			// build also request_fields fields with different structure
			$request_fields[$subsection][$param_name] = $field_param;

			if (isset($data["display"]) && ($data["display"]=='select' || $data["display"]=="select_without_non_selected"))
				$new_fields[$section][$subsection][$param_name][0]["selected"] = $field_param;
			else
				$new_fields[$section][$subsection][$param_name]["value"]       = $field_param;
		}

		$status = (!count($this->error_field)) ? true : false;
		return array($status, "fields"=>$new_fields, "request_fields"=>$request_fields);
	}


	/**
	 * Makes the callback for the function set in "validity" field from array $fields (from ybts_fields.php).  
	 * Note: There are 4 types of callbacks:(call_function_from_validity_field will make the callback for the last 3 cases)
	 * - a generic function that test if a value is in an array (check_valid_values_for_select_field($param_name, $field_param, $select_array)) this is not set in "validity" field
	 * this function is triggered by $fields[$section][$subsection]["display"] == "select"
	 * - a generic function that test if a value is in a specific interval OR in a given regex(check_field_validity($field_name, $field_value, $min=false, $max=false, $regex=false))
	 * this is triggered from $fields[$section][$subsection]["validity"] field with the specific parameters.
	 * Ex: "validity" => array("check_field_validity", false, false, "^[0-9]{2,3}$") OR 
	 *     "validity" => array("check_field_validity", 0, 65535) 
	 * - a specified function name with 3 parameter: field_name, field_value, $restricted_value is the getparam("field_value")
	 * this is triggered from $fields[$section][$subsection]["validity"] that contains the name of the function and the specific parameters. Ex: "validity" => array("check_uplink_persistent", "Uplink_KeepAlive")
	 * - a specified function name with 2 parameters: field_name, field_value
	 * this is triggered from $fields[$section][$subsection]["validity"] that contains only the name of the function. Ex: "validity" => array("check_timer_raupdate") 
	 */
	function cbValidityField($validity, $param_name, $field_param)
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs_validations");

		if (function_exists("call_user_func_array")) { //this function exists in PHP 5.3.0
			$total = count($validity);
			$args  = array($param_name,$field_param);
			
			for ($i=1; $i<$total; $i++) 
				$args[] = ($total==2) ? getparam($validity[$i]) : $validity[$i];

			//call a function ($validity[0] = the function name; and his args as an array)
			$res = call_user_func_array($validity[0],$args);
		} else {
			$func_name = $validity[0];
			if (count($validity)>=3) {
				$param1 = $validity[1];
				$param2 = $validity[2];
				$param3 = (isset($validity[3])) ? $validity[3] : false;
				//call to specified validity function
				$res = $func_name($param_name, $field_param, $param1, $param2, $param3);
			} else  {
				if (!isset($validity[1])) 
					$res = $func_name($param_name, $field_param);
				else
					$res = $func_name($param_name, $field_param, getparam($validity[1]));
			}
		}
		return $res;
	}

	function validSelectField($field_name, $field_value, $select_array)
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs_validations");

		return check_valid_values_for_select_field($field_name, $field_value, $select_array);
	}


	/**
	 * Build array with conf section names by taking all subsections and formating them: apply lowercase and replace " " with "_"
	 */
	function buildConfSections()
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs");

		$structure = $this->getMenuStructure();

		$fields_structure = array();
		foreach ($structure as $section=>$data) {
			$section = str_replace(" ", "_", strtolower($section));
			if (!$data)
				$data = array(str_replace(" ", "_", strtolower($section)));
			foreach ($data as $key=>$subsection) {
				$subsection = str_replace(" ", "_", strtolower($subsection));
				if (!$subsection)
					$subsection = $section;
				$fields_structure[$section][] = $subsection;
			}
		}
		return $fields_structure;
	}

	/**
	 * Find section for specified subsection
	 */
	function findSection($subsection)
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs");

		$menu = $this->getMenuStructure();
		$subsection = str_replace("_", " ", strtolower($subsection));

		foreach ($menu as $section=>$subsections) {
			foreach ($subsections as $menu_subsection)
				if ($subsection==strtolower($menu_subsection))
					return str_replace("_", " ", strtolower($section));
		}

		return;
	}

	/**
	 * Returns true if it finds differences between the fields set in FORM and the ones from file (if file is not set from default fields) 
	 */ 
	function haveChanges($structure) 
	{
		Debug::func_start(__METHOD__,func_get_args(),"tabs");

		$fields_modified = false;

		$response_fields = $this->getApiFields();
		if (!$response_fields)
			return true;

		$fields = $this->applyRequestFields($response_fields, true);

		foreach ($structure as $m_section=>$data) {
			foreach ($data as $key=>$m_subsection) {
				foreach ($fields[$m_section][$m_subsection] as $param_name=>$data) {

					$paramname = str_replace(".", "_", $param_name);
					$val_req   = getparam($paramname);

					if (isset($data["display"]) && $data["display"]=='checkbox') {

						$value = $data["value"];
						if ($value!="on" && $value!="off")
							$value = ($value=="1") ? "on" : "off";

						$val_req = (getparam($paramname)) ? getparam($paramname) : "off";

					} elseif (isset($data["display"]) && ($data["display"]=='select' || $data["display"]=='select_without_non_selected'))
						$value = (isset($data[0]["selected"])) ? $data[0]["selected"] : "";

					else
						$value = isset($data["value"]) ? $data["value"] : "";

					if ($value!=$val_req) {
						$fields_modified = true;
						// exit all 3 cicles if we discover we have fields that were modified
						break 3;
					}
				}
			}
		}
		return $fields_modified;
	}

	/**
	 * Detects the invalid fields by comparing the requests fields (received from api)
	 * with the ones in the default fields structure
	 * Overrides the request invalid fields with their corresponding default values
	 */ 
	function detectInvalidFields($structure,&$request_fields,$default_fields)
	{
		Debug::func_start(__METHOD__,func_get_args(),"tabs");

		if ($this->validated_fields || getparam('action')=="database")
			return;
		$this->validated_fields = true;

		foreach ($structure as $section=>$data) {
			foreach ($data as $key=>$subsection) {
				if (isset($request_fields[$subsection])) {
					foreach ($request_fields[$subsection] as $param=>$data) {
						if (!isset($default_fields[$section][$subsection]))
							continue;
						
						// check if a value that was already set in api has changed from the default structure in the fields
						// for example a checkbox value was changed into select/text field or a select value has changed and now is not among the values allowed
						// all this will be displayed to the user as notice 
						if (isset($default_fields[$section][$subsection][$param]["display"])) {
							$display = $default_fields[$section][$subsection][$param]["display"];
							$ckbox_val = array("yes","on","no","off","true","false"); // "1"/"0" can't be tested since this could be a default value for any type of field
							if (in_array($data,$ckbox_val,true) && $display!="checkbox" && !in_array($param, $this->skip_special_params)) {
								//this array will keep the parameters that have checkboxes allowed values but they are not displayed as checkboxes
								$this->detected_invalidities_message .= "In $section, $subsection, the parameter: $param=$data. Automatically mapped to: ";
								if ($default_fields[$section][$subsection][$param]["display"] == "select" ||
								    $default_fields[$section][$subsection][$param]["display"] == "select_without_non_selected") {
									$this->detected_invalidities_message .= $default_fields[$section][$subsection][$param][0]["selected"]. "</br>";
									$request_fields[$subsection][$param] = $default_fields[$section][$subsection][$param][0]["selected"];
								} else {
									$this->detected_invalidities_message .= $default_fields[$section][$subsection][$param]["value"]. "</br>";
									$request_fields[$subsection][$param] = $default_fields[$section][$subsection][$param]["value"];
								}
								continue;//so that the value will not be changed with the one set in API fields

								// check if the values of select field were changed from the api ones
							} elseif ($display == 'select' || $display == 'select_without_non_selected') {
								//if (isset($default_fields[$section][$subsection][$param][0][0]) && isset($default_fields[$section][$subsection][$param][0][0][$param."_id"])) {
								if (isset($default_fields[$section][$subsection][$param][0][0]) && 
								    is_array($default_fields[$section][$subsection][$param][0][0]) && 
								    array_key_exists($param."_id", $default_fields[$section][$subsection][$param][0][0])) {
									
									foreach ($default_fields[$section][$subsection][$param][0] as $id=>$param_val){
										if (isset($param_val[$param."_id"]))
											$allowed_values[] = $param_val[$param."_id"];
									}
									$new_data = $this->buildParticularParam($data, $param, $request_fields);
									if (!in_array($new_data, $allowed_values) && !in_array("Custom", $allowed_values)) {
										$def_value = (isset($default_fields[$section][$subsection][$param][0]["selected"])) ? $default_fields[$section][$subsection][$param][0]["selected"] : "Not selected";
										$this->detected_invalidities_message .= "In $section, $subsection, the parameter: $param=$data. Automatically mapped to: ". $def_value ."</br>";
										$request_fields[$subsection][$param] = $def_value;
										continue;

									}
								    } elseif (!in_array($data, $default_fields[$section][$subsection][$param][0]) &&
									      !in_array("Custom",$default_fields[$section][$subsection][$param][0])) {
									$def_value = (isset($default_fields[$section][$subsection][$param][0]["selected"])) ? $default_fields[$section][$subsection][$param][0]["selected"] : "Not selected";
									$this->detected_invalidities_message .= "In $section, $subsection, the parameter: $param=$data. Automatically mapped to: ". $def_value ."</br>";
									$request_fields[$subsection][$param] = $def_value;
									continue;
								}
							}
						}
					}
				}
			}
		}
	}

	/**
	 * Sets Calibration fields into request fields format
	 * to be send to API
	 */ 
	function setCalibrationFields(&$fields)
	{
		Debug::func_start(__METHOD__,func_get_args(),"tabs");

		$request_fields = array();
		if (isset($fields["calibration"])) {
			$request_fields["general"] = $fields["calibration"];
			unset($fields["calibration"]);
		}
		return $request_fields;
	}

	/**
	 * Retrieves Calibration fields from API response and 
	 * store them into fields values
	 */ 
	function storeCalibrationFields(&$response_fields, &$res)
	{
		Debug::func_start(__METHOD__,func_get_args(),"tabs");

		if (isset($response_fields["calibrate"])) {
			$cal = $response_fields["calibrate"];
			if (isset($cal["general"])) {
				$cal = $cal["general"];
				unset($cal["updated"], $cal["locked"], $cal["mode"]);
			}
			$res["calibration"] = $cal; 
		}
	}

	/** Graphical methods */

	/**
	 * Display form fields with their values.
	 */
	function editForm($section, $subsection, $error=null, $error_fields=array())
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs");

		$structure       = $this->buildConfSections();

		$response_fields = $this->getApiFields();
		$default_fields  = $this->getDefaultFields();

		//if the params are not set in ybts get the default values to be displayed
		if (!$response_fields) { 
			$fields = $default_fields;
		} else {
			$this->detectInvalidFields($structure,$response_fields,$default_fields);
			$fields = $this->applyRequestFields($response_fields);
		}
		
		if (strlen($this->error)) {//get fields value from getparam
			foreach ($structure as $m_section=>$params) {
				foreach ($params as $key=>$m_subsection) {
					foreach ($fields[$m_section][$m_subsection] as $param_name=>$data) {

						$paramname = str_replace(".", "_", $param_name);
						if (isset($data["display"]) && ($data["display"]=="select" || $data["display"]=="select_without_non_selected")) {
							$fields[$m_section][$m_subsection][$param_name][0]["selected"] = getparam($paramname);
							if (getparam($paramname)=="Custom") {
								$fields[$m_section][$m_subsection]["custom_".$param_name]["value"] = getparam("custom_".$param_name);
								$fields[$m_section][$m_subsection]["custom_".$param_name]["column_name"] = "";
							}
						}

						elseif (isset($data["display"]) && $data["display"]=="checkbox")
							$fields[$m_section][$m_subsection][$param_name]["value"] = (getparam($paramname)=="on") ? "1" : "0";

						elseif (isset($data["display"]) && in_array($data["display"], array("message","objtitle")))
							continue;

						else
							$fields[$m_section][$m_subsection][$param_name]["value"] = getparam($paramname);

						// unmark triggered fields if they are set
						if (getparam($param_name) && isset($data["triggered_by"])) {

							$trigger_names = $this->trigger_names;
							if (!isset($trigger_names) && !is_array($trigger_names)) {
								Debug::trigger_report("critical", "Class variable trigger_names is not array.");
								return array(false);
							}
						
							$trigger_name = $trigger_names[$m_subsection];

							if (isset($fields[$m_section][$m_subsection][$param_name]["triggered_by"]) && ctype_digit($fields[$m_section][$m_subsection][$param_name]["triggered_by"])) {

								$triggered_by   = $fields[$m_section][$m_subsection][$param_name]["triggered_by"];
								$former_trigger = $triggered_by - 1;
								
								if (isset($fields[$section][$subsection][$trigger_name.$former_trigger]))
									unset($fields[$section][$subsection][$trigger_name.$former_trigger]);

								$fld = $fields[$m_section][$m_subsection];

								foreach ($fld as $fldn=>$fldd) {
									if (isset($fldd["triggered_by"]) && $fldd["triggered_by"]==$triggered_by) {
										unset($fields[$m_section][$m_subsection][$fldn]["triggered_by"]);
									}
								}

								unset($fields[$m_section][$m_subsection][$param_name]["triggered_by"]);
							}
						}

					}
				}
			}
		}
		
		if (strlen($this->detected_invalidities_message))
			print "<div class=\"notice\">Detected invalid values<br/>".$this->detected_invalidities_message."</div>";

		print "<div id=\"err_$subsection\">";
		if (!isset($fields[$section][$subsection]))
			print "Could not find parameters for section '$section', subsection'$subsection'";
		error_handle($error, $fields[$section][$subsection], $error_fields);
		print "</div>";
		start_form();
		foreach ($structure as $m_section=>$data) {
			foreach ($data as $key=>$m_subsection) {
				$style = ($m_subsection==$subsection) ? "" : "style='display:none;'";
				
				print "<div id=\"$m_subsection\" $style>";
				if (!isset($fields[$m_section][$m_subsection])) {
					print "Could not retrieve parameters for $m_subsection. Looking for section $m_section, subsection $m_subsection";
					print "</div>";
					continue;
				}
				editObject(null, $fields[$m_section][$m_subsection], "Set parameters values for section [$m_subsection].");
				print "</div>";
			}
		}
		addHidden("database");
		end_form();
	}


	/**
	 * Displayes the explanation of each subsection. 
	 */
	function sectionDescriptions()
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs");

		$subsection   = $this->current_subsection;
		$section_desc = $this->getSectionsDescription();

		foreach ($section_desc as $subsect=>$desc) {
			$desc = str_replace("\n","<br/>",$desc);
			$style = (isset($section_desc[$subsection]) && $subsect==$subsection) ? "" : "style='display:none;'";
			print "<div id=\"info_$subsect\" $style>". $desc ."</div>";
		}
	}

	/**
	 * Build Tabbed Menu and submenu
	 */
	function tabbedMenu()
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs");

		$current_section    = $this->current_section;
		$current_subsection = $this->current_subsection;
		$structure          = $this->getMenuStructure();

		$i = 0;
		foreach ($structure as $section_name=>$subsections) {
			if ($i>=$this->open_tabs)
				break;
			$first_sections[] = strtolower($section_name);
			$i++;
		}

		//Create the js variables used in open/close menu structure
		?>
		<script type="text/javascript">
			var open_tabs = <?php print $this->open_tabs;?>;
			var subsections = new Array();
			var sect_with_subsect = new Array();
			var main_subsections = {};
			var subsections_advanced = new Array();
<?php

		foreach ($this->subsections_advanced as $k=>$subsect_name) {
			$subsect_name = str_replace(" ", "_", strtolower($subsect_name));
			echo "subsections_advanced[" . $k++ . "]=\"" . $subsect_name . "\";";
		}
		$i = $j = 0; 
		foreach ($structure as $j_section=>$j_subsections) {
			$j_section = str_replace(" ", "_", strtolower($j_section));
			if (count($j_subsections)) {
				echo "sect_with_subsect[\"" . $j . "\"]='" . $j_section . "';";
				echo "main_subsections[\"$j_section\"]='" . str_replace(" ", "_",strtolower($j_subsections[0])) . "';";
				$j++;
			} else { 
				echo "subsections[\"" . $i . "\"]='" . $j_section . "';";
				echo "main_subsections[\"$j_section\"]='" . $j_section . "';";
				$i++;
			}

			foreach ($j_subsections as $key=>$j_subsection) {
				if (isset($j_subsection)) {
					$j_subsection = str_replace(" ", "_", strtolower($j_subsection));
					echo "subsections[\"" . $i . "\"]='" . $j_subsection . "';";
					$i++;
				}  
			}
		}

		?>	
		</script>
		<!-- Create MENU -->
		<table class="<?php print $this->menu_css; ?>" cellspacing="0" cellpadding="0">
		<tr>
		<?php

		$i = 0;
		$total = count($structure);
		$button_pos = $this->open_tabs-1; // position of Advanced/Basic button

		foreach ($structure as $menu=>$submenu) {

			$menu_name = $menu;
			$menu      = str_replace(" ", "_", strtolower($menu));
			$subsect   = str_replace(" ", "_", strtolower($menu));
			$css       = ($menu==$current_section) ? "open" : "close";

			print "<td class='menu_$css' id='section_$i' onclick=\"show_submenu_tabs($i, $total, '$subsect')\"";
			// If section that should be opened is the first one then it's ok to hide advanced
			if ($i>=$this->open_tabs && in_array($current_section,$first_sections))
				print "style='display:none;'";
			print ">" . str_replace(" ","&nbsp;",$menu_name) . "</td>";

			print "<td class='menu_space'>&nbsp;</td>";

			if ($i==($total-1)) {
				print "<td class='menu_empty'>&nbsp;</td>";
			}
			if ($i==$button_pos && in_array($current_section,$first_sections)) {
				// If section that should be opened is the first ones then it's ok to hide advanced and show the Advanced button
				print "<td class='menu_toggle' id='menu_toggle' onclick='toggle_menu();'>Advanced >></td>";
				print "<td class='menu_fill' id='menu_fill'>&nbsp;</td>";
				print "<td class='menu_space'>&nbsp;</td>";
			} elseif ($i==$button_pos) {
				// Otherwise show them and add the Basic button
				print "<td class='menu_toggle' id='menu_toggle' onclick='toggle_menu();'> << Basic </td>";
				print "<td class='menu_fill' id='menu_fill' style='display:none;'>&nbsp;</td>";
				print "<td class='menu_space'>&nbsp;</td>";
			}

			$i++;
		}
		?>
		</tr>
		</table>
		</td>
		</tr>
		<tr><td class="submenu" id="submenu_line" colspan="2">
		    <!-- Create SUBMENU -->
		<table class="submenu" cellspacing="0" cellpadding="0">
		<tr> <td>
<?php
		$subsections_advanced = $this->subsections_advanced;
		$css = "open";
		$i = 0;
		foreach ($structure as $menu=>$submenu) {
			$menu = str_replace(" ", "_", strtolower($menu));
			if ($menu==$current_section) 
				$style = "";
			else
				$style = "style='display:none;'";
			if (count($submenu)<1) {
				$i++;
				continue;
			}
			print "<div class='submenu' id='submenu_$i' $style>";

			foreach ($submenu as $key=>$name) {
				$link = str_replace(" ", "_", strtolower($name));
				if ($link==$current_subsection) {
					$css = "open";
				} else
					$css = "close";
				$hide_subsect = "";
				if (in_array($name, $subsections_advanced) && (in_array($current_section, $first_sections) && $css=="close"))
					$hide_subsect = "style='display:none;'";

				print "<div class='submenu_$css' id=\"tab_$link\" $hide_subsect onclick=\"show_submenu_fields('$link')\">" . $name . "</a></div>";
				print "<div class='submenu_space' id=\"space_$link\" $hide_subsect >&nbsp;</div>";
			}
			$i++;
			print '</div>';
		}
		?>
		</td></tr>
		</table>
		<?php
	}

	// Main method to call to display editing form organized in tabs and subtabs
	function displayTabbedSettings()
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs");

		?>
		<table class="page" cellspacing="0" cellpadding="0">
		<tr>
		    <td class="menu" colspan="2"><?php $this->tabbedMenu();?></td>
		<tr>
		    <td class="content_form"><?php $this->editForm($this->current_section, $this->current_subsection); ?></td>
		    <td class="content_info"><?php $this->sectionDescriptions(); ?></td>
		</tr>
		<tr><td class="page_space" colspan="2"> &nbsp;</td></tr>
		</table>
		<?php
	}

	// Apply form result and display form with succes message or error/warnings
	function applyFormResults()
	{
		Debug::func_start(__METHOD__, func_get_args(), "tabs");

		global $module;

		$structure = $this->buildConfSections();

		$fields = array();
		$this->warnings = "";
		$this->error    = "";

		$warning_fields = array();
		$error_fields = array();
		foreach ($structure as $m_section=>$data) {
			foreach ($data as $key=>$m_subsection) {

				$this->error_field = array();
				$this->warning_field = array();

				Debug::xdebug($module,"Preparing to validate subsection $m_subsection");
				$res = $this->validateFields($m_section, $m_subsection);

				foreach ($this->warning_field as $key=>$err) {
					$this->warnings .=  $err[0]."<br/>";
					$warning_fields[] =  $err[1];
				}
				foreach ($this->error_field as $key=>$err) {
					$this->error .=  $err[0]."<br/>";
					$error_fields[] =  $err[1];
				}

				if (!$res[0]) {
					$this->current_section    = $m_section;
					$this->current_subsection = $m_subsection;
					if (!strlen($this->title))
						Debug::trigger_report("critical", "Implementation error must set title in class that extends TabbedSettings.");	
					$_SESSION[$this->title]["section"]      = $m_section;
					$_SESSION[$this->title]["subsection"]   = $m_subsection;
					break;
				} else {
					$fields = array_merge($fields, $res["request_fields"]);  
				}

			}
			if (strlen($this->error))
				break;
		}

		if (!strlen($this->error)) {
			// see if there were changes
			$fields_modified = $this->haveChanges($structure);
			if ($fields_modified) {
				$res = $this->storeFormResult($fields);
				if (!$res[0])
					$this->error = $res[1];
			}
		}

		?>
		<table class="page" cellspacing="0" cellpadding="0">
		<tr>
		    <td class="menu" colspan="2"><?php $this->tabbedMenu();?>
		<tr> 
			<td class="content_form"><?php 
			if (strlen($this->error)) {
				if (strlen($this->warnings))
					message("Warning! " . $this->warnings, "no");
				$this->editForm($this->current_section, $this->current_subsection, $this->error, $error_fields);
			} else {
				$this->cleanSession();
				$message = (!$fields_modified) ? "Finished editing sections. Nothing to update." : "Finished applying configuration.";
				print "<div id=\"notice_" . $this->current_subsection . "\">";
				message($message, "no");
				print "</div>";

				if (strlen($this->warnings))
					message("Warning! " . $this->warnings,"no");

				$this->editForm($this->current_section, $this->current_subsection);
			}
		?></td>
		    <td class="content_info"><?php $this->sectionDescriptions(); ?></td>
		</tr>
		<tr><td class="page_space" colspan="2"> &nbsp;</td></tr>
		</table>
		<?php
	}
}

?>
