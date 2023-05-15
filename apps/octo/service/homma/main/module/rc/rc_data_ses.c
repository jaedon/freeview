/* ==========================================================================
@ filename :    rc_data_ses.c
@ brief	   : 	Remote Programming Data Process application
@ author   :

Copyright (c) 2008 HUMAX Co., Ltd.				\n
All rights reserved.							\n
=============================================================================*/


/*=============================================================================*/
/*----------------------------   HEADER FILE       ----------------------------*/
/*=============================================================================*/

//#include "octo_common.h"
//#include "mw_fs.h"
//#include "db_rsv.h"
//#include "mw_pvr.h"
//#include "mw_rsv.h"
//#include "mw_epg.h"
//#include "mwc_fs.h"
//#include "mwc_util.h"
//#include "epg_common_lib.h"
//#include "isosvc.h"
//#include "rc_freesat_data.h"
//#include "app.h"

#include "rc_data_ses.h"
//#include <directfb.h>

//#include <hapi.h>
//#include	<mwc_param.h>
//#include <ap_zapping.h>

//#include <ox_epg.h>
//#include <octo_module.h>
//#include <pal_xml.h>
//#include <ap_ota.h>
//#include <ap_mplayer.h>
/*=============================================================================*/
/*----------------------------       Global        ----------------------------*/
/*=============================================================================*/

#define	STRING_LEN_MAX					256
#define	STRING_URI_MAX					2048
#define	STRING_NAME_MAX					512
#define	PACKET_MAX_BUF_SIZE				(8192*4)
#define TRANSPORT_MAX_BUF_SIZE		(1024*100)
#define	FS_MAX_POSTCODE_LEN			9
#define	EVT_NAME_LEN_MAX				64
#define	SERVICE_ID_LEN_MAX				20
//#define IS_DEVICETYPE_HDD(Type)					(((Type==eMW_FS_DevType_IntHdd) || (Type==eMW_FS_DevType_ExtHdd)) ? TRUE : FALSE)
#define	FSATCOMMAND_EVENT_FORMAT	"{\"MSGTYPE\":\"%s\", \"PARAMS\":{%s}}"
//#define IS_WATCHING(Type)						(((Type==RSV_TYPE_WATCHING_TBR) || (Type==RSV_TYPE_WATCHING_EBR)) ? TRUE : FALSE)
//#define IS_REC(Type)							(((Type==RSV_TYPE_RECORD_TBR)   || (Type==RSV_TYPE_RECORD_EBR)) ? TRUE : FALSE)
//#define GET_DATA(a, b, c) 						(IS_WATCHING(a) ? b : (IS_REC(a) ? c : 0));

#define RECDATA_MAX_NAME_LEN		128
#define MAX_RC_ATTRIBUTE_LEN		30

#define MAX_EVENT_LOG_CNT			65535
#define GET_NEXT_CHANGE_ID(x)		((++x) % MAX_EVENT_LOG_CNT)

#define MAX_AUD_NUM					4
#define RC_RESOURCE_MAX_BUF_SIZE		(8192)
#define RC_RESOURCE_SHORT_MAX_BUF_SIZE	(2048)

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))


/*=============================================================================*/
/*----------------------------       Typedef       ----------------------------*/
/*=============================================================================*/

//#define 		RC_REMOTE_KEY_NUM 		38

RC_REMOTE_KeyMap_Element_t	s_stRC_RemoteKeyMap[] =
{
	{"VK_UP", DIKS_CURSOR_UP},
	{"VK_DOWN", DIKS_CURSOR_DOWN},
	{"VK_LEFT", DIKS_CURSOR_LEFT},
	{"VK_RIGHT", DIKS_CURSOR_RIGHT},
	{"VK_OK", DIKS_ENTER},
	{"VK_PLAY", DIKS_PLAY},
	{"VK_PAUSE", DIKS_PAUSE},
	{"VK_STOP", DIKS_STOP},
	{"VK_FAST_FWD", DIKS_FASTFORWARD},
	{"VK_REWIND", DIKS_REWIND},//10
	{"VK_TRACK_NEXT", DIKS_NEXT},
	{"VK_TRACK_PREV", DIKS_PREVIOUS},
	{"VK_RECORD", DIKS_RECORD},
	{"VK_0", DIKS_0},
	{"VK_1", DIKS_1},
	{"VK_2", DIKS_2},
	{"VK_3", DIKS_3},
	{"VK_4", DIKS_4},
	{"VK_5", DIKS_5},
	{"VK_6", DIKS_6},//20
	{"VK_7", DIKS_7},
	{"VK_8", DIKS_8},
	{"VK_9", DIKS_9},
	{"VK_RED", DIKS_RED},
	{"VK_GREEN", DIKS_GREEN},
	{"VK_YELLLOW", DIKS_YELLOW},
	{"VK_BLUE", DIKS_BLUE},
	{"VK_BACK", DIKS_LAST},
	{"VK_INFO", DIKS_INFO},
	{"VK_ESCAPE", DIKS_EXIT},//30
	{"VK_TELETEXT", DIKS_TEXT},
	{"VK_CHANNEL_UP", DIKS_CHANNEL_UP},
	{"VK_CHANNEL_DOWN", DIKS_CHANNEL_DOWN},
	{"VK_SUBTITLE", DIKS_SUBTITLE},
	{"VK_AUDIO_DESCRIPTION", DIKS_AUDIO},
	{"VK_VOLUME_UP", DIKS_VOLUME_UP},
	{"VK_VOLUME_DOWN", DIKS_VOLUME_DOWN},
	{"VK_MUTE", DIKS_MUTE}//40
};

/* XML Print Type */
typedef	enum
{
	PRINT_NONE,
	PRINT_OPENING,
	PRINT_CLOSING,
	PRINT_STRING,
	PRINT_DECIMAL,
	PRINT_ATTR1,
	PRINT_ATTR1_CLOSING,
	PRINT_ATTR2,
	PRINT_ATTR2_OPENING,
	PRINT_ATTR3_CONDITIONAL,
	PRINT_ATTR3,
	PRINT_ATTR4,
	PRINT_ATTR5,
	PRINT_ATTR5_OPENING,
	PRINT_ATTR6,
	PRINT_ATTR7,
	PRINT_ATTR7_OPENING,
	PRINT_ATTR8,
	PRINT_ATTR8_OPENING,
	PRINT_ATTR9,
	PRINT_ATTR10,
	PRINT_ATTR_DECIMAL,
	PRINT_ATTR11_CONDITIONAL,
}RC_PRINT_TYPE_e;

typedef	enum
{
	DEPTH_ROOT,
	DEPTH_1,
	DEPTH_2,
	DEPTH_3,
	DEPTH_4,
}RC_DEPTH_e;

typedef	enum
{
	RC_RESPONSE_OPEN,
	RC_RESPONSE_CLOSE,
}RC_RESPONSE_e;

typedef	enum
{
	RC_SRCLISTS_RESPONSE_OPEN,
	RC_SRCLISTS_OPEN,
	RC_SRCLIST_INFO,
	RC_SRCLISTS_CLOSE,
	RC_SRCLISTS_RESPONSE_CLOSE,
}RC_SRCLISTS_e;

typedef	enum
{
	RC_SOURCES_RESPONSE_OPEN,
	RC_SOURCES_OPEN,
	RC_SOURCES_INFO,
	RC_SOURCES_CLOSE,
	RC_SOURCES_RESPONSE_CLOSE,
}RC_SOURCES_e;

typedef	enum
{
	RC_CHANNELS_RESPONSE_OPEN,
	RC_CHANNELS_OPEN,
	RC_CHANNELS_INFO,
	RC_CHANNELS_CLOSE,
	RC_CHANNELS_RESPONSE_CLOSE,
}RC_CHANNELS_e;

typedef	enum
{
	RC_LOCALE_RESPONSE_OPEN,
	RC_LOCALE_OPEN,
	RC_LOCALE_DEVICEID,
	RC_LOCALE_POSTCODE,
	RC_LOCALE_TUNERS,
	RC_LOCALE_CLOSE,
	RC_LOCALE_RESPONSE_CLOSE,
}RC_LOCALE_e;

typedef	enum
{
	RC_REMOTE_RESPONSE_OPEN,
	RC_REMOTE_OPEN,
	RC_REMOTE_KEY,
	RC_REMOTE_CLOSE,
	RC_REMOTE_RESPONSE_CLOSE,
}RC_REMOTE_e;

typedef	enum
{
	RC_POWER_RESPONSE_OPEN,
	RC_POWER_INFO,
	RC_POWER_RESPONSE_CLOSE,
}RC_POWER_e;

typedef	enum
{
	RC_TIME_RESPONSE_OPEN,
	RC_TIME_INFO,
	RC_TIME_RESPONSE_CLOSE,
}RC_TIME_e;

typedef	enum
{
	RC_OUTPUTS_RESPONSE_OPEN,
	RC_OUTPUTS_OPEN,
	RC_OUTPUT,
	RC_OUTPUTS_CLOSE,
	RC_OUTPUTS_RESPONSE_CLOSE,
}RC_OUTPUTS_e;

typedef	enum
{
	RC_OUTPUT_RESPONSE_OPEN,
	RC_OUTPUT_OPEN,
	RC_OUTPUT_SETTINGS,
	RC_OUTPUT_PROGRAMME_OPEN,
	RC_OUTPUT_COMPONENT,
	RC_OUTPUT_EIT,
	RC_OUTPUT_PROGRAMME_CLOSE,
	RC_OUTPUT_TSRBUFFER,
	RC_OUTPUT_PLAYBACK,
	RC_OUTPUT_APP,
	RC_OUTPUT_CA,
	RC_OUTPUT_CLOSE,
	RC_OUTPUT_RESPONSE_CLOSE,
}RC_OUTPUTS_ITEM_e;
/*
typedef	enum
{
	RC_OUTPUT_APP_RESPONSE_OPEN,
	RC_OUTPUT_APP_OPEN,
	RC_OUTPUT_APP_PROGRAMME,
	RC_OUTPUT_APP_SETTINGS,
	RC_OUTPUT_APP_CLOSE,
	RC_OUTPUT_APP_RESPONSE_CLOSE,
}RC_OUTPUTS_APP_e;

typedef	enum
{
	RC_OUTPUT_RECORD_RESPONSE_OPEN,
	RC_OUTPUT_RECORD_OPEN,
	RC_OUTPUT_RECORD_PROGRAMME,
	RC_OUTPUT_RECORD_SETTINGS,
	RC_OUTPUT_RECORD_PLAYBACK,
	RC_OUTPUT_RECORD_CLOSE,
	RC_OUTPUT_RECORD_RESPONSE_CLOSE,
}RC_OUTPUTS_RECORD_e;
*/

typedef	enum
{
	RC_CA_RESPONSE_OPEN,
	RC_CA_INFO,
	RC_CA_RESPONSE_CLOSE,
}RC_CA_e;


typedef	enum
{
	RC_STORAGE_RESPONSE_OPEN,
	RC_STORAGE_OPEN,
	RC_STORAGE_STORED_CONTENT_OPEN,
	RC_STORAGE_SYNOPSIS,
	RC_STORAGE_CATEGORY,
	RC_STORAGE_MEDIA_VIDEO_COMPONENT,
	RC_STORAGE_MEDIA_AUDIO_COMPONENT,
	RC_STORAGE_MEDIA_SUBTITLE_COMPONENT,
	RC_STORAGE_IMAGE,
	RC_STORAGE_STORED_CONTENT_CLOSE,
	RC_STORAGE_CLOSE,
	RC_STORAGE_RESPONSE_CLOSE,
}RC_STORAGES_e;

typedef	enum
{
	RC_SCHEDULED_RESPONSE_OPEN,
	RC_SCHEDULED_OPEN,
	RC_SCHEDULED_EVENT_BOOKING,
	RC_SCHEDULED_EVENT_BOOKING_SERIES,
	RC_SCHEDULED_CLOSE,
	RC_SCHEDULED_RESPONSE_CLOSE,
}RC_SCHEDULED_e;

typedef	enum
{
	RC_SCHEDULED_CONFLICT_RESPONSE_OPEN,
	RC_SCHEDULED_CONFLICT_OPEN,
	RC_SCHEDULED_CONFLICT_EVENT_BOOKING_OPEN,
	RC_SCHEDULED_CONFLICT_WITH_OPEN,
	RC_SCHEDULED_CONFLICT_EVENT_BOOKING,
	RC_SCHEDULED_CONFLICT_EVENT_BOOKING_SERIES,
	RC_SCHEDULED_CONFLICT_WITH_CLOSE,
	RC_SCHEDULED_CONFLICT_EVENT_BOOKING_CLOSE,
	RC_SCHEDULED_CONFLICT_CLOSE,
	RC_SCHEDULED_CONFLICT_RESPONSE_CLOSE,
}RC_CONFLICT_SCHEDULED_e;

typedef	enum
{
	RC_EVENTS_RESPONSE_OPEN,
	RC_EVENTS_OPEN,
	RC_EVENTS_DATA,
	RC_EVENTS_CLOSE,
	RC_EVENTS_RESPONSE_CLOSE,
}RC_EVENTS_e;


typedef	enum
{
	RC_OUTPUTS_NONE_RESPONSE_OPEN,
	RC_OUTPUT_NONE,
	RC_OUTPUTS_NONE_RESPONSE_CLOSE,
}RC_OUTPUT_NONE_e;

typedef	enum
{
	RC_Outputs_Type_UnKnown = -1,
	RC_Outputs_Type_None = 0,
	RC_Outputs_Type_Live,
	RC_Outputs_Type_App,
	RC_Outputs_Type_Record,
	RC_Outputs_Type_Media,
	RC_Outputs_Type_Max,
}RC_OUTPUTS_Type_e;

typedef	enum
{
	RC_ELEMENT_RESPONSE = -1,
	RC_ELEMENT_LIST_SRCLISTS,
	RC_ELEMENT_LIST_SOURCES,
	RC_ELEMENT_LIST_CHANNELS,
	RC_ELEMENT_LIST_LOCALE,
	RC_ELEMENT_LIST_REMOTE,
	RC_ELEMENT_LIST_POWER,
	RC_ELEMENT_LIST_OUTPUTS,
	RC_ELEMENT_LIST_OUTPUT_ITEM,
	/*
	RC_ELEMENT_LIST_OUTPUT_3RDPARTY_APP,
	RC_ELEMENT_LIST_OUTPUT_RECORDING,
	*/
	RC_ELEMENT_LIST_CA,
	RC_ELEMENT_LIST_TIME,
	RC_ELEMENT_LIST_STORAGE,
	RC_ELEMENT_LIST_SCHEDULED,
	RC_ELEMENT_LIST_SCHEDULED_CONFLICT,
	RC_ELEMENT_LIST_EVENTS,
	RC_ELEMENT_LIST_FREETIME,
	RC_ELEMENT_LIST_OUTPUTS_NONE,
	RC_ELEMENT_LIST_MAX
}RC_ELEMENT_LISTTYPE_e;



typedef struct tagRC_DataType
{
	HUINT8				*pString;
	HUINT32				uiDecimal;
} RC_DataType_t;

typedef struct tagRC_Element
{
	HUINT32				uiDepth;
	HUINT8				*pucTagName;
	HUINT32				uiPrintType;
	HUINT8				*pucAttr0Name;
	HUINT8				*pucAttr1Name;
	HUINT8				*pucAttr2Name;
	HUINT8				*pucAttr3Name;
	HUINT8				*pucAttr4Name;
	HUINT8				*pucAttr5Name;
	HUINT8				*pucAttr6Name;
	HUINT8				*pucAttr7Name;
	HUINT8				*pucAttr8Name;
	HUINT8				*pucAttr9Name;
	HUINT8				*pucAttr10Name;
} RC_ELEMENT_t;


typedef struct tagRC_AttrElement
{
	HUINT8				*pucAttr1;
	HUINT8				*pucAttr2;
	HUINT8				*pucAttr3;
	HUINT8				*pucAttr4;
	HUINT8				*pucAttr5;
	HUINT8				*pucAttr6;
	HUINT8				*pucAttr7;
	HUINT8				*pucAttr8;
	HUINT8				*pucAttr9;
	HUINT8				*pucAttr10;
	HUINT8				*pucAttr11;
	HUINT8              ucDecimal;
} RC_ATTR_ELEMENT_t;

typedef struct tagRC_ElementList
{
	HUINT32				ulListType;
	RC_ELEMENT_t		*pstElement;
} RC_ELEMENTLIST_t;

typedef enum	{
	eResourceType_Locale = 0,
	eResourceType_Remote,
	eResourceType_Power,
	eResourceType_Time,
	eResourceType_Events,
	eResourceType_Search,
	eResourceType_Srclists,
	eResourceType_Sources,
	eResourceType_Outputs,
	eResourceType_Scheduled,
	eResourceType_Storage,
	eResourceType_Categories,
	eResourceType_Apps, //For DIAL
	eResourceType_Max
} eResourcesType;

static	HUINT8		*s_pszresources[eResourceType_Max] =
{
		"/rc/locale",
		"/rc/remote",
		"/rc/power",
		"/rc/time",
		"/rc/events",
		"/rc/search",
		"/rc/sourcelists",
		"/rc/sources",
		"/rc/outputs/0",
		"/rc/scheduled",
		"/rc/storage",
		"/rc/categories",
		"/rc/apps",
};

typedef enum	{
	eCommandType_None = 0,
	eCommandType_ChannelChange,
	eCommandType_VolumeSetting,
	eCommandType_FilePlay,
	eCommandType_Max
} eCommandType;
/*
typedef enum
{
	RCEVT_TYPE_NONE = 0,
	RCEVT_TYPE_LOCALE,
	RCEVT_TYPE_POWER,
	RCEVT_TYPE_STORAGE,
	RCEVT_TYPE_PROGRAMME,
	RCEVT_TYPE_APP,
	RCEVT_TYPE_SETTINGS,
	RCEVT_TYPE_SCHEDULE,
	RCEVT_TYPE_PLAYBACK,
	RCEVT_TYPE_MAX
} RCEvent_Type;

*/


static RC_EVENT_TYPE_Element_t	s_stRC_EventTypes[RCEVT_MAX]=
{
		{RCEVT_TYPE_RECEIVER,		RCEVT_SUBTYPE_RESOURCE_NAME,					"/rc/resource/name"},
		{RCEVT_TYPE_PAIRING,		RCEVT_SUBTYPE_RESOURCE_PAIR,					"/rc/resource/pair"},
		{RCEVT_TYPE_PAIRING,		RCEVT_SUBTYPE_RESOURCE_UNPAIR,					"/rc/resource/unpair"},
		{RCEVT_TYPE_CONTENTS,		RCEVT_SUBTYPE_OUTPUTS_PROGRAM,					"/rc/outputs/%d/programme"},
		{RCEVT_TYPE_CONTENTS,		RCEVT_SUBTYPE_OUTPUTS_PROGRAM_OVERRIDE,			"/rc/outputs/%d/programme/component-override"},
		{RCEVT_TYPE_CONTENTS,		RCEVT_SUBTYPE_OUTPUTS_APP,						"/rc/outputs/%d/app"},
		{RCEVT_TYPE_SETTINGS,		RCEVT_SUBTYPE_OUTPUTS_SETTINGS_VOLUME,			"/rc/outputs/settings/volume"},
		{RCEVT_TYPE_SETTINGS,		RCEVT_SUBTYPE_OUTPUTS_SETTINGS_MUTE,			"/rc/outputs/settings/mute"},
		{RCEVT_TYPE_SETTINGS,		RCEVT_SUBTYPE_OUTPUTS_SETTINGS_SUBTITLE,		"/rc/outputs/settings/subtitle"},
		{RCEVT_TYPE_SETTINGS,		RCEVT_SUBTYPE_OUTPUTS_SETTINGS_AD,				"/rc/outputs/settings/ad"},
		{RCEVT_TYPE_TRICKMODE,		RCEVT_SUBTYPE_OUTPUTS_PROGRAM_LIVE_PAUSE,		"/rc/outputs/%d/programme/livepause"},
		{RCEVT_TYPE_TRICKMODE,		RCEVT_SUBTYPE_OUTPUTS_PROGRAM_PLAYBACK,			"/rc/outputs/%d/programme/playback"},
		{RCEVT_TYPE_LOCALE,			RCEVT_SUBTYPE_LOCALE_TUNER,						"/rc/locale/tuner"},
		{RCEVT_TYPE_LOCALE,			RCEVT_SUBTYPE_LOCALE_POSTCODE,					"/rc/locale/postcode"},
		{RCEVT_TYPE_LOCALE,			RCEVT_SUBTYPE_LOCALE_PLATFORM,					"/rc/locale/platform"},
		{RCEVT_TYPE_CA,				RCEVT_SUBTYPE_CA,								"/rc/ca"},
		{RCEVT_TYPE_POWER,			RCEVT_SUBTYPE_POWER_ON,							"/rc/power/on"},
		{RCEVT_TYPE_POWER,			RCEVT_SUBTYPE_POWER_STANDBY,					"/rc/power/standby"},
};


static RC_OUTPUT_TYPE_Element_t s_stRC_OutputTypes[RC_OUTPUT_MAX]=
{
		{"Main Screen", "0"},
		{NULL, NULL},
		{NULL, NULL},
		//{"3rd Party Apps", "apps"},
		//{"Recording", "rec"},
};



#if 0
static	HUINT8		*s_pszcommand[eCommandType_Max] =
{
		"none",
		"channelchange",
		"volumesetting",
		"fileplay",
};

static	HUINT8		*s_pszDevString[9] =
{
		"/dev/sda",
		"/dev/sdb",
		"/dev/sdc",
		"/dev/sdd",
		"/dev/sde",
		"/dev/sdf",
		"/dev/sdg",
		"/dev/sdh",
		"/dev/sdi",
};
#endif

typedef struct
{
	HUINT8		ucOrgStrLen;
	HUINT8		ucFlagReplace;
	HUINT8		szOrgStr[8];
	HUINT8		ucReplaceCode;
} RC_REPLACESTR_t;

static RC_REPLACESTR_t		s_astRcReplaceString[] =
{
		{	5,		FALSE,	{'&', 'a', 'm', 'p', ';', '\0', 0, },		0x26	},
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

typedef enum	{
	eRsvType_None = 0,
	eRsvType_Record,
	eRsvType_Reminder,
	eRsvType_Series,
	eRsvType_All
} eRsvType;

typedef	struct tagRcListAttr
{
	HUINT8				ucVersion;
	HUINT8				aucCountry[MAX_RC_ATTRIBUTE_LEN];
	HUINT8 				ucCount;
}RCListAttr_t;

typedef struct tagRcSrclists
{
	HUINT8		aucListId[RECDATA_MAX_NAME_LEN];
	HUINT8		aucName[RECDATA_MAX_NAME_LEN];
	HBOOL		bModify;
	HUINT8		aucDescription[STRING_LEN_MAX];
	HUINT8		*pucLogo; //[STRING_URI_MAX];
}RC_Srclists_t;

#define	MAX_RC_SRCLISTS		3
static RC_Srclists_t	s_astRcSrclistsInfo[MAX_RC_SRCLISTS] =
{
	{ "dvb-all", "All Services", FALSE, "Every DVB service available on the server", NULL },
	{ "dvb-default-visible", "Default TV Channels", FALSE, "Every DVB service that is visible for the consumer in the default order", NULL },
	{ "dvb-custom-visible", "Custom TV Channels", TRUE, "Every DVB service that is visible for the consumer as ordered by the user", NULL },
};

typedef struct tagRcData
{
	HUINT16				usKind;
	HUINT16				usAction;
	HUINT16            	usIndex;
	HUINT16 		   	usUniqueId;
	HUINT16				usFsatId;
	HUINT16				usUserId;
	UNIXTIME 		   	utStartTime;
	UNIXTIME			utDuration;
	HUINT16				usRsvType;
	HUINT8				ucRepeat;
//	SVC_TripleId_t 	   	stTripleId;
	Handle_t			hSvc;
	RCLIST_RESULT_e		eRsvResult;
}RC_Data_t;


const static RC_ELEMENT_t		s_stElementResponse[] =
{
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL}
};

const static RC_ELEMENT_t		s_stElementLocale[] =
{
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1,	(HUINT8 *)"locale",	PRINT_OPENING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"deviceid", PRINT_STRING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"postcode", PRINT_STRING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"tuners", PRINT_DECIMAL,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1,	(HUINT8 *)"locale", PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL}
};

const static RC_ELEMENT_t		s_stElementPower[] =
{
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1,	(HUINT8 *)"power",	PRINT_ATTR3,	(HUINT8 *)"state", (HUINT8 *)"active-standby", (HUINT8 *)"standby-time",	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL}
};

const static RC_ELEMENT_t		s_stElementSrclists[] =
{
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"lists",	PRINT_OPENING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2, 	(HUINT8 *)"list",	PRINT_ATTR5,	(HUINT8 *)"list-id",	(HUINT8 *)"name",	"modify",	"description",	"logo-href"	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"lists",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL}
};

const static RC_ELEMENT_t		s_stElementSources[] =
{
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"sources",	PRINT_OPENING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2, 	(HUINT8 *)"source",	PRINT_ATTR3,	(HUINT8 *)"sid",	(HUINT8 *)"name",	"lcn",	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"sources",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL}
};

const static RC_ELEMENT_t		s_stElementChannels[] =
{
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"channels",	PRINT_OPENING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2, 	(HUINT8 *)"source",	PRINT_ATTR6,	(HUINT8 *)"sid",	(HUINT8 *)"sref",	"default-content-id",	"audio-only",	"logo",	"lcn"	,		NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"channels",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL}
};

const static RC_ELEMENT_t		s_stElementRemote[] =
{
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"remote",	PRINT_OPENING,	NULL	, NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2, 	(HUINT8 *)"key",	PRINT_ATTR1_CLOSING,	(HUINT8 *)"code",	NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"remote",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL}
};

const static RC_ELEMENT_t		s_stElementOutputs[] =
{
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1,	(HUINT8 *)"outputs",	PRINT_OPENING,	NULL, NULL, NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"output", PRINT_ATTR3_CONDITIONAL,	(HUINT8 *)"name", (HUINT8 *)"oid", (HUINT8 *)"main",	NULL,	NULL,	NULL,	NULL,NULL,	NULL,	NULL},
	{ DEPTH_1,	(HUINT8 *)"outputs",	PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
};

const static RC_ELEMENT_t		s_stElementOutputs_Item[] =
{
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"output",	PRINT_ATTR1,	(HUINT8 *)"name", NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"settings",	PRINT_ATTR4,	(HUINT8 *)"volume",	(HUINT8 *)"mute",	(HUINT8 *)"subtitle",	(HUINT8 *)"ad",	NULL,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2, 	(HUINT8 *)"programme",	PRINT_ATTR2_OPENING,	(HUINT8 *)"sid",	(HUINT8 *)"cid",	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_3,	(HUINT8 *)"component-override",	PRINT_ATTR3,	(HUINT8 *)"type",	(HUINT8 *)"mcid",	(HUINT8 *)"lang",	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_3,	(HUINT8 *)"eit",	PRINT_ATTR4,	(HUINT8 *)"name",	(HUINT8 *)"starttime",	(HUINT8 *)"duration",	(HUINT8 *)"description",	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"programme",	PRINT_CLOSING,	NULL,	NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"livebuffer",	PRINT_ATTR4,	(HUINT8 *)"live-timestamp", (HUINT8 *)"curr-offset",	(HUINT8 *)"speed",	(HUINT8 *)"buffer_duration",	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"playback",	PRINT_ATTR3,	(HUINT8 *)"curr-pos", (HUINT8 *)"speed",	(HUINT8 *)"dur",	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"app",	PRINT_ATTR3,	(HUINT8 *)"sid", (HUINT8 *)"cid",	(HUINT8 *)"app-type",	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"ca",	PRINT_ATTR3,	(HUINT8 *)"casid", (HUINT8 *)"status",	(HUINT8 *)"errorcode",	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"output",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL}
};
/*
const static RC_ELEMENT_t		s_stElementOutputs_3rdparty_App[] =
{
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"output",	PRINT_ATTR1,	(HUINT8 *)"oid", NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2, 	(HUINT8 *)"app",	PRINT_ATTR2,	(HUINT8 *)"sid",	(HUINT8 *)"cid",	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"settings",	PRINT_ATTR4,	(HUINT8 *)"volume",	(HUINT8 *)"mute",	(HUINT8 *)"subtitle",	(HUINT8 *)"ad",	NULL,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"output",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL}
};

const static RC_ELEMENT_t		s_stElementOutputs_Recording[] =
{
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"output",	PRINT_ATTR1,	(HUINT8 *)"oid", NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"programme",	PRINT_ATTR2,	(HUINT8 *)"sid",	(HUINT8 *)"cid",	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"settings",	PRINT_ATTR4,	(HUINT8 *)"volume",	(HUINT8 *)"mute",	(HUINT8 *)"subtitle",	(HUINT8 *)"ad",	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"playback",	PRINT_ATTR3,	(HUINT8 *)"curr-pos", (HUINT8 *)"speed",	(HUINT8 *)"dur",	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"output",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL}
};
*/

const static RC_ELEMENT_t		s_stElementCA[] =
{
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL, NULL},
	{ DEPTH_1,	(HUINT8 *)"ca",	PRINT_ATTR11_CONDITIONAL,	"cs-uid", "cs-type", "cs-revision",	"ca-vender",	"ca-version",	"ca-number",	"sc-inserted",	"sc-suitable",	"sc-type",	"sc-number",  "sc-status"},
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL, NULL},
};

const static RC_ELEMENT_t		s_stElementTime[] =
{
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1,	(HUINT8 *)"time",	PRINT_ATTR2,	(HUINT8 *)"received-time", (HUINT8 *)"sent-time", NULL,	NULL,	NULL,	NULL,	NULL,NULL,	NULL,	NULL},
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
};

const static RC_ELEMENT_t		s_stElementStorage[] =
{
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1, 	(HUINT8 *)"storage",	PRINT_ATTR5_OPENING,	(HUINT8 *)"sid", (HUINT8 *)"size", (HUINT8 *)"free",	(HUINT8 *)"external",	(HUINT8 *)"write",	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"stored-content",	PRINT_ATTR7_OPENING,	(HUINT8 *)"cid", (HUINT8 *)"created-time", (HUINT8 *)"size",	(HUINT8 *)"orig_sid",	(HUINT8 *)"title",	(HUINT8 *)"start",	(HUINT8 *)"duration",	NULL,	NULL,	NULL},
	{ DEPTH_3,	(HUINT8 *)"synopsis", PRINT_STRING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_3,	(HUINT8 *)"category", PRINT_ATTR1,	(HUINT8 *)"category-id",  NULL, NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_3,	(HUINT8 *)"media-component", PRINT_ATTR6,	(HUINT8 *)"type", (HUINT8 *)"aspect", (HUINT8 *)"mcid",	(HUINT8 *)"name",	(HUINT8 *)"vidformat",	(HUINT8 *)"default",	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_3,	(HUINT8 *)"media-component", PRINT_ATTR6,	(HUINT8 *)"type", (HUINT8 *)"name", (HUINT8 *)"mcid", (HUINT8 *)"lang",	(HUINT8 *)"audformat",	(HUINT8 *)"default",	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_3,	(HUINT8 *)"media-component", PRINT_ATTR5,	(HUINT8 *)"type", (HUINT8 *)"lang", (HUINT8 *)"mcid", (HUINT8 *)"intent", 	(HUINT8 *)"default",	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_3,	(HUINT8 *)"image", PRINT_STRING,	NULL,  NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"stored-content",	PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1,	(HUINT8 *)"storage",	PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT, 	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL,	NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
};

const static RC_ELEMENT_t		s_stElementScheduled[] =
{
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1,	(HUINT8 *)"scheduled",	PRINT_OPENING,	NULL, NULL, NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"event-booking", PRINT_ATTR9,	(HUINT8 *)"bid", (HUINT8 *)"type", (HUINT8 *)"sid",	(HUINT8 *)"cid",	(HUINT8 *)"global-content-id",	(HUINT8 *)"title",	(HUINT8 *)"start-time",(HUINT8 *)"end-time",	(HUINT8 *)"active",	NULL},
	{ DEPTH_2,	(HUINT8 *)"event-booking", PRINT_ATTR10,	(HUINT8 *)"bid", (HUINT8 *)"type", (HUINT8 *)"sid", (HUINT8 *)"cid",	(HUINT8 *)"global-content-id",	(HUINT8 *)"title",	(HUINT8 *)"start-time",(HUINT8 *)"end-time",	(HUINT8 *)"active", (HUINT8 *)"series-id"},
	{ DEPTH_1,	(HUINT8 *)"scheduled",	PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
};

