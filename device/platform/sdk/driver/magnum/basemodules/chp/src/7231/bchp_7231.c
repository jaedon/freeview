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
 * $brcm_Workfile: bchp_7231.c $
 * $brcm_Revision: Hydra_Software_Devel/22 $
 * $brcm_Date: 9/28/12 1:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/src/7231/bchp_7231.c $
 * 
 * Hydra_Software_Devel/22   9/28/12 1:14p randyjew
 * HW7344-467: Merge raaga reset core sequence
 * 
 * Hydra_Software_Devel/21   7/20/12 4:47p rjlewis
 * SW7425-3461: messed up the cut & paste.
 * 
 * Hydra_Software_Devel/20   7/20/12 4:34p rjlewis
 * SW7425-3461: moved get data function from chip specific to common.
 * 
 * Hydra_Software_Devel/19   4/5/12 2:57p katrep
 * SW7231-705:cleanedup the code
 * 
 * Hydra_Software_Devel/18   4/2/12 12:09p katrep
 * SW7231-705:fixed the issue with chip version testing
 * 
 * Hydra_Software_Devel/17   3/6/12 3:12p katrep
 * SW7231-317:update for B2
 * 
 * Hydra_Software_Devel/16   3/6/12 11:59a jtna
 * SW7425-2362: add sw workaround for raaga sw init problem
 * 
 * Hydra_Software_Devel/15   2/17/12 1:20p katrep
 * SW7231-565:add b2 build
 * 
 * Hydra_Software_Devel/14   2/15/12 8:11p katrep
 * SW7231-317:compiler warning
 * 
 * Hydra_Software_Devel/13   2/15/12 7:44p katrep
 * SW7231-317:fixed issue with SID for BCHP != B0
 * 
 * Hydra_Software_Devel/12   2/13/12 7:10p katrep
 * SW7231-317:7231B2 binary compatibility
 * 
 * Hydra_Software_Devel/11   10/10/11 4:53p katrep
 * SW7231-317:add 7231 B1 support
 * 
 * Hydra_Software_Devel/10   8/20/11 5:57p rjlewis
 * SW7346-117: Added support for AVS.
 * 
 * Hydra_Software_Devel/9   8/10/11 11:16a katrep
 * SW7231-317:add support for 7231B0
 * 
 * Hydra_Software_Devel/8   8/9/11 5:42p katrep
 * SW7231-317:add support for 7231B0
 * 
 * Hydra_Software_Devel/7   7/25/11 4:13p gskerl
 * SW7231-246: Refactored the Magnum core reset logic to address some
 * power management related issues
 * 
 * Hydra_Software_Devel/6   4/29/11 3:47p gmohile
 * SW7231-128 : Add bchp_pwr support for 7231
 * 
 * Hydra_Software_Devel/5   12/24/10 8:47a katrep
 * SW7231-4:updated to include correct chip ID
 * 
 * Hydra_Software_Devel/4   12/17/10 5:52p katrep
 * SW7231-4:more improvements
 * 
 * Hydra_Software_Devel/3   12/9/10 2:12p katrep
 * SW7231-4:fixed compiler warning
 * 
 * Hydra_Software_Devel/2   11/29/10 3:43p katrep
 * SW7231-4:updated to pass compiler
 * 
 * Hydra_Software_Devel/1   11/3/10 5:41p katrep
 * SW7231-4: add initial 7231 support
 *
 *
 **************************************************************************/
 
#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "breg_mem.h"
#include "bchp.h"
#include "bchp_priv.h"
#include "bchp_7231.h"
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
typedef struct BCHP_P_7231_Info
{
    uint32_t      ulChipFamilyIdReg; /* index into the table. */

    /* Chip Id */
    uint16_t      usChipId;

    /* Major revision */
    uint16_t      usMajor;

    /* Minor revision */
    uint16_t      usMinor;

    /* TODO: Other features or infos if needed */
} BCHP_P_7231_Info;


/* Lookup table for chip features and etc.
 * The are many times when the chip device id register
 * not conforming to the standard numbering convention. We do
 * it this way to work-around those problems.
 *
 * TODO: Update this table to support new revisions.
 */
static const BCHP_P_7231_Info s_aChipInfoTable[] =
{
    /* Chip Family contains the major and minor revs */
#if BCHP_VER == BCHP_VER_A0
    /* A0 code will run on A0 */
   {0x72310000, BCHP_BCM7231, BCHP_MAJOR_A, BCHP_MINOR_0},
#elif BCHP_VER == BCHP_VER_B0
    /* B0 code will run on B0,B1 & B2 */
   {0x72310010, BCHP_BCM7231, BCHP_MAJOR_B, BCHP_MINOR_0},
   {0x72310011, BCHP_BCM7231, BCHP_MAJOR_B, BCHP_MINOR_1},
   {0x72310012, BCHP_BCM7231, BCHP_MAJOR_B, BCHP_MINOR_1},
#elif BCHP_VER == BCHP_VER_B2
   {0x72310012, BCHP_BCM7231, BCHP_MAJOR_B, BCHP_MINOR_2},
#else
   #error "Port required"
#endif
};


