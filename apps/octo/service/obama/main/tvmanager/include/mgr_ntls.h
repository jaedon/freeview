#ifndef __MGR_NTLS_H__
#define __MGR_NTLS_H__

enum
{
	eMEVT_NTLS			= eMEVT_NTLS_START,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_NTLS_NOTIFY_START_FAILED,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_NTLS_NOTIFY_STARTED,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_NTLS_NOTIFY_START_STOPPED,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_NTLS_NOTIFY_UPDATE_PIDS,

};

typedef enum
{
	eMgrNtlsFailed_NOSIGNAL			= 0,
	eMgrNtlsFailed_CREATE			= 1,

} MgrNtls_EndReasonType_e;

typedef struct
{
	Handle_t		hSvc;

#if 0
	/* manager */
	HUINT32 		ulRsvSlotId;

	/* input param */
	DxTuneParam_t	stTuneParam;

	HINT32			nPIDsCount;
	HUINT16			usPIDs[DxRSV_PIDTABLE_COUNT];
#endif

	/* output param */
	Handle_t		hRingBuf;

}MgrNtls_Setup_t;

HERROR			MGR_NTLS_Init (void);
HERROR			MGR_NTLS_Start (MgrNtls_Setup_t *pstSetup, HUINT32 *pulActionId);
HERROR			MGR_NTLS_ChangePids (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids);
HERROR			MGR_NTLS_Stop (HUINT32 ulActionId);

#endif /* __MGR_NTLS_H__ */
