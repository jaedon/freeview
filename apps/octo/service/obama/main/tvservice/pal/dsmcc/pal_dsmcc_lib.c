/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include "util.h"
#include <pal_sef.h>
#include <pal_dsmcc.h>

#include "pal_dsmcc_lib.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define 		VALUE_DSMCC_PROTOCOLDISCRIMINATOR		0x11
#define		SECTION_PRIVATE_MAXLEN					0x4096
#define		TVTV_MODULE_SIZE_MAX						(50*1024)
#define		DSMCC_MAX_FILE_FULLPATH_LENGTH			255

typedef enum
{
	DSMCC_TABLEID							= 1,
	DSMCC_SECTION_ETCINFO				= 2,
	DSMCC_TABLEID_EXT						= 2,
	DSMCC_CRC								= 4,
	DSMCC_SECTIONNUMBER					= 1,
	DSMCC_LASTSECTIONNUMBER				= 1,


	DSMCC_PROTOCOLDISCRIMINATOR			= 1,
	DSMCC_DSMCCTYPE						= 1,
	DSMCC_MESSAGEID						= 2,
	DSMCC_TRANSACTIONID					= 4,
	DSMCC_DOWNLOADID						= 4,
	DSMCC_RESERVED						= 1,
	DSMCC_ADAPTATIONLENGTH				= 1,
	DSMCC_MESSAGELENGTH					= 2,


	DSMCC_SERVERID						= 20,
	DSMCC_CMPTDES							= 2,
	DSMCC_PRIVATEDATALEN					= 2,
	DSMCC_NUMOFGROUP						= 2,

	DSMCC_GROUPID							= 4,
	DSMCC_GROUPSIZE						= 4,
	DSMCC_GROUPINFOLEN					= 2,

	DSMCC_CMPTDESLENGTH					= 2,
	DSMCC_CMPTDESCOUNT					= 2,
	DSMCC_CMPTDESTYPE					= 1,
	DSMCC_CMPTDESLEN						= 1,
	DSMCC_CMPTSPECIFIERTYPE				= 1,
	DSMCC_CMPTSPECIFIERDATA				= 3,
	DSMCC_CMPTMODEL						= 2,
	DSMCC_CMPTVER							= 2,

	DSMCC_SUBDESCOUNT					= 1,


	DSMCC_DII_DOWNLOADID				= 4,
	DSMCC_DII_BLOCKSIZE					= 2,
	DSMCC_DII_WINDOWSIZE					= 1,
	DSMCC_DII_ACKPERIOD					= 1,
	DSMCC_DII_DOWNLOADWINDOW			= 4,
	DSMCC_DII_DOWNLOADSCENARIO			= 4,
	DSMCC_DII_NUMOFMODULE				= 2,
	DSMCC_DII_MODULEID					= 2,
	DSMCC_DII_MODULESIZE					= 4,
	DSMCC_DII_MODULEVER					= 1,
	DSMCC_DII_MODULEINFOLEN				= 1,
	DSMCC_DII_PRIVATEDATALEN				= 2,

	DSMCC_DDB_MODULEID					= 2,
	DSMCC_DDB_MODULEVER					= 1,
	DSMCC_DDB_RESERVED					= 1,
	DSMCC_DDB_BLOCKNUMBER				= 2,

	DSMCC_MAX
}DSMCC_HEADER_e;

typedef enum
{
	DSMCC_TABLE_ID_OFFSET								= 0,
	DSMCC_SECTION_SYSTAX_INDICATOR_OFFSET				= 1,
	DSMCC_PRIVATE_INDICATOR_OFFSE						= 1,
	DSMCC_RESERVED1_OFFSET								= 1,
	DSMCC_SECTION_LENGTH_OFFSET							= 1,

	DSMCC_TABLE_ID_EXTENSION_OFFSET					= 3,
	DSMCC_RESERVED2_OFFSET								= 5,
	DSMCC_VERSION_NUMBER_OFFSET						= 5,
	DSMCC_CURRENT_NEXT_INDICATOR_OFFSET				= 5,

	DSMCC_SECTION_NUMBER_OFFSET						= 6,
	DSMCC_LAST_SECTION_NUMBER_OFFSET					= 7,
	DSMCC_MESSAGE_OFFSET								= 8


}DSMCC_SECTION_MAP_e;




/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static DSMCC_INFO_t				s_stDsmccInfo = {0, };




/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
static HERROR				dsmcclib_InitResource(void);
static HERROR				dsmcclib_GetDsmccSessionInfo(HUINT32 *pulTransactionId, HUINT32 *pulDsiCrc, HUINT32 *pulRegisteredTime);
static HERROR				dsmcclib_SetDsmccSessionInfo(HUINT32 ulTransactionId, HUINT32 ulDsiCrc, HUINT32 ulRegisteredTime);
static HERROR				dsmcclib_SetDsmccMode(HUINT32 ulDsmccMode);
static HERROR				dsmcclib_GetDsmccMode(HUINT32 *pulDsmccMode);
static HERROR				dsmcclib_SetDsmccLayerType(HUINT32 ulLayerType);
static HERROR				dsmcclib_GetDsmccLayerType(HUINT32 *pulLayerType);
static HERROR				dsmcclib_GetDownloadSize(HUINT32 *pulSize);
static HERROR				dsmcclib_BuildDii(void);
static HERROR				dsmcclib_BuildDdb(HUINT32 ulLayerType, HUINT8 * pucIsComplete, PalDsmcc_File_t **ppDsmccFile);
static HERROR				dsmcclib_SortDdbListbyNumber(DDBMSG_t *pDdbMsg);
static HERROR				dsmcclib_CheckDiiBuild(HUINT8 *pucIsComplete);
static HERROR				dsmcclib_CheckCompleteDii(DIIMSG_t *pDiiMsg, HUINT8 *pucIsComplete);
static HERROR				dsmcclib_CheckCompleteDSMCC(GROUP_INFO_t *pRootGroup, HUINT16 usTotalNumOfGroup, HUINT8 *pucIsComplete);
static HERROR				dsmcclib_CheckModuleBuild(MODULE_INFO_t *pModuleInfo, HUINT32 ulModuleSize, HBOOL *pbComplete);
static HERROR				dsmcclib_AddGroupInfoList(HUINT16 usNumOfGroup, GROUP_INFO_t *pGroupList);
static HERROR				dsmcclib_AddDiiMsgList(DIIMSG_t *pNewDiiMsg);
static HERROR				dsmcclib_AddDdbMsgList(DDBMSG_t *pNewDdbMsg);

static HERROR				dsmcclib_FindGroupInfo(HUINT32 ulGroupId, GROUP_INFO_t *pGroupList, GROUP_INFO_t **ppGroupInfo);

static HERROR				dsmcclib_FindModuleInfo(	HUINT32 ulLayerType,
															HUINT32 ulDownloadId,
															HUINT16 usModuleId,
															GROUP_INFO_t *pRootGroup,
															DIIMSG_t *pRootDii,
															DIIMSG_t ** ppDiiMsg,
															MODULE_INFO_t **ppModuleInfo);

static HERROR				dsmcclib_ParseDsmccSectionInfo(HUINT8 *pRawData, HINT16 *pusSectionLen, HUINT32 *pulCrc, HUINT8 *pucVerNum);
static HERROR				dsmcclib_ParseDsi(HUINT8 *pRawData, HINT16 sSectionLen, HUINT16 *pusNumOfGroup, GROUP_INFO_t **ppGroupList);
static HERROR				dsmcclib_ParseDii(HUINT8 *pRawData, HINT16 sSectionLen, HUINT32 ulTransactionId, DIIMSG_t **ppDiiMsg);
static HERROR				dsmcclib_ParseDdb(HUINT8 *pRawData, HUINT16 sSectionLen, HUINT32 ulDownloadId, DDBMSG_t **ppDdbMsg);
static HERROR				dsmcclib_ParseDsmccMessageHeader(HUINT8 *pHeader,	HINT32 *plHeaderLen,
																							HUINT16 *pusMessageId,
																							HUINT32 *pulTransactionId);

static HERROR				dsmcclib_ParseDsmccDownloadDataHeader(HUINT8 *pHeader,	HINT32 *plHeaderLen,
																									HUINT16 *pusMessageId,
																									HUINT32 *pulDownloadId);


static HERROR				dsmcclib_ParseGroupInfo(HUINT8 *pRawData, HINT16 sGroupSize, GROUP_INFO_t **ppGroupInfo, HINT32 *plLen);
#if defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
static HERROR				dsmcclib_ParseSubDes(HUINT8 *pRawData, HINT8 cSubDesLen, SUBDES_t **ppSubDes);
static HERROR				dsmcclib_ParseCmptDes(HUINT8 *pRawData, CMPTDES_t *pCmptDes);
#endif
static DIIMSG_t*			dsmcclib_AllocDiiMsg(void);
static MODULE_INFO_t*		dsmcclib_AllocModuleInfo(void);
static DDBMSG_t*			dsmcclib_AllocDdbMsg (void);
#if defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
static CMPTDES_t* 		dsmcclib_AllocCmptDes(void);
static CMPTDES_INFO_t*	dsmcclib_AllocCmptDesInfo(void);
static SUBDES_t*			dsmcclib_AllocSubDes(void);
#endif
static GROUP_INFO_t*		dsmcclib_AllocGroupInfo (void);
static DIIMSG_t*			dsmcclib_DeleteDiiMsg(DIIMSG_t *pRoot, DIIMSG_t *pDiiMsg, HBOOL bFree);
static GROUP_INFO_t*		dsmcclib_AppendGroupInfo(GROUP_INFO_t *pRoot, GROUP_INFO_t *pGroupInfo);
#if defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
static CMPTDES_INFO_t*	dsmcclib_AppendCmptDesInfo (CMPTDES_INFO_t *pRoot, CMPTDES_INFO_t *pCmptDesInfo);
static SUBDES_t*			dsmcclib_AppendSubDes (SUBDES_t *pRoot, SUBDES_t *pSubDes);
#endif
static MODULE_INFO_t*		dsmcclib_AppendModuleInfo (MODULE_INFO_t *pRoot, MODULE_INFO_t *pModuleInfo);
static DIIMSG_t*			dsmcclib_AppendDiiMsg(DIIMSG_t *pRoot, DIIMSG_t *pDiiMsg);
static DDBMSG_t*			dsmcclib_AppendDdbMsg (DDBMSG_t *pRoot, DDBMSG_t *pDdbMsg);
static void				dsmcclib_FreeDsmcc(void);
static void				dsmcclib_FreeDdbMsg (DDBMSG_t *pDdbMsg);
static void				dsmcclib_FreeModuleInfo (MODULE_INFO_t *pModuleInfo);
static void				dsmcclib_FreeDiiMsg (DIIMSG_t *pDiiMsg);
#if defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
static void				dsmcclib_FreeCmptDes(CMPTDES_t *pCmptDes);
static void				dsmcclib_FreeCmptDesNode(CMPTDES_INFO_t *pCmptDesInfo);
static void				dsmcclib_FreeSubDes(SUBDES_t *pSubDes);
#endif
static void				dsmcclib_FreeGroupInfo (GROUP_INFO_t *pGroupInfo);




#define	______GLOBAL_FUNC_START______
HERROR				DSMCCLIB_InitResource(void)
{
	return dsmcclib_InitResource();
}

HERROR				DSMCCLIB_GetDsmccSessionInfo(HUINT32 *pulTransactionId, HUINT32 *pulDsiCrc, HUINT32 *pulRegisteredTime)
{
	return dsmcclib_GetDsmccSessionInfo(pulTransactionId, pulDsiCrc, pulRegisteredTime);
}

HERROR				DSMCCLIB_SetDsmccSessionInfo(HUINT32 ulTransactionId, HUINT32 ulDsiCrc, HUINT32 ulRegisteredTime)
{
	return dsmcclib_SetDsmccSessionInfo(ulTransactionId, ulDsiCrc, ulRegisteredTime);
}

HERROR				DSMCCLIB_SetDsmccMode(HUINT32 ulDsmccMode)
{
	return dsmcclib_SetDsmccMode(ulDsmccMode);
}

HERROR				DSMCCLIB_GetDsmccMode(HUINT32 *pulDsmccMode)
{
	return dsmcclib_GetDsmccMode(pulDsmccMode);
}

HERROR				DSMCCLIB_SetDsmccLayerType(HUINT32 ulLayerType)
{
	return dsmcclib_SetDsmccLayerType(ulLayerType);
}

HERROR				DSMCCLIB_GetDsmccLayerType(HUINT32 *pulLayerType)
{
	return dsmcclib_GetDsmccLayerType(pulLayerType);
}

HERROR				DSMCCLIB_GetDownloadSize(HUINT32 *pulSize)
{
	return dsmcclib_GetDownloadSize(pulSize);
}


#define	______GLOBAL_FREE_FUNC______
void					DSMCCLIB_FreeDsmcc(void)
{
	dsmcclib_FreeDsmcc();
}

void					DSMCCLIB_FreeDdbMsg (DDBMSG_t *pDdbMsg)
{
	dsmcclib_FreeDdbMsg (pDdbMsg);
}

void					DSMCCLIB_FreeDiiMsg (DIIMSG_t *pDiiMsg)
{

	dsmcclib_FreeDiiMsg (pDiiMsg);
}

void					DSMCCLIB_FreeGroupInfo (GROUP_INFO_t *pGroupInfo)
{

	dsmcclib_FreeGroupInfo (pGroupInfo);
}



#define	______GLOBAL_PARSING_FUNC______

HERROR				DSMCCLIB_ParseDsmccSectionInfo(HUINT8 *pRawData, HUINT16 *psSectionLen, HUINT32 *pulCrc, HUINT8 *pucVerNum)
{
	return dsmcclib_ParseDsmccSectionInfo(pRawData, psSectionLen, pulCrc, pucVerNum);
}


HERROR				DSMCCLIB_ParseDsi(HUINT8 *pRawData, HINT16 sSectionLen, HUINT16 *pusNumOfGroup, GROUP_INFO_t **ppGroupList)
{
	return dsmcclib_ParseDsi(pRawData, sSectionLen, pusNumOfGroup, ppGroupList);
}


HERROR				DSMCCLIB_ParseDii(HUINT8 *pRawData, HINT16 uSectionLen, HUINT32 ulTransactionId, DIIMSG_t **ppDiiMsg)
{
	return dsmcclib_ParseDii(pRawData, uSectionLen, ulTransactionId, ppDiiMsg);
}


HERROR				DSMCCLIB_ParseDdb(HUINT8 *pRawData, HUINT16 sSectionLen, HUINT32 ulDownloadId, DDBMSG_t **ppDdbMsg)
{
	return dsmcclib_ParseDdb(pRawData, sSectionLen, ulDownloadId, ppDdbMsg);
}


