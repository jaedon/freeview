/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_7550ib_lna.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/7/12 10:11a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/soc/7550ib/btnr_7550ib_lna.c $
 * 
 * Hydra_Software_Devel/1   9/7/12 10:11a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/38   6/16/11 4:59p farshidf
 * SW7550-756: Changed AGC setting to fix this issue: [PaceDTA]
 * [CASE#416611]:  Updated - HD DTA: Front End performance degradation
 * for frequencies <  330
 * 
 * Hydra_Software_Devel/37   4/14/11 3:32p farshidf
 * SW7550-727: merge to main
 * 
 * Hydra_Software_Devel/SW7550-727/2   4/12/11 11:26a pinglong
 * SW7550-727: BTNR_7550_P_Probe_First_LNA and
 * BTNR_7550_P_Probe_Second_LNA should use hI2cTHD or hI2cADS dependent
 * on the I2C opening status
 * 
 * Hydra_Software_Devel/SW7550-727/1   4/12/11 10:33a pinglong
 * SW7550-727: BTNR_7550_P_Probe_First_LNA and
 * BTNR_7550_P_Probe_Second_LNA should use hI2cTHD or hI2cADS dependent
 * on the modulation type
 * 
 * Hydra_Software_Devel/36   11/18/10 9:36a farshidf
 * SW7550-607: Tak. Added state machine reset in LNA initialization
 * 
 * Hydra_Software_Devel/35   11/17/10 1:51p farshidf
 * SW7550-607:Added enough delay time to fix input on/off issue
 * 
 * Hydra_Software_Devel/33   10/29/10 11:33a farshidf
 * SW7550-607: add tak's changes
 * 
 * Hydra_Software_Devel/31   6/14/10 3:24p farshidf
 * SW7550-38: merge Tak's code
 * 
 * Hydra_Software_Devel/TNR_7550_40_11/1   6/14/10 1:29p thayashi
 * Reduced power for Cable/Terrestrial mode
 * 
 * Hydra_Software_Devel/29   5/20/10 5:02p farshidf
 * SW7550-336: add the coverity fix
 * 
 * Hydra_Software_Devel/28   5/18/10 12:02p farshidf
 * SW7550-440: merge to main branch
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/3   5/13/10 5:53p cbrooks
 * Fixed the DPM Added TIlt Compensation
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/2   5/10/10 10:41p thayashi
 * Added low power mode for ISDBT through cable scenario
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/1   4/29/10 5:16p cbrooks
 * Added miniTune
 * 
 * Hydra_Software_Devel/27   4/21/10 6:13p farshidf
 * SW7550-38: merge charlie's clean up code
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/7   4/20/10 6:58p cbrooks
 * BEGIN TUNER CODE CLEANUP
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/6   4/14/10 3:34p cbrooks
 * clean up status
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/5   3/11/10 2:39p thayashi
 * Lower LNA power for terrestrial mode
 * 
 * Hydra_Software_Devel/24   3/4/10 5:23p farshidf
 * SW7550-38: add the dynamic LNA address
 * 
 * Hydra_Software_Devel/23   3/1/10 6:42p farshidf
 * SW7550-38: remove the tuner warning
 * 
 * Hydra_Software_Devel/22   3/1/10 12:54p farshidf
 * SW7550-38: merge from branch TNR_9
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/3   2/27/10 1:22a thayashi
 * added separate register init for rf bypass on/off
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/2   2/24/10 5:05p farshidf
 * SW7550-38: RF bypass update
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/1   2/17/10 4:16p thayashi
 * Added bias settings for loop-through
 * 
 * Hydra_Software_Devel/21   2/12/10 3:28p farshidf
 * SW7550-38: merge from TNR branch 8
 * 
 * Hydra_Software_Devel/TNR_7550_40_8/1   2/3/10 4:44p thayashi
 * SW7550-38:Changed LNA ramp speed to improve tracking
 * 
 * Hydra_Software_Devel/20   1/20/10 11:30a farshidf
 * SW7550-38: merge the TNR 7
 * 
 * Hydra_Software_Devel/TNR_7550_40_7/1   1/12/10 5:08p thayashi
 * Fixed LNA gain calculation
 * 
 * Hydra_Software_Devel/19   1/8/10 4:33p farshidf
 * SW7550-38: merge the TNR branch 6
 * 
 * Hydra_Software_Devel/TNR_7550_40_6/1   1/8/10 3:44p cbrooks
 * Added the tuner gains
 * 
 * Hydra_Software_Devel/18   1/5/10 12:47p farshidf
 * SW7550-38: add the RF bypass and daisy chain
 * 
 * Hydra_Software_Devel/17   12/23/09 4:07p farshidf
 * SW7550-38: fix LNA GPIOs
 * 
 * Hydra_Software_Devel/16   12/23/09 3:59p farshidf
 * SW7550-38: fix the LNA
 * 
 * Hydra_Software_Devel/15   12/23/09 3:46p farshidf
 * SW7550-38: fix the LNA read chip ID
 * 
 * Hydra_Software_Devel/14   12/23/09 10:00a farshidf
 * sw7550-38: merge from branch 5 of tnr
 * 
 * Hydra_Software_Devel/TNR_7550_40_5/2   12/18/09 2:45p thayashi
 * Changed the way lna chip id is passed
 * 
 * Hydra_Software_Devel/TNR_7550_40_5/1   12/17/09 9:55p thayashi
 * Added 3412 controls
 * 
 * Hydra_Software_Devel/13   12/16/09 3:48p farshidf
 * SW7550-38: include the 3412 header file
 * 
 * Hydra_Software_Devel/12   12/4/09 12:39p farshidf
 * SW7550-38: merge the branch 3
 * 
 * Hydra_Software_Devel/TNR_7550_40_3/1   11/25/09 4:27p cbrooks
 * Changes for tuner development 11/25/2009
 * 
 * Hydra_Software_Devel/10   11/23/09 1:33p farshidf
 * SW7550-38: compile fix
 * 
 * Hydra_Software_Devel/9   11/23/09 1:18p farshidf
 * SW7550-38: merge from branch
 * 
 * Hydra_Software_Devel/TNR_7550_40_2/2   11/23/09 12:50p cbrooks
 * Ready To Merge With PI Ver0.0
 * 
 * Hydra_Software_Devel/TNR_7550_40_2/1   11/20/09 11:05p thayashi
 * fixed auto/fixed gain bug
 * 
 * Hydra_Software_Devel/6   11/19/09 5:10p farshidf
 * SW7550-38: check-in latest changes
 * 
 * Hydra_Software_Devel/THD_7550_40_6/1   11/18/09 9:29p jputnam
 * Functional for terrestrial
 * 
 * Hydra_Software_Devel/4   11/16/09 4:41p farshidf
 * SW7550-38: include Charles's latest changes
 * 
 * Hydra_Software_Devel/3   11/16/09 3:59p farshidf
 * SW7550-38: add the LNA code
 * 
 * Hydra_Software_Devel/2   11/13/09 1:42p farshidf
 * SW7550-38: make Nexus compatible
 * 
 * Hydra_Software_Devel/1   11/12/09 7:33p farshidf
 * SW7550-38: Initial 7550 Tuner code
 * 
 * 
 ***************************************************************************/

#include "bstd.h"
#include "breg_mem.h"
#include "bchp.h"
#include "btnr_75xxib_priv.h"
#include "btnr_7550ib_tune.h"
#include "bchp_bscb.h"
#include "bchp_bcm3412_regs.h"
#include "breg_i2c.h"



/*registers needed for this function */
BDBG_MODULE(btnr_7550ib_LNA);
#define BCHP_BCM341X_regs_LNA1_CNTL_MODE_SHIFT     BCHP_BCM341X_regs_LNA1_CNTL_STAGE1_FB_PUP_SHIFT
#define BCHP_BCM341X_regs_LNA1_CNTL_MODE_MASK (BCHP_BCM341X_regs_LNA1_CNTL_STAGE1_FB_PUP_MASK | BCHP_BCM341X_regs_LNA1_CNTL_STAGE1_FB_CURRENT_MASK | BCHP_BCM341X_regs_LNA1_CNTL_STAGE1_TAIL3_MASK | BCHP_BCM341X_regs_LNA1_CNTL_STAGE1_TAIL2_MASK |\
	BCHP_BCM341X_regs_LNA1_CNTL_STAGE1_TAIL2_MASK | BCHP_BCM341X_regs_LNA1_CNTL_STAGE1_TAIL1_MASK | BCHP_BCM341X_regs_LNA1_CNTL_STAGE1_CSC3_MASK | BCHP_BCM341X_regs_LNA1_CNTL_STAGE1_CSC2_MASK | BCHP_BCM341X_regs_LNA1_CNTL_STAGE1_CSC1_MASK)

