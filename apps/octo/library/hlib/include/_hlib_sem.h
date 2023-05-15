/*************************************************************
 * @file		hxsem.h
 * @date		2011/06/01
 * @author		someone
 * @brief		semaphore interface for IPC / HUMAX common library header

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef	_HXSEM_H_
#define	_HXSEM_H_

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___Global_Variables______________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Macro_Definition______________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___MACRO_FUCTION_Definition______________________________________________________________________
/* *************************************************************************************************** */

typedef enum _HxSEMT_SuspendType
{
	HxSEMT_FIFO,
	HxSEMT_PRIORITY,

	HxSEMT_MAX
} HxSEMT_SuspendType_t;


/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */

HINT32	HLIB_VKSEM_Create (HUINT32 *semId, const HCHAR *name, HINT32 etSuspendtype);
HINT32	HLIB_VKSEM_Destroy (HUINT32 semId);
HINT32	HLIB_VKSEM_Get (HUINT32 semId, const HCHAR *file, HINT32 line);
HINT32	HLIB_VKSEM_Release (HUINT32 semId, const HCHAR *file, HINT32 line);
HINT32	HLIB_VKSEM_GetTimeout (HUINT32 semId, HUINT32 timeout, const HCHAR *file, HINT32 line);


/*
 * SEMT : thread와 thread간에 sem이 필요할 때, 한 process에 종속적
 */
#define HxSEMT_Create(semId,name,etype)		(HINT32)HLIB_VKSEM_Create(semId, name, etype)
#define HxSEMT_Destroy(semId)				HLIB_VKSEM_Destroy(semId)
#define HxSEMT_Get(semId)					HLIB_VKSEM_Get(semId, __FILE__, __LINE__)
#define HxSEMT_Release(semId)				HLIB_VKSEM_Release(semId, __FILE__, __LINE__)
#define HxSEMT_GetTimeout(semId,timeout)	HLIB_VKSEM_GetTimeout(semId,timeout, __FILE__, __LINE__)

/*
 * Sem : process와 process간에 sem이 필요할때, 커널에 종속적
 */
HERROR	HLIB_SEM_Create(HINT32 nKey, HINT32 *pnSemId);
HERROR	HLIB_SEM_Get(HINT32 nSemId);
HERROR	HLIB_SEM_Release(HINT32 nSemId);
HERROR	HLIB_SEM_Destroy(HINT32 nSemId);



#endif /* _HXSEM_H_ */

