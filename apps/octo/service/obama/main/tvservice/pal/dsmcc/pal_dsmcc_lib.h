/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__DSMCCLIB_H__
#define	__DSMCCLIB_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#define SUPPORT_PARSING_DSMCC_LEVEL_2


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagSubDes
{
	HUINT8								ucSubDesType;
	HINT8								cSubDesLen;
	HUINT8								*pucAdditionalInfo;
	struct tagSubDes						*pNext;
}SUBDES_t;




typedef struct tagCompatibilityDesInfo
{
	HUINT8								ucDesType;
	HINT8  								cDesLen;
	HUINT8								ucSpecifierType;
	HUINT8								ucSubDesCount;
	HUINT32								ulSpecifierData;
	HUINT16								usModel;
	HUINT16								usVersion;
	SUBDES_t							*pSubDes;
	struct tagCompatibilityDesInfo			*pNext;
} CMPTDES_INFO_t;



typedef struct tagCompatibilityDes
{
	HINT16								sCmptDesSize;
	HUINT16								usDesCount;
	CMPTDES_INFO_t						*pInfo;
} CMPTDES_t;



typedef struct tagDDBMSG
{
	HUINT32								ulDownloadId;
	HUINT8								ucModuleVer;
	HUINT16								usModuleId;
	HUINT16								usBlockNumber;
	HUINT32								ulDataSize;
	HUINT8								*pucData;
	struct tagDDBMSG					*pNext;
} DDBMSG_t;


typedef struct tagModuleInfo
{
	HUINT8								ucIsComplete;
	HUINT32								ulDownloadId;
	HUINT8								ucModuleVer;
	HUINT16								usModuleId;
	HUINT32								ulModuleSize;
	HUINT8								*szModuleName;			// TVTV EPG Dependent. Get info from name descriptor.
	DDBMSG_t							*pDdb;
	struct tagModuleInfo					*pNext;
}MODULE_INFO_t;



typedef struct tagDIIMSG
{
	HUINT8								ucIsComplete;
	HUINT32								ulTransactionId;
	HUINT32								ulDownloadId;
	HUINT16								usBlockSize;
	HUINT16								usNumOfModule;
	HUINT32								ulCrc;
	CMPTDES_t							*pCmptDes;
	MODULE_INFO_t						*pModuleInfo;
	struct tagDIIMSG						*pNext;
} DIIMSG_t;


typedef struct tagGroupInfo
{
	HUINT32								ulGroupId;				// mapping with transaction id of DII
	HINT32								lGroupSize;
#if	defined(SUPPORT_PARSING_DSMCC_LEVEL_2)
	CMPTDES_t							*pCmptDes;
#endif
	DIIMSG_t							*pDiiMsg;

	struct tagGroupInfo	 				*pNext;
} GROUP_INFO_t;


typedef struct tagDSMCC_INFO
{
	HUINT32								ulSemId;

	HBOOL								bVaild;
	HUINT32								ulDsmccType;
	HUINT32								ulDsmccMode;
	HUINT32								ulBuildNumOfDii;

	HUINT32								ulDsiCrc;
	HUINT32								ulTransactionId;
	HUINT32								ulRegisteredTime;

	HUINT16								usNumOfGroup;
	GROUP_INFO_t						*pGroupInfo;

	DIIMSG_t							*pDiiMsg;

	MODULE_INFO_t						*pDdbBufferModule;

	DDBMSG_t							*pDdbMsg;
}DSMCC_INFO_t;




/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/* Public Function 마다 필히 기재하도록 함 */
/**
  @brief     LAYER_MODULE_DoSomething 함수.

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/

HERROR				DSMCCLIB_InitResource(void);
HERROR				DSMCCLIB_GetDsmccSessionInfo(HUINT32 *pulTransactionId, HUINT32 *pulDsiCrc, HUINT32 *pulRegisteredTime);
HERROR				DSMCCLIB_SetDsmccSessionInfo(HUINT32 ulTransactionId, HUINT32 ulDsiCrc, HUINT32 ulRegisteredTime);
HERROR				DSMCCLIB_SetDsmccMode(HUINT32 ulDsmccMode);
HERROR				DSMCCLIB_GetDsmccMode(HUINT32 *pulDsmccMode);
HERROR				DSMCCLIB_SetDsmccLayerType(HUINT32 ulLayerType);
HERROR				DSMCCLIB_GetDsmccLayerType(HUINT32 *pulLayerType);
HERROR				DSMCCLIB_GetDownloadSize(HUINT32 *pulSize);
void					DSMCCLIB_FreeDsmcc(void);
void					DSMCCLIB_FreeDdbMsg (DDBMSG_t *pDdbMsg);
void					DSMCCLIB_FreeDiiMsg (DIIMSG_t *pDiiMsg);
void					DSMCCLIB_FreeGroupInfo (GROUP_INFO_t *pGroupInfo);
HERROR				DSMCCLIB_ParseDsmccSectionInfo(HUINT8 *pRawData, HUINT16 *psSectionLen, HUINT32 *pulCrc, HUINT8 *pucVerNum);
HERROR				DSMCCLIB_ParseDsi(HUINT8 *pRawData, HINT16 sSectionLen, HUINT16 *pusNumOfGroup, GROUP_INFO_t **ppGroupList);
HERROR				DSMCCLIB_ParseDii(HUINT8 *pRawData, HINT16 uSectionLen, HUINT32 ulTransactionId, DIIMSG_t **ppDiiMsg);
HERROR				DSMCCLIB_ParseDdb(HUINT8 *pRawData, HUINT16 sSectionLen, HUINT32 ulDownloadId, DDBMSG_t **ppDdbMsg);
HERROR				DSMCCLIB_ParseDsmccMessageHeader(	HUINT8 *pHeader,
																	HINT32 *plHeaderLen,
																	HUINT16 *pusMessageId,
																	HUINT32 *pulTransactionId);


HERROR				DSMCCLIB_ParseDsmccDownloadDataHeader(	HUINT8 *pHeader,
																			HINT32 *plHeaderLen,
																			HUINT16 *pusMessageId,
																			HUINT32 *pulDownloadId);

HERROR				DSMCCLIB_BuildDii(void);

HERROR				DSMCCLIB_BuildDdb(HUINT32 ulLayerType, HUINT8 * pucIsComplete, PalDsmcc_File_t **ppDsmccFile);

HERROR				DSMCCLIB_AddGroupInfoList(HUINT16 usNumOfGroup, GROUP_INFO_t *pGroupList);

HERROR				DSMCCLIB_AddDiiMsgList(DIIMSG_t *pNewDiiMsg);

HERROR				DSMCCLIB_AddDdbMsgList(DDBMSG_t *pNewDdbMsg);

HERROR				DSMCCLIB_CheckDiiBuild(HUINT8 *pucIsComplete);


#endif /* !__DSMCCLIB_H__ */
