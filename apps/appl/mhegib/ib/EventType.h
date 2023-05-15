/*
 * EventType.h
 */

#ifndef __EVENTTYPE_H__
#define __EVENTTYPE_H__

#if !defined(WIN32)
#include <stdbool.h>
#endif

#include "ISO13522-MHEG-5.h"

bool EventType_isAsync(EventType);
char *EventType_name(EventType);

#endif	/* __EVENTTYPE_H__ */
