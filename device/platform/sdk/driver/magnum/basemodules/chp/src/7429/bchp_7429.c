/***************************************************************************
 *     (c)2007-2009 Broadcom Corporation
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
 * $brcm_Workfile: bchp_7429.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 10/17/12 11:29a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/src/7429/bchp_7429.c $
 * 
 * Hydra_Software_Devel/13   10/17/12 11:29a jessem
 * SW7429-328: Updated reset raaga core sequence.
 *
 * Hydra_Software_Devel/12   7/20/12 4:54p rjlewis
 * SW7425-3461: moved get data function from chip specific to common.
 *
 * Hydra_Software_Devel/11   6/21/12 3:23p katrep
 * SW7429-1:add support for 7429 B0
 *
 * Hydra_Software_Devel/10   6/13/12 3:19p katrep
 * SW7429-1:cleanup code
 *
 * Hydra_Software_Devel/9   6/5/12 1:03p rjlewis
 * SW7429-180: Added support for AVS.
 *
 * Hydra_Software_Devel/8   5/18/12 3:44p gmohile
 * SW7429-155 : Add 7429 Power Management Support
 *
 * Hydra_Software_Devel/7   3/6/12 12:37p jtna
 * SW7425-2362: add sw workaround for raaga sw init problem
 *
 * Hydra_Software_Devel/6   12/1/11 6:14p mward
 * SW7429-1:  Compiler warning.
 *
 * Hydra_Software_Devel/5   11/30/11 4:30p mward
 * SW7429-1:  Modifying BCHP_SUN_GISB_ARB_REQ_MASK is not necessary or
 * helpful.
 *
 * Hydra_Software_Devel/4   10/7/11 3:45p katrep
 * SW7429-1:removed call to pwr functions
 *
 * Hydra_Software_Devel/3   10/3/11 1:53p katrep
 * SW7429-1:updated for chip bringup
 *
 * Hydra_Software_Devel/2   9/14/11 4:22p katrep
 * SW7429-1:updated for 7429
 *
 * Hydra_Software_Devel/1   8/9/11 6:56p katrep
 * SW7429-1:initial support
 *
 **************************************************************************/

#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "breg_mem.h"
#include "bchp.h"
#include "bchp_priv.h"
#include "bchp_7429.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_decode_sd_0.h"
#include "bchp_decode_ip_shim_0.h"
#include "bchp_sun_gisb_arb.h"
#include "bchp_memc_ddr23_shim_addr_cntl_0.h"
#include "bchp_memc_ddr_0.h"
#include "bchp_pwr.h"

#include "bchp_avs_ro_registers_0.h"
#include "bchp_avs.h"

BDBG_MODULE(BCHP);

/* Miscellaneous macros. */
#define BCHP_P_MAJOR_REV_SHIFT          (4)

/* Chip info and features */
typedef struct BCHP_P_7429_Info
{
    uint32_t      ulChipFamilyIdReg; /* index into the table. */

    /* Chip Id */
    uint16_t      usChipId;

    /* Major revision */
    uint16_t      usMajor;

    /* Minor revision */
    uint16_t      usMinor;

    /* TODO: Other features or infos if needed */
} BCHP_P_7429_Info;


/* Lookup table for chip features and etc.
 * The are many times when the chip device id register
 * not conforming to the standard numbering convention. We do
 * it this way to work-around those problems.
 *
 * TODO: Update this table to support new revisions.
 */
static const BCHP_P_7429_Info s_aChipInfoTable[] =
{
    /* Chip Family contains the major and minor revs */
#if BCHP_VER == BCHP_VER_A0
    /* A0 code will run on A0 */
   {0x74290000, BCHP_BCM7429, BCHP_MAJOR_A, BCHP_MINOR_0},
#elif BCHP_VER == BCHP_VER_B0
   {0x74290010, BCHP_BCM7429, BCHP_MAJOR_B, BCHP_MINOR_0},
#else
   #error "Port required"
#endif
};


