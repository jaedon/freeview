/**
	@file     db_svc_svcprech.c
	@brief    service managing module - pre-programmed (or preferred) channels

	Description: service api를 제공한다	\n
	Module: DB-SVC \n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <util_task.h>

#include <isosvc.h>

#include <db_common.h>
#include <db_svc.h>
#include <db_param.h>

#include <_db_svc_table.h>
#include <_db_svc.h>
#include <_db_svc_transponder.h>

#include <otl.h>
#include "_db_svc_common.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define MAX_MAPPING_TABLE_NUM					4
#ifdef CONFIG_MW_CH_SATELLITE
#define SVC_SAT_MAPPING_COLUMN_NUM				(sizeof(s_astSvcSatColumnMappingTable)/sizeof(dbSvc_CsvColumnMapping_t))
#define TS_SAT_MAPPING_COLUMN_NUM				(sizeof(s_astTsSatColumnMappingTable)/sizeof(dbSvc_CsvColumnMapping_t))
#endif
#ifdef CONFIG_MW_CH_CABLE
#define SVC_CAB_MAPPING_COLUMN_NUM				(sizeof(s_astSvcCabColumnMappingTable)/sizeof(dbSvc_CsvColumnMapping_t))
#define TS_CAB_MAPPING_COLUMN_NUM				(sizeof(s_astTsCabColumnMappingTable)/sizeof(dbSvc_CsvColumnMapping_t))
#endif
#define PRV_MAPPING_COLUMN_NUM					(sizeof(s_astPrvColumnMappingTable)/sizeof(dbSvc_CsvColumnMapping_t))
#define NET_MAPPING_COLUMN_NUM					(sizeof(s_astNetColumnMappingTable)/sizeof(dbSvc_CsvColumnMapping_t))
#define DB_SVC_PRE_DEFINED_CHLIST_FORMAT_STR	"pre_ch_%s.%s"

#define SVCPRECH_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4, arg5, arg6)	\
			{																\
				(void)arg1;													\
				(void)arg2;													\
				(void)arg3;													\
				(void)arg4;				 									\
				(void)arg5;				 									\
				(void)arg6;				 									\
			}


#define DEF_ANT_INDEX		(1)
#define CSV_FILE_PATH		"/etc/"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eTableKind_None		= 0x00,
	eTableKind_SVC		= 0x01,
	eTableKind_TS		= 0x02,
	eTableKind_PRV		= 0x04,
	eTableKind_NET		= 0x08
}  dbSvc_UsingTableKind_e, dbSvc_UsingTableKind_b;

typedef void (*dbSvc_SetFieldCb_t)(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue);

typedef struct tagCsvField
{
	dbSvc_UsingTableKind_b 	 bwUsingTableKind;	/* 각 필드는 table 별로 중복사용이 가능하다 */
	const HINT8 			*apszDbField[MAX_MAPPING_TABLE_NUM];
	HINT8 					*szCsvField;
	dbSvc_SetFieldCb_t		 afnSvcDbSetField[MAX_MAPPING_TABLE_NUM];
	struct tagCsvField		*pstNext;
} dbSvc_CsvFieldList_t;

typedef enum
{
	eCol_Mandatory,
	eCol_Optional
} dbSvc_CsvColAttr_t;

typedef struct
{
	dbSvc_CsvColAttr_t		 eColAttr;
	HINT8 					*szDbField;
	HINT8 					*szCsvField;
	dbSvc_SetFieldCb_t		 fnSvcDbSetField;
} dbSvc_CsvColumnMapping_t;

typedef struct
{
	DbSvc_Info_t		stSvcInfo;
	DbSvc_TsInfo_t		stTsInfo;
	DbSvc_PrvInfo_t		stPrvInfo;
	DbSvc_NetInfo_t		stNetInfo;
} dbSvc_PreChInfo_t;

enum
{ /* [WARN] forbid to change a query index */
	eSvcTable		= 0,
	eTsTable		= 1,
	ePrvTable		= 2,
	eNetTable		= 3,
	eFavTable		= 4,
	eBouqTable		= 5,
	eGenreTable		= 6,
	/*----------------------------tables to be loaded---*/
	eRecentTable	= 7,
	/*----------------------------tables to be created---*/
	eSqlMax_LoadTable	= 7,
	eSqlMax_CreateTable	= 8
} ;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define ____PRE_PROGRAMMED_CHLIST______________________________________

/* skip white space and '"' - skip spaces and tabs to alphabet field start */

STATIC HINT32 db_svc_PrechGetProperFieldStr(HINT8 *szStr)
{
	HINT8	*pucProperStr = szStr;
	HINT32	nCurPos = 0;

	while((szStr[0] == 0x09) || (szStr[0] == 0x0A) || (szStr[0] == 0x20) || (szStr[0] == 0x22))
	{ /* 0x09:tab, 0x20:space, 0x22:'"' */
		szStr++;
	}
	while(szStr[0] != '\0')
	{
		if(szStr[0] != 0x22)
		{
			pucProperStr[nCurPos] = szStr[0];
			nCurPos++;
		}
		szStr++;
	}
	pucProperStr[nCurPos] = '\0';

	return nCurPos; /* it is new string length */
}


STATIC HINT32 db_svc_PrechCountToNextComma(HINT8 *szStr)
{
	HINT32	nLen = 0;
	while(*szStr != '\0')
	{
		if(*szStr == ',')
			return nLen;

		nLen++;
		szStr++;
	}

	return (nLen == 0) ? 0 : (nLen-1);
}

/*
** If zNum represents an integer that will fit in 32-bits, then set
** *pValue to that integer and return true.  Otherwise return false.
**
** Any non-numeric characters that following zNum are ignored.
** This is different from sqlite3Atoi64() which requires the
** input number to be zero-terminated.
** 	- from sqlite3's sqlite3GetInt32()
*/

STATIC int db_svc_PrechGetInt32(const char *zNum, int *pValue)
{
  long long int v = 0;
  int i, c;
  int neg = 0;

  if( zNum[0]=='-' ){
    neg = 1;
    zNum++;
  }else if( zNum[0]=='+' ){
    zNum++;
  }
  while( zNum[0]=='0' ) zNum++;
  for(i=0; (i<11) && (zNum[i] != '\0'); i++){
  	if((c = zNum[i] - '0')>=0 && c<=9)
	    v = v*10 + c;
	else
		return ERR_FAIL; /* not integer */
  }

  /* The longest decimal representation of a 32 bit integer is 10 digits:
  **
  **             1234567890
  **     2^31 -> 2147483648
  */
  if( i>10 ){
    return ERR_FAIL;
  }
  if( v-neg>2147483647 ){
    return ERR_FAIL;
  }
  if( neg ){
    v = -v;
  }
  *pValue = (int)v;
  return ERR_OK;
}

STATIC HINT32 db_svc_PrechGetUsingTableKind(HINT32 nTableIndex)
{
	switch(nTableIndex)
	{
		case eSvcTable:			return eTableKind_SVC;
		case eTsTable:			return eTableKind_TS;
		case ePrvTable:			return eTableKind_PRV;
		case eNetTable:			return eTableKind_NET;
		default:				return -1; /* error */
	}
}


STATIC HINT32 db_svc_PrechGetMappingTableIndex(dbSvc_UsingTableKind_b bwUsingTableKind)
{
	switch(bwUsingTableKind)
	{
		case eTableKind_SVC:	return eSvcTable;
		case eTableKind_TS:		return eTsTable;
		case eTableKind_PRV:	return ePrvTable;
		case eTableKind_NET:	return eNetTable;
		default:				return -1; /* error */
	}
}


STATIC HERROR db_svc_PrechCheckMandatoryField(dbSvc_UsingTableKind_b bwUsingTableKind, HINT32 nMappingColumnNum, const dbSvc_CsvColumnMapping_t *pstColumnMappingTable, dbSvc_CsvFieldList_t *pstFieldListHead, HINT32 *pnTotalFieldLen)
{
	HBOOL					 bFound;
	HINT32					 i, nTotalFieldLen, nTableIndex;
	dbSvc_CsvFieldList_t	*pstTempField;

	nTotalFieldLen = 0;

	for(i=0; i<nMappingColumnNum; i++)
	{
		bFound = FALSE;
		pstTempField = pstFieldListHead;
		while(pstTempField)
		{
			if(0 == HxSTD_StrCmp(pstColumnMappingTable[i].szCsvField, pstTempField->szCsvField))
			{
				pstTempField->bwUsingTableKind 				|= bwUsingTableKind;
				nTableIndex = db_svc_PrechGetMappingTableIndex(bwUsingTableKind);
				if (-1 == nTableIndex)
				{
					pstTempField = pstTempField->pstNext;
					continue;
				}
				pstTempField->apszDbField[nTableIndex] 			= pstColumnMappingTable[i].szDbField;
				pstTempField->afnSvcDbSetField[nTableIndex] 	= pstColumnMappingTable[i].fnSvcDbSetField;

				nTotalFieldLen += (HxSTD_StrLen(pstColumnMappingTable[i].szDbField)+1);
				nTotalFieldLen += (HxSTD_StrLen(pstColumnMappingTable[i].szCsvField)+1);
				bFound = TRUE;
				HxLOG_Print("(%d, bwUsingTableKind:0x%x) db/csv = %s/%s\n", i, pstTempField->bwUsingTableKind, pstColumnMappingTable[i].szDbField, pstColumnMappingTable[i].szCsvField);
				break;
			}

			pstTempField = pstTempField->pstNext;
		}

		if(pstColumnMappingTable[i].eColAttr == eCol_Mandatory)
		{ /* mandatory의 경우, field를 찾지 못하면 load fail임. */
			if(bFound == FALSE)
			{ /* not found */
				HxLOG_Error("insufficient mandatory field(%d:%s/%s)...\n", i, pstColumnMappingTable[i].szDbField, pstColumnMappingTable[i].szCsvField);
				return ERR_FAIL;
			}
		}
	}

	*pnTotalFieldLen += nTotalFieldLen;

	return ERR_OK;
}


