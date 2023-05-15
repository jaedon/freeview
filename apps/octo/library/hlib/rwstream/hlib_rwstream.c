

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>

#include "_hlib_rwstream.h"

#if 1
#include <_hlib_log.h>
#include <_hlib_mem.h>
#include <_hlib_stdlib.h>

#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"RWSTREAM"

// TODO: temporary...
#define	RWSTREAM_ASSERT		HxLOG_Assert

#endif

#ifndef	RWSTREAM_BUFSIZE
#define	RWSTREAM_BUFSIZE	128
#endif

#ifndef	RWSTREAM_MALLOC
#define	RWSTREAM_MALLOC		HLIB_MEM_Malloc
#endif

#ifndef	RWSTREAM_FREE
#define	RWSTREAM_FREE		HLIB_MEM_Free
#endif

#ifndef	RWSTREAM_MALLOC_CB
#define	RWSTREAM_MALLOC_CB	HLIB_MEM_Malloc_CB
#endif

#ifndef	RWSTREAM_FREE_CB
#define	RWSTREAM_FREE_CB	HLIB_MEM_Free_CB
#endif

#ifndef	RWSTREAM_ASSERT
#define	RWSTREAM_ASSERT		assert
#endif

#ifndef	RWSTREAM_VSNPRINTF
	#define	RWSTREAM_VSNPRINTF	vsnprintf
#endif

#define	_____rwstream_type______________________________________________________________

struct rwstream
{
	void 	*buf;
	size_t	size;
	size_t	offset;
	size_t	written;

	void	*garbage;
	void *	(* _malloc) (size_t);
	void	(* _free) (void *);
};

#define	_____rwstream_private______________________________________________________________

static void *	__rwstream_realloc (struct rwstream *s, void *buf, size_t size, size_t extend)
{
	void *	newbuf;

	newbuf = s->_malloc(size + extend);
	if (newbuf == NULL)
		return NULL;

	memcpy(newbuf, buf, size);
	s->_free(buf);

	return newbuf;
}


static size_t	__rwstream_writeable (const struct rwstream *s)
{
	if (s->offset >= s->size)
		return 0;
	return s->size - s->offset;
}


static int	__rwstream_grow (struct rwstream *s, size_t wanted)
{
	void *	newbuf;

	if (s->garbage == NULL)
		return -1;

	if (/*wanted == 0 || */wanted < s->size)
		wanted = s->size;

	newbuf = __rwstream_realloc(s, s->buf, s->size, wanted);
	if (newbuf == NULL)
		return -1;
	s->buf   = s->garbage = newbuf;
	s->size += wanted;

	return 0;
}

#define	_____rwstream_public______________________________________________________________


int		rwstream_openex (void *buf, size_t size, void *(*memalloc)(size_t), void (*memfree)(void *))
{
	struct rwstream	*s;

	RWSTREAM_ASSERT(memalloc && memfree);

	s = (struct rwstream *)memalloc(sizeof(struct rwstream));
	if (s == NULL)
		return 0;
	memset(s, 0, sizeof(struct rwstream));

	if (buf == NULL)
	{
		if (size == 0)
			size = RWSTREAM_BUFSIZE;

		buf = memalloc(size);
		if (buf == NULL)
		{
			memfree(s);
			return 0;
		}
		else
		{
			((char *)buf)[0] = '\0';
		}

		s->garbage	= buf;
	}
	s->buf		= buf;
	s->size		= size;
	s->_malloc  = memalloc;
	s->_free    = memfree;

	return (int)s;
}


int		rwstream_open (void *buf, size_t size)
{
	return rwstream_openex(buf, size, RWSTREAM_MALLOC_CB, RWSTREAM_FREE_CB);
}

void	rwstream_close (int sd)
{
	struct rwstream *s = (struct rwstream *)sd;
	void 	(*memfree)(void *);

	RWSTREAM_ASSERT(s);

	memfree = s->_free;
	if (s->garbage)
	{
		memfree(s->garbage);
	}
	memfree(s);
}

void *	rwstream_close_without_buf (int sd)
{
	struct rwstream *s = (struct rwstream *)sd;
	void	*buf;
	void	(*memfree)(void *);

	RWSTREAM_ASSERT(s);

	buf = s->buf;
	memfree = s->_free;

	memfree(s);

	return buf;
}

void	rwstream_close_buf (void *buf)
{
	RWSTREAM_ASSERT(buf);
	RWSTREAM_FREE(buf);
}

size_t	rwstream_getbufsize (int sd)
{
	struct rwstream *s = (struct rwstream *)sd;

	RWSTREAM_ASSERT(s);

	return s->size;
}

size_t	rwstream_getoffset (int sd)
{
	struct rwstream *s = (struct rwstream *)sd;

	RWSTREAM_ASSERT(s);

	return s->offset;
}

