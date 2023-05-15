/**************************************************************************************/
/**
 * @file hxtransport_ftp.c
 * defines APIs's body
 *
 * APIs for the transport module using Ftp
 *
 * @author  Jongwon Kim(jwkim@humaxdigital.com)
 * @date    2011/05/07
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
**************************************************************************************
**/
#include <hlib.h>
#include "_hlib_transport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <curl/curl.h>

#include "hlib_transport_int.h"

#define HxTRANSPORT_MAX_FILE_PATH				(1024)
#define HxTRANSPORT_FTP_TIMEOUT_SEC				(60)

/*******************************************************************************************
 * Definitions
 ******************************************************************************************/

#undef	HxLOG_DOMAIN
#define HxLOG_DOMAIN	DOMAIN_TVSVC_PAL_TRANSPORT

typedef enum {
	eHxFTP_STATE_IDLE,
	eHxFTP_STATE_DOWNLOADING,
	eHxFTP_STATE_DOWNLOAD_FAILED,
	eHxFTP_STATE_DOWNLOAD_STOPPED,
	eHxFTP_STATE_DOWNLOAD_FINISHED,
} HxFtpState_e;

typedef struct {
	const HINT8 *szFilePath;
	FILE *ftpStream;
	HBOOL bStop;
	HUINT32		uploadSize;
	HFLOAT64	downloadedSize;
	HxFtpState_e eState;
	HxFtp_callback callback;
	void *handle;
	HINT32 calltime;
} HxTransport_FtpFile_t;

typedef struct  {
	CURL	*curl;
	HxTransport_FtpFile_t *fileinfo;
} HxTransport_Handle_t;

/*******************************************************************************************
 * Static functions
 ******************************************************************************************/
inline static CURL *__curl_handle(HxTRANSPORT_Ftp_t *ptFtp)
{
	return (CURL *)((HxTransport_Handle_t *)ptFtp)->curl;
}

inline static HxTransport_Handle_t *__ftp_handle(HxTRANSPORT_Ftp_t *ptFtp)
{
	return (HxTransport_Handle_t *)ptFtp;
}

static size_t download_callback(void *buffer, size_t size, size_t nmemb, void *stream)
{
	HxTransport_FtpFile_t *ptFile = (HxTransport_FtpFile_t *)stream;

	if ( !ptFile )
		return -1;

	if (!ptFile->ftpStream )
	{
		ptFile->ftpStream = fopen((const char *)ptFile->szFilePath, "wb");
		if ( !ptFile->ftpStream )
		{
			return -1;
		}

		ptFile->eState = eHxFTP_STATE_DOWNLOADING;
	}

	if ( ptFile->bStop == TRUE )
	{
		fclose(ptFile->ftpStream);
		(void)remove(ptFile->szFilePath);
		ptFile->bStop == FALSE;
		ptFile->eState = eHxFTP_STATE_DOWNLOAD_STOPPED;
		return -1;
	}

	ptFile->downloadedSize += ((HINT32)size*(HINT32)nmemb);
	ptFile->calltime += 1;

	if(ptFile->calltime > 5)
	{
		ptFile->calltime = 0;
		if(ptFile->callback) (* ptFile->callback)(eHxTRANSPORT_FTP_EVENT_PROGRESS_CHANGED, (HINT32)ptFile->handle, (HINT32)ptFile->downloadedSize);
	}

	return fwrite(buffer, size, nmemb, ptFile->ftpStream);
}


static size_t upload_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	HxTransport_FtpFile_t *ptFile = (HxTransport_FtpFile_t *)stream;

	size_t n = fread(ptr, size, nmemb, stream);

	ptFile->uploadSize += n;

	return n;
}

static size_t dummy_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	return (size_t)(size * nmemb);
}

/*******************************************************************************************
 * Interfaces
 ******************************************************************************************/
void HLIB_TRANSPORT_FtpInit(void)
{
	curlSo(curl_global_init(CURL_GLOBAL_ALL));
}

void HLIB_TRANSPORT_FtpDestory(void)
{
	curlSo(curl_global_cleanup());
}

void HLIB_TRANSPORT_FtpSetListener(HxTRANSPORT_Ftp_t *handle, HxFtp_callback callback, void *hAct)
{
	if ( callback )
	{
		__ftp_handle(handle)->fileinfo->callback = callback;
		__ftp_handle(handle)->fileinfo->handle = hAct;
	}
}

