/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_7425.c $
 * $brcm_Revision: Hydra_Software_Devel/29 $
 * $brcm_Date: 10/17/12 11:26a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/src/7425/bchp_7425.c $
 * 
 * Hydra_Software_Devel/29   10/17/12 11:26a jessem
 * SW7425-4098: Updated reset raaga core sequence.
 *
 * Hydra_Software_Devel/28   7/20/12 4:32p rjlewis
 * SW7425-3461: moved get data function from chip specific to common.
 *
 * Hydra_Software_Devel/27   7/11/12 2:41p vanessah
 * SW7425-3471: fix ViCE SW_INIT
 *
 * Hydra_Software_Devel/26   3/19/12 3:06p jessem
 * SW7425-2605: Moved BCHP_P_ResetMagnumCores() after chip id check.
 *
 * Hydra_Software_Devel/25   3/15/12 1:44p jessem
 * SW7425-2605: Move BCHP_P_ResetRaagaCore to BCHP_P_ResetMagnumCores.
 * Removed disabling of chip id runtime detection. Removed 7422A1
 * detection. Removed writing to GISB_ARB_REQ registers.
 *
 * Hydra_Software_Devel/24   3/2/12 4:35p jtna
 * SW7425-2362: add sw workaround for raaga sw init problem
 *
 * Hydra_Software_Devel/23   2/15/12 3:26p jessem
 * SW7425-2225: Disabled runtime chip ID identification for B1 and B2.
 *
 * Hydra_Software_Devel/22   2/10/12 5:25p rjlewis
 * SW7425-2218: removed AvsLock/Unlock and added get handle.
 *
 * Hydra_Software_Devel/21   2/6/12 1:38p rjlewis
 * SW7425-2218: Added support for lock and unlock
 *
 * Hydra_Software_Devel/20   1/19/12 3:58p jessem
 * SW7425-2227: Added B2 in chip info table.
 *
 * Hydra_Software_Devel/19   11/11/11 4:22p jessem
 * SW7425-1723: Disabled runtime chip version mismatch detection for
 * 7425Bx.
 *
 * Hydra_Software_Devel/18   11/7/11 11:11a jessem
 * SW7425-1693: Added 7425B1 to s_aChipInfoTable.
 *
 * Hydra_Software_Devel/17   9/12/11 11:59p hongtaoz
 * SW7346-117: get chip info before avs open to avoid segfault;
 *
 * Hydra_Software_Devel/16   8/16/11 11:35a rjlewis
 * SW7346-117: Added support for chip specific standby mode function
 * (currently supports only AVS).
 *
 * Hydra_Software_Devel/15   8/15/11 1:36p rjlewis
 * SW7346-117: AVS open needs to be after PWR open and need chip handle
 * instead of register.
 *
 * Hydra_Software_Devel/14   8/8/11 4:51p rjlewis
 * SW7346-117: merging to main.
 *
 * Hydra_Software_Devel/13   7/29/11 3:31p gmohile
 * SW7425-533 : Fix sw core reset
 *
 * Hydra_Software_Devel/12   6/27/11 11:56a gmohile
 * SW7425-533 : Add power management support
 *
 * Hydra_Software_Devel/11   6/9/11 6:56p nickh
 * SW7425-533: Add BCHP_PWR support
 *
 * Hydra_Software_Devel/SW7346-117/2   8/4/11 6:37p rjlewis
 * SW7346-117: Added support for AVS.
 *
 * Hydra_Software_Devel/SW7346-117/1   8/3/11 1:12p rjlewis
 * SW7346-117: Added support for AVS data.
 *
 * Hydra_Software_Devel/10   4/7/11 11:36p hongtaoz
 * SW7425-112: add B0 support;
 *
 * Hydra_Software_Devel/9   4/5/11 7:03p nickh
 * SW7422-412: Merge 7422 bchp with 7425
 *
 * Hydra_Software_Devel/8   3/25/11 10:42p hongtaoz
 * SW7425-157: clean up nexus startup errors:
 *
 * Hydra_Software_Devel/7   3/15/11 12:49a hongtaoz
 * SW7425-170: add 7425A1 support;
 *
 * Hydra_Software_Devel/6   12/7/10 10:45a nilesh
 * SW7425-1: Moved VICE2 SUN_TOP init to bchp_7425.c
 *
 * Hydra_Software_Devel/5   12/3/10 9:50a nilesh
 * SW7425-16: Clear avd/svd shutdown bits
 *
 * Hydra_Software_Devel/4   10/11/10 4:13p hongtaoz
 * SW7425-7: removed compile warning;
 *
 * Hydra_Software_Devel/3   10/11/10 4:05p hongtaoz
 * SW7425-7: uninit cores after sw_init;
 *
 * Hydra_Software_Devel/2   10/11/10 11:22a hongtaoz
 * SW7425-7: refactoring ddr striping parameter programming to BCHP
 * GetFeature;
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "breg_mem.h"
#include "bchp.h"
#include "bchp_priv.h"
#include "bchp_7425.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_decode_sd_0.h"
#include "bchp_decode_sd_1.h"
#include "bchp_decode_ip_shim_0.h"
#include "bchp_decode_ip_shim_1.h"
#include "bchp_sun_gisb_arb.h"

