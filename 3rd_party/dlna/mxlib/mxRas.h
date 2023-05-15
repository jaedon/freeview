
/*******************************************************************************
 * HUMAX R&D Lab., HUMAX INC., SEOUL, KOREA
 * COPYRIGHT(c) 2010 by HUMAX Inc.
 * 
 * All rights reserved. No part of this work may be reproduced, stored in a
 * retrieval system, or transmitted by any means without prior written
 * permission of HUMAX Inc.
 ******************************************************************************/

 

#ifndef ____RASERVER__H____ 
#define ____RASERVER__H____ 


#if defined(WIN32) || defined(_WIN32_WCE)
#if defined(WINSOCK2)
	#include <winsock2.h>
	#include <ws2tcpip.h>
#elif defined(WINSOCK1)
	#include <winsock.h>
	#include <wininet.h>
#endif
#else 
	#include <string.h>
	#include <pthread.h>
	#include <semaphore.h>
	#include <unistd.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <errno.h>
	#include <netdb.h>
	#include <fcntl.h>

	#define SOCKET	int

	#define INVALID_SOCKET		(SOCKET)(~0)
	#define SOCKET_ERROR			    (-1)

	#define	WSAGetLastError()	errno
	#define WSAEWOULDBLOCK		EWOULDBLOCK
	#define	WSAEINPROGRESS		EINPROGRESS
	#define SD_BOTH				SHUT_RDWR
	
	typedef struct sockaddr_in SOCKADDR_IN;
	typedef struct in_addr IN_ADDR;
	typedef struct sockaddr SOCKADDR;
	typedef struct linger LINGER;
	typedef struct timeval TIMEVAL;
	typedef struct hostent HOSTENT;

	#define closesocket(s)	close(s) 
#endif

