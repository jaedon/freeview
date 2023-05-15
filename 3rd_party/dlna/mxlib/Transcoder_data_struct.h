#ifndef TRANSCODER_DATA_STRUCT_H
#define TRANSCODER_DATA_STRUCT_H

#include "mxkernel.h"
#include "mxtypes.h"

#ifdef WIN32
#	if !defined(strncasecmp)
#define strncasecmp(x,y,z) _strnicmp(x,y,z)
#	endif
#endif

/*******************************************************************************
 *	Transcdoer API wrapper dll.
 *  using ffmpeg.exe and it's family dll, for transcoding.
 *        segmenter.exe for segmenting.
 *  Primary target is http live streaming for iOS with transcoder_live_apis.
 *  And offline converting with transcoder_conv_apis.
 *  Some others for getting thumbnail, metadata, etc.
 ******************************************************************************/

/*******************************************************************************
 *	error code definitions
 ******************************************************************************/
#define		TRANSCODER_NO_ERROR							(0)
#define		TRANSCODER_ERROR_UNKNOWN					(-1)
#define		TRANSCODER_ERROR_OUT_OF_MEMORY				(-2)
#define		TRANSCODER_ERROR_FILE_NOT_FOUND				(-101)
#define		TRANSCODER_ERROR_FILE_WRITE_FAIL			(-102)
#define		TRANSCODER_ERROR_INVALID_DIRECTORY			(-103)
#define		TRANSCODER_ERROR_FFMPEG_NOT_FOUND			(-201)
#define		TRANSCODER_ERROR_FFMPEG_FAIL				(-202)
#define		TRANSCODER_ERROR_FFMPEG_PROCESS_FAIL		(-203)
#define		TRANSCODER_ERROR_FFMPEG_TIMEOUT				(-204)
#define		TRANSCODER_ERROR_FFMPEG_BAD_FILE			(-205)
#define 	TRANSCODER_ERROR_FFMPEG_FILE_NOT_FOUND		(-206)

#define		TRANSCODER_ERROR_FD_DUPLICATION_FAIL		(-301)
#define		TRANSCODER_ERROR_PIPE_CREATION_FAIL			(-302)
#define		TRANSCODER_ERROR_PROCESS_CREATION_FAIL		(-303)
#define		TRANSCODER_ERROR_THREAD_CREATION_FAIL		(-304)
#define		TRANSCODER_ERROR_METADATA_NOT_FOUND			(-401)
#define		TRANSCODER_ERROR_BENCHMARK_BUSY				(-501)
#define		TRANSCODER_ERROR_BENCHMARK_TOO_SHORT_FILE	(-502)


#define		TRANSCODER_ERROR_THUMBNAIL_NOT_SUPPORT   	(-600)

#define		TRANSCODER_ERROR_LIVE_UNKNOWN				(-1001)
#define		TRANSCODER_ERROR_LIVE_OUT_OF_MEMORY			(-1002)
#define		TRANSCODER_ERROR_LIVE_FILE_NOT_FOUND		(-1101)
#define		TRANSCODER_ERROR_LIVE_FILE_WRITE_FAIL		(-1102)
#define		TRANSCODER_ERROR_LIVE_TS_NOT_READY			(-1103)//Is this really error??
#define		TRANSCODER_ERROR_LIVE_TS_OUT_OF_INDEX		(-1104)//Is this really error??
#define		TRANSCODER_ERROR_LIVE_FFMPEG_NOT_FOUND		(-1201)
#define		TRANSCODER_ERROR_LIVE_FFMPEG_FAIL			(-1202)
#define		TRANSCODER_ERROR_LIVE_INVALID_REQUEST		(-1301)

#define		TRANSCODER_ERROR_CONV_UNKNOWN				(-2001)
#define		TRANSCODER_ERROR_CONV_OUT_OF_MEMORY			(-2002)
#define		TRANSCODER_ERROR_CONV_FILE_NOT_FOUND		(-2101)
#define		TRANSCODER_ERROR_CONV_FILE_WRITE_FAIL		(-2102)
#define		TRANSCODER_ERROR_CONV_FFMPEG_NOT_FOUND		(-2201)
#define		TRANSCODER_ERROR_CONV_FFMPEG_FAIL			(-2202)

#define		TRANSCODER_ERROR_QUEUE_FULL					(-3002)

