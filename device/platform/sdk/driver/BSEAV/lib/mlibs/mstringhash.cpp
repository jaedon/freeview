/***************************************************************************
 *     Copyright (c) 2001-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mstringhash.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 3/8/10 5:56p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mstringhash.cpp $
 * 
 * 3   3/8/10 5:56p jtna
 * SW7420-527: add support for 'include <filename>' syntax
 * 
 * 2   3/27/07 6:11p rjlewis
 * PR29124: Remove leading and trailing whitespace from both key and
 * parameter of config file entries.
 * 
 * 1   2/7/05 11:18p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   3/5/04 2:41p erickson
 * PR9883: use MListItr
 *
 * Irvine_BSEAVSW_Devel/2   2/26/04 11:00a erickson
 * PR9900: added copy constructor, print method, and more api
 * documentation
 *
 ****************************************************************************/
#include "mstringhash.h"
#include "mstringlist.h"

static MString stub;

MStringHash::MStringHash(bool caseSensitive) {
	_caseSensitive = caseSensitive;
}

MStringHash::MStringHash(const MStringHash &hash) {
	_caseSensitive = hash._caseSensitive;

	/* copy the hash list */
	MListItr<Node> itr(&hash._list);
	for (;itr.current();itr.next()) {
		add(itr.current()->name, itr.current()->data);
	}
}

MStringHash::Node *MStringHash::find(const char *name, int len) const {
	MListItr<Node> itr(&_list);
	for (Node *n = itr.first(); n; n = itr.next()) {
		if (_caseSensitive && !n->name.strncmp(name,len) ||
			!_caseSensitive && !n->name.strncasecmp(name, len))
			return n;
	}
	return NULL;
}

void MStringHash::add(const char *name, int namelen, const char *value, int len) {
	Node *node = find(name, namelen);
	if (node)
		node->data.strncpy(value,len);
	else
		_list.add(new Node(name, namelen, value, len));
}

void MStringHash::add(const char *name, const char *value, int len) {
	add(name, -1, value, len);
}

MString &MStringHash::get(const char *name) const {
	Node *node = find(name);
	if (node)
		return node->data;
	else
		return stub;
}

#ifdef I_PREFER_SLOW_CODE
MString &MStringHash::name(int index) {
	Node *n = _list[index];
	if (n)
		return n->name;
	else
		return stub;
}

MString &MStringHash::value(int index) {
	Node *n = _list[index];
	if (n)
		return n->data;
	else
		return stub;
}
#endif

MString &MStringHash::name() const {
	Node *n = _list.current();
	if (n)
		return n->name;
	else
		return stub;
}

MString &MStringHash::value() const {
	Node *n = _list.current();
	if (n)
		return n->data;
	else
		return stub;
}

bool MStringHash::remove(const char *name) {
	if (find(name)) {
		_list.remove();
		return true;
	}
	else
		return false;
}

#include <ctype.h> /*for isspace*/

bool MStringHash::read(const char *filename, bool recursive) {
	MStringList list;
	if (!readFile(filename, list))
		return false;
	int line=1;
	for (const char *s = list.first(); s; s = list.next(), line++) {
		bool added = false;
		// '#' begins line comments (ignore everything after the #)
		char *x = strchr(s, '#');
		if (x) *x = 0;
        
		// remove leading and trailing whitespace from the input
		while (*s && isspace(*s)) s++;
		if (*s) { x = (char*)&s[strlen(s)-1]; while (x != s && isspace(*x)) x--; if (*x) x[1] = 0; }
		// is there anything left after striping the whitespace?  Ignore blank lines
		if (!*s) continue;

		if (recursive) {
			if (strncmp(s, "include ", 8)==0 && strlen(s)>8) {
				char temp[64], *a;
				strncpy(temp, s, 64);
				a = &temp[8];
				/* remove leading whitespace */
				while (*a && isspace(*a)) a++;
				/* remove surrounding " " or < > */
				if ((a[0] == '"' && a[strlen(a)-1] == '"') || (a[0] == '<' && a[strlen(a)-1] == '>')) { 
					a++; 
					a[strlen(a)-1] = 0; 
				}
				read(a, false); /* allow only one level of recursion to prevent circular includes */
				added = true;
			}
		}

		// split the name/value pair and clean them both up
		x = strchr(s, '='); /*marks the spot*/
		if (x && x != s) {
			char *b = x, *a = x;
			// remove trailing whitespace from key and leading whitespace from data
			// note: the '=' tests below are important to skip over the '=' (starting point)
			while (*a && (*a == '=' || isspace(*a))) a++;
			while (b != s && (*b == '=' || isspace(*b))) b--;
			*(b+1) = 0;
			// If the data is quoted (either kind), remove the quotes
			if ((a[0] == '"'  && a[strlen(a)-1] == '"') ||
			    (a[0] == '\'' && a[strlen(a)-1] == '\'')) { a++; a[strlen(a)-1] = 0; }
			// If there is anything left (after striping off whitespace and quotes) add it.
			if (*s && *a) { 
				//printf("Adding s:'%s'  a:'%s'\n", s,a);
				add(s, a);
				added = true;
			}
		}
		if (!added) printf("Configuration file '%s' line %d is malformed (format is: A=B or A=\"B\")\n", filename, line);
	}
	return true;
}

void MStringHash::print() const {
	MListItr<Node> itr(&_list);
	for (;itr.current();itr.next()) {
		printf("%s=%s\n", (const char *)itr.current()->name, (const char *)itr.current()->data);
	}
}

