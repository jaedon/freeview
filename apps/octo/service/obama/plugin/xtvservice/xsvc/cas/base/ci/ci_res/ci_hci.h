/* $Header:   //BASE/archives/STING_T/mw/cas/ci/ci_res/ci_hci.h-arc   1.0   17 Dec 2004 10:32:30   jhkim1  $ */
/***************************************************************
* $Workfile:   ci_hci.h  $
* $Modtime:   13 Dec 2004 09:27:30  $
* Auther : Jaehee Cho / hmkim (CI+)
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_HCI_H_
#define _CI_HCI_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/


/***************************************************************
* global function prototypes
***************************************************************/
/* CI HCI (Host Control Interface Resource) */
int CiHci_Init(void);
int CiHci_SsOpenedProc(unsigned short usSsNo);
int CiHci_SsClosedProc(unsigned short usSsNo);
int CiHci_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);

#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
int CiHci_TuneReply(CI_SLOT_ID usSlotId, CI_TUNE_STATUS eTuneResult);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _CI_HCI_H_ */

