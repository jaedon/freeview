/***************************************************************************
 *     Copyright (c) 2001-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mvoidlist.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 7/17/12 4:37p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mvoidlist.h $
 * 
 * 2   7/17/12 4:37p gskerl
 * SW7346-853: Merged from branch SW7346-853 to main.
 * 
 * SW7346-853/1   6/20/12 6:25p gskerl
 * SW7346-853: Fixed compile warning about unused arguments.
 * 
 * 1   2/7/05 11:20p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   4/30/04 12:04p erickson
 * PR10832: allow const lookup of entries using get() and []
 * 
 * Irvine_BSEAVSW_Devel/2   2/26/04 10:59a erickson
 * PR9900: Create MListItr class
 *
 ****************************************************************************/
#ifndef MVOIDLIST_H
#define MVOIDLIST_H

#include <stdlib.h>

class MVoidList;

struct MVoidListNode {
	MVoidListNode *prev;
	MVoidListNode *next;
	void *data;
	MVoidListNode(void *d) {data=d;prev=next=NULL;}
};

/**
Summary:
Provides an iterator to search an MVoidList without modifying the state of the MVoidList.

Description:
If you remove an element that is currently pointed to my MVoidList, you are in an
undefined state. You must call first(), last() or at() to reset state.
**/
class MVoidListItr {
friend class MVoidList;
public:
	MVoidListItr(const MVoidList *list);

	void *first();
	void *next();
	void *prev();
	void *last();
	void *at(int i);
	int index(void *data);
	void *current() const {return _current?_current->data:NULL;}

protected:
	const MVoidList *_list;
	const MVoidListNode *_current;
};

/**
* List of void*. Note that autodelete doesn't do anything
* because it's type-agnostic. But it's there because of
* MList<T>.
*/
class MVoidList {
friend class MVoidListItr;
public:
	MVoidList(bool autodelete = false);
	virtual ~MVoidList() {clear();}

	void setAutoDelete(bool ad) {_autoDelete = ad;}
	void add(void *data);
	void insert(int index, void *data);
	void *remove(void *data);
	void *remove(int i);
	void *remove();
	void clear();

	/**
	Get a node at a certain index without changing the current index. 
	**/
	void *get(int i) const;
	int total() const {return _total;}

	/**
	The following functions use an internal MVoidListItr and
	will relocated it.
	**/
	void *first() {return _itr.first();}
	void *next() {return _itr.next();}
	void *prev() {return _itr.prev();}
	void *last() {return _itr.last();}
	void *at(int i) {return _itr.at(i);}
	void *current() const {return _itr.current();}
	int index(void *data) {return _itr.index(data);}

	/**
	* Sort in ascending order (currently using bubble sort).
	* If compar returns -1, d1 < d2.
	* If compar returns 1, d1 > d2.
	* If compar returns 0, d1 == d2.
	**/
	void sort(int (*compar)(void *d1, void *d2));

protected:
	MVoidListNode *_first;
	MVoidListNode *_last;
	MVoidListItr _itr;
	bool _autoDelete;
	int _total;

	virtual void deleteData(void *n) {n=n;}  /* "n=n" to avoid compiler warning */
};

#endif //MVOIDLIST_H
