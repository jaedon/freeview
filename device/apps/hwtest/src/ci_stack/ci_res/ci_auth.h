/* $Header:   //BASE/archives/STING_T/mw/cas/ci/ci_res/ci_auth.h-arc   1.0   17 Dec 2004 10:32:26   jhkim1  $ */
/***************************************************************
* $Workfile:   ci_auth.h  $
* $Modtime:   13 Dec 2004 09:27:30  $
* Auther : Jaehee Cho
*
* Desc :
*
* Copyright (c) 2002 by Humax Co., Ltd.
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


/***************************************************************
* definition
***************************************************************/
#define	CI_AUTH_BD_PROTOCOL_ID				0x1702


/***************************************************************
* typedef
***************************************************************/


/***************************************************************
* global function prototypes
***************************************************************/
/* CI AUTH (Authentication Resource) */
int CiAuth_Init(void);
int CiAuth_SsOpenedProc(unsigned short usSsNo);
int CiAuth_SsClosedProc(unsigned short usSsNo);
int CiAuth_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
					unsigned char *pucApduData, unsigned long ulApduLen);


#ifdef __cplusplus
}
#endif

#endif /* _CI_AUTH_H_ */
