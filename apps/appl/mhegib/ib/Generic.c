/*
 * Generic.c
 */

#include "MHEGEngine.h"
#include "Generic.h"
#include "Reference.h"
#include "clone.h"
#include "utils.h"

bool
GenericBoolean_getBoolean(GenericBoolean *g, OctetString *caller_gid)
{
	bool val = false;
	VariableClass *var;

	switch(g->choice)
	{
	case GenericBoolean_boolean:
		val = g->u.boolean;
		break;

	case GenericBoolean_indirect_reference:
		/* points to a BooleanVariable */
		if((var = (VariableClass *) MHEGEngine_findObjectReference(&g->u.indirect_reference, caller_gid)) != NULL)
		{
			if(var->rootClass.inst.rtti != RTTI_VariableClass
			|| var->inst.Value.choice != OriginalValue_boolean)
			{
//				error("GenericBoolean indirect reference is not a BooleanVariable");
				error("Not BooleanVariable (%d, %d)", var->rootClass.inst.rtti, var->inst.Value.choice);
			}
			else
			{
				val = var->inst.Value.u.boolean;
			}
		}
		break;

	default:
		error("Unknown GenericBoolean type: %d", g->choice);
		break;
	}

	return val;
}

void
GenericBoolean_setBoolean(GenericBoolean *g, OctetString *caller_gid, bool val)
{
	VariableClass *var;

	switch(g->choice)
	{
	case GenericBoolean_boolean:
		g->u.boolean = val;
		break;

	case GenericBoolean_indirect_reference:
		/* points to a BooleanVariable */
		if((var = (VariableClass *) MHEGEngine_findObjectReference(&g->u.indirect_reference, caller_gid)) != NULL)
		{
			if(var->rootClass.inst.rtti != RTTI_VariableClass
			|| var->inst.Value.choice != OriginalValue_boolean)
			{
//				error("GenericBoolean indirect reference is not a BooleanVariable");
				error("Not BooleanVariable (%d, %d)", var->rootClass.inst.rtti, var->inst.Value.choice);
			}
			else
			{
				var->inst.Value.u.boolean = val;
			}
		}
		break;

	default:
		error("Unknown GenericBoolean type: %d", g->choice);
		break;
	}

	return;
}

int
GenericInteger_getInteger(GenericInteger *g, OctetString *caller_gid)
{
	int val = 0;
	VariableClass *var;

	switch(g->choice)
	{
	case GenericInteger_integer:
		val = g->u.integer;
		break;

	case GenericInteger_indirect_reference:
		/* points to an IntegerVariable */
		if((var = (VariableClass *) MHEGEngine_findObjectReference(&g->u.indirect_reference, caller_gid)) != NULL)
		{
			if(var->rootClass.inst.rtti != RTTI_VariableClass
			|| var->inst.Value.choice != OriginalValue_integer)
			{
//				error("GenericInteger indirect reference is not an IntegerVariable");
				error("Not IntegerVariable (%d, %d)", var->rootClass.inst.rtti, var->inst.Value.choice);
			}
			else
			{
				val = var->inst.Value.u.integer;
			}
		}
		break;

	default:
		error("Unknown GenericInteger type: %d", g->choice);
		break;
	}

	return val;
}

void
GenericInteger_setInteger(GenericInteger *g, OctetString *caller_gid, int val)
{
	VariableClass *var;

	switch(g->choice)
	{
	case GenericInteger_integer:
		g->u.integer = val;
		break;

	case GenericInteger_indirect_reference:
		/* points to an IntegerVariable */
		if((var = (VariableClass *) MHEGEngine_findObjectReference(&g->u.indirect_reference, caller_gid)) != NULL)
		{
			if(var->rootClass.inst.rtti != RTTI_VariableClass
			|| var->inst.Value.choice != OriginalValue_integer)
			{
//				error("GenericInteger indirect reference is not an IntegerVariable");
				error("Not IntegerVariable (%d, %d)", var->rootClass.inst.rtti, var->inst.Value.choice);
			}
			else
			{
				var->inst.Value.u.integer = val;
			}
		}
		break;

	default:
		error("Unknown GenericInteger type: %d", g->choice);
		break;
	}

	return;
}

