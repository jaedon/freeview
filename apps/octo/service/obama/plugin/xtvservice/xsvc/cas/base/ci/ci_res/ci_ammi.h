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
* debugging
***************************************************************/
#if defined(_CI_PLUS_DEBUG_)

// ������� ���� ķ�� MHEG ���� ���� �ְ� �޴� �����͸� ���� ���̾�� ����͸��� (�⺻ ���)
#define _CI_DEBUG_AMMI_DATA_

// ������� ���� ķ���κ��� �ö���� ���� �����͸� raw �����ͷ� ������
//#define _CI_DEBUG_AMMI_SAVE_FILE_DATA_

// ������� ���� ķ���κ��� �ö���� ���� �����͸� ���ڵ��� (ci_ammi_lib ���丮 ���� �䱸��)
//#define _CI_DEBUG_AMMI_DECODE_FILE_DATA_

// ������� ���� ķ�� MHEG ������ ���ķ���Ʈ�� (ķ�� ���ķ���Ʈ�ϱ� ���ؼ��� ������ ����� raw �����͵� �䱸��)
#define _CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_
//#define _CI_DEBUG_AMMI_USE_EMULATOR_FOR_MHEG_ENGINE_

#ifdef _CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_
	#ifndef _CI_DEBUG_AMMI_SAVE_FILE_DATA_
	#define _CI_DEBUG_AMMI_SAVE_FILE_DATA_
	#endif
#endif

#ifdef _CI_DEBUG_AMMI_USE_EMULATOR_FOR_MHEG_ENGINE_
	#ifndef _CI_DEBUG_AMMI_DECODE_FILE_DATA_
	#define _CI_DEBUG_AMMI_DECODE_FILE_DATA_
	#endif
#endif

#ifdef _CI_DEBUG_AMMI_DECODE_FILE_DATA_
	#ifndef _CI_DEBUG_AMMI_SAVE_FILE_DATA_
	#define _CI_DEBUG_AMMI_SAVE_FILE_DATA_
	#endif
#endif

#ifdef _CI_DEBUG_AMMI_SAVE_FILE_DATA_
	#ifndef _CI_DEBUG_AMMI_DATA_
	#define _CI_DEBUG_AMMI_DATA_
	#endif
#endif

#endif

#ifdef WIN32
	#ifndef _CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_
	#define _CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_
	#endif
	#ifndef _CI_DEBUG_AMMI_SAVE_FILE_DATA_
	#define _CI_DEBUG_AMMI_SAVE_FILE_DATA_
	#endif
#endif

/***************************************************************
* global function prototypes
***************************************************************/
/* CI AMMI (CI+ Browser Application MMI Resource) */
int CiAmmi_Init(void);
int CiAmmi_SsOpenedProc(unsigned short usSsNo);
int CiAmmi_SsClosedProc(unsigned short usSsNo);
int CiAmmi_ApduProc(unsigned short usSsNo, unsigned long ulApduTag, unsigned char *pucApduData, unsigned long ulApduLen);

// �Ʒ� �Լ����� CI+ Protocol Porting Layer�� ���ؼ� �ᱹ Application MMI�� ó���ϴ� ����("CI+ Application Presentation Engine")���� ȣ��...
int CiAmmi_RegisterCallback(CI_AMMI_CALLBACK_T pfnCallback); // CI_RegisterAppMmiCallback() ���κ��� ȣ��
int CiAmmi_FileRequest(unsigned short usSsNo, CI_AMMI_REQUEST_TYPE enReqType, unsigned char *pucReqName, unsigned long ulReqNameLen); // CI_RequestFileorData() ���κ��� ȣ��
int CiAmmi_AppAbortRequest(unsigned short usSsNo, CI_AMMI_ABORT_REQUEST_CODE enAbortReqCode); // CI_RequestAppAbort() ���κ��� ȣ��

#if defined(_CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_)
void CiAmmi_Emul_RequestStart(unsigned long ulTimerId, void *pvParam); // CiAmmi_RequestStart()�� ���ķ���Ʈ��
#endif

#ifdef __cplusplus
}
#endif

#endif /* _CI_AMMI_H_ */