STATIC void db_svc_PrechFreeCsvFieldList(dbSvc_CsvFieldList_t *pstFieldListHead)
{
	dbSvc_CsvFieldList_t	*pstFieldList, *pstTempField;

	pstFieldList = pstFieldListHead;
	while(pstFieldList)
	{
		if(pstFieldList->szCsvField)
			OxDB_Free(pstFieldList->szCsvField);

		pstTempField = pstFieldList->pstNext;
		OxDB_Free(pstFieldList);
		pstFieldList = pstTempField;
	}
}

STATIC HERROR db_svc_PrechMakeCsvFieldList(HUINT8 *szLine, HINT32 nLineLen, dbSvc_CsvFieldList_t **ppstFieldListHead)
{
	HINT32					i, nFieldNum, nPrevFieldOffset, nFieldNameLen;
	dbSvc_CsvFieldList_t	*pstFieldListHead = NULL, *pstPrevField = NULL, *pstTempField;

	if((nLineLen > 3) && ((HUINT8)szLine[0] == 0xEF) && ((HUINT8)szLine[1] == 0xBB) && ((HUINT8)szLine[2] == 0xBF))
	{ /* UTF-8 문서의 경우, 3글자 제외하고 필드 값 읽어들임 */
		szLine = &szLine[3];
		nLineLen -= 3;
	}

	/* 첫줄은 따옴표가 없다고 가정 */
	for(i=0, nFieldNum=0, nPrevFieldOffset=0, nFieldNameLen=0; i<nLineLen; i++)
	{
		if((szLine[i] == '\n') || (szLine[i] == '"'))
		{
			db_svc_PrechFreeCsvFieldList(pstFieldListHead);
			return ERR_FAIL; /* abnormal case */
		}

		if((szLine[i] == 0x09) || (szLine[i] == 0x0A) || (szLine[i] == 0x20))
			continue; /* ignore space, tab and those kinds of characters. */

		nFieldNameLen++;
		if(szLine[i] == ',')
		{
			if(pstFieldListHead == NULL)
			{
				pstFieldListHead = (dbSvc_CsvFieldList_t *)OxDB_Malloc(sizeof(dbSvc_CsvFieldList_t));
				HxSTD_MemSet(pstFieldListHead, 0, sizeof(dbSvc_CsvFieldList_t));

				nFieldNameLen = i-nPrevFieldOffset;
				nPrevFieldOffset = i+1;
				pstFieldListHead->szCsvField = (HINT8 *)OxDB_Malloc(nFieldNameLen+1);
				HxSTD_MemCopy(pstFieldListHead->szCsvField, &szLine[i-nFieldNameLen], nFieldNameLen);
				pstFieldListHead->szCsvField[nFieldNameLen] = '\0';
				HLIB_STD_StrLower(pstFieldListHead->szCsvField);

				pstPrevField = pstFieldListHead;
			}
			else
			{
				pstTempField = (dbSvc_CsvFieldList_t *)OxDB_Malloc(sizeof(dbSvc_CsvFieldList_t));
				HxSTD_MemSet(pstTempField, 0, sizeof(dbSvc_CsvFieldList_t));

				nFieldNameLen = i-nPrevFieldOffset;
				nPrevFieldOffset = i+1;
				pstTempField->szCsvField = (HINT8 *)OxDB_Malloc(nFieldNameLen+1);
				HxSTD_MemCopy(pstTempField->szCsvField, &szLine[i-nFieldNameLen], nFieldNameLen);
				pstTempField->szCsvField[nFieldNameLen] = '\0';
				HLIB_STD_StrLower(pstTempField->szCsvField);

				pstPrevField->pstNext = pstTempField;
				pstPrevField = pstPrevField->pstNext;
			}
			nFieldNum++;
			nFieldNameLen = 0;
		}
	}

	/* 맨 마지막 필드도 붙이기 */
	if(i == nLineLen)
	{
		nFieldNum++;
		if(pstFieldListHead == NULL)
		{
			pstFieldListHead = (dbSvc_CsvFieldList_t *)OxDB_Malloc(sizeof(dbSvc_CsvFieldList_t));
			HxSTD_MemSet(pstFieldListHead, 0, sizeof(dbSvc_CsvFieldList_t));

			nFieldNameLen = i-nPrevFieldOffset;
			nPrevFieldOffset = i;
			pstFieldListHead->szCsvField = (HINT8 *)OxDB_Malloc(nFieldNameLen+1);
			HxSTD_MemCopy(pstFieldListHead->szCsvField, &szLine[i-nFieldNameLen], nFieldNameLen);
			pstFieldListHead->szCsvField[nFieldNameLen] = '\0';
			HLIB_STD_StrLower(pstFieldListHead->szCsvField);

			pstPrevField = pstFieldListHead;
		}
		else
		{
			pstTempField = (dbSvc_CsvFieldList_t *)OxDB_Malloc(sizeof(dbSvc_CsvFieldList_t));
			HxSTD_MemSet(pstTempField, 0, sizeof(dbSvc_CsvFieldList_t));

			nFieldNameLen = i-nPrevFieldOffset-1; /* i가 끝까지 갔으니까 1빼줌 */
			pstTempField->szCsvField = (HINT8 *)OxDB_Malloc(nFieldNameLen+1);
			HxSTD_MemCopy(pstTempField->szCsvField, &szLine[nPrevFieldOffset], nFieldNameLen);
			pstTempField->szCsvField[nFieldNameLen] = '\0';
			HLIB_STD_StrLower(pstTempField->szCsvField);

			pstPrevField->pstNext = pstTempField;
			pstPrevField = pstPrevField->pstNext;
		}
	}

	*ppstFieldListHead = pstFieldListHead;
	return ERR_OK;
}

#define _________field_setting__________

STATIC DxDeliveryType_e db_svc_PrechSetDeliveryType(HINT8 *szFieldData)
{
	if(szFieldData != NULL)
	{
		if(szFieldData[0] == 'S' || szFieldData[0] == 's')
			return eDxDELIVERY_TYPE_SAT;
		else if(szFieldData[0] == 'T' || szFieldData[0] == 't')
			return eDxDELIVERY_TYPE_TER;
		else if(szFieldData[0] == 'C' || szFieldData[0] == 'c')
			return eDxDELIVERY_TYPE_CAB;
		else
			return eDxDELIVERY_TYPE_SAT;
	}
	else
	{
		return eDxDELIVERY_TYPE_SAT;
	}
}

#define _________net_fields__________________________________
STATIC void db_svc_PrechSetNetField_usNetId(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	pstNetInfo->usNetId = (HUINT16)nFieldValue;
}

STATIC void db_svc_PrechSetNetField_eOrgDeliType(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	pstNetInfo->eOrgDeliType = db_svc_PrechSetDeliveryType(szFieldData);
}

STATIC void db_svc_PrechSetNetField_szNetName(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	if(szFieldData == NULL)
		return;

	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	HxSTD_MemCopy(pstNetInfo->szNetName, szFieldData, DxMAX_NET_NAME_LEN);
	pstNetInfo->szNetName[DxMAX_NET_NAME_LEN - 1] = '\0';

	HxLOG_Print("	--> szNetName	: %s\n", pstNetInfo->szNetName);
}

#define _________ts_fields______________________________________
/* usTsId, usOnId, eDeliType은 SVC_TsInfo_t에서 공통파라미터 */
STATIC void db_svc_PrechSetTsField_usTsId(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	pstTsInfo->usTsId = (HUINT16)nFieldValue;
}

STATIC void db_svc_PrechSetTsField_usOnId(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	pstTsInfo->usOnId = (HUINT16)nFieldValue;
}

STATIC void db_svc_PrechSetTsField_eDeliType(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	pstTsInfo->eDeliType = db_svc_PrechSetDeliveryType(szFieldData);
}

#ifdef CONFIG_MW_CH_SATELLITE
STATIC void db_svc_PrechSetTsField_Sat_ulFrequency(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	pstTsInfo->stTuningParam.sat.ulFrequency = (HUINT32)nFieldValue;
}

STATIC void db_svc_PrechSetTsField_Sat_ulSymbolRate(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	pstTsInfo->stTuningParam.sat.ulSymbolRate = (HUINT32)nFieldValue;
}

