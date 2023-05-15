/*******************************************************************************
 *
 * FILE NAME              : MxL_Common.h
 *
 * AUTHOR                 : Howard Chan / Patrick Dizon
 * DATE CREATED       : June 4, 2010
 *
 * DESCRIPTION        : MxL201RF V6 Type enumeration & struct declaration
 *
 * Version                  :    6.2.5
 *******************************************************************************
 *                Copyright (c) 2010, MaxLinear, Inc.
 ******************************************************************************/

#ifndef __MxL201RF_COMMON_H
#define __MxL201RF_COMMON_H


/******************************************************************************
    User-Defined Types (Typedefs)
******************************************************************************/
typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;
typedef char           SINT8;
typedef short          SINT16;
typedef int            SINT32;
typedef float          REAL32;
typedef int           BOOL;

/*
typedef enum
{
  TRUE = 1,
  FALSE = 0
} BOOL;
*/


/******************************/
/*	MxL201RF Err message  	  */
/******************************/
typedef enum{
	MxL_OK					=   0x0,
	MxL_ERR_INIT			=   0x1,
	MxL_ERR_RFTUNE			=   0x2,
	MxL_ERR_SET_REG			=   0x3,
	MxL_ERR_GET_REG			=   0x4,
	MxL_ERR_MODE			=   0x10,
	MxL_ERR_IF_FREQ			=   0x11,
	MxL_ERR_XTAL_FREQ		=   0x12,
	MxL_ERR_BANDWIDTH		=   0x13,
	MxL_GET_ID_FAIL			=   0x14,
	MxL_ERR_OTHERS			=   0x0A
}MxL_ERR_MSG;

/******************************/
/*	MxL201RF Chip verstion     */
/******************************/
typedef enum{
	MxL_UNKNOWN_ID		= 0x00,
	MxL_201RF_ES4		= 0x06
}MxL201RF_ChipVersion;


/******************************************************************************
    CONSTANTS
******************************************************************************/

#ifndef MHz
	#define MHz 1000000
#endif

#define MAX_ARRAY_SIZE 100

#if defined(CONFIG_PLATFORM_TM201COMBO)
#define MASTER_I2C_BUS			2
#define SLAVE_I2C_BUS			3
#endif

/* Enumeration of Mode */
typedef enum
{
	MxL_MODE_DVBT	= 0x01,
	MxL_MODE_ATSC	= 0x02,
	MxL_MODE_CAB_OPT1 = 0x10,
	MxL_MODE_CAB_STD = 0x11  /*Cable "Standard" setting */
} MxL201RF_Mode ;

/* Enumeration of Acceptable IF Frequencies */
typedef enum
{
	MxL_IF_4_MHZ	  = 4000000,
	MxL_IF_4_5_MHZ	  = 4500000,
	MxL_IF_4_57_MHZ	  =	4570000,
	MxL_IF_5_MHZ	  =	5000000,
	MxL_IF_5_38_MHZ	  =	5380000,
	MxL_IF_6_MHZ	  =	6000000,
	MxL_IF_6_28_MHZ	  =	6280000,
	MxL_IF_7_2_MHZ    = 7200000,
	MxL_IF_35_25_MHZ  = 35250000,
	MxL_IF_36_MHZ	  = 36000000,
	MxL_IF_36_15_MHZ  = 36150000,
	MxL_IF_44_MHZ	  = 44000000
} MxL201RF_IF_Freq ;

/* Enumeration of Acceptable Crystal Frequencies */
typedef enum
{
	MxL_XTAL_16_MHZ		= 16000000,
	MxL_XTAL_20_MHZ		= 20000000,
	MxL_XTAL_20_25_MHZ	= 20250000,
	MxL_XTAL_20_48_MHZ	= 20480000,
	MxL_XTAL_24_MHZ		= 24000000,
	MxL_XTAL_25_MHZ		= 25000000,
	MxL_XTAL_25_14_MHZ	= 25140000,
	MxL_XTAL_28_8_MHZ	= 28800000,
	MxL_XTAL_32_MHZ		= 32000000,
	MxL_XTAL_40_MHZ		= 40000000,
	MxL_XTAL_44_MHZ		= 44000000,
	MxL_XTAL_48_MHZ		= 48000000,
	MxL_XTAL_49_3811_MHZ = 49381100
} MxL201RF_Xtal_Freq ;

/* Enumeration of Acceptable IF Bandwidths */
typedef enum
{
	MxL_BW_6MHz = 6,
	MxL_BW_7MHz = 7,
	MxL_BW_8MHz = 8
} MxL201RF_BW_MHz;

