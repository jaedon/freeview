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
 * @file	  		nx_prism_util.c
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
#include <nx_core.h>
#include <nx_prism_app.h>
#include <nx_prism_util.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_UTIL_DTI_MAX				11
#define	NX_UTIL_WEEKDAY				7


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static HINT8	*nx_prism_util_GetGenreNibbleLevel2_MovieDramaString(HUINT8 ucNibbleLevel2);
static HINT8	*nx_prism_util_GetGenreNibbleLevel2_NewsCurrentAffairsString(HUINT8 ucNibbleLevel2);
static HINT8	*nx_prism_util_GetGenreNibbleLevel2_ShowGameShowString(HUINT8 ucNibbleLevel2);
static HINT8	*nx_prism_util_GetGenreNibbleLevel2_SportsString(HUINT8 ucNibbleLevel2);
static HINT8	*nx_prism_util_GetGenreNibbleLevel2_ChildrensYouthString(HUINT8 ucNibbleLevel2);
static HINT8	*nx_prism_util_GetGenreNibbleLevel2_MusicBalletDanceString(HUINT8 ucNibbleLevel2);
static HINT8	*nx_prism_util_GetGenreNibbleLevel2_ArtsCultureString(HUINT8 ucNibbleLevel2);
static HINT8	*nx_prism_util_GetGenreNibbleLevel2_SocialPoliticalEconomicsString(HUINT8 ucNibbleLevel2);
static HINT8	*nx_prism_util_GetGenreNibbleLevel2_EducationScienceFactureTopicsString(HUINT8 ucNibbleLevel2);
static HINT8	*nx_prism_util_GetGenreNibbleLevel2_LeisureHobbiesString(HUINT8 ucNibbleLevel2);
static HINT8	*nx_prism_util_GetGenreNibbleLevel2_SpecialCharacteristicsString(HUINT8 ucNibbleLevel2);

static HCHAR	*nx_prism_util_OptionItemResource(eNxUiUtil_OptType_t eNxOptType, eNxUiUtil_OptItem_t eNxOptItem);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static HINT8	*nx_prism_util_GetGenreNibbleLevel2_MovieDramaString(HUINT8 ucNibbleLevel2)
{
	NX_APP_Info("ucNibbleLevel2(0x%x)\n", ucNibbleLevel2);

	switch (ucNibbleLevel2)
	{
		case eNxUtil_NibbleLevel2_0x0:		return	RES_GENRE_MOVIE_DRAMA_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x1:		return	RES_GENRE_MOVIE_DETECTIVE_THRILER_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x2:		return	RES_GENRE_MOVIE_ADVENTURE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x3:		return	RES_GENRE_MOVIE_SCIENCE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x4:		return	RES_GENRE_MOVIE_COMEDY_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x5:		return	RES_GENRE_MOVIE_SOAP_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x6:		return	RES_GENRE_MOVIE_ROMANCE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x7:		return	RES_GENRE_MOVIE_SERIOUS_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x8:		return	RES_GENRE_MOVIE_ADULT_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x9:
		case eNxUtil_NibbleLevel2_0xA:
		case eNxUtil_NibbleLevel2_0xB:
		case eNxUtil_NibbleLevel2_0xC:
		case eNxUtil_NibbleLevel2_0xD:
		case eNxUtil_NibbleLevel2_0xE:
		case eNxUtil_NibbleLevel2_0xF:
		default:
			return	NULL;
	}

}


static HINT8	*nx_prism_util_GetGenreNibbleLevel2_NewsCurrentAffairsString(HUINT8 ucNibbleLevel2)
{
	NX_APP_Info("ucNibbleLevel2(0x%x)\n", ucNibbleLevel2);

	switch (ucNibbleLevel2)
	{
		case eNxUtil_NibbleLevel2_0x0:		return	RES_GENRE_NEWS_CURRENT_AFFAIRS_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x1:		return	RES_GENRE_NEWS_WEATHER_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x2:		return	RES_GENRE_NEWS_MAGAZINE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x3:		return	RES_GENRE_NEWS_DOCUMENTARY_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x4:		return	RES_GENRE_NEWS_DISCUSSION_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x5:
		case eNxUtil_NibbleLevel2_0x6:
		case eNxUtil_NibbleLevel2_0x7:
		case eNxUtil_NibbleLevel2_0x8:
		case eNxUtil_NibbleLevel2_0x9:
		case eNxUtil_NibbleLevel2_0xA:
		case eNxUtil_NibbleLevel2_0xB:
		case eNxUtil_NibbleLevel2_0xC:
		case eNxUtil_NibbleLevel2_0xD:
		case eNxUtil_NibbleLevel2_0xE:
		case eNxUtil_NibbleLevel2_0xF:
		default:
			return	NULL;
	}

}


static HINT8	*nx_prism_util_GetGenreNibbleLevel2_ShowGameShowString(HUINT8 ucNibbleLevel2)
{
	NX_APP_Info("ucNibbleLevel2(0x%x)\n", ucNibbleLevel2);

	switch (ucNibbleLevel2)
	{
		case eNxUtil_NibbleLevel2_0x0:		return	RES_GENRE_SHOWS_SHOW_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x1:		return	RES_GENRE_SHOWS_GAME_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x2:		return	RES_GENRE_SHOWS_VARIETY_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x3:		return	RES_GENRE_SHOWS_TALKSHOW_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x4:
		case eNxUtil_NibbleLevel2_0x5:
		case eNxUtil_NibbleLevel2_0x6:
		case eNxUtil_NibbleLevel2_0x7:
		case eNxUtil_NibbleLevel2_0x8:
		case eNxUtil_NibbleLevel2_0x9:
		case eNxUtil_NibbleLevel2_0xA:
		case eNxUtil_NibbleLevel2_0xB:
		case eNxUtil_NibbleLevel2_0xC:
		case eNxUtil_NibbleLevel2_0xD:
		case eNxUtil_NibbleLevel2_0xE:
		case eNxUtil_NibbleLevel2_0xF:
		default:
			return	NULL;
	}

}


static HINT8	*nx_prism_util_GetGenreNibbleLevel2_SportsString(HUINT8 ucNibbleLevel2)
{
	NX_APP_Info("ucNibbleLevel2(0x%x)\n", ucNibbleLevel2);

	switch (ucNibbleLevel2)
	{
		case eNxUtil_NibbleLevel2_0x0:		return	RES_GENRE_SPORTS_SPORTS_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x1:		return	RES_GENRE_SPORTS_SPECIAL_EVENTS_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x2:		return	RES_GENRE_SPORTS_MAGAZINE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x3:		return	RES_GENRE_SPORTS_FOOTBALL_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x4:		return	RES_GENRE_SPORTS_TENNIS_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x5:		return	RES_GENRE_SPORTS_TEAM_SPORTS_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x6:		return	RES_GENRE_SPORTS_ATHLETICS_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x7:		return	RES_GENRE_SPORTS_MOTOR_SPORT_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x8:		return	RES_GENRE_SPORTS_WATER_SPORT_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x9:		return	RES_GENRE_SPORTS_WINTER_SPORT_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0xA:		return	RES_GENRE_SPORTS_EQUESRIAN_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0xB:		return	RES_GENRE_SPORTS_MARTIAL_SPORT_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0xC:
		case eNxUtil_NibbleLevel2_0xD:
		case eNxUtil_NibbleLevel2_0xE:
		case eNxUtil_NibbleLevel2_0xF:
		default:
			return	NULL;
	}

}


static HINT8	*nx_prism_util_GetGenreNibbleLevel2_ChildrensYouthString(HUINT8 ucNibbleLevel2)
{
	NX_APP_Info("ucNibbleLevel2(0x%x)\n", ucNibbleLevel2);

	switch (ucNibbleLevel2)
	{
		case eNxUtil_NibbleLevel2_0x0:		return	RES_GENRE_CHILD_YOUTH_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x1:		return	RES_GENRE_CHILD_PRESCHOOL_CHILD_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x2:		return	RES_GENRE_CHILD_ENTERTAINMENT_FOR_6_TO_14_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x3:		return	RES_GENRE_CHILD_ENTERTAINMENT_FOR_10_TO_16_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x4:		return	RES_GENRE_CHILD_INFORMATION_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x5:		return	RES_GENRE_CHILD_CARTOONS_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x6:
		case eNxUtil_NibbleLevel2_0x7:
		case eNxUtil_NibbleLevel2_0x8:
		case eNxUtil_NibbleLevel2_0x9:
		case eNxUtil_NibbleLevel2_0xA:
		case eNxUtil_NibbleLevel2_0xB:
		case eNxUtil_NibbleLevel2_0xC:
		case eNxUtil_NibbleLevel2_0xD:
		case eNxUtil_NibbleLevel2_0xE:
		case eNxUtil_NibbleLevel2_0xF:
		default:
			return	NULL;
	}

}


static HINT8	*nx_prism_util_GetGenreNibbleLevel2_MusicBalletDanceString(HUINT8 ucNibbleLevel2)
{
	NX_APP_Info("ucNibbleLevel2(0x%x)\n", ucNibbleLevel2);

	switch (ucNibbleLevel2)
	{
		case eNxUtil_NibbleLevel2_0x0:		return	RES_GENRE_MUSIC_DANCE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x1:		return	RES_GENRE_MUSIC_ROCK_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x2:		return	RES_GENRE_MUSIC_CLASSIC_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x3:		return	RES_GENRE_MUSIC_FOLK_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x4:		return	RES_GENRE_MUSIC_JAZZ_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x5:		return	RES_GENRE_MUSIC_MUSICAL_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x6:		return	RES_GENRE_MUSIC_BALLET_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x7:
		case eNxUtil_NibbleLevel2_0x8:
		case eNxUtil_NibbleLevel2_0x9:
		case eNxUtil_NibbleLevel2_0xA:
		case eNxUtil_NibbleLevel2_0xB:
		case eNxUtil_NibbleLevel2_0xC:
		case eNxUtil_NibbleLevel2_0xD:
		case eNxUtil_NibbleLevel2_0xE:
		case eNxUtil_NibbleLevel2_0xF:
		default:
			return	NULL;
	}

}


static HINT8	*nx_prism_util_GetGenreNibbleLevel2_ArtsCultureString(HUINT8 ucNibbleLevel2)
{
	NX_APP_Info("ucNibbleLevel2(0x%x)\n", ucNibbleLevel2);

	switch (ucNibbleLevel2)
	{
		case eNxUtil_NibbleLevel2_0x0:		return	RES_GENRE_ARTS_CULTURE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x1:		return	RES_GENRE_ARTS_PERFORMING_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x2:		return	RES_GENRE_ARTS_FINE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x3:		return	RES_GENRE_ARTS_RELIGION_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x4:		return	RES_GENRE_ARTS_POPULAR_CULTURE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x5:		return	RES_GENRE_ARTS_LITERATURE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x6:		return	RES_GENRE_ARTS_FILM_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x7:		return	RES_GENRE_ARTS_VIDEO_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x8:		return	RES_GENRE_ARTS_PRESS_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x9:		return	RES_GENRE_ARTS_NEW_MEDIA_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0xA:		return	RES_GENRE_ARTS_CULTURE_MAGAZINE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0xB:		return	RES_GENRE_ARTS_FASHION_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0xC:
		case eNxUtil_NibbleLevel2_0xD:
		case eNxUtil_NibbleLevel2_0xE:
		case eNxUtil_NibbleLevel2_0xF:
		default:
			return	NULL;
	}

}


static HINT8	*nx_prism_util_GetGenreNibbleLevel2_SocialPoliticalEconomicsString(HUINT8 ucNibbleLevel2)
{
	NX_APP_Info("ucNibbleLevel2(0x%x)\n", ucNibbleLevel2);

	switch (ucNibbleLevel2)
	{
		case eNxUtil_NibbleLevel2_0x0:		return	RES_GENRE_SOCIETY_SOCIAL_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x1:		return	RES_GENRE_SOCIETY_REPORTS_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x2:		return	RES_GENRE_SOCIETY_ECONOMICS_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x3:		return	RES_GENRE_SOCIETY_REMARKABLE_PEOPLE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x4:
		case eNxUtil_NibbleLevel2_0x5:
		case eNxUtil_NibbleLevel2_0x6:
		case eNxUtil_NibbleLevel2_0x7:
		case eNxUtil_NibbleLevel2_0x8:
		case eNxUtil_NibbleLevel2_0x9:
		case eNxUtil_NibbleLevel2_0xA:
		case eNxUtil_NibbleLevel2_0xB:
		case eNxUtil_NibbleLevel2_0xC:
		case eNxUtil_NibbleLevel2_0xD:
		case eNxUtil_NibbleLevel2_0xE:
		case eNxUtil_NibbleLevel2_0xF:
		default:
			return	NULL;
	}

}


static HINT8	*nx_prism_util_GetGenreNibbleLevel2_EducationScienceFactureTopicsString(HUINT8 ucNibbleLevel2)
{
	NX_APP_Info("ucNibbleLevel2(0x%x)\n", ucNibbleLevel2);

	switch (ucNibbleLevel2)
	{
		case eNxUtil_NibbleLevel2_0x0:		return	RES_GENRE_EDU_SCIENCE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x1:		return	RES_GENRE_EDU_NATURE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x2:		return	RES_GENRE_EDU_TECHNOLOGY_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x3:		return	RES_GENRE_EDU_MEDICINE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x4:		return	RES_GENRE_EDU_FOREIGN_COUNTRY_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x5:		return	RES_GENRE_EDU_SPIRITUAL_SCIENCE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x6:		return	RES_GENRE_EDU_FURTHER_EDU_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x7:		return	RES_GENRE_EDU_LANGUAGE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x8:
		case eNxUtil_NibbleLevel2_0x9:
		case eNxUtil_NibbleLevel2_0xA:
		case eNxUtil_NibbleLevel2_0xB:
		case eNxUtil_NibbleLevel2_0xC:
		case eNxUtil_NibbleLevel2_0xD:
		case eNxUtil_NibbleLevel2_0xE:
		case eNxUtil_NibbleLevel2_0xF:
		default:
			return	NULL;
	}

}


static HINT8	*nx_prism_util_GetGenreNibbleLevel2_LeisureHobbiesString(HUINT8 ucNibbleLevel2)
{
	NX_APP_Info("ucNibbleLevel2(0x%x)\n", ucNibbleLevel2);

	switch (ucNibbleLevel2)
	{
		case eNxUtil_NibbleLevel2_0x0:		return	RES_GENRE_LEISURE_HOBBY_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x1:		return	RES_GENRE_LEISURE_TRAVEL_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x2:		return	RES_GENRE_LEISURE_HANDICRAFT_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x3:		return	RES_GENRE_LEISURE_MOTORING_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x4:		return	RES_GENRE_LEISURE_FITNESS_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x5:		return	RES_GENRE_LEISURE_COOKING_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x6:		return	RES_GENRE_LEISURE_SHOPPING_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x7:		return	RES_GENRE_LEISURE_GARDENING_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x8:
		case eNxUtil_NibbleLevel2_0x9:
		case eNxUtil_NibbleLevel2_0xA:
		case eNxUtil_NibbleLevel2_0xB:
		case eNxUtil_NibbleLevel2_0xC:
		case eNxUtil_NibbleLevel2_0xD:
		case eNxUtil_NibbleLevel2_0xE:
		case eNxUtil_NibbleLevel2_0xF:
		default:
			return	NULL;
	}

}


