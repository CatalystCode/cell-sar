#!/usr/bin/env bash

# chplmn.sh: a script to automate a TCP connection to the yate rmanager 
# and chagne the BTS's PLMN
# 
# Cellular Search and Rescue - Cellular Sensor BTS
#   Copyright (C) 2017 Microsoft
# 
# This file is part of cell-sar/the Yate-BTS Project http://www.yatebts.com
# 
# cell-sar is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
# 
# cell-sar is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with cell-sar.  If not, see <http://www.gnu.org/licenses/>.

nc -z localhost 5038
if [ $? -eq 0 ]; then
   printf "mbts cellid $1 $2\nmbts reloadPLMN\n" | nc -q 5 localhost 5038 > /dev/null
fi