/****************************************************************************************************/
/* START OF LNA FUNCTIONS MAINTAINED BY PI **********************************************************/
/****************************************************************************************************/

void BTNR_7550_P_LNA_WriteRegister(BTNR_75xx_Handle, uint8_t, uint32_t);
uint32_t BTNR_7550_P_LNA_ReadRegister(BTNR_75xx_Handle, uint8_t);

#define BTNR_7550_P_LNA_ReadRegisterField(h, Register, Field) \
  ((((BTNR_7550_P_LNA_ReadRegister(h, BCHP_##Register)) & BCHP_MASK(Register,Field)) >> \
  BCHP_SHIFT(Register,Field)))

#define BTNR_7550_P_LNA_WriteRegisterField(h, Register, Field, Data) \
  BTNR_7550_P_LNA_WriteRegister(h, BCHP_##Register, ((BTNR_7550_P_LNA_ReadRegister(h, BCHP_##Register) & \
  ~((uint32_t)BCHP_MASK(Register, Field))) | \
  BCHP_FIELD_DATA(Register, Field, Data)))

void BTNR_7550_P_Test_LNA(BTNR_75xx_Handle h)
{
	uint32_t  reg;

	BTNR_7550_P_LNA_WriteRegister(h,  BCHP_BCM341X_regs_LNA1_CNTL, 0x00);
	BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA1_CNTL, STAGE1_CSC3, 1);
	reg = BTNR_7550_P_LNA_ReadRegisterField(h, BCM341X_regs_LNA1_CNTL, STAGE1_CSC3);
	BDBG_MSG(("we are reading address 0x34 = %0x", reg));

	reg = BTNR_7550_P_LNA_ReadRegister(h, BCHP_BCM341X_regs_LNA1_CNTL);
	BDBG_MSG(("we are reading address 0x34 = %0x", reg));

}


