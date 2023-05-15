/*
 * VideoClass.c
 */

#include "MHEGEngine.h"
#include "RootClass.h"
#include "StreamClass.h"
#include "Reference.h"
#include "Generic.h"
#include "VariableClass.h"
#include "clone.h"

static void
default_VideoClassInstanceVars(VideoClass *t, VideoClassInstanceVars *v)
{
	/*
	 * don't do:
	 * VK_memset(v, 0x00, sizeof(VideoClassInstanceVars));
	 * or:
	 * v->owner = NULL;
	 * the whole VideoClass including these instance vars is zero'd when it is DER decoded
	 * we need to make sure v->owner is not set to NULL here
	 * in case our StreamClass is already active and has set our owner
	 */

	/* VisibleClass */
	VK_memcpy(&v->BoxSize, &t->original_box_size, sizeof(OriginalBoxSize));
	VK_memcpy(&v->Position, &t->original_position, sizeof(XYPosition));
	v->have_PaletteRef = t->have_original_palette_ref;
	if(v->have_PaletteRef)
		ObjectReference_dup(&v->PaletteRef, &t->original_palette_ref);

	/* VideoClass */
	v->VideoDecodeOffset.x_position = 0;
	v->VideoDecodeOffset.y_position = 0;

#if !defined(WIN32)
	pthread_mutex_init(&v->bbox_lock, NULL);
	pthread_mutex_init(&v->scaled_lock, NULL);
#endif

	v->scaled = false;
	v->no_video = false;

	return;
}

static void
free_VideoClassInstanceVars(VideoClassInstanceVars *v)
{
	if(v->have_PaletteRef)
		free_ObjectReference(&v->PaletteRef);

#if !defined(WIN32)
	pthread_mutex_destroy(&v->bbox_lock);
	pthread_mutex_destroy(&v->scaled_lock);
#endif

	return;
}

void
VideoClass_Preparation(VideoClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("VideoClass: %s; Preparation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been prepared */
	if(t->rootClass.inst.AvailabilityStatus)
		return;

	/* retrieve the object from an entity outside the engine and set each internal attribute of the object to its initial value */
	default_VideoClassInstanceVars(t, &t->inst);

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&t->rootClass);

	/* add it to the DisplayStack of the active application */
	MHEGEngine_addVisibleObject(&t->rootClass);

	return;
}

bool
VideoClass_Activation(VideoClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("VideoClass: %s; Activation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been activated */
	if(t->rootClass.inst.RunningStatus)
		return false;

	/* has it been prepared yet */
	if(!t->rootClass.inst.AvailabilityStatus)
	{
		/* generates an IsAvailable event */
		VideoClass_Preparation(t);
	}

	/* Activation inherited from the RootClass */
	RootClass_Activation(&t->rootClass);

#if 0 // hmkim : commented out.
	/* set its RunningStatus and generate IsRunning event */
	t->rootClass.inst.RunningStatus = true;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_is_running, NULL);
#endif

	/*
	 * tell our StreamClass to start playing us
	 * owner maybe NULL if our StreamClass is in the process of activating itself
	 * in which case, it will start us when needed
	 */
	if(t->inst.owner != NULL)
		StreamClass_activateVideoComponent(t->inst.owner, t);

	/* now its RunningStatus is true, get it drawn at its position in the application's DisplayStack */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return true;
}

