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
 * @file	  		nx_prism_util.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


#ifndef __NX_PRISM_UTIL_H__
#define __NX_PRISM_UTIL_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <nx_prism_ui.h>
#include <nx_core_media.h>
#include <nx_core_channel.h>


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/

#define	NX_UTIL_OPTION_MAX_NUM								64
#if defined(CONFIG_MW_CH_SATELLITE)
#define	NX_UTIL_SAT_SATNAME_AMOS_2_3						"AMOS 2&3"
#define	NX_UTIL_SAT_SATNAME_APSTAR_2R						"APSTAR 2R"
#define	NX_UTIL_SAT_SATNAME_ARABSAT_5A						"ARABSAT 5A"
#define	NX_UTIL_SAT_SATNAME_ASIASAT_3S						"ASIASAT 3S"
#define	NX_UTIL_SAT_SATNAME_ASIASAT_5						"ASIASAT 5"
#define	NX_UTIL_SAT_SATNAME_ASTRA_1							"ASTRA 1"
#define	NX_UTIL_SAT_SATNAME_ASTRA_2							"ASTRA 2"
#define	NX_UTIL_SAT_SATNAME_ASTRA_3A_3B						"ASTRA 3A&3B"
#define	NX_UTIL_SAT_SATNAME_ASTRA_4A						"ASTRA 4A"
#define	NX_UTIL_SAT_SATNAME_ATLANTIC_BIRD_1					"ATLANTIC BIRD 1"
#define	NX_UTIL_SAT_SATNAME_ATLANTIC_BIRD_2					"ATLANTIC BIRD 2"
#define	NX_UTIL_SAT_SATNAME_ATLANTIC_BIRD_3					"ATLANTIC BIRD 3"
#define	NX_UTIL_SAT_SATNAME_BADR_4_6						"BADR 4&6"
#define	NX_UTIL_SAT_SATNAME_EUROBIRD_1						"EUROBIRD 1"
#define	NX_UTIL_SAT_SATNAME_EUROBIRD_3						"EUROBIRD 3"
#define	NX_UTIL_SAT_SATNAME_EUROBIRD_9A						"EUROBIRD 9A"
#define	NX_UTIL_SAT_SATNAME_EUTELSAT_W2A					"EUTELSAT W2A"
#define	NX_UTIL_SAT_SATNAME_EUTELSAT_W3A					"EUTELSAT W3A"
#define	NX_UTIL_SAT_SATNAME_EUTELSAT_W4_W7					"EUTELSAT W4&W7"
//#define	NX_UTIL_SAT_SATNAME_EUTELSAT_W4_W7				"EUTELSAT W4&W7"
#define	NX_UTIL_SAT_SATNAME_EUTELSAT_W6						"EUTELSAT W6"
#define	NX_UTIL_SAT_SATNAME_EXPRESS_AM1						"EXPRESS AM1"
#define	NX_UTIL_SAT_SATNAME_EXPRESS_AM22					"EXPRESS AM22"
#define	NX_UTIL_SAT_SATNAME_HELLAS_SAT2						"HELLAS SAT2"
#define	NX_UTIL_SAT_SATNAME_HISPASAT_1C_1D					"HISPASAT 1C&1D"
#define	NX_UTIL_SAT_SATNAME_HOTBIRD							"HOTBIRD"
#define	NX_UTIL_SAT_SATNAME_INSAT_3A_4B						"INSAT 3A&4B"
#define	NX_UTIL_SAT_SATNAME_INTELSAT_10_02					"INTELSAT 10-02"
#define	NX_UTIL_SAT_SATNAME_INTELSAT_12						"INTELSAT 12"
#define	NX_UTIL_SAT_SATNAME_INTELSAT_7_10					"INTELSAT 7&10"
#define	NX_UTIL_SAT_SATNAME_INTELSAT_901					"INTELSAT 901"
#define	NX_UTIL_SAT_SATNAME_INTELSAT_902					"INTELSAT 902"
#define	NX_UTIL_SAT_SATNAME_INTELSAT_904					"INTELSAT 904"
#define	NX_UTIL_SAT_SATNAME_INTELSAT_906					"INTELSAT 906"
#define	NX_UTIL_SAT_SATNAME_INTELSAT_907					"INTELSAT 907"
#define	NX_UTIL_SAT_SATNAME_NILESAT_101_102					"NILESAT 101&102"
#define	NX_UTIL_SAT_SATNAME_NSS_12							"NSS 12"
#define	NX_UTIL_SAT_SATNAME_NSS_7							"NSS 7"
#define	NX_UTIL_SAT_SATNAME_TELSTAR_12						"TELSTAR 12"
#define	NX_UTIL_SAT_SATNAME_THOR_5_6						"THOR 5&6"
#define	NX_UTIL_SAT_SATNAME_TURKSAT_2A_3A					"TURKSAT 2A&3A"
#define	NX_UTIL_SAT_SATNAME_YAMAL_201						"YAMAL 201"
#define	NX_UTIL_SAT_SATNAME_ARABSAT_2B						"ARABSAT 2B"
#define	NX_UTIL_SAT_SATNAME_EUTELSAT_W1						"EUTELSAT W1"
#define	NX_UTIL_SAT_SATNAME_EUTELSAT_W2						"EUTELSAT W2"
#define	NX_UTIL_SAT_SATNAME_NSS_703							"NSS 703"
#define	NX_UTIL_SAT_SATNAME_SIRIUS							"SIRIUS"
#define	NX_UTIL_SAT_SATNAME_TELECOM_2D						"TELECOM 2D"
#define	NX_UTIL_SAT_SATNAME_EUROBIRD_9						"EUROBIRD 9"
#define	NX_UTIL_SAT_SATNAME_INTELSAT_802					"INTELSAT 802"
#define	NX_UTIL_SAT_SATNAME_THOR_3_5						"THOR 3_5"
#define	NX_UTIL_SAT_SATNAME_INSAT_2E4A						"INSAT 2E4A"
#define	NX_UTIL_SAT_SATNAME_INTELSAT_17						"INTELSAT 17"
#define	NX_UTIL_SAT_SATNAME_YAHLIVE							"YAHLIVE"
#define NX_UTIL_SAT_SATNAME_HISPASAT_1D						"Hispasat 1D"
#define NX_UTIL_SAT_SATNAME_HISPASAT_1E						"Hispasat 1E"
#define NX_UTIL_SAT_SATNAME_SES_2							"SES 2"
#define NX_UTIL_SAT_SATNAME_SES_3							"SES 3"
#define NX_UTIL_SAT_SATNAME_SES_6							"SES 6"
#define NX_UTIL_SAT_SATNAME_EUTELSAT_5_WEST_A				"EUTELSAT 5 West A"
#define NX_UTIL_SAT_SATNAME_EUTELSAT_12_WEST_B				"EUTELSAT 12 West B"
#define NX_UTIL_SAT_SATNAME_EUTELSAT_113_WEST_A				"EUTELSAT 113 West A"
#define NX_UTIL_SAT_SATNAME_EUTELSAT_117_WEST_A				"EUTELSAT 117 West A"
#define NX_UTIL_SAT_SATNAME_EUTELSAT_HOTBIRD_13B			"EUTELSAT HOTBIRD 13B"
#define NX_UTIL_SAT_SATNAME_EUTELSAT_HOTBIRD_13C			"EUTELSAT HOTBIRD 13C"
#define NX_UTIL_SAT_SATNAME_EUTELSAT_HOTBIRD_13D			"EUTELSAT HOTBIRD 13D"
#define NX_UTIL_SAT_SATNAME_INTELSAT_1R						"INTELSAT 1R"
#define NX_UTIL_SAT_SATNAME_INTELSAT_11						"INTELSAT 11"
#define NX_UTIL_SAT_SATNAME_INTELSAT_14						"INTELSAT 14"
#define NX_UTIL_SAT_SATNAME_INTELSAT_21						"INTELSAT 21"
#define NX_UTIL_SAT_SATNAME_INTELSAT_23						"INTELSAT 23"
#define NX_UTIL_SAT_SATNAME_INTELSAT_34						"INTELSAT 34"
#define NX_UTIL_SAT_SATNAME_INTELSAT_903					"INTELSAT 903"
#define NX_UTIL_SAT_SATNAME_GALAXY_18						"Galaxy 18"
#define NX_UTIL_SAT_SATNAME_GALAXY_19						"Galaxy 19"
#define NX_UTIL_SAT_SATNAME_GALAXY_28						"Galaxy 28"
#define NX_UTIL_SAT_SATNAME_SPACEWAY_1_AND_DIRECTV_10_02	"Spaceway 1 & DirecTV 10/12"
#define NX_UTIL_SAT_SATNAME_AMAZONAS_2						"AMAZONAS 2"
#define NX_UTIL_SAT_SATNAME_AMAZONAS_3						"AMAZONAS 3"
#define NX_UTIL_SAT_SATNAME_AMC_6							"AMC 6"
#define NX_UTIL_SAT_SATNAME_AMC_8							"AMC 8"
#define NX_UTIL_SAT_SATNAME_AMC_15							"AMC 15"
#define NX_UTIL_SAT_SATNAME_AMC_18							"AMC 18"
#define NX_UTIL_SAT_SATNAME_AMC_21							"AMC 21"
#define NX_UTIL_SAT_SATNAME_BSAT_3C_JCSAT_110R				"BSAT 3C JCSAT 110R"
#define NX_UTIL_SAT_SATNAME_N_SAT_110						"N SAT 110"
#define NX_UTIL_SAT_SATNAME_TELSTAR_12_VANTAGE				"TELSTAR 12 VANTAGE"
#define NX_UTIL_SAT_SATNAME_TELSTAR_14R						"TELSTAR 14R"
#define NX_UTIL_SAT_SATNAME_ST_2							"ST 2"
#define NX_UTIL_SAT_SATNAME_SIMON_BOLIVAR					"Simon Bolivar"
#define NX_UTIL_SAT_SATNAME_STAR_ONE_C1						"Star One C1"
#define NX_UTIL_SAT_SATNAME_STAR_ONE_C2						"Star One C2"
#define NX_UTIL_SAT_SATNAME_STAR_ONE_C4						"Star One C4"
#define NX_UTIL_SAT_SATNAME_BRASILSAT_B4					"Brasilsat B4"
#define NX_UTIL_SAT_SATNAME_NSS_10							"NSS 10"