/* Chip context */
typedef struct BCHP_P_7231_Context
{
    uint32_t                           ulBlackMagic;
    BREG_Handle                        hRegister;
    const BCHP_P_7231_Info            *pChipInfo;
    BCHP_P_AvsHandle                  hAvsHandle;
} BCHP_P_7231_Context;

/* Max entry of lookup table */
#define BCHP_P_CHIP_INFO_MAX_ENTRY \
    (sizeof(s_aChipInfoTable) / sizeof(BCHP_P_7231_Info))

/* This macro checks for a validity of a handle, and
 * cast to context pointer. */
#define BCHP_P_GET_CONTEXT(handle, context) \
{ \
    if(!(handle) || \
       !((handle)->chipHandle) || \
       (((BCHP_P_7231_Context*)((handle)->chipHandle))->ulBlackMagic != \
       sizeof(BCHP_P_7231_Context))) \
    { \
        BDBG_ERR(("Corrupted context handle\n")); \
        (context) = NULL; \
    } \
    else \
    { \
        (context) = (BCHP_P_7231_Context*)((handle)->chipHandle); \
    } \
    BDBG_ASSERT(context); \
}

/* Static function prototypes */
static BERR_Code BCHP_P_Close7231
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
 * Open BCM7231 Chip.
 *
 */
