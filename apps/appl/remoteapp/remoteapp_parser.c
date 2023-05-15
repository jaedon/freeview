/**************************************************************
 *	@file		remoteapp_parser.c
 *	RemoteApp Server Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/07/25
 *	@author			humax
 *	@brief			Remote App Data Parser
 ***************************************************************/

#include    <ondk.h>
#include	<stdarg.h>

#include	<oapi.h>
#include	<hapi.h>

#if defined(CONFIG_ENHANCED_SECURITY)
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>
#endif

#include <directfb_keyboard.h>

#include "remoteapp_key_params.h"
#include "remoteapp_parser.h"

typedef struct ConverTable
{
   KEY_Code_t eRemoteappKeyCode;
   ONDK_KEY_Code_t	eKeyCode;
} ConverTable_t;

static const ConverTable_t gKeyCodeTable[] = 
{
	{ REMOTEAPP_KEY_STANDBY , KEY_STANDBY },
	{ REMOTEAPP_KEY_0 , KEY_0 },
	{ REMOTEAPP_KEY_1,  KEY_1 }, 
	{ REMOTEAPP_KEY_2,  KEY_2 },
	{ REMOTEAPP_KEY_3,  KEY_3 },
	{ REMOTEAPP_KEY_4,  KEY_4 },
	{ REMOTEAPP_KEY_5,  KEY_5},
	{ REMOTEAPP_KEY_6,  KEY_6},
	{ REMOTEAPP_KEY_7,  KEY_7},
	{ REMOTEAPP_KEY_8,  KEY_8},
	{ REMOTEAPP_KEY_9,  KEY_9},
	{ REMOTEAPP_KEY_RED, KEY_RED},
	{ REMOTEAPP_KEY_GREEN, KEY_GREEN},
	{ REMOTEAPP_KEY_YELLOW, KEY_YELLOW},
	{ REMOTEAPP_KEY_BLUE, KEY_BLUE},
	{ REMOTEAPP_KEY_MENU,  KEY_MENU},
	{ REMOTEAPP_KEY_IP_BROWSER,  KEY_IP_BROWSER},
	{ REMOTEAPP_KEY_PAGEUP,  KEY_PAGEUP},
	{ REMOTEAPP_KEY_PAGEDOWN, KEY_PAGEDOWN},
	{ REMOTEAPP_KEY_ARROWUP, KEY_ARROWUP},
	{ REMOTEAPP_KEY_ARROWDOWN, KEY_ARROWDOWN},
	{ REMOTEAPP_KEY_ARROWLEFT, KEY_ARROWLEFT},
	{ REMOTEAPP_KEY_ARROWRIGHT, KEY_ARROWRIGHT},
	{ REMOTEAPP_KEY_BACK, KEY_BACK},
	{ REMOTEAPP_KEY_EXIT, KEY_EXIT},
	{ REMOTEAPP_KEY_OK, KEY_OK},
	{ REMOTEAPP_KEY_PLAY, KEY_PLAY},
	{ REMOTEAPP_KEY_PAUSE, KEY_PAUSE},
	{ REMOTEAPP_KEY_STOP, KEY_STOP},
	{ REMOTEAPP_KEY_RECORD, KEY_RECORD},
	{ REMOTEAPP_KEY_FORWARD, KEY_FORWARD},
	{ REMOTEAPP_KEY_BACKWARD, KEY_BACKWARD},
	{ REMOTEAPP_KEY_PREV, KEY_PREV},
	{ REMOTEAPP_KEY_NEXT, KEY_NEXT},
	{ REMOTEAPP_KEY_MEDIA, KEY_MEDIA},
	{ REMOTEAPP_KEY_GUIDE, KEY_GUIDE},
	{ REMOTEAPP_KEY_VOLUMEUP, KEY_VOLUMEUP},
	{ REMOTEAPP_KEY_VOLUMEDOWN, KEY_VOLUMEDOWN},
	{ REMOTEAPP_KEY_INFO, KEY_INFO},
	{ REMOTEAPP_KEY_MUTE, KEY_MUTE},
	{ REMOTEAPP_KEY_TVRADIO, KEY_TVRADIO},
	{ REMOTEAPP_KEY_SUBTITLE, KEY_SUBTITLE},
	{ REMOTEAPP_KEY_TEXT, KEY_TEXT},
	{ REMOTEAPP_KEY_WIDE, KEY_WIDE},
	{ REMOTEAPP_KEY_CHLIST, KEY_CHLIST},
	{ REMOTEAPP_KEY_OPT, KEY_OPT},
	{ REMOTEAPP_KEY_SMART_SEARCH, KEY_SMART_SEARCH},
	{ REMOTEAPP_KEY_AUDIO_LANGUAGE, KEY_AUDIO_LANGUAGE},
	{ REMOTEAPP_KEY_VOD, KEY_VOD}
};

