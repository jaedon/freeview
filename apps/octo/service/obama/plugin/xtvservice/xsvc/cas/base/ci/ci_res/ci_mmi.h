/* $Header:   //BASE/archives/PDR_LIGHT/mw/cas/ci/ci_res/ci_mmi.h-arc   1.5   Jan 13 2007 00:18:38   htson  $ */
/***************************************************************
* $Workfile:   ci_mmi.h  $
* $Modtime:   Jan 12 2007 06:19:44  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_MMI_H_
#define _CI_MMI_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/
#include "ci.h"

/***************************************************************
* global function prototypes
***************************************************************/
/* CI MMI (Man Machine Interface Resource) */
int CiMmi_Init(void);
int CiMmi_SsOpenedProc(unsigned short usSsNo);
int CiMmi_SsClosedProc(unsigned short usSsNo);
int CiMmi_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);

int CiMmi_SendCloseMmi(unsigned short usSsNo);
int CiMmi_SendMenuAnsw(unsigned short usSsNo, MMI_REQ nMmiReq, unsigned char ucChoiceNo);
int CiMmi_SendAnswString(unsigned short usSsNo, unsigned char *pucStr, unsigned short usStrSize);

#ifdef SUPPORT_SMART_CI_MMI_PINCODE
int CiMmi_CheckWaitingUserInput(CI_SLOT_ID usSlotId, unsigned long* IsWaiting);
#endif


#ifdef __cplusplus
}
#endif

#endif /* _CI_MMI_H_ */

