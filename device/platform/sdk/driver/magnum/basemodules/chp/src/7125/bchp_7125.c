/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_7125.c $
 * $brcm_Revision: Hydra_Software_Devel/19 $
 * $brcm_Date:  
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/src/7125/bchp_7125.c $
 * 
 * Hydra_Software_Devel/19   10/2/12 4:41p vanessah
 * SW7425-2501: add BCHP_Feature_eMemCtrl0DDRDeviceTechCount query
 * 
 * Hydra_Software_Devel/18   9/27/12 7:01p vanessah
 * sw7425-2501: add memory controller Dram width query
 * 
 * Hydra_Software_Devel/17   4/19/12 10:36a mward
 * SW7425-2605: Reset Magnum cores after chip id check.
 * 
 * Hydra_Software_Devel/16   4/13/11 1:45p mward
 * SW7125-905: Use atomic access to BCHP_CLKGEN_DVP_HT_CLK_PM_CTRL.
 * 
 * Hydra_Software_Devel/15   4/12/11 5:45p mward
 * SW7125-905:  Support for BCHP_PWR.
 * 
 * Hydra_Software_Devel/14   2/15/11 2:36p mward
 * SW7125-790:  Accept Rev. E0.
 * 
 * Hydra_Software_Devel/13   12/13/10 4:55p mward
 * SW7125-691:  SW7125-669:  Cycle power for RAP and AVD in chip open to
 * work around potential SRAM issue.
 * 
 * Hydra_Software_Devel/12   10/18/10 12:55p mward
 * SW7125-651:  Toggle HDMI 250 MHz clock during BCHP_Open7125() to work
 * around possible SRAM issue.
 * 
 * Hydra_Software_Devel/11   7/22/10 2:36p mward
 * SW7125-513: 7124 PROD_REVISION, will read 7116.  Accept 7124 also in
 * case that changes in the future.
 * 
 * Hydra_Software_Devel/10   7/16/10 12:14p mward
 * SW7125-513: Non-DOCSIS bond-out is now called 7124, not 7116.
 * 
 * Hydra_Software_Devel/9   4/27/10 4:41p mward
 * SW7125-374:  Also reset VEC and BVN blocks to prevent ARC errors on
 * startup.
 * 
 * Hydra_Software_Devel/8   1/29/10 5:04p mward
 * SW7125-176: Accept 7125C0.
 * 
 * Hydra_Software_Devel/7   12/18/09 12:16p mward
 * SW7125-149:  Allow B0 code to run on B1 chip.
 * 
 * Hydra_Software_Devel/6   11/23/09 6:34p mward
 * SW7125-97: Add 7125 B0, allow B0 code to run on A0.
 * 
 * Hydra_Software_Devel/5   10/23/09 1:33p mward
 * SW7125-42: Correct stripe width and data width settings with reference
 * to bchp_7420.c.
 * 
 * Hydra_Software_Devel/4   10/22/09 3:08p mward
 * SW7125-42: Read DDR_TECH bit from BCHP_MEMC_DDR_0_DRAM_INIT_CNTRL, set
 * ddr3_mode in
 * BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH.
 * 
 * Hydra_Software_Devel/3   8/21/09 4:54p mward
 * PR55545: set pfri_data_width.
 * 
 * Hydra_Software_Devel/2   7/23/09 11:05a mward
 * PR 55545: Corrected register names.
 * 
 * Hydra_Software_Devel/1   7/13/09 6:54p mward
 * PR55545:  Add bchp_7125.c,.h.
 * 
 * Hydra_Software_Devel/1   5/29/09 6:23p jrubio
 * PR55232: add 7125 BCHP
 * 
*
 * 
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "breg_mem.h"
#include "bchp.h"
#include "bchp_priv.h"
#include "bchp_7125.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_decode_sd_0.h"
#include "bchp_decode_ip_shim_0.h"
#include "bchp_sun_gisb_arb.h"
#include "bchp_pwr.h"
#include "bchp_memc_ddr23_aphy_ac_0.h"
#include "bchp_memc_ddr_0.h"
#include "bchp_clkgen.h"

