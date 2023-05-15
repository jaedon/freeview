/********************************************************************
 * $Workfile:   xdb_cmd.c  $
 * Author     :
 * Description: octo db data debugging tool
 *
 *                                 Copyright (c) 2008 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

/********************************************************************
 Header Files
 ********************************************************************/
#include <octo_common.h>

#include <db_common.h>
#include <db_svc.h>
#include <db_param.h>
#include <db_param.h>

#include "../local_include/_db_main.h"

#include <xdb_svc.h>

//void 			DB_SVC_SqliteExec(char *szDbName, char *szSql) {}
//void 			SH_PrintHashTable(void) {}


/********************************************************************
 Macro Definition
 ********************************************************************/

/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/

/********************************************************************
 External Variable Declaration
 ********************************************************************/

/********************************************************************
 Static Function
 ********************************************************************/
#if defined(CONFIG_DEBUG)
extern HINT32 MGR_SVCLIST_CountSvcInDB (DxSvcType_e eSvcType);
extern DxSvcType_e MGR_SVCLIST_GetSvcTypeForListCount (void);
extern HERROR		MWC_UTIL_Get639CodeByLangIdx(const HxLANG_Id_e eLangIdx, HINT32 *plMatchCount, HUINT8 *pStr1, HUINT8 *pStr2, HUINT8 *pStr3);

#define __________________________________________________________________________

#define __________________________________________________________________________

#define ARG_SIZE    2048
#define MAX_PARAMS  100

STATIC int  argn;
STATIC char *argv[MAX_PARAMS];
STATIC char cmdString[ARG_SIZE];
STATIC char args[MAX_PARAMS][ARG_SIZE];

void setup_args(char **argv)
{
	int i;

	argv[0] = &cmdString[0];
	for (i=1; i<MAX_PARAMS; i++)
	{
		argv[i] = &args[i][0];
	}

	argn = 0;
}

int parseCmdLine(char *cmdLine)
{
	int done = 0;
	int start = 0;
	int prevch;
	int ch;
	int double_quote;
	char *_arg;
	int i = 0;	   /*  index of cmdLine  */
	int j = 0;	   /*  index of each arg */
	int k = 0;	   /*  Select argument	 */
	int m;

	argn = 0;
	cmdString[0] = '\0';

	for (m=1; m<MAX_PARAMS; m++)
	{
		args[m][0]='\0';
	}

	double_quote = 0;
	prevch = 0;


	_arg = &cmdString[0];

	while(!done)
	{
		ch = cmdLine[i];
		if(ch == 0x00) /* end of cmdLine */
		{
			_arg[j] = 0x00;
			if(prevch == '"')
				_arg[j-1] = 0x00;

			done = 1;
			start = 0;
			if( k == 0 && j == 0)
			{
				k = 0;
			}
			else if (j != 0)
			{
				k ++;
			}
		}
		else
		{
			if(ch == 0x20 && start == 0)
			{
				/* if there are extra space characters at the beginning of cmdLine  */
			}
			else if(ch != 0x20 && start == 0)
			{
				start = 1;
				if(ch == '"')
				{
					double_quote= 1;
					prevch = 0;
				}
				else
					_arg[j++] = ch;
			}
			else if(double_quote == 0 && ch == 0x20 && start == 1)
			{
				prevch = ch;
				start = 0;
				_arg[j] = 0x00;
				j = 0;
				k ++;
			}
			else if(double_quote == 1 && prevch == '"' && ch == 0x20 && start == 1)
			{
				prevch = ch;
				double_quote = 0;
				start = 0;
				_arg[j-1] = 0x00;
				j = 0;
				k ++;
			}
			else if(double_quote == 1 && prevch == '"' && ch != 0x20 && start == 1)
			{
				HLIB_CMD_Printf("Warning: errorneous command string\n");
				return -1;
			}
			else
			{
				_arg[j++] = ch;
				prevch = ch;
			}

			if(j == ARG_SIZE - 1)
			{
				HLIB_CMD_Printf("Warning: command argnument is out of range.\n");
				HLIB_CMD_Printf("	Try again with each argument in %d characters\n", ARG_SIZE);
				return -1;
			}
		}
		i++;


		if(start == 0)
		{
			if (k >= MAX_PARAMS)
			{
				done=1;
			}
			else
			{
				if (k==0)
					_arg = &cmdString[0];
				else
					_arg = &args[k][0];
			}
		}
	}  /* While loop */
	argn = k;

	return k;
}

STATIC char xdb_cmd_GetSvcTypeChar(DxSvcType_e eSvcType)
{
	switch(eSvcType)
	{
		case eDxSVC_TYPE_TV:			return 'T';
		case eDxSVC_TYPE_RADIO:			return 'R';
		case eDxSVC_TYPE_DATA:			return 'D';
		case eDxSVC_TYPE_SUBFEED:		return 'S';
		case eDxSVC_TYPE_REPLACEMENT:	return 'P';
		default:						return '?';
	}
}

#if defined(CONFIG_OP_FREESAT)
STATIC void xdb_cmd_PrintSvcInfo(Handle_t hSvc, HINT32 nCondition)
{
	HERROR			err;
	DbSvc_Info_t		svcInfo;
	DbSvc_TsInfo_t	tsInfo;
	DbSvc_PrvInfo_t	prvInfo;
	DbSvc_NetInfo_t	netInfo;
	HINT8			cSvcType;
//	HUINT32			i;

	HxSTD_memset(&svcInfo, 0x00, sizeof(DbSvc_Info_t));
	HxSTD_memset(&tsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

	err = DB_SVC_GetServiceInfo(hSvc, &svcInfo);
	if(err != ERR_OK)
	{
		HLIB_CMD_Printf("\t-----> 0x%08x - err:getsvc\n", hSvc);
		return;
	}
	err = DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&svcInfo), &tsInfo);
	if(err != ERR_OK)
	{
		HLIB_CMD_Printf("\t-----> 0x%08x - err:getts\n", hSvc);
		return;
	}
	err = DB_SVC_GetNetworkInfo(tsInfo.usNetIdx, &netInfo);
	if(err != ERR_OK)
	{
		HLIB_CMD_Printf("\t-----> 0x%08x - err:getnet\n", hSvc);
		//return;
		netInfo.usNetId      = 0;
		netInfo.szNetName[0] = '\0';
	}
	cSvcType = xdb_cmd_GetSvcTypeChar(DbSvc_GetSvcType(&svcInfo));
	switch(nCondition)
	{
		case 0:
			err = DB_SVC_GetPrvInfo(DbSvc_GetPrvIdx(&svcInfo), &prvInfo);
			if(err != ERR_OK)
			{
//				HLIB_CMD_Printf("\t-----> 0x%08x - err:getprv\n", hSvc);
				//return;
				prvInfo.szPrvName[0] = '\0';
			}
			HLIB_CMD_Printf("============================================================================\n") ;
			HLIB_CMD_Printf("= [%c%3d, %s] SName[%s] UNIQ_ID:0x%04X  SVC_ID:0x%04x(%d)\n", cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo), XdbSvc_FreeSatGetSvcShortName(svcInfo), tsInfo.usUniqueId, DbSvc_GetSvcId(&svcInfo), DbSvc_GetSvcId(&svcInfo));

#if 0

//			HLIB_CMD_Printf(" SVC Name Hexa Value & String\n --------\n") ;
			//Util_Print_Dump(DbSvc_GetName(svcInfo),MWC_UTIL_DvbStrlen(DbSvc_GetName(&svcInfo)),0,1);
