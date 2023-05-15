/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_fuse.h
	@brief	  Octo pal system information header

	Description: Product 정보를 제공하는 system information PAL의 헤더 파일.\n
	Module: PAL/SYS			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_FUSE_H__
#define	__PAL_FUSE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

extern HERROR PAL_FUSE_GetInfo(Sys_FusingInfo_t *pstFusingInfo);
extern HERROR PAL_FUSE_GetInfoExt(Sys_FusingInfo_t *pstFusingInfo);
extern HERROR PAL_FUSE_SetSecureBootInfo(HUINT8 ucSB);
extern HERROR PAL_FUSE_SetSecureCWInfo(HUINT8 ucSC);
extern HERROR PAL_FUSE_SetJtagInfo(HUINT8 ucJTAG);
extern HERROR PAL_FUSE_SetRamScramblingInfo(HUINT8 ucRamScr);

#endif	//!__PAL_FUSE_H__

