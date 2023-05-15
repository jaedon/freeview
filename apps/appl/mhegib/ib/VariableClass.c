/*
 * VariableClass.c
 */

#include <ctype.h> // for isdigit()

#include "MHEGEngine.h"
#include "VariableClass.h"
#include "RootClass.h"
#include "Generic.h"
#include "Reference.h"
#include "clone.h"

static void
default_VariableClassInstanceVars(VariableClass *t, VariableClassInstanceVars *v)
{
	VK_memset(v, 0x00, sizeof(VariableClassInstanceVars));

	/* take a copy of the OriginalValue */
	OriginalValue_dup(&v->Value, &t->original_value);

	return;
}

static void
free_VariableClassInstanceVars(VariableClassInstanceVars *v)
{
	free_OriginalValue(&v->Value);

	return;
}

void
VariableClass_Preparation(VariableClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("VariableClass: %s; Preparation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been prepared */
	if(t->rootClass.inst.AvailabilityStatus)
		return;

	/* retrieve the object from an entity outside the engine and set each internal attribute of the object to its initial value */
	default_VariableClassInstanceVars(t, &t->inst);

	/* Preparation inherited from the RootClass */
	RootClass_Preparation(&t->rootClass);

	return;
}

void
VariableClass_Activation(VariableClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("VariableClass: %s; Activation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* has it already been activated */
	if(t->rootClass.inst.RunningStatus)
		return;

	/* has it been prepared yet */
	if(!t->rootClass.inst.AvailabilityStatus)
	{
		/* generates an IsAvailable event */
		VariableClass_Preparation(t);
	}

#if 0 // org
	/* set its RunningStatus and generate IsRunning event */
	t->rootClass.inst.RunningStatus = true;
	MHEGEngine_generateEvent(&t->rootClass.inst.ref, EventType_is_running, NULL);
#else // hmkim : modified.
	/* Activation inherited from the RootClass */
	RootClass_Activation(&t->rootClass);
#endif

	return;
}

void
VariableClass_Deactivation(VariableClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("VariableClass: %s; Deactivation", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already deactivated */
	if(!t->rootClass.inst.RunningStatus)
		return;

	/* Deactivation inherited from the RootClass */
	RootClass_Deactivation(&t->rootClass);

	return;
}

void
VariableClass_Destruction(VariableClass *t)
{
#if (CLS_VERBOSE)
	verbose_class("VariableClass: %s; Destruction", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	/* is it already destroyed */
	if(!t->rootClass.inst.AvailabilityStatus)
		return;

#if 1 // hmkim : added.
	/* Deactivate it if it is running */
	if(t->rootClass.inst.RunningStatus)
	{
		/* generates an IsStopped event */
		VariableClass_Deactivation(t);
	}
#endif

	/*
	 * spec says we should handle caching here
	 * rb-download caches everything
	 */

	free_VariableClassInstanceVars(&t->inst);

	/* Destruction inherited from the RootClass */
	RootClass_Destruction(&t->rootClass);

	return;
}

void
VariableClass_Clone(VariableClass *t, Clone *clone, OctetString *caller_gid)
{
	switch(t->inst.Value.choice)
	{
	case OriginalValue_boolean:
		BooleanVariableClass_Clone(t, clone, caller_gid);
		break;

	case OriginalValue_integer:
		IntegerVariableClass_Clone(t, clone, caller_gid);
		break;

	case OriginalValue_octetstring:
		OctetStringVariableClass_Clone(t, clone, caller_gid);
		break;

	case OriginalValue_object_reference:
		ObjectRefVariableClass_Clone(t, clone, caller_gid);
		break;

	case OriginalValue_content_reference:
		ContentRefVariableClass_Clone(t, clone, caller_gid);
		break;

	default:
		error("Unknown VariableClass %s; type: %d", ExternalReference_name(&t->rootClass.inst.ref), t->inst.Value.choice);
		break;
	}

	return;
}

/*
 * caller_gid is used to resolve the Generic variables in the new_value
 */

void
VariableClass_SetVariable(VariableClass *v, NewVariableValue *new_value, OctetString *caller_gid)
{
	switch(v->inst.Value.choice)
	{
	case OriginalValue_boolean:
		BooleanVariableClass_SetVariable(v, new_value, caller_gid);
		break;

	case OriginalValue_integer:
		IntegerVariableClass_SetVariable(v, new_value, caller_gid);
		break;

	case OriginalValue_octetstring:
		OctetStringVariableClass_SetVariable(v, new_value, caller_gid);
		break;

	case OriginalValue_object_reference:
		ObjectRefVariableClass_SetVariable(v, new_value, caller_gid);
		break;

	case OriginalValue_content_reference:
		ContentRefVariableClass_SetVariable(v, new_value, caller_gid);
		break;

	default:
		error("Unknown VariableClass %s; type: %d", ExternalReference_name(&v->rootClass.inst.ref), v->inst.Value.choice);
		break;
	}

#if (CLS_VERBOSE)
	verbose_class("SetVariable: %s", VariableClass_stringValue(v));
#endif
	return;
}

/*
 * caller_gid is used to resolve the Generic variables in the ComparisionValue
 */

void
VariableClass_TestVariable(VariableClass *v, int op, ComparisonValue *comp, OctetString *caller_gid)
{
	/* check the types match */
	if(v->inst.Value.choice == OriginalValue_boolean
	&& comp->choice == ComparisonValue_new_generic_boolean)
	{
		bool val = GenericBoolean_getBoolean(&comp->u.new_generic_boolean, caller_gid);
		BooleanVariableClass_TestVariable(v, op, val);
	}
	else if(v->inst.Value.choice == OriginalValue_integer
	&& comp->choice == ComparisonValue_new_generic_integer)
	{
		int val = GenericInteger_getInteger(&comp->u.new_generic_integer, caller_gid);
		IntegerVariableClass_TestVariable(v, op, val);
	}
	else if(v->inst.Value.choice == OriginalValue_octetstring
	&& comp->choice == ComparisonValue_new_generic_octetstring)
	{
		OctetString *val = GenericOctetString_getOctetString(&comp->u.new_generic_octetstring, caller_gid);
		OctetStringVariableClass_TestVariable(v, op, val);
	}
	else if(v->inst.Value.choice == OriginalValue_object_reference
	&& comp->choice == ComparisonValue_new_generic_object_reference)
	{
		ObjectReference *val = GenericObjectReference_getObjectReference(&comp->u.new_generic_object_reference, caller_gid);
		/* need to pass the caller_gid as the ObjectReference may not contain a group id */
		ObjectRefVariableClass_TestVariable(v, op, val, caller_gid);
	}
	else if(v->inst.Value.choice == OriginalValue_content_reference
	&& comp->choice == ComparisonValue_new_generic_content_reference)
	{
		ContentReference *val = GenericContentReference_getContentReference(&comp->u.new_generic_content_reference, caller_gid);
		ContentRefVariableClass_TestVariable(v, op, val);
	}
	else
	{
		error("TestVariable: %s; type mismatch", ExternalReference_name(&v->rootClass.inst.ref));
	}

	return;
}

/*
 * returns OriginalValue_boolean/integer/octetstring/content_reference/object_reference
 */

unsigned int
VariableClass_type(VariableClass *v)
{
	return v->inst.Value.choice;
}

/*
 * returns a static string that will be overwritten by the next call to this routine
 */

static char *_value = NULL;

char *
VariableClass_stringValue(VariableClass *v)
{
	OctetString *oct;

	switch(v->inst.Value.choice)
	{
	case OriginalValue_boolean:
		_value = safe_realloc(_value, 16);
		snprintf(_value, 16, "Boolean %s", v->inst.Value.u.boolean ? "true" : "false");
		return _value;

	case OriginalValue_integer:
		_value = safe_realloc(_value, 64);
		snprintf(_value, 64, "Integer %d", v->inst.Value.u.integer);
		return _value;

	case OriginalValue_octetstring:
		oct = &v->inst.Value.u.octetstring;
		_value = safe_realloc(_value, oct->size + 128);
		snprintf(_value, oct->size + 128, "OctetString '%.*s'", oct->size, oct->data);
		return _value;

	case OriginalValue_object_reference:
		_value = safe_realloc(_value, PATH_MAX + 32);
		snprintf(_value, PATH_MAX + 32, "ObjectReference %s", ObjectReference_name(&v->inst.Value.u.object_reference));
		return _value;

	case OriginalValue_content_reference:
		oct = &v->inst.Value.u.content_reference;
		_value = safe_realloc(_value, oct->size + 128);
		snprintf(_value, oct->size + 128, "ContentReference '%.*s'", oct->size, oct->data);
		return _value;

	default:
		error("Unknown VariableClass %s; type: %d", ExternalReference_name(&v->rootClass.inst.ref), v->inst.Value.choice);
		_value = safe_realloc(_value, 32);
		snprintf(_value, 32, "Invalid VariableClass");
		return _value;
	}
}

void
BooleanVariableClass_Clone(BooleanVariableClass *t, Clone *params, OctetString *caller_gid)
{
#if (CLS_VERBOSE)
	verbose_class("BooleanVariableClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	CLONE_OBJECT(t, VariableClass, boolean_variable);

	return;
}

/*
 * caller_gid is used to resolve the GenericBoolean
 */

void
BooleanVariableClass_SetVariable(BooleanVariableClass *v, NewVariableValue *new_value, OctetString *caller_gid)
{
	bool val;

	/* check the type of the new value */
	if(new_value->choice != NewVariableValue_new_generic_boolean)
	{
		error("SetVariable: %s; type mismatch (%d)", ExternalReference_name(&v->rootClass.inst.ref), new_value->choice);
		return;
	}

	val = GenericBoolean_getBoolean(&new_value->u.new_generic_boolean, caller_gid);

#if (CLS_VERBOSE)
	verbose_class("BooleanVariableClass: %s; SetVariable; %s", ExternalReference_name(&v->rootClass.inst.ref), val ? "true" : "false");
#endif

	v->inst.Value.u.boolean = val;

	return;
}

void
BooleanVariableClass_TestVariable(BooleanVariableClass *v, int op, bool comp)
{
	EventData result;

#if (CLS_VERBOSE)
	verbose_class("BooleanVariableClass: %s; TestVariable", ExternalReference_name(&v->rootClass.inst.ref));
#endif

	/* assert */
	if(v->inst.Value.choice != OriginalValue_boolean)
	{
		fatal("BooleanVariableClass_TestVariable unexpected type: %d", v->inst.Value.choice);
		return;
	}

	result.choice = EventData_boolean;

	switch(op)
	{
	case 1:
		/* equal */
		result.u.boolean = (v->inst.Value.u.boolean == comp);
		break;

	case 2:
		/* not equal */
		result.u.boolean = (v->inst.Value.u.boolean != comp);
		break;

	default:
		error("Unknown operator type: %d", op);
		return;
	}

	MHEGEngine_generateEvent(&v->rootClass.inst.ref, EventType_test_event, &result);

	return;
}

void
BooleanVariableClass_setBoolean(BooleanVariableClass *v, bool val)
{
	/* assert */
	if(v->inst.Value.choice != OriginalValue_boolean)
	{
		fatal("BooleanVariableClass_setBoolean unexpected type: %d", v->inst.Value.choice);
		return;
	}

	v->inst.Value.u.boolean = val;

	return;
}

void
IntegerVariableClass_Clone(IntegerVariableClass *t, Clone *params, OctetString *caller_gid)
{
#if (CLS_VERBOSE)
	verbose_class("IntegerVariableClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	CLONE_OBJECT(t, VariableClass, integer_variable);

	return;
}

/*
 * caller_gid is used to resolve the GenericInteger or GenericOctetString
 */

void
IntegerVariableClass_SetVariable(IntegerVariableClass *v, NewVariableValue *new_value, OctetString *caller_gid)
{
	int val;
	OctetString *oct;
	unsigned int i;

#if (CLS_VERBOSE)
	verbose_class("IntegerVariableClass: %s; SetVariable", ExternalReference_name(&v->rootClass.inst.ref));
#endif

	/* check the type of the new value */
	if(new_value->choice != NewVariableValue_new_generic_integer
	&& new_value->choice != NewVariableValue_new_generic_octet_string)
	{
		error("SetVariable: %s; type mismatch (%d)", ExternalReference_name(&v->rootClass.inst.ref), new_value->choice);
		return;
	}

	if(new_value->choice == NewVariableValue_new_generic_integer)
	{
		val = GenericInteger_getInteger(&new_value->u.new_generic_integer, caller_gid);
	}
	else
	{
		/* convert the OctetString to an int */
		oct = GenericOctetString_getOctetString(&new_value->u.new_generic_octet_string, caller_gid);
		val = 0;
		i = (oct->size != 0 && oct->data[0] == '-') ? 1 : 0;
		while(i < oct->size && isdigit(oct->data[i]))
		{
			val *= 10;
			val += oct->data[i] - '0';
			i ++;
		}
		if(oct->size != 0 && oct->data[0] == '-')
			val = -val;
	}

	v->inst.Value.u.integer = val;

	return;
}

void
IntegerVariableClass_TestVariable(IntegerVariableClass *v, int op, int comp)
{
	EventData result;

#if (CLS_VERBOSE)
	verbose_class("IntegerVariableClass: %s; TestVariable", ExternalReference_name(&v->rootClass.inst.ref));
#endif

	/* assert */
	if(v->inst.Value.choice != OriginalValue_integer)
	{
		fatal("IntegerVariableClass_TestVariable unexpected type: %d", v->inst.Value.choice);
		return;
	}

	result.choice = EventData_boolean;

	switch(op)
	{
	case 1:
		/* equal */
		result.u.boolean = (v->inst.Value.u.integer == comp);
		break;

	case 2:
		/* not equal */
		result.u.boolean = (v->inst.Value.u.integer != comp);
		break;

	case 3:
		/* less than */
		result.u.boolean = (v->inst.Value.u.integer < comp);
		break;

	case 4:
		/* less than or equal to */
		result.u.boolean = (v->inst.Value.u.integer <= comp);
		break;

	case 5:
		/* greater than */
		result.u.boolean = (v->inst.Value.u.integer > comp);
		break;

	case 6:
		/* greater than or equal to */
		result.u.boolean = (v->inst.Value.u.integer >= comp);
		break;

	default:
		error("Unknown operator type: %d", op);
		return;
	}

	MHEGEngine_generateEvent(&v->rootClass.inst.ref, EventType_test_event, &result);

	return;
}

void
IntegerVariableClass_Add(IntegerVariableClass *v, Add *params, OctetString *caller_gid)
{
	int val;

	/* assert */
	if(v->inst.Value.choice != OriginalValue_integer)
	{
		fatal("IntegerVariableClass_Add unexpected type: %d", v->inst.Value.choice);
		return;
	}

	val = GenericInteger_getInteger(&params->value, caller_gid);

#if (CLS_VERBOSE)
	verbose_class("IntegerVariableClass: %s; Add; %d + %d", ExternalReference_name(&v->rootClass.inst.ref), v->inst.Value.u.integer, val);
#endif

	v->inst.Value.u.integer += val;

	return;
}

void
IntegerVariableClass_Subtract(IntegerVariableClass *v, Subtract *params, OctetString *caller_gid)
{
	int val;

	/* assert */
	if(v->inst.Value.choice != OriginalValue_integer)
	{
		fatal("IntegerVariableClass_Subtract unexpected type: %d", v->inst.Value.choice);
		return;
	}

	val = GenericInteger_getInteger(&params->value, caller_gid);

#if (CLS_VERBOSE)
	verbose_class("IntegerVariableClass: %s; Subtract; %d - %d", ExternalReference_name(&v->rootClass.inst.ref), v->inst.Value.u.integer, val);
#endif

	v->inst.Value.u.integer -= val;

	return;
}

void
IntegerVariableClass_Multiply(IntegerVariableClass *v, Multiply *params, OctetString *caller_gid)
{
	int val;

	/* assert */
	if(v->inst.Value.choice != OriginalValue_integer)
	{
		fatal("IntegerVariableClass_Multiply unexpected type: %d", v->inst.Value.choice);
		return;
	}

	val = GenericInteger_getInteger(&params->value, caller_gid);

#if (CLS_VERBOSE)
	verbose_class("IntegerVariableClass: %s; Multiply; %d * %d", ExternalReference_name(&v->rootClass.inst.ref), v->inst.Value.u.integer, val);
#endif

	v->inst.Value.u.integer *= val;

	return;
}

void
IntegerVariableClass_Divide(IntegerVariableClass *v, Divide *params, OctetString *caller_gid)
{
	int val;

	/* assert */
	if(v->inst.Value.choice != OriginalValue_integer)
	{
		fatal("IntegerVariableClass_Divide unexpected type: %d", v->inst.Value.choice);
		return;
	}

	val = GenericInteger_getInteger(&params->value, caller_gid);

#if (CLS_VERBOSE)
	verbose_class("IntegerVariableClass: %s; Divide; %d / %d", ExternalReference_name(&v->rootClass.inst.ref), v->inst.Value.u.integer, val);
#endif

	v->inst.Value.u.integer /= val; // When the result is not an integer value, rounding is made towards 0

	return;
}

void
IntegerVariableClass_Modulo(IntegerVariableClass *v, Modulo *params, OctetString *caller_gid)
{
	int val;

	/* assert */
	if(v->inst.Value.choice != OriginalValue_integer)
	{
		fatal("IntegerVariableClass_Modulo unexpected type: %d", v->inst.Value.choice);
		return;
	}

	val = GenericInteger_getInteger(&params->value, caller_gid);

#if (CLS_VERBOSE)
	verbose_class("IntegerVariableClass: %s; Modulo; %d %% %d", ExternalReference_name(&v->rootClass.inst.ref), v->inst.Value.u.integer, val);
#endif

	v->inst.Value.u.integer %= val;

	return;
}

void
IntegerVariableClass_setInteger(IntegerVariableClass *v, int val)
{
	/* assert */
	if(v->inst.Value.choice != OriginalValue_integer)
	{
		fatal("IntegerVariableClass_setInteger unexpected type: %d", v->inst.Value.choice);
		return;
	}

	v->inst.Value.u.integer = val;

	return;
}

void
OctetStringVariableClass_Clone(OctetStringVariableClass *t, Clone *params, OctetString *caller_gid)
{
#if (CLS_VERBOSE)
	verbose_class("OctetStringVariableClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	CLONE_OBJECT(t, VariableClass, octet_string_variable);

	return;
}

/*
 * caller_gid is used to resolve the GenericInteger or GenericOctetString
 */

void
OctetStringVariableClass_SetVariable(OctetStringVariableClass *v, NewVariableValue *new_value, OctetString *caller_gid)
{
	OctetString *oct;
	int val;
	char val_str[16];

#if (CLS_VERBOSE)
	verbose_class("OctetStringVariableClass: %s; SetVariable", ExternalReference_name(&v->rootClass.inst.ref));
#endif

	/* check the type of the new value */
	#if 0 // org
	if(new_value->choice != NewVariableValue_new_generic_integer
	&& new_value->choice != NewVariableValue_new_generic_octet_string)
	#else // hmkim : modified.
	if(new_value->choice != NewVariableValue_new_generic_integer
	&& new_value->choice != NewVariableValue_new_generic_octet_string
	&& new_value->choice != NewVariableValue_new_generic_content_reference)
	#endif
	{
		error("SetVariable: %s; type mismatch (%d)", ExternalReference_name(&v->rootClass.inst.ref), new_value->choice);
		return;
	}

	#if 0 // org
	if(new_value->choice == NewVariableValue_new_generic_octet_string)
	#else // hmkim : modified.
	if(new_value->choice == NewVariableValue_new_generic_octet_string || new_value->choice == NewVariableValue_new_generic_content_reference)
	#endif
	{
		oct = GenericOctetString_getOctetString(&new_value->u.new_generic_octet_string, caller_gid);
		OctetString_copy(&v->inst.Value.u.octetstring, oct);
	}
	else
	{
		/* convert the int to an OctetString */
		val = GenericInteger_getInteger(&new_value->u.new_generic_integer, caller_gid);
		snprintf(val_str, sizeof(val_str), "%d", val);
		/* free any existing data */
		safe_free(v->inst.Value.u.octetstring.data);
		v->inst.Value.u.octetstring.size = strlen(val_str);
		if(val_str && v->inst.Value.u.octetstring.size>0)
		{
			v->inst.Value.u.octetstring.data = safe_malloc(v->inst.Value.u.octetstring.size + 1);
			memset(v->inst.Value.u.octetstring.data, 0, v->inst.Value.u.octetstring.size + 1);
			strncpy((char *) v->inst.Value.u.octetstring.data, val_str, v->inst.Value.u.octetstring.size);
		}
	}

	return;
}

void
OctetStringVariableClass_TestVariable(OctetStringVariableClass *v, int op, OctetString *comp)
{
	EventData result;

	/* assert */
	if(v->inst.Value.choice != OriginalValue_octetstring)
	{
		fatal("OctetStringVariableClass_TestVariable unexpected type: %d", v->inst.Value.choice);
		return;
	}

	result.choice = EventData_boolean;

	switch(op)
	{
	case 1:
		/* equal */
		result.u.boolean = (OctetString_cmp(&v->inst.Value.u.octetstring, comp) == 0);
		break;

	case 2:
		/* not equal */
		result.u.boolean = (OctetString_cmp(&v->inst.Value.u.octetstring, comp) != 0);
		break;

	default:
		error("Unknown operator type: %d", op);
		return;
	}

	MHEGEngine_generateEvent(&v->rootClass.inst.ref, EventType_test_event, &result);

	return;
}

void
OctetStringVariableClass_Append(OctetStringVariableClass *v, Append *params, OctetString *caller_gid)
{
	OctetString *target;
	OctetString *append;

#if (CLS_VERBOSE)
	verbose_class("OctetStringVariableClass: %s; Append", ExternalReference_name(&v->rootClass.inst.ref));
#endif

	/* assert */
	if(v->inst.Value.choice != OriginalValue_octetstring)
	{
		fatal("OctetStringVariableClass_Append unexpected type: %d", v->inst.Value.choice);
		return;
	}

	target = &v->inst.Value.u.octetstring;
	append = GenericOctetString_getOctetString(&params->append_value, caller_gid);

#if (CLS_VERBOSE)
	verbose_class("Append: '%.*s' + '%.*s'", target->size, target->data, append->size, append->data);
#endif

	target->data = safe_realloc(target->data, target->size + append->size);
	VK_memcpy(&target->data[target->size], append->data, append->size);
	target->size += append->size;

	return;
}

/*
 * takes a copy of the source data
 */

void
OctetStringVariableClass_setOctetString(OctetStringVariableClass *v, OctetString *val)
{
	/* assert */
	if(v->inst.Value.choice != OriginalValue_octetstring)
	{
		fatal("OctetStringVariableClass_setOctetString unexpected type: %d", v->inst.Value.choice);
		return;
	}

	OctetString_copy(&v->inst.Value.u.octetstring, val);

	return;
}

void
ObjectRefVariableClass_Clone(ObjectRefVariableClass *t, Clone *params, OctetString *caller_gid)
{
#if (CLS_VERBOSE)
	verbose_class("ObjectRefVariableClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	CLONE_OBJECT(t, VariableClass, object_ref_variable);

	return;
}

/*
 * caller_gid is used to resolve the GenericObjectReference
 */

void
ObjectRefVariableClass_SetVariable(ObjectRefVariableClass *v, NewVariableValue *new_value, OctetString *caller_gid)
{
	ObjectReference *val;

#if (CLS_VERBOSE)
	verbose_class("ObjectRefVariableClass: %s; SetVariable", ExternalReference_name(&v->rootClass.inst.ref));
#endif

	/* check the type of the new value */
	if(new_value->choice != NewVariableValue_new_generic_object_reference)
	{
		error("SetVariable: %s; type mismatch (%d)", ExternalReference_name(&v->rootClass.inst.ref), new_value->choice);
		return;
	}

	val = GenericObjectReference_getObjectReference(&new_value->u.new_generic_object_reference, caller_gid);

	ObjectReference_copy(&v->inst.Value.u.object_reference, val);

	return;
}

/*
 * if comp does not have a group id, caller_gid is used
 */

void
ObjectRefVariableClass_TestVariable(ObjectRefVariableClass *v, int op, ObjectReference *comp, OctetString *caller_gid)
{
	ObjectReference *ref;
	OctetString *ref_gid;
	EventData result;

#if (CLS_VERBOSE)
	verbose_class("ObjectRefVariableClass: %s; TestVariable", ExternalReference_name(&v->rootClass.inst.ref));
#endif

	/* assert */
	if(v->inst.Value.choice != OriginalValue_object_reference)
	{
		fatal("ObjectRefVariableClass_TestVariable unexpected type: %d", v->inst.Value.choice);
		return;
	}

	/* the ObjectReference in this variable */
	ref = &v->inst.Value.u.object_reference;
	/* its group id (if it is an internal reference) */
	ref_gid = &v->rootClass.inst.ref.group_identifier;

	result.choice = EventData_boolean;

	switch(op)
	{
	case 1:
		/* equal */
		result.u.boolean = (ObjectReference_cmp(ref, ref_gid, comp, caller_gid) == 0);
		break;

	case 2:
		/* not equal */
		result.u.boolean = (ObjectReference_cmp(ref, ref_gid, comp, caller_gid) != 0);
		break;

	default:
		error("Unknown operator type: %d", op);
		return;
	}

	MHEGEngine_generateEvent(&v->rootClass.inst.ref, EventType_test_event, &result);

	return;
}

void
ObjectRefVariableClass_setObjectReference(ObjectRefVariableClass *v, ObjectReference *val)
{
	/* assert */
	if(v->inst.Value.choice != OriginalValue_object_reference)
	{
		fatal("ObjectRefVariableClass_setObjectReference unexpected type: %d", v->inst.Value.choice);
		return;
	}

	ObjectReference_copy(&v->inst.Value.u.object_reference, val);

	return;
}

void
ContentRefVariableClass_Clone(ContentRefVariableClass *t, Clone *params, OctetString *caller_gid)
{
#if (CLS_VERBOSE)
	verbose_class("ContentRefVariableClass: %s; Clone", ExternalReference_name(&t->rootClass.inst.ref));
#endif

	CLONE_OBJECT(t, VariableClass, content_ref_variable);

	return;
}

/*
 * caller_gid is used to resolve the GenericContentReference
 */

void
ContentRefVariableClass_SetVariable(ContentRefVariableClass *v, NewVariableValue *new_value, OctetString *caller_gid)
{
	ContentReference *val;

#if (CLS_VERBOSE)
	verbose_class("ContentRefVariableClass: %s; SetVariable", ExternalReference_name(&v->rootClass.inst.ref));
#endif

	/* check the type of the new value */
	if(new_value->choice != NewVariableValue_new_generic_content_reference)
	{
		error("SetVariable: %s; type mismatch (%d)", ExternalReference_name(&v->rootClass.inst.ref), new_value->choice);
		return;
	}

	val = GenericContentReference_getContentReference(&new_value->u.new_generic_content_reference, caller_gid);

	OctetString_copy(&v->inst.Value.u.content_reference, val);

	return;
}

void
ContentRefVariableClass_TestVariable(ContentRefVariableClass *v, int op, ContentReference *comp)
{
	EventData result;

#if (CLS_VERBOSE)
	verbose_class("ContentRefVariableClass: %s; TestVariable", ExternalReference_name(&v->rootClass.inst.ref));
#endif

	/* assert */
	if(v->inst.Value.choice != OriginalValue_content_reference)
	{
		fatal("ContentRefVariableClass_TestVariable unexpected type: %d", v->inst.Value.choice);
		return;
	}

	result.choice = EventData_boolean;

	switch(op)
	{
	case 1:
		/* equal */
		result.u.boolean = (OctetString_cmp(&v->inst.Value.u.content_reference, comp) == 0);
		break;

	case 2:
		/* not equal */
		result.u.boolean = (OctetString_cmp(&v->inst.Value.u.content_reference, comp) != 0);
		break;

	default:
		error("Unknown operator type: %d", op);
		return;
	}

	MHEGEngine_generateEvent(&v->rootClass.inst.ref, EventType_test_event, &result);

	return;
}

