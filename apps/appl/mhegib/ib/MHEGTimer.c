/*
 * MHEGTimer.c
 */

#include "MHEGEngine.h"
#include "MHEGTimer.h"
#include "GroupClass.h"
#include "Reference.h"

/*
 * timer callback
 */

typedef struct
{
	ExternalReference *ref;		/* object that contains the timer */
	int id;				/* timer ID */
	void *fired_data;		/* passed onto GroupClass_timerFired after the event has been generated */
} TimerCBData;

static void
#if 0 // org
timer_cb(XtPointer usr_data, XtIntervalId *id)
#else // hmkim : modified.
timer_cb(unsigned long id, void * usr_data)
#endif
{
	TimerCBData *data = (TimerCBData *) usr_data;
	EventData event_data;

	/* generate a TimerFired event */
	event_data.choice = EventData_integer;
	event_data.u.integer = data->id;
	MHEGEngine_generateAsyncEvent(data->ref, EventType_timer_fired, &event_data);

	/* let the object do any additional cleaning up */
	#if 0 // org
	GroupClass_timerFired(data->ref, data->id, *id, data->fired_data);
	safe_free(data);
	#else // hmkim : modified.
	GroupClass_timerFired(data->ref, data->id, id, data->fired_data);
	#endif

	/*
	 * a timer going off does not get us out of a block in XtAppNextEvent
	 * but we need to process the async event we just generated
	 * we could just call MHEGEngine_processMHEGEvents() here
	 * but if processing that means we want to Launch, Retune etc we will not be able to do it until XtAppNextEvent exits
	 * so generate a fake event here, just to end XtAppNextEvent and get back to the engine main loop
	 */

	return;
}

/*
 * generate a TimerFired event after interval milliseconds
 * the source of the event will be the given ExternalReference
 * the event data will be the given timer ID
 * also calls GroupClass_timerFired after generating the event and passes fired_data to it
 */

MHEGTimer
MHEGTimer_addGroupClassTimer(unsigned int interval, ExternalReference *ref, int id, void *fired_data)
{
	#if 0 // org

	MHEGTimer xtid;
	TimerCBData *data = safe_malloc(sizeof(TimerCBData));

	data->ref = ref;
	data->id = id;
	data->fired_data = fired_data;

	xtid = XtAppAddTimeOut(MHEGEngine_getDisplay()->app, interval, timer_cb, (XtPointer) data);

	#else // hmkim : modified.

	MHEGTimer xtid;
	TimerCBData data;

	data.ref = ref;
	data.id = id;
	data.fired_data = fired_data;

	VK_TIMER_EventAfter(interval, timer_cb, (void *)&data, sizeof(TimerCBData), (unsigned long *)&xtid);

	#endif

	verbose_class("GroupClass: %s; Timer %d:%d add", ExternalReference_name(ref), id, xtid);

	return xtid;
}

void
MHEGTimer_removeGroupClassTimer(MHEGTimer id)
{
	verbose_class("GroupClass: Timer %d remove", id);

	#if 0 // org

	XtRemoveTimeOut(id);

	#else // hmkim : modified.

	VK_TIMER_Cancel(id);

	#endif
}

/*
 * returns the number of milliseconds between t1 and t0
 */

int
time_diff(struct timeval *t1, struct timeval *t0)
{
	return ((t1->tv_sec - t0->tv_sec) * 1000) + ((t1->tv_usec - t0->tv_usec) / 1000);
}

