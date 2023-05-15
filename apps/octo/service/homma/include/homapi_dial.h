/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  homapi_dial.h
	@brief

	Description:  									\n
	Module: HOMMA / API								\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef __HOMAPI_DIAL_H__
#define __HOMAPI_DIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	________________Header_Files_______________________________
#include    <homapi.h>

void	HOMAPI_DIAL_Init(void);
void	HOMAPI_DIAL_DeInit(void);
HERROR	HOMAPI_DIAL_SetFriendlyName(const HCHAR *pFriendlyName );
void	HOMAPI_DIAL_RegisterNotifier(HOMAPI_EventCallback pfnEventHandler);
void	HOMAPI_DIAL_UnRegisterNotifier(void);

HERROR	HOMAPI_DIAL_Start(HCHAR *pFriendlyName);
HERROR	HOMAPI_DIAL_Stop(void);

HERROR	HOMAPI_DIAL_AppRegist(const HCHAR * app);
HERROR	HOMAPI_DIAL_AppUnregist(const HCHAR * app);
HERROR	HOMAPI_DIAL_SetAppStatus(const HCHAR * app, HINT32 status);
HERROR	HOMAPI_DIAL_SetCookie(const HCHAR * cookie );

HERROR	HOMAPI_DIAL_GetDialStatus(HBOOL * pDialActivated);
HERROR	HOMAPI_DIAL_GetFriendlyName(HCHAR * pFriendlyName, HUINT32 size);
HERROR	HOMAPI_DIAL_GetAdditionalDataUrl(HCHAR * pAdditionalDataUrl, HUINT32 size, HCHAR * pAppName);

#ifdef	__cplusplus
}
#endif

#endif //end of __HOMAPI_DIAL_H__