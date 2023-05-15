/*************************************************************************************************************
	File            : pl_rc.c
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
#include "pl_rc.h"

#include "mxdlna_userdevice.h"

#define ________________Defines____________________________________

#define PL_RC_SSDP_PORT (58801)
#define PL_RC_PORT 		(58802)

#if 0
#define	DIAL_DATA_MAX	4

#define APPS_URI 		"/apps/"
#define RUN_URI 		"/run"
#define DIAL_DATA_URI 	"/dialData?app="

typedef struct
{
	HCHAR 	name[16];
	HCHAR	value[32];
} dial_data_t;

typedef struct
{
    HCHAR 			*name;
	HBOOL			bIsReady;
    PL_DIAL_Status 	state;

	dial_data_t		data[DIAL_DATA_MAX];
	HUINT32			data_cnt;

	HCHAR			szAdditionalData[1024];

    struct 	DIALAppCallbacks callbacks;
} dial_app_t;
#endif

typedef struct
{
	HCHAR 		ip_addr[20];
	HCHAR		mac_addr[20];
	HULONG		lock;
	MXUSER_DEVICE_HANDLE  handle;
} rc_server_t;

#define ________________Static_Variables___________________________
static rc_server_t server;

#define ________________Internal_Functions_________________________
static rc_server_t * __server(void)
{
	return &server;
}

static void __lock(void)
{
	rc_server_t *context = __server();
	VK_SEM_Get(context->lock);
}

static void __unlock(void)
{
	rc_server_t *context = __server();
	VK_SEM_Release(context->lock);
}

static HERROR  __sync_wait(HUINT32 ulWaitTick, HBOOL *bCheck)
{
    HUINT32     starttick;
    HUINT32     duration;

    duration  = 0;
    starttick = HLIB_STD_GetSystemTick();
    while (duration < ulWaitTick)
    {
        HLIB_STD_TaskSleep(10);

		if(*bCheck == TRUE)
			break;

        duration = HLIB_STD_GetSystemTick() - starttick;
    }

    return ERR_OK;
}

#define ________________Public_Interfaces__________________________
#define	PL_RC_DEFAULT_UDN_PREFIX			"1E96FF1A-6771-2F44-H0C5-"
HERROR	PL_RC_start(HCHAR *pszFriendlyName, struct RCClientCallbacks *client_cb)
{
	HCHAR			szUdn[48] = {0, };

	rc_server_t 	*context	= __server();
	MXUSER_DEVICE_INFO_t device_info;
	MXUSER_EXTERNAL_SEARCHTARGET_INFO extSTInfo[3];

	PL_COMMON_ProductInfo_t	product_info;
	PL_COMMON_NetworkInfo_t	net_info;

	HxLOG_Error("[%s:%d] start(+)\n", __func__, __LINE__);

	HOM_MEMZERO(context, sizeof(rc_server_t));
	HOM_MEMZERO(&device_info, sizeof(MXUSER_DEVICE_INFO_t));


	PL_COMMON_GetProductInfo(&product_info);
	PL_COMMON_GetDefaultNetwork(&net_info);

	HxSTD_snprintf(szUdn, sizeof(szUdn), "%s%s", PL_RC_DEFAULT_UDN_PREFIX, net_info.szMacAddr);
	HxSTD_snprintf(context->ip_addr, sizeof(context->ip_addr), "%s", net_info.szIp);
	HxSTD_snprintf(context->mac_addr, sizeof(context->mac_addr), "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c",
			net_info.szMacAddr[0], net_info.szMacAddr[1], net_info.szMacAddr[2], net_info.szMacAddr[3],
			net_info.szMacAddr[4], net_info.szMacAddr[5], net_info.szMacAddr[6], net_info.szMacAddr[7],
			net_info.szMacAddr[8], net_info.szMacAddr[9], net_info.szMacAddr[10], net_info.szMacAddr[11]);

	HOM_MEMZERO(extSTInfo, sizeof(MXUSER_EXTERNAL_SEARCHTARGET_INFO) * 3);

	

	// External Search Target 등록		
	extSTInfo[0].ConfigId = 1;
	extSTInfo[0].ServerExt= "OPAL/1.0";
	extSTInfo[0].SearchTarget = "upnp:rootdevice";
	extSTInfo[0].ipAddress = NULL;  //context->ip_addr;//  // NULL로 설정할 경우 mxlib이 관리함
	extSTInfo[0].portNumber = PL_RC_PORT;
	extSTInfo[0].relativeURL = "/desc.xml";
	extSTInfo[0].UDN = szUdn;		//"DUMMY-UDN";
	extSTInfo[0].bSecureAccess = 0;   // SSDP message LOCATION 값의 prefix를 https 로 설정
	//extSTInfo.AdditionalField = "Name: STB-0001\r\nSampleField: Blah-blah";    // 필요한 경우 SSDP message에 필드 추가
	extSTInfo[0].next = &extSTInfo[1];    // 여러개 등록 가능	

	// External Search Target 등록		
	extSTInfo[1].ConfigId = 1;
	extSTInfo[1].ServerExt= "OPAL/1.0";
	extSTInfo[1].SearchTarget = "urn:humax:device:rcserver:1";
	extSTInfo[1].ipAddress = NULL;  //context->ip_addr;//  // NULL로 설정할 경우 mxlib이 관리함
	extSTInfo[1].portNumber = PL_RC_PORT;
	extSTInfo[1].relativeURL = "/desc.xml";
	extSTInfo[1].UDN = szUdn;		//"DUMMY-UDN";
	extSTInfo[1].bSecureAccess = 0;   // SSDP message LOCATION 값의 prefix를 https 로 설정
	//extSTInfo.AdditionalField = "Name: STB-0001\r\nSampleField: Blah-blah";    // 필요한 경우 SSDP message에 필드 추가
	extSTInfo[1].next = &extSTInfo[2];    // 여러개 등록 가능	

	extSTInfo[1].ConfigId = 1;
	extSTInfo[2].ServerExt= "OPAL/1.0";
	extSTInfo[2].SearchTarget = "urn:hdplus:service:opalrc-service:1";
	extSTInfo[2].ipAddress = NULL;  //context->ip_addr;//  // NULL로 설정할 경우 mxlib이 관리함
	extSTInfo[2].portNumber = PL_RC_PORT;
	extSTInfo[2].relativeURL = "/desc.xml";
	extSTInfo[2].UDN = szUdn;		//"DUMMY-UDN";
	extSTInfo[2].bSecureAccess = 0;   // SSDP message LOCATION 값의 prefix를 https 로 설정
	//extSTInfo.AdditionalField = "Name: STB-0001\r\nSampleField: Blah-blah";    // 필요한 경우 SSDP message에 필드 추가
	extSTInfo[2].next = NULL;    // 여러개 등록 가능	

	device_info.pExtSTInfo = extSTInfo;

	context->handle = mxUserDevice_createDevice(&device_info);
	if(context->handle == NULL)
	{
		HxLOG_Error("[%s:%d] Error> Mxdial handle is null !!! \n", __func__, __LINE__);
		return ERR_FAIL;
	}

	PL_RC_RestService_Start(pszFriendlyName, client_cb);

	mxUserDevice_startDevice(context->handle);

	(void)VK_SEM_Create(&context->lock, "rc_server_sema", VK_SUSPENDTYPE_FIFO);
	HxLOG_Error("[%s:%d] start(-)\n", __func__, __LINE__);
	return ERR_OK;
}

void PL_RC_stop(void)
{
	rc_server_t *context = __server();
	HxLOG_Error("[%s:%d] stop(+)\n", __func__, __LINE__);
	if (context->handle != NULL)
	{
		mxUserDevice_destroyDevice(context->handle);
	}

	PL_RC_RestService_Stop();

	(void)VK_SEM_Destroy(context->lock);
	HxLOG_Error("[%s:%d] stop(-)\n", __func__, __LINE__);
}

/*
HERROR 	PL_RC_SetApplicationURL(void)
{
	rc_server_t 	*context = __server();

	if (context != NULL)
	{
		return ERR_FAIL;
	}
	else
	{
		HCHAR szAppUrl[256] = {0, };
		PL_COMMON_NetworkInfo_t	net_info;
		PL_COMMON_GetDefaultNetwork(&net_info);

		HxSTD_snprintf(szAppUrl, sizeof(szAppUrl), "http://%s:%d/rc/", net_info.szIp, PL_RC_PORT);

		mxUserDevice_setApplicationURL(context->handle, szAppUrl);
	}

	return ERR_OK;
}
*/


HERROR	PL_RC_SetFriendlyName( const HCHAR *pszFriendlyName)
{
	if(HxSTD_StrEmpty(pszFriendlyName))
	{
		HxLOG_Error("[%s:%d] Error> FriendlyName is empty. \n", __func__, __LINE__);
		return ERR_FAIL;
	}
	
    PL_RC_RestService_SetFriendlyName(pszFriendlyName);

	return ERR_OK;
}

HERROR 	PL_RC_UnpairClient(void)
{
	return PL_RC_RestService_UnpairClient();
}

HERROR 	PL_RC_SetPowerStatus(HBOOL bOperating)
{
	return PL_RC_RestService_SetPowerStatus(bOperating);
}

