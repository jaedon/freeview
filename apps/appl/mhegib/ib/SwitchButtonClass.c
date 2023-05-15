/*
 * SwitchButtonClass.c
 */

#include "MHEGEngine.h"
#include "SwitchButtonClass.h"
#include "Reference.h"

void
SwitchButtonClass_Preparation(SwitchButtonClass *t)
{
	error("SwitchButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

bool
SwitchButtonClass_Activation(SwitchButtonClass *t)
{
	error("SwitchButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return true;
}

bool
SwitchButtonClass_Deactivation(SwitchButtonClass *t)
{
	error("SwitchButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return true;
}

void
SwitchButtonClass_Destruction(SwitchButtonClass *t)
{
	error("SwitchButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
SwitchButtonClass_GetSelectionStatus(SwitchButtonClass *t, GetSelectionStatus *params, OctetString *caller_gid)
{
	error("SwitchButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
SwitchButtonClass_Select(SwitchButtonClass *t)
{
	error("SwitchButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
SwitchButtonClass_Deselect(SwitchButtonClass *t)
{
	error("SwitchButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
SwitchButtonClass_GetLabel(SwitchButtonClass *t, GetLabel *params, OctetString *caller_gid)
{
	error("SwitchButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
SwitchButtonClass_SetLabel(SwitchButtonClass *t, SetLabel *params, OctetString *caller_gid)
{
	error("SwitchButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
SwitchButtonClass_Toggle(SwitchButtonClass *t)
{
	error("SwitchButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
SwitchButtonClass_render(SwitchButtonClass *t, MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	error("SwitchButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

