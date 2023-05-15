/*************************************************************
 * @file		hxvector.h
 * @date		2011/01/17
 * @author	Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header, related with vector system
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
	HxVector_t	*vec;

	vec = HLIB_VECTOR_NewEx(NULL,
				10,		// pre-allocate vector length
				strdup,	// called when a item is created.
				free,	// called when a item is desroyed.
				compareItem	// comparison function
			);
	assert(vec != NULL);

	puts("1. Add()");
	{
		HLIB_VECTOR_Add(vec, "Hello");
		HLIB_VECTOR_Add(vec, "HxVector_t");
		HLIB_VECTOR_ForEach(vec, printItem, stdout);
	}

	puts("2. Insert()");
	{
		HLIB_VECTOR_InsertAfter(vec, "Hello", "World");
		HLIB_VECTOR_ForEach(vec, printItem, stdout);
	}

	puts("3. Sort()");
	{
		HLIB_VECTOR_Sort(vec, NULL);
		HLIB_VECTOR_ForEach(vec, printItem, stdout);
	}

	HLIB_VECTOR_Delete(vec);
}

 [Expected Result]
1. Add()
	Hello
	HxVector_t
2. Insert()
	Hello
	World
	HxVector_t
3. Sort()
	HxVector_t
	Hello
	World
 ************************************************************************************/

#ifndef HVECTOR_H_
#define HVECTOR_H_

//#include <htype.h>
#include "_hlib_types.h"


#define	____HXVECTOR_TYPE___________________

