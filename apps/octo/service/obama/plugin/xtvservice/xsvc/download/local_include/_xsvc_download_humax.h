/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name�� �����ϴ�.
	@brief	  file_name & simple comment.

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__DOWNLOAD_HUMAX_H__
#define	__DOWNLOAD_HUMAX_H__

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

HERROR xsvc_download_InitAction_Humax (void **ppvAction, void **ppvData, void *pvSiMsg);
HERROR xsvc_download_TermAction_Humax (void **ppvAction, void **ppvData);
HERROR xsvc_download_CheckDownloadComplete_Humax(HUINT32 ulSignalVersion, HBOOL *pbComplete);
HERROR xsvc_download_ClearDownloadInfo_Humax(void);
HERROR xsvc_download_InitMetaInfo_Humax(void);

#endif /* !__DOWNLOAD_HUMAX_H__ */
