/**
@file			mheg_rawmgr.h
@brief			mheg_rawmgr.h (MHEG Service)

Description:  \n
Module: MW/ \n
Remarks :\n

Copyright (c) 2008 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef		__MHEG_MGR_RAW_H__
#define		__MHEG_MGR_RAW_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include "mheg_param.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define MHEG_DEMUX_VALUE_DONT_CARE				0xFFFE

#define MAX_SI_RAWSEC_NUM		4000

#define RAW_POOL_NAME_LEN		8

#define INVALID_INDEX_HANDLE			0xffffffff
#define MAX_INDEX_HANDLE_NUM			128
#define MACRO_MAKE_INDEX_HANDLE(usUniqueId, usIdx)	(HUINT32)(usUniqueId << 16 | usIdx)
#define MACRO_GET_UNIQUEID_FROM_IDXHANDLE(hIdx)		(HUINT16)(hIdx >> 16 & 0xffff)
#define MACRO_GET_INDEX_FROM_IDXHANDLE(hIdx)		(HUINT16)(hIdx & 0xffff)

#define EXTID_USE		0
#define EXTID_UNUSE		1
#define EXTID_PRIVATE	2


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagMHEG_SiRawDataList_t
{
	HUINT32 	ulViewId;				/* action handle for Watch TV related with current SI Event */
	HUINT16		uniqeId;				/* tsIdx */
	HUINT16		pid;
	HUINT8		tableId;
	HUINT16		tableIdExtn; 			/* 4th ~ 5th bytes of Section data */
	HUINT16 	savedRawDataSize;
	HUINT8		*savedRawData;

	struct tagMHEG_SiRawDataList_t	*pNext;

}MHEG_SiRawDataList_t;

typedef enum eMHEG_SI_RAW_DATA_TYPE
{
	eMHEG_SI_RAW_DATA_TYPE_LIVE,
	eMHEG_SI_RAW_DATA_TYPE_STREAMING,
	eMHEG_SI_RAW_DATA_TYPE_MAX
}MhegSiRawDataType;

/** @brief raw section을 관리하는 구조체 */

#define NUM_SEC_FOR_RECEIVING_TABLE				16

typedef struct tagRECEIVING_RAW_SEC
{
	HBOOL		 bSecCompleted;					// 하나의 table에 해당하는 모든 section (0 ~ last_sec_number)를 다 받았는가?
	HUINT32		 ulCrc32;						// Section 0 의 CRC32
	HUINT16		 usTsUniqId;					// TS Idx
	HUINT16		 usExtId1, usExtId2;			// Table을 구분하기 위한 EXT ID 2개
	HUINT8		 ucVer;							// Section versions:
	HUINT8		 ucStartSecNum, ucEndSecNum;
	HUINT8		 ucLastSecNum;					// 에 기록되어 있는 last_section_number
	HUINT8		*apucRawSec[NUM_SEC_FOR_RECEIVING_TABLE];

	struct tagRECEIVING_RAW_SEC	*pstNext;
} MhegReceivingRaw;

typedef struct
{
	SiTableType_t		 eSiTableType;
	HUINT16				 usSecNum;				// The number of the section in the index array
	HUINT16				 usMaxSecNum;			// The length of the phIdx : the maximum number of the section stored in the current array
	HUINT16				 usStartPos;
	HUINT16				 usInsertPos;			// The position for the section to be inserted in the next time
	HUINT32					*phIdx;					// index handle = unique_id + rawsec_index
	MhegReceivingRaw		*pstReceivingTable;
	MhegReceivingRaw		*pstFreeRcvTable;
} MhegRawSecTable;

typedef struct
{
	HUINT8			  szPoolName[RAW_POOL_NAME_LEN];
	MhegRawSecTable	  astSiTable[MAX_SI_TABLE_TYPE];

	HUINT16			  usLastEmptySecIdx;
	HUINT16			  usMaxNumRawSec;
	HUINT8			**ppucRawSec;
} MhegRawPool;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR 		MHEG_Raw_InitModule(void);
HINT32		MHEG_Raw_GetSemaphore(void);
HINT32		MHEG_Raw_ReleaseSemaphore(void);
HINT32		MHEG_RawMakeList_GetSemaphore(void);


HINT32		MHEG_RawMakeList_ReleaseSemaphore(void);
MhegRawPool*	MHEG_Raw_GetRawPool (MhegSiRawDataType eType);
void 			MHEG_Raw_ClearRawPool (MhegSiRawDataType eType);
HERROR			MHEG_AppendSiTblRawDataToList(MHEG_SiRawDataList_t* pstRawData, MhegSiRawDataType eType);
HERROR			MHEG_RemoveSiTblRawData(MHEG_SiRawDataList_t* pstRawData, MhegSiRawDataType eType);
MHEG_SiRawDataList_t *	MHEG_GetSiRawData(HUINT16 usUniqId, HUINT16 usPid, HUINT16 usTableId, HUINT16 usExtTableId, MhegSiRawDataType eType);
HUINT32			MHEG_RAW_AddRawSection(HUINT16 usUniqueId, SiTableType_t eSiTableType, HUINT8 *pucRawSec, MhegSiRawDataType eType);
void			MHEG_RAW_FreeAllSection(HUINT16 usUniqueId, SiTableType_t eSiTableType, MhegSiRawDataType eType);
HERROR			MHEG_RAW_FreeSection(HUINT16 usUniqueId, SiTableType_t eSiTableType, HBOOL bUseExtId, HUINT32 ulExtId, MhegSiRawDataType eType);
HERROR			MHEG_SI_GetFromMHEGRawData(HUINT16 usTsUniqId, HUINT16 usPid, HUINT16 usTableId, HUINT16 usExtTableId, HUINT16* pusDataLen, HUINT8** ppucSection);
HERROR			MHEG_RAW_GetComponentInfo(HUINT16 usTSUniqId, HUINT16 usSvcId, HINT32 nComponent, HUINT8 ucElementType, HINT32 *pnCompPid, HUINT8 *pucCodecType, HUINT8 *pucStreamType, DxStereoSelect_e *pSoundMode);
HERROR			MHEG_RemoveSiTblPMTDataExceptCurSvc(HUINT16 usUniqId, HUINT16 usPmtPid, MhegSiRawDataType eType);
MhegRawSecTable* MHEG_RAW_GetRawTable (MhegRawPool *pstPool, SiTableType_t eTableType);
HUINT8*			MHEG_RAW_GetRawSection (MhegRawPool *pstPool, HUINT32 ulSecIdx);
HUINT16			MHEG_RAW_GetExtIdFromRawData (HUINT8 *pucRaw);


#endif /* !__MHEG_MGR_RAWMGR_H__ */