#define	NX_UTIL_SAT_SATNAME_USERDEFINE						"User Define"
#define	NX_UTIL_SAT_SATNAME_USERDEFINE1						"User Define 1"
#define	NX_UTIL_SAT_SATNAME_USERDEFINE2						"User Define 2"
#define	NX_UTIL_SAT_SATNAME_USERDEFINE3						"User Define 3"
#define	NX_UTIL_SAT_SATNAME_USERDEFINE4						"User Define 4"
#define	NX_UTIL_SAT_SATNAME_USERDEFINE5						"User Define 5"
#endif
#define	NX_UTIL_SAT_SATNAME_NONE							"NONE"

#if defined(CONFIG_OP_AMX)
#define NX_UTIL_SAT_SATNAME_STAR_ONE_CENAM					"STAR ONE CENAM"
#endif

/******************************************************************************/
#define _________UTIL_SPEC____________________________________________________
/******************************************************************************/
typedef enum
{
	eNxUtil_HomeMenu_LiveTv = 0,
	eNxUtil_HomeMenu_Guide,
	eNxUtil_HomeMenu_Record,
	eNxUtil_HomeMenu_MediaCenter,
	eNxUtil_HomeMenu_Settings,
	eNxUtil_HomeMenu_TVApps,
	eNxUtil_HomeMenu_Schedule,
	eNxUtil_HomeMenu_Recent,
	eNxUtil_HomeMenu_Shahid,
	// TODO: Add Here
	eNxUtil_HomeMenu_Max
} eNxUiUtilHomeMenuSpec_t;


