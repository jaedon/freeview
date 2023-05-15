/**************************************************************************************/
/**
 * @file oplprogramme_japan.c
 *
 * Defines DVB programme routine
 *
 * @author  seung bum kim (sbkim@humaxdigital.com)
 * @date    2012/06/13
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hlib.h>
#include <silib.h>

#include <apk.h>
#include <oplprogramme_japan.h>

#define	return_if_void(expr)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	return_if(expr, err)	do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	goto_if(expr, pos)		do{if(expr){HxLOG_Debug("[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)

#define ______________GENRE________________________________

#if 0 // genre string
#define OPL_PROGRAMME_GENRE_TEXT_MAX_LEN 100

#define OPL_PROGRAMME_JAPAN_MAX_COUNT 16
#define OPL_PROGRAMME_JAPAN_MAIN_GENRE_MAX 12
#define OPL_PROGRAMME_JAPAN_USER_MAX 3
#define OPL_PROGRAMME_JAPAN_BS_NIBBLE_MAX 2

static const HCHAR	*s_AribMainGenre[] = {
	  "news/reports"
	, "sports"
	, "information/tabloid_show"
	, "dramas"
	, "music"
	, "variety"
	, "movies"
	, "animation/special_effects"
	, "documentary/culture"
	, "theatre/public_performance"
	, "hobby/education"
	, "welfare"
	, NULL
	, NULL
	, NULL
	, "others"
};

static const HCHAR	*s_AribSubGenre[OPL_PROGRAMME_JAPAN_MAIN_GENRE_MAX][OPL_PROGRAMME_JAPAN_MAX_COUNT] ={
{
	  "regular/general"
	, "weather_report"
	, "special_program/documentary"
	, "politics/national_assembly"
	, "economics/market_report"
	, "overseas/international_report"
	, "news_analysis"
	, "discussion/conference"
	, "special_report"
	, "local_program"
	, "traffic_report"
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
},
{
	  "sports_news"
	, "baseball"
	, "soccer"
	, "golf"
	, "other_ball_games"
	, "sumo/combative_sports"
	, "olympic/international_games"
	, "marathon/athletic_sports/swimming"
	, "motor_sports"
	, "marine_sports/winter_sports"
	, "horse_race/public_race"
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
},
{
	  "gossip/tabloid_show"
	, "fashion"
	, "living_home"
	, "health/medical_treatment"
	, "shopping/mail_order_business"
	, "gourmet/cocking"
	, "events"
	, "program_guide/information"
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
},
{
	  "japanese_dramas"
	, "overseas_dramas"
	, "period_dramas"
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
},
{
	  "japanese_rock/pop_music"
	, "overseas_rock/pop_music"
	, "classic/opera"
	, "jazz/fusion"
	, "popular_songs/japanese_popular_songs"
	, "live_concert"
	, "ranking/request_music"
	, "karaoke/amateur_singing_contests"
	, "japanese_ballad/japanese_traditional_music"
	, "hildrens_song"
	, "folk_music/world_music"
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
},
{
	  "quiz"
	, "game"
	, "talk_variety"
	, "comedy_program"
	, "music_variety"
	, "tour_variety"
	, "cocking_variety"
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
},
{
	  "overseas_movies"
	, "japanese_movies"
	, "animation"
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
},
{
	  "japanese_animation"
	, "overseas_animation"
	, "special_effects"
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
},
{
	  "social/current_events"
	, "history/travel_record"
	, "nature/animal/environment"
	, "space/science/medical_science"
	, "culture/traditional_culture"
	, "literature/literary_art"
	, "sports"
	, "total_documentary"
	, "interviews/discussions"
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
},
{
	  "modern_drama/western_style_drama"
	, "musical"
	, "dance/ballet"
	, "comic_story/entertainment"
	, "kabuki/classical_drama"
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
},
{
	  "trip/fishing/outdoor_entertainment"
	, "gardening/pet/handicrafts"
	, "music/art/industrial_art"
	, "japanese_chess/go"
	, "mah_jong/pinball_games"
	, "cars/motorbikes"
	, "computer/tv_games"
	, "conversation/languages"
	, "little_children/schoolchildren"
	, "junior_high_school/high_school_students"
	, "university_students/examinations"
	, "lifelong_education/qualifications"
	, "educational_problem"
	, NULL
	, NULL
	, "others"
},
{
	  "old_aged_persons"
	, "handicapped_persons"
	, "social_welfare"
	, "volunteers"
	, "sign_language"
	, "text(subtitles)"
	, "explanation_on_sound_multiplex_broadcast"
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
}};

static const HCHAR	*s_AribSubUserNibble[OPL_PROGRAMME_JAPAN_USER_MAX][OPL_PROGRAMME_JAPAN_MAX_COUNT] = {
{
	  "tennis"
	, "basketball"
	, "rugby"
	, "football"
	, "boxing"
	, "professional_wrestling"
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
},
{
	  "action_movies"
	, "sf/fantasy_movies"
	, "comedy_movies"
	, "suspense/mystery_movies"
	, "love/romance_movies"
	, "horror/thriller_movies"
	, "western_movies"
	, "drama/social_drama"
	, "animation_movies"
	, "documentary_movies"
	, "adventure_movies"
	, "musical_movies"
	, "domestic_drama"
	, NULL
	, NULL
	, "others"
},
{
	  "action_movies"
	, "sf/fantasy_movies"
	, "comedy"
	, "suspense/mystery_movies"
	, "love/romance_movies"
	, "horror/thriller_movies"
	, "young/campus/idol_dramas"
	, "japanese_gangster/costume_drama"
	, "animation_movies"
	, "documentary_movies"
	, "adventure_movies"
	, "musical_movies"
	, "domestic_drama"
	, NULL
	, NULL
	, "others"
}};

static const HCHAR	*s_AribSubUserBsNibble[OPL_PROGRAMME_JAPAN_BS_NIBBLE_MAX][OPL_PROGRAMME_JAPAN_MAX_COUNT] = {
{
	  "cancellation_possible"
	, "extension_possible"
	, "suspension_possible"
	, "broadcasting_of_another_episode_in_the_same_series_possible"
	, "undecided_programming_frame"
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
},
{
	  "nterruptive_news_available"
	, "extraordinary_service_pertaining_to_this_event_available"
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, NULL
	, "others"
}};
#endif

#define OPL_MAX_GENRE_VALUE_STRING 11
const HCHAR *OPL_PROGRAMME_JAPAN_GetGenreValue (HCHAR *genre)
{
	static char s_valueInt[OPL_MAX_GENRE_VALUE_STRING];
	HUINT32 ulRetVal = 0;
	HUINT8 ucMajorNibble =0xff, ucMinorNibble =0xff, ucUserNibble1 =0xff, ucUserNibble2 =0xff;

	if( genre == NULL )
		return NULL;

#if 0 // sbkim, string - jlabs 장르 표현 변경에 따라서 막는다.
	HCHAR szCpGenre[OPL_PROGRAMME_GENRE_TEXT_MAX_LEN];
	HCHAR *pszTokenGenre = NULL;
	HUINT8  i =0, j =0;

	memset( szCpGenre, 0x00, OPL_PROGRAMME_GENRE_TEXT_MAX_LEN );
	strncpy( szCpGenre, genre , OPL_PROGRAMME_GENRE_TEXT_MAX_LEN );

	pszTokenGenre = strtok ( szCpGenre, ":" );

	HxLOG_Print( "[%s]\n" , pszTokenGenre );

	if( pszTokenGenre != NULL )
	{/* find main genre */
		for( i=0 ; i< OPL_PROGRAMME_JAPAN_MAX_COUNT; i++ )
		{
			if( strcmp ( pszTokenGenre, s_AribMainGenre[i] ) == 0 )
			{
				ucMajorNibble = i;
				break;
			}
		}
	}

	if( i >= OPL_PROGRAMME_JAPAN_MAX_COUNT )
	{/* not found */
		return NULL;
	}

	pszTokenGenre = strtok ( NULL, ":" );
	while( pszTokenGenre )
	{
		HxLOG_Print( "[%s]\n" , pszTokenGenre );

		for( i=0 ; i< OPL_PROGRAMME_JAPAN_MAX_COUNT; i++ )
		{/* find sub genre */
		 	if( s_AribSubGenre[ucMajorNibble][i] == NULL )
				continue;

			if( strcmp ( pszTokenGenre, s_AribSubGenre[ucMajorNibble][i] ) == 0 )
			{
				ucMinorNibble = i;
				break;
			}
		}

		if( i >= OPL_PROGRAMME_JAPAN_MAX_COUNT )
		{
			ucMinorNibble = 0xff;

			for( i=0 ; i< OPL_PROGRAMME_JAPAN_USER_MAX; i++ )
			{/* find user nibble */
				for( j =0; j< OPL_PROGRAMME_JAPAN_MAX_COUNT ; j++ )
				{
					if(  s_AribSubUserNibble[i][j] == NULL )
						continue ;

					if( strcmp ( pszTokenGenre, s_AribSubUserNibble[i][j] ) == 0 )
					{
						ucMajorNibble = 0x0e;
						ucMinorNibble = 0x01;
						ucUserNibble1 = i;
						ucUserNibble2 = j;
						break;
					}
				}
				if( j < OPL_PROGRAMME_JAPAN_MAX_COUNT )
					break;
			}

			if( i >= OPL_PROGRAMME_JAPAN_USER_MAX && j>= OPL_PROGRAMME_JAPAN_MAX_COUNT )
			{/* 없을 경우 검색 안되게 0xf로 세팅 */
				ucUserNibble1 = 0xff;
				ucUserNibble2 = 0xff;
			}
		}
		else
		{/* 없을 경우 검색 안되게 0xf로 세팅 */
			ucUserNibble1 = 0xff;
			ucUserNibble2 = 0xff;
		}
		pszTokenGenre = strtok ( NULL, ":" );
	}