#include "bchp_memc_ddr23_shim_addr_cntl_0.h"
#include "bchp_memc_ddr23_shim_addr_cntl_1.h"
#include "bchp_memc_ddr_0.h"
#include "bchp_memc_ddr_1.h"
#include "bchp_pwr.h"

#include "bchp_avs_ro_registers_0.h"
#include "bchp_avs.h"

BDBG_MODULE(BCHP);

/* Miscellaneous macros. */
#define BCHP_P_MAJOR_REV_SHIFT          (4)

/* Chip info and features */
typedef struct BCHP_P_7425_Info
{
    uint32_t      ulChipFamilyIdReg; /* index into the table. */

    /* Chip Id */
    uint16_t      usChipId;

    /* Major revision */
    uint16_t      usMajor;

    /* Minor revision */
    uint16_t      usMinor;

    /* TODO: Other features or infos if needed */
} BCHP_P_7425_Info;


/* Lookup table for chip features and etc.
 * The are many times when the chip device id register
 * not conforming to the standard numbering convention. We do
 * it this way to work-around those problems.
 *
 * TODO: Update this table to support new revisions.
 */
static const BCHP_P_7425_Info s_aChipInfoTable[] =
{
    /* Chip Family contains the major and minor revs */
#if BCHP_VER == BCHP_VER_A0
	/* A0 code will run on A0 */
   {0x74250000, BCHP_BCM7425, BCHP_MAJOR_A, BCHP_MINOR_0},
#elif BCHP_VER == BCHP_VER_A1
    /* A1 code will run on A1 */
   {0x74250001, BCHP_BCM7425, BCHP_MAJOR_A, BCHP_MINOR_1},
#elif BCHP_VER == BCHP_VER_B0
	/* B0 code will run on B0 */
	{0x74250010, BCHP_BCM7425, BCHP_MAJOR_B, BCHP_MINOR_0},
#elif BCHP_VER == BCHP_VER_B1
    /* B1 code will run on B1 */
    {0x74250011, BCHP_BCM7425, BCHP_MAJOR_B, BCHP_MINOR_1},
#elif BCHP_VER == BCHP_VER_B2
    /* B2 code will run on B2 */
    {0x74250012, BCHP_BCM7425, BCHP_MAJOR_B, BCHP_MINOR_2},
#else
    #error "Port required"
#endif
};


/* Chip context */
typedef struct BCHP_P_7425_Context
{
    uint32_t                           ulBlackMagic;
    BREG_Handle                        hRegister;
    const BCHP_P_7425_Info            *pChipInfo;
    BCHP_P_AvsHandle                  hAvsHandle;
} BCHP_P_7425_Context;

