/*
 * DynamicLineArtClass.c
 */

#include "MHEGEngine.h"
#include "DynamicLineArtClass.h"
#include "LineArtClass.h"
#include "RootClass.h"
#include "Reference.h"
#include "Generic.h"
#include "VariableClass.h"
#include "clone.h"

void
DynamicLineArtClass_Preparation(DynamicLineArtClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("DynamicLineArtClass: %s; Preparation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been prepared */
	if(t->rootClass.inst.AvailabilityStatus)
		return;

	/* retrieve the object from an entity outside the engine and set each internal attribute of the object to its initial value */
	default_LineArtClassInstanceVars(t, &t->inst);

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&t->rootClass);

	/* offscreen canvas to draw on */
	t->inst.canvas = new_MHEGCanvas(t->inst.BoxSize.x_length, t->inst.BoxSize.y_length);

	/*
	 * default value for BorderedBoundingBox is true
	 * the border uses OriginalLineWidth/Style/Colour
	 * ie it can never change
	 */
	if(!t->have_bordered_bounding_box || t->bordered_bounding_box)
		MHEGCanvas_setBorder(t->inst.canvas, t->original_line_width, t->original_line_style, &t->inst.OriginalRefLineColour);

	/* now we have set the border, clear the drawing area */
	MHEGCanvas_clear(t->inst.canvas, &t->inst.OriginalRefFillColour);

	/* add it to the DisplayStack of the active application */
	MHEGEngine_addVisibleObject(&t->rootClass);

	return;
}

