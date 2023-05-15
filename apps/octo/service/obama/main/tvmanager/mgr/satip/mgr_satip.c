#include <octo_common.h>
#include <mgr_common.h>
#include <mgr_satip.h>
#include <mgr_models.h>
#include <pmgr_satip.h>

HERROR MGR_SATIP_Init (void)
{
	return pmgr_satip_Init ();
}

HERROR MGR_SATIP_Start (MgrSatIp_Setup_t *pstSetup, HUINT32 *pulActionId)
{
	return pmgr_satip_Start (pstSetup, pulActionId);
}

HERROR MGR_SATIP_GetSatipSiSpec (SvcSi_SatipSpec_t *pstSiSpec)
{
	return MGR_MODEL_SatipGetSiSpec (pstSiSpec);
}

HERROR MGR_SATIP_ChangePids (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids)
{
	return pmgr_satip_ChangePids (ulActionId, nPidsCount, pusPids);
}

HERROR MGR_SATIP_ChangePriority (HUINT32 ulActionId, HUINT16 usPriority)
{
	return pmgr_satip_ChangePriority (ulActionId, usPriority);
}

HERROR MGR_SATIP_ChangeTranscodingProfile (HUINT32 ulActionId, MgrSatIp_TranscoderStreamProfile stProfile)
{
	return pmgr_satip_ChangeTranscodingProfile (ulActionId, stProfile);
}

HERROR MGR_SATIP_Try (MgrSatIp_Setup_t *pstSetup)
{
	return pmgr_satip_Try (pstSetup);
}

HERROR MGR_SATIP_Stop (HUINT32 ulActionId)
{
	return pmgr_satip_Stop (ulActionId);
}

HBOOL MGR_SATIP_GetDrmFlag (HUINT32 ulActionId)
{
	return pmgr_satip_GetDrmFlag (ulActionId);
}