static HINT8	*nx_prism_util_GetGenreNibbleLevel2_SpecialCharacteristicsString(HUINT8 ucNibbleLevel2)
{
	NX_APP_Info("ucNibbleLevel2(0x%x)\n", ucNibbleLevel2);

	switch (ucNibbleLevel2)
	{
		case eNxUtil_NibbleLevel2_0x0:		return	RES_GENRE_SPECIALS_ORIGIANL_LANGUAGE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x1:		return	RES_GENRE_SPECIALS_BLACK_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x2:		return	RES_GENRE_SPECIALS_UNPUBLISHED_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x3:		return	RES_GENRE_SPECIALS_LIVE_ELEMENT_ID;
		case eNxUtil_NibbleLevel2_0x4:
		case eNxUtil_NibbleLevel2_0x5:
		case eNxUtil_NibbleLevel2_0x6:
		case eNxUtil_NibbleLevel2_0x7:
		case eNxUtil_NibbleLevel2_0x8:
		case eNxUtil_NibbleLevel2_0x9:
		case eNxUtil_NibbleLevel2_0xA:
		case eNxUtil_NibbleLevel2_0xB:
		case eNxUtil_NibbleLevel2_0xC:
		case eNxUtil_NibbleLevel2_0xD:
		case eNxUtil_NibbleLevel2_0xE:
		case eNxUtil_NibbleLevel2_0xF:
		default:
			return	NULL;
	}

}