const static RC_ELEMENT_t		s_stElementConflictScheduled[] =
{
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1,	(HUINT8 *)"scheduled",	PRINT_OPENING,	NULL, NULL, NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"event-booking", PRINT_ATTR8_OPENING,	(HUINT8 *)"bid", (HUINT8 *)"type", (HUINT8 *)"sid",	(HUINT8 *)"cid",	(HUINT8 *)"global-content-id",	(HUINT8 *)"title",	(HUINT8 *)"start-time",(HUINT8 *)"end-time",	NULL,	NULL},
	{ DEPTH_3,	(HUINT8 *)"conflicts-with", PRINT_OPENING,	NULL, NULL, NULL, NULL,	NULL,	NULL,	NULL,NULL,	NULL, NULL},
	{ DEPTH_4,	(HUINT8 *)"event-booking", PRINT_ATTR8, (HUINT8 *)"bid", (HUINT8 *)"type", (HUINT8 *)"sid", (HUINT8 *)"cid",	(HUINT8 *)"global-content-id",	(HUINT8 *)"title",	(HUINT8 *)"start-time",(HUINT8 *)"end-time",	NULL,	NULL},
	{ DEPTH_4,	(HUINT8 *)"event-booking", PRINT_ATTR9, (HUINT8 *)"bid", (HUINT8 *)"type", (HUINT8 *)"sid", (HUINT8 *)"cid",	(HUINT8 *)"global-content-id",	(HUINT8 *)"title",	(HUINT8 *)"start-time",(HUINT8 *)"end-time",	(HUINT8 *)"series-id",	NULL},
	{ DEPTH_3,	(HUINT8 *)"conflicts-with", PRINT_CLOSING,	NULL, NULL, NULL, NULL, NULL,	NULL,	NULL,NULL,	NULL, NULL},
	{ DEPTH_1,	(HUINT8 *)"scheduled",	PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
};

//<event uri="/rc/power/on" timestamp="11:57:00 09-07-2011"/>
const static RC_ELEMENT_t		s_stElementEvents[] =
{
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1,	(HUINT8 *)"eventlog",	PRINT_OPENING,	NULL, NULL, NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_2,	(HUINT8 *)"event", PRINT_ATTR3,	(HUINT8 *)"id", (HUINT8 *)"uri", (HUINT8 *)"timestamp",	NULL,	NULL,	NULL,	NULL,NULL,	NULL,	NULL},
	{ DEPTH_1,	(HUINT8 *)"eventlog",	PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
};

const static RC_ELEMENT_t		s_stElementOutputsNone[] =
{
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_ATTR1,	(HUINT8 *)"resource", NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_1,	(HUINT8 *)"output", PRINT_ATTR1_CLOSING,	(HUINT8 *)"oid", NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	NULL},
	{ DEPTH_ROOT,	(HUINT8 *)"response",	PRINT_CLOSING,	NULL, NULL, NULL,	NULL,	NULL	,	NULL,	NULL,	NULL,	NULL,	NULL},
};


const static RC_ELEMENTLIST_t		s_stElementResponseList 	= { RC_ELEMENT_RESPONSE,	(RC_ELEMENT_t *)s_stElementResponse	 	};
const static RC_ELEMENTLIST_t		s_stElementListsSrclist 		= { RC_ELEMENT_LIST_SRCLISTS,	(RC_ELEMENT_t *)s_stElementSrclists	 	};
const static RC_ELEMENTLIST_t		s_stElementSourcesList 		= { RC_ELEMENT_LIST_SOURCES,	(RC_ELEMENT_t *)s_stElementSources	 	};
const static RC_ELEMENTLIST_t		s_stElementChannelsList		= { RC_ELEMENT_LIST_CHANNELS,	(RC_ELEMENT_t *)s_stElementChannels	 	};
const static RC_ELEMENTLIST_t		s_stElementLocaleList		= { RC_ELEMENT_LIST_LOCALE,	(RC_ELEMENT_t *)s_stElementLocale	 	};
const static RC_ELEMENTLIST_t		s_stElementRemoteList		= { RC_ELEMENT_LIST_REMOTE,	(RC_ELEMENT_t *)s_stElementRemote	 	};
const static RC_ELEMENTLIST_t		s_stElementPowerList		= { RC_ELEMENT_LIST_POWER,	(RC_ELEMENT_t *)s_stElementPower	 	};
const static RC_ELEMENTLIST_t		s_stElementOutputList[2]		=
{
	{ RC_ELEMENT_LIST_OUTPUT_ITEM,			(RC_ELEMENT_t *)s_stElementOutputs_Item	 	},
	{ RC_ELEMENT_LIST_OUTPUTS,	(RC_ELEMENT_t *)s_stElementOutputs	 	},
/*
	{ RC_ELEMENT_LIST_OUTPUT_3RDPARTY_APP,	(RC_ELEMENT_t *)s_stElementOutputs_3rdparty_App	 	},
	{ RC_ELEMENT_LIST_OUTPUT_RECORDING,		(RC_ELEMENT_t *)s_stElementOutputs_Recording	 	},
*/
};
const static RC_ELEMENTLIST_t		s_stElementCAList 		= { RC_ELEMENT_LIST_CA,	(RC_ELEMENT_t *)s_stElementCA	 	};
const static RC_ELEMENTLIST_t		s_stElementTimeList 		= { RC_ELEMENT_LIST_TIME,	(RC_ELEMENT_t *)s_stElementTime	 	};

const static RC_ELEMENTLIST_t		s_stElementStorageList 		= { RC_ELEMENT_LIST_STORAGE,	(RC_ELEMENT_t *)s_stElementStorage	 	};
const static RC_ELEMENTLIST_t		s_stElementScheduledList 		= { RC_ELEMENT_LIST_SCHEDULED,	(RC_ELEMENT_t *)s_stElementScheduled	 	};
const static RC_ELEMENTLIST_t		s_stElementScheduledConflictList 		= { RC_ELEMENT_LIST_SCHEDULED_CONFLICT,	(RC_ELEMENT_t *)s_stElementConflictScheduled	 	};
const static RC_ELEMENTLIST_t		s_stElementEventsList 		= { RC_ELEMENT_LIST_EVENTS,	(RC_ELEMENT_t *)s_stElementEvents	 	};
const static RC_ELEMENTLIST_t		s_stElementOutputNoneList		= { RC_ELEMENT_LIST_OUTPUTS_NONE,	(RC_ELEMENT_t *)s_stElementOutputsNone	 	};




//static HUINT8		*g_pucTempUploadBuf = NULL;
//static HUINT32	g_uiBufSize = 0;
static RCEvent_Element	**g_pstRCEvtElements = NULL;
static HUINT16	g_usElemCnt = 0;
static HUINT16	g_usLastReqId = 0;
static HUINT32	g_ulRepeat = 0;
static HUINT32	g_ulLastReqRepeat = 0;

static HUINT32 local_rc_str_len(RC_ELEMENT_t *pstElement, HUINT32 uiTagType, RC_ATTR_ELEMENT_t *pstAttr, RC_DataType_t *pstData);

static HUINT32   local_rc_get(HUINT8 *pBuf, HUINT32 uiBufSize,HUINT8 *pulUploadBuf,HUINT32 ulTotalBufSize);


//static HERROR			local_rcRsv_SetServiceHandle(Handle_t *phDstSvc, HUINT16 usKind, RC_Data_t *pstRcData);
//static HERROR			local_rcRsv_FindMatchingSlot(HUINT32 *pulSlot, HUINT16 usFsatSvcId, HUINT16 usEvtId);
//static RCLIST_RESULT_e		local_rcRsv_ConstructRsvItem(RSV_Item_t * pstRsvItem, RC_Data_t *pstRcData);
//static RCLIST_RESULT_e local_rcRsv_DoReservation(RC_Data_t *pstRcData,HUINT32 *ulSlot,AM_Conflict_Info_t *conflictInfo,RSV_Item_t *rsvitemInfo);
//static HBOOL local_RC_IsOTARunning(void);
//static HERROR local_RC_NoticeCommandToWebApp(HCHAR *header,HCHAR *body);
//static HUINT8 * local_RemoteCtrl_GetEventSubTypeText(RCEvent_SubType eSubType);
//static HERROR local_RC_EventsURI(HUINT8 *pucURI,HUINT32 subEventType,HINT32 lParam1,HINT32 lParam2,HINT32 lParam3);


static HERROR local_rc_common(HUINT8 *pulUploadBuf,HUINT32 ulTotalBufSize,HUINT32 *ulNewTotalBufSize, RC_ELEMENT_t *pstElement,  HUINT32 uiTagType, RC_ATTR_ELEMENT_t *pstAttr, RC_DataType_t *pstData)

{
	HERROR					hErr = ERR_OK;
	HUINT32 				ulBufLen;
	HUINT32 				ulLen = 0;
	HUINT8					*pStrBuf=NULL;

	ulBufLen = local_rc_str_len(pstElement, uiTagType, pstAttr, pstData);
	if(ulBufLen>0)
	{
		pStrBuf = (HUINT8*)HLIB_STD_MemAlloc(ulBufLen+1);

		if(!pStrBuf)
		{
			HxLOG_Print("[local_rc_common] HLIB_STD_MemAlloc Fail \n");
			return hErr;
		}

		switch(pstElement[uiTagType].uiPrintType)
		{
			case PRINT_OPENING:
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s>\r\n", pstElement[uiTagType].pucTagName);
				break;
			case PRINT_CLOSING:
				ulLen = snprintf(pStrBuf, ulBufLen, "</%s>\r\n", pstElement[uiTagType].pucTagName);
				break;
			case PRINT_DECIMAL:
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s>%d</%s>\r\n", pstElement[uiTagType].pucTagName, pstData->uiDecimal, pstElement[uiTagType].pucTagName);
				break;
			case PRINT_STRING :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s>%s</%s>\r\n", pstElement[uiTagType].pucTagName, pstData->pString, pstElement[uiTagType].pucTagName);
				break;
			case PRINT_ATTR1 :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\">\r\n", pstElement[uiTagType].pucTagName, pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1);
				break;
			case PRINT_ATTR1_CLOSING :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName, pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1);
				break;
			case PRINT_ATTR2 :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" />\r\n",   pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2);
				break;
			case PRINT_ATTR2_OPENING :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\">\r\n",   pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2);
				break;
			case PRINT_ATTR3_CONDITIONAL:
				if(NULL == pstAttr->pucAttr3)
				{
					ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
					pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
					pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2);
				}
				else
				{
					ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
					pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
					pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
					pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3);
				}
				break;
			case PRINT_ATTR3 :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3);
				break;
			case PRINT_ATTR4 :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
				pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4);
				break;
			case PRINT_ATTR5 :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
				pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
				pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5);
				break;
			case PRINT_ATTR5_OPENING :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" >\r\n", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
				pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
				pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5);
				break;

			case PRINT_ATTR6 :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
				pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
				pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
				pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6);
				break;
			case PRINT_ATTR7 :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
				pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
				pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
				pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
				pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7);
				break;

			case PRINT_ATTR7_OPENING :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" >\r\n", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
				pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
				pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
				pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
				pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7);
				break;

			case PRINT_ATTR8 :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
				pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
				pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
				pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
				pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7,
				pstElement[uiTagType].pucAttr7Name, pstAttr->pucAttr8);
				break;

			case PRINT_ATTR8_OPENING :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" >\r\n", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
				pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
				pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
				pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
				pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7,
				pstElement[uiTagType].pucAttr7Name, pstAttr->pucAttr8);

				break;

			case PRINT_ATTR9 :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
				pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
				pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
				pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
				pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7,
				pstElement[uiTagType].pucAttr7Name, pstAttr->pucAttr8,
				pstElement[uiTagType].pucAttr8Name, pstAttr->pucAttr9);
				break;
			case PRINT_ATTR10 :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
				pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
				pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
				pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
				pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7,
				pstElement[uiTagType].pucAttr7Name, pstAttr->pucAttr8,
				pstElement[uiTagType].pucAttr8Name, pstAttr->pucAttr9,
				pstElement[uiTagType].pucAttr9Name, pstAttr->pucAttr10);
				break;

			case PRINT_ATTR11_CONDITIONAL :
				if(NULL == pstAttr->pucAttr8)
				{
					ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
					pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
					pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
					pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
					pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
					pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
					pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
					pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7);
				}
				else
				{
					ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
					pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
					pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
					pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
					pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
					pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
					pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
					pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7,
					pstElement[uiTagType].pucAttr7Name, pstAttr->pucAttr8,
					pstElement[uiTagType].pucAttr8Name, pstAttr->pucAttr9,
					pstElement[uiTagType].pucAttr9Name, pstAttr->pucAttr10,
					pstElement[uiTagType].pucAttr10Name, pstAttr->pucAttr11);
				}
				break;

			case PRINT_ATTR_DECIMAL :
				ulLen = snprintf(pStrBuf, ulBufLen, "<%s %s=%d>\r\n", pstElement[uiTagType].pucTagName, pstElement[uiTagType].pucAttr0Name, pstAttr->ucDecimal);
				break;
			default :
				break;
		}

		*ulNewTotalBufSize = local_rc_get(pStrBuf, ulBufLen-1,pulUploadBuf,ulTotalBufSize);

		HLIB_STD_MemFree(pStrBuf);
		pStrBuf = NULL;
	}

	if(ulLen <= 0) hErr = ERR_FAIL;
	return hErr;
}
static HERROR		local_rc_SetReplaceString(HUINT8 *pucSrcPtr, HUINT8 *pucDesPtr, HUINT32 *pucSaveSize, HUINT32 ulDeslen)
{
	HERROR					hError = ERR_OK;
	HUINT32					ulSrcLen = 0;
	HUINT32					ulDesPos = 0;
	HUINT32					ulSrcPos = 0;
	HUINT32					ulReplaceCharPos = 0;
	HUINT8					pucTempPtr[1024] = {0,};

	if(pucSrcPtr == NULL || pucDesPtr == NULL || ulDeslen == 0)
	{
		HxLOG_Print("[local_Rp_SetReplaceString] Param NULL!!\n");
		return ERR_FAIL;
	}

	ulSrcLen = strlen(pucSrcPtr);

	if(ulSrcLen >= ulDeslen)
	{
		HxLOG_Print("[local_Rp_SetReplaceString] Destnation Str Len is shorter than Source Str Len!!\n");
		return ERR_FAIL;
	}

//	VK_Print("[%s] pucSrcPtr : [%s]\n", __func__, pucSrcPtr);

	for( ulSrcPos = 0, ulDesPos = 0 ; ulSrcPos <  ulSrcLen ; ulSrcPos++)
	{
		HUINT32 ulReplaceCharIdx = 0;
		for(ulReplaceCharIdx = 0 ; ulReplaceCharIdx < 7 ; ulReplaceCharIdx++)
		{
			if(pucSrcPtr[ulSrcPos] == s_astRcReplaceString[ulReplaceCharIdx].ucReplaceCode)
			{
				break;
			}
		}

		if(ulReplaceCharIdx == 7)
		{
			pucTempPtr[ulDesPos++] = pucSrcPtr[ulSrcPos];
			continue;
		}


		for(ulReplaceCharPos = 0; ulReplaceCharPos < s_astRcReplaceString[ulReplaceCharIdx].ucOrgStrLen ; ulReplaceCharPos++)
		{
			pucTempPtr[ulDesPos++] = s_astRcReplaceString[ulReplaceCharIdx].szOrgStr[ulReplaceCharPos];
		}
	}

	ulSrcLen = strlen(pucTempPtr);

	for( ulSrcPos = 0, ulDesPos = 0 ; ulSrcPos <  ulSrcLen ; ulSrcPos++)
	{
		if(pucTempPtr[ulSrcPos] == 0xee || pucTempPtr[ulSrcPos] == 0x82 || pucTempPtr[ulSrcPos] == 0x86 || pucTempPtr[ulSrcPos] == 0x87)
		{
			continue;
		}

		// 근본원인은 record info의 aucDisplayEvtName 저장시 이미 UTF8 변환중 문제가 생김
		// 여기서는 일단 문제 character는 제거하는 것으로 처리
		if(pucTempPtr[ulSrcPos] < 0x20 || pucTempPtr[ulSrcPos] >= 0xE0)
		{
			continue;
		}

		pucDesPtr[ulDesPos++] = pucTempPtr[ulSrcPos];
	}

	hError = ERR_OK;

	return hError;
}


static HUINT32 local_rc_str_len(RC_ELEMENT_t *pstElement, HUINT32 uiTagType, RC_ATTR_ELEMENT_t *pstAttr, RC_DataType_t *pstData)
{
	HUINT32 uiLen = 0;

	switch(pstElement[uiTagType].uiPrintType)
	{
		case PRINT_OPENING:
			uiLen = snprintf(NULL, 0, "<%s>", pstElement[uiTagType].pucTagName) + 1;
			break;
		case PRINT_CLOSING:
			uiLen = snprintf(NULL, 0, "</%s>", pstElement[uiTagType].pucTagName) + 1;
			break;
		case PRINT_DECIMAL :
			uiLen = snprintf(NULL, 0, "<%s>%d</%s>", pstElement[uiTagType].pucTagName, pstData->uiDecimal, pstElement[uiTagType].pucTagName) + 1;
			break;
		case PRINT_STRING :
			uiLen = snprintf(NULL, 0, "<%s>%s</%s>", pstElement[uiTagType].pucTagName, pstData->pString, pstElement[uiTagType].pucTagName) + 1;
			break;
		case PRINT_ATTR1 :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\">", pstElement[uiTagType].pucTagName, pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1) + 1;
			break;
		case PRINT_ATTR1_CLOSING :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" />", pstElement[uiTagType].pucTagName, pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1) + 1;
			break;
		case PRINT_ATTR2 :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" />", pstElement[uiTagType].pucTagName,
			pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
			pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2) + 1;
			break;
		case PRINT_ATTR2_OPENING :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\">", pstElement[uiTagType].pucTagName,
			pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
			pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2) + 1;
			break;
		case PRINT_ATTR3_CONDITIONAL:
			if(NULL == pstAttr->pucAttr3)
			{
				uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" />", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2) + 1;
			}
			else
			{
				uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" />", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3) + 1;
			}
			break;
		case PRINT_ATTR3 :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" />", pstElement[uiTagType].pucTagName,
			pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
			pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
			pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3) + 1;
			break;
		case PRINT_ATTR4 :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />", pstElement[uiTagType].pucTagName,
			pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
			pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
			pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
			pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4) +1;
			break;
		case PRINT_ATTR5 :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />", pstElement[uiTagType].pucTagName,
			pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
			pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
			pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
			pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
			pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5) +1;
			break;
		case PRINT_ATTR5_OPENING :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" >", pstElement[uiTagType].pucTagName,
			pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
			pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
			pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
			pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
			pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5) +1;
			break;
		case PRINT_ATTR6 :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />", pstElement[uiTagType].pucTagName,
			pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
			pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
			pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
			pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
			pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
			pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6) +1;
			break;
		case PRINT_ATTR7 :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />", pstElement[uiTagType].pucTagName,
			pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
			pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
			pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
			pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
			pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
			pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
			pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7) +1;
			break;
		case PRINT_ATTR7_OPENING :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" >", pstElement[uiTagType].pucTagName,
			pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
			pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
			pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
			pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
			pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
			pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
			pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7) +1;
			break;
		case PRINT_ATTR8 :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />", pstElement[uiTagType].pucTagName,
			pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
			pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
			pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
			pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
			pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
			pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
			pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7,
			pstElement[uiTagType].pucAttr7Name, pstAttr->pucAttr8) +1;
			break;
		case PRINT_ATTR8_OPENING :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" >", pstElement[uiTagType].pucTagName,
			pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
			pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
			pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
			pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
			pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
			pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
			pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7,
			pstElement[uiTagType].pucAttr7Name, pstAttr->pucAttr8) +1;
			break;

		case PRINT_ATTR9 :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />", pstElement[uiTagType].pucTagName,
			pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
			pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
			pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
			pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
			pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
			pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
			pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7,
			pstElement[uiTagType].pucAttr7Name, pstAttr->pucAttr8,
			pstElement[uiTagType].pucAttr8Name, pstAttr->pucAttr9) +1;
			break;
		case PRINT_ATTR10 :
			uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />", pstElement[uiTagType].pucTagName,
			pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
			pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
			pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
			pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
			pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
			pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
			pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7,
			pstElement[uiTagType].pucAttr7Name, pstAttr->pucAttr8,
			pstElement[uiTagType].pucAttr8Name, pstAttr->pucAttr9,
			pstElement[uiTagType].pucAttr9Name, pstAttr->pucAttr10) +1;
			break;

		case PRINT_ATTR11_CONDITIONAL :
			if(NULL == pstAttr->pucAttr8)
			{
				uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
				pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
				pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
				pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
				pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7) + 1;
			}
			else
			{
				uiLen = snprintf(NULL, 0, "<%s %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\r\n", pstElement[uiTagType].pucTagName,
				pstElement[uiTagType].pucAttr0Name, pstAttr->pucAttr1,
				pstElement[uiTagType].pucAttr1Name, pstAttr->pucAttr2,
				pstElement[uiTagType].pucAttr2Name, pstAttr->pucAttr3,
				pstElement[uiTagType].pucAttr3Name, pstAttr->pucAttr4,
				pstElement[uiTagType].pucAttr4Name, pstAttr->pucAttr5,
				pstElement[uiTagType].pucAttr5Name, pstAttr->pucAttr6,
				pstElement[uiTagType].pucAttr6Name, pstAttr->pucAttr7,
				pstElement[uiTagType].pucAttr7Name, pstAttr->pucAttr8,
				pstElement[uiTagType].pucAttr8Name, pstAttr->pucAttr9,
				pstElement[uiTagType].pucAttr9Name, pstAttr->pucAttr10,
				pstElement[uiTagType].pucAttr10Name, pstAttr->pucAttr11) + 1;
			}
			break;
		case PRINT_ATTR_DECIMAL :
			uiLen = snprintf(NULL, 0,"<%s %s=%d>", pstElement[uiTagType].pucTagName, pstElement[uiTagType].pucAttr0Name, pstAttr->ucDecimal) + 1;
			break;
		default :
			break;
	}

	return uiLen;
}

static HERROR local_rc_loadElement(HUINT32 Cmd, RC_ELEMENT_t **ppstElement)
{
	HERROR			hErr = ERR_FAIL;

	if(ppstElement == NULL)
	{
		HxLOG_Print("[local_rc_loadElement] Param NULL !!\n");
		goto EXIT_FUNC;
	}

	*ppstElement = NULL;

	switch(Cmd)
	{
		case RC_ELEMENT_RESPONSE:
			*ppstElement = (RC_ELEMENT_t *)s_stElementResponseList.pstElement;
			break;
		case RC_ELEMENT_LIST_SRCLISTS:
			*ppstElement = (RC_ELEMENT_t *)s_stElementListsSrclist.pstElement;
			break;
		case RC_ELEMENT_LIST_SOURCES:
			*ppstElement = (RC_ELEMENT_t *)s_stElementSourcesList.pstElement;
			break;
		case RC_ELEMENT_LIST_CHANNELS:
			*ppstElement = (RC_ELEMENT_t *)s_stElementChannelsList.pstElement;
			break;
		case RC_ELEMENT_LIST_LOCALE:
			*ppstElement = (RC_ELEMENT_t *)s_stElementLocaleList.pstElement;
			break;
		case RC_ELEMENT_LIST_REMOTE:
			*ppstElement = (RC_ELEMENT_t *)s_stElementRemoteList.pstElement;
			break;
		case RC_ELEMENT_LIST_POWER:
			*ppstElement = (RC_ELEMENT_t *)s_stElementPowerList.pstElement;
			break;
		case RC_ELEMENT_LIST_OUTPUT_ITEM:
			*ppstElement = (RC_ELEMENT_t *)s_stElementOutputList[0].pstElement;
			break;
		case RC_ELEMENT_LIST_OUTPUTS:
			*ppstElement = (RC_ELEMENT_t *)s_stElementOutputList[1].pstElement;
			break;
	/*
		case RC_ELEMENT_LIST_OUTPUT_3RDPARTY_APP:
			*ppstElement = (RC_ELEMENT_t *)s_stElementOutputList[2].pstElement;
			break;
		case RC_ELEMENT_LIST_OUTPUT_RECORDING:
			*ppstElement = (RC_ELEMENT_t *)s_stElementOutputList[3].pstElement;
			break;
	*/
		case RC_ELEMENT_LIST_CA:
			*ppstElement = (RC_ELEMENT_t *)s_stElementCAList.pstElement;
			break;
		case RC_ELEMENT_LIST_TIME:
			*ppstElement = (RC_ELEMENT_t *)s_stElementTimeList.pstElement;
			break;
		case RC_ELEMENT_LIST_STORAGE:
			*ppstElement = (RC_ELEMENT_t *)s_stElementStorageList.pstElement;
			break;

		case RC_ELEMENT_LIST_SCHEDULED:
			*ppstElement = (RC_ELEMENT_t *)s_stElementScheduledList.pstElement;
			break;

		case RC_ELEMENT_LIST_SCHEDULED_CONFLICT:
			*ppstElement = (RC_ELEMENT_t *)s_stElementScheduledConflictList.pstElement;
			break;

		case RC_ELEMENT_LIST_EVENTS:
			*ppstElement = (RC_ELEMENT_t *)s_stElementEventsList.pstElement;
			break;
		case RC_ELEMENT_LIST_OUTPUTS_NONE:
			*ppstElement = (RC_ELEMENT_t *)s_stElementOutputNoneList.pstElement;
			break;

		default :
			*ppstElement = NULL;
			HxLOG_Print("[local_rc_loadElement] invalid mode !!\n");
			goto EXIT_FUNC;
	}

	hErr = ERR_OK;

	EXIT_FUNC :

	return hErr;
}


static HUINT32   local_rc_get(HUINT8 *pBuf, HUINT32 uiBufSize,HUINT8 *pulUploadBuf,HUINT32 ulTotalBufSize)
{
	if(pulUploadBuf)
	{
		HxSTD_MemCopy(pulUploadBuf + ulTotalBufSize, pBuf, uiBufSize);
		ulTotalBufSize += uiBufSize;
	}

	return ulTotalBufSize;
}

HERROR Make_RC_XML_Header(HUINT8 *pucUploadBuf,HUINT32 ulTotalBufSize,HUINT32 *ulNewTotalBufSize)
{
	HERROR					hErr = ERR_OK;
	HUINT32 				ulLen;
	HUINT8					StrBuf[32] = {0,};

	ulLen = snprintf(StrBuf, 32, "<?xml version=\"1.0\" ?>");
	ulLen = snprintf(NULL, 0, "<?xml version=\"1.0\" ?>") + 1;

	*ulNewTotalBufSize = local_rc_get(StrBuf, ulLen-1,pucUploadBuf,ulTotalBufSize);

	if(ulLen <= 0) hErr = ERR_FAIL;
	return hErr;
}



