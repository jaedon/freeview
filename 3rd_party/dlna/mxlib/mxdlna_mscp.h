
/*******************************************************************************
 * HUMAX R&D Lab., HUMAX INC., SEOUL, KOREA
 * COPYRIGHT(c) 2010 by HUMAX Inc.
 * 
 * All rights reserved. No part of this work may be reproduced, stored in a
 * retrieval system, or transmitted by any means without prior written
 * permission of HUMAX Inc.
 ******************************************************************************/

 


#ifndef		_DLNA_CDSCP_44232_
#define		_DLNA_CDSCP_44232_

// ---------------------------------------
#include "mxlib_config.h"
// ---------------------------------------

#include "mxapi_define.h"

#ifdef __cplusplus
extern "C"{
#endif


#include "mxkernel.h"
#include "mxtypes.h"
#include "mxdlna_struct.h"
#include "mxdlna_define.h"


//////////////////////////////////////////////////////////////////////////
// @ 정의
//////////////////////////////////////////////////////////////////////////

#define MSCP_ACTION_CHECK_TRUE									1
#define MSCP_ACTION_CHECK_FALSE									-5
#define MSCP_ACTION_CHECK_ERROR_TRANSITION_NOT_AVAILABLE		-701

//>> dypark : 2013-03-25 browse 관련 
#define MSCP_BROWSE_ERROR_DEVICE_REMOVED						-100
#define MSCP_BROWSE_ERROR_UPDATE_OBJECT							-200
//<< 


#define MXDLNA_ERROR_NO_MEDIASERVER								(-2)
#define MXDLNA_ERROR_SERVER_REMOVED								(-3)
#define MXDLNA_ERROR_CACHE_ERROR								(-4)
#define MXDLNA_ERROR_INVALID_OBJECT								(-5)
#define MXDLNA_ERROR_UNKNOWN									(-6)


#define	MXDLNA_RESPONSE_ERROR_ACTION_FAILED						501
#define	MXDLNA_RESPONSE_ERROR_NO_OBJECT							701
#define	MXDLNA_RESPONSE_ERROR_UNSUPPORTED_SEARCH				708
#define	MXDLNA_RESPONSE_ERROR_UNSUPPORTED_SORT					709
#define	MXDLNA_RESPONSE_ERROR_NO_CONTAINER						710
#define	MXDLNA_RESPONSE_ERROR_CANNOT_PROCESS_REQ				720
#define MXDLNA_RESPONSE_ERROR_TIMEOUT							408
#define MXDLNA_RESPONSE_ERROR_INTERRUPT							4			//WEBCLIENT_INTERRUPT
#define MXDLNA_RESPONSE_ERROR_INVALIDHEADER						999
#define MXDLNA_RESPONSE_ERROR_ACCESS_DENIED						801

/* 20131/4/2 mhkang: iOS용 mxlib에서 정의되었던 것인데, 일단 comment처리해서 merge함
#define MXDLNA_GSP_ARTIST										1
#define MXDLNA_GSP_GENRE										2
#define MXDLNA_GSP_ALBUMARTURI									3
#define MXDLNA_GSP_RATING										4
#define MXDLNA_GSP_DATE											5
#define MXDLNA_GSP_ACTOR										6

#define MXDLNA_GIP_ORIGINALTRACKNUMBER							1
*/

#define MXDLNA_MSCP_OLD_VERSION_SUPPORT


typedef enum 
{
	MXDLNA_SC_NONE = 0,
	MXDLNA_SC_AND = 1,
	MXDLNA_SC_OR
} MXDLNA_SEARCH_ADD_TYPE ;

typedef enum 
{
	MXDLNA_SC_NONE_SEARCH = 0,
	MXDLNA_SC_ITEM_TYPE_PHOTO = 1,			// argument is NULL don't care
	MXDLNA_SC_ITEM_TYPE_AUDIO,
	MXDLNA_SC_ITEM_TYPE_VIDEO,
	MXDLNA_SC_ITEM_TITLE_CONTAIN,			// argument is search word for title ex "carol"

//	MXDLNA_SC_ITEM_CREATEOR,				// argument is search word for createor ex "carol"
//	MXDLNA_SC_ITEM_PUBLISHER,				// argument is search word for publisher ex "carol"
//	MXDLNA_SC_ITEM_CONTRIBUTOR,				// argument is search word for contributor ex "carol"

	MXDLNA_SC_ITEM_ARTIST,					// argument is search word for artist ex "carol"
	MXDLNA_SC_ITEM_ACTOR,					// argument is search word for actor ex "carol"
	MXDLNA_SC_ITEM_AUTHOR,					// argument is search word for author ex "carol"
//	MXDLNA_SC_ITEM_PRODUCER,				// argument is search word for producer ex "carol"
//	MXDLNA_SC_ITEM_DIRECTOR,				// argument is search word for director ex "carol"

	MXDLNA_SC_ITEM_ALBUM,					// argument is search word for album ex "carol"
	MXDLNA_SC_ITEM_GENRE,					// argument is search word for genre ex "carol"

	MXDLNA_SC_ITEM_DATE_BEFORE,				// argument is date ex "2006-01-02" 
	MXDLNA_SC_ITEM_DATE_AFTER,				// argument is date ex "2006-01-02" 
	MXDLNA_SC_ITEM_AT_CONTAINER,			// argument is exact containerID 

	MXDLNA_SC_ITEM_TYPE,

#ifdef HUMAX_EPG_TUNER_FEATURE_20120328
	MXDLNA_SC_ITEM_SCHEDULEDSTARTTIME_BEFORE,
	MXDLNA_SC_ITEM_SCHEDULEDSTARTTIME_AFTER,
	MXDLNA_SC_ITEM_CHANNELNR_EQUAL,
#endif

} MXDLNA_SEARCH_TYPE ;

typedef enum
{
	MXDLNA_MSCP_BROWSEFLAG_METADATA = 0,	// browse metadata 
	MXDLNA_MSCP_BROWSEFLAG_CHILDREN = 1,		// browse children 
	
	MXDLNA_MSCP_BROWSEFLAG_MAKE_USER_DATA = 2,
	MXDLNA_MSCP_BROWSEFLAG_CHILDREN_MAKE_USER_DATA = 3,


} MXDLNA_BROWSE_FLAG;

typedef enum
{
	MXDLNA_TITLE_LIST_NO_PARENT = 0,
	MXDLNA_TITLE_LIST_EXCLUDE_PARENT_STRING = 1,
	MXDLNA_TITLE_LIST_INCLUDE_PARENT_STRING = 2,
} MXDLNA_TITLE_LIST_TYPE ;




typedef void*							MXDLNA_MSCP_OBJECTHANDLE;
typedef MXDLNA_INVOKE*					MSCP_ACTION_HANDLE; 
typedef struct AVRendererConnection*	MSCP_CONNECTION_HANDLE;
typedef MX_INT32						FREE_USER_DATA( void* userData);
typedef MX_INT32						MAKE_USER_DATA( MXDLNA_CDSOBJECT* object,void* userData);


//////////////////////////////////////////////////////////////////////////
// @ 구조체
//////////////////////////////////////////////////////////////////////////

typedef struct _MXDLNA_MSCP_CONTAINER_UPDATEID {
	char **ContainerID;
	char **ContainerUpdateID;
	int cnt;
} MXDLNA_MSCP_CONTAINER_UPDATEID ;


typedef struct _MXDLNA_MSCP_BROWSEARGS
{
	char *ObjectID;
	MXDLNA_BROWSE_FLAG BrowseFlag;
	char *Filter;             
	MX_UINT32 StartingIndex;
	MX_UINT32 RequestedCount;
	MX_UINT32 SortCriteria;
	char *SearchCriteria;

	void *Callback;
	void *UserObject;

	void *device;
	char *browseFlagString;
	char *sortString;

} MXDLNA_MSCP_BROWSEARGS;

typedef struct _MXDLNA_USER_CREATEOBJECT {
	char* ObjectID;
	MXDLNA_CDSOBJECT *ObjectData;
} MXDLNA_USER_CREATEOBJECT;

typedef struct _MXDLNA_USER_GET_TRANSFER_PROGRESS {
	char *TransferStatus;
	MX_INT64 TransferLength;
	MX_INT64 TransferTotal;
} MXDLNA_USER_GET_TRANSFER_PROGRESS;

typedef struct _DLNA_MSCP_OPTION
{
	MX_INT32  user_data_size;
	MXDLNA_TITLE_LIST_TYPE	user_parentfolder_title_type;
	char user_parentfolder_title[10];

	MAKE_USER_DATA * user_make;
	FREE_USER_DATA * user_free;
} MXDLNA_MSCP_OPTION;

typedef struct _MXDLNA_PLAY_SINGLE_PARSER {
	MXDLNA_UDN UDN;
	char *SID;
	char *IID;
} MXDLNA_PLAY_SINGLE_PARSER;

typedef void (*MXDLNA_MSCP_BROWSE_RESULT) (MXDLNA_CP_HANDLE hCp, MXDLNA_MSCP_OBJECTHANDLE hObject, void* data, int error, int numberReturned,int totalNumber);
typedef void (*MXDLNA_MSCP_BROWSE_RESULT_CUSTOM) ( MXDLNA_CP_HANDLE CDSCP,MXDLNA_MSCP_BROWSEARGS *args,MXDLNA_CDSOBJECT* result, int errorCode, int NumberReturned,int TotalNumber);

typedef void (*MXDLNA_MSCP_PURE_BROWSE_RESULT) (void *CP, char *UDN, int ErrorCode,void *User,char* Result,unsigned int NumberReturned, unsigned int TotalMatches,unsigned int UpdateID, void *UserData);


//////////////////////////////////////////////////////////////////////////
// @ 함수 
//////////////////////////////////////////////////////////////////////////

// @ MSCP create/destroy
__MXAPI MXDLNA_CP_HANDLE mxDlnaMscp_createMsCp(void);
__MXAPI MX_INT32 mxDlnaMscp_destroyMsCp(MXDLNA_CP_HANDLE mscp);



// @ MSCP Config
__MXAPI void mxDlnaMscp_configure(MXDLNA_CP_HANDLE mscp, MXDLNA_CP_CONFIG *conf);
__MXAPI void mxDlnaMscp_unconfigure(MXDLNA_CP_HANDLE mscp);
__MXAPI void mxDlnaMscp_saveParent(MXDLNA_CP_HANDLE mscp, MXDLNA_HANDLE parent);



// @ MSCP Start & End
__MXAPI int mxDlnaMscp_start(MXDLNA_CP_HANDLE mscp);
__MXAPI int mxDlnaMscp_end(MXDLNA_CP_HANDLE mscp);



// @ MSCP Action Context
__MXAPI MSCP_ACTION_HANDLE mxDlnaMscp_createContext(MXDLNA_CP_HANDLE mscp, MXDLNA_UDN UDN, MXDLNA_CMID CMID);
__MXAPI void mxDlnaMscp_destroyContext(MSCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMscp_setUDN(MSCP_ACTION_HANDLE hAction, MXDLNA_UDN UDN);
__MXAPI int mxDlnaMscp_setCMID(MSCP_ACTION_HANDLE hAction, MXDLNA_CMID CMID);
__MXAPI int mxDlnaMscp_setUserTag(MSCP_ACTION_HANDLE hAction, MXDLNA_USER_TAG user);
__MXAPI MXDLNA_UDN mxDlnaMscp_getUDN(MSCP_ACTION_HANDLE hAction);
__MXAPI MXDLNA_CMID mxDlnaMscp_getCMID(MSCP_ACTION_HANDLE hAction);
__MXAPI MXDLNA_USER_TAG mxDlnaMscp_getUserTag(MSCP_ACTION_HANDLE hAction);


// @ MSCP Device 
__MXAPI void mxDlnaMscp_sendMSearch( MXDLNA_CP_HANDLE mscp);
__MXAPI MXDLNA_DEVICE_TINY *mxDlnaMscp_getMediaServerList( MXDLNA_CP_HANDLE mscp);
__MXAPI MXDLNA_DEVICE_TINY *mxDlnaMscp_getMediaServerListEx(MXDLNA_CP_HANDLE mscp, MXDLNA_DEVICE *device, int addRemove, int isRemote );
__MXAPI void mxDlnaMscp_destroyMediaServerList( MXDLNA_DEVICE_TINY *list);
__MXAPI MX_INT32 mxDlnaMscp_initServer( MSCP_ACTION_HANDLE hAction, MXDLNA_UDN UDN);
__MXAPI MXDLNA_DEVICE *mxDlnaMscp_getDeviceAtUDN( MXDLNA_CP_HANDLE mscp, MXDLNA_UDN UDN);
__MXAPI void mxDlnaMscp_freeDeviceAtUDN(MXDLNA_DEVICE *dev);
__MXAPI int mxDlnaMscp_isMediaServer( MXDLNA_DEVICE* dev);

__MXAPI int mxDlnaMscp_isAliveDevice( MXDLNA_CP_HANDLE mscp, MXDLNA_UDN UDN);

__MXAPI MXDLNA_SERVICE *mxDlnaMscp_getService(MXDLNA_CP_HANDLE mscp, MXDLNA_UDN UDN, const char* selectService);
__MXAPI void mxDlnaMscp_freeService(MXDLNA_SERVICE *service);								// 2012-12-05

__MXAPI MXDLNA_ACTION  *mxDlnaMscp_getAction(MXDLNA_CP_HANDLE mscp, MXDLNA_UDN UDN, const char *selectService, const char *actionName);
__MXAPI void mxDlnaMscp_freeAction(MXDLNA_ACTION *action);									// 2012-12-05

__MXAPI MXDLNA_STATE_VARIABLE *mxDlnaMscp_getStateVariable(MXDLNA_CP_HANDLE mscp, MXDLNA_UDN UDN, const char *selectService, const char *stateName);
__MXAPI void mxDlnaMscp_freeStateVariable(MXDLNA_STATE_VARIABLE *stateVariable);			// 2012-12-05

__MXAPI int mxDlnaMscp_isMscpAction(MXDLNA_CP_HANDLE mscp, MXDLNA_UDN UDN, const char *serviceName, const char *actionName);

__MXAPI MXDLNA_DEVICE_PROTOCOL_INFO  *mxDlnaMscp_loadProtocolInfo(MXDLNA_CP_HANDLE mscp, MXDLNA_UDN UDN);
__MXAPI void mxDlnaMscp_unloadProtocolInfo(MXDLNA_DEVICE_PROTOCOL_INFO  *ptcInfo);
__MXAPI int mxDlnaMscp_compatibleContent2ProtocolInfo(MXDLNA_CP_HANDLE mscp, MXDLNA_UDN UDN, char *contentPtcInfo);
__MXAPI int mxDlnaMscp_get_X_DlnaCap(MXDLNA_CP_HANDLE cp, char * UDN);
__MXAPI char * mxDlnaMscp_get_UserDevCap(void* mscp, char * udn, char * NameSpace, char * UserCapName);
__MXAPI void *mxDlnaMscp_getContentDirectoryService(MXDLNA_CP_HANDLE mscp, char *udn);				// 내부적으로만 사용하길 바랍니다.
__MXAPI void mxDlnaMscp_freeContentDirectoryService(void * service);

// 2012-08-30, 새로 추가한 부분.
//__MXAPI char *mxDlnaMscp_getDeviceIPByUDN( MXDLNA_CP_HANDLE mscp, MXDLNA_UDN UDN); 를 변경함.
__MXAPI char *mxdlnaMscp_getHostIPAddress(MXDLNA_CP_HANDLE mscp, char *UDN);						// Device에 연결된 CP의 IP 주소를 반환한다
__MXAPI char *mxdlnaMscp_getDeviceIPAddress(MXDLNA_CP_HANDLE mscp, char *UDN);					// CP에 연결된 Device의 IP 주소를 반환한다.
__MXAPI int mxDlnaMscp_subscribeForUPnPEvents(MXDLNA_CP_HANDLE mscp, char *udn);					// 2012-07-16 추가함. subscribe 하다가 죽는 문제

// 2012-09-03 mklim modified - jcngs 추가 속성 구현 작업 - DLNA_USER_DEVICE_CAPABILITY 확장
__MXAPI char * mxDlnaMscp_get_UserDevCap(MXDLNA_CP_HANDLE mscp, char * udn, char * fullNameSpace, char * name);
// 2012-09-03 mklim modified - jcngs 추가 속성 구현 작업 - ip/mac address 접근
__MXAPI int mxDlnaMscp_IPtoMAC( char *ipAddress, char **macAddress );

//--------------------------------------------------------------------------------------------


// @ Content Directory Service - Browse, Search
__MXAPI MX_INT32 mxDlnaMscp_setOrder(MXDLNA_SORT_ORDER first,MXDLNA_SORT_ORDER second,MXDLNA_SORT_ORDER third,MXDLNA_SORT_ORDER fourth);
__MXAPI void mxDlnaMscp_makeOrderString(MX_UINT32 sortCriteria, char *sortString, int sortStringLength);
__MXAPI MX_INT32 mxDlnaMscp_supportSortOrder(MXDLNA_CP_HANDLE cdscp, MXDLNA_UDN  UDN, MXDLNA_SORT_ORDER type);
__MXAPI MXDLNA_MSCP_OBJECTHANDLE mxDlnaMscp_openCdsObjectHandle(MSCP_ACTION_HANDLE hAction, MXDLNA_MSCP_BROWSEARGS *args, int interval);
__MXAPI MX_INT32 mxDlnaMscp_closeCdsObjectHandle( MXDLNA_MSCP_OBJECTHANDLE handle );
__MXAPI MX_INT32 mxDlnaMscp_getDataFromHandle( MXDLNA_MSCP_OBJECTHANDLE handle, int idx, void** data );
__MXAPI char *mxDlnaMscp_makeMetaDataStringByCdsObject(MXDLNA_CDSOBJECT *obj, int *error);
__MXAPI MX_INT32 mxDlnaMscp_getDataCountFromHandle( MXDLNA_MSCP_OBJECTHANDLE handle );
__MXAPI MX_INT32 mxDlnaMscp_isObjectList( MXDLNA_MSCP_OBJECTHANDLE handle );
__MXAPI MX_INT32 mxDlnaMscp_isTitleList( MXDLNA_MSCP_OBJECTHANDLE handle );
__MXAPI char* mxDlnaMscp_getObjectID( MXDLNA_MSCP_OBJECTHANDLE handle );
__MXAPI MX_INT32 mxDlnaMscp_requestCdsOperation(MSCP_ACTION_HANDLE hAction, MXDLNA_MSCP_BROWSEARGS *args, MXDLNA_MSCP_BROWSE_RESULT_CUSTOM callback);
__MXAPI unsigned int mxDlnaMscp_getMediaClass(MXDLNA_CDSOBJECT *obj);
__MXAPI MXDLNA_OBJECT_CLASS_TYPE mxDlnaMscp_getRootClass( MXDLNA_CDSOBJECT* obj );
__MXAPI MXDLNA_OBJECT_MAJOR_TYPE mxDlnaMscp_getMajorClass( MXDLNA_CDSOBJECT* obj );
__MXAPI MXDLNA_OBJECT_MINOR_TYPE mxDlnaMscp_getMinorClass( MXDLNA_CDSOBJECT* obj );
__MXAPI const char* mxDlnaMscp_getStrProperty( MXDLNA_CDSOBJECT* obj, int property , int index ); 
__MXAPI int mxDlnaMscp_getIntProperty( MXDLNA_CDSOBJECT* obj, int property, int index );
__MXAPI MXDLNA_DEVICE_PROTOCOL_INFO  *mxDlnaMscp_getProtocolInfo(void *CDSCP, char* UDN);
__MXAPI MXDLNA_PLAY_SINGLE_PARSER *mxDlnaMscp_playSingleParser(char *playSingle);
__MXAPI void mxDlnaMscp_destroyPlaySingleParser(MXDLNA_PLAY_SINGLE_PARSER *playSingleData);
__MXAPI char *mxDlnaMscp_makePlayContainer(void *handle, char *CID, char *FID, char *FII, char *SortCriteria, char *MaxDepth);
__MXAPI char *mxDlnaMscp_makeSearchCriteriaString(char *SearchCriteriaData, MXDLNA_SEARCH_TYPE type, char *argument, MXDLNA_SEARCH_ADD_TYPE addtype);
__MXAPI int mxDlnaMscp_isSearchAction(MXDLNA_CP_HANDLE mscp, MXDLNA_UDN UDN);



// @ Content Directory Service - etc
__MXAPI int mxDlnaMscp_createObject(MSCP_ACTION_HANDLE hAction, char *ProtocolInfo, char *ContainerID, char *Title, unsigned int MediaClass, char *Extension);
__MXAPI int mxDlnaMscp_destroyObject(MSCP_ACTION_HANDLE hAction, char *ObjectID);
__MXAPI int mxDlnaMscp_importResource(MSCP_ACTION_HANDLE hAction, char *SourceURI, char *DestinationURI);
__MXAPI int mxDlnaMscp_exportResource(MSCP_ACTION_HANDLE hAction, char *SourceURI, char *DestinationURI);
__MXAPI int mxDlnaMscp_getTransferProgress(MSCP_ACTION_HANDLE hAction, char *TransferID);
__MXAPI int mxDlnaMscp_stopTransferResource(MSCP_ACTION_HANDLE hAction, char *TransferID);
__MXAPI int mxDlnaMscp_moveObject(MSCP_ACTION_HANDLE hAction, char* ObjectID, char *NewParentID);
__MXAPI int mxDlnaMscp_updateObject(MSCP_ACTION_HANDLE hAction, char* ObjectID, char* CurrentTagValue, char* NewTagValue);

__MXAPI char *mxDlnaMscp_getDeviceDescriptionURL(MSCP_ACTION_HANDLE hAction, MXDLNA_UDN udn);
__MXAPI int mxDlnaMscp_checkDevice(MSCP_ACTION_HANDLE hAction, char *deviceDescriptionURL);



#ifdef DMC_PAIRING_DEVICE_SUPPORT
char *mxDlnaMscp_GetPairingDeviceUDN(MXDLNA_CP_HANDLE mscp, MXDLNA_UDN udn);
#endif


// @ 구 버전 
#ifdef MXDLNA_MSCP_OLD_VERSION_SUPPORT
__MXAPI void mxDlnaMscp_config(MXDLNA_CP_CONFIG *conf);
__MXAPI MX_INT32 mxDlnaMscp_setCallBack( MXDLNA_MSCP_BROWSE_RESULT callBrowse, MXDLNA_DEVICE_ADDREMOVE callDevice, MXDLNA_DEVICE_EVENT_SINK callEvent);
__MXAPI MX_INT32 mxDlnaMscp_setOption( MXDLNA_MSCP_OPTION *opt );
__MXAPI MX_INT32 mxDlnaMscp_isSupportSortOrder(MXDLNA_CP_HANDLE cdscp, MXDLNA_SORT_ORDER type);
__MXAPI MX_INT32 mxDlnaMscp_setMediaServer( MXDLNA_CP_HANDLE CDSCP, MXDLNA_DEVICE_TINY* server );		
__MXAPI MX_INT32 mxDlnaMscp_setMediaServerByUDN( MXDLNA_CP_HANDLE CDSCP, char* udn );					
__MXAPI MX_INT32 mxDlnaMscp_setMediaServerByDevice( MXDLNA_CP_HANDLE CDSCP, MXDLNA_DEVICE* server );	
__MXAPI char* mxDlnaMscp_getCurrentServerUDN( MXDLNA_CP_HANDLE cp);										
__MXAPI MXDLNA_MSCP_OBJECTHANDLE mxDlnaMscp_openObjectHandle(void *cdscp, MXDLNA_MSCP_BROWSEARGS *args,int interval);	
__MXAPI void* mxDlnaMscp_openObjectHandleEx(void *cdscp, MXDLNA_MSCP_BROWSEARGS *args,int interval, char* udn); 
__MXAPI MX_INT32 mxDlnaMscp_closeObjectHandle( MXDLNA_MSCP_OBJECTHANDLE handle );	
__MXAPI MX_INT32 mxDlnaMscp_requestOperation(void *CDSCP, MXDLNA_UDN UDN, MXDLNA_MSCP_BROWSEARGS *args, MXDLNA_MSCP_BROWSE_RESULT_CUSTOM callback); 


// mxDlnaMscp_makeSearchCriteriaString로 대체 되는 함수임.. 코드 상에서는 아직 남아있습니다.
__MXAPI MX_INT32 mxDlnaMscp_makeSearchCriteria(char** des, MXDLNA_SEARCH_TYPE type, char* argument , MXDLNA_SEARCH_ADD_TYPE addtype);

// mxDlnaMscp_get_X_DlnaCap이 함수를 사용하는게 맞음.
__MXAPI int	mxDlnaMscp_get_X_DlnaCapByAction(MSCP_ACTION_HANDLE hAction);

// mxDlnaMscp_getDeviceByUDN를 사용하는게 맞음.
//120904 jhbae : modified getDeviceInfo to retain the MXDLNA_DEVICE structure while it is used outside of library. So after using this method, should call mxDlnaMscp_freeDeviceInfo
__MXAPI MXDLNA_DEVICE *mxDlnaMscp_getDeviceInfo(MSCP_ACTION_HANDLE hAction, MXDLNA_UDN udn);
__MXAPI void mxDlnaMscp_freeDeviceInfo(MXDLNA_DEVICE *deviceInfo);

// Cache를 지우기 위해서 임시로 공개된 API, 다음 함수에 대한 정리가 필요함.
// mxDlnaMscp_makeMetaDataStringByCdsObject 함수로 대체 됨.
__MXAPI char *mxDlnaMscp_makeCurrentURIMetaData(MXDLNA_CDSOBJECT *obj, int *error);

__MXAPI int mxDlnaMscp_cacheDelete(MXDLNA_CP_HANDLE mscp);


__MXAPI void mxDlnaMscp_undo_device_callback(void);
// 지워질 것으로 보임.
//extern int CacheDelete(void);
//__MXAPI void mxDlnaMscp_msearch(MSCP_ACTION_HANDLE hAction);
//__MXAPI int mxDlnaMscp_prepareForConnection(void *handle, int CMD);
//__MXAPI void mxDlnaMscp_undo_device_callback();
//__MXAPI char *mxDlnaMscp_makePeerConnectionManagerData(MSCP_ACTION_HANDLE hAction); // 정의만 있고 원형은 없음.
//__MXAPI void *mxDlnaMscp_loadConnectionIDs(MSCP_ACTION_HANDLE hAction); // 정의만 있고 원형은 없음.

#endif

#ifdef HDRL_CP_SUPPORT
#include "mxdlna_mscp_hdrl.h"
#endif

#ifdef XSRS_CP_SUPPORT
#include "mxdlna_mscp_xsrs.h"
#endif

// 201206 NewAPI 
// Browse 시 SOAP Header에 사용자 명령을 추가할 수 있다. (JLab Spec의 Parental Rating 관련 문자열을 추가할 때 사용)
__MXAPI void mxDlnaMscp_setUserCommandAtBrowseSOAPHeader(char * userCommand);
__MXAPI char * mxDlnaMscp_getUserCommandAtBrowseSOAPHeader(void);


__MXAPI int mxDlnaMscp_GetUserDataSize(MXDLNA_CP_HANDLE mscp);


__MXAPI MX_INT32 mxDlnaMscp_invokePureBrowse( void *CDSCP, char *UDN, char *ObjectID, char *BrowseFlag, char *Filter, int StartingIndex, int RequestedCount, char *SortCriteria, MXDLNA_MSCP_PURE_BROWSE_RESULT CB, void *UserData);

__MXAPI int mxDlnaMscp_getSortCapabilities(
	MSCP_ACTION_HANDLE hAction, void (*getSortCapabilitiesResponsePtr)(void *sender,int ErrorCode,void *user,char* SortCaps));

#ifdef __cplusplus
}
#endif

#endif