BDBG_MODULE(BCHP);

/* Miscellaneous macros. */
#define BCHP_P_MAJOR_REV_SHIFT          (4)

/* Chip info and features */
typedef struct BCHP_P_7125_Info
{
    uint32_t      ulChipIdReg; /* index into the table. */

    /* Chip Id */
    uint16_t      usChipId;

    /* Major revision */
    uint16_t      usMajor;

    /* Minor revision */
    uint16_t      usMinor;

    /* TODO: Other features or infos if needed */
} BCHP_P_7125_Info;


/* Lookup table for chip features and etc.
 * The are many times when the chip device id register
 * not conforming to the standard numbering convention. We do 
 * it this way to work-around those problems. 
 * 
 * TODO: Update this table to support new revisions.
 */
static const BCHP_P_7125_Info s_aChipInfoTable[] =
{
#if BCHP_VER == BCHP_VER_A0
    /* A0 code will run on A0 */
    {0x71250000, BCHP_BCM7125, BCHP_MAJOR_A, BCHP_MINOR_0},
	/* For now, 7125 bond-out variants will also be accepted */
    {0x71190000, BCHP_BCM7119, BCHP_MAJOR_A, BCHP_MINOR_0},
    {0x70190000, BCHP_BCM7019, BCHP_MAJOR_A, BCHP_MINOR_0},
	{0x70190000, BCHP_BCM7025, BCHP_MAJOR_A, BCHP_MINOR_0},
	{0x71160000, BCHP_BCM7116, BCHP_MAJOR_A, BCHP_MINOR_0},
    {0x71240000, BCHP_BCM7124, BCHP_MAJOR_A, BCHP_MINOR_0},
#elif BCHP_VER == BCHP_VER_B0
    /* B0 code will run on A0, B1 */
    {0x71250000, BCHP_BCM7125, BCHP_MAJOR_A, BCHP_MINOR_0},
	/* For now, 7125 bond-out variants will also be accepted */
    {0x71190000, BCHP_BCM7119, BCHP_MAJOR_A, BCHP_MINOR_0},
    {0x70190000, BCHP_BCM7019, BCHP_MAJOR_A, BCHP_MINOR_0},
	{0x70190000, BCHP_BCM7025, BCHP_MAJOR_A, BCHP_MINOR_0},
	{0x71160000, BCHP_BCM7116, BCHP_MAJOR_A, BCHP_MINOR_0},
    {0x71240000, BCHP_BCM7124, BCHP_MAJOR_A, BCHP_MINOR_0},
    {0x71250010, BCHP_BCM7125, BCHP_MAJOR_B, BCHP_MINOR_0},
	{0x71190010, BCHP_BCM7119, BCHP_MAJOR_B, BCHP_MINOR_0},
	{0x70190010, BCHP_BCM7019, BCHP_MAJOR_B, BCHP_MINOR_0},
	{0x70190010, BCHP_BCM7025, BCHP_MAJOR_B, BCHP_MINOR_0},
	{0x71160010, BCHP_BCM7116, BCHP_MAJOR_B, BCHP_MINOR_0},
	{0x71240010, BCHP_BCM7124, BCHP_MAJOR_B, BCHP_MINOR_0},
	{0x71250011, BCHP_BCM7125, BCHP_MAJOR_B, BCHP_MINOR_1},
	{0x71190011, BCHP_BCM7119, BCHP_MAJOR_B, BCHP_MINOR_1},
	{0x70190011, BCHP_BCM7019, BCHP_MAJOR_B, BCHP_MINOR_1},
	{0x70190011, BCHP_BCM7025, BCHP_MAJOR_B, BCHP_MINOR_1},
	{0x71160011, BCHP_BCM7116, BCHP_MAJOR_B, BCHP_MINOR_1},
	{0x71240011, BCHP_BCM7124, BCHP_MAJOR_B, BCHP_MINOR_1},
#elif BCHP_VER == BCHP_VER_C0
    /* C0 code will run on C0, E0 */
    {0x71250020, BCHP_BCM7125, BCHP_MAJOR_C, BCHP_MINOR_0},
	/* For now, 7125 bond-out variants will also be accepted */
    {0x71190020, BCHP_BCM7119, BCHP_MAJOR_C, BCHP_MINOR_0},
    {0x70190020, BCHP_BCM7019, BCHP_MAJOR_C, BCHP_MINOR_0},
	{0x70190020, BCHP_BCM7025, BCHP_MAJOR_C, BCHP_MINOR_0},
	{0x71160020, BCHP_BCM7116, BCHP_MAJOR_C, BCHP_MINOR_0},
    {0x71240020, BCHP_BCM7124, BCHP_MAJOR_C, BCHP_MINOR_0},

    {0x71250040, BCHP_BCM7125, BCHP_MAJOR_E, BCHP_MINOR_0},
    {0x71190040, BCHP_BCM7119, BCHP_MAJOR_E, BCHP_MINOR_0},
    {0x70190040, BCHP_BCM7019, BCHP_MAJOR_E, BCHP_MINOR_0},
    {0x70190040, BCHP_BCM7025, BCHP_MAJOR_E, BCHP_MINOR_0},
    {0x71160040, BCHP_BCM7116, BCHP_MAJOR_E, BCHP_MINOR_0},
    {0x71240040, BCHP_BCM7124, BCHP_MAJOR_E, BCHP_MINOR_0},
#else
    #error "Port required (build with BCHP_VER=B0 for B1)"
#endif
};


