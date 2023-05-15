/***************************************************************************
 *     Copyright (c) 2001-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mstringhash.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 3/8/10 5:56p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mstringhash.h $
 * 
 * 2   3/8/10 5:56p jtna
 * SW7420-527: add support for 'include <filename>' syntax
 * 
 * 1   2/7/05 11:18p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   2/26/04 11:01a erickson
 * PR9900: added copy constructor, print method, and more api
 * documentation
 *
 ****************************************************************************/
#ifndef MSTRINGHASH_H
#define MSTRINGHASH_H

#include "mstring.h"
#include "mlist.h"

class MStringHash {
public:
	/**
	Summary:
	Create a new hash
	**/
	MStringHash(bool caseSensitive = true);

	/**
	Summary:
	Create a new hash and copy the contents of another
	**/
	MStringHash(const MStringHash &hash);

	/**
	Summary:
	Add a value to the hash.
	Description:
	If the name is already used, the value is replaced.
	*/
	void add(const char *name, const char *value, int valuelen = -1);

	/**
	Summary:
	Add a value to the hash, but used both namelen and optional valuelen.
	Description:
	This allows you to add a name/value pair by clipping a portion of a string, not
	taking the entire string.
	**/
	void add(const char *name, int namelen, const char *value, int valuelen = -1);

	/**
	Summary:
	Remove a value from the hash.
	**/
	bool remove(const char *name);

	/**
	Summary:
	Remove all values from the hash.
	**/
	void clear() {_list.clear();}


	/**
	Summary:
	Find a value in the hash.
	**/
	MString &get(const char *name) const;

	/**
	Summary:
	Position the hash to its first element.

	Example code:
		for (hash.first();hash.current();hash.next()) {
			printf("%s=%s\n", hash.name().s(), hash.value().s());
		}
	*/
	bool first() {return _list.first() ? true:false;}

	/**
	Summary:
	Go to the next element in the hash.
	**/
	bool next() {return _list.next() ? true:false;}

	/**
	Summary:
	Check if the hash is currently on an element.
	**/
	bool current() const {return _list.current() ? true:false;}

	/**
	Summary:
	Return the name of the current element.
	**/
	MString &name() const;

	/**
	Summary:
	Return the value of the current element.
	**/
	MString &value() const;

#ifdef I_PREFER_SLOW_CODE
	MString &name(int index);
	MString &value(int index);
	int total() const {return _list.total();}
#endif

	/**
	Summary:
	Read name=value from a file. Doesn't clear the hash first.
	Returns true on success.
	*/
	bool read(const char *filename, bool recursive = false);


	/**
	Summary:
	Print hash contents to stdout for debug
	*/
	void print() const;

protected:
	struct Node {
		MString name,data;
		Node(const char *n, int nlen, const char *d, int len = -1) {
			name.strncpy(n,nlen);
			data.strncpy(d,len);
		}
	};
	Node *find(const char *name, int len = -1) const;
	MAutoList<Node> _list;
	bool _caseSensitive;
};

#endif // MSTRINGHASH_H