//			HLIB_CMD_Printf(" --------\n") ;

			for(i=0; i</*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength(); i++)
			{
				if(DbSvc_GetName(&svcInfo)[i] == 0)
					break;
				HLIB_CMD_Printf("%c [0x%02x]\n", DbSvc_GetName(&svcInfo)[i], DbSvc_GetName(&svcInfo)[i]);
			}
#endif
			HLIB_CMD_Printf("= PID V[0x%04X] A[0x%04X] PCR[0x%04X] PMT[0x%04X] SUBTT[%d] TTX[0x%04X] VType[%d]\n", DbSvc_GetVideoPid(&svcInfo), DbSvc_GetAudioPid(&svcInfo), DbSvc_GetPcrPid(&svcInfo), DbSvc_GetPmtPid(&svcInfo), DbSvc_GetSubttlIdx(&svcInfo), DbSvc_GetTtxPid(&svcInfo), DbSvc_GetVideoType(&svcInfo));
			HLIB_CMD_Printf("= PID A_AUX [0x%04X], DOLBY[0x%04X], DOLBY FLAG [%d], STEREO CFG[%d], DUALMONO SEL[%d]\n", DbSvc_GetAudioAuxPid(&svcInfo), DbSvc_GetDolbyPid(&svcInfo), DbSvc_GetDolbyFlag(&svcInfo), DbSvc_GetStereoSelect(&svcInfo), DbSvc_GetDualmonoSelect(&svcInfo));
			HLIB_CMD_Printf("= ON_ID  [0x%04X] TS_ID[0x%04X] TS_SLOT[0x%X] \n", tsInfo.usOnId, DbSvc_GetTsId(&tsInfo), DbSvc_GetTsIdx(&svcInfo));
			HLIB_CMD_Printf("= SVCUid[%04d] FSID[0x%x] Visible[%2d] Customizable = [%2d] Selectable[%2d] RegionId[%04d] \n", DbSvc_GetSvcIdx(&svcInfo), XdbSvc_FreeSatGetSvcIdentifier(svcInfo), DbSvc_SetVisibleFlag(&svcInfo), XdbSvc_GetCustomizableFlag(svcInfo), XdbSvc_GetSelectableFlag(svcInfo), XdbSvc_FreeSatGetSatRegionId(svcInfo));
			HLIB_CMD_Printf("= Handle[0x%04X] NET_ID [0x%04X, %s, 0x%X], PRV[%s, 0x%X]\n", hSvc, netInfo.usNetId, netInfo.szNetName, tsInfo.usNetIdx, prvInfo.szPrvName, DbSvc_GetPrvIdx(&svcInfo), XdbSvc_FreeSatGetSvcIdentifier(svcInfo), XdbSvc_FreeSatGetSvcShortName(svcInfo));
			HLIB_CMD_Printf("= AntIdx[%04d] eOrgDeliType[%04d]\n", DbSvc_GetAntIdx(&svcInfo), DbSvc_GetDeliType(svcInfo));
			HLIB_CMD_Printf("\033[22;36m= Guidance : Type[%d] Mode[%d] Str[%s]\033[0m\n", XdbSvc_FreeSatGetGuidanceType(svcInfo), XdbSvc_FreeSatGetGuidanceMode(svcInfo), XdbSvc_FreeSatGetGuidanceStr(svcInfo));
			HLIB_CMD_Printf("= FTA Content Flag : Net:(0x%08x) Ts:(0x%08x) Svc:(0x%08x)\n", XdbSvc_FreeSatTsGetFTAContentMgr(netInfo), DB_SVC_TsGetFTAContentMgr(tsInfo), XdbSvc_TvAnyTimeGetContentMgr(svcInfo));


			HLIB_CMD_Printf("============================================================================\n") ;
			break;

		case 1:
			HLIB_CMD_Printf("= [%c%3d, %-10s\t: SVC_ID:0x%04X, UNIQ_ID:0x%04x, TS_ID:0x%04x, ON_ID:0x%04x, NET_ID:0x%04x]\n",
					cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
					DbSvc_GetSvcId(&svcInfo), tsInfo.usUniqueId, DbSvc_GetTsId(&tsInfo), tsInfo.usOnId, netInfo.usNetId);
			break;

		case 2:
			HLIB_CMD_Printf("= [%c%3d, %-10s\t: SVC_ID:0x%04X, UNIQ_ID:0x%04x, PMT:0x%04x, V:0x%04x, A:0x%04x, PCR:0x%04x, STT:%d, TTX:0x%04x]\n",
					cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
					DbSvc_GetSvcId(&svcInfo), tsInfo.usUniqueId, DbSvc_GetPmtPid(&svcInfo),
					DbSvc_GetVideoPid(&svcInfo), DbSvc_GetAudioPid(&svcInfo), DbSvc_GetPcrPid(&svcInfo), DbSvc_GetSubttlIdx(&svcInfo), DbSvc_GetTtxPid(&svcInfo));
			break;

		case 3:
			HLIB_CMD_Printf("= [%c%3d, %-20s\t: SVC_ID:0x%04X, UNIQ_ID:0x%04x, TS_IDX:%d, PRV_IDX:%d, NET_IDX:%d]\n",
					cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
					DbSvc_GetSvcId(&svcInfo), tsInfo.usUniqueId,
					DbSvc_GetTsIdx(&svcInfo), DbSvc_GetPrvIdx(&svcInfo), tsInfo.usNetIdx);
			break;

		case 4:
#if 0
			{
				HUINT8 ucFlag;
				//EM_FindPFSection((svcInfo.usSvcId | tsInfo.usUniqueId << 16), &ucFlag);
				HLIB_CMD_Printf("= [%c%3d, %-20s\t: SVC_ID:0x%04X, UNIQ_ID:0x%04x, TS_ID:0x%04x]\n",	/* P(%c) F(%c) */
						cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
						svcInfo.usSvcId, tsInfo.usUniqueId, tsInfo.usTsId);    /* (ucFlag & 0x1) ? 'o':'x', (ucFlag & 0x2) ? 'o':'x') */
			}
			break;
#endif
		case 5:
#if 0
			{
				HUINT8 ucFlag;
				//EM_FindPFSection((svcInfo.usSvcId | tsInfo.usUniqueId << 16), &ucFlag);
				HLIB_CMD_Printf("= [%c%3d, %-20s\t: SVC_ID: %08d, TS_ID: %08d, ON_ID: %08d]\n",	/* P(%c) F(%c) */
						cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
						svcInfo.usSvcId, svcInfo.usTsId, svcInfo.usOnId);    /* (ucFlag & 0x1) ? 'o':'x', (ucFlag & 0x2) ? 'o':'x') */
			}
			break;
#endif


		case 6:
			{
				DbSvc_TsInfo_t	stTsInfo;
				HUINT8			*aucTsDefAuth = NULL, *aucSvcDefAuth = NULL;

				(void)DB_SVC_GetTsDefaultAuthority((const DbSvc_TsInfo_t *)&stTsInfo, &aucTsDefAuth);
				(void)DB_SVC_GetServiceDefaultAuthority((const DbSvc_Info_t *)&svcInfo, &aucSvcDefAuth);
				if (aucTsDefAuth && aucSvcDefAuth)
				{
					aucTsDefAuth[0] = 0;

					DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&svcInfo), &stTsInfo);
					HLIB_CMD_Printf("= [%c%3d, %-20s\t: SVC_ID: %08d, Authority: Sdt (%-25s) Bat2th(%s)]\n",	/* P(%c) F(%c) */
							cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
							DbSvc_GetSvcId(&svcInfo), aucSvcDefAuth, aucTsDefAuth);
				}
				else
				{
					HLIB_CMD_Printf("= [%c%3d, %-20s\t: SVC_ID: %08d, Default Authority: (%s)]\n",	/* P(%c) F(%c) */
							cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
							DbSvc_GetSvcId(&svcInfo), "[--]");
				}
			}
			break;

		case 7:
			/* User Delete 채널만 표시 */
			if (DbSvc_GetSvcRemoved(&svcInfo))
			{
				HLIB_CMD_Printf("= [%c%3d, %-20s\t: SVC_ID:0x%04X, UNIQ_ID:0x%04X, TS_ID:0x%04X]-DELETE\n",	/* P(%c) F(%c) */
						cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
						DbSvc_GetSvcId(&svcInfo), tsInfo.usUniqueId, tsInfo.usTsId);    /* (ucFlag & 0x1) ? 'o':'x', (ucFlag & 0x2) ? 'o':'x') */
			}
			else
			{
				HLIB_CMD_Printf("= [%c%3d, %-20s\t: SVC_ID:0x%04X, UNIQ_ID:0x%04X, TS_ID:0x%04X]\n",	/* P(%c) F(%c) */
						cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
						DbSvc_GetSvcId(&svcInfo), tsInfo.usUniqueId, tsInfo.usTsId);    /* (ucFlag & 0x1) ? 'o':'x', (ucFlag & 0x2) ? 'o':'x') */
			}
			break;

		case 8:
			/* Target region information print */
			{
				HBOOL				bExist=FALSE;
				HUINT32				i;
				DbSvc_T_RegionInfo_t	*pstRegionInfo;

				for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
				{
					pstRegionInfo = &svcInfo.stRegionInfo[i];
					if ( 0xFF != pstRegionInfo->ucRegionDepth )
					{
						bExist = TRUE;
					}
				}

				if ( TRUE == bExist)
				{
					HLIB_CMD_Printf("== Print Target Region Information of SVC\n"); fflush(NULL);
					for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
					{
						pstRegionInfo = &svcInfo.stRegionInfo[i];
						if ( 0xFF != pstRegionInfo->ucRegionDepth )
						{
							HLIB_CMD_Printf("[%04d][%s][0x%X] RegionDepth:0x%X RegionCode:0x%08X CountryCode:%s\n",
								DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo), DbSvc_GetSvcId(&svcInfo), pstRegionInfo->ucRegionDepth, pstRegionInfo->ulRegionCode,
								pstRegionInfo->szCountryCode); fflush(NULL);
						}
					}
				}
			}
			break;

		default:
			break;
	}
}
#else

STATIC HCHAR* xdb_cmd_GetStrOfCasType(DxCasType_e eType)
{
	switch(eType)
	{
	case eDxCAS_TYPE_All: 			return "____0x0000";
	case eDxCAS_TYPE_FTA: 			return "FTA_0x0001";
	case eDxCAS_TYPE_NAGRA:			return "NA__0x0002";
	case eDxCAS_TYPE_IRDETO:		return "IR__0x0004";
	case eDxCAS_TYPE_NDS: 			return "NDS_0x0008";
	case eDxCAS_TYPE_VIACCESS:		return "VIA_0x0010";
	case eDxCAS_TYPE_CONAX:			return "CX__0x0020";
	case eDxCAS_TYPE_MEDIAGUARD:	return "MG__0x0040";
	case eDxCAS_TYPE_CRYPTOWORKS: 	return "CR__0x0080";
	case eDxCAS_TYPE_JPBCAS:		return "JPB_0x0100";
	case eDxCAS_TYPE_JPCCAS:		return "JPC_0x0200";
	case eDxCAS_TYPE_CIPLUS:		return "CI+_0x0400";
	case eDxCAS_TYPE_VERIMATRIX: 	return "VMX_0x0800";
	case eDxCAS_TYPE_ALLCAS:		return "ALL_0x1000";
	default:
		return "??";
	}
}

