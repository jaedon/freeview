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
	 * Item�� �߰��� �� ȣ��Ǵ� ����(Method), return ���� vector array�� �߰���
	 */
	void *	(* mkItem) (void *data);

	/**
	 * Item�� ������ �� ȣ��Ǵ� ����(Method), mkItem���� alloc�� �־����� ���⼭ free�ϸ� ��
	 */
	void *	(* rmItem) (void *data);

	/**
	 * Item�� ���� �� ȣ��Ǵ� ����(Method), ���� NULL�̸� default compare ���� ȣ���

	 * @param arg1	pointer to Item (�������� ��������!!! ����!!)
	 * @param arg2	pointer to Item (�������� ��������!!! ����!!)
	 */
	HINT32	(* cmpItem)	(const void *arg1, const void *arg2);

	void *	allocSelf;	///< New�� ���� ��, �ڽ��� alloc������ ���⿡ ǥ��
#if defined(CONFIG_DEBUG)
	HCHAR	* debugText;
	HUINT32	debugLine;
#endif	
	
} HxVector_t;


#define	____HXVECTOR_CompareAPIs____________________________________________________

/**
 * Item�� ���� �� ȣ��Ǵ� ����(Method), ���� NULL�̸� default compare ���� ȣ���

 *
 * ���� ��ũ�η� �Ʒ��� �Լ��� ���� �Ǿ� ���� (HINT8, HINT16, HINT32, HUINT8, HUINT16, HUINT32)
 * @param arg1  pointer to Item (�������� ��������!!! ����!!)
 * @param arg2  pointer to Item (�������� ��������!!! ����!!)
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
 * @param	self	NULL�̸� ���������� �Ҵ��ؼ� ����
 * @param	cmp		item�� ���� �� ȣ��(���� ���� ������ �ܼ� ��)
 * @return	Instance of 'HxVector_t'
 */
HxVector_t	*_HLIB_VECTOR_New (HxVector_t *self, HINT32 (* cmp)(const void *, const void *), const HCHAR * file, HUINT32 line);

/**
 * Alloc(or Init) New HxVector_t
 *
 * @param	self	NULL�̸� ���������� �Ҵ��ؼ� ����
 * @param	length	pre-allocate vector length
 * @param	mk		item�� ������ �� ȣ��(������ ����)
 * @param	rm		item�� ������ �� ȣ��(�Ҹ��� ����)
 * @param	cmp		item�� ���� �� ȣ��(���� ���� ������ �ܼ� ��)
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
 * "Add" Method (Vector�� �������� �߰���)
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	data	�߰��ϰ��� �ϴ� ������
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_Add (HxVector_t *self, void *data);

/**
 * "Append" Method (Vector�� �������� �߰���)
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	other	�߰��ϰ��� �ϴ� ����
 * @param	each	�߰��� �� �� �����ۺ��� ȣ��Ǵ� �ݹ�
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_Append (HxVector_t *self, HxVector_t *other, void *(*each)(void *));

/**
 * "Insert" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	n		index
 * @param	data	�߰��ϰ��� �ϴ� ������
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_Insert (HxVector_t *self, HINT32 n, void *data);

/**
 * "InsertAfter" Method, sibling�� �ڿ� data �߰�
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	sibling	item
 * @param	data	�߰��ϰ��� �ϴ� ������
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_InsertAfter (HxVector_t *self, const void *sibling, void *data);

/**
 * "InsertBefore" Method, sibling�� �տ� data �߰�
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	sibling	item
 * @param	data	�߰��ϰ��� �ϴ� ������
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
 * "Length" Method, Item ���� ����
 *
 * @param	self	Instance of 'HxVector_t'
 * @return	Item ����
 */
HINT32		HLIB_VECTOR_Length	(HxVector_t *self);

/**
 * "MemLength" Method, �޸𸮿� �Ҵܵ� Item ���� ����
 *
 * @param	self	Instance of 'HxVector_t'
 * @return	�޸𸮿� �Ҵ�� item ����
 */
HINT32		HLIB_VECTOR_MemLength	(HxVector_t *self);

/**
 * "Reverse" Method, vector�� ������ ��
 *
 * @param	self	Instance of 'HxVector_t'
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_Reverse	(HxVector_t *self);

/**
 * "Sort" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	cmp		Item �� �Լ�(���� ���� ������ �⺻ cmp�Լ��� ��)
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		HLIB_VECTOR_Sort (HxVector_t *self, HINT32 (* cmp)(const void *, const void *));

HERROR		HLIB_VECTOR_SortEx (HxVector_t *self, HINT32 (* cmp)(const void *, const void *, void *), void *userdata);

/**
 * "First" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @return	vector�� ù ��° ������
 */
void *		HLIB_VECTOR_First	(HxVector_t *self);

/**
 * "Last" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @return	vector�� ������ ������
 */
void *		HLIB_VECTOR_Last	(HxVector_t *self);

/**
 * "ItemAt" Method
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	n		index
 * @return	vector�� n��° ������
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
 * @param	data	ã���� �ϴ� ������
 * @return	data�� index (or -1, �� ã��)
 */
HINT32		HLIB_VECTOR_IndexOf	(HxVector_t *self, const void *data);

/**
 * "BSearch" Method (Binary Search)
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	key		ã���� �ϴ� ������ Ű
 * @param	cmp		Item �� �Լ�(���� ���� ������ �⺻ cmp�Լ��� ��)
 * @return	ERR_OK / ERR_FAIL
 */
HINT32		HLIB_VECTOR_BSearch (HxVector_t *self, const void *key, HINT32 (* cmp)(const void *, const void *));

/**
 * "ForEach" Method, Vector�� ��� �����ۿ� ���� func������ ����
 *
 * @param	self	Instance of 'HxVector_t'
 * @param	func	�� �����ۿ� ���� ����Ǵ� ���� (TRUE�� �����ϸ�, loop stop!)
 * @param	userdata	func�� ��
 * @return	loop�� ���� ������ �ε���
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
