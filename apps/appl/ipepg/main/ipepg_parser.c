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

//////////////////////////////////////////////////////////////////////////////////// XML
typedef	enum
{
	IPEPG_ELEMENT_DEPTH_ROOT 					= 0,
	IPEPG_ELEMENT_DEPTH_1,
	IPEPG_ELEMENT_DEPTH_2,
	IPEPG_ELEMENT_DEPTH_3,
	IPEPG_ELEMENT_DEPTH_4,
	IPEPG_ELEMENT_DEPTH_5,
	IPEPG_ELEMENT_DEPTH_6,
	IPEPG_ELEMENT_DEPTH_7,
	IPEPG_ELEMENT_DEPTH_8,
	IPEPG_ELEMENT_DEPTH_9,
	IPEPG_ELEMENT_DEPTH_10
}IPEPG_ELEMENT_DEPTH_e;


typedef	enum
{
	IPEPG_TYPE_NONE 							= 0,
	IPEPG_TYPE_STRING,
	IPEPG_TYPE_DECIMAL,
	IPEPG_TYPE_DECIMAL64,
	IPEPG_TYPE_HEXA

}IPEPG_ELEMENTDATA_TYPE_e;




typedef enum
{
	IPEPG_STATION_RESPONSE = 0,
	IPEPG_STATION_SVCLIST,
	IPEPG_STATION_SVCDATA,
	IPEPG_SVC_NAME ,
	IPEPG_SVC_SUPPORTMEDIA ,
	IPEPG_SVC_ONID,
	IPEPG_SVC_TSID,
	IPEPG_SVC_SVCID,
	IPEPG_SVC_SVCHANDLE,
	IPEPG_SVC_URL,
	IPEPG_SVC_THUMBNAIL_URL,
	IPEPG_STATIONID,
	IPEPG_LANG,
	IPEPG_VERSION,
	IPEPG_SVC_TRIPLEID,
	IPEPG_SVC_TRIPLEIDS,
	IPEPG_SVC_THUMBNAIL_URLS,
	IPEPG_SVC_THUMBNAIL_SIZE,
	IPEPG_SVC_THUMBNAIL_TYPE,
	IPEPG_SVC_MAX
}IPEPG_TAG_e;

typedef enum
{
	IPEPG_ELEM_EVENT_IPEPG = 0,
	IPEPG_ELEM_CHANNELS,
	IPEPG_ELEM_CHANNEL,
	IPEPG_ATTR_CHANNEL_HSVC,
	IPEPG_ATTR_CHANNEL_ID,
	IPEPG_ELEM_CHANNEL_NUM,
	IPEPG_ELEM_CHANNEL_NAME,
	IPEPG_ELEM_CHANNEL_TRIPLEIDS,
	IPEPG_ELEM_CHANNEL_TRIPLEID,
	IPEPG_ATTR_CHANNEL_TRIPLE_ONID,
	IPEPG_ATTR_CHANNEL_TRIPLE_TSID,
	IPEPG_ATTR_CHANNEL_TRIPLE_SVCID,

	IPEPG_ELEM_CHANNEL_IMAGES,
	IPEPG_ELEM_CHANNEL_IMAGE,
	IPEPG_ATTR_CHANNEL_IMAGESIZE,
	IPEPG_ATTR_CHANNEL_IMAGETYPE,

	IPEPG_ELEM_CHANNEL_UPDATE,
	IPEPG_ATTR_CHANNEL_UPDATETYPE,

	IPEPG_ELEM_PROGRAMS,
	IPEPG_ELEM_PROGRAM,
	IPEPG_ATTR_PROGRAM_ID,
	IPEPG_ATTR_PROGRAM_STARTTIME,
	IPEPG_ATTR_PROGRAM_ENDTIME,


	IPEPG_ELEM_PROGRAM_NAME,
	IPEPG_ATTR_PROGRAM_NAME,
	IPEPG_ELEM_PROGRAM_SUBNAME,
	IPEPG_ATTR_PROGRAM_SUBNAME,
	IPEPG_ELEM_PROGRAM_SHORTTEXT,
	IPEPG_ATTR_PROGRAM_SHORTTEXT,
	IPEPG_ELEM_PROGRAM_LONGTEXT,
	IPEPG_ATTR_PROGRAM_LONGTEXT,

	IPEPG_ELEM_PROGRAM_RECOMMENDATION,
	IPEPG_ELEM_PROGRAM_RATING,

	IPEPG_ELEM_PROGRAM_IMAGES,
	IPEPG_ELEM_PROGRAM_IMAGE,
	IPEPG_ATTR_PROGRAM_IMAGE_SIZE,
	IPEPG_ATTR_PROGRAM_IMAGE_TYPE,

	IPEPG_ELEM_PROGRAM_PERSONS,
	IPEPG_ELEM_PROGRAM_PERSON,
	IPEPG_ATTR_PROGRAM_PERSON_TYPE,

	IPEPG_ELEM_PROGRAM_GENRES,
	IPEPG_ELEM_PROGRAM_GENRE,
	IPEPG_ATTR_PROGRAM_GENRE_CODE1,
	IPEPG_ATTR_PROGRAM_GENRE_CODE2,


	IPEPG_ELEM_PROGRAM_CODECS,
	IPEPG_ELEM_PROGRAM_CODEC,
	IPEPG_ATTR_PROGRAM_CODEC_TYPE,

	IPEPG_ELEM_PROGRAM_SERIES,
	IPEPG_ATTR_PROGRAM_SERIES_ID,
	IPEPG_ATTR_PROGRAM_SERIES_COUNT,
	IPEPG_ATTR_PROGRAM_SERIES_EPISODE,
	IPEPG_ATTR_PROGRAM_SERIES_SEASON,
	IPEPG_ELEM_PROGRAM_SERIESLINKS,
	IPEPG_ELEM_PROGRAM_SERIESLINK,
	IPEPG_ATTR_PROGRAM_SERIESLINK_PROGID,
	IPEPG_ATTR_PROGRAM_SERIESLINK_STARTTIME,
	IPEPG_ATTR_PROGRAM_SERIESLINK_ENDTIME,
	IPEPG_ATTR_PROGRAM_SERIESLINK_EPISODE,
	IPEPG_ATTR_PROGRAM_SERIESLINK_SEASON,
	IPEPG_ELEM_PROGRAM_SERIESLINK_NAME,
	IPEPG_ATTR_PROGRAM_SERIESLINK_NAME,

	IPEPG_ELEM_PROGRAM_MEDIAS,
	IPEPG_ELEM_PROGRAM_MEDIA,
	IPEPG_ATTR_PROGRAM_MEDIA_ID,
	IPEPG_ATTR_PROGRAM_MEDIA_TYPE,
	IPEPG_ATTR_PROGRAM_MEDIA_TARGET,
	IPEPG_ELEM_PROGRAM_MEDIA_URLS,
	IPEPG_ELEM_PROGRAM_MEDIA_URL,

	IPEPG_ELEM_PROGRAM_MEDIA_IMAGES,
	IPEPG_ELEM_PROGRAM_MEDIA_IMAGE,
	IPEPG_ATTR_PROGRAM_MEDIA_IMAGE_SIZE,
	IPEPG_ATTR_PROGRAM_MEDIA_IMAGE_TYPE,

	IPEPG_ELEM_PROGRAM_MEDIA_PUBDATE,
	IPEPG_ELEM_PROGRAM_MEDIA_EXPIREDATE,
	IPEPG_ELEM_PROGRAM_MEDIA_TRANSMITDATE,
	IPEPG_ELEM_PROGRAM_MEDIA_PRICE,
	IPEPG_ELEM_EPG_MAX
}IPEPG_ELEMENT_e;


typedef struct
{
	IPEPG_TAG_e		eDataIdx;
	HBOOL					bFlagEnd;
	HUINT32					ulAttrDataType;
	HUINT8					*pucAttrName;
} IPEPG_ATTR_t;


typedef struct IpepgHumaxElement
{
//	HBOOL					bFlagAttr;
	HUINT32					eDataIdx;

	HBOOL					bFlagEnd;
	HUINT32					ulDepth;

	HUINT32					ulTagDataType;
	HUINT8					*pucTagName;

	struct IpepgHumaxElement	*ElementList;

	IPEPG_ATTR_t	*AttributeList;

} IPEPG_ELEMENT_t;


typedef struct
{
	HUINT8		ucOrgStrLen;
	HUINT8		ucFlagReplace;
	HUINT8		szOrgStr[8];
	HUINT8		ucReplaceCode;
} IPEPG_REPLACESTR_t;


////////////////////////////////////////////////////////////////////////////////////////
#define _________________________________________________________________SVC_URL_INFO________________

const static IPEPG_ATTR_t	s_astAttrChannel[] =
{
	{IPEPG_SVC_SVCHANDLE,		TRUE,		IPEPG_TYPE_DECIMAL, (HUINT8 *)"HSVC"}
};

const static IPEPG_ATTR_t		s_astAttrTriple[] =
{
	{IPEPG_SVC_ONID,		FALSE,		IPEPG_TYPE_DECIMAL,		(HUINT8 *)"ONID"},
	{IPEPG_SVC_TSID,		FALSE,		IPEPG_TYPE_DECIMAL,		(HUINT8 *)"TSID"},
	{IPEPG_SVC_SVCID, 	TRUE,		IPEPG_TYPE_DECIMAL,		(HUINT8 *)"SID"}
};


const static IPEPG_ATTR_t		s_astAttrImage[] =
{
	{IPEPG_SVC_THUMBNAIL_SIZE,		FALSE,		IPEPG_TYPE_DECIMAL,		(HUINT8 *)"URLSIZE"},
	{IPEPG_SVC_THUMBNAIL_TYPE,		TRUE,		IPEPG_TYPE_STRING,		(HUINT8 *)"URLTYPE"},
};

const static IPEPG_ELEMENT_t		s_astElementImages[] =
{
	{IPEPG_SVC_THUMBNAIL_URL,		TRUE,		IPEPG_ELEMENT_DEPTH_4,		IPEPG_TYPE_STRING,			(HUINT8 *)"IMAGE",			NULL,	(IPEPG_ATTR_t *)s_astAttrImage	}
};


const static IPEPG_ELEMENT_t		s_astElementTriples[] =
{
	{IPEPG_SVC_TRIPLEID,	TRUE,		IPEPG_ELEMENT_DEPTH_4,		IPEPG_TYPE_NONE,			(HUINT8 *)"TRIPLEID",			NULL,	(IPEPG_ATTR_t *)s_astAttrTriple	}
};


const static IPEPG_ELEMENT_t		s_astElementChannelInfo[] =
{
	{IPEPG_SVC_NAME,				FALSE,		IPEPG_ELEMENT_DEPTH_3,		IPEPG_TYPE_STRING,	(HUINT8 *)"CHANNELNAME",			NULL,	NULL	},
	{IPEPG_SVC_SUPPORTMEDIA,	FALSE,		IPEPG_ELEMENT_DEPTH_3,		IPEPG_TYPE_STRING,		(HUINT8 *)"SUPPORTMEDIA",			NULL,	NULL	},
	{IPEPG_SVC_TRIPLEIDS, 		FALSE,		IPEPG_ELEMENT_DEPTH_3,		IPEPG_TYPE_NONE,		(HUINT8 *)"TRIPLEIDS",				(IPEPG_ELEMENT_t *)s_astElementTriples,	NULL	},
	{IPEPG_SVC_THUMBNAIL_URLS,	TRUE,		IPEPG_ELEMENT_DEPTH_3,		IPEPG_TYPE_NONE,		(HUINT8 *)"IMAGES", 				(IPEPG_ELEMENT_t *)s_astElementImages,		NULL	}
};