static HCHAR	*nx_prism_util_OptionItemResource(eNxUiUtil_OptType_t eNxOptType, eNxUiUtil_OptItem_t eNxOptItem)
{
	HCHAR		*pszResource = NULL;

	switch (eNxOptType)
	{
		case eNxUtil_OptType_TEXT:
			switch (eNxOptItem)
			{
				case eNxUtil_OptItem_Group:			pszResource = (HCHAR*)ONDK_GetString(RES_CHANGE_GROUP_01_ID);		break;
				case eNxUtil_OptItem_Sort:			pszResource = (HCHAR*)ONDK_GetString(RES_SORT_CHANNELS_01_ID);		break;
				case eNxUtil_OptItem_AddFav:		pszResource = (HCHAR*)ONDK_GetString(RES_ADD_TO_FAVOURITE_01_ID);	break;
				case eNxUtil_OptItem_Delete:		pszResource = (HCHAR*)ONDK_GetString(RES_DELETE_02_ID);				break;
				case eNxUtil_OptItem_Lock:			pszResource = (HCHAR*)ONDK_GetString(RES_LOCK_08_ID);				break;
				case eNxUtil_OptItem_UnLock:		pszResource = (HCHAR*)ONDK_GetString(RES_UNLOCK_04_ID);				break;
				case eNxUtil_OptItem_Rename:		pszResource = (HCHAR*)ONDK_GetString(RES_RENAME_08_ID);				break;
				case eNxUtil_OptItem_GenreSearch:	pszResource = (HCHAR*)ONDK_GetString(RES_GENRE_SEARCH_ID);			break;
				case eNxUtil_OptItem_JumpDate:		pszResource = (HCHAR*)ONDK_GetString(RES_JUMP_TO_DATE_01_ID);		break;
				case eNxUtil_OptItem_SortTime:		pszResource = (HCHAR*)ONDK_GetString(RES_BY_AIRING_TIME_ID);		break;
				case eNxUtil_OptItem_SortNumber:	pszResource = (HCHAR*)ONDK_GetString(RES_BY_CHANNEL_NUMBER_ID);		break;
				case eNxUtil_OptItem_SortName:		pszResource = (HCHAR*)ONDK_GetString(RES_BY_CHANNEL_NAME_ID);		break;
				case eNxUtil_OptItem_ReSchedule:	pszResource = (HCHAR*)ONDK_GetString(RES_RESCHEDULE_ID);			break;
				case eNxUtil_OptItem_Select:		pszResource = (HCHAR*)ONDK_GetString(RES_SELECT_ID);				break;
				case eNxUtil_OptItem_SelectAll:		pszResource = (HCHAR*)ONDK_GetString(RES_SELECT_ALL_02_ID);			break;
				case eNxUtil_OptItem_DeSelectAll:	pszResource = (HCHAR*)ONDK_GetString(RES_DESELECT_ALL_01_ID);		break;
				case eNxUtil_OptItem_Move:			pszResource = (HCHAR*)ONDK_GetString(RES_MOVE_01_ID);				break;
				case eNxUtil_OptItem_Remove:		pszResource = (HCHAR*)ONDK_GetString(RES_REMOVE_ID);
					break;
				case eNxUtil_OptItem_ChangeFav:		pszResource = (HCHAR*)ONDK_GetString(RES_CHANGE_FAVOURITE_02_ID);
					break;
				case eNxUtil_OptItem_RenameFav:		pszResource = (HCHAR*)ONDK_GetString(RES_RENAME_FAVOURITE_01_ID);
					break;

#if defined(CONFIG_MW_MM_PVR) || defined(CONFIG_MW_MM_MEDIA)
				case eNxUtil_OptItem_Copy:			pszResource = (HCHAR*)ONDK_GetString(RES_COPY_ID);					break;
				case eNxUtil_OptItem_StopCopying:	pszResource = (HCHAR*)ONDK_GetString(RES_STOP_COPYING_ID);			break;
				case eNxUtil_OptItem_StopMoving:	pszResource = (HCHAR*)ONDK_GetString(RES_STOP_MOVING_ID);			break;
#endif
#if defined(CONFIG_MW_MM_MEDIA)
				case eNxUtil_OptItem_MediaTypeVideo:	pszResource = (HCHAR*)ONDK_GetString(RES_VIDEO_03_ID);			break;
				case eNxUtil_OptItem_MediaTypePhoto:	pszResource = (HCHAR*)ONDK_GetString(RES_PHOTO_01_ID);			break;
				case eNxUtil_OptItem_MediaTypeMusic:	pszResource = (HCHAR*)ONDK_GetString(RES_MUSIC_01_ID);			break;
				case eNxUtil_OptItem_Eject:				pszResource = (HCHAR*)ONDK_GetString(RES_EJECT_ID);				break;
				case eNxUtil_OptItem_DeleteAll:			pszResource = (HCHAR*)ONDK_GetString(RES_DELETE_ALL_FILES_ID);	break;
#endif
#if defined(CONFIG_MW_CH_SATELLITE)
				case eNxUtil_OptItem_AntennaType:		pszResource = (HCHAR*)ONDK_GetString(RES_ANTENNA_TYPE_ID);		break;
				case eNxUtil_OptItem_AntennaTypeFixed:	pszResource = (HCHAR*)ONDK_GetString(RES_FIXED_ANTENNA_ID);		break;
				case eNxUtil_OptItem_AntennaTypeScd:	pszResource = (HCHAR*)ONDK_GetString(RES_SCD_ANTENNA_ID);		break;
				case eNxUtil_OptItem_AntennaTypeMotor:	pszResource = (HCHAR*)ONDK_GetString(RES_MOTOR_ANTENNA_ID);		break;
#endif
				case eNxUtil_OptItem_SearchOpt:			pszResource = (HCHAR*)ONDK_GetString(RES_SEARCH_OPTION_ID);		break;

				case eNxUtil_OptItem_KeywordSearch:		pszResource = (HCHAR*)ONDK_GetString(RES_KEYWORD_SEARCH_ID);	break;

				default:								pszResource = NULL;												break;
			}
			break;
		case eNxUtil_OptType_IMG_N:
			switch (eNxOptItem)
			{
				case eNxUtil_OptItem_Group:			pszResource = RES_216_00_OPT_GROUP_CHANGE_N_PNG;	break;
				case eNxUtil_OptItem_Sort:			pszResource = RES_216_00_OPT_SORT_N_PNG;			break;
				case eNxUtil_OptItem_AddFav:		pszResource = RES_216_00_OPT_FAV_ADD_N_PNG;			break;
				case eNxUtil_OptItem_Delete:		pszResource = RES_216_00_OPT_DELETE_N_PNG;			break;
				case eNxUtil_OptItem_Lock:			pszResource = RES_216_00_OPT_LOCK_N_PNG;			break;
				case eNxUtil_OptItem_UnLock:		pszResource = RES_216_00_OPT_LOCK_N_PNG;			break;
				case eNxUtil_OptItem_Rename:		pszResource = RES_216_00_OPT_RENAME_N_PNG;			break;
				case eNxUtil_OptItem_GenreSearch:	pszResource = RES_216_00_OPT_SEARCH_N_PNG;			break;
				case eNxUtil_OptItem_JumpDate:		pszResource = RES_216_00_OPT_DAY_N_PNG;				break;
				case eNxUtil_OptItem_ReSchedule:	pszResource = RES_216_00_OPT_RESCHEDULE_N_PNG;		break;
				case eNxUtil_OptItem_Select:		pszResource = RES_216_00_OPT_SEL_N_PNG;				break;
				case eNxUtil_OptItem_SelectAll: 	pszResource = RES_216_00_OPT_SEL_ALL_N_PNG;			break;
				case eNxUtil_OptItem_DeSelectAll: 	pszResource = RES_216_00_OPT_DESEL_ALL_N_PNG;		break;
				case eNxUtil_OptItem_Move:			pszResource = RES_216_00_OPT_MOVE01_N_PNG;			break;
				case eNxUtil_OptItem_Remove:		pszResource = RES_216_00_OPT_FAV_REMOVE_N_PNG;		break;
				case eNxUtil_OptItem_ChangeFav:		pszResource = RES_216_00_OPT_FAV_CHANGE_N_PNG;		break;
				case eNxUtil_OptItem_RenameFav:		pszResource = RES_216_00_OPT_FAV_RENAME_N_PNG;		break;
#if defined(CONFIG_MW_MM_PVR) || defined(CONFIG_MW_MM_MEDIA)
				case eNxUtil_OptItem_Copy:			pszResource = RES_216_00_OPT_COPY_N_PNG;			break;
				case eNxUtil_OptItem_InternalHDD:	pszResource = RES_214_00_STORAGE_L_IN_N_PNG;		break;
				case eNxUtil_OptItem_DetachableHDD: pszResource = RES_214_00_STORAGE_L_ESATA_N_PNG;		break;
				case eNxUtil_OptItem_USB:			pszResource = RES_214_00_STORAGE_L_USB_N_PNG;		break;
				case eNxUtil_OptItem_SdCard:		pszResource = RES_214_00_STORAGE_L_SD_N_PNG;		break;
#endif
#if defined(CONFIG_MW_MM_MEDIA)
				case eNxUtil_OptItem_MediaTypeVideo:	pszResource = RES_427_05_ME_USB_VIDEO_N_PNG;	break;
				case eNxUtil_OptItem_MediaTypePhoto:	pszResource = RES_427_05_ME_USB_PHOTO_N_PNG;	break;
				case eNxUtil_OptItem_MediaTypeMusic:	pszResource = RES_427_05_ME_USB_MUSIC_N_PNG;	break;
				case eNxUtil_OptItem_Eject:				pszResource = RES_216_00_OPT_FAV_REMOVE_N_PNG; 	break;
				case eNxUtil_OptItem_DeleteAll:			pszResource = RES_216_00_OPT_DELETE_N_PNG;		break;
#endif
#if defined(CONFIG_MW_CH_SATELLITE)
				case eNxUtil_OptItem_AntennaType:		pszResource = RES_216_01_OPT_ANTE_N_PNG;		break;
				case eNxUtil_OptItem_AntennaTypeFixed:	pszResource = RES_431_01_SET_POP_FIXED_N_PNG;	break;
				case eNxUtil_OptItem_AntennaTypeScd:	pszResource = RES_431_01_SET_POP_SCD_N_PNG;		break;
				case eNxUtil_OptItem_AntennaTypeMotor:	pszResource = RES_431_01_SET_POP_MOTOR_N_PNG;	break;
#endif
				case eNxUtil_OptItem_SearchOpt: 		pszResource = RES_216_00_OPT_SEARCH_N_PNG; 		break;

				case eNxUtil_OptItem_KeywordSearch:		pszResource = RES_216_00_OPT_SEARCH_N_PNG;		break;

				default:								pszResource = RES_NONE_PNG;						break;
			}
			break;
		case eNxUtil_OptType_IMG_C:
			switch (eNxOptItem)
			{
				case eNxUtil_OptItem_Group:			pszResource = RES_216_00_OPT_GROUP_CHANGE_C_PNG;	break;
				case eNxUtil_OptItem_Sort:			pszResource = RES_216_00_OPT_SORT_C_PNG;			break;
				case eNxUtil_OptItem_AddFav:		pszResource = RES_216_00_OPT_FAV_ADD_C_PNG;			break;
				case eNxUtil_OptItem_Delete:		pszResource = RES_216_00_OPT_DELETE_C_PNG;			break;
				case eNxUtil_OptItem_Lock:			pszResource = RES_216_00_OPT_LOCK_C_PNG;			break;
				case eNxUtil_OptItem_UnLock:		pszResource = RES_216_00_OPT_LOCK_C_PNG;			break;
				case eNxUtil_OptItem_Rename:		pszResource = RES_216_00_OPT_RENAME_C_PNG;			break;
				case eNxUtil_OptItem_GenreSearch:	pszResource = RES_216_00_OPT_SEARCH_C_PNG;			break;
				case eNxUtil_OptItem_JumpDate:		pszResource = RES_216_00_OPT_DAY_C_PNG;				break;
				case eNxUtil_OptItem_ReSchedule:	pszResource = RES_216_00_OPT_RESCHEDULE_C_PNG;		break;
				case eNxUtil_OptItem_Select:		pszResource = RES_216_00_OPT_SEL_C_PNG;				break;
				case eNxUtil_OptItem_SelectAll: 	pszResource = RES_216_00_OPT_SEL_ALL_C_PNG;			break;
				case eNxUtil_OptItem_DeSelectAll: 	pszResource = RES_216_00_OPT_DESEL_ALL_C_PNG;		break;
				case eNxUtil_OptItem_Move:			pszResource = RES_216_00_OPT_MOVE01_C_PNG;			break;
				case eNxUtil_OptItem_Remove:		pszResource = RES_216_00_OPT_FAV_REMOVE_C_PNG;		break;
				case eNxUtil_OptItem_ChangeFav:		pszResource = RES_216_00_OPT_FAV_CHANGE_C_PNG;		break;
				case eNxUtil_OptItem_RenameFav:		pszResource = RES_216_00_OPT_FAV_RENAME_C_PNG;		break;
#if defined(CONFIG_MW_MM_PVR) || defined(CONFIG_MW_MM_MEDIA)
				case eNxUtil_OptItem_Copy:			pszResource = RES_216_00_OPT_COPY_C_PNG;			break;
				case eNxUtil_OptItem_InternalHDD:	pszResource = RES_214_00_STORAGE_L_IN_C_PNG;		break;
				case eNxUtil_OptItem_DetachableHDD: pszResource = RES_214_00_STORAGE_L_ESATA_C_PNG;		break;
				case eNxUtil_OptItem_USB:			pszResource = RES_214_00_STORAGE_L_USB_C_PNG;		break;
				case eNxUtil_OptItem_SdCard:		pszResource = RES_214_00_STORAGE_L_SD_C_PNG;		break;
#endif
#if defined(CONFIG_MW_MM_MEDIA)
				case eNxUtil_OptItem_MediaTypeVideo:	pszResource = RES_427_05_ME_USB_VIDEO_C_PNG;	break;
				case eNxUtil_OptItem_MediaTypePhoto:	pszResource = RES_427_05_ME_USB_PHOTO_C_PNG;	break;
				case eNxUtil_OptItem_MediaTypeMusic:	pszResource = RES_427_05_ME_USB_MUSIC_C_PNG;	break;
				case eNxUtil_OptItem_Eject:				pszResource = RES_216_00_OPT_FAV_REMOVE_C_PNG; 	break;
				case eNxUtil_OptItem_DeleteAll:			pszResource = RES_216_00_OPT_DELETE_C_PNG;		break;
#endif
#if defined(CONFIG_MW_CH_SATELLITE)
				case eNxUtil_OptItem_AntennaType:		pszResource = RES_216_01_OPT_ANTE_C_PNG;		break;
				case eNxUtil_OptItem_AntennaTypeFixed:	pszResource = RES_431_01_SET_POP_FIXED_C_PNG;	break;
				case eNxUtil_OptItem_AntennaTypeScd:	pszResource = RES_431_01_SET_POP_SCD_C_PNG;		break;
				case eNxUtil_OptItem_AntennaTypeMotor:	pszResource = RES_431_01_SET_POP_MOTOR_C_PNG;	break;
#endif
				case eNxUtil_OptItem_SearchOpt: 	pszResource = RES_216_00_OPT_SEARCH_C_PNG; 			break;

				case eNxUtil_OptItem_KeywordSearch:		pszResource = RES_216_00_OPT_SEARCH_C_PNG;		break;

				default:							pszResource = RES_NONE_PNG;							break;
			}
			break;
		default:
			NX_APP_Error("Error!!! Invalid Option Item type : (%d)\n", eNxOptType);
			break;
	}

	return	pszResource;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
HERROR	NX_PRISM_UTIL_ConvertDecStrToInt(HUINT8 *szString, HUINT32 *pulValue)
{
	HUINT32		 ulPos = 0;
	HUINT32		 ulValue = 0;

	if (szString == NULL)				{ return ERR_FAIL; }
	if (pulValue == NULL)				{ return ERR_FAIL; }

	// 32bit�� �ִ� ���� 4,294,967,296
	// �� string�� 10�� �̻� �Ǵ� ���ڴ� �ǹ̰� ����.

	ulValue = 0;
	for (ulPos = 0; ulPos < NX_UTIL_DTI_MAX; ulPos++)
	{
		/* ���ڴ� ���� �ٷ� '\0'�� ������ �װ� ERR_FAIL */
		if ((ulPos !=0) && (szString[ulPos] == '\0'))
		{
			*pulValue = ulValue;
			return	ERR_OK;
		}

		if ((ulPos ==0) && (szString[ulPos] < '0' || szString[ulPos] > '9'))
		{
			return ERR_FAIL;
		}
		else if ((ulPos !=0) && (szString[ulPos] < '0' || szString[ulPos] > '9'))
		{
			*pulValue = ulValue;
			return	ERR_OK;
		}

		ulValue *= 10;
		ulValue += (HUINT32)(szString[ulPos] - '0');
	}

	return	ERR_FAIL;
}


HINT8		*NX_PRISM_UTIL_GetResStringIdByWeekIndex(HUINT32 ulWeekIndex)
{
	switch (ulWeekIndex)
	{
		case WEEKDAY_SUNDAY:		return	RES_SUN_ID;
		case WEEKDAY_MONDAY:		return	RES_MON_ID;
		case WEEKDAY_TUESDAY:		return	RES_TUE_ID;
		case WEEKDAY_WEDNESDAY:		return	RES_WED_ID;
		case WEEKDAY_THURSDAY:		return	RES_THU_ID;
		case WEEKDAY_FRIDAY:		return	RES_FRI_ID;
		case WEEKDAY_SATURDAY:		return	RES_SAT_ID;
		default:
			return RES_UNKNOWN_ID;
	}
}


HINT8		*NX_PRISM_UTIL_GetResStringIdByMonth(HINT32 lMonth)
{
	switch (lMonth)
	{
		case eNxUtil_Month_JANUARY:		return	RES_JAN_ID;
		case eNxUtil_Month_FEBRUARY:	return	RES_FEB_ID;
		case eNxUtil_Month_MARCH:		return	RES_MAR_ID;
		case eNxUtil_Month_APRIL:		return	RES_APR_ID;
		case eNxUtil_Month_MAY:			return	RES_MAY_ID;
		case eNxUtil_Month_JUNE:		return	RES_JUN_ID;
		case eNxUtil_Month_JULY:		return	RES_JUL_ID;
		case eNxUtil_Month_AUGUST:		return	RES_AUG_ID;
		case eNxUtil_Month_SEPTEMBER:	return	RES_SEP_ID;
		case eNxUtil_Month_OCTOBER:		return	RES_OCT_ID;
		case eNxUtil_Month_NOVEMBER:	return	RES_NOV_ID;
		case eNxUtil_Month_DECEMBER:	return	RES_DEC_ID;
		default:						return	RES_UNKNOWN_ID;
	}
}


HERROR 	NX_PRISM_UTIL_GetDateString(eNxUiUtilStrType_t eType, HUINT8 ucDate, HUINT8 **ppszDateString64)
{
/*
	HERROR		hError = ERR_FAIL;
	HUINT8		*pucResult = NULL;
	HUINT8		*pStr = NULL;
	HUINT8		anDateStringId[][2] =
	{
		{RES_SUN_ID, RES_SUNDAY_ID},
		{RES_MON_ID, RES_MONDAY_ID},
		{RES_TUE_ID, RES_TUESDAY_ID},
		{RES_WED_ID, RES_WEDNESDAY_ID},
		{RES_THU_ID, RES_THURSDAY_ID},
		{RES_FRI_ID, RES_FRIDAY_ID},
		{RES_SAT_ID, RES_SATURDAY_ID}
	};

	if((eType > eNx_UI_Uitl_StrLong) || (ucDate >= 7) || (ppszDateString64 == NULL))
	{
		return hError;
	}

	*ppszDateString64 = NX_PRISM_GetTextBuffer(NX_TEXT_SIZE_64);
	HxSTR_Memset(*ppszDateString64, 0x00, sizeof(HUINT8)*NX_TEXT_SIZE_64);

	pStr = (HUINT8*)ONDK_GetString(anDateStringId[ucDate][eType]);
	if(pStr == NULL)
		return	ERR_FAIL;

	HxSTD_StrNCpy(*ppszDateString64, pStr, HxSTD_StrLen(pStr));
*/

	NX_COMMON_NOT_USED_PARAM(eType);
	NX_COMMON_NOT_USED_PARAM(ucDate);
	NX_COMMON_NOT_USED_PARAM(ppszDateString64);

	return	ERR_OK;
}


HINT8		*NX_PRISM_UTIL_GetGenreNibbleLevel1String(HUINT8 ucNibbleLevel1)
{
	NX_APP_Info("ucNibbleLevel1(0x%x)\n", ucNibbleLevel1);
	switch (ucNibbleLevel1)
	{
		case eNxUtil_NibbleLevel1_Undefined:						return	RES_UNDEFINEDED_ID;
		case eNxUtil_NibbleLevel1_MovieDrama:						return	RES_GENRE_MOVIE_ID;
		case eNxUtil_NibbleLevel1_NewsCurrentAffairs:				return	RES_GENRE_NEWS_ID;
		case eNxUtil_NibbleLevel1_ShowGameShow:						return	RES_GENRE_SHOWS_ID;
		case eNxUtil_NibbleLevel1_Sports:							return	RES_GENRE_SPORTS_ID;
		case eNxUtil_NibbleLevel1_ChildrensYouth:					return	RES_GENRE_CHILDREN_S_ID;
		case eNxUtil_NibbleLevel1_MusicBalletDance:					return	RES_GENRE_MUSIC_ID;
		case eNxUtil_NibbleLevel1_ArtsCulture:						return	RES_GENRE_ART_ID;
		case eNxUtil_NibbleLevel1_SocialPoliticalEconomics:			return	RES_GENRE_SOCIETY_ID;
		case eNxUtil_NibbleLevel1_EducationScienceFactureTopics:	return	RES_GENRE_EDUCATION_ID;
		case eNxUtil_NibbleLevel1_LeisureHobbies:					return	RES_GENRE_LEISURE_ID;
		case eNxUtil_NibbleLevel1_SpecialCharacteristics:			return	RES_GENRE_SPECIALS_ID;
		case eNxUtil_NibbleLevel1_Reserved1:						return	RES_UNDEFINEDED_ID;
		case eNxUtil_NibbleLevel1_Reserved2:						return	RES_UNDEFINEDED_ID;
		case eNxUtil_NibbleLevel1_Reserved3:						return	RES_UNDEFINEDED_ID;
		case eNxUtil_NibbleLevel1_UserDefined:						return	RES_USER_DEFINE_ID;
		default:													return	RES_UNDEFINEDED_ID;
	}
}


HINT8		*NX_PRISM_UTIL_GetGenreNibbleLevel2String(HUINT8 ucNibbleLevel1, HUINT8 ucNibbleLevel2)
{
	NX_APP_Info("ucMainGenre(0x%x)\n", ucNibbleLevel1);
	switch (ucNibbleLevel1)
	{
		case eNxUtil_NibbleLevel1_Undefined:						return	RES_UNDEFINEDED_ID;
		case eNxUtil_NibbleLevel1_MovieDrama:						return	nx_prism_util_GetGenreNibbleLevel2_MovieDramaString(ucNibbleLevel2);
		case eNxUtil_NibbleLevel1_NewsCurrentAffairs:				return	nx_prism_util_GetGenreNibbleLevel2_NewsCurrentAffairsString(ucNibbleLevel2);
		case eNxUtil_NibbleLevel1_ShowGameShow:						return	nx_prism_util_GetGenreNibbleLevel2_ShowGameShowString(ucNibbleLevel2);
		case eNxUtil_NibbleLevel1_Sports:							return	nx_prism_util_GetGenreNibbleLevel2_SportsString(ucNibbleLevel2);
		case eNxUtil_NibbleLevel1_ChildrensYouth:					return	nx_prism_util_GetGenreNibbleLevel2_ChildrensYouthString(ucNibbleLevel2);
		case eNxUtil_NibbleLevel1_MusicBalletDance:					return	nx_prism_util_GetGenreNibbleLevel2_MusicBalletDanceString(ucNibbleLevel2);
		case eNxUtil_NibbleLevel1_ArtsCulture:						return	nx_prism_util_GetGenreNibbleLevel2_ArtsCultureString(ucNibbleLevel2);
		case eNxUtil_NibbleLevel1_SocialPoliticalEconomics:			return	nx_prism_util_GetGenreNibbleLevel2_SocialPoliticalEconomicsString(ucNibbleLevel2);
		case eNxUtil_NibbleLevel1_EducationScienceFactureTopics:	return	nx_prism_util_GetGenreNibbleLevel2_EducationScienceFactureTopicsString(ucNibbleLevel2);
		case eNxUtil_NibbleLevel1_LeisureHobbies:					return	nx_prism_util_GetGenreNibbleLevel2_LeisureHobbiesString(ucNibbleLevel2);
		case eNxUtil_NibbleLevel1_SpecialCharacteristics:			return	nx_prism_util_GetGenreNibbleLevel2_SpecialCharacteristicsString(ucNibbleLevel2);
		case eNxUtil_NibbleLevel1_Reserved1:						return	RES_UNDEFINEDED_ID;
		case eNxUtil_NibbleLevel1_Reserved2:						return	RES_UNDEFINEDED_ID;
		case eNxUtil_NibbleLevel1_Reserved3:						return	RES_UNDEFINEDED_ID;
		case eNxUtil_NibbleLevel1_UserDefined:						return	RES_USER_DEFINE_ID;
		default:													return	RES_UNDEFINEDED_ID;
	}
}


HCHAR 	*NX_PRISM_UTIL_GetSubtitleResource(HINT32 lSubtitleType)
{
	if ((lSubtitleType == 0x02) || (lSubtitleType == 0x05))
	{
		return RES_218_00_SUBTITLE_EBU_PNG;
	}
	else if ((lSubtitleType >= 0x10 && lSubtitleType <= 0x14) ||
		(lSubtitleType >= 0x20 && lSubtitleType <= 0x24))
	{
		return RES_218_00_SUBTITLE_DVB_PNG;
	}
	else
	{
		return RES_218_00_SUBTITLE_DVB_PNG;
	}
}


HCHAR	*NX_PRISM_UTIL_GetCasTypeResource(HINT32 lCasType)
{
	HCHAR	*szRetRes = NULL;
	HINT32	n = 0;

	if (lCasType == eDxCAS_TYPE_All)
	{
		szRetRes = RES_202_00_CAS_MULTI_PNG;
		return	szRetRes;
	}

	if (lCasType & eDxCAS_TYPE_NAGRA)		{	szRetRes = RES_202_00_CAS_NA_PNG;		n++;	}
	if (lCasType & eDxCAS_TYPE_IRDETO)		{	szRetRes = RES_202_00_CAS_IR_PNG;		n++;	}
	if (lCasType & eDxCAS_TYPE_NDS)			{	szRetRes = RES_202_00_CAS_ND_PNG;		n++;	}
	if (lCasType & eDxCAS_TYPE_VIACCESS)	{	szRetRes = RES_202_00_CAS_VA_PNG;		n++;	}
	if (lCasType & eDxCAS_TYPE_CONAX)		{	szRetRes = RES_202_00_CAS_CX_PNG;		n++;	}
	if (lCasType & eDxCAS_TYPE_MEDIAGUARD)	{	szRetRes = RES_202_00_CAS_MG_PNG;		n++;	}
	if (lCasType & eDxCAS_TYPE_CRYPTOWORKS)	{	szRetRes = RES_202_00_CAS_CR_PNG;		n++;	}
	if (lCasType & eDxCAS_TYPE_ALLCAS)		{	szRetRes = RES_202_00_CAS_MULTI_PNG;	n++;	}
	if (lCasType & eDxCAS_TYPE_CIPLUS)		{	szRetRes = RES_202_00_CAS_MULTI_PNG;	n++;	}
	if (lCasType & eDxCAS_TYPE_VERIMATRIX)	{	szRetRes = RES_202_00_CAS_MULTI_PNG;	n++;	}

	if (n >= 2)
	{
		szRetRes = RES_202_00_CAS_MULTI_PNG;
	}

	return	szRetRes;
}


HCHAR	*NX_PRISM_UTIL_GetRatingResource(HINT32 lRating)
{
	HCHAR	*szRatingRes = NULL;

	switch (lRating)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			szRatingRes = NULL;
			break;
		case 4:		szRatingRes = RES_204_00_RATING_04_PNG;	break;
		case 5:		szRatingRes = RES_204_00_RATING_05_PNG;	break;
		case 6:		szRatingRes = RES_204_00_RATING_06_PNG;	break;
		case 7:		szRatingRes = RES_204_00_RATING_07_PNG;	break;
		case 8:		szRatingRes = RES_204_00_RATING_08_PNG;	break;
		case 9:		szRatingRes = RES_204_00_RATING_09_PNG;	break;
		case 10:	szRatingRes = RES_204_00_RATING_10_PNG;	break;
		case 11:	szRatingRes = RES_204_00_RATING_11_PNG;	break;
		case 12:	szRatingRes = RES_204_00_RATING_12_PNG;	break;
		case 13:	szRatingRes = RES_204_00_RATING_13_PNG;	break;
		case 14:	szRatingRes = RES_204_00_RATING_14_PNG;	break;
		case 15:	szRatingRes = RES_204_00_RATING_15_PNG;	break;
		case 16:	szRatingRes = RES_204_00_RATING_16_PNG;	break;
		case 17:	szRatingRes = RES_204_00_RATING_17_PNG;	break;
		case 18:	szRatingRes = RES_204_00_RATING_18_PNG;	break;
		default:	szRatingRes = RES_204_00_RATING_04_PNG;	break;
	}

	return szRatingRes;
}