/*---------------------------------------------------------------------------
* Function: BTNR_7550_P_LNA_WriteRegister
*---------------------------------------------------------------------------*/
void BTNR_7550_P_LNA_WriteRegister(BTNR_75xx_Handle h, uint8_t registerAddress, uint32_t value)
{	
	uint8_t buf[8];
			
	buf[0]  = (char)(value & 0xFF);
	buf[1]  = (char)((value >> 8) & 0xFF);
	buf[2]  = (char)((value >> 16) & 0xFF);
	buf[3]  = (char)((value >> 24) & 0xFF);

	if (h->pTnrModeData->TunerSetup == BTNR_7550_Setup_eTerrestrial)
		BREG_I2C_Write(h->hI2cTHD, (uint16_t)(h->pTnrModeData->BBS_Terrestrial_LNA_Address), (registerAddress >> 2), &buf[0], 4);
	else
		BREG_I2C_Write(h->hI2cADS, (uint16_t)(h->pTnrModeData->BBS_Cable_LNA_Address), (registerAddress >> 2), &buf[0], 4);
}
/*---------------------------------------------------------------------------
* Function: BTNR_7550_P_LNA_ReadRegister
*---------------------------------------------------------------------------*/
uint32_t BTNR_7550_P_LNA_ReadRegister(BTNR_75xx_Handle h, uint8_t registerAddress )
{

	uint32_t reg = 0;
	uint8_t	 buf[5];

	if (h->pTnrModeData->TunerSetup == BTNR_7550_Setup_eTerrestrial)
		BREG_I2C_Read(h->hI2cTHD, (uint16_t)(h->pTnrModeData->BBS_Terrestrial_LNA_Address), (registerAddress >> 2), &buf[0], 4);
	else
		BREG_I2C_Read(h->hI2cADS, (uint16_t)(h->pTnrModeData->BBS_Cable_LNA_Address), (registerAddress >> 2), &buf[0], 4);

	reg = 	 buf[0];
	reg |= 	 buf[1] << 8;
	reg |=	 buf[2] << 16;
	reg |=	 buf[3] << 24;
	return reg;
}


/****************************************************************************************************/
/* END OF LNA FUNCTIONS MAINTAINED BY PI ************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/* START OF LNA FUNCTIONS MAINTAINED BY TUNING CODE *************************************************/
/****************************************************************************************************/

/****************************************************************************************************
 * BTNR_7550_P_Init_LNA()  This routine initializes the external LNA
 ****************************************************************************************************/
