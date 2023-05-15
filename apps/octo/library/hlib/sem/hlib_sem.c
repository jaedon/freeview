/*
* hxsem.c
*
*  Created on: 2011. 6. 01.
*/

#include <hlib.h>

#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"hxsem"

#include "vkernel.h"

#if defined(OS_LINUX)
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define	____HxSEM_PRIVATE_FUNCTIONS______________________________________________


#define	____HxSEM_APIs____________________________________________________


/* 공유 메모리 생성 */
HERROR	HLIB_SEM_Create(HINT32 nKey, HINT32 *pnSemId)
{
	HINT32		nSemId;
	/* semaphores 초기값을 줄때 사용하는 공용체 */
	union semun {
		int 				val;
		struct semid_ds		*buf;
		unsigned short int	*array;
	} argument;

	if(pnSemId == NULL)
		return ERR_FAIL;

	/* semaphore 생성 */
	nSemId = semget((key_t)nKey, 1, IPC_CREAT|0666);
	if(nSemId < 0)
	{
		HxLOG_Error("semget error\n");
		return ERR_FAIL;
	}

	/* semaphore 값을 1로 설정 */
	argument.val = 1;
	if(-1 == semctl(nSemId, 0, SETVAL, argument))
	{
		HxLOG_Error("semctl error\n");
		HLIB_SEM_Destroy(nSemId);
		return ERR_FAIL;
	}

	*pnSemId = nSemId;

	return ERR_OK;
}

HERROR	HLIB_SEM_Get(HINT32 nSemId)
{
	struct sembuf	in = {0, -1, SEM_UNDO};

	if(-1 == semop(nSemId, &in, 1))
	{
		HxLOG_Error("get sem error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	HLIB_SEM_Release(HINT32 nSemId)
{
	struct sembuf	out = {0, 1, SEM_UNDO};

	if(-1 == semop(nSemId, &out, 1))
	{
		HxLOG_Error("release sem error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR	HLIB_SEM_Destroy(HINT32 nSemId)
{
	if(-1 == semctl(nSemId, 0, IPC_RMID))
	{
		HxLOG_Error("fail to destroy the semaphore\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
#else

HINT32	HxSem_Create (HUINT32 *semId, const HCHAR *name, HINT32 etSuspendtype)
{
	return (HINT32)VK_SEM_Create((unsigned long *)semId, (const char *)name, (VK_SUSPENDTYPE)etSuspendtype);
}

HINT32	HLIB_SEM_Destroy (HUINT32 semId)
{
	return (HINT32)VK_SEM_Destroy((unsigned long)semId);
}

HINT32	HLIB_SEM_Get (HUINT32 semId)
{
	return (HINT32)VK_SEM_Get((unsigned long)semId);
}

HINT32	HLIB_SEM_Release (HUINT32 semId)
{
	return (HINT32)VK_SEM_Release((unsigned long)semId);
}


#endif


HINT32	HLIB_VKSEM_Create (HUINT32 *semId, const HCHAR *name, HINT32 etSuspendtype)
{
	return (HINT32)VK_SEM_Create((unsigned long *)semId, (const char *)name, (VK_SUSPENDTYPE)etSuspendtype);
}

HINT32	HLIB_VKSEM_Destroy (HUINT32 semId)
{
	return (HINT32)VK_SEM_Destroy((unsigned long)semId);
}


#if defined(VK_SEM_DEBUG)
HINT32	HLIB_VKSEM_Get (HUINT32 semId, const HCHAR *file, HINT32 line)
{
	return (HINT32)VK_DBGSEM_Get((unsigned long)semId, file, line);
}

HINT32	HLIB_VKSEM_Release (HUINT32 semId, const HCHAR *file, HINT32 line)
{
	return (HINT32)VK_DBGSEM_Release((unsigned long)semId, file, line);
}

HINT32	HLIB_VKSEM_GetTimeout (HUINT32 semId, HUINT32 timeout, const HCHAR *file, HINT32 line)
{
	return (HINT32)VK_DBGSEM_GetTimeout((unsigned long)semId, (unsigned long)timeout, file, line);
}

#else /* #if defined(VK_SEM_DEBUG)	 */

HINT32	HLIB_VKSEM_Get (HUINT32 semId, const HCHAR *file, HINT32 line)
{
	(void)file; (void)line;
	return (HINT32)VK_SEM_Get((unsigned long)semId);
}

HINT32	HLIB_VKSEM_Release (HUINT32 semId, const HCHAR *file, HINT32 line)
{
	(void)file; (void)line;
	return (HINT32)VK_SEM_Release((unsigned long)semId);
}

HINT32	HLIB_VKSEM_GetTimeout (HUINT32 semId, HUINT32 timeout, const HCHAR *file, HINT32 line)
{
	(void)file; (void)line;
	return (HINT32)VK_SEM_GetTimeout((unsigned long)semId, (unsigned long)timeout);
}

#endif /* #if defined(VK_SEM_DEBUG)	 */

#define _______________________________________________

/* end of file */

