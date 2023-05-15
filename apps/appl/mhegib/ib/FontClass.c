/*
 * FontClass.c
 */

#include "MHEGEngine.h"
#include "FontClass.h"
#include "Reference.h"

void
FontClass_Preparation(FontClass *t)
{
	error("FontClass: %s; not supported in UK MHEG Profile but Downloadable Fonts required in CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
FontClass_Activation(FontClass *t)
{
	error("FontClass: %s; not supported in UK MHEG Profile but Downloadable Fonts required in CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
FontClass_Deactivation(FontClass *t)
{
	error("FontClass: %s; not supported in UK MHEG Profile but Downloadable Fonts required in CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
FontClass_Destruction(FontClass *t)
{
	error("FontClass: %s; not supported in UK MHEG Profile but Downloadable Fonts required in CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

