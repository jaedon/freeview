/*
 * StreamClass.c
 */

#include "MHEGEngine.h"
#include "RootClass.h"
#include "StreamComponent.h"
#include "Reference.h"
#include "Generic.h"
#include "Content.h"
#include "si.h"
#include "utils.h"

static void
default_StreamClassInstanceVars(StreamClass *t, StreamClassInstanceVars *v)
{
	VK_memset(v, 0x00, sizeof(StreamClassInstanceVars));

	v->Speed.numerator = 1;
	v->Speed.have_denominator = true;
	v->Speed.demoninator = 1;

	v->CounterPosition = 0;
	v->CounterEndPosition = -1;

	v->CounterTriggers = NULL;

//	if(!MHEGEngine_avDisabled())
		MHEGStreamPlayer_init(&v->player);

	return;
}

static void
free_StreamClassInstanceVars(StreamClassInstanceVars *v)
{
//	if(!MHEGEngine_avDisabled())
		MHEGStreamPlayer_fini(&v->player);

	LIST_FREE(&v->CounterTriggers, CounterTrigger, safe_free);

	return;
}

void
StreamClass_Preparation(StreamClass *t)
{
	LIST_TYPE(StreamComponent) *comp;

#if (CLS_VERBOSE)
	verbose_class("StreamClass: %s; Preparation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been prepared */
	if(t->rootClass.inst.AvailabilityStatus)
		return;

	/* retrieve the object from an entity outside the engine and set each internal attribute of the object to its initial value */
	default_StreamClassInstanceVars(t, &t->inst);

	/* do Activation of each initially active StreamComponent */
	comp = t->multiplex;
	while(comp)
	{
		if(StreamComponent_isInitiallyActive(&comp->item))
			StreamComponent_Activation(&comp->item);
		comp = comp->next;
	}

#if 0 // org

	/* finish the RootClass Preparation */
	t->rootClass.inst.AvailabilityStatus = true;

	/* generate IsAvailable event */
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_is_available, NULL);

	/* generate an asynchronous ContentAvailable event */
	MHEGEngine_generateAsyncEvent(&t->rootClass.inst.ref, EventType_content_available, NULL);

#else // hmkim : modified.

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&t->rootClass);

#endif

	return;
}

void
StreamClass_Activation(StreamClass *t)
{
	LIST_TYPE(StreamComponent) *comp;
	RootClass *r;
	OctetString *service;

#if (CLS_VERBOSE)
	verbose_class("StreamClass: %s; Activation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been activated */
	if(t->rootClass.inst.RunningStatus)
		return;

	/* has it been prepared yet */
	if(!t->rootClass.inst.AvailabilityStatus)
	{
		/* generates an IsAvailable event */
		StreamClass_Preparation(t);
	}

	/* assume default is MHEG_REC_SVC_CUR, ie current channel */
	if(t->have_original_content && (service = ContentBody_getReference(&t->original_content)) != NULL)
	{
		/*
		 * service can be:
		 * MHEG_DVB_SERVICE - "dvb://<original_network_id>.[<transport_stream_id>].<service_id>"
		 * MHEG_REC_SVC_DEF - use the service we are downloading the carousel from
		 * MHEG_REC_SVC_CUR - use the current service
		 * this will be the same as "def" unless SetData has been called on the StreamClass
		 * MHEG_REC_SVC_LCN X - use logical channel number X (eg 1 for BBC1, 3 for ITV1, etc)
		 */
		if(OctetString_strncmp(service, MHEG_DVB_SERVICE, 4) == 0)
		{
			MHEGStreamPlayer_setServiceID(t->inst.player, si_get_service_id(service));
		}
		else if(OctetString_strncmp(service, MHEG_REC_SVC_LCN, 14) == 0)
		{
			verbose_todo("%s; service='%.*s'", __FUNCTION__, service->size, service->data);
		}
		else if(OctetString_strcmp(service, MHEG_REC_SVC_DEF) == 0)
		{
			/* use the service ID we are currently tuned to */
			MHEGStreamPlayer_setServiceID(t->inst.player, -1);
		}
		/* leave player's service ID as it is for MHEG_REC_SVC_CUR */
		else if(OctetString_strcmp(service, MHEG_REC_SVC_CUR) != 0)
		{
			error("StreamClass: unexpected service '%.*s'", service->size, service->data);
		}
	}

	/* start playing all active StreamComponents */
	comp = t->multiplex;
	while(comp)
	{
		if((r = StreamComponent_rootClass(&comp->item)) != NULL && r->inst.RunningStatus)
			StreamComponent_play(&comp->item, t->inst.player);
		comp = comp->next;
	}
	MHEGStreamPlayer_play(t->inst.player);

	/* multiplex is now playing */
	MHEGEngine_generateAsyncEvent(&t->rootClass.inst.ref, EventType_stream_playing, NULL); /* CI+ MHEG Profile (CI+ spec) 에서는 Stream Actions and Stream Events are not required... 이다 */

#if 0 // org
	/* finish our Activation : set its RunningStatus and generate IsRunning event */
	t->rootClass.inst.RunningStatus = true;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_is_running, NULL);
#else // hmkim : modified.
	/* Activation inherited from the RootClass */
	RootClass_Activation(&t->rootClass);
#endif

	/* now we are fully activated, let our StreamComponents know who they belong to */
	comp = t->multiplex;
	while(comp)
	{
		StreamComponent_registerStreamClass(&comp->item, t);
		comp = comp->next;
	}

	return;
}

