/**************************************************************
 *	@file		ipepg_downloader.c
 *	IP EPG downloader for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/08/01
 *	@author			humax
 *	@brief			IP EPG Downloader for OCTO 2.0 Project
 **************************************************************/
#include <unistd.h>
#include <signal.h>
#include <dlib.h>
#include <vkernel.h>
#include <hlib.h>


#include "../include/ipepg.h"
#include "ipepg_downloader.h"

//////////////////////////////////////////////////////////////////////////////////// XML

#define IPEPG_FVC_OTHER_IDENTIFIER_PCRID	"eit-programme-crid"
#define IPEPG_FVC_OTHER_IDENTIFIER_SCRID	"eit-series-crid"
#define IPEPG_FVC_OTHER_IDENTIFIER_RCRID	"eit-recommendation-crid"

#define	IPEPG_FVC_GENRE_PREFIX							"urn:fvc:metadata:cs:"
#define	IPEPG_FVC_GENRE_CONTENT_SUBJECT					"ContentSubjectCS:2014-07:"
#define IPEPG_FVC_GENRE_MEDIA_AVAILABILITY				"MediaAvailabilityCS:2014-07:"
#define IPEPG_FVC_GENRE_MEDIA_AVAILABILITY_AVAILABLE	"media_available"
#define IPEPG_FVC_GENRE_MEDIA_AVAILABILITY_UNAVAILABLE	"media_unavailable"
#define	IPEPG_FVC_GENRE_FEPG_AVAILABILITY				"FEPGAvailabilityCS:2014-10:"
#define IPEPG_FVC_GENRE_FEPG_AVAILABILITY_AVAILABLE		"fepg_available"
#define IPEPG_FVC_GENRE_FEPG_AVAILABILITY_UNAVAILABLE	"fepg_unavailable"

#define	IPEPG_FVC_AVATTRIBUTES_VIDEO_HORIZONTAL_HD		1920

#define	MACRO_IPEPG_STRLEN(s)				(sizeof(s)/sizeof(s[0])-1)

typedef	enum
{
	FVC_ELEMENT_DEPTH_ROOT 					= 0,
	FVC_ELEMENT_DEPTH_1,
	FVC_ELEMENT_DEPTH_2,
	FVC_ELEMENT_DEPTH_3,
	FVC_ELEMENT_DEPTH_4,
	FVC_ELEMENT_DEPTH_5,
	FVC_ELEMENT_DEPTH_6,
	FVC_ELEMENT_DEPTH_7,
	FVC_ELEMENT_DEPTH_8,
	FVC_ELEMENT_DEPTH_9,
	FVC_ELEMENT_DEPTH_10
}FVC_ELEMENT_DEPTH_e;


typedef	enum
{
	FVC_TYPE_NONE 							= 0,
	FVC_TYPE_STRING,
	FVC_TYPE_DECIMAL,
	FVC_TYPE_DECIMAL64,
	FVC_TYPE_HEXA

}FVC_ELEMENTDATA_TYPE_e;




typedef enum
{
	FVC_STATION_TVAMAIN = 0,
	FVC_STATION_PROGRAMMEDESCRIPTION,
	FVC_STATION_SIT,
	FVC_STATION_SERVICEINFORMATION,
	FVC_SVC_NAME ,
	FVC_SVC_SUPPORTMEDIA ,
	FVC_SVC_ONID,
	FVC_SVC_TSID,
	FVC_SVC_SVCID,
	FVC_SVC_SVCHANDLE,
	FVC_SVC_RELATED_MATERIAL,
	FVC_SVC_MEDIA_LOCATOR,
	FVC_SVC_URL,
	FVC_SVC_MEDIA_URL,
	FVC_STATIONID,
	FVC_LANG,
	FVC_VERSION,
	FVC_SVC_TRIPLEID,
	FVC_SVC_TRIPLEIDS,
	FVC_SVC_MEDIA_URLS,
	FVC_SVC_THUMBNAIL_SIZE,
	FVC_SVC_THUMBNAIL_TYPE,
	FVC_SVC_MAX
}FVC_TAG_e;

typedef enum
{
	FVC_ELEM_EVENT_TVAMAIN = 0,
	FVC_ELEM_EVENT_PROGRAMDESCRIPTION,
	FVC_ELEM_EVENT_PIT,					// Program Information Table
	FVC_ELEM_EVENT_PLT,					// Program Location	 Table
	FVC_ELEM_EVENT_PROGRAM_INFORMATION,
	FVC_ELEM_EVENT_SCHEDULE,


	FVC_ATTR_EVENT_PI_PROGRAMID,		// PI: Program Information
	FVC_ATTR_EVENT_PI_GENRE_HREF,
	FVC_ELEM_EVENT_PI_BASICDESCRIPTION,
	FVC_ELEM_EVENT_PI_TITLE,
	FVC_ELEM_EVENT_PI_SYNOPSIS,
	FVC_ELEM_EVENT_PI_GENRE,
	FVC_ELEM_EVENT_PI_GUIDANCE,
	FVC_ELEM_EVENT_PI_GUIDANCE_EXPLANATORY_TEXT,
	FVC_ELEM_EVENT_PI_RELATED_MATERIAL,
	FVC_ELEM_EVENT_PI_MEDIA_LOCATOR,
	FVC_ELEM_EVENT_PI_MEDIA_URL,
	FVC_ELEM_EVENT_PI_AV_ATTRIBUTES,

	FVC_ELEM_EVENT_PL_SCHEDULE_EVENT,
	FVC_ELEM_EVENT_PL_PROGRAM,
	FVC_ATTR_EVENT_PL_PROGRAMID,
	FVC_ELEM_EVENT_PL_PROGRAM_URL,
	FVC_ELEM_EVENT_PL_INSTANCE_DESCRIPTION,
	FVC_ATTR_EVENT_PL_ID_OTHER_IDENTIFIER_TYPE,
	FVC_ATTR_EVENT_PL_ID_GENRE_TYPE,
	FVC_ATTR_EVENT_PL_ID_GENRE_HREF,
	FVC_ELEM_EVENT_PL_ID_OTHER_IDENTIFIER,
	FVC_ELEM_EVENT_PL_ID_GENRE,
	FVC_ELEM_EVENT_PL_ID_AVATTRIBUTES,
	FVC_ELEM_EVENT_PL_ID_AVATTRIBUTES_VIDEO,
	FVC_ELEM_EVENT_PL_ID_AVATTRIBUTES_VIDEO_HORIZONTAL,
	FVC_ELEM_EVENT_PL_ID_AVATTRIBUTES_VIDEO_VERTICAL,
	FVC_ELEM_EVENT_PL_ID_AVATTRIBUTES_AUDIO,
	FVC_ELEM_EVENT_PL_ONID,
	FVC_ELEM_EVENT_PL_SVCID,
	FVC_ELEM_EVENT_PL_START_TIME,
	FVC_ELEM_EVENT_PL_DURATION,

	FVC_ELEM_EVENT_ONDEMAND,
	FVC_ATTR_EVENT_PL_OD_PROGRAM_URL_TYPE,
	FVC_ATTR_EVENT_PL_OD_AUXILIARY_URL_TYPE,
	FVC_ATTR_EVENT_PL_OD_FREE_VALUE,
	FVC_ELEM_EVENT_PL_OD_PROGRAM,
	FVC_ELEM_EVENT_PL_OD_PROGRAMID,
	FVC_ELEM_EVENT_PL_OD_PROGRAM_URL,
	FVC_ELEM_EVENT_PL_OD_AUXILIARY_URL,
	FVC_ELEM_EVENT_PL_OD_INSTANCE_DESCRIPTION,
	FVC_ATTR_EVENT_PL_OD_ID_GENRE_HREF,
	FVC_ELEM_EVENT_PL_OD_ID_GENRE,
	FVC_ELEM_EVENT_PL_OD_ID_AVATTRIBUTES,
	FVC_ELEM_EVENT_PL_OD_ID_AVATTRIBUTES_VIDEO,
	FVC_ELEM_EVENT_PL_OD_ID_AVATTRIBUTES_VIDEO_HORIZONTAL,
	FVC_ELEM_EVENT_PL_OD_ID_AVATTRIBUTES_VIDEO_VERTICAL,
	FVC_ELEM_EVENT_PL_OD_DURATION,
	FVC_ELEM_EVENT_PL_OD_STARTOFAVAILABILITY,
	FVC_ELEM_EVENT_PL_OD_ENDOFAVAILABILITY,
	FVC_ELEM_EVENT_PL_OD_FREE,
	FVC_ELEM_EVENT_PL_OD_DELIVERYMODE,

	FVC_ATTR_CHANNEL_HSVC,
	FVC_ATTR_CHANNEL_ID,
	FVC_ELEM_CHANNEL_NUM,
	FVC_ELEM_CHANNEL_NAME,
	FVC_ELEM_CHANNEL_TRIPLEIDS,
	FVC_ELEM_CHANNEL_TRIPLEID,
	FVC_ATTR_CHANNEL_TRIPLE_ONID,
	FVC_ATTR_CHANNEL_TRIPLE_TSID,
	FVC_ATTR_CHANNEL_TRIPLE_SVCID,

	FVC_ELEM_CHANNEL_IMAGES,
	FVC_ELEM_CHANNEL_IMAGE,
	FVC_ATTR_CHANNEL_IMAGESIZE,
	FVC_ATTR_CHANNEL_IMAGETYPE,

	FVC_ELEM_CHANNEL_UPDATE,
	FVC_ATTR_CHANNEL_UPDATETYPE,

	FVC_ELEM_PROGRAMS,
	FVC_ELEM_PROGRAM,
	FVC_ATTR_PROGRAM_ID,
	FVC_ATTR_PROGRAM_STARTTIME,
	FVC_ATTR_PROGRAM_ENDTIME,


	FVC_ELEM_PROGRAM_NAME,
	FVC_ATTR_PROGRAM_NAME,
	FVC_ELEM_PROGRAM_SUBNAME,
	FVC_ATTR_PROGRAM_SUBNAME,
	FVC_ELEM_PROGRAM_SHORTTEXT,
	FVC_ATTR_PROGRAM_SHORTTEXT,
	FVC_ELEM_PROGRAM_LONGTEXT,
	FVC_ATTR_PROGRAM_LONGTEXT,

	FVC_ELEM_PROGRAM_EVENTID,
	FVC_ELEM_PROGRAM_CRID,

	FVC_ELEM_PROGRAM_RECOMMENDATION,
	FVC_ELEM_PROGRAM_RATING,

	FVC_ELEM_PROGRAM_IMAGES,
	FVC_ELEM_PROGRAM_IMAGE,
	FVC_ATTR_PROGRAM_IMAGE_SIZE,
	FVC_ATTR_PROGRAM_IMAGE_TYPE,

	FVC_ELEM_PROGRAM_PERSONS,
	FVC_ELEM_PROGRAM_PERSON,
	FVC_ATTR_PROGRAM_PERSON_TYPE,

	FVC_ELEM_PROGRAM_GENRES,
	FVC_ELEM_PROGRAM_GENRE,
	FVC_ATTR_PROGRAM_GENRE_CODE1,
	FVC_ATTR_PROGRAM_GENRE_CODE2,


	FVC_ELEM_PROGRAM_CODECS,
	FVC_ELEM_PROGRAM_CODEC,
	FVC_ATTR_PROGRAM_CODEC_TYPE,

	FVC_ELEM_PROGRAM_SERIES,
	FVC_ATTR_PROGRAM_SERIES_ID,
	FVC_ATTR_PROGRAM_SERIES_COUNT,
	FVC_ATTR_PROGRAM_SERIES_EPISODE,
	FVC_ATTR_PROGRAM_SERIES_SEASON,
	FVC_ELEM_PROGRAM_SERIESLINKS,
	FVC_ELEM_PROGRAM_SERIESCRID,
	FVC_ELEM_PROGRAM_SERIESLINK,
	FVC_ATTR_PROGRAM_SERIESLINK_PROGID,
	FVC_ATTR_PROGRAM_SERIESLINK_STARTTIME,
	FVC_ATTR_PROGRAM_SERIESLINK_ENDTIME,
	FVC_ATTR_PROGRAM_SERIESLINK_EPISODE,
	FVC_ATTR_PROGRAM_SERIESLINK_SEASON,
	FVC_ELEM_PROGRAM_SERIESLINK_NAME,
	FVC_ATTR_PROGRAM_SERIESLINK_NAME,

	FVC_ELEM_PROGRAM_MEDIAS,
	FVC_ELEM_PROGRAM_MEDIA,
	FVC_ATTR_PROGRAM_MEDIA_ID,
	FVC_ATTR_PROGRAM_MEDIA_TYPE,
	FVC_ATTR_PROGRAM_MEDIA_TARGET,
	FVC_ELEM_PROGRAM_MEDIA_URLS,
	FVC_ELEM_PROGRAM_MEDIA_URL,

	FVC_ELEM_PROGRAM_MEDIA_IMAGES,
	FVC_ELEM_PROGRAM_MEDIA_IMAGE,
	FVC_ATTR_PROGRAM_MEDIA_IMAGE_SIZE,
	FVC_ATTR_PROGRAM_MEDIA_IMAGE_TYPE,

	FVC_ELEM_PROGRAM_MEDIA_PUBDATE,
	FVC_ELEM_PROGRAM_MEDIA_EXPIREDATE,
	FVC_ELEM_PROGRAM_MEDIA_TRANSMITDATE,
	FVC_ELEM_PROGRAM_MEDIA_PRICE,

	FVC_ELEM_EPG_MAX
}FVC_ELEMENT_e;