// GET OUTPUTS
HERROR Get_RC_Outputs(HUINT8 *pucUploadBuf, HUINT8 *command, HUINT32 *ulUploadLen)
{
	HERROR			 		hError = ERR_FAIL;
	RC_ELEMENT_t			*pstElement = NULL;
	RC_ATTR_ELEMENT_t		st_rcAttr;
	HUINT8					szAttrBuf[5][STRING_LEN_MAX];

	HUINT32 				i = 0;
	HUINT32					uiTotalBufSize = 0,ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;

	pucTempUploadBuf = HLIB_STD_MemAlloc(RC_RESOURCE_MAX_BUF_SIZE);
	if(pucTempUploadBuf == NULL)
	{
		HxLOG_Print("[%s][%d]  HLIB_STD_MemAlloc() ERROR \n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}


	hError = local_rc_loadElement(RC_ELEMENT_LIST_OUTPUTS, &pstElement);
	if( hError != ERR_OK )
	{
		HxLOG_Print("[%s][%d]  load element ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}

	// Make Outputs list
	st_rcAttr.pucAttr1 = command;
	uiTotalBufSize	= 0;

	hError = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;

	// <response>
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUTS_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUTS_OPEN, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	for(i = 0; i < RC_OUTPUT_MAX; i++)
	{
		if(NULL == s_stRC_OutputTypes[i].szName)
		{
			continue;
		}

		HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
		HxSTD_snprintf(szAttrBuf[0],STRING_LEN_MAX,"%s", s_stRC_OutputTypes[i].szName);
		st_rcAttr.pucAttr1 = szAttrBuf[0];
		HxSTD_snprintf(szAttrBuf[1],STRING_LEN_MAX,"%s", s_stRC_OutputTypes[i].szOid);
		st_rcAttr.pucAttr2 = szAttrBuf[1];

		if(HLIB_STD_StrCaseCmp(s_stRC_OutputTypes[i].szOid, "0") == 0)
		{
			HxSTD_snprintf(szAttrBuf[2],STRING_LEN_MAX,"true");
			st_rcAttr.pucAttr3 = szAttrBuf[2];
		}
		else
		{
			st_rcAttr.pucAttr3 = NULL;
		}

		hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT, &st_rcAttr, NULL);
		uiTotalBufSize = ulNewTotalBufSize;
	}

	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUTS_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUTS_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	if(uiTotalBufSize > RC_RESOURCE_MAX_BUF_SIZE)
	{
		HxLOG_Print("\t[%s][%d]  @@@ Output informations are too large(%d)..!!! \n",__FUNCTION__,__LINE__, uiTotalBufSize);
		uiTotalBufSize = RC_RESOURCE_MAX_BUF_SIZE;
	}
	HxSTD_MemCopy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);
	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HLIB_STD_MemFree(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	HxLOG_Print("[%s][%d] (%s)-\n", __FUNCTION__,__LINE__,pucUploadBuf);
	return hError;
}

HERROR Get_RC_OutputInfo(HUINT8 *pucUploadBuf, HUINT8 *command, HUINT32 *ulUploadLen, void* pOutputInfo)
{
	HERROR			 		hError = ERR_FAIL;
	RC_ELEMENT_t			*pstElement = NULL;
	RC_ATTR_ELEMENT_t		st_rcAttr;
	HUINT8					szUri[STRING_URI_MAX];
	HUINT8					szUri2[STRING_URI_MAX];
	HUINT8					szAttrBuf[5][STRING_LEN_MAX];
	RC_Outputs_Info_t *pInfo = (RC_Outputs_Info_t*)pOutputInfo;

	HUINT32					i, uiTotalBufSize = 0,ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;

	HFLOAT32				convertVolume;

	HxLOG_Debug("\t\t @@@ Get_RC_OutputInfo : (%s\n", command);

	pucTempUploadBuf = HLIB_STD_MemAlloc(RC_RESOURCE_MAX_BUF_SIZE);
	if(pucTempUploadBuf == NULL)
	{
		HxLOG_Print("[%s][%d]  HLIB_STD_MemAlloc() ERROR \n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	hError = local_rc_loadElement(RC_ELEMENT_LIST_OUTPUT_ITEM, &pstElement);
	if( hError != ERR_OK )
	{
		HxLOG_Print("[%s][%d]  load element ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}

	// Make Outputs list
	st_rcAttr.pucAttr1 = command;
	uiTotalBufSize	= 0;

	hError = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;

	// <response>
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
	HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s", s_stRC_OutputTypes[0].szName); //oid name
	st_rcAttr.pucAttr1 = szAttrBuf[0];
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	HxLOG_Debug("\t\t @@@ Get_RC_OutputInfo [%d] %d\n", __LINE__, pInfo->stSetupInfo.ulVolumne);

	// setting
	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
	convertVolume = (HFLOAT32)pInfo->stSetupInfo.ulVolumne;
	convertVolume /= 100;
	HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%.2f", convertVolume); //volume
	//HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%d", pInfo->stSetupInfo.ulVolumne); //volume
	st_rcAttr.pucAttr1 = szAttrBuf[0];
	HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", pInfo->stSetupInfo.bMute==TRUE?"true":"false"); //mute
	st_rcAttr.pucAttr2 = szAttrBuf[1];
	HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%s", pInfo->stSetupInfo.bSubtitle==TRUE?"true":"false"); //subtitle
	st_rcAttr.pucAttr3 = szAttrBuf[2];
	HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s", pInfo->stSetupInfo.bAD==TRUE?"true":"false"); //ad
	st_rcAttr.pucAttr4 = szAttrBuf[3];
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT_SETTINGS, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	HxLOG_Debug("\t\t @@@ Get_RC_OutputInfo [%d] %d\n", __LINE__, pInfo->stSvcInfo.usSourceId);
	//programe
	if(pInfo->stSvcInfo.bAvailable)
	{

		HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
		HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%d", pInfo->stSvcInfo.usSourceId);
		st_rcAttr.pucAttr1 = szAttrBuf[0];
		HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%X", pInfo->stSvcInfo.nContentId);
		st_rcAttr.pucAttr2 = szAttrBuf[1];
		hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT_PROGRAMME_OPEN, &st_rcAttr, NULL);
		uiTotalBufSize = ulNewTotalBufSize;

		HxLOG_Debug("\t\t @@@ Get_RC_OutputInfo [%d]\n", __LINE__);
		//Components
		for(i = 0; i < pInfo->stSvcInfo.ulComponentNum; i++)
		{
			HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
			HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s", pInfo->stSvcInfo.stEsInfo[i].eType==eRC_COMPONENT_AUDIO?"audio":"subtitle");
			st_rcAttr.pucAttr1 = szAttrBuf[0];
			HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", pInfo->stSvcInfo.stEsInfo[i].szLang);
			st_rcAttr.pucAttr2 = szAttrBuf[1];
			HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%s", pInfo->stSvcInfo.stEsInfo[i].szDescription);
			st_rcAttr.pucAttr3 = szAttrBuf[2];
			hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT_COMPONENT, &st_rcAttr, NULL);
			uiTotalBufSize = ulNewTotalBufSize;
		}
		HxLOG_Debug("\t\t @@@ Get_RC_OutputInfo [%d]\n", __LINE__);
		// EIT
		if(pInfo->stEitInfo.bAvailable)
		{
			HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
			HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s", pInfo->stEitInfo.szEvtName);
			st_rcAttr.pucAttr1 = szAttrBuf[0];
			HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%d", pInfo->stEitInfo.ulStartTime);
			st_rcAttr.pucAttr2 = szAttrBuf[1];
			HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%d", pInfo->stEitInfo.ulDuration);
			st_rcAttr.pucAttr3 = szAttrBuf[2];
			HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s", pInfo->stEitInfo.szDescription);
			st_rcAttr.pucAttr4 = szAttrBuf[3];
			hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT_EIT, &st_rcAttr, NULL);
			uiTotalBufSize = ulNewTotalBufSize;
		}
		HxLOG_Debug("\t\t @@@ Get_RC_OutputInfo [%d]\n", __LINE__);
		hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT_PROGRAMME_CLOSE, NULL, NULL);
		uiTotalBufSize = ulNewTotalBufSize;
	}

	HxLOG_Debug("\t\t @@@ Get_RC_OutputInfo [%d]\n", __LINE__);
	//TSR(live buffer)
	if(pInfo->stTsrInfo.bAvailable)
	{
		HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
		HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%d", pInfo->stTsrInfo.ulTimeStamp);
		st_rcAttr.pucAttr1 = szAttrBuf[0];
		HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%d", pInfo->stTsrInfo.ulOffset);
		st_rcAttr.pucAttr2 = szAttrBuf[1];
		HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%d", pInfo->stTsrInfo.nSpeed);
		st_rcAttr.pucAttr3 = szAttrBuf[2];
		HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%d", pInfo->stTsrInfo.ulBufferDuration);
		st_rcAttr.pucAttr4 = szAttrBuf[3];
		hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT_TSRBUFFER, &st_rcAttr, NULL);
		uiTotalBufSize = ulNewTotalBufSize;
	}
	HxLOG_Debug("\t\t @@@ Get_RC_OutputInfo [%d]\n", __LINE__);
	// Recording(PB)
	if(pInfo->stPbInfo.bAvailable)
	{
		HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
		HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%d", pInfo->stPbInfo.ulCurPos);
		st_rcAttr.pucAttr1 = szAttrBuf[0];
		HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%d", pInfo->stPbInfo.nSpeed);
		st_rcAttr.pucAttr2 = szAttrBuf[1];
		HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%d", pInfo->stPbInfo.ulDuration);
		st_rcAttr.pucAttr3 = szAttrBuf[2];
		hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT_PLAYBACK, &st_rcAttr, NULL);
		uiTotalBufSize = ulNewTotalBufSize;
	}
	HxLOG_Debug("\t\t @@@ Get_RC_OutputInfo [%d]\n", __LINE__);
	// APPS
	if(pInfo->stAppsInfo.bAvailable)
	{
		HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
		HxSTD_snprintf(szUri, STRING_URI_MAX, "%s", pInfo->stAppsInfo.szSourceId);
		st_rcAttr.pucAttr1 = szUri;
		HxSTD_snprintf(szUri2, STRING_URI_MAX, "%s", pInfo->stAppsInfo.szContentId);
		st_rcAttr.pucAttr2 = szUri2;

		switch(pInfo->stAppsInfo.eAppType)
		{
			case eRC_APPS_EMBEDDED:
				HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "embedded");
				break;
			case eRC_APPS_NATIVE:
				HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "native");
				break;
			case eRC_APPS_MHEG:
				HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "mheg");
				break;
			case eRC_APPS_HBBTV:
				HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "hbbtv");
				break;
			case eRC_APPS_DIAL:
				HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "dial");
				break;
			case eRC_APPS_SATIP:
				HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "satip");
				break;
			default:
				HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "other");
				break;
		}
		st_rcAttr.pucAttr3 = szAttrBuf[2];

		hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT_APP, &st_rcAttr, NULL);
		uiTotalBufSize = ulNewTotalBufSize;
	}
	HxLOG_Debug("\t\t @@@ Get_RC_OutputInfo [%d]\n", __LINE__);

	// CA
	if(pInfo->stCasInfo.bAvailable)
	{
		HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
		HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%d", pInfo->stCasInfo.ulCasId);
		st_rcAttr.pucAttr1 = szAttrBuf[0];
		HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%d", pInfo->stCasInfo.ulStatus);
		st_rcAttr.pucAttr2 = szAttrBuf[1];
		HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%X", pInfo->stCasInfo.ulErrorCode);
		st_rcAttr.pucAttr3 = szAttrBuf[2];
		hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT_CA, &st_rcAttr, NULL);
		uiTotalBufSize = ulNewTotalBufSize;
	}
	HxLOG_Debug("\t\t @@@ Get_RC_OutputInfo [%d]\n", __LINE__);



	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_OUTPUT_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	HxLOG_Debug("\t\t @@@ Get_RC_OutputInfo [%d]\n", __LINE__);
	if(uiTotalBufSize > RC_RESOURCE_MAX_BUF_SIZE)
	{
		HxLOG_Print("\t[%s][%d]  @@@ Output informations are too large(%d)..!!! \n",__FUNCTION__,__LINE__, uiTotalBufSize);
		uiTotalBufSize = RC_RESOURCE_MAX_BUF_SIZE;
	}
	HxSTD_MemCopy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);
	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HLIB_STD_MemFree(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	HxLOG_Print("[%s][%d] (%s)-\n", __FUNCTION__,__LINE__,pucUploadBuf);
	return hError;
}


// Get Lists
HERROR Get_RC_Srclists(HUINT8 *pucUploadBuf, HUINT8 *command, HUINT32 *ulUploadLen)
{
	HERROR			 		hError = ERR_FAIL;
	RC_ELEMENT_t			*pstElement = NULL;
	RC_ATTR_ELEMENT_t		st_rcAttr;
	HUINT8					szAttrBuf[5][STRING_LEN_MAX];

	HUINT32 				i = 0;
	HUINT32					uiTotalBufSize = 0,ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;

	pucTempUploadBuf = HLIB_STD_MemAlloc(RC_RESOURCE_MAX_BUF_SIZE);
	if(pucTempUploadBuf == NULL)
	{
		HxLOG_Print("[%s][%d]  HLIB_STD_MemAlloc() ERROR \n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}


	hError = local_rc_loadElement(RC_ELEMENT_LIST_SRCLISTS, &pstElement);
	if( hError != ERR_OK )
	{
		HxLOG_Print("[%s][%d]  load element ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}

	// Make Outputs list
	st_rcAttr.pucAttr1 = command;
	uiTotalBufSize	= 0;

	hError = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;

	// <response>
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SRCLISTS_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SRCLISTS_OPEN, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	for(i = 0; i < MAX_RC_SRCLISTS; i++)
	{
		HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
		HxSTD_snprintf(szAttrBuf[0],STRING_LEN_MAX,"%s", s_astRcSrclistsInfo[i].aucListId);
		st_rcAttr.pucAttr1 = szAttrBuf[0];
		HxSTD_snprintf(szAttrBuf[1],STRING_LEN_MAX,"%s", s_astRcSrclistsInfo[i].aucName);
		st_rcAttr.pucAttr2 = szAttrBuf[1];
		HxSTD_snprintf(szAttrBuf[2],STRING_LEN_MAX,"%s", s_astRcSrclistsInfo[i].bModify == TRUE? "true": "false");
		st_rcAttr.pucAttr3 = szAttrBuf[2];
		HxSTD_snprintf(szAttrBuf[3],STRING_LEN_MAX,"%s", s_astRcSrclistsInfo[i].aucDescription);
		st_rcAttr.pucAttr4 = szAttrBuf[3];
		HxSTD_snprintf(szAttrBuf[4],STRING_LEN_MAX,"%s", s_astRcSrclistsInfo[i].pucLogo != NULL? (HCHAR*)s_astRcSrclistsInfo[i].pucLogo: "");
		st_rcAttr.pucAttr5 = szAttrBuf[4];

		hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SRCLIST_INFO, &st_rcAttr, NULL);
		uiTotalBufSize = ulNewTotalBufSize;
	}

	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SRCLISTS_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SRCLISTS_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	if(uiTotalBufSize > RC_RESOURCE_MAX_BUF_SIZE)
	{
		HxLOG_Print("\t[%s][%d]  @@@ List informations are too large(%d)..!!! \n",__FUNCTION__,__LINE__, uiTotalBufSize);
		uiTotalBufSize = RC_RESOURCE_MAX_BUF_SIZE;
	}
	HxSTD_MemCopy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);
	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HLIB_STD_MemFree(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	HxLOG_Print("[%s][%d] (%s)-\n", __FUNCTION__,__LINE__,pucUploadBuf);
	return hError;
}

HBOOL IsExist_RC_Srclist(HUINT8 *pszListId, HBOOL *pbModifiable)
{
	HUINT32 	i;
	HBOOL		bExist = FALSE;

	for(i = 0; i < MAX_RC_SRCLISTS; i++)
	{
		if(HLIB_STD_StrCaseCmp(s_astRcSrclistsInfo[i].aucListId, pszListId) == 0)
		{
			bExist = TRUE;

			if(pbModifiable)
			{
				*pbModifiable = s_astRcSrclistsInfo[i].bModify;
			}
			break;
		}
	}

	return bExist;
}

// GET Sources
HERROR Get_RC_Sources(HUINT8 *pucUploadBuf, HUINT8 *command, HUINT32 *ulUploadLen, void *pSrcInfo, HINT32 nSrcNum)
{
	HERROR			 		hError = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	HUINT32					ulSvcCnt= 0,ulSvcIdx = 0;
	RC_ATTR_ELEMENT_t		st_rcAttr;
	HUINT8					szAttrBuf[5][STRING_LEN_MAX];

	/*
	UNIXTIME				uxCurrentTime;
	HUINT16					usEvtId;
	HUINT8					EvtName[EVT_NAME_LEN_MAX];
	*/
	HUINT32					uiTotalBufSize = 0,ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;

	RC_Sources_Info_t		*pstSrcInfos = (RC_Sources_Info_t*)pSrcInfo;

	ulSvcCnt =	(HUINT32)nSrcNum;

	/* element load */
	hError = local_rc_loadElement(RC_ELEMENT_LIST_SOURCES, &pstElement);
	if( hError != ERR_OK )
	{
		HxLOG_Print("[%s][%d] Get_RC_Sources load element ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}

	pucTempUploadBuf = pucUploadBuf; //HLIB_STD_MemAlloc(*ulUploadLen);
	uiTotalBufSize  = 0;

	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));

	st_rcAttr.pucAttr1 = command;

	hError = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;
	// <response>
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SOURCES_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SOURCES_OPEN, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	for(ulSvcIdx=0; ulSvcIdx< ulSvcCnt; ulSvcIdx++)
	{
		//<source sid="505" sref="dvb://2.2045.6301" default-content-id="23DA" logo="http://www.bbc.com/images/bbc1logo.gif" lcn="101" />
		HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%d", pstSrcInfos[ulSvcIdx].usSid); //sid
		st_rcAttr.pucAttr1 = szAttrBuf[0];
		HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", pstSrcInfos[ulSvcIdx].szSvcName); //name
		st_rcAttr.pucAttr2 = szAttrBuf[1];
		HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%d",pstSrcInfos[ulSvcIdx].usLcn); //lcn
		st_rcAttr.pucAttr3 = szAttrBuf[2];
		hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SOURCES_INFO, &st_rcAttr, NULL);
		uiTotalBufSize = ulNewTotalBufSize;
	}

	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SOURCES_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SOURCES_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	//HxSTD_MemCopy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);
	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	//HLIB_STD_MemFree(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	HxLOG_Print("[%s][%d] (%s)-\n", __FUNCTION__,__LINE__,pucUploadBuf);
	return hError;
}


// GET Sources
HERROR Get_RC_CaInfo(HUINT8 *pucUploadBuf, HUINT8 *command, HUINT32 *ulUploadLen, void *pCaInfo)
{
	HERROR			 		hError = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	RC_ATTR_ELEMENT_t		st_rcAttr;
	HUINT8					szAttrBuf[11][STRING_LEN_MAX];

	HUINT32					uiTotalBufSize = 0,ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;

	RC_CA_Info_t		*pstCaInfos = (RC_CA_Info_t*)pCaInfo;

	HxLOG_Debug("\t\t @@@ Get_RC_CaInfo [%d]\n", __LINE__);

	/* element load */
	hError = local_rc_loadElement(RC_ELEMENT_LIST_CA, &pstElement);
	if( hError != ERR_OK )
	{
		HxLOG_Print("[%s][%d] Get_RC_CaInfo load element ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}
	pucTempUploadBuf = HLIB_STD_MemAlloc(RC_RESOURCE_SHORT_MAX_BUF_SIZE);
	uiTotalBufSize  = 0;

	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));

	st_rcAttr.pucAttr1 = command;

	hError = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;
	// <response>
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_CA_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	{
		HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s", pstCaInfos->aucCsId);
		st_rcAttr.pucAttr1 = szAttrBuf[0];
		HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", pstCaInfos->aucCsType);
		st_rcAttr.pucAttr2 = szAttrBuf[1];
		HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%s", pstCaInfos->aucCsVer);
		st_rcAttr.pucAttr3 = szAttrBuf[2];

		HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s", pstCaInfos->aucCaVender);
		st_rcAttr.pucAttr4 = szAttrBuf[3];
		HxSTD_snprintf(szAttrBuf[4], STRING_LEN_MAX, "%s", pstCaInfos->aucCaVer);
		st_rcAttr.pucAttr5 = szAttrBuf[4];
		HxSTD_snprintf(szAttrBuf[5], STRING_LEN_MAX, "%s", pstCaInfos->aucCaSN);
		st_rcAttr.pucAttr6 = szAttrBuf[5];

		HxSTD_snprintf(szAttrBuf[6],STRING_LEN_MAX,"%s", pstCaInfos->bScInserted == TRUE? "true": "false");
		st_rcAttr.pucAttr7 = szAttrBuf[6];

		if((pstCaInfos->bScInserted == TRUE) && (pstCaInfos->ulScStatus == 0))  //CA_SMARTCARD_STATE_OK
		{
			HxSTD_snprintf(szAttrBuf[7],STRING_LEN_MAX,"%s", pstCaInfos->bScSuitable == TRUE? "true": "false");
			st_rcAttr.pucAttr8 = szAttrBuf[7];


			HxSTD_snprintf(szAttrBuf[8], STRING_LEN_MAX, "%s", pstCaInfos->aucScType);
			st_rcAttr.pucAttr9 = szAttrBuf[8];
			HxSTD_snprintf(szAttrBuf[9], STRING_LEN_MAX, "%s", pstCaInfos->aucScNUm);
			st_rcAttr.pucAttr10 = szAttrBuf[9];
			HxSTD_snprintf(szAttrBuf[10], STRING_LEN_MAX, "%d", pstCaInfos->ulScStatus);
			st_rcAttr.pucAttr11 = szAttrBuf[10];
		}

		hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_CA_INFO, &st_rcAttr, NULL);
		uiTotalBufSize = ulNewTotalBufSize;
	}

	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_CA_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	if(uiTotalBufSize > RC_RESOURCE_SHORT_MAX_BUF_SIZE)
	{
		HxLOG_Print("\t[%s][%d]  @@@ CA informations are too large(%d)..!!! \n",__FUNCTION__,__LINE__, uiTotalBufSize);
		uiTotalBufSize = RC_RESOURCE_SHORT_MAX_BUF_SIZE;
	}
	HxSTD_MemCopy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);
	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HLIB_STD_MemFree(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	HxLOG_Print("[%s][%d] (%s)-\n", __FUNCTION__,__LINE__,pucUploadBuf);
	return hError;
}

#if 0		// GET Current Source
HERROR Get_RC_Current_Source(HUINT8 *pucUploadBuf,HUINT8 *pUserData1,HUINT32 *ulUploadLen)
{
	HERROR			 		hError = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	HUINT32					ulSvcCnt= 0,ulSvcIdx = 0;
	Handle_t					*phSvcList = NULL;
	Handle_t					hSvc = HANDLE_NULL;
	SVC_Info_t				stSvcInfo;
	RC_ATTR_ELEMENT_t		st_rcAttr;
	HUINT8					szAttrBuf[5][STRING_LEN_MAX];
	UNIXTIME				uxCurrentTime;
	HUINT16					usEvtId;
	HUINT8					EvtName[EVT_NAME_LEN_MAX];
	HUINT16					usFsatSvcId =0;
	HINT32					i = 0;
	HCHAR					ServiceId[SERVICE_ID_LEN_MAX];
	HUINT16					pos = 0;
	HUINT32					uiTotalBufSize = 0,ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;

	/* element load */
	hError = local_rc_loadElement(RC_ELEMENT_LIST_CHANNELS, &pstElement);
	if( hError != ERR_OK )
	{
		HxLOG_Print("[%s][%d] Get_RC_Sources load element ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}

	for(i = 0;i <HxSTD_StrLen(pUserData1);i++)
	{
		if(HxSTD_StrNCmp((char *)&pUserData1[i],"/",1) == 0)
		{
			pos = i;
		}
	}
	if(pos+1 >= strlen(pUserData1) )
	{
		return hError;
	}

	HxSTD_snprintf(ServiceId, SERVICE_ID_LEN_MAX, "%s",&pUserData1[pos+1]); //sid
	usFsatSvcId = atoi(ServiceId);

	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
	HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s",pUserData1); //sid
	st_rcAttr.pucAttr1 = szAttrBuf[0];

	pucTempUploadBuf = HxMEM_MEMDUP(pucUploadBuf,PACKET_MAX_BUF_SIZE);
	uiTotalBufSize  = 0;

	hError = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;

	// <response>
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_CHANNELS_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_CHANNELS_OPEN, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;



	hError = FSAT_DB_SVC_FindServiceListByFsatIdf(eChDeliveryType_SAT, eSvcType_All, eLcnVisibleSvc_FindAll,
										usFsatSvcId, &ulSvcCnt, &phSvcList);
	HxLOG_Print("[%s][%d] ulSvcCnt: %d\n",__FUNCTION__,__LINE__,ulSvcCnt);
	if ( ulSvcCnt != 1)
	{
		HxLOG_Print("[%s:%d] multiple fsatid services. # of services are (%d)\n",__FUNCTION__, __LINE__, ulSvcCnt);
	}
	if (phSvcList != NULL )
	{
		for(ulSvcIdx=0; ulSvcIdx< ulSvcCnt; ulSvcIdx++)
		{
			hSvc = phSvcList[ulSvcIdx];
			HxSTD_MemSet(&stSvcInfo, 0x00, sizeof(SVC_Info_t));
			hError = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
			if (hError == ERR_OK)
			{
				//<source sid="505" sref="dvb://2.2045.6301" default-content-id="23DA" logo="http://www.bbc.com/images/bbc1logo.gif" lcn="101" />
				HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%d", stSvcInfo.usFSatSvcIdentifier); //sid
				st_rcAttr.pucAttr1 = szAttrBuf[0];
				HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "dvb://%d.%d.%d", stSvcInfo.usOnId,stSvcInfo.usTsId,stSvcInfo.usSvcId); //sref
				st_rcAttr.pucAttr2 = szAttrBuf[1];

				if(VK_CLOCK_GetTime((unsigned long*)&uxCurrentTime) == ERR_OK)
				{
					hError = MW_EPG_GetEventIdAndNameByTime(hSvc, uxCurrentTime, &usEvtId, EvtName, EVT_NAME_LEN_MAX);
					if(hError == ERR_OK)
					{
		 				HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%X",usEvtId); //default-content-id (current time event id)
						st_rcAttr.pucAttr3 = szAttrBuf[2];
					}
				}
				if(stSvcInfo.eSvcType == eSvcType_Radio)
					st_rcAttr.pucAttr4 = "true";//audio-only
				else
					st_rcAttr.pucAttr4 = "";//audio-only

				st_rcAttr.pucAttr5 = ""; //chanel logo
				HxSTD_snprintf(szAttrBuf[4], STRING_LEN_MAX, "%d",stSvcInfo.usLcn); //lcn
				st_rcAttr.pucAttr6 = szAttrBuf[4];
				hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_CHANNELS_INFO, &st_rcAttr, NULL);
				uiTotalBufSize = ulNewTotalBufSize;
			}
		}


	}
	else
	{
		hSvc = HANDLE_NULL;
	}
	if (phSvcList != NULL)
	{
		DB_SVC_FreeServiceList (phSvcList);
	}

	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_CHANNELS_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_CHANNELS_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	VK_memcpy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);
	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HxMEM_FREE(pucTempUploadBuf);
	pucTempUploadBuf = NULL;
	HxLOG_Print("[%s][%d] (%s)-\n", __FUNCTION__,__LINE__,pucUploadBuf);

	return hError;

}
#endif

#if 0		// GET Storage
HERROR Get_RC_Storage(HUINT8 *pucUploadBuf,HUINT32 *ulUploadLen)
{
	HERROR			 		hError = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	MW_PvrService_t			stPvrSvcInfo;

	RC_ATTR_ELEMENT_t		st_rcAttr;
	HERROR			 		hErr = ERR_OK;
	HUINT32					uiDeviceNum, uiCnt=0, uiPartNum, uiPartIdx=0;
	MW_FS_DevIdx_t			*peDevIdxArray =NULL;
	MW_FS_DevInfo_t	 		stDevInfo = {0,};
	MW_FsPartitionInfo_t	*pstPartArray = NULL, *pstPartInfo;
	HUINT8					szAttrBuf[5][STRING_LEN_MAX];
	HUINT64 				ullTotalKbSize = 0, ullUsedKbSize = 0, ullTsrTotalSize = 0;
	HUINT64 				ullPhysicalDevSizeKb = 0;
	HCHAR					szTsrPath[MW_FS_PVR_DIR_PATH_LEN] =  {0,};
	HCHAR					ucStartupPath[MW_FS_PVR_DIR_PATH_LEN] =  {0,};
	RC_DataType_t			stData;
	HUINT32			        *puiContentId = NULL;
	HUINT32                 uiContentNum = 0, idx = 0;
	MW_PvrRecord_t	        stRecInfo;

	DATE_TIME_T 	stDate;
	MW_FS_FileStat_t out_file_stat;
	HCHAR					ucFilePath[MW_FS_PVR_DIR_PATH_LEN] =  {0,};
	MW_PvrEvent_t		stEventInfo;
	HUINT8					tempBuf[PVR_EVENT_TEXT];
	HUINT32					uiTotalBufSize = 0,ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;


	/* element load */
	hError = local_rc_loadElement(RC_ELEMENT_LIST_STORAGE, &pstElement);
	if( hError != ERR_OK )
	{
		HxLOG_Error("[%s][%d] Get_RC_Storage load element ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}

	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));

	st_rcAttr.pucAttr1 = s_pszresources[eResourceType_Storage];// "/rc/storage"

	pucTempUploadBuf = HxMEM_MEMDUP(pucUploadBuf,PACKET_MAX_BUF_SIZE);
	uiTotalBufSize  = 0;
	hError = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;
	// <response>
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hErr = MW_FS_GetDeviceIndexArray (&uiDeviceNum, &peDevIdxArray);	// Device 개수를 얻어 온다.
	if(hErr == ERR_OK && (peDevIdxArray!=NULL))
	{
		for (uiCnt = 0; uiCnt < uiDeviceNum; uiCnt++)
		{
			hErr = MW_FS_GetMountedDeviceInfo (peDevIdxArray[uiCnt], &stDevInfo);	// Device 정보를 얻어 온다.
			if (hErr != ERR_OK || IS_DEVICETYPE_HDD(stDevInfo.eDevType) == FALSE )	// ERROR 및 HDD 걸러내기.
			{
				continue;
			}

			/* HDD는 존재한다, 하지만 Partition이 있는가??? 확인한다. */
			hErr = MW_FS_GetPartitionInfoArray (peDevIdxArray[uiCnt], &uiPartNum, &pstPartArray); // STB의 Device Partition 정보를 얻어 온다.
			if ( (hErr != ERR_OK) || (uiPartNum == 0) || (pstPartArray == NULL) )
			{
				/* Partition이 없다 !! */
				uiPartNum = 0;
				goto CLOSE_FUNC;
			}
			for (uiPartIdx = 0; uiPartIdx < uiPartNum; uiPartIdx++)
			{
				pstPartInfo = NULL;
				pstPartInfo = &(pstPartArray[uiPartIdx]);

				if (pstPartInfo->bAvailable == TRUE && pstPartInfo->bMounted == TRUE && pstPartInfo->bPvrFlag == TRUE && pstPartInfo->ulPvrIdx == MW_FS_DEFAULT_PVR_INDEX)
				{
					hErr = MW_FS_GetMountedPartitionStatus(peDevIdxArray[uiCnt], uiPartIdx, &ullTotalKbSize, &ullUsedKbSize);
					if ( hErr != ERR_OK )
					{
						uiPartNum = 0;
						HxLOG_Error("[local_status_client_HardDiskProcess] ERROR\n");
						continue;
					}
					hErr = MW_FS_GetPhysicalDeviceSize(peDevIdxArray[uiCnt], &ullPhysicalDevSizeKb);
					if (hErr != ERR_OK)
					{
						HxLOG_Error("[%s][%d] Get Device Size Fail\n", __FUNCTION__, __LINE__);
						continue;
					}

					HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s%d",s_pszDevString[stDevInfo.eDevIdx],uiPartIdx+1); //sid
					st_rcAttr.pucAttr1 = szAttrBuf[0];

					stData.uiDecimal = (HUINT32)(ullPhysicalDevSizeKb*1.073741); // 1.073 ~~ (1.024*1.024*1.024)
					HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%d",stData.uiDecimal); //totalsize
					st_rcAttr.pucAttr2 = szAttrBuf[1];

					szTsrPath[0] = '\0';
					MW_FS_GetPvrPath (MW_FS_DEFAULT_PVR_INDEX, eMW_FS_UsageType_Tsr, szTsrPath);	// TSR이면	MW_FS_DEFAULT_PVR_INDEX 맞다.

					// 실제 TSR 파일의 크기를 구한다.
					ullTsrTotalSize = MWC_FILE_GetDirUsage(szTsrPath);
					if( ullTsrTotalSize > 0 )
					{
						ullTsrTotalSize /= SIZE_1K;  // Byte 단위이므로 KB 로 변경한다.

						if(ullUsedKbSize >= ullTsrTotalSize)
						{
							ullUsedKbSize -= ullTsrTotalSize;
						}

#if defined(CONFIG_MW_MM_DELAYED_REC)
						if((ullUsedKbSize + 41943040) >= ullTotalKbSize)
						{
							ullUsedKbSize = ullTotalKbSize - 41943040; // 40 * 1024 * 1024
						}
#else
						if((ullUsedKbSize + 20971520) >= ullTotalKbSize)
						{
							ullUsedKbSize = ullTotalKbSize - 20971520; // 20 * 1024 * 1024
						}
#endif
					}

#if defined(CONFIG_MW_MM_DELAYED_REC)
					stData.uiDecimal = (HUINT32)((ullTotalKbSize - ullUsedKbSize - 41943040) * 1.073741); // 1.073 ~~ (1.024*1.024*1.024)
#else
					stData.uiDecimal = (HUINT32)((ullTotalKbSize - ullUsedKbSize - 20971520) * 1.073741);
#endif


					HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%d",stData.uiDecimal); //freesize
					st_rcAttr.pucAttr3 = szAttrBuf[2];

					if(stDevInfo.eDevType == eMW_FS_DevType_IntHdd) //external
						st_rcAttr.pucAttr4 = "false";
					else if(stDevInfo.eDevType == eMW_FS_DevType_ExtHdd)
						st_rcAttr.pucAttr4 = "true";


					HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s",stDevInfo.bWriteProtected?"true":"false"); //write
					st_rcAttr.pucAttr5 = szAttrBuf[3];

					hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_OPEN, &st_rcAttr, NULL);
					uiTotalBufSize = ulNewTotalBufSize;


					hErr = MW_FS_GetPvrPath (MW_FS_DEFAULT_PVR_INDEX, eMW_FS_UsageType_Record, ucStartupPath);
					if(hErr == ERR_OK)
					{
						hErr = MW_PVR_GetContentsIdList(&uiContentNum, &puiContentId);
						if(hErr == ERR_OK && uiContentNum > 0)
						{
							for(idx = 0; idx < uiContentNum; idx++)
							{
								HxSTD_MemSet(&stRecInfo, 0x00, sizeof(MW_PvrRecord_t));
								hErr = MW_PVR_GetRecordInfo(puiContentId[idx], &stRecInfo);
								// TSR을 제외 하기 위한 기준을 status != start 로 잡는다.
								if(hErr != ERR_OK
									|| stRecInfo.ucRecordStatus == ePVR_REC_STS_RECORD_START)
								{

									HxLOG_Error("[%s][%d]%s%s\n",__FUNCTION__,__LINE__,stRecInfo.aucPath,stRecInfo.aucFileName);
									continue;
								}

								HxSTD_MemSet(&stPvrSvcInfo, 0x00, sizeof(MW_PvrService_t));
								hErr = MW_PVR_GetServiceInfo(puiContentId[idx], &stPvrSvcInfo);

								HxLOG_Print("[%s][%d]%s%s\n",__FUNCTION__,__LINE__,stRecInfo.aucPath,stRecInfo.aucFileName);

								HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));

								HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "R_%d_%d",stPvrSvcInfo.nChNumber,stRecInfo.ulStartTime); //cid
						//		HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%d",puiContentId[idx]); //cid
								st_rcAttr.pucAttr1 = szAttrBuf[0];
								HxLOG_Print("[%s][%d]%s%s\n",__FUNCTION__,__LINE__,stRecInfo.aucPath,stRecInfo.aucFileName);

								hErr = UTIL_TCONV_ConvertUnixTimeToDateTime(stRecInfo.ulStartTime, &stDate);

								HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%04d-%02d-%02dT%02d:%02d:%02dZ",stDate.stDate.usYear,stDate.stDate.ucMonth,stDate.stDate.ucDay,stDate.stTime.ucHour,stDate.stTime.ucMinute,stDate.stTime.ucSecond); //created-time
								st_rcAttr.pucAttr2 = szAttrBuf[1];

								HxSTD_snprintf(ucFilePath, MW_FS_PVR_DIR_PATH_LEN, "%s%s", stRecInfo.aucPath, stRecInfo.aucFileName);

								hErr = MWC_FILE_GetFileInfo (ucFilePath, &out_file_stat);
								if (hErr != ERR_OK)
								{
									st_rcAttr.pucAttr3 = "";
								}
								else
								{
									HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%lld",out_file_stat.size); //orig_sid
									st_rcAttr.pucAttr3 = szAttrBuf[2];
								}
#if 0
								HxSTD_MemSet(aucFileName, 0, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
								HxSTD_snprintf(aucFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH, "%s%s%s",
											stRecInfo.aucPath,
											stRecInfo.aucFileName,  EXT_THUMBNAIL_FILE);
								HxSTD_MemSet(aucThumbnail, 0, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
								HxSTD_snprintf(aucThumbnail,PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH,"%s%s", stRecInfo.aucFileName, EXT_THUMBNAIL_FILE);
#endif

								HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%d",stPvrSvcInfo.nChNumber); //orig_sid  == channl number
								st_rcAttr.pucAttr4 = szAttrBuf[3];

								HxSTD_snprintf(szAttrBuf[4], STRING_LEN_MAX, "%s",stRecInfo.aucDisplayEvtName); //title
								st_rcAttr.pucAttr5 = szAttrBuf[4];

								HxSTD_snprintf(szAttrBuf[5], STRING_LEN_MAX, "%04d-%02d-%02dT%02d:%02d:%02dZ",stDate.stDate.usYear,stDate.stDate.ucMonth,stDate.stDate.ucDay,stDate.stTime.ucHour,stDate.stTime.ucMinute,stDate.stTime.ucSecond);  //start
								st_rcAttr.pucAttr6 = szAttrBuf[5];

								HxSTD_snprintf(szAttrBuf[6], STRING_LEN_MAX, "%d",stRecInfo.ulEndTime - stRecInfo.ulStartTime); //duration
								st_rcAttr.pucAttr7 = szAttrBuf[6];

								hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_STORED_CONTENT_OPEN, &st_rcAttr, NULL);
								uiTotalBufSize = ulNewTotalBufSize;


								HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%d",puiContentId[idx]); //cid
								st_rcAttr.pucAttr1 = szAttrBuf[0];
								hErr = MW_PVR_GetEventInfo(puiContentId[idx],&stEventInfo);


								if(hErr != ERR_OK)
								{
									stData.pString = "";
									hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_SYNOPSIS, NULL, &stData);
									uiTotalBufSize = ulNewTotalBufSize;
								}
								else
								{
									if (HxSTD_StrLen(stEventInfo.aucEventText ) <= 0)
									{
										stData.pString = "";
										hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_SYNOPSIS, NULL, &stData);
										uiTotalBufSize = ulNewTotalBufSize;
									}
									else
									{
										HxSTD_MemSet(tempBuf,0x00,PVR_EVENT_TEXT);
										stData.pString = "";

#if 0
										HxSTD_StrUtf8NCpy((char *)stEventInfo.aucEventText, stEventInfo.aucEventText, PVR_EVENT_TEXT);
										MWC_UTIL_EliminateShyAndControlCharacter(stEventInfo.aucEventText, MWC_UTIL_DvbStrlen(pstGuidance->szGuidanceText));
										local_rc_SetReplaceString(stEventInfo.aucEventText, tempBuf, NULL, PVR_EVENT_TEXT);

										stData.pString =  (HUINT8 *)tempBuf;
#endif
										hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_SYNOPSIS, NULL, &stData);
										uiTotalBufSize = ulNewTotalBufSize;
									}
								}
								//Not supported ...... Category
						//		hErr = local_rc_common(pstElement, RC_STORAGE_CATEGORY, &st_rcAttr,NULL);

								//media-component (video, audio, subtitle)
								if(stPvrSvcInfo.usVideoPid != PID_NULL)
								{
									HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s","video"); //video
									st_rcAttr.pucAttr1 = szAttrBuf[0];

//									HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s",""); //aspect
									st_rcAttr.pucAttr2 = "";

									HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s","video"); //mcid
									st_rcAttr.pucAttr3 = szAttrBuf[1];

									HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%s","mainvideo"); //name
									st_rcAttr.pucAttr4 = szAttrBuf[2];

									if(stPvrSvcInfo.eVideoType == 1) //sd
										HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","sd"); //vformat
									else
										HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","hd"); //vformat
									st_rcAttr.pucAttr5 = szAttrBuf[3];


									HxSTD_snprintf(szAttrBuf[4], STRING_LEN_MAX, "%s","true"); //default true
									st_rcAttr.pucAttr6 = szAttrBuf[4];

									hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_MEDIA_VIDEO_COMPONENT, &st_rcAttr,NULL);
									uiTotalBufSize = ulNewTotalBufSize;
								}
//Audio
								if(stPvrSvcInfo.usAudioPid != PID_NULL)
								{
									HUINT16	i,j;
									HUINT16	count = 0;
									for(i = 0;i<stPvrSvcInfo.stAudioInfoList.ulItemNum;i++)
									{
										META_PvrAudioInfo_t pstAudioInfo = stPvrSvcInfo.stAudioInfoList.astAudioInfo[i];

										for (j = 0; j < pstAudioInfo.ulLangNum; ++j)
										{

											count++;

											//type : audio
											//name :audio description
											//mcid :audio1
											//lang : UK English
											HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s","audio"); //audio
											st_rcAttr.pucAttr1 = szAttrBuf[0];
											st_rcAttr.pucAttr2= "";
											HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s%d","audio",count); // mcid
											st_rcAttr.pucAttr3 = szAttrBuf[1];
											HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%s",pstAudioInfo.stLangInfo[j].szIso639LangCode); // lang
											st_rcAttr.pucAttr4 = szAttrBuf[2];

											if(stPvrSvcInfo.eSoundMode == 1)
												HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","stereo");
											else if(stPvrSvcInfo.eSoundMode == 2 || stPvrSvcInfo.eSoundMode == 3)
												HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","dualmono");
											else if(stPvrSvcInfo.eSoundMode == 4)
												HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","mix");
											else
												HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","");
											st_rcAttr.pucAttr5 = szAttrBuf[3];

											HxSTD_snprintf(szAttrBuf[4], STRING_LEN_MAX, "%s","false"); //default
											st_rcAttr.pucAttr6 = szAttrBuf[4];

											hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_MEDIA_AUDIO_COMPONENT, &st_rcAttr,NULL);
											uiTotalBufSize = ulNewTotalBufSize;

										}
									}

								}
