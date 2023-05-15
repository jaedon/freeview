/*
 * SliderClass.c
 */

#include "MHEGEngine.h"
#include "SliderClass.h"
#include "RootClass.h"
#include "ApplicationClass.h"
#include "Reference.h"
#include "Generic.h"
#include "VariableClass.h"
#include "clone.h"

static void
default_SliderClassInstanceVars(SliderClass *t, SliderClassInstanceVars *v)
{
	VK_memset(v, 0x00, sizeof(SliderClassInstanceVars));

	/* VisibleClass */
	VK_memcpy(&v->BoxSize, &t->original_box_size, sizeof(OriginalBoxSize));
	VK_memcpy(&v->Position, &t->original_position, sizeof(XYPosition));
	v->have_PaletteRef = t->have_original_palette_ref;
	if(v->have_PaletteRef)
		ObjectReference_dup(&v->PaletteRef, &t->original_palette_ref);

	/* InteractibleClass */
	v->HighlightStatus = false;
	v->InteractionStatus = false;

#if 1 // hmkim : added.

	/* defaults come from the ApplicationClass */
	if(t->have_slider_ref_colour)
	{
		MHEGColour_fromColour(&v->SliderRefColour, &t->slider_ref_colour);
	}
	else
	{
		DefaultAttribute *attr = ApplicationClass_getDefaultAttribute(MHEGEngine_getActiveApplication(), DefaultAttribute_slider_ref_colour);
		Colour *col = (attr != NULL) ? &attr->u.slider_ref_colour : NULL;
		/* default is transparent */
		if(col != NULL)
			MHEGColour_fromColour(&v->SliderRefColour, col);
		else
			MHEGColour_transparent(&v->SliderRefColour);
	}

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

	/* SliderClass */
	if(t->have_initial_value)
		v->SliderValue = t->initial_value;
	else
		v->SliderValue = 0;

	if(t->have_initial_portion) // && t->slider_style == SliderStyle_proportional ?
		v->Portion = t->initial_portion;
	else
		v->Portion = 0;

	v->MinValue = t->min_value;
	v->MaxValue = t->max_value;
	v->StepSize = t->step_size;

	return;
}

static void
free_SliderClassInstanceVars(SliderClassInstanceVars *v)
{
	if(v->have_PaletteRef)
		free_ObjectReference(&v->PaletteRef);

	return;
}

void
SliderClass_Preparation(SliderClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("SliderClass: %s; Preparation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been prepared */
	if(t->rootClass.inst.AvailabilityStatus)
		return;

	/* retrieve the object from an entity outside the engine and set each internal attribute of the object to its initial value */
	default_SliderClassInstanceVars(t, &t->inst);

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&t->rootClass);

	/* add it to the DisplayStack of the active application */
	MHEGEngine_addVisibleObject(&t->rootClass);

	return;
}

bool
SliderClass_Activation(SliderClass *t)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if (CLS_VERBOSE)
	verbose_class("SliderClass: %s; Activation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

#if 1 // hmkim : added.

	/* has it already been activated */
	if(t->rootClass.inst.RunningStatus)
		return false;

	/* has it been prepared yet */
	if(!t->rootClass.inst.AvailabilityStatus)
	{
		/* generates an IsAvailable event */
		SliderClass_Preparation(t);
	}

	/* Activation inherited from the RootClass */
	RootClass_Activation(&t->rootClass);

	/* now its RunningStatus is true, get it drawn at its position in the application's DisplayStack */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

#endif

	return true;
}

bool
SliderClass_Deactivation(SliderClass *t)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if (CLS_VERBOSE)
	verbose_class("SliderClass: %s; Deactivation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

#if 1 // hmkim : added.

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

#endif

	return true;
}