HERROR	local_strtohex(char *pStr, HUINT32 *pValue)
{
	HERROR hErr = ERR_OK;

	HUINT32 uVal = 0;
	HINT32	n = 1;
	HINT32	i = 0;


	if (pStr == NULL || pValue == NULL)
	{
		HxLOG_Error("%s() Invalid paramter pStr=%p, pValue=%p\n", __FUNCTION__, pStr, pValue);
		hErr = ERR_FAIL;
		goto exit;
	}

	HxLOG_Error("%s() pStr=%s +\n", __FUNCTION__, pStr);

	for (i = strlen(pStr)-1; i >= 0; i--)
	{
		//HxLOG_Debug("%s() pStr[%d]=%c\n", __FUNCTION__, i, pStr[i]);
		if (pStr[i] >= '0' && pStr[i] <= '9')
		{
			uVal += ( (pStr[i] - '0') * n);
		}
		else if (pStr[i] >= 'a' && pStr[i] <= 'f')
		{
			uVal += ( (pStr[i] - 'a' + 10) * n);
		}
		else if (pStr[i] >= 'A' && pStr[i] <= 'F')
		{
			uVal += ( (pStr[i] - 'A' + 10) * n);
		}
		else
		{
			HxLOG_Error("%s() Unknown keycode=%c, index=%d\n", __FUNCTION__, pStr[i], i);
			hErr = ERR_FAIL;
			goto exit;
		}
		//HxLOG_Debug("%s() uVal=%d, 0x%x\n", __FUNCTION__, uVal, uVal);
		n *= 16;
	}

	*pValue = uVal;

exit:
	HxLOG_Info("%s() -\n", __FUNCTION__);
	return hErr;
}


KEY_Code_t  local_convert_remoteappkey_to_ondkkey(KEY_Code_t eRemoteappKeyCode)
{
    const ConverTable_t *pKeyCodeTable;
    
    for ( pKeyCodeTable = gKeyCodeTable; pKeyCodeTable < gKeyCodeTable + NELEM(gKeyCodeTable) ; pKeyCodeTable++)
    {
      if ( pKeyCodeTable->eRemoteappKeyCode == eRemoteappKeyCode )
      {
        return pKeyCodeTable->eKeyCode;
      }
    }  

    return KEY_UNKNOWN;
}