typedef enum
{
	eNxUtil_StrShort = 0,
	eNxUtil_StrLong,
} eNxUiUtilStrType_t;


typedef enum
{
	eNxUtil_Month_JANUARY = 1,
	eNxUtil_Month_FEBRUARY,
	eNxUtil_Month_MARCH,
	eNxUtil_Month_APRIL,
	eNxUtil_Month_MAY,
	eNxUtil_Month_JUNE,
	eNxUtil_Month_JULY,
	eNxUtil_Month_AUGUST,
	eNxUtil_Month_SEPTEMBER,
	eNxUtil_Month_OCTOBER,
	eNxUtil_Month_NOVEMBER,
	eNxUtil_Month_DECEMBER,
	eNxUtil_Month_MAX
} eNxUiUtilMonth_t;


typedef enum
{
	eNxUtil_NibbleLevel1_Undefined = 0x0,
	eNxUtil_NibbleLevel1_MovieDrama,
	eNxUtil_NibbleLevel1_NewsCurrentAffairs,
	eNxUtil_NibbleLevel1_ShowGameShow,
	eNxUtil_NibbleLevel1_Sports,
	eNxUtil_NibbleLevel1_ChildrensYouth,
	eNxUtil_NibbleLevel1_MusicBalletDance,
	eNxUtil_NibbleLevel1_ArtsCulture,
	eNxUtil_NibbleLevel1_SocialPoliticalEconomics,
	eNxUtil_NibbleLevel1_EducationScienceFactureTopics,
	eNxUtil_NibbleLevel1_LeisureHobbies,
	eNxUtil_NibbleLevel1_SpecialCharacteristics,
	eNxUtil_NibbleLevel1_Reserved1,
	eNxUtil_NibbleLevel1_Reserved2,
	eNxUtil_NibbleLevel1_Reserved3,
	eNxUtil_NibbleLevel1_UserDefined = 0xF,
} eNxUiUtilGenreNibbleLevel1Type_t;


