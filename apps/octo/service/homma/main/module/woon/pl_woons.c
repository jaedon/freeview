/*************************************************************************************************************
	File 		: pl_woons.c
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/10/24
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (징째Humax징짹) and
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
#include "pl_common.h"
#include "pl_woons.h"
#include "vkernel.h"
#include <netdb.h>
#include <mxdlna_dms.h>
#include <mxlib_config.h>
#include <mxRas.h>
//#include "transcoderControl.h"

#define ________________Defines____________________________________
#define PL_WOON_CONSERVER_IP			"woon.humaxtvportal.com"
#define PL_WOON_CONSERVER_PORT			80
#define PL_WOON_WEBSERVER_PORT			9251

#define PL_WOON_INTERNAL_MSGQ_SIZE					32
#define PL_WOON_INTERNAL_MSGQ_NAME					"WoonInternalMsgQ"
#define PL_WOON_TESTCON_TASK_NAME					"plWoonTestConnectionTask"

//1 ###### 인증서와 키 저장소 결정해야됨.
#define PL_WWON_PEMFILE_ROOT			"/usr/woon/"
#define PL_WOON_PEMFILE_NAME			PL_WWON_PEMFILE_ROOT"servcert.pem"
#define PL_WOON_KEYFILE_NAME			PL_WWON_PEMFILE_ROOT"servkey.pem"

/* mxlib stack에서 alloc()하였기 때문에 HLIB_STD_MemFree() 대신 free() 사용 */
#define UPDATE_NETINFO(src, dst) 										\
		{																\
			HINT16 nTempLength=0;										\
			HxSTD_MemSet(dst, 0x00, PL_WOONS_IP_MAX_LEN);				\
			nTempLength =  HxSTD_StrLen(src);							\
			if(nTempLength > PL_WOONS_IP_MAX_LEN)						\
			{															\
				nTempLength = PL_WOONS_IP_MAX_LEN-1;					\
			}															\
			HxSTD_StrNCpy(dst, src, nTempLength);						\
			MX_FREE( src );												\
			src = NULL;													\
		}

#define ________________Typedef____________________________________
typedef enum
{
	MSG_CMD_WOONS_SERVER_START = 0x00,
	MSG_CMD_WOONS_SERVER_STOP,
	MSG_CMD_WOONS_SERVER_RENAME,
	MSG_CMD_WOONS_START_TESTCONNECTION = 0xf0,
}WOONS_MSG_CMD_e;

typedef enum
{
	PL_WOONS_STATUS_STOPPED = 0x0,
	PL_WOONS_STATUS_STOPPING,
	PL_WOONS_STATUS_STARTING,
	PL_WOONS_STATUS_STARTED
}PL_WOONS_Status_e;

typedef struct Woon_Msg
{
	HUINT32					uiCommand;
	HINT32					nParam1;
}WOONS_Msg_t;

typedef struct
{
	HBOOL					bTestConnecting;
	PL_WOONS_Status_e		eStatus;
	HBOOL					bPrevStartFailed;
	HBOOL					bPrevStopFailed;
	HUINT32					uiTestConnectionEventCount;
	HUINT32					uiTestConnectionTryCount;
	HCHAR					szPinCode[PL_WOON_PINCODE_LEN+1];
	HCHAR					szPairingId[PL_WOONS_PAIRINGID_LEN];
	PL_WOONS_DmsInfo_t		stDmsInfo;
	PL_WOONS_INFO_t		stServerInfo;
} PL_WOONS_Context_t;

#define ________________static_Variables___________________________
static HULONG					s_woonServer_TaskID = 0l;
static HULONG					s_PL_WoonMsgQId = 0l;

static HBOOL					s_bTask_Created = FALSE;

static MXRAS_HANDLE 			s_hRas = NULL;
static PL_WOONS_Context_t 		s_PL_Woons_Context = {
										FALSE,                    /* test conn                            */
										PL_WOONS_STATUS_STOPPED, /* status                               */
										FALSE,                    /* prev failure for preventing crash    */
										FALSE,                    /* prev failure for deinit() ras handle */
										0,                        /* event count                          */
										0,                        /* trial count                          */
										{"0000"},                 /* PIN code                             */
										{""},                     /* Pairing ID                           */
										{NULL, NULL, NULL, ""},
										{0, "", "", "", 0, 0} };
static PL_WOONS_EVENT_CALLBACK		s_PL_Woons_EventListener = NULL;
static PL_WOONS_COLLECTOR_CALLBACK	s_PL_Woons_CollectorListener = NULL;


#define ________________Internal_APIs_Prototype____________________
static MXRAS_HANDLE 		__get_ras_handle( void );

static HCHAR* 				__generate_randomcode(void);
static HERROR				__print_status( HINT32 ePublicStatus );
static HINT32 				__get_event_type(MXRA_EVENT_TYPE type);
static void 				__check_event_error(HINT32 error);
static void 				__check_wid_status(void *data, HINT32 data_type);

static void 				__event_cb(void *handle, MXRA_EVENT_TYPE type, MXRA_EVENT_ERROR_CODE error,void *data, void* user);
static void 				__collect_event_cb(MXDLNA_DMS_HANDLE handle, DMS_DB_HANDLE h_db, HCHAR *deviceID, HCHAR *objectID, HCHAR *path, HINT32 error, HINT32 bContainer, HINT32 added);

static PL_WOONS_Context_t* 	__context(void);
static PL_WOONS_Context_t*	__init_context(PL_WOONS_DmsInfo_t *stDmsInfo);


static HCHAR*				__get_server_name(void);
static void					__update_network_info(void);
static HCHAR* 				__make_pincode(HINT32 error);
static void 				__notifier(HINT32 type, HINT32 error);

static HERROR 				__start(PL_WOONS_DmsInfo_t *param);
static HERROR				__stop(void);
static HBOOL 				__is_started(void);

#define ________________Internal_Functions_________________________
static HUINT32 __urandom(void)
{
  int fd = open("/dev/urandom", O_RDONLY);
  HINT32 r = 0;
  ssize_t ret = -1;

  if (fd < 0) {
    HxLOG_Error("[PL_WOONS][%s] Error> could not open /dev/urandom\n", __FUNCTION__);
    return 0;
  }

  ret = read(fd, &r, sizeof (r));
  if (ret < 0) {
    HxLOG_Error("[PL_WOONS][%s] Error> could not read /dev/urandom\n", __FUNCTION__);
  }

  if (fd >= 0) close(fd);
  return (HUINT32)r;
}

static MXRAS_HANDLE __get_ras_handle( void )
{
	if (s_hRas == NULL)
		HxLOG_Error("[PL_WOONS][%s] Error> invalid NULL ras handle!\n", __FUNCTION__);

	return s_hRas;
}

static HBOOL __is_started(void)
{
	if(NULL == s_hRas || NULL == s_hRas->hDms)
		return FALSE;

	return TRUE;
}

