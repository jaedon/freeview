/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   adp_buffermgr.h  $
 * Version:			$Revision:   $
 * Original Author:	YoungKi Kim
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date:   $
 * File Description:	Adaptive Buffer Manager Header
 * Module:
 * Remarks:	
 */	

/**
 * @ingroup ADP_BUFFERMGR
 */

/**
 * @author YoungKi Kim(ykkim3@humaxdigital.com)
 * @date 19 July 2012
 */

/**
 * @note
 * Copyright (C) 2012 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file adp_buffermgr.h
 */


#ifndef __ADP_BUFFERMGR_H__
#define __ADP_BUFFERMGR_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include <sys/time.h> 	 /** for checking delay */
#include "util_dllist.h"

/* End Including Headers*/

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */

/*******************************************************************/
/************************ Constant Definition **********************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/** 
 * Adaptive Buffer Manager Errcode
 */
typedef enum
{
	ADP_BUFFER_OK, /**< OK */
	ADP_BUFFER_NOHANDLE,
	ADP_BUFFER_NOFREEBLOCK,
	ADP_BUFFER_INVALIDPARAM,
	ADP_BUFFER_LIST,	
	ADP_BUFFER_REFBLOCK,
	ADP_BUFFER_NOTOPENED,
	ADP_BUFFER_NOTCLOSED,
	ADP_BUFFER_ISOPENED,
	ADP_BUFFER_ISCLOSED,
	ADP_BUFFER_NOMOREDATA,
	ADP_BUFFER_ALLOCFAIL,
	ADP_BUFFER_CRITICAL,
	ADP_BUFFER_FAIL /**< FAIL */
} ADP_BUFFER_ERR;

/** 
 * Adaptive Buffer Manager Type
 */
typedef enum
{
	ADP_BUFFERTYPE_MEMORY, /**< Memory */
	ADP_BUFFERTYPE_FILE /**< File not supported yet.*/
} ADP_BUFFER_TYPE;

/** 
 * Adaptive Buffer Manager Mode
 */
typedef enum
{
	ADP_BUFFERMODE_SEQUENCIAL, /**< Sequencial */
	ADP_BUFFERMODE_FRAGMENTED /**< Fragmented */
} ADP_BUFFER_MODE;

/** 
 * Adaptive Buffer Manager Configuration
 */
typedef struct ADP_BUFFERMGR_CONFIG_s
{
	ADP_BUFFER_TYPE eType;
	ADP_BUFFER_MODE eMode;
	HUINT64 ullBufferSize;
	HUINT32 ulBlockSize;
} ADP_BUFFERMGR_CONFIG_t;

/** 
 * Adaptive Buffer Manager Status
 */
typedef struct ADP_BUFFERMGR_STATUS_s
{
	ADP_BUFFER_TYPE eType;
	ADP_BUFFER_MODE eMode;	
	HUINT32 ulBlockSize;

	HUINT64 ullTotalSize;
	HUINT64 ullReadableSize;
	HUINT64 ullUsedSize;
	HUINT64 ullFreedSize;
	HUINT32 ulTotalBlocks;
	HUINT32 ulUsedBlocks;
	HUINT32 ulFreedBlocks;

	HUINT32 unSegments;
} ADP_BUFFERMGR_STATUS_t;

/** 
 * Adaptive Buffer Manager Status
 */
typedef struct ADP_BUFFERMGR_BlockSnapshot_s
{
	HUINT32 unBlockId;
	HUINT32 ulBlockSize;
	HUINT32 ulReadableSize;
	HUINT32 ulWritableSize;	
	HUINT32 ulFreedSize;
	HUINT32 ulCarvedSize;
	HUINT64 ullStartOffset;
	HUINT32 unInterest;
	HINT32 unRefCnt;
	/* do not modify */
	HUINT8* pBuffer;
	void *hdl;
} ADP_BUFFERMGR_BlockSnapshot_t;

typedef struct ADP_BUFFER_UserData_s
{
	HUINT64 ulSegId;
	HUINT32 ulCategory;
	HUINT64 ullStartTime;
	HUINT64 ullDuration;
	HUINT64 ullStartOffset;
	HUINT64 ullSegmentSize;
} ADP_BUFFER_UserData_t;

/** 
 * Adaptive Buffer Node
 */
