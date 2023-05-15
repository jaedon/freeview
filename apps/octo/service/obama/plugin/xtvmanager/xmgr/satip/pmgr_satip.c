#include <octo_common.h>
#include <mgr_common.h>
#include <mgr_satip.h>
#include <pmgr_satip.h>

#include <svc_rec.h>

#include <_xmgr_satip_base.h>
#include <_xmgr_satip_tara.h>

HERROR pmgr_satip_Init (void)
{
#if defined(CONFIG_STREAMING_SATIP)
#if defined(CONFIG_STREAMING_SATIP_TARA)
	return xmgr_satip_Init_TARA();
#else
	return xmgr_satip_Init_BASE ();
#endif
#else
	return ERR_FAIL;
#endif
}
HERROR pmgr_satip_Start (MgrSatIp_Setup_t *pstSetup, HUINT32 *pulActionId)
{
#if defined(CONFIG_STREAMING_SATIP)
#if defined(CONFIG_STREAMING_SATIP_TARA)
	return xmgr_satip_Start_TARA (pstSetup, pulActionId);
#else
	return xmgr_satip_Start_BASE (pstSetup, pulActionId);
#endif
#else
	return ERR_FAIL;
#endif
}
HERROR pmgr_satip_ChangePids (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids)
{
#if defined(CONFIG_STREAMING_SATIP)
#if defined(CONFIG_STREAMING_SATIP_TARA)
	return xmgr_satip_ChangePids_TARA (ulActionId, nPidsCount, pusPids);
#else
	return xmgr_satip_ChangePids_BASE (ulActionId, nPidsCount, pusPids);
#endif
#else
	return ERR_FAIL;
#endif
}
HERROR pmgr_satip_ChangePriority (HUINT32 ulActionId, HUINT16 usPriority)
{
#if defined(CONFIG_STREAMING_SATIP)
#if defined(CONFIG_STREAMING_SATIP_TARA)
	return xmgr_satip_ChangePriority_TARA (ulActionId, usPriority);
#else
	return xmgr_satip_ChangePriority_BASE (ulActionId, usPriority);
#endif
#else
	return ERR_FAIL;
#endif
}
HERROR pmgr_satip_ChangeTranscodingProfile (HUINT32 ulActionId, MgrSatIp_TranscoderStreamProfile stProfile)
{
#if defined(CONFIG_STREAMING_SATIP)
#if defined(CONFIG_STREAMING_SATIP_TARA)
	return xmgr_satip_ChangeTranscodingProfile_TARA (ulActionId, stProfile);
#else
	return xmgr_satip_ChangeTranscodingProfile_BASE (ulActionId, stProfile);
#endif
#else
	return ERR_FAIL;
#endif
}
HERROR pmgr_satip_Try (MgrSatIp_Setup_t *pstSetup)
{
#if defined(CONFIG_STREAMING_SATIP)
#if defined(CONFIG_STREAMING_SATIP_TARA)
	return xmgr_satip_Try_TARA (pstSetup);
#else
	return xmgr_satip_Try_BASE (pstSetup);
#endif
#else
	return ERR_FAIL;
#endif
}
HERROR pmgr_satip_Stop (HUINT32 ulActionId)
{
#if defined(CONFIG_STREAMING_SATIP)
#if defined(CONFIG_STREAMING_SATIP_TARA)
	return xmgr_satip_Stop_TARA (ulActionId);
#else
	return xmgr_satip_Stop_BASE (ulActionId);
#endif
#else
	return ERR_FAIL;
#endif
}
HBOOL pmgr_satip_GetDrmFlag (HUINT32 ulActionId)
{
#if defined(CONFIG_STREAMING_SATIP)
#if defined(CONFIG_STREAMING_SATIP_TARA)
	return xmgr_satip_GetDrmFlag_TARA (ulActionId);
#else
	return FALSE;
#endif
#else
	return FALSE;
#endif
}