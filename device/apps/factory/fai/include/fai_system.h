/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application Library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FAI_SYSTEM_H_
#define _FAI_SYSTEM_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fai_err.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FAI_SYSTEM_TMD_ERR_1=0,
	FAI_SYSTEM_TMD_ERR_2,
	FAI_SYSTEM_TMD_ERR_3,
	FAI_SYSTEM_TMD_ERR_MAX
} FAI_SYSTEM_TMD_ERR;

typedef enum
{
	FAI_SYSTEM_SYSTEM_ID = 0,
	FAI_SYSTEM_LOADER,
	FAI_SYSTEM_FACTORY_LOADER,
	FAI_SYSTEM_APPLICATION,
	FAI_SYSTEM_MICOM,
	FAI_SYSTEM_MICOM_MODE,
	FAI_SYSTEM_MODEL_NAME,
	FAI_SYSTEM_ATV_APP,
	FAI_SYSTEM_ATV_LOADER,
	FAI_SYSTEM_OTA,
	FAI_SYSTEM_CHIP_INFO,
	FAI_SYSTEM_SYSTEM,
	FAI_SYSTEM_KERNEL,
	FAI_SYSTEM_BOOTLOADER,
	FAI_SYSTEM_UPDATE_DATE,
	FAI_SYSTEM_MOCA_FW
} FAI_SYSTEM_INFO;

typedef enum
{
	FAI_SYSTEM_CHIP_ID = 0,
	FAI_SYSTEM_RECEIVER_NO,
	FAI_SYSTEM_SERIAL_NO,
	FAI_SYSTEM_MAC,
	FAI_SYSTEM_HDCP,
	FAI_SYSTEM_DTCP,
	FAI_SYSTEM_PARING_DATA,
	FAI_SYSTEM_NDS_SVP,
	FAI_SYSTEM_CERTIFICATION,
	FAI_SYSTEM_MAC2,
	FAI_SYSTEM_DIRECTV_SN,
	FAI_SYSTEM_MANUFACTURE_DATE,
	FAI_SYSTEM_MAC3,
	FAI_SYSTEM_MAC4, /* CABLE MODEM MAC */
	FAI_SYSTEM_EXT_MAC1,/* Wi-Fi */
	FAI_SYSTEM_EXT_MAC2,
	FAI_SYSTEM_NDS_CAID,
	FAI_SYSTEM_EXT_DOWN_MAC1,
	FAI_SYSTEM_EXT_DOWN_MAC2,
	FAI_SYSTEM_RF4CE_MAC,
	FAI_SYSTEM_ESN_1,
	FAI_SYSTEM_ESN_2,
	FAI_SYSTEM_HOST_ID,
	FAI_SYSTEM_MAC5,
	FAI_SYSTEM_MAC6,
	FAI_SYSTEM_NAGRA_CAID,
	FAI_SYSTEM_EXT_MAC3,
	FAI_SYSTEM_EXT_MAC4,
	FAI_SYSTEM_MAC7,
	FAI_SYSTEM_MAC8,
	FAI_SYSTEM_CM_MAC,
	FAI_STYTEM_IRDETO_PRIVATEDATA = 0x1f,/*IRDETO PRIVATE DATA*/
	FAI_SYSTEM_WIFI_LOCAL,
	FAI_SYSTEM_WIFI_MAC2,
	FAI_SYSTEM_WIFIMAC1=0x30,
} FAI_SYSTEM_UPLOAD_INFO;


typedef enum
{
	FAI_FACTORY_DEFAULT_TMD_ERR_MES_DOWNLOAD	= 0,
	FAI_FACTORY_DEFAULT_TMD_ERR_EEPROM_WRITE,		
	FAI_FACTORY_DEFAULT_TMD_ERR_CAM_PAIR_DATA,		
	FAI_FACTORY_DEFAULT_TMD_ERR_BSL_WRITE,
	FAI_FACTORY_DEFAULT_TMD_ERR_SECURE_BOOT,
	FAI_FACTORY_DEFAULT_TMD_ERR_JTAG,		
	FAI_FACTORY_DEFAULT_TMD_ERR_FLASH_OTP,
	FAI_FACTORY_DEFAULT_TMD_ERR_FAC_SW_ERASE,
	FAI_FACTORY_DEFAULT_TMD_ERR_CW_MODE,
	FAI_FACTORY_DEFAULT_TMD_ERR_DEFAULT_DATA_WRITE,
	FAI_FACTORY_DEFAULT_TMD_ERR_FAC_LDR_ERASE,
	FAI_FACTORY_DEFAULT_TMD_ERR_CHIP_OTP,
	FAI_FACTORY_DEFAULT_TMD_ERR_NO_DEFAULT_FILE,
	FAI_FACTORY_DEFAULT_TMD_ERR_SYS_ID,
	FAI_FACTORY_DEFAULT_TMD_ERR_CCB_ENCRYPT,
	FAI_FACTORY_DEFAULT_TMD_ERR_MAKE_SPCB,
	FAI_FACTORY_DEFAULT_TMD_ERR_MARKET_ID
}FAI_FACTORY_DEFAULT_TMD_ERR;

typedef enum
{
	FAI_ETC_TMD_ERR_INVALID_SHEET					= 0,
	FAI_ETC_TMD_ERR_
}FAI_ETC_TMD_ERR;

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
FAI_ERR_CODE FAI_SYSTEM_Init(void);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_WIFI_KO_Module_Load(void);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_DefaultSetting(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_GetSWVersion(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_GetUploadInfo(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_GetSecurityInfo(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

FAI_ERR_CODE FAI_SYSTEM_InformationOff(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);
/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/

FAI_ERR_CODE FAI_SYSTEM_SWReset(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_PowerCtrl(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_DTCPCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_ChangeSheet(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_DisplayInfoMessageBox(void);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_GetTemperature(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_CheckRf4ceRssi(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_DisplayInfo_UI(void);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_DefaultSetting_UI(void);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_SWReset_UI(void);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_PowerCtrl_UI(void);

/*******************************************************************************
* function : FAI_SYSTEM_DTCPCheck_UI
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_DTCPCheck_UI(void);

/*******************************************************************************
* function : FAI_SYSTEM_CiPlusCheck_UI
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SYSTEM_CiPlusCheck_UI(void);


void FAI_SYSTEM_MountDBandDBBACKUP(int *mounted1, int *mounted2, int *mounted3);
void FAI_SYSTEM_UnMountDBandDBBACKUP(void);
void FAI_SYSTEM_UBIFSInit(void);

#endif

