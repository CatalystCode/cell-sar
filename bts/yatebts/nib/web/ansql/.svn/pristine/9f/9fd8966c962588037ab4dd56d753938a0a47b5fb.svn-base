<?php

function get_default_fields_enb()
{
	global $server_name, $request_protocol;

$enodeb_params = array(
/* "section" => array(
	"param" => array(  // value to be used in display_pair()
		$value,
		"display"  =>"", 
		"comment"  =>
		"validity" => ""
    )*/
"radio" => array(
"enodeb" => array(
    // this configurations will be sent in the 'basic' section when actually sending request to API
    // they were grouped here because they are unique per enodeb or per enodeb in overlapping coverage area

    "enodebId" => array(
	"column_name" => "eNodeB Id",
	"comment" =>"eNodeB ID
Unique to every eNodeB in the network.
Must be 5 hex digits for a Macro ENB or 7 hex digits for a Home ENB",	
	"required" => true,
	"validity"=> array("check_valid_enodebid")
	),

    "MCC" => array(
	"comment" => "Mobile Country Code part of the PLMN Identity.
The same for every eNodeB in the network.",
	"required" => true,
	"validity" => array("check_field_validity", false, false,"^[0-9]{3}$")
    ),

    "MNC" => array(
	"comment" => "Mobile Network Code part of the PLMN Identity.
The same for every eNodeB in the network.",
	"required" => true,
	"validity" => array("check_field_validity", false, false,"^[0-9]{2,3}$")
    ),

    "TAC" => array(
	"comment" => "Tracking Area Code
4 digit hex value. Ex: 2A9F
This value must be set. There is no default.",
	"required" => true,
	"validity" => array("check_valid_tac") //array("check_field_validity", 0, 65535)
    ),

    "CellIdentity" => array(
	"column_name" => "Cell Identity",
	"comment" => "Must be 7 hex digits
Ex: 0000001",
	"required" => true,
	"validity" => array("validate_cell_identity")
    ),

    "Name" => array(
	"comment" =>"Human readable eNB Name that is sent to the MME during S1-MME setup.
According to 3GPP 36.413, this parameter is optional. 
If it is set, the MME may use it as a human readable name of the eNB. 
See paragraphs 8.7.3.2 and 9.1.8.4 of the above referenced specification.",
	"validity" => array("check_field_validity", false, false, "^[a-zA-Z0-9 ]+$")
    ),

    "Band" => array(
	array("1","2","3","4","5","6","7","8","9","10","11","12","13","14","17","18","19","20","21","22","23","24","25","26","27","28","29","30","31","32","33","34","35","36","37","38","39","40","41","42","43","44","selected"=> "1"),
	"display" => "select_without_non_selected",
	"required" => true,	
	"comment" => 'Band selection ("freqBandIndicator" in SIB1)
In most systems, this is set by the hardware type and should not be changed.'
    ),

    "Bandwidth" => array(
	array("selected"=>"25", "6","15","25","50","75","100"),
	"display" => "select_without_non_selected",
	"comment" => format_comment("
Bandwidth is the LTE radio channel BW, in number of resource blocks
(in the frequency domain). The allowed values and the corresponding
bandwidth (in MHz) is listed in the following table:
   Value (N RBs)  Bandwidth (MHz)
   -------------  ---------------
   6              1.4
   15             3
   25             5
   50             10
   75             15
   100            20
 A simple formula for calculating the bandwidth in MHz for a given
 number of RBs is: MHz BW = (N_RB * 2) / 10, except for 6 RBs.
 Probably the same for all eNodeBs in a network that are operating in the same band.

Example: Bandwidth = 25"),
	"required" => true
    ),

    "EARFCN" => array(
	"comment" => format_comment('Downlink ARFCN
Uplink ARFCN selected automatically based on this downlink.
Must be compatible with the band selection.
Probably the same for all eNodeBs in a network that are operating in the same band.
Some handy examples of downlink EARFCNs:
EARFCN 900, 1960 MHz, Band 2 ("PCS1900")
EARFCN 1575, 1842.5 MHz, Band 3 ("DCS1800")
EARFCN 2175, 2132,5 MHz, Band 4 ("AWS-1")
EARFCN 2525, 881.5 MHz, Band 5 ("850")
EARFCN 3100, 2655.0 MHz, Band 7 ("2600")
EARFCN 5790, 740.0 MHz, Band 17 ("700 b")
EARFCN 6300, 806.0 MHz, Band 20 ("800 DD")
EARFCN 36100, 1910.0 MHz, Band 33 ("IMT/1900")
EARFCN 38000, 2595.0 MHz, Band 38 ("TD 2600")

Debug EARFCN extension: 2400 MHz @ EARFCN 50000 offset; valid range: 50000 - 50959
Some handy examples for WiFi \'extended EARFCNs\':
EARFCN 50120, 2412 MHz, Channel 1 (2.4 GHz WiFi)	2401 - 2423 MHz
EARFCN 50170, 2417 MHz, Channel 2 (2.4 GHz WiFi)	2404 - 2428 MHz
EARFCN 50220, 2422 MHz, Channel 3 (2.4 GHz WiFi)	2411 - 2433 MHz
EARFCN 50270, 2427 MHz, Channel 4 (2.4 GHz WiFi)	2416 - 2438 MHz
EARFCN 50320, 2432 MHz, Channel 5 (2.4 GHz WiFi)	2421 - 2443 MHz
EARFCN 50370, 2437 MHz, Channel 6 (2.4 GHz WiFi)	2426 - 2448 MHz
EARFCN 50420, 2442 MHz, Channel 7 (2.4 GHz WiFi)	2431 - 2453 MHz
EARFCN 50470, 2447 MHz, Channel 8 (2.4 GHz WiFi)	2436 - 2458 MHz
EARFCN 50520, 2452 MHz, Channel 9 (2.4 GHz WiFi)	2441 - 2463 MHz
EARFCN 50570, 2457 MHz, Channel 10 (2.4 GHz WiFi)	2451 - 2468 MHz
EARFCN 50620, 2462 MHz, Channel 11 (2.4 GHz WiFi)	2451 - 2473 MHz
EARFCN 50670, 2467 MHz, Channel 12 (2.4 GHz WiFi)	2456 - 2478 MHz
EARFCN 50720, 2472 MHz, Channel 13 (2.4 GHz WiFi)	2461 - 2483 MHz
EARFCN 50840, 2484 MHz, Channel 14 (2.4 GHz WiFi)	2473 - 2495 MHz'),
	"required" => true,
	"validity" => array("validate_earfcn_band", "Band"),
    ),

    "__" => array(
	"display" => "objtitle",
	"value" => format_comment("Physical Layer Cell ID 
Phy Cell ID = 3*NID1 + NID2,
NID1 is 0..167
NID2 is 0..2
This gives a phy cell id range of 0..503
The combination 3*NID1+NID2 should never be the same for cells with overlapping coverage.
Some parameters are to have default values derived from physical layer cell id.
These are: 
On Access Channels in PRACH screen: RootSequenceIndex
On Access Channels in PUSCH screen: groupAssignmentPUSCH
On Access Channels in PUSCH screen: cyclicShift
On Access Channels in PUCCH screen: n1Pucch-An
")
    ),

    "NID1" => array(
	"required" => true,
	"comment"  => "NID1 is between 0 and 167",
	"required" => true,
	"validity" => array("check_field_validity", 0, 167),
	"javascript" => "onchange='set_cellid_dependencies();'" 
    ),
    "NID2" => array(
	array("0", "1", "2"),
	"display" => "select_without_non_selected",
	"required" => true,
	"javascript" => "onchange='set_cellid_dependencies();'"
    ),

    "CrestFactor" => array(
	array("5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20", "selected"=>"12"),
	"display" => "select_without_non_selected",
	"comment" => "Estimated crest factor in dB, 5 to 20, default 12.
Larger value gives higher power output at risk of more interference to adjacent channels."
    ),

    "DistributedVrbs" => array(
	"value" => false,
	"column_name" => "Distributed Virtual Resource Blocks",
	"display" => "checkbox",
	"comment" => "Distributed Virtual Resource Blocks
Allowed values: false for localized or true for distributed type.
This option improves multipath performance,
but limits resource allocations to 16 RBs (2.88 MHz) in systems
with LTE bandwidth of > 5 MHz.
Default is false.",
    )
),
"calibration" => array(
	"auto_calibration" => array(
	"display" => "checkbox",
	"value" => "off",
	"comment" => "Enable auto calibration.
This parameter is applied on reload."
    ),
	"freqoffs_calibration" => array(
	"display" => "checkbox",
	"value" => "off",
	"comment" => "Enable frequency offset calibration.
This parameter is applied on reload."
    )
)
),

"core" => array(

"gtp" => array(

	"error_get_network" => array(
		"display" => "message",
		"column_name"=> "",
	    "value" => ""
	),	
    "addr4" => array(
	    "comment" => "IPv4 address to use with the eNodeB tunnel end",
	),

    "addr6" => array(
		"comment" => "IPv6 address to use with the eNodeB tunnel end",
	),
),

"mme" => array(

    "__" => array(
	"value" => "Hand-configured MME",
	"display" => "objtitle"
    ),

    "mme_address" => array(
	"column_name" => "Address",
	"comment" => "Ex: 192.168.56.62",
    ),
    "local" => array(
	"comment" => "Ex: 192.168.56.1",
    ),
    "streams" => array(
	"comment" => "Ex: 5",
    ),
    "dscp" => array(
	"column_name" => "DSCP",
	"comment" => "Ex: expedited"
    ),
    "add_mme_1" => array("value"=>"<a id=\"add_mme_1\" class='llink' onclick=\"fields_another_obj(2, 'add_mme_');\">Add another MME</a>", "display"=>"message"),


    "objtitle2" => array("display"=>"objtitle", "triggered_by"=>"2", "value"=>"2<sup>nd</sup> MME"),

    "mme_address_2" => array(
	"column_name" => "Address",
	"comment" => "Ex: 192.168.56.62",
	"triggered_by" => "2",
    ),
    "local_2" => array(
	"column_name" => "Local",
	"comment" => "Ex: 192.168.56.1",
	"triggered_by" => "2",
    ),
    "streams_2" => array(
	"column_name" => "Streams",
	"comment" => "Ex: 5",
	"triggered_by" => "2",
    ),
    "dscp_2" => array(
	"column_name" => "DSCP",
	"comment" => "Ex: expedited",
	"triggered_by" => "2"
    ),
    "add_mme_2" => array("value"=>"<a id=\"add_mme_2\" class='llink' onclick=\"fields_another_obj(3, 'add_mme_');\">Add another MME</a>", "display"=>"message", "triggered_by" => "2"),

    "objtitle3" => array("display"=>"objtitle", "triggered_by"=>"3", "value"=>"3<sup>rd</sup> MME"),

    "mme_address_3" => array(
	"column_name" => "Address",
	"comment" => "Ex: 192.168.56.62",
	"triggered_by" => "3",
    ),
    "local_3" => array(
	"column_name" => "Local",
	"comment" => "Ex: 192.168.56.1",
	"triggered_by" => "3",
    ),
    "streams_3" => array(
	"column_name" => "Streams",
	"comment" => "Ex: 5",
	"triggered_by" => "3",
    ),
    "dscp_3" => array(
	"column_name" => "DSCP",
	"comment" => "Ex: expedited",
	"triggered_by" => "3"
    ),
    "add_mme_3" => array("value"=>"<a id=\"add_mme_3\" class='llink' onclick=\"fields_another_obj(4, 'add_mme_');\">Add another MME</a>", "display"=>"message", "triggered_by" => "3"),

    "objtitle4" => array("display"=>"objtitle", "triggered_by"=>"4", "value"=>"4<sup>th</sup> MME"),

    "mme_address_4" => array(
	"column_name" => "Address",
	"comment" => "Ex: 192.168.56.62",
	"triggered_by" => "4",
    ),
    "local_4" => array(
	"column_name" => "Local",
	"comment" => "Ex: 192.168.56.1",
	"triggered_by" => "4",
    ),
    "streams_4" => array(
	"column_name" => "Streams",
	"comment" => "Ex: 5",
	"triggered_by" => "4",
    ),
    "dscp_4" => array(
	"column_name" => "DSCP",
	"comment" => "Ex: expedited",
	"triggered_by" => "4"
    ),
    "add_mme_4" => array("value"=>"<a id=\"add_mme_4\" class='llink' onclick=\"fields_another_obj(5, 'add_mme_');\">Add another MME</a>", "display"=>"message", "triggered_by" => "4"),

    "objtitle5" => array("display"=>"objtitle", "triggered_by"=>"5", "value"=>"5<sup>th</sup> MME"),

    "mme_address_5" => array(
	"column_name" => "Address",
	"comment" => "Ex: 192.168.56.62",
	"triggered_by" => "5",
    ),
    "local_5" => array(
	"column_name" => "Local",
	"comment" => "Ex: 192.168.56.1",
	"triggered_by" => "5",
    ),
    "streams_5" => array(
	"column_name" => "Streams",
	"comment" => "Ex: 5",
	"triggered_by" => "5",
    ),
    "dscp_5" => array(
	"column_name" => "DSCP",
	"comment" => "Ex: expedited",
	"triggered_by" => "5"
    )
),

"s1ap" => array(
)

),

"hardware" => array(
"site_info" => array(
    "location" => array(
	"column_name" => "Site location",
	"comment" => "No default is provided
Latitude, longitude
ddd.dddddd,ddd.dddddd format",
	"validity" => array("check_valid_geolocation")
    ),
    "siteName" => array(
	"column_name" => "Site name"
    ),
    "antennaDirection" => array(
	"column_name" => "Antenna direction",
	"comment" => "The direction the antenna is pointed, in degrees from true north.
No default is provided",
	"validity" => array("check_valid_float")
    ),
    "antennaBeamwidth" => array(
	"value" => "360",
	"column_name" => "Antenna beam width",
	"comment" => "Default is omni, 360 degrees",
	"validity" => array("check_field_validity", 1, 360)
    ),
    "reportingPeriod" => array(
	"value" => 15,
	"column_name" => "Reporting period",
	"comment" => "Reporting period in minutes for HW status. 
Default is 15 minutes",
	"validity" => array("check_valid_integer")
    ),
),
"site_equipment" => array(
    "antenna_type"=> array(
	"comment" => "String"
    ),
    "antenna_serial_number"=> array(
	"comment" => "String"
    ),
    "antenna_cable_type"=> array(
	"comment" => "String"
    ),
    "antenna_cable_length"=> array(
	"comment" => "String"
    ),
    "power_suply_type"=> array(
	"comment" => "String"
    ),
    "power_suply_serial_number"=> array(
	"comment" => "String"
    ),
    "custom_parameters" => array(
	"display" => "textarea",
	"comment" => "Custom parameters that will be stored on the equipment. 
They are not used in configuration but are kept for informative purpose.
They are stored in key=value form. The keys should not match any of the above keys.
Ex:
antenna_serial_number2=342134
antenna_cable_length2=0.5
"
    ),
),
"shutdown" => array(
    "maxVswr" => array(
	"value" => "1.5",
	"comment" => "VSWR level for automatic shutdown of the power amplifier if the RF feed is damaged.
Default is 1.5",
	"validity" => array("check_valid_float")
    ),
    "amplifierShutdownTemp" => array(
	"value" => 80,
	"comment" => "Transistor temperature in deg C for safety shutdown of the power amplifier. 
Default is 80.",
	"validity" => array("check_field_validity", 1, 85)
    ),
    "amplifierRestartTemp" => array(
	"value" => 70,
	"comment" => "Default is 70.",
	"validity" => array("check_field_validity", 1, 85)
    ),
    "powerSupplyShutdownTemp" => array(
	"value" => 85,
	"comment" => "Transformer temperature in deg C for safety shutdown of power supply sections.
Default is 85",
	"validity" => array("check_field_validity", 1, 85)
    ),
    "powerSupplyRestartTemp" => array(
	"value" => 75,
	"comment" => "Default 75.",
	"validity" => array("check_field_validity", 1, 85)
    ),
    "softwareShutdownTemp" => array(
	"value" => 100,
	"comment" => "CPU core temperature for shutdown of the SatSite
If the temperature exceeds this level, all digital radio processing functions are suspended. Default 100.",
	"validity" => array("check_field_validity", 1, 100)
    ),
    "softwareRestartTemp" => array(
	"value" => 80,
	"comment" => "Default 80",
	"validity" => array("check_field_validity", 1, 85)
    )
),
),

"system" => array(
"system_information" => array(
    // System Information repetition parameters and Paging parameters
    // This params will be sent in "basic" section when sending request to API

    "SiWindowLength" => array(
	array("1","2","5","10","15","20","40", "selected"=>"20"),
	"display" => "select_without_non_selected",
	"comment" => "RRC System Information Window Length in milliseconds (subframes)."
    ),

    "SiPeriodicity" => array(
	array("8","16","32","64","128","256","512", "selected"=>"8"),
	"display" => "select_without_non_selected",
	"comment" => "Scheduler System Information Periodicity in frames."
    ),

    "SiRedundancy" => array(
	array("1","2","3","4","5","6","7","8", "selected"=> "2"),
	"display" => "select_without_non_selected",
	"comment" => "Scheduler SI Redundancy. Should be larger for cell with large coverage area."
    ),

    "defaultPagingCycle" => array(
	array("32","64","126","256","selected"=>"32"),
	"display" => "select_without_non_selected",
	"comment" => "RRC default paging cycle",
    ),
    
    "nB" => array(
	array("0", "32", "64", "128", "256", "512", "1024", "2048", "4096", "selected"=>"512"),
	"display" => "select_without_non_selected",
	"comment" => "RRC paging nB parameter x1000"
    ),
		
    "TxGain2" => array(
	array("0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "selected"=>"25"),
	"display" => "select_without_non_selected",
	"comment" => "TX post-mixer gain in dB.
\"Naked\" BladeRFor Lab Kit: 25 dB.
SatSite: 11 dB"
    ),
    
    "RxGain1" => array(
	array("5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "selected"=>"5"),
	"display" => "select_without_non_selected",
	"comment" => "Rx pre-mixer gain in dB.
\"Naked\" BladeRF or Lab Kit: 5 dB"
    ),
    
    "RxGain2" => array(
	array("0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "selected"=>"5"),
	"display" => "select_without_non_selected",
	"comment" => "Rx post-mixer gain in dB.
\"Naked\" BladeRF or Lab Kit: 5 dB"
    ),
    
    'referenceSignalPower' => array(
	"value" => -20,
	"comment" => 'Total power transmitted in CSRS.
Actual value in dBm, range -50 to +60
Depends on hardware, crest factor adjustment and TX gain settings
"Naked" BladeRF, default settings: -34
LabKit: -20
SatSite: 20',
	"validity" => array("check_field_validity",-60,50)
    ),
    
    "activity_timeout" => array(
	"value" => 10000,
	"comment" => "Timeout interval (in milliseconds) for detecting user inactivity on radio connection 
Defaults to 10 seconds.",
	// tried to guess possible validation. No validation in code
	"validity" => array("check_field_validity", 500, 500000)
    ),
    
    "T302" => array(
	"value" => 5,
	"comment" => "T302: integer: Value of RRC T302 timer in seconds
Provided by the enodeB to the UE in a RRC connection reject
Defaults to 5 seconds.",
	"validity" => array("check_valid_integer")
    )
),

"scheduler" => array(

	"SpecialRntiCodeEfficiency" => array(
		 "value" => "0.0625",
		 "comment" => "SpecialRntiCodeEfficiency: float: The bits/element for DL special RNTI messages (SIBs, paging, RAR).
Default 0.0652.
NOTE: This value will be increased automatically if a message can not be scheduled.",
	 	 "validity" => array("check_SpecialRntiCodingEfficiency")
	 ),
	"SpecialDCI" => array(
		array("dci1a","dci1c","selected"=>"dci1c"),
		"display" => "select_without_non_selected",
		"comment" => "The DCI format for special RNTI's RA-RNTI, P-RNTI, SI-RNTI",
		"column_name" => "DCI for S-RNTI"
	),
    
	"retxBSR-Timer" => array(
		array("sf320", "sf640", "sf1280", "sf2560", "sf5120", "sf10240", "selected"=>"sf1280"),
		"display" => "select_without_non_selected",
		"comment" => "Retransmission BSR timer
After how many subframes the UE should send a new BSR should it receive no allocation.
Value in subframes.
Default is sf1280."
	),

	"periodicBSR-Timer" => array(
		array("sf5", "sf10", "sf16", "sf20", "sf32", "sf40", "sf64", "sf80", "sf128", "sf160", "sf320", "sf640", "sf1280", "sf2560", "infinity", "selected"=>"infinity"),
		"display" => "select_without_non_selected",
		"comment" => "Periodic BSR timer
At which interval UE should send BSR even if it has no new data.
Value is in subframes.
Default is infinity."
	),
    
	"GapType" => array(
		array("1","2","selected"=>"1"),
		"display" => "select_without_non_selected",
		"comment" => "Parameter Ngap from DCI, currently set as system wide configuration; corresponds to Ngap,1/Ngap,2"
	),
)
),


"access_channels" => array(

"pusch" => array(
    // This params will be sent in "basic" section when sending request to API

    "groupAssignmentPUSCH"  => array(
	array("0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29", "selected" => "0"),
	"display" => "select_without_non_selected",
	"comment" => 'PUSCH group assignment, delta-SS in 36.211 5.5.2.1.1
Cells with overlapping coverage should have different values.
Default 0.
Derived from physical CellID: physical cellid % 30'
    ),
    
    "cyclicShift" => array(
	array("0","1","2","3","4","5","6","7","selected"=>"0"),
	"display" => "select_without_non_selected",
	"comment" => 'Base cyclic shift for PUSCH DMRS, 36.211 5.5.2.1.1.
Default 0. 
Derived from physical CellID: physical cellid % 8 '
    ),
    
    "p0_NominalPUSCH" => array(
	"value" => -70,
	"comment" => "Expected UL RSSI power on PUSCH in dBm. 
Range: -126,24. 
Default -70. ",
	"validity" => array("check_field_validity",-126,24)
    )
),

"pucch" => array(
    // This params will be sent in "basic" section when sending request to API

    "deltaPUCCH-Shift" => array(
	array("1","2","3","selected"=>"1"),
	"display" => "select_without_non_selected",
	"comment" => 'Delta Shift ("deltaPUCCH_Shift" in SIB2). Default 1.'
    ),

    "nRB-CQI" => array(
	"value" => 3,
	"comment" => 'PUCCH resource for CQI reporting
Allowed values: 1 .. (number of RB - 2)
Default 3.',
	"validity" => array("check_field_validity",0,98)
    ),

    "nCS-An" => array(
	array("0","1","2","3","4","5","6","7","selected"=>"5"),
	"display" => "select_without_non_selected",
	"comment" => 'PUCCH cycle shift.
Allowed values: 0..7, but must also be a multiple of deltaPUCCH-Shift.
Default is 5.'
    ),

    "srPeriodicity" => array(
	array("5", "10", "20", "40", "80", "2", "1","selected"=>"10"),
	"display" => "select_without_non_selected",
	"comment" => "PUCCH Scheduling Request Periodicity in subframes. 
Default 10."
    ),

    "p0_NominalPUCCH" => array(
	"value" => -96,
	"comment" => "Expected UL RSSI power in PUCCH in dBm.
Range -127 to -96.
Default -96",
	"validity" => array("check_field_validity",-127,-96)
    ),
    
    "n1Pucch-An" => array(
	"value" => 0,
	"comment" => "Pucch resource allocation offset.
Valid range is 0 .. (2047 * number of UL resource blocks / 110)
Where number of UL resource blocks = Bandwitdh
",
	"validity" => array("check_n1PucchAn", "Bandwidth")
    )
),
"prach" => array(
    // This params will be sent in "basic" section when sending request to API
    
    "rootSequenceIndex" => array(
	"value" => "0",
	"comment" => 'PRACH Root Sequence Index
Cells with overlapping coverage should have different values.
Allowed values 0..837.
Default value taken from PHY CID.',
	"validity" => array("check_field_validity", 0, 837)
    ),

    "prach-FreqOffset" => array(
	"value" => 0,
	"comment" => 'PRACH frequency offset in the uplink resource grid.
In units of resource blocks.
Allow values are 0 .. (number of RBs - 6)
Default is 0.',
	"validity" => array("check_field_validity", 0, 94)
    ),
    
    "numberOfRA-Preambles" => array(
	array("4","8","12","16","20","24","28","32","36","40","44","48","52","56","60","64","selected"=>"4"),
	"display" => "select_without_non_selected",
	"comment" => 'Number of PRACH preambles to use.
Default 4.'
    ),
    
    "preambleInitialReceivedTargetPower" => array(
	array("-120", "-118", "-116", "-114", "-112", "-110", "-108", "-106", "-104", "-102", "-100", "-98", "-96", "-94", "-92", "-90", "selected" => "-90"),
	"display" => "select_without_non_selected",
	"column_name" => 'Initial RSSI Target',
	"comment" => 'Initial target RSSI for PRACH transmissions in dBm.
Allows values multiples of 2, -90 .. -120. Default -90.',
    ),
    
    "prach-ConfigIndex" => array(
	array("0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15", "selected"=>"0"),
	"display" => "select_without_non_selected",
	"comment" => 'PRACH configuration index
Determines how often the UE is allowed to send PRACH.
3GPP 36.211 Table 5.7.1-2 (FDD) or Tables 5.7.1-3, 4 (TDD)'
    ),
    
    "zeroCorrelationZoneConfig" => array(
	array("0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","selected"=>"7"),
	"display" => "select_without_non_selected",
	"comment" => 'PRACH zero correlation zone configuration
Larger cell radius requires larger values.
3GPP 36.211 5.7.2-2
Allowed values: 0, 1..15.
Values 1..15 correspond to cell radius of 0.8 - 58.9 km, but it is not a linear range; see the table.
Value 0 is a special case that forces each PRACH preamble to have a unique root sequence index,
which allows for any cell radius.
Default value of 7 (4.4 km)'
    ),
    
    "PrachThreshold-dB" => array(
	array("15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30","selected"=>"20"),
	"display" => "select_without_non_selected",
	"comment" => "PRACH post-filter SNR detection threshold in dB
Negative threshold enables a debugging feature that steps up the threshold with each detection event.
Allowed range 15-30 dB.
Default is 20 dB."
    )
),

"pdcch" => array(
    "CFI" => array(
	array("1","2","3","selected"=>"2"),
	"display" => "select_without_non_selected",
	"comment" => "Control format indicator (from PCFICH)
Controls number of symbols used for PDCCH."
    ),

    "Ng" => array(
	array("oneSixth", "half", "one", "two", "selected"=>"oneSixth"),
	"display" => "select_without_non_selected",
	"column_name" => "PHICH Ng factor",
	"comment" => "PHICH Ng factor (from MIB in PBCH)
Determines bandwidth used for PHICH, affects available bandwidth for PDCCH."
    ),

    "PdcchFormat" => array(
	array("2","3","selected"=>"2"),
	"display" => "select_without_non_selected",
	"comment" => "PdcchFormat, 3GPP 36.211 Table 6.8.1-1
Aggregation level in PDCCH is 2^format.
Default is 2."
    ),

)
),


"developers" => array(

"general" => array(

    "mode" => array(
	"value" => "ENB",
	"comment" => "Operation mode
This setting determines which control Javascript file to load.
Optional Javascript files are used for special test modes. Default ENB."
    ),

    "autostart" => array(
	"value"   => true,
	"display" => "checkbox",
	"comment" => "Start the cell operation at load time
Disabling autostart allows deferring cell startup"
    ),

    "transceiver" => array(
	"value" => true,
	"display" => "checkbox",
	"comment" => "Start the radio transceiver (TrxManager).
This setting is mainly used for testing, since most of the ENB is
driven by the radio transceiver."
    ),
),

"radio" => array(

    "radio_driver" => array(
	"comment" => 'Name of the radio device driver to use
Leave it empty to try any installed driver',
    ),
),

"uu-simulator" => array(

    "address" => array(
	"comment"=> "Example: 127.0.0.1",
	"validity"=> array("check_valid_ipaddress")
    ),
    
    "port" => array(
	"comment" => "Example: 9350",
	"validity"=> array("check_valid_integer") 
    ),

    "pci" => array(
	"comment" => "Example: 0",
	"validity"=> array("check_valid_integer")
    ),

    "earfcn" => array(
	"comment" => "Example: 0",
	"validity"=> array("check_valid_integer")
    ),

    "broadcast" => array( 
	"comment" => "Example: 127.0.0.1:9351, 127.0.0.1:9352",
	"validity"=> array("check_broadcast")
    ),

    "max_pdu" => array(
	"comment" => "Example: 128",
    )
),

"uu-loopback" => array(

    "AddrSubnet" => array(
	"comment" => "IPv4 subnet for allocating IP addresses to TUN interfaces
Ex: 10.167.227.%u",
	"validity" => array("check_field_validity", false, false, "^([0-9]{1,3}\.){3}%u$")
    ),

    "AuxRoutingTable" => array(
	"comment" => "Auxiliary routing table that is used for configuring TUN interfaces routes. 
Can be the table number or name (if it's configured in
/etc/iproute2/rt_tables)
Ex: 131",
	"validity" => array("check_auxrouting_table")
    ),

    "SymmetricMacCapture" => array(
	"display" => "checkbox",
	"comment" => "Whether to submit wireshark capture for MAC PDUs on both LteConnection instances. 
If disabled, only the LteConnection that represents the eNodeB submits wireshark capture, causing each MAC PDUs to appear only once and in the right direction (DL-SCH vs. UL-SCH)."
    ),
),

"test-enb" => array(

    "__" => array(
	"value" => "PDCCH Test",
	"display" => "objtitle"
    ),

    "PdcchTestMode" => array(
	"display" => "checkbox",
	"comment" => "Specifies if the PDCCH test mode is on or off.
PDCCH test mode sends extra messages on PDCCH."
    ),

    "___" => array(
	"value" => "PDSCH traffic simulator",
	"display" => "objtitle"
    ),

    "DownlinkTrafficGenererator" => array(
	"display" => "checkbox",
	"comment" => "Specifies if the traffic generator test mode is on or off.
The test mode generate random traffic on shared channel (PDSCH) on random resource blocks
NOTE: When test mode is enabled, regular PDSCH traffic is suppressed,included SIBs."
    ),

    "DownlinkTrafficGeneratorLoad" => array(
	"value" => "0",
	"comment" => "The PDSCH utilisation for the traffic generator, in percentages.
Allowed integer range: 0 - 100. Default value: 0."
    ),

    "DownlinkTrafficGeneratorSubframes" => array(
	"value" => "10",
	"comment" => "The number of subframes to use for traffic simulation.
Traffic simulator uses this many subframes, starting from subframe 0."
    ),
    
    "SimulatePdschTraffic" => array(
	array("0", "1023", "31", "301", "602", "selected"=>"0"),
	"display" => "select_without_non_selected",
	"comment" => "PDSCH traffic simulator
Fills free PDSCH elements with random QPSK symbols in selected subframes.
Control is a bit mask. If bit N is set in the mask, fill subframe N.
If configured, this feature is active even if PdschEnabled is false.
Default is 0 - no simulated traffic.
1023 - all 10 subframes
31 - first 5 subframes
301 - every even subframe
602 - every odd subframe"
    ),

    "____" => array(
	"value" => "Channel controls for testing
Enable and disable specific channel types.

Indicates if the named PHY channel's data can be sent on the TX grid.
Note that the RsEnabled is the master enabler for all the reference signals, currently only CSRS. 
To enable CSRS, RsEnabled must also be true.
Default is yes for all.
",
	"display" => "message"
    ),

    "PhichEnabled" => array(
	"display" => "checkbox",
	"value" => true
    ),

    "PcfichEnabled" => array(
	"display" => "checkbox",
	"value" => true
    ),

    "PssEnabled" => array(
	"display" => "checkbox",
	"value" => true
    ),

    "SssEnabled" => array(
	"display" => "checkbox",
	"value" => true
    ),

    "CsrsEnabled" => array(
	"display" => "checkbox",
	"value" => true
    ),

    "PdschEnabled" => array(
	"display" => "checkbox",
	"value" => true
    ), 

    "PdcchEnabled" => array(
	"display" => "checkbox",
	"value" => true
    ), 

    "PbchEnabled" => array(
	"display" => "checkbox",
	"value" => true
    ),

    "RsEnabled" => array(
	"display" => "checkbox",
	"value" => true
    )
),

"test-scheduler" => array(
    
    "scheduler-generator" => array(
	"value" => true,
	"display" => "checkbox",
	"comment" => "Flag which enables/disables scheduler UE traffic generator"
    ),

    "scheduler-generator.ues" => array(
	"value" => 5,
	"comment" => "Integer; The number of UE's which generates traffic"
    ),

    "scheduler-generator.qci" => array(
	"value" => "7",
	"comment" => "Integer; Downlink cqi to obtain MCS"
    ),

    "scheduler-generator.cqi" => array(
	"value" => "7",
	"comment" => "Integer; Uplink cqi to obtain MCS",
    ),

    "scheduler-generator.gbr" => array(
	"value" => 10000,
	"comment" => "Integer; UE GBR"
    ),

    "scheduler-generator.ambr" => array(
	"value" => "0",
	"comment" => "Integer; UE AMBR"
    ),

    "scheduler-generator.upload-rate" => array(
	"value" => "10000",
	"comment" => "Integer; This specifies the maximum number of bytes to be generated in a second for upload."
    ),

    "scheduler-generator.download-rate" => array(
	"value" => "10000",
	"comment" => "Integer; This specifies the maximum number of bytes to be generated in a second for download."
    )
)
)
);

	foreach ($enodeb_params as $section=>$subsections) {
		foreach ($subsections as $subs=>$data1) {
			foreach ($data1 as $paramname=>$data) {
				if (isset($data["comment"])) {
					$enodeb_params[$section][$subs][$paramname]["comment"] = str_replace(array("\t\t\t\t","\n"),array("","<br/>"), $data["comment"]);
				}
			}
		}
	}

	if (isset($_SESSION["enb_fields"]["interfaces_ips"])) {
		$interfaces_ips = $_SESSION["enb_fields"]["interfaces_ips"]["both"];
		$ipv4 = $_SESSION["enb_fields"]["interfaces_ips"]["ipv4_gtp"];
		$ipv6 = $_SESSION["enb_fields"]["interfaces_ips"]["ipv6_gtp"];

	} else {
		if (!$request_protocol)
			$request_protocol = "http";

		$url = "$request_protocol://$server_name/api.php";
		$out = array("request"=>"get_net_address","node"=>"sdr","params"=>"net_address");
		$res = make_request($out, $url);

		if ($res["code"]=="0") {

			$interfaces_ips = build_net_addresses_dropdown($res, true);
			$_SESSION["enb_fields"]["interfaces_ips"]["both"] = $interfaces_ips;

			$ipv4 = build_net_addresses_dropdown($res, true, "ipv4");
			$_SESSION["enb_fields"]["interfaces_ips"]["ipv4_gtp"] = $ipv4;
			$ipv6 = build_net_addresses_dropdown($res, true, "ipv6");
			$_SESSION["enb_fields"]["interfaces_ips"]["ipv6_gtp"] = $ipv6;

			// keep the error message in session if request 'get_net_address' failed
		} else {
			$_SESSION["enb_fields"]["error_get_net_interfaces"] = "[API: ".$res["code"]."] ".$res["message"];
		}
	}						

	if (isset($_SESSION["enb_fields"]["error_get_net_interfaces"])) {
		$enodeb_params["core"]["gtp"]["error_get_network"] = array("display"=>"message", "value"=> "<div class=\"notice\"><font class=\"error\">Error!! </font><font style=\"font-weight:bold;\">".$_SESSION["enb_fields"]["error_get_net_interfaces"]. " Please fix the error before setting the addresses.</font></div>");
	} else {
		$gtp_ipv4 = $ipv4;
		$gtp_ipv4[] = "0.0.0.0";
		$enodeb_params["core"]["gtp"]["addr4"] = array($gtp_ipv4,"display"=>"select", "comment" => "IPv4 address to use with the eNodeB tunnel end. <br/>0.0.0.0 - GTP listener on all ipv4 interfaces.");
		$gtp_ipv6 = $ipv6;
		$gtp_ipv6[] = "::";
		$enodeb_params["core"]["gtp"]["addr6"] = array($gtp_ipv6,"display"=>"select", "comment" => "IPv6 address to use with the eNodeB tunnel end. <br/>:: - GTP listener on all ipv6 interfaces.");

		$enodeb_params["core"]["mme"]["local"] = array($interfaces_ips,"display"=>"select","comment"=>"Ex: 192.168.56.1");
		$enodeb_params["core"]["mme"]["local_2"] = array($interfaces_ips,"display"=>"select","comment"=>"Ex: 192.168.56.1","column_name"=>"Local", "triggered_by" => "2");
		$enodeb_params["core"]["mme"]["local_3"] = array($interfaces_ips,"display"=>"select","comment"=>"Ex: 192.168.56.1", "column_name"=>"Local", "triggered_by" => "3");
		$enodeb_params["core"]["mme"]["local_4"] = array($interfaces_ips,"display"=>"select","comment"=>"Ex: 192.168.56.1", "column_name"=>"Local", "triggered_by" => "4");
		$enodeb_params["core"]["mme"]["local_5"] = array($interfaces_ips,"display"=>"select","comment"=>"Ex: 192.168.56.1", "column_name"=>"Local", "triggered_by" => "5");
	}

	return $enodeb_params;
}
?>