bool
VideoClass_Deactivation(VideoClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("VideoClass: %s; Deactivation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already deactivated */
	if(!t->rootClass.inst.RunningStatus)
		return false;

	/* Deactivation inherited from the RootClass */
	RootClass_Deactivation(&t->rootClass);

	/*
	 * tell our StreamClass to stop playing us
	 * owner maybe NULL if our StreamClass is in the process of deactivating itself
	 * in which case, it will stop us when needed
	 */
	if(t->inst.owner != NULL)
		StreamClass_deactivateVideoComponent(t->inst.owner, t);

	/* now its RunningStatus is false, redraw the area it covered */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return true;
}

void
VideoClass_Destruction(VideoClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("VideoClass: %s; Destruction", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already destroyed */
	if(!t->rootClass.inst.AvailabilityStatus)
		return;

	/* remove it from the DisplayStack */
	MHEGEngine_removeVisibleObject(&t->rootClass);

	/* Deactivate it if it is running */
	if(t->rootClass.inst.RunningStatus)
	{
		/* generates an IsStopped event */
		VideoClass_Deactivation(t);
	}

	/*
	 * spec says we should handle caching here
	 * rb-download caches everything
	 */

	free_VideoClassInstanceVars(&t->inst);

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
VideoClass_SetPosition(VideoClass *t, SetPosition *params, OctetString *caller_gid)
{
//	XYPosition old;

//	old.x_position = t->inst.Position.x_position;
//	old.y_position = t->inst.Position.y_position;

#if !defined(WIN32)
	pthread_mutex_lock(&t->inst.bbox_lock);
#endif
	t->inst.Position.x_position = GenericInteger_getInteger(&params->new_x_position, caller_gid);
	t->inst.Position.y_position = GenericInteger_getInteger(&params->new_y_position, caller_gid);
#if !defined(WIN32)
	pthread_mutex_unlock(&t->inst.bbox_lock);
#endif

	verbose_class("VideoClass: %s; SetPosition; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.Position.x_position, t->inst.Position.y_position);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
	{
		MHEGEngine_redrawArea(&old, &t->inst.BoxSize);
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	}
	*/

	return;
}

void
VideoClass_GetPosition(VideoClass *t, GetPosition *params, OctetString *caller_gid)
{
	VariableClass *var;

	verbose_class("VideoClass: %s; GetPosition", ExternalReference_name(&t->rootClass.inst.ref));

	/* X position */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("VideoClass: GetPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Position.x_position);

	/* Y position */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("VideoClass: GetPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Position.y_position);

	return;
}

void
VideoClass_SetBoxSize(VideoClass *t, SetBoxSize *params, OctetString *caller_gid)
{
//	OriginalBoxSize old;

//	old.x_length = t->inst.BoxSize.x_length;
//	old.y_length = t->inst.BoxSize.y_length;

#if !defined(WIN32)
	pthread_mutex_lock(&t->inst.bbox_lock);
#endif
	t->inst.BoxSize.x_length = GenericInteger_getInteger(&params->x_new_box_size, caller_gid);
	t->inst.BoxSize.y_length = GenericInteger_getInteger(&params->y_new_box_size, caller_gid);
#if !defined(WIN32)
	pthread_mutex_unlock(&t->inst.bbox_lock);
#endif

	verbose_class("VideoClass: %s; SetBoxSize; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.BoxSize.x_length, t->inst.BoxSize.y_length);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
	{
		MHEGEngine_redrawArea(&t->inst.Position, &old);
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	}
	*/

	return;
}

void
VideoClass_GetBoxSize(VideoClass *t, GetBoxSize *params, OctetString *caller_gid)
{
	VariableClass *var;

	verbose_class("VideoClass: %s; GetBoxSize", ExternalReference_name(&t->rootClass.inst.ref));

	/* width */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_box_size_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("VideoClass: GetBoxSize: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BoxSize.x_length);

	/* height */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_box_size_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("VideoClass: GetBoxSize: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BoxSize.y_length);

	return;
}

void
VideoClass_BringToFront(VideoClass *t)
{
	verbose_class("VideoClass: %s; BringToFront", ExternalReference_name(&t->rootClass.inst.ref));

	MHEGEngine_bringToFront(&t->rootClass);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
VideoClass_SendToBack(VideoClass *t)
{
	verbose_class("VideoClass: %s; SendToBack", ExternalReference_name(&t->rootClass.inst.ref));

	MHEGEngine_sendToBack(&t->rootClass);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
VideoClass_PutBefore(VideoClass *t, PutBefore *params, OctetString *caller_gid)
{
	ObjectReference *ref;
	RootClass *obj;

	verbose_class("VideoClass: %s; PutBefore", ExternalReference_name(&t->rootClass.inst.ref));

	if(((ref = GenericObjectReference_getObjectReference(&params->reference_visible, caller_gid)) != NULL)
	&& ((obj = MHEGEngine_findObjectReference(ref, caller_gid)) != NULL))
	{
		MHEGEngine_putBefore(&t->rootClass, obj);

		/* if it is active, redraw it */
		/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
		if(t->rootClass.inst.RunningStatus)
			MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
		*/
	}

	return;
}

void
VideoClass_PutBehind(VideoClass *t, PutBehind *params, OctetString *caller_gid)
{
	ObjectReference *ref;
	RootClass *obj;

	verbose_class("VideoClass: %s; PutBehind", ExternalReference_name(&t->rootClass.inst.ref));

	if(((ref = GenericObjectReference_getObjectReference(&params->reference_visible, caller_gid)) != NULL)
	&& ((obj = MHEGEngine_findObjectReference(ref, caller_gid)) != NULL))
	{
		MHEGEngine_putBehind(&t->rootClass, obj);

		/* if it is active, redraw it */
		/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
		if(t->rootClass.inst.RunningStatus)
			MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
		*/
	}

	return;
}

void
VideoClass_SetVideoDecodeOffset(VideoClass *t, SetVideoDecodeOffset *params, OctetString *caller_gid)
{
	/* VideoDecodeOffset required in UK MHEG Profile but not required to support in CI+ MHEG Profile */

	/* The offset parameters may be negative. */
#if !defined(WIN32)
	pthread_mutex_lock(&t->inst.bbox_lock);
#endif
	t->inst.VideoDecodeOffset.x_position = GenericInteger_getInteger(&params->new_x_offset, caller_gid);
	t->inst.VideoDecodeOffset.y_position = GenericInteger_getInteger(&params->new_y_offset, caller_gid);
#if !defined(WIN32)
	pthread_mutex_unlock(&t->inst.bbox_lock);
#endif

	verbose_class("VideoClass: %s; SetVideoDecodeOffset; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.VideoDecodeOffset.x_position, t->inst.VideoDecodeOffset.y_position);

	/* screen will be updated next time we draw a video frame */

	verbose_todo("%s not yet implemented - should probably clear content Pixmap to black (x,y = t->inst.Position, w,h = t->inst.BoxSize)", __FUNCTION__);

	return;
}

void
VideoClass_GetVideoDecodeOffset(VideoClass *t, GetVideoDecodeOffset *params, OctetString *caller_gid)
{
	/* VideoDecodeOffset required in UK MHEG Profile but not required to support in CI+ MHEG Profile */

//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("VideoClass: %s; GetVideoDecodeOffset", ExternalReference_name(&t->rootClass.inst.ref));

	/* x offset : The offset values may be negative. */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_offset_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("VideoClass: GetVideoDecodeOffset: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.VideoDecodeOffset.x_position);

	/* y offset : The offset values may be negative. */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_offset_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("VideoClass: GetVideoDecodeOffset: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.VideoDecodeOffset.y_position);
}
#endif

	return;
}

void
VideoClass_ScaleVideo(VideoClass *t, ScaleVideo *params, OctetString *caller_gid)
{
	/* ScaleVideo required in UK MHEG Profile but not required to support in CI+ MHEG Profile */

	/* Note that this action does not affect the BoxSize internal attribute of the Video object. */
#if !defined(WIN32)
	pthread_mutex_lock(&t->inst.scaled_lock);
#endif
	t->inst.scaled_width = GenericInteger_getInteger(&params->x_scale, caller_gid);
	t->inst.scaled_height = GenericInteger_getInteger(&params->y_scale, caller_gid);
	if (t->inst.scaled_width > 0 && t->inst.scaled_height > 0)
	{
		t->inst.scaled = true;
	}
	else
	{
		error("VideoClass: %s; ScaleVideo: XScale %d and YScale %d shall be positive Integers.", ExternalReference_name(&t->rootClass.inst.ref), t->inst.scaled_width, t->inst.scaled_height);
	}
#if !defined(WIN32)
	pthread_mutex_unlock(&t->inst.scaled_lock);
#endif

	verbose_class("VideoClass: %s; ScaleVideo; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.scaled_width, t->inst.scaled_height);

	return;
}

void
VideoClass_render(VideoClass *t, MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	XYPosition ins_pos;
	OriginalBoxSize ins_box;
//	MHEGColour black;

	verbose_class("VideoClass: %s; render", ExternalReference_name(&t->rootClass.inst.ref));

	if(!intersects(pos, box, &t->inst.Position, &t->inst.BoxSize, &ins_pos, &ins_box))
		return;

	/*
	 * if we have no video stream, just draw a black rectangle
	 * if we do have a video stream, make a transparent hole in the MHEG overlay so we can see the video below it
	 */
#if 0 // org
	if(t->inst.no_video)
	{
		MHEGColour_black(&black);
		MHEGDisplay_fillRectangle(d, &ins_pos, &ins_box, &black);
	}
	else
	{
		MHEGDisplay_fillTransparentRectangle(d, &ins_pos, &ins_box);
	}
#else // 위처럼 배경을 검은색으로 그려서 video를 가리는 것이 옳은가..?
	MHEGDisplay_fillTransparentRectangle(d, &ins_pos, &ins_box);
#endif

	return;
}