OctetString *
GenericOctetString_getOctetString(GenericOctetString *g, OctetString *caller_gid)
{
	OctetString *val = NULL;
	VariableClass *var;

	switch(g->choice)
	{
	case GenericOctetString_octetstring:
		val = &g->u.octetstring;
		break;

	case GenericOctetString_indirect_reference:
		/* points to an OctetStringVariable */
		if((var = (VariableClass *) MHEGEngine_findObjectReference(&g->u.indirect_reference, caller_gid)) != NULL)
		{
			if(var->rootClass.inst.rtti != RTTI_VariableClass
			|| var->inst.Value.choice != OriginalValue_octetstring)
			{
//				error("GenericOctetString indirect reference is not an OctetStringVariable");
				error("Not OctetStringVariable (%d, %d)", var->rootClass.inst.rtti, var->inst.Value.choice);
			}
			else
			{
				val = &var->inst.Value.u.octetstring;
			}
		}
		break;

	default:
		error("Unknown GenericOctetString type: %d", g->choice);
		break;
	}

	return val;
}

void
GenericOctetString_setOctetString(GenericOctetString *g, OctetString *caller_gid, OctetString *val)
{
	VariableClass *var;

	switch(g->choice)
	{
	case GenericOctetString_octetstring:
		OctetString_copy(&g->u.octetstring, val);
		break;

	case GenericOctetString_indirect_reference:
		/* points to an OctetStringVariable */
		if((var = (VariableClass *) MHEGEngine_findObjectReference(&g->u.indirect_reference, caller_gid)) != NULL)
		{
			if(var->rootClass.inst.rtti != RTTI_VariableClass
			|| var->inst.Value.choice != OriginalValue_octetstring)
			{
//				error("GenericOctetString indirect reference is not an OctetStringVariable");
				error("Not OctetStringVariable (%d, %d)", var->rootClass.inst.rtti, var->inst.Value.choice);
			}
			else
			{
				OctetString_copy(&var->inst.Value.u.octetstring, val);
			}
		}
		break;

	default:
		error("Unknown GenericOctetString type: %d", g->choice);
		break;
	}

	return;
}

/*
 * if we have a Scene with a GenericObjectReference in it
 * and the GenericObjectReference points to an ObjectRefVariable in the Application
 * and the ObjectReference it contains does not have a group identifier
 * then when should we resolve the group identifier for the ObjectReference?
 * when we read the ObjectReference from the ObjectRefVariable (=> it gets the Application group id)
 * or when we use the ObjectReference in the Scene (=> it gets the Scene group id)
 *
 * we choose to only resolve ObjectReferences when we use them (so we will get the Scene group id)
 * if we did it the other way, we would need to rewrite this function to always return an ExternalReference
 */

ObjectReference *
GenericObjectReference_getObjectReference(GenericObjectReference *g, OctetString *caller_gid)
{
	ObjectReference *ref = NULL;
	VariableClass *var;

	switch(g->choice)
	{
	case GenericObjectReference_direct_reference:
		ref = &g->u.direct_reference;
		break;

	case GenericObjectReference_indirect_reference:
		/* points to an ObjectRefVariable */
		if((var = (VariableClass *) MHEGEngine_findObjectReference(&g->u.indirect_reference, caller_gid)) != NULL)
		{
			if(var->rootClass.inst.rtti != RTTI_VariableClass
			|| var->inst.Value.choice != OriginalValue_object_reference)
			{
//				error("GenericObjectReference indirect reference is not an ObjectRefVariable");
				error("Not ObjectRefVariable (%d, %d)", var->rootClass.inst.rtti, var->inst.Value.choice);
			}
			else
			{
				ref = &var->inst.Value.u.object_reference;
			}
		}
		break;

	default:
		error("Unknown GenericObjectReference type: %d", g->choice);
		break;
	}

	return ref;
}