/* Chip context */
typedef struct BCHP_P_7125_Context
{
    uint32_t                           ulBlackMagic;
    BREG_Handle                        hRegister;
    const BCHP_P_7125_Info            *pChipInfo;
} BCHP_P_7125_Context;

/* Max entry of lookup table */
#define BCHP_P_CHIP_INFO_MAX_ENTRY \
    (sizeof(s_aChipInfoTable) / sizeof(BCHP_P_7125_Info))

/* This macro checks for a validity of a handle, and
 * cast to context pointer. */
#define BCHP_P_GET_CONTEXT(handle, context) \
{ \
    if(!(handle) || \
       !((handle)->chipHandle) || \
       (((BCHP_P_7125_Context*)((handle)->chipHandle))->ulBlackMagic != \
       sizeof(BCHP_P_7125_Context))) \
    { \
        BDBG_ERR(("Corrupted context handle\n")); \
        (context) = NULL; \
    } \
    else \
    { \
        (context) = (BCHP_P_7125_Context*)((handle)->chipHandle); \
    } \
    BDBG_ASSERT(context); \
}

/* Static function prototypes */
static BERR_Code BCHP_P_Close7125
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

/***************************************************************************
 * Open BCM7125 Chip.
 *
 */
BERR_Code BCHP_Open7125
    ( BCHP_Handle                     *phChip,
      BREG_Handle                      hRegister )
{
    BCHP_P_Context *pChip;
    BCHP_P_7125_Context *p7125Chip;
    uint32_t ulChipIdReg;
    uint32_t ulIdx;
    uint32_t ulReg;
    uint32_t bankHeight=0, stripeWidth=0, dataWidth=0;
    uint32_t ddr0Device=0;
	bool ddr3=false;
    BERR_Code rc;

    BDBG_ENTER(BCHP_Open7125);

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

    p7125Chip = (BCHP_P_7125_Context*)
        (BKNI_Malloc(sizeof(BCHP_P_7125_Context)));
    if(!p7125Chip)
    {
        BKNI_Free(pChip);
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }

    /* Clear out the context and set defaults. */
    BKNI_Memset((void*)p7125Chip, 0x0, sizeof(BCHP_P_7125_Context));

    /* Fill up the base chip context. */
    pChip->chipHandle       = (void*)p7125Chip;
    pChip->regHandle        = hRegister;
    pChip->pCloseFunc       = BCHP_P_Close7125;
    pChip->pGetChipInfoFunc = BCHP_P_GetChipInfoComformWithBaseClass;
    pChip->pGetFeatureFunc  = BCHP_P_GetFeature;

    /* Fill up the chip context. */
    p7125Chip->ulBlackMagic = sizeof(BCHP_P_7125_Context);
    p7125Chip->hRegister    = hRegister;

    /* Register id is use for indexing into the table. */
    ulChipIdReg = BREG_Read32(hRegister, BCHP_SUN_TOP_CTRL_PROD_REVISION);

/* decompose 32 bit chip id for use with printf format string %x%c%d
Example: 0x74010020 becomes "7401C0" */
#define PRINT_CHIP(CHIPID) \
    ((CHIPID)>>16), ((((CHIPID)&0xF0)>>4)+'A'), ((CHIPID)&0x0F)
            
    for(ulIdx = 0; ulIdx < BCHP_P_CHIP_INFO_MAX_ENTRY; ulIdx++) 
    {
        BDBG_MSG(("supported revision: %x%c%d", PRINT_CHIP(s_aChipInfoTable[ulIdx].ulChipIdReg)));
    }

    /* Lookup corresponding chip id. */
    for(ulIdx = 0; ulIdx < BCHP_P_CHIP_INFO_MAX_ENTRY; ulIdx++)
    {
        const BCHP_P_7125_Info *compareChipInfo = &s_aChipInfoTable[ulIdx];

        if(compareChipInfo->ulChipIdReg == ulChipIdReg)
        {
            /* Chip Information. */
            p7125Chip->pChipInfo = compareChipInfo;
            break;
        }
        else if (ulIdx == BCHP_P_CHIP_INFO_MAX_ENTRY - 1 && compareChipInfo->usMajor == (ulChipIdReg&0xF0)>>4)
        {
            /* This is a future minor revision. We will allow it with a WRN. */
            BDBG_WRN(("An unknown minor revision %x%c%d has been detected. Certain operations may result in erratic behavior. Please confirm this chip revision is supported with this software.",
                PRINT_CHIP(ulChipIdReg)));
            p7125Chip->pChipInfo = compareChipInfo;
            break;
        }
    }

    if(!p7125Chip->pChipInfo)
    {
        BKNI_Free(p7125Chip);
        BKNI_Free(pChip);
        BDBG_ERR(("unsupported revision: %x%c%d", PRINT_CHIP(ulChipIdReg)));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BDBG_MSG(("found %x%c%d", PRINT_CHIP(p7125Chip->pChipInfo->ulChipIdReg)));

    BCHP_P_ResetMagnumCores( pChip );

	/* Open BCHP_PWR */
	rc = BCHP_PWR_Open(&pChip->pwrManager, pChip); 
	if (rc) {
		BKNI_Free(pChip);
		BKNI_Free(p7125Chip);
		return BERR_TRACE(rc);
	}

	/* All done. now return the new fresh context to user. */
	*phChip = (BCHP_Handle)pChip;

    /* read the strap registers and program system stripe width and pfri bank height */
    ulReg = BREG_Read32(hRegister, BCHP_MEMC_DDR_0_CNTRLR_CONFIG);
    ddr0Device = BCHP_GET_FIELD_DATA(ulReg, MEMC_DDR_0_CNTRLR_CONFIG, DEVICE_TECH);

    BDBG_MSG(("Strap ddr0 Device Tech %x, ",ddr0Device));

	/* read the strap registers and program system stripe width and pfri bank height */
	ulReg = BREG_Read32(hRegister, BCHP_MEMC_DDR_0_DRAM_INIT_CNTRL);
	ddr3 = BCHP_GET_FIELD_DATA(ulReg, MEMC_DDR_0_DRAM_INIT_CNTRL, DDR_TECH);
	BDBG_MSG(("ddr%1d detected",ddr3 ? 3:2));


    /* Table to Determine Stripe Width and Bank Height based on Device Tech */
	/* Only 32-bit UMA is supported. */
	dataWidth =1; /* 128-b */
	{
		switch(ddr0Device)
		{
			case 0: /* 256 Mbits Device Tech*/
				stripeWidth =0; /* 64 bytes */
				bankHeight = 1; /* 2 Mblks */
				break;
			case 1: /* 512 Mbits Device Tech*/
				stripeWidth =1; /* 128 bytes */
				bankHeight = 1; /* 2 Mblks */
				break;
			case 2: /* 1024 Mbits Device Tech*/
				stripeWidth =1; /* 128 bytes */
				bankHeight = 1; /* 2 Mblks */
				break;
			case 3: /* 2048 Mbits Device Tech*/
				stripeWidth =1; /* 128 bytes */
				bankHeight = 1; /* 2 Mblks */
				break;
			case 4: /* 4096 Mbits Device Tech*/
				stripeWidth =1; /* 128 bytes */
				bankHeight = 1; /* 2 Mblks */
				break;
		default:
				BDBG_ERR(("Unknown Value in DDR Device Config Register"));
				break;
		}

	}
#if BCHP_PWR_RESOURCE_AVD0
    BCHP_PWR_AcquireResource(pChip, BCHP_PWR_RESOURCE_AVD0);
#endif
    BREG_AtomicUpdate32(hRegister, 
            BCHP_DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH, 
            BCHP_MASK(DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH, pfri_data_width) |
			BCHP_MASK(DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH, ddr3_mode),
            BCHP_FIELD_DATA( DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH, pfri_data_width, dataWidth ) |
			BCHP_FIELD_DATA( DECODE_SD_0_REG_SD_PFRI_DATA_WIDTH, ddr3_mode, ddr3 )
    );
    BREG_AtomicUpdate32(hRegister, 
        BCHP_DECODE_SD_0_REG_SD_STRIPE_WIDTH,
        BCHP_MASK( DECODE_SD_0_REG_SD_STRIPE_WIDTH, pfri_bank_height) |
		BCHP_MASK( DECODE_SD_0_REG_SD_STRIPE_WIDTH, Stripe_Width),
        BCHP_FIELD_DATA( DECODE_SD_0_REG_SD_STRIPE_WIDTH, pfri_bank_height, bankHeight ) |
		BCHP_FIELD_DATA( DECODE_SD_0_REG_SD_STRIPE_WIDTH, Stripe_Width, stripeWidth )
    );

#if BCHP_PWR_RESOURCE_AVD0    
    BCHP_PWR_ReleaseResource(pChip, BCHP_PWR_RESOURCE_AVD0);
#endif


    BDBG_MSG(("data width=%#x, stripe width=%#x,pfri bank height=%#x",dataWidth,stripeWidth,bankHeight));

    /* Make sure the HDMI CLOCK_250_MAX is disabled.  See SW7125-651 for more information. */
    #if (BCHP_VER < BCHP_VER_C0)
    BREG_AtomicUpdate32(hRegister, BCHP_CLKGEN_PWRDN_CTRL_0,BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_250_DVP_HT_MAX_MASK,
                        BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_250_DVP_HT_MAX_MASK);

    /* Power down AVD clocks */
    ulReg = BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_250_AVD_MASK;
    BREG_AtomicUpdate32( hRegister, BCHP_CLKGEN_PWRDN_CTRL_0,ulReg,ulReg);
    ulReg = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_216_AVD0_MASK;
    BREG_AtomicUpdate32( hRegister, BCHP_CLKGEN_PWRDN_CTRL_1,ulReg,ulReg);
    ulReg = BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_108_AVD0_MASK;
    BREG_AtomicUpdate32( hRegister, BCHP_CLKGEN_PWRDN_CTRL_2,ulReg,ulReg);

    /* Power down AIO clocks */
    ulReg = BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_250_RPTD_MASK;
    BREG_AtomicUpdate32( hRegister, BCHP_CLKGEN_PWRDN_CTRL_0,ulReg,ulReg);
    ulReg = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_216_RPTD_AIO_MASK;
    BREG_AtomicUpdate32( hRegister, BCHP_CLKGEN_PWRDN_CTRL_1,ulReg,ulReg);
    ulReg = BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_108_RPTD_AIO_MASK;
    BREG_AtomicUpdate32( hRegister, BCHP_CLKGEN_PWRDN_CTRL_2,ulReg,ulReg);


    /* Now re-enable them */
    /* Power up AIO clocks */
    ulReg = BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_250_RPTD_MASK;
    BREG_AtomicUpdate32( hRegister, BCHP_CLKGEN_PWRDN_CTRL_0,ulReg,0);
    ulReg = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_216_RPTD_AIO_MASK;
    BREG_AtomicUpdate32( hRegister, BCHP_CLKGEN_PWRDN_CTRL_1,ulReg,0);
    ulReg = BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_108_RPTD_AIO_MASK;
    BREG_AtomicUpdate32( hRegister, BCHP_CLKGEN_PWRDN_CTRL_2,ulReg,0);

    /* Power up AVD clocks */
    ulReg =  BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_250_AVD_MASK;;
    BREG_AtomicUpdate32( hRegister, BCHP_CLKGEN_PWRDN_CTRL_0,ulReg,0);
    ulReg = BCHP_CLKGEN_PWRDN_CTRL_1_PWRDN_CLOCK_216_AVD0_MASK;
    BREG_AtomicUpdate32( hRegister, BCHP_CLKGEN_PWRDN_CTRL_1,ulReg,0);
    ulReg = BCHP_CLKGEN_PWRDN_CTRL_2_PWRDN_CLOCK_108_AVD0_MASK;
    BREG_AtomicUpdate32( hRegister, BCHP_CLKGEN_PWRDN_CTRL_2,ulReg,0);

    /* Power up HDMI clock */
    BREG_AtomicUpdate32(hRegister, BCHP_CLKGEN_PWRDN_CTRL_0,BCHP_CLKGEN_PWRDN_CTRL_0_PWRDN_CLOCK_250_DVP_HT_MAX_MASK,0);
    #else
    /* Make sure the HDMI CLOCK_250_MAX is disabled */
	BREG_AtomicUpdate32(hRegister,    BCHP_CLKGEN_DVP_HT_CLK_PM_CTRL ,
										/* clear these bits */
										BCHP_CLKGEN_DVP_HT_CLK_PM_CTRL_DIS_CLK_250_MAX_MASK,
										/* set these bits */
										BCHP_CLKGEN_DVP_HT_CLK_PM_CTRL_DIS_CLK_250_MAX_MASK);

    /* Power down AVD clocks */
    BREG_AtomicUpdate32(hRegister,    BCHP_CLKGEN_AVD_CLK_PM_CTRL ,
                                        /* clear these bits */
                                        BCHP_CLKGEN_AVD_CLK_PM_CTRL_reserved0_MASK      ,
                                        /* set these bits */
                                        BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_250_MASK    |   /* Power down AVD 250 clock */
                                        BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_216_MASK    |   /* Power down AVD 216 clock */
                                        BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_108_MASK    );  /* Power down AVD 108 clock */

    /* Power down audio core clocks */
    BREG_AtomicUpdate32(hRegister, BCHP_CLKGEN_RPTD_AIO_CLK_PM_CTRL,
                        /* clear these bits */
                        BCHP_CLKGEN_RPTD_AIO_CLK_PM_CTRL_reserved0_MASK ,
                        /* set these bits */
                        BCHP_CLKGEN_RPTD_AIO_CLK_PM_CTRL_DIS_CLK_250_RPTD_MASK  |  /* Power down RPTD AIO 250MHz clocks  */
                        BCHP_CLKGEN_RPTD_AIO_CLK_PM_CTRL_DIS_CLK_216_MASK       |  /* Power down RPTD AIO 216 MHz clocks */
                        BCHP_CLKGEN_RPTD_AIO_CLK_PM_CTRL_DIS_CLK_108_MASK   );     /* Power down RPTD AIO 108 clocks     */
        
    /* Now re-enable the clocks */

    /* Power up AIO clocks */
    BREG_AtomicUpdate32(hRegister, BCHP_CLKGEN_RPTD_AIO_CLK_PM_CTRL,
                        /* clear these bits */
                        BCHP_CLKGEN_RPTD_AIO_CLK_PM_CTRL_reserved0_MASK         |
                        BCHP_CLKGEN_RPTD_AIO_CLK_PM_CTRL_DIS_CLK_250_RPTD_MASK  |  /* Power up RPTD AIO 250MHz clocks  */
                        BCHP_CLKGEN_RPTD_AIO_CLK_PM_CTRL_DIS_CLK_216_MASK       |  /* Power up RPTD AIO 216 MHz clocks */
                        BCHP_CLKGEN_RPTD_AIO_CLK_PM_CTRL_DIS_CLK_108_MASK ,        /* Power up RPTD AIO 108 clocks     */
                        /* set these bits */
                        0 );

    BREG_AtomicUpdate32(hRegister,    BCHP_CLKGEN_AVD_CLK_PM_CTRL ,
                        /* clear these bits */
                        BCHP_CLKGEN_AVD_CLK_PM_CTRL_reserved0_MASK      |
                        BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_250_MASK    |   /* Power up AVD 250 clock */
                        BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_216_MASK    |   /* Power up AVD 216 clock */
                        BCHP_CLKGEN_AVD_CLK_PM_CTRL_DIS_CLK_108_MASK    ,   /* Power up AVD 108 clock */
                        /* clear these bits */
                        0 );

    /* Enable the HDMI CLOCK_250_MAX */
	BREG_AtomicUpdate32(hRegister,    BCHP_CLKGEN_DVP_HT_CLK_PM_CTRL ,
										/* clear these bits */
										BCHP_CLKGEN_DVP_HT_CLK_PM_CTRL_DIS_CLK_250_MAX_MASK,
										/* set these bits */
										0);
    #endif

    BDBG_LEAVE(BCHP_Open7125);
    return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
static BERR_Code BCHP_P_Close7125
    ( BCHP_Handle                      hChip )
{
    BCHP_P_7125_Context *p7125Chip;

    BDBG_ENTER(BCHP_P_Close7125);
    
    BCHP_P_GET_CONTEXT(hChip, p7125Chip);

    if(!p7125Chip)
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    BCHP_PWR_Close(hChip->pwrManager);
    
    /* Invalidate the magic number. */
    p7125Chip->ulBlackMagic = 0;

    BKNI_Free((void*)p7125Chip);
    BKNI_Free((void*)hChip);

    BDBG_LEAVE(BCHP_P_Close7125);
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
    const BCHP_P_7125_Context *p7125Chip;

    BDBG_ENTER(BCHP_P_GetChipInfo);

    BCHP_P_GET_CONTEXT(hChip, p7125Chip);

    if(!p7125Chip)
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if(pusChipId)
    {
        *pusChipId = p7125Chip->pChipInfo->usChipId;
    }

    if(pusChipMajorRev)
    {
        *pusChipMajorRev = p7125Chip->pChipInfo->usMajor;
    }

    if(pusChipMinorRev)
    {
        *pusChipMinorRev = p7125Chip->pChipInfo->usMinor;
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
    BCHP_P_7125_Context *p7125Chip;
    uint32_t             ulBondStatus;

    BDBG_ENTER(BCHP_P_GetFeature);
    
    /* get base context */
    BCHP_P_GET_CONTEXT(hChip, p7125Chip);

    /* read bond-out status common for many features */
    ulBondStatus = BREG_Read32(p7125Chip->hRegister,
        BCHP_SUN_TOP_CTRL_OTP_OPTION_STATUS_0);
        
    /* which feature? */
    switch (eFeature)
    {
    case BCHP_Feature_e3DGraphicsCapable:
        /* 3D capable? (bool) */
		*(bool *)pFeatureValue = BCHP_GET_FIELD_DATA(ulBondStatus,SUN_TOP_CTRL_OTP_OPTION_STATUS_0, 
													 otp_option_3d_disable ) ? false : true;
		rc = BERR_SUCCESS;
        rc = BERR_SUCCESS;
        break;

    case BCHP_Feature_eDvoPortCapable:
        /* dvo port capable? (bool) */
        *(bool *)pFeatureValue = false;
        rc = BERR_SUCCESS;
        break;

    case BCHP_Feature_eMacrovisionCapable:
        /* macrovision capable? (bool) */
		*(bool *)pFeatureValue = BCHP_GET_FIELD_DATA(ulBondStatus,SUN_TOP_CTRL_OTP_OPTION_STATUS_0,
													 otp_option_macrovision_enable ) ? true : false;
        rc = BERR_SUCCESS;
        break;
        
    case BCHP_Feature_eMpegDecoderCount:
        /* number of MPEG decoders (int) */
        *(int *)pFeatureValue = 1;
        rc = BERR_SUCCESS;
        break;
        
    case BCHP_Feature_eHdcpCapable:
        /* HDCP capable? (bool) */
        *(bool *)pFeatureValue = BCHP_GET_FIELD_DATA(ulBondStatus,SUN_TOP_CTRL_OTP_OPTION_STATUS_0,
													 otp_option_hdcp_disable ) ? false : true;
        rc = BERR_SUCCESS;
        break;
        
    case BCHP_Feature_e3desCapable:
        /* 3DES capable? (bool) */
        *(bool *)pFeatureValue = true;
        rc = BERR_SUCCESS;
        break;

    case BCHP_Feature_e1080pCapable:
        /* 1080p Capable? (bool) */
        *(bool *)pFeatureValue = false;
        rc = BERR_SUCCESS;
        break;      
        
    case BCHP_Feature_eMemCtrl1Capable:
    {
		*(bool *)pFeatureValue = false;
        rc = BERR_SUCCESS;
        break ;
    }
	
    case BCHP_Feature_eMemCtrl0DDRDeviceTechCount:
	{
	 	uint32_t ulReg, ddrDevice;
	   /* Size of memory part in MBits ie: 256, 512, 1024 */
	   /* Device Tech: 0 (256Mbits), 1 (512MBbits), 2 (1Gbit), 3 (2Gbit), 4 (4Gbit) */
	   ulReg = BREG_Read32(p7125Chip->hRegister, BCHP_MEMC_DDR_0_CNTRLR_CONFIG);
	   ddrDevice = BCHP_GET_FIELD_DATA(ulReg, MEMC_DDR_0_CNTRLR_CONFIG, DEVICE_TECH);

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
	}
	
	case BCHP_Feature_eMemCtrl0DramWidthCount:
    {
       uint32_t ulReg, memc_config;
       /* DRAM Width: 0 (32 bit), 1 (16 bit) */
       ulReg = BREG_Read32(p7125Chip->hRegister, BCHP_MEMC_DDR23_APHY_AC_0_CONFIG);
       memc_config = BCHP_GET_FIELD_DATA(ulReg, MEMC_DDR23_APHY_AC_0_CONFIG,  DRAM_WIDTH);

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
    }
        
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
	/* Reset some cores. This is needed to avoid L1 interrupts before BXXX_Open can be called per core. */
	BREG_AtomicUpdate32(hRegister, BCHP_SUN_TOP_CTRL_SW_RESET, 0,
		  BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_RESET, xpt_sw_reset, 1 )
		| BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_RESET, avd0_sw_reset, 1 )
		| BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_RESET, vec_sw_reset, 1 )
		| BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_RESET, bvn_sw_reset, 1 )
		| BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_RESET, aio_sw_reset, 1 )
		| BCHP_FIELD_DATA( SUN_TOP_CTRL_SW_RESET, rptd_sw_reset, 1 ));

	/* Now clear the reset. */
	BREG_AtomicUpdate32(hRegister, BCHP_SUN_TOP_CTRL_SW_RESET,
		  BCHP_MASK( SUN_TOP_CTRL_SW_RESET, xpt_sw_reset)
		| BCHP_MASK( SUN_TOP_CTRL_SW_RESET, avd0_sw_reset)
		| BCHP_MASK( SUN_TOP_CTRL_SW_RESET, vec_sw_reset)
		| BCHP_MASK( SUN_TOP_CTRL_SW_RESET, bvn_sw_reset)
		| BCHP_MASK( SUN_TOP_CTRL_SW_RESET, aio_sw_reset)
		| BCHP_MASK( SUN_TOP_CTRL_SW_RESET, rptd_sw_reset), 0);

    return BERR_SUCCESS;
}

/* End of File */