//Subtitles
								{
									HUINT16	i;
									HUINT16	count = 0;
									for(i = 0;i<stPvrSvcInfo.stSbtlInfoList.ulItemNum;i++)
									{
										META_PvrSbtlInfo_t pstSubtitleInfo = stPvrSvcInfo.stSbtlInfoList.astSbtlInfo[i];

										count++;

										//type : subtitle
										//mcid :audio1
										//lang : UK English
										HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s","subtitles"); //subtitle
										st_rcAttr.pucAttr1 = szAttrBuf[0];
										HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s",pstSubtitleInfo.aucLangCode); // lang
										st_rcAttr.pucAttr2 = szAttrBuf[1];
										HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%s%d","subtitles",count); // mcid
										st_rcAttr.pucAttr3 = szAttrBuf[2];
										st_rcAttr.pucAttr4= "";

										if(stPvrSvcInfo.ucSubttlIdx == i)
											HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","true"); // default
										else
											HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","false"); // default
										st_rcAttr.pucAttr5 = szAttrBuf[3];
										hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_MEDIA_SUBTITLE_COMPONENT, &st_rcAttr,NULL);
										uiTotalBufSize = ulNewTotalBufSize;

									}

								}
//	Thumnail Image...



#if 0

								bExist = PAL_PVR_IsFileExist(aucFileName);
								if(TRUE == bExist)
								{
									stData.pString =  (HUINT8 *)aucThumbnail;

									hErr = local_rc_common(pstElement, RC_STORAGE_IMAGE, NULL, &stData);
								}
								else
								{
									hErr = local_rc_common(pstElement, RC_STORAGE_IMAGE, NULL, "");
								}
#endif
								stData.pString = "";
								hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_IMAGE, NULL, &stData);
								uiTotalBufSize = ulNewTotalBufSize;

								hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_STORED_CONTENT_CLOSE, NULL, NULL);
								uiTotalBufSize = ulNewTotalBufSize;

							}
						}
						MW_PVR_FreeContentsIdList(puiContentId);
					}

					hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_CLOSE, NULL, NULL);
					uiTotalBufSize = ulNewTotalBufSize;
				}

			}


		}
	}
CLOSE_FUNC:

	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;


	VK_memcpy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);
	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HxMEM_FREE(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	if(pstPartArray)
	{
		MW_FS_FreePartitionInfoArray(pstPartArray);
		pstPartInfo = NULL;
	}
	if(peDevIdxArray)
		MW_FS_FreeDeviceIndexArray(peDevIdxArray);
	HxLOG_Print("[%s][%d] (%s)-\n", __FUNCTION__,__LINE__,pucUploadBuf);

	return hError;

}
#endif

#if 0		// GET Current Storage
HERROR Get_RC_Current_Storage(HUINT8 *pucUploadBuf,HUINT8 *pUserData1,HUINT32 *ulUploadLen)
{
	HERROR			 		hError = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	MW_PvrService_t			stPvrSvcInfo;

	RC_ATTR_ELEMENT_t		st_rcAttr;
	HERROR			 		hErr = ERR_OK;
	HUINT32					uiDeviceNum, uiCnt=0, uiPartNum, uiPartIdx=0;
	MW_FS_DevIdx_t			*peDevIdxArray =NULL;
	MW_FS_DevInfo_t	 		stDevInfo = {0,};
	MW_FsPartitionInfo_t	*pstPartArray = NULL, *pstPartInfo = NULL;
	HUINT8					szAttrBuf[5][STRING_LEN_MAX];
	HUINT64 				ullTotalKbSize = 0, ullUsedKbSize = 0, ullTsrTotalSize = 0;
	HUINT64 				ullPhysicalDevSizeKb = 0;
	HCHAR					szTsrPath[MW_FS_PVR_DIR_PATH_LEN] =  {0,};
	HCHAR					ucStartupPath[MW_FS_PVR_DIR_PATH_LEN] =  {0,};
	RC_DataType_t			stData;
	HUINT32			        *puiContentId = NULL;
	HUINT32                 uiContentNum = 0, idx = 0;
	MW_PvrRecord_t	        stRecInfo;

	DATE_TIME_T 	stDate;
	MW_FS_FileStat_t out_file_stat;
	HCHAR					ucFilePath[MW_FS_PVR_DIR_PATH_LEN] =  {0,};
	MW_PvrEvent_t		stEventInfo;
	HINT32					i = 0;
	HCHAR					ucDevicePath[MW_FS_PVR_DIR_PATH_LEN];
	HCHAR					ucTempDevicePath[MW_FS_PVR_DIR_PATH_LEN];
	HUINT16					pos = 0;
	HUINT32					uiTotalBufSize = 0,ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;



	/* element load */
	hError = local_rc_loadElement(RC_ELEMENT_LIST_STORAGE, &pstElement);
	if( hError != ERR_OK )
	{
		HxLOG_Error("[%s][%d] Get_RC_Storage load element ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}


	for(i = 0;i <HxSTD_StrLen(pUserData1);i++)
	{
		if(HxSTD_StrNCmp((char *)&pUserData1[i],"/",1) == 0)
		{
			pos = i;
		}
	}
	if(pos+1 >= HxSTD_StrLen(pUserData1) )
	{
		return hError;
	}

	HxSTD_snprintf(ucDevicePath, MW_FS_PVR_DIR_PATH_LEN, "/dev/%s",&pUserData1[pos+1]); //sid


	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
	HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s",pUserData1); //sid == /dev/sda1
	st_rcAttr.pucAttr1 = szAttrBuf[0];

	pucTempUploadBuf = HxMEM_MEMDUP(pucUploadBuf,PACKET_MAX_BUF_SIZE);
	uiTotalBufSize  = 0;

	hError = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;
	// <response>
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	hErr = MW_FS_GetDeviceIndexArray (&uiDeviceNum, &peDevIdxArray);	// Device 개수를 얻어 온다.
	if(hErr == ERR_OK && (peDevIdxArray!=NULL))
	{
		for (uiCnt = 0; uiCnt < uiDeviceNum; uiCnt++)
		{
			hErr = MW_FS_GetMountedDeviceInfo (peDevIdxArray[uiCnt], &stDevInfo);	// Device 정보를 얻어 온다.
			if (hErr != ERR_OK || IS_DEVICETYPE_HDD(stDevInfo.eDevType) == FALSE )	// ERROR 및 HDD 걸러내기.
			{
				continue;
			}

			/* HDD는 존재한다, 하지만 Partition이 있는가??? 확인한다. */
			hErr = MW_FS_GetPartitionInfoArray (peDevIdxArray[uiCnt], &uiPartNum, &pstPartArray); // STB의 Device Partition 정보를 얻어 온다.
			if ( (hErr != ERR_OK) || (uiPartNum == 0) || (pstPartArray == NULL) )
			{
				/* Partition이 없다 !! */
				uiPartNum = 0;
				goto CLOSE_FUNC;
			}
			for (uiPartIdx = 0; uiPartIdx < uiPartNum; uiPartIdx++)
			{
				pstPartInfo = &(pstPartArray[uiPartIdx]);

				if (pstPartInfo->bAvailable == TRUE && pstPartInfo->bMounted == TRUE && pstPartInfo->bPvrFlag == TRUE && pstPartInfo->ulPvrIdx == MW_FS_DEFAULT_PVR_INDEX)
				{
					hErr = MW_FS_GetMountedPartitionStatus(peDevIdxArray[uiCnt], uiPartIdx, &ullTotalKbSize, &ullUsedKbSize);
					if ( hErr != ERR_OK )
					{
						uiPartNum = 0;
						HxLOG_Error("[local_status_client_HardDiskProcess] ERROR\n");
						continue;
					}
					hErr = MW_FS_GetPhysicalDeviceSize(peDevIdxArray[uiCnt], &ullPhysicalDevSizeKb);
					if (hErr != ERR_OK)
					{
						HxLOG_Error("[%s][%d] Get Device Size Fail\n", __FUNCTION__, __LINE__);
						continue;
					}


					HxSTD_snprintf(ucTempDevicePath, MW_FS_PVR_DIR_PATH_LEN, "%s%d",s_pszDevString[stDevInfo.eDevIdx],uiPartIdx+1); //sid

					if(HLIB_STD_StrCaseCmp(ucTempDevicePath,ucDevicePath) != 0)
					{
						continue;
					}

					HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s%d",s_pszDevString[stDevInfo.eDevIdx],uiPartIdx+1); //sid
					st_rcAttr.pucAttr1 = szAttrBuf[0];

					stData.uiDecimal = (HUINT32)(ullPhysicalDevSizeKb*1.073741); // 1.073 ~~ (1.024*1.024*1.024)
					HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%d",stData.uiDecimal); //totalsize
					st_rcAttr.pucAttr2 = szAttrBuf[1];

					szTsrPath[0] = '\0';
					MW_FS_GetPvrPath (MW_FS_DEFAULT_PVR_INDEX, eMW_FS_UsageType_Tsr, szTsrPath);	// TSR이면	MW_FS_DEFAULT_PVR_INDEX 맞다.

					// 실제 TSR 파일의 크기를 구한다.
					ullTsrTotalSize = MWC_FILE_GetDirUsage(szTsrPath);
					if( ullTsrTotalSize > 0 )
					{
						ullTsrTotalSize /= SIZE_1K;  // Byte 단위이므로 KB 로 변경한다.

						if(ullUsedKbSize >= ullTsrTotalSize)
						{
							ullUsedKbSize -= ullTsrTotalSize;
						}

#if defined(CONFIG_MW_MM_DELAYED_REC)
						if((ullUsedKbSize + 41943040) >= ullTotalKbSize)
						{
							ullUsedKbSize = ullTotalKbSize - 41943040; // 40 * 1024 * 1024
						}
#else
						if((ullUsedKbSize + 20971520) >= ullTotalKbSize)
						{
							ullUsedKbSize = ullTotalKbSize - 20971520; // 20 * 1024 * 1024
						}
#endif
					}

#if defined(CONFIG_MW_MM_DELAYED_REC)
					stData.uiDecimal = (HUINT32)((ullTotalKbSize - ullUsedKbSize - 41943040) * 1.073741); // 1.073 ~~ (1.024*1.024*1.024)
#else
					stData.uiDecimal = (HUINT32)((ullTotalKbSize - ullUsedKbSize - 20971520) * 1.073741);
#endif


					HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%d",stData.uiDecimal); //freesize
					st_rcAttr.pucAttr3 = szAttrBuf[2];

					if(stDevInfo.eDevType == eMW_FS_DevType_IntHdd) //external
						st_rcAttr.pucAttr4 = "false";
					else if(stDevInfo.eDevType == eMW_FS_DevType_ExtHdd)
						st_rcAttr.pucAttr4 = "true";


					HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s",stDevInfo.bWriteProtected?"true":"false"); //write
					st_rcAttr.pucAttr5 = szAttrBuf[3];

					hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_OPEN, &st_rcAttr, NULL);
					uiTotalBufSize = ulNewTotalBufSize;


					hErr = MW_FS_GetPvrPath (MW_FS_DEFAULT_PVR_INDEX, eMW_FS_UsageType_Record, ucStartupPath);
					if(hErr == ERR_OK)
					{
						hErr = MW_PVR_GetContentsIdList(&uiContentNum, &puiContentId);
						if(hErr == ERR_OK && uiContentNum > 0)
						{
							for(idx = 0; idx < uiContentNum; idx++)
							{
								HxSTD_MemSet(&stRecInfo, 0x00, sizeof(MW_PvrRecord_t));
								hErr = MW_PVR_GetRecordInfo(puiContentId[idx], &stRecInfo);
								if(hErr != ERR_OK || (stRecInfo.ulRecTime < 30))
								{
									HxLOG_Error("[%s][%d]%s%s\n",__FUNCTION__,__LINE__,stRecInfo.aucPath,stRecInfo.aucFileName);
									continue;
								}

								HxSTD_MemSet(&stPvrSvcInfo, 0x00, sizeof(MW_PvrService_t));
								hErr = MW_PVR_GetServiceInfo(puiContentId[idx], &stPvrSvcInfo);

								HxLOG_Print("[%s][%d]%s%s\n",__FUNCTION__,__LINE__,stRecInfo.aucPath,stRecInfo.aucFileName);

								HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
								HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%d",puiContentId[idx]); //cid
								st_rcAttr.pucAttr1 = szAttrBuf[0];
								HxLOG_Print("[%s][%d]%s%s\n",__FUNCTION__,__LINE__,stRecInfo.aucPath,stRecInfo.aucFileName);

								hErr = UTIL_TCONV_ConvertUnixTimeToDateTime(stRecInfo.ulStartTime, &stDate);
								HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%04d-%02d-%02dT%02d:%02d:%02dZ",stDate.stDate.usYear,stDate.stDate.ucMonth,stDate.stDate.ucDay,stDate.stTime.ucHour,stDate.stTime.ucMinute,stDate.stTime.ucSecond);  //start
								st_rcAttr.pucAttr2 = szAttrBuf[1];

								HxSTD_snprintf(ucFilePath, MW_FS_PVR_DIR_PATH_LEN, "%s%s", stRecInfo.aucPath, stRecInfo.aucFileName);

								hErr = MWC_FILE_GetFileInfo (ucFilePath, &out_file_stat);
								if (hErr != ERR_OK)
								{
									st_rcAttr.pucAttr3 = "";
								}
								else
								{
									HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%lld",out_file_stat.size); //orig_sid
									st_rcAttr.pucAttr3 = szAttrBuf[2];
								}
#if 0
								HxSTD_MemSet(aucFileName, 0, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
								HxSTD_snprintf(aucFileName, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH, "%s%s%s",
											stRecInfo.aucPath,
											stRecInfo.aucFileName,  EXT_THUMBNAIL_FILE);
								HxSTD_MemSet(aucThumbnail, 0, PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH);
								HxSTD_snprintf(aucThumbnail,PAL_PVR_MAX_FILE_NAME_FULLPATH_LENGTH,"%s%s", stRecInfo.aucFileName, EXT_THUMBNAIL_FILE);
#endif

								HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%d",stPvrSvcInfo.nChNumber); //orig_sid  == channl number
								st_rcAttr.pucAttr4 = szAttrBuf[3];

								HxSTD_snprintf(szAttrBuf[4], STRING_LEN_MAX, "%s",stRecInfo.aucDisplayEvtName); //title
								st_rcAttr.pucAttr5 = szAttrBuf[4];

								HxSTD_snprintf(szAttrBuf[5], STRING_LEN_MAX, "%04d-%02d-%02dT%02d:%02d:%02dZ",stDate.stDate.usYear,stDate.stDate.ucMonth,stDate.stDate.ucDay,stDate.stTime.ucHour,stDate.stTime.ucMinute,stDate.stTime.ucSecond);  //start
								st_rcAttr.pucAttr6 = szAttrBuf[5];

								HxSTD_snprintf(szAttrBuf[6], STRING_LEN_MAX, "%d",stRecInfo.ulEndTime - stRecInfo.ulStartTime); //duration
								st_rcAttr.pucAttr7 = szAttrBuf[6];

								hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_STORED_CONTENT_OPEN, &st_rcAttr, NULL);
								uiTotalBufSize = ulNewTotalBufSize;


								HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%d",puiContentId[idx]); //cid
								st_rcAttr.pucAttr1 = szAttrBuf[0];
								hErr = MW_PVR_GetEventInfo(puiContentId[idx],&stEventInfo);


								if(hErr != ERR_OK)
								{
									stData.pString = "";
									hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_SYNOPSIS, NULL, &stData);
									uiTotalBufSize = ulNewTotalBufSize;
								}
								else
								{
									if (HxSTD_StrLen(stEventInfo.aucEventText ) <= 0)
									{
										stData.pString = "";
										hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_SYNOPSIS, NULL, &stData);
										uiTotalBufSize = ulNewTotalBufSize;
									}
									else
									{
#if 0 //synopsis char 오류로 인하여 xml parsing 이 않됨

										HxSTD_MemSet(tempBuf,0x00,PVR_EVENT_TEXT);
										local_rc_SetReplaceString(stEventInfo.aucEventText, tempBuf, NULL, PVR_EVENT_TEXT);

										stData.pString =  (HUINT8 *)tempBuf;
#else
										stData.pString =  (HUINT8 *)"";
#endif
										hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_SYNOPSIS, NULL, &stData);
										uiTotalBufSize = ulNewTotalBufSize;
									}
								}
								//Not supported ...... Category
						//		hErr = local_rc_common(pstElement, RC_STORAGE_CATEGORY, &st_rcAttr,NULL);

								//media-component (video, audio, subtitle)
								if(stPvrSvcInfo.usVideoPid != PID_NULL)
								{
									HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s","video"); //video
									st_rcAttr.pucAttr1 = szAttrBuf[0];

//									HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s",""); //aspect
									st_rcAttr.pucAttr2 = "";

									HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s","video"); //mcid
									st_rcAttr.pucAttr3 = szAttrBuf[1];

									HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%s","mainvideo"); //name
									st_rcAttr.pucAttr4 = szAttrBuf[2];

									if(stPvrSvcInfo.eVideoType == 1) //sd
										HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","sd"); //vformat
									else
										HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","hd"); //vformat
									st_rcAttr.pucAttr5 = szAttrBuf[3];


									HxSTD_snprintf(szAttrBuf[4], STRING_LEN_MAX, "%s","true"); //default true
									st_rcAttr.pucAttr6 = szAttrBuf[4];

									hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_MEDIA_VIDEO_COMPONENT, &st_rcAttr,NULL);
									uiTotalBufSize = ulNewTotalBufSize;
								}
//Audio
								if(stPvrSvcInfo.usAudioPid != PID_NULL)
								{
									HUINT16	i,j;
									HUINT16	count = 0;
									for(i = 0;i<stPvrSvcInfo.stAudioInfoList.ulItemNum;i++)
									{
										META_PvrAudioInfo_t pstAudioInfo = stPvrSvcInfo.stAudioInfoList.astAudioInfo[i];

										for (j = 0; j < pstAudioInfo.ulLangNum; ++j)
										{

											count++;

											//type : audio
											//name :audio description
											//mcid :audio1
											//lang : UK English
											HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s","audio"); //audio
											st_rcAttr.pucAttr1 = szAttrBuf[0];
											st_rcAttr.pucAttr2= "";
											HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s%d","audio",count); // mcid
											st_rcAttr.pucAttr3 = szAttrBuf[1];
											HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%s",pstAudioInfo.stLangInfo[j].szIso639LangCode); // lang
											st_rcAttr.pucAttr4 = szAttrBuf[2];

											if(stPvrSvcInfo.eSoundMode == 1)
												HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","stereo");
											else if(stPvrSvcInfo.eSoundMode == 2 || stPvrSvcInfo.eSoundMode == 3)
												HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","dualmono");
											else if(stPvrSvcInfo.eSoundMode == 4)
												HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","mix");
											else
												HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","");
											st_rcAttr.pucAttr5 = szAttrBuf[3];

											HxSTD_snprintf(szAttrBuf[4], STRING_LEN_MAX, "%s","false"); //default
											st_rcAttr.pucAttr6 = szAttrBuf[4];

											hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_MEDIA_AUDIO_COMPONENT, &st_rcAttr,NULL);
											uiTotalBufSize = ulNewTotalBufSize;

										}
									}

								}
//Subtitles
								{
									HUINT16	i;
									HUINT16	count = 0;
									for(i = 0;i<stPvrSvcInfo.stSbtlInfoList.ulItemNum;i++)
									{
										META_PvrSbtlInfo_t pstSubtitleInfo = stPvrSvcInfo.stSbtlInfoList.astSbtlInfo[i];

										count++;

										//type : subtitle
										//mcid :audio1
										//lang : UK English
										HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s","subtitles"); //subtitle
										st_rcAttr.pucAttr1 = szAttrBuf[0];
										HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s",pstSubtitleInfo.aucLangCode); // lang
										st_rcAttr.pucAttr2 = szAttrBuf[1];
										HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%s%d","subtitles",count); // mcid
										st_rcAttr.pucAttr3 = szAttrBuf[2];
										st_rcAttr.pucAttr4= "";

										if(stPvrSvcInfo.ucSubttlIdx == i)
											HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","true"); // default
										else
											HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%s","false"); // default
										st_rcAttr.pucAttr5 = szAttrBuf[3];
										hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_MEDIA_SUBTITLE_COMPONENT, &st_rcAttr,NULL);
										uiTotalBufSize = ulNewTotalBufSize;

									}

								}
//	Thumnail Image...



#if 0

								bExist = PAL_PVR_IsFileExist(aucFileName);
								if(TRUE == bExist)
								{
									stData.pString =  (HUINT8 *)aucThumbnail;

									hErr = local_rc_common(pstElement, RC_STORAGE_IMAGE, NULL, &stData);
								}
								else
								{
									hErr = local_rc_common(pstElement, RC_STORAGE_IMAGE, NULL, "");
								}
#endif
								stData.pString = "";
								hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_IMAGE, NULL, &stData);
								uiTotalBufSize = ulNewTotalBufSize;

								hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_STORED_CONTENT_CLOSE, NULL, NULL);
								uiTotalBufSize = ulNewTotalBufSize;

							}
						}
						MW_PVR_FreeContentsIdList(puiContentId);
					}

					hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_CLOSE, NULL, NULL);
					uiTotalBufSize = ulNewTotalBufSize;
				}

			}


		}
	}
CLOSE_FUNC:

	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_STORAGE_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	VK_memcpy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);
	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HxMEM_FREE(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	if(pstPartArray)
	{
		MW_FS_FreePartitionInfoArray(pstPartArray);
		pstPartInfo = NULL;

	}
	if(peDevIdxArray)
		MW_FS_FreeDeviceIndexArray(peDevIdxArray);

	HxLOG_Print("[%s][%d] (%s)-\n", __FUNCTION__,__LINE__,pucUploadBuf);

	return hError;

}
#endif

