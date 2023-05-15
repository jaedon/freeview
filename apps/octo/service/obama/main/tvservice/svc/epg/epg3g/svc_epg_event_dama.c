
#include <dlib.h>

#include <octo_common.h>

#include "../local_include/_svc_epg.h"
#include "../local_include/_svc_epg_event.h"
#include "../local_include/_svc_epg_event_dama.h"

#include <svc_epg.h>



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	OxEPG_DEVENT(e)	(((SvcEpg_EventDama_t *)(e))->event)

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	SvcEpg_EventInfo_t	super;	///< super class
	DxEvent_t		*event;
} SvcEpg_EventDama_t;

/*******************************************************************/
/********************      Functions         *************************/
/*******************************************************************/

#define	_____LOCAL_FUCN___________

STATIC SvcEpg_Type_b	svc_epgeventdama_ToOxEPG_Type (DxEPG_Type_e bwType)
{
	switch (bwType)
	{
	case eDxEPG_TYPE_PF:	return eOxEPG_TYPE_PF;
	case eDxEPG_TYPE_SC:	return eOxEPG_TYPE_SC;
	case eDxEPG_TYPE_PF_EX:	return eOxEPG_TYPE_PF_EX;
	case eDxEPG_TYPE_SC_EX:	return eOxEPG_TYPE_SC_EX;
	case eDxEPG_TYPE_TVTV:	return eOxEPG_TYPE_TVTV;
	default:
		break;
	}
	return eOxEPG_TYPE_NONE;
}

STATIC SvcEpg_EventInfo_t *	svc_epgeventdama_Constructor (SvcEpg_EventInfo_t *self, va_list ap)
{
	SvcEpg_EventDama_t *thiz;
	DxEvent_t *devt;

	thiz = (SvcEpg_EventDama_t *)self;
	devt = va_arg(ap, DxEvent_t *);
	thiz->event = devt;

	self->uid       = devt->uid;
#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	self->svcuid    = devt->svcuid;
#endif
	self->eventid   = devt->eventId;
	self->startTime = devt->startTime;
	self->duration  = devt->duration;
	self->runstatus	= devt->runStatus;
	self->version   = 0;

	return self;
}

STATIC void	svc_epgeventdama_Destructor (SvcEpg_EventInfo_t *self)
{
	DxEvent_t *devt = OxEPG_DEVENT(self);

	if (devt)
		DLIB_FreeType(DxEPG_NAMEOF(devt->type), devt);
}

STATIC SvcEpg_EventInfo_t *	svc_epgeventdama_Clone (const SvcEpg_EventInfo_t *self)
{
	DxEvent_t *devt = OxEPG_DEVENT(self);
	DxEvent_t *clone;
	SvcEpg_EventInfo_t* ret = NULL;

	clone = (DxEvent_t *)DLIB_NewType(
							DxEPG_NAMEOF(devt->type)
							, DxEPG_CLONE_UID
							, devt
							, sizeof(DxEvent_t)
						);
	E_RETURN_IF(clone == NULL, NULL);
	ret = SVC_EPGEVENTDAMA_New(clone);
	if (clone) HLIB_STD_MemFree(clone);
	return ret;
}

STATIC void		svc_epgeventdama_Print (SvcEpg_EventInfo_t *self, FILE *fp)
{
	DxEvent_t *devt = OxEPG_DEVENT(self);

	DLIB_Print(DxEPG_NAMEOF(devt->type), fp, devt);
}

STATIC const HCHAR *	epgdama_event_GetName (SvcEpg_EventInfo_t *self, HxLANG_Id_e language)
{
	(void)language;
	return (const HCHAR*)(OxEPG_DEVENT(self)->name);
}

STATIC const HCHAR *	svc_epgeventdama_GetText (SvcEpg_EventInfo_t *self, HxLANG_Id_e language)
{
	(void)language;
	return (const HCHAR*)(OxEPG_DEVENT(self)->text);
}

STATIC const HCHAR *	svc_epgeventdama_GetLongText (SvcEpg_EventInfo_t *self, HxLANG_Id_e language)
{
	(void)language;
	return (const HCHAR*)(OxEPG_DEVENT(self)->longText);
}

STATIC HINT32		svc_epgeventdama_GetParentalRating (SvcEpg_EventInfo_t *self, HxCountry_e country)
{
	(void)country;
	return OxEPG_DEVENT(self)->parentalRating;
}

STATIC const SvcEpg_EventClass_t *	svc_epgeventdama_Class (void)
{
	static SvcEpg_EventClass_t	s_stClass = {
		  sizeof(SvcEpg_EventDama_t)
		, svc_epgeventdama_Constructor
		, svc_epgeventdama_Destructor
		, svc_epgeventdama_Clone
		, /*compare   */NULL
		, /*match     */NULL
		, svc_epgeventdama_Print
		, /*is_updated*/NULL
		, epgdama_event_GetName
		, svc_epgeventdama_GetText
		, svc_epgeventdama_GetLongText
		, svc_epgeventdama_GetParentalRating
	};
	return &s_stClass;
}

#define	_____GLOBAL_FUCN___________


SvcEpg_EventInfo_t *	SVC_EPGEVENTDAMA_New (DxEvent_t *pstEvent)
{
	HxLOG_Trace();
	HxLOG_Assert(pstEvent);

	return svc_epgevent_New(svc_epgeventdama_Class(), svc_epgeventdama_ToOxEPG_Type(pstEvent->type), pstEvent);
}



