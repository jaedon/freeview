/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_sysinfo_stb.h
	@brief

	Description:  									\n
	Module: OAPI / API					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding ?» »ç¿ëÇÏ´Â source file ?Ô´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef __OAPI_SYSINFO_STB_H__
#define __OAPI_SYSINFO_STB_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	RPC_OAPI_SYSINFO_STB_Init					"oapi.sysinfo.stb.Init"
#define	RPC_OAPI_SYSINFO_STB_Refresh				"oapi.sysinfo.stb.Refresh"

#define	RPC_OAPI_SYSINFO_SetFirstBootOnProduction	"oapi.sysinfo.SetFirstBootOnProduction"
#define	RPC_OAPI_SYSINFO_SetAntennaPower			"oapi.sysinfo.SetAntennaPower"
#define	RPC_OAPI_SYSINFO_SetSeriveUpdateFlag		"oapi.sysinfo.SetSeriveUpdateFlag"

#define	RPC_OBAMA_SYSINFO_STB_onStatusChanged	"obama.sysinfo.stb.onStatusChanged"

#define	RPC_OAPI_SYSINFO_GetCPUTemperature				"oapi.sysinfo.GetCPUTemperature"

#if defined(CONFIG_NETFLIX_APP)
#define RPC_OAPI_SYSINFO_SetNetflixESN              "oapi.sysinfo.SetNetflixESN"

#define	OAPI_SYSINFO_ESN_LEN				64
#endif


#define	OAPI_SYSINFO_NAME_LEN				32
#define	OAPI_SYSINFO_UPDATEDLIST_LEN		1024
#define	OAPI_SYSINFO_DTCP_IP_KEY_LEN		512

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HCHAR	szModelName[OAPI_SYSINFO_NAME_LEN];
	HCHAR	szSoftwareVersion[OAPI_SYSINFO_NAME_LEN];
	HCHAR	szLoaderName[OAPI_SYSINFO_NAME_LEN];
	HCHAR	szLoaderVersion[OAPI_SYSINFO_NAME_LEN];
	HCHAR	szUpgraderVersion[OAPI_SYSINFO_NAME_LEN];
	HCHAR	szManufacturerName[OAPI_SYSINFO_NAME_LEN];
	HCHAR	szSystemId[OAPI_SYSINFO_NAME_LEN];
	HCHAR	szDeviceId[OAPI_SYSINFO_NAME_LEN];
	HCHAR	szChipId[OAPI_SYSINFO_NAME_LEN];
	HCHAR	szObamaVersion[OAPI_SYSINFO_NAME_LEN];
	HCHAR	szMicomVersion[OAPI_SYSINFO_NAME_LEN];
	HCHAR	szUpdateDate[OAPI_SYSINFO_NAME_LEN];
	HCHAR	szSerialNumber[OAPI_SYSINFO_NAME_LEN];
	HCHAR	szUpdatedList[OAPI_SYSINFO_UPDATEDLIST_LEN];
	HINT8	szDtcpIpKey[OAPI_SYSINFO_DTCP_IP_KEY_LEN];
	HCHAR	szPackageId[OAPI_SYSINFO_NAME_LEN];
#if defined(CONFIG_NETFLIX_APP)
    HCHAR	szNetflixESN[OAPI_SYSINFO_ESN_LEN];
#endif

	HUINT32	ulDtcpIpKeySize;

	HUINT32	ulRcuCustomCode;
	HBOOL	bColdBootDone;

	HBOOL	bFirstBootOnProduction;
	HUINT32	ulLaunchAppMode;
	HBOOL	bAntnnaPower;
	HBOOL	bSvcUpdateFlag;			// MENU���� Dynamic update flagó����

	HUINT8	ucSCS;
	HUINT8	ucJTAG;
	HUINT8	ucOTP;
	HUINT8	ucCWE;
	HUINT32 ulMarketID;
	HUINT32	ulOTPField1;
	HUINT32	ulOTPField2;

	HUINT32	ulNandBadBlockCntLdr;
	HUINT32	ulNandBadBlockCntUBI;
	HUINT32	ulNandBadBlockCntUBIFS;

	HUINT32 ulCPUTemperature;		// Current
	HUINT32 ulMaxCPUTemperature;	// Maximum
} OxSysInfoStb_Info_t;