HERROR Get_RC_Locale(HUINT8 *pucUploadBuf, HUINT32 *ulUploadLen)
{
	HERROR			 		hError = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	RC_ATTR_ELEMENT_t		st_rcAttr;
	HUINT32					uiIndex;
	RC_DataType_t			stData;
	HUINT32 				nCount = 0;
	HUINT32					uiTotalBufSize = 0, ulNewTotalBufSize = 0;
	HCHAR					*pucTempUploadBuf;

	/* element load */
	hError = local_rc_loadElement(RC_ELEMENT_LIST_LOCALE, &pstElement);
	if( hError != ERR_OK )
	{
		HxLOG_Print("[%s][%d]  load element ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}

	pucTempUploadBuf = HLIB_STD_MemDup(pucUploadBuf,RC_RESOURCE_SHORT_MAX_BUF_SIZE);
	if( pucTempUploadBuf == NULL )
	{
		HxLOG_Print("[%s][%d]  HLIB_STD_MemDup() ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}

	uiTotalBufSize  = 0;

	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));

	st_rcAttr.pucAttr1 = s_pszresources[eResourceType_Locale];// "/rc/locale/"

	hError = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;
	// <response>
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_LOCALE_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_LOCALE_OPEN, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	// locale info
	for(uiIndex = RC_LOCALE_DEVICEID; uiIndex <= RC_LOCALE_TUNERS; uiIndex++)
	{
		switch(uiIndex)
		{
			case RC_LOCALE_DEVICEID:
				{
					PL_COMMON_ProductInfo_t	product_info;
					PL_COMMON_GetProductInfo(&product_info);
					stData.pString   = (HUINT8 *)product_info.szSerialNumber;
				}
				break;
			case RC_LOCALE_POSTCODE:
//				MWC_PARAM_GetFsatPostcode ((HCHAR *)postcode, FS_MAX_POSTCODE_LEN);
				stData.pString	 = "TW8 9HH";
				break;
			case RC_LOCALE_TUNERS:
				{
#if 0
					SAT_AntConn_Type_t	connectionType;
					HUINT32				ulNumTunerGroup = 0;
					MW_CH_GetNumberOfTunerGroup(&ulNumTunerGroup);
					connectionType = MWC_PARAM_GetAntennaConnectionType();
					nCount = 0;
					if (connectionType == 0)
					{
						if (ulNumTunerGroup > 0)
						{
							nCount = 1;
						}
					}
					else if (ulNumTunerGroup > 1 && (connectionType == 1 || connectionType == 2))
					{
						nCount = 2;
					}
#else
					nCount = 2;
#endif
				}

				stData.uiDecimal= nCount;
				break;
		}
		hError =	local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, uiIndex, NULL, &stData);
		uiTotalBufSize = ulNewTotalBufSize;
	}

	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_LOCALE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_LOCALE_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	HxSTD_MemCopy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);
	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HLIB_STD_MemFree(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	HxLOG_Print("[%s][%d] (%s)-\n", __FUNCTION__, __LINE__, pucUploadBuf);

	return hError;
}

HERROR Get_RC_Remote(HUINT8 *pucUploadBuf, HUINT32 *ulUploadLen)
{
	HERROR			 		hError = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	RC_ATTR_ELEMENT_t		st_rcAttr;
//	HUINT8					szAttrBuf[STRING_LEN_MAX] = {0, };
	HUINT32					i;
	HUINT32					uiTotalBufSize = 0, ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;

	/* element load */
	hError = local_rc_loadElement(RC_ELEMENT_LIST_REMOTE, &pstElement);
	if( hError != ERR_OK )
	{
		HxLOG_Print("[%s][%d]  load element ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}

	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));

	st_rcAttr.pucAttr1 = s_pszresources[eResourceType_Remote];// "/rc/remote"

	pucTempUploadBuf = HLIB_STD_MemAlloc(RC_RESOURCE_SHORT_MAX_BUF_SIZE);
	if(pucTempUploadBuf == NULL)
	{
		HxLOG_Print("[%s][%d]  HLIB_STD_MemAlloc() ERROR \n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}
	uiTotalBufSize  = 0;

	hError = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;

	// <response>
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_REMOTE_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_REMOTE_OPEN, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	for(i = 0; i < ARRAY_SIZE(s_stRC_RemoteKeyMap); i++)
	{
		st_rcAttr.pucAttr1 = s_stRC_RemoteKeyMap[i].szKeyDesc;
		//<key>
		hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_REMOTE_KEY, &st_rcAttr, NULL);
		uiTotalBufSize = ulNewTotalBufSize;
	}

	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_REMOTE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_REMOTE_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	if(uiTotalBufSize > RC_RESOURCE_SHORT_MAX_BUF_SIZE)
	{
		HxLOG_Print("\t[%s][%d]  @@@ Remote Informatino is too large(%d)..!!! \n",__FUNCTION__,__LINE__, uiTotalBufSize);
		uiTotalBufSize = RC_RESOURCE_SHORT_MAX_BUF_SIZE;
	}
	HxSTD_MemCopy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);
	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HLIB_STD_MemFree(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	HxLOG_Print("[%s][%d] (%s)-\n", __FUNCTION__, __LINE__, pucUploadBuf);

	return hError;
}

HERROR Post_RC_Remote(HUINT8* pucBuf, HUINT32 ulContentLen)
{
	HERROR	hErr = ERR_FAIL;

	HxList_t	*pList = NULL;
	HxXMLDOC				hXmlDocHandle = NULL;
	HxXMLELEMENT			hResponseElement = NULL;
	HUINT8	*pszCmd = NULL;
	HUINT32		i;

	hErr = HLIB_XML_InitParser();
	hErr = HLIB_XML_ParseBuffer(pucBuf, ulContentLen, &hXmlDocHandle);
	hErr = HLIB_XML_GetRootElement(hXmlDocHandle, &hResponseElement);			// -------> <remote>
	hErr = HLIB_XML_FindChildElementByName(hResponseElement, "key", &pList);
	if(pList != NULL)
	{
		hErr = HLIB_XML_GetAttrContents((HxXMLELEMENT)HLIB_LIST_Data(pList), "code", &pszCmd);		// pszCmd : VK_XXX
		if(hErr == ERR_OK)
		{
			if(pszCmd != NULL)
			{
				for(i = 0; i < ARRAY_SIZE(s_stRC_RemoteKeyMap); i++)
				{
					if(HLIB_STD_StrCaseCmp(pszCmd, s_stRC_RemoteKeyMap[i].szKeyDesc) == 0)
					{
						HxLOG_Print("\n\t\t+++++ %s (0x%x) Key Received...\n", s_stRC_RemoteKeyMap[i].szKeyDesc, s_stRC_RemoteKeyMap[i].ulStbKeyCode);
						HAPI_SendRemoteRCUKey(s_stRC_RemoteKeyMap[i].ulStbKeyCode);
						break;
					}
				}
			}

			HLIB_XML_FreeAttrContents(pszCmd);
		}
	}
	else
	{
		HxLOG_Debug("\t\t+++++ [%s][%d]\n", __FUNCTION__, __LINE__);
	}

	if(hXmlDocHandle)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}

	if(pList)
	{
		HLIB_LIST_RemoveAll(pList);
	}

	return hErr;

}

HERROR Get_RC_Time(HUINT8 *pucUploadBuf, HUINT32 *ulUploadLen, HUINT32 ulRcvTime)
{

	HERROR					hError = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	RC_ATTR_ELEMENT_t		st_rcAttr;
	HUINT8					szAttrBuf[2][STRING_LEN_MAX];
	HUINT32 				uiTotalBufSize = 0, ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;

	HxDATETIME_t 			dateTime;
	HUINT32					ulSndTime;

	/* element load */
	hError = local_rc_loadElement(RC_ELEMENT_LIST_TIME, &pstElement);
	if( hError != ERR_OK )
	{
		HxLOG_Print("[%s][%d]  load element ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}

	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));

	st_rcAttr.pucAttr1 = s_pszresources[eResourceType_Time];

	pucTempUploadBuf = HLIB_STD_MemAlloc(RC_RESOURCE_SHORT_MAX_BUF_SIZE);
	if(pucTempUploadBuf == NULL)
	{
		HxLOG_Print("[%s][%d]  HLIB_STD_MemAlloc() ERROR \n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}
	uiTotalBufSize	= 0;

	hError = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;

	// <response>
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_TIME_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	(void)HLIB_DATETIME_ConvertUnixTimeToDateTime((UNIXTIME)ulRcvTime, &dateTime);
	HxSTD_snprintf(szAttrBuf[0],STRING_LEN_MAX, "%04d-%02d-%02dT%02d:%02d:%02d,%03d", dateTime.stDate.usYear, dateTime.stDate.ucMonth, dateTime.stDate.ucDay,
					dateTime.stTime.ucHour, dateTime.stTime.ucMinute, dateTime.stTime.ucSecond, dateTime.stTime.usMillisecond);
	st_rcAttr.pucAttr1 = szAttrBuf[0];

	ulSndTime = HLIB_STD_GetSystemTime();
	(void)HLIB_DATETIME_ConvertUnixTimeToDateTime((UNIXTIME)ulSndTime, &dateTime);
	HxSTD_snprintf(szAttrBuf[1],STRING_LEN_MAX, "%04d-%02d-%02dT%02d:%02d:%02d,%03d", dateTime.stDate.usYear, dateTime.stDate.ucMonth, dateTime.stDate.ucDay,
					dateTime.stTime.ucHour, dateTime.stTime.ucMinute, dateTime.stTime.ucSecond, dateTime.stTime.usMillisecond);
	st_rcAttr.pucAttr2 = szAttrBuf[1];

	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_TIME_INFO, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_TIME_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	if(uiTotalBufSize > RC_RESOURCE_SHORT_MAX_BUF_SIZE)
	{
		HxLOG_Print("\t[%s][%d]  @@@ CA informations are too large(%d)..!!! \n",__FUNCTION__,__LINE__, uiTotalBufSize);
		uiTotalBufSize = RC_RESOURCE_SHORT_MAX_BUF_SIZE;
	}
	HxSTD_MemCopy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);
	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HLIB_STD_MemFree(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	HxLOG_Print("[%s][%d] (%s)-\n", __FUNCTION__,__LINE__,pucUploadBuf);
	return hError;
}


HERROR Get_RC_Power(HUINT8 *pucUploadBuf,HUINT32 *ulUploadLen, HBOOL bOperating, HBOOL bEnableActiveStandby, HUINT32 ulStandbyTime)
{
	HERROR					hError = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	RC_ATTR_ELEMENT_t		st_rcAttr;
	HUINT8					szAttrBuf[3][STRING_LEN_MAX];
	HUINT32 				uiTotalBufSize = 0, ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;

	HUINT32					ulCurTime;

	/* element load */
	hError = local_rc_loadElement(RC_ELEMENT_LIST_POWER, &pstElement);
	if( hError != ERR_OK )
	{
		HxLOG_Print("[%s][%d]  load element ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}

	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));

	st_rcAttr.pucAttr1 = s_pszresources[eResourceType_Power];

	pucTempUploadBuf = HLIB_STD_MemAlloc(RC_RESOURCE_SHORT_MAX_BUF_SIZE);
	if(pucTempUploadBuf == NULL)
	{
		HxLOG_Print("[%s][%d]  HLIB_STD_MemAlloc() ERROR \n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	uiTotalBufSize	= 0;
	ulCurTime = HLIB_STD_GetSystemTime();

	hError = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;



	// <response>
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_POWER_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	HxSTD_snprintf(szAttrBuf[0],STRING_LEN_MAX, "%s", bOperating == TRUE ? "on" : "standby");
	st_rcAttr.pucAttr1 = szAttrBuf[0];

	//HxSTD_snprintf(szAttrBuf[1],STRING_LEN_MAX, "%s", bEnableActiveStandby == TRUE ? "true" : "false");
	HxSTD_snprintf(szAttrBuf[1],STRING_LEN_MAX, "false");
	st_rcAttr.pucAttr2 = szAttrBuf[1];

	HxSTD_snprintf(szAttrBuf[2],STRING_LEN_MAX, "%d", ulStandbyTime == 0? 0:(ulCurTime - ulStandbyTime));
	st_rcAttr.pucAttr3 = szAttrBuf[2];

	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_POWER_INFO, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_POWER_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	if(uiTotalBufSize > RC_RESOURCE_SHORT_MAX_BUF_SIZE)
	{
		HxLOG_Print("\t[%s][%d]  @@@ Power informations are too large(%d)..!!! \n",__FUNCTION__,__LINE__, uiTotalBufSize);
		uiTotalBufSize = RC_RESOURCE_SHORT_MAX_BUF_SIZE;
	}
	HxSTD_MemCopy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);
	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HLIB_STD_MemFree(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	HxLOG_Print("[%s][%d] (%s)-\n", __FUNCTION__,__LINE__,pucUploadBuf);
	return hError;
}


#if 1	// POST Power
#define	FSATRC_XMLELEMENT_POWER		"power"
#define	FSATRC_XMLATTR_POWER_STATE	"state"
//#define	FSATRC_XMLATTR_POWER_NO_PASSIVE_STANDBY	"no-passive-standby"

HERROR Post_RC_Power(HUINT8* pucBuf, HUINT32 ulContentLen, HBOOL bOperating, HBOOL bEnableActiveStandby)
{
	HERROR					hError = ERR_FAIL;
	HxXMLDOC				hXmlDocHandle = NULL;
	HxXMLELEMENT			hResponseElement = NULL;
	HUINT8					*pszCmd = NULL;
	HUINT8					*pszstate = NULL;

	HxLOG_Print("[%s][%d]: -\n", __FUNCTION__, __LINE__);

	hError = HLIB_XML_InitParser();
	hError = HLIB_XML_ParseBuffer(pucBuf, ulContentLen, &hXmlDocHandle);
	hError = HLIB_XML_GetRootElement(hXmlDocHandle, &hResponseElement);
	hError = HLIB_XML_GetElementName(hResponseElement, &pszCmd);
	if((hError == ERR_OK) && (pszCmd != NULL) && (HLIB_STD_StrCaseCmp(pszCmd, FSATRC_XMLELEMENT_POWER) == 0))
	{
		hError = HLIB_XML_GetAttrContents(hResponseElement, FSATRC_XMLATTR_POWER_STATE, &pszstate);
		if((hError == ERR_OK) && (pszstate != NULL))
		{
			if(HLIB_STD_StrCaseCmp(pszstate,"standby") == 0)
			{
				if((bOperating == TRUE))//*&& (bEnableActiveStandby == TRUE))
				{
					hError = ERR_OK;
					HAPI_SendRemoteRCUKey(DIKS_POWER);
				}
				else
				{
					hError = 	ERR_FAIL;
				}

			}
			else if(HLIB_STD_StrCaseCmp(pszstate,"on") == 0)
			{
				if(bOperating == FALSE)
				{
					hError = ERR_OK;
					HAPI_SendRemoteRCUKey(DIKS_POWER);
				}
				else
				{
					hError = 	ERR_FAIL;
				}
			}
			else
			{
				hError = 	ERR_FAIL;
			}

			HLIB_XML_FreeAttrContents(pszstate);
		}
		else
		{
			hError = 	ERR_FAIL;
		}
	}
	else
	{
		hError = 	ERR_FAIL;
	}

	HxLOG_Print("[%s][%d]: -\n", __FUNCTION__, __LINE__);

	if(hXmlDocHandle)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}

	return hError;
}
#endif



#if 0		// [POST] Outputs > Settings
#define		FSATRC_XMLELEMENT_OUTPUTS_SETTINGS			"settings"
#define		FSATRC_XMLATTR_OUTPUTS_VOLUME				"volume"
#define		FSATRC_XMLATTR_OUTPUTS_MUTE					"mute"

HERROR Send_RC_Outputs_Settings(HUINT8* pBody,HUINT32 ulBodyLen)
{
	HERROR	hError = ERR_FAIL;
	HXMLDOC hNewXMLDoc = NULL;
	HXMLELEMENT	hXMLElement = NULL;
	HUINT8	*pszVolume = NULL,*pszMute = NULL;
	HFLOAT32	ulVolume = -1;
	HBOOL	bMute,bPrevMute;
	HUINT32 	min_volume, max_volume,volume;

	hError = PAL_XML_InitParser();
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[%s][%d]:	XML parser init was failed \n", __FUNCTION__, __LINE__);
		goto End_Function;
	}

	hError = PAL_XML_ParseBuffer(pBody, ulBodyLen, &hNewXMLDoc);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[%s][%d]:	XML paser was not able to parse the file [%s] \n", __FUNCTION__, __LINE__, pBody);
		goto End_Function;
	}

	hError = PAL_XML_GetRootElement(hNewXMLDoc, &hXMLElement);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[%s][%d]:	XML parser was failed to get ROOT element \n", __FUNCTION__, __LINE__);
		goto End_Function;
	}

	hError = PAL_XML_GetAttrContents(hXMLElement, FSATRC_XMLATTR_OUTPUTS_VOLUME, &pszVolume); //volume
	if(hError == ERR_OK)
	{
		ulVolume = atof(pszVolume);


		ulVolume = ulVolume*100;
		volume = (HUINT32)ulVolume;
		min_volume = 0;
		max_volume = 100;

		if((volume % 2) !=	0)
			volume -= -1;
		if (volume > max_volume)		volume = max_volume;
		else if (volume < min_volume)	volume = min_volume;


		if(OxSVC_OUT_SetVolume(volume) == ERR_OK)
		{
			BUS_PostMessage(NULL, MSG_BUS_MGR_EVENT, (Handle_t)EVT_SYSCTL_AUDIO_VOLUME_CHANGED, 0, 0, 0);
			hError = ERR_OK;
		}
		else
		{
			hError = ERR_FAIL;
		}

		PAL_XML_FreeAttrContents(pszVolume);
	}
	hError = PAL_XML_GetAttrContents(hXMLElement, FSATRC_XMLATTR_OUTPUTS_MUTE, &pszMute); //mute
	if(hError == ERR_OK)
	{

		if(HLIB_STD_StrCaseCmp(pszMute,"true") == 0)
		{
			bMute = TRUE;
		}
		else
		{
			bMute = FALSE;
		}
		bPrevMute = OxSVC_OUT_GetUserAudioMute ();
		if (bMute != bPrevMute)
		{
			hError = OxSVC_OUT_SetUserAudioMute(bMute);
		}
		PAL_XML_FreeAttrContents(pszMute);
	}

End_Function:
	HxLog_Info("[%s][%d]: -\n", __FUNCTION__, __LINE__);
	if(hNewXMLDoc != NULL)
	{
		PAL_XML_FreeDocument(hNewXMLDoc);
	}

	return hError;

}
#endif

#if 0		// [POST] Outputs > Programme
HERROR Send_RC_Outputs_Programme(HUINT32	reqid,HUINT8* command,HUINT8 *pUserData1)
{
	HERROR		hError = ERR_FAIL;
	HCHAR		szHeader[1024] = {0,};
	HCHAR		szBody[1024] = {0,};

	HxSTD_snprintf(szHeader,1024,"%d %s %s",reqid,"POST",pUserData1);
	HxSTD_snprintf(szBody,1024,"%s",command);
	hError =local_RC_NoticeCommandToWebApp(szHeader,szBody);

	return hError;


}
#endif

#if 0		// [DELETE] Storage
/********************************************************************
*	function name : Delete_RC_Storage
*	input : pUserData1 : R_CHANNELNUMBER_STARTTIME (ex: R_102_13232532)
*	output : pucUploadBuf
*	description :
********************************************************************/
HERROR Delete_RC_Storage(HUINT8 *pucUploadBuf,HUINT8 *pUserData1)
{
	HUINT32					ulContentsId=0;
	HUINT32			        *puiContentId = NULL;
	HUINT32                 uiContentNum = 0, idx = 0;
	MW_PvrRecord_t	        stRecordedInfo;
	MW_PvrService_t	        stSvcInfo;
	AP_RecordingInfoType_t	stRecordingInfo;
	HUINT32					ulRecordingActId;
	HERROR					hErr = ERR_FAIL;
	HBOOL 					bPlay = FALSE;

	UNIXTIME				ulStartTime;
	HUINT32					ulChannelNumber;

	if ( pUserData1 == NULL )
	{
		HxLog_Error("[%s:%d] Null Data. \n",__FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	sscanf(pUserData1,"/rc/storage/R_%d_%d",&ulChannelNumber,&ulStartTime);
	HxLog_Print("[%s:%d] parsed: channel num=(%d), starttime=(%d) \n",__FUNCTION__, __LINE__, ulChannelNumber ,ulStartTime);

	hErr = MW_PVR_GetContentsIdList(&uiContentNum, &puiContentId);
	if ( hErr != ERR_OK || uiContentNum == 0 )
	{
		// free & exit
		if (NULL != puiContentId)
		{
			MW_PVR_FreeContentsIdList (puiContentId);
		}

		HxLOG_Error("[%s:%d] there are no record contents. \n",__FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	// 1. search for the matching record content
	for ( idx=0; idx < uiContentNum; idx++ )
	{
		VK_memset(&stRecordedInfo, 0x00, sizeof(MW_PvrRecord_t));
		VK_memset(&stSvcInfo, 0x00, sizeof(MW_PvrService_t));
		hErr = MW_PVR_GetRecordInfo(puiContentId[idx], &stRecordedInfo);
		hErr |= MW_PVR_GetServiceInfo(puiContentId[idx], &stSvcInfo);
		if ( hErr != ERR_OK )
		{
			continue;
		}

		if ( stRecordedInfo.ulStartTime == ulStartTime
			&& stSvcInfo.nChNumber == ulChannelNumber )
		{
			ulContentsId = puiContentId[idx];
			break;
		}
	}
	if ( ulContentsId == 0 )
	{
		HxLOG_Error("[%s:%d] Couldn't finds cid for index=[%s] \n",__FUNCTION__, __LINE__,pUserData1);
		// free & exit
		if (NULL != puiContentId)
		{
			MW_PVR_FreeContentsIdList (puiContentId);
		}
		return ERR_FAIL;
	}


	// 2. check if it's currently under recording
	for ( ulRecordingActId= eActionId_REC_FIRST; ulRecordingActId <= eActionId_REC_LAST; ulRecordingActId++ )
	{
		hErr = AP_Record_GetRecordingSvcInfo (ulRecordingActId, &stRecordingInfo);
		if ( hErr != ERR_OK )
		{
			continue;
		}

		if (ulContentsId == stRecordingInfo.ulContentId )
		{
			HxLog_Error("[%s:%d] Currently under recording the recording.  urn=[%s] \n",__FUNCTION__, __LINE__,pUserData1);
			return ERR_FAIL;
		}

	}

	// 3. check if it's currently under playing
	bPlay = AP_Playback_IsPlaybackId(ulContentsId, NULL);
	if ( bPlay == TRUE )
	{
		HxLog_Error("[%s:%d] Currently under playing the recording.  urn=[%s] \n",__FUNCTION__, __LINE__,pUserData1);
		return ERR_FAIL;
	}

	// 3. delete
	hErr = MW_PVR_DeleteContents(ulContentsId);
	if ( hErr != ERR_OK )
	{
		HxLog_Error("[%s:%d] delete contents failed.	content id=(%d) \n",__FUNCTION__, __LINE__,ulContentsId);
		return ERR_FAIL;
	}

	// 4. notify
	BUS_PostMessage(NULL, MSG_BUS_MGR_EVENT, (Handle_t)EVT_META_RECLIST_DELETED, ulContentsId, 0, 0);


	// free & exit
	if (NULL != puiContentId)
	{
		MW_PVR_FreeContentsIdList (puiContentId);
	}

	HxLog_Print("\033[34m[%s:%d] rec delete done \033[0m\n",__FUNCTION__,__LINE__);

	return ERR_OK;
}
#endif

#if 0		// [GET] Scheduled
HERROR Get_RC_Scheduled(HUINT8 *pucUploadBuf,HUINT32 *ulUploadLen)
{
	HERROR					hErr = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	RSV_Item_t				*pRsvItem = NULL;
	RSV_Item_t				*ppRsvItem[MAX_RSV_ALL];
	RSV_Item_t				dbItem;
	SVC_Info_t				stSvcInfo;
	HUINT32 				uinRsv = 0;

	SCHEDULEDLIST_RSVTYPE_e		eRsvType = RC_RSVTYPE_RESERVED;
	SCHEDULEDLIST_REPEAT_e 		eRepeat = RC_REPEAT_ONCE;
	HUINT16 				usEvtId = 0;
	RC_ATTR_ELEMENT_t		st_rcAttr;
	HUINT8					szAttrBuf[11][STRING_LEN_MAX];
	HUINT8					tempBuf[MAX_RSV_EVT_NAME*2];
	HUINT8					*pszEventName;
	DATE_TIME_T 			stDate;
	HCHAR					buf[EPG_TVA_CRID_AUTH_LEN + EPG_TVA_CRID_DATA_LEN + 1];
	OxEPG_Event_t			*event;
	HUINT32					ulRsvCount;
	HUINT32 				ulRsvSeriesRecordCount = 0;
	HUINT32					uiTotalBufSize = 0,ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;


	/* element load */
	hErr =local_rc_loadElement(RC_ELEMENT_LIST_SCHEDULED, &pstElement);
	if( hErr != ERR_OK )
	{
		HxLOG_Print("[%s][%d] local_rc_loadElement ERROR \n",__FUNCTION__,__LINE__);
		goto EXIT_FUN;
	}


	st_rcAttr.pucAttr1 = s_pszresources[eResourceType_Scheduled];// "/rc/scheduled"

	pucTempUploadBuf = HxMEM_MEMDUP(pucUploadBuf,PACKET_MAX_BUF_SIZE);
	uiTotalBufSize  = 0;

	hErr = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;
	// <response>
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_OPEN, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	/* 모든 RSV Item의 개수를 얻어 온다. */
	ulRsvCount = MW_RSV_CountItem();
	if( ulRsvCount == 0 )
	{
		HxLOG_Print("[%s][%d] MW_RSV_CountItem is Zero \n",__FUNCTION__,__LINE__);
	}

	if( ulRsvCount > MAX_RSV_ALL )
	{
		HxLOG_Print("[%s][%d] ulRsvCount(%d) exceed max rsv(%d)\n", __FUNCTION__,__LINE__,ulRsvCount, MAX_RSV_ALL);
		goto EXIT_FUN;
	}

	if( ulRsvCount > 0 )
	{
		/* MEM ALLOC */
		pRsvItem = (RSV_Item_t *)HxSTD_MemAlloc(sizeof(RSV_Item_t) * ulRsvCount);
		if(!pRsvItem)
		{
			HxLOG_Print("[%s][%d] AP_MEM_Alloc ERROR\n" ,__FUNCTION__,__LINE__);
			goto EXIT_FUN;
		}

		/* MEM Init. */
		VK_memset(pRsvItem, 0x00, sizeof(RSV_Item_t) * ulRsvCount);

		for ( uinRsv = 0; uinRsv < ulRsvCount; uinRsv++ )
		{
			ppRsvItem[uinRsv] = &pRsvItem[uinRsv];
		}

		hErr = MW_RSV_ReadReservedItemAll( ulRsvCount, ppRsvItem );
		if( hErr  != ERR_OK )
		{
			HxLOG_Print("[%s][%d] MW_RSV_ReadReservedItemAll ERROR\n" ,__FUNCTION__,__LINE__);
			goto EXIT_FUN;
		}
#if defined(CONFIG_MW_SI_TVANYTIME_SERIES_REC)
		for (uinRsv = 0; uinRsv < ulRsvCount ; uinRsv++)
		{
			ulRsvSeriesRecordCount += MWC_TVA_GetCountAvailableEvtToRec(ppRsvItem[uinRsv]);
		}

		// ulRsvCount (예약슬롯개수)+ulRsvSeriesRecordCount (예약슬롯에 딸린 Series레코딩 개수)는 MAX_RSV_ALL보다 작아야 한다.
		if ( ulRsvCount+ulRsvSeriesRecordCount > MAX_RSV_ALL )
		{
			HxLOG_Print("[%s:%d] too many rsv items \n",__FUNCTION__,__LINE__);
			goto EXIT_FUN;
		}

		if( pRsvItem != NULL)
		{
			AP_MEM_Free( pRsvItem );
		}

		pRsvItem = (RSV_Item_t *)AP_MEM_Alloc(sizeof(RSV_Item_t) * (ulRsvCount+ulRsvSeriesRecordCount));
		if(!pRsvItem)
		{
			HxLOG_Print("[%s:%d] AP_MEM_Alloc ERROR \n",__FUNCTION__,__LINE__);
			goto EXIT_FUN;
		}

		/* MEM Init. */
		VK_memset(pRsvItem, 0x00, sizeof(RSV_Item_t) * (ulRsvCount+ulRsvSeriesRecordCount));

		for ( uinRsv = 0; uinRsv < ulRsvCount+ulRsvSeriesRecordCount; uinRsv++ )
		{
			ppRsvItem[uinRsv] = &pRsvItem[uinRsv];
		}

		hErr = MW_RSV_ReadReservedItemsAndSeriesItemsAll( ulRsvCount, ppRsvItem );
		if( hErr  != ERR_OK )
		{
			HxLOG_Print("[%s:%d] MW_RSV_ReadReservedItemsAndSeriesItemsAll failed. \n",__FUNCTION__,__LINE__);
			goto EXIT_FUN;
		}
		ulRsvCount += ulRsvSeriesRecordCount;
#endif
	}

	for ( uinRsv = 0; uinRsv < ulRsvCount; uinRsv++ )
	{
		eRsvType = 0;
		eRepeat = 0;

		VK_memcpy(&dbItem,	ppRsvItem[uinRsv], sizeof(RSV_Item_t));
		VK_memset32(&stSvcInfo, 0x00, sizeof(SVC_Info_t));
		hErr = DB_SVC_GetServiceInfo(dbItem.hSvc, &stSvcInfo);
		if(hErr != ERR_OK)
		{
			HxLOG_Print("[local_status_client_RsvListProcess] DB_SVC_GetServiceInfo fail - RSV_TYPE_NOTHING\n");
			continue;
		}

		if( !IS_WATCHING(dbItem.eRsvType) && !IS_REC(dbItem.eRsvType))
		{
			continue;
		}

		usEvtId = GET_DATA(dbItem.eRsvType, dbItem.ext.stWch.usEvtId,	dbItem.ext.stRec.usEvtId);
		HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
		HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "P_%d_%d",stSvcInfo.usFSatSvcIdentifier, usEvtId);  // P_fsatid_starttime
		st_rcAttr.pucAttr1 = szAttrBuf[0];

		VK_memset(tempBuf, 0x00, MAX_RSV_EVT_NAME * 2);

		if(IS_WATCHING(dbItem.eRsvType))
		{
			HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "reminder");
			HxSTD_snprintf(tempBuf, MAX_RSV_EVT_NAME * 2, "%s", dbItem.ext.stWch.szEvtName);
		}
		else if(IS_REC(dbItem.eRsvType))
		{
			if(dbItem.ext.stRec.ucRecKind == CONTENT_KIND_SERIES)
				HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "series");
			else
				HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "record");

			HxSTD_snprintf(tempBuf, MAX_RSV_EVT_NAME * 2, "%s", dbItem.ext.stRec.szEvtName);
		}
		else
		{
			HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "");
			HxSTD_snprintf(tempBuf, MAX_RSV_EVT_NAME * 2, "%s", "");
		}
		st_rcAttr.pucAttr2 = szAttrBuf[1]; //type



		HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%d", stSvcInfo.usFSatSvcIdentifier);  //sid (fsatid)
		st_rcAttr.pucAttr3 = szAttrBuf[2];


		usEvtId = GET_DATA(dbItem.eRsvType, dbItem.ext.stWch.usEvtId,   dbItem.ext.stRec.usEvtId);

		HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%X", usEvtId);  //cid (usEvtId)
		st_rcAttr.pucAttr4 = szAttrBuf[3];

	//	HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%X", usEvtId);  //global-content-id
		st_rcAttr.pucAttr5 = "";
		event = OxEPG_FindEvent(
					OxEPGDVB_EVENT_UID(DB_SVC_GetServiceUID(dbItem.hSvc), usEvtId)
					, eOxEPG_TYPE_PF
					, eOxEPG_TYPE_SC
					, eOxEPG_TYPE_NONE
				);
		if (event == NULL)
		{
			HxLOG_Error("[%s:%d] find event returned null. \n",__FUNCTION__, __LINE__);
			goto EXIT_FUN;
		}

		pszEventName = (HUINT8 *)OxEPG_Event_GetName(event, MWC_PARAM_GetMainLanguage());
		if ( pszEventName == NULL )
		{
			HxLOG_Error("[%s:%d] event get name returned null. \n",__FUNCTION__, __LINE__);
		}


		HxSTD_MemSet(tempBuf,0x00,MAX_RSV_EVT_NAME * 2);
		local_rc_SetReplaceString(pszEventName, tempBuf, NULL, MAX_RSV_EVT_NAME * 2);

		HxSTD_snprintf(szAttrBuf[5], STRING_LEN_MAX, "%s", tempBuf);  //Event Name
		st_rcAttr.pucAttr6 = szAttrBuf[5];

		hErr = UTIL_TCONV_ConvertUnixTimeToDateTime(dbItem.utStTime, &stDate);

		HxSTD_snprintf(szAttrBuf[6], STRING_LEN_MAX, "%02d:%02d:%02d %02d-%02d-%04d",stDate.stTime.ucHour,stDate.stTime.ucMinute,stDate.stTime.ucSecond,\
			stDate.stDate.ucDay,stDate.stDate.ucMonth,stDate.stDate.usYear);
		st_rcAttr.pucAttr7 = szAttrBuf[6];

		hErr = UTIL_TCONV_ConvertUnixTimeToDateTime(dbItem.utStTime+dbItem.utDuration, &stDate);
		HxSTD_snprintf(szAttrBuf[7], STRING_LEN_MAX, "%02d:%02d:%02d %02d-%02d-%04d",stDate.stTime.ucHour,stDate.stTime.ucMinute,stDate.stTime.ucSecond,\
			stDate.stDate.ucDay,stDate.stDate.ucMonth,stDate.stDate.usYear);
		st_rcAttr.pucAttr8 = szAttrBuf[7];

		HxSTD_snprintf(szAttrBuf[8], STRING_LEN_MAX, "%s","false"); //active
		st_rcAttr.pucAttr9 = szAttrBuf[8];


		if(IS_REC(dbItem.eRsvType) == TRUE && dbItem.ext.stRec.ucRecKind == CONTENT_KIND_SERIES )
		{

				HxSTD_PrintToStrN(buf\
					, EPG_TVA_CRID_AUTH_LEN + EPG_TVA_CRID_DATA_LEN + 1\
					, "%s%s"\
					, (HCHAR *)dbItem.ext.stRec.pstSeriesSet->stCrid.auth\
					, (HCHAR *)dbItem.ext.stRec.pstSeriesSet->stCrid.data\
				);


				HxSTD_snprintf(szAttrBuf[9], STRING_LEN_MAX, "%s",buf); //active
				st_rcAttr.pucAttr10 = szAttrBuf[9];

				hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_EVENT_BOOKING_SERIES, &st_rcAttr, NULL);
				uiTotalBufSize = ulNewTotalBufSize;
		}
		else
		{
			hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_EVENT_BOOKING, &st_rcAttr, NULL);
			uiTotalBufSize = ulNewTotalBufSize;
		}
	}




	EXIT_FUN :

	if( pRsvItem )
	{
		HxSTD_MemFree( pRsvItem );
	}

	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	VK_memcpy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);

	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HxMEM_FREE(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	return hErr;

}
#endif

#if 0		// [GET] Current Scheduled
HERROR Get_RC_Current_Scheduled(HUINT8 *pucUploadBuf,HUINT8 *pUserData1,HUINT32 *ulUploadLen)
{
	HERROR					hErr = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	RSV_Item_t		rsvItem;
	SVC_Info_t				stSvcInfo;
	HUINT32 				ulRsvCount = 0;

	SCHEDULEDLIST_RSVTYPE_e 	eRsvType = RC_RSVTYPE_RESERVED;
	SCHEDULEDLIST_REPEAT_e		eRepeat = RC_REPEAT_ONCE;
	HUINT32					ulEvtId = 0;
	HUINT16 				usEvtId = 0;
	RC_ATTR_ELEMENT_t		st_rcAttr;
	HUINT8					szAttrBuf[11][STRING_LEN_MAX];
	HUINT8					tempBuf[MAX_RSV_EVT_NAME*2];
	HUINT8					*pszEventName;
	DATE_TIME_T 			stDate;
	HCHAR					buf[EPG_TVA_CRID_AUTH_LEN + EPG_TVA_CRID_DATA_LEN + 1];
	HUINT16					pos = 0,i;
	HUINT32					ulSlot = 0;
	HCHAR					ucBookingId[MAX_RSV_EVT_NAME];
	OxEPG_Event_t			*event;
	HUINT32					ulSvcId;
	HUINT16					usFsatSvcId;
	HUINT32					uiTotalBufSize = 0,ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;

	/* element load */
	hErr =local_rc_loadElement(RC_ELEMENT_LIST_SCHEDULED, &pstElement);
	if( hErr != ERR_OK )
	{
		HxLOG_Print("[%s][%d] local_rc_loadElement ERROR \n",__FUNCTION__,__LINE__);
		goto FUN_END;
	}

	for(i = 0;i <HxSTD_StrLen(pUserData1);i++)
	{
		if(HxSTD_StrNCmp((char *)&pUserData1[i],"/",1) == 0)
		{
			pos = i;
		}
	}
	if(pos+1 >= HxSTD_StrLen(pUserData1) )
	{
		goto FUN_END;
	}

	HxSTD_snprintf(ucBookingId, MAX_RSV_EVT_NAME, "%s",(HCHAR *)&pUserData1[pos+1]); //bid

	sscanf(ucBookingId,"P_%d_%d",&ulSvcId,&ulEvtId);
	usFsatSvcId = ulSvcId;
	usEvtId		= ulEvtId;
	HxLog_Debug("[%s:%d] parsed: usFsatSvcId=(%d), usEvtId=(%d) \n",__FUNCTION__, __LINE__, usFsatSvcId ,usEvtId);

	hErr = local_rcRsv_FindMatchingSlot(&ulSlot, usFsatSvcId, usEvtId);
	if ( hErr != ERR_OK)
	{
		HxLOG_Error("[%s:%d] local_rpRsv_FindMatchingSlot failed. \n", __FUNCTION__ , __HxLINE__);
		return ERR_FAIL;
	}


	HxSTD_MemSet(&rsvItem, 0x00, sizeof(RSV_Item_t));
	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
	HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s",pUserData1); // /rc/scheduled/P_fsatid_starttime
	st_rcAttr.pucAttr1 = szAttrBuf[0];

	pucTempUploadBuf = HxMEM_MEMDUP(pucUploadBuf,PACKET_MAX_BUF_SIZE);
	uiTotalBufSize  = 0;

	hErr = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;

	// <response>
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_OPEN, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	/* 모든 RSV Item의 개수를 얻어 온다. */
	ulRsvCount = MW_RSV_CountItem();
	if( ulRsvCount == 0 )
	{
		HxLOG_Print("[%s][%d] MW_RSV_CountItem is Zero \n",__FUNCTION__,__LINE__);
	}


	if( ulRsvCount > 0 )
	{

		hErr = MW_RSV_ReadReservedItem(ulSlot, &rsvItem);
		if( hErr  != ERR_OK )
		{
			HxLOG_Print("[%s][%d] MW_RSV_ReadReservedItemAll ERROR\n" ,__FUNCTION__,__LINE__);
			goto EXIT_FUN;
		}
	}

	eRsvType = 0;
	eRepeat = 0;

	HxSTD_MemSet(&stSvcInfo, 0x00, sizeof(SVC_Info_t));
	hErr = DB_SVC_GetServiceInfo(rsvItem.hSvc, &stSvcInfo);
	if(hErr != ERR_OK)
	{
		HxLOG_Print("[local_status_client_RsvListProcess] DB_SVC_GetServiceInfo fail - RSV_TYPE_NOTHING\n");
		goto EXIT_FUN;
	}

	if( !IS_WATCHING(rsvItem.eRsvType) && !IS_REC(rsvItem.eRsvType))
	{
		goto EXIT_FUN;
	}


	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
	HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s", ucBookingId);	//P_fsatid_evtid
	st_rcAttr.pucAttr1 = szAttrBuf[0];

	HxSTD_MemSet(tempBuf, 0x00, MAX_RSV_EVT_NAME * 2);

	if(IS_WATCHING(rsvItem.eRsvType))
	{
		HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "reminder");
		HxSTD_snprintf(tempBuf, MAX_RSV_EVT_NAME * 2, "%s", rsvItem.ext.stWch.szEvtName);
	}
	else if(IS_REC(rsvItem.eRsvType))
	{
		if(rsvItem.ext.stRec.ucRecKind == CONTENT_KIND_SERIES)
			HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "series");
		else
			HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "record");

		HxSTD_snprintf(tempBuf, MAX_RSV_EVT_NAME * 2, "%s", rsvItem.ext.stRec.szEvtName);
	}
	else
	{
		HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "");
		HxSTD_snprintf(tempBuf, MAX_RSV_EVT_NAME * 2, "%s", "");
	}
	st_rcAttr.pucAttr2 = szAttrBuf[1]; //type



	HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%d", stSvcInfo.usFSatSvcIdentifier);	//sid (fsatid)
	st_rcAttr.pucAttr3 = szAttrBuf[2];


	usEvtId = GET_DATA(rsvItem.eRsvType, rsvItem.ext.stWch.usEvtId,	rsvItem.ext.stRec.usEvtId);

	HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%X", usEvtId);  //cid (usEvtId)
	st_rcAttr.pucAttr4 = szAttrBuf[3];

