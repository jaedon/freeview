/***************************************************************************
 *     (c)2002-2009 Broadcom Corporation
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
 * $brcm_Workfile: splash_vdc_rulgen.h $
 * $brcm_Revision: 10 $
 * $brcm_Date: 10/31/12 6:54p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/splash_vdc_rulgen.h $
 * 
 * 10   10/31/12 6:54p mward
 * SW7435-114:  Back out dual surface support.  Move to branch pending CFE
 * support.
 * 
 * 8   4/18/12 2:47p jessem
 * SW7425-2828: Corrected method on obtaining heap. Added run-time options
 * to set the display format and to disable the 2nd display. Backed-out
 * of creating a separate surface for the the 2nd display.
 * 
 * 7   4/9/12 11:25a jessem
 * SW7425-2828: Removed hardcoded RDC scratch registers and use
 * BVDC_Test_Source_GetGfdScratchRegisters() instead. Also, updated the
 * implementation of the splash surface for the 2nd display.
 *
 * 6   8/8/11 11:22p nickh
 * SW7425-878: Pass in both MEMC0 and MEMC1
 *
 * 5   4/8/09 4:29p shyam
 * PR52386 : Add support for DTV platforms
 *
 * 3   4/8/09 12:44p shyam
 * PR52386 : Port splash to nexus Base Build system
 *
 * 2   12/24/08 5:03p katrep
 * PR50711: Add splash support for 7405/7335/7325 and settop chips
 *
 * 1   9/29/08 11:34a erickson
 * PR46184: added splash app
 *
 * 4   10/8/07 7:28p shyam
 * PR 30741 : Add support for No HDMI
 *
 * 2   6/27/07 6:21p shyam
 * PR 30741 : Magnum compliant RDC hooks
 *
 * 1   5/14/07 6:52p shyam
 * PR 30741 : Add reference support for generic portable splash
 *
 * Hydra_Software_Devel/1   11/17/05 4:46p dkaufman
 * PR3481: Added file
 *
 ***************************************************************************/

#ifndef SPLASH_VDC_GENERATE_H__
#define SPLASH_VDC_GENERATE_H__

#include "bfmt.h"
#include "bchp.h"
#include "brdc.h"
#include "bvdc.h"
#include "bi2c.h"
#if NEXUS_DTV_PLATFORM
#include "blvd.h"
#include "bpwm.h"
#endif

#ifndef SPLASH_NOHDM
#include "bhdm.h"
#endif


#define TestError(e, str)   {\
    eErr = e;\
    if (eErr != BERR_SUCCESS)\
    {\
    BDBG_ERR(( str". %s: %d", __FILE__, __LINE__ ));\
        goto done;\
    }\
}

typedef struct
{
    BRDC_Handle            hRdc;
    BVDC_Handle            hVdc;
    BI2C_Handle            hI2c;
    BI2C_ChannelHandle     hI2cChn ;
    BREG_I2C_Handle        hRegI2c;
    BMEM_Handle            hMem0;
    BMEM_Handle            hMem1;

#ifndef SPLASH_NOHDM
    BHDM_Handle            hHdm;
#endif

#if NEXUS_DTV_PLATFORM
    BLVD_Handle            hLvd;
    BPWM_Handle            hPwm;
    BPWM_ChannelHandle     hPwmChn;
#endif

    BVDC_Compositor_Handle hCompositor0;
    BVDC_Display_Handle    hDisplay0;
	BFMT_VideoFmt          eDisplay0Fmt;
    BVDC_Source_Handle     hGfxSource0;
    BVDC_Window_Handle     hGfxWindow0;
    uint32_t               ulDisp0GfdScrReg0;
    uint32_t               ulDisp0GfdScrReg1;

    BVDC_Compositor_Handle hCompositor1;
    BVDC_Display_Handle    hDisplay1;
    BFMT_VideoFmt          eDisplay1Fmt;
    BVDC_Source_Handle     hGfxSource1;
    BVDC_Window_Handle     hGfxWindow1;
    uint32_t               ulDisp1GfdScrReg0;
    uint32_t               ulDisp1GfdScrReg1;

    BSUR_Surface_Handle    hSurface;
    uint32_t               ulGfxWidth;
    uint32_t               ulGfxHeight;
	
	bool                   bNoDisplay1;
} ModeHandles;

BERR_Code  splash_vdc_setup
	( BCHP_Handle         hChp,
      BREG_Handle         hReg,
      BINT_Handle         hInt,
      BTMR_Handle         hTmr,
      ModeHandles        *pState );


BERR_Code  close_mode
	( ModeHandles        *pState );

void APP_BREG_Write32
    ( BREG_Handle  hReg,
      uint32_t     ulReg,
      uint32_t     ulValue );

#endif

/* End of file */