/* 현재 Status를 출력하기 위한 Debugging용 함수 */
static HERROR	__print_status( HINT32 ePublicStatus )
{
	HERROR	ret = ERR_FAIL;

	switch( ePublicStatus )
	{
		case MXRA_NETWORK_FAIL:
			HxLOG_Error(" ePublicStatus : MXRA_NETWORK_FAIL\n");
			ret = ERR_FAIL;
			break;

		case MXRA_NETWORK_CHECKING:
			HxLOG_Print(" ePublicStatus : MXRA_NETWORK_CHECKING\n");
			//ret = PL_WOONS_Result_Working;
			ret = ERR_OK;
			break;

		case MXRA_NETWORK_ERROR:
			HxLOG_Error(" ePublicStatus : MXRA_NETWORK_ERROR\n");
			ret = ERR_FAIL;
			break;

		case MXRA_NETWORK_SUCCESS:
			HxLOG_Print(" ePublicStatus : MXRA_NETWORK_SUCCESS\n");
			ret = ERR_OK;
			break;

		case MXRA_NETWORK_UNKNOWN:
			HxLOG_Print(" ePublicStatus : MXRA_NETWORK_UNKNOWN\n");
			ret = ERR_FAIL;
			break;

		case MXRA_NETWORK_FAIL_FIREWALL:
			HxLOG_Error(" ePublicStatus : MXRA_NETWORK_FAIL_FIREWALL\n");
			ret = ERR_FAIL;
			break;

		case MXRA_NETWORK_FAIL_NEED_MANUAL_FORWARDING:
			HxLOG_Print(" ePublicStatus : MXRA_NETWORK_FAIL_NEED_MANUAL_FORWARDING\n");
			ret = ERR_FAIL;
			break;

		case MXRA_NETWORK_FAIL_AUTO_MANUAL_FORWARDING:
			HxLOG_Print(" ePublicStatus : MXRA_NETWORK_FAIL_AUTO_MANUAL_FORWARDING\n");
			ret = ERR_FAIL;
			break;

		case MXRA_NETWORK_ERROR_PUBLIC_IP:
			HxLOG_Error(" ePublicStatus : MXRA_NETWORK_ERROR_PUBLIC_IP\n");
			ret = ERR_FAIL;
			break;
		default:
			HxLOG_Print(" ePublicStatus : UNKNOWN Status!!!!!!\n");
			break;
	}

	return ret;
}

static HINT32 __get_event_type(MXRA_EVENT_TYPE type)
{
	HINT32 ret_type = 0;
	HxLOG_Print("\t TYPE[%d] : ", type);
	switch( type )
	{
		case MXRA_EVENT_WID_QUERY:
			HxLOG_Print("MXRA_EVENT_QUERY\t");
			ret_type = 1;
			break;
		case MXRA_EVENT_WID_REGISTER:
			HxLOG_Print("MXRA_EVENT_REGISTER\t");
			ret_type = 1;
			break;
		case MXRA_EVENT_WID_UNREGISTER:
			HxLOG_Print("MXRA_EVENT_UNREGISTER\t");
			ret_type = 1;
			break;
		case MXRA_EVENT_WID_GENERATE:
			HxLOG_Print("MXRA_EVENT_GENERATE\t");
			ret_type = 1;
			break;
		case MXRA_EVENT_NETWORK_EXPIRED_WID_OCCURED:
			HxLOG_Print("MXRA_EVENT_EXPIRED_WID_OCCURED\t");
			ret_type = 1;
			break;
		case MXRA_EVENT_NETWORK_REVERSE_CONNECTION:
			HxLOG_Print("MXRA_EVENT_REVERSE_CONNECTION\t");
			break;
		case MXRA_EVENT_NETWORK_CS_PUBLIC_IP:
			HxLOG_Print("MXRA_EVENT_CS_PUBLIC_IP\t");
			break;
		case MXRA_EVENT_NETWORK_LOCAL_IP:
			HxLOG_Print("MXRA_EVENT_LOCAL_IP\t");
			break;
		case MXRA_EVENT_NETWORK_UPNP_PORT_FORWARDING:
			HxLOG_Print("MXRA_EVENT_UPNP_PORT_FORWARDING\t");
			break;
		case MXRA_EVENT_NETWORK_INFO_IDX:
			HxLOG_Print("MXRA_EVENT_NETWORK_INFO_IDX\t");
			break;
		case MXRA_EVENT_NETWORK_SETUP_IDX:
			HxLOG_Print("MXRA_EVENT_NETWORK_SETUP_IDX\t");
			break;
		case MXRA_EVENT_NETWORK_REVERSE_IDX:
			HxLOG_Print("MXRA_EVENT_NETWORK_REVERSE_IDX\t");
			break;
		case MXRA_EVENT_WID_QUERY_PROGRASS:
			HxLOG_Print("MXRA_EVENT_WID_QUERY_PROGRASS\t");
			break;
		case MXRA_EVENT_WID_REGISTER_PROGRASS:
			HxLOG_Print("MXRA_EVENT_WID_REGISTER_PROGRASS\t");
			break;
		case MXRA_EVENT_WID_UNREGISTER_PROGRASS:
			HxLOG_Print("MXRA_EVENT_WID_UNREGISTER_PROGRASS\t");
			break;
		case MXRA_EVENT_WID_GENERATE_PROGRASS:
			HxLOG_Print("MXRA_EVENT_WID_GENERATE_PROGRASS\t");
			break;
		case MXRA_EVENT_WID_LIST_IDX:
			HxLOG_Print("MXRA_EVENT_WID_LIST_IDX\t");
			break;
		case MXRA_EVENT_WID_SET_DESCRIPTION_IDX:
			HxLOG_Print("MXRA_EVENT_WID_SET_DESCRIPTION_IDX\t");
			break;
		case MXRA_EVENT_TOKEN:
			HxLOG_Print("MXRA_EVENT_TOKEN\t");
			break;
		case MXRA_EVENT_ACCOUNT_INFO:
			HxLOG_Print("MXRA_EVENT_ACCOUNT_INFO\t");
			break;
		case MXRA_EVENT_ACOUNT_IDX:
			HxLOG_Print("MXRA_EVENT_ACOUNT_IDX\t");
			break;
		case MXRA_EVENT_FILES:
			HxLOG_Print("MXRA_EVENT_FILES\t");
			break;
		case MXRA_EVENT_METADATA:
			HxLOG_Print("MXRA_EVENT_METADATA\t");
			break;
		case MXRA_EVENT_THUMBNAILS:
			HxLOG_Print("MXRA_EVENT_THUMBNAILS\t");
			break;
		case MXRA_EVENT_FILEOPS_DELETE:
			HxLOG_Print("MXRA_EVENT_FILEOPS_DELETE\t");
			break;
		case MXRA_EVENT_FILEOPS_RENAME:
			HxLOG_Print("MXRA_EVENT_FILEOPS_RENAME\t");
			break;
		case MXRA_EVENT_FILEOPS_TRANSCODE_CHK:
			HxLOG_Print("MXRA_EVENT_FILEOPS_TRANSCODE_CHK\t");
			break;
		case MXRA_EVENT_TRANSCODE:
			HxLOG_Print("MXRA_EVENT_TRANSCODE\t");
			break;
		case MXRA_EVENT_SEARCH:
			HxLOG_Print("MXRA_EVENT_SEARCH\t");
			break;
		case MXRA_EVENT_GROUP:
			HxLOG_Print("MXRA_EVENT_GROUP\t");
			break;
		case MXRA_EVENT_DEVICE_LIST:
			HxLOG_Print("MXRA_EVENT_DEVICE_LIST\t");
			break;
		case MXRA_EVENT_DEVICE_SET:
			HxLOG_Print("MXRA_EVENT_DEVICE_SET\t");
			break;
		case MXRA_EVENT_DEVICE_RESET:
			HxLOG_Print("MXRA_EVENT_DEVICE_RESET\t");
			break;
		case MXRA_EVENT_DEVICE_MSEARCH:
			HxLOG_Print("MXRA_EVENT_DEVICE_MSEARCH\t");
			break;
		case MXRA_EVENT_PAIRING_INIT:
			HxLOG_Print("MXRA_EVENT_PAIRING_INIT\t");
			break;
		case MXRA_EVENT_PROGRAM_VERSION:
			HxLOG_Print("MXRA_EVENT_PROGRAM_VERSION\t");
			break;
		case MXRA_EVENT_NOTIFICATION:
			HxLOG_Print("MXRA_EVENT_NOTIFICATION\t");
			break;
		case MXRA_EVENT_DEVICE_ADD:
			{
				HxLOG_Print("MXRA_EVENT_DEVICE_ADD\t\n");
				PL_WOONS_GetDeviceList();
			}
			break;
		case MXRA_EVENT_DEVICE_REMOVE:
			HxLOG_Print("MXRA_EVENT_DEVICE_REMOVE\t");
			break;
		case MXRA_EVENT_SHARED_BROWSE:
			HxLOG_Print("MXRA_EVENT_SHARED_BROWSE\t");
			break;
		case MXRA_EVENT_SHARED_SET:
			HxLOG_Print("MXRA_EVENT_SHARED_SET\t");
			break;
		case MXRA_EVENT_SHARED_RESET:
			HxLOG_Print("MXRA_EVENT_SHARED_RESET\t");
			break;
		case MXRA_EVENT_SHARED_GET_LIST:
			HxLOG_Print("MXRA_EVENT_SHARED_GET_LIST\t");
			break;
		case MXRA_EVENT_PORT_FORWARDING:
			HxLOG_Print("MXRA_EVENT_PORT_FORWARDING\t");
			break;

		default:
			break;

	}
	return ret_type;
}

