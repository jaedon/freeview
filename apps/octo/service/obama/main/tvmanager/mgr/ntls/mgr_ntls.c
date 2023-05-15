#include <octo_common.h>
#include <mgr_common.h>
#include <mgr_ntls.h>
#include <pmgr_ntls.h>

HERROR MGR_NTLS_Init (void)
{
	return pmgr_ntls_Init ();
}

HERROR MGR_NTLS_Start (MgrNtls_Setup_t *pstSetup, HUINT32 *pulActionId)
{
	return pmgr_ntls_Start (pstSetup, pulActionId);
}

HERROR MGR_NTLS_ChangePids (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids)
{
	return pmgr_ntls_ChangePids (ulActionId, nPidsCount, pusPids);
}

HERROR MGR_NTLS_Stop (HUINT32 ulActionId)
{
	return pmgr_ntls_Stop (ulActionId);
}

/* end of file */

