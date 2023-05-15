
#ifndef __SVC_EPG_EVENT_INT_H__
#define __SVC_EPG_EVENT_INT_H__

#include <octo_common.h>

#include <svc_epg.h>

#include "_svc_epg_query.h"


// global function in module
SvcEpg_EventInfo_t *	svc_epgevent_New (const SvcEpg_EventClass_t *clazz, SvcEpg_Type_b bwType, ...);
void					svc_epgevent_Delete (SvcEpg_EventInfo_t *self);

#if 0
SvcEpg_EventInfo_t *	svc_epgevent_Clone (const SvcEpg_EventInfo_t *self);
HBOOL					svc_epgevent_Match (SvcEpg_EventInfo_t *self, const SvcEpg_Query_t *query);
SvcEpg_EventInfo_t *	svc_epgevent_Incref (SvcEpg_EventInfo_t *self);
HBOOL					svc_epgevent_IsUpdated (SvcEpg_EventInfo_t *self, ...);


// module api
const HCHAR *			svc_epgevent_GetName (SvcEpg_EventInfo_t *self, HxLANG_Id_e language);
const HCHAR *			svc_epgevent_GetText (SvcEpg_EventInfo_t *self, HxLANG_Id_e language);
const HCHAR *			svc_epgevent_GetLongText (SvcEpg_EventInfo_t *self, HxLANG_Id_e language);
HINT32					svc_epgevent_GetParentalRating (SvcEpg_EventInfo_t *self, HxCountry_e country);

#if 0 // not used
HINT32			OxEPG_Event_Compare (const SvcEpg_EventInfo_t *self, const SvcEpg_EventInfo_t *other);
#endif
#endif


#endif /* __SVC_EPG_EVENT_INT_H__ */
