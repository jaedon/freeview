/*************************************************************
 * @file		_hlib_collection.h
 * @date		2011/02/28
 * @author	Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header, related with collection template?
 			item collection of default value (inherited from HxVector)

 * http://www.humaxdigital.com
 *************************************************************/

#define	____HXCOLLECTION_README___________________

/******************************************************************************
 [Example Code]

void example (void)
{
	HINT32 i;
	HxCOLLECTION_t *collection;

	collection = HLIB_COLLECTION_New(NULL, HLIB_COLLECTION_CompareSzASCII);

	HLIB_COLLECTION_Add(collection, "Welcome");
	HLIB_COLLECTION_Add(collection, "to");
	HLIB_COLLECTION_Add(collection, "Humax");
	HLIB_COLLECTION_Add(collection, "STB");
	HLIB_COLLECTION_Add(collection, "World");

	HLIB_COLLECTION_SetDefault(collection, 2);	//index of "Humax"

	for (i = 0 ; i < 10 ; i++)
	{
		printf("%2d. %s\n", i + 1, (char *)HLIB_COLLECTION_GetItem(collection, i));
	}
	// collection은 out of range 상황에서 default index를 리턴하는 것을 특징으로 한다.
}

 [Expected Result]

 1. Welcome
 2. to
 3. Humax
 4. STB
 5. World
 6. Humax
 7. Humax
 8. Humax
 9. Humax
10. Humax
 ************************************************************************************/

#ifndef HCOLLECTION_H_
#define HCOLLECTION_H_

#include <_hlib_vector.h>

#define	____HXCOLLECTION_TYPE___________________

typedef struct
{
	HxVector_t	vector;		///< extended from vector
	HINT32		defIndex;	///< default index
	void *		allocSelf;	///< allocated by self??????
} HxCOLLECTION_t;


#define	____HXCOLLECTION_CompareAPIs____________________________________________________

/**
 * Item을 비교할 때 호출되는 연산(Method), 만약 NULL이면 default compare 연산 호출됨

 *
 * 위의 매크로로 아래의 함수들 정의 되어 있음 (HINT8, HINT16, HINT32, HUINT8, HUINT16, HUINT32)
 * @param arg1  pointer to Item (아이템의 포인터임!!! 주의!!)
 * @param arg2  pointer to Item (아이템의 포인터임!!! 주의!!)
 * @return	0: same, -/+
 */
#define HLIB_COLLECTION_CompareHINT8	HLIB_VECTOR_CompareHINT8
#define HLIB_COLLECTION_CompareHINT16	HLIB_VECTOR_CompareHINT16
#define HLIB_COLLECTION_CompareHINT32	HLIB_VECTOR_CompareHINT32
#define	HLIB_COLLECTION_CompareHUINT8	HLIB_VECTOR_CompareHUINT8
#define	HLIB_COLLECTION_CompareHUINT16	HLIB_VECTOR_CompareHUINT16
#define	HLIB_COLLECTION_CompareHUINT32	HLIB_VECTOR_CompareHUINT32
#define	HLIB_COLLECTION_CompareSzASCII	HLIB_VECTOR_CompareSzASCII


#define	____HXCOLLECTION_APIs___________________

/**
 * Alloc(or Init) New HxCOLLECTION_t
 *
 * @param	self	NULL이면 내부적으로 할당해서 리턴
 * @param	length	pre-allocate collection length
 * @param	mk		item을 생성할 때 호출(생성자 역할)
 * @param	rm		item을 삭제할 때 호출(소멸자 역할)
 * @param	cmp		item을 비교할 때 호출(만약 넣지 않으면 단순 비교)
 * @return	Instance of 'HxCOLLECTION_t'
 */
HxCOLLECTION_t	*HLIB_COLLECTION_NewEx  (
					HxCOLLECTION_t *self,
					HINT32 length,
					void * (*mk)(void *),
					void * (*rm)(void *),
					HINT32 (* cmp)(const void *, const void *)
				);
#define		HLIB_COLLECTION_New(self,cmp)		HLIB_COLLECTION_NewEx(self, 10, NULL, NULL, cmp)

/**
 * Free(or Deinit)
 *
 * @param	self	Instance of 'HxCOLLECTION_t'
 */
void		HLIB_COLLECTION_Delete (HxCOLLECTION_t *self);

/**
 * Default Index를 세팅한다.
 *
 * @param	self	Instance of 'HxCOLLECTION_t'
 * @param	index	default index
 * @return	if success, return value is same as index / if fail, return negative value
 */
HINT32		HLIB_COLLECTION_SetDefault (HxCOLLECTION_t *self, HINT32 index);

#define		HLIB_COLLECTION_Add(self,data)		HLIB_VECTOR_Add((HxVector_t *)(self), data)
#define		HLIB_COLLECTION_Length(self)		HLIB_VECTOR_Length((HxVector_t *)(self))

/**
 * "GetItem" Method
 * n번째 아이템을 리턴하는 것을 기본으로 하며, n이 범위를 벗어난 경우, default value를 리턴
 *
 * @param	self	Instance of 'HxCOLLECTION_t'
 * @param	n		index
 * @return	collection의 n번째 아이템
 */
void *		HLIB_COLLECTION_GetItem	(HxCOLLECTION_t *self, HINT32 n);
#define		HLIB_COLLECTION_GetDefaultItem(self)		HLIB_COLLECTION_GetItem(self, -1)

#define		HLIB_COLLECTION_ForEach(self,func,user)	HLIB_VECTOR_ForEach((HxVector_t *)(self),func,user)

/**
 * "ToString" Method
 *
 * @param	self	Instance of 'HxCOLLECTION_t'
 * @param	str		string buffer
 * @param	n		string buffer length
 * @return	str
 */
HUINT8 *	HLIB_COLLECTION_ToString (HxCOLLECTION_t *self, HUINT8 *str, HUINT32 n);

#endif /* HCOLLECTION_H_ */