#ifdef __cplusplus
extern "C"{
#endif

#include "mxapi_define.h"
#include "mxdlna_struct.h"
#include "mxdlna_mscp.h"
#include "mxdlna_dms.h"



#include "mxRa.h"

#define RAS_VERSION_LENGTH		12
#define RAS_VERSION				"V2.0"
#define RAS_DEFAULT_OPEN_PORT	 9251

//////////////////////////////////////////////////////////////////
// WOON ID 
//////////////////////////////////////////////////////////////////

// WOON ID의 용도 구분, Pairing위한 특수한 목적의 WOON ID 인 PairingID를 생성 할 때 사용한다.
enum MXRAS_WID_TYPE {
	MXRAS_WID_TYPE_NORMAL								= 0,
	MXRAS_WID_TYPE_PAIRING								= 4,
};

// WOON ID의 접근 권한 설정, 비트연산으로 중복 설정이 가능 하다.
enum MXRAS_WID_PERMISSION {
	MXRAS_WID_PERMISSION_READ							= 4,
	MXRAS_WID_PERMISSION_WRITE							= 2,
	MXRAS_WID_PERMISSION_EXECUTE						= 1
};

// Object의 타입
enum MXRAS_WID_OBJECT_TYPE {
	MXRAS_WID_OBJECT_TYPE_UNKNOW						= 0,
	MXRAS_WID_OBJECT_TYPE_CONTAINER						= 1,
	MXRAS_WID_OBJECT_TYPE_ITEM							= 2,
};


//////////////////////////////////////////////////////////////////
// DEVICE
//////////////////////////////////////////////////////////////////
// Device가 관리되고 있는 Device인지 상태를 확인 값
typedef enum _MXRAS_DEVICE_COLLECTION {
	MXRAS_DEVICE_COLLECTION_UNKNOWN						= -1,
	MXRAS_DEVICE_COLLECTION_UNUSED,
	MXRAS_DEVICE_COLLECTION_USED,
	MXRAS_DEVICE_COLLECTION_REMOVE,
} MXRAS_DEVICE_COLLECTION;

// Device가 현재 켜져 있는 지, 꺼져 있는 확인하는 값, UNKOWN 확인된 적 없는 상태로 OFF와 유사하다.
enum MXRAS_DEVICE_STATUS {
	MXRAS_DEVICE_STATUS_UNKOWN							= -1,
	MXRAS_DEVICE_STATUS_OFF,
	MXRAS_DEVICE_STATUS_ON,
};

// Device가 Search 명령을 내릴 수 있는지 확인 값 
enum MXRAS_DEVICE_SEARCH {
	MXRAS_DEVICE_SEARCH_UNKNOWN							= -1,
	MXRAS_DEVICE_SEARCH_OFF,
	MXRAS_DEVICE_SEARCH_ON,
};

/*
enum MXRAS_DEVICE_REMOVE {
	MXRAS_DEVICE_REMOVE_OFF								= 0,
	MXRAS_DEVICE_REMOVE_ON,
};
*/

// Device에 내릴 수 있는 Action 들이다. 비트연산을 통해서 중복해서 내릴 수 있다. 
// 예를 들면 1, 64, 384를 비트 연산으로 내리면 Add하는 액션을 Browse로 시작해서
// Search Container 그리고 Search item 순으로 순차적으로 실행한다.
// 논리적으로 맞지 않는 경우가장 처음에 내린 명령을 우선적으로 실행한다.

enum MXRAS_DEVICE_ACTION 
{
	MXRAS_DEVICE_ACTION_ADD								= 1,
	MXRAS_DEVICE_ACTION_DELETE							= 2,
	MXRAS_DEVICE_ACTION_UPDATE							= 4,

	MXRAS_DEVICE_ACTION_OPTION_BROWSE					= 64,
	MXRAS_DEVICE_ACTION_OPTION_SEARCH_ITEM				= 128,
	MXRAS_DEVICE_ACTION_OPTION_SEARCH_CONTAINER			= 256,
	MXRAS_DEVICE_ACTION_OPTION_SEARCH					= 384,
};

//////////////////////////////////////////////////////////////////
// NETWORK
//////////////////////////////////////////////////////////////////
// RAS를 실행할 때, 네트워크 설정 값이다. 
// 일반적으로 MXRAS_UPNP_PORT_FORWARDING을 사용한다. 
// MXRAS_MANUAL_SETTING일 경우, 사용자가 외부에서 접근 가능한 port를 알고 있어야 한다
typedef enum _MXRA_NETWORK_SETTING {
	MXRAS_MANUAL_SETTING							= 0,
	MXRAS_UPNP_PORT_FORWARDING						= 1,
	MXRAS_EXTERNAL_DISABLE							= 2,
	MXRAS_EXTERNAL_DISABLE_LIVE						= 4 + MXRAS_EXTERNAL_DISABLE,
	MXRAS_EXTERNAL_DISABLE_REC						= 8 + MXRAS_EXTERNAL_DISABLE,
	MXRAS_EXTERNAL_DISABLE_MEDIA					= 16 + MXRAS_EXTERNAL_DISABLE,
	MXRAS_EXTERNAL_DISABLE_ALL						= 4 + 8 + 16 + MXRAS_EXTERNAL_DISABLE,
} MXRA_NETWORK_SETTING;



//////////////////////////////////////////////////////////////////
// callback
//////////////////////////////////////////////////////////////////

// 관리 되는 DMS에서 가져오는 CDSObject가 추가/삭제 될 때, 호출 되는 Callback의 정의이다.
#define MXRAS_USER_COLLECT_CALLBACK		MXDLNA_DMS_CAI_ADD_REMOVE_FN


//>>
// 이 부분은 변경될 가능성이 있어서 문서에는 빠져있다.
//################################################################
// TRANSCODER
//################################################################
#define		MXRAS_TRANSCODER_MAX_FILENAME						1024 * 4
typedef struct _MXRAS_TRANSCODER_BUFFER 
{
	unsigned char	*ptr;
	unsigned int	size;
	char			localfilename[MXRAS_TRANSCODER_MAX_FILENAME]; //<<- confusing?
// --------------------------------------------------------//
} MXRAS_TRANSCODER_BUFFER, *MXRAS_TRANSCODER_BUFFER_HANDLE;

// transcoder 시작시 사용되는 함수
typedef int (*MXRAS_TRANSCODER_START_FUNCTION) (const char* session_id, char *device_name, const char* filename, const char* subtitle_filename);
// transcoder m3u8 파일명을 만드는데 사용되는 함수
typedef int (*MXRAS_TRANSCODER_M3U8_NAME_FUNCTION) (const char* session_id, const char* device_name, const char* filename, const char* subtitle_filename, char **szM3U8Name);
// transcoder m3u8 data를 가져오는데 사용되는 함수
typedef int (*MXRAS_TRANSCODER_M3U8_DATA_FUNCTION) (char* session_id, char* m3u8_filename, MXRAS_TRANSCODER_BUFFER_HANDLE m3u8_contents);
// transcoder segment 데이터를 가져오는데 사용되는 함수
typedef int (*MXRAS_TRANSCODER_GET_SEGMENT_FUNCTION) (const char* session_id, const char* ts_filename, MXRAS_TRANSCODER_BUFFER_HANDLE psegment_contents);
// transcoder error 처리를 하는데 사용되는 함수
typedef void (*MXRAS_TRANSCODER_ERROR_FUNCTION) (const char* session_id, char **pszError);
//<<

//////////////////////////////////////////////////////////////////
// STRUCT
//////////////////////////////////////////////////////////////////
// mxRas의 전체 구조체 이다.
typedef struct _MXRAS_DATA 
{
	int ReservedID;
	void *hRas;
	void *hWidMgr;
	void *hWanCp;
	void *hCollector;
	void *hDb;
	void *hDms;
	void *hFs;


	char *temp_path;
	char pairing_code[9];
	MXKERNEL_CRITICALSECTION cs;
	
	MXRA_USER_EVENT_CALLBACK ras_user_callback;

	MXRAS_TRANSCODER_START_FUNCTION	start_fn;
	MXRAS_TRANSCODER_M3U8_NAME_FUNCTION	m3u_name_fn;
	MXRAS_TRANSCODER_M3U8_DATA_FUNCTION m3u_data_fn;
	MXRAS_TRANSCODER_GET_SEGMENT_FUNCTION get_segment_fn;
	MXRAS_TRANSCODER_ERROR_FUNCTION error_fn;

	MXKERNEL_CRITICALSECTION csPairing;

	unsigned int network_flag;

} MXRAS_DATA, *MXRAS_HANDLE;

// Device의 정보를 가진다.
typedef struct _MXRAS_DEVICE
{
	int type;

	int ID;
	char *UDN;
	char *name;
//	char *objectID;
	char *deviceID;

	char *ip;
	int port;

	int onoff;
	int collection_flag;
	int search_flag;

//	int remove_flag;

	struct _MXRAS_DEVICE *pPrev;
	struct _MXRAS_DEVICE *pNext;


	// dypark, 2014-06-16 Connected Device에 WOON ID가 있는 경우에 붙여준다.
	MXRA_WID_ARRAY wids;
	int wids_cnt;

	int selected;

} MXRAS_DEVICE;




//////////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////////

/**	\brief	mxRas_setPairingID 
	\brief	RAS PairingID를  설정한다.
	\param	PairingID	: DMS를 통해서 Device Description에 전달될 Pairing ID를 설정합니다.
*/
__MXAPI 
void mxRas_setPairingID(char *PairingID);

/**	\brief	mxRas_getPairingID 
	\brief	RAS PairingID를 가져온다.
	\return	 char * 타입으로 현재 설정된 Pairing ID를 전달 합니다. */
__MXAPI 
char* mxRas_getPairingID(void);


/**	\brief	mxRas_init 
	\brief	RAS를 초기화 한다
	\param	UDN	: NULL을 주면 기본값으로 설정된다. RAS 서버의 식별자로 Connection 서버가 고유한 RAS를 인식하는데 사용된다. Pairing ID 와 유사하지만 다른 용도이다.  
	\param	hDms : DMS 핸들 
	\param	hFs	: DMS 파일 시스템 핸들 
	\param	db	: DB 핸들 
	\param	connection_ip : connection 서버 IP 주소나, 도메인을 적어준다. (ex:woon.humaxtvportal.com)
	\param	connection_port : connection 서버의 포트를 적어준다. (ex:80)
	\param	server_port : RAS 서버의 포트를 적어준다. 적어준 포트와 적어준 포트+1를 열어준다. (ex : 9251)
	\param	network_flag : UPnP Port Forwarding을 할지, 외부 네트워크를 막을 지 등을 설정해준다. 
	\param	ras_user_callback :	RAS에서 발생되는 이벤트를 전달하기 위한 콜백 함수 이다. 
	\param	collector_addremove_callback : 관리되는 DMS로 부터, CDS Object가 추가되면 발생하는 콜백이다. CDS Object 정보로 로컬에 Thumbnail등을 만드는데 사용된다.
	\return	    MXRAS_HANDLE 을 반환한다. */
__MXAPI 
MXRAS_HANDLE mxRas_init(char *UDN, void *hDms, void *hFs, void *db, 
	char *connection_ip, int connection_port, int server_port, 
	MXRA_NETWORK_SETTING network_flag,
	MXRA_USER_EVENT_CALLBACK ras_user_callback, 
	MXRAS_USER_COLLECT_CALLBACK collector_addremove_callback
);

/**	\brief	mxRas_deinit 
	\brief	RAS를 정리한다.
	\param	handle	:  MXRAS_HANDLE
	\return	성공이면 1을 실패이면 0 이하의 값을 전달한다.*/
__MXAPI 
int mxRas_deinit(MXRAS_HANDLE handle);

/**	\brief	mxRas_registerPairingID 
	\brief	ParingID정보를 Connection 서버로 전송한다. (밑에 단에서 하고 있지만 네트워크 장애나 다른 문제로 인하여 생성되지 않았을 때 호출 한다.)
	\return	사용안함. */
__MXAPI
int mxRas_registerPairingID(void);

/**	\brief	mxRas_existPairingID 
	\brief	PariingID가 생성되어있는지 확인한다.
	\return	성공 1, 실패 0 */
__MXAPI 
int mxRas_existPairingID(void);

/**	\brief	mxRas_setParingCode
	\brief	PIN CODE 를 저장한다.
	\param	handle : MXRAS_HANDLE
	\param	code : pairing을 요청하면, STB에 PIN CODE가 표현되고, 모바일에서 PIN CODE를 입력한다. 그때 사용하는 값을 적어준다.
	\return	성공 1, 실패 0 */
__MXAPI 
int mxRas_setParingCode(MXRAS_HANDLE handle, char *code);

/**	\brief	mxRas_checkPairingCode
	\brief	입력된 PIN CODE 값과 저장된 PIN CODE 값이 같은지 확인한다.
	\param	handle : MXRAS_HANDLE
	\param	code : 확인 할 PIN CODE 를 적는다. 
	\return	성공 1, 실패 0 */
__MXAPI 
int mxRas_checkPairingCode(MXRAS_HANDLE handle, char *code);


/**	\brief	mxRas_savePairing
	\brief	페어링 된 장비를 저장한다. (Pairing 과정의 여러 시나리오를 대응하기 위해서 만들었지만, 현재는 사용하지 않는다.)
	\param	handle : MXRAS_HANDLE
	\param	UDN : 클라이언트 고유 UDN
	\param	password : 클라이언트 비밀번호 
	\param	name : 클라이언트 설명 
	\return	 */
__MXAPI 
char *mxRas_savePairing(MXRAS_HANDLE handle, char *UDN, char *password, char *name);

/**	\brief	mxRas_setServerName
	\brief	서버 이름을 설정한다. 
	\param	handle : MXRAS_HANDLE
	\param	server_name : 서버 이름
	\return	성공 1, 실패 0 이하의 값 */
__MXAPI 
int mxRas_setServerName(MXRAS_HANDLE handle, char *server_name);

/**	\brief	mxRas_getServerName
	\brief	설정된 서버 이름을 가져온다.. 
	\param	handle : MXRAS_HANDLE
	\return	설정된 서버 이름 */
__MXAPI 
char *mxRas_getServerName(MXRAS_HANDLE handle);

/**	\brief	mxRas_setSSLPath
	\brief	SSL Key Path를 설정한다.
	\param	pem_fullpath : 인증서 경로
	\param	key_fullpath : 개인키 경로 
	\return	성공 1, 실패 0 이하의 값 */
__MXAPI 
int mxRas_setSSLPath(char* pem_fullpath, char* key_fullpath);


//////////////////////////////////////////////////////////////////
// DEVICE
//////////////////////////////////////////////////////////////////

/**	\brief	mxRas_device_msearch
	\brief	DMS를 검색하기 위해서 msearch 명령을 수행한다.
	\param	handle : MXRAS_HANDLE */
__MXAPI
void mxRas_device_msearch(MXRAS_HANDLE handle);

/**	\brief	mxRas_device_getList
	\brief	MXRAS DEVICE List를 가져 온다.
	\param	handle : MXRAS_HANDLE 
	\return	Device list  */
__MXAPI 
MXRAS_DEVICE *mxRas_device_getList(MXRAS_HANDLE handle);

/**	\brief	mxRas_device_freeList
	\brief	사용한 MXRAS DEVICE를 정리한다.
	\param	device_list : 디바이스 리스트를 전달한다. */
__MXAPI
void mxRas_device_freeList(MXRAS_DEVICE *device_list);

/**	\brief	mxRas_device_select
	\brief	Home Network에 보이는 DMS를 관리하는 Device로 등록/삭제한다.
	\param	handle : MXRAS_HANDLE
	\param	UDN : 관리되는 Device로 등록할 DMS의 UDN 
	\param	collection_flag : Device 등록/삭제 설정 값.
	\param	force_flag : Device에 WOONID가 있어도 제거 하는 옵션
	\return	MXDLNA_OK 또는 MXDLNA_ERROR */
__MXAPI 
int mxRas_device_select(MXRAS_HANDLE handle, char *UDN, MXRAS_DEVICE_COLLECTION collection_flag, int force_flag);

/**	\brief	mxRas_device_getHttpAddress
	\brief	DMS의 미디어에 접근 가능한 IP와 Port로 HTTP 주소를 만들어준다.
	\param	handle : MXRAS_HANDLE
	\param	UDN : DMS의 UDN, deviceID를 모르는 경우에 사용된다.
	\param	deviceID : Device의 고유 Object ID를 전달한다. UDN를 알지 못하는 경우에 사용된다. 
	\return	Media의 Base URL를 생성해서 전달한다. */
__MXAPI 
char *mxRas_device_getHttpAddress(MXRAS_HANDLE handle, char *UDN, char *deviceID);

/**	\brief	mxRas_device_action
	\brief	관리되는 DMS에 Scanning 시작/중지 명령을 내릴 수 있다.
	\param	handle : MXRAS_HANDLE
	\param	UDN : DMS의 UDN
	\param	objectID : 검색을 시작 할 ObjectID를 적어준다. 대부분 "0"을 사용한다.
	\param	db_objectID :DB에 설정되는 ObjectID를 적어준다.
	\param	deviceID : Device Object ID를 설정해 준다. 관리되는 Device는 모두 할당받는다.
	\param	action : 스캐닝 방식 및 동작을 설정해준다. 
	\param	force_flag : 강제로 동작하기 위한 플래그 이다. 현재는 사용하지 않는다.
	\return	MXDLNA_OK 또는 MXDLNA_ERROR */
__MXAPI 
int mxRas_device_action(MXRAS_HANDLE handle, char *UDN, char *objectID, char *db_objectID, char *deviceID, int action, int force_flag);

/**	\brief	mxRas_device_get_info
	\brief	관리되는 DMS에 Device 정보를 가져온다. (현재 사용하고 있지 않은 함수이다. 테스트를 위해서 만들었습니다. 사용하려면 free함수를 만들어줘야 합니다.)
	\param	handle : MXRAS_HANDLE
	\param	UDN : DMS의 UDN
	\return	Device 정보 전달한다.*/
__MXAPI 
MXRAS_DEVICE *mxRas_device_get_info(MXRAS_HANDLE handle, char *UDN);


//////////////////////////////////////////////////////////////////
// NETWORK
//////////////////////////////////////////////////////////////////

/**	\brief	mxRas_reverseConnection
	\brief	Reverse connection test를 요청한다.
	\param	handle : MXRAS_HANDLE
	\param	user : 리턴될 때, 전달되는 사용자 데이터 */
__MXAPI 
void mxRas_reverseConnection(MXRAS_HANDLE handle, void* user);

/**	\brief	mxRas_getLocalIp
	\brief	Local IP 를 가져온다.
	\param	handle : MXRAS_HANDLE
	\return	local ip를 스트링으로 전달한다. free해줘야한다. */
__MXAPI 
char *mxRas_getLocalIp( MXRAS_HANDLE handle );

/**	\brief	mxRas_getExternalIp
	\brief	외부에서 접근 가능한 IP를 가져온다.
	\param	handle : MXRAS_HANDLE
	\return	external ip를 스트링으로 전달한다. free해줘야한다. */
__MXAPI 
char* mxRas_getExternalIp( MXRAS_HANDLE handle );

/**	\brief	mxRas_getPublicIp
	\brief	Public IP를 가져온다. (Public IP와 External IP의 차이점은 이중 NAT 일때, 최종적으로 나가는 게이트웨이 속한 IP가 External IP가 되며, 현재 LAN에서 나가는 IP가 public IP가 된다.)
	\param	handle : MXRAS_HANDLE
	\return	public ip를 스트링으로 전달한다. free해줘야한다. */
__MXAPI 
char *mxRas_getPublicIp( MXRAS_HANDLE handle );

/**	\brief	mxRas_getPublicPort
	\brief	Public Port를 가져온다.
	\param	handle : MXRAS_HANDLE
	\return	public port를 정수로 전달한다. */
__MXAPI 
int mxRas_getPublicPort( MXRAS_HANDLE handle );

/**	\brief	mxRas_getExternalPort
	\brief	외부포트를 가져온다.
	\param	handle : MXRAS_HANDLE
	\return	external port를 정수로 전달한다. */
__MXAPI 
int mxRas_getExternalPort( MXRAS_HANDLE handle );

/**	\brief	mxRas_getPublicStatus
	\brief	접근 가능 상태를 가져온다.
	\param	handle : MXRAS_HANDLE
	\return 외부 접근 가능 여부를 전달한다. MXRA_NETWORK_STATUS를 사용한다. */
__MXAPI 
int mxRas_getPublicStatus( MXRAS_HANDLE handle );



//////////////////////////////////////////////////////////////////
// WOON ID
//////////////////////////////////////////////////////////////////

/**	\brief	mxRas_wid_generate
	\brief	Connection 서버로부터 임의의 WOON ID를 받아온다 (현재 사용되지 않고 있다.)
	\param	handle : MXRAS_HANDLE
	\return 성공 1, 실패 404 등의 HTTP 에러값 */
__MXAPI 
int mxRas_wid_generate(MXRAS_HANDLE handle);

/**	\brief	mxRas_wid_query
	\brief	Connection 서버에서 WOON ID가 존재하는 확인한다 WOON ID상태를 반환한다.
	\param	handle : MXRAS_HANDLE
	\param	wid : WOON ID
	\return 성공 1, 실패 404 등의 HTTP 에러값 */
__MXAPI 
int mxRas_wid_query(MXRAS_HANDLE handle, char* wid);

/**	\brief	mxRas_wid_renew
	\brief	Connection 서버에 WOON ID 정보를 갱신한다.
	\param	handle : MXRAS_HANDLE
	\param	wid_array_data : 갱신할 WOON ID 배열을 전달한다.
	\param	count : WOON ID 배열의 개수 
	\return 성공 1, 실패 404 등의 HTTP 에러값 */
__MXAPI 
int mxRas_wid_renew(MXRAS_HANDLE handle, void *wid_array_data, int count );

/**	\brief	mxRas_wid_renew_all
	\brief	Connection 서버에 '모든' WOON ID 정보를 갱신한다.
	\param	handle : MXRAS_HANDLE
	\return 성공 MXDLNA_OK, 실패 MXDLNA_ERROR*/
__MXAPI 
int mxRas_wid_renew_all(MXRAS_HANDLE handle);

/**	\brief	mxRas_wid_register
	\brief	Connection 서버에 WOON ID를 등록한다 WOON ID의 자세한 정보는 renew를 통해 자동 갱신된다.
	\param	handle : MXRAS_HANDLE
	\param	wid : WOON ID
	\param	path : 등록할 ObjectID를 주면된다. 
	\param	password : 비밀번호가 있으면 넣어준다. 없으면 NULL
	\param	description : WOON ID에 대한 설명을 넣어준다. 
	\param	permission : 읽기 쓰기 권한을 유닉스 시스템 퍼미션 숫자로 표현해 전달한다.
	\return 성공 1, 실패 404 등의 HTTP 에러값 */
__MXAPI 
int mxRas_wid_register(MXRAS_HANDLE handle, char *wid, char *path, char *password, char *description, int permission);

/**	\brief	mxRas_wid_unRegister
	\brief	WOONID를 삭제한다. 함수 실행 성공/실패 (처리 결과는 MXRA_USER_EVENT_CALLBACK 콜백으로 올라온다)
	\param	handle : MXRAS_HANDLE
	\param	wid : WOON ID
	\return 성공 1, 실패 404 등의 HTTP 에러값 */
__MXAPI 
int mxRas_wid_unRegister(MXRAS_HANDLE handle, char* wid);

/**	\brief	mxRas_wid_getDatas
	\brief	WOON ID 배열 정보를 반환한다.
	\param	handle : MXRAS_HANDLE
	\param	idx : index를 넣는다.
	\param	cnt : 개수를 넣는다. 
	\param	result : 전달되는 배열의 개수를 준다.
	\param	err : 에러 값을 전달한다. 
	\return WOON ID의 배열을 전달한다. (mxRas_wid_freeDatas 통해 free한다.)*/
__MXAPI 
MXRA_WID_ARRAY mxRas_wid_getDatas(MXRAS_HANDLE handle, int idx, int cnt, int *result, int *err);

/**	\brief	mxRas_wid_getDatasByUDN
	\brief	WOON ID 배열 정보를 반환한다, UDN에 해당하는 Device를 반환한다.
	\param	handle : MXRAS_HANDLE
	\param	idx : index를 넣는다.
	\param	cnt : 개수를 넣는다. 
	\param	result : 전달되는 배열의 개수를 준다.
	\param	err : 에러 값을 전달한다. 
	\param	UDN : UDN에 해당하는 WOON ID들을 전달한다. 
	\return WOON ID의 배열을 전달한다. (mxRas_wid_freeDatas 통해 free한다.)*/
__MXAPI 
MXRA_WID_ARRAY mxRas_wid_getDatasByUDN(MXRAS_HANDLE handle, int idx, int cnt, int *result, int *err, char *UDN);

/**	\brief	mxRas_wid_getDataCnt
	\brief	WOON ID 개수를 반환한다.
	\param	handle : MXRAS_HANDLE
	\return WOON ID 개수 */
__MXAPI
int mxRas_wid_getDataCnt(MXRAS_HANDLE handle);

/**	\brief	mxRas_wid_freeDatas
	\brief	사용한 WOON ID 배열을 정리한다
	\param	wids : WOON ID 배열을 준다. 
	\param	cnt : WOON ID 배열을 개수를 준다.
	\return MXDLNA_OK 를 전달한다. */
__MXAPI 
int mxRas_wid_freeDatas(MXRA_WID_ARRAY wids, int cnt);

/**	\brief	mxRas_wid_getDataFromWid
	\brief	입력된 WOON ID를 가지고 WOON ID 정보를 가져온다.
	\param	handle : MXRAS_HANDLE
	\param	wid : WOON ID 
	\param	err : 에러값을 전달한다.
	\return WOONID 1개의 정보를 전달한다. (mxRas_wid_freeData를 통해서 free한다.)*/
__MXAPI 
MXRA_WID_ITEM mxRas_wid_getDataFromWid(MXRAS_HANDLE handle, char* wid, int *err);

/**	\brief	mxRas_wid_freeData
	\brief	WOON ID 정보를 정리한다.
	\param	wid_data : WOON ID 정보
	\return MXDLNA_OK 를 전달한다. */
__MXAPI 
int mxRas_wid_freeData(MXRA_WID_ITEM wid_data);

/**	\brief	mxRas_wid_setName
	\brief	저장된 WOON ID의 이름을 변경 한다.
	\param	handle : MXRAS_HANDLE
	\param	wid : WOON ID
	\param	name : WOON ID 이름,상세 정보
	\return 성공 1, 실패 404 등의 HTTP 에러값 */
__MXAPI
int mxRas_wid_setName(MXRAS_HANDLE handle, char *wid, char *name);

/**	\brief	mxRas_wid_setPassword
	\brief	저장된 WOON ID의 비밀 번호를 변경한다.
	\param	handle : MXRAS_HANDLE
	\param	wid : WOON ID
	\param	password : WOON ID 비밀번호
	\return 성공 1, 실패 404 등의 HTTP 에러값 */
__MXAPI 
int mxRas_wid_setPassword(MXRAS_HANDLE handle, char *wid, char *password);


/**	\brief	mxRas_wid_setPermission
	\brief	저장된 WOON ID의 접근허용 권한을 변경한다.
	\param	handle : MXRAS_HANDLE
	\param	wid : WOON ID
	\param	permission : 읽기 쓰기 권한을 유닉스 시스템 퍼미션 숫자로 표현해 전달한다.
	\return 성공 1, 실패 404 등의 HTTP 에러값 */
__MXAPI 
int mxRas_wid_setPermission(MXRAS_HANDLE handle, char *wid, int permission);

/**	\brief	mxRas_setVersion
	\brief	버전을 설정한다. 버전 별 처리가 달라지는 경우를 위해서 만들어졌다.
	\param	handle : MXRAS_HANDLE
	\param	version : 버전 스트링 
	\return 성공 1, 실패 0 */
__MXAPI 
int mxRas_setVersion(MXRAS_HANDLE handle, char *version);

/**	\brief	mxRas_getVersion
	\brief	버전을 가져온다.
	\param	handle : MXRAS_HANDLE
	\return 버전 스트링을 전달한다. (free 하지 않는다.) */ 
__MXAPI 
char *mxRas_getVersion(MXRAS_HANDLE handle);


/**	\brief	mxRas_getWidStatusString
	\brief	WID_STATUS 값을 string으로 변경해준다.
	\param	status : 현재 네트워크 상태값 MXRA_WID_STATUS
	\return 상태값을 스트링으로 전달한다. */ 
__MXAPI 
char *mxRas_getWidStatusString(MXRA_WID_STATUS status);



//////////////////////////////////////////////////////////////////
// Transcoder
//////////////////////////////////////////////////////////////////
// Transcoder 처리 하기 위한 함수를 등록한다.
int mxRas_transcoder_fn(
	MXRAS_HANDLE handle,
	MXRAS_TRANSCODER_START_FUNCTION	start_fn,
	MXRAS_TRANSCODER_M3U8_NAME_FUNCTION	m3u_name_fn,
	MXRAS_TRANSCODER_M3U8_DATA_FUNCTION m3u_data_fn,
	MXRAS_TRANSCODER_GET_SEGMENT_FUNCTION get_segment_fn,
	MXRAS_TRANSCODER_ERROR_FUNCTION error_fn
);

typedef enum _MXRAS_REQUEST_FLAG_SETTING_TYPE 
{
	MXRAS_REQUEST_FLAG_SETTING_M3U8_TYPE	= 1,			// M3U8이 필요하다 / 필요하지 않다. (향후 SAT>IP에서 HTTP를 지원하는 경우를 대비함.)
	MXRAS_REQUEST_FLAG_SETTING_ONLY_AUDIO_TYPE	= 2,		// Audio만 필요하다. 
	MXRAS_REQUEST_FLAG_SETTING_LIVE_TYPE	= 4,			// 라이브 형태 요청 이다 / 라이브 형태 요청이 아니다. 
} MXRAS_REQUEST_FLAG_SETTING_TYPE;


#ifdef __cplusplus
}
#endif

#endif /* ____RASERVER__H____  */
