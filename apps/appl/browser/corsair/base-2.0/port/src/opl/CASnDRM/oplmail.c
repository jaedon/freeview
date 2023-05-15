/**************************************************************************************/
/**
 * @file oplmail.c
 *
 * Defines the channel structure.
 *
 * @author  KIM SEUNG BUM (sbkim@humaxdigital.com)
 * @date    2012/09/04
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplmail.h"

#include "oapi.h"
#include "apk.h"
#include <hlib.h>


//#define	_DLOG					printf
#define	_DLOG					HxLOG_Debug
#define	ASSERT(expr)			HxLOG_Assert(expr)
#define	TRACE()					do{_DLOG("OPLFAV::TRACE [%s:%d]\n", __FUNCTION__, __LINE__);}while(0)
#define	RETURN_IF_VOID(expr)	do{if(expr){_DLOG("OPLFAV::[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return;}}while(0)
#define	RETURN_IF(expr, err)	do{if(expr){_DLOG("OPLFAV::[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);return (err);}}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr){_DLOG("OPLFAV::[%s:%d] %s failed!!\n", __FUNCTION__, __HxLINE__, #expr);goto pos;}}while(0)

typedef struct
{
	HxVector_t	*list;		/// [ DxMail_t * ]
	HxTREE_t		*map;	/// [ int ulId, DxMail_t * ]
} MailManager_t;

enum
{
	eNetwork_DTT =0,
	eNetwork_BS,
	eNetwork_CATV,
	eNetwork_CS1,
	eNetwork_CS2,
	eNetwork_JCHITS,
	eNetwork_SELF,
	eNetwork_MAX
};

static HCHAR szNetworkString[eNetwork_MAX][10] = {"DTT", "BS", "CATV", "CS1", "CS2", "JC-HITS", "SELF"};

static HINT32	opl_mail_CompareHINT32 (const void *arg1, const void *arg2)
{
	return (HINT32)arg1 - (HINT32)arg2;
}

static HERROR opl_mail_LoadList (MailManager_t *manager)
{
	HxVector_t		*list;
	HxTREE_t			*uidMap;
	DxMail_t			*mail;
	HERROR			hErr = ERR_FAIL;

	if (manager && manager->list)
		return ERR_OK;

	HxLOG_Trace();

	hErr = APK_META_MAIL_LoadFromOcto();
	if( hErr != ERR_OK )
	{
		HxLOG_Error("[%s,%d]Load mail data\n", __FUNCTION__, __LINE__);
		return hErr;
	}

	list = APK_META_MAIL_GetMailList();
	if (list == NULL)
	{
		HxLOG_Error("[%s,%d]Cannot get channel list from appkit\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	uidMap = HLIB_TREE_New(opl_mail_CompareHINT32);
	if (uidMap == NULL)
	{
		HLIB_VECTOR_Delete(list);
		HxLOG_Error("[%s,%d]Out of memory!!!\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	HxVECTOR_EACH(list, DxMail_t*, mail, {
		HLIB_TREE_Insert(uidMap, (void *)mail->ulId, (void *)mail);
	});

	if (manager->list)
		HLIB_VECTOR_Delete(manager->list);
	if (manager->map)
		HLIB_TREE_Destroy(manager->map);

	manager->list = list;
	manager->map  = uidMap;
	return ERR_OK;
}

static void opl_mail_ReleaseList (MailManager_t *manager)
{
	if( manager )
	{
		if (manager->list)
			HLIB_VECTOR_Delete(manager->list);
		if (manager->map)
			HLIB_TREE_Destroy(manager->map);
	}
	else
	{
		HxLOG_Error("[%s,%d]Argumetn ( manager) is NULL \n", __FUNCTION__, __LINE__);
	}
}

void			OPL_Mail_Delete (OPL_MailHandle mail)
{
	APK_META_MAIL_Delete((void *)mail);
}

HERROR		OPL_Mail_Remove( HINT32 msgId )
{
	return APK_META_MAIL_Remove( msgId );
}

HERROR		OPL_Mail_UpdateReadDoneFlag( OPL_MailHandle self )
{
	DxMail_t		*thiz = (DxMail_t *)self;
	return APK_META_MAIL_Update( thiz );
}

int		OPL_Mail_GetMailListByType ( OPL_MailHandle **mailList, OPL_MailType_e type)
{
	DxMail_t			*mail =NULL;
	DxMail_t			**tempList =NULL;
	OPL_MailHandle		*list =NULL;
	int				i =0, n =0, count =0;
	MailManager_t		manager;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();
	HxLOG_Assert(mailList);

	memset( &manager, 0x00, sizeof( MailManager_t ));
	hErr = opl_mail_LoadList( &manager );
	if( hErr != ERR_OK )
	{
		HxLOG_Error("[%s,%d]load mail list error!\n", __FUNCTION__, __LINE__);
		return 0;
	}

	if (manager.list == NULL
		|| (n = HLIB_VECTOR_Length(manager.list)) == 0)
	{
		HxLOG_Error("[%s,%d]Channel manager does not initialized!\n", __FUNCTION__, __LINE__);
		return 0;
	}
	HxLOG_Print("[%s,%d] count : %d \n", __FUNCTION__ , __LINE__,n );

	tempList = (DxMail_t **)HLIB_STD_MemAlloc(sizeof(DxMail_t*) * n);
	if (tempList == NULL)
	{
		HxLOG_Error("[%s,%d]Out of memory!!\n", __FUNCTION__, __LINE__);
		return 0;
	}

	list = (OPL_MailHandle *)tempList;

	for (i = 0, count = 0 ; i < n ; i++)
	{
		mail = (DxMail_t *)HLIB_VECTOR_ItemAt(manager.list, i);
		if (mail == NULL)
			continue;

		if( type == eOPL_MailType_Self )
		{
			switch( mail->eMailType )
			{
				case eOPL_MailCheckType_DOWNLOAD_INFO:
				case eOPL_MailCheckType_PASSWORD_DELETED:
				case eOPL_MailCheckType_RESERVE_FAIL:
				case eOPL_MailCheckType_RESCAN:
				case eOPL_MailCheckType_REC_FAIL:
					/* do nothing check 데이타 */
					break;

				case eOPL_MailCheckType_BS:
				case eOPL_MailCheckType_DT:
				case eOPL_MailCheckType_REMUX:
				case eOPL_MailCheckType_JCHITS:
				case eOPL_MailCheckType_CS1:
				case eOPL_MailCheckType_CS2:
				default:
					/* 처리하면 안된다. 외부에서 들어오는 메일 */
					continue;
					break;
			}
		}
		else if( type == eOPL_MailType_All )
		{
			/* all pass do nothing do to */
		}
		else if( type == eOPL_MailType_Emm )
		{
			switch( mail->eMailType )
			{
				case eOPL_MailCheckType_BS:
				case eOPL_MailCheckType_DT:
				case eOPL_MailCheckType_REMUX:
				case eOPL_MailCheckType_JCHITS:
				case eOPL_MailCheckType_CS1:
				case eOPL_MailCheckType_CS2:
					/* do nothing check 데이타 */
					break;

				case eOPL_MailCheckType_DOWNLOAD_INFO:
				case eOPL_MailCheckType_PASSWORD_DELETED:
				case eOPL_MailCheckType_RESERVE_FAIL:
				case eOPL_MailCheckType_RESCAN:
				case eOPL_MailCheckType_REC_FAIL:
				default:
					/* 처리하면 안된다. 내부 발행 메일 */
					continue;
					break;
			}
		}
		else
		{
			/* error break */
			HxLOG_Error("[%s,%d]Error Invalide mail type : %d \n", __FUNCTION__, __LINE__, type);
			break;
		}

		HxLOG_Print("[%s,%d] msgid : %d \n", __FUNCTION__ , __LINE__, mail->ulId);
		HxLOG_Print("[%s,%d] type : %d \n", __FUNCTION__ , __LINE__, mail->eMailType);
		HxLOG_Print("[%s,%d] time : %s \n", __FUNCTION__ , __LINE__, mail->ucSendTime);
		HxLOG_Print("[%s,%d] title : %s \n", __FUNCTION__ , __LINE__, mail->ucMailTitle);
		HxLOG_Print("[%s,%d] text : %s \n", __FUNCTION__ , __LINE__, mail->ucMailText);

		list[count] = (OPL_MailHandle)APK_META_MAIL_Clone(mail);

		if (list[count])
			count++;
	}

	if (count == 0)
	{
		HLIB_STD_MemFree(list);
		list = NULL;
	}

	opl_mail_ReleaseList( &manager );

	*mailList = list;

	HxLOG_Print("[%s,%d] count : %d \n", __FUNCTION__ , __LINE__ , count );
	return count;
}

