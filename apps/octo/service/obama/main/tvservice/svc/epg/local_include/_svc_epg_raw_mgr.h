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

#ifndef	__EPG_RAWMGR_H__
#define	__EPG_RAWMGR_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

#define SvcEpg_NUM_SEC_FOR_RECEIVING_TABLE						16
#define SvcEpg_RAW_POOL_NAME_LEN								8

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct tagSvcEpg_RECEIVING_RAW_SEC
{
	HBOOL		 bSecCompleted;					// 하나의 table에 해당하는 모든 section (0 ~ last_sec_number)를 다 받았는가?
	HUINT32		 ulCrc32;						// Section 0 의 CRC32
	HUINT16		 usTsUniqId;					// TS Idx
	HUINT16		 usExtId1, usExtId2;			// Table을 구분하기 위한 EXT ID 2개
	HUINT8		 ucVer;							// Section versions:
	HUINT8		 ucStartSecNum, ucEndSecNum;
	HUINT8		 ucLastSecNum;					// 에 기록되어 있는 last_section_number
	HUINT8		*apucRawSec[SvcEpg_NUM_SEC_FOR_RECEIVING_TABLE];

	struct tagSvcEpg_RECEIVING_RAW_SEC	*pstNext;
} svcEpg_ReceivingRaw_t;

typedef struct
{
	SiTableType_e			 eSiTableType;
	HUINT16					 usSecNum;				// The number of the section in the index array
	HUINT16					 usMaxSecNum;			// The length of the phIdx : the maximum number of the section stored in the current array
	HUINT16					 usStartPos;
	HUINT16					 usInsertPos;			// The position for the section to be inserted in the next time
	HUINT32					*phIdx;					// index handle = unique_id + rawsec_index
	svcEpg_ReceivingRaw_t	*pstReceivingTable;
	svcEpg_ReceivingRaw_t	*pstFreeRcvTable;
} svcEpg_RawSecTable_t;

typedef struct
{
	HUINT8			  		  szPoolName[SvcEpg_RAW_POOL_NAME_LEN];
	svcEpg_RawSecTable_t	  astSiTable[MAX_SI_TABLE_TYPE];

	HUINT16					  usLastEmptySecIdx;
	HUINT16					  usMaxNumRawSec;
	HUINT8					**ppucRawSec;

	// 2012.11.24 : jinh
	// MWC Raw Pool 에 각종 기능 (Private Descriptor Parsing, OP Type, Section Synthesize 등)을 할 수 있도록
	// 일단 Section Making 부터 하도록 한다.
	void			 *pfSectionMaker;			// MakeSectionCb_t
} svcEpg_RawPool_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR			SVC_EPGRAWMGR_InitResource(void);
HUINT32			SVC_EPGRAWMGR_GetSemaphoreId(void);
HERROR			SVC_EPGRAWMGR_GetRawPool (svcEpg_RawPool_t **ppRawPool);
HERROR			SVC_EPGRAWMGR_ClearRawPool (void);
HERROR			SVC_EPGRAWMGR_AddRawSection(HUINT16 usUniqueId, SiTableType_e eSiTableType, HUINT8 *pucRawSec);
HERROR			SVC_EPGRAWMGR_FreeAllSection(HUINT16 usUniqueId, SiTableType_e eSiTableType);
HERROR			SVC_EPGRAWMGR_FreeSection(HUINT16 usUniqueId, SiTableType_e eSiTableType, HBOOL bUseExtId, HUINT16 usExtId);


#endif /* !__EPG_RAWMGR_H__ */
