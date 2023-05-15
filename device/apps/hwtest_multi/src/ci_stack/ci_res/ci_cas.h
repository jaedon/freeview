/* $Header:   //BASE/archives/STING_T/mw/cas/ci/ci_res/ci_cas.h-arc   1.0   17 Dec 2004 10:32:28   jhkim1  $ */
/***************************************************************
* $Workfile:   ci_cas.h  $
* $Modtime:   13 Dec 2004 09:27:30  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_CAS_H_
#define _CI_CAS_H_

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
typedef struct {
	unsigned short * pusCaSysId;
	unsigned char ucCaSysIdCnt;
} CI_CAS_INST;


/***************************************************************
* global function prototypes
***************************************************************/
/* CI CAS (Conditional Access Support Resource) */
int CiCas_Init(void);
int CiCas_SsOpenedProc(unsigned short usSsNo);
int CiCas_SsClosedProc(unsigned short usSsNo);
int CiCas_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
						unsigned char *pucApduData, unsigned long ulApduLen);
int CiCas_SendCaPmt(unsigned short usSsNo, unsigned char *pucCaPmt, unsigned short usCaPmtLen);
unsigned short CiCas_GetSsNo(CI_SLOT_ID usSlotId);


#ifdef __cplusplus
}
#endif

#endif /* _CI_CAS_H_ */

