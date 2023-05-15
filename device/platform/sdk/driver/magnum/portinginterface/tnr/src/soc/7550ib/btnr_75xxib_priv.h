/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_75xxib_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/7/12 10:11a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/soc/7550ib/btnr_75xxib_priv.h $
 * 
 * Hydra_Software_Devel/1   9/7/12 10:11a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/30   2/4/11 8:50p farshidf
 * SW7550-423: fix 60 and 65 nm
 * 
 * Hydra_Software_Devel/29   9/29/10 10:23a farshidf
 * SW7550-542: add the device type
 * 
 * Hydra_Software_Devel/28   5/19/10 5:59p farshidf
 * SW7550-400: add the ofdm signal strength and dpm power level
 * 
 * Hydra_Software_Devel/27   5/18/10 12:00p farshidf
 * SW7550-440: merge to main branch
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/SW7550-440/1   5/18/10 11:47a farshidf
 * SW7550-440: add the dpm callback
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/3   5/13/10 6:17p cbrooks
 * Added DPM
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/2   5/10/10 6:47p cbrooks
 * added DPM
 * 
 * Hydra_Software_Devel/TNR_7550_40_10/1   4/29/10 5:16p cbrooks
 * Added miniTune
 * 
 * Hydra_Software_Devel/26   4/27/10 1:37p farshidf
 * SW7550-38: merge from branch 9 of TNR
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/5   4/24/10 1:38p cbrooks
 * Added support for MiniTune and Tuner_AGC_Status
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/4   4/23/10 5:17p cbrooks
 * Cleaned up lots of tuner code
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/3   4/20/10 6:57p cbrooks
 * BEGIN TUNER CODE CLEANUP
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/2   4/14/10 3:34p cbrooks
 * clean up status
 * 
 * Hydra_Software_Devel/TNR_7550_40_9/1   2/24/10 5:06p farshidf
 * SW7550-38: RF bypass update
 * 
 * Hydra_Software_Devel/22   1/8/10 5:20p farshidf
 * SW7550-38: compile fix
 * 
 * Hydra_Software_Devel/21   1/8/10 4:32p farshidf
 * SW7550-38: merge the TNR branch 6
 * 
 * Hydra_Software_Devel/TNR_7550_40_6/1   1/8/10 3:44p cbrooks
 * Added the tuner gains
 * 
 * Hydra_Software_Devel/20   12/28/09 2:45p farshidf
 * SW7550-38: be able to tune with BBS with Nexus
 * 
 * Hydra_Software_Devel/19   12/23/09 9:58a farshidf
 * sw7550-38: merge from branch 5 of tnr
 * 
 * Hydra_Software_Devel/TNR_7550_40_5/2   12/18/09 2:45p thayashi
 * Updated the way lna chip id is passed
 * 
 * Hydra_Software_Devel/TNR_7550_40_5/1   12/17/09 9:58p thayashi
 * Added LNA chip id as an aquiredsetting
 * 
 * Hydra_Software_Devel/17   12/4/09 12:39p farshidf
 * SW7550-38: merge the branch 3
 * 
 * Hydra_Software_Devel/TNR_7550_40_3/1   11/25/09 5:51p thayashi
 * incl latest changes from charlie
 * 
 * Hydra_Software_Devel/15   11/23/09 1:12p farshidf
 * SW7550-38: merge from branch
 * 
 * Hydra_Software_Devel/TNR_7550_40_2/2   11/23/09 12:49p cbrooks
 * Ready To Merge With PI Ver0.0
 * 
 * Hydra_Software_Devel/TNR_7550_40_2/1   11/20/09 1:04p cbrooks
 * First Pass Tryout
 * 
 * Hydra_Software_Devel/13   11/19/09 5:10p farshidf
 * SW7550-38: check-in latest changes
 * 
 * Hydra_Software_Devel/11   11/17/09 8:10p farshidf
 * SW7550-38: ad the new nexus IF
 * 
 * Hydra_Software_Devel/10   11/17/09 5:28p farshidf
 * SW7550-38: merge the latest
 * 
 * Hydra_Software_Devel/THD_7550_40_6/1   11/17/09 5:28p farshidf
 * SW7550-38: add the I2c reg handle
 * 
 * Hydra_Software_Devel/9   11/17/09 5:16p farshidf
 * SW7550-38: add the i2c handle
 * 
 * Hydra_Software_Devel/8   11/16/09 4:42p farshidf
 * SW7550-38: include Charles's latest changes
 * 
 * Hydra_Software_Devel/7   11/16/09 4:00p farshidf
 * SW7550-38: add the LNA code
 * 
 * Hydra_Software_Devel/6   11/13/09 11:28a farshidf
 * SW7550-38: update the BBS interface
 * 
 * Hydra_Software_Devel/5   11/12/09 10:04p farshidf
 * SW7550-38: BBS code changes required
 * 
 * Hydra_Software_Devel/4   11/12/09 8:42p farshidf
 * SW7550-38: bbs control function
 * 
 * Hydra_Software_Devel/3   11/12/09 6:57p farshidf
 * SW7550-40: update the tuner files
 * 
 * Hydra_Software_Devel/2   11/10/09 7:11p farshidf
 * 7550-38: add the tuner code
 * 
 * Hydra_Software_Devel/1   11/5/09 10:24a farshidf
 * SW7550-38: add the 7550 tuner code
 *  
 ***************************************************************************/
