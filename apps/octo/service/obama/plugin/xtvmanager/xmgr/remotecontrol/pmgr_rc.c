#include <octo_common.h>
#include <mgr_common.h>
#include <mgr_rc.h>
#include <pmgr_rc.h>

#include <svc_rec.h>

#include <_xmgr_rc_base.h>

HERROR pmgr_rc_Init (void)
{
	return xmgr_rc_Init_BASE ();
}
HERROR pmgr_rc_Start (void)
{
	return xmgr_rc_Start_BASE ();
}
/*
HERROR pmgr_rc_ChangePids (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids)
{
	return xmgr_rc_ChangePids_BASE (ulActionId, nPidsCount, pusPids);
}
*/
HERROR pmgr_rc_Stop (void)
{
	return xmgr_rc_Stop_BASE ();
}
