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

// 디버깅을 위해 캠과 MHEG 엔진 간에 주고 받는 데이터를 스택 레이어에서 모니터링함 (기본 모드)
#define _CI_DEBUG_AMMI_DATA_

// 디버깅을 위해 캠으로부터 올라오는 파일 데이터를 raw 데이터로 저장함
//#define _CI_DEBUG_AMMI_SAVE_FILE_DATA_

// 디버깅을 위해 캠으로부터 올라오는 파일 데이터를 디코딩함 (ci_ammi_lib 디렉토리 빌드 요구됨)
//#define _CI_DEBUG_AMMI_DECODE_FILE_DATA_

// 디버깅을 위해 캠과 MHEG 엔진을 에뮬레이트함 (캠을 에뮬레이트하기 위해서는 사전에 저장된 raw 데이터들 요구됨)
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

// 아래 함수들은 CI+ Protocol Porting Layer를 통해서 결국 Application MMI를 처리하는 어플("CI+ Application Presentation Engine")에서 호출...
int CiAmmi_RegisterCallback(CI_AMMI_CALLBACK_T pfnCallback); // CI_RegisterAppMmiCallback() 으로부터 호출
int CiAmmi_FileRequest(unsigned short usSsNo, CI_AMMI_REQUEST_TYPE enReqType, unsigned char *pucReqName, unsigned long ulReqNameLen); // CI_RequestFileorData() 으로부터 호출
int CiAmmi_AppAbortRequest(unsigned short usSsNo, CI_AMMI_ABORT_REQUEST_CODE enAbortReqCode); // CI_RequestAppAbort() 으로부터 호출

#if defined(_CI_DEBUG_AMMI_USE_EMULATOR_FOR_CAM_)
void CiAmmi_Emul_RequestStart(unsigned long ulTimerId, void *pvParam); // CiAmmi_RequestStart()를 에뮬레이트함
#endif

#ifdef __cplusplus
}
#endif

#endif /* _CI_AMMI_H_ */
