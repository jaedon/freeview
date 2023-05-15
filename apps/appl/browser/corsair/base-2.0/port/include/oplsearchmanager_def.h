#ifndef __OPLSEARCHMANAGER_DEF_H__
#define __OPLSEARCHMANAGER_DEF_H__

#include "opltype.h"
#include "oplprogramme.h"

typedef enum {
    eOPL_SEARCH_TARGET_SCHEDULED = 1,
    eOPL_SEARCH_TARGET_ONDEMAND = 2,
    eOPL_SEARCH_TARGET_PROGRAMME = 3,
    eOPL_SEARCH_TARGET_CHANNEL = 4,
    eOPL_SEARCH_TARGET_FILE = 5,
    eOPL_SEARCH_TARGET_RECORDING = 6,

    eOPL_SEARCH_TARGET_UNKNOWN
} OPL_SearchTarget_e;

typedef enum {
    eOPL_QUERY_TYPE_FIELD = 1,
    eOPL_QUERY_TYPE_STRING = 2,

    eOPL_QUERY_TYPE_UNKNOWN
} OPL_QueryType_e;

typedef struct {
    OPL_HANDLE			search;

    int					length;
    int					offset;
    int					totalSize;
    int					count;

    OPL_Collection_t	*items;
} OPL_SearchResults_t;


typedef enum {
    eOPL_SEARCH_STATE_MORE_DATA,
    eOPL_SEARCH_STATE_FINISHED,
    eOPL_SEARCH_STATE_UPDATED,
    eOPL_SEARCH_STATE_INVALID,

    eOPL_SEARCH_STATE_UNKNOWN
} OPL_SearchState_e;

typedef void	(* OPL_SearchListener_t) (OPL_HANDLE, OPL_SearchState_e);
typedef void 	(* OPL_SearchPfMonitor_t) (OPL_HANDLE self, OPL_ProgrammeHandle *pf, HINT32 n, void *);

typedef struct {
    HxTREE_t	*instanceMap;
} MetadataSearchManager_t;

#endif // __OPLSEARCHMANAGER_DEF_H__
