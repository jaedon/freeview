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
 * @file	  		nx_prism_menu_items.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_MENU_ITEMS_H__
#define __NX_PRISM_MENU_ITEMS_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/

typedef enum
{
	eNxUiMenuHiddenType_None 				= 0x00000000,
	eNxUiMenuHiddenType_All 				= 0x00000001,
	eNxUiMenuHiddenType_PasswordReset 		= 0x00000002,
	eNxUiMenuHiddenType_FusingInfo			= 0x00000004,
	eNxUiMenuHiddenType_CountryCode 		= 0x00000008,
} NxUiMenuHiddenType_t;


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
typedef enum
{
	NX_MENUITEM_ID_NONE = 0,

	// ----    Common	 ---- //
	NX_MENUITEM_ID_SET_OFF_ON,
	NX_MENUITEM_ID_SET_ON_OFF,

	NX_MENUITEM_ID_SET_MATURITY_RATING,
	NX_MENUITEM_ID_SET_CHANGE_PW,
	NX_MENUITEM_ID_SET_BLOCKCHSETTING,
	NX_MENUITEM_ID_SET_SYSTEM_LANG,
	NX_MENUITEM_ID_SET_AUDIO_LANG,
	NX_MENUITEM_ID_SET_SUBTITLE_LANG,
	NX_MENUITEM_ID_SET_DAYLIGHT_SAVING_TIME,
	NX_MENUITEM_ID_SET_ASPECT_RATIO,
	NX_MENUITEM_ID_SET_4_3_DISPLAY_FORMAT,
	NX_MENUITEM_ID_SET_16_9_DISPLAY_FORMAT,
	NX_MENUITEM_ID_SET_RESOLUTION,
	NX_MENUITEM_ID_SET_SPDIF,
	NX_MENUITEM_ID_SET_HDMI,
	NX_MENUITEM_ID_SET_AUDIOTYPE,
    NX_MENUITEM_ID_SET_INFO_DISPLAY_TIME,
    NX_MENUITEM_ID_SET_SUBTITLE_FONT,
    NX_MENUITEM_ID_SET_REPEATOPTION,
    NX_MENUITEM_ID_SET_TEXTPOSITION,
    NX_MENUITEM_ID_SET_FONTSIZE,
	NX_MENUITEM_ID_SET_COUNTRYSETTING,

    // ----    System    ---- //
    NX_MENUITEM_ID_SET_AUTOMATICPOWERDOWN,
    NX_MENUITEM_ID_SET_POWERSAVINGINSTANDBY,
    NX_MENUITEM_ID_SET_POWERONTIMER,
    NX_MENUITEM_ID_SET_POWEROFFTIMER,
    NX_MENUITEM_ID_SET_POWERTIMERVOLUME,
    NX_MENUITEM_ID_SET_POWERTIMERREPEAT,

	// ----    Search Channels 		  ---- //
	NX_MENUITEM_ID_SEARCH_OPTION_SCRAMBLEDTYPE,
	NX_MENUITEM_ID_SEARCH_OPTION_CHANNELTYPE,

	// ----    Search Channels (Fixed Antenna)    ---- //
	NX_MENUITEM_ID_SEARCH_FIXED_SWITCHTYPE,
	NX_MENUITEM_ID_SEARCH_FIXED_OPTION,
	NX_MENUITEM_ID_SEARCH_FIXED_OPTION_IMAGE_N,
	NX_MENUITEM_ID_SEARCH_FIXED_OPTION_IMAGE_C,
	NX_MENUITEM_ID_SEARCH_SATSETTING_LNBFREQUENCY,
	NX_MENUITEM_ID_SEARCH_SATSETTING_22KTONE,
	NX_MENUITEM_ID_SEARCH_SATSETTING_DISEQC,

	NX_MENUITEM_ID_SEARCH_TPSETTING_POLARIZATION,
	NX_MENUITEM_ID_SEARCH_TPSETTING_SYMBOLRATE,
	NX_MENUITEM_ID_SEARCH_TPSETTING_TRANSMISSION,
	NX_MENUITEM_ID_SEARCH_TPSETTING_FEC_DVB,
	NX_MENUITEM_ID_SEARCH_TPSETTING_FEC_DVBS2,

	// ----		Search Channels (Scd Antenna)	---- //
	NX_MENUITEM_ID_SEARCH_SCDSETTING_TUNER,
	NX_MENUITEM_ID_SEARCH_SCDSETTING_USERBAND,

#ifdef  CONFIG_MW_INET
	// ----    Network 		  ---- //
	NX_MENUITEM_ID_SET_NETWORK_CONFIGURE_IP,
#endif  //CONFIG_MW_INET

#ifdef	CONFIG_MW_MM_PVR
	// ----		PVR			---- //
	NX_MENUITEM_ID_SET_PVR_INSTANT_REPLAY_TIME,
	NX_MENUITEM_ID_SET_PVR_SKIP_FORWARD_TIME,
	NX_MENUITEM_ID_SET_PVR_PADDING_TIME,
#endif	//CONFIG_MW_MM_PVR

	// ----    ELSE	 ---- //
	NX_MENUITEM_ID_SCHEDULE_REPEAT,
	NX_MENUITEM_ID_SCHEDULE_MODE,

	NX_MENUITEM_ID_MAX_ITEMS
} Nx_MenuSubItemId_t;


