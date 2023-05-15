/* $Header:   //BASE/archives/STING_T/mw/cas/ci/ci_res/ci_ai.h-arc   1.0   17 Dec 2004 10:32:26   jhkim1  $ */
/***************************************************************
* $Workfile:   ci_ai.h  $
* $Modtime:   13 Dec 2004 09:27:30  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_AI_H_
#define _CI_AI_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/
#include "ci.h"

/***************************************************************
* definition
***************************************************************/


/***************************************************************
* typedef
***************************************************************/


/***************************************************************
* global function prototypes
***************************************************************/
/* CI AI (Application Information Resource) */
int CiAi_Init(void);
int CiAi_SsOpenedProc(unsigned short usSsNo);
int CiAi_SsClosedProc(unsigned short usSsNo);
int CiAi_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);
int CiAi_EnterMenu(unsigned short usSsNo);
CI_APP_INFO * CiAi_GetAppInfo(CI_SLOT_ID usSlotId);
unsigned short CiAi_GetSsNo(CI_SLOT_ID usSlotId);


#ifdef __cplusplus
}
#endif

#endif /* _CI_AI_H_ */

