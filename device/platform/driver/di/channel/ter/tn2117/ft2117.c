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
//#define CONST_CXD2820_I2CADDR	(0xD8>>1)
#define CONST_CXD2820_GWSUBADDR	(0x09)
#define CONST_FT2117_I2CADDR	(0xC0)

#define CONST_IF_FREQ	(36000) /* 36.166667MHz */
#define CONST_REF_FREQ	(166667)//(166.667) /* 166.667KHz */
//#define KHZ_TO_HZ	(1000)
#define SHIFT_CP	(3)

#define SHIFT_CP_ONLY	(6)

#define AGC_HIGH_CURRENT	(0x20)
#define Step_Size (0x04)
#define CHANNEL_SEARCH (0xA0)
#define CHANNEL_LOCK    (0x20)

//#define MONITOR_FT2117_STATUS
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
} FT2117_Reg_e;

typedef enum
{
	FT2117_CP0	= 0x7,
	FT2117_CP1	= 0xE,
	FT2117_CP2	= 0xF
} FT2117_ChargePump_e;

typedef enum
{
	FT2117_CP_ONLY0	= 0x0,
	FT2117_CP_ONLY1	= 0x1
} FT2117_Top_e;


typedef enum {
    DVB_SYSTEM_UNKNOWN,    /**< Unknown. */
    DVB_SYSTEM_DVBC,       /**< DVB-C. */
    DVB_SYSTEM_DVBT,       /**< DVB-T. */
    DVB_SYSTEM_DVBT2       /**< DVB-T2. */
} FT2117_DvbSystem_e;

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

static HUINT8 SetChargePumpCurrentBandSelect(HUINT32 ulRfFreqKhz)  // Digital Charge Pump setting
{
	HUINT8	ucChargePumpBand = 0;

	/* CP T2 T1 T0 setting */
	if(ulRfFreqKhz <= 230000) 	ucChargePumpBand = (FT2117_CP0<<SHIFT_CP);
	else if((ulRfFreqKhz > 468000) && (ulRfFreqKhz <= 500000)) ucChargePumpBand = (FT2117_CP0<<SHIFT_CP);
	else if((ulRfFreqKhz > 500000) && (ulRfFreqKhz <= 800000)) ucChargePumpBand = (FT2117_CP1<<SHIFT_CP);
	else if((ulRfFreqKhz > 800000) && (ulRfFreqKhz <= 862000)) ucChargePumpBand = (FT2117_CP2<<SHIFT_CP);

	return ucChargePumpBand;
}

static HUINT8 SetChargePumpOnly(HUINT32 ulRfFreqKhz)  // Digital Charge Pump setting
{
	HUINT8	ucChargePumpBand = 0x98 | Step_Size;

	/* CP T2 T1 T0 setting */
	if(ulRfFreqKhz <= 230000) 	ucChargePumpBand |= (FT2117_CP_ONLY0<<SHIFT_CP_ONLY);
	else if((ulRfFreqKhz > 468000) && (ulRfFreqKhz <= 500000)) ucChargePumpBand |= (FT2117_CP_ONLY0<<SHIFT_CP_ONLY);
	else if((ulRfFreqKhz > 500000) && (ulRfFreqKhz <= 800000)) ucChargePumpBand |= (FT2117_CP_ONLY1<<SHIFT_CP_ONLY);
	else if((ulRfFreqKhz > 800000) && (ulRfFreqKhz <= 862000)) ucChargePumpBand |= (FT2117_CP_ONLY1<<SHIFT_CP_ONLY);

	return ucChargePumpBand;
}


