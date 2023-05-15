/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_WIFI_H_
#define _FAI_WIFI_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FAI_WIFI_TMD_ERR_REQUEST = 0,
	FAI_WIFI_TMD_ERR_CONNECT,
	FAI_WIFI_TMD_ERR_FILE_OPEN,
	FAI_WIFI_TMD_ERR_MEMORY,
	FAI_WIFI_TMD_ERR_FILE_READ,
	FAI_WIFI_TMD_ERR_RECEIVE_DATA,
	FAI_WIFI_TMD_ERR_RSSI_READ,
	FAI_WIFI_TMD_ERR_SSID_SCAN,
	FAI_WIFI_TMD_ERR_CONNECT_TIMEOUT,	
}FAI_WIFI_TMD_ERR;

#define WIFI_READ_SAMPLE	10
/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */


/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_WIFI_Init(void);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_WIFI_ThroughputSetup(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);
FAI_ERR_CODE FAI_WIFI_ThroughputTest(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);
FAI_ERR_CODE FAI_WIFI_RSSI(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);
FAI_ERR_CODE FAI_WIFI_SSID(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);
FAI_ERR_CODE FAI_WIFI_RSSITest_UI(void);
#if(PRODUCT_YSR2000==YES)
FAI_ERR_CODE FAI_WIFI_Bridge_Setup_For_Conducted_Test_UI(void);
#endif

#if ( WIFI_MODULE_TYPE == WIFI_MODULE_QT3490)
FAI_ERR_CODE FAI_WIFI_QT3940_AssocSSID(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);
FAI_ERR_CODE FAI_WIFI_QT3940_ReadRSSI(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);
FAI_ERR_CODE FAI_WIFI_QT3940_RSSITest_UI(void);
#endif

#endif