HCHAR*	NX_PRISM_UTIL_GetDolbyIconResource(DxAudioCodec_e eCodec)
{
	HCHAR		*szDisplyImage = NULL;

	switch (eCodec)
	{
		case eDxAUDIO_CODEC_DOLBY_AC3:
		case eDxAUDIO_CODEC_DOLBY_AC3P:
			szDisplyImage = RES_208_00_DOLBY_SUBLIST_PNG;
			break;
		default:
			szDisplyImage = NULL;
			break;
	}

	return	szDisplyImage;
}


HCHAR*	NX_PRISM_UTIL_GetSubtitleIconResource(OxMediaPlay_SubtitleType_e eSubtitleType)
{
	HCHAR		*szDisplyImage = NULL;

	switch (eSubtitleType)
	{
		case eOxMP_SUBTITLETYPE_Dvb:	szDisplyImage = RES_218_00_SUBTITLE_DVB_PNG;	break;
		case eOxMP_SUBTITLETYPE_Ebu:	szDisplyImage = RES_218_00_SUBTITLE_EBU_PNG;	break;
		default:						szDisplyImage = NULL;						break;
	}

	return	szDisplyImage;
}


HCHAR 	*NX_PRISM_UTIL_GetReservationResource(DxRsvType_e eRsvType)
{
	HCHAR		*szRsvImage = NULL;

	switch (eRsvType)
	{
		case DxRSVTYPE_WATCHING_EBR:
		case DxRSVTYPE_WATCHING_TBR:
		case DxRSVTYPE_WATCHING_SERIES_EBR:
			szRsvImage = RES_205_00_RESERVATION_WATCH_PNG;
			break;

#if defined(CONFIG_MW_MM_PVR)
		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_TBR:
		case DxRSVTYPE_RECORD_SERIES_EBR:
		case DxRSVTYPE_RECORD_SATRECORD_TBR:
			szRsvImage = RES_205_04_REC_RECORDING_PNG;
			break;
#endif
		default:
			szRsvImage = NULL;
			break;
	}

	return	szRsvImage;
}


HERROR 	NX_PRISM_UTIL_FindNext4_3_DisplayFormat(HINT32 eDispFormat, HINT32 *eChgDispFormat)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!!! NX_SYSTEM_GetOperatorType() \n");
	}

	NX_APP_Info("eOpType(0x%x)\n", eOpType);
	switch (eOpType)
	{
		// TODO: Add here if the specifications are different
		default:
			*eChgDispFormat = eDxDFC_SELECT_UNKNOWN;
			if (eDispFormat == eDxDFC_SELECT_DEFAULT)		*eChgDispFormat = eDxDFC_SELECT_LETTERBOX;
			if (eDispFormat == eDxDFC_SELECT_LETTERBOX)		*eChgDispFormat = eDxDFC_SELECT_CENTERCUTOUT;
			if (eDispFormat == eDxDFC_SELECT_CENTERCUTOUT)	*eChgDispFormat = eDxDFC_SELECT_DEFAULT;
			break;
	}

	return	ERR_OK;
}


HERROR 	NX_PRISM_UTIL_FindNext16_9_DisplayFormat(HINT32 eDispFormat, HINT32 *eChgDispFormat)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!!! NX_SYSTEM_GetOperatorType() \n");
	}

	NX_APP_Info("eOpType(0x%x)\n", eOpType);
	switch (eOpType)
	{
		// TODO: Add here if the specifications are different
		default:
			*eChgDispFormat = eDxDFC_SELECT_UNKNOWN;
			if (eDispFormat == eDxDFC_SELECT_DEFAULT)		*eChgDispFormat = eDxDFC_SELECT_PILLARBOX;
			if (eDispFormat == eDxDFC_SELECT_PILLARBOX)		*eChgDispFormat = eDxDFC_SELECT_ZOOM;
			if (eDispFormat == eDxDFC_SELECT_ZOOM)			*eChgDispFormat = eDxDFC_SELECT_FULLSCREEN;
			if (eDispFormat == eDxDFC_SELECT_FULLSCREEN)	*eChgDispFormat = eDxDFC_SELECT_DEFAULT;
			break;
	}

	return	ERR_OK;
}


HERROR 	NX_PRISM_UTIL_FindNextVideoFormatByRatioDisplayFormat(HINT32 eScreenRatio, HINT32 eDispFormat, HINT32 *eChgDispFormat)
{

	NX_APP_Info("eScreenRatio:[%d], eDispFormat: [%d]\n", eScreenRatio, eDispFormat);

	switch (eScreenRatio)
	{
		case eDxASPECTRATIO_4X3:
			NX_PRISM_UTIL_FindNext4_3_DisplayFormat(eDispFormat, eChgDispFormat);
			break;
		case eDxASPECTRATIO_16X9:
			NX_PRISM_UTIL_FindNext16_9_DisplayFormat(eDispFormat, eChgDispFormat);
			break;
		default:
			NX_APP_Error("ERROR!!! Not Support Format \n");
			break;
	}

	return	ERR_OK;
}


HCHAR* 	NX_PRISM_UTIL_GetLangFullNameBy639Code(HCHAR *pszString)
{
	return	(HCHAR*)HLIB_LANG_639ToFullName(pszString);
}


HINT8	*NX_PRISM_UTIL_GetResStringIdByLangId(HxLANG_Id_e eLangId)
{
	switch (eLangId)
	{
#if defined(CONFIG_PROD_LANG_ARABIC)
		case eLangID_Arabic:			return	RES_ARABIAN_ID;
#endif
#if defined(CONFIG_PROD_LANG_BULGARIAN)
		case eLangID_Bulgarian: 		return	RES_UNKNOWN_ID;
#endif
#if defined(CONFIG_PROD_LANG_CATALAN)
		case eLangID_Catalan:			return	RES_UNKNOWN_ID;
#endif
#if defined(CONFIG_PROD_LANG_CROATIAN)
		case eLangID_Croatian:			return	RES_UNKNOWN_ID;
#endif
#if defined(CONFIG_PROD_LANG_CZECH)
		case eLangID_Czech:				return	RES_CZECH_01_ID;
#endif
#if defined(CONFIG_PROD_LANG_DANISH)
		case eLangID_Danish: 			return	RES_DANSK_ID;
#endif
#if defined(CONFIG_PROD_LANG_DUTCH)
		case eLangID_Dutch:				return	RES_NEDERLANDS_ID;
#endif
#if defined(CONFIG_PROD_LANG_ENGLISH)
		case eLangID_English:			return	RES_ENGLISH_01_ID;
#endif
#if defined(CONFIG_PROD_LANG_FINNISH)
		case eLangID_Finnish:			return	RES_SUOMI_ID;
#endif
#if defined(CONFIG_PROD_LANG_FRENCH)
		case eLangID_French:			return	RES_FRENCH_01_ID;
#endif
#if defined(CONFIG_PROD_LANG_GERMAN)
		case eLangID_German:			return	RES_DEUTSCH_01_ID;
#endif
#if defined(CONFIG_PROD_LANG_GREEK)
		case eLangID_Greek:				return	RES_GREEK_01_ID;
#endif
#if defined(CONFIG_PROD_LANG_HUNGARIAN)
		case eLangID_Hungarian:			return	RES_MAGYAR_ID;
#endif
#if defined(CONFIG_PROD_LANG_HINDI)
		case eLangID_Hindi:				return	RES_UNKNOWN_ID;
#endif
#if defined(CONFIG_PROD_LANG_ITALIAN)
		case eLangID_Italian:			return	RES_ITALIANO_01_ID;
#endif
#if defined(CONFIG_PROD_LANG_NORWEGIAN)
		case eLangID_Norwegian:			return	RES_UNKNOWN_ID;
#endif
#if defined(CONFIG_PROD_LANG_PERSIAN)
		case eLangID_Persian:			return	RES_PERSIAN_01_ID;
#endif
#if defined(CONFIG_PROD_LANG_POLISH)
		case eLangID_Polish:			return	RES_POLSKI_ID;
#endif
#if defined(CONFIG_PROD_LANG_PORTUGUESE)
		case eLangID_Portuguese:		return	RES_UNKNOWN_ID;
#endif
#if defined(CONFIG_PROD_LANG_ROMANIAN)
		case eLangID_Romanian:			return	RES_UNKNOWN_ID;
#endif
#if defined(CONFIG_PROD_LANG_RUSSIAN)
		case eLangID_Russian:			return	RES_RUSSIAN_01_ID;
#endif
#if defined(CONFIG_PROD_LANG_SERBIAN)
		case eLangID_Serbian:			return	RES_UNKNOWN_ID;
#endif
#if defined(CONFIG_PROD_LANG_SLOVAK)
		case eLangID_Slovak:			return	RES_SLOVAK_01_ID;
#endif
#if defined(CONFIG_PROD_LANG_SLOVENIAN)
		case eLangID_Slovenian:			return	RES_UNKNOWN_ID;
#endif
#if defined(CONFIG_PROD_LANG_SPANISH)
		case eLangID_Spanish:			return	RES_SPANISH_01_ID;
#endif
#if defined(CONFIG_PROD_LANG_SWEDISH)
		case eLangID_Swedish:			return	RES_SVENSKA_ID;
#endif
#if defined(CONFIG_PROD_LANG_TURKISH)
		case eLangID_Turkish:			return	RES_TURKEY_01_ID;
#endif
#if defined(CONFIG_PROD_LANG_UKRAINIAN)
		case eLangID_Ukrainian:			return	RES_UNKNOWN_ID;
#endif
#if defined(CONFIG_PROD_LANG_THAI)
		case eLangID_Thai:				return	RES_UNKNOWN_ID;
#endif
#if defined(CONFIG_PROD_LANG_JAPAN)
		case eLangID_Japanese:			return	RES_UNKNOWN_ID;
#endif

		case eLangID_OriginalAudio:		return	RES_ORIGINAL_ID;
		case eLangID_Undefined:			return	RES_UNDEFINEDED_ID;

		case eLangID_NONE:				return	RES_OFF_ID;
		default:						return	RES_UNKNOWN_ID;
	}
}


HCHAR	*NX_PRISM_UTIL_GetResStringIdByLangCode(HCHAR* pszLangCode)
{
	if (HxSTD_StrCmp(pszLangCode, "en-EN") == 0 || HxSTD_StrCmp(pszLangCode, "en-US") == 0)
	{
		return (HCHAR*)ONDK_GetString(RES_ENGLISH_01_ID);
	}
	else if (HxSTD_StrCmp(pszLangCode, "de-DE") == 0)
	{
		return (HCHAR*)ONDK_GetString(RES_DEUTSCH_01_ID);
	}
	else if (HxSTD_StrCmp(pszLangCode, "ko-KR") == 0)
	{
		return (HCHAR*)ONDK_GetString(RES_KOREAN_ID);
	}
	else
	{
		return (HCHAR*)ONDK_GetString(RES_UNKNOWN_ID);
	}
}