HCHAR *get_fvc_element_str (FVC_ELEMENT_e eFvc)
{
	switch (eFvc)
	{
	case FVC_ELEM_EVENT_TVAMAIN : return DxNAMEOF(FVC_ELEM_EVENT_TVAMAIN);
	case FVC_ELEM_EVENT_PROGRAMDESCRIPTION: return DxNAMEOF(FVC_ELEM_EVENT_PROGRAMDESCRIPTION);
	case FVC_ELEM_EVENT_PIT: return DxNAMEOF(FVC_ELEM_EVENT_PIT);
	case FVC_ELEM_EVENT_PLT:	 return DxNAMEOF(FVC_ELEM_EVENT_PLT);
	case FVC_ELEM_EVENT_PROGRAM_INFORMATION: return DxNAMEOF(FVC_ELEM_EVENT_PROGRAM_INFORMATION);
	case FVC_ELEM_EVENT_SCHEDULE: return DxNAMEOF(FVC_ELEM_EVENT_SCHEDULE);
	case FVC_ATTR_EVENT_PI_PROGRAMID:		 return DxNAMEOF(FVC_ATTR_EVENT_PI_PROGRAMID);
	case FVC_ELEM_EVENT_PI_BASICDESCRIPTION: return DxNAMEOF(FVC_ELEM_EVENT_PI_BASICDESCRIPTION);
	case FVC_ELEM_EVENT_PI_TITLE: return DxNAMEOF(FVC_ELEM_EVENT_PI_TITLE);
	case FVC_ELEM_EVENT_PI_SYNOPSIS: return DxNAMEOF(FVC_ELEM_EVENT_PI_SYNOPSIS);
	case FVC_ELEM_EVENT_PI_GENRE: return DxNAMEOF(FVC_ELEM_EVENT_PI_GENRE);
	case FVC_ELEM_EVENT_PI_GUIDANCE: return DxNAMEOF(FVC_ELEM_EVENT_PI_GUIDANCE);
	case FVC_ELEM_EVENT_PI_RELATED_MATERIAL: return DxNAMEOF(FVC_ELEM_EVENT_PI_RELATED_MATERIAL);
	case FVC_ELEM_EVENT_PI_MEDIA_LOCATOR: return DxNAMEOF(FVC_ELEM_EVENT_PI_MEDIA_LOCATOR);
	case FVC_ELEM_EVENT_PI_MEDIA_URL: return DxNAMEOF(FVC_ELEM_EVENT_PI_MEDIA_URL);
	case FVC_ELEM_EVENT_PI_AV_ATTRIBUTES: return DxNAMEOF(FVC_ELEM_EVENT_PI_AV_ATTRIBUTES);
	case FVC_ELEM_EVENT_PL_SCHEDULE_EVENT: return DxNAMEOF(FVC_ELEM_EVENT_PL_SCHEDULE_EVENT);
	case FVC_ELEM_EVENT_PL_PROGRAM: return DxNAMEOF(FVC_ELEM_EVENT_PL_PROGRAM);
	case FVC_ATTR_EVENT_PL_PROGRAMID: return DxNAMEOF(FVC_ATTR_EVENT_PL_PROGRAMID);
	case FVC_ELEM_EVENT_PL_PROGRAM_URL: return DxNAMEOF(FVC_ELEM_EVENT_PL_PROGRAM_URL);
	case FVC_ELEM_EVENT_PL_ONID: return DxNAMEOF(FVC_ELEM_EVENT_PL_ONID);
	case FVC_ELEM_EVENT_PL_SVCID: return DxNAMEOF(FVC_ELEM_EVENT_PL_SVCID);
	case FVC_ELEM_EVENT_PL_START_TIME: return DxNAMEOF(FVC_ELEM_EVENT_PL_START_TIME);
	case FVC_ELEM_EVENT_PL_DURATION: return DxNAMEOF(FVC_ELEM_EVENT_PL_DURATION);
	case FVC_ELEM_EVENT_PL_OD_PROGRAM_URL: return DxNAMEOF(FVC_ELEM_EVENT_PL_OD_PROGRAM_URL);
	case FVC_ELEM_EVENT_PL_OD_AUXILIARY_URL: return DxNAMEOF(FVC_ELEM_EVENT_PL_OD_AUXILIARY_URL);
	case FVC_ELEM_EVENT_PL_OD_DURATION: return DxNAMEOF(FVC_ELEM_EVENT_PL_OD_DURATION);
	case FVC_ELEM_EVENT_PL_OD_STARTOFAVAILABILITY: return DxNAMEOF(FVC_ELEM_EVENT_PL_OD_STARTOFAVAILABILITY);
	case FVC_ELEM_EVENT_PL_OD_ENDOFAVAILABILITY: return DxNAMEOF(FVC_ELEM_EVENT_PL_OD_ENDOFAVAILABILITY);
	case FVC_ELEM_EVENT_PL_OD_FREE: return DxNAMEOF(FVC_ELEM_EVENT_PL_OD_FREE);
	case FVC_ELEM_EVENT_PL_OD_DELIVERYMODE: return DxNAMEOF(FVC_ELEM_EVENT_PL_OD_DELIVERYMODE);

	default: break;
	}

	return "UNKNOWN";
}

typedef struct
{
	FVC_TAG_e		eDataIdx;
	HBOOL					bFlagEnd;
	HUINT32					ulAttrDataType;
	HUINT8					*pucAttrName;
} FVC_ATTR_t;


typedef struct IpepgHumaxElement
{
//	HBOOL					bFlagAttr;
	HUINT32					eDataIdx;

	HBOOL					bFlagEnd;
	HUINT32					ulDepth;

	HUINT32					ulTagDataType;
	HUINT8					*pucTagName;

	struct IpepgHumaxElement	*ElementList;

	FVC_ATTR_t	*AttributeList;

} FVC_ELEMENT_t;


typedef struct
{
	HUINT8		ucOrgStrLen;
	HUINT8		ucFlagReplace;
	HUINT8		szOrgStr[8];
	HUINT8		ucReplaceCode;
} FVC_REPLACESTR_t;

typedef struct
{
	HUINT8			szProgCrid[DxCRID_LEN*2+10];
	HxXMLELEMENT	*phElement;
} FVC_PROGRAM_INFORMATION_t;

typedef struct
{
	HCHAR			szProgCrid[DxCRID_LEN*2+10];
	DxIpEvent_t		*pstIpEvent;
} FVC_PROGRAM_LOCATION_t;


////////////////////////////////////////////////////////////////////////////////////////
#define __________SVC_URL_INFO________________


const static FVC_ELEMENT_t		s_astElementMediaLocator[] =
{
	{FVC_SVC_MEDIA_URL,		TRUE,		FVC_ELEMENT_DEPTH_6,		FVC_TYPE_STRING,			(HUINT8 *)"MediaUri",			NULL,	NULL	}
};

const static FVC_ELEMENT_t		s_astElementRelatedMaterial[] =
{
	{FVC_SVC_MEDIA_LOCATOR,		TRUE,		FVC_ELEMENT_DEPTH_5,		FVC_TYPE_NONE,			(HUINT8 *)"MediaLocator",			(FVC_ELEMENT_t *)s_astElementMediaLocator,	NULL	}
};


const static FVC_ELEMENT_t		s_astElementServiceInfo[] =
{
	{FVC_SVC_NAME,			FALSE,		FVC_ELEMENT_DEPTH_4,		FVC_TYPE_STRING,		(HUINT8 *)"Name",					NULL,	NULL	},
	{FVC_SVC_URL, 			FALSE,		FVC_ELEMENT_DEPTH_4,		FVC_TYPE_STRING,		(HUINT8 *)"ServiceURL",				NULL,	NULL	},
	{FVC_SVC_RELATED_MATERIAL,	TRUE,		FVC_ELEMENT_DEPTH_4,		FVC_TYPE_NONE,		(HUINT8 *)"RelatedMaterial", 		(FVC_ELEMENT_t *)s_astElementRelatedMaterial,		NULL	}
};

const static FVC_ELEMENT_t		s_astElementService[] =
{
	{FVC_STATION_SERVICEINFORMATION,	TRUE,		FVC_ELEMENT_DEPTH_3,		FVC_TYPE_NONE,		(HUINT8 *)"ServiceInformation", 	(FVC_ELEMENT_t *)s_astElementServiceInfo,	NULL	}
};

const static FVC_ELEMENT_t		s_astElementSIT[] =
{
	{FVC_STATION_SIT, 	TRUE,		FVC_ELEMENT_DEPTH_2,		FVC_TYPE_NONE, 		(HUINT8 *)"ServiceInformationTable",	(FVC_ELEMENT_t *)s_astElementService,	NULL	}
};

const static FVC_ELEMENT_t		s_astElementProgrammeDescription[] =
{
	{FVC_STATION_PROGRAMMEDESCRIPTION, 	TRUE,		FVC_ELEMENT_DEPTH_1,		FVC_TYPE_NONE, 		(HUINT8 *)"ProgramDescription",	(FVC_ELEMENT_t *)s_astElementSIT,	NULL	}
};

const static FVC_ELEMENT_t		s_astElementTvaMain[] =
{
	{FVC_STATION_TVAMAIN,	TRUE,		FVC_ELEMENT_DEPTH_ROOT,		FVC_TYPE_NONE,		(HUINT8 *)"TVAMain",		(FVC_ELEMENT_t *)s_astElementProgrammeDescription,	NULL	}
};

