/*************************************************************************************************************
	File 		: pal_woon_server.h
	author 		: IPM tipmsw1@humaxdigital.com
	comment		:
	date    	: 2012/10/24
	attention 	:

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Â¡Â°HumaxÂ¡Â±) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED Â¡Â°AS ISÂ¡Â± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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

#ifndef	__PAL_WOON_SERVER_H__
#define	__PAL_WOON_SERVER_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/* PAL_WOONÀÇ Return°ª Á¤ÀÇ */
typedef enum
{
	ePAL_WOONS_Result_Ok,
	ePAL_WOONS_Result_Error,
	ePAL_WOONS_Result_InvalidParam,
	ePAL_WOONS_Result_AlreadyStarted,
	ePAL_WOONS_Result_Working
} PAL_WOONS_Result_e;

typedef enum
{
	ePAL_MXRAS_EVENT_WID_QUERY,								// disable
	ePAL_MXRAS_EVENT_WID_REGISTER,							// disable
	ePAL_MXRAS_EVENT_WID_UNREGISTER,						// disable
	ePAL_MXRAS_EVENT_WID_GENERATE,							// disable

	ePAL_MXRAS_EVENT_NETWORK_REVERSE_CONNECTION,  			// enable
	ePAL_MXRAS_EVENT_NETWORK_EXPIRED_WID_OCCURED,			// disable
	ePAL_MXRAS_EVENT_NETWORK_CS_PUBLIC_IP,   				// disable // ConnectionServer  me Public IP
	ePAL_MXRAS_EVENT_NETWORK_LOCAL_IP,						// disable
	ePAL_MXRAS_EVENT_NETWORK_UPNP_PORT_FORWARDING,			// disable

	ePAL_MXRAS_EVENT_NETWORK_INFO_IDX,						// disable
	ePAL_MXRAS_EVENT_NETWORK_SETUP_IDX,						// disable
	ePAL_MXRAS_EVENT_NETWORK_REVERSE_IDX,					// disable

	ePAL_MXRAS_EVENT_WID_QUERY_PROGRASS,					// disable
	ePAL_MXRAS_EVENT_WID_REGISTER_PROGRASS,					// disable
	ePAL_MXRAS_EVENT_WID_UNREGISTER_PROGRASS,				// disable
	ePAL_MXRAS_EVENT_WID_GENERATE_PROGRASS,					// disable
	ePAL_MXRAS_EVENT_WID_LIST_IDX,							// disable
	ePAL_MXRAS_EVENT_WID_SET_DESCRIPTION_IDX,				// disable

	ePAL_MXRAS_EVENT_TOKEN,									// disable
	ePAL_MXRAS_EVENT_ACCOUNT_INFO,							// disable
	ePAL_MXRAS_EVENT_ACOUNT_IDX,							// disable
	ePAL_MXRAS_EVENT_FILES,									// disable
	ePAL_MXRAS_EVENT_METADATA,								// disable
	ePAL_MXRAS_EVENT_THUMBNAILS,							// disable
	ePAL_MXRAS_EVENT_FILEOPS_DELETE,						// disable
	ePAL_MXRAS_EVENT_FILEOPS_RENAME,						// disable
	ePAL_MXRAS_EVENT_FILEOPS_TRANSCODE_CHK,					// disable
	ePAL_MXRAS_EVENT_TRANSCODE,								// disable
	ePAL_MXRAS_EVENT_STREAMMING_SEND,
	ePAL_MXRAS_EVENT_SEARCH,								// disable
	ePAL_MXRAS_EVENT_GROUP,									// disable

	ePAL_MXRAS_EVENT_DEVICE_LIST,							// disable
	ePAL_MXRAS_EVENT_DEVICE_SET,							// disable
	ePAL_MXRAS_EVENT_DEVICE_RESET,							// disable
	ePAL_MXRAS_EVENT_DEVICE_MSEARCH,						// disable

	ePAL_MXRAS_EVENT_PAIRING_INIT,							// disable

	ePAL_MXRAS_EVENT_PROGRAM_VERSION,						// disable

	ePAL_MXRAS_EVENT_NOTIFICATION,							// disable

	ePAL_MXRAS_EVENT_DEVICE_ADD,							// disable
	ePAL_MXRAS_EVENT_DEVICE_REMOVE,							// disable
	ePAL_MXRAS_EVENT_SHARED_BROWSE,
	ePAL_MXRAS_EVENT_SHARED_SET,
	ePAL_MXRAS_EVENT_SHARED_RESET,
	ePAL_MXRAS_EVENT_SHARED_GET_LIST,

	ePAL_MXRAS_EVENT_PORT_FORWARDING,
	ePAL_MXRAS_EVENT_WID_SET_PASSWORD_IDX,

	ePAL_MXRAS_EVENT_WOONSTART_FAILED,						// private for only humax
} PAL_WOONS_EVENT_e;

