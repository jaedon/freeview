#ifndef __MXDLNA_USERCP__
#define __MXDLNA_USERCP__

#ifdef __cplusplus
extern "C"{
#endif

#include "mxapi_define.h"
#include "mxdlna_struct.h"
#include "mxtypes.h"


//////////////////////////////////////////////////////////////////////////
// @ 정의
//////////////////////////////////////////////////////////////////////////
#define USERCP_ACTION_CHECK_TRUE									1
#define USERCP_ACTION_CHECK_FALSE									-5
#define USERCP_ACTION_CHECK_ERROR_TRANSITION_NOT_AVAILABLE		-701

#define USERCP_LENGTH_TRACKPOSITION 32


//////////////////////////////////////////////////////////////////////////
// @ 구조체
//////////////////////////////////////////////////////////////////////////

typedef MXDLNA_INVOKE* USERCP_ACTION_HANDLE;



//////////////////////////////////////////////////////////////////////////
// @ 함수
//////////////////////////////////////////////////////////////////////////

// @ upnpcp create/destroy
__MXAPI MXDLNA_CP_HANDLE mxDlnaUserCp_createUserCp(void);
__MXAPI void mxDlnaUserCp_destroyUserCp(MXDLNA_CP_HANDLE upnpcp);



// @ upnpcp Config
__MXAPI void mxDlnaUserCp_configure(MXDLNA_CP_HANDLE upnpcp, MXDLNA_CP_CONFIG *conf);
__MXAPI void mxDlnaUserCp_unconfigure(MXDLNA_CP_HANDLE upnpcp);
__MXAPI void mxDlnaUserCp_saveParent(MXDLNA_CP_HANDLE upnpcp, MXDLNA_HANDLE parent);



// @ upnpcp Start & End
__MXAPI int mxDlnaUserCp_start(MXDLNA_CP_HANDLE upnpcp);
__MXAPI int mxDlnaUserCp_end(MXDLNA_CP_HANDLE upnpcp);



// @ upnpcp Action Context
__MXAPI USERCP_ACTION_HANDLE mxDlnaUserCp_createContext(MXDLNA_CP_HANDLE mscp, MXDLNA_UDN UDN, MXDLNA_CMID CMID);
__MXAPI void mxDlnaUserCp_destroyContext    (USERCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaUserCp_setUDN             (USERCP_ACTION_HANDLE hAction, MXDLNA_UDN UDN);
__MXAPI MXDLNA_UDN mxDlnaUserCp_getUDN      (USERCP_ACTION_HANDLE hAction);

__MXAPI MXDLNA_USER_TAG mxDlnaUserCp_getUserTag(USERCP_ACTION_HANDLE hAction);



// @ Device Function
__MXAPI void mxDlnaUserCp_sendMSearch(MXDLNA_CP_HANDLE upnpcp);
__MXAPI MXDLNA_DEVICE_TINY *mxDlnaUserCp_geUserDeviceList(MXDLNA_CP_HANDLE upnpcp);
__MXAPI MXDLNA_DEVICE_TINY *mxDlnaUserCp_getUserDeviceListEx(MXDLNA_CP_HANDLE upnpcp, MXDLNA_DEVICE *device, int addRemove);
__MXAPI void mxDlnaUserCp_destroyUserDeviceList(MXDLNA_DEVICE_TINY *list);
__MXAPI int mxDlnaUserCp_initUserDevice(USERCP_ACTION_HANDLE hAction, MXDLNA_UDN UDN);

// 2012-12-05 복사해서 전달하고 메모리를 정리하는 형태로 변경함.
__MXAPI MXDLNA_DEVICE *mxDlnaUserCp_getDeviceAtUDN( MXDLNA_CP_HANDLE upnpcp, MXDLNA_UDN UDN);
__MXAPI void mxDlnaUserCp_freeDeviceAtUDN(MXDLNA_DEVICE *device);		// 추가 함.

__MXAPI int mxDlnaUserCp_isUserDevice(MXDLNA_DEVICE* device);
__MXAPI int mxDlnaUserCp_isAliveDevice(MXDLNA_CP_HANDLE upnpcp, MXDLNA_UDN UDN);

__MXAPI MXDLNA_SERVICE *mxDlnaUserCp_getService(MXDLNA_CP_HANDLE upnpcp, MXDLNA_UDN UDN, const char* selectService);
__MXAPI void mxDlnaUserCp_freeService(MXDLNA_SERVICE *service);								// 2012-12-05

__MXAPI MXDLNA_ACTION  *mxDlnaUserCp_getAction(MXDLNA_CP_HANDLE upnpcp, MXDLNA_UDN UDN, const char *selectService, const char *actionName);
__MXAPI void mxDlnaUserCp_freeAction(MXDLNA_ACTION *action);									// 2012-12-05

__MXAPI MXDLNA_STATE_VARIABLE *mxDlnaUserCp_getStateVariable(MXDLNA_CP_HANDLE upnpcp, MXDLNA_UDN UDN, const char *selectService, const char *stateName);
__MXAPI void mxDlnaUserCp_freeStateVariable(MXDLNA_STATE_VARIABLE *stateVariable);			// 2012-12-05

__MXAPI int mxDlnaUserCp_isUpnpCpAction(MXDLNA_CP_HANDLE upnpcp, MXDLNA_UDN UDN, const char *serviceName, const char *actionName);

__MXAPI int mxDlnaUserCp_get_X_DlnaCap(MXDLNA_CP_HANDLE upnpcp, MXDLNA_UDN UDN);



// 2012-08-30
__MXAPI char *mxDlnaUserCp_getHostIPAddress( MXDLNA_CP_HANDLE upnpcp, char* udn);
__MXAPI char *mxDlnaUserCp_getDeviceIPAddress( MXDLNA_CP_HANDLE upnpcp, char* udn);



#ifdef __cplusplus
}
#endif

#endif /*__MXDLNA_USERCP__*/


