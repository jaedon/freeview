/*************************************************************
 * @file		hxtransport.h
 * @date		2012/08/10
 * @author		humax
 * @brief		HUMAX CURL library header

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef _HxTRANSPORT_H_
#define _HxTRANSPORT_H_
#include <htype.h>


/*******************************************************************/
/********************     ENUM  			*************************/
/*******************************************************************/
#define _______________Enum__________

/**
 * The error codes of the transport module
 */
typedef enum  {
	/* default curl library error code */
	eHxTPE_OK = 0,					 /** 0 works well~ */
	eHxTPE_UNSUPPORTED_PROTOCOL,    /** 1 */
	eHxTPE_FAILED_INIT,             /** 2 */
	eHxTPE_URL_MALFORMAT,           /** 3 */
	eHxTPE_OBSOLETE4,               /** 4 - NOT USED */
	eHxTPE_COULDNT_RESOLVE_PROXY,   /** 5 */
	eHxTPE_COULDNT_RESOLVE_HOST,    /** 6 */
	eHxTPE_COULDNT_CONNECT,         /* *7 */
	eHxTPE_FTP_WEIRD_SERVER_REPLY,  /** 8 */
	eHxTPE_REMOTE_ACCESS_DENIED,    /** 9 a service was denied by the server
									  due to lack of access - when login fails
									  this is not returned. */
	eHxTPE_OBSOLETE10,              /** 10 - NOT USED */
	eHxTPE_FTP_WEIRD_PASS_REPLY,    /** 11 */
	eHxTPE_OBSOLETE12,              /** 12 - NOT USED */
	eHxTPE_FTP_WEIRD_PASV_REPLY,    /** 13 */
	eHxTPE_FTP_WEIRD_227_FORMAT,    /** 14 */
	eHxTPE_FTP_CANT_GET_HOST,       /** 15 */
	eHxTPE_OBSOLETE16,              /** 16 - NOT USED */
	eHxTPE_FTP_COULDNT_SET_TYPE,    /** 17 */
	eHxTPE_PARTIAL_FILE,            /** 18 */
	eHxTPE_FTP_COULDNT_RETR_FILE,   /** 19 */
	eHxTPE_OBSOLETE20,              /** 20 - NOT USED */
	eHxTPE_QUOTE_ERROR,             /** 21 - quote command failure */
	eHxTPE_HTTP_RETURNED_ERROR,     /** 22 */
	eHxTPE_WRITE_ERROR,             /** 23 */
	eHxTPE_OBSOLETE24,              /** 24 - NOT USED */
	eHxTPE_UPLOAD_FAILED,           /** 25 - failed upload "command" */
	eHxTPE_READ_ERROR,              /** 26 - couldn't open/read from file */
	eHxTPE_OUT_OF_MEMORY,           /** 27 */
	eHxTPE_OPERATION_TIMEDOUT,      /** 28 - the timeout time was reached */
	eHxTPE_OBSOLETE29,              /** 29 - NOT USED */
	eHxTPE_FTP_PORT_FAILED,         /** 30 - FTP PORT operation failed */
	eHxTPE_FTP_COULDNT_USE_REST,    /** 31 - the REST command failed */
	eHxTPE_OBSOLETE32,              /** 32 - NOT USED */
	eHxTPE_RANGE_ERROR,             /** 33 - RANGE "command" didn't work */
	eHxTPE_HTTP_POST_ERROR,         /** 34 */
	eHxTPE_SSL_CONNECT_ERROR,       /** 35 - wrong when connecting with SSL */
	eHxTPE_BAD_DOWNLOAD_RESUME,     /** 36 - couldn't resume download */
	eHxTPE_FILE_COULDNT_READ_FILE,  /** 37 */
	eHxTPE_LDAP_CANNOT_BIND,        /** 38 */
	eHxTPE_LDAP_SEARCH_FAILED,      /** 39 */
	eHxTPE_OBSOLETE40,              /** 40 - NOT USED */
	eHxTPE_FUNCTION_NOT_FOUND,      /** 41 */
	eHxTPE_ABORTED_BY_CALLBACK,     /** 42 */
	eHxTPE_BAD_FUNCTION_ARGUMENT,   /** 43 */
	eHxTPE_OBSOLETE44,              /** 44 - NOT USED */
	eHxTPE_INTERFACE_FAILED,        /** 45 - INTERFACE failed */
	eHxTPE_OBSOLETE46,              /** 46 - NOT USED */
	eHxTPE_TOO_MANY_REDIRECTS ,     /** 47 - catch endless re-direct loops */
	eHxTPE_UNKNOWN_TELNET_OPTION,   /** 48 - User specified an unknown option */
	eHxTPE_TELNET_OPTION_SYNTAX ,   /** 49 - Malformed telnet option */
	eHxTPE_OBSOLETE50,              /** 50 - NOT USED */
	eHxTPE_PEER_FAILED_VERIFICATION, /** 51 - peer's certificate or fingerprint
									   wasn't verified fine */
	eHxTPE_GOT_NOTHING,             /** 52 - when this is a specific error */
	eHxTPE_SSL_ENGINE_NOTFOUND,     /** 53 - SSL crypto engine not found */
	eHxTPE_SSL_ENGINE_SETFAILED,    /** 54 - can not set SSL crypto engine as
									  default */
	eHxTPE_SEND_ERROR,              /** 55 - failed sending network data */
	eHxTPE_RECV_ERROR,              /** 56 - failure in receiving network data */
	eHxTPE_OBSOLETE57,              /** 57 - NOT IN USE */
	eHxTPE_SSL_CERTPROBLEM,         /** 58 - problem with the local certificate */
	eHxTPE_SSL_CIPHER,              /** 59 - couldn't use specified cipher */
	eHxTPE_SSL_CACERT,              /** 60 - problem with the CA cert (path?) */
	eHxTPE_BAD_CONTENT_ENCODING,    /** 61 - Unrecognized transfer encoding */
	eHxTPE_LDAP_INVALID_URL,        /** 62 - Invalid LDAP URL */
	eHxTPE_FILESIZE_EXCEEDED,       /** 63 - Maximum file size exceeded */
	eHxTPE_USE_SSL_FAILED,          /** 64 - Requested FTP SSL level failed */
	eHxTPE_SEND_FAIL_REWIND,        /** 65 - Sending the data requires a rewind
									  that failed */
	eHxTPE_SSL_ENGINE_INITFAILED,   /** 66 - failed to initialise ENGINE */
	eHxTPE_LOGIN_DENIED,            /** 67 - user, password or similar was not
									  accepted and we failed to login */
	eHxTPE_TFTP_NOTFOUND,           /** 68 - file not found on server */
	eHxTPE_TFTP_PERM,               /** 69 - permission problem on server */
	eHxTPE_REMOTE_DISK_FULL,        /** 70 - out of disk space on server */
	eHxTPE_TFTP_ILLEGAL,            /** 71 - Illegal TFTP operation */
	eHxTPE_TFTP_UNKNOWNID,          /** 72 - Unknown transfer ID */
	eHxTPE_REMOTE_FILE_EXISTS,      /** 73 - File already exists */
	eHxTPE_TFTP_NOSUCHUSER,         /** 74 - No such user */
	eHxTPE_CONV_FAILED,             /** 75 - conversion failed */
	eHxTPE_CONV_REQD,               /** 76 - caller must register conversion */
	eHxTPE_SSL_CACERT_BADFILE,      /** 77 - could not load CACERT file, missing
									  or wrong format */
	eHxTPE_REMOTE_FILE_NOT_FOUND,   /** 78 - remote file not found */
	eHxTPE_SSH,                     /** 79 - error from the SSH layer, somewhat
									  generic so the error message will be of
									  interest when this has happened */
	eHxTPE_SSL_SHUTDOWN_FAILED,     /** 80 - Failed to shut down the SSL
									  connection */
	eHxTPE_AGAIN,                   /** 81 - socket is not ready for send/recv,
									  wait till it's ready and try again */
	eHxTPE_SSL_CRL_BADFILE,         /** 82 - could not load CRL file, missing or
									  wrong format */
	eHxTPE_SSL_ISSUER_ERROR,        /** 83 - Issuer check failed.*/

	/* added by Humax */
	eHxTPE_PARAMETER_ERROR	= 100,	 /** 100 - your parameter is invaild or null */
	eHxTPE_FILE_OPEN_ERROR	= 101,	 /** 101 - when you use the HTTP GET function with the file
										path for storing the data, file open error */
	eHxTPE_NOT_SUPPORTED	= 102,	 /** 102 - please load the libcurl library */
	eHxTPE_HTTP_FORM_ERROR	= 103,	 /** 103 - */

	eHxTPE_LAST /* never use! */
} HxTRANSPORT_Error_e;