STATIC void db_svc_PrechSetTsField_Sat_eFecCodeRate(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	HINT8	szFec[6];

	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	if(szFieldData != NULL)
	{
		HxSTD_MemCopy(szFec, szFieldData, 5);
		szFec[5] = '\0';
		MWC_UTIL_ConvertCharUpper((HUINT8*)szFec);

		if(0 == HxSTD_StrCmp("AUTO", szFec))
			pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_AUTO;
		else if(0 == HxSTD_StrCmp("1_2", szFec))
			pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_1_2;
		else if(0 == HxSTD_StrCmp("2_3", szFec))
			pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_2_3;
		else if(0 == HxSTD_StrCmp("3_4", szFec))
			pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_3_4;
		else if(0 == HxSTD_StrCmp("5_6", szFec))
			pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_5_6;
		else if(0 == HxSTD_StrCmp("7_8", szFec))
			pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_7_8;
		/* DVB-S2 code rate --> */
		else if(0 == HxSTD_StrCmp("3_5", szFec))
			pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_3_5;
		else if(0 == HxSTD_StrCmp("4_5", szFec))
			pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_4_5;
		else if(0 == HxSTD_StrCmp("5_11", szFec))
			pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_5_11;
		else if(0 == HxSTD_StrCmp("6_7", szFec))
			pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_6_7;
		else if(0 == HxSTD_StrCmp("8_9", szFec))
			pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_8_9;
		else if(0 == HxSTD_StrCmp("9_10", szFec))
			pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_9_10;
		else
			pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_AUTO;
	}
	else
	{
		pstTsInfo->stTuningParam.sat.eFecCodeRate = eDxSAT_CODERATE_AUTO;
	}

}

STATIC void db_svc_PrechSetTsField_Sat_ePolarization(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	if(szFieldData != NULL)
	{
		if(szFieldData[0] == 'H' || szFieldData[0] == 'h')
			pstTsInfo->stTuningParam.sat.ePolarization = eDxSAT_POLAR_HOR;
		else if(szFieldData[0] == 'V' || szFieldData[0] == 'v')
			pstTsInfo->stTuningParam.sat.ePolarization = eDxSAT_POLAR_VER;
		else
			pstTsInfo->stTuningParam.sat.ePolarization = eDxSAT_POLAR_AUTO;
	}
	else
	{
		pstTsInfo->stTuningParam.sat.ePolarization = eDxSAT_POLAR_AUTO;
	}
}

STATIC void db_svc_PrechSetTsField_Sat_eSatType(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	if(szFieldData != NULL)
	{
		MWC_UTIL_ConvertCharUpper((HUINT8*)szFieldData);
		if(0 == HxSTD_StrCmp("ASTRA_1", szFieldData))
			pstTsInfo->stTuningParam.sat.eSatType = eSatType_ASTRA_1;
		else if(0 == HxSTD_StrCmp("HOTBIRD", szFieldData))
			pstTsInfo->stTuningParam.sat.eSatType = eSatType_HOTBIRD;
		else
			pstTsInfo->stTuningParam.sat.eSatType = eSatType_ASTRA_1;
	}
	else
	{
		pstTsInfo->stTuningParam.sat.eSatType = (SVC_SatType_t)nFieldValue;
	}
}

STATIC void db_svc_PrechSetTsField_Sat_eTransSpec(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	HINT8	szTransSpec[8];

	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	if(szFieldData != NULL)
	{
		HxSTD_MemCopy(szTransSpec, szFieldData, 6);
		szTransSpec[6] = '\0';
		MWC_UTIL_ConvertCharUpper((HUINT8*)szTransSpec);

		if(0 == HxSTD_StrCmp("DVB-S", szTransSpec))
			pstTsInfo->stTuningParam.sat.eTransSpec = eDxSAT_TRANS_DVBS;
		else if(0 == HxSTD_StrCmp("DVB-S2", szTransSpec))
			pstTsInfo->stTuningParam.sat.eTransSpec = eDxSAT_TRANS_DVBS2;
		else
			pstTsInfo->stTuningParam.sat.eTransSpec = eDxSAT_TRANS_DVBS;
	}
	else
	{
		pstTsInfo->stTuningParam.sat.eTransSpec = eDxSAT_TRANS_DVBS;
	}
}
#endif

#ifdef CONFIG_MW_CH_CABLE
STATIC void db_svc_PrechSetTsField_Cab_ulFreq(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	pstTsInfo->stTuningParam.cab.ulFreq = (HUINT32)nFieldValue;
}

STATIC void db_svc_PrechSetTsField_Cab_usSR(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	pstTsInfo->stTuningParam.cab.usSR = (HUINT16)(nFieldValue & 0xffff);
}

STATIC void db_svc_PrechSetTsField_Cab_ucCon(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	HINT8	szCons[8];

	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	if(szFieldData != NULL)
	{
		HxSTD_MemCopy(szCons, szFieldData, 5);
		szCons[5] = '\0';
		MWC_UTIL_ConvertCharUpper(szCons);

		if(0 == HxSTD_StrNCmp("AUTO", szCons, 4))
			pstTsInfo->stTuningParam.cab.ucCon = eDxSAT_CODERATE_AUTO;
		else if(0 == HxSTD_StrNCmp("16", szCons, 2))
			pstTsInfo->stTuningParam.cab.ucCon = eDxCAB_CONSTELLATION_16QAM;
		else if(0 == HxSTD_StrNCmp("32", szCons, 2))
			pstTsInfo->stTuningParam.cab.ucCon = eDxCAB_CONSTELLATION_32QAM;
		else if(0 == HxSTD_StrNCmp("64", szCons, 2))
			pstTsInfo->stTuningParam.cab.ucCon = eDxCAB_CONSTELLATION_64QAM;
		else if(0 == HxSTD_StrNCmp("128", szCons, 3))
			pstTsInfo->stTuningParam.cab.ucCon = eDxCAB_CONSTELLATION_128QAM;
		else if(0 == HxSTD_StrNCmp("256", szCons, 3))
			pstTsInfo->stTuningParam.cab.ucCon = eDxCAB_CONSTELLATION_256QAM;
		else
			pstTsInfo->stTuningParam.cab.ucCon = eDxSAT_CODERATE_AUTO;
	}
	else
	{
		pstTsInfo->stTuningParam.cab.ucCon = eDxSAT_CODERATE_AUTO;
	}
}

STATIC void db_svc_PrechSetTsField_Cab_ucSpec(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	HINT8	szSpec[10];

	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	if(szFieldData != NULL)
	{
		HxSTD_MemCopy(szSpec, szFieldData, 8);
		szSpec[8] = '\0';
		MWC_UTIL_ConvertCharUpper(szSpec);

		if(0 == HxSTD_StrNCmp("AUTO", szSpec, 4))
			pstTsInfo->stTuningParam.cab.ucSpec = eDxCAB_SPECTRUM_AUTO;
		else if(0 == HxSTD_StrNCmp("NORMAL", szSpec, 6))
			pstTsInfo->stTuningParam.cab.ucSpec = eDxCAB_SPECTRUM_NORMAL;
		else if(0 == HxSTD_StrNCmp("INVERTED", szSpec, 8))
			pstTsInfo->stTuningParam.cab.ucSpec = eDxCAB_SPECTRUM_INVERTED;
		else
			pstTsInfo->stTuningParam.cab.ucSpec = eDxCAB_SPECTRUM_AUTO;
	}
	else
	{
		pstTsInfo->stTuningParam.cab.ucSpec = eDxCAB_SPECTRUM_AUTO;
	}
}
#endif

#define _________prv_fields__________________________
STATIC void db_svc_PrechSetPrvField_szPrvName(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	if(szFieldData == NULL)
		return;

	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	HxSTD_MemCopy(pstPrvInfo->szPrvName, szFieldData, DxMAX_PRV_NAME_LEN);
	pstPrvInfo->szPrvName[DxMAX_PRV_NAME_LEN - 1] = '\0';

	HxLOG_Print("	--> szPrvName	: %s\n", pstPrvInfo->szPrvName);
}

#define _________svc_fields__________________________
STATIC void db_svc_PrechSetSvcField_usLcn(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	DbSvc_SetLcn(pstSvcInfo, nFieldValue);
}