#ifndef BTNR_75XXIB_PRIV_H__
#define BTNR_75XXIB_PRIV_H__

#include "bchp.h"
#include "breg_mem.h"
#include "bint.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "berr_ids.h"
#include "btnr.h"
#include "bmem.h"
#include "breg_i2c.h"
#include "btnr_7550ib.h"

#ifdef __cplusplus
extern "C" {
#endif


#define BTNR_7550_P_WriteField(h, Register, Field, Data) \
  BREG_Write32(h, BCHP_##Register, ((BREG_Read32(h, BCHP_##Register) & \
  ~((uint32_t)BCHP_MASK(Register, Field))) | \
  BCHP_FIELD_DATA(Register, Field, Data)))


#define BTNR_7550_P_ReadField(h, Register, Field) \
  ((((BREG_Read32(h, BCHP_##Register)) & BCHP_MASK(Register,Field)) >> \
  BCHP_SHIFT(Register,Field)))

#ifdef TUNE_BBS_TEST
#define INIT_BBS_CLICK  0x1
#define TUNE_BBS_CLICK  0x2
#endif

/*******************************************************************************
*
*   Private Module Handles
*
*******************************************************************************/

typedef struct BTNR_P_75xx_Handle     *BTNR_75xx_Handle;

typedef enum BTNR_LNA_ID_s
{
	BTNR_LNAID_3410 = 0,
	BTNR_LNAID_3412 = 1
}BTNR_LNA_ID_t;

typedef enum BTNR_OFFON_s
{
	BTNR_OFF = 0,
	BTNR_ON = 1
}BTNR_OFFON_t;

typedef enum SignalSource_s
{
	InternalTuner=0,
	ExternalTuner=1
} SignalSource_t;


typedef enum TNR_TuneStartMode_s
{
  TNR_TuneStartMode_Manual = 1, 
  TNR_TuneStartMode_ResetStatus = 2,
  TNR_TuneStartMode_DPMStatus = 4
} TNR_TuneStartMode_t;

typedef enum TNR_NexusStatusMode_s
{
  TNR_NexusStatusMode_EnableStatusForNexus = 1
} TNR_NexusStatusMode_t;

typedef enum TNR_NexusParamsMode_s
{
  TNR_NexusParamsMode_eDisable = 1
} TNR_NexusParamsMode_t;

typedef enum TNR_DeviceTechType_s
{
  TNR_DeviceTechType_e65nm = 0,
  TNR_DeviceTechType_e60nm = 1
} TNR_DeviceTechType_t;

/* internal Acquire settings */
typedef struct  BTNR_P_75xx_AcquireSettings
{
	uint32_t												StartSturctureAddress;
	volatile TNR_TuneStartMode_t		BBSConnectMode;
	TNR_NexusParamsMode_t						NexusParamsDisable;					
	BTNR_7550_TunerAcquisitionType	TunerAcquisitionType;
	BTNR_7550_Bandwidth							TunerBw;
	BTNR_7550_Standard							TunerAcquisitionMode;
	BTNR_7550_Setup									TunerSetup;
	uint32_t												TunerFreq;
	volatile TNR_NexusStatusMode_t  BBSNexusConnectMode;
	uint32_t			BBS_AcqWord0;					/*Beginning of 37 words of memory for BBS*/ 
	uint32_t			BBS_AcqWord1;
	uint32_t			BBS_AcqWord2;
	uint32_t			BBS_AcqWord3;
	uint32_t			BBS_LoopVGAGain_Init_Dwell;				 
	uint32_t			BBS_LoopVGAGain_VGA_Dwell;	 
	uint32_t			BBS_LoopVGAGain_notVGA_Dwell;
	uint32_t			BBS_LoopVGAGain_Display;
	uint32_t			BBS_Input_Source;
	uint32_t			BBS_Rerun_Init;
	uint32_t			BBS_THD_RF_AGC_Acq_BW;
	uint32_t 			BBS_THD_IF_AGC_Acq_BW;
	uint32_t 			BBS_THD_RF_AGC_Trk_BW;
	uint32_t			BBS_THD_IF_AGC_Trk_BW;
	uint32_t 			BBS_THD_DCOC_On;
	uint32_t 			BBS_THD_DCOC_Acq_BW;
	uint32_t 			BBS_THD_DCOC_Trk_BW;
	uint32_t 			BBS_THD_BCM3410_Auto;
	uint32_t 			BBS_THD_BCM3410_Gain_State;
	uint32_t 			BBS_THD_LNA_Boost_On;
	uint32_t 			BBS_THD_LNA_Tilt_On;
	uint32_t 			BBS_THD_LNA_Tilt_Freq;
	uint32_t 			BBS_DS_RF_AGC_Acq_BW;
	uint32_t 			BBS_DS_IF_AGC_Acq_BW;
	uint32_t 			BBS_DS_RF_AGC_Trk_BW;
	uint32_t 			BBS_DS_IF_AGC_Trk_BW;
	uint32_t 			BBS_DS_DCOC_On;
	uint32_t 			BBS_DS_DCOC_Acq_BW;
	uint32_t 			BBS_DS_DCOC_Trk_BW;
	uint32_t 			BBS_DS_BCM3410_Auto;
	uint32_t 			BBS_DS_BCM3410_Gain_State;
	uint32_t 			BBS_DS_LNA_Boost_On;
	uint32_t 			BBS_DS_LNA_Tilt_On;
	uint32_t 			BBS_DS_LNA_Tilt_Freq;		
	uint32_t			BBS_Cable_LNA_Address;
	uint32_t			BBS_Terrestrial_LNA_Address;	 
	uint32_t			BBS_Force_TunerAcquisitionType; /*End of 37 words of memory for BBS*/
	BTNR_OFFON_t	DoneLoadFlag;
	BTNR_OFFON_t	DoneFirstTimeFlag;
	BTNR_OFFON_t	ReRunInit;
	uint8_t				Ref_PLL_Status;
	uint8_t				Mixer_PLL_Status;
	uint8_t				LNA_Gain;
	BTNR_LNA_ID_t	LNA_ChipID;
	bool				RFbypassMode;
} BTNR_P_75xx_AcquireSettings_t;							/*this is part of the PI but since it is bool it needs to be after the BBS stuff*/

/*structure for status parameters to/from Kaylin to C script	*/   
/*structure for status parameters to/from BBS to C script 		*/ 
typedef struct BTNR_75xx_P_Status
{ 
  uint32_t	Tuner_Ref_Lock_Status; 		/*Beginning of 38 words of memory for BBS*/
  uint32_t	Tuner_Mixer_Lock_Status;
	uint32_t  Tuner_AGC_Status;
  uint32_t	TunerFreq;
  uint32_t	TunerMode;
  uint32_t	TunerBW;
  uint32_t	Cable_LNA_Gain;
  uint32_t	Cable_RF_Attenuation;
  uint32_t	Cable_RF_Tracking_Gain;
  uint32_t	Cable_RF_Tracking_BW;
  uint32_t	Cable_RF_Tracking_Roof;
  uint32_t	Terrestrial_LNA_Gain;
  uint32_t	Terrestrial_RFAGC_Gain;
  uint32_t	Terrestrial_LNA2_Gain;
	uint32_t	FGA_Gain;
  uint32_t	IF_VGA_Gain_Range;
  uint32_t	IF_VGA_Control_Value;
  uint32_t  LNA_AGC_LEVEL;
  uint32_t  RF_AGC_LEVEL;
  uint32_t  IF_AGC_LEVEL;
  uint32_t  IFDCVAL_I;
  uint32_t  IFDCVAL_Q;
	uint32_t	LNA_Gain_256db;
	uint32_t	Terrestrial_RF_Gain_256db;
	uint32_t	Terrestrial_LNA2_Gain_256db;
	uint32_t	Cable_HarmonicFilter;
	uint32_t	Cable_RF_Gain_256db;
	uint32_t	Cable_RF_TRKGain_256db;
	uint32_t	FGA_Gain_256db;
	uint32_t	IF_PGA_Gain_256db; 
	uint32_t	IF_VGA_Gain_256db; 
	uint32_t	ADC_IPGA_Gain_256db;  
	uint32_t	ADC_QPGA_Gain_256db;
	uint32_t	ADC_EXTPGA_Gain_256db;
	uint32_t	TotalGainToADC;
	uint32_t	AGF_Gain_256db;
	uint32_t	AGF2_Gain_256db;
	uint32_t	InputPower_256db; 			 
	uint32_t	LNA_Boost_Status;
	uint32_t	LNA_Tilt_Status;
	uint32_t  DPM_Gain_256dbmV;  /*End of 38 words of memory for BBS*/
	TNR_DeviceTechType_t    Device_Tech_type;
}BTNR_75xx_P_Status_t;


typedef struct BTNR_P_75xx_Settings
{
    unsigned long rfFreq;
    int iRevLetter, iRevNumber, iType;      /* Saved chip information */
    BTNR_TunerMode tunerMode;
    bool powerSaver;
} BTNR_P_75xx_Settings;

typedef struct BTNR_P_75xx_Handle
{
    uint32_t magicId;                   /* Used to check if structure is corrupt */
	BREG_Handle hRegister;
    BTNR_P_75xx_Settings settings;
	BTNR_P_75xx_AcquireSettings_t *pTnrModeData;
	BTNR_75xx_P_Status_t *pTnrStatus;
	BTNR_P_CallbackInfo InterruptCallbackInfo;
	BREG_I2C_Handle hI2cTHD;
	BREG_I2C_Handle hI2cADS;
	BMEM_Heap_Handle hHeap;
	BTMR_TimerHandle hTimer;
	BKNI_EventHandle hInterruptEvent;
} BTNR_P_75xx_Handle;

/* function prototypes in btnr_7550_acquire.c*/
BERR_Code BTNR_7550_P_Init_TNR_Core0(BTNR_75xx_Handle);
BERR_Code BTNR_7550_P_Tune_TNR_Core0(BTNR_75xx_Handle);
BERR_Code BTNR_7550_P_Get_LockStatus_TNR_Core0(BTNR_75xx_Handle);

/* function prototypes in btnr_7550_status.c*/
BERR_Code BTNR_7550_P_Reset_Status_TNR_Core0(BTNR_75xx_Handle);
BERR_Code BTNR_7550_P_Get_Status_TNR_Core0(BTNR_75xx_Handle);
BERR_Code BTNR_7550_P_Get_DPM(BTNR_75xx_Handle);

/* function prototypes in btnr_7550_utils.c*/
BERR_Code BTNR_7550_P_PowerDn_TNR_Core0(BTNR_75xx_Handle);
BERR_Code BTNR_7550_P_PowerUp_TNR_Core0(BTNR_75xx_Handle);

void BTNR_7550_P_Set_Rf_ByPass_mode(BTNR_75xx_Handle, bool);
void BTNR_7550_P_Get_LNA_Status(BTNR_75xx_Handle);

/*******************************************************************************
*
*   Private Module Data
*
*******************************************************************************/
BERR_Code BTNR_75xx_Close(
    BTNR_Handle hDev                   		/* [in] Device handle */
    );

BERR_Code BTNR_75xx_SetRfFreq(
    BTNR_75xx_Handle hDev,                /* [in] Device handle */
    uint32_t rfFreq,                    	/* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode            	/* [in] Requested tuner mode */
    );

BERR_Code BTNR_75xx_GetRfFreq(
    BTNR_75xx_Handle hDev,                /* [in] Device handle */
    uint32_t *rfFreq,                   	/* [output] Returns tuner freq., in Hertz */
    BTNR_TunerMode *tunerMode           	/* [output] Returns tuner mode */
    );

BERR_Code BTNR_P_75xx_GetAgcRegVal(
    BTNR_75xx_Handle hDev,                /* [in] Device handle */
    uint32_t regOffset,                 	/* [in] AGC register offset */
    uint32_t *agcVal                    	/* [out] output value */
    );

BERR_Code BTNR_75xx_GetInfo(
    BTNR_75xx_Handle hDev,                /* [in] Device handle */
    BTNR_TunerInfo *tnrInfo             	/* [out] Tuner information */
    );

BERR_Code BTNR_75xx_GetPowerSaver(
	BTNR_75xx_Handle hDev,				/* [in] Device handle */
	BTNR_PowerSaverSettings *pwrSettings	/* [in] Power saver settings. */
	);

BERR_Code BTNR_75xx_SetPowerSaver(
	BTNR_75xx_Handle hDev,				/* [in] Device handle */
	BTNR_PowerSaverSettings *pwrSettings 	/* [in] Power saver settings. */
	);



#ifdef __cplusplus
    }
#endif
    
#endif

