/* $Header:   //BASE/archives/STING_T/mw/cas/ci/ci_res/ci_dt.h-arc   1.0   17 Dec 2004 10:32:28   jhkim1  $ */
/***************************************************************
* $Workfile:   ci_dt.h  $
* $Modtime:   13 Dec 2004 09:27:30  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_DT_H_
#define _CI_DT_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/


/***************************************************************
* global function prototypes
***************************************************************/
/* CI DT (Date & Time Resource) */
int CiDt_Init(void);
int CiDt_SsOpenedProc(unsigned short usSsNo);
int CiDt_SsClosedProc(unsigned short usSsNo);
int CiDt_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);

void CiDt_TimerProc(void);


#ifdef __cplusplus
}
#endif

#endif /* _CI_DT_H_ */

