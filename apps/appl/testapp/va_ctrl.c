/*
  FILE : stub_ctrl.c
  PURPOSE: This file is a stub for linking tests.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "va_ctrl.h"

INT VA_CTRL_GetProgramInformation( DWORD dwAcsId, tVA_CTRL_ProgramInformation *pProgramInfo )
{
  return 0;
}

INT   VA_CTRL_Init(const tVA_CTRL_ConfigurationParameters *pParameters)
{
	printf("\n ::::VA_CTRL_Init 3 ::: \n");	
	return kVA_OK;
}

void  VA_CTRL_Start(void)
{
	printf("\n ::::VA_CTRL_Start 3 ::: \n");	
}

INT   VA_CTRL_Stop(void)
{
	return kVA_OK;
}

INT   VA_CTRL_OpenAcsInstance(DWORD dwAcsId, tVA_CTRL_AcsMode eAcsMode)
{
	return kVA_OK;
}

INT   VA_CTRL_ChangeAcsInstanceMode(DWORD dwAcsId, tVA_CTRL_AcsMode eNewAcsMode)
{
	return kVA_OK;
}

INT   VA_CTRL_CloseAcsInstance(DWORD dwAcsId)
{
	return kVA_OK;
}

INT   VA_CTRL_TsChanged(DWORD dwAcsId)
{
	return kVA_OK;
}

INT   VA_CTRL_CatUpdated(DWORD dwAcsId, UINT32 uiCatLength, BYTE * pCat)
{
	return kVA_OK;
}

INT   VA_CTRL_SwitchOnProgram(DWORD dwAcsId, UINT32 uiPmtLength, BYTE * pPmt)
{
	return kVA_OK;
}
	
INT   VA_CTRL_SwitchOffProgram(DWORD dwAcsId)
{
	return kVA_OK;
}
	
INT   VA_CTRL_PmtUpdated(DWORD dwAcsId, UINT32 uiPmtLength, BYTE * pPmt)
{
	return kVA_OK;
}
	
INT   VA_CTRL_AddStream(DWORD dwAcsId, DWORD dwStbStreamHandle, DWORD dwPid, tVA_CTRL_StreamNotificationCallback pfVaStreamNotificationCallback)
{
	return kVA_OK;
}

INT   VA_CTRL_QueryStream(DWORD dwAcsId,
                          DWORD dwStbStreamHandle,
                          DWORD dwPid,
                          tVA_CTRL_QueryStreamNotificationCallback pfVaQueryStreamNotificationCallback)
{
	return kVA_OK;
}
                          
INT   VA_CTRL_UpdateStream(DWORD dwStbStreamHandle, DWORD dwPid)
{
	printf("\n ::::VA_CTRL_UpdateStream 3 ::: \n");
	return kVA_OK;
}
	
INT   VA_CTRL_RemoveStream(DWORD dwStbStreamHandle)
{
	printf("\n ::::VA_CTRL_RemoveStream 3 ::: \n");
	return kVA_OK;
}
	
INT   VA_CTRL_SetSoidBlackList(DWORD dwAcsId, UINT32 uiSoidCount, tVA_CTRL_Soid * pSoidBlackList)
{
	return kVA_OK;
}

/* End of File */
