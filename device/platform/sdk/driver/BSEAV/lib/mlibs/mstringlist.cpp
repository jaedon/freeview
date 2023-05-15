/***************************************************************************
 *     Copyright (c) 2004-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mstringlist.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 5/19/08 12:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mstringlist.cpp $
 * 
 * 2   5/19/08 12:03p erickson
 * PR42752: added BDBG_OBJECT checks
 *
 ****************************************************************************/
#include <bstd.h>
#include <bdbg.h>
#include "mstringlist.h"
#include "mstring.h"
#include <stdio.h>

BDBG_OBJECT_ID(MStringListNode);
BDBG_OBJECT_ID(MStringList);
BDBG_MODULE(mstringlist);

struct MStringList::Node {
    BDBG_OBJECT(MStringListNode)
    Node *prev;
    Node *next;
    MString data;
    Node(const char *d,int len) {data.strncpy(d,len);prev=next=NULL;}
};

void MStringList::add(const char *data, int len) {
    Node *node = new Node(data, len);
    BDBG_OBJECT_SET(node, MStringListNode);
    if (_last) {
        _last->next = node;
        node->prev = _last;
        _last = node;
    }
    else {
        _first = _last = node;
    }
    _current = node;
    _total++;
}

void MStringList::insert(int index, const char *data, int len) {
    at(index);
    if (_current) {
        Node *node = new Node(data, len);
        BDBG_OBJECT_SET(node, MStringListNode);
        if (_current->prev) {
            _current->prev->next = node;
            node->prev = _current->prev;
        }
        else {
            _first = node;
        }
        _current->prev = node;
        node->next = _current;
        _current = node;
        _total++;
    }
    else {
        add(data);
    }
}

void MStringList::change(int index, const char *data, int len) {
    at(index);
    if (_current) {
        _current->data.strncpy(data, len);
    }
}

bool MStringList::remove(const char *data) {
    if (index(data) != -1)
        return remove();
    else
        return false;
}

bool MStringList::remove(int i) {
    if (at(i))
        return remove();
    else
        return false;
}

bool MStringList::remove() {
    BDBG_OBJECT_ASSERT(this, MStringList);
    if (_current) {
        BDBG_OBJECT_ASSERT(_current, MStringListNode);
        Node *temp = _current->next;
        if (!temp)
            temp = _current->prev;
        if (temp) {
            BDBG_OBJECT_ASSERT(temp, MStringListNode);
        }

        if (_current->prev) {
            BDBG_OBJECT_ASSERT(_current->prev, MStringListNode);
            _current->prev->next = _current->next;
        }
        else {
            _first = _current->next;
        }
        if (_current->next) {
            BDBG_OBJECT_ASSERT(_current->next, MStringListNode);
            _current->next->prev = _current->prev;
        }
        else {
            _last = _current->prev;
        }
        delete _current;
        _total--;

        _current = temp;
        return true;
    }
    return false;
}

int MStringList::index(const char *data) {
    int i = 0;

    BDBG_OBJECT_ASSERT(this, MStringList);
    _current = _first;
    while (_current) {
        BDBG_OBJECT_ASSERT(_current, MStringListNode);
        if (_current->data == data) {
            return i;
        }
        _current = _current->next;
        i++;
    }
    return -1;
}

MString &MStringList::at(int i) {
    BDBG_OBJECT_ASSERT(this, MStringList);
    if (i < 0) {
        _current = NULL;
    }
    else {
        _current = _first;
        while (i-- && _current) {
            BDBG_OBJECT_ASSERT(_current, MStringListNode);
            _current = _current->next;
        }
    }
    return current();
}

void MStringList::clear() {
    first();
    while (_current) {
        remove();
    }
}

MString &MStringList::first() {
    _current = _first;
    return current();
}

MString &MStringList::next() {
    if (_current)
        _current = _current->next;
    return current();
}

MString &MStringList::prev() {
    if (_current)
        _current = _current->prev;
    return current();
}

MString &MStringList::last() {
    _current = _last;
    return current();
}

MString &MStringList::current() {
    static MString stub;
    return _current?_current->data:stub;
}

const char *MStringList::current() const {
    return _current?(const char *)_current->data:NULL;
}

int MStringList::split(const char *str, const char *delim, MStringList &list) {
    const char *next;
    int count = 0;
    while (str && *str) {
        next = strpbrk(str, delim);
        count++;
        if (next)
            list.add(str, next-str);
        else {
            list.add(str);
            break;
        }
        str = strspn(next, delim) + next;
    }
    return count;
}

int MStringList::split(const char *str, char delim, MStringList &list) {
    char *next;
    int count = 0;
    if (str && *str)
    while (1) {
        next = strchr(str, delim);
        count++;
        if (next)
            list.add(str, next-str);
        else {
            list.add(str);
            break;
        }
        str = next+1;
    }
    return count;
}

MString MStringList::join(char delim) {
    MString result;
    const char *s = first();
    while (s) {
        result += s;
        s = next();
        if (s)
            result += delim;
    }
    return result;
}

bool readFile(const char *filename, MStringList &list) {
    char buf[256];

    FILE *f = fopen(filename, "r");
    if (!f)
        return false;

    while (!feof(f)) {
        if (!fgets(buf, 256, f))
            break;
        int len = strlen(buf);
        if (buf[len-1] == '\n')
            buf[--len] = 0;
        if (buf[len-1] == '\r')
            buf[--len] = 0;
        list.add(buf);
    }
    fclose(f);
    return true;
}

bool writeFile(const char *filename, MStringList &list) {
    FILE *f = fopen(filename, "w+");
    if (!f)
        return false;

    for (const char *s = list.first(); list.current(); s = list.next()) {
        fputs(s, f);
        fputc('\n', f);
    }
    fclose(f);
    return true;
}

void MStringList::add(MStringList &list)
{
    for (const char *s = list.first(); s; s = list.next())
        add(s);
}
