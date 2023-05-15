/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include "int_streaming.h"


/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/




/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

STATIC HxFILE_t s_fpDebugFile = NULL;


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC void	_plstreaming_Debug_PrintPids(const HINT32 nPIDsCount, const HUINT16 usPIDs[], const HCHAR *pszFunction, const HINT32 nLine)
{
	HxSTR_t		*pStr = NULL;
	HINT32		i = 0;
	HINT32		nBytes = 0;
	HUINT8		*pucStr = NULL;
	HCHAR 		*pszBuf = NULL;
	HINT32		nStrBufSize = 2048;


	pStr = HLIB_STR_New("pids=");
	for(i = 0; i < nPIDsCount; i++)
	{
		if((nPIDsCount - i) > 1)
		{
			pStr = HLIB_STR_AppendFormat(pStr, "%d,", usPIDs[i]);
		}
		else
		{
			pStr = HLIB_STR_AppendFormat(pStr, "%d", usPIDs[i]);
		}
	}

	nBytes = HLIB_STR_GetNumOfBytes(pStr);
	if(nBytes >= nStrBufSize || nBytes < 5)
	{
		HLIB_STR_Delete(pStr);
		HxLOG_Error("nBytes(%d) > nStrBufSize(%d)\r\n", nBytes, nStrBufSize);
		return;
	}

	pszBuf = HLIB_STD_MemCalloc(nStrBufSize);
	if(NULL == pszBuf)
	{
		HLIB_STR_Delete(pStr);
		return;
	}

	pucStr = HLIB_STR_GetBuffer(pStr);
	HxSTD_memcpy(pszBuf, pucStr, nBytes);
	pszBuf[nBytes] = '\0';

	HLIB_STR_Delete(pStr);


	HxLOG_Print("[%s:%d] nPIDsCount(%d) %s\r\n", pszFunction, nLine, nPIDsCount, pszBuf);
	HLIB_STD_MemFree(pszBuf);
}

STATIC void 	_plStreaming_Debug_PrintTuningParam(const DxTuneParam_t *pstTuningParam, const HCHAR *pszFunction, const HINT32 nLine)
{
	if(NULL != pstTuningParam)
	{
		HxLOG_Print("\r\n[%s:%d] \r\n========= TP Info ==========\r\n", pszFunction, nLine);

		HxLOG_HexIntDump(pstTuningParam->eDeliType);
		switch(pstTuningParam->eDeliType)
		{
		case eDxDELIVERY_TYPE_SAT:
			HxLOG_DecIntDump(pstTuningParam->sat.antennaType);
			HxLOG_DecIntDump(pstTuningParam->sat.ulRfInputId);
			HxLOG_DecIntDump(pstTuningParam->sat.ulTunerGroupId);

			HxLOG_DecIntDump(pstTuningParam->sat.antInfo.uid);
			HxLOG_StringDump(pstTuningParam->sat.antInfo.satType);
			HxLOG_StringDump(pstTuningParam->sat.antInfo.satName);
			HxLOG_DecIntDump(pstTuningParam->sat.antInfo.antType);
			switch(pstTuningParam->sat.antInfo.antType)
			{
			case eDxANT_TYPE_LNB_ONLY:
				HxLOG_Print("\teDxANT_TYPE_LNB_ONLY\r\n");
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.lnb.lnbVoltage);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.lnb.ulLnbFreq);
				break;

			case eDxANT_TYPE_DISEQC:
				HxLOG_Print("\eDxANT_TYPE_DISEQC\r\n");
				HxLOG_BoolDump(pstTuningParam->sat.antInfo.info.diseqc.b22kTone);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.diseqcInput);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.diseqcVersion);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.lnbVoltage);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.sSatLonggitude);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.toneBurst);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.ucMotorPosition);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.diseqc.ulLnbFreq);
				break;

			case eDxANT_TYPE_SCD:
				HxLOG_Print("\eDxANT_TYPE_SCD\r\n");
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.scd.b22kTone);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.scd.lnbVoltage);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.scd.scdInput);
				HxLOG_DecIntDump(pstTuningParam->sat.antInfo.info.scd.ulLnbFreq);
				break;
			default:
				break;
			}
			HxLOG_DecIntDump(pstTuningParam->sat.tuningInfo.ulFrequency);
			HxLOG_DecIntDump(pstTuningParam->sat.tuningInfo.ulSymbolRate);
			HxLOG_DxEnumDump(DxNAMEOF(DxSat_CodeRate_e), pstTuningParam->sat.tuningInfo.eFecCodeRate);
			HxLOG_DxEnumDump(DxNAMEOF(DxSat_Polarization_e), pstTuningParam->sat.tuningInfo.ePolarization);
			HxLOG_DxEnumDump(DxNAMEOF(DxSat_TransportSpec_e), pstTuningParam->sat.tuningInfo.eTransSpec);
			HxLOG_DecIntDump(pstTuningParam->sat.tuningInfo.antuid);
			HxLOG_StringDump(pstTuningParam->sat.tuningInfo.satType);
			HxLOG_DxEnumDump(DxNAMEOF(DxSat_RollOff_e), pstTuningParam->sat.tuningInfo.eRollOff);
			HxLOG_DxEnumDump(DxNAMEOF(DxSat_Pilot_e), pstTuningParam->sat.tuningInfo.ePilot);
			HxLOG_DxEnumDump(DxNAMEOF(DxSat_PskModulation_e), pstTuningParam->sat.tuningInfo.ePskMod);

			break;

		case eDxDELIVERY_TYPE_CAB:
			HxLOG_DecIntDump(pstTuningParam->cab.ulFrequency);
			HxLOG_DecIntDump(pstTuningParam->cab.ulSymbolRate);
			HxLOG_DecIntDump(pstTuningParam->cab.nTsId);
			HxLOG_DecIntDump(pstTuningParam->cab.nNetId);
			break;

		case eDxDELIVERY_TYPE_TER:
			HxLOG_DecIntDump(pstTuningParam->ter.ulFrequency);
			HxLOG_DecIntDump(pstTuningParam->ter.eBandWidth);
			HxLOG_DecIntDump(pstTuningParam->ter.deliveryType);
			HxLOG_DecIntDump(pstTuningParam->ter.ulTunerGroupId);
			HxLOG_DecIntDump(pstTuningParam->ter.eTransMode);
			break;

		default:
			break;
		}
		HxLOG_Print("======================================================\r\n");
	}
}

