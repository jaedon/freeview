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
# $brcm_Workfile: vce_generate_tests.py $
# $brcm_Revision: Hydra_Software_Devel/23 $
# $brcm_Date: 9/20/12 4:10p $
#
# [File Description:]
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/portinginterface/vce/tests/vce_generate_tests.py $
# 
# Hydra_Software_Devel/23   9/20/12 4:10p nilesh
# SW7425-2568: Reduced decode time on encoded cc stream to prevent
# hitting loop back scenario
# 
# Hydra_Software_Devel/22   9/18/12 1:35p delkert
# SW7425-2568: Generate test_suites.txt to match changes to EC schedule
# creation.
#
# Hydra_Software_Devel/21   5/24/12 12:39p nilesh
# SW7425-2568: Use correct STG for 97435
#
# Hydra_Software_Devel/20   5/24/12 12:25p nilesh
# SW7425-2568: Switch 97435 encode instance to 1
#
# Hydra_Software_Devel/19   5/23/12 5:08p nilesh
# SW7425-2568: Allow config of xvd and feeder instances
#
# Hydra_Software_Devel/18   5/23/12 4:54p nilesh
# SW7425-2568: Fixed VDC config for 97435
#
# Hydra_Software_Devel/17   5/23/12 2:48p nilesh
# SW7425-2568: Add variable frame rate mode
#
# Hydra_Software_Devel/16   5/16/12 9:38p nilesh
# SW7425-2568: Skip invalid tests (1080 w/ B Pictures)
#
# Hydra_Software_Devel/15   5/15/12 3:46p nilesh
# SW7425-2568: Changed 708 24 and 30 Hz streams
#
# Hydra_Software_Devel/14   5/11/12 11:08a nilesh
# SW7425-2568: Added 30Hz and 24Hz CC sources
#
# Hydra_Software_Devel/13   5/10/12 11:42p nilesh
# SW7425-2568: Dynamically generate output filename for each test based
# on the encode instance, channel and protocol
#
# Hydra_Software_Devel/12   5/10/12 11:17p nilesh
# SW7425-2568: Fixed 97435 output file
#
# Hydra_Software_Devel/11   5/10/12 5:09p nilesh
# SW7425-2568: Added 7435 support
#
# Hydra_Software_Devel/10   5/9/12 4:41p nilesh
# SW7425-2568: Skip invalid [720/1080]p60-->60fps IBBP encode
# configurations
#
# Hydra_Software_Devel/9   5/9/12 9:23a nilesh
# SW7425-2568: Add leading zeros to test name for proper sort
#
# Hydra_Software_Devel/8   5/8/12 5:20p nilesh
# SW7425-2568: Reduced cc test time to 1 minute. Added I/IBBP to gop
# test.
#
# Hydra_Software_Devel/7   5/7/12 5:07p nilesh
# SW7425-2568: Fixed typo
#
# Hydra_Software_Devel/6   5/7/12 4:40p nilesh
# SW7425-2568: Increased encode time for CC tests to 2 minutes
#
# Hydra_Software_Devel/5   5/7/12 4:35p nilesh
# SW7425-2568: Updated to new encoder command version
#
# Hydra_Software_Devel/4   5/7/12 3:21p nilesh
# SW7425-2568: Added GOP tests
#
# Hydra_Software_Devel/3   5/7/12 12:31p nilesh
# SW7425-2568: Fixed CC test output file name
#
# Hydra_Software_Devel/2   5/7/12 11:08a nilesh
# SW7425-2568: Added 1080p tests. Added CC tests.
#
# Hydra_Software_Devel/1   4/5/12 4:31p nilesh
# SW7425-2568: VCE Generate Tests script
#
#############################################################################

from string import Template
import sys
import os
import shutil
import itertools

streamList = [
              # cnnticker
              {"source_friendly_name":"cnnticker",
               "source_file":"/data/videos/cnnticker.mpg",
               "source_pid_video":"0x21",
               "source_pid_pcr":"0x21",
               "source_type":"0",
               "source_protocol":"1"
               },
              # avatar
              {"source_friendly_name":"avatar",
               "source_file":"/data/videos/vce/Common/avatar_AVC_15M.ts",
               "source_pid_video":"0x101",
               "source_pid_pcr":"0x101",
               "source_type":"0",
               "source_protocol":"0"
               }
              ]

