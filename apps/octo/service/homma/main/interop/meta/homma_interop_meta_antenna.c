/**************************************************************
*	@file	homma_interop.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			HOMMA / interoperation supporting instead
*					of using APK
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/* ------------------------------------------------------------- //
 * Author : tjjang
 * Description : This is for replacing apk methods
 *  - HOMMA_INTEROP prefix is used for external methods
 *  - s_HOMMA_INTEROP prefix is used to represent static methods
 */

#include <homma_interop.h>

/*
 * methods
 *  - static & external
 */


#ifdef HOMMA_USE_APPKIT_DIRECTLY
#else
#define ENTER_CRITICAL_HINTOPMETAANT		do { HxSEMT_Get(s_ulhintopMetaAntSemId); } while (0)
#define	LEAVE_CRITICAL_HINTOPMETAANT		do { HxSEMT_Release(s_ulhintopMetaAntSemId); } while (0)

#define	RETURN_IF_VOID(expr)	do{if(expr)return;}while(0)
#define	RETURN_IF(expr, err)	do{if(expr)return (err);}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr)goto pos;}while(0)

typedef struct
{
	HxVector_t		*pstAntVector;
	HUINT32			nAntNotifierId;
} hintopMetaAnt_Ctx_t;


static HUINT32					s_ulhintopMetaAntSemId;


static HERROR	homma_interop_meta_antenna_Load (void);
#endif

/* ############################################################################
                  _
                 | |
 _ __ ___    ___ | |_  __ _
| '_ ` _ \  / _ \| __|/ _` |
| | | | | ||  __/| |_| (_| |
|_| |_| |_| \___| \__|\__,_|
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  meta record & antenna & svn  xxxxxxxxx
############################################################################ */

// unwilling to use apps/octo/appkit/
#ifdef HOMMA_USE_APPKIT_DIRECTLY

HBOOL HOMMA_INTEROP_META_ANTENNA_Init(HBOOL bMainApp)
{
	return APK_META_ANTENNA_Init(bMainApp);
}

HBOOL HOMMA_INTEROP_META_ANTENNA_DeInit(void)
{
	return APK_META_ANTENNA_DeInit();
}

void HOMMA_INTEROP_META_ANTENNA_InfoFree (DxAntInfo_t *p)
{
	APK_META_ANTENNA_InfoFree(p);
}

HERROR HOMMA_INTEROP_META_ANTENNA_Load (void)
{
	return APK_META_ANTENNA_Load();
}

HUINT32 HOMMA_INTEROP_META_ANTENNA_Count (void)
{
	return APK_META_ANTENNA_Count();
}

HINT32 HOMMA_INTEROP_META_ANTENNA_GetUidAt (HINT32 index)
{
	return APK_META_ANTENNA_GetUidAt(index);
}

DxAntInfo_t * HOMMA_INTEROP_META_ANTENNA_GetInfo (HINT32 uid)
{
	return APK_META_ANTENNA_GetInfo(uid);
}

#else // #ifdef HOMMA_USE_APPKIT_DIRECTLY else

#define __________________INTERNAL_FUNCTION__________________
static hintopMetaAnt_Ctx_t	*	homma_interop_meta_antenna_GetContext (void)
{
	static hintopMetaAnt_Ctx_t		s_stManager;
	if (s_stManager.pstAntVector == NULL)
	{
		s_stManager.pstAntVector = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
		HxLOG_Assert(s_stManager.pstAntVector != NULL);
	}

	return &s_stManager;
}

static DxAntInfo_t *	homma_interop_meta_antenna_FindAntenna (HxVector_t *list, HINT32 uid)
{
	DxAntInfo_t	*info;

	HxLOG_Trace();

	HxVECTOR_EACH(list, DxAntInfo_t*, info, {
		if (info && info->uid == uid)
			return info;
	});
	return NULL;
}

static void		homma_interop_meta_antenna_OnDataReceived(const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	HxVector_t		*pstAntVector;
	DxAntInfo_t		*pstDxAnt;
	DAxIterator_t	iter;
	hintopMetaAnt_Ctx_t	*ctx;

	HxLOG_Trace();

	if (pvData == NULL || size == 0)
	{
		HxLOG_Error("[%s] Invalid param(s).\n", __FUNCTION__);
		return;
	}

	pstAntVector  = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))HLIB_STD_MemFree_CB, NULL);
	if (pstAntVector == NULL)
	{
		HxLOG_Critical("[%s] Out of memory!\n", __FUNCTION__);
		return;
	}

	DAPI_InitIterator(&iter, pvData, size, sizeof(DxAntInfo_t));
	while (DAPI_NextIterator(&iter))
	{
		pstDxAnt    = (DxAntInfo_t*)iter.data;
		if (pstDxAnt == NULL)
		{
			continue;
		}
		HLIB_VECTOR_Add(pstAntVector, HLIB_STD_MemDup(pstDxAnt, sizeof(DxAntInfo_t)));
	}
	if (HLIB_VECTOR_Length(pstAntVector) == 0)
	{
		HLIB_VECTOR_Delete(pstAntVector);
		pstAntVector = NULL;
	}

	ENTER_CRITICAL_HINTOPMETAANT;
	ctx = homma_interop_meta_antenna_GetContext();
	if (ctx->pstAntVector)
		HLIB_VECTOR_Delete(ctx->pstAntVector);
	ctx->pstAntVector = pstAntVector;
	HxLOG_Debug("[%s:%d] Antenna loading done!! count:%d\n", __FUNCTION__, __LINE__, pstAntVector ? pstAntVector->count : 0);
	LEAVE_CRITICAL_HINTOPMETAANT;
}

