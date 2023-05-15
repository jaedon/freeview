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
 * @file	  		nx_prism_skin_alpha.c
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
//  At alpha = 0.1 (10%), At alpha = 0.05 (5%) At alpha = 0.01 (1%)


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
static HUINT32		s_NxPrismSkinColor_m1[C_Table_MAX] = {
	0xffde3e3e,		//C_T_Rythme01 = 0,
	0xffffa200,		//C_T_Orgel01,
	0xffffc444,		//C_T_Orgel02,
	0xffffff00,		//C_T_Yoyo01,
	0xfff8ff44,		//C_T_Yoyo02,
	0xffbaff00,		//C_T_Groove01,
	0xff2eb2ff,		//C_T_Base01,
	0xff135c86,		//C_T_Base02,
	0xff9c67ff,		//C_T_Pitch01,
	0xffffffff,		//C_T_Whistle_100,
	0x0dffffff,		//C_T_Whistle_05,
	0xffe6e6e6,		//C_T_Gong01,
	0xffd2d2d2,		//C_T_Gong02,
	0xffbebebe,		//C_T_Gong03,
	0xffaaaaaa,		//C_T_Gong04,
	0xff969696,		//C_T_Gong05,
	0xff828282,		//C_T_Gong06,
	0xff646464,		//C_T_Gong07,
	0xff414141,		//C_T_Gong08,
	0x66414141,		//C_T_Gong08_40,
	0x1a414141,		//C_T_Gong08_10,
	0xff000000,		//C_T_Bang_100,
	0xcc000000,		//C_T_Bang_80,

	//Graphic
	0xffde3e3e,		//C_G_Rythme01,
	0xfff0640f,		//C_G_Orgel01,
	0xffffa200,		//C_G_Orgel02,
	0xffffff00,		//C_G_Yoyo01,
	0xfff8ff44,		//C_G_Yoyo02,
	0xffbaff00,		//C_G_Groove01,
	0xff2eb2ff,		//C_G_Base01_100,
	0x332eb2ff,		//C_G_Base01_20,
	0xff135c86,		//C_G_Base02_100,
	0xff9c67ff,		//C_G_Pitch01,
	0xffffffff,		//C_G_Whistle_100,
	0x80ffffff,		//C_G_Whistle_50,
	0x4dffffff,		//C_G_Whistle_30,
	0x33ffffff,		//C_G_Whistle_20,
	0x26ffffff,		//C_G_Whistle_15,
	0x1affffff,		//C_G_Whistle_10,
	0x0dffffff,		//C_G_Whistle_05,
	0xff373839,		//C_G_Gong01,
	0x0d373839,		//C_G_Gong01_05,
	0xff1e1e1e,		//C_G_Gong02,
	0xff232323,		//C_G_Gong03,
	0x80232323,		//C_G_Gong03_50,
	0x0d232323,		//C_G_Gong03_05,
	0xff0f0f0f,		//C_G_Gong04,
	0xff000000,		//C_G_Bang_100,
	0xf2000000,		//C_G_Bang_95,
	0xe6000000,		//C_G_Bang_90,
	0xd9000000,		//C_G_Bang_85,
	0xcc000000,		//C_G_Bang_80
	0xc0000000, 	//C_G_Bangk_75
	0x80000000,		//C_G_Bang_50

	// Addd M1
	0xff000000,		//C_T_Shahid_Black
	0xff71cb15,		//C_T_Shahid_Green
	0x33aaaaaa,		//C_T_Gong04_20

	0x3f969ba0,		//C_G_Shahid_01
	0xff808080,		//C_G_Shahid_02,
	0xff71cb15,		//C_G_Shahid_03
};


static	HUINT32		*s_SkinThemeColorTbls[NX_PRSIM_SKIN_MAX] = {
	s_NxPrismSkinColor_m1,
};

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

HUINT32 **NX_PRISM_GetPrimColorTable (void)
{
	return	s_SkinThemeColorTbls;
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

