/**
	@file     file_name.c File Name�� �����ϴ�.
	@brief    file_name & simple comment.

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
/** @brief global header file  */
#include <stdio.h>
#include <string.h>
#include <octo_common.h>

/** @brief local header file  */
#include "../local_include/_xsvc_download.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR xsvc_download_Init_FoxsatHdr2(void)
{
#if defined(CONFIG_MW_DOWNLOAD)
	HERROR			hErr;

	hErr = xsvc_download_InitDvbssu();
	hErr |= xsvc_download_InitDsmcc_Hbbtv();
	hErr |= xsvc_download_InitDsmcc_Fsat();

	return hErr;
#else
	return ERR_FAIL;
#endif
}


/*********************** End of File ******************************/
