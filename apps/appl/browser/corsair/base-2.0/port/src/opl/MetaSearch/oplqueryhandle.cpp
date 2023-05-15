#include "oplqueryhandle.h"
#include "oplsearchmanager_def.h"

#include <hlib.h>
#include <apk.h>
#if defined( CONFIG_OP_JAPAN)
#include "oplprogramme_japan.h"
#endif

#include <apk_scenario.h>

OplQueryHandle::OplQueryHandle(const char *queryString) {

    HxLOG_Trace();
    HxLOG_Assert(queryString);

    this->type = eOPL_QUERY_TYPE_STRING;
    this->queryString = HLIB_STD_StrDup(queryString);
    this->handle = NULL;
}

OplQueryHandle::OplQueryHandle(const char *field, int comparison, const char *value) {
    const HCHAR	*comp=NULL;

    HxLOG_Trace();
    HxLOG_Assert(field);
    HxLOG_Assert(value);

    if(field == NULL || value == NULL)
    {
	return;
    }

    this->type = eOPL_QUERY_TYPE_FIELD;
    this->queryString = NULL;
    this->handle = NULL;

    switch(comparison) {
    case eOPL_QUERY_EQ:
        comp = "==";
        break;
    case eOPL_QUERY_NOT_EQ:
        comp = "!=";
        break;
    case eOPL_QUERY_GT:
        comp = ">";
        break;
    case eOPL_QUERY_GT_OR_EQ:
        comp = ">=";
        break;
    case eOPL_QUERY_LT:
        comp = "<";
        break;
    case eOPL_QUERY_LT_OR_EQ:
        comp = "<=";
        break;
    case eOPL_QUERY_CONTAINS:
        comp = "contains";
        break;
    case eOPL_QUERY_EXIST:
        comp = "exist";
        break;
    default:
        this->handle = (OPL_HANDLE)NULL;
    }

    static char	s_valueInt[64];
    DxOperator_e op;

    APK_SCENARIO_GetOperatorType(&op);

    if (HLIB_STD_StrCaseCmp(field, "programmeID") == 0
        && comparison == eOPL_QUERY_EQ
        && value
    ) {
		int			svcuid = 0;	// invalid
		int 		eventid = 0;
		int			onid = 0;
		int			tsid = 0;
		int			svcid = 0;
		OPL_BOOL	bUniqueIDs = OPL_FALSE;
		OPL_BOOL	bTrippleID = OPL_FALSE;
		OPL_Channel_t	channel;

		bUniqueIDs = OPL_Programme_GetUniqueIDsFromProgrammeID(value, &svcuid, &eventid);
		if ((bUniqueIDs == OPL_TRUE) && (svcuid == 0))
		{
			HxLOG_Error("invalid programmeId format(NO svcuid). programmeID(%s). trying to recover svc uid \n", value);

			bTrippleID = OPL_Programme_GetTrippleIDFromProgrammeID(value,&onid, &tsid, &svcid);
			if (bTrippleID != OPL_TRUE)
			{
				HxLOG_Error("Fail to find tripple ID \n");
				return ;
			}

			channel = OPL_Channel_FindChannelByTriplet(OPL_Channel_GetManager(), onid, tsid, svcid);
			if (channel == NULL)
			{
				HxLOG_Error("Fail to find channel. onid=(%d), tsid=(%d), svcid=(%d) \n",onid, tsid, svcid);
				return;
			}

			svcuid = OPL_Channel_GetUniqueID(channel);
			HxLOG_Error("recovered svcuid(%d) \n",svcuid);
		}

        if (bUniqueIDs == OPL_TRUE)
        {
            HxSTD_PrintToStrN(s_valueInt, 64, "%d.%d", svcuid, eventid);
            value = s_valueInt;
        }
    }

    if (HLIB_STD_StrCaseCmp(field, "genre") == 0
        && comparison == eOPL_QUERY_EQ
        && value
    ) {

        switch(op) {
            case eDxOPERATOR_JCOM:
#if defined( CONFIG_OP_JAPAN)
				value = OPL_PROGRAMME_JAPAN_GetGenreValue((HCHAR*) value );
#endif
                break;

            default:
                break;
        }
    }

#if defined(CONFIG_OP_TDC)
	// Convert query( ccid -> svcuid )
	static const char *s_strSvcUid = "svcuid";
	if (HLIB_STD_StrCaseCmp(field, "ccid") == 0
        && comparison == eOPL_QUERY_EQ
        && value
	) {
		int svcuid = 0;
		svcuid = OPL_Channel_CCID2UniqueID(value);
		if( svcuid != 0 )
		{
			HxSTD_PrintToStrN(s_valueInt, 64, "%d", svcuid);
			field = s_strSvcUid;
			value = s_valueInt;
		}
	}
#endif
	
    this->handle = (OPL_HANDLE)APK_META_QUERY_NewQuery(
                          (const HCHAR *)field
                        , comp
                        , (const HCHAR *)value
                    );
}

OplQueryHandle::~OplQueryHandle() {
    if (this->queryString) {
        HLIB_STD_MemFree(this->queryString);
    }
    if (this->handle) {
        APK_META_QUERY_DeleteQuery((APK_QUERY_t)this->handle);
    }
}

char* OplQueryHandle::getQueryString() {
    return this->queryString;
}
/*
void OplQueryHandle::_and(const OplQueryHandle *query) {
    HxLOG_Trace();

    this->handle = (OPL_HANDLE)APK_META_QUERY_And((const APK_QUERY_t)this->handle, (const APK_QUERY_t)query->handle);
}

void OplQueryHandle::_or(const OplQueryHandle *query) {
    HxLOG_Trace();

    this->handle = (OPL_HANDLE)APK_META_QUERY_Or((const APK_QUERY_t)this->handle, (const APK_QUERY_t)query->handle);
}
*/

void OplQueryHandle::_and(const OplQueryHandle *query) {
	OPL_HANDLE 	handle = this->handle;

	HxLOG_Trace();
	this->handle = (OPL_HANDLE)APK_META_QUERY_And((const APK_QUERY_t)this->handle, (const APK_QUERY_t)query->handle);

	if (handle)
	{
		APK_META_QUERY_DeleteQuery((APK_QUERY_t)handle);
	}
}

void OplQueryHandle::_or(const OplQueryHandle *query) {
	OPL_HANDLE 	handle = this->handle;

	HxLOG_Trace();
	this->handle = (OPL_HANDLE)APK_META_QUERY_Or((const APK_QUERY_t)this->handle, (const APK_QUERY_t)query->handle);

	if (handle)
	{
		APK_META_QUERY_DeleteQuery((APK_QUERY_t)handle);
	}
}

void OplQueryHandle::_not() {
    HxLOG_Trace();

    this->handle = (OPL_HANDLE)APK_META_QUERY_Not((const APK_QUERY_t)this->handle);
}