#if defined(CONFIG_PLATFORM_DS2820)
int FT2117_SetTune ( HUINT16 usDeviceAddr, HUINT32 ulRfFreqKhz, FT2117_DvbSystem_e etSystem, HUINT8 bandWidth )
{
	HUINT8 aucWriteDate[MAXBYTE];
	HUINT32 ulDivideCount;
	HUINT32 ulFreqOsc;
	double fDivider;
	int 	nRet = 0;
	HUINT16 usChipAddr = 0;
#if defined(MONITOR_FT2117_STATUS)
	int i;
#endif
	CH_UNUSED(etSystem);

	usChipAddr = (usDeviceAddr >> 1);
	ulFreqOsc = (ulRfFreqKhz + CONST_IF_FREQ);
	fDivider = ((ulFreqOsc*KHZ_TO_HZ) / CONST_REF_FREQ ) + 1;
	ulDivideCount = (HUINT32)fDivider;
#if defined(MONITOR_FT2117_STATUS)
	CH_PrintData(" &&&&&&&&&&&&&& ulRfFreqKhz = (%d)\n", ulRfFreqKhz);
	CH_PrintData(" &&&&&&&&&&&&&& fDivider = (%f)\n", fDivider);
	CH_PrintData(" &&&&&&&&&&&&&& ulDivideCount = (0x%x)\n", ulDivideCount);
	CH_PrintData(" &&&&&&&&&&&&&& usDeviceAddr = (0x%x)\n", usDeviceAddr);
#endif
	aucWriteDate[ADB] = CONST_FT2117_I2CADDR;

	aucWriteDate[DB1] = 0x7F & (ulDivideCount >> 8);
	aucWriteDate[DB2] = 0xFF & ulDivideCount;

	aucWriteDate[CB1] = 0x80|Step_Size;

	aucWriteDate[CB1]|=SetChargePumpCurrentBandSelect(ulRfFreqKhz);

	if(bandWidth==7)
		aucWriteDate[BB]=0x00;
	else
		aucWriteDate[BB]=0x10; //8M BW

	if(ulFreqOsc<474000)
		aucWriteDate[BB] |=0x02; //Mid Band
	else
		aucWriteDate[BB] |=0x04; //High Band

	aucWriteDate[CB2] = SetChargePumpOnly(ulRfFreqKhz);

#if defined( NORDIC_SSI_SQI )
	if(usDeviceAddr == 0xD8)
	{
		aucWriteDate[AB] = CHANNEL_SEARCH ;
	}
	else
	{
		aucWriteDate[AB] = 0xD0 ;
	}
#else
	aucWriteDate[AB] = CHANNEL_SEARCH ;
#endif

#if defined(MONITOR_FT2117_STATUS)
	for( i=0; i<MAXBYTE; i++)
	{
		CH_PrintData(" 0x%x: ", aucWriteDate[i]);
	}
	CH_PrintData("\n");
#endif
	nRet = DRV_I2c_WriteA8( CONST_CHANNEL_I2C_BUS, usChipAddr, CONST_CXD2820_GWSUBADDR,\
							aucWriteDate, MAXBYTE );
    if ( nRet != 0 )
    {
    	CH_PrintErr(" ");
        return -1;
    }

	VK_TASK_Sleep(70);

#if defined( NORDIC_SSI_SQI )
	if(usDeviceAddr == 0xD8)
	{
		aucWriteDate[AB] = CHANNEL_LOCK ;
	}
	else
	{
		aucWriteDate[AB] = 0x50 ;//ATC bit to '0'
	}
#else
	aucWriteDate[AB] = CHANNEL_LOCK ;
#endif


#if defined(MONITOR_FT2117_STATUS)
	for( i=0; i<MAXBYTE; i++)
	{
		CH_PrintData(" 0x%x: ", aucWriteDate[i]);
	}
	CH_PrintData("\n");
#endif
	nRet = DRV_I2c_WriteA8( CONST_CHANNEL_I2C_BUS, usChipAddr, CONST_CXD2820_GWSUBADDR,\
							aucWriteDate, MAXBYTE );
    if ( nRet != 0 )
    {
    	CH_PrintErr(" ");
        return -1;
    }

    return 0;

}
#endif //#if defined(CONFIG_PLATFORM_DS2820)



#if defined(CONFIG_PLATFORM_DP10048)