typedef struct tag_ItemTextList
{
	HINT32		 lValue;
	HCHAR		*ucStrId;
	char		*szText;
} NxMenuItemText_t;

typedef struct tag_MenuItemList
{
	Nx_MenuSubItemId_t	 eSubitemId;
	HUINT32			 	ulItemNum;
	NxMenuItemText_t	*pstItem;
} NxMenuItem_t;


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/
ONDK_Result_t	NX_PRISM_MENU_Menuitem_CountSubitems (Nx_MenuSubItemId_t eMenuItemId, HUINT32 *pulNum);
HUINT8 **NX_PRISM_MENU_MenuItem_MakeTextListFromMenuItemId(Nx_MenuSubItemId_t eMenuItemId);
HUINT8 **NX_PRISM_MENU_MenuItem_AllocTextList (HUINT32 ulNumItem, HUINT32 ulTextLen);
void			NX_PRISM_MENU_MenuItem_DeleteTextListFromMenuItemId (Nx_MenuSubItemId_t eMenuItemId, HUINT8 **ppucTextArray);
HERROR			NX_PRISM_MENU_MenuItem_FreeTextList (HUINT32 ulNumItem, HUINT8 **ppucTextArray);
ONDK_Result_t	NX_PRISM_MENU_MenuItem_GetIndexFromValue(Nx_MenuSubItemId_t eMenuItemId, HINT32 lValue, HUINT32 *pulIndex);
ONDK_Result_t	NX_PRISM_MENU_MenuItem_GetValueFromIndex(Nx_MenuSubItemId_t eMenuItemId, HUINT32 ulIndex, HINT32 *pnValue);

ONDK_Result_t	NX_PRISM_MENU_Menuitem_CountExpandSubitems(Nx_MenuSubItemId_t eMenuItemId, HUINT32 *pulNum);
ONDK_Result_t	NX_PRISM_MENU_MenuItem_GetExpandIndexFromValue(Nx_MenuSubItemId_t eMenuItemId, HINT32 lValue, HUINT32 *pulIndex);
ONDK_Result_t	NX_PRISM_MENU_MenuItem_GetExpandValueFromIndex(Nx_MenuSubItemId_t eMenuItemId, HUINT32 ulIndex, HINT32 *pnValue);
HUINT8 **NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId(Nx_MenuSubItemId_t eMenuItemId, HBOOL bShowNumber);

ONDK_Result_t	NX_PRISM_MENU_MenuItem_MoveLanguageList(HUINT32 ulMenuItemId, HxLANG_Id_e eLangId, HINT32 lIndex);
ONDK_Result_t	NX_PRISM_MENU_MenuItem_InsertLanguageList(HUINT32 ulMenuItemId, HxLANG_Id_e eLangId, HINT32 lIndex);
ONDK_Result_t	NX_PRISM_MENU_MenuItem_AddLanguageList(HUINT32 ulMenuItemId, HxLANG_Id_e eLangId);
void			NX_PRISM_MENU_MenuItem_SetDefaultLanguageList(HUINT32 ulMenuItemId);
ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadSystemLanguageList(HBOOL bSetDefault);
ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadAudioLanguageList(HBOOL bSetDefault);
ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadSubtitleLanguageList(HBOOL bSetDefault);
ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadLanguageList(HUINT32 ulMenuItemId, HBOOL bSetDefault);

ONDK_Result_t	NX_PRISM_MENU_MenuItem_AddResolutionList(DxResolutionSelect_e eResolution);
ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadResolutionList(HBOOL bSetDefault);

ONDK_Result_t	NX_PRISM_MENU_MenuItem_AddCountryList(HxCountry_e eCountryIdx);
ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadCountryList(HBOOL bSetDefault);

ONDK_Result_t	NX_PRISM_MENU_MenuItem_AddParentalContrlList(DxRatingAge_e eRatingAgeIdx, HINT8 *pszStrId);
ONDK_Result_t	NX_PRISM_MENU_MenuItem_LoadParentalContrl(HBOOL bSetDefault);

#endif // __NX_PRISM_MENU_ITEMS_H__

