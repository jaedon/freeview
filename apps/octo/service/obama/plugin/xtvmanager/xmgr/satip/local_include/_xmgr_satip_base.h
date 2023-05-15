#ifndef __XMGR_SATIP_BASE_H__
#define __XMGR_SATIP_BASE_H__

typedef enum
{
	eSatIpState_Idle			= 0,

	eSatIpState_ReqStart,
	eSatIpState_Running,

	// 정지되어 있는 status
	eSatIpState_Stopped,
	eSatIpState_NonPid,

	eSatIpState_EndOfCase
} xMgrSatIpState_e;

typedef struct
{

	Handle_t			hAction;
	MgrSatIp_Setup_t	stSetup;

	xMgrSatIpState_e	eState;
	BUS_Callback_t		pfActionProc;

}xMgrSatIpContext_t;


HERROR			xmgr_satip_Init_BASE (void);
HERROR			xmgr_satip_Start_BASE (MgrSatIp_Setup_t *pstSetup, HUINT32 *pulActionId);
HERROR			xmgr_satip_ChangePids_BASE (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids);
HERROR			xmgr_satip_ChangePriority_BASE (HUINT32 ulActionId, HUINT16 usPriority);
HERROR			xmgr_satip_ChangeTranscodingProfile_BASE (HUINT32 ulActionId, MgrSatIp_TranscoderStreamProfile stProfile);
HERROR			xmgr_satip_Try_BASE (MgrSatIp_Setup_t *pstSetup);
HERROR			xmgr_satip_Stop_BASE (HUINT32 ulActionId);

#endif /* __XMGR_SATIP_BASE_H__ */
