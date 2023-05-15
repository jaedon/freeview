/***************************************************************************
 *     Copyright (c) 2001-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mvoidlist.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 7/26/07 8:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mvoidlist.cpp $
 * 
 * 3   7/26/07 8:40p erickson
 * PR33552: support broader range of strcmp functions
 * 
 * 2   4/8/05 7:32p erickson
 * PR14701: fix possible null dereference
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
#include "mvoidlist.h"

MVoidList::MVoidList(bool autodelete) : _itr(this) {
	setAutoDelete(autodelete);
	_first = _last = NULL;
	_itr.first(); // must reset after _first is initialized
	_total = 0;
}

void MVoidList::add(void *data) {
	MVoidListNode *node = new MVoidListNode(data);
	if (_last) {
		_last->next = node;
		node->prev = _last;
		_last = node;
	}
	else {
		_first = _last = node;
	}
	_itr._current = node;
	_total++;
}

void MVoidList::insert(int index, void *data) {
	at(index);
	MVoidListNode *current = (MVoidListNode *)_itr._current;
	if (current) {
		MVoidListNode *node = new MVoidListNode(data);
		if (current->prev) {
			current->prev->next = node;
			node->prev = current->prev;
		}
		else {
			_first = node;
		}
		current->prev = node;
		node->next = current;
		_itr._current = node;
		_total++;
	}
	else {
		add(data);
	}
}

void *MVoidList::remove(void *data) {
	if (index(data) != -1)
		return remove();
	else
		return NULL;
}

void *MVoidList::remove(int i) {
	if (at(i))
		return remove();
	else
		return NULL;
}

void *MVoidList::remove() {
	MVoidListNode *current = (MVoidListNode *)_itr._current;
	if (current) {
		MVoidListNode *temp = current->next;
//		if (!temp)
//			temp = _current->prev;

		if (current->prev) {
			current->prev->next = current->next;
		}
		else {
			_first = current->next;
		}
		if (current->next) {
			current->next->prev = current->prev;
		}
		else {
			_last = current->prev;
		}
		//NOTE: I don't delete the data here
		void *data = current->data;
		delete current;
		_total--;

		_itr._current = temp;
		return data;
	}
	return NULL;
}

void MVoidList::clear() {
	while (_first) {
		if (_autoDelete && _first->data)
			deleteData(_first->data);
		remove(0);
	}
	_itr._current = NULL;
}

void MVoidList::sort(int (*compar)(void *d1, void *d2)) {
	MVoidListNode *last = NULL;
	while (last != _first) {
		MVoidListNode *d;
		for (d = _first; d && d->next != last; d = d->next)
			if ((*compar)(d->data, d->next->data) > 0) {
				void *temp = d->data;
				d->data = d->next->data;
				d->next->data = temp;
			}
		last = d;
	}
}

void *MVoidList::get(int i) const
{
	if (i < 0) {
		return NULL;
	}
	else {
		MVoidListNode *n = _first;
		while (i-- && n)
			n = n->next;
		return n?n->data:NULL;
	}
}

////////////////////////////////////////////////

MVoidListItr::MVoidListItr(const MVoidList *list)
{
	_list = list;
	_current = _list->_first;
}

void *MVoidListItr::first()
{
	_current = _list->_first;
	return current();
}

void *MVoidListItr::next()
{
	if (_current)
		_current = _current->next;
	return current();
}

void *MVoidListItr::prev()
{
	if (_current)
		_current = _current->prev;
	return current();
}

void *MVoidListItr::last()
{
	_current = _list->_last;
	return current();
}

void *MVoidListItr::at(int i)
{
	if (i < 0) {
		_current = NULL;
	}
	else {
		_current = _list->_first;
		while (i-- && _current)
			_current = _current->next;
	}
	return current();
}

int MVoidListItr::index(void *data) {
	int i = 0;
	first();
	while (_current) {
		if (_current->data == data) {
			return i;
		}
		_current = _current->next;
		i++;
	}
	return -1;
}