#define __________EVENT_INFO________________
/*
const static FVC_ATTR_t	s_astAttrIpEpgChannel[] =
{
	{FVC_ATTR_CHANNEL_HSVC,			FALSE,	FVC_TYPE_DECIMAL, (HUINT8 *)"HSVC"},
	{FVC_ATTR_CHANNEL_ID,				TRUE,	FVC_TYPE_DECIMAL, 	(HUINT8 *)"CHANNELID"}
};

const static FVC_ATTR_t	s_astAttrIpEpgChannelTripleId[] =
{
	{FVC_ATTR_CHANNEL_TRIPLE_ONID,	FALSE,	FVC_TYPE_DECIMAL, 	(HUINT8 *)"ONID"},
	{FVC_ATTR_CHANNEL_TRIPLE_TSID,	FALSE,	FVC_TYPE_DECIMAL, 	(HUINT8 *)"TSID"},
	{FVC_ATTR_CHANNEL_TRIPLE_SVCID,	TRUE,	FVC_TYPE_DECIMAL, 	(HUINT8 *)"SID"}
};

const static FVC_ATTR_t	s_astAttrIpEpgChannelImage[] =
{
	{FVC_ATTR_CHANNEL_IMAGESIZE,		FALSE,	FVC_TYPE_DECIMAL, 	(HUINT8 *)"URLSIZE"},
	{FVC_ATTR_CHANNEL_IMAGETYPE,		TRUE,	FVC_TYPE_STRING, 		(HUINT8 *)"URLTYPE"}
};

const static FVC_ATTR_t	s_astAttrIpEpgProgramUpdate[] =
{
	{FVC_ATTR_PROGRAM_ENDTIME,		TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"TYPE"}
};


const static FVC_ATTR_t	s_astAttrIpEpgProgram[] =
{
	{FVC_ATTR_PROGRAM_ID,				FALSE,	FVC_TYPE_DECIMAL, (HUINT8 *)"PROGRAMID"},
	{FVC_ATTR_PROGRAM_STARTTIME,		FALSE,	FVC_TYPE_DECIMAL, (HUINT8 *)"STARTDATETIME"},
	{FVC_ATTR_PROGRAM_ENDTIME,		TRUE,	FVC_TYPE_DECIMAL, 	(HUINT8 *)"ENDDATETIME"}
};

// Ip Epg Element

const static FVC_ATTR_t	s_astAttrIpEpgProgName[] =
{
	{FVC_ATTR_PROGRAM_NAME,			TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"LANG"}
};
const static FVC_ATTR_t	s_astAttrIpEpgProgSubName[] =
{
	{FVC_ATTR_PROGRAM_SUBNAME,		TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"LANG"}
};
const static FVC_ATTR_t	s_astAttrIpEpgProgShortText[] =
{
	{FVC_ATTR_PROGRAM_SHORTTEXT,		TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"LANG"}
};
const static FVC_ATTR_t	s_astAttrIpEpgProgLongText[] =
{
	{FVC_ATTR_PROGRAM_LONGTEXT,		TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"LANG"}
};

const static FVC_ATTR_t	s_astAttrIpEpgProSeries[] =
{
	{FVC_ATTR_PROGRAM_SERIES_ID,		FALSE,	FVC_TYPE_DECIMAL, (HUINT8 *)"SERIESID"},
	{FVC_ATTR_PROGRAM_SERIES_COUNT,	FALSE,	FVC_TYPE_DECIMAL, (HUINT8 *)"SERIESCOUNT"},
	{FVC_ATTR_PROGRAM_SERIES_EPISODE,	FALSE,	FVC_TYPE_DECIMAL, (HUINT8 *)"SERIESNUMBER"},
	{FVC_ATTR_PROGRAM_SERIES_SEASON,	TRUE,	FVC_TYPE_DECIMAL, (HUINT8 *)"SEASONNUMBER"}
};

const static FVC_ATTR_t	s_astAttrIpEpgProgImage[] =
{
	{FVC_ATTR_PROGRAM_IMAGE_SIZE,		FALSE,	FVC_TYPE_DECIMAL, 	(HUINT8 *)"URLSIZE"},
	{FVC_ATTR_PROGRAM_IMAGE_TYPE,		TRUE,	FVC_TYPE_STRING, 		(HUINT8 *)"URLTYPE"}
};

const static FVC_ATTR_t	s_astAttrIpEpgProgPerson[] =
{
	{FVC_ATTR_PROGRAM_PERSON_TYPE,	TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"TYPE"}
};

const static FVC_ATTR_t	s_astAttrIpEpgProgGenre[] =
{
	{FVC_ATTR_PROGRAM_GENRE_CODE1,	FALSE,	FVC_TYPE_DECIMAL, 	(HUINT8 *)"CODE1"},
	{FVC_ATTR_PROGRAM_GENRE_CODE2,	TRUE,	FVC_TYPE_DECIMAL, 	(HUINT8 *)"CODE2"}
};

const static FVC_ATTR_t	s_astAttrIpEpgProgCodec[] =
{
	{FVC_ATTR_PROGRAM_GENRE_CODE2,	TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"TYPE"}
};

const static FVC_ATTR_t	s_astAttrIpEpgProSeriesLink[] =
{
	{FVC_ATTR_PROGRAM_SERIESLINK_PROGID,		FALSE,	FVC_TYPE_DECIMAL, (HUINT8 *)"PROGRAMID"},
	{FVC_ATTR_PROGRAM_SERIESLINK_STARTTIME,	FALSE,	FVC_TYPE_DECIMAL, (HUINT8 *)"STARTDATETIME"},
	{FVC_ATTR_PROGRAM_SERIESLINK_ENDTIME,		FALSE,	FVC_TYPE_DECIMAL, (HUINT8 *)"ENDDATETIME"},
	{FVC_ATTR_PROGRAM_SERIESLINK_EPISODE,		FALSE,	FVC_TYPE_DECIMAL, (HUINT8 *)"SERIESNUMBER"},
	{FVC_ATTR_PROGRAM_SERIESLINK_SEASON,		TRUE,	FVC_TYPE_DECIMAL, (HUINT8 *)"SEASONNUMBER"}
};

const static FVC_ATTR_t	s_astAttrIpEpgProgSeriesLinkName[] =
{
	{FVC_ATTR_PROGRAM_SERIESLINK_NAME,		TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"LANG"}
};

const static FVC_ELEMENT_t 	s_astElemIpEpgProgSeriesLinkName[] =
{
	{FVC_ELEM_PROGRAM_SERIESLINK_NAME,	TRUE,	FVC_ELEMENT_DEPTH_9,	FVC_TYPE_STRING,	(HUINT8 *)"TITLESUB", 		(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)s_astAttrIpEpgProgSeriesLinkName		}
};

const static FVC_ATTR_t	s_astAttrIpEpgProgMediaImage[] =
{
	{FVC_ATTR_PROGRAM_IMAGE_SIZE,			FALSE,	FVC_TYPE_DECIMAL, 	(HUINT8 *)"SIZE"	},
	{FVC_ATTR_PROGRAM_IMAGE_TYPE,			TRUE,	FVC_TYPE_STRING, 		(HUINT8 *)"TYPE"	}
};
const static FVC_ELEMENT_t	s_astElemIpEpgProgMediaImage[] =
{
	{FVC_ELEM_PROGRAM_MEDIA_IMAGE,		TRUE,	FVC_ELEMENT_DEPTH_9, 	FVC_TYPE_STRING, 	(HUINT8 *)"IMAGE", 		(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)s_astAttrIpEpgProgMediaImage	},
};

const static FVC_ATTR_t	s_astAttrIpEpgProgMediaURL[] =
{
	{FVC_ATTR_PROGRAM_MEDIA_TYPE,			FALSE,	FVC_TYPE_STRING, 	(HUINT8 *)"TYPE"	},
	{FVC_ATTR_PROGRAM_MEDIA_TARGET,		TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"TARGET"	}
};
const static FVC_ELEMENT_t	s_astElemIpEpgProgMediaURL[] =
{
	{FVC_ELEM_PROGRAM_MEDIA_URL,			TRUE,	FVC_ELEMENT_DEPTH_9, 	FVC_TYPE_STRING, 	(HUINT8 *)"MEDIAURL", 		(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)s_astAttrIpEpgProgMediaURL	},
};

const static FVC_ELEMENT_t 	s_astElemIpEpgProgMediaInfo[] =
{

	{FVC_ELEM_PROGRAM_MEDIA_URLS,			FALSE,	FVC_ELEMENT_DEPTH_8,	FVC_TYPE_NONE,	(HUINT8 *)"MEDIAURLS", 		(FVC_ELEMENT_t *)s_astElemIpEpgProgMediaURL,			(FVC_ATTR_t *)NULL		},
	{FVC_ELEM_PROGRAM_MEDIA_IMAGES,		FALSE,	FVC_ELEMENT_DEPTH_8,	FVC_TYPE_NONE,	(HUINT8 *)"IMAGES", 		(FVC_ELEMENT_t *)s_astElemIpEpgProgMediaImage,		(FVC_ATTR_t *)NULL		},
	{FVC_ELEM_PROGRAM_MEDIA_PUBDATE,		FALSE,	FVC_ELEMENT_DEPTH_8,	FVC_TYPE_DECIMAL,	(HUINT8 *)"PUBDATE", 		(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL		},
	{FVC_ELEM_PROGRAM_MEDIA_EXPIREDATE,	FALSE,	FVC_ELEMENT_DEPTH_8,	FVC_TYPE_DECIMAL,	(HUINT8 *)"EXPIREDATE", 	(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL		},
	{FVC_ELEM_PROGRAM_MEDIA_TRANSMITDATE,	FALSE,	FVC_ELEMENT_DEPTH_8,	FVC_TYPE_DECIMAL,	(HUINT8 *)"TRANSMITDATE", 	(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL		},
	{FVC_ELEM_PROGRAM_MEDIA_PRICE,		TRUE,	FVC_ELEMENT_DEPTH_8,	FVC_TYPE_DECIMAL,	(HUINT8 *)"PRICE", 			(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL		}
};

const static FVC_ELEMENT_t 	s_astElemIpEpgProgSeriesLink[] =
{
	{FVC_ELEM_PROGRAM_SERIESLINK,			TRUE,	FVC_ELEMENT_DEPTH_8,	FVC_TYPE_NONE,	(HUINT8 *)"SERIESLINK", 	(FVC_ELEMENT_t *)s_astElemIpEpgProgSeriesLinkName,	(FVC_ATTR_t *)s_astAttrIpEpgProSeriesLink		}
};


const static FVC_ELEMENT_t		s_astElemIpEpgProgImage[] =
{
	{FVC_ELEM_PROGRAM_IMAGE,				TRUE,	FVC_ELEMENT_DEPTH_7,	FVC_TYPE_STRING,		(HUINT8 *)"IMAGE",	(FVC_ELEMENT_t *)NULL,		(FVC_ATTR_t *)s_astAttrIpEpgProgImage}
};

const static FVC_ELEMENT_t		s_astElemIpEpgProgPerson[] =
{
	{FVC_ELEM_PROGRAM_PERSON,				TRUE,	FVC_ELEMENT_DEPTH_7,	FVC_TYPE_STRING,		(HUINT8 *)"PERSON",	(FVC_ELEMENT_t *)NULL,		(FVC_ATTR_t *)s_astAttrIpEpgProgPerson}
};

const static FVC_ELEMENT_t		s_astElemIpEpgProgGenre[] =
{
	{FVC_ELEM_PROGRAM_GENRE,				TRUE,	FVC_ELEMENT_DEPTH_7,	FVC_TYPE_STRING,		(HUINT8 *)"GENRE",	(FVC_ELEMENT_t *)NULL,		(FVC_ATTR_t *)s_astAttrIpEpgProgGenre}
};

const static FVC_ELEMENT_t		s_astElemIpEpgProgCodec[] =
{
	{FVC_ELEM_PROGRAM_CODEC,				TRUE,	FVC_ELEMENT_DEPTH_7,	FVC_TYPE_STRING,		(HUINT8 *)"CODEC",	(FVC_ELEMENT_t *)NULL,		(FVC_ATTR_t *)s_astAttrIpEpgProgCodec}
};

const static FVC_ELEMENT_t		s_astElemIpEpgProgSeriesLinks[] =
{
	{FVC_ELEM_PROGRAM_SERIESLINKS,		FALSE,	FVC_ELEMENT_DEPTH_7,	FVC_TYPE_NONE,		(HUINT8 *)"SERIESLINKS",	(FVC_ELEMENT_t *)s_astElemIpEpgProgSeriesLink,		(FVC_ATTR_t *)NULL},
	{FVC_ELEM_PROGRAM_SERIESCRID,			TRUE,	FVC_ELEMENT_DEPTH_7,	FVC_TYPE_STRING,		(HUINT8 *)"SERIESCRID",		NULL,													(FVC_ATTR_t *)NULL}
};

const static FVC_ATTR_t			s_astAttrIpEpgProgMedia[] =
{
	{FVC_ATTR_PROGRAM_MEDIA_ID,		TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"ID"	}
};

const static FVC_ELEMENT_t		s_astElemIpEpgProgMedia[] =
{
	{FVC_ELEM_PROGRAM_MEDIA,			TRUE,	FVC_ELEMENT_DEPTH_7,	FVC_TYPE_NONE,		(HUINT8 *)"MEDIA",	(FVC_ELEMENT_t *)s_astElemIpEpgProgMediaInfo,		(FVC_ATTR_t *)s_astAttrIpEpgProgMedia}
};


const static FVC_ELEMENT_t 	s_astElemIpEpgProgInfo[] =
{
	{FVC_ELEM_PROGRAM_NAME,			FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_STRING,	(HUINT8 *)"TITLE",			(FVC_ELEMENT_t *)NULL,							(FVC_ATTR_t *)s_astAttrIpEpgProgName		},
	{FVC_ELEM_PROGRAM_SUBNAME,		FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_STRING,	(HUINT8 *)"TITLESUB", 		(FVC_ELEMENT_t *)NULL,							(FVC_ATTR_t *)s_astAttrIpEpgProgSubName		},
	{FVC_ELEM_PROGRAM_SHORTTEXT,		FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_STRING,	(HUINT8 *)"SHORTTEXT", 		(FVC_ELEMENT_t *)NULL,							(FVC_ATTR_t *)s_astAttrIpEpgProgShortText		},
	{FVC_ELEM_PROGRAM_EVENTID,		FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_DECIMAL, (HUINT8 *)"EVENTID",		(FVC_ELEMENT_t *)NULL,							(FVC_ATTR_t *)NULL		},
	{FVC_ELEM_PROGRAM_CRID,			FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_STRING,	(HUINT8 *)"CRID",			(FVC_ELEMENT_t *)NULL,							(FVC_ATTR_t *)NULL		},
	{FVC_ELEM_PROGRAM_LONGTEXT,		FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_STRING,	(HUINT8 *)"LONGTEXT", 		(FVC_ELEMENT_t *)NULL,							(FVC_ATTR_t *)s_astAttrIpEpgProgLongText		},
	{FVC_ELEM_PROGRAM_RECOMMENDATION,	FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_DECIMAL,	(HUINT8 *)"RECOMMENDATION", (FVC_ELEMENT_t *)NULL,							(FVC_ATTR_t *)NULL		},
	{FVC_ELEM_PROGRAM_RATING,			FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_DECIMAL,	(HUINT8 *)"RATING", 		(FVC_ELEMENT_t *)NULL,							(FVC_ATTR_t *)NULL		},
	{FVC_ELEM_PROGRAM_IMAGES,			FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_NONE,	(HUINT8 *)"IMAGES", 		(FVC_ELEMENT_t *)s_astElemIpEpgProgImage,			(FVC_ATTR_t *)NULL		},
	{FVC_ELEM_PROGRAM_PERSONS,		FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_NONE,	(HUINT8 *)"PERSONS",		(FVC_ELEMENT_t *)s_astElemIpEpgProgPerson,		(FVC_ATTR_t *)NULL		},
	{FVC_ELEM_PROGRAM_GENRES, 		FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_NONE,	(HUINT8 *)"GENRES", 		(FVC_ELEMENT_t *)s_astElemIpEpgProgGenre, 		(FVC_ATTR_t *)NULL		},
	{FVC_ELEM_PROGRAM_CODECS, 		FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_NONE,	(HUINT8 *)"CODECS", 		(FVC_ELEMENT_t *)s_astElemIpEpgProgCodec, 		(FVC_ATTR_t *)NULL		},
	{FVC_ELEM_PROGRAM_SERIES,			FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_NONE,	(HUINT8 *)"SERIES", 		(FVC_ELEMENT_t *)s_astElemIpEpgProgSeriesLinks,	(FVC_ATTR_t *)s_astAttrIpEpgProSeries		},
	{FVC_ELEM_PROGRAM_MEDIAS, 		TRUE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_NONE,	(HUINT8 *)"MEDIAS", 		(FVC_ELEMENT_t *)s_astElemIpEpgProgMedia, 		(FVC_ATTR_t *)NULL		}
};


const static FVC_ELEMENT_t		s_astElemIpEpgProgram[] =
{
	{FVC_ELEM_PROGRAM,				TRUE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_NONE,		(HUINT8 *)"PROGRAM",	(FVC_ELEMENT_t *)s_astElemIpEpgProgInfo,		(FVC_ATTR_t *)s_astAttrIpEpgProgram}
};

const static FVC_ELEMENT_t		s_astElemIpEpgPrograms[] =
{
	{FVC_ELEM_PROGRAMS,				TRUE,	FVC_ELEMENT_DEPTH_4,	FVC_TYPE_NONE,		(HUINT8 *)"PROGRAMS",	(FVC_ELEMENT_t *)s_astElemIpEpgProgram,		(FVC_ATTR_t *)NULL}
};

const static FVC_ELEMENT_t		s_astElemEvtProgramDescriptionImage[] =
{
	{FVC_ELEM_CHANNEL_IMAGE,			TRUE,	FVC_ELEMENT_DEPTH_4,	FVC_TYPE_STRING,		(HUINT8 *)"IMAGE",		(FVC_ELEMENT_t *)NULL,							(FVC_ATTR_t *)s_astAttrIpEpgChannelImage}
};

const static FVC_ELEMENT_t		s_astElemEvtProgramDescriptionTripleId[] =
{
	{FVC_ELEM_CHANNEL_TRIPLEID,		TRUE,	FVC_ELEMENT_DEPTH_4,	FVC_TYPE_NONE,		(HUINT8 *)"TRIPLEID",	(FVC_ELEMENT_t *)NULL,							(FVC_ATTR_t *)s_astAttrIpEpgChannelTripleId}
};

const static FVC_ELEMENT_t		s_astElemEvtProgramDescriptionInfo[] =
{
	{FVC_ELEM_CHANNEL_NUM,			FALSE,	FVC_ELEMENT_DEPTH_3,	FVC_TYPE_DECIMAL, 	(HUINT8 *)"CHANNELNUM", (FVC_ELEMENT_t *)NULL,							(FVC_ATTR_t *)NULL},
	{FVC_ELEM_CHANNEL_NAME,			FALSE,	FVC_ELEMENT_DEPTH_3,	FVC_TYPE_STRING,		(HUINT8 *)"CHANNELNAME",(FVC_ELEMENT_t *)NULL,							(FVC_ATTR_t *)NULL},
	{FVC_ELEM_CHANNEL_TRIPLEIDS,		FALSE,	FVC_ELEMENT_DEPTH_3,	FVC_TYPE_NONE,		(HUINT8 *)"TRIPLEIDS",	(FVC_ELEMENT_t *)s_astElemEvtProgramDescriptionTripleId,	(FVC_ATTR_t *)NULL},
	{FVC_ELEM_CHANNEL_IMAGES, 		FALSE,	FVC_ELEMENT_DEPTH_3,	FVC_TYPE_NONE,		(HUINT8 *)"IMAGES", 	(FVC_ELEMENT_t *)s_astElemEvtProgramDescriptionImage,		(FVC_ATTR_t *)NULL},
	{FVC_ELEM_CHANNEL_UPDATE, 		TRUE,	FVC_ELEMENT_DEPTH_3,	FVC_TYPE_STRING,		(HUINT8 *)"UPDATETYPE", (FVC_ELEMENT_t *)s_astElemIpEpgPrograms,			(FVC_ATTR_t *)s_astAttrIpEpgProgramUpdate}
};
*/





#define __________Program_Information_Table______


const static FVC_ELEMENT_t		s_astElemEvtMediaLocator[] =
{
	{FVC_ELEM_EVENT_PI_MEDIA_URL,		TRUE,		FVC_ELEMENT_DEPTH_7,		FVC_TYPE_STRING,			(HUINT8 *)"MediaUri",			NULL,	NULL	}
};

const static FVC_ELEMENT_t		s_astElemEvtRelatedMaterial[] =
{
	{FVC_ELEM_EVENT_PI_MEDIA_LOCATOR,		TRUE,		FVC_ELEMENT_DEPTH_6,		FVC_TYPE_NONE,			(HUINT8 *)"MediaLocator",			(FVC_ELEMENT_t *)s_astElemEvtMediaLocator,	NULL	}
};

const static FVC_ATTR_t	s_astAttrEvtGenre[] =
{
	{FVC_ATTR_EVENT_PI_GENRE_HREF,		TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"href"	}
};

const static FVC_ELEMENT_t		s_astElemEvtGuidanceExplanatoryText[] =
{
	{FVC_ELEM_EVENT_PI_GUIDANCE_EXPLANATORY_TEXT,		TRUE,		FVC_ELEMENT_DEPTH_6,		FVC_TYPE_STRING,			(HUINT8 *)"ExplanatoryText",			(FVC_ELEMENT_t *)s_astElemEvtMediaLocator,	NULL	}
};