ccStreamList = [
              # scte20
              {"source_friendly_name":"scte20",
               "source_file":"/data/videos/vce/CC/scte20only.ts",
               "source_pid_video":"0x10",
               "source_pid_pcr":"0x10",
               "source_type":"0",
               "source_protocol":"1"
               },
              # powervu
              {"source_friendly_name":"powervu",
               "source_file":"/data/videos/vce/CC/PowerVu_4-7-3.ts",
               "source_pid_video":"0x488",
               "source_pid_pcr":"0x488",
               "source_type":"0",
               "source_protocol":"1"
               },
              # 708stream
              {"source_friendly_name":"708stream",
               "source_file":"/data/videos/vce/CC/708stream.trp",
               "source_pid_video":"21",
               "source_pid_pcr":"21",
               "source_type":"0",
               "source_protocol":"1"
               },
              # cc708_30Hhz
              {"source_friendly_name":"708stream_30",
               "source_file":"/data/videos/vce/CC/708stream_xcoded_30.ts",
               "source_pid_video":"0x12",
               "source_pid_pcr":"0x11",
               "source_type":"0",
               "source_protocol":"1"
               },
              # cc708_24Hz
              {"source_friendly_name":"708stream_24",
               "source_file":"/data/videos/vce/CC/708stream_xcoded_30.ts",
               "source_pid_video":"0x12",
               "source_pid_pcr":"0x11",
               "source_type":"0",
               "source_protocol":"1"
               },
              ]

vdcPlatformDefaults = {
                       "97425": {"vdc_compositor_id":"3",
                                 "vdc_display_id":"3",
                                 "vdc_stg_id":"5",
                                 },
                       "97435": {"vdc_compositor_id":"5",
                                 "vdc_display_id":"5",
                                 "vdc_stg_id":"7",
                                 },
                       }

vdcDefaults = {
               "vdc_display_fmt":"2",
               "vdc_refresh_rate":"0",
               }

encodePlatformDefaults = {
                          "97425":{
                                   "encode_instance":"0",
                                   "encode_channel":"0",
                                   "decode_instance":"0",
                                   "decode_feeder":"0",
                                   },
                          "97435":{
                                   "encode_instance":"1",
                                   "encode_channel":"0",
                                   "decode_instance":"1",
                                   "decode_feeder":"3",
                                   },
                          }
encodeDefaults = {
                  "encode_multi_channel_mode":"1",
                  "encode_frame_rate":"5",
                  "encode_frame_rate_variable_mode":"0",
                  "encode_bitrate":"6000000",
                  "encode_protocol":"0",
                  "encode_profile":"3",
                  "encode_level":"13",
                  "encode_gop_number_of_p":"0",
                  "encode_gop_number_of_b":"0",
                  "encode_nrt_mode":"0",
                  "encode_time":"30",
                  "decode_time":"24",
                  "encode_stop_immediately":"0"
                  }

outputDefaults = {
                  "output_file":"encode_output_[00][00]_000.es.h264",
                  "output_protocol":"0",
                  }

displayFmtLUT = {
                 "0":"480i",
                 "1":"480p",
                 "2":"720p",
                 "3":"1080i",
                 "11":"720p",
                 "12":"720p",
                 "13":"720p",
                 "14":"720p",
                 "15":"1080i",
                 "16":"1080p",
                 "17":"1080p",
                 "18":"1080p",
                 "19":"1080p",
                 "20":"1080p"
                 }
refreshRateLUT = {
                  "0":"60Hz",
                  "1":"50Hz",
                  "2":"24Hz",
                  "3":"25Hz",
                  "4":"30Hz"
                  }
encodeRateLUT = {
                 "1":"24fps",
                 "2":"24fps",
                 "3":"25fps",
                 "4":"30fps",
                 "5":"30fps",
                 "6":"50fps",
                 "8":"60fps",
                 "9":"15fps",
                 "10":"7"
                 }

encodeProtocolLUT = {
                     "0":"H264",
                     "1":"MPEG2",
                     "7":"MPEG4",
                     "8":"VC1"
                     }

encodeProfileLUT = {
                    "0":{
                         "9":"Baseline",
                         "3":"High",
                         "2":"Main"
                         },
                    "1":{
                         "2":"Main"
                         },
                    "7":{
                         "1":"Simple",
                         "8":"AdvSimple",
                         },
                    "8":{
                         "2":"Main"
                         }
                    }

encodeLevelLUT = {
                  "0":{
                       "2":"10",
                       "4":"11",
                       "5":"12",
                       "6":"13",
                       "7":"20",
                       "8":"21",
                       "9":"22",
                       "10":"30",
                       "11":"31",
                       "12":"32",
                       "13":"40",
                       },
                  "1":{
                       "20":"Low",
                       "21":"Main",
                       "22":"High"
                       },
                  "8":{
                       "20":"Low",
                       "21":"Medium"
                       },
                  "7":{
                       "2":"10",
                       "4":"11",
                       "7":"20",
                       "10":"30",
                       "13":"40",
                       "16":"50"
                       },
                  }

encodeGOPLUT = {
                "0":"I",
                "1":"IPP",
                "2":"IBP",
                "3":"IBBP"
                }

