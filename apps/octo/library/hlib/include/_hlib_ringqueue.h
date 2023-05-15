/*************************************************************
 * @file		hxstack.h
 * @date		2011/01/25
 * @author	Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header, related with stack system?

 * http://www.humaxdigital.com
 *************************************************************/

#define	____HXRINGQUEUE_README___________________

/******************************************************************************
 HxVector_t를 기반으로 만든 스택

 [Example Code]


 [Expected Result]
 ************************************************************************************/

#ifndef __HLIB_RINGQUEUE_H__
#define __HLIB_RINGQUEUE_H__


#include <_hlib_vector.h>

#define	____HXRINGQUEUE_TYPE___________________

typedef HxVector_t	HxRingQueue_t;

#define	____HXRINGQUEUE_CompareAPIs____________________________________________________

/**
 * Item을 비교할 때 호출되는 연산(Method), 만약 NULL이면 default compare 연산 호출됨

 *
 * 위의 매크로로 아래의 함수들 정의 되어 있음 (HINT8, HINT16, HINT32, HUINT8, HUINT16, HUINT32)
 * @param arg1  pointer to Item (아이템의 포인터임!!! 주의!!)
 * @param arg2  pointer to Item (아이템의 포인터임!!! 주의!!)
 * @return	0: same, -/+
 */
#define HxRINGQUEUE_CompareHINT8	HLIB_VECTOR_CompareHINT8
#define HxRINGQUEUE_CompareHINT16	HLIB_VECTOR_CompareHINT16
#define HxRINGQUEUE_CompareHINT32	HLIB_VECTOR_CompareHINT32
#define	HxRINGQUEUE_CompareHUINT8	HLIB_VECTOR_CompareHUINT8
#define	HxRINGQUEUE_CompareHUINT16	HLIB_VECTOR_CompareHUINT16
#define	HxRINGQUEUE_CompareHUINT32	HLIB_VECTOR_CompareHUINT32
#define	HxRINGQUEUE_CompareSzASCII	HLIB_VECTOR_CompareSzASCII


#define	____HXRINGQUEUE_APIs___________________

#define	HxRINGQUEUE_New(self,queuesize)		((HxRingQueue_t *)HLIB_VECTOR_NewEx((HxVector_t*)(self),queuesize,NULL,NULL,NULL))
#define	HxRINGQUEUE_NewEx(s,l,mk,rm,cmp)	((HxRingQueue_t *)HLIB_VECTOR_NewEx((HxVector_t*)(s),l,mk,rm,cmp))
#define	HxRINGQUEUE_Delete(self)			HLIB_VECTOR_Delete((HxVector_t*)(self))
#define	HxRINGQUEUE_Length(self)			HLIB_VECTOR_Length((HxVector_t*)(self))
#define	HxRINGQUEUE_ForEach(s,func,udata)	HLIB_VECTOR_ForEach((HxVector_t*)(s),func,udata)


HERROR	HLIB_RINGQUEUE_Put (HxRingQueue_t *self, void *data);
void	*HLIB_RINGQUEUE_Get (HxRingQueue_t *self);

/**
 * "ToString" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	str		string buffer
 * @param	n		string buffer length
 * @return	str
 */
HUINT8	*HLIB_RINGQUEUE_ToString (HxRingQueue_t *self, HUINT8 *str, HUINT32 n);

#endif /* __HLIB_RINGQUEUE_H__ */

