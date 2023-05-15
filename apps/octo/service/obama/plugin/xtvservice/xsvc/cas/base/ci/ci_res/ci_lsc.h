/***************************************************************
* $Workfile:   ci_lsc.h  $
* $Modtime:
* Auther : hmkim
*
* Desc : Low Speed Communication (CI+ v1.3)
*
* Copyright (c) 2011 by Humax Co., Ltd.
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
* global function prototypes
***************************************************************/
/* CI LSC (Low Speed Communication) */
int CiLsc_Init(void);
int CiLsc_SsOpenedProc(unsigned short usSsNo);
int CiLsc_SsClosedProc(unsigned short usSsNo);
int CiLsc_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);


#ifdef __cplusplus
}
#endif

#endif /* _CI_LSC_H_ */
