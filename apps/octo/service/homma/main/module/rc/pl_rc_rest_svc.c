/*************************************************************************************************************
	File            : pl_rc_rest_svc.h
	author          : yjlim@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED 징째AS IS징짹 AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/
#define	________________Header_Files_______________________________

#include "vkernel.h"
#include "pl_rc_rest_svc.h"
#include "rc_data_ses.h"

#define ________________Typedef____________________________________
#define MAX_REQUEST_SIZE 4096
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define	__TEST_WITH_POSTMAN__		0

#define	RC_RESOURCE_MAX_NUM				32
#define	RC_RESOURCE_PATH_MAX_LEN		32
#define	RC_RESOURCE_DEPTH_MAX_LEN		32
#define	RC_RESOURCE_MAX_BUF_SIZE		(8192)
#define	RC_RESOURCE_SHORT_MAX_BUF_SIZE	(2048)
#define RC_MAX_PATH_LEN					256

#define	RC_PACKET_MAX_BUF_SIZE				(8192*4)

#define	SRC_INFO_MAX_SIZE		512


#define RC_REST_SVC_EVENTS				"/rc/events"
#define RC_REST_SVC_REMOTE				"/rc/remote"
#define RC_REST_SVC_OUTPUTS				"/rc/outputs"
#define RC_REST_SVC_LOCALE				"/rc/locale"
#define RC_REST_SVC_POWER				"/rc/power"
#define RC_REST_SVC_TIME				"/rc/time"

#define RC_REST_SVC_SOURCELISTS			"/rc/sourcelists"
#define RC_REST_SVC_SOURCES				"/rc/sources"
#define RC_REST_SVC_APPS				"/rc/apps"
#define RC_REST_SVC_CA					"/rc/ca"




#define RC_CURRENT_PAIRED_CLIENT_ID				"PairedClientId"
#define RC_CURRENT_PAIRED_CLIENT_NAME			"PairedClientName"
#define RC_CURRENT_PAIRED_CLIENT_AUTH_TOKEN		"PairedClientAuthToken"

#define	RC_DB_POWERSAVING_ON_STANDBY			"PowerSavingOnStandby"

#define RC_SERVER_NAME							"RCServerName"


#define RC_SSDP_PORT 			(58801)
#define RC_PORT 				(58802)
#define RC_SSL_PORT 			(58803)

#define RC_PEMFILE_ROOT			"/usr/rc/"
#define RC_PEMFILE_NAME			RC_PEMFILE_ROOT"rc-servcert.pem"
#define RC_CAFILE_NAME			RC_PEMFILE_ROOT"rc-rootca.pem"

#if 1		// 최초 SW Release 에서 Pairing 관련 항목만 확인하기 위해서 feature disable 처리함
#define RC_REST_SVC_CHECK_AUTH_HEADER
#endif

typedef HERROR(*RC_OnGet_Response)(struct mg_connection *conn, void *param1, void *param2, void *param3);
typedef HERROR(*RC_OnPost_Action)(struct mg_connection *conn, void *param1, void *param2, void *param3);
typedef HERROR(*RC_OnReply_Send)(struct mg_connection *conn, void *param1, void *param2, void *param3);


typedef struct
{
	HCHAR		szClientID[RC_MAX_PATH_LEN];
	HCHAR		szClientName[RC_MAX_PATH_LEN];
	HCHAR		szAuthToken[RC_MAX_PATH_LEN];
	HBOOL		bPaired;
} rc_rest_service_client_info_t;

typedef struct
{
	HCHAR 					szRootPath[RC_RESOURCE_PATH_MAX_LEN];  // ex: /rc/locale, /rc/remote, etc..
	HUINT16					usRootPathLen;
	RC_OnGet_Response		OnGet_Response;
	RC_OnPost_Action		OnPost_Action;
	RC_OnReply_Send			OnReply_Send;
} rc_rest_service_desc_t;

typedef struct
{
	HULONG	lock;
	struct mg_server *server1;
	struct mg_server *server2;
	struct mg_server *server3;

	HUINT32 	ulServerTaskId1;
	HUINT32 	ulServerTaskId2;
	HUINT32 	ulServerTaskId3;

	HxList_t	*pRestSvcList;
	HCHAR		pszFriendlyName[RC_MAX_PATH_LEN];

	struct RCClientCallbacks client_cb;

	rc_rest_service_client_info_t	*pstCurrentClientInfo;

	HBOOL		bReply;
	void		*pReplyData;
	HINT32		nReplyInt;

	HBOOL		bOperating;
	HBOOL		bEnableActiveStandby;
	HUINT32		ulStandbyTime;
} rc_rest_service_context_t;

typedef enum
{
	eRC_Module_Remote,
	eRC_Module_Events,
	eRC_Module_Outputs,
	eRC_Module_Sourcelists,
	eRC_Module_Sources,
	eRC_Module_Apps,
	eRC_Module_CA,
	eRC_Module_Power,
	eRC_Module_Locale,
	eRC_Module_Time,
	eRC_Module_Max
} RC_Module_e;

typedef enum
{
	eRC_Header_Type1,					// normal response
	eRC_Header_Type2,					// simple response
	eRC_Header_With_AuthToken,			// Response header에 Auth token을 생성하여 포함시킴
	eRC_Header_Req_AuthToken,			// Request header에 Auth token이 없을 경우
	eRC_Header_Max
} RC_HeaderType_e;


typedef enum
{
	eRC_NO_PAIRING,		// Not exist paired client
	eRC_PAIRED,			// Exist paired client, identified client
	eRC_UNPAIRED		// Exist paired client, not identified client
} RC_Pairing_Status_e;


RC_Pairing_Status_e __rc_restsvc_is_paired_client(struct mg_connection *conn);


#define ________________Static_Variables___________________________
static rc_rest_service_context_t	s_pst_context;
HxList_t *s_list_rest_service;

#define ________________Internal_Functions_________________________
static rc_rest_service_context_t *__context(void)
{
	return &s_pst_context;
}

static void __lock(void)
{
	rc_rest_service_context_t *ctx = __context();
	VK_SEM_Get(ctx->lock);
}

static void __unlock(void)
{
	rc_rest_service_context_t *ctx = __context();
	VK_SEM_Release(ctx->lock);
}

void __rc_restsvc_memfree(void *mem)
{
	if(mem) HLIB_STD_MemFree(mem);
}

void __rc_restsvc_server(void *server)
{
	while(1)
	{
		mg_poll_server((struct mg_server *) server, 1000);
	}
}

HERROR __rc_restsvc_check_auth_header(struct mg_connection *conn)
{
#if defined(RC_REST_SVC_CHECK_AUTH_HEADER)

	HCHAR		*pszAuth = NULL;
	rc_rest_service_context_t *ctx = __context();
	rc_rest_service_client_info_t	*pstClientInfo = ctx->pstCurrentClientInfo;

	pszAuth = (HCHAR*)mg_get_header(conn, "Authorization");
	if(pszAuth == NULL)
	{
		return ERR_FAIL;
	}

/*
	if((pstClientInfo->bPaired == TRUE) && (__rc_restsvc_is_paired_client(conn) == eRC_PAIRED))
	{
		return ERR_OK;
	}
	else
	{
		return ERR_FAIL;
	}
*/
	return ERR_OK;

#else
	return ERR_OK;
#endif
}

