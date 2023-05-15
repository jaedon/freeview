#! /usr/local/bin/python

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
# $brcm_Workfile: vce_verify.py $
# $brcm_Revision: Hydra_Software_Devel/16 $
# $brcm_Date: 9/26/12 3:25p $
#
# [File Description:]
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/portinginterface/vce/verification/vce_verify.py $
# 
# Hydra_Software_Devel/16   9/26/12 3:25p nilesh
# SW7425-2568: Added itfp gimme logic
# 
# Hydra_Software_Devel/15   9/24/12 4:51p nilesh
# SW7425-2568: Handle ITFP cases
# 
# Hydra_Software_Devel/14   9/6/12 11:07a nilesh
# SW7425-2568: Fix typo
# 
# Hydra_Software_Devel/13   9/6/12 11:05a nilesh
# SW7425-2568: Report non-zero error code if userdata tests fail.
# 
# Hydra_Software_Devel/12   5/24/12 11:07p nilesh
# SW7425-2568: Hunt for correct userdata files
# 
# Hydra_Software_Devel/11   5/23/12 3:15p nilesh
# SW7425-2568: Added logic to allow 30fps deltaDTS in 60fps transcode
# mode
# 
# Hydra_Software_Devel/10   5/23/12 12:46p nilesh
# SW7425-2568: Sync with test gen script
# 
# Hydra_Software_Devel/9   5/23/12 12:34p nilesh
# SW7425-2568: Added more prints
# 
# Hydra_Software_Devel/8   5/10/12 11:22p nilesh
# SW7425-2568: Added 97435 file output support. Fixed CC verification
# when no CC data encoded
# 
# Hydra_Software_Devel/7   5/10/12 1:56p nilesh
# SW7425-2568: Fixed CC verification to check that results is a subset of
# golden
# 
# Hydra_Software_Devel/6   5/10/12 1:21p nilesh
# SW7425-2568: Fixed 708 verification script
# 
# Hydra_Software_Devel/5   5/9/12 4:40p nilesh
# SW7425-2568: Fix verification of 708 data when IBBP GOP is used
# 
# Hydra_Software_Devel/4   5/8/12 5:21p nilesh
# SW7425-2568: Added userdata verification
# 
# Hydra_Software_Devel/3   5/7/12 3:22p nilesh
# SW7425-2568: Fixed status verification script
# 
# Hydra_Software_Devel/2   4/3/12 3:44p nilesh
# SW7425-2568: Added basic frame rate verification
# 
# Hydra_Software_Devel/1   3/12/12 4:22p nilesh
# SW7425-2568: VCE Test Verification Script
# 
#############################################################################

import sys
import argparse
import csv
import math
import os
import itertools

frameRateLUT = {"1":24.0,#*1000/1001
                "2":24.0,
                "3":25.0,
                "4":30.0,#*1000/1001
                "5":30.0,
                "6":50.0,
                "7":60.0,#*1000/1001
                "8":60.0,
                "9":15.0,#*1000/1001
                "10":7.5,#*1000/1001
                }
                
