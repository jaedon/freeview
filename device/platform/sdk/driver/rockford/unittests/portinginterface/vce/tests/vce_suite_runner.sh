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
# $brcm_Workfile: vce_suite_runner.sh $
# $brcm_Revision: Hydra_Software_Devel/11 $
# $brcm_Date: 9/19/12 1:47p $
#
# [File Description:]
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/portinginterface/vce/tests/vce_suite_runner.sh $
# 
# Hydra_Software_Devel/11   9/19/12 1:47p nilesh
# SW7425-2568: Delete results from STB after being copied to server
# 
# Hydra_Software_Devel/10   5/22/12 3:13p nilesh
# SW7425-2568: Added sleep calls after sync
# 
# Hydra_Software_Devel/9   5/22/12 9:46a nilesh
# SW7425-2568: sync between tests
# 
# Hydra_Software_Devel/8   5/17/12 11:19p nilesh
# SW7425-2568: Copy test binary to local HDD
# 
# Hydra_Software_Devel/7   5/10/12 11:16p nilesh
# SW7425-2568: chmod 777 the server results so they can be deleted later
# 
# Hydra_Software_Devel/6   5/9/12 4:58p nilesh
# SW7425-2568: Use server copy of test suite if one exists
# 
# Hydra_Software_Devel/5   5/8/12 5:21p nilesh
# SW7425-2568: Copy results to server incrementally
# 
# Hydra_Software_Devel/4   5/7/12 10:49a nilesh
# SW7425-2568: Server path is created if it doesn't exist
# 
# Hydra_Software_Devel/3   3/28/12 12:26p nilesh
# SW7425-2568: Added optional copy of results to server
# 
# Hydra_Software_Devel/2   3/27/12 1:05p nilesh
# SW7425-2568: Fixed empty line handling
# 
# Hydra_Software_Devel/1   3/27/12 12:56p nilesh
# SW7425-2568: VCE Suite Runner
# 
#############################################################################

testbin=$1
testsuite=$2
outpath=$3
serverpath=$4

if [ $# -lt 3 ]
then
	echo "Usage: vce_suite_runner <test bin> <test suite> <output path> [server path]"
	exit -1
fi

rm -rf $outpath
mkdir $outpath
echo "Copying $testbin binary to STB"
cp $testbin $outpath/vdc_test
testbin=$outpath/vdc_test

if [ -n "$serverpath" ]; then
    rm -rf $serverpath/*
    mkdir -p $serverpath
    cp $testsuite $serverpath/
    testsuite=$serverpath/$testsuite
    chmod 777 -R $serverpath
    sync;sleep 2
fi

while read line; do
    if [ -n "$line" ]; then # Skip empty lines
        if [ "${line:0:1}" == "#" ]; then # Skip comment lines
            echo "Running:" ${line:1}
        else # Run specified script
            cfg="$line"".vdc_test"
            echo "Running:" ${line}
            ./vce_runner.sh $testbin $(pwd)"/$cfg" $outpath"/$line"           
            if [ -n "$serverpath" ]; then
#              echo "Copying $outpath/$line to $serverpath/$line"
                mkdir -p $serverpath/$line
                cp -R $outpath/$line/* $serverpath/$line/
                chmod 777 -R $serverpath/$line
                sync;sleep 2
                rm -rf $outpath"/$line"/*
            fi
        fi
    fi
done < $testsuite

if [ -n "$serverpath" ]; then
    chmod 777 -R $serverpath
fi