typedef struct
{
	void **	vector;	///< vector array
	HINT32	size;	///< vector memory size
	HINT32	count;	///< item count

	/**
	 * Item을 추가할 때 호출되는 연산(Method), return 값이 vector array에 추가됨
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
#if defined(CONFIG_DEBUG)
	HCHAR	* debugText;
	HUINT32	debugLine;
#endif	
	
} HxVector_t;


#define	____HXVECTOR_CompareAPIs____________________________________________________

/**
 * Item을 비교할 때 호출되는 연산(Method), 만약 NULL이면 default compare 연산 호출됨

 *
 * 위의 매크로로 아래의 함수들 정의 되어 있음 (HINT8, HINT16, HINT32, HUINT8, HUINT16, HUINT32)
 * @param arg1  pointer to Item (아이템의 포인터임!!! 주의!!)
 * @param arg2  pointer to Item (아이템의 포인터임!!! 주의!!)
 * @return	0: same, -/+
 */
HINT32	HLIB_VECTOR_CompareHINT8	(const void *arg1, const void *arg2);
HINT32	HLIB_VECTOR_CompareHINT16	(const void *arg1, const void *arg2);
HINT32	HLIB_VECTOR_CompareHINT32	(const void *arg1, const void *arg2);
HINT32	HLIB_VECTOR_CompareHUINT8	(const void *arg1, const void *arg2);
HINT32	HLIB_VECTOR_CompareHUINT16	(const void *arg1, const void *arg2);
HINT32	HLIB_VECTOR_CompareHUINT32	(const void *arg1, const void *arg2);
HINT32	HLIB_VECTOR_CompareSzASCII	(const void *arg1, const void *arg2);


#define	____HXVECTOR_MACRO_APIs___________________

#define	HxVECTOR_EACH(vector,type,item,expr)	\
	do {\
		HINT32	__i, __n;\
		__n = HLIB_VECTOR_Length(vector);\
		for (__i = 0 ; __i < __n ; __i++) {\
			item = (type)HLIB_VECTOR_ItemAt(vector, __i);\
			expr;\
		}\
	} while (0)

#define HxVECTOR_DELETE_IF_EMPTY(vector)	\
	do {\
		if (HLIB_VECTOR_Length(vector) == 0) {\
			HLIB_VECTOR_Delete(vector);\
			vector = NULL;\
		}\
	} while (0)

#define	HxVECTOR_IS_EMPTY(vector)	(vector == NULL || HLIB_VECTOR_Length(vector) == 0)

#define	____HXVECTOR_APIs___________________

/**
 * Alloc(or Init) New HxVector_t
 *
 * @param	self	NULL이면 내부적으로 할당해서 리턴
 * @param	cmp		item을 비교할 때 호출(만약 넣지 않으면 단순 비교)
 * @return	Instance of 'HxVector_t'
 */
HxVector_t	*_HLIB_VECTOR_New (HxVector_t *self, HINT32 (* cmp)(const void *, const void *), const HCHAR * file, HUINT32 line);

/**
 * Alloc(or Init) New HxVector_t
 *
 * @param	self	NULL이면 내부적으로 할당해서 리턴
 * @param	length	pre-allocate vector length
 * @param	mk		item을 생성할 때 호출(생성자 역할)
 * @param	rm		item을 삭제할 때 호출(소멸자 역할)
 * @param	cmp		item을 비교할 때 호출(만약 넣지 않으면 단순 비교)
 * @return	Instance of 'HxVector_t'
 */
HxVector_t	*_HLIB_VECTOR_NewEx  (HxVector_t *self, HINT32 length, void * (*mk)(void *), void * (*rm)(void *), HINT32 (* cmp)(const void *, const void *), const HCHAR * file, HUINT32 line);
HxVector_t	*_HLIB_VECTOR_NewEasy (HINT32 length, HxFreeFunc_t rm, HxCompareFunc_t cmp, const HCHAR * file, HUINT32 line);


#define HLIB_VECTOR_New(self, cmp)						_HLIB_VECTOR_New (self, cmp, __FILE__, __LINE__)
#define HLIB_VECTOR_NewEx(self, length, mk, rm, cmp)	_HLIB_VECTOR_NewEx  (self, length, mk, rm, cmp, __FILE__, __LINE__)
#define HLIB_VECTOR_NewEasy(length, rm, cmp)			_HLIB_VECTOR_NewEasy (length, rm, cmp, __FILE__, __LINE__)

/**
 * Free(or Deinit)
 *
 * @param	self	Instance of 'HxVector_t'
 */
void		HLIB_VECTOR_Delete (HxVector_t *self);

/**
 * "Add" Method (Vector의 마지막에 추가함)
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	data	추가하고자 하는 데이터
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_Add (HxVector_t *self, void *data);

/**
 * "Append" Method (Vector의 마지막에 추가함)
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	other	추가하고자 하는 벡터
 * @param	each	추가할 때 각 아이템별로 호출되는 콜백
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_Append (HxVector_t *self, HxVector_t *other, void *(*each)(void *));

/**
 * "Insert" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	n		index
 * @param	data	추가하고자 하는 데이터
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_Insert (HxVector_t *self, HINT32 n, void *data);

/**
 * "InsertAfter" Method, sibling의 뒤에 data 추가
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	sibling	item
 * @param	data	추가하고자 하는 데이터
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_InsertAfter (HxVector_t *self, const void *sibling, void *data);

/**
 * "InsertBefore" Method, sibling의 앞에 data 추가
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	sibling	item
 * @param	data	추가하고자 하는 데이터
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_InsertBefore (HxVector_t *self, const void *sibling, void *data);

/**
 * "Remove" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	n		index
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_Remove (HxVector_t *self, HINT32 n);

/**
 * "RemoveAll" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_RemoveAll (HxVector_t *self);

/**
 * "Detach" Method
 *
 * @param	self	 Instance of 'HxVector_t'
 * @param	n		index
 * @return  detached item
 */
void *		HLIB_VECTOR_Detach (HxVector_t *self, HINT32 n);

/**
 * "Length" Method, Item 개수 리턴
 *
 * @param	self	Instance of 'HxVector_t'
 * @return	Item 개수
 */
HINT32		HLIB_VECTOR_Length	(HxVector_t *self);

/**
 * "MemLength" Method, 메모리에 할단된 Item 개수 리턴
 *
 * @param	self	Instance of 'HxVector_t'
 * @return	메모리에 할당된 item 개수
 */
HINT32		HLIB_VECTOR_MemLength	(HxVector_t *self);

/**
 * "Reverse" Method, vector를 뒤집어 줌
 *
 * @param	self	Instance of 'HxVector_t'
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_Reverse	(HxVector_t *self);

/**
 * "Sort" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	cmp		Item 비교 함수(만약 넣지 않으면 기본 cmp함수로 비교)
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_Sort (HxVector_t *self, HINT32 (* cmp)(const void *, const void *));

HERROR		HLIB_VECTOR_SortEx (HxVector_t *self, HINT32 (* cmp)(const void *, const void *, void *), void *userdata);

/**
 * "First" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @return	vector의 첫 번째 아이템
 */
void *		HLIB_VECTOR_First	(HxVector_t *self);

/**
 * "Last" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @return	vector의 마지막 아이템
 */
void *		HLIB_VECTOR_Last	(HxVector_t *self);

/**
 * "ItemAt" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	n		index
 * @return	vector의 n번째 아이템
 */
void *		HLIB_VECTOR_ItemAt	(HxVector_t *self, HINT32 n);

/**
 * "Replace" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	n		index
 * @param	data	new data
 * @return	old data
 */
void *		HLIB_VECTOR_Replace (HxVector_t *self, HINT32 n, void *data);

/**
 * "IndexOf" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	data	찾고자 하는 데이터
 * @return	data의 index (or -1, 못 찾음)
 */
HINT32		HLIB_VECTOR_IndexOf	(HxVector_t *self, const void *data);

/**
 * "BSearch" Method (Binary Search)
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	key		찾고자 하는 데이터 키
 * @param	cmp		Item 비교 함수(만약 넣지 않으면 기본 cmp함수로 비교)
 * @return	ERR_OK / ERR_FAIL
 */
HINT32		HLIB_VECTOR_BSearch (HxVector_t *self, const void *key, HINT32 (* cmp)(const void *, const void *));

/**
 * "ForEach" Method, Vector의 모든 아이템에 대해 func연산을 수행
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	func	각 아이템에 대해 수행되는 연산 (TRUE를 리턴하면, loop stop!)
 * @param	userdata	func로 들어감
 * @return	loop가 끝난 시점의 인덱스
 */
HINT32		HLIB_VECTOR_ForEach	(HxVector_t *self, HBOOL (* func)(void *data, void *userdata), void *userdata);

void *		HLIB_VECTOR_Find (HxVector_t *self, HBOOL (* filter)(void *data, void *userdata), void *userdata);
HINT32		HLIB_VECTOR_RemoveIf (HxVector_t *self, HBOOL (* filter)(void *data, void *userdata), void *userdata);

/**
 * Clone
 *
 * @param	self	Instance of 'HxVector_t'
 * @return	clone~!
 */
HxVector_t *HLIB_VECTOR_Clone (HxVector_t *self);
HxVector_t *HLIB_VECTOR_CloneEx (HxVector_t *self, void *(* cloneItem)(void *));

/**
 * "ToString" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	str		string buffer
 * @param	n		string buffer length
 * @return	str
 */
HUINT8 *	HLIB_VECTOR_ToString (HxVector_t *self, HUINT8 *str, HUINT32 n);

#endif /* HVECTOR_H_ */
