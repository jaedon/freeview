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
 * @file	  		nx_core_channel_mbc.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
// middle east
#define		BQT_MBC_ME			0xE000
#define		BQT_NEWS_ME			0xE001
#define		BQT_MOVIE_ME		0xE002
#define		BQT_DRAMA_ME		0xE003
#define		BQT_SPORTS_ME		0xE004
// north africa
#define		BQT_MBC_NA			0XF000
#define		BQT_NEWS_NA			0xF001
#define		BQT_MOVIE_NA		0xF002
#define		BQT_DRAMA_NA		0xF003
#define		BQT_SPORTS_NA		0xF004

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include "nx_core_channel_local.h"

HxVector_t	*nx_channel_group_mbc_GetReferServiceList(HxVector_t *totalSvcList, NX_ChListGroup_t eChGroupType)
{
	DxService_t	*service	= NULL;
	HxVector_t	*referList 	= HLIB_VECTOR_NewEasy(0, NULL, NULL);

	switch(eChGroupType)
	{
	case eNxChListGroup_MBC:
		HxVECTOR_EACH(totalSvcList, DxService_t *, service, {
			if ((service->bqtuids[0] == BQT_MBC_ME) || (service->bqtuids[0] == BQT_MBC_NA))
				HLIB_VECTOR_Add(referList, (void*)service);
		});
		break;

	case eNxChListGroup_News:
		HxVECTOR_EACH(totalSvcList, DxService_t *, service, {
			if ((service->grpuids[0] == BQT_NEWS_ME) || (service->grpuids[0] == BQT_NEWS_NA))
				HLIB_VECTOR_Add(referList, (void*)service);
		});
		break;

	case eNxChListGroup_Movie:
		HxVECTOR_EACH(totalSvcList, DxService_t *, service, {
			if ((service->grpuids[0] == BQT_MOVIE_ME) || (service->grpuids[0] == BQT_MOVIE_NA))
				HLIB_VECTOR_Add(referList, (void*)service);
		});
		break;

	case eNxChListGroup_Drama:
		HxVECTOR_EACH(totalSvcList, DxService_t *, service, {
			if ((service->grpuids[0] == BQT_DRAMA_ME) || (service->grpuids[0] == BQT_DRAMA_NA))
				HLIB_VECTOR_Add(referList, (void*)service);
		});
		break;

	case eNxChListGroup_Sports:
		HxVECTOR_EACH(totalSvcList, DxService_t *, service, {
			if ((service->grpuids[0] == BQT_SPORTS_ME) || (service->grpuids[0] == BQT_SPORTS_NA))
				HLIB_VECTOR_Add(referList, (void*)service);
		});
		break;

	default:
		break;
	}
	return referList;
}