typedef enum
{
	ePAL_MXRAS_EVENT_OK,
	ePAL_MXRAS_EVENT_ERROR,
	ePAL_MXRAS_EVENT_ERROR_CONNECTION_FAIL,
} PAL_WOONS_ERROR_CODE_e;

typedef enum
{
	ePAL_MXRAS_NOTIFICATION_NETWORK_INFO 				= 0,
	ePAL_MXRAS_NOTIFICATION_NETWORK_REVERSE				= 1,
	ePAL_MXRAS_NOTIFICATION_DEVICE_LIST,
	ePAL_MXRAS_NOTIFICATION_DEVICE_SET,
	ePAL_MXRAS_NOTIFICATION_DEVICE_RESET,
	ePAL_MXRAS_NOTIFICATION_DEVICE_MESEARCH,
	ePAL_MXRAS_NOTIFICATION_CODE_PAIRING 				= 1,
} PAL_WOONS_NOTIFY_e;

typedef enum
{
	ePAL_WOONS_MAX_SRVNAME_LEN	= 64,
	ePAL_WOONS_PAIRINGID_LEN	= 52,
	ePAL_WOON_PINCODE_LEN 		= 4,
	ePAL_WOON_ETH0				= 0,
} PAL_WOONS_MAX_e;

typedef struct
{
	void*			hDms;
	void*			hDmsDB;
	void*			hFs;
	HCHAR 			szServerName[ePAL_WOONS_MAX_SRVNAME_LEN];
} PAL_WOONS_DmsInfo_t;

/* WOON ¼­¹ö ±¸µ¿ ÈÄ Àü´Þ¹ÞÀ» ÀÌº¥Æ®, ÅëÁö ÄÝ¹é */
typedef void 	(*PAL_WOONS_EVENT_MW_CALLBACK)(PAL_WOONS_EVENT_e type, PAL_WOONS_ERROR_CODE_e error, void* data);
typedef void 	(*PAL_WOONS_COLLECTOR_MW_CALLBACK)(PAL_WOONS_NOTIFY_e notification_code);


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

// ap_mw interface
extern void					PAL_WOONS_AddEventCallback(PAL_WOONS_EVENT_MW_CALLBACK listener);
extern void 				PAL_WOONS_AddCollectorCallback(PAL_WOONS_COLLECTOR_MW_CALLBACK listener);
extern void 				PAL_WOONS_InitPairingId( void );
extern HCHAR * 				PAL_WOONS_GetPairingId( void );
extern void 				PAL_WOONS_Init(void);
extern void 				PAL_WOONS_Deinit(void);

extern PAL_WOONS_Result_e	PAL_WOONS_Start(PAL_WOONS_DmsInfo_t 	*pstDmsInfo );
extern PAL_WOONS_Result_e	PAL_WOONS_Stop(void);
extern void 				PAL_WOONS_TestConnection( void );
extern PAL_WOONS_Result_e	PAL_WOONS_RenameServer(HCHAR *pszServerName);
extern HCHAR* 				PAL_WOONS_GetWoonId(void);
extern PAL_WOONS_INFO_t* 	PAL_WOONS_GetServerInfo(void);
extern HBOOL 				PAL_WOONS_IsStarted(void);
extern HCHAR*				PAL_WOONS_GetPinCode( void );


// cmd_mw interface
extern PAL_WOONS_Result_e 	PAL_WOONS_AddWoonID( HCHAR *pszWoonID, HCHAR *pszPassword );
extern PAL_WOONS_Result_e 	PAL_WOONS_DelWoonID( HCHAR *pszWoonID );
extern PAL_WOONS_Result_e 	PAL_WOONS_GetWoonIDList( void );
extern PAL_WOONS_Result_e	PAL_WOONS_ShowNetworkInfo( void );
extern HCHAR*				PAL_WOONS_GetExternalIp(void);
extern HCHAR*				PAL_WOONS_GetPublicIp(void);
extern HINT16				PAL_WOONS_GetExternalPort(void);
extern HINT16 				PAL_WOONS_GetPublicPort(void);
extern HCHAR* 				PAL_WOONS_GetLocalIp(void);
extern void 				PAL_WOONS_GetDeviceList(void);


#endif
/* end od file */

