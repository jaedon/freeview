/*
 * RootClass.c
 */

#if !defined(WIN32)
#include <stdbool.h>
#endif

#include "MHEGEngine.h"
#include "RootClass.h"
#include "GroupItem.h"
#include "BitmapClass.h"
#include "Reference.h"
#include "TextClass.h"
#include "EntryFieldClass.h"
#include "HyperTextClass.h"
#include "VariableClass.h"
#include "clone.h"

/*
 * any existing data is dst will be lost
 * (can't auto-gen this as add_instance_vars changes RootClass from the XSD definition)
 */

void
RootClass_dup(RootClass *dst, RootClass *src)
{
	ObjectReference_dup(&dst->ObjectReference, &src->ObjectReference);

	return;
}

/*
 * sets the RootClassInstanceVars
 * adds the object to the engine with MHEGEngine_addObjectReference
 */

void
RootClass_registerObject(RootClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("RootClass: register");
#endif

	VK_memset(&t->inst, 0x00, sizeof(RootClassInstanceVars));

	t->inst.AvailabilityStatus = false;
	t->inst.RunningStatus = false;

	/* set the RTTI to an invalid value, the der_decode_TYPE() routines set it to the correct value */
	t->inst.rtti = RTTI_Unknown;

	/*
	 * we always store a fully resolved object reference (ie including the group id)
	 * this means we don't need a ptr to the enclosing app/scene in every object
	 */
	MHEGEngine_resolveDERObjectReference(&t->ObjectReference, &t->inst.ref);

	/* remember it */
	MHEGEngine_addObjectReference(t);

	/* this will be set if a subclass calls MHEGEngine_needContent() */
	t->inst.need_content = false;

	return;
}

/*
 * registers the object contained in the given GroupItem
 * sets the RootClassInstanceVars to default values
 * sets the object's group ID and object number
 * sets the object's RTTI type
 * adds the object to the GroupItems list of the given GroupClass
 * adds the object to the engine with MHEGEngine_addObjectReference
 */

void
RootClass_registerClonedObject(LIST_TYPE(GroupItem) *gi, RootClass *group, unsigned int object_num, unsigned int rtti)
{
	RootClass *r;
	LIST_OF(GroupItem) **items = NULL;

	/* find the RootClass of the GroupItem */
	if((r = GroupItem_rootClass(&gi->item)) == NULL)
	{
		fatal("Trying to register an invalid cloned object");
		return;
	}

	VK_memset(&r->inst, 0x00, sizeof(RootClassInstanceVars));

	r->inst.AvailabilityStatus = false;
	r->inst.RunningStatus = false;

	r->inst.rtti = rtti;

	/*
	 * we always store a fully resolved object reference (ie including the group id)
	 * this means we don't need a ptr to the enclosing app/scene in every object
	 */
	OctetString_dup(&r->inst.ref.group_identifier, &group->inst.ref.group_identifier);
	r->inst.ref.object_number = object_num;

#if (CLS_VERBOSE)
	verbose_class("RootClass: %s; register clone", ExternalReference_name(&r->inst.ref));
#endif

	/* remember the object */
	MHEGEngine_addObjectReference(r);

	/* this will be set if a subclass calls MHEGEngine_needContent() */
	r->inst.need_content = false;

	/* add the object to its parent group */
	if(group->inst.rtti == RTTI_ApplicationClass)
		items = &(((ApplicationClass *) group)->items);
	else if(group->inst.rtti == RTTI_SceneClass)
		items = &(((SceneClass *) group)->items);
	else
		fatal("Trying to add a clone to an invalid object: %s", ExternalReference_name(&group->inst.ref));

	LIST_APPEND(items, gi);

	return;
}

void
RootClass_unregisterObject(RootClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("RootClass: %s; unregister", ExternalReference_name(&t->inst.ref));
#endif

	/* assert */
	if(t->inst.AvailabilityStatus)
	{
		fatal("Trying to unregister an active object: %s", ExternalReference_name(&t->inst.ref));
		return;
	}

	/* tell the engine we are going */
	MHEGEngine_removeObjectReference(t);

	free_ExternalReference(&t->inst.ref);

	return;
}

/*
 * returns false if we have already prepared it
 * -> hmkim : void 함수로 변경하고 AvailabilityStatus 체크는 caller 쪽에서 처리하도록 함.
 */

void
RootClass_Preparation(RootClass *r)
{
#if (CLS_VERBOSE)
	verbose_class("RootClass: %s; Preparation", ExternalReference_name(&r->inst.ref));
#endif

	#if 0
	/* have we already done it */
	if(r->inst.AvailabilityStatus)
		return false;
	#endif

	/*
	 * the subclass will need to:
	 * - retrieve the object from an entity outside the engine
	 * - set each internal attribute of the object to its initial value
	 */

	/* set its AvailabilityStatus and generate IsAvailable event */
	r->inst.AvailabilityStatus = true;
	MHEGEngine_generateEvent(&r->inst.ref, EventType_is_available, NULL);

	/* if the content is ready, generate an asynchronous ContentAvailable event */
	if(!r->inst.need_content)
		MHEGEngine_generateAsyncEvent(&r->inst.ref, EventType_content_available, NULL);

	return; // return true;
}