#else
	memset( s_valueInt, 0x00 , OPL_MAX_GENRE_VALUE_STRING );

	s_valueInt[0] = genre[0];
	s_valueInt[1] = '\0';
	ucMajorNibble = HLIB_STD_HexToInt(s_valueInt);

	memset( s_valueInt, 0x00 , OPL_MAX_GENRE_VALUE_STRING );
	if( genre[1] == '*' )
	{
		ucMinorNibble = 0xff;
	}
	else
	{
		s_valueInt[0] = genre[1];
		s_valueInt[1] = '\0';
		ucMinorNibble = HLIB_STD_HexToInt(s_valueInt);
	}
#endif

	memset( s_valueInt, 0x00, OPL_MAX_GENRE_VALUE_STRING );

	if( (ucMajorNibble != 0) || (ucMinorNibble != 0) || (ucUserNibble1 != 0) || (ucUserNibble2 != 0))
	{
		ulRetVal = (HUINT32)((ucMajorNibble << 24) | (ucMinorNibble << 16) | (ucUserNibble1 << 8) | ucUserNibble2);
	}
	else
	{/* not found */
		return NULL;
	}
	HxSTD_PrintToStrN(s_valueInt, OPL_MAX_GENRE_VALUE_STRING, "%d", ulRetVal);

	HxLOG_Print( "\n\n\n[Major : 0x%x][Minor 0x%x][Total : 0x%x][String ; %s] \n\n\n" , ucMajorNibble, ucMinorNibble, ulRetVal, s_valueInt );

	return s_valueInt;
}