STATIC void xdb_cmd_PrintTuningInfo(DxDeliveryType_e eDeliType, DbSvc_TuningInfo_t *pstTuningParam)
{
#ifdef CONFIG_DEBUG
	if(NULL == pstTuningParam)
		return;

	switch(eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
		{
			HUINT8			szFecStr[5];
			switch(pstTuningParam->sat.eFecCodeRate)
			{
				case eDxSAT_CODERATE_1_2:		HxSTD_StrNCpy(szFecStr, "1/2",4); 	break;
				case eDxSAT_CODERATE_2_3:		HxSTD_StrNCpy(szFecStr, "2/3",4); 	break;
				case eDxSAT_CODERATE_3_4:		HxSTD_StrNCpy(szFecStr, "3/4",4); 	break;
				case eDxSAT_CODERATE_3_5:		HxSTD_StrNCpy(szFecStr, "3/5",4); 	break;
				case eDxSAT_CODERATE_4_5:		HxSTD_StrNCpy(szFecStr, "4/5",4); 	break;
				case eDxSAT_CODERATE_5_6:		HxSTD_StrNCpy(szFecStr, "5/6",4); 	break;
				case eDxSAT_CODERATE_5_11:		HxSTD_StrNCpy(szFecStr, "5/11",4);	break;
				case eDxSAT_CODERATE_6_7:		HxSTD_StrNCpy(szFecStr, "6/7",4); 	break;
				case eDxSAT_CODERATE_7_8:		HxSTD_StrNCpy(szFecStr, "7/8",4); 	break;
				case eDxSAT_CODERATE_8_9:		HxSTD_StrNCpy(szFecStr, "8/9",4); 	break;
				case eDxSAT_CODERATE_9_10:		HxSTD_StrNCpy(szFecStr, "9/10",4);	break;
				case eDxSAT_CODERATE_AUTO:		HxSTD_StrNCpy(szFecStr, "Auto",4);	break;
				default:						HxSTD_StrNCpy(szFecStr, "???",4); 	break;
			}
			HLIB_CMD_Printf("SAT: [%5dMHz(%c) SR:%5d %4s]\n"
				, pstTuningParam->sat.ulFrequency
				, ((pstTuningParam->sat.ePolarization == eDxSAT_POLAR_HOR) ? 'H': ((pstTuningParam->sat.ePolarization == eDxSAT_POLAR_VER) ? 'V':'?'))
				, pstTuningParam->sat.ulSymbolRate
				, szFecStr);
		}
		break;

	case eDxDELIVERY_TYPE_TER:
		HLIB_CMD_Printf("TER : [%5dMHz] \n", pstTuningParam->ter.ulFreq);
		break;

	case eDxDELIVERY_TYPE_CAB:
		{
			HUINT8			szCons[10];
			switch(pstTuningParam->cab.ucCon)
			{
				case eDxCAB_CONSTELLATION_AUTO:		HxSTD_snprintf(szCons, 10, "AUTO");		break;
				case eDxCAB_CONSTELLATION_16QAM:	HxSTD_snprintf(szCons, 10, "16QAM");	break;
				case eDxCAB_CONSTELLATION_32QAM:	HxSTD_snprintf(szCons, 10, "32QAM");	break;
				case eDxCAB_CONSTELLATION_64QAM:	HxSTD_snprintf(szCons, 10, "64QAM");	break;
				case eDxCAB_CONSTELLATION_128QAM:	HxSTD_snprintf(szCons, 10, "128QAM");	break;
				case eDxCAB_CONSTELLATION_256QAM:	HxSTD_snprintf(szCons, 10, "256QAM");	break;
				case eDxCAB_CONSTELLATION_1024QAM:	HxSTD_snprintf(szCons, 10, "1024QAM");	break;
				case eDxCAB_CONSTELLATION_4096QAM:	HxSTD_snprintf(szCons, 10, "4096QAM");	break;
				default:							HxSTD_snprintf(szCons, 10, "????");		break;
			}
			HLIB_CMD_Printf("CAB : [%d KHz, %d, %s]\n", pstTuningParam->cab.ulFreq, pstTuningParam->cab.usSR, szCons);
		}
		break;

	default:
		break;
	}
#endif
}

STATIC void xdb_cmd_PrintSvcInfo(Handle_t hSvc, HINT32 nCondition)
{
	HERROR			err;
	DbSvc_Info_t		svcInfo;
	DbSvc_TsInfo_t	tsInfo;
	DbSvc_PrvInfo_t	prvInfo;
	DbSvc_NetInfo_t	netInfo;
	HINT8			cSvcType;
//	HUINT8			ulGuidanceMode,ulGuidanceType;
//	HCHAR			GuidanceStr[256];

	HxSTD_memset(&svcInfo, 0x00, sizeof(DbSvc_Info_t));
	HxSTD_memset(&tsInfo, 0x00, sizeof(DbSvc_TsInfo_t));

	err = DB_SVC_GetServiceInfo(hSvc, &svcInfo);
	if(err != ERR_OK)
	{
		HLIB_CMD_Printf("\t-----> 0x%08x - err:getsvc\n", hSvc);
		return;
	}
	err = DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&svcInfo), &tsInfo);
	if(err != ERR_OK)
	{
		HLIB_CMD_Printf("\t-----> 0x%08x - err:getts\n", hSvc);
		return;
	}
	err = DB_SVC_GetNetworkInfo(tsInfo.usNetIdx, &netInfo);
	if(err != ERR_OK)
	{
		HLIB_CMD_Printf("\t-----> 0x%08x - err:getnet\n", hSvc);
		//return;
		netInfo.usNetId      = 0;
		netInfo.szNetName[0] = '\0';
	}
	cSvcType = xdb_cmd_GetSvcTypeChar(DbSvc_GetSvcType(&svcInfo));
	switch(nCondition)
	{
		case 0:
			err = DB_SVC_GetPrvInfo(DbSvc_GetPrvIdx(&svcInfo), &prvInfo);
			if(err != ERR_OK)
			{
				HLIB_CMD_Printf("\t-----> 0x%08x - err:getprv\n", hSvc);
				//return;
				prvInfo.szPrvName[0] = '\0';
			}
			HLIB_CMD_Printf("============================================================================\n") ;
			HLIB_CMD_Printf("= [%c%3d, %s]\tUNIQ_ID:0x%04X    SVC_ID:0x%04x(%d) hService(0x%X), SvcIdx : %d\n", cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo), tsInfo.usUniqueId, DbSvc_GetSvcId(&svcInfo), DbSvc_GetSvcId(&svcInfo), hSvc, DbSvc_GetSvcIdx(&svcInfo));
			HLIB_CMD_Printf("= PID V[0x%04X] A[0x%04X] PCR[0x%04X] PMT[0x%04X] SUBTT[%d] TTX[0x%04X] VType[%d]\n", DbSvc_GetVideoPid(&svcInfo), DbSvc_GetAudioPid(&svcInfo), DbSvc_GetPcrPid(&svcInfo), DbSvc_GetPmtPid(&svcInfo), DbSvc_GetSubttlIdx(&svcInfo), DbSvc_GetTtxPid(&svcInfo), DbSvc_GetVideoType(&svcInfo));
			HLIB_CMD_Printf("= PID A_AUX [0x%04X], DOLBY[0x%04X], DOLBY FLAG [%d], STEREO CFG[%d], DUALMONO SEL[%d]\n", DbSvc_GetAudioAuxPid(&svcInfo), DbSvc_GetDolbyPid(&svcInfo), DbSvc_GetDolbyFlag(&svcInfo), DbSvc_GetStereoSelect(&svcInfo), DbSvc_GetDualmonoSelect(&svcInfo));
			HLIB_CMD_Printf("= ON_ID  [0x%04X] TS_ID[0x%04X] TS_SLOT[0x%X]\n", tsInfo.usOnId, tsInfo.usTsId, DbSvc_GetTsIdx(&svcInfo));
			HLIB_CMD_Printf("= NET_ID [0x%04X, %s, 0x%X], PRV[%s, 0x%X]\n", netInfo.usNetId, netInfo.szNetName, tsInfo.usNetIdx, prvInfo.szPrvName, DbSvc_GetPrvIdx(&svcInfo));
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		{
			DbSvc_UserFlag1_e bwSvcFlag = 0;
			DB_SVC_ServiceGetUserFlag(&bwSvcFlag, &svcInfo);

			HLIB_CMD_Printf("= SVCUid[%04d] Visible[%2d] bwSvcFlag[0x%08x]\n", DbSvc_GetSvcIdx(&svcInfo), DbSvc_GetVisibleFlag(&svcInfo), bwSvcFlag);
			HLIB_CMD_Printf("= Guidance : Type[%d] Mode[%d] Str[%s]\n", XdbSvc_TvAnyTimeGetGuidanceType(&svcInfo), XdbSvc_TvAnyTimeGetGuidanceMode(&svcInfo), XdbSvc_TvAnyTimeGetGuidanceStr(&svcInfo));
			HLIB_CMD_Printf("=FTA Content Flag : Net:(0x%08x) Ts:(0x%08x) Svc:(0x%08x)\n", XdbSvc_TvAnyTimeNetGetFTAContentMgr(&netInfo), XdbSvc_TvAnyTimeTsGetFTAContentMgr(&tsInfo), XdbSvc_TvAnyTimeGetContentMgr(&svcInfo));
		}
