/**
  @file  mxdlna_dmc.h
  @brief defines DMC API

  @mainpage
  mxlib/dmc 모듈에서 노출하는 API에 대해서 설명한다.<br>
  doxygen 작성시 mxlib 소스코드 :  http://svn/ROMA/library/internal_library/ROMA rev 1955 <br>
  DMC API가 변경되면 반드시 doxygen 내용을 업데이트하도록 한다.

  @author mhkang2@humaxdigital.com
  @date 2014/9/19
  @attention Copyright (C) 2014 Humax - All rights reserved.
*/

#ifndef MXDLNA_MXDLNA_DMC_H
#define MXDLNA_MXDLNA_DMC_H

#include "mxapi_define.h"

#ifdef __cplusplus
extern "C"{
#endif

#include "mxdlna_define.h"
#include "mxdlna_mrcp.h"
#include "mxdlna_mscp.h"

#ifdef _ANDROID
	// current 
	//#define _UPLOAD_FILE_DETECT
#endif

#if !defined MXDLNA_DOWNLOAD_MODULE && !defined MXDLNA_UPLOAD_MODULE
//#define MXDLNA_DOWNLOAD_MODULE
#define MXDLNA_UPLOAD_MODULE
#endif

#define MXDLNA_PLAYSING_MODULE
#define MXDLNA_PLAYCONTAINER_MODULE
#define MXDLNA_SUBTITLE

#ifdef IPHONE_OS
#undef MXDLNA_DOWNLOAD_MODULE
// #undef MXDLNA_UPLOAD_MODULE
#endif

#ifndef USER_TYPE_STRING
#ifdef	_WIN32_WCE
#define USER_TYPE_STRING wchar_t
#else 
#define USER_TYPE_STRING char
#endif
#endif

typedef void* MXDMC_HANDLE; 
typedef void *MXDMC_OBJECT_HANDLE;

/** @enum _MXDMC_DEVICE_TYPE
  Device종류
 */
typedef enum _MXDMC_DEVICE_TYPE {
	DMC_UNKOWN_TYPE				= MXDLNA_UNKOWN_TYPE,
	DMC_MEDIA_DEVICE_TYPE		= MXDLNA_MEDIA_DEVICE_TYPE,
	DMC_SERVER_TYPE				= MXDLNA_SERVER_TYPE,
    DMC_RENDERER_TYPE			= MXDLNA_RENDERER_TYPE,
    DMC_USER_DEVICE_TYPE		= MXDLNA_USER_DEVICE_TYPE
} MXDMC_DEVICE_TYPE;


typedef struct _MXDMC {	

	MXDLNA_CP_HANDLE MSCP;
	MXDLNA_CP_HANDLE MRCP;
	MSCP_ACTION_HANDLE MSCPA;
	MRCP_ACTION_HANDLE MRCPA;

    MXDLNA_CP_HANDLE    USERCP;
    MSCP_ACTION_HANDLE USERCPA;

	void *objectHandle;
	void *userData;

	void *pNext;
	
} MXDMC_DATA;

typedef struct _MXDMC_TAG {
	void *handle;
	int	CMID;
} MXDMC_TAG;

typedef struct _MXDLNA_DEVICE_URL {
	int UrlCnt;
	char **Url;
} MXDLNA_DEVICE_URL;

//------------------------------------------------------------------------
// @ DMC 생성 소멸 관련,
//------------------------------------------------------------------------
/**
  DMC를 생성전 DMS, DMR 또는 User Device CP의 환경설정(주로 응답받을 Callback함수 등록)을 합니다.
  <pre>
  MXDLNA_CP_CONFIG::DeviceAddOrRemove : Device가 Network에 Add되거나 Remove되었을 때 호출되는 Callback함수.
  MXDLNA_CP_CONFIG::DeviceResponse    : Device/Service/Action이 호출되었을 때 Device으로부터 Response 받았을 때 호출되는 Callback함수.
  MXDLNA_CP_CONFIG::DeviceEvent       : Device에서 Event를 보냈을 때 호출되는 Callback함수.
  MXDLNA_CP_CONFIG::user_make         : 사용자 정의 CDS를 생성할 때 호출되는 Callback함수.
  MXDLNA_CP_CONFIG::user_free         : 사용자 정의 CDS의 메모리를 해제할 때 호출되는 Callback함수.
  </pre>
  @param  serverConf
  @param  rendererConf
  @return N/A
*/
__MXAPI void mxDmc_configure(MXDLNA_CP_CONFIG *serverConf, MXDLNA_CP_CONFIG *rendererConf);	// DMC를 생성전 환경설정을 합니다.

/**
  DMS, DMR, UserDevice CP의 환경설정을 해제합니다.
  @param hDmc mxDmc_create 호출후 반환 받은 핸들값.
  @return N/A
*/
__MXAPI void mxDmc_unConfigure(MXDMC_HANDLE hDmc);						// 환경설정 값을 초기화 합니다.

#ifdef _ANDROID
// 안드로이드에서 MXDMC_HANDLE 구조체에 안드로이드에서 필요한 데이터를 붙이기 위한 용도로 사용됨.
__MXAPI int mxDmc_createWithUserData(MXDMC_HANDLE hDmc);				// DMC를 생성합니다.
__MXAPI void mxDmc_destroyWithUserData(MXDMC_HANDLE hDmc);				// DMC를 종료후에 제거합니다.
#endif

/**
  DMC 핸들을 생성합니다(내부적으로는 MRCP, MSCP, UserCP(옵션)들이 생성됩니다).
  UserCp는 MXDLNA_CP_CONFIG::UserDeviceTypeName 가 설정되어 있는 경우에만 내부적으로 생성됩니다.
  @return MXDMC_HANDLE 값이 반환됩니다(실제 type은 MXDMC_DATA입니다).
*/
__MXAPI MXDMC_HANDLE mxDmc_create();									// DMC를 생성합니다.

/**
  DMC핸들을 정리합니다.
  @param hDmc MXDMC_HANDLE 핸들값.
  @return N/A
*/
__MXAPI void mxDmc_destroy(MXDMC_HANDLE hDmc);							// DMC를 종료후에 제거합니다.


//------------------------------------------------------------------------
// @ DMC 정보 관련
//------------------------------------------------------------------------
/**
  hDmc 핸들에 사용자의 임의의 데이터를 저장하여 mxDmc_getDmcUserTag 함수를 통하여 임의의 데이터를 갖져 올 수 있도록 합니다.
  @param hDmc MXDMC_HANDLE 핸들값.
  @param user hDmc핸들에 저장할 사용자 임의의 데이타.
  @return 성공 1, 에러 0
*/
__MXAPI int mxDmc_setDmcUserTag(MXDMC_HANDLE hDmc, void *user);			// DMC 핸들에 USER 데이터를 저장합니다. 같은 핸들을 사용하면 같은 데이터가 반환됩니다.

/**
  mxDmc_setDmcUserTag 호출하여 저장한 사용자 임의의 데이터를 가져옵니다.
  @param hDmc MXDMC_HANDLE 핸들값.
  @return 성공하면 사용자의 임의의 데이터, 에러시 NULL.
*/
__MXAPI MXDLNA_USER_TAG mxDmc_getDmcUserTag(MXDMC_HANDLE hDmc);			// DMC 핸들에 저장된 USER 데이터를 불러옵니다. 같은 핸들을 사용하면 같은 데이터가 반환됩니다.

/**
  hDmc 핸들로부터 MXDLNA_CP_HANDLE 타입의 MSCP 핸들을 얻어온다.
  @param hDmc MXDMC_HANDLE 핸들
  @return MXDLNA_CP_HANDLE 핸들이 반환되고 실패시 NULL 반환된다.
*/
__MXAPI void *mxDmc_getMscpHandle(MXDMC_HANDLE hDmc);					// MSCP 핸들을 받아옵니다.

/**
  hDmc 핸들로부터 MXDLNA_CP_HANDLE 타입의 MRCP 핸들을 얻어온다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return MXDLNA_CP_HANDLE 핸들이 반환되고 실패시 NULL 반환된다.
*/
__MXAPI void *mxDmc_getMrcpHandle(MXDMC_HANDLE hDmc);					// MSCP 핸들을 받아옵니다.

/**
  MSCP 핸들로부터 MXDMC_HANDLE 핸들을 얻어온다.
  @param hMscp MXDLNA_CP_HANDLE 타입의 핸들.
  @return MXDMC_HANDLE 핸들. 실패서 NULL이 반환된다.
*/
__MXAPI void *mxDmc_getMscpHandle2DmcHandle(MXDLNA_CP_HANDLE hMscp);	// MSCP 핸들을 주면 DMC 핸들을 찾아주는 함수입니다.

/**
  MRCP 핸들로부터 MXDMC_HANDLE 핸들을 얻어온다.
  @param hMrcp MXDLNA_CP_HANDLE 타입의 핸들.
  @return MXDMC_HANDLE 핸들. 실패서 NULL이 반환된다.
*/
__MXAPI void *mxDmc_getMrcpHandle2DmcHandle(MXDLNA_CP_HANDLE hMrcp);	// MRCP 핸들을 주면 DMC 핸들을 찾아주는 함수입니다.

/**
  MSCP 또는 MRCP 핸들로부터 MXDMC_HANDLE 핸들을 얻어온다.
  @param hCP MXDLNA_CP_HANDLE 타입의 핸들.
  @return MXDMC_HANDLE 핸들. 실패서 NULL이 반환된다.
*/
__MXAPI void *mxDmc_getCpHandle2DmcHandle(MXDLNA_CP_HANDLE hCp);		// MRCP 또는 MSCP 핸들을 주면 DMC 핸들을 찾아주는 함수입니다.

/**
  hDmc핸들안의 MXDLNA_ACTION_DATA 구조체의 udn 필드에 UDN 값을 복사합니다.
         사용할 장치를 설정할때 사용합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE 값중에 하나를 선택한다.
  @param UDN Device의 UDN String.
  @return 성공하면 1, 실패하면 0 값이 반환된다.
*/
__MXAPI int mxDmc_setUDN(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, MXDLNA_UDN UDN);

/**
  MXDMC_HANDLE핸들로부터 MXDLNA_ACTION_DATA 핸들의 UDN 값을 얻어온다.
        현재 사용중인 장치를 가져올때 사용합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE 값중에 하나를 선택한다.
  @return 성공하면 UDN string, 실패하면 NULL이 반환된다. 반환된 포인터는 메모리해제하면 안된다.
*/
__MXAPI char *mxDmc_getUDN(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type);

/**
  MXDLNA_ACTION_DATA 핸들로부터 UDN 값을 얻어온다.         
  @param hAction MXDLNA_ACTION_HANDLE 핸들.
  @return 성공하면 UDN string, 실패하면 NULL이 반환된다. 반환된 포인터는 메모리 해제하면 안된다.
*/
__MXAPI char *mxDmc_getActionUDN(MXDLNA_ACTION_HANDLE hAction);

/**  
  MSCP의 MSCP_ACTION_HANDLE 또는 MRCP의 MRCP_ACTION_HANDLE에 ConnectionID 값을 저장합니다.
  @param hDmc MXDMC_HANDLE 핸들
  @param type  DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @param CMID ConnectionID. 기본 값 0.
  @return 성공하면 1, 실패시 0.
*/
__MXAPI int mxDmc_setCMID(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, MXDLNA_CMID CMID);

/**  
  MSCP의 MSCP_ACTION_HANDLE 또는 MRCP의 MRCP_ACTION_HANDLE에 ConnectionID 값을 가져옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type  DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @return 성공하면 Connection ID를 반환하고 실패시 -1를 반합니다.
  @remarks TO DO: 실패시 0 을 반환하도록 구현되어 있는데 수정해야 함.
*/
__MXAPI int mxDmc_getCMID(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type);

/**
  Action의 Respose로 반환된 Action Hanlde를 통해 Invoke를 수행한 Connection ID를 알고자 할때 사용합니다.
  @param hAction MXDLNA_ACTION_HANDLE 핸들.
  @return 성공하면 Connection ID를 반환하고 실패시 -1를 반합니다.
  @remarks TO DO: 실패시 0 을 반환하도록 구현되어 있는데 수정해야 함.
*/
__MXAPI int mxDmc_getActionCMID(MXDLNA_ACTION_HANDLE hAction);

/**
  Action을 Invoke하기 전에 설정하고 Respose를 통해 받는 user 데이터입니다. Action에 따라 다른 데이터를 가질수 있습니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @param user 사용자 정의 임의의 데이타.
  @return 성공하면 1, 실패시 0 을 반환한다.
*/
__MXAPI int mxDmc_setActionUserTag(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, MXDLNA_USER_TAG user);

/**
  현재 설정된 Action의 사용자 임의의 데이타를 갖고 올 때 사용합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @return 성공하면 사용자 임의의 데이타를 반환하고 실패시 NULL를 반환한다.
*/
__MXAPI MXDLNA_USER_TAG mxDmc_getActionUserTag(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type);

/**
  Action의 Respose로 반환된 Action Handle를 통해 기존에 저장된 User Tag를 확인할때, 사용합니다.
  @param hAction MXDLNA_ACTION_HANDLE 핸들.
  @return 성공하면 사용자 임의의 데이타를 반환하고 실패시 NULL를 반환한다.
*/
__MXAPI MXDLNA_USER_TAG mxDmc_getActionUserTagFromHandle(MXDLNA_ACTION_HANDLE hAction);

/**
  사용자 정의 CDS Object로부터 CDS Meta Data  XML String을 받아옵니다.
  @param appData  make_app_data 함수 두번째 파라미터에서 사용되는 사용자 정의 CDS object 구조체
  @return <DIDL> XML String이 반환되고 실패시 NULL이 반환된다.
*/
__MXAPI char *mxDmc_getMetaString(void * appData);


//------------------------------------------------------------------------
// @ DMC Device 관련
//------------------------------------------------------------------------

/**
  동일 네트워크상에서 1900 port로 M-SEARCH multicasting합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE 값중에 하나를 선택한다.
  @return N/A
*/
__MXAPI void mxDmc_msearch(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type);														// 디바이스를 검색합니다.

/**
  Device 리스트를 갖고 옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE 값중에 하나를 선택한다.
  @param CompareUDN [deprecated]사용되지 파라미터입니다. NULL.
  @return 성공하면 linked list형태의 MXDLNA_DEVICE_TINY 포인터를 반환하고 실패시 NULL을 반환합니다.
          반환된 포인터는 mxDmc_freeDeviceList 함수를 사용하여 메모리 해제해야 합니다.
*/
__MXAPI MXDLNA_DEVICE_TINY *mxDmc_getDeviceList(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, char *CompareUDN);

/**
  mxDmc_getDeviceList 함수로 가져온 MXDLNA_DEVICE_TINY 구조체 메모리를 해제합니다.
  @param list MXDLNA_DEVICE_TINY구조체의 linked list.
  @return N/A
*/
__MXAPI void mxDmc_freeDeviceList(MXDLNA_DEVICE_TINY *list);

/**
  선택된 Device를 지정합니다.
  각 Deivce Service Action을 사용하기 전에 mxDmc_initDevice 함수가 각각 호출되어야 합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE 값중에 하나를 선택한다.
  @param UDN 선택할 Device의 UDN String.
  @return 성공하면 MXDLNA_OK 실패하면 MXDLNA_ERROR 를 반환합니다.
*/
__MXAPI int mxDmc_initDevice(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, MXDLNA_UDN UDN);

/**
  type = DMC_RENDERER_TYPE 인 경우는 DMR의 ConnectionManager Service의 Sink Protocol Info 정보를 갖고 오고
  type = DMC_SERVER_TYPE 인 경우는 DMS의 ConnectionManager Service의 Source Protocol Info 정보를 갖고 옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @param udn Device의 UDN String.
  @return MXDLNA_DEVICE_PROTOCOL_INFO 구조체 포인터를 반환하고 실패시 NULL를 반환합니다.
*/
__MXAPI MXDLNA_DEVICE_PROTOCOL_INFO *mxDmc_getDeviceProtocolInfo(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, MXDLNA_UDN udn);

/**
  mxDmc_getDeviceProtocolInfo 함수를 호출하여 갖고 온 MXDLNA_DEVICE_PROTOCOL_INFO 구조체 포인터의 메모리를 해제합니다.
  @param ptcInfo MXDLNA_DEVICE_PROTOCOL_INFO구조체 포인터
  @return N/A
*/
__MXAPI void mxDmc_unloadDeviceProtocolInfo(MXDLNA_DEVICE_PROTOCOL_INFO  *ptcInfo);

/**
  device가 살아있는지 확인합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE 값중에 하나를 선택한다.
  @param udn Device의 UDN String.
  @return live: MXCP_TRUE, dead: MXCP_FALSE, error:MXCP_ERROR_VALUE
  @remarks TO DO: return 값이 올바르게 구현되어 있지 않음(수정 필요함).
*/
__MXAPI int mxDmc_isAliveDevice(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, MXDLNA_UDN udn);

/**
  Device에 serviceName 이름의 Service가 있는 지 확인합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE 값중에 하나를 선택한다.
  @param serviceName Device Description내의 "serviceid" 값을 사용한다. 예) "urn:upnp-org:serviceId:ContentDirectory"
  @return exist: MXCP_TRUE, not exist: MXCP_FALSE, error:MXCP_ERROR_VALUE
  @remarks TO DO: return 값이 올바르게 구현되어 있지 않음(수정 필요함).
*/
__MXAPI int mxDmc_isDeviceService(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, char *serviceName);

/**
  Device/Service에 actioName의 Action이 있는 지 확인합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE 값중에 하나를 선택한다.
  @param serviceName Device Description내의 "serviceid" 값을 사용한다. 예) "urn:upnp-org:serviceId:ContentDirectory".
  @param actionName Action 이름.
  @return exist: MXCP_TRUE, not exist: MXCP_FALSE, error:MXCP_ERROR_VALUE.
  @remarks TO DO:return 값이 올바르게 구현되어 있지 않음(수정 필요함).
*/
__MXAPI int mxDmc_isDeviceAction(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, char *serviceName, char *actionName);

/**
  Device의 Service 정보를 얻어옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE 값중에 하나를 선택한다.
  @param serviceName Device Description내의 "serviceid" 값을 사용한다. 예) "urn:upnp-org:serviceId:ContentDirectory"
  @return 성공하면 MXDLNA_SERVICE 구조체 포인터를 반환하고 실패시 NULL를 반환합니다. 반환된 포인터는 mxDmc_freeDeviceService 함수를 사용하여 메모리 해제해야 한다.
*/
__MXAPI MXDLNA_SERVICE *mxDmc_getDeviceService(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, char *serviceName);

/**
  Device의 Service들이 갖고 있는 Action 이름을 갖고옵니다. Service에 Action이 존재하는지 확인할 때 사용합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE 값중에 하나를 선택한다.
  @param serviceName Device Description내의 "serviceid" 값을 사용한다. 예) "urn:upnp-org:serviceId:ContentDirectory"
  @param actionName  Action 이름
  @return 성공하면 MXDLNA_ACTION 구조체 포인터를 반환하고 실패시 NULL를 반환한다. 반환된 포인터는 mxDmc_freeDeviceAction 함수를 호출하여 메모리를 해제해야 한다.
*/
__MXAPI MXDLNA_ACTION *mxDmc_getDeviceAction(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, char *serviceName, char *actionName);

/**
  Device의 State Variable 정보를 갖고옵니다.
  @param hDmc MXDMC_HANDLE 핸들값
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE 값중에 하나를 선택한다.
  @param serviceName Device Description내의 "serviceid" 값을 사용한다. 예) "urn:upnp-org:serviceId:ContentDirectory"
  @param stateName  StateVariable 이름을 선택한다(Device Spy Tool에서 보면 "State variables" 폴더내의 변수 이름).
  @return 성공하면 MXDLNA_STATE_VARIABLE 구조체 포인터를 반환하고 실패시 NULL을 반환한다.  리턴한 포인터는 mxDmc_freeDeviceStateVariable 함수를 호출하여 메모리 해제해야 한다.
*/
__MXAPI MXDLNA_STATE_VARIABLE *mxDmc_getDeviceStateVariable(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, char *serviceName, char *stateName);

/**
  MXDLNA_DEVICE 구조체의 device 정보를 갖고옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE 값중에 하나를 선택한다.
  @param udn Device의 UDN String.
  @return 성공하면 MXDLNA_DEVICE 구조체 포인터를 반환하고 실패시 NULL를 반환합니다. 리턴된 포인터는 mxDmc_freeDeviceInfo 함수를 호출하여 메모리를 해제해야 한다.
  @remarks  mxDmc_freeDeviceInfo 함수 return값이 잘못 정의되어 있음. jni 부분과 확인해서 정정 필요함.
*/
__MXAPI MXDLNA_DEVICE *mxDmc_getDeviceInfo(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, MXDLNA_UDN udn);

/**
  MXDLNA_DEVICE 구조체의 메모리를 해제한다.
  예) mxDmc_getDeviceInfo 함수는 MXDLNA_DEVICE* 형태로 메모리를 할당하여 리턴하는데,  이 메모리를 해제할 때 사용한다.
  @param device MXDLNA_DEVICE* 구조체
  @return N/A
*/
__MXAPI void mxDmc_freeDeviceInfo(MXDLNA_DEVICE *device);

/**
  X_DLNACAP 값을 integer 값으로 갖고옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE 값중에 하나를 선택한다.
  @return 성공하면 DEVICE_IS_AV_UPLOAD, DEVICE_IS_IMAGE_UPLOAD, DEVICE_IS_AUDIO_UPLOAD, DEVICE_IS_PLAYCONTAINER 값들의 bit 조합값을 반환하고 에러시 0 값을 반환한다.
*/
__MXAPI int	mxDmc_get_X_DlnaCap(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type);													// 다비이스의 X_DlnaCap 정보를 가져옵니다.

/**
  User Capability의 String값을 반환합니다. humax는 device description의 <hmx:X_HMXCAP/> element에서 User Capability string을 사용합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param udn Device의 UDN String. <UDN>uuid:XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX</UND> 형태에서 "uuid:" string을 제외한 string을 입력해야 한다.
  @param NameSpace device description에서 <root xmlns:hmx="urn:schemas-humax-co-kr:metadata-1-0"...> 부분에서 xmlns:hmx attribute값.
  @param UserCapName Device description <hmx:X_HMXCAP/> 에서 "X_HMXCAP" String.
  @param DeviceType mxdlna_define.h에서 정의된 enum _MXDLNA_DEVICE_TYPE 값중에 하나. 현재 MXDLNA_SERVER_TYPE(1), MXDLNA_USER_DEVICE_TYPE(16) 만 사용가능.
  @return User capability String. 실패하면 NULL.  리턴된 String은 메모리 해제하면 안된다.
*/
__MXAPI char * mxDmc_get_UserDevCap(MXDMC_HANDLE hDmc, char * udn, char * NameSpace, char * UserCapName, int DeviceType);

/**
  Deivice description URL 정보를 얻어옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE, DMC_MEDIA_DEVICE_TYPE 값중에 하나를 선택한다.
  @param udn Device의 UDN String.
  @return 성공하면 device description url을 반환하고 실패하면 NULL을 반환한다. 리턴된 string은 메모리 해제해야 한다.
*/
__MXAPI char *mxDmc_getDeviceDescriptionURL(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, MXDLNA_UDN udn);

/**
  TO DO : 설명필요
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE,  DMC_USER_DEVICE_TYPE, DMC_MEDIA_DEVICE_TYPE 값중에 하나를 선택한다.
  @param deviceDescriptionURL
  @return TO DO : 리턴값이 올바르게 정의되지 않았음(수정 필요).
*/
__MXAPI int mxDmc_checkDevice(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, char *deviceDescriptionURL);

// 2012-12-05
/**
  mxDmc_getDeviceService 함수 호출후 반환된 MXDLNA_SERVICE 구조체 포인터의 메모리를 해제합니다.
  @param service MXDLNA_SERVICE 타입의 구조체 포인터
  @return N/A
*/
__MXAPI void mxDmc_freeDeviceService(MXDLNA_SERVICE *service);

/**
  mxDmc_getDeviceAction 함수 호출후 반환된 MXDLNA_ACTION 구조체 포인터의 메모리를 해제합니다.
  @param action MXDLNA_ACTION 구조체 포인터.
  @return N/A
*/
__MXAPI void mxDmc_freeDeviceAction(MXDLNA_ACTION *action);

/**
  mxDmc_getDeviceStateVariable 함수 호출후 반환된 MXDLNA_STATE_VARIABLE 구조체 포인터의 메모리를 해제합니다.
  @param stateVariable MXDLNA_STATE_VARIABLE 구조체 포인터.
  @return N/A
*/
__MXAPI void mxDmc_freeDeviceStateVariable(MXDLNA_STATE_VARIABLE *stateVariable);


//------------------------------------------------------------------------
// @ Connection 관련
//------------------------------------------------------------------------
/**
  Muti Connection을 지원하는지 확인합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @param udn Device의 UDN String.
  @return live: MXCP_TRUE, dead: MXCP_FALSE, error: MXCP_ERROR_VALUE.
            TODO: 리터값 정리가 필요함.
  @remarks DMC_SERVER_TYPE은 사용할 수 없도록 주석처리되어 있는데 확인이 필요함.

*/
__MXAPI int mxDmc_isPrepareForConnection(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, MXDLNA_UDN UDN );

/**
  Connection을 생성합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @param RemoteProtocolInfo
  @param PeerConnectionManager
  @return 0
  @remarks 미구현(주석처리되어 있음).
*/
__MXAPI int mxDmc_createConnection
	(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, char *RemoteProtocolInfo, char *PeerConnectionManager);	

/**
  Connection을 종료합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @return TODO: 리터값 정리가 필요함.
  @remarks DMC_SERVER_TYPE은 사용할 수 없도록 주석처리되어 있는데 확인이 필요함.
*/
__MXAPI int mxDmc_closeConnection(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type);

/**
  Connection ID들을 받아온다.(Action)
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @return TODO: 리터값 정리가 필요함.
  @remarks DMC_SERVER_TYPE은 사용할 수 없도록 주석처리되어 있는데 확인이 필요함.
*/
__MXAPI int mxDmc_getCurrentConnectionIDs(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type);

/**
  저장된 Connection 정보를 가져옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @return TODO: 리터값 정리가 필요함.
  @remarks DMC_SERVER_TYPE은 사용할 수 없도록 주석처리되어 있는데 확인이 필요함.
*/
__MXAPI MXDLNA_USER_IDS	*mxDmc_loadConnectionIDs(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, MXDLNA_UDN UDN);

/**
  가져온 Connection 정보를 사용후 삭제합니다.
  @param start MXDLNA_USER_IDS 구조체 타입의 linked list.
  @return N/A
*/
__MXAPI void mxDmc_unloadConnectionIDs(MXDLNA_USER_IDS *start);
 
/**
  Connection 정보를 받아옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @return TODO: 리터값 정리가 필요함.
  @remarks DMC_SERVER_TYPE은 사용할 수 없도록 주석처리되어 있는데 확인이 필요함.
*/
__MXAPI int mxDmc_getCurrentConnectionInfo(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type);

/**
  연결되어있는 커넥션 정보를 받아옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @return 성공하면 MXDLNA_USER_RENDERER_INFO 구조체 포인터를 반환하고 실패시 NULL를 반환한다.
  @remarks DMC_SERVER_TYPE은 사용할 수 없도록 주석처리되어 있는데 확인이 필요함.
*/
__MXAPI MXDLNA_USER_RENDERER_INFO *mxDmc_loadConnectionInfo(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type);

/**
  연결되어있는 커넥션 정보를 사용후 정리합니다.
  @param start MXDLNA_USER_CONNECTION_DATA 구조체
  @return N/A
*/
__MXAPI void mxDmc_unloadConnectionInfo(MXDLNA_USER_CONNECTION_DATA *start);

/**
  장치가 지원하는 프로토콜 인포를 가져온다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @return  TODO: 리터값 정리가 필요함.
*/
__MXAPI int mxDmc_invokeGetProtocolInfo(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type);



//------------------------------------------------------------------------
// @ MSCP CDS Object 관련
//------------------------------------------------------------------------
typedef void (*MXDLNA_MSCP_BROWSERESPNSE_USER_CALLBACK) (void *CDSCP, MXDLNA_CP_HANDLE *hAction, void *userObject, int errorCode, int NumberReturned, int TotalNumber);
typedef void (*MXDLNA_MSCP_OPEN_MEDIA_CALLBACK)(void *CDSCP, MXDLNA_CP_HANDLE hAction, void *userObject, int errorCode, int NumberReturned,int TotalNumber);

/**
   현재 선택된 DMS로부터 자동으로 CDS 목록을 갖고 올 수 있도록 핸들을 생성합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param id  UPnP Container의 object id값.
  @param tag    browse reponse 때 호출되는 Callback함수의 파라미터로 전달되는 사용자 임의의 데이타
  @param order  정렬순서. enum MXDLNA_SORT_ORDER 에서 정의된 값을 사용한다. 0 : 정렬하지 않음.
  @param filter Response 받는 <DIDL> XML에서 알고싶은 정보만 filterout할 때 사용한다. comma로 분리하여 여러 항목을 filter 걸수 있다.
        일부 정보는 filter out 되지 않는다(예: 전체표시 "*", 일부항목 filter out "dc:creator".  dc:title 항목은 필수로 표시됨)
        ContentDirectory Service 문서의 A_ARG_TYPE_Filter 항목 내용을 참고한다.
  @return 성공하면 MXDLNACP_HANDLE 구조체 포인터가 void* 형태로 반환하고 실패시 NULL을 반환한다.
  @remarks 하나의 dms안에서 동시에 여러 container를 open하여 cds 목록을 갖고 올 수 있는지 확인이 필요하다.
*/
__MXAPI void *mxDmc_openContentObjectHandle(MXDMC_HANDLE hDmc, char *id, MXDLNA_USER_TAG tag, int order, char *filter);

/**
  현재 자동으로 갖고 온 CDS 목록의 개수를 반환한다.
  @param hObject mxDmc_openContentObjectHandle 함수 호출하여 반환된 MXDLNACP_HANDLE 구조체 포인터 값.
  @return 성공하면 CDS 목록 개수를 반환하고 실패하면 -1를 반환한다.
*/
__MXAPI int mxDmc_getContentObjectCount(void *hObject);

/**
  자동으로 갖고온 CDS 목록에서 idx 번째  CDS 정보를 가져옵니다.
  @param hObject mxDmc_openContentObjectHandle 함수 호출하여 반환된 MXDLNACP_HANDLE 구조체 포인터 값.
  @param idx  CDS index.
  @return 성공하면 make_app_data 함수 두번째 파라미터에서 사용되는 사용자 정의 CDS object 구조체 포인터를 반환하고 실패시 NULL을 반환한다.
*/
__MXAPI void *mxDmc_getContentObject(void *hObject, int idx);

/**
  MXDLNACP_HANDLE 핸들을 close합니다.
  @param hObject mxDmc_openContentObjectHandle 함수 호출하여 반환된 MXDLNACP_HANDLE 구조체 포인터 값.
  @return 성공하면 MXDLNA_OK 반환하고 실패서 MXDLNA_ERROR를 한다.
*/
__MXAPI int mxDmc_closeContentObjectHandle(void *objectHandle);

/**
  mxDmc_openSearchContentObjectHandle 함수 호출시 사용할 "SearchCriteria" parameter에 사용될 string을 생성합니다.
  @param SearchCriteriaData
  @param type MXDLNA_SEARCH_TYPE 에서 정의된 search type.
  @param argument SearchCriteria string에 추가한 정보.
  @param add MXDLNA_SEARCH_ADD_TYPE 에서 정의된 type(MXDLNA_SC_NONE, MXDLNA_SC_AND,	MXDLNA_SC_OR).
  @return 성공하면 "SearchCriteria" string을 반환하고 실패시 NULL을 반환한다. 사용이 끝나면 리턴된 string은 메모리 해제해야 한다.
          SearchCriteria String syntax는 EBNF를 따르는데 자세한 것은 UPnP ContentDirectory service 문서의 A_ARG_TYPE_SearchCriteria 항목을 참고한다.
*/
__MXAPI char *mxDmc_makeSearchCriteriaString(char *SearchCriteriaData, int type, char *argument, int add);

/**
  mxDmc_openContentObjectHandle 함수 기능에서 SearchCriteria 기능이 추가된 버전입니다. SearchCriterial 를 만족하는 CDS 목록만을 가져올 수 있습니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param id  UPnP Container의 object id값.
  @param tag    browse reponse 때 호출되는 Callback함수의 파라미터로 전달되는 사용자 임의의 데이타
  @param order  정렬순서. enum MXDLNA_SORT_ORDER 에서 정의된 값을 사용한다. 0 : 정렬하지 않음.
  @param SearchCriteria  UPnP ContentDirectory serivce 문서의 A_ARG_TYPE_SearchCriteria 에서 정의된 syntax를 따르는 string.
  @param filter Response 받는 <DIDL> XML에서 알고싶은 정보만 filterout할 때 사용한다. comma로 분리하여 여러 항목을 filter 걸수 있다.
        일부 정보는 filter out 되지 않는다(예: 전체표시 "*", 일부항목 filter out "dc:creator".  dc:title 항목은 필수로 표시됨)
        PnP ContentDirectory Service 문서의 A_ARG_TYPE_Filter 항목 내용을 참고한다.
  @return 성공하면 MXDLNACP_HANDLE 구조체 포인터가 void* 형태로 반환하고 실패시 NULL을 반환한다.
*/
__MXAPI void *mxDmc_openSearchContentObjectHandle(MXDMC_HANDLE hDmc, char *id, MXDLNA_USER_TAG tag, int order, char *SearchCriteria, char *fliter);



// 구버전 
__MXAPI void *mxDmc_openCdsHandle(MXDMC_HANDLE hDmc, char *id, MXDLNA_USER_TAG tag, int order, int *count);					// CDS Handle을 Open 합니다.
__MXAPI void *mxDmc_getCdsContent(void *hObject, int idx);															// 해당 idx에 CDS Contents를 가져옵니다.
__MXAPI void mxDmc_getMetaData(MXDMC_HANDLE hDmc, MXDLNA_UDN UDN, char *ObjectID, int order, MXDLNA_MSCP_BROWSE_RESULT_CUSTOM callback);		// 특정 Object에 대해서 Action을 날려서 Object에 대한 메타정보를 가져온다. (Action)
__MXAPI int mxDmc_closeCdsHandle(void *objectHandle);																		// CDS Handle 사용후 닫습니다.

/**
  ContentDirectory::Browse Action을 호출한다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param UDN ContentDirectory::Browse Action을 호출할 DMS의 UDN.
  @param id ObjectID.
  @param callBack Action 호출후 DMS으로부터 Response를 받으면 호출되는 Callback 함수.
  @param order 정렬순서. enum MXDLNA_SORT_ORDER 에서 정의된 값을 사용한다. 0 : 정렬하지 않음.
  @param idx CDS index
  @param cnt 요청한 CDS 개수.
  @param UserObject 사용자 정의 데이터
  @param BrowseFlag MXDLNA_MSCP_BROWSEFLAG_METADATA or  MXDLNA_MSCP_BROWSEFLAG_CHILDREN
  @return TO DO: 코드 수정하여 리턴값 수정해야 함.
*/
__MXAPI int mxDmc_invokeBrowse(MXDMC_HANDLE hDmc, MXDLNA_UDN UDN, char *id, MXDLNA_MSCP_BROWSE_RESULT_CUSTOM callBack, int order, int idx, int cnt, void *UserObject, MXDLNA_BROWSE_FLAG BrowseFlag);	// CDS를 Cache를 가치지 않고 받아옵니다.

//-_-;; 이리로 오시믄 안되는뎅....
__MXAPI int mxDmc_invokeGetSortCapabilities(MXDMC_HANDLE hDmc,void (*getSortCapabilitiesResponsePtr)(void *sender,int ErrorCode,void *user,char* SortCaps));

/**
  ContentDirectory::Browse Action을 호출합니다. DMS로부터 응답 받은 <DIDL-Lite> XML 원본을 parsing하지 않고 그대로 전달합니다.
(유렵에서 위성채널 수천개를 browse할 때 속도 이슈때문에 추가한 API).
  @param hDmc MXDMC_HANDLE 핸들.
  @param UDN device의 UDN string.
  @param ObjectID CDS 목록이 들어 있는 container의 objectID.
  @param BrowseFlag "BrowseMetadata" 또는 "BrowseDirectChildren".
      BrowseMetadata - this indicates that the properties of the object specified by the ObjectID parameter will be returned in the result.
      BrowseDirectChildren - this indicates that first level objects under the object specified by ObjectID parameter will be returned in the result, as well as the metadata of all objects specified.
  @param Filter Response 받는 <DIDL> XML에서 알고싶은 정보만 filterout할 때 사용한다. comma로 분리하여 여러 항목을 filter 걸수 있다.
        일부 정보는 filter out 되지 않는다(예: 전체표시 "*", 일부항목 filter out "dc:creator".  dc:title 항목은 필수로 표시됨)
        PnP ContentDirectory Service 문서의 A_ARG_TYPE_Filter 항목 내용을 참고한다.
  @param StartingIndex  갖고 CDS 목록 start index.
  @param RequestedCount   start index로부터 갖고 올 CDS 개수.
  @param SortCriteria UPnP ContentDirectory serivce 문서의 A_ARG_TYPE_SearchCriteria 에서 정의된 syntax를 따르는 string.
  @param CB  DMS로 부터 CDS 목록을 받았을 때 호출되는 Callback 함수입니다. 아래처럼 정의되어 있습니다.
        void (*MXDLNA_MSCP_PURE_BROWSE_RESULT) (void *CP, char *UDN, int ErrorCode,void *User,char* Result,unsigned int NumberReturned, unsigned int TotalMatches,unsigned int UpdateID, void *UserData)
  @return 성공하면 MXDLNA_OK, 실패하면 MXDLNA_ERROR을 반환한다.
*/
__MXAPI int mxDmc_invokePureBrowse(MXDMC_HANDLE hDmc, char *UDN, char *ObjectID, char *BrowseFlag, char *Filter, int StartingIndex, int RequestedCount, char *SortCriteria, MXDLNA_MSCP_PURE_BROWSE_RESULT CB);

/**
ContentDirectory Service의 A_ARG_TYPE_SearchCriteria 에서 정의한 Syntax를 따르는 SearchCriteria string을 생성합니다.
  @param SearchCriteria [OUT] refer to A_ARG_TYPE_SearchCriteria.
  @param type enum MXDLNA_SEARCH_TYPE 에서 정의한 값을 사용합니다.
  @param argument
  @param add enum MXDLNA_SEARCH_ADD_TYPE 에서 정의한 MXDLNA_SC_NONE, MXDLNA_SC_AND, MXDLNA_SC_OR을 사용할 수 있습니다.
  @return 성공하면 SearchCriteria string 길이를 반환하고 실패하면 0을 반환합니다.
*/
__MXAPI int mxDmc_makeSearchCriteria(char **SearchCriteria, int type, char *argument, int add);																								// 검색 할 스트링을 만듭니다.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @param id
  @param tag
  @param order
  @param count
  @param type
  @param argument
  @param add
  @return
*/
__MXAPI void *mxDmc_openServerSearchObjectList(MXDMC_HANDLE hDmc, char *id, MXDLNA_USER_TAG tag, int order, int *count, int type, char *argument, int add);									// CDS를 검색합니다.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @param UDN
  @param id
  @param tag
  @param order
  @param count
  @param type
  @param argument
  @param add
  @return
*/
__MXAPI void *mxDmc_openServerSearchObjectListEx(MXDMC_HANDLE hDmc, MXDLNA_UDN UDN, char *id, MXDLNA_USER_TAG tag, int order, int *count, int type, char *argument, int add);		// CDS를 검색합니다.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI void *mxDmc_openServerSearchObjectListWithCriteria(	MXDMC_HANDLE hDmc, char *id, MXDLNA_USER_TAG tag, int order, int *count, char *SearchCriteria);									// CDS를 검색합니다.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI void *mxDmc_openServerSearchObjectListWithCriteriaNUDN(	MXDMC_HANDLE hDmc, char *id, MXDLNA_USER_TAG tag, int order, int *count, char *SearchCriteria, MXDLNA_UDN UDN);		// CDS를 검색합니다.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI int mxDmc_closeServerObjectList(void *objectHandle);	// CDS를 검색한 리스트를 사용후 정리합니다.


//------------------------------------------------------------------------
// @ MSCP Action 관련
//------------------------------------------------------------------------
/**
ContentDirectory Service의 CreateObject Action을 호출하여 Object를 생성합니다.

결과값은 MXDLNA_CP_CONFIG::DeviceResponse Callback 함수로 전달됩니다.
DeviceResponse(MXDLNACP mscp,MSCP_ACTION_HANDLE hAction, int Code = MXDLNA_MSCP_CREATEOBJECT_SINK, int  ErrorCode, MXDLNA_USER_CREATEOBJECT* Data);
  @param hDmc MXDMC_HANDLE 핸들.
  @param ProtocolInfo <rec> element의 attribute으로 들어갈 "ProtocolInfo" 정보.
  갈@param ContainerID Object를 생성할 container의 object ID값.
  @param Title <dc:title>에 들어갈 내용.
  @param MediaClass, CdsMediaClass.h에서 정의된 CDS_MEDIACLASS_XXX 값들을 사용한다. 이 값이 0 일때는 Extension 값으로 정한다. <upnp:class> 에 들어가는 정보이다.
  @param Extension MediaClass = 0일때, 파일의 확장자로 MediaClass를 정한다. NULL일수있음. MediaClass = 0 이고 Extension = NULL이면 CDS_MEDIACLASS_Container 값을 사용하여 Object를 생성한다.
  @return
*/
__MXAPI int mxDmc_createObject(MXDMC_HANDLE hDmc, char *ProtocolInfo, char *ContainerID, char *Title, unsigned int MediaClass, char *Extension);	// Create Object 명령을 내린다.

/**
ContentDirectory Service의 DestroyObject Action을 호출하여 Object를 파괴합니다.

결과값은 MXDLNA_CP_CONFIG::DeviceResponse Callback 함수로 전달됩니다.
DeviceResponse(MXDLNACP mscp,MSCP_ACTION_HANDLE hAction, int Code = MXDLNA_MSCP_DESTROYOBJECT_SINK, int  ErrorCode, MXDLNA_USER_CREATEOBJECT* Data = NULL);
  @param hDmc MXDMC_HANDLE 핸들.
  @param ObjectID  파괴할 obectID.
  @return
*/
__MXAPI int mxDmc_destroyObject(MXDMC_HANDLE hDmc, char *ObjectID);															// Destroy Object 명령을 내린다.

/**
This action transfers a file from a remote source resource, specified by SourceURI, to a local destination
resource, specified by DestinationURI, in the Content Directory Service. When the Content Directory
Service identifies the destination resource in CDS, the action will return a unique TransferID in the
response and start HTTP GET. A client can monitor the progress of file transfer by using
GetTransferProgress().
The DestinationURI should correspond to an importURI attribute of a <res> element present in the CDS.
This <res> element might already have accessible content, or not. In the first case, ImportResource()
actually updates the resource contents. In the second case, ImportResource() assigns the first content to a
object. Note that it is up to the Content Directory Service to determine if a specified <res> element is
actually allowed to be updated.

결과값은 MXDLNA_CP_CONFIG::DeviceResponse Callback 함수로 전달됩니다.
DeviceResponse(MXDLNACP mscp,MSCP_ACTION_HANDLE hAction, int Code = MXDLNA_MSCP_IMPORTRESOURCE_SINK, int  ErrorCode,char* TransferID);
  @param hDmc MXDMC_HANDLE 핸들.
  @param SourceURI
  @param DestinationURI
  @return 에러시 0 또는 -1이 반환된다. TO DO: 코드수정하여 리턴값 조정이 필요하다.
*/
__MXAPI int mxDmc_importResource(MXDMC_HANDLE hDmc, char *SourceURI, char *DestinationURI);									// Import Resource 명령을 내린다.

/**
This action transfers a file from a local source resource, specified by SourceURI, to a remote destination
resource, specified by DestinationURI. When the CDS identifies the source resource, the action will
return a unique TransferID in the response and start the HTTP POST. A client can monitor the progress of
file transfer by using the GetTransferProgress() action.

결과값은 MXDLNA_CP_CONFIG::DeviceResponse Callback 함수로 전달됩니다.
DeviceResponse(MXDLNACP mscp,MSCP_ACTION_HANDLE hAction, int Code = MXDLNA_MSCP_EXPORTRESOURCE_SINK, int  ErrorCode,char* TransferID);
  @param hDmc MXDMC_HANDLE 핸들.
  @param SourceURI
  @param DestinationURI
  @return
*/
__MXAPI int mxDmc_exportResource(MXDMC_HANDLE hDmc, char *SourceURI, char *DestinationURI);									// Export Resource 명령을 내린다.

/**
This action is used to query the progress of the file transfer initiated by the ImportResource() or the
ExportResource() action. Progress of the file transfer, specified by TransferID, will be returned in the
response. TransferStatus indicates the status of the file transfer, it can be either IN_PROGRESS,
STOPPED, ERROR, or COMPLETED. TransferLength specifies the length in bytes that has been
transferred. TransferTotal specifies the total length of file in bytes that should be transferred in this file
transfer. If the CDS cannot determine the total length, TransferTotal is set to zero. If the file transfer is
started, the status is changed to IN_PROGRESS. If the file transfer is finished, the status is changed to
either STOPPED, ERROR, or COMPLETED depending on the result of the file transfer. CDS maintains
the status of a file transfer for at least 30 seconds after the file transfer has finished, ensuring that a client
can query the result of file transfer.

결과값은 MXDLNA_CP_CONFIG::DeviceResponse Callback 함수로 TransferStatus,TransferLength, TransferTotal 정보가 전달된다.
DeviceResponse(MXDLNACP mscp,MSCP_ACTION_HANDLE hAction, int Code = MXDLNA_MSCP_TRANSFERPROGRESS_SINK, int  ErrorCode, MXDLNA_USER_GET_TRANSFER_PROGRESS* Progress);
  @param hDmc MXDMC_HANDLE 핸들.
  @param TransferID ImportResource 또는 ExportResource Action후 전달 받은 TranserID 값.
  @return TO DO: 코드수정하여 리턴값 조정 필요.
*/
__MXAPI int mxDmc_getTransferProgress(MXDMC_HANDLE hDmc, char *TransferID);

/**
This action stops the file transfer initiated by the ImportResource() or ExportResource() action. The file
transfer, identified by TransferID, is halted immediately.

결과값은 MXDLNA_CP_CONFIG::DeviceResponse Callback 함수로 전달된다.
DeviceResponse(MXDLNACP mscp,MSCP_ACTION_HANDLE hAction, int Code = MXDLNA_MSCP_STOP_TRANSFER_RESOURCE_SINK, int  ErrorCode, NULL);
  @param hDmc MXDMC_HANDLE 핸들.
  @param TransferID ImportResource 또는 ExportResource Action후 전달 받은 TranserID 값.
  @return TO DO: 코드수정하여 리턴값 조정 필요.
*/
__MXAPI int mxDmc_stopTransferResource(MXDMC_HANDLE hDmc, char *TransferID);												// Stop Transfer Resource 명령을 내린다.

/**
CDS Object를 다른 Container 아래로 이동시킨다.
결과값은 MXDLNA_CP_CONFIG::DeviceResponse Callback 함수로 전달된다.
DeviceResponse(MXDLNACP mscp,MSCP_ACTION_HANDLE hAction, int Code = MXDLNA_MSCP_MOVEOBJECT_RESOURCE_SINK, int  ErrorCode, char* NewObjectID);
  @param hDmc MXDMC_HANDLE 핸들.
  @param ObjectID
  @param NewParentID
  @return TO DO: 코드수정하여 리턴값 조정 필요.
*/
__MXAPI int mxDmc_moveObject(MXDMC_HANDLE hDmc, char* ObjectID, char *NewParentID);											// Move Object 명령을 내린다.

/**
This action modifies, deletes or inserts object metadata. The object to be updated is specified by
ObjectID. CurrentTagValue is a CSV list of XML fragments. Each fragment is either the complete,
exact, current text of an existing metadata element of the object or an empty placeholder. NewTagValue is
also a CSV list of XML fragments, each of which is the complete new text of a metadata element for the
object or an empty placeholder. The two tag/value lists must have the same number of entries. Each entry
in CurrentTagValue represents metadata to be modified. The corresponding entry in NewTagValue
represents the new, replacement metadata for the element identified by CurrentTagValue.

결과값은 MXDLNA_CP_CONFIG::DeviceResponse Callback 함수로 전달된다.
DeviceResponse(MXDLNACP mscp,MSCP_ACTION_HANDLE hAction, int Code = MXDLNA_MSCP_UPDATEOBJECT_RESOURCE_SINK, int  ErrorCode, NULL);
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 코드수정하여 리턴값 조정 필요.
*/
__MXAPI int mxDmc_updateObject(MXDMC_HANDLE hDmc, char* ObjectID, char* CurrentTagValue, char* NewTagValue);				// Update Object 명령을 내린다.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 코드수정하여 리턴값 조정 필요.
*/
__MXAPI int mxDmc_deleteObjectCache(MXDMC_HANDLE hDmc);


//------------------------------------------------------------------------
// @ MRCP Action 관련
//------------------------------------------------------------------------

/**
DMR device에서 AVTransport Service의 SetAVTransportURI Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_SETURL_SINK, int ErrorCode, NULL)
  @param hDmc MXDMC_HANDLE 핸들.
  @param URI  Media URL. NULL이면 안된다.
  @param CurrentURIMetaData  the form of a DIDL-Lite XML Fragment(defined in the ContentDirectory service template). XML String. NULL이면 안된다.
  @return TO DO: 리턴값 수정이 필요함.
  @remarks TO DO: CurrentURIMetaData parameter가 NULL이면 에러처리하지 않아 crash 발생할 수 있다. 수정필요함.
*/
__MXAPI int mxDmc_invokeSetUriWithMeta(MXDMC_HANDLE hDmc, char *URI, char *CurrentURIMetaData);			// Seturi를 Metadata를 통해서 설정합니다. (Action)

/**
DMR device에서 AVTransport Service의 SetAVTransportURI Action을 호출합니다.
CurrentURIMetaData XML String대신에 자용자 정의 CDS 구조체를 사용하여 Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_SETURL_SINK, int ErrorCode, NULL)
  @param hDmc MXDMC_HANDLE 핸들.
  @param URI  Media URL. NULL이면 안된다.
  @param CdsData make_app_data API 두번째 파라미터로 사용되는 사용자 정의 CDS 포인터.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeSetUriWithCdsData(MXDMC_HANDLE hDmc, char *URI, void *CdsData);					// Seturi를 CdsObject Data를 가지고 MetaData를 생성해서 설정합니다. (Action)

/**
DMR device에서 AVTransport Service의 Play Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_PLAY_SINK, int ErrorCode, NULL)
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokePlay(MXDMC_HANDLE hDmc);														// 렌더러에 Play 명령을 내립니다. (Action)

/**
DMR device에서 AVTransport Service의 play Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_PLAY_SINK, int ErrorCode, NULL)
  @param hDmc MXDMC_HANDLE 핸들.
  @param PlaySpeed 배속 재생(정상속도:1)
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokePlayWithSpeed(MXDMC_HANDLE hDmc, char *PlaySpeed);								// 렌더러에 Play 명령을 플레이 스피드와 함께 내립니다. (Action)

/**
DMR device에서 AVTransport Service의 Stop Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_STOP_SINK, int ErrorCode, NULL)
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeStop(MXDMC_HANDLE hDmc);														// 렌더러에 Stop 명령을 내립니다. (Action)

/**
DMR device에서 AVTransport Service의 Pause Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_PAUSE_SINK, int ErrorCode, NULL)
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokePause(MXDMC_HANDLE hDmc);														// 렌더러에 Pause 명령을 내립니다. (Action)

/**
DMR device에서 AVTransport Service의 Seek Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_SEEK_SINK, int ErrorCode, NULL)
  @param hDmc MXDMC_HANDLE 핸들.
  @param Unit UPnP AVTransport Service A_ARG_TYPE_SeekMode 정보입니다. Seek 단위를 설정합니다. mxlib에서는 아래처럼 enum 으로 정의되어 있습니다.
   https://lists.01.org/pipermail/dleyna/2013-May/000124.html 사이트 상세한 설명이 있습니다.
<pre>
enum
{
    AVT_SEEK_MODE_TRACK_NR = 0,
    AVT_SEEK_MODE_ABS_TIME,
    AVT_SEEK_MODE_REL_TIME,
    AVT_SEEK_MODE_ABS_COUNT,
    AVT_SEEK_MODE_REL_COUNT,
    AVT_SEEK_MODE_CHANNEL_FREQ,
    AVT_SEEK_MODE_TAPE_INDEX,
    AVT_SEEK_MODE_REL_TAPE_INDEX,
    AVT_SEEK_MODE_FRAME,
    AVT_SEEK_MODE_REL_FRAME,
    AVT_SEEK_MODE_X_DLNA_REL_BYTE
};
</pre>
  @param seekTimeOrTrack Unit parameter에 따라 다릅니다. 주로 AVT_SEEK_MODE_REL_TIME, AVT_SEEK_MODE_ABS_TIME 를 사용하는데, "hh:mm:ss" 형태 time format에서 hh x 3600 + mm x 60 + ss 한 값을 사용합니다.
  UPnP AVTransport Service의 A_ARG_TYPE_SeekTarget 항목을 참조 바랍니다.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeSeek(MXDMC_HANDLE hDmc, int Unit, unsigned long seekTimeOrTrack);				// 렌더러에 Seek 명령을 내립니다. (Action)

/**
DMR device에서 AVTransport Service의 GetPositionInfo Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_GET_POSITION_INFO_SINK, int ErrorCode, MXDLNA_USER_POSITION* pos)
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeGetPositionInfo(MXDMC_HANDLE hDmc);												// 렌더러로 부터 재생 위치 시간을 가져옵니다. (Action)

/**
DMR device에서 AVTransport Service의 X_DLNA_GetPositionInfo Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_X_DLNA_GET_POSITION_INFO_SINK, int ErrorCode, MXDLNA_USER_POSITION_BYTE* pos)
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invoke_X_DLNA_GetPositionInfo(MXDMC_HANDLE hDmc);										// 렌더러로 부터 재생 위치 바이트를 가져옵니다. (Action)

/**
DMR device에서 RenderingControl Service의 SetMute Action을 호출합니다.
음을 소거할 때 사용합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_SET_MUTE_SINK, int ErrorCode, NULL)

  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeSetMute(MXDMC_HANDLE hDmc, char *channel, int muteState);						// 렌더러에 음소거를 명령을 내립니다. (Action)

/**
DMR device에서 RenderingControl Service의 SetVolume Action을 호출합니다.
소리 볼륨을 조절할 때 사용합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_SET_VOLUME_SINK, int ErrorCode, NULL)
  @param hDmc MXDMC_HANDLE 핸들.
  @param channel A_ARG_TYPE_Channel에서 정의된 채널 정보입니다. 구현된 DMR에 따라 다릅니다.
  @param newVolumeValue UPnP RenderingControl Service의 state variables/Volume에 값의 범위가 정의되어 있습니다. 구현된 DMR에 따라 다릅니다.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeSetVolume(MXDMC_HANDLE hDmc, char *channel, char newVolumeValue);				// 렌더러에 볼륨을 조절합니다. (Action)

/**
DMR device에서 RenderingControl Service의 GetCurrentTransportActions Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_GET_CURRENT_TRANSPORT_ACTION_SINK, int ErrorCode, char* Actions)
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeGetCurrentTransportActions(MXDMC_HANDLE hDmc);									// 현재 사용가능 액션을 반환합니다. (Action)

/**
DMR device에서 RenderingControl Service의 GetDeviceCapabilities Action을 호출합니다.
DMR이 지원하는 기기 정보를 갖고옵니다. HMS STB인 경우 "Network" 으로 설정되어 있고 Windows Media Player 같은 경우 "Network, HDD, CD-DA, DVD-VIDEO" 등으로 설정되어 있습니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_GET_DEVICE_CAPABILITIES_SINK, int ErrorCode, MXDLNA_USER_RENDERER_INFO* info)
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeGetDeviceCapabilities(MXDMC_HANDLE hDmc);										// 디바이스에 대한 정보 어떤 기기를 지원하는지를 알아옵니다 (Action)

/**
DMR device에서 RenderingControl Service의 GetMediaInfo Action을 호출합니다.
DMR에 설정된 duration, currentURI, currentURIMetaData 등을 가져올 때 사용합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_GET_MEDIA_INFO_SINK, int ErrorCode, MXDLNA_USER_RENDERER_INFO* info)
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeGetMediaInfo(MXDMC_HANDLE hDmc);												// 렌더러에 설정된 미디어에 대한 정보(duration, currentUri, currentURIMetaData 등..)를 받아온다. (Action)

/**
DMR device에서 RenderingControl Service의 GetTransportInfo Action을 호출합니다.
DMR의 전송 속도/상태 정보를 가져올 때 사용합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_GET_TRANSPORT_INFO_SINK, int ErrorCode, MXDLNA_USER_RENDERER_INFO* info)
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeGetTransportInfo(MXDMC_HANDLE hDmc);											// 렌더러에 전송 정보를 가져옵니다. (Action)

/**
DMR device에서 RenderingControl Service의 GetTransportSettings Action을 호출합니다.
DMR의 PlayMode, RecQualityMode(녹화품질) 정보를 가져올 때 사용합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_GET_TRANSPORT_SETTING_SINK, int ErrorCode, MXDLNA_USER_RENDERER_INFO* info)
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeGetTransportSettings(MXDMC_HANDLE hDmc);										// 렌더러의 전송 정보중 플레이모등, 녹화 품질 정보를 가져옵니다 (Action)

/**
DMR device에서 AVTransport Service의 SetNextUriWithMeta Action을 호출합니다.
SetNextUriWithMeta Action은 Options 항목이라서 구현된 DMR에 따라 Action이 없을 수 있습니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_SET_NEXT_URI_SINK, int ErrorCode, NULL)
  @param hDmc MXDMC_HANDLE 핸들.
  @param URI  Media URL. NULL이면 안된다.
  @param CurrentURIMetaData  the form of a DIDL-Lite XML Fragment(defined in the ContentDirectory service template). XML String. NULL이면 안된다.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeSetNextUriWithMeta(MXDMC_HANDLE hDmc, char *URI, char *CurrentURIMetaData);		// Meta데이터를 가지고 Next URI 설정 (Action)

/**
DMR device에서 AVTransport Service의 SetNextUriWithMeta Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_SET_NEXT_URI_SINK, int ErrorCode, NULL)
  @param hDmc MXDMC_HANDLE 핸들.
  @param CdsData make_app_data API 두번째 파라미터에 사용되는 사용자 정의 CDS 구조체.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeSetNextUriWithCdsData(MXDMC_HANDLE hDmc, void *CdsData);						// CDS Data 데이터를 가지고 Next URI 설정 (Action)

/**
DMR device에서 AVTransport Service의 Previous Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_PREV_SINK, int ErrorCode, NULL)
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokePrev(MXDMC_HANDLE hDmc);														// 렌더러에 Prev 명령을 내립니다. (Action)

/**
DMR device에서 AVTransport Service의 Next Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_NEXT_SINK, int ErrorCode, NULL)
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO: 리턴값 수정이 필요함.
*/
__MXAPI int mxDmc_invokeNext(MXDMC_HANDLE hDmc);														// 렌더러에 Next 명령을 내립니다. (Action)

#define mxDmc_setUriWithMeta(hDmc, URI, CurrentURIMetaData)			mxDmc_invokeSetUriWithMeta(hDmc, URI, CurrentURIMetaData)
#define mxDmc_setUriWithCdsData(hDmc, URI, CdsData)					mxDmc_invokeSetUriWithCdsData(hDmc, URI, CdsData) 
#define mxDmc_play(hDmc)											mxDmc_invokePlay(hDmc)
#define mxDmc_playWithSpeed(hDmc, PlaySpeed)						mxDmc_invokePlayWithSpeed(hDmc, PlaySpeed)
#define mxDmc_stop(hDmc)											mxDmc_invokeStop(hDmc)
#define mxDmc_pause(hDmc)											mxDmc_invokePause(hDmc)
#define mxDmc_seek(hDmc, Unit, seekTimeOrTrack)						mxDmc_invokeSeek(hDmc, Unit, seekTimeOrTrack)
#define mxDmc_getPositionInfo(hDmc)									mxDmc_invokeGetPositionInfo(hDmc)
#define mxDmc_X_DLNA_GetPositionInfo(hDmc)							mxDmc_invoke_X_DLNA_GetPositionInfo(hDmc)
#define mxDmc_setMute(hDmc, channel, muteState)						mxDmc_invokeSetMute(hDmc, channel, muteState)
#define mxDmc_setVolume(hDmc, channel, newVolumeValue)				mxDmc_invokeSetVolume(hDmc, channel, newVolumeValue)
#define mxDmc_getCurrentTransportActions(hDmc)						mxDmc_invokeGetCurrentTransportActions(hDmc)
#define mxDmc_getDeviceCapabilities(hDmc)							mxDmc_invokeGetDeviceCapabilities(hDmc)
#define mxDmc_getMediaInfo(hDmc)									mxDmc_invokeGetMediaInfo(hDmc)
#define mxDmc_getTransportInfo(hDmc)								mxDmc_invokeGetTransportInfo(hDmc)
#define mxDmc_getTransportSettings(hDmc)							mxDmc_invokeGetTransportSettings(hDmc)												
#define mxDmc_setNextUriWithMeta(hDmc, URI, CurrentURIMetaData)		mxDmc_invokeSetNextUriWithMeta(hDmc, URI, CurrentURIMetaData)
#define mxDmc_setNextUriWithCdsData(hDmc, ObjectID)					mxDmc_invokeSetNextUriWithCdsData(hDmc, ObjectID)
#define mxDmc_prev(hDmc)											mxDmc_invokePrev(hDmc)
#define mxDmc_next(hDmc)											mxDmc_invokeNext(hDmc)


//------------------------------------------------------------------------
// @ MRCP Load Data 관련
//------------------------------------------------------------------------
/**
DMR의 AVTransport Service의 GetMediaInfo Action에서 반환하는 값중에서 CurrentURIMetaData 인자의 <dc:title> 값을 가져옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return 성공하면 Media title string, 실패시 NULL를 반환합니다. 반환된 string은 메모리 해제 해야합니다.
*/
__MXAPI char *mxDmc_getMediaTitle(MXDMC_HANDLE hDmc);

/**
DMR의 AVTransport Service의 GetMediaInfo Action에서 반환하는 값중에서 CurrentURI String을 가져옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return 성공하면 Media URI string, 실패시 NULL를 반환합니다. 반환된 string은 메모리 해제 해야합니다.
*/
__MXAPI char *mxDmc_getMediaUrl(MXDMC_HANDLE hDmc);

/**
DMR의 RenderingControl Service의 State variables/A_ARG_TYPE_Channel/Allowed Values 개수(Audio Channel개수)를 반환합니다.
이 값들은 다음처럼 정의되어 있습니다.
Master, Left Front, Right Front, Center Front,...
  @param hDmc MXDMC_HANDLE 핸들.
  @return 성공하면 Audio Chnnel 개수를 반환하고 실패하면 0을 반환합니다.
  @remarks TO DO: 에러시 -1로 반환하도록 코드 수정이 필요함.
*/
__MXAPI int mxDmc_getChannelCount(MXDMC_HANDLE hDmc);

/**
DMR의 RenderingControl Service의 State variables/A_ARG_TYPE_Channel/Allowed Values에서 index번째 Audio Channel 이름을 반환합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return 성공하면 Audio Channel을, 실패시 NULL를 반환합니다. 반환된 string은 메모리 해제 해야합니다.
*/
__MXAPI char *mxDmc_getChannelName(MXDMC_HANDLE hDmc, int index);

/**
DMR의 RenderingControl Service의 GetVolume Action을 호출하여 index번째 Audio Channel의 volume값을 가져옵니다.
volume값의 범위는 RenderingControl Service/State variables/Volume 에서 "Minimum", "Maximum" 값으로 범위가 정해져 있습니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return 성공하면 index번째 audio chnannel 값을 반환하고 실패시 0을 반환합니다.
  @remarks TO DO: 에러시 -1로 반환하도록 코드 수정이 필요함.
*/
__MXAPI int mxDmc_getVolume(MXDMC_HANDLE hDmc, int index);

/**
DMR의 RenderingControl Service의 GetMute Action을 호출하여 index번째 Audio Channel의 음소거 여부를 확인합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return 음소거가 되었으면 0을 반환하고 그 외에는 0이 아닌값을 반환한다.
  @remarks TO DO: 에러시 -1로 반환하도록 코드 수정이 필요함.
*/
__MXAPI int mxDmc_getMute(MXDMC_HANDLE hDmc, int index);

/**
DMR의 AVTransport Service의 GetTransportInfo Action을 호출하여 CurrentTransportState 값을 가져옵니다.
반환하는 값은 아래처럼 정의되어 있습니다.
<pre>
enum PlayStateEnum
{
    AVRCP_UNKNOWN = -1,
    AVRCP_PLAYING = 0,
    AVRCP_STOPPED,
    AVRCP_PAUSED,
    AVRCP_RECORDING,
    AVRCP_TRANSITIONING,
    AVRCP_NO_MEDIA,
};
</pre>
  @param hDmc MXDMC_HANDLE 핸들.
  @return 성공하면 PlayStateEnum 값을 반환하고 실패시 MXCP_ERROR_VALUE 을 반환합니다.
*/
__MXAPI int mxDmc_getPlayStatus(MXDMC_HANDLE hDmc);							// 재생 상태를 가져옵니다.

/**
DMR의 RenderingControl Service의 State variables/A_ARG_TYPE_Channel/Allowed Values 정보를 MXDLNA_USER_CHANNEL_LIST 구조체 형태로 가져옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return 성공하면 MXDLNA_USER_CHANNEL_LIST 구조체 포인터를 반환하고 실패시 NULL을 반환합니다. 반환된 포인터는 mxDmc_unloadChannel 함수를 사용하여 메모리 해제해야 합니다.
*/
__MXAPI MXDLNA_USER_CHANNEL_LIST* mxDmc_loadChannel(MXDMC_HANDLE hDmc);		// 채널 값들을 가져옵니다.

/**
 mxDmc_loadChannel 함수에서 반환된 포인터의 메모리를 해제할 때 사용합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return N/A
*/
__MXAPI void  mxDmc_unloadChannel(MXDLNA_USER_CHANNEL_LIST* list);			// 채널 값들을 정리합니다.

/**
DMR device에서 MXDLNA_USER_RCS_EVENT_DATA 구조체 정보를 가져옵니다.
<pre>
typedef struct _MXDLNA_USER_RCS_EVENT_DATA
{
    int ChannelCount;
    char **Channel;
    int *Volume;
    int *Mute;

    unsigned short Brightness;
    unsigned short Contrast;
    short* VolumeDB;
    int* Loudness;

    int Subtitle;

} MXDLNA_USER_RCS_EVENT_DATA;
</pre>

  @param hDmc MXDMC_HANDLE 핸들.
  @return 성공하면 MXDLNA_USER_RCS_EVENT_DATA 구조체 포인터를 반환하고 실패시 NULL을 반환합니다.
        반환된 포인터는 mxDmc_unloadVolume 함수를 사용하여 메모리 해제해야 합니다.
*/
__MXAPI void *mxDmc_loadVolume(MXDMC_HANDLE hDmc);							// 볼륨 값들을 가져옵니다.

/**
mxDmc_loadVolume 함수에서 반환된 포인터의 메모리를 해제할 때 사용합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return N/A
*/
__MXAPI void mxDmc_unloadVolume(MXDLNA_USER_RCS_EVENT_DATA *temp);		// 볼륨 값들을 정리합니다.

/**
Audio Channel들의 음소거 정보(integer array)를 가져옵니다. array 크기는 mxDmc_getChannelCount 함수를 호출하여 얻어오는 audio channel 개수입니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return 성공하면 integer array정보를 반환하고 실패시 NULL을 반환합니다. 반환된 포인터는 mxDmc_unloadMute 함수를 사용하여 메모리 해제해야 합니다.
*/
__MXAPI int *mxDmc_loadMute(MXDMC_HANDLE hDmc, MXDLNA_UDN UDN);		// 음소거 값들을 가져옵니다.

/**
mxDmc_loadMute 함수에서 반환된 포인터의 메모리를 해제할 때 사용합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return N/A
*/
__MXAPI void mxDmc_unloadMute(int *mute);									// 음소거 값들을 정리합니다.



//------------------------------------------------------------------------
// @ RA Load Data 관련
//------------------------------------------------------------------------
/**

  @param
  @return
*/
__MXAPI int mxDmc_setRaOptions( char* serverip, int serverPort, char* local_public_ip );

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI int mxDmc_createRaClient(MXDMC_HANDLE hDmc, char* kt_id,char* kt_pwd, char* rada, MXDLNA_DEVICE_ADDREMOVE func );

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI int mxDmc_destroyRaClient(MXDMC_HANDLE hDmc);

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI MXDLNA_DEVICE_TINY* mxDmc_getRemoteDeviceList(MXDMC_HANDLE hDmc);

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI char* mxDmc_getRaID(MXDMC_HANDLE hDmc);

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI char* mxDmc_getDeviceRada( MXDMC_HANDLE hDmc, MXDLNA_UDN UDN);

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI char* mxDmc_getDeviceServer( MXDMC_HANDLE hDmc, MXDLNA_UDN UDN);

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI char* mxDmc_getDeviceMac( MXDMC_HANDLE hDmc, MXDLNA_UDN UDN);



//------------------------------------------------------------------------
// @ Resource Selection 관련
//------------------------------------------------------------------------
/**
디바이스 리스트에서 전달된 UDN에 해당하는 디바이스가 재생 가능한 목록만 검색하여 다시 전달함.
  @param hDmc MXDMC_HANDLE 핸들.
  @param udnType
  @param UND
  @param listType
  @param list
  @return
*/
__MXAPI MXDLNA_DEVICE_TINY *mxDmc_getDeviceFilter(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE udnType, MXDLNA_UDN UDN, MXDMC_DEVICE_TYPE listType, MXDLNA_DEVICE_TINY *list );	// DMS 디바이스의 정보와 DMR 디비아스 정보를 비교해서 재생할 수 있는 디바이스 리스트만 반환함.

/**
사용자 정의 CDS구조체의 multi resource에서 재생가능한 리소스의 URI를 반환합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type MXDLNA_SERVER_TYPE or MXDLNA_RENDERER_TYPE 값을 사용할 수 있습니다.
  @param UDN Device의 UDN.
  @param userCdsData 사용자 정의 CDS 구조체.
  @return 성공하면 Resource URI string을 반환하고 실패하면 NULL을 반환한다. 반환한 포인터는 메모리를 해제해야 한다.
*/
__MXAPI char *mxDmc_isRenewableMetaData(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, MXDLNA_UDN UDN, void *userCdsData);							// 프로토콜 인포를 확인해서 해당 디바이스가 재생할 수 있는지 확인합니다.

/**
  Device에서 재생 가능한지 확인합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type MXDLNA_SERVER_TYPE or MXDLNA_RENDERER_TYPE 값을 사용할 수 있습니다.
  @param UDN Device UDN string.
  @param contentPtcInfo ProtocolInfo String.
  @return
*/
__MXAPI int mxDmc_isRenewableProtocolInfo(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, MXDLNA_UDN UDN, char *contentPtcInfo);						// 메타데이터에 재생가능 리소스가 있는지 확인중. 

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI MXDLNA_CDSRESOURCE *mxDmc_loadCdsResource(MXDMC_HANDLE hDmc, MXDLNA_UDN serverUDN, MXDLNA_UDN rendererUDN, void *userCdsData);	// DMS, DMR, CDSData를 확인하고 CDSData중 재생 가능한 리소스만을 가지는 CDS Resource를 반환함.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI void mxDmc_unloadCdsResource(MXDLNA_CDSRESOURCE *cdsResource);																					// 사용한 완료한 Cds Resource를 정리합니다.



//------------------------------------------------------------------------
// @ PlaySingle 관련
//------------------------------------------------------------------------
#ifdef MXDLNA_PLAYSING_MODULE
#define MXDLNA_PLAYSINGLE			"dlna-playsingle"
#define MXDLNA_PLAYSINGLE_LENGTH	15

/**

  @param
  @return
*/
__MXAPI int  mxDmc_responseSetPlaySingleURL(void *CDSCP, void *hAction , void *userObject, int errorCode, int NumberReturned,int TotalNumber);			// 응답을 받고 응답받은 내용을 SetURI 합니다.

/**

  @param
  @return
*/
__MXAPI int  mxDmc_responseSetNextPlaySingleURL(void *CDSCP, void *hAction , void *userObject, int errorCode, int NumberReturned,int TotalNumber);		// 응답을 받고 응답받은 내용을 NextURI 합니다.

/**

  @param
  @return
*/
__MXAPI void mxDmc_setPlaySingleURL(MXDMC_HANDLE hDmc, char *playSingle, int order, MXDLNA_MSCP_BROWSE_RESULT_CUSTOM callback);							// PlaySingleURI값을 가져옵니다.
#endif



//------------------------------------------------------------------------
// @ PlayContainer 관련
//------------------------------------------------------------------------
#ifdef MXDLNA_PLAYCONTAINER_MODULE
/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI void mxDmc_playContainerSetURL(MXDMC_HANDLE hDmc, MXDLNA_CMID CMID, void *objectClass, void *object, int order, unsigned int maxDepth);		// 플레이 컨테이너를 설정합니다.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI int  mxDmc_isDevicePlayContainer(MXDMC_HANDLE hDmc, MXDLNA_UDN UDN, MXDLNA_CMID CMID);	// 렌더러가 플레이어 컨테이너를 지원하는 확인합니다.
#endif



//------------------------------------------------------------------------
// @ MRCP 자막 관련
//------------------------------------------------------------------------
#ifdef MXDLNA_SUBTITLE

/**
DMR AVTransport Service에 SetSubtitleURL Action이 있는 경우 자막 URL을 설정합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO : 성공유무를 판단할 수 없음. 코드 수정이 필요함.
*/
__MXAPI int  mxDmc_setSubtitleURL(MXDMC_HANDLE hDmc, char *URL);										// 렌더러에 자막을 설정합니다.

/**
DMR AVTransport Service에 SetSubtitle Action이 있는 경우 자막 On/Off 합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO : 성공유무를 판단할 수 없음. 코드 수정이 필요함.
*/
__MXAPI int  mxDmc_setSubtitle(MXDMC_HANDLE hDmc, char *SubtitleState);									// 렌더러에 자막을 On/Off 합니다.

/**
DMR AVTransport Service에 SetSubtitleSync Action이 있는 경우 자막의 Sync를 조절합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return TO DO : 성공유무를 판단할 수 없음. 코드 수정이 필요함.
*/
__MXAPI int  mxDmc_setSubtitleSync(MXDMC_HANDLE hDmc, int SyncValue);									// 렌더러에 자막을 Sync를 맞춥니다. (-1/0/+1 등의 값을 줍니다.)
#endif



//------------------------------------------------------------------------
//	@ Download & Upload 관련
//------------------------------------------------------------------------
#if defined MXDLNA_UPLOAD_MODULE || defined MXDLNA_DOWNLOAD_MODULE
typedef void (*mxDmc_responseOpenObject)(void *session, int error_code, int http_error_code);
#endif

#ifdef MXDLNA_DOWNLOAD_MODULE
#include "mxdlna_upload.h"

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI void *mxDmc_downloadOpenObject											// 다운로드를 생성함.
	(MXDMC_HANDLE hDmc, void *Cds, int resume_flag, char *save_as_file_name, mxDmc_responseOpenObject callback_response, int mode);

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI void *mxDmc_downloadOpenObject2											// 다운로드를 생성함.
	(MXDMC_HANDLE hDmc, char *cURI, char *cProtocolInfo, int resume_flag, char *save_as_file_name, mxDmc_responseOpenObject callback_response, int mode);

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI int mxDmc_downloadPauseTransfer(void *session);							// 다운로드를 일시정지함.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI int mxDmc_downloadResumeTransfer(void *session);						// 다운로드를 다시시작함.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI int mxDmc_downloadAbortTransfer(void *session);							// 다운로드를 중지함.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI MX_INT64 mxDmc_downloadBytesReceived(void *session);					// 현재까지 받은 바이트 크기.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI MX_INT64 mxDmc_downloadTotalBytes(void *session);						// 다운로드 파일의 전체 바이트 크기.
#endif /*MXDLNA_DOWNLOAD_MODULE*/



#ifdef MXDLNA_UPLOAD_MODULE
#include "mxdlna_download.h"
#define MXDLNA_UPLOAD_ANYCONTAINER "DLNA.ORG_AnyContainer"

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI void *mxDmc_uploadOpenObject											// 업로드를 생성함.
	(MXDMC_HANDLE hDmc, MXDLNA_UDN UDN, char *parentID, int resume_flag, char *fullPath, mxDmc_responseOpenObject callback_response);

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI int mxDmc_uploadPauseTransfer(void *session);							// 업로드를 일시정지함.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI int mxDmc_uploadResumeTransfer(MXDMC_HANDLE hDmc, void *session);		// 업로드를 다시시작함.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI int mxDmc_uploadAbortTransfer(void *session);							// 업로드를 중지함.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI MX_INT64 mxDmc_uploadBytesSent(void *session);							// 현재까지 받은 업로드 크기.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI MX_INT64 mxDmc_uploadTotalBytes(void *session);							// 업로드 파일의 전체 바이트 크기.
#endif /*MXDLNA_UPLOAD_MODULE*/


//------------------------------------------------------------------------
//------------------------------------------------------------------------
/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI MXDLNA_UDN mxDmc_getCurrentDeviceUdn(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type);						// 마지막으로 선택된 디바이스 UDN

/**
DMS 리스트를 가져옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param rendereUDN (deprecated)사용하지않는 파라미터입니다. NULL.
  @return 성공하면 MXDLNA_DEVICE_TINY구조체 형태의 linked list를 반환하고 실패하면 NULL을 반환합니다. 반환된 포인터는 mxDmc_destroyDeviceList 함수를 사용하여 메모리 해제해야 합니다.
*/
__MXAPI MXDLNA_DEVICE_TINY *mxDmc_getServerList(MXDMC_HANDLE hDmc, char *rendereUDN);							// DMS 리스트를 받아옴.

/**
  mxDmc_getServerList 함수에서 반환한 device list에서 parameter device와 일치하는 device가 있는 경우, parameter addRemove = 1 인 경우 device list를 그대로 반환하고
  addRemove = 0이면 device list에서 parameter device정보를 제거하여 반환합니다.
  parameter device와 일치하는 정보가 없는 경우, addRemove=1 일때  device list 끝에서 parameter device의 정보를 추가하여 반환합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param device  device list에서 추가/삭제할 device.
  @param addRemove  1이면 device list에 추가하고 0이면 device list에서 제거합니다.
  @param rendereUDN (deprecated)사용하지않는 파라미터입니다. NULL.
  @return 성공하면 device list 를 반환하고 실패하면 NULL을 반환합니다.반환된 포인터는 mxDmc_destroyDeviceList 함수를 사용하여 메모리 해제해야 합니다.
*/
__MXAPI MXDLNA_DEVICE_TINY *mxDmc_getServerListEx(MXDMC_HANDLE hDmc, MXDLNA_DEVICE *device, int addRemove, char *rendereUDN);							// DMS 리스트를 받아옴.

/**
DMR 리스트를 가져옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param serverUDN (deprecated)사용하지않는 파라미터입니다. NULL.
  @return 성공하면 MXDLNA_DEVICE_TINY구조체 형태의 linked list를 반환하고 실패하면 NULL을 반환합니다. 반환된 포인터는 mxDmc_destroyDeviceList 함수를 사용하여 메모리 해제해야 합니다.
*/
__MXAPI MXDLNA_DEVICE_TINY *mxDmc_getRendererList(MXDMC_HANDLE hDmc, char *serverUDN);							// DMR 리스트를 받아옴.

/**
  mxDmc_getRendererList 함수에서 반환한 device list에서 parameter device와 일치하는 device가 있는 경우, parameter addRemove = 1 인 경우 device list를 그대로 반환하고
  addRemove = 0이면 device list에서 parameter device정보를 제거하여 반환합니다.
  parameter device와 일치하는 정보가 없는 경우, addRemove=1 일때  device list 끝에서 parameter device의 정보를 추가하여 반환합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param device  device list에서 추가/삭제할 device.
  @param addRemove  1이면 device list에 추가하고 0이면 device list에서 제거합니다.
  @param serverUDN (deprecated)사용하지않는 파라미터입니다. NULL.
  @return 성공하면 device list 를 반환하고 실패하면 NULL을 반환합니다.반환된 포인터는 mxDmc_destroyDeviceList 함수를 사용하여 메모리 해제해야 합니다.
*/
__MXAPI MXDLNA_DEVICE_TINY *mxDmc_getRendererListEx(MXDMC_HANDLE hDmc, MXDLNA_DEVICE *device, int addRemove, char *serverUDN);

/**
User Device list를 가져옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param deviceUDN (deprecated)사용하지않는 파라미터입니다. NULL.
  @return 성공하면 device list 를 반환하고 실패하면 NULL을 반환합니다.반환된 포인터는 mxDmc_destroyDeviceList 함수를 사용하여 메모리 해제해야 합니다.
*/
__MXAPI MXDLNA_DEVICE_TINY *mxDmc_getUserDeviceList(MXDMC_HANDLE hDmc, char *deviceUDN);

/**
MXDLNA_DEVICE_TINY 구조체의 linkedlist 메모리를 해제할 때 사용합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return N/A
*/
__MXAPI void mxDmc_destroyDeviceList(MXDLNA_DEVICE_TINY *list);												// DMS, DMR 리스트를 사용후 날림.

/**
  device의 @ref MXDMC_DEVICE_TYPE 에서 정의된 device Type 값을 반환합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @return 성공하면 MXDMC_DEVICE_TYPE enum값을 반환하고 실패하면 -1값을 반환합니다.
*/
__MXAPI MXDMC_DEVICE_TYPE mxDmc_getDeviceType(MXDLNA_DEVICE *dev);														// 디바이스 타입을 알아옵니다.

/**
ContentDirectory Service의 Browse Action에서 반환된 <DIDl-Lite> 형태의 XML 메타데이터를 MXDLNA_CDSOBJECT 구조체 포인터로 변환하여 줍니다.
<DIDL-Lite>에 여러개의 item의 존재하는 경우, MXDLNA_CDSOBJECT::next 에  LinkedList로 여러개의 Cds Object를 생성합니다.
  @param MetaData <DIDL_Lite>형태의 XML String.
  @return 성공하면 MXDLNA_CDSOBJECT 구조체 포인터를 반환하고 실패하면 NULL을 반합니다. 반환된 포인터는 mxDmc_releaseCdsDeserialize 함수를 사용하여 메모리 해제해야 합니다.
*/
__MXAPI MXDLNA_CDSOBJECT *mxDmc_makeCdsDeserialize(char *MataData);													// 메타데이터를 가지고 CDS Object를 만듬.

/**
mxDmc_makeCdsDeserialize 함수에서 반환한 포인터의 메모리를 해제할 때 사용합니다. Ref.count 값을 감소시켜서 0이 되었을 때 메모리를 해제합니다.
CdsObj->next로 연결된 Cds Object는 삭제하지 않습니다.
  @param CdsObj MXDLNA_CDSOBJECT 객체.
  @return CdsObj객체를 삭제하면 1를 반환하고, Ref.Count값만 1 감소했을 때는 0을 반환한다. 그외 에러가 발생하면 -1를 반환한다.
*/
__MXAPI int mxDmc_releaseCdsDeserialize(MXDLNA_CDSOBJECT *CdsObj);													// 생성된 CDS Object를 사용 후에 정리합니다.

/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI int  mxDmc_compatibleURL_HTTP(MXDMC_HANDLE hDmc, char *URL);												// URL이 외부로 나갈수 있는지 확인합니다.
//__MXAPI void *mxDmc_getMainConnectionData(MXDMC_HANDLE hDmc, MXDLNA_UDN UDN);								// 기본으로 연결되는 0번 Connection 정보를 가져옵니다.

/**
  SortCriteria integer값을 만든다.
  아래 함수들의 order 파라미터 값을 생성할 때 사용한다.
  <pre>
    mxDmc_openContentObjectHandle
    mxDmc_openSearchContentObjectHandle
    mxDmc_invokeBrowse
    mxDmc_openServerSearchObjectList
    mxDmc_openServerSearchObjectListEx
</pre>
  @param hDmc MXDMC_HANDLE 핸들.
  @return
*/
__MXAPI MX_INT32 mxDmc_setOrder(MXDLNA_SORT_ORDER a,MXDLNA_SORT_ORDER b,MXDLNA_SORT_ORDER c,MXDLNA_SORT_ORDER d);


// 새로 추가된 내용
/**
UDN 값을 가지는 device와 연결된 CP의 IP 정보를 가져옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @param UDN device의 UDN
  @return
*/
__MXAPI char *mxDmc_getConnectedHostIPAddress(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, char *UDN);		// DEVICE가 연결된 CP의 IP를 가져옵니다.

/**
UDN 값을 가지는 device의 IP 정보를 가져옵니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param type DMC_SERVER_TYPE,  DMC_RENDERER_TYPE 값중에 하나를 선택한다.
  @param UDN device의 UDN
  @return
*/
__MXAPI char *mxDmc_getConnectedDeviceIPAddress(MXDMC_HANDLE hDmc, MXDMC_DEVICE_TYPE type, char *UDN);		// CP에 연결된 Device의 IP를 가져옵니다.



#ifdef DMC_X_DO_COMMAND_SUPPORT
/**
DMR의 AVTransport Service의 X_DoCommand Action을 호출합니다.
이 Action은 Humax가 정의한 임의의 사용자 Action입니다. CP와 Humax가 만든 DMR간의 특정한 명령 protocol을 정의할 때 사용할 수 있습니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_X_DO_COMMAND_SINK, int ErrorCode, char* command)
  @param hDmc MXDMC_HANDLE 핸들.
  @param command DMR에 전송할 string.
  @return TO DO: 리턴값으로 실패 유무를 판단할 수 없습니다. 코드 수정이 필요함.
*/
__MXAPI int mxDmc_invoke_X_DoCommand(MXDMC_HANDLE hDmc, char *command);
#endif

#ifdef DMC_X_GET_FLING_INFO_SUPPORT
/**
DMR의 AVTransport Service의 X_GetFlingInfo Action을 호출합니다.
Action에 대한 응답은 MXDLNA_CP_CONFIG::DeviceResponse Callback 호출로 아래처럼 전달됩니다.
DeviceResponse(MXDLNACP*mrcp, MRCP_ACTION_HANDLE hAction, int Code=MXDLNA_MRCP_X_GET_FLING_INFO_SINK, int ErrorCode, MXDLNA_USER_SETURI_DATA* uri)
  @param hDmc MXDMC_HANDLE 핸들.
  @return 리턴값으로 실패 유무를 판단할 수 없습니다. 코드 수정이 필요함.
*/
__MXAPI int mxDmc_invoke_X_GetFlingInfo(MXDMC_HANDLE hDmc);

/**
ContentDirectory Service의 Browse Action에서 반환된 <DIDl-Lite> 형태의 XML 메타데이터를 make_app_data 함수의 두번째 parameter로 사용되는 형태의 사용자 정의구조체로 변환합니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param metaData <DIDL_Lite>형태의 XML String.
  @return 성공하면 사용자 정의 CDS 구조체 포인터를 반환하고 실패하면 NULL을 반환합니다. 반환된 포인터는 mxDmc_freeUserCdsData 함수를 사용하여 메모리를 해제해야 합니다.
*/
__MXAPI void *mxDmc_makeUserCdsData(MXDMC_HANDLE hDmc, char *metaData);

/**
mxDmc_makeUserCdsData 함수 호출하여 반환된 포인터의 메모리를 해제할 때 사용합니다.
  @param userData 사용자 정의 CDS 구조체
  @return N/A
*/
__MXAPI void mxDmc_freeUserCdsData(void *userData);
#endif

#ifdef DMC_PAIRING_DEVICE_SUPPORT

/**
  Device description XML에서 <icube:X_WOON_PAIRING> element의 text 값을 반환합니다.
  반환된 String은 WOON을 사용하는 STB인 경우, PairingID로 사용합니다.
  @param hDmc mxDmc_create함수 호출후 반환된 핸들값.
  @param UDN Device의 UPnP UDN값.
  @return 성공하면 PairingID을 반환하고 에러시 NULL을 반환한다. 리턴값은 메모리해제하면 안된다.
  @example <icube:X_WOON_PAIRING xmlns:icube="urn:schemas-icube-co-kr:WoonDevice-1-0">hmx_base_000102034ee4</icube:X_WOON_PAIRING>
  @remarks 현재 HUMAX LiveTV 모바일앱은 STB이 Woon 지원 여부와 상관없이 이 함수를 호출하여 반환된 String을 STB의 Unique ID 값으로 사용하고 있다(UPnP Device UDN사용하지 않음).
        Woon사용하지 않는 경우 LiveTV 앱에서 코드 수정될 때까지는 STB의 Device description에 <icube:X_WOON_PAIRING> element가 반드시 존재해야 LiveTV와 연동할 수 있다.
*/
__MXAPI char *mxDmc_getPairingDeviceUDN(MXDMC_HANDLE hDmc, MXDLNA_UDN UDN);
#endif

/**
ContentDictory Service의 Browse Action으로 반환된 MXDLNA_CDSOBJECT 구조체 linkedlist를 사용자 정의 CDS 구조체 linkedlist로 바꿔줍니다.
  @param hDmc MXDMC_HANDLE 핸들.
  @param result MXDLNA_CDSOBJECT 구조체 타입의 linkedlist
  @return 성공하면 사용자 정의 CDS 구조체 포인터를 반환하고 실패하면 NULL을 반환한다. 반환된 포인터는 mxDmc_freeUserCdsDataList 함수를 사용하여 메모리를 해제해야 한다.
*/
__MXAPI void *mxDmc_cdsObjectToUserCdsDataList(MXDMC_HANDLE hDmc, MXDLNA_CDSOBJECT *result);

/**
mxDmc_cdsObjectToUserCdsDataList 함수에서 반환한 포인터의 메모리를 해제할 때 사용합니다.
  @param user_cds_object 사용자 정의 CDS 구조체
  @return N/A
*/
__MXAPI void mxDmc_freeUserCdsDataList(void *user_cds_object);

/**
사용자 CDS 구조체 linkedlist에서 next CDS 구조체 포인터를 반환합니다.
  @param user_cds_object 사용자 정의 CDS 구조체
  @return 성공하면 사용자 정의 CDS 구조체를 반환하고 Next CDS가 없거나 에러가 발생하면 NULL을 반환합니다.
*/
__MXAPI void *mxDmc_getNextCdsDataRef(void *user_cds_object);


//#ifdef ADD_USERINFO_AT_SOAPHEADER_FOR_BROWSE_20120612
/**

  @param hDmc MXDMC_HANDLE 핸들.
  @return N/A
*/
void mxDmc_setUserCommandAtBrowseSOAPHeader(char * userCommand);

/**


  @return
*/
char * mxDmc_getUserCommandAtBrowseSOAPHeader(void);
//#endif ADD_USERINFO_AT_SOAPHEADER_FOR_BROWSE_20120612


#ifdef __cplusplus
}
#endif

#endif	/*MXDLNA_MXDLNA_DMC_H*/
