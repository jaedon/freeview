/**
	@file     xmgr_zapping.c
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/**
  * Character encoding.
  *
  * MS949
  *
  * This source file that uses MS949 encoding.
  * MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
  * MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
  * Quelldatei, die MS949-Codierung verwendet.
  *
**/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_svc.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <svc_av.h>
#include <svc_transcoder.h>

#include <mgr_svclist.h>
#include <mgr_live.h>
#include "../local_include/_xmgr_live_zap.h"
#include "../local_include/_xmgr_live.h"

#define ___LOCAL_DEFINITIONS___

#define ___LOCAL_DATATYPES___

#define ___LOCAL_VARIABLES___

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___

#define ___API_GLOBAL_FUNCTIONS___

void XMGR_ZAP_Init_Base(void)
{
	MGR_LIVE_ResetZappingInfo(TRUE);
}


/* EOF */