/* Enumeration of Inverted/Normal IF Spectrum */
typedef enum
{
	MxL_NORMAL_IF = 0 ,
	MxL_INVERT_IF

} MxL201RF_IF_Spectrum ;

/* Enumeration of Clock out Enable/Disable */
typedef enum
{
	MxL_CLKOUT_DISABLE = 0 ,
	MxL_CLKOUT_ENABLE

} MxL201RF_ClkOut;

/* Enumeration of Clock out Amplitude */
typedef enum
{
	MxL_CLKOUT_AMP_0 = 0 , 	/* min Clock out Amplitude */
	MxL_CLKOUT_AMP_1,
	MxL_CLKOUT_AMP_2,
	MxL_CLKOUT_AMP_3,
	MxL_CLKOUT_AMP_4,
	MxL_CLKOUT_AMP_5,
	MxL_CLKOUT_AMP_6,
	MxL_CLKOUT_AMP_7,
	MxL_CLKOUT_AMP_8,
	MxL_CLKOUT_AMP_9,
	MxL_CLKOUT_AMP_10,
	MxL_CLKOUT_AMP_11,
	MxL_CLKOUT_AMP_12,
	MxL_CLKOUT_AMP_13,
	MxL_CLKOUT_AMP_14,
	MxL_CLKOUT_AMP_15  /* max Clock out Amplitude */

} MxL201RF_ClkOut_Amp;

/* Enumeration of I2C Addresses */
typedef enum
{
	MxL_I2C_ADDR_96 = 96 ,
	MxL_I2C_ADDR_97 = 97 ,
	MxL_I2C_ADDR_98 = 98 ,
	MxL_I2C_ADDR_99 = 99
} MxL201RF_I2CAddr ;

/* Enumeration of Acceptable Crystal Capacitor values */
typedef enum
{
	MxL_XTAL_CAP_0_PF = 0,
	MxL_XTAL_CAP_1_PF = 1,
	MxL_XTAL_CAP_2_PF = 2,
	MxL_XTAL_CAP_3_PF = 3,
	MxL_XTAL_CAP_4_PF = 4,
	MxL_XTAL_CAP_5_PF = 5,
	MxL_XTAL_CAP_6_PF = 6,
	MxL_XTAL_CAP_7_PF = 7,
	MxL_XTAL_CAP_8_PF = 8,
	MxL_XTAL_CAP_9_PF = 9,
	MxL_XTAL_CAP_10_PF = 10,
	MxL_XTAL_CAP_11_PF = 11,
	MxL_XTAL_CAP_12_PF = 12,
	MxL_XTAL_CAP_13_PF = 13,
	MxL_XTAL_CAP_14_PF = 14,
	MxL_XTAL_CAP_15_PF = 15,
	MxL_XTAL_CAP_16_PF = 16,
	MxL_XTAL_CAP_17_PF = 17,
	MxL_XTAL_CAP_18_PF = 18,
	MxL_XTAL_CAP_19_PF = 19,
	MxL_XTAL_CAP_20_PF = 20,
	MxL_XTAL_CAP_21_PF = 21,
	MxL_XTAL_CAP_22_PF = 22,
	MxL_XTAL_CAP_23_PF = 23,
	MxL_XTAL_CAP_24_PF = 24,
	MxL_XTAL_CAP_25_PF = 25
}	MxL201RF_Xtal_Cap;



/* =====================
   MxL201RF Tuner Struct
   ===================== */
typedef struct _MxL201RF_TunerConfigS
{
	MxL201RF_Mode		Mode;
	MxL201RF_Xtal_Freq	Xtal_Freq;
	MxL201RF_IF_Freq	IF_Freq;
	MxL201RF_IF_Spectrum IF_Spectrum;
	MxL201RF_ClkOut		ClkOut_Setting;
    MxL201RF_ClkOut_Amp	ClkOut_Amp;
	MxL201RF_BW_MHz		BW_MHz;
	UINT32				RF_Freq_Hz;
	MxL201RF_Xtal_Cap	Xtal_Cap;
#if defined(CONFIG_PLATFORM_TM201COMBO)				// JD - MASTER/SLAVE, TER/CAB를 동시에 구분하기 위해 Channel ID를 받아야함
	SINT32 					nChannelId;
#endif
#if 0
        struct semosal_i2c_obj  i2c;                /* I: i2c controller abstract interface */
#endif
} MxL201RF_TunerConfigS ;

