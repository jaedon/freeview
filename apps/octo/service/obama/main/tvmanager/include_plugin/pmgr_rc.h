#ifndef __PMGR_SATIP_H__
#define __PMGR_SATIP_H__

#include <octo_common.h>
#include <mgr_rc.h>


HERROR			pmgr_rc_Init (void);
HERROR			pmgr_rc_Start (void);
//HERROR			pmgr_rc_ChangePids (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids);
HERROR			pmgr_rc_Stop (void);

#endif /* __PMGR_SATIP_H__ */
