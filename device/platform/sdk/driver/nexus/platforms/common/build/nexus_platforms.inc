############################################################
#     (c)2003-2012 Broadcom Corporation
#
#  This program is the proprietary software of Broadcom Corporation and/or its licensors,
#  and may only be used, duplicated, modified or distributed pursuant to the terms and
#  conditions of a separate, written license agreement executed between you and Broadcom
#  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
#  no license (express or implied), right to use, or waiver of any kind with respect to the
#  Software, and Broadcom expressly reserves all rights in and to the Software and all
#  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
#  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
#  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
#
#  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
#  USE OR PERFORMANCE OF THE SOFTWARE.
#
#  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
#  ANY LIMITED REMEDY.
#
# $brcm_Workfile: nexus_platforms.inc $
# $brcm_Revision: 37 $
# $brcm_Date: 9/26/12 10:57a $
#
# Module Description:
#   This file defines the rules to build the 97400 platform
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/platforms/common/build/nexus_platforms.inc $
# 
# 37   9/26/12 10:57a jzhu
# SW7435-246:sms3335 support.
# 
# 36   9/10/12 5:33p randyjew
# SW7563-2: Add 7563 support.
# 
# 35   8/23/12 2:42p erickson
# SW7401-4517: remove old code
# 
# 34   7/11/12 10:51a katrep
# SW7445-1:add 97445 platform
# 
# 33   6/8/12 5:31p chengs
# SW7584-34: Add 7584 support.
# 
# 32   5/23/12 2:12p bselva
# SW7360-19: Add support for 7360 platform
# 
# 31   3/14/12 12:51p randyjew
# SW7344-286: Add 7344 B1 support
# 
# 30   3/12/12 4:41p gkamosa
# SW7425-2543: Merge support for 3383 SMS platform
# 
# Centaurus_Phase_1.0/2   3/6/12 6:03p gkamosa
# SW7425-2337: Merge branch to tip
# 
# 29   2/6/12 5:41p katrep
# SW7231-317:changed defult chip version to B0
# 
# Centaurus_Phase_1.0/1   2/6/12 5:52p jmiddelk
# SW7425-2337: Adding support for 93383sms
# 
# 28   11/15/11 3:19p erickson
# SW7425-1747: remove old code
# 
# 27   10/31/11 4:19p mward
# SW7435-7:  Add 97435.
# 
# 26   9/22/11 5:14p katrep
# SW7429-1:add support for 7429 & bondouts
# 
# 25   6/20/11 3:46p jhaberf
# SWDTV-7484: removed set of BREFSW_ARCH for the 35233
# 
# 24   5/17/11 12:02p jhaberf
# SWDTV-6928: Merge of 35233 platform code to the main
# 
# SWDTV-6928/1   5/4/11 6:39p mstefan
# SWDTV-6928: merge to main.
# 
# 23   3/28/11 12:42p nickh
# SW7422-412: Set 7422 BCHP_CHIP to 7425 for versions A1 and above
# 
# 22   3/2/11 4:40p randyjew
# SW7418-1:Add 97418 support
# 
# 21   12/21/10 11:38a katrep
# SW7231-25:add support for 7230
# 
# 20   12/15/10 2:26p xhuang
# SW7358-21: Add 97358/97552 support
# 
# 19   12/7/10 11:40a jrubio
# SW7344-9: modify existing features 7344/7346
# 
# 18   11/17/10 6:25p katrep
# SW7231-4:add support for 7231
# 
# 17   10/7/10 5:21p nickh
# SW7422-10: Add 7422 support
# 
# 16   10/1/10 5:05p spothana
# SW7420-1078:Add support for BCM97409 platform.
# 
# 15   9/9/10 5:31p hongtaoz
# SW7425-9: add 7425 nexus platform support;
# 
# 14   6/9/10 4:11p prasadv
# SW7400-2546: Adding 97456 platform support.
# 
# 13   5/12/10 4:38p abelous
# SW7630-82: adding 7630
# 
# 12   2/25/10 6:21p mward
# SW7400-2673: Implement support for BCM4500.
# 
# SW7400-2673/1   2/22/10 1:46p mward
# SW7400-2673:  Support BCM4500.
# 
# 11   1/19/10 5:44p garetht
# SW7420-553: Add platform for 93380vms
# 
# 10   1/19/10 2:38p nickh
# SW7420-541: Rename 97420dvr2 to 97420
# 
# 9   12/3/09 4:26p randyjew
# SW7468-6: support for 7208
# 
# 8   12/3/09 4:22p randyjew
# SW7468-6: Update for 7208
# 
# 7   11/16/09 1:25p katrep
# SW7405-3408:replace findstring with filter to do exact word match
# 
# 6   11/6/09 2:45p gmohile
# SW7408-1 : Add 7408 support
# 
# 5   10/30/09 5:04p nickh
# SW7420-331: Rename 97410c to 97410
# 
# 4   10/23/09 1:28p nitinb
# SW7550-29: Add 7550 support
# 
# 3   10/8/09 6:05p haisongw
# SW7405-3091:Add 97205CB Platform support
#
# 2   10/7/09 1:57p lwhite
# SW7468-6: Add 7468 support
#
# 1   9/16/09 1:38p erickson
# SW7400-2511: add B_REFSW_ or NEXUS_ prefix to build variables
#
############################################################

