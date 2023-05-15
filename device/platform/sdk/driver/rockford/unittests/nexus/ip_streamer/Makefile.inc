############################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile.inc $
# $brcm_Revision: Hydra_Software_Devel/5 $
# $brcm_Date: 11/30/11 4:33p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /rockford/unittests/nexus/ip_streamer/Makefile.inc $
# 
# Hydra_Software_Devel/5   11/30/11 4:33p jtna
# SW7405-5458: remove callbacks.c
# 
# Hydra_Software_Devel/4   8/31/11 2:14p jtna
# SW7405-4989: remove record_with_timestamps
# 
# Hydra_Software_Devel/3   7/27/10 12:51p calvinho
# SW7405-4653: Updated nomenclature
# 
# Hydra_Software_Devel/2   7/26/10 4:52p calvinho
# SW7405-4653: Created new variable IP_STREAMER_SRC
# 
# Hydra_Software_Devel/1   7/23/10 10:53a calvinho
# SW7405-4653: Added IP Streamer input substitution
# 
############################################################

IP_STREAMER_SRC = ip_streamer_subst.c

#from unittests/nexus/3dtv
IP_STREAMER_APPS += 3dsettop_pig

#from unittests/nexus/audio
IP_STREAMER_APPS += audio_test audio_test_auto codec_test

#from unittests/nexus/display
IP_STREAMER_APPS += digital_cc_capture dnr_pip_swap dtv_pip mad read_vbi resize_window

#from unittests/nexus/file
IP_STREAMER_APPS += timeshift_jump

#from unittests/nexus/general
IP_STREAMER_APPS += analog_digital_channel_change commercial_splice

#from unittests/nexus/graphics2d
IP_STREAMER_APPS += a8_blit decode_video_via_graphics palette_blit3 premultiply_alpha1 premultiply_alpha2

#from unittests/nexus/record
IP_STREAMER_APPS += allpass_test pacing_record record_play record_stress_test record1 record2 record3

#from unittests/nexus/transport
IP_STREAMER_APPS += change_timebase message message_alloc message_dss message_group2 message_pes message_stress1 message_stress2 message_stress3 message_stress4 pacing_recpump parser_band pid_splicing scrambling_status tpit

#from unittests/nexus/video_decoder
IP_STREAMER_APPS += avc_fcc_demo channel_change dataready fcc_demo hd_fcc_demo mosaic_decode most_recent_pts still_decoder_stress userdata_capture video_primer
#removed redundancy: codec_test

IP_STREAMER_APP_OBJS = $(addsuffix .o, $(IP_STREAMER_APPS))
