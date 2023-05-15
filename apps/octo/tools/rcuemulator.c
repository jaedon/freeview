#include <hapi.h>
#include <hlib.h>
#include <directfb.h>

#define XML_DEFAULT_SCRIPT_FILE		"/usr/tools/script.xml"
#define XML_USB_SCRIPT_FILE			"/media/drive1/script.xml"

#define TXT_DEFAULT_SCRIPT_FILE		"/usr/tools/user_aging.txt"
#define TXT_USB_SCRIPT_FILE			"/media/drive1/user_aging.txt"

#define MAX_CHAR_NUMBER_OF_USER_KEY		20
#define MAX_REPEAT_TEST_COUNT			5000000000

typedef struct _UserKey
{
	HINT32				pause;
	HUINT8				*signalName;

} UserKey_t;

typedef struct _ScheduleAction
{
	HUINT8				*macroName;

	HINT32				repeatCount;
	HINT32				enable;
	HINT32				repeatIndefinitely;

} ScheduleAction_t;

typedef struct _MacroAction
{
	HINT32				pause;
	HUINT8				*signalName;

} MacroAction_t;


typedef struct _Macro
{
	MacroAction_t		*pMacroAction;

	HINT32				macroActionCount;
	HINT32				pause;

	HUINT8				*name;

} Macro_t;

ScheduleAction_t		*s_scheduleAction;
Macro_t 				*s_macro;
HINT32					s_scheduleAction_count,s_macro_count;

UserKey_t				*s_userKeyAction;
HINT32					s_userKeyAction_count;