void BTNR_7550_P_Init_LNA(BTNR_75xx_Handle h)
{
	BTNR_OFFON_t BCM3410_Auto;
	uint32_t BCM3410_Gain_State;
	uint32_t LNA_chip_id, LNA_Gain;
	uint8_t lval1, lval2, lval3, i;
	lval3 = 63; 
	lval1 = 0; 
	lval2 = 31; 



	/*Set the LNA to Fixed Gain or Auto mode based on the DS setting if QAM mode or THD setting if DVB-T/ISDB-T mode from the BTNR_P_75xx_AcquireSettings_t structure*/
	BCM3410_Auto = (BTNR_OFFON_t)(h->pTnrModeData->TunerSetup == BTNR_7550_Setup_eCable) ? h->pTnrModeData->BBS_DS_BCM3410_Auto : h->pTnrModeData->BBS_THD_BCM3410_Auto;
	BCM3410_Gain_State = (h->pTnrModeData->TunerSetup == BTNR_7550_Setup_eCable) ? h->pTnrModeData->BBS_DS_BCM3410_Gain_State : h->pTnrModeData->BBS_THD_BCM3410_Gain_State;

	/* WE SHOULD NOT WRITE TO LNA ABOVE THIS LINE */

	/*Reset Registers in LNA*/
	BTNR_7550_P_LNA_WriteRegister(h,  BCHP_BCM341X_regs_SERIAL_CTL, 0x00000000);
	BTNR_7550_P_LNA_WriteRegister(h,  BCHP_BCM341X_regs_SM_BYPASS, 0x01951000);
	BTNR_7550_P_LNA_WriteRegister(h,  BCHP_BCM341X_regs_MWC_CNTL, 0x3A222000);
	BTNR_7550_P_LNA_WriteRegister(h,  BCHP_BCM341X_regs_MWC_PM_CNTL, 0x101E0100);
	BTNR_7550_P_LNA_WriteRegister(h,  BCHP_BCM341X_regs_LNA_ADDR, 0x1D1F011F);
	BTNR_7550_P_LNA_WriteRegister(h,  BCHP_BCM341X_regs_RAMP_RATE_CNTL, 0x10041030);
	BTNR_7550_P_LNA_WriteRegister(h,  BCHP_BCM341X_regs_RAMP_DAC_CNTL, 0x00000B01);
	BTNR_7550_P_LNA_WriteRegister(h,  BCHP_BCM341X_regs_LNA1_CNTL, 0x92492400);
	BTNR_7550_P_LNA_WriteRegister(h,  BCHP_BCM341X_regs_BG_LNA2_RCAL_CNTL, 0x4000241B);
	if (h->pTnrModeData->RFbypassMode == false)  /* RF bypass off */
	{
		BTNR_7550_P_LNA_WriteRegister(h,  BCHP_BCM341X_regs_PD_OOB_OSC_CNTL, 0x04640F00);
	}
	else
	{
		BTNR_7550_P_LNA_WriteRegister(h,  BCHP_BCM341X_regs_PD_OOB_OSC_CNTL, 0x04240F00);
	}
	
	LNA_chip_id = BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_CHIP_ID, CHIP_PART_NUMBER);
	if (LNA_chip_id == 0x3410)
	{
		h->pTnrModeData->LNA_ChipID = BTNR_LNAID_3410;
		BDBG_MSG(("LNA Chip ID is %x",LNA_chip_id));
	}
	else if (LNA_chip_id == 0x3412)
	{
		h->pTnrModeData->LNA_ChipID = BTNR_LNAID_3412;
		BDBG_MSG(("LNA Chip ID is %x",LNA_chip_id));
	}
	else
	{
		BDBG_WRN(("LNA Chip ID is INVALID received value is %x",LNA_chip_id));;
	}
	
	/*Widen window comparator limits to give more hysteresis*/
	BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_MWC_CNTL, MWC_HI, 0x25); 
	BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_MWC_PM_CNTL, MWC_LO, 0x1B);

	/*Set AGC TOP*/		
	if (h->pTnrModeData->TunerSetup == BTNR_7550_Setup_eCable)
	{
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_PD_OOB_OSC_CNTL, PD_PSET_B, 0x3); 
	}
	else
	{
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_PD_OOB_OSC_CNTL, PD_PSET_B, 0x4); 
	}
	BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_PD_OOB_OSC_CNTL, PD_BO, 0x1); 

	/*Adjust power consumption levels depending on application*/
	if (h->pTnrModeData->TunerSetup == BTNR_7550_Setup_eTerrestrial)
	{		
		/*low noise setting for loop-through*/
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_PD_OOB_OSC_CNTL, LT_CurrCont, 0x1A);
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_PD_OOB_OSC_CNTL, LT_LowNF, 0x3);
		
		/*low power setting*/
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA1_CNTL, MODE, 0x492492);
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE2_BIAS, 0x1B);
	}
	else
	{
		/*high linearity setting for loop-through*/
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_PD_OOB_OSC_CNTL, LT_CurrCont, 0x1A);
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_PD_OOB_OSC_CNTL, LT_LowNF, 0x0);

		if (h->pTnrModeData->TunerAcquisitionMode != BTNR_7550_Standard_eQAM) /*Reduce power for ISDBT through cable mode*/
		{
		    BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA1_CNTL, MODE, 0x492492);
			BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE2_BIAS, 0x1B);
		}
	}

	/*Start resistor calibration*/
	for(i = 0; i <= 5; i++)
	{		
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_MWC_PM_CNTL, PM_VREF, lval2);
		BKNI_Sleep(10);
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_MWC_PM_CNTL, COMP_LAT_EN, 0x0);		 
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_MWC_PM_CNTL, COMP_LAT_EN, 0x1);		 
 
		if (BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_STATUS_READ2, COMP_OUT))
		{ 
			lval1 = lval2 + 1; 
			lval2 = (int)((lval3 + lval1)/2); 
		}
		else
		{
			lval3 = lval2 - 1;	 
			lval2 = (int)((lval3 + lval1)/2);	 
		}
	}
	if (lval2 > 36) 
	{
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE1_RCAL, 0x0);	 
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE2_RCAL, 0x1); 
	}
	else if (lval2 > 31)
	{ 
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE1_RCAL, 0x1);	 
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE2_RCAL, 0x3);	 
	}
	else if (lval2 > 26)
	{ 
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE1_RCAL, 0x3);	 
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE2_RCAL, 0x3); 
	}
	else if (lval2 > 21) 
	{
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE1_RCAL, 0x5);	 
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE2_RCAL, 0x3);	 
	}
	else
	{
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE1_RCAL, 0x7);	 
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE2_RCAL, 0x5);	 
	}
	BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_MWC_PM_CNTL, PM_PD, 0x1);

	/*wait for AGC Lock*/
	BTNR_7550_P_Set_LNA_AGC_BW(h, LNA_AGC_BW_eAcquisition);   /* Set LNA AGC to Acquisition Bandwidth */
	BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_MWC_CNTL, SM_RESET, 0x1);		 
	BTNR_7550_P_LNA_WriteRegisterField(h, BCM341X_regs_MWC_CNTL, SM_RESET, 0x0);
	
	BKNI_Sleep(150);

	BTNR_7550_P_Wait_For_LNA_AGCLock(h);

	if(BCM3410_Auto == BTNR_OFF)		 
	{
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA_ADDR, STG1_GAIN_I2C, BCM3410_Gain_State);		 
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_SM_BYPASS, DEC_LAT_ENB_STG1_I2C, 0x0);		  
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_SM_BYPASS, DEC_LAT_ENB_STG1_I2C, 0x1);	 	
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_MWC_CNTL, SM_RESET, 0x1);
	}
	else	 
	{
		BTNR_7550_P_LNA_WriteRegisterField(h, BCM341X_regs_MWC_CNTL, SM_RESET, 0x0);
	}

	LNA_Gain= BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_STATUS_READ2, STG1_GAIN);
	BDBG_MSG(("LNA_GAIN after initialization = %x",LNA_Gain));
}