HCHAR *	OPL_PROGRAMME_JAPAN_GetGenre (HUINT16 content)
{
	HUINT8 			contentNibble =0, userNibble =0;
	HUINT8 			ucMajorNibble =0, ucMinorNibble =0, ucUserNibble1 =0, ucUserNibble2 =0;
	HINT32			sd = 0;

	contentNibble = (HUINT8)(content >> 8);
	userNibble = (HUINT8)(content | 0xff );

	ucMajorNibble = contentNibble >> 4;
	ucMinorNibble = contentNibble & 0xf;
	ucUserNibble1 = userNibble >> 4;
	ucUserNibble2 = userNibble & 0xf;

	HxLOG_Print("[%s,%d] major : %d \n" , __FUNCTION__ , __LINE__, ucMajorNibble);
	HxLOG_Print("[%s,%d] minor : %d \n" , __FUNCTION__ , __LINE__, ucMinorNibble);
	HxLOG_Print("[%s,%d] user1 : %d \n" , __FUNCTION__ , __LINE__, ucUserNibble1 );
	HxLOG_Print("[%s,%d] user2 : %d \n" , __FUNCTION__ , __LINE__, ucUserNibble2 );

	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc, HLIB_STD_MemFree);
	if( sd == 0 )
		return NULL;

	if (ucMajorNibble <= 0x0f)
	{
		if (ucMajorNibble == 0x0e)
		{// CS 확장 장르인 경우
			if (ucMinorNibble == 0x00)
			{// BS 디지털 방송용 프로그램 부속 정보 Other
				/* do nothing */
				// sbkim, if string HLIB_RWSTREAM_Print(sd, "%s:%s", s_AribMainGenre[ucMajorNibble] , s_AribSubUserBsNibble[ucUserNibble1][ucUserNibble2]);
				return NULL;
			}
			else if (ucMinorNibble == 0x01)
			{// 광대역 CS 디지털 방송 확장용 정보
				if( ucUserNibble1== 0x00 )
				{/* 스포츠는 메인에 바로 이어서 */
					HLIB_RWSTREAM_Print(sd, "%x%x",  0x1/* sport */, 0xf /* other */ );
					// sbkim, if string HLIB_RWSTREAM_Print(sd, "%s:%s", s_AribMainGenre[ucMajorNibble] , s_AribSubUserNibble[ucUserNibble1][ucUserNibble2]);
				}
				else if( ucUserNibble1== 0x01 )
				{/* 영화는 더 붙여서 */
					HUINT8 ucTempMinor = 0;

					if( ucUserNibble1 == 0x01 )
					{
						ucTempMinor = 0;
					}
					else if( ucUserNibble2 == 0x02 )
					{
						ucTempMinor = 1;
					}
					else
					{
						return NULL;
					}

					HLIB_RWSTREAM_Print(sd, "%x%x",  0x6/* movies */, 0xf /* other */ );
					// sbkim, if string HLIB_RWSTREAM_Print(sd, "%s:%s:%s", s_AribMainGenre[ucMajorNibble] , s_AribSubGenre[ucMajorNibble][ucTempMinor], s_AribSubUserNibble[ucUserNibble1][ucUserNibble2] );
				}
				else
				{
					return NULL;
				}
			}
			else
			{
				return NULL;
			}
		}
		else
		{// BS 표준 장르인 경우
			HLIB_RWSTREAM_Print(sd, "%x%x",  ucMajorNibble, ucMinorNibble );
			// sbkim, if string HLIB_RWSTREAM_Print(sd, "%s:%s", s_AribMainGenre[ucMajorNibble] , s_AribSubGenre[ucMajorNibble][ucMinorNibble]);
		}
	}

	return (char *)HLIB_RWSTREAM_CloseWithoutBuf(sd);
}

HFLOAT32 OPL_PROGRAMME_JAPAN_GetVideoAspectRatio(HUINT8 ucVideoCompType)
{
	HFLOAT32 fRetVal = 0;

	switch(ucVideoCompType)
	{
		case eProgrammeVideoRes_525i_4_3:
		case eProgrammeVideoRes_525p_4_3:
		case eProgrammeVideoRes_1125i_4_3:
		case eProgrammeVideoRes_750p_4_3:
		case eProgrammeVideoRes_1125p_4_3:
			fRetVal = 1.33;
			break;

		case eProgrammeVideoRes_525i_16_9_WPV:
		case eProgrammeVideoRes_525i_16_9_WOPV:
		case eProgrammeVideoRes_525i_16_9_OVER:
		case eProgrammeVideoRes_525p_16_9_WPV:
		case eProgrammeVideoRes_525p_16_9_WOPV:
		case eProgrammeVideoRes_525p_16_9_OVER:
		case eProgrammeVideoRes_1125i_16_9_WPV:
		case eProgrammeVideoRes_1125i_16_9_WOPV:
		case eProgrammeVideoRes_1125i_16_9_OVER:
		case eProgrammeVideoRes_750p_16_9_WPV:
		case eProgrammeVideoRes_750p_16_9_WOPV:
		case eProgrammeVideoRes_750p_16_9_OVER:
		case eProgrammeVideoRes_1125p_16_9_WPV:
		case eProgrammeVideoRes_1125p_16_9_WOPV:
		case eProgrammeVideoRes_1125p_16_9_OVER:
			fRetVal = 1.78;
			break;

		default:
			HxLOG_Error( "[%s,%d] Invalide Type Error\n", __FUNCTION__,  __LINE__ );
			break;
	}

	return fRetVal;
}