#define		TRANSCODER_ERROR_NOT_INIT					(-4001)

/*******************************************************************************
 *	some constant definitions
 ******************************************************************************/

#if !defined(USE_MBCS_FILENAME)
#define		TRANSCODER_REPORT_FILENAME					L"transcoder_report.log"
#else
#define		TRANSCODER_REPORT_FILENAME					"transcoder_report.log"
#endif

#define		TRANSCODER_REPORT_BUFFER_SIZE				(4096)
#define		TRANSCODER_GENERAL_TIMEOUT_MS				(3*1000)
#define		TRANSCODER_THUMBNAIL_TIMEOUT_MS				(20*1000)
#define		TRANSCODER_IMAGERESIZE_TIMEOUT_MS			(60*1000)
#define		TRANSCODER_HLS_TIMEOUT_MS				    (20*1000)
#define		TRANSCODER_HLS_TIMEGAP_MS				    (300*1000)
#define		TRANSCODER_HLS_PROCESS_TIMEOUT_MS			(30*1000)

#if !(defined(MAX_PATH))
#define		MAX_PATH									260
#endif
#if !(defined(MAX_LINE))
#define		MAX_LINE									1024
#endif


#define		BITRATE_SIZE								8
#define		TRANSCODER_MAX_FILENAME						MAX_PATH * 4

#if defined(WIN32)

#	if !defined(BOOL)
typedef int BOOL;
//typedef unsigned short wchar_t;
#	endif

#else
#	if !defined(BOOL)
typedef		int		BOOL;
typedef		BOOL*	PBOOL;
typedef		size_t		HANDLE;
typedef 	HANDLE*	PHANDLE;
typedef		unsigned int		DWORD;
#	endif

#define		GetOEMCP(X)	949
#define		CloseHandle(X)
#define		TerminateThread(X,Y)
#define		TerminateProcess(X,Y)
#define		ResumeThread(X)
#define		SuspendThread(X)

#define	WINAPI
#define		TRUE	1
#define		FALSE	0
#define		LPVOID	void*
#endif
/*******************************************************************************
 *	some definitions and structures to use transcoder_api
 ******************************************************************************/
typedef struct transcoder_buffer_tag
{
	unsigned char	*ptr;
	unsigned int	size;
	// 밑에 처럼 수정해야하는데...
	//unsigned int	nBufsize;
	//unsigned int	nDataSize;
	char			localfilename[TRANSCODER_MAX_FILENAME]; //<<- confusing?
// --------------------------------------------------------//
} TRANSCODER_BUFFER, *PTRANSCODER_BUFFER;
// --------------------------------------------------------//
typedef struct transcoder_metadata_tag
{
	int				duration_sec;
	double			start_time_sec;
	double			filesize_byte_in_double;
	int				bitrate_kbps;
	int				audio_bitrate_kbps;
	int				video_width;
	int				video_height;
	double			src_frame_rate;
	BOOL			isBadFile;
	int				nMediaType;
//	char			video_codec[32];
//	char			title[MAX_PATH];
// --------------------------------------------------------//
} TRANSCODER_METADATA, *PTRANSCODER_METADATA;
// --------------------------------------------------------//
typedef struct _transcoder_benchmark_tag
{
	char					filename[TRANSCODER_MAX_FILENAME];

	TRANSCODER_METADATA		Metadata;
	int				target_bitrate_kbps;
	int				video_width;
	int				video_height;
	double			target_frame_rate;

	unsigned int	tickcount_start;
	unsigned int	tickcount_end;

	double			elapsed_time_sec;
	double			cpu_time_sec;//unavailable now.
	double			fps_average;
	double			fps_max;
	double			fps_min;
	int				max_consecutive_under_frame;
	char 			report[TRANSCODER_REPORT_BUFFER_SIZE];

	BOOL			bPassed;
	// --------------------------------------------------------//
} TRANSCODER_BENCHMARK, *PTRANSCODER_BENCHMARK;
// --------------------------------------------------------//
typedef void(*TRANSCODER_BENCHMARK_CALLBACK)(PTRANSCODER_BENCHMARK pBenchmark);





typedef enum _TRANSCODER_COMMAND_TYPE
{
	eTR_COMMAND_NONE =0,
	eTR_COMMAND_STOP,

}TRANSCODER_COMMAND_TYPE;


