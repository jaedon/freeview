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

/******************************************************************************/
/**
 * @file	  		nx_port_meta_record.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#include <nx_port.h>


void NX_PORT_META_RECORD_EnableLoadEvent(void)
{
	APK_META_RECORD_EnableLoadEvent();
}

void NX_PORT_META_RECORD_AddNotifier(NX_MetaRecord_Notifier_t notifier, void *userdata)
{
	APK_META_RECORD_AddNotifier((ApkMetaRecord_Notifier_t)notifier, userdata);
}

DxRecListData_t* NX_PORT_META_RECORD_GetByCID (const HUINT32 cid)
{
	return APK_META_RECORD_GetByCID (cid);
}

DxRecListData_t* NX_PORT_META_RECORD_GetByUrl (const HCHAR *url)
{
	return APK_META_RECORD_GetByUrl(url);
}

void NX_PORT_META_RECORD_Release (DxRecListData_t *recData)
{
	APK_META_RECORD_Release(recData);
}

HUINT32	NX_PORT_META_RECORD_Count (void)
{
	return APK_META_RECORD_Count();
}

HxVector_t *NX_PORT_META_RECORD_GetItems(void)
{
	return APK_META_RECORD_GetItems();
}


