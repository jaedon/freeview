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
 * @file	  		nx_core_search.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_SHAHID_H__
#define __NX_SHAHID_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <hapi.h>
#include <papi.h>
#include <oapi.h>
#include <apk.h>

#include <nx_common.h>
#include <nx_port.h>
#include <nx_core_message.h>

/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/

typedef enum {
    eSHAHID_REQ_START = 0,
    eSHAHID_REQ_HOME_0, //Slider
    eSHAHID_REQ_HOME_1, //Ads
    eSHAHID_REQ_HOME_2, //Series Episodes
    eSHAHID_REQ_HOME_3, //Show Episodes
    eSHAHID_REQ_HOME_4, //Movies
    eSHAHID_REQ_HOME_5, //Ramadan Series
    eSHAHID_REQ_HOME_6, //Shows
    eSHAHID_REQ_HOME_7, //Movement Movies
    eSHAHID_REQ_HOME_8, //Series
    eSHAHID_REQ_HOME_9, //Series Clips
    eSHAHID_REQ_HOME_10, //Show Clips
    eSHAHID_REQ_HOME_MAX,
    eSHAHID_REQ_MAX
} ShahidReqType;

typedef enum{
    SHAHID_DIAL_STATE_STOPPED = 0,
    SHAHID_DIAL_STATE_RUNNING
} ShahidDialState;



#define SHAHID_STRING_SHORT_LENGTH      128
#define SHAHID_STRING_LONG_LENGTH       512
#define SHAHID_DIAL_NAME       "com.humaxdigital.shahid"

typedef struct ShowInfo{
    HINT32  id;
}ShowInfo;

typedef struct ShowListInfo{
    HINT32  pageSize;
    HINT32  pageNumber;
}ShowListInfo;

typedef struct ShahidCastData{
    HCHAR   url[SHAHID_STRING_SHORT_LENGTH];
    HUINT32 durationSeconds;
    HCHAR   mainTitle[SHAHID_STRING_LONG_LENGTH];
}ShahidCastData;

typedef struct ShahidShow{
    HINT32  id;
    HCHAR   name[SHAHID_STRING_SHORT_LENGTH];
    HCHAR   description[SHAHID_STRING_LONG_LENGTH];
    HCHAR   shortDescription[SHAHID_STRING_LONG_LENGTH];
    HCHAR   thumbnailUrl[SHAHID_STRING_SHORT_LENGTH];
}ShahidShow;

typedef struct ShahidShowList{
    ShahidShow    *stArrayOfShahidShow;
    HINT32  length;
}ShahidShowList;

typedef struct VASTTrackingEvents{
	HUINT8	*evtCreativeView;
	HUINT8	*evtStart;
	HUINT8	*evtMidpoint;
	HUINT8	*evtFirstQuartile;
	HUINT8	*evtThirdQuartile;
	HUINT8	*evtComplete;
}VASTTrackingEvents;

typedef struct VASTData{
	HUINT8	*adId;
	HUINT8	*errorData;
	HUINT8	*impression;
	HUINT8	*duration;
	HUINT8	*mediaFile;
	HUINT8	*adTagURI;
	VASTTrackingEvents		stTrackingEvents;
}VASTData;

extern HERROR       	    NX_SHAHID_Init(void);
extern HERROR               NX_SHAHID_ReqShowList(ShowListInfo  *stShowListInfo);
extern ShahidShowList*      NX_SHAHID_GetShowList(ShowListInfo  *stShowListInfo);
#endif // __NX_SHAHID_H__
