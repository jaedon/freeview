
#define	____HLIB_ID_README___________________

/******************************************************************************
 [Example Code]

 [Expected Result]

 ************************************************************************************/

#ifndef	HLIB_ID_H__
#define HLIB_ID_H__

#include "htype.h"


#ifdef	__cplusplus

#include <cstdio>
#include <cstdarg>
extern	"C" {
#else
#include <stdio.h>
#include <stdarg.h>
#endif


typedef	void *	HxID_t;

#define	HxID_NULL	0
#define	HxID_MIN	0x0001
#define	HxID_MAX	0xFFFF

/**
 * min ~ max 범위의 ID를 생성해 주는 id gen을 생성
 *
 * @param	min		minimum id(>= HxID_MIN)
 * @param	max		maximum id(<= HxID_MAX)
 * @return	id gen
 */
HxID_t	HLIB_ID_New (HUINT16 min, HUINT16 max);

/**
 * id gen 삭제
 *
 * @param	hiD		id gen
 */
void	HLIB_ID_Delete (HxID_t hID);

/**
 * id 생성
 *
 * @param	hID		id gen
 * @return	id (or HxID_NULL)
 */
HINT32	HLIB_ID_Alloc (HxID_t hID);

/**
 * id 반환
 *
 * @param	hID		id gen
 * @param	id 		id
 */
void	HLIB_ID_Free (HxID_t hID, HINT32 id);

/**
 * id 생성 (특정 id로)
 *
 * @param	hID		id gen
 * @param	id 		id
 * @return	id (or HxID_NULL, if id가 이미 생성되어 있는 경우)
 */
HINT32	HLIB_ID_AllocWithSpecifiedID (HxID_t hID, HINT32 id);
#define	HLIB_ID_AllocS(hID,id)		HLIB_ID_AllocWithSpecifiedID(hID,id)

/**
 * id
 *
 * @param	hID		id gen
 * @param	id 		id
 * @return	id (or HxID_NULL, if id가 이미 생성되어 있는 경우)
 */
HBOOL	HLIB_ID_HasID (HxID_t hID, HINT32 id);

#ifdef	__cplusplus
}
#endif

#endif /* HLIB_ID_H__ */