const static FVC_ELEMENT_t		s_astElemEvtBasicDescription[] =
{
	{FVC_ELEM_EVENT_PI_TITLE,				FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_STRING, 	(HUINT8 *)"Title", 				(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_PI_SYNOPSIS,			FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_STRING,	(HUINT8 *)"Synopsis",			(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_PI_GENRE,				FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_NONE,		(HUINT8 *)"Genre",				(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)s_astAttrEvtGenre},
	{FVC_ELEM_EVENT_PI_GUIDANCE, 			FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_NONE,		(HUINT8 *)"ParentalGuidance", 	(FVC_ELEMENT_t *)s_astElemEvtGuidanceExplanatoryText,			(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_PI_RELATED_MATERIAL, 	TRUE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_NONE,		(HUINT8 *)"RelatedMaterial", 	(FVC_ELEMENT_t *)s_astElemEvtRelatedMaterial,			(FVC_ATTR_t *)NULL}
};

const static FVC_ATTR_t	s_astAttrEvtProgramInfo[] =
{
	{FVC_ATTR_EVENT_PI_PROGRAMID,		TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"programId"	}
};

const static FVC_ELEMENT_t		s_astElemEvtIDAVAttributesVideo[] =
{
	{FVC_ELEM_EVENT_PL_ID_AVATTRIBUTES_VIDEO_HORIZONTAL,	FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_DECIMAL, 		(HUINT8 *)"HorizontalSize", 	(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_PL_ID_AVATTRIBUTES_VIDEO_VERTICAL,		TRUE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_DECIMAL, 		(HUINT8 *)"VerticalSize", 		(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL},
};

const static FVC_ELEMENT_t		s_astElemEvtIDAVAttributes[] =
{
	{FVC_ELEM_EVENT_PL_ID_AVATTRIBUTES_VIDEO,	FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_NONE, 		(HUINT8 *)"VideoAttributes", 				(FVC_ELEMENT_t *)s_astElemEvtIDAVAttributesVideo,	(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_PL_ID_AVATTRIBUTES_AUDIO,	TRUE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_DECIMAL,	(HUINT8 *)"AudioAttributes", 				(FVC_ELEMENT_t *)NULL,	(FVC_ATTR_t *)NULL},
};

const static FVC_ELEMENT_t		s_astElemEvtProgramInfo[] =
{
	{FVC_ELEM_EVENT_PI_BASICDESCRIPTION,		TRUE,	FVC_ELEMENT_DEPTH_4,	FVC_TYPE_NONE, 	(HUINT8 *)"BasicDescription", (FVC_ELEMENT_t *)s_astElemEvtBasicDescription, (FVC_ATTR_t *)NULL},
};

const static FVC_ELEMENT_t		s_astElemEvtPit[] =
{
	{FVC_ELEM_EVENT_PROGRAM_INFORMATION,	TRUE,	FVC_ELEMENT_DEPTH_3,	FVC_TYPE_NONE, 	(HUINT8 *)"ProgramInformation", (FVC_ELEMENT_t *)s_astElemEvtProgramInfo,	(FVC_ATTR_t *)s_astAttrEvtProgramInfo},
};

#define __________Program_Location_Table______

const static FVC_ATTR_t	s_astAttrEvtIDGenre[] =
{
	{FVC_ATTR_EVENT_PL_ID_GENRE_HREF,		TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"href"	}
};

const static FVC_ATTR_t	s_astAttrEvtIDOtherIdentifier[] =
{
	{FVC_ATTR_EVENT_PL_ID_OTHER_IDENTIFIER_TYPE,	TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"type"	}
};

const static FVC_ELEMENT_t		s_astElemEvtInstanceDescription[] =
{
	{FVC_ELEM_EVENT_PL_ID_GENRE,			FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_NONE, 		(HUINT8 *)"Genre", 				(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)s_astAttrEvtIDGenre},
	{FVC_ELEM_EVENT_PL_ID_OTHER_IDENTIFIER, FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_STRING,	(HUINT8 *)"OtherIdentifier", 	(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)s_astAttrEvtIDOtherIdentifier},
	{FVC_ELEM_EVENT_PL_ID_AVATTRIBUTES,		TRUE,	FVC_ELEMENT_DEPTH_4,	FVC_TYPE_NONE,		(HUINT8 *)"AVAttributes", 		(FVC_ELEMENT_t *)s_astElemEvtIDAVAttributes, (FVC_ATTR_t *)NULL},
};

const static FVC_ATTR_t	s_astAttrEvtScheduleEventCrid[] =
{
	{FVC_ATTR_EVENT_PL_PROGRAMID,		TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"crid"	}
};

const static FVC_ATTR_t s_astAttrEvtOnDemandCrid[] =
{
	{FVC_ELEM_EVENT_PL_OD_PROGRAMID,		TRUE,	FVC_TYPE_STRING,	(HUINT8 *)"crid"	}
};

const static FVC_ATTR_t s_astAttrEvtProgramURLType[] =
{
	{FVC_ATTR_EVENT_PL_OD_PROGRAM_URL_TYPE,		TRUE,	FVC_TYPE_STRING,	(HUINT8 *)"contentType"	}
};

const static FVC_ATTR_t s_astAttrEvtAuxilaryURLType[] =
{
	{FVC_ATTR_EVENT_PL_OD_AUXILIARY_URL_TYPE,		TRUE,	FVC_TYPE_STRING,	(HUINT8 *)"contentType"	}
};

const static FVC_ATTR_t s_astAttrEvtFreeValue[] =
{
	{FVC_ATTR_EVENT_PL_OD_FREE_VALUE,		TRUE,	FVC_TYPE_STRING,	(HUINT8 *)"value"	}
};


const static FVC_ELEMENT_t		s_astElemEvtScheduleEvent[] =
{
	{FVC_ELEM_EVENT_PL_PROGRAM,			FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_NONE, 		(HUINT8 *)"Program", 				(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)s_astAttrEvtScheduleEventCrid},
	{FVC_ELEM_EVENT_PL_PROGRAM_URL,		FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_STRING,	(HUINT8 *)"ProgramURL",				(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_PL_INSTANCE_DESCRIPTION,FALSE,FVC_ELEMENT_DEPTH_5,	FVC_TYPE_NONE,		(HUINT8 *)"InstanceDescription",	(FVC_ELEMENT_t *)s_astElemEvtInstanceDescription,(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_PL_START_TIME,		FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_STRING,	(HUINT8 *)"PublishedStartTime",		(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_PL_DURATION, 		TRUE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_STRING,	(HUINT8 *)"PublishedDuration", 		(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL}
};

const static FVC_ELEMENT_t		s_astElemEvtSchedule[] =
{
	{FVC_ELEM_EVENT_PL_SCHEDULE_EVENT,			TRUE,	FVC_ELEMENT_DEPTH_4,	FVC_TYPE_NONE, 	(HUINT8 *)"ScheduleEvent", (FVC_ELEMENT_t *)s_astElemEvtScheduleEvent,	(FVC_ATTR_t *)NULL},
};

const static FVC_ELEMENT_t		s_astElemEvtODIDAVAttributesVideo[] =
{
	{FVC_ELEM_EVENT_PL_OD_ID_AVATTRIBUTES_VIDEO_HORIZONTAL,	FALSE,	FVC_ELEMENT_DEPTH_7,	FVC_TYPE_DECIMAL, 		(HUINT8 *)"HorizontalSize", 	(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_PL_OD_ID_AVATTRIBUTES_VIDEO_VERTICAL,	TRUE,	FVC_ELEMENT_DEPTH_7,	FVC_TYPE_DECIMAL, 		(HUINT8 *)"VerticalSize", 		(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL},
};

const static FVC_ELEMENT_t		s_astElemEvtODIDAVAttributes[] =
{
	{FVC_ELEM_EVENT_PL_OD_ID_AVATTRIBUTES_VIDEO,	TRUE,	FVC_ELEMENT_DEPTH_7,	FVC_TYPE_NONE, 		(HUINT8 *)"VideoAttributes", 				(FVC_ELEMENT_t *)s_astElemEvtODIDAVAttributesVideo,	(FVC_ATTR_t *)NULL},
};

const static FVC_ATTR_t	s_astAttrEvtODIDGenre[] =
{
	{FVC_ATTR_EVENT_PL_OD_ID_GENRE_HREF,		TRUE,	FVC_TYPE_STRING, 	(HUINT8 *)"href"	}
};


const static FVC_ELEMENT_t		s_astElemEvtODInstanceDescription[] =
{
	{FVC_ELEM_EVENT_PL_OD_ID_GENRE,			FALSE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_NONE, 		(HUINT8 *)"Genre", 				(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)s_astAttrEvtODIDGenre},
	{FVC_ELEM_EVENT_PL_OD_ID_AVATTRIBUTES,	TRUE,	FVC_ELEMENT_DEPTH_6,	FVC_TYPE_NONE, 		(HUINT8 *)"AVAttributes", 		(FVC_ELEMENT_t *)s_astElemEvtODIDAVAttributes,	(FVC_ATTR_t *)NULL},
};


const static FVC_ELEMENT_t		s_astElemEvtOnDemand[] =
{
	{FVC_ELEM_EVENT_PL_OD_PROGRAM,				FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_NONE, 		(HUINT8 *)"Program", 				(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)s_astAttrEvtOnDemandCrid},
	{FVC_ELEM_EVENT_PL_OD_PROGRAM_URL,			FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_STRING,	(HUINT8 *)"ProgramURL",				(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)s_astAttrEvtProgramURLType},
	{FVC_ELEM_EVENT_PL_OD_AUXILIARY_URL,		FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_STRING,	(HUINT8 *)"AuxiliaryURL",			(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)s_astAttrEvtAuxilaryURLType},
	{FVC_ELEM_EVENT_PL_OD_INSTANCE_DESCRIPTION,	FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_NONE,		(HUINT8 *)"InstanceDescription",	(FVC_ELEMENT_t *)s_astElemEvtODInstanceDescription,(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_PL_OD_DURATION,				FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_STRING,	(HUINT8 *)"PublishedDuration", 		(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_PL_OD_STARTOFAVAILABILITY,	FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_STRING,	(HUINT8 *)"StartOfAvailability",	(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_PL_OD_ENDOFAVAILABILITY,	FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_STRING,	(HUINT8 *)"EndOfAvailability",		(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_PL_OD_FREE,					FALSE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_NONE,		(HUINT8 *)"Free",					(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)s_astAttrEvtFreeValue},
	{FVC_ELEM_EVENT_PL_OD_DELIVERYMODE,			TRUE,	FVC_ELEMENT_DEPTH_5,	FVC_TYPE_STRING,	(HUINT8 *)"DeliveryMode",			(FVC_ELEMENT_t *)NULL,			(FVC_ATTR_t *)NULL},
};

const static FVC_ELEMENT_t		s_astElemEvtPlt[] =
{
	{FVC_ELEM_EVENT_SCHEDULE,	FALSE,	FVC_ELEMENT_DEPTH_3,	FVC_TYPE_NONE, 	(HUINT8 *)"Schedule", (FVC_ELEMENT_t *)s_astElemEvtSchedule,	(FVC_ATTR_t *)NULL},
	{FVC_ELEM_EVENT_ONDEMAND,	TRUE,	FVC_ELEMENT_DEPTH_3,	FVC_TYPE_NONE, 	(HUINT8 *)"OnDemandProgram", (FVC_ELEMENT_t *)s_astElemEvtOnDemand,	(FVC_ATTR_t *)NULL},

};

#define __________Program_Description______

const static FVC_ELEMENT_t		s_astElemEvtProgramDescription[] =
{
	{FVC_ELEM_EVENT_PIT,				FALSE,	FVC_ELEMENT_DEPTH_2,	FVC_TYPE_NONE,		(HUINT8 *)"ProgramInformationTable",	(FVC_ELEMENT_t *)s_astElemEvtPit,		NULL},
	{FVC_ELEM_EVENT_PLT,				TRUE,	FVC_ELEMENT_DEPTH_2,	FVC_TYPE_NONE,		(HUINT8 *)"ProgramLocationTable",		(FVC_ELEMENT_t *)s_astElemEvtPlt,		NULL}
};


const static FVC_ELEMENT_t		s_astElemEvtTvaMain[] =
{
	{FVC_ELEM_EVENT_PROGRAMDESCRIPTION,	TRUE,	FVC_ELEMENT_DEPTH_1,	FVC_TYPE_NONE,	(HUINT8 *)"ProgramDescription",	(FVC_ELEMENT_t *)s_astElemEvtProgramDescription,		NULL}
};

const static FVC_ELEMENT_t	s_astElemIpEpg[] =
{
	{FVC_ELEM_EVENT_TVAMAIN,			TRUE,	FVC_ELEMENT_DEPTH_ROOT,	FVC_TYPE_NONE,	(HUINT8 *)"TVAMain",		(FVC_ELEMENT_t *)s_astElemEvtTvaMain,	NULL	}
};


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static FVC_REPLACESTR_t		s_astIpepgHumaxReplaceString[] =
{
		{	5,		FALSE,	{'&', 'a', 'm', 'p', ';', '\0', 0, },		0x00	},
		{	6,		TRUE,	{'&', 'q', 'u', 'o', 't', ';', '\0', 0, },	0x22	},
		{	4,		TRUE,	{'&', 'g', 't', ';', '\0', 0, },			0x3C	},
		{	4,		TRUE,	{'&', 'l', 't', ';', '\0', 0, },			0x3E	},

		// system specification 2.4.2
		// 5.3 programme data
		{	2, 		TRUE, 	{'\\', 'n', '\0', 0, }, 				0x0A	},
		{	2, 		TRUE, 	{'\\', 't', '\0', 0, }, 				0x09	},
		{	1, 		TRUE, 	{'\\', '\0', 0, },		 			0x5C	},
		{	0, 		FALSE, 	{0, }, 						0x00	}
};

static HERROR			ipepg_LoadSvcElementList(FVC_ELEMENT_t **ppstElementList)
{
	IPEPG_RETURN_IF(ppstElementList == NULL, ERR_FAIL);
	*ppstElementList = (FVC_ELEMENT_t *)&s_astElementTvaMain;
	return ERR_OK;
}

static HERROR			ipepg_LoadEventElementList(FVC_ELEMENT_t **ppstElementList)
{
	IPEPG_RETURN_IF(ppstElementList == NULL, ERR_FAIL);
	*ppstElementList = (FVC_ELEMENT_t *)&s_astElemIpEpg;

	return ERR_OK;
}
static HERROR	ipepg_parseDateTimeField(HUINT8 **field, HINT32 numChars,
                                      HINT16 minValue, HINT16 maxValue,
                                      HCHAR trailing, HINT16 *value)
{
	HUINT8 *s;
    HINT32 total = 0;

    if (numChars < 0 || numChars > 5)
    {
        return ERR_FAIL;
    }

    s = *field;

    while (numChars-- > 0)
    {
        char c = *s++;

        if (!isdigit((int)((unsigned char)c)))
        {
            return ERR_FAIL;
        }

        total = (total * 10) + (HCHAR)(c - '0');
    }

    if (total < minValue || total > maxValue || total > 65535)
    {
        return ERR_FAIL;
    }

    if (trailing != '\0')
    {
        if (*s++ != trailing)
        {
            return ERR_FAIL;
        }
    }

    *field = s;
    *value = (HINT16)total;
    return ERR_OK;
}
static HUINT32		ipepg_Hextoi (HUINT8 *pucPtr)
{
	HUINT32	ulVal = 0 ;


	if (pucPtr == NULL)
	{
		return 0 ;
	}

	while (1)
	{
		if (*pucPtr=='\0')
		{
			break ;
		}
		else if (*pucPtr=='0' && ( pucPtr[1]=='x' || pucPtr[1]=='X' ))
		{
			pucPtr++ ;
		}
		else if (*pucPtr>='0' && *pucPtr<='9')
		{
			ulVal *= 16 ;
			ulVal += *pucPtr - '0' ;
		}
		else if (*pucPtr>='a' && *pucPtr<='f')
		{
			ulVal *= 16 ;
			ulVal += *pucPtr - 'a' + 10 ;
		}
		else if (*pucPtr>='A' && *pucPtr<='F')
		{
			ulVal *= 16 ;
			ulVal += *pucPtr - 'A' + 10 ;
		}
		pucPtr++ ;
	}

	return ulVal ;
}

static HBOOL	ipepg_ParseDVBURI (HCHAR *uri, HUINT16 *onid, HUINT16 *tsid, HUINT16 *sid, HUINT16 *eid, HCHAR *tvaID, HUINT16 *svcuid)
{
	char 	*ret;
	char	*backupret;
	char	field[128];
	size_t	adv;

	IPEPG_RETURN_IF( strncmp(uri, "dvb://", 6) != 0, FALSE );
	uri += 6;

	// onid
	IPEPG_RETURN_IF( (ret = strchr(uri, '.')) == NULL, FALSE );
	adv = (size_t)ret - (size_t)uri;
	if (adv && onid)
	{
		memcpy(field, uri, adv); field[adv] = '\0';
		*onid = ipepg_Hextoi(field);
	}
	uri += adv + 1;

	// tsid
	IPEPG_RETURN_IF( (ret = strchr(uri, '.')) == NULL, FALSE );
	adv = (size_t)ret - (size_t)uri;
	if (adv && tsid)
	{
		memcpy(field, uri, adv); field[adv] = '\0';
		*tsid = ipepg_Hextoi(field);
	}
	uri += adv + 1;

	// sid
	IPEPG_RETURN_IF( (ret = strchr(uri, 0x0)) == NULL, FALSE );
	backupret = ret;
	if ((ret = strchr(uri,';')) == NULL)
	{
		ret = backupret;
	}
	adv = (size_t)ret - (size_t)uri;
	if (adv && sid)
	{
		memcpy(field, uri, adv); field[adv] = '\0';
		*sid = ipepg_Hextoi(field);
	}
	uri += adv + 1;

	// eid
	if ((ret = strchr(uri, ';')) == NULL)
	{
		if (*uri == '\0')
		{
			return FALSE;
		}
		if (eid)
		{
			*eid = ipepg_Hextoi(uri);
		}
		return TRUE;
	}
	adv = (size_t)ret - (size_t)uri;
	if (adv && eid)
	{
		memcpy(field, uri, adv); field[adv] = '\0';
		*eid = ipepg_Hextoi(field);
	}
	uri += adv + 1;

	// TVA id
	if ((ret = strchr(uri, ';')) == NULL)
	{
		IPEPG_RETURN_IF( *uri == '\0', FALSE );
		if (tvaID)
		{
			strcpy(tvaID, uri);
		}
		return TRUE;
	}
	adv = (size_t)ret - (size_t)uri;
	if (adv && tvaID)
	{
		memcpy(tvaID, uri, adv); tvaID[adv] = '\0';
	}
	uri += adv + 1;

	// service_uid
	IPEPG_RETURN_IF( *uri == '\0', FALSE );
	if (svcuid)
	{
		*svcuid = ipepg_Hextoi(uri);
	}
	return TRUE;
}


HINT32 ipepg_ParseHeaderFieldValue(HCHAR *szValue, HCHAR *szHeader, HCHAR *szName, HINT32 nLimit)
{
    HCHAR   *pPos = NULL, *pNextPos = NULL;
    HINT32  nLen = 0;

    if( szValue == NULL || szHeader == NULL || szName == NULL ) return -1;

    pPos = HxSTD_StrStr(szHeader, szName);

    if( pPos == NULL )
	{
		HxLOG_Warning("Fail to find header=[%s] \n",szName);
		return -1;
    }

    /* Find the last match! */
    while( 1 )
    {
        pPos += strlen(szName);

        pNextPos = HxSTD_StrStr(pPos, szName);
        if( pNextPos == NULL )
            break;
        else
            pPos = pNextPos;
    }

    while( (*pPos == ':') || (*pPos == ' ') )
    {
        if( pPos > (szHeader + nLimit) ) break;
        pPos++;
    }

     while( 1 )
     {
           if( (pPos > (szHeader + nLimit)) || (*pPos == EOF) || (*pPos == '\r') || (*pPos == '\n') )
                        break;

           szValue[nLen] = *pPos;

           nLen++;
           pPos++;
     }

     szValue[nLen] = '\0';

     return nLen;
}
HUINT32				ipepg_ParseMonth (HCHAR *month)
{
	HCHAR	szMonth[4] = {0,};
	HCHAR	*szMonthTable[12] = { "jan","feb","mar","apr","may","jun","jul","aug","sep","oct","nov","dec" };
	HUINT32	i;
	HxSTD_StrNCpy(szMonth, month, 3);
	szMonth[3] = '\0';

	for (i=0; i<12; i++)
	{
		if (HLIB_STD_StrNCaseCmp(szMonth, szMonthTable[i], 3) == 0 )
		{
			return i+1;
		}
	}

	HxLOG_Error("parse month failed. month =[%s] \n",szMonth);
	return 0;
}

static HERROR		ipepg_IsReplaceString(HUINT8 *pucPtr, HBOOL *pbIsReplace, HUINT8 *pucSize, HUINT8 *puReplaceCode)
{
	HINT16					sIndex = 0;
	HINT32					lRet = 0;
	//HxLOG_Trace();

	IPEPG_RETURN_IF(pucPtr == NULL || pbIsReplace == NULL || pucSize == NULL || puReplaceCode == NULL, ERR_FAIL);

	*pbIsReplace = FALSE;
	*pucSize = 0;
	*puReplaceCode = 0;

	for( ; ; sIndex++)
	{
		if(s_astIpepgHumaxReplaceString[sIndex].szOrgStr[0] == 0)
		{
			break;
		}

		lRet = (HINT32)memcmp(pucPtr, s_astIpepgHumaxReplaceString[sIndex].szOrgStr, s_astIpepgHumaxReplaceString[sIndex].ucOrgStrLen);
		if(lRet == 0)
		{
			*pbIsReplace = TRUE;
			*pucSize = s_astIpepgHumaxReplaceString[sIndex].ucOrgStrLen;
			*puReplaceCode = s_astIpepgHumaxReplaceString[sIndex].ucReplaceCode;
			break;
		}
	}

	return ERR_OK;
}

static HERROR		ipepg_SetDataByType(	HUINT32 ulType,
													HUINT8 *pucSrc,
													HUINT32 ulMaxLen,
													HUINT8 *pszString,
													HUINT32 *pulDecimal,
													HUINT32 *ulRealLen
													)
{
	HERROR					hError = ERR_FAIL;
	HINT16					sIndex = 0;
	HINT16					sCount = 0;
	HBOOL					bReplace = FALSE;
	HUINT8					ucSize = 0;
	HUINT8					ucReplaceCode = 0;
	HUINT8					*pucTempString = NULL;
	HUINT32 				ulLandCodeLen = 0;	// FOR OCTO 2.0
	//HxLOG_Trace();

	IPEPG_RETURN_IF(pucSrc == NULL, ERR_FAIL);

	switch(ulType)
	{
		case FVC_TYPE_STRING :
			// it is type of UTF8 that is received data from RP SERVER.
			if(pszString == NULL)
				return ERR_FAIL;
			pucTempString = &(pszString[ulLandCodeLen]);

			for(sIndex = 0 ; sIndex<  (ulMaxLen - ulLandCodeLen) - 1 ; sIndex++)
			{
				if(pucSrc[sIndex] == '\0' || sCount >= (ulMaxLen - ulLandCodeLen))
				{
					break;
				}

				if(pucSrc[sIndex] == '\\' || pucSrc[sIndex] == '&')
				{
					bReplace = FALSE;
					ucSize = 0;
					ucReplaceCode = 0;
					hError = ipepg_IsReplaceString(&pucSrc[sIndex], &bReplace, &ucSize, &ucReplaceCode);
					if(hError != ERR_OK)
					{
					}

					if(bReplace == TRUE)
					{
						sIndex += (ucSize-1);
						pucTempString[sCount++] = ucReplaceCode;
						continue;
					}
				}
				pucTempString[sCount++] = pucSrc[sIndex];

			}
			pucTempString[sCount]	= '\0';
			*ulRealLen= sCount+1;
			break;

		case FVC_TYPE_DECIMAL :
			*pulDecimal = atoi((const char *)pucSrc);
			break;

		case FVC_TYPE_DECIMAL64 :
			*pulDecimal = 0;
			break;

		case FVC_TYPE_HEXA:
			*pulDecimal = ipepg_Hextoi(pucSrc);
			break;

		default :
			break;
	}

	return ERR_OK;
}

HBOOL ipepg_CompareTripleId(void *elem1, void* elem2)
{
	EPG_URL_INFO_t *pEpg = (EPG_URL_INFO_t *)elem2;
	IPEPG_TripleId_t *pTripleId1, *pTripleId2;
	//HxLOG_Trace();

	pTripleId1 = &pEpg->stTripleId;
	pTripleId2 = (IPEPG_TripleId_t *)elem1;

	if (pTripleId1->usOnId == pTripleId2->usOnId &&
		pTripleId1->usTsId == pTripleId2->usTsId &&
		pTripleId1->usSvcId == pTripleId2->usSvcId)
	{
		return TRUE;
	}

	return FALSE;
}

typedef	HERROR	(*local_ipepg_SetData_t)(void * target, HUINT32	eDataIdx, HUINT32 ulMaxLen, HUINT8 *pszString, HUINT32 ulDecimal);
static void	TEST_Function(HUINT32 _line, HCHAR * TagName, HxXMLELEMENT taget)
{
#if 0
	HCHAR * 	pucTestName = NULL;
	HLIB_XML_GetElementName(taget, (HUINT8**)&pucTestName);
	if(pucTestName != NULL && TagName != NULL)
		HxLOG_Print("=========================> LINE[%d] TagName[%s] EleName[%s]\n", _line, TagName, pucTestName);
	else if(pucTestName != NULL)
		HxLOG_Print("=========================> LINE[%d] EleName[%s]\n", _line, pucTestName);
	else
		HxLOG_Print("=========================> LINE[%d] TagName[%s]\n", _line, TagName);
#endif

}

void	ipepg_cbFreeFvcProgramInfo	(void* key , void* value , void* user_data)
{
	IPEPG_MemFree(value);
}



static HERROR ipepg_Fvc_SetServiceData(void	*target, HUINT32 eDataIdx,HUINT32 ulMaxLen, HUINT8 *pszString, HUINT32 pulDecimal)
{
	EPG_URL_INFO_t	*pstTempURLInfo = (EPG_URL_INFO_t	*)target;
	//HxLOG_Trace();

	if(pstTempURLInfo == NULL)
	{
		return ERR_FAIL;
	}
	switch((FVC_TAG_e)eDataIdx)
	{
		case FVC_SVC_URL :
			{
				HUINT32 ulLengthUrl = 0;
				HUINT16 onId;
				HUINT16 svcId;

				if(pszString == NULL)
					return ERR_FAIL;

				HxSTD_MemSet(pstTempURLInfo->pucUrl, 0x00, sizeof(HUINT8)*IPEPG_URL_MAX_LEN);
				ulLengthUrl = HxSTD_StrLen(pszString);
				if((ulLengthUrl>=IPEPG_URL_MAX_LEN) || (ulLengthUrl == 0))
				{
					HxLOG_Error(" invalid EPGHUMAX_SVC_URL fail!!\n");
					return ERR_FAIL;
				}
				HxSTD_MemCopy(pstTempURLInfo->pucUrl, pszString, ulLengthUrl);
				ipepg_ParseDVBURI(pstTempURLInfo->pucUrl,&onId, NULL, &svcId, NULL,NULL,NULL);
				pstTempURLInfo->stTripleId.usOnId = onId;
				pstTempURLInfo->stTripleId.usSvcId = svcId;
				{
					HxVector_t	*pstSvcList;
					HINT32		ulSvcLength;
					DxService_t * pstService;

					pstSvcList = APK_META_SVC_FindServiceBySvcId(svcId);
					if (pstSvcList == NULL)
					{
						HxLOG_Error("there is no matching svc id. sid=(%d) \n",svcId);
						return ERR_FAIL;
					}

					ulSvcLength = HLIB_VECTOR_Length (pstSvcList);
					if ( ulSvcLength <= 0 )
					{
						HxLOG_Error("found svc. but list length is zero???. sid=(%d) \n",svcId);
						return ERR_FAIL;
					}

					pstService = (DxService_t*)HLIB_VECTOR_ItemAt(pstSvcList, 0);
					pstTempURLInfo->stTripleId.usTsId= pstService->tsid;
					pstTempURLInfo->ulSvcUid = pstService->uid;

					HLIB_VECTOR_Delete(pstSvcList);
				}

				//pstTempURLInfo->pucUrl[ulLengthUrl]= '\0';
			}
			break;
		case FVC_SVC_MEDIA_URL:
			{
				HUINT32 ulLengthUrl = 0;
				if(pszString == NULL)
					return ERR_FAIL;

				HxSTD_MemSet(pstTempURLInfo->pucThumbnailUrl, 0x00, sizeof(HUINT8)*IPEPG_URL_MAX_LEN);
				ulLengthUrl = HxSTD_StrLen(pszString);
				if((ulLengthUrl>=IPEPG_URL_MAX_LEN) || (ulLengthUrl == 0))
				{
					HxLOG_Error("[xsvc_epglib_ParseToHumaxServiceList_Humax] invalid EPGHUMAX_SVC_URL fail!!\n");
					return ERR_FAIL;
				}
				HxSTD_MemCopy(pstTempURLInfo->pucThumbnailUrl, pszString, ulLengthUrl);
				HxLOG_Print("THUMBNAIL : %s\n", pszString);
			}
			break;
		case FVC_STATION_TVAMAIN :
		case FVC_STATION_PROGRAMMEDESCRIPTION :
		case FVC_STATION_SIT :
		case FVC_STATION_SERVICEINFORMATION:
		case FVC_SVC_MAX :
		case FVC_STATIONID :
		case FVC_LANG :
		case FVC_VERSION :
		case FVC_SVC_NAME :
		default :
			//HxLOG_Error("No Need to Set for this flag[%d]\n", eDataIdx);
			break;
	}

	return ERR_OK;

}

static HERROR 	local_ipepg_IpEventStrDup(DxIpEventStr_t * pIpEventStr, DxIPEPG_Text_e eType, HUINT8 *pszString)
{
	if(NULL != pIpEventStr && NULL != pszString)
	{
		pIpEventStr->eType = eType;
		if(NULL != pIpEventStr->ucStr)
		{
			IPEPG_MemFree(pIpEventStr->ucStr);
		}
		pIpEventStr->ucStr = IPEPG_StrDup(pszString);
		pIpEventStr->usSize = HxSTD_StrLen(pszString);
		return TRUE;
	}
	return FALSE;
}

static HERROR	local_ipepg_SetEventData(void * target, HUINT32	eDataIdx, HUINT32 ulMaxLen, HUINT8 *pszString, HUINT32 ulDecimal)
{
	DxIpEvent_t *pEvent = (DxIpEvent_t *)target;
	HUINT32			ulCount;
	DxIpEventFvcOnDemand_t	*pOnDemand = NULL;
	HUINT8					*pStr;

	if (pEvent == NULL)
	{
		HxLOG_Error("null target");
		return ERR_FAIL;
	}

	//HxLOG_Debug(  HxANSI_COLOR_PURPLE("[%d][%s]\n") , eDataIdx, get_fvc_element_str(eDataIdx));

	switch((FVC_ELEMENT_e)eDataIdx)
	{
		case FVC_ATTR_EVENT_PL_ID_OTHER_IDENTIFIER_TYPE:
			{
				ulCount = pEvent->ext.ukdtt.usCRIDCount;
				if (ulCount >= eDxIPEPG_CRID_MAXCOUNT)
				{
					break;
				}

				if (IPEPG_StrCmp(IPEPG_FVC_OTHER_IDENTIFIER_PCRID,pszString) == 0)
				{
					pEvent->ext.ukdtt.stCRID[ulCount].eType = eDxIPEPG_UKDTT_TEXT_PROGRAMME_CRID;
				}
				else if (IPEPG_StrCmp(IPEPG_FVC_OTHER_IDENTIFIER_SCRID,pszString) == 0)
				{
					pEvent->ext.ukdtt.stCRID[ulCount].eType = eDxIPEPG_UKDTT_TEXT_SERIES_CRID;
				}
				else if (IPEPG_StrCmp(IPEPG_FVC_OTHER_IDENTIFIER_RCRID,pszString) == 0)
				{
					pEvent->ext.ukdtt.stCRID[ulCount].eType = eDxIPEPG_UKDTT_TEXT_RECOMMENDATION_CRID;
				}
			}
			break;
		case FVC_ELEM_EVENT_PL_ID_OTHER_IDENTIFIER:
			{
				ulCount = pEvent->ext.ukdtt.usCRIDCount;
				if (ulCount >= eDxIPEPG_CRID_MAXCOUNT)
				{
					break;
				}

				if (pEvent->ext.ukdtt.stCRID[ulCount].eType == eDxIPEPG_UKDTT_TEXT_PROGRAMME_CRID
					|| pEvent->ext.ukdtt.stCRID[ulCount].eType == eDxIPEPG_UKDTT_TEXT_SERIES_CRID
					|| pEvent->ext.ukdtt.stCRID[ulCount].eType == eDxIPEPG_UKDTT_TEXT_RECOMMENDATION_CRID)
				{
					pEvent->ext.ukdtt.stCRID[ulCount].ucStr = IPEPG_StrDup(pszString);
					pEvent->ext.ukdtt.stCRID[ulCount].usSize = HxSTD_StrLen(pszString);
					pEvent->ext.ukdtt.usCRIDCount++;
				}
				else
				{
					HxLOG_Error("fail to get type of crid. \n");
				}
			}
			break;
		case FVC_ELEM_EVENT_PI_BASICDESCRIPTION:
		case FVC_ELEM_EVENT_PI_RELATED_MATERIAL:
		case FVC_ELEM_EVENT_PI_MEDIA_LOCATOR:
		case FVC_ELEM_EVENT_PL_SCHEDULE_EVENT:
		case FVC_ELEM_EVENT_PL_PROGRAM:
		case FVC_ELEM_EVENT_PI_AV_ATTRIBUTES:
			break;

//		case FVC_ATTR_EVENT_PI_PROGRAMID:
		case FVC_ATTR_EVENT_PL_PROGRAMID:
			{
				pEvent->ext.ukdtt.stProgramId.eType = eDxIPEPG_UKDTT_TEXT_PROGRAMME_CRID;
				pEvent->ext.ukdtt.stProgramId.ucStr = IPEPG_StrDup(pszString);
				pEvent->ext.ukdtt.stProgramId.usSize= HxSTD_StrLen(pszString);
			}
			break;

		case FVC_ATTR_EVENT_PI_GENRE_HREF:
			pStr = pszString;
			if (HLIB_STD_StrNCaseCmp(pszString,IPEPG_FVC_GENRE_PREFIX,MACRO_IPEPG_STRLEN(IPEPG_FVC_GENRE_PREFIX)) != 0)
			{
				break;
			}
			pStr += MACRO_IPEPG_STRLEN(IPEPG_FVC_GENRE_PREFIX);

			if (HLIB_STD_StrNCaseCmp(pStr,IPEPG_FVC_GENRE_CONTENT_SUBJECT,MACRO_IPEPG_STRLEN(IPEPG_FVC_GENRE_CONTENT_SUBJECT)) != 0)
			{
				break;
			}
			pStr += MACRO_IPEPG_STRLEN(IPEPG_FVC_GENRE_CONTENT_SUBJECT);

			{
				HUINT8	ucContentNibbleChar[2];
				HUINT8	ucContentNibble;

				ucContentNibbleChar[0] = *pStr;
				ucContentNibbleChar[1] = 0;

				if (HLIB_STD_IsInteger(ucContentNibbleChar))
				{
					ucContentNibble = ucContentNibbleChar[0] - '0';
					pEvent->stBase.ucGenre = ucContentNibble<<4;
				}

				// TODO: userNibble을 저장할 필요가 있다.  ex) 3.1 Show/Game Show는 contentNibble 3, userNibble 1 ?
			}
			break;
		case FVC_ATTR_EVENT_PL_OD_ID_GENRE_HREF:
			pStr = pszString;
			pOnDemand = &(pEvent->ext.ukdtt.stFvcOnDemand);
			if (HLIB_STD_StrNCaseCmp(pStr,IPEPG_FVC_GENRE_PREFIX,MACRO_IPEPG_STRLEN(IPEPG_FVC_GENRE_PREFIX)) != 0)
			{
				break;
			}
			pStr += MACRO_IPEPG_STRLEN(IPEPG_FVC_GENRE_PREFIX);

			if (HLIB_STD_StrNCaseCmp(pStr,IPEPG_FVC_GENRE_MEDIA_AVAILABILITY,MACRO_IPEPG_STRLEN(IPEPG_FVC_GENRE_MEDIA_AVAILABILITY)) == 0)
			{
				pStr += MACRO_IPEPG_STRLEN(IPEPG_FVC_GENRE_MEDIA_AVAILABILITY);
				if (HLIB_STD_StrNCaseCmp(pStr,IPEPG_FVC_GENRE_MEDIA_AVAILABILITY_AVAILABLE,MACRO_IPEPG_STRLEN(IPEPG_FVC_GENRE_MEDIA_AVAILABILITY_AVAILABLE)) == 0)
				{
					pOnDemand->ulOnDemandGenre |= eDxIPEPG_UKDTT_GENRE_MEDIA;
				}
			}
			else if (HLIB_STD_StrNCaseCmp(pStr,IPEPG_FVC_GENRE_FEPG_AVAILABILITY,MACRO_IPEPG_STRLEN(IPEPG_FVC_GENRE_FEPG_AVAILABILITY)) == 0)
			{
				pStr += MACRO_IPEPG_STRLEN(IPEPG_FVC_GENRE_FEPG_AVAILABILITY);
				if (HLIB_STD_StrNCaseCmp(pStr,IPEPG_FVC_GENRE_FEPG_AVAILABILITY_AVAILABLE,MACRO_IPEPG_STRLEN(IPEPG_FVC_GENRE_FEPG_AVAILABILITY_AVAILABLE)) == 0)
				{
					pOnDemand->ulOnDemandGenre |= eDxIPEPG_UKDTT_GENRE_FEPG;
				}
			}
			break;
		case FVC_ELEM_EVENT_PI_TITLE:
		case FVC_ELEM_EVENT_PI_SYNOPSIS:
		case FVC_ELEM_EVENT_PI_MEDIA_URL:
			{
				DxIPEPG_Text_e eFVC_TextType = eDxIPEPG_TEXT_MAX;
				if(pEvent->ulStrCount >= eDxIPEPG_TEXT_MAXCOUNT)
					break;

				if (eDataIdx == FVC_ELEM_EVENT_PI_TITLE) eFVC_TextType = eDxIPEPG_TEXT_NAME;
				else if (eDataIdx == FVC_ELEM_EVENT_PI_SYNOPSIS) eFVC_TextType = eDxIPEPG_TEXT_DETAIL;
				else if (eDataIdx == FVC_ELEM_EVENT_PI_MEDIA_URL) eFVC_TextType = eDxIPEPG_TEXT_THUMBNAIL;

				pEvent->stText[pEvent->ulStrCount].eType = eFVC_TextType;
				pEvent->stText[pEvent->ulStrCount].ucStr = IPEPG_StrDup(pszString);
				pEvent->stText[pEvent->ulStrCount].usSize= HxSTD_StrLen(pszString);
				pEvent->ulStrCount++;

	//			HxLOG_Print("INDEX[%d] STRING[%s]\n",pEvent->ulStrCount, pEvent->stText[pEvent->ulStrCount-1].ucStr);
			}
			break;
		case FVC_ELEM_EVENT_PI_GUIDANCE_EXPLANATORY_TEXT:
			pEvent->ext.ukdtt.stGuidance.eType = eDxIPEPG_UKDTT_TEXT_GUIDANCE;
			pEvent->ext.ukdtt.stGuidance.ucStr = IPEPG_StrDup(pszString);
			pEvent->ext.ukdtt.stGuidance.usSize = HxSTD_StrLen(pszString);
			pEvent->ext.ukdtt.usGuidanceCount = 1;
			break;

		case FVC_ELEM_EVENT_PL_ONID:
		case FVC_ELEM_EVENT_PL_SVCID:
			// there is no input of this type
			break;

		case FVC_ELEM_EVENT_PL_PROGRAM_URL:
			{
				HUINT32 ulLengthUrl = 0;
				HUINT16 onId;
				HUINT16 svcId;
				HUINT16 evtId;
				static HUINT16 staticEvtId = 1;

				if(pszString == NULL)
					return FALSE;

				ulLengthUrl = HxSTD_StrLen(pszString);
				if((ulLengthUrl>=IPEPG_URL_MAX_LEN) || (ulLengthUrl == 0))
				{
					HxLOG_Error(" invalid EPGHUMAX_SVC_URL fail!!\n");
					break;
				}
				ipepg_ParseDVBURI(pszString,&onId, NULL, &svcId, &evtId,NULL,NULL);
				evtId = ++staticEvtId;

				pEvent->stBase.usOnId = onId;
				pEvent->stBase.usSvcId = svcId;
				pEvent->stBase.ulUniqueId = evtId;
				{
					HxVector_t	*pstSvcList;
					HINT32		ulSvcLength;
					DxService_t * pstService;

					pstSvcList = APK_META_SVC_FindServiceBySvcId(svcId);
					if (pstSvcList == NULL)
					{
						HxLOG_Debug("there is no matching svc id. sid=(%d) \n",svcId);
						return ERR_FAIL;

					}

					ulSvcLength = HLIB_VECTOR_Length (pstSvcList);
					if ( ulSvcLength <= 0 )
					{
						HxLOG_Debug("???????????? sid=(%d) \n",svcId);
						return ERR_FAIL;
					}

					pstService = (DxService_t*)HLIB_VECTOR_ItemAt(pstSvcList, 0);
					pEvent->stBase.usTsId= pstService->tsid;
					pEvent->stBase.ulSvcUid = pstService->uid;

					HLIB_VECTOR_Delete(pstSvcList);
				}

				//pstTempURLInfo->pucUrl[ulLengthUrl]= '\0';
			}
			break;
		case FVC_ELEM_EVENT_PL_START_TIME:
		case FVC_ELEM_EVENT_PL_OD_STARTOFAVAILABILITY:
		case FVC_ELEM_EVENT_PL_OD_ENDOFAVAILABILITY:
			{
				HxDATETIME_t		stStartTime;
				HUINT8				*pszBackupString = pszString;
				if (ipepg_parseDateTimeField(&pszString, 4, 0, 9999, '-', &stStartTime.stDate.usYear) != ERR_OK
					|| ipepg_parseDateTimeField(&pszString, 2, 1, 12, '-', (HUINT16 *)&stStartTime.stDate.ucMonth) != ERR_OK
					|| ipepg_parseDateTimeField(&pszString, 2, 1, 31, 'T', (HUINT16 *)&stStartTime.stDate.ucDay) != ERR_OK
					|| ipepg_parseDateTimeField(&pszString, 2, 0, 23, ':', (HUINT16 *)&stStartTime.stTime.ucHour) != ERR_OK
					|| ipepg_parseDateTimeField(&pszString, 2, 0, 59, ':', (HUINT16 *)&stStartTime.stTime.ucMinute) != ERR_OK
					|| ipepg_parseDateTimeField(&pszString, 2, 0, 59, 'Z', (HUINT16 *)&stStartTime.stTime.ucSecond) != ERR_OK)
				{
					HxLOG_Error("Invalid format? time=[%s]\n",pszBackupString);
					return ERR_FAIL;
				}
#if defined(IPEPG_TEST_DIFFERENT_TIME)
//	#define STUB_FVC
	#if defined(STUB_FVC)
				{
					HUINT32 ulSystemTime = 0;
					HxDATETIME_t dateTime;
					VK_CLOCK_GetTime((unsigned long*)&ulSystemTime);
					HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSystemTime,&dateTime);
					stStartTime.stDate.usYear	= dateTime.stDate.usYear;
					stStartTime.stDate.ucMonth	= dateTime.stDate.ucMonth;
					stStartTime.stDate.ucDay	= dateTime.stDate.ucDay;
				}
	#else
				{
					HUINT32 ulSystemTime = 0, ulCurTime = 0, ulDifferentTime = 0;
					HxDATETIME_t dateTime;
					VK_CLOCK_GetTime((unsigned long*)&ulSystemTime);
					HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSystemTime,&dateTime);
					dateTime.stTime.ucHour = 0; 	dateTime.stTime.ucMinute = 0;		dateTime.stTime.ucSecond = 0;
					HLIB_DATETIME_ConvertDateTimeToUnixTime (&dateTime, &ulSystemTime);
					dateTime.stDate.usYear = CUR_YEAR;		dateTime.stDate.ucMonth = CUR_MONTH;		dateTime.stDate.ucDay = CUR_DAY;
					HLIB_DATETIME_ConvertDateTimeToUnixTime (&dateTime, &ulCurTime);
					ulDifferentTime = ulCurTime-ulSystemTime;
					ulDecimal = ulDecimal - ulDifferentTime;
				}
	#endif
#endif

				if ((FVC_ELEMENT_e)eDataIdx == FVC_ELEM_EVENT_PL_START_TIME)
				{
					HLIB_DATETIME_ConvertDateTimeToUnixTime(&stStartTime, &pEvent->stBase.ulStartTime);
				}
				else if ((FVC_ELEMENT_e)eDataIdx == FVC_ELEM_EVENT_PL_OD_STARTOFAVAILABILITY)
				{
					HLIB_DATETIME_ConvertDateTimeToUnixTime(&stStartTime, &pEvent->ext.ukdtt.stFvcOnDemand.ulStartOfAvailability);
				}
				else if ((FVC_ELEMENT_e)eDataIdx == FVC_ELEM_EVENT_PL_OD_ENDOFAVAILABILITY)
				{
					HLIB_DATETIME_ConvertDateTimeToUnixTime(&stStartTime, &pEvent->ext.ukdtt.stFvcOnDemand.ulEndOfAvailability);
				}
			}
			break;
		case FVC_ELEM_EVENT_PL_DURATION:
		case FVC_ELEM_EVENT_PL_OD_DURATION:
			{
				HxDATETIME_Time_t		stDuration = {0,};
				UNIXTIME				utDuration;

				pszString += 1;	// designator P
				pszString += 1;	// T
				if (ipepg_parseDateTimeField(&pszString, 2, 0, 23, 'H', (HUINT16 *)&stDuration.ucHour) != ERR_OK)
					ipepg_parseDateTimeField(&pszString, 1, 0, 23, 'H', (HUINT16 *)&stDuration.ucHour);
				if (ipepg_parseDateTimeField(&pszString, 2, 0, 59, 'M', (HUINT16 *)&stDuration.ucMinute)!= ERR_OK)
					ipepg_parseDateTimeField(&pszString, 1, 0, 59, 'M', (HUINT16 *)&stDuration.ucMinute);
				if (ipepg_parseDateTimeField(&pszString, 2, 0, 59, 'S', (HUINT16 *)&stDuration.ucSecond) != ERR_OK)
					ipepg_parseDateTimeField(&pszString, 1, 0, 59, 'S', (HUINT16 *)&stDuration.ucSecond);

				utDuration = stDuration.ucHour*60*60 + stDuration.ucMinute*60 + stDuration.ucSecond;

				if ((FVC_ELEMENT_e)eDataIdx == FVC_ELEM_EVENT_PL_DURATION)
				{
					pEvent->stBase.ulEndTime = pEvent->stBase.ulStartTime + utDuration;
				}
				else if ((FVC_ELEMENT_e)eDataIdx == FVC_ELEM_EVENT_PL_OD_DURATION)
				{
					pEvent->ext.ukdtt.stFvcOnDemand.ulPublishedDuration = utDuration;
				}
			}
			break;
		case FVC_ELEM_EVENT_PL_OD_PROGRAM:
			pEvent->ext.ukdtt.ulOnDemandCount = 1;
			break;
		case FVC_ATTR_EVENT_PL_OD_AUXILIARY_URL_TYPE:
		case FVC_ELEM_EVENT_PL_OD_AUXILIARY_URL:
		case FVC_ATTR_EVENT_PL_OD_PROGRAM_URL_TYPE:
		case FVC_ELEM_EVENT_PL_OD_PROGRAM_URL:
		case FVC_ELEM_EVENT_PL_OD_DELIVERYMODE:
			{
				DxIPEPG_Text_e eFVC_TextType = eDxIPEPG_TEXT_MAX;
				pOnDemand = &(pEvent->ext.ukdtt.stFvcOnDemand);
				ulCount = pOnDemand->usOnDemandStrCount;
				if (ulCount >= eDxIPEPG_FVC_OD_MAXCOUNT)
					break;

				if (eDataIdx == FVC_ATTR_EVENT_PL_OD_AUXILIARY_URL_TYPE) eFVC_TextType = eDxIPEPG_UKDTT_TEXT_OD_AUXILIARY_URL_TYPE;
				else if (eDataIdx == FVC_ELEM_EVENT_PL_OD_AUXILIARY_URL) eFVC_TextType = eDxIPEPG_UKDTT_TEXT_OD_AUXILIARY_URL;
				else if (eDataIdx == FVC_ATTR_EVENT_PL_OD_PROGRAM_URL_TYPE) eFVC_TextType = eDxIPEPG_UKDTT_TEXT_OD_PROGRAM_URL_TYPE;
				else if (eDataIdx == FVC_ELEM_EVENT_PL_OD_PROGRAM_URL) eFVC_TextType = eDxIPEPG_UKDTT_TEXT_OD_PROGRAM_URL;
				else if (eDataIdx == FVC_ELEM_EVENT_PL_OD_DELIVERYMODE) eFVC_TextType = eDxIPEPG_UKDTT_TEXT_OD_DELIVERY_MODE;

				pOnDemand->stOnDemandStr[ulCount].eType = eFVC_TextType;
				pOnDemand->stOnDemandStr[ulCount].ucStr = IPEPG_StrDup(pszString);
				pOnDemand->stOnDemandStr[ulCount].usSize= HxSTD_StrLen(pszString);
				pOnDemand->usOnDemandStrCount++;

				HxLOG_Print("INDEX[%d] STRING[%s]\n",pOnDemand->usOnDemandStrCount, pOnDemand->stOnDemandStr[ulCount].ucStr);
			}
			break;
		case FVC_ATTR_EVENT_PL_OD_FREE_VALUE:
			{
				if (IPEPG_StrCmp("true",pszString) == 0)
				{
					pEvent->ext.ukdtt.stFvcOnDemand.ucFree = 1;
				}
				else
				{
					pEvent->ext.ukdtt.stFvcOnDemand.ucFree = 0;
				}
			}
			break;
		case FVC_ELEM_EVENT_PL_OD_ID_AVATTRIBUTES_VIDEO_HORIZONTAL:
			{
				// OnDemand HD
				if (ulDecimal >= IPEPG_FVC_AVATTRIBUTES_VIDEO_HORIZONTAL_HD)
				{
					pEvent->ext.ukdtt.stFvcOnDemand.ucIsHD = 1;
				}
			}
			break;
		case FVC_ELEM_EVENT_PL_ID_AVATTRIBUTES_VIDEO_HORIZONTAL:
			{
				// HD
				if (ulDecimal >= IPEPG_FVC_AVATTRIBUTES_VIDEO_HORIZONTAL_HD)
				{
					pEvent->ext.ukdtt.eComponent |= eDxIPEPG_UKDTT_COMPONENT_HD;
				}
			}
			break;
		case FVC_ELEM_EVENT_PL_ID_AVATTRIBUTES_AUDIO:
			pEvent->ext.ukdtt.eComponent |= eDxIPEPG_UKDTT_COMPONENT_AUDIO_DESCRIPTION;
			break;

		default :
			break;
	}
	return ERR_OK;
}

static HERROR		local_ipepg_ParseAttribute(HxXMLELEMENT *pElement, FVC_ELEMENT_t *pstElement, void *target, local_ipepg_SetData_t func)
{
	HERROR					hError = ERR_FAIL;
	HUINT32					ulIndex = 0;
	HUINT8					* pszAttrContents = NULL;
	FVC_ATTR_t 		* attributeList = NULL;

	IPEPG_RETURN_IF(pElement == NULL || pstElement == NULL || target == NULL, ERR_FAIL);
	if(pstElement->AttributeList == NULL)	return ERR_FAIL;

	attributeList = pstElement->AttributeList;
	do
	{
		hError = HLIB_XML_GetAttrContents(pElement, attributeList[ulIndex].pucAttrName, &pszAttrContents);
		if(hError == ERR_OK)
		{
			HUINT32 ulValue = 0;
			HUINT8	szText[EPG_HUMAX_NAME_LEN + 1] = {0, };				// ?. If over 50, truncated.
			HUINT32 ulRealLen = 0;

			hError = ipepg_SetDataByType(attributeList[ulIndex].ulAttrDataType, pszAttrContents, EPG_HUMAX_NAME_LEN, szText, &ulValue, &ulRealLen);

			if(func)
			{
				hError = func(target, attributeList[ulIndex].eDataIdx,ulRealLen, szText, ulValue);
			}
		}

		if(pszAttrContents)	HLIB_XML_FreeAttrContents(pszAttrContents);
		pszAttrContents = NULL;
	}
	while(!(attributeList[ulIndex++].bFlagEnd));
	return ERR_OK;
}

static HERROR		local_ipepg_ParseElement(HxXMLDOC hDocHandle, HxXMLELEMENT pElement, FVC_ELEMENT_t *pstElement, void *target, local_ipepg_SetData_t func)
{
	FVC_ELEMENT_t *pstSubElement = NULL;
	HxList_t		*pChildList = NULL;
	HxList_t		*pBackupChildList = NULL;
	HERROR			hError = ERR_OK;

	IPEPG_RETURN_IF(pElement == NULL || pstElement == NULL || target == NULL, ERR_FAIL);

	local_ipepg_ParseAttribute(pElement, pstElement, target, func);
	if(HLIB_XML_GetChildElementList(pElement, &pChildList) == ERR_FAIL)
	{
		if(pChildList) HLIB_LIST_RemoveAll(pChildList);
		return ERR_OK;
	}
	pstSubElement = pstElement->ElementList;

	pBackupChildList = pChildList;

	while(pstSubElement != NULL && pChildList != NULL)
	{
		HxXMLELEMENT		hChildElement = NULL;
		FVC_ELEMENT_t *pstSameElement = NULL;
		HUINT8			*pucElementName = NULL;
		HUINT32			ulIndex= 0;

		hChildElement = (HxXMLELEMENT)HLIB_LIST_Data(pChildList);

		IPEPG_GOTO_IF(hChildElement == NULL,EXIT_EVENT_LOOP);
		IPEPG_GOTO_IF(HLIB_XML_GetElementName(hChildElement, &pucElementName) == ERR_FAIL, EXIT_EVENT_LOOP);
		IPEPG_GOTO_IF(pucElementName == NULL, EXIT_EVENT_LOOP);

		do
		{
			//HxLOG_Print("COMPARE [%s] [%d] %s, %s\n",__FUNCTION__, __LINE__, pstSubElement[ulIndex].pucTagName, pucElementName);
			if(!strcmp(pstSubElement[ulIndex].pucTagName, pucElementName))
			{
				pstSameElement = &pstSubElement[ulIndex];
				break;
			}
		}while(!(pstSubElement[ulIndex++].bFlagEnd));

		if (pstSameElement == NULL)
		{
			goto EXIT_EVENT_LOOP;
		}

		if(pstSameElement->ulTagDataType == FVC_TYPE_NONE)
		{
			hError |= func(target, pstSameElement->eDataIdx, 0, NULL, 0);
			hError |= local_ipepg_ParseElement(hDocHandle, hChildElement, pstSameElement, target, func);
		}
		else
		{
			HUINT32 ulValue = 0;
			HUINT8	szText[EPG_HUMAX_DETAIL_LEN + 1] = {0, };
			HUINT8			*pContents= NULL;
			HUINT32 ulRealLen = 0;
			if(pstSameElement->AttributeList) local_ipepg_ParseAttribute(hChildElement, pstSameElement, target, func);

			if(HLIB_XML_GetElementContents(hDocHandle, hChildElement, &pContents) == ERR_FAIL)
			{
				if(pContents) HLIB_XML_FreeAttrContents(pContents);
				goto EXIT_EVENT_LOOP;
			}
			hError |= ipepg_SetDataByType(pstSameElement->ulTagDataType, pContents, EPG_HUMAX_DETAIL_LEN, szText, &ulValue, &ulRealLen);
			if(func)
			{
				hError |= func(target, pstSameElement->eDataIdx,ulRealLen, szText, ulValue);
			}
			if(pContents) HLIB_XML_FreeAttrContents(pContents);

		}

EXIT_EVENT_LOOP :
		pChildList = pChildList->next;
	}
	HLIB_LIST_RemoveAll(pBackupChildList);

	return hError;
}

HINT32		IPEPG_ParseHeaderFieldValue (HCHAR *szValue, HCHAR *szHeader, HCHAR *szName, HINT32 nLimit)
{
	return ipepg_ParseHeaderFieldValue(szValue, szHeader, szName, nLimit);
}


HERROR		IPEPG_FvcParseHeader(HUINT8 *pRawBuffer, DxFvcExpiry_t *pstFvcExpiry)
{
	HCHAR 		pucTempValue[FVC_EXPIRY_DATE_LENGTH] = {0,};
	HCHAR		*pucTemp = NULL;
	HINT32		nLen;
	UNIXTIME	utCurrTime=0;
	HINT32		nMaxAge=0;
	HERROR 		hResult = ERR_FAIL;

	if (pRawBuffer == NULL || pstFvcExpiry == NULL)
	{
		return ERR_FAIL;
	}

	// parser statusCode - HTTP-Version SP Status-Code SP Reason-Phrase CRLF
	pucTemp = HxSTD_StrChar(pRawBuffer, ' ');
	if (pucTemp != NULL)
	{
		++pucTemp;
		HxSTD_snprintf(pucTempValue,4,"%s",pucTemp);
		pstFvcExpiry->statusCode = HLIB_STD_StrToUINT32(pucTempValue);
		if (pstFvcExpiry->statusCode == IPEPG_FVC_STATUS_CODE_BAD
			|| pstFvcExpiry->statusCode == 403
			|| pstFvcExpiry->statusCode == 404
			|| pstFvcExpiry->statusCode == 406)
		{
			// 400, 403, 404, 406은 DB에 모두 400으로 저장한다 (db query시에 400 타입 체크 하나만 하기 위함)
			pstFvcExpiry->statusCode == IPEPG_FVC_STATUS_CODE_BAD;
		}
		HxLOG_Info(HxANSI_COLOR_BLUE(" parsed return code [%s](%d) \n"),pucTempValue,pstFvcExpiry->statusCode);
	}

	nLen = ipepg_ParseHeaderFieldValue(pucTempValue, pRawBuffer, IPEPG_FVC_HEADER_LAST_MODIFIED, IPEPG_HEADER_MAX_SIZE-10);
	IPEPG_GOTO_IF(nLen == -1,END_FVC_PARSER_HEADER);

	HxLOG_Info(HxANSI_COLOR_BLUE(" parsed modified :[%s] \n"), pucTempValue);
	HxSTD_snprintf(pstFvcExpiry->lastModifiedTime,FVC_EXPIRY_DATE_LENGTH,"%s",pucTempValue);

	// FVC B2C API spec 0.94에서는 Expires를 사용하여서 DxFvcExpiry_t라는 이름을 사용하였으나,
	// FVC B2C API spec 0.98에서 Cache-Control로 바뀌어 버렸다.
	// max-age=21600, public
	nLen = ipepg_ParseHeaderFieldValue(pucTempValue, pRawBuffer, IPEPG_FVC_HEADER_CACHE_CONTROL, IPEPG_HEADER_MAX_SIZE-10);
	IPEPG_GOTO_IF(nLen == -1,END_FVC_PARSER_HEADER);
	HxLOG_Info(HxANSI_COLOR_BLUE(" parsed cache control :[%s]  \n"), pucTempValue);

	sscanf(pucTempValue, "max-age=%d",&nMaxAge);
	if ( nMaxAge < 0 || nMaxAge > 25*60*60)
	{
		// default & 한계 를 넣어야 한다. (devices SHOULD NOT wait indefinitely before attempting to refresh the schedule.)
		HxLOG_Error("invalid max age. got Max age=(%d) \n",nMaxAge);
		nMaxAge = 24*60*60;
	}

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

	pstFvcExpiry->expiryTime = utCurrTime + (HUINT32)nMaxAge;

	hResult = ERR_OK;
END_FVC_PARSER_HEADER:
	return ERR_OK;
}

HERROR		IPEPG_FvcParseServiceList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ListOfSvcInfo)
{
	HERROR					hError = ERR_FAIL;
	HERROR					hResult = ERR_FAIL;
	HxXMLDOC				hXmlDocHandle = NULL;
	FVC_ELEMENT_t			*pstElement = NULL;
	HxXMLELEMENT			hRootElement = NULL;
	HxXMLELEMENT			hProgrammeDescriptionElement = NULL;
	HxXMLELEMENT			hSIT = NULL;
	HxXMLELEMENT			hServiceInformation = NULL;
	HxList_t				*pProgrammeDescription = NULL;
	HxList_t				*pSIT = NULL;
	HxList_t				*pServiceInformation = NULL;
	HxList_t				*pBackupServiceInformation = NULL;

	HxList_t				*pTempSvcList = NULL;


	IPEPG_GOTO_IF(pRawBuffer == NULL, EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(ListOfSvcInfo == NULL, EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(ulBufferSize == 0, EXIT_EVENT_FUNC);

	*ListOfSvcInfo = NULL;

	// GET PASER DOC
	IPEPG_GOTO_IF(ipepg_LoadSvcElementList(&pstElement) == ERR_FAIL, EXIT_EVENT_FUNC);

	if(pstElement && pstElement->pucTagName)
	{
		if(HxSTD_StrStr(pRawBuffer, pstElement->pucTagName) == NULL)
		{
#if defined(CONFIG_DEBUG)
			HxLOG_Error("SVCLIST XML : %s\n", pRawBuffer);
#endif
			goto EXIT_EVENT_FUNC;
		}
	}

	// INIT XML PARSER
	IPEPG_GOTO_IF(HLIB_XML_InitParser() == ERR_FAIL, EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_ParseBuffer(pRawBuffer, ulBufferSize, &hXmlDocHandle) == ERR_FAIL, EXIT_EVENT_FUNC);

	// GET Element TVAMain
	IPEPG_GOTO_IF(pstElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_GetRootElement(hXmlDocHandle, &hRootElement) == ERR_FAIL,  EXIT_EVENT_FUNC);

	// GET Element ProgramDescription
	pstElement = pstElement->ElementList;
	IPEPG_GOTO_IF(pstElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_FindChildElementByName(hRootElement, pstElement->pucTagName, &pProgrammeDescription) == ERR_FAIL, EXIT_EVENT_FUNC);
	hProgrammeDescriptionElement =(HxXMLELEMENT)HLIB_LIST_Data(pProgrammeDescription);

	// GET Element ServiceInformationTable
	pstElement = pstElement->ElementList;
	IPEPG_GOTO_IF(pstElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_FindChildElementByName(hProgrammeDescriptionElement, pstElement->pucTagName, &pSIT) == ERR_FAIL, EXIT_EVENT_FUNC);
	hSIT =(HxXMLELEMENT)HLIB_LIST_Data(pSIT);

	// GET Element ServiceInformation
	pstElement = pstElement->ElementList;
	IPEPG_GOTO_IF(pstElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_FindChildElementByName(hSIT, pstElement->pucTagName, &pServiceInformation) == ERR_FAIL, EXIT_EVENT_FUNC);

	 pBackupServiceInformation = pServiceInformation;
	// EPG_URL_INFO_t
	while (pServiceInformation != NULL)
	{
		EPG_URL_INFO_t * pIpSvcInfo = NULL;
		hServiceInformation = (HxXMLELEMENT)HLIB_LIST_Data(pServiceInformation);
		//HxLOG_Debug(" tagname [%s]\n", pstElement->pucTagName);

		if (hServiceInformation != NULL)
		{
			pIpSvcInfo = (EPG_URL_INFO_t*)IPEPG_MemAlloc(sizeof(EPG_URL_INFO_t));
			IPEPG_GOTO_IF(pIpSvcInfo == NULL, EXIT_SVC_LOOP);
			HxSTD_MemSet(pIpSvcInfo, 0x00, sizeof(EPG_URL_INFO_t));
			hError = local_ipepg_ParseElement(hXmlDocHandle, hServiceInformation, pstElement, pIpSvcInfo, ipepg_Fvc_SetServiceData);
			IPEPG_GOTO_IF(hError != ERR_OK, EXIT_SVC_LOOP);
		}
EXIT_SVC_LOOP :

		if (pIpSvcInfo != NULL)
		{
			if (HLIB_LIST_FindEx(pTempSvcList, (void *)&(pIpSvcInfo->stTripleId), ipepg_CompareTripleId) == NULL
				&& hError == ERR_OK)
			{
				pTempSvcList = HLIB_LIST_Append(pTempSvcList, pIpSvcInfo);
			}
			else
			{
				IPEPG_MemFree(pIpSvcInfo);
			}
		}
		pServiceInformation = pServiceInformation->next;
	}


	hResult = ERR_OK;

EXIT_EVENT_FUNC :
	if (pSIT) HLIB_LIST_RemoveAll(pSIT);
	if (pBackupServiceInformation) HLIB_LIST_RemoveAll(pBackupServiceInformation);
	if (pProgrammeDescription) HLIB_LIST_RemoveAll(pProgrammeDescription);

	if(hXmlDocHandle != NULL)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}

	if(pTempSvcList != NULL)
	{
		*ListOfSvcInfo = pTempSvcList;
	}

	return hResult;
}





HERROR		IPEPG_FvcParseEventList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ppEventList, HINT32 ulSvcUid)
{
	HERROR					hError = ERR_FAIL;
	HERROR					hResult = ERR_FAIL;
	HxXMLDOC				hXmlDocHandle = NULL;
	FVC_ELEMENT_t			*pstElement = NULL;
	FVC_ATTR_t 				*pstAttribute = NULL;

	HxXMLELEMENT			hTvaMain = NULL;

	HxXMLELEMENT			hProgramDescription = NULL;
	HxList_t				*pProgramDescription = NULL;

	FVC_ELEMENT_t			*pstPITElement = NULL;
	HxXMLELEMENT			hPIT = NULL;
	HxList_t				*pPIT = NULL;

	FVC_ELEMENT_t			*pstPLTElement = NULL;
	HxXMLELEMENT			hPLT = NULL;
	HxList_t				*pPLT = NULL;

	HxXMLELEMENT			hProgramInformation = NULL;
	HxList_t				*pBackupProgramInformation = NULL;
	HxList_t				*pProgramInformation = NULL;

	FVC_ELEMENT_t			*pstScheduleElement = NULL;
	HxXMLELEMENT			hSchedule = NULL;
	HxList_t				*pBackupSchedule = NULL;
	HxList_t				*pSchedule = NULL;

	FVC_ELEMENT_t			*pstScheduleEventElement = NULL;
	HxXMLELEMENT			hScheduleEvent = NULL;
	HxList_t				*pBackupScheduleEvent = NULL;
	HxList_t				*pScheduleEvent = NULL;

	FVC_ELEMENT_t			*pstOnDemandElement = NULL;
	HxXMLELEMENT			hOnDemand = NULL;
	HxList_t				*pBackupOnDemand = NULL;
	HxList_t				*pOnDemand = NULL;

	HxXMLELEMENT			hOnDemandProgram = NULL;


	HxHASH_t				*pstPITHash = NULL;
	HxHASH_t				*pstPLTHash = NULL;

	HxList_t				*pTempEventList = NULL;



	IPEPG_GOTO_IF(pRawBuffer == NULL || ppEventList == NULL || ulBufferSize == 0,EXIT_EVENT_FUNC);
	*ppEventList = NULL;

	// GET PASER DOC
	IPEPG_GOTO_IF(ipepg_LoadEventElementList(&pstElement) == ERR_FAIL, EXIT_EVENT_FUNC);

	if(pstElement && pstElement->pucTagName)
	{
		if(HxSTD_StrStr(pRawBuffer, pstElement->pucTagName) == NULL)
		{
			HxLOG_Error("EVENT XML : %s\n", pRawBuffer);
			goto EXIT_EVENT_FUNC;
		}
	}

	// INIT XML PARSER
	IPEPG_GOTO_IF(HLIB_XML_InitParser() == ERR_FAIL, EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_ParseBuffer(pRawBuffer, ulBufferSize, &hXmlDocHandle) == ERR_FAIL, EXIT_EVENT_FUNC);


	// GET Element TvaMain
	IPEPG_GOTO_IF(pstElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_GetRootElement(hXmlDocHandle, &hTvaMain) == ERR_FAIL,  EXIT_EVENT_FUNC);
	TEST_Function(__LINE__, pstElement->pucTagName, hTvaMain);


	// GET Element Program Description
	pstElement = pstElement->ElementList;
	IPEPG_GOTO_IF(pstElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_FindChildElementByName(hTvaMain, pstElement->pucTagName, &pProgramDescription) == ERR_FAIL, EXIT_EVENT_FUNC);
	hProgramDescription =(HxXMLELEMENT)HLIB_LIST_Data(pProgramDescription);
	TEST_Function(__LINE__, pstElement->pucTagName, hProgramDescription);


	// GET Element Program Information Table
	pstPITElement = pstElement->ElementList;
	IPEPG_GOTO_IF(pstPITElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_FindChildElementByName(hProgramDescription, pstPITElement->pucTagName, &pPIT) == ERR_FAIL, EXIT_EVENT_FUNC);
	hPIT =(HxXMLELEMENT)HLIB_LIST_Data(pPIT);
	TEST_Function(__LINE__, pstPITElement->pucTagName, hPIT);

	// GET Element Program Information
	pstPITElement = pstPITElement->ElementList;
	IPEPG_GOTO_IF(pstPITElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF_NO_PRINT(HLIB_XML_FindChildElementByName(hPIT, pstPITElement->pucTagName, &pProgramInformation) == ERR_FAIL, EXIT_EVENT_FUNC);
	hProgramInformation =(HxXMLELEMENT)HLIB_LIST_Data(pProgramInformation);
	TEST_Function(__LINE__, pstPITElement->pucTagName, hProgramInformation);

	// FOREACH program information, add its crid to hash
	pstPITHash = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, NULL, HLIB_STD_MemFree_CB);
	pBackupProgramInformation = pProgramInformation;
	while (pProgramInformation != NULL)
	{
		FVC_PROGRAM_INFORMATION_t			*pstProgInfo = NULL;
		HUINT8								*pszAttrContents = NULL;
		HUINT32								ulRealLen;
		hProgramInformation = (HxXMLELEMENT)HLIB_LIST_Data(pProgramInformation);

		if (hProgramInformation != NULL)
		{
			pstProgInfo = (FVC_PROGRAM_INFORMATION_t *)IPEPG_MemAlloc(sizeof(FVC_PROGRAM_INFORMATION_t));

			//  1. parse prog crid
			pstAttribute = pstPITElement->AttributeList;
			// GET Atrribute HSVC

			hError = HLIB_XML_GetAttrContents(hProgramInformation, pstAttribute[0].pucAttrName, &pszAttrContents);
			if (hError != ERR_OK)
			{
				HxLOG_Error("Fail to find attr name=[%s] \n",pstAttribute[0].pucAttrName);
				// ????
			}

			// FVC에서 crid라고 해놓고는 crid://eventis.nl/00000000-0000-1000-0404-000000058FD0,imi:0010000000070599 와 같이 지나 치게 긴 crid를 주므로  *2를 해준다.(crid_len*2)
			hError = ipepg_SetDataByType(pstAttribute[0].ulAttrDataType, pszAttrContents, DxCRID_LEN*2, pstProgInfo->szProgCrid,NULL, &ulRealLen);
			if (hError != ERR_OK)
			{
				HxLOG_Error("Fail to set prog crid \n");
				// ????
			}

			HLIB_XML_FreeAttrContents(pszAttrContents);


			// 2. assign prog crid to pstProgInfo
			pstProgInfo->phElement = (void *)hProgramInformation;

			// 3. hash.add(pstProgInfo)
			HLIB_HASH_Insert(pstPITHash, (void *)pstProgInfo->szProgCrid, pstProgInfo);
		}
		pProgramInformation = pProgramInformation->next;
	}

//	pstElement += sizeof(FVC_ELEMENT_t);
	// TODO: 수정할 것
	pstPLTElement = (FVC_ELEMENT_t *)&s_astElemEvtProgramDescription[1];

	// GET Element Program Location Table
	IPEPG_GOTO_IF(pstPLTElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_FindChildElementByName(hProgramDescription, pstPLTElement->pucTagName, &pPLT) == ERR_FAIL, EXIT_EVENT_FUNC);
	hPLT =(HxXMLELEMENT)HLIB_LIST_Data(pPLT);
	TEST_Function(__LINE__, pstPLTElement->pucTagName, hPLT);

	// GET Element Schedule
	pstScheduleElement = pstPLTElement->ElementList;
	IPEPG_GOTO_IF(pstScheduleElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_FindChildElementByName(hPLT, pstScheduleElement->pucTagName, &pSchedule) == ERR_FAIL, EXIT_EVENT_FUNC);

	pstPLTHash = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, NULL, NULL);
	pBackupSchedule = pSchedule;
	while (pSchedule != NULL)
	{
		hSchedule = (HxXMLELEMENT)HLIB_LIST_Data(pSchedule);
//		HxLOG_Debug(" tagname [%s]\n", pstScheduleElement->pucTagName);
		TEST_Function(__LINE__, pstScheduleElement->pucTagName, hSchedule);

		// GET Element ScheduleEvent
		pstScheduleEventElement = pstScheduleElement->ElementList;
		IPEPG_GOTO_IF(pstScheduleEventElement == NULL , EXIT_EVENT_FUNC);
		IPEPG_GOTO_IF_NO_PRINT(HLIB_XML_FindChildElementByName(hSchedule, pstScheduleEventElement->pucTagName, &pScheduleEvent) == ERR_FAIL, EXIT_SCHEDULE_LOOP);
		hScheduleEvent =(HxXMLELEMENT)HLIB_LIST_Data(pScheduleEvent);
		TEST_Function(__LINE__, pstScheduleEventElement->pucTagName, hScheduleEvent);

		pBackupScheduleEvent = pScheduleEvent;
		while (pScheduleEvent != NULL)
		{
			IPEPG_UpdateEvent_t * UpdateEvent = NULL;
			DxIpEvent_t * pIpEvent = NULL;
			FVC_PROGRAM_INFORMATION_t	*pstProgramInformation;

			hError = ERR_FAIL;
			hScheduleEvent = (HxXMLELEMENT)HLIB_LIST_Data(pScheduleEvent);

			if (hScheduleEvent != NULL)
			{
				UpdateEvent = (IPEPG_UpdateEvent_t*)IPEPG_MemCalloc(sizeof(IPEPG_UpdateEvent_t));
				pIpEvent = (DxIpEvent_t*)IPEPG_MemCalloc(sizeof(DxIpEvent_t));

				IPEPG_GOTO_IF(UpdateEvent == NULL || pIpEvent == NULL, EXIT_SCHEDULE_PROGRAM_LOOP);
				UpdateEvent->pIpEvent = pIpEvent;
				hError = local_ipepg_ParseElement(hXmlDocHandle, hScheduleEvent, pstScheduleEventElement, pIpEvent, local_ipepg_SetEventData);
				IPEPG_GOTO_IF(hError != ERR_OK, EXIT_SCHEDULE_PROGRAM_LOOP);

				// 1. get hash using crid
				pstProgramInformation = HLIB_HASH_Lookup(pstPITHash,(const void*)pIpEvent->ext.ukdtt.stProgramId.ucStr);
				if (pstProgramInformation == NULL)
				{
					HxLOG_Error("fail to find PI. program id = [%s] \n", pIpEvent->ext.ukdtt.stProgramId.ucStr);
					if(UpdateEvent != NULL)
						IPEPG_MemFree(UpdateEvent);
					if(pIpEvent != NULL)
						IPEPG_MemFree(pIpEvent);
					UpdateEvent = NULL;
					pIpEvent = NULL;
					goto EXIT_SCHEDULE_PROGRAM_LOOP;
				}

				// 2. parse element from hash handle
				hError = local_ipepg_ParseElement(hXmlDocHandle, pstProgramInformation->phElement, (FVC_ELEMENT_t *)&s_astElemEvtPit, pIpEvent, local_ipepg_SetEventData);
				IPEPG_GOTO_IF(hError != ERR_OK, EXIT_SCHEDULE_PROGRAM_LOOP);

				// 3.  add update event to hash
				HLIB_HASH_Insert(pstPLTHash, (void *)pIpEvent->ext.ukdtt.stProgramId.ucStr, pIpEvent);
			}
		EXIT_SCHEDULE_PROGRAM_LOOP :
			if(UpdateEvent != NULL && pIpEvent != NULL && hError == ERR_OK)
			{
				pIpEvent->stBase.type = eDxEPG_TYPE_IP;
				UpdateEvent->eUpdateType = eIPEPG_EVENT_TYPE_UPDATE;
				pTempEventList = HLIB_LIST_Append(pTempEventList, UpdateEvent);
			}
			else
			{
				if (UpdateEvent != NULL)
				{
					IPEPG_MemFree(UpdateEvent);
					UpdateEvent = NULL;
				}

				if (pIpEvent != NULL)
				{
					IPEPG_MemFreeIpEvent(pIpEvent);
					pIpEvent = NULL;
				}
			}

			pScheduleEvent = pScheduleEvent->next;
		}
	EXIT_SCHEDULE_LOOP :
		if (pBackupScheduleEvent) HLIB_LIST_RemoveAll(pBackupScheduleEvent);
		pBackupScheduleEvent = NULL;
		pSchedule = pSchedule->next;
	}

	// onDemand는 optional이므로, schedule만 제대로 받을 경우 parse OK
	hResult = ERR_OK;

	// GET Element OnDemand
	// TODO: 수정할 것
	pstOnDemandElement = (FVC_ELEMENT_t *)&s_astElemEvtPlt[1];
	IPEPG_GOTO_IF(pstScheduleElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF_NO_PRINT(HLIB_XML_FindChildElementByName(hPLT, pstOnDemandElement->pucTagName, &pOnDemand) == ERR_FAIL, EXIT_EVENT_FUNC);

	pBackupOnDemand = pOnDemand;
	while (pOnDemand != NULL)
	{
		HUINT8			szProgCrid[DxCRID_LEN+10] = {0,};
		HUINT32			ulRealLen;
		DxIpEvent_t 	*pIpEvent = NULL;
		HxList_t		*pOnDemandProgram = NULL;
		HUINT8			*pszAttrContents = NULL;

		hOnDemand = (HxXMLELEMENT)HLIB_LIST_Data(pOnDemand);
		if (hOnDemand != NULL)
		{
			pOnDemandProgram = NULL;
			pszAttrContents = NULL;

			hError = HLIB_XML_FindChildElementByName(hOnDemand, pstOnDemandElement->ElementList->pucTagName, &pOnDemandProgram);
			IPEPG_GOTO_IF(hError != ERR_OK, EXIT_ON_DEMAND_LOOP);

			hOnDemandProgram = (HxXMLELEMENT)HLIB_LIST_Data(pOnDemandProgram);
			hError = HLIB_XML_GetAttrContents(hOnDemandProgram, pstOnDemandElement->ElementList->AttributeList[0].pucAttrName, &pszAttrContents);
			IPEPG_GOTO_IF(hError != ERR_OK, EXIT_ON_DEMAND_LOOP);

			hError = ipepg_SetDataByType(pstOnDemandElement->ElementList->AttributeList[0].ulAttrDataType, pszAttrContents, DxCRID_LEN, szProgCrid,NULL, &ulRealLen);
			IPEPG_GOTO_IF(hError != ERR_OK, EXIT_ON_DEMAND_LOOP);



			// 1. get hash using crid
			pIpEvent = HLIB_HASH_Lookup(pstPLTHash,(const void*)szProgCrid);
			IPEPG_GOTO_IF(pIpEvent == NULL, EXIT_ON_DEMAND_LOOP);

			// 2. parse element from hash handle
			hError = local_ipepg_ParseElement(hXmlDocHandle, hOnDemand, pstOnDemandElement, pIpEvent, local_ipepg_SetEventData);
			IPEPG_GOTO_IF(hError != ERR_OK, EXIT_ON_DEMAND_LOOP);

			EXIT_ON_DEMAND_LOOP :
			if (pOnDemandProgram) {HLIB_LIST_RemoveAll(pOnDemandProgram);}
			if (pszAttrContents) {HLIB_XML_FreeAttrContents(pszAttrContents);}

		}
		pOnDemand = pOnDemand->next;
	}

	hResult = ERR_OK;
EXIT_EVENT_FUNC :
	if (pProgramDescription) HLIB_LIST_RemoveAll(pProgramDescription);
	if (pPIT) HLIB_LIST_RemoveAll(pPIT);
	if (pPLT) HLIB_LIST_RemoveAll(pPLT);
	if (pBackupProgramInformation) HLIB_LIST_RemoveAll(pBackupProgramInformation);
	if (pBackupSchedule) HLIB_LIST_RemoveAll(pBackupSchedule);
	if (pBackupOnDemand) HLIB_LIST_RemoveAll(pBackupOnDemand);
	if (pstPITHash) HLIB_HASH_Destroy(pstPITHash);
	if (pstPLTHash) HLIB_HASH_Destroy(pstPLTHash);
	if (hXmlDocHandle != NULL)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}

	*ppEventList = pTempEventList;
	return hResult;
}
/*********************** End of File ******************************/


