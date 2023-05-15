#!/bin/bash

# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

maDir=`pwd`

outDir=$maDir/memanalyzer_result
netflixDir=`cd ../../src/platform/gibbon && pwd`
categoryFile=$maDir/summary_generation/configurations/nrdp41_conf.txt

pushd .

if [ -n "$1" ]; then
   outDir=$1
fi

if [ -n "$2" ]; then
   categoryFile=$2
fi

if [ -n "$3" ]; then
   netflixDir=$3
fi

cd $maDir
rm -rf $outDir
gnome-terminal -e "./rss_info_generation/ma_rss_info_retriever.py -d $outDir"
sleep 1
gnome-terminal -e "./ma_rss_monitor_driver.sh $outDir $categoryFile"

cd $netflixDir
#LD_PRELOAD=$maDir/rss_info_generation/libmemanalyzer.so ./netflix --telnet --nbpd -i
#./netflix --telnet --nbpd -i
./netflix

echo ""
echo "##########################################################"
echo "To redraw the result, execute the following command"
#echo "cd $maDir;./ma_rss_monitor_driver.sh $outDir $categoryFile;cd -"
echo "./ma_rss_monitor_driver.sh $outDir $categoryFile"
echo "#########################################################"
echo ""