/* Max entry of lookup table */
#define BCHP_P_CHIP_INFO_MAX_ENTRY \
    (sizeof(s_aChipInfoTable) / sizeof(BCHP_P_7425_Info))

/* This macro checks for a validity of a handle, and
 * cast to context pointer. */
#define BCHP_P_GET_CONTEXT(handle, context) \
{ \
    if(!(handle) || \
       !((handle)->chipHandle) || \
       (((BCHP_P_7425_Context*)((handle)->chipHandle))->ulBlackMagic != \
       sizeof(BCHP_P_7425_Context))) \
    { \
        BDBG_ERR(("Corrupted context handle\n")); \
        (context) = NULL; \
    } \
    else \
    { \
        (context) = (BCHP_P_7425_Context*)((handle)->chipHandle); \
    } \
    BDBG_ASSERT(context); \
}

/* Static function prototypes */
static BERR_Code BCHP_P_Close7425
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
    ( const BCHP_Handle                hChip,
      const BREG_Handle                hReg );

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
 * Open BCM7425 Chip.
 *
 */
BERR_Code BCHP_Open7425
	( BCHP_Handle                     *phChip,
	  BREG_Handle                      hRegister )
{
	BCHP_P_Context *pChip;
	BCHP_P_7425_Context *p7425Chip;
	uint32_t ulChipFamilyIdReg;
	uint32_t ulChipIdReg;
	uint32_t ulIdx;
	BERR_Code rc;

	BDBG_ENTER(BCHP_Open7425);

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

	p7425Chip = (BCHP_P_7425_Context*)
	    (BKNI_Malloc(sizeof(BCHP_P_7425_Context)));
	if(!p7425Chip)
	{
	    BKNI_Free(pChip);
	    return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)p7425Chip, 0x0, sizeof(BCHP_P_7425_Context));

	/* Fill up the base chip context. */
	pChip->chipHandle       = (void*)p7425Chip;
	pChip->regHandle        = hRegister;
	pChip->pCloseFunc       = BCHP_P_Close7425;
	pChip->pGetChipInfoFunc = BCHP_P_GetChipInfoComformWithBaseClass;
	pChip->pGetFeatureFunc  = BCHP_P_GetFeature;
    pChip->pMonitorPvtFunc  = BCHP_P_MonitorPvt;
    pChip->pGetAvsDataFunc  = BCHP_P_GetAvsData;
	pChip->pStandbyModeFunc = BCHP_P_StandbyMode;

	/* Fill up the chip context. */
	p7425Chip->ulBlackMagic = sizeof(BCHP_P_7425_Context);
	p7425Chip->hRegister    = hRegister;

	/* Chip Family Register id is use for indexing into the table. */
    ulChipFamilyIdReg = BREG_Read32(hRegister, BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID);
    ulChipIdReg = BREG_Read32(hRegister, BCHP_SUN_TOP_CTRL_PRODUCT_ID);
    ulChipIdReg >>= 16;

