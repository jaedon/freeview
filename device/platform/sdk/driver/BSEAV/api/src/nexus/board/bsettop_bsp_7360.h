/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bsettop_bsp_7360.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/board/bsettop_bsp_7360.h $
 * 
 * 2   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
/* VDC options */
#define B_HAS_MNR 1
#define B_HAS_BNR 1
#define B_HAS_MAD 1
#define B_HAS_BOX_DETECT 1
#define B_HAS_SHARPNESS 1
#define B_HAS_LAB 1
#define B_HAS_CAB 1
#define B_N_DNR 1
#define B_HAS_DCR 1
#define B_HAS_ANR 0



#define B_MAX_DECODES            NEXUS_NUM_VIDEO_DECODERS
#define B_MAX_STILL_DECODES      NEXUS_NUM_STILL_DECODES
#define B_MAX_AUDIO_ES_DECODES   1
#define B_MAX_MOSAIC_DECODES     NEXUS_NUM_MOSAIC_DECODES
#define B_MAX_REMUXES            NEXUS_NUM_REMUX


#define B_N_DISPLAYS NEXUS_NUM_DISPLAYS
#define B_N_GRAPHICS NEXUS_NUM_DISPLAYS
#define B_N_WINDOWS  NEXUS_NUM_VIDEO_WINDOWS

#define B_N_COMPOSITE_OUTPUTS   NEXUS_NUM_COMPOSITE_OUTPUTS
#define B_N_RF_OUTPUTS          NEXUS_NUM_RFM_OUTPUTS
#define B_N_COMPONENT_OUTPUTS   NEXUS_NUM_COMPONENT_OUTPUTS
#define B_N_SVIDEO_OUTPUTS      NEXUS_NUM_SVIDEO_OUTPUTS
#define B_N_DVI_OUTPUTS         NEXUS_NUM_HDMI_OUTPUTS

#define B_N_AUDIO_DACS NEXUS_NUM_AUDIO_DACS

#ifdef B_HAS_IP
#define B_N_I2S_OUTPUTS 1
#endif

#define B_MAX_PCRS    NEXUS_NUM_TIMEBASES
#define B_MAX_PARSERS NEXUS_NUM_PARSER_BANDS
#define B_MAX_INPUTS  NEXUS_NUM_INPUT_BANDS