BERR_Code BCHP_Open7231
    ( BCHP_Handle                     *phChip,
      BREG_Handle                      hRegister )
{
    BCHP_P_Context *pChip;
    BCHP_P_7231_Context *p7231Chip;
    uint32_t ulChipFamilyIdReg;
    uint32_t ulChipIdReg;
    uint32_t ulIdx;
    BERR_Code rc;

    BDBG_ENTER(BCHP_Open7231);

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

    p7231Chip = (BCHP_P_7231_Context*)
        (BKNI_Malloc(sizeof(BCHP_P_7231_Context)));
    if(!p7231Chip)
    {
        BKNI_Free(pChip);
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    /* Clear out the context and set defaults. */
    BKNI_Memset((void*)p7231Chip, 0x0, sizeof(BCHP_P_7231_Context));

    /* Fill up the base chip context. */
    pChip->chipHandle       = (void*)p7231Chip;
    pChip->regHandle        = hRegister;
    pChip->pCloseFunc       = BCHP_P_Close7231;
    pChip->pGetChipInfoFunc = BCHP_P_GetChipInfoComformWithBaseClass;
    pChip->pGetFeatureFunc  = BCHP_P_GetFeature;
    pChip->pMonitorPvtFunc  = BCHP_P_MonitorPvt;
    pChip->pGetAvsDataFunc  = BCHP_P_GetAvsData;
    pChip->pStandbyModeFunc = BCHP_P_StandbyMode;

    /* Fill up the chip context. */
    p7231Chip->ulBlackMagic = sizeof(BCHP_P_7231_Context);
    p7231Chip->hRegister    = hRegister;

    /* Chip Family Register id is use for indexing into the table. */
    ulChipFamilyIdReg = BREG_Read32(hRegister, BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID);
    ulChipIdReg = BREG_Read32(hRegister, BCHP_SUN_TOP_CTRL_PRODUCT_ID);
    ulChipIdReg >>= 16;

/* decompose 32 bit chip family id for use with printf format string %x%c%d
Example: 0x72310000 becomes "77231A0" */
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
        const BCHP_P_7231_Info *compareChipInfo = &s_aChipInfoTable[ulIdx];

        if(compareChipInfo->ulChipFamilyIdReg == ulChipFamilyIdReg)
        {
            /* Chip Information. */
            p7231Chip->pChipInfo = compareChipInfo;
            break;
        }
        else if (ulIdx == BCHP_P_CHIP_INFO_MAX_ENTRY - 1 && compareChipInfo->usMajor == (ulChipFamilyIdReg&0xF0)>>4)
        {
            /* This is a future minor revision. We will allow it with a WRN. */
            BDBG_WRN(("An unknown minor revision %x%c%d has been detected. Certain operations may result in erratic behavior. Please confirm this chip revision is supported with this software.",
                PRINT_CHIP(ulChipFamilyIdReg)));
            p7231Chip->pChipInfo = compareChipInfo;
            break;
        }
    }

    if(!p7231Chip->pChipInfo)
    {
        BKNI_Free(p7231Chip);
        BKNI_Free(pChip);
        BDBG_ERR(("*****************************************************************\n"));
        BDBG_ERR(("ERROR ERROR ERROR ERROR \n"));
        BDBG_ERR(("Unsupported Revision: %x%c%d", PRINT_CHIP(ulChipIdReg)));
        BDBG_ERR(("*****************************************************************\n"));
        phChip = NULL;
        BDBG_ASSERT(phChip);
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    BDBG_MSG(("found %x%c%d", PRINT_CHIP(p7231Chip->pChipInfo->ulChipFamilyIdReg)));

    /* Open BCHP_PWR - but first, do a reset on some of the Magnum controlled cores so 
     * that they don't interfere with BCHP_PWR_Open's powering up/down.
     */
    BCHP_P_ResetMagnumCores( pChip );
    
    rc = BCHP_PWR_Open(&pChip->pwrManager, pChip); 
    if (rc) {
        BKNI_Free(pChip);
        BKNI_Free(p7231Chip);
        return BERR_TRACE(rc);
    }

    /* Open AVS module */
    BCHP_P_AvsOpen(&p7231Chip->hAvsHandle, pChip);
    if(!p7231Chip->hAvsHandle)
    {
        BCHP_PWR_Close(pChip->pwrManager);
        BKNI_Free(pChip);
        BKNI_Free(p7231Chip);
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    /* All done. now return the new fresh context to user. */
    *phChip = (BCHP_Handle)pChip;

    /* Now, if we have power management, do a second reset of the Magnum cores. This second reset is
     * necessary to reset the cores that might have been powered down during the first reset above.
     * Make sure the Magnum hardware is powered up before the reset, then power it down afterwards.
     */
    #ifdef BCHP_PWR_RESOURCE_MAGNUM_CONTROLLED
        BCHP_PWR_AcquireResource(pChip, BCHP_PWR_RESOURCE_MAGNUM_CONTROLLED);
    #endif

    #if BCHP_PWR_SUPPORT
        BCHP_P_ResetMagnumCores( pChip );
    #endif
    
    /* Clear AVD/SVD shutdown enable bit */
    BREG_Write32(hRegister, BCHP_DECODE_IP_SHIM_0_SOFTSHUTDOWN_CTRL_REG, 0x0);
    
    #ifdef BCHP_PWR_RESOURCE_MAGNUM_CONTROLLED
        BCHP_PWR_ReleaseResource(pChip, BCHP_PWR_RESOURCE_MAGNUM_CONTROLLED);
    #endif

    BDBG_LEAVE(BCHP_Open7231);
    return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
static BERR_Code BCHP_P_Close7231
    ( BCHP_Handle                      hChip )
{
    BCHP_P_7231_Context *p7231Chip;

    BDBG_ENTER(BCHP_P_Close7231);

    BCHP_P_GET_CONTEXT(hChip, p7231Chip);

    if(!p7231Chip)
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    
    if (p7231Chip->hAvsHandle) {
        BCHP_P_AvsClose(p7231Chip->hAvsHandle);
        p7231Chip->hAvsHandle = NULL;
    }

    BCHP_PWR_Close(hChip->pwrManager);

    /* Invalidate the magic number. */
    p7231Chip->ulBlackMagic = 0;

    BKNI_Free((void*)p7231Chip);
    BKNI_Free((void*)hChip);

    BDBG_LEAVE(BCHP_P_Close7231);
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
    const BCHP_P_7231_Context *p7231Chip;

    BDBG_ENTER(BCHP_P_GetChipInfo);

    BCHP_P_GET_CONTEXT(hChip, p7231Chip);

    if(!p7231Chip)
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if(pusChipId)
    {
        *pusChipId = p7231Chip->pChipInfo->usChipId;
    }

    if(pusChipMajorRev)
    {
        *pusChipMajorRev = p7231Chip->pChipInfo->usMajor;
    }

    if(pusChipMinorRev)
    {
        *pusChipMinorRev = p7231Chip->pChipInfo->usMinor;
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
    BCHP_P_7231_Context *p7231Chip;
    uint32_t             ulBondStatus;
    uint32_t             uiReg;
    uint32_t             ddr_type, ddrDevice;
    uint32_t             memc_config;

    BDBG_ENTER(BCHP_P_GetFeature);

    /* get base context */
    BCHP_P_GET_CONTEXT(hChip, p7231Chip);

    /* read bond-out status common for many features */
    ulBondStatus = BREG_Read32(p7231Chip->hRegister,
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
            SUN_TOP_CTRL_OTP_OPTION_STATUS_0, otp_option_hdcp_disable ) ? false : true;
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
       /* This should be DDR3 for 7231 */

       uiReg = BREG_Read32(p7231Chip->hRegister, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG);
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
       uiReg = BREG_Read32(p7231Chip->hRegister, BCHP_MEMC_DDR_0_CNTRLR_CONFIG);
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
       uiReg = BREG_Read32(p7231Chip->hRegister, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG);
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
    BREG_Handle  hRegister = hChip->regHandle;
    
    BCHP_P_ResetRaagaCore(hChip, hRegister); /* do this before all the other cores */

    /* Note, SW_INIT set/clear registers don't need read-modify-write. */
    BREG_Write32(hChip->regHandle, BCHP_SUN_TOP_CTRL_SW_INIT_0_SET,
          BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, xpt_sw_init, 1 )
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, svd0_sw_init, 1 )
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, vec_sw_init, 1 )
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, aio_sw_init, 1 )
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, bvn_sw_init, 1 )
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, raaga_sw_init, 1 )
         );

    /* Now clear the reset. */
    BREG_Write32(hChip->regHandle, BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR,
           BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, xpt_sw_init, 1)
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, svd0_sw_init, 1)
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, vec_sw_init, 1)
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, aio_sw_init, 1)
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, bvn_sw_init, 1)
        | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, raaga_sw_init, 1)
         );