void
SliderClass_Destruction(SliderClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("SliderClass: %s; Destruction", ExternalReference_name(&t->rootClass.inst.ref));
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
		SliderClass_Deactivation(t);
	}

	/*
	 * spec says we should handle caching here
	 * rb-download caches everything
	 */

	free_SliderClassInstanceVars(&t->inst);

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
SliderClass_Clone(SliderClass *t, Clone *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

	verbose_class("SliderClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));

	CLONE_OBJECT(t, SliderClass, slider);

	return;
}

void
SliderClass_SetPosition(SliderClass *t, SetPosition *params, OctetString *caller_gid)
{
//	XYPosition old;

//	old.x_position = t->inst.Position.x_position;
//	old.y_position = t->inst.Position.y_position;

	t->inst.Position.x_position = GenericInteger_getInteger(&params->new_x_position, caller_gid);
	t->inst.Position.y_position = GenericInteger_getInteger(&params->new_y_position, caller_gid);

	verbose_class("SliderClass: %s; SetPosition; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.Position.x_position, t->inst.Position.y_position);

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
SliderClass_GetPosition(SliderClass *t, GetPosition *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("SliderClass: %s; GetPosition", ExternalReference_name(&t->rootClass.inst.ref));

	/* X position */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("SliderClass: GetPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Position.x_position);

	/* Y position */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("SliderClass: GetPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Position.y_position);
}
#endif

	return;
}

void
SliderClass_SetBoxSize(SliderClass *t, SetBoxSize *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
//	OriginalBoxSize old;

//	old.x_length = t->inst.BoxSize.x_length;
//	old.y_length = t->inst.BoxSize.y_length;

	t->inst.BoxSize.x_length = GenericInteger_getInteger(&params->x_new_box_size, caller_gid);
	t->inst.BoxSize.y_length = GenericInteger_getInteger(&params->y_new_box_size, caller_gid);

	verbose_class("SliderClass: %s; SetBoxSize; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.BoxSize.x_length, t->inst.BoxSize.y_length);

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
SliderClass_GetBoxSize(SliderClass *t, GetBoxSize *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("SliderClass: %s; GetBoxSize", ExternalReference_name(&t->rootClass.inst.ref));

	/* width */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_box_size_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("SliderClass: GetBoxSize: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BoxSize.x_length);

	/* height */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_box_size_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("SliderClass: GetBoxSize: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BoxSize.y_length);
}
#endif

	return;
}

void
SliderClass_BringToFront(SliderClass *t)
{
	verbose_class("SliderClass: %s; BringToFront", ExternalReference_name(&t->rootClass.inst.ref));

	MHEGEngine_bringToFront(&t->rootClass);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
SliderClass_SendToBack(SliderClass *t)
{
	verbose_class("SliderClass: %s; SendToBack", ExternalReference_name(&t->rootClass.inst.ref));

	MHEGEngine_sendToBack(&t->rootClass);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
SliderClass_PutBefore(SliderClass *t, PutBefore *params, OctetString *caller_gid)
{
	ObjectReference *ref;
	RootClass *obj;

	verbose_class("SliderClass: %s; PutBefore", ExternalReference_name(&t->rootClass.inst.ref));

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
SliderClass_PutBehind(SliderClass *t, PutBehind *params, OctetString *caller_gid)
{
	ObjectReference *ref;
	RootClass *obj;

	verbose_class("SliderClass: %s; PutBehind", ExternalReference_name(&t->rootClass.inst.ref));

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
SliderClass_SetPaletteRef(SliderClass *t, SetPaletteRef *params, OctetString *caller_gid)
{
	/* UK MHEG Profile says we don't support PaletteClass */
	error("SliderClass: %s; SetPaletteRef not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
SliderClass_SetSliderParameters(SliderClass *t, SetSliderParameters *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int NewMinValue = GenericInteger_getInteger(&params->new_min_value, caller_gid);
	int NewMaxValue = GenericInteger_getInteger(&params->new_max_value, caller_gid);
	int NewStepSize = GenericInteger_getInteger(&params->new_step_size, caller_gid);
	int Portion = t->slider_style == SliderStyle_proportional ? t->inst.Portion : 0;
	EventData event_tag;

	verbose_class("SliderClass: %s; SetSliderParameters; (%d, %d, %d); SliderStyle %d; Portion %d",
		ExternalReference_name(&t->rootClass.inst.ref), NewMinValue, NewMaxValue, NewStepSize, t->slider_style, t->inst.Portion);

	if (NewMinValue >= NewMaxValue)
	{
		error("SliderClass: %s; SetSliderParameters: NewMinValue %d shall be < NewMaxValue %d", ExternalReference_name(&t->rootClass.inst.ref), NewMinValue, NewMaxValue);
		return;
	}
	if (NewStepSize == 0 || (NewMaxValue - NewMinValue) % NewStepSize != 0)
	{
		error("SliderClass: %s; SetSliderParameters: N x NewStepSize %d shall be (NewMaxValue %d - NewMinValue %d)", ExternalReference_name(&t->rootClass.inst.ref), NewStepSize, NewMaxValue, NewMinValue);
		return;
	}
//	if (NewMaxValue - NewMinValue < t->inst.Portion) <- UK MHEG Profile (D-BOOK 5.0) 에 분명히 명시되어 있지는 않으나 다른 경우에 비춰보면 t->inst.Portion 보다는 Portion 이 맞을 것 같다.
	if (NewMaxValue - NewMinValue < Portion)
	{
		error("SliderClass: %s; SetSliderParameters: (NewMaxValue %d - NewMinValue %d) shall be >= Portion %d", ExternalReference_name(&t->rootClass.inst.ref), NewMaxValue, NewMinValue, Portion);
		return;
	}

	t->inst.MinValue = NewMinValue;
	t->inst.MaxValue = NewMaxValue;
	t->inst.StepSize = NewStepSize;

	t->inst.SliderValue = NewMinValue;

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	event_tag.choice = EventData_integer;
	event_tag.u.integer = t->inst.SliderValue;
	MHEGEngine_generateAsyncEvent(&t->rootClass.inst.ref, EventType_slider_value_changed, &event_tag);
}
#endif

	return;
}

void
SliderClass_SetInteractionStatus(SliderClass *t, SetInteractionStatus *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	bool OldInteractionStatus = t->inst.InteractionStatus;
	bool NewInteractionStatus = GenericBoolean_getBoolean(&params->new_interaction_status, caller_gid);
	SceneClass *scene = MHEGEngine_getActiveScene();

	verbose_class("SliderClass: %s; SetInteractionStatus; %s", ExternalReference_name(&t->rootClass.inst.ref), NewInteractionStatus ? "true" : "false");

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
				verbose_class("SliderClass: %s; SetInteractionStatus; EntryField %s has true InteractionStatus -> discard the action",
					ExternalReference_name(&t->rootClass.inst.ref), ExternalReference_name(&gi->item.u.entry_field.rootClass.inst.ref));
				return;
			}
			else if (gi->item.choice == GroupItem_hyper_text
				&& gi->item.u.hyper_text.rootClass.inst.AvailabilityStatus
				&& gi->item.u.hyper_text.rootClass.inst.RunningStatus
				&& gi->item.u.hyper_text.inst.InteractionStatus)
			{
				verbose_class("SliderClass: %s; SetInteractionStatus; HyperText %s has true InteractionStatus -> discard the action",
					ExternalReference_name(&t->rootClass.inst.ref), ExternalReference_name(&gi->item.u.hyper_text.rootClass.inst.ref));
				return;
			}
			else if (gi->item.choice == GroupItem_slider
				&& gi->item.u.slider.rootClass.inst.AvailabilityStatus
				&& gi->item.u.slider.rootClass.inst.RunningStatus
				&& gi->item.u.slider.inst.InteractionStatus)
			{
				verbose_class("SliderClass: %s; SetInteractionStatus; Slider %s has true InteractionStatus -> discard the action",
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
SliderClass_GetInteractionStatus(SliderClass *t, GetInteractionStatus *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("SliderClass: %s; GetInteractionStatus", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->interaction_status_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_boolean)
	{
		error("SliderClass: GetInteractionStatus: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	BooleanVariableClass_setBoolean(var, t->inst.InteractionStatus);
}
#endif

	return;
}

void
SliderClass_SetHighlightStatus(SliderClass *t, SetHighlightStatus *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	bool OldHighlightStatus = t->inst.HighlightStatus;
	bool NewHighlightStatus = GenericBoolean_getBoolean(&params->new_highlight_status, caller_gid);

	verbose_class("SliderClass: %s; SetHighlightStatus; %s", ExternalReference_name(&t->rootClass.inst.ref), NewHighlightStatus ? "true" : "false");

	if (OldHighlightStatus != NewHighlightStatus)
	{
		t->inst.HighlightStatus = NewHighlightStatus;

		/* if it is active, redraw it */
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
SliderClass_GetHighlightStatus(SliderClass *t, GetHighlightStatus *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("SliderClass: %s; GetHighlightStatus", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->highlight_status_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_boolean)
	{
		error("SliderClass: GetHighlightStatus: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	BooleanVariableClass_setBoolean(var, t->inst.HighlightStatus);
}
#endif

	return;
}

void
SliderClass_Step(SliderClass *t, Step *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int NbOfSteps = GenericInteger_getInteger(&params->nb_of_steps, caller_gid);
	int Portion = t->slider_style == SliderStyle_proportional ? t->inst.Portion : 0;
	EventData event_tag;

	verbose_class("SliderClass: %s; Step; NbOfSteps %d; SliderStyle %d; Portion %d", ExternalReference_name(&t->rootClass.inst.ref), NbOfSteps, t->slider_style, t->inst.Portion);

	/* ISO/IEC 13522-5 :
	   NbOfSteps shall be set so that:
	   MinValue <= (NbOfSteps X StepSize) + SliderValue, and
	   (NbOfSteps X StepSize) + SliderValue + Portion <= MaxValue.
	   The value of Portion in the expression above shall be taken to be 0 if the SliderStyle is not proportional. */
	if (t->inst.MinValue > (NbOfSteps * t->inst.StepSize) + t->inst.SliderValue)
	{
		error("SliderClass: %s; Step: NbOfSteps shall be, MinValue %d <= (NbOfSteps %d X StepSize %d) + SliderValue %d",
			ExternalReference_name(&t->rootClass.inst.ref), t->inst.MinValue, NbOfSteps, t->inst.StepSize, t->inst.SliderValue);
		return;
	}
	if ((NbOfSteps * t->inst.StepSize) + t->inst.SliderValue + Portion > t->inst.MaxValue)
	{
		error("SliderClass: %s; Step: NbOfSteps shall be, (NbOfSteps %d X StepSize %d) + SliderValue %d + Portion %d <= MaxValue %d",
			ExternalReference_name(&t->rootClass.inst.ref), NbOfSteps, t->inst.StepSize, t->inst.SliderValue, Portion, t->inst.MaxValue);
		return;
	}

	t->inst.SliderValue += (NbOfSteps * t->inst.StepSize);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	/* ISO/IEC 13522-5 에 따르면 InteractionCompleted event 를 generate 하도록 되어 있으나 UK MHEG Profile (D-BOOK 5.0) 에서는 SliderValueChanged event 를 generate 하도록 되어 있다. */
	event_tag.choice = EventData_integer;
	event_tag.u.integer = t->inst.SliderValue;
	MHEGEngine_generateAsyncEvent(&t->rootClass.inst.ref, EventType_slider_value_changed, &event_tag);
}
#endif

	return;
}

void
SliderClass_SetSliderValue(SliderClass *t, SetSliderValue *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int NewSliderValue = GenericInteger_getInteger(&params->new_slider_value, caller_gid);
	int Portion = t->slider_style == SliderStyle_proportional ? t->inst.Portion : 0;
	EventData event_tag;

	verbose_class("SliderClass: %s; SetSliderValue; NewVal %d; SliderStyle %d; Portion %d", ExternalReference_name(&t->rootClass.inst.ref), NewSliderValue, t->slider_style, t->inst.Portion);

	/* ISO/IEC 13522-5 :
	   NewSliderValue shall be within the range [MinValue, MaxValue - Portion].
	   The value of Portion in the expression above shall be taken to be 0 if the SliderStyle is not proportional.
	   (NewSliderValue - MinValue) MOD StepSize shall be equal to 0. */
	if (NewSliderValue < t->inst.MinValue || NewSliderValue > t->inst.MaxValue - Portion)
	{
		error("SliderClass: %s; SetSliderValue: NewSliderValue %d shall be within [MinValue %d, MaxValue %d - Portion %d]",
			ExternalReference_name(&t->rootClass.inst.ref), NewSliderValue, t->inst.MinValue, t->inst.MaxValue, Portion);
		return;
	}
	if (t->inst.StepSize == 0 || (NewSliderValue - t->inst.MinValue) % t->inst.StepSize != 0)
	{
		error("SliderClass: %s; SetSliderValue: (NewSliderValue %d - MinValue %d) MOD StepSize %d shall be equal to 0",
			ExternalReference_name(&t->rootClass.inst.ref), NewSliderValue, t->inst.MinValue, t->inst.StepSize);
		return;
	}

	if (t->inst.SliderValue != NewSliderValue)
	{
		t->inst.SliderValue = NewSliderValue;

		/* if it is active, redraw it */
		/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
		if(t->rootClass.inst.RunningStatus)
			MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
		*/

		/* ISO/IEC 13522-5 에 따르면 InteractionCompleted event 를 generate 하도록 되어 있으나 UK MHEG Profile (D-BOOK 5.0) 에서는 SliderValueChanged event 를 generate 하도록 되어 있다. */
		event_tag.choice = EventData_integer;
		event_tag.u.integer = t->inst.SliderValue;
		MHEGEngine_generateAsyncEvent(&t->rootClass.inst.ref, EventType_slider_value_changed, &event_tag);
	}
}
#endif

	return;
}

void
SliderClass_GetSliderValue(SliderClass *t, GetSliderValue *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("SliderClass: %s; GetSliderValue", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->slider_value_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("SliderClass: GetSliderValue: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.SliderValue);
}
#endif

	return;
}

void
SliderClass_SetPortion(SliderClass *t, SetPortion *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	int NewPortion = GenericInteger_getInteger(&params->new_portion, caller_gid);
	EventData event_tag;

	verbose_class("SliderClass: %s; SetPortion; %d", ExternalReference_name(&t->rootClass.inst.ref), NewPortion);

	/* ISO/IEC 13522-5 : The Target object shall be an available Slider object of style proportional and NewPortion shall be smaller than or equal to (MaxValue - SliderValue) */
	if (t->slider_style != SliderStyle_proportional)
	{
		error("SliderClass: %s; SetPortion: not proportional (%d)",
			ExternalReference_name(&t->rootClass.inst.ref), t->slider_style);
		return;
	}
	if (NewPortion > t->inst.MaxValue - t->inst.SliderValue)
	{
		error("SliderClass: %s; SetPortion: NewPortion %d shall be <= (MaxValue %d - SliderValue %d)",
			ExternalReference_name(&t->rootClass.inst.ref), NewPortion, t->inst.MaxValue, t->inst.SliderValue);
		return;
	}

	if (t->inst.Portion != NewPortion)
	{
		t->inst.Portion = NewPortion;

		/* if it is active, redraw it */
		/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
		if(t->rootClass.inst.RunningStatus)
			MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
		*/

		/* ISO/IEC 13522-5 에 따르면 InteractionCompleted event 를 generate 하도록 되어 있으나 UK MHEG Profile (D-BOOK 5.0) 에서는 SliderValueChanged event 를 generate 하도록 되어 있다. */
		event_tag.choice = EventData_integer;
		event_tag.u.integer = t->inst.SliderValue;
		MHEGEngine_generateAsyncEvent(&t->rootClass.inst.ref, EventType_slider_value_changed, &event_tag);
	}
}
#endif

	return;
}

void
SliderClass_GetPortion(SliderClass *t, GetPortion *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("SliderClass: %s; GetPortion", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->portion_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("SliderClass: GetPortion: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Portion);
}
#endif

	return;
}

void
SliderClass_render(SliderClass *t, MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	XYPosition ins_pos;
	OriginalBoxSize ins_box;

	verbose_class("SliderClass: %s; render", ExternalReference_name(&t->rootClass.inst.ref));

	if(!intersects(pos, box, &t->inst.Position, &t->inst.BoxSize, &ins_pos, &ins_box))
		return;

	MHEGDisplay_setClipRectangle(d, &ins_pos, &ins_box);

//	verbose_todo("%s not yet implemented", __FUNCTION__);

if (g_MhegIbDbgFlag & MHEG_IB_DBG_MASK_CLS)
{
	Hx_Print("---< SliderClass_render >---\n");
	Hx_Print("SliderClass :\n"); // SliderClassInstanceVars 에 셋팅되지 않는 것들만 프린트한다.
	Hx_Print("  have_highlight_ref_colour = %d %d\n", t->have_highlight_ref_colour, t->highlight_ref_colour.choice);
	Hx_Print("  have_slider_ref_colour = %d %d\n", t->have_slider_ref_colour, t->slider_ref_colour.choice);
	Hx_Print("  initially_active = %d\n", t->initially_active);
	Hx_Print("  have_content_hook = %d %d\n", t->have_content_hook, t->content_hook);
	Hx_Print("  have_original_content = %d %d\n", t->have_original_content, t->original_content.choice);
	Hx_Print("  have_shared = %d %d\n", t->have_shared, t->shared);
	Hx_Print("  engine_resp = %d\n", t->engine_resp);
	Hx_Print("  orientation = %d\n", t->orientation);
	Hx_Print("  have_initial_value = %d %d\n", t->have_initial_value, t->initial_value);
	Hx_Print("	have_initial_portion = %d %d\n", t->have_initial_portion, t->initial_portion);
	Hx_Print("	slider_style = %d\n", t->slider_style);
	Hx_Print("SliderClassInstanceVars :\n");
	Hx_Print("  BoxSize = (%d, %d), OriginalBoxSize = (%d, %d)\n", t->inst.BoxSize.x_length, t->inst.BoxSize.y_length, ins_box.x_length, ins_box.y_length);
	Hx_Print("  Position = (%d, %d), XYPosition = (%d, %d)\n", t->inst.Position.x_position, t->inst.Position.y_position, ins_pos.x_position, ins_pos.y_position);
	Hx_Print("  have_PaletteRef = %d %d\n", t->inst.have_PaletteRef, t->inst.PaletteRef.choice);
	Hx_Print("  HighlightStatus = %d\n", t->inst.HighlightStatus);
	Hx_Print("  InteractionStatus = %d\n", t->inst.InteractionStatus);
	Hx_Print("  HighlightRefColour = (%d,%d,%d,%d)\n", t->inst.HighlightRefColour.r, t->inst.HighlightRefColour.g, t->inst.HighlightRefColour.b, t->inst.HighlightRefColour.t);
	Hx_Print("  SliderRefColour = (%d,%d,%d,%d)\n", t->inst.SliderRefColour.r, t->inst.SliderRefColour.g, t->inst.SliderRefColour.b, t->inst.SliderRefColour.t);
	Hx_Print("	SliderValue = %d\n", t->inst.SliderValue);
	Hx_Print("  Portion = %d\n", t->inst.Portion);
	Hx_Print("  MinValue = %d\n", t->inst.MinValue);
	Hx_Print("  MaxValue = %d\n", t->inst.MaxValue);
	Hx_Print("  StepSize = %d\n", t->inst.StepSize);
	Hx_Print("--------------------------------\n");
}

#if 1 // hmkim : added.
{
	MHEGColour thumb_col;
	XYPosition thumb_pos;
	OriginalBoxSize thumb_box;

	unsigned int GuideWidth, GuideLen, ThumbWidth, ThumbLen;
	int GuideWidthOrgPos, GuideLenOrgPos, ThumbWidthOrgPos, ThumbLenOrgPos;

	int NbOfSteps;
	int StepIndex;
	int StepLen;

	/* 총 Step 의 갯수 = (t->inst.MaxValue - t->inst.MinValue) / t->inst.StepSize
	   현 Step 의 인덱스 (0 ~ 총 Step 의 갯수) = (t->inst.SliderValue - t->inst.MinValue) / t->inst.StepSize
	   한 Step 의 길이 (pixel) = Slider 의 길이 / 총 Step 의 갯수
	   현 Step 의 시작 좌표 = Slider 의 시작 좌표 + 한 Step 의 길이 * 현 Step 의 인덱스

	   TODO : 스펙에 구체적으로 명시되어 있지 않아 추후 확인 필요한 사항들...

	   1. Slider 의 Position 및 BoxSize 와 MinValue, MaxValue 와의 매칭 : MinValue, MaxValue 값이 각기 Slider 의 주축의 시작점과 끝점에 매칭된다고 보는 것이 맞을 것 같은데 분명치 않음.
	   2. Portion 의 단위 : pixel 인지 StepSize 와 같은 류인지... 일단 pixel 로 보고 처리하도록 하자. */

	/* set the colour of Thumb :
	   The Thumb shall be a rectangular area, the colour of which is defined by the SliderRefColour attribute.
	   The Slider shall be highlighted if both the HighlightStatus and EngineResp attributes are True.
	   The appearance of this highlight shall be to change the colour of the rectangular area representing the Thumb to be that defined by the HighlightRefColour. */
	if (t->inst.HighlightStatus && t->engine_resp)
	{
		thumb_col = t->inst.HighlightRefColour;
	}
	else
	{
		thumb_col = t->inst.SliderRefColour;
	}

	/* set the width / length and position of Guide and Thumb :
	   The "width" of the Thumb shall be the size of the Slider object in the dimension perpendicular to the axis defined by the Orientation attribute.
	   The "length" of the Thumb is its dimension along the axis defined by the Orientation attribute.
	   The Thumb's position and length will vary depending upon the SliderStyle attribute as follows:
	   - If the SliderStyle is set to normal, the Thumb is rendered as a 9 pixel long ≪marker≫
	     which is centred on the position on the ≪main axis≫ corresponding to the SliderValue attribute.
	     At the extremes of SliderValue the Thumb shall remain within the bounding box of the Slider and still be rendered completely.
	     Positions for intermediate values of SliderValue shall be evenly spaced within these limits (to within one pixel).
	   - If the SliderStyle is set to thermometer, the Slider is rendered as a <<main axis,>>
	     which is filled from its beginning to the position corresponding to the SliderValue attribute.
	   - If the SliderStyle is set to proportional, the Slider is rendered as a <<main axis,>>
	     which is filled from the position corresponding to the SliderValue attribute
	     to the position corresponding to the sum of the SliderValue and the Portion internal attributes.

       이하 코드와 같이...

	   1. Orientation 고려하여 Guide 의 좌표와 width / length 계산하고
	   2. Orientation 및 SliderStyle 고려하여 Thumb 의 좌표와 width / length 계산하도록 하였는데

	   단순히 Orientation 이 right 인 경우에 대해서만 계산한 후 다른 Orientation 에 대해서는 행렬 대칭 변환식을 이용할 수도 있겠다.
	   하지만 변환에 따른 연산 오버헤드가 있을 것이므로 여기서는 이대로 각 케이스별로 분리해서 처리하도록 하자. */

	/* set the width / length and position of Guide */
	if (t->orientation == Orientation_left || t->orientation == Orientation_right)
	{
		GuideWidth       = t->inst.BoxSize.y_length;
		GuideLen         = t->inst.BoxSize.x_length;
		GuideWidthOrgPos = t->inst.Position.y_position;
		GuideLenOrgPos   = t->inst.Position.x_position;
	}
	else // if (t->orientation == Orientation_up || t->orientation == Orientation_down)
	{
		GuideWidth       = t->inst.BoxSize.x_length;
		GuideLen         = t->inst.BoxSize.y_length;
		GuideWidthOrgPos = t->inst.Position.x_position;
		GuideLenOrgPos   = t->inst.Position.y_position;
	}

	/* set the width and its origin of Thumb */
	ThumbWidth = GuideWidth;
	ThumbWidthOrgPos = GuideWidthOrgPos;

	/* set the length and its origin of Thumb */
	if (t->orientation == Orientation_left || t->orientation == Orientation_up)
	{
		if (t->slider_style == SliderStyle_normal)
		{
			ThumbLen = 9;
			if (t->inst.SliderValue == t->inst.MinValue)
			{
				ThumbLenOrgPos = GuideLenOrgPos + GuideLen - ThumbLen;
			}
			else if (t->inst.SliderValue == t->inst.MaxValue)
			{
				ThumbLenOrgPos = GuideLenOrgPos;
			}
			else
			{
				NbOfSteps = (t->inst.MaxValue - t->inst.MinValue) / t->inst.StepSize;
				StepIndex = (t->inst.SliderValue - t->inst.MinValue) / t->inst.StepSize;
				StepLen = GuideLen / NbOfSteps;
				ThumbLenOrgPos = GuideLenOrgPos + GuideLen - StepLen * StepIndex - 4; // -4 is for centering
			}
		}
		else if (t->slider_style == SliderStyle_thermometer)
		{
			if (t->inst.SliderValue == t->inst.MinValue)
			{
				ThumbLen = 0;
				ThumbLenOrgPos = GuideLenOrgPos + GuideLen;
			}
			else if (t->inst.SliderValue == t->inst.MaxValue)
			{
				ThumbLen = GuideLen;
				ThumbLenOrgPos = GuideLenOrgPos;
			}
			else
			{
				NbOfSteps = (t->inst.MaxValue - t->inst.MinValue) / t->inst.StepSize;
				StepIndex = (t->inst.SliderValue - t->inst.MinValue) / t->inst.StepSize;
				StepLen = GuideLen / NbOfSteps;
				ThumbLen = StepLen * StepIndex;
				ThumbLenOrgPos = GuideLenOrgPos + GuideLen - ThumbLen;
			}
		}
		else // if (t->slider_style == SliderStyle_proportional)
		{
			ThumbLen = t->inst.Portion;
			if (t->inst.SliderValue == t->inst.MinValue)
			{
				ThumbLenOrgPos = GuideLenOrgPos + GuideLen - ThumbLen;
			}
			else if (t->inst.SliderValue == t->inst.MaxValue)
			{
				ThumbLenOrgPos = GuideLenOrgPos;
			}
			else
			{
				NbOfSteps = (t->inst.MaxValue - t->inst.MinValue) / t->inst.StepSize;
				StepIndex = (t->inst.SliderValue - t->inst.MinValue) / t->inst.StepSize;
				StepLen = GuideLen / NbOfSteps;
				ThumbLenOrgPos = GuideLenOrgPos + GuideLen - StepLen * StepIndex - ThumbLen;
			}
		}
	}
	else // if (t->orientation == Orientation_right || t->orientation == Orientation_down)
	{
		if (t->slider_style == SliderStyle_normal)
		{
			ThumbLen = 9;
			if (t->inst.SliderValue == t->inst.MinValue)
			{
				ThumbLenOrgPos = GuideLenOrgPos;
			}
			else if (t->inst.SliderValue == t->inst.MaxValue)
			{
				ThumbLenOrgPos = GuideLenOrgPos + GuideLen - ThumbLen;
			}
			else
			{
				NbOfSteps = (t->inst.MaxValue - t->inst.MinValue) / t->inst.StepSize;
				StepIndex = (t->inst.SliderValue - t->inst.MinValue) / t->inst.StepSize;
				StepLen = GuideLen / NbOfSteps;
				ThumbLenOrgPos = GuideLenOrgPos + StepLen * StepIndex - 4; // -4 is for centering
			}
		}
		else if (t->slider_style == SliderStyle_thermometer)
		{
			ThumbLenOrgPos = GuideLenOrgPos;
			if (t->inst.SliderValue == t->inst.MinValue)
			{
				ThumbLen = 0;
			}
			else if (t->inst.SliderValue == t->inst.MaxValue)
			{
				ThumbLen = GuideLen;
			}
			else
			{
				NbOfSteps = (t->inst.MaxValue - t->inst.MinValue) / t->inst.StepSize;
				StepIndex = (t->inst.SliderValue - t->inst.MinValue) / t->inst.StepSize;
				StepLen = GuideLen / NbOfSteps;
				ThumbLen = StepLen * StepIndex;
			}
		}
		else // if (t->slider_style == SliderStyle_proportional)
		{
			ThumbLen = t->inst.Portion;
			if (t->inst.SliderValue == t->inst.MinValue)
			{
				ThumbLenOrgPos = GuideLenOrgPos;
			}
			else if (t->inst.SliderValue == t->inst.MaxValue)
			{
				ThumbLenOrgPos = GuideLenOrgPos + GuideLen - ThumbLen;
			}
			else
			{
				NbOfSteps = (t->inst.MaxValue - t->inst.MinValue) / t->inst.StepSize;
				StepIndex = (t->inst.SliderValue - t->inst.MinValue) / t->inst.StepSize;
				StepLen = GuideLen / NbOfSteps;
				ThumbLenOrgPos = GuideLenOrgPos + StepLen * StepIndex;
			}
		}
	}

	/* set the thumb_pos and thumb_box */
	if (t->orientation == Orientation_left || t->orientation == Orientation_right)
	{
		thumb_box.x_length   = ThumbLen;
		thumb_box.y_length   = ThumbWidth;       // = GuideWidth       = t->inst.BoxSize.y_length;
		thumb_pos.x_position = ThumbLenOrgPos;
		thumb_pos.y_position = ThumbWidthOrgPos; // = GuideWidthOrgPos = t->inst.Position.y_position;
	}
	else // if (t->orientation == Orientation_up || t->orientation == Orientation_down)
	{
		thumb_box.x_length   = ThumbWidth;       // = GuideWidth       = t->inst.BoxSize.x_length;
		thumb_box.y_length   = ThumbLen;
		thumb_pos.x_position = ThumbWidthOrgPos; // = GuideWidthOrgPos = t->inst.Position.x_position;
		thumb_pos.y_position = ThumbLenOrgPos;
	}

	/* draw the Guide */
	/* Guide is invisible. so nothing to do for it. */

	/* draw the Thumb */
	MHEGDisplay_fillRectangle(d, &thumb_pos, &thumb_box, &thumb_col);
}
#endif

	MHEGDisplay_unsetClipRectangle(d);

	return;
}

