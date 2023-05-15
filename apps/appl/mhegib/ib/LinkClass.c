/*
 * LinkClass.c
 */

#if !defined(WIN32)
#include <stdbool.h>
#endif

#include "MHEGEngine.h"
#include "LinkClass.h"
#include "RootClass.h"
#include "EventType.h"
#include "Reference.h"
#include "der_decode.h"

void
LinkClass_Preparation(LinkClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("LinkClass: %s; Preparation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been prepared */
	if(t->rootClass.inst.AvailabilityStatus)
		return;

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&t->rootClass);

	return;
}

void
LinkClass_Activation(LinkClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("LinkClass: %s; Activation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been activated */
	if(t->rootClass.inst.RunningStatus)
		return;

#if 1 // hmkim : added.
	/* has it been prepared yet */
	if(!t->rootClass.inst.AvailabilityStatus)
	{
		/* generates an IsAvailable event */
		LinkClass_Preparation(t);
	}
#endif

	/* add it to the list of active links */
	MHEGEngine_addActiveLink(t);

	/* Activation inherited from the RootClass */
	RootClass_Activation(&t->rootClass);

#if 0 // hmkim : commented out.
	/* set its RunningStatus and generate IsRunning event */
	t->rootClass.inst.RunningStatus = true;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_is_running, NULL);
#endif

	return;
}

