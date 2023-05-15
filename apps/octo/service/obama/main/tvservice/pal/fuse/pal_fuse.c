/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_fuse.c
	@brief	  Octo pal system information

	Description: Product 정보를 제공하는 PAL		\n
	Module: PAL/SYS			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>

#include <di_err.h>
#include <di_system.h>

#include <pal_fuse.h>

/*******************************************************************/
/************************     Definition   * ***********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


HERROR PAL_FUSE_GetInfo(Sys_FusingInfo_t *pstFusingInfo)
{
	DI_SYS_FUSING_INFO_t 	stFusingInfo;
	DI_ERR_CODE				eDiError;

	HxSTD_MemSet(&stFusingInfo, 0, sizeof(Sys_FusingInfo_t));

	eDiError = DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO, (void *)&stFusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
	if ( eDiError != DI_ERR_OK )
		return ERR_FAIL;

	pstFusingInfo->ucSCS 			= stFusingInfo.ucSCS;
	pstFusingInfo->ucJTAG 			= stFusingInfo.ucJTAG;
	pstFusingInfo->ucOTP			= stFusingInfo.ucOTP;
	pstFusingInfo->ucCWE			= stFusingInfo.ucCWE;
	pstFusingInfo->ulMarketID		= stFusingInfo.ulMarketID;
	pstFusingInfo->ucRAMSCR			= stFusingInfo.ucRAMSCR;
	pstFusingInfo->ucMarketID		= stFusingInfo.ucMarketID;
	pstFusingInfo->ulParams[0]		= stFusingInfo.ulParms[0];
	pstFusingInfo->ulParams[1]		= stFusingInfo.ulParms[1];
	pstFusingInfo->ulParams[2]		= stFusingInfo.ulParms[2];
	pstFusingInfo->ulParams[3]		= stFusingInfo.ulParms[3];
	pstFusingInfo->usSTBOwnerID		= stFusingInfo.usSTBOwnerID;
//	pstFusingInfo->ucRemuxFusing	= stFusingInfo.ucRemuxFusing;

	return ERR_OK;
}

HERROR PAL_FUSE_GetInfoExt(Sys_FusingInfo_t *pstFusingInfo)
{
	DI_SYS_FUSING_INFO_t 	stFusingInfo;
	DI_ERR_CODE				eDiError;

	HxSTD_MemSet(&stFusingInfo, 0, sizeof(Sys_FusingInfo_t));

	eDiError = DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO, (void *)&stFusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
	if ( eDiError != DI_ERR_OK )
		return ERR_FAIL;

	pstFusingInfo->ucSCS 			= stFusingInfo.ucSCS;
	pstFusingInfo->ucJTAG 			= stFusingInfo.ucJTAG;
	pstFusingInfo->ucOTP			= stFusingInfo.ucOTP;
	pstFusingInfo->ucCWE			= stFusingInfo.ucCWE;
	pstFusingInfo->ucRAMSCR			= stFusingInfo.ucRAMSCR;
	pstFusingInfo->ucMarketID		= stFusingInfo.ucMarketID;
	pstFusingInfo->ulParams[0]		= stFusingInfo.ulParms[0];
	pstFusingInfo->ulParams[1]		= stFusingInfo.ulParms[1];
	pstFusingInfo->ulParams[2]		= stFusingInfo.ulParms[2];
	pstFusingInfo->ulParams[3]		= stFusingInfo.ulParms[3];
	pstFusingInfo->usSTBOwnerID		= stFusingInfo.usSTBOwnerID;
	pstFusingInfo->ulOTPField1		= 0x0;
	pstFusingInfo->ulOTPField2		= 0x0;
	return ERR_OK;
}

HERROR PAL_FUSE_SetSecureBootInfo(HUINT8 ucSB)
{
	DI_SYS_FUSING_INFO_t 	stFusingInfo;
	DI_ERR_CODE				eDiError;

	HxSTD_MemSet(&stFusingInfo, 0, sizeof(Sys_FusingInfo_t));

	stFusingInfo.ucSCS		= ucSB;
	stFusingInfo.ucJTAG		= FALSE;
	stFusingInfo.ucOTP		= FALSE;
	stFusingInfo.ucCWE		= FALSE;
	stFusingInfo.ucRAMSCR	= FALSE;

	eDiError = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, (void *)&stFusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
	if ( eDiError != DI_ERR_OK )
		return ERR_FAIL;

	return (eDiError != ERR_OK) ? ERR_FAIL : ERR_OK;
}

HERROR PAL_FUSE_SetSecureCWInfo(HUINT8 ucSC)
{
	DI_SYS_FUSING_INFO_t 	stFusingInfo;
	DI_ERR_CODE				eDiError;

	HxSTD_MemSet(&stFusingInfo, 0, sizeof(Sys_FusingInfo_t));

	stFusingInfo.ucSCS		= FALSE;
	stFusingInfo.ucJTAG		= FALSE;
	stFusingInfo.ucOTP		= FALSE;
	stFusingInfo.ucCWE		= ucSC;
	stFusingInfo.ucRAMSCR	= FALSE;

	eDiError = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, (void *)&stFusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
	if ( eDiError != DI_ERR_OK )
		return ERR_FAIL;

	return (eDiError != ERR_OK) ? ERR_FAIL : ERR_OK;
}

HERROR PAL_FUSE_SetJtagInfo(HUINT8 ucJTAG)
{
	DI_SYS_FUSING_INFO_t 	stFusingInfo;
	DI_ERR_CODE				eDiError;

	HxSTD_MemSet(&stFusingInfo, 0, sizeof(Sys_FusingInfo_t));

	stFusingInfo.ucSCS		= FALSE;
	stFusingInfo.ucJTAG		= ucJTAG;
	stFusingInfo.ucOTP		= FALSE;
	stFusingInfo.ucCWE		= FALSE;
	stFusingInfo.ucRAMSCR	= FALSE;

	eDiError = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, (void *)&stFusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
	if ( eDiError != DI_ERR_OK )
		return ERR_FAIL;

	return (eDiError != ERR_OK) ? ERR_FAIL : ERR_OK;
}

HERROR PAL_FUSE_SetRamScramblingInfo(HUINT8 ucRamScr)
{
	DI_SYS_FUSING_INFO_t 	stFusingInfo;
	DI_ERR_CODE				eDiError;

	HxSTD_MemSet(&stFusingInfo, 0, sizeof(Sys_FusingInfo_t));

	stFusingInfo.ucSCS		= FALSE;
	stFusingInfo.ucJTAG		= ucRamScr;
	stFusingInfo.ucOTP		= FALSE;
	stFusingInfo.ucCWE		= FALSE;
	stFusingInfo.ucRAMSCR	= FALSE;

	eDiError = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, (void *)&stFusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
	if ( eDiError != DI_ERR_OK )
		return ERR_FAIL;

	return (eDiError != ERR_OK) ? ERR_FAIL : ERR_OK;
}


/******** EOF *********/