/****************************************************************************************************
 * BTNR_7550_P_Probe_First_LNA()  
 ****************************************************************************************************/
void BTNR_7550_P_Probe_First_LNA(BTNR_75xx_Handle h)
{
	uint8_t	 buf[5];
	BERR_Code ret;
	uint16_t addr[]={0x60,0x6c,0x61, 0x6d, 0x62, 0x6e, 0x63, 0x6f};
	int addr_count = 8;
	int i;

	buf[0]  = 0;
	buf[1]  = 0;
	buf[2]  = 0;
	buf[3]  = 1;
	
	for(i=0; i<addr_count; i++)
	{
		if (h->hI2cTHD)
			ret = BREG_I2C_Read(h->hI2cTHD, addr[i], (BCHP_BCM341X_regs_CHIP_ID >> 2), &buf[0], 4);
		else if (h->hI2cADS)
			ret = BREG_I2C_Read(h->hI2cADS, addr[i], (BCHP_BCM341X_regs_CHIP_ID >> 2), &buf[0], 4);
		else
		{
			BDBG_MSG(("hI2cTHD and hI2cTHD haven't been created"));
			ret = BERR_NOT_INITIALIZED;
		}
		BDBG_MSG(("I2C 0x%x,BREG_I2C_Read=%d\n",addr[i],ret));
		if(ret != BERR_SUCCESS)
		{
			continue;
		}

		if (h->hI2cTHD)
			ret = BREG_I2C_Write(h->hI2cTHD, addr[i], (BCHP_BCM341X_regs_MWC_CNTL >> 2), &buf[0], 4);
		else if (h->hI2cADS)
			ret = BREG_I2C_Write(h->hI2cADS, addr[i], (BCHP_BCM341X_regs_MWC_CNTL >> 2), &buf[0], 4);
		else
		{
			BDBG_MSG(("hI2cTHD and hI2cTHD haven't been created"));
			ret = BERR_NOT_INITIALIZED;
		}
		BDBG_MSG(("I2C 0x%x BREG_I2C_Write=%d\n",addr[i],ret));
		if(ret != BERR_SUCCESS)
		{
			continue;
		}

		BDBG_MSG(("Set LNA I2C 0x%x \n",addr[i]));
		break;	
	}
	if (i < addr_count)
	{
		h->pTnrModeData->BBS_Cable_LNA_Address = addr[i];
		h->pTnrModeData->BBS_Terrestrial_LNA_Address = addr[i];
	}
	else
		BDBG_WRN(("None of the first LNA address work\n"));
	
}

/****************************************************************************************************
 * BTNR_7550_P_Probe_Second_LNA()  
 ****************************************************************************************************/
void BTNR_7550_P_Probe_Second_LNA(BTNR_75xx_Handle h)
{
	uint8_t	 buf[5];
	BERR_Code ret;
	uint16_t addr[]={0x62,0x6e};
	int addr_count = 2;
	int i;

	buf[0]  = 0;
	buf[1]  = 0;
	buf[2]  = 0;
	buf[3]  = 1;
	
	for(i=0; i<addr_count; i++)
	{
		if (h->hI2cTHD)
			ret = BREG_I2C_Read(h->hI2cTHD, addr[i], (BCHP_BCM341X_regs_CHIP_ID >> 2), &buf[0], 4);
		else if (h->hI2cADS)
			ret = BREG_I2C_Read(h->hI2cADS, addr[i], (BCHP_BCM341X_regs_CHIP_ID >> 2), &buf[0], 4);
		else
		{
			BDBG_MSG(("hI2cTHD and hI2cTHD haven't been created"));
			ret = BERR_NOT_INITIALIZED;
		}
		BDBG_MSG(("I2C 0x%x,BREG_I2C_Read=%d\n",addr[i],ret));
		if(ret != BERR_SUCCESS)
		{
			continue;
		}

		if (h->hI2cTHD)
			ret = BREG_I2C_Write(h->hI2cTHD, addr[i], (BCHP_BCM341X_regs_MWC_CNTL >> 2), &buf[0], 4);
		else if (h->hI2cADS)
			ret = BREG_I2C_Read(h->hI2cADS, addr[i], (BCHP_BCM341X_regs_CHIP_ID >> 2), &buf[0], 4);
		else
		{
			BDBG_MSG(("hI2cTHD and hI2cTHD haven't been created"));
			ret = BERR_NOT_INITIALIZED;
		}
		BDBG_MSG(("I2C 0x%x BREG_I2C_Write=%d\n",addr[i],ret));
		if(ret != BERR_SUCCESS)
		{
			continue;
		}
		BDBG_MSG(("Set LNA I2C 0x%x \n",addr[i]));
		break;	
	}
	if (i < addr_count)
		h->pTnrModeData->BBS_Terrestrial_LNA_Address = addr[i];
	else
		BDBG_WRN(("None of the second LNA address work\n"));
}
/****************************************************************************************************
 * BTNR_7550_P_Set_LNA_Boost()  This routine controls the boost function of the external LNA
 ****************************************************************************************************/