def verify_userdata(configList,userdataGoldenList,userdata608List,userdata708List): # Verifies the frame rate of the encoded video is correct
    print "verify userdata"
    # Split userdata into scte20,atsc53,scte21,
    userdataGoldenSCTE20List = [(int(entry["CC Data[0]"]),int(entry["CC Data[1]"])) for entry in userdataGoldenList if entry["Analog"] == "1" and entry["Format"] == "1"]
    userdataResultSCTE20List = [(int(entry["CC Data[0]"]),int(entry["CC Data[1]"])) for entry in userdata608List if entry["Format"] == "1"]

    userdataGoldenATSC53List = [(int(entry["CC Data[0]"]),int(entry["CC Data[1]"])) for entry in userdataGoldenList if entry["Analog"] == "1" and entry["Format"] == "2"]
    userdataResultATSC53List = [(int(entry["CC Data[0]"]),int(entry["CC Data[1]"])) for entry in userdata608List if entry["Format"] == "2"]

    if ( configList[0]["b frames"] != "0" ):
        print "ignoring 708 padding for B-frame encode"
        userdataGolden708List =    [(int(entry["CC Data[0]"]),int(entry["CC Data[1]"])) for entry in userdataGoldenList if entry["Analog"] == "0" and entry["CC Valid"] == "1"]
        userdataResult708List =    [(int(entry["CC Data[0]"]),int(entry["CC Data[1]"])) for entry in userdata708List if entry["CC Valid"] == "1"]
    else:
        userdataGolden708List =    [(int(entry["CC Data[0]"]),int(entry["CC Data[1]"])) for entry in userdataGoldenList if entry["Analog"] == "0"]
        userdataResult708List =    [(int(entry["CC Data[0]"]),int(entry["CC Data[1]"])) for entry in userdata708List]
    
    maxFieldDropped = 6
    
    bSCTE20Pass = False
    if ( (0 == len(userdataResultSCTE20List)) and (0 != len(userdataGoldenSCTE20List) ) ):
        print "FAIL: SCTE20 data missing" 
    elif ( 0 != len(userdataGoldenSCTE20List)):
        maxDropped = 2*maxFieldDropped
        for i,j in itertools.product(range(0,maxDropped),repeat=2):
            if ( userdataGoldenSCTE20List[i:len(userdataResultSCTE20List)+i-j] == userdataResultSCTE20List[j:]):
                bSCTE20Pass = True
                break
        if ( False == bSCTE20Pass ):
#            for i,entry in enumerate(userdataGoldenSCTE20List):
#                print userdataGoldenSCTE20List[i],userdataResultSCTE20List[i],(userdataGoldenSCTE20List[i] == userdataResultSCTE20List[i]) 
            print "FAIL: SCTE20 userdata mismatch"
    else:  
#        print "No SCTE20 data"
        bSCTE20Pass = True
    
    bATSC53Pass = False
    if ( (0 == len(userdataResultATSC53List)) and (0 != len(userdataGoldenATSC53List) ) ):
        print "FAIL: ATSC53 data missing" 
    elif ( 0 != len(userdataGoldenATSC53List) ):
        maxDropped = 2*maxFieldDropped
        for i,j in itertools.product(range(0,maxDropped),repeat=2):
            if ( userdataGoldenATSC53List[i:len(userdataResultATSC53List)+i-j] == userdataResultATSC53List[j:]):
                bATSC53Pass = True
                break
        if ( False == bATSC53Pass ):
#            for i,entry in enumerate(userdataGoldenATSC53List):
#                print userdataGoldenATSC53List[i],userdataResultATSC53List[i],(userdataGoldenATSC53List[i] == userdataResultATSC53List[i])
            print "FAIL: ATSC53 userdata mismatch"
    else:
#        print "No ATSC53 data"
        bATSC53Pass = True

    b708Pass = False
    if ( (0 == len(userdataResult708List)) and (0 != len(userdataGolden708List) ) ):
        print "FAIL: 708 data missing" 
    elif ( 0 != len(userdataGolden708List) ):
        maxDropped = 9*maxFieldDropped
        for i,j in itertools.product(range(0,maxDropped),repeat=2):
            if ( userdataGolden708List[i:len(userdataResult708List)+i-j] == userdataResult708List[j:]):
                b708Pass = True
                break
        if ( False == b708Pass ):
#            for i,entry in enumerate(userdataGolden708List):
#                print userdataGolden708List[i],userdataResult708List[i],(userdataGolden708List[i] == userdataResult708List[i])
            print "FAIL: 708 userdata mismatch"
    else:
#        print "No 708 data"
        b708Pass = True
            
    return ( bSCTE20Pass and bATSC53Pass and b708Pass )
    