ONDK_Result_t	NX_PRISM_UTIL_MakeSubtitleString(NX_Media_SBTList_t stSubtList, HUINT32 ulUserSelectIdx, HCHAR *pszStrBuf)
{
	HCHAR	*szFullName = NULL;

	switch (stSubtList.pstSubtitlelInfo[ulUserSelectIdx].eSubtitleType)
	{
		case eOxMP_SUBTITLETYPE_Dvb :
			/* Language code�� ���� ���?"Subtitle"���� Display �Ѵ�. */
			if (HxSTD_StrLen(stSubtList.pstSubtitlelInfo[ulUserSelectIdx].utArg.stDvbInfo.szLangCode) == 0)
			{
				HxSTD_sprintf((HCHAR*)pszStrBuf, "%s %d", (HCHAR*)ONDK_GetString(RES_SUBTITLE_ID), ulUserSelectIdx);
			}
			else
			{
				szFullName = NX_PRISM_UTIL_GetLangFullNameBy639Code(stSubtList.pstSubtitlelInfo[ulUserSelectIdx].utArg.stDvbInfo.szLangCode);
				if (szFullName)
				{
					HxSTD_StrCpy((HCHAR*)pszStrBuf, szFullName);
				}
				else
				{
					HxSTD_StrCpy((HCHAR*)pszStrBuf, stSubtList.pstSubtitlelInfo[ulUserSelectIdx].utArg.stDvbInfo.szLangCode);
				}
			}
			break;

		case eOxMP_SUBTITLETYPE_Ebu:
			/* Language code�� ���� ���?"Subtitle"���� Display �Ѵ�. */
			if (HxSTD_StrLen(stSubtList.pstSubtitlelInfo[ulUserSelectIdx].utArg.stEbuInfo.szLangCode) == 0)
			{
				HxSTD_sprintf((HCHAR*)pszStrBuf, "%s %d", (HCHAR*)ONDK_GetString(RES_SUBTITLE_ID), ulUserSelectIdx);
			}
			else
			{
				szFullName = NX_PRISM_UTIL_GetLangFullNameBy639Code(stSubtList.pstSubtitlelInfo[ulUserSelectIdx].utArg.stEbuInfo.szLangCode);
				if (szFullName)
				{
					HxSTD_StrCpy((HCHAR*)pszStrBuf, szFullName);
				}
				else
				{
					HxSTD_StrCpy((HCHAR*)pszStrBuf, stSubtList.pstSubtitlelInfo[ulUserSelectIdx].utArg.stEbuInfo.szLangCode);
				}
			}
			break;

		case eOxMP_SUBTITLETYPE_Media:
			if (HxSTD_StrLen(stSubtList.pstSubtitlelInfo[ulUserSelectIdx].utArg.stMediaInfo.szLangCode) == 0)
			{
				HxSTD_sprintf((HCHAR*)pszStrBuf, "%s %d", (HCHAR*)ONDK_GetString(RES_SUBTITLE_ID), ulUserSelectIdx);
			}
			else
			{
				szFullName = NX_PRISM_UTIL_GetResStringIdByLangCode(stSubtList.pstSubtitlelInfo[ulUserSelectIdx].utArg.stMediaInfo.szLangCode);
				if (szFullName)
				{
					HxSTD_StrCpy((HCHAR*)pszStrBuf, szFullName);
				}
				else
				{
					HxSTD_StrCpy((HCHAR*)pszStrBuf, stSubtList.pstSubtitlelInfo[ulUserSelectIdx].utArg.stMediaInfo.szLangCode);
				}
			}
			break;

		case eOxMP_SUBTITLETYPE_Unknown:
			if ((stSubtList.pstSubtitlelInfo[ulUserSelectIdx].nPid == NX_CORE_PID_NULL) || (ulUserSelectIdx == 0))
			{
				HxSTD_sprintf((HCHAR*)pszStrBuf, "%s", (HCHAR*)ONDK_GetString(RES_OFF_ID));
			}
			break;

		default:
			break;
	}

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_UTIL_MakeAudioString(NX_Media_AUDIOList_t stAudioList, HUINT32 ulUserSelectIdx, HCHAR *pszStrBuf, HBOOL bDulaMonoFullName)
{
	HCHAR	*szFullName = NULL, *szOtherFullName = NULL;
	HINT32	i = 0, lTotlaLen = (NX_TEXT_SIZE_64 - 1), lTextLen = 0;


	if (bDulaMonoFullName == TRUE)
	{
		return NX_PRISM_UTIL_MakeAudioStringWithDualMonoFullName(stAudioList, ulUserSelectIdx, pszStrBuf);
	}

	if (stAudioList.pstAudioInfo[ulUserSelectIdx].eMode == eOxMP_AUDMODE_DUALMONO)
	{
		// Left Language
		szFullName = NX_PRISM_UTIL_GetLangFullNameBy639Code((HCHAR*)stAudioList.pstAudioInfo[ulUserSelectIdx].astLangInfo[0].szLangCode);
		if (szFullName)
		{
			lTextLen = HxSTD_StrLen(szFullName);
			HxSTD_StrNCpy(pszStrBuf, szFullName, lTextLen);
		}
		else
		{
			lTextLen = DxLANG_CODE_LEN;
			HxSTD_StrNCpy(pszStrBuf, (HCHAR*)stAudioList.pstAudioInfo[ulUserSelectIdx].astLangInfo[0].szLangCode, lTextLen);
		}
		lTotlaLen -= lTextLen;
		for (i = 1; i < stAudioList.pstAudioInfo[ulUserSelectIdx].nLangNum; i++)
		{
			HxSTD_StrCat(pszStrBuf, " / ");

			// Right Language
			szOtherFullName = NX_PRISM_UTIL_GetLangFullNameBy639Code((HCHAR*)stAudioList.pstAudioInfo[ulUserSelectIdx].astLangInfo[1].szLangCode);
			if (szOtherFullName)
			{
				lTextLen = HxSTD_StrLen(szOtherFullName);
				if (lTotlaLen < lTextLen)
					break;
				HxSTD_StrNCat(pszStrBuf, szOtherFullName, lTextLen);
			}
			else
			{
				lTextLen = DxLANG_CODE_LEN;
				if (lTotlaLen < lTextLen)
					break;
				HxSTD_StrNCat(pszStrBuf, (HCHAR*)stAudioList.pstAudioInfo[ulUserSelectIdx].astLangInfo[1].szLangCode, lTextLen);
			}
		}
	}
	else
	{
		/* Language code�� ���� ���?"Audio + Index"���� Display �Ѵ�. */
		if (HxSTD_StrLen((HCHAR*)stAudioList.pstAudioInfo[ulUserSelectIdx].astLangInfo[0].szLangCode) == 0)
		{
			HxSTD_sprintf(pszStrBuf, "%s %d", (HCHAR*)ONDK_GetString(RES_AUDIO_ID), ulUserSelectIdx);
		}
		else
		{
			szFullName = NX_PRISM_UTIL_GetLangFullNameBy639Code((HCHAR*)stAudioList.pstAudioInfo[ulUserSelectIdx].astLangInfo[0].szLangCode);
			if (szFullName)
			{
				HxSTD_StrCpy(pszStrBuf, szFullName);
			}
			else
			{
				HxSTD_StrCpy(pszStrBuf, (HCHAR*)stAudioList.pstAudioInfo[ulUserSelectIdx].astLangInfo[0].szLangCode);
			}
		}
	}

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_UTIL_MakeAudioStringWithDualMonoFullName(NX_Media_AUDIOList_t stAudioList, HUINT32 ulUserSelectIdx, HCHAR *pszStrBuf)
{
	HCHAR	*szFullName = NULL;


	/* Language code�� ���� ���?"Audio + Index"���� Display �Ѵ�. */
	if (HxSTD_StrLen((HCHAR*)stAudioList.pstAudioInfo[ulUserSelectIdx].astLangInfo[0].szLangCode) == 0)
	{
		HxSTD_sprintf(pszStrBuf, "%s %d", (HCHAR*)ONDK_GetString(RES_AUDIO_ID), ulUserSelectIdx);
	}
	else
	{
		szFullName = NX_PRISM_UTIL_GetLangFullNameBy639Code((HCHAR*)stAudioList.pstAudioInfo[ulUserSelectIdx].astLangInfo[0].szLangCode);
		if (szFullName)
		{
			HxSTD_StrCpy(pszStrBuf, szFullName);
		}
		else
		{
			HxSTD_StrCpy(pszStrBuf, (HCHAR*)stAudioList.pstAudioInfo[ulUserSelectIdx].astLangInfo[0].szLangCode);
		}
	}

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_UTIL_InitNUllPIDValue(NX_Media_AUDIOList_t *stAudioList)
{
	HUINT32		i = 0;

	for (i = 0; i < NX_CORE_AUDIO_MAX_NUM; i++)
	{
		stAudioList->pstAudioInfo[i].nPid = NX_CORE_PID_NULL;
	}

	return	ONDK_RET_OK;
}


HERROR		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType(NX_ChListGroup_t eChGroupType, HCHAR *pszString)
{
	HBOOL bUserUpdateFlag = FALSE;

	if (pszString == NULL)
	{
		return ERR_FAIL;
	}

	switch (eChGroupType)
	{
		case eNxChListGroup_ALL:			HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_ALL_43_ID));	break;
		case eNxChListGroup_TV:				HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_TV_01_ID));	break;
		case eNxChListGroup_RADIO:			HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_RADIO_ID));	break;
		case eNxChListGroup_HDTV:			HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_HDTV_ID));	break;
		case eNxChListGroup_FTA:			HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_FREE_CHANNEL_ID));	break;
		case eNxChListGroup_CAS:			HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_PAY_CHANNEL_ID));	 break;
		case eNxChListGroup_ALPHABET:		HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_A_Z_ID));	break;
		case eNxChListGroup_AlphabetAll:	HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_ALL_ID));	break;
		case eNxChListGroup_Satellite:		HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_SATELLITE_ID)); break;
		case eNxChListGroup_MBC:			HxSTD_sprintf(pszString, "%s", (HCHAR*)"MBC"); break;
		case eNxChListGroup_Movie:			HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_GENRE_MOVIE_ID)); break;
		case eNxChListGroup_Kids:			HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_KIDS_02_ID)); break;
		case eNxChListGroup_Sports:			HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_GENRE_SPORTS_ID)); break;
		case eNxChListGroup_AlphabetA:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"A");	break;
		case eNxChListGroup_AlphabetB:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"B");	break;
		case eNxChListGroup_AlphabetC:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"C");	break;
		case eNxChListGroup_AlphabetD:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"D");	break;
		case eNxChListGroup_AlphabetE:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"E");	break;
		case eNxChListGroup_AlphabetF:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"F");	break;
		case eNxChListGroup_AlphabetG:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"G");	break;
		case eNxChListGroup_AlphabetH:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"H");	break;
		case eNxChListGroup_AlphabetI:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"I");	break;
		case eNxChListGroup_AlphabetJ:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"J");	break;
		case eNxChListGroup_AlphabetK:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"K");	break;
		case eNxChListGroup_AlphabetL:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"L");	break;
		case eNxChListGroup_AlphabetM:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"M");	break;
		case eNxChListGroup_AlphabetN:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"N");	break;
		case eNxChListGroup_AlphabetO:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"O");	break;
		case eNxChListGroup_AlphabetP:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"P");	break;
		case eNxChListGroup_AlphabetQ:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"Q");	break;
		case eNxChListGroup_AlphabetR:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"R");	break;
		case eNxChListGroup_AlphabetS:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"S");	break;
		case eNxChListGroup_AlphabetT:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"T");	break;
		case eNxChListGroup_AlphabetU:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"U");	break;
		case eNxChListGroup_AlphabetV:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"V");	break;
		case eNxChListGroup_AlphabetW:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"W");	break;
		case eNxChListGroup_AlphabetX:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"X");	break;
		case eNxChListGroup_AlphabetY:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"Y");	break;
		case eNxChListGroup_AlphabetZ:		HxSTD_sprintf(pszString, "%s", (HCHAR*)"Z");	break;
		case eNxChListGroup_AlphabetDigit:	HxSTD_sprintf(pszString, "%s", (HCHAR*)"0 ~ 9");	break;
		case eNxChListGroup_AlphabetEtc:	HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_ETC_ID));	break;

		case eNxChListGroup_Favorite1:
			NX_CHANNEL_FAV_IsUpdateFavouriteName(eNxFavGroupID_User, &bUserUpdateFlag);
			if (bUserUpdateFlag == TRUE)
			{
				NX_CHANNEL_FAV_GetFavouriteName(eNxFavGroupID_User, pszString);
			}
			else
			{
				HxSTD_sprintf(pszString, "%s", ONDK_GetString(RES_FAVOURITE1_ID));
			}
			break;
		case eNxChListGroup_Favorite2:
			NX_CHANNEL_FAV_IsUpdateFavouriteName(eNxFavGroupID_User + 1, &bUserUpdateFlag);
			if (bUserUpdateFlag == TRUE)
			{
				NX_CHANNEL_FAV_GetFavouriteName(eNxFavGroupID_User + 1, pszString);
			}
			else
			{
				HxSTD_sprintf(pszString, "%s", ONDK_GetString(RES_FAVOURITE2_ID));
			}
			break;
		case eNxChListGroup_Favorite3:
			NX_CHANNEL_FAV_IsUpdateFavouriteName(eNxFavGroupID_User + 2, &bUserUpdateFlag);
			if (bUserUpdateFlag == TRUE)
			{
				NX_CHANNEL_FAV_GetFavouriteName(eNxFavGroupID_User + 2, pszString);
			}
			else
			{
				HxSTD_sprintf(pszString, "%s", ONDK_GetString(RES_FAVOURITE3_ID));
			}
			break;
		case eNxChListGroup_Favorite4:
			NX_CHANNEL_FAV_IsUpdateFavouriteName(eNxFavGroupID_User + 3, &bUserUpdateFlag);
			if (bUserUpdateFlag == TRUE)
			{
				NX_CHANNEL_FAV_GetFavouriteName(eNxFavGroupID_User + 3, pszString);
			}
			else
			{
				HxSTD_sprintf(pszString, "%s", ONDK_GetString(RES_FAVOURITE4_ID));
			}
			break;
		case eNxChListGroup_Favorite5:
			NX_CHANNEL_FAV_IsUpdateFavouriteName(eNxFavGroupID_User + 4, &bUserUpdateFlag);
			if (bUserUpdateFlag == TRUE)
			{
				NX_CHANNEL_FAV_GetFavouriteName(eNxFavGroupID_User + 4, pszString);
			}
			else
			{
				HxSTD_sprintf(pszString, "%s", ONDK_GetString(RES_FAVOURITE5_ID));
			}
			break;
 		default:
			NX_APP_Error(" Error!!! UnKnow Group [%u]!!\n", eChGroupType);
			HxSTD_sprintf(pszString, "%s", (HCHAR*)ONDK_GetString(RES_UNKNOWN_ID));
			break;
	}

	return ERR_OK;
}


HCHAR		*NX_PRISM_UTIL_ConvSatelliteNameToDispSatelliteName(HCHAR *szAntennaName)
{
#if defined(CONFIG_MW_CH_SATELLITE)

	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_AMOS_2_3) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_AMOS_2_3;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_APSTAR_2R) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_APSTAR_2R;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_ARABSAT_5A) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_ARABSAT_5A;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_ASIASAT_3S) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_ASIASAT_3S;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_ASIASAT_5) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_ASIASAT_5;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_ASTRA_1) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_ASTRA_1;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_ASTRA_2) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_ASTRA_2;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_ASTRA_3A_3B) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_ASTRA_3A_3B;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_ASTRA_4A) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_ASTRA_4A;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_ATLANTIC_BIRD_1) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_ATLANTIC_BIRD_1;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_ATLANTIC_BIRD_2) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_ATLANTIC_BIRD_2;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_ATLANTIC_BIRD_3) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_ATLANTIC_BIRD_3;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_BADR_4_6) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_BADR_4_6;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUROBIRD_1) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUROBIRD_1;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUROBIRD_3) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUROBIRD_3;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUROBIRD_9A) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUROBIRD_9A;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUTELSAT_W2A) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUTELSAT_W2A;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUTELSAT_W3A) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUTELSAT_W3A;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUTELSAT_W4_W7) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUTELSAT_W4_W7;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUTELSAT_W6) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUTELSAT_W6;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EXPRESS_AM1) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EXPRESS_AM1;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EXPRESS_AM22) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EXPRESS_AM22;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_HELLAS_SAT2) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_HELLAS_SAT2;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_HISPASAT_1C_1D) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_HISPASAT_1C_1D;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_HOTBIRD) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_HOTBIRD;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INSAT_3A_4B) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INSAT_3A_4B;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_10_02) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_10_02;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_12) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_12;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_7_10) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_7_10;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_901) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_901;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_902) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_902;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_904) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_904;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_906) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_906;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_907) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_907;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_NILESAT_101_102) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_NILESAT_101_102;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_NSS_12) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_NSS_12;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_NSS_7) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_NSS_7;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_TELSTAR_12) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_TELSTAR_12;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_THOR_5_6) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_THOR_5_6;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_TURKSAT_2A_3A) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_TURKSAT_2A_3A;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_YAMAL_201) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_YAMAL_201;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_ARABSAT_2B) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_ARABSAT_2B;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUTELSAT_W1) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUTELSAT_W1;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUTELSAT_W2) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUTELSAT_W2;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_NSS_703) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_NSS_703;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_SIRIUS) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_SIRIUS;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_TELECOM_2D) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_TELECOM_2D;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUROBIRD_9) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUROBIRD_9;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_802) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_802;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_THOR_3_5) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_THOR_3_5;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INSAT_2E4A) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INSAT_2E4A;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_17) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_17;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_YAHLIVE) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_YAHLIVE;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_HISPASAT_1D) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_HISPASAT_1D;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_HISPASAT_1E) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_HISPASAT_1E;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_SES_2) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_SES_2;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_SES_3) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_SES_3;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_SES_6) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_SES_6;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUTELSAT_5_WEST_A) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUTELSAT_5_WEST_A;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUTELSAT_12_WEST_B) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUTELSAT_12_WEST_B;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUTELSAT_113_WEST_A) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUTELSAT_113_WEST_A;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUTELSAT_117_WEST_A) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUTELSAT_117_WEST_A;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUTELSAT_HOTBIRD_13B) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUTELSAT_HOTBIRD_13B;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUTELSAT_HOTBIRD_13C) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUTELSAT_HOTBIRD_13C;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_EUTELSAT_HOTBIRD_13D) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_EUTELSAT_HOTBIRD_13D;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_1R) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_1R;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_11) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_11;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_14) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_14;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_21) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_21;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_23) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_23;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_34) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_34;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_INTELSAT_903) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_INTELSAT_903;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_GALAXY_18) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_GALAXY_18;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_GALAXY_19) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_GALAXY_19;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_GALAXY_28) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_GALAXY_28;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_SPACEWAY_1_AND_DIRECTV_10_12) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_SPACEWAY_1_AND_DIRECTV_10_02;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_AMAZONAS_2) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_AMAZONAS_2;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_AMAZONAS_3) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_AMAZONAS_3;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_AMC_6) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_AMC_6;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_AMC_8) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_AMC_8;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_AMC_15) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_AMC_15;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_AMC_18) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_AMC_18;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_AMC_21) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_AMC_21;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_BSAT_3C_JCSAT_110R) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_BSAT_3C_JCSAT_110R;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_N_SAT_110) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_N_SAT_110;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_TELSTAR_12_VANTAGE) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_TELSTAR_12_VANTAGE;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_TELSTAR_14R) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_TELSTAR_14R;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_ST_2) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_ST_2;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_SIMON_BOLIVAR) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_SIMON_BOLIVAR;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_STAR_ONE_C1) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_STAR_ONE_C1;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_STAR_ONE_C2) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_STAR_ONE_C2;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_STAR_ONE_C4) == 0)
	{
#if defined(CONFIG_OP_AMX)
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_STAR_ONE_CENAM;
#else
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_STAR_ONE_C4;
#endif
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_NSS_10) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_NSS_10;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_BRASILSAT_B4) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_BRASILSAT_B4;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_USERDEFINE) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_USERDEFINE;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_USERDEFINE1) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_USERDEFINE1;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_USERDEFINE2) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_USERDEFINE2;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_USERDEFINE3) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_USERDEFINE3;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_USERDEFINE4) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_USERDEFINE4;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_USERDEFINE4) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_USERDEFINE4;
	}
	if (HxSTD_StrCmp(szAntennaName, NX_SAT_SATNAME_USERDEFINE5) == 0)
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_USERDEFINE5;
	}
	else
	{
		return	(HCHAR*)NX_UTIL_SAT_SATNAME_NONE;
	}
