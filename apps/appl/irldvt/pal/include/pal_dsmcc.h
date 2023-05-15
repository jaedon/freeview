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

#ifndef	__PAL_DSMCC_H__
#define	__PAL_DSMCC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#if 0
#define SUPPORT_PARSING_DSMCC_LEVEL_1			1
#define SUPPORT_PARSING_DSMCC_LEVEL_2			1
#define SUPPORT_PARSING_DSMCC_LEVEL_3			1
#endif

#define DII_MESSAGE_ID							0x1002
#define DDB_MESSAGE_ID							0x1003
#define DSI_MESSAGE_ID							0x1006


#define DSMCC_FILE_NAME_LEN					256


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef	enum
{
	ePAL_DSMCC_ONE_LAYER,
	ePAL_DSMCC_TWO_LAYER
} PalDsmcc_Layer_e;

typedef	enum
{
	ePAL_DSMCC_DSI_RECEIVED,
	ePAL_DSMCC_DDB_RECEIVED
}PalDsmcc_ReceiveEvent_e;

typedef enum
{
	ePAL_DSMCC_DOWNLOAD_INIT,
	ePAL_DSMCC_DOWNLOAD_BUILD,
	ePAL_DSMCC_DOWNLOAD_COMPLETE,
	ePAL_DSMCC_DOWNLOAD_FAIL,
	ePAL_DSMCC_DOWNLOAD_TIMEOUT,

	ePAL_DSMCC_DOWNLOAD_MAX
} PalDsmcc_DownloadEvent_e;

typedef	void*		HFILEHANDLE;

typedef struct tagDsmccFile
{
	HUINT32				ulModuleSize;
	HUINT8				*pucModuleName;
	HUINT8				*pucFilePointer;
	struct tagDsmccFile	*pNext;
}PalDsmcc_File_t;


typedef struct tagDSMCCSectionInfo
{
	HUINT8		ucTableId;
	HUINT8		ucSectionLength;
	HUINT8		ucVersion;
	HUINT32		ulCrc;

}PalDsmcc_SectionInfo_t;


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
extern HERROR		PAL_DSMCC_Init(void);
extern HERROR		PAL_DSMCC_FreeDsmcc(void);
extern HERROR		PAL_DSMCC_GetDsmccSessionInfo(HUINT32 *pulTransactionId, HUINT32 *pulDsiCrc, HUINT32 *pulRegisteredTime);
extern HERROR		PAL_DSMCC_BuildDownloadData(HUINT16 usMessageId, HUINT8 *pucIsComplete, PalDsmcc_File_t **ppDsmccFile);
extern HERROR		PAL_DSMCC_ParseDsmccForDsiNDii(HUINT8 *pucRawData, HUINT16 *pusMessageId);
extern HERROR		PAL_DSMCC_ParseDsmccForDdb(HUINT8 *pucRawData, HUINT16 *pusMessageId);
extern HERROR		PAL_DSMCC_SetDsmccLayerType(HUINT32 ulLayerType);
extern HERROR		PAL_DSMCC_ParseDsmccSectionInfo(HUINT8 *pRawData, HUINT16 *psSectionLen, HUINT32 *pulCrc, HUINT8 *pucVerNum);
extern HERROR		PAL_DSMCC_ParseDsmccMessageHeader(	HUINT8 *pHeader, HINT32 *plHeaderLen, HUINT16 *pusMessageId, HUINT32 *pulTransactionId);
extern HERROR		PAL_DSMCC_GetDownloadSize(HUINT32 *pulSize);


#endif /* !__PAL_DSMCC_H__ */
