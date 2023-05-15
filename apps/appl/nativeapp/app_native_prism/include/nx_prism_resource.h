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

/******************************************************************************/
/**
 * @file	  		nx_prism_resource.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_RESOURCE_H__
#define __NX_PRISM_RESOURCE_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
//
#include <nx_system_startlogo.h>
#include <nx_prism_definition.h>

// menu/settings/
//#include <nx_ui_search.h>

/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum {
	NX_PRISM_SKIN_DEFAULT = 0,
	NX_PRISM_SKIN_ALPHA = NX_PRISM_SKIN_DEFAULT,

	// TODO: Add to hear
	NX_PRSIM_SKIN_MAX,
} NxSkinThemeID_t;

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/

/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/

extern	HERROR		NX_PRISM_Resource_Init(void);

extern	HUINT8		*NX_PRISM_GetTextBuffer(HUINT32 ulSize);

extern	HERROR		NX_PRISM_Resource_InitSkinTheme(void);
extern	HINT32		NX_PRISM_Resource_GetCurSkinTheme(void);
extern	HERROR		NX_PRISM_Resource_SetCurSkinTheme(HINT32 lSkinTheme, HBOOL bFirst);
extern	HUINT32		NX_PRISM_Resource_GetNumOfThemes(void);
extern	HUINT32		NX_PRISM_Resource_GetDefaultTheme(void);
extern	HBOOL		NX_PRISM_Resource_IsUseColorTbl(void);
extern	HUINT32		**NX_PRISM_Resource_GetThemeColorTbls(void);

extern	HUINT32	inline	COL(SkinColorIdx_t eColorIdx);
extern	HUINT32	inline	COL_Alpha(SkinColorIdx_t eColorIdx);
extern	HUINT32	inline	COL_Red(SkinColorIdx_t eColorIdx);
extern	HUINT32	inline	COL_Green(SkinColorIdx_t eColorIdx);
extern	HUINT32	inline	COL_Blue(SkinColorIdx_t eColorIdx);
extern	HUINT32	inline	COL_ARGB(HUINT32 alpha, HUINT32 red, HUINT32 green, HUINT32 blue);
extern	HUINT32	inline	Get_Alpha(HUINT32 color);
extern	HUINT32	inline	Get_Red(HUINT32 color);
extern	HUINT32	inline	Get_Green(HUINT32 color);
extern	HUINT32	inline	Get_Blue(HUINT32 color);

#if !defined(CONFIG_APUI_SKIN_COLOR_NONE)
extern	HUINT32 **NX_PRISM_GetPrimColorTable(void);
#endif

#endif // __NX_PRISM_RESOURCE_H__

