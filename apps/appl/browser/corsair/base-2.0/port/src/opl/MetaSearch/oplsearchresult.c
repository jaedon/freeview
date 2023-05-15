/**************************************************************************************/
/**
 * @file oplsearchresult.c
 *
 * Defines search result routine
 *
 * @author  ultracat(leecw@humaxdigital.com)
 * @date    2011/11/17
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include <hlib.h>
#include <oplchannel.h>
#include <oplsearchresult.h>

// TODO: This is temporary!!!!!. We have to implement, right way....
BPLStatus	DUMMY_OPL_SEARCHRESULT_GetProgramme(void *ch, void *prog)
{
#if 0
	OPL_CHANNEL_t	*channel = (OPL_CHANNEL_t *)ch;
	OplProgramme_t	*programme = (OplProgramme_t *)prog;
	HxSTD_PrintToStrN(programme->szName,     OPLPROGRAMME_MAX_NAME_LEN, "Programme:%s", channel->name);
	HxSTD_PrintToStrN(programme->szLongName, OPLPROGRAMME_MAX_LONGNAME_LEN, "Programme:LongName");
	HxSTD_PrintToStrN(programme->szDescription,     OPLPROGRAMME_MAX_DESC_LEN, "Programme:Description");
	HxSTD_PrintToStrN(programme->szLongDescription, OPLPROGRAMME_MAX_LONGDESC_LEN, "Programme:Long Description");
	programme->ulStartTimeSec = (unsigned int)HLIB_STD_GetSystemTime();
	programme->ulDurationSec = 60 * (HLIB_MATH_Random() % 60);
	HLIB_STD_StrNCpySafe(programme->szChannelID, channel->ccid, OPLCHANNEL_MAX_ID_LEN);
	programme->ulEpisode = 0;
	programme->ulTotalEpisode = 0;
	HxSTD_PrintToStrN(programme->szID, OPLPROGRAMME_MAX_ID_LEN, "ID:%d", (int)(HLIB_MATH_Random() % 1000));
	programme->eIDType = OPLPROGRAMME_IDTYPE_DVB_EVENT;
	programme->parentalRatings = NULL;
	programme->bBlocked = FALSE;
	programme->eShowType = OPLPROGRAMME_SHOWTYPE_LIVE;
	programme->bSubtitles = FALSE;
	programme->bHD = FALSE;
	programme->eAudioType = OPLPROGRAMME_AUDIOTYPE_STEREO;
	programme->bMultiLingual = FALSE;
	programme->szGenres[0] = '\0';
	programme->bRecording = FALSE;
	programme->szAudioLanguages[0] = '\0';
	programme->szSubtitleLanguages[0] = '\0';
	programme->bLocked = FALSE;
#endif
	return BPL_STATUS_OK;
}