HERROR				DSMCCLIB_ParseDsmccMessageHeader(	HUINT8 *pHeader,
																	HINT32 *plHeaderLen,
																	HUINT16 *pusMessageId,
																	HUINT32 *pulTransactionId)
{
	return dsmcclib_ParseDsmccMessageHeader( pHeader, plHeaderLen, pusMessageId, pulTransactionId);
}

HERROR				DSMCCLIB_ParseDsmccDownloadDataHeader(	HUINT8 *pHeader,
																			HINT32 *plHeaderLen,
																			HUINT16 *pusMessageId,
																			HUINT32 *pulDownloadId)
{
	return dsmcclib_ParseDsmccDownloadDataHeader( pHeader, plHeaderLen, pusMessageId, pulDownloadId);
}


#define	______GLOBAL_BUILD_FUNC______
HERROR				DSMCCLIB_BuildDii(void)
{
	return dsmcclib_BuildDii();
}



HERROR				DSMCCLIB_BuildDdb(HUINT32 ulLayerType, HUINT8 * pucIsComplete, PalDsmcc_File_t **ppDsmccFile)
{
	return dsmcclib_BuildDdb(ulLayerType,  pucIsComplete, ppDsmccFile);
}



#define	______GLOBAL_ADD_FUNC______
HERROR				DSMCCLIB_AddGroupInfoList(HUINT16 usNumOfGroup, GROUP_INFO_t *pGroupList)
{
	return dsmcclib_AddGroupInfoList(usNumOfGroup, pGroupList);
}


HERROR				DSMCCLIB_AddDiiMsgList(DIIMSG_t *pNewDiiMsg)
{
	return dsmcclib_AddDiiMsgList(pNewDiiMsg);
}


HERROR				DSMCCLIB_AddDdbMsgList(DDBMSG_t *pNewDdbMsg)
{

	return dsmcclib_AddDdbMsgList(pNewDdbMsg);
}

#define	______GLOBAL_CHECK_FUNC______
HERROR				DSMCCLIB_CheckDiiBuild(HUINT8 *pucIsComplete)
{
	return dsmcclib_CheckDiiBuild(pucIsComplete);
}




#if 0
#define	______TEMP_FILEIO_START______
#include <stdio.h>

#define	DSMCC_FILEIO_READONLY			"rb"
#define	DSMCC_FILEIO_WRITEONLY			"wb"


static HERROR		dsmcclib_fopen(HUINT8 *pszFileName, HUINT8 *ucMode, HFILEHANDLE *phFileHandle);
static HERROR		dsmcclib_fread(void *pBuffer, HUINT32 ulUnitSize, HUINT32 ulUnitNum, HFILEHANDLE hFileHandle, HUINT32 *pulReadSize);
static HERROR		dsmcclib_fwrite(void *pBuffer, HUINT32 ulUnitSize, HUINT32 ulUnitNum, HFILEHANDLE hFileHandle, HUINT32 *pulWriteSize);
static HERROR		dsmcclib_fclose(HFILEHANDLE hFileHandle);
static HERROR		dsmcclib_filelength(HFILEHANDLE hFileHandle, HUINT32 *pulFileLength);


