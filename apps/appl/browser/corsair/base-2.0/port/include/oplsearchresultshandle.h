#ifndef __OPLSEARCHRESULTSHANDLE_H__
#define __OPLSEARCHRESULTSHANDLE_H__

#include "opltype.h"
#include "oplprogramme.h"
#include "oplrecording.h"

#include <oplsearchmanager_def.h>

class OplSearchResultsHandle
{
public:
    OPL_HANDLE context;
    OPL_SearchTarget_e target;
    OPL_Collection_t *items;
    int length;
    int offset;
    int totalSize;
    int count;
    void abort();
    void release();
    OPL_BOOL search();
    OPL_BOOL update();
    OPL_BOOL update(const char *typeName, HxVector_t *list);
    int getTotalSize();
    OplSearchResultsHandle(OPL_SearchTarget_e target, OPL_HANDLE context);
    ~OplSearchResultsHandle();
};

#endif // __OPLSEARCHRESULTSHANDLE_H__