HERROR	local_change_to_direct_fb_key(char *keycode, HUINT32 *puKeySymbol)
{
	HERROR 		hErr = ERR_FAIL;
	ONDK_KEY_Code_t	eKeyCode = KEY_UNKNOWN;
	KEY_Code_t eRemoteappKeyCode = REMOTEAPP_KEY_UNKNOWN;
	
	if (keycode == NULL || puKeySymbol == NULL)
	{
		goto exit;
	}

	hErr = local_strtohex(keycode, (HUINT32*) &eRemoteappKeyCode);

    eKeyCode = local_convert_remoteappkey_to_ondkkey(eRemoteappKeyCode);

	switch (eKeyCode)
	{
		case KEY_STANDBY:
			*puKeySymbol = DIKS_POWER;
			break;
		case KEY_0:
			*puKeySymbol = DIKS_0;
			break;
		case KEY_1:
			*puKeySymbol = DIKS_1;
			break;
		case KEY_2:
			*puKeySymbol = DIKS_2;
			break;
		case KEY_3:
			*puKeySymbol = DIKS_3;
			break;
		case KEY_4:
			*puKeySymbol = DIKS_4;
			break;
		case KEY_5:
			*puKeySymbol = DIKS_5;
			break;
		case KEY_6:
			*puKeySymbol = DIKS_6;
			break;
		case KEY_7:
			*puKeySymbol = DIKS_7;
			break;
		case KEY_8:
			*puKeySymbol = DIKS_8;
			break;
		case KEY_9:
			*puKeySymbol = DIKS_9;
			break;
		case KEY_RED:
			*puKeySymbol = DIKS_RED;
			break;
		case KEY_GREEN:
			*puKeySymbol = DIKS_GREEN;
			break;
		case KEY_YELLOW:
			*puKeySymbol = DIKS_YELLOW;
			break;
		case KEY_BLUE:
			*puKeySymbol = DIKS_BLUE;
			break;
		case KEY_MENU: /*  = KEY_HUB */
			*puKeySymbol = DIKS_MENU;
			break;
		case KEY_IP_BROWSER:
			*puKeySymbol = DIKS_INTERNET;
			break;
		case KEY_PAGEUP:
			*puKeySymbol = DIKS_CHANNEL_UP;
			break;
		case KEY_PAGEDOWN:
			*puKeySymbol = DIKS_CHANNEL_DOWN;
			break;
		case KEY_ARROWUP:
			*puKeySymbol = DIKS_CURSOR_UP;
			break;
		case KEY_ARROWDOWN:
			*puKeySymbol = DIKS_CURSOR_DOWN;
			break;
		case KEY_ARROWLEFT:
			*puKeySymbol = DIKS_CURSOR_LEFT;
			break;
		case KEY_ARROWRIGHT:
			*puKeySymbol = DIKS_CURSOR_RIGHT;
			break;
		case KEY_BACK:
			*puKeySymbol = DIKS_BACK;
			break;
		case KEY_EXIT:
			*puKeySymbol = DIKS_EXIT;
			break;
		case KEY_OK:
			*puKeySymbol = DIKS_RETURN;
			break;
		/*
		case KEY_PLAYPAUSE:
			*puKeySymbol = DIKS_PLAYPAUSE;
			break;
		*/
		case KEY_PLAY:
			*puKeySymbol = DIKS_PLAY;
			break;
		case KEY_PAUSE:
			*puKeySymbol = DIKS_PAUSE;
			break;
		case KEY_STOP:
			*puKeySymbol = DIKS_STOP;
			break;
		case KEY_RECORD:
			*puKeySymbol = DIKS_RECORD;
			break;
		case KEY_FORWARD:
			*puKeySymbol = DIKS_FASTFORWARD;
			break;
		case KEY_BACKWARD:
			*puKeySymbol = DIKS_REWIND;
			break;
		case KEY_PREV:
			*puKeySymbol = DIKS_PREVIOUS;
			break;
		case KEY_NEXT:
			*puKeySymbol = DIKS_NEXT;
			break;
		case KEY_MEDIA:
			*puKeySymbol = DIKS_PLAYER;
			break;
		case KEY_GUIDE:
			*puKeySymbol = DIKS_EPG;
			break;
		case KEY_VOLUMEUP:
			*puKeySymbol = DIKS_VOLUME_UP;
			break;
		case KEY_VOLUMEDOWN:
			*puKeySymbol = DIKS_VOLUME_DOWN;
			break;
		case KEY_INFO:
			*puKeySymbol = DIKS_INFO;
			break;
		case KEY_MUTE:
			*puKeySymbol = DIKS_MUTE;
			break;
		case KEY_TVRADIO:
			HxLOG_Error("%s() KEY_TVRADIO is not support\n");
			//*puKeySymbol = DIKS_;
			break;
		case KEY_SUBTITLE:
			*puKeySymbol = DIKS_SUBTITLE;
			break;
		case KEY_TEXT:
			*puKeySymbol = DIKS_TEXT;
			break;
		case KEY_WIDE:
			*puKeySymbol = DIKS_ZOOM;
			break;
		case KEY_CHLIST:
			*puKeySymbol = DIKS_LIST;
			break;
		case KEY_OPT:	/* = KEY_PLUS */
			*puKeySymbol = DIKS_OPTION;
			break;
 		case KEY_SMART_SEARCH:
			*puKeySymbol = DIKS_CUSTOM6;
			break;
		case KEY_AUDIO_LANGUAGE:
			*puKeySymbol = DIKS_AUDIO;
			break;
		case KEY_VOD:
			*puKeySymbol = DIKS_CUSTOM45;
			break;
		default:
			HxLOG_Error("%s() Unknown keycode=(%s:0x%x)\n", __FUNCTION__, keycode, eKeyCode);
			hErr = ERR_FAIL;
			goto exit;
			break;
	}

exit:
	HxLOG_Info("%s() -\n", __FUNCTION__);
	return hErr;
}

