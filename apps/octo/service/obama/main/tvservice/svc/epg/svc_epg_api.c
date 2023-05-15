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
#include <octo_common.h>

#include "linkedlist.h"

#include <db_svc.h>
#include <db_param.h>

#include <svc_epg.h>

#include "./local_include/_svc_epg_common.h"
#include "./local_include/_svc_epg_db_raw_lib.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#if defined(CONFIG_DEBUG)
#define	LOG					HxLOG_Print
#define 	INFO_LOG			HxLOG_Info
#define	ERR_LOG			HxLOG_Error
#define 	MSG_LOG			HxLOG_Message
#define	ScartENTER			HxLOG_Info("(+)Enter!!\n");
#define	ScartLEAVE(X)		HxLOG_Info("(-)Leave -- (0x%X)\n",X);
#else
#define	LOG					OCTO_NULL_PRINT
#define 	INFO_LOG			OCTO_NULL_PRINT
#define	ERR_LOG			OCTO_NULL_PRINT
#define 	MSG_LOG			OCTO_NULL_PRINT
#define	ScartENTER			OCTO_NULL_PRINT
#define	ScartLEAVE(X)		OCTO_NULL_PRINT
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


#define ____GLOBAL_FUNC____


// TODO: cmd �� �� �ű��, �� ���� ����.



#if defined(CONFIG_DEBUG)
void			SVC_EPG_PrintEventTree(void)
{
	// TODO: ���� �����ؾ� �˴ϴ�..
}

void			SVC_EPG_PrintEpgResource(void)
{
	// TODO: ���� �����ؾ� �˴ϴ�..
}

void 				SVC_EPG_PrintFindParam(SvcEpg_FindParam_t *pParam)
{
	// TODO: ���� �����ؾ� �˴ϴ�..
}

void 				SVC_EPG_PrintEventStruct(SvcEpg_Event_t *pstEpgEvt)
{
	// TODO: ���� �����ؾ� �˴ϴ�..

}

void 			SVC_EPG_PrintEventsTime(HINT32 type, Handle_t hSvc, UNIXTIME utTime, UNIXTIME utEnd )
{
	// TODO: ���� �����ؾ� �˴ϴ�..
}

void 			SVC_EPG_PrintEventsGenre(HINT32 type, Handle_t hSvc, HUINT32 ulGenre )
{
	// TODO: ���� �����ؾ� �˴ϴ�..
}

void 			SVC_EPG_PrintEventsKeyword(HINT32 type, Handle_t hSvc, HCHAR *pstKeyword )
{
	// TODO: ���� �����ؾ� �˴ϴ�..
}


#endif

/*********************** End of File ******************************/