void BTNR_7550_P_Set_LNA_Boost(BTNR_75xx_Handle h)
{
	BTNR_OFFON_t LNA_Boost_On;
	uint32_t LNA_Gain, LNA_Boost;

	LNA_Boost = BTNR_7550_P_LNA_ReadRegisterField(h, BCM341X_regs_LNA1_CNTL, STAGE1_GAIN_BOOST);

	/*Set Boost in the LNA based on the DS setting if QAM mode or THD setting if DVB-T/ISDB-T mode from the BTNR_P_75xx_AcquireSettings_t structure*/
	LNA_Boost_On = (BTNR_OFFON_t)(h->pTnrModeData->TunerSetup == BTNR_7550_Setup_eCable) ? h->pTnrModeData->BBS_DS_LNA_Boost_On : h->pTnrModeData->BBS_THD_LNA_Boost_On;

	if (LNA_Boost_On == BTNR_ON) /*Manual boost on*/
	{
		if (h->pTnrModeData->LNA_ChipID == BTNR_LNAID_3412)
		{
			BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA_ADDR, SuperBoost_en, 0x1);
			BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA1_CNTL, STAGE1_GAIN_BOOST, 0x1);
			if (LNA_Boost == 0)
			{
				BTNR_7550_P_Wait_For_LNA_AGCLock(h);
			}
			BDBG_MSG(("Enhanced LNA_BOOST is enabled"));
		}
		else /*3410*/
		{
			BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA1_CNTL, STAGE1_GAIN_BOOST, 0x1);
			if (LNA_Boost == 0)
			{
				BTNR_7550_P_Wait_For_LNA_AGCLock(h);
			}
			BDBG_MSG(("LNA_BOOST is enabled"));
		}
	}
	else if ((DS_LNA_AUTO_BOOST_ON == 1) && (h->pTnrModeData->TunerSetup == BTNR_7550_Setup_eCable)) /*DS Auto boost on (cable mode only)*/
	{
		LNA_Gain= BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_STATUS_READ2, STG1_GAIN);
		BDBG_MSG(("LNA_GAIN before boost routine = %x",LNA_Gain));
	
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA1_CNTL, STAGE1_GAIN_BOOST, 0x1);
		if (h->pTnrModeData->LNA_ChipID == BTNR_LNAID_3412)
		{
			BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA_ADDR, SuperBoost_en, 0x1);
		}
		BTNR_7550_P_Wait_For_LNA_AGCLock(h);
		LNA_Gain= BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_STATUS_READ2, STG1_GAIN);
		BDBG_MSG(("LNA_GAIN after boost(s) is enabled = %x",LNA_Gain));

		if (LNA_Gain < (0x1F - DS_LNA_AUTO_BOOST_TOL))
		{	
			if (h->pTnrModeData->LNA_ChipID == BTNR_LNAID_3412)
			{
				BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA_ADDR, SuperBoost_en, 0x0);
				BTNR_7550_P_Wait_For_LNA_AGCLock(h);
				LNA_Gain= BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_STATUS_READ2, STG1_GAIN);
				BDBG_MSG(("LNA_GAIN after enhanced boost disabled = %x",LNA_Gain));
			}
			if (LNA_Gain < (0x1F - DS_LNA_AUTO_BOOST_TOL))
			{
				BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA1_CNTL, STAGE1_GAIN_BOOST, 0x0);
				BTNR_7550_P_Wait_For_LNA_AGCLock(h);
				LNA_Gain= BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_STATUS_READ2, STG1_GAIN);
				BDBG_MSG(("LNA_GAIN after boost disabled = %x",LNA_Gain));					
				BDBG_MSG(("Enhanced LNA_BOOST and LNA_BOOST are disabled"));
			}
			else
			{		
				BDBG_MSG(("LNA_BOOST is enabled"));
			}
		}
		else
		{
			if (h->pTnrModeData->LNA_ChipID == BTNR_LNAID_3412)
			{
				BDBG_MSG(("Enhanced LNA_BOOST is enabled"));
			}
			else
			{
				BDBG_MSG(("LNA_BOOST is enabled"));
			}
		}
	}
	else /*All Boost off*/
	{
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA1_CNTL, STAGE1_GAIN_BOOST, 0x0);
		if (h->pTnrModeData->LNA_ChipID == BTNR_LNAID_3412)
		{
			BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA_ADDR, SuperBoost_en, 0x0);
		}
		if (LNA_Boost == 1)
		{
			BTNR_7550_P_Wait_For_LNA_AGCLock(h);
		}
		BDBG_MSG(("Enhanced LNA_BOOST and LNA_BOOST are disabled"));
	}

}
/****************************************************************************************************
 * BTNR_7550_P_Set_LNA_Tilt()  This routine controls the tilt function of the external LNA
 ****************************************************************************************************/
