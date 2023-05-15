
/*******************************************************************************
 * HUMAX R&D Lab., HUMAX INC., SEOUL, KOREA
 * COPYRIGHT(c) 2010 by HUMAX Inc.
 *
 * All rights reserved. No part of this work may be reproduced, stored in a
 * retrieval system, or transmitted by any means without prior written
 * permission of HUMAX Inc.
 ******************************************************************************/




#ifndef _DLNA_HEADER_INCLUDED_123_
#define _DLNA_HEADER_INCLUDED_123_
// ---------------------------------------
#include "mxlib_config.h"
// ---------------------------------------

#ifdef __cplusplus
extern "C"{
#endif


#include "mxtypes.h"
#include "mxlog.h"
#include "mxdlna_struct.h"
#include "mxdlna_define.h"

#include "mxutil.h"

typedef char *STRING_CONVERT_CALLBACK(char * src, int srcLen, int * dstLen);

__MXAPI
MX_INT32 mxDlna_start(void);

__MXAPI
MX_INT32 mxDlna_addUserAgent(char *name);

__MXAPI
MX_INT32 mxDlna_setCpFriendlyName(char *name);

__MXAPI
MX_INT32 mxDlna_stop(void);

__MXAPI
MX_INT32 mxDlna_ping(char* ip4v);


__MXAPI
char *mxDlna_protocolinfoSerialize( MXDLNA_PROTOCOLINFO * info );
__MXAPI
void mxDlna_protocolinfoStrFree( void *info );
__MXAPI
MXDLNA_PROTOCOLINFO *mxDlna_protocolinfoParser(char *protocolInfo, int protocolInfoLength);
__MXAPI
void mxDlna_protocolinfoDestruct(MXDLNA_PROTOCOLINFO *protocolInfo);
__MXAPI
char* mxDlna_getProtocolByPath(char* path );


// 20101104 - performance 제어를 위한 함수
// 호출 시점은 :  DLNA 모듈 초기화 전
// 입력 매개 변수 :
//		- NULL : 설정하지 않음
//		- pUpload_delay_ms			- upload 시 delay 설정 : 느린 환경의 경우 이 값이 0이면 CPU 점유율이 100프로가 될 수 있음
//										너무 지연시간을 많이 줄경우 업로드 속도가 느려짐
//		- pbBrowse_Reentrant_block	- Browse or Search() 함수의 재진입을 Block 시킬지 유무 :
//										느린 환경의 경우 비동기적으로 Response를 확인하지 않고 호출 할 경우 CPU 점유율이 높아짐
// 리턴 값 : MXDLNA_OK or MXDLNA_ERROR
__MXAPI
int mxDlna_setConfigPerformance(
	int * pDmsUpload_delay_ms ,
	int * pbDmsBrowse_Reentrant_block,
	int * pbDescriptionDataSend_delay_ms,
	int * pbDmsUploadProtocolInfoCheck
);

// 설정된 값을 확인 하기 위한 함수
// 리턴 값 :
//				NULL : 설정되지 않았음
//				구조체 안의 값이 NULL 인 경우 해당 값에 대한 설정이 되지 않았음
__MXAPI
MXDLNA_PERFORMANCE_CONFIG * mxDlna_getConfigPerformance(void);

// 호출 시점은 :  DLNA 모듈 종료 뒤
// 메모리 해제
__MXAPI
int mxDlna_delConfigPerformance(void);


//====================================================
// Unicode
//====================================================
__MXAPI
int mxDlna_setSystemCharacterSet(
	MXDLNA_CODE_PAGE systemCodePage,
	MXDLNA_DMS_LIB_TO_SYS fLib2Sys ,
	MXDLNA_DMS_SYS_TO_LIB fSys2Lib
);
__MXAPI
int mxDlna_setAppCharacterSet(
	MXDLNA_CODE_PAGE appCodePage,
	MXDLNA_DMS_LIB_TO_APP fLib2App,
	MXDLNA_DMS_APP_TO_LIB fApp2Lib
);

// hoonie for iPhone
__MXAPI
void *mxDlna_getUserDataOfCP( void *cp );

__MXAPI
void mxDlna_setUserDataOfCP( void *cp , void *data );

__MXAPI
MXDLNA_UNICODE_DATA * mxDlna_getCharaterStruct(void);
//====================================================


// 20120315
//__MXAPI
//char * mxDlna_getStringValueFromNode(struct ILibXMLNode *node, char * pStrNamespace , int szNamespaceLen, int bCheckNS);
//__MXAPI
//long mxDlna_getLongValueFromNode(struct ILibXMLNode *node, char * pStrNamespace , int szNamespaceLen, int bCheckNS);

__MXAPI
char * mxDlna_getStringValueFromNode(void *pNode, char * pStrNamespace , int szNamespaceLen, int bCheckNS);
__MXAPI
long mxDlna_getLongValueFromNode(void *pNode, char * pStrNamespace , int szNamespaceLen, int bCheckNS);


#ifdef MXCORE_DISABLE_IP_SUPPORT
__MXAPI
int mxDlna_setDisableIP(MXDLNA_DISABLE_IP *disable_ip_array, int cnt);
#endif


typedef int	(*MXDLNA_NIC_CB_GET_IP_ADDRESS)		(int **pp_int);
typedef int	(*MXDLNA_NIC_CB_GET_MAC_ADDRESS)	(char **pp_char);
typedef void (*MXDLNA_NIC_CB_REGISTER_EVENT)	(void *user_data);
typedef int	(*MXDLNA_NIC_CB_GET_BEST_IP_ADDRESS)	(int local_IP, int default_IP, int *pList, int cnt);


__MXAPI
MX_RETURN mxDlna_setDefine(const char *define_name);

__MXAPI
MX_RETURN mxDlna_unsetDefine(const char *define_name);

__MXAPI
MX_RETURN mxDlna_existDefine(const char *define_name);

__MXAPI
MX_RETURN mxDlna_NIC_create(MXDLNA_NIC_CB_GET_IP_ADDRESS cb_ip_address, MXDLNA_NIC_CB_GET_MAC_ADDRESS cp_mac_address);

__MXAPI
MX_RETURN mxDlna_NIC_destroy(void);

__MXAPI
MX_RETURN mxDlna_NIC_registerEvent(MXDLNA_NIC_CB_REGISTER_EVENT callback, void *user_data);

__MXAPI
MX_RETURN mxDlna_NIC_unRegisterEvent(MXDLNA_NIC_CB_REGISTER_EVENT callback);

__MXAPI
MX_RETURN mxDlna_NIC_change(void);

__MXAPI
MX_RETURN mxDlna_NIC_getLocalIPAddressList(int **pp_ip, int *cnt, int **pp_access_ip, int *access_cnt);

__MXAPI
MX_RETURN mxDlna_NIC_checkFirstTime(int *p_ip, int cnt, int *p_access_ip, int access_cnt);

__MXAPI
MX_RETURN mxDlna_bootid_setPath(const char *path);

__MXAPI
unsigned long mxDlna_bootid_getBootId(void);

__MXAPI
MX_RETURN mxDlna_setOSversion(const char *szOSversion);

__MXAPI
char* mxDlna_getOSversion(void);

#ifdef __cplusplus
}
#endif

#endif


