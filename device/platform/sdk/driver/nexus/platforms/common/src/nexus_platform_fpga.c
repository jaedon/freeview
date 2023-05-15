/***************************************************************************
*     (c)2004-2010 Broadcom Corporation
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
* $brcm_Workfile: nexus_platform_fpga.c $
* $brcm_Revision: 5 $
* $brcm_Date: 3/22/10 5:10p $
*
* API Description:
*   API name: Platform FPGA
*    APIs to control the transport FPGA on broadcom reference boards
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97400/src/nexus_platform_fpga.c $
* 
* 5   3/22/10 5:10p mward
* SW7400-2726:  Set global handles to NULL after closing to indicate
*  closed to prevent errors re-initializing.
* 
* 4   3/19/09 4:59p erickson
* PR53323: add more #if so that more modules can be optionally removed
*
* 3   1/26/09 12:35p erickson
* PR51468: global variable naming convention
*
* 2   8/14/08 10:21a katrep
* PR45674:Fix compiiler warning in kernel mode non debug builds
*
* 1   1/18/08 2:17p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/5   10/10/07 11:29a jgarrett
* PR 35551: Revising I2C Handle for synchronization purposes
*
* Nexus_Devel/4   10/5/07 1:42p jgarrett
* PR 35744: Adding i2c module
*
* Nexus_Devel/3   9/25/07 1:23p jgarrett
* PR 35002: Removing warnings
*
* Nexus_Devel/2   9/24/07 12:06p jgarrett
* PR 35002: adding conditional for 7401/7403
*
* Nexus_Devel/1   9/21/07 6:26p jgarrett
* PR 35002: Adding nexus_platform_fpga
*
***************************************************************************/

#include "nexus_types.h"

#if NEXUS_HAS_I2C
#include "priv/nexus_core.h"
#include "nexus_i2c.h"
#include "bfpga.h"
#include "bfpga_name.h"
#include "breg_i2c.h"
#include "priv/nexus_i2c_priv.h"
#include "nexus_platform_priv.h"

BDBG_MODULE(nexus_platform_fpga);

NEXUS_Error NEXUS_Fpga_Init(NEXUS_I2cHandle i2cHandle)
{
#ifdef NEXUS_FPGA_SUPPORT
    NEXUS_Error errCode;
    BFPGA_Settings fpgaSettings;
    BREG_I2C_Handle i2cRegHandle;

    BDBG_ASSERT(NULL != i2cHandle);

    i2cRegHandle = NEXUS_I2c_GetRegHandle(i2cHandle, NULL);
    BDBG_ASSERT(NULL != i2cRegHandle);

    BFPGA_GetDefaultSettings(&fpgaSettings);

    BDBG_MSG(("Open FPGA"));
    errCode = BFPGA_Open(&g_NEXUS_platformHandles.fpgaHandle,
                         NULL,
                         i2cRegHandle,
                         &fpgaSettings);
    if( errCode != BERR_SUCCESS )
    {
        errCode=BERR_TRACE(errCode);
        goto err_fpga;
    }

    /* Init all channels to streamer by default for now */
    BDBG_MSG(("Setup Streamer"));
    BFPGA_Reset(g_NEXUS_platformHandles.fpgaHandle);
    errCode = BFPGA_SetTsOutput(g_NEXUS_platformHandles.fpgaHandle, BFPGA_OutputSelect_ePkt0, BFPGA_TsSelect_eLvds_1);
    errCode = errCode || BFPGA_SetTsOutput(g_NEXUS_platformHandles.fpgaHandle, BFPGA_OutputSelect_ePkt1, BFPGA_TsSelect_eLvds_1);
    errCode = errCode || BFPGA_SetTsOutput(g_NEXUS_platformHandles.fpgaHandle, BFPGA_OutputSelect_ePkt2, BFPGA_TsSelect_eLvds_1);
    errCode = errCode || BFPGA_SetTsOutput(g_NEXUS_platformHandles.fpgaHandle, BFPGA_OutputSelect_ePkt3, BFPGA_TsSelect_eLvds_1);
    errCode = errCode || BFPGA_SetTsOutput(g_NEXUS_platformHandles.fpgaHandle, BFPGA_OutputSelect_ePkt4, BFPGA_TsSelect_eLvds_1);

    if ( errCode )
    {
        errCode=BERR_TRACE(errCode);
        goto err_ts;
    }

    return BERR_SUCCESS;

err_ts:
    BFPGA_Close(g_NEXUS_platformHandles.fpgaHandle);
	g_NEXUS_platformHandles.fpgaHandle = NULL;
err_fpga:
    return errCode;

#else
    BSTD_UNUSED(i2cHandle);
    return BERR_SUCCESS;
#endif
}

NEXUS_Error NEXUS_Fpga_Uninit(void)
{
#ifdef NEXUS_FPGA_SUPPORT
    BFPGA_Close(g_NEXUS_platformHandles.fpgaHandle);
	g_NEXUS_platformHandles.fpgaHandle = NULL;
    return BERR_SUCCESS;
#else
    return BERR_SUCCESS;
#endif
}

#endif /* NEXUS_HAS_I2C */