const static IPEPG_ELEMENT_t		s_astElementSvcChannel[] =
{
	{IPEPG_STATION_SVCDATA,	TRUE,		IPEPG_ELEMENT_DEPTH_2,		IPEPG_TYPE_NONE,		(HUINT8 *)"CHANNEL", 	(IPEPG_ELEMENT_t *)s_astElementChannelInfo,	(IPEPG_ATTR_t *)s_astAttrChannel	}
};

const static IPEPG_ELEMENT_t		s_astElementSvcChannels[] =
{
	{IPEPG_STATION_SVCLIST, 	TRUE,		IPEPG_ELEMENT_DEPTH_1,		IPEPG_TYPE_NONE, 		(HUINT8 *)"CHANNELS",	(IPEPG_ELEMENT_t *)s_astElementSvcChannel,	NULL	}
};

const static IPEPG_ELEMENT_t		s_astElementSvcResponse[] =
{
	{IPEPG_STATION_RESPONSE,	TRUE,		IPEPG_ELEMENT_DEPTH_ROOT,		IPEPG_TYPE_NONE,		(HUINT8 *)"IPEPG",		(IPEPG_ELEMENT_t *)s_astElementSvcChannels,	NULL	}
};

#define _________________________________________________________________EVENT_INFO________________

const static IPEPG_ATTR_t	s_astAttrIpEpgChannel[] =
{
	{IPEPG_ATTR_CHANNEL_HSVC,			FALSE,	IPEPG_TYPE_DECIMAL, (HUINT8 *)"HSVC"},
	{IPEPG_ATTR_CHANNEL_ID,				TRUE,	IPEPG_TYPE_DECIMAL, 	(HUINT8 *)"CHANNELID"}
};

const static IPEPG_ATTR_t	s_astAttrIpEpgChannelTripleId[] =
{
	{IPEPG_ATTR_CHANNEL_TRIPLE_ONID,	FALSE,	IPEPG_TYPE_DECIMAL, 	(HUINT8 *)"ONID"},
	{IPEPG_ATTR_CHANNEL_TRIPLE_TSID,	FALSE,	IPEPG_TYPE_DECIMAL, 	(HUINT8 *)"TSID"},
	{IPEPG_ATTR_CHANNEL_TRIPLE_SVCID,	TRUE,	IPEPG_TYPE_DECIMAL, 	(HUINT8 *)"SID"}
};

const static IPEPG_ATTR_t	s_astAttrIpEpgChannelImage[] =
{
	{IPEPG_ATTR_CHANNEL_IMAGESIZE,		FALSE,	IPEPG_TYPE_DECIMAL, 	(HUINT8 *)"URLSIZE"},
	{IPEPG_ATTR_CHANNEL_IMAGETYPE,		TRUE,	IPEPG_TYPE_STRING, 		(HUINT8 *)"URLTYPE"}
};

const static IPEPG_ATTR_t	s_astAttrIpEpgProgramUpdate[] =
{
	{IPEPG_ATTR_PROGRAM_ENDTIME,		TRUE,	IPEPG_TYPE_STRING, 	(HUINT8 *)"TYPE"}
};


const static IPEPG_ATTR_t	s_astAttrIpEpgProgram[] =
{
	{IPEPG_ATTR_PROGRAM_ID,				FALSE,	IPEPG_TYPE_DECIMAL, (HUINT8 *)"PROGRAMID"},
	{IPEPG_ATTR_PROGRAM_STARTTIME,		FALSE,	IPEPG_TYPE_DECIMAL, (HUINT8 *)"STARTDATETIME"},
	{IPEPG_ATTR_PROGRAM_ENDTIME,		TRUE,	IPEPG_TYPE_DECIMAL, 	(HUINT8 *)"ENDDATETIME"}
};

// Ip Epg Element

const static IPEPG_ATTR_t	s_astAttrIpEpgProgName[] =
{
	{IPEPG_ATTR_PROGRAM_NAME,			TRUE,	IPEPG_TYPE_STRING, 	(HUINT8 *)"LANG"}
};
const static IPEPG_ATTR_t	s_astAttrIpEpgProgSubName[] =
{
	{IPEPG_ATTR_PROGRAM_SUBNAME,		TRUE,	IPEPG_TYPE_STRING, 	(HUINT8 *)"LANG"}
};
const static IPEPG_ATTR_t	s_astAttrIpEpgProgShortText[] =
{
	{IPEPG_ATTR_PROGRAM_SHORTTEXT,		TRUE,	IPEPG_TYPE_STRING, 	(HUINT8 *)"LANG"}
};
const static IPEPG_ATTR_t	s_astAttrIpEpgProgLongText[] =
{
	{IPEPG_ATTR_PROGRAM_LONGTEXT,		TRUE,	IPEPG_TYPE_STRING, 	(HUINT8 *)"LANG"}
};

const static IPEPG_ATTR_t	s_astAttrIpEpgProSeries[] =
{
	{IPEPG_ATTR_PROGRAM_SERIES_ID,		FALSE,	IPEPG_TYPE_DECIMAL, (HUINT8 *)"SERIESID"},
	{IPEPG_ATTR_PROGRAM_SERIES_COUNT,	FALSE,	IPEPG_TYPE_DECIMAL, (HUINT8 *)"SERIESCOUNT"},
	{IPEPG_ATTR_PROGRAM_SERIES_EPISODE,	FALSE,	IPEPG_TYPE_DECIMAL, (HUINT8 *)"SERIESNUMBER"},
	{IPEPG_ATTR_PROGRAM_SERIES_SEASON,	TRUE,	IPEPG_TYPE_DECIMAL, (HUINT8 *)"SEASONNUMBER"}
};

const static IPEPG_ATTR_t	s_astAttrIpEpgProgImage[] =
{
	{IPEPG_ATTR_PROGRAM_IMAGE_SIZE,		FALSE,	IPEPG_TYPE_DECIMAL, 	(HUINT8 *)"URLSIZE"},
	{IPEPG_ATTR_PROGRAM_IMAGE_TYPE,		TRUE,	IPEPG_TYPE_STRING, 		(HUINT8 *)"URLTYPE"}
};

const static IPEPG_ATTR_t	s_astAttrIpEpgProgPerson[] =
{
	{IPEPG_ATTR_PROGRAM_PERSON_TYPE,	TRUE,	IPEPG_TYPE_STRING, 	(HUINT8 *)"TYPE"}
};

const static IPEPG_ATTR_t	s_astAttrIpEpgProgGenre[] =
{
	{IPEPG_ATTR_PROGRAM_GENRE_CODE1,	FALSE,	IPEPG_TYPE_DECIMAL, 	(HUINT8 *)"CODE1"},
	{IPEPG_ATTR_PROGRAM_GENRE_CODE2,	TRUE,	IPEPG_TYPE_DECIMAL, 	(HUINT8 *)"CODE2"}
};

const static IPEPG_ATTR_t	s_astAttrIpEpgProgCodec[] =
{
	{IPEPG_ATTR_PROGRAM_GENRE_CODE2,	TRUE,	IPEPG_TYPE_STRING, 	(HUINT8 *)"TYPE"}
};

const static IPEPG_ATTR_t	s_astAttrIpEpgProSeriesLink[] =
{
	{IPEPG_ATTR_PROGRAM_SERIESLINK_PROGID,		FALSE,	IPEPG_TYPE_DECIMAL, (HUINT8 *)"PROGRAMID"},
	{IPEPG_ATTR_PROGRAM_SERIESLINK_STARTTIME,	FALSE,	IPEPG_TYPE_DECIMAL, (HUINT8 *)"STARTDATETIME"},
	{IPEPG_ATTR_PROGRAM_SERIESLINK_ENDTIME,		FALSE,	IPEPG_TYPE_DECIMAL, (HUINT8 *)"ENDDATETIME"},
	{IPEPG_ATTR_PROGRAM_SERIESLINK_EPISODE,		FALSE,	IPEPG_TYPE_DECIMAL, (HUINT8 *)"SERIESNUMBER"},
	{IPEPG_ATTR_PROGRAM_SERIESLINK_SEASON,		TRUE,	IPEPG_TYPE_DECIMAL, (HUINT8 *)"SEASONNUMBER"}
};

const static IPEPG_ATTR_t	s_astAttrIpEpgProgSeriesLinkName[] =
{
	{IPEPG_ATTR_PROGRAM_SERIESLINK_NAME,		TRUE,	IPEPG_TYPE_STRING, 	(HUINT8 *)"LANG"}
};

const static IPEPG_ELEMENT_t 	s_astElemIpEpgProgSeriesLinkName[] =
{
	{IPEPG_ELEM_PROGRAM_SERIESLINK_NAME,	TRUE,	IPEPG_ELEMENT_DEPTH_9,	IPEPG_TYPE_STRING,	(HUINT8 *)"TITLESUB", 		(IPEPG_ELEMENT_t *)NULL,			(IPEPG_ATTR_t *)s_astAttrIpEpgProgSeriesLinkName		}
};

const static IPEPG_ATTR_t	s_astAttrIpEpgProgMediaImage[] =
{
	{IPEPG_ATTR_PROGRAM_IMAGE_SIZE,			FALSE,	IPEPG_TYPE_DECIMAL, 	(HUINT8 *)"SIZE"	},
	{IPEPG_ATTR_PROGRAM_IMAGE_TYPE,			TRUE,	IPEPG_TYPE_STRING, 		(HUINT8 *)"TYPE"	}
};
const static IPEPG_ELEMENT_t	s_astElemIpEpgProgMediaImage[] =
{
	{IPEPG_ELEM_PROGRAM_MEDIA_IMAGE,		TRUE,	IPEPG_ELEMENT_DEPTH_9, 	IPEPG_TYPE_STRING, 	(HUINT8 *)"IMAGE", 		(IPEPG_ELEMENT_t *)NULL,			(IPEPG_ATTR_t *)s_astAttrIpEpgProgMediaImage	},
};

const static IPEPG_ATTR_t	s_astAttrIpEpgProgMediaURL[] =
{
	{IPEPG_ATTR_PROGRAM_MEDIA_TYPE,			FALSE,	IPEPG_TYPE_STRING, 	(HUINT8 *)"TYPE"	},
	{IPEPG_ATTR_PROGRAM_MEDIA_TARGET,		TRUE,	IPEPG_TYPE_STRING, 	(HUINT8 *)"TARGET"	}
};
const static IPEPG_ELEMENT_t	s_astElemIpEpgProgMediaURL[] =
{
	{IPEPG_ELEM_PROGRAM_MEDIA_URL,			TRUE,	IPEPG_ELEMENT_DEPTH_9, 	IPEPG_TYPE_STRING, 	(HUINT8 *)"MEDIAURL", 		(IPEPG_ELEMENT_t *)NULL,			(IPEPG_ATTR_t *)s_astAttrIpEpgProgMediaURL	},
};