HxTRANSPORT_Ftp_t *HLIB_TRANSPORT_FtpOpen(void)
{
	HxTransport_Handle_t *pHandle = NULL;
	CURL *ctx = NULL;

	pHandle = (HxTransport_Handle_t *)HLIB_MEM_Malloc(sizeof(*pHandle));
	if ( !pHandle )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Creating the handle is failed.\n", __FUNCTION__);
		return NULL;
	}

	pHandle->fileinfo = (HxTransport_FtpFile_t *)HLIB_MEM_Malloc(sizeof(*(pHandle->fileinfo)));
	if ( !pHandle->fileinfo )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Creating the dummy filedata is failed.\n",
				__FUNCTION__);
		HLIB_MEM_Free(pHandle);
		return NULL;
	}
	pHandle->fileinfo->szFilePath = NULL;
	pHandle->fileinfo->ftpStream = NULL;
	pHandle->fileinfo->bStop = FALSE;
	pHandle->fileinfo->uploadSize = 0;
	pHandle->fileinfo->downloadedSize = 0;

	ctx = curlSo(curl_easy_init());
	if ( !ctx )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Open error.\n", __FUNCTION__);
		HLIB_MEM_Free(pHandle->fileinfo);
		HLIB_MEM_Free(pHandle);
		return NULL;
	}

#if defined(CONFIG_DEBUG)
	/* turn off the debugging. if you want to display msg of curl, chane 0L -> 1L */
	curlSo(curl_easy_setopt(ctx, CURLOPT_VERBOSE, 0L));
#endif
	curlSo(curl_easy_setopt(ctx, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4));

	/* signal problem, So set NOSIGNAL */
	curlSo(curl_easy_setopt(ctx, CURLOPT_NOSIGNAL, 1L));

	/* set PASV mode */
	curlSo(curl_easy_setopt(ctx, CURLOPT_FTP_USE_EPSV, 0));

	pHandle->curl = ctx;

	return (HxTRANSPORT_Ftp_t *)pHandle;
}

void HLIB_TRANSPORT_FtpClose(HxTRANSPORT_Ftp_t *ptFtp)
{
	if ( ptFtp )
	{
		if ( __ftp_handle(ptFtp)->curl )
		{
			curlSo(curl_easy_cleanup(__curl_handle(ptFtp)));
		}

		if ( __ftp_handle(ptFtp)->fileinfo )
		{
			HLIB_MEM_Free(__ftp_handle(ptFtp)->fileinfo);
		}

		HLIB_MEM_Free(__ftp_handle(ptFtp));
	}
}

HERROR HLIB_TRANSPORT_FtpSetUserPwd(HxTRANSPORT_Ftp_t *ptFtp, const HINT8 *szUserpwd)
{
	CURLcode curlRet = CURLE_OK;

	if ( !ptFtp || !szUserpwd )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_USERPWD, szUserpwd));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: UserPwd setting error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_FtpGetFileSize(HxTRANSPORT_Ftp_t *ptFtp, const HINT8 *szUri, HFLOAT64 *pflFileSize)
{
	CURLcode curlRet	= CURLE_OK;
	HFLOAT64 filesize = 0;

	if ( !ptFtp || !szUri || !pflFileSize)
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_URL, szUri));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Unknown or Invalid URL.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* No download if the file */

	curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_NOBODY, 1L));
	curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_HEADERFUNCTION, dummy_callback));
	curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_HEADER, 0L));

	curlRet = curlSo(curl_easy_perform(__curl_handle(ptFtp)));

	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: FTP communication is failed.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	curlRet = curlSo(curl_easy_getinfo(__curl_handle(ptFtp), CURLINFO_CONTENT_LENGTH_DOWNLOAD,
				&filesize));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Downloading a FTP data is failed.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	*pflFileSize = filesize;

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_FtpGetSize(HxTRANSPORT_Ftp_t *ptFtp, HFLOAT64 *pflFileSize)
{
	CURLcode curlRet	= CURLE_OK;
	HFLOAT64 filesize = 0;

	if ( ptFtp == NULL || pflFileSize == NULL)
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	curlRet = curlSo(curl_easy_getinfo(__curl_handle(ptFtp), CURLINFO_CONTENT_LENGTH_DOWNLOAD,
				&filesize));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Downloading a FTP data is failed.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	*pflFileSize = filesize;

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}