/* decompose 32 bit chip family id for use with printf format string %x%c%d
Example: 0x74250000 becomes "7425A0" */
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
	    const BCHP_P_7425_Info *compareChipInfo = &s_aChipInfoTable[ulIdx];

		if(compareChipInfo->ulChipFamilyIdReg == ulChipFamilyIdReg)
	    {
	        /* Chip Information. */
	        p7425Chip->pChipInfo = compareChipInfo;
	        break;
	    }
		else if (ulIdx == BCHP_P_CHIP_INFO_MAX_ENTRY - 1 && compareChipInfo->usMajor == (ulChipFamilyIdReg&0xF0)>>4)
	    {
	        /* This is a future minor revision. We will allow it with a WRN. */
	        BDBG_WRN(("An unknown minor revision %x%c%d has been detected. Certain operations may result in erratic behavior. Please confirm this chip revision is supported with this software.",
	            PRINT_CHIP(ulChipFamilyIdReg)));
	        p7425Chip->pChipInfo = compareChipInfo;
	        break;
	    }
	}

	if(!p7425Chip->pChipInfo)
	{
		BKNI_Free(p7425Chip);
		BKNI_Free(pChip);
		BDBG_ERR(("*****************************************************************\n"));
		BDBG_ERR(("ERROR ERROR ERROR ERROR \n"));
		BDBG_ERR(("Unsupported Revision: %x%c%d", PRINT_CHIP(ulChipIdReg)));
		BDBG_ERR(("*****************************************************************\n"));
		phChip = NULL;
		BDBG_ASSERT(phChip);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	BDBG_MSG(("found %x%c%d", PRINT_CHIP(p7425Chip->pChipInfo->ulChipFamilyIdReg)));

    BCHP_P_ResetMagnumCores( pChip );

    /* Open BCHP_PWR */
    rc = BCHP_PWR_Open(&pChip->pwrManager, pChip);
    if (rc) {
        BKNI_Free(pChip);
        BKNI_Free(p7425Chip);
        return BERR_TRACE(rc);
    }

    /* Open AVS module */
    BCHP_P_AvsOpen(&p7425Chip->hAvsHandle, pChip);
    if(!p7425Chip->hAvsHandle)
    {
		BCHP_PWR_Close(pChip->pwrManager);
        BKNI_Free(pChip);
        BKNI_Free(p7425Chip);
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
	/* This makes the handle available to nexus extensions for testing purposes */
	pChip->avsHandle = (BCHP_AVS_Handle)p7425Chip->hAvsHandle;

	/* All done. now return the new fresh context to user. */
	*phChip = (BCHP_Handle)pChip;

	/* Clear AVD/SVD shutdown enable bit */
#if BCHP_PWR_RESOURCE_AVD0
	BCHP_PWR_AcquireResource(pChip, BCHP_PWR_RESOURCE_AVD0);
#endif
	BREG_Write32(hRegister, BCHP_DECODE_IP_SHIM_0_SOFTSHUTDOWN_CTRL_REG, 0x0);
#if BCHP_PWR_RESOURCE_AVD0
	BCHP_PWR_ReleaseResource(pChip, BCHP_PWR_RESOURCE_AVD0);
#endif

#if BCHP_PWR_RESOURCE_AVD1
	BCHP_PWR_AcquireResource(pChip, BCHP_PWR_RESOURCE_AVD1);
#endif
	BREG_Write32(hRegister, BCHP_DECODE_IP_SHIM_1_SOFTSHUTDOWN_CTRL_REG, 0x0);
#if BCHP_PWR_RESOURCE_AVD1
	BCHP_PWR_ReleaseResource(pChip, BCHP_PWR_RESOURCE_AVD1);
#endif

	BDBG_LEAVE(BCHP_Open7425);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
static BERR_Code BCHP_P_Close7425
    ( BCHP_Handle                      hChip )
{
    BCHP_P_7425_Context *p7425Chip;

    BDBG_ENTER(BCHP_P_Close7425);

    BCHP_P_GET_CONTEXT(hChip, p7425Chip);

    if(!p7425Chip)
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if (p7425Chip->hAvsHandle) {
		BCHP_P_AvsClose(p7425Chip->hAvsHandle);
    	p7425Chip->hAvsHandle = NULL;
	}

	BCHP_PWR_Close(hChip->pwrManager);

    /* Invalidate the magic number. */
    p7425Chip->ulBlackMagic = 0;

    BKNI_Free((void*)p7425Chip);
    BKNI_Free((void*)hChip);

    BDBG_LEAVE(BCHP_P_Close7425);
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
    const BCHP_P_7425_Context *p7425Chip;

    BDBG_ENTER(BCHP_P_GetChipInfo);

    BCHP_P_GET_CONTEXT(hChip, p7425Chip);

    if(!p7425Chip)
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if(pusChipId)
    {
        *pusChipId = p7425Chip->pChipInfo->usChipId;
    }

    if(pusChipMajorRev)
    {
        *pusChipMajorRev = p7425Chip->pChipInfo->usMajor;
    }

    if(pusChipMinorRev)
    {
        *pusChipMinorRev = p7425Chip->pChipInfo->usMinor;
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
    BCHP_P_7425_Context *p7425Chip;
    uint32_t             ulBondStatus;
    uint32_t             uiReg;
    uint32_t             ddr_type, ddrDevice;
    uint32_t             memc_config;

    BDBG_ENTER(BCHP_P_GetFeature);

    /* get base context */
    BCHP_P_GET_CONTEXT(hChip, p7425Chip);

    /* read bond-out status common for many features */
    ulBondStatus = BREG_Read32(p7425Chip->hRegister,
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
        *(int *)pFeatureValue = 2;
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
       /* This should be DDR3 for 7425 */

       uiReg = BREG_Read32(p7425Chip->hRegister, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG);
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
       uiReg = BREG_Read32(p7425Chip->hRegister, BCHP_MEMC_DDR_0_CNTRLR_CONFIG);
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
       uiReg = BREG_Read32(p7425Chip->hRegister, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_0_CONFIG);
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

       /* DDR Mode: 0 (DDR3 parts), 1 (DDR2 parts) */
       /* This should be DDR3 for 7425 */

       uiReg = BREG_Read32(p7425Chip->hRegister, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_1_CONFIG);
       ddr_type = BCHP_GET_FIELD_DATA(uiReg, MEMC_DDR23_SHIM_ADDR_CNTL_1_CONFIG, DDR_MODE);

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

    case BCHP_Feature_eMemCtrl1DDRDeviceTechCount:

       /* Device Tech: 0 (256Mbits), 1 (512MBbits), 2 (1Gbit), 3 (2Gbit), 4 (4Gbit) */
       uiReg = BREG_Read32(p7425Chip->hRegister, BCHP_MEMC_DDR_1_CNTRLR_CONFIG);
       ddrDevice = BCHP_GET_FIELD_DATA(uiReg, MEMC_DDR_1_CNTRLR_CONFIG, DEVICE_TECH);

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

    case BCHP_Feature_eMemCtrl1DramWidthCount:

       /* DRAM Width: 0 (32 bit), 1 (16 bit) */
       uiReg = BREG_Read32(p7425Chip->hRegister, BCHP_MEMC_DDR23_SHIM_ADDR_CNTL_1_CONFIG);
       memc_config = BCHP_GET_FIELD_DATA(uiReg, MEMC_DDR23_SHIM_ADDR_CNTL_1_CONFIG, DRAM_WIDTH);

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
    uint32_t ulChipIdReg = BREG_Read32(hRegister, BCHP_SUN_TOP_CTRL_PRODUCT_ID);
    uint32_t ulChipId    = ulChipIdReg >> 16;

	BCHP_P_ResetRaagaCore(hChip, hRegister); /* must be done first before all other cores. */

    /* Reset some cores. This is needed to avoid L1 interrupts before BXXX_Open can be called per core. */
    /* Note, SW_INIT set/clear registers don't need read-modify-write. */
    BREG_Write32(hRegister, BCHP_SUN_TOP_CTRL_SW_INIT_0_SET,
		 BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, xpt_sw_init, 1 )
		 | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, avd0_sw_init, 1 )
		 | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, svd0_sw_init, 1 )
		 | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, vec_sw_init, 1 )
		 | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, aio_sw_init, 1 )
		 | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, bvn_sw_init, 1 )
		 | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_SET, raaga_sw_init, 1 ));

    if (ulChipId == 0x7425 || ulChipId == 0x7424 || ulChipIdReg == 0x0)
    {
        BREG_Write32(hRegister, BCHP_SUN_TOP_CTRL_SW_INIT_1_SET,
                     BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_1_SET, vice2_sw_init, 1 )
	    );
    }

    /* Now clear the reset. */
    BREG_Write32(hRegister, BCHP_SUN_TOP_CTRL_SW_INIT_0_CLEAR,
		 BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, xpt_sw_init, 1)
		 | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, avd0_sw_init, 1)
		 | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, svd0_sw_init, 1)
		 | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, vec_sw_init, 1)
		 | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, aio_sw_init, 1)
		 | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, bvn_sw_init, 1)
		 | BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_0_CLEAR, raaga_sw_init, 1));

    if (ulChipId == 0x7425 || ulChipId == 0x7424 || ulChipIdReg == 0x0)
    {
        BREG_Write32(hRegister, BCHP_SUN_TOP_CTRL_SW_INIT_1_CLEAR,
                     BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_INIT_1_CLEAR, vice2_sw_init, 1 )
	    );
    }

    return BERR_SUCCESS;
}


