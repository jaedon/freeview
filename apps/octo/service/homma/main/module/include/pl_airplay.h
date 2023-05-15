/*************************************************************************************************************
	File            : pl_dmp.h
	author          : IPM tipmsw1@humaxdigital.com
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

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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

#ifndef __PL_AIRPLAY_H__
#define __PL_AIRPLAY_H__

#define	________________Header_Files_______________________________ 
#include <common.h>
#include <pl_common.h>

#define ________________Typedef____________________________________
#define	MAX_SERVICE_STRING			(128)

typedef enum
{
	PL_AIRPLAY_OP_PLAY,
	PL_AIRPLAY_OP_SETPOSITION,
	PL_AIRPLAY_OP_SETRATE,
	PL_AIRPLAY_OP_STOP,
	PL_AIRPLAY_OP_SHOWPHOTO,
	PL_AIRPLAY_OP_GET_PLAYBACK_INFO,
	PL_AIRTUNES_OP_PLAY,
	PL_AIRTUNES_OP_FLUSH,
	PL_AIRTUNES_OP_STOP,
	PL_AIRTUNES_OP_SET_METADATA,
	PL_AIRTUNES_OP_SET_ALBUMART
} PL_AIRPLAY_OP_TYPE;

typedef enum
{
	PL_AIRPLAY_PLAYBACK_NONE,
	PL_AIRPLAY_PLAYBACK_PAUSE,
	PL_AIRPLAY_PLAYBACK_LOADING,
	PL_AIRPLAY_PLAYBACK_PLAYING,
	PL_AIRPLAY_PLAYBACK_REACHED_EOF,
	PL_AIRPLAY_PLAYBACK_STOPPED,
	PL_AIRPLAY_PLAYBACK_ERROR
} PL_AIRPLAY_PLAYBACK_STATUS;

typedef struct PL_PLAYBACK_INFO {
	PL_AIRPLAY_PLAYBACK_STATUS ePlaybackStatus;
	HINT64 ullDuration;
	HINT64 ullStartPosition;
	HINT64 ullPosition;
	HINT64 ullBufferedRange;
	HINT64 ullSeekableRange;
	HINT32 unRate;
}PL_AIRPLAY_PLAYBACK_INFO_t;

typedef struct PL_AIRPLAY_METADATA {
	HCHAR			szTitle[128];
	HCHAR			szAlbum[128];
	HCHAR			szArtist[128];
}PL_AIRPLAY_METADATA_t;

typedef struct
{
	HINT32			nHttpServicePort;					/* httpd service port : airplay client will attemp to connect via this port */
	HINT32			nRtspServicePort;					/* rtspd service port : airplay client will attemp to connect via this port */	
	HINT8			aDeviceModel[MAX_SERVICE_STRING];	/* device model : this name is device name of published service */
	HINT8			aServiceName[MAX_SERVICE_STRING];	/* service name : this name will be shown by client */
	HCHAR			szMacAddr[20];						/* mac address : this value to be sent publisher directly. so format should be 00:00:00:00:00:00*/
	HCHAR			szIpAddr[20];						/* ip address : this value is for publisher address recored */
}PL_AIRPLAY_SERVICE_PARAM_t;

typedef void (*PL_AIRPLAY_OperationCallback)(PL_AIRPLAY_OP_TYPE opType, char* pPARAM, HINT64 iPARAM);

#define ________________Public_Functions___________________________

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	PL_AIRPLAY_Init(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	PL_AIRPLAY_DeInit(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	PL_AIRPLAY_Start(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	PL_AIRPLAY_Stop( void);

/* *
 * @brief 
 *
 * @param cbDevice  
 * @param cbRequestResult  
 *
 * @return 
 **/ 
HERROR	PL_AIRPLAY_RegisterEventCB(PL_AIRPLAY_OperationCallback cb_op);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	PL_AIRPLAY_UnRegisterEventCB(void);

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HULONG	PL_AIRPLAY_GetNumericCurrentIp( void );

/* *
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	PL_AIRPLAY_SetServiceParameter(PL_AIRPLAY_SERVICE_PARAM_t* pServiceParam);


/* *
 * @brief 
 *
 * @param PL_AIRPLAY_IsStarted  
 *
 * @return 
 **/ 
HBOOL	PL_AIRPLAY_IsStarted(void);


/* *
 * @brief 
 *
 * @param PL_AIRPLAY_SetPlaybackInfo
 *
 * @return 
 **/ 
void	PL_AIRPLAY_SetPlaybackInfo( PL_AIRPLAY_PLAYBACK_INFO_t tPlaybackInfo );

#endif	//__PL_DLNA_DMP_H__
/* end od file */