HERROR HLIB_TRANSPORT_FtpDownload(HxTRANSPORT_Ftp_t *ptFtp, const HINT8 *szUri,
								const HINT8 *szDownloadPath, HFLOAT64 *pflDownloadSize)
{
	CURLcode curlRet	= CURLE_OK;

	if ( !ptFtp || !szUri || !szDownloadPath )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	/* because this function will be fininshed after using the callback function
	 * the local variable "szDownloadPath" can be used without strcpy */
	__ftp_handle(ptFtp)->fileinfo->szFilePath = szDownloadPath;
	__ftp_handle(ptFtp)->fileinfo->ftpStream = NULL;
	__ftp_handle(ptFtp)->fileinfo->bStop = FALSE;
	__ftp_handle(ptFtp)->fileinfo->uploadSize = 0;
	__ftp_handle(ptFtp)->fileinfo->downloadedSize = 0;

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_URL, szUri));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Unknown or Invalid URL.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* the progress bar option will be disabled */
	curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_NOPROGRESS , 1L));

	/* set the callback function to the FTP transport module */
	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_WRITEFUNCTION, download_callback));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Invalid callback function.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* set the FTP data to pass to the callback function */
	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_WRITEDATA ,
				__ftp_handle(ptFtp)->fileinfo));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Invalid file stream.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* download a data from a server */
	curlRet = curlSo(curl_easy_perform(__curl_handle(ptFtp)));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Downloading a FTP data is failed.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	if ( pflDownloadSize )
	{
		curlRet = curlSo(curl_easy_getinfo(__curl_handle(ptFtp), CURLINFO_SIZE_DOWNLOAD,
					pflDownloadSize));
		if ( curlRet != CURLE_OK )
		{
			HxLOG_Print("[HxTRANSPORT][%s] Error: Getting a size of FTP data is failed.\n",
					__FUNCTION__);
			return HxTRANSPORT_RETURN(curlRet);
		}
	}

	if ( __ftp_handle(ptFtp)->fileinfo->ftpStream )
	{
		fclose(__ftp_handle(ptFtp)->fileinfo->ftpStream);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_FtpDownloadStop(HxTRANSPORT_Ftp_t *ptFtp, HBOOL *bStop)
{
	if ( !ptFtp )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	if(__ftp_handle(ptFtp)->fileinfo)
	{
		__ftp_handle(ptFtp)->fileinfo->bStop = TRUE;
	}

	if(bStop)
		*bStop = TRUE;

	return ERR_OK;
}

HERROR HLIB_TRANSPORT_FtpUpload(HxTRANSPORT_Ftp_t *ptFtp, const HINT8 *szUri,
								const HINT8 *szFilePath, HUINT32 *pulUploadSize)
{
	CURLcode curlRet	= CURLE_OK;
	FILE *curlFile		= NULL;
	struct stat finfo;
	HUINT32 ulFileSize	= 0;

	if ( !ptFtp || !szUri || !szFilePath )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	if ( stat((const char *)szFilePath, &finfo) != 0 )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: The file path is invalid.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	ulFileSize = finfo.st_size;

	curlFile = fopen((const char *)szFilePath, "rb");
	if ( !curlFile )
	{
		HxLOG_Print("[PAL_TRANSPORT][%s] Error: Invalid a file path.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_FILE_OPEN_ERROR);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_READFUNCTION, upload_callback));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Invalid callback function.\n", __FUNCTION__);
		fclose(curlFile);
		return HxTRANSPORT_RETURN(curlRet);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_UPLOAD, 1L));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Setting a upload mask to TP is failed.\n",
				__FUNCTION__);
		fclose(curlFile);
		return HxTRANSPORT_RETURN(curlRet);
	}

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_URL, szUri));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Unknown or Invalid URL.\n", __FUNCTION__);
		fclose(curlFile);
		return HxTRANSPORT_RETURN(curlRet);
	}

	/* the progress bar option will be disabled */
	curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_NOPROGRESS , 1L));

	/* set the file handle to the FTP transport module */
	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_READDATA , curlFile));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Invalid file stream.\n", __FUNCTION__);
		fclose(curlFile);
		return HxTRANSPORT_RETURN(curlRet);
	}

	//s_uploadSize = 0;

	/* upload a data to a server */
	curlRet = curlSo(curl_easy_perform(__curl_handle(ptFtp)));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Uploading a FTP data is failed.\n", __FUNCTION__);
		fclose(curlFile);
		return HxTRANSPORT_RETURN(curlRet);
	}

	fclose(curlFile);
	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_FtpSetTimeout(HxTRANSPORT_Ftp_t *ptFtp, HUINT32 ulTimeoutSec)
{
	CURLcode curlRet	= CURLE_OK;

	curlRet = curlSo(curl_easy_setopt(__curl_handle(ptFtp), CURLOPT_TIMEOUT, ulTimeoutSec));
	if ( curlRet != CURLE_OK )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Setting the maximu time is failed.\n",
				__FUNCTION__);
		return HxTRANSPORT_RETURN(curlRet);
	}

	return HxTRANSPORT_RETURN(eHxTPE_OK);
}

HERROR HLIB_TRANSPORT_FtpDownloadedSize(HxTRANSPORT_Ftp_t *ptFtp, HFLOAT64 *pSize)
{
	if ( !pSize )
	{
		HxLOG_Print("[HxTRANSPORT][%s] Error: Parameter error.\n", __FUNCTION__);
		return HxTRANSPORT_RETURN(eHxTPE_PARAMETER_ERROR);
	}

	*pSize = __ftp_handle(ptFtp)->fileinfo->downloadedSize;

	return ERR_OK;
}