void
StreamClass_Deactivation(StreamClass *t)
{
	LIST_TYPE(StreamComponent) *comp;
	RootClass *r;

#if (CLS_VERBOSE)
	verbose_class("StreamClass: %s; Deactivation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already deactivated */
	if(!t->rootClass.inst.RunningStatus)
		return;

	/* disown all our StreamComponents */
	comp = t->multiplex;
	while(comp)
	{
		StreamComponent_registerStreamClass(&comp->item, NULL);
		comp = comp->next;
	}

	/* stop playing all active StreamComponents */
	MHEGStreamPlayer_stop(t->inst.player);
	comp = t->multiplex;
	while(comp)
	{
		if((r = StreamComponent_rootClass(&comp->item)) != NULL && r->inst.RunningStatus)
			StreamComponent_stop(&comp->item, t->inst.player);
		comp = comp->next;
	}

	/* multiplex is now stopped */
	MHEGEngine_generateAsyncEvent(&t->rootClass.inst.ref, EventType_stream_stopped, NULL); /* CI+ MHEG Profile (CI+ spec) 에서는 Stream Actions and Stream Events are not required... 이다 */

#if 0 // org
	/* finish our Deactivation */
	t->rootClass.inst.RunningStatus = false;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_is_stopped, NULL);
#else // hmkim : modified.
	/* Deactivation inherited from the RootClass */
	RootClass_Deactivation(&t->rootClass);
#endif

	return;
}