static void __check_event_error(HINT32 error)
{
	HxLOG_Print("\n\t ERROR[%d] : ", error);

	switch( error )
	{
		case MXRA_EVENT_OK:
			HxLOG_Print("MXRA_EVENT_OK\t");
			break;
		case MXRA_EVENT_ERROR:
			HxLOG_Error("MXRA_EVENT_ERROR\t");
			break;
		case MXRA_EVENT_ERROR_CONNECTION_FAIL:
			HxLOG_Error("[PL_WOONS] Error> MXRA_EVENT_ERROR_CONNECTION_FAIL\t");
			break;
		default:
			break;
	}
}
static void __check_wid_status(void *data, HINT32 data_type)
{
	HxLOG_Print("\n\t DATA[%p] : ", data);
	if( data  && data_type == 1)
	{
		MXRA_WID_CONNECTION_DATA *recvData = data;
		if(recvData->wid != NULL)
			HxLOG_Print("wid=(%s)\t", recvData->wid );

		switch( recvData->status )
		{
			case MXRA_WID_STATUS_UNKNOWN:
				HxLOG_Print("MXRA_WID_STATUS_UNKNOWN\t");
				break;
			case MXRA_WID_STATUS_WATING_FOR_REPLY:
				HxLOG_Print("MXRA_WID_STATUS_WATING_FOR_REPLY\t");
				break;
			case MXRA_WID_STATUS_NOT_USED:
				HxLOG_Print("MXRA_WID_STATUS_NOT_USED\t");
				break;
			case MXRA_WID_STATUS_ALIVE_ONLINE:
				HxLOG_Print("MXRA_WID_STATUS_ALIVE_ONLINE\t");
				break;
			case MXRA_WID_STATUS_ALIVE_OFFLINE:
				HxLOG_Print("MXRA_WID_STATUS_ALIVE_OFFLINE\t");
				break;
			case MXRA_WID_STATUS_NOT_ALLOWED:
				HxLOG_Print("MXRA_WID_STATUS_NOT_ALLOWED\t");
				break;
			case MXRA_WID_STATUS_EXPIRED:
				HxLOG_Print("MXRA_WID_STATUS_EXPIRED\t");
				break;
			case MXRA_WID_STATUS_GENERATED:
				HxLOG_Print("MXRA_WID_STATUS_GENERATED\t");
				break;
			case MXRA_WID_STATUS_CONNECTION_ERROR:
				HxLOG_Print("MXRA_WID_STATUS_CONNECTION_ERROR\t");
				break;
			case MXRA_WID_STATUS_ARG_ERROR:
				HxLOG_Print("MXRA_WID_STATUS_ARG_ERROR\t");
				break;
			case MXRA_WID_STATUS_YOURS:
				HxLOG_Print("MXRA_WID_STATUS_YOURS\t");
				break;
			default:
				break;
		}
	}
}

static void	__update_network_info(void)
{
	PL_WOONS_Context_t *pCtx = __context();
	MXRAS_HANDLE hRas = __get_ras_handle();
	HCHAR *pLocalIP		= NULL;
	HCHAR *pExternalIp		= NULL;
	HCHAR *pPubilcIP		= NULL;
	HINT32 nPublicPort		= 0;
	HINT32 nExternalPort	= 0;

	if ((NULL == hRas) || (pCtx == NULL))
	{
		HxLOG_Print("[PL_WOONS] Error> invalid NULL handle!\n" );
		return;
	}
	pLocalIP		= mxRas_getLocalIp(hRas);
	pExternalIp		= mxRas_getExternalIp(hRas);
	pPubilcIP		= mxRas_getPublicIp(hRas);

	nPublicPort		= mxRas_getPublicPort(hRas);
	nExternalPort	= mxRas_getExternalPort(hRas);

	pCtx->stServerInfo.nPubPort = nPublicPort;
	pCtx->stServerInfo.nExtPort = nExternalPort;
	pCtx->stServerInfo.nState 	= mxRas_getPublicStatus(hRas);

	if( pLocalIP )
	{
		/*  woon memory corruption crash by MXRA_EVENT_NETWORK_CS_PUBLIC_IP event when being enabled mem-debug */
		UPDATE_NETINFO(pLocalIP, pCtx->stServerInfo.szLocalIp);
	}

	if( pExternalIp )
	{
		UPDATE_NETINFO(pExternalIp, pCtx->stServerInfo.szExtIp);
	}

	if( pPubilcIP )
	{
		UPDATE_NETINFO(pPubilcIP, pCtx->stServerInfo.szPubIp);
	}

#if 1 /* commented */
	HxLOG_Print("__update_network_info =============================\n");
	HxLOG_Print("[PL_WOONS] state      >> %d \n",pCtx->stServerInfo.nState);
	HxLOG_Print("[PL_WOONS] Local IP   >> %s:%d \n", pCtx->stServerInfo.szLocalIp, pCtx->stServerInfo.nPubPort);
	HxLOG_Print("[PL_WOONS] ExternIP   >> %s:%d \n", pCtx->stServerInfo.szExtIp, pCtx->stServerInfo.nExtPort);
	HxLOG_Print("[PL_WOONS] PublicIP   >> %s \n", pCtx->stServerInfo.szPubIp);
	HxLOG_Print("===================================================\n");
#endif
}