//	HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%X", usEvtId);  //global-content-id
	st_rcAttr.pucAttr5 = "";

	event = OxEPG_FindEvent(
				OxEPGDVB_EVENT_UID(DB_SVC_GetServiceUID(rsvItem.hSvc), usEvtId)
				, eOxEPG_TYPE_PF
				, eOxEPG_TYPE_SC
				, eOxEPG_TYPE_NONE
			);
	if (event == NULL)
	{
		HxLOG_Error("[%s:%d] find event returned null. \n",__FUNCTION__, __LINE__);
		goto EXIT_FUN;
	}

	pszEventName = (HUINT8 *)OxEPG_Event_GetName(event, MWC_PARAM_GetMainLanguage());
	if ( pszEventName == NULL )
	{
		HxLOG_Error("[%s:%d] event get name returned null. \n",__FUNCTION__, __LINE__);
	}

	HxSTD_MemSet(tempBuf,0x00,MAX_RSV_EVT_NAME * 2);
	local_rc_SetReplaceString(pszEventName, tempBuf, NULL, MAX_RSV_EVT_NAME * 2);

	HxSTD_snprintf(szAttrBuf[5], STRING_LEN_MAX, "%s", tempBuf);	//Event Name
	st_rcAttr.pucAttr6 = szAttrBuf[5];

	hErr = UTIL_TCONV_ConvertUnixTimeToDateTime(rsvItem.utStTime, &stDate);
	HxSTD_snprintf(szAttrBuf[6], STRING_LEN_MAX, "%02d:%02d:%02d %02d-%02d-%04d",stDate.stTime.ucHour,stDate.stTime.ucMinute,stDate.stTime.ucSecond,\
		stDate.stDate.ucDay,stDate.stDate.ucMonth,stDate.stDate.usYear); //start-time
	st_rcAttr.pucAttr7 = szAttrBuf[6];

	hErr = UTIL_TCONV_ConvertUnixTimeToDateTime(rsvItem.utStTime+rsvItem.utDuration, &stDate);
	HxSTD_snprintf(szAttrBuf[7], STRING_LEN_MAX, "%02d:%02d:%02d %02d-%02d-%04d",stDate.stTime.ucHour,stDate.stTime.ucMinute,stDate.stTime.ucSecond,\
		stDate.stDate.ucDay,stDate.stDate.ucMonth,stDate.stDate.usYear);//end-time
	st_rcAttr.pucAttr8 = szAttrBuf[7];

	HxSTD_snprintf(szAttrBuf[8], STRING_LEN_MAX, "%s","false"); //active
	st_rcAttr.pucAttr9 = szAttrBuf[8];


	if(IS_REC(rsvItem.eRsvType) == TRUE && rsvItem.ext.stRec.ucRecKind == CONTENT_KIND_SERIES )
	{

			HxSTD_PrintToStrN(buf
				, EPG_TVA_CRID_AUTH_LEN + EPG_TVA_CRID_DATA_LEN + 1
				, "%s%s"
				, (HCHAR *)rsvItem.ext.stRec.pstSeriesSet->stCrid.auth
				, (HCHAR *)rsvItem.ext.stRec.pstSeriesSet->stCrid.data
			);


			HxSTD_snprintf(szAttrBuf[9], STRING_LEN_MAX, "%s",buf); //active
			st_rcAttr.pucAttr10 = szAttrBuf[9];

			hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_EVENT_BOOKING_SERIES, &st_rcAttr, NULL);
			uiTotalBufSize = ulNewTotalBufSize;
	}
	else
	{
		hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_EVENT_BOOKING, &st_rcAttr, NULL);
		uiTotalBufSize = ulNewTotalBufSize;
	}


	EXIT_FUN :

	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
FUN_END :
	VK_memcpy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);

	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HxMEM_FREE(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	return hErr;

}
#endif

#if 0		// [GET] Current Conflict Scheduled
HERROR Get_RC_Current_Conflict_Scheduled(HUINT8 *pucUploadBuf,HUINT8 *Param,RSV_Item_t *rsvm_Info,AM_Conflict_Info_t conflict_Info,HUINT32 *ulUploadLen)
{
	HERROR					hErr = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	RSV_Item_t			conflictrsvItem;

	HUINT16 				usEvtId = 0;
	RC_ATTR_ELEMENT_t		st_rcAttr;
	HUINT8					szAttrBuf[11][STRING_LEN_MAX];
	HUINT8					tempBuf[MAX_RSV_EVT_NAME*2];
	DATE_TIME_T 			stDate;
	HUINT8					ucMonth[10] ,ucDay[10],ucHour[10],ucMin[10],ucSec[10];
	HUINT16					i;
	OxEPG_Event_t			*event;
	HCHAR					*pszEventName = NULL;
	HUINT16					usFSatSvcIdentifier;

	HUINT32					uiTotalBufSize = 0,ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;


	/* element load */
	hErr =local_rc_loadElement(RC_ELEMENT_LIST_SCHEDULED_CONFLICT, &pstElement);
	if( hErr != ERR_OK )
	{
		HxLOG_Print("[%s][%d] local_rc_loadElement ERROR \n",__FUNCTION__,__LINE__);
		goto FUN_END;
	}

	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
	HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s",Param); // /rc/scheduled/{bid}
	st_rcAttr.pucAttr1 = szAttrBuf[0];

	pucTempUploadBuf = HxMEM_MEMDUP(pucUploadBuf,PACKET_MAX_BUF_SIZE);
	uiTotalBufSize  = 0;

	hErr = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;

	// <response>
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_CONFLICT_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_CONFLICT_OPEN, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;


	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));

	FSAT_DB_SVC_GetSvcIdentifier(rsvm_Info->hSvc,&usFSatSvcIdentifier);
	usEvtId = GET_DATA(rsvm_Info->eRsvType, rsvm_Info->ext.stWch.usEvtId,	rsvm_Info->ext.stRec.usEvtId);

	HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "P_%d_%d", usFSatSvcIdentifier, usEvtId);	//P_fsatid_evtid
	st_rcAttr.pucAttr1 = szAttrBuf[0];

	HxSTD_MemSet(tempBuf, 0x00, MAX_RSV_EVT_NAME * 2);

	if(IS_WATCHING(rsvm_Info->eRsvType))
	{
		HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "reminder");
		HxSTD_snprintf(tempBuf, MAX_RSV_EVT_NAME * 2, "%s", rsvm_Info->ext.stWch.szEvtName);
	}
	else if(IS_REC(rsvm_Info->eRsvType))
	{
		if(rsvm_Info->ext.stRec.ucRecKind == CONTENT_KIND_SERIES)
			HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "series");
		else
			HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "record");

		HxSTD_snprintf(tempBuf, MAX_RSV_EVT_NAME * 2, "%s", rsvm_Info->ext.stRec.szEvtName);
	}
	else
	{
		HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "");
		HxSTD_snprintf(tempBuf, MAX_RSV_EVT_NAME * 2, "%s", "");
	}
	st_rcAttr.pucAttr2 = szAttrBuf[1]; //type


	FSAT_DB_SVC_GetSvcIdentifier(rsvm_Info->hSvc,&usFSatSvcIdentifier);

	HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%d", usFSatSvcIdentifier);	//sid (fsatid)
	st_rcAttr.pucAttr3 = szAttrBuf[2];


	usEvtId = GET_DATA(rsvm_Info->eRsvType, rsvm_Info->ext.stWch.usEvtId,	rsvm_Info->ext.stRec.usEvtId);

	HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%X", usEvtId);  //cid (usEvtId)
	st_rcAttr.pucAttr4 = szAttrBuf[3];

//	HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%X", usEvtId);  //global-content-id
	st_rcAttr.pucAttr5 = "";

	usEvtId = GET_DATA(rsvm_Info->eRsvType, rsvm_Info->ext.stWch.usEvtId,	rsvm_Info->ext.stRec.usEvtId);

	event = OxEPG_FindEvent(
				OxEPGDVB_EVENT_UID(DB_SVC_GetServiceUID(rsvm_Info->hSvc), usEvtId)
				, eOxEPG_TYPE_PF
				, eOxEPG_TYPE_SC
				, eOxEPG_TYPE_NONE
			);
	if (event == NULL)
	{
		HxLOG_Error("[%s:%d] find event returned null. \n",__FUNCTION__, __LINE__);
		goto	CONFLICT_END ;
	}

	pszEventName = (HUINT8 *)OxEPG_Event_GetName(event, MWC_PARAM_GetMainLanguage());
	if ( pszEventName == NULL )
	{
		HxLOG_Error("[%s:%d] event get name returned null. \n",__FUNCTION__, __LINE__);
	}


	HxSTD_MemSet(tempBuf,0x00,MAX_RSV_EVT_NAME * 2);
	local_rc_SetReplaceString(pszEventName, tempBuf, NULL, MAX_RSV_EVT_NAME * 2);

	HxSTD_snprintf(szAttrBuf[5], STRING_LEN_MAX, "%s", pszEventName);	//Event Name
	st_rcAttr.pucAttr6 = szAttrBuf[5];

	hErr = UTIL_TCONV_ConvertUnixTimeToDateTime(rsvm_Info->utStTime, &stDate);
	HxSTD_snprintf(szAttrBuf[6], STRING_LEN_MAX, "%02d:%02d:%02d %02d-%02d-%04d",stDate.stTime.ucHour,stDate.stTime.ucMinute,stDate.stTime.ucSecond,\
		stDate.stDate.ucDay,stDate.stDate.ucMonth,stDate.stDate.usYear); //start-time
	st_rcAttr.pucAttr7 = szAttrBuf[6];

	hErr = UTIL_TCONV_ConvertUnixTimeToDateTime(rsvm_Info->utStTime+rsvm_Info->utDuration, &stDate);
	HxSTD_snprintf(szAttrBuf[7], STRING_LEN_MAX, "%02d:%02d:%02d %02d-%02d-%04d",stDate.stTime.ucHour,stDate.stTime.ucMinute,stDate.stTime.ucSecond,\
		stDate.stDate.ucDay,stDate.stDate.ucMonth,stDate.stDate.usYear);  //end-time
	st_rcAttr.pucAttr8 = szAttrBuf[7];



#if 0
	if(IS_REC(rsvItem.eRsvType) == TRUE && rsvItem.ext.stRec.ucRecKind == CONTENT_KIND_SERIES )
	{

			HxSTD_PrintToStrN(buf
				, EPG_TVA_CRID_AUTH_LEN + EPG_TVA_CRID_DATA_LEN + 1
				, "%d%s%s"
				, rsvItem.ext.stRec.pstSeriesSet->stCrid.type - 0x30
				, rsvItem.ext.stRec.pstSeriesSet->stCrid.auth
				, rsvItem.ext.stRec.pstSeriesSet->stCrid.data
			);


			HxSTD_snprintf(szAttrBuf[9], STRING_LEN_MAX, "%s",buf); //active
			st_rcAttr.pucAttr10 = szAttrBuf[9];

			hErr = local_rc_common(pstElement, RC_SCHEDULED_EVENT_BOOKING_SERIES, &st_rcAttr, NULL);
	}
	else
#endif
	{
		hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_CONFLICT_EVENT_BOOKING_OPEN, &st_rcAttr, NULL);
		uiTotalBufSize = ulNewTotalBufSize;
	}

	if(conflict_Info.ulNumOfRsv  <= 0)
		goto	EVENT_END;

	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_CONFLICT_WITH_OPEN, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	for(i = 0;i<conflict_Info.ulNumOfRsv;i++)
	{
	// Conflict Info
		HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
		HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%d", conflict_Info.aulSlot[i]);	//bid
		st_rcAttr.pucAttr1 = szAttrBuf[0];

		HxSTD_MemSet(tempBuf, 0x00, MAX_RSV_EVT_NAME * 2);

	//	HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "");  //conflict type is none
		st_rcAttr.pucAttr2 = "";//type
		hErr = MW_RSV_ReadReservedItem(conflict_Info.aulSlot[i], &conflictrsvItem);
		usEvtId = GET_DATA(conflictrsvItem.eRsvType, conflictrsvItem.ext.stWch.usEvtId, conflictrsvItem.ext.stRec.usEvtId);

		event = OxEPG_FindEvent(
					OxEPGDVB_EVENT_UID(DB_SVC_GetServiceUID(conflictrsvItem.hSvc), usEvtId)
					, eOxEPG_TYPE_PF
					, eOxEPG_TYPE_SC
					, eOxEPG_TYPE_NONE
				);
		if (event == NULL)
		{
			HxLOG_Error("[%s:%d] find event returned null. \n",__FUNCTION__, __LINE__);
			goto	EVENT_END ;
		}

		pszEventName = (HUINT8 *)OxEPG_Event_GetName(event, MWC_PARAM_GetMainLanguage());
		if ( pszEventName == NULL )
		{
			HxLOG_Error("[%s:%d] event get name returned null. \n",__FUNCTION__, __LINE__);
		}
		HxLog_Info("[%s:%d] event name = [%s] \n",__FUNCTION__, __LINE__, pszEventName);


		FSAT_DB_SVC_GetSvcIdentifier(conflictrsvItem.hSvc,&usFSatSvcIdentifier);

		HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%d", usFSatSvcIdentifier);	//sid (fsatid)
		st_rcAttr.pucAttr3 = szAttrBuf[2];

		HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%X", usEvtId);  //cid (usEvtId)
		st_rcAttr.pucAttr4 = szAttrBuf[3];

	//	HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%X", usEvtId);  //global-content-id
		st_rcAttr.pucAttr5 = "";


		HxSTD_MemSet(tempBuf,0x00,MAX_RSV_EVT_NAME * 2);
		local_rc_SetReplaceString(pszEventName, tempBuf, NULL, MAX_RSV_EVT_NAME * 2);

		HxSTD_snprintf(szAttrBuf[5], STRING_LEN_MAX, "%s", tempBuf);	//Event Name
		st_rcAttr.pucAttr6 = szAttrBuf[5];

		hErr = UTIL_TCONV_ConvertUnixTimeToDateTime(conflictrsvItem.utStTime, &stDate);

		if(stDate.stDate.ucDay <10)
		{
			HxSTD_snprintf(ucDay,10,"0%d",stDate.stDate.ucDay);
		}
		else
		{
			HxSTD_snprintf(ucDay,10,"%2d",stDate.stDate.ucDay);
		}
		if(stDate.stDate.ucMonth <10)
		{
			HxSTD_snprintf(ucMonth,10,"0%d",stDate.stDate.ucMonth);
		}
		else
		{
			HxSTD_snprintf(ucMonth,10,"%2d",stDate.stDate.ucMonth);
		}

		if(stDate.stTime.ucHour <10)
		{
			HxSTD_snprintf(ucHour,10,"0%d",stDate.stTime.ucHour);
		}
		else
		{
			HxSTD_snprintf(ucHour,10,"%2d",stDate.stTime.ucHour);
		}

		if(stDate.stTime.ucMinute<10)
		{
			HxSTD_snprintf(ucMin,10,"0%d",stDate.stTime.ucMinute);
		}
		else
		{
			HxSTD_snprintf(ucMin,10,"%2d",stDate.stTime.ucMinute);
		}
		if(stDate.stTime.ucSecond<10)
		{
			HxSTD_snprintf(ucSec,10,"0%d",stDate.stTime.ucSecond);
		}
		else
		{
			HxSTD_snprintf(ucSec,10,"%2d",stDate.stTime.ucSecond);
		}

		HxSTD_snprintf(szAttrBuf[6], STRING_LEN_MAX, "%s:%s:%s %s-%s-%d",ucHour,ucMin,ucSec,ucDay,ucMonth,stDate.stDate.usYear); //start-time
		st_rcAttr.pucAttr7 = szAttrBuf[6];

		hErr = UTIL_TCONV_ConvertUnixTimeToDateTime(conflictrsvItem.utStTime+conflictrsvItem.utDuration, &stDate);

		if(stDate.stDate.ucDay <10)
		{
			HxSTD_snprintf(ucDay,10,"0%d",stDate.stDate.ucDay);
		}
		else
		{
			HxSTD_snprintf(ucDay,10,"%2d",stDate.stDate.ucDay);
		}
		if(stDate.stDate.ucMonth <10)
		{
			HxSTD_snprintf(ucMonth,10,"0%d",stDate.stDate.ucMonth);
		}
		else
		{
			HxSTD_snprintf(ucMonth,10,"%2d",stDate.stDate.ucMonth);
		}

		if(stDate.stTime.ucHour <10)
		{
			HxSTD_snprintf(ucHour,10,"0%d",stDate.stTime.ucHour);
		}
		else
		{
			HxSTD_snprintf(ucHour,10,"%2d",stDate.stTime.ucHour);
		}

		if(stDate.stTime.ucMinute<10)
		{
			HxSTD_snprintf(ucMin,10,"0%d",stDate.stTime.ucMinute);
		}
		else
		{
			HxSTD_snprintf(ucMin,10,"%2d",stDate.stTime.ucMinute);
		}
		if(stDate.stTime.ucSecond<10)
		{
			HxSTD_snprintf(ucSec,10,"0%d",stDate.stTime.ucSecond);
		}
		else
		{
			HxSTD_snprintf(ucSec,10,"%2d",stDate.stTime.ucSecond);
		}

		HxSTD_snprintf(szAttrBuf[7], STRING_LEN_MAX, "%s:%s:%s %s-%s-%d",ucHour,ucMin,ucSec,ucDay,ucMonth,stDate.stDate.usYear); //end-time
		st_rcAttr.pucAttr8 = szAttrBuf[7];
		hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_CONFLICT_EVENT_BOOKING,  &st_rcAttr, NULL);
		uiTotalBufSize = ulNewTotalBufSize;
	}
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_CONFLICT_WITH_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

EVENT_END :
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_CONFLICT_EVENT_BOOKING_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;


CONFLICT_END :

	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_CONFLICT_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_CONFLICT_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
FUN_END :

	VK_memcpy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);

	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HxMEM_FREE(pucTempUploadBuf);
	pucTempUploadBuf = NULL;
	return hErr;

}
#endif

#if 0		//[GET] Filtered Scheduled
HERROR Get_RC_Filtered_Scheduled(HUINT8 *pucUploadBuf,HUINT8 *pUserData1,HUINT32 *ulUploadLen)
{
	HERROR					hErr = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	RSV_Item_t				*pRsvItem = NULL;
	RSV_Item_t				*ppRsvItem[MAX_RSV_ALL];
	RSV_Item_t				dbItem;
	SVC_Info_t				stSvcInfo;
	HUINT32 				ulRsvCount = 0;
	HUINT32 				uinRsv = 0;

	SCHEDULEDLIST_RSVTYPE_e 	eRsvType = RC_RSVTYPE_RESERVED;
	SCHEDULEDLIST_REPEAT_e		eRepeat = RC_REPEAT_ONCE;
	HUINT16 				usEvtId = 0;
	RC_ATTR_ELEMENT_t		st_rcAttr;
	HUINT8					szAttrBuf[11][STRING_LEN_MAX];
	HUINT8					tempBuf[MAX_RSV_EVT_NAME*2];
	HUINT8					*pszEventName;
	DATE_TIME_T 			stDate;
	HCHAR					buf[EPG_TVA_CRID_AUTH_LEN + EPG_TVA_CRID_DATA_LEN + 1];
	HUINT16					i,pos;
	HUINT8					ucRsvType[MAX_RSV_EVT_NAME] = {0,};
	HUINT8					ucBookingKind;
	OxEPG_Event_t			*event;
	HUINT32 				ulRsvSeriesRecordCount = 0;
	HUINT32					uiTotalBufSize = 0,ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;


	/* element load */
	hErr =local_rc_loadElement(RC_ELEMENT_LIST_SCHEDULED, &pstElement);
	if( hErr != ERR_OK )
	{
		HxLOG_Print("[%s][%d] local_rc_loadElement ERROR \n",__FUNCTION__,__LINE__);
		goto EXIT_FUN;
	}

	for(i = 0;i <HxSTD_StrLen(pUserData1);i++)
	{
		if(HxSTD_StrNCmp((char *)&pUserData1[i],"=",1) == 0) //?type=series 로 들어옴
		{
			pos = i;
		}
	}
	if(pos+1 >= HxSTD_StrLen(pUserData1) )
	{
		// all kind
	}
	else
	{
		HxSTD_snprintf(ucRsvType, MAX_RSV_EVT_NAME, "%s",(HCHAR *)&pUserData1[pos+1]); //filtered by type
	}

	HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
	HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "%s",pUserData1); // /rc/scheduled/{filtered by type}
	st_rcAttr.pucAttr1 = szAttrBuf[0];

	pucTempUploadBuf = HxMEM_MEMDUP(pucUploadBuf,PACKET_MAX_BUF_SIZE);
	uiTotalBufSize  = 0;

	hErr = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;

	// <response>
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_OPEN, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;


	/* 모든 RSV Item의 개수를 얻어 온다. */
	ulRsvCount = MW_RSV_CountItem();
	if( ulRsvCount == 0 )
	{
		HxLOG_Print("[%s][%d] MW_RSV_CountItem is Zero \n",__FUNCTION__,__LINE__);
	}

	if( ulRsvCount > MAX_RSV_ALL )
	{
		HxLOG_Print("[%s][%d] ulRsvCount(%d) exceed max rsv(%d)\n", __FUNCTION__,__LINE__,ulRsvCount, MAX_RSV_ALL);
		goto EXIT_FUN;
	}

	if( ulRsvCount > 0 )
	{
		/* MEM ALLOC */
		pRsvItem = (RSV_Item_t *)HxSTD_MemAlloc(sizeof(RSV_Item_t) * ulRsvCount);
		if(!pRsvItem)
		{
			HxLOG_Print("[%s][%d] AP_MEM_Alloc ERROR\n" ,__FUNCTION__,__LINE__);
			goto EXIT_FUN;
		}

		/* MEM Init. */
		VK_memset(pRsvItem, 0x00, sizeof(RSV_Item_t) * ulRsvCount);

		for ( uinRsv = 0; uinRsv < ulRsvCount; uinRsv++ )
		{
			ppRsvItem[uinRsv] = &pRsvItem[uinRsv];
		}

		hErr = MW_RSV_ReadReservedItemAll( ulRsvCount, ppRsvItem );
		if( hErr  != ERR_OK )
		{
			HxLOG_Print("[%s][%d] MW_RSV_ReadReservedItemAll ERROR\n" ,__FUNCTION__,__LINE__);
			goto EXIT_FUN;
		}
#if defined(CONFIG_MW_SI_TVANYTIME_SERIES_REC)
		for (uinRsv = 0; uinRsv < ulRsvCount ; uinRsv++)
		{
			ulRsvSeriesRecordCount += MWC_TVA_GetCountAvailableEvtToRec(ppRsvItem[uinRsv]);
		}

		// ulRsvCount (예약슬롯개수)+ulRsvSeriesRecordCount (예약슬롯에 딸린 Series레코딩 개수)는 MAX_RSV_ALL보다 작아야 한다.
		if ( ulRsvCount+ulRsvSeriesRecordCount > MAX_RSV_ALL )
		{
			HxLOG_Print("[%s:%d] too many rsv items \n",__FUNCTION__,__LINE__);
			goto EXIT_FUN;
		}

		if( pRsvItem != NULL)
		{
			AP_MEM_Free( pRsvItem );
		}

		pRsvItem = (RSV_Item_t *)AP_MEM_Alloc(sizeof(RSV_Item_t) * (ulRsvCount+ulRsvSeriesRecordCount));
		if(!pRsvItem)
		{
			HxLOG_Print("[%s:%d] AP_MEM_Alloc ERROR \n",__FUNCTION__,__LINE__);
			goto EXIT_FUN;
		}

		/* MEM Init. */
		VK_memset(pRsvItem, 0x00, sizeof(RSV_Item_t) * (ulRsvCount+ulRsvSeriesRecordCount));

		for ( uinRsv = 0; uinRsv < ulRsvCount+ulRsvSeriesRecordCount; uinRsv++ )
		{
			ppRsvItem[uinRsv] = &pRsvItem[uinRsv];
		}

		hErr = MW_RSV_ReadReservedItemsAndSeriesItemsAll( ulRsvCount, ppRsvItem );
		if( hErr  != ERR_OK )
		{
			HxLOG_Print("[%s:%d] MW_RSV_ReadReservedItemsAndSeriesItemsAll failed. \n",__FUNCTION__,__LINE__);
			goto EXIT_FUN;
		}
		ulRsvCount += ulRsvSeriesRecordCount;
#endif
	}

	if(HLIB_STD_StrCaseCmp((HCHAR *)ucRsvType,  "series") == 0)
	{
		ucBookingKind = eRsvType_Series;
	}
	else if(HLIB_STD_StrCaseCmp((HCHAR *)ucRsvType, "record") == 0)
	{
		ucBookingKind = eRsvType_Record;
	}
	else if(HLIB_STD_StrCaseCmp((HCHAR *)ucRsvType, "reminder") == 0)
	{
		ucBookingKind = eRsvType_Reminder;
	}
	else //all
	{
		ucBookingKind = eRsvType_All;
	}


	for ( uinRsv = 0; uinRsv < ulRsvCount; uinRsv++ )
	{
		eRsvType = 0;
		eRepeat = 0;

		VK_memcpy(&dbItem,	ppRsvItem[uinRsv], sizeof(RSV_Item_t));
		VK_memset32(&stSvcInfo, 0x00, sizeof(SVC_Info_t));
		hErr = DB_SVC_GetServiceInfo(dbItem.hSvc, &stSvcInfo);
		if(hErr != ERR_OK)
		{
			HxLOG_Print("[local_status_client_RsvListProcess] DB_SVC_GetServiceInfo fail - RSV_TYPE_NOTHING\n");
			continue;
		}

		if(ucBookingKind == eRsvType_Reminder)
		{
			if(!IS_WATCHING(dbItem.eRsvType) )
			{
				continue;
			}
		}
		else if(ucBookingKind == eRsvType_Record)
		{
			if(!IS_REC(dbItem.eRsvType) )
			{
				continue;
			}
			else
			{
				if(dbItem.ext.stRec.ucRecKind == CONTENT_KIND_SERIES)
				{
					continue;
				}
			}
		}
		else if(ucBookingKind == eRsvType_Series)
		{
			if(!IS_REC(dbItem.eRsvType) )
			{
				continue;
			}
			else
			{
				if(dbItem.ext.stRec.ucRecKind != CONTENT_KIND_SERIES)
				{
					continue;
				}

			}
		}

		usEvtId = GET_DATA(dbItem.eRsvType, dbItem.ext.stWch.usEvtId,	dbItem.ext.stRec.usEvtId);

		HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
		HxSTD_snprintf(szAttrBuf[0], STRING_LEN_MAX, "P_%d_%d", stSvcInfo.usFSatSvcIdentifier,usEvtId);	//P_fsatid_evtid
		st_rcAttr.pucAttr1 = szAttrBuf[0];

		VK_memset(tempBuf, 0x00, MAX_RSV_EVT_NAME * 2);

		if(IS_WATCHING(dbItem.eRsvType))
		{
			HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "reminder");
			HxSTD_snprintf(tempBuf, MAX_RSV_EVT_NAME * 2, "%s", dbItem.ext.stWch.szEvtName);
		}
		else if(IS_REC(dbItem.eRsvType))
		{
			if(dbItem.ext.stRec.ucRecKind == CONTENT_KIND_SERIES)
				HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "series");
			else
				HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "record");

			HxSTD_snprintf(tempBuf, MAX_RSV_EVT_NAME * 2, "%s", dbItem.ext.stRec.szEvtName);
		}
		else
		{
			HxSTD_snprintf(szAttrBuf[1], STRING_LEN_MAX, "%s", "");
			HxSTD_snprintf(tempBuf, MAX_RSV_EVT_NAME * 2, "%s", "");
		}
		st_rcAttr.pucAttr2 = szAttrBuf[1]; //type



		HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%d", stSvcInfo.usFSatSvcIdentifier);	//sid (fsatid)
		st_rcAttr.pucAttr3 = szAttrBuf[2];


		usEvtId = GET_DATA(dbItem.eRsvType, dbItem.ext.stWch.usEvtId,	dbItem.ext.stRec.usEvtId);

		HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%X", usEvtId);  //cid (usEvtId)
		st_rcAttr.pucAttr4 = szAttrBuf[3];

	//	HxSTD_snprintf(szAttrBuf[3], STRING_LEN_MAX, "%X", usEvtId);  //global-content-id
		st_rcAttr.pucAttr5 = "";
		event = OxEPG_FindEvent(
					OxEPGDVB_EVENT_UID(DB_SVC_GetServiceUID(dbItem.hSvc), usEvtId)
					, eOxEPG_TYPE_PF
					, eOxEPG_TYPE_SC
					, eOxEPG_TYPE_NONE
				);
		if (event == NULL)
		{
			HxLOG_Error("[%s:%d] find event returned null. \n",__FUNCTION__, __LINE__);
			goto	EXIT_FUN ;
		}

		pszEventName = (HUINT8 *)OxEPG_Event_GetName(event, MWC_PARAM_GetMainLanguage());
		if ( pszEventName == NULL )
		{
			HxLOG_Error("[%s:%d] event get name returned null. \n",__FUNCTION__, __LINE__);
		}

		HxSTD_MemSet(tempBuf,0x00,MAX_RSV_EVT_NAME * 2);
		local_rc_SetReplaceString(pszEventName, tempBuf, NULL, MAX_RSV_EVT_NAME * 2);

		HxSTD_snprintf(szAttrBuf[5], STRING_LEN_MAX, "%s", tempBuf);	//Event Name
		st_rcAttr.pucAttr6 = szAttrBuf[5];

		hErr = UTIL_TCONV_ConvertUnixTimeToDateTime(dbItem.utStTime, &stDate);
		HxSTD_snprintf(szAttrBuf[6], STRING_LEN_MAX, "%02d:%02d:%02d %02d-%02d-%04d",stDate.stTime.ucHour,stDate.stTime.ucMinute,stDate.stTime.ucSecond,\
			stDate.stDate.ucDay,stDate.stDate.ucMonth,stDate.stDate.usYear);  //start-time
		st_rcAttr.pucAttr7 = szAttrBuf[6];

		hErr = UTIL_TCONV_ConvertUnixTimeToDateTime(dbItem.utStTime+dbItem.utDuration, &stDate);
		HxSTD_snprintf(szAttrBuf[7], STRING_LEN_MAX, "%02d:%02d:%02d %02d-%02d-%04d",stDate.stTime.ucHour,stDate.stTime.ucMinute,stDate.stTime.ucSecond,\
			stDate.stDate.ucDay,stDate.stDate.ucMonth,stDate.stDate.usYear); //end-time
		st_rcAttr.pucAttr8 = szAttrBuf[7];

		HxSTD_snprintf(szAttrBuf[8], STRING_LEN_MAX, "%s","false"); //active
		st_rcAttr.pucAttr9 = szAttrBuf[8];


		if(IS_REC(dbItem.eRsvType) == TRUE && dbItem.ext.stRec.ucRecKind == CONTENT_KIND_SERIES )
		{

				HxSTD_PrintToStrN(buf
					, EPG_TVA_CRID_AUTH_LEN + EPG_TVA_CRID_DATA_LEN + 1
					, "%d%s%s"
					, dbItem.ext.stRec.pstSeriesSet->stCrid.type - 0x30
					, (HCHAR *)dbItem.ext.stRec.pstSeriesSet->stCrid.auth
					, (HCHAR *)dbItem.ext.stRec.pstSeriesSet->stCrid.data
				);


				HxSTD_snprintf(szAttrBuf[9], STRING_LEN_MAX, "%s",buf); //active
				st_rcAttr.pucAttr10 = szAttrBuf[9];

				hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_EVENT_BOOKING_SERIES, &st_rcAttr, NULL);
				uiTotalBufSize = ulNewTotalBufSize;
		}
		else
		{
			hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_EVENT_BOOKING, &st_rcAttr, NULL);
			uiTotalBufSize = ulNewTotalBufSize;
		}
	}




	EXIT_FUN :

	if( pRsvItem )
	{
		HxSTD_MemFree( pRsvItem );
	}
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hErr = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_SCHEDULED_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	VK_memcpy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);

	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HxMEM_FREE(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	return hErr;

}
#endif