HERROR local_RCUEMUL_ExecuteAction(HUINT8	*signalName)
{
	HERROR				hError = ERR_FAIL;

	if(HLIB_STD_StrCaseCmp(signalName,"Power") == 0)
		HAPI_SendRemoteRCUKey(DIKS_POWER);

	else if(HLIB_STD_StrCaseCmp(signalName,"Arrow_up") == 0)
		HAPI_SendRemoteRCUKey(DIKS_CURSOR_UP);
	else if(HLIB_STD_StrCaseCmp(signalName,"Arrow_down") == 0)
		HAPI_SendRemoteRCUKey(DIKS_CURSOR_DOWN);
	else if(HLIB_STD_StrCaseCmp(signalName,"down") == 0)
		HAPI_SendRemoteRCUKey(DIKS_CURSOR_DOWN);
	else if(HLIB_STD_StrCaseCmp(signalName,"Arrow_left") == 0)
		HAPI_SendRemoteRCUKey(DIKS_CURSOR_LEFT);
	else if((HLIB_STD_StrCaseCmp(signalName,"Arrow_right") == 0) || (HLIB_STD_StrCaseCmp(signalName,"Arrowright") == 0))
		HAPI_SendRemoteRCUKey(DIKS_CURSOR_RIGHT);

	else if(HLIB_STD_StrCaseCmp(signalName,"OK") == 0)
		HAPI_SendRemoteRCUKey(DIKS_OK);
	else if(HLIB_STD_StrCaseCmp(signalName,"ENTER") == 0)
		HAPI_SendRemoteRCUKey(DIKS_ENTER);

	else if(HLIB_STD_StrCaseCmp(signalName,"Vol+") == 0)
		HAPI_SendRemoteRCUKey(DIKS_VOLUME_UP);
	else if(HLIB_STD_StrCaseCmp(signalName,"Vol-") == 0)
		HAPI_SendRemoteRCUKey(DIKS_VOLUME_DOWN);

	else if(HLIB_STD_StrCaseCmp(signalName,"Channel+") == 0)
		HAPI_SendRemoteRCUKey(DIKS_CHANNEL_UP);
	else if(HLIB_STD_StrCaseCmp(signalName,"Channel-") == 0)
		HAPI_SendRemoteRCUKey(DIKS_CHANNEL_DOWN);

	else if(HLIB_STD_StrCaseCmp(signalName,"Guide") == 0)
		HAPI_SendRemoteRCUKey(DIKS_EPG);

	else if(HLIB_STD_StrCaseCmp(signalName,"Mute") == 0)
		HAPI_SendRemoteRCUKey(DIKS_MUTE);

	else if(HLIB_STD_StrCaseCmp(signalName,"Soundtrack") == 0)
		HAPI_SendRemoteRCUKey(DIKS_AUDIO);

	else if(HLIB_STD_StrCaseCmp(signalName,"0") == 0)
		HAPI_SendRemoteRCUKey(DIKS_0);
	else if(HLIB_STD_StrCaseCmp(signalName,"1") == 0)
		HAPI_SendRemoteRCUKey(DIKS_1);
	else if(HLIB_STD_StrCaseCmp(signalName,"2") == 0)
		HAPI_SendRemoteRCUKey(DIKS_2);
	else if(HLIB_STD_StrCaseCmp(signalName,"3") == 0)
		HAPI_SendRemoteRCUKey(DIKS_3);
	else if(HLIB_STD_StrCaseCmp(signalName,"4") == 0)
		HAPI_SendRemoteRCUKey(DIKS_4);
	else if(HLIB_STD_StrCaseCmp(signalName,"5") == 0)
		HAPI_SendRemoteRCUKey(DIKS_5);
	else if(HLIB_STD_StrCaseCmp(signalName,"6") == 0)
		HAPI_SendRemoteRCUKey(DIKS_6);
	else if(HLIB_STD_StrCaseCmp(signalName,"7") == 0)
		HAPI_SendRemoteRCUKey(DIKS_7);
	else if(HLIB_STD_StrCaseCmp(signalName,"8") == 0)
		HAPI_SendRemoteRCUKey(DIKS_8);
	else if(HLIB_STD_StrCaseCmp(signalName,"9") == 0)
		HAPI_SendRemoteRCUKey(DIKS_9);

	else if(HLIB_STD_StrCaseCmp(signalName,"Red") == 0)
		HAPI_SendRemoteRCUKey(DIKS_RED);
	else if(HLIB_STD_StrCaseCmp(signalName,"Green") == 0)
		HAPI_SendRemoteRCUKey(DIKS_GREEN);
	else if(HLIB_STD_StrCaseCmp(signalName,"Yellow") == 0)
		HAPI_SendRemoteRCUKey(DIKS_YELLOW);
	else if(HLIB_STD_StrCaseCmp(signalName,"Blue") == 0)
		HAPI_SendRemoteRCUKey(DIKS_BLUE);

	else if(HLIB_STD_StrCaseCmp(signalName,"Exit") == 0)
		HAPI_SendRemoteRCUKey(DIKS_EXIT);

	else if(HLIB_STD_StrCaseCmp(signalName,"Subtitle") == 0)
		HAPI_SendRemoteRCUKey(DIKS_SUBTITLE);
	else if(HLIB_STD_StrCaseCmp(signalName,"Teletext") == 0)
		HAPI_SendRemoteRCUKey(DIKS_TEXT);
	else if(HLIB_STD_StrCaseCmp(signalName,"Rec") == 0)
		HAPI_SendRemoteRCUKey(DIKS_RECORD);
	else if(HLIB_STD_StrCaseCmp(signalName,"Stop") == 0)
		HAPI_SendRemoteRCUKey(DIKS_STOP);
	else
		HxLOG_Warning("Can not find key value!! \r\n");

	hError = ERR_OK;

	return hError;
}

HERROR local_RCUEMUL_Execute(void)
{
	HERROR				hError = ERR_FAIL;

	HINT32				i,j = 0;
	HINT32				scheduleAction_num,macro_num = 0;

	for(i=0;i<s_scheduleAction_count;i++)
	{
		if(s_scheduleAction[i].enable == 1)
		{
			scheduleAction_num = i;
			break;
		}
	}

	for(i=0;i<s_macro_count;i++)
	{
		if(HxSTD_StrCmp(s_macro[i].name,s_scheduleAction[scheduleAction_num].macroName)==0)
		{
			macro_num = i;
			break;
		}
	}

	for (j = 0; j < s_macro[macro_num].macroActionCount; j++)
	{
		printf("RCUEMUL] signalname: [%s], macro_num: %d\n", s_macro[macro_num].pMacroAction[j].signalName, macro_num);
	}

	i=1;
	while(1)
	{

		for(j=0;j<s_macro[macro_num].macroActionCount;j++)
		{
			sleep(s_macro[macro_num].pMacroAction[j].pause/1000);

			printf("RCUEMUL] Execute [%s] after %d msec sleep\n", s_macro[macro_num].pMacroAction[j].signalName, s_macro[macro_num].pMacroAction[j].pause);

			hError = local_RCUEMUL_ExecuteAction(s_macro[macro_num].pMacroAction[j].signalName);
			if(hError != ERR_OK)
			{
				HxLOG_Warning("error!!!! RCUEMUL_ExecuteAction\r\n");
				return ERR_FAIL;
			}
		}

		if(s_scheduleAction[scheduleAction_num].repeatIndefinitely == FALSE && i==s_scheduleAction[scheduleAction_num].repeatCount)
			break;

		sleep(s_macro[macro_num].pause/1000);

		i++;
	}

	hError = ERR_OK;

	return hError;

}

