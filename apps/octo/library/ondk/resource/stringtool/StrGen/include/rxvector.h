/*************************************************************
 * @file		rxvector.h
 * @date		2011/01/17
 * @author	Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header, related with rxvector system
                     dynamic arrary to one side direction.

 * http://www.humaxdigital.com
 *************************************************************/

#define	____HXVECTOR_README___________________

/******************************************************************************
 [Example Code]

void printItem (void *data, void *userdata)
{
	fprintf(userdata, "\t%s\n", (char *)data);
}

// same as 'HVector_CompareStrASCII()'
HINT32 compareItem (const void *arg1, const void *arg2)
{
	return (HINT32)strcmp(*(char **)arg1, *(char **)arg2);
}

void example (void)
{
	MHVector_t	*vec;

	vec = MHVector_NewEx(NULL,
				10,		// pre-allocate rxvector length
				strdup,	// called when a item is created.
				MHSTD_Free,	// called when a item is desroyed.
				compareItem	// comparison function
			);
	assert(vec != NULL);

	puts("1. Add()");
	{
		MHVector_Add(vec, "Hello");
		MHVector_Add(vec, "MHVector_t");
		MHVector_ForEach(vec, printItem, stdout);
	}

	puts("2. Insert()");
	{
		MHVector_InsertAfter(vec, "Hello", "World");
		MHVector_ForEach(vec, printItem, stdout);
	}

	puts("3. Sort()");
	{
		MHVector_Sort(vec, NULL);
		MHVector_ForEach(vec, printItem, stdout);
	}

	MHVector_Delete(vec);
}

 [Expected Result]
1. Add()
	Hello
	MHVector_t
2. Insert()
	Hello
	World
	MHVector_t
3. Sort()
	MHVector_t
	Hello
	World
 ************************************************************************************/

#ifndef HVECTOR_H_
#define HVECTOR_H_

//#include <htype.h>
#include "rxtype.h"


#define	____HXVECTOR_TYPE___________________

typedef struct
{
	void **	rxvector;	///< rxvector array
	HINT32	size;	///< rxvector memory size
	HINT32	count;	///< item count

	/**
	 * Item을 추가할 때 호출되는 연산(Method), return 값이 rxvector array에 추가됨
	 */
	void *	(* mkItem) (void *data);

	/**
	 * Item을 삭제할 때 호출되는 연산(Method), mkItem에서 alloc이 있었으면 여기서 free하면 됨
	 */
	void *	(* rmItem) (void *data);

	/**
	 * Item을 비교할 때 호출되는 연산(Method), 만약 NULL이면 default compare 연산 호출됨

	 * @param arg1	pointer to Item (아이템의 포인터임!!! 주의!!)
	 * @param arg2	pointer to Item (아이템의 포인터임!!! 주의!!)
	 */
	HINT32	(* cmpItem)	(const void *arg1, const void *arg2);

	void *	allocSelf;	///< New를 했을 때, 자신을 alloc했으면 여기에 표시
} RxVector_t;


#define	____HXVECTOR_CompareAPIs____________________________________________________

/**
 * Item을 비교할 때 호출되는 연산(Method), 만약 NULL이면 default compare 연산 호출됨

 *
 * 위의 매크로로 아래의 함수들 정의 되어 있음 (HINT8, HINT16, HINT32, HUINT8, HUINT16, HUINT32)
 * @param arg1  pointer to Item (아이템의 포인터임!!! 주의!!)
 * @param arg2  pointer to Item (아이템의 포인터임!!! 주의!!)
 * @return	0: same, -/+
 */
HINT32	RxVector_CompareHINT8	(const void *arg1, const void *arg2);
HINT32	RxVector_CompareHINT16	(const void *arg1, const void *arg2);
HINT32	RxVector_CompareHINT32	(const void *arg1, const void *arg2);
HINT32	RxVector_CompareHUINT8	(const void *arg1, const void *arg2);
HINT32	RxVector_CompareHUINT16	(const void *arg1, const void *arg2);
HINT32	RxVector_CompareHUINT32	(const void *arg1, const void *arg2);
HINT32	RxVector_CompareSzASCII	(const void *arg1, const void *arg2);


#define	____HXVECTOR_APIs___________________

/**
 * Alloc(or Init) New MHVector_t
 *
 * @param	self	NULL이면 내부적으로 할당해서 리턴
 * @param	cmp		item을 비교할 때 호출(만약 넣지 않으면 단순 비교)
 * @return	Instance of 'MHVector_t'
 */
RxVector_t	*RxVector_New (RxVector_t *self, HINT32 (* cmp)(const void *, const void *));