#if 0
typedef enum
{
	FSAT_RC_KIND_RESERVED_BY_MANUFACTURER	= 0x00,
	FSAT_RC_KIND_DVB_SI						= 0x01,
	FSAT_RC_KIND_DVB_FREESAT				= 0x02,
	FSAT_RC_KIND_TV_ANYTIME					= 0x03,
	FSAT_RC_KIND_HWW						= 0x04,
	FSAT_RC_KIND_RECORDED_ASSET				= 0x05,
} FSAT_RC_KIND_e;
static HERROR			local_rcRsv_SetServiceHandle(Handle_t *phDstSvc, HUINT16 usKind, RC_Data_t *pstRcData)
{
	HERROR				hErr = ERR_OK;

	HUINT32 			 ulSvcCount = 0;
	Handle_t			*phSvcList = NULL;

	if ( pstRcData == NULL )
	{
		HxLOG_Error("[%s:%d] null Rp Data Element\n",__FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	switch ( usKind )
	{
		case FSAT_RC_KIND_DVB_FREESAT:
			hErr = FSAT_DB_SVC_FindServiceListByFsatIdf(eChDeliveryType_SAT, eSvcType_All, eLcnVisibleSvc_FindAll,
												pstRcData->usFsatId, &ulSvcCount, &phSvcList);
			if ( ulSvcCount != 1)
			{
				HxLog_Warning("[%s:%d] multiple fsatid services. # of services are (%d)\n",__FUNCTION__, __LINE__, ulSvcCount);
			}
			// TODO: should be capable of handling alternative list
			// in case of alternative list -> select first service
			if (phSvcList != NULL )
			{
				*phDstSvc = phSvcList[0];
			}
			else
			{
				*phDstSvc = 0;
			}
			if (phSvcList != NULL)
			{
				DB_SVC_FreeServiceList (phSvcList);
			}
			break;

		default :
			HxLog_Debug("[%s:%d] unknown kind = (%d)\n",__FUNCTION__, __LINE__, usKind);
			return ERR_FAIL;

	}

	if ( hErr != ERR_OK )
	{
		HxLOG_Error("[%s:%d] get service handle failed.\n",__FUNCTION__, __LINE__);
		*phDstSvc = 0;
		return hErr;
	}

	HxLog_Debug("[%s:%d] for kind=(%d), service handle=(%d)\n",__FUNCTION__, __LINE__, usKind, *phDstSvc);

	return ERR_OK;
}
static HERROR			local_rcRsv_FindMatchingSlot(HUINT32 *pulSlot, HUINT16 usFsatSvcId, HUINT16 usEvtId)
{
	Handle_t	hSvc = HANDLE_NULL;
	HERROR		hErr = ERR_FAIL;
	HUINT32		ulSlot;
	RC_Data_t	stRcData;

	if ( pulSlot == NULL)
	{
		HxLOG_Error("[%s:%d] Null input. \n", __FUNCTION__ , __HxLINE__);
		return ERR_FAIL;
	}

	// 1. get service handle
	stRcData.usFsatId = usFsatSvcId;
	hErr = local_rcRsv_SetServiceHandle(&hSvc, FSAT_RC_KIND_DVB_FREESAT, &stRcData);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("[%s:%d] get service handle failed.\n",__FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	// 2. get slot
	hErr = MW_RSV_GetSlotByhSvcAndEvtId(hSvc, usEvtId, &ulSlot);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("[%s:%d] get slot with time failed.\n",__FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	// 3. return slot
	*pulSlot = ulSlot;

	return ERR_OK;
}
static RCLIST_RESULT_e		local_rcRsv_ConstructRsvItem(RSV_Item_t * pstRsvItem, RC_Data_t *pstRcData)
{
	SVC_Info_t			stSvcInfo;
	OxEPG_Event_t		*event;
	const HCHAR			*pszEventName;
	HERROR				hErr = ERR_OK;

	VK_memset(pstRsvItem, 0x00, sizeof(RSV_Item_t));
	VK_memset(&pstRsvItem->ext, 0x00, sizeof(RsvExt_t));

	hErr = local_rcRsv_SetServiceHandle(&pstRsvItem->hSvc, pstRcData->usKind, pstRcData);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("[%s:%d] Set service handle failed. Rpdata kind = (%d).\n",__FUNCTION__, __LINE__,pstRcData->usKind);
		return RCLIST_RESULT_FAIL_SERVICE_IS_NOT_FOUND;
	}

	hErr = DB_SVC_GetServiceInfo(pstRsvItem->hSvc, &stSvcInfo);
	if ( hErr != ERR_OK)
	{
		HxLOG_Error("[%s:%d] get service info failed. service handle=(%d),err code=(%d)\n",__FUNCTION__, __LINE__, pstRsvItem->hSvc,hErr);
		return RCLIST_RESULT_FAIL_SERVICE_IS_NOT_FOUND;
	}

	event = OxEPG_FindEvent(
				OxEPGDVB_EVENT_UID(DB_SVC_GetServiceUID(pstRsvItem->hSvc), pstRcData->usUniqueId)
				, eOxEPG_TYPE_PF
				, eOxEPG_TYPE_SC
				, eOxEPG_TYPE_NONE
			);
	if (event == NULL)
	{
		HxLOG_Error("[%s:%d] find event returned null. \n",__FUNCTION__, __LINE__);
		return RCLIST_RESULT_FAIL_EVENT_IS_NOT_FOUND;
	}

	pszEventName = OxEPG_Event_GetName(event, MWC_PARAM_GetMainLanguage());
	if ( pszEventName == NULL )
	{
		HxLOG_Error("[%s:%d] event get name returned null. \n",__FUNCTION__, __LINE__);
	}
	HxLog_Debug("[%s:%d] event name = [%s] \n",__FUNCTION__, __LINE__, pszEventName);

	switch(pstRcData->usRsvType)
	{
		case RC_RSVTYPE_WATCHING: pstRsvItem->eRsvType = RSV_TYPE_WATCHING_TBR; break;
		case RC_RSVTYPE_REC_EBR : pstRsvItem->eRsvType = RSV_TYPE_RECORD_EBR; break;
		case RC_RSVTYPE_REC_TBR : pstRsvItem->eRsvType = RSV_TYPE_RECORD_TBR; break;
		case RC_RSVTYPE_REC_EBR_SERIES: pstRsvItem->eRsvType = RSV_TYPE_RECORD_EBR; break;
		default                 : pstRsvItem->eRsvType = RSV_TYPE_NOTHING;
			HxLog_Debug("[%s:%d] unknown rsv type = (%d)\n",__FUNCTION__, __LINE__, pstRcData->usRsvType);
			break;
	}

	if ( pstRsvItem->eRsvType == RSV_TYPE_RECORD_EBR
		|| pstRsvItem->eRsvType == RSV_TYPE_RECORD_TBR)
	{
		HxSTD_snprintf((HCHAR *)&(pstRsvItem->ext.stRec.szEvtName),MAX_RSV_EVT_NAME,"%s",pszEventName);
		pstRsvItem->ext.stRec.usEvtId	= pstRcData->usUniqueId;
	}
	else if ( pstRsvItem->eRsvType == RSV_TYPE_WATCHING_TBR )
	{
		HxSTD_snprintf((HCHAR *)&(pstRsvItem->ext.stWch.szEvtName),MAX_RSV_EVT_NAME,"%s",pszEventName);
		pstRsvItem->ext.stWch.usEvtId	= pstRcData->usUniqueId;
	}
	else
	{
		HxLOG_Error("[%s:%d] Unknown RsvType=(%d) \n",__FUNCTION__, __LINE__, pstRcData->usRsvType);
		hErr = RCLIST_RESULT_FAIL;
	}

	/* Reservation Variable Init. */
	pstRsvItem->ulSlot				= RSV_SLOT_NULL;
	pstRsvItem->status				= RSV_ITEM_STATUS_WAITING;
	pstRsvItem->eReady				= RSV_READY_30_SEC;
	pstRsvItem->utStTime			= event->startTime;
	pstRsvItem->utDuration			= event->duration;
	pstRsvItem->bExceptConflict		= FALSE;
	pstRsvItem->hMasterSvc			= pstRsvItem->hSvc;
#if defined(CONFIG_APCORE_RP_HUMAX)
	pstRsvItem->eKind				= RSV_KIND_REMOTE;
#endif

	switch(pstRcData->ucRepeat)
	{
		case RC_REPEAT_ONCE     : pstRsvItem->eRepeat = RSV_ONCE;   break;
		case RC_REPEAT_DAILY    : pstRsvItem->eRepeat = RSV_DAILY;  break;
		case RC_REPEAT_WEEKLY   : pstRsvItem->eRepeat = RSV_WEEKLY; break;
		case RC_REPEAT_SERIES   : pstRsvItem->eRepeat = RSV_ONCE; break;
		default                 :
			HxLog_Debug("[%s:%d] unknown repeat type = (%d)\n",__FUNCTION__, __LINE__, pstRcData->ucRepeat);
			break;
	}



	return RCLIST_RESULT_OK;
}
#if defined(CONFIG_MW_SI_TVANYTIME_SERIES_REC)
static HERROR			local_rcRsv_SetScheduleCridAndName(RSV_Item_t *pstRsvItem, HBOOL isSeries)
{
	HUINT32 	ulContentKind;
	HUINT8		ucRecKind;
	HERROR		hErr = ERR_FAIL;

	hErr = MWC_TVA_GetContentKind(pstRsvItem->hSvc, pstRsvItem->ext.stRec.usEvtId, &ulContentKind);
	ucRecKind	= (hErr != ERR_OK)
				? CONTENT_KIND_NONE
				: (ulContentKind & CONTENT_KIND_SPLIT)
					? CONTENT_KIND_SPLIT
					: CONTENT_KIND_SINGLE;

	if(isSeries == TRUE)
	{
		ucRecKind = CONTENT_KIND_SERIES;
	}

	hErr = MWC_TVA_SetScheduleCridAndName(pstRsvItem->ulSlot, pstRsvItem, pstRsvItem->ext.stRec.usEvtId, ucRecKind);
	if(hErr != ERR_OK)
	{
		MW_EPG_GetEventIdAndNameByTime(pstRsvItem->hSvc, pstRsvItem->utStTime, &pstRsvItem->ext.stRec.usEvtId, pstRsvItem->ext.stRec.szEvtName, MAX_RSV_EVT_NAME);
	}

	return ERR_OK;
}
static RCLIST_RESULT_e local_rcRsv_DoReservation(RC_Data_t *pstRcData,HUINT32 *ulSlot,AM_Conflict_Info_t *conflictInfo,RSV_Item_t *rsvitemInfo)
{
	RCLIST_RESULT_e			hResult = RCLIST_RESULT_OK;
	HERROR					hErr	= ERR_OK;

	RSV_Item_t	       		stRsvItems;
	AM_Conflict_Info_t		conflict ;
	HBOOL					isSeries = FALSE;

	HxSTD_MemSet(&conflict,0x00,sizeof(AM_Conflict_Info_t));

	hResult = local_rcRsv_ConstructRsvItem(&stRsvItems, pstRcData);
	if ( hResult != RCLIST_RESULT_OK )
	{
		HxLOG_Print("[%s:%d] wrong hSvc ! (hSvc=0x%x)\n",__FUNCTION__, __LINE__, stRsvItems.hSvc);

		return hResult;
	}

	hErr  = MW_RSV_GetAvailableSlot(stRsvItems.eRsvType, &stRsvItems.ulSlot);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[%s:%d] There is no available slot~!! type(%d)\n", __FUNCTION__ , __HxLINE__,stRsvItems.eRsvType);
		return RCLIST_RESULT_FAIL_SLOT_ERROR;
	}
#if 0
		HUINT32 	ulSlot;
		RsvType_t	eRsvType;						/**< Rsv TYPE(Watching, Record, Power on/off etc). */
		Handle_t	hSvc;							/**< Service Handle. */
#if defined(CONFIG_OP_FREESAT)
		Handle_t hMasterSvc;
#endif
		RsvReadyT_t eReady; 						/**< Rsv Ready Time. */
		UNIXTIME	utStTime;						/**< Rsv Start Time. */
		UNIXTIME	utDuration; 					/**< Rsv Duration. */
		RsvRepeat_t eRepeat;						/**< Rsv Repeat Mode. */
		HBOOL		bExceptConflict;				/**< Conflict 체크 제외 할지 여부 */
#if defined(CONFIG_APCORE_RP_HUMAX)
		RsvKind_t	eKind;							/** Local, Remote */
#endif
		RsvExt_t	ext;

		/* application 용으로 사용 */
		RsvStatus_t status; 						/**< Rsv Status.(app에서는 참고만 할 것) */
		Handle_t	hModule;						/**<  MODULE_HANDLE 값 : 예약을 걸은 Module Handle (AppMgr) */
		Handle_t	hRsvInfo;						/**< Application에서 정보를 찾기위한 구조체 - _RSVINFO_t 참고*/

		HUINT32 ulPvrIdx;						/** 녹화할 HDD index*/
#endif
	rsvitemInfo->ulSlot = stRsvItems.ulSlot;
	rsvitemInfo->eRsvType = stRsvItems.eRsvType;
	rsvitemInfo->hSvc = stRsvItems.hSvc;
	rsvitemInfo->hMasterSvc = stRsvItems.hMasterSvc;
	rsvitemInfo->eReady = stRsvItems.eReady;
	rsvitemInfo->utStTime = stRsvItems.utStTime;
	rsvitemInfo->utDuration = stRsvItems.utDuration;
	rsvitemInfo->eRepeat = stRsvItems.eRepeat;
	rsvitemInfo->bExceptConflict = stRsvItems.bExceptConflict;
	HxSTD_MemCopy(&rsvitemInfo->ext ,&stRsvItems.ext,sizeof(stRsvItems.ext));
	rsvitemInfo->status = stRsvItems.status;
	rsvitemInfo->hModule = stRsvItems.hModule;
	rsvitemInfo->hRsvInfo = stRsvItems.hRsvInfo;
	rsvitemInfo->ulPvrIdx = stRsvItems.ulPvrIdx;
	*ulSlot =stRsvItems.ulSlot;

#if defined(CONFIG_MW_SI_TVANYTIME_SERIES_REC)
	isSeries = (pstRcData->ucRepeat == RC_REPEAT_SERIES)? TRUE:FALSE;
	hErr  = local_rcRsv_SetScheduleCridAndName(&stRsvItems,isSeries);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[%s:%d] local_rcRsv_SetScheduleCridAndName failed\n", __FUNCTION__ , __HxLINE__);
		return RCLIST_RESULT_RESERVED;
	}
#endif

	hErr = AP_AM_CheckReservationConflict(&stRsvItems, &conflict);
	if (hErr != ERR_OK
		|| conflict.eType != eAM_Conflict_Nothing
		)
	{
		HUINT32 i;
		HxLog_Warning("[%s:%d] Conflict occured. \n", __HxFILE__, __HxLINE__);
		conflictInfo->eType = conflict.eType;
		conflictInfo->hSvcHandle = conflict.hSvcHandle;
		conflictInfo->ulNumOfRsv = conflict.ulNumOfRsv;
		conflictInfo->usTsIndex = conflict.usTsIndex;
		conflictInfo->usEventId = conflict.usEventId;
		for (i = 0; i < conflict.ulNumOfRsv; i++)
		{
			conflictInfo->aulSlot[i] = conflict.aulSlot[i];
			conflictInfo->ausTsIndex[i] = conflict.ausTsIndex[i];
			conflictInfo->ausEventId[i] = conflict.ausEventId[i];
			conflictInfo->ahSvcHandle[i] = conflict.ahSvcHandle[i];
		}
		return RCLIST_RESULT_FAIL_CONFLICT;
	}

	hErr = MW_RSV_MakeReservedItem(stRsvItems.ulSlot, &stRsvItems);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[%s:%d] Make reserved Item failed\n", __FUNCTION__ , __HxLINE__);
		return RCLIST_RESULT_FAIL_RESERVATION_IS_NOT_FOUND;
	}

#if defined(CONFIG_MW_SI_TVANYTIME_SERIES_REC)
	MWC_TVA_UpdateTVASchedulesToEarliest();
#endif

	return RCLIST_RESULT_OK;
}
#endif
#endif

#if 0		// [GET] Put Scheduled
#define	FSATRC_XMLELEMENT_RSV_PROGRAMME		"programme-booking"
#define	FSATRC_XMLELEMENT_RSV_SCHEDULED		"scheduled"

#define	FSATRC_XMLATTR_RSV_TYPE		"type"
#define	FSATRC_XMLATTR_RSV_CHID		"sid"
#define	FSATRC_XMLATTR_RSV_EVENTID		"cid"
#define	FSATRC_XMLATTR_RSV_STARTTIME		"start-time"
#define	FSATRC_XMLATTR_RSV_ENDTIME		"end-time"

RCLIST_RESULT_e Get_RC_Put_Scheduled(HUINT8 *pucUploadBuf,HUINT8* command,HUINT32 commandLen,HUINT32 *ulUploadLen)
{
	HERROR	hError = ERR_OK;
	HXMLDOC hNewXMLDoc = NULL;
	HXMLELEMENT	hXMLElement = NULL, hXMLChildElement = NULL,hXMLNextElement = NULL;
	POINTER_LIST_T	*pPointerList = NULL, *pPointerListTemp = NULL;
	HUINT8	*pszRsvType = NULL;
	HUINT8	*pszFsatId = NULL;
	HUINT8	*pszEventId = NULL;
	RCLIST_RESULT_e  hResult = RCLIST_RESULT_FAIL;
	HUINT32		ulSlot = 0;
	RC_Data_t		rcData;
	HUINT8		szParams[64];
	AM_Conflict_Info_t		conflict;
	RSV_Item_t		rsvmInfo;
	HUINT32					uiTotalBufSize = 0;


	rcData.usKind = FSAT_RC_KIND_DVB_FREESAT;

	hError = PAL_XML_InitParser();
	if(hError != ERR_XML_OK)
	{
		HxLOG_Error("[%s][%d]:	XML parser init was failed \n", __FUNCTION__, __LINE__);
		goto End_Function;
	}

	hError = PAL_XML_ParseBuffer(command, commandLen, &hNewXMLDoc);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Error("[%s][%d]:	XML paser was not able to parse the file [%s] \n", __FUNCTION__, __LINE__, command);
		goto End_Function;
	}

	hError = PAL_XML_GetRootElement(hNewXMLDoc, &hXMLElement);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Error("[%s][%d]:	XML parser was failed to get ROOT element \n", __FUNCTION__, __LINE__);
		goto End_Function;
	}

	hError = PAL_XML_FindChildElementByName(hXMLElement, FSATRC_XMLELEMENT_RSV_SCHEDULED, &pPointerList);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Error("[%s][%d]:	PAL_XML_FindChildElementByName Errort \n", __FUNCTION__, __LINE__);
		goto End_Function;
	}

	hXMLChildElement = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pPointerList);

	hError = PAL_XML_FindChildElementByName(hXMLChildElement, FSATRC_XMLELEMENT_RSV_PROGRAMME, &pPointerListTemp);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Error("[%s][%d]:	PAL_XML_FindChildElementByName Errort \n", __FUNCTION__, __LINE__);
		goto End_Function;
	}

	hXMLNextElement = (HXMLELEMENT)UTIL_LINKEDLIST_GetContents(pPointerListTemp);
	hError = PAL_XML_GetAttrContents(hXMLNextElement, FSATRC_XMLATTR_RSV_TYPE, &pszRsvType); //type = record/series / reminder/all
	if(hError == ERR_OK)
	{
		if(pszRsvType == NULL)
		{
			hResult = RCLIST_RESULT_FAIL_RESERVATION_IS_NOT_FOUND;

			goto End_Function;
		}
		if(HLIB_STD_StrCaseCmp(pszRsvType,"reminder") == 0)
		{
			rcData.usRsvType = RC_RSVTYPE_WATCHING;
			rcData.ucRepeat = RC_REPEAT_ONCE;

			hError = PAL_XML_GetAttrContents(hXMLNextElement, FSATRC_XMLATTR_RSV_CHID, &pszFsatId); //type = record/series / reminder/all
			if(hError == ERR_OK)
			{
				rcData.usFsatId = atoi(pszFsatId);
				PAL_XML_FreeAttrContents(pszFsatId);
			}
			hError = PAL_XML_GetAttrContents(hXMLNextElement, FSATRC_XMLATTR_RSV_EVENTID, &pszEventId);
			if(hError == ERR_OK)
			{
				rcData.usUniqueId= atoi(pszEventId);
				PAL_XML_FreeAttrContents(pszEventId);
			}

#if 0
			hError = PAL_XML_GetAttrContents(hXMLNextElement, FSATRC_XMLATTR_RSV_STARTTIME, &pszStarttime); //type = record/series / reminder/all
			if(hError == ERR_OK)
			{
				if(pszStarttime != NULL)
				{
					HUINT32 i,j,date=0,time=0;
					for(i = 0,j=0;i<sizeof(pszStarttime);i++)
					{
						if(HxSTD_StrNCmp(pszStarttime[i],":",1) == 0 || HxSTD_StrNCmp(pszStarttime[i]," ",1) == 0)
						{

							ucDateTime[j] = 0;
							if(time == 0)
								stDateTime.stTime.ucHour = atoi(ucDateTime);
							else if(time == 1)
								stDateTime.stTime.ucMinute= atoi(ucDateTime);
							else if(time == 2)
								stDateTime.stTime.ucSecond= atoi(ucDateTime);
							if(time <=3)
								time++;
							j = 0;

						}
						else if(HxSTD_StrNCmp(pszStarttime[i],"-",1) == 0)

						{
							ucDateTime[j] = 0;
							if(date == 0)
								stDateTime.stDate.ucDay = atoi(ucDateTime);
							else if(date == 1)
								stDateTime.stDate.ucMonth = atoi(ucDateTime);

							if(time > 3 && date <=3)
								date++;
							j = 0;

						}
						ucDateTime[j++] = pszStarttime[i];
					}

					ucDateTime[j] = 0;
					stDateTime.stDate.usYear= atoi(ucDateTime);
					/* Get Current Time */
					hError = VK_CLOCK_GetTime (&curTime);
					if ( hError != ERR_OK )
					{
						HxLog_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
						//return	ERR_FAIL;
					}
#if 0
					HxLog_Info("\t - Date : (%d.%d.%d) & Time : (%dh:%dm:%ds) : %0x%x]\n",
					(int)stDateTime.stDate.usYear,(int)stDateTime.stDate.ucMonth,(int)stDateTime.stDate.ucDay, (int)stDateTime.stTime.ucHour,(int)stDateTime.stTime.ucMinute,(int)stDateTime.stTime.ucSecond, curTime);
#endif
				//	UTIL_TCONV_ConvertUnixTimeToDateTime(curTime, &stRsvDate);

					hError = UTIL_TCONV_ConvertDateTimeToUnixTime (&stDateTime, &ulRsvUnixStarttime);
					rcData.utStartTime = ulRsvUnixStarttime;

				}

				PAL_XML_FreeAttrContents(pszStarttime);
			}

			hError = PAL_XML_GetAttrContents(hXMLNextElement, FSATRC_XMLATTR_RSV_ENDTIME, &pszEndtime); //type = record/series / reminder/all
			if(hError == ERR_OK)
			{
				if(pszEndtime != NULL)
				{
					HUINT32 i,j,date=0,time=0;
					for(i = 0,j=0;i<sizeof(pszEndtime);i++)
					{
						if(HxSTD_StrNCmp(pszEndtime[i],":",1) == 0 || HxSTD_StrNCmp(pszEndtime[i]," ",1) == 0)
						{

							ucDateTime[j] = 0;
							if(time == 0)
								stDateTime.stTime.ucHour = atoi(ucDateTime);
							else if(time == 1)
								stDateTime.stTime.ucMinute= atoi(ucDateTime);
							else if(time == 2)
								stDateTime.stTime.ucSecond= atoi(ucDateTime);
							if(time <=3)
								time++;
							j = 0;

						}
						else if(HxSTD_StrNCmp(pszEndtime[i],"-",1) == 0)

						{
							ucDateTime[j] = 0;
							if(date == 0)
								stDateTime.stDate.ucDay = atoi(ucDateTime);
							else if(date == 1)
								stDateTime.stDate.ucMonth = atoi(ucDateTime);

							if(time > 3 && date <=3)
								date++;
							j = 0;

						}
						ucDateTime[j++] = pszEndtime[i];
					}

					ucDateTime[j] = 0;
					stDateTime.stDate.usYear= atoi(ucDateTime);
					/* Get Current Time */
					hError = VK_CLOCK_GetTime (&curTime);
					if ( hError != ERR_OK )
					{
						HxLog_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
						//return	ERR_FAIL;
					}
#if 0
					HxLog_Info("\t - Date : (%d.%d.%d) & Time : (%dh:%dm:%ds) : %0x%x]\n",
					(int)stDateTime.stDate.usYear,(int)stDateTime.stDate.ucMonth,(int)stDateTime.stDate.ucDay, (int)stDateTime.stTime.ucHour,(int)stDateTime.stTime.ucMinute,(int)stDateTime.stTime.ucSecond, curTime);
#endif
				//	UTIL_TCONV_ConvertUnixTimeToDateTime(curTime, &stRsvDate);

					hError = UTIL_TCONV_ConvertDateTimeToUnixTime (&stDateTime, &ulRsvUnixEndtime);
					//rcData.utEndtime = ulRsvUnixEndtime;

				}

				PAL_XML_FreeAttrContents(pszEndtime);
			}

			ulRsvCount = MW_RSV_CountItem();
			if( ulRsvCount == 0 )
			{

			}

			if( ulRsvCount > MAX_RSV_ALL )
			{
				HxLOG_Print("[%s][%d] ulRsvCount(%d) exceed max rsv(%d)\n", __FUNCTION__,__LINE__,ulRsvCount, MAX_RSV_ALL);
				goto EXIT_FUN;
			}

			if( ulRsvCount > 0 )
#endif
			hResult = local_rcRsv_DoReservation(&rcData,&ulSlot,&conflict,&rsvmInfo);

		}
		else if(HLIB_STD_StrCaseCmp(pszRsvType,"record") == 0)
		{
			rcData.usRsvType = RC_RSVTYPE_REC_EBR;
			rcData.ucRepeat = RC_REPEAT_ONCE;

			hError = PAL_XML_GetAttrContents(hXMLNextElement, FSATRC_XMLATTR_RSV_CHID, &pszFsatId); //type = record/series / reminder/all
			if(hError == ERR_OK)
			{
				rcData.usFsatId = atoi(pszFsatId);
				PAL_XML_FreeAttrContents(pszFsatId);
			}
			hError = PAL_XML_GetAttrContents(hXMLNextElement, FSATRC_XMLATTR_RSV_EVENTID, &pszEventId); //type = record/series / reminder/all
			if(hError == ERR_OK)
			{
				char* stop;
				rcData.usUniqueId = strtol( pszEventId, &stop, 16 );
				PAL_XML_FreeAttrContents(pszEventId);
			}

			hResult = local_rcRsv_DoReservation(&rcData,&ulSlot,&conflict,&rsvmInfo);


		}
		else if(HLIB_STD_StrCaseCmp(pszRsvType,"series") == 0)
		{
			rcData.usRsvType = RC_RSVTYPE_REC_EBR_SERIES;
			rcData.ucRepeat = RC_REPEAT_SERIES;
			hError = PAL_XML_GetAttrContents(hXMLNextElement, FSATRC_XMLATTR_RSV_CHID, &pszFsatId); //type = record/series / reminder/all
			if(hError == ERR_OK)
			{
				rcData.usFsatId = atoi(pszFsatId);
				PAL_XML_FreeAttrContents(pszFsatId);
			}
			hError = PAL_XML_GetAttrContents(hXMLNextElement, FSATRC_XMLATTR_RSV_EVENTID, &pszEventId); //type = record/series / reminder/all
			if(hError == ERR_OK)
			{
				char* stop;
				rcData.usUniqueId = strtol( pszEventId, &stop, 16 );
				PAL_XML_FreeAttrContents(pszEventId);
			}

			hResult = local_rcRsv_DoReservation(&rcData,&ulSlot,&conflict,&rsvmInfo);
		}
		PAL_XML_FreeAttrContents(pszRsvType);
	}

	if(hResult == RCLIST_RESULT_OK)
	{
		HxSTD_snprintf(szParams, sizeof(szParams),"/rc/scheduled/%d", ulSlot);
		HxSTD_MemSet(&conflict,0x00,sizeof(conflict));
		hError = Get_RC_Current_Conflict_Scheduled(pucUploadBuf,szParams,&rsvmInfo,conflict,&uiTotalBufSize);
	}
	else if(hResult == RCLIST_RESULT_FAIL_CONFLICT)
	{
		HxSTD_snprintf(szParams, sizeof(szParams),"/rc/scheduled/%d", ulSlot);
		hError = Get_RC_Current_Conflict_Scheduled(pucUploadBuf,szParams,&rsvmInfo,conflict,&uiTotalBufSize);
	}
	else
	{
		HxSTD_snprintf(szParams, sizeof(szParams),"/rc/scheduled/%d", ulSlot);
		hError = Get_RC_Current_Conflict_Scheduled(pucUploadBuf,szParams,&rsvmInfo,conflict,&uiTotalBufSize);
	}




End_Function:
	HxLog_Info("[%s][%d]: -\n", __FUNCTION__, __LINE__);

	pPointerList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pPointerList, NULL);
	if(pPointerList != NULL)
	{
		HxLOG_Error("[%s][%d]:	failed to free pointer list \n", __FUNCTION__, __LINE__);
	}

	pPointerListTemp = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pPointerListTemp, NULL);
	if(pPointerListTemp != NULL)
	{
		HxLOG_Error("[%s][%d]:	failed to free pointer list \n", __FUNCTION__, __LINE__);
	}

	if(hNewXMLDoc != NULL)
	{
		PAL_XML_FreeDocument(hNewXMLDoc);
	}

	return hResult;

}
#endif

