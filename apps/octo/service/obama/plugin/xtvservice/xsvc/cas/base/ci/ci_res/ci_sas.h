/***************************************************************
* $Workfile:   ci_sas.h  $
* $Modtime:
* Auther : hmkim
*
* Desc : Specific Application Support for MHP (CI+)
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_SAS_H_
#define _CI_SAS_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/


/***************************************************************
* global function prototypes
***************************************************************/
/* CI SAS (Specific Application Support for MHP Resource) */
int CiSas_Init(void);
int CiSas_SsOpenedProc(unsigned short usSsNo);
int CiSas_SsClosedProc(unsigned short usSsNo);
int CiSas_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);


#ifdef __cplusplus
}
#endif

#endif /* _CI_SAS_H_ */
