/*
 * Reference.h
 */

#ifndef __REFERENCE_H__
#define __REFERENCE_H__

#include "ISO13522-MHEG-5.h"

int ObjectReference_cmp(ObjectReference *, OctetString *, ObjectReference *, OctetString *);
char *ObjectReference_name(ObjectReference *);

int ExternalReference_cmp(ExternalReference *, ExternalReference *);
char *ExternalReference_name(ExternalReference *);

#endif	/* __REFERENCE_H__ */