#if 0		// [GET] Delete Scheduled
/********************************************************************
*	function name : Get_RC_Delete_Scheduled
*	input : command ( slot number in string format )
*	output :
*	description :
********************************************************************/
HERROR Get_RC_Delete_Scheduled(HUINT8* command,HUINT32 commandLen)
{
	HERROR		hErr	= ERR_OK;

	HUINT32		ulEpisodeId;
	HUINT32		ulSlot;
	HUINT32		ulSvcId;
	HUINT16		usFsatSvcId;
	HUINT32		ulEvtId;
	HUINT16		usEvtId;

	if ( command == NULL )
	{
		HxLog_Error("\033[34m[%s:%d] command is null \033[0m\n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	// 1. parsing
	sscanf(command,"P_%d_%d",&ulSvcId,&ulEvtId);
	usFsatSvcId = ulSvcId;
	usEvtId		= ulEvtId;
	HxLog_Debug("[%s:%d] parsed: usFsatSvcId=(%d), usEvtId=(%d) \n",__FUNCTION__, __LINE__, usFsatSvcId ,usEvtId);

	// 2. get slot
	hErr = local_rcRsv_FindMatchingSlot(&ulSlot, usFsatSvcId, usEvtId);
	if ( hErr != ERR_OK)
	{
		HxLOG_Error("[%s:%d] local_rpRsv_FindMatchingSlot failed. \n", __FUNCTION__ , __HxLINE__);
		return ERR_FAIL;
	}

	// 3. get episode id
	hErr = MW_RSV_GetEpisodeIdByEvtId(ulSlot, usEvtId, &ulEpisodeId);
	if ( hErr != ERR_OK)
	{
		HxLOG_Error("[%s:%d] MW_RSV_GetEpisodeIdByTime failed. \n", __FUNCTION__ , __HxLINE__);
		return ERR_FAIL;
	}
	HxLog_Debug("[%s:%d] ulSlot(%d) ulEpisodeId(%d) \n",__FUNCTION__,__LINE__,ulSlot,ulEpisodeId);
	// 4. cancel reservation
	hErr  = MW_RSV_CancelSingleEpisode(ulSlot, ulEpisodeId);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[%s:%d] Cancel reservation failed. slot(%d) episode id(%d)\n", __FUNCTION__ , __HxLINE__, ulSlot, ulEpisodeId);
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif

#if 1
static HERROR local_RC_EventsURI(HUINT8 *pucURI,HUINT32 subEventType,HINT32 lParam1,HINT32 lParam2,HINT32 lParam3)
{
	HERROR hErr = ERR_OK;
//	SVC_Info_t	stSvcInfo;
//	Handle_t		hSvc;
//	UNIXTIME		uxCurrentTime;
//	HUINT16 		usEvtId;
//	HUINT8			EvtName[EVT_NAME_LEN_MAX];
//	HCHAR			tempbuf[EVT_NAME_LEN_MAX];
	HUINT8			*pucTempURI;
	HUINT8			szTempURI[STRING_LEN_MAX] = {0,};
	HUINT32			ulURISize = 0;


	pucTempURI = pucURI;
	ulURISize  = 0;

	switch(subEventType)
	{
		case RCEVT_SUBTYPE_RESOURCE_NAME:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_RESOURCE_NAME].szEventDesc);
			break;

		case RCEVT_SUBTYPE_RESOURCE_PAIR:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_RESOURCE_PAIR].szEventDesc);
			break;

		case RCEVT_SUBTYPE_RESOURCE_UNPAIR:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_RESOURCE_UNPAIR].szEventDesc);
			break;

		case RCEVT_SUBTYPE_OUTPUTS_PROGRAM:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_OUTPUTS_PROGRAM].szEventDesc, lParam1);
			break;

		case RCEVT_SUBTYPE_OUTPUTS_PROGRAM_OVERRIDE:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_OUTPUTS_PROGRAM_OVERRIDE].szEventDesc, lParam1);
			break;

		case RCEVT_SUBTYPE_OUTPUTS_APP:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_OUTPUTS_APP].szEventDesc, lParam1);
			break;

		case RCEVT_SUBTYPE_OUTPUTS_SETTINGS_VOLUME:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_OUTPUTS_SETTINGS_VOLUME].szEventDesc);
			break;

		case RCEVT_SUBTYPE_OUTPUTS_SETTINGS_MUTE:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_OUTPUTS_SETTINGS_MUTE].szEventDesc);
			break;

		case RCEVT_SUBTYPE_OUTPUTS_SETTINGS_SUBTITLE:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_OUTPUTS_SETTINGS_SUBTITLE].szEventDesc);
			break;

		case RCEVT_SUBTYPE_OUTPUTS_SETTINGS_AD:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_OUTPUTS_SETTINGS_AD].szEventDesc);
			break;

		case RCEVT_SUBTYPE_OUTPUTS_PROGRAM_LIVE_PAUSE:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_OUTPUTS_PROGRAM_LIVE_PAUSE].szEventDesc);
			break;

		case RCEVT_SUBTYPE_OUTPUTS_PROGRAM_PLAYBACK:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_OUTPUTS_PROGRAM_PLAYBACK].szEventDesc);
			break;

		case RCEVT_SUBTYPE_LOCALE_TUNER:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_LOCALE_TUNER].szEventDesc);
			break;

		case RCEVT_SUBTYPE_LOCALE_POSTCODE:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_LOCALE_POSTCODE].szEventDesc);
			break;

		case RCEVT_SUBTYPE_LOCALE_PLATFORM:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_LOCALE_PLATFORM].szEventDesc);
			break;

		case RCEVT_SUBTYPE_CA:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_CA].szEventDesc);
			break;

		case RCEVT_SUBTYPE_POWER_STANDBY:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_POWER_STANDBY].szEventDesc);
			break;

		case RCEVT_SUBTYPE_POWER_ON:
			HxSTD_snprintf(szTempURI, STRING_LEN_MAX, s_stRC_EventTypes[RCEVT_SUBTYPE_POWER_ON].szEventDesc);
			break;

		default:
			break;
	}

	if(pucTempURI)
	{
		HxSTD_MemCopy(pucTempURI + ulURISize, szTempURI, HxSTD_StrLen(szTempURI));
		ulURISize += HxSTD_StrLen(szTempURI);
	}

	return hErr;
}

HERROR Get_RC_Events(HUINT8* pucUploadBuf,HUINT8 *command,HUINT32 *ulUploadLen)
{
	HERROR					hError = ERR_OK;
	RC_ELEMENT_t			*pstElement = NULL;
	RC_ATTR_ELEMENT_t		st_rcAttr;
//	HUINT8					eventcommand[STRING_LEN_MAX] = {0, };
	HUINT32 				ulRequestId = 0;
	RCEvent_Element			*rcEvent = NULL;
	HUINT8					szAttrBuf[11][STRING_LEN_MAX];
	HUINT8					*pucEvents = NULL;
	HUINT32					ulIndex = 0;
	HUINT32					i;
	HUINT32					uiTotalBufSize = 0,ulNewTotalBufSize = 0;
	HUINT8					*pucTempUploadBuf;

	HxDATETIME_t 			dateTime;

	/* element load */
	hError = local_rc_loadElement(RC_ELEMENT_LIST_EVENTS, &pstElement);
	if( hError != ERR_OK )
	{
		HxLOG_Print("[%s][%d]  load element ERROR \n",__FUNCTION__,__LINE__);
		return hError;
	}

#if 0
	if(HxSTD_StrLen(command) > 10)
	{
		sscanf(command,"/rc/events?since=%d",&ulRequestId);
		HxSTD_snprintf(eventcommand,STRING_LEN_MAX,"/rc/events?since=%d",ulRequestId);
		RemoteCtrl_SetLastReqChangeId(ulRequestId);
	}
	else
	{
		HxSTD_snprintf(eventcommand,STRING_LEN_MAX,s_pszresources[eResourceType_Events]);
		RemoteCtrl_SetLastReqChangeId(g_usElemCnt);
	}
#else		// freesat 대비 prototype이 변경 됨
	sscanf(command,"/rc/events?since=%d",&ulRequestId);
	RemoteCtrl_SetLastReqChangeId(ulRequestId);
#endif

	st_rcAttr.pucAttr1 = command;

	pucTempUploadBuf = HLIB_STD_MemAlloc(PACKET_MAX_BUF_SIZE);
	if(pucTempUploadBuf == NULL)
	{
		HxLOG_Print("[%s][%d]  HLIB_STD_MemAlloc() ERROR \n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}
	uiTotalBufSize	= 0;

	hError = Make_RC_XML_Header(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize);
	uiTotalBufSize = ulNewTotalBufSize;

	// <response>
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_EVENTS_RESPONSE_OPEN, &st_rcAttr, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_EVENTS_OPEN, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	pucEvents = (HUINT8 *)HLIB_STD_MemAlloc(sizeof(HUINT8) * STRING_LEN_MAX);
	if(pucEvents == NULL)
	{
		HxLOG_Print("[RemoteControl_StreamRC_Sources] Memory Allocation Fail \n");
		goto End_Function;
	}

// events info
	for(i = 0,ulIndex = 0;i<MAX_EVENT_LOG_CNT;i++)
	{
		rcEvent = RemoteCtrl_GetNextEventLog();

		if(NULL == rcEvent)
		{
			HxLOG_Print("rcEvent NULL\n");
			break;
		}

		if(ulIndex >= 100) break;

		if(g_ulLastReqRepeat > rcEvent->ulRepeat) break; // request한 event 정보가 이전의 정보이면  더이상 최신 update event log는 없다.

		if(HxSTD_StrLen(command) > 10 && ulRequestId == rcEvent->usChangeID) break; //한번 부른 event는 다시 부르지 않는다.

		HxSTD_MemSet(pucEvents, 0, sizeof(HUINT8) * STRING_LEN_MAX);
		hError = local_RC_EventsURI(pucEvents,rcEvent->eRCEvtSubType,rcEvent->ulParam1,rcEvent->ulParam2,rcEvent->ulParam3);
		if(hError == ERR_OK && pucEvents != NULL)
		{
			if(ulIndex == 0)
			{
				RemoteCtrl_SetLastReqRepeat(rcEvent->ulRepeat);
			}
			HxSTD_MemSet(&st_rcAttr, 0x00, sizeof(RC_ATTR_ELEMENT_t));
			HxSTD_snprintf(szAttrBuf[0],STRING_LEN_MAX,"%d", RemoteCtrl_GetLastReqChangeId());
			st_rcAttr.pucAttr1 = szAttrBuf[0];
			HxSTD_snprintf(szAttrBuf[1],STRING_LEN_MAX,"%s",pucEvents);
			st_rcAttr.pucAttr2 = szAttrBuf[1];

			HxSTD_MemSet(&dateTime,0,sizeof(HxDATETIME_t));
			/* 만약 local time이 invalide 하다면, 에러가 나겠지. */
			(void)HLIB_DATETIME_ConvertUnixTimeToDateTime(rcEvent->ulEvtTime, &dateTime);
			HxSTD_snprintf(szAttrBuf[2],STRING_LEN_MAX, "%02d:%02d:%02d %02d-%02d-%04d", dateTime.stTime.ucHour, dateTime.stTime.ucMinute, dateTime.stTime.ucSecond,
							dateTime.stDate.ucDay, dateTime.stDate.ucMonth, dateTime.stDate.usYear);
			st_rcAttr.pucAttr3 = szAttrBuf[2];

			hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_EVENTS_DATA, &st_rcAttr, NULL);
			uiTotalBufSize = ulNewTotalBufSize;
			ulIndex++;
		}
	}

	if(pucEvents != NULL)
		HLIB_STD_MemFree(pucEvents);

End_Function:
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_EVENTS_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;
	hError = local_rc_common(pucTempUploadBuf,uiTotalBufSize,&ulNewTotalBufSize,pstElement, RC_EVENTS_RESPONSE_CLOSE, NULL, NULL);
	uiTotalBufSize = ulNewTotalBufSize;

	if(uiTotalBufSize > PACKET_MAX_BUF_SIZE)
	{
		HxLOG_Print("\t[%s][%d]  @@@ Event logs are too large(%d)..!!! \n",__FUNCTION__,__LINE__, uiTotalBufSize);
		uiTotalBufSize = PACKET_MAX_BUF_SIZE;
	}
	HxSTD_MemCopy(pucUploadBuf, pucTempUploadBuf, uiTotalBufSize);
	pucUploadBuf[uiTotalBufSize] = '\0';
	*ulUploadLen = uiTotalBufSize;

	HLIB_STD_MemFree(pucTempUploadBuf);
	pucTempUploadBuf = NULL;

	HxLOG_Print("[%s][%d] (%s)-\n", __FUNCTION__,__LINE__,pucUploadBuf);

	return hError;

}
#endif

#if 0
#define		FSATRC_XMLELEMENT_EVENTS			"events"
#define		FSATRC_XMLATTR_EVENTS_COMMAND		"cmd"
#define		FSATRC_XMLATTR_EVENTS_ONSCREEN		"onscreen"
#define		FSATRC_XMLATTR_EVENTS_MESSAGE		"msg"

HERROR Send_RC_Events(HUINT8* pBody,HUINT32 ulBodyLen)
{
	HERROR	hError = ERR_FAIL;
	HXMLDOC hNewXMLDoc = NULL;
	HXMLELEMENT	hXMLElement = NULL;
	HUINT8	*pszRCmdType = NULL;

//Freesat Mode 인 가 ? 아니면 Pass
	hError = PAL_XML_InitParser();
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[%s][%d]:	XML parser init was failed \n", __FUNCTION__, __LINE__);
		goto End_Function;
	}

	hError = PAL_XML_ParseBuffer(pBody, ulBodyLen, &hNewXMLDoc);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[%s][%d]:	XML paser was not able to parse the file [%s] \n", __FUNCTION__, __LINE__, pBody);
		goto End_Function;
	}

	hError = PAL_XML_GetRootElement(hNewXMLDoc, &hXMLElement);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[%s][%d]:	XML parser was failed to get ROOT element \n", __FUNCTION__, __LINE__);
		goto End_Function;
	}

	hError = PAL_XML_GetAttrContents(hXMLElement, FSATRC_XMLATTR_EVENTS_COMMAND, &pszRCmdType); //cmd : "remoterecord"
	if(hError == ERR_OK)
	{
		if(pszRCmdType == NULL)
		{
			HxLOG_Print("pszRCmdType is NULL\n");
			hError = ERR_FAIL;
			goto End_Function;
		}
		if(HLIB_STD_StrCaseCmp(pszRCmdType,"remoterecordsync") == 0)
		{
			HxLOG_Print("POST Message MSG_APP_RP_GET_RPLIST \n");
			//POST Event to RR.
			BUS_PostMessage(NULL, MSG_APP_RP_GET_RPLIST, (Handle_t)HANDLE_NULL, 0, 0, 0);
		}

		PAL_XML_FreeAttrContents(pszRCmdType);
	}


End_Function:
	HxLog_Info("[%s][%d]: -\n", __FUNCTION__, __LINE__);



	if(hNewXMLDoc != NULL)
	{
		PAL_XML_FreeDocument(hNewXMLDoc);
	}

	return hError;

}
#endif

#if 0
#define		FSATRC_XMLELEMENT_CONTENT_LINK			"content_link"
#define		FSATRC_XMLATTR_FREETIME_TYPE			"type"
#define		FSATRC_XMLATTR_FREETIME_SERVICEID		"serviceid"
#define		APP_URI_FREETIME		"http://services.platform.freesat.tv/g2/ipchannels/ipchanman.php"
/*
<?xml version="1.0" ?>
<content_link type="IP_HTML" serviceid="901">http://services.platform.freesat.tv/g2/ipchannels/ipchanman.php? channel=901&amp;programmeid=3589450&amp;ui_id=2.5.2</content_link>
*/

HERROR Send_RC_Freetime(HCHAR* pBody,HUINT32 ulBodyLen,HCHAR *pszUrl)
{
	HERROR	hError = ERR_FAIL;
	HXMLDOC hNewXMLDoc = NULL;
	HXMLELEMENT	hXMLElement = NULL;
	HUINT8	*pszRFreetimeType = NULL,*pszRFreetimeSvcId = NULL,*pszLinkUrl = NULL;
	HCHAR	szTmpBuf[4096] = {0,};
	HCHAR	szAppName[4096] = {0,};

	hError = PAL_XML_InitParser();
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[%s][%d]:	XML parser init was failed \n", __FUNCTION__, __LINE__);
		goto End_Function;
	}

	hError = PAL_XML_ParseBuffer(pBody, ulBodyLen, &hNewXMLDoc);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[%s][%d]:	XML paser was not able to parse the file [%s] \n", __FUNCTION__, __LINE__, pBody);
		goto End_Function;
	}

	hError = PAL_XML_GetRootElement(hNewXMLDoc, &hXMLElement);
	if(hError != ERR_XML_OK)
	{
		HxLOG_Print("[%s][%d]:	XML parser was failed to get ROOT element \n", __FUNCTION__, __LINE__);
		goto End_Function;
	}
	hError = PAL_XML_GetElementContents(hNewXMLDoc, hXMLElement, &pszLinkUrl);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[%s][%d]:	Failed to urlstring [%s]\n", __FUNCTION__, __LINE__, pszLinkUrl);
		goto End_Function;
	}
	printf("pszLinkUrl : %s\n",pszLinkUrl);

	hError = PAL_XML_GetAttrContents(hXMLElement, FSATRC_XMLATTR_FREETIME_TYPE, &pszRFreetimeType); //type : IP_HTML
	if(hError == ERR_OK)
	{
		if(pszRFreetimeType == NULL)
		{
			HxLOG_Print("pszRFreetimeType is NULL\n");
			hError = ERR_FAIL;
			goto End_Function;
		}


		PAL_XML_FreeAttrContents(pszRFreetimeType);
	}
	hError = PAL_XML_GetAttrContents(hXMLElement, FSATRC_XMLATTR_FREETIME_SERVICEID, &pszRFreetimeSvcId); //svcid : decimal
	if(hError == ERR_OK)
	{
		if(pszRFreetimeSvcId == NULL)
		{
			HxLOG_Print("pszRFreetimeSvcId is NULL\n");
			hError = ERR_FAIL;
			goto End_Function;
		}

		HxSTD_snprintf(szAppName,4096,"%s %s","freetime", (HCHAR *)pszRFreetimeSvcId);
		PAL_XML_FreeAttrContents(pszRFreetimeSvcId);
	}

	if(HxSTD_StrNCmp((HCHAR *)pszLinkUrl,(HCHAR *)"http://",7) == 0)
	{
		HxSTD_snprintf(szTmpBuf,4096,"%s",(HCHAR *)pszLinkUrl);
	}
	else
	{
		HxSTD_snprintf(szTmpBuf,4096,"%s%s",APP_URI_FREETIME"?", (HCHAR *)pszLinkUrl);
	}

	HxLOG_Print("szTmpBuf:%s ,szAppName:%s\n",szTmpBuf,szAppName);
	Dial_Run_Application(szTmpBuf,szAppName); //launch Freetime( web으로 url 보냄 )
	HxSTD_snprintf(pszUrl,4096,"%s",szTmpBuf);

End_Function:
	HxLog_Info("[%s][%d]: -\n", __FUNCTION__, __LINE__);

	PAL_XML_FreeAttrContents(pszLinkUrl);


	if(hNewXMLDoc != NULL)
	{
		PAL_XML_FreeDocument(hNewXMLDoc);
	}

	return hError;

}


/********************************************************************
//DIAL API
********************************************************************/
HERROR Dial_Run_Application(HCHAR *pszUrl,HCHAR * pszAppName)
{
	HERROR		hError = ERR_OK;
	printf("[Dial_Run_Application] pszUrl:%s, pszAppName:%s\n",pszUrl,pszAppName);
	MWC_UTIL_PostMsgToAp(MSG_DIAL_APP_LAUNCH_APPLICATION, HANDLE_NULL,
				(HUINT32)pszUrl, (HUINT32)pszAppName, 0);

	return hError;

}
HERROR Dial_Stop_Application(HCHAR *pszUrl,HCHAR * pszAppName)
{
	HERROR		hError = ERR_OK;
	MWC_UTIL_PostMsgToAp(MSG_DIAL_APP_STOP_APPLICATION, HANDLE_NULL,
				(HUINT32)pszUrl, (HUINT32)pszAppName, 0);


	return hError;

}

/********************************************************************
*	function name : local_FSAT_MetaBGMgr_IsOTARunning
*	input :
*	output :
*	description :
********************************************************************/
static HBOOL local_RC_IsOTARunning(void)
{
	HUINT32	i;
	Handle_t mgHandle = HANDLE_NULL;
	HUINT32 swNum = 0;

	swNum = MGR_APPMGR_MODULE_DOWNLOADER_SWUPDATE_GetSwupdateNum();
	for(i = 0 ; i < swNum; i++)
	{
	   mgHandle = MGR_APPMGR_MODULE_DOWNLOADER_SWUPDATE_GetSwupdateHandle(i);
	   HxLOG_Print("\n\n\n++++++++++++  GET MGR HANDLED : 0x%X ++++++++++\n\n",mgHandle);
	   if (MGR_SWUP_IsSwupExec(mgHandle))
	   {
	   		OxSWUP_SOURCE_e	eSource = MGR_SWUP_GetSwupSource(mgHandle);
			if (eSWUP_SOURCE_RF == eSource  || eSWUP_SOURCE_IP == eSource)
			{
				 HxLog_Critical("\n\n\n+++++++++++++  SW UPDATE MOUDLE EXE +++++++++++++++\n\n\n");
				 return TRUE;
			}
	   }
	   else
	   {
			HxLog_Critical("\n\n\n+++++++++++++  SW UPDATE MOUDLE NOT EXE	+++++++++++++++\n\n\n");
	   }
	}

	return FALSE;

}

/********************************************************************
*	function name : local_RC_NoticeCommandToWebApp
*	input :
*	output :
*	description :
********************************************************************/
static HERROR local_RC_NoticeCommandToWebApp(HCHAR *header,HCHAR *body)
{
	HUINT32	ulBufLen =0;
	HUINT8	szMessage[512]={0,}, szParams[512]={0,};
	HUINT8	*pucEvent = NULL;
	const HCHAR	szHeader[512]={0,},szBody[512]={0,};

	HxLog_Info("[%s][%d]: +\n", __FUNCTION__, __LINE__);



	if(header)
	{
		HxJSONBIN_t	enc1 = HxJSONBIN_ENCODER_Open();

		// encode as base 64
		HxJSONBIN_ENCODER_Clear(enc1);
		if (FALSE == HxJSONBIN_ENCODER_SetBinary(enc1, header, strlen(header)))
		{	// fail
			HxLOG_Error("[%s:%d] HxJSONBIN_ENCODER_SetBinary() Fail! \n", __FUNCTION__,__LINE__);
			goto ERROR;
		}
		HxSTD_MemCopy((void *)szHeader,HxJSONBIN_ENCODER_Encode(enc1),512);
		HxJSONBIN_ENCODER_Close(enc1);

	}
	else
	{
		HxSTD_MemCopy((void *)szHeader,"{\"bin\": \"\"}", strlen("{\"bin\": \"\"}")+1);
	}
	if(body)
	{

		HxJSONBIN_t	enc2 = HxJSONBIN_ENCODER_Open();
		HxJSONBIN_ENCODER_Clear(enc2);
		if (FALSE == HxJSONBIN_ENCODER_SetBinary(enc2, body, strlen(body)))
		{	// fail
			HxLOG_Error("[%s:%d] HxJSONBIN_ENCODER_SetBinary() Fail! \n", __FUNCTION__,__LINE__);
			goto ERROR;
		}
		HxSTD_MemCopy((void *)szBody,HxJSONBIN_ENCODER_Encode(enc2),512);
		HxJSONBIN_ENCODER_Close(enc2);
	}
	else
	{
		HxSTD_MemCopy((void *)szBody,"{\"bin\": \"\"}",strlen("{\"bin\": \"\"}")+1);
	}


	// Make linked list for Updated items
	HxSTD_snprintf(szParams, sizeof(szParams),"\"command\":{\"header\":%s,\"body\":%s}", szHeader,szBody);
	HxSTD_snprintf(szMessage, sizeof(szMessage), FSATCOMMAND_EVENT_FORMAT, "rc", szParams);

	ulBufLen = HxSTD_StrLen(szMessage);
	pucEvent = (HUINT8*)HLIB_STD_MemAlloc(ulBufLen+1);
	if(pucEvent == NULL)
		return ERR_FAIL;

	HxSTD_MemSet(pucEvent, 0x00, ulBufLen+1);
	HxSTD_MemCopy(pucEvent, szMessage, ulBufLen);

	BUS_PostMessage(NULL, MSG_BUS_MGR_EVENT, (Handle_t)EVT_OPERATOR_FREESAT_RC_COMMAND, (HINT32)pucEvent, 0, 0);

ERROR:
	HxLog_Info("[%s][%d]: -\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}

#endif


#if 1


#if 0 //not used
STATIC HUINT8 * local_RemoteCtrl_GetEventTypeText(RCEvent_Type eType)
{
	switch (eType)
	{
		ENUM_TO_STR(RCEVT_TYPE_NONE);
		ENUM_TO_STR(RCEVT_TYPE_LOCALE);
		ENUM_TO_STR(RCEVT_TYPE_POWER);
		ENUM_TO_STR(RCEVT_TYPE_STORAGE);
		ENUM_TO_STR(RCEVT_TYPE_PROGRAMME);
		ENUM_TO_STR(RCEVT_TYPE_APP);
		ENUM_TO_STR(RCEVT_TYPE_SETTINGS);
		ENUM_TO_STR(RCEVT_TYPE_SCHEDULE);
		ENUM_TO_STR(RCEVT_TYPE_PLAYBACK);
		ENUM_TO_STR(RCEVT_TYPE_MAX);
		default :
			break;
	}

	return (HUINT8*)NULL;
}
#endif


STATIC HUINT8 * local_RemoteCtrl_GetEventSubTypeText(RCEvent_SubType eSubType)
{

	HUINT32	i = 0;
	for(i = 0;i<RCEVT_MAX;i++)
	{
		if(s_stRC_EventTypes[i].ulRcEventSubType == eSubType)
		{
			break;
		}
	}

	if(RCEVT_MAX == i)
	{

		return NULL;
	}
	else
	{
		return (HUINT8*)s_stRC_EventTypes[i].szEventDesc;
	}
}


STATIC void local_RemoteCtrl_PrintEventLog(void)
{
	HUINT32 i;
	RCEvent_Element *pstRcEvtElem;
//	DATE_TIME_T stDateTime;
	HUINT8		*hStr = NULL;

	if(g_pstRCEvtElements == NULL)
		return;

	for(i = 0; i < MAX_EVENT_LOG_CNT; i++)
	{
		pstRcEvtElem = g_pstRCEvtElements[i];
		if(pstRcEvtElem)
		{
			HxLOG_Print("===============================\n");
			HxLOG_Print("\t Change ID : %d Repeat : %d lParam1 : %d\n", pstRcEvtElem->usChangeID,pstRcEvtElem->ulRepeat,pstRcEvtElem->ulParam1);
			hStr = local_RemoteCtrl_GetEventSubTypeText(pstRcEvtElem->eRCEvtSubType);
			if(hStr == NULL)
				HxLOG_Print("\t Event Type : %d, Event SubType : None \n", pstRcEvtElem->eRCEvtType);
			else
				HxLOG_Print("\t Event Type : %d, Event SubType : %s \n", pstRcEvtElem->eRCEvtType, local_RemoteCtrl_GetEventSubTypeText(pstRcEvtElem->eRCEvtSubType));
#if 0
			UTIL_TCONV_ConvertUnixTimeToDateTime(pstRcEvtElem->ulEvtTime, &stDateTime);
			HxLOG_Print("\t DateTime : (0x%08X) %02d:%02d:%02d %02d-%02d-%04d \n", pstRcEvtElem->ulEvtTime,
								stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond,
								stDateTime.stDate.ucDay, stDateTime.stDate.ucMonth, stDateTime.stDate.usYear);
#endif
		}
	}
}

HERROR RemoteCtrl_AppendEventLogs(RCEvent_Type eType, RCEvent_SubType eSubType,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	RCEvent_Element *pstRcEvtElem;
	UNIXTIME		ulCurTime = (UNIXTIME)HLIB_STD_GetSystemTime();

	if(ulCurTime < UNIX_FIRST_DAY)
		return ERR_FAIL;

	/* Linked List를 사용했다가 Array 형식으로 변경함. Linked List를 사용할 경우 Event 발생시마다 15만번 이상의 비교연산이 발생할 수 있다. */
	if(g_pstRCEvtElements == NULL)
	{
		g_pstRCEvtElements = HLIB_STD_MemAlloc(sizeof(RCEvent_Element *) * MAX_EVENT_LOG_CNT);
		if(g_pstRCEvtElements == NULL)
			return ERR_FAIL;

		HxSTD_MemSet(g_pstRCEvtElements, 0, sizeof(RCEvent_Element *) * MAX_EVENT_LOG_CNT);
	}

	if(g_pstRCEvtElements[g_usElemCnt] == NULL)
	{
		g_pstRCEvtElements[g_usElemCnt] = HLIB_STD_MemAlloc(sizeof(RCEvent_Element));
		if(g_pstRCEvtElements[g_usElemCnt] == NULL)
			return ERR_FAIL;
	}
	HxLOG_Print("[RemoteCtrl_AppendEventLogs] g_usElemCnt : %d\n",g_usElemCnt);
	pstRcEvtElem = g_pstRCEvtElements[g_usElemCnt];

	HxSTD_MemSet(pstRcEvtElem, 0, sizeof(RCEvent_Element));
	pstRcEvtElem->usChangeID = g_usElemCnt;
	pstRcEvtElem->eRCEvtType = eType;
	pstRcEvtElem->eRCEvtSubType = eSubType;
	pstRcEvtElem->ulEvtTime = ulCurTime;
	pstRcEvtElem->ulRepeat	= g_ulRepeat;
	pstRcEvtElem->ulParam1 = ulParam1;
	pstRcEvtElem->ulParam2 = ulParam2;
	pstRcEvtElem->ulParam3 = ulParam3;

	g_usElemCnt = GET_NEXT_CHANGE_ID(g_usElemCnt);

	/* 65535이상 저장될 경우 +1은 초기화 해두자. - 마지막임을 표시 */
	// 65535 이상 저장될 경우 0부터 다시 시작하고, repeat 값을 추가해주자.
//	if(g_pstRCEvtElements[g_usElemCnt] != NULL)
	if(g_usElemCnt == 0 && g_pstRCEvtElements[g_usElemCnt] != NULL)
	{
		HxLOG_Print("g_ulLastReqRepeat : %d  g_ulRepeat : %d , g_usElemCnt:%d\n",g_ulLastReqRepeat,g_ulRepeat,g_usElemCnt);
		g_ulRepeat++; // 추가로 지우고 새로 저장된 것을 의미..
		g_usElemCnt = 0; //다시 처음으로 저장
		HxSTD_MemSet(g_pstRCEvtElements[g_usElemCnt], 0, sizeof(RCEvent_Element)); // 처음 저장할 값을 memset

	}

#if defined(CONFIG_DEBUG)
	local_RemoteCtrl_PrintEventLog();
#endif

	return ERR_OK;
}

RCEvent_Element * RemoteCtrl_GetEventLogByChangeId(HUINT16 ulChangeId)
{
	if(g_pstRCEvtElements != NULL
		&& g_pstRCEvtElements[ulChangeId] != NULL
		&& g_pstRCEvtElements[ulChangeId]->eRCEvtType != RCEVT_TYPE_NONE)
	{
		g_usLastReqId = ulChangeId;
		return g_pstRCEvtElements[ulChangeId];
	}

	return NULL;
}

RCEvent_Element * RemoteCtrl_GetNextEventLog(void)
{
	HUINT16 usReqId;

	usReqId = GET_NEXT_CHANGE_ID(g_usLastReqId);

	return RemoteCtrl_GetEventLogByChangeId(usReqId);
}

HUINT16 RemoteCtrl_GetLastReqChangeId(void)
{
	return g_usLastReqId;
}

void RemoteCtrl_SetLastReqChangeId(HUINT16 ulChangeId)
{
	g_usLastReqId = ulChangeId;
}
void RemoteCtrl_SetLastReqRepeat(HUINT32 ulRepeat)
{
	g_ulLastReqRepeat = ulRepeat;
}

HERROR RemoteCtrl_FreeAllEventLogs(void)
{
	HUINT32 i;
	if(g_pstRCEvtElements != NULL)
	{
		for(i = 0; i < MAX_EVENT_LOG_CNT; i++)
		{
			if(g_pstRCEvtElements[i] != NULL)
				HLIB_STD_MemFree(g_pstRCEvtElements[i]);
		}
		HLIB_STD_MemFree(g_pstRCEvtElements);
		g_pstRCEvtElements = NULL;
	}

	return ERR_OK;
}
#endif

#if 0
HBOOL	RemoteCtrl_IsOTARunning(void)
{
	return local_RC_IsOTARunning();
}
#endif