def genTestFilename(currentTest, configActive):
    filename = "%(number)03d" % {"number": currentTest}
    filename += "_" + displayFmtLUT[configActive["vdc_display_fmt"]]
    filename += refreshRateLUT[configActive["vdc_refresh_rate"]]
    filename += "_" + encodeRateLUT[configActive["encode_frame_rate"]]
    filename += "_" + str(int(configActive["encode_bitrate"])/1000000) + "Mbps"
    filename += "_" + encodeProtocolLUT[configActive["encode_protocol"]]
    filename += "_" + "P" + encodeProfileLUT[configActive["encode_protocol"]][configActive["encode_profile"]]
    filename += "_" + "L" + encodeLevelLUT[configActive["encode_protocol"]][configActive["encode_level"]]

    filename += "_"
    filename += "I"
    if ( configActive["encode_gop_number_of_p"] != '0' ):
        filename += "P" + configActive["encode_gop_number_of_p"]
        if ( configActive["encode_gop_number_of_b"] != '0' ):
             filename += "B" + configActive["encode_gop_number_of_b"]
    filename += "_" + configActive["source_friendly_name"]
    return filename

gopList = [
           # I
           {
            "encode_gop_number_of_p":"0",
            "encode_gop_number_of_b":"0",
            },
           # IP
           {
            "encode_gop_number_of_p":"14",
            "encode_gop_number_of_b":"0",
            },
           # IBBP
           {
            "encode_gop_number_of_p":"14",
            "encode_gop_number_of_b":"2",
            },
           ]

frameRateList = [
                   # 480p60 --> 60
                   {
                    "vdc_display_fmt":"1",
                    "vdc_refresh_rate":"0",
                    "encode_frame_rate":"8",
                    },
                   # 720p60 --> 60
                   {
                    "vdc_display_fmt":"2",
                    "vdc_refresh_rate":"0",
                    "encode_frame_rate":"8",
                    },
                   # 720p60 --> 30
                   {
                    "vdc_display_fmt":"2",
                    "vdc_refresh_rate":"0",
                    "encode_frame_rate":"5",
                    },
                   # 720p60 --> 24
                   {
                    "vdc_display_fmt":"2",
                    "vdc_refresh_rate":"0",
                    "encode_frame_rate":"2",
                    },
                   # 720p30 --> 30
                   {
                    "vdc_display_fmt":"13",
                    "vdc_refresh_rate":"4",
                    "encode_frame_rate":"5",
                    },
                   # 720p24 --> 24
                   {
                    "vdc_display_fmt":"11",
                    "vdc_refresh_rate":"2",
                    "encode_frame_rate":"2",
                    },
                   # 1080p60 --> 60
                   {
                    "vdc_display_fmt":"20",
                    "vdc_refresh_rate":"0",
                    "encode_frame_rate":"8",
                    },
                   # 1080p60 --> 30
                   {
                    "vdc_display_fmt":"20",
                    "vdc_refresh_rate":"0",
                    "encode_frame_rate":"5",
                    },
                   # 1080p30 --> 30
                   {
                    "vdc_display_fmt":"18",
                    "vdc_refresh_rate":"4",
                    "encode_frame_rate":"5",
                    },
                   # 1080p24 --> 24
                   {
                    "vdc_display_fmt":"16",
                    "vdc_refresh_rate":"2",
                    "encode_frame_rate":"2",
                    },
                   ]

frameRateConfig = {
                   "friendly_name":"Frame Rate Tests",
                   "path_name":"frameRate",
                   "tests":
                   [
                    frameRateList,
                    gopList,
                    streamList,
                    ]
                   }

ccFrameRateList = [
                   # 480p60 --> 60
                   {
                    "vdc_display_fmt":"1",
                    "vdc_refresh_rate":"0",
                    "encode_frame_rate":"8",
                    },
                   # 720p60 --> 30
                   {
                    "vdc_display_fmt":"2",
                    "vdc_refresh_rate":"0",
                    "encode_frame_rate":"5",
                    },
                   # 720p60 --> 24
                   {
                    "vdc_display_fmt":"2",
                    "vdc_refresh_rate":"0",
                    "encode_frame_rate":"2",
                    },
                   # 720p30 --> 30
                   {
                    "vdc_display_fmt":"13",
                    "vdc_refresh_rate":"4",
                    "encode_frame_rate":"5",
                    },
                   # 720p24 --> 24
                   {
                    "vdc_display_fmt":"11",
                    "vdc_refresh_rate":"2",
                    "encode_frame_rate":"2"
                    },
                   ]

protocolList = [
                {
                 "encode_protocol":"0",
                 "encode_profile":"3",
                 "encode_level":"13",
                 },
                {
                 "encode_protocol":"1",
                 "encode_profile":"2",
                 "encode_level":"22",
                 }
                ]