const static IPEPG_ELEMENT_t 	s_astElemIpEpgProgMediaInfo[] =
{

	{IPEPG_ELEM_PROGRAM_MEDIA_URLS,			FALSE,	IPEPG_ELEMENT_DEPTH_8,	IPEPG_TYPE_NONE,	(HUINT8 *)"MEDIAURLS", 		(IPEPG_ELEMENT_t *)s_astElemIpEpgProgMediaURL,			(IPEPG_ATTR_t *)NULL		},
	{IPEPG_ELEM_PROGRAM_MEDIA_IMAGES,		FALSE,	IPEPG_ELEMENT_DEPTH_8,	IPEPG_TYPE_NONE,	(HUINT8 *)"IMAGES", 		(IPEPG_ELEMENT_t *)s_astElemIpEpgProgMediaImage,		(IPEPG_ATTR_t *)NULL		},
	{IPEPG_ELEM_PROGRAM_MEDIA_PUBDATE,		FALSE,	IPEPG_ELEMENT_DEPTH_8,	IPEPG_TYPE_DECIMAL,	(HUINT8 *)"PUBDATE", 		(IPEPG_ELEMENT_t *)NULL,			(IPEPG_ATTR_t *)NULL		},
	{IPEPG_ELEM_PROGRAM_MEDIA_EXPIREDATE,	FALSE,	IPEPG_ELEMENT_DEPTH_8,	IPEPG_TYPE_DECIMAL,	(HUINT8 *)"EXPIREDATE", 	(IPEPG_ELEMENT_t *)NULL,			(IPEPG_ATTR_t *)NULL		},
	{IPEPG_ELEM_PROGRAM_MEDIA_TRANSMITDATE,	FALSE,	IPEPG_ELEMENT_DEPTH_8,	IPEPG_TYPE_DECIMAL,	(HUINT8 *)"TRANSMITDATE", 	(IPEPG_ELEMENT_t *)NULL,			(IPEPG_ATTR_t *)NULL		},
	{IPEPG_ELEM_PROGRAM_MEDIA_PRICE,		TRUE,	IPEPG_ELEMENT_DEPTH_8,	IPEPG_TYPE_DECIMAL,	(HUINT8 *)"PRICE", 			(IPEPG_ELEMENT_t *)NULL,			(IPEPG_ATTR_t *)NULL		}
};

const static IPEPG_ELEMENT_t 	s_astElemIpEpgProgSeriesLink[] =
{
	{IPEPG_ELEM_PROGRAM_SERIESLINK,			TRUE,	IPEPG_ELEMENT_DEPTH_8,	IPEPG_TYPE_NONE,	(HUINT8 *)"SERIESLINK", 	(IPEPG_ELEMENT_t *)s_astElemIpEpgProgSeriesLinkName,	(IPEPG_ATTR_t *)s_astAttrIpEpgProSeriesLink		}
};


const static IPEPG_ELEMENT_t		s_astElemIpEpgProgImage[] =
{
	{IPEPG_ELEM_PROGRAM_IMAGE,				TRUE,	IPEPG_ELEMENT_DEPTH_7,	IPEPG_TYPE_STRING,		(HUINT8 *)"IMAGE",	(IPEPG_ELEMENT_t *)NULL,		(IPEPG_ATTR_t *)s_astAttrIpEpgProgImage}
};

const static IPEPG_ELEMENT_t		s_astElemIpEpgProgPerson[] =
{
	{IPEPG_ELEM_PROGRAM_PERSON,				TRUE,	IPEPG_ELEMENT_DEPTH_7,	IPEPG_TYPE_STRING,		(HUINT8 *)"PERSON",	(IPEPG_ELEMENT_t *)NULL,		(IPEPG_ATTR_t *)s_astAttrIpEpgProgPerson}
};

const static IPEPG_ELEMENT_t		s_astElemIpEpgProgGenre[] =
{
	{IPEPG_ELEM_PROGRAM_GENRE,				TRUE,	IPEPG_ELEMENT_DEPTH_7,	IPEPG_TYPE_STRING,		(HUINT8 *)"GENRE",	(IPEPG_ELEMENT_t *)NULL,		(IPEPG_ATTR_t *)s_astAttrIpEpgProgGenre}
};

const static IPEPG_ELEMENT_t		s_astElemIpEpgProgCodec[] =
{
	{IPEPG_ELEM_PROGRAM_CODEC,				TRUE,	IPEPG_ELEMENT_DEPTH_7,	IPEPG_TYPE_STRING,		(HUINT8 *)"CODEC",	(IPEPG_ELEMENT_t *)NULL,		(IPEPG_ATTR_t *)s_astAttrIpEpgProgCodec}
};

const static IPEPG_ELEMENT_t		s_astElemIpEpgProgSeriesLinks[] =
{
	{IPEPG_ELEM_PROGRAM_SERIESLINKS,		TRUE,	IPEPG_ELEMENT_DEPTH_7,	IPEPG_TYPE_NONE,		(HUINT8 *)"SERIESLINKS",	(IPEPG_ELEMENT_t *)s_astElemIpEpgProgSeriesLink,		(IPEPG_ATTR_t *)NULL}
};

const static IPEPG_ATTR_t			s_astAttrIpEpgProgMedia[] =
{
	{IPEPG_ATTR_PROGRAM_MEDIA_ID,		TRUE,	IPEPG_TYPE_STRING, 	(HUINT8 *)"ID"	}
};

const static IPEPG_ELEMENT_t		s_astElemIpEpgProgMedia[] =
{
	{IPEPG_ELEM_PROGRAM_MEDIA,			TRUE,	IPEPG_ELEMENT_DEPTH_7,	IPEPG_TYPE_NONE,		(HUINT8 *)"MEDIA",	(IPEPG_ELEMENT_t *)s_astElemIpEpgProgMediaInfo,		(IPEPG_ATTR_t *)s_astAttrIpEpgProgMedia}
};


const static IPEPG_ELEMENT_t 	s_astElemIpEpgProgInfo[] =
{
	{IPEPG_ELEM_PROGRAM_NAME,			FALSE,	IPEPG_ELEMENT_DEPTH_6,	IPEPG_TYPE_STRING,	(HUINT8 *)"TITLE",			(IPEPG_ELEMENT_t *)NULL,							(IPEPG_ATTR_t *)s_astAttrIpEpgProgName		},
	{IPEPG_ELEM_PROGRAM_SUBNAME,		FALSE,	IPEPG_ELEMENT_DEPTH_6,	IPEPG_TYPE_STRING,	(HUINT8 *)"TITLESUB", 		(IPEPG_ELEMENT_t *)NULL,							(IPEPG_ATTR_t *)s_astAttrIpEpgProgSubName		},
	{IPEPG_ELEM_PROGRAM_SHORTTEXT,		FALSE,	IPEPG_ELEMENT_DEPTH_6,	IPEPG_TYPE_STRING,	(HUINT8 *)"SHORTTEXT", 		(IPEPG_ELEMENT_t *)NULL,							(IPEPG_ATTR_t *)s_astAttrIpEpgProgShortText		},
	{IPEPG_ELEM_PROGRAM_LONGTEXT,		FALSE,	IPEPG_ELEMENT_DEPTH_6,	IPEPG_TYPE_STRING,	(HUINT8 *)"LONGTEXT", 		(IPEPG_ELEMENT_t *)NULL,							(IPEPG_ATTR_t *)s_astAttrIpEpgProgLongText		},
	{IPEPG_ELEM_PROGRAM_RECOMMENDATION,	FALSE,	IPEPG_ELEMENT_DEPTH_6,	IPEPG_TYPE_DECIMAL,	(HUINT8 *)"RECOMMENDATION", (IPEPG_ELEMENT_t *)NULL,							(IPEPG_ATTR_t *)NULL		},
	{IPEPG_ELEM_PROGRAM_RATING,			FALSE,	IPEPG_ELEMENT_DEPTH_6,	IPEPG_TYPE_DECIMAL,	(HUINT8 *)"RATING", 		(IPEPG_ELEMENT_t *)NULL,							(IPEPG_ATTR_t *)NULL		},
	{IPEPG_ELEM_PROGRAM_IMAGES,			FALSE,	IPEPG_ELEMENT_DEPTH_6,	IPEPG_TYPE_NONE,	(HUINT8 *)"IMAGES", 		(IPEPG_ELEMENT_t *)s_astElemIpEpgProgImage,			(IPEPG_ATTR_t *)NULL		},
	{IPEPG_ELEM_PROGRAM_PERSONS,		FALSE,	IPEPG_ELEMENT_DEPTH_6,	IPEPG_TYPE_NONE,	(HUINT8 *)"PERSONS",		(IPEPG_ELEMENT_t *)s_astElemIpEpgProgPerson,		(IPEPG_ATTR_t *)NULL		},
	{IPEPG_ELEM_PROGRAM_GENRES, 		FALSE,	IPEPG_ELEMENT_DEPTH_6,	IPEPG_TYPE_NONE,	(HUINT8 *)"GENRES", 		(IPEPG_ELEMENT_t *)s_astElemIpEpgProgGenre, 		(IPEPG_ATTR_t *)NULL		},
	{IPEPG_ELEM_PROGRAM_CODECS, 		FALSE,	IPEPG_ELEMENT_DEPTH_6,	IPEPG_TYPE_NONE,	(HUINT8 *)"CODECS", 		(IPEPG_ELEMENT_t *)s_astElemIpEpgProgCodec, 		(IPEPG_ATTR_t *)NULL		},
	{IPEPG_ELEM_PROGRAM_SERIES,			FALSE,	IPEPG_ELEMENT_DEPTH_6,	IPEPG_TYPE_NONE,	(HUINT8 *)"SERIES", 		(IPEPG_ELEMENT_t *)s_astElemIpEpgProgSeriesLinks,	(IPEPG_ATTR_t *)s_astAttrIpEpgProSeries		},
	{IPEPG_ELEM_PROGRAM_MEDIAS, 		TRUE,	IPEPG_ELEMENT_DEPTH_6,	IPEPG_TYPE_NONE,	(HUINT8 *)"MEDIAS", 		(IPEPG_ELEMENT_t *)s_astElemIpEpgProgMedia, 		(IPEPG_ATTR_t *)NULL		}
};


const static IPEPG_ELEMENT_t		s_astElemIpEpgProgram[] =
{
	{IPEPG_ELEM_PROGRAM,				TRUE,	IPEPG_ELEMENT_DEPTH_5,	IPEPG_TYPE_NONE,		(HUINT8 *)"PROGRAM",	(IPEPG_ELEMENT_t *)s_astElemIpEpgProgInfo,		(IPEPG_ATTR_t *)s_astAttrIpEpgProgram}
};

const static IPEPG_ELEMENT_t		s_astElemIpEpgPrograms[] =
{
	{IPEPG_ELEM_PROGRAMS,				TRUE,	IPEPG_ELEMENT_DEPTH_4,	IPEPG_TYPE_NONE,		(HUINT8 *)"PROGRAMS",	(IPEPG_ELEMENT_t *)s_astElemIpEpgProgram,		(IPEPG_ATTR_t *)NULL}
};

