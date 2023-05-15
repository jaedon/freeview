#ifndef __OPLSEARCHMANAGER_H__
#define __OPLSEARCHMANAGER_H__

#include "opltype.h"
#include "oplquery.h"
#include "oplchannel.h"
#include "oplprogramme.h"

#include <oplsearchmanager_def.h>
#include <oplmetadatasearchhandle.h>
#include <list>

using namespace std;
typedef list<OPL_HANDLE> LIST;

class OplSearchManager
{
private:
    static bool instanceFlag;
    static OplSearchManager* manager;
    static LIST map;
    static OPL_SearchListener_t onSearch;
    static void onMetadataSearchEvent(OPL_HANDLE handle, HINT32 event, const HINT32 *args, void *userdata);
    static HBOOL onSmartSearchEvent(HINT32 id, HINT32 remainingJob, const char *typeName, HxVector_t *list, void *userdata);
public:
    static OplSearchManager* getInstance();
    OPL_HANDLE createSearch(OPL_SearchTarget_e target, OPL_HANDLE searchmanager);
    void removeSearch(OPL_HANDLE search);
    static bool hasSearch(OplMetadataSearchHandle* search);
    void addEventListener (OPL_HANDLE handle, OPL_SearchListener_t onSearch);
    void executeQuery(OPL_HANDLE handle);
~OplSearchManager();
protected:
};

#endif // __OPLSEARCHMANAGER_H__
