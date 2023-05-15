/*
 * Content.h
 */

#ifndef __CONTENT_H__
#define __CONTENT_H__

#if !defined(WIN32)
#include <stdbool.h>
#endif

#include "ISO13522-MHEG-5.h"

bool NewContent_getContent(NewContent *, OctetString *, RootClass *, OctetString *);

bool ContentBody_getContent(ContentBody *, RootClass *, OctetString *);
OctetString *ContentBody_getReference(ContentBody *);

#endif	/* __CONTENT_H__ */