HERROR local_RCUEMUL_SetMacroAction(HxXMLDOC hXmlDocHandle, HxList_t *lMacroAction, HINT32 i)
{
	HERROR				hError = ERR_FAIL;

	HxXMLELEMENT		hMacroAction = NULL;
	HxXMLELEMENT		hSignal = NULL;
	HxXMLELEMENT		hSignalName = NULL;
	HxXMLELEMENT		hPause = NULL;

	HxList_t			*lSignal = NULL;
	HxList_t			*lSignalName = NULL;
	HxList_t			*lPause = NULL;

	int					j;
	HUINT8				*childContents= NULL;

	s_macro[i].macroActionCount = HLIB_LIST_Length(lMacroAction);
	s_macro[i].pMacroAction = (MacroAction_t *)HLIB_STD_MemAlloc(sizeof(MacroAction_t)*s_macro[i].macroActionCount);
	HxSTD_MemSet(s_macro[i].pMacroAction , 0, sizeof(MacroAction_t)*s_macro[i].macroActionCount);

	j = 0;
	while(lMacroAction != NULL)
	{
		hMacroAction = (HxXMLELEMENT)HLIB_LIST_Data(lMacroAction);
		HLIB_XML_FindChildElementByName(hMacroAction, "Pause", &lPause);
		hPause = (HxXMLELEMENT)HLIB_LIST_Data(lPause);
		HLIB_XML_GetElementContents(hXmlDocHandle,hPause,&childContents);
		s_macro[i].pMacroAction[j].pause = HLIB_STD_StrToINT32(childContents);

		HLIB_XML_FindChildElementByName(hMacroAction, "Signal", &lSignal);
		hSignal = (HxXMLELEMENT)HLIB_LIST_Data(lSignal);

		HLIB_XML_FindChildElementByName(hSignal, "Name", &lSignalName);
		hSignalName = (HxXMLELEMENT)HLIB_LIST_Data(lSignalName);
		HLIB_XML_GetElementContents(hXmlDocHandle,hSignalName,&childContents);
		s_macro[i].pMacroAction[j].signalName = HLIB_STD_StrDup(childContents);
		lMacroAction = HLIB_LIST_Next(lMacroAction);
		j++;
	}

	if(childContents != NULL)
		HLIB_STD_MemFree(childContents);
	if(lSignal != NULL)
		HLIB_LIST_RemoveAll(lSignal);
	if(lSignalName != NULL)
		HLIB_LIST_RemoveAll(lSignalName);
	if(lPause != NULL)
		HLIB_LIST_RemoveAll(lPause);

	hError = ERR_OK;

	return hError;
}

