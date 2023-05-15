/*
 * VariableClass.h
 */

#ifndef __VARIABLECLASS_H__
#define __VARIABLECLASS_H__

#include "ISO13522-MHEG-5.h"

void VariableClass_Preparation(VariableClass *);
void VariableClass_Activation(VariableClass *);
void VariableClass_Deactivation(VariableClass *);
void VariableClass_Destruction(VariableClass *);

void VariableClass_Clone(VariableClass *, Clone *, OctetString *);
void VariableClass_SetVariable(VariableClass *, NewVariableValue *, OctetString *);
void VariableClass_TestVariable(VariableClass *, int, ComparisonValue *, OctetString *);

unsigned int VariableClass_type(VariableClass *);
char *VariableClass_stringValue(VariableClass *);

/*
 * BooleanVariableClass.h
 */

void BooleanVariableClass_Clone(BooleanVariableClass *, Clone *, OctetString *);
void BooleanVariableClass_SetVariable(BooleanVariableClass *, NewVariableValue *, OctetString *);
void BooleanVariableClass_TestVariable(BooleanVariableClass *, int, bool);
void BooleanVariableClass_setBoolean(BooleanVariableClass *, bool);

/*
 * IntegerVariableClass.h
 */

void IntegerVariableClass_Clone(IntegerVariableClass *, Clone *, OctetString *);
void IntegerVariableClass_SetVariable(IntegerVariableClass *, NewVariableValue *, OctetString *);
void IntegerVariableClass_TestVariable(IntegerVariableClass *, int, int);
void IntegerVariableClass_Add(IntegerVariableClass *, Add *, OctetString *);
void IntegerVariableClass_Subtract(IntegerVariableClass *, Subtract *, OctetString *);
void IntegerVariableClass_Multiply(IntegerVariableClass *, Multiply *, OctetString *);
void IntegerVariableClass_Divide(IntegerVariableClass *, Divide *, OctetString *);
void IntegerVariableClass_Modulo(IntegerVariableClass *, Modulo *, OctetString *);
void IntegerVariableClass_setInteger(IntegerVariableClass *, int);

/*
 * OctetStringVariableClass.h
 */

void OctetStringVariableClass_Clone(OctetStringVariableClass *, Clone *, OctetString *);
void OctetStringVariableClass_SetVariable(OctetStringVariableClass *, NewVariableValue *, OctetString *);
void OctetStringVariableClass_TestVariable(OctetStringVariableClass *, int, OctetString *);
void OctetStringVariableClass_Append(OctetStringVariableClass *, Append *, OctetString *);
void OctetStringVariableClass_setOctetString(OctetStringVariableClass *, OctetString *);

/*
 * ObjectRefVariableClass.h
 */

void ObjectRefVariableClass_Clone(ObjectRefVariableClass *, Clone *, OctetString *);
void ObjectRefVariableClass_SetVariable(ObjectRefVariableClass *, NewVariableValue *, OctetString *);
void ObjectRefVariableClass_TestVariable(ObjectRefVariableClass *, int, ObjectReference *, OctetString *);
void ObjectRefVariableClass_setObjectReference(ObjectRefVariableClass *, ObjectReference *);

/*
 * ContentRefVariableClass.h
 */

void ContentRefVariableClass_Clone(ContentRefVariableClass *, Clone *, OctetString *);
void ContentRefVariableClass_SetVariable(ContentRefVariableClass *, NewVariableValue *, OctetString *);
void ContentRefVariableClass_TestVariable(ContentRefVariableClass *, int, ContentReference *);

#endif	/* __VARIABLECLASS_H__ */

