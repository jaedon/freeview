/*
 * HyperTextClass.c
 */

#include "MHEGEngine.h"
#include "HyperTextClass.h"
#include "RootClass.h"
#include "ApplicationClass.h"
#include "Content.h"
#include "Reference.h"
#include "VariableClass.h"
#include "Generic.h"
#include "clone.h"

static void
default_HyperTextClassInstanceVars(HyperTextClass *t, HyperTextClassInstanceVars *v)
{
	VK_memset(v, 0x00, sizeof(HyperTextClassInstanceVars));

/* TODO */
/* this code is duplicated in TextClass, EntryFieldClass and HyperTextClass */
/* except the InteractibleClass initialisation is not in TextClass */

	/* VisibleClass */
	VK_memcpy(&v->BoxSize, &t->original_box_size, sizeof(OriginalBoxSize));
	VK_memcpy(&v->Position, &t->original_position, sizeof(XYPosition));
	v->have_PaletteRef = t->have_original_palette_ref;
	if(v->have_PaletteRef)
		ObjectReference_dup(&v->PaletteRef, &t->original_palette_ref);

	/* InteractibleClass */
	v->HighlightStatus = false;
	v->InteractionStatus = false;

	/* TextClass */
	if(t->have_original_content)
	{
		verbose_class("HyperTextClass: %s; ContentBody_getContent", ExternalReference_name(&t->rootClass.inst.ref));
		ContentBody_getContent(&t->original_content, &t->rootClass, &v->TextData);
	}
	else
	{
		v->TextData.size = 0;
		v->TextData.data = NULL;
	}

	/* defaults come from the ApplicationClass */
	MHEGFont_init(&v->Font);
	if(t->have_original_font)
	{
		MHEGFont_setName(&v->Font, &t->original_font);
	}
	else
	{
		DefaultAttribute *attr = ApplicationClass_getDefaultAttribute(MHEGEngine_getActiveApplication(), DefaultAttribute_font);
		FontBody *font = (attr != NULL) ? &attr->u.font : NULL;
		if(font != NULL)
			MHEGFont_setName(&v->Font, font);
		else
			MHEGFont_defaultName(&v->Font);
	}

	if(t->have_text_colour)
	{
		MHEGColour_fromColour(&v->TextColour, &t->text_colour);
	}
	else
	{
		DefaultAttribute *attr = ApplicationClass_getDefaultAttribute(MHEGEngine_getActiveApplication(), DefaultAttribute_text_colour);
		Colour *col = (attr != NULL) ? &attr->u.text_colour : NULL;
		/* UK MHEG Profile says default is white */
		if(col != NULL)
			MHEGColour_fromColour(&v->TextColour, col);
		else
			MHEGColour_white(&v->TextColour);
	}

	if(t->have_background_colour)
	{
		MHEGColour_fromColour(&v->BackgroundColour, &t->background_colour);
	}
	else
	{
		DefaultAttribute *attr = ApplicationClass_getDefaultAttribute(MHEGEngine_getActiveApplication(), DefaultAttribute_background_colour);
		Colour *col = (attr != NULL) ? &attr->u.background_colour : NULL;
		/* default is transparent */
		if(col != NULL)
			MHEGColour_fromColour(&v->BackgroundColour, col);
		else
			MHEGColour_transparent(&v->BackgroundColour);
	}

#if 1 // hmkim : added.

	if(t->have_highlight_ref_colour)
	{
		MHEGColour_fromColour(&v->HighlightRefColour, &t->highlight_ref_colour);
	}
	else
	{
		DefaultAttribute *attr = ApplicationClass_getDefaultAttribute(MHEGEngine_getActiveApplication(), DefaultAttribute_highlight_ref_colour);
		Colour *col = (attr != NULL) ? &attr->u.highlight_ref_colour : NULL;
		/* default is transparent */
		if(col != NULL)
			MHEGColour_fromColour(&v->HighlightRefColour, col);
		else
			MHEGColour_transparent(&v->HighlightRefColour);
	}

#endif

	if(t->have_font_attributes)
	{
		MHEGFont_setAttributes(&v->Font, &t->font_attributes);
	}
	else
	{
		DefaultAttribute *attr = ApplicationClass_getDefaultAttribute(MHEGEngine_getActiveApplication(), DefaultAttribute_font_attributes);
		OctetString *font_attr = (attr != NULL) ? &attr->u.font_attributes : NULL;
		if(font_attr != NULL)
		{
			MHEGFont_setAttributes(&v->Font, font_attr);
		}
		else
		{
			verbose_class("HyperTextClass: %s; MHEGFont_defaultAttributes (%s)", ExternalReference_name(&t->rootClass.inst.ref), v->Font.name);
			MHEGFont_defaultAttributes(&v->Font);
		}
	}

	v->element = NULL;

	/* HyperTextClass */
	v->LastAnchorFired.size = 0;
	v->LastAnchorFired.data = NULL;

	v->FocusPosition = 0; /* no anchor to highlight */

	return;
}

