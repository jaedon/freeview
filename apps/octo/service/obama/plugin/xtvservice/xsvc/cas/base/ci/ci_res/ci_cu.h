/***************************************************************
* $Workfile:   ci_cu.h  $
* $Modtime:
* Auther : hmkim
*
* Desc : CAM Firmware Upgrade (CI+)
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_CU_H_
#define _CI_CU_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/
#include "ci_plus.h"

/***************************************************************
* global function prototypes
***************************************************************/
/* CI CU (CAM Firmware Upgrade Resource) */
int CiCu_Init(void);
int CiCu_SsOpenedProc(unsigned short usSsNo);
int CiCu_SsClosedProc(unsigned short usSsNo);
int CiCu_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);


#ifdef __cplusplus
}
#endif

#endif /* _CI_CU_H_ */
