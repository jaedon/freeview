/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__TVTVDATA_H__
#define	__TVTVDATA_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



typedef	enum
{
	ERR_TVTVDATA_OK 							= 0,
	ERR_TVTVDATA_FAIL

}XsvcEpg_TvtvDataErr_e;



typedef	enum
{
	TVTV_ELEMENT_LIST_TSMT 					= 0,
	TVTV_ELEMENT_LIST_LTOT,
	TVTV_ELEMENT_LIST_EPG

}XsvcEpg_TvtvDataListType_e;


typedef	enum
{
	TVTV_TYPE_NONE 							= 0,
	TVTV_TYPE_STRING,
	TVTV_TYPE_DECIMAL,
	TVTV_TYPE_DECIMAL64,
	TVTV_TYPE_HEXA

}XsvcEpg_TvtvElementDataType_e;



typedef	enum
{
	TVTV_ELEMENT_DEPTH_ROOT 					= 0,
	TVTV_ELEMENT_DEPTH_1,
	TVTV_ELEMENT_DEPTH_2,
	TVTV_ELEMENT_DEPTH_3,
	TVTV_ELEMENT_DEPTH_4,
	TVTV_ELEMENT_DEPTH_5,
	TVTV_ELEMENT_DEPTH_6,
	TVTV_ELEMENT_DEPTH_7,
	TVTV_ELEMENT_DEPTH_8
}XsvcEpg_TvtvElementDepth_e;



typedef enum
{
	TVTV_TSMT_TS = 1,
	TVTV_TSMT_TVTVID,
	TVTV_TSMT_OID,
	TVTV_TSMT_TSID,
	TVTV_TSMT_SID,
	TVTV_TSMT_F1830,
	TVTV_TSMT_F2830,
	TVTV_TSMT_X26,
	TVTV_TSMT_VPS,
	TVTV_TSMT_SNAME,
	TVTV_TSMT_LNAME,
	TVTV_TSMT_LANG,
	TVTV_TSMT_CAT,
	TVTV_TSMT_LOGO,
	TVTV_TSMT_ORDER,
	TVTV_TSMT_CATYPE,
	TVTV_TSMT_CAID,
	TVTV_TSMT_MAX
}XsvcEpg_TvtvTsmt_e;



typedef enum
{
	TVTV_LTOT_LTO = 1,
	TVTV_LTOT_COC,
	TVTV_LTOT_COR,
	TVTV_LTOT_TOC,
	TVTV_LTOT_LTOBEFORE,
	TVTV_LTOT_LTOAFTER,
	TVTV_LTOT_MAX
} XsvcEpg_TvtvLtot_e;



typedef enum
{
	TVTV_EPG_EPG = 0,
	TVTV_EPG_PROG,
	TVTV_EPG_PROGID,
	TVTV_EPG_TVTVID,
	TVTV_EPG_START,
	TVTV_EPG_END,
	TVTV_EPG_TITLE,
	TVTV_EPG_LTEXT,
	TVTV_EPG_SID,
	TVTV_EPG_SNR,
	TVTV_EPG_FSK,
	TVTV_EPG_CONTENT,
	TVTV_EPG_IMAGE,
	TVTV_EPG_VER,
	TVTV_EPG_LINK,
	TVTV_EPG_TIP,
	TVTV_EPG_EVENTID,
	TVTV_EPG_SEASON,

	// detail info
	TVTV_EPG_NAME,
	TVTV_EPG_NAME_EXT,
	TVTV_EPG_DTEXT,
	TVTV_EPG_YEAR,
	TVTV_EPG_COUNTRY,

	// not used
	TVTV_EPG_CAT,
	TVTV_EPG_PIL,
	TVTV_EPG_OTITLE,
	TVTV_EPG_SCT,
	TVTV_EPG_AUDIO,
	TVTV_EPG_VIDEO,
	TVTV_EPG_SUBTITLE,
	TVTV_EPG_FORMAT,

	TVTV_EPG_MAX
} XsvcEpg_TvtvEpg_e;



typedef struct
{
	HBOOL				bFlagAttr;
	HUINT32				ulDepth;

	HUINT32				ulTagDataType;
	HUINT8				*pucTagName;

	HUINT32				ulAttrDataType;
	HUINT8				*pucAttrName;
} XsvcEpg_TvtvElement_t;


typedef struct
{
	HUINT32					ulListType;
	HUINT32					ulElementNum;
	XsvcEpg_TvtvElement_t			*pstElement;
} XsvcEpg_TvtvElementList_t;



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/* Public Function 마다 필히 기재하도록 함 */
/**
  @brief     LAYER_MODULE_DoSomething 함수.

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/
HERROR			xsvc_epgdata_LoadElementList_Tvtv(HUINT32 ulMode, XsvcEpg_TvtvElementList_t **ppstElementList);


#endif /* !__TVTVDATA_H__ */
