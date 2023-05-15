/**************************************************************************************/
/**
 * @file 		mxdial_server.h
 *
 * defines		DIAL Server APIs
 *
 * @author		Minkyu Lim (pn_mklim@humaxdigital.com)
 * @date		2013/07/10
 * @attention	Copyright (c) 2013 Humax - All rights reserved.
 *
 **/
/**************************************************************************************/

#ifndef __MXDIAL_SERVER_H__
#define __MXDIAL_SERVER_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#if defined(WIN32) || defined(_WIN32_WCE)
#if defined(WINSOCK2)
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(WINSOCK1)
#include <winsock.h>
#include <wininet.h>
#endif
#endif

#include "mxapi_define.h"
#include "mxtypes.h"
/* End Including Headers */

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variable */
/* End Extern variable */

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
/* Start Macro definition */
/* End Macro definition */

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef void * MXDIAL_SERVER_HANDLE;

//DIAL Server 서버 정보로, 이를 통해 설정된 값을 사용하여 ControlPoint가 서버를 검색할 때 서버 정보로 알려준다.
typedef struct _MXDIAL_SERVER_INFO
{
	char *friendlyName;				// ControlPoint에서 해당 서버를 지칭하는 이름
	char *UDN;						// UPnP 장치들을 구별하기 위한 이름
	char *serialNumber;				// 장치의 시리얼 번호

	char *manufacturer;			// 제조사
	char *manufacturerURL;			// 제조사의 설명이 들어 있는 페이지의 URL
	char *modelName;				// 제품의 모델 이름
	char *modelDescription;		// 모델에 대한 세부 설명
	char *modelURL;				// 모델에 대한 더 자세한 설명이 들어 있는 페이지의 URL

	char *corporation;				// 회사이름 
	char *modelNumber;				// 모델에 대한 넘버
	char *productCode;				// 제품 코드

	char *applicationURL;			// DIAL REST Service 할때 사용되는 URL
									// Device description의 response에 Application-URL의 값으로 사용 됨

	unsigned short upnp_port;	// DIAL Discovery Service를 위한 통신을 할 때 사용하는 port
								// 라이브러리 내부에서 모든 동작이 처리
								// 보통은 0값을 주어 임의로 설정하도록 하면되고, 특정 포트만 사용해야 하는 환경에서는 이 값을 지정해 주면 됨

} MXDIAL_SERVER_INFO_t;
/* End typedef */

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
/**
 * @name			mxDialServer_createServer
 * @brief			새로운 DIAL Server 객체를 생성
 *
 * @param[in]		pstInfo		- DIAL Server 생성 시 필요한 정보
 *								  내부에서 본 구조체 포인터에 대한 메모리 해제를 하지 않는다.
 *								  본 함수 호출자가 메모리 해제하여야 한다.
 * @return			DIAL Server 객체가 정상 생성 되었을 경우 server handle을 반환, 그렇지 않을 경우 NULL 반환
 */
__MXAPI
MXDIAL_SERVER_HANDLE mxDialServer_createServer(
		MXDIAL_SERVER_INFO_t *pstInfo
		);

/**
 * @name			mxDialServer_startServer
 * @brief			DIAL Discovery Service를 시작
 *
 * @param[in]		handle		- mxDialServer_createServer() 함수로 생성한 Dial server handle
 *
 * @return			
 */
__MXAPI
void mxDialServer_startServer(MXDIAL_SERVER_HANDLE handle);

/**
 * @name			mxDialServer_setFriendlyName
 * @brief			
 *
 * @param[in]		handle			- mxDialServer_createServer() 함수로 생성한 Dial server handle
 * @param[in]		szFriendlyName	- 새로 설정할 DIAL server의 friendlyName string
 *
 * @return			
 */
__MXAPI
void mxDialServer_setFriendlyName(MXDIAL_SERVER_HANDLE handle, const char *szFriendlyName);

/**
 * @name			mxDialServer_destroyServer
 * @brief			DIAL Server 객체를 종료 및 해제
 *
 * @param[in]		handle		- mxDialServer_createServer() 함수로 생성한 Dial server handle
 *
 * @return			
 */
__MXAPI
void mxDialServer_destroyServer(
		MXDIAL_SERVER_HANDLE handle
		);

/**
 * @name			mxDialServer_setApplicationURL
 * @brief			DIAL REST Service를 위한 통신을 할 때 사용하는 Application-URL을 변경
 *
 * @param[in]		handle		- mxDialServer_createServer() 함수로 생성한 Dial server handle
 * @param[in]		applicationURL	- DIAL REST Service를 위한 통신을 할 때 사용하는 URL
 *									  라이브러리 내부에서 처리하지 않음
 *									  DIAL client가 device description 요청 시, server response에 포함되는 Application-URL에 사용 됨
 *
 * @return			
 */
__MXAPI
void mxDialServer_setApplicationURL(
		MXDIAL_SERVER_HANDLE handle,
		char *applicationURL
		);

__MXAPI
void mxDialServer_setAdditionalSSDPHeaderFields(
		MXDIAL_SERVER_HANDLE handle,
		const char * additionalSSDPHeaderFields
		);

	
#ifdef __cplusplus
}
#endif
#endif
/*********************** End of File ******************************/