# backward compatibility
NEXUS_PLATFORM ?= $(PLATFORM)

# Sanity check that platform is defined
ifeq ($(NEXUS_PLATFORM),)
$(error NEXUS_PLATFORM is not defined!)
endif

# derive BCHP_CHIP from NEXUS_PLATFORM if not explicitly defined
ifndef BCHP_CHIP
    # Define platform specifics
    ifeq ($(filter $(NEXUS_PLATFORM),97400), $(NEXUS_PLATFORM))
        BCHP_CHIP := 7400
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=D0)
        BCHP_VER := D0
        endif
    endif
    ifeq ($(filter $(NEXUS_PLATFORM),97405 97205 97466), $(NEXUS_PLATFORM))
        BCHP_CHIP := 7405
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=B0)
        BCHP_VER := B0
        endif
    endif
    ifeq ($(NEXUS_PLATFORM),97325)
        BCHP_CHIP := 7325
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif 
    ifeq ($(NEXUS_PLATFORM),97346)
        BCHP_CHIP := 7346
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=B0)
        BCHP_VER := B0
        endif
    endif
    ifeq ($(filter $(NEXUS_PLATFORM),97344 97418), $(NEXUS_PLATFORM))
        BCHP_CHIP := 7344
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=B1)
        BCHP_VER := B1
        endif
    endif 
    ifeq ($(NEXUS_PLATFORM),97342)
        BCHP_CHIP := 7342
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=B0)
        BCHP_VER := B0
        endif
    endif
    ifeq ($(NEXUS_PLATFORM),97340)
        BCHP_CHIP := 7340
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=B0)
        BCHP_VER := B0
        endif
    endif
    ifeq ($(NEXUS_PLATFORM),97335)
        BCHP_CHIP := 7335
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=B0)
        BCHP_VER := B0
        endif
    endif
    ifeq ($(filter $(NEXUS_PLATFORM),97420 97410 97409 93380sms 93380vms), $(NEXUS_PLATFORM))
        BCHP_CHIP := 7420
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=B0)
        BCHP_VER := B0
        endif
    endif
    ifeq ($(filter $(NEXUS_PLATFORM),97125 97019), $(NEXUS_PLATFORM))
        BCHP_CHIP := 7125
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif
    ifeq ($(filter $(NEXUS_PLATFORM),97468 97208), $(NEXUS_PLATFORM))
        BCHP_CHIP := 7468
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif
    ifeq ($(NEXUS_PLATFORM),97550)
        BCHP_CHIP := 7550
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif
    ifeq ($(NEXUS_PLATFORM),97408)
        BCHP_CHIP := 7408
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif
    ifeq ($(NEXUS_PLATFORM),97630)
        BCHP_CHIP := 7630
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=B0)
        BCHP_VER := B0
        endif
    endif
    ifeq ($(NEXUS_PLATFORM),97435)
        BCHP_CHIP := 7435
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif
    ifeq ($(filter $(NEXUS_PLATFORM),97425 93383sms), $(NEXUS_PLATFORM))
        BCHP_CHIP := 7425
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif
    ifeq ($(NEXUS_PLATFORM),97422)
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
        ifeq ($(BCHP_VER),A0)
            BCHP_CHIP := 7422
        else
            BCHP_CHIP := 7425
        endif
    endif

    ifeq ($(NEXUS_PLATFORM),97231)
        BCHP_CHIP := 7231
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := B0
        endif
    endif

    ifeq ($(NEXUS_PLATFORM),97230)
        BCHP_CHIP := 7231
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := B0
        endif
    endif

    ifeq ($(NEXUS_PLATFORM),97358)
        BCHP_CHIP := 7358
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif

    ifeq ($(NEXUS_PLATFORM),97552)
        BCHP_CHIP := 7552
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif
   
    ifeq ($(NEXUS_PLATFORM),97429)
        BCHP_CHIP := 7429
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif

    ifeq ($(NEXUS_PLATFORM),97428)
        BCHP_CHIP := 7429
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif

    ifeq ($(NEXUS_PLATFORM),97241)
        BCHP_CHIP := 7429
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif

    ifeq ($(NEXUS_PLATFORM),97360)
        BCHP_CHIP := 7360
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif
 
    ifeq ($(NEXUS_PLATFORM),97584)
        BCHP_CHIP := 7584
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif    

    ifeq ($(NEXUS_PLATFORM),97445)
        BCHP_CHIP := 7445
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif
	
    ifeq ($(NEXUS_PLATFORM),97563)
        BCHP_CHIP := 7563
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif
 
    ifeq ($(NEXUS_PLATFORM),93335)
        BCHP_CHIP := 7435
        ifndef BCHP_VER
        $(warning Defaulting to BCHP_VER=A0)
        BCHP_VER := A0
        endif
    endif

    # Sanity check that we received a valid platform
    ifndef BCHP_CHIP
    $(error Unsupported platform $(NEXUS_PLATFORM))
    endif
endif