typedef enum {
	eHxTRANSPORT_FTP_EVENT_PROGRESS_CHANGED,
	eHxTRANSPORT_FTP_EVENT_COMPLETED,
	eHxTRANSPORT_FTP_EVENT_DOWNLOADSTOP,
	eHxTRANSPORT_FTP_EVENT_UNDEFINED
} HxTRANSPORT_Ftp_e;


/**
 * The encoded type of the certificate
 */
typedef enum {
	eHxTP_HTTP_CERTIFICATE_PEM		= 0,
	eHxTP_HTTP_CERTIFICATE_DER,
	eHxTP_HTTP_CERTIFICATE_P12,
} HxTRANSPORT_Certtype_e;

/**
 * The status code of the HTTP response from a server
 */
typedef enum  {
	eHxTP_HTTP_RESCODE_CONTINUE						=	100 ,
	eHxTP_HTTP_RESCODE_SWITCHING						=	101 ,
	eHxTP_HTTP_RESCODE_OK								=	200 ,
	eHxTP_HTTP_RESCODE_CREATED							=	201 ,
	eHxTP_HTTP_RESCODE_ACCEPTED						=	202 ,
	eHxTP_HTTP_RESCODE_NO_AUTH_INFO					=	203 ,
	eHxTP_HTTP_RESCODE_NO_CONTENT						=	204 ,
	eHxTP_HTTP_RESCODE_RESET_CONTENT					=	205 ,
	eHxTP_HTTP_RESCODE_PARTIAL_CONTENT					=	206 ,
	eHxTP_HTTP_RESCODE_MULTIPLE_CHOICE					=	300 ,
	eHxTP_HTTP_RESCODE_MOVED_PERMANENTLY				=	301 ,
	eHxTP_HTTP_RESCODE_FOUND							=	302 ,
	eHxTP_HTTP_RESCODE_SEE_OTHER						=	303 ,
	eHxTP_HTTP_RESCODE_NOT_MODIFIED					=	304 ,
	eHxTP_HTTP_RESCODE_USE_PROXY						=	305 ,
	eHxTP_HTTP_RESCODE_TEMP_REDIRECT					=	307 ,
	eHxTP_HTTP_RESCODE_BAD_REQUEST						=	400 ,
	eHxTP_HTTP_RESCODE_UNAUTHORIZED					=	401 ,
	eHxTP_HTTP_RESCODE_PAYMENT_REQUIRED				=	402 ,
	eHxTP_HTTP_RESCODE_FORBIDDEN						=	403 ,
	eHxTP_HTTP_RESCODE_NOT_FOUND						=	404 ,
	eHxTP_HTTP_RESCODE_METHOD_NOT_ALLOWED				=	405 ,
	eHxTP_HTTP_RESCODE_NOT_ACCEPTABLE					=	406 ,
	eHxTP_HTTP_RESCODE_PROXY_AUTH_REQUIRED				=	407 ,
	eHxTP_HTTP_RESCODE_REQUEST_TIMEOUT					=	408 ,
	eHxTP_HTTP_RESCODE_CONFILICT						=	409 ,
	eHxTP_HTTP_RESCODE_GONE							=	410 ,
	eHxTP_HTTP_RESCODE_LENGTH_REQUIRED					=	411 ,
	eHxTP_HTTP_RESCODE_PRECONDITION_FAILED				=	412 ,
	eHxTP_HTTP_RESCODE_REQUEST_ENTITY_TOO_LARGE		=   413 ,
	eHxTP_HTTP_RESCODE_REQUEST_URI_TOO_LARGE			=   414 ,
	eHxTP_HTTP_RESCODE_UNSUPPORTED_MEDIA_TYPE			=   415 ,
	eHxTP_HTTP_RESCODE_REQUESTED_RANGE_NOT_SATISFIABLE = 	416 ,
	eHxTP_HTTP_RESCODE_EXPECTATION_FAILED				=   417 ,
	eHxTP_HTTP_RESCODE_INTERNAL_SERVER_ERROR			=   500 ,
	eHxTP_HTTP_RESCODE_NOT_IMPLEMENTED					=   501 ,
	eHxTP_HTTP_RESCODE_BAD_GATEWAY						=   502 ,
	eHxTP_HTTP_RESCODE_SERVICE_UNAVAILABLE				=   503 ,
	eHxTP_HTTP_RESCODE_GATEWAY_TIMEOUT					=   504 ,
	eHxTP_HTTP_RESCODE_VERSION_NOT_SUPPORTED			=   505
} HxTRANSPORT_HttpResponse_e;