#endif
			HLIB_CMD_Printf("============================================================================\n") ;
			break;

		case 1:
			HLIB_CMD_Printf("= [%c%c%3d, %-10s\t: SVC_ID:0x%04X, UNIQ_ID:0x%04x, TS_ID:0x%04x, ON_ID:0x%04x, NET_ID:0x%04x]\n",
					TRUE == DbSvc_GetLcnFlag(&svcInfo) ? '*':' ',
					cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
					DbSvc_GetSvcId(&svcInfo), tsInfo.usUniqueId, tsInfo.usTsId, tsInfo.usOnId, netInfo.usNetId);
			break;

		case 2:
			HLIB_CMD_Printf("= [%c%c%3d, %-10s\t: SVC_ID:0x%04X, UNIQ_ID:0x%04x, PMT:0x%04x, V:0x%04x, A:0x%04x, PCR:0x%04x, STT:%d, TTX:0x%04x]\n",
					TRUE == DbSvc_GetLcnFlag(&svcInfo) ? '*':' ',
					cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
					DbSvc_GetSvcId(&svcInfo), tsInfo.usUniqueId, DbSvc_GetPmtPid(&svcInfo),
					DbSvc_GetVideoPid(&svcInfo), DbSvc_GetAudioPid(&svcInfo), DbSvc_GetPcrPid(&svcInfo), DbSvc_GetSubttlIdx(&svcInfo), DbSvc_GetTtxPid(&svcInfo));
			break;

		case 3:
			HLIB_CMD_Printf("= [%c%c%3d, %-20s\t: SVC_ID:0x%04X, UNIQ_ID:0x%04x, TS_IDX:%d, PRV_IDX:%d, NET_IDX:%d]\n",
						TRUE == DbSvc_GetLcnFlag(&svcInfo) ? '*':' ',
					cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
					DbSvc_GetSvcId(&svcInfo), tsInfo.usUniqueId,
					DbSvc_GetTsIdx(&svcInfo), DbSvc_GetPrvIdx(&svcInfo), tsInfo.usNetIdx);
			break;

		case 4:
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
			{
				DbSvc_UserFlag1_e bwSvcFlag = 0;
				DB_SVC_ServiceGetUserFlag(&bwSvcFlag, &svcInfo);
				HLIB_CMD_Printf("============================================================================\n") ;
				HLIB_CMD_Printf("= [%c%3d, %s]\tUNIQ_ID:0x%04X    SVC_ID:0x%04x(%d) hService(0x%X), SvcIdx : %d\n", cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo), tsInfo.usUniqueId, DbSvc_GetSvcId(&svcInfo), DbSvc_GetSvcId(&svcInfo), hSvc, DbSvc_GetSvcIdx(&svcInfo));

				HLIB_CMD_Printf("= SVCUid[%04d] Visible[%2d] bwSvcFlag[0x%08x]\n", DbSvc_GetSvcIdx(&svcInfo), DbSvc_GetVisibleFlag(&svcInfo), bwSvcFlag);
				HLIB_CMD_Printf("\033[22;36m= Guidance : Type[%d] Mode[%d] Str[%s]\033[0m\n", XdbSvc_TvAnyTimeGetGuidanceType(&svcInfo), XdbSvc_TvAnyTimeGetGuidanceMode(&svcInfo), XdbSvc_TvAnyTimeGetGuidanceStr(&svcInfo));
				HLIB_CMD_Printf("\033[22;36m= FTA Content Flag : Net:(0x%08x) Ts:(0x%08x) Svc:(0x%08x)\033[0m\n", XdbSvc_TvAnyTimeNetGetFTAContentMgr(&netInfo), XdbSvc_TvAnyTimeTsGetFTAContentMgr(&tsInfo), XdbSvc_TvAnyTimeGetContentMgr(&svcInfo));
				HLIB_CMD_Printf("============================================================================\n\n") ;
			}
#endif
#if 0
			{
				HUINT8 ucFlag;
				//EM_FindPFSection((svcInfo.usSvcId | tsInfo.usUniqueId << 16), &ucFlag);
				HLIB_CMD_Printf("= [%c%3d, %-20s\t: SVC_ID:0x%04X, UNIQ_ID:0x%04x, TS_ID:0x%04x]\n",	/* P(%c) F(%c) */
						cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
						svcInfo.usSvcId, tsInfo.usUniqueId, tsInfo.usTsId);    /* (ucFlag & 0x1) ? 'o':'x', (ucFlag & 0x2) ? 'o':'x') */
			}
#endif
			break;
		case 5:
			HLIB_CMD_Printf("= [%c%c%3d, %-10s\t: CAS:%s, SVC_ID:0x%04X, UNIQ_ID:0x%04x, ",
					TRUE == DbSvc_GetLcnFlag(&svcInfo) ? '*':' ',
					cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
					xdb_cmd_GetStrOfCasType(DbSvc_GetCasType(&svcInfo)), DbSvc_GetSvcId(&svcInfo),
					tsInfo.usUniqueId);
			xdb_cmd_PrintTuningInfo(tsInfo.eDeliType, &tsInfo.stTuningParam);
			break;
#if 0
			{
				HUINT8 ucFlag;
				//EM_FindPFSection((svcInfo.usSvcId | tsInfo.usUniqueId << 16), &ucFlag);
				HLIB_CMD_Printf("= [%c%3d, %-20s\t: SVC_ID: %08d, TS_ID: %08d, ON_ID: %08d]\n",	/* P(%c) F(%c) */
						cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
						svcInfo.usSvcId, svcInfo.usTsId, svcInfo.usOnId);    /* (ucFlag & 0x1) ? 'o':'x', (ucFlag & 0x2) ? 'o':'x') */
			}
#endif
			break;


		case 6:
			{
				DbSvc_TsInfo_t	stTsInfo;
				HUINT8			*aucTsDefAuth, *aucSvcDefAuth;

				aucTsDefAuth = aucSvcDefAuth = NULL;

				(void)DB_SVC_GetTsDefaultAuthority((const DbSvc_TsInfo_t *)&stTsInfo, &aucTsDefAuth);
				(void)DB_SVC_GetServiceDefaultAuthority((const DbSvc_Info_t *)&svcInfo, &aucSvcDefAuth);
				if (aucTsDefAuth && aucSvcDefAuth)
				{
					aucTsDefAuth[0] = 0;

					DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&svcInfo), &stTsInfo);
					HLIB_CMD_Printf("= [%c%3d, %-20s\t: SVC_ID: %08d, Authority: Sdt (%-25s) Bat2th(%s)]\n",	/* P(%c) F(%c) */
							cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
							DbSvc_GetSvcId(&svcInfo), aucSvcDefAuth, aucTsDefAuth);
				}
				else
				{
					HLIB_CMD_Printf("= [%c%3d, %-20s\t: SVC_ID: %08d, Default Authority: (%s)]\n",	/* P(%c) F(%c) */
							cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
							DbSvc_GetSvcId(&svcInfo), "[--]");
				}
			}
			break;

		case 7:
			/* User Delete 채널만 표시 */
			if (DbSvc_GetSvcRemoved(&svcInfo))
			{
				HLIB_CMD_Printf("= [%c%3d, %-20s\t: SVC_ID:0x%04X, UNIQ_ID:0x%04X, TS_ID:0x%04X]-DELETE\n",	/* P(%c) F(%c) */
						cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
						DbSvc_GetSvcId(&svcInfo), tsInfo.usUniqueId, tsInfo.usTsId);    /* (ucFlag & 0x1) ? 'o':'x', (ucFlag & 0x2) ? 'o':'x') */
			}
			else
			{
				HLIB_CMD_Printf("= [%c%3d, %-20s\t: SVC_ID:0x%04X, UNIQ_ID:0x%04X, TS_ID:0x%04X]\n",	/* P(%c) F(%c) */
						cSvcType, DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo),
						DbSvc_GetSvcId(&svcInfo), tsInfo.usUniqueId, tsInfo.usTsId);    /* (ucFlag & 0x1) ? 'o':'x', (ucFlag & 0x2) ? 'o':'x') */
			}
			break;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		case 8:
			/* Target region information print */
			{
				HBOOL					 bExist = FALSE;
				DbSvc_T_RegionInfo_t	*pstRegionInfo;
				HUINT32			i;

				for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
				{
					pstRegionInfo = XdbSvc_TvAnyTimeGetRegionInfo(&svcInfo, i);
					if ( 0xFF != pstRegionInfo->ucRegionDepth )
					{
						bExist = TRUE;
					}
				}

				if ( TRUE == bExist)
				{
					HLIB_CMD_Printf("== Print Target Region Information of SVC\n"); fflush(NULL);
					for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
					{
						pstRegionInfo = XdbSvc_TvAnyTimeGetRegionInfo(&svcInfo, i);
						if ( 0xFF != pstRegionInfo->ucRegionDepth )
						{
							HLIB_CMD_Printf("[%04d][%s][0x%X] RegionDepth:0x%X RegionCode:0x%08X CountryCode:%s\n",
								DbSvc_GetLcn(&svcInfo), DbSvc_GetName(&svcInfo), DbSvc_GetSvcId(&svcInfo), pstRegionInfo->ucRegionDepth, pstRegionInfo->ulRegionCode,
								pstRegionInfo->szCountryCode); fflush(NULL);
						}
					}
				}
			}
			break;
#endif

		default:
			break;
	}
}
#endif

