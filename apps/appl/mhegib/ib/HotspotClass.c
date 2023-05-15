/*
 * HotspotClass.c
 */

#include "MHEGEngine.h"
#include "HotspotClass.h"
#include "Reference.h"

void
HotspotClass_Preparation(HotspotClass *t)
{
	error("HotspotClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

bool
HotspotClass_Activation(HotspotClass *t)
{
	error("HotspotClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return true;
}

bool
HotspotClass_Deactivation(HotspotClass *t)
{
	error("HotspotClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return true;
}

void
HotspotClass_Destruction(HotspotClass *t)
{
	error("HotspotClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
HotspotClass_Select(HotspotClass *t)
{
	error("HotspotClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
HotspotClass_Deselect(HotspotClass *t)
{
	error("HotspotClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
HotspotClass_render(HotspotClass *t, MHEGDisplay *d, XYPosition *pos, OriginalBoxSize *box)
{
	error("HotspotClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

