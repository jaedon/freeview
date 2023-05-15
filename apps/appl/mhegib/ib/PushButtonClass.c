/*
 * PushButtonClass.c
 */

#include "MHEGEngine.h"
#include "PushButtonClass.h"
#include "Reference.h"

void
PushButtonClass_Preparation(PushButtonClass *t)
{
	error("PushButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

bool
PushButtonClass_Activation(PushButtonClass *t)
{
	error("PushButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return true;
}

bool
PushButtonClass_Deactivation(PushButtonClass *t)
{
	error("PushButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return true;
}

void
PushButtonClass_Destruction(PushButtonClass *t)
{
	error("PushButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
PushButtonClass_Select(PushButtonClass *t)
{
	error("PushButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
PushButtonClass_Deselect(PushButtonClass *t)
{
	error("PushButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
PushButtonClass_GetLabel(PushButtonClass *t, GetLabel *params, OctetString *caller_gid)
{
	error("PushButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
PushButtonClass_SetLabel(PushButtonClass *t, SetLabel *params, OctetString *caller_gid)
{
	error("PushButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
PushButtonClass_render(PushButtonClass *t, MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	error("PushButtonClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

