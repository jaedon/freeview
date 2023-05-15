/***************************************************************
* $Workfile:   ci_capvr.h  $
* $Modtime:
* Auther : hmkim
*
* Desc : CA PVR (CI+)
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_CAPVR_H_
#define _CI_CAPVR_H_

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
/* CI CAPVR (CA PVR Resource) */
int CiCaPvr_Init(void);
int CiCaPvr_SsOpenedProc(unsigned short usSsNo);
int CiCaPvr_SsClosedProc(unsigned short usSsNo);
int CiCaPvr_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);

int CiCaPvr_SendCaPvrCmd(unsigned short usSsNo, unsigned char *pucCaPvrCmd, unsigned short usCaPvrCmdLen);
unsigned short CiCaPvr_GetSsNo(CI_SLOT_ID usSlotId);


#ifdef __cplusplus
}
#endif

#endif /* _CI_CAPVR_H_ */