HERROR __rc_restsvc_make_authorization_token(struct mg_connection *conn, HCHAR *pszToken)
{
	HCHAR *pszFrom = NULL, *pszDest = NULL;
	HCHAR szAuthToken[RC_MAX_PATH_LEN] = {0,};
	PL_COMMON_NetworkInfo_t net_info;

#if defined(RC_REST_SVC_CHECK_AUTH_HEADER)
	// Make Authorization token
	//  - The token shall be associated with the Client ID as defined above.
	//  - The token is BASE64 encoded by the RECEIVER.
	//  - The RECEIVER shall use the Client ID as the primary key for the APP
	//     in terms of "Simple authorization", "Pairing" and "Parental control".

	pszFrom = (HCHAR*)mg_get_header(conn, "From");		// From 헤더의 내용이 Client ID
	if(pszFrom == NULL)
	{
		return ERR_FAIL;
	}

	PL_COMMON_GetDefaultNetwork(&net_info);

	HxSTD_MemSet(szAuthToken, 0, RC_MAX_PATH_LEN);
	HLIB_CRYPT_DES_EncryptData(net_info.szMacAddr, pszFrom, szAuthToken,  HxSTD_StrLen(pszFrom));

	pszDest = HLIB_MATH_EncodeBase64Easy(szAuthToken, HxSTD_StrLen(szAuthToken));
	if(pszDest == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_MemSet(szAuthToken, 0, RC_MAX_PATH_LEN);
	HxSTD_MemCopy(szAuthToken, pszDest, sizeof(szAuthToken));
	HxSTD_snprintf(pszToken, RC_MAX_PATH_LEN, "OPAL-RC-TOKEN %s", szAuthToken);
	pszToken[RC_MAX_PATH_LEN - 1] = '\0';
#else
	// Send fixed dummy token..
	HxSTD_snprintf(pszToken, RC_MAX_PATH_LEN, "OPAL-RC-TOKEN OPAL-RC-TOKEN QWxhZGRpbjpvcGVuIHNlc2FtZQ==");
	pszToken[RC_MAX_PATH_LEN - 1] = '\0';
#endif

	HxLOG_Debug("\t\t+++++ [%s][%d], Auth Token : %s\n", __FUNCTION__, __LINE__, pszToken);

	if(pszDest)
	{
		HLIB_MATH_FreeBase64(pszDest);
	}

	return ERR_OK;
}

void __rc_restsvc_get_client_name(struct mg_connection *conn, HCHAR *pszClientName)
{
	HCHAR *pszUserAgent = NULL;
#if __TEST_WITH_POSTMAN__
	HCHAR pszTemp[] = "OPAL-RC/1.0 OperatorApp/3.2 iPhone4/6.1.3 (apple; ios7; Helen's Favourite Tablet) ";
#endif

	HCHAR *pClient[RC_MAX_PATH_LEN] = {0, };
	HCHAR *pEnd = NULL;

	// <User-Agent> Header
	// User-Agent: OPAL-RC/{opal-version} {app}/{app-version} {device}/{device-version} ({device-info};{client-name})
	// ex) User-Agent: OPAL-RC/1.0 OperatorApp/3.2 iPhone4/6.1.3 (apple; ios7; Helen's Favourite Table)

#if __TEST_WITH_POSTMAN__
	pszUserAgent = pszTemp;
#else
	pszUserAgent = (HCHAR*)mg_get_header(conn, "User-Agent");
#endif

	if(pszUserAgent != NULL)
	{
		pszUserAgent = HxSTD_StrRChar(pszUserAgent,';');

		if(pszUserAgent != NULL)
		{
			pszUserAgent++;

			HxSTD_snprintf(pClient, RC_MAX_PATH_LEN, "%s", pszUserAgent);
			pEnd = HxSTD_StrRChar(pClient,')');
			if(pEnd != NULL)
			{
				pEnd[0] = '\0';
			}

			HxLOG_Debug("\t\t+++++ [%d], Client : %s\n", __LINE__, pClient);
			HxSTD_MemSet(pszClientName, 0x0, RC_MAX_PATH_LEN);
			HxSTD_MemCopy(pszClientName, pClient, HxSTD_StrLen(pClient));
		}
	}
}

HERROR __rc_restsvc_pair_client_info(struct mg_connection *conn)
{
	HERROR hErr = ERR_FAIL;

	rc_rest_service_context_t *ctx = __context();
	rc_rest_service_client_info_t	*pstClientInfo = ctx->pstCurrentClientInfo;

	HCHAR szTemp[RC_MAX_PATH_LEN] = {0,};
	HCHAR *pszFrom = NULL;
	HCHAR *pszAuthToken = NULL;

	HxLOG_Debug("\t\t+++++ [%s][%d]\n", __FUNCTION__, __LINE__);

	// 1. current pairing 상태를 보고 이미 다른 device가 pairing 상태라면
	//   ---> 같은 device가 pairing 상태면 202 Accepted
	//   ---> 다른 device가 pairing 상태면 423 Locked
	// 2. 이미 pairing된 다른 device가 없다면 client info에 auth token, client ID, client Name 메모리에 저장 (registry 에도 저장)

	pszFrom = (HCHAR*)mg_get_header(conn, "From");
	pszAuthToken = (HCHAR*)mg_get_header(conn, "Authorization");

	if(pstClientInfo->bPaired == TRUE)
	{
		if(__rc_restsvc_is_paired_client(conn) == eRC_PAIRED)
		{
			HxLOG_Debug("\t\t+++++ [%s][%d] Same device already paired!!\n", __FUNCTION__, __LINE__);
			hErr = ERR_OK;			// 202 Accepted
		}
		else
		{
			HxLOG_Debug("\t\t+++++ [%s][%d] Another device already paired!!  [clientID : %s]\n", __FUNCTION__, __LINE__, pstClientInfo->szClientID);
			hErr = ERR_FAIL;			// 423 Locked
		}
	}
	else
	{
		HxSTD_MemCopy(pstClientInfo->szClientID, pszFrom, RC_MAX_PATH_LEN);			// "From" header parsing 후 저장
		HxSTD_MemCopy(pstClientInfo->szAuthToken, pszAuthToken, RC_MAX_PATH_LEN);			// "Authorization" header parsing 후 저장
		__rc_restsvc_get_client_name(conn, pstClientInfo->szClientName);		// "User-Agent" header parsing 후 저장
		pstClientInfo->bPaired = TRUE;

		HxLOG_Debug("pstClientInfo->szClientName : %s\n", pstClientInfo->szClientName);

		hErr = HAPI_GetRegistryStr(RC_CURRENT_PAIRED_CLIENT_ID, szTemp, sizeof(szTemp));
		if( (hErr != ERR_OK) /* 최초 생성 */ || (hErr == ERR_OK && HLIB_STD_StrCaseCmp(szTemp, "") == 0) )
		{
			HxLOG_Debug("\t\t+++++ [%s][%d] Try to pairing on STB!!\n", __FUNCTION__, __LINE__);
			HAPI_SetRegistryStr(RC_CURRENT_PAIRED_CLIENT_ID, pstClientInfo->szClientID /* From Header -> clientID */);
			HAPI_SetRegistryStr(RC_CURRENT_PAIRED_CLIENT_NAME, pstClientInfo->szClientName /* User-Agent Header -> clientName */);
			HAPI_SetRegistryStr(RC_CURRENT_PAIRED_CLIENT_AUTH_TOKEN, pstClientInfo->szAuthToken/* Authorization Header -> auth token */);
			hErr = ERR_OK;
		}
		else
		{
			HxLOG_Debug("\t\t+++++ [%s][%d] Somthing wrong!!\n", __FUNCTION__, __LINE__);
			hErr = ERR_FAIL;
		}
	}

	if(hErr == ERR_OK)
	{
		// Current paired device의 status를 update 하기 위해서 webapp 으로 message를 보내야 한다.
	}

	return hErr;
}

HERROR __rc_restsvc_unpair_client_info(struct mg_connection *conn, HBOOL bForceUnpair)
{
	HERROR hErr = ERR_FAIL;

	rc_rest_service_context_t *ctx = __context();
	rc_rest_service_client_info_t	*pstClientInfo = ctx->pstCurrentClientInfo;

	HCHAR szTemp[RC_MAX_PATH_LEN] = {0,};

	HxLOG_Debug("\t\t+++++ [%s][%d]\n", __FUNCTION__, __LINE__);

	// 1. Current ClientID, ClientName을 읽어보고 이미 있는 값이면 unpairing -> DB 값 삭제

	if((pstClientInfo->bPaired == TRUE) && ((TRUE == bForceUnpair) || (__rc_restsvc_is_paired_client(conn) == eRC_PAIRED)))
	{
	 	HxSTD_MemSet(pstClientInfo->szClientID, 0x00, RC_MAX_PATH_LEN);
		HxSTD_MemSet(pstClientInfo->szClientName, 0x00, RC_MAX_PATH_LEN);
		HxSTD_MemSet(pstClientInfo->szAuthToken, 0x00, RC_MAX_PATH_LEN);
		pstClientInfo->bPaired = FALSE;

		hErr = HAPI_GetRegistryStr(RC_CURRENT_PAIRED_CLIENT_ID, szTemp, sizeof(szTemp));
		if(hErr == ERR_OK)
		{
			HxLOG_Debug("\t\t+++++ [%s][%d]\n", __FUNCTION__, __LINE__);
			HAPI_SetRegistryStr(RC_CURRENT_PAIRED_CLIENT_ID, NULL);
			HAPI_SetRegistryStr(RC_CURRENT_PAIRED_CLIENT_NAME, NULL);
			HAPI_SetRegistryStr(RC_CURRENT_PAIRED_CLIENT_AUTH_TOKEN, NULL);
			hErr = ERR_OK;
		}
		else
		{
			HxLOG_Debug("\t\t+++++ [%s][%d] Something wrong!!\n", __FUNCTION__, __LINE__);
			hErr = ERR_FAIL;
		}
	}
	else
	{
		// 이전에 pairing 된 device가 없는 상태에서 unpair command가 날라온 경우)
		// TODO: ERROR CODE를 어떻게 처리해야 할지..
		HxLOG_Debug("\t\t+++++ [%s][%d] Unpaired or Client isn't pairing client..!!\n", __FUNCTION__, __LINE__);
		hErr = ERR_FAIL;
	}

	return hErr;
}

void __rc_restsvc_send_response(struct mg_connection *conn, RC_HeaderType_e eType, HUINT32 ulErrorCode, void *pBody, HUINT32 ulBodyLen)
{
	HERROR hErr = ERR_FAIL;

	HUINT8 szBodyLen[10] = {0,};
	HCHAR szAuthToken[RC_MAX_PATH_LEN] = {0,};

	HxSTD_snprintf(szBodyLen, sizeof(szBodyLen), "%d", ulBodyLen);
	HxLOG_Debug("\t\t+++++ content length : %d (%s)\n", ulBodyLen, szBodyLen);
	if(ulBodyLen > 0 && pBody != NULL)
	{
		HxLOG_Debug("\t\t+++++ body :\n%s\n", (HCHAR*)pBody);
	}

	// send status
	mg_send_status(conn, ulErrorCode);

	// send header
	if(eType == eRC_Header_Type1)
	{
		mg_send_header(conn, "Content-Language", "en-gb");
		mg_send_header(conn, "Content-Length", szBodyLen);
		mg_send_header(conn, "Content-Type", "text/xml");
		mg_send_header(conn, "Server", "POSIX UPnP/1.1 OPAL/1.0");		// {server}/{server-version} {more} OPAL-RC/{opal-version}
		mg_send_header(conn, "Connection", "close");
		mg_write(conn, "\r\n", 2);		// terminate_headers
	}
	else if(eType == eRC_Header_Type2)
	{
		mg_send_header(conn, "Content-Length", szBodyLen);		// have to check (not included in OPAL RC specification)
		mg_send_header(conn, "Server", "POSIX UPnP/1.1 OPAL/1.0");
		mg_send_header(conn, "Connection", "close");
		mg_write(conn, "\r\n", 2);		// terminate_headers
	}
	else if(eType == eRC_Header_With_AuthToken)
	{
		hErr = __rc_restsvc_make_authorization_token(conn, szAuthToken);
		if(hErr == ERR_OK)
		{
			mg_send_header(conn, "Authorization", szAuthToken);
		}

		mg_send_header(conn, "Content-Language", "en-gb");
		mg_send_header(conn, "Content-Length", szBodyLen);
		mg_send_header(conn, "Content-Type", "text/xml");
		mg_send_header(conn, "Server", "POSIX UPnP/1.1 OPAL/1.0");		// {server}/{server-version} {more} OPAL-RC/{opal-version}
		mg_send_header(conn, "Connection", "close");
		mg_write(conn, "\r\n", 2);		// terminate_headers
	}
	else if(eType == eRC_Header_Req_AuthToken)
	{
		mg_send_header(conn, "Content-Length", szBodyLen);		// have to check (not included in OPAL RC specification)
		mg_send_header(conn, "Server", "POSIX UPnP/1.1 OPAL/1.0");
		mg_send_header(conn, "WWW-Authenticate", "OPAL-RC-TOKEN");
		mg_send_header(conn, "Connection", "close");
		mg_write(conn, "\r\n", 2);		// terminate_headers
	}
	else
	{

	}

	// send body
	if(ulBodyLen > 0)
	{
//		mg_send_data(conn, (void*)pBody, ulBodyLen);
		mg_write(conn, (const void*)pBody, ulBodyLen);
	}
	else
	{
		if(pBody)
		{
			HxLOG_Error("[__rc_restsvc_send_response] pBody : %d Null Action\n", pBody);
		}
	}
}

// Device Description
#define	__RC_DEFAULT_UDN_PREFIX			"1E96FF1A-6771-2F44-H0C5-"

void __rc_get_DeviceDescription(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
#if 0
	HERROR hErr = ERR_FAIL;
#endif
	PL_COMMON_ProductInfo_t product_info;
	PL_COMMON_NetworkInfo_t net_info;

	HCHAR			szUdn[48] = {0, };
	HCHAR			szHttpUrl[256] = {0, };
	HCHAR			szHttpsUrl[256] = {0, };

	rc_rest_service_context_t *ctx = __context();
	rc_rest_service_client_info_t *pstClientInfo = ctx->pstCurrentClientInfo;

	int 	 		written_len = 0;
	HUINT32 	ulLen = RC_RESOURCE_SHORT_MAX_BUF_SIZE;		// 2048
	static char * form_begin =  		"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"\
										"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\r\n"\
										"<specVersion>\r\n"\
										"<major>1</major>\r\n"\
										"<minor>1</minor>\r\n"\
										"</specVersion>\r\n";

	static char * form_device =  		"<device>\r\n"\
										"<deviceType>urn:humax:device:rcserver:1</deviceType>\r\n"\
										"<friendlyName>%s</friendlyName>\r\n"\
										"<manufacturer>%s</manufacturer>\r\n"\
										"<modelName>%s</modelName>\r\n"\
										"<modelNumber>%s</modelNumber>\r\n"\
										"<UDN>uuid:%s</UDN>\r\n";

	static char * form_icon_start =		"<iconList>\r\n";

	static char * form_icon =			"<icon>\r\n"\
										"<mimetype>%s</mimetype>\r\n"\
										"<width>%d</width>\r\n"\
										"<height>%d</height>\r\n"\
										"<depth>%d</depth>\r\n"\
										"<url>%s</url>\r\n"\
										"</icon>\r\n";

	static char * form_icon_end =		"</iconList>\r\n";

	static char * form_service_start =	"<serviceList>\r\n";

	static char * form_service =		"<service>\r\n"\
										"<serviceType>urn:hdplus:service:opalrc-service:1</serviceType>\r\n"\
										"<serviceId>urn:upnp-org:serviceId:RemoteControl</serviceId>\r\n"\
										"<controlURL/>\r\n"\
										"<eventSubURL/>\r\n"\
										"<SCPDURL>/scpd.xml</SCPDURL>\r\n"\
										"<base-url-http>%s</base-url-http>\r\n"\
										"<base-url-https>%s</base-url-https>\r\n"\
 										"</service>\r\n";

	static char * form_service_end =	"</serviceList>\r\n";

	static char * form_end =  			"</device>\r\n"\
										"</root>\r\n";

	HCHAR		*pBuf = NULL;
	HUINT32		ulDataLen = 0;

	// BODY
	pBuf = (HCHAR *)HLIB_STD_MemAlloc(RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n",__FUNCTION__,__LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 405, NULL, 0);
		return;
	}

	HxSTD_MemSet(pBuf, 0x00, RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);

	PL_COMMON_GetProductInfo(&product_info);
	PL_COMMON_GetDefaultNetwork(&net_info);

	HxSTD_snprintf(szUdn, sizeof(szUdn), "%s%s", __RC_DEFAULT_UDN_PREFIX, net_info.szMacAddr);
	HxSTD_snprintf(szHttpUrl, sizeof(szHttpUrl), "http://%s:%d", net_info.szIp, RC_PORT);
	HxSTD_snprintf(szHttpsUrl, sizeof(szHttpsUrl), "https://%s:%d", net_info.szIp, RC_SSL_PORT);

	HxLOG_Debug("\t\t+++++ [%s][%d] friendly name : %s\n", __FUNCTION__, __LINE__, ctx->pszFriendlyName);

	// MakeString -----> 1) form_begin

	written_len = HxSTD_snprintf(pBuf, ulLen, form_begin);
	ulLen -= written_len;
	ulDataLen += written_len;

	// MakeString -----> 2) form_device
	written_len = HxSTD_snprintf(pBuf + ulDataLen, ulLen, form_device, (ctx->pszFriendlyName == NULL? "" : ctx->pszFriendlyName), (product_info.szManufacturer == NULL? "" : product_info.szManufacturer),
									(product_info.szModelName == NULL? "" : product_info.szModelName), (product_info.szSerialNumber == NULL? "" : product_info.szSerialNumber), szUdn);
	ulLen -= written_len;
	ulDataLen += written_len;

#if 0
	//MakeString -----> 3) form_iconlist
	written_len = HxSTD_snprintf(pBuf + ulDataLen, ulLen, form_icon_start);
	ulLen -= written_len;
	ulDataLen += written_len;
	/*
	for(i = 0; i < icon_num; i++)
	{
		written_len = HxSTD_snprintf(pBuf + ulDataLen, ulLen, form_icon, icon[i].mimetype, icon[i].width, icon[i].height, icon[i].depth, icon[i].url);
		ulLen -= written_len;
		ulDataLen += written_len;
	}
	*/
	written_len = HxSTD_snprintf(pBuf + ulDataLen, ulLen, form_icon_end);
	ulLen -= written_len;
	ulDataLen += written_len;
#endif

	// MakeString -----> 4) form_servicelist
	written_len = HxSTD_snprintf(pBuf + ulDataLen, ulLen, form_service_start);
	ulLen -= written_len;
	ulDataLen += written_len;

	//for(i = 0; i < icon_num; i++)
	{
		written_len = HxSTD_snprintf(pBuf + ulDataLen, ulLen, form_service, szHttpUrl, szHttpsUrl);
		ulLen -= written_len;
		ulDataLen += written_len;
	}

	written_len = HxSTD_snprintf(pBuf + ulDataLen, ulLen, form_service_end);
	ulLen -= written_len;
	ulDataLen += written_len;

	// MakeString -----> 5) form_end
	written_len = HxSTD_snprintf(pBuf + ulDataLen, ulLen, form_end);
	ulDataLen += written_len;

	// Send response
	if(ulDataLen > 0)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type1, 200, pBuf, ulDataLen);
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 405, NULL, 0);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}
}


void __rc_get_ServiceDescription(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
#if 0
	HERROR hErr = ERR_FAIL;
#endif
	int 			written_len = 0;
	HUINT32 	ulLen = RC_RESOURCE_SHORT_MAX_BUF_SIZE; 	// 2048
	static char * scpd =			"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"\
										"<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\r\n"\
										"<specVersion>\r\n"\
										"<major>1</major>\r\n"\
										"<minor>0</minor>\r\n"\
										"</specVersion>\r\n"\
										"<actionList/>\r\n"\
										"<serviceStateTable/>\r\n"\
										"</scpd>\r\n";

	HCHAR		*pBuf = NULL;
	HUINT32 	ulDataLen = 0;

	// BODY
	pBuf = (HCHAR *)HLIB_STD_MemAlloc(RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n",__FUNCTION__,__LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 405, NULL, 0);
		return;
	}

	HxSTD_MemSet(pBuf, 0x00, RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);

	// MakeString ----->

	written_len = HxSTD_snprintf(pBuf, ulLen, scpd);
	ulLen -= written_len;
	ulDataLen += written_len;

	// Send response
	if(ulDataLen > 0)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type1, 200, pBuf, ulDataLen);
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 405, NULL, 0);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}
}



#define __CHECK_PAIRING_INCLUDE_NAME__		0

