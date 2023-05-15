#ifndef __OPLMETADATASEARCHHANDLE_H__
#define __OPLMETADATASEARCHHANDLE_H__

#include "opltype.h"
#include "oplquery.h"
#include "oplchannel.h"
#include "oplprogramme.h"

#include <oplsearchmanager_def.h>
#include <oplsearchresultshandle.h>
#include <oplqueryhandle.h>

class OplMetadataSearchHandle {
private:
    static HxTREE_t *map;
public:
    OplQueryHandle *query;
    OplSearchResultsHandle *result;
    OPL_HANDLE searchmanager;
public:
    OplMetadataSearchHandle(OPL_SearchTarget_e target);
    virtual ~OplMetadataSearchHandle();
    OPL_SearchTarget_e getTarget();
    OplQueryHandle* createQuery(const HCHAR *field, int comparison, const char *value);
    OPL_HANDLE getQuery();
    int getQueryType();
    void setQuery(OplQueryHandle *query);
    void addRatingConstraint(void *scheme, int threshold);
    void clearRatingConstraint();
    void addTargetConstraint(const char *target);
    void addChannelConstraint(OPL_Channel_t channel);
    void clearChannelConstraint();
    void orderBy(const char *field, OPL_BOOL ascending);
    OplSearchResultsHandle *getResults();
    void abort();
    void setSearchManager(OPL_HANDLE searchmanager);
    OPL_HANDLE getSearchManager(void );
protected:
};

#endif // __OPLMETADATASEARCHHANDLE_H__
