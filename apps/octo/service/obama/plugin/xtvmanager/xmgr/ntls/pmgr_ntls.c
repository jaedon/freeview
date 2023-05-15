#include <octo_common.h>
#include <mgr_common.h>
#include <mgr_ntls.h>
#include <pmgr_ntls.h>

#include <svc_rec.h>

#include <_xmgr_ntls_base.h>

HERROR pmgr_ntls_Init (void)
{
#if defined(CONFIG_NON_TRANSCODING_STREAMING)
	return xmgr_ntls_Init_BASE ();
#else
	return ERR_FAIL;
#endif
}
HERROR pmgr_ntls_Start (MgrNtls_Setup_t *pstSetup, HUINT32 *pulActionId)
{
#if defined(CONFIG_NON_TRANSCODING_STREAMING)
	return xmgr_ntls_Start_BASE (pstSetup, pulActionId);
#else
	return ERR_FAIL;
#endif
}
HERROR pmgr_ntls_ChangePids (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids)
{
#if defined(CONFIG_NON_TRANSCODING_STREAMING)
	return xmgr_ntls_ChangePids_BASE (ulActionId, nPidsCount, pusPids);
#else
	return ERR_FAIL;
#endif
}
HERROR pmgr_ntls_Stop (HUINT32 ulActionId)
{
#if defined(CONFIG_NON_TRANSCODING_STREAMING)
	return xmgr_ntls_Stop_BASE (ulActionId);
#else
	return ERR_FAIL;
#endif
}

/* end of file */