void
LinkClass_Deactivation(LinkClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("LinkClass: %s; Deactivation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been deactivated */
	if(!t->rootClass.inst.RunningStatus)
		return;

	MHEGEngine_removeActiveLink(t);

	/* Deactivation inherited from the RootClass */
	RootClass_Deactivation(&t->rootClass);

	return;
}

void
LinkClass_Destruction(LinkClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("LinkClass: %s; Destruction", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already destroyed */
	if(!t->rootClass.inst.AvailabilityStatus)
		return;

	/* Deactivate it if it is running */
	if(t->rootClass.inst.RunningStatus)
	{
		/* generates an IsStopped event */
		LinkClass_Deactivation(t);
	}

	/*
	 * we always cache the objects (ie dont free them)
	 * until free_InterchangedObject is called on the whole app or scene
	 */

#if 0 // org
	/* generate an IsDeleted event */
	t->rootClass.inst.AvailabilityStatus = false;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_is_deleted, NULL);
#else // hmkim : modified.
	/* Destruction inherited from the RootClass */
	RootClass_Destruction(&t->rootClass);
#endif

	return;
}

void
LinkClass_Clone(LinkClass *t, Clone *params, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__);

	verbose_class("LinkClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
LinkClass_Activate(LinkClass *l)
{
	verbose_class("LinkClass: %s; Activate", ExternalReference_name(&l->rootClass.inst.ref));

	#if 0 // hmkim : commented out (아래 LinkClass_Activation() 내부에서 처리된다).
	/* is it already active */
	if(l->rootClass.inst.RunningStatus)
		return;
	#endif

	LinkClass_Activation(l);

	return;
}

void
LinkClass_Deactivate(LinkClass *l)
{
	verbose_class("LinkClass: %s; Deactivate", ExternalReference_name(&l->rootClass.inst.ref));

	#if 0 // hmkim : commented out (아래 LinkClass_Deactivation() 내부에서 처리된다).
	/* is it already deactivated */
	if(!l->rootClass.inst.RunningStatus)
		return;
	#endif

	LinkClass_Deactivation(l);

	return;
}

/*
 * returns true if the given src/type/data match the LinkCondition for this LinkClass
 */

bool
LinkClass_conditionMet(LinkClass *l, ExternalReference *src, EventType type, EventData *data)
{
	EventType link_event_type;
	ExternalReference *link_ref;
	ObjectReference *link_src;
	char *fullname;
	OctetString absolute;
	OctetString *link_gid;

	link_event_type = l->link_condition.event_type;
	link_ref = &l->rootClass.inst.ref;
	link_src = &l->link_condition.event_source;

	/* easiest first */
	if(link_event_type != type)
	{
		#if (EVT_VERBOSE > 1)
		verbose_event("LinkCondition met NG: %s; %s != %s", ExternalReference_name(link_ref), EventType_name(link_event_type), EventType_name(type));
		#endif
		return false;
	}

	/* shouldn't happen, event type determines if there is any data or not, but just in case */
	if(l->link_condition.have_event_data && data == NULL)
	{
		#if (EVT_VERBOSE > 1)
		verbose_event("LinkCondition met NG: %s; no data !!!", ExternalReference_name(link_ref));
		#endif
		return false;
	}

	/*
	 * if we have any data, does it match
	 * if the link condition has no event data, then it matches
	 */
	if(l->link_condition.have_event_data && data != NULL)
	{
		switch(data->choice)
		{
		case EventData_boolean:
			if(data->u.boolean != l->link_condition.event_data.u.boolean)
			{
				#if (EVT_VERBOSE > 1)
				verbose_event("LinkCondition met NG: %s; boolean %d != %d", ExternalReference_name(link_ref), l->link_condition.event_data.u.boolean, data->u.boolean);
				#endif
				return false;
			}
			break;

		case EventData_integer:
			if(data->u.integer != l->link_condition.event_data.u.integer)
			{
				#if (EVT_VERBOSE > 1)
				verbose_event("LinkCondition met NG: %s; integer %d != %d", ExternalReference_name(link_ref), l->link_condition.event_data.u.integer, data->u.integer);
				#endif
				return false;
			}
			break;

		case EventData_octetstring:
			if(OctetString_cmp(&data->u.octetstring, &l->link_condition.event_data.u.octetstring) != 0)
			{
				#if (EVT_VERBOSE > 1)
				verbose_event("LinkCondition met NG: %s; string %.*s != %.*s", ExternalReference_name(link_ref),
					l->link_condition.event_data.u.octetstring.size, l->link_condition.event_data.u.octetstring.data, data->u.octetstring.size, data->u.octetstring.data);
				#endif
				return false;
			}
			break;

		default:
			error("Unknown EventData type: %d\n", data->choice);
			return false;
		}
	}

	/*
	 * check the source group id and object number
	 * if the group id is not specified in the link condition, it defaults to the enclosing app/scene (ie link_ref)
	 */
	switch(link_src->choice)
	{
	case ObjectReference_internal_reference:
		if(OctetString_cmp(&src->group_identifier, &link_ref->group_identifier) != 0)
		{
			#if (EVT_VERBOSE > 1)
			verbose_event("LinkCondition met NG: %s; internal %.*s != %.*s",
				ExternalReference_name(link_ref), link_ref->group_identifier.size, link_ref->group_identifier.data, src->group_identifier.size, src->group_identifier.data);
			#endif
			return false;
		}
		if(src->object_number != link_src->u.internal_reference)
		{
			#if (EVT_VERBOSE > 1)
			verbose_event("LinkCondition met NG: %s; internal %d != %d", ExternalReference_name(link_ref), link_src->u.internal_reference, src->object_number);
			#endif
			return false;
		}
		break;

	case ObjectReference_external_reference:
		/* make sure the event src is an absolute group id (ie starts with ~//) */
		fullname = MHEGEngine_absoluteFilename(&link_src->u.external_reference.group_identifier);
		absolute.size = strlen(fullname);
		absolute.data = (unsigned char *) fullname;
		link_gid = &absolute;
		if(OctetString_cmp(&src->group_identifier, link_gid) != 0)
		{
			#if (EVT_VERBOSE > 1)
			verbose_event("LinkCondition met NG: %s; external %.*s != %.*s",
				ExternalReference_name(link_ref), link_gid->size, link_gid->data, src->group_identifier.size, src->group_identifier.data);
			#endif
			return false;
		}
		if(src->object_number != link_src->u.external_reference.object_number)
		{
			#if (EVT_VERBOSE > 1)
			verbose_event("LinkCondition met NG: %s; external %d != %d", ExternalReference_name(link_ref), link_src->u.external_reference.object_number, src->object_number);
			#endif
			return false;
		}
		break;

	default:
		error("Unknown ObjectReference type: %d", link_src->choice);
		return false;
	}

	verbose_event("LinkCondition met OK: %s", ExternalReference_name(link_ref));

	return true;
}