HERROR OPL_PROGRAMME_JAPAN_GetSvcType (HUINT32 ulSvcType, HINT32 *svcType)
{
	switch (ulSvcType)
	{
		case eDxOrgSVC_TYPE_DIGITAL_TV:
		case eDxOrgSVC_TYPE_ARIB_TEMP_VIDEO:
		case eDxOrgSVC_TYPE_ARIB_PROMOTION_VIDEO:
			*svcType = eDxSVC_TYPE_TV;
			return ERR_OK;

		case eDxOrgSVC_TYPE_DIGITAL_RADIO:
		case eDxOrgSVC_TYPE_ARIB_TEMP_AUDIO:
		case eDxOrgSVC_TYPE_ARIB_PROMOTION_AUDIO:
			*svcType = eDxSVC_TYPE_RADIO;
			return ERR_OK;

		case eDxOrgSVC_TYPE_DATA:
		case eDxOrgSVC_TYPE_ARIB_TEMP_DATA:
		case eDxOrgSVC_TYPE_ARIB_PROMOTION_DATA:
		case eDxOrgSVC_TYPE_ARIB_BEFORE_ACCUMUL_DATA:
		case eDxOrgSVC_TYPE_ARIB_ACCUMUL_DATA:
		case eDxOrgSVC_TYPE_ARIB_BOOKMARK_LIST_DATA:
			*svcType = eDxSVC_TYPE_DATA;
			return ERR_OK;

		default :
			*svcType = eDxSVC_TYPE_All;
			break;
	}

	return ERR_FAIL;
}

