#ifndef __UPLOAD_CONTROLLER_H__
#define __UPLOAD_CONTROLLER_H__

#include "mxapi_define.h"


#ifdef __cplusplus
extern "C"{
#endif

typedef void* mxDlnaUL_Session;
//typedef void (*mxDlnaUL_OnCreateObjectUploadResponse)(mxDlnaUL_Session session, enum mxDlnaUL_Errors error_code, int http_error_code, struct CdsObject *result, void* user_obj);
//typedef void (*mxDlnaUL_OnDestroyObjectResponse)(enum mxDlnaUL_Errors error_code, void* user_obj);
typedef void (*mxDlnaUL_OnCreateObjectUploadResponse)(mxDlnaUL_Session session, int error_code, int http_error_code, struct CdsObject *result, void* user_obj);
typedef void (*mxDlnaUL_OnDestroyObjectResponse)(int error_code, void* user_obj);

enum mxDlnaUL_Errors
{
	MXDLNA_UPLOAD_ERROR_NONE					= 0,
	MXDLNA_UPLOAD_ERROR_FAILED_OR_ABORTED		= 1,
	MXDLNA_UPLOAD_ERROR_RESUME_OR_RETRY_FAILED	= 2,
	MXDLNA_UPLOAD_ERROR_SERVER_NOT_EXIST		= 3,
	MXDLNA_UPLOAD_ERROR_HTTP_ERROR				= 4,
	MXDLNA_UPLOAD_ERROR_INVALID_CONTENT			= 5,
	MXDLNA_UPLOAD_ERROR_XML_NOT_WELL_FORMED		= 6,
	MXDLNA_UPLOAD_ERROR_CREATE_OBJECT_FAILED	= 7,
	MXDLNA_UPLOAD_ERROR_IMPORT_URI_NOT_AVAILBLE = 8,
	MXDLNA_UPLOAD_ERROR_UNKNOWN					= 9,
	MXDLNA_UPLOAD_ERROR_RESUMED					= 10,
	MXDLNA_UPLOAD_ERROR_FILE_NOT_EXIST			= 11
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

__MXAPI void mxDlnaUL_init();
__MXAPI void mxDlnaUL_destroy();

__MXAPI void mxDlnaUL_contentDirectoryCreateObject(void *serviceObj, struct CdsObject *obj);
__MXAPI void mxDlnaUL_contentDirectoryDestroyObject(void *serviceObj, struct CdsObject *obj);
__MXAPI void mxDlnaUL_contentDirectoryBrowse(void *serviceObj, struct CdsObject *obj);

__MXAPI mxDlnaUL_Session mxDlnaUL_uploadObject(int any_container_flag, 
									   int resume_flag,
									   char *parentID,
									   void *service_object,
									   struct CdsObject* new_object, 
									   char* file_path, 
									   mxDlnaUL_OnCreateObjectUploadResponse callback_response, 
									   void* user_obj);
__MXAPI void mxDlnaUL_destroyObject(struct CdsObject *obj, void *service_object,
							mxDlnaUL_OnDestroyObjectResponse callback_response, void* user_obj);

__MXAPI int mxDlnaUL_isResumeSupported(mxDlnaUL_Session session);

__MXAPI MX_INT64 mxDlnaUL_getBytesSent(mxDlnaUL_Session session);
__MXAPI MX_INT64 mxDlnaUL_getTotalBytes(mxDlnaUL_Session session);

__MXAPI int mxDlnaUL_isPaused(mxDlnaUL_Session session);
__MXAPI int mxDlnaUL_isPausedByFlowControl(mxDlnaUL_Session session);
__MXAPI int mxDlnaUL_pauseTransfer(mxDlnaUL_Session session);
__MXAPI int mxDlnaUL_pauseTransferByFlowControl(mxDlnaUL_Session session);
__MXAPI int mxDlnaUL_resumeTransfer(mxDlnaUL_Session session, void *service_object);
__MXAPI int mxDlnaUL_resumeTransferByFlowControl(mxDlnaUL_Session session);
__MXAPI int mxDlnaUL_abortTransfer(mxDlnaUL_Session session);

#ifdef __cplusplus
}
#endif

#endif /*__UPLOAD_CONTROLLER_H__*/