/* Chip context */
typedef struct BCHP_P_7429_Context
{
    uint32_t                           ulBlackMagic;
    BREG_Handle                        hRegister;
    const BCHP_P_7429_Info            *pChipInfo;
    BCHP_P_AvsHandle                  hAvsHandle;
} BCHP_P_7429_Context;

/* Max entry of lookup table */
#define BCHP_P_CHIP_INFO_MAX_ENTRY \
    (sizeof(s_aChipInfoTable) / sizeof(BCHP_P_7429_Info))

/* This macro checks for a validity of a handle, and
 * cast to context pointer. */
#define BCHP_P_GET_CONTEXT(handle, context) \
{ \
    if(!(handle) || \
       !((handle)->chipHandle) || \
       (((BCHP_P_7429_Context*)((handle)->chipHandle))->ulBlackMagic != \
       sizeof(BCHP_P_7429_Context))) \
    { \
        BDBG_ERR(("Corrupted context handle\n")); \
        (context) = NULL; \
    } \
    else \
    { \
        (context) = (BCHP_P_7429_Context*)((handle)->chipHandle); \
    } \
    BDBG_ASSERT(context); \
}

/* Static function prototypes */
static BERR_Code BCHP_P_Close7429
    ( BCHP_Handle                      hChip );

static BERR_Code BCHP_P_GetChipInfoComformWithBaseClass
    ( const BCHP_Handle                hChip,
      uint16_t                        *pusChipId,
      uint16_t                        *pusChipRev );

static BERR_Code BCHP_P_GetChipInfo
    ( const BCHP_Handle                hChip,
      uint16_t                        *pusChipId,
      uint16_t                        *pusChipMajorRev,
      uint16_t                        *pusChipMinorRev );

static BERR_Code BCHP_P_GetFeature
    ( const BCHP_Handle                hChip,
      const BCHP_Feature               eFeature,
      void                            *pFeatureValue );

static BERR_Code BCHP_P_ResetMagnumCores
    ( const BCHP_Handle                hChip );

static void BCHP_P_ResetRaagaCore
    ( const BCHP_Handle hChip, const BREG_Handle hReg );

static void BCHP_P_MonitorPvt
    ( BCHP_Handle                      hChip,
      BCHP_AvsSettings                *pSettings );

static BERR_Code BCHP_P_GetAvsData
    ( BCHP_Handle                      hChip,
      BCHP_AvsData                    *pData );

static BERR_Code BCHP_P_StandbyMode
    ( BCHP_Handle                      hChip,
      bool                             activate );

/***************************************************************************
 * Open BCM7429 Chip.
 *
 */