typedef struct ADP_BUFFER_Block_s
{
//	HBOOL bActive;
	HUINT32 id;
	HBOOL bStart;
	HBOOL bEnd;
	HUINT64 ullStartOffset;
	HUINT32 ulBlockSize;
	HUINT32 ulCarved;
	HUINT32 ulFreed;	
	HINT32 unRefCnt;

	HUINT32 unInterest;
	
	HUINT8 *pBuffer;

	/** User Data */
	ADP_BUFFER_UserData_t tUserData;
} ADP_BUFFER_Block_t;

/** 
 * Adaptive Buffer Manager Handle
 */
typedef struct ADP_BUFFERMGR_s
{
	HUINT8 *pBuffer;
	ADP_BUFFERMGR_STATUS_t tStatus;

	UTIL_DLL_t activeList;
	UTIL_DLL_t freeList;
	
	UTIL_DLLNode_t *readBlockNode;
	HUINT64 ullReadOffset;  /**< Read Block �� Offset */
	UTIL_DLLNode_t *writeBlockNode;
	HUINT64 ullWriteOffset;  /**< Write Block �� Offset */
	HULONG ulSema;	/**< Semaphore */

	HBOOL bWriteLock;  /**< Write ���� Lock  */
	HBOOL bReadLock;  /**< Read ���� Lock  */
} ADP_BUFFERMGR_t;

/* End typedef */
/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/*! 
 *	\brief Buffer Manager ����
 *	\param pConfig : Configuration
 *	\return Buffer Manager Handle
 *	\see 
 */
ADP_BUFFERMGR_t*	ADP_BUFFERMGR_Create (ADP_BUFFERMGR_CONFIG_t *pConfig);

/*! 
 *	\brief Buffer Manager ����
 *	\param hBufferMgr : Buffer Manager Handle
 *	\return ADP_BUFFER_ERR
 *	\see 
 */
ADP_BUFFER_ERR		ADP_BUFFERMGR_Destroy (ADP_BUFFERMGR_t *hBufferMgr);

/*! 
 *	\brief Buffer Manager
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param pUserData : UserData
 *	\return ADP_BUFFER_ERR
 *	\see 
 */
ADP_BUFFER_ERR		ADP_BUFFERMGR_Open (ADP_BUFFERMGR_t *hBufferMgr, ADP_BUFFER_UserData_t *pUserData);

/*! 
 *	\brief Buffer Manager ����
 *	\param hBufferMgr : Buffer Manager Handle
 *	\return ADP_BUFFER_ERR
 *	\see 
 */
ADP_BUFFER_ERR		ADP_BUFFERMGR_Close (ADP_BUFFERMGR_t *hBufferMgr);


/*! 
 *	\brief Buffer Manager ���
 *	\param hBufferMgr : Buffer Manager Handle
 *	\return ADP_BUFFER_ERR
 *	\see 
 */
ADP_BUFFER_ERR		ADP_BUFFERMGR_Flush (ADP_BUFFERMGR_t *hBufferMgr);

/*! 
 *	\brief Buffer�� Write
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param pData : Write �� Data
 *	\param ullSize : Data ũ��
 *	\return Write �� ũ�� 
 *	\see 
 */
HUINT64				ADP_BUFFERMGR_Write (ADP_BUFFERMGR_t *hBufferMgr, HUINT8 *pData, HUINT64 ullSize);

/*! 
 *	\brief Buffer�� ���� Read
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param pData : Read �� Data Buffer
 *	\param ullSize : Read �� ũ��
 *	\return Read �� ũ�� 
 *	\see 
 */
HUINT64				ADP_BUFFERMGR_Read (ADP_BUFFERMGR_t *hBufferMgr, HUINT8 *pData, HUINT64 ullSize);

/*! 
 *  \brief Block�� Userdata�� ���� 
 *  \param hBufferMgr : Buffer Manager Handle
 *  \param unCategory : Type or Adaption Id(Stream Id) ������ ���
 *  \param unSegId : Segment Id
 *  \param[out] pUserData : ã�� Segment�� UserData
 *  \return ErrorCode
 *  \see 
*/
ADP_BUFFER_ERR   ADP_BUFFERMGR_GetUserData (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 unCategory, HUINT32 unSegId, ADP_BUFFER_UserData_t *pUserData);

