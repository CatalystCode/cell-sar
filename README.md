# Cellular Search and Rescue

This repository holds all program code for the sensor component, drone communications, and client application used to drive the Cellular Search and Rescue system.

### Getting Started / Documentation
A great place to get started is the documentation in the [project wiki](https://github.com/CatalystCode/cell-sar/wiki). This also contains reference materials for the configuration files and automation extensibility framework.

### Components
##### /bts
The "bts" folder contains Yate and YateBTS, along with their patches to support the Nuand bladeRF x40. This software is required to run the cellular base station software on the cellular sensor.

##### /ocp
The "ocp" folder contains the application responsible for managing the lifecycle of the BTS, as well as encoding and transporting commands and event data between the operator's controller software and the BTS.

##### /client
The "client" folder contains the controller application to fly the drone, send commands to the BTS, and interpret events from the cellular sensor.

### Licensing
Please note this project is released under multiple licenses:
* All BTS components (Yate, YateBTS, modifications to either project, and associated enhancements to this software) are licensed exclusively under GPLv2. More details and relevant licenses can be found in bts\README.md.

* All DJI components (SDK and associated libraries) are copyright (c) 2014-Present DJI. All rights reserved. More details and relevant licenses can be found in client\README.md and ocp\README.md.

* All other components are licensed by the authors under an MIT license. More details and relevant licenses can be found in client\README.md and ocp\README.md

### Disclaimer
This product is provided as-is; configuring, deploying, and operating it is done at the risk and responsibility of the end user (operator). By invoking or reusing any part of this software, it is understood that the operator has procured any and all required licenses and approvals from the relevant law enforcement and/or regulatory bodies for their region.
