/*************************************************************************************************************
	File 		: pal_samba_client.h
	author 		: IPM tipmsw1@humaxdigital.com
	comment		: 
	date    	: 2012/12/10
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

#ifndef	__PAL_SAMBA_CLIENT_H__
#define	__PAL_SAMBA_CLIENT_H__

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
	ePAL_SAMBAC_Result_Ok,
	ePAL_SAMBAC_Result_Error,
	ePAL_SAMBAC_Result_InvalidParam,
	ePAL_SAMBAC_Result_AlreadyStarted,
	ePAL_SAMBAC_Result_Working,
} PAL_SAMBAC_Result_e;

typedef enum
{
	ePAL_SAMBAC_Unknown,
} PAL_SAMBAC_ServerState_e;

typedef enum
{
	ePAL_SAMBAC_ALL,
	ePAL_SAMBAC_MOUNTLIST,
	ePAL_SAMBAC_UNMOUNTLIST,
} PAL_SAMBAC_ServerMountList_e;

typedef enum
{
	ePAL_SAMBAC_EVT_START,
	ePAL_SAMBAC_EVT_STOP,
	ePAL_SAMBAC_EVT_ADD,
	ePAL_SAMBAC_EVT_REMOVE,
	ePAL_SAMBAC_EVT_END,
	ePAL_SAMBAC_EVT_MOUNT,
	ePAL_SAMBAC_EVT_UMOUNT,
} PAL_SAMBAC_Event_e;

typedef enum
{
	ePAL_SAMBAC_MAX_MOUNT			= 128,
	
	ePAL_SAMBAC_MAX_IP_LEN			= 16,
	ePAL_SAMBAC_MAX_ID_LEN			= 64,
	ePAL_SAMBAC_MAX_PATH_LEN		= 512,
	ePAL_SAMBAC_MAX_PASSWORD_LEN	= 64,
	ePAL_SAMBAC_MAX_LOG_LEN			= 128,
	ePAL_SAMBAC_MAX_FOLDER_LEN		= 128,
	ePAL_SAMBAC_MAX_SRVNAME_LEN		= 64,
	
	ePAL_SAMBAC_MAX_CMD_LEN			= 512,
	ePAL_SAMBAC_MAX_STDOUT_LEN		= 128,
} PAL_SAMBAC_MAX_e;

typedef struct
{
	HCHAR 				szServerName[ePAL_SAMBAC_MAX_SRVNAME_LEN];
	HCHAR				szSharedName[ePAL_SAMBAC_MAX_FOLDER_LEN];
	HCHAR 				szIp[ePAL_SAMBAC_MAX_IP_LEN];	
	HCHAR 				szId[ePAL_SAMBAC_MAX_ID_LEN];		
	HCHAR 				szPassword[ePAL_SAMBAC_MAX_PASSWORD_LEN];
	HCHAR 				szMountPath[ePAL_SAMBAC_MAX_PATH_LEN];
	HCHAR				szLogMsg[ePAL_SAMBAC_MAX_LOG_LEN];
	HBOOL   			bMounted;
	HINT32				nState;	
} PAL_SAMBAC_ServerInfo_t;

typedef struct
{
	HBOOL				bInit;
	HBOOL				bScanning;
	HxList_t			*pstSharedList;
} PAL_SAMBAC_Context_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

typedef void (*PAL_SAMBAC_Listener)(HINT32 event, void *pServerInfo, HBOOL flag);

// ap_mw interface
extern PAL_SAMBAC_Result_e	PAL_SAMBAC_StartScan( PAL_SAMBAC_Listener listener );
extern PAL_SAMBAC_Result_e	PAL_SAMBAC_StopScan( void );

extern PAL_SAMBAC_Result_e	PAL_SAMBAC_Mount( PAL_SAMBAC_ServerInfo_t *pServerInfo );
extern PAL_SAMBAC_Result_e	PAL_SAMBAC_UnMount( PAL_SAMBAC_ServerInfo_t *pServerInfo );
extern PAL_SAMBAC_Result_e	PAL_SAMBAC_UnMountAll( void );
extern PAL_SAMBAC_Result_e  PAL_SAMBAC_Disconnect(void);

#endif
/* end od file */