void BTNR_7550_P_Set_LNA_Tilt(BTNR_75xx_Handle h)
{
	/*variable passed from BTNR_Handle*/	
	BTNR_OFFON_t LNA_Tilt_On;
	uint32_t LNA_Tilt_Freq, LNA_Tilt;

	LNA_Tilt = BTNR_7550_P_LNA_ReadRegisterField(h, BCM341X_regs_LNA1_CNTL, STAGE1_TILT_ON);

	/*Set the LNA Tilt and LNA Tilt Frequency based on the DS setting if QAM mode or THD setting if DVB-T/ISDB-T mode from the BTNR_P_75xx_AcquireSettings_t structure*/
	LNA_Tilt_On = (BTNR_OFFON_t)(h->pTnrModeData->TunerSetup == BTNR_7550_Setup_eCable) ? h->pTnrModeData->BBS_DS_LNA_Tilt_On : h->pTnrModeData->BBS_THD_LNA_Tilt_On;
	LNA_Tilt_Freq = (h->pTnrModeData->TunerSetup == BTNR_7550_Setup_eCable) ? h->pTnrModeData->BBS_DS_LNA_Tilt_Freq : h->pTnrModeData->BBS_THD_LNA_Tilt_Freq;
		
	if ((LNA_Tilt_On == BTNR_ON) && (h->pTnrModeData->TunerFreq > LNA_Tilt_Freq))
	{
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA1_CNTL, STAGE1_TILT_ON, 0x1);
		BDBG_MSG(("LNA_TILT is enabled, LNA Tilt Frequency is %d",LNA_Tilt_Freq));
		if (LNA_Tilt == 0)
		{
			BTNR_7550_P_Wait_For_LNA_AGCLock(h);
		}

	}
	else
	{
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_LNA1_CNTL, STAGE1_TILT_ON, 0x0); 
		BDBG_MSG(("LNA_TILT is disabled"));
		if (LNA_Tilt == 1)
		{
			BTNR_7550_P_Wait_For_LNA_AGCLock(h);
		}
	}
}

/****************************************************************************************************
 * BTNR_7550_P_Set_LNA_Output()  This routine enables the correct output ports of the external LNA
 ****************************************************************************************************/
void BTNR_7550_P_Set_LNA_Output(BTNR_75xx_Handle h)
{

	if (h->pTnrModeData->LNA_ChipID == BTNR_LNAID_3412) /*Don't do anything if 3410 is used because there is only one output*/
	{
		if (h->pTnrModeData->TunerSetup == BTNR_7550_Setup_eCable) /*Only need 1 output for QAM. Disable 2nd output*/
		{
				BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE2_PD1, 0x0);
				BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE2_PD2, 0x1);
				BDBG_MSG(("LNA Output 1 is enabled"));
		}
		else /*Terrestrial case.  Enable required output and disable the other one*/
		{
			if (h->pTnrModeData->TunerFreq < VHF_UHF_BREAKPOINT ) /*signal path set to VHF band*/
			{
				BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE2_PD1, 0x0);
				BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE2_PD2, 0x1);
				BDBG_MSG(("LNA Output 1 is enabled"));
			}
			else /*signal path set to UHF band*/
			{
				BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE2_PD1, 0x1);
				BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_BG_LNA2_RCAL_CNTL, STAGE2_PD2, 0x0);
				BDBG_MSG(("LNA Output 2 is enabled"));
			}
		}
	}
}

/****************************************************************************************************
 * BTNR_7550_P_Wait_For_LNA_AGCLock()  This routine waits for AGC lock
 ****************************************************************************************************/
void BTNR_7550_P_Wait_For_LNA_AGCLock(BTNR_75xx_Handle h)
{
	uint32_t LNA_Gain, AGC_Lock;
	uint8_t i;
	
	i=0;
	while (i < 50)
	{
		BKNI_Sleep(5);
		LNA_Gain= BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_STATUS_READ2, STG1_GAIN);
		AGC_Lock = BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_STATUS_READ2, AGC_LOCK);
		BDBG_MSG(("LNA_GAIN = %x AGC_LOCK = %x",LNA_Gain, AGC_Lock));
		if (AGC_Lock == 1 || LNA_Gain == 0x1F || LNA_Gain == 0x1)
		{
			i = 50;
		}
		i = i + 1;
	}
}

/****************************************************************************************************
 * BTNR_7550_P_Set_Rf_ByPass_mode()  This routine enables/disables the RF bypass
 ****************************************************************************************************/
void BTNR_7550_P_Set_Rf_ByPass_mode(BTNR_75xx_Handle h, bool mode)
{
	if (mode) /* if bypass is enabled) */
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_PD_OOB_OSC_CNTL, LT_PD, 0x0);
	else
		BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_PD_OOB_OSC_CNTL, LT_PD, 0x1);
}

/****************************************************************************************************
 * BTNR_7550_P_Get_LNA_Gain()  This routine reads the gain of the external LNA
 ****************************************************************************************************/
uint8_t BTNR_7550_P_Get_LNA_Gain(BTNR_75xx_Handle h)
{
	return (BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_STATUS_READ2, STG1_GAIN));
}

/****************************************************************************************************
 * BTNR_7550_P_Get_LNA_AGCLock_Status()  This routine reads the lock status bit of the external LNA
 ****************************************************************************************************/
uint8_t BTNR_7550_P_Get_LNA_AGCLock_Status(BTNR_75xx_Handle h)
{
	return (BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_STATUS_READ2, AGC_LOCK));
}

/****************************************************************************************************
 * BTNR_7550_P_Set_LNA_AGC_BW()  This routine sets LNA AGC BW
 ****************************************************************************************************/
