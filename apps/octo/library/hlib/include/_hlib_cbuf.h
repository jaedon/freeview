/*************************************************************
 * @file		hxcbuf.h
 * @date		2012/03/13
 * @author		someone
 * @brief		HLIB UserDB API

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef	_HLIB_CBUF_H_
#define	_HLIB_CBUF_H_

typedef	struct _HxCBUF_t*		HxCBUF_t;

/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */
extern HxCBUF_t		HLIB_CBUF_New(HINT32 nSize);
extern HERROR		HLIB_CBUF_Delete(HxCBUF_t hBufHandle);
extern HCHAR*		HLIB_CBUF_GetBuffer(HxCBUF_t hBufHandle, HINT32 nSize);


#endif /* _HLIB_CBUF_H_ */