typedef void (* OxSysInfoStb_Notifier_t)(void);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
HERROR	OAPI_SYSINFO_STB_Init(OxSysInfoStb_Notifier_t fnNotifier);
HERROR	OAPI_SYSINFO_STB_DeInit(void);
HERROR	OAPI_SYSINFO_Initialized(void);
HERROR	OAPI_SYSINFO_STB_Refresh(HBOOL bSync);
HERROR	OAPI_SYSINFO_STB_GetModelName(HCHAR *pszModelName, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetUpdatedList(HCHAR *pszUpdatedList, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetSoftwareVersion(HCHAR *pszSoftwareVersion, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetPackageVersion(HCHAR *pszPackageVersion, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetLoaderName(HCHAR *pszLoaderName, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetLoaderVersion(HCHAR *pszLoaderVersion, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetUpgraderVersion(HCHAR *pszVersion, HINT32 nBufSize);
HERROR	OAPI_SYSINFO_STB_GetManufacturerName(HCHAR *pszManufacturerName, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetSystemId(HCHAR *pszSystemId, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetDeviceId(HCHAR *pszDeviceId, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetChipId(HCHAR *pszChipId, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetObamaVersion(HCHAR *pszObamaVersion, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetMicomVersion(HCHAR *pszMicomVersion, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetUpdateDate(HCHAR *pszUpdateDate, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetSerialNo(HCHAR *pszSerialNo, HINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetCustomCode(HUINT32 *pulCustomCode);
HERROR 	OAPI_SYSINFO_STB_GetColdBootDone(HBOOL *pbColdBootDone);
HERROR 	OAPI_SYSINFO_STB_SetFirstBootOnProduction(HBOOL bFirstBootOnProduction);
HERROR 	OAPI_SYSINFO_STB_GetFirstBootOnProduction(HBOOL *pbFirstBootOnProduction);
HERROR 	OAPI_SYSINFO_STB_GetLaunchAppMode(HUINT32 *pulLaunchAppMode);
HERROR 	OAPI_SYSINFO_STB_SetAntennaPower(HBOOL bAntPower);
HERROR 	OAPI_SYSINFO_STB_GetAntennaPower(HBOOL *pbAntPower);
HERROR 	OAPI_SYSINFO_STB_SetServiceUpdateFlag(HBOOL bOnOff);
HERROR 	OAPI_SYSINFO_STB_GetServiceUpdateFlag(HBOOL *pbOnOff);
HERROR 	OAPI_SYSINFO_STB_GetNandBadBlocks(HUINT32 *pulCntLdr, HUINT32 *pulCntUBI, HUINT32 *pulCntUBIFS);
HERROR 	OAPI_SYSINFO_STB_GetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT32 *pulMarketID);
HERROR 	OAPI_SYSINFO_STB_GetFusingInfoExt(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT32 *pulOTPField1, HUINT32 *pulOTPField2);
HERROR	OAPI_SYSINFO_STB_GetDtcpIpKeySize(HUINT32 * pnSize);
HERROR	OAPI_SYSINFO_STB_GetDtcpIpKey(HINT8 *pszDtcpIpKey, HUINT32 nMax);
HERROR	OAPI_SYSINFO_STB_GetCPUTemperature(HUINT32 *pulCurTemp, HUINT32 *pulMaxTemp);

#if defined(CONFIG_NETFLIX_APP)
// for Netflix
HERROR	OAPI_SYSINFO_STB_SetNetflixESN(HCHAR *pszESN);
HERROR	OAPI_SYSINFO_STB_GetNetflixESN(HCHAR *pszESN, HINT32 nMax);
#endif

#endif // __OAPI_SYSINFO_STB_H__

