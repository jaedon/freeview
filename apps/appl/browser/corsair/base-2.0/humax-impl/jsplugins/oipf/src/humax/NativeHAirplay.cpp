/*************************************************************************************************************
	File 		: NativeHAirplay.cpp
	author 		: STB-Component@humaxdigital.com
	comment		: 
	date    	: 2013/09/05
	attention 	: 

	(c) 2011-2012 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. (¡°Humax¡±) and 
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "NativeHAirplay.h"
#include "macros.h"
#include <hlib.h>

#include "oplairplay.h"

#ifdef OIPF

static AirplayEventListener 	    g_cbAirplay = NULL;


static void	__native_airplay_listener(unsigned int event, unsigned long long decimal, char* pUrl, char * title, char * album, char * artist, char * path)
{
    unsigned int convEvent = eNATIVE_AIRPLAY_REQ_UNKNOWN;

    OOIF_LOG_DEBUG("[nativeHAirplay] Receive msg [%d]\n", event);
    switch(event)
    {
        case eOPL_AIRPLAY_REQ_PLAY:
            OOIF_LOG_DEBUG("[nativeHAirplay] Url : %s\n", pUrl);
            convEvent = eNATIVE_AIRPLAY_REQ_PLAY;
            break;
        case eOPL_AIRPLAY_REQ_STOP:
            OOIF_LOG_DEBUG("[nativeHAirplay] Stop\n");
            convEvent = eNATIVE_AIRPLAY_REQ_STOP;
            break;
        case eOPL_AIRPLAY_REQ_SETPOSITION:
            OOIF_LOG_DEBUG("[nativeHAirplay] SetPosition : %lld\n", decimal);
            convEvent = eNATIVE_AIRPLAY_REQ_SETPOSITION;
            break;
        case eOPL_AIRPLAY_REQ_SETRATE:
            OOIF_LOG_DEBUG("[nativeHAirplay] SetRate : %lld\n", decimal);
            convEvent = eNATIVE_AIRPLAY_REQ_SETRATE;
            break;
        case eOPL_AIRPLAY_REQ_GET_PLAYBACK_INFO:
            OOIF_LOG_DEBUG("[nativeHAirplay] Playback info\n");
            /* Async event sender required : HOMMA API event notifier got Semma */
            return;
        case eOPL_AIRPLAY_REQ_SHOWPHOTO:
            OOIF_LOG_DEBUG("[nativeHAirplay] Show Photo %s\n", path);
            convEvent = eNATIVE_AIRPLAY_REQ_SHOWPHOTO;
            break;
        case eOPL_AIRTUNES_REQ_PLAY:
            OOIF_LOG_DEBUG("[nativeHAirtunes] Play %s\n", pUrl);
            convEvent = eNATIVE_AIRTUNES_REQ_PLAY;
            break;
        case eOPL_AIRTUNES_REQ_STOP:
            OOIF_LOG_DEBUG("[nativeHAirtunes] Stop %s\n");
            convEvent = eNATIVE_AIRTUNES_REQ_STOP;
            break;
        case eOPL_AIRTUNES_REQ_FLUSH:
            OOIF_LOG_DEBUG("[nativeHAirtunes] Flush\n");
            convEvent = eNATIVE_AIRTUNES_REQ_FLUSH;
            break;
        case eOPL_AIRTUNES_REQ_SETALBUMART:
            OOIF_LOG_DEBUG("[nativeHAirtunes] Set Albumart %s\n", path);
            convEvent = eNATIVE_AIRTUNES_REQ_SETALBUMART;
            break;
        case eOPL_AIRTUNESY_REQ_SETMETADATA:
            OOIF_LOG_DEBUG("[nativeHAirtunes] Set Metadata title = %s\n", title);
            convEvent = eNATIVE_AIRTUNES_REQ_SETMETADATA;
            break;
        default:
            convEvent = eNATIVE_AIRPLAY_REQ_UNKNOWN;
            break;
    } 

    g_cbAirplay(convEvent, decimal, pUrl, path, title, album, artist );
}



OOIFReturnCode 	native_Airplay_Start(OOIFBoolean *retval)
{
	if ( OPL_AIRPLAY_Start() == BPL_STATUS_OK )
	{
    	OPL_AIRPLAY_IsActivated(retval);
        
        /*
        **  note ; run thread if airplay works  only
        */
        if ( *retval == TRUE )
        {
            OOIF_LOG_DEBUG("Start Airplay start playbackinfo threadn");
        }
	}
    else
    {
         *retval = FALSE;
    }

	return OOIF_RETURN_OK;
}
OOIFReturnCode 	native_Airplay_Stop(OOIFBoolean *retval)
{

	OPL_AIRPLAY_IsActivated(retval);   
    
    if ( *retval == TRUE )
    {   
        /*
        ** run timer on OPL_AIRPLAY_Stop and change activated value on timer , 
        ** so can't use OPL_AIRPLAY_IsActivated right away
        ** It's architecture problem.
        */
    	if ( OPL_AIRPLAY_Stop() == BPL_STATUS_OK )
    	{
            *retval = FALSE;
    	}
    	//OPL_AIRPLAY_IsActivated(retval);
    }
	return OOIF_RETURN_OK;
}

bool 			native_Airplay_IsStarted() // same with IsActivated???
{
	HBOOL ret = FALSE;
	OPL_AIRPLAY_IsActivated(&ret);
	return (bool)ret;
}

bool 			native_Airplay_IsActivated()
{
	HBOOL ret = FALSE;
	OPL_AIRPLAY_IsActivated(&ret);
	return (bool)ret;
}

OOIFReturnCode 	native_Airplay_RegEventListener(AirplayEventListener listener)
{
    g_cbAirplay = listener;
	OPL_AIRPLAY_RegisterListener(__native_airplay_listener);
	return OOIF_RETURN_OK;
}

OOIFReturnCode 	native_Airplay_UnRegEventListener()
{
    g_cbAirplay = NULL;
	OPL_AIRPLAY_UnRegisterListener();
	return OOIF_RETURN_OK;
}

OOIFReturnCode  native_Airplay_SetPlaybackStatusInfo(const char* status)
{
    OPL_AIRPLAY_SetPlaybackStatusInfo(status);
    return OOIF_RETURN_OK;
}

OOIFReturnCode  native_Airplay_SetBufferedRangeInfo(long long int buffered_range)
{
    OPL_AIRPLAY_SetBufferedRangeInfo(buffered_range);
    return OOIF_RETURN_OK;
}

OOIFReturnCode  native_Airplay_SetSeekableRangeInfo(long long int seekable_range)
{
    OPL_AIRPLAY_SetBufferedRangeInfo(seekable_range);

    return OOIF_RETURN_OK;
}

OOIFReturnCode  native_Airplay_SetDurationInfo(long long int duration)
{
    OPL_AIRPLAY_SetDurationInfo(duration);

    return OOIF_RETURN_OK;
}

OOIFReturnCode  native_Airplay_SetPositionInfo(long long int position)
{
    OPL_AIRPLAY_SetPositionInfo(position);

    return OOIF_RETURN_OK;
}

OOIFReturnCode  native_Airplay_SetStartPositionInfo(long long int start_position)
{
    OPL_AIRPLAY_SetStartPositionInfo(start_position);

    return OOIF_RETURN_OK;
}

OOIFReturnCode  native_Airplay_SetRateInfo(int rate)
{
    OPL_AIRPLAY_SetRateInfo(rate);

    return OOIF_RETURN_OK;
}

#endif // OIPF
