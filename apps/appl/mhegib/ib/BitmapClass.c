/*
 * BitmapClass.c
 */

#include "MHEGEngine.h"
#include "RootClass.h"
#include "Content.h"
#include "Reference.h"
#include "Generic.h"
#include "VariableClass.h"
#include "clone.h"

static void
default_BitmapClassInstanceVars(BitmapClass *t, BitmapClassInstanceVars *v)
{
	VK_memset(v, 0x00, sizeof(BitmapClassInstanceVars));

	/* VisibleClass */
	VK_memcpy(&v->BoxSize, &t->original_box_size, sizeof(OriginalBoxSize));
	VK_memcpy(&v->Position, &t->original_position, sizeof(XYPosition));
	v->have_PaletteRef = t->have_original_palette_ref;
	if(v->have_PaletteRef)
		ObjectReference_dup(&v->PaletteRef, &t->original_palette_ref);

	/* BitmapClass */
	v->Transparency = t->original_transparency;
	v->BitmapDecodeOffset.x_position = 0;
	v->BitmapDecodeOffset.y_position = 0;

	v->scaled = false;

	/* get the bitmap */
	if(t->have_original_content)
	{
		verbose_class("BitmapClass: %s; ContentBody_getContent", ExternalReference_name(&t->rootClass.inst.ref));
		ContentBody_getContent(&t->original_content, &t->rootClass, &v->BitmapData);
	}
	else
	{
		v->BitmapData.size = 0;
		v->BitmapData.data = NULL;
	}

	/* no internal representation yet, as the content may not be available */
	v->Bitmap = NULL;

	return;
}

static void
free_BitmapClassInstanceVars(BitmapClassInstanceVars *v)
{
	MHEGEngine_freeBitmap(v->Bitmap);

	if(v->have_PaletteRef)
		free_ObjectReference(&v->PaletteRef);

	free_OctetString(&v->BitmapData);

	return;
}

void
BitmapClass_Preparation(BitmapClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("BitmapClass: %s; Preparation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been prepared */
	if(t->rootClass.inst.AvailabilityStatus)
		return;

#if 1 // hmkim : added to get rid of any existing content. ContentBody_getContent(), NewContent_getContent() 가 사용되는 경우 필요함. 예: SetData -> Activation -> Preparation -> ContentBody_getContent.
	free_OctetString(&t->inst.BitmapData);
#endif

	/* retrieve the object from an entity outside the engine and set each internal attribute of the object to its initial value */
	default_BitmapClassInstanceVars(t, &t->inst);

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&t->rootClass);

	/* add it to the DisplayStack of the active application */
	MHEGEngine_addVisibleObject(&t->rootClass);

	return;
}

bool
BitmapClass_Activation(BitmapClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("BitmapClass: %s; Activation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been activated */
	if(t->rootClass.inst.RunningStatus)
		return false;

	/* has it been prepared yet */
	if(!t->rootClass.inst.AvailabilityStatus)
	{
		/* generates an IsAvailable event */
		BitmapClass_Preparation(t);
	}

	MHEGEngine_freeBitmap(t->inst.Bitmap); // hmkim : added to get rid of any existing content.

	/* convert it to MHEGDisplay's internal format */
	t->inst.Bitmap = MHEGEngine_newBitmap(&t->inst.BitmapData, t->have_content_hook, t->content_hook);

#if 0 // org
	/* set its RunningStatus and generate IsRunning event */
	t->rootClass.inst.RunningStatus = true;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_is_running, NULL);
#else // hmkim : modified.
	/* Activation inherited from the RootClass */
	RootClass_Activation(&t->rootClass);
#endif

	/* now its RunningStatus is true, get it drawn at its position in the application's DisplayStack */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return true;
}

