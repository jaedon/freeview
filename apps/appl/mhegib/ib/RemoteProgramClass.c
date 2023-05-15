/*
 * RemoteProgramClass.c
 */

#include "MHEGEngine.h"
#include "RemoteProgramClass.h"
#include "Reference.h"

void
RemoteProgramClass_Preparation(RemoteProgramClass *t)
{
	error("RemoteProgramClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
RemoteProgramClass_Activation(RemoteProgramClass *t)
{
	error("RemoteProgramClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
RemoteProgramClass_Deactivation(RemoteProgramClass *t)
{
	error("RemoteProgramClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
RemoteProgramClass_Destruction(RemoteProgramClass *t)
{
	error("RemoteProgramClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