HINT32		OPL_Mail_GetMsgId ( OPL_MailHandle self )
{
	DxMail_t		*thiz = (DxMail_t *)self;

	HxLOG_Trace();
	RETURN_IF(!self, -1);

	return thiz->ulId;
}

HINT32		OPL_Mail_GetMsgType ( OPL_MailHandle self )
{
	DxMail_t		*thiz = (DxMail_t *)self;
	HINT32		nMsgType = -1;

	HxLOG_Trace();
	RETURN_IF(!self, -1);

	switch( thiz->eMailType )
	{
		case eOPL_MailCheckType_BS:
		case eOPL_MailCheckType_DT:
		case eOPL_MailCheckType_REMUX:
		case eOPL_MailCheckType_JCHITS:
		case eOPL_MailCheckType_CS1:
		case eOPL_MailCheckType_CS2:
			nMsgType = 0; /* emm meesage , JLABS_EMM_MESSAGE_EMM */
			break;

		case eOPL_MailCheckType_DOWNLOAD_INFO:
		case eOPL_MailCheckType_PASSWORD_DELETED:
		case eOPL_MailCheckType_RESERVE_FAIL:
		case eOPL_MailCheckType_RESCAN:
		case eOPL_MailCheckType_REC_FAIL:
			nMsgType = 1; /* emm meesage , JLABS_EMM_MESSAGE_SELF */
			break;

		default:
			HxLOG_Error("[%s,%d]Error Invalide mail type : %d \n", __FUNCTION__, __LINE__, thiz->eMailType);
			break;
	}

	return nMsgType;
}