#else
	return	(HUINT8*)NX_UTIL_SAT_SATNAME_NONE;
#endif
}


void	NX_PRISM_UTIL_MakeTpStringByTuneParamAndTunerType(DxDeliveryType_e eTunerType, DxTuneParam_t *pstTp, HUINT8 *pString)
{
	HCHAR		*szDispSatName = NULL;
	HUINT8		szTextBuffer[NX_TEXT_SIZE_16];


	if ((pstTp == NULL) || (pString == NULL))
	{
		NX_APP_Error("Error!!! pstTp == NULL or pString == NULL!!\n");
		return;
	}

	switch (eTunerType)
	{
		case eDxDELIVERY_TYPE_SAT:
			/* Satellite Name */
			szDispSatName = NX_PRISM_UTIL_ConvSatelliteNameToDispSatelliteName(pstTp->sat.antInfo.satType);
			NX_ANTENNA_GetSatelliteNameWithSatPosition(pstTp->sat.antInfo.satType, szDispSatName, pString);
			//HxSTD_StrNCpy((char*)pString, (char*)szDispSatName, DxANTINFO_NAME_LEN);

			/* tp frequecy */
			HxSTD_memset(szTextBuffer, 0, NX_TEXT_SIZE_16);
			szTextBuffer[0] = '\0';
			HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)", %d, ", (int)pstTp->sat.tuningInfo.ulFrequency);
			HxSTD_StrCat((HCHAR*)pString, (HCHAR*)szTextBuffer);

			/* polarization */
			HxSTD_memset(szTextBuffer, 0, NX_TEXT_SIZE_16);
			szTextBuffer[0] = '\0';
			switch (pstTp->sat.tuningInfo.ePolarization)
			{
				case eDxSAT_POLAR_AUTO: 	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"%s, ", "A");		break;
				case eDxSAT_POLAR_HOR:		HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"%s, ", "H");		break;
				case eDxSAT_POLAR_VER:		HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"%s, ", "V");		break;
				case eDxSAT_POLAR_LEFT: 	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"%s, ", "L");		break;
				case eDxSAT_POLAR_RIGHT:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"%s, ", "R");		break;
				default:					HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"NOT, ");			break;
			}
			HxSTD_StrCat((HCHAR*)pString, (HCHAR*)szTextBuffer);

			/* symbol rate */
			HxSTD_memset (szTextBuffer, 0x00, NX_TEXT_SIZE_16);
			szTextBuffer[0] = '\0';
			HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"%d, ", (int)pstTp->sat.tuningInfo.ulSymbolRate);
			HxSTD_StrCat ((HCHAR*)pString, (HCHAR*)szTextBuffer);

			/* FEC */
			HxSTD_memset (szTextBuffer, 0x00, NX_TEXT_SIZE_16);
			szTextBuffer[0] = '\0';
			switch (pstTp->sat.tuningInfo.eFecCodeRate)
			{
				case eDxSAT_CODERATE_1_2:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"1/2"); 	break;
				case eDxSAT_CODERATE_2_3:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"2/3"); 	break;
				case eDxSAT_CODERATE_3_4:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"3/4"); 	break;
				case eDxSAT_CODERATE_3_5:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"3/5"); 	break;
				case eDxSAT_CODERATE_4_5:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"4/5"); 	break;
				case eDxSAT_CODERATE_5_11:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"5/11");	break;
				case eDxSAT_CODERATE_5_6:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"5/6"); 	break;
				case eDxSAT_CODERATE_6_7:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"6/7"); 	break;
				case eDxSAT_CODERATE_7_8:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"7/8"); 	break;
				case eDxSAT_CODERATE_8_9:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"8/9"); 	break;
				case eDxSAT_CODERATE_9_10:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"9/10");	break;
				case eDxSAT_CODERATE_AUTO:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"A");		break;
				case eDxSAT_CODERATE_NONE:
				default:
					HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"NOT");
					break;
			}
			HxSTD_StrCat ((HCHAR*)pString, (HCHAR*)szTextBuffer);

			break;

		case eDxDELIVERY_TYPE_TER:
			HxSTD_snprintf((HCHAR*)pString, NX_TEXT_SIZE_64, (HCHAR*)"%d kHz", (int)pstTp->ter.ulFrequency);

			/* bandwidth */
			switch(pstTp->ter.eBandWidth)
			{
				case eDxTER_BANDWIDTH_6M:	HxSTD_StrCat((HCHAR*)pString, (HCHAR *)", 6MHz");	break;
				case eDxTER_BANDWIDTH_7M:	HxSTD_StrCat((HCHAR*)pString, (HCHAR *)", 7MHz");	break;
				case eDxTER_BANDWIDTH_8M:	HxSTD_StrCat((HCHAR*)pString, (HCHAR *)", 8MHz");	break;
				default:					HxSTD_StrCat((HCHAR*)pString, (HCHAR *)", NONE");	break;
			}
			break;

		case eDxDELIVERY_TYPE_CAB:
			HxSTD_memset (szTextBuffer, 0x00, NX_TEXT_SIZE_16);
			szTextBuffer[0] = '\0';
			switch (pstTp->cab.constellation)
			{
				case eDxCAB_CONSTELLATION_16QAM: 	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"16");	break;
				case eDxCAB_CONSTELLATION_32QAM: 	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"32");	break;
				case eDxCAB_CONSTELLATION_64QAM:	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"64");	break;
				case eDxCAB_CONSTELLATION_128QAM:	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"128");	break;
				case eDxCAB_CONSTELLATION_256QAM:	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"256");	break;
				case eDxCAB_CONSTELLATION_1024QAM:	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"1024");	break;
				case eDxCAB_CONSTELLATION_4096QAM:	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"4096");	break;
				case eDxCAB_CONSTELLATION_AUTO:		HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"AUTO");	break;
			}
			HxSTD_snprintf((HCHAR*)pString, (NX_TEXT_SIZE_32 - 1), (HCHAR*)"%d kHz SR %d %s QAM", (int)pstTp->cab.ulFrequency, (int)pstTp->cab.ulSymbolRate, (HCHAR*)szTextBuffer);
			break;

		default:
			break;
	}
}


void	NX_PRISM_UTIL_MakeTpStringByTuneParam(DxTuneParam_t *pstTp, HUINT8 *pString)
{
	HCHAR		*szDispSatName = NULL;
	HUINT8		szTextBuffer[NX_TEXT_SIZE_16];


	if ((pstTp == NULL) || (pString == NULL))
	{
		NX_APP_Error("Error!!! pstTp == NULL or pString == NULL!!\n");
		return;
	}

	switch (pstTp->eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
			/* Satellite Name */
			szDispSatName = NX_PRISM_UTIL_ConvSatelliteNameToDispSatelliteName(pstTp->sat.antInfo.satType);
			NX_ANTENNA_GetSatelliteNameWithSatPosition(pstTp->sat.antInfo.satType, szDispSatName, pString);

			/* tp frequecy */
			HxSTD_memset(szTextBuffer, 0, NX_TEXT_SIZE_16);
			szTextBuffer[0] = '\0';
			HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)", %dMhz, ", (int)pstTp->sat.tuningInfo.ulFrequency);
			HxSTD_StrCat((HCHAR*)pString, (HCHAR*)szTextBuffer);

			/* polarization */
			HxSTD_memset(szTextBuffer, 0, NX_TEXT_SIZE_16);
			szTextBuffer[0] = '\0';
			switch (pstTp->sat.tuningInfo.ePolarization)
			{
				case eDxSAT_POLAR_AUTO: 	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"%s, ", "A");		break;
				case eDxSAT_POLAR_HOR:		HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"%s, ", "H");		break;
				case eDxSAT_POLAR_VER:		HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"%s, ", "V");		break;
				case eDxSAT_POLAR_LEFT: 	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"%s, ", "L");		break;
				case eDxSAT_POLAR_RIGHT:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"%s, ", "R");		break;
				default:					HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"NOT, ");			break;
			}
			HxSTD_StrCat((HCHAR*)pString, (HCHAR*)szTextBuffer);

			/* symbol rate */
			HxSTD_memset (szTextBuffer, 0x00, NX_TEXT_SIZE_16);
			szTextBuffer[0] = '\0';
			HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"%d, ", (int)pstTp->sat.tuningInfo.ulSymbolRate);
			HxSTD_StrCat ((HCHAR*)pString, (HCHAR*)szTextBuffer);

			/* FEC */
			HxSTD_memset (szTextBuffer, 0x00, NX_TEXT_SIZE_16);
			szTextBuffer[0] = '\0';
			switch (pstTp->sat.tuningInfo.eFecCodeRate)
			{
				case eDxSAT_CODERATE_1_2:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"1/2"); 	break;
				case eDxSAT_CODERATE_2_3:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"2/3"); 	break;
				case eDxSAT_CODERATE_3_4:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"3/4"); 	break;
				case eDxSAT_CODERATE_3_5:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"3/5"); 	break;
				case eDxSAT_CODERATE_4_5:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"4/5"); 	break;
				case eDxSAT_CODERATE_5_11:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"5/11");	break;
				case eDxSAT_CODERATE_5_6:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"5/6"); 	break;
				case eDxSAT_CODERATE_6_7:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"6/7"); 	break;
				case eDxSAT_CODERATE_7_8:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"7/8"); 	break;
				case eDxSAT_CODERATE_8_9:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"8/9"); 	break;
				case eDxSAT_CODERATE_9_10:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"9/10");	break;
				case eDxSAT_CODERATE_AUTO:	HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"A");		break;
				case eDxSAT_CODERATE_NONE:
				default:
					HxSTD_snprintf((HCHAR*)szTextBuffer, NX_TEXT_SIZE_16, (HCHAR*)"NOT");
					break;
			}
			HxSTD_StrCat ((HCHAR*)pString, (HCHAR*)szTextBuffer);

			break;

		case eDxDELIVERY_TYPE_TER:
			HxSTD_snprintf((HCHAR*)pString, NX_TEXT_SIZE_64, (HCHAR*)"%d kHz", (int)pstTp->ter.ulFrequency);

			/* bandwidth */
			switch(pstTp->ter.eBandWidth)
			{
				case eDxTER_BANDWIDTH_6M:	HxSTD_StrCat((HCHAR*)pString, (HCHAR *)", 6MHz");	break;
				case eDxTER_BANDWIDTH_7M:	HxSTD_StrCat((HCHAR*)pString, (HCHAR *)", 7MHz");	break;
				case eDxTER_BANDWIDTH_8M:	HxSTD_StrCat((HCHAR*)pString, (HCHAR *)", 8MHz");	break;
				default:					HxSTD_StrCat((HCHAR*)pString, (HCHAR *)", NONE");	break;
			}
			break;

		case eDxDELIVERY_TYPE_CAB:
			HxSTD_memset (szTextBuffer, 0x00, NX_TEXT_SIZE_16);
			szTextBuffer[0] = '\0';
			switch (pstTp->cab.constellation)
			{
				case eDxCAB_CONSTELLATION_16QAM: 	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"16");	break;
				case eDxCAB_CONSTELLATION_32QAM: 	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"32");	break;
				case eDxCAB_CONSTELLATION_64QAM:	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"64");	break;
				case eDxCAB_CONSTELLATION_128QAM:	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"128");	break;
				case eDxCAB_CONSTELLATION_256QAM:	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"256");	break;
				case eDxCAB_CONSTELLATION_1024QAM:	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"1024");	break;
				case eDxCAB_CONSTELLATION_4096QAM:	HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"4096");	break;
				case eDxCAB_CONSTELLATION_AUTO:		HxSTD_StrCpy((HCHAR*)szTextBuffer, (HCHAR*)"AUTO");	break;
			}
			HxSTD_snprintf((HCHAR*)pString, (NX_TEXT_SIZE_32 - 1), (HCHAR*)"%d kHz SR %d %s QAM", (int)pstTp->cab.ulFrequency, (int)pstTp->cab.ulSymbolRate, (HCHAR*)szTextBuffer);
			break;

		default:
			break;
	}
}


GWM_UiState_t NX_PRISM_UTIL_GetUiState (void)
{

	if (ONDK_GWM_APP_CountAppByUiState (UISTATE_INSTALLWIZARD) > 0)
	{
		return UISTATE_INSTALLWIZARD;
	}
	else if (ONDK_GWM_APP_CountAppByUiState (UISTATE_MENU) > 0)
	{
		return UISTATE_MENU;
	}
	else if (ONDK_GWM_APP_CountAppByUiState (UISTATE_EPG) > 0)
	{
		return UISTATE_EPG;
	}
	else if (ONDK_GWM_APP_CountAppByUiState (UISTATE_RECORDINGS) > 0)
	{
		return UISTATE_RECORDINGS;
	}
	else if (ONDK_GWM_APP_CountAppByUiState (UISTATE_MEDIA) > 0)
	{
		return UISTATE_MEDIA;
	}
	else if (ONDK_GWM_APP_CountAppByUiState (UISTATE_SCHEDULE) > 0)
	{
		return UISTATE_SCHEDULE;
	}

	return UISTATE_NORMAL;
}


