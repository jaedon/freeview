/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_display_priv.h $
 * $brcm_Revision: 8 $
 * $brcm_Date: 3/15/12 2:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7425/include/priv/nexus_display_priv.h $
 * 
 * 8   3/15/12 2:25p jtna
 * SW7231-584, SW7231-586: NEXUS_DisplayCapturedImage.framerate
 * 
 * 7   3/7/12 3:09p jtna
 * SW7231-584, SW7231-586: Add external interrupt info for go bit
 * 
 * 6   2/2/12 2:31p jtna
 * SW7231-587: add NEXUS_DisplayCapturedImage.origPts
 * 
 * 5   2/1/12 11:54a erickson
 * SW7425-1473: fix compilation error
 * 
 * 4   1/31/12 5:23p jtna
 * SW7425-1473: merge
 * 
 * 3   1/16/12 3:29p erickson
 * SW7425-1473: merge
 * 
 * 3   1/16/12 3:28p erickson
 * SW7425-1473: merge
 * 
 * 2   12/16/11 5:29p erickson
 * SW7425-1878: merge XUDlib support for video encoder userdata
 * 
 * SW7425-1878/2   12/16/11 5:26p ajagmag
 * SW7425-1878: Change prototype of private function to use real type
 *  instead of void *
 * 
 * SW7425-1878/1   12/9/11 1:31p ajagmag
 * SW7425-1878: Integrate XUDlib into nexus
 * 
 * 1   10/20/11 1:43p erickson
 * SW7420-1992: add private api's for atomically setting multiple video
 *  window settings
 * 
 **************************************************************************/
#ifndef NEXUS_DISPLAY_PRIV_H__
#define NEXUS_DISPLAY_PRIV_H__

#include "nexus_display.h"
#if NEXUS_HAS_VIDEO_ENCODER && !NEXUS_NUM_DSP_VIDEO_ENCODERS
#include "bxudlib.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
        
NEXUS_Error NEXUS_DisplayModule_SetUpdateMode_priv(
    NEXUS_DisplayUpdateMode updateMode,
    NEXUS_DisplayUpdateMode *pPrevUpdateMode /* optional */
    );

#if NEXUS_HAS_VIDEO_ENCODER
#ifndef NEXUS_NUM_DSP_VIDEO_ENCODERS
NEXUS_Error NEXUS_DisplayModule_SetUserDataEncodeMode_priv(
    NEXUS_DisplayHandle display,
    bool encodeUserData,
    BXUDlib_Settings *userDataEncodeSettings
    );
#else 
typedef struct NEXUS_DisplayCapturedImage {
    void *              pvaddress;
    unsigned            width;
    unsigned            height;   
    BAVC_Polarity       polarity;
    uint32_t            origPts;
    NEXUS_VideoFrameRate framerate;
} NEXUS_DisplayCapturedImage;

typedef struct NEXUS_DisplayEncoderSettings {
    NEXUS_Error (*enqueueCb_isr)(void *context, NEXUS_DisplayCapturedImage *image);
    NEXUS_Error (*dequeueCb_isr)(void *context, NEXUS_DisplayCapturedImage *image);
    void *context;
    NEXUS_VideoFrameRate encodeRate;
    uint32_t extIntAddress;       
    uint32_t extIntBitNum;
} NEXUS_DisplayEncoderSettings;

NEXUS_Error NEXUS_Display_SetEncoderCallback_priv(NEXUS_DisplayHandle display, NEXUS_DisplayEncoderSettings *pSettings);
NEXUS_Error NEXUS_Display_EnableEncoderCallback_priv(NEXUS_DisplayHandle display);
NEXUS_Error NEXUS_Display_DisableEncoderCallback_priv(NEXUS_DisplayHandle display);
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_DISPLAY_PRIV_H__ */

