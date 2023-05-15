#include "oplalexahelper.h"
#include "oapi.h"
#include "hapi.h"
#include <hlib.h>
#include "apk.h"

#if defined(CONFIG_ALEXA)
#include <directfb.h>

#define OPL_ALEXA_REQUEST_NAME							"name"
#define OPL_ALEXA_REQUEST_DATA							"data"
#endif

typedef enum
{
	OPL_ALEXA_REQUEST_NONE							= 0,
	OPL_ALEXA_REQUEST_CHANNEL_CHANGE				= 1,
	OPL_ALEXA_REQUEST_CHANNEL_SKIP					= 2,
	OPL_ALEXA_REQUEST_TURN_OFF						= 3,
	OPL_ALEXA_REQUEST_TURN_ON						= 4,
	OPL_ALEXA_REQUEST_PLAYBACK_FAST_FORWARD			= 5,
	OPL_ALEXA_REQUEST_PLAYBACK_NEXT					= 6,	// Web work (FVP-5000T not supported Key)
	OPL_ALEXA_REQUEST_PLAYBACK_PAUSE				= 7,
	OPL_ALEXA_REQUEST_PLAYBACK_PLAY					= 8,
	OPL_ALEXA_REQUEST_PLAYBACK_PREVIOUS				= 9,	// Web work (FVP-5000T not supported Key)
	OPL_ALEXA_REQUEST_PLAYBACK_REWIND				= 10,
	OPL_ALEXA_REQUEST_PLAYBACK_STARTOVER			= 11,	// Web work (Seek function on WebApp)
	OPL_ALEXA_REQUEST_PLAYBACK_STOP					= 12,
	OPL_ALEXA_REQUEST_VOLUME_SET					= 13,	// Volume ... (Holding)
	OPL_ALEXA_REQUEST_VOLUME_ADJUST					= 14,
	OPL_ALEXA_REQUEST_VOLUME_MUTE					= 15,
	OPL_ALEXA_REQUEST_SEARCH_AND_DISPLAY_RESULT		= 16,
	OPL_ALEXA_REQUEST_SEARCH_AND_PLAY				= 17	// ¹ÌÁö¿ø...
} OPL_AlexaHelper_Request_e;

#define ________________Public_Interfaces__________________________
BPLStatus OPL_AlexaHelper_processData(const char* data)
{
#if defined(CONFIG_ALEXA)
	if(data)
	{
		HxJSON_t pstData = HLIB_JSON_Decode(data);
		HxLOG_Debug("Data: %s\n", data);
		if(pstData)
		{
			HINT32 nRequest = HLIB_JSON_Object_GetInteger(pstData, OPL_ALEXA_REQUEST_NAME);
			HxLOG_Debug("pszRequest: %s\n", nRequest);
			
			switch(nRequest)
			{
				case OPL_ALEXA_REQUEST_TURN_OFF:
				case OPL_ALEXA_REQUEST_TURN_ON:
					APK_SCENARIO_SendKeyFromApp((HUINT32)DIKS_POWER);
					break;
				case OPL_ALEXA_REQUEST_PLAYBACK_FAST_FORWARD:
					APK_SCENARIO_SendKeyFromApp((HUINT32)DIKS_FASTFORWARD);
					break;
				case OPL_ALEXA_REQUEST_PLAYBACK_NEXT:
					// APK_SCENARIO_SendKeyFromApp((HUINT32)DIKS_NEXT);
					break;
				case OPL_ALEXA_REQUEST_PLAYBACK_PAUSE:
					APK_SCENARIO_SendKeyFromApp((HUINT32)DIKS_PAUSE);
					break;
				case OPL_ALEXA_REQUEST_PLAYBACK_PLAY:
					APK_SCENARIO_SendKeyFromApp((HUINT32)DIKS_PLAY);
					break;

				case OPL_ALEXA_REQUEST_PLAYBACK_PREVIOUS:
					// APK_SCENARIO_SendKeyFromApp((HUINT32)DIKS_PREVIOUS);
					break;
				case OPL_ALEXA_REQUEST_PLAYBACK_REWIND:
					APK_SCENARIO_SendKeyFromApp((HUINT32)DIKS_REWIND);
					break;
				case OPL_ALEXA_REQUEST_PLAYBACK_STOP:
					APK_SCENARIO_SendKeyFromApp((HUINT32)DIKS_STOP);
					break;
				case OPL_ALEXA_REQUEST_VOLUME_ADJUST:
					{
						HINT32 nVolume = HLIB_JSON_Object_GetInteger(pstData, OPL_ALEXA_REQUEST_DATA);
						if(nVolume > 0)
						{
							APK_SCENARIO_SendKeyFromApp((HUINT32)DIKS_VOLUME_UP);
						}
						else if(nVolume < 0)
						{
							APK_SCENARIO_SendKeyFromApp((HUINT32)DIKS_VOLUME_DOWN);
						}
						else
						{
							HxLOG_Debug("Something wrong. Invalid volume parameter.\n");
						}
					}
					break;
				case OPL_ALEXA_REQUEST_VOLUME_MUTE:
					APK_SCENARIO_SendKeyFromApp((HUINT32)DIKS_MUTE);
					break;
				case OPL_ALEXA_REQUEST_SEARCH_AND_DISPLAY_RESULT:
					// Search Key ...
					APK_SCENARIO_SendKeyFromApp((HUINT32)DIKS_CUSTOM6);
					break;
				case OPL_ALEXA_REQUEST_NONE:
				case OPL_ALEXA_REQUEST_CHANNEL_CHANGE:
				case OPL_ALEXA_REQUEST_CHANNEL_SKIP:
				case OPL_ALEXA_REQUEST_PLAYBACK_STARTOVER:
				case OPL_ALEXA_REQUEST_VOLUME_SET:
				case OPL_ALEXA_REQUEST_SEARCH_AND_PLAY:
				default:
					HxLOG_Error("This skill [%d] is not supported on the MW.\n", nRequest);
					break;
			}
		}
		else
		{
			HxLOG_Debug("Error, There is no parsed data.\n");
		}
	}
	else
	{
		HxLOG_Debug("Error, There is no data.\n");
	}
#else
	HxLOG_Debug("Enable CONFIG_ALEXA to use this function.\n");
#endif
	return BPL_STATUS_OK;
}


