/*
 * Generic.h
 */

#ifndef __GENERIC_H__
#define __GENERIC_H__

#include "ISO13522-MHEG-5.h"

bool GenericBoolean_getBoolean(GenericBoolean *, OctetString *);
void GenericBoolean_setBoolean(GenericBoolean *, OctetString *, bool);

int GenericInteger_getInteger(GenericInteger *, OctetString *);
void GenericInteger_setInteger(GenericInteger *, OctetString *, int);

OctetString *GenericOctetString_getOctetString(GenericOctetString *, OctetString *);
void GenericOctetString_setOctetString(GenericOctetString *, OctetString *, OctetString *);

ObjectReference *GenericObjectReference_getObjectReference(GenericObjectReference *, OctetString *);
void GenericObjectReference_setObjectReference(GenericObjectReference *, OctetString *, ObjectReference *);

ContentReference *GenericContentReference_getContentReference(GenericContentReference *, OctetString *);
void GenericContentReference_setContentReference(GenericContentReference *, OctetString *, ContentReference *);

#endif	/* __GENERIC_H__ */

