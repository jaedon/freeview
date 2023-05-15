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
# $brcm_Workfile: vce_runner.sh $
# $brcm_Revision: Hydra_Software_Devel/4 $
# $brcm_Date: 5/8/12 5:21p $
#
# [File Description:]
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/portinginterface/vce/tests/vce_runner.sh $
# 
# Hydra_Software_Devel/4   5/8/12 5:21p nilesh
# SW7425-2568: Copy vdc_test script to results directory
# 
# Hydra_Software_Devel/3   3/27/12 1:05p nilesh
# SW7425-2568: Old results directory is deleted if one exists
# 
# Hydra_Software_Devel/2   3/27/12 12:57p nilesh
# SW7425-2568: Results directory is now recursively created
# 
# Hydra_Software_Devel/1   3/12/12 4:23p nilesh
# SW7425-2568: VCE Test Runner Script
# 
#############################################################################

testbin=$1
cfg=$2
outpath=$3

if [ $# -ne 3 ]
then
	echo "Usage: vce_runner <test bin> <test cfg> <output path>"
	exit -1
fi

rm -rf $3
mkdir -p $3
cd $3
$testbin < $cfg &> console.txt 
cp $cfg .