HERROR local_RCUEMUL_SetMacro(HxXMLDOC hXmlDocHandle, HxList_t	*lMacro)
{
	HERROR				hError = ERR_FAIL;

	HxXMLELEMENT		hMacro = NULL;
	HxXMLELEMENT		hName = NULL;
	HxXMLELEMENT		hPause = NULL;
	HxXMLELEMENT		hMacroActions = NULL;

	HxList_t			*lName = NULL;
	HxList_t			*lPause = NULL;
	HxList_t			*lMacroActions = NULL;
	HxList_t			*lMacroAction = NULL;

	HINT32				i=0;

	HUINT8				*childContents= NULL;

	s_macro_count = HLIB_LIST_Length(lMacro);

	s_macro = (Macro_t *)HLIB_STD_MemAlloc(sizeof(Macro_t)*s_macro_count);
	HxSTD_MemSet(s_macro , 0,sizeof(Macro_t)*s_macro_count);

	while(lMacro != NULL)
	{
		hMacro = (HxXMLELEMENT)HLIB_LIST_Data(lMacro);

		HLIB_XML_FindChildElementByName(hMacro, "Pause", &lPause);
		hPause = (HxXMLELEMENT)HLIB_LIST_Data(lPause);
		HLIB_XML_GetElementContents(hXmlDocHandle,hPause,&childContents);
		s_macro[i].pause = HLIB_STD_StrToINT32(childContents);

		HLIB_XML_FindChildElementByName(hMacro, "Name", &lName);
		hName = (HxXMLELEMENT)HLIB_LIST_Data(lName);
		HLIB_XML_GetElementContents(hXmlDocHandle,hName,&childContents);
		s_macro[i].name = HLIB_STD_StrDup(childContents);

		HLIB_XML_FindChildElementByName(hMacro, "MacroActions", &lMacroActions);
		hMacroActions = (HxXMLELEMENT)HLIB_LIST_Data(lMacroActions);

		HLIB_XML_FindChildElementByName(hMacroActions, "MacroAction", &lMacroAction);

		hError = local_RCUEMUL_SetMacroAction(hXmlDocHandle, lMacroAction, i);
		if(hError != ERR_OK)
		{
			HxLOG_Warning("error!!!! RCUEMUL_SetMacroAction\r\n");
			return ERR_FAIL;
		}

		lMacro = HLIB_LIST_Next(lMacro);
		i++;
	}

	if(childContents != NULL)
		HLIB_STD_MemFree(childContents);
	if(lName != NULL)
			HLIB_LIST_RemoveAll(lName);
	if(lPause != NULL)
		HLIB_LIST_RemoveAll(lPause);
	if(lMacroActions != NULL)
		HLIB_LIST_RemoveAll(lMacroActions);
	if(lMacroAction != NULL)
		HLIB_LIST_RemoveAll(lMacroAction);

	hError = ERR_OK;

	return hError;
}

HERROR local_RCUEMUL_SetScheduleAction(HxXMLDOC hXmlDocHandle, HxList_t	*lScheduledAction)
{
	HERROR				hError = ERR_FAIL;

	HxXMLELEMENT		hScheduledAction = NULL;
	HxXMLELEMENT		hEnabled = NULL;
	HxXMLELEMENT		hMacroName = NULL;
	HxXMLELEMENT		hrepeatCount = NULL;
	HxXMLELEMENT		hrepeatIndefinitely = NULL;

	HxList_t			*lEnabled = NULL;
	HxList_t			*lMacroName = NULL;
	HxList_t			*lHrepeatCount = NULL;
	HxList_t			*lRepeatIndefinitely = NULL;

	HINT32				i=0;

	HUINT8				*childContents= NULL;

	s_scheduleAction_count = HLIB_LIST_Length(lScheduledAction);

	s_scheduleAction = (ScheduleAction_t *)HLIB_STD_MemAlloc(sizeof(ScheduleAction_t)*s_scheduleAction_count);
	HxSTD_MemSet(s_scheduleAction , 0,sizeof(ScheduleAction_t)*s_scheduleAction_count);

	while(lScheduledAction != NULL)
	{
		hScheduledAction = (HxXMLELEMENT)HLIB_LIST_Data(lScheduledAction);

		HLIB_XML_FindChildElementByName(hScheduledAction, "Enabled", &lEnabled);

		hEnabled = (HxXMLELEMENT)HLIB_LIST_Data(lEnabled);
		HLIB_XML_GetElementContents(hXmlDocHandle,hEnabled,&childContents);

		if(HxSTD_StrCmp(childContents,"true") == 0 || HxSTD_StrCmp(childContents,"TRUE") == 0 || HxSTD_StrCmp(childContents,"True") == 0 )
			s_scheduleAction[i].enable = 1;
		else s_scheduleAction[i].enable = 0;

		HLIB_XML_FindChildElementByName(hScheduledAction, "MacroName", &lMacroName);
		hMacroName = (HxXMLELEMENT)HLIB_LIST_Data(lMacroName);
		HLIB_XML_GetElementContents(hXmlDocHandle,hMacroName,&childContents);
		s_scheduleAction[i].macroName = HLIB_STD_StrDup(childContents);

		HLIB_XML_FindChildElementByName(hScheduledAction, "RepeatIndefinitely", &lRepeatIndefinitely);
		hrepeatIndefinitely = (HxXMLELEMENT)HLIB_LIST_Data(lRepeatIndefinitely);
		HLIB_XML_GetElementContents(hXmlDocHandle,hrepeatIndefinitely,&childContents);
		if(HxSTD_StrCmp(childContents,"true") == 0 || HxSTD_StrCmp(childContents,"TRUE") == 0 || HxSTD_StrCmp(childContents,"True") == 0 )
			s_scheduleAction[i].repeatIndefinitely = 1;
		else s_scheduleAction[i].repeatIndefinitely = 0;

		HLIB_XML_FindChildElementByName(hScheduledAction, "RepeatCount", &lHrepeatCount);
		hrepeatCount = (HxXMLELEMENT)HLIB_LIST_Data(lHrepeatCount);
		HLIB_XML_GetElementContents(hXmlDocHandle,hrepeatCount,&childContents);
		s_scheduleAction[i].repeatCount = HLIB_STD_StrToINT32(childContents);

		lScheduledAction = HLIB_LIST_Next(lScheduledAction);

		i++;

	}

	if(childContents != NULL)
		HLIB_STD_MemFree(childContents);
	if(lEnabled != NULL)
		HLIB_LIST_RemoveAll(lEnabled);
	if(lMacroName != NULL)
		HLIB_LIST_RemoveAll(lMacroName);
	if(lHrepeatCount != NULL)
		HLIB_LIST_RemoveAll(lHrepeatCount);
	if(lRepeatIndefinitely != NULL)
		HLIB_LIST_RemoveAll(lRepeatIndefinitely);

	hError = ERR_OK;

	return hError;
}

