#ifndef __PMGR_NTLS_H__
#define __PMGR_NTLS_H__

#include <octo_common.h>
#include <mgr_ntls.h>


HERROR			pmgr_ntls_Init (void);
HERROR			pmgr_ntls_Start (MgrNtls_Setup_t *pstSetup, HUINT32 *pulActionId);
HERROR			pmgr_ntls_ChangePids (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids);
HERROR			pmgr_ntls_Stop (HUINT32 ulActionId);

#endif /* __PMGR_NTLS_H__ */