#if BCHP_VER >= BCHP_VER_B0
    /* Note, SW_INIT set/clear registers don't need read-modify-write. */
    BREG_Write32(hChip->regHandle, BCHP_SUN_TOP_CTRL_SW_INIT_1_SET,
        BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_1_SET, sid_sw_init, 1 )
         );
    /* Now clear the reset. */
    BREG_Write32(hChip->regHandle, BCHP_SUN_TOP_CTRL_SW_INIT_1_CLEAR,
         BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_1_CLEAR, sid_sw_init, 1)
         );
#endif

    return BERR_SUCCESS;
}

#include "bchp_raaga_dsp_misc.h"

/* re-use functions in bchp_pwr_impl.c in order to power-on raaga */
void BCHP_PWR_P_HW_PLL_AVD_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_AVD_CH2_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_VEC_AIO_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_RAAGA_Control(BCHP_Handle handle, bool activate);

/* SW workaround to ensure we can hit the Raaga SW_INIT safely */
static void BCHP_P_ResetRaagaCore(const BCHP_Handle hChip, const BREG_Handle hReg)
{
    uint32_t val;
    
    /* unconditionally turn on everything that's needed to do the register write below. 
       we don't know what power state we were left in. BCHP_PWR_Open() will later turn stuff off as needed */
    BCHP_PWR_P_HW_PLL_AVD_Control(hChip, true);
    BCHP_PWR_P_HW_PLL_AVD_CH2_Control(hChip, true);
    BCHP_PWR_P_HW_RAAGA_Control(hChip, true);

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
    BCHP_P_7231_Context *p7231Chip;

    BDBG_ENTER(BCHP_P_MonitorPvt);
    BSTD_UNUSED(pSettings);

    /* get base context */
    BCHP_P_GET_CONTEXT(hChip, p7231Chip);

    if (p7231Chip->hAvsHandle)
        BCHP_P_AvsMonitorPvt(p7231Chip->hAvsHandle);

    BDBG_LEAVE(BCHP_P_MonitorPvt);
}

/* This provides the current AVS data */
static BERR_Code BCHP_P_GetAvsData( BCHP_Handle hChip, BCHP_AvsData *pData )
{
    BCHP_P_7231_Context *p7231Chip;

    BDBG_ASSERT(pData);

    BDBG_ENTER(BCHP_GetAVdata);

    /* get base context */
    BCHP_P_GET_CONTEXT(hChip, p7231Chip);

    if (p7231Chip->hAvsHandle)
    	BCHP_P_AvsGetData(p7231Chip->hAvsHandle, pData);

    BDBG_LEAVE(BCHP_GetAVdata);
    return BERR_SUCCESS;
}

static BERR_Code BCHP_P_StandbyMode( BCHP_Handle hChip, bool activate )
{
    BCHP_P_7231_Context *p7231Chip;

    BDBG_ENTER(BCHP_P_StandbyMode);

    /* get base context */
    BCHP_P_GET_CONTEXT(hChip, p7231Chip);

    /* Do anything required for CHP Standby changes */

    if (p7231Chip->hAvsHandle)
        BCHP_P_AvsStandbyMode(p7231Chip->hAvsHandle, activate);

    BDBG_LEAVE(BCHP_P_StandbyMode);
    return BERR_SUCCESS;
}

/* End of File */

