/*************************************************************
 * @file		hxrwstream.h
 * @date		2011/06/24
 * @author	Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header, stream reader & writer
 * http://www.humaxdigital.com
 *************************************************************/

#define	____HXRWSTREAM_README___________________

/******************************************************************************
 [Example Code]

int main (int argc, char *argv[])
{
	HCHAR	buf[10];
	HINT32	sd;
	HINT32	ret;

	sd = HLIB_RWSTREAM_Open(NULL, 10);

	ret = HLIB_RWSTREAM_Print(sd, "ABCDE");
	printf("size:%4d, offset:%4d, ret=%4d:%s\n", HLIB_RWSTREAM_GetBufSize(sd), HLIB_RWSTREAM_GetOffset(sd), ret, HLIB_RWSTREAM_GetBuf(sd));
	ret = HLIB_RWSTREAM_Print(sd, "abcde");
	printf("size:%4d, offset:%4d, ret=%4d:%s\n", HLIB_RWSTREAM_GetBufSize(sd), HLIB_RWSTREAM_GetOffset(sd), ret, HLIB_RWSTREAM_GetBuf(sd));

	ret = HLIB_RWSTREAM_Print(sd, "123456789");
	printf("size:%4d, offset:%4d, ret=%4d:%s\n", HLIB_RWSTREAM_GetBufSize(sd), HLIB_RWSTREAM_GetOffset(sd), ret, HLIB_RWSTREAM_GetBuf(sd));

	memset(buf, 0, 10);
	ret = HLIB_RWSTREAM_Read(sd, buf, 9);
	printf("read>> offset:%4d, ret=%4d:%s\n", HLIB_RWSTREAM_GetOffset(sd), ret, buf);

	HLIB_RWSTREAM_Seek(sd, 5);
	memset(buf, 0, 10);
	ret = HLIB_RWSTREAM_Read(sd, buf, 9);
	printf("read>> offset:%4d, ret=%4d:%s\n", HLIB_RWSTREAM_GetOffset(sd), ret, buf);

	buf[0] = buf[1] = buf[2] = buf[3] = buf[4] = 'X';
	ret = HLIB_RWSTREAM_Write(sd, buf, 5);

	HLIB_RWSTREAM_Seek(sd, 15);
	memset(buf, 0, 10);
	ret = HLIB_RWSTREAM_Read(sd, buf, 9);
	printf("read>> offset:%4d, ret=%4d:%s\n", HLIB_RWSTREAM_GetOffset(sd), ret, buf);

	HLIB_RWSTREAM_Close(sd);

	return 0;
}


 [Expected Result]
size:  10, offset:   5, ret=   5:ABCDE
size: 138, offset:  10, ret=   5:ABCDEabcde
size: 138, offset:  19, ret=   9:ABCDEabcde123456789
read>> offset:  19, ret=   0:
read>> offset:  14, ret=   9:abcde1234
read>> offset:  24, ret=   9:XXXX

 ************************************************************************************/

#ifndef __HLIB_RWSTREAM_H__
#define __HLIB_RWSTREAM_H__

//#include <htype.h>
#include "htype.h"

#include <stdarg.h>

/**
 * Open byte (RW)stream
 * 1. static stream
 *    - buf와 size를 open시에 넣어주면 해당 공간내에서 stream read/write한다.
 * 2. dynamic stream
 *    - buf와 size를 open시에 넣어주지 않으면, buf가 자동으로 증가되면서, stream read/write한다.
 *
 * @param	buf		user buffer (if NULL, 내부적으로 버퍼 할당됨)
 * @param	size	buffer size
 * @return	rwstream descriptor
 */
HINT32		HLIB_RWSTREAM_Open (void *buf, HUINT32 size);
HINT32		HLIB_RWSTREAM_OpenEx (void *buf, HUINT32 size, void *(memalloc)(HUINT32), void (*memfree)(void *));

/**
 * Close stream
 *
 * @param	sd		(rw)stream descriptor
 */
void		HLIB_RWSTREAM_Close (HINT32 sd);

/**
 * Close stream without Buffer
 *
 * 1.  static stream: buf: user buffer
 * 2. dynamic stream: buf: allocated buffer (must be free[HLIB_RWSTREAM_CloseBuf()])
 *
 * @param	sd		(rw)stream descriptor
 * @return	buf		stream buffer
 */
void *		HLIB_RWSTREAM_CloseWithoutBuf (HINT32 sd);

/**
 * Close dynamic allocated buffer
 *
 * @param	buf		buffer
 */
void		HLIB_RWSTREAM_CloseBuf (void *buf);

/**
 * get buffer memory size
 *
 * @param	sd		(rw)stream descriptor
 * @return	size of buffer
 */
HUINT32		HLIB_RWSTREAM_GetBufSize (HINT32 sd);

/**
 * get current offset of stream
 *
 * @param	sd		(rw)stream descriptor
 * @return	offset
 */
HUINT32		HLIB_RWSTREAM_GetOffset (HINT32 sd);

/**
 * get number of written bytes
 *
 * @param	sd		(rw)stream descriptor
 * @return	written
 */
HUINT32		HLIB_RWSTREAM_GetWritten (HINT32 sd);

/**
 * get buffer (not copy)
 *
 * @param	sd		(rw)stream descriptor
 * @return	buffer
 */
void *		HLIB_RWSTREAM_GetBuf (HINT32 sd);

/**
 * write 'buf' into stream
 *
 * @param	sd		(rw)stream descriptor
 * @param	buf		data
 * @param	size	size of buf
 * @return	number of items successfully written (if an error occurs, the return valus is zero)
 */
HUINT32		HLIB_RWSTREAM_Write (HINT32 sd, void *buf, HUINT32 size);

/**
 * read data from stream
 *
 * @param	sd		(rw)stream descriptor
 * @param	buf		data
 * @param	size	size of buf
 * @return	number of items successfully read (if an error occurs, the return valus is zero)
 */
HUINT32		HLIB_RWSTREAM_Read (HINT32 sd, void *buf, HUINT32 size);

/**
 * set the stream position indicator(offset)
 *
 * @param	sd		(rw)stream descriptor
 * @param	offset	new offset
 * @return	-1 (error), 0 (success)
 */
HINT32		HLIB_RWSTREAM_Seek (HINT32 sd, HUINT32 offset);

/**
 * set the stream position indicator(offset) to the beginning of the stream.
 *
 * @param	sd		(rw)stream descriptor
 */
void		HLIB_RWSTREAM_Rewind (HINT32 sd);
void		HLIB_RWSTREAM_Truncate (HINT32 sd);

/**
 * write 'formatted text' output to 'sd'stream.
 *
 * @param	sd		(rw)stream descriptor
 * @param	format	same scheme as printf()
 * @return	The number of characters written. (not including '\0') if error, a negative value is returned.
 */
HINT32		HLIB_RWSTREAM_Print (HINT32 sd, const HCHAR *format, ...);
HINT32		HLIB_RWSTREAM_VPrint (HINT32 sd, const HCHAR *format, va_list ap);

/**
 * Put the character 'c' to stream
 *
 * @param	sd		(rw)stream descriptor
 * @param	c		character
 * @return	the written character or negative value on error.
 */
HINT32		HLIB_RWSTREAM_PutChar (HINT32 sd, HINT32 c);

HINT32		HLIB_RWSTREAM_PutString (HINT32 sd, const HCHAR *str);

void		HLIB_RWSTREAM_MemFree(void *p);

#endif /* __HLIB_RWSTREAM_H__ */
