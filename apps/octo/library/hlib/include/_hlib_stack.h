/*************************************************************
 * @file		hxstack.h
 * @date		2011/01/25
 * @author	Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header, related with stack system?

 * http://www.humaxdigital.com
 *************************************************************/

#define	____HXSTACK_README___________________

/******************************************************************************
 HxVector_t를 기반으로 만든 스택

 [Example Code]


 [Expected Result]
 ************************************************************************************/

#ifndef _HXSTACK_H_
#define _HXSTACK_H_


#include <_hlib_vector.h>

#define	____HXSTACK_TYPE___________________

typedef HxVector_t	HxStack_t;

#define	____HXSTACK_CompareAPIs____________________________________________________

/**
 * Item을 비교할 때 호출되는 연산(Method), 만약 NULL이면 default compare 연산 호출됨

 *
 * 위의 매크로로 아래의 함수들 정의 되어 있음 (HINT8, HINT16, HINT32, HUINT8, HUINT16, HUINT32)
 * @param arg1  pointer to Item (아이템의 포인터임!!! 주의!!)
 * @param arg2  pointer to Item (아이템의 포인터임!!! 주의!!)
 * @return	0: same, -/+
 */
#define HxStack_CompareHINT8	HLIB_VECTOR_CompareHINT8
#define HxStack_CompareHINT16	HLIB_VECTOR_CompareHINT16
#define HxStack_CompareHINT32	HLIB_VECTOR_CompareHINT32
#define	HxStack_CompareHUINT8	HLIB_VECTOR_CompareHUINT8
#define	HxStack_CompareHUINT16	HLIB_VECTOR_CompareHUINT16
#define	HxStack_CompareHUINT32	HLIB_VECTOR_CompareHUINT32
#define	HxStack_CompareSzASCII	HLIB_VECTOR_CompareSzASCII


#define	____HXSTACK_APIs___________________

#define	HxStack_New(self,cmp)			((HxStack_t *)HLIB_VECTOR_New((HxVector_t*)(self),cmp))
#define	HxStack_NewEx(s,l,mk,rm,cmp)	((HxStack_t *)HLIB_VECTOR_NewEx((HxVector_t*)(s),l,mk,rm,cmp))
#define	HxStack_Delete(self)			HLIB_VECTOR_Delete((HxVector_t*)(self))
#define	HxStack_Length(self)			HLIB_VECTOR_Length((HxVector_t*)(self))

#define	HxStack_Push(self,data)			HLIB_VECTOR_Add((HxVector_t*)(self),data)
void *	HLIB_STACK_Pop(HxStack_t *self);

/**
 * "ToString" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	str		string buffer
 * @param	n		string buffer length
 * @return	str
 */
HUINT8 *	HLIB_STACK_ToString (HxVector_t *self, HUINT8 *str, HUINT32 n);

#endif /* _HXSTACK_H_ */

