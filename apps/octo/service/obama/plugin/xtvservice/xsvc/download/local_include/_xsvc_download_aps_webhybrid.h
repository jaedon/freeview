/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__DOWNLOAD_APS_WEBHYBRID_H__
#define	__DOWNLOAD_APS_WEBHYBRID_H__

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

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR xsvc_download_InitAction_Apsweb (void **ppvAction, void **ppvData, void *pvSiMsg);
HERROR xsvc_download_TermAction_Apsweb (void **ppvAction, void **ppvData);

HERROR xsvc_download_GetData_Apsweb (void *pvData, SvcDownload_DataType_e eDataType, HUINT32 *pulDataNum, void **ppvDataArray);
HERROR xsvc_download_FreeData_Apsweb (SvcDownload_DataType_e eDataType, HUINT32 ulDataNum, void *pvDataArray);

#endif /* !__DOWNLOAD_APS_WEBHYBRID_H__ */

