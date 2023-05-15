#!/bin/bash
# /*++
# Microsoft (r) PlayReady (r)
# Copyright (c) Microsoft Corporation. All rights reserved.
# --*/

#
# This is a simple BASH script for running the PK tests on Linux.
#
###################################################################################################

# Set this to something, anything, if you want to see some extra debugging output
__DEBUG__=

# Optional Parameters
PKRUNAREA=$1
PKRUNNUMBER=$2

# Add \r to the Internal Field Separators variable because we will be parsing some DOS/Windows style text files
IFS+=$(echo -en "\r")

PKDIR=~/PlayReady
PKTESTDIR=$PKDIR/test
PKTESTEXEDIR=$PKDIR/bin/exe
PKTESTRUNDIR=~/Test/PlayReady

# variables used in the files 'filedep.txt'
if [ -d $PKDIR/pregenerated ]; then
    _DRM_OEM_FILES=$PKDIR/pregenerated
else if [ -d $PKDIR/Samples ]; then
    _DRM_OEM_FILES=$PKDIR/Samples
else
    echo -e "\n*ERROR*: Could not locate the pregenerated sample files necessary for testing; exiting."
    exit 1
fi
fi
_DRM_REFCODE_TEST_FILES=$PKDIR/test/common

if [ $__DEBUG__ ]; then 
    echo "PKDIR                   = $PKDIR"
    echo "PKTESTDIR               = $PKTESTDIR"
    echo "PKTESTEXEDIR            = $PKTESTEXEDIR"
    echo "PKTESTRUNDIR            = $PKTESTRUNDIR"
    echo "_DRM_OEM_FILES          = $_DRM_OEM_FILES"
    echo "_DRM_REFCODE_TEST_FILES = $_DRM_REFCODE_TEST_FILES"
fi

echo -e "\nDeleting all old test results..."
rm -r -f $PKTESTRUNDIR

echo -e "\nRunning tests..."

cd $PKTESTDIR/testrunner/linux

cat testareas.txt | while read PKTESTAREA PKTESTEXE; do

    if [[ ! $PKTESTAREA ]]; then
        continue   # blank line
    fi

    if [[ $PKTESTAREA = \#* ]]; then
        continue   # comment line
    fi

    # Run only the specified test area, if one was specified
    if [ $PKRUNAREA ]; then
        if [ $PKTESTAREA != $PKRUNAREA ]; then
            continue;
        fi
    fi

    PKTESTAREADIR=$PKTESTDIR/$PKTESTAREA
    PKTESTRUNSUBDIR=$PKTESTRUNDIR/$PKTESTAREA
    PKTESTRUNSUBDIRDRM=$PKTESTRUNDIR/$PKTESTAREA/DRM

    if [ $__DEBUG__ ]; then 
        echo -e "\nPKTESTAREA         = $PKTESTAREA"
        echo      "PKTESTEXE          = $PKTESTEXE"
        echo      "PKTESTAREADIR      = $PKTESTAREADIR"
        echo      "PKTESTRUNSUBDIR    = $PKTESTRUNSUBDIR"
        echo      "PKTESTRUNSUBDIRDRM = $PKTESTRUNSUBDIRDRM"
    fi

    echo -e "\nPreparing test area: $PKTESTAREA"
    cd $PKTESTAREADIR

    if [ -d $PKTESTRUNSUBDIR ]; then
        rm -r -f $PKTESTRUNSUBDIR    # Remove the directory if it already exists
    fi
    mkdir --parents $PKTESTRUNSUBDIRDRM       # Create all the directories via the deepest path
    mkdir --parents $PKTESTRUNSUBDIR/files/   # Create all the directories via the deepest path (some test areas want files here)
    mkdir --parents $PKTESTRUNSUBDIR/data/    # Create all the directories via the deepest path (some test areas want files here)

    # Copy all the necessary files to the test execution directories
    perl -pi -e 's|\\|/|g' filedep.txt          # change all back-slashes (Windows/DOS path separator) to forward-slashes (Unix path separator)
    cat filedep.txt | while read ONEFILE; do
        if [ $__DEBUG__ ]; then echo $ONEFILE ; fi
        ONEFILE=`echo $ONEFILE | tr "()" "{}"`  # change any ()s to {}s [so what they contain will be seen as a variable]
        if [ $__DEBUG__ ]; then echo $ONEFILE ; fi
        ONEFILE=$(eval echo $ONEFILE)           # evaluate/expand any variables in the file path
        if [ $__DEBUG__ ]; then echo $ONEFILE ; fi
        
        if [ -e $ONEFILE ]; then
            cp $ONEFILE $PKTESTRUNSUBDIR
            cp $ONEFILE $PKTESTRUNSUBDIR/files/
            cp $ONEFILE $PKTESTRUNSUBDIR/data/
        fi
    done

    cp $PKTESTEXEDIR/$PKTESTEXE $PKTESTRUNSUBDIR
    
    cp $PKTESTDIR/testrunner/common/result-transform.xsl $PKTESTRUNSUBDIR
    cp $PKTESTDIR/testrunner/common/result-style.css     $PKTESTRUNSUBDIR

    # Actually run the tests
    LOGFILE=${PKTESTEXE}_stdout.log
    echo "Running $PKTESTRUNSUBDIR/$PKTESTEXE $PKRUNNUMBER -drmpath $PKTESTRUNSUBDIRDRM -drt -bvt -func -perf > $LOGFILE"
    cd $PKTESTRUNSUBDIR
    ./$PKTESTEXE $PKRUNNUMBER -drmpath $PKTESTRUNSUBDIRDRM -drt -bvt -func -perf > $LOGFILE

done

# Process the results
echo -e "\nProcessing all test results..."
cd $PKTESTDIR/testrunner/linux
perl ./processresults.pl testareas.txt $PKTESTRUNDIR > TestResultSummary.html
xdg-open ./TestResultSummary.html

exit 0