static HCHAR* __generate_randomcode(void)
{
	PL_WOONS_Context_t* pCtx = __context();
	HCHAR* pszPinCode = pCtx->szPinCode;
	HINT32 index = 0;
	HUINT32 nTemp =0;

	memset(pszPinCode, 0x00, sizeof(pCtx->szPinCode));
	srand((unsigned)time(NULL));
	for(index = 0; index < PL_WOON_PINCODE_LEN; index ++)
	{
	  //nTemp = rand()%10;
	  nTemp = __urandom()%10; /* Prevent special chars by /dev/random negative value */
		pszPinCode[index] = nTemp + 48;
	}

	return pszPinCode;
}

static HCHAR* __make_pincode(HINT32 error)
{
	HCHAR *pszPinCode = __generate_randomcode();
	if(MXRA_EVENT_OK == error)
	{
		MXRAS_HANDLE hRas = __get_ras_handle();
		if (NULL == hRas)
		{
			HxLOG_Error("Error> NULL ras handle!\n");
		}
		else
		{
			HxLOG_Error("PIN code[%s]\n", pszPinCode);
			mxRas_setParingCode(hRas, pszPinCode);
		}
	}
	return pszPinCode;
}

/* Woon Event 처리 관련 Callback */
static void __notifier(HINT32 type, HINT32 error)
{
	void *data = NULL;

	HxLOG_Print("\n");
	if(NULL != s_PL_Woons_EventListener)
	{
		switch(type)
		{
			/* async event result about TestConnection */
			case MXRA_EVENT_NETWORK_REVERSE_CONNECTION:
				{
					PL_WOONS_Context_t *pCtx 	= __context();

					HxLOG_Debug("[%s:%d] @@ MSG_CMD_WOONS_START_TESTCONNECTION @@ \n", __FUNCTION__, __LINE__);
					__update_network_info();
					error = __print_status(mxRas_getPublicStatus(__get_ras_handle()));
					pCtx->bTestConnecting = FALSE;
					pCtx->uiTestConnectionEventCount++;
				}
				break;

			case MXRA_EVENT_NOTIFICATION: // makes pin-code
				{
					data = (void*)__make_pincode(error);
				}
				break;

			case MXRA_EVENT_NETWORK_CS_PUBLIC_IP: /* Nothing TODO */
				{
#if 0 /* Temporarily removed because un-needed and crash flow */
					__update_network_info();
#endif
				}
				break;
		}

		HxLOG_Error("Woons_EventListener(%d, %d, %p)\n", type, error, data);
		s_PL_Woons_EventListener(type, error, data);
	}
}

static void __event_cb( void*  handle,
				 	 	MXRA_EVENT_TYPE type,
						MXRA_EVENT_ERROR_CODE error,
						void* data, 	//MXRAS_WID_CONNECTION_DATA* data, 	// void*
						void* user)		// void*
{
	HINT32 data_type = 0;
	HxLOG_Print("#################### Event Receive ####################\n");
	data_type = __get_event_type(type);
	__check_event_error(error);
	__check_wid_status(data, data_type);
	__notifier(type, error);
	HxLOG_Print("#######################################################\n\n");
}

/* MXRAS_COLLECT_EVENT_CALLBACK */
static void __collect_event_cb(MXDLNA_DMS_HANDLE handle,
	   							DMS_DB_HANDLE h_db,
								HCHAR *deviceID,
								HCHAR *objectID,
								HCHAR *path,
								HINT32 error,
								HINT32 bContainer,
								HINT32 added)
{
#ifdef TRANSCODER_SUPPORT
	HCHAR *url = NULL;
	HCHAR *name = NULL;

	HxLOG_Print("CollectorAddRemoveCallback objectID=(%s), path=(%s), error=(%d), bContainer=(%d), added=(%d)\n",\
		   	objectID, path, error, bContainer, added);

	if(bContainer == 0)
	{
		name = mxDlnaDms_CaiMakeThumbnailFileName(objectID);
		if(name)
		{
			if(added)
			{
				mxDlnaDms_CaiGetLocalUrl( path, &url);

				if(url)
				{
					mxDlnaDms_CaiInsertWaitingThumbnail(h_db, TRANSCODER_IPROFILE_TYPE_THUMBNAIL_URL, deviceID, objectID, path, name);
					TransCtrl_Thmbnl_AddQueue(deviceID, objectID, url, h_db, name, NULL, TRANSCODER_IPROFILE_TYPE_THUMBNAIL_URL, NULL);
					MX_FREE(url);
				}
			}
			else
			{
				TransCtrl_Thmbnl_RemoveQueue(deviceID, objectID);
				mxDlnaDms_CaiDeleteWaitingThumbnail(h_db, deviceID, objectID);
				mxDlnaDms_CaiRemoveThumbnail(handle, h_db, objectID);
			}
			MX_FREE(name);
		}
	}
#endif
}

static PL_WOONS_Context_t* __context(void)
{
	return &s_PL_Woons_Context;
}

static PL_WOONS_Context_t* __init_context(PL_WOONS_DmsInfo_t *stDmsInfo)
{
	s_PL_Woons_Context.stDmsInfo.hDms 		= stDmsInfo->hDms;
	s_PL_Woons_Context.stDmsInfo.hDmsDB 	= stDmsInfo->hDmsDB;
	s_PL_Woons_Context.stDmsInfo.hFs		= stDmsInfo->hFs;
	s_PL_Woons_Context.bTestConnecting		= FALSE;
	HxSTD_StrNCpy(s_PL_Woons_Context.stDmsInfo.szServerName, stDmsInfo->szServerName, PL_WOONS_MAX_SRVNAME_LEN-1);

	return &s_PL_Woons_Context;
}

static void __deinit_context(void)
{
	s_PL_Woons_Context.bTestConnecting = FALSE;
	s_PL_Woons_Context.eStatus = PL_WOONS_STATUS_STOPPED;
	s_PL_Woons_Context.bPrevStartFailed = FALSE;
	s_PL_Woons_Context.bPrevStopFailed = FALSE;
	s_PL_Woons_Context.uiTestConnectionEventCount = 0;
	s_PL_Woons_Context.uiTestConnectionTryCount = 0;
	HxSTD_memset(&s_PL_Woons_Context.szPinCode, '0', sizeof(HCHAR)*(PL_WOON_PINCODE_LEN));
	s_PL_Woons_Context.szPinCode[PL_WOON_PINCODE_LEN] = '\0';
	s_PL_Woons_Context.szPairingId[0] = '\0';
	HxSTD_memset(&s_PL_Woons_Context.stDmsInfo, 0, sizeof(PL_WOONS_DmsInfo_t));
	HxSTD_memset(&s_PL_Woons_Context.stServerInfo, 0, sizeof(PL_WOONS_INFO_t));
}


static HCHAR *__get_server_name(void)
{
	return s_PL_Woons_Context.stDmsInfo.szServerName;
}

