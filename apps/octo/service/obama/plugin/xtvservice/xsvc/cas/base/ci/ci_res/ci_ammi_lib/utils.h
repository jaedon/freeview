/*
 * utils.h
 */

#ifndef __UTILS_H__
#define __UTILS_H__

// hmkim : added.
#include "../ci_ammi.h"
#ifdef _CI_DEBUG_AMMI_DECODE_FILE_DATA_

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#if 0 // hmkim : commented out.

void write_all(FILE *, unsigned char *, size_t);

unsigned int char2hex(unsigned char);

void *safe_malloc(size_t);
void *safe_realloc(void *, size_t);
void safe_free(void *);

void hexdump(FILE *, unsigned char *, size_t);

#endif

#if 0

void error(char *, ...);
void fatal(char *, ...);

#else // hmkim : modified.

void error_at_(char *fn, int line, char *message, ...);
#define error(message, ...) error_at_((char*)__FUNCTION__, __LINE__, message, ## __VA_ARGS__)
void fatal_at_(char *fn, int line, char *message, ...);
#define fatal(message, ...) fatal_at_((char*)__FUNCTION__, __LINE__, message, ## __VA_ARGS__)

#endif

#if 1 // hmkim : added.

#define DER_VERBOSE_VALUE  0

#if (DER_VERBOSE_VALUE)
void verbose_at_(char *fn, int line, const char *fmt, ...);
#define verbose(fmt, ...) verbose_at_((char*)__FUNCTION__, __LINE__, fmt, ## __VA_ARGS__)
void vverbose_at_(char *fn, int line, const char *fmt, ...);
#define vverbose(fmt, ...) vverbose_at_((char*)__FUNCTION__, __LINE__, fmt, ## __VA_ARGS__)
#else
#define verbose(fmt, ...) ((void)0)
#define vverbose(fmt, ...) ((void)0)
#endif

#endif

#endif // hmkim : added.

#endif	/* __UTILS_H__ */

