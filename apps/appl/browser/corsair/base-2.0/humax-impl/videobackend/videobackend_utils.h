/*
 *
 *	Utils for Videobackend
 *
 *	Written by Jongwon Kim
 *
 */

#ifndef __VIDEOBACKEND_UITLS_H__
#define __VIDEOBACKEND_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

unsigned long vbutils_get_ticks();

unsigned long vbutils_get_diff(unsigned long tm1, unsigned long tm2);

#ifdef __cplusplus
};
#endif

#endif