HINT8	*NX_PRISM_UTIL_GetResStringIdByResolution(DxResolutionSelect_e eResolution)
{
	switch (eResolution)
	{
		case eDxRESOLUTION_SELECT_480I:		return	RES_480I_ID;
		case eDxRESOLUTION_SELECT_480P:		return	RES_480P_ID;
		case eDxRESOLUTION_SELECT_576I:		return	RES_576I_ID;
		case eDxRESOLUTION_SELECT_576P:		return	RES_576P_ID;
		case eDxRESOLUTION_SELECT_720P:		return	RES_720P_ID;
		case eDxRESOLUTION_SELECT_1080I:	return	RES_1080I_ID;
		case eDxRESOLUTION_SELECT_1080P:	return	RES_1080P_ID;
		case eDxRESOLUTION_SELECT_2160P:	return	RES_2160P_ID;
		default:							return	RES_UNKNOWN_ID;
	}
}

HINT8	*NX_PRISM_UTIL_GetResStringIdByCountryID(HxCountry_e eCountryId)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!!! NX_SYSTEM_GetOperatorType() \n");
	}

	switch (eCountryId)
    {
        case eCountryID_GTM:	return	RES_GUATEMALA_ID;
        case eCountryID_PAN:	return	RES_PANAMA_ID;
        case eCountryID_DOM:	return	RES_DOMINICA_REPUBLIC_ID;
        case eCountryID_SLV:	return	RES_EL_SALVADOR_ID;
        case eCountryID_NIC:	return	RES_NICARAGUA_ID;
        case eCountryID_CRI:	return	RES_COSTA_RICA_ID;
        case eCountryID_HND:	return	RES_HONDURAS_ID;
		case eCountryID_BHR:	return	RES_BAHRAIN_ID;
		case eCountryID_EGY:	return	RES_EGYPT_ID;
		case eCountryID_GRC:	return	RES_GREECE_ID;
		case eCountryID_IRN:	return	RES_IRAN_ID;
		case eCountryID_IRQ:	return	RES_IRAQ_ID;
		case eCountryID_KWT:	return	RES_KUWAIT_ID;
		case eCountryID_OMN:	return	RES_OMAN_ID;
		case eCountryID_QAT:	return	RES_QATAR_ID;
		case eCountryID_SAU:	return	RES_SAUDI_ARABIA_ID;
		case eCountryID_TUR:	return	RES_TURKEY_ID;
		case eCountryID_ARE:	return	RES_UAE_ID;
		case eCountryID_TUN:	return	RES_TUNISIA_ID;
		case eCountryID_DZA:	return	RES_ALGERIA_ID;
		case eCountryID_MAR:	return	RES_MOROCCO_ID;
		// TODO: Add here
		case eCountryID_OTHER:
			if (eOpType == eDxOPERATOR_AMX)
			{
				return	RES_ALL_COUNTRIES_ID;
			}
			else
			{
				return	RES_UNKNOWN_ID;
			}
		default:				return	RES_UNKNOWN_ID;
    }
}

void	NX_PRISM_UTIL_GetGmtOffsetByCountryID(HxCountry_e eCountryId, HINT32 *plHour, HINT32 *plMinute)
{
	HERROR	hError = ERR_FAIL;

	hError = HLIB_DATETIME_GetTimeOffsetByCountryIndex(eCountryId, plHour, plMinute);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!!! HLIB_DATETIME_GetTimeOffsetByCountryIndex() - eCountryId:(0x%x)\n", eCountryId);
	}
}

HBOOL	NX_PRISM_UTIL_IsSupportResolutionList(DxResolutionSelect_e eResolution)
{
	HBOOL			bSupport = FALSE;
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!!! NX_SYSTEM_GetOperatorType() \n");
	}

	if (eOpType == eDxOPERATOR_AMX)
	{
		switch (eResolution)
		{
			case eDxRESOLUTION_SELECT_480I:
			case eDxRESOLUTION_SELECT_480P:
			case eDxRESOLUTION_SELECT_720P:
			case eDxRESOLUTION_SELECT_1080I:
			case eDxRESOLUTION_SELECT_1080P:
			case eDxRESOLUTION_SELECT_2160P:
				bSupport = TRUE;
				break;
			default:
				bSupport = FALSE;
				break;
		}
	}
	else
	{
		switch (eResolution)
		{
			case eDxRESOLUTION_SELECT_576I:
			case eDxRESOLUTION_SELECT_576P:
			case eDxRESOLUTION_SELECT_720P:
			case eDxRESOLUTION_SELECT_1080I:
			case eDxRESOLUTION_SELECT_1080P:
				bSupport = TRUE;
				break;
			default:
				bSupport = FALSE;
				break;
		}
	}

	return	bSupport;
}


HERROR		NX_PRISM_UTIL_GetGMTOffsetByTime(HINT32 lTime, eNxUiUtilGMTOffset_t *peGMTOffset)
{
	switch (lTime)
	{
		case -720:  *peGMTOffset =  eNxUtil_GMTOffset_m12_00;	break;
		case -660:  *peGMTOffset =  eNxUtil_GMTOffset_m11_00;	break;
		case -600:  *peGMTOffset =  eNxUtil_GMTOffset_m10_00;	break;
		case -540:  *peGMTOffset =  eNxUtil_GMTOffset_m09_00;	break;
		case -480:  *peGMTOffset =  eNxUtil_GMTOffset_m08_00;	break;
		case -420:  *peGMTOffset =  eNxUtil_GMTOffset_m07_00;	break;
		case -360:  *peGMTOffset =  eNxUtil_GMTOffset_m06_00;	break;
		case -300:  *peGMTOffset =  eNxUtil_GMTOffset_m05_00;	break;
		case -240:  *peGMTOffset =  eNxUtil_GMTOffset_m04_00;	break;
		case -210:  *peGMTOffset =  eNxUtil_GMTOffset_m03_30;	break;
		case -180:  *peGMTOffset =  eNxUtil_GMTOffset_m03_00;	break;
		case -120:  *peGMTOffset =  eNxUtil_GMTOffset_m02_00;	break;
		case  -60:  *peGMTOffset =  eNxUtil_GMTOffset_m01_00;	break;
		case    0:  *peGMTOffset =  eNxUtil_GMTOffset_p00_00;	break;
		case   60:  *peGMTOffset =  eNxUtil_GMTOffset_p01_00;	break;
		case  120:  *peGMTOffset =  eNxUtil_GMTOffset_p02_00;	break;
		case  180:  *peGMTOffset =  eNxUtil_GMTOffset_p03_00;	break;
		case  210:  *peGMTOffset =  eNxUtil_GMTOffset_p03_30;	break;
		case  240:  *peGMTOffset =  eNxUtil_GMTOffset_p04_00;	break;
		case  270:  *peGMTOffset =  eNxUtil_GMTOffset_p04_30;	break;
		case  300:  *peGMTOffset =  eNxUtil_GMTOffset_p05_00;	break;
		case  330:  *peGMTOffset =  eNxUtil_GMTOffset_p05_30;	break;
		case  345:  *peGMTOffset =  eNxUtil_GMTOffset_p05_45;	break;
		case  360:  *peGMTOffset =  eNxUtil_GMTOffset_p06_00;	break;
		case  390:  *peGMTOffset =  eNxUtil_GMTOffset_p06_30;	break;
		case  420:  *peGMTOffset =  eNxUtil_GMTOffset_p07_00;	break;
		case  480:  *peGMTOffset =  eNxUtil_GMTOffset_p08_00;	break;
		case  540:  *peGMTOffset =  eNxUtil_GMTOffset_p09_00;	break;
		case  570:  *peGMTOffset =  eNxUtil_GMTOffset_p09_30;	break;
		case  600:  *peGMTOffset =  eNxUtil_GMTOffset_p10_00;	break;
		case  630:  *peGMTOffset =  eNxUtil_GMTOffset_p10_30;	break;
		case  660:  *peGMTOffset =  eNxUtil_GMTOffset_p11_00;	break;
		case  720:  *peGMTOffset =  eNxUtil_GMTOffset_p12_00;	break;
		case  780:  *peGMTOffset =  eNxUtil_GMTOffset_p13_00;	break;
		default:
			*peGMTOffset = 0;
			return ERR_FAIL;
	}

	return ERR_OK;
}


HERROR		NX_PRISM_UTIL_GetTimeByGMTOffset(eNxUiUtilGMTOffset_t eGMTOffset, HINT32 *plHour, HINT32 *plMin)
{
	switch(eGMTOffset)
	{
		case eNxUtil_GMTOffset_m12_00:		*plHour = -12;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_m11_00:		*plHour = -11;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_m10_00:		*plHour = -10;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_m09_00:		*plHour = -9;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_m08_00:		*plHour = -8;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_m07_00:		*plHour = -7;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_m06_00:		*plHour = -6;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_m05_00:		*plHour = -5;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_m04_00:		*plHour = -4;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_m03_30:		*plHour = -3;			*plMin = -30;		break;
		case eNxUtil_GMTOffset_m03_00:		*plHour = -3;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_m02_00:		*plHour = -2;			*plMin = 00;		break;
		case eNxUtil_GMTOffset_m01_00:		*plHour = -1;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p00_00:		*plHour = 0;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p01_00:		*plHour = 1;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p02_00:		*plHour = 2;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p03_00:		*plHour = 3;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p03_30:		*plHour = 3;			*plMin = 30;		break;
		case eNxUtil_GMTOffset_p04_00:		*plHour = 4;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p04_30:		*plHour = 4;			*plMin = 30;		break;
		case eNxUtil_GMTOffset_p05_00:		*plHour = 5;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p05_30:		*plHour = 5;			*plMin = 30;		break;
		case eNxUtil_GMTOffset_p05_45:		*plHour = 5;			*plMin = 45;		break;
		case eNxUtil_GMTOffset_p06_00:		*plHour = 6;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p06_30:		*plHour = 6;			*plMin = 30;		break;
		case eNxUtil_GMTOffset_p07_00:		*plHour = 7;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p08_00:		*plHour = 8;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p09_00:		*plHour = 9;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p09_30:		*plHour = 9;			*plMin = 30;		break;
		case eNxUtil_GMTOffset_p10_00:		*plHour = 10;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p10_30:		*plHour = 10;			*plMin = 30;		break;
		case eNxUtil_GMTOffset_p11_00:		*plHour = 11;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p12_00:		*plHour = 12;			*plMin = 0;			break;
		case eNxUtil_GMTOffset_p13_00:		*plHour = 13;			*plMin = 0;			break;
		default:
			*plHour = 0;
			*plMin = 0;
			return ERR_FAIL;
	}

	return ERR_OK;
}


HINT8	*NX_PRISM_UTIL_GetResStringIdByGMTOffset(eNxUiUtilGMTOffset_t eGMTOffset)
{
	switch(eGMTOffset)
	{
		case eNxUtil_GMTOffset_m12_00:		return	RES_GMT_MINUS_1200_ID;
		case eNxUtil_GMTOffset_m11_00:		return	RES_GMT_MINUS_1100_ID;
		case eNxUtil_GMTOffset_m10_00:		return	RES_GMT_MINUS_1000_ID;
		case eNxUtil_GMTOffset_m09_00:		return	RES_GMT_MINUS_0900_ID;
		case eNxUtil_GMTOffset_m08_00:		return	RES_GMT_MINUS_0800_ID;
		case eNxUtil_GMTOffset_m07_00:		return	RES_GMT_MINUS_0700_ID;
		case eNxUtil_GMTOffset_m06_00:		return	RES_GMT_MINUS_0600_ID;
		case eNxUtil_GMTOffset_m05_00:		return	RES_GMT_MINUS_0500_ID;
		case eNxUtil_GMTOffset_m04_00:		return	RES_GMT_MINUS_0400_ID;
		case eNxUtil_GMTOffset_m03_30:		return	RES_GMT_MINUS_0330_ID;
		case eNxUtil_GMTOffset_m03_00:		return	RES_GMT_MINUS_0300_ID;
		case eNxUtil_GMTOffset_m02_00:		return	RES_GMT_MINUS_0200_ID;
		case eNxUtil_GMTOffset_m01_00:		return	RES_GMT_MINUS_0100_ID;
		case eNxUtil_GMTOffset_p00_00:		return	RES_GMT_0000_ID;
		case eNxUtil_GMTOffset_p01_00:		return	RES_GMT_0100_ID;
		case eNxUtil_GMTOffset_p02_00:		return	RES_GMT_0200_ID;
		case eNxUtil_GMTOffset_p03_00:		return	RES_GMT_0300_ID;
		case eNxUtil_GMTOffset_p03_30:		return	RES_GMT_0330_ID;
		case eNxUtil_GMTOffset_p04_00:		return	RES_GMT_0400_ID;
		case eNxUtil_GMTOffset_p04_30:		return	RES_GMT_0430_ID;
		case eNxUtil_GMTOffset_p05_00:		return	RES_GMT_0500_ID;
		case eNxUtil_GMTOffset_p05_30:		return	RES_GMT_0530_ID;
		case eNxUtil_GMTOffset_p05_45:		return	RES_GMT_0545_ID;
		case eNxUtil_GMTOffset_p06_00:		return	RES_GMT_0600_ID;
		case eNxUtil_GMTOffset_p06_30:		return	RES_GMT_0630_ID;
		case eNxUtil_GMTOffset_p07_00:		return	RES_GMT_0700_ID;
		case eNxUtil_GMTOffset_p08_00:		return	RES_GMT_0800_ID;
		case eNxUtil_GMTOffset_p09_00:		return	RES_GMT_0900_ID;
		case eNxUtil_GMTOffset_p09_30:		return	RES_GMT_0930_ID;
		case eNxUtil_GMTOffset_p10_00:		return	RES_GMT_1000_ID;
		case eNxUtil_GMTOffset_p10_30:		return	RES_GMT_1030_ID;
		case eNxUtil_GMTOffset_p11_00:		return	RES_GMT_1100_ID;
		case eNxUtil_GMTOffset_p12_00:		return	RES_GMT_1200_ID;
		case eNxUtil_GMTOffset_p13_00:		return	RES_GMT_1200_ID;	// TODO: Add String
		case eNxUtil_GMTOffset_Auto:		return	RES_AUTO_ID;
		default:							return 	RES_UNKNOWN_ID;
	}

}


void	NX_PRISM_UTIL_GetMakeChannelBerValue (HINT32 lBer, HINT32 *plDecimal, HINT32 *plFloat, HINT32 *plExp)
{
	HERROR			hError = ERR_FAIL;
	DxOperator_e	eOpType = eDxOPERATOR_None;

	hError = NX_SYSTEM_GetOperatorType(&eOpType);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!!! NX_SYSTEM_GetOperatorType() \n");
	}

	switch (eOpType)
	{
		case eDxOPERATOR_AMX:
			NX_PRISM_UTIL_GetMakeChannelBerValueForAMX(lBer, plDecimal, plFloat, plExp);
			break;
		default:
			NX_PRISM_UTIL_GetMakeChannelBerBase(lBer, plDecimal, plFloat, plExp);
			break;
	}

}


