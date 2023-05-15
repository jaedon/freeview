############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: Hydra_Software_Devel/18 $
# $brcm_Date: 10/4/12 5:10p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /rockford/unittests/nexus/record/Makefile $
# 
# Hydra_Software_Devel/18   10/4/12 5:10p mward
# SW7435-56:  add record_with_timestamps.
# 
# Hydra_Software_Devel/17   10/3/12 11:34a mward
# SW7435-56:  add encrypted_dvr
# 
# Hydra_Software_Devel/16   9/15/11 10:32a erickson
# SW7405-5489: add record_stress_test2
# 
# Hydra_Software_Devel/15   3/4/11 6:24p vsilyaev
# SW7422-14: Added svc indexing unit tests
# 
# Hydra_Software_Devel/14   2/4/10 2:41p mward
# SW7125-195:  Add APPS record_play, record_overflow_recovery,
# recpump_overflow_recovery, record_append_from_playpump.
# 
# Hydra_Software_Devel/13   11/10/09 12:50p jtna
# SWDEPRECATED-3451: remove overflow_recovery from automation
# 
# Hydra_Software_Devel/12   10/20/09 11:29a jtna
# SW7405-2758: remove record_append_from_playpump from automation
# 
# Hydra_Software_Devel/11   9/22/09 6:36p mphillip
# SW7405-3034: Fix include path for bcmplayer dependency
# 
# Hydra_Software_Devel/10   7/24/09 11:45a jtna
# PR56980: added record_append_from_playpump
# 
# Hydra_Software_Devel/9   5/21/09 5:19p vishk
# PR54841 : add create HD record overflow stress test.
# 
# Hydra_Software_Devel/8   3/10/09 10:54a erickson
# PR52946: add example
#
# Hydra_Software_Devel/7   2/13/09 10:19a erickson
# PR50099: added record_play test app
#
# Hydra_Software_Devel/6   12/16/08 11:56p erickson
# PR50347: added multiple_index_only_records
#
# Hydra_Software_Devel/5   12/12/08 5:10p erickson
# PR50347: added test app
#
# Hydra_Software_Devel/4   12/11/08 6:28p jtna
# PR48779: added record and recpump overflow recovery unittests
#
# Hydra_Software_Devel/3   12/1/08 12:48p erickson
# PR49676: added allpass_record_from_playback.c test
#
# Hydra_Software_Devel/2   5/14/08 10:07a erickson
# PR42679: eliminate duplicate code in unittest Makefiles
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
BSEAV_TOP := $(shell cd ../../../../BSEAV && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
BSEAV_TOP := $(shell cd ../../../../BSEAV; pwd)
endif

APPS = \
    allpass_record_from_playback \
    encrypted_dvr \
    index_only_record \
    record_stress_test \
    record_stress_test2 \
    multiple_index_only_records \
    pacing_record \
    record1 \
    record2 \
    record3 \
    record_play \
    record_overflow_recovery \
    recpump_overflow_recovery \
    record_append_from_playpump \
    record_svc \
    record_with_timestamps \
    recpump_itb_svc

EXTRA_SRC = examples_lib.c check_index_lib.c

# need bcmplayer include directory to build check_index_lib
CFLAGS += -I$(BSEAV_TOP)/lib/bcmplayer/include

include ../common/Makefile.inc