RC_Pairing_Status_e __rc_restsvc_is_paired_client(struct mg_connection *conn)
{
	HERROR hErr = ERR_FAIL;
	HCHAR szAuthToken[RC_MAX_PATH_LEN] = {0,};
	HCHAR szClientId[RC_MAX_PATH_LEN] = {0,};


	HCHAR *pszAuthToken = NULL;
	HCHAR *pszClientId = NULL;

#if __CHECK_PAIRING_INCLUDE_NAME__
	HCHAR szPairedName[RC_MAX_PATH_LEN] = {0,};
	HCHAR szName[RC_MAX_PATH_LEN] = {0,};
#endif

	if(NULL == conn)
	{
		return eRC_UNPAIRED;
	}

	hErr = HAPI_GetRegistryStr(RC_CURRENT_PAIRED_CLIENT_AUTH_TOKEN, szAuthToken, sizeof(szAuthToken));
	if( (hErr != ERR_OK) /* 최초 생성 */ || (hErr == ERR_OK && HLIB_STD_StrCaseCmp(szAuthToken, "") == 0) )
	{
		return eRC_NO_PAIRING;
	}

#if __CHECK_PAIRING_INCLUDE_NAME__
	hErr = HAPI_GetRegistryStr(RC_CURRENT_PAIRED_CLIENT_NAME, szPairedName, sizeof(szPairedName));
	if( (hErr != ERR_OK) /* 최초 생성 */ || (hErr == ERR_OK && HLIB_STD_StrCaseCmp(szPairedName, "") == 0) )
	{
		return eRC_NO_PAIRING;
	}
	__rc_restsvc_get_client_name(conn, szName);
#endif

	pszAuthToken = (HCHAR*)mg_get_header(conn, "Authorization");
	if(NULL != pszAuthToken )
	{
		HxLOG_Debug("\t\t+++++ [is_paired_client][%d] AuthToken(%s:%s)\n", __LINE__, szAuthToken, pszAuthToken);
#if __CHECK_PAIRING_INCLUDE_NAME__
		HxLOG_Debug("\t\t+++++ [is_paired_client][%d] Name(%s:%s)\n", __LINE__, szPairedName, szName);

		if((HLIB_STD_StrCaseCmp(szAuthToken, pszAuthToken) == 0) && (HLIB_STD_StrCaseCmp(szPairedName, szName) == 0))
#else
		if(HLIB_STD_StrCaseCmp(szAuthToken, pszAuthToken) == 0)
#endif
		{
			return eRC_PAIRED;
		}
		else
		{
			return eRC_UNPAIRED;
		}
	}
	else // case of "GET /rc", Use ID instead of Token
	{
		pszClientId = (HCHAR*)mg_get_header(conn, "From");

		if(NULL != pszClientId)
		{
			HxSTD_MemSet(szClientId, 0x00, RC_MAX_PATH_LEN);
			hErr = HAPI_GetRegistryStr(RC_CURRENT_PAIRED_CLIENT_ID, szClientId, sizeof(szClientId));
			if( (hErr != ERR_OK) /* 최초 생성 */ || (hErr == ERR_OK && HLIB_STD_StrCaseCmp(szClientId, "") == 0) )
			{
				// abnormal status : need to pair again..
				return eRC_NO_PAIRING;;
			}

			HxLOG_Debug("\t\t+++++ [is_paired_client][%d] ClientId(%s:%s)\n", __LINE__, szClientId, pszClientId);
#if __CHECK_PAIRING_INCLUDE_NAME__
			HxLOG_Debug("\t\t+++++ [is_paired_client][%d] Name(%s:%s)\n", __LINE__, szPairedName, szName);

			if((HLIB_STD_StrCaseCmp(szClientId, pszClientId) == 0) && (HLIB_STD_StrCaseCmp(szPairedName, szName) == 0))
#else
			if(HLIB_STD_StrCaseCmp(szClientId, pszClientId) == 0)
#endif
			{
				return eRC_PAIRED;
			}
			else
			{
				return eRC_UNPAIRED;
			}
		}
		else
		{
			return eRC_UNPAIRED;
		}
	}

}

HERROR __rc_restsvc_get_resource(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
#if 0
	HERROR hErr = ERR_FAIL;
#endif
	rc_rest_service_context_t *ctx = __context();
	rc_rest_service_client_info_t *pstClientInfo = ctx->pstCurrentClientInfo;

	int 	 		written_len = 0;
	HUINT32 	ulLen = RC_RESOURCE_SHORT_MAX_BUF_SIZE;		// 2048
	static char * form_begin =  "<?xml version=\"1.0\" ?>"\
								"<response resource=\"rc\">\r\n"\
								"<rcserver name=\"%s\" logo-href=\"http://stb.local/me.jpg\" logo-width=\"20\" logo-height=\"20\" server-id=\"%s\" version=\"%s\" friendly-name=\"%s\">\r\n";
	static char * form_body_resource =  	"<resource rref=\"%s\" />\r\n";
	static char * form_body_resource_end = "</rcserver>\r\n"\
											"<clients>\r\n";
	static char * form_body_client =  	"<client friendly-name=\"%s\" paired=\"%s\" connected=\"%s\" />\r\n";
	static char * form_end =  	"</clients>\r\n"\
								"</response>\r\n";

	HCHAR		szClientName[RC_MAX_PATH_LEN];
	HCHAR		*pBuf = NULL;
	HUINT32		ulDataLen = 0;

	PL_COMMON_ProductInfo_t product_info;
	PL_COMMON_NetworkInfo_t net_info;

	HCHAR			szUdn[48] = {0, };

	HxList_t	*pList = NULL;
	rc_rest_service_desc_t *pDesc = NULL;

	// BODY
	pBuf = (HCHAR *)HLIB_STD_MemAlloc(RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n",__FUNCTION__,__LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
		return ERR_OK;
	}

	HxSTD_MemSet(pBuf, 0x00, RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);

	PL_COMMON_GetProductInfo(&product_info);
	PL_COMMON_GetDefaultNetwork(&net_info);

	HxSTD_snprintf(szUdn, sizeof(szUdn), "%s%s", __RC_DEFAULT_UDN_PREFIX, net_info.szMacAddr);

	HxLOG_Debug("\t\t+++++ [%s][%d] friendly name : %s\n", __FUNCTION__, __LINE__, ctx->pszFriendlyName);

	// MakeString -----> 1) form_begin
	written_len = HxSTD_snprintf(pBuf, ulLen, form_begin, (product_info.szModelName == NULL? "" : product_info.szModelName),
									szUdn, "1.0", (ctx->pszFriendlyName == NULL? "" : ctx->pszFriendlyName));

	ulLen -= written_len;
	ulDataLen += written_len;

	// MakeString -----> 2) form_body_resource (pairing 되어있는 장치가 없을때만 필요한 정보)
	if((pstClientInfo->bPaired == FALSE) || (__rc_restsvc_is_paired_client(conn) == eRC_PAIRED))
	{
		pList = HLIB_LIST_First(ctx->pRestSvcList);
		while(pList)
		{
			if(pList->data)
			{
				pDesc = (rc_rest_service_desc_t*)HLIB_LIST_Data(pList);
				written_len = HxSTD_snprintf(pBuf + ulDataLen, ulLen, form_body_resource, pDesc->szRootPath + 1);
				ulLen -= written_len;
				ulDataLen += written_len;

				pList = pList->next;
			}
		}
	}

	// MakeString -----> 3) form_body_resource_end
	written_len = HxSTD_snprintf(pBuf + ulDataLen, ulLen, form_body_resource_end);
	ulLen -= written_len;
	ulDataLen += written_len;

	// MakeString -----> 3) form_body_client
	__rc_restsvc_get_client_name(conn, szClientName);
	written_len = HxSTD_snprintf(pBuf + ulDataLen, ulLen, form_body_client, szClientName, (__rc_restsvc_is_paired_client(conn) == eRC_PAIRED) ? "true" : "false", "true" /*connected*/);
	ulLen -= written_len;
	ulDataLen += written_len;


	// MakeString -----> 3) form_end
	written_len = HxSTD_snprintf(pBuf + ulDataLen, ulLen, form_end);
	ulDataLen += written_len;

	// Send response
	if(ulDataLen > 0)
	{
		if((pstClientInfo->bPaired == TRUE) && (__rc_restsvc_is_paired_client(conn) == eRC_UNPAIRED))
		{
			// TODO: 이미 pairing 되어있는 device가 GET /rc 를 요청한 device와 동일한 경우에 대한 처리는 어떻게..
			__rc_restsvc_send_response(conn, eRC_Header_Type2, 423, pBuf, ulDataLen);
		}
		else
		{
			__rc_restsvc_send_response(conn, eRC_Header_With_AuthToken, 200, pBuf, ulDataLen);
		}
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	return ERR_OK;
}

HERROR __rc_restsvc_post_resource(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	rc_rest_service_context_t 	*ctx = __context();
	HERROR						hErr = ERR_FAIL;
	HxXMLDOC					hXmlDocHandle = NULL;
	HxXMLELEMENT				hResponseElement = NULL;
	HUINT8						*pszCmd = NULL;
	HUINT8						*pszOnscreen = NULL;
	HUINT8						*pszMsg = NULL;

	HCHAR						*pBuf = NULL;
	HUINT32						ulContentLen = 0;
	HBOOL						bPopupMsg = FALSE;

	// 1. conn->content 의 XML Parsing
	// 2. Parsing된 내용의 cmd / arg / msg / onscreen element를 확인하여 적절한 처리 진행

	// Get Content (RAW)
	ulContentLen = conn->content_len + 1;		// NULL ('\0') included

	pBuf = (HCHAR*)HLIB_STD_MemAlloc(sizeof(HCHAR) * ulContentLen);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n", __FUNCTION__, __LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		return ERR_OK;
	}

	HxSTD_MemSet(pBuf, 0x00, ulContentLen);
	HxSTD_MemCopy(pBuf, conn->content, conn->content_len);

	HxLOG_Debug("\t\t+++++ post data (LEN:%d)\n\n%s\n\n", ulContentLen, pBuf);

	// Parse Content (XML)
	hErr = HLIB_XML_InitParser();
	hErr = HLIB_XML_ParseBuffer(pBuf, ulContentLen, &hXmlDocHandle);
	hErr = HLIB_XML_GetRootElement(hXmlDocHandle, &hResponseElement);		// -------> <command> element

	// onscreen attribute
	bPopupMsg = FALSE;
	hErr = HLIB_XML_GetAttrContents(hResponseElement, "onscreen", &pszOnscreen);
	if((hErr == ERR_OK)&& (NULL != pszOnscreen))
	{
		if(HLIB_STD_StrCaseCmp(pszOnscreen, "true") == 0)
		{
			bPopupMsg = TRUE;
		}
	}

	// <cmd="XX"> attribute
	hErr = HLIB_XML_GetAttrContents(hResponseElement, "cmd", &pszCmd);
	if(hErr == ERR_OK)
	{
		if(pszCmd != NULL)
		{
			if(HLIB_STD_StrCaseCmp(pszCmd, "pair") == 0)
			{
				hErr = __rc_restsvc_pair_client_info(conn);
				if(hErr == ERR_OK)				{
					pszMsg = NULL;
					RemoteCtrl_AppendEventLogs(RCEVT_TYPE_PAIRING, RCEVT_SUBTYPE_RESOURCE_PAIR, 0, 0, 0);
					if(bPopupMsg == TRUE)
					{
						hErr = HLIB_XML_GetAttrContents(hResponseElement, "msg", &pszMsg);
						if(hErr == ERR_OK)
						{
							if(pszMsg == NULL)
							{
								goto RC_RESTSVC_EXIT;
							}
							HxLOG_Debug("\t\t+++++ [%s][%d] msg : %s\n", __FUNCTION__, __LINE__, pszMsg);
						}
					}
					ctx->client_cb.pair_cb(pszMsg, ctx->pstCurrentClientInfo->szClientName);
				}
			}
			else if(HLIB_STD_StrCaseCmp(pszCmd, "unpair") == 0)
			{
				hErr = __rc_restsvc_unpair_client_info(conn, FALSE);
				if(hErr == ERR_OK)				{
					pszMsg = NULL;
					RemoteCtrl_AppendEventLogs(RCEVT_TYPE_PAIRING, RCEVT_SUBTYPE_RESOURCE_UNPAIR, 0, 0, 0);
					if(bPopupMsg == TRUE)
					{
						hErr = HLIB_XML_GetAttrContents(hResponseElement, "msg", &pszMsg);
						if(hErr == ERR_OK)
						{
							if(pszMsg == NULL)
							{
								goto RC_RESTSVC_EXIT;
							}
							HxLOG_Debug("\t\t+++++ [%s][%d] msg : %s\n", __FUNCTION__, __LINE__, pszMsg);
						}
					}
					ctx->client_cb.unpair_cb(pszMsg);
				}
			}
			else if(HLIB_STD_StrCaseCmp(pszCmd, "customer-message") == 0)
			{
				if((ctx->pstCurrentClientInfo->bPaired == TRUE) && (__rc_restsvc_is_paired_client(conn) == eRC_PAIRED))
				{
					pszMsg = NULL;
					hErr = HLIB_XML_GetAttrContents(hResponseElement, "msg", &pszMsg);
					if(hErr == ERR_OK)
					{
						if(pszMsg == NULL)
						{
							hErr = ERR_FAIL;
							goto RC_RESTSVC_EXIT;
						}
						HxLOG_Debug("\t\t+++++ [%s][%d] msg : %s\n", __FUNCTION__, __LINE__, pszMsg);
						if(bPopupMsg == TRUE)
						{
							ctx->client_cb.msg_cb(pszMsg);
						}
					}
				}
				else
				{
					hErr = ERR_FAIL;
				}
			}
			else if(HLIB_STD_StrCaseCmp(pszCmd, "setname") == 0)
			{
				if((ctx->pstCurrentClientInfo->bPaired == TRUE) && (__rc_restsvc_is_paired_client(conn) == eRC_PAIRED))
				{
					hErr = HLIB_XML_GetAttrContents(hResponseElement, "arg", &pszCmd);
					if(hErr == ERR_OK)
					{
						if(pszCmd == NULL)
						{
							hErr = ERR_FAIL;
							goto RC_RESTSVC_EXIT;
						}

						HxSTD_MemSet(ctx->pszFriendlyName, 0x00, RC_MAX_PATH_LEN);
						if(HxSTD_StrLen(pszCmd) < RC_MAX_PATH_LEN)
						{
							HxSTD_MemCopy(ctx->pszFriendlyName, pszCmd, HxSTD_StrLen(pszCmd));
						}
						else
						{
							HxSTD_MemCopy(ctx->pszFriendlyName, pszCmd, (RC_MAX_PATH_LEN -1));
						}

						ctx->client_cb.set_friendly_name_cb(ctx->pszFriendlyName);

						hErr = HAPI_SetRegistryStr(RC_SERVER_NAME, ctx->pszFriendlyName);
						if(hErr == ERR_OK)
						{
							RemoteCtrl_AppendEventLogs(RCEVT_TYPE_RECEIVER, RCEVT_SUBTYPE_RESOURCE_NAME, 0, 0, 0);
						}
					}
				}
				else
				{
					hErr = ERR_FAIL;
				}
			}
			else
			{
				HxLOG_Debug("\t\t+++++ [%s][%d] Something wrong!!!, cmd : %s\n", __FUNCTION__, __LINE__, pszCmd);
				__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
				goto RC_RESTSVC_EXIT;
			}
		}
	}
	else
	{
		HxLOG_Debug("\t\t+++++ [%s][%d]\n", __FUNCTION__, __LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
		goto RC_RESTSVC_EXIT;
	}

	if(hErr == ERR_OK)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 202, NULL, 0);
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 423, NULL, 0);
	}

