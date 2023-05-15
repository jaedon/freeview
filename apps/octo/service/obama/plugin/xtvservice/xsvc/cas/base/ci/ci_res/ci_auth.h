/***************************************************************
* $Workfile:   ci_auth.h  $
* $Modtime:
* Auther : hmkim
*
* Desc : Authentication
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_AUTH_H_
#define _CI_AUTH_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/
#include "ci_plus.h"

/***************************************************************
* global function prototypes
***************************************************************/
/* CI AUTH (Authentication Resource) */
int CiAuth_Init(void);
int CiAuth_SsOpenedProc(unsigned short usSsNo);
int CiAuth_SsClosedProc(unsigned short usSsNo);
int CiAuth_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);

#ifdef CONFIG_OP_CANALREADY
int CiAuth_CanalReadyGetInitInfo(CI_SLOT_ID usSlotId, SvcCas_CiAuthInfo_t *pstCiAuthInfo);
int CiAuth_CanalReadyMoralityLevelUnlock(CI_SLOT_ID usSlotId);
#endif


#ifdef __cplusplus
}
#endif

#endif /* _CI_AUTH_H_ */