static void
free_HyperTextClassInstanceVars(HyperTextClassInstanceVars *v)
{
	if(v->have_PaletteRef)
		free_ObjectReference(&v->PaletteRef);

	free_OctetString(&v->TextData);

	free_MHEGFont(&v->Font);

	LIST_FREE(&v->element, MHEGTextElement, safe_free);

	free_OctetString(&v->LastAnchorFired);

	return;
}

void
HyperTextClass_Preparation(HyperTextClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("HyperTextClass: %s; Preparation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been prepared */
	if(t->rootClass.inst.AvailabilityStatus)
		return;

#if 1 // hmkim : added to get rid of any existing content. ContentBody_getContent(), NewContent_getContent() 가 사용되는 경우 필요함. 예: SetData -> Activation -> Preparation -> ContentBody_getContent.
	free_OctetString(&t->inst.TextData);
#endif

	/* retrieve the object from an entity outside the engine and set each internal attribute of the object to its initial value */
	default_HyperTextClassInstanceVars(t, &t->inst);

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&t->rootClass);

	/* add it to the DisplayStack of the active application */
	MHEGEngine_addVisibleObject(&t->rootClass);

	return;
}

bool
HyperTextClass_Activation(HyperTextClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("HyperTextClass: %s; Activation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been activated */
	if(t->rootClass.inst.RunningStatus)
		return false;

	/* has it been prepared yet */
	if(!t->rootClass.inst.AvailabilityStatus)
	{
		/* generates an IsAvailable event */
		HyperTextClass_Preparation(t);
	}

	/* Activation inherited from the RootClass */
	RootClass_Activation(&t->rootClass);

/**************************************************************************/
/* anything you do here, you probably also need to do in contentAvailable() */
/**************************************************************************/

#if 0 // hmkim : commented out.
	/* set its RunningStatus and generate IsRunning event */
	t->rootClass.inst.RunningStatus = true;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_is_running, NULL);
#endif

	/* now its RunningStatus is true, get it drawn at its position in the application's DisplayStack */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return true;
}