RC_RESTSVC_EXIT:
	if(pszCmd)
	{
		HLIB_XML_FreeAttrContents(pszCmd);
	}

	if(pszOnscreen)
	{
		HLIB_XML_FreeAttrContents(pszOnscreen);
	}

	if(pszMsg)
	{
		HLIB_XML_FreeAttrContents(pszMsg);
	}

	if(hXmlDocHandle)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	return ERR_OK;
}

HERROR __rc_restsvc_get_locale(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HUINT32						ulLen = 0;
	HUINT8						*pBuf = NULL;
	HERROR						hErr = ERR_FAIL;

	pBuf = (HUINT8 *)HLIB_STD_MemAlloc(RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n", __FUNCTION__, __LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		return ERR_OK;
	}

	HxSTD_MemSet(pBuf, 0x00, RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);

	hErr = Get_RC_Locale(pBuf, &ulLen);
	if(hErr == ERR_OK)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type1, 200, pBuf, ulLen);
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	return ERR_OK;
}


HERROR __rc_restsvc_get_events(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HUINT32						ulLen = 0;
	HUINT8						*pBuf = NULL;
	HERROR						hErr = ERR_FAIL;

	rc_rest_service_context_t *ctx = __context();
	rc_rest_service_client_info_t *pstClientInfo = ctx->pstCurrentClientInfo;

	pBuf = (HUINT8 *)HLIB_STD_MemAlloc(RC_PACKET_MAX_BUF_SIZE+1);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n", __FUNCTION__, __LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		return ERR_OK;
	}

	if((pstClientInfo->bPaired == TRUE) && (__rc_restsvc_is_paired_client(conn) == eRC_PAIRED))
	{
		HxSTD_MemSet(pBuf, 0x00, RC_PACKET_MAX_BUF_SIZE+1);

		hErr = Get_RC_Events(pBuf, (HUINT8*)conn->uri, &ulLen);
		if(hErr == ERR_OK)
		{
			__rc_restsvc_send_response(conn, eRC_Header_Type1, 200, pBuf, ulLen);
		}
		else
		{
			__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		}

		if(pBuf)
		{
			HLIB_STD_MemFree(pBuf);
		}
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 423, NULL, 0);
	}

	return ERR_OK;
}

HERROR __rc_restsvc_post_locale(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HxLOG_Debug("\t\t+++++ CALL [%s][%d]\n", __FUNCTION__, __LINE__);
	// TODO:
	return ERR_OK;
}

HERROR __rc_restsvc_get_remote(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HUINT32						ulLen = 0;
	HUINT8						*pBuf = NULL;
	HERROR						hErr = ERR_FAIL;

	pBuf = (HUINT8 *)HLIB_STD_MemAlloc(RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n", __FUNCTION__, __LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		return ERR_OK;
	}

	HxSTD_MemSet(pBuf, 0x00, RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);

	hErr = Get_RC_Remote(pBuf, &ulLen);
	if(hErr == ERR_OK)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type1, 200, pBuf, ulLen);
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	return ERR_OK;
}

HERROR __rc_restsvc_post_remote(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HERROR hErr = ERR_FAIL;
	HCHAR *pBuf = NULL;
	HUINT32 ulContentLen = 0;

	ulContentLen = conn->content_len + 1;		// NULL ('\0') included

	pBuf = (HCHAR*)HLIB_STD_MemAlloc(sizeof(HCHAR) * ulContentLen);

	HxSTD_MemSet(pBuf, 0x00, ulContentLen);
	HxSTD_MemCopy(pBuf, conn->content, conn->content_len);

	HxLOG_Debug("\t\t+++++ post data (LEN:%d)\n\n%s\n\n", ulContentLen, pBuf);

	// 1. conn->content 의 XML Parsing
	// 2. Parsing된 내용의 VK_XXX 를 DIKS_XXX로 변환하여 HAPI_SendRemoteRCUKey() param 으로 전달

	hErr = Post_RC_Remote(pBuf, ulContentLen);
	if(hErr == ERR_OK)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 202, NULL, 0);
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	return ERR_OK;
}

HERROR __rc_restsvc_get_outputs(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HUINT32						ulLen = 0;
	HUINT8						*pBuf = NULL;
	HERROR						hErr = ERR_FAIL;

	HCHAR					*pszOutput = NULL;

	pszOutput = HxSTD_StrRChar((HUINT8*)conn->uri,'/');
	pszOutput++;

	if(HLIB_STD_StrCaseCmp(pszOutput, "outputs") == 0)
	{
		pBuf = (HUINT8 *)HLIB_STD_MemAlloc(RC_RESOURCE_MAX_BUF_SIZE+1);
		if(pBuf == NULL)
		{
			HxLOG_Print("[%s:%d] Memory Allocation Fail \n", __FUNCTION__, __LINE__);
			__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
			return ERR_OK;
		}

		HxSTD_MemSet(pBuf, 0x00, RC_RESOURCE_MAX_BUF_SIZE+1);

		hErr = Get_RC_Outputs(pBuf, (HUINT8*)conn->uri, &ulLen);
		if(hErr == ERR_OK)
		{
			__rc_restsvc_send_response(conn, eRC_Header_Type1, 200, pBuf, ulLen);
		}
		else
		{
			__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		}

		if(pBuf)
		{
			HLIB_STD_MemFree(pBuf);
		}

		return ERR_OK;
	}
	else
	{
		rc_rest_service_context_t *ctx = __context();

		HAPI_BroadcastSignal("signal:onRcRequestOutputs", 0, "ss", conn->uri, pszOutput);
		ctx->bReply = FALSE;

		return ERR_FAIL;
	}
}

HERROR __rc_restsvc_post_outputs(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	//rc_rest_service_context_t 	*ctx = __context();
	HERROR						hErr = ERR_FAIL;
	HxXMLDOC					hXmlDocHandle = NULL;
	HxXMLELEMENT				hResponseElement = NULL;
	HUINT8						*pszCmd = NULL;
	HUINT8						*pszTemp = NULL;

	HCHAR						*pBuf = NULL;
	HUINT32						ulContentLen = 0;

	// 1. conn->content 의 XML Parsing
	// 2. Parsing된 내용의 cmd / arg / msg / onscreen element를 확인하여 적절한 처리 진행

	// Get Content (RAW)
	ulContentLen = conn->content_len + 1;		// NULL ('\0') included

	pBuf = (HCHAR*)HLIB_STD_MemAlloc(sizeof(HCHAR) * ulContentLen);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n", __FUNCTION__, __LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		return ERR_OK;
	}

	HxSTD_MemSet(pBuf, 0x00, ulContentLen);
	HxSTD_MemCopy(pBuf, conn->content, conn->content_len);

	HxLOG_Debug("\t\t+++++ post data (LEN:%d)\n\n%s\n\n", ulContentLen, pBuf);

	// Parse Content (XML)
	hErr = HLIB_XML_InitParser();
	hErr = HLIB_XML_ParseBuffer(pBuf, ulContentLen, &hXmlDocHandle);
	hErr = HLIB_XML_GetRootElement(hXmlDocHandle, &hResponseElement);

	// <XXXX ...> Element name..
	hErr = HLIB_XML_GetElementName(hResponseElement, &pszCmd);
	if(hErr == ERR_OK)
	{
		if(pszCmd != NULL)
		{
			if(HLIB_STD_StrCaseCmp(pszCmd, "settings") == 0)
			{
				HFLOAT32				fVolume;
				RC_Outputs_Setup_t		stSetupInfo;

				HxSTD_MemSet(&stSetupInfo, 0x00, sizeof(RC_Outputs_Setup_t));

				//RemoteCtrl_AppendEventLogs(RCEVT_TYPE_PAIRING, RCEVT_SUBTYPE_RESOURCE_PAIR, 0, 0, 0);

				hErr = HLIB_XML_GetAttrContents(hResponseElement, "volume", &pszTemp);
				if(hErr == ERR_OK)
				{
					if(pszTemp != NULL)
					{
						sscanf(pszTemp, "%f", &fVolume);
						fVolume *= 100;
						HxLOG_Debug("\t\t+++++ [%s][%d] volume : %s, %.2f\n", __FUNCTION__, __LINE__, pszTemp, fVolume);

						stSetupInfo.ulVolumne = (HUINT32)fVolume;

						HLIB_XML_FreeAttrContents(pszTemp);
						pszTemp = NULL;
					}
				}
				else
				{
					goto POST_OUTPUT_EXIT;
				}

				hErr = HLIB_XML_GetAttrContents(hResponseElement, "mute", &pszTemp);
				if(hErr == ERR_OK)
				{
					if(pszTemp != NULL)
					{
						stSetupInfo.bMute = (HLIB_STD_StrCaseCmp(pszTemp, "true") == 0?  TRUE: FALSE);
						HxLOG_Debug("\t\t+++++ [%s][%d] mute : %s, %d\n", __FUNCTION__, __LINE__, pszTemp, stSetupInfo.bMute);

						HLIB_XML_FreeAttrContents(pszTemp);
						pszTemp = NULL;
					}
				}
				else
				{
					goto POST_OUTPUT_EXIT;
				}

				hErr = HLIB_XML_GetAttrContents(hResponseElement, "subtitle", &pszTemp);
				if(hErr == ERR_OK)
				{
					if(pszTemp != NULL)
					{
						stSetupInfo.bSubtitle = (HLIB_STD_StrCaseCmp(pszTemp, "true") == 0?  TRUE: FALSE);
						HxLOG_Debug("\t\t+++++ [%s][%d] sbtl : %s, %d\n", __FUNCTION__, __LINE__, pszTemp, stSetupInfo.bSubtitle);

						HLIB_XML_FreeAttrContents(pszTemp);
						pszTemp = NULL;
					}
				}
				else
				{
					goto POST_OUTPUT_EXIT;
				}

				hErr = HLIB_XML_GetAttrContents(hResponseElement, "ad", &pszTemp);
				if(hErr == ERR_OK)
				{
					if(pszTemp != NULL)
					{
						stSetupInfo.bAD = (HLIB_STD_StrCaseCmp(pszTemp, "true") == 0?  TRUE: FALSE);
						HxLOG_Debug("\t\t+++++ [%s][%d] AD : %s, %d\n", __FUNCTION__, __LINE__, pszTemp, stSetupInfo.bAD);

						HLIB_XML_FreeAttrContents(pszTemp);
						pszTemp = NULL;
					}
				}
				else
				{
					goto POST_OUTPUT_EXIT;
				}

				hErr = OAPI_OUTPUT_AUDIO_SetVolume(stSetupInfo.ulVolumne);
				hErr = HAPI_BroadcastSignal("signal:onSubtitleState", 1, "i", stSetupInfo.bSubtitle ? 1 : 0);
				hErr = OAPI_OUTPUT_AUDIO_SetAudioDescription(stSetupInfo.bAD);
				hErr = OAPI_OUTPUT_AUDIO_SetMasterMute(stSetupInfo.bMute);
			}
			else if(HLIB_STD_StrCaseCmp(pszCmd, "programme") == 0)
			{
				RC_Outputs_Program_t	stSvcInfo;
				HxXMLELEMENT			hChildElement = NULL;

				HxSTD_MemSet(&stSvcInfo, 0x00, sizeof(RC_Outputs_Program_t));

				//RemoteCtrl_AppendEventLogs(RCEVT_TYPE_PAIRING, RCEVT_SUBTYPE_RESOURCE_PAIR, 0, 0, 0);

				hErr = HLIB_XML_GetAttrContents(hResponseElement, "sid", &pszTemp);
				if(hErr == ERR_OK)
				{
					if(pszTemp != NULL)
					{
						//HxSTD_MemSet(stSvcInfo.szSourccId, 0x00, RC_MAX_SERVICE_NAME_LEN);
						//HxSTD_snprintf(stSvcInfo.szSourccId, RC_MAX_SERVICE_NAME_LEN, "%s", pszTemp);
						sscanf(pszTemp, "%d", &stSvcInfo.usSourceId);
						HxLOG_Debug("\t\t+++++ [%s][%d] sid : %s, %d\n", __FUNCTION__, __LINE__, pszTemp, stSvcInfo.usSourceId);

						HLIB_XML_FreeAttrContents(pszTemp);
						pszTemp = NULL;
					}
				}
				else
				{
					goto POST_OUTPUT_EXIT;
				}

				hErr = HLIB_XML_GetChildElement(hResponseElement, &hChildElement);
				if(hErr == ERR_OK)
				{
					hErr = HLIB_XML_GetAttrContents(hChildElement, "type", &pszTemp);
					if(hErr == ERR_OK)
					{
						if(pszTemp != NULL)
						{
							stSvcInfo.stEsInfo[0].eType = (HLIB_STD_StrCaseCmp(pszTemp, "audio") == 0?  eRC_COMPONENT_AUDIO: eRC_COMPONENT_SUBTITLE);
							HxLOG_Debug("\t\t+++++ [%s][%d] sid : %s, %d\n", __FUNCTION__, __LINE__, pszTemp, stSvcInfo.stEsInfo[0].eType);

							HLIB_XML_FreeAttrContents(pszTemp);
							pszTemp = NULL;

							hErr = HLIB_XML_GetAttrContents(hChildElement, "lang", &pszTemp);
							if(hErr == ERR_OK)
							{
								if(pszTemp != NULL)
								{
									HxSTD_MemSet(stSvcInfo.stEsInfo[0].szLang, 0x00, 4);
									HxSTD_snprintf(stSvcInfo.stEsInfo[0].szLang, 4, "%s", pszTemp);
									HxLOG_Debug("\t\t+++++ [%s][%d] sid : %s, %s\n", __FUNCTION__, __LINE__, pszTemp, stSvcInfo.stEsInfo[0].szLang);

									HLIB_XML_FreeAttrContents(pszTemp);
									pszTemp = NULL;

									stSvcInfo.ulComponentNum++;
								}
							}
						}
					}


				}

				hErr = HAPI_BroadcastSignal("signal:onRcSetChannel", 0, "b", &stSvcInfo, sizeof(RC_Outputs_Program_t));
			}
			else
			{
				HxLOG_Debug("\t\t+++++ [%s][%d] Something wrong!!!, cmd : %s\n", __FUNCTION__, __LINE__, pszCmd);
				__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
				goto RC_RESTSVC_EXIT;
			}
		}
	}
	else
	{
		HxLOG_Debug("\t\t+++++ [%s][%d]\n", __FUNCTION__, __LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
		goto RC_RESTSVC_EXIT;
	}

POST_OUTPUT_EXIT:
	if(hErr == ERR_OK)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 202, NULL, 0);
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 423, NULL, 0);
	}

