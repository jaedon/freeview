/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_TMD_H_
#define _FAI_TMD_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
/* category list */
#define CATEGORY_CHANNEL	0x01
#define CATEGORY_AVOUTPUT	0x02
#define CATEGORY_FRONT_PANEL	0x04
#define CATEGORY_MEMORY	0x08
#define CATEGORY_PERIPHERAL	0x10
#define CATEGORY_SYSTEM	0x20

/* module list of CHANNEL category */
#define MODULE_CHANNEL_SATELLITE	0x01
#define MODULE_CHANNEL_TERRESTRIAL	0x02
#define MODULE_CHANNEL_CABLE	0x03
#define MODULE_CHANNEL_AV_PLAY	0x04
#define MODULE_CHANNEL_MOCA	0x05

/* module list of AV OUTPUT category */
#define MODULE_AVOUTPUT_COLORBAR	0x01
#define MODULE_AVOUTPUT_SCART	0x02
#define MODULE_AVOUTPUT_SVIDEO	0x03
#define MODULE_AVOUTPUT_COMPOSITE	0x04	
#define MODULE_AVOUTPUT_COMPNENT	0x05
#define MODULE_AVOUTPUT_RF	0x06
#define MODULE_AVOUTPUT_SPDIF	0x07
#define MODULE_AVOUTPUT_DCONNECTOR	0x08
#define MODULE_AVOUTPUT_HDMI	0x09
#define MODULE_AVOUTPUT_HW_AUDIOMUTE	0x0A
#define MODULE_AVOUTPUT_AC3_CONTROL	0x0B

/* module list of FRONT PANEL category */
#define MODULE_FRONT_PANEL_KEY	0x01
#define MODULE_FRONT_PANEL_LED	0x02
#define MODULE_FRONT_PANEL_7SEG	0x03
#define MODULE_FRONT_PANEL_LCD	0x04
#define MODULE_FRONT_PANEL_IRB	0x05

/* module list of MEMORY category */
#define MODULE_MEMORY_FLASH	0x01
#define MODULE_MEMORY_EEPROM	0x02
#define MODULE_MEMORY_HDD	0x03

/* module list of PERIPHERAL category */
#define MODULE_PERIPHERAL_ETHERNET	0x01
#define MODULE_PERIPHERAL_MODEM	0x02
#define MODULE_PERIPHERAL_SMARTCARD	0x03
#define MODULE_PERIPHERAL_USB	0x04	
#define MODULE_PERIPHERAL_FAN	0x05
#define MODULE_PERIPHERAL_RFRCU	0x06
#define MODULE_PERIPHERAL_TVLINK	0x07
#define MODULE_PERIPHERAL_TEMPSENSOR	0x08
#define MODULE_PERIPHERAL_1394	0x09
#define MODULE_PERIPHERAL_CABLEMODEM	0x0A
#define MODULE_PERIPHERAL_SERIALCOMM	0x0B
#define MODULE_PERIPHERAL_RTC	0x0C
#define MODULE_PERIPHERAL_DOOR_SENSOR	0x0D
#define MODULE_PERIPHERAL_MIC	0x0E
#define MODULE_PERIPHERAL_BLUETOOTH	0x0F
#define MODULE_PERIPHERAL_DEF_DETECTION	0x10
#define MODULE_PERIPHERAL_CPUTYPE	0x11
#define MODULE_PERIPHERAL_MPU_DN_PATH	0x12

/* module list of SYSTEM category */
#define MODULE_SYSTEM_MES_ENTER	0x01
#define MODULE_SYSTEM_FAC_DEF	0x02
#define MODULE_SYSTEM_INFO	0x03
#define MODULE_SYSTEM_POWER	0x04	
#define MODULE_SYSTEM_ETC	0x05