HERROR	OPL_PROGRAMME_JAPAN_InterpretCopyProtectInfo(AribCpInfo_t stAribCpInfo, HBOOL bPlayMode, AribCpOutputInfo_t *pCpOutput)
{
	if(pCpOutput == NULL)
	{
		return ERR_FAIL;
	}

	HxLOG_Print("\n[%s,%d]ucCopyRestrictionMode : %d \n" , __FUNCTION__, __LINE__, stAribCpInfo.ucCopyRestrictionMode);
	HxLOG_Print("\n[%s,%d]ucImageContraintToken : %d \n" , __FUNCTION__, __LINE__, stAribCpInfo.ucImageContraintToken);
	HxLOG_Print("\n[%s,%d]ucRetentionMode : %d \n" , __FUNCTION__, __LINE__, stAribCpInfo.ucRetentionMode);
	HxLOG_Print("\n[%s,%d]ucRetentionState : %d \n" , __FUNCTION__, __LINE__, stAribCpInfo.ucRetentionState);
	HxLOG_Print("\n[%s,%d]ucEncryptionMode : %d \n" , __FUNCTION__, __LINE__, stAribCpInfo.ucEncryptionMode);
	HxLOG_Print("\n[%s,%d]ucCopyRestrictionMode : %d \n" , __FUNCTION__, __LINE__, stAribCpInfo.ucCopyRestrictionMode);
	HxLOG_Print("\n[%s,%d]ucAnalogRecording : %d \n" , __FUNCTION__, __LINE__, stAribCpInfo.ucAnalogRecording);
	HxLOG_Print("\n[%s,%d]ucCopyControlType : %d \n" , __FUNCTION__, __LINE__, stAribCpInfo.ucCopyControlType);
	HxLOG_Print("\n[%s,%d]ucDigitalRecording : %d \n" , __FUNCTION__, __LINE__, stAribCpInfo.ucDigitalRecording);
	HxLOG_Print("\n[%s,%d]usSvcType : %d \n" , __FUNCTION__, __LINE__, stAribCpInfo.usSvcType);

	/* ARIB TR-B14 제3분책 제8편 5.5.2 재카피 금지 */
	if((bPlayMode == TRUE) && (stAribCpInfo.ucDigitalRecording == ARIB_COPY_NOMORE))
	{
		stAribCpInfo.ucDigitalRecording = ARIB_COPY_NEVER;
	}

	/* 에외 처리 SvcType */
	if( stAribCpInfo.usSvcType == 0 )
	{
		/* ALL -> TV로 강제 변환 */
		 stAribCpInfo.usSvcType = 0x01;
	}

	/* ARIB TR-B14 제3분책 제8편 표5-1 */
	if(stAribCpInfo.usSvcType == 0x01)	/* eSvcType_Tv : db_svc에 있는 enum이라서 값을 사용할 수 밖에 없다. */
	{
		switch(stAribCpInfo.ucCopyControlType)
		{
			/* copy_control_type : 01 */
			case 0x01:
			{
				/* digital_recording_control_data : 00 */
				if (stAribCpInfo.ucDigitalRecording == 0x00)
				{
					if (stAribCpInfo.ucEncryptionMode == 0)
					{
						pCpOutput->eMpegTsOutput = ARIB_COPY_ONCE;/*F4*/
						pCpOutput->eMpegTsIpOutput  = ARIB_COPY_FREE_WITH_EPN;/*G4*/
					}
					else
					{
						pCpOutput->eMpegTsOutput	= ARIB_COPY_FREE;/*F3*/
						pCpOutput->eMpegTsIpOutput  = ARIB_COPY_FREE;/*G3*/
					}
					pCpOutput->eDigitalOutput = ARIB_COPY_FREE;/*H3*/
					pCpOutput->eAnalogOutput = ARIB_COPY_FREE;/*I3*/
				}
				/* digital_recording_control_data : 10 */
				else if(stAribCpInfo.ucDigitalRecording == 0x02)
				{
					if(stAribCpInfo.ucCopyRestrictionMode == 0)
					{
						pCpOutput->eDigitalOutput  = ARIB_COPY_ONCE;/*H6*/
					}
					else
					{
						pCpOutput->eDigitalOutput  = ARIB_COPY_10;/*H5*/
					}

					pCpOutput->eMpegTsOutput = ARIB_COPY_ONCE;/*F5*/
					pCpOutput->eMpegTsIpOutput  = ARIB_COPY_ONCE;/*G5*/
					pCpOutput->eAnalogOutput  = ARIB_COPY_FREE;/*I5*/
				}
				/* digital_recording_control_data : 01 */
				else if(stAribCpInfo.ucDigitalRecording == 0x01)
				{
					pCpOutput->eDigitalOutput  = ARIB_OUTPUT_NEVER;/*Q7*/
					pCpOutput->eAnalogOutput  = ARIB_OUTPUT_NEVER;/*R7*/
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F7*/
					pCpOutput->eMpegTsIpOutput  = ARIB_OUTPUT_NEVER;/*G7*/
				}
				/* digital_recording_control_data : 11*/
				else if(stAribCpInfo.ucDigitalRecording == 0x03)
				{
					pCpOutput->eMpegTsOutput = ARIB_COPY_NEVER;/*F8*/
					pCpOutput->eMpegTsIpOutput  = ARIB_COPY_NEVER;/*G8*/
					pCpOutput->eDigitalOutput  = ARIB_COPY_NEVER;/*H8*/
					pCpOutput->eAnalogOutput  = stAribCpInfo.ucAnalogRecording;/*I8*/
				}
				/* invalid */
				else
				{
					pCpOutput->eMpegTsOutput = ARIB_COPY_NEVER;
					pCpOutput->eMpegTsIpOutput  = ARIB_COPY_NEVER;
					pCpOutput->eDigitalOutput  = ARIB_COPY_NEVER;
					pCpOutput->eAnalogOutput  = ARIB_COPY_NEVER;
				}
				break;
			}
			/* copy_control_type : 11 */
			case 0x03:
			{
				/* digital_recording_control_data : 00 */
				if (stAribCpInfo.ucDigitalRecording == 0x00)
				{
					pCpOutput->eDigitalOutput  = ARIB_OUTPUT_NEVER;/*Q9*/
					pCpOutput->eAnalogOutput  = ARIB_OUTPUT_NEVER;/*R9*/
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F9*/
					pCpOutput->eMpegTsIpOutput  = ARIB_OUTPUT_NEVER;/*G9*/
				}
				/* digital_recording_control_data : 10 */
				else if(stAribCpInfo.ucDigitalRecording == 0x02)
				{
					pCpOutput->eDigitalOutput  = ARIB_OUTPUT_NEVER;/*Q10*/
					pCpOutput->eAnalogOutput  = ARIB_OUTPUT_NEVER;/*R10*/
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F10*/
					pCpOutput->eMpegTsIpOutput  = ARIB_OUTPUT_NEVER;/*G10*/
				}
				/* digital_recording_control_data : 01 */
				else if(stAribCpInfo.ucDigitalRecording == 0x01)
				{
					pCpOutput->eDigitalOutput  = ARIB_OUTPUT_NEVER;/*Q11*/
					pCpOutput->eAnalogOutput  = ARIB_OUTPUT_NEVER;/*R11*/
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F11*/
					pCpOutput->eMpegTsIpOutput  = ARIB_OUTPUT_NEVER;/*G11*/
				}
				/* digital_recording_control_data : 11*/
				else if(stAribCpInfo.ucDigitalRecording == 0x03)
				{
					pCpOutput->eDigitalOutput  = ARIB_OUTPUT_NEVER;/*Q12*/
					pCpOutput->eAnalogOutput  = ARIB_OUTPUT_NEVER;/*R12*/
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F12*/
					pCpOutput->eMpegTsIpOutput  = ARIB_OUTPUT_NEVER;/*G12*/
				}
				/* invalid */
				else
				{
					pCpOutput->eMpegTsOutput = ARIB_COPY_NEVER;
					pCpOutput->eMpegTsIpOutput  = ARIB_COPY_NEVER;
					pCpOutput->eDigitalOutput  = ARIB_COPY_NEVER;
					pCpOutput->eAnalogOutput  = ARIB_COPY_NEVER;
				}
				break;
			}
			/* copy_control_type : 10 or 00 */
			case 0x02:
			case 0x00:
			{
				pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F13*/
				pCpOutput->eMpegTsIpOutput  = ARIB_OUTPUT_NEVER;/*G13*/
				pCpOutput->eDigitalOutput  = ARIB_OUTPUT_NEVER;/*H13*/
				pCpOutput->eAnalogOutput  = ARIB_OUTPUT_NEVER;/*I13*/
				break;
			}
			/* invalid */
			default:
			{
				pCpOutput->eMpegTsOutput = ARIB_COPY_NEVER;
				pCpOutput->eMpegTsIpOutput  = ARIB_COPY_NEVER;
				pCpOutput->eDigitalOutput  = ARIB_COPY_NEVER;
				pCpOutput->eAnalogOutput  = ARIB_COPY_NEVER;
				break;
			}
		}
	}
	else if(stAribCpInfo.usSvcType == 0x02)	/* eSvcType_Radio : db_svc에 있는 enum이라서 값을 사용할 수 밖에 없다. */
	{
		switch(stAribCpInfo.ucCopyControlType)
		{
			/* copy_control_type : 01 */
			case 0x01:
			{
				/* digital_recording_control_data : 00 */
				if (stAribCpInfo.ucDigitalRecording == 0x00)
				{
					if (stAribCpInfo.ucEncryptionMode == 0)
					{
						pCpOutput->eMpegTsIpOutput = ARIB_COPY_FREE_WITH_EPN;/*G17*/
					}
					else
					{
						pCpOutput->eMpegTsIpOutput = ARIB_COPY_FREE;/*G16*/
					}
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F16*/
					pCpOutput->eDigitalOutput = ARIB_COPY_FREE;/*H16*/
					pCpOutput->eAnalogOutput = ARIB_COPY_FREE;/*I16*/
				}
				/* digital_recording_control_data : 10 */
				else if(stAribCpInfo.ucDigitalRecording == 0x02)
				{
					if(stAribCpInfo.ucCopyRestrictionMode == 0)
					{
						pCpOutput->eDigitalOutput = ARIB_COPY_ONCE;/*H19*/
					}
					else
					{
						pCpOutput->eDigitalOutput = ARIB_COPY_10;/*H18*/
					}

					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F18*/
					pCpOutput->eMpegTsIpOutput = ARIB_COPY_ONCE;/*G18*/
					pCpOutput->eAnalogOutput = ARIB_COPY_FREE;/*I18*/
				}
				/* digital_recording_control_data : 01 */
				else if(stAribCpInfo.ucDigitalRecording == 0x01)
				{
					pCpOutput->eDigitalOutput = ARIB_OUTPUT_NEVER;/*Q20*/
					pCpOutput->eAnalogOutput = ARIB_OUTPUT_NEVER;/*R20*/
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F20*/
					pCpOutput->eMpegTsIpOutput = ARIB_OUTPUT_NEVER;/*G20*/
				}
				/* digital_recording_control_data : 11*/
				else if(stAribCpInfo.ucDigitalRecording == 0x03)
				{
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F21*/
					pCpOutput->eMpegTsIpOutput = ARIB_COPY_NOMORE;/*G21*/
					pCpOutput->eDigitalOutput = ARIB_COPY_NEVER;/*H21*/
					pCpOutput->eAnalogOutput = stAribCpInfo.ucAnalogRecording;/*I21*/
				}
				/* invalid */
				else
				{
					pCpOutput->eMpegTsOutput = ARIB_COPY_NEVER;
					pCpOutput->eMpegTsIpOutput = ARIB_COPY_NEVER;
					pCpOutput->eDigitalOutput = ARIB_COPY_NEVER;
					pCpOutput->eAnalogOutput = ARIB_COPY_NEVER;
				}
				break;
			}
			/* copy_control_type : 11 */
			case 0x03:
			{
				/* digital_recording_control_data : 00 */
				if(stAribCpInfo.ucDigitalRecording == 0x00)
				{
					pCpOutput->eMpegTsOutput = ARIB_COPY_FREE;/*F22*/
					pCpOutput->eMpegTsIpOutput = ARIB_COPY_FREE;/*G22*/
					pCpOutput->eDigitalOutput = ARIB_COPY_FREE;/*H22*/
					pCpOutput->eAnalogOutput = ARIB_COPY_FREE;/*I22*/
				}
				/* digital_recording_control_data : 10 */
				else if(stAribCpInfo.ucDigitalRecording == 0x02)
				{
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F23*/
					pCpOutput->eMpegTsIpOutput = ARIB_OUTPUT_NEVER;/*G23*/
					pCpOutput->eDigitalOutput = ARIB_COPY_ONCE;/*H23*/
					pCpOutput->eAnalogOutput = ARIB_COPY_FREE;/*I23*/
				}
				/* digital_recording_control_data : 01 */
				else if(stAribCpInfo.ucDigitalRecording == 0x01)
				{
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F24*/
					pCpOutput->eMpegTsIpOutput = ARIB_OUTPUT_NEVER;/*G24*/
					pCpOutput->eDigitalOutput = ARIB_COPY_NEVER;/*H24*/
					pCpOutput->eAnalogOutput = stAribCpInfo.ucAnalogRecording;/*I24*/
				}
				/* digital_recording_control_data : 11 */
				else if(stAribCpInfo.ucDigitalRecording == 0x03)
				{
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F25*/
					pCpOutput->eMpegTsIpOutput = ARIB_OUTPUT_NEVER;/*G25*/
					pCpOutput->eDigitalOutput = ARIB_COPY_NEVER;/*H25*/
					pCpOutput->eAnalogOutput = stAribCpInfo.ucAnalogRecording;/*I25*/
				}
				/* invalid */
				else
				{
					pCpOutput->eMpegTsOutput = ARIB_COPY_NEVER;
					pCpOutput->eMpegTsIpOutput = ARIB_COPY_NEVER;
					pCpOutput->eDigitalOutput = ARIB_COPY_NEVER;
					pCpOutput->eAnalogOutput = ARIB_COPY_NEVER;
				}
				break;
			}
			/* copy_control_type : 10 or 00 */
			case 0x02:
			case 0x00:
			{
				pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F26*/
				pCpOutput->eMpegTsIpOutput = ARIB_OUTPUT_NEVER;/*G26*/
				pCpOutput->eDigitalOutput = ARIB_OUTPUT_NEVER;/*H26*/
				pCpOutput->eAnalogOutput = ARIB_OUTPUT_NEVER;/*I26*/
				break;
			}
			/* invalid */
			default:
			{
				pCpOutput->eMpegTsOutput = ARIB_COPY_NEVER;
				pCpOutput->eMpegTsIpOutput = ARIB_COPY_NEVER;
				pCpOutput->eDigitalOutput = ARIB_COPY_NEVER;
				pCpOutput->eAnalogOutput = ARIB_COPY_NEVER;
				break;
			}
		}
	}
	else
	{
		switch(stAribCpInfo.ucCopyControlType)
		{
			/* copy_control_type : 01 */
			case 0x01:
			{
				/* digital_recording_control_data : 00 */
				if (stAribCpInfo.ucDigitalRecording == 0x00)
				{
					if (stAribCpInfo.ucEncryptionMode == 0)
					{
						pCpOutput->eMpegTsOutput = ARIB_COPY_ONCE;/*F30*/
						pCpOutput->eMpegTsIpOutput = ARIB_COPY_FREE_WITH_EPN;/*G30*/
					}
					else
					{
						pCpOutput->eMpegTsOutput	= ARIB_COPY_FREE;/*F29*/
						pCpOutput->eMpegTsIpOutput = ARIB_COPY_FREE;/*G29*/
					}
					pCpOutput->eDigitalOutput = ARIB_COPY_FREE;/*H29*/
					pCpOutput->eAnalogOutput = ARIB_COPY_FREE;/*I29*/
				}
				/* digital_recording_control_data : 10 */
				else if(stAribCpInfo.ucDigitalRecording == 0x02)
				{
					if(stAribCpInfo.ucCopyRestrictionMode == 0)
					{
						pCpOutput->eDigitalOutput = ARIB_COPY_ONCE;/*H32*/
					}
					else
					{
						pCpOutput->eDigitalOutput = ARIB_COPY_10;/*H31*/
					}

					pCpOutput->eMpegTsOutput = ARIB_COPY_ONCE;/*F31*/
					pCpOutput->eMpegTsIpOutput = ARIB_COPY_ONCE;/*G31*/
					pCpOutput->eAnalogOutput = ARIB_COPY_FREE;/*I31*/
				}
				/* digital_recording_control_data : 01 */
				else if(stAribCpInfo.ucDigitalRecording == 0x01)
				{
					pCpOutput->eDigitalOutput = ARIB_OUTPUT_NEVER;/*Q33*/
					pCpOutput->eAnalogOutput = ARIB_OUTPUT_NEVER;/*R33*/
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F33*/
					pCpOutput->eMpegTsIpOutput = ARIB_OUTPUT_NEVER;/*G33*/
				}
				/* digital_recording_control_data : 11*/
				else if(stAribCpInfo.ucDigitalRecording == 0x03)
				{
					pCpOutput->eMpegTsOutput = ARIB_COPY_NEVER;/*F34*/
					pCpOutput->eMpegTsIpOutput = ARIB_COPY_NEVER;/*G34*/
					pCpOutput->eDigitalOutput = ARIB_COPY_NEVER;/*H34*/
					pCpOutput->eAnalogOutput = stAribCpInfo.ucAnalogRecording;/*I34*/
				}
				/* invalid */
				else
				{
					pCpOutput->eMpegTsOutput = ARIB_COPY_NEVER;
					pCpOutput->eMpegTsIpOutput = ARIB_COPY_NEVER;
					pCpOutput->eDigitalOutput = ARIB_COPY_NEVER;
					pCpOutput->eAnalogOutput = ARIB_COPY_NEVER;
				}
				break;
			}
			/* copy_control_type : 11 */
			case 0x03:
			{
				/* digital_recording_control_data : 00 */
				if(stAribCpInfo.ucDigitalRecording == 0x00)
				{
					pCpOutput->eMpegTsOutput = ARIB_COPY_FREE;/*F35*/
					pCpOutput->eMpegTsIpOutput = ARIB_COPY_FREE;/*G35*/
					pCpOutput->eDigitalOutput = ARIB_COPY_FREE;/*H35*/
					pCpOutput->eAnalogOutput = ARIB_COPY_FREE;/*I35*/
				}
				/* digital_recording_control_data : 10 */
				else if(stAribCpInfo.ucDigitalRecording == 0x02)
				{
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F36*/
					pCpOutput->eMpegTsIpOutput = ARIB_OUTPUT_NEVER;/*G36*/
					pCpOutput->eDigitalOutput = ARIB_COPY_ONCE;/*H36*/
					pCpOutput->eAnalogOutput = ARIB_COPY_FREE;/*I36*/
				}
				/* digital_recording_control_data : 01 */
				else if(stAribCpInfo.ucDigitalRecording == 0x01)
				{
					pCpOutput->eDigitalOutput = ARIB_OUTPUT_NEVER;/*Q37*/
					pCpOutput->eAnalogOutput = ARIB_OUTPUT_NEVER;/*R37*/
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F37*/
					pCpOutput->eMpegTsIpOutput = ARIB_OUTPUT_NEVER;/*G37*/
				}
				/* digital_recording_control_data : 11 */
				else if(stAribCpInfo.ucDigitalRecording == 0x03)
				{
					pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F38*/
					pCpOutput->eMpegTsIpOutput = ARIB_OUTPUT_NEVER;/*G38*/
					pCpOutput->eDigitalOutput = ARIB_COPY_NEVER;/*H38*/
					pCpOutput->eAnalogOutput = stAribCpInfo.ucAnalogRecording;/*I38*/
				}
				/* invalid */
				else
				{
					pCpOutput->eMpegTsOutput = ARIB_COPY_NEVER;
					pCpOutput->eMpegTsIpOutput = ARIB_COPY_NEVER;
					pCpOutput->eDigitalOutput = ARIB_COPY_NEVER;
					pCpOutput->eAnalogOutput = ARIB_COPY_NEVER;
				}
				break;
			}
			/* copy_control_type : 10 or 00 */
			case 0x02:
			case 0x00:
			{
				pCpOutput->eMpegTsOutput = ARIB_OUTPUT_NEVER;/*F39*/
				pCpOutput->eMpegTsIpOutput = ARIB_OUTPUT_NEVER;/*G39*/
				pCpOutput->eDigitalOutput = ARIB_OUTPUT_NEVER;/*H39*/
				pCpOutput->eAnalogOutput = ARIB_OUTPUT_NEVER;/*I39*/
				break;
			}
			/* invalid */
			default:
			{
				pCpOutput->eMpegTsOutput = ARIB_COPY_NEVER;
				pCpOutput->eMpegTsIpOutput = ARIB_COPY_NEVER;
				pCpOutput->eDigitalOutput = ARIB_COPY_NEVER;
				pCpOutput->eAnalogOutput = ARIB_COPY_NEVER;
				break;
			}
		}
	}

	pCpOutput->eAnalogOutputInCgms = stAribCpInfo.ucAnalogRecording;

	return ERR_OK;
}