void BTNR_7550_P_Set_LNA_AGC_BW(BTNR_75xx_Handle h, LNA_AGC_BW_t LNA_AGC_BW)
{
	uint32_t FastUpRate, SlowUpRate, SlowDnRate, FastDnRate;

	/*check LNA_AGC_BW is in range*/

	if ((LNA_AGC_BW != LNA_AGC_BW_eAcquisition) && (LNA_AGC_BW != LNA_AGC_BW_eTracking))
	{
		BDBG_ERR(("ERROR LNA_AGC_BW is not in range in BTNR_7550_P_Set_LNA_AGC_BW() value received is %d",LNA_AGC_BW));
	}

	if (LNA_AGC_BW == LNA_AGC_BW_eAcquisition) /*acquisition*/
	{
		FastUpRate = 0x7F;
		SlowUpRate = 0x7F;
		SlowDnRate = 0x7F;
		FastDnRate = 0x7F;
		BDBG_MSG(("LNA AGC acquisition ramp rates set: 0x%x 0x%x 0x%x 0x%x",FastUpRate, SlowUpRate, SlowDnRate, FastDnRate));
	}
	else /*tracking*/
	{
		if (h->pTnrModeData->TunerSetup == BTNR_7550_Setup_eTerrestrial) /*faster tracking for terrestrial*/
		{
			FastUpRate = 0x10;
			SlowUpRate = 0x8;
			SlowDnRate = 0x20;
			FastDnRate = 0x70;
			BDBG_MSG(("LNA AGC tracking ramp rates set: 0x%x 0x%x 0x%x 0x%x",FastUpRate, SlowUpRate, SlowDnRate, FastDnRate));
		}
		else /*slower tracking for cable*/
		{
			FastUpRate = 0x10;
			SlowUpRate = 0x4;
			SlowDnRate = 0x10;
			FastDnRate = 0x30;
			BDBG_MSG(("LNA AGC tracking rates set: 0x%x 0x%x 0x%x 0x%x",FastUpRate, SlowUpRate, SlowDnRate, FastDnRate));
		}
	}

	BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_RAMP_RATE_CNTL, FAST_UP_RATE_RATE, FastUpRate); 
	BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_RAMP_RATE_CNTL, SLOW_UP_RAMP_RATE, SlowUpRate);
	BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_RAMP_RATE_CNTL, SLOW_DWN_RAMP_RATE, SlowDnRate);
	BTNR_7550_P_LNA_WriteRegisterField(h,  BCM341X_regs_RAMP_RATE_CNTL, FAST_DWN_RAMP_RATE, FastDnRate);
}

/****************************************************************************************************
 * BTNR_7550_P_Get_LNA_Status()  This routine get teh LNA Status fror the Tuner Status Page
 ****************************************************************************************************/
void BTNR_7550_P_Get_LNA_Status(BTNR_75xx_Handle h)
{

	uint32_t ReadReg;
	int32_t GainReg;

	/*The LNA Gain is reported in 1/256 db*/	
	/*Get the LNA GAIN if Boost and SuperBoost are disabled*/
	ReadReg = BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_STATUS_READ2, STG1_GAIN);
	GainReg = (-14 + ReadReg)<<8;
	if (ReadReg == 0)
	{
		BDBG_WRN(("WARNING!!! Invalid STG1_GAIN in LNA detected, STG1_GAIN= %d",ReadReg));
	}

	/*Compenasate if Boost is enabled*/
	/*Check for LNA Boost*/
	ReadReg = BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_LNA1_CNTL, STAGE1_GAIN_BOOST);
	h->pTnrStatus->LNA_Boost_Status = 0;
	if (ReadReg == 1)
	{
		GainReg = GainReg + 768;
		h->pTnrStatus->LNA_Boost_Status = 1;
		/*Compensate again if SuperBoost is enabled in addition to Boost*/
		/*Check for LNA SuperBoost in 3412 Only*/
		if (h->pTnrModeData->LNA_ChipID == BTNR_LNAID_3412)
		{
			if (BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_LNA_ADDR, SuperBoost_en) == 1)
			{
				GainReg = GainReg + 768; 
				h->pTnrStatus->LNA_Boost_Status = 2;
			}
		}
	}

	/*Get tilt*/
	ReadReg = BTNR_7550_P_LNA_ReadRegisterField(h,  BCM341X_regs_LNA1_CNTL, STAGE1_TILT_ON);
	h->pTnrStatus->LNA_Tilt_Status = 0;
	if (ReadReg == 1)
	{
			h->pTnrStatus->LNA_Tilt_Status = 1;

			/*Compensate gain if tilt is on, a 5 db rise from 500 MHz to 1 GHz*/
			if ((h->pTnrStatus->TunerFreq > 500000000) && (h->pTnrStatus->TunerFreq <= 1000000000))
			{
				GainReg = GainReg + (((h->pTnrStatus->TunerFreq-500000000)>>8)*5*256)/(500000000>>8); /*keep in range of 32 bits*/
			}
			else if (h->pTnrStatus->TunerFreq > 1000000000)
			{
				GainReg = GainReg + 1280;  /*add 5 db*/
			}
	}

	/*assign the tilt compensated gain*/
	h->pTnrStatus->LNA_Gain_256db = (uint32_t)GainReg;
}

/****************************************************************************************************/
/* END OF LNA FUNCTIONS MAINTAINED BY TUNING CODE *************************************************/
/****************************************************************************************************/
