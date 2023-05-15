/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_live.h
	@brief

	Description: main main procedure ���� ������� �з��� ����̴�. \n
	Module:
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_LIVE_H__
#define	__XMGR_LIVE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <isosvc.h>
#include <stb_params.h>
#include <linkedlist.h>

/*******************************************************************/
// MW
/*******************************************************************/
//#include <svc_epg.h>
/*******************************************************************/
// DB
/*******************************************************************/

/*******************************************************************/
// MGR
/*******************************************************************/
//

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eLiveModuleType_None,

	eLiveModuleType_Base,
	eLiveModuleType_Japan,

	eLiveModuleType_EndOfCase
} XmgrLive_ModuleType_e;

typedef struct
{
	Handle_t					 hMasterSvc;
	Handle_t					 hSuppleSvc;
} XmgrLive_LiveType_InArg_t;

typedef struct
{
	XmgrLive_ModuleType_e		 eViewModule;
	XmgrLive_ModuleType_e		 eLockModule;
} XmgrLive_LiveType_t;

typedef enum
{
	eAribEventRelayMode_DIALOG,		/* Event relay �ÿ� dialog�� ��� user confirm�� �޴´�. �⺻ ���� ���� 0 ���� ������ ��. */
	eAribEventRelayMode_FORCE,		/* Event relay �ÿ� dialog�� ����� �ʰ� ������ ����. */
	eAribEventRelayMode_OFF		/* Event relay ���� ����. */
} XmgrZap_AribEventRelayMode_t;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/*	Zapping Functions
 */

#endif	/*	!__XMGR_LIVE_H__	*/
