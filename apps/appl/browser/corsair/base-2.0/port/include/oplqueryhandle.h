#ifndef __OPLQUERYHANDLE_H__
#define __OPLQUERYHANDLE_H__

#include "oplprogramme.h"
#include "opltype.h"
#include "oplquery.h"

class OplQueryHandle
{
private:
    char *queryString;
public:
    int type;
    void* eventHandler;
    OPL_HANDLE handle;
    OplQueryHandle(const char *queryString);
    OplQueryHandle(const char *field, int comparison, const char *value);
    ~OplQueryHandle();
    char* getQueryString();
    void _and(const OplQueryHandle *query);
    void _or(const OplQueryHandle *query);
    void _not();
};
#endif // __OPLQUERYHANDLE_H__