STATIC void db_svc_PrechSetSvcField_szSvcName(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	//HERROR	err;
	HINT32	nTextSize;

	if(szFieldData == NULL)
		return;

	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);

	nTextSize = MWC_UTIL_DvbStrlen((const HUINT8*)szFieldData);
	if(nTextSize > (/*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength() - 2))
		nTextSize = /*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength() - 2;

#if 0
	pstSvcInfo->szSvcName[0] = eHxCP_DVBLANG_VALUE_UTF_8;
	HLIB_STD_StrUtf8NCpy(&pstSvcInfo->szSvcName[1], szFieldData, nTextSize+1); // 1번지부터 시작하므로....
#else
	HLIB_STD_StrUtf8NCpy(DbSvc_GetName(pstSvcInfo), szFieldData, nTextSize+1);
#endif
	HxLOG_Print("	--> szSvcName	: %s\n", DbSvc_GetName(pstSvcInfo));
}

STATIC void db_svc_PrechSetSvcField_eSvcType(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	if(szFieldData != NULL)
	{
		if(szFieldData[0] == 'T' || szFieldData[0] == 't')
			DbSvc_SetSvcType(pstSvcInfo, eDxSVC_TYPE_TV);
		else if(szFieldData[0] == 'R' || szFieldData[0] == 'r')
			DbSvc_SetSvcType(pstSvcInfo, eDxSVC_TYPE_RADIO);
		else if(szFieldData[0] == 'D' || szFieldData[0] == 'd')
			DbSvc_SetSvcType(pstSvcInfo, eDxSVC_TYPE_DATA);
		else
			DbSvc_SetSvcType(pstSvcInfo, eDxSVC_TYPE_TV);
	}
	else
	{
		DbSvc_SetSvcType(pstSvcInfo, eDxSVC_TYPE_TV);
	}
}

STATIC void db_svc_PrechSetSvcField_eVideoType(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	if(szFieldData != NULL)
	{
		if(szFieldData[0] == 'S' || szFieldData[0] == 's')
			DbSvc_SetVideoType(pstSvcInfo, eDxVIDEO_TYPE_SD);
		else if(szFieldData[0] == 'H' || szFieldData[0] == 'h')
			DbSvc_SetVideoType(pstSvcInfo, eDxVIDEO_TYPE_HD);
		else
			DbSvc_SetVideoType(pstSvcInfo, eDxVIDEO_TYPE_SD);
	}
	else
	{
		DbSvc_SetVideoType(pstSvcInfo, eDxVIDEO_TYPE_SD);
	}
}

STATIC void db_svc_PrechSetSvcField_eCasType(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);

	if(szFieldData != NULL)
	{
		DxCasType_e		bCasType;

		MWC_UTIL_ConvertCharUpper((HUINT8*)szFieldData);

		bCasType = eDxCAS_TYPE_All;

		if(NULL != HxSTD_StrStr("FTA", szFieldData))		bCasType |= eDxCAS_TYPE_FTA;
		if(NULL != HxSTD_StrStr("NA", szFieldData))			bCasType |= eDxCAS_TYPE_NAGRA;
		if(NULL != HxSTD_StrStr("IR", szFieldData))			bCasType |= eDxCAS_TYPE_IRDETO;
		if(NULL != HxSTD_StrStr("ND", szFieldData))			bCasType |= eDxCAS_TYPE_NDS;
		if(NULL != HxSTD_StrStr("VA", szFieldData))			bCasType |= eDxCAS_TYPE_VIACCESS;
		if(NULL != HxSTD_StrStr("CX", szFieldData))			bCasType |= eDxCAS_TYPE_CONAX;
		if(NULL != HxSTD_StrStr("MG", szFieldData))			bCasType |= eDxCAS_TYPE_MEDIAGUARD;
		if(NULL != HxSTD_StrStr("CR", szFieldData))			bCasType |= eDxCAS_TYPE_CRYPTOWORKS;
		if(NULL != HxSTD_StrStr("VX", szFieldData))			bCasType |= eDxCAS_TYPE_VERIMATRIX;

		if (bCasType == eDxCAS_TYPE_All)
		{
			bCasType = eDxCAS_TYPE_FTA;
		}
		DbSvc_SetCasType(pstSvcInfo, bCasType);
	}
	else
	{
		DbSvc_SetCasType(pstSvcInfo, eDxCAS_TYPE_FTA);
	}
}

STATIC void db_svc_PrechSetSvcField_eVideoCodec(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	DbSvc_SetVideoCodec(pstSvcInfo, nFieldValue);
}

STATIC void db_svc_PrechSetSvcField_eAudioCodec(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	DbSvc_SetAudioCodec(pstSvcInfo, nFieldValue);
}

STATIC void db_svc_PrechSetSvcField_usSvcId(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	DbSvc_SetSvcId(pstSvcInfo, nFieldValue);
}

STATIC void db_svc_PrechSetSvcField_usPmtPid(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	DbSvc_SetPmtPid(pstSvcInfo, nFieldValue);
}

STATIC void db_svc_PrechSetSvcField_usVideoPid(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	DbSvc_SetVideoPid(pstSvcInfo, nFieldValue);
}

STATIC void db_svc_PrechSetSvcField_usAudioPid(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	DbSvc_SetAudioPid(pstSvcInfo, nFieldValue);
}

STATIC void db_svc_PrechSetSvcField_usPcrPid(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	DbSvc_SetPcrPid(pstSvcInfo, nFieldValue);
}

STATIC void db_svc_PrechSetSvcField_usTtxPid(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	DbSvc_SetTtxPid(pstSvcInfo, nFieldValue);
}

STATIC void db_svc_PrechSetSvcField_eOrgDeliType(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	DbSvc_SetDeliTypePtr(pstSvcInfo, db_svc_PrechSetDeliveryType(szFieldData));
}

#ifdef CONFIG_MW_CH_SATELLITE
STATIC void db_svc_PrechSetSvcField_eSatType(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	if(szFieldData != NULL)
	{
		MWC_UTIL_ConvertCharUpper((HUINT8*)szFieldData);
		if(0 == HxSTD_StrCmp("ASTRA_1", szFieldData))
			DbSvc_SetDeliTypePtr(pstSvcInfo, eSatType_ASTRA_1);
		else if(0 == HxSTD_StrCmp("HORBIRD", szFieldData))
			DbSvc_SetDeliTypePtr(pstSvcInfo, eSatType_HOTBIRD);
		else
			DbSvc_SetDeliTypePtr(pstSvcInfo, eSatType_ASTRA_1);
	}
	else
	{
		DbSvc_SetDeliTypePtr(pstSvcInfo, (SVC_SatType_t)nFieldValue);
	}
}
#endif

STATIC void db_svc_PrechSetSvcField_usOnId(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	DbSvc_SetOnId(pstSvcInfo, nFieldValue);
}

STATIC void db_svc_PrechSetSvcField_usTsId(DbSvc_Info_t *pstSvcInfo, DbSvc_TsInfo_t *pstTsInfo, DbSvc_PrvInfo_t *pstPrvInfo, DbSvc_NetInfo_t *pstNetInfo, HINT8 *szFieldData, HINT32 nFieldValue)
{
	SVCPRECH_PARAM_WARRING_REMOVE(pstSvcInfo, pstTsInfo, pstPrvInfo, pstNetInfo, szFieldData, nFieldValue);
	DbSvc_SetTsId(pstSvcInfo, nFieldValue);
}

#define _______field_mapping_table___________________________________________________________________

const STATIC dbSvc_CsvColumnMapping_t s_astNetColumnMappingTable[] =
{ /* columns on TBL_NET */
	{ eCol_Mandatory, 	"usNetId",		"network_id" 		,db_svc_PrechSetNetField_usNetId},
	{ eCol_Optional, 	"szNetName",	"network_name" 		,db_svc_PrechSetNetField_szNetName},
	{ eCol_Mandatory, 	"eOrgDeliType",	"modulation" 		,db_svc_PrechSetNetField_eOrgDeliType},
};

const STATIC dbSvc_CsvColumnMapping_t s_astPrvColumnMappingTable[] =
{ /* columns on TBL_PRV */
	{ eCol_Optional, 	"szPrvName",	"provider_name"		,db_svc_PrechSetPrvField_szPrvName},
};

#ifdef CONFIG_MW_CH_SATELLITE
const STATIC dbSvc_CsvColumnMapping_t s_astTsSatColumnMappingTable[] =
{ /* columns on TBL_TS */
	{ eCol_Mandatory, 	"usTsId",		"ts_id" 			,db_svc_PrechSetTsField_usTsId},
	{ eCol_Mandatory, 	"usOrgNetId",	"network_id" 		,db_svc_PrechSetTsField_usOnId},
	{ eCol_Mandatory, 	"eDeliType",	"modulation" 		,db_svc_PrechSetTsField_eDeliType},	/* modulation이 sat인 경우만, 나머지 필드들이 의미 있다 */
	//
	// 다음은 cable 전용
	//--------------------------------------------------------------------------------------------
	{ eCol_Mandatory, 	"ulFrequency",	"frequency" 		,db_svc_PrechSetTsField_Sat_ulFrequency},
	{ eCol_Mandatory, 	"ePolarization","polarization" 		,db_svc_PrechSetTsField_Sat_ePolarization},
	{ eCol_Mandatory, 	"ulSymbolRate",	"symbol_rate" 		,db_svc_PrechSetTsField_Sat_ulSymbolRate},
	{ eCol_Mandatory, 	"eFecCodeRate",	"fec" 				,db_svc_PrechSetTsField_Sat_eFecCodeRate},
	{ eCol_Mandatory, 	"eSatType",		"satellite_type" 	,db_svc_PrechSetTsField_Sat_eSatType},
	{ eCol_Mandatory, 	"eTransSpec",	"transport_spec" 	,db_svc_PrechSetTsField_Sat_eTransSpec},
};

const STATIC dbSvc_CsvColumnMapping_t s_astSvcSatColumnMappingTable[] =
{
	/* columns on TBL_SVC */
	{ eCol_Mandatory, 	"usLcn",		"channel_number"	,db_svc_PrechSetSvcField_usLcn},
	{ eCol_Optional, 	"szSvcName",	"service_name" 		,db_svc_PrechSetSvcField_szSvcName},
	{ eCol_Mandatory, 	"eSvcType",		"service_type" 		,db_svc_PrechSetSvcField_eSvcType},
//	{ eCol_Optional, 	"ucLcnFlag",	"lcn_flag"				},
//	{ eCol_Optional, 	"ucVisibleSvcFlag",	"visible_svc_flag"	},
//	{ eCol_Optional, 	"ucLocked",		"lock" 					},
	{ eCol_Mandatory, 	"eVideoType",	"video_type" 		,db_svc_PrechSetSvcField_eVideoType},
	{ eCol_Optional, 	"eCasType",		"cas_type" 			,db_svc_PrechSetSvcField_eCasType},
	{ eCol_Optional, 	"ucVideoCodec",	"video_codec" 		,db_svc_PrechSetSvcField_eVideoCodec},
	{ eCol_Optional, 	"ucAudioCodec",	"audio_codec" 		,db_svc_PrechSetSvcField_eAudioCodec},

	{ eCol_Mandatory, 	"usSvcId",		"service_id" 		,db_svc_PrechSetSvcField_usSvcId},
	{ eCol_Mandatory, 	"usPmtPid",		"pmt_pid" 			,db_svc_PrechSetSvcField_usPmtPid},
	{ eCol_Mandatory, 	"usVideoPid",	"video_pid" 		,db_svc_PrechSetSvcField_usVideoPid},
	{ eCol_Mandatory, 	"usAudioPid",	"audio_pid" 		,db_svc_PrechSetSvcField_usAudioPid},
	{ eCol_Mandatory, 	"usPcrPid",		"pcr_pid"			,db_svc_PrechSetSvcField_usPcrPid},
	{ eCol_Mandatory, 	"usTtxPid",		"teletext_pid" 		,db_svc_PrechSetSvcField_usTtxPid},
	{ eCol_Mandatory, 	"eOrgDeliType",	"modulation" 		,db_svc_PrechSetSvcField_eOrgDeliType},
	{ eCol_Mandatory, 	"ucSatType",	"satellite_type" 	,db_svc_PrechSetSvcField_eSatType},
	{ eCol_Mandatory, 	"usOnId",		"network_id" 		,db_svc_PrechSetSvcField_usOnId}, /* on_id가 없어서, 그냥 network_id를 사용하기로 한다. 이렇게 해도 위성에서는 큰 문제 없을 듯 */
	{ eCol_Mandatory, 	"usTsId",		"ts_id" 			,db_svc_PrechSetSvcField_usTsId},
	/* 외부로부터 세팅되지 않는 값인, hSvc, ucChListPriority, ucAntIdx 등은 field value를 모두 읽어들인 후 코딩에 의해 값이 부여된다 */
};
#endif

#ifdef CONFIG_MW_CH_CABLE
const STATIC dbSvc_CsvColumnMapping_t s_astTsCabColumnMappingTable[] =
{ /* columns on TBL_TS */
	{ eCol_Mandatory, 	"usTsId",			"ts_id" 			,db_svc_PrechSetTsField_usTsId},
	{ eCol_Mandatory, 	"usOrgNetId",		"network_id" 		,db_svc_PrechSetTsField_usOnId},
	{ eCol_Mandatory, 	"eDeliType",		"modulation" 		,db_svc_PrechSetTsField_eDeliType},	/* modulation이 cab인 경우만, 나머지 필드들이 의미 있다 */
	//
	// 다음은 cable 전용
	//--------------------------------------------------------------------------------------------
	{ eCol_Mandatory, 	"ulFrequency",		"frequency" 		,db_svc_PrechSetTsField_Cab_ulFreq},
	{ eCol_Mandatory, 	"ulSymbolRate",		"symbol_rate" 		,db_svc_PrechSetTsField_Cab_usSR},
	{ eCol_Mandatory, 	"eConstellation",	"constellation" 	,db_svc_PrechSetTsField_Cab_ucCon},
	{ eCol_Mandatory, 	"eSpectrum",		"spectrum" 			,db_svc_PrechSetTsField_Cab_ucSpec},
};

const STATIC dbSvc_CsvColumnMapping_t s_astSvcCabColumnMappingTable[] =
{
	/* columns on TBL_SVC */
	{ eCol_Mandatory, 	"usLcn",		"channel_number"	,db_svc_PrechSetSvcField_usLcn},
	{ eCol_Optional, 	"szSvcName",	"service_name" 		,db_svc_PrechSetSvcField_szSvcName},
	{ eCol_Mandatory, 	"eSvcType",		"service_type" 		,db_svc_PrechSetSvcField_eSvcType},
	{ eCol_Mandatory, 	"eVideoType",	"video_type" 		,db_svc_PrechSetSvcField_eVideoType},
	{ eCol_Optional, 	"eCasType",		"cas_type" 			,db_svc_PrechSetSvcField_eCasType},
	{ eCol_Optional, 	"ucVideoCodec",	"video_codec" 		,db_svc_PrechSetSvcField_eVideoCodec},
	{ eCol_Optional, 	"ucAudioCodec",	"audio_codec" 		,db_svc_PrechSetSvcField_eAudioCodec},

	{ eCol_Mandatory, 	"usSvcId",		"service_id" 		,db_svc_PrechSetSvcField_usSvcId},
	{ eCol_Mandatory, 	"usPmtPid",		"pmt_pid" 			,db_svc_PrechSetSvcField_usPmtPid},
	{ eCol_Mandatory, 	"usVideoPid",	"video_pid" 		,db_svc_PrechSetSvcField_usVideoPid},
	{ eCol_Mandatory, 	"usAudioPid",	"audio_pid" 		,db_svc_PrechSetSvcField_usAudioPid},
	{ eCol_Mandatory, 	"usPcrPid",		"pcr_pid"			,db_svc_PrechSetSvcField_usPcrPid},
	{ eCol_Mandatory, 	"usTtxPid",		"teletext_pid" 		,db_svc_PrechSetSvcField_usTtxPid},
	{ eCol_Mandatory, 	"eOrgDeliType",	"modulation" 		,db_svc_PrechSetSvcField_eOrgDeliType},
	{ eCol_Mandatory, 	"usOnId",		"network_id" 		,db_svc_PrechSetSvcField_usOnId}, /* on_id가 없어서, 그냥 network_id를 사용하기로 한다. 이렇게 해도 위성에서는 큰 문제 없을 듯 */
	{ eCol_Mandatory, 	"usTsId",		"ts_id" 			,db_svc_PrechSetSvcField_usTsId},
	/* 외부로부터 세팅되지 않는 값인, hSvc, ucChListPriority, ucAntIdx 등은 field value를 모두 읽어들인 후 코딩에 의해 값이 부여된다 */
};
#endif

#define _______________________________________________________________________________________

STATIC void db_svc_PrechSetFieldText(HINT32 nTableIndex, dbSvc_CsvFieldList_t *pstFieldInfo, dbSvc_PreChInfo_t *pstPreChInfo, HINT8 *szFieldText)
{
	pstFieldInfo->afnSvcDbSetField[nTableIndex](
									&pstPreChInfo->stSvcInfo,
									&pstPreChInfo->stTsInfo,
									&pstPreChInfo->stPrvInfo,
									&pstPreChInfo->stNetInfo,
									szFieldText,
									0);
}

STATIC void db_svc_PrechSetFieldValue(HINT32 nTableIndex, dbSvc_CsvFieldList_t *pstFieldInfo, dbSvc_PreChInfo_t *pstPreChInfo, HINT32 nFieldValue)
{
	pstFieldInfo->afnSvcDbSetField[nTableIndex](
									&pstPreChInfo->stSvcInfo,
									&pstPreChInfo->stTsInfo,
									&pstPreChInfo->stPrvInfo,
									&pstPreChInfo->stNetInfo,
									NULL,
									nFieldValue);
}

STATIC HERROR db_svc_PrechParseData(
								DxDeliveryType_e		eOrgDeliType,
								dbSvc_UsingTableKind_b 	bwUsingTableKind,
								HINT32 					nStartChNum,
								HINT32 					nEndChNum,
								HUINT8 					*pszLine,
								dbSvc_CsvFieldList_t 	*pstFieldListHead,
								dbSvc_PreChInfo_t 		*pstPreChInfo)
{
	HERROR					 hErr;
	HBOOL					 bTextItem;
	HINT8 					*szFieldValue, szBuf[128];
	HINT32					 nTableIndex, i, nTemp, nFieldVal;
	dbSvc_CsvFieldList_t	*pstTempField;

	if(pstPreChInfo == NULL)
		return ERR_FAIL;

	nTableIndex = db_svc_PrechGetMappingTableIndex(bwUsingTableKind);
	if(nTableIndex < 0)
		return ERR_FAIL;

	i = 1;
	szFieldValue = (HINT8*)pszLine;
	pstTempField = pstFieldListHead;
	while(pstTempField)
	{
		nTemp = db_svc_PrechCountToNextComma(szFieldValue);
		if(0 == (pstTempField->bwUsingTableKind & bwUsingTableKind))
		{ /* 위에서 체크한 내용 중 use flag이 FALSE인 것은 skip */
			szFieldValue += (nTemp+1); /* comma len 포함 */
		}
		else
		{
			// TODO: szFieldValue를 text 또는 integer 값으로 변경해야됨
			if(nTemp > 0)
				HxSTD_MemCopy(szBuf, szFieldValue, nTemp);

			szBuf[nTemp] = '\0';

			szFieldValue += (nTemp+1); /* comma len 포함 */

			db_svc_PrechGetProperFieldStr(szBuf);

			HxLOG_Print("\t(%d)	%s:", i, pstTempField->szCsvField);
			if(HxSTD_StrNCmp(szBuf,"0x",2) == 0)
			{
				nFieldVal = strtol(szBuf, NULL, 16);
				db_svc_PrechSetFieldValue(nTableIndex, pstTempField, pstPreChInfo, nFieldVal);
				HxLOG_Print("	--> %d (bind:%d)\n", nFieldVal, i);
			}
			else
			{
				hErr = db_svc_PrechGetInt32(szBuf, &nFieldVal);
				if(hErr == ERR_FAIL)
				{ /* text field */
					bTextItem = FALSE;
					switch(nTableIndex)
					{
						case eSvcTable:
							if(0 == HxSTD_StrCmp("szSvcName", pstTempField->apszDbField[nTableIndex]))			bTextItem = TRUE;
							else if(0 == HxSTD_StrCmp("eSvcType", pstTempField->apszDbField[nTableIndex]))		bTextItem = TRUE;
							else if(0 == HxSTD_StrCmp("eVideoType", pstTempField->apszDbField[nTableIndex]))	bTextItem = TRUE;
							else if(0 == HxSTD_StrCmp("eCasType", pstTempField->apszDbField[nTableIndex]))		bTextItem = TRUE;
							else if(0 == HxSTD_StrCmp("ucSatType", pstTempField->apszDbField[nTableIndex]))		bTextItem = TRUE;
							else if(0 == HxSTD_StrCmp("szSvcName", pstTempField->apszDbField[nTableIndex]))		bTextItem = TRUE;
							break;

						case eTsTable:
							switch(eOrgDeliType)
							{
#ifdef CONFIG_MW_CH_SATELLITE
								case eDxDELIVERY_TYPE_SAT:
								{
									if(0 == HxSTD_StrCmp("eDeliType", pstTempField->apszDbField[nTableIndex]))
									{
										if((szBuf[0] == 'S') || (szBuf[0] == 's'))
											bTextItem = TRUE;
										else
											return ERR_FAIL;
									}
									else if(0 == HxSTD_StrCmp("ePolarization", pstTempField->apszDbField[nTableIndex]))	bTextItem = TRUE;
									else if(0 == HxSTD_StrCmp("eTransSpec", pstTempField->apszDbField[nTableIndex]))	bTextItem = TRUE;
									else if(0 == HxSTD_StrCmp("eFecCodeRate", pstTempField->apszDbField[nTableIndex]))	bTextItem = TRUE;
									else if(0 == HxSTD_StrCmp("eSatType", pstTempField->apszDbField[nTableIndex]))		bTextItem = TRUE;
									break;
								}
#endif
								default:
									break;
							}
							break;

						case ePrvTable:
							if(0 == HxSTD_StrCmp("szPrvName", pstTempField->apszDbField[nTableIndex]))			bTextItem = TRUE;
							break;

						default:
							break;
					}
					if(bTextItem == TRUE)
					{
						db_svc_PrechSetFieldText(nTableIndex, pstTempField, pstPreChInfo, szBuf);
						if(0 == HxSTD_StrCmp("szSvcName", pstTempField->apszDbField[nTableIndex]))
						{
							HxLOG_Print("	--> %s (bind:%d)\n", szBuf, i);
							HxLOG_Print("[db_svc_PrechParseData] stPreChInfo.stSvcInfo.szSvcName(%s)\n", DbSvc_GetName(&pstPreChInfo->stSvcInfo));
						}
					}
					else
					{
						db_svc_PrechSetFieldValue(nTableIndex, pstTempField, pstPreChInfo, nFieldVal);
						HxLOG_Print("	--> %d (bind:%d)\n", nFieldVal, i);
					}
				}
				else
				{ /* value */
					/* start, end channel number 조건은 0보다 큰 경우만, 의미 있는 값 */
					if((nStartChNum > 0) && (nEndChNum >= nStartChNum))
					{
						if((nTableIndex == eSvcTable) && (HxSTD_StrCmp("usLcn", pstTempField->apszDbField[nTableIndex]) == 0))
						{
							if((nFieldVal < nStartChNum) || (nFieldVal > nEndChNum))
							{ /* Start, End 범위를 초과하는 경우 추가 하지 않는다 */
								return ERR_FAIL;
							}
						}
					}
					db_svc_PrechSetFieldValue(nTableIndex, pstTempField, pstPreChInfo, nFieldVal);
					HxLOG_Print("	--> %d (bind:%d)\n", nFieldVal, i);
				}
			}

			i++;
		}

		pstTempField = pstTempField->pstNext;
	}


	return ERR_OK;
}

STATIC HERROR db_svc_PrechAddPreChInfo(DxDeliveryType_e eOrgDeliType, dbSvc_PreChInfo_t *pstPreChInfo)
{
	HERROR					 hErr;
	HBOOL					 bNewTs;
	DbSvc_Info_t			*pstSvcInfo;
	DbSvc_TsInfo_t			*pstTsInfo, *pstTsInfoList = NULL, stExistingTsInfo;
	DbSvc_PrvInfo_t			*pstPrvInfo;
	DbSvc_NetInfo_t			*pstNetInfo, stExistingNetInfo;
	DbSvc_TsCondition_t		 stTsCond;
	HINT32					 i, nTsCnt;
	HUINT16					 usNetIdx, usTsIdx = 0, usPrvIdx;

	DbSvc_AntInfo_t			stAntInfo;
	HUINT16				 	ulAntIdx = DEF_ANT_INDEX;
	HINT32					nAntCnt, nTotalAntNum;
	HINT32					*pnAntIdxArray;

	if(pstPreChInfo == NULL)
		return ERR_FAIL;

	pstSvcInfo	= &pstPreChInfo->stSvcInfo;
	pstTsInfo	= &pstPreChInfo->stTsInfo;
	pstPrvInfo	= &pstPreChInfo->stPrvInfo;
	pstNetInfo	= &pstPreChInfo->stNetInfo;

	//
	// 1. net info
	//-----------------------------------------------------
	{
		/* find existing network info */
		pstNetInfo->eOrgDeliType = eOrgDeliType;
		hErr = db_svc_MgrFindNetworkIndexById(pstNetInfo->eOrgDeliType, pstNetInfo->usNetId, &usNetIdx);
		if(hErr == ERR_OK)
		{ /* update existing info */
			if(pstNetInfo->szNetName[0] != '\0')
			{
				hErr = DB_SVC_GetNetworkInfo(usNetIdx, &stExistingNetInfo);
				if (hErr == ERR_OK)
				{
					if(0 != MWC_UTIL_DvbStrcmp(pstNetInfo->szNetName, stExistingNetInfo.szNetName))
					{
						db_svc_MgrUpdateNetworkInfo(usNetIdx, pstNetInfo);
					}
				}
			}
		}
		else
		{ /* add newly */
			db_svc_MgrAddNetworkInfo(pstNetInfo, &usNetIdx);
		}
	}

	//
	// 2. ts info
	//-----------------------------------------------------
	{
		bNewTs = TRUE;
		pstTsInfo->usNetIdx = usNetIdx;
		pstTsInfo->eDeliType = eOrgDeliType;

		hErr = db_svc_MgrFindAllAntIdxList (&nTotalAntNum, &pnAntIdxArray);
		if (hErr == ERR_OK)
		{
			for (nAntCnt = 0; nAntCnt < nTotalAntNum; nAntCnt++)
			{
				hErr = db_svc_MgrGetAntInfo ((HUINT16)pnAntIdxArray[nAntCnt], &stAntInfo);
				if (hErr == ERR_OK)
				{
					if (stAntInfo.eSatType == pstTsInfo->stTuningParam.sat.eSatType)
					{
						ulAntIdx = (HUINT16)pnAntIdxArray[nAntCnt];
						break;
					}
				}
			}

			if (pnAntIdxArray != NULL)
				OxDB_Free(pnAntIdxArray);
		}

		pstTsInfo->stTuningParam.sat.ucAntId = (HUINT8)ulAntIdx;

		// find existing ts
		db_svc_TransponderInitFilter(NULL, &stTsCond);

		stTsCond.nDeliType	= eOrgDeliType;
		stTsCond.nOnId		= (HINT32)pstTsInfo->usOnId;
		stTsCond.nTsId		= (HINT32)pstTsInfo->usTsId;

		hErr = db_svc_MgrFindTsInfoList(&stTsCond, &nTsCnt, &pstTsInfoList);
		if(hErr == ERR_OK && pstTsInfoList != NULL)
		{
			for(i = 0; i < nTsCnt; i++)
			{
				switch(eOrgDeliType)
				{
					case eDxDELIVERY_TYPE_SAT:
						if(pstTsInfoList[i].stTuningParam.sat.eSatType == pstTsInfo->stTuningParam.sat.eSatType)
						{
							stExistingTsInfo = pstTsInfoList[i];
							usTsIdx = stExistingTsInfo.usTsIdx;
							/*
								tuning info 비교하기전 antenna index를 같게 만들어 줘야 제대로 비교가 된다.
								왜냐하면, pstTsInfo는 아직 antenna가 할당 되기 전이다.
							*/
							pstTsInfo->stTuningParam.sat.ucAntId = stExistingTsInfo.stTuningParam.sat.ucAntId;
							if(0 == DB_SVC_UtilCompareDbTuningParam(eOrgDeliType, &(pstTsInfo->stTuningParam), &(stExistingTsInfo.stTuningParam)))
							{ /* update existing info */
								bNewTs = FALSE;
								db_svc_MgrUpdateTsInfo(usTsIdx, pstTsInfo);
								break;
							}
						}
						break;

					case eDxDELIVERY_TYPE_CAB:
						stExistingTsInfo = pstTsInfoList[i];
						usTsIdx = stExistingTsInfo.usTsIdx;
						if(0 == DB_SVC_UtilCompareDbTuningParam(eOrgDeliType, &(pstTsInfo->stTuningParam), &(stExistingTsInfo.stTuningParam)))
						{ /* update existing info */
							bNewTs = FALSE;
							db_svc_MgrUpdateTsInfo(usTsIdx, pstTsInfo);
							break;
						}
						break;

					default:
						break;
				}
			}

			if (pstTsInfoList)
			{
				OxDB_Free(pstTsInfoList);
			}
		}

		if(bNewTs == TRUE)
		{ /* add newly */
			hErr = db_svc_MgrAddTsInfo(pstTsInfo, &usTsIdx);
			if(hErr != ERR_OK)
				return hErr;
		}
	}

	//
	// 3. prv info
	//-----------------------------------------------------
	{
		hErr = db_svc_MgrFindProviderIndexByName(pstPrvInfo->szPrvName, &usPrvIdx);
		if(hErr != ERR_OK)
		{ /* add newly */
			// 등록되어 있지 않으면 추가한다.
			db_svc_MgrAddProviderInfo(pstPrvInfo, &usPrvIdx);
		}
	}

	//
	// 4. svc info
	//-----------------------------------------------------
	{
		HUINT32			 ulSvcCnt;
		Handle_t		*phSvcList;
		DxOperator_e	 eOpType;
		HERROR			 hErr;

//		eOpType = MWC_PARAM_GetOperatorType();
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOpType, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
		}

		DbSvc_SetDeliTypePtr(pstSvcInfo, eOrgDeliType);
		DbSvc_SetTsIdx(pstSvcInfo, usTsIdx);
		DbSvc_SetPrvIdx(pstSvcInfo, usPrvIdx);
		DbSvc_SetVisibleFlag(pstSvcInfo, TRUE);

		DbSvc_SetSvcType(pstSvcInfo, DbSvc_GetSvcType(pstSvcInfo));	// MOON_pre
		DbSvc_SetLcnFlag(pstSvcInfo, TRUE);
		DbSvc_SetOpType(pstSvcInfo, eOpType);//OTL_CONV_SysOperator2SvcOperator(eOpType);
		DbSvc_SetAntIdx(pstSvcInfo, pstTsInfo->stTuningParam.sat.ucAntId);
		DbSvc_SetSvcSection(pstSvcInfo, eDxSvcSection_PreProgrammed);

		phSvcList = NULL;
		hErr = db_svc_MgrFindServiceListByNumber(eOrgDeliType, DbSvc_GetSvcType(pstSvcInfo), eLcnVisibleSvc_FindAll, DbSvc_GetLcn(pstSvcInfo), &ulSvcCnt, &phSvcList);
		if(hErr != ERR_OK)
		{
			// 등록되어 있지 않으면 추가한다.
			hErr = db_svc_MgrAddServiceInfo(pstSvcInfo, NULL);
		}

		if (phSvcList != NULL)
			OxDB_Free(phSvcList);
	}

	return hErr;
}

