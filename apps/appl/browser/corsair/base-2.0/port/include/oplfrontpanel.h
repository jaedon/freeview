/**************************************************************************************/
/**
 * @file oplfrontpanel.h
 *
 * FrontPanel Interfaces
 *
 * @author  moon-young kim mykim@humaxdigital.com)
 * @date    2012/05/11
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
**************************************************************************************
**/

#ifndef __OPLFRONTPANEL_H__
#define __OPLFRONTPANEL_H__

/******************************************************************
	Define
******************************************************************/

/******************************************************************
	Enum
 ******************************************************************/

/******************************************************************
	Typedef
******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

BPLStatus	OPL_HFrontPanel_SetString( const char *panelString, int align, int dimlevel, BPL_BOOL bAuto );
BPLStatus	OPL_HFrontPanel_SetStringAsTopPriority( const char *panelString, int align, int dimlevel, BPL_BOOL bAuto );
BPLStatus	OPL_HFrontPanel_SetLedOnOff(int ledId, BPL_BOOL bOnOff, BPL_BOOL bAuto, BPL_BOOL bAllClear);
BPLStatus	OPL_HFrontPanel_SetAnimation(int fpItemType, int param1, int param2, int param3);
BPLStatus 	OPL_HFrontPanel_SetCategoryInform(int nCategory);


#ifdef __cplusplus
};

#endif

/******************************************************************
	Interfaces
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

#ifdef __cplusplus
};

#endif	/* __cplusplus */

#endif	/* __OPLFRONTPANEL_H__ */



