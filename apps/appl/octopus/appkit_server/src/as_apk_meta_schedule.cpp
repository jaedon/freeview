#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>

#include <dbus_util_cvt.h>
#include <as_apk_meta_schedule.h>

char const * const cApkMetaSchedule::BUSNAME_APK_META_SCHEDULE = "Octopus.Appkit.Meta.Schedule";
char const * const cApkMetaSchedule::BUSPATH_APK_META_SCHEDULE = "/Octopus/Appkit/Meta/Schedule";


cApkMetaSchedule::cApkMetaSchedule(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_META_SCHEDULE)
{
	/* apkit meta schedule */
    {	
    }
}

DbtSchedule_t cApkMetaSchedule::GetSchedule(const uint32_t& slot)
{
	DbtSchedule_t reval;

	HERROR hErr = ERR_FAIL;

	DxSchedule_t stSchedule;
	HxSTD_memset (&stSchedule, 0, sizeof(DxSchedule_t));
	
	hErr = APK_META_SCHEDULE_GetSchedule ((HUINT32)slot, &stSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SCHEDULE_GetSchedule : Error");
	}

	reval = cDbusUtilCvt::C2DxSchedule (&stSchedule);
	return reval;
}

DbtSchedule_t cApkMetaSchedule::GetScheduleByIndex(const uint32_t& index)
{
	DbtSchedule_t reval;

	HERROR hErr = ERR_FAIL;

	DxSchedule_t stSchedule;
	HxSTD_memset (&stSchedule, 0, sizeof(DxSchedule_t));

	hErr = APK_META_SCHEDULE_GetScheduleByIndex ((HUINT32)index, &stSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SCHEDULE_GetScheduleByIndex : Error");
	}

	reval = cDbusUtilCvt::C2DxSchedule (&stSchedule);
	return reval;
}

DbtSchedule_t cApkMetaSchedule::GetScheduleByType(const uint32_t& rsvType, const uint32_t& index)
{
	DbtSchedule_t reval;

	HERROR hErr = ERR_FAIL;

	DxSchedule_t stSchedule;
	HxSTD_memset (&stSchedule, 0, sizeof(DxSchedule_t));

	hErr = APK_META_SCHEDULE_GetScheduleByType ((DxRsvType_e)rsvType, (HUINT32)index, &stSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SCHEDULE_GetScheduleByType : Error");
	}

	reval = cDbusUtilCvt::C2DxSchedule (&stSchedule);
	return reval;
}

uint32_t cApkMetaSchedule::CountSchedule()
{
	HERROR hErr = ERR_FAIL;

	HUINT32 ulNumSch	=	0;
		
	hErr = APK_META_SCHEDULE_CountSchedule (&ulNumSch);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SCHEDULE_CountSchedule : Error");
	}

	return (uint32_t)ulNumSch;
}

uint32_t cApkMetaSchedule::CountScheduleByType(const uint32_t& rsvType)
{
	HERROR hErr = ERR_FAIL;

	HUINT32 ulNumSch	=	0;

	hErr = APK_META_SCHEDULE_CountScheduleByType ((DxRsvType_e)rsvType, &ulNumSch);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SCHEDULE_CountScheduleByType : Error");
	}

	return (uint32_t)ulNumSch;
}

uint32_t cApkMetaSchedule::AddSchedule(const DbtSchedule_t& schedule)
{
	HERROR hErr = ERR_FAIL;
	HUINT32 ulSlot	=	0;

	DxSchedule_t stSchedule;
	HxSTD_memset (&stSchedule, 0, sizeof(DxSchedule_t));

	cDbusUtilCvt::C2DbtSchedule (schedule, &stSchedule);

	hErr = APK_META_SCHEDULE_AddSchedule (&stSchedule, &ulSlot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SCHEDULE_AddSchedule : Error");
	}

	return (uint32_t)ulSlot;
}

void cApkMetaSchedule::UpdateSchedule(const uint32_t& slot, const DbtSchedule_t& schedule)
{
	HERROR hErr = ERR_FAIL;

	DxSchedule_t stSchedule;

	cDbusUtilCvt::C2DbtSchedule (schedule, &stSchedule);

	hErr = APK_META_SCHEDULE_UpdateSchedule ((HUINT32)slot, &stSchedule);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SCHEDULE_UpdateSchedule : Error");
	}

	return ;
}

void cApkMetaSchedule::CancelSchedule(const uint32_t& slot)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_META_SCHEDULE_CancelSchedule ((HUINT32)slot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SCHEDULE_CancelSchedule : Error");
	}

	return ;
}

void cApkMetaSchedule::StopSchedule(const uint32_t& slot)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_META_SCHEDULE_StopSchedule ((HUINT32)slot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SCHEDULE_StopSchedule : Error");
	}

	return ;
}

void cApkMetaSchedule::PauseSchedule(const uint32_t& slot)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_META_SCHEDULE_PauseSchedule ((HUINT32)slot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SCHEDULE_PauseSchedule : Error");
	}

	return ;
}

void cApkMetaSchedule::ResumeSchedule(const uint32_t& slot)
{
	HERROR hErr = ERR_FAIL;
	hErr = APK_META_SCHEDULE_ResumeSchedule ((HUINT32)slot);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_META_SCHEDULE_ResumeSchedule : Error");
	}

	return ;
}

::DBus::Struct< uint32_t, std::vector< uint32_t > > cApkMetaSchedule::GetConflictResult (const uint32_t& slot)
{
	::DBus::Struct< uint32_t, std::vector< uint32_t > >	reval;

	APK_META_SCH_Conflict_t * pstConflictInfo = NULL;
	pstConflictInfo = APK_META_SCHEDULE_GetConflictResult ((HUINT32)slot);

	if (NULL != pstConflictInfo)
	{
		reval._1	=	pstConflictInfo->eType;

		{
			HINT32 i;
			for (i=0 ; i<(HINT32)pstConflictInfo->ulNumConflict ; i++)
			{
				reval._2.push_back(pstConflictInfo->pstConfArray[i].ulSlot);
			}
		}
		
		APK_META_SCHEDULE_FreeConflictResult (pstConflictInfo);
	}

	return reval;
}