HERROR db_svc_PrechMakeJffs2FullPath(HINT8 *zDbName, HINT8 **ppszFullPath)
{
	HINT32	nFilenameLen, nJffs2PathLen;
	HINT8	*pszBuffer;
	HUINT32	ulSize;

	if(zDbName == NULL)
		return ERR_FAIL;

	nFilenameLen = HxSTD_StrLen(zDbName);
	if(nFilenameLen == 0)
		return ERR_FAIL;

	nJffs2PathLen = HxSTD_StrLen(CSV_FILE_PATH);
	if((nFilenameLen + nJffs2PathLen) <= 0)
		return ERR_FAIL;

	ulSize = nFilenameLen + nJffs2PathLen + 2;
	pszBuffer = OxDB_Malloc(ulSize);
	if (pszBuffer == NULL)
		return ERR_FAIL;

	if(nJffs2PathLen > 0)
	{
		HxSTD_StrNCpy(pszBuffer, CSV_FILE_PATH, ulSize-1);
		HxSTD_StrNCat(pszBuffer, zDbName, ulSize-1);
	}
	else
	{
		HxSTD_StrNCpy(pszBuffer, zDbName, ulSize-1);
	}

	*ppszFullPath = pszBuffer;

	return ERR_OK;
}

/*
	pre-defined chlist db name format:
			pre_ch_xxx.db
			xxx는 iso3166 country code
*/
STATIC HERROR db_svc_PrechMakePreDefinedChFileName(HxCountry_e eCountryId, HINT8 *szFileExtName, HINT8 **ppszFullPath)
{
	HERROR			hErr;
	HCHAR			szIso3166Code[4];
	HINT8			*szDbName;
	HINT32			nBufLen;

#if 1
	{
		const HCHAR *pszIsoCountryCode = NULL;
		pszIsoCountryCode = HLIB_COUNTRY_IdTo3166_Alpha3(eCountryId);
		if (pszIsoCountryCode)
		{
			HxSTD_MemCopy(szIso3166Code, pszIsoCountryCode, 4);
			hErr = ERR_OK;
		}
		else
		{
			hErr = ERR_FAIL;
		}
	}
#else
	hErr = MWC_UTIL_GetIso3166CodeByCountryId(eCountryId, szIso3166Code);
#endif
	if(hErr != ERR_OK)
		return hErr;
	HLIB_STD_StrLower(szIso3166Code);

	nBufLen = HxSTD_StrLen(DB_SVC_PRE_DEFINED_CHLIST_FORMAT_STR) + 4 + HxSTD_StrLen(szFileExtName);
	szDbName = (HINT8 *)OxDB_Malloc(nBufLen);
	snprintf(szDbName, nBufLen, DB_SVC_PRE_DEFINED_CHLIST_FORMAT_STR, szIso3166Code, szFileExtName);

	hErr = db_svc_PrechMakeJffs2FullPath(szDbName, ppszFullPath);
	OxDB_Free(szDbName);

	return hErr;
}