STATIC void xdb_cmd_PrintTsInfo(HINT32 nCondition, DbSvc_TsInfo_t *pTsInfo)
{
	HUINT8			szFecStr[5];

	switch(pTsInfo->eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
			switch(pTsInfo->stTuningParam.sat.eFecCodeRate)
			{
				case eDxSAT_CODERATE_1_2:		strncpy(szFecStr, "1/2",4); 	break;
				case eDxSAT_CODERATE_2_3:		strncpy(szFecStr, "2/3",4);	break;
				case eDxSAT_CODERATE_3_4:		strncpy(szFecStr, "3/4",4);	break;
				case eDxSAT_CODERATE_3_5:		strncpy(szFecStr, "3/5",4);	break;
				case eDxSAT_CODERATE_4_5:		strncpy(szFecStr, "4/5",4);	break;
				case eDxSAT_CODERATE_5_6:		strncpy(szFecStr, "5/6",4);	break;
				case eDxSAT_CODERATE_5_11:		strncpy(szFecStr, "5/11",4);	break;
				case eDxSAT_CODERATE_6_7:		strncpy(szFecStr, "6/7",4);	break;
				case eDxSAT_CODERATE_7_8:		strncpy(szFecStr, "7/8",4);	break;
				case eDxSAT_CODERATE_8_9:		strncpy(szFecStr, "8/9",4);	break;
				case eDxSAT_CODERATE_9_10:		strncpy(szFecStr, "9/10",4);	break;
				case eDxSAT_CODERATE_AUTO:		strncpy(szFecStr, "Auto",4);	break;
				default:					strncpy(szFecStr, "???",4);	break;
			}
			HLIB_CMD_Printf("TS:SAT: TsIdx[0x%04X] UNIQID[0x%04X] ANT[%02d] TUID[%d] [%5dMHz(%c) SR:%5d %4s] TSID[0x%04X] ONID[0x%04X] NETIDX[%d]\n",
				pTsInfo->usTsIdx, pTsInfo->usUniqueId, pTsInfo->stTuningParam.sat.ucAntId, pTsInfo->ucTunerId,
				pTsInfo->stTuningParam.sat.ulFrequency, ((pTsInfo->stTuningParam.sat.ePolarization == eDxSAT_POLAR_HOR) ? 'H': ((pTsInfo->stTuningParam.sat.ePolarization == eDxSAT_POLAR_VER) ? 'V':'?')), pTsInfo->stTuningParam.sat.ulSymbolRate, szFecStr,
				pTsInfo->usTsId, pTsInfo->usOnId, pTsInfo->usNetIdx);
			{
				HUINT8	*aucTsDefAuth = NULL;

				(void)DB_SVC_GetTsDefaultAuthority((const DbSvc_TsInfo_t *)pTsInfo, &aucTsDefAuth);
				if (aucTsDefAuth)
				{
					HLIB_CMD_Printf("\tDefAuth: (%s)\n", aucTsDefAuth);
				}
			}
			if(nCondition > 0)
			{
				HLIB_CMD_Printf("             --> eTransSpec        = %d\n", pTsInfo->stTuningParam.sat.eTransSpec);
				HLIB_CMD_Printf("             --> ePskMod           = %d\n", pTsInfo->stTuningParam.sat.ePskMod);
				HLIB_CMD_Printf("             --> ePilot            = %d\n", pTsInfo->stTuningParam.sat.ePilot);
				HLIB_CMD_Printf("             --> eRollOff          = %d\n", pTsInfo->stTuningParam.sat.eRollOff);
			}
			break;
		case eDxDELIVERY_TYPE_TER:
			HLIB_CMD_Printf("= TS : TER : UNIQID[0x%04X] TUID[%d] [%5dMHz] TSID[0x%04X] ONID[0x%04X] NET_SLOT[0x%08x]\n",
				pTsInfo->usUniqueId, pTsInfo->ucTunerId, pTsInfo->stTuningParam.ter.ulFreq, pTsInfo->usTsId, pTsInfo->usOnId, pTsInfo->usNetIdx);
			{
				HUINT8	*aucTsDefAuth = NULL;

				(void)DB_SVC_GetTsDefaultAuthority((const DbSvc_TsInfo_t *)pTsInfo, &aucTsDefAuth);
				if (aucTsDefAuth)
				{
					HLIB_CMD_Printf("\tDefAuth: (%s)\n", aucTsDefAuth);
				}
			}
			/* Target region information print */
			{
				HBOOL				bExist=FALSE;
				HUINT32				i;
				DbSvc_T_RegionInfo_t	*pstRegionInfo;

				for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
				{
					pstRegionInfo = &pTsInfo->utExt.stTvAnyTime.stRegionInfo[i];
					if ( 0xFF != pstRegionInfo->ucRegionDepth )
					{
						bExist = TRUE;
					}
				}

				if ( TRUE == bExist)
				{
					HLIB_CMD_Printf("== Print Target Region Information of TS\n");
					for ( i = 0 ; i < DxMAX_T_REGION_NUM ; i++ )
					{
						pstRegionInfo = &pTsInfo->utExt.stTvAnyTime.stRegionInfo[i];
						if ( 0xFF != pstRegionInfo->ucRegionDepth )
						{
							HLIB_CMD_Printf("\tRegionDepth:0x%X RegionCode:0x%08X CountryCode:%s\n",
								pstRegionInfo->ucRegionDepth, pstRegionInfo->ulRegionCode,
								pstRegionInfo->szCountryCode);
						}
					}
				}
			}
			break;
		case eDxDELIVERY_TYPE_CAB:
			HLIB_CMD_Printf("= TS : CAB : UNIQID[0x%04X] TUID[%d] [%5dMHz SR:%5d, cons:%d, spec:%d] TSID[0x%04X] ONID[0x%04X] NETIDX[%d]\n",
				pTsInfo->usUniqueId, pTsInfo->ucTunerId,
				pTsInfo->stTuningParam.cab.ulFreq, pTsInfo->stTuningParam.cab.usSR, pTsInfo->stTuningParam.cab.ucCon, pTsInfo->stTuningParam.cab.ucSpec,
				pTsInfo->usTsId, pTsInfo->usOnId, pTsInfo->usNetIdx);
			break;
		default:
			HLIB_CMD_Printf("= TS : UNIQID[0x%04X] TUID[%d] TSID[0x%04X] ONID[0x%04X] NETIDX[%d] --> unknown delivery type (0x%x)\n",
				pTsInfo->usUniqueId, pTsInfo->ucTunerId, pTsInfo->usTsId, pTsInfo->usOnId, pTsInfo->usNetIdx, pTsInfo->eDeliType);
			break;
	}

}

int	xdb_cmd_PrintAntenna (int nAntId, DbSvc_AntInfo_t antInfo)
{
	char		*aszStrDiseqcVer[] = {
		"DISEQC_VER_1_0",
		"DISEQC_VER_1_1",
		"DISEQC_VER_1_2",
		"DISEQC_VER_1_3",
		"DISEQC_VER_2_0",
	};

	char		*aszStrDiseqcInput[] = {
		"DISEQC_INPUT_NONE",
		"DISEQC_INPUT_A",
		"DISEQC_INPUT_B",
		"DISEQC_INPUT_C",
		"DISEQC_INPUT_D",
	};

	char		*aszStrToneBurst[] = {
		"TONEBURST_NONE",
		"TONEBURST_A",
		"TONEBURST_B",
	};

	char		*aszStrScdInput[] = {
		"eDxSCD_INPUT_NONE",
		"eDxSCD_INPUT_SCD_A",
		"eDxSCD_INPUT_SCD_B",
	};



	HLIB_CMD_Printf("[]antenna info index(%d) satellite name(%s)\n", nAntId, antInfo.szName);
	switch (antInfo.eAntennaType)
	{
	case eDxANT_TYPE_LNB_ONLY:
		HLIB_CMD_Printf("| Antenna Type	LNB Only\n");
		HLIB_CMD_Printf("| LNB Frequency	%d\n", antInfo.utAntInfo.stLnbAnt.ulLnbFreq);
		HLIB_CMD_Printf("| LNB Power		%s\n", antInfo.utAntInfo.stLnbAnt.eLnbVoltage ? "On":"Off");
		break;
	case eDxANT_TYPE_DISEQC:
		HLIB_CMD_Printf("| Antenna Type	DiSEqC\n");
		HLIB_CMD_Printf("| LNB Frequency	%d\n", antInfo.utAntInfo.stDiseqcAnt.ulLnbFreq);
		HLIB_CMD_Printf("| LNB Power		%s\n", antInfo.utAntInfo.stDiseqcAnt.eLnbVoltage ? "On":"Off");
		HLIB_CMD_Printf("| 22kTone 		%s\n", antInfo.utAntInfo.stDiseqcAnt.b22kTone ? "On":"Off");
		HLIB_CMD_Printf("| DiSEqC Ver		%s\n", aszStrDiseqcVer[antInfo.utAntInfo.stDiseqcAnt.eDiseqcVer]);
		HLIB_CMD_Printf("| DiSEqC Input	%s\n", aszStrDiseqcInput[antInfo.utAntInfo.stDiseqcAnt.eDiseqcInput]);
		HLIB_CMD_Printf("| Tone Burst		%s\n", aszStrToneBurst[antInfo.utAntInfo.stDiseqcAnt.eToneBurst]);
		HLIB_CMD_Printf("| Motor Position	%d\n", antInfo.utAntInfo.stDiseqcAnt.ucMotorPosition);
		HLIB_CMD_Printf("| Longgitude		%d\n", antInfo.utAntInfo.stDiseqcAnt.sSatLonggitude);

		break;
	case eDxANT_TYPE_SCD:
		HLIB_CMD_Printf("| Antenna Type	SCD\n");
		HLIB_CMD_Printf("| LNB Frequency	%d\n", antInfo.utAntInfo.stScdAnt.ulLnbFreq);
		HLIB_CMD_Printf("| LNB Power		%s\n", antInfo.utAntInfo.stScdAnt.eLnbVoltage ? "On":"Off");
		HLIB_CMD_Printf("| 22kTone 		%s\n", antInfo.utAntInfo.stScdAnt.b22kTone ? "On":"Off");
		HLIB_CMD_Printf("| SCD Input		%s\n", aszStrScdInput[antInfo.utAntInfo.stScdAnt.eScdInput]);
		HLIB_CMD_Printf("| User Band		%d\n", antInfo.utAntInfo.stScdAnt.ulScdUserBandNum);
		HLIB_CMD_Printf("| User Frequency	%d\n", antInfo.utAntInfo.stScdAnt.ulScdUserBandFreq);
		break;

	default:
		return HxCMD_ERR;
	}

	HLIB_CMD_Printf("[]------------------------------\n");

	return HxCMD_OK;
}