/*! 
 *	\brief Segment Id�� Offset���� Block Id�� Offset ã��
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param unCategory : Type or Adaption Id(Stream Id) ������ ���
 *	\param unSegId : Segment Id
 *	\param ullOffset : Segment Offset
 *	\param[out] punBlockId : ã�� Block Id
 *	\param[out] pullBlockOffset : ã�� Block �� Offset
 *	\return ErrorCode
 *	\see 
 */
ADP_BUFFER_ERR		ADP_BUFFERMGR_FindBlock (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 unCategory, HUINT32 unSegId, HUINT64 ullOffset, HUINT32 *punBlockId, HUINT64 *pullBlockOffset);

/*! 
 *	\brief Offset���� Block Id�� Offset ã�� (for None UserData)
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param ullOffset : Segment Offset
 *	\param[out] punBlockId : ã�� Block Id
 *	\param[out] pullBlockOffset : ã�� Block �� Offset
 *	\return ErrorCode
 *	\see 
 */
ADP_BUFFER_ERR 	 	ADP_BUFFERMGR_FindBlockByOffset (ADP_BUFFERMGR_t *hBufferMgr, HUINT64 ullOffset, HUINT32 *punBlockId, HUINT64 *pullBlockOffset);

/*! 
 *	\brief Segment Id�� Time���� Block Id�� Offset ã��
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param unCategory : Type or Adaption Id(Stream Id) ������ ���
 *	\param unSegId : Segment Id
 *	\param ullTime : Time
 *	\param[out] punBlockId : ã�� Block Id
 *	\param[out] pullBlockOffset : ã�� Block �� Offset
 *	\return ErrorCode
 *	\see 
 */
ADP_BUFFER_ERR		ADP_BUFFERMGR_FindBlockByTime (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 unCategory, HUINT32 unSegId, HUINT64 ullTime, HUINT32 *punBlockId, HUINT64 *pullBlockOffset);

/*! 
 *	\brief Buffer Read ���� Lock Setting
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param bLock : Lock ���� 
 *	\return ADP_BUFFER_ERR
 *	\see 
 */
ADP_BUFFER_ERR		ADP_BUFFERMGR_SetReadLock (ADP_BUFFERMGR_t *hBufferMgr, HBOOL bLock);

/*! 
 *	\brief Buffer Write ���� Lock Setting
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param bLock : Lock ���� 
 *	\return ADP_BUFFER_ERR
 *	\see 
 */
ADP_BUFFER_ERR		ADP_BUFFERMGR_SetWriteLock (ADP_BUFFERMGR_t *hBufferMgr, HBOOL bLock);

/*! 
 *	\brief Read Offset ����
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param ulReadBlock : ������ Read Block
 *	\param ullReadPos : ������ Read Offset
 *	\return ADP_BUFFER_ERR
 *	\see 
 */
ADP_BUFFER_ERR		ADP_BUFFERMGR_SetReadPos (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 ulReadBlock, HUINT64 ullReadPos);

/*! 
 *	\brief Head Block ���� refcnt�� ���� Block�� ���Ͽ� FreeList�� �̵� (Write �� ���� Block Ȯ��)
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param unMaxBlocks : Ȯ���� �ִ� Block ��
 *	\return Ȯ���� Block ���� 
 *	\see 
 */
HUINT32				ADP_BUFFERMGR_SimpleRetainBlock (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 unMaxBlocks);

 /*! 
  *  \brief Head Block ���� refcnt�� ���� Block�� ���Ͽ� FreeList�� �̵� (Write �� ���� Block Ȯ��)
  *  \param hBufferMgr : Buffer Manager Handle
  *  \param unCategory :  ���� ��� ī�װ� 
  *  \param unSegId :  ���� ��� Segment
  *  \param ullBefore :  ���� ���� Offset (�ش� Offset ���� Block �� ����)
  *  \param unMaxBlocks : �ִ� Ȯ�� Block  
  *  \return Ȯ���� Block ���� 
  *  \note Ȯ�� ����� ���� Offset�� ���� ��� ������ �� ���� ����, Write�� �����Ͱ� ���� ��� ����, ���� Offset ���� Block ����   
  *  \see 
  */
HUINT32				ADP_BUFFERMGR_RetainBlock (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 unCategory, HUINT32 unSegId, HUINT64 ullBefore,  HUINT32 unMaxBlocks);