#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

/* 단위: msec */
#define DB_TASK_START_SLEEP						300

STATIC void db_svc_PrechPrintFieldList(dbSvc_CsvFieldList_t *pstFieldListHead)
{
#ifdef CONFIG_DEBUG
	HINT32			i;
	dbSvc_CsvFieldList_t	*pstTempField;

	i = 0;
	pstTempField = pstFieldListHead;
	while(pstTempField)
	{
		i++;
		if(pstTempField->bwUsingTableKind)
		{
			if(0 != (pstTempField->bwUsingTableKind & eTableKind_SVC))
				HxLOG_Print("(final:%d) eTableKind_SVC, db/csv = %s/%s\n", i, pstTempField->apszDbField[eSvcTable], pstTempField->szCsvField);
			if(0 != (pstTempField->bwUsingTableKind & eTableKind_TS))
				HxLOG_Print("(final:%d) eTableKind_TS, db/csv = %s/%s\n", i, pstTempField->apszDbField[eTsTable], pstTempField->szCsvField);
			if(0 != (pstTempField->bwUsingTableKind & eTableKind_PRV))
				HxLOG_Print("(final:%d) eTableKind_PRV, db/csv = %s/%s\n", i, pstTempField->apszDbField[ePrvTable], pstTempField->szCsvField);
			if(0 != (pstTempField->bwUsingTableKind & eTableKind_NET))
				HxLOG_Print("(final:%d) eTableKind_NET, db/csv = %s/%s\n", i, pstTempField->apszDbField[eNetTable], pstTempField->szCsvField);
		}
		pstTempField = pstTempField->pstNext;
	}
#endif
}