static const HCHAR *	s_jp639map[] ={
	 "alb" //Albanian,	eLangID_Albanian
	,"ara" //Arabic,	eLangID_Arabic
	,"aus" //Australian,	eLangID_Australian
	,"baq" //Basque,	eLangID_Basque
	,"bas" //Basaa,	eLangID_Basaa
	,"bul" //Bulgarian,	eLangID_Bulgarian
	,"cat" //Catalan,	eLangID_Catalan
	,"cze" //Czech,	eLangID_Czech
	,"dan" //Danish,	eLangID_Danish
	,"dut" //Dutch,	eLangID_Dutch
	,"eng" //English,	eLangID_English
	,"est" //Estonian,	eLangID_Estonian
	,"fao" //Faroese,	eLangID_Faroese
	,"fin" //Finnish,	eLangID_Finnish
	,"fre" //French,	eLangID_French
	,"ger" //German,	eLangID_German
	,"gla" //Scottish Gaelic,	eLangID_Gaelic
	,"gle" //Irish,	eLangID_Irish
	,"glg" //Galician,	eLangID_Galician
	,"gre" //Greek,	eLangID_Greek
	,"heb" //Hebrew,	eLangID_Hebrew
	,"hin" //Hindi,	eLangID_Hindi
	,"hun" //Hungarian,	eLangID_Hungarian
	,"ice" //Icelandic,	eLangID_Icelandic
	,"ita" //Italian,	eLangID_Italian
	,"jpn" //Japanese,	eLangID_Japanese
	,"kor" //Korean,	eLangID_Korean
	,"lav" //Latvian,	eLangID_Latvian
	,"lit" //Lithuanian,	eLangID_Lithuanian
	,"nar" //Audio Description,	eLangID_Narration
	,"nor" //Norwegian,	eLangID_Norwegian
	,"per" //Persian,	eLangID_Persian
	,"pol" //Polish,	eLangID_Polish
	,"por" //Portuguese,	eLangID_Portuguese
	,"qaa" //Original Language,	eLangID_OriginalAudio
	,"rum" //Romanian,	eLangID_Romanian
	,"rus" //Russian,	eLangID_Russian
	,"scr" //Croatian,	eLangID_Croatian
	,"slo" //Slovak,	eLangID_Slovak
	,"slv" //Slovenian,	eLangID_Slovenian
	,"spa" //Spanish,	eLangID_Spanish
	,"srp" //Serbian,	eLangID_Serbian
	,"swe" //Swedish,	eLangID_Swedish
	,"tha" //Thai,	eLangID_Thai
	,"tur" //Turkish,	eLangID_Turkish
	,"ukr" //Ukrainian,	eLangID_Ukrainian
	,"und" //Undefine,	eLangID_Undefined
	,"vie" //Vietnamese,	eLangID_Vietnamese
	,"wel" //Welsh,	eLangID_Welsh
};