RC_RESTSVC_EXIT:
	if(hXmlDocHandle)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	return ERR_OK;
}

HERROR __rc_restsvc_send_outputs(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HUINT32						ulLen = 0;
	HUINT8						*pBuf = NULL;
	HERROR						hErr = ERR_FAIL;

	pBuf = (HUINT8 *)HLIB_STD_MemAlloc(RC_RESOURCE_MAX_BUF_SIZE+1);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n", __FUNCTION__, __LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		return ERR_OK;
	}

	HxSTD_MemSet(pBuf, 0x00, RC_RESOURCE_MAX_BUF_SIZE+1);

	hErr = Get_RC_OutputInfo(pBuf, (HUINT8*)conn->uri, &ulLen, param1);
	if(hErr == ERR_OK)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type1, 200, pBuf, ulLen);
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	return ERR_OK;
}

HERROR __rc_restsvc_get_sourcelists(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HUINT32						ulLen = 0;
	HUINT8						*pBuf = NULL;
	HERROR						hErr = ERR_FAIL;

	HCHAR					*pszListId = NULL;

	pszListId = HxSTD_StrRChar((HUINT8*)conn->uri,'/');
	pszListId++;

	HxLOG_Debug("\t\t+++++ [%s][%d] %s \n", __FUNCTION__, __LINE__, pszListId);

	if(HLIB_STD_StrCaseCmp(pszListId, "sourcelists") == 0)
	{
		pBuf = (HUINT8 *)HLIB_STD_MemAlloc(RC_RESOURCE_MAX_BUF_SIZE+1);
		if(pBuf == NULL)
		{
			HxLOG_Print("[%s:%d] Memory Allocation Fail \n", __FUNCTION__, __LINE__);
			__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
			return ERR_OK;
		}

		HxSTD_MemSet(pBuf, 0x00, RC_RESOURCE_MAX_BUF_SIZE+1);

		hErr = Get_RC_Srclists(pBuf, (HUINT8*)conn->uri, &ulLen);
		if(hErr == ERR_OK)
		{
			__rc_restsvc_send_response(conn, eRC_Header_Type1, 200, pBuf, ulLen);
		}
		else
		{
			__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		}

		if(pBuf)
		{
			HLIB_STD_MemFree(pBuf);
		}

		return ERR_OK;
	}
	else
	{
		rc_rest_service_context_t *ctx = __context();

		if(IsExist_RC_Srclist(pszListId, NULL) == TRUE)
		{
			HAPI_BroadcastSignal("signal:onRcRequestSources", 0, "ss", conn->uri, pszListId);
			ctx->bReply = FALSE;

			return ERR_FAIL;
		}
		else
		{
			__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
			return ERR_OK;
		}
	}
}

HERROR __rc_restsvc_get_sources(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	rc_rest_service_context_t 	*ctx = __context();
	HCHAR						*pszListId = NULL;

	pszListId = HxSTD_StrRChar((HUINT8*)conn->uri,'/');
	pszListId++;

	HxLOG_Debug("\t\t+++++ [%s][%d] %s \n", __FUNCTION__, __LINE__, pszListId);

	if(IsExist_RC_Srclist(pszListId, NULL) == TRUE)
	{
		HAPI_BroadcastSignal("signal:onRcRequestSources", 0, "ss", conn->uri, pszListId);
		ctx->bReply = FALSE;

		return ERR_FAIL;
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
		return ERR_OK;
	}
}

HERROR __rc_restsvc_get_update_sources(HUINT8 *pBuf, HUINT32 ulContentLen, RC_Sources_Info_t **pstUpdateSources, HUINT32 *pulNum)
{

	HERROR						hErr = ERR_FAIL;
	HxXMLDOC					hXmlDocHandle = NULL;
	HxXMLELEMENT				hResponseElement = NULL;
	HxXMLELEMENT				hChildElement = NULL;
	HUINT8						*pszCmd = NULL;
	HUINT8						*pszTemp = NULL;

	RC_Sources_Info_t 			*pstSvcInfos = NULL;
	HUINT32						i, ulSvcNum = 0;

	*pstUpdateSources = NULL;
	*pulNum = 0;

	// Parse Content (XML)
	hErr = HLIB_XML_InitParser();
	hErr = HLIB_XML_ParseBuffer(pBuf, ulContentLen, &hXmlDocHandle);
	hErr = HLIB_XML_GetRootElement(hXmlDocHandle, &hResponseElement);

	// <XXXX ...> Element name..
	hErr = HLIB_XML_GetElementName(hResponseElement, &pszCmd);
	if(hErr == ERR_OK)
	{
		if(pszCmd != NULL)
		{
			if(HLIB_STD_StrCaseCmp(pszCmd, "sources") == 0)
			{
				HxList_t		*pChildEelementList = NULL;

				hErr = HLIB_XML_FindChildElementByName(hResponseElement, "source", &pChildEelementList);
				if(hErr == ERR_OK)
				{
					ulSvcNum = HLIB_LIST_Length(pChildEelementList);

					pstSvcInfos = HLIB_STD_MemAlloc(ulSvcNum * sizeof(RC_Sources_Info_t));
					if(pstSvcInfos == NULL)
					{
						hErr = ERR_FAIL;
						goto RC_RESTSVC_EXIT;
					}

					for(i = 0; i < ulSvcNum; i++)
					{
						hChildElement = HLIB_LIST_GetListItem(pChildEelementList, i);

						hErr = HLIB_XML_GetAttrContents(hChildElement, "lcn", &pszTemp);
						if(hErr == ERR_OK)
						{
							if(pszTemp != NULL)
							{
								sscanf(pszTemp, "%d", &pstSvcInfos[i].usLcn);

								HLIB_XML_FreeAttrContents(pszTemp);
								pszTemp = NULL;

								hErr = HLIB_XML_GetAttrContents(hChildElement, "sid", &pszTemp);
								if(hErr == ERR_OK)
								{
									if(pszTemp != NULL)
									{
										sscanf(pszTemp, "%d", &pstSvcInfos[i].usSid);
										//HxSTD_MemSet(pstSvcInfos[i].szSourccId, 0x00, RC_MAX_SERVICE_NAME_LEN);
										//HxSTD_snprintf(pstSvcInfos[i].szSourccId, RC_MAX_SERVICE_NAME_LEN, "%s", pszTemp);
										HxLOG_Debug("\t\t+++++ [%s][%d] sid : %s, %d\n", __FUNCTION__, __LINE__, pszTemp, pstSvcInfos[i].usSid);

										HLIB_XML_FreeAttrContents(pszTemp);
										pszTemp = NULL;
									}
								}
							}
						}
					}

					*pstUpdateSources = pstSvcInfos;
					*pulNum = ulSvcNum;
				}
			}
			else
			{
				HxLOG_Debug("\t\t+++++ [%s][%d] Something wrong!!!, cmd : %s\n", __FUNCTION__, __LINE__, pszCmd);
				hErr = ERR_FAIL;
			}
		}
	}
	else
	{
		hErr = ERR_FAIL;
	}

RC_RESTSVC_EXIT:
	if(hXmlDocHandle)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}

	return hErr;
}


HERROR __rc_restsvc_post_sources(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HERROR 						hErr = ERR_FAIL;

	HBOOL						bModifiable = FALSE;
	HBOOL						bExist = FALSE;

	//rc_rest_service_context_t 	*ctx = __context();
	HCHAR						*pszListId = NULL;

	pszListId = HxSTD_StrRChar((HUINT8*)conn->uri,'/');
	pszListId++;

	bExist = IsExist_RC_Srclist(pszListId, &bModifiable);

	if(TRUE == bExist || TRUE == bModifiable)
	{
		HUINT8 						*pBuf = NULL;
		HUINT32 					ulNum = 0, ulContentLen = 0;
		RC_Sources_Info_t			*pstUpdateSources = NULL;

		ulContentLen = conn->content_len + 1;		// NULL ('\0') included

		pBuf = (HCHAR*)HLIB_STD_MemAlloc(sizeof(HCHAR) * ulContentLen);

		HxSTD_MemSet(pBuf, 0x00, ulContentLen);
		HxSTD_MemCopy(pBuf, conn->content, conn->content_len);

		hErr = __rc_restsvc_get_update_sources(pBuf, ulContentLen, &pstUpdateSources, &ulNum);
		if(hErr == ERR_OK)
		{
			HAPI_BroadcastSignal("signal:onRcUpdateSources", 0, "sib", pszListId, ulNum, pstUpdateSources, (ulNum * sizeof(RC_Sources_Info_t)));
			__rc_restsvc_send_response(conn, eRC_Header_Type2, 202, NULL, 0);
		}
		else
		{
			__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
		}

		if(pBuf)
		{
			HLIB_STD_MemFree(pBuf);
		}

		if(pstUpdateSources)
		{
			HLIB_STD_MemFree(pstUpdateSources);
		}
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 403, NULL, 0);
	}

	return ERR_OK;
}

HERROR __rc_restsvc_send_sources(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HUINT32						ulLen = 0;
	HUINT8						*pBuf = NULL;
	HERROR						hErr = ERR_FAIL;

	if((HINT32)param2 == -1)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 400, NULL, 0);
		return ERR_OK;
	}

	ulLen = SRC_INFO_MAX_SIZE * (HUINT32)param2;
	ulLen++;

	pBuf = (HUINT8 *)HLIB_STD_MemAlloc(ulLen);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n", __FUNCTION__, __LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		return ERR_OK;
	}

	HxSTD_MemSet(pBuf, 0x00, ulLen);

	hErr = Get_RC_Sources(pBuf, (HUINT8*)conn->uri, &ulLen, param1, (HINT32)param2);
	if(hErr == ERR_OK)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type1, 200, pBuf, ulLen);
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	return ERR_OK;
}



HERROR __rc_restsvc_post_apps(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HUINT8 						*pBuf = NULL;
	HUINT32 					ulContentLen = 0;

	rc_rest_service_context_t 	*ctx = __context();

	//RC_Outputs_Apps_t			stAppsInfo;
	HCHAR						*pszSid = NULL;
	HCHAR						*pszCid = NULL;

	HUINT8						szUrl[RC_MAX_URL];

#if 1
	HBOOL						bOK = FALSE;
	HCHAR *delim = "\"";

	HCHAR *token = NULL, *save = NULL;
	HxList_t *pList = NULL, *pListNode = NULL;

	ulContentLen = conn->content_len + 1;		// NULL ('\0') included

	pBuf = (HCHAR*)HLIB_STD_MemAlloc(sizeof(HCHAR) * ulContentLen);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n", __FUNCTION__, __LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		return ERR_OK;
	}

	HxSTD_MemSet(pBuf, 0x00, ulContentLen);
	HxSTD_MemCopy(pBuf, conn->content, conn->content_len);

	HxLOG_Debug("\t\t+++++ post data (LEN:%d)\n\n%s\n\n", ulContentLen, pBuf);

	token = HxSTD_StrTok(pBuf, delim, &save);
	//HxLOG_Debug("\t\t+++++ [%s][%d], Token : %s\n", __FUNCTION__, __LINE__, token);

	while(token != NULL)
	{
		pList = HLIB_LIST_Append(pList, (void*)HLIB_STD_StrDup(token));
		token = HxSTD_StrTok(NULL, delim, &save);
		HxLOG_Debug("\t\t+++++ [%d], Token : %s\n", __LINE__, token);
	}

	// 분리된 token 중 맨 마지막 data를 client name으로 판단함
	pListNode = HLIB_LIST_First(pList);
	while(pListNode != NULL)
	{
		token = (HCHAR*)HLIB_LIST_Data(pListNode);
		//HxLOG_Debug("\t\t+++++ [%d], Token : %s\n", __LINE__, token);
		if(HxSTD_StrStr(token, "sid") != NULL)
		{
			pListNode = HLIB_LIST_Next(pListNode);
			if(pListNode != NULL)
			{
				pszSid = (HCHAR*)HLIB_LIST_Data(pListNode);
				//HxSTD_MemSet(stAppsInfo.szSourceId, 0x00, RC_MAX_URL);
				//HxSTD_snprintf(stAppsInfo.szSourceId, RC_MAX_URL, "%s", token);
				HxLOG_Debug("\t [%d]: sid: %s \n", __LINE__, pszSid);

				pListNode = HLIB_LIST_Next(pListNode);
				if((pListNode != NULL) && (HxSTD_StrStr(HLIB_LIST_Data(pListNode), "cid") != NULL))
				{
					pListNode = HLIB_LIST_Next(pListNode);
					if(pListNode != NULL)
					{
						pszCid = (HCHAR*)HLIB_LIST_Data(pListNode);
						//HxSTD_MemSet(stAppsInfo.szContentId, 0x00, RC_MAX_URL);
						//HxSTD_snprintf(stAppsInfo.szContentId, RC_MAX_URL, "%s", token);
						HxLOG_Debug("\t [%d]: cid: %s \n", __LINE__, pszCid);

						bOK = TRUE;

						/*
						HxLOG_Debug("\t [%d]: send signal:onRcLaunchApp\n", __LINE__);
						HAPI_BroadcastSignal("signal:onRcLaunchApp", 0, "b", &stAppsInfo, sizeof(RC_Outputs_Apps_t));
						*/

						HxSTD_MemSet(szUrl, 0x00, RC_MAX_URL);
						HxSTD_snprintf(szUrl, RC_MAX_URL, "%s?%s", pszSid, pszCid);
						ctx->client_cb.launch_app_cb(szUrl);

						__rc_restsvc_send_response(conn, eRC_Header_Type2, 202, NULL, 0);

					}
				}
			}
			break;
		}
		pListNode = HLIB_LIST_Next(pListNode);
	}

	if(pList)
	{
		HLIB_LIST_Foreach(pList, __rc_restsvc_memfree);
		HLIB_LIST_RemoveAll(pList);
	}

	if(bOK == FALSE)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
	}

