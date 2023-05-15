/*************************************************************
 * @file		hxpriorityqueue.h
 * @date		2012/12/20
 * http://www.humaxdigital.com
 *************************************************************/

#define	____HxPRIORITYQUEUE_README___________________

/******************************************************************************
 [Example Code]

 [Expected Result]


 ************************************************************************************/

#ifndef __HLIB_PRIORITYQUEUE_H__
#define __HLIB_PRIORITYQUEUE_H__

#include "_hlib_types.h"

#ifdef	__cplusplus
extern	"C" {
#endif


typedef struct hxpriorityqueue	HxPriorityQueue_t;

#define		HxPRIORITYQUEUE_IsEmpty(queue)	(HLIB_PRIORITYQUEUE_Length(queue) ? FALSE : TRUE)

HxPriorityQueue_t *	_HLIB_PRIORITYQUEUE_New (HxCompareFunc_t comp, HxFreeFunc_t _free, const HCHAR * file, HUINT32 line);

#define		HLIB_PRIORITYQUEUE_New(comp,_free)	_HLIB_PRIORITYQUEUE_New (comp, _free, __FILE__, __LINE__);

void		HLIB_PRIORITYQUEUE_Delete (HxPriorityQueue_t *queue);
HUINT32		HLIB_PRIORITYQUEUE_Length (HxPriorityQueue_t *queue);
void		HLIB_PRIORITYQUEUE_Clear (HxPriorityQueue_t *queue);
void *		HLIB_PRIORITYQUEUE_Front (HxPriorityQueue_t *queue);
void *		HLIB_PRIORITYQUEUE_Back (HxPriorityQueue_t *queue);
void *		HLIB_PRIORITYQUEUE_Pop (HxPriorityQueue_t *queue);
HBOOL		HLIB_PRIORITYQUEUE_PopEx (HxPriorityQueue_t *queue, void **element);
HBOOL		HLIB_PRIORITYQUEUE_Push (HxPriorityQueue_t *queue, void *element);

#ifdef	__cplusplus
}
#endif

#endif /* __HLIB_PRIORITYQUEUE_H__ */