#include "bchp_raaga_dsp_misc.h"

/* re-use functions in bchp_pwr_impl.c in order to power-on raaga */
void BCHP_PWR_P_HW_PLL_RAAGA_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_PLL_RAAGA_PLL_CH1_Control(BCHP_Handle handle, bool activate);
void BCHP_PWR_P_HW_RAAGA_Control(BCHP_Handle handle, bool activate);

/* SW workaround to ensure we can hit the Raaga SW_INIT safely */
static void BCHP_P_ResetRaagaCore(const BCHP_Handle hChip, const BREG_Handle hReg)
{
    uint32_t val;

    /* unconditionally turn on everything that's needed to do the register write below.
       we don't know what power state we were left in. BCHP_PWR_Open() will later turn stuff off as needed */
    BCHP_PWR_P_HW_PLL_RAAGA_Control(hChip, true);
    BCHP_PWR_P_HW_PLL_RAAGA_PLL_CH1_Control(hChip, true);
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
    BCHP_P_7425_Context *p7425Chip;

    BDBG_ENTER(BCHP_P_MonitorPvt);
    BSTD_UNUSED(pSettings);

    /* get base context */
    BCHP_P_GET_CONTEXT(hChip, p7425Chip);

    if (p7425Chip->hAvsHandle)
    	BCHP_P_AvsMonitorPvt(p7425Chip->hAvsHandle);

    BDBG_LEAVE(BCHP_P_MonitorPvt);
}

/* This provides the current AVS data */
static BERR_Code BCHP_P_GetAvsData( BCHP_Handle hChip, BCHP_AvsData *pData )
{
    BCHP_P_7425_Context *p7425Chip;

    BDBG_ASSERT(pData);

    BDBG_ENTER(BCHP_GetAVdata);

    /* get base context */
    BCHP_P_GET_CONTEXT(hChip, p7425Chip);

    if (p7425Chip->hAvsHandle)
    	BCHP_P_AvsGetData(p7425Chip->hAvsHandle, pData);

    BDBG_LEAVE(BCHP_GetAVdata);
    return BERR_SUCCESS;
}

static BERR_Code BCHP_P_StandbyMode( BCHP_Handle hChip, bool activate )
{
    BCHP_P_7425_Context *p7425Chip;

    BDBG_ENTER(BCHP_P_StandbyMode);

    /* get base context */
    BCHP_P_GET_CONTEXT(hChip, p7425Chip);

	/* Do anything required for CHP Standby changes */

    if (p7425Chip->hAvsHandle)
    	BCHP_P_AvsStandbyMode(p7425Chip->hAvsHandle, activate);

    BDBG_LEAVE(BCHP_P_StandbyMode);
    return BERR_SUCCESS;
}

/* End of File */

