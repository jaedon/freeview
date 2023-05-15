/*
 * ApplicationClass.c
 */

#include <ctype.h>

#include "MHEGEngine.h"
#include "RootClass.h"
#include "GroupClass.h"
#include "ActionClass.h"
#include "GroupItem.h"
#include "Reference.h"
#include "Generic.h"
#include "VariableClass.h"
#include "clone.h"

/* internal funcs */
static bool feature_AncillaryConnections(OctetString *);
static bool feature_ApplicationStacking(OctetString *);
static bool feature_Cloning(OctetString *);
static bool feature_FreeMovingCursor(OctetString *);
static bool feature_MultipleAudioStreams(OctetString *);
static bool feature_MultipleRTGraphicsStreams(OctetString *); /* ISO/IEC 13522-5 에만 기술되어 있음 */
static bool feature_MultipleVideoStreams(OctetString *);
static bool feature_OverlappingVisibles(OctetString *);
static bool feature_Scaling(OctetString *);
static bool feature_SceneAspectRatio(OctetString *);
static bool feature_SceneCoordinateSystem(OctetString *);
static bool feature_TrickModes(OctetString *);
static bool feature_VideoScaling(OctetString *);
static bool feature_BitmapScaling(OctetString *);
static bool feature_VideoDecodeOffset(OctetString *);
static bool feature_BitmapDecodeOffset(OctetString *);
static bool feature_UniversalEngineProfile(OctetString *);
static bool feature_DownloadableFont(OctetString *); /* CI+ MHEG Profile 추가 사항 */
static bool feature_HDVideoExtension(OctetString *); /* CI+ 1.3 MHEG Profile 추가 사항 */
static bool feature_HDGraphicsPlaneExtension(OctetString *); /* CI+ 1.3 MHEG Profile 추가 사항 */

static int get_int_param(unsigned char **, unsigned int *);

static void
default_ApplicationClassInstanceVars(ApplicationClass *t, ApplicationClassInstanceVars *v)
{
	VK_memset(v, 0x00, sizeof(ApplicationClassInstanceVars));

	/* GroupClass */
	v->GroupCachePriority = t->original_group_cache_priority;
	v->Timers = NULL;
	v->removed_timers = NULL;

	/* ApplicationClass */
	v->LockCount = 0;
	v->DisplayStack = NULL;

	v->next_clone = FIRST_CLONED_OBJ_NUM;

	return;
}

static void
free_ApplicationClassInstanceVars(ApplicationClassInstanceVars *v)
{
	GroupClass_freeTimers(&v->Timers, &v->removed_timers);

	LIST_FREE(&v->DisplayStack, RootClassPtr, safe_free);

	return;
}

void
ApplicationClass_Preparation(ApplicationClass *a)
{
	LIST_TYPE(GroupItem) *gi;

#if (CLS_VERBOSE)
	verbose_class("ApplicationClass: %s; Preparation", ExternalReference_name(&a->rootClass.inst.ref));
#endif

	/* has it already been prepared */
	if(a->rootClass.inst.AvailabilityStatus)
		return;

	/* retrieve the object from an entity outside the engine and set each internal attribute of the object to its initial value */
	default_ApplicationClassInstanceVars(a, &a->inst);

	/* do Preparation on all Ingredients that are initially_active */
	gi = a->items;
	while(gi)
	{
		if(GroupItem_isInitiallyActive(&gi->item))
			GroupItem_Preparation(&gi->item);
		gi = gi->next;
	}

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&a->rootClass);

	return;
}

void
ApplicationClass_Activation(ApplicationClass *a)
{
	LIST_TYPE(GroupItem) *gi;

#if (CLS_VERBOSE)
	verbose_class("ApplicationClass: %s; Activation", ExternalReference_name(&a->rootClass.inst.ref));
#endif

	/* has it already been activated */
	if(a->rootClass.inst.RunningStatus)
		return;

	/* has it been prepared yet */
	if(!a->rootClass.inst.AvailabilityStatus)
	{
		ApplicationClass_Preparation(a);
		#if 0 // hmkim : commented out (위의 ApplicationClass_Preparation() 내부에서 처리된다).
		/* generates an IsAvailable event */
		RootClass_Preparation(&a->rootClass);
		#endif
	}

	/* do Activation of the GroupClass */
	if(a->have_on_start_up)
		ActionClass_execute(&a->on_start_up, &a->rootClass.inst.ref); // hmkim : org is ActionClass_execute(&a->on_start_up, &a->rootClass.inst.ref.group_identifier);

	/* do Activation on all Ingredients that are initially_active */
	gi = a->items;
	while(gi)
	{
		/* do Activation of all InitiallyActive group items */
		if(GroupItem_isInitiallyActive(&gi->item))
			GroupItem_Activation(&gi->item);
		gi = gi->next;
	}

#if 0 // org
	/* set its RunningStatus and generate IsRunning event */
	a->rootClass.inst.RunningStatus = true;
	MHEGEngine_generateEvent(&a->rootClass.inst.ref, EventType_is_running, NULL);
#else // hmkim : modified.
	/* Activation inherited from the RootClass */
	RootClass_Activation(&a->rootClass);
#endif

	/* time base for absolute timers */
	gettimeofday(&a->inst.start_time, NULL);

	return;
}

