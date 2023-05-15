/* $Header:   //BASE/archives/STING_T/mw/cas/ci/ci_res/ci_rm.h-arc   1.0   17 Dec 2004 10:32:34   jhkim1  $ */
/***************************************************************
* $Workfile:   ci_rm.h  $
* $Modtime:   13 Dec 2004 09:27:32  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_RM_H_
#define _CI_RM_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/


/***************************************************************
* global function prototypes
***************************************************************/
/* CI RM (CI Resource Manager Resource) */
int CiRm_Init(void);
int CiRm_SsOpenedProc(unsigned short usSsNo);
int CiRm_SsClosedProc(unsigned short usSsNo);
int CiRm_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);


#ifdef __cplusplus
}
#endif

#endif /* _CI_RM_H_ */