STATIC void 	_plStreaming_Debug_PrintScheduleInfo(const DxSchedule_t	*pstSchedule, const HCHAR *pszFunction, const HINT32 nLine)
{
	const DxRSV_Streaming_t	*pstDxStreaming = NULL;

	if(NULL != pstSchedule)
	{
		HxLOG_Print("\r\n[%s:%d] \r\n========= Schedule Info ==========\r\n", pszFunction, nLine);
		HxLOG_DecIntDump(pstSchedule->ulSlot);
		HxLOG_HexIntDump(pstSchedule->eRsvType);
		HxLOG_DecIntDump(pstSchedule->ulSvcUid);
		HxLOG_HexIntDump(pstSchedule->eRsvReady);
		HxLOG_DecIntDump(pstSchedule->stStartTime);
		HxLOG_DecIntDump(pstSchedule->ulDuration);
		HxLOG_HexIntDump(pstSchedule->eRepeat);
		HxLOG_HexIntDump(pstSchedule->ulRepeatDays);
		HxLOG_HexIntDump(pstSchedule->eStatus);
		HxLOG_BoolDump(pstSchedule->bExceptConflict);
		HxLOG_HexIntDump(pstSchedule->eEpgType);

		HxLOG_Print("pstDxStreaming = &pstSchedule->uExtInfo.stStrm\r\n");
		pstDxStreaming	= &pstSchedule->uExtInfo.stStrm;
		HxLOG_HexIntDump(pstDxStreaming->ulMsgId);
		HxLOG_DecIntDump(pstDxStreaming->ulPlayerPosition);
		HxLOG_HexIntDump(pstDxStreaming->eStreamType);
		HxLOG_BoolDump(pstDxStreaming->bTranscoding);
		HxLOG_BoolDump(pstDxStreaming->bPcrTsPacketInsert);
		HxLOG_DecIntDump(pstDxStreaming->nPIDsCount);
		HxLOG_HexIntDump(pstDxStreaming->eUpdate);

		_plstreaming_Debug_PrintPids(pstDxStreaming->nPIDsCount,  pstDxStreaming->usPIDs, pszFunction, nLine);
		_plStreaming_Debug_PrintTuningParam(&pstDxStreaming->stTuningParam, pszFunction, nLine);

		HxLOG_StringDump(pstDxStreaming->szStreamingKey);
		HxLOG_StringDump(pstDxStreaming->szProfileString);
		HxLOG_StringDump(pstDxStreaming->szAppDisplayString);
		HxLOG_Print("============================================================================\r\n");
	}
}


STATIC HCHAR *_plstreaming_Debug_GetSetupStatusStr(PLStreaming_SetupStatus_e	eSetupStatus)
{
	switch (eSetupStatus)
	{
 	ENUM_TO_STR(ePLStreaming_SetupStatus_None);
 	ENUM_TO_STR(ePLStreaming_SetupStatus_Start);
	ENUM_TO_STR(ePLStreaming_SetupStatus_Conflict);
 	ENUM_TO_STR(ePLStreaming_SetupStatus_End);

	default:
		break;
	}

	return "Unknown";
}

