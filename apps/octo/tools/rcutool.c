/*****************************************
	RCU Recorder and Player
******************************************/
#include	<hlib.h>
#include	<hapi.h>

static void	rcutool_printhelp(void)
{
	printf("usages : \n");
	printf("         rcutool -r(-record) recorder.bin\n");
	printf("              All key is recording in recorder.bin\n");
	printf("         rcutool -p(-play) recorder.bin\n");
	printf("              Key is playing in recorder.bin\n");
	printf("\n");
}

typedef struct
{
	HINT32	nKeyType;
	HINT32	nKeySymbol;
	HUINT32	ulTick;
} keyinfo_t;

static HUINT32	s_ulStartTick;
static HxList_t	*s_pstRecorderList = NULL;
static HCHAR	s_szFileName[1024];
static HBOOL	s_bPlayerMode = FALSE;

static HERROR	rcutool_key_received(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HINT32	nKeyType;
	HINT32	nKeySymbol;
	keyinfo_t	stKeyInfo;

	nKeyType = HxOBJECT_INT(apArgv[0]);
	nKeySymbol = HxOBJECT_INT(apArgv[1]);

	//	Key Pressed ONLY.
	if (nKeyType == 1)
	{
		stKeyInfo.nKeyType = nKeyType;
		stKeyInfo.nKeySymbol = nKeySymbol;
		stKeyInfo.ulTick = HLIB_STD_GetSystemTick() - s_ulStartTick;
		printf(HxANSI_COLOR_YELLOW("KEY RECORDER : Symbol [%p]\n"), nKeySymbol);
		s_pstRecorderList = HLIB_LIST_Append(s_pstRecorderList, (void*)HLIB_STD_MemDup(&stKeyInfo, sizeof(keyinfo_t)));
	}
}

static void	rcutool_save_recorder(void)
{
	FILE		*file;
	HxList_t	*list;
	keyinfo_t	*info;
	HINT32		jw;
	HCHAR		*pszJson;

	if (s_bPlayerMode == FALSE)
	{
		printf(HxANSI_COLOR_YELLOW("RCUTOOL is stopped\n")"\n");
		jw = HLIB_JSONWRITER_Open();
		HxJSONWRITER_ObjectBegin(jw);
		HxJSONWRITER_Array(jw, "inputs");
		list = s_pstRecorderList;
		while (list)
		{
			info = (keyinfo_t*)HLIB_LIST_Data(list);

			HxJSONWRITER_ObjectBegin(jw);
				HxJSONWRITER_Integer(jw, "kt", info->nKeyType);
				HxJSONWRITER_Integer(jw, "kv", info->nKeySymbol);
				HxJSONWRITER_Integer(jw, "tk", info->ulTick);
			HxJSONWRITER_ObjectEnd(jw);
			list = HLIB_LIST_Next(list);
		}
		HxJSONWRITER_ArrayEnd(jw);
		HxJSONWRITER_ObjectEnd(jw);

		pszJson = HLIB_JSONWRITER_GetJSON(jw);
		file = fopen(s_szFileName, "wt");
		if (file)
		{
			fprintf(file, pszJson);
			fclose(file);
		}

		HLIB_JSONWRITER_Close(jw);

		printf("key is logged... \n");
	}

	exit(0);
}

static HBOOL	s_bExitFlag = FALSE;

static void	*key_receiver(void *arg)
{
	char	szStr[256];

	while (1)
	{
		gets(szStr);
		if (strstr(szStr, "exit"))
			s_bExitFlag = TRUE;
		else
		{
			printf("for exit : input exit\n");
			printf("# ");
		}
	}
	return NULL;
}

