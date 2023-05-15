/*
 * utils.c
 */

// hmkim : added.
#include "../ci_ammi.h"
#ifdef _CI_DEBUG_AMMI_DECODE_FILE_DATA_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "utils.h"

#if 0 // hmkim : commented out.

void
write_all(FILE *out, unsigned char *buf, size_t len)
{
	if(fwrite(buf, 1, len, out) != len)
		fatal("Error writing to file: %s", strerror(errno));

	return;
}

/*
 * returns 15 for 'f' etc
 */

unsigned int
char2hex(unsigned char c)
{
	if(!isxdigit(c))
		return 0;
	else if(c >= '0' && c <= '9')
		return c - '0';
	else
		return 10 + (tolower(c) - 'a');
}

/*
 * I don't want to double the size of my code just to deal with malloc failures
 * if you've run out of memory you're fscked anyway, me trying to recover is not gonna help...
 */

void *
safe_malloc(size_t nbytes)
{
	void *buf;

	if((buf = HLIB_STD_MemAlloc(nbytes)) == NULL)
		fatal("Out of memory");

	return buf;
}

/*
 * safe_realloc(NULL, n) == safe_malloc(n)
 */

void *
safe_realloc(void *oldbuf, size_t nbytes)
{
	void *newbuf;

	if(oldbuf == NULL)
		return safe_malloc(nbytes);

	if((newbuf = HLIB_STD_MemRealloc(oldbuf, nbytes)) == NULL)
		fatal("Out of memory");

	return newbuf;
}

/*
 * safe_free(NULL) is okay
 */

void
safe_free(void *buf)
{
	if(buf != NULL)
		HLIB_STD_MemFree(buf);

	return;
}

#endif

#if 0 // org

void
error(char *message, ...)
{
	va_list ap;

	va_start(ap, message);
	vfprintf(stderr, message, ap);
	fprintf(stderr, "\n");
	va_end(ap);

	return;
}

void
fatal(char *message, ...)
{
	va_list ap;

	va_start(ap, message);
	vfprintf(stderr, message, ap);
	fprintf(stderr, "\n");
	va_end(ap);

	exit(EXIT_FAILURE);
}

#else // hmkim : modified.

void
error_at_(char *fn, int line, char *message, ...)
{
	va_list ap;

	va_start(ap, message);
	vfprintf(stderr, message, ap);
	fprintf(stderr, " at %s() %d line !!!\n", fn, line);
	va_end(ap);

	return;
}

void
fatal_at_(char *fn, int line, char *message, ...)
{
	va_list ap;

	va_start(ap, message);
	vfprintf(stderr, message, ap);
	fprintf(stderr, " at %s() %d line (fatal) !!!\n", fn, line);
	va_end(ap);

	exit(EXIT_FAILURE);
}

#endif

#if (DER_VERBOSE_VALUE) // hmkim : added.

void
verbose_at_(char *fn, int line, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "%s() %d line : ", fn, line);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	return;
}

void
vverbose_at_(char *fn, int line, const char *fmt, ...)
{
	va_list ap;

	if(DER_VERBOSE_VALUE > 1)
	{
		va_start(ap, fmt);
		fprintf(stderr, "%s() %d line : ", fn, line);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}

	return;
}

#endif

#if 0 // hmkim : commented out.

/* number of bytes per line */
#define HEXDUMP_WIDTH	16

void
hexdump(FILE *out, unsigned char *data, size_t nbytes)
{
	size_t nout;
	int i;

	nout = 0;
	while(nout < nbytes)
	{
		/* byte offset at start of line */
		if((nout % HEXDUMP_WIDTH) == 0)
			fprintf(out, "0x%.8x  ", nout);
		/* the byte value in hex */
		fprintf(out, "%.2x ", data[nout]);
		/* the ascii equivalent at the end of the line */
		if((nout % HEXDUMP_WIDTH) == (HEXDUMP_WIDTH - 1))
		{
			fprintf(out, " ");
			for(i=HEXDUMP_WIDTH-1; i>=0; i--)
				fprintf(out, "%c", isprint(data[nout - i]) ? data[nout - i] : '.');
			fprintf(out, "\n");
		}
		nout ++;
	}

	/* the ascii equivalent if we haven't just done it */
	if((nout % HEXDUMP_WIDTH) != 0)
	{
		/* pad to the start of the ascii equivalent */
		for(i=(nout % HEXDUMP_WIDTH); i<HEXDUMP_WIDTH; i++)
			fprintf(out, "   ");
		fprintf(out, " ");
		/* print the ascii equivalent */
		nout --;
		for(i=(nout % HEXDUMP_WIDTH); i>=0; i--)
			fprintf(out, "%c", isprint(data[nout - i]) ? data[nout - i] : '.');
		fprintf(out, "\n");
	}

	return;
}

#endif

#endif // hmkim : added.
