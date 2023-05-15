/*************************************************************
 * @file		hxmem.h
 * @date		2012/09/25
 * @author	jwlee2 (jwlee2@humaxdigital.com)
 * @brief		HUMAX common library header, fixed size blocks allocation memory
 * http://www.humaxdigital.com
 *************************************************************/

#ifndef	__HX_ZLIB_H__
#define	__HX_ZLIB_H__
#include <htype.h>

HERROR		HLIB_ZLIB_Uncompress(HUINT8 *pucDest, HUINT32 *pulDestLen, HUINT8 *pucSource, HUINT32 ulSourceLen);
HERROR		HLIB_ZLIB_Compress(HUINT8 *pucDest, HUINT32 *pulDestLen, HUINT8 *pucSource, HUINT32 ulSourceLen);
HERROR 		HLIB_GZ_Uncompress(HUINT8 *infilename, HUINT8 *outfilename);
#endif /* !__PALZLIB_H__ */
