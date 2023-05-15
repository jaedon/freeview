
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_ftp.h
// Original Author: Full Name!! : Jason Han
// File Description: FTP SW upgrade
// Remarks:

/*******************************************************************/
/* Copyright (c) 2011 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __DI_FTP_H__
#define __DI_FTP_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
// Start Including Header Files
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
#define FTP_MAC_ADDR_LENGTH		6
#define FTP_ETH_ADDR_LENGTH		4
#define FTP_USER_LENGTH 		20
#define FTP_PASSWD_LENGTH 		20
#define FTP_DIR_PATH_LENGTH 	200
#define FTP_FILE_NAME_LENGTH 	50
// End Macro definition


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef
typedef struct _ftp_strct_
{
	HINT32 		ulCmdSocket;
	HINT32 		ulDataSocket;

	HUINT8		aServerIp[FTP_ETH_ADDR_LENGTH];
	HUINT16		usPortNum;
	HUINT16		usPasvPortNum;

	HUINT8 		aUserName[FTP_USER_LENGTH];
	HUINT8 		aPasswd[FTP_PASSWD_LENGTH];
	HUINT8 		aDirPath[FTP_DIR_PATH_LENGTH];
	HUINT8 		aFileName[FTP_FILE_NAME_LENGTH];
	HUINT32 	ulFileSize;
} FTP_INFO, *pFTP_INFO;

typedef struct tagDiFtpOtaSetting
{
	HUINT8		aServerIp[FTP_ETH_ADDR_LENGTH];
	HUINT8 		aUserName[FTP_USER_LENGTH];
	HUINT8 		aPasswd[FTP_PASSWD_LENGTH];
	HUINT8 		aDirPath[FTP_DIR_PATH_LENGTH];
	HUINT8 		aFileName[FTP_FILE_NAME_LENGTH];

} DI_FTP_OTA_SETTING_T;

// End typedef


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
// Start global function prototypes
DI_ERR_CODE DI_FTP_init(void);
DI_ERR_CODE DI_FTP_open(DI_FTP_OTA_SETTING_T *pOtaInfo);
HUINT32 DI_FTP_read(HUINT8 *buf, HUINT32 len);
HUINT32 DI_FTP_write(void);
DI_ERR_CODE DI_FTP_seek(void);
DI_ERR_CODE DI_FTP_close(void);
DI_ERR_CODE DI_FTP_uninit(void);
DI_ERR_CODE DI_FTP_setFtpOtaPath(HUINT8 *pOtaInfo);
DI_ERR_CODE DI_FTP_getFtpOtaPath(DI_FTP_OTA_SETTING_T *pszOtaPath);
// End global function prototypes

#ifdef __cplusplus
}
#endif

#endif /* __DI_ETH_H__ */


