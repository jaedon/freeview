/*************************************************************************************************************
	File            : airplay.h
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

#ifndef __AIRPLAY_H__
#define __AIRPLAY_H__

#define	________________Header_Files_______________________________ 
#include "pl_airplay.h"

#define ________________Typedef____________________________________
typedef enum
{
	eAIRPLAY_OP_PLAY = 0,
	eAIRPLAY_OP_SETPOSITION,
	eAIRPLAY_OP_SETRATE,
	eAIRPLAY_OP_STOP,
	eAIRPLAY_OP_SHOWPHOTO,
	eAIRPLAY_OP_GET_PLAYBACK_INFO,
	eAIRTUNES_OP_PLAY,
	eAIRTUNES_OP_FLUSH,
	eAIRTUNES_OP_STOP,
	eAIRTUNES_OP_SET_METADATA,
	eAIRTUNES_OP_SET_ALBUMART
} AIRPLAY_Event_e;

typedef struct {
	HCHAR szTitle[128];
	HCHAR szArtist[128];
	HCHAR szAlbum[128];
} AIRTUNES_Meatadata_t;

typedef struct {
	HCHAR szAlbumArtPath[512];
	HINT32 unAlbumArtSize;
} AIRTUNES_AlbumArt_t;

typedef void (*AIRPLAY_EventCallback_t)(HINT32 param1, HINT32 param2, HINT64 param3);

#define ________________Public_Functions___________________________

/**
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	AIRPLAY_Start(void);

/**
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	AIRPLAY_Stop(void);

/**
 * @brief 
 *
 * @param cb  
 *
 * @return 
 **/ 
HERROR	AIRPLAY_RegisterEventCallback(AIRPLAY_EventCallback_t cb);

/**
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	AIRPLAY_UnregisterEventCallback(void);

/**
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HBOOL	AIRPLAY_IsStart(void);

/**
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HBOOL	AIRPLAY_NeedRestart(void);

/**
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	AIRPLAY_SetServiceParameter(PL_AIRPLAY_SERVICE_PARAM_t* pServiceParam);

/**
 * @brief 
 *
 * @param void  
 *
 * @return 
 **/ 
HERROR	AIRPLAY_SetPlaybackInfo(PL_AIRPLAY_PLAYBACK_INFO_t tPlaybackInfo);

#endif /* __AIRPLAY_H__ */
/* end od file */