BERR_Code BCHP_Open7429
    ( BCHP_Handle                     *phChip,
      BREG_Handle                      hRegister )
{
    BCHP_P_Context *pChip;
    BCHP_P_7429_Context *p7429Chip;
    uint32_t ulChipFamilyIdReg;
    uint32_t ulChipIdReg;
    uint32_t ulIdx;
    BERR_Code rc;

    BDBG_ENTER(BCHP_Open7429);

    if((!phChip) ||
       (!hRegister))
    {
        BDBG_ERR(("Invalid parameter\n"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* If error ocurr user get a NULL *phChip */
    *phChip = NULL;

    /* Alloc the base chip context. */
    pChip = (BCHP_P_Context*)(BKNI_Malloc(sizeof(BCHP_P_Context)));
    if(!pChip)
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    /* Clear out the context and set defaults. */
    BKNI_Memset((void*)pChip, 0x0, sizeof(BCHP_P_Context));

    p7429Chip = (BCHP_P_7429_Context*)
        (BKNI_Malloc(sizeof(BCHP_P_7429_Context)));
    if(!p7429Chip)
    {
        BKNI_Free(pChip);
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    /* Clear out the context and set defaults. */
    BKNI_Memset((void*)p7429Chip, 0x0, sizeof(BCHP_P_7429_Context));

    /* Fill up the base chip context. */
    pChip->chipHandle       = (void*)p7429Chip;
    pChip->regHandle        = hRegister;
    pChip->pCloseFunc       = BCHP_P_Close7429;
    pChip->pGetChipInfoFunc = BCHP_P_GetChipInfoComformWithBaseClass;
    pChip->pGetFeatureFunc  = BCHP_P_GetFeature;
    pChip->pMonitorPvtFunc  = BCHP_P_MonitorPvt;
    pChip->pGetAvsDataFunc  = BCHP_P_GetAvsData;
    pChip->pStandbyModeFunc = BCHP_P_StandbyMode;

    /* Fill up the chip context. */
    p7429Chip->ulBlackMagic = sizeof(BCHP_P_7429_Context);
    p7429Chip->hRegister    = hRegister;


    /* Chip Family Register id is use for indexing into the table. */
    ulChipFamilyIdReg = BREG_Read32(hRegister, BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID);
    ulChipIdReg = BREG_Read32(hRegister, BCHP_SUN_TOP_CTRL_PRODUCT_ID);

/* decompose 32 bit chip family id for use with printf format string %x%c%d
Example: 0x74290000 becomes "7429A0" */
#define PRINT_CHIP(CHIPID) \
    ((CHIPID)>>16), ((((CHIPID)&0xF0)>>4)+'A'), ((CHIPID)&0x0F)

    for(ulIdx = 0; ulIdx < BCHP_P_CHIP_INFO_MAX_ENTRY; ulIdx++)
    {
        BDBG_MSG(("Supported Chip Family and revision: %x%c%d", PRINT_CHIP(s_aChipInfoTable[ulIdx].ulChipFamilyIdReg)));
        BDBG_MSG(("Supported Chip ID: %x", s_aChipInfoTable[ulIdx].usChipId));
        BDBG_MSG(("\n"));
    }

    /* Lookup corresponding chip id. */
    for(ulIdx = 0; ulIdx < BCHP_P_CHIP_INFO_MAX_ENTRY; ulIdx++)
    {
        const BCHP_P_7429_Info *compareChipInfo = &s_aChipInfoTable[ulIdx];

        if(compareChipInfo->ulChipFamilyIdReg == ulChipFamilyIdReg)
        {
            /* Chip Information. */
            p7429Chip->pChipInfo = compareChipInfo;
            break;
        }
        else if (ulIdx == BCHP_P_CHIP_INFO_MAX_ENTRY - 1 && compareChipInfo->usMajor == (ulChipFamilyIdReg&0xF0)>>4)
        {
            /* This is a future minor revision. We will allow it with a WRN. */
            BDBG_WRN(("An unknown minor revision %x%c%d has been detected. Certain operations may result in erratic behavior. Please confirm this chip revision is supported with this software.",
                PRINT_CHIP(ulChipFamilyIdReg)));
            p7429Chip->pChipInfo = compareChipInfo;
            break;
        }
    }

    if(!p7429Chip->pChipInfo)
    {
        BKNI_Free(p7429Chip);
        BKNI_Free(pChip);
        BDBG_ERR(("*****************************************************************\n"));
        BDBG_ERR(("ERROR ERROR ERROR ERROR \n"));
        BDBG_ERR(("Unsupported Revision: %x%c%d", PRINT_CHIP(ulChipFamilyIdReg)));
        BDBG_ERR(("*****************************************************************\n"));
        phChip = NULL;
        BDBG_ASSERT(phChip);
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BDBG_WRN(("found chip family %x%c%d, product %x%c%d", PRINT_CHIP(p7429Chip->pChipInfo->ulChipFamilyIdReg),PRINT_CHIP(p7429Chip->pChipInfo->ulChipFamilyIdReg)));

    /* Open BCHP_PWR - but first, do a reset on some of the Magnum controlled cores so
     * that they don't interfere with BCHP_PWR_Open's powering up/down.
     */
    BCHP_P_ResetMagnumCores( pChip );

    rc = BCHP_PWR_Open(&pChip->pwrManager, pChip);
    if (rc) {
    BKNI_Free(pChip);
    BKNI_Free(p7429Chip);
    return BERR_TRACE(rc);
    }

    /* Open AVS module */
    BCHP_P_AvsOpen(&p7429Chip->hAvsHandle, pChip);
    if(!p7429Chip->hAvsHandle)
    {
        BCHP_PWR_Close(pChip->pwrManager);
        BKNI_Free(pChip);
        BKNI_Free(p7429Chip);
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    /* This makes the handle available to nexus extensions for testing purposes */
    pChip->avsHandle = (BCHP_AVS_Handle)p7429Chip->hAvsHandle;

    /* All done. now return the new fresh context to user. */
    *phChip = (BCHP_Handle)pChip;


    #ifdef BCHP_PWR_RESOURCE_MAGNUM_CONTROLLED
        BCHP_PWR_AcquireResource(pChip, BCHP_PWR_RESOURCE_AVD0);
    #endif

    /* Clear AVD/SVD shutdown enable bit */
    BREG_Write32(hRegister, BCHP_DECODE_IP_SHIM_0_SOFTSHUTDOWN_CTRL_REG, 0x0);

    #ifdef BCHP_PWR_RESOURCE_MAGNUM_CONTROLLED
        BCHP_PWR_ReleaseResource(pChip, BCHP_PWR_RESOURCE_AVD0);
    #endif

    BDBG_LEAVE(BCHP_Open7429);
    return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
static BERR_Code BCHP_P_Close7429
    ( BCHP_Handle                      hChip )
{
    BCHP_P_7429_Context *p7429Chip;

    BDBG_ENTER(BCHP_P_Close7429);

    BCHP_P_GET_CONTEXT(hChip, p7429Chip);

    if(!p7429Chip)
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if (p7429Chip->hAvsHandle) {
        BCHP_P_AvsClose(p7429Chip->hAvsHandle);
        p7429Chip->hAvsHandle = NULL;
    }

    BCHP_PWR_Close(hChip->pwrManager);


    /* Invalidate the magic number. */
    p7429Chip->ulBlackMagic = 0;

    BKNI_Free((void*)p7429Chip);
    BKNI_Free((void*)hChip);

    BDBG_LEAVE(BCHP_P_Close7429);
    return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
static BERR_Code BCHP_P_GetChipInfoComformWithBaseClass
    ( const BCHP_Handle                hChip,
      uint16_t                        *pusChipId,
      uint16_t                        *pusChipRev )

{
    BERR_Code eStatus;
    uint16_t usMajor=0;
    uint16_t usMinor=0;

    eStatus = BERR_TRACE(BCHP_P_GetChipInfo(hChip, pusChipId,
        &usMajor, &usMinor));
    if(BERR_SUCCESS != eStatus)
    {
        return eStatus;
    }

    if(pusChipRev)
    {
        *pusChipRev = ((usMajor << BCHP_P_MAJOR_REV_SHIFT) + usMinor);
    }

    return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
static BERR_Code BCHP_P_GetChipInfo
    ( const BCHP_Handle                hChip,
      uint16_t                        *pusChipId,
      uint16_t                        *pusChipMajorRev,
      uint16_t                        *pusChipMinorRev )
{
    const BCHP_P_7429_Context *p7429Chip;

    BDBG_ENTER(BCHP_P_GetChipInfo);

    BCHP_P_GET_CONTEXT(hChip, p7429Chip);

    if(!p7429Chip)
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if(pusChipId)
    {
        *pusChipId = p7429Chip->pChipInfo->usChipId;
    }

    if(pusChipMajorRev)
    {
        *pusChipMajorRev = p7429Chip->pChipInfo->usMajor;
    }

    if(pusChipMinorRev)
    {
        *pusChipMinorRev = p7429Chip->pChipInfo->usMinor;
    }

    BDBG_LEAVE(BCHP_P_GetChipInfo);
    return BERR_SUCCESS;
}

/***************************************************************************
 * {private}
 *
 */
static BERR_Code BCHP_P_GetFeature
    ( const BCHP_Handle                hChip,
      const BCHP_Feature               eFeature,
      void                            *pFeatureValue )
{
    BERR_Code            rc = BERR_UNKNOWN;
    BCHP_P_7429_Context *p7429Chip;
    uint32_t             ulBondStatus;
    uint32_t             uiReg;
    uint32_t             ddr_type, ddrDevice;
    uint32_t             memc_config;

    BDBG_ENTER(BCHP_P_GetFeature);

    /* get base context */
    BCHP_P_GET_CONTEXT(hChip, p7429Chip);

    /* read bond-out status common for many features */
    ulBondStatus = BREG_Read32(p7429Chip->hRegister,
        BCHP_SUN_TOP_CTRL_OTP_OPTION_STATUS_0);

    /* which feature? */
    switch (eFeature)
    {
    case BCHP_Feature_e3DGraphicsCapable:
        /* 3D capable? (bool) */
        *(bool *)pFeatureValue = true;
        rc = BERR_SUCCESS;
        break;

    case BCHP_Feature_eDvoPortCapable:
        /* dvo port capable? (bool) */
        *(bool *)pFeatureValue = false;
        rc = BERR_SUCCESS;
        break;

    case BCHP_Feature_eMacrovisionCapable:
        /* macrovision capable? (bool) */
        *(bool *)pFeatureValue = BCHP_GET_FIELD_DATA(ulBondStatus,
            SUN_TOP_CTRL_OTP_OPTION_STATUS_0, otp_option_macrovision_disable) ? false : true;
        rc = BERR_SUCCESS;
        break;

    case BCHP_Feature_eMpegDecoderCount:
        /* number of MPEG decoders (int) */
        *(int *)pFeatureValue = 1;
        rc = BERR_SUCCESS;
        break;

    case BCHP_Feature_eHdcpCapable:
        /* HDCP capable? (bool) */
        *(bool *)pFeatureValue = BCHP_GET_FIELD_DATA(ulBondStatus,
            SUN_TOP_CTRL_OTP_OPTION_STATUS_0, otp_option_hdcp_tx_disable ) ? false : true;
        rc = BERR_SUCCESS;
        break;

    case BCHP_Feature_e3desCapable:
        /* 3DES capable? (bool) */
        *(bool *)pFeatureValue = true;
        rc = BERR_SUCCESS;
        break;

    case BCHP_Feature_e1080pCapable:
        /* 1080p Capable? (bool) */
        *(bool *)pFeatureValue = true;
        rc = BERR_SUCCESS;
        break;

    case BCHP_Feature_eMemCtrl1Capable:
    {
        *(bool *)pFeatureValue = true;
        rc = BERR_SUCCESS;
        break ;
    }

   case BCHP_Feature_eMemCtrl0DDR3ModeCapable:
       /* DDR Mode: 0 (DDR3 parts), 1 (DDR2 parts) */
       /* This should be DDR3 for 7429 */

       uiReg = BREG_Read32(p7429Chip->hRegister, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG);
       ddr_type = BCHP_GET_FIELD_DATA(uiReg, MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG, DDR_MODE);

       if (ddr_type)
       {
          *(bool *)pFeatureValue = false;
       }
       else
       {
          *(bool *)pFeatureValue = true;
       }

       rc = BERR_SUCCESS;
       break;

    case BCHP_Feature_eMemCtrl0DDRDeviceTechCount:

       /* Size of memory part in MBits ie: 256, 512, 1024 */
       /* Device Tech: 0 (256Mbits), 1 (512MBbits), 2 (1Gbit), 3 (2Gbit), 4 (4Gbit) */
       uiReg = BREG_Read32(p7429Chip->hRegister, BCHP_MEMC_DDR_0_CNTRLR_CONFIG);
       ddrDevice = BCHP_GET_FIELD_DATA(uiReg, MEMC_DDR_0_CNTRLR_CONFIG, DEVICE_TECH);

       switch(ddrDevice)
       {
          case 0:

             *(int *)pFeatureValue = 256;
             rc = BERR_SUCCESS;
             break;

          case 1:

             *(int *)pFeatureValue = 512;
             rc = BERR_SUCCESS;
             break;

          case 2:

             *(int *)pFeatureValue = 1024;
             rc = BERR_SUCCESS;
             break;

          case 3:

             *(int *)pFeatureValue = 2048;
             rc = BERR_SUCCESS;
             break;

          case 4:

             *(int *)pFeatureValue = 4096;
             rc = BERR_SUCCESS;
             break;
       }

       break;

    case BCHP_Feature_eMemCtrl0DramWidthCount:

       /* DRAM Width: 0 (32 bit), 1 (16 bit) */
       uiReg = BREG_Read32(p7429Chip->hRegister, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG);
       memc_config = BCHP_GET_FIELD_DATA(uiReg, MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG, DRAM_WIDTH);

       if (memc_config == 0)
       {
          *(int *)pFeatureValue = 32;
       }
       else
       {
          *(int *)pFeatureValue = 16;
       }

       rc = BERR_SUCCESS;
       break;

    case BCHP_Feature_eMemCtrl1DDR3ModeCapable:    /* True = DDR3 */
       rc = BERR_NOT_SUPPORTED;
       break;

    case BCHP_Feature_eMemCtrl1DDRDeviceTechCount:


       BDBG_ERR(("DDR 1 is not supported"));
       rc = BERR_NOT_SUPPORTED;
       break;

    case BCHP_Feature_eMemCtrl1DramWidthCount:

       rc = BERR_NOT_SUPPORTED;
       break;

    default:
        rc = BERR_TRACE(BERR_UNKNOWN);
    }

    /* return result */
    BDBG_LEAVE(BCHP_P_GetFeature);
    return rc;
}

/***************************************************************************
 * {private}
 *
 */
static BERR_Code BCHP_P_ResetMagnumCores
    ( const BCHP_Handle                hChip )

{
    BCHP_P_ResetRaagaCore(hChip, hChip->regHandle); /* must be done first before all other cores. */

    /* Note, SW_INIT set/clear registers don't need read-modify-write. */
    BREG_Write32(hChip->regHandle, BCHP_SUN_TOP_CTRL_SW_INIT_0_SET,
          BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, xpt_sw_init, 1 )
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, svd0_sw_init, 1 )
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, vec_sw_init, 1 )
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, aio_sw_init, 1 )
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, bvn_sw_init, 1 )
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, raaga_sw_init, 1 )
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, rfm_sw_init, 1 )
         );

    /* Now clear the reset. */
    BREG_Write32(hChip->regHandle, BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR,
           BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, xpt_sw_init, 1)
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, svd0_sw_init, 1)
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, vec_sw_init, 1)
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, aio_sw_init, 1)
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, bvn_sw_init, 1)
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, raaga_sw_init, 1)
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, rfm_sw_init, 1)
        );


    /* Note, SW_INIT set/clear registers don't need read-modify-write. */
    BREG_Write32(hChip->regHandle, BCHP_SUN_TOP_CTRL_SW_INIT_1_SET,
          BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_1_SET, sid_sw_init, 1 )
         );

    /* Now clear the reset. */
    BREG_Write32(hChip->regHandle, BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR,
           BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_1_CLEAR, sid_sw_init, 1)
         );

    return BERR_SUCCESS;
}