typedef enum
{
	eNxUtil_NibbleLevel2_0x0 = 0x0,
	eNxUtil_NibbleLevel2_0x1,
	eNxUtil_NibbleLevel2_0x2,
	eNxUtil_NibbleLevel2_0x3,
	eNxUtil_NibbleLevel2_0x4,
	eNxUtil_NibbleLevel2_0x5,
	eNxUtil_NibbleLevel2_0x6,
	eNxUtil_NibbleLevel2_0x7,
	eNxUtil_NibbleLevel2_0x8,
	eNxUtil_NibbleLevel2_0x9,
	eNxUtil_NibbleLevel2_0xA,
	eNxUtil_NibbleLevel2_0xB,
	eNxUtil_NibbleLevel2_0xC,
	eNxUtil_NibbleLevel2_0xD,
	eNxUtil_NibbleLevel2_0xE,
	eNxUtil_NibbleLevel2_0xF = 0xF,
} eNxUiUtilGenreNibbleLevel2Type_t;


typedef enum {
	eNxUtil_GMTOffset_m12_00,
	eNxUtil_GMTOffset_m11_00,
	eNxUtil_GMTOffset_m10_00,
	eNxUtil_GMTOffset_m09_00,
	eNxUtil_GMTOffset_m08_00,
	eNxUtil_GMTOffset_m07_00,
	eNxUtil_GMTOffset_m06_00,
	eNxUtil_GMTOffset_m05_00,
	eNxUtil_GMTOffset_m04_00,
	eNxUtil_GMTOffset_m03_30,
	eNxUtil_GMTOffset_m03_00,
	eNxUtil_GMTOffset_m02_00,
	eNxUtil_GMTOffset_m01_00,
	eNxUtil_GMTOffset_p00_00,
	eNxUtil_GMTOffset_p01_00,
	eNxUtil_GMTOffset_p02_00,
	eNxUtil_GMTOffset_p03_00,
	eNxUtil_GMTOffset_p03_30,
	eNxUtil_GMTOffset_p04_00,
	eNxUtil_GMTOffset_p04_30,
	eNxUtil_GMTOffset_p05_00,
	eNxUtil_GMTOffset_p05_30,
	eNxUtil_GMTOffset_p05_45,
	eNxUtil_GMTOffset_p06_00,
	eNxUtil_GMTOffset_p06_30,
	eNxUtil_GMTOffset_p07_00,
	eNxUtil_GMTOffset_p08_00,
	eNxUtil_GMTOffset_p09_00,
	eNxUtil_GMTOffset_p09_30,
	eNxUtil_GMTOffset_p10_00,
	eNxUtil_GMTOffset_p10_30,
	eNxUtil_GMTOffset_p11_00,
	eNxUtil_GMTOffset_p12_00,
	eNxUtil_GMTOffset_p13_00,
	eNxUtil_GMTOffset_MAX,

	eNxUtil_GMTOffset_Auto,
	eNxUtil_GMTOffset_m05_00_m08_00,
} eNxUiUtilGMTOffset_t;