size_t	rwstream_getwritten (int sd)
{
	struct rwstream *s = (struct rwstream *)sd;

	RWSTREAM_ASSERT(s);

	return s->written;
}

void *	rwstream_getbuf (int sd)
{
	struct rwstream *s = (struct rwstream *)sd;

	RWSTREAM_ASSERT(s);

	return s->buf;
}

size_t	rwstream_write (int sd, void *buf, size_t size)
{
	struct rwstream *s = (struct rwstream *)sd;

	RWSTREAM_ASSERT(s && buf && size);

	if (__rwstream_writeable(s) < size)
	{
		if (__rwstream_grow(s, size) < 0)
			return 0;
	}

	memcpy((char *)s->buf + s->offset, buf, size);
	s->offset  += size;
	if (s->offset > s->written)
		s->written = s->offset;

	return size;
}

size_t	rwstream_read (int sd, void *buf, size_t size)
{
	struct rwstream *s = (struct rwstream *)sd;
	size_t	readable;

	RWSTREAM_ASSERT(s && buf && size);

	if (s->written <= s->offset)
		return 0;

	readable = s->written - s->offset;
	if (readable < size)
		size = readable;

	memcpy(buf, (char *)s->buf + s->offset, size);
	s->offset  += size;

	return size;
}


int		rwstream_seek (int sd, size_t offset)
{
	struct rwstream *s = (struct rwstream *)sd;

	RWSTREAM_ASSERT(s);

	if (s->size < offset)
		return -1;

	s->offset = offset;
	return 0;
}

void	rwstream_rewind (int sd)
{
	struct rwstream *s = (struct rwstream *)sd;

	RWSTREAM_ASSERT(s);

	s->offset = 0;
}

void	rwstream_truncate (int sd)
{
	struct rwstream *s = (struct rwstream *)sd;

	RWSTREAM_ASSERT(s);

	s->offset = 0;
	s->written = 0;
}


int		rwstream_vprint (int sd, const char *format, va_list ap)
{
	struct rwstream *s = (struct rwstream *)sd;
	int 	ret;
	int		retry;
	size_t	size;

	RWSTREAM_ASSERT(s && format);

	for (retry = 0 ; retry < 10 ; retry++)
	{
		size = __rwstream_writeable(s);
		ret  = RWSTREAM_VSNPRINTF((char *)s->buf + s->offset, size, format, ap);
		if (ret >= 0 && ret < (int)size)
		{
			s->offset  += ret;
			if (s->offset > s->written)
				s->written = s->offset;

			return ret;
		}

		if (__rwstream_grow(s, (ret > 0) ? ret + 1/*'\0'*/ : 0) < 0)
			break;
	}
	return -1;
}

int		rwstream_print (int sd, const char *format, ...)
{
	int		ret;
	va_list	ap;

	va_start(ap, format);
	ret = rwstream_vprint(sd, format, ap);
	va_end(ap);

	return ret;
}

int		rwstream_putchar (int sd, int c)
{
	struct rwstream *s = (struct rwstream *)sd;
	char	*curr;

	RWSTREAM_ASSERT(s);

	if (__rwstream_writeable(s) < 2)
	{
		if (__rwstream_grow(s, 0) < 0)
			return -1;
	}

	curr = (char *)s->buf + s->offset++;
	*curr++ = (char)c;
	*curr++ = (char)'\0';

	if (s->offset > s->written)
		s->written = s->offset;

	return c;
}

int		rwstream_puts (int sd, const char *str)
{
	struct rwstream *s = (struct rwstream *)sd;
	size_t	size;

	RWSTREAM_ASSERT(s && str);

	size = strlen(str);
	if (rwstream_write(sd, (void *)str, size + 1) == 0)
		return -1;

	rwstream_seek(sd, s->offset - 1);
	return (int)size;
}

