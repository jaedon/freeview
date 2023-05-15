/*
 * CursorShapeClass.c
 */

#include "MHEGEngine.h"
#include "CursorShapeClass.h"
#include "Reference.h"

void
CursorShapeClass_Preparation(CursorShapeClass *t)
{
	error("CursorShapeClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
CursorShapeClass_Activation(CursorShapeClass *t)
{
	error("CursorShapeClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
CursorShapeClass_Deactivation(CursorShapeClass *t)
{
	error("CursorShapeClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

void
CursorShapeClass_Destruction(CursorShapeClass *t)
{
	error("CursorShapeClass: %s; not supported in UK / CI+ MHEG Profile", ExternalReference_name(&t->rootClass.inst.ref));

	return;
}

