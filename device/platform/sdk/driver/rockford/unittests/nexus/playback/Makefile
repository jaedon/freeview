############################################################
#     Copyright (c) 2003-2011, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/20 $
# $brcm_Date: 10/12/11 11:46a $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/playback/Makefile $
# 
# Hydra_Software_Devel/20   10/12/11 11:46a erickson
# SW7425-1261: add dqt unittest
# 
# Hydra_Software_Devel/19   8/2/11 6:25p vsilyaev
# SW7425-1044: Added Playback with the STC 'Host' mode
# 
# Hydra_Software_Devel/18   10/13/09 6:27p mphillip
# SW7405-3034: Disable playback_slow_io test due to lack of test files
# 
# Hydra_Software_Devel/17   6/2/09 7:38p vsilyaev
# PR 55417: Added support for read method returning no data or completing
# with a partial read
# 
# Hydra_Software_Devel/16   5/5/09 4:58p erickson
# PR51821: create unittest for this PR
#
# Hydra_Software_Devel/15   4/6/09 2:20p erickson
# PR42679: update
#
# Hydra_Software_Devel/14   4/3/09 12:57p erickson
# PR53824: remove redundant test
#
# Hydra_Software_Devel/13   2/13/09 10:59a erickson
# PR47221: added test app for starting playback from a previous location
#
# Hydra_Software_Devel/12   1/20/09 1:22p erickson
# PR42679: added playback_start_stop test
#
# Hydra_Software_Devel/11   12/8/08 11:18a erickson
# PR49930: add seek test
#
# Hydra_Software_Devel/10   11/4/08 5:03p erickson
# PR42679: added audio trick test
#
# Hydra_Software_Devel/9   11/3/08 12:18p jtna
# PR42679: added codec_mismatch test
#
# Hydra_Software_Devel/8   10/29/08 5:22p erickson
# PR48517: added playback_list test
#
# Hydra_Software_Devel/7   10/24/08 10:34a erickson
# PR48340: update
#
# Hydra_Software_Devel/6   10/23/08 3:59p erickson
# PR42679: update
#
# Hydra_Software_Devel/5   9/8/08 11:33a erickson
# PR42679: added stctrick test
#
# Hydra_Software_Devel/4   7/24/08 4:33p erickson
# PR45124: test opening duplicate pid channels
#
# Hydra_Software_Devel/3   5/14/08 10:07a erickson
# PR42679: eliminate duplicate code in unittest Makefiles
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = \
    audio_switch \
    codec_mismatch \
    create_destroy \
    dqt_trick \
    frameadvance_stress \
    playback_list \
    playback_openpidchannel \
    playback_stctrick \
	playback_stc_host \
    playback_stop_start \
    playback_trick_loop \
    seek \
    seek2 \
    start_stop \
    trick_stress

EXTRA_SRC = examples_lib.c

CFLAGS += -I${NEXUS_TOP}/../BSEAV/lib/bfile 

include ../common/Makefile.inc

