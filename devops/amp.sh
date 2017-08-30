#!/usr/bin/env bash

# Make sure the blade is connected
bladeRF-cli -p 1>/dev/null 2>&1
if [ $? -ne 0 ]; then
   echo "No bladeRF devices connected." >&2
   exit 1
fi

# Enable the XB300
echo "enabling XB300 Amplifier"
bladeRF-cli -e "xb 300 enable"

# Turn on PA
bladecmd="xb 300 pa $1"
echo $bladecmd
bladeRF-cli -e "$bladecmd"

# Turn on LNA
bladecmd="xb 300 lna $1"
echo $bladecmd
bladeRF-cli -e "$bladecmd"

