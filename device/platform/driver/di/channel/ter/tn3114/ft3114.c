/* $Header: */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name:		template.c
// Original Author: Full Name!!
// File Description:
// Module:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
// Start Including Header Files
#include "htype.h"
#include "di_channel_priv.h"

// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define CONST_CHANNEL_I2C_BUS	(2)
#define CONST_CXD2820_I2CADDR	(0xD8>>1)
#define CONST_CXD2820_GWSUBADDR	(0x09)
#define CONST_FT3114_I2CADDR	(0xC0)

#define CONST_IF_FREQ	(36000) /* 36MHz */
#define CONST_REF_FREQ	(166.667) /* 166.667KHz */
#define KHZ_TO_HZ	(1000)
#define SHIFT_CP	(6)
#define SHIFT_TOP	(3)
#define AGC_HIGH_CURRENT	(0x20)
// End #define


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef
typedef enum{
	ADB = 0,
	DB1, 		/* Divider Byte 1 */
	DB2,		/* Divider Byte 2 */
	CB1,		/* Control Byte 1st */
	BB,			/* Band switch Byte */
	CB2,		/* Control Byte 2nd */
	MAXBYTE
} FT3114_Reg_e;

typedef enum
{
	FT3114_CP0	= 0,
	FT3114_CP1	= 1,
	FT3114_CP2	= 2,
	FT3114_CP3	= 3
} FT3114_ChargePump_e;

typedef enum
{
	FT3114_BS0	= 0x1,
	FT3114_BS1	= 0x2,
	FT3114_BS2	= 0x8
} FT3114_BandSelect_e;

typedef enum{
	FT3114_TOP_112 = 0,
	FT3114_TOP_109,
	FT3114_TOP_106,
	FT3114_TOP_103,
	FT3114_TOP_97,
	FT3114_TOP_94,
	FT3114_TOP_DISABLE
} FT3114_TakeOverPoint_e;

typedef enum{
	FT3114_REF_166 = 0,
	FT3114_REF_142,
	FT3114_REF_80,
	FT3114_REF_62,
	FT3114_REF_31,
	FT3114_REF_50
} FT3114_RefDivisionRatio_e;

typedef enum {
    DVB_SYSTEM_UNKNOWN,    /**< Unknown. */
    DVB_SYSTEM_DVBC,       /**< DVB-C. */
    DVB_SYSTEM_DVBT,       /**< DVB-T. */
    DVB_SYSTEM_DVBT2       /**< DVB-T2. */
} FT3114_DvbSystem_e;

// End typedef


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee

// End global variable


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee

// End static variable


/****************** Function 시작점에 필히 기록 ********************/
/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name:
// Function Description:
// Parameter:
// Return Value:

static HUINT8 SetChargePumpCurrentBandSelect(HUINT32 ulFreqOsc)  // Digital Charge Pump setting
{
	HUINT8	ucChargePumpBand = 0;

	if(ulFreqOsc <= 121000) 	ucChargePumpBand = (FT3114_CP0<<SHIFT_CP)|FT3114_BS0;
	else if((ulFreqOsc > 121000) && (ulFreqOsc <= 141000)) ucChargePumpBand = (FT3114_CP1<<SHIFT_CP)|FT3114_BS0;
	else if((ulFreqOsc > 141000) && (ulFreqOsc <= 166000)) ucChargePumpBand = (FT3114_CP2<<SHIFT_CP)|FT3114_BS0;
	else if((ulFreqOsc > 166000) && (ulFreqOsc <= 182000)) ucChargePumpBand = (FT3114_CP3<<SHIFT_CP)|FT3114_BS0;
	else if((ulFreqOsc > 182000) && (ulFreqOsc <= 286000)) ucChargePumpBand = (FT3114_CP0<<SHIFT_CP)|FT3114_BS1;
	else if((ulFreqOsc > 286000) && (ulFreqOsc <= 386000)) ucChargePumpBand = (FT3114_CP1<<SHIFT_CP)|FT3114_BS1;
	else if((ulFreqOsc > 386000) && (ulFreqOsc <= 446000)) ucChargePumpBand = (FT3114_CP2<<SHIFT_CP)|FT3114_BS1;
	else if((ulFreqOsc > 446000) && (ulFreqOsc <= 466000)) ucChargePumpBand = (FT3114_CP3<<SHIFT_CP)|FT3114_BS1;
	else if((ulFreqOsc > 466000) && (ulFreqOsc <= 506000)) ucChargePumpBand = (FT3114_CP0<<SHIFT_CP)|FT3114_BS2;
	else if((ulFreqOsc > 506000) && (ulFreqOsc <= 761000)) ucChargePumpBand = (FT3114_CP1<<SHIFT_CP)|FT3114_BS2;
	else if((ulFreqOsc > 761000) && (ulFreqOsc <= 846000)) ucChargePumpBand = (FT3114_CP2<<SHIFT_CP)|FT3114_BS2;
	else if((ulFreqOsc > 846000) && (ulFreqOsc <= 905000)) ucChargePumpBand = (FT3114_CP3<<SHIFT_CP)|FT3114_BS2;
	else	ucChargePumpBand =(FT3114_CP3<<SHIFT_CP)|FT3114_BS2;

	return ucChargePumpBand;
}


int FT3114_SetTune ( HUINT32 ulRfFreqKhz, FT3114_DvbSystem_e etSystem, HUINT8 bandWidth )
{
	HUINT8 aucWriteDate[MAXBYTE];
	HUINT32 ulDivideCount;
	HUINT32 ulFreqOsc;
	int i, nRet = 0;

	ulFreqOsc = (ulRfFreqKhz + CONST_IF_FREQ);
	ulDivideCount = (ulFreqOsc / CONST_REF_FREQ) + 0.5;

	aucWriteDate[ADB] = CONST_FT3114_I2CADDR;

	aucWriteDate[DB1] = 0x7F & (ulDivideCount >> 8);
	aucWriteDate[DB2] = 0xFF & ulDivideCount;

	aucWriteDate[CB1] = (0x80|(FT3114_TOP_109<<SHIFT_TOP)|FT3114_REF_166);

	aucWriteDate[BB]  = SetChargePumpCurrentBandSelect(ulFreqOsc);

	aucWriteDate[CB2] = (0xc3|AGC_HIGH_CURRENT);

	for( i=0; i<MAXBYTE; i++)
	{
		CH_PrintData(" 0x%x: ", aucWriteDate[i]);
	}
	CH_PrintData("\n");

	nRet = DRV_I2c_WriteA8( CONST_CHANNEL_I2C_BUS, CONST_CXD2820_I2CADDR, CONST_CXD2820_GWSUBADDR,\
							aucWriteDate, MAXBYTE );
    if ( nRet != 0 )
    {
    	CH_PrintErr(" ");
        return -1;
    }

	VK_TASK_Sleep(70);

	aucWriteDate[CB2] = (0xc3&(~AGC_HIGH_CURRENT));

	for( i=0; i<MAXBYTE; i++)
	{
		CH_PrintData(" 0x%x: ", aucWriteDate[i]);
	}
	CH_PrintData("\n");

	nRet = DRV_I2c_WriteA8( CONST_CHANNEL_I2C_BUS, CONST_CXD2820_I2CADDR, CONST_CXD2820_GWSUBADDR,\
							aucWriteDate, MAXBYTE );
    if ( nRet != 0 )
    {
    	CH_PrintErr(" ");
        return -1;
    }

    return 0;

}
/*********************** End of File ******************************/