void
ApplicationClass_Deactivation(ApplicationClass *a)
{
	LIST_TYPE(GroupItem) *gi;
	LIST_TYPE(GroupItem) *gi_tail;
	RootClass *r;

#if (CLS_VERBOSE)
	verbose_class("ApplicationClass: %s; Deactivation", ExternalReference_name(&a->rootClass.inst.ref));
#endif

	/* is it already deactivated */
	if(!a->rootClass.inst.RunningStatus)
		return;

	/*
	 * UK MHEG Profile says we don't need to support Open/CloseConnection
	 * so no need to perform a CloseConnection here
	 */

	/* run the OnCloseDown action */
	if(a->have_on_close_down)
		ActionClass_execute(&a->on_close_down, &a->rootClass.inst.ref); // hmkim : org is ActionClass_execute(&a->on_close_down, &a->rootClass.inst.ref.group_identifier);

	/* do Deactivation of all active Ingredients in the reverse order they appear in the list */
	gi = a->items;
	/* find the tail */
	gi_tail = (gi != NULL) ? gi->prev : NULL;
	gi = gi_tail;
	while(gi)
	{
		/* only do Deactivation if it is active */
		if((r = GroupItem_rootClass(&gi->item)) != NULL && r->inst.RunningStatus)
			GroupItem_Deactivation(&gi->item);
		/* have we reached the head */
		gi = (gi->prev != gi_tail) ? gi->prev : NULL;
	}

	/* Deactivation inherited from the RootClass */
	RootClass_Deactivation(&a->rootClass);

	return;
}

void
ApplicationClass_Destruction(ApplicationClass *a)
{
	LIST_TYPE(GroupItem) *gi;
	LIST_TYPE(GroupItem) *gi_tail;
	RootClass *r;

#if (CLS_VERBOSE)
	verbose_class("ApplicationClass: %s; Destruction", ExternalReference_name(&a->rootClass.inst.ref));
#endif

	/* is it already destroyed */
	if(!a->rootClass.inst.AvailabilityStatus)
		return;

	/* apply the Destruction behaviour to all Ingredients in the reverse order they appear in the list */
	gi = a->items;
	/* find the tail */
	gi_tail = (gi != NULL) ? gi->prev : NULL;
	gi = gi_tail;
	while(gi)
	{
		/* only do Destruction if it is available */
		if((r = GroupItem_rootClass(&gi->item)) != NULL && r->inst.AvailabilityStatus)
			GroupItem_Destruction(&gi->item);
		/* have we reached the head */
		gi = (gi->prev != gi_tail) ? gi->prev : NULL;
	}

	/* Deactivate it if it is running */
	if(a->rootClass.inst.RunningStatus)
	{
		/* generates an IsStopped event */
		ApplicationClass_Deactivation(a);
	}

	/*
	 * spec says we should handle caching here
	 * rb-download caches everything
	 */

	free_ApplicationClassInstanceVars(&a->inst);

#if 0 // org
	/* generate an IsDeleted event */
	a->rootClass.inst.AvailabilityStatus = false;
	MHEGEngine_generateEvent(&a->rootClass.inst.ref, EventType_is_deleted, NULL);
#else // hmkim : modified.
	/* Destruction inherited from the RootClass */
	RootClass_Destruction(&a->rootClass);
#endif

	return;
}

void
ApplicationClass_SetCachePriority(ApplicationClass *t, SetCachePriority *params, OctetString *caller_gid)
{
//	verbose_class("ApplicationClass: %s; SetCachePriority (ignored)", ExternalReference_name(&t->rootClass.inst.ref));

#if 1 // hmkim : added.
{
	int NewCachePriority = GenericInteger_getInteger(&params->new_cache_priority, caller_gid);

	verbose_class("ApplicationClass: %s; SetCachePriority; %d", ExternalReference_name(&t->rootClass.inst.ref), NewCachePriority);

	/* ISO/IEC 13522-5 : NewCachePriority shall be set within the range [0, 255] */
	if (NewCachePriority < 0 || NewCachePriority > 255)
	{
		error("ApplicationClass: %s; SetCachePriority: NewCachePriority %d is set within the range [0, 255]", ExternalReference_name(&t->rootClass.inst.ref), NewCachePriority);
		return;
	}

	if (t->inst.GroupCachePriority != NewCachePriority)
	{
		t->inst.GroupCachePriority = NewCachePriority;
	}
}
#endif

	return;
}

void
ApplicationClass_StorePersistent(ApplicationClass *t, StorePersistent *params, OctetString *caller_gid)
{
	OctetString *filename;
	PersistentData *p;
	LIST_TYPE(ObjectReference) *ref;
	VariableClass *var;
	LIST_TYPE(OriginalValue) *val;
	bool succeed = true;

	verbose_class("ApplicationClass: %s; StorePersistent", ExternalReference_name(&t->rootClass.inst.ref));

	filename = GenericOctetString_getOctetString(&params->out_file_name, caller_gid);

	/* create a new file if it does not exist */
	p = MHEGEngine_findPersistentData(filename, true);

	/* free any existing contents */
	LIST_FREE_ITEMS(&p->data, OriginalValue, free_OriginalValue, safe_free);

	verbose_class("StorePersistent: filename '%.*s'", filename->size, filename->data);

	/* add the new values */
	ref = params->in_variables;
	while(ref && succeed)
	{
		if((var = (VariableClass *) MHEGEngine_findObjectReference(&ref->item, caller_gid)) != NULL)
		{
			if(var->rootClass.inst.rtti == RTTI_VariableClass)
			{
				verbose_class("StorePersistent: variable '%s'", VariableClass_stringValue(var));
				val = safe_malloc(sizeof(LIST_TYPE(OriginalValue)));
				OriginalValue_dup(&val->item, &var->inst.Value);
				LIST_APPEND(&p->data, val);
			}
			else
			{
				error("StorePersistent: %s is not a VariableClass", ExternalReference_name(&var->rootClass.inst.ref));
				succeed = false;
			}
		}
		ref = ref->next;
	}

	/* set the succeeded variable */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->store_succeeded, caller_gid)) != NULL)
	{
		if(var->rootClass.inst.rtti == RTTI_VariableClass
		&& VariableClass_type(var) == OriginalValue_boolean)
		{
			BooleanVariableClass_setBoolean(var, succeed);
		}
		else
		{
			error("StorePersistent: %s is not a BooleanVariableClass", ExternalReference_name(&var->rootClass.inst.ref));
		}
	}

	return;
}

void
ApplicationClass_ReadPersistent(ApplicationClass *t, ReadPersistent *params, OctetString *caller_gid)
{
	OctetString *filename;
	PersistentData *p;
	LIST_TYPE(ObjectReference) *ref = NULL;
	VariableClass *var;
	LIST_TYPE(OriginalValue) *val;
	bool succeed = true;

	verbose_class("ApplicationClass: %s; ReadPersistent", ExternalReference_name(&t->rootClass.inst.ref));

	filename = GenericOctetString_getOctetString(&params->in_file_name, caller_gid);

	/* find the file */
	if((p = MHEGEngine_findPersistentData(filename, false)) != NULL)
	{
		verbose_class("ReadPersistent: filename '%.*s'", filename->size, filename->data);
		/* read the values into the variables */
		ref = params->out_variables;
		val = p->data;
		while(ref && val && succeed)
		{
			if((var = (VariableClass *) MHEGEngine_findObjectReference(&ref->item, caller_gid)) != NULL)
			{
				if(var->rootClass.inst.rtti == RTTI_VariableClass
				&& VariableClass_type(var) == val->item.choice)
				{
					/* free any existing data */
					OriginalValue_copy(&var->inst.Value, &val->item);
					verbose_class("ReadPersistent: variable '%s'", VariableClass_stringValue(var));
				}
				else
				{
					error("ReadPersistent: %s: type mismatch (%d, %d, %d)", ExternalReference_name(&var->rootClass.inst.ref), var->rootClass.inst.rtti, VariableClass_type(var), val->item.choice);
					succeed = false;
				}
			}
			ref = ref->next;
			val = val->next;
		}
	}
	else
	{
		/* not an error message */
		verbose_class("ReadPersistent: '%.*s' not found", filename->size, filename->data);
		succeed = false;
	}

	/* did we set all the variables */
	if(ref)
	{
		error("ReadPersistent: '%.*s': unable to read all the variables", filename->size, filename->data);
		succeed = false;
	}

	/* set the succeeded variable */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->read_succeeded, caller_gid)) != NULL)
	{
		if(var->rootClass.inst.rtti == RTTI_VariableClass
		&& VariableClass_type(var) == OriginalValue_boolean)
		{
			BooleanVariableClass_setBoolean(var, succeed);
		}
		else
		{
			error("ReadPersistent: %s is not a BooleanVariableClass", ExternalReference_name(&var->rootClass.inst.ref));
		}
	}

	return;
}

