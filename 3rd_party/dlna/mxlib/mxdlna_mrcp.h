#ifndef __MXDLNA_DMRCP_PUDA__
#define __MXDLNA_DMRCP_PUDA__

#ifdef __cplusplus
extern "C"{
#endif

#include "mxapi_define.h"
#include "mxdlna_struct.h"
#include "mxtypes.h"

#define MXDLNA_MRCP_OLD_VERSION_SUPPORT


//////////////////////////////////////////////////////////////////////////
// @ 정의
//////////////////////////////////////////////////////////////////////////
#define MRCP_ACTION_CHECK_TRUE									1
#define MRCP_ACTION_CHECK_FALSE									-5
#define MRCP_ACTION_CHECK_ERROR_TRANSITION_NOT_AVAILABLE		-701

#define MRCP_LENGTH_TRACKPOSITION 32

#define SERVICE_AVT		"AVTransport"
#define SERVICE_CMS		"ConnectionManager"
#define SERVICE_RCS		"RenderingControl"

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

enum PlayModeEnum
{
	AVRCP_INVALID		=0x0000000,
	AVRCP_NORMAL		=0x0000001,
	AVRCP_REPEAT_ALL	=0x0000002,
	AVRCP_REPEAT_ONE	=0x0000004,
	AVRCP_RANDOM		=0x0000008,
	AVRCP_SHUFFLE       =0x0000010,
	AVRCP_INTRO			=0x0000020
};

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


//////////////////////////////////////////////////////////////////////////
// @ 구조체
//////////////////////////////////////////////////////////////////////////

typedef MXDLNA_INVOKE* MRCP_ACTION_HANDLE; 
typedef struct AVRendererConnection* MRCP_CONNECTION_HANDLE;

typedef struct _MXDLNA_USER_CONNECTION_DATA {

	int type;

	char *ProtocolInfo;
	char *ConnectionIDs;

} MXDLNA_USER_CONNECTION_DATA;

// _MXDLNA_USER_RENDERER_INFO 구조체는 LIB(AVRendererConnection) 랑 같은 형태(순서)이여야 합니다 함부러 수정하면 안됩니다.
typedef struct _MXDLNA_USER_RENDERER_INFO
{
	int type;

	char *ProtocolInfo;
	char *ConnectionIDs;

	char *MediaServer;
	char *MediaTitle;
	char *MediaUri;
	char *TrackUri;
	char *TransportStatus;
	unsigned int TotalTracks;
	unsigned int TrackNumber;
	enum PlayStateEnum PlayState;
	enum PlayModeEnum PlayMode;

	unsigned int Duration;
	
	char TrackPosition[MRCP_LENGTH_TRACKPOSITION];
	struct CdsObject* TrackObject;
	
	char **SupportedRecordQualityModes;
	int SupportedRecordQualityModesLength;
	char **RecordMedia;
	int RecordMediaLength;
	char **PlayMedia;
	int PlayMediaLength;
	
	int ChannelCount;
	char **Channel;

	// int형으로 변경되었습니다.
	int *Volume;
	int *Mute;
	
	unsigned short Brightness;
	unsigned short Contrast;
	short* VolumeDB;
	int* Loudness;
	
	int TrackDurationTime;
	int RelativeSeconds;
	int AbsoluteSeconds;
	int RelativeCounter;
	int AbsoluteCounter;

} MXDLNA_USER_RENDERER_INFO;

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


typedef struct _MXDLNA_USER_CMS_EVENT_DATA
{
	char *dump;
} MXDLNA_USER_CMS_EVENT_DATA;


typedef struct _MXDLNA_USER_AVT_EVENT_DATA
{
	char *MediaUri;						// AVT_EVENT_AVTRANSPORT_URL
	char *TrackUri;						// AVT_EVENT_CURRENT_TRACK_URI	
	char *TransportStatus;				// AVT_EVENT_TRANSPORT_STATUS		
	enum PlayStateEnum PlayState;	 	// AVT_EVENT_TRANSPORT_STATE
	unsigned int TrackNumber;			// AVT_EVENT_CURRENT_TRACK
	unsigned int TotalTracks;			// AVT_EVENT_NUMBER_OF_TRACKS
	enum PlayModeEnum PlayMode;			// AVT_EVENT_CURRENT_PLAY_MODE	
	unsigned int Duration;				// AVT_EVENT_CURRENT_TRACK_DURATION			
	char TrackPosition[MRCP_LENGTH_TRACKPOSITION];				// AVT_EVENT_RELATIVE_TIME_POSITION
	struct CdsObject* TrackObject;		// AVT_EVENT_CURRENT_METADATA_TRACK_OBJECT		

#ifdef DMC_X_EVENT_SUPPORT
	char *x_event_value;
#endif


	// dypark 2013-03-25
	char *MediaTitle;					// AVT_EVENT_CURRENT_METADATA_TRACK_OBJECT

} MXDLNA_USER_AVT_EVENT_DATA;


typedef struct _MXDLNA_USER_CHANNEL_LIST {
    int cnt;							// 채널 수 
    char **channel;						// 채널
} MXDLNA_USER_CHANNEL_LIST;


typedef struct _MXDLNA_USER_CONNECTION_LIST {
	MXDLNA_CMID CMID;
	struct _MXDLNA_USER_CONNECTION_LIST *pNext;
} MXDLNA_USER_CONNECTION_LIST;


typedef struct _MXDLNA_USER_IDS {
	int ConnectionID;
	int AVTransPortID;
	int RcsID;
		
	struct _MXDLNA_USER_IDS *pNext;
} MXDLNA_USER_IDS;


typedef struct _MXDLNA_USER_POSITION{
	int TrackDurationTime;
	int RelativeSeconds;
	int AbsoluteSeconds;
	int RelativeCounter;
	int AbsoluteCounter;
} MXDLNA_USER_POSITION;


typedef struct _MXDLNA_USER_POSITION_BYTE {
	unsigned long TrackSize;
	unsigned long RelByte;
	unsigned long AbsByte;
} MXDLNA_USER_POSITION_BYTE;


typedef struct _MXDLNA_USER_VOLUME
{
	short MinValue; 
	short MaxValue;
} MXDLNA_USER_VOLUME;


typedef struct _MXDLNA_USER_ACTIONS {
	int IsPlay;
	int IsStop;
	int IsSeek;
	int IsX_DLNA_SeekTime;
	int IsX_DLNA_SeekByte;
} MXDLNA_USER_ACTIONS;


typedef struct _MXDLNA_USER_LOAD_VOLUME {
	
	int ChannelCount;
	char **Channel;
	char *Volume;
	int *Mute;

} MXDLNA_USER_LOAD_VOLUME;


typedef struct _MXDLNA_USER_SETURI_DATA {
	char *contentURL;
	char *metaData;
} MXDLNA_USER_SETURI_DATA;







//////////////////////////////////////////////////////////////////////////
// @ 함수 
//////////////////////////////////////////////////////////////////////////

// @ MRCP create/destroy
__MXAPI MXDLNA_CP_HANDLE mxDlnaMrcp_createMrCp();
__MXAPI void mxDlnaMrcp_destroyMrCp(MXDLNA_CP_HANDLE mrcp);



// @ MRCP Config
__MXAPI void mxDlnaMrcp_configure(MXDLNA_CP_HANDLE mrcp, MXDLNA_CP_CONFIG *conf);
__MXAPI void mxDlnaMrcp_unconfigure(MXDLNA_CP_HANDLE mrcp);
__MXAPI void mxDlnaMrcp_saveParent(MXDLNA_CP_HANDLE mrcp, MXDLNA_HANDLE parent);



// @ MRCP Start & End
__MXAPI int mxDlnaMrcp_start(MXDLNA_CP_HANDLE mrcp);
__MXAPI int mxDlnaMrcp_end(MXDLNA_CP_HANDLE mrcp);



// @ MRCP Action Context
__MXAPI MRCP_ACTION_HANDLE mxDlnaMrcp_createContext(MXDLNA_CP_HANDLE mscp, MXDLNA_UDN UDN, MXDLNA_CMID CMID);
__MXAPI void mxDlnaMrcp_destroyContext(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_setUDN(MRCP_ACTION_HANDLE hAction, MXDLNA_UDN UDN);
__MXAPI int mxDlnaMrcp_setCMID(MRCP_ACTION_HANDLE hAction, MXDLNA_CMID CMID);
__MXAPI int mxDlnaMrcp_setUserTag(MRCP_ACTION_HANDLE hAction, MXDLNA_USER_TAG user);
__MXAPI MXDLNA_UDN mxDlnaMrcp_getUDN(MRCP_ACTION_HANDLE hAction);
__MXAPI MXDLNA_CMID mxDlnaMrcp_getCMID(MRCP_ACTION_HANDLE hAction);
__MXAPI MXDLNA_USER_TAG mxDlnaMrcp_getUserTag(MRCP_ACTION_HANDLE hAction);



// @ Device Function
__MXAPI void mxDlnaMrcp_sendMSearch(MXDLNA_CP_HANDLE mrcp);
__MXAPI MXDLNA_DEVICE_TINY *mxDlnaMrcp_getMediaRendererList(MXDLNA_CP_HANDLE mrcp);
__MXAPI MXDLNA_DEVICE_TINY *mxDlnaMrcp_getMediaRendererListEx(MXDLNA_CP_HANDLE mrcp, MXDLNA_DEVICE *device, int addRemove);
__MXAPI void mxDlnaMrcp_destroyMediaRendererList(MXDLNA_DEVICE_TINY *list);
__MXAPI int mxDlnaMrcp_initRenderer(MRCP_ACTION_HANDLE hAction, MXDLNA_UDN UDN);

// 2012-12-05 복사해서 전달하고 메모리를 정리하는 형태로 변경함.
__MXAPI MXDLNA_DEVICE *mxDlnaMrcp_getDeviceAtUDN( MXDLNA_CP_HANDLE mrcp, MXDLNA_UDN UDN);
__MXAPI void mxDlnaMrcp_freeDeviceAtUDN(MXDLNA_DEVICE *device);		// 추가 함. 

__MXAPI int mxDlnaMrcp_isMediaRenderer(MXDLNA_DEVICE* device);
__MXAPI int mxDlnaMrcp_isAliveDevice(MXDLNA_CP_HANDLE mrcp, MXDLNA_UDN UDN);

__MXAPI MXDLNA_SERVICE *mxDlnaMrcp_getService(MXDLNA_CP_HANDLE mrcp, MXDLNA_UDN UDN, const char* selectService);
__MXAPI void mxDlnaMrcp_freeService(MXDLNA_SERVICE *service);								// 2012-12-05

__MXAPI MXDLNA_ACTION  *mxDlnaMrcp_getAction(MXDLNA_CP_HANDLE mrcp, MXDLNA_UDN UDN, const char *selectService, const char *actionName);
__MXAPI void mxDlnaMrcp_freeAction(MXDLNA_ACTION *action);									// 2012-12-05

__MXAPI MXDLNA_STATE_VARIABLE *mxDlnaMrcp_getStateVariable(MXDLNA_CP_HANDLE mrcp, MXDLNA_UDN UDN, const char *selectService, const char *stateName);
__MXAPI void mxDlnaMrcp_freeStateVariable(MXDLNA_STATE_VARIABLE *stateVariable);			// 2012-12-05

__MXAPI int mxDlnaMrcp_isMrcpAction(MXDLNA_CP_HANDLE mrcp, MXDLNA_UDN UDN, const char *serviceName, const char *actionName);
__MXAPI MXDLNA_DEVICE_PROTOCOL_INFO  *mxDlnaMrcp_loadProtocolInfo(MXDLNA_CP_HANDLE mrcp, MXDLNA_UDN UDN);
__MXAPI void mxDlnaMrcp_unloadProtocolInfo(MXDLNA_DEVICE_PROTOCOL_INFO  *ptcInfo);
__MXAPI int mxDlnaMrcp_compatibleContent2ProtocolInfo(MXDLNA_CP_HANDLE mrcp, MXDLNA_UDN UDN, char *contentPtcInfo);
__MXAPI int mxDlnaMrcp_compatibleDevice2ProtocolInfo(MXDLNA_CP_HANDLE mrcp, MXDLNA_UDN UDN, MXDLNA_DEVICE_PROTOCOL_INFO *ptcInfo);
__MXAPI int mxDlnaMrcp_compatibleURL2lcoalIP(MXDLNA_CP_HANDLE mrcp, MXDLNA_UDN UDN, char *contentURL);
__MXAPI int mxDlnaMrcp_get_X_DlnaCap(MXDLNA_CP_HANDLE mrcp, MXDLNA_UDN UDN);



// 2012-08-30 
__MXAPI char *mxdlnaMrcp_getHostIPAddress( MXDLNA_CP_HANDLE mrcp, char* udn);				//__MXAPI char *mxDlnaMrcp_getDeviceIPByUDN( MXDLNA_CP_HANDLE mrcp, MXDLNA_UDN UDN);에서 변경
__MXAPI char *mxdlnaMrcp_getDeviceIPAddress( MXDLNA_CP_HANDLE mrcp, char* udn);				// 추가



// @ Make Cds , PlayContainer 관련 함수, Load Data 관련 함수
__MXAPI MXDLNA_CDSOBJECT *mxDlnaMrcp_makeCdsDeserialize(char *MetaData);
__MXAPI int mxDlnaMrcp_releaseCdsDeserialize(MXDLNA_CDSOBJECT *CdsObj);
__MXAPI char *mxDlnaMrcp_getDevicePlayContainerCapabilityID(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getDevicePlayContainerFID_FLAG(MRCP_ACTION_HANDLE hAction);
__MXAPI void *mxDlnaMrcp_loadVolume(MRCP_ACTION_HANDLE hAction);
__MXAPI void mxDlnaMrcp_unloadVolume(MXDLNA_USER_RCS_EVENT_DATA *temp);
__MXAPI int mxDlnaMrcp_loadPlayState(MRCP_ACTION_HANDLE hAction);
__MXAPI int *mxDlnaMrcp_loadMute(MRCP_ACTION_HANDLE hAction);
__MXAPI MXDLNA_USER_CHANNEL_LIST* mxDlnaMrcp_loadChannel(MRCP_ACTION_HANDLE hAction);
__MXAPI void mxDlnaMrcp_unloadChannel (MXDLNA_USER_CHANNEL_LIST*list);
__MXAPI MXDLNA_USER_IDS	*mxDlnaMrcp_loadConnectionIDs(MRCP_ACTION_HANDLE hAction);
__MXAPI void mxDlnaMrcp_unloadConnectionIDs(MXDLNA_USER_IDS	*start);
//__MXAPI void *mxDlnaMrcp_getMainConnectionData(MRCP_ACTION_HANDLE hAction);
__MXAPI MXDLNA_USER_RENDERER_INFO *mxDlnaMrcp_loadConnectionInfo(MRCP_ACTION_HANDLE hAction);
__MXAPI void mxDlnaMrcp_unloadConnectionInfo(MXDLNA_USER_RENDERER_INFO *info);
__MXAPI char *mxDlnaMrcp_makePeerConnectionManagerData(MRCP_ACTION_HANDLE hAction);	// PeerConnectionManager 값을 만듭니다.
__MXAPI int mxDlnaMrcp_isPrepareForConnection(MRCP_ACTION_HANDLE hAction, MXDLNA_UDN udn);
__MXAPI int mxDlnaMrcp_getSupportPlayMode(MRCP_ACTION_HANDLE hAction, int playmode);
__MXAPI int mxDlnaMrcp_getSupportVolume(MRCP_ACTION_HANDLE hAction); 
__MXAPI int mxDlnaMrcp_getSupportMute(MRCP_ACTION_HANDLE hAction); 



// @ CMS Invoke Function
__MXAPI int mxDlnaMrcp_createConnection(MRCP_ACTION_HANDLE hAction, char *RemoteProtocolInfo, char *PeerConnectionManager, int PeerConnectionID, char *Direction);
__MXAPI int mxDlnaMrcp_closeConnection(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getCurrentConnectionIDs(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getCurrentConnectionInfo(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getProtocolInfo(MRCP_ACTION_HANDLE hAction);

// @ AVT Invoke Function
__MXAPI int mxDlnaMrcp_getCurrentTransportActions(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getDeviceCapabilities(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getMediaInfo(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getTransportInfo(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getTransportSettings(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_setURLEx(MRCP_ACTION_HANDLE hAction, struct CdsObject* object );
__MXAPI int mxDlnaMrcp_setURL(MRCP_ACTION_HANDLE hAction, char *contentURL, char* unescaped_CurrentURIMetaData);
__MXAPI int mxDlnaMrcp_setSubtitleURL(MRCP_ACTION_HANDLE hAction, char *contentURL, char* unescaped_CurrentURIMetaData);
__MXAPI int mxDlnaMrcp_play(MRCP_ACTION_HANDLE hAction, char *playSpeed);
__MXAPI int mxDlnaMrcp_stop(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_pause(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_next(MRCP_ACTION_HANDLE hAction); 
__MXAPI int mxDlnaMrcp_prev(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_seek(MRCP_ACTION_HANDLE hAction, int Unit, unsigned long seekTimeOrTrack);
__MXAPI int mxDlnaMrcp_setPlayMode(MRCP_ACTION_HANDLE hAction, int playMode);
__MXAPI int mxDlnaMrcp_setNextURL(MRCP_ACTION_HANDLE hAction, char *contentURL, char* unescaped_CurrentURIMetaData);
__MXAPI int mxDlnaMrcp_getPositionInfo(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_X_DLNA_GetPositionInfo(MRCP_ACTION_HANDLE hAction);

// @ RCS Invoke Function
__MXAPI int mxDlnaMrcp_getBlueVideoBlackLevel(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getBlueVideoGain(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getBrightness(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getColorTemperature(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getContrast(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getGreenVideoBlackLevel(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getGreenVideoGain(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getHorizontalKeystone(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getLoudness(MRCP_ACTION_HANDLE hAction, char* unescaped_Channel);
__MXAPI int mxDlnaMrcp_getMute(MRCP_ACTION_HANDLE hAction, char* unescaped_Channel);
__MXAPI int mxDlnaMrcp_getRedVideoBlackLevel(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getRedVideoGain(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getSharpness(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getVerticalKeystone(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_getVolume(MRCP_ACTION_HANDLE hAction, char* unescaped_Channel);
__MXAPI int mxDlnaMrcp_getVolumeDB(MRCP_ACTION_HANDLE hAction, char* unescaped_Channel);
__MXAPI int mxDlnaMrcp_getVolumeDBRange(MRCP_ACTION_HANDLE hAction, char* unescaped_Channel);
__MXAPI int mxDlnaMrcp_listPresets(MRCP_ACTION_HANDLE hAction);
__MXAPI int mxDlnaMrcp_selectPreset(MRCP_ACTION_HANDLE hAction, char* unescaped_PresetName);
__MXAPI int mxDlnaMrcp_setBlueVideoBlackLevel(MRCP_ACTION_HANDLE hAction, unsigned short DesiredBlueVideoBlackLevel);
__MXAPI int mxDlnaMrcp_setBlueVideoGain(MRCP_ACTION_HANDLE hAction, unsigned short DesiredBlueVideoGain);
__MXAPI int mxDlnaMrcp_setBrightness(MRCP_ACTION_HANDLE hAction, unsigned short DesiredBrightness);
__MXAPI int mxDlnaMrcp_setColorTemperature(MRCP_ACTION_HANDLE hAction, unsigned short DesiredColorTemperature);
__MXAPI int mxDlnaMrcp_setContrast(MRCP_ACTION_HANDLE hAction, unsigned short DesiredContrast);
__MXAPI int mxDlnaMrcp_setGreenVideoBlackLevel(MRCP_ACTION_HANDLE hAction, unsigned short DesiredGreenVideoBlackLevel);
__MXAPI int mxDlnaMrcp_setGreenVideoGain(MRCP_ACTION_HANDLE hAction, unsigned short DesiredGreenVideoGain);
__MXAPI int mxDlnaMrcp_setHorizontalKeystone(MRCP_ACTION_HANDLE hAction, short DesiredHorizontalKeystone);
__MXAPI int mxDlnaMrcp_setLoudness(MRCP_ACTION_HANDLE hAction, char* unescaped_Channel, int DesiredLoudness);
__MXAPI int mxDlnaMrcp_setMute(MRCP_ACTION_HANDLE hAction, char *channel, int muteState);
__MXAPI int mxDlnaMrcp_setRedVideoBlackLevel(MRCP_ACTION_HANDLE hAction, unsigned short DesiredRedVideoBlackLevel);
__MXAPI int mxDlnaMrcp_setRedVideoGain(MRCP_ACTION_HANDLE hAction, unsigned short DesiredRedVideoGain);
__MXAPI int mxDlnaMrcp_setSharpness(MRCP_ACTION_HANDLE hAction, unsigned short DesiredSharpness);
__MXAPI int mxDlnaMrcp_setVerticalKeystone(MRCP_ACTION_HANDLE hAction, char *contentURL, short DesiredVerticalKeystone);
__MXAPI int mxDlnaMrcp_setVolume(MRCP_ACTION_HANDLE hAction, char *channel, char newVolumeValue);
__MXAPI int mxDlnaMrcp_setVolumeDB(MRCP_ACTION_HANDLE hAction, char* unescaped_Channel, short DesiredVolume);
__MXAPI int mxDlnaMrcp_setSubtitle(MRCP_ACTION_HANDLE hAction, char* SubtitleState);
__MXAPI int mxDlnaMrcp_setSubtitleSync(MRCP_ACTION_HANDLE hAction, int SyncValue);
__MXAPI char *mxDlnaMrcp_getDeviceDescriptionURL(MRCP_ACTION_HANDLE hAction, MXDLNA_UDN udn);
__MXAPI int mxDlnaMrcp_checkDevice(MRCP_ACTION_HANDLE hAction, char *deviceDescriptionURL);

// @ pulsus
#ifdef DMC_X_DO_COMMAND_SUPPORT
__MXAPI int mxDlnaMrcp_X_DoCommand(MRCP_ACTION_HANDLE hAction, char *command);
#endif

#ifdef DMC_X_GET_FLING_INFO_SUPPORT
__MXAPI int mxDlnaMrcp_X_GetFlingInfo(MRCP_ACTION_HANDLE hAction);
#endif


#ifdef MXDLNA_MRCP_OLD_VERSION_SUPPORT
__MXAPI void mxDlnaMrcp_config(MXDLNA_CP_CONFIG *conf);
__MXAPI char *mxDlnaMrcp_getCurrentRendererByUDN(MRCP_ACTION_HANDLE hAction);
__MXAPI void mxDlnaMrcp_msearch(MRCP_ACTION_HANDLE hAction);
//120904 jhbae : modified getDeviceInfo to retain the MXDLNA_DEVICE structure while it is used outside of library. So after using this method, should call mxDlnaMscp_freeDeviceInfo
__MXAPI MXDLNA_DEVICE *mxDlnaMrcp_getDeviceInfo(MRCP_ACTION_HANDLE hAction, MXDLNA_UDN udn);
__MXAPI void mxDlnaMrcp_freeDeviceInfo(MXDLNA_DEVICE *deviceInfo);
#endif





#ifdef __cplusplus
}
#endif

#endif /*__MXDLNA_DMRCP_PUDA__*/