#define ____________________________________________________________

#define _____common_db_cmd_____

STATIC int xdb_cmd_CloseDb(void *szArgList)
{
	DB_SVC_Close();
	DB_PARAM_Close();

	return HxCMD_OK;
}

STATIC int xdb_cmd_MakeDefault(void *szArgList)
{

	return HxCMD_OK;
}


#define _____svc_db_cmd_____

STATIC int xdb_cmd_RemoveSearchFlag(void *szArgList)
{
	DB_SVC_RemoveSearchFlag();
	return HxCMD_OK;
}

STATIC int xdb_cmd_SyncSvcDb(void *szArgList)
{
	db_main_NotifyUpdate(eSEVT_DB_NOTIFY_UPDATED, HANDLE_NULL, 0, 0, 0);
	return HxCMD_OK;
}

STATIC int xdb_cmd_LoadSvcDb(void *szArgList)
{
	db_main_NotifyUpdate(eSEVT_DB_CHECK_SVC_DB, (Handle_t)0, 0, 0, 0);
	return HxCMD_OK;
}

STATIC int xdb_cmd_DelSvcDb(void *szArgList)
{
	db_main_NotifyUpdate(eSEVT_DB_CHECK_SVC_DB, (Handle_t)0, eCheckDbType_SvcDeleted, 0, 0);
	return HxCMD_OK;
}

STATIC int xdb_cmd_DelNet(void *szArgList)
{
	HINT32				i, nSvcCnt;
	DxDeliveryType_e	eOrgDeliType;

	setup_args(argv);
	parseCmdLine(szArgList);

	eOrgDeliType = eDxDELIVERY_TYPE_ALL;

	if(argn > 0)
	{
		for(i=0; i<4; i++)
		{
			if(0 == strcmp(argv[i], "sat"))
			{
				eOrgDeliType |= eDxDELIVERY_TYPE_SAT;
				continue;
			}
			if(0 == strcmp(argv[i], "ter"))
			{
				eOrgDeliType |= eDxDELIVERY_TYPE_TER;
				continue;
			}
			if(0 == strcmp(argv[i], "cab"))
			{
				eOrgDeliType |= eDxDELIVERY_TYPE_CAB;
				continue;
			}
			break;
		}
	}

	if(eOrgDeliType == eDxDELIVERY_TYPE_ALL)
	{
		eOrgDeliType = 0xff;
	}
	DB_SVC_DeleteAllNetworkService(eOrgDeliType);

	nSvcCnt = MGR_SVCLIST_CountSvcInDB(MGR_SVCLIST_GetSvcTypeForListCount());
	if(nSvcCnt == 0)
	{ /* Normal로 돌고 있었는데, service DB가 없어졌다. No DB 상태로 가야 함. */
		db_main_NotifyUpdate(eSEVT_DB_CHECK_SVC_DB, (Handle_t)0, eCheckDbType_SvcDeleted, 0, 0);
	}

	return HxCMD_OK;
}

STATIC int xdb_cmd_PrintSh(void *szArgList)
{
//	SH_PrintHashTable();
	return HxCMD_OK;
}

STATIC int xdb_cmd_PrintSvc(void *szArgList)
{
	HINT32			res, nLcn;
	DbSvc_Attr_t		stAttr;
	HUINT32			ulSvcCnt;
	Handle_t			*phSvcList;
	DbSvc_Info_t		svcInfo;
	HERROR			err;
	HINT32			i;

	setup_args(argv);
	parseCmdLine(szArgList);



	if(argn > 0)
	{
		if(strncmp(argv[0],"0x",2) == 0)
		{
			res = sscanf(argv[0],"0x%x", &nLcn);
		}
		else
		{
			res = sscanf(argv[0], "%d", &nLcn);
		}
		if(res != 1)
		{

			return HxCMD_ERR;
		}
		DB_SVC_InitSvcAttribute (&stAttr);
		err = DB_SVC_FindServiceList(eSvcGroup_All, &stAttr, 0, HANDLE_NULL, eSvcFindDirection_FowardFromStart, &ulSvcCnt, &phSvcList);
		if(err == ERR_OK)
		{
			for(i=0; i<ulSvcCnt; i++)
			{
				err = DB_SVC_GetServiceInfo(phSvcList[i], &svcInfo);
				if(err != ERR_OK)
				{
					HLIB_CMD_Printf("\t-----> 0x%08x - err:getsvc\n", phSvcList[i]);

					return HxCMD_ERR;
				}
				else
				if(DbSvc_GetLcn(&svcInfo) == nLcn)
				{
					xdb_cmd_PrintSvcInfo(phSvcList[i], 0);
				}
			}
			DB_SVC_FreeServiceList(phSvcList);
			HLIB_CMD_Printf("------------------------------------------\n");
		}
	}



	return HxCMD_OK;
}

STATIC int xdb_cmd_PrintTs(void *szArgList)
{
	HINT32			res, tsIdx;
	HINT8			szSql[128];

	setup_args(argv);
	parseCmdLine(szArgList);

	if(argn > 0)
	{
		if(strncmp(argv[0],"0x",2) == 0)   { res = sscanf(argv[0],"0x%x", &tsIdx); }
		else                               { res = sscanf(argv[0], "%d", &tsIdx); }
		if(res != 1)					   return HxCMD_ERR;

		snprintf(szSql, 127, "select * from TBL_TS where tsIdx=%d;", tsIdx);
//		DB_SVC_SqliteExec(NULL, szSql);

	}
	else
	{
		snprintf(szSql, 127, "select * from TBL_TS LIMIT 1");
//		DB_SVC_SqliteExec(NULL, szSql);
	}

	return HxCMD_OK;
}

STATIC int xdb_cmd_CurSvc(void *szArgList)
{
	Handle_t		hCurSvc;
	HERROR			err;



	/* Last service info 초기화 */
	err = DB_PARAM_GetItem(eDB_PARAM_ITEM_CUR_SVC, (HUINT32 *)&hCurSvc, 0);
	if(err == ERR_OK)
	{
		err = DB_SVC_CheckValidServiceHandle(hCurSvc);
		if(err == ERR_OK)
		{
			xdb_cmd_PrintSvcInfo(hCurSvc, 0);
		}
	}



	return HxCMD_ERR;
}

STATIC int xdb_cmd_ListSvc(void *szArgList)
{
	HINT32			res, nCondition;
	DbSvc_Attr_t		stAttr;
	HUINT32			ulSvcCnt;
	Handle_t		*phSvcList;
	HERROR			err;
	HINT32			i;



	setup_args(argv);
	parseCmdLine(szArgList);

	if(argn > 0)
	{
		if(strncmp(argv[0],"0x",2) == 0)   { res = sscanf(argv[0],"0x%x", &nCondition); }
		else                               { res = sscanf(argv[0], "%d", &nCondition); }

		if(res != 1)
		{

			return HxCMD_ERR;
		}
	}
	else
	{
		nCondition = 0;
	}

	DB_SVC_InitSvcAttribute (&stAttr);
	if (nCondition == 7)
	{
//		stAttr.eUserMask1 &= ~eSvcUserFlag_03_SvcDeletedByUser;
		DbSvc_SetUser1Flag(stAttr.eUserMask1, eSvcUserFlag_03_SvcDeletedByUser, FALSE);
	}
	err = DB_SVC_FindServiceList(eSvcGroup_All, &stAttr, 0, HANDLE_NULL, eSvcFindDirection_FowardFromStart, &ulSvcCnt, &phSvcList);
	if(err == ERR_OK)
	{
		HLIB_CMD_Printf("\n");

		if(nCondition < 9)
		{
			for(i=0; i<ulSvcCnt; i++)
			{
				xdb_cmd_PrintSvcInfo(phSvcList[i], nCondition);
			}
		}
		DB_SVC_FreeServiceList(phSvcList);

		HLIB_CMD_Printf("------------------------------------------\n");
			HLIB_CMD_Printf("== ALL    :%d\n", ulSvcCnt);
			DB_SVC_InitSvcAttribute (&stAttr);
		stAttr.eSvcType = eDxSVC_TYPE_TV;
		err = DB_SVC_GetServiceCount(eSvcGroup_All, &stAttr, &ulSvcCnt);
		if(err == ERR_OK)
			HLIB_CMD_Printf("== TV     :%d\n", ulSvcCnt);
		stAttr.eSvcType = eDxSVC_TYPE_RADIO;
		err = DB_SVC_GetServiceCount(eSvcGroup_All, &stAttr, &ulSvcCnt);
		if(err == ERR_OK)
			HLIB_CMD_Printf("== RADIO  :%d\n", ulSvcCnt);
		stAttr.eSvcType = eDxSVC_TYPE_TV;
		stAttr.eVideoType = eDxVIDEO_TYPE_HD;
		err = DB_SVC_GetServiceCount(eSvcGroup_All, &stAttr, &ulSvcCnt);
		if(err == ERR_OK)
			HLIB_CMD_Printf("== HDTV   :%d\n", ulSvcCnt);

		DB_SVC_InitSvcAttribute (&stAttr);
		for(i=eSvcGroup_Fav_1; i<=eSvcGroup_Fav_5; i++)
		{
			err = DB_SVC_GetServiceCount(i, &stAttr, &ulSvcCnt);
			//if(err == ERR_OK)
				HLIB_CMD_Printf("== FAV%d   :%d\n", i, ulSvcCnt);
		}
		HLIB_CMD_Printf("------------------------------------------\n");


		return HxCMD_OK;
	}
	else
	{
		HxLOG_Error("DB_SVC_FindServiceList ret:0x%X\n",err);
	}


	return HxCMD_ERR;
}