void
ApplicationClass_Launch(ApplicationClass *t, GenericObjectReference *launch, OctetString *caller_gid)
{
	ObjectReference *ref;
	char *absolute;
	OctetString quit_data;

	/* assert */
	if (t == NULL)
	{
		error("Can't find ApplicationClass");
		return;
	}

	verbose_class("ApplicationClass: %s; Launch", ExternalReference_name(&t->rootClass.inst.ref));

	/* check we can find an ExternalReference for the new app */
	if(((ref = GenericObjectReference_getObjectReference(launch, caller_gid)) == NULL)
	|| ref->choice != ObjectReference_external_reference)
	{
		if(ref != NULL)
			error("Launch: unable to launch an internal reference");
		return;
	}

	/* get the absolute group ID of the new app */
	absolute = MHEGEngine_absoluteFilename(&ref->u.external_reference.group_identifier);
	quit_data.size = strlen(absolute);
	quit_data.data = (unsigned char *) absolute;

	MHEGEngine_quit(QuitReason_Launch, &quit_data);

	return;
}

void
ApplicationClass_Spawn(ApplicationClass *t, GenericObjectReference *spawn, OctetString *caller_gid)
{
	ObjectReference *ref;
	char *absolute;
	OctetString quit_data;

	/* assert */
	if (t == NULL)
	{
		error("Can't find ApplicationClass");
		return;
	}

	verbose_class("ApplicationClass: %s; Spawn; OnSpawnCloseDown ? %s", ExternalReference_name(&t->rootClass.inst.ref), t->have_on_spawn_close_down ? "true" : "false");

	/* check we can find an ExternalReference for the new app */
	if(((ref = GenericObjectReference_getObjectReference(spawn, caller_gid)) == NULL)
	|| ref->choice != ObjectReference_external_reference)
	{
		if(ref != NULL)
			error("Spawn: unable to spawn an internal reference");
		return;
	}

	/* get the absolute group ID of the new app */
	absolute = MHEGEngine_absoluteFilename(&ref->u.external_reference.group_identifier);
	quit_data.size = strlen(absolute);
	quit_data.data = (unsigned char *) absolute;

	MHEGEngine_quit(QuitReason_Spawn, &quit_data);

	verbose_todo("%s not yet implemented - on_spawn_close_down Actions", __FUNCTION__);

	return;
}

void
ApplicationClass_Quit(ApplicationClass *t)
{
	verbose_class("ApplicationClass: %s; Quit; OnRestart ? %s", ExternalReference_name(&t->rootClass.inst.ref), t->have_on_restart ? "true" : "false");

	MHEGEngine_quit(QuitReason_QuitByApp, NULL);

	verbose_todo("%s not yet implemented - on_restart Actions", __FUNCTION__); // hmkim : todo notification added.

	return;
}

void
ApplicationClass_LockScreen(ApplicationClass *t)
{
	verbose_class("ApplicationClass: %s; LockScreen; %d", ExternalReference_name(&t->rootClass.inst.ref), t->inst.LockCount);

	t->inst.LockCount ++;

	return;
}

void
ApplicationClass_UnlockScreen(ApplicationClass *t)
{
//	XYPosition pos;
//	OriginalBoxSize box;

	verbose_class("ApplicationClass: %s; UnlockScreen; %d", ExternalReference_name(&t->rootClass.inst.ref), t->inst.LockCount);

	/* if it is not already unlocked */
	if(t->inst.LockCount > 0)
	{
		t->inst.LockCount --;
		/* update the screen if it is now unlocked */
		/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
		if(t->inst.LockCount == 0)
		{
			pos.x_position = 0;
			pos.y_position = 0;
			box.x_length = MHEG_XRES;
			box.y_length = MHEG_YRES;
			MHEGEngine_redrawArea(&pos, &box);
		}
		*/
	}

	return;
}

