/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_epg.h>
#include "_svc_epg_common.h"
#include "_svc_epg_common_lib.h"

#if defined(CONFIG_MW_EPG_TVTV)
#include "_xsvc_epg_tvtv_data.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
//#define CONFIG_XML_DTD



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


const STATIC XsvcEpg_TvtvElement_t		s_astElementTsmt[] =
{
	{FALSE,		TVTV_ELEMENT_DEPTH_ROOT,	TVTV_TYPE_NONE, 		(HUINT8 *)"tsl",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_1,		TVTV_TYPE_NONE,			(HUINT8 *)"ts", 				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"tvtvid", 			TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"oid", 				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"tsid",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"sid",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_HEXA,			(HUINT8 *)"f183FALSE",			TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_HEXA,			(HUINT8 *)"f283FALSE",			TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_HEXA,			(HUINT8 *)"x26",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_HEXA,			(HUINT8 *)"vps",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"sname",			TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"lname",			TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"lang",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"cat",				TVTV_TYPE_NONE,			NULL				},
	{TRUE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"logo",				TVTV_TYPE_STRING,		(HUINT8*)"copyright"	},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"order",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"catype",			TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL64,	(HUINT8 *)"caid",				TVTV_TYPE_NONE,			NULL				}
};


const STATIC XsvcEpg_TvtvElement_t		s_astElementLtot[] =
{
	{FALSE,		TVTV_ELEMENT_DEPTH_ROOT,	TVTV_TYPE_NONE,			(HUINT8 *)"ltol",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_1,		TVTV_TYPE_NONE,			(HUINT8 *)"lto", 				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"coc", 				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"cor", 				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"toc",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"ltobefore",			TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"ltoafter",			TVTV_TYPE_NONE,			NULL				}
};




const STATIC XsvcEpg_TvtvElement_t		s_astElementEpg[] =
{
	{TRUE,		TVTV_ELEMENT_DEPTH_ROOT,	TVTV_TYPE_NONE,			(HUINT8 *)"epg",				TVTV_TYPE_NONE,			(HUINT8 *)"xml:lang"	},
	{FALSE,		TVTV_ELEMENT_DEPTH_1,		TVTV_TYPE_NONE,			(HUINT8 *)"prog", 				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"progid", 			TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"tvtvid", 			TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"start",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"end",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"title",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"ltext",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"sid",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"snr",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"fsk",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"content", 			TVTV_TYPE_NONE,			NULL				},
	{TRUE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"image", 			TVTV_TYPE_STRING,		(HUINT8 *)"copyright"	},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"ver", 				TVTV_TYPE_NONE,			NULL				},
	{TRUE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"link",				TVTV_TYPE_DECIMAL,		(HUINT8 *)"type"		},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"tip", 				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"eventid", 			TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"season", 			TVTV_TYPE_NONE,			NULL				},

	// detail info
	{TRUE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"name",				TVTV_TYPE_DECIMAL,		(HUINT8 *)"type"		},
	{TRUE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"name_ext",			TVTV_TYPE_NONE,			(HUINT8 *)"type"		},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"dtext",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"year",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"country",			TVTV_TYPE_NONE,			NULL				},

	// not used
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"cat", 				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"pil",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_STRING,		(HUINT8 *)"otitle",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"sct",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"audio",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"video",				TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"subtitle",			TVTV_TYPE_NONE,			NULL				},
	{FALSE,		TVTV_ELEMENT_DEPTH_2,		TVTV_TYPE_DECIMAL,		(HUINT8 *)"format",			TVTV_TYPE_NONE,			NULL				}

};



const STATIC XsvcEpg_TvtvElementList_t		s_stElementListTsmt =	{		TVTV_ELEMENT_LIST_TSMT,		TVTV_TSMT_MAX,		(XsvcEpg_TvtvElement_t *)s_astElementTsmt	};
const STATIC XsvcEpg_TvtvElementList_t		s_stElementListLtot =	{		TVTV_ELEMENT_LIST_LTOT,		TVTV_LTOT_MAX,		(XsvcEpg_TvtvElement_t *)s_astElementLtot		};
const STATIC XsvcEpg_TvtvElementList_t		s_stElementListEpg =	{		TVTV_ELEMENT_LIST_EPG,		TVTV_EPG_MAX,		(XsvcEpg_TvtvElement_t *)s_astElementEpg		};

#if 0
#if defined(CONFIG_XML_DTD)
const STATIC HUINT8*				s_pucTsmtDtdStream =
"\
\
\
\
";

const STATIC HUINT8*				s_pucLtotDtdStream =
"\
\
\
\
";

const STATIC HUINT8*				s_pucEpgDtdStream =
"\
\
\
\
";
#endif
#endif

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/







#define	______GLOBAL_FUNC_START______



HERROR			xsvc_epgdata_LoadElementList_Tvtv(HUINT32 ulMode, XsvcEpg_TvtvElementList_t **ppstElementList)
{
	HERROR			hError = ERR_TVTVDATA_FAIL;

	if(ppstElementList == NULL)
	{
		HxLOG_Print("[local_tvtvdata_LoadElementList] Param NULL !!\n");
		goto EXIT_TVTVDATA_FUNC;
	}
	*ppstElementList = NULL;

	switch(ulMode)
	{
		case TVTV_ELEMENT_LIST_TSMT :
			*ppstElementList = (XsvcEpg_TvtvElementList_t *)&s_stElementListTsmt;
			break;

		case TVTV_ELEMENT_LIST_LTOT :
			*ppstElementList = (XsvcEpg_TvtvElementList_t *)&s_stElementListLtot;
			break;

		case TVTV_ELEMENT_LIST_EPG :
			*ppstElementList = (XsvcEpg_TvtvElementList_t *)&s_stElementListEpg;
			break;

		default :
			*ppstElementList = NULL;
			HxLOG_Print("[local_tvtvdata_LoadElementList] invalid mode !!\n");
			goto EXIT_TVTVDATA_FUNC;
	}

	hError = ERR_TVTVDATA_OK;


EXIT_TVTVDATA_FUNC :


	return hError;

}
#endif



/*********************** End of File ******************************/
