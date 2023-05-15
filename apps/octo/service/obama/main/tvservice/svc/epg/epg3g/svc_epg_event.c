

//#include "../local_include/_svc_epg.h"
//#include "../local_include/_svc_epg_event.h"

#include <svc_epg.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	GOTO_IF(expr, pos)		do{if(expr)goto pos;}while(0)

/*******************************************************************/
/********************     Functions         *************************/
/*******************************************************************/

#define	_____epg_event_APIs______________________________________________________________

SvcEpg_EventInfo_t *	svc_epgevent_New (const SvcEpg_EventClass_t *clazz, SvcEpg_Type_b bwType, ...)
{
	SvcEpg_EventInfo_t	*self;
	SvcEpg_EventInfo_t	*ret;
	va_list			ap;

	HxLOG_Trace();
	HxLOG_Assert(clazz);

	self = (SvcEpg_EventInfo_t *)HLIB_STD_MemCalloc(clazz->ulSize);
	if (self)
	{
		self->clazz    = clazz;
		self->refCount = 1;
		self->type     = bwType;

		va_start(ap, bwType);
		ret = clazz->fnCtor(self, ap);
		va_end(ap);

		GOTO_IF(ret == NULL, ERROR);
	}
	return self;
ERROR:
	if (self) HLIB_STD_MemFree(self);
	return NULL;
}

void			svc_epgevent_Delete (SvcEpg_EventInfo_t *self)
{
	if(self == NULL)
	{
		return ;
	}

	HxLOG_Assert(self->refCount > 0);

	if (--self->refCount == 0)
	{
		self->clazz->fnDtor(self);
		HLIB_STD_MemFree(self);
	}
}

