/*
 * der_decode.h
 */

#ifndef __DER_DECODE_H__
#define __DER_DECODE_H__

// hmkim : added.
#include "../ci_ammi.h"
#ifdef _CI_DEBUG_AMMI_DECODE_FILE_DATA_

#include <stdio.h>
#include <stdarg.h>

#include "utils.h"

#if 0 // hmkim : commented out.
#define der_alloc(N)		safe_malloc(N)
#define der_realloc(P, N)	safe_realloc(P, N)
#define der_free(P)		safe_free(P)
#endif

struct der_tag
{
	unsigned char class;
	unsigned int number;
	unsigned int length;
};

#define DER_CLASS_UNIVERSAL	0x00
#define DER_CLASS_APPLICATION	0x40
#define DER_CLASS_CONTEXT	0x80
#define DER_CLASS_PRIVATE	0xc0

/* top 8 bits are class, bottom 24 are the tag number */
#define MATCH_TAGCLASS(CLASS, NUMBER, TAGCLASS)	((CLASS == ((TAGCLASS >> 24) & 0xff)) && (NUMBER == (TAGCLASS & 0xffffff)))

#if 0
int der_decode_Tag(FILE *, struct der_tag *);
#else // hmkim : modified.
int der_decode_Tag_(FILE *der, struct der_tag *tag);
#define der_decode_Tag(der,tag) der_decode_Tag_(der,tag)
#endif

int der_decode_BOOLEAN(FILE *, FILE *, int);
int der_decode_INTEGER(FILE *, FILE *, int);
#if 0
int der_decode_OctetString(FILE *, FILE *, int);
int der_decode_Null(FILE *, FILE *, int);
#else // hmkim : modified.
int der_decode_OctetString_(FILE *der, FILE *out, int length);
#define der_decode_OctetString(der,out,length) der_decode_OctetString_(der,out,length)
int der_decode_Null_(FILE *der, FILE *out, int length);
#define der_decode_Null(der,out,length) der_decode_Null_(der,out,length)
#endif
int der_decode_ENUMERATED(FILE *, FILE *, int, unsigned int, char **);

int get_der_int(FILE *, int);

#if 0
int der_read_file(FILE *, unsigned int, void *);
#else // hmkim : modified.
int der_read_file_(FILE *in, unsigned int nbytes, void *buf);
#define der_read_file(in,nbytes,buf) der_read_file_(in,nbytes,buf)
#endif

#if 0
int der_error(char *, ...);
#else // hmkim : modified.
int der_error_at_(char *fn, int line, char *fmt, ...);
#define der_error(fmt, ...) der_error_at_((char*)__FUNCTION__, __LINE__, fmt, ## __VA_ARGS__)
#endif

#endif // hmkim : added.

#endif	/* __DER_DECODE_H__ */