#endif /* __MxL201RF_COMMON_H__*/

/*

 Driver APIs for MxL201RF Tuner

 Copyright, Maxlinear, Inc.
 All Rights Reserved

 File Name:      MxL201RF_API.h


 */
#ifndef __MxL201RF_API_H
#define __MxL201RF_API_H

/******************************************************************************
**
**  Name: MxL_I2C_Write
**
**  Description:    I2C write operations
**
**  Parameters:
**					DeviceAddr	- MxL201RF Device address
**					pArray		- Write data array pointer
**					count		- total number of array
**
**  Returns:        0 if success
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   12-16-2007   khuang initial release.
**
******************************************************************************/
UINT32 MxL_I2C_Write(MxL201RF_TunerConfigS* p_obj, UINT8* pArray, UINT32 count);

/******************************************************************************
**
**  Name: MxL_I2C_Read
**
**  Description:    I2C read operations
**
**  Parameters:
**					DeviceAddr	- MxL201RF Device address
**					Addr		- register address for read
**					*Data		- data return
**
**  Returns:        0 if success
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   12-16-2007   khuang initial release.
**
******************************************************************************/
UINT32 MxL_I2C_Read(MxL201RF_TunerConfigS* p_obj, UINT8 Addr, UINT8* mData);

/******************************************************************************
**
**  Name: MxL_Delay
**
**  Description:    Delay function in milli-second
**
**  Parameters:
**					mSec		- milli-second to delay
**
**  Returns:        0
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   12-16-2007   khuang initial release.
**
******************************************************************************/
void MxL_Delay(UINT32 mSec);

/******************************************************************************
**
**  Name: MxL_Set_Register
**
**  Description:    Write one register to MxL201RF
**
**  Parameters:    	myTuner				- Pointer to MxL201RF_TunerConfigS
**					RegAddr				- Register address to be written
**					RegData				- Data to be written
**
**  Returns:        MxL_ERR_MSG			- MxL_OK if success
**										- MxL_ERR_SET_REG if fail
**
******************************************************************************/
MxL_ERR_MSG MxL_Set_Register(MxL201RF_TunerConfigS* myTuner, UINT8 RegAddr, UINT8 RegData);

/******************************************************************************
**
**  Name: MxL_Get_Register
**
**  Description:    Read one register from MxL201RF
**
**  Parameters:    	myTuner				- Pointer to MxL201RF_TunerConfigS
**					RegAddr				- Register address to be read
**					RegData				- Pointer to register read
**
**  Returns:        MxL_ERR_MSG			- MxL_OK if success
**										- MxL_ERR_GET_REG if fail
**
******************************************************************************/
MxL_ERR_MSG MxL_Get_Register(MxL201RF_TunerConfigS* myTuner, UINT8 RegAddr, UINT8 *RegData);

/******************************************************************************
**
**  Name: MxL_Tuner_Init
**
**  Description:    MxL201RF Initialization
**
**  Parameters:    	myTuner				- Pointer to MxL201RF_TunerConfigS
**
**  Returns:        MxL_ERR_MSG			- MxL_OK if success
**										- MxL_ERR_INIT if fail
**
******************************************************************************/
MxL_ERR_MSG MxL_Tuner_Init(MxL201RF_TunerConfigS* );

/******************************************************************************
**
**  Name: MxL_Tuner_RFTune
**
**  Description:    Frequency tunning for channel
**
**  Parameters:    	myTuner				- Pointer to MxL201RF_TunerConfigS
**					RF_Freq_Hz			- RF Frequency in Hz
**					BWMHz				- Bandwidth 6, 7 or 8 MHz
**
**  Returns:        MxL_ERR_MSG			- MxL_OK if success
**										- MxL_ERR_RFTUNE if fail
**
******************************************************************************/
MxL_ERR_MSG MxL_Tuner_RFTune(MxL201RF_TunerConfigS*, UINT32 RF_Freq_Hz, MxL201RF_BW_MHz BWMHz);

/******************************************************************************
**
**  Name: MxL_Soft_Reset
**
**  Description:    Software Reset the MxL201RF Tuner
**
**  Parameters:    	myTuner				- Pointer to MxL201RF_TunerConfigS
**
**  Returns:        MxL_ERR_MSG			- MxL_OK if success
**										- MxL_ERR_OTHERS if fail
**
******************************************************************************/
MxL_ERR_MSG MxL_Soft_Reset(MxL201RF_TunerConfigS*);