const static IPEPG_ELEMENT_t		s_astElemIpEpgChannelImage[] =
{
	{IPEPG_ELEM_CHANNEL_IMAGE,			TRUE,	IPEPG_ELEMENT_DEPTH_4,	IPEPG_TYPE_STRING,		(HUINT8 *)"IMAGE",		(IPEPG_ELEMENT_t *)NULL,							(IPEPG_ATTR_t *)s_astAttrIpEpgChannelImage}
};

const static IPEPG_ELEMENT_t		s_astElemIpEpgChannelTripleId[] =
{
	{IPEPG_ELEM_CHANNEL_TRIPLEID,		TRUE,	IPEPG_ELEMENT_DEPTH_4,	IPEPG_TYPE_NONE,		(HUINT8 *)"TRIPLEID",	(IPEPG_ELEMENT_t *)NULL,							(IPEPG_ATTR_t *)s_astAttrIpEpgChannelTripleId}
};

const static IPEPG_ELEMENT_t		s_astElemIpEpgChannelInfo[] =
{
	{IPEPG_ELEM_CHANNEL_NUM,			FALSE,	IPEPG_ELEMENT_DEPTH_3,	IPEPG_TYPE_DECIMAL, 	(HUINT8 *)"CHANNELNUM", (IPEPG_ELEMENT_t *)NULL,							(IPEPG_ATTR_t *)NULL},
	{IPEPG_ELEM_CHANNEL_NAME,			FALSE,	IPEPG_ELEMENT_DEPTH_3,	IPEPG_TYPE_STRING,		(HUINT8 *)"CHANNELNAME",(IPEPG_ELEMENT_t *)NULL,							(IPEPG_ATTR_t *)NULL},
	{IPEPG_ELEM_CHANNEL_TRIPLEIDS,		FALSE,	IPEPG_ELEMENT_DEPTH_3,	IPEPG_TYPE_NONE,		(HUINT8 *)"TRIPLEIDS",	(IPEPG_ELEMENT_t *)s_astElemIpEpgChannelTripleId,	(IPEPG_ATTR_t *)NULL},
	{IPEPG_ELEM_CHANNEL_IMAGES, 		FALSE,	IPEPG_ELEMENT_DEPTH_3,	IPEPG_TYPE_NONE,		(HUINT8 *)"IMAGES", 	(IPEPG_ELEMENT_t *)s_astElemIpEpgChannelImage,		(IPEPG_ATTR_t *)NULL},
	{IPEPG_ELEM_CHANNEL_UPDATE, 		TRUE,	IPEPG_ELEMENT_DEPTH_3,	IPEPG_TYPE_STRING,		(HUINT8 *)"UPDATETYPE", (IPEPG_ELEMENT_t *)s_astElemIpEpgPrograms,			(IPEPG_ATTR_t *)s_astAttrIpEpgProgramUpdate}
};


const static IPEPG_ELEMENT_t		s_astElemIpEpgChannel[] =
{
	{IPEPG_ELEM_CHANNEL,				TRUE,	IPEPG_ELEMENT_DEPTH_2,	IPEPG_TYPE_NONE,		(HUINT8 *)"CHANNEL",	(IPEPG_ELEMENT_t *)s_astElemIpEpgChannelInfo,		(IPEPG_ATTR_t *)s_astAttrIpEpgChannel}
};


const static IPEPG_ELEMENT_t		s_astElemIpEpgChannels[] =
{
	{IPEPG_ELEM_CHANNELS,				TRUE,	IPEPG_ELEMENT_DEPTH_1,	IPEPG_TYPE_NONE,		(HUINT8 *)"CHANNELS",	(IPEPG_ELEMENT_t *)s_astElemIpEpgChannel,		(IPEPG_ATTR_t *)s_astAttrIpEpgChannel}
};

const static IPEPG_ELEMENT_t	s_astElemIpEpg[] =
{
	{IPEPG_ELEM_EVENT_IPEPG,			TRUE,	IPEPG_ELEMENT_DEPTH_ROOT,	IPEPG_TYPE_NONE,	(HUINT8 *)"IPEPG",		(IPEPG_ELEMENT_t *)s_astElemIpEpgChannels,	NULL	}
};

static HERROR			ipepg_LoadSvcElementList(IPEPG_ELEMENT_t **ppstElementList)
{
	IPEPG_RETURN_IF(ppstElementList == NULL, ERR_FAIL);
	*ppstElementList = (IPEPG_ELEMENT_t *)&s_astElementSvcResponse;
	return ERR_OK;
}

static HERROR			ipepg_LoadEventElementList(IPEPG_ELEMENT_t **ppstElementList)
{
	IPEPG_RETURN_IF(ppstElementList == NULL, ERR_FAIL);
	*ppstElementList = (IPEPG_ELEMENT_t *)&s_astElemIpEpg;
	return ERR_OK;
}




/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static IPEPG_REPLACESTR_t		s_astIpepgHumaxReplaceString[] =
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