/*! 
 *	\brief Head Block ���� refcnt�� ���� Block�� ���Ͽ� ��� �󵵰� ���� ���� ������ �ϳ��� Block �� FreeList�� �̵� (Write �� ���� Block Ȯ��)
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param unCategory :  ���� ��� ī�װ� 
 *	\param unSegId :  ���� ��� Segment
 *	\param ullBefore : ���� ���� Offset  (�ش� Offset ���� Block �� ����)	
 *	\return Ȯ���� Block ���� 
 * 	\note ���� Offset ������ �߿��ϴ�.  ���� Offset �������� �ѹ� �̻��� ����� �Ǿ���� �� Block ���� ������� ����ؾ� �Ѵ�. 
 *	\see 
 */
HUINT32 			ADP_BUFFERMGR_RetainUninterestedBlock (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 unCategory, HUINT32 unSegId, HUINT64 ullBefore);

/*! 
  *  \brief ���� Write ���� Block�� Flush ��Ų��.
  *  \param hBufferMgr : Buffer Manager Handle
  *  \see 
  */
ADP_BUFFER_ERR 		ADP_BUFFERMGR_FlushWriteBlock (ADP_BUFFERMGR_t *hBufferMgr);

/*! 
  *  \brief Buffer �� ũ�⸦ ������ �Ѵ�. (���� ũ�� ���� �۰� ���� �Ұ���)
  *  \param hBufferMgr : Buffer Manager Handle
  *  \param ullSize : �������� ũ��
  *  \return ���� Buffer ũ��
  *  \see 
  */
HUINT64 			ADP_BUFFERMGR_Resize (ADP_BUFFERMGR_t *hBufferMgr, HUINT64 ullSize);

/*! 
 *  \brief Block�� ũ�⸦ ��´�.
 *  \param hBufferMgr : Buffer Manager Handle
 *  \param[out] pSize : Block ũ��
 *  \return ADP_BUFFER_ERR
 *  \see 
*/
ADP_BUFFER_ERR 		ADP_BUFFERMGR_GetBlockSize (ADP_BUFFERMGR_t *hBufferMgr, HUINT32 *pSize);

/*! 
 *	\brief Status �� ��´�.
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param[out] pStatus : Status
 *	\return ADP_BUFFER_ERR
 *	\see 
 */
ADP_BUFFER_ERR 		ADP_BUFFERMGR_GetStatus (ADP_BUFFERMGR_t *hBufferMgr, ADP_BUFFERMGR_STATUS_t *pStatus);

/*! 
 *  \brief Read Block�� Status �� ��´�.
 *  \param hBufferMgr : Buffer Manager Handle
 *  \param[out] pStatus : Block Status
 *  \return ADP_BUFFER_ERR
 *  \see 
*/
ADP_BUFFER_ERR 		ADP_BUFFERMGR_GetReaderSnapshot(ADP_BUFFERMGR_t *hBufferMgr, ADP_BUFFERMGR_BlockSnapshot_t *pSnapshot);

/*! 
 *  \brief Write Block�� Status �� ��´�.
 *  \param hBufferMgr : Buffer Manager Handle
 *  \param[out] pStatus : Block Status
 *  \return ADP_BUFFER_ERR
 *  \see 
*/
ADP_BUFFER_ERR 		ADP_BUFFERMGR_GetWriterSnapshot(ADP_BUFFERMGR_t *hBufferMgr, ADP_BUFFERMGR_BlockSnapshot_t *pSnapshot);

/*! 
 *  \brief ���� Block�� Snapshot �� ��´�. Snapshot�� hdl�� NULL �̸� ù Block�� Snapshot�� ��´�.
 *  \param hBufferMgr : Buffer Manager Handle
 *  \param[out] pSnapthot : Next Block Snapshot
 *  \return ADP_BUFFER_ERR
 *  \see 
*/
ADP_BUFFER_ERR 	 ADP_BUFFERMGR_NextSnapshot(ADP_BUFFERMGR_t *hBufferMgr, ADP_BUFFERMGR_BlockSnapshot_t *pSnapshot);

void 				ADP_BUFFERMGR_PrintStatus (ADP_BUFFERMGR_t *hBufferMgr);

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __ADP_BUFFERMGR_H__ */

/*@}*/

