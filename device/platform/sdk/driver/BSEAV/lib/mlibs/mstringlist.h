/***************************************************************************
 *     Copyright (c) 2004-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mstringlist.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 5/19/08 12:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mstringlist.h $
 * 
 * 2   5/19/08 12:03p erickson
 * PR42752: added BDBG_OBJECT checks
 *
 ****************************************************************************/
#ifndef MSTRINGLIST_H
#define MSTRINGLIST_H

#include <bstd.h>
#include <bdbg.h>
#include <stdlib.h>
#include "mstring.h"

BDBG_OBJECT_ID_DECLARE(MStringList);

/**
* MStringList holds MStrings, put allows "const char *" input and output, which
* reduces copying. Memory is always managed by the list, so there is no
* autoDelete flag, like MList.
*/
class MStringList {
    BDBG_OBJECT(MStringList)
public:
    MStringList() {
        BDBG_OBJECT_SET(this, MStringList);
        _first = _last = _current = NULL;
        _total = 0;
    }
    virtual ~MStringList() {
        clear();
        BDBG_OBJECT_UNSET(this, MStringList);
    }

    void add(const char *data, int len = -1);
    void insert(int index, const char *data, int len = -1);
    void change(int index, const char *data, int len = -1);
    bool remove(const char *data);
    bool remove(int i);
    bool remove();
    int index(const char *data);

    int total() const {return _total;}

    void clear();

    MString &first();
    MString &next();
    MString &prev();
    MString &last();
    MString &at(int i);
    MString &current();
    const char *current() const;
    MString &operator [](int index) {return at(index);}

    static int split(const char *str, const char *delims, MStringList &list);
    static int split(const char *str, char delims, MStringList &list);
    MString join(char delim);

    /**
    * Append a list to the end. The list param is not constant because of first/next.
    **/
    void add(MStringList &list);

protected:
    struct Node;
    Node *_first;
    Node *_last;
    Node *_current;
    int _total;
};

bool readFile(const char *filename, MStringList &list);
bool writeFile(const char *filename, MStringList &list);

#endif //MStringList_H
