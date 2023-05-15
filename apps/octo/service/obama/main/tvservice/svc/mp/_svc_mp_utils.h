/** **********************************************************************************************************
	@file 		_svc_mp_utils.h

	@date		2012/01/03
	@author		IPM-SW1T (tipmsw1@humaxdigital.com)
	@breif

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. ("Humax") and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */


#ifndef __SVC_MP_UTILS_H__
#define __SVC_MP_UTILS_H__

#include "htype.h"

#define     MEDIA_TEST_SHM_KEY_ID         0x10230222 + 1234
#define     MEDIA_TEST_SHM_MEM_SIZE       (1024 * 16)
#define     MEDIA_TEST_SHM_HEAD_SIZE      13

#ifdef __cplusplus
extern "C" {
#endif

HINT32 media_player_utils_PostMsgToMgr(HINT32 nMsg, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3);

void media_player_utils_RegisterPostMsgCallback(PostMsgToMgrCb_t fnPostMsgToMgr);

HERROR media_player_utils_GetFileNameWithoutExt(HUINT8 *pucPathName, HUINT8 *pucPathFileName);

HERROR media_player_utils_make_png(void *pFrameBuffer, const HINT8 *pucPathFileName);

HERROR media_player_utils_subtitle_create_shmdata(HUINT32 *pulSHM_ID);

HERROR media_player_utils_subtitle_send_data(HUINT8 *buffer, HINT32 len, HUINT32 *pulStartOffset);

HERROR media_player_utils_subtitle_destroy_shmdata(HINT32 ulid);

#ifdef __cplusplus
};
#endif

#endif	/* __SVC_MP_UTILS_H__ */



