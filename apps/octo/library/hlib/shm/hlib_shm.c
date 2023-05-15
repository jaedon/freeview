/*
 * hxshm.c
 *
 *  Created on: 2011. 3. 17.
 */

#include <hlib.h>

#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"hxshm"

#if defined(OS_LINUX)
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#define	____HxSHM_PRIVATE_FUNCTIONS______________________________________________


#define	____HxSHM_APIs____________________________________________________


HERROR	HLIB_SHM_Create(HINT32 nKey, HUINT32 ulMemSize, HINT32 *pnShmId)
{
	HINT32		nShmId;

	if(pnShmId == NULL)
		return ERR_FAIL;

    /* shared memory »ý¼º */
	nShmId = shmget((key_t)nKey, (size_t)ulMemSize, IPC_CREAT|0666);
	if(nShmId == -1)
	{
		HxLOG_Error("fail to create a shared memory (HLIB_SHM_Create)(ulMemSize:%d)\n",ulMemSize);
		return ERR_FAIL;
	}

	*pnShmId = nShmId;

	return ERR_OK;
}

HERROR	HLIB_SHM_Attach(HINT32 nShmId, void **ppvMemPtr)
{
	void	*pvShmAddr;

	if(ppvMemPtr == NULL)
		return ERR_FAIL;

	pvShmAddr = shmat(nShmId, (void *)NULL, 0);
	if(pvShmAddr == (void *)-1)
	{
		HxLOG_Error("fail to attach the shared memory to this process\n");
		return ERR_FAIL;
	}

	*ppvMemPtr = pvShmAddr;
	return ERR_OK;
}

HERROR	HLIB_SHM_Detach(void *pvMemPtr)
{
	if(pvMemPtr == NULL)
		return ERR_FAIL;

	if(-1 == shmdt(pvMemPtr))
	{
		HxLOG_Error("fail to detach the shared memory from this process\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	HLIB_SHM_Destroy(HINT32 nShmId)
{
	if(-1 == shmctl(nShmId, IPC_RMID, 0))
	{
		int errsv = errno;
		struct shmid_ds info;

		// you can find "man shmctl" documents.
		if ( EACCES == errsv )
		{
			HxLOG_Error("[errno:0x%X] your permission is wrong!!!\n",errsv);
		}
		else if ( EINVAL == errsv )
		{
			HxLOG_Error("[errno:0x%X] your shmid(0x%X) is not valid identifier !!!\n",errsv,nShmId);
		}

		shmctl(nShmId,IPC_STAT, &info);
		HxLOG_Error("fail to destroy the shared memory (attched count:%d)(mode:0x%X)\n",
					info.shm_nattch,info.shm_perm.mode);

		return ERR_FAIL;
	}

	return ERR_OK;
}

#else

#include <vkernel.h>

STATIC HxHASH_t *s_pShmHash = NULL;
STATIC HULONG	s_ulShmSem = 0;
STATIC HULONG	s_ulShmIdIndex = 0x991;

typedef struct _HxSHM_Context
{
	void	*buf;
	HULONG	sem;
	HUINT32	shmId;
} HxSHM_Context_t;

STATIC HxSHM_Context_t *_hlib_shm_NewContext(HUINT32 ulMemSize)
{
	HERROR	hErr = ERR_OK;
	HULONG	ulSemId = 0;
	HCHAR	szSemName[128] = {0, };

	HxSHM_Context_t 		*pContext = NULL;

	HxSTD_sprintf(szSemName, "shm_%x", s_ulShmIdIndex);
	hErr = VK_SEM_Create(&ulSemId, szSemName, VK_SUSPENDTYPE_FIFO);
	if(ERR_OK != hErr)
	{
		return NULL;
	}

	pContext = HLIB_MEM_Malloc(sizeof(HxSHM_Context_t));
	HxLOG_Assert(pContext);

	pContext->buf		= HLIB_MEM_Malloc(ulMemSize);
	pContext->shmId		= s_ulShmIdIndex++;
	pContext->sem		= ulSemId;

	return pContext;
}


HERROR	HLIB_SHM_Create(HINT32 nKey, HUINT32 ulMemSize, HINT32 *pnShmId)
{
	HERROR	hErr = ERR_OK;

	HxSHM_Context_t 		*pContext = NULL;

	if(0 == s_ulShmSem)
	{
		hErr = VK_SEM_Create(&s_ulShmSem, "ulShmSem", VK_SUSPENDTYPE_FIFO);
		if(ERR_OK != hErr)
		{
			return ERR_FAIL;
		}
	}

	VK_SEM_Get(s_ulShmSem);

	if(NULL == s_pShmHash)
	{
		s_pShmHash = HLIB_HASH_New (HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual);
	}
	else
	{
		pContext = HLIB_HASH_Lookup(s_pShmHash, nKey);
	}

	if(NULL == pContext)
	{
		pContext = _hlib_shm_NewContext(ulMemSize);
		if(NULL != pContext)
		{
			HLIB_HASH_Insert(s_pShmHash, nKey, pContext);
			*pnShmId = nKey;
		}
	}

	VK_SEM_Release(s_ulShmSem);

	if(NULL == pContext)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	HLIB_SHM_Attach(HINT32 nShmId, void **ppvMemPtr)
{
	HxSHM_Context_t 		*pContext = NULL;

	if(0 == s_ulShmSem)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulShmSem);
	pContext = HLIB_HASH_Lookup(s_pShmHash, nShmId);
	if(NULL != pContext)
	{
		*ppvMemPtr = pContext->buf;
	}

	VK_SEM_Release(s_ulShmSem);

	if(NULL == pContext)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get(pContext->sem);

	return ERR_OK;
}


STATIC HBOOL	_hxshm_FindContext	(void*  key , void*  value , void*  user_data)
{
	HxSHM_Context_t 		*pContext = (HxSHM_Context_t *)value;

	if(user_data == pContext->buf)
	{
		return TRUE;
	}
	return FALSE;
}


HERROR	HLIB_SHM_Detach(void *pvMemPtr)
{
	HxSHM_Context_t 		*pContext = NULL;

	if(0 == s_ulShmSem)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulShmSem);
	pContext  = HLIB_HASH_Find (s_pShmHash, _hxshm_FindContext, pvMemPtr);
	VK_SEM_Release(s_ulShmSem);

	if(NULL != pContext)
	{
		VK_SEM_Release(pContext->sem);
	}

	return ERR_FAIL;
}

HERROR	HLIB_SHM_Destroy(HINT32 nShmId)
{
	HxSHM_Context_t 	*pContext = NULL;
	HULONG				semId = 0;
	if(0 == s_ulShmSem)
	{
		return ERR_FAIL;
	}

	VK_SEM_Get(s_ulShmSem);
	pContext = HLIB_HASH_Lookup(s_pShmHash, nShmId);	
	if(NULL != pContext)
	{
		semId = pContext->sem;
		VK_SEM_Get(semId);
		HLIB_MEM_Free(pContext->buf);
		HLIB_MEM_Free(pContext);
		VK_SEM_Release(semId);
	}
	VK_SEM_Release(s_ulShmSem);
	
	if(0 != semId)
	{
		VK_SEM_Destroy (semId);
	}

	return ERR_OK;
}

#endif /* defined(OS_LINUX) */

/* end of file */