typedef enum {
	eHxTRANSPORT_EVENT_PROGRESS_CHANGED,
	eHxTRANSPORT_EVENT_COMPLETED,
	eHxTRANSPORT_EVENT_DOWNLOADSTOP,
	eHxTRANSPORT_EVENT_UNDEFINED
} HxTRANSPORT_e;

/*******************************************************************/
/********************     Typedef  			*************************/
/*******************************************************************/

#define _______________Typedef__________


#define HxTRANSPORT_RETURN(err) 	HLIB_TRANSPORT_return(err)

/**
 * Return the last error code of the transport module
 *
 * @return Return the last error code of the transport module
 */
#define HxTransport_GetLastError() HLIB_TRANSPORT_getlasterror()

/**
 * The FTP transport type
 */
typedef void HxTRANSPORT_Ftp_t;

/**
 * Listener callback
 */
typedef void (*HxFtp_callback)(HxTRANSPORT_Ftp_e event, HINT32 param1, HINT32 param2);


/**
 * The HTTP transport type
 */
typedef void HxTRANSPORT_Http_t;

/**
 * When getting a data using HTTP GET, you can register own callback function.
 */
typedef HINT32 HxHttpGet_Callback(void *ptr, HINT32 size, HINT32 nmemb, void *stream);

/**
 * The HTTP Header type
 */