int FT2117_SetTune ( int nUnitId, HUINT32 ulRfFreqKhz, HUINT8 bandWidth )
{
	HUINT8 aucWriteDate[MAXBYTE];
	HUINT8 aucWriteDateTmp[MAXBYTE];
	HUINT32 ulDivideCount;
	HUINT32 ulFreqOsc;
	double fDivider;
	int 	nRet = 0;
#if defined(MONITOR_FT2117_STATUS)
	int i;
#endif

	ulFreqOsc = (ulRfFreqKhz + CONST_IF_FREQ);
	fDivider = ((ulFreqOsc*KHZ_TO_HZ) / CONST_REF_FREQ ) + 1;
	ulDivideCount = (HUINT32)fDivider + 1;
#if defined(MONITOR_FT2117_STATUS)
	CH_PrintData(" &&&&&&&&&&&&&& ulRfFreqKhz = (%d)\n", ulRfFreqKhz);
	CH_PrintData(" &&&&&&&&&&&&&& fDivider = (%f)\n", fDivider);
	CH_PrintData(" &&&&&&&&&&&&&& ulDivideCount = (0x%x)\n", ulDivideCount);
	CH_PrintData(" &&&&&&&&&&&&&& nUnitId = (%d)\n", nUnitId);
#endif
	aucWriteDate[ADB] = CONST_FT2117_I2CADDR;

	aucWriteDate[DB1] = 0x7F & (ulDivideCount >> 8);
	aucWriteDate[DB2] = 0xFF & ulDivideCount;

	aucWriteDate[CB1] = 0x80|Step_Size;

	aucWriteDate[CB1]|=SetChargePumpCurrentBandSelect(ulRfFreqKhz);

	if(bandWidth==7)
		aucWriteDate[BB]=0x00;
	else
		aucWriteDate[BB]=0x10; //8M BW

	if(ulFreqOsc<474000)
		aucWriteDate[BB] |=0x02; //Mid Band
	else
		aucWriteDate[BB] |=0x04; //High Band

	aucWriteDate[CB2] = SetChargePumpOnly(ulRfFreqKhz);

#if defined( NORDIC_SSI_SQI )
	if(usDeviceAddr == 0xD8)
	{
		aucWriteDate[AB] = CHANNEL_SEARCH ;
	}
	else
	{
		aucWriteDate[AB] = 0xD0 ;
	}
#else
	aucWriteDate[AB] = CHANNEL_SEARCH ;
#endif

#if defined(MONITOR_FT2117_STATUS)
	for( i=0; i<MAXBYTE; i++)
	{
		CH_PrintData(" 0x%x: ", aucWriteDate[i]);
	}
	CH_PrintData("\n");
#endif


	DRV_CH_EnableI2c( nUnitId );

	nRet = DRV_CH_WriteTn2117( nUnitId, &aucWriteDate[DB1], MAXBYTE-1);
    if ( nRet != 0 )
    {
    	CH_PrintErr(" ");
		DRV_CH_DisableI2c( nUnitId );
        return -1;
    }

	VK_TASK_Sleep(100);//TODO_ILEE

#if defined( NORDIC_SSI_SQI )
	if(usDeviceAddr == 0xD8)
	{
		aucWriteDate[AB] = CHANNEL_LOCK ;
	}
	else
	{
		aucWriteDate[AB] = 0x50 ;//ATC bit to '0'
	}
#else
	aucWriteDate[AB] = CHANNEL_LOCK ;
#endif


#if defined(MONITOR_FT2117_STATUS)
	for( i=0; i<MAXBYTE; i++)
	{
		CH_PrintData(" 0x%x: ", aucWriteDate[i]);
	}
	CH_PrintData("\n");
#endif

	nRet = DRV_CH_WriteTn2117( nUnitId, &aucWriteDate[DB1], MAXBYTE-1);
    if ( nRet != 0 )
    {
    	CH_PrintErr(" ");
		DRV_CH_DisableI2c( nUnitId );
        return -1;
    }

	DRV_CH_DisableI2c( nUnitId );
    return 0;

}

#endif //#if defined(CONFIG_PLATFORM_DP10048)

/*********************** End of File ******************************/
