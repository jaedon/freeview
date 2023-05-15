/*
 * TextClass.c
 */

#include "MHEGEngine.h"
#include "Content.h"
#include "Reference.h"
#include "RootClass.h"
#include "ApplicationClass.h"
#include "VariableClass.h"
#include "Generic.h"
#include "clone.h"

static void
default_TextClassInstanceVars(TextClass *t, TextClassInstanceVars *v)
{
	VK_memset(v, 0x00, sizeof(TextClassInstanceVars));

/* TODO */
/* this code is duplicated in TextClass, EntryFieldClass and HyperTextClass */

	/* VisibleClass */
	VK_memcpy(&v->BoxSize, &t->original_box_size, sizeof(OriginalBoxSize));
	VK_memcpy(&v->Position, &t->original_position, sizeof(XYPosition));
	v->have_PaletteRef = t->have_original_palette_ref;
	if(v->have_PaletteRef)
		ObjectReference_dup(&v->PaletteRef, &t->original_palette_ref);

	/* TextClass */
	if(t->have_original_content)
	{
#if (CLS_VERBOSE)
		verbose_class("TextClass: %s; ContentBody_getContent", ExternalReference_name(&t->rootClass.inst.ref));
#endif
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
#if (CLS_VERBOSE)
			verbose_class("TextClass: %s; MHEGFont_defaultAttributes (%s)", ExternalReference_name(&t->rootClass.inst.ref), v->Font.name);
#endif
			MHEGFont_defaultAttributes(&v->Font);
		}
	}

	v->element = NULL;

	return;
}

static void
free_TextClassInstanceVars(TextClassInstanceVars *v)
{
	if(v->have_PaletteRef)
		free_ObjectReference(&v->PaletteRef);

	free_OctetString(&v->TextData);

	free_MHEGFont(&v->Font);

	LIST_FREE(&v->element, MHEGTextElement, safe_free);

	return;
}

void
TextClass_Preparation(TextClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; Preparation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been prepared */
	if(t->rootClass.inst.AvailabilityStatus)
		return;

#if 1 // hmkim : added to get rid of any existing content. ContentBody_getContent(), NewContent_getContent() 가 사용되는 경우 필요함. 예: SetData -> Activation -> Preparation -> ContentBody_getContent.
	free_OctetString(&t->inst.TextData);
#endif

	/* retrieve the object from an entity outside the engine and set each internal attribute of the object to its initial value */
	default_TextClassInstanceVars(t, &t->inst);

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&t->rootClass);

	/* add it to the DisplayStack of the active application */
	MHEGEngine_addVisibleObject(&t->rootClass);

	return;
}

