/*************************************************************************************************************
	File            : airplay.h
	author          : parkjh4@humaxdigital.com
	comment         : 
	date            : 2013-08-27
	attention       :

	(c) 2011-2013 Humax Co., Ltd.
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

#define	________________Header_Files_______________________________
#include "airplay.h"
#include "homma_bus.h"

#define ________________Typedef____________________________________
#define	__MODULE__	"SERVICE:AIRPLAY"

typedef struct
{
	HBOOL				bIsStart;
	AIRPLAY_EventCallback_t	event_cb;
	HULONG				ulCurrentIp;
} airplay_context_t;

typedef struct
{
	HINT32 p1;
	HINT32 p2;
} int32_pair_t;

typedef struct
{
	PL_AIRPLAY_OP_TYPE opEventType;
	const HCHAR *opEventName;
} AIRPLAY_OP_EVENTS; /* AirPlay/AirTunes */

#define ________________Private_Static_Variables___________________
static airplay_context_t pst_airplay = {FALSE, NULL, 0};

static AIRPLAY_OP_EVENTS s_stOpEvents[] = /* AirPlay/AirTunes */
{
	{PL_AIRPLAY_OP_PLAY,               "AIRPLAY_PLAY"},
	{PL_AIRPLAY_OP_SETPOSITION,        "AIRPLAY_SETPOSITION"},
	{PL_AIRPLAY_OP_SETRATE,            "AIRPLAY_SETRATE"},
	{PL_AIRPLAY_OP_STOP,               "AIRPLAY_STOP"},
	{PL_AIRPLAY_OP_SHOWPHOTO,          "AIRPLAY_SHOWPHOTO"},
	{PL_AIRPLAY_OP_GET_PLAYBACK_INFO,  "AIRPLAY_GET_PLAYBACK_INFO"},
	{PL_AIRTUNES_OP_PLAY,              "TUNES_PLAY"},
	{PL_AIRTUNES_OP_FLUSH,             "TUNES_FLUSH"},
	{PL_AIRTUNES_OP_STOP,              "TUNES_STOP"},
	{PL_AIRTUNES_OP_SET_METADATA,      "TUNES_SET_METADATA"},
	{PL_AIRTUNES_OP_SET_ALBUMART,      "TUNES_SET_ALBUMART"}
};

#define ________________Internal_Functions_________________________

static AIRPLAY_OP_EVENTS *__lookup_op_event_type(PL_AIRPLAY_OP_TYPE eEvent)
{
	unsigned i;
	for (i=0; i<sizeof(s_stOpEvents)/sizeof(s_stOpEvents[0]); i++) 
	{
		if (s_stOpEvents[i].opEventType == eEvent) 
		{
			return &s_stOpEvents[i];
		}
	}
	return NULL;
}

const HCHAR *__get_op_event_str(PL_AIRPLAY_OP_TYPE eEvent)
{
	AIRPLAY_OP_EVENTS *stOpEvents = __lookup_op_event_type(eEvent);
	return stOpEvents?stOpEvents->opEventName:"UnknownEvent";
}