STATIC int xdb_cmd_ListPrv(void *szArgList)
{
	HERROR			err;
	DbSvc_PrvInfo_t	prvInfo;
	HINT32			i, nCnt;


	err = DB_SVC_GetProviderCount(&nCnt);
	if(err == ERR_OK)
	{
		HLIB_CMD_Printf("\n");

		for(i=0; i<nCnt; i++)
		{
			err = DB_SVC_GetPrvInfo(i+1, &prvInfo);
			if(err == ERR_OK)
			{
				HLIB_CMD_Printf("= PROVIDER : %s =\n", prvInfo.szPrvName) ;
			}
		}


		return HxCMD_OK;
	}


	return HxCMD_ERR;
}

STATIC int xdb_cmd_ListTs(void *szArgList)
{
	HERROR			err;
	DbSvc_TsInfo_t	*pstTsInfo = NULL;
	HINT32			i, nCnt;
	HINT32			nCondition, res;
	DbSvc_TsCondition_t	stCond;



	setup_args(argv);
	parseCmdLine(szArgList);

	if(argn > 0)
	{
		if(strncmp(argv[0],"0x",2) == 0)   { res = sscanf(argv[0],"0x%x", &nCondition); }
		else                               { res = sscanf(argv[0], "%d", &nCondition); }
		if(res != 1)
		{

			return HxCMD_ERR;
		}
	}
	else
	{
		nCondition = 0;
	}

	DB_SVC_InitTsFindCondition(&stCond);
	stCond.nDeliType = eDxDELIVERY_TYPE_TER;
	err = DB_SVC_FindTsInfoList(&stCond, &nCnt, &pstTsInfo);
	if(err == ERR_OK)
	{
		HLIB_CMD_Printf("\n");
		for(i=0; i<nCnt; i++)
			xdb_cmd_PrintTsInfo(nCondition, &pstTsInfo[i]);

		DB_SVC_FreeTsInfoList(pstTsInfo);
	}

	stCond.nDeliType = eDxDELIVERY_TYPE_CAB;
	err = DB_SVC_FindTsInfoList(&stCond, &nCnt, &pstTsInfo);
	if(err == ERR_OK)
	{
		HLIB_CMD_Printf("\n");
		for(i=0; i<nCnt; i++)
			xdb_cmd_PrintTsInfo(nCondition, &pstTsInfo[i]);

		DB_SVC_FreeTsInfoList(pstTsInfo);
	}

	stCond.nDeliType = eDxDELIVERY_TYPE_SAT;
	err = DB_SVC_FindTsInfoList(&stCond, &nCnt, &pstTsInfo);
	if(err == ERR_OK)
	{
		HLIB_CMD_Printf("\n");
		for(i=0; i<nCnt; i++)
			xdb_cmd_PrintTsInfo(nCondition, &pstTsInfo[i]);

		DB_SVC_FreeTsInfoList(pstTsInfo);
	}


	return HxCMD_OK;
}

STATIC int xdb_cmd_ListNet(void *szArgList)
{
	HERROR			err = HxCMD_ERR;
	DbSvc_NetInfo_t	netInfo;
	HINT32			i, nCnt;


	HLIB_CMD_Printf("\n");

	err = DB_SVC_GetNetworkCount(&nCnt);
	if (err == ERR_OK)
	{
		for (i = 0; i < nCnt; i++)
		{
			err = DB_SVC_GetNetworkInfo(i+1, &netInfo);
			if (err == ERR_OK && !(netInfo.usNetId == 0 && netInfo.eOrgDeliType == eDxDELIVERY_TYPE_ALL))
			{
				HLIB_CMD_Printf("= NETWORK(%d) : eOrgDeliType = 0x%x, NID = 0x%04x, %s\n", i+1, netInfo.eOrgDeliType, netInfo.usNetId, netInfo.szNetName);

				{
					HUINT8	*aucNetDefAuth = NULL;
					HUINT8	*aucNetUriPrefix = NULL;

					(void)DB_SVC_GetNetworkDefaultAuthority((const DbSvc_NetInfo_t *)&netInfo, &aucNetDefAuth);
					if (aucNetDefAuth)
					{
						if(0 < HxSTD_StrLen(aucNetDefAuth))
						{
							HLIB_CMD_Printf("\tDefAuth: (%s)\n", aucNetDefAuth);
						}
					}

					(void)DB_SVC_GetNetworkUriPrefix((const DbSvc_NetInfo_t *)&netInfo, &aucNetUriPrefix);
					if (aucNetUriPrefix)
					{
						if(0 < HxSTD_StrLen(aucNetUriPrefix))
						{
							HLIB_CMD_Printf("\aucNetUriPrefix: (%s)\n", aucNetUriPrefix);

						}
					}
				}
#ifdef CONFIG_OP_UK_DTT
				/* Target region information print */
				{
					DbSvc_T_RegionInfo_t	*pstRegionInfo;
					HINT32					nIndex;

					HLIB_CMD_Printf("== Print Target Region Information of NET\n");

					for ( nIndex = 0 ; nIndex < DxMAX_T_REGION_NUM ; nIndex++ )
					{
						pstRegionInfo = &netInfo.utExt.stTvAnyTime.stRegionInfo[nIndex];
						if ( 0xFF != pstRegionInfo->ucRegionDepth )
						{	/* bExist = TRUE */
							HLIB_CMD_Printf("\tRegionDepth:0x%X RegionCode:0x%08X CountryCode:%s\n",
								pstRegionInfo->ucRegionDepth, pstRegionInfo->ulRegionCode, pstRegionInfo->szCountryCode);
							break;
						}
					}
				}
#endif

			}
		}

		err = HxCMD_OK;
	}

//FUNC_OUT:

	return err;
}


#define _____setup_db_cmd_____

STATIC int xdb_cmd_SyncSetupDb(void *szArgList)
{
	DB_PARAM_Sync();
	DAPI_Sync(DxDBSETUP_SYSTEM);
	return HxCMD_OK;
}

STATIC int xdb_cmd_ListAnt(void *szArgList)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HINT32		i, cnt, *pnAntIdxList=NULL;
	HERROR		err;
	DbSvc_AntInfo_t	antInfo;



	err = DB_SVC_FindAllAntIdxList(&cnt, &pnAntIdxList);
	if((err == ERR_OK) && (cnt > 0))
	{
		HLIB_CMD_Printf("Total Antenna Count : %d\n",cnt);
		for(i=0; i<cnt; i++)
		{
			err = DB_SVC_GetAntInfo((HUINT16)pnAntIdxList[i], &antInfo);
			if(err != ERR_OK)
				continue;

			xdb_cmd_PrintAntenna (pnAntIdxList[i], antInfo);
		}
		HLIB_CMD_Printf("\n\n");

		DB_SVC_FreeAntList(pnAntIdxList);

		err = HxCMD_OK;
		goto FUNC_OUT;
	}
	else
	{
		DB_SVC_FreeAntList(pnAntIdxList);

		HLIB_CMD_Printf("\tno antenna info\n\n");
		err = HxCMD_ERR;
	}
FUNC_OUT:

	return err;
#else
	return HxCMD_OK;
#endif
}

