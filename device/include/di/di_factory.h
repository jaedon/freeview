/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_factory.h
// Original Author: hkyang
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/
#ifndef __DI_FACTORY_DEF__
#define __DI_FACTORY_DEF__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
// Start Including Header Files
#include "htype.h"
#include "di_err.h"
// End Including Headers

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
// Start Extern variablee

// End Extern variable


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
// Start Macro definition


// End Macro definition


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef
typedef struct
{
	HINT8	pcLoaderVersion[20];
	HINT8	pcMcuLoaderVersion[20];
	HINT8	pcMcuVersion[20];
	HINT8	pcChipInfo[20];
	HINT8	pcModelName[20];
} DI_FACTORY_VERSION_INFO_t;


/** @brief Factory ÀÇ Range ±¸Á¶. */
typedef enum
{
    DI_FAC_PATTERN = 0,
    DI_FAC_DYNAMICCONTRAST,
    DI_FAC_SPAMPLITUDE,
    DI_FAC_SPPERIOD,
    DI_FAC_LVDSCURRENT,
    DI_FAC_NUM_ADJ
} DI_FACTORY_Items;


typedef enum
{
    DI_FAC_DOWNLOAD_SUCCESS = 0,
    DI_FAC_DOWNLOAD_FAIL,
    DI_FAC_DOWNLOAD_TIMEOUT,
    DI_FAC_DOWNLOAD_SIGNAL_CHECKED,
    DI_FAC_DOWNLOAD_RESULT_MAX
} DI_FACTORY_DOWNLOAD_RESULT_t;


typedef enum
{
	DI_FACTORY_TEST_NO_ERROR = 0,
	DI_FACTORY_TEST_PENDING = 0xAA,
	DI_FACTORY_TEST_ERROR = 0xFF
} DI_FACTORY_TEST_STATE;

typedef enum
{
	DI_EDID_VGA = 0,
	DI_EDID_HDMI_ALL,
	DI_EDID_HDMI_1,
	DI_EDID_HDMI_2,
	DI_EDID_HDMI_3,
	DI_EDID_HDMI_4,
	DI_EDID_HDMI_5
} DI_EDID_Type_t;


typedef DI_ERR_CODE	(*pfnDI_FACTORY_DOWNLOAD_Notify)(DI_FACTORY_DOWNLOAD_RESULT_t etResult);
typedef void (*pfnDI_FACTORY_Notify)(unsigned int ulCommand, unsigned char *msg_data , unsigned char msg_length);

// End typedef

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
// Start global function prototypes
DI_ERR_CODE DRV_FACTORY_Init (void);
DI_ERR_CODE DI_FACTORY_Start(void);
DI_ERR_CODE DI_FACTORY_SetPattern (HBOOL bEnable, HUINT8 ucValue);
DI_ERR_CODE DI_FACTORY_SetBackGNDColorTestPattern ( HBOOL bRed, HBOOL bGreen, HBOOL bBlue );
DI_ERR_CODE DI_FACTORY_SetParameticEQ (HBOOL bOnOff);
DI_ERR_CODE DI_FACTORY_SetWatchDog (HBOOL bOnOff);
DI_ERR_CODE DI_FACTORY_SetDynamicContrast (HUINT16 wValue);

DI_ERR_CODE DI_FACTORY_SetSPAmplitude (HUINT8 ucValue);
DI_ERR_CODE DI_FACTORY_SetSPPeriod(HUINT8 ucValue);
DI_ERR_CODE DI_FACTORY_GetSPAmplitude (HUINT8 *ucValue);
DI_ERR_CODE DI_FACTORY_GetSPPeriod (HUINT8 *ucValue);
DI_ERR_CODE DI_FACTORY_SetSpreadSpectrum (HUINT8 ucValue);

DI_ERR_CODE DI_FACTORY_SetLvdsCurrentControl (HUINT8 ucValue);
DI_ERR_CODE DI_FACTORY_GetLvdsCurrentControl (HUINT8 *ucValue);

DI_ERR_CODE DI_FACTORY_GetVersionInfo (DI_FACTORY_VERSION_INFO_t *pstVersion);
DI_ERR_CODE DI_FACTORY_DefaultSet (void);

DI_ERR_CODE DI_FACTORY_SetCiLicenseDownload (HUINT8 ucValue);
DI_ERR_CODE DI_FACTORY_RegisterDownloadNotify (pfnDI_FACTORY_DOWNLOAD_Notify fnFacNotify);
DI_ERR_CODE DI_FACTORY_RegisterCallBack(pfnDI_FACTORY_Notify pfnNotify);
DI_ERR_CODE DI_FACTORY_CommNotify(unsigned int ulCommand, unsigned char *pucData, unsigned char ucDataSize);
DI_ERR_CODE DI_FACTORY_SetResultValue(unsigned int ulCommand,
										unsigned char ucState,
										unsigned char ucNumberOfParam,
										unsigned char *pucParam1,
										unsigned char ucParam1Size,
										unsigned int pucParam2,
										unsigned int pucParam3,
										unsigned int pucParam4);

DI_ERR_CODE DI_FACTORY_EDID_Verify(HUINT8 *pModelName, HUINT32 nLength, DI_EDID_Type_t edidType);

// End global function prototypes
#endif