void
StreamClass_Destruction(StreamClass *t)
{
	LIST_TYPE(StreamComponent) *comp;
	LIST_TYPE(StreamComponent) *comp_tail;
	RootClass *r;

#if (CLS_VERBOSE)
	verbose_class("StreamClass: %s; Destruction", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already destroyed */
	if(!t->rootClass.inst.AvailabilityStatus)
		return;

	/* Deactivate it if it is running */
	if(t->rootClass.inst.RunningStatus)
	{
		/* generates an IsStopped event */
		StreamClass_Deactivation(t);
	}

	/* do Destruction of all StreamComponents in the reverse order they appear in the list */
	comp = t->multiplex;
	/* find the tail */
	comp_tail = (comp != NULL) ? comp->prev : NULL;
	comp = comp_tail;
	while(comp)
	{
		/* only do Destruction if it is available */
		if((r = StreamComponent_rootClass(&comp->item)) != NULL && r->inst.AvailabilityStatus)
			StreamComponent_Destruction(&comp->item);
		/* have we reached the head */
		comp = (comp->prev != comp_tail) ? comp->prev : NULL;
	}

	/*
	 * spec says we should handle caching here
	 * rb-download caches everything
	 */

	free_StreamClassInstanceVars(&t->inst);

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
StreamClass_activateVideoComponent(StreamClass *t, VideoClass *c)
{
	/* UK MHEG Profile (D-BOOK 5.0) : Video can only be played from stream */
	/* CI+ MHEG Profile : no requirement for stream memory formats */

	verbose_class("StreamClass_activateVideoComponent (tag=%d); Storage is %s", c->component_tag, t->storage == Storage_memory ? "memory" : "stream");

	/*
	 * don't think we need to generate stream_playing/stopped events
	 * but I'm not entirely sure
	 */

	/* if we are activated, stop playing while we change the component */
	if(t->rootClass.inst.RunningStatus)
		MHEGStreamPlayer_stop(t->inst.player);

	MHEGStreamPlayer_setVideoStream(t->inst.player, c);

	if(t->rootClass.inst.RunningStatus)
		MHEGStreamPlayer_play(t->inst.player);

	return;
}

void
StreamClass_activateAudioComponent(StreamClass *t, AudioClass *c)
{
	/* UK MHEG Profile (D-BOOK 5.0) : the only StreamComponent that accepts a Storage specification of memory is Audio */
	/* CI+ MHEG Profile : no requirement for stream memory formats */

	verbose_class("StreamClass_activateAudioComponent (tag=%d); Storage is %s", c->component_tag, t->storage == Storage_memory ? "memory" : "stream");

	/*
	 * don't think we need to generate stream_playing/stopped events
	 * but I'm not entirely sure
	 */

	/* if we are activated, stop playing while we change the component */
	if(t->rootClass.inst.RunningStatus)
		MHEGStreamPlayer_stop(t->inst.player);

	MHEGStreamPlayer_setAudioStream(t->inst.player, c);

	if(t->rootClass.inst.RunningStatus)
		MHEGStreamPlayer_play(t->inst.player);

	return;
}

void
StreamClass_deactivateVideoComponent(StreamClass *t, VideoClass *c)
{
	/* UK MHEG Profile (D-BOOK 5.0) : Video can only be played from stream */
	/* CI+ MHEG Profile : no requirement for stream memory formats */

	verbose_class("StreamClass_deactivateVideoComponent (tag=%d); Storage is %s", c->component_tag, t->storage == Storage_memory ? "memory" : "stream");

	/*
	 * don't think we need to generate stream_playing/stopped events
	 * but I'm not entirely sure
	 */

	/* if we are activated, stop playing while we change the component */
	if(t->rootClass.inst.RunningStatus)
		MHEGStreamPlayer_stop(t->inst.player);

	MHEGStreamPlayer_setVideoStream(t->inst.player, NULL);

	if(t->rootClass.inst.RunningStatus)
		MHEGStreamPlayer_play(t->inst.player);

	return;
}

void
StreamClass_deactivateAudioComponent(StreamClass *t, AudioClass *c)
{
	/* UK MHEG Profile (D-BOOK 5.0) : the only StreamComponent that accepts a Storage specification of memory is Audio */
	/* CI+ MHEG Profile : no requirement for stream memory formats */

	verbose_class("StreamClass_deactivateAudioComponent (tag=%d); Storage is %s", c->component_tag, t->storage == Storage_memory ? "memory" : "stream");

	/*
	 * don't think we need to generate stream_playing/stopped events
	 * but I'm not entirely sure
	 */

	/* if we are activated, stop playing while we change the component */
	if(t->rootClass.inst.RunningStatus)
		MHEGStreamPlayer_stop(t->inst.player);

	MHEGStreamPlayer_setAudioStream(t->inst.player, NULL);

	if(t->rootClass.inst.RunningStatus)
		MHEGStreamPlayer_play(t->inst.player);

	return;
}

/*
 * corrigendum says StreamClass can be the target of SetData
 * this changes the multiplex (ie service ID)
 */

void
StreamClass_SetData(StreamClass *t, SetData *set, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__); // MHEGEngine_setRecSvcCur(...)

	verbose_class("StreamClass: %s; SetData", ExternalReference_name(&t->rootClass.inst.ref));

	/* TODO : A Set Data elementary action resets the scaling factor of a video object contained in a Stream object to its original value. */

	return;
}

void
StreamClass_SetCounterTrigger(StreamClass *t, SetCounterTrigger *params, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__); // SetCounterTrigger required in UK MHEG Profile but not required to support in CI+ MHEG Profile

	verbose_class("StreamClass: %s; SetCounterTrigger", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
StreamClass_SetSpeed(StreamClass *t, SetSpeed *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int numerator = GenericInteger_getInteger(&params->new_speed.numerator, caller_gid);
	bool have_denominator = params->new_speed.have_denominator;
	int demoninator = params->new_speed.have_denominator ? GenericInteger_getInteger(&params->new_speed.denominator, caller_gid) : 1;
	int NewSpeed;

	verbose_class("StreamClass: %s; SetSpeed; (%d, %d, %d); Storage is %s",
		ExternalReference_name(&t->rootClass.inst.ref), numerator, have_denominator, demoninator, t->storage == Storage_memory ? "memory" : "stream");

	/* ISO/IEC 13522-5 에 따르면 본래 비디오 디코딩의 trick mode 처리에 사용되는 것이지만
	   UK MHEG Profile (D-BOOK 5.0) 에서는 Receivers shall not implement trick modes... 라고 되어 있고, 다만 speed > 0 shall be treated as normal decoding, speed <= 0 shall pause the decoding... 이다.
	   또한, CI+ MHEG Profile (CI+ spec) 에서는 Stream Actions and Stream Events are not required... 이다. */

	t->inst.Speed.numerator = numerator;
	t->inst.Speed.have_denominator = have_denominator;
	t->inst.Speed.demoninator = demoninator;

	NewSpeed = numerator / demoninator; // The NewSpeed attribute is defined as a ratio Numerator/Denominator (ISO/IEC 13522-5)
	if (NewSpeed <= 0)
	{
		// video freeze, audio mute 하고 또한 subtitle 이 활성화 되어 있으면 역시 freeze 또는 닫아라... (이미 freeze, mute, 비활성화 되어 있는 경우에도 문제 없도록 핸들링할 것)
		MHEGIB_Video_Freeze();
	}
	else
	{
		// play resume 해라... (이미 play 되어 있는 경우에도 문제 없도록 핸들링할 것)
		MHEGIB_Video_Resume();
	}
}
#endif

	return;
}

void
StreamClass_SetCounterPosition(StreamClass *t, SetCounterPosition *params, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__); // SetCounterPosition required in UK MHEG Profile but not required to support in CI+ MHEG Profile

	verbose_class("StreamClass: %s; SetCounterPosition", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
StreamClass_SetCounterEndPosition(StreamClass *t, SetCounterEndPosition *params, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__); // SetCounterEndPosition required in UK MHEG Profile but not required to support in CI+ MHEG Profile

	verbose_class("StreamClass: %s; SetCounterEndPosition", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