#include "bchp_raaga_dsp_misc.h"

/* re-use functions in bchp_pwr_impl.c in order to power-on raaga */
void BCHP_PWR_P_HW_PLL_RAAGA_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_RAAGA_PLL_CH0_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_RAAGA0_Control(BCHP_Handle handle, bool activate);


/* SW workaround to ensure we can hit the Raaga SW_INIT safely */
static void BCHP_P_ResetRaagaCore(const BCHP_Handle hChip, const BREG_Handle hReg)
{
    uint32_t val;

    /* unconditionally turn on everything that's needed to do the register write below.
       we don't know what power state we were left in. BCHP_PWR_Open() will later turn stuff off as needed */
    BCHP_PWR_P_HW_PLL_RAAGA_Control(hChip, true);
    BCHP_PWR_P_HW_PLL_RAAGA_PLL_CH0_Control(hChip, true);
    BCHP_PWR_P_HW_RAAGA0_Control(hChip, true);

    val = BREG_Read32(hReg,BCHP_RAAGA_DSP_MISC_SOFT_INIT) ;
	val = (val & ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_INIT, DO_SW_INIT)))|
	 (BCHP_FIELD_DATA(RAAGA_DSP_MISC_SOFT_INIT, DO_SW_INIT,1));
	BREG_Write32(hReg,BCHP_RAAGA_DSP_MISC_SOFT_INIT, val);

	val = BREG_Read32(hReg,BCHP_RAAGA_DSP_MISC_REVISION) ;
	val = BREG_Read32(hReg,BCHP_RAAGA_DSP_MISC_REVISION) ;

	BDBG_MSG(("REV ID VAL = 0x%x", val));

	val = BREG_Read32(hReg,BCHP_RAAGA_DSP_MISC_SOFT_INIT) ;
	val &=  ~(BCHP_MASK(RAAGA_DSP_MISC_SOFT_INIT,INIT_PROC_B));
	BREG_Write32(hReg,BCHP_RAAGA_DSP_MISC_SOFT_INIT, val);

	/*RDB says no need of Read modify write.*/
	val = 0;
	val = (BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_INIT_0_SET, raaga_sw_init,1));
	BREG_Write32(hReg,BCHP_SUN_TOP_CTRL_SW_INIT_0_SET, val);

	BKNI_Delay(2);

	/*RDB says no need of Read modify write.*/
	val = 0;
	val = (BCHP_FIELD_DATA(SUN_TOP_CTRL_SW_INIT_0_CLEAR, raaga_sw_init,1));
	BREG_Write32(hReg,BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR, val);

    return;
}

