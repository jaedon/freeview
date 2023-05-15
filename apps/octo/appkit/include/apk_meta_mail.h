/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_meta_mail.h
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
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

#ifndef ___APK_META_MAIL_H___
#define ___APK_META_MAIL_H___
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef void	(*APK_META_MAIL_Notifier_t) (Handle_t self, HINT32 event, const HINT32 *args, void *userdata);

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
void	APK_META_MAIL_Delete (void *object);
void *	APK_META_MAIL_Clone (const void *object);
HERROR		APK_META_MAIL_Remove( HINT32 id );
HERROR		APK_META_MAIL_Update( DxMail_t *updateMail);
DxMail_t *	APK_META_MAIL_GetMail (HINT32 uid);
HxVector_t *	APK_META_MAIL_GetMailList (void);
HERROR	APK_META_MAIL_LoadFromOcto (void);
void			APK_META_MAIL_RegisterNotifier (APK_META_MAIL_Notifier_t fnNotifier, void *userdata);
void			APK_META_MAIL_UnRegisterNotifier ( void );

#endif

/* end of file */
