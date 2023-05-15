#ifndef _TRANSEXTERNAPI_H_
#define _TRANSEXTERNAPI_H_

typedef enum _Streaming_ReturnCode
{
	/* 0 보다큰값은count로사용*/
	eStreamingRet_OK					= 0,
	eStreamingRet_Error					= -1,
	eStreamingRet_EOF					= -2,
	eStreamingRet_Cancel				= -3,
	eStreamingRet_Busy					= -4,
	eStreamingRet_Lock					= -5,
	eStreamingRet_OutOfMem				= -6,
	eStreamingRet_InvalidArg			= -7,
	eStreamingRet_Destroyed				= -8,
	eStreamingRet_Timeout				= -9,
	eStreamingRet_Unsupported			= -10,
	eStreamingRet_Suspended				= -11,
	eStreamingRet_IOError				= -12,
	eStreamingRet_Overflow				= -13,
	eStreamingRet_Underflow				= -14,
	eStreamingRet_OutOfRange			= -15,
	eStreamingRet_InitFail				= -16,
	eStreamingRet_DoNotInit				= -17,
	eStreamingRet_Conflict				= -18,
	eStreamingRet_NoResource			= -19,
	eStreamingRet_InvalidResult			= -20,
	eStreamingRet_InvalidStreamingType	= -21,
	eStreamingRet_StartError			= -22
} Streaming_RetCode_e;

typedef enum _Streaming_ErrorState
{
	/* 0 보다큰값은count로사용*/
	eStreamingErrorState_OK          = 0,
	eStreamingErrorState_Error       = -1,
	eStreamingErrorState_EOF         = -2,
	eStreamingErrorState_Cancel      = -3,
	eStreamingErrorState_Busy        = -4,
	eStreamingErrorState_Lock        = -5,
	eStreamingErrorState_OutOfMem    = -6,
	eStreamingErrorState_Destroyed   = -8,
	eStreamingErrorState_Timeout     = -9,
	eStreamingErrorState_Unsupported = -10,
	eStreamingErrorState_Suspended   = -11,
	eStreamingErrorState_IOError     = -12,
	eStreamingErrorState_Overflow    = -13,
	eStreamingErrorState_Underflow   = -14,
	eStreamingErrorState_Conflict    = -18,
	/* 추가요망*/
} Streaming_ErrState_e;

typedef enum _eStreaming_SpecType_e
{
	eStreamingSpecType_Unknown,
	eStreamingSpecType_DMS,
	eStreamingSpecType_WOON,
	eStreamingSpecType_SATIP,
	eStreamingSpecType_NTLS,  // bypass mode
	eStreamingSpecType_MPEG_DASH,
	eStreamingSpecType_MAX
} Streaming_SpecType_e;

typedef enum _Streaming_MediaType_e
{
	eStreamingMediaType_Unknown,
	eStreamingMediaType_PVRTS_AUDIO,
	eStreamingMediaType_PVRTS_VIDEO,
	eStreamingMediaType_LIVE_AUDIO,
	eStreamingMediaType_LIVE_VIDEO,
	eStreamingMediaType_LIVE_RTP,
	eStreamingMediaType_MAX
} Streaming_MediaType_e;

typedef Streaming_RetCode_e	(*fp_Streaming_Start)			( 	Streaming_SpecType_e 	svc,
																Streaming_MediaType_e 	media,
																const char 				*device_name,
																const char 				*user_agent,
																const char 				*session_id,
																const char 				*path_name,
																const char 				*ip,
																int                     socketfd[2], /* 2015/3/19 mhkang: for sat>ip. refer to http://svn:3000/issues/101671 */
																int 					*hStream);
typedef Streaming_RetCode_e (*fp_Streaming_Stop)			( const int hStream);

typedef Streaming_RetCode_e (*fp_Streaming_M3U8_Get)       	( const int hStream, const char *path_name, unsigned int *bufSize, char **m3u8buf);
typedef Streaming_RetCode_e (*fp_Streaming_M3U8_Release)	( const int hStream, const char *path_name, char *m3u8buf);

typedef Streaming_RetCode_e (*fp_Streaming_TS_Get)			( const int hStream, const char *path_name, unsigned int *bufSize, char **tsbuf);
typedef Streaming_RetCode_e (*fp_Streaming_TS_Release)		( const int hStream, const char *path_name, char *tsbuf);

typedef Streaming_RetCode_e (*fp_Streaming_Info_Get)		( const int hStream, const char *cmd, const char *path_name, unsigned int *bufSize, char **tsbuf);
typedef Streaming_RetCode_e (*fp_Streaming_Info_Release)	( const int hStream, const char *cmd, const char *path_name, char *buf);

typedef Streaming_RetCode_e (*fp_Streaming_Init)	  		( void );
typedef Streaming_RetCode_e (*fp_Streaming_Deinit)       	( void );

typedef int (*cb_Streaming_State)									( const char *session_id, /* Streaming_ErrState_e or */int state);
typedef Streaming_RetCode_e (*fp_Streaming_RegisterStateCallback) 	( const cb_Streaming_State callback);

typedef struct _TransExApi
{
	fp_Streaming_Start					fp_ExStart;
	fp_Streaming_Stop					fp_ExStop;

	fp_Streaming_M3U8_Get				fp_ExM3U8_Get;
	fp_Streaming_M3U8_Release			fp_ExM3U8_Release;

	fp_Streaming_TS_Get					fp_ExTS_Get;
	fp_Streaming_TS_Release				fp_ExTS_Release;

	fp_Streaming_Info_Get				fp_ExInfo_Get;
	fp_Streaming_Info_Release			fp_ExInfo_Release;

	fp_Streaming_Init					fp_ExInit;
	fp_Streaming_Deinit					fp_ExDeinit;

	fp_Streaming_RegisterStateCallback	fp_ExRegisterStateCallback;
}TransExApi, *PTransExApi;
#endif