typedef enum _TRANSCODER_MEDIA_TYPE
{
	eTR_MEDIA_NONE =0,
	eTR_MEDIA_VIDEO,
	eTR_MEDIA_VIDEO_ONLY,
	eTR_MEDIA_AUDIO,
	eTR_MEDIA_IMAGE,
	eTR_MEDIA_MAX,
}TRANSCODER_MEDIA_TYPE;
// --------------------------------------------------------//
typedef enum _TRANSCODER_FINISH_NOTIFY_TYPE
{
	TRANSCODER_THUMBNAIL	= 1,
	TRANSCODER_CONVERSION	= 2,
} TRANSCODER_TYPE;

// 아래 flag와 통합되어도 좋을듯 합니다.
typedef enum _TRANSCODER_STREAM_TYPE
{
	ETRANSCODER_STREAM_LIVE	= 0,
	ETRANSCODER_STREAM_FILE = 1,
} TRANSCODER_STREAM_TYPE;


// Device 타입
typedef enum _TRANSCODER_REQUSET_DEVICE_TYPE
{
	TRANSCODER_REQUSET_DEVICE_UNKNOWN_TYPE = -1,
	TRANSCODER_REQUSET_DEVICE_DMS_TYPE,
	TRANSCODER_REQUSET_DEVICE_WOON_TYPE,
	TRANSCODER_REQUSET_DEVICE_SATIP_TYPE,
	TRANSCODER_REQUSET_DEVICE_NTLS_TYPE,  // bypass mode
	TRANSCODER_REQUSET_DEVICE_MPEG_DASH_TYPE,

} TRANSCODER_REQUSET_DEVICE_TYPE;


// 설정 값
typedef enum _TRANSCODER_REQUEST_FLAG_SETTING_TYPE
{
	TRANSCODER_REQUEST_FLAG_SETTING_UNKNOWN_TYPE = -1,

	TRANSCODER_REQUEST_FLAG_SETTING_M3U8_TYPE	= 1,			// M3U8이 필요하다 / 필요하지 않다. (향후 SAT>IP에서 HTTP를 지원하는 경우를 대비함.)
	TRANSCODER_REQUEST_FLAG_SETTING_ONLY_AUDIO_TYPE	= 2,		// Audio만 필요하다.
	TRANSCODER_REQUEST_FLAG_SETTING_LIVE_TYPE	= 4,			// 라이브 형태 요청 이다 / 라이브 형태 요청이 아니다.
	TRANSCODER_REQUEST_FLAG_SETTING_NTLS_TYPE = 8,
} TRANSCODER_REQUEST_FLAG_SETTING_TYPE;


// 설정 값을 좀 더 쓰기 편하게 설정한 define
typedef enum _TRANSCODER_REQUEST_FLAG_TYPE
{
	TRANSCODER_REQUEST_FLAG_UNKNOWN_TYPE				= -1,
	TRANSCODER_REQUEST_FLAG_AUDIO_RECODING_TYPE			=	(TRANSCODER_REQUEST_FLAG_SETTING_M3U8_TYPE | TRANSCODER_REQUEST_FLAG_SETTING_ONLY_AUDIO_TYPE),
	TRANSCODER_REQUEST_FLAG_VIDEO_RECODING_TYPE			=	(TRANSCODER_REQUEST_FLAG_SETTING_M3U8_TYPE),
	TRANSCODER_REQUEST_FLAG_AUDIO_LIVE_TYPE				=	(TRANSCODER_REQUEST_FLAG_SETTING_M3U8_TYPE | TRANSCODER_REQUEST_FLAG_SETTING_ONLY_AUDIO_TYPE | TRANSCODER_REQUEST_FLAG_SETTING_LIVE_TYPE),
	TRANSCODER_REQUEST_FLAG_VIDEO_LIVE_TYPE				=	(TRANSCODER_REQUEST_FLAG_SETTING_M3U8_TYPE | TRANSCODER_REQUEST_FLAG_SETTING_LIVE_TYPE),
	TRANSCODER_REQUEST_FLAG_LIVE_RTP_TYPE				=	(TRANSCODER_REQUEST_FLAG_SETTING_LIVE_TYPE),
	TRANSCODER_REQUEST_FLAG_LIVE_NTLS_TYPE				=	(TRANSCODER_REQUEST_FLAG_SETTING_NTLS_TYPE),

} TRANSCODER_REQUEST_FLAG_TYPE;