bool
BitmapClass_Deactivation(BitmapClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("BitmapClass: %s; Deactivation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already deactivated */
	if(!t->rootClass.inst.RunningStatus)
		return false;

	/* Deactivation inherited from the RootClass */
	RootClass_Deactivation(&t->rootClass);

	/* now its RunningStatus is false, redraw the area it covered */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return true;
}

void
BitmapClass_Destruction(BitmapClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("BitmapClass: %s; Destruction", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already destroyed */
	if(!t->rootClass.inst.AvailabilityStatus)
		return;

	/* if we are waiting for content, remove ourselves from the missing content list */
	if(t->rootClass.inst.need_content)
	{
		MHEGEngine_enterCriticalMissingContent;
		MHEGEngine_removeMissingContent(&t->rootClass);
		MHEGEngine_leaveCriticalMissingContent;
	}

	/* remove it from the DisplayStack */
	MHEGEngine_removeVisibleObject(&t->rootClass);

	/* Deactivate it if it is running */
	if(t->rootClass.inst.RunningStatus)
	{
		/* generates an IsStopped event */
		BitmapClass_Deactivation(t);
	}

	/*
	 * spec says we should handle caching here
	 * rb-download caches everything
	 */

	free_BitmapClassInstanceVars(&t->inst);

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
BitmapClass_SetData(BitmapClass *t, SetData *set, OctetString *caller_gid)
{
	verbose_class("BitmapClass: %s; SetData", ExternalReference_name(&t->rootClass.inst.ref));

	#if 0 // hmkim : moved into NewContent_getContent() and below.
	/* get rid of any existing content */
	free_OctetString(&t->inst.BitmapData);
	MHEGEngine_freeBitmap(t->inst.Bitmap);
	t->inst.Bitmap = NULL;
	#endif

	/* TODO : A Set Data elementary action resets the scaling factor of the bitmap to its original value. */

	/*
	 * the content may need to be loaded from an external file
	 * if the file is not available, this returns false and calls BitmapClass_contentAvailable() when it appears
	 */
	if(NewContent_getContent(&set->new_content, caller_gid, &t->rootClass, &t->inst.BitmapData))
	{
		MHEGEngine_freeBitmap(t->inst.Bitmap); // hmkim : added to get rid of any existing content.

		/* convert it to MHEGDisplay's internal format */
		t->inst.Bitmap = MHEGEngine_newBitmap(&t->inst.BitmapData, t->have_content_hook, t->content_hook);

		/* if it is active, redraw the area it covers */
		/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
		if(t->rootClass.inst.RunningStatus)
			MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
		*/

		/* ContentPreparation behaviour specified in the ISO MHEG Corrigendum */
		MHEGEngine_generateAsyncEvent(&t->rootClass.inst.ref, EventType_content_available, NULL);
	}

	return;
}

void
BitmapClass_Clone(BitmapClass *t, Clone *params, OctetString *caller_gid)
{
	verbose_class("BitmapClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));

	CLONE_OBJECT(t, BitmapClass, bitmap);

	return;
}

void
BitmapClass_SetPosition(BitmapClass *t, SetPosition *params, OctetString *caller_gid)
{
//	XYPosition old;

//	old.x_position = t->inst.Position.x_position;
//	old.y_position = t->inst.Position.y_position;

	t->inst.Position.x_position = GenericInteger_getInteger(&params->new_x_position, caller_gid);
	t->inst.Position.y_position = GenericInteger_getInteger(&params->new_y_position, caller_gid);

	verbose_class("BitmapClass: %s; SetPosition; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.Position.x_position, t->inst.Position.y_position);

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
BitmapClass_GetPosition(BitmapClass *t, GetPosition *params, OctetString *caller_gid)
{
	VariableClass *var;

	verbose_class("BitmapClass: %s; GetPosition", ExternalReference_name(&t->rootClass.inst.ref));

	/* X position */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("BitmapClass: GetPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Position.x_position);

	/* Y position */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("BitmapClass: GetPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Position.y_position);

	return;
}

void
BitmapClass_SetBoxSize(BitmapClass *t, SetBoxSize *params, OctetString *caller_gid)
{
//	OriginalBoxSize old;

//	old.x_length = t->inst.BoxSize.x_length;
//	old.y_length = t->inst.BoxSize.y_length;

	t->inst.BoxSize.x_length = GenericInteger_getInteger(&params->x_new_box_size, caller_gid);
	t->inst.BoxSize.y_length = GenericInteger_getInteger(&params->y_new_box_size, caller_gid);

	verbose_class("BitmapClass: %s; SetBoxSize; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.BoxSize.x_length, t->inst.BoxSize.y_length);

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
BitmapClass_GetBoxSize(BitmapClass *t, GetBoxSize *params, OctetString *caller_gid)
{
	VariableClass *var;

	verbose_class("BitmapClass: %s; GetBoxSize", ExternalReference_name(&t->rootClass.inst.ref));

	/* width */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_box_size_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("BitmapClass: GetBoxSize: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BoxSize.x_length);

	/* height */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_box_size_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("BitmapClass: GetBoxSize: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BoxSize.y_length);

	return;
}

void
BitmapClass_BringToFront(BitmapClass *t)
{
	verbose_class("BitmapClass: %s; BringToFront", ExternalReference_name(&t->rootClass.inst.ref));

	MHEGEngine_bringToFront(&t->rootClass);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
BitmapClass_SendToBack(BitmapClass *t)
{
	verbose_class("BitmapClass: %s; SendToBack", ExternalReference_name(&t->rootClass.inst.ref));

	MHEGEngine_sendToBack(&t->rootClass);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
BitmapClass_PutBefore(BitmapClass *t, PutBefore *params, OctetString *caller_gid)
{
	ObjectReference *ref;
	RootClass *obj;

	verbose_class("BitmapClass: %s; PutBefore", ExternalReference_name(&t->rootClass.inst.ref));

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
BitmapClass_PutBehind(BitmapClass *t, PutBehind *params, OctetString *caller_gid)
{
	ObjectReference *ref;
	RootClass *obj;

	verbose_class("BitmapClass: %s; PutBehind", ExternalReference_name(&t->rootClass.inst.ref));

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
BitmapClass_SetPaletteRef(BitmapClass *t, SetPaletteRef *params, OctetString *caller_gid)
{
	/* UK MHEG Profile says we don't support PaletteClass */
	error("BitmapClass: %s; SetPaletteRef not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
BitmapClass_SetBitmapDecodeOffset(BitmapClass *t, SetBitmapDecodeOffset *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.

	/* The offset parameters may be negative. */
	t->inst.BitmapDecodeOffset.x_position = GenericInteger_getInteger(&params->new_x_offset, caller_gid);
	t->inst.BitmapDecodeOffset.y_position = GenericInteger_getInteger(&params->new_y_offset, caller_gid);

	verbose_class("BitmapClass: %s; SetBitmapDecodeOffset; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.BitmapDecodeOffset.x_position, t->inst.BitmapDecodeOffset.y_position);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

#endif

	return;
}

void
BitmapClass_GetBitmapDecodeOffset(BitmapClass *t, GetBitmapDecodeOffset *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("BitmapClass: %s; GetBitmapDecodeOffset", ExternalReference_name(&t->rootClass.inst.ref));

	/* x offset : The offset values may be negative. */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_offset_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("BitmapClass: GetBitmapDecodeOffset: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BitmapDecodeOffset.x_position);

	/* y offset : The offset values may be negative. */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_offset_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("BitmapClass: GetBitmapDecodeOffset: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BitmapDecodeOffset.y_position);
}
#endif

	return;
}

void
BitmapClass_ScaleBitmap(BitmapClass *t, ScaleBitmap *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

	/* Note that this action does not affect the BoxSize internal attribute of the Bitmap object; in other words, the Bitmap is scaled, but its bounding box remains the same. */
	t->inst.scaled_width = GenericInteger_getInteger(&params->x_scale, caller_gid);
	t->inst.scaled_height = GenericInteger_getInteger(&params->y_scale, caller_gid);
	if (t->inst.scaled_width > 0 && t->inst.scaled_height > 0)
	{
		t->inst.scaled = true;
	}
	else
	{
		error("BitmapClass: %s; ScaleBitmap: XScale %d and YScale %d shall be positive Integers", ExternalReference_name(&t->rootClass.inst.ref), t->inst.scaled_width, t->inst.scaled_height);
	}

	verbose_class("BitmapClass: %s; ScaleBitmap; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.scaled_width, t->inst.scaled_height);

	return;
}

void
BitmapClass_SetTransparency(BitmapClass *t, SetTransparency *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int NewTransparency = GenericInteger_getInteger(&params->new_transparency, caller_gid);

	#if 0
	verbose_class("BitmapClass: %s; SetTransparency; %d", ExternalReference_name(&t->rootClass.inst.ref), NewTransparency);

	/* ISO/IEC 13522-5 : NewTransparency is set within the range [0, 100] */
	if (NewTransparency < 0 || NewTransparency > 100)
	{
		error("BitmapClass: %s; SetTransparency: NewTransparency %d is set within the range [0, 100]", ExternalReference_name(&t->rootClass.inst.ref), NewTransparency);
		return;
	}

	if (t->inst.Transparency != NewTransparency)
	{
		t->inst.Transparency = NewTransparency;

		/* if it is active, redraw it */
		/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
		if(t->rootClass.inst.RunningStatus)
			MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
		*/
	}
	#else
	/* UK MHEG Profile (D-BOOK 5.0) : Support for object level transparency (i.e. the Transparency internal attribute of the Bitmap class) is not required for any bitmap type. */
	error("BitmapClass: %s; SetTransparency; %d not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref), NewTransparency);
	#endif
}
#endif

	return;
}

/*
 * called when the content is available
 */

void
BitmapClass_contentAvailable(BitmapClass *t, OctetString *file)
{
	/* get rid of any existing content */
	free_OctetString(&t->inst.BitmapData);
	MHEGEngine_freeBitmap(t->inst.Bitmap);

	/* load the new content */
	MHEGEngine_loadFile(file, &t->inst.BitmapData);

	/* convert it to MHEGDisplay's internal format */
	t->inst.Bitmap = MHEGEngine_newBitmap(&t->inst.BitmapData, t->have_content_hook, t->content_hook);

	/* if it is active, redraw the area it covers */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
BitmapClass_render(BitmapClass *t, MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	XYPosition ins_pos;
	OriginalBoxSize ins_box;
	XYPosition src;

	verbose_class("BitmapClass: %s; render", ExternalReference_name(&t->rootClass.inst.ref));

	if(!intersects(pos, box, &t->inst.Position, &t->inst.BoxSize, &ins_pos, &ins_box))
		return;

	MHEGDisplay_setClipRectangle(d, &ins_pos, &ins_box);

	/* work out where the intersection starts on the bitmap */
	src.x_position = ins_pos.x_position - t->inst.Position.x_position;
	src.y_position = ins_pos.y_position - t->inst.Position.y_position;

/****************************************************************************************/
/*TODO */
/* take tiling into account */
/* take BitmapDecodeOffset into account */
/* if we are not tiling and the actual bitmap is a different size than t->inst.BoxSize do we need to scale it? */
/* can probably get XRender to tile it for us */
/****************************************************************************************/
/* UK MHEG Profile (D-BOOK 5.0) :
   * Scaling : Scaling (the ScaleBitmap action) shall be supported for bitmap objects with MPEG I-frame content.
			   Scaling shall not be supported for Bitmap objects using PNG bitmaps.
   * Tiling : Support for tiling is only required for PNG bitmaps.
   * Transparency : Transparency can be encoded within the PNG bitmaps.
					Support for object level transparency (i.e. the Transparency internal attribute of the Bitmap class) is not required for any bitmap type.
   * BitmapDecodeOffset : only required for MPEG I-frame not PNG bitmaps.
*/

	/* use scaled values if ScaleBitmap has been called */
	/* Note that ScaleBitmap action does not affect the BoxSize internal attribute of the Bitmap object; in other words, the Bitmap is scaled, but its bounding box remains the same. */
	if (t->inst.scaled)
	{
		if (t->have_content_hook == true && t->content_hook == MHEG_ContentHook_Bitmap_MPEG)
		{
			ins_box.x_length = t->inst.scaled_width;
			ins_box.y_length = t->inst.scaled_height;
		}
		else
		{
			error("BitmapClass: %s; render: bitmap has been scaled but I-frame content not available", ExternalReference_name(&t->rootClass.inst.ref));
		}
	}

	MHEGDisplay_drawBitmap(d, &src, &ins_box, t->inst.Bitmap, &ins_pos, t->tiling);

	MHEGDisplay_unsetClipRectangle(d);

	return;
}