void
ApplicationClass_OpenConnection(ApplicationClass *t, OpenConnection *params, OctetString *caller_gid)
{
	error("ApplicationClass: %s; OpenConnection not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
ApplicationClass_CloseConnection(ApplicationClass *t, CloseConnection *params, OctetString *caller_gid)
{
	error("ApplicationClass: %s; CloseConnection not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
ApplicationClass_GetEngineSupport(ApplicationClass *t, GetEngineSupport *params, OctetString *caller_gid)
{
	OctetString *feature;
	unsigned char *pars_start;
	unsigned int feature_len;
	OctetString feature_pars;
	unsigned int i;
	bool found;
	bool answer = false;
	VariableClass *var;
	struct
	{
		char *long_name;
		char *short_name;
		bool (*func)(OctetString *);
	} features[] =
	{
		/* UK MHEG Profile (D-BOOK 5.0) = UKEngineProfile1 and CI+ MHEG Profile (CI+ spec) = CIEngineProfile1 : Set of features */
		/* Caching feature 의 경우 UK MHEG Profile 에선 필요하지만 CI+ MHEG Profile 에선 필요하지 않다. 하지만 현실적으로 전송 속도가 열악한 CI 에선 더욱 필요하므로 기본으로 캐싱 지원한다. */
		{ "AncillaryConnections", "ACo", feature_AncillaryConnections },
		{ "ApplicationStacking", "ASt", feature_ApplicationStacking },
		{ "Cloning", "Clo", feature_Cloning },
		{ "FreeMovingCursor", "FMC", feature_FreeMovingCursor },
		{ "MultipleAudioStreams(", "MAS(", feature_MultipleAudioStreams },
		{ "MultipleRTGraphicsStreams(", "MRS(", feature_MultipleRTGraphicsStreams }, /* ISO/IEC 13522-5 에만 기술되어 있음 */
		{ "MultipleVideoStreams(", "MVS(", feature_MultipleVideoStreams },
		{ "OverlappingVisibles(", "OvV(", feature_OverlappingVisibles },
		{ "Scaling", "Sca", feature_Scaling },
		{ "SceneAspectRatio(", "SAR(", feature_SceneAspectRatio },
		{ "SceneCoordinateSystem(", "SCS(", feature_SceneCoordinateSystem },
		{ "TrickModes", "TrM", feature_TrickModes },
		{ "VideoScaling(", "VSc(", feature_VideoScaling },
		{ "BitmapScaling(", "BSc(", feature_BitmapScaling },
		{ "VideoDecodeOffset(", "VDO(", feature_VideoDecodeOffset },
		{ "BitmapDecodeOffset(", "BDO(", feature_BitmapDecodeOffset },
		{ "UniversalEngineProfile(", "UEP(", feature_UniversalEngineProfile },
		{ "DownloadableFont(", "DLF(", feature_DownloadableFont }, /* CI+ MHEG Profile 추가 사항 */
		{ "HDVideoExtension(", "HDE(0)(", feature_HDVideoExtension}, /* CI+ 1.3 MHEG Profile 추가 사항 */
		{ "HDGraphicsPlaneExtension(", "HDE(1)(", feature_HDGraphicsPlaneExtension}, /* CI+ .13 MHEG Profile 추가 사항 */
		{ NULL, NULL, NULL }
	};

	feature = GenericOctetString_getOctetString(&params->feature, caller_gid);

	/* does the feature take parameters */
	pars_start = memchr(feature->data, '(', feature->size);
	if(pars_start != NULL)
		feature_len = pars_start - feature->data;
	else
		feature_len = feature->size;

	found = false;
	for(i=0; features[i].func != NULL && !found; i++)
	{
		if(strncmp((char *) feature->data, features[i].short_name, feature_len) == 0
		|| strncmp((char *) feature->data, features[i].long_name, feature_len) == 0)
		{
			feature_pars.data = pars_start;
			feature_pars.size = feature->size - feature_len;
			answer = (*(features[i].func))(&feature_pars);
			found = true;
		}
	}

	if(!found)
	{
		error("GetEngineSupport: unknown feature '%.*s'", feature->size, feature->data);
		answer = false;
	}

	verbose_class("ApplicationClass: %s; GetEngineSupport(\"%.*s\") = %s",
		ExternalReference_name(&t->rootClass.inst.ref),
		feature->size, feature->data,
		answer ? "true" : "false");

	/* set the answer variable */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->answer, caller_gid)) != NULL)
	{
		if(var->rootClass.inst.rtti == RTTI_VariableClass
		&& VariableClass_type(var) == OriginalValue_boolean)
		{
			BooleanVariableClass_setBoolean(var, answer);
		}
		else
		{
			error("GetEngineSupport: %s is not a BooleanVariableClass", ExternalReference_name(&var->rootClass.inst.ref));
		}
	}

	return;
}

void
ApplicationClass_SetTimer(ApplicationClass *t, SetTimer *params, OctetString *caller_gid)
{
	verbose_class("ApplicationClass: %s; SetTimer", ExternalReference_name(&t->rootClass.inst.ref));

	GroupClass_SetTimer(&t->rootClass.inst.ref, &t->inst.Timers, &t->inst.removed_timers, &t->inst.start_time, params, caller_gid);

	return;
}

/*
 * returns NULL if the attribute is not specified in the ApplicationClass
 */

DefaultAttribute *
ApplicationClass_getDefaultAttribute(ApplicationClass *a, unsigned int attr)
{
	LIST_TYPE(DefaultAttribute) *list;

	/* assert */
	if (a == NULL)
	{
		error("Can't find ApplicationClass");
		return NULL;
	}

	list = a->default_attributes;

	while(list)
	{
		if(list->item.choice == attr)
			return &list->item;
		list = list->next;
	}

	return NULL;
}

/*
 * GetEngineSupport features
 * params->data will either be NULL or contain the full params string enclosed in brackets
 * if the feature takes a parameter, ie the name includes a (, then params will never be NULL
 * and params->size will always be at least 1
 */

static bool
feature_AncillaryConnections(OctetString *params)
{
	/* ISO/IEC 13522-5 : asks whether engine supports ancillary point to point connections.
	   These connections deal with the actions OpenConnection and CloseConnection,
	   and the attribute ConnectionTag in various elementary actions (Optional) */
	/* UK MHEG Profile (D-BOOK 5.0) : Shall return "false". */

	return false;
}

static bool
feature_ApplicationStacking(OctetString *params)
{
	/* ISO/IEC 13522-5 : asks whether engine provides support for the Spawn action of the Application class (Optional) */
	/* UK MHEG Profile (D-BOOK 5.0) : Shall return "true". To support application stacking receivers shall implement an application identifier stack capable of holding references to 5 applications. */

	return true;
}

static bool
feature_Cloning(OctetString *params)
{
	/* ISO/IEC 13522-5 : asks whether engine supports the Clone action (Mandatory) */
	/* UK MHEG Profile (D-BOOK 5.0) : Shall return "true". The receiver shall support cloning at least of the following classes: Text, Bitmap & Rectangle. */

	return true;
}

static bool
feature_FreeMovingCursor(OctetString *params)
{
	/* ISO/IEC 13522-5 : asks whether engine provides support for the class CursorShape,
	   for the events CursorEnter and CursorLeave, and for the actions GetCursorPosition, SetCursorPosition, and SetCursorShape (Optional) */
	/* UK MHEG Profile (D-BOOK 5.0) : Shall return "false". */

	return false;
}

static bool
feature_MultipleAudioStreams(OctetString *params)
{
	/* ISO/IEC 13522-5 : asks whether engine supports at least N simultaneous Audio streams (One audio stream of type stream and one of type memory at a time) */
	/* UK MHEG Profile (D-BOOK 5.0) : Shall return "true" for N <= 1 */

	unsigned char *data;
	unsigned int size;
	int n;

	/* skip the opening bracket */
	data = params->data + 1;
	size = params->size - 1;

	n = get_int_param(&data, &size);

	return (n <= 1);
}

static bool
feature_MultipleRTGraphicsStreams(OctetString *params) /* ISO/IEC 13522-5 에만 기술되어 있음 */
{
	/* ISO/IEC 13522-5 : asks whether engine supports at least N simultaneous RTGraphics streams (One active RTGraphics at a time) */

	unsigned char *data;
	unsigned int size;
	int n;

	/* skip the opening bracket */
	data = params->data + 1;
	size = params->size - 1;

	n = get_int_param(&data, &size);

	return (n <= 1);
}

static bool
feature_MultipleVideoStreams(OctetString *params)
{
	/* ISO/IEC 13522-5 : asks whether engine supports at least N simultaneous Video streams (One video stream playing at a time) */
	/* UK MHEG Profile (D-BOOK 5.0) : Shall return "true" for N <= 1 */

	unsigned char *data;
	unsigned int size;
	int n;

	/* skip the opening bracket */
	data = params->data + 1;
	size = params->size - 1;

	n = get_int_param(&data, &size);

	return (n <= 1);
}

static bool
feature_OverlappingVisibles(OctetString *params)
{
	/* ISO/IEC 13522-5 : asks whether engine supports at least N overlapping Visibles (No constraint) */
	/* UK MHEG Profile (D-BOOK 5.0) : Shall return "true" for all values of N */

	return true;
}

static bool
feature_Scaling(OctetString *params)
{
	/* ISO/IEC 13522-5 : asks whether engine supports ScaleBitmap and ScaleVideo actions (Optional) */
	/* UK MHEG Profile (D-BOOK 5.0) : Shall return "false". */

	return false;
}

static bool
feature_SceneAspectRatio(OctetString *params)
{
	/* ISO/IEC 13522-5 : asks whether engine supports a given aspect ratio. W & H are two integers, W / H is the width/height aspect ratio (Optional) */
	/* UK MHEG Profile (D-BOOK 5.0) : Shall return "true" for (W, H) is (4,3) or (16,9) */
	/* CI+ MHEG Profile : Not Required. */

	unsigned char *data;
	unsigned int size;
	int w, h;

	if (MHEGIB_GetProfile() & MHEGIB_Profile_CI)
		return false;

	/* skip the opening bracket */
	data = params->data + 1;
	size = params->size - 1;

	w = get_int_param(&data, &size);
	h = get_int_param(&data, &size);

	return ((w == 4) && (h == 3)) || ((w == 16) && (h == 9));
}

static bool
feature_SceneCoordinateSystem(OctetString *params)
{
	/* ISO/IEC 13522-5 : asks whether engine supports a given coordinate system. X & Y are two integers defining the coordinate system */
	/* UK MHEG Profile (D-BOOK 5.0) : Shall return "true" for (X,Y) is (720,576) */

	unsigned char *data;
	unsigned int size;
	int x, y;

	/* skip the opening bracket */
	data = params->data + 1;
	size = params->size - 1;

	x = get_int_param(&data, &size);
	y = get_int_param(&data, &size);

	return (x == MHEG_XRES) && (y == MHEG_YRES);
}

static bool
feature_TrickModes(OctetString *params)
{
	/* ISO/IEC 13522-5 : asks whether engine supports trick modes for Streams (Optional) */
	/* UK MHEG Profile (D-BOOK 5.0) : Shall return "false". */

	return false;
}

static bool
feature_VideoScaling(OctetString *params)
{
	/* UK MHEG Profile (D-BOOK 5.0) :
	   Shall return "true" for the combinations of CHook, X & Y tabulated below that are supported by the implementation.
	   CHook  10
	   X  1440, 1080, 720, 540, 360
	   Y  1152, 576, 288 */
	/* CI+ MHEG Profile : Not Required (May return "false") */

	unsigned char *data;
	unsigned int size;
	int chook, x, y;

	if (MHEGIB_GetProfile() & MHEGIB_Profile_CI)
		return false;

	/* skip the opening bracket */
	data = params->data + 1;
	size = params->size - 1;

	chook = get_int_param(&data, &size);
	x = get_int_param(&data, &size);
	y = get_int_param(&data, &size);

	if(chook != MHEG_ContentHook_Stream_MPEG)
		return false;

	return ((x == MHEG_XRES * 2) && (y == MHEG_YRES * 2))
	    || ((x == MHEG_XRES) && (y == MHEG_YRES))
	    || ((x == MHEG_XRES / 2) && (y == MHEG_YRES / 2));
}

static bool
feature_BitmapScaling(OctetString *params)
{
	/* UK MHEG Profile (D-BOOK 5.0) :
	   Limited scaling is required for I-frame bitmaps, but not for PNGs.
	   Scaling (the ScaleBitmap action) shall be supported for bitmap objects with MPEG I-frame content.
	   Scaling shall not be supported for Bitmap objects using PNG bitmaps.
	   Shall return "true" for the combinations of CHook, X & Y tabulated below that are supported by the implementation.
	   CHook  2
	   X  1440, 1080, 720, 540, 360
	   Y  1152, 576, 288 */

	unsigned char *data;
	unsigned int size;
	int chook, x, y;

	/* skip the opening bracket */
	data = params->data + 1;
	size = params->size - 1;

	chook = get_int_param(&data, &size);
	x = get_int_param(&data, &size);
	y = get_int_param(&data, &size);

	if(chook != MHEG_ContentHook_Bitmap_MPEG)
		return false;

#if 1	/*  X  1440, 1080, 720, 540, 360   Y  1152, 576, 288 를 모두 지원하기 위해 수정 */
return (
		((x == MHEG_BITMAP_SCALING_XRES_1 * 2) || (x == MHEG_BITMAP_SCALING_XRES_1) || (x == MHEG_BITMAP_SCALING_XRES_1 / 2)
		|| (x == MHEG_BITMAP_SCALING_XRES_2 * 2) || (x == MHEG_BITMAP_SCALING_XRES_2))
		&& ((y == MHEG_YRES * 2) || (y == MHEG_YRES) || (y == MHEG_YRES / 2))
		);
#else
	return ((x == MHEG_XRES * 2) && (y == MHEG_YRES * 2))
	    || ((x == MHEG_XRES) && (y == MHEG_YRES))
	    || ((x == MHEG_XRES / 2) && (y == MHEG_YRES / 2));
#endif
}

static bool
feature_VideoDecodeOffset(OctetString *params)
{
	/* UK MHEG Profile (D-BOOK 5.0) :
	   Shall return "true" for the combinations of CHook & Level tabulated below that are supported by the implementation.
	   CHook  10
	   Level  0, 1 */
	/* CI+ MHEG Profile : Not Required (May return "false") */

	unsigned char *data;
	unsigned int size;
	int chook, level;

	if (MHEGIB_GetProfile() & MHEGIB_Profile_CI)
		return false;

	/* skip the opening bracket */
	data = params->data + 1;
	size = params->size - 1;

	chook = get_int_param(&data, &size);
	level = get_int_param(&data, &size);

	if(chook != MHEG_ContentHook_Stream_MPEG)
		return false;

	return (level == 0 || level == 1);
}

static bool
feature_BitmapDecodeOffset(OctetString *params)
{
	/* UK MHEG Profile (D-BOOK 5.0) :
	   Shall return "true" for the combinations of CHook & Level tabulated below that are supported by the implementation.
	   CHook  2
	   Level  0, 1 */

	unsigned char *data;
	unsigned int size;
	int chook, level;

	/* skip the opening bracket */
	data = params->data + 1;
	size = params->size - 1;

	chook = get_int_param(&data, &size);
	level = get_int_param(&data, &size);

	if(chook != MHEG_ContentHook_Bitmap_MPEG)
		return false;

	return (level == 0 || level == 1);
}

#define UKEP_MATCH(STR)		if(params->size == strlen(STR)				\
				&& strncmp((char *) params->data, STR, params->size) == 0)	\
					return true

