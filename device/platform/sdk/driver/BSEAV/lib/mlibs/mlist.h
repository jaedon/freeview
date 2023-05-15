/***************************************************************************
 *     Copyright (c) 2001-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mlist.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 1/12/12 6:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mlist.h $
 * 
 * 4   1/12/12 6:50p tokushig
 * SW7405-5581: fix cast in index()
 * 
 * 3   3/6/08 1:32p tokushig
 * PR20349: added warning comments to MAutoList and MHash class
 * declarations
 * 
 * 1   2/7/05 11:14p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/4   4/30/04 12:04p erickson
 * PR10832: allow const lookup of entries using get() and []
 *
 * Irvine_BSEAVSW_Devel/3   2/26/04 10:59a erickson
 * PR9900: Create MListItr class
 *
 ****************************************************************************/
#ifndef MLIST_H
#define MLIST_H

#include <stdlib.h> //define NULL
#include <stdio.h>
#include "mvoidlist.h"

template<class T>
class MListItr : MVoidListItr {
public:
	MListItr(const MVoidList *list) : MVoidListItr(list) {}

	T *first() {return (T*)MVoidListItr::first();}
	T *next() {return (T*)MVoidListItr::next();}
	T *prev() {return (T*)MVoidListItr::prev();}
	T *last() {return (T*)MVoidListItr::last();}
	T *at(int i) {return (T*)MVoidListItr::at(i);}
	int index(T *data) {return (int)MVoidListItr::index(data);}
	T *current() const {return (T*)MVoidListItr::current();}
};

template<class T>
class MList : public MVoidList {
public:
	/* CAUTION: see comment for MAutoList concerning autodelete=true */
	MList(bool autodelete = false) : MVoidList(autodelete) {}
	~MList() { clear(); }

	void add(T *data) {MVoidList::add(data);}
	void insert(int index, T*data) {MVoidList::insert(index, data);}
	T *remove(T *data) {return (T*)MVoidList::remove(data);}
	T *remove(int i) {return (T*)MVoidList::remove(i);}
	T *remove() {return (T*)MVoidList::remove();}
	int index(T *data) {return MVoidList::index(data);}
	T *get(int i) const {return (T*)MVoidList::get(i);}

	int total() const {return _total;}

	T *first() {return (T*)MVoidList::first();}
	T *next() {return (T*)MVoidList::next();}
	T *prev() {return (T*)MVoidList::prev();}
	T *last() {return (T*)MVoidList::last();}
	T *at(int i) {return (T*)MVoidList::at(i);}
	T *current() const {return (T*)MVoidList::current();}

	T *operator [](int i) const {return get(i);}

	void sort(int (*compar)(T *d1, T *d2)) {
		MVoidList::sort((int (*)(void *,void *))compar);
	}

protected:
	void deleteData(void *data) 
	{
		if (data) 
		{ 
			delete (T*)data; 
			data = NULL;
		} 
	}
};

/**
* MAutoList is a list which defaults to autodelete.
*/

/* !!!!!!!!!! C A U T I O N !!!!!!!!!! */
/* 
   ONLY DYNAMICALLY ALLOCATED DATA MAY BE ADDED TO MAUTOLIST. THAT DATA WILL 
   THEN BELONG TO THE MAUTOLIST OBJECT, AND WILL BE AUTOMATICALLY DELETED WHEN 
   THE MAUTOLIST IS DELETED.  DO NOT DELETE THE DYNAMICALLY ALLOCATED DATA 
   ELSEWHERE OR ***MEMORY CORRUPTION*** WILL RESULT!

   CARE MUST ALSO BE TAKEN NOT TO ADD THE SAME DYNAMICALLY ALLOCATED DATA TO
   MULTIPLE MAUTOLISTS (OR ANY SUBCLASSES SUCH AS MHASH).
*/
template <class T>
class MAutoList : public MList<T> {
public:
	MAutoList() : MList<T>(true) {}
};

//////////////////////////////////////////////////


#endif