/*
 * returns false if we have already activated it
 * -> hmkim : void 함수로 변경하고 RunningStatus 체크 및 Preparation 은 caller 쪽에서 처리하도록 함. 또한, RunningStatus 셋팅 및 IsRunning event 발생은 caller 가 아닌 여기서 일괄 처리하도록 함.
 */

void
RootClass_Activation(RootClass *r)
{
#if (CLS_VERBOSE)
	verbose_class("RootClass: %s; Activation", ExternalReference_name(&r->inst.ref));
#endif

	#if 0
	/* is it already activated */
	if(r->inst.RunningStatus)
		return false;

	/* has it been prepared yet */
	if(!r->inst.AvailabilityStatus)
	{
		/* generates an IsAvailable event */
		RootClass_Preparation(r);
	}
	#endif

	#if 1
	/* set its RunningStatus and generate IsRunning event */
	r->inst.RunningStatus = true;
	MHEGEngine_generateEvent(&r->inst.ref, EventType_is_running, NULL);
	#endif

	return; // return true;
}

/*
 * returns false if we have already deactivated it
 * -> hmkim : void 함수로 변경하고 RunningStatus 체크는 caller 쪽에서 처리하도록 함.
 */

void
RootClass_Deactivation(RootClass *r)
{
#if (CLS_VERBOSE)
	verbose_class("RootClass: %s; Deactivation", ExternalReference_name(&r->inst.ref));
#endif

	#if 0
	/* is it already deactivated */
	if(!r->inst.RunningStatus)
		return false;
	#endif

	/* reset its RunningStatus and generate an IsStopped event */
	r->inst.RunningStatus = false;
	MHEGEngine_generateEvent(&r->inst.ref, EventType_is_stopped, NULL);

	return; // return true;
}

/*
 * returns false if we have already destroyed it
 * -> hmkim : void 함수로 변경하고 AvailabilityStatus 체크 및 Deactivation 은 caller 쪽에서 처리하도록 함.
 */

void
RootClass_Destruction(RootClass *r)
{
#if (CLS_VERBOSE)
	verbose_class("RootClass: %s; Destruction", ExternalReference_name(&r->inst.ref));
#endif

	#if 0
	/* is it already destroyed */
	if(!r->inst.AvailabilityStatus)
		return false;

	/* Deactivate it if it is running */
	if(r->inst.RunningStatus)
	{
		/* generates an IsStopped event */
		RootClass_Deactivation(r);
	}
	#endif

	/*
	 * spec says we should handle caching here
	 * rb-download caches everything
	 */

	/* reset its AvailabilityStatus and generate an IsDeleted event */
	r->inst.AvailabilityStatus = false;
	MHEGEngine_generateEvent(&r->inst.ref, EventType_is_deleted, NULL);

	return; // return true;
}

/*
 * value is stored in status BooleanVariable
 * caller_gid is used to resolve the status ObjectReference
 */

void
RootClass_GetAvailabilityStatus(RootClass *r, ObjectReference *status, OctetString *caller_gid)
{
	VariableClass *var;

	verbose_class("RootClass: %s; GetAvailabilityStatus", ExternalReference_name(&r->inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(status, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_boolean)
	{
		error("RootClass: GetAvailabilityStatus: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	BooleanVariableClass_setBoolean(var, r->inst.AvailabilityStatus);

	return;
}

/*
 * value is stored in status BooleanVariable
 * caller_gid is used to resolve the status ObjectReference
 */

void
RootClass_GetRunningStatus(RootClass *r, ObjectReference *status, OctetString *caller_gid)
{
	VariableClass *var;

	verbose_class("RootClass: %s; GetRunningStatus", ExternalReference_name(&r->inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(status, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_boolean)
	{
		error("RootClass: GetRunningStatus: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	BooleanVariableClass_setBoolean(var, r->inst.RunningStatus);

	return;
}

/*
 * calls the contentAvailable method for the subtype
 * generates a ContentAvailable event
 */

void
RootClass_contentAvailable(RootClass *r, OctetString *file)
{
	/* clear the need_content flag */
	r->inst.need_content = false;

	switch(r->inst.rtti)
	{
/**********************************************************************/
/* every class that calls ContentBody_getContent() will need an entry here */
/**********************************************************************/
	case RTTI_BitmapClass:
		BitmapClass_contentAvailable((BitmapClass *) r, file);
		break;

	case RTTI_TextClass:
		TextClass_contentAvailable((TextClass *) r, file);
		break;

	case RTTI_EntryFieldClass:
		EntryFieldClass_contentAvailable((EntryFieldClass *) r, file);
		break;

	case RTTI_HyperTextClass:
		HyperTextClass_contentAvailable((HyperTextClass *) r, file);
		break;

	default:
		fatal("Unexpected ContentBody in type %s", MHEGEngine_RTTI_name(r->inst.rtti));
		return;
	}

	MHEGEngine_generateAsyncEvent(&r->inst.ref, EventType_content_available, NULL);

	return;
}