static const HCHAR *	s_jpadditional639Map[][2] =
{
	 {"ces", "cze"} //Czech, eLangID_Czech
	,{"cym", "wel"} //Welsh, eLangID_Welsh
	,{"deu", "ger"} //German, eLangID_German
	,{"ell", "gre"} //Greek, eLangID_Greek
	,{"esl", "spa"} //Spanish, eLangID_Spanish
	,{"esp", "spa"} //Spanish, eLangID_Spanish
	,{"fas", "per"} //Persian, eLangID_Persian
	,{"fra", "fre"} //French, eLangID_French
	,{"gai", "gle"} //Irish, eLangID_Irish
	,{"hrv", "scr"} //Croatian, eLangID_Croatian
	,{"iri", "gle"} //Irish, eLangID_Irish
	,{"isl", "ice"} //Icelandic, eLangID_Icelandic
	,{"nla", "dut"} //Dutch, eLangID_Dutch
	,{"nld", "dut"} //Dutch, eLangID_Dutch
	,{"ron", "rum"} //Romanian, eLangID_Romanian
	,{"scc", "srp"} //Serbian, eLangID_Serbian
	,{"sga", "gle"} //Irish, eLangID_Irish
	,{"slk", "slo"} //Slovak, eLangID_Slovak
	,{"sqi", "alb"} //Albanian, eLangID_Albanian
	,{"sve", "swe"} //Swedish, eLangID_Swedish
};


