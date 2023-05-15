/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_common.h
	@brief

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다. 			\n
	Remarks :										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__DB_COMMON_H__
#define	__DB_COMMON_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <unistd.h>
#include <octo_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define DB_JFFS2_PATH				OBAMA_DIR_DATA "/"

#define	OCTO_NVRAM_BASE				DB_JFFS2_PATH
#define	OCTO_NVRAM_BACKUP			OBAMA_DIR_BACKUPDATA
#define	OCTO_NVRAM_USER				OBAMA_DIR_USERDATA

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/** @brief DB event messages */
enum
{

	eSEVT_DB_ = eSEVT_DB_START,

	/* - Description : Dynamic update, channel list edit등으로 인하여 service DB가 바뀐 경우 notify하여
					service handle을 사용중인 application들이 각자 체크하도록 하는 이벤트 메시지.
	   - Parameters Usage :
		   handle : TBD
		   p1 : DbCheckType_e
		   p2 : 강제 튜닝 하기 위한 (Handle_t)service handle, HANDLE_NULL인 경우 받는 쪽에서 처리함. [주의] 0 값을 쓰면 안된다
		   p3 : TBD */
	eSEVT_DB_CHECK_SVC_DB = eSEVT_DB_START,

	// pmt updated 등 ... 위의 EVT_DB_CHECK_SVC_DB는 너무 많이 사용되고 있어서, 그냥 하나 더 만듦~
	eSEVT_DB_NOTIFY_UPDATED,


	 //DAMA로 부터 Lock 정보가 바뀌었을때 올라오는 메시지
	eSEVT_DB_SVCLIST_CHECK_LOCK,

	eSEVT_DB_PARAM_UPDATED,

	eSEVT_DB_PARAM_CHECK_ALL_REMOVED,

	eSEVT_DB_SVCLIST_CHANGED_BY_DAMA,

	eSEVT_DB_INITIALIZED,

	eSEVT_DB_END
};

typedef enum
{
	eCheckDbType_Nothing			= 0,
	eCheckDbType_SvcInfoUpdated		= 0x0001,
	eCheckDbType_SvcNameUpdated		= 0x0002,
	eCheckDbType_SvcAdded			= 0x0004,
	eCheckDbType_SvcDeleted			= 0x0008,

	eCheckDbType_ALL				= 0xFFFF
} DbCheckType_e;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR	DB_Init(PostMsgToMgrCb_t fnNotify);



#endif	/* __DB_COMMON_H__ */

