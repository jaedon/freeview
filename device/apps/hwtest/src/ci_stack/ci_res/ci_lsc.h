/* $Header:   //BASE/archives/STING_T/mw/cas/ci/ci_res/ci_lsc.h-arc   1.0   17 Dec 2004 10:32:32   jhkim1  $ */
/***************************************************************
* $Workfile:   ci_lsc.h  $
* $Modtime:   13 Dec 2004 09:27:30  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_LSC_H_
#define _CI_LSC_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/


/***************************************************************
* definition
***************************************************************/


/***************************************************************
* typedef
***************************************************************/


/***************************************************************
* global function prototypes
***************************************************************/
/* CI LSC (Low Speed Communication Resource) */
int CiLsc_Init(void);
int CiLsc_SsOpenedProc(unsigned short usSsNo);
int CiLsc_SsClosedProc(unsigned short usSsNo);
int CiLsc_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
					unsigned char *pucApduData, unsigned long ulApduLen);


#ifdef __cplusplus
}
#endif

#endif /* _CI_LSC_H_ */

