/**************************************************************************************/
/**
 * @file oplsqcinfo_freesat.c
 *
 * System information
 *
 * @author
 * @date
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "oplsqcinfo_freesat.h"

#include "oapi.h"
#include "hapi.h"
#include <apk.h>
#include <stdlib.h>
#include <string.h>


/******************************************************************
	Define
******************************************************************/
#define	CHECK_BPL_PARAM_PTR(a)		{if (a == NULL) {HxLOG_Error("Input argument is NULL\n"); return BPL_STATUS_ERROR;}}

/******************************************************************
	Static variables
******************************************************************/

/******************************************************************
	Static functions
******************************************************************/

/******************************************************************
	Interfaces
******************************************************************/
OplSqcInfo_Freesat_t *OPL_SqcInfo_Freesat_Create(void)
{
	//	Nothing to do...

	return (OplSqcInfo_Freesat_t*)NULL;
}

void OPL_SqcInfo_Freesat_Destroy(OplSqcInfo_Freesat_t *hsqcInfo)
{
	//	Nothing to do...
}


BPLStatus OPL_SqcInfo_Freesat_GetBouquetName(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, char *pszBouqName,unsigned int ulLen)
{
	CHECK_BPL_PARAM_PTR(pszBouqName);

	memset(pszBouqName,0,ulLen);
	APK_SCENARIO_GetBouquetName(pszBouqName, ulLen);
	pszBouqName[ulLen-1] = '\0';

	return BPL_STATUS_OK;
}

BPLStatus OPL_SqcInfo_Freesat_SetBouquetName(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, const char *pszBouqName)
{
	CHECK_BPL_PARAM_PTR(pszBouqName);

	APK_SCENARIO_SetBouquetName(pszBouqName);

	return BPL_STATUS_OK;
}
BPLStatus OPL_SqcInfo_Freesat_SetPostcode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, const char *pszPostcode)
{
	CHECK_BPL_PARAM_PTR(pszPostcode);

	if(APK_SCENARIO_SetPostCode(pszPostcode) != ERR_OK)
	{
		/* APK_SQCSCENARIO_SetFsatPostcode�� octo�� �ݿ��ϱ� ���� �ڵ���.
		DB���� �ݿ��� �Ǿ��ְ�, OCTO�� �ݿ��� Fail�� ��� Error ó���� ��� �� ������ ����ؾ� ��.
		DB�� ����� ���� �������� �켱 �����Ͽ�, APK_SCENARIO_SetPostcode �� Error�� Return�ϴ���
		Debug Msg���� ����ϰ� Return Error�� ������ �ʴ´�. */
		HxLOG_Error("Setting the freesat postcode is failed..\n");
	}

	return BPL_STATUS_OK;
}

BPLStatus OPL_SqcInfo_Freesat_GetPostcode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, char *pszPostcode, unsigned int ulLen)
{
	memset(pszPostcode,0,ulLen);

	APK_SCENARIO_GetPostCode(pszPostcode, ulLen);
	pszPostcode[ulLen-1] = '\0';

	return BPL_STATUS_OK;
}

BPLStatus OPL_SqcInfo_Freesat_SetSiMode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, int siMode)
{
	APK_SCENARIO_SetSIMode(siMode);

	return BPL_STATUS_OK;
}

BPLStatus OPL_SqcInfo_Freesat_GetSiMode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, int *siMode)
{
	APK_SCENARIO_GetSIMode(siMode);

	return BPL_STATUS_OK;
}

BPLStatus OPL_SqcInfo_Freesat_GetBouquetId(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, int *bouquetId)
{
	APK_SCENARIO_GetBouquetID(bouquetId);

	return BPL_STATUS_OK;
}

BPLStatus OPL_SqcInfo_Freesat_GetRegionCode(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, int *regionCode)
{
	APK_SCENARIO_GetRegionCode(regionCode);

	return BPL_STATUS_OK;
}

BPLStatus OPL_SqcInfo_Freesat_SetAdultChannels(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, BPL_BOOL badultChannels)
{

	APK_SCENARIO_SetAdultChannels(badultChannels);

	return BPL_STATUS_OK;
}

BPLStatus OPL_SqcInfo_Freesat_GetAdultChannels(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, BPL_BOOL *badultChannels)
{
	APK_SCENARIO_GetAdultChannels(badultChannels);

	return BPL_STATUS_OK;
}

BPLStatus OPL_SqcInfo_Freesat_SetDeepLinkForITV(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, const char *url, int lcn)
{
	APK_MHEG_SetDeepLinkForITV(url, lcn);

	return BPL_STATUS_OK;
}

BPLStatus OPL_SqcInfo_Freesat_SetMHEGOperationControl(OplSqcInfo_Freesat_t *hSqcInfo_Freesat, BPL_BOOL bControl)
{
	APK_MHEG_OperationControl(bControl);

	return BPL_STATUS_OK;
}

/* end of file */