typedef enum
{
	FAI_MODULE_TMD_ERR_COMMON 			= 0x00,
	FAI_MODULE_TMD_ERR_CHANNEL 			= 0x01,
	FAI_MODULE_TMD_ERR_AVPLAY			= 0x02,
	FAI_MODULE_TMD_ERR_MOCA				= 0x03,
	FAI_MODULE_TMD_ERR_COLORBAR_BEEP	= 0x04,
	FAI_MODULE_TMD_ERR_SCART			= 0x05,
	FAI_MODULE_TMD_ERR_SVIDEO			= 0x06,
	FAI_MODULE_TMD_ERR_COMPOSITE		= 0x07,
	FAI_MODULE_TMD_ERR_RF				= 0x08,
	FAI_MODULE_TMD_ERR_DCONNECTOR		= 0x09,
	FAI_MODULE_TMD_ERR_HDMI				= 0x0A,
	FAI_MODULE_TMD_ERR_HWAUDIOMUTE		= 0x0B,
	FAI_MODULE_TMD_ERR_KEY				= 0x0C,
	FAI_MODULE_TMD_ERR_LED				= 0x0D,
	FAI_MODULE_TMD_ERR_LCDVFDSEG		= 0x0E,
	FAI_MODULE_TMD_ERR_IRSENSOR_BLASTER	= 0x0F,
	FAI_MODULE_TMD_ERR_FLASH			= 0x10,
	FAI_MODULE_TMD_ERR_EEPROM			= 0x11,
	FAI_MODULE_TMD_ERR_HDD				= 0x12,
	FAI_MODULE_TMD_ERR_DDR				= 0x13,
	FAI_MODULE_TMD_ERR_ETHERNET			= 0x14,
	FAI_MODULE_TMD_ERR_MODEM			= 0x15,
	FAI_MODULE_TMD_ERR_SMARTCARD		= 0x16,
	FAI_MODULE_TMD_ERR_USB				= 0x17,
	FAI_MODULE_TMD_ERR_FAN				= 0x18,
	FAI_MODULE_TMD_ERR_RFRCU			= 0x19,
	FAI_MODULE_TMD_ERR_TEMPERATURESENSOR	= 0x1A,
	FAI_MODULE_TMD_ERR_CABLEMODEM		= 0x1B,
	FAI_MODULE_TMD_ERR_MICTEST			= 0x1C,
	FAI_MODULE_TMD_ERR_DEFAULTPINSTATUS	= 0x1D,
	FAI_MODULE_TMD_ERR_CPUTYPE			= 0x1E,
	FAI_MODULE_TMD_ERR_MICOM			= 0x1F,
	FAI_MODULE_TMD_ERR_MES				= 0x20,
	FAI_MODULE_TMD_ERR_FACTORYDEFAULT	= 0x21,
	FAI_MODULE_TMD_ERR_SYSTEMINFORMATION	= 0x22,
	FAI_MODULE_TMD_ERR_POWER			= 0x23,
	FAI_MODULE_TMD_ERR_ETC				= 0x24,
	FAI_MODULE_TMD_ERR_WIFI				= 0x25,	
	FAI_MODULE_TMD_ERR_	
}FAI_MODULE_TMD_ERR;

typedef enum 
{ 
	FAI_CATEGORY_CHANNEL 		= 0x01, 
	FAI_CATEGORY_AV_OUTPUT 		= 0x02,
	FAI_CATEGORY_FRONT_PANEL	= 0x04,
	FAI_CATEGORY_MEMORY			= 0x08,
	FAI_CATEGORY_PERIPHERAL		= 0x10,
	FAI_CATEGORY_SYSTEM			= 0x20
} FAI_CATEGORY;


typedef enum
{
	FAI_TMD_TEST_NO_ERROR = 0, 
	FAI_TMD_TEST_PENDING = 0xAA,
	FAI_TMD_TEST_ERROR = 0xFF
} FAI_TMD_TEST_STATE;

#define FAI_TMD_MAX_PARAM_LENGTH	255

typedef enum
{
	FAI_TMD_ERR_OK = 0,
	FAI_TMD_ERR_FAIL	
} FAI_TMD_ERR;

/* error code define for TMD */
#define FAI_TMD_DEFAULT_ERROR_CODE	FAI_TMD_ERR_FAIL
#define FAI_TMD_ERROR_PARM_LENGTH	3

typedef struct
{
	unsigned char ucCategory;
	unsigned char ucModule;
	unsigned char ucAction;
	FAI_TMD_ERR   eResult;
	unsigned char ucNumOfParam;
	unsigned short usMsgId;
	unsigned char aucParams[FAI_TMD_MAX_PARAM_LENGTH];
} FAI_TMD_RESULT_T;

/******************************************************************************
* Global variables and structures
******************************************************************************/

/******************************************************************************
* External variables and functions
******************************************************************************/

/******************************************************************************
* Static variables and structures
******************************************************************************/


/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/*******************************************************************************
* function : FAI_TMD_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_TMD_Init();

/*******************************************************************************
* function : FAI_TMD_GetParam
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned char FAI_TMD_GetParam(unsigned char *pucParamBuf, unsigned char *pucArgList, int numOfParams);

/*******************************************************************************
* function : FAI_TMD_AddResultParam
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_TMD_AddResultParam(FAI_TMD_RESULT_T *ptResult, void *pvParam, unsigned char ucLen);

/*******************************************************************************
* function : FAI_TMD_SetResult
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_TMD_SetResult(FAI_TMD_RESULT_T *ptResult);

/*******************************************************************************
* function : FAI_TMD_ArgToValue
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_TMD_ArgToValue(unsigned long *pulValue, unsigned char *pucArg, int nArgSize);

#endif /* fai_tesla.h */
