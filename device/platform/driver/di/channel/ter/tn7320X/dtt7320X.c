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

#include "drv_i2c.h"
// End Including Headers


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define
#define CONST_CHANNEL_I2C_BUS	(2)
#define CONST_CXD2820_I2CADDR	(0xD8>>1)
#define CONST_CXD2820_GWSUBADDR	(0x09)
#define CONST_DTT7320X_I2CADDR	(0xC2)

#define CONST_IF_FREQ	(36166.667) /* 36.166667MHz */
#define CONST_REF_FREQ	(166.667) /* 166.667KHz */
//#define KHZ_TO_HZ	(1000)
#define SHIFT_CP	(5)
#define SHIFT_TOP	(3)
#define AGC_HIGH_CURRENT	(0x20)
#define Step_Size (0x13)
#define CHANNEL_SEARCH (0x51)
#define CHANNEL_LOCK    (0x71)

#define  DTT73201_new

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
	AB,			    /*auxiliary byte*/
	MAXBYTE
} DTT7320X_Reg_e;

typedef enum
{
	DTT7320X_CP0	= 0,
	DTT7320X_CP1	= 1,
	DTT7320X_CP2	= 2,
	DTT7320X_CP3	= 3
} DTT7320X_ChargePump_e;

typedef enum {
    DVB_SYSTEM_UNKNOWN,    /**< Unknown. */
    DVB_SYSTEM_DVBC,       /**< DVB-C. */
    DVB_SYSTEM_DVBT,       /**< DVB-T. */
    DVB_SYSTEM_DVBT2       /**< DVB-T2. */
} DTT7320X_DvbSystem_e;

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
#ifdef DTT73201_new
	if(ulFreqOsc <= 263000) 	ucChargePumpBand = (DTT7320X_CP0<<SHIFT_CP);
	else if((ulFreqOsc > 263000) && (ulFreqOsc <= 802000)) ucChargePumpBand = (DTT7320X_CP0<<SHIFT_CP);
	else if((ulFreqOsc > 802000) && (ulFreqOsc <= 882000)) ucChargePumpBand = (DTT7320X_CP1<<SHIFT_CP);
	else if((ulFreqOsc > 882000) && (ulFreqOsc <= 898000)) ucChargePumpBand = (DTT7320X_CP3<<SHIFT_CP);
#else
	if(ulFreqOsc <= 236000) 	ucChargePumpBand = (DTT7320X_CP1<<SHIFT_CP);
	else if((ulFreqOsc > 236000) && (ulFreqOsc <= 263000)) ucChargePumpBand = (DTT7320X_CP2<<SHIFT_CP);
	else if((ulFreqOsc > 263000) && (ulFreqOsc <= 536000)) ucChargePumpBand = (DTT7320X_CP1<<SHIFT_CP);
	else if((ulFreqOsc > 536000) && (ulFreqOsc <= 811000)) ucChargePumpBand = (DTT7320X_CP2<<SHIFT_CP);
	else if((ulFreqOsc > 811000) && (ulFreqOsc <= 898000)) ucChargePumpBand = (DTT7320X_CP3<<SHIFT_CP);
#endif

	return ucChargePumpBand;
}

int DTT7320X_SetTune ( HUINT32 ulRfFreqKhz, DTT7320X_DvbSystem_e etSystem, HUINT8 bandWidth )
{
	HUINT8 aucWriteDate[MAXBYTE];
	HUINT32 ulDivideCount;
	HUINT32 ulFreqOsc;
	int i, nRet = 0;

	CH_UNUSED(etSystem);

	ulFreqOsc = (ulRfFreqKhz + CONST_IF_FREQ);
	ulDivideCount = (ulFreqOsc / CONST_REF_FREQ) + 0.5;

	aucWriteDate[ADB] = CONST_DTT7320X_I2CADDR;

	aucWriteDate[DB1] = 0x7F & (ulDivideCount >> 8);
	aucWriteDate[DB2] = 0xFF & ulDivideCount;

	aucWriteDate[CB1] = 0x80|Step_Size;

	aucWriteDate[CB1]|=SetChargePumpCurrentBandSelect(ulFreqOsc);

	if(bandWidth==7)
		aucWriteDate[BB]=0x09;
	else
		aucWriteDate[BB]=0x00; //8M BW

	if(ulFreqOsc<474000)
		aucWriteDate[BB] |=0x79; //Mid Band
	else
		aucWriteDate[BB] |=0xB0; //High Band

	aucWriteDate[CB2] = CHANNEL_SEARCH;

	aucWriteDate[AB] = 0x90;

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

	aucWriteDate[CB2] = CHANNEL_LOCK;

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
