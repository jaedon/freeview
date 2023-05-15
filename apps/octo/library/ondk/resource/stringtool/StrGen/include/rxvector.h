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
	 * Item�� �߰��� �� ȣ��Ǵ� ����(Method), return ���� rxvector array�� �߰���
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
} RxVector_t;


#define	____HXVECTOR_CompareAPIs____________________________________________________

/**
 * Item�� ���� �� ȣ��Ǵ� ����(Method), ���� NULL�̸� default compare ���� ȣ���

 *
 * ���� ��ũ�η� �Ʒ��� �Լ��� ���� �Ǿ� ���� (HINT8, HINT16, HINT32, HUINT8, HUINT16, HUINT32)
 * @param arg1  pointer to Item (�������� ��������!!! ����!!)
 * @param arg2  pointer to Item (�������� ��������!!! ����!!)
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
 * @param	self	NULL�̸� ���������� �Ҵ��ؼ� ����
 * @param	cmp		item�� ���� �� ȣ��(���� ���� ������ �ܼ� ��)
 * @return	Instance of 'MHVector_t'
 */
RxVector_t	*RxVector_New (RxVector_t *self, HINT32 (* cmp)(const void *, const void *));

/**
 * Alloc(or Init) New MHVector_t
 *
 * @param	self	NULL�̸� ���������� �Ҵ��ؼ� ����
 * @param	length	pre-allocate rxvector length
 * @param	mk		item�� ������ �� ȣ��(������ ����)
 * @param	rm		item�� ������ �� ȣ��(�Ҹ��� ����)
 * @param	cmp		item�� ���� �� ȣ��(���� ���� ������ �ܼ� ��)
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
 * "Add" Method (Vector�� �������� �߰���)
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	data	�߰��ϰ��� �ϴ� ������
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		RxVector_Add (RxVector_t *self, void *data);

/**
 * "Insert" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	n		index
 * @param	data	�߰��ϰ��� �ϴ� ������
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		RxVector_Insert (RxVector_t *self, HINT32 n, void *data);

/**
 * "InsertAfter" Method, sibling�� �ڿ� data �߰�
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	sibling	item
 * @param	data	�߰��ϰ��� �ϴ� ������
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		RxVector_InsertAfter (RxVector_t *self, const void *sibling, void *data);

/**
 * "InsertBefore" Method, sibling�� �տ� data �߰�
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	sibling	item
 * @param	data	�߰��ϰ��� �ϴ� ������
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
 * "Length" Method, Item ���� ����
 *
 * @param	self	Instance of 'MHVector_t'
 * @return	Item ����
 */
HINT32		RxVector_Length	(RxVector_t *self);

/**
 * "MemLength" Method, �޸𸮿� �Ҵܵ� Item ���� ����
 *
 * @param	self	Instance of 'MHVector_t'
 * @return	�޸𸮿� �Ҵ�� item ����
 */
HINT32		RxVector_MemLength	(RxVector_t *self);

/**
 * "Reverse" Method, rxvector�� ������ ��
 *
 * @param	self	Instance of 'MHVector_t'
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		RxVector_Reverse	(RxVector_t *self);

/**
 * "Sort" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	cmp		Item �� �Լ�(���� ���� ������ �⺻ cmp�Լ��� ��)
 * @return	ERR_OK / ERR_FAIL
 */
HERROR		RxVector_Sort (RxVector_t *self, HINT32 (* cmp)(const void *, const void *));

/**
 * "First" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @return	rxvector�� ù ��° ������
 */
void *		RxVector_First	(RxVector_t *self);

/**
 * "Last" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @return	rxvector�� ������ ������
 */
void *		RxVector_Last	(RxVector_t *self);

/**
 * "ItemAt" Method
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	n		index
 * @return	rxvector�� n��° ������
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
 * @param	data	ã���� �ϴ� ������
 * @return	data�� index (or -1, �� ã��)
 */
HINT32		RxVector_IndexOf	(RxVector_t *self, const void *data);

/**
 * "BSearch" Method (Binary Search)
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	key		ã���� �ϴ� ������ Ű
 * @param	cmp		Item �� �Լ�(���� ���� ������ �⺻ cmp�Լ��� ��)
 * @return	ERR_OK / ERR_FAIL
 */
HINT32		RxVector_BSearch (RxVector_t *self, const void *key, HINT32 (* cmp)(const void *, const void *));

/**
 * "ForEach" Method, Vector�� ��� �����ۿ� ���� func������ ����
 *
 * @param	self	Instance of 'MHVector_t'
 * @param	func	�� �����ۿ� ���� ����Ǵ� ���� (TRUE�� �����ϸ�, loop stop!)
 * @param	userdata	func�� ��
 * @return	loop�� ���� ������ �ε���
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
