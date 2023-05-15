
#ifndef		_DLNA_RA_1332111_
#define		_DLNA_RA_1332111_


#include "mxapi_define.h"


#ifdef __cplusplus
extern "C"{
#endif


#include "mxdlna_struct.h"



#define RE_SESSION_TIME   (40)
#define ARRIVE_TO_RARS_COUNT   (5)
#define ARRIVE_TO_MAX_TTL   (64)
#define ARRIVE_TO_MIN_TTL   (1)

#define KT_PASSWORD "kthome"
//#define SERVERIP  "112.166.136.155"
//#define SERVERIP  "222.114.35.185"    // ALG Proxy Server IP
//#define SERVERIP  "112.166.136.164"
#define SERVERIP  "220.80.25.105"
#define SSDPWANPORT 39500


enum Ra_Cmd_List {
	Ra_Cmd_Play = 0,
	Ra_Cmd_Stop,
	Ra_Cmd_Pause,
}; 
enum Ra_Cmd2_List {
	Ra_Cmd2_Volume = 0,
	Ra_Cmd2_Mute,
}; 


typedef void* MXDLNA_RA_HANDLE;

__MXAPI MXDLNA_RA_HANDLE mxDlnaRa_createRaClient(char* kt_user_id,char* kt_user_passwd ,char* rada, MXDLNA_DEVICE_ADDREMOVE func, void* user);
//__MXAPI int mxDlnaRa_destroy( MXDLNA_RA_HANDLE ra);
__MXAPI int mxDlnaRa_destroy( void* rac, void* cp, char* id, char* pwd  );


//__MXAPI int mxDlnaRa_setOptions( char* serverip, int serverPort, char* local_public_ip );
__MXAPI int mxDlnaRa_setOptions( char* serverip, int serverPort, char* local_public_ip , int useTCP );

//int mxDlnaRa_getRemoteDeviceCnt( MXDLNA_RA_HANDLE ra );
__MXAPI void mxDlnaRa_destroyDeviceList( MXDLNA_DEVICE_TINY *list);
//__MXAPI MXDLNA_DEVICE_TINY *mxDlnaRa_getRemoteDeviceList( MXDLNA_RA_HANDLE ra ,char* id);
__MXAPI MXDLNA_DEVICE* mxDlnaRa_getDeviceByUDN( MXDLNA_RA_HANDLE ra, char* udn );


//MXDLNA_DEVICE* mxDlnaRa_getDeviceAtUDN( MXDLNA_CP_HANDLE cp, char * UDN);
//char* mxDlnaMscp_getDeviceIPByUDN( MXDLNA_CP_HANDLE cp, char* udn);
//char* mxDlnaMscp_getCurrentServerUDN( MXDLNA_CP_HANDLE cp);


#ifdef __cplusplus
}
#endif

#endif
