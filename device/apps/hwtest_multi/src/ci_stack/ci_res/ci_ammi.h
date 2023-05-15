
/***************************************************************
* $Workfile:   ci_ammi.h  $
* $Modtime:
* Auther : hmkim
*
* Desc : CI+ Browser Application MMI (CI+)
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/

#ifndef _CI_AMMI_H_
#define _CI_AMMI_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************
* include
***************************************************************/
#include "ci_plus.h"

/***************************************************************
* definition
***************************************************************/


/***************************************************************
* typedef
***************************************************************/


/***************************************************************
* global function prototypes
***************************************************************/
int CiAmmi_Init(void);
int CiAmmi_SsOpenedProc(unsigned short usSsNo);
int CiAmmi_SsClosedProc(unsigned short usSsNo);
int CiAmmi_ApduProc(unsigned short usSsNo, unsigned long ulApduTag,
					unsigned char *pucApduData, unsigned long ulApduLen);

// �Ʒ� �Լ����� CI+ Protocol Porting Layer�� ���ؼ� �ᱹ Application MMI�� ó���ϴ� ����("CI+ Application Presentation Engine")���� ȣ��...
int CiAmmi_RegisterCallback(CI_AMMI_CALLBACK_T pfnCallback); // CI_RegisterAppMmiCallback() ���κ��� ȣ��
int CiAmmi_FileRequest(unsigned short usSsNo, unsigned char ucReqType /* CI_AMMI_REQUEST_TYPE */, unsigned char *pucBytes, unsigned long ulBytesLen); // CI_RequestFileorData() ���κ��� ȣ��
int CiAmmi_AppAbortRequest(unsigned short usSsNo, unsigned char ucAbortReqCode /* CI_AMMI_ABORT_REQUEST_CODE */); // CI_RequestAppAbort() ���κ��� ȣ��


#ifdef __cplusplus
}
#endif

#endif /* _CI_AMMI_H_ */
