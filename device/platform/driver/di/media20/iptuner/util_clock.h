#ifndef __UTIL_CLOCK_H__
#define __UTIL_CLOCK_H__

#include <sys/time.h>
#include "htype.h"

HULONG UTIL_GetClockMsec (void);

HULONG UTIL_GetClock (void);
long UTIL_GetTimevalDiff(const struct timeval *current, const struct timeval *past);
void UTIL_GetCurrentTimeval(struct timeval *time);

#endif /* !__UTIL_CLOCK_H__ */