def verify_framerate(configList,descList): # Verifies the frame rate of the encoded video is correct
    print "verify framerate"
    # Generate a list of frame start descriptors
    frameList = [desc for desc in descList if desc["dts valid"] == "1" and desc["opts valid"] == "1"]
    # Generate a list of original PTS (normalized to 90Khz)
    originalPTSList = [float(desc["opts"])*2 for desc in frameList]
    # Generate a list of new PTS
    newPTSList = [float(desc["pts"]) for desc in frameList]
    
    deltaOriginalPTSList = map(lambda x,y: y-x,originalPTSList[:-1],originalPTSList[1:])
    deltaOriginalPTSin90Khz = (math.fsum(deltaOriginalPTSList)/len(deltaOriginalPTSList))    
    deltaPTSDigitalFrameRates = map(lambda x:90000/x,set(frameRateLUT.itervalues()));
    if deltaOriginalPTSin90Khz in deltaPTSDigitalFrameRates:
        bAnalogSource = False;
    else:
        bAnalogSource = True;        
    
    # Determine the descriptor index this config is valid until
    prevConfigIndex = -1
    configIndex = 0
    
    if len(configList) > configIndex + 1:
        descriptorEndIndex = int(configList[configIndex + 1]["count"])
    else:
        descriptorEndIndex = None;    
        
    targetDTSList = []
    errorCount = 0
    errorCountMax = 0
    for descriptorIndex in range(1,len(frameList)):
        if descriptorEndIndex != None:
            # Check to see if we've processed the last descriptor for this config
            if ( int(frameList[descriptorIndex]["count"]) >= descriptorEndIndex ):
                configIndex += 1
                configDescStartIndex = int(configList[configIndex]["count"])
                if len(configList) > configIndex+1:
                    descriptorEndIndex = int(configList[configIndex+1]["count"])
                else:
                    descriptorEndIndex = None
            
        # Determine target deltaDTS for this config        
        if prevConfigIndex != configIndex:
            targetDTSList = []
            frameRateList = []
            itfpMultiplierList = [1]
            frameRateList.append(configList[configIndex]['frame rate'])
            # Handle possible 60-->30 cases, so allow 30fps if 60fps specified for encode
            if frameRateList[0] in ["7","8"]:
                itfpMultiplierList.append(2)
                # Handle possible 60-->24 cases, so allow 24fps if 60fps specified for encode and ITFP is enabled
                if (configList[configIndex]['itfp'] == "1"):
                    itfpMultiplierList.append(3)
            if frameRateList[0] in ["4","5"]:
                if (configList[configIndex]['itfp'] == "1"):
                    # Handle possible 60 -> 30/15 cases when ITFP enabled
                    frameRateList.append("9")
            if (configList[configIndex]['itfp'] == "1"):
                # if ITFP enabled, the deltaDTS may stutter while ITFP is locking phase.  
                # So, ignore up to 3 errors when ITFP is enabled 
                errorCountMax = 3
            if configList[configIndex]['frame rate mode'] != "0":
                print "FAIL: unsupported frame rate mode"
                sys.exit(-1)
            for frameRate in frameRateList: 
                if frameRate in frameRateLUT:
                    for itfpMultiplier in itfpMultiplierList:
                        targetDTS = 90000/frameRateLUT[frameRate]
                        targetDTS *= itfpMultiplier
                        if bAnalogSource == True:
                            targetDTS *= 1001
                            targetDTS /= 1000
                        targetDTSList.append(math.floor(targetDTS))
                        targetDTSList.append(math.ceil(targetDTS))
                else:
                    print "FAIL: unrecognized frame rate"
                    sys.exit(-1)
                
            prevConfigIndex = configIndex
            

        deltaDTS = float(frameList[descriptorIndex]["dts"]) - float(frameList[descriptorIndex-1]["dts"])
        if deltaDTS not in targetDTSList:
            errorCount += 1
            print "WARN: deltaDTS[",descriptorIndex,"] does not match frame rate", deltaDTS, targetDTSList, float(frameList[descriptorIndex]["dts"]), float(frameList[descriptorIndex-1]["dts"])
            if errorCount > errorCountMax:
                print "FAIL: Max error count reached"
                return False

    return True
            