/******************************************************************************
**
**  Name: MxL_Standby
**
**  Description:    Enter Standby Mode
**
**  Parameters:    	myTuner				- Pointer to MxL201RF_TunerConfigS
**
**  Returns:        MxL_ERR_MSG			- MxL_OK if success
**										- MxL_ERR_OTHERS if fail
**
******************************************************************************/
MxL_ERR_MSG MxL_Stand_By(MxL201RF_TunerConfigS*);

/******************************************************************************
**
**  Name: MxL_Wakeup
**
**  Description:    Wakeup from Standby Mode (Note: after wake up, please call RF_Tune again)
**
**  Parameters:    	myTuner				- Pointer to MxL201RF_TunerConfigS
**
**  Returns:        MxL_ERR_MSG			- MxL_OK if success
**										- MxL_ERR_OTHERS if fail
**
******************************************************************************/
MxL_ERR_MSG MxL_Wake_Up(MxL201RF_TunerConfigS*);

/******************************************************************************
**
**  Name: MxL_Enable_LT
**
**  Description:    Enable/Disable the on-chip loop-thru
**
**  Parameters:    	myTuner				- Pointer to MxL201RF_TunerConfigS
**					En_LT				- 0: disable loop-thru, otherwise enable loop-thru
**
**  Returns:        MxL_ERR_MSG			- MxL_OK if success
**										- MxL_ERR_SET_REG if fail
**
******************************************************************************/
MxL_ERR_MSG MxL_Enable_LT(MxL201RF_TunerConfigS* myTuner, UINT8 En_LT);

/******************************************************************************
**
**  Name: MxL_Check_ChipVersion
**
**  Description:    Return the MxL201RF Chip ID
**
**  Parameters:    	myTuner				- Pointer to MxL201RF_TunerConfigS
**					myChipVersion		- MxL_ChipVersion
**  Returns:        MxL_ERR_MSG
**
******************************************************************************/
MxL_ERR_MSG MxL_Check_ChipVersion(MxL201RF_TunerConfigS*, MxL201RF_ChipVersion*);

/******************************************************************************
**
**  Name: MxL_RFSynth_Lock_Status
**
**  Description:    RF synthesizer lock status of MxL201RF
**
**  Parameters:    	myTuner				- Pointer to MxL201RF_TunerConfigS
**					isLock				- Pointer to Lock Status
**
**  Returns:        MxL_ERR_MSG			- MxL_OK if success
**										- MxL_ERR_OTHERS if fail
**
******************************************************************************/
MxL_ERR_MSG MxL_RFSynth_Lock_Status(MxL201RF_TunerConfigS* , BOOL* isLock);

/******************************************************************************
**
**  Name: MxL_REFSynth_Lock_Status
**
**  Description:    REF synthesizer lock status of MxL201RF
**
**  Parameters:    	myTuner				- Pointer to MxL201RF_TunerConfigS
**					isLock				- Pointer to Lock Status
**
**  Returns:        MxL_ERR_MSG			- MxL_OK if success
**										- MxL_ERR_OTHERS if fail
**
******************************************************************************/
MxL_ERR_MSG MxL_REFSynth_Lock_Status(MxL201RF_TunerConfigS* , BOOL* isLock);

/******************************************************************************
**
**  Name: MxL_Check_RF_Input_Power
**
**  Description:    Reads the RF Input power
**
**  Parameters:    	myTuner				- Pointer to MxL201RF_TunerConfigS
**					RF_Input_Level		- Pointer to return value, RF input level
**
**  Returns:        MxL_ERR_MSG			- MxL_OK if success
**										- MxL_ERR_OTHERS if fail
**
******************************************************************************/
MxL_ERR_MSG MxL_Check_RF_Input_Power(MxL201RF_TunerConfigS* , REAL32* RF_Input_Level);

/******************************************************************************
**
**  Name: MxL_Input_Power_LUT_Reg_Write
**
**  Description:    Overwrite the registers used by LUT for input power reporting
**
**  Parameters:    	myTuner				- Pointer to MxL201RF_TunerConfigS
**					LUT_Index			- Sub-address index of LUT to be written
					LUT_Val				- Value to be written to LUT
**
**  Returns:        MxL_ERR_MSG			- MxL_OK if success
**										- MxL_ERR_OTHERS if fail
**
******************************************************************************/
MxL_ERR_MSG MxL_Input_Power_LUT_Reg_Write(MxL201RF_TunerConfigS* , UINT8 LUT_Index, UINT8 LUT_Val);

#endif //__MxL201RF_API_H

