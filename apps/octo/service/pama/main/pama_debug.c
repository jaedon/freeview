/**************************************************************
 *	@file		Pama_debug.c
 *	PAMA for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013/06/13
 *	@author			humax
 *	@brief			PAMA for OCTO 2.0 Project
 **************************************************************/
#if defined(CONFIG_DEBUG)
 void PamaSendMsgCmd(PAMA_MSG_e eMsg)
 {
	 pama_t* pama = pama_getInstance();
	 PAMA_MSG_t 	 msg;

#define PamaCmdDefaultWatingTime 3000;
	 HxSTD_MemSet(&msg, 0, sizeof(PAMA_MSG_t));

	 msg.eMsg = eMsg;
	 msg.pszProcessName = HLIB_STD_StrDup("PAMA CMD");
	 msg.ulParam1 =   PamaCmdDefaultWatingTime;

	 HLIB_MSG_Send(pama->ulMainMsgQueue, (void*)&msg, sizeof(PAMA_MSG_t));
#undef 	PamaCmdDefaultWatingTime
	 pama_releaseInstance(pama);
 }

static HINT32 PamaCmd(void *szArgs)
{
	HCHAR			*szArg_kind = NULL;
	PAMA_MSG_e 		ePamaMsg = ePAMA_MSG_UNKNOWN;

	szArg_kind = HLIB_CMD_GetNextArg((HCHAR **)&szArgs);
	if( szArg_kind != NULL )
	{
		HLIB_STD_StrUpper(szArg_kind);

		if(HxSTD_StrCmp(szArg_kind, "RO") == 0)
			ePamaMsg = ePAMA_MSG_REQUEST_OPERATION;
		else if(HxSTD_StrCmp(szArg_kind, "RS") == 0)
			ePamaMsg = ePAMA_MSG_REQUEST_SHUTDOWN;
		else if(HxSTD_StrCmp(szArg_kind, "RR") == 0)
			ePamaMsg = ePAMA_MSG_REQUEST_REBOOT;
		else if(HxSTD_StrCmp(szArg_kind, "WR") == 0)
			ePamaMsg = ePAMA_MSG_WAIT_SHUTDOWN;
		else if(HxSTD_StrCmp(szArg_kind, "WS") == 0)
			ePamaMsg = ePAMA_MSG_WAIT_REBOOT;
	}

	HLIB_CMD_Printf("Pama Send Command [%s]\n", MSG2STR(ePamaMsg));

	if(	ePamaMsg != ePAMA_MSG_UNKNOWN)
	{
		PamaSendMsgCmd(ePamaMsg);
	}
	return HxCMD_OK;
}




static HINT32 PamaCheckStatus(void *szArgs)
{
	pama_t* pama = pama_getInstance();
	if(pama)
	{
		HLIB_CMD_Printf("====> Main App is %s Ready\n", pama->bMainAppReady?"":"Not");
		HLIB_CMD_Printf("\t\tCUR STATUS[%s] REQ STATUS[%s] \n\t\tShutDown Request [%s] Reboot Request[%s] \t\tTime[%d] Threshold[%d]\n"
			, STATUS2STR(pama->eCurrentMode), STATUS2STR(pama->eRequestMode), pama->bWaitShutDownFlag?"YES":"NO", pama->bWaitRebootFlag?"YES":"NO"
			, HLIB_STD_GetSystemTick(), pama->ulWaitThreshold);
		 pama_releaseInstance(pama);
	}
	return HxCMD_OK;
}
#endif

void	Pama_command_Init(void)
{
#if defined(CONFIG_DEBUG)
#define	cmdHandle		"Pamacmd"
	HLIB_CMD_RegisterWord(cmdHandle, PamaCheckStatus,
					 (HINT8 *)"status",
					 (HINT8 *)"print Pama status",
					 (HINT8 *)"status");

	HLIB_CMD_RegisterWord(cmdHandle, PamaCmd,
					 (HINT8 *)"msg",
					 (HINT8 *)"RO : request operation, RS : request ShutDown, RR : request ReBoot, WS : Wait Shutdown, WR : Wait Reboot",
					 (HINT8 *)"msg ro, msg rs");
#endif
}