void
GenericObjectReference_setObjectReference(GenericObjectReference *g, OctetString *caller_gid, ObjectReference *ref)
{
	VariableClass *var;

	switch(g->choice)
	{
	case GenericObjectReference_direct_reference:
		ObjectReference_copy(&g->u.direct_reference, ref);
		break;

	case GenericObjectReference_indirect_reference:
		/* points to an ObjectRefVariable */
		if((var = (VariableClass *) MHEGEngine_findObjectReference(&g->u.indirect_reference, caller_gid)) != NULL)
		{
			if(var->rootClass.inst.rtti != RTTI_VariableClass
			|| var->inst.Value.choice != OriginalValue_object_reference)
			{
//				error("GenericObjectReference indirect reference is not an ObjectRefVariable");
				error("Not ObjectRefVariable (%d, %d)", var->rootClass.inst.rtti, var->inst.Value.choice);
			}
			else
			{
				ObjectReference_copy(&var->inst.Value.u.object_reference, ref);
			}
		}
		break;

	default:
		error("Unknown GenericObjectReference type: %d", g->choice);
		break;
	}

	return;
}

ContentReference *
GenericContentReference_getContentReference(GenericContentReference *g, OctetString *caller_gid)
{
	ContentReference *ref = NULL;
	VariableClass *var;

	switch(g->choice)
	{
	case GenericContentReference_content_reference:
		ref = &g->u.content_reference;
		break;

	case GenericContentReference_indirect_reference:
		/* points to a ContentRefVariable */
		if((var = (VariableClass *) MHEGEngine_findObjectReference(&g->u.indirect_reference, caller_gid)) != NULL)
		{
			if(var->rootClass.inst.rtti != RTTI_VariableClass
			#if 0 // org
			|| var->inst.Value.choice != OriginalValue_content_reference)
			#else // hmkim : modified.
			|| (var->inst.Value.choice != OriginalValue_octetstring && var->inst.Value.choice != OriginalValue_content_reference))
			#endif
			{
//				error("GenericContentReference indirect reference is not an ContentRefVariable");
				error("Not ContentRefVariable (%d, %d)", var->rootClass.inst.rtti, var->inst.Value.choice);
			}
			else
			{
				ref = &var->inst.Value.u.content_reference;
			}
		}
		break;

	default:
		error("Unknown GenericContentReference type: %d", g->choice);
		break;
	}

	return ref;
}

void
GenericContentReference_setContentReference(GenericContentReference *g, OctetString *caller_gid, ContentReference *ref)
{
	VariableClass *var;

	switch(g->choice)
	{
	case GenericContentReference_content_reference:
		/* a ContentReference is just an OctetString */
		OctetString_copy(&g->u.content_reference, ref);
		break;

	case GenericContentReference_indirect_reference:
		/* points to an ContentRefVariable */
		if((var = (VariableClass *) MHEGEngine_findObjectReference(&g->u.indirect_reference, caller_gid)) != NULL)
		{
			if(var->rootClass.inst.rtti != RTTI_VariableClass
			#if 0 // org
			|| var->inst.Value.choice != OriginalValue_content_reference)
			#else // hmkim : modified.
			|| (var->inst.Value.choice != OriginalValue_octetstring && var->inst.Value.choice != OriginalValue_content_reference))
			#endif
			{
//				error("GenericContentReference indirect reference is not an ContentRefVariable");
				error("Not ContentRefVariable (%d, %d)", var->rootClass.inst.rtti, var->inst.Value.choice);
			}
			else
			{
				/* a ContentReference is just an OctetString */
				OctetString_copy(&var->inst.Value.u.content_reference, ref);
			}
		}
		break;

	default:
		error("Unknown GenericContentReference type: %d", g->choice);
		break;
	}

	return;
}