#else /* XML Parsing Fail   */
	HERROR 						hErr = ERR_FAIL;
	//HxXMLDOC					hXmlDocHandle = NULL;
	//HxXMLELEMENT				hResponseElement = NULL;
	//HUINT8						*pszCmd = NULL;
	//HUINT8						*pszTemp = NULL;

	// Parse Content (XML)
	//hErr = HLIB_XML_InitParser();
	//hErr = HLIB_XML_ParseBuffer(pBuf, ulContentLen, &hXmlDocHandle);
	//hErr = HLIB_XML_GetRootElement(hXmlDocHandle, &hResponseElement);

	HxLOG_Debug("[%s][%d]: -\n", __FUNCTION__, __LINE__);

	//// <XXXX ...> Element name..
	//hErr = HLIB_XML_GetElementName(hResponseElement, &pszCmd);
	//if(hErr == ERR_OK)
	//{
	//	if((pszCmd != NULL) && (HLIB_STD_StrCaseCmp(pszCmd, "app") == 0))
	//	{
			hErr = HLIB_XML_GetAttrContents(hResponseElement, "sid", &pszTemp);
			if((hErr == ERR_OK) && (pszTemp != NULL))
			{
				HxSTD_MemSet(stAppsInfo.szSourceId, 0x00, RC_MAX_URL);
				HxSTD_snprintf(stAppsInfo.szSourceId, RC_MAX_URL, "%s", pszTemp);

				HxLOG_Debug("\t [%d]: sid: %s \n", __LINE__, stAppsInfo.szSourceId);

				HLIB_XML_FreeAttrContents(pszTemp);
				pszTemp = NULL;

				hErr = HLIB_XML_GetAttrContents(hResponseElement, "cid", &pszTemp);
				if((hErr == ERR_OK) && (pszTemp != NULL))
				{
					HxSTD_MemSet(stAppsInfo.szContentId, 0x00, RC_MAX_URL);
					HxSTD_snprintf(stAppsInfo.szContentId, RC_MAX_URL, "%s", pszTemp);

					HxLOG_Debug("\t [%d]: cid: %s \n", __LINE__, stAppsInfo.szContentId);

					HLIB_XML_FreeAttrContents(pszTemp);
					pszTemp = NULL;

					HxLOG_Debug("\t [%d]: send signal:onRcLaunchApp\n", __LINE__);

					HAPI_BroadcastSignal("signal:onRcLaunchApp", 0, "b", &stAppsInfo, sizeof(RC_Outputs_Apps_t));
					__rc_restsvc_send_response(conn, eRC_Header_Type2, 202, NULL, 0);

					goto EXIT_POST_APP;
				}
			}
	//	}
	//}

	__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);

EXIT_POST_APP:
	if(hXmlDocHandle)
	{
		HLIB_XML_FreeDocument(hXmlDocHandle);
	}
#endif

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	return ERR_OK;
}


HERROR __rc_restsvc_get_ca(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	rc_rest_service_context_t 	*ctx = __context();

	HxLOG_Debug("\t\t+++++ [%s][%d] %s \n", __FUNCTION__, __LINE__, conn->uri);

	HAPI_BroadcastSignal("signal:onRcRequestCaInfo", 0, "s", conn->uri);
	ctx->bReply = FALSE;

	return ERR_FAIL;
}

HERROR __rc_restsvc_send_ca(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HUINT32						ulLen = 0;
	HUINT8						*pBuf = NULL;
	HERROR						hErr = ERR_FAIL;

	HxLOG_Debug("\t\t+++++ [%s][%d] %s \n", __FUNCTION__, __LINE__, conn->uri);

	pBuf = (HUINT8 *)HLIB_STD_MemAlloc(RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n", __FUNCTION__, __LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		return ERR_OK;
	}

	HxSTD_MemSet(pBuf, 0x00, RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);

	hErr = Get_RC_CaInfo(pBuf, (HUINT8*)conn->uri, &ulLen, param1);
	if(hErr == ERR_OK)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type1, 200, pBuf, ulLen);
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	return ERR_OK;
}



HERROR __rc_restsvc_get_power(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HUINT32						ulSavingPower, ulLen = 0;
	HUINT8						*pBuf = NULL;
	HERROR						hErr = ERR_FAIL;

	rc_rest_service_context_t 	*ctx = __context();

	pBuf = (HUINT8 *)HLIB_STD_MemAlloc(RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n", __FUNCTION__, __LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		return ERR_OK;
	}

	(void)HAPI_GetRegistryInt(RC_DB_POWERSAVING_ON_STANDBY, &ulSavingPower);
	if(ulSavingPower == 0)
	{
		ctx->bEnableActiveStandby = TRUE;
	}
	else
	{
		ctx->bEnableActiveStandby = FALSE;
	}

	HxSTD_MemSet(pBuf, 0x00, RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);

	hErr = Get_RC_Power(pBuf, &ulLen, ctx->bOperating, ctx->bEnableActiveStandby, ctx->ulStandbyTime);
	if(hErr == ERR_OK)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type1, 200, pBuf, ulLen);
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	return ERR_OK;
}

HERROR __rc_restsvc_post_power(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HERROR 		hErr = ERR_FAIL;
	HCHAR 		*pBuf = NULL;
	HUINT32 	ulSavingPower, ulContentLen = 0;

	rc_rest_service_context_t 	*ctx = __context();

	(void)HAPI_GetRegistryInt(RC_DB_POWERSAVING_ON_STANDBY, &ulSavingPower);
	if(ulSavingPower == 0)
	{
		ctx->bEnableActiveStandby = TRUE;
	}
	else
	{
		ctx->bEnableActiveStandby = FALSE;
	}

	ulContentLen = conn->content_len + 1;		// NULL ('\0') included

	pBuf = (HCHAR*)HLIB_STD_MemAlloc(sizeof(HCHAR) * ulContentLen);

	HxSTD_MemSet(pBuf, 0x00, ulContentLen);
	HxSTD_MemCopy(pBuf, conn->content, conn->content_len);

	HxLOG_Debug("\t\t+++++ post data (LEN:%d)\n\n%s\n\n", ulContentLen, pBuf);

	// 1. conn->content 의 XML Parsing
	// 2. Parsing된 내용의 VK_XXX 를 DIKS_XXX로 변환하여 HAPI_SendRemoteRCUKey() param 으로 전달

	hErr = Post_RC_Power(pBuf, ulContentLen, ctx->bOperating, ctx->bEnableActiveStandby);
	if(hErr == ERR_OK)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 202, NULL, 0);
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, NULL, 0);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	return ERR_OK;
}

HERROR __rc_restsvc_get_time(struct mg_connection *conn, void *param1, void *param2, void *param3)
{
	HUINT32						ulLen = 0;
	HUINT8						*pBuf = NULL;
	HERROR						hErr = ERR_FAIL;

	HUINT32		ulRcvTime = HLIB_STD_GetSystemTime();

	pBuf = (HUINT8 *)HLIB_STD_MemAlloc(RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);
	if(pBuf == NULL)
	{
		HxLOG_Print("[%s:%d] Memory Allocation Fail \n", __FUNCTION__, __LINE__);
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
		return ERR_OK;
	}

	HxSTD_MemSet(pBuf, 0x00, RC_RESOURCE_SHORT_MAX_BUF_SIZE+1);

	hErr = Get_RC_Time(pBuf, &ulLen, ulRcvTime);
	if(hErr == ERR_OK)
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type1, 200, pBuf, ulLen);
	}
	else
	{
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 503, pBuf, 0);
	}

	if(pBuf)
	{
		HLIB_STD_MemFree(pBuf);
	}

	return ERR_OK;
}

HBOOL __rc_restsvc_compare_uri(void *user_data, void *list_data)
{
	HCHAR *pUri = (HCHAR*)user_data;
	rc_rest_service_desc_t *pRestSvcDesc = (rc_rest_service_desc_t*)list_data;

	HxLOG_Debug("\t\t+++++ check point ##2, pURI : %s, root path : %s\n", pUri, pRestSvcDesc->szRootPath);

	if(HLIB_STD_StrStartWith(pUri, pRestSvcDesc->szRootPath) == TRUE)
	{
		return TRUE;
	}

	return FALSE;
}

static void	__rc_restsvc_sendRequestData(struct mg_connection *conn)
{
	HxList_t					*pList;
	rc_rest_service_context_t 	*ctx = __context();
	rc_rest_service_desc_t *pRestSvcDesc = NULL;

	pList = HLIB_LIST_FindEx(ctx->pRestSvcList, (const void*)conn->uri, __rc_restsvc_compare_uri);
	if(pList)
	{
		pRestSvcDesc = (rc_rest_service_desc_t*)HLIB_LIST_Data(pList);
		if(pRestSvcDesc->OnReply_Send)
		{
			pRestSvcDesc->OnReply_Send(conn, ctx->pReplyData, (void*)ctx->nReplyInt, (void*)0);
		}
		else
		{
			// send error response ----> not support
			__rc_restsvc_send_response(conn, eRC_Header_Type2, 405, NULL, 0);
		}

	}

	if(ctx->pReplyData)
	{
		HLIB_STD_MemFree(ctx->pReplyData);
		ctx->pReplyData = NULL;
	}
	ctx->bReply = FALSE;

	return;
}

HERROR __rc_restsvc_processHTTPPacket(struct mg_connection *conn)
{
	HERROR hErr = ERR_FAIL;
	HBOOL 	bSecure = FALSE;
	rc_rest_service_context_t *ctx = __context();
	HxList_t		*pList;
	rc_rest_service_desc_t *pRestSvcDesc = NULL;
	HUINT32	param1 = 0, param2 = 0, param3 = 0;

#if __TEST_WITH_POSTMAN__
	bSecure = TRUE;
#else
	if(conn->local_port == RC_SSL_PORT)
	{
		bSecure = TRUE;
	}
#endif

	if(HLIB_STD_StrCaseCmp(conn->request_method, "GET") == 0)
	{
		HxLOG_Debug("\t\t+++++ __rc_restsvc_processHTTPPacket[%d] : GET %s\n", __LINE__, conn->uri);
		if(HLIB_STD_StrCaseCmp(conn->uri, "/desc.xml") == 0)
		{
			__rc_get_DeviceDescription(conn, (void*)param1, (void*)param2, (void*)param3);
		}
		else if(HLIB_STD_StrCaseCmp(conn->uri, "/scpd.xml") == 0)
		{
			__rc_get_ServiceDescription(conn, (void*)param1, (void*)param2, (void*)param3);
		}
		else if(HLIB_STD_StrCaseCmp(conn->uri, "/rc") == 0)		// ROOT API
		{
			if(bSecure == TRUE)
			{
				__rc_restsvc_get_resource(conn, (void*)param1, (void*)param2, (void*)param3);
			}
			else
			{
				// send error response ----> Forbidden
				__rc_restsvc_send_response(conn, eRC_Header_Type2, 403, NULL, 0);
			}
		}
		else
		{
			pList = HLIB_LIST_FindEx(ctx->pRestSvcList, (const void*)conn->uri, __rc_restsvc_compare_uri);
			if(pList)
			{
				// check authorization header
				hErr = __rc_restsvc_check_auth_header(conn);
				if(hErr != ERR_OK)
				{
					__rc_restsvc_send_response(conn, eRC_Header_Req_AuthToken, 401, NULL, 0);
				}
				else
				{
					pRestSvcDesc = (rc_rest_service_desc_t*)HLIB_LIST_Data(pList);

					if((HLIB_STD_StrCaseCmp(pRestSvcDesc->szRootPath, "/rc/ca") == 0) && (bSecure == FALSE))
					{
						// send error response ----> Forbidden
						__rc_restsvc_send_response(conn, eRC_Header_Type2, 403, NULL, 0);
					}
					else
					{
						if(pRestSvcDesc->OnGet_Response)
						{
							if(pRestSvcDesc->OnGet_Response(conn, (void*)param1, (void*)param2, (void*)param3) != ERR_OK)
							{
								return ERR_FAIL;
							}
						}
						else
						{
							// send error response ----> not support
							__rc_restsvc_send_response(conn, eRC_Header_Type2, 405, NULL, 0);
						}
					}
				}
			}
			else
			{
				// send error response ----> not support
				__rc_restsvc_send_response(conn, eRC_Header_Type2, 405, NULL, 0);
			}
		}
	}
	else if(HLIB_STD_StrCaseCmp(conn->request_method, "POST") == 0)
	{
		HxLOG_Debug("\t\t+++++ __rc_restsvc_processHTTPPacket[%d] : POST %s\n", __LINE__, conn->uri);
		if(HLIB_STD_StrCaseCmp(conn->uri, "/rc") == 0)		// ROOT API
		{
			if(bSecure == TRUE)
			{
				HCHAR		*pszAuth = NULL;

				// check authorization header
				pszAuth = (HCHAR*)mg_get_header(conn, "Authorization");
				if(pszAuth == NULL)
				{
					__rc_restsvc_send_response(conn, eRC_Header_Req_AuthToken, 401, NULL, 0);
				}
				else
				{
					__rc_restsvc_post_resource(conn, (void*)param1, (void*)param2, (void*)param3);
				}
			}
			else
			{
				// send error response ----> Forbidden
				__rc_restsvc_send_response(conn, eRC_Header_Type2, 403, NULL, 0);
			}
		}
		else
		{
			pList = HLIB_LIST_FindEx(ctx->pRestSvcList, (const void*)conn->uri, __rc_restsvc_compare_uri);
			if(pList)
			{
				hErr = __rc_restsvc_check_auth_header(conn);
				if(hErr != ERR_OK)
				{
					__rc_restsvc_send_response(conn, eRC_Header_Req_AuthToken, 401, NULL, 0);
				}
				else
				{
					pRestSvcDesc = (rc_rest_service_desc_t*)HLIB_LIST_Data(pList);
					if(pRestSvcDesc->OnPost_Action)
					{
						if(pRestSvcDesc->OnPost_Action(conn, (void*)param1, (void*)param2, (void*)param3) != ERR_OK)
						{
							return ERR_FAIL;
						}
					}
					else
					{
						// send error response ----> not support
						__rc_restsvc_send_response(conn, eRC_Header_Type2, 405, NULL, 0);
					}
				}
			}
			else
			{
				// send error response ----> not support
				__rc_restsvc_send_response(conn, eRC_Header_Type2, 405, NULL, 0);
			}
		}
	}
	else
	{
		HxLOG_Debug("\t\t+++++ __rc_restsvc_processHTTPPacket[%d] : Other..\n", __LINE__);
		// send error response ----> not support
		__rc_restsvc_send_response(conn, eRC_Header_Type2, 405, NULL, 0);
	}

	return ERR_OK;
}