/*
void	printbuf(void *buf, size_t size)
{
	char *p = (char *)buf;

	while (size--)
	{
		printf("%02X ", *p++);
	}
	printf("\n");
}



int main (int argc, char *argv[])
{
	char	buf[10];
	int		sd;
	int		ret;

	sd = rwstream_open(buf, 10);

	ret = rwstream_print(sd, "ABCDE");
	printf("%4d:%s\n", ret, rwstream_getbuf(sd));
	ret = rwstream_print(sd, "abcde");
	printf("%4d:%s\n", ret, rwstream_getbuf(sd));

	printbuf(rwstream_getbuf(sd), 10);

	rwstream_close(sd);

	sd = rwstream_open(NULL, 10);

	ret = rwstream_print(sd, "ABCDE");
	printf("size:%4d, offset:%4d, ret=%4d:%s\n", rwstream_getbufsize(sd), rwstream_getoffset(sd), ret, rwstream_getbuf(sd));
	ret = rwstream_print(sd, "abcde");
	printf("size:%4d, offset:%4d, ret=%4d:%s\n", rwstream_getbufsize(sd), rwstream_getoffset(sd), ret, rwstream_getbuf(sd));

	ret = rwstream_print(sd, "123456789");
	printf("size:%4d, offset:%4d, ret=%4d:%s\n", rwstream_getbufsize(sd), rwstream_getoffset(sd), ret, rwstream_getbuf(sd));

	memset(buf, 0, 10);
	ret = rwstream_read(sd, buf, 9);
	printf("read>> offset:%4d, ret=%4d:%s\n", rwstream_getoffset(sd), ret, buf);

	rwstream_seek(sd, 5);
	memset(buf, 0, 10);
	ret = rwstream_read(sd, buf, 9);
	printf("read>> offset:%4d, ret=%4d:%s\n", rwstream_getoffset(sd), ret, buf);

	buf[0] = buf[1] = buf[2] = buf[3] = buf[4] = 'X';
	ret = rwstream_write(sd, buf, 5);

	rwstream_seek(sd, 15);
	memset(buf, 0, 10);
	ret = rwstream_read(sd, buf, 9);
	printf("read>> offset:%4d, ret=%4d:%s\n", rwstream_getoffset(sd), ret, buf);

	rwstream_close(sd);

	return 0;
}
*/

#define	_____HxRWSTREAM_PORT______________________________________________________________


HINT32		HLIB_RWSTREAM_Open (void *buf, HUINT32 size)
{
	return (HINT32)rwstream_open(buf, (size_t)size);
}

HINT32		HLIB_RWSTREAM_OpenEx (void *buf, HUINT32 size, void *(memalloc)(HUINT32), void (*memfree)(void *))
{
	return (HINT32)rwstream_openex(buf, (size_t)size, (void *)memalloc, (void *)memfree);
}

void		HLIB_RWSTREAM_Close (HINT32 sd)
{
	rwstream_close((int)sd);
}

void *		HLIB_RWSTREAM_CloseWithoutBuf (HINT32 sd)
{
	return rwstream_close_without_buf((int)sd);
}

void		HLIB_RWSTREAM_CloseBuf (void *buf)
{
	rwstream_close_buf(buf);
}

HUINT32		HLIB_RWSTREAM_GetBufSize (HINT32 sd)
{
	return (HUINT32)rwstream_getbufsize((int)sd);
}

HUINT32		HLIB_RWSTREAM_GetOffset (HINT32 sd)
{
	return (HUINT32)rwstream_getoffset((int)sd);
}

HUINT32		HLIB_RWSTREAM_GetWritten (HINT32 sd)
{
	return (HUINT32)rwstream_getwritten((int)sd);
}

void *		HLIB_RWSTREAM_GetBuf (HINT32 sd)
{
	return rwstream_getbuf((int)sd);
}

HUINT32		HLIB_RWSTREAM_Write (HINT32 sd, void *buf, HUINT32 size)
{
	return (HUINT32)rwstream_write((int)sd, buf, (size_t)size);
}

HUINT32		HLIB_RWSTREAM_Read (HINT32 sd, void *buf, HUINT32 size)
{
	return (HUINT32)rwstream_read((int)sd, buf, (size_t)size);
}

HINT32		HLIB_RWSTREAM_Seek (HINT32 sd, HUINT32 offset)
{
	return (HINT32)rwstream_seek((int)sd, (size_t)offset);
}

void		HLIB_RWSTREAM_Rewind (HINT32 sd)
{
	rwstream_rewind((int)sd);
}

void		HLIB_RWSTREAM_Truncate (HINT32 sd)
{
	rwstream_truncate((int)sd);
}

HINT32		HLIB_RWSTREAM_VPrint (HINT32 sd, const HCHAR *format, va_list ap)
{
	return rwstream_vprint((int)sd, (const char *)format, ap);
}

HINT32		HLIB_RWSTREAM_Print (HINT32 sd, const HCHAR *format, ...)
{
	int		ret;
	va_list	ap;

	va_start(ap, format);
	ret = rwstream_vprint((int)sd, (const char *)format, ap);
	va_end(ap);

	return (HINT32)ret;
}

HINT32		HLIB_RWSTREAM_PutChar (HINT32 sd, HINT32 c)
{
	return (HINT32)rwstream_putchar((int)sd, (int)c);
}

HINT32		HLIB_RWSTREAM_PutString (HINT32 sd, const HCHAR *str)
{
	return (HINT32)rwstream_puts((int)sd, (const char *)str);
}

void			HLIB_RWSTREAM_MemFree(void *p)
{
	HLIB_MEM_Free(p);
}

