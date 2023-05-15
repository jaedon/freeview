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
 * @file	  		nx_prism_resource.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>
#include <nx_prism_app.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_MAX_BUF_TEXT64						32
#define	NX_PRISM_MAX_BUF_TEXT128					(NX_PRISM_MAX_BUF_TEXT64/2)
#define	NX_PRISM_MAX_BUF_TEXT256					(NX_PRISM_MAX_BUF_TEXT128/2)
#define	NX_PRISM_MAX_BUF_TEXT512					(NX_PRISM_MAX_BUF_TEXT256/2)


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct {
	HINT32		nNumOfSkins;
	HINT32		nCurSkinIdx;

	/* color table, color table은 쓰지 않을 수도 있다. */
	HBOOL		bUseColorTbl;
	HUINT32		**s_SkinColorTable;

} SkinThemeInfo_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static SkinThemeInfo_t		s_SkinThemeInfo;
//static HUINT32				*s_SkinThemeColorTbls;

static HUINT8	s_indexBuf64;
static HUINT8	s_indexBuf128;
static HUINT8	s_indexBuf256;
static HUINT8	s_indexBuf512;

static HUINT8	s_textBuf64[NX_PRISM_MAX_BUF_TEXT64][NX_TEXT_SIZE_64];
static HUINT8	s_textBuf128[NX_PRISM_MAX_BUF_TEXT128][NX_TEXT_SIZE_128];
static HUINT8	s_textBuf256[NX_PRISM_MAX_BUF_TEXT256][NX_TEXT_SIZE_256];
static HUINT8	s_textBuf512[NX_PRISM_MAX_BUF_TEXT512][NX_TEXT_SIZE_512];


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
HERROR		NX_PRISM_Resource_Init(void)
{
	HERROR	resResult = ERR_FAIL;

	/* init */
	resResult = NX_PRISM_Resource_InitSkinTheme();

	return	ERR_OK;
}


#define	___About_STRING___________________________________

HUINT8	*NX_PRISM_GetTextBuffer(HUINT32 ulSize)
{
	HINT32	i = 0;

	switch (ulSize)
	{
		default:
		case NX_TEXT_SIZE_64 :
			i = s_indexBuf64++&(NX_PRISM_MAX_BUF_TEXT64-1);
			s_textBuf64[i][0] = 0;
			return s_textBuf64[i];
		case NX_TEXT_SIZE_128 :
			i = s_indexBuf128++&(NX_PRISM_MAX_BUF_TEXT128-1);
			s_textBuf128[i][0] = 0;
			return s_textBuf128[i];
		case NX_TEXT_SIZE_256 :
			i = s_indexBuf256++&(NX_PRISM_MAX_BUF_TEXT256-1);
			s_textBuf256[i][0] = 0;
			return s_textBuf256[i];
		case NX_TEXT_SIZE_512 :
			i = s_indexBuf512++&(NX_PRISM_MAX_BUF_TEXT512-1);
			s_textBuf512[i][0] = 0;
			return s_textBuf512[i];
	}
}


#define	___About_SKIN_______________________________________

HERROR		NX_PRISM_Resource_InitSkinTheme(void)
{
	HxSTD_memset(&s_SkinThemeInfo, 0, sizeof(SkinThemeInfo_t));

	/* real used value settings */
	s_SkinThemeInfo.nNumOfSkins = NX_PRISM_Resource_GetNumOfThemes();
	s_SkinThemeInfo.nCurSkinIdx = NX_PRISM_Resource_GetDefaultTheme();

	s_SkinThemeInfo.bUseColorTbl = NX_PRISM_Resource_IsUseColorTbl();
	if(s_SkinThemeInfo.bUseColorTbl == TRUE)
	{
		s_SkinThemeInfo.s_SkinColorTable = NX_PRISM_Resource_GetThemeColorTbls();
	}

	if((s_SkinThemeInfo.nNumOfSkins == 0)
		|| (s_SkinThemeInfo.nCurSkinIdx < 0)
		|| (s_SkinThemeInfo.nCurSkinIdx >= s_SkinThemeInfo.nNumOfSkins)
		|| ((s_SkinThemeInfo.bUseColorTbl == TRUE) && (s_SkinThemeInfo.s_SkinColorTable == NULL)))
	{
		NX_APP_Error("Error!!! NX_PRISM_Resource_InitSkinTheme() Fail!! ");
		return	ERR_FAIL;
	}

	return ERR_OK;
}

HINT32		NX_PRISM_Resource_GetCurSkinTheme(void)
{
	return	s_SkinThemeInfo.nCurSkinIdx;
}

HERROR		NX_PRISM_Resource_SetCurSkinTheme(HINT32 nSkinTheme, HBOOL bFirst)
{
	if ((nSkinTheme < NX_PRISM_SKIN_DEFAULT) || (nSkinTheme >= s_SkinThemeInfo.nNumOfSkins))
	{
		return	ERR_FAIL;
	}

	if ((bFirst == TRUE) || (s_SkinThemeInfo.nCurSkinIdx != nSkinTheme))
	{
		s_SkinThemeInfo.nCurSkinIdx = nSkinTheme;
	}

	return	ERR_OK;
}

HUINT32		NX_PRISM_Resource_GetNumOfThemes(void)
{
	return	NX_PRSIM_SKIN_MAX;
}

HUINT32		NX_PRISM_Resource_GetDefaultTheme(void)
{
	return	NX_PRISM_SKIN_DEFAULT;
}

HBOOL		NX_PRISM_Resource_IsUseColorTbl(void)
{
	return TRUE;
}

HUINT32		**NX_PRISM_Resource_GetThemeColorTbls(void)
{
#if defined(CONFIG_APUI_SKIN_COLOR_NONE)
	return	NULL;
#else
	return	NX_PRISM_GetPrimColorTable();
#endif
}