// dypark : 원본위치 확인을 위해서 수정.
typedef enum _TRANSCODER_IPROFILE_TYPE
{
	TRANSCODER_IPROFILE_TYPE_FILE = 0,
	TRANSCODER_IPROFILE_TYPE_URL = 1,

	TRANSCODER_IPROFILE_TYPE_CONVERTER_HQ_FILE = (2 + TRANSCODER_IPROFILE_TYPE_FILE),
	TRANSCODER_IPROFILE_TYPE_CONVERTER_LQ_FILE = (4 + TRANSCODER_IPROFILE_TYPE_FILE),

	TRANSCODER_IPROFILE_TYPE_CONVERTER_HQ_URL= (8 + TRANSCODER_IPROFILE_TYPE_URL),
	TRANSCODER_IPROFILE_TYPE_CONVERTER_LQ_URL= (16 + TRANSCODER_IPROFILE_TYPE_URL),

	TRANSCODER_IPROFILE_TYPE_THUMBNAIL_FILE = (32 + TRANSCODER_IPROFILE_TYPE_FILE),
	TRANSCODER_IPROFILE_TYPE_THUMBNAIL_URL= (64 + TRANSCODER_IPROFILE_TYPE_URL),

} TRANSCODER_IPROFILE_TYPE;


typedef int (*TRASCODER_CALLBACK_KILL)(void* userObject, char *in_key);


// info 타입
typedef enum _TRANSCODER_REQUSET_INFO_TYPE
{
	TRANSCODER_REQUSET_INFO_UNKNOWN_TYPE = -1,
	TRANSCODER_REQUSET_INFO_SATIP_SDP,
	TRANSCODER_REQUSET_INFO_SATIP_OPTION,
} TRANSCODER_REQUSET_INFO_TYPE;

typedef enum
{
	STREAMING_PROTOCOL_UNKNOWN = 0,
	STREAMING_PROTOCOL_HLS,
	STREAMING_PROTOCOL_HTTP,
	STREAMING_PROTOCOL_SATIP

} STREAMING_PROTOCOL;

typedef struct _transcoder_instance_tag
{
	char					szKey[MAX_PATH];
	char					szDeviceId[MAX_PATH];
	char					szStreamid[MAX_PATH];
	char					szIPAdrr[MAX_PATH];
	char					filename[TRANSCODER_MAX_FILENAME];
	char					subtitle_filename[TRANSCODER_MAX_FILENAME];
	int                     nStreamType;
//	MX_UINT64           	ulTickTimeLastUse;
	//char					mainM3U8NameforClient[MAX_PATH];//just make on demand.
	TRANSCODER_BUFFER		M3U8_main;
	int                     nMax_bitrate;
	TRANSCODER_BUFFER		*pM3U8_sub;
	char					szErrorFunc[TRANSCODER_MAX_FILENAME];
	int 					szErrorCode;
	int                     nRef;
//	MXKERNEL_CRITICALSECTION Lock_Inst;


	//int 					szErrordes[TRANSCODER_MAX_FILENAME];

	//PFFMPEG_TRANSCODER_INSTANCE pFFmpeg_Inst;

	void					*ptrans_inst;
	int                     (*Clear_transinst)(void * pCurinst);

	int nRemove;

	struct _transcoder_instance_tag	*pNext;


	void * pUserData;
	void * pFastUserData;
	//void * user_data_free;

	TRANSCODER_REQUSET_DEVICE_TYPE nRequest_type;
	TRANSCODER_REQUEST_FLAG_TYPE nRequest_flag;


	TRASCODER_CALLBACK_KILL destroy_callback;
	void *destroy_user_data;

	STREAMING_PROTOCOL	protocol;
// --------------------------------------------------------//
} TRANSCODER_INSTANCE, *PTRANSCODER_INSTANCE;
// --------------------------------------------------------//


typedef void(*TRANSCODER_FINISH_NOTIFY_CALLBACK)(const char* device_id,
												 const char* object_id,
												 const char* InFullPathFileName,
												 const char * OutFullPathFileName,
												 void* user_data,
												 TRANSCODER_TYPE transcoder_type,
												 int iProfile,
												 int iErrorCode);

typedef void(*TRANSCODER_FINISH_EMPTY_CALLBACK)(void);

typedef int (*fp_TRANSCODE_HLS_State)         ( const char *session_id, int state);





#endif