bool
HyperTextClass_Deactivation(HyperTextClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("HyperTextClass: %s; Deactivation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already deactivated */
	if(!t->rootClass.inst.RunningStatus)
		return false;

	/* ISO/IEC 13522-5 Cor.1 : Setting the InteractionStatus attribute to False before deactivating an Interactible object
	   Deactivation Set the InteractionStatus internal attribute to False.
	   NOTE - The InteractionStatus internal attribute of an Interactible object needs to be set to False when the object is deactivated
	          because the SetInteractionStatus elementary action can only be targeted to active Interactibles. */
	t->inst.InteractionStatus = false;

	/* Deactivation inherited from the RootClass */
	RootClass_Deactivation(&t->rootClass);

	/* now its RunningStatus is false, redraw the area it covered */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return true;
}

void
HyperTextClass_Destruction(HyperTextClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("HyperTextClass: %s; Destruction", ExternalReference_name(&t->rootClass.inst.ref));
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
		HyperTextClass_Deactivation(t);
	}

	/*
	 * spec says we should handle caching here
	 * rb-download caches everything
	 */

	free_HyperTextClassInstanceVars(&t->inst);

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
HyperTextClass_SetData(HyperTextClass *t, SetData *set, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.

	verbose_class("HyperTextClass: %s; SetData", ExternalReference_name(&t->rootClass.inst.ref));

	/*
	 * the content may need to be loaded from an external file
	 * if the file is not available, this returns false and calls HyperTextClass_contentAvailable() when it appears
	 */
	if(NewContent_getContent(&set->new_content, caller_gid, &t->rootClass, &t->inst.TextData))
	{
		LIST_FREE(&t->inst.element, MHEGTextElement, safe_free); // get rid of any existing content.

		/* if it is active, redraw the area it covers */
		/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
		if(t->rootClass.inst.RunningStatus)
			MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
		*/

		/* ContentPreparation behaviour specified in the ISO MHEG Corrigendum */
		MHEGEngine_generateAsyncEvent(&t->rootClass.inst.ref, EventType_content_available, NULL);
	}

#endif

	return;
}

void
HyperTextClass_Clone(HyperTextClass *t, Clone *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

	verbose_class("HyperTextClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));

	CLONE_OBJECT(t, HyperTextClass, hyper_text);

	return;
}

void
HyperTextClass_SetPosition(HyperTextClass *t, SetPosition *params, OctetString *caller_gid)
{
//	XYPosition old;

//	old.x_position = t->inst.Position.x_position;
//	old.y_position = t->inst.Position.y_position;

	t->inst.Position.x_position = GenericInteger_getInteger(&params->new_x_position, caller_gid);
	t->inst.Position.y_position = GenericInteger_getInteger(&params->new_y_position, caller_gid);

	verbose_class("HyperTextClass: %s; SetPosition; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.Position.x_position, t->inst.Position.y_position);

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
HyperTextClass_GetPosition(HyperTextClass *t, GetPosition *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("HyperTextClass: %s; GetPosition", ExternalReference_name(&t->rootClass.inst.ref));

	/* X position */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("HyperTextClass: GetPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Position.x_position);

	/* Y position */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("HyperTextClass: GetPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Position.y_position);
}
#endif

	return;
}

void
HyperTextClass_SetBoxSize(HyperTextClass *t, SetBoxSize *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
//	OriginalBoxSize old;

//	old.x_length = t->inst.BoxSize.x_length;
//	old.y_length = t->inst.BoxSize.y_length;

	t->inst.BoxSize.x_length = GenericInteger_getInteger(&params->x_new_box_size, caller_gid);
	t->inst.BoxSize.y_length = GenericInteger_getInteger(&params->y_new_box_size, caller_gid);

	verbose_class("HyperTextClass: %s; SetBoxSize; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.BoxSize.x_length, t->inst.BoxSize.y_length);

	/* remove the previous layout info, gets recalculated when we redraw it */
	LIST_FREE(&t->inst.element, MHEGTextElement, safe_free);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
	{
		MHEGEngine_redrawArea(&t->inst.Position, &old);
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	}
	*/
}
#endif

	return;
}

void
HyperTextClass_GetBoxSize(HyperTextClass *t, GetBoxSize *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("HyperTextClass: %s; GetBoxSize", ExternalReference_name(&t->rootClass.inst.ref));

	/* width */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_box_size_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("HyperTextClass: GetBoxSize: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BoxSize.x_length);

	/* height */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_box_size_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("HyperTextClass: GetBoxSize: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BoxSize.y_length);
}
#endif

	return;
}

void
HyperTextClass_BringToFront(HyperTextClass *t)
{
	verbose_class("HyperTextClass: %s; BringToFront", ExternalReference_name(&t->rootClass.inst.ref));

	MHEGEngine_bringToFront(&t->rootClass);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
HyperTextClass_SendToBack(HyperTextClass *t)
{
	verbose_class("HyperTextClass: %s; SendToBack", ExternalReference_name(&t->rootClass.inst.ref));

	MHEGEngine_sendToBack(&t->rootClass);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
HyperTextClass_PutBefore(HyperTextClass *t, PutBefore *params, OctetString *caller_gid)
{
	ObjectReference *ref;
	RootClass *obj;

	verbose_class("HyperTextClass: %s; PutBefore", ExternalReference_name(&t->rootClass.inst.ref));

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
HyperTextClass_PutBehind(HyperTextClass *t, PutBehind *params, OctetString *caller_gid)
{
	ObjectReference *ref;
	RootClass *obj;

	verbose_class("HyperTextClass: %s; PutBehind", ExternalReference_name(&t->rootClass.inst.ref));

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
HyperTextClass_SetPaletteRef(HyperTextClass *t, SetPaletteRef *params, OctetString *caller_gid)
{
	/* UK MHEG Profile says we don't support PaletteClass */
	error("HyperTextClass: %s; SetPaletteRef not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
HyperTextClass_SetBackgroundColour(HyperTextClass *t, SetBackgroundColour *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.

	verbose_class("HyperTextClass: %s; SetBackgroundColour", ExternalReference_name(&t->rootClass.inst.ref));

	MHEGColour_fromNewColour(&t->inst.BackgroundColour, &params->new_background_colour, caller_gid);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

#endif

	return;
}

void
HyperTextClass_SetTextColour(HyperTextClass *t, SetTextColour *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.

	verbose_class("HyperTextClass: %s; SetTextColour", ExternalReference_name(&t->rootClass.inst.ref));

	MHEGColour_fromNewColour(&t->inst.TextColour, &params->new_text_colour, caller_gid);

	/* remove the previous layout info, colours get recalculated when we redraw it */
	LIST_FREE(&t->inst.element, MHEGTextElement, safe_free);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

#endif

	return;
}

void
HyperTextClass_SetFontAttributes(HyperTextClass *t, SetFontAttributes *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	OctetString *attr;

	verbose_class("HyperTextClass: %s; SetFontAttributes", ExternalReference_name(&t->rootClass.inst.ref));

	attr = GenericOctetString_getOctetString(&params->new_font_attribute, caller_gid);

	MHEGFont_setAttributes(&t->inst.Font, attr);

	/* remove the previous layout info, gets recalculated when we redraw it */
	LIST_FREE(&t->inst.element, MHEGTextElement, safe_free);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/
}
#endif

	return;
}

void
HyperTextClass_GetFocusPosition(HyperTextClass *t, GetFocusPosition *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("HyperTextClass: %s; GetFocusPosition", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->focus_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("HyperTextClass: GetFocusPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.FocusPosition);
}
#endif

	return;
}

void
HyperTextClass_SetFocusPosition(HyperTextClass *t, SetFocusPosition *params, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int NewFocusPosition = GenericInteger_getInteger(&params->new_focus_position, caller_gid);
	EventData event_tag;

	verbose_class("HyperTextClass: %s; SetFocusPosition; %d", ExternalReference_name(&t->rootClass.inst.ref), NewFocusPosition);

	/* UK MHEG Profile (D-BOOK 5.0) : If NewFocusPosition is greater than N, where N is the number of anchors available in the content for the target object then it shall be treated as N. */
	#if 0 // TODO this ...
	if (NewFocusPosition > N)
	{
		NewFocusPosition = N;
	}
	#endif

	if (t->inst.FocusPosition != NewFocusPosition)
	{
		t->inst.FocusPosition = NewFocusPosition;

		/* if it is active, redraw it */
		/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
		if(t->rootClass.inst.RunningStatus)
			MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
		*/

		event_tag.choice = EventData_integer;
		event_tag.u.integer = t->inst.FocusPosition;
		MHEGEngine_generateAsyncEvent(&t->rootClass.inst.ref, EventType_focus_moved, &event_tag);
	}
}
#endif

	return;
}

void
HyperTextClass_GetTextContent(HyperTextClass *t, GetTextContent *params, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__);

	verbose_class("HyperTextClass: %s; GetTextContent", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

/*
 * caller_gid is used to resolve the text_data_var ObjectReference
 */

void
HyperTextClass_GetTextData(HyperTextClass *t, GetTextData *get, OctetString *caller_gid)
{
	VariableClass *var;

	verbose_class("HyperTextClass: %s; GetTextData", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&get->text_data_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_octetstring)
	{
		error("HyperTextClass: GetTextData: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	OctetStringVariableClass_setOctetString(var, &t->inst.TextData);

	return;
}

void
HyperTextClass_SetFontRef(HyperTextClass *t, SetFontRef *params, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__); /* 특히 CI+ MHEG Profile 에서 필요 */

	verbose_class("HyperTextClass: %s; SetFontRef", ExternalReference_name(&t->rootClass.inst.ref));

	/* Set the value of the Font attribute to Newfont */

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
HyperTextClass_SetInteractionStatus(HyperTextClass *t, SetInteractionStatus *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	bool OldInteractionStatus = t->inst.InteractionStatus;
	bool NewInteractionStatus = GenericBoolean_getBoolean(&params->new_interaction_status, caller_gid);
	SceneClass *scene = MHEGEngine_getActiveScene();

	verbose_class("HyperTextClass: %s; SetInteractionStatus; %s", ExternalReference_name(&t->rootClass.inst.ref), NewInteractionStatus ? "true" : "false");

	/* If NewlnteractionStatus is set to True and if the target lnteractible or any other lnteractible in the current Scene has its InteractionStatus attribute set to True, discard the action. */
	if(NewInteractionStatus == true && scene != NULL)
	{
		LIST_TYPE(GroupItem) *gi;
		gi = scene->items;
		while(gi)
		{
			if (gi->item.choice == GroupItem_entry_field
				&& gi->item.u.entry_field.rootClass.inst.AvailabilityStatus
				&& gi->item.u.entry_field.rootClass.inst.RunningStatus
				&& gi->item.u.entry_field.inst.InteractionStatus)
			{
				verbose_class("HyperTextClass: %s; SetInteractionStatus; EntryField %s has true InteractionStatus -> discard the action",
					ExternalReference_name(&t->rootClass.inst.ref), ExternalReference_name(&gi->item.u.entry_field.rootClass.inst.ref));
				return;
			}
			else if (gi->item.choice == GroupItem_hyper_text
				&& gi->item.u.hyper_text.rootClass.inst.AvailabilityStatus
				&& gi->item.u.hyper_text.rootClass.inst.RunningStatus
				&& gi->item.u.hyper_text.inst.InteractionStatus)
			{
				verbose_class("HyperTextClass: %s; SetInteractionStatus; HyperText %s has true InteractionStatus -> discard the action",
					ExternalReference_name(&t->rootClass.inst.ref), ExternalReference_name(&gi->item.u.hyper_text.rootClass.inst.ref));
				return;
			}
			else if (gi->item.choice == GroupItem_slider
				&& gi->item.u.slider.rootClass.inst.AvailabilityStatus
				&& gi->item.u.slider.rootClass.inst.RunningStatus
				&& gi->item.u.slider.inst.InteractionStatus)
			{
				verbose_class("HyperTextClass: %s; SetInteractionStatus; Slider %s has true InteractionStatus -> discard the action",
					ExternalReference_name(&t->rootClass.inst.ref), ExternalReference_name(&gi->item.u.slider.rootClass.inst.ref));
				return;
			}
			gi = gi->next;
		}
	}

	if (OldInteractionStatus != NewInteractionStatus)
		t->inst.InteractionStatus = NewInteractionStatus;

	if (OldInteractionStatus == true && NewInteractionStatus == false)
		MHEGEngine_generateAsyncEvent(&t->rootClass.inst.ref, EventType_interaction_completed, NULL);
}
#endif

	return;
}

void
HyperTextClass_GetInteractionStatus(HyperTextClass *t, GetInteractionStatus *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("HyperTextClass: %s; GetInteractionStatus", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->interaction_status_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_boolean)
	{
		error("HyperTextClass: GetInteractionStatus: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	BooleanVariableClass_setBoolean(var, t->inst.InteractionStatus);
}
#endif

	return;
}

void
HyperTextClass_SetHighlightStatus(HyperTextClass *t, SetHighlightStatus *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	bool OldHighlightStatus = t->inst.HighlightStatus;
	bool NewHighlightStatus = GenericBoolean_getBoolean(&params->new_highlight_status, caller_gid);

	verbose_class("HyperTextClass: %s; SetHighlightStatus; %s", ExternalReference_name(&t->rootClass.inst.ref), NewHighlightStatus ? "true" : "false");

	if (OldHighlightStatus != NewHighlightStatus)
	{
		t->inst.HighlightStatus = NewHighlightStatus;

		/* remove the previous layout info, colours get recalculated when we redraw it */
		LIST_FREE(&t->inst.element, MHEGTextElement, safe_free);

		/* if it is active, redraw it */ /* UK MHEG Profile (D-BOOK 5.0) : 15.8.2.3 Highlight (HyperText) - The HighlightStatus attribute shall not affect the visible appearance of HyperText objects. */
		/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
		if(t->rootClass.inst.RunningStatus && t->engine_resp)
			MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
		*/
	}

	if (OldHighlightStatus == false && NewHighlightStatus == true)
		MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_highlight_on, NULL);

	if (OldHighlightStatus == true && NewHighlightStatus == false)
		MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_highlight_off, NULL);
}
#endif

	return;
}

void
HyperTextClass_GetHighlightStatus(HyperTextClass *t, GetHighlightStatus *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("HyperTextClass: %s; GetHighlightStatus", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->highlight_status_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_boolean)
	{
		error("HyperTextClass: GetHighlightStatus: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	BooleanVariableClass_setBoolean(var, t->inst.HighlightStatus);
}
#endif

	return;
}

void
HyperTextClass_GetLastAnchorFired(HyperTextClass *t, GetLastAnchorFired *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("HyperTextClass: %s; GetLastAnchorFired", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->last_anchor_fired_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_octetstring)
	{
		error("HyperTextClass: GetLastAnchorFired: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	OctetStringVariableClass_setOctetString(var, &t->inst.LastAnchorFired);
}
#endif

	return;
}

/*
 * called when the content is available
 */

void
HyperTextClass_contentAvailable(HyperTextClass *t, OctetString *file)
{
	/* get rid of any existing content */
	free_OctetString(&t->inst.TextData);
	LIST_FREE(&t->inst.element, MHEGTextElement, safe_free);

	/* load the new content */
	MHEGEngine_loadFile(file, &t->inst.TextData);

	/* if it is active, redraw the area it covers */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
HyperTextClass_render(HyperTextClass *t, MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	XYPosition ins_pos;
	OriginalBoxSize ins_box;

	verbose_class("HyperTextClass: %s; render", ExternalReference_name(&t->rootClass.inst.ref));

	if(!intersects(pos, box, &t->inst.Position, &t->inst.BoxSize, &ins_pos, &ins_box))
		return;

	MHEGDisplay_setClipRectangle(d, &ins_pos, &ins_box);

	verbose_todo("%s not yet implemented", __FUNCTION__);

if (g_MhegIbDbgFlag & MHEG_IB_DBG_MASK_CLS)
{
	Hx_Print("---< HyperTextClass_render >---\n");
	Hx_Print("HyperTextClass :\n"); // HyperTextClassInstanceVars 에 셋팅되지 않는 것들만 프린트한다.
	Hx_Print("  have_character_set = %d %d\n", t->have_character_set, t->character_set);
	Hx_Print("  have_background_colour = %d %d\n", t->have_background_colour, t->background_colour.choice);
	Hx_Print("  have_text_colour = %d %d\n", t->have_text_colour, t->text_colour.choice);
	Hx_Print("  have_font_attributes = %d %.*s\n", t->have_font_attributes, t->font_attributes.size, t->font_attributes.data);
	Hx_Print("  have_highlight_ref_colour = %d %d\n", t->have_highlight_ref_colour, t->highlight_ref_colour.choice);
	Hx_Print("  initially_active = %d\n", t->initially_active);
	Hx_Print("  have_content_hook = %d %d\n", t->have_content_hook, t->content_hook);
	Hx_Print("  have_original_content = %d %d\n", t->have_original_content, t->original_content.choice);
	Hx_Print("  have_shared = %d %d\n", t->have_shared, t->shared);
	Hx_Print("  have_original_font = %d %d\n", t->have_original_font, t->original_font.choice);
	Hx_Print("  horizontal_justification = %d\n", t->horizontal_justification);
	Hx_Print("  vertical_justification = %d\n", t->vertical_justification);
	Hx_Print("  line_orientation = %d\n", t->line_orientation);
	Hx_Print("  start_corner = %d\n", t->start_corner);
	Hx_Print("  text_wrapping = %d\n", t->text_wrapping);
	Hx_Print("  engine_resp = %d\n", t->engine_resp);
	Hx_Print("HyperTextClassInstanceVars :\n");
	Hx_Print("  BoxSize = (%d, %d), OriginalBoxSize = (%d, %d)\n", t->inst.BoxSize.x_length, t->inst.BoxSize.y_length, ins_box.x_length, ins_box.y_length);
	Hx_Print("  Position = (%d, %d), XYPosition = (%d, %d)\n", t->inst.Position.x_position, t->inst.Position.y_position, ins_pos.x_position, ins_pos.y_position);
	Hx_Print("  have_PaletteRef = %d %d\n", t->inst.have_PaletteRef, t->inst.PaletteRef.choice);
	Hx_Print("  HighlightStatus = %d\n", t->inst.HighlightStatus);
	Hx_Print("  InteractionStatus = %d\n", t->inst.InteractionStatus);
	Hx_Print("  TextData = %.*s\n", t->inst.TextData.size, t->inst.TextData.data);
	Hx_Print("  Font = %s %d\n", t->inst.Font.name, t->inst.Font.size);
	Hx_Print("  TextColour = (%d,%d,%d,%d)\n", t->inst.TextColour.r, t->inst.TextColour.g, t->inst.TextColour.b, t->inst.TextColour.t);
	Hx_Print("  BackgroundColour = (%d,%d,%d,%d)\n", t->inst.BackgroundColour.r, t->inst.BackgroundColour.g, t->inst.BackgroundColour.b, t->inst.BackgroundColour.t);
	Hx_Print("  HighlightRefColour = (%d,%d,%d,%d)\n", t->inst.HighlightRefColour.r, t->inst.HighlightRefColour.g, t->inst.HighlightRefColour.b, t->inst.HighlightRefColour.t);
	Hx_Print("  element = %p\n", t->inst.element);
	Hx_Print("  LastAnchorFired = %.*s\n", t->inst.LastAnchorFired.size, t->inst.LastAnchorFired.data);
	Hx_Print("  FocusPosition = %d\n", t->inst.FocusPosition);
	Hx_Print("--------------------------------\n");
}

#if 1 // hmkim : added.
{
	/* UK MHEG Profile (D-BOOK 5.0) : 15.8.2.3 Highlight (HyperText) - The HighlightStatus attribute shall not affect the visible appearance of HyperText objects. */

	// todo ...
}
#endif

	MHEGDisplay_unsetClipRectangle(d);

	return;
}