static void __rc_restsvc_initContext(HCHAR *pszFriendlyName)
{
	HERROR hErr = ERR_FAIL;
	rc_rest_service_context_t *ctx = __context();
	rc_rest_service_client_info_t *pstClientInfo = NULL;
	HCHAR szTemp[RC_MAX_PATH_LEN] = {0,};

	if(NULL == pszFriendlyName)
	{
		hErr = HAPI_GetRegistryStr(RC_SERVER_NAME, szTemp, sizeof(szTemp));
		if( (hErr == ERR_OK) && (HLIB_STD_StrCaseCmp(szTemp, "") != 0) )
		{
			HxSTD_MemCopy(ctx->pszFriendlyName, szTemp, RC_MAX_PATH_LEN);
		}
	}
	else
	{
		// Friendly Name
		HxSTD_MemSet(ctx->pszFriendlyName, 0x00, RC_MAX_PATH_LEN);
		if(HxSTD_StrLen(pszFriendlyName) < RC_MAX_PATH_LEN)
		{
			HxSTD_MemCopy(ctx->pszFriendlyName, pszFriendlyName, HxSTD_StrLen(pszFriendlyName));
		}
		else
		{
			HxSTD_MemCopy(ctx->pszFriendlyName, pszFriendlyName, (RC_MAX_PATH_LEN-1));
		}
		HAPI_SetRegistryStr(RC_SERVER_NAME, ctx->pszFriendlyName);
	}
	HxLOG_Debug("\t\t+++++ [%d] Friendly name : %s\n", __LINE__, ctx->pszFriendlyName);

	// Client Info (Last paired Device에 대한 registry 정보를 가져온다)
	pstClientInfo = (rc_rest_service_client_info_t*)HLIB_STD_MemAlloc(sizeof(rc_rest_service_client_info_t));
	if(pstClientInfo == NULL)
	{
		return;
	}

	HxSTD_MemSet(pstClientInfo, 0x00, sizeof(rc_rest_service_client_info_t));

	hErr = HAPI_GetRegistryStr(RC_CURRENT_PAIRED_CLIENT_ID, szTemp, sizeof(szTemp));
	if( (hErr == ERR_OK) && (HLIB_STD_StrCaseCmp(szTemp, "") != 0) )
	{
		HxSTD_MemCopy(pstClientInfo->szClientID, szTemp, RC_MAX_PATH_LEN);
		HxLOG_Debug("\t\t+++++ [%s][%d] %s\n", __FUNCTION__, __LINE__, pstClientInfo->szClientID);
	}
	else
	{
		hErr = ERR_FAIL;
	}

	hErr |= HAPI_GetRegistryStr(RC_CURRENT_PAIRED_CLIENT_NAME, szTemp, sizeof(szTemp));
	if( (hErr == ERR_OK) && (HLIB_STD_StrCaseCmp(szTemp, "") != 0) )
	{
		HxSTD_MemCopy(pstClientInfo->szClientName, szTemp, RC_MAX_PATH_LEN);
		HxLOG_Debug("\t\t+++++ [%s][%d] %s\n", __FUNCTION__, __LINE__, pstClientInfo->szClientName);
	}
	else
	{
		hErr = ERR_FAIL;
	}

	hErr |= HAPI_GetRegistryStr(RC_CURRENT_PAIRED_CLIENT_AUTH_TOKEN, szTemp, sizeof(szTemp));
	if( (hErr == ERR_OK) && (HLIB_STD_StrCaseCmp(szTemp, "") != 0) )
	{
		HxSTD_MemCopy(pstClientInfo->szAuthToken, szTemp, RC_MAX_PATH_LEN);
		HxLOG_Debug("\t\t+++++ [%s][%d] %s\n", __FUNCTION__, __LINE__, pstClientInfo->szAuthToken);
	}
	else
	{
		hErr = ERR_FAIL;
	}

	if(hErr == ERR_OK)
	{
		pstClientInfo->bPaired = TRUE;
		HxLOG_Debug("\t\t+++++ [%s][%d]\n", __FUNCTION__, __LINE__);
	}
	else
	{
		HxLOG_Debug("\t\t+++++ [%s][%d]\n", __FUNCTION__, __LINE__);
		HxSTD_MemSet(pstClientInfo->szClientID, 0x00, RC_MAX_PATH_LEN);
		HxSTD_MemSet(pstClientInfo->szClientName, 0x00, RC_MAX_PATH_LEN);
		HxSTD_MemSet(pstClientInfo->szAuthToken, 0x00, RC_MAX_PATH_LEN);
		pstClientInfo->bPaired = FALSE;
	}

	ctx->pstCurrentClientInfo = pstClientInfo;
	ctx->bReply = FALSE;
	ctx->pReplyData = NULL;
	ctx->nReplyInt = 0;

}

static void __rc_restsvc_registerObject(RC_Module_e eModule)
{
	rc_rest_service_context_t *ctx = __context();

	switch(eModule)
	{
		case eRC_Module_Locale:
			{
				rc_rest_service_desc_t *pRestSvcDesc = NULL;

				pRestSvcDesc = (rc_rest_service_desc_t*)HLIB_STD_MemAlloc(sizeof(rc_rest_service_desc_t));
				if(pRestSvcDesc == NULL)
				{
					return;
				}
				HxSTD_MemSet(pRestSvcDesc, 0x0, sizeof(rc_rest_service_desc_t));

				pRestSvcDesc->usRootPathLen = HxSTD_snprintf(pRestSvcDesc->szRootPath, RC_RESOURCE_PATH_MAX_LEN, RC_REST_SVC_LOCALE);
				pRestSvcDesc->OnGet_Response = __rc_restsvc_get_locale;
				pRestSvcDesc->OnPost_Action = __rc_restsvc_post_locale;

				ctx->pRestSvcList = HLIB_LIST_Append(ctx->pRestSvcList, pRestSvcDesc);
			}
			break;

		case eRC_Module_Remote:
			{
				rc_rest_service_desc_t *pRestSvcDesc = NULL;

				pRestSvcDesc = (rc_rest_service_desc_t*)HLIB_STD_MemAlloc(sizeof(rc_rest_service_desc_t));
				if(pRestSvcDesc == NULL)
				{
					return;
				}
				HxSTD_MemSet(pRestSvcDesc, 0x0, sizeof(rc_rest_service_desc_t));

				pRestSvcDesc->usRootPathLen = HxSTD_snprintf(pRestSvcDesc->szRootPath, RC_RESOURCE_PATH_MAX_LEN, RC_REST_SVC_REMOTE);
				pRestSvcDesc->OnGet_Response = __rc_restsvc_get_remote;
				pRestSvcDesc->OnPost_Action = __rc_restsvc_post_remote;
				pRestSvcDesc->OnReply_Send = NULL;

				ctx->pRestSvcList = HLIB_LIST_Append(ctx->pRestSvcList, pRestSvcDesc);
			}
			break;

		case eRC_Module_Events:
			{
				rc_rest_service_desc_t *pRestSvcDesc = NULL;

				pRestSvcDesc = (rc_rest_service_desc_t*)HLIB_STD_MemAlloc(sizeof(rc_rest_service_desc_t));
				if(pRestSvcDesc == NULL)
				{
					return;
				}
				HxSTD_MemSet(pRestSvcDesc, 0x0, sizeof(rc_rest_service_desc_t));

				pRestSvcDesc->usRootPathLen = HxSTD_snprintf(pRestSvcDesc->szRootPath, RC_RESOURCE_PATH_MAX_LEN, RC_REST_SVC_EVENTS);
				pRestSvcDesc->OnGet_Response = __rc_restsvc_get_events;
				pRestSvcDesc->OnPost_Action = NULL;
				pRestSvcDesc->OnReply_Send = NULL;

				ctx->pRestSvcList = HLIB_LIST_Append(ctx->pRestSvcList, pRestSvcDesc);
			}
			break;

		case eRC_Module_Outputs:
			{
				rc_rest_service_desc_t *pRestSvcDesc = NULL;

				pRestSvcDesc = (rc_rest_service_desc_t*)HLIB_STD_MemAlloc(sizeof(rc_rest_service_desc_t));
				if(pRestSvcDesc == NULL)
				{
					return;
				}
				HxSTD_MemSet(pRestSvcDesc, 0x0, sizeof(rc_rest_service_desc_t));

				pRestSvcDesc->usRootPathLen = HxSTD_snprintf(pRestSvcDesc->szRootPath, RC_RESOURCE_PATH_MAX_LEN, RC_REST_SVC_OUTPUTS);
				pRestSvcDesc->OnGet_Response = __rc_restsvc_get_outputs;
				pRestSvcDesc->OnPost_Action = __rc_restsvc_post_outputs;
				pRestSvcDesc->OnReply_Send = __rc_restsvc_send_outputs;

				ctx->pRestSvcList = HLIB_LIST_Append(ctx->pRestSvcList, pRestSvcDesc);
			}
			break;

		case eRC_Module_Sourcelists:
			{
				rc_rest_service_desc_t *pRestSvcDesc = NULL;

				pRestSvcDesc = (rc_rest_service_desc_t*)HLIB_STD_MemAlloc(sizeof(rc_rest_service_desc_t));
				if(pRestSvcDesc == NULL)
				{
					return;
				}
				HxSTD_MemSet(pRestSvcDesc, 0x0, sizeof(rc_rest_service_desc_t));

				pRestSvcDesc->usRootPathLen = HxSTD_snprintf(pRestSvcDesc->szRootPath, RC_RESOURCE_PATH_MAX_LEN, RC_REST_SVC_SOURCELISTS);
				pRestSvcDesc->OnGet_Response = __rc_restsvc_get_sourcelists;
				pRestSvcDesc->OnPost_Action = __rc_restsvc_post_sources;
				pRestSvcDesc->OnReply_Send = __rc_restsvc_send_sources;

				ctx->pRestSvcList = HLIB_LIST_Append(ctx->pRestSvcList, pRestSvcDesc);
			}
			break;


		case eRC_Module_Sources:
			{
				rc_rest_service_desc_t *pRestSvcDesc = NULL;

				pRestSvcDesc = (rc_rest_service_desc_t*)HLIB_STD_MemAlloc(sizeof(rc_rest_service_desc_t));
				if(pRestSvcDesc == NULL)
				{
					return;
				}
				HxSTD_MemSet(pRestSvcDesc, 0x0, sizeof(rc_rest_service_desc_t));

				pRestSvcDesc->usRootPathLen = HxSTD_snprintf(pRestSvcDesc->szRootPath, RC_RESOURCE_PATH_MAX_LEN, RC_REST_SVC_SOURCES);
				pRestSvcDesc->OnGet_Response = __rc_restsvc_get_sources;
				pRestSvcDesc->OnPost_Action = __rc_restsvc_post_sources;
				pRestSvcDesc->OnReply_Send = __rc_restsvc_send_sources;

				ctx->pRestSvcList = HLIB_LIST_Append(ctx->pRestSvcList, pRestSvcDesc);
			}
			break;

		case eRC_Module_Apps:
			{
				rc_rest_service_desc_t *pRestSvcDesc = NULL;

				pRestSvcDesc = (rc_rest_service_desc_t*)HLIB_STD_MemAlloc(sizeof(rc_rest_service_desc_t));
				if(pRestSvcDesc == NULL)
				{
					return;
				}
				HxSTD_MemSet(pRestSvcDesc, 0x0, sizeof(rc_rest_service_desc_t));

				pRestSvcDesc->usRootPathLen = HxSTD_snprintf(pRestSvcDesc->szRootPath, RC_RESOURCE_PATH_MAX_LEN, RC_REST_SVC_APPS);
				pRestSvcDesc->OnGet_Response = NULL;
				pRestSvcDesc->OnPost_Action = __rc_restsvc_post_apps;
				pRestSvcDesc->OnReply_Send = NULL;

				ctx->pRestSvcList = HLIB_LIST_Append(ctx->pRestSvcList, pRestSvcDesc);
			}
			break;

		case eRC_Module_CA:
			{
				rc_rest_service_desc_t *pRestSvcDesc = NULL;

				pRestSvcDesc = (rc_rest_service_desc_t*)HLIB_STD_MemAlloc(sizeof(rc_rest_service_desc_t));
				if(pRestSvcDesc == NULL)
				{
					return;
				}
				HxSTD_MemSet(pRestSvcDesc, 0x0, sizeof(rc_rest_service_desc_t));

				pRestSvcDesc->usRootPathLen = HxSTD_snprintf(pRestSvcDesc->szRootPath, RC_RESOURCE_PATH_MAX_LEN, RC_REST_SVC_CA);
				pRestSvcDesc->OnGet_Response = __rc_restsvc_get_ca;
				pRestSvcDesc->OnPost_Action = NULL;
				pRestSvcDesc->OnReply_Send = __rc_restsvc_send_ca;

				ctx->pRestSvcList = HLIB_LIST_Append(ctx->pRestSvcList, pRestSvcDesc);
			}
			break;

		case eRC_Module_Power:
			{
				rc_rest_service_desc_t *pRestSvcDesc = NULL;

				pRestSvcDesc = (rc_rest_service_desc_t*)HLIB_STD_MemAlloc(sizeof(rc_rest_service_desc_t));
				if(pRestSvcDesc == NULL)
				{
					return;
				}
				HxSTD_MemSet(pRestSvcDesc, 0x0, sizeof(rc_rest_service_desc_t));

				pRestSvcDesc->usRootPathLen = HxSTD_snprintf(pRestSvcDesc->szRootPath, RC_RESOURCE_PATH_MAX_LEN, RC_REST_SVC_POWER);
				pRestSvcDesc->OnGet_Response = __rc_restsvc_get_power;
				pRestSvcDesc->OnPost_Action = __rc_restsvc_post_power;
				pRestSvcDesc->OnReply_Send = NULL;

				ctx->pRestSvcList = HLIB_LIST_Append(ctx->pRestSvcList, pRestSvcDesc);
			}
			break;

		case eRC_Module_Time:
			{
				rc_rest_service_desc_t *pRestSvcDesc = NULL;

				pRestSvcDesc = (rc_rest_service_desc_t*)HLIB_STD_MemAlloc(sizeof(rc_rest_service_desc_t));
				if(pRestSvcDesc == NULL)
				{
					return;
				}
				HxSTD_MemSet(pRestSvcDesc, 0x0, sizeof(rc_rest_service_desc_t));

				pRestSvcDesc->usRootPathLen = HxSTD_snprintf(pRestSvcDesc->szRootPath, RC_RESOURCE_PATH_MAX_LEN, RC_REST_SVC_TIME);
				pRestSvcDesc->OnGet_Response = __rc_restsvc_get_time;
				pRestSvcDesc->OnPost_Action = NULL;
				pRestSvcDesc->OnReply_Send = NULL;

				ctx->pRestSvcList = HLIB_LIST_Append(ctx->pRestSvcList, pRestSvcDesc);
			}
			break;

		default:
			break;
	}
}