/**
 * Alloc(or Init) New MHVector_t
 *
 * @param	self	NULL이면 내부적으로 할당해서 리턴
 * @param	length	pre-allocate rxvector length
 * @param	mk		item을 생성할 때 호출(생성자 역할)
 * @param	rm		item을 삭제할 때 호출(소멸자 역할)
 * @param	cmp		item을 비교할 때 호출(만약 넣지 않으면 단순 비교)
 * @return	Instance of 'MHVector_t'
 */
RxVector_t	*RxVector_NewEx  (RxVector_t *self, HINT32 length, void * (*mk)(void *), void * (*rm)(void *), HINT32 (* cmp)(const void *, const void *));

/**
 * Free(or Deinit)
 *
 * @param	self	Instance of 'MHVector_t'
 */
void		RxVector_Delete (RxVector_t *self);

/**
 * "Add" Method (Vector의 마지막에 추가함)
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	data	추가하고자 하는 데이터
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		RxVector_Add (RxVector_t *self, void *data);

/**
 * "Insert" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	n		index
 * @param	data	추가하고자 하는 데이터
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		RxVector_Insert (RxVector_t *self, HINT32 n, void *data);

/**
 * "InsertAfter" Method, sibling의 뒤에 data 추가
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	sibling	item
 * @param	data	추가하고자 하는 데이터
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		RxVector_InsertAfter (RxVector_t *self, const void *sibling, void *data);

/**
 * "InsertBefore" Method, sibling의 앞에 data 추가
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	sibling	item
 * @param	data	추가하고자 하는 데이터
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		RxVector_InsertBefore (RxVector_t *self, const void *sibling, void *data);

/**
 * "Remove" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	n		index
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		RxVector_Remove (RxVector_t *self, HINT32 n);

/**
 * "RemoveAll" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		RxVector_RemoveAll (RxVector_t *self);

/**
 * "Detach" Method
 *
 * @param	self	 Instance of 'MHVector_t'
 * @param	n		index
 * @return  detached item
 */
void *		RxVector_Detach (RxVector_t *self, HINT32 n);

/**
 * "Length" Method, Item 개수 리턴
 *
 * @param	self	Instance of 'MHVector_t'
 * @return	Item 개수
 */
HINT32		RxVector_Length	(RxVector_t *self);

/**
 * "MemLength" Method, 메모리에 할단된 Item 개수 리턴
 *
 * @param	self	Instance of 'MHVector_t'
 * @return	메모리에 할당된 item 개수
 */
HINT32		RxVector_MemLength	(RxVector_t *self);

/**
 * "Reverse" Method, rxvector를 뒤집어 줌
 *
 * @param	self	Instance of 'MHVector_t'
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		RxVector_Reverse	(RxVector_t *self);

/**
 * "Sort" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	cmp		Item 비교 함수(만약 넣지 않으면 기본 cmp함수로 비교)
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		RxVector_Sort (RxVector_t *self, HINT32 (* cmp)(const void *, const void *));

/**
 * "First" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @return	rxvector의 첫 번째 아이템
 */
void *		RxVector_First	(RxVector_t *self);

/**
 * "Last" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @return	rxvector의 마지막 아이템
 */
void *		RxVector_Last	(RxVector_t *self);

/**
 * "ItemAt" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	n		index
 * @return	rxvector의 n번째 아이템
 */
void *		RxVector_ItemAt	(RxVector_t *self, HINT32 n);

/**
 * "Replace" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	n		index
 * @param	data	new data
 * @return	old data
 */
void *		RxVector_Replace (RxVector_t *self, HINT32 n, void *data);

/**
 * "IndexOf" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	data	찾고자 하는 데이터
 * @return	data의 index (or -1, 못 찾음)
 */
HINT32		RxVector_IndexOf	(RxVector_t *self, const void *data);

/**
 * "BSearch" Method (Binary Search)
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	key		찾고자 하는 데이터 키
 * @param	cmp		Item 비교 함수(만약 넣지 않으면 기본 cmp함수로 비교)
 * @return	ERR_OK / ERR_FAIL
 */
HINT32		RxVector_BSearch (RxVector_t *self, const void *key, HINT32 (* cmp)(const void *, const void *));

/**
 * "ForEach" Method, Vector의 모든 아이템에 대해 func연산을 수행
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	func	각 아이템에 대해 수행되는 연산 (TRUE를 리턴하면, loop stop!)
 * @param	userdata	func로 들어감
 * @return	loop가 끝난 시점의 인덱스
 */
HINT32		RxVector_ForEach	(RxVector_t *self, HBOOL (* func)(void *data, void *userdata), void *userdata);

/**
 * "ToString" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	str		string buffer
 * @param	n		string buffer length
 * @return	str
 */
HUINT8 *	RxVector_ToString (RxVector_t *self, HUINT8 *str, HUINT32 n);

#endif /* HVECTOR_H_ */