void	NX_PRISM_UTIL_GetMakeChannelBerBase (HINT32 lBer, HINT32 *plDecimal, HINT32 *plFloat, HINT32 *plExp)
{
	HINT32		lExp = 0, lDecimal= 0, lFloat = 0;

	if (lBer > 999999)
	{
		lExp = -2;
		lDecimal = lBer / 1000000;
		lFloat = (lBer % 1000000) / 10000;
	}
	else if (lBer > 99999)
	{
		lExp = -3;
		lDecimal = lBer / 100000;
		lFloat = (lBer % 100000) / 1000;
	}
	else if (lBer > 9999)
	{
		lExp = -4;
		lDecimal = lBer / 10000;
		lFloat = (lBer % 10000) / 100;
	}
	else if (lBer > 999)
	{
		lExp = -5;
		lDecimal = lBer / 1000;
		lFloat = (lBer % 1000) / 10;
	}
	else if (lBer > 99)
	{
		lExp = -6;
		lDecimal = lBer / 100;
		lFloat = lBer % 100;
	}
	else if (lBer > 9)
	{
		lExp = -7;
		lDecimal = lBer / 10;
		lFloat = lBer % 10;
	}
	else
	{
		lExp = -8;
		lDecimal = lBer;
	}

	*plExp = lExp;
	*plDecimal = lDecimal;
	*plFloat = lFloat;
}


void	NX_PRISM_UTIL_GetMakeChannelBerValueForAMX (HINT32 lBer, HINT32 *plDecimal, HINT32 *plFloat, HINT32 *plExp)
{
	HINT32		lExp = 0, lDecimal= 0, lFloat = 0;

	if (lBer > 200000)
	{
		lExp = 0;
		lDecimal = lBer / 1000000;
		lFloat = (lBer % 1000000) / 10000;
	}
	else if ((lBer > 99999) && (lBer <= 200000))
	{
		lExp = -2;
		lDecimal = lBer / 100000;
		lFloat = (lBer % 100000) / 1000;
	}
	else if (lBer > 9999)
	{
		lExp = -3;
		lDecimal = lBer / 10000;
		lFloat = (lBer % 10000) / 100;
	}
	else if (lBer > 999)
	{
		lExp = -4;
		lDecimal = lBer / 1000;
		lFloat = (lBer % 1000) / 10;
	}
	else if (lBer > 99)
	{
		lExp = -5;
		lDecimal = lBer / 100;
		lFloat = lBer % 100;
	}
	else if (lBer > 9)
	{
		lExp = -6;
		lDecimal = lBer / 10;
		lFloat = lBer % 10;
	}
	else
	{
		lExp = -7;
		lDecimal = lBer;
	}

	*plExp = lExp;
	*plDecimal = lDecimal;
	*plFloat = lFloat;
}


/******************************************************************************/
#define _________TEXT_UTIL_Functions_____________________________________________
/******************************************************************************/

void	NX_PRISM_UTIL_OptionDeleteItemTextList(NX_Util_OPTIONList_t *pstOptItemList, HUINT8 **ppucTextArray)
{
	HUINT32			ulIndex = 0, ulArraySize = 0;


	if ((ppucTextArray == NULL) || (pstOptItemList == NULL))
	{
		NX_APP_Error(" Error!!! ppucTextArray Or pstOptItemList is NULL !!!\n");
		return;
	}

	ulArraySize = pstOptItemList->ulOptItemNum;
	if (ulArraySize == 0)
	{
		NX_APP_Error(" Error!!! ulItemList:(0x%x) list ulItemNum is Zero!!\n", pstOptItemList->ulOptItemNum);
		return;
	}

	for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
	{
		// Source Text
		if (ppucTextArray[ulIndex] != NULL)
		{
			NX_APP_Free(ppucTextArray[ulIndex]);
		}
	}

	if (ppucTextArray != NULL)
	{
		NX_APP_Free(ppucTextArray);
	}
}


HUINT8		**NX_PRISM_UTIL_OptionMakeFavouriteItemTextList(eNxUiUtil_OptType_t ulItemType, NX_Util_OPTIONList_t *pstOptItemList)
{
	HUINT32 		ulIndex = 0, ulArraySize = 0;
	HUINT32 		ulTextLen = 0;
	HUINT8			pucSrcText[NX_TEXT_SIZE_128] = {'\0',}, *pucDstText = NULL;
	HUINT8			**ppucTextArray = NULL;


	ulArraySize = pstOptItemList->ulOptItemNum;
	if (ulArraySize == 0)
	{
		NX_APP_Error(" Error!!! ulItemList list ulItemNum is Zero!!\n");
		return	NULL;
	}

	ppucTextArray = (HUINT8 **)NX_APP_Malloc(sizeof(HUINT8 *) * ulArraySize);
	if (ppucTextArray == NULL)
	{
		return	NULL;
	}

	HxSTD_memset(ppucTextArray, 0, sizeof(HUINT8 *) * ulArraySize);
	for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
	{
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)pstOptItemList->aeOptItemList[ulIndex], (HCHAR*)pucSrcText);
		if (pucSrcText[0] == '\0')
		{
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		ulTextLen  = HxSTD_StrLen ((char*)pucSrcText);

		// Destination Text
		pucDstText = (HUINT8 *)NX_APP_Malloc (ulTextLen + 1);
		if (pucDstText == NULL)
		{
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		HxSTD_memset(pucDstText, 0x0, ulTextLen + 1);
		pucDstText[0] = '\0';

		HxSTD_StrCpy ((char*)pucDstText, (char*)pucSrcText);
		ppucTextArray[ulIndex] = pucDstText;
	}

	NX_COMMON_NOT_USED_PARAM(ulItemType);

	return	ppucTextArray;
}


HUINT8		**NX_PRISM_UTIL_OptionMakeChannlGroupItemTextList(eNxUiUtil_OptType_t ulItemType, NX_Util_OPTIONList_t *pstOptItemList)
{
	HUINT32 		ulIndex = 0, ulArraySize = 0;
	HUINT32 		ulTextLen = 0;
	HUINT8			pucSrcText[NX_TEXT_SIZE_128] = {'\0',}, *pucDstText = NULL;
	HUINT8			**ppucTextArray = NULL;


	ulArraySize = pstOptItemList->ulOptItemNum;
	if (ulArraySize == 0)
	{
		NX_APP_Error(" Error!!! ulItemList list ulItemNum is Zero!!\n");
		return	NULL;
	}

	ppucTextArray = (HUINT8 **)NX_APP_Malloc(sizeof(HUINT8 *) * ulArraySize);
	if (ppucTextArray == NULL)
	{
		return	NULL;
	}

	HxSTD_memset(ppucTextArray, 0, sizeof(HUINT8 *) * ulArraySize);
	for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
	{
		NX_PRISM_UTIL_GetResStringIdByChannelListGroupType((NX_ChListGroup_t)pstOptItemList->aeOptItemList[ulIndex], (HCHAR*)pucSrcText);
		if (pucSrcText[0] == '\0')
		{
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		ulTextLen  = HxSTD_StrLen ((char*)pucSrcText);

		// Destination Text
		pucDstText = (HUINT8 *)NX_APP_Malloc (ulTextLen + 1);
		if (pucDstText == NULL)
		{
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		HxSTD_memset(pucDstText, 0x0, ulTextLen + 1);
		pucDstText[0] = '\0';

		HxSTD_StrCpy ((char*)pucDstText, (char*)pucSrcText);
		ppucTextArray[ulIndex] = pucDstText;
	}

	NX_COMMON_NOT_USED_PARAM(ulItemType);

	return	ppucTextArray;
}


HUINT8		**NX_PRISM_UTIL_OptionMakeItemTextList(eNxUiUtil_OptType_t ulItemType, NX_Util_OPTIONList_t *pstOptItemList)
{
	HUINT32 		ulIndex = 0, ulArraySize = 0;
	HUINT32 		ulTextLen = 0;
	HUINT8			*pucSrcText = NULL, *pucDstText = NULL;
	HUINT8			**ppucTextArray = NULL;


	ulArraySize = pstOptItemList->ulOptItemNum;
	if (ulArraySize == 0)
	{
		NX_APP_Error(" Error!!! ulItemList list ulItemNum is Zero!!\n");
		return	NULL;
	}

	ppucTextArray = (HUINT8 **)NX_APP_Malloc(sizeof(HUINT8 *) * ulArraySize);
	if (ppucTextArray == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Malloc() Fail!!\n");
		return	NULL;
	}

	HxSTD_memset(ppucTextArray, 0, sizeof(HUINT8 *) * ulArraySize);
	for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
	{
		pucSrcText = (HUINT8*)nx_prism_util_OptionItemResource((eNxUiUtil_OptType_t)ulItemType, pstOptItemList->aeOptItemList[ulIndex]);
		if (pucSrcText == NULL)
		{
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		ulTextLen  = HxSTD_StrLen ((char*)pucSrcText);

		// Destination Text
		pucDstText = (HUINT8 *)NX_APP_Malloc (ulTextLen + 1);
		if (pucDstText == NULL)
		{
			for (ulIndex = 0; ulIndex < ulArraySize; ulIndex++)
			{
				if (ppucTextArray[ulIndex] != NULL)
				{
					NX_APP_Free (ppucTextArray[ulIndex]);
				}
			}

			NX_APP_Free (ppucTextArray);
			return	NULL;
		}

		HxSTD_memset(pucDstText, 0x0, ulTextLen + 1);
		pucDstText[0] = '\0';

		HxSTD_StrCpy ((char*)pucDstText, (char*)pucSrcText);
		ppucTextArray[ulIndex] = pucDstText;
	}

	return	ppucTextArray;
}


void	NX_PRISM_UTIL_OptionAddItemList(NX_Util_OPTIONList_t *pstOptItemList, HUINT32 ulOptItemId)
{
	HUINT32		ulIndex = 0;

	if (pstOptItemList->ulOptItemNum >= NX_UTIL_OPTION_MAX_NUM)
	{
		NX_APP_Error("Error!!! OUT OFRANGE!! pstOptItemList->ulOptItemNum(%d), NX_UTIL_OPTION_MAX_NUM(%d)\n", pstOptItemList->ulOptItemNum, NX_UTIL_OPTION_MAX_NUM);
		return;
	}

	NX_APP_Info("===== Add Option List Item ====\n");
	ulIndex = pstOptItemList->ulOptItemNum;
	NX_APP_Info("== Index:[%d] ====\n", ulIndex);
	NX_APP_Info("== Item ID:[0x%x] ====\n", ulOptItemId);
	pstOptItemList->aeOptItemList[ulIndex] = ulOptItemId;
	pstOptItemList->ulOptItemNum = ulIndex + 1;
	NX_APP_Info("== Item Num:[%d] ====\n", pstOptItemList->ulOptItemNum);
	NX_APP_Info("======================\n");
}


HINT32 	NX_PRISM_UTIL_GetTextLineCount(ONDK_FontId_t eFontId, HUINT32 ulFontSize, HUINT8 *pszMessage, HUINT32 w)
{
	HINT32				lLineCnt = 0;
	HUINT32				ulLineH = 0;
	ONDKFontHandle		*hFontHandle = NULL;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	lLineCnt = ONDK_FONT_GetLineCount(NULL, hFontHandle, 0, 0, w, (HUINT8*)pszMessage, NX_PRISM_FONT_VSPACE,
										(ONDKSTF_EQUALSPACE|ONDKSTF_WORDWRAP), &ulLineH);
	if (lLineCnt < 1)
		lLineCnt = 1;

	if (hFontHandle)
	{
		ONDK_FONT_Release (hFontHandle);
	}

	return	lLineCnt;
}

/******************************************************************************/
#define _________ARRAY_UTIL_Functions_____________________________________________
/******************************************************************************/
void** 	NX_PRISM_UTIL_ArraySlice(void** ppSrcArray, HUINT32 ulSizeOfType, HINT32 lStart, HINT32 lEnd)
{
	void**	ppDstArray = NULL;
	HINT32	lIndex = 0, lDstIndex = 0;
	HUINT32 ulSize = 0;


	ulSize = ulSizeOfType * (lEnd - lStart);
	ppDstArray = (void **)NX_APP_Malloc(ulSize);
	if (ppDstArray == NULL)
	{
		return NULL;
	}
	HxSTD_memset(ppDstArray, 0, ulSize);

	for (lIndex = lStart ; lIndex < lEnd ; lIndex++)
	{
		ppDstArray[lDstIndex++] = ppSrcArray[lIndex];
	}

	return ppDstArray;
}

/******************************************************************************/
#define _________KEY_UTIL_Functions_____________________________________________
/******************************************************************************/
void	NX_PRISM_UTIL_KeyConv_UiCommon(HINT32 *keyCode)
{
	switch(*keyCode)
	{
		case KEY_FRONT_STANDBY:
			*keyCode = KEY_STANDBY;
			break;

		case KEY_FRONT_TVRADIO:
			*keyCode = KEY_TVRADIO;
			break;

		case KEY_FRONT_MENU:
			*keyCode = KEY_MENU;
			break;

		case KEY_FRONT_GUIDE:
			*keyCode = KEY_GUIDE;
			break;

		case KEY_FRONT_OK:
			*keyCode = KEY_OK;
			break;

		case KEY_FRONT_ARROW_UP:
			*keyCode = KEY_ARROWUP;
			break;

		case KEY_FRONT_ARROW_DOWN:
			*keyCode = KEY_ARROWDOWN;
			break;

		case KEY_FRONT_ARROW_LEFT:
			*keyCode = KEY_ARROWLEFT;
			break;

		case KEY_FRONT_ARROW_RIGHT:
			*keyCode = KEY_ARROWRIGHT;
			break;

		case KEY_FRONT_VOLUME_UP:
			*keyCode = KEY_VOLUMEUP;
			break;

		case KEY_FRONT_VOLUME_DOWN:
			*keyCode = KEY_VOLUMEDOWN;
			break;

		case KEY_FRONT_CH_PLUS:
			*keyCode = KEY_CH_PLUS;
			break;

		case KEY_FRONT_CH_MINUS:
			*keyCode = KEY_CH_MINUS;
			break;

		case KEY_FRONT_BACK:
			*keyCode = KEY_BACK;
			break;

		case KEY_SPECIAL_HOME:
			*keyCode = KEY_MENU;
			break;

		case KEY_CUSTOM_SMART_SEARCH:
			*keyCode = KEY_SMART_SEARCH;
			break;

		default:
			break;
	}
}


void	NX_PRISM_UTIL_KeyConv_MenuRoot(HINT32 *keyCode)
{
	switch(*keyCode)
	{
		case KEY_FRONT_ARROW_UP:
			*keyCode = KEY_CH_PLUS;
			break;

		case KEY_FRONT_ARROW_DOWN:
			*keyCode = KEY_CH_MINUS;
			break;

		case KEY_OK:
			*keyCode = KEY_CHLIST;
			break;

		case KEY_FRONT_OK:
			*keyCode = KEY_CHLIST;
			break;

		case KEY_FRONT_ARROW_LEFT:
			*keyCode = KEY_VOLUMEDOWN;
			break;

		case KEY_FRONT_ARROW_RIGHT:
			*keyCode = KEY_VOLUMEUP;
			break;

		case KEY_SPECIAL_HOME:
			*keyCode = KEY_MENU;
			break;

		case KEY_CUSTOM_SMART_SEARCH:
			*keyCode = KEY_SMART_SEARCH;
			break;

		default:
			NX_PRISM_UTIL_KeyConv_UiCommon(keyCode);
			break;
	}
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