typedef enum
{
	eNxUtil_UI_DispSubttlTypeIcon = 0,
	// TODO: Add Here
	eNxUtil_UI_Max
} eNxUiUtilSpec_t;


typedef enum
{
	eNxUtil_OptItem_None = 0,
	eNxUtil_OptItem_Group,
	eNxUtil_OptItem_Sort,
	eNxUtil_OptItem_AddFav,
	eNxUtil_OptItem_Delete,
	eNxUtil_OptItem_Lock,
	eNxUtil_OptItem_UnLock,
	eNxUtil_OptItem_Rename,
	eNxUtil_OptItem_GenreSearch,
	eNxUtil_OptItem_JumpDate,
	eNxUtil_OptItem_SortTime,
	eNxUtil_OptItem_SortNumber,
	eNxUtil_OptItem_SortName,
	eNxUtil_OptItem_ReSchedule,
	eNxUtil_OptItem_Select,
	eNxUtil_OptItem_SelectAll,
	eNxUtil_OptItem_DeSelectAll,
	eNxUtil_OptItem_Move,
	eNxUtil_OptItem_Remove,
	eNxUtil_OptItem_ChangeFav,
	eNxUtil_OptItem_RenameFav,


	// PVR & Media
	eNxUtil_OptItem_Copy,
	eNxUtil_OptItem_StopCopying,
	eNxUtil_OptItem_StopMoving,
	eNxUtil_OptItem_InternalHDD,
	eNxUtil_OptItem_DetachableHDD,
	eNxUtil_OptItem_USB,
	eNxUtil_OptItem_SdCard,

	// Satellite
	eNxUtil_OptItem_AntennaType,
	eNxUtil_OptItem_AntennaTypeFixed,
	eNxUtil_OptItem_AntennaTypeScd,
	eNxUtil_OptItem_AntennaTypeMotor,

	eNxUtil_OptItem_SearchOpt,

	// Media
	eNxUtil_OptItem_MediaTypeVideo,
	eNxUtil_OptItem_MediaTypePhoto,
	eNxUtil_OptItem_MediaTypeMusic,
	eNxUtil_OptItem_Eject,
	eNxUtil_OptItem_DeleteAll,

	//MBC - keyword search
	eNxUtil_OptItem_KeywordSearch,

	eNxUtil_OptItem_Max
} eNxUiUtil_OptItem_t;


typedef enum
{
	eNxUtil_OptType_TEXT = 0,
	eNxUtil_OptType_IMG_N,
	eNxUtil_OptType_IMG_C,
	eNxUtil_OptType_Max,
} eNxUiUtil_OptType_t;

/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

typedef struct tagOPTION_INFO_LIST
{
	HUINT32		ulOptItemNum;
	HUINT32		aeOptItemList[NX_UTIL_OPTION_MAX_NUM];
} NX_Util_OPTIONList_t;


/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/