static void __server_task( void *param)
{
	HERROR				hErr = ERR_FAIL;
	WOONS_Msg_t			tReceiveMsg = {0,};
	HINT32 				nPublicStatus = 0;
	MXRAS_HANDLE 		hRas = NULL;
	PL_WOONS_Context_t* pCtx = NULL;

	while (1)
	{
		hErr = VK_MSG_Receive(s_PL_WoonMsgQId, &tReceiveMsg, sizeof(WOONS_Msg_t));
		if ( hErr != VK_OK )
		{
			VK_TASK_Sleep(100);
			continue;
		}

		hRas = __get_ras_handle();
		pCtx = __context();

		HxLOG_Debug( "[PL_WOONS] MSG_CMD_WOONS_XXXXX processing (+)(0x%0x) .......... OK \n", tReceiveMsg.uiCommand );
		switch(tReceiveMsg.uiCommand) {
			case MSG_CMD_WOONS_SERVER_START :
				{
					if (__start(param) != ERR_OK)
					{
						HxLOG_Error( "[PL_WOONS] Error> Cant start Woons!\n" );
					}
					else
					{
						HxLOG_Print( "[PL_WOONS] MSG_CMD_WOONS_SERVER_START .......... OK \n" );
						__notifier(PL_MXRAS_EVENT_WOONSERVER_START , 0);
					}
				}
				break;

			case MSG_CMD_WOONS_SERVER_STOP :
				{
					if (__stop() != ERR_OK)
					{
						HxLOG_Error( "[PL_WOONS] Error> Cant stop Woons!\n" );
					}
					else
					{
						HxLOG_Print( "[PL_WOONS] MSG_CMD_WOONS_SERVER_STOP .......... OK \n" );
						__notifier(PL_MXRAS_EVENT_WOONSERVER_STOP , 0);
					}
				}
				break;

			case MSG_CMD_WOONS_SERVER_RENAME :
				{
					HCHAR *pszServerName = pCtx->stDmsInfo.szServerName;
					HxLOG_Print("[PL_WOONS] MSG_CMD_WOONS_SERVER_RENAME/name:%s[0x%0x]\n", pszServerName, hRas);
					if ( NULL == hRas )
						break;

					if(MXRA_OK == mxRas_setServerName(hRas, pszServerName))
					{
						HxLOG_Print( "[PL_WOONS] MSG_CMD_WOONS_SERVER_RENAME .......... OK \n" );
						hErr = ERR_OK;
					}
					else
					{
						HxLOG_Print( "[PL_WOONS] MSG_CMD_WOONS_SERVER_RENAME .......... Failed \n" );
						hErr = ERR_FAIL;
					}
					__notifier(PL_MXRAS_EVENT_WOONSERVER_RENAME, hErr);
				}
				break;

    		case MSG_CMD_WOONS_START_TESTCONNECTION :
	    		{
					if ((NULL == hRas) || (pCtx == NULL))
						HxLOG_Print("[PL_WOONS][%s] Error> invalid handle\n", __FUNCTION__);
					else
					{
						HxLOG_Debug("[%s:%d] @@ MSG_CMD_WOONS_START_TESTCONNECTION @@ \n", __FUNCTION__, __LINE__);

						/* Check reverseConnection event count */
						if (pCtx->uiTestConnectionEventCount == 0)
						{
							/* STB 부팅 이후 어떤 이유로 reverseConnection event가 없는 경우 직접 호출한다. */
							/* 테스트 결과는 __event_cb 통해서 올라옴 */
							mxRas_reverseConnection( hRas, NULL ); /* __Wmc_Invoke_ReverseConnectionEx(), MXRA_EVENT_NETWORK_REVERSE_CONNECTION */
							nPublicStatus = mxRas_getPublicStatus( hRas );
							__update_network_info();

							if(s_PL_Woons_EventListener)
							{
								HxLOG_Print("[PL_WOONS:%d] REVERSE_CONNECTION(ev:%d,%d)\n", __LINE__, pCtx->uiTestConnectionEventCount, pCtx->uiTestConnectionTryCount);
								s_PL_Woons_EventListener(MXRA_EVENT_NETWORK_REVERSE_CONNECTION, MXRA_EVENT_OK, NULL);
							}
							__print_status( nPublicStatus );
						}
						else
						{
							__update_network_info();
							if(s_PL_Woons_EventListener)
							{
								HxLOG_Print("[PL_WOONS:%d] REVERSE_CONNECTION(ev:%d,%d)\n", __LINE__, pCtx->uiTestConnectionEventCount, pCtx->uiTestConnectionTryCount);
								s_PL_Woons_EventListener(MXRA_EVENT_NETWORK_REVERSE_CONNECTION, MXRA_EVENT_OK, NULL);
							}

							/* 테스트 결과는 __event_cb 통해서 올라옴 */
							mxRas_reverseConnection( __get_ras_handle(), NULL );
							nPublicStatus = mxRas_getPublicStatus( __get_ras_handle() );
							__print_status( nPublicStatus );
						}
						pCtx->uiTestConnectionTryCount++;
					}
	    		}
	    		break;

    		default:
    			break;
		}
		HxLOG_Debug( "[PL_WOONS] MSG_CMD_WOONS_XXXXX processing (-)(0x%0x) .......... OK \n", tReceiveMsg.uiCommand );

    }
}

static	void __create_task( PL_WOONS_DmsInfo_t 	*pstDmsInfo )
{
	if ( TRUE == s_bTask_Created )
		return ;

	if( VK_OK !=  VK_MSG_Create(PL_WOON_INTERNAL_MSGQ_SIZE, sizeof(WOONS_Msg_t),
				PL_WOON_INTERNAL_MSGQ_NAME,
				&s_PL_WoonMsgQId,
				VK_SUSPENDTYPE_FIFO))
	{
		HxLOG_Error("[PL_WOONS:%s] internal message queue create failed.", __func__);
		return ;
	}

	if(VK_OK != VK_TASK_Create(__server_task,
				VK_TASK_PRIORITY_MW_REF,
				SIZE_64K,
				PL_WOON_TESTCON_TASK_NAME,
				(void*)pstDmsInfo,
				&s_woonServer_TaskID,
				FALSE) ||
			VK_OK != VK_TASK_Start(s_woonServer_TaskID))
	{
		HxLOG_Error("[PL_WOONS:%s] internal task create failed.", __func__);
		return ;
	}

	s_bTask_Created = TRUE;
}


