/**************************************************************
 *	@file		rsv_fvc.h
 *	IP EPG FVC for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/08/01
 *	@author			humax
 *	@brief			IP EPG FVC for OCTO 2.0 Project
 **************************************************************/
#ifndef _IPEPG_FVC_H_
#define _IPEPG_FVC_H_
#include	<hlib.h>
#include "../include/ipepg.h"

extern HUINT32	gTimerFvcTime;
extern HBOOL	gbSetFvcTimer;

extern HUINT32	gTimerFvcRetryTime;
extern HBOOL	gbSetFvcRetryTimer;

extern HUINT32	gTimerFvcSeriesEpgTime;
extern HBOOL	gbSetFvcSeriesEpgTimer;

typedef	enum
{
	eIPEPG_FVC_URL_EXPIRY_ONID,
	eIPEPG_FVC_URL_SERIES_SVCID,
	eIPEPG_FVC_URL_MAX
} IPEPG_FVC_URL_TYPE_e;


HERROR	IPEPG_GetScheduleExpiryHash (HxHASH_t **ppstExpiryHash, HUINT32 ulLimitSize);
HERROR	IPEPG_GetServiceExpiry (DxFvcExpiry_t	*pstFvcServiceExpiry);
HERROR	IPEPG_SetNextWakeup (void);
void	IPEPG_RefreshExpiryQueue (void *arg);

HERROR	IPEPG_SetFvcTimer (HUINT32 time);
HERROR	IPEPG_SetFvcRetryTimer (HUINT32 time);
HERROR	IPEPG_SetFvcSeriesEpgTimer (HUINT32 time);

HERROR	IPEPG_CancelFvcTimer (void);
HERROR	IPEPG_CancelFvcRetryTimer (void);
HERROR	IPEPG_CancelFvcSeriesEpgTimer (void);

HERROR	IPEPG_RequestAuth (void);
void	IPEPG_DeleteOldIpEpg (void);


#endif

/*********************** End of File ******************************/