encodeTimeList = [
                  {
                   "encode_time":"30",                   
                   }
                  ]

ccGopList = [
           # I
           {
            "encode_gop_number_of_p":"0",
            "encode_gop_number_of_b":"0",
            },
           # IBBP
           {
            "encode_gop_number_of_p":"14",
            "encode_gop_number_of_b":"2",
            },
           ]

ccConfig = {
                   "friendly_name":"CC Tests",
                   "path_name":"cc",
                   "tests":
                   [
                    encodeTimeList,
                    ccFrameRateList,
                    protocolList,
                    ccGopList,
                    ccStreamList,
                    ]
                   }

outputFileProtocolLUT = {
                         "0":"h264",
                         "1":"mpeg2",
                         }

def updateOutputFileEntries(currentConfig):
    output_file="encode_output_[%(instance)02d][%(channel)02d]_000.es.%(protocol)s" % {
                                                                                    "instance": int(currentConfig["encode_instance"]),
                                                                                    "channel": int(currentConfig["encode_channel"]),
                                                                                    "protocol": outputFileProtocolLUT[currentConfig["encode_protocol"]]
                                                                                    }
    currentConfig.update({"output_file":output_file})
    currentConfig.update({"output_protocol":currentConfig["encode_protocol"]})

def generateTests_1T(suite, path, configDefaults, testTemplate, testConfigurations):
    suite.write("#### " + testConfigurations["friendly_name"] + " ###\n")

    testPath = path + "/" + testConfigurations["path_name"];
    os.mkdir(testPath)

    # Create a list of combinations
    testCombinations = itertools.product(*testConfigurations["tests"])

    currentTest = 0

    for test in testCombinations:
        currentConfig = configDefaults.copy()
        for config in test:
            currentConfig.update(config)
        encodeNum = 0

        updateOutputFileEntries(currentConfig)

        if not all (key in configDefaults for key in currentConfig):
            print "Error: keys in currentConfig don't match any keys in default"
            print "+++ currentConfig +++"
            print currentConfig
            print "--- defaults ---"
            print configDefaults
            print "++++++"
            sys.exit(-1)

        # ignore invalid combinations
        if ( currentConfig["encode_gop_number_of_b"] != "0" ):
            # 720p60 doesn't support B pictures
            if ( currentConfig["vdc_display_fmt"] == "2" and currentConfig["vdc_refresh_rate"] == "0" and currentConfig["encode_frame_rate"] == "8" ):
#                print "Skipping:", currentConfig
                continue
            # 1080pAny don't support B pictures
            if ( currentConfig["vdc_display_fmt"] == "20" or currentConfig["vdc_display_fmt"] == "18" or currentConfig["vdc_display_fmt"] == "16" ):
#                print "Skipping:", currentConfig
                continue

        configActive = {}
        for key in currentConfig.iterkeys():
            configActive[key+"_"+str(encodeNum)] = currentConfig[key]

        defaultRun = testTemplate.substitute(configActive)
        filename = testPath + "/" + genTestFilename(currentTest, currentConfig)
        suite.write(filename + "\n")
        file = open(filename+".vdc_test","w");
        file.write(defaultRun);
        file.close()
        currentTest += 1

def generateSuite_1T(suite, path, configDefaults):
    path += "/1T";
    os.mkdir(path)
    # Frame Rate Tests
    generateTests_1T(suite, path, configDefaults.copy(), Template(open("templates/basic_single_encode_template.vdc_test","r").read()), frameRateConfig)
    # CC Tests
    generateTests_1T(suite, path, configDefaults.copy(), Template(open("templates/cc_encode_template.vdc_test","r").read()), ccConfig)

def generateSuite_2T(suite, path, configDefaults):
    pass

def generateSuite_4T(suite, path, configDefaults):
    pass


configurations = {
                  "97425":["1T","2T"],
                  "97435":["1T","2T","4T"]
                  }
#
# Main
#
if __name__ == '__main__':
    platform = os.environ["PLATFORM"]
    autogendir = platform + "_autogen"

    shutil.rmtree(autogendir,ignore_errors=True);
    os.mkdir(autogendir)

    configDefaults = vdcDefaults.copy()
    configDefaults.update(vdcPlatformDefaults[platform])
    configDefaults.update(encodeDefaults)
    configDefaults.update(encodePlatformDefaults[platform])
    configDefaults.update(outputDefaults)
    configDefaults.update(streamList[0])

    with open('test_suites.txt','w') as all_suites_file:
        for config in configurations[platform]:
            suiteName = platform+"_"+ config + "_vce_test_suite.txt"
            all_suites_file.write(suiteName+"\n")
            print "generating suite:", suiteName
            with open(suiteName,"w") as suite:
               #TODO: Append static tests here
               globals()["generateSuite_"+config](suite,autogendir,configDefaults.copy())

