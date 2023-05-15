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
 * @file	  		nx_prism_definition.h
 *
 * 	Description:  	define for GUI Guide			\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_DEFINITION_H__
#define __NX_PRISM_DEFINITION_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
/******************************************************************************/
#define _________OSD_________________________________________________________
/******************************************************************************/
#define NX_PRISM_OSD_X								0
#define NX_PRISM_OSD_Y								0
#define NX_PRISM_OSD_HORIZONTAL						1280//CONFIG_MWC_OSD_HORIZONTAL
#define NX_PRISM_OSD_VERTICAL						720//CONFIG_MWC_OSD_VERTICAL


/* define font family, size degree */
/******************************************************************************/
#define _________Font_________________________________________________________
/******************************************************************************/
/* size degrees for font */
#define	NX_PRISM_FONT_SIZE_64						64
#define	NX_PRISM_FONT_SIZE_46						46
#define	NX_PRISM_FONT_SIZE_36						36
#define	NX_PRISM_FONT_SIZE_34						34
#define	NX_PRISM_FONT_SIZE_32						32
#define	NX_PRISM_FONT_SIZE_28						28
#define	NX_PRISM_FONT_SIZE_26						26
#define	NX_PRISM_FONT_SIZE_24						24
#define	NX_PRISM_FONT_SIZE_22						22
#define	NX_PRISM_FONT_SIZE_20						20
#define	NX_PRISM_FONT_SIZE_18						18

#define	NX_PRISM_FONT_VSPACE						8


/* define main color element */
/******************************************************************************/
#define _________COLOR________________________________________________________
/******************************************************************************/
/* new color palette */
typedef enum {
	// TEXT Color
	C_T_Rythme01 = 0,
	C_T_Orgel01,
	C_T_Orgel02,
	C_T_Yoyo01,
	C_T_Yoyo02,
	C_T_Groove01,
	C_T_Base01,
	C_T_Base02,
	C_T_Pitch01,
	C_T_Whistle_100,
	C_T_Whistle_05,
	C_T_Gong01,
	C_T_Gong02,
	C_T_Gong03,
	C_T_Gong04,
	C_T_Gong05,
	C_T_Gong06,
	C_T_Gong07,
	C_T_Gong08,
	C_T_Gong08_40,
	C_T_Gong08_10,
	C_T_Bang_100,
	C_T_Bang_80,

	//Graphic
	C_G_Rythme01,
	C_G_Orgel01,
	C_G_Orgel02,
	C_G_Yoyo01,
	C_G_Yoyo02,
	C_G_Groove01,
	C_G_Base01_100,
	C_G_Base01_20,
	C_G_Base02_100,
	C_G_Pitch01,
	C_G_Whistle_100,
	C_G_Whistle_50,
	C_G_Whistle_30,
	C_G_Whistle_20,
	C_G_Whistle_15,
	C_G_Whistle_10,
	C_G_Whistle_05,
	C_G_Gong01,
	C_G_Gong01_05,
	C_G_Gong02,
	C_G_Gong03,
	C_G_Gong03_50,
	C_G_Gong03_05,
	C_G_Gong04,
	C_G_Bang_100,
	C_G_Bang_95,
	C_G_Bang_90,
	C_G_Bang_85,
	C_G_Bang_80,
	C_G_Bangk_75,
	C_G_Bang_50,

#if defined(CONFIG_APUI_SKIN_COLOR_MBC)
	C_T_Shahid_Black,
	C_T_Shahid_Green,
	C_T_Gong04_20,

	C_G_Shahid_01,
	C_G_Shahid_02,
	C_G_Shahid_03,
#else
	// TODO: Add Skin Color palette

#endif

	C_Table_MAX,
} SkinColorIdx_t;

#if 0
#define	COLOR_TRANSPARENT						0x00000000
#define	COLOR_BLACK								0xFF000000
#define	COLOR_WHITE								0xFFFFFFFF
#define	COLOR_WHITE2							0xFFDCDCDC		// LCD용 white 너무 하얗게 할 수 없는 부분이 있다.
#define	COLOR_RED								0xFFFF0000
#define	COLOR_GREEN								0xFF00FF00
#define	COLOR_BLUE								0xFF0000FF
#endif

#define	COLOR_MASK_32BIT_ONLY_COLOR_BYTES		0x00FFFFFF
#define	COLOR_MASK_32BIT_ONLY_ALPHA_BYTE		0xFF000000
#define	COLOR_MASK_32BIT_ONLY_RED_BYTE			0x00FF0000
#define	COLOR_MASK_32BIT_ONLY_GREEN_BYTE		0x0000FF00
#define	COLOR_MASK_32BIT_ONLY_BLUE_BYTE			0x000000FF

#define	COLOR_MASK_32BIT_ONLY_COMPONENT_EFFECTIVE_BYTE	0x000000FF

#define	COLOR_SHIFTER_ALPHA_BYTE				24
#define	COLOR_SHIFTER_RED_BYTE					16
#define	COLOR_SHIFTER_GREEN_BYTE				8
#define	COLOR_SHIFTER_BLUE_BYTE					0

#define RES_NONE_PNG							"NONE"


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/


#endif // __NX_PRISM_DEFINITION_H__

