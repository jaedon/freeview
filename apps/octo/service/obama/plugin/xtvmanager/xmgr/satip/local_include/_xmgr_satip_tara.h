#ifndef __XMGR_SATIP_TARA_H__
#define __XMGR_SATIP_TARA_H__

HERROR			xmgr_satip_Init_TARA (void);
HERROR			xmgr_satip_Start_TARA (MgrSatIp_Setup_t *pstSetup, HUINT32 *pulActionId);
HERROR			xmgr_satip_ChangePids_TARA (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids);
HERROR			xmgr_satip_ChangePriority_TARA (HUINT32 ulActionId, HUINT16 usPriority);
HERROR			xmgr_satip_ChangeTranscodingProfile_TARA (HUINT32 ulActionId, MgrSatIp_TranscoderStreamProfile stProfile);
HERROR			xmgr_satip_Try_TARA (MgrSatIp_Setup_t *pstSetup);
HERROR			xmgr_satip_Stop_TARA (HUINT32 ulActionId);
HBOOL 			xmgr_satip_GetDrmFlag_TARA (HUINT32 ulActionId);

#endif /* __XMGR_SATIP_TARA_H__ */
