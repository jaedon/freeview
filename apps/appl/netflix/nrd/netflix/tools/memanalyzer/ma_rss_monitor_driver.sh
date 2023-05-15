#!/bin/bash

# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

if [ ! -d "$1" ]; then
   echo "The directory $logDir does not exist. Exiting..."
   exit
fi

# set category configuration file
if [ ! -z "$2" ]; then
   categoryFile="$2"
else
   categoryFile="./summary_generation/configurations/nrdp41_conf.txt"
fi

# convert logDir to absolute path
logDir=`cd $1 && pwd`

# set GnuPlot script file name
outGnuplotScript=${logDir}.gp

# enable this for older gnuplot (e.g. gnuplot 4.2)
#wxtSizeSupport="--nowxtsizesupport"
wxtSizeSupport=""

# Generate the summary for the first time
./summary_generation/ma_rss_summary_generator.py -d $logDir --categoryfile=$categoryFile

# wait for some results to populate
#sleep 5

# Generate gnuplot script
./gp_script_generation/gp_script_generator.py -s $logDir/summary/finalSummary.txt -o $outGnuplotScript -p ${logDir}.png -i 1 --width=1900 --height=600 $wxtSizeSupport

# launch the gnuplot in a background

echo "load '$outGnuplotScript'" | gnuplot &


while (true)
do
    # Generate the summary
    ./summary_generation/ma_rss_summary_generator.py -d $logDir --categoryfile=$categoryFile
    sleep 2
done