HERROR	local_compare_code_and_codename(char *keycode, HUINT32 *puKeyCode)
{
	HERROR 		hErr = ERR_OK;

	HxLOG_Info("%s() +\n", __FUNCTION__);
	HxLOG_Info("%s() keycode = %s\n", __FUNCTION__, keycode);

	if(puKeyCode == NULL)
	{
		hErr = ERR_FAIL;
		goto exit;
	}

	*puKeyCode = 0;

	while(*keycode != ',')
	{
		if(*keycode >= '0' || *keycode <= '9')
		{
			*puKeyCode = *puKeyCode * 10;
			*puKeyCode += *keycode - '0';
		}
		else
		{
			HxLOG_Error("%s() Not supported HEX Keycode!!\n", __FUNCTION__);
			hErr = ERR_FAIL;
			goto exit;
		}
		keycode++;
	}

	keycode++;
	HxLOG_Info("%s() keycode = %d, key name = %s \n", __FUNCTION__, *puKeyCode, keycode);

	if(strncmp(keycode, KEY_NAME_ENTER, REMOTE_RCU_KEY_TYPE_LEN) == 0)
	{
		if(*puKeyCode != KEY_VALUE_ENTER)
		{
			HxLOG_Error("%s() Key Code(%d) != Key Name(%s:%d)\n", __FUNCTION__, *puKeyCode, KEY_NAME_ENTER, KEY_VALUE_ENTER);
			hErr = ERR_FAIL;
			goto exit;
		}
	}
	else if(strncmp(keycode, KEY_NAME_TAB, REMOTE_RCU_KEY_TYPE_LEN) == 0)
	{
		if(*puKeyCode != KEY_VALUE_TAB)
		{
			HxLOG_Error("%s() Key Code(%d) != Key Name(%s:%d)\n", __FUNCTION__, *puKeyCode, KEY_NAME_TAB, KEY_VALUE_TAB);
			hErr = ERR_FAIL;
			goto exit;
		}
	}
	else if(strncmp(keycode, KEY_NAME_DEL, REMOTE_RCU_KEY_TYPE_LEN) == 0)
	{
		if(*puKeyCode != KEY_VALUE_DEL)
		{
			HxLOG_Error("%s() Key Code(%d) != Key Name(%s:%d)\n", __FUNCTION__, *puKeyCode, KEY_NAME_DEL, KEY_VALUE_DEL);
			hErr = ERR_FAIL;
			goto exit;
		}
	}
	else
	{
		HxLOG_Error("%s() No Registered Key Code & Key Name...\n", __FUNCTION__);
		hErr = ERR_FAIL;
		goto exit;
	}

exit:
	HxLOG_Info("%s() -\n", __FUNCTION__);
	return hErr;
}

HERROR	local_get_qt_keydata(char *keycode, HUINT8 **psString, RemoteData_e* peResultType)
{
	HERROR 		hErr = ERR_OK;

	HxLOG_Info("%s() +\n", __FUNCTION__);
	HxLOG_Info("%s() keycode = %s\n", __FUNCTION__, keycode);

	if (keycode == NULL || *psString != NULL)
	{
		goto exit;
	}

	if(*keycode == '1')	// 1,0,a charactor
	{
		*psString = HLIB_STD_MemAlloc(sizeof(HUINT8) + 1);
		if(*psString == NULL)
		{
			HxLOG_Error("%s() Cannot alloc memory. \n", __FUNCTION__);
			hErr = ERR_FAIL;
			goto exit;
		}
		else
		{
			*psString[0] = *(keycode + 4);
			*psString[1] = '\0';
			*peResultType = eRemote_Keyboard_Char;
			HxLOG_Info("%s() input key : %s\n", __FUNCTION__, *psString);
		}
	}
	else if(*keycode == '2') // 2,keycode,cmd
	{
		HxLOG_Error("%s() keycode\n", __FUNCTION__);

		*psString = HLIB_STD_MemAlloc(sizeof(HUINT32));
		if(*psString == NULL)
		{
			HxLOG_Error("%s() Cannot alloc memory. \n", __FUNCTION__);
			hErr = ERR_FAIL;
			goto exit;
		}
		else
		{
			hErr = local_compare_code_and_codename(keycode + 2, *((HUINT32 **)psString));

			//hErr = local_strtohex(keycode[2], *psString);
			if (hErr != ERR_OK)
			{
				HxLOG_Error("%s() Fail to parsing key code\n", __FUNCTION__);
				hErr = ERR_FAIL;
				goto exit;
			}
			else
			{
				*peResultType = eRemote_Keyboard_Code;
				HxLOG_Info("%s() input key : %d\n", __FUNCTION__, *psString);
			}
		}
	}
	else if(*keycode == '3') // 3,0,abcd string
	{
		HUINT32 ulLen = 0;

		ulLen = HxSTD_StrLen(keycode) - 4;
		*psString = HLIB_STD_MemAlloc(sizeof(HUINT8) * ulLen + 1); // prefix(3,0,) remove
		if(*psString == NULL)
		{
			HxLOG_Error("%s() Cannot alloc memory. \n", __FUNCTION__);
			hErr = ERR_FAIL;
			goto exit;
		}
		else
		{
			HxSTD_StrNCpy(*psString, keycode + 4, ulLen + 1);
			*peResultType = eRemote_Keyboard_String;
			HxLOG_Info("%s() input key : %s\n", __FUNCTION__, *psString);
		}
	}

exit:
	HxLOG_Info("%s() -\n", __FUNCTION__);
	return hErr;
}


