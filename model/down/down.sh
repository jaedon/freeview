#!/bin/bash

# dowh.sh
#
# 2016-06-01: Ver 0.11, by khwoo
# - add command branch_info for branch source codes.
#
# 2016-06-01: Ver 0.10, by khwoo
# - add command cp for branch source codes.
#
# 2015-10-22: Ver 0.9, by khwoo
# - add "include" command to seperate download script for each components.
#
# 2009-11-20: Ver 0.6, by kimkb
# - add command, cleanup, revert, local_info and server_info for local directory.
#
# 2008-12-01: Ver 0.5, by dssonds
# - down: drr (distributed repository reference) initital script version for distributed repository reference
#

RED='\033[0;31m'
BLACK='\033[0;30m'
DEFAULT='\e[0m'    

rm -rf .error_occured_tag

./_down.sh $1 $2 $3 $4 $5

if [ -f .error_occured_tag ]; then
	echo -e "\n#   ${RED}ERROR!!${DEFAULT} Cannot complete your request. Please check error messages above.\n"
else
	echo -e "\n#   Completed your request.\n"
fi

rm -rf .error_occured_tag