/******************************************************************************/
#define _________UTIL_SPEC____________________________________________________
/******************************************************************************/
extern void		NX_PRISM_SPEC_GetHomeMenuUiSupportNum(HUINT32 *ulCount);
extern HBOOL	NX_PRISM_SPEC_GetHomeMenuUiSupport(eNxUiUtilHomeMenuSpec_t eNxHomeMenuSpec);
extern HBOOL	NX_PRISM_SPEC_GetUiSupport(eNxUiUtilSpec_t eNxUiUtilSpec);
extern HBOOL	NX_PRISM_SPEC_GetUiSupportZapperOnly(void);
extern HBOOL	NX_PRISM_SPEC_GetUiSupportPassiveStandby(void);
extern HBOOL	NX_PRISM_SPEC_GetGoPassiveStandby(void);
extern HUINT8	*NX_PRISM_SPEC_GetPanelString(NXUI_PANEL_STR_Type_t ePanelStrType);


/******************************************************************************/
#define _________UTIL_UTIL____________________________________________________
/******************************************************************************/
extern HERROR	NX_PRISM_UTIL_ConvertDecStrToInt (HUINT8 *szString, HUINT32 *pulValue);
extern HINT8	*NX_PRISM_UTIL_GetResStringIdByWeekIndex(HUINT32 ulWeekIndex);
extern HINT8	*NX_PRISM_UTIL_GetResStringIdByMonth(HINT32 lMonth);
extern HERROR	NX_PRISM_UTIL_GetDateString(eNxUiUtilStrType_t eType, HUINT8 ucDate, HUINT8 **ppszDateString64);

extern HINT8	*NX_PRISM_UTIL_GetGenreNibbleLevel1String(HUINT8 ucNibbleLevel1);
extern HINT8	*NX_PRISM_UTIL_GetGenreNibbleLevel2String(HUINT8 ucNibbleLevel1, HUINT8 ucNibbleLevel2);

extern HCHAR 	*NX_PRISM_UTIL_GetSubtitleResource(HINT32 lSubtitleType);
extern HCHAR	*NX_PRISM_UTIL_GetCasTypeResource(HINT32 lCasType);
extern HCHAR	*NX_PRISM_UTIL_GetRatingResource(HINT32 lRating);
extern HCHAR	*NX_PRISM_UTIL_GetDolbyIconResource(DxAudioCodec_e eCodec);
extern HCHAR	*NX_PRISM_UTIL_GetSubtitleIconResource(OxMediaPlay_SubtitleType_e eSubtitleType);
extern HCHAR	*NX_PRISM_UTIL_GetReservationResource(DxRsvType_e eRsvType);

extern HERROR 	NX_PRISM_UTIL_FindNext16_9_DisplayFormat(HINT32 eDispFormat, HINT32 *eChgDispFormat);
extern HERROR	NX_PRISM_UTIL_FindNext4_3_DisplayFormat(HINT32 eDispFormat, HINT32 *eChgDispFormat);
extern HERROR 	NX_PRISM_UTIL_FindNextVideoFormatByRatioDisplayFormat(HINT32 eScreenRatio, HINT32 eDispFormat, HINT32 *eChgDispFormat);

extern HCHAR	*NX_PRISM_UTIL_GetLangFullNameBy639Code(HCHAR *pszString);
extern HINT8	*NX_PRISM_UTIL_GetResStringIdByLangId(HxLANG_Id_e eLangId);
extern HCHAR	*NX_PRISM_UTIL_GetResStringIdByLangCode(HCHAR *pszLangCode);