bool
DynamicLineArtClass_Activation(DynamicLineArtClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("DynamicLineArtClass: %s; Activation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been activated */
	if(t->rootClass.inst.RunningStatus)
		return false;

	/* has it been prepared yet */
	if(!t->rootClass.inst.AvailabilityStatus)
	{
		/* generates an IsAvailable event */
		DynamicLineArtClass_Preparation(t);
	}

	/* Activation inherited from the RootClass */
	RootClass_Activation(&t->rootClass);

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
DynamicLineArtClass_Deactivation(DynamicLineArtClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("DynamicLineArtClass: %s; Deactivation", ExternalReference_name(&t->rootClass.inst.ref));
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
DynamicLineArtClass_Destruction(DynamicLineArtClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("DynamicLineArtClass: %s; Destruction", ExternalReference_name(&t->rootClass.inst.ref));
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
		DynamicLineArtClass_Deactivation(t);
	}

	/*
	 * spec says we should handle caching here
	 * rb-download caches everything
	 */

	free_LineArtClassInstanceVars(&t->inst);

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
DynamicLineArtClass_Clone(DynamicLineArtClass *t, Clone *params, OctetString *caller_gid)
{
//	verbose_todo("%s not yet implemented", __FUNCTION__);

	verbose_class("DynamicLineArtClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));

	CLONE_OBJECT(t, DynamicLineArtClass, dynamic_line_art);

	return;
}

void
DynamicLineArtClass_SetPosition(DynamicLineArtClass *t, SetPosition *params, OctetString *caller_gid)
{
//	XYPosition old;

	/* corrigendum says we don't need to clear to OriginalRefFillColour */

//	old.x_position = t->inst.Position.x_position;
//	old.y_position = t->inst.Position.y_position;

	t->inst.Position.x_position = GenericInteger_getInteger(&params->new_x_position, caller_gid);
	t->inst.Position.y_position = GenericInteger_getInteger(&params->new_y_position, caller_gid);

	verbose_class("DynamicLineArtClass: %s; SetPosition; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.Position.x_position, t->inst.Position.y_position);

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
DynamicLineArtClass_GetPosition(DynamicLineArtClass *t, GetPosition *params, OctetString *caller_gid)
{
	VariableClass *var;

	verbose_class("DynamicLineArtClass: %s; GetPosition", ExternalReference_name(&t->rootClass.inst.ref));

	/* X position */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("DynamicLineArtClass: GetPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Position.x_position);

	/* Y position */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_position_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("DynamicLineArtClass: GetPosition: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.Position.y_position);

	return;
}

void
DynamicLineArtClass_SetBoxSize(DynamicLineArtClass *t, SetBoxSize *params, OctetString *caller_gid)
{
//	OriginalBoxSize old;

//	old.x_length = t->inst.BoxSize.x_length;
//	old.y_length = t->inst.BoxSize.y_length;

	t->inst.BoxSize.x_length = GenericInteger_getInteger(&params->x_new_box_size, caller_gid);
	t->inst.BoxSize.y_length = GenericInteger_getInteger(&params->y_new_box_size, caller_gid);

	verbose_class("DynamicLineArtClass: %s; SetBoxSize; (%d, %d)", ExternalReference_name(&t->rootClass.inst.ref), t->inst.BoxSize.x_length, t->inst.BoxSize.y_length);

	/* spec says we should fill the drawing area with OriginalRefFillColour */

	/* delete the old drawing area and create a new one at the new size */
	free_MHEGCanvas(t->inst.canvas);
	t->inst.canvas = new_MHEGCanvas(t->inst.BoxSize.x_length, t->inst.BoxSize.y_length);

	/* default value for BorderedBoundingBox is true */
	if(!t->have_bordered_bounding_box || t->bordered_bounding_box)
		MHEGCanvas_setBorder(t->inst.canvas, t->original_line_width, t->original_line_style, &t->inst.OriginalRefLineColour);

	/* now we have set the border, clear the drawing area */
	MHEGCanvas_clear(t->inst.canvas, &t->inst.OriginalRefFillColour);

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
DynamicLineArtClass_GetBoxSize(DynamicLineArtClass *t, GetBoxSize *params, OctetString *caller_gid)
{
	VariableClass *var;

	verbose_class("DynamicLineArtClass: %s; GetBoxSize", ExternalReference_name(&t->rootClass.inst.ref));

	/* width */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->x_box_size_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("DynamicLineArtClass: GetBoxSize: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BoxSize.x_length);

	/* height */
	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->y_box_size_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("DynamicLineArtClass: GetBoxSize: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.BoxSize.y_length);

	return;
}

void
DynamicLineArtClass_BringToFront(DynamicLineArtClass *t)
{
	verbose_class("DynamicLineArtClass: %s; BringToFront", ExternalReference_name(&t->rootClass.inst.ref));

	MHEGEngine_bringToFront(&t->rootClass);

	/* corrigendum says we don't need to clear to OriginalRefFillColour */

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
DynamicLineArtClass_SendToBack(DynamicLineArtClass *t)
{
	verbose_class("DynamicLineArtClass: %s; SendToBack", ExternalReference_name(&t->rootClass.inst.ref));

	MHEGEngine_sendToBack(&t->rootClass);

	/* corrigendum says we don't need to clear to OriginalRefFillColour */

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
DynamicLineArtClass_PutBefore(DynamicLineArtClass *t, PutBefore *params, OctetString *caller_gid)
{
	ObjectReference *ref;
	RootClass *obj;

	verbose_class("DynamicLineArtClass: %s; PutBefore", ExternalReference_name(&t->rootClass.inst.ref));

	if(((ref = GenericObjectReference_getObjectReference(&params->reference_visible, caller_gid)) != NULL)
	&& ((obj = MHEGEngine_findObjectReference(ref, caller_gid)) != NULL))
	{
		MHEGEngine_putBefore(&t->rootClass, obj);

		/* corrigendum says we don't need to clear to OriginalRefFillColour */

		/* if it is active, redraw it */
		/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
		if(t->rootClass.inst.RunningStatus)
			MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
		*/
	}

	return;
}

void
DynamicLineArtClass_PutBehind(DynamicLineArtClass *t, PutBehind *params, OctetString *caller_gid)
{
	ObjectReference *ref;
	RootClass *obj;

	verbose_class("DynamicLineArtClass: %s; PutBehind", ExternalReference_name(&t->rootClass.inst.ref));

	if(((ref = GenericObjectReference_getObjectReference(&params->reference_visible, caller_gid)) != NULL)
	&& ((obj = MHEGEngine_findObjectReference(ref, caller_gid)) != NULL))
	{
		MHEGEngine_putBehind(&t->rootClass, obj);

		/* corrigendum says we don't need to clear to OriginalRefFillColour */

		/* if it is active, redraw it */
		/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
		if(t->rootClass.inst.RunningStatus)
			MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
		*/
	}

	return;
}

void
DynamicLineArtClass_SetPaletteRef(DynamicLineArtClass *t, SetPaletteRef *params, OctetString *caller_gid)
{
	/* UK MHEG Profile says we don't support PaletteClass */
	error("DynamicLineArtClass: %s; SetPaletteRef not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
DynamicLineArtClass_SetLineWidth(DynamicLineArtClass *t, SetLineWidth *params, OctetString *caller_gid)
{
	t->inst.LineWidth = GenericInteger_getInteger(&params->new_line_width, caller_gid);

	verbose_class("DynamicLineArtClass: %s; SetLineWidth; %d", ExternalReference_name(&t->rootClass.inst.ref), t->inst.LineWidth);

	return;
}

void
DynamicLineArtClass_SetLineStyle(DynamicLineArtClass *t, SetLineStyle *params, OctetString *caller_gid)
{
	t->inst.LineStyle = GenericInteger_getInteger(&params->new_line_style, caller_gid);

	verbose_class("DynamicLineArtClass: %s; SetLineStyle; %d", ExternalReference_name(&t->rootClass.inst.ref), t->inst.LineStyle);

	return;
}

void
DynamicLineArtClass_SetLineColour(DynamicLineArtClass *t, SetLineColour *params, OctetString *caller_gid)
{
	verbose_class("DynamicLineArtClass: %s; SetLineColour", ExternalReference_name(&t->rootClass.inst.ref));

	MHEGColour_fromNewColour(&t->inst.RefLineColour, &params->new_line_colour, caller_gid);

	return;
}

void
DynamicLineArtClass_SetFillColour(DynamicLineArtClass *t, SetFillColour *params, OctetString *caller_gid)
{
	verbose_class("DynamicLineArtClass: %s; SetFillColour", ExternalReference_name(&t->rootClass.inst.ref));

	/* if no colour is given, use transparent */
	if(params->have_new_fill_colour)
		MHEGColour_fromNewColour(&t->inst.RefFillColour, &params->new_fill_colour, caller_gid);
	else
		MHEGColour_transparent(&t->inst.RefFillColour);

	return;
}

void
DynamicLineArtClass_GetLineWidth(DynamicLineArtClass *t, GetLineWidth *params, OctetString *caller_gid)
{
	VariableClass *var;

	verbose_class("DynamicLineArtClass: %s; GetLineWidth", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->line_width_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("DynamicLineArtClass: GetLineWidth: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.LineWidth);

	return;
}

void
DynamicLineArtClass_GetLineStyle(DynamicLineArtClass *t, GetLineStyle *params, OctetString *caller_gid)
{
	VariableClass *var;

	verbose_class("DynamicLineArtClass: %s; GetLineStyle", ExternalReference_name(&t->rootClass.inst.ref));

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->line_style_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti != RTTI_VariableClass
	|| VariableClass_type(var) != OriginalValue_integer)
	{
		error("DynamicLineArtClass: GetLineStyle: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
		return;
	}

	IntegerVariableClass_setInteger(var, t->inst.LineStyle);

	return;
}

void
DynamicLineArtClass_GetLineColour(DynamicLineArtClass *t, GetLineColour *params, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("DynamicLineArtClass: %s; GetLineColour", ExternalReference_name(&t->rootClass.inst.ref));

	// GetLineColour(LineColourVar) Returns the current LineColour in the variable LineColourVar
	// LineColourVar shall refer to an active OctetStringVariable object if the RefLineColour is specified as an OctetString.
	// LineColourVar shall refer to an active IntegerVariable object if the RefLineColour is specified as an Integer.
	// The Target object shall be an available DynamicLineArt object

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->line_colour_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti == RTTI_VariableClass && VariableClass_type(var) == OriginalValue_octetstring)
	{
		OctetString val = {0, NULL};

		// TODO : get OctetString val from t->inst.RefLineColour (MHEGColour.c 에 변환 함수 추가 구현 필요)

		OctetStringVariableClass_setOctetString(var, &val);
	}
	else if(var->rootClass.inst.rtti == RTTI_VariableClass && VariableClass_type(var) == OriginalValue_integer)
	{
		int val = 0;

		// TODO : get int val from t->inst.RefLineColour (MHEGColour.c 에 변환 함수 추가 구현 필요. 단, PaletteClass not supported in UK / CI+ MHEG Profile)

		error("DynamicLineArtClass: GetLineColour: PaletteClass not supported in UK / CI+ MHEG Profile");

		IntegerVariableClass_setInteger(var, val);
	}
	else
	{
		error("DynamicLineArtClass: GetLineColour: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
	}
}
#endif

	return;
}

void
DynamicLineArtClass_GetFillColour(DynamicLineArtClass *t, GetFillColour *params, OctetString *caller_gid)
{
	verbose_todo("%s not yet implemented", __FUNCTION__);

#if 1 // hmkim : added.
{
	VariableClass *var;

	verbose_class("DynamicLineArtClass: %s; GetFillColour", ExternalReference_name(&t->rootClass.inst.ref));

	// GetFillColour(FillColourVar) Returns the current RefFillColour in the variable FillColourVar
	// FillColourVar shall shall refer to an active OctetStringVariable object if the RefFillColour is specified as an OctetString.
	// FillColourVar shall shall refer to an active IntegerVariable object if the RefFiIlColour is specified as an Integer.
	// The Target object shall be an available DynamicLineArt object

	if((var = (VariableClass *) MHEGEngine_findObjectReference(&params->fill_colour_var, caller_gid)) == NULL)
		return;

	if(var->rootClass.inst.rtti == RTTI_VariableClass && VariableClass_type(var) == OriginalValue_octetstring)
	{
		OctetString val = {0, NULL};

		// TODO : get OctetString val from t->inst.RefFillColour (MHEGColour.c 에 변환 함수 추가 구현 필요)

		OctetStringVariableClass_setOctetString(var, &val);
	}
	else if(var->rootClass.inst.rtti == RTTI_VariableClass && VariableClass_type(var) == OriginalValue_integer)
	{
		int val = 0;

		// TODO : get int val from t->inst.RefFillColour (MHEGColour.c 에 변환 함수 추가 구현 필요. 단, PaletteClass not supported in UK / CI+ MHEG Profile)

		error("DynamicLineArtClass: GetFillColour: PaletteClass not supported in UK / CI+ MHEG Profile");

		IntegerVariableClass_setInteger(var, val);
	}
	else
	{
		error("DynamicLineArtClass: GetFillColour: type mismatch (%d, %d)", var->rootClass.inst.rtti, VariableClass_type(var));
	}
}
#endif

	return;
}

void
DynamicLineArtClass_DrawArc(DynamicLineArtClass *t, DrawArc *params, OctetString *caller_gid)
{
	XYPosition pos;
	OriginalBoxSize box;
	int start;
	int arc;

	pos.x_position = GenericInteger_getInteger(&params->x, caller_gid);
	pos.y_position = GenericInteger_getInteger(&params->y, caller_gid);
	box.x_length = GenericInteger_getInteger(&params->ellipse_width, caller_gid);
	box.y_length = GenericInteger_getInteger(&params->ellipse_height, caller_gid);
	start = GenericInteger_getInteger(&params->start_angle, caller_gid);
	arc = GenericInteger_getInteger(&params->arc_angle, caller_gid);

	verbose_class("DynamicLineArtClass: %s; DrawArc; (%d, %d, %d, %d, %d, %d)", ExternalReference_name(&t->rootClass.inst.ref),
		pos.x_position, pos.y_position, box.x_length, box.y_length, start, arc);

	/* ISO spec says ArcAngle should not be 0 */
	if(arc == 0)
	{
		error("DynamicLineArtClass_DrawArc: invalid ArcAngle (%d)", arc);
		return;
	}
	/* corrigendum puts these limits on the ellipse size */
	if(box.x_length < 0 || box.y_length < 0)
	{
		error("DynamicLineArtClass_DrawArc: invalid ellipse size (%d,%d)", box.x_length, box.y_length);
		return;
	}

	MHEGCanvas_drawArc(t->inst.canvas, &pos, &box, start, arc,
			   t->inst.LineWidth, t->inst.LineStyle,
			   &t->inst.RefLineColour);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
DynamicLineArtClass_DrawSector(DynamicLineArtClass *t, DrawSector *params, OctetString *caller_gid)
{
	XYPosition pos;
	OriginalBoxSize box;
	int start;
	int arc;

	pos.x_position = GenericInteger_getInteger(&params->x, caller_gid);
	pos.y_position = GenericInteger_getInteger(&params->y, caller_gid);
	box.x_length = GenericInteger_getInteger(&params->ellipse_width, caller_gid);
	box.y_length = GenericInteger_getInteger(&params->ellipse_height, caller_gid);
	start = GenericInteger_getInteger(&params->start_angle, caller_gid);
	arc = GenericInteger_getInteger(&params->arc_angle, caller_gid);

	verbose_class("DynamicLineArtClass: %s; DrawSector; (%d, %d, %d, %d, %d, %d)", ExternalReference_name(&t->rootClass.inst.ref),
		pos.x_position, pos.y_position, box.x_length, box.y_length, start, arc);

	/* ISO spec says ArcAngle should not be 0 */
	if(arc == 0)
	{
		error("DynamicLineArtClass_DrawSector: invalid ArcAngle (%d)", arc);
		return;
	}
	/* corrigendum puts these limits on the ellipse size */
	if(box.x_length < 0 || box.y_length < 0)
	{
		error("DynamicLineArtClass_DrawSector: invalid ellipse size (%d,%d)", box.x_length, box.y_length);
		return;
	}

	MHEGCanvas_drawSector(t->inst.canvas, &pos, &box, start, arc,
			      t->inst.LineWidth, t->inst.LineStyle,
			      &t->inst.RefLineColour, &t->inst.RefFillColour);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
DynamicLineArtClass_DrawLine(DynamicLineArtClass *t, DrawLine *params, OctetString *caller_gid)
{
	XYPosition p1;
	XYPosition p2;

	p1.x_position = GenericInteger_getInteger(&params->x1, caller_gid);
	p1.y_position = GenericInteger_getInteger(&params->y1, caller_gid);
	p2.x_position = GenericInteger_getInteger(&params->x2, caller_gid);
	p2.y_position = GenericInteger_getInteger(&params->y2, caller_gid);

	verbose_class("DynamicLineArtClass: %s; DrawLine; (%d, %d, %d, %d)", ExternalReference_name(&t->rootClass.inst.ref),
		p1.x_position, p1.y_position, p2.x_position, p2.y_position);

	MHEGCanvas_drawLine(t->inst.canvas, &p1, &p2,
			    t->inst.LineWidth, t->inst.LineStyle,
			    &t->inst.RefLineColour);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
DynamicLineArtClass_DrawOval(DynamicLineArtClass *t, DrawOval *params, OctetString *caller_gid)
{
	XYPosition pos;
	OriginalBoxSize box;

	pos.x_position = GenericInteger_getInteger(&params->x, caller_gid);
	pos.y_position = GenericInteger_getInteger(&params->y, caller_gid);
	box.x_length = GenericInteger_getInteger(&params->ellipse_width, caller_gid);
	box.y_length = GenericInteger_getInteger(&params->ellipse_height, caller_gid);

	verbose_class("DynamicLineArtClass: %s; DrawOval; (%d, %d, %d, %d)", ExternalReference_name(&t->rootClass.inst.ref),
		pos.x_position, pos.y_position, box.x_length, box.y_length);

	/* corrigendum puts these limits on the ellipse size */
	if(box.x_length < 0 || box.y_length < 0)
	{
		error("DynamicLineArtClass_DrawOval: invalid ellipse size (%d,%d)", box.x_length, box.y_length);
		return;
	}

	MHEGCanvas_drawOval(t->inst.canvas, &pos, &box,
			    t->inst.LineWidth, t->inst.LineStyle,
			    &t->inst.RefLineColour, &t->inst.RefFillColour);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
DynamicLineArtClass_DrawPolygon(DynamicLineArtClass *t, DrawPolygon *params, OctetString *caller_gid)
{
	LIST_OF(XYPosition) *xy_list = NULL;
	LIST_TYPE(XYPosition) *xy;
	LIST_TYPE(Point) *pt;

	verbose_class("DynamicLineArtClass: %s; DrawPolygon", ExternalReference_name(&t->rootClass.inst.ref));

	for(pt=params->pointlist; pt; pt=pt->next)
	{
		xy = safe_malloc(sizeof(LIST_TYPE(XYPosition)));
		xy->item.x_position = GenericInteger_getInteger(&pt->item.x, caller_gid);
		xy->item.y_position = GenericInteger_getInteger(&pt->item.y, caller_gid);
		LIST_APPEND(&xy_list, xy);
	}

	MHEGCanvas_drawPolygon(t->inst.canvas, xy_list,
			       t->inst.LineWidth, t->inst.LineStyle,
			       &t->inst.RefLineColour, &t->inst.RefFillColour);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	LIST_FREE(&xy_list, XYPosition, safe_free);

	return;
}

void
DynamicLineArtClass_DrawPolyline(DynamicLineArtClass *t, DrawPolyline *params, OctetString *caller_gid)
{
	LIST_OF(XYPosition) *xy_list = NULL;
	LIST_TYPE(XYPosition) *xy;
	LIST_TYPE(Point) *pt;

	verbose_class("DynamicLineArtClass: %s; DrawPolyline", ExternalReference_name(&t->rootClass.inst.ref));

	for(pt=params->pointlist; pt; pt=pt->next)
	{
		xy = safe_malloc(sizeof(LIST_TYPE(XYPosition)));
		xy->item.x_position = GenericInteger_getInteger(&pt->item.x, caller_gid);
		xy->item.y_position = GenericInteger_getInteger(&pt->item.y, caller_gid);
		LIST_APPEND(&xy_list, xy);
	}

	MHEGCanvas_drawPolyline(t->inst.canvas, xy_list,
			       t->inst.LineWidth, t->inst.LineStyle,
			       &t->inst.RefLineColour);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	LIST_FREE(&xy_list, XYPosition, safe_free);

	return;
}

void
DynamicLineArtClass_DrawRectangle(DynamicLineArtClass *t, DrawRectangle *params, OctetString *caller_gid)
{
	XYPosition pos;
	OriginalBoxSize box;

	pos.x_position = GenericInteger_getInteger(&params->x1, caller_gid);
	pos.y_position = GenericInteger_getInteger(&params->y1, caller_gid);
	box.x_length = GenericInteger_getInteger(&params->x2, caller_gid) - pos.x_position;
	box.y_length = GenericInteger_getInteger(&params->y2, caller_gid) - pos.y_position;

	verbose_class("DynamicLineArtClass: %s; DrawRectangle; (%d, %d, %d, %d)", ExternalReference_name(&t->rootClass.inst.ref),
		pos.x_position, pos.y_position, box.x_length, box.y_length);

	MHEGCanvas_drawRectangle(t->inst.canvas, &pos, &box,
				 t->inst.LineWidth, t->inst.LineStyle,
				 &t->inst.RefLineColour, &t->inst.RefFillColour);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
DynamicLineArtClass_Clear(DynamicLineArtClass *t)
{
	verbose_class("DynamicLineArtClass: %s; Clear", ExternalReference_name(&t->rootClass.inst.ref));

	/* fill with OriginalRefFillColour */
	MHEGCanvas_clear(t->inst.canvas, &t->inst.OriginalRefFillColour);

	/* if it is active, redraw it */
	/* MHEGEngine_processMHEGEvents() 에서 모든 처리가 종료된 후에 한번만 호출한다
	if(t->rootClass.inst.RunningStatus)
		MHEGEngine_redrawArea(&t->inst.Position, &t->inst.BoxSize);
	*/

	return;
}

void
DynamicLineArtClass_render(DynamicLineArtClass *t, MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	XYPosition ins_pos;
	OriginalBoxSize ins_box;
	XYPosition src;

	verbose_class("DynamicLineArtClass: %s; render", ExternalReference_name(&t->rootClass.inst.ref));

	if(!intersects(pos, box, &t->inst.Position, &t->inst.BoxSize, &ins_pos, &ins_box))
		return;

	MHEGDisplay_setClipRectangle(d, &ins_pos, &ins_box);

	/* work out where the intersection starts on the canvas */
	src.x_position = ins_pos.x_position - t->inst.Position.x_position;
	src.y_position = ins_pos.y_position - t->inst.Position.y_position;

	MHEGDisplay_drawCanvas(d, &src, &ins_box, t->inst.canvas, &ins_pos);

	MHEGDisplay_unsetClipRectangle(d);

	return;
}