HERROR	remoteapp_parser(RemoteRCU_Rcu_t *pstRecvData, void **pvResult, RemoteData_e *peResultType)
{
	HERROR hErr = ERR_FAIL;

	HxLOG_Info("%s() +\n", __FUNCTION__);
	HxLOG_Info("%s() pstRecvData[id = %s, keytype = %s, data = %s]\n", __FUNCTION__, pstRecvData->id, pstRecvData->key_type, pstRecvData->data.reserved);

	if (pstRecvData == NULL)
	{
		HxLOG_Error("%s() Invalid parameter pstRecvData=%p \n", __FUNCTION__, pstRecvData);
		hErr = ERR_FAIL;
		goto exit;
	}

	if (strncmp(pstRecvData->id, REMOTE_RCU_ID, REMOTE_RCU_ID_LEN) != 0)
	{
		HxLOG_Error("%s() Incorrect RCU ID\n", __FUNCTION__);
		hErr = ERR_FAIL;
		goto exit;
	}

	if (strncmp(pstRecvData->key_type, KEY_TYPE_RCU, REMOTE_RCU_KEY_TYPE_LEN) == 0)
	{
		*pvResult = HLIB_STD_MemAlloc(sizeof(HUINT32));
		if(*pvResult == NULL)
		{
			HxLOG_Error("%s() Cannot alloc memory. \n", __FUNCTION__);
			hErr = ERR_FAIL;
			goto exit;
		}
		else
		{
			*peResultType = eRemote_RCU;
			hErr = local_change_to_direct_fb_key(pstRecvData->data.stRcuData.keycode, *((HUINT32 **)pvResult));
			if (hErr != ERR_OK)
			{
				HxLOG_Error("%s() Fail to change to direct fb key\n", __FUNCTION__);
				hErr = ERR_FAIL;
				goto exit;
			}
			else
			{
				HxLOG_Error("%s() type : %x, direct fb key : %d\n", __FUNCTION__, *peResultType, **((HUINT32 **)pvResult));
			}
		}
	}
	else if (strncmp(pstRecvData->key_type, KEY_TYPE_TOUCH, REMOTE_RCU_KEY_TYPE_LEN) == 0)
	{
		HxLOG_Error("%s() touchpad is not support\n", __FUNCTION__);
		hErr = ERR_FAIL;
		goto exit;
	}
	else if (strncmp(pstRecvData->key_type, KEY_TYPE_KEY, REMOTE_RCU_KEY_TYPE_LEN) == 0)
	{
		hErr = local_get_qt_keydata(pstRecvData->data.stKeyboardData.inputkey, (HUINT8 **)pvResult, peResultType);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("%s() Fail to parsing string\n", __FUNCTION__);
			hErr = ERR_FAIL;
			goto exit;
		}
		else
		{
			HxLOG_Error("%s() type : %x string : %s\n", __FUNCTION__, *peResultType, *((HUINT8 **)pvResult));
		}
	}
	else
	{
		HxLOG_Error("%s() Unknown keytype !!! %s\n", __FUNCTION__, pstRecvData->key_type);
		hErr = ERR_FAIL;
		goto exit;
	}

exit:
	HxLOG_Info("%s() -\n", __FUNCTION__);
	return hErr;
}