extern ONDK_Result_t	NX_PRISM_UTIL_MakeSubtitleString(NX_Media_SBTList_t stSubtList, HUINT32 ulUserSelectIdx, HCHAR *pszStrBuf);
extern ONDK_Result_t	NX_PRISM_UTIL_MakeAudioString(NX_Media_AUDIOList_t stAudioList, HUINT32 ulUserSelectIdx, HCHAR *pszStrBuf, HBOOL bDulaMonoFullName);
extern ONDK_Result_t	NX_PRISM_UTIL_MakeAudioStringWithDualMonoFullName(NX_Media_AUDIOList_t stAudioList, HUINT32 ulUserSelectIdx, HCHAR *pszStrBuf);
extern ONDK_Result_t	NX_PRISM_UTIL_InitNUllPIDValue(NX_Media_AUDIOList_t *stAudioList);
extern HERROR	NX_PRISM_UTIL_GetResStringIdByChannelListGroupType(NX_ChListGroup_t eChGroupType, HCHAR *pszString);
extern HCHAR	*NX_PRISM_UTIL_ConvSatelliteNameToDispSatelliteName(HCHAR *szAntennaName);
extern void		NX_PRISM_UTIL_MakeTpStringByTuneParamAndTunerType(DxDeliveryType_e eTunerType, DxTuneParam_t *pstTp, HUINT8 *pString);
extern void		NX_PRISM_UTIL_MakeTpStringByTuneParam(DxTuneParam_t *pstTp, HUINT8 *pString);
extern GWM_UiState_t NX_PRISM_UTIL_GetUiState (void);

extern HINT8	*NX_PRISM_UTIL_GetResStringIdByResolution(DxResolutionSelect_e eResolution);
extern HINT8	*NX_PRISM_UTIL_GetResStringIdByCountryID(HxCountry_e eCountryId);
extern void		NX_PRISM_UTIL_GetGmtOffsetByCountryID(HxCountry_e eCountryId, HINT32 *plHour, HINT32 *plMinute);
extern HBOOL	NX_PRISM_UTIL_IsSupportResolutionList(DxResolutionSelect_e eResolution);
extern HERROR	NX_PRISM_UTIL_GetGMTOffsetByTime(HINT32 lTime, eNxUiUtilGMTOffset_t *peGMTOffset);
extern HERROR	NX_PRISM_UTIL_GetTimeByGMTOffset(eNxUiUtilGMTOffset_t eGMTOffset, HINT32 *plHour, HINT32 *plMin);
extern HINT8	*NX_PRISM_UTIL_GetResStringIdByGMTOffset(eNxUiUtilGMTOffset_t eGMTOffset);
extern void		NX_PRISM_UTIL_GetMakeChannelBerValue (HINT32 lBer, HINT32 *plDecimal, HINT32 *plFloat, HINT32 *plExp);
extern void		NX_PRISM_UTIL_GetMakeChannelBerBase (HINT32 lBer, HINT32 *plDecimal, HINT32 *plFloat, HINT32 *plExp);
extern void		NX_PRISM_UTIL_GetMakeChannelBerValueForAMX (HINT32 lBer, HINT32 *plDecimal, HINT32 *plFloat, HINT32 *plExp);

extern void		NX_PRISM_UTIL_OptionDeleteItemTextList(NX_Util_OPTIONList_t *pstOptItemList, HUINT8 **ppucTextArray);
extern HUINT8	**NX_PRISM_UTIL_OptionMakeFavouriteItemTextList(eNxUiUtil_OptType_t ulItemType, NX_Util_OPTIONList_t *pstOptItemList);
extern HUINT8	**NX_PRISM_UTIL_OptionMakeChannlGroupItemTextList(eNxUiUtil_OptType_t ulItemType, NX_Util_OPTIONList_t *pstOptItemList);
extern HUINT8	**NX_PRISM_UTIL_OptionMakeItemTextList(eNxUiUtil_OptType_t ulItemType, NX_Util_OPTIONList_t *pstOptItemList);
extern void		NX_PRISM_UTIL_OptionAddItemList(NX_Util_OPTIONList_t *pstOptItemList, HUINT32 ulOptItemId);
extern HINT32	NX_PRISM_UTIL_GetTextLineCount(ONDK_FontId_t eFontId, HUINT32 ulFontSize, HUINT8 *pszMessage, HUINT32 w);
extern void** 	NX_PRISM_UTIL_ArraySlice(void** ppSrcArray, HUINT32 ulSizeOfType, HINT32 lStart, HINT32 lEnd);

extern void		NX_PRISM_UTIL_KeyConv_UiCommon(HINT32 *keyCode);
extern void		NX_PRISM_UTIL_KeyConv_MenuRoot(HINT32 *keyCode);

#endif // __NX_PRISM_UTIL_H__