static void	rcutool_key_recorder(char *pszFileName)
{
	int		tid;
	FILE	*file;

	pthread_create(&tid, NULL, key_receiver, (void*)NULL);
	printf(" -- exit : Stopped key recording..\n");
	printf(HxANSI_COLOR_YELLOW(" ###### 5")"\n");		HLIB_STD_TaskSleep(1000);
	printf(HxANSI_COLOR_YELLOW(" ###### 4")"\n");		HLIB_STD_TaskSleep(1000);
	printf(HxANSI_COLOR_YELLOW(" ###### 3")"\n");		HLIB_STD_TaskSleep(1000);
	printf(HxANSI_COLOR_YELLOW(" ###### 2")"\n");		HLIB_STD_TaskSleep(1000);
	printf(HxANSI_COLOR_YELLOW(" ###### 1")"\n");		HLIB_STD_TaskSleep(1000);
	printf(HxANSI_COLOR_YELLOW(" Start KEY recording...")"\n");

	HAPI_ConnectSystemSignal("hama:onInputNotify", "ii", NULL, rcutool_key_received);
	s_ulStartTick = HLIB_STD_GetSystemTick();
}

static void	rcutool_key_player(char *pszFileName)
{
	HxJSON_t	root, obj;
	HxJSON_t	array;
	keyinfo_t	stInfo, *pstInfo, *pstNext;
	HxList_t	*list;

	root = HLIB_JSON_DecodeFile(pszFileName);
	if (root == NULL)
	{
		printf("<--- fail to load [%s]\n", pszFileName);
		return;
	}

	array = HLIB_JSON_Object_GetArray(root, "inputs");
	if (array == NULL)
	{
		printf("<--- no key registered! [%s]\n", pszFileName);
		return;
	}

	HxJSON_ARRAY_EACH(array, obj, {
		stInfo.nKeyType = HLIB_JSON_Object_GetInteger(obj, "kt");
		stInfo.nKeySymbol = HLIB_JSON_Object_GetInteger(obj, "kv");
		stInfo.ulTick = HLIB_JSON_Object_GetInteger(obj, "tk");

		s_pstRecorderList = HLIB_LIST_Append(s_pstRecorderList, (void*)HLIB_STD_MemDup(&stInfo, sizeof(keyinfo_t)));
	});
	HLIB_JSON_Delete(root);

	pstInfo = (keyinfo_t*)HLIB_LIST_Data(HLIB_LIST_Last(s_pstRecorderList));
	printf(" -- Step of KeyPlayer : [%d]\n", HLIB_LIST_Length(s_pstRecorderList));
	printf(" -- Total Duration    : [%d]sec\n", pstInfo->ulTick / 1000);
	printf(" -- Now run script ...\n");

	list = s_pstRecorderList;
	while (1)
	{
		list = s_pstRecorderList;
		while (list)
		{
			pstInfo = (keyinfo_t*)HLIB_LIST_Data(list);
			printf(HxANSI_COLOR_RED(" - Fire : [%d]\n"), pstInfo->nKeySymbol);
			HAPI_SendRemoteRCUKey(pstInfo->nKeySymbol);
			pstNext = (keyinfo_t*)HLIB_LIST_Data(HLIB_LIST_Next(list));
			if (pstNext)
			{
 				HLIB_STD_TaskSleep(pstNext->ulTick - pstInfo->ulTick);
			}
			list = HLIB_LIST_Next(list);
		}
		printf(HxANSI_COLOR_YELLOW("Key Player - wrap-around...\n"));
		HLIB_STD_TaskSleep(3000);
		printf(HxANSI_COLOR_CYAN("Restarting...\n"));
	}
}

int	main(int argc, char **argv)
{
	char	*pszOption;
	char	*pszFileName;
	HBOOL	bPlayer;

	printf("==========================================\n");
	printf("*          RCU Recorder and Player       *\n");
	printf("==========================================\n\n");

	if (argc != 3)
		goto err_rtn;

	pszOption = argv[1];
	pszFileName = argv[2];
	if (strstr(pszOption, "-r"))
		bPlayer = FALSE;
	else if (strstr(pszOption, "-p"))
		bPlayer = TRUE;
	else
		goto err_rtn;

	if (pszFileName == NULL)
		goto err_rtn;

	HxSTD_StrCpy(s_szFileName, pszFileName);
	s_bPlayerMode = bPlayer;

	VK_Init();
	HAPI_Init();

	if (bPlayer == FALSE)
	{
		rcutool_key_recorder(pszFileName);
		while (!s_bExitFlag)
			HLIB_STD_TaskSleep(100);
		rcutool_save_recorder();
	} else
	{
		rcutool_key_player(pszFileName);
	}

	return 0;

err_rtn:
	rcutool_printhelp();

	return 0;
}

