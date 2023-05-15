/*
 *
 *	Utils for Videobackend
 *
 *	Written by Jongwon Kim
 *
 */

#include "videobackend_utils.h"

#include <sys/time.h>
#include <time.h>



unsigned long vbutils_get_ticks()
{
	struct timeval tv;
	gettimeofday( &tv, 0 );
	return tv.tv_sec*1000 + (tv.tv_usec+500)/1000;
}

unsigned long vbutils_get_diff(unsigned long tm1, unsigned long tm2)
{
	return tm2>=tm1 ? tm2-tm1 : 0xFFFFFFFFUL-(tm1-tm2);
}