static HUINT32		ipepg_Humax_Hextoi (HUINT8 *pucPtr)
{
	HUINT32	ulVal = 0 ;
	HxLOG_Trace();


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


static HERROR		ipepg_Humax_IsReplaceString(HUINT8 *pucPtr, HBOOL *pbIsReplace, HUINT8 *pucSize, HUINT8 *puReplaceCode)
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

static HERROR		ipepg_Humax_SetDataByType(	HUINT32 ulType,
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
		case IPEPG_TYPE_STRING :
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
					hError = ipepg_Humax_IsReplaceString(&pucSrc[sIndex], &bReplace, &ucSize, &ucReplaceCode);
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

		case IPEPG_TYPE_DECIMAL :
			*pulDecimal = atoi((const char *)pucSrc);
			break;

		case IPEPG_TYPE_DECIMAL64 :
			*pulDecimal = 0;
			break;

		case IPEPG_TYPE_HEXA:
			*pulDecimal = ipepg_Humax_Hextoi(pucSrc);
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


static HERROR ipepg_Humax_SetServiceData(void	*target, HUINT32 eDataIdx,HUINT32 ulMaxLen, HUINT8 *pszString, HUINT32 pulDecimal)
{
	EPG_URL_INFO_t	*pstTempURLInfo = (EPG_URL_INFO_t	*)target;
	//HxLOG_Trace();

	if(pstTempURLInfo == NULL)
	{
		return FALSE;
	}
	switch((IPEPG_TAG_e)eDataIdx)
	{
		case IPEPG_SVC_ONID :
			pstTempURLInfo->stTripleId.usOnId = (HUINT16)pulDecimal;
			break;
		case IPEPG_SVC_TSID :
			pstTempURLInfo->stTripleId.usTsId = (HUINT16)pulDecimal;
			break;
		case IPEPG_SVC_SVCID :
			pstTempURLInfo->stTripleId.usSvcId = (HUINT16)pulDecimal;
			break;
		case IPEPG_SVC_SVCHANDLE :
			pstTempURLInfo->ulSvcUid = (HUINT32)pulDecimal;
			break;
		case IPEPG_SVC_URL :
			{
				HUINT32 ulLengthUrl = 0;
				if(pszString == NULL)
					return FALSE;

				HxSTD_MemSet(pstTempURLInfo->pucUrl, 0x00, sizeof(HUINT8)*IPEPG_URL_MAX_LEN);
				ulLengthUrl = HxSTD_StrLen(pszString);
				if((ulLengthUrl>=IPEPG_URL_MAX_LEN) || (ulLengthUrl == 0))
				{
					HxLOG_Error("[xsvc_epglib_ParseToHumaxServiceList_Humax] invalid EPGHUMAX_SVC_URL fail!!\n");
					break;
				}
				HxSTD_MemCopy(pstTempURLInfo->pucUrl, pszString, ulLengthUrl);
				//pstTempURLInfo->pucUrl[ulLengthUrl]= '\0';
			}
			break;
		case IPEPG_SVC_THUMBNAIL_URL:
			{
				HUINT32 ulLengthUrl = 0;
				if(pszString == NULL)
					return FALSE;

				HxSTD_MemSet(pstTempURLInfo->pucThumbnailUrl, 0x00, sizeof(HUINT8)*IPEPG_URL_MAX_LEN);
				ulLengthUrl = HxSTD_StrLen(pszString);
				if((ulLengthUrl>=IPEPG_URL_MAX_LEN) || (ulLengthUrl == 0))
				{
					HxLOG_Error("[xsvc_epglib_ParseToHumaxServiceList_Humax] invalid EPGHUMAX_SVC_URL fail!!\n");
					break;
				}
				HxSTD_MemCopy(pstTempURLInfo->pucThumbnailUrl, pszString, ulLengthUrl);
				HxLOG_Print("THUMBNAIL : %s\n", pszString);
			}
			break;

		case IPEPG_SVC_SUPPORTMEDIA:
			{
				// "TRUE" or "FALSE"
				if(NULL != pszString && 4 == HxSTD_StrLen(pszString) && 0 == HLIB_STD_StrCaseCmp(pszString, "TRUE") )
				{
					pstTempURLInfo->bSupportMedia = TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			break;

		case IPEPG_STATION_RESPONSE :
		case IPEPG_STATION_SVCLIST :
		case IPEPG_STATION_SVCDATA :
		case IPEPG_SVC_MAX :
		case IPEPG_STATIONID :
		case IPEPG_LANG :
		case IPEPG_VERSION :
		case IPEPG_SVC_NAME :
		default :
			//HxLOG_Error("No Need to Set for this flag[%d]\n", eDataIdx);
			break;
	}

	return TRUE;

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
	static		HUINT32	ulImageSize = 0;
	DxIpEventMedia_t * pMedia = NULL;
	DxIpEventStr_t * pIpEventStr = NULL;

	IPEPG_RETURN_IF(pEvent == NULL, ERR_FAIL);

	if(pEvent == NULL)
		return ERR_FAIL;

	switch((IPEPG_ELEMENT_e)eDataIdx)
	{
		case IPEPG_ATTR_CHANNEL_HSVC :
		case IPEPG_ATTR_CHANNEL_ID :
		case IPEPG_ELEM_CHANNEL_NUM :
		case IPEPG_ELEM_CHANNEL_NAME :
			break;
		case IPEPG_ATTR_CHANNEL_TRIPLE_ONID :
			pEvent->stBase.usOnId = (HUINT16)ulDecimal;
			break;
		case IPEPG_ATTR_CHANNEL_TRIPLE_TSID :
			pEvent->stBase.usTsId = (HUINT16)ulDecimal;
			break;
		case IPEPG_ATTR_CHANNEL_TRIPLE_SVCID :
			pEvent->stBase.usSvcId = (HUINT16)ulDecimal;
			break;

		case IPEPG_ATTR_PROGRAM_ID :
			pEvent->stBase.ulUniqueId = ulDecimal;
			break;

		case IPEPG_ATTR_PROGRAM_STARTTIME :
#if defined(IPEPG_TEST_DIFFERENT_TIME)
			{
				HUINT32	ulSystemTime = 0, ulCurTime = 0, ulDifferentTime = 0;
				HxDATETIME_t dateTime;
				VK_CLOCK_GetTime((unsigned long*)&ulSystemTime);
				HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSystemTime,&dateTime);
				dateTime.stTime.ucHour = 0;		dateTime.stTime.ucMinute = 0;		dateTime.stTime.ucSecond = 0;
				HLIB_DATETIME_ConvertDateTimeToUnixTime (&dateTime, &ulSystemTime);
				dateTime.stDate.usYear = CUR_YEAR;		dateTime.stDate.ucMonth = CUR_MONTH;		dateTime.stDate.ucDay = CUR_DAY;
				HLIB_DATETIME_ConvertDateTimeToUnixTime (&dateTime, &ulCurTime);
				ulDifferentTime = ulCurTime-ulSystemTime;
				ulDecimal = ulDecimal - ulDifferentTime;
			}
#endif
			pEvent->stBase.ulStartTime = ulDecimal;
			break;
		case IPEPG_ATTR_PROGRAM_ENDTIME :
#if defined(IPEPG_TEST_DIFFERENT_TIME)
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
			pEvent->stBase.ulEndTime = ulDecimal;
			break;

		case IPEPG_ELEM_PROGRAM_RECOMMENDATION :
			pEvent->stBase.ucRecommendation = ulDecimal;
			break;
		case IPEPG_ELEM_PROGRAM_RATING :
			#ifdef IPEPG_PARSER_RATING_DO_NOT_MODIFY	// for HMS1000T
				if(ulDecimal <= 15 && ulDecimal >= 0)
				{
					pEvent->stBase.ucRating = (HUINT8)(ulDecimal);
				}
			#else
			pEvent->stBase.ucRating = (HUINT8)((ulDecimal<=3)?0:ulDecimal-3);
			#endif
			//HxLOG_Error("[%s][%d] IPEPG_ELEM_PROGRAM_RATING : %d/%d ", __FUNCTION__,__LINE__,pEvent->stBase.ucRating, ulDecimal);
			break;
		case IPEPG_ATTR_PROGRAM_IMAGE_SIZE :
			{
				if(ulImageSize == 0)		// 처음 받은 이미지
					ulImageSize = ulDecimal;
				else
				{
					HUINT32 i = 0;
					HINT32 ulImgIndex = -1;

					for(i = 0; i<pEvent->ulStrCount; i++)
					{
						if(pEvent->stText[i].eType == eDxIPEPG_TEXT_THUMBNAIL)
						{
							ulImgIndex = i;
							break;
						}
					}
					if(ulImgIndex == -1)	// there is no url for latest thumbnail
					{
						HxLOG_Error("IPEPG Parsing Error\n");
						ulImageSize = ulDecimal;
						break;
					}

					if(ulImageSize > IPEPG_MAX_IMAGE_SIZE)
					{
						if(ulImageSize > ulDecimal)		// 기존 이미지가 max size 보다 큰경우 new image가 작으면 대체.
						{
							ulImageSize = ulDecimal;
							IPEPG_MemFree(pEvent->stText[ulImgIndex].ucStr);
							pEvent->stText[ulImgIndex].ucStr = NULL;
						}
					}
					if(ulImageSize <= IPEPG_MAX_IMAGE_SIZE)
					{
						if(ulImageSize < ulDecimal && ulDecimal <= IPEPG_MAX_IMAGE_SIZE)
						{
							ulImageSize = ulDecimal;
							IPEPG_MemFree(pEvent->stText[ulImgIndex].ucStr);
							pEvent->stText[ulImgIndex].ucStr = NULL;
						}
					}
				}
				//HxLOG_Print("IMAGE : %d ", ulDecimal);
			}
			break;
		case IPEPG_ATTR_PROGRAM_IMAGE_TYPE :
			//HxLOG_Print("type : %s ", pszString);
			break;
		case IPEPG_ATTR_PROGRAM_PERSON_TYPE :
		case IPEPG_ELEM_PROGRAM_GENRE :
			break;
		case IPEPG_ATTR_PROGRAM_GENRE_CODE1 :
			{
				HUINT8 ucMainGenre = (HUINT8)(ulDecimal);
				pEvent->stBase.ucGenre |= ((ucMainGenre << 4) & 0xf0);
				//HxLOG_Print("IPEPG_ATTR_PROGRAM_GENRE_CODE1 : decimal(%d) ucGenre(%d)", ulDecimal,pEvent->stBase.ucGenre);
			}
			break;
		case IPEPG_ATTR_PROGRAM_GENRE_CODE2 :
			{
				HUINT8 ucSubGenre = (HUINT8)(ulDecimal);
				pEvent->stBase.ucGenre |= (ucSubGenre & 0x0f);
				//HxLOG_Print("IPEPG_ATTR_PROGRAM_GENRE_CODE2 : decimal(%d) ucGenre(%d)", ulDecimal,pEvent->stBase.ucGenre);
			}
			break;

		case IPEPG_ATTR_PROGRAM_SERIES_ID :
			pEvent->stBase.ulSeriesId = ulDecimal;
			break;
		case IPEPG_ATTR_PROGRAM_SERIES_COUNT :
			pEvent->stBase.ulTotalNum = ulDecimal;
			break;
		case IPEPG_ATTR_PROGRAM_SERIES_EPISODE :
			pEvent->stBase.ulEpisodeNum = ulDecimal;
			break;
		case IPEPG_ATTR_PROGRAM_SERIES_SEASON :
			pEvent->stBase.ulSeason = ulDecimal;
			break;
		case IPEPG_ATTR_PROGRAM_NAME :
		case IPEPG_ATTR_PROGRAM_SUBNAME :
		case IPEPG_ATTR_PROGRAM_SHORTTEXT :
		case IPEPG_ATTR_PROGRAM_LONGTEXT :

			if(pEvent->ulStrCount >= eDxIPEPG_TEXT_MAXCOUNT)
				break;
			strncpy(pEvent->stText[pEvent->ulStrCount].ucLang, pszString, EPG_HUMAX_LANG_LEN);

			break;
		case IPEPG_ELEM_PROGRAM_NAME :
		case IPEPG_ELEM_PROGRAM_SHORTTEXT :
		case IPEPG_ELEM_PROGRAM_LONGTEXT :
		case IPEPG_ELEM_PROGRAM_SUBNAME :
			{
				DxIPEPG_Text_e eIPEPG_TextType = eDxIPEPG_TEXT_MAX;
				if(pEvent->ulStrCount >= eDxIPEPG_TEXT_MAXCOUNT)
					break;

				if(eDataIdx == IPEPG_ELEM_PROGRAM_NAME) eIPEPG_TextType = eDxIPEPG_TEXT_NAME;
				else if(eDataIdx == IPEPG_ELEM_PROGRAM_SHORTTEXT) eIPEPG_TextType = eDxIPEPG_TEXT_SHORT;
				else if(eDataIdx == IPEPG_ELEM_PROGRAM_LONGTEXT) eIPEPG_TextType = eDxIPEPG_TEXT_DETAIL;
				else if(eDataIdx == IPEPG_ELEM_PROGRAM_SUBNAME) eIPEPG_TextType = eDxIPEPG_TEXT_SUBNAME;

				pEvent->stText[pEvent->ulStrCount].eType = eIPEPG_TextType;
				pEvent->stText[pEvent->ulStrCount].ucStr = IPEPG_StrDup(pszString);
				pEvent->stText[pEvent->ulStrCount].usSize= HxSTD_StrLen(pszString);
				pEvent->ulStrCount++;

				//HxLOG_Print("INDEX[%d] STRING[%s]\n",eIndex, pEvent->stText[eIndex].ucStr);
			}
			break;
		case IPEPG_ELEM_PROGRAM_IMAGE :
			{
				HUINT32 i = 0;
				HINT32 ulImgIndex = -1;
				for(i = 0; i<pEvent->ulStrCount; i++)
				{
					if(pEvent->stText[i].eType == eDxIPEPG_TEXT_THUMBNAIL)
					{
						ulImgIndex = i;
						break;
					}
				}
				if(ulImgIndex == -1)
					ulImgIndex = pEvent->ulStrCount++;

				if(pEvent->stText[ulImgIndex].ucStr == NULL)
				{
					pEvent->stText[ulImgIndex].eType = eDxIPEPG_TEXT_THUMBNAIL;
					pEvent->stText[ulImgIndex].ucStr = IPEPG_StrDup(pszString);
					pEvent->stText[ulImgIndex].usSize= HxSTD_StrLen(pszString);
					//HxLOG_Print("length: %d URL[%s]\n", pEvent->stText[ulImgIndex].usSize, pEvent->stText[ulImgIndex].ucStr);
				}
			}
			break;
		case IPEPG_ELEM_PROGRAM_IMAGES :
			ulImageSize = 0;
			//HxLOG_Debug("IPEPG_ELEM_PROGRAM_IMAGES %d\n", ulImageSize);
			break;
		case IPEPG_ELEM_PROGRAM_PERSON :
			{
				HCHAR * pPersons = NULL;
				HUINT32 i = 0;
				HINT32 ulPersonIndex = -1;

				if(pszString == NULL)
					return ERR_FAIL;

				for(i = 0; i<pEvent->ulStrCount; i++)
				{
					if(pEvent->stText[i].eType == eDxIPEPG_TEXT_PERSON)
					{
						ulPersonIndex = i;
						break;
					}
				}

				if(pEvent->ulStrCount >= eDxIPEPG_TEXT_MAXCOUNT && ulPersonIndex == -1)
					break;


				if(ulPersonIndex == -1)
				{
					ulPersonIndex = pEvent->ulStrCount;
					pEvent->ulStrCount++;
					pEvent->stText[ulPersonIndex].eType = eDxIPEPG_TEXT_PERSON;
				}

				pPersons = IPEPG_MemCalloc(pEvent->stText[ulPersonIndex].usSize + HxSTD_StrLen(pszString) + 1/*;*/ + 1/*NULL*/);

				if(pEvent->stText[ulPersonIndex].ucStr)
					strncat(pPersons, pEvent->stText[ulPersonIndex].ucStr, pEvent->stText[ulPersonIndex].usSize);

				if(pszString) strncat(pPersons, pszString, HxSTD_StrLen(pszString));
				strncat(pPersons, ";", 1);

				IPEPG_MemFree(pEvent->stText[ulPersonIndex].ucStr);
				pEvent->stText[ulPersonIndex].ucStr = pPersons;
				pEvent->stText[ulPersonIndex].usSize = HxSTD_StrLen(pPersons);
				//HxLOG_Print("PERSON INDEX[%d] STRING[%s]\n",ulPersonIndex, pEvent->stText[ulPersonIndex].ucStr);
			}
			break;

		case IPEPG_ATTR_PROGRAM_SERIESLINK_PROGID :
				if(pEvent->ulLinkCount < eDxIPEPG_LINK_MAXCOUNT){
					pEvent->stLink[pEvent->ulLinkCount].ulProgId = ulDecimal;
					// LINK의 개수를 먼저 증가 시킨다. 따라서 배열에 접근할때는 -1을 뺀다.
					pEvent->ulLinkCount++;
				}
				break;
		case IPEPG_ATTR_PROGRAM_SERIESLINK_STARTTIME :
			if(pEvent->ulLinkCount> 0 && pEvent->ulLinkCount <= eDxIPEPG_LINK_MAXCOUNT)
				pEvent->stLink[pEvent->ulLinkCount-1].ulStartTime = ulDecimal;
			break;
		case IPEPG_ATTR_PROGRAM_SERIESLINK_ENDTIME :
			if(pEvent->ulLinkCount> 0 && pEvent->ulLinkCount <= eDxIPEPG_LINK_MAXCOUNT)
				pEvent->stLink[pEvent->ulLinkCount-1].ulEndTime = ulDecimal;
			break;
		case IPEPG_ATTR_PROGRAM_SERIESLINK_EPISODE :
			if(pEvent->ulLinkCount> 0 && pEvent->ulLinkCount <= eDxIPEPG_LINK_MAXCOUNT)
				pEvent->stLink[pEvent->ulLinkCount-1].ulEpisodeNum = ulDecimal;
			break;
		case IPEPG_ATTR_PROGRAM_SERIESLINK_SEASON :
			if(pEvent->ulLinkCount> 0 && pEvent->ulLinkCount <= eDxIPEPG_LINK_MAXCOUNT)
				pEvent->stLink[pEvent->ulLinkCount-1].ulSeason = ulDecimal;
			break;
		case IPEPG_ELEM_PROGRAM_SERIESLINK_NAME :
			if(pEvent->ulLinkCount> 0 && pEvent->ulLinkCount <= eDxIPEPG_LINK_MAXCOUNT)
			{
				pIpEventStr = &pEvent->stLink[pEvent->ulLinkCount-1].stSubName;
				local_ipepg_IpEventStrDup(pIpEventStr, eDxIPEPG_TEXT_SUBNAME, pszString);
			}
			break;
		case IPEPG_ATTR_PROGRAM_SERIESLINK_NAME :
			if(pEvent->ulLinkCount> 0 && pEvent->ulLinkCount <= eDxIPEPG_LINK_MAXCOUNT)
			{
				strncpy(pEvent->stLink[pEvent->ulLinkCount-1].stSubName.ucLang, pszString, EPG_HUMAX_LANG_LEN);
			}
			break;

			// <MEDIAS>
			// <MEDIA>
		case IPEPG_ELEM_PROGRAM_MEDIAS :
		case IPEPG_ELEM_PROGRAM_MEDIA :
			break;

		case IPEPG_ATTR_PROGRAM_MEDIA_ID :	// <MEDIA ID="SBS">
			if(NULL != pszString && pEvent->ulMediaCount < eDxIPEPG_MEDIA_MAXCOUNT)
			{
				HxLOG_Print("[%s] [IPEPG_ATTR_PROGRAM_MEDIA_ID] stMedia[%d] STRING[%s] Decimal[%d]\n", __FUNCTION__ , pEvent->ulMediaCount, pszString, ulDecimal);
				pMedia = &pEvent->stMedia[pEvent->ulMediaCount];
				local_ipepg_IpEventStrDup(&pMedia->stMediaId, eDxIPEPG_TEXT_NONE, pszString);

				// 미디어의 개수를 먼저 증가 시킨다. 따라서 배열에 접근할때는 -1을 뺀다.
				pEvent->ulMediaCount++;
			}
			break;

			// <MEDIAURLS>
			// <MEDIAURL> =========================================
		case IPEPG_ELEM_PROGRAM_MEDIA_URL :
			if( pEvent->ulMediaCount > 0 && pEvent->ulMediaCount <= eDxIPEPG_MEDIA_MAXCOUNT && NULL != pszString)
			{
				HxLOG_Print("[%s] [IPEPG_ELEM_PROGRAM_MEDIA_URL] mediaCount[%d] STRING[%s] \n", __FUNCTION__ , pEvent->ulMediaCount, pszString);
				pMedia = &pEvent->stMedia[ pEvent->ulMediaCount -1];
				if( pMedia->ulMediaUrlCount < eDxIPEPG_MEDIAURL_MAXCOUNT)
				{
					pIpEventStr = &pMedia->stMediaUrl[ pMedia->ulMediaUrlCount ].stUrl;
					local_ipepg_IpEventStrDup(pIpEventStr , eDxIPEPG_TEXT_NONE, pszString);
					// MEDIAURL의 개수를 가장 마지막에 증가 시킨다.
					pMedia->ulMediaUrlCount++;
				}
			}
			break;
			// <MEDIAURL TYPE>
		case IPEPG_ATTR_PROGRAM_MEDIA_TYPE :
			if( pEvent->ulMediaCount > 0 && pEvent->ulMediaCount <= eDxIPEPG_MEDIA_MAXCOUNT && NULL != pszString)
			{
				//HxLOG_Print("[%s] [IPEPG_ATTR_PROGRAM_MEDIA_TYPE] mediaCount[%d] STRING[%s] \n", __FUNCTION__ , pEvent->ulMediaCount, pszString);
				pMedia = &pEvent->stMedia[pEvent->ulMediaCount-1];
				if( pMedia->ulMediaUrlCount < eDxIPEPG_MEDIAURL_MAXCOUNT)
				{
					pIpEventStr = &pMedia->stMediaUrl[ pMedia->ulMediaUrlCount ].stType;
					local_ipepg_IpEventStrDup(pIpEventStr , eDxIPEPG_TEXT_NONE, pszString);

					HxLOG_Print("[%s] [IPEPG_ATTR_PROGRAM_MEDIA_TYPE] stMedia[%d] STRING[%s]\n",__FUNCTION__ ,
						pEvent->ulMediaCount, pIpEventStr->ucStr);
				}
			}
			break;
			// <MEDIAURL TARGET>
		case IPEPG_ATTR_PROGRAM_MEDIA_TARGET :
			if( pEvent->ulMediaCount > 0 && pEvent->ulMediaCount <= eDxIPEPG_MEDIA_MAXCOUNT && NULL != pszString)
			{
				HxLOG_Print("[%s] [IPEPG_ATTR_PROGRAM_MEDIA_TARGET] mediaCount[%d] STRING[%s] \n", __FUNCTION__ , pEvent->ulMediaCount, pszString);
				pMedia = &pEvent->stMedia[pEvent->ulMediaCount-1];
				if(pMedia->ulMediaUrlCount < eDxIPEPG_MEDIAURL_MAXCOUNT)
				{
					pIpEventStr = &pMedia->stMediaUrl[ pMedia->ulMediaUrlCount ].stTarget;
					local_ipepg_IpEventStrDup(pIpEventStr , eDxIPEPG_TEXT_NONE, pszString);

					HxLOG_Print("[%s] [IPEPG_ATTR_PROGRAM_MEDIA_TARGET] stMedia[%d] String[%s] \n", __FUNCTION__,
						pEvent->ulMediaCount , pIpEventStr->ucStr );
				}
			}
			break;
			// </MEDIAURL> =========================================

			// <IMAGES> ==========================================
		case IPEPG_ELEM_PROGRAM_MEDIA_IMAGES :
			break;
		case IPEPG_ELEM_PROGRAM_MEDIA_IMAGE :
			if( pEvent->ulMediaCount > 0 && pEvent->ulMediaCount <= eDxIPEPG_MEDIA_MAXCOUNT && NULL != pszString)
			{
				pMedia = &pEvent->stMedia[ pEvent->ulMediaCount-1 ];
				if( pMedia->ulMediaImageCount < eDxIPEPG_MEDIAIMAGE_MAXCOUNT )
				{
					pIpEventStr = &pMedia->stMediaImage[ pMedia->ulMediaImageCount ].stImageUrl;
					local_ipepg_IpEventStrDup(pIpEventStr , eDxIPEPG_TEXT_NONE, pszString);

					// MEDIAIMAGE의 개수를 가장 마지막에 증가 시킨다.
					pMedia->ulMediaImageCount++;
				}
			}
			break;
		case IPEPG_ATTR_PROGRAM_MEDIA_IMAGE_SIZE :
			if( pEvent->ulMediaCount > 0 && pEvent->ulMediaCount <= eDxIPEPG_MEDIA_MAXCOUNT )
			{
				pMedia = &pEvent->stMedia[ pEvent->ulMediaCount-1 ];
				if(pMedia->ulMediaImageCount < eDxIPEPG_MEDIAIMAGE_MAXCOUNT)
				{
					pMedia->stMediaImage[ pMedia->ulMediaImageCount ].ulUrlSize = ulDecimal;
				}
			}
			break;
		case IPEPG_ATTR_PROGRAM_MEDIA_IMAGE_TYPE :
			if( pEvent->ulMediaCount > 0 && pEvent->ulMediaCount <= eDxIPEPG_MEDIA_MAXCOUNT && NULL != pszString)
			{
				pMedia = &pEvent->stMedia[ pEvent->ulMediaCount-1 ];
				if(pMedia->ulMediaImageCount < eDxIPEPG_MEDIAIMAGE_MAXCOUNT)
				{
					pIpEventStr = &pMedia->stMediaImage[ pMedia->ulMediaImageCount ].stUrlType;
					local_ipepg_IpEventStrDup(pIpEventStr , eDxIPEPG_TEXT_NONE, pszString);
				}
			}
			break;
			// </IMAGES> =========================================


		case IPEPG_ELEM_PROGRAM_MEDIA_PUBDATE :
			HxLOG_Print("[%s] [IPEPG_ELEM_PROGRAM_MEDIA_PUBDATE] mediaCount[%d] Dec[%d] \n", __FUNCTION__ , pEvent->ulMediaCount, ulDecimal);
			if(pEvent->ulMediaCount > 0 && pEvent->ulMediaCount <= eDxIPEPG_MEDIA_MAXCOUNT )
			{
				pEvent->stMedia[pEvent->ulMediaCount-1].ulPubDate = ulDecimal;
			}
			break;
		case IPEPG_ELEM_PROGRAM_MEDIA_EXPIREDATE :
			HxLOG_Print("[%s] [IPEPG_ELEM_PROGRAM_MEDIA_EXPIREDATE] mediaCount[%d] Dec[%d] \n", __FUNCTION__ , pEvent->ulMediaCount, ulDecimal);
			if(pEvent->ulMediaCount > 0 && pEvent->ulMediaCount <= eDxIPEPG_MEDIA_MAXCOUNT )
			{
				pEvent->stMedia[pEvent->ulMediaCount-1].ulExPireDate = ulDecimal;
			}
			break;
		case IPEPG_ELEM_PROGRAM_MEDIA_TRANSMITDATE :
			if(pEvent->ulMediaCount > 0 && pEvent->ulMediaCount <= eDxIPEPG_MEDIA_MAXCOUNT )
			{
				HxLOG_Print("[%s] [IPEPG_ELEM_PROGRAM_MEDIA_TRANSMITDATE] mediaCount[%d] Dec[%d] \n", __FUNCTION__ , pEvent->ulMediaCount, ulDecimal);
				pEvent->stMedia[pEvent->ulMediaCount-1].ulTransmitDate = ulDecimal;
			}
			break;
		case IPEPG_ELEM_PROGRAM_MEDIA_PRICE :
			if(pEvent->ulMediaCount > 0 && pEvent->ulMediaCount <= eDxIPEPG_MEDIA_MAXCOUNT )
			{
				pEvent->stMedia[pEvent->ulMediaCount-1].ulPrice = ulDecimal;
			}
			break;
		// </MEDIA> =======================================================================
		case IPEPG_ELEM_PROGRAM_SERIESLINKS :
		case IPEPG_ELEM_PROGRAM_SERIESLINK :
		case IPEPG_ELEM_PROGRAM_CODECS :
		case IPEPG_ELEM_PROGRAM_CODEC :
		case IPEPG_ATTR_PROGRAM_CODEC_TYPE :
		case IPEPG_ELEM_CHANNEL_IMAGE :
		case IPEPG_ATTR_CHANNEL_IMAGESIZE :
		case IPEPG_ATTR_CHANNEL_IMAGETYPE :
		case IPEPG_ATTR_CHANNEL_UPDATETYPE :
			break;
		default :
			break;
	}
	return ERR_OK;
}


#define IP_ATTR_STRING_MAX	1024
#define	IP_CONTENTS_STRING_MAX 256

static HERROR		local_ipepg_ParseAttribute(HxXMLELEMENT *pElement, IPEPG_ELEMENT_t *pstElement, void *target, local_ipepg_SetData_t func)
{
	HERROR					hError = ERR_FAIL;
	HUINT32					ulIndex = 0;
	HUINT8					* pszAttrContents = NULL;
	IPEPG_ATTR_t 		* attributeList = NULL;

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

			hError = ipepg_Humax_SetDataByType(attributeList[ulIndex].ulAttrDataType, pszAttrContents, EPG_HUMAX_NAME_LEN, szText, &ulValue, &ulRealLen);
			if(hError != ERR_OK)
			{
				HxLOG_Debug("[%s][%d] ipepg_Humax_SetDataByType\n",__FUNCTION__,__LINE__);
			}

			if(func)
			{
				hError = func(target, attributeList[ulIndex].eDataIdx,ulRealLen, szText, ulValue);
				if(hError != ERR_OK)
				{
					HxLOG_Debug("[%s][%d] func error\n",__FUNCTION__,__LINE__);
				}
			}
		}

		if(pszAttrContents)	HLIB_XML_FreeAttrContents(pszAttrContents);
		pszAttrContents = NULL;
	}
	while(!(attributeList[ulIndex++].bFlagEnd));
	return ERR_OK;
}

static HERROR		local_ipepg_ParseElement(HxXMLDOC hDocHandle, HxXMLELEMENT pElement, IPEPG_ELEMENT_t *pstElement, void *target, local_ipepg_SetData_t func)
{
	IPEPG_ELEMENT_t *pstSubElement = NULL;
	HxList_t		*pChildList = NULL;
	HxList_t		*pBackupChildList = NULL;
	HERROR			hError = ERR_FAIL;

	IPEPG_RETURN_IF(pElement == NULL || pstElement == NULL || target == NULL, ERR_FAIL);

	local_ipepg_ParseAttribute(pElement, pstElement, target, func);
	if(HLIB_XML_GetChildElementList(pElement, &pChildList) == ERR_FAIL)
	{
		if(pChildList) HLIB_LIST_RemoveAll(pChildList);
		return ERR_FAIL;
	}
	pstSubElement = pstElement->ElementList;

	pBackupChildList = pChildList;

	while(pstSubElement != NULL && pChildList != NULL)
	{
		HxXMLELEMENT		hChildElement = NULL;
		IPEPG_ELEMENT_t *pstSameElement = NULL;
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

		IPEPG_GOTO_IF(pstSameElement == NULL, EXIT_EVENT_LOOP);

		if(pstSameElement->ulTagDataType == IPEPG_TYPE_NONE)
		{
			hError = func(target, pstSameElement->eDataIdx, 0, NULL, 0);
			if(hError != ERR_OK)
			{
				HxLOG_Debug("[%s][%d] func error\n",__FUNCTION__,__LINE__);
			}

			local_ipepg_ParseElement(hDocHandle, hChildElement, pstSameElement, target, func);
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
			hError = ipepg_Humax_SetDataByType(pstSameElement->ulTagDataType, pContents, EPG_HUMAX_DETAIL_LEN, szText, &ulValue, &ulRealLen);
			if(hError != ERR_OK)
			{
				HxLOG_Debug("[%s][%d] ipepg_Humax_SetDataByType\n",__FUNCTION__,__LINE__);
			}

			if(func)
			{
				hError = func(target, pstSameElement->eDataIdx,ulRealLen, szText, ulValue);
				if(hError != ERR_OK)
				{
					HxLOG_Debug("[%s][%d] func error\n",__FUNCTION__,__LINE__);
				}
			}
			if(pContents) HLIB_XML_FreeAttrContents(pContents);

		}

EXIT_EVENT_LOOP :
		pChildList = pChildList->next;
	}
	HLIB_LIST_RemoveAll(pBackupChildList);
	return ERR_OK;
}


HERROR		IPEPG_HumaxParseServiceList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ListOfSvcInfo)
{
	HERROR					hError = ERR_FAIL;
	HxXMLDOC				hXmlDocHandle = NULL;
	IPEPG_ELEMENT_t	*pstElement = NULL;
	HxXMLELEMENT			hResponseElement = NULL;
	HxXMLELEMENT			hSvcDataListElement = NULL;
	HxXMLELEMENT			hSvcDataElement = NULL;
	HxList_t				*pSvcDataList = NULL;
	HxList_t				*pSvcData = NULL;
	HxList_t				*pBackupSvcData = NULL;

	HxList_t				*pTempSvcList = NULL;


	IPEPG_GOTO_IF(pRawBuffer == NULL || ListOfSvcInfo == NULL || ulBufferSize == 0,EXIT_EVENT_FUNC);
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


	// GET Element Response
	IPEPG_GOTO_IF(pstElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_GetRootElement(hXmlDocHandle, &hResponseElement) == ERR_FAIL,  EXIT_EVENT_FUNC);

	// GET Element EpgDataList
	pstElement = pstElement->ElementList;
	IPEPG_GOTO_IF(pstElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_FindChildElementByName(hResponseElement, pstElement->pucTagName, &pSvcDataList) == ERR_FAIL, EXIT_EVENT_FUNC);
	hSvcDataListElement =(HxXMLELEMENT)HLIB_LIST_Data(pSvcDataList);

	// GET Element SvcData
	pstElement = pstElement->ElementList;
	IPEPG_GOTO_IF(pstElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_FindChildElementByName(hSvcDataListElement, pstElement->pucTagName, &pSvcData) == ERR_FAIL,	EXIT_EVENT_FUNC);

	 pBackupSvcData = pSvcData;
	// IPEPG_EPG_EPGDATA
	while(pSvcData != NULL)
	{
		EPG_URL_INFO_t * pIpSvcInfo = NULL;
		hSvcDataElement = (HxXMLELEMENT)HLIB_LIST_Data(pSvcData);
		//HxLOG_Print("[%s] [%d] %s\n",__FUNCTION__, __LINE__, pstElement->pucTagName);

		if(hSvcDataElement != NULL)
		{
			pIpSvcInfo = (EPG_URL_INFO_t*)IPEPG_MemAlloc(sizeof(EPG_URL_INFO_t));
			IPEPG_GOTO_IF(pIpSvcInfo == NULL, EXIT_EVENT_LOOP);
			HxSTD_MemSet(pIpSvcInfo, 0x00, sizeof(EPG_URL_INFO_t));
			IPEPG_GOTO_IF(local_ipepg_ParseElement(hXmlDocHandle, hSvcDataElement, pstElement, pIpSvcInfo, ipepg_Humax_SetServiceData) == ERR_FAIL, EXIT_EVENT_LOOP);
		}
EXIT_EVENT_LOOP :

		if(pIpSvcInfo != NULL)
		{
#if defined(CONFIG_FUNC_EPG_USE_SVCUID)
			pTempSvcList = HLIB_LIST_Append(pTempSvcList, pIpSvcInfo);
#else
			if(HLIB_LIST_FindEx(pTempSvcList, (void *)&(pIpSvcInfo->stTripleId), ipepg_CompareTripleId) == NULL)
			{
				pTempSvcList = HLIB_LIST_Append(pTempSvcList, pIpSvcInfo);
			}
			else
			{
				IPEPG_MemFree(pIpSvcInfo);
			}
#endif
		}
		pSvcData = pSvcData->next;
	}



EXIT_EVENT_FUNC :
	HLIB_LIST_RemoveAll(pBackupSvcData);
	HLIB_LIST_RemoveAll(pSvcDataList);

	if(hXmlDocHandle != NULL)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}

	if(pTempSvcList != NULL)
	{
		*ListOfSvcInfo = pTempSvcList;
	}
	return hError;
}



