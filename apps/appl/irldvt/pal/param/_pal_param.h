/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _pal_param.h
	@brief	  Octo pal system information header

	Description: Product 정보를 제공하는 system information PAL의 헤더 파일.\n
	Module: PAL/SYS			 	\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PAL_PARAM_INT_H__
#define	__PAL_PARAM_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <octo_common.h>

#include <pal_param.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	ePAL_PARAM_TYPE_INT		= 0,
	ePAL_PARAM_TYPE_STR,
	ePAL_PARAM_TYPE_BIN,
	ePAL_PARAM_TYPE_MAX
};

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR	 pal_param_GetSizeInfo(PAL_PARAM_Item_e eItem, HUINT32 *pulDiSize, HUINT32 *pulPalSize);
HERROR	 pal_param_GetDataType(PAL_PARAM_Item_e eItem);
HCHAR	*pal_param_GetItemStr(PAL_PARAM_Item_e eItem);
HCHAR	*pal_param_GetDataTypeStr(HUINT32 ulDataType);
HBOOL	 pal_param_IsActionAvailable(PAL_PARAM_Item_e eItem, HBOOL bSet);

#endif	//!__PAL_PARAM_INT_H__

