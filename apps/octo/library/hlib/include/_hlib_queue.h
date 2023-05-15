/*************************************************************
 * @file		hxqueue.h
 * @date		2012/08/27
 * @author	Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header, stream reader & writer
 * http://www.humaxdigital.com
 *************************************************************/

#define	____HXQUEUE_README___________________

/******************************************************************************
 [Example Code]

 [Expected Result]


 ************************************************************************************/

#ifndef __HxQUEUE_H__
#define __HxQUEUE_H__

#include "htype.h"

#ifdef	__cplusplus
extern	"C" {
#endif


typedef struct hxqueue	HxQueue_t;

typedef void	(* HxQueue_RemoveCallback_t) (void *element);

HxQueue_t *	_HLIB_QUEUE_New (HxQueue_RemoveCallback_t removeNotify, const char* file, const int line);
HxQueue_t *	_HLIB_QUEUE_NewEx (HUINT32 length, HBOOL dynamic, HxQueue_RemoveCallback_t removeNotify, const char* file, const int line);

#define		HLIB_QUEUE_New(removeNotify) _HLIB_QUEUE_New(removeNotify, __FILE__, __LINE__)
#define		HLIB_QUEUE_NewEx(length, dynamic, removeNotify) _HLIB_QUEUE_NewEx(length, dynamic, removeNotify, __FILE__, __LINE__)
void		HLIB_QUEUE_Delete (HxQueue_t *q);
HUINT32		HLIB_QUEUE_Length (HxQueue_t *q);
#define		HLIB_QUEUE_IsEmpty(q)	(HLIB_QUEUE_Length(q) ? FALSE : TRUE)
void		HLIB_QUEUE_Clear (HxQueue_t *q);
void *		HLIB_QUEUE_Front (HxQueue_t *q);
void *		HLIB_QUEUE_Back (HxQueue_t *q);
void *		HLIB_QUEUE_Pop (HxQueue_t *q);
HBOOL		HLIB_QUEUE_PopEx (HxQueue_t *q, void **element);
HBOOL		HLIB_QUEUE_Push (HxQueue_t *q, void *element);

#ifdef	__cplusplus
}
#endif

#endif /* __HxQUEUE_H__ */