STATIC int xdb_cmd_DelAnt(void *szArgList)
{
#if defined(CONFIG_MW_CH_SATELLITE)
#if defined(CONFIG_DB_ANT_RAMFS)

	HUINT16 usAntIdx=0;
	HINT32	idx,nCnt, *pnAntIdxList;

	DB_SVC_DeleteAllAntInfo();
	HLIB_CMD_Printf("\tcall [DB_SVC_DeleteAllAntInfo()]\n\n");

	DB_SVC_AddAntInfo(NULL, &usAntIdx);

	HLIB_CMD_Printf("\t added index:%d\n",usAntIdx);

	DB_ANT_Print();

	DB_SVC_AddAntInfo(NULL, &usAntIdx);
	DB_SVC_AddAntInfo(NULL, &usAntIdx);

	HLIB_CMD_Printf("\t added index:%d\n",usAntIdx);

	DB_ANT_Print();

	DB_SVC_DeleteAntInfo(usAntIdx-1);

	DB_ANT_Print();


	DB_SVC_FindAllAntIdxList(&nCnt, &pnAntIdxList);

	HLIB_CMD_Printf("\t Valid Idx Cnt:%d\n",nCnt);
	HLIB_CMD_Printf("\t Valid Idx List : ");

	for (idx=0; idx < nCnt; idx++)
	{
		HLIB_CMD_Printf("[%d] ",pnAntIdxList[idx]);
	}
	HLIB_CMD_Printf("\n");

	DB_SVC_FreeAntList(pnAntIdxList);
#endif
	return HxCMD_ERR;
#else
	return HxCMD_OK;
#endif
}

#if 0
extern HERROR MWC_UTIL_GetIso3166CodeByCountryId(HxCountry_e eCountryId, HUINT8 *szIso3166Code);
extern HERROR MWC_UTIL_GetCountryIdByIso3166Code(HUINT8 *szIso3166Code, HxCountry_e *peCountryId);

STATIC int xdb_cmd_SetCountry(void *szArgList)
{
	HERROR			err;
	HINT32			nVal = 0;
	HUINT8			aucIso3166[4];
	HxCountry_e		eCountryId;

	setup_args(argv);
	parseCmdLine(szArgList);

//	eCountryId = MWC_PARAM_GetCountryId();
	err = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
	if (ERR_OK != err)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
	}

	if(HLIB_COUNTRY_IsValidCountryId(eCountryId))
	{
		err = MWC_UTIL_GetIso3166CodeByCountryId(eCountryId, aucIso3166);
		if(err == ERR_OK)
		{
			aucIso3166[3] = '\0';
			HLIB_CMD_Printf("CUR# DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) : value = %d (iso3166code:%s)\n", nVal, aucIso3166);
		}
	}

	if(argn > 0)
	{
		if(strcmp(argv[0],"uk") == 0)			eCountryId = eCountryID_GBR;
		else if(strcmp(argv[0],"nordig") == 0)	eCountryId = eCountryID_FIN;
		else if(strcmp(argv[0],"italy") == 0)	eCountryId = eCountryID_ITA;
		else if(strcmp(argv[0],"eu") == 0)		eCountryId = eCountryID_DEU;
		else
		{
			HxSTD_memcpy(aucIso3166, argv[0], 3);
			aucIso3166[3] = '\0';
			err = MWC_UTIL_GetCountryIdByIso3166Code(aucIso3166, &eCountryId);
			if(err != ERR_OK)
				return HxCMD_ERR;
		}

		//MWC_PARAM_SetCountryId(eCountryId);
		err = DB_PARAM_SetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32)eCountryId, 0);
		if (ERR_OK != err)
		{
			HxLOG_Warning("DB_PARAM_SetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!!\n");
		}

		err = MWC_UTIL_GetIso3166CodeByCountryId(eCountryId, aucIso3166);
		if(err == ERR_OK)
		{
			aucIso3166[3] = '\0';
			HLIB_CMD_Printf("NEW# DB_PARAM_SetItem(eDB_PARAM_ITEM_COUNTRY_ID) : value = %d (iso3166code:%s)\n", nVal, aucIso3166);
		}
		return HxCMD_OK;
	}

	return HxCMD_ERR;
}
#endif

#if ___disabled_db_cmd___


/* "import db_data_filename" */
STATIC int xdb_cmd_ImportData(void *szArgList)
{
	setup_args(argv);
	parseCmdLine(szArgList);

	if(argn == 1)
	{
		DB_SVC_ImportData(NULL, argv[0]);
	}
	else if(argn == 2)
	{
		DB_SVC_ImportData(argv[0], argv[1]);
	}
	else
	{
		return HxCMD_ERR;
	}

	return HxCMD_OK;
}

#endif

STATIC int xdb_cmd_printsetupdb(void *szArgList)
{
	HERROR			err;
	HUINT32			ulValue;
	HUINT8			aucIso3166[4];
	HINT32			ulMatchCount;
	HUINT8			szStr1[10],szStr2[10],szStr3[10];
	HxCountry_e		eCountryId;

//	eCountryId = MWC_PARAM_GetCountryId();
	err = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
	if (ERR_OK != err)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
	}

	if(HLIB_COUNTRY_IsValidCountryId(eCountryId))
	{
#if 1
		{
			const HCHAR *pszIsoCountryCode = NULL;
			pszIsoCountryCode = HLIB_COUNTRY_IdTo3166_Alpha3(eCountryId);
			if (pszIsoCountryCode)
			{
				HxSTD_MemCopy(aucIso3166, pszIsoCountryCode, 4);
				err = ERR_OK;
			}
			else
			{
				err = ERR_FAIL;
			}
		}
#else
		err = MWC_UTIL_GetIso3166CodeByCountryId(eCountryId, aucIso3166);
#endif
		if(err == ERR_OK)
		{
			aucIso3166[3] = '\0';
			HLIB_CMD_Printf("CUR# DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) : value = %d (iso3166code:%s)\n", eCountryId, aucIso3166);
		}
	}

//	ulValue = MWC_PARAM_GetMainLanguage();
	err = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&ulValue, 0);
	if (ERR_OK != err)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN) Error!!\n");
	}

	HxSTD_memset(szStr1,0,10);		HxSTD_memset(szStr2,0,10);		HxSTD_memset(szStr3,0,10);
	err = MWC_UTIL_Get639CodeByLangIdx((const HxLANG_Id_e)ulValue,&ulMatchCount,szStr1,szStr2,szStr3);
	if(err == ERR_OK)
	{
		szStr1[3] = '\0';
		szStr2[3] = '\0';
		szStr3[3] = '\0';
		HLIB_CMD_Printf("CUR# menu_config_item : value = %d (iso3166code MatchNum:%d :[%s][%s][%s])\n",
			ulValue,ulMatchCount, szStr1,szStr2,szStr3);
	}

	return HxCMD_ERR;
}
#endif

void pdb_cmd_Register (void)
{
#define	hCmdHandle		"db"

#if defined(CONFIG_DEBUG)
	///////////////////////// COMMON DB CMD /////////////////
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_CloseDb,
			"closedb",	/* keyword */
			"closedb",	/*   help  */
			"closedb");	/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_MakeDefault,
			"makedefault",		/* keyword */
			"makedefault",		/*   help  */
			"makedefault");		/*  usage  */

	///////////////////////// SVC DB CMD /////////////////
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_RemoveSearchFlag,
			"rmvsearchflag",	/* keyword */
			"rmvsearchflag",	/*   help  */
			"rmvsearchflag");	/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_SyncSvcDb,
			"syncsvcdb",	/* keyword */
			"syncsvcdb",	/*   help  */
			"syncsvcdb");	/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_LoadSvcDb,
			"loadsvcdb",	/* keyword */
			"loadsvcdb",	/*   help  */
			"loadsvcdb");	/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_DelSvcDb,
			"delsvcdb",										/* keyword */
			"delete service db file and create new file",	/*   help  */
			"delsvcdb");									/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_DelNet,
			"delnet",															/* keyword */
			"delete all services or speficic delivery's and sync service db",	/*   help  */
			"delnet or delnet sat|cab|ter|ana");								/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_PrintSh,
			"printsh",	/* keyword */
			"print service hash table",	/*   help  */
			"printsh");	/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_PrintSvc,
			"printsvc",				/* keyword */
			"printsvc [lcn]",		/*   help  */
			"printsvc [lcn]");		/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_PrintTs,
			"printts",	/* keyword */
			"printts [unique_id]",	/*   help  */
			"printts [unique_id]");	/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_CurSvc,
			"cursvc",	/* keyword */
			"cursvc",	/*   help  */
			"cursvc");	/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_ListSvc,
			"listsvc",					/* keyword */
			"listsvc [condition:0~6]",	/*   help  */
			"listsvc [condition:0~6]");	/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_ListPrv,
			"listprv",	/* keyword */
			"listprv",	/*   help  */
			"listprv");	/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_ListTs,
			"listts",	/* keyword */
			"listts",	/*   help  */
			"listts");	/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_ListNet,
			"listnet",	/* keyword */
			"listnet",	/*   help  */
			"listnet");	/*  usage  */

	///////////////////////// SETUP DB CMD /////////////////
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_SyncSetupDb,
			"syncsetupdb",	/* keyword */
			"syncsetupdb",	/*   help  */
			"syncsetupdb");	/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_ListAnt,
			"listant",	/* keyword */
			"listant",	/*   help  */
			"listant");	/*  usage  */
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_DelAnt,
			"delant",	/* keyword */
			"delant",	/*   help  */
			"delant");	/*  usage  */
#if 0
	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_SetCountry,
			"setcountry",	/* keyword */
			"set country index to setup db (uk or nordig or italy or eu or iso3166code:nld, ... ",	/*   help  */
			"setcountry [ 'uk' | 'nordig' | 'italy' | 'eu' | iso3166 code:nld,ita,... ]");			/*  usage  */
#endif

	HLIB_CMD_RegisterWord(hCmdHandle,
			xdb_cmd_printsetupdb,
			"printsetupdb",	/* keyword */
			"print current system's setup database",	/*   help  */
			"printsetupdb");			/*  usage  */
#endif
}

/* end of file */