STATIC HERROR db_svc_PrechMakeFieldList(DxDeliveryType_e eOrgDeliType, HUINT8 *pszLine, HINT32 nLineLen, dbSvc_CsvFieldList_t **ppstFieldListHead, HINT32 *pnTotalFieldLen)
{
	HERROR					hErr;
	dbSvc_CsvFieldList_t 	*pstFieldListHead = NULL;

	if(pnTotalFieldLen == NULL)
		return ERR_FAIL;

	*ppstFieldListHead = NULL;

	hErr = db_svc_PrechMakeCsvFieldList(pszLine, nLineLen, &pstFieldListHead);
	if(hErr != ERR_OK)
		return ERR_FAIL;

	*pnTotalFieldLen = 0; /* nTotalFieldLen값은 이후로 계속 누적됨 - 다음 스텝에서 sql 만들 때 활용됨 */

	switch(eOrgDeliType)
	{
#ifdef CONFIG_MW_CH_SATELLITE
		case eDxDELIVERY_TYPE_SAT:
			hErr = db_svc_PrechCheckMandatoryField(eTableKind_SVC, SVC_SAT_MAPPING_COLUMN_NUM, s_astSvcSatColumnMappingTable, pstFieldListHead, pnTotalFieldLen);
			if(hErr != ERR_OK)
			{
			    db_svc_PrechFreeCsvFieldList(pstFieldListHead);
			    return ERR_FAIL;
			}

			hErr = db_svc_PrechCheckMandatoryField(eTableKind_TS, TS_SAT_MAPPING_COLUMN_NUM, s_astTsSatColumnMappingTable, pstFieldListHead, pnTotalFieldLen);
			if(hErr != ERR_OK)
			{
			    db_svc_PrechFreeCsvFieldList(pstFieldListHead);
			    return ERR_FAIL;
			}
			break;
#endif
#ifdef CONFIG_MW_CH_CABLE
		case eDxDELIVERY_TYPE_CAB:
			hErr = db_svc_PrechCheckMandatoryField(eTableKind_SVC, SVC_CAB_MAPPING_COLUMN_NUM, s_astSvcCabColumnMappingTable, pstFieldListHead, pnTotalFieldLen);
			if(hErr != ERR_OK)
			{
			    db_svc_PrechFreeCsvFieldList(pstFieldListHead);
			    return ERR_FAIL;
			}

			hErr = db_svc_PrechCheckMandatoryField(eTableKind_TS, TS_CAB_MAPPING_COLUMN_NUM, s_astTsCabColumnMappingTable, pstFieldListHead, pnTotalFieldLen);
			if(hErr != ERR_OK)
			{
			    db_svc_PrechFreeCsvFieldList(pstFieldListHead);
			    return ERR_FAIL;
			}
			break;
#endif
		default:
			db_svc_PrechFreeCsvFieldList(pstFieldListHead);
			return ERR_FAIL;
	}

	hErr = db_svc_PrechCheckMandatoryField(eTableKind_PRV, PRV_MAPPING_COLUMN_NUM, s_astPrvColumnMappingTable, pstFieldListHead, pnTotalFieldLen);
	if(hErr != ERR_OK)
	{
	    db_svc_PrechFreeCsvFieldList(pstFieldListHead);
	    return ERR_FAIL;
	}

	hErr = db_svc_PrechCheckMandatoryField(eTableKind_NET, NET_MAPPING_COLUMN_NUM, s_astNetColumnMappingTable, pstFieldListHead, pnTotalFieldLen);
	if(hErr != ERR_OK)
	{
	    db_svc_PrechFreeCsvFieldList(pstFieldListHead);
	    return ERR_FAIL;
	}

	db_svc_PrechPrintFieldList(pstFieldListHead);

	*ppstFieldListHead = pstFieldListHead;

	return ERR_OK;
}

