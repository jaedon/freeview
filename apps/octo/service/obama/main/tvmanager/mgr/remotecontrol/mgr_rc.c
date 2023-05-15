#include <octo_common.h>
#include <mgr_common.h>
#include <mgr_rc.h>
#include <pmgr_rc.h>

HERROR MGR_RC_Init (void)
{
	return pmgr_rc_Init ();
}

HERROR MGR_RC_Start (void)
{
	return pmgr_rc_Start ();
}

/*
HERROR MGR_RC_ChangePids (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids)
{
	return pmgr_rc_ChangePids (ulActionId, nPidsCount, pusPids);
}
*/

HERROR MGR_RC_Stop (void)
{
	return pmgr_rc_Stop ();
}