#
# Main
#
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Verifies VCE behavior by analyzing CSV output generated when VCE is compiled with BVCE_TEST_MODE=y")
    parser.add_argument("-csvpath")
    args = vars(parser.parse_args())
    results = []
    
    configPath = args["csvpath"] + "/BVCE_CONFIG_00_00_000.csv"
    statusPath = args["csvpath"] + "/BVCE_STATUS_00_00_000.csv"
    descPath = args["csvpath"] + "/BVCE_BUFFER_DESC_00_00_000.csv"
    
    if not (os.path.exists(configPath) and os.path.exists(statusPath) and os.path.exists(descPath)):
        configPath = args["csvpath"] + "/BVCE_CONFIG_01_00_000.csv"
        statusPath = args["csvpath"] + "/BVCE_STATUS_01_00_000.csv"
        descPath = args["csvpath"] + "/BVCE_BUFFER_DESC_01_00_000.csv"
    
        if not (os.path.exists(configPath) and os.path.exists(statusPath) and os.path.exists(descPath)):
            print "FAIL: Required CSV files are missing"
            sys.exit(-1)
        
    configIter = csv.DictReader(open(configPath, "rU"))
    configList = list(configIter)
    descIter = csv.DictReader(open(descPath,"rU"))
    descList = list(descIter)
    statusIter = csv.DictReader(open(statusPath,"rU"))
    statusList = list(statusIter)
    
    for row in statusList:
        # Verify that the drop due to error ids zero
        if row["dropped (err)"] != '0':      
            print "FAIL: dropped (err) is non-zero"
            sys.exit(-1)
        if row["count"] != '0':
            if row["received"] == '0':
                print "FAIL: received is zero"
                sys.exit(-2)
            if row["encoded"] == '0':
                print "FAIL: encoded is zero"
                sys.exit(-3)
    
    results.append(verify_framerate(configList,descList))
    
    bVerifyUserdata = False
    userdataGoldenPath = args["csvpath"] + "/BXUDLIB_USERDATA_INPUT_RAW.csv"
    if (os.path.exists(userdataGoldenPath)):
        userdata608Path = args["csvpath"] + "/userdata_608_[00][00].csv"
        userdata708Path = args["csvpath"] + "/userdata_708_[00][00].csv"
        if (os.path.exists(userdata608Path) and os.path.exists(userdata708Path)):
            bVerifyUserdata = True
        else:
            userdata608Path = args["csvpath"] + "/userdata_608_[01][00].csv"
            userdata708Path = args["csvpath"] + "/userdata_708_[01][00].csv"
            if (os.path.exists(userdata608Path) and os.path.exists(userdata708Path)):
                bVerifyUserdata = True

    if ( True == bVerifyUserdata ):                
        userdataGoldenList = list(csv.DictReader(open(userdataGoldenPath, "rU")))
        userdata608List = list(csv.DictReader(open(userdata608Path, "rU")))
        userdata708List = list(csv.DictReader(open(userdata708Path, "rU")))
        results.append(verify_userdata(configList, userdataGoldenList, userdata608List, userdata708List))
        
    if ( True == reduce(lambda x,y: x and y, results)):
        print "PASS: OK"        
        sys.exit(0)
    else:
        sys.exit(-4)
    
# Scan current directory for CSV files
#    Determine number of encode channels
#    For each encode channel
#       Determine number of config changes
#       For each config
#          Verify descriptors row match config
#               protocol
#                  h264/mpeg4
#                     verify NALU detection
#               frame rate/mode
#                  verify deltaPTS is correct
#               bitrate
#                  verify tpb/shr is correct
#                  verify length does not exceed next ESCR
#               GOP structure
#                  Verify number of P/B frames between I frames is correct
#                  Verify B frames by looking at PTS vs DTS
#          Verify status

#def verify_desc_XXX(descDict,currentXXX,previousXXX=None,startCount=0,endCount=0):
        
        
        
    
