#ifndef __SFD_MXRA_HEADER__

#define __SFD_MXRA_HEADER__

#ifdef __cplusplus
extern "C"{
#endif

#ifdef WIN32
#if defined(WINSOCK2)
	#include <winsock2.h>
	#include <ws2tcpip.h>
#elif defined(WINSOCK1)
	#include <winsock.h>
	#include <wininet.h>
#endif
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "mxlib_config.h"
#include "mxdlna_define.h"

enum MXRA_WID_REACHABLE {
	MXRA_WID_REACHABLE_FAIL = -1,
	MXRA_WID_REACHABLE_UNKNOWN = 0,
	MXRA_WID_REACHABLE_SUCCESS = 1,
};

// WOON ID의 현재 상태를 값이다.
typedef enum _MXRA_WID_STATUS
{
	MXRA_WID_STATUS_UNKNOWN,
	MXRA_WID_STATUS_WATING_FOR_REPLY,
	MXRA_WID_STATUS_NOT_USED,
	MXRA_WID_STATUS_ALIVE_ONLINE,
	MXRA_WID_STATUS_ALIVE_OFFLINE,
	MXRA_WID_STATUS_NOT_ALLOWED,
	MXRA_WID_STATUS_EXPIRED,

	MXRA_WID_STATUS_GENERATED,    //generated
	MXRA_WID_STATUS_CONNECTION_ERROR,
	MXRA_WID_STATUS_ARG_ERROR,

	MXRA_WID_STATUS_YOURS,
} MXRA_WID_STATUS;

// WOON ID 정보 중 일부와 Connection 서버와 통신을 하기 위한 데이터를 가진다.
typedef struct _MXRA_WID_CONNECTION_DATA
{
	char* wid ;
	char* desc;
	
	struct sockaddr_in public_addr;
	struct sockaddr_in local_addr;

	enum _MXRA_WID_STATUS status;

	int expired_remaining_date;
	enum MXRA_WID_REACHABLE reachable;  //20110829 

	int network_area;					//20130330
	
} MXRA_WID_CONNECTION_DATA;


//////////////////////////////////////////////////////////////////
// RETURN 
//////////////////////////////////////////////////////////////////
// 함수 사용시 반환 되는 코드 값이다.
enum MXRA_RETURN_CODE
{
	MXRA_OK							= MXDLNA_OK,
	MXRA_FALSE						= MXDLNA_FALSE,
	MXRA_ERROR						= MXDLNA_ERROR,
	MXRA_ERROR_NOT_PERMITTED		= (-23),
	MXRA_FILE_NOT_FOUND				= (-24),
	MXRA_ERROR_ERROR_WID_CAN_NOT_DELETEED_DUE_TO = 403,		// "COLLECTOR_ERROR_WID_CAN_NOT_DELETEED_DUE_TO should be changed to."
};



//////////////////////////////////////////////////////////////////
// EVENT
//////////////////////////////////////////////////////////////////
// RAS Event 발생시 Event 식별 값으로 사용된다.
typedef enum _MXRA_EVENT_TYPE
{
	MXRA_EVENT_WID_QUERY,
	MXRA_EVENT_WID_REGISTER,
	MXRA_EVENT_WID_UNREGISTER,
	MXRA_EVENT_WID_GENERATE,

	MXRA_EVENT_NETWORK_REVERSE_CONNECTION,
	MXRA_EVENT_NETWORK_EXPIRED_WID_OCCURED,
	MXRA_EVENT_NETWORK_CS_PUBLIC_IP,   // ConnectionServer  me Public IP
	MXRA_EVENT_NETWORK_LOCAL_IP,
	MXRA_EVENT_NETWORK_UPNP_PORT_FORWARDING,
	
	MXRA_EVENT_NETWORK_INFO_IDX,
	MXRA_EVENT_NETWORK_SETUP_IDX,
	MXRA_EVENT_NETWORK_REVERSE_IDX,
	
	MXRA_EVENT_WID_QUERY_PROGRASS,
	MXRA_EVENT_WID_REGISTER_PROGRASS,
	MXRA_EVENT_WID_UNREGISTER_PROGRASS,
	MXRA_EVENT_WID_GENERATE_PROGRASS,
	MXRA_EVENT_WID_LIST_IDX,
	MXRA_EVENT_WID_SET_DESCRIPTION_IDX,

	MXRA_EVENT_TOKEN,
	MXRA_EVENT_ACCOUNT_INFO,
	MXRA_EVENT_ACOUNT_IDX,
	MXRA_EVENT_FILES,
	MXRA_EVENT_METADATA,									// 이벤트를 상위에서 사용하지 않고, 속도 문제로 주석처리함. 필요시 다시 추가 고려.
	MXRA_EVENT_THUMBNAILS,									// 이벤트를 상위에서 사용하지 않고, 속도 문제로 주석처리함. 필요시 다시 추가 고려.
	MXRA_EVENT_FILEOPS_DELETE,
	MXRA_EVENT_FILEOPS_RENAME,
	MXRA_EVENT_FILEOPS_TRANSCODE_CHK,
	MXRA_EVENT_TRANSCODE,
	MXRA_EVENT_STREAMMING_SEND, 
	MXRA_EVENT_SEARCH,										// 이벤트를 상위에서 사용하지 않고, 속도 문제로 주석처리함. 필요시 다시 추가 고려.
	MXRA_EVENT_GROUP,

	MXRA_EVENT_DEVICE_LIST,									// 이벤트를 상위에서 사용하지 않고, 속도 문제로 주석처리함. 필요시 다시 추가 고려.
	MXRA_EVENT_DEVICE_SET,
	MXRA_EVENT_DEVICE_RESET,
	MXRA_EVENT_DEVICE_MSEARCH,
	
	MXRA_EVENT_PAIRING_INIT,
	
	MXRA_EVENT_PROGRAM_VERSION,
	
	MXRA_EVENT_NOTIFICATION,

	MXRA_EVENT_DEVICE_ADD,
	MXRA_EVENT_DEVICE_REMOVE,


	MXRA_EVENT_SHARED_BROWSE,
	MXRA_EVENT_SHARED_SET,
	MXRA_EVENT_SHARED_RESET,
	MXRA_EVENT_SHARED_GET_LIST,

	MXRA_EVENT_PORT_FORWARDING,
	MXRA_EVENT_WID_SET_PASSWORD_IDX,

} MXRA_EVENT_TYPE;


// RAS에서 사용하는 에러 코드 값이다.
typedef enum _MXRA_EVENT_ERROR_CODE
{
	MXRA_EVENT_OK,
	MXRA_EVENT_ERROR,
	MXRA_EVENT_ERROR_CONNECTION_FAIL,
} MXRA_EVENT_ERROR_CODE;

// RAS의 네트워크 상태 값이다.
enum MXRA_NETWORK_STATUS
{
	MXRA_NETWORK_UNKNOWN							= 100,
	MXRA_NETWORK_SUCCESS							= 200,
	MXRA_NETWORK_FAIL								= 300,
	MXRA_NETWORK_FAIL_FIREWALL						= 400,
	MXRA_NETWORK_FAIL_NEED_MANUAL_FORWARDING		= 500,
	MXRA_NETWORK_ERROR								= 600,
	MXRA_NETWORK_ERROR_PUBLIC_IP					= 601,

	MXRA_NETWORK_CHECKING							= 900,


	//>> 2013-03-06, UPnP WAN Device가 없고, manual forwarding도 안되는 상황. 
	MXRA_NETWORK_FAIL_AUTO_MANUAL_FORWARDING		= 501,
	//<<

};


// RASA에서 전달 하는 Error 코드 값입니다.
enum MXRA_RASA_ERROR
{
	MXRA_RASA_ERROR_MAX_WID							= -100,

};


//WOON 클라이언트로부터 Event 명령이 날라온 경우 등록된 Callback 을 호출한다. 이때 호출 되는 Callback의 정의이다.
typedef void (*MXRA_USER_EVENT_CALLBACK) (
	void *h, 
	enum _MXRA_EVENT_TYPE type, 
	enum _MXRA_EVENT_ERROR_CODE error, 
	void *data,
	void *user
);



typedef void (*MXRA_EVENT_CALLBACK) (void *h, void *user, MXRA_EVENT_TYPE type, MXRA_EVENT_ERROR_CODE error, MXRA_WID_CONNECTION_DATA *data, void* user2);


// WOON ID 정보를 가진다.
typedef struct _MXRA_WID_DATA 
{
	char *wid;
	int type;
	char *objectID;
	char *UDN;
	char *name;
	char *password;
	int permission;
	int status;

} MXRA_WID_DATA, *MXRA_WID_ARRAY, *MXRA_WID_ITEM;

typedef struct _RA_MGR_USER_DATA {
	void *user;
	void *cb;
} RA_MGR_USER_DATA;



enum RA_NETWORK {
	RA_NETWORK_INTERNAL = 0,
	RA_NETWORK_EXTERNAL = 1,
};



#ifdef __cplusplus
}
#endif


#endif
