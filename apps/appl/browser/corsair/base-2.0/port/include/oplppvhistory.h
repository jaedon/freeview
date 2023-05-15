/**************************************************************************************/
/**
 * @file oplppvhistory.h
 *
 * Defines the ppvhistory structure.
 *
 * @author  LEE SUNG HOON(shlee3@humaxdigital.com)
 * @date    2012/10/11
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLPPVHISTORY_H__
#define __OPLPPVHISTORY_H__

#include	"opltype.h"
#include 	"bpltype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *	OPL_PPVHistoryHandle;
typedef void	(* OPL_UploadPPVViewlogDataListener_t) (int result);

/******************************************************************
	Define
******************************************************************/
void			OPL_PPVHistory_Delete (OPL_PPVHistoryHandle ppvhistory);
HERROR		OPL_PPVHistory_Remove( void  );
HERROR		OPL_PPVHistory_CancelPPVViewlogDataUpload( HINT32 * pnCancelResult  );
void			OPL_PPVHistory_SetUploadPPVViewlogDataListener( OPL_UploadPPVViewlogDataListener_t listener);
HERROR		OPL_PPVHistory_UploadPPVViewlogData(HINT32 * pnUploadResult);
HINT32		OPL_PPVHistory_GetPPVHistoryList ( OPL_PPVHistoryHandle **ppvhistoryList);
HINT32		OPL_PPVHistory_GetId ( OPL_PPVHistoryHandle self );
HINT32		OPL_PPVHistory_GetPurchaseDate ( OPL_PPVHistoryHandle self );
HUINT32		OPL_PPVHistory_GetPPVFee ( OPL_PPVHistoryHandle self );
HERROR		OPL_PPVHistory_GetSystemId ( OPL_PPVHistoryHandle self, HCHAR *pBuf, HUINT32 bufLen );
HERROR		OPL_PPVHistory_GetEventData ( OPL_PPVHistoryHandle self, HCHAR *pBuf, HUINT32 bufLen );

#ifdef __cplusplus
};
#endif

#endif //__OPLPPVHISTORY_H__



