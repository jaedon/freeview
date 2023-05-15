/**************************************************************************************/
/**
 * @file 		mxdlna_userdevice.h
 *
 * defines		USER Device APIs
 *
 * @author		Minkyu Lim (mklim@humaxdigital.com)
 * @date		2013/10/28
 * @attention	Copyright (c) 2013 Humax - All rights reserved.
 *
 **/
/**************************************************************************************/

#ifndef __MXUSER_DEVICE_H__
#define __MXUSER_DEVICE_H__

#ifdef __cplusplus
extern "C"{
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
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */
typedef void *	MXUSER_DEVICE_HANDLE;


typedef struct _MXUSER_DEVICECAP
{
	const char *Namespace;			// Device Description에서 XML로 표현할 때 사용할 Namespace
	const char *NamespacePrefix;	// <NamespacePrefix:AttributeName>Value</NamespacePrefix:AttributeName>
	const char *AttributeName;		// <NamespacePrefix:AttributeName>Value</NamespacePrefix:AttributeName>
	const char *Value;				// <NamespacePrefix:AttributeName>Value</NamespacePrefix:AttributeName>
									// <hmx:X_HMXCAP>RCUTYPE_LEGACY</hmx:X_HMXCAP>

	struct _MXUSER_DEVICECAP *next;	// linked list 형태로 여러개의 capability 설정 가능
}MXUSER_DEVICECAP;

typedef struct _MXUSER_EXTERNAL_SEARCHTARGET_INFO
{
	const char *SearchTarget;			// M-Search의 ST값, notify의 NT 값에 사용된다. ex: urn:rc-freetime-tv:service:freetimerc:1, digiturk-stb:webservice
	const char *UDN;					// UDN 값 설정
	const char *ipAddress;				// M-Search 및 notify의 LOCATION URL 상의 ipAddress를 설정한다.
										// NULL로 설정할 경우 library가 관리한다. 일반적으로 NULL로 설정함
	unsigned short portNumber;			// LOCATION의 URL 상의 portNumber 설정
	const char *relativeURL;			// LOCATION의 URL 상의 상대 주소를 설정, "/" 로 시작함. ex: "/", "/rc"
	int bSecureAccess;					// 1일 경우 LOCATION URL이 https 로 설정 됨, 0일 경우 http로 설정 됨

	const char *ServerExt;				// SSDP notification / M-Search resp. 의 SERVER: 값에 추가 되는 값 설정
	unsigned long ConfigId;				// Device Description의 초기 ConfigId 값을 설정 (0보다 큰 값이어야 함)

	const char *AdditionalField;		// User defined additional field ex: Name: STB-0001

	struct _MXUSER_EXTERNAL_SEARCHTARGET_INFO *next;
}MXUSER_EXTERNAL_SEARCHTARGET_INFO;

//USER Device 서버 정보로, 이를 통해 설정된 값을 사용하여 ControlPoint가 서버를 검색할 때 서버 정보로 알려준다.
typedef struct _MXUSER_DEVICE_INFO
{
	const char *devicetype;				// User device type을 설정
	const char *friendlyName;				// ControlPoint에서 해당 서버를 지칭하는 이름
	const char *UDN;						// UPnP 장치들을 구별하기 위한 이름
	const char *serialNumber;				// 장치의 시리얼 번호

	const char *manufacturer;			// 제조사
	const char *manufacturerURL;			// 제조사의 설명이 들어 있는 페이지의 URL
	const char *modelName;				// 제품의 모델 이름
	const char *modelDescription;		// 모델에 대한 세부 설명
	const char *modelURL;				// 모델에 대한 더 자세한 설명이 들어 있는 페이지의 URL
	
	const char *corporation;				// 회사이름 
	const char *modelNumber;				// 모델에 대한 넘버
	const char *productCode;				// 제품 코드

	int  bDialServer;			// 1 일 경우 DIAL Server on, 0 일 경우 off
	const char *applicationURL;		// DIAL REST Service 에 사용되는 URL
								// Device description response header의 Application-URL의 값으로 사용 됨

	unsigned short upnp_port;		// User device의 UPnP 관련 통신을 할 때 사용하는 port
									// 라이브러리 내부에서 모든 동작이 처리
									// 보통은 0값을 주어 임의로 설정하도록 하면되고, 특정 포트만 사용해야 하는 환경에서는 이 값을 지정해 주면 됨
	
	MXUSER_DEVICECAP *pUserDeviceCapability;	// linked list 형태로 여러개의 capability 설정 가능

	MXUSER_EXTERNAL_SEARCHTARGET_INFO *pExtSTInfo;		// External webserver에 대한 SSDP 정보 설정 (여러개 설정 가능)

}MXUSER_DEVICE_INFO_t;

/* End typedef */



/*******************************************************************/
/*********************** Function Description **********************/
/*******************************************************************/
/**
 * @name			mxUserDevice_createDevice
 * @brief			새로운 USER Device 객체를 생성
 *
 * @param[in]		pstInfo		- USER Device 생성 시 필요한 정보
 *								  내부에서 본 구조체 포인터에 대한 메모리 해제를 하지 않는다.
 *								  본 함수 호출자가 메모리 해제하여야 한다.
 * @return			USER Device 객체가 정상 생성 되었을 경우 device handle을 반환, 그렇지 않을 경우 NULL 반환
 */
__MXAPI 
MXUSER_DEVICE_HANDLE mxUserDevice_createDevice (
	MXUSER_DEVICE_INFO_t *pstInfo
);

/**
 * @name			mxUserDevice_startDevice
 * @brief			UPnP user device를 시작
 *
 * @param[in]		handle		- mxUserDevice_createDevice() 함수로 생성한 user device handle
 *
 * @return			[ MX_RETURN_OK | MX_RETURN_FAILED ]
 */
__MXAPI
MX_RETURN mxUserDevice_startDevice (
	MXUSER_DEVICE_HANDLE handle
);

/**
 * @name			mxUserDevice_destroyDevice
 * @brief			USER Device 객체를 종료 및 해제
 *
 * @param[in]		handle		- mxUserDevice_createDevice() 함수로 생성한 user device handle
 *
 * @return			[ MX_RETURN_OK | MX_RETURN_FAILED ]
 */
__MXAPI
MX_RETURN mxUserDevice_destroyDevice (
	MXUSER_DEVICE_HANDLE handle
);

/**
 * @name			mxUserDevice_setFriendlyName
 * @brief			FriendlyName을 변경
 *
 * @param[in]		handle			- mxUserDevice_createDevice() 함수로 생성한 user device handle
 * @param[in]		szFriendlyName	- 새로 설정할 user device의 friendlyName string
 *
 * @return			[ MX_RETURN_OK | MX_RETURN_FAILED ]
 */
__MXAPI
MX_RETURN mxUserDevice_setFriendlyName( 
	MXUSER_DEVICE_HANDLE handle,
	const char *szFriendlyName 
);

/**
 * @name			mxUserDevice_setApplicationURL
 * @brief			DIAL REST Service를 위한 통신을 할 때 사용하는 Application-URL을 변경
 *					DIAL on 상태에서만 동작 (Device 생성 시 bDialServer == 1)
 *
 * @param[in]		handle			- mxUserDevice_createDevice() 함수로 생성한 user device handle
 * @param[in]		applicationURL	- DIAL REST Service를 위한 통신을 할 때 사용하는 URL
 *									  DIAL client가 device description 요청 시, server response에 포함되는 Application-URL에 사용 됨
 *
 * @return			[ MX_RETURN_OK | MX_RETURN_FAILED ]
 */
__MXAPI
MX_RETURN mxUserDevice_setApplicationURL(
	MXUSER_DEVICE_HANDLE handle,
	const char *szApplicationURL
);

/**
 * @name			mxUserDevice_setIPAddress
 * @brief			External webserver에 대한 SSDP message에 포함되는 LOCATION 을 변경 (아래 예시 참조)
 *					
 *					NOTIFY * HTTP/1.1
 *					LOCATION: https://[192.168.0.6 ==> 100.100.100.100]:56799/rc
 *					HOST: 239.255.255.250:1900
 *					SERVER: POSIX UPnP/1.1
 *					NTS: ssdp:alive
 *					USN: uuid:0E96FF2A-6771-4F44-A0C5-0A1B0A0D1CCC::urn:rc-freetime-tv:service:freetimerc:1
 *					CACHE-CONTROL: max-age=1800
 *					NT: urn:rc-freetime-tv:service:freetimerc:1
 *					Content-Length: 0
 *					
 *
 * @param[in]		handle		- mxUserDevice_createDevice() 함수로 생성한 user device handle
 * @param[in]		ipAddress	- 새로 변경 할 ipAddress
 * @param[in]		deviceType	- 변경 대상 (User device 시작 시 설정한 search target)
 *
 * @return			[ MX_RETURN_OK | MX_RETURN_FAILED ]
 */
__MXAPI
MX_RETURN mxUserDevice_setIPAddress(
	MXUSER_DEVICE_HANDLE handle,
	const char *szIpAddress,
	const char *szSearchTarget
);

/**
 * @name			mxUserDevice_setConfigId
 * @brief			External webserver에 대한 SSDP message에 포함되는 CONFIGID.UPNP.ORG 값을 변경
 *					
 * @param[in]		handle		- mxUserDevice_createDevice() 함수로 생성한 user device handle
 * @param[in]		ConfigId	- 새로 변경 할 ConfigId
 * @param[in]		szSearchTarget	- 변경 대상 (User device 시작 시 설정한 search target)
 *
 * @return			[ MX_RETURN_OK | MX_RETURN_FAILED ]
 */
__MXAPI
MX_RETURN mxUserDevice_setConfigId( 
	MXUSER_DEVICE_HANDLE handle,
	unsigned long ConfigId,
	const char *szSearchTarget
);

/**
 * @name			mxUserDevice_setAdditionalField
 * @brief			External webserver에 대한 SSDP message에 포함되는 AdditionalField 를 변경
 *					이 함수를 호출할 경우 변경된 AdditionalField에 대한 SSDP re-notification이 이루어짐
 *					
 * @param[in]		handle		- mxUserDevice_createDevice() 함수로 생성한 user device handle
 * @param[in]		szAdditionalField	- 변경 할 additionalField
 * @param[in]		szSearchTarget	- 변경 대상 (User device 시작 시 설정한 search target)
 *
 * @return			[ MX_RETURN_OK | MX_RETURN_FAILED ]
 */
__MXAPI
MX_RETURN mxUserDevice_setAdditionalField(
	MXUSER_DEVICE_HANDLE handle,
	const char *szAdditionalField,
	const char *szSearchTarget
);

#ifdef __cplusplus
}
#endif
#endif
/*********************** End of File ******************************/