static HERROR __start(PL_WOONS_DmsInfo_t *param)
{
	PL_WOONS_Context_t* 	pCtx = __context();
	struct hostent* pHostEnt = NULL;
	HBOOL bCheckHost = FALSE;
	//PL_WOONS_DmsInfo_t 	*pstDmsInfo = (PL_WOONS_DmsInfo_t*)param;

	HxLOG_Error("[PL_WOONS] ++start(status:%d)\n", pCtx->eStatus);

	if (pCtx->eStatus > PL_WOONS_STATUS_STOPPING)
	{
		HxLOG_Error("[PL_WOONS] Error> invalid status(%d)\n", pCtx->eStatus);
		goto func_error;
	}

	pCtx->eStatus = PL_WOONS_STATUS_STARTING;
	if (pCtx->bPrevStartFailed == TRUE)
	{
		HxLOG_Error("[PL_WOONS] Clear previous control failure(%d, %d)\n", pCtx->bPrevStartFailed, pCtx->bPrevStopFailed);
		pCtx->bPrevStartFailed = FALSE;
	}

#if 0 /* Removed because 30~40 (sec) blocking problem for ras_init() has been solved under no WAN connection or DNS resolving error */
	if (bCheckHost == TRUE)
	{
		/* Try host resolving first here for preventing blocking at mxRas_init()
		   because sometimes it takes too long time caused by network error like resolv.conf, gateway, ... */
		pHostEnt = (struct hostent *) gethostbyname(PL_WOON_CONSERVER_IP);
		if (pHostEnt == NULL)
		{
			HxLOG_Error("[PL_WOONS] Error> gethostbyname() failed! status(%d)\n", pCtx->eStatus);
			goto func_error;
		}
	}
#endif

	mxDlnaDms_CaiSetCallback( pCtx->stDmsInfo.hDms, __collect_event_cb, (MXDLNA_DMS_CAI_GET_HTTP_ADDRESS_FN)mxRas_device_getHttpAddress);

	/* Set Certification Location */
	mxRas_setSSLPath( PL_WOON_PEMFILE_NAME, PL_WOON_KEYFILE_NAME );

	//HxSTD_snprintf(pstDmsInfo->szPairingId, sizeof(pstDmsInfo->szPairingId),"%s", "00010203041a");
	if (PL_WOONS_InitPairingId() != TRUE)
	{
		HxLOG_Error("[PL_WOONS] Error> Cant initialize pairingID\n");
		goto func_error;
	}

	/* Init Ras */
	/* Check "Get Device XML" from device-spy: http://192.168.0.2:50001/ */
	/* <icube:X_WOON_PAIRING xmlns:icube="urn:schemas-icube-co-kr:WoonDevice-1-0">hmx_00010203da31</icube:X_WOON_PAIRING> */
	s_hRas = (MXRAS_HANDLE)mxRas_init(
									NULL, /* Removed mxRas_getPairingID() arg by dypark's guide */
									pCtx->stDmsInfo.hDms,
									pCtx->stDmsInfo.hFs,
									pCtx->stDmsInfo.hDmsDB,
									PL_WOON_CONSERVER_IP,
									PL_WOON_CONSERVER_PORT,
									PL_WOON_WEBSERVER_PORT,
#if defined(CONFIG_PROD_HMS1000T) /* TODO: Replace product's function config, not use model name */
									MXRAS_EXTERNAL_DISABLE_ALL, /* Check mxRas.h */
#else
									MXRAS_UPNP_PORT_FORWARDING,
#endif
									__event_cb,
									__collect_event_cb );
	if(s_hRas == NULL)
	{
		HxLOG_Error("[PL_WOONS] Error> mxRas_init() failed\n" );
		if(NULL != s_PL_Woons_EventListener)
			s_PL_Woons_EventListener(PL_MXRAS_EVENT_WOONSTART_FAILED, 0, NULL);
	}
	else
	{
		HxLOG_Print("FriendlyName  -------------------------------> %s\n",	pCtx->stDmsInfo.szServerName);
		mxRas_setServerName(s_hRas, pCtx->stDmsInfo.szServerName);
		HxLOG_Print("mxRas Handle  -------------------------------> %p\n",	s_hRas);
		HxLOG_Print("DMS Handle    -------------------------------> %p\n",	pCtx->stDmsInfo.hDms);
		HxLOG_Print("DMS DB Handle -------------------------------> %p\n",	pCtx->stDmsInfo.hDmsDB);
		HxLOG_Print("PAIRING ID    -------------------------------> %s\n",	mxRas_getPairingID());
		HxLOG_Print("PIN-CODE      -------------------------------> %s\n",	__make_pincode(MXRA_EVENT_OK));
	}
	pCtx->eStatus = PL_WOONS_STATUS_STARTED;
	HxLOG_Error("[PL_WOONS] --start\n" );
	return ERR_OK;

func_error:
	HxLOG_Error("[PL_WOONS] Error> WOONS start() failed! [status:%d]\n", pCtx->eStatus);
	if (pCtx->eStatus > PL_WOONS_STATUS_STOPPING)
	{
		pCtx->eStatus = PL_WOONS_STATUS_STOPPED;
	}
	pCtx->bPrevStartFailed = TRUE;
	return ERR_FAIL;
}

static HERROR	__stop(void )
{
	PL_WOONS_Context_t *pCtx = __context();
	HxLOG_Error("[PL_WOONS] ++stop(status:%d)\n", pCtx->eStatus);

	if (pCtx->eStatus == PL_WOONS_STATUS_STARTING)
	{
		HxLOG_Error("[PL_WOONS] Error> Cant continue stop() because start() is in progress! [status:%d]\n", pCtx->eStatus);
		pCtx->bPrevStopFailed = TRUE;
		return ERR_FAIL;
	}
	pCtx->eStatus = PL_WOONS_STATUS_STOPPING;

	if (pCtx->bPrevStopFailed == TRUE)
	{
		HxLOG_Error("[PL_WOONS] Clear previous control failure(%d, %d)\n", pCtx->bPrevStartFailed, pCtx->bPrevStopFailed);
		pCtx->bPrevStopFailed = FALSE;
	}

	if(NULL != s_hRas)
	{
		mxRas_deinit(s_hRas);
		s_hRas = NULL;
	}

	__deinit_context();

#if 0
	VK_ERR_CODE	eVKResult = VK_ERROR;
	eVKResult = VK_MSG_Destroy( s_PL_WoonMsgQId );
	if( VK_OK == eVKResult )
	{
		s_PL_WoonMsgQId = 0;
		HxLOG_Print( "[WOONS] VK_MSG_Destroy() .......... OK \n" );
	}
	eVKResult = VK_TASK_Stop( s_woonServer_TaskID );
	eVKResult = VK_TASK_Destroy( s_woonServer_TaskID );
	if(VK_OK == eVKResult)
	{
		s_woonServer_TaskID = 0;
		HxLOG_Print( "[WOONS] TestConnection VK_TASK_Destroy() .......... OK \n" );
	}

	if(VK_OK == eVKResult)
	{
		HxLOG_Print( "[WOONS] mainTask VK_TASK_Destroy() .......... OK \n" );
	}
	else
	{
		return ERR_FAIL;
	}
#endif

	pCtx->eStatus = PL_WOONS_STATUS_STOPPED;
	HxLOG_Error("[PL_WOONS] --stop(%d)\n", pCtx->eStatus);
	return ERR_OK;
}

#define ________________Public_Interfaces__________________________
HBOOL 	PL_WOONS_IsStarted(void)
{
	return __is_started();
}

HCHAR	*PL_WOONS_GetPairingId( void )
{
	/* Made by getting our pairingID, not mxRas_getPairingID() */
	/* Don't call mxRas_getPairingID() before DMS initializing */
	PL_WOONS_Context_t* pCtx = __context();
	HxLOG_Error("## WOON PairingId [%s] ##\n", pCtx->szPairingId);
	return pCtx->szPairingId;
}

HERROR	PL_WOONS_Start(PL_WOONS_DmsInfo_t 	*pstDmsInfo )
{
	WOONS_Msg_t tMsg = {0,};
	HERROR hErr;
	HxLOG_Error("[PL_WOONS] (%s)++\n", __FUNCTION__);

	if( TRUE == __is_started())
	{
		HxLOG_Error("Woon Server has already started \n");
		return ERR_FAIL;
	}

	if(NULL == pstDmsInfo->hDms || NULL == pstDmsInfo->hDmsDB)
	{
		HxLOG_Error("[PL_WOONS] Error> DMS information doesn't enough to start\n");
		return ERR_FAIL;
	}

	 /* 최초 1회만 생성 */
	__create_task(pstDmsInfo);

	__init_context(pstDmsInfo);

	tMsg.uiCommand = MSG_CMD_WOONS_SERVER_START;
	hErr = VK_MSG_Send( s_PL_WoonMsgQId, &tMsg, sizeof(WOONS_Msg_t) );
	if (VK_OK != hErr)
	{
		HxLOG_Error("[PL_WOONS][%s] Error> Message send fail for TestConnection (hErr:%d)\n", __FUNCTION__, hErr);
		return ERR_FAIL;
	}
	HxLOG_Print("[%s] Try to Start for Woon Server......\n", __FUNCTION__);
	HxLOG_Error("[PL_WOONS] (%s)--\n", __FUNCTION__);
	return ERR_OK;
}

HERROR	PL_WOONS_Stop(void)
{
	WOONS_Msg_t tMsg = {0,};
	HERROR		hErr = ERR_FAIL;
	HxLOG_Error("[PL_WOONS] (%s)++\n", __FUNCTION__);

	tMsg.uiCommand = MSG_CMD_WOONS_SERVER_STOP;
	hErr = VK_MSG_Send( s_PL_WoonMsgQId, &tMsg, sizeof(WOONS_Msg_t) );
	if (VK_OK != hErr)
	{
		HxLOG_Error("[PL_WOONS][%s] Error> Message send fail for STOP (hErr:%d)\n", __FUNCTION__, hErr);
		return ERR_FAIL;
	}
	HxLOG_Print("[%s] Try to Stop for Woon Server......\n", __FUNCTION__);
	HxLOG_Error("[PL_WOONS] (%s)--\n", __FUNCTION__);
	return ERR_OK;
}

HBOOL 	PL_WOONS_InitPairingId(void)
{
	HERROR		hErr = ERR_FAIL;
	PL_WOONS_Context_t* 	pCtx = __context();
	HCHAR 					szPairingId[PL_WOONS_PAIRINGID_LEN] = {0,};
	PL_COMMON_MacInfo_t	stMacInfo;

	HxSTD_MemSet(&stMacInfo, 0x00, sizeof(PL_COMMON_MacInfo_t));

	/* IP를 늦게 받거나 IP미할당 존재하므로 GetDefaultNetwork() 대신 MAC만 얻음 */
	hErr = PL_COMMON_GetEthMac(&stMacInfo);
	if (hErr != ERR_OK)
	{
		/* It's situation that should not occur absolutely.  */
		HxLOG_Error("[PL_WOONS][%s] Critical/Error> Cant get ethernet mac(err:%d)!\n", __FUNCTION__, hErr);

		/* Dont use this; It can't be ensured that the "Default" is same with "ethernet". */
		HxSTD_MemSet(&stMacInfo, 0x0, sizeof(PL_COMMON_MacInfo_t));
		hErr = PL_COMMON_GetDefaultMac(&stMacInfo);
		if (hErr != ERR_OK)
		{
			HxLOG_Error( "[PL_WOONS] Error> Cant get default mac(%s)!\n", stMacInfo.szMacAddr);
			HxSTD_PrintToStrN(szPairingId, sizeof(szPairingId), "%s", "hmx_nomac");
		}
		else
		{
			HxSTD_PrintToStrN(szPairingId, sizeof(szPairingId), "hmx_base_%s", stMacInfo.szMacAddr);
		}
	}
	else
	{
		HxSTD_PrintToStrN(szPairingId, sizeof(szPairingId), "hmx_base_%s", stMacInfo.szMacAddr);
	}
	HxLOG_Print("##########################################\n");
	HxLOG_Error("## WOON setPairingId [%s] ##\n", szPairingId);
	HxLOG_Print("##########################################\n");

	mxRas_setPairingID(szPairingId);

	HxSTD_PrintToStrN(pCtx->szPairingId, sizeof(pCtx->szPairingId), "%s", szPairingId);
	return TRUE;
}

void	PL_WOONS_AddEventCallback(PL_WOONS_EVENT_CALLBACK listener)
{
	s_PL_Woons_EventListener = listener;
}

void 	PL_WOONS_AddCollectorCallback(PL_WOONS_COLLECTOR_CALLBACK listener)
{
	s_PL_Woons_CollectorListener = listener;
}

HCHAR*	PL_WOONS_GetPinCode( void )
{
	PL_WOONS_Context_t* pCtx = __context();
	return pCtx->szPinCode;
}

/*********************************************************************/
/* Network 관련 Function */
/*********************************************************************/
/* 현재 Network 상태 확인 - SAMPLE : smp_ras_network_info() */
HERROR	PL_WOONS_ShowNetworkInfo( void )
{
	HCHAR 	*pLocalIP = NULL, *pExternalIp = NULL, *pPubilcIP = NULL;
	HINT32 	nPublicPort, nExternalPort, nPublicStatus;
	MXRAS_HANDLE hRas = __get_ras_handle();

	if(NULL == hRas)
	{
		HxLOG_Error("[%s] Error> Cant get ras handle!\n", __FUNCTION__);
		return ERR_FAIL;
	}

	pLocalIP		= mxRas_getLocalIp( hRas );
	pExternalIp		= mxRas_getExternalIp(hRas);
	pPubilcIP		= mxRas_getPublicIp( hRas );

	nPublicPort		= mxRas_getPublicPort( hRas);
	nExternalPort	= mxRas_getExternalPort(hRas );
	nPublicStatus	= mxRas_getPublicStatus(hRas );

	HxLOG_Print( "########################################################################\n" );
	HxLOG_Print( " pLocalIP : %s\n pExternalIp : %s\n pPubilcIP : %s\n nPublicPort : %d\n nExternalPort : %d\n",
			pLocalIP, pExternalIp, pPubilcIP, nPublicPort, nExternalPort );
	__print_status( nPublicStatus );
	HxLOG_Print( "########################################################################\n" );

	MX_FREE(pLocalIP);
	MX_FREE(pExternalIp);
	MX_FREE(pPubilcIP);

	return ERR_OK;
}

/* Reverse Connection Test -  Reverse connection test를 요청한다 */
/* Async return - through eventlistener */
void	PL_WOONS_TestConnection( void )
{
	PL_WOONS_Context_t *pCtx 	= __context();
	WOONS_Msg_t tMsg = {0,};
	HERROR hErr;

	tMsg.uiCommand = MSG_CMD_WOONS_START_TESTCONNECTION;

	if(TRUE == pCtx->bTestConnecting)
	{
		HxLOG_Print("[%s] Already do TestConnection !!! \n", __FUNCTION__);
		return;
	}
	pCtx->bTestConnecting = TRUE;

	HxLOG_Print("[%s:%d] Send Msg for TestConnection .1 \n", __FUNCTION__, __LINE__);

	hErr = VK_MSG_Send ( s_PL_WoonMsgQId, &tMsg, sizeof(WOONS_Msg_t) );
	if (VK_OK != hErr)
		HxLOG_Print("[PL_WOONS][%s] Error> Message send fail for TestConnection (hErr:%d)\n", __FUNCTION__, hErr);

	HxLOG_Print("[%s:%d] Send Msg for TestConnection .2 \n", __FUNCTION__, __LINE__);
}

HCHAR*	PL_WOONS_GetLocalIp(void)
{
	HCHAR *pLocalIP = NULL;
	PL_WOONS_Context_t *pCtx 	= __context();

	MXRAS_HANDLE hRas = __get_ras_handle();
	if(NULL == hRas)
		return "unknown";

	pLocalIP			= mxRas_getLocalIp( 	hRas );
	if(pLocalIP != NULL)
		UPDATE_NETINFO(pLocalIP, pCtx->stServerInfo.szLocalIp);

	return pCtx->stServerInfo.szLocalIp;
}

HCHAR*	PL_WOONS_GetExternalIp(void)
{
	HCHAR *pExternalIp			= NULL;
	PL_WOONS_Context_t *pCtx 	= __context();

	MXRAS_HANDLE hRas = __get_ras_handle();
	if(NULL == hRas)
		return "unknown";

	pExternalIp	= mxRas_getExternalIp(hRas);
	if(pExternalIp != NULL)
		UPDATE_NETINFO(pExternalIp, pCtx->stServerInfo.szExtIp);

	return pCtx->stServerInfo.szExtIp;
}

HCHAR*	PL_WOONS_GetPublicIp(void)
{
	HCHAR *pPubilcIP = NULL;
	PL_WOONS_Context_t *pCtx 	= __context();

	MXRAS_HANDLE hRas = __get_ras_handle();
	if(NULL == hRas)
		return "unknown";

	pPubilcIP			= mxRas_getPublicIp(hRas);
	if(pPubilcIP != NULL)
		UPDATE_NETINFO(pPubilcIP, pCtx->stServerInfo.szPubIp);

	return pCtx->stServerInfo.szPubIp;
}

HINT16	PL_WOONS_GetExternalPort(void)
{
	MXRAS_HANDLE hRas = __get_ras_handle();
	return (NULL == hRas) ? 0 : (HINT16)mxRas_getExternalPort(hRas);
}

HINT16	PL_WOONS_GetPublicPort(void)
{
	MXRAS_HANDLE hRas = __get_ras_handle();
	return (NULL == hRas) ? 0 : (HINT16)mxRas_getPublicPort(hRas);
}

PL_WOONS_INFO_t* PL_WOONS_GetServerInfo(void)
{
	PL_WOONS_Context_t *hContext = __context();
	PL_WOONS_INFO_t *pWoonsInfo = NULL;

	if(NULL != hContext)
		pWoonsInfo = &hContext->stServerInfo;

	return pWoonsInfo;
}

/* Add Woon ID - Connection 서버에 WID를 등록 및 저장 한다. */
HCHAR* 	PL_WOONS_GetWoonId(void)
{
	if( TRUE == __is_started())
		return (HCHAR*)mxRas_getPairingID();

	return NULL;
}

HERROR	PL_WOONS_AddWoonID( HCHAR *pszWoonID, HCHAR *pszPassword )
{
	if(NULL == pszWoonID || NULL == s_hRas || FALSE == __is_started())
		return ERR_FAIL;

	// 여기선 바로 DB에 등록 하지만 smpRas_registerWid을 해서 결과값을 sf_WidCallback으로 확인하고
	// smpRas_insertWid을 해서 DB에 데이터를 넣는다.
	HxLOG_Print("Try to add Woon id = %s\n", pszWoonID);
	if(mxRas_wid_register( __get_ras_handle(), pszWoonID, "-1", pszPassword, __get_server_name(), 4+2+1 ) == MXRA_ERROR)
		return ERR_FAIL;

	return ERR_OK;
}

/* Delete Woon ID - WID 삭제 */
HERROR	PL_WOONS_DelWoonID( HCHAR *pWoonID )
{
	if( pWoonID == NULL || FALSE == __is_started())
		return ERR_FAIL;

	// 여기선 바로 DB에 등록 하지만 smpRas_unregisterWid을 해서 결과값을 sf_WidCallback으로 확인하고
	// smpRas_removeWid을 해서 DB에 데이터를 지운다.
	mxRas_wid_unRegister( __get_ras_handle(), pWoonID);

	return ERR_OK;
}

/* PrHINT32 DMS List */
void	PL_WOONS_GetDeviceList(void)
{
	HINT32 i = 0;
	if(TRUE == __is_started())
	{
		MXRAS_DEVICE *device_list = mxRas_device_getList(NULL);
		MXRAS_DEVICE *temp = device_list;

		while(temp)
		{
			if(temp->onoff == MXRAS_DEVICE_STATUS_ON)
			{
				mxlog(0, "%d) %s %s ON %s %d %d\n", i, temp->name, temp->UDN, temp->ip, temp->port, temp->type);
				HxLOG_Print("\t%d) [ON] %s %s %s %d %d\n", i, temp->name, temp->UDN, temp->ip, temp->port, temp->type);
			}
			else
			{
				mxlog(0, "%d) %s %s OFF %d\n", i, temp->name, temp->UDN, temp->type);
				HxLOG_Print("\t%d) [OFF] %s %s %d\n", i, temp->name, temp->UDN, temp->type);
			}

			i++;
			temp = temp->pNext;
		}

		mxRas_device_freeList(device_list);
	}
}

/* Get Woon ID List - WoonID 리스트 확인 */
HERROR	PL_WOONS_GetWoonIDList( void )
{
	HINT32 result = 0;
	MXRAS_HANDLE hRas = __get_ras_handle();

	if(TRUE == __is_started() && NULL != hRas)
	{
		MXRA_WID_ARRAY arr;
		arr = mxRas_wid_getDatas( __get_ras_handle(),0, -1, &result, NULL );

		if( arr )
		{
			HINT32 idx = 0;

			HxLOG_Print( "########################################################################\n" );
			for( idx = 0 ; idx < result ; idx++ )
				HxLOG_Print( "%d) wid(%s), objectID(%s)\n", idx, arr[ idx ].wid, arr[ idx ].objectID );
			HxLOG_Print( "########################################################################\n" );

			mxRas_wid_freeDatas( arr, result );
		}
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR	PL_WOONS_RenameServer(HCHAR *pszServerName)
{
	PL_WOONS_Context_t *pCtx = __context();
	WOONS_Msg_t 		tMsg = {0,};
	MXRAS_HANDLE 		hRas = __get_ras_handle();
	HERROR 				hErr = ERR_FAIL;

	HxSTD_StrNCpy( pCtx->stDmsInfo.szServerName, pszServerName, sizeof(pCtx->stDmsInfo.szServerName)-1 );
	if( NULL == hRas )
	{
		return ERR_FAIL;
	}

	tMsg.uiCommand = MSG_CMD_WOONS_SERVER_RENAME;
	hErr = VK_MSG_Send( s_PL_WoonMsgQId, &tMsg, sizeof(WOONS_Msg_t) );
	if (VK_OK != hErr)
	{
		HxLOG_Error("[PL_WOONS][%s] Error> Message send fail for Server Rename (hErr:%d)\n", __FUNCTION__, hErr);
		return ERR_FAIL;
	}
	HxLOG_Print("[PL_WOONS][%s] Try to rename for Woon Server......(%s)(%s)\n", __FUNCTION__, pCtx->stDmsInfo.szServerName, pszServerName);

	return ERR_FAIL;
}
/* end od file */
