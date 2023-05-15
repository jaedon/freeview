

#ifndef __MXDLNA_MSCP_HDRL_H__
#define __MXDLNA_MSCP_HDRL_H__

// Warning: mxdlna_mscp.h 에서만 include 하도록 한다.


#ifdef __cplusplus
extern "C"{
#endif



#ifdef HDRL_CP_SUPPORT



#define MSCP_HDRLACTION_OK									(0)
#define MSCP_HDRLACTION_SUCCESS								(1)
#define MSCP_HDRLACTION_ERROR								(-100)
#define MSCP_HDRLACTION_ERROR_PARAMISNULL					(MSCP_HDRLACTION_ERROR-1)
#define MSCP_HDRLACTION_ERROR_UDNISNULL						(MSCP_HDRLACTION_ERROR-2)
#define MSCP_HDRLACTION_ERROR_COULDNT_FIND_MEDIASERVER		(MSCP_HDRLACTION_ERROR-3)
#define MSCP_HDRLACTION_ERROR_DONTHAVEACTION				(MSCP_HDRLACTION_ERROR-4)
#define MSCP_HDRLACTION_ERROR_WRONGPARAM					(MSCP_HDRLACTION_ERROR-5)



typedef struct _MXDLNA_MSCP_ARGS_HDRLGETRECORDCONTAINER
{
	char * title;
	char * parentID;
	char * upnpClass;
	char * aribObjectType;
	char * protocolInfo;	
	char * date;
}MXDLNA_MSCP_ARGS_HDRLGETRECORDCONTAINER;




// 해당 UDN을 가진 서버가, HDRL기능을 지원하는 서버인지 확인하는 함수이다.
// CP Handle에 설정된 UDN이나, 매개변수의 UDN 둘중 하나의 값만 정확하면 된다.
__MXAPI 
int mxDlnaMscp_isHDRLMediaServer( 
	MXDLNA_CP_HANDLE cp, 
	char * pUDN
);


/* ************************************************************************************************* */
// Action 요청
// 요청한 Action들은 mxDlnaMscp_config()에서 등록한 DeviceResponse callback 함수로 결과가 전달된다.
// HDRL에서 정의하는 각각의 Action의 Commnad ID는 다음과 같다.
// MXDLNA_MSCP_X_HDLNKGETRECORDDESTINATIONS
// MXDLNA_MSCP_X_HDLNKGETRECORDDESTINATIONINFO
// MXDLNA_MSCP_X_GETDLNAUPLOADPROFILES
// MXDLNA_MSCP_X_HDLNKGETRECORDCONTAINERID
/* ************************************************************************************************* */

/*HDRL의 RecordDestinations 즉, 녹화할 수 있는 장소들에 대한 대략적인 정보를 요청하는 API이다.
매개변수: 
	hAction:
	- mxDlnaMscp_createContext()를 이용하여 만든 Action Handle를 사용한다.
	pInUDN:
	- HDRL를 지원하는 DMS의 UDN을 명시적으로 전달시, 해당 DMS에 액션을 날린다.
	- NULL를 입력시, SetMediaServer로 설정된 DMS에 액션을 날린다.
*/
__MXAPI 
int mxDlnaMscp_HDRL_X_HDLnkGetRecordDestinations(
	MSCP_ACTION_HANDLE hAction,
	char * pInUdn
);

/*HDRL의 RecordDestination 즉, 녹화할 수 있는 장소들에 대한 세부적인 정보를 요청하는 API이다.
매개변수: 
	hAction:
	- mxDlnaMscp_createContext()를 이용하면 만든 Action Handle를 사용한다.
	pInUDN:
	- HDRL를 지원하는 DMS의 UDN을 명시적으로 전달시, 해당 DMS에 액션을 날린다.
	- NULL를 입력시, SetMediaServer로 설정된 DMS에 액션을 날린다.
	pInRecordDestID:
	- mxDlnaMscp_HDRL_X_HDLnkGetRecordDestinations()를 이용하여 얻은 RecordDestID 중 세부적인 정보를 알기 원하는 RecordDestID를 전달한다.
*/
__MXAPI 
int mxDlnaMscp_HDRL_X_HDLnkGetRecordDestinationInfo(
	MSCP_ACTION_HANDLE hAction,
	char * pInUdn,
	char * pInRecordDestID
);

/*HDRL를 지원하는 서버에 Upload를 할 수 있는 Media Profile에 대한 정보를 요청하는 API이다.
매개변수: 
	hAction:
	- mxDlnaMscp_createContext()를 이용하면 만든 Action Handle를 사용한다.
	pInUDN:
	- HDRL를 지원하는 DMS의 UDN을 명시적으로 전달시, 해당 DMS에 액션을 날린다.
	- NULL를 입력시, SetMediaServer로 설정된 DMS에 액션을 날린다.
	pInFilterProfiles:
	- 원하는 프로파일만 존재하는지 알고 싶은경우, 해당 프로파일을 입력한다.
	- NULL를 입력시, 서버가 지원하는 모든 Media Profile들에 대한 리스트를 넘겨준다.
*/
__MXAPI 
int mxDlnaMscp_HDRL_X_GetDLNAUploadProfiles(
	MSCP_ACTION_HANDLE hAction,
	char * pInUdn,
	char * pInFilterProfiles
);


/*HDRL를 지원하는 서버에 Upload를 할 위치를 묻는 Action이다. (AnyContainer를 지원하는 서버인 경우, 이 값을 얻어올 필요는 없다.)
매개변수: 
	hAction:
	- mxDlnaMscp_createContext()를 이용하면 만든 Action Handle를 사용한다.
	pInUDN:
	- HDRL를 지원하는 DMS의 UDN을 명시적으로 전달시, 해당 DMS에 액션을 날린다.
	- NULL를 입력시, SetMediaServer로 설정된 DMS에 액션을 날린다.
	pInRecordDestID:
	- mxDlnaMscp_HDRL_X_HDLnkGetRecordDestinations()를 이용하여 얻은 RecordDestID 중 Upload를 할 수 있는 ContainerID 정보를 알기 원하는 RecordDestID를 전달한다.
	pInArgs
	- 실제 CreateObject를 수행할때 필요한 값들을 MXDLNA_MSCP_ARGS_HDRLGETRECORDCONTAINER를 사용하여 전달한다.
*/
__MXAPI 
int mxDlnaMscp_HDRL_X_HDLnkGetRecordContainerID(
	MSCP_ACTION_HANDLE hAction,
	char * pInUdn,
	char * pInRecordDestID, 
	MXDLNA_MSCP_ARGS_HDRLGETRECORDCONTAINER * pInArgs
);

#endif // HDRL_CP_SUPPORT



#ifdef __cplusplus
}
#endif


#endif // __MXDLNA_MSCP_HDRL_H__