STATIC HCHAR *_plstreaming_Debug_GetHomResultStr(HOM_Result_e	eHomResult)
{
	switch (eHomResult)
	{
 	ENUM_TO_STR(eHOM_Result_Ok);
 	ENUM_TO_STR(eHOM_Result_Error);
 	ENUM_TO_STR(eHOM_Result_Error_Used);
 	ENUM_TO_STR(eHOM_Result_Error_NotInit);
 	ENUM_TO_STR(eHOM_Result_Error_NullPoint);
 	ENUM_TO_STR(eHOM_Result_Error_Invalid_Arguments);
 	ENUM_TO_STR(eHOM_Result_Error_Resource_Notfound);
 	ENUM_TO_STR(eHOM_Result_Error_Resource_Full);
 	ENUM_TO_STR(eHOM_Result_Error_InitFail);
	ENUM_TO_STR(eHOM_Result_Error_OutofMem);
 	ENUM_TO_STR(eHOM_Result_Error_Destroyed);
	ENUM_TO_STR(eHOM_Result_Ok_ButNotEffect);
	ENUM_TO_STR(eHOM_Result_Error_Conflict);
	ENUM_TO_STR(eHOM_Result_Error_CreateFail);

	ENUM_TO_STR(eHOM_Result_Max);

	default:
		break;
	}

	return "Unknown";
}




STATIC void	_plstreaming_Debug_PrintMem(HINT32 nPid)
{
#ifdef	OS_LINUX
	HCHAR	procName[256] = {0, };
	HCHAR	line[256];
	FILE	*fp;

	HxSTD_snprintf(procName, 256, "/proc/%d/status", nPid);

	if (fp = fopen(procName, "r"), NULL == fp)
	{
		HLIB_CMD_Printf("error to open file : [%s]\n", procName);
		return;
	}

	while (fgets(line, 255, fp))
	{
		if (HLIB_STD_StrCaseStartWith(line, "Name") || HLIB_STD_StrCaseStartWith(line, "VmRSS") || HLIB_STD_StrCaseStartWith(line, "VmSize"))
		{
			HLIB_CMD_Printf("%s", line);
		}
	}
	fclose(fp);
#endif
}


/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/


void 	PL_Streaming_Debug_PrintTuningParam(const DxTuneParam_t *pstTuningParam, const HCHAR *pszFunction, const HINT32 nLine)
{
	_plStreaming_Debug_PrintTuningParam(pstTuningParam, pszFunction, nLine);
}


void 	PL_Streaming_Debug_PrintScheduleInfo(const DxSchedule_t	*pstSchedule, const HCHAR *pszFunction, const HINT32 nLine)
{
	_plStreaming_Debug_PrintScheduleInfo(pstSchedule, pszFunction, nLine);
}

HCHAR	*PL_Streaming_Debug_GetSetupStatusStr(PLStreaming_SetupStatus_e	eSetupStatus)
{
	return _plstreaming_Debug_GetSetupStatusStr(eSetupStatus);
}

HCHAR	*PL_Streaming_Debug_GetHomResultStr(HOM_Result_e	eHomResult)
{
	return _plstreaming_Debug_GetHomResultStr(eHomResult);
}

void	PL_Streaming_Debug_PrintPids(HINT32 nPIDsCount, HUINT16 usPIDs[], const HCHAR *pszFunction, const HINT32 nLine)
{
	_plstreaming_Debug_PrintPids(nPIDsCount, usPIDs, pszFunction, nLine);
}

void	PL_Streaming_Debug_PrintMem(HINT32 nPid)
{
	_plstreaming_Debug_PrintMem(nPid);
}

void 	PL_Streaming_Debug_FilePrint(const HCHAR *format, ...)
{
	HCHAR		*pszCmdPrintBuffer = NULL;
	va_list 	va;

	pszCmdPrintBuffer = HLIB_STD_MemCalloc(2048);
	if(NULL == pszCmdPrintBuffer)
	{
		return;
	}

	HxSTD_snprintf(pszCmdPrintBuffer, 2047, "[%10d:%4d] ", (HINT32)HLIB_STD_GetSystemTick(), (HINT32)HLIB_STD_GetTID());

	va_start(va, format);
	vsnprintf((pszCmdPrintBuffer+17), 2047, format, va);
	va_end(va);

	HxLOG_Print(pszCmdPrintBuffer);

	/* file write */
	if(NULL == s_fpDebugFile)
	{
		HCHAR szFileName[256] = {0, };

		#if defined(CONFIG_DEBUG)
		HxSTD_snprintf(szFileName, 256, "homma_streaming_%d.log", HLIB_STD_GetSystemTime());
		#elif defined(CONFIG_DEVICE_DEBUG)
		HxSTD_snprintf(szFileName, 256, "/mnt/hd2/homma_streaming_%d.log", HLIB_STD_GetSystemTime());
		#else
		szFileName[0] = 0;
		#endif

		if(0 != szFileName[0])
		{
			s_fpDebugFile = HLIB_FILE_OpenByIndex(szFileName, "w");
		}
	}

	if(NULL != s_fpDebugFile)
	{
		HLIB_FILE_Write(s_fpDebugFile, pszCmdPrintBuffer, 1, HxSTD_StrLen(pszCmdPrintBuffer));
	}

	HLIB_STD_MemFree(pszCmdPrintBuffer);
}


HOM_Result_e PL_Streaming_Debug_Init(void)
{


	return eHOM_Result_Ok;
}


void 	PL_Streaming_Debug_DeInit(void)
{
	if(NULL != s_fpDebugFile)
	{
		HLIB_FILE_Close(s_fpDebugFile);
		s_fpDebugFile = NULL;
	}
}

/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/