bool
TextClass_Activation(TextClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; Activation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been activated */
	if(t->rootClass.inst.RunningStatus)
		return false;

	/* has it been prepared yet */
	if(!t->rootClass.inst.AvailabilityStatus)
	{
		/* generates an IsAvailable event */
		TextClass_Preparation(t);
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
TextClass_Deactivation(TextClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; Deactivation", ExternalReference_name(&t->rootClass.inst.ref));
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
TextClass_Destruction(TextClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; Destruction", ExternalReference_name(&t->rootClass.inst.ref));
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
		TextClass_Deactivation(t);
	}

	/*
	 * spec says we should handle caching here
	 * rb-download caches everything
	 */

	free_TextClassInstanceVars(&t->inst);

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
TextClass_SetData(TextClass *t, SetData *set, OctetString *caller_gid)
{
#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; SetData", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	#if 0 // hmkim : moved into NewContent_getContent() and below.
	/* get rid of any existing content */
	free_OctetString(&t->inst.TextData);
	LIST_FREE(&t->inst.element, MHEGTextElement, safe_free);
	#endif

	/*
	 * the content may need to be loaded from an external file
	 * if the file is not available, this returns false and calls TextClass_contentAvailable() when it appears
	 */
	if(NewContent_getContent(&set->new_content, caller_gid, &t->rootClass, &t->inst.TextData))
	{
		LIST_FREE(&t->inst.element, MHEGTextElement, safe_free); // hmkim : added to get rid of any existing content.

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
TextClass_Clone(TextClass *t, Clone *params, OctetString *caller_gid)
{
#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	CLONE_OBJECT(t, TextClass, text);

	return;
}

void
TextClass_SetPosition(TextClass *t, SetPosition *params, OctetString *caller_gid)
{
//	XYPosition old;

//	old.x_position = t->inst.Position.x_position;
//	old.y_position = t->inst.Position.y_position;

	t->inst.Position.x_position = GenericInteger_getInteger(&params->new_x_position, caller_gid);
	t->inst.Position.y_position = GenericInteger_getInteger(&params->new_y_position, caller_gid);

#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; SetPosition; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.Position.x_position, t->inst.Position.y_position);
#endif

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
TextClass_GetPosition(TextClass *t, GetPosition *params, OctetString *caller_gid)
{
	VariableClass *var;

#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; GetPosition", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* X position */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("TextClass: GetPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Position.x_position);

	/* Y position */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("TextClass: GetPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Position.y_position);

	return;
}

void
TextClass_SetBoxSize(TextClass *t, SetBoxSize *params, OctetString *caller_gid)
{
//	OriginalBoxSize old;

//	old.x_length = t->inst.BoxSize.x_length;
//	old.y_length = t->inst.BoxSize.y_length;

	t->inst.BoxSize.x_length = GenericInteger_getInteger(&params->x_new_box_size, caller_gid);
	t->inst.BoxSize.y_length = GenericInteger_getInteger(&params->y_new_box_size, caller_gid);

#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; SetBoxSize; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.BoxSize.x_length, t->inst.BoxSize.y_length);
#endif

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

	return;
}

void
TextClass_GetBoxSize(TextClass *t, GetBoxSize *params, OctetString *caller_gid)
{
	VariableClass *var;

#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; GetBoxSize", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* width */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_box_size_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("TextClass: GetBoxSize: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BoxSize.x_length);

	/* height */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_box_size_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("TextClass: GetBoxSize: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BoxSize.y_length);

	return;
}

void
TextClass_BringToFront(TextClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; BringToFront", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	MHEGEngine_bringToFront(&t->rootClass);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
TextClass_SendToBack(TextClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; SendToBack", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	MHEGEngine_sendToBack(&t->rootClass);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
TextClass_PutBefore(TextClass *t, PutBefore *params, OctetString *caller_gid)
{
	ObjectReference *ref;
	RootClass *obj;

#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; PutBefore", ExternalReference_name(&t->rootClass.inst.ref));
#endif

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
TextClass_PutBehind(TextClass *t, PutBehind *params, OctetString *caller_gid)
{
	ObjectReference *ref;
	RootClass *obj;

#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; PutBehind", ExternalReference_name(&t->rootClass.inst.ref));
#endif

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
TextClass_SetPaletteRef(TextClass *t, SetPaletteRef *params, OctetString *caller_gid)
{
	/* UK MHEG Profile says we don't support PaletteClass */
	error("TextClass: %s; SetPaletteRef not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
TextClass_SetBackgroundColour(TextClass *t, SetBackgroundColour *params, OctetString *caller_gid)
{
#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; SetBackgroundColour", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	MHEGColour_fromNewColour(&t->inst.BackgroundColour, &params->new_background_colour, caller_gid);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
TextClass_SetTextColour(TextClass *t, SetTextColour *params, OctetString *caller_gid)
{
#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; SetTextColour", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	MHEGColour_fromNewColour(&t->inst.TextColour, &params->new_text_colour, caller_gid);

	/* remove the previous layout info, colours get recalculated when we redraw it */
	LIST_FREE(&t->inst.element, MHEGTextElement, safe_free);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
TextClass_SetFontAttributes(TextClass *t, SetFontAttributes *params, OctetString *caller_gid)
{
	OctetString *attr;

#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; SetFontAttributes", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	attr = GenericOctetString_getOctetString(&params->new_font_attribute, caller_gid);

	MHEGFont_setAttributes(&t->inst.Font, attr);

	/* remove the previous layout info, gets recalculated when we redraw it */
	LIST_FREE(&t->inst.element, MHEGTextElement, safe_free);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
TextClass_GetTextContent(TextClass *t, GetTextContent *params, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__);

#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; GetTextContent", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	return;
}

/*
 * caller_gid is used to resolve the text_data_var ObjectReference
 */

void
TextClass_GetTextData(TextClass *t, GetTextData *get, OctetString *caller_gid)
{
	VariableClass *var;

#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; GetTextData", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&get->text_data_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_octetstring)
	{
		error("TextClass: GetTextData: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	OctetStringVariableClass_setOctetString(var, &t->inst.TextData);

	return;
}

void
TextClass_SetFontRef(TextClass *t, SetFontRef *params, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__); /* 특히 CI+ MHEG Profile 에서 필요 */

#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; SetFontRef", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* Set the value of the Font attribute to Newfont */

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

/*
 * called when the content is available
 */

void
TextClass_contentAvailable(TextClass *t, OctetString *file)
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
TextClass_render(TextClass *t, MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	XYPosition ins_pos;
	OriginalBoxSize ins_box;
	LIST_TYPE(MHEGTextElement) *element;
	bool tabs;

#if (CLS_VERBOSE)
	verbose_class("TextClass: %s; render; %d; %d; %.*s", ExternalReference_name(&t->rootClass.inst.ref), t->inst.Font.size, t->inst.TextData.size, t->inst.TextData.size, t->inst.TextData.data);
#endif

	if(!intersects(pos, box, &t->inst.Position, &t->inst.BoxSize, &ins_pos, &ins_box))
		return;

	MHEGDisplay_setClipRectangle(d, &ins_pos, &ins_box);

	/* draw the background */
	MHEGDisplay_fillRectangle(d, &ins_pos, &ins_box, &t->inst.BackgroundColour);

	/* layout the text if not already done */
	if(t->inst.element == NULL)
	{
		t->inst.element = MHEGFont_layoutText(&t->inst.Font, &t->inst.TextColour, &t->inst.TextData, &t->inst.BoxSize,
						      t->horizontal_justification, t->vertical_justification,
						      t->line_orientation, t->start_corner, t->text_wrapping);
	}

	/* tabs are treated as spaces if horizontal justification is not Justification_start */
	tabs = (t->horizontal_justification == Justification_start);

	/* draw each text element */
	element = t->inst.element;
	while(element)
	{
		//Hx_Print("t->horizontal_justification = %d, t->vertical_justification = %d\n", t->horizontal_justification, t->vertical_justification);
		MHEGDisplay_drawTextElement(d, &t->inst.Position, &t->inst.Font, &element->item, tabs, t->horizontal_justification, t->vertical_justification);
		element = element->next;
	}

	MHEGDisplay_unsetClipRectangle(d);

	return;
}