static bool
feature_UniversalEngineProfile(OctetString *params)
{
	/* UK MHEG Profile (D-BOOK 5.0) :
	   Certain legacy (non-compliant) receivers respond true when N=1 (character code 0x31) or N=2 (character code 0x32).
	   Receivers that respond true to N=1 should not return true for any value of manufacturer-specific string. */
	/* CI+ MHEG Profile :
	   UniversalEngineProfile shall respond with a true response to a string argument of "CIPLUS001" which identifies the MHEG engine as being CI Plus Profile 1 compliant. */

	/* UK MHEG Profile launch ID */
	UKEP_MATCH("(2)");

	/* CI+ MHEG Profile launch ID */
	UKEP_MATCH("(CIPLUS001)");

	/* WhoAmI strings */
	UKEP_MATCH("(" MHEG_RECEIVER_ID ")");
	UKEP_MATCH("(" MHEG_ENGINE_ID ")");
	UKEP_MATCH("(" MHEG_DSMCC_ID ")");

	return false;
}

static bool
feature_DownloadableFont(OctetString *params) /* CI+ MHEG Profile 추가 사항 */
{
#if 0

	/* CI+ MHEG Profile :
	   Shall return "true" for the values of CHook that are supported by the Font class.
	   Shall return "false" for all other values of N. */

	unsigned char *data;
	unsigned int size;
	int chook;

	/* skip the opening bracket */
	data = params->data + 1;
	size = params->size - 1;

	chook = get_int_param(&data, &size);

	if(chook == MHEG_ContentHook_OpenType_Font)
		return true;
	else
		return false;

#else

	/* CI+ MHEG Profile (CI+ spec v1.2), 12.5.1 Downloadable Fonts : Receivers may optionally support downloadable fonts using the MHEG-5 Font class... */

	return false;

#endif
}

static bool
feature_HDVideoExtension(OctetString *params) /* CI+ 1.3 MHEG Profile 추가 사항 */
{
	return true;
}

static bool
feature_HDGraphicsPlaneExtension(OctetString *params) /* CI+ 1.3 MHEG Profile 추가 사항 */
{
	return true;
}


/*
 * gets the next base 10 integer parameter
 * updates *data and *size
 */

static int
get_int_param(unsigned char **data, unsigned int *size)
{
	int val = 0;

	while(*size > 0 && isdigit(**data))
	{
		val *= 10;
		val += (**data) - '0';
		(*data) ++;
		(*size) --;
	}

	/* skip the terminator */
	if(*size > 0)
	{
		(*data) ++;
		(*size) --;
	}

	return val;
}