/* 특정 theme palette를 지정하여 해당 index의 color를 가져 온다. */
HUINT32		NX_PRISM_Resource_GetThemeColor(HINT32 nSkinIdx, SkinColorIdx_t eColorIdx)
{
	if((s_SkinThemeInfo.bUseColorTbl == FALSE)
		|| (nSkinIdx < NX_PRISM_SKIN_DEFAULT) || (nSkinIdx >= s_SkinThemeInfo.nNumOfSkins)
		|| (eColorIdx < 0) || (eColorIdx >= C_Table_MAX))
		return 0x00000000;	// transparent color

	return s_SkinThemeInfo.s_SkinColorTable[nSkinIdx][eColorIdx];
}

/* 현재의 skin thteme의 color를 준다. 어디나 불리며 간단하다. inline */
HUINT32	inline COL(SkinColorIdx_t eColorIdx)
{
	if((s_SkinThemeInfo.bUseColorTbl == FALSE) ||(eColorIdx < 0) || (eColorIdx >= C_Table_MAX))
		return 0x00000000;	// transparent color

	return s_SkinThemeInfo.s_SkinColorTable[s_SkinThemeInfo.nCurSkinIdx][eColorIdx];
}

/* 현재의 skin thteme의 color의 alpha를 준다. 어디나 불리며 간단하다. inline */
HUINT32	inline COL_Alpha(SkinColorIdx_t eColorIdx)
{
	if((s_SkinThemeInfo.bUseColorTbl == FALSE) ||(eColorIdx < 0) || (eColorIdx >= C_Table_MAX))
		return 0x00000000;	// transparent color

	return (((s_SkinThemeInfo.s_SkinColorTable[s_SkinThemeInfo.nCurSkinIdx][eColorIdx]) & COLOR_MASK_32BIT_ONLY_ALPHA_BYTE) >> COLOR_SHIFTER_ALPHA_BYTE);
}

/* 현재의 skin thteme의 color의 red를 준다. 어디나 불리며 간단하다. inline */
HUINT32	inline COL_Red(SkinColorIdx_t eColorIdx)
{
	if((s_SkinThemeInfo.bUseColorTbl == FALSE) ||(eColorIdx < 0) || (eColorIdx >= C_Table_MAX))
		return 0x00000000;	// transparent color

	return (((s_SkinThemeInfo.s_SkinColorTable[s_SkinThemeInfo.nCurSkinIdx][eColorIdx]) & COLOR_MASK_32BIT_ONLY_RED_BYTE) >> COLOR_SHIFTER_RED_BYTE);
}

/* 현재의 skin thteme의 color의 green를 준다. 어디나 불리며 간단하다. inline */
HUINT32	inline COL_Green(SkinColorIdx_t eColorIdx)
{
	if((s_SkinThemeInfo.bUseColorTbl == FALSE) ||(eColorIdx < 0) || (eColorIdx >= C_Table_MAX))
		return 0x00000000;	// transparent color

	return (((s_SkinThemeInfo.s_SkinColorTable[s_SkinThemeInfo.nCurSkinIdx][eColorIdx]) & COLOR_MASK_32BIT_ONLY_GREEN_BYTE) >> COLOR_SHIFTER_GREEN_BYTE);
}

/* 현재의 skin thteme의 color의 blue를 준다. 어디나 불리며 간단하다. inline */
HUINT32	inline COL_Blue(SkinColorIdx_t eColorIdx)
{
	if((s_SkinThemeInfo.bUseColorTbl == FALSE) ||(eColorIdx < 0) || (eColorIdx >= C_Table_MAX))
		return 0x00000000;	// transparent color

	return ((s_SkinThemeInfo.s_SkinColorTable[s_SkinThemeInfo.nCurSkinIdx][eColorIdx]) & COLOR_MASK_32BIT_ONLY_BLUE_BYTE);
}

/* ARGB로 조합된 color를 준다. 어디나 불리며 간단하다. inline */
HUINT32	inline	COL_ARGB(HUINT32 alpha, HUINT32 red, HUINT32 green, HUINT32 blue)
{
	return (((alpha & COLOR_MASK_32BIT_ONLY_COMPONENT_EFFECTIVE_BYTE) << COLOR_SHIFTER_ALPHA_BYTE)
		| ((red & COLOR_MASK_32BIT_ONLY_COMPONENT_EFFECTIVE_BYTE) << COLOR_SHIFTER_RED_BYTE)
		| ((green & COLOR_MASK_32BIT_ONLY_COMPONENT_EFFECTIVE_BYTE) << COLOR_SHIFTER_GREEN_BYTE)
		| (blue & COLOR_MASK_32BIT_ONLY_COMPONENT_EFFECTIVE_BYTE));
}

/* color의 alpha component를 준다. */
HUINT32	inline	Get_Alpha(HUINT32 color)
{
	return ((color & COLOR_MASK_32BIT_ONLY_ALPHA_BYTE) >> COLOR_SHIFTER_ALPHA_BYTE);
}

/* color의 red component를 준다. */
HUINT32	inline	Get_Red(HUINT32 color)
{
	return ((color & COLOR_MASK_32BIT_ONLY_RED_BYTE) >> COLOR_SHIFTER_RED_BYTE);
}

/* color의 green component를 준다. */
HUINT32	inline	Get_Green(HUINT32 color)
{
	return ((color & COLOR_MASK_32BIT_ONLY_GREEN_BYTE) >> COLOR_SHIFTER_GREEN_BYTE);
}

/* color의 blue component를 준다. */
HUINT32	inline	Get_Blue(HUINT32 color)
{
	return (color & COLOR_MASK_32BIT_ONLY_BLUE_BYTE);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