static HERROR		dsmcclib_fopen(HUINT8 *pszFileName, HUINT8 *ucMode, HFILEHANDLE *phFileHandle)
{
	FILE				*pFile = NULL;
	HERROR			hError = ERR_FAIL;

	if(pszFileName == NULL || ucMode == NULL || phFileHandle == NULL)
	{
		HxLOG_Print("[dsmcclib_fopen] Param NULL!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	*phFileHandle = NULL;

	pFile = fopen((const char*)pszFileName, (const char*)ucMode);
	if(pFile == NULL)
	{
		HxLOG_Print("[dsmcclib_fopen] fopen fail!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	*phFileHandle =(HFILEHANDLE)pFile ;
	hError = ERR_OK;

EXIT_DSMCCLIB_FUNC :

	return hError;

}


static HERROR		dsmcclib_fread(void *pBuffer, HUINT32 ulUnitSize, HUINT32 ulUnitNum, HFILEHANDLE hFileHandle, HUINT32 *pulReadSize)
{
	HERROR			hError = ERR_FAIL;
	HUINT32			ulReadUnit = 0;

	if(pBuffer == NULL || hFileHandle == NULL || pulReadSize == NULL)
	{
		HxLOG_Print("[dsmcclib_fread] Param NULL!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	*pulReadSize = 0;

	ulReadUnit = (HUINT32)fread((void *)pBuffer, (size_t)ulUnitSize, (size_t)ulUnitNum, (FILE*)hFileHandle);
	if(ulReadUnit == 0)
	{
		HxLOG_Print("[dsmcclib_fread] reading fail 1!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}
	else if(ulReadUnit != ulUnitNum)
	{
		HxLOG_Print("[dsmcclib_fread] reading fail 2!!\n");
	}

	*pulReadSize = ulUnitSize*ulReadUnit;
	hError = ERR_OK;

EXIT_DSMCCLIB_FUNC :

	return hError;

}


static HERROR		dsmcclib_fwrite(void *pBuffer, HUINT32 ulUnitSize, HUINT32 ulUnitNum, HFILEHANDLE hFileHandle, HUINT32 *pulWriteSize)
{
	HERROR			hError = ERR_FAIL;
	HUINT32			ulWriteNum = 0;

	if(pBuffer == NULL || hFileHandle == NULL || pulWriteSize == NULL)
	{
		HxLOG_Print("[dsmcclib_fwrite] Param NULL!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	*pulWriteSize = 0;

	ulWriteNum = (HUINT32)fwrite((void *)pBuffer, (size_t)ulUnitSize, (size_t)ulUnitNum, (FILE*)hFileHandle);
	if(ulWriteNum == 0)
	{
		HxLOG_Print("[dsmcclib_fwrite] writing fail 1!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}
	else if(ulWriteNum != ulUnitNum)
	{
		HxLOG_Print("[dsmcclib_fwrite] writing fail 2!!\n");
	}

	*pulWriteSize = ulUnitSize*ulWriteNum;
	hError = ERR_OK;

EXIT_DSMCCLIB_FUNC :

	return hError;

}

static HERROR		dsmcclib_fclose(HFILEHANDLE hFileHandle)
{
	HERROR			hError = ERR_FAIL;

	if( hFileHandle == NULL)
	{
		HxLOG_Print("[dsmcclib_fclose] Param NULL!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	fclose((FILE*)hFileHandle);

	hError = ERR_OK;

EXIT_DSMCCLIB_FUNC :

	return hError;
}


static HERROR		dsmcclib_filelength(HFILEHANDLE hFileHandle, HUINT32 *pulFileLength)
{
	HERROR			hError = ERR_FAIL;
	HINT32			ulRet = 0;
	HINT32			ulLength = 0;

	if( hFileHandle == NULL || pulFileLength == NULL)
	{
		HxLOG_Print("[dsmcclib_filelength] Param NULL!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	*pulFileLength = 0;

	ulRet = (HINT32)fseek((FILE*)hFileHandle, 0, SEEK_END);
	if(ulRet != 0)
	{
		HxLOG_Print("[dsmcclib_filelength] fseek SEEK_END fail!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	ulLength = (HINT32)ftell((FILE*)hFileHandle);
	if(ulLength == -1)
	{
		HxLOG_Print("[dsmcclib_filelength] ftell fail!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	ulRet = (HINT32)fseek((FILE*)hFileHandle, 0, SEEK_SET);
	if(ulRet != 0)
	{
		HxLOG_Print("[dsmcclib_filelength] fseek SEEK_SET fail!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	*pulFileLength = ulLength;
	hError = ERR_OK;

EXIT_DSMCCLIB_FUNC :

	return hError;

}

#define	______TEMP_FILEIO_END______
#endif



#define	______LOCAL_FUNC_START______


static HERROR			dsmcclib_InitResource(void)
{
	HERROR				hError = ERR_FAIL;

	hError = VK_SEM_Create((unsigned long*)&s_stDsmccInfo.ulSemId, "dsmcc_sem", VK_SUSPENDTYPE_PRIORITY);
	if(hError != VK_OK)
	{
		HxLOG_Print("[dsmcclib_InitResource] VK_SEM_Create fail!!\n");
		return hError;
	}

	hError = ERR_OK;

	return hError;
}

static HERROR			dsmcclib_GetDsmccSessionInfo(HUINT32 *pulTransactionId, HUINT32 *pulDsiCrc, HUINT32 *pulRegisteredTime)
{
	HERROR				hError = ERR_FAIL;

	VK_SEM_Get(s_stDsmccInfo.ulSemId);

	if(pulTransactionId == NULL || pulDsiCrc == NULL || pulRegisteredTime == NULL)
	{
		HxLOG_Print("[dsmcclib_GetDsmccSessionInfo] param fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	if(!s_stDsmccInfo.bVaild)
	{
		HxLOG_Print("[dsmcclib_GetDsmccSessionInfo] invalid!!\n");
		hError = ERR_OK;
		goto EXIT_DSMCC_FUNC;
	}

	*pulTransactionId = s_stDsmccInfo.ulTransactionId;
	*pulRegisteredTime = s_stDsmccInfo.ulRegisteredTime;
	*pulDsiCrc = s_stDsmccInfo.ulDsiCrc;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :
	VK_SEM_Release(s_stDsmccInfo.ulSemId);

	return hError;
}



static HERROR			dsmcclib_SetDsmccSessionInfo(HUINT32 ulTransactionId, HUINT32 ulDsiCrc, HUINT32 ulRegisteredTime)
{
	HERROR				hError = ERR_FAIL;

	VK_SEM_Get(s_stDsmccInfo.ulSemId);

	s_stDsmccInfo.ulTransactionId = ulTransactionId;
	s_stDsmccInfo.ulDsiCrc = ulDsiCrc;
	s_stDsmccInfo.ulRegisteredTime= ulRegisteredTime;
	s_stDsmccInfo.bVaild = TRUE;

	hError = ERR_OK;

	VK_SEM_Release(s_stDsmccInfo.ulSemId);

	return hError;
}


static HERROR			dsmcclib_SetDsmccMode(HUINT32 ulDsmccMode)
{
	HERROR				hError = ERR_FAIL;

	VK_SEM_Get(s_stDsmccInfo.ulSemId);

	s_stDsmccInfo.ulDsmccMode = ulDsmccMode;

	hError = ERR_OK;

	VK_SEM_Release(s_stDsmccInfo.ulSemId);

	return hError;
}


static HERROR			dsmcclib_GetDsmccMode(HUINT32 *pulDsmccMode)
{
	HERROR				hError = ERR_FAIL;

	VK_SEM_Get(s_stDsmccInfo.ulSemId);

	if(pulDsmccMode == NULL)
	{
		HxLOG_Print("[dsmcclib_GetDsmccMode] param fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	*pulDsmccMode = s_stDsmccInfo.ulDsmccType;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	VK_SEM_Release(s_stDsmccInfo.ulSemId);

	return hError;
}


static HERROR			dsmcclib_SetDsmccLayerType(HUINT32 ulLayerType)
{
	HERROR				hError = ERR_FAIL;

	VK_SEM_Get(s_stDsmccInfo.ulSemId);

	s_stDsmccInfo.ulDsmccType = ulLayerType;

	hError = ERR_OK;

	VK_SEM_Release(s_stDsmccInfo.ulSemId);

	return hError;
}


static HERROR			dsmcclib_GetDsmccLayerType(HUINT32 *pulLayerType)
{
	HERROR				hError = ERR_FAIL;

	VK_SEM_Get(s_stDsmccInfo.ulSemId);

	if(pulLayerType == NULL)
	{
		HxLOG_Print("[dsmcclib_GetDsmccLayerType] param fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	*pulLayerType = s_stDsmccInfo.ulDsmccType;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	VK_SEM_Release(s_stDsmccInfo.ulSemId);

	return hError;
}


static HERROR			dsmcclib_GetDownloadSize(HUINT32 *pulSize)
{
	HERROR				hError = ERR_FAIL;
	GROUP_INFO_t		*pTempGroup = NULL;
	HUINT32				ulGroupListSize = 0;

	VK_SEM_Get(s_stDsmccInfo.ulSemId);

	if(pulSize == NULL)
	{
		HxLOG_Print("[dsmcclib_GetGroupListSize] param NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	pTempGroup = s_stDsmccInfo.pGroupInfo;
	while(pTempGroup)
	{
		ulGroupListSize += pTempGroup->lGroupSize;
		pTempGroup = pTempGroup->pNext;
	}

	*pulSize = ulGroupListSize;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :
	VK_SEM_Release(s_stDsmccInfo.ulSemId);

	return hError;}

#define	______BUILD_FUNC______


static HERROR		dsmcclib_BuildDii(void)
{
	HERROR					hError = ERR_FAIL;
	DIIMSG_t				*pTempDiiMsg = NULL;
	DIIMSG_t				*pBuildDiiMsg = NULL;
	GROUP_INFO_t			*pGroupInfo = NULL;
	HBOOL					bExisted = FALSE;
	HBOOL					bBuild = FALSE;

	VK_SEM_Get(s_stDsmccInfo.ulSemId);

	if(s_stDsmccInfo.ulBuildNumOfDii == s_stDsmccInfo.usNumOfGroup)
	{
		HxLOG_Print("[dsmcclib_BuildDii] complete!!\n");
		hError = ERR_OK;
		goto EXIT_DSMCC_FUNC;
	}

	pTempDiiMsg = s_stDsmccInfo.pDiiMsg;
	while(pTempDiiMsg)
	{
		bBuild = bExisted = FALSE;
		pBuildDiiMsg = pTempDiiMsg;
		pGroupInfo = NULL;

		// ETSI EN 301 192 8.1.2 DownloadServerInitiate message
		// group id : Equal to transactionId of the DownloadInfoIndication message
		hError = dsmcclib_FindGroupInfo(pTempDiiMsg->ulTransactionId, s_stDsmccInfo.pGroupInfo, &pGroupInfo);
		if(hError != ERR_OK || pGroupInfo == NULL)
		{
			HxLOG_Print("[dsmcclib_ProcessDii] FindGroupInfo fail ulTransactionId(%x)!!\n", pTempDiiMsg->ulTransactionId);
			goto END_GROUPLIST_LOOP;
		}

		if(pGroupInfo->pDiiMsg != NULL)
		{
			HxLOG_Print("[dsmcclib_ProcessDii] bExisted Dii!!\n");
			bExisted = TRUE;
			goto END_GROUPLIST_LOOP;
		}

		pGroupInfo->pDiiMsg = pBuildDiiMsg;

		s_stDsmccInfo.ulBuildNumOfDii++;

		bBuild = TRUE;

END_GROUPLIST_LOOP:
		pTempDiiMsg = pTempDiiMsg->pNext;
		if(bExisted)
		{
			s_stDsmccInfo.pDiiMsg = dsmcclib_DeleteDiiMsg(s_stDsmccInfo.pDiiMsg, pBuildDiiMsg, TRUE);
		}
		if(bBuild)
		{
			s_stDsmccInfo.pDiiMsg = dsmcclib_DeleteDiiMsg(s_stDsmccInfo.pDiiMsg, pBuildDiiMsg, FALSE);
		}
	}

	hError = ERR_OK;

EXIT_DSMCC_FUNC :
	VK_SEM_Release(s_stDsmccInfo.ulSemId);

	return hError;
}


static HERROR		dsmcclib_BuildDdb(HUINT32 ulLayerType, HUINT8 * pucIsComplete, PalDsmcc_File_t **ppDsmccFile)
{
	HERROR					hError = ERR_FAIL;
	MODULE_INFO_t			*pBuildModule = NULL;
	MODULE_INFO_t			*pTempModule = NULL;
	MODULE_INFO_t			*pFoundModule = NULL;
	DIIMSG_t				*pFoundDii = NULL;
	PalDsmcc_File_t				*pRootDsmccFile = NULL;
	PalDsmcc_File_t				*pDsmccFile = NULL;
	DDBMSG_t				*pTempDdbMsg = NULL;
	HUINT8					ucIsGroupComplete = 0;
	HUINT8					ucIsDSMCCComplete = 0;
	HBOOL					bFound = FALSE;
	HBOOL					bModuleComplete = FALSE;
	HUINT32					ulFileOffset = 0;

	VK_SEM_Get(s_stDsmccInfo.ulSemId);

	if(pucIsComplete == NULL || ppDsmccFile == NULL)
	{
		HxLOG_Print("[dsmcclib_BuildDdb] param NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*pucIsComplete = 0;
	*ppDsmccFile = NULL;

	pTempModule = s_stDsmccInfo.pDdbBufferModule;
	while(pTempModule)
	{
		bFound = bModuleComplete = FALSE;
		pFoundDii = NULL;
		pFoundModule = NULL;
		pDsmccFile = NULL;

		if(pTempModule->ucIsComplete == 1)
		{
			goto END_DDBLIST_LOOP;
		}
		else
		{
			HxLOG_Print("[dsmcclib_BuildDdb] not complete module!!\n");
		}

		if(ulLayerType == ePAL_DSMCC_ONE_LAYER)
		{
			hError = dsmcclib_FindModuleInfo(	ulLayerType,
													pTempModule->ulDownloadId,
													pTempModule->usModuleId,
													NULL,
													s_stDsmccInfo.pDiiMsg,
													&pFoundDii,
													&pFoundModule);
		}
		else if(ulLayerType == ePAL_DSMCC_TWO_LAYER)
		{
			hError = dsmcclib_FindModuleInfo(	ulLayerType,
													pTempModule->ulDownloadId,
													pTempModule->usModuleId,
													s_stDsmccInfo.pGroupInfo,
													NULL,
													&pFoundDii,
													&pFoundModule);
		}

		if(hError != ERR_OK || pFoundModule == NULL)
		{
			HxLOG_Print("[dsmcclib_BuildDdb] FindModuleInfo fail!!\n");
			goto END_DDBLIST_LOOP;
		}
		if(pFoundModule->ucIsComplete)
		{
			goto END_DDBLIST_LOOP;
		}
		bFound = TRUE;

END_DDBLIST_LOOP:
		pBuildModule = pTempModule;
		pTempModule = pTempModule->pNext;
		if(bFound == TRUE)
		{
			hError = dsmcclib_CheckModuleBuild(pBuildModule, pFoundModule->ulModuleSize, &bModuleComplete);
			if(hError != ERR_OK)
			{
				HxLOG_Print("[dsmcclib_BuildDdb] dsmcclib_CheckModuleBuild!!\n");
				goto EXIT_DSMCC_FUNC;
			}
			if(bModuleComplete == TRUE)
			{
				pFoundModule->ucIsComplete = 1;
				pBuildModule->ucIsComplete = 1;
			}
			else
			{
				continue;
			}

///////////////////////////////////////////////////////////
// sort ddb data
///////////////////////////////////////////////////////////
			hError = dsmcclib_SortDdbListbyNumber(pBuildModule->pDdb);
			if(hError != ERR_OK)
			{
				HxLOG_Print("[dsmcclib_BuildDdb] SortDdbListbyNumber fail!!\n");
				continue;
			}

///////////////////////////////////////////////////////////
// create file
///////////////////////////////////////////////////////////
			pDsmccFile = (PalDsmcc_File_t*)OxDSMCC_Malloc(sizeof(PalDsmcc_File_t));
			if(pDsmccFile == NULL)
			{
				HxLOG_Print("[dsmcclib_BuildDdb] pDsmccFile alloc fail!!\n");
				continue;
			}
			VK_memset32(pDsmccFile, 0x00, sizeof(PalDsmcc_File_t));

			pDsmccFile->pucFilePointer = (HUINT8*)OxDSMCC_Malloc(sizeof(HUINT8)*pFoundModule->ulModuleSize);
			if(pDsmccFile->pucFilePointer == NULL)
			{
				OxDSMCC_Free(pDsmccFile);
				pDsmccFile = NULL;

				HxLOG_Print("[dsmcclib_BuildDdb] pDsmccFile alloc fail!!\n");
				continue;
			}

			pTempDdbMsg = pBuildModule->pDdb;
			ulFileOffset = 0;
			while(pTempDdbMsg)
			{
				VK_memcpy32(&(pDsmccFile->pucFilePointer[ulFileOffset]), pTempDdbMsg->pucData, (sizeof(HUINT8))*(pTempDdbMsg->ulDataSize));
				ulFileOffset += pTempDdbMsg->ulDataSize;
				pTempDdbMsg = pTempDdbMsg->pNext;
			}
			pDsmccFile->ulModuleSize =  pFoundModule->ulModuleSize;
			pDsmccFile->pucModuleName =  pFoundModule->szModuleName;
			pFoundModule->szModuleName = NULL;

			dsmcclib_FreeDdbMsg(pBuildModule->pDdb);
			pBuildModule->pDdb = NULL;

///////////////////////////////////////////////////////////
// link file
///////////////////////////////////////////////////////////
			if(pRootDsmccFile == NULL)
			{
				pRootDsmccFile = pDsmccFile;
			}
			else
			{
				pDsmccFile->pNext = pRootDsmccFile;
				pRootDsmccFile = pDsmccFile;
			}

///////////////////////////////////////////////////////////
// check dsmcc full complete
///////////////////////////////////////////////////////////
			pFoundModule->ucIsComplete = 1;
			hError = dsmcclib_CheckCompleteDii(pFoundDii, &ucIsGroupComplete);
			if(hError != ERR_OK || ucIsGroupComplete == 0)
			{
				HxLOG_Print("[dsmcclib_BuildDdb] CheckCompleteGroup fail!!\n");
				continue;
			}
			if(ulLayerType == ePAL_DSMCC_TWO_LAYER)
			{
				hError = dsmcclib_CheckCompleteDSMCC(	s_stDsmccInfo.pGroupInfo,
															s_stDsmccInfo.usNumOfGroup,
															&ucIsDSMCCComplete);
				if(hError != ERR_OK || ucIsDSMCCComplete == 0)
				{
					HxLOG_Print("[dsmcclib_BuildDdb] CheckCompleteDSMCC fail!!\n");
					continue;
				}
				if(ucIsDSMCCComplete)
				{
					*pucIsComplete = 1;
				}
			}
			else
			{
				*pucIsComplete = 1;
			}
		}

	}

	*ppDsmccFile = pRootDsmccFile;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :
	VK_SEM_Release(s_stDsmccInfo.ulSemId);

	return hError;
}




static HERROR		dsmcclib_SortDdbListbyNumber(DDBMSG_t *pDdbMsg)
{
	HERROR					hError = ERR_FAIL;
	DDBMSG_t				*pFrontItem = NULL;
	DDBMSG_t				*pTempItem = NULL;
	DDBMSG_t				stBufferItem = {0, };

	if(pDdbMsg == NULL)
	{
		HxLOG_Print("[dsmcclib_SortDdbListbyNumber] param NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	// bubble sort
	pFrontItem = pDdbMsg;
	while(pFrontItem)
	{
		pTempItem = pFrontItem->pNext;
		while(pTempItem)
		{
			if(pFrontItem->usBlockNumber > pTempItem->usBlockNumber)
			{
				// swap
				VK_memcpy32( &stBufferItem, pFrontItem, sizeof(DDBMSG_t));

				pFrontItem->ulDownloadId = pTempItem->ulDownloadId;
				pFrontItem->ucModuleVer = pTempItem->ucModuleVer;
				pFrontItem->usModuleId = pTempItem->usModuleId;
				pFrontItem->usBlockNumber = pTempItem->usBlockNumber;
				pFrontItem->ulDataSize = pTempItem->ulDataSize;
				pFrontItem->pucData = pTempItem->pucData;

				pTempItem->ulDownloadId = stBufferItem.ulDownloadId;
				pTempItem->ucModuleVer = stBufferItem.ucModuleVer;
				pTempItem->usModuleId = stBufferItem.usModuleId;
				pTempItem->usBlockNumber = stBufferItem.usBlockNumber;
				pTempItem->ulDataSize = stBufferItem.ulDataSize;
				pTempItem->pucData = stBufferItem.pucData;

				VK_memset32( &stBufferItem, 0x00, sizeof(DDBMSG_t));
			}
			pTempItem = pTempItem->pNext;
		}
		pFrontItem = pFrontItem->pNext;
	}

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	return hError;

}






#define	______CHECK_FUNC______
static HERROR		dsmcclib_CheckDiiBuild(HUINT8 *pucIsComplete)
{
	HERROR					hError = ERR_FAIL;

	VK_SEM_Get(s_stDsmccInfo.ulSemId);
	if(pucIsComplete == NULL)
	{
		HxLOG_Print("[dsmcclib_CheckDiiBuild] param fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*pucIsComplete = FALSE;

	if(s_stDsmccInfo.ulDsmccType == ePAL_DSMCC_TWO_LAYER)
	{
		if(	s_stDsmccInfo.usNumOfGroup == s_stDsmccInfo.ulBuildNumOfDii
			&& s_stDsmccInfo.usNumOfGroup != 0 )
		{
			HxLOG_Info("[dsmcclib_CheckDiiBuild] s_stDsmccInfo.usNumOfGroup == s_stDsmccInfo.ulBuildNumOfDii(%d)!!\n", s_stDsmccInfo.usNumOfGroup);
			*pucIsComplete = TRUE;
		}
	}

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	VK_SEM_Release(s_stDsmccInfo.ulSemId);

	return hError;
}



static HERROR		dsmcclib_CheckCompleteDii(DIIMSG_t *pDiiMsg, HUINT8 *pucIsComplete)
{
	HERROR					hError = ERR_FAIL;
	DIIMSG_t				*pTempDii = NULL;
	MODULE_INFO_t			*pTempModule = NULL;
	HUINT16					usNumOfModule = 0;

	if(pDiiMsg == NULL || pucIsComplete == NULL)
	{
		HxLOG_Print("[dsmcclib_CheckCompleteGroup] param NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*pucIsComplete = 0;

	pTempDii = pDiiMsg;

	pTempModule = pTempDii->pModuleInfo;
	while(pTempModule)
	{
		if(!pTempModule->ucIsComplete)
		{
			HxLOG_Print("[dsmcclib_CheckCompleteGroup] pTempModule not complete!!\n");
			break;
		}
		usNumOfModule++;
		pTempModule = pTempModule->pNext;
	}

	if(pTempDii->usNumOfModule == usNumOfModule)
	{
		pTempDii->ucIsComplete = 1;
		*pucIsComplete = 1;
	}
	else
	{
		HxLOG_Print("[dsmcclib_CheckCompleteGroup] pTempModule not complete!!\n");
	}

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	return hError;

}



static HERROR		dsmcclib_CheckCompleteDSMCC(GROUP_INFO_t *pRootGroup, HUINT16 usTotalNumOfGroup, HUINT8 *pucIsComplete)
{
	HERROR					hError = ERR_FAIL;
	GROUP_INFO_t			*pTempGroup = NULL;
	DIIMSG_t				*pTempDii = NULL;
	HUINT16					usNumOfGroup = 0;

	if(pRootGroup == NULL || pucIsComplete == NULL)
	{
		HxLOG_Print("[dsmcclib_CheckCompleteDSMCC] param NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*pucIsComplete = 0;

	pTempGroup = pRootGroup;
	while(pTempGroup)
	{
		pTempDii = pTempGroup->pDiiMsg;
		if(pTempDii == NULL)
		{
			HxLOG_Print("[dsmcclib_CheckCompleteDSMCC] pTempGroup not complete!!\n");
			break;
		}
		if(!pTempDii->ucIsComplete)
		{
			HxLOG_Print("[dsmcclib_CheckCompleteDSMCC] pTempGroup not complete!!\n");
			break;
		}
		usNumOfGroup++;
		pTempGroup = pTempGroup->pNext;
	}

	if(usTotalNumOfGroup == usNumOfGroup)
	{
		*pucIsComplete = 1;
	}

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	return hError;


}

static HERROR		dsmcclib_CheckModuleBuild(MODULE_INFO_t *pModuleInfo, HUINT32 ulModuleSize, HBOOL *pbComplete)
{
	HERROR					hError = ERR_FAIL;
	DDBMSG_t				*pTemp = NULL;
	HUINT32					ulTotalDdbSize = 0;

	if(pModuleInfo == NULL || pbComplete == NULL)
	{
		HxLOG_Print("[dsmcclib_CheckModuleBuild] param fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*pbComplete = FALSE;

	pTemp = pModuleInfo->pDdb;
	while(pTemp)
	{
		ulTotalDdbSize += pTemp->ulDataSize;
		pTemp = pTemp->pNext;
	}

	if(ulTotalDdbSize == ulModuleSize)
	{
		HxLOG_Print("[dsmcclib_CheckModuleBuild] Complete!!\n");

		*pbComplete = TRUE;
	}
	else if(ulTotalDdbSize > ulModuleSize)
	{
		HxLOG_Print("[dsmcclib_CheckModuleBuild] ulTotalDdbSize > ulModuleSize!!\n");

//		dsmcclib_FreeDdbMsg(pModuleInfo->pDdb);
//		pModuleInfo->pDdb = NULL;
	}
	else if(ulTotalDdbSize < ulModuleSize)
	{
		HxLOG_Print("[dsmcclib_CheckModuleBuild] ulTotalDdbSize < ulModuleSize!!\n");
	}

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	return hError;


}



#define	______ADD_FUNC______
static HERROR			dsmcclib_AddGroupInfoList(HUINT16 usNumOfGroup, GROUP_INFO_t *pGroupList)
{
	HERROR				hError = ERR_FAIL;
	GROUP_INFO_t		*pTempGroup = NULL;
	GROUP_INFO_t		*pAppendGroup = NULL;
	GROUP_INFO_t		*pGroupItem = NULL;
	GROUP_INFO_t		*pDelRoot = NULL;
	HBOOL				bExisted = FALSE;

	VK_SEM_Get(s_stDsmccInfo.ulSemId);

	if(pGroupList == NULL)
	{
		HxLOG_Print("[dsmcclib_AddGroupInfoList] param NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	if(s_stDsmccInfo.pGroupInfo == NULL)
	{
		s_stDsmccInfo.usNumOfGroup = usNumOfGroup;
		s_stDsmccInfo.pGroupInfo = pGroupList;
	}
	else
	{
		pTempGroup = pGroupList;
		while(pTempGroup)
		{
			pGroupItem = s_stDsmccInfo.pGroupInfo;
			while(pGroupItem)
			{
				if(pGroupItem->ulGroupId == pTempGroup->ulGroupId)
				{
					bExisted = TRUE;
					break;
				}
				pGroupItem = pGroupItem->pNext;
			}
			pAppendGroup = pTempGroup;
			pTempGroup = pTempGroup->pNext;
			if(bExisted)
			{
				pDelRoot = dsmcclib_AppendGroupInfo(pDelRoot, pAppendGroup);
			}
			else
			{
				s_stDsmccInfo.pGroupInfo = dsmcclib_AppendGroupInfo(s_stDsmccInfo.pGroupInfo, pAppendGroup);
				s_stDsmccInfo.usNumOfGroup++;
			}
		}
	}

	dsmcclib_FreeGroupInfo(pDelRoot);
	pDelRoot = NULL;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :
	VK_SEM_Release(s_stDsmccInfo.ulSemId);

	return hError;
}



static HERROR			dsmcclib_AddDiiMsgList(DIIMSG_t *pNewDiiMsg)
{
	HERROR				hError = ERR_FAIL;
	DIIMSG_t			*pTempDii = NULL;
	HBOOL				bExisted = FALSE;

	VK_SEM_Get(s_stDsmccInfo.ulSemId);

	if(pNewDiiMsg == NULL)
	{
		HxLOG_Print("[dsmcclib_AddDiiList] param fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	pTempDii = s_stDsmccInfo.pDiiMsg;
	while(pTempDii)
	{
		if(	pNewDiiMsg->ulDownloadId == pTempDii->ulDownloadId
			&& pNewDiiMsg->ulTransactionId == pTempDii->ulTransactionId
			&& pNewDiiMsg->usBlockSize == pTempDii->usBlockSize
			&& pNewDiiMsg->usNumOfModule == pTempDii->usNumOfModule
			&& pNewDiiMsg->ulCrc == pTempDii->ulCrc
		)
		{
			bExisted = TRUE;
			break;
		}
		pTempDii = pTempDii->pNext;
	}

	if(bExisted)
	{
		HxLOG_Print("[dsmcclib_AddDiiMsgList] bExisted!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	else
	{
		s_stDsmccInfo.pDiiMsg = dsmcclib_AppendDiiMsg(s_stDsmccInfo.pDiiMsg, pNewDiiMsg);
	}

	hError = ERR_OK;

EXIT_DSMCC_FUNC :
	VK_SEM_Release(s_stDsmccInfo.ulSemId);

	return hError;
}



static HERROR			dsmcclib_AddDdbMsgList(DDBMSG_t *pNewDdbMsg)
{
	HERROR				hError = ERR_FAIL;
	MODULE_INFO_t		*pTempDdbModule = NULL;
	MODULE_INFO_t		*pNewModule = NULL;
	DDBMSG_t			*pTempDdb = NULL;
	HBOOL				bExistedDdbList = FALSE;
	HBOOL				bExistedDdbMsg = FALSE;


	VK_SEM_Get(s_stDsmccInfo.ulSemId);

	if(pNewDdbMsg == NULL)
	{
		HxLOG_Error("[dsmcclib_AddDdbMsgList] param fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	pTempDdbModule = s_stDsmccInfo.pDdbBufferModule;
	while(pTempDdbModule)
	{
		if(pTempDdbModule->usModuleId == pNewDdbMsg->usModuleId)
		{
			bExistedDdbList = TRUE;
			break;
		}
		pTempDdbModule = pTempDdbModule->pNext;
	}

	if(bExistedDdbList)
	{
		if(pTempDdbModule->ucIsComplete)
		{
			HxLOG_Info("[dsmcclib_AddDdbMsgList] Complete!!\n");
			goto EXIT_DSMCC_FUNC;
		}
		pTempDdb = pTempDdbModule->pDdb;
		while(pTempDdb)
		{
			if(pTempDdb->usBlockNumber == pNewDdbMsg->usBlockNumber)
			{
				bExistedDdbMsg = TRUE;
				break;
			}
			pTempDdb = pTempDdb->pNext;
		}

		if(bExistedDdbMsg)
		{
			HxLOG_Info("[dsmcclib_AddDdbMsgList] bExistedDdbMsg!!\n");
			dsmcclib_FreeDdbMsg(pNewDdbMsg);
			hError = ERR_OK;
			goto EXIT_DSMCC_FUNC;
		}
		else
		{
			pTempDdbModule->pDdb =dsmcclib_AppendDdbMsg(pTempDdbModule->pDdb, pNewDdbMsg);
		}
	}
	else
	{
		pNewModule = NULL;
		pNewModule = dsmcclib_AllocModuleInfo();
		if(pNewModule == NULL)
		{
			HxLOG_Error("[dsmcclib_AddDdbMsgList] dsmcclib_AllocModuleInfo fail!!\n");
			goto EXIT_DSMCC_FUNC;
		}
		pNewModule->usModuleId = pNewDdbMsg->usModuleId;
		pNewModule->ulDownloadId = pNewDdbMsg->ulDownloadId;
		pNewModule->ucModuleVer = pNewDdbMsg->ucModuleVer;

		pNewModule->pDdb =dsmcclib_AppendDdbMsg(pNewModule->pDdb, pNewDdbMsg);
		s_stDsmccInfo.pDdbBufferModule = dsmcclib_AppendModuleInfo(s_stDsmccInfo.pDdbBufferModule, pNewModule);
	}

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	VK_SEM_Release(s_stDsmccInfo.ulSemId);

	return hError;
}





#define	______FIND_FUNC______
static HERROR		dsmcclib_FindGroupInfo(HUINT32 ulGroupId, GROUP_INFO_t *pGroupList, GROUP_INFO_t **ppGroupInfo)
{
	HERROR					hError = ERR_FAIL;
	GROUP_INFO_t			*pTempGroupInfo = NULL;

	if(ppGroupInfo == NULL || pGroupList == NULL)
	{
		HxLOG_Print("[dsmcclib_FindGroupInfo] param NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*ppGroupInfo = NULL;

	pTempGroupInfo = pGroupList;
	while(pTempGroupInfo)
	{
		if(pTempGroupInfo->ulGroupId == ulGroupId)
		{
			break;
		}
		pTempGroupInfo = pTempGroupInfo->pNext;
	}

	if(pTempGroupInfo == NULL)
	{
		HxLOG_Print("[dsmcclib_FindGroupInfo] pTempGroupInfo NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*ppGroupInfo = pTempGroupInfo;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	return hError;


}



static HERROR		dsmcclib_FindModuleInfo(	HUINT32 ulLayerType,
													HUINT32 ulDownloadId,
													HUINT16 usModuleId,
													GROUP_INFO_t *pRootGroup,
													DIIMSG_t *pRootDii,
													DIIMSG_t ** ppDiiMsg,
													MODULE_INFO_t **ppModuleInfo)
{
	HERROR					hError = ERR_FAIL;
	GROUP_INFO_t			*pTempGroup = NULL;
	DIIMSG_t				*pTempDii = NULL;
	MODULE_INFO_t			*pTempModule = NULL;

	if(ppDiiMsg == NULL ||ppModuleInfo == NULL)
	{
		HxLOG_Print("[dsmcclib_FindModuleInfo] param fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*ppDiiMsg = NULL;
	*ppModuleInfo = NULL;

	switch(ulLayerType)
	{
		case ePAL_DSMCC_TWO_LAYER:
			if(pRootGroup == NULL)
			{
				HxLOG_Print("[dsmcclib_FindModuleInfo] pRootGroup fail!!\n");
				goto EXIT_DSMCC_FUNC;
			}

			pTempGroup = pRootGroup;
			while(pTempGroup)
			{
				pTempDii = pTempGroup->pDiiMsg;
				if(pTempDii == NULL)
				{
					pTempGroup = pTempGroup->pNext;
					continue;
				}

				if(pTempDii->ucIsComplete == 1)
				{
					pTempGroup = pTempGroup->pNext;
					continue;
				}

				if(pTempDii->ulDownloadId != ulDownloadId)
				{
					pTempGroup = pTempGroup->pNext;
					continue;
				}

				pTempModule = pTempDii->pModuleInfo;
				while(pTempModule)
				{
					if(pTempModule->usModuleId == usModuleId)
					{
						if(pTempModule->ucIsComplete)
						{
							HxLOG_Print("[dsmcclib_FindModuleInfo] pTempModule complete!!\n");
						}
						else
						{
							*ppModuleInfo = pTempModule;
							*ppDiiMsg = pTempDii;
						}
						hError = ERR_OK;
						goto EXIT_DSMCC_FUNC;
					}
					pTempModule = pTempModule->pNext;
				}
				pTempGroup = pTempGroup->pNext;
			}
			break;

		case ePAL_DSMCC_ONE_LAYER:
			if(pRootDii == NULL)
			{
				HxLOG_Print("[dsmcclib_FindModuleInfo] pRootDii fail!!\n");
				goto EXIT_DSMCC_FUNC;
			}

			pTempDii = pRootDii;
			while(pTempDii)
			{
				if(pTempDii->ulDownloadId != ulDownloadId)
				{
					pTempDii = pTempDii->pNext;
					continue;
				}

				pTempModule = pTempDii->pModuleInfo;
				while(pTempModule)
				{
					if(pTempModule->usModuleId == usModuleId)
					{
						if(pTempModule->ucIsComplete)
						{
							HxLOG_Print("[dsmcclib_FindModuleInfo] pTempModule complete!!\n");
						}
						else
						{
							*ppModuleInfo = pTempModule;
							*ppDiiMsg = pTempDii;
						}
						hError = ERR_OK;
						goto EXIT_DSMCC_FUNC;
					}
					pTempModule = pTempModule->pNext;
				}

				pTempDii = pTempDii->pNext;
			}
			break;

	}

EXIT_DSMCC_FUNC :

	return hError;
}




#define	______PARSING_FUNC______


static HERROR			dsmcclib_ParseDsmccSectionInfo(HUINT8 *pRawData, HINT16 *psSectionLen, HUINT32 *pulCrc, HUINT8 *pucVerNum)
{
	HUINT8			ucTableId = 0;
	HUINT8			ucSecSyntaxIndicator = 0;
	HUINT8			ucPrivateIndicator = 0;
	HUINT16 			sSecLen = 0;
	HUINT8			ucSecNum = 0;
	HUINT32			ulCrc = 0;
	HUINT8			ucTemp = 0;
	HUINT8			ucLastSecNum = 0;
	HUINT8			ucVerNum = 0;
	HUINT8			ucCurNextIndicator = 0;
	HUINT16			usTableIdExtension = 0;
	HERROR			hError = ERR_FAIL;


	if(pRawData == NULL || psSectionLen == NULL || pulCrc == NULL || pucVerNum == NULL)
	{
		HxLOG_Print("[ParseDsmccSectionInfo] param NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*pulCrc = 0;
	*pucVerNum = 0;
	*psSectionLen = 0;

	// table id (1byte)
	ucTableId = *pRawData;
	pRawData += DSMCC_TABLEID;
	HxLOG_Print("[ParseDsmccSectionInfo] ucTableId = 0x%02X\n", ucTableId);

	// section syntax indicator, private_indicator, section length (2byte)
	ucSecSyntaxIndicator = (HUINT8)((*pRawData & 0x80) >> 7);
	ucPrivateIndicator = (HUINT8)((*pRawData & 0x40) >> 6);
	sSecLen = (HINT16)get12bit(pRawData);
	pRawData += DSMCC_SECTION_ETCINFO;

	HxLOG_Print("[ParseDsmccSectionInfo] ucTableId = 0x%02X\n", ucSecSyntaxIndicator);
	HxLOG_Print("[ParseDsmccSectionInfo] private_indicator = 0x%02X\n", ucPrivateIndicator);
	HxLOG_Print("[ParseDsmccSectionInfo] section length = %d (0x%03X)\n", sSecLen, sSecLen);

	if (sSecLen > SECTION_PRIVATE_MAXLEN)
	{
		HxLOG_Print("[ParseDsmccSectionInfo] SECTION_PRIVATE_MAXLEN overflow!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	ulCrc = (HUINT32)get32bit(pRawData + sSecLen - DSMCC_CRC);

	// table_id_extension (2byte)
	usTableIdExtension = get16bit(pRawData);
	pRawData += DSMCC_TABLEID_EXT;
	HxLOG_Print("[ParseDsmccSectionInfo] table_id_extension = 0x%04X\n", usTableIdExtension);

	// version number & current next indicator (1byte)
	ucTemp = *pRawData++;
	ucVerNum = (HUINT8)((ucTemp & 0x3e) >> 1);
	ucCurNextIndicator = ucTemp & 0x01;
	HxLOG_Print("[ParseDsmccSectionInfo] version number = %d\n", ucVerNum);
	HxLOG_Print("[ParseDsmccSectionInfo] ucCurNextIndicator = %d\n", ucCurNextIndicator);

	// section number (1byte)
	ucSecNum = *pRawData;
	pRawData += DSMCC_SECTIONNUMBER;
	HxLOG_Print("[ParseDsmccSectionInfo] section number = 0x%x\n", ucSecNum);

	// last section number (1byte)
	ucLastSecNum = *pRawData;
	pRawData += DSMCC_LASTSECTIONNUMBER;
	HxLOG_Print("[ParseDsmccSectionInfo] last section number = 0x%x\n", ucLastSecNum);

	*pulCrc = ulCrc;
	*pucVerNum = ucVerNum;
	*psSectionLen = sSecLen;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	return hError;

}




static HERROR			dsmcclib_ParseDsi(HUINT8 *pRawData, HINT16 sSectionLen, HUINT16 *pusNumOfGroup, GROUP_INFO_t **ppGroupList)
{
	HUINT16				usGroupNum = 0;
	HUINT16				usNumOfGroup = 0;
	HINT32				lLen = 0;
	HINT16				sGroupSize = 0;
	HINT16				sPrivateDataLen = 0;

	GROUP_INFO_t		*pGroupList = NULL;
	HERROR				hError = ERR_FAIL;


	if(pRawData == NULL)
	{
		HxLOG_Print("[dsmcclib_ParseDSI] pRawData NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	// skip server Id
	pRawData += DSMCC_SERVERID;
	sSectionLen -= DSMCC_SERVERID;

	// skip compatibilityDescriptor --> 0x0000;
	pRawData += DSMCC_CMPTDES;
	sSectionLen -= DSMCC_CMPTDES;

	// private_data_length (2byte)
	sPrivateDataLen = get16bit(pRawData);
	pRawData += DSMCC_PRIVATEDATALEN;
	sSectionLen -= DSMCC_PRIVATEDATALEN;
	HxLOG_Print("[dsmcclib_ParseDSI] private_data_length = %d (0x%04X)!!\n", sPrivateDataLen, sPrivateDataLen);

	// set dsi body
	if (sSectionLen <= 0)
	{
		HxLOG_Print("[dsmcclib_ParseDSI] sSectionLen short!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	/************************************************************************/
	/*																		*/
	/* GroupInfoIndication()												*/
	/*																		*/
	/************************************************************************/
	if(sPrivateDataLen == 0)
	{
		HxLOG_Print("[dsmcclib_ParseDSI] sPrivateDataLen short 1!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	if(sPrivateDataLen > sSectionLen)
	{
		HxLOG_Print("[dsmcclib_ParseDSI] sPrivateDataLen short 2!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	sGroupSize = sPrivateDataLen;

	// Number Of Groups (2byte)
	usNumOfGroup = usGroupNum = get16bit(pRawData);
	pRawData += DSMCC_NUMOFGROUP;
	sSectionLen -= DSMCC_NUMOFGROUP;
	sGroupSize -= DSMCC_NUMOFGROUP;
	HxLOG_Print("[dsmcclib_ParseDSI] Number Of Groups = %d (0x%04X)\n", usGroupNum, usGroupNum);

	if(usGroupNum > 0 && sGroupSize >= 14)
	{
		hError = dsmcclib_ParseGroupInfo(pRawData, sGroupSize, &pGroupList, &lLen);
		if(hError != ERR_OK)
		{
			HxLOG_Print("[dsmcclib_ParseDSI] ParseGroupInfo fail!!\n");
			goto EXIT_DSMCC_FUNC;
		}

		pRawData += lLen;
		sSectionLen -= (HINT16)lLen;
		sGroupSize -= (HINT16)lLen;
	}

	// PrivateDataLength (2byte)
	sPrivateDataLen = get16bit(pRawData);
	pRawData += DSMCC_PRIVATEDATALEN;
	sSectionLen -= DSMCC_PRIVATEDATALEN;
	HxLOG_Print("[dsmcclib_ParseDSI] PrivateDataLength = 0x%04X\n", sPrivateDataLen);

	// PrivateDataByte
	pRawData += sPrivateDataLen;
	sSectionLen -= sPrivateDataLen;


	*ppGroupList = pGroupList;
	*pusNumOfGroup = usNumOfGroup;
	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	return hError;
}





static HERROR			dsmcclib_ParseDii(HUINT8 *pRawData, HINT16 sSectionLen, HUINT32 ulTransactionId, DIIMSG_t **ppDiiMsg)
{
	HUINT32					ulDownloadId = 0;
	HUINT32					ulDownloadWindow = 0;
	HUINT32					ulDownloadScenario = 0;
	HUINT32					ulModuleSize = 0;
	HUINT16					usBlockSize = 0;
	HUINT16					usNumOfModule = 0;
	HUINT16					usCountOfModule = 0;
	HUINT16					usModuleId = 0;
	HINT16					sCmptDesLen = 0;
	HUINT8					ucWindowSize = 0;
	HUINT8					ucAckPeriod = 0;
	HUINT8					ucModuleVer = 0;
	HINT8					cModuleInfoLen = 0;
	DIIMSG_t				*pNewDiiMsg= NULL;
	MODULE_INFO_t			*pRootModule = NULL;
	MODULE_INFO_t			*pNewModuleInfo = NULL;
	HUINT16					usPrivateDataLen = 0;
	HERROR					hError = ERR_FAIL;
	HUINT8					ucDesTag = 0;
	HINT8					cDesLen = 0;
	HUINT8					*szModuleName = NULL;


	if(pRawData == NULL)
	{
		HxLOG_Print("[dsmcclib_ParseDii] pRawData NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	// Download ID
	ulDownloadId = get32bit(pRawData);
	pRawData += DSMCC_DII_DOWNLOADID;
	sSectionLen -= DSMCC_DII_DOWNLOADID;
	HxLOG_Print("[dsmcclib_ParseDii] Download ID = 0x%X\n", ulDownloadId);

	// Block Size
	usBlockSize = get16bit(pRawData);
	pRawData += DSMCC_DII_BLOCKSIZE;
	sSectionLen -= DSMCC_DII_BLOCKSIZE;
	HxLOG_Print("[dsmcclib_ParseDii] Block size = %d\n", usBlockSize);

	// Window Size
	ucWindowSize = *pRawData;
	pRawData += DSMCC_DII_WINDOWSIZE;
	sSectionLen -= DSMCC_DII_WINDOWSIZE;
	HxLOG_Print("[dsmcclib_ParseDii] Window size = %d\n", ucWindowSize);

	// Ack Period
	ucAckPeriod = *pRawData;
	pRawData += DSMCC_DII_ACKPERIOD;
	sSectionLen -= DSMCC_DII_ACKPERIOD;
	HxLOG_Print("[dsmcclib_ParseDii] ACK Period = %d\n", ucAckPeriod);

	// Timeout Download Window
	ulDownloadWindow = get32bit(pRawData);
	pRawData += DSMCC_DII_DOWNLOADWINDOW;
	sSectionLen -= DSMCC_DII_DOWNLOADWINDOW;
	HxLOG_Print("[dsmcclib_ParseDii] Timeout download window %d\n", ulDownloadWindow);

	// Timeout Download Scenario
	ulDownloadScenario = get32bit(pRawData);
	pRawData += DSMCC_DII_DOWNLOADSCENARIO;
	sSectionLen -= DSMCC_DII_DOWNLOADSCENARIO;
	HxLOG_Print("[dsmcclib_ParseDii] Timeout download scenario = %d\n", ulDownloadScenario);

	// Compatibility Descriptor
	sCmptDesLen = get16bit(pRawData);
#if	defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
{
	HUINT16					usDesCount = 0;
	CMPTDES_t				*pCmptDes = NULL;

	HxLOG_Print("[dsmcclib_ParseDii] Compatibility Descriptor Length = %d (0x%04X)\n", sCmptDesLen, sCmptDesLen);
 	if (sCmptDesLen)
 	{
		usDesCount = (HUINT16)(get32bit(pRawData) & 0x0000ffff);
		HxLOG_Print("[dsmcclib_ParseDii] Descriptor Count = %d\n", usDesCount);

		pCmptDes = dsmcclib_AllocCmptDes();
		if (pCmptDes == NULL)
		{
			HxLOG_Print("[dsmcclib_ParseDii] dsmcclib_AllocCmptDes fail");
			goto EXIT_DSMCC_FUNC;
		}

		hError = dsmcclib_ParseCmptDes(pRawData, pCmptDes);
		if (hError != ERR_OK)
		{
			HxLOG_Print("[dsmcclib_ParseDii] dsmcclib_ParseCmptDes fail");

			dsmcclib_FreeCmptDes(pCmptDes);
			pCmptDes = NULL;
			goto EXIT_DSMCC_FUNC;
		}

		pNewDiiMsg->pCmptDes = pCmptDes;
 	}
}
#endif

	pRawData += (sCmptDesLen+DSMCC_CMPTDESLENGTH);
	sSectionLen -= (sCmptDesLen+DSMCC_CMPTDESLENGTH);

	// Number Of Module
	usNumOfModule = get16bit(pRawData);
	pRawData += DSMCC_DII_NUMOFMODULE;
	sSectionLen -= DSMCC_DII_NUMOFMODULE;
	HxLOG_Print("[dsmcclib_ParseDii] NumOfModule = %d\n", usNumOfModule);

	usCountOfModule = usNumOfModule;
	while(usCountOfModule > 0)
	{
		szModuleName = NULL;

		// Module ID
		usModuleId = get16bit(pRawData);
		pRawData += DSMCC_DII_MODULEID;
		sSectionLen -= DSMCC_DII_MODULEID;
		HxLOG_Print("[dsmcclib_ParseDii] Module ID = 0x%X\n", usModuleId);

		// Module Size
		ulModuleSize = get32bit(pRawData);
		pRawData += DSMCC_DII_MODULESIZE;
		sSectionLen -= DSMCC_DII_MODULESIZE;
		HxLOG_Print("[dsmcclib_ParseDii] Module Size = %d\n", ulModuleSize);

		// Module Version
		ucModuleVer = *pRawData;
		pRawData += DSMCC_DII_MODULEVER;
		sSectionLen -= DSMCC_DII_MODULEVER;
		HxLOG_Print("[dsmcclib_ParseDii] Module Ver = 0x%X\n", ucModuleVer);

		// Length of Module Information
		cModuleInfoLen = *pRawData;
		pRawData += DSMCC_DII_MODULEINFOLEN;
		sSectionLen -= DSMCC_DII_MODULEINFOLEN;
		HxLOG_Print("[dsmcclib_ParseDii] Module Info Len = %d\n", cModuleInfoLen);

		if (cModuleInfoLen > 0)
		{
			ucDesTag = *pRawData++;
			cDesLen = *pRawData++;

			// module information. ==> name descriptor : 0x02 !!!
			// ETSI EN 301 192 8.2.1 Descriptor idenfification
			if (ucDesTag == 0x02)
			{
				szModuleName = (HUINT8 *)OxDSMCC_Malloc ((cDesLen + 1) * sizeof(HUINT8));
				if (szModuleName == NULL)
				{
					HxLOG_Print("[dsmcclib_ParseDii] OxDSMCC_Malloc fail");
					goto EXIT_DSMCC_FUNC;
				}
				HxSTD_memset(szModuleName, 0, (cDesLen + 1)*(sizeof(HUINT8)));
				HxSTD_memcpy(szModuleName, pRawData, (cDesLen)*(sizeof(HUINT8)));
				HxLOG_Print("[dsmcclib_ParseDii] Module Name = %s\n", szModuleName);
			}
			else
			{
				HxLOG_Print("[dsmcclib_ParseDii] Not a 'Name Descriptor'. Descriptor tag(0x%X) !!!!!\n", ucDesTag);
			}

			pRawData += cDesLen;
			sSectionLen -= cModuleInfoLen;
		}

		pNewModuleInfo = NULL;
		pNewModuleInfo = dsmcclib_AllocModuleInfo();
		if (pNewModuleInfo == NULL)
		{
			OxDSMCC_Free(szModuleName);
			szModuleName = NULL;

			HxLOG_Print("[dsmcclib_ParseDii] dsmcclib_AllocModuleInfo fail");
			goto EXIT_DSMCC_FUNC;
		}
		pNewModuleInfo->usModuleId = usModuleId;
		pNewModuleInfo->ulModuleSize = ulModuleSize;
		pNewModuleInfo->ucModuleVer = ucModuleVer;
		pNewModuleInfo->szModuleName = szModuleName;

		pRootModule= dsmcclib_AppendModuleInfo(pRootModule, pNewModuleInfo);

		usCountOfModule--;
	}

	pNewDiiMsg = dsmcclib_AllocDiiMsg();
	if (pNewDiiMsg == NULL)
	{
		HxLOG_Print("[dsmcclib_ParseDii] dsmcclib_AllocDiiMsg fail");
		goto EXIT_DSMCC_FUNC;
	}
	pNewDiiMsg->ulTransactionId = ulTransactionId;
	pNewDiiMsg->ulDownloadId = ulDownloadId;
	pNewDiiMsg->usBlockSize = usBlockSize;
	pNewDiiMsg->usNumOfModule = usNumOfModule;
	pNewDiiMsg->pModuleInfo = pRootModule;

	// Private Data Length
	usPrivateDataLen = get16bit(pRawData);
	pRawData += DSMCC_DII_PRIVATEDATALEN;
	sSectionLen -= DSMCC_DII_PRIVATEDATALEN;
	HxLOG_Print("[dsmcclib_ParseDii] Private data len = %d\n", usPrivateDataLen);

	// Skip Private Data
	pRawData += usPrivateDataLen;
	sSectionLen -= usPrivateDataLen;

	*ppDiiMsg = pNewDiiMsg;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :
	if(hError != ERR_OK)
	{
		if(pNewDiiMsg != NULL)
		{
			dsmcclib_FreeDiiMsg(pNewDiiMsg);
			pRootModule = NULL;
		}
		if(pRootModule)
		{
			dsmcclib_FreeModuleInfo(pRootModule);
		}
		*ppDiiMsg = NULL;
	}

	return hError;

}



static HERROR			dsmcclib_ParseDdb(HUINT8 *pRawData, HUINT16 sSectionLen, HUINT32 ulDownloadId, DDBMSG_t **ppDdbMsg)
{
	HUINT16					usModuleId = 0;
	HUINT16					usBlockNumber = 0;
	HUINT8					ucModuleVer = 0;
	HUINT8					*pucData = NULL;
	DDBMSG_t				*pNewDdbMsg = NULL;
	HERROR					hError = ERR_FAIL;


	if(pRawData == NULL || ppDdbMsg == NULL)
	{
		HxLOG_Print("[dsmcclib_ParseDdb] param null!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*ppDdbMsg = NULL;

	// Module ID
	usModuleId = (HUINT16)get16bit(pRawData);
	pRawData += DSMCC_DDB_MODULEID;
	sSectionLen -= DSMCC_DDB_MODULEID;
	HxLOG_Print("[dsmcclib_ParseDdb] Module ID = 0x%X\n", usModuleId);

	// Module Version
	ucModuleVer = (HUINT8)*pRawData;
	pRawData += DSMCC_DDB_MODULEVER;
	sSectionLen -= DSMCC_DDB_MODULEVER;
	HxLOG_Print("[dsmcclib_ParseDdb] Module Ver = 0x%X\n", ucModuleVer);

	// Skip 1 reserved byte
	pRawData += DSMCC_DDB_RESERVED;
	sSectionLen -= DSMCC_DDB_RESERVED;

	// Block Number : Position of the block within the module. 0 is first.
	usBlockNumber = (HUINT16)get16bit(pRawData);
	pRawData += DSMCC_DDB_BLOCKNUMBER;
	sSectionLen -= DSMCC_DDB_BLOCKNUMBER;
	HxLOG_Print("[dsmcclib_ParseDdb] Block Number = 0x%X, SecLen(0x%X)\n", usBlockNumber, sSectionLen);

	if (sSectionLen <= 0)
	{
		HxLOG_Print("[dsmcclib_ParseDdb] no data!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	pucData = (HUINT8 *)OxDSMCC_Malloc(sSectionLen*(sizeof(HUINT8)));
	if (pucData == NULL)
	{
		HxLOG_Print("[dsmcclib_ParseDdb] Alloc fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	HxSTD_memset(pucData, 0x00, sSectionLen*(sizeof(HUINT8)));
	HxSTD_memcpy(pucData, pRawData, sSectionLen*(sizeof(HUINT8)));

	pNewDdbMsg = dsmcclib_AllocDdbMsg();
	if (pNewDdbMsg == NULL)
	{
		HxLOG_Print("[dsmcclib_ParseDdb] param null!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	pNewDdbMsg->ulDownloadId = ulDownloadId;
	pNewDdbMsg->ulDataSize = sSectionLen;
	pNewDdbMsg->usModuleId = usModuleId;
	pNewDdbMsg->usBlockNumber = usBlockNumber;
	pNewDdbMsg->ucModuleVer = ucModuleVer;
	pNewDdbMsg->pucData = pucData;
	pucData = NULL;

	*ppDdbMsg = pNewDdbMsg;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :
	if(pucData != NULL)
	{
		OxDSMCC_Free(pucData);
		pucData = NULL;
	}
	return hError;

}





static HERROR			dsmcclib_ParseDsmccMessageHeader(HUINT8 *pHeader,	HINT32 *plHeaderLen,
																						HUINT16 *pusMessageId,
																						HUINT32 *pulTransactionId)
{
	HINT32 			lLen = 0;
	HUINT8 			ucProtocolDiscrim = 0;
	HUINT8 			ucDsmccType = 0;
	HUINT16	 		usMessageId = 0;
	HUINT32 			ulTransactionId = 0;
	HINT8 			cAdaptLen = 0;
	HINT16 			sMessageLen = 0;
	HERROR			hError = ERR_FAIL;


	if(pHeader == NULL || plHeaderLen == NULL || pusMessageId == NULL || pulTransactionId == NULL)
	{
		HxLOG_Print("[parseDsmccMessageHeader] Param NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*plHeaderLen = 0;
	*pusMessageId = 0;
	*pulTransactionId = 0;

	// protocolDiscriminator (1byte)
	ucProtocolDiscrim = *pHeader;
	pHeader += DSMCC_PROTOCOLDISCRIMINATOR;
	lLen += DSMCC_PROTOCOLDISCRIMINATOR;
	HxLOG_Print("[parseDsmccMessageHeader] protocolDiscriminator = 0x%02X\n", ucProtocolDiscrim);
	if(ucProtocolDiscrim != VALUE_DSMCC_PROTOCOLDISCRIMINATOR)
	{
		goto EXIT_DSMCC_FUNC;
	}

	// dsmccType (1byte)
	ucDsmccType = *pHeader;
	pHeader += DSMCC_DSMCCTYPE;
	lLen += DSMCC_DSMCCTYPE;
	HxLOG_Print("[parseDsmccMessageHeader] dsmccType = 0x%02X\n", ucDsmccType);
	if(ucDsmccType !=0x03)
	{
		goto EXIT_DSMCC_FUNC;
	}

	// message_id (2byte)
	usMessageId= get16bit(pHeader);
	pHeader += DSMCC_MESSAGEID;
	lLen += DSMCC_MESSAGEID;
	HxLOG_Print("[parseDsmccMessageHeader] messageId = 0x%02X\n", usMessageId);

	ulTransactionId = get32bit(pHeader);
	pHeader += DSMCC_TRANSACTIONID;
	lLen += DSMCC_TRANSACTIONID;
	HxLOG_Print("[parseDsmccMessageHeader] ulTransactionId = 0x%08X\n", ulTransactionId);

	// reserved (1byte)
	pHeader += DSMCC_RESERVED;
	lLen += DSMCC_RESERVED;

	// adaptation lLength (1byte)
	cAdaptLen= *pHeader;
	pHeader += DSMCC_ADAPTATIONLENGTH;
	lLen += DSMCC_ADAPTATIONLENGTH;
	HxLOG_Print("[parseDsmccMessageHeader] cAdaptLen = %u (0x%02x)\n", cAdaptLen, cAdaptLen);

	// message lLength (2byte)
	sMessageLen = get16bit(pHeader);
	pHeader += DSMCC_MESSAGELENGTH;
	lLen += DSMCC_MESSAGELENGTH;
	HxLOG_Print("[parseDsmccMessageHeader] sMessageLen = %u (0x%04x)\n", sMessageLen, sMessageLen);

	//adaptation field : skip~
	pHeader += cAdaptLen;
	lLen += cAdaptLen;

	*pusMessageId = usMessageId;
	*pulTransactionId = ulTransactionId;
	*plHeaderLen = lLen;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	return hError;
}




static HERROR			dsmcclib_ParseDsmccDownloadDataHeader(HUINT8 *pHeader,	HINT32 *plHeaderLen,
																								HUINT16 *pusMessageId,
																								HUINT32 *pulDownloadId)
{
	HINT32 			lLen = 0;
	HUINT8 			ucProtocolDiscrim = 0;
	HUINT8 			ucDsmccType = 0;
	HUINT16	 		usMessageId = 0;
	HUINT32 			ulDownloadId = 0;
	HINT8 			cAdaptLen = 0;
	HINT16 			sMessageLen = 0;
	HERROR			hError = ERR_FAIL;


	if(pHeader == NULL || plHeaderLen == NULL || pusMessageId == NULL || pulDownloadId == NULL)
	{
		HxLOG_Print("[ParseDsmccDownloadDataHeader] Param NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*plHeaderLen = 0;
	*pusMessageId = 0;
	*pulDownloadId = 0;

	// protocolDiscriminator (1byte)
	ucProtocolDiscrim = *pHeader;
	pHeader += DSMCC_PROTOCOLDISCRIMINATOR;
	lLen += DSMCC_PROTOCOLDISCRIMINATOR;
	HxLOG_Print("[ParseDsmccDownloadDataHeader] protocolDiscriminator = 0x%02X\n", ucProtocolDiscrim);
	if(ucProtocolDiscrim != VALUE_DSMCC_PROTOCOLDISCRIMINATOR)
	{
		goto EXIT_DSMCC_FUNC;
	}

	// dsmccType (1byte)
	ucDsmccType = *pHeader;
	pHeader += DSMCC_DSMCCTYPE;
	lLen += DSMCC_DSMCCTYPE;
	HxLOG_Print("[ParseDsmccDownloadDataHeader] dsmccType = 0x%02X\n", ucDsmccType);
	if(ucDsmccType !=0x03)
	{
		goto EXIT_DSMCC_FUNC;
	}

	// message_id (2byte)
	usMessageId= get16bit(pHeader);
	pHeader += DSMCC_MESSAGEID;
	lLen += DSMCC_MESSAGEID;
	HxLOG_Print("[ParseDsmccDownloadDataHeader] messageId = 0x%02X\n", usMessageId);

	ulDownloadId = get32bit(pHeader);
	pHeader += DSMCC_DOWNLOADID;
	lLen += DSMCC_DOWNLOADID;
	HxLOG_Print("[ParseDsmccDownloadDataHeader] ulDownloadId = 0x%08X\n", ulDownloadId);

	// reserved (1byte)
	pHeader += DSMCC_RESERVED;
	lLen += DSMCC_RESERVED;

	// adaptation lLength (1byte)
	cAdaptLen= *pHeader;
	pHeader += DSMCC_ADAPTATIONLENGTH;
	lLen += DSMCC_ADAPTATIONLENGTH;
	HxLOG_Print("[ParseDsmccDownloadDataHeader] cAdaptLen = %u (0x%02x)\n", cAdaptLen, cAdaptLen);

	// message lLength (2byte)
	sMessageLen = get16bit(pHeader);
	pHeader += DSMCC_MESSAGELENGTH;
	lLen += DSMCC_MESSAGELENGTH;
	HxLOG_Print("[ParseDsmccDownloadDataHeader] sMessageLen = %u (0x%04x)\n", sMessageLen, sMessageLen);

	//adaptation field : skip~
	pHeader += cAdaptLen;
	lLen += cAdaptLen;

	*pusMessageId = usMessageId;
	*pulDownloadId = ulDownloadId;
	*plHeaderLen = lLen;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	return hError;

}


static HERROR		dsmcclib_ParseGroupInfo(HUINT8 *pRawData, HINT16 sGroupSize, GROUP_INFO_t **ppGroupInfo, HINT32 *plLen)
{
	HINT32			lLen = 0;
	HINT32			lTotalLen = 0;
	HERROR			hError = ERR_FAIL;
	HUINT32 		ulGroupId = 0;
	HINT32			lGroupSize = 0;
	HINT16			sGroupInfoLen = 0;
	HINT16			sCmptDesSize = 0;
#if		defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
	CMPTDES_t		*pCmptDes = NULL;
#endif
	GROUP_INFO_t	*pNewGroupInfo = NULL;
	GROUP_INFO_t	*pRootGroupInfo = NULL;

	if(pRawData == NULL || ppGroupInfo == NULL || plLen == NULL)
	{
		HxLOG_Print("[ParseGroupInfo] Param NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*plLen = 0;

	while(sGroupSize >= 14)
	{
		lLen = 0;

		// GroupId (4byte)
		ulGroupId = get32bit(pRawData);
		pRawData += DSMCC_GROUPID;
		lLen += DSMCC_GROUPID;
		HxLOG_Print("[ParseGroupInfo] GroupId = 0x%08X\n", ulGroupId);

		// GroupSize (4byte)
		lGroupSize = get32bit(pRawData);
		pRawData += DSMCC_GROUPSIZE;
		lLen += DSMCC_GROUPSIZE;
		HxLOG_Print("[ParseGroupInfo] GroupSize = 0x%08X\n", lGroupSize);

		/************************************************************************/
		/*																		*/
		/* GroupCompatibility()  												*/
		/*																		*/
		/************************************************************************/

		sCmptDesSize = get16bit(pRawData);
		HxLOG_Print("[ParseGroupInfo] Compatibility Descriptor Length = %d (0x%04X)\n", sCmptDesSize, sCmptDesSize);
#if		defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
{
		HUINT16			usDesCount = 0;

	 	if (sCmptDesSize > 0)
	 	{
			usDesCount = (HUINT16)(get32bit(pRawData) & 0x0000ffff);
			HxLOG_Print("[ParseGroupInfo] Descriptor Count = %d\n", usDesCount);

			pCmptDes = dsmcclib_AllocCmptDes();
			if (pCmptDes == NULL)
			{
				goto EXIT_DSMCC_FUNC;
			}

			hError = dsmcclib_ParseCmptDes(pRawData, pCmptDes);
			if (hError != ERR_OK)
			{
				dsmcclib_FreeCmptDes(pCmptDes);

				pRawData += (sCmptDesSize + DSMCC_CMPTDESLENGTH);
				lLen += (HUINT32)(sCmptDesSize + DSMCC_CMPTDESLENGTH);
				goto EXIT_DSMCC_FUNC;
			}
	 	}
}
#endif
		pRawData += (sCmptDesSize+DSMCC_CMPTDESLENGTH);
		lLen += (sCmptDesSize+DSMCC_CMPTDESLENGTH);

		// GroupInfoLength (2byte)
		sGroupInfoLen = get16bit(pRawData);
		pRawData += DSMCC_GROUPINFOLEN;
		lLen += DSMCC_GROUPINFOLEN;
		HxLOG_Print("[ParseGroupInfo] GroupInfoLength = 0x%04X\n", sGroupInfoLen);

		// GroupInfoByte
		pRawData += sGroupInfoLen;
		lLen += sGroupInfoLen;

		lTotalLen += lLen;
		sGroupSize -= (HINT16)lLen;

		pNewGroupInfo = NULL;
		pNewGroupInfo = dsmcclib_AllocGroupInfo();
		if(pNewGroupInfo == NULL)
		{
			HxLOG_Print("[ParseGroupInfo] dsmcclib_AllocGroupInfo fail!!\n");
			goto EXIT_DSMCC_FUNC;
		}
		pNewGroupInfo->ulGroupId = ulGroupId;
		pNewGroupInfo->lGroupSize = lGroupSize;

#if		defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
		pNewGroupInfo->pCmptDes = pCmptDes;
#endif

		pRootGroupInfo = dsmcclib_AppendGroupInfo(pRootGroupInfo, pNewGroupInfo);
		if(pRootGroupInfo == NULL)
		{
			HxLOG_Print("[ParseGroupInfo] pRootGroupInfo null!!\n");
			goto EXIT_DSMCC_FUNC;
		}
	}

	*ppGroupInfo = pRootGroupInfo;
	*plLen = lTotalLen;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :
	if(hError != ERR_OK)
	{
		if(pRootGroupInfo != NULL)
		{
			dsmcclib_FreeGroupInfo(pRootGroupInfo);
		}
		if(ppGroupInfo)
		{
			*ppGroupInfo = NULL;
		}
		if(plLen)
		{
			*plLen = 0;
		}
	}

	return hError;
}


#if defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
static HERROR			dsmcclib_ParseSubDes(HUINT8 *pRawData, HINT8 cSubDesLen, SUBDES_t **ppSubDes)
{
	HINT32		lLen = 0;
	HUINT8		ucDesType = 0;
	HINT8		cDesLen = 0;
	HUINT8		*pucAdditionalInfo = NULL;
	HERROR		hError = ERR_FAIL;
	SUBDES_t	*pNewSubDes = NULL;
	SUBDES_t	*pRootSubDes = NULL;

	if(pRawData == NULL || ppSubDes == NULL)
	{
		HxLOG_Print("[ParseSubDes] Param NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	while( cSubDesLen > 0)
	{
		ucDesType = *pRawData++;
		lLen++;

		cDesLen = *pRawData++;
		lLen++;

		pucAdditionalInfo = OxDSMCC_Malloc(cDesLen+1);
		if(pucAdditionalInfo == NULL)
		{
			HxLOG_Print("[ParseSubDes] alloc fail!!\n");
			goto EXIT_DSMCC_FUNC;
		}

		HxSTD_memcpy (pucAdditionalInfo, pRawData, cDesLen);
		pRawData += cDesLen;
		lLen += cDesLen;

		pNewSubDes = dsmcclib_AllocSubDes();
		if (pNewSubDes == NULL)
		{
			goto EXIT_DSMCC_FUNC;
		}
		pNewSubDes->ucSubDesType = ucDesType;
		pNewSubDes->cSubDesLen = cDesLen;
		pNewSubDes->pucAdditionalInfo = pucAdditionalInfo;

		pRootSubDes = dsmcclib_AppendSubDes(pRootSubDes, pNewSubDes);
		if(pRootSubDes == NULL)
		{
			goto EXIT_DSMCC_FUNC;
		}
		cSubDesLen -= cDesLen;
	}

	*ppSubDes = pRootSubDes;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :
	if(hError != ERR_OK)
	{
		if(pRootSubDes != NULL)
		{
			dsmcclib_FreeSubDes(pRootSubDes);
		}
		*ppSubDes = NULL;
	}

	return hError;
}


static HERROR				dsmcclib_ParseCmptDes(HUINT8 *pRawData, CMPTDES_t *pCmptDes)
{
	HERROR				hError = ERR_FAIL;
	HINT16				sCmptDesSize = 0;
	HINT16				usDesCount = 0;
	HINT8				cDesLen = 0;
	HINT8				cSubDesLen = 0;
	HINT8				cTmpDesLen = 0;
	HUINT8				ucDesType = 0;
	HUINT8				ucSpecifierType = 0;
	HUINT32				ulSpecifierData = 0;
	HUINT16				usModel = 0;
	HUINT16				usVersion = 0;
	HUINT8				ucSubDesCount = 0;
	CMPTDES_INFO_t		*pNewCmptDesInfo = NULL;
	CMPTDES_INFO_t		*pRootCmptDesInfo = NULL;
	SUBDES_t			*pSubDes = NULL;


	// compatibility_descriptor_length (2byte)
	sCmptDesSize = get16bit(pRawData);
	pRawData += DSMCC_CMPTDESLEN;
	HxLOG_Print("[ParseCmptDes] compatibility_descriptor_length = %d\n", sCmptDesSize);

	if (sCmptDesSize < 3)
	{
		goto EXIT_DSMCC_FUNC;
	}

	// descriptor count (2byte)
	usDesCount = get16bit(pRawData);
	pRawData += DSMCC_CMPTDESCOUNT;
	sCmptDesSize -= DSMCC_CMPTDESCOUNT;
	HxLOG_Print("[ParseCmptDes] (descriptor count = %d)\n", usDesCount);

	// set compatibility desc.
	pCmptDes->sCmptDesSize = sCmptDesSize;
	pCmptDes->usDesCount = usDesCount;

	while(usDesCount > 0 && sCmptDesSize > 2)
	{
		// descriptor type (1byte)
		ucDesType = *pRawData;
		pRawData += DSMCC_CMPTDESTYPE;
		sCmptDesSize -= DSMCC_CMPTDESTYPE;
		HxLOG_Print("[ParseCmptDes] (descriptor type = 0x%X)\n", ucDesType);

		// descriptor length (1byte)
		cDesLen = *pRawData;
		cTmpDesLen = cDesLen;
		pRawData += DSMCC_CMPTDESLEN;
		sCmptDesSize -= DSMCC_CMPTDESLEN;

		HxLOG_Print("[ParseCmptDes] (descriptor length = %d)\n", cDesLen);
		if (cDesLen < 8)
		{
			HxLOG_Print("[ParseCmptDes] Unknown descriptor Format!\n");
			pRawData += cDesLen;
			sCmptDesSize -= cDesLen;
			usDesCount--;

			if(usDesCount > 0 && sCmptDesSize > 2)
			{
				continue;
			}
			else
			{
				break;
			}
		}

		// specifier type (1byte)
		ucSpecifierType = *pRawData;
		pRawData += DSMCC_CMPTSPECIFIERTYPE;
		sCmptDesSize -= DSMCC_CMPTSPECIFIERTYPE;
		HxLOG_Print("[ParseCmptDes] (specifier type = 0x%X)\n", ucSpecifierType);

		// specifier data(OUI) (3byte)
		ulSpecifierData = (HUINT32)get24bit(pRawData);
		pRawData += DSMCC_CMPTSPECIFIERDATA;
		sCmptDesSize -= DSMCC_CMPTSPECIFIERDATA;
		cDesLen -= DSMCC_CMPTSPECIFIERDATA;
		HxLOG_Print("[ParseCmptDes] (specifier data(OUI) = 0x%06X)\n", ulSpecifierData);

		// model (2byte)
		usModel = get16bit(pRawData);
		pRawData += DSMCC_CMPTMODEL;
		sCmptDesSize -= DSMCC_CMPTMODEL;
		cDesLen -= DSMCC_CMPTMODEL;
		HxLOG_Print("[ParseCmptDes] (model = 0x%04X)\n", usModel);

		// version (2byte)
		usVersion = get16bit(pRawData);
		pRawData += DSMCC_CMPTVER;
		sCmptDesSize -= DSMCC_CMPTVER;
		cDesLen -= DSMCC_CMPTVER;
		HxLOG_Print("[ParseCmptDes] (version = 0x%04X)\n", usVersion);

		// subDescriptor count (1byte)
		ucSubDesCount = *pRawData;
		pRawData += DSMCC_SUBDESCOUNT;
		sCmptDesSize -= DSMCC_SUBDESCOUNT;
		cDesLen -= DSMCC_SUBDESCOUNT;
		HxLOG_Print("[ParseCmptDes] (subDescriptor count %d)\n", ucSubDesCount);

		// Compatibility Descriptor 데이타 구성
		pNewCmptDesInfo = dsmcclib_AllocCmptDesInfo();
		if (pNewCmptDesInfo == NULL)
		{
			goto EXIT_DSMCC_FUNC;
		}
		// set cmptdes info
		pNewCmptDesInfo->ucDesType = ucDesType;
		pNewCmptDesInfo->cDesLen = cTmpDesLen;
		pNewCmptDesInfo->ucSpecifierType = ucSpecifierType;
		pNewCmptDesInfo->ulSpecifierData = ulSpecifierData;
		pNewCmptDesInfo->usModel = usModel;
		pNewCmptDesInfo->usVersion = usVersion;
		pNewCmptDesInfo->ucSubDesCount = ucSubDesCount;

		pRootCmptDesInfo = dsmcclib_AppendCmptDesInfo(pRootCmptDesInfo, pNewCmptDesInfo);
		if(pRootCmptDesInfo == NULL)
		{
			goto EXIT_DSMCC_FUNC;
		}

		cSubDesLen = cDesLen;
		if(ucSubDesCount > 0 && cSubDesLen > 0)
		{

			hError = dsmcclib_ParseSubDes(pRawData, cSubDesLen, &pSubDes);
			if(hError != ERR_OK)
			{
				pRawData += cSubDesLen;
				sCmptDesSize -= cSubDesLen;
				cDesLen -= cSubDesLen;
				continue;
			}
			pRawData += cSubDesLen;
			sCmptDesSize -= cSubDesLen;
			cDesLen -= cSubDesLen;

			pNewCmptDesInfo->pSubDes = pSubDes;
		}
	}

	pCmptDes->pInfo = pRootCmptDesInfo;

	hError = ERR_OK;

EXIT_DSMCC_FUNC :
	if(hError != ERR_OK)
	{
		if(pRootCmptDesInfo != NULL)
		{
			dsmcclib_FreeCmptDesNode(pRootCmptDesInfo);
		}
		pCmptDes->pInfo = NULL;
	}

	return hError;
}
#endif



#define	______ALLOC_FUNC______
static DIIMSG_t*				dsmcclib_AllocDiiMsg(void)
{
	DIIMSG_t 		*pDiiMsg = NULL;

	pDiiMsg = (DIIMSG_t *)OxDSMCC_Malloc (sizeof(DIIMSG_t));
	if (pDiiMsg == NULL)
	{
		return NULL;
	}
	HxSTD_memset (pDiiMsg, 0, sizeof(DIIMSG_t));

	return ((DIIMSG_t *)pDiiMsg);
}


static MODULE_INFO_t*			dsmcclib_AllocModuleInfo(void)
{
	MODULE_INFO_t		 *pModuleInfo = NULL;

	pModuleInfo = (MODULE_INFO_t*)OxDSMCC_Malloc (sizeof(MODULE_INFO_t));
	if (pModuleInfo == NULL)
	{
		return NULL;
	}
	HxSTD_memset (pModuleInfo, 0, sizeof(MODULE_INFO_t));

	return ((MODULE_INFO_t*)pModuleInfo);
}


static DDBMSG_t*				dsmcclib_AllocDdbMsg (void)
{
	DDBMSG_t 		*pDdbMsg = NULL;

	pDdbMsg = (DDBMSG_t*)OxDSMCC_Malloc (sizeof(DDBMSG_t));
	if (pDdbMsg == NULL)
	{
		return NULL;
	}
	HxSTD_memset (pDdbMsg, 0, sizeof(DDBMSG_t));

	return ((DDBMSG_t*)pDdbMsg);
}

#if defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
static CMPTDES_t* 			dsmcclib_AllocCmptDes(void)
{
	CMPTDES_t *pCmptDes = NULL;

	pCmptDes = (CMPTDES_t *) OxDSMCC_Malloc (sizeof (CMPTDES_t));
	if(pCmptDes == NULL)
	{
		return NULL;
	}
	HxSTD_memset (pCmptDes, 0, sizeof(CMPTDES_t));
	return ((CMPTDES_t *) pCmptDes);
}

static CMPTDES_INFO_t*		dsmcclib_AllocCmptDesInfo(void)
{
	CMPTDES_INFO_t *pCmptDesInfo = NULL;

	pCmptDesInfo = (CMPTDES_INFO_t *) OxDSMCC_Malloc (sizeof (CMPTDES_INFO_t));
	if(pCmptDesInfo == NULL)
	{
		return (CMPTDES_INFO_t *)NULL;
	}
	HxSTD_memset (pCmptDesInfo, 0, sizeof(CMPTDES_INFO_t));

	return ((CMPTDES_INFO_t *) pCmptDesInfo);

}


static SUBDES_t*				dsmcclib_AllocSubDes(void)
{
	SUBDES_t *pSubDes = NULL;

	pSubDes = (SUBDES_t *) OxDSMCC_Malloc (sizeof (SUBDES_t));
	if(pSubDes == NULL)
	{
		return NULL;
	}
	HxSTD_memset (pSubDes, 0, sizeof(SUBDES_t));
	return ((SUBDES_t *) pSubDes);

}
#endif

static GROUP_INFO_t*			dsmcclib_AllocGroupInfo (void)
{
	GROUP_INFO_t	*pGroupInfo = NULL;

	pGroupInfo = (GROUP_INFO_t *)OxDSMCC_Malloc (sizeof (GROUP_INFO_t));
	if (pGroupInfo == NULL)
	{
		return NULL;
	}
	HxSTD_memset (pGroupInfo, 0x00, sizeof(GROUP_INFO_t));

	return ((GROUP_INFO_t *)pGroupInfo);
}



#define	______DELETE_FUNC______


static DIIMSG_t*				dsmcclib_DeleteDiiMsg(DIIMSG_t *pRoot, DIIMSG_t *pDiiMsg, HBOOL bFree)
{
	DIIMSG_t	*pTemp = NULL;
	DIIMSG_t	*pPrev = NULL;
	HBOOL		bFound = FALSE;

	if(pDiiMsg == NULL)
	{
		return pRoot;
	}

	pPrev = pTemp = pRoot;
	while(pTemp)
	{
		if(pTemp == pDiiMsg)
		{
			if(pTemp == pRoot)
			{
				pRoot = pTemp->pNext;
			}
			else
			{
				pPrev->pNext = pTemp->pNext;
			}
			pDiiMsg->pNext = NULL;
			bFound = TRUE;
			break;
		}
		pPrev = pTemp;
		pTemp = pTemp->pNext;
	}

	if(bFree == TRUE && bFound == TRUE)
	{
		dsmcclib_FreeDiiMsg(pDiiMsg);
	}

	return pRoot;

}





#define	______APPEND_FUNC______

static GROUP_INFO_t*				dsmcclib_AppendGroupInfo(GROUP_INFO_t *pRoot, GROUP_INFO_t *pGroupInfo)
{
	GROUP_INFO_t	*pTemp = NULL;

	if(pGroupInfo == NULL)
	{
		return pRoot;
	}

	pGroupInfo->pNext = NULL;

	if(pRoot == NULL)
	{
		pRoot = pGroupInfo;
	}
	else
	{
		pTemp = pRoot;

		pRoot = pGroupInfo;
		pGroupInfo->pNext = pTemp;
	}

	return pRoot;
}

#if defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
static CMPTDES_INFO_t*			dsmcclib_AppendCmptDesInfo (CMPTDES_INFO_t *pRoot, CMPTDES_INFO_t *pCmptDesInfo)
{
	CMPTDES_INFO_t	*pTemp = NULL;

	if(pCmptDesInfo == NULL)
	{
		return pRoot;
	}

	pCmptDesInfo->pNext = NULL;

	if(pRoot == NULL)
	{
		pRoot = pCmptDesInfo;
	}
	else
	{
		pTemp = pRoot;

		pRoot = pCmptDesInfo;
		pCmptDesInfo->pNext = pTemp;
	}

	return pRoot;
}


static SUBDES_t*					dsmcclib_AppendSubDes (SUBDES_t *pRoot, SUBDES_t *pSubDes)
{
	SUBDES_t	*pTemp = NULL;


	if(pSubDes == NULL)
	{
		return pRoot;
	}

	pSubDes->pNext = NULL;

	if(pRoot == NULL)
	{
		pRoot = pSubDes;
	}
	else
	{
		pTemp = pRoot;

		pRoot = pSubDes;
		pSubDes->pNext = pTemp;
	}

	return pRoot;
}
#endif

static MODULE_INFO_t*				dsmcclib_AppendModuleInfo (MODULE_INFO_t *pRoot, MODULE_INFO_t *pModuleInfo)
{
	MODULE_INFO_t	*pTemp = NULL;


	if(pModuleInfo == NULL)
	{
		return pRoot;
	}

	pModuleInfo->pNext = NULL;

	if(pRoot == NULL)
	{
		pRoot = pModuleInfo;
	}
	else
	{
		pTemp = pRoot;

		pRoot = pModuleInfo;
		pModuleInfo->pNext = pTemp;
	}

	return pRoot;
}


static DIIMSG_t*				dsmcclib_AppendDiiMsg(DIIMSG_t *pRoot, DIIMSG_t *pDiiMsg)
{
	DIIMSG_t	*pTemp = NULL;


	if(pDiiMsg == NULL)
	{
		return pRoot;
	}

	pDiiMsg->pNext = NULL;

	if(pRoot == NULL)
	{
		pRoot = pDiiMsg;
	}
	else
	{
		pTemp = pRoot;

		pRoot = pDiiMsg;
		pDiiMsg->pNext = pTemp;
	}

	return pRoot;
}



static DDBMSG_t*				dsmcclib_AppendDdbMsg (DDBMSG_t *pRoot, DDBMSG_t *pDdbMsg)
{
	DDBMSG_t	*pTemp = NULL;


	if(pDdbMsg == NULL)
	{
		return pRoot;
	}

	pDdbMsg->pNext = NULL;

	if(pRoot == NULL)
	{
		pRoot = pDdbMsg;
	}
	else
	{
		pTemp = pRoot;

		pRoot = pDdbMsg;
		pDdbMsg->pNext = pTemp;
	}

	return pRoot;
}





#define	______FREE_FUNC______

static void			dsmcclib_FreeDsmcc(void)
{
	HUINT32			ulSemId = 0;


	VK_SEM_Get(s_stDsmccInfo.ulSemId);

	ulSemId = s_stDsmccInfo.ulSemId;

	dsmcclib_FreeGroupInfo(s_stDsmccInfo.pGroupInfo);
	dsmcclib_FreeDdbMsg(s_stDsmccInfo.pDdbMsg);
	dsmcclib_FreeDiiMsg(s_stDsmccInfo.pDiiMsg);
	dsmcclib_FreeModuleInfo(s_stDsmccInfo.pDdbBufferModule);
	HxSTD_memset(&s_stDsmccInfo, 0x00, sizeof(DSMCC_INFO_t));

	s_stDsmccInfo.ulSemId = ulSemId;

	VK_SEM_Release(s_stDsmccInfo.ulSemId);
}


static void			dsmcclib_FreeDdbMsg (DDBMSG_t *pDdbMsg)
{
	DDBMSG_t			*pTmpDdbMsg = NULL;
	DDBMSG_t			*pNextDdbMsg = NULL;


	if(pDdbMsg == NULL)
	{
		return;
	}
	pNextDdbMsg = pDdbMsg;

	while (pNextDdbMsg)
	{
		pTmpDdbMsg = pNextDdbMsg;

		if (pTmpDdbMsg->pucData)
		{
			OxDSMCC_Free (pTmpDdbMsg->pucData);
		}

		pNextDdbMsg = pNextDdbMsg->pNext;
		OxDSMCC_Free (pTmpDdbMsg);
	}
}




static void			dsmcclib_FreeModuleInfo (MODULE_INFO_t *pModuleInfo)
{
	MODULE_INFO_t			*pTmpModuleInfo = NULL;
	MODULE_INFO_t			*pNextModuleInfo = pModuleInfo;

	if (pModuleInfo)
	{
		while (pNextModuleInfo)
		{
			pTmpModuleInfo = pNextModuleInfo;

			if (pTmpModuleInfo->szModuleName)
			{
				OxDSMCC_Free (pTmpModuleInfo->szModuleName);
			}

			if(pTmpModuleInfo->pDdb)
			{
				dsmcclib_FreeDdbMsg(pTmpModuleInfo->pDdb);
			}

			pNextModuleInfo = pNextModuleInfo->pNext;
			OxDSMCC_Free (pTmpModuleInfo);
		}
	}
}



static void			dsmcclib_FreeDiiMsg (DIIMSG_t *pDiiMsg)
{
	DIIMSG_t			*pTmpDiiMsg = NULL;
	DIIMSG_t			*pPrevDiiMsg = NULL;


	if(pDiiMsg == NULL)
	{
		return;
	}

	pTmpDiiMsg	= pDiiMsg;
	while (pTmpDiiMsg)
	{
#if		defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
		dsmcclib_FreeCmptDes (pTmpDiiMsg->pCmptDes);
#endif
		dsmcclib_FreeModuleInfo (pTmpDiiMsg->pModuleInfo);
		pTmpDiiMsg->pModuleInfo = NULL;

		pPrevDiiMsg = pTmpDiiMsg;
		pTmpDiiMsg = pTmpDiiMsg->pNext;
		OxDSMCC_Free (pPrevDiiMsg);
	}
}

#if defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
static void					dsmcclib_FreeCmptDes(CMPTDES_t *pCmptDes)
{
	if(pCmptDes == NULL)
	{
		return;
	}

	dsmcclib_FreeCmptDesNode(pCmptDes->pInfo);

	OxDSMCC_Free (pCmptDes);
}



static void					dsmcclib_FreeCmptDesNode(CMPTDES_INFO_t *pCmptDesInfo)
{
	CMPTDES_INFO_t			*pTempCmptDesInfo = NULL;
	CMPTDES_INFO_t			*pPrevCmptDesInfo = NULL;
	SUBDES_t				*pSubDes = NULL;
	SUBDES_t				*pPrevSubDes = NULL;


	if(pCmptDesInfo == NULL)
	{
		return;
	}

	pTempCmptDesInfo = pCmptDesInfo;
	while (pTempCmptDesInfo)
	{
		pSubDes = pTempCmptDesInfo->pSubDes;
		while(pSubDes)
		{
			if(pSubDes->pucAdditionalInfo)
			{
				OxDSMCC_Free (pSubDes->pucAdditionalInfo);
			}
			pPrevSubDes = pSubDes;
			pSubDes = pSubDes->pNext;
			OxDSMCC_Free (pPrevSubDes);
		}
		pPrevCmptDesInfo = pTempCmptDesInfo;
		pTempCmptDesInfo = pTempCmptDesInfo->pNext;
		OxDSMCC_Free (pPrevCmptDesInfo);
	}
}


static void				dsmcclib_FreeSubDes(SUBDES_t *pSubDes)
{
	SUBDES_t				*pTempDes = NULL;
	SUBDES_t				*pPrevSubDes = NULL;

	if(pSubDes == NULL)
	{
		return;
	}

	pTempDes = pSubDes;
	while(pTempDes)
	{
		if(pTempDes->pucAdditionalInfo)
		{
			OxDSMCC_Free (pTempDes->pucAdditionalInfo);
		}
		pPrevSubDes = pTempDes;
		pTempDes = pTempDes->pNext;
		OxDSMCC_Free (pPrevSubDes);
	}
	OxDSMCC_Free (pSubDes);
}
#endif




static void					dsmcclib_FreeGroupInfo (GROUP_INFO_t *pGroupInfo)
{
	GROUP_INFO_t			*pTmpGroupInfo = NULL;
	GROUP_INFO_t			*pPrevGroupInfo = NULL;

	if(pGroupInfo == NULL)
	{
		return;
	}

	pTmpGroupInfo = pGroupInfo;
	while(pTmpGroupInfo)
	{
#if		defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
		dsmcclib_FreeCmptDes(pTmpGroupInfo->pCmptDes);
		pTmpGroupInfo->pCmptDes = NULL;
#endif
		dsmcclib_FreeDiiMsg(pTmpGroupInfo->pDiiMsg);
		pTmpGroupInfo->pDiiMsg = NULL;

		pPrevGroupInfo = pTmpGroupInfo;
		pTmpGroupInfo = pTmpGroupInfo->pNext;
		OxDSMCC_Free(pPrevGroupInfo);
	}
}

#if __these_functions_are_not_used_at_this_time__



static HERROR		dsmcclib_SaveModule(MODULE_INFO_t *pModuleInfo)
{
	HERROR					hError = ERR_FAIL;
	DDBMSG_t				*pTempDdbMsg = NULL;
	HUINT8					*pucFileBuffer = NULL;
	HUINT32					ulFileOffset = 0;
	HUINT8					szFileName[256] = {0, };

	if(pModuleInfo == NULL)
	{
		HxLOG_Print("[dsmcclib_SaveModule] param NULL!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	hError = dsmcclib_SortDdbListbyNumber(pModuleInfo->pDdb);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[dsmcclib_SaveModule] SortDdbListbyNumber fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	pucFileBuffer = OxDSMCC_Malloc(sizeof(HUINT8)*pModuleInfo->ulModuleSize);
	if(pucFileBuffer == NULL)
	{
		HxLOG_Print("[dsmcclib_SaveModule] OxDSMCC_Malloc fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	pTempDdbMsg = pModuleInfo->pDdb;
	while(pTempDdbMsg)
	{
		VK_memcpy32((pucFileBuffer + ulFileOffset), pTempDdbMsg->pucData, (pTempDdbMsg->ulDataSize)*sizeof(HUINT8));
		ulFileOffset += pTempDdbMsg->ulDataSize;

		pTempDdbMsg = pTempDdbMsg->pNext;
	}

{
	HFILEHANDLE		hFileHandle = NULL;
	HUINT32			ulWriteSize = 0;

	sprintf ((char *)szFileName, "%s%s", ".\\mnt\\hd2\\TVTV\\", pModuleInfo->szModuleName);
	hError = dsmcclib_fopen(szFileName, DSMCC_FILEIO_WRITEONLY, &hFileHandle);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[dsmcclib_SaveModule] fopen fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}

	hError = dsmcclib_fwrite(pucFileBuffer, pModuleInfo->ulModuleSize, 1, hFileHandle, &ulWriteSize);
	if(hError != ERR_OK || pModuleInfo->ulModuleSize != ulWriteSize)
	{
		HxLOG_Print("[dsmcclib_SaveModule] fwrite fail(%x : %x)!!\n", pModuleInfo->ulModuleSize, ulWriteSize);
		goto EXIT_DSMCC_FUNC;
	}

	hError = dsmcclib_fclose(hFileHandle);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[dsmcclib_SaveModule] fclose fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}
}

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	return hError;
}



static HERROR		dsmcclib_CheckDdbConflict(DDBMSG_t *pList, DDBMSG_t *pDdbMsg, HBOOL *pbConflict)
{
	HERROR					hError = ERR_FAIL;
	DDBMSG_t				*pTemp = NULL;

	if(pList == NULL || pDdbMsg == NULL || pbConflict == NULL)
	{
		HxLOG_Print("[dsmcclib_CheckDdbConflict] param fail!!\n");
		goto EXIT_DSMCC_FUNC;
	}
	*pbConflict = FALSE;

	pTemp = pList;
	while(pTemp)
	{
		if(pTemp->usBlockNumber == pDdbMsg->usBlockNumber)
		{
			*pbConflict = TRUE;
			goto EXIT_DSMCC_FUNC;
		}
		pTemp = pTemp->pNext;
	}

	hError = ERR_OK;

EXIT_DSMCC_FUNC :

	return hError;
}

static GROUP_INFO_t*				dsmcclib_DeleteGroupInfoItem(GROUP_INFO_t *pRoot, GROUP_INFO_t *pGroupInfo, HBOOL bFree)
{
	GROUP_INFO_t	*pTemp = NULL;
	GROUP_INFO_t	*pPrev = NULL;
	HBOOL		bFound = FALSE;

	if(pGroupInfo == NULL)
	{
		return pRoot;
	}

	pPrev = pTemp = pRoot;
	while(pTemp)
	{
		if(pTemp == pGroupInfo)
		{
			if(pTemp == pRoot)
			{
				pRoot = pTemp->pNext;
			}
			else
			{
				pPrev->pNext = pTemp->pNext;
			}
			pGroupInfo->pNext = NULL;
			bFound = TRUE;
			break;
		}
		pPrev = pTemp;
		pTemp = pTemp->pNext;
	}

	if(bFree == TRUE && bFound == TRUE)
	{
		dsmcclib_FreeGroupInfo(pGroupInfo);
	}

	return pRoot;

}


static CMPTDES_INFO_t*			dsmcclib_DeleteCmptDesInfo (CMPTDES_INFO_t *pRoot, CMPTDES_INFO_t *pCmptDesInfo, HBOOL bFree)
{
	CMPTDES_INFO_t	*pTemp = NULL;
	CMPTDES_INFO_t	*pPrev = NULL;
	HBOOL		bFound = FALSE;

	if(pCmptDesInfo == NULL)
	{
		return pRoot;
	}

	pPrev = pTemp = pRoot;
	while(pTemp)
	{
		if(pTemp == pCmptDesInfo)
		{
			if(pTemp == pRoot)
			{
				pRoot = pTemp->pNext;
			}
			else
			{
				pPrev->pNext = pTemp->pNext;
			}
			pCmptDesInfo->pNext = NULL;
			bFound = TRUE;
			break;
		}
		pPrev = pTemp;
		pTemp = pTemp->pNext;
	}

	if(bFree == TRUE && bFound == TRUE)
	{
		dsmcclib_FreeCmptDesNode(pCmptDesInfo);
	}

	return pRoot;

}


static MODULE_INFO_t*				dsmcclib_DeleteModuleInfo (MODULE_INFO_t *pRoot, MODULE_INFO_t *pModuleInfo, HBOOL bFree)
{
	MODULE_INFO_t	*pTemp = NULL;
	MODULE_INFO_t	*pPrev = NULL;
	HBOOL		bFound = FALSE;

	if(pModuleInfo == NULL)
	{
		return pRoot;
	}

	pPrev = pTemp = pRoot;
	while(pTemp)
	{
		if(pTemp == pModuleInfo)
		{
			if(pTemp == pRoot)
			{
				pRoot = pTemp->pNext;
			}
			else
			{
				pPrev->pNext = pTemp->pNext;
			}
			pModuleInfo->pNext = NULL;
			bFound = TRUE;
			break;
		}
		pPrev = pTemp;
		pTemp = pTemp->pNext;
	}

	if(bFree == TRUE && bFound == TRUE)
	{
		dsmcclib_FreeModuleInfo(pModuleInfo);
	}

	return pRoot;

}



static DDBMSG_t*				dsmcclib_DeleteDdbMsg (DDBMSG_t *pRoot, DDBMSG_t *pDdbMsg, HBOOL bFree)
{
	DDBMSG_t	*pTemp = NULL;
	DDBMSG_t	*pPrev = NULL;
	HBOOL		bFound = FALSE;

	if(pDdbMsg == NULL)
	{
		return pRoot;
	}

	pPrev = pTemp = pRoot;
	while(pTemp)
	{
		if(pTemp == pDdbMsg)
		{
			if(pTemp == pRoot)
			{
				pRoot = pTemp->pNext;
			}
			else
			{
				pPrev->pNext = pTemp->pNext;
			}
			pDdbMsg->pNext = NULL;
			bFound = TRUE;
			break;
		}
		pPrev = pTemp;
		pTemp = pTemp->pNext;
	}

	if(bFree == TRUE && bFound == TRUE)
	{
		dsmcclib_FreeDdbMsg(pDdbMsg);
	}

	return pRoot;

}



static HERROR		dsmcclib_ReadFileToBuffer(HUINT8 *pszFileName, HUINT8 *pBuffer, HUINT32 *pulReadSize)
{
	HERROR			hError = ERR_FAIL;
	HFILEHANDLE		hFileHandle = NULL;
	HUINT32			ulFileSize = 0;
	HUINT32			ulSizeToRead = 0;


	if( pszFileName == NULL || pBuffer == NULL || pulReadSize == NULL)
	{
		HxLOG_Print("[dsmcclib_ReadFileToBuffer] Param NULL!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	*pulReadSize = 0;

	hError = dsmcclib_fopen(pszFileName, DSMCC_FILEIO_READONLY, &hFileHandle);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[dsmcclib_ReadFileToBuffer] file open fail!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	hError = dsmcclib_filelength(hFileHandle, &ulFileSize);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[dsmcclib_ReadFileToBuffer] filelength read fail!!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	hError = dsmcclib_fread(pBuffer, ulFileSize, 1, hFileHandle, &ulSizeToRead);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[dsmcclib_ReadFileToBuffer] file read fail !!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	hError = dsmcclib_fclose(hFileHandle);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[dsmcclib_ReadFileToBuffer] file close fail !!\n");
		goto EXIT_DSMCCLIB_FUNC;
	}

	*pulReadSize = ulSizeToRead;
	hError = ERR_OK;

EXIT_DSMCCLIB_FUNC :

	return hError;

}


#endif


/*********************** End of File ******************************/