static void __rc_restsvc_initObject(void)
{
	// 1. Project 별로 정의된 XML 파일을 읽어와서 Parsing (TODO)
	// 2. module별 registerObject()
	__rc_restsvc_registerObject(eRC_Module_Remote);
	__rc_restsvc_registerObject(eRC_Module_Events);
	__rc_restsvc_registerObject(eRC_Module_Outputs);
	__rc_restsvc_registerObject(eRC_Module_Sourcelists);
	__rc_restsvc_registerObject(eRC_Module_Sources);
	__rc_restsvc_registerObject(eRC_Module_Apps);
	__rc_restsvc_registerObject(eRC_Module_CA);
	__rc_restsvc_registerObject(eRC_Module_Power);
	__rc_restsvc_registerObject(eRC_Module_Time);

}

static void __rc_restsvc_deinitObject(void)
{
	rc_rest_service_context_t *ctx = __context();

	if(ctx->pRestSvcList)
	{
		HLIB_LIST_Foreach(ctx->pRestSvcList, __rc_restsvc_memfree);
		HLIB_LIST_RemoveAll(ctx->pRestSvcList);
	}

	if(ctx->pstCurrentClientInfo)
	{
		HLIB_STD_MemFree(ctx->pstCurrentClientInfo);
	}
}

static int __request_cb(struct mg_connection *conn, enum mg_event ev)
{
	rc_rest_service_context_t 	*ctx = __context();

#if defined(CONFIG_DEBUG)
	int i;

	if(ev == MG_REQUEST) // || ev == MG_AUTH)
	{
		HxLOG_Debug("\t\t+++++ check point ##0, ev:%d, num_headers : %d, server param : %s, remote port : %d, local port : %d\n", ev, conn->num_headers, (char*)conn->server_param,
																										conn->remote_port, conn->local_port);
		for(i=0; i<conn->num_headers; i++)
		{
			HxLOG_Debug("\t\t\t+++++ [%s] [%s]\n", conn->http_headers[i].name, conn->http_headers[i].value);
		}
		HxLOG_Debug("\t\t #### conn->request_method : %s, conn->uri: %s\n", conn->request_method, conn->uri);
	}
#endif

	switch (ev)
	{
		case MG_AUTH:
			return MG_TRUE;
		case MG_REQUEST:
			HxLOG_Error("MG_REQUEST\n");
			if(__rc_restsvc_processHTTPPacket(conn) != ERR_OK)
			{
				do
				{
					HLIB_STD_TaskSleep(20);
				}while(FALSE == ctx->bReply);

				__rc_restsvc_sendRequestData(conn);
				return MG_TRUE;
			}
			else
			{
				return MG_TRUE;
			}
			break;
		case MG_POLL:
			HxLOG_Error("MG_POLL\n");
			break;
		case MG_HTTP_ERROR:
			HxLOG_Error("MG_HTTP_ERROR\n");
			break;
		case MG_CLOSE:
			HxLOG_Error("MG_CLOSE\n");
			break;
		default:
			break;
	}

	return MG_FALSE;
}

static HERROR	__rc_server_onRcReplyOutputs(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	rc_rest_service_context_t 	*ctx = __context();
	RC_Outputs_Info_t			*pData;

	HxLOG_Assert(nArgc == 1);

	pData = (RC_Outputs_Info_t*)HxOBJECT_BIN_DATA(apArgv[0]);
	ctx->bReply = TRUE;
	ctx->pReplyData = HLIB_STD_MemAlloc(sizeof(RC_Outputs_Info_t));

	HxSTD_MemCopy(ctx->pReplyData, pData, sizeof(RC_Outputs_Info_t));

	HxLOG_Debug("\t\t #### __rc_signal_onRcReplyOutputs : %d, %d, %d\n", pData->stSvcInfo.ulComponentNum, pData->stSvcInfo.bAvailable, pData->stSvcInfo.usSourceId);

	return ERR_OK;
}

static HERROR	__rc_server_onRcReplySources(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	rc_rest_service_context_t 	*ctx = __context();
	HINT32						nSrcNum = 0;
	RC_Sources_Info_t			*pData;

	HxLOG_Assert(nArgc == 2);

	nSrcNum = HxOBJECT_INT(apArgv[0]);
	pData = (RC_Sources_Info_t*)HxOBJECT_BIN_DATA(apArgv[1]);

	ctx->bReply = TRUE;
	ctx->nReplyInt = nSrcNum;
	ctx->pReplyData = HLIB_STD_MemAlloc(sizeof(RC_Sources_Info_t) * nSrcNum);

	HxSTD_MemCopy(ctx->pReplyData, pData, (sizeof(RC_Sources_Info_t) * nSrcNum));

	HxLOG_Debug("\t\t #### __rc_server_onRcReplySources : %d, %d, %s\n", pData->usSid, pData->usLcn, pData->szSvcName);

	return ERR_OK;
}

static HERROR __rc_server_onFactoryDefault(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HERROR hRet = ERR_OK;

	hRet = __rc_restsvc_unpair_client_info(NULL, TRUE);

	return hRet;
}

static HERROR	__rc_server_onRcReplyCaInfo(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	rc_rest_service_context_t 	*ctx = __context();
	RC_CA_Info_t			*pData;

	HxLOG_Assert(nArgc == 1);

	pData = (RC_CA_Info_t*)HxOBJECT_BIN_DATA(apArgv[0]);

	ctx->bReply = TRUE;
	ctx->pReplyData = HLIB_STD_MemAlloc(sizeof(RC_CA_Info_t));

	HxSTD_MemCopy(ctx->pReplyData, pData, sizeof(RC_CA_Info_t));

	HxLOG_Debug("\t\t #### __rc_server_onRcReplyCaInfo : %s, %s\n", pData->aucCsId, pData->aucCaVender);

	return ERR_OK;
}

static HERROR	__rc_server_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	rc_rest_service_context_t 	*ctx = __context();
	RC_CA_Info_t			*pData;

	HxLOG_Assert(nArgc == 1);

	pData = (RC_CA_Info_t*)HxOBJECT_BIN_DATA(apArgv[0]);

	ctx->bReply = TRUE;
	ctx->pReplyData = HLIB_STD_MemAlloc(sizeof(RC_CA_Info_t));

	HxSTD_MemCopy(ctx->pReplyData, pData, sizeof(RC_CA_Info_t));

	HxLOG_Debug("\t\t #### __rc_server_onRcReplyCaInfo : %s, %s\n", pData->aucCsId, pData->aucCaVender);

	return ERR_OK;
}


#define ________________Public_Interfaces__________________________
HERROR PL_RC_RestService_Start(HCHAR *pszFriendlyName, struct RCClientCallbacks *client_cb)
{
	rc_rest_service_context_t 	*ctx = __context();
	HUINT8 	 					 szPort[64] = {0,};
	HINT8						*szErr = NULL;
	HUINT32						ulSavingPower;


	HxSTD_MemSet(ctx, 0,  sizeof(rc_rest_service_context_t));

	// Init Context
	__rc_restsvc_initContext(pszFriendlyName);
	__rc_restsvc_initObject();

	// Create and configure the server
	ctx->server1 = mg_create_server("server1", __request_cb);
	ctx->server2 = mg_create_server("server2", __request_cb);
	ctx->server3 = mg_create_server("server3", __request_cb);

	mg_set_option(ctx->server1, "document_root", RC_PEMFILE_ROOT);
	mg_set_option(ctx->server2, "document_root", RC_PEMFILE_ROOT);
	mg_set_option(ctx->server3, "document_root", RC_PEMFILE_ROOT);

	HxSTD_snprintf(szPort, sizeof(szPort), "%d", RC_PORT);
	szErr = mg_set_option(ctx->server1, "listening_port", szPort);
	if(szErr)
	{
		HxLOG_Error(" Set listening (HTTP)port => Fail..!!(%s)\n", szErr);
	}

	szErr = mg_set_option(ctx->server2, "listening_port", szPort);
	if(szErr)
	{
		HxLOG_Error(" Set listening (HTTP)port => Fail..!!(%s)\n", szErr);
	}

	mg_set_listening_socket(ctx->server2, mg_get_listening_socket(ctx->server1));

	HxSTD_snprintf(szPort, sizeof(szPort), "%d", RC_SSL_PORT);
	szErr = mg_set_option(ctx->server3, "listening_port", szPort);
	if(szErr)
	{
		HxLOG_Error(" Set listening (HTTPS)port => Fail..!!(%s)\n", szErr);
	}

	szErr = mg_set_option(ctx->server3, "ssl_certificate", RC_PEMFILE_NAME);
	if(szErr)
	{
		HxLOG_Error(" Set cert => Fail..!!(%s)\n", szErr);
	}

	szErr = mg_set_option(ctx->server3, "ssl_ca_certificate", RC_CAFILE_NAME);
	if(szErr)
	{
		HxLOG_Error(" Set ca-cert => Fail..!!(%s)\n", szErr);
	}

	if(VK_SEM_Create(&ctx->lock, "rc_server_sema", VK_SUSPENDTYPE_PRIORITY) != ERR_OK)
	{
		HxLOG_Error("Cann't start rc rest server. (sema error) \n");
		return ERR_FAIL;
	}

	HxLOG_Debug("\t Starting on port => HTTP(%s:%s), HTTPS(%s) \n", mg_get_option(ctx->server1, "listening_port"), mg_get_option(ctx->server2, "listening_port"), mg_get_option(ctx->server3, "listening_port"));

	ctx->client_cb = *client_cb;

	(void)HAPI_GetRegistryInt(RC_DB_POWERSAVING_ON_STANDBY, &ulSavingPower);
	if(ulSavingPower == 0)
	{
		ctx->bEnableActiveStandby = TRUE;
	}
	else
	{
		ctx->bEnableActiveStandby = FALSE;
	}
	ctx->bOperating = TRUE;
	ctx->ulStandbyTime = 0;

	HAPI_ConnectSignal("signal:onRcReplyOutputs", NULL, __rc_server_onRcReplyOutputs);
	HAPI_ConnectSignal("signal:onRcReplySources", NULL, __rc_server_onRcReplySources);
	HAPI_ConnectSignal("signal:onFactoryDefault", NULL, __rc_server_onFactoryDefault);
	HAPI_ConnectSignal("signal:onRcReplyCaInfo", NULL, __rc_server_onRcReplyCaInfo);

#if 0
	mg_start_thread(__rc_server, ctx->server1);
	mg_start_thread(__rc_server, ctx->server2);
#else
	VK_TASK_Create(__rc_restsvc_server, VK_TASK_PRIORITY_MW_REF, SIZE_16K, (const char*)"MongooseServerTask1", (void*)ctx->server1, (unsigned long *)&ctx->ulServerTaskId1, 0);
	VK_TASK_Create(__rc_restsvc_server, VK_TASK_PRIORITY_MW_REF, SIZE_16K, (const char*)"MongooseServerTask2", (void*)ctx->server2, (unsigned long *)&ctx->ulServerTaskId2, 0);
	VK_TASK_Create(__rc_restsvc_server, VK_TASK_PRIORITY_MW_REF, SIZE_16K, (const char*)"MongooseServerTask3", (void*)ctx->server3, (unsigned long *)&ctx->ulServerTaskId3, 0);

	VK_TASK_Start(ctx->ulServerTaskId1);
	VK_TASK_Start(ctx->ulServerTaskId2);
	VK_TASK_Start(ctx->ulServerTaskId3);
#endif
	return ERR_OK;
}

HERROR PL_RC_RestService_Stop(void)
{
	rc_rest_service_context_t *ctx = __context();

	VK_SEM_Destroy(ctx->lock);

#if 0		// TODO: crash point
	// Cleanup, and free server instance
	mg_destroy_server(&ctx->server1);
	mg_destroy_server(&ctx->server2);
	mg_destroy_server(&ctx->server3);

	// Destroy server task
	VK_TASK_Stop(ctx->ulServerTaskId1);
	VK_TASK_Stop(ctx->ulServerTaskId2);
	VK_TASK_Stop(ctx->ulServerTaskId3);

	VK_TASK_Destroy(ctx->ulServerTaskId1);
	VK_TASK_Destroy(ctx->ulServerTaskId2);
	VK_TASK_Destroy(ctx->ulServerTaskId3);
#endif

	// Destroy context
	__rc_restsvc_deinitObject();

	return ERR_OK;
}

HERROR	PL_RC_RestService_UnpairClient(void)
{
	return __rc_restsvc_unpair_client_info(NULL, TRUE);
}


HERROR 	PL_RC_RestService_SetPowerStatus(HBOOL bOperating)
{
	HUINT32		ulSavingPower;
	rc_rest_service_context_t *ctx = __context();

	ctx->bOperating = bOperating;
	(void)HAPI_GetRegistryInt(RC_DB_POWERSAVING_ON_STANDBY, &ulSavingPower);

	if(ulSavingPower == 0)	// Support Active Standby
	{
		ctx->bEnableActiveStandby = TRUE;
	}
	else
	{
		ctx->bEnableActiveStandby = FALSE;
	}

	if(FALSE == bOperating)
	{
		if(ctx->bEnableActiveStandby == FALSE)	// Support Active Standby
		{
			ctx->ulStandbyTime = HLIB_STD_GetSystemTime();
			RemoteCtrl_AppendEventLogs(RCEVT_TYPE_POWER, RCEVT_SUBTYPE_POWER_STANDBY, 0, 0, 0);
		}
		else
		{
			ctx->ulStandbyTime = 0;
		}
	}
	else
	{
		ctx->ulStandbyTime = 0;
		RemoteCtrl_AppendEventLogs(RCEVT_TYPE_POWER, RCEVT_SUBTYPE_POWER_ON, 0, 0, 0);
	}

	return ERR_OK;
}


HERROR	PL_RC_RestService_SetFriendlyName( const HCHAR *pszFriendlyName)
{
	rc_rest_service_context_t *ctx = __context();

	if(pszFriendlyName != NULL)
	{
		HxSTD_MemSet(ctx->pszFriendlyName, 0x00, RC_MAX_PATH_LEN);
		if(HxSTD_StrLen(pszFriendlyName) < RC_MAX_PATH_LEN)
		{
			HxSTD_MemCopy(ctx->pszFriendlyName, pszFriendlyName, HxSTD_StrLen(pszFriendlyName));
		}
		else
		{
			HxSTD_MemCopy(ctx->pszFriendlyName, pszFriendlyName, (RC_MAX_PATH_LEN-1));
		}

		if(HAPI_SetRegistryStr(RC_SERVER_NAME, ctx->pszFriendlyName) == ERR_OK)
		{
			RemoteCtrl_AppendEventLogs(RCEVT_TYPE_RECEIVER, RCEVT_SUBTYPE_RESOURCE_NAME, 0, 0, 0);
		}
	}

	return ERR_OK;
}