typedef void HxTRANSPORT_Httpheader_t;

/**
 * The HTTP POST form type
 */
typedef void HxTRANSPORT_Httpform_t;


/**
 * Listener callback
 */
typedef void (*HxHttp_callback)(HxTRANSPORT_e event, HINT32 param1, HINT32 param2);


/*******************************************************************/
/********************     Public API 		*************************/
/*******************************************************************/

#define ______________PUBLIC_API___________


#ifdef __cplusplus
extern "C" {
#endif

HERROR HLIB_TRANSPORT_return(HINT32 nErrCode);
HxTRANSPORT_Error_e HLIB_TRANSPORT_getlasterror(void);

/**
 * Initialize the transport module for FTP(S)
 */
void HLIB_TRANSPORT_FtpInit(void);

/**
 * Destory the transport module for FTP(S)
 */
void HLIB_TRANSPORT_FtpDestory(void);

void HLIB_TRANSPORT_FtpSetListener(HxTRANSPORT_Ftp_t *handle, HxFtp_callback callback, void *hAct);

/**
 * Open the transport module for FTP(S)
 * This function will initialize the FTP trasnport module and return the handle.
 * Also this call MUST have a corresponding call to PAL_Transport_FtpClose() when
 * the operation is complete.
 *
 * @return the handle of the FTP transport module if success, error NULL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HxTRANSPORT_Ftp_t *HLIB_TRANSPORT_FtpOpen(void);

/**
 * Close the transport module for FTP(S)
 * This function will free all resources for the FTP transport module.
 *
 * @return none
 */
void HLIB_TRANSPORT_FtpClose(HxTRANSPORT_Ftp_t *ptFtp);

/**
 * Set the username and password.
 * Many protocols support that you specifiy the name and password in the URL itself.
 * (e.g. "ftp://user:password@www.humaxdigital.com")
 * If you want to set the username and password to the FTP transport module,
 * you can use this function as belows:
 * PAL_Transport_Ftp_SetUserPwd(pTp, "user:password");
 * PAL_Transport_Ftp_Download(pTp, "ftp://ftp.humaxdigital.com/temp/temp.bin",
 * "/usr/download/temp.bin", &downloadSize);
 *
 * @param[in] ptFtp the handle for the FTP transport module.
 * @param[in] szUserpwd the username and password to set the FTP transport module.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_FtpSetUserPwd(HxTRANSPORT_Ftp_t *ptFtp, const HINT8 *szUserpwd);


/**
 * Get the size of the file at the remoted FTP directory
 *
 * @param[in] ptFtp the handle for the FTP transport module.
 * @param[in] szUri the uri to access to the specific application.
 * (e.g. ftp://ftp.humaxdigital.com/temp/temp.bin)
 * If you'd like to use the FTP protocol, you have to add the "ftp://" as the prefix of URI.
 * @param[out] pflFileSize the size of file at the remoted FTP directory.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_FtpGetFileSize(HxTRANSPORT_Ftp_t *ptFtp, const HINT8 *szUri,
								HFLOAT64 *pflFileSize);
HERROR HLIB_TRANSPORT_FtpGetSize(HxTRANSPORT_Ftp_t *ptFtp, HFLOAT64 *pflFileSize);


/**
 * Download the data from the spcific server using the FTP(s) protocol
 *
 * @param[in] ptFtp the handle for the FTP transport module.
 * @param[in] szUri the uri to access to the specific application.
 * (e.g. ftp://ftp.humaxdigital.com/temp/temp.bin)
 * If you'd like to use the FTP protocol, you have to add the "ftp://" as the prefix of URI.
 * @param[in] szDownloadPath The name to store the file as if successful
 * @param[out] pflDownloadSize the size of downloaded content.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_FtpDownload(HxTRANSPORT_Ftp_t *ptFtp, const HINT8 *szUri,
								const HINT8 *szDownloadPath, HFLOAT64 *pflDownloadSize);
/**
 * Stop the dowonload
 * If you'd like to stop the download during FTP comunication, you can use this function.
 * The PAL_Transport_Ftp_Download will be returrned with ERR_FAIL as soon as calling this function.
 * Also this call MUST have a corresponding call to PAL_Transport_FtpClose() when
 * the operation is stopped.
 *
 * @param[in] ptFtp the handle for the FTP transport module.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_FtpDownloadStop(HxTRANSPORT_Ftp_t *ptFtp, HBOOL *bStop);

/**
 * Upload the data to the spcific server using the FTP(s) protocol
 *
 * @param[in] ptFtp the handle for the FTP transport module.
 * @param[in] szUri the uri to access to the specific path in the FTP server.
 * (e.g. ftp://ftp.humaxdigital.com/temp/temp.bin)
 * If you'd like to use the FTP protocol, you have to add the "ftp://" as the prefix of URI.
 * @param[in] szFilePath the file path for storing the data to the FTP server.
 * @param[out] pulUploadSize the size of uploaded content.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_FtpUpload(HxTRANSPORT_Ftp_t *ptFtp, const HINT8 *szUri,
								const HINT8 *szFilePath, HUINT32 *pulUploadSize);

/**
 * Set the maximum time in seconds that you allow the FTP transfer operation to take.
 *
 * @param[in] ptFtp the handle for the FTP transport module.
 * @param[in] ulTimeoutSec the maximum time in seconds
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_FtpSetTimeout(HxTRANSPORT_Ftp_t *ptFtp, HUINT32 ulTimeoutSec);

/**
 * Get the OTA data size
 *
 * @param[out] pSize the downloaded file size for ota data.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_FtpDownloadedSize(HxTRANSPORT_Ftp_t *ptFtp, HFLOAT64*pSize);


/**
 * Initialize the transport module for HTTP(S)
 */
void HLIB_TRANSPORT_HttpInit(void);

/**
 * Destory the transport module for HTTP(S)
 */
void HLIB_TRANSPORT_HttpDestory(void);

/**
 * Callback
 */
void HLIB_TRANSPORT_HttpSetListener(HxTRANSPORT_Http_t *handle, HxHttp_callback callback, void *hAct);

/**
 * Open the transport module for HTTP(S)
 * This function will initialize the HTTP trasnport module and return the handle.
 * Also this call MUST have a corresponding call to PAL_Transport_HttpClose() when
 * the operation is complete.
 *
 * @return the handle of the HTTP transport module if success, error NULL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HxTRANSPORT_Http_t *HLIB_TRANSPORT_HttpOpen(void);

/**
 * Close the transport module for HTTP(S)
 * This function will free all resources for the HTTP transport module.
 *
 * @return none
 */
void HLIB_TRANSPORT_HttpClose(HxTRANSPORT_Http_t *ptHttp);

/**
 * Set the username and password.
 * Many protocols support that you specifiy the name and password in the URL itself.
 * (e.g. "http://user:password@www.humaxdigital.com")
 * If you want to set the username and password to the HTTP transport module,
 * you can use this function as belows:
 * PAL_Transport_Http_SetUserPwd(pTp, "user:password");
 * PAL_Transport_Http_Get(pTp, "http://www.humaxdigital.com", pBuf, &downloadSize);
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] szUserpwd the username and password to set the HTTP transport module.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpSetUserPwd(HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUserpwd);

/**
 * Set the cilent certificate file for SSL.
 * If your certificate has a password, you have to set the password to the szPassPharse.
 * the certType is "PEM" type as default but you can change it to another type.
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] szCertFile the path of the file encoded by some certificate type.
 * @param[in] certType the encoded type of the certificate.
 * @param[in] szPassPharse if the certificate is encrypted, you set the pass pharse.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpSetCertificateFile(HxTRANSPORT_Http_t *ptHttp,
							const HINT8 *szCertFile, HxTRANSPORT_Certtype_e certType,
							const HINT8 *szPassPharse);

/**
 * Set the private key for your certificate.
 * If the key is not combined with the client certificate, you have to set it to
 * the tranport module.
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] szCertKeyFile the path of the key file encoded by some certificate type.
 * @param[in] certType the encoded type of the certificate.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpSetCertificateKeyFile(HxTRANSPORT_Http_t *ptHttp,
							const HINT8 *szCertKeyFile, HxTRANSPORT_Certtype_e certType);

/**
 * Set the certificate authority for SSL.
 * Set the CA certificate verification path where to find CA certificates
 * for server authentication.
 * This file must be PEM encoded.
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] szCACertFile the path of the file encoded by some certificate type.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpSetCACertificateFile(HxTRANSPORT_Http_t *ptHttp,
							const HINT8 *szCACertFile);

/**
 * Set the peer verification.
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] T/F for verfication..
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */

HERROR HLIB_TRANSPORT_HttpSetPeerVeryfication(HxTRANSPORT_Http_t *ptHttp,
							HBOOL bVerify);


/**
 * Set the host verification.
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] T/F for verfication..
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */

HERROR HLIB_TRANSPORT_HttpSetHostVeryfication(HxTRANSPORT_Http_t *ptHttp,
							HBOOL bVerify);

/**
 * Append the the field to the HTTP header.
 * User can be build a llst of own headers using this API.
 * For example, if you add the "Content-Type: text/xml" to your header,
 * you can use like this, "PAL_Transport_Http_AppendHeader(pHttpHeader, "Content-Type: text/xml");".
 * Also you MUST free a the list of HTTP header with PAL_Transport_Http_FreeHeader() when
 * the list is not used any more.
 *
 * @param[in] ptHttpHeader the handle for the Http header.
 * @param[in] szHeader own header string to append the http header.
 *
 * @return the handle for HTTP header if success, error NULL.
 */
HxTRANSPORT_Httpheader_t *HLIB_TRANSPORT_HttpAppendHeader(
								HxTRANSPORT_Httpheader_t *ptHttpHeader,
								const HINT8 *szHeader);

/**
 * Free the the header for HTTP(s)
 * This function removes all list of HTTP header created by PAL_Transport_Http_AppendHeader().
 *
 * @param[in] ptHttpHeader the handle for the HTTP header.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpFreeHeader(HxTRANSPORT_Httpheader_t *ptHttpHeader);

/**
 * Set the HTTP header by created user to the HTTP transport module.
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] ptHttpHeader the handle for the HTTP header.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpSetHeaderToTP(HxTRANSPORT_Http_t *ptHttp,
								HxTRANSPORT_Httpheader_t *ptHttpHeader);

/**
 * Get the data from the spcific server using the HTTP(s) protocol
 * The ppcBuf will be allocated internally and filled by the HTTP transport module.
 * Therefore The ppcBuf have to be freed by user with PAL_Transport_Http_FreeBuffer()
 * after finishing to use the buffer as strdup.
 * This method does not support multi thread.
 * If you want to support multi thread, you need to use PAL_Transport_Http_GetWithCallback() method.
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] szUri the uri to access to the specific application.
 * (e.g. http://www.humaxdigital.com)
 * @param[out] ppcBuf the reference of the pointer is used to the buffer.
 * @param[out] pflDownloadSize the size of downloaded content.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpGet(HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri,
								HINT8 **ppcBuf, HFLOAT64 *pflDownloadSize);

/**
 * Free the buffer.
 * This function is used to free a buffer previously created with PAL_Transport_Http_Get();
 *
 * @param[in] pBuf the pointer of buffer for receiving a data on HTTP session.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
void HLIB_TRANSPORT_HttpFreeBuffer(HINT8 *pBuf);

/**
 * Get the data from the spcific server using the HTTP(s) protocol
 * the callback gets called by the HTTP transport module as soon as there is data
 * received that needs to be saved. the ptr includes the data and size is the amount of
 * data. User can be check the data in the callback function and this function can be called
 * often by receiving the data.
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] szUri the uri to access to the specific application.
 * (e.g. http://www.humaxdigital.com)
 * @param[in] callback the callback function pointer for HTTP GET.
 * @param[out] pflDownloadSize the size of downloaded content.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpGetWithCallback(HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri,
								HxHttpGet_Callback *callback, HFLOAT64 *pflDownloadSize);

/**
 * Get the data from the spcific server using the HTTP(s) protocol
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] szUri the uri to access to the specific application.
 * (e.g. http://www.humaxdigital.com)
 * @param[out] ppcHeader the reference of the pointer is used to the header buffer.
 * @param[out] ppcBuf the reference of the pointer is used to the body buffer.
 * @param[out] pflDownloadSize the size of downloaded content.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpGetHeaderAndBody (HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri
								, HINT8 **ppcHeader, HUINT32 ulMaxHeaderSize
								, HINT8 **ppcBuf, HUINT32 ulMaxBufSize
								, HFLOAT64 *pflDownloadSize);

/**
 * Get the data from the spcific server using the HTTP(s) protocol and store to local file.
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] szUri the uri to access to the specific application.
 * (e.g. http://www.humaxdigital.com)
 * @param[in] szFilepath the file path for storing the data from the HTTP server.
 * @param[out] pflDownloadSize the size of downloaded content.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpGetAndStore(HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri,
								const HINT8 *szDownlaodPath, HFLOAT64 *pflDownloadSize);

/**
 * Stop the dowonload
 * If you'd like to stop the download during HTTP comunication, you can use this function.
 * The PAL_Transport_Http_Download will be returrned with ERR_FAIL as soon as calling this function.
 * Also this call MUST have a corresponding call to PAL_Transport_HttpClose() when
 * the operation is stopped.
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpDownloadStop(HxTRANSPORT_Http_t *ptHttp, HBOOL *bStop);

/**
 * Put the data to the spcific server using the HTTP(s) protocol
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] szUri the uri to access to the specific application.
 * (e.g. http://www.humaxdigital.com)
 * @param[in] szFilepath the local file path for storing the data to the HTTP server.
 * @param[out] pulUploadSize the size of uploaded content.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpPut(HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri,
								const HINT8 *szFilepath, HFLOAT64 *pflUploadSize);

/**
 * Append the form data for file.
 * This function is used to append section when building a multipart/formdata HTTP POST
 *
 * @param[in] ptFirst the first handle for the form data.
 * @param[in] ptLast the last handle for the form data.
 * @param[in] copyName the field name.
 * @param[in] szFilePath the local file path for posting the data to the HTTP server.
 *
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_AddFormforFile(HxTRANSPORT_Httpform_t **ptFirst,
								HxTRANSPORT_Httpform_t **ptLast,
								const HINT8 *copyName, const HINT8 *szFilePath);

/**
 * Append the form data for contents.
 * This function is used to append section when building a multipart/formdata HTTP POST
 *
 * @param[in] ptFirst the first handle for the form data.
 * @param[in] ptLast the last handle for the form data.
 * @param[in] copyName the field name.
 * @param[in] ptrContents the buffer for posting the data to the HTTP server.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpAddFormforContents(HxTRANSPORT_Httpform_t **ptFirst,
								HxTRANSPORT_Httpform_t **ptLast, const HINT8 *copyName, const HINT8 *ptrContents);

/**
 * Free the form data.
 * This function is used to clean up data previously built with PAL_Transport_Http_AddForm().
 * And it must be called when the data has been used.
 *
 * @param[in] ptForm the handle for the form data.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpFreeForm(HxTRANSPORT_Httpform_t *ptForm);

/**
 * Set the form data created by user to the HTTP transport module.
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] ptHttpform the handle for the HTTP from.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpSetPostformToTP(HxTRANSPORT_Http_t *ptHttp,
								HxTRANSPORT_Httpform_t *ptHttpform);
/**
 * Post the data to the spcific server using the HTTP(s) protocol
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] szUri the uri to access to the specific application.
 * (e.g. http://www.humaxdigital.com)
 * @param[out] ppcHeader the reference of the pointer is used to the header buffer.
  *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code
 */
HERROR HLIB_TRANSPORT_HttpHead (HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri
															, HINT8 **ppcHeader, HUINT32 ulMaxHeaderSize);

/**
 * Post the data to the spcific server using the HTTP(s) protocol
 *
 * @param[in] ptHttp is the handle for the HTTP transport module.
 * @param[in] szUri is the uri to access to the specific application.
 * (e.g. http://www.humaxdigital.com)
 * @param[in] szData is the data to add using HTTP POST Method.
 * @param[in] ulDataSize is the data about size of szData.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpPost(HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri,
			const HINT8 *szData, HUINT32 ulDataSize);

/**
 * Post the data to the spcific server using the HTTP(s) protocol
 * The ppcBuf will be allocated internally and filled by the HTTP transport module.
 * Therefore The ppcBuf have to be freed by user with PAL_Transport_Http_FreeBuffer()
 * after finishing to use the buffer as strdup.
 *
 * @param[in] ptHttp is the handle for the HTTP transport module.
 * @param[in] szUri is the uri to access to the specific application.
 * (e.g. http://www.humaxdigital.com)
 * @param[in] szData is the data to add using HTTP POST Method.
 * @param[in] ulDataSize is the size of szData(POSTFIELDS).
 * @param[out] ppcBuf is the reference of the pointer is used to the buffer.
 * @param[out] pflDownloadSize is the size of downloaded content.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpPostAndRecvResponse(HxTRANSPORT_Http_t *ptHttp,
			const HINT8 *szUri, const HINT8 *szData, HUINT32 ulDataSize,
			HINT8 **ppcBuf, HFLOAT64 *pflDownloadSize);

/**
 * Get the HTTP response code.
 * After getting from a server or posting to a server, you can check the status of HTTP.
 * Regarding the status code of HTTP response, please refer to PAL_Transport_HttpResponse_e.
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[out] peResCode the status code of HTTP response from a server
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpGetHttpResponseCode(HxTRANSPORT_Http_t *ptHttp,
								HxTRANSPORT_HttpResponse_e *peResCode);

/**
 * Set the maximum time in seconds that you allow the HTTP transfer operation to take.
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] ulTimeoutSec the maximum time in seconds
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpSetTimeout(HxTRANSPORT_Http_t *ptHttp, HUINT32 ulTimeoutSec);

/**
 * Get the Downloaded data size
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[out] pSize the downloaded file size for data.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpDownloadedSize(HxTRANSPORT_Http_t *ptHttp, HFLOAT64* pSize);

/**
 * Get the size of the file at the remoted HTTP directory
 *
 * @param[in] ptHttp the handle for the HTTP transport module.
 * @param[in] szUri the uri to access to the specific application.
 * (e.g. http://http.humaxdigital.com/temp/temp.bin)
 * If you'd like to use the HTTP protocol, you have to add the "http://" as the prefix of URI.
 * @param[out] pflFileSize the size of file at the remoted HTTP directory.
 *
 * @return ERR_OK if success, error ERR_FAIL.
 * if you'd like to check the detailed error code, please use the PAL_Transport_GetLastError().
 */
HERROR HLIB_TRANSPORT_HttpHttpGetFileSize(HxTRANSPORT_Http_t *ptHttp,
								HFLOAT64 *pflFileSize);



HERROR HLIB_TRANSPORT_HttpSetUrl(HxTRANSPORT_Http_t *ptHttp, const HINT8 *szUri);
#ifdef __cplusplus
}
#endif






#endif


