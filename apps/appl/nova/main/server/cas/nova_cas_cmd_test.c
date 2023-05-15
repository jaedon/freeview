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
#include <apk.h>
#include <nova_cas.h>
#include "nova_errcode.h"
#include "nova_utils.h"

/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

/**********************************************************************************************************/
#define ____IMPLEMENTATION_______________________
/**********************************************************************************************************/
typedef struct
{
	HUINT8 test;
} NovaCasCmd_Context_t;

NovaCasCmd_Context_t stContext;

static void NovaDrmLicenseListener(void * aContext, NovaspreadTDrmLicense * aLicense )
{
	int i;
	HxLOG_Info("[%s:%d]\n", __FUNCTION__, __LINE__);
	HxLOG_Info("* \t aContext = 0x%08x\n", aContext);
	HxLOG_Info("* \t LicenseLength = %d\n", aLicense->LicenseLength);

	for(i=0; i<aLicense->LicenseLength; i++)
	{
		if(i%16 == 0)
		{
			printf("\n");
		}

		printf("%02x ", aLicense->License[i]);
	}
	printf("\n");
}

static HINT32 NovaspreadGetFirstTsId(void)
{
	int i = 0;
	NOVA_CAS_t *pCas = NULL;
	NOVA_CAS_TransportSessionInfo_t *pTsIdInfo = NULL;

	HxLOG_Info("[%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_CAS_MAIN_INFO);

	pCas = NOVA_CAS_GetCas();
	if(pCas == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pCas is fail\n",__FUNCTION__, __LINE__, NOVA_CAS_MAIN_ERR);
		return NOVA_INVALID_INDEX;
	}

	for(i=0; i<NOVA_MAX_NUM_OF_TS; i++)
	{
		pTsIdInfo = &pCas->stNotifier.stTs[i].stTsIdInfo;
		if(pTsIdInfo == NULL)
		{
			continue;
		}

		if(pTsIdInfo->ulTransportSessionId != NOVA_INVALID_TSID)
		{
			HxLOG_Info("*\t hAction(0x%08x), ulTransportSessionId(%d), usOnId(0x%08x), usTsId(0x%08x), usSvcId(0x%08x)\n", 
				pTsIdInfo->hAction, pTsIdInfo->ulTransportSessionId, pTsIdInfo->usOnId, pTsIdInfo->usTsId+NOVA_TRANSPORTSESSIONID_SUM, pTsIdInfo->usSvcId);

			return pTsIdInfo->ulTransportSessionId;
		}
	}

	return 0;
}

STATIC int nova_cmd_cas(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[20+1]={0,};

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	
	if( aucArg == NULL )
	{
		HxLOG_Error("[%s:%d] aucArg is fail\n",__FUNCTION__, __LINE__);
		return HxCMD_ERR;
	}
	
	if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
	{
		HxLOG_Error("[%s:%d] aucArg is fail\n",__FUNCTION__, __LINE__);
		return HxCMD_ERR;
	}

	HLIB_STD_StrUpper(aucStr);

	HxLOG_Info("[%s:%d] - Cmd : %s -\n", __FUNCTION__, __LINE__, aucStr);

	if(strcmp(aucStr, "DRMSETOLD") == 0 )
	{
		HUINT32 tsid;
		NovaspreadTDrmLicenseParameters stDrm;
		HUINT8 License[5] = { 1, 2, 3, 4 , 5 };
		HUINT8 UR[6] = { 6, 7, 8, 9, 10, 11 };

		NOVA_memset(&stDrm, 0, sizeof(NovaspreadTDrmLicenseParameters));
		stDrm.OldLicense.License = &License[0];
		stDrm.OldLicense.LicenseLength = 5;
		stDrm.UsageRules = &UR[0];
		stDrm.UsageRulesLength = 6;
		stDrm.CollectionId = 12;
		stDrm.Duration = 13;

		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &tsid) != 0 )
			{
				tsid = NovaspreadGetFirstTsId();
				NovaspreadDrmSetLicenseChangeListener(tsid, NovaDrmLicenseListener, &stContext);
				NovaspreadDrmSetParameters(tsid, &stDrm);
			}
		}
	}
	else if(strcmp(aucStr, "DRMSETNEW") == 0 )
	{
		HUINT32 tsid;
		NovaspreadTDrmLicenseParameters stDrm;

		HUINT8 UR[6] = { 6, 7, 8, 9, 10, 11 };

		NOVA_memset(&stDrm, 0, sizeof(NovaspreadTDrmLicenseParameters));
		stDrm.UsageRules = &UR[0];
		stDrm.UsageRulesLength = 6;
		stDrm.CollectionId = 12;
		stDrm.Duration = 13;

		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &tsid) != 0 )
			{
				tsid = 10;//NovaspreadGetFirstTsId();
				NovaspreadDrmSetLicenseChangeListener(tsid, NovaDrmLicenseListener, &stContext);
				NovaspreadDrmSetParameters(tsid, &stDrm);
			}
		}
	}
	
	else if(strcmp(aucStr, "DRMRM") == 0 )
	{
		HUINT32 tsid;

		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &tsid) != 0 )
			{
				NOVA_CAS_DRM_ReleaseDrmLisntener(tsid);
			}
		}
	}
	else if(strcmp(aucStr, "DRMNOTI") == 0 )
	{

		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "CASVCUR") == 0 ) // ca service usagerules
	{
		int tsid;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &tsid) != 0 )
			{

				return HxCMD_OK;
			}
		}		
	}	
	else if(strcmp(aucStr, "CAPMUR") == 0 ) // ca platform usagerules
	{
		int tsid;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &tsid) != 0 )
			{

				return HxCMD_OK;
			}
		}		
		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "CASINFO") == 0 ) // ca cas info
	{
		NovaspreadCaGetInfo();
		return HxCMD_OK;
	}

	return HxCMD_ERR;
}


void	NOVA_CAS_Cmd(const HCHAR *hCmdHandle)
{
	HLIB_CMD_RegisterWord(hCmdHandle,
						nova_cmd_cas,
						"novacas",				/* keyword */
						"Naovaspread Server : CAS\n novacas [cmd] (start[tsid][cerd_num] | stop | restart[tsid])",	/* help   */
						"novacas [cmd] ( start [tsid][cerd_num] | stop | restart [tsid])");	/* usage  */
}