HERROR local_RCUEMUL_FreeTxtFileAction(void)
{
	HERROR			lRet = ERR_FAIL;
	HINT32			i = 0;

	if(s_userKeyAction!= NULL)
	{
		for(i=0;i<s_userKeyAction_count;i++)
		{
			if(s_userKeyAction[i].signalName != NULL)
				HLIB_STD_MemFree(s_userKeyAction[i].signalName);
		}
		HLIB_STD_MemFree(s_userKeyAction);
	}

	lRet = ERR_OK;

	return lRet;
}

HERROR local_RCUEMUL_ExecuteTxtFileAction(void)
{
	HERROR				hError = ERR_FAIL;
	HINT32				testCount = 0, userKeyAction = 0;

	while(1)
	{
		printf("\n\t\t @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		printf("\t\t @@@@@ Key Repeat Test (%dth) \n", testCount);
		printf("\t\t @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

		for(userKeyAction=0; userKeyAction<s_userKeyAction_count; userKeyAction++)
		{
			printf("RCUEMUL] Execute userKeyAction[%02d] : [%s]\n", userKeyAction, s_userKeyAction[userKeyAction].signalName);

			hError = local_RCUEMUL_ExecuteAction(s_userKeyAction[userKeyAction].signalName);
			if(hError != ERR_OK)
			{
				HxLOG_Warning("error!!!! RCUEMUL_ExecuteAction\r\n");
				return ERR_FAIL;
			}

			printf("RCUEMUL] User aging interval [%d sec] before executes the next key.\n", s_userKeyAction[userKeyAction].pause);
			sleep(s_userKeyAction[userKeyAction].pause);
		}

		if(testCount == MAX_REPEAT_TEST_COUNT)
		{
			printf("\n\t\t @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
			printf("\t\t @@@@@ Finish Key Repeat Test!\n");
			printf("\t\t @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
			break;
		}

		testCount++;
	}

	hError = ERR_OK;

	return hError;
}

HERROR local_RCUEMUL_ParseScriptTxtFile(void)
{
	HERROR				hError = ERR_FAIL;
	HCHAR				*szTxtData[MAX_CHAR_NUMBER_OF_USER_KEY];
	HxFILE_t			hTxtDocHandle = NULL;
	HCHAR 				*ret = NULL;
	HCHAR				*token, *save;
	HINT32				userKeyAction = 0;

	hTxtDocHandle = HLIB_FILE_Open(TXT_USB_SCRIPT_FILE, "rb");
	if (hTxtDocHandle == NULL)
	{
		hTxtDocHandle = HLIB_FILE_Open(TXT_DEFAULT_SCRIPT_FILE, "rb");
		if (hTxtDocHandle == NULL)
		{
			HxLOG_Error("[%s] HLIB_FILE_Open fail!!\n",__FUNCTION__);
			return ERR_FAIL;
		}
		else
		{
			printf("RCUEMUL] Read TXT file from the default path.\n");
		}
	}
	else
	{
		printf("RCUEMUL] Read TXT file from USB.\n");
	}

	while( !HLIB_FILE_EOF(hTxtDocHandle) )
	{
		ret = HLIB_FILE_GetString(hTxtDocHandle, (const HUINT8 *)szTxtData, sizeof(szTxtData));
		if(ret != NULL)
		{
			userKeyAction++;
		}
	}
	s_userKeyAction_count = userKeyAction;

	s_userKeyAction = (UserKey_t *)HLIB_STD_MemAlloc(sizeof(UserKey_t)*s_userKeyAction_count);
	HxSTD_MemSet(s_userKeyAction , 0,sizeof(UserKey_t)*s_userKeyAction_count);

	hError = HLIB_FILE_Seek(hTxtDocHandle, 0, eHxFILE_SEEK_SET, NULL);
	if (hError != ERR_OK)
	{
		HxLOG_Error("error occurred in HLIB_FILE_Seek(%d).\n", hError);
		HLIB_FILE_Close(hTxtDocHandle);
		return ERR_FAIL;
	}

	for(userKeyAction=0; userKeyAction<s_userKeyAction_count; userKeyAction++)
	{
		ret = HLIB_FILE_GetString(hTxtDocHandle, (const HUINT8 *)szTxtData, sizeof(szTxtData));
		if(ret != NULL)
		{
			token = HxSTD_strtok_r(szTxtData, ",", &save);
			if(token != NULL)
			{
				s_userKeyAction[userKeyAction].signalName = HLIB_STD_StrDup(token);
				if(HxSTD_StrCmp(s_userKeyAction[userKeyAction].signalName,"Power") == 0)
				{
					/* The rcuemulator has a difficulty in controlling the power saving mode. */
					/* Thus, setting the default power's interval as 10 seconds that would be modified if there is a further request. */
					s_userKeyAction[userKeyAction].pause = 10;
				}
				else
				{
					s_userKeyAction[userKeyAction].pause = HLIB_STD_StrToINT32(save);
				}
			}

			printf("RCUEMUL] userKeyAction[%02d] : signalName [%s]\n", userKeyAction, s_userKeyAction[userKeyAction].signalName);
		}
	}

	printf("RCUEMUL] Parsed Total User Aging Key Action Numbers (%d) \n", s_userKeyAction_count);

	HLIB_FILE_Close(hTxtDocHandle);

	hError = ERR_OK;

	return hError;
}


HERROR local_RCUEMUL_ParseScriptXMLFile(void)
{
	HERROR				hError = ERR_FAIL;

	HxXMLDOC			hXmlDocHandle = NULL;

	HxXMLELEMENT		hSchedulerState = NULL;
	HxXMLELEMENT		hActions = NULL;
	HxXMLELEMENT		hMacros = NULL;

	HxList_t			*lActions = NULL;
	HxList_t			*lScheduledAction = NULL;
	HxList_t			*lMacros = NULL;
	HxList_t			*lMacro = NULL;

	hError = HLIB_XML_InitParser();
	if(hError != ERR_OK)
	{
		HxLOG_Warning("error!!!! HxXML_InitParser\r\n");
		return ERR_FAIL;
	}

	hError = HLIB_XML_ParseFile(XML_USB_SCRIPT_FILE,&hXmlDocHandle);
	if(hError != ERR_OK)
	{
		hError = HLIB_XML_ParseFile(XML_DEFAULT_SCRIPT_FILE,&hXmlDocHandle);
		if(hError != ERR_OK)
		{
			HxLOG_Warning("error!!!! HxXML_ParseFile\r\n");
			return ERR_FAIL;
		}
		else
		{
			printf("RCUEMUL] Read XML file from the default path.\n");
		}
	}
	else
	{
		printf("RCUEMUL] Read XML file from USB.\n");
	}


	hError = HLIB_XML_GetRootElement(hXmlDocHandle, &hSchedulerState);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("error!!!! HxXML_GetRootElement\r\n");
		return ERR_FAIL;
	}

	HLIB_XML_FindChildElementByName(hSchedulerState, "Actions", &lActions);
	hActions = (HxXMLELEMENT)HLIB_LIST_Data(lActions);

	HLIB_XML_FindChildElementByName(hActions, "ScheduledAction", &lScheduledAction);

	hError = local_RCUEMUL_SetScheduleAction(hXmlDocHandle, lScheduledAction);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("error!!!! RCUEMUL_SetScheduleAction\r\n");
		return ERR_FAIL;
	}
	HLIB_XML_FindChildElementByName(hSchedulerState, "Macros", &lMacros);
	hMacros = (HxXMLELEMENT)HLIB_LIST_Data(lMacros);

	HLIB_XML_FindChildElementByName(hMacros, "Macro", &lMacro);

	hError = local_RCUEMUL_SetMacro(hXmlDocHandle, lMacro);
	if(hError != ERR_OK)
	{
		HxLOG_Warning("error!!!! RCUEMUL_SetMacro\r\n");
		return ERR_FAIL;
	}

	hError = ERR_OK;

	return hError;
}

HERROR local_RCUEMUL_Free(void)
{
	HERROR			lRet = ERR_FAIL;

	HINT32			i,j=0;

	if(s_scheduleAction != NULL)
	{
		for(i=0;i<s_scheduleAction_count;i++)
		{
			if(s_scheduleAction[i].macroName != NULL)
				HLIB_STD_MemFree(s_scheduleAction[i].macroName);
		}
		HLIB_STD_MemFree(s_scheduleAction);
	}

	if(s_macro != NULL)
	{
		for(i=0;i<s_macro_count;i++)
		{
			if(s_macro[i].name != NULL)
			{
				HLIB_STD_MemFree(s_macro[i].name);
			}

			if(s_macro[i].pMacroAction != NULL)
			{
				for(j=0;j<s_macro[i].macroActionCount;j++)
				{
					HLIB_STD_MemFree(s_macro[i].pMacroAction[j].signalName);
				}
				HLIB_STD_MemFree(s_macro[i].pMacroAction);
			}
		}
		HLIB_STD_MemFree(s_macro);
	}

	lRet = ERR_OK;

	return lRet;
}

int main(void)
{

	HERROR	lRet = ERR_OK;

	VK_Init();
	HLIB_DBG_Init();

	/* Parsing priority in order of importance list as following.
	   1. .txt file on USB
	   2. The default .txt file
	   3. .xml file on USB
	   4. The default .xml file	*/

	lRet = local_RCUEMUL_ParseScriptTxtFile();
	if(lRet != ERR_FAIL)
	{
		HxLOG_Warning("Txt file is found. Executing the txt file.\r\n");
		lRet = local_RCUEMUL_ExecuteTxtFileAction();
		if(lRet == ERR_FAIL)
		{
			HxLOG_Warning("!!!error!!! local_RCUEMUL_ExecuteTxtFileAction\r\n");
		}

		lRet = local_RCUEMUL_FreeTxtFileAction();
		if(lRet == ERR_FAIL)
		{
			HxLOG_Warning("!!!error!!! local_RCUEMUL_FreeTxtFileAction\r\n");
		}
	}
	else
	{
		HxLOG_Warning("A txt file is not found. Try to find a XML file.\r\n");

		lRet = local_RCUEMUL_ParseScriptXMLFile();
		if(lRet == ERR_FAIL)
		{
			HxLOG_Warning("!!!error!!! RCUEMUL_ParseScriptXMLFile\r\n");
		}

		HxLOG_Warning("XML file is found. Executing the XML file.\r\n");

		lRet = local_RCUEMUL_Execute();
		if(lRet == ERR_FAIL)
		{
			HxLOG_Warning("!!!error!!! RCUEMUL_Execute\r\n");
		}

		lRet = local_RCUEMUL_Free();
		if(lRet == ERR_FAIL)
		{
			HxLOG_Warning("!!!error!!! RCUEMUL_Free\r\n");
		}
	}

	return 0;

}

