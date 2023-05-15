#ifndef __PMGR_SATIP_H__
#define __PMGR_SATIP_H__

#include <octo_common.h>
#include <mgr_satip.h>


HERROR			pmgr_satip_Init (void);
HERROR			pmgr_satip_Start (MgrSatIp_Setup_t *pstSetup, HUINT32 *pulActionId);
HERROR			pmgr_satip_ChangePids (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids);
HERROR			pmgr_satip_ChangePriority (HUINT32 ulActionId, HUINT16 usPriority);
HERROR			pmgr_satip_ChangeTranscodingProfile (HUINT32 ulActionId, MgrSatIp_TranscoderStreamProfile stProfile);
HERROR			pmgr_satip_Try (MgrSatIp_Setup_t *pstSetup);
HERROR			pmgr_satip_Stop (HUINT32 ulActionId);
HBOOL 			pmgr_satip_GetDrmFlag (HUINT32 ulActionId);

#endif /* __PMGR_SATIP_H__ */