/* This gets called regularly to handle the AVS processing */
static void BCHP_P_MonitorPvt( BCHP_Handle hChip, BCHP_AvsSettings *pSettings )
{
    BCHP_P_7429_Context *p7429Chip;

    BDBG_ENTER(BCHP_P_MonitorPvt);
    BSTD_UNUSED(pSettings);

    /* get base context */
    BCHP_P_GET_CONTEXT(hChip, p7429Chip);

    if (p7429Chip->hAvsHandle)
        BCHP_P_AvsMonitorPvt(p7429Chip->hAvsHandle);

    BDBG_LEAVE(BCHP_P_MonitorPvt);
}

static BERR_Code BCHP_P_GetAvsData( BCHP_Handle hChip, BCHP_AvsData *pData )
{
    BCHP_P_7429_Context *p7429Chip;

    BDBG_ASSERT(pData);

    BDBG_ENTER(BCHP_GetAVdata);

    /* get base context */
    BCHP_P_GET_CONTEXT(hChip, p7429Chip);

    if (p7429Chip->hAvsHandle)
    	BCHP_P_AvsGetData(p7429Chip->hAvsHandle, pData);

    BDBG_LEAVE(BCHP_GetAVdata);
    return BERR_SUCCESS;
}

static BERR_Code BCHP_P_StandbyMode( BCHP_Handle hChip, bool activate )
{
    BCHP_P_7429_Context *p7429Chip;

    BDBG_ENTER(BCHP_P_StandbyMode);

    /* get base context */
    BCHP_P_GET_CONTEXT(hChip, p7429Chip);

    /* Do anything required for CHP Standby changes */

    if (p7429Chip->hAvsHandle)
        BCHP_P_AvsStandbyMode(p7429Chip->hAvsHandle, activate);

    BDBG_LEAVE(BCHP_P_StandbyMode);
    return BERR_SUCCESS;
}

/* End of File */
