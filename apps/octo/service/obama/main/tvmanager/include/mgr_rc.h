#ifndef __MGR_RC_H__
#define __MGR_RC_H__

enum
{
	eMEVT_RC			= eMEVT_RC_START,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_RC_GET_OUTPUTS,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_RC_SETUP,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_RC_SET_CAHNNEL,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_RC_GET_SOURCES,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_RC_UPDATE_SOURCES,	

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_RC_LAUNCH_APP,	
	
	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_RC_GET_CA_INFO,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_RC_NOTIFY_START_STOPPED,
};

typedef enum
{
	eMgrRCFailed_NOSIGNAL			= 0,
	eMgrRCFailed_CREATE				= 1,

} MgrRC_EndReasonType_e;

#if 0
typedef struct
{
	/* manager */
	HUINT32 		ulRsvSlotId;

	/* input param */
	DxTuneParam_t	stTuneParam;

	HINT32			nPIDsCount;
	HUINT16			usPIDs[DxRSV_PIDTABLE_COUNT];


	/* output param */
	Handle_t		hRingBuf;

}MgrRC_Setup_t;
#endif

HERROR			MGR_RC_Init (void);
HERROR			MGR_RC_Start (void);
//HERROR			MGR_RC_ChangePids (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids);
HERROR			MGR_RC_Stop (void);

#endif /* __MGR_RC_H__ */
