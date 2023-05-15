/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pal_scart.h
	@brief	  scart driver의 adaptation layer 													\n

	Description: scart 동작과 각 parameter의 조합을 판단하여, HMDI Setting을 한다. 				\n
	Module: PAL/SCART			 																\n

	Copyright (c) 2008 HUMAX Co., Ltd.															\n
	All rights reserved.																		\n
*/


#ifndef	__PAL_SCART_H__
#define	__PAL_SCART_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "octo_common.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

void (*PalScart_Callback_t)(ScartEvent_t, HUINT32, HUINT32);

void PAL_SCART_Init(void);

HERROR PAL_SCART_RegisterCallback(void (*ScartCallbackFunc)(ScartEvent_t eEvent, HUINT32 ulScartId, HUINT32 ulParam1));

HERROR PAL_SCART_SetOutput(HBOOL bEnable);
HERROR PAL_SCART_GetPath (DxScartPath_e *pePath);
HERROR PAL_SCART_SetPath(DxScartPath_e ePath);
HERROR PAL_SCART_SetAspecRatio (DxAspectRatio_e eAR);
HERROR PAL_SCART_GetTvFormat (DxScartFormat_e *peScartFormat);
HERROR PAL_SCART_SetTvFormat (DxScartFormat_e eScartFormat);
HERROR PAL_SCART_GetVcrFormat (DxScartFormat_e *peScartFormat);
HERROR PAL_SCART_SetVcrFormat (DxScartFormat_e eScartFormat);

#endif	//__PAL_VIDEO_H__