HCHAR*		OPL_Mail_GetNetworkTypeName ( OPL_MailHandle self )
{
	DxMail_t		*thiz = (DxMail_t *)self;
	HCHAR*		pstNetTypeName = NULL;

	HxLOG_Trace();
	RETURN_IF(!self, NULL);

	switch( thiz->eMailType )
	{
		case eOPL_MailCheckType_DT:
			pstNetTypeName = szNetworkString[eNetwork_DTT]; /* "DTT" */
			break;

		case eOPL_MailCheckType_BS:
			pstNetTypeName = szNetworkString[eNetwork_BS]; /* "BS" */
			break;

		case eOPL_MailCheckType_REMUX:
			pstNetTypeName = szNetworkString[eNetwork_CATV]; /* "CATV" */
			break;

		case eOPL_MailCheckType_CS1:
			pstNetTypeName = szNetworkString[eNetwork_CS1]; /* "CS1" */
			break;

		case eOPL_MailCheckType_CS2:
			pstNetTypeName = szNetworkString[eNetwork_CS2]; /* "CS2" */
			break;

		case eOPL_MailCheckType_JCHITS:
			pstNetTypeName = szNetworkString[eNetwork_JCHITS]; /* "JCHITS" */
			break;

		case eOPL_MailCheckType_DOWNLOAD_INFO:
		case eOPL_MailCheckType_PASSWORD_DELETED:
		case eOPL_MailCheckType_RESERVE_FAIL:
		case eOPL_MailCheckType_RESCAN:
		case eOPL_MailCheckType_REC_FAIL:
			pstNetTypeName = szNetworkString[eNetwork_SELF]; /* "SELF" */
			break;

		default:
			HxLOG_Error("[%s,%d]Error Invalide mail type : %d \n", __FUNCTION__, __LINE__, thiz->eMailType);
			break;
	}

	return pstNetTypeName;
}

HCHAR*		OPL_Mail_GetDate ( OPL_MailHandle self )
{
	DxMail_t		*thiz = (DxMail_t *)self;

	HxLOG_Trace();
	RETURN_IF(!self, NULL);

	return (HCHAR *)thiz->ucSendTime;
}

HCHAR*		OPL_Mail_GetMsgBody ( OPL_MailHandle self )
{
	DxMail_t		*thiz = (DxMail_t *)self;

	HxLOG_Trace();
	RETURN_IF(!self, NULL);

	return (HCHAR *)thiz->ucMailText;
}

HBOOL		OPL_Mail_GetReadDoneFlag ( OPL_MailHandle self )
{
	DxMail_t		*thiz = (DxMail_t *)self;

	HxLOG_Trace();
	RETURN_IF(!self, FALSE);

	return thiz->bIsOpen;
}

HCHAR*		OPL_Mail_GetMsgTitle ( OPL_MailHandle self )
{
	DxMail_t		*thiz = (DxMail_t *)self;

	HxLOG_Trace();
	RETURN_IF(!self, NULL);

	return (HCHAR *)thiz->ucMailTitle;
}

HINT32		OPL_Mail_GetDummyBoardMsgList(OPL_MailHandle **mailList)
{
	DxMail_t		**tempList =NULL;
	DxMail_t		*msg = NULL;
	int			i = 0, n = 2;
	if(mailList == NULL)
	{
		HxLOG_Error("[%s,%d]mailList is NULL!!\n", __FUNCTION__, __LINE__);
		return 0;
	}
	tempList = (DxMail_t **)HLIB_STD_MemAlloc(sizeof(DxMail_t*) * n);
	if (tempList == NULL)
	{
		HxLOG_Error("[%s,%d]Out of memory!!\n", __FUNCTION__, __LINE__);
		*mailList = NULL;
		return 0;
	}
	HxSTD_MemSet(tempList, 0x00, sizeof(DxMail_t*) * n);
	for(i =0; i < n; i++)
	{
		tempList[i] = (DxMail_t*)HLIB_STD_MemAlloc(sizeof(DxMail_t));
		if(tempList[i] != NULL) {
			HxSTD_MemSet(tempList[i], 0, sizeof(DxMail_t));
		}

		msg = tempList[i];
		msg->nNetworkType = eNetwork_CS1+i;
		msg->eMailType = eOPL_MailCheckType_CS1+i;
	}
	*mailList = (OPL_MailHandle *)tempList;
	return n;
}

void			OPL_Mail_DeleteDummyBoardMsg (OPL_MailHandle mail)
{
	if(mail != NULL)
	{
		HLIB_STD_MemFree((void *)mail);
	}
}


