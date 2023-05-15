/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_common.h
	@brief

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�. 			\n
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

	/* - Description : Dynamic update, channel list edit������ ���Ͽ� service DB�� �ٲ� ��� notify�Ͽ�
					service handle�� ������� application���� ���� üũ�ϵ��� �ϴ� �̺�Ʈ �޽���.
	   - Parameters Usage :
		   handle : TBD
		   p1 : DbCheckType_e
		   p2 : ���� Ʃ�� �ϱ� ���� (Handle_t)service handle, HANDLE_NULL�� ��� �޴� �ʿ��� ó����. [����] 0 ���� ���� �ȵȴ�
		   p3 : TBD */
	eSEVT_DB_CHECK_SVC_DB = eSEVT_DB_START,

	// pmt updated �� ... ���� EVT_DB_CHECK_SVC_DB�� �ʹ� ���� ���ǰ� �־, �׳� �ϳ� �� ����~
	eSEVT_DB_NOTIFY_UPDATED,


	 //DAMA�� ���� Lock ������ �ٲ������ �ö���� �޽���
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