static int	local_opl_programme_japan_CompareMap (const void *arg1, const void *arg2)
{
	const HCHAR *lhs = *(const HCHAR **)arg1;
	const HCHAR *rhs = *(const HCHAR **)arg2;

	return HLIB_STD_StrCaseCmp(lhs, rhs);
}

static const HCHAR *	local_opl_programme_japan_FindCode (const HCHAR *code639)
{
	const HCHAR **ret;

	ret = (const HCHAR **)bsearch(&code639,
							s_jp639map,
							sizeof(s_jp639map) / sizeof(s_jp639map[0]),
							sizeof(HCHAR *),
							local_opl_programme_japan_CompareMap
						);
	if (ret)
	{
		return *ret;
	}

	ret = (const HCHAR **)bsearch(&code639,
							s_jpadditional639Map,
							sizeof(s_jpadditional639Map) / sizeof(s_jpadditional639Map[0]),
							sizeof(HCHAR *) * 2,
							local_opl_programme_japan_CompareMap
						);
	if (ret)
	{
		return *(ret + 1);
	}
	return NULL;
}

static const HCHAR *	local_opl_programme_japan_ToCode (HCHAR *buf, HUINT32 id)
{
	HCHAR *code = buf;

	*code++ = (HCHAR)((id >>  16) & 0xFF);
	*code++ = (HCHAR)((id >>  8) & 0xFF);
	*code++ = (HCHAR)((id >> 0) & 0xFF);
	*code++ = '\0';

	return buf;
}

const HCHAR *	OPL_PROGRAMME_JAPAN_IntTo639 (HUINT32 value)
{
	HCHAR	code[4];
	const HCHAR *_639;

	HxLOG_Trace();

	(void)local_opl_programme_japan_ToCode(code, (HUINT32)value);

	_639 = local_opl_programme_japan_FindCode(code);
	if (_639 == NULL)
	{
		HxLOG_Warning("Cannot find 639 code:%s(0x%X:%d)\n", code, value, value);
		return 0;
	}
	return _639;
}


