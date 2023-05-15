/**
	@file     mgr_emmupdate.h
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/03/27		initiated					chmin\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_NOVASPREAD_H__
#define	__XMGR_NOVASPREAD_H__

#include <mgr_common.h>
#include <mgr_action.h>
/*
	Related Config(s)
	CONFIG_APCORE_EMM_UPDATE : Dependent
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if defined(CONFIG_SUPPORT_NOVA_APP)
#define NOVA_MAX_LICENSE_LENGTH			(1024)
#define NOVA_MAX_UR_LENGTH					(125)

typedef struct
{
	HUINT8 usData[NOVA_MAX_LICENSE_LENGTH];
	HUINT32 ulSize;
} xMgrCas_NovaLicense_t;

typedef struct
{
	HUINT8  usData[NOVA_MAX_UR_LENGTH];
	HUINT32 ulSize;
} xMgrCas_NovaUsageRules_t;

typedef struct
{
	HUINT32 hAction;
	HUINT32 ulTransportSessionId;

	HINT32 usOnId;
	HINT32 usTsId;
	HINT32 usSvcId;

	HUINT16 usAudioPid;
	HUINT16 usVideoPid;
} xMgrCas_TransportSessionInfo_t;

typedef struct
{
	HUINT32 	ulTransportSessionId;
	xMgrCas_NovaLicense_t	stOldLicense;
	HUINT32 	ulCollectionId;
	HUINT32 	ulDuration;
	xMgrCas_NovaUsageRules_t	stUsageRules;

	HUINT8		usRelativity;
	HUINT32		ulRetentionDuration;
	xMgrCas_NovaLicense_t	stNewLicense;
} xMgrCas_NovaDrmSetParameters_t;
#endif

/*******************************************************************/
/********************      Messages             ********************/
/*******************************************************************/

#endif /* __XMGR_NOVASPREAD_H__ */

