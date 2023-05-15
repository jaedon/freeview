############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile.inc $
# $brcm_Revision: Hydra_Software_Devel/1 $
# $brcm_Date: 1/10/12 5:46p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/nexus/channels_file/Makefile.inc $
# 
# Hydra_Software_Devel/1   1/10/12 5:46p mward
# SW7125-1205:  Add channels file support.
# 
############################################################

CHANNELS_FILE_SRC = read_channels_file.c

#from unittests/nexus/3dtv
#CHANNELS_FILE_APPS += 3dsettop_pig

#from unittests/nexus/audio
#CHANNELS_FILE_APPS += audio_test audio_test_auto codec_test

#from unittests/nexus/display
#CHANNELS_FILE_APPS += digital_cc_capture dnr_pip_swap dtv_pip mad read_vbi resize_window

#from unittests/nexus/file
CHANNELS_FILE_APPS += timeshift_jump

#from unittests/nexus/general
#CHANNELS_FILE_APPS += analog_digital_channel_change commercial_splice

#from unittests/nexus/graphics2d
#CHANNELS_FILE_APPS += a8_blit decode_video_via_graphics palette_blit3 premultiply_alpha1 premultiply_alpha2

#from unittests/nexus/record
#CHANNELS_FILE_APPS += allpass_test pacing_record record_play record_stress_test record1 record2 record3

#from unittests/nexus/transport
#CHANNELS_FILE_APPS += change_timebase message message_alloc message_dss message_group2 message_pes message_stress1 message_stress2 message_stress3 message_stress4 pacing_recpump parser_band pid_splicing scrambling_status tpit

#from unittests/nexus/video_decoder
#CHANNELS_FILE_APPS += avc_fcc_demo channel_change dataready fcc_demo hd_fcc_demo mosaic_decode most_recent_pts still_decoder_stress userdata_capture video_primer
#removed redundancy: codec_test

CHANNELS_FILE_APP_OBJS = $(addsuffix .o, $(CHANNELS_FILE_APPS))