#define LINE_LENGTH_UNIT	300

HINT8 *db_svc_PrechGetLine(FILE *pfIn, HINT32 *pnLineLength)
{
	HINT8 	*pszLine = NULL;
	HINT32 	nLine, n, nEol;

	nLine = LINE_LENGTH_UNIT;
	pszLine = OxDB_Malloc( nLine );
	if( pszLine==0 ) goto err_rtn;
	n = 0;
	nEol = 0;
	while( !nEol )
	{
		if( n+LINE_LENGTH_UNIT>nLine )
		{
			nLine = nLine*2 + LINE_LENGTH_UNIT;
			pszLine = OxDB_Realloc(pszLine, nLine);
			if( pszLine==0 ) goto err_rtn;
		}
		if( fgets(&pszLine[n], nLine - n, pfIn)==0 )
		{
			if( n==0 )
			{
				OxDB_Free(pszLine);
				goto err_rtn;
			}
			pszLine[n] = 0;
			nEol = 1;
			break;
		}
		while( pszLine[n] ){ n++; }
		if( n>0 && pszLine[n-1]=='\n' )
		{
			n--;
			pszLine[n] = 0;
			nEol = 1;
		}
	}
	pszLine = OxDB_Realloc( pszLine, n+1 );
	*pnLineLength = n;
	return pszLine;

err_rtn:
	*pnLineLength = 0;
	return NULL;
}


STATIC HERROR db_svc_PrechLoadPreProgrammedChannelInDelivery(DxDeliveryType_e eOrgDeliType, HINT8 *pPreChCsvFullName, HINT32 nStartChNum, HINT32 nEndChNum)
{
	FILE 					*fp = 0;
	HERROR 					 hErr;
	HUINT8					*pszLine = NULL;
	HINT32					 nLineLen, nTotalFieldLen;
	dbSvc_CsvFieldList_t	*pstFieldListHead = NULL;
	dbSvc_PreChInfo_t		 stPreChInfo;
	HINT32					 i;
	dbSvc_UsingTableKind_b	 bwTableKind;

	fp = fopen(pPreChCsvFullName, "rb");
	if(fp == 0)
	{
		HxLOG_Error("[db_svc_PrechLoadPreProgrammedChannelInDelivery] can't open \"%s\"\n", pPreChCsvFullName);
		goto err_rtn;
	}

	//
	// 1. field list를 구성한다.
	//--------------------------------------------------------

	// 1-1. 첫줄의 필드명을 읽어 들임.
	//--------------------------------------------------------
	pszLine = (HUINT8 *)db_svc_PrechGetLine(fp, &nLineLen);
	if(pszLine == NULL)
	{
		HxLOG_Error("[db_svc_PrechLoadPreProgrammedChannelInDelivery] Fail to get Line.\n");
		goto err_rtn;
	}

	// 1-2. 필요한 모든 필드가 구성되어 있나?
	//		- loading 가능성 판단 : 필수 필드가 모두 매칭되어야 한다.
	//-----------------------------------------------------------------------
	hErr = db_svc_PrechMakeFieldList(eOrgDeliType, pszLine, nLineLen, &pstFieldListHead, &nTotalFieldLen);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("[db_svc_PrechLoadPreProgrammedChannelInDelivery] Fail to Make FieldList\n");
		goto err_rtn;
	}

	if(pszLine)
	{
		// 첫 줄 데이터 Free 함...
		OxDB_Free(pszLine);
	}
	while(NULL != (pszLine = (HUINT8 *)db_svc_PrechGetLine(fp, &nLineLen)))
	{
		hErr = ERR_OK;
		HxSTD_MemSet(&stPreChInfo, 0, sizeof(dbSvc_PreChInfo_t));

		DbSvc_SetSubttlIdx(&stPreChInfo.stSvcInfo, 0xff); // subtitle index null

		for(i=eNetTable; i>=eSvcTable; i--)
		{
			bwTableKind = db_svc_PrechGetUsingTableKind(i);

			hErr = db_svc_PrechParseData(eOrgDeliType, bwTableKind, nStartChNum, nEndChNum, pszLine, pstFieldListHead, &stPreChInfo);
			if(hErr != ERR_OK)
				break;
		}

		if(hErr == ERR_OK)
		{
			db_svc_PrechAddPreChInfo(eOrgDeliType, &stPreChInfo);
		}

		OxDB_Free(pszLine);
		pszLine = NULL;
	}

err_rtn:


	if(pstFieldListHead !=NULL)	db_svc_PrechFreeCsvFieldList(pstFieldListHead);
	if(pszLine)					OxDB_Free(pszLine);
	if(fp != 0) 				fclose(fp);
	return ERR_OK;
}

/* load pre-defined channels, file type to load is csv */
STATIC HERROR db_svc_PrechLoadSyncMode(HINT8 *pcPreChCsvFullName, HINT32 nStartChNum, HINT32 nEndChNum)
{
#if defined(CONFIG_MW_CH_SATELLITE) || defined(CONFIG_MW_CH_CABLE)
	HERROR 		hErr;
#endif
	HxLOG_Info("++++ Enter ++++\n");

	if(pcPreChCsvFullName == NULL)
		return ERR_FAIL;

	HxLOG_Print("%s\n", pcPreChCsvFullName);

	VK_TASK_Sleep(DB_TASK_START_SLEEP);

#ifdef CONFIG_MW_CH_SATELLITE
	hErr = db_svc_PrechLoadPreProgrammedChannelInDelivery(eDxDELIVERY_TYPE_SAT, pcPreChCsvFullName, nStartChNum, nEndChNum);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("satellite preferred channel loading failure...\n");
	}
#endif

#ifdef CONFIG_MW_CH_CABLE
	hErr = db_svc_PrechLoadPreProgrammedChannelInDelivery(eDxDELIVERY_TYPE_CAB, pcPreChCsvFullName, nStartChNum, nEndChNum);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("cable preferred channel loading failure...\n");
	}
#endif

	HxLOG_Info("---- Leave ----\n");
	return ERR_OK;
}

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

#define PRECH_LCN_BASE_ASTRA_START			1
#define PRECH_LCN_BASE_ASTRA_END			99

HERROR db_svc_PrechLoad(HxCountry_e eCountryId)
{
	HERROR			 hErr;
	HINT8			*pcPreChCsvFullName = NULL;

	HxLOG_Info("++++ Enter ++++\n");

	// 국가코드에 따라 csv file 명을 가져온다
	hErr = db_svc_PrechMakePreDefinedChFileName(eCountryId, "csv", &pcPreChCsvFullName);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Fail to Make CSV FILE NAME !!!\n");
		return hErr;
	}

	hErr = db_svc_PrechLoadSyncMode(pcPreChCsvFullName, PRECH_LCN_BASE_ASTRA_START, PRECH_LCN_BASE_ASTRA_END);

	if (pcPreChCsvFullName)
		OxDB_Free(pcPreChCsvFullName);

	HxLOG_Info("---- Leave ----\n");

	return hErr;
}

/*********************** End of File ******************************/

