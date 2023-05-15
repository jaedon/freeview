


#ifndef		_DLNA_DMR_55212_
#define		_DLNA_DMR_55212_

#include "mxapi_define.h"
#include "mxdlna_struct.h"

#ifdef __cplusplus
extern "C" {
#endif



#define MXDLNA_STATE_PLAYING					1
#define MXDLNA_STATE_STOPPED					2
#define MXDLNA_STATE_PAUSED						3
#define MXDLNA_STATE_TRANSITIONING				4
#define MXDLNA_STATE_CURRENTDURATION			20     // value is long
#define MXDLNA_STATE_CURRENTTRACKDURATION		21     // value is long 
#define MXDLNA_STATE_CURRENTTRANSPORTACTION		22	   // value is string
#define MXDLNA_STATE_VOLUME						300    // value is unsigned short 0~100
#define MXDLNA_STATE_MUTE						301    // value is int 0 or 1

#ifdef DMR_X_EVENT_SUPPORT
#define MXDLNA_STATE_X_EVENT					600
#endif

#define MXDLNA_STATE_CURRENTDURATION_NOTINVOKE  3003

#define MXDLNA_DMR_RET_OK									0			
#define MXDLNA_DMR_RET_RETRY								(MXDLNA_DMR_RET_OK + 1)
#define MXDLNA_DMR_RET_OK_NOTRANSITIONING					(MXDLNA_DMR_RET_OK + 2)

#define MXDLNA_DMR_RET_ERR									700
#define MXDLNA_DMR_RET_ERR_INAVLID_ARGS              		(MXDLNA_DMR_RET_ERR + 1)
// SETAVT RETURN
#define MXDLNA_DMR_RET_ERR_ILLEGAL_MIME						(MXDLNA_DMR_RET_ERR + 2)
#define MXDLNA_DMR_RET_ERR_CONTENT_BUSY						(MXDLNA_DMR_RET_ERR + 3)
#define MXDLNA_DMR_RET_ERR_RESOURCE_NOT_FOUND				(MXDLNA_DMR_RET_ERR + 4)
#define MXDLNA_DMR_RET_ERR_INVALID_INSTANCEID				(MXDLNA_DMR_RET_ERR + 5)
#define MXDLNA_DMR_RET_ERR_DRM_ERROR						(MXDLNA_DMR_RET_ERR + 6)
#define MXDLNA_DMR_RET_ERR_DRM_ERROR_EXPIRED				(MXDLNA_DMR_RET_ERR + 7)
#define MXDLNA_DMR_RET_ERR_DRM_ERROR_NONALLOWED				(MXDLNA_DMR_RET_ERR + 8)
#define MXDLNA_DMR_RET_ERR_DRM_ERROR_CANNOTDETERMINE		(MXDLNA_DMR_RET_ERR + 9)
#define MXDLNA_DMR_RET_ERR_DRM_ERROR_EXHAUSTED				(MXDLNA_DMR_RET_ERR + 10)
#define MXDLNA_DMR_RET_ERR_DRM_ERROR_AUTHENTICATION_FAIL	(MXDLNA_DMR_RET_ERR + 11)
#define MXDLNA_DMR_RET_ERR_DRM_ERROR_REVOCATION				(MXDLNA_DMR_RET_ERR + 12)
// PLAY&STOP RETURN
#define MXDLNA_DMR_RET_ERR_TRANSITION_NOT_AVAILABLE			(MXDLNA_DMR_RET_ERR + 13)
#define MXDLNA_DMR_RET_ERR_TRANSPORT_IS_LOCKED				(MXDLNA_DMR_RET_ERR + 14)
// PLAY RETURN
#define MXDLNA_DMR_RET_ERR_NO_CONTENTS						(MXDLNA_DMR_RET_ERR + 15)
#define MXDLNA_DMR_RET_ERR_READ_ERROR						(MXDLNA_DMR_RET_ERR + 16)
#define MXDLNA_DMR_RET_ERR_NOTSUPPORTED_FORMAT				(MXDLNA_DMR_RET_ERR + 17)
#define MXDLNA_DMR_RET_ERR_NOTSUPPORTED_PLAY_SPEED			(MXDLNA_DMR_RET_ERR + 18)


__MXAPI 
int mxdlna_CreateDMR(int portNum, char* udn, char* friendlyname, char* serialNumber, const char* protocolinfo[]);
__MXAPI 
int mxdlna_DestroyDMR(void);

__MXAPI 
int mxdlna_dmr_setNames( const char *Manufacturer,	const char *ManufacturerURL,	const char *ModelName,	const char *ModelDescription,	const char *ModelNumber,	const char *ModelURL );


typedef int (*MXDLNA_Dmr_con_PrepareForConnection) (void *dmr,char* RemoteProtocolInfo, char* PeerConnectionManager,int PeerConnectionID,char* Direction);

//20120813 khpark Device Description send 성공 후 수행할 함수 선언
typedef int (*MXDLNA_Dmr_giveDeviceDescription) (int remoteIP,  char *userAgent, char *friendlyName);
typedef int (*MXDLNA_Dmr_avt_SetAVTransportURI) (void *dmr,int instanceid,char* uri, void* data);
typedef int (*MXDLNA_Dmr_avt_Play) (void *dmr,int instanceid,char* playspeed);
typedef int (*MXDLNA_Dmr_avt_Stop) (void *dmr,int instanceid);
typedef int (*MXDLNA_Dmr_avt_Pause) (void *dmr,int instanceid);
typedef int (*MXDLNA_Dmr_avt_GetRelativeTimePosition) (void *dmr,int instanceid, long* time);    
typedef int (*MXDLNA_Dmr_avt_GetAbsoluteTimePosition) (void *dmr,int instanceid, long* time);
typedef int (*MXDLNA_Dmr_avt_SeekPosition) (void *dmr,int instanceid,long position);
typedef int (*MXDLNA_Dmr_rcs_GetVolume) (void *dmr,int instanceid,char* channel, unsigned short* volume);
typedef int (*MXDLNA_Dmr_rcs_SetVolume) (void *dmr,int instanceid,char* channel, unsigned short volume);
typedef int (*MXDLNA_Dmr_rcs_SetMute) (void *dmr,int instanceid,char* channel, int mute);

//20120813 khpark	Device Description send 성공 여부 확인 callback 등록
__MXAPI
int mxdlna_dmr_giveDeviceDescription_setCallback(MXDLNA_Dmr_giveDeviceDescription _giveDeviceDescription);

__MXAPI 
int mxdlna_dmr_avt_setCallBack( MXDLNA_Dmr_avt_SetAVTransportURI _seturi 
								,MXDLNA_Dmr_avt_Play _play
								,MXDLNA_Dmr_avt_Stop _stop
							    ,MXDLNA_Dmr_avt_Pause _pause
								,MXDLNA_Dmr_avt_GetRelativeTimePosition _getRelativeTime
								,MXDLNA_Dmr_avt_GetAbsoluteTimePosition _getAbsoluteTime
								);
__MXAPI 
int mxdlna_dmr_avt_setCallBack2( MXDLNA_Dmr_avt_SeekPosition _seekAbsPosition,
								MXDLNA_Dmr_avt_SeekPosition _seekRelPosition );

__MXAPI 
int mxdlna_dmr_rcs_setCallBack( MXDLNA_Dmr_rcs_GetVolume _getvol
								,MXDLNA_Dmr_rcs_SetVolume _setvol
								,MXDLNA_Dmr_rcs_SetMute _setMute
								);

__MXAPI 
int mxdlna_dmr_con_setCallBack( MXDLNA_Dmr_con_PrepareForConnection _prepare);
__MXAPI 
char* mxdlna_dmr_getProtocolInfo(int);
__MXAPI 
int mxdlna_dmr_setProtocolInfo( int instancdID, char* protocolinfo );


// 이벤트... 등록...
__MXAPI 
int mxdlna_dmr_setstate( int instanceID, int state_type, void* value); 


__MXAPI
int mxDlnaDmr_SetScope( int scope_level );  //scope_level  *( 1 = 자기 자신의 IP를 가진 이만 허용, 3 = 외부만 허용,  그외값 모든 IP 허용 )


__MXAPI
char *mxDlnaDmr_getThumbnail(void *object);

__MXAPI
int mxDlnaDmr_renameFriendlyName(char* utf8_rename);


#ifdef SUPPORT_SUBTITLE_KT
// For KT subtitle
typedef int (*MXDLNA_Dmr_avt_SetAVTransportSubtitleURI) (void *dmr,int instanceid,char* uri, void* data);
typedef int (*MXDLNA_Dmr_rcs_SetSubtitle) (void *dmr,int instanceid,int state);
typedef int (*MXDLNA_Dmr_rcs_SetSubtitleSync) (void *dmr,int instanceid,int sync);

__MXAPI int mxdlna_dmr_kt_setCallBack( MXDLNA_Dmr_avt_SetAVTransportSubtitleURI _setsubtitleuri ,
							  MXDLNA_Dmr_rcs_SetSubtitle _setsubtitle,
							  MXDLNA_Dmr_rcs_SetSubtitleSync _setsubtitlesync );
#endif


#ifdef DMR_X_DO_COMMAND_SUPPORT
typedef char *(*MXDLNA_Dmr_avt_X_DoCommand) (void *dmr,int instanceid,char* command);
__MXAPI int mxdlna_dmr_X_DoCommand_setCallBack( MXDLNA_Dmr_avt_X_DoCommand cb);
#endif

#ifdef DMR_X_GET_FLING_INFO_SUPPORT
__MXAPI int mxdlna_dmr_setFlingInfo(int instanceID, char* uri, char *metadata);
__MXAPI char *mxdlna_dmr_getFlingInfoURI(int instanceID);
__MXAPI char *mxdlna_dmr_getFlingInfoMetadata(int instanceID);
#endif

//20120910 LG s-dong WFD khpark
#ifdef DMR_X_INIT_WFD_SUPPORT
typedef int (*MXDLNA_Dmr_X_LG_WFDisplay) (char *RTSP_IP_PORT);
__MXAPI int mxdlna_dmr_X_LG_WFDisplay_setCallback(MXDLNA_Dmr_X_LG_WFDisplay _X_initWFD_CB);
#endif

#ifdef _ANDROID
// dypark : 2012-10-29
__MXAPI char *mxDlnaDmr_getMimeTypeFromUrl(char *url);
#endif

// dypark 2012-12-13 ~18
__MXAPI char *mxDlnaDmr_getArtist(void *cdsObject);
__MXAPI unsigned int mxDlnaDmr_getMediaClass( MXDLNA_CDSOBJECT* obj );
__MXAPI MXDLNA_OBJECT_CLASS_TYPE mxDlnaDmr_getRootClass( MXDLNA_CDSOBJECT* obj );
__MXAPI MXDLNA_OBJECT_MAJOR_TYPE mxDlnaDmr_getMajorClass( MXDLNA_CDSOBJECT* obj );
__MXAPI MXDLNA_OBJECT_MINOR_TYPE mxDlnaDmr_getMinorClass( MXDLNA_CDSOBJECT* obj );
__MXAPI const char* mxDlnaDmr_getStrProperty( MXDLNA_CDSOBJECT* obj, int property , int index );
__MXAPI int mxDlnaDmr_getIntProperty( MXDLNA_CDSOBJECT* obj, int property, int index );
__MXAPI unsigned int mxDlnaDmr_isRestricted(MXDLNA_CDSOBJECT* obj);
__MXAPI int mxDlnaDmr_isContainer(MXDLNA_CDSOBJECT* obj);
__MXAPI int mxDlnaDmr_getMediaClassification(MXDLNA_CDSOBJECT* obj);
__MXAPI int mxDlnaDmr_IsFolderType(MXDLNA_CDSOBJECT* obj);
__MXAPI char * mxDlnaDmr_dateKorToEuro( char * inp );



#ifdef __cplusplus
}
#endif

#endif
