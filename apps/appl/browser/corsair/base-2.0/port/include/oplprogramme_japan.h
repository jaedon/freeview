/**************************************************************************************/
/**
 * @file oplprograme_japan.h
 *
 * Programme_ngs
 *
 * @author  Seung bum kim (sbkim@humaxdigital.com)
 * @date    2012/06/12
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLPROGRAMME_JAPAN_H__
#define __OPLPROGRAMME_JAPAN_H__

#include "oplcollection.h"
#include <hlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
0x00 Reserved for future use
0x01 Video 480i(525i), 4:3 aspect ratio
0x02 Video 480i(525i), 16:9 aspect ratio, with pan vectors
0x03 Video 480i(525i), 16:9 aspect ratio, without pan vectors
0x04 Video 480i(525i), >16:9 aspect ratio
0x05 . 0xA0 Reserved for future use
0xA1 Video 480p(525p), 4:3 aspect ratio
0xA2 Video 480p(525p), 16:9 aspect ratio, with pan vectors
0xA3 Video 480p(525p), 16:9 aspect ratio, without pan vectors
0xA4 Video 480p(525p), >16:9 aspect ratio
0xA5 . 0xB0 Reserved for future use
0xB1 Video 1080i(1125i), 4:3 aspect ratio
0xB2 Video 1080i(1125i), 16:9 aspect ratio, with pan vectors
0xB3 Video 1080i(1125i), 16:9 aspect ratio, without pan vectors
0xB4 Video 1080i(1125i), >16:9 aspect ratio
0xB5 . 0xC0 Reserved for future use
0xC1 Video 720p(750p), 4:3 aspect ratio
0xC2 Video 720p(750p), 16:9 aspect ratio, with pan vectors
0xC3 Video 720p(750p), 16:9 aspect ratio, without pan vectors
0xC4 Video 720p(750p), >16:9 aspect ratio
0xC5 . 0xD0 Reserved for future use
0xD1 Video 240p, 4:3 aspect ratio
0xD2 Video 240p, 4:3 aspect ratio, with pan vectors
0xD3 Video 240p, 4:3 aspect ratio, without pan vector
0xD4 Video 240p, 4:3 aspect ratio > 16:9
0xD5 . 0xFF Reserved for future use
*/

typedef enum tagProgrammeVideoResoultion
{
	/*
		PV   : pan vectors
		WOPV : without pan vectors
		WPV  : with pan vectors
		OVER : 16_9 over
	*/
	eProgrammeVideoRes_525i_4_3 = 0x01,
	eProgrammeVideoRes_525i_16_9_WPV = 0x02,
	eProgrammeVideoRes_525i_16_9_WOPV = 0x03,
	eProgrammeVideoRes_525i_16_9_OVER = 0x04,

	eProgrammeVideoRes_525p_4_3 = 0xA1,
	eProgrammeVideoRes_525p_16_9_WPV = 0xA2,
	eProgrammeVideoRes_525p_16_9_WOPV = 0xA3,
	eProgrammeVideoRes_525p_16_9_OVER = 0xA4,

	eProgrammeVideoRes_1125i_4_3 = 0xB1,
	eProgrammeVideoRes_1125i_16_9_WPV = 0xB2,
	eProgrammeVideoRes_1125i_16_9_WOPV = 0xB3,
	eProgrammeVideoRes_1125i_16_9_OVER = 0xB4,

	eProgrammeVideoRes_750p_4_3 = 0xC1,
	eProgrammeVideoRes_750p_16_9_WPV = 0xC2,
	eProgrammeVideoRes_750p_16_9_WOPV = 0xC3,
	eProgrammeVideoRes_750p_16_9_OVER = 0xC4,

	eProgrammeVideoRes_240p_4_3 = 0xD1,
	eProgrammeVideoRes_240p_4_3_WPV = 0xD2,
	eProgrammeVideoRes_240p_4_3_WOPV = 0xD3,
	eProgrammeVideoRes_240p_4_3_OVER = 0xD4,

	eProgrammeVideoRes_1125p_4_3 = 0xE1,
	eProgrammeVideoRes_1125p_16_9_WPV = 0xE2,
	eProgrammeVideoRes_1125p_16_9_WOPV = 0xE3,
	eProgrammeVideoRes_1125p_16_9_OVER = 0xE4,

	eProgrammeVideoRes_Max			= 0xFF
} AribVideoResoultion_t;

typedef enum
{
	ARIB_COPY_FREE 	= 0x00, /* MUST be 00b */
	ARIB_COPY_NOMORE 	= 0x01, /* MUST be 01b */
	ARIB_COPY_ONCE	= 0x02, /* MUST be 10b */
	ARIB_COPY_NEVER	= 0x03, /* MUST be 11b */
	ARIB_COPY_10,
	ARIB_OUTPUT_NEVER,
	ARIB_COPY_FREE_WITH_EPN
}eAribCpOutput_t;

typedef struct
{
	eAribCpOutput_t eAnalogOutput;
	eAribCpOutput_t eAnalogOutputInCgms;
	eAribCpOutput_t eDigitalOutput;
	eAribCpOutput_t eMpegTsOutput;
	eAribCpOutput_t eMpegTsIpOutput;
}AribCpOutputInfo_t;

typedef struct
{
	HUINT16	usSvcType;
	HUINT8	ucCopyControlType;		/* 2bits */
	HUINT8	ucDigitalRecording;		/* 2bits */
	HUINT8	ucAnalogRecording;		/* 2bits */
	HUINT8	ucCopyRestrictionMode;	/* For Dubbing-10 */
	HUINT8	ucImageContraintToken;
	HUINT8	ucEncryptionMode;
	HUINT8	ucRetentionMode;
	HUINT8	ucRetentionState;
}AribCpInfo_t;

#define ____EIT_PRIVATE____
HCHAR *	OPL_PROGRAMME_JAPAN_GetGenre (HUINT16 content);
const HCHAR *OPL_PROGRAMME_JAPAN_GetGenreValue (HCHAR *genre);
HERROR OPL_PROGRAMME_JAPAN_GetSvcType (HUINT32 ulSvcType, HINT32 *svcType);
HERROR	OPL_PROGRAMME_JAPAN_InterpretCopyProtectInfo(AribCpInfo_t stAribCpInfo, HBOOL bPlayMode, AribCpOutputInfo_t *pCpOutput);
HFLOAT32 OPL_PROGRAMME_JAPAN_GetVideoAspectRatio(HUINT8 ucVideoCompType);
const HCHAR *	OPL_PROGRAMME_JAPAN_IntTo639 (HUINT32 value);
#ifdef __cplusplus
};
#endif

#endif