static HERROR	homma_interop_meta_antenna_Load (void)
{
	HUINT32	id;

	HxLOG_Trace();

	if ((id = DAPI_GetAll(DxNAMEOF(DxAntInfo_t), homma_interop_meta_antenna_OnDataReceived, NULL)) == 0)
	{
		HxLOG_Error("[%s:%d] Query error\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	if (DAPI_Wait(6000, id) == DERR_TIMEOUT)
	{
		HxLOG_Critical("[%s:%d] DAPI_Wait() timeout! \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	return ERR_OK;
}

static void		homma_interop_meta_antenna_OnDamaAntennaUpdated (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	(void)homma_interop_meta_antenna_Load();
}

#define __________________EXTERNAL_FUNCTION__________________
//APK_META_ANTENNA_Init();
HBOOL HOMMA_INTEROP_META_ANTENNA_Init(void)
{
	hintopMetaAnt_Ctx_t *ctx;

	ctx = homma_interop_meta_antenna_GetContext();
	HxSTD_MemSet(ctx, 0x00, sizeof(hintopMetaAnt_Ctx_t));

	HxSEMT_Create(&s_ulhintopMetaAntSemId, "hintopmetaant", HxSEMT_FIFO);

	(void)homma_interop_meta_antenna_Load();

	ctx->nAntNotifierId = DAPI_AddTableNotifier(DxNAMEOF(DxAntInfo_t), homma_interop_meta_antenna_OnDamaAntennaUpdated, NULL);

	return TRUE;
}

HBOOL HOMMA_INTEROP_META_ANTENNA_DeInit(void)
{
	hintopMetaAnt_Ctx_t *ctx;

	ctx = homma_interop_meta_antenna_GetContext();

	// clear vector
	ENTER_CRITICAL_HINTOPMETAANT;
	if (ctx->pstAntVector)
	{
		HLIB_VECTOR_Delete(ctx->pstAntVector);
	}
	LEAVE_CRITICAL_HINTOPMETAANT;

	// clear notifier
	if (ctx->nAntNotifierId)
	{
		DAPI_RemoveNotifier(ctx->nAntNotifierId);
		ctx->nAntNotifierId = 0;
	}

	// remove semaphore
	HxSEMT_Destroy(s_ulhintopMetaAntSemId);

	return TRUE;
}

//APK_META_ANTENNA_InfoFree(pAntInfo);
void HOMMA_INTEROP_META_ANTENNA_InfoFree (DxAntInfo_t *p)
{
	if(p != NULL)
	{
		HLIB_STD_MemFree(p);
	}
}

//APK_META_ANTENNA_Count();
HUINT32 HOMMA_INTEROP_META_ANTENNA_Count (void)
{
	hintopMetaAnt_Ctx_t	*ctx;
	HINT32	len;

	HxLOG_Trace();

	ENTER_CRITICAL_HINTOPMETAANT;
	ctx = homma_interop_meta_antenna_GetContext();
	len = HLIB_VECTOR_Length(ctx->pstAntVector);
	LEAVE_CRITICAL_HINTOPMETAANT;

	HxLOG_Debug("[%s] return Length(%d) \n", __FUNCTION__, len);

	return (HUINT32)len;
}

//APK_META_ANTENNA_GetUidAt(i)
HINT32 HOMMA_INTEROP_META_ANTENNA_GetUidAt (HINT32 index)
{
	hintopMetaAnt_Ctx_t	*ctx;
	DxAntInfo_t	*pstDxAnt;
	HINT32				uid;

	HxLOG_Trace();

	ENTER_CRITICAL_HINTOPMETAANT;
	ctx  = homma_interop_meta_antenna_GetContext();
	pstDxAnt = (DxAntInfo_t *)HLIB_VECTOR_ItemAt(ctx->pstAntVector, index);
	if (pstDxAnt)
		uid = pstDxAnt->uid;
	else
		uid = 0;
	LEAVE_CRITICAL_HINTOPMETAANT;

	HxLOG_Debug("[%s] idx(%d) uid(%d) ", __FUNCTION__, index, uid);

	return uid;
}

//APK_META_ANTENNA_GetInfo(APK_META_ANTENNA_GetUidAt(i));
DxAntInfo_t * HOMMA_INTEROP_META_ANTENNA_GetInfo (HINT32 uid)
{
	hintopMetaAnt_Ctx_t	*ctx;
	DxAntInfo_t	*info, *clone;

	HxLOG_Trace();

	ENTER_CRITICAL_HINTOPMETAANT;
	ctx  = homma_interop_meta_antenna_GetContext();
	info = homma_interop_meta_antenna_FindAntenna(ctx->pstAntVector, uid);
	if (info)
		clone = HLIB_STD_MemDup((void *)info, sizeof(DxAntInfo_t));
	else
		clone = NULL;

	LEAVE_CRITICAL_HINTOPMETAANT;

	if ((clone == NULL) || (clone->satName == NULL))
	{
		HxLOG_Debug("[%s] uid(%d) satName(NULL)", __FUNCTION__, uid);
	}
	else
	{
		HxLOG_Debug("[%s] uid(%d) satName(%s)", __FUNCTION__, uid, clone->satName);
	}

	return clone;
}

#endif // #ifdef HOMMA_USE_APPKIT_DIRECTLY endif