static void __operation_callback(PL_AIRPLAY_OP_TYPE opType, char* pPARAM, HINT64 ullPARAM)
{
	HxLOG_Print("[ %s:%d ] op_type(%d:%s)\n", __MODULE__, __LINE__, opType, __get_op_event_str(opType));
	
	switch(opType)
	{
		case PL_AIRPLAY_OP_PLAY:
			if(pst_airplay.event_cb != NULL)
			{
				pst_airplay.event_cb(eAIRPLAY_OP_PLAY, (HINT32)pPARAM, ullPARAM);
			}
			break;
		case PL_AIRPLAY_OP_SETPOSITION:
			if(pst_airplay.event_cb != NULL)
			{
				pst_airplay.event_cb(eAIRPLAY_OP_SETPOSITION, (HINT32)pPARAM, ullPARAM);
			}			
			break;
		case PL_AIRPLAY_OP_SETRATE:			
			if(pst_airplay.event_cb != NULL)
			{
				pst_airplay.event_cb(eAIRPLAY_OP_SETRATE, (HINT32)pPARAM, ullPARAM);
			}					
			break;
		case PL_AIRPLAY_OP_STOP:
			if(pst_airplay.event_cb != NULL)
			{
				pst_airplay.event_cb(eAIRPLAY_OP_STOP, 0, 0);
			}
			break;
		case PL_AIRPLAY_OP_SHOWPHOTO:
			if(pst_airplay.event_cb != NULL)
			{
				pst_airplay.event_cb(eAIRPLAY_OP_SHOWPHOTO, (HINT32)pPARAM, 0);
			}		
			break;
		case PL_AIRPLAY_OP_GET_PLAYBACK_INFO:
			if(pst_airplay.event_cb != NULL)
			{
				pst_airplay.event_cb(eAIRPLAY_OP_GET_PLAYBACK_INFO, 0, 0);
			}				
			break;
		case PL_AIRTUNES_OP_PLAY:
			if(pst_airplay.event_cb != NULL)
			{
				pst_airplay.event_cb(eAIRTUNES_OP_PLAY, (HINT32)pPARAM, 0);
			}
			break;
		case PL_AIRTUNES_OP_FLUSH:
			if(pst_airplay.event_cb != NULL)
			{
				pst_airplay.event_cb(eAIRTUNES_OP_FLUSH, 0, 0);
			}
			break;
		case PL_AIRTUNES_OP_STOP:
			if(pst_airplay.event_cb != NULL)
			{
				pst_airplay.event_cb(eAIRTUNES_OP_STOP, 0, 0);
			}
			break;
		case PL_AIRTUNES_OP_SET_METADATA:
			if(pst_airplay.event_cb != NULL)
			{
				pst_airplay.event_cb(eAIRTUNES_OP_SET_METADATA, (HINT32)pPARAM, 0);
			}
			break;
		case PL_AIRTUNES_OP_SET_ALBUMART:
			if(pst_airplay.event_cb != NULL)
			{
				pst_airplay.event_cb(eAIRTUNES_OP_SET_ALBUMART, (HINT32)pPARAM, 0);
			}
			break;
		default:
			HxLOG_Error("[ %s:%d ] Error> Unknown request(%d)\n", __MODULE__, __LINE__, opType);
			break;
	}
	return;
}

#define ________________Public_Functions___________________________
HERROR	AIRPLAY_Start(void)
{
	pst_airplay.bIsStart = FALSE;
	if (PL_AIRPLAY_GetNumericCurrentIp() == 0x0)
	{
		/* Currently Public IP address is not retrieved. Do not start */
		HxLOG_Error("[ %s:%d ] Error> Cant start airplay [invalide network]\n", __MODULE__, __LINE__);
		return ERR_FAIL;
	}
	if( PL_AIRPLAY_IsStarted() == TRUE )
	{
		pst_airplay.bIsStart = TRUE;
		return ERR_OK;
	}
	( void )PL_AIRPLAY_Init();
	( void )PL_AIRPLAY_RegisterEventCB(__operation_callback);
	( void )PL_AIRPLAY_Start();
	pst_airplay.bIsStart = TRUE;

	return ERR_OK;
}

HERROR	AIRPLAY_Stop(void)
{
	if( PL_AIRPLAY_IsStarted()  == TRUE )
	{
		( void )PL_AIRPLAY_Stop();
		( void )PL_AIRPLAY_DeInit();
	}
	pst_airplay.bIsStart = FALSE;

	return ERR_OK;
}

HBOOL	AIRPLAY_IsStart(void)
{
	return  pst_airplay.bIsStart;
}

HBOOL	AIRPLAY_NeedRestart(void)
{
	HULONG ulIp = PL_AIRPLAY_GetNumericCurrentIp();
	if(ulIp == 0)
	{
		/* Currently Public IP address is not retrieved. Do not restart */
		HxLOG_Error("Error> Cant get network\n");
		return FALSE;
	}
	if(ulIp != pst_airplay.ulCurrentIp)
	{
		pst_airplay.ulCurrentIp = ulIp;
		return TRUE;
	}
	return FALSE;
}

HERROR	AIRPLAY_RegisterEventCallback(AIRPLAY_EventCallback_t cb)
{
	pst_airplay.event_cb = cb;
	return ERR_OK;
}

HERROR	AIRPLAY_UnregisterEventCallback(void)
{
	pst_airplay.event_cb = NULL;
	return ERR_OK;
}

HERROR	AIRPLAY_SetServiceParameter(PL_AIRPLAY_SERVICE_PARAM_t* pServiceParam)
{
	return PL_AIRPLAY_SetServiceParameter(pServiceParam);
}

HERROR	AIRPLAY_SetPlaybackInfo(PL_AIRPLAY_PLAYBACK_INFO_t tPlaybackInfo)
{
	PL_AIRPLAY_SetPlaybackInfo(tPlaybackInfo);
	return ERR_OK;
}
/* end od file */
