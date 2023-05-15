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

#ifndef	__TVTVLIB_H__
#define	__TVTVLIB_H__



/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#if defined(CONFIG_MW_EPG_TVTV_NAND_STORAGE)
#define EPG_FS_HD1								CONFIG_PRODUCT_USERDATA_DIR"/tvtv_info/"
#define EPG_FS_TVTV_DATA_PATH					CONFIG_PRODUCT_USERDATA_DIR"/tvtv_info/"
// 아래 나머지는 삭제하자.
#define EPG_FS_TVTV_IMAGE_PATH					CONFIG_PRODUCT_USERDATA_DIR"/TVTV/IMAGE/"
#define EPG_FS_TVTV_PATH						CONFIG_PRODUCT_USERDATA_DIR"/TVTV/"
#define EPG_FS_TVTV_OLD_PATH					CONFIG_PRODUCT_USERDATA_DIR"/TVTV_OLD/"
#define EPG_FS_TVTV_OLD_DATA_PATH				CONFIG_PRODUCT_USERDATA_DIR"/TVTV_OLD/DATA/"
#else
#define EPG_FS_HD1								"/mnt/hd1/tvtv_info/"
#define EPG_FS_TVTV_DATA_PATH					"/mnt/hd1/tvtv_info/"
// 아래 나머지는 삭제하자.
#define EPG_FS_TVTV_IMAGE_PATH					"/mnt/hd1/TVTV/IMAGE/"
#define EPG_FS_TVTV_PATH							"/mnt/hd1/TVTV/"
#define EPG_FS_TVTV_OLD_PATH						"/mnt/hd1/TVTV_OLD/"
#define EPG_FS_TVTV_OLD_DATA_PATH				"/mnt/hd1/TVTV_OLD/DATA/"
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef	enum
{
	ERR_TVTV_OK 					= 0,
	ERR_TVTV_FAIL,

}xsvcEpg_TvtvError_e;


typedef enum
{
	TVTV_FileType_TVST				= 0,
	TVTV_FileType_DGKT,
	TVTV_FileType_LTOT,
	TVTV_FileType_TSMT,
	TVTV_FileType_LOGO,
	TVTV_FileType_EPG_EVENT,
	TVTV_FileType_EPG_TABLE,
	TVTV_FileType_EPG_IMAGE,
	TVTV_FileType_EPG_DSI,
	TVTV_FileType_MAX

}xsvcEpg_TvtvFileType_e;


typedef struct tagPostprocFileInfo
{
	xsvcEpg_TvtvFileType_e					eFileType;
	HUINT32							ulFileSize;
	HUINT8							*pucFileName;
	HUINT8							*pucFilePointer;
	HINT32							lWidth;
	HINT32							lHeight;
#if	1
	HUINT32							ulTableId;
	HUINT16							usFileIndex;
#endif
	struct tagPostprocFileInfo			*pNext;
}xsvcEpg_PostprocFile_t;




/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR		xsvc_epglib_InitBuffer_Tvtv(void);
HERROR		xsvc_epglib_CreateBuffer_Tvtv(void);
HERROR		xsvc_epglib_CreateEpgInfo_Tvtv(HBOOL bNeedConverting);
HERROR		xsvc_epglib_CreateLogoImage_Tvtv(HBOOL bNeedConverting);
HERROR		xsvc_epglib_CreateTsmtInfo_Tvtv(HBOOL bNeedConverting);
HERROR		xsvc_epglib_DestroyTsmtInfo_Tvtv(POINTER_LIST_T *pTsmtList);
HERROR		xsvc_epglib_CreateLtotInfo_Tvtv(HBOOL bNeedConverting);
HERROR		xsvc_epglib_CreateTvtvSystemInfo_Tvtv(HBOOL bNeedConverting);
HERROR		xsvc_epglib_CreateTvstInfo_Tvtv(void);
HERROR		xsvc_epglib_CreateDgktInfo_Tvtv(HBOOL bNeedConverting);


HERROR		xsvc_epglib_GetTvtvFileType_Tvtv(HUINT8 *pucFileName, xsvcEpg_TvtvFileType_e *pTvtvFileType);
HERROR		xsvc_epglib_SetTvstInfo_Tvtv(HUINT8 *pucTvst, HUINT32 ulSize);
HERROR		xsvc_epglib_SetTvtvSystemInfo_Tvtv(void);
HERROR		xsvc_epglib_SetDgktInfo_Tvtv(HUINT8 *pucDgkt, HUINT32 ulSize, HBOOL bNeedConverting);
HERROR		xsvc_epglib_Postprocess_Tvtv(PalDsmcc_File_t *pDsmccFile, xsvcEpg_TvtvFileType_e eFileType, HBOOL bNeedConverting, xsvcEpg_PostprocFile_t **ppPostprocessedFile);
HERROR		xsvc_epglib_ParseTsmt_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppTsmtList);
HERROR		xsvc_epglib_ParseEpg_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppEpgList, HUINT8 *pszLang);
HERROR		xsvc_epglib_ParseLtot_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppLtotList);

#if 1//defined(TVTV_NEW_ONE)
HERROR		xsvc_epglib_ParseToHumaxServiceList_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize);
#else
HERROR		xsvc_epglib_ParseToHumaxServiceList_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppTvtvMapList);
#endif
HERROR		xsvc_epglib_ParseParseToHumaxExtEvent_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HUINT32 ulProgId, POINTER_LIST_T **ppExtEventList);
#if 0//defined(TVTV_NEW_ONE)
HERROR		xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize);
#else
HERROR		xsvc_epglib_ParseToHumaxOnlyEventList_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppEventList);
#endif
HERROR		xsvc_epglib_ParseToHumaxEventList_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, POINTER_LIST_T **ppEventList, POINTER_LIST_T **ppExtList);
HERROR		xsvc_epglib_GetFileStartTime_Tvtv(HUINT8 *pRawBuffer, HUINT32 ulBufferSize, HUINT32 *pulStartTime);

HERROR		xsvc_epglib_DestroyLtotInfo_Tvtv(void);
HERROR		xsvc_epglib_DestroyTvstInfo_Tvtv(void);
HERROR		xsvc_epglib_DestroyDgktInfo_Tvtv(void);

HERROR		xsvc_epglib_SetDataByType_Tvtv(	HUINT32 ulType,
													HUINT8 *pucSrc,
													HUINT8 *pucLandCode,
													HUINT32 ulLandCodeLen,
													HUINT32 ulMaxLen,
													HUINT8 *pszString,
													HUINT32 *pulDecimal);

#endif /* !__TEMPLATE_H__ */
