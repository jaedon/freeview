/**************************************************************************************/
/**
 * @file oplmail.h
 *
 * Defines the mail structure.
 *
 * @author  KIM SEUNG BUM(sbkim@humaxdigital.com)
 * @date    2012/09/04
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLMAIL_H__
#define __OPLMAIL_H__

#include	"opltype.h"
#include 	"bpltype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *	OPL_MailHandle;

typedef enum
{
	eOPL_MailCheckType_BS,
	eOPL_MailCheckType_DT,
	eOPL_MailCheckType_REMUX,
	eOPL_MailCheckType_JCHITS,
	eOPL_MailCheckType_CS1,
	eOPL_MailCheckType_CS2,
	eOPL_MailCheckType_DOWNLOAD_INFO,
	eOPL_MailCheckType_PASSWORD_DELETED,
	eOPL_MailCheckType_RESERVE_FAIL,
	eOPL_MailCheckType_RESCAN,
	eOPL_MailCheckType_REC_FAIL,
} OPL_MailCheckType_e;

typedef enum
{
	eOPL_MailType_All =0,
	eOPL_MailType_Self =1,
	eOPL_MailType_Board =2,
	eOPL_MailType_Emm =3,
	eOPL_MailType_Max,
}OPL_MailType_e;

/******************************************************************
	Define
******************************************************************/

void			OPL_Mail_Delete (OPL_MailHandle mail);
HERROR		OPL_Mail_Remove( HINT32 msgId );
HERROR		OPL_Mail_UpdateReadDoneFlag( OPL_MailHandle self );
int		OPL_Mail_GetMailListByType ( OPL_MailHandle **mailList, OPL_MailType_e type);

HINT32		OPL_Mail_GetMsgId ( OPL_MailHandle self );
HINT32		OPL_Mail_GetMsgType ( OPL_MailHandle self );
HCHAR*		OPL_Mail_GetNetworkTypeName ( OPL_MailHandle self );
HCHAR*		OPL_Mail_GetDate ( OPL_MailHandle self );
HCHAR*		OPL_Mail_GetMsgBody ( OPL_MailHandle self );
HBOOL		OPL_Mail_GetReadDoneFlag ( OPL_MailHandle self );
HCHAR*		OPL_Mail_GetMsgTitle ( OPL_MailHandle self );

HINT32		OPL_Mail_GetDummyBoardMsgList(OPL_MailHandle **mailList);
void			OPL_Mail_DeleteDummyBoardMsg (OPL_MailHandle mail);

#ifdef __cplusplus
};
#endif

#endif //__OPLMAIL_H__


