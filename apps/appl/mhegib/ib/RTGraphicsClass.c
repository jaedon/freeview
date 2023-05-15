/*
 * RTGraphicsClass.c
 */

#include "MHEGEngine.h"
#include "RTGraphicsClass.h"
#include "Reference.h"

void
RTGraphicsClass_Preparation(RTGraphicsClass *t)
{
	error("RTGraphicsClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

bool
RTGraphicsClass_Activation(RTGraphicsClass *t)
{
	error("RTGraphicsClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return true;
}

bool
RTGraphicsClass_Deactivation(RTGraphicsClass *t)
{
	error("RTGraphicsClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return true;
}

void
RTGraphicsClass_Destruction(RTGraphicsClass *t)
{
	error("RTGraphicsClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
RTGraphicsClass_render(RTGraphicsClass *t, MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	error("RTGraphicsClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

