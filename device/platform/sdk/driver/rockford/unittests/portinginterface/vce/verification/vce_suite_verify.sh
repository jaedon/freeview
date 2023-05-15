#!/bin/bash          

#############################################################################
#
#       Copyright (c) 2003-2012, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
# 
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: vce_suite_verify.sh $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 9/19/12 1:45p $
#
# [File Description:]
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/portinginterface/vce/verification/vce_suite_verify.sh $
# 
# Hydra_Software_Devel/2   9/19/12 1:45p nilesh
# SW7425-2568: Save err codes to csv file for analysis
# 
# Hydra_Software_Devel/1   3/28/12 12:27p nilesh
# SW7425-2568: Added vce suite verifification script
# 
#############################################################################

testsuite=$1
outpath=$2

if [ $# -ne 2 ]
then
	echo "Usage: vce_suite_verify <test suite> <output path>"
	exit -1
fi

rm -rf $outpath/errcode.csv

while read line; do    
    if [ -n "$line" ]; then # Skip empty lines
        if [ "${line:0:1}" == "#" ]; then # Skip comment lines
            echo "Verifying:" ${line:1}
        else # Run specified script
            echo "Verifying:" ${line}
            ./vce_verify.py -csvpath $outpath"/$line"
            echo $? >> $outpath/errcode.csv
        fi
    fi
done < $testsuite