void	TEST_Function(HUINT32 _line, HCHAR * TagName, HxXMLELEMENT taget)
{
#if 0
	HCHAR * 	pucTestName = NULL;
	HLIB_XML_GetElementName(taget, (HUINT8**)&pucTestName);
	if(pucTestName != NULL && TagName != NULL)
		HxLog_Print("=========================> LINE[%d] TagName[%s] EleName[%s]\n", _line, TagName, pucTestName);
	else if(pucTestName != NULL)
		HxLog_Print("=========================> LINE[%d] EleName[%s]\n", _line, pucTestName);
	else
		HxLog_Print("=========================> LINE[%d] TagName[%s]\n", _line, TagName);
#endif

}

HERROR		IPEPG_HumaxParseEventList(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HxList_t **ppEventList, HINT32 ulSvcUid)
{
	HERROR					hError = ERR_FAIL;
	HxXMLDOC				hXmlDocHandle = NULL;
	IPEPG_ELEMENT_t			*pstElement = NULL;
	IPEPG_ELEMENT_t			*pstTempElement = NULL;
	IPEPG_ATTR_t 			*pstAttribute = NULL;

	HUINT8					* pszAttrContents = NULL;

	HxXMLELEMENT 			hElement = NULL;
	HxXMLELEMENT 			hChannelInfoElement = NULL;

	HxList_t 				*pChannels = NULL;
	HxList_t 				*pChannel = NULL;

	HxList_t				*pTempEventList = NULL;

	HUINT32					ulHSVC = 0, ulCHANNELID = 0, ulONID = 0, ulTSID = 0, ulSID = 0;
	HUINT32					i = 0;

	IPEPG_EVENT_TYPE_e		eUpdate = eIPEPG_EVENT_TYPE_MAX;

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


	// GET Element IPEPG
	IPEPG_GOTO_IF(pstElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_GetRootElement(hXmlDocHandle, &hElement) == ERR_FAIL,  EXIT_EVENT_FUNC);
	TEST_Function(__LINE__, pstElement->pucTagName, hElement);


	// GET Element CHANNELS	// depth 1
	pstElement = pstElement->ElementList;
	IPEPG_GOTO_IF(pstElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_FindChildElementByName(hElement, pstElement->pucTagName, &pChannels) == ERR_FAIL, EXIT_EVENT_FUNC);
	hElement =(HxXMLELEMENT)HLIB_LIST_Data(pChannels);
	TEST_Function(__LINE__, pstElement->pucTagName, hElement);


	// GET Element CHANNEL	// depth 2
	pstElement = pstElement->ElementList;
	IPEPG_GOTO_IF(pstElement == NULL , EXIT_EVENT_FUNC);
	IPEPG_GOTO_IF(HLIB_XML_FindChildElementByName(hElement, pstElement->pucTagName, &pChannel) == ERR_FAIL, EXIT_EVENT_FUNC);
	hElement =(HxXMLELEMENT)HLIB_LIST_Data(pChannel);
	TEST_Function(__LINE__, pstElement->pucTagName, hElement);


	pstAttribute = pstElement->AttributeList;
	// GET Atrribute HSVC
	if(HLIB_XML_GetAttrContents(hElement, pstAttribute[0].pucAttrName, &pszAttrContents) == ERR_OK)
		hError = ipepg_Humax_SetDataByType(pstAttribute[0].ulAttrDataType, pszAttrContents, EPG_HUMAX_NAME_LEN, 0, &ulHSVC, 0);

	if(pszAttrContents)	HLIB_XML_FreeAttrContents(pszAttrContents);
	pszAttrContents = NULL;

	// GET Atrribute CHANNELID
	if(HLIB_XML_GetAttrContents(hElement, pstAttribute[1].pucAttrName, &pszAttrContents) == ERR_OK)
		hError = ipepg_Humax_SetDataByType(pstAttribute[1].ulAttrDataType, pszAttrContents, EPG_HUMAX_NAME_LEN, 0, &ulCHANNELID, 0);

	if(pszAttrContents)	HLIB_XML_FreeAttrContents(pszAttrContents);
	pszAttrContents = NULL;

	// GET Element CHANNEL INFO depth 3
	pstElement = pstElement->ElementList;
	IPEPG_GOTO_IF(pstElement == NULL , EXIT_EVENT_FUNC);

	if(pstElement!= NULL)
	{
		do
		{
			// GET Element CHANNEL INFO depth 3
			// IPEPG_ELEM_CHANNEL_NUM, IPEPG_ELEM_CHANNEL_NAME, IPEPG_ELEM_CHANNEL_TRIPLEIDS, IPEPG_ELEM_CHANNEL_IMAGES, IPEPG_ELEM_CHANNEL_UPDATE
			HxList_t	*pChannelInfo = NULL;
			if(HLIB_XML_FindChildElementByName(hElement, pstElement[i].pucTagName, &pChannelInfo) == ERR_FAIL )
			{
				HxLOG_Info("[%s][%d] index:%d, TagName=%s \n", __FUNCTION__, __LINE__, i , pstElement[i].pucTagName);
				//goto EXIT_EVENT_FUNC;
			}
			else
			{
				hChannelInfoElement =(HxXMLELEMENT)HLIB_LIST_Data(pChannelInfo);
				TEST_Function(__LINE__, pstElement[i].pucTagName, hChannelInfoElement);
				pstTempElement = pstElement[i].ElementList;

				switch(pstElement[i].eDataIdx)
				{
					case 	IPEPG_ELEM_CHANNEL_NUM :
						break;
					case	IPEPG_ELEM_CHANNEL_NAME :
						break;
					case	IPEPG_ELEM_CHANNEL_TRIPLEIDS :
						{
							HxXMLELEMENT	hChannelTripleElement = NULL;
							HxList_t		*pChannelTriple = NULL;

							// GET Element TripleId depth 4
							//IPEPG_GOTO_IF(HLIB_XML_FindChildElementByName(hChannelInfoElement, pstTempElement->pucTagName, &pChannelTriple) == ERR_FAIL, EXIT_EVENT_FUNC);
							if(HLIB_XML_FindChildElementByName(hChannelInfoElement, pstTempElement->pucTagName, &pChannelTriple) == ERR_FAIL)
							{
								if(pChannelInfo)
									HLIB_LIST_RemoveAll(pChannelInfo);
								goto EXIT_EVENT_FUNC;
							}
							hChannelTripleElement =(HxXMLELEMENT)HLIB_LIST_Data(pChannelTriple);
							TEST_Function(__LINE__, pstTempElement->pucTagName, hChannelTripleElement);

							pstAttribute = pstTempElement->AttributeList;

							// GET Atrribute ONID
							if(HLIB_XML_GetAttrContents(hChannelTripleElement, pstAttribute[0].pucAttrName, &pszAttrContents) == ERR_OK)
								hError = ipepg_Humax_SetDataByType(pstAttribute[0].ulAttrDataType, pszAttrContents, EPG_HUMAX_NAME_LEN, 0, &ulONID, 0);

							if(pszAttrContents) HLIB_XML_FreeAttrContents(pszAttrContents);
							pszAttrContents = NULL;

							// GET Atrribute TSID
							if(HLIB_XML_GetAttrContents(hChannelTripleElement, pstAttribute[1].pucAttrName, &pszAttrContents) == ERR_OK)
								hError = ipepg_Humax_SetDataByType(pstAttribute[1].ulAttrDataType, pszAttrContents, EPG_HUMAX_NAME_LEN, 0, &ulTSID, 0);

							if(pszAttrContents) HLIB_XML_FreeAttrContents(pszAttrContents);
							pszAttrContents = NULL;

							// GET Atrribute SID
							if(HLIB_XML_GetAttrContents(hChannelTripleElement, pstAttribute[2].pucAttrName, &pszAttrContents) == ERR_OK)
								hError = ipepg_Humax_SetDataByType(pstAttribute[1].ulAttrDataType, pszAttrContents, EPG_HUMAX_NAME_LEN, 0, &ulSID, 0);

							if(pszAttrContents) HLIB_XML_FreeAttrContents(pszAttrContents);
							pszAttrContents = NULL;

							//HxLOG_Print("TRIPLE ID : ONID:%d TSID:%d SID:%d\n", ulONID, ulTSID, ulSID);
							HLIB_LIST_RemoveAll(pChannelTriple);
						}
						break;
					case	IPEPG_ELEM_CHANNEL_IMAGES :
						break;
					case	IPEPG_ELEM_CHANNEL_UPDATE :
						{
							HxList_t				*pChannelUpdate = NULL;
							// GET Element UPDATETYPE depth 4
							pChannelUpdate = pChannelInfo;

							while(pChannelUpdate)
							{
								HUINT32 ulRealLen = 0;
								HUINT8	szText[EPG_HUMAX_DETAIL_LEN + 1] = {0, };
								HxXMLELEMENT 			hProgramTempElement = NULL;
								HxList_t				*pPrograms = NULL;

								hProgramTempElement =(HxXMLELEMENT)HLIB_LIST_Data(pChannelUpdate);

								TEST_Function(__LINE__, pstElement[i].pucTagName, hProgramTempElement);

								pstAttribute = pstElement[i].AttributeList;

								if(HLIB_XML_GetAttrContents(hProgramTempElement, pstAttribute[0].pucAttrName, &pszAttrContents) == ERR_OK)
								{
									hError = ipepg_Humax_SetDataByType(pstAttribute[0].ulAttrDataType, pszAttrContents, EPG_HUMAX_DETAIL_LEN, szText, 0, &ulRealLen);
									if(strcmp(szText, "UPDATE") == 0)
										eUpdate = eIPEPG_EVENT_TYPE_UPDATE;
									else
										eUpdate = eIPEPG_EVENT_TYPE_DELETE;
								}
								if(pszAttrContents) HLIB_XML_FreeAttrContents(pszAttrContents);
								pszAttrContents = NULL;


								// GET Element Programs
								if(pstTempElement == NULL)
									break;
								if(HLIB_XML_GetChildElementList(hProgramTempElement, &pPrograms) == ERR_OK)
								{
									hProgramTempElement =(HxXMLELEMENT)HLIB_LIST_Data(pPrograms);
									TEST_Function(__LINE__, pstTempElement->pucTagName, hProgramTempElement);
									pstTempElement = pstTempElement->ElementList;


									// GET Element Program List
									if(pstTempElement)
									{
										HxList_t				*pProgram = NULL;
										HxList_t				*pFreeProgram = NULL;
										if(HLIB_XML_GetChildElementList(hProgramTempElement, &pProgram) == ERR_OK)
										{
											pFreeProgram = pProgram;
											// IPEPG_EPG_EPGDATA
											while(pProgram != NULL)
											{
												IPEPG_UpdateEvent_t * UpdateEvent = NULL;
												DxIpEvent_t * pIpEvent = NULL;
												hProgramTempElement = (HxXMLELEMENT)HLIB_LIST_Data(pProgram);

												if(hProgramTempElement != NULL)
												{
													UpdateEvent = (IPEPG_UpdateEvent_t*)IPEPG_MemCalloc(sizeof(IPEPG_UpdateEvent_t));
													pIpEvent = (DxIpEvent_t*)IPEPG_MemCalloc(sizeof(DxIpEvent_t));
													IPEPG_GOTO_IF(UpdateEvent == NULL || pIpEvent == NULL, EXIT_EVENT_LOOP);
													UpdateEvent->pIpEvent = pIpEvent;

													IPEPG_GOTO_IF(local_ipepg_ParseElement(hXmlDocHandle, hProgramTempElement, pstTempElement, pIpEvent, (local_ipepg_SetData_t)local_ipepg_SetEventData) == ERR_FAIL, EXIT_EVENT_LOOP);
												}
										EXIT_EVENT_LOOP :
												if(UpdateEvent != NULL && pIpEvent != NULL)
												{
													pIpEvent->stBase.type = eDxEPG_TYPE_IP;
													pIpEvent->stBase.ulSvcUid = ulSvcUid;
													pIpEvent->stBase.usOnId = (HUINT16)ulONID;
													pIpEvent->stBase.usTsId = (HUINT16)ulTSID;
													pIpEvent->stBase.usSvcId = (HUINT16)ulSID;
													UpdateEvent->eUpdateType = eUpdate;
													pTempEventList = HLIB_LIST_Append(pTempEventList, UpdateEvent);
												}
												else
												{
													if(UpdateEvent != NULL)
														IPEPG_MemFree(UpdateEvent);
													if(pIpEvent != NULL)
														IPEPG_MemFree(pIpEvent);
												}

												pProgram = pProgram->next;
											}

											if(pFreeProgram) HLIB_LIST_RemoveAll(pFreeProgram);
										}
										else
										{
											if(pProgram) HLIB_LIST_RemoveAll(pProgram);
										}
									}
								}
								if(pPrograms) HLIB_LIST_RemoveAll(pPrograms);
								pChannelUpdate = HLIB_LIST_Next(pChannelUpdate);
							}
						}
						break;
					default :
						break;
				}
				HLIB_LIST_RemoveAll(pChannelInfo);
			}
		}while(!(pstElement[i++].bFlagEnd));
	}

EXIT_EVENT_FUNC :

	if(pChannel) HLIB_LIST_RemoveAll(pChannel);
	if(pChannels) HLIB_LIST_RemoveAll(pChannels);
	if(hXmlDocHandle != NULL)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}

	*ppEventList = pTempEventList;
	return hError;
}
/*********************** End of File ******************************/


