#ifndef __MXDLNA_DOWNLOAD_CONTROLLER_H__
#define __MXDLNA_DOWNLOAD_CONTROLLER_H__

#include "mxapi_define.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef void* mxDlnaDL_Session;
typedef void (*mxDlnaDL_OnDownloadResponse)(void * session, int error_code, int http_error_code, void* user_obj);


enum mxDlnaDL_Errors
{
	MXDLNA_DOWNLOAD_ERROR_NONE				= 0,
	MXDLNA_DOWNLOAD_ERROR_FAILED_OR_ABORTED	= 1,
	MXDLNA_DOWNLOAD_ERROR_FILE_NOT_EXIST	= 2,
	MXDLNA_DOWNLOAD_ERROR_HTTP_ERROR		= 3,
	MXDLNA_DOWNLOAD_ERROR_UNKNOWN			= 4,
	MXDLNA_DOWNLOAD_ERROR_DISK_FULL			= 5
};

#ifndef MXDLNA_HTTP_ERRORS
#define MXDLNA_HTTP_ERRORS
enum mxDlnaHTTP_Errors
{
	MXDLNA_HTTP_ERRORS_NONE = 0,
	MXDLNA_HTTP_ERRORS_PEER_FRIENDLY_UNSPECIFIED, 
	MXDLNA_HTTP_ERRORS_CONNECTION_FAILED_OR_ABORTED =5,
	MXDLNA_HTTP_ERRORS_HTTP = 10
};
#endif


__MXAPI void mxDlnaDL_init();
__MXAPI void mxDlnaDL_destroy();
__MXAPI int mxDlnaDL_isResumeSupported(mxDlnaDL_Session session);

__MXAPI MX_INT64 mxDlnaDL_getBytesReceived(mxDlnaDL_Session session);
__MXAPI MX_INT64 mxDlnaDL_getTotalBytesExpected(mxDlnaDL_Session session);

__MXAPI int mxDlnaDL_isPaused(mxDlnaDL_Session session);
__MXAPI int mxDlnaDL_isPausedByFlowControl(mxDlnaDL_Session session);
__MXAPI int	mxDlnaDL_pauseTransfer(mxDlnaDL_Session session);
__MXAPI int mxDlnaDL_pauseTransferByFlowControl(mxDlnaDL_Session session);
__MXAPI int mxDlnaDL_resumeTransfer(mxDlnaDL_Session session);
__MXAPI int mxDlnaDL_resumeTransferByFlowControl(mxDlnaDL_Session session);
__MXAPI int mxDlnaDL_abortTransfer(mxDlnaDL_Session session);

#ifdef WIN32
__MXAPI mxDlnaDL_Session mxDlnaDL_downloadObject(const struct CdsResource* download_this, int resume_flag, char* save_as_file_name, mxDlnaDL_OnDownloadResponse callback_response, void* user_obj, int mode);
__MXAPI mxDlnaDL_Session mxDlnaDL_downloadObject2(char *cURI, char *cProtocolInfo, int resume_flag, char* save_as_file_name, mxDlnaDL_OnDownloadResponse callback_response, void* user_obj, int mode);

//__MXAPI mxDlnaDL_Session mxDlnaDL_downloadObject(const struct CdsResource* download_this, int resume_flag, char* save_as_file_name, mxDlnaDL_OnDownloadResponse callback_response, void* user_obj);
//__MXAPI mxDlnaDL_Session mxDlnaDL_downloadObject2(char *cURI, char *cProtocolInfo, int resume_flag, char* save_as_file_name, mxDlnaDL_OnDownloadResponse callback_response, void* user_obj);
#endif

#ifdef __cplusplus
}
#endif

#endif /*__MXDLNA_DOWNLOAD_CONTROLLER_H__*/

