#/***************************************************************************
# *     (c)2007-2011 Broadcom Corporation
# *
# *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
# *  and may only be used, duplicated, modified or distributed pursuant to the terms and
# *  conditions of a separate, written license agreement executed between you and Broadcom
# *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
# *  no license (express or implied), right to use, or waiver of any kind with respect to the
# *  Software, and Broadcom expressly reserves all rights in and to the Software and all
# *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
# *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
# *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
# *
# *  Except as expressly set forth in the Authorized License,
# *
# *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
# *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
# *  and to use this information only in connection with your use of Broadcom integrated circuit products.
# *
# *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
# *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
# *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
# *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
# *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
# *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
# *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
# *  USE OR PERFORMANCE OF THE SOFTWARE.
# *
# *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
# *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
# *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
# *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
# *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
# *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
# *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
# *  ANY LIMITED REMEDY.
# *
# * $brcm_Workfile: bhsm.inc $
# * $brcm_Revision: Hydra_Software_Devel/13 $
# * $brcm_Date: 12/5/12 1:26a $
# *
# * Module Description:
# *
# * Revision History:
# *
# * $brcm_Log: /magnum/portinginterface/hsm/bhsm.inc $
# * 
# * Hydra_Software_Devel/13   12/5/12 1:26a atruong
# * SW7563-16: HSM PI support for 7563A0
# * 
# * Hydra_Software_Devel/12   11/19/12 11:01a atruong
# * SW7584-46: HSM PI/Nexus Security support for 7584A0 SW7435-232: HSM
# * PI/Nexus Security support for 7435B0
# * 
# * Hydra_Software_Devel/11   9/20/12 2:41p atruong
# * SWSECURITY-175: Bug fix - Removing Symbolic Link in HSM PI
# * 
# * Hydra_Software_Devel/10   9/11/12 2:38p atruong
# * SWSECURITY-175: Remove symbolic links in HSM PI
# * 
# * Hydra_Software_Devel/9   4/3/12 2:09a atruong
# * SW7425-2807:Add support for BSP PKL hardware
# * 
# * Hydra_Software_Devel/8   10/17/11 5:26p atruong
# * SWSECURITY-69: Support for 40-nm B0 Zeus 2.0 platforms
# * 
# * Hydra_Software_Devel/7   10/12/11 10:48p atruong
# * SW7425-1251: HSM PI Support For BCM7425B0
# * 
# * Hydra_Software_Devel/6   9/23/11 7:09p atruong
# * SW7346-495: HSM_SOURCE_AVAILABLE option fix
# * 
# * Hydra_Software_Devel/5   9/21/11 11:55a atruong
# * SW7346-495: Add NSK2AVK support
# * 
# * Hydra_Software_Devel/4   6/2/11 6:08p atruong
# * SW7422-451: New Folder structure/BSP shared header file separation
# * 
# * Hydra_Software_Devel/3   5/31/11 11:34p atruong
# * SW7422-451: New Folder structure/BSP shared header file separation
# * 
# * Hydra_Software_Devel/2   1/24/11 10:44a atruong
# * SW7422-26: Add HSM PI Support For 7422A0  - CC header format
# *
# ******************************************************************************/

#-----------------------------------------------------------------------------------#
# HSM build-command-line-options:
#  CUST=GENERIC3A  or 
#  CUST=GENERIC3B  or
#  CUST=DIRECTV    or 
#  CUST=ECHOSTAR   or
#  CUST=CONAX      or
#  CUST=CISCO      or
#  CUST=IRDETO     or
#  CUST=MICROSOFT  or
#  CUST=MOTOROLA   or
#  CUST=NAGRA      or
#  CUST=NDS        or
#  CUST=NDSDSS     or
#  CUST=ALL        to select customer mode
#  CUST=Not Set or Invalid -  Customer Mode defaults to GENERIC3A
#  SCVALUE=ON                to turn on scrambling ctrl bits setting with specified value
#  BHSM_REGIONVERIFY=ON      to turn on region verification with Generic mode  (invalid option for non-Generic mode build)
#  BHSM_BSECK=ON             to turn on/off BSECK feature 
#  BHSM_IPTV=ON              to turn on/off IPTV feature
#  BHSM_KEYLADDER=ON         to select spitted, protected secure PIs 
#  BHSM_MISC=ON              to select spitted, protected secure PIs 
#  BHSM_OTPMSP=ON            to select spitted, protected secure PIs 
#  BHSM_SECURE_RSA=ON        to turn on Secure RSA feature
#  BHSM_SECURE_ACCESS=ON     to turn on Secure Access (DRM Data Op) feature
#  BHSM_IP_LICENSING=ON      to turn on IP Licensing feature
#  BHSM_MISC=ON              to turn on support for DEBUG_DUMP_EXEC command
#  BHSM_PWR_MGMT=ON          to turn on support for power management
#  BHSM_WEB_CPU=ON           to turn on support for Web CPU interface
#  BHSM_SAGE_INTF=ON         to turn on SAGE-Host interface support
#  BHSM_SAGE_PI=ON           to compile HSM PI for SAGE
#
#-----------------------------------------------------------------------------------#


#
# Include the right bhsm_*.inc file
#

ifeq ($(filter $(BCHP_CHIP), 7231 7344 7346 7358 7360 7422 7425 7429 7435 7552 7563 7584 7145 7445), $(BCHP_CHIP))
include $(MAGNUM)/portinginterface/hsm/bhsm_zeus.inc
else
include $(MAGNUM)/portinginterface/hsm/bhsm_legacy.inc
endif

