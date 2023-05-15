/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:   sstr_buffermgr.h  $
 * Version:			$Revision:   $
 * Original Author:	YoungKi Kim
 * Current Author:	$Author: ykkim3@humaxdigital.com $
 * Date:			$Date:   $
 * File Description:	     SSTR Buffer Manager Header
 * Module:
 * Remarks:	
 */	

/**
 * @ingroup SSTR_BUFFERMGR
 */

/**
 * @author YoungKi Kim(ykkim3@humaxdigital.com)
 * @date 28 Feb 2012
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
 * @file sstr_buffermgr.h
 */


#ifndef __SSTR_BUFFERMGR_H__
#define __SSTR_BUFFERMGR_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* Start Including Header Files */
#include "htype.h"
#include <sys/time.h> 	 /** for checking delay */

/* End Including Headers*/

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
/* Start Extern variablee */

/* End Extern variable */

/*******************************************************************/
/************************ Constant Definition *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* Start typedef */

/** 
 * SSTR BUFFERMGR ERRORCODE
 */
typedef enum
{
	SSTR_BUFF_OK, /**< OK */
	SSTR_BUFF_FAIL /**< FAIL */
} SSTR_BUFF_ERR;

/** 
 * SSTR BUFFERMGR HANDLE
 */
typedef struct SSTR_BUFFERMGR_s
{
	HINT8 *pBuffer; /**< Buffer */
	HUINT64 ullWritePos; /**< Buffer �� Write Offset */
	HUINT64 ullReadPos;  /**< Buffer �� Read Offset */
	HUINT64 ullBufferSize;  /**< Buffer ũ�� */
	HBOOL bWriteLock;  /**< Write ���� Lock  */
	HBOOL bReadLock;  /**< Read ���� Lock  */
	HULONG ulSema;	/**< Semaphore */
} SSTR_BUFFERMGR_t;

/* End typedef */
/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
/* Start global function prototypes */

/*! 
 *	\brief Buffer Manager ����
 *	\param ullBufferSize : ������ Buffer ũ�� 
 *	\param nBufferType : not supported yet
 *	\return Buffer Manager Handle
 *	\see 
 */
SSTR_BUFFERMGR_t*	SSTR_BUFFERMGR_Open (HUINT64 ullBufferSize, HINT32 nBufferType);

/*! 
 *	\brief Buffer Manager ����
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param nBufferType : not supported yet
 *	\return SSTR_BUFF_ERR
 *	\see 
 */
SSTR_BUFF_ERR		SSTR_BUFFERMGR_Close (SSTR_BUFFERMGR_t *hBufferMgr);

/*! 
 *	\brief Buffer Manager �ʱ�ȭ (Buffer ���)
 *	\param hBufferMgr : Buffer Manager Handle
 *	\return SSTR_BUFF_ERR
 *	\see 
 */
SSTR_BUFF_ERR		SSTR_BUFFERMGR_Clear (SSTR_BUFFERMGR_t *hBufferMgr);

/*! 
 *	\brief Buffer�� Write
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param pData : Write �� Data
 *	\param ullSize : Data ũ��
 *	\return Write �� ũ�� 
 *	\see 
 */
HUINT64				SSTR_BUFFERMGR_Write (SSTR_BUFFERMGR_t *hBufferMgr, HUINT8 *pData, HUINT64 ullSize);

/*! 
 *	\brief Buffer�� ���� Read
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param pData : Read �� Data Buffer
 *	\param ullSize : Read �� ũ��
 *	\return Read �� ũ�� 
 *	\see 
 */
HUINT64				SSTR_BUFFERMGR_Read (SSTR_BUFFERMGR_t *hBufferMgr, HUINT8 *pData, HUINT64 ullSize);

/*! 
 *	\brief Buffer ���� Write Offset ��ȯ
 *	\param hBufferMgr : Buffer Manager Handle
 *	\return Write Offset
 *	\see 
 */
HUINT64				SSTR_BUFFERMGR_GetWritePos (SSTR_BUFFERMGR_t *hBufferMgr);

/*! 
 *	\brief Buffer ���� Read Offset ��ȯ
 *	\param hBufferMgr : Buffer Manager Handle
 *	\return Read Offset
 *	\see 
 */
HUINT64				SSTR_BUFFERMGR_GetReadPos (SSTR_BUFFERMGR_t *hBufferMgr);

/*! 
 *	\brief Buffer ���� Write ������ ũ�� (�� ����)
 *	\param hBufferMgr : Buffer Manager Handle
 *	\return Write ������ ũ��
 *	\see 
 */
HUINT64				SSTR_BUFFERMGR_GetWritableSize (SSTR_BUFFERMGR_t *hBufferMgr);

/*! 
 *	\brief Buffer ���� Read ������ ũ�� (����� ����)
 *	\param hBufferMgr : Buffer Manager Handle
 *	\return Read ������ ũ��
 *	\see 
 */
HUINT64				SSTR_BUFFERMGR_GetReadableSize (SSTR_BUFFERMGR_t *hBufferMgr);

/*! 
 *	\brief Buffer ũ�� ��ȯ  
 *	\param hBufferMgr : Buffer Manager Handle
 *	\return Buffer ũ��
 *	\see 
 */
HUINT64				SSTR_BUFFERMGR_GetBufferSize (SSTR_BUFFERMGR_t *hBufferMgr);

/*! 
 *	\brief Buffer Read ���� Lock Setting
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param bLock : Lock ���� 
 *	\return SSTR_BUFF_ERR
 *	\see 
 */
SSTR_BUFF_ERR		SSTR_BUFFERMGR_SetReadLock (SSTR_BUFFERMGR_t *hBufferMgr, HBOOL bLock);

/*! 
 *	\brief Write Offset ���� ( not used )
 *	\param hBufferMgr : Buffer Manager Handle
 *	\param ullWritePos : ������ Write Offset
 *	\return SSTR_BUFF_ERR
 *	\see 
 */
SSTR_BUFF_ERR		SSTR_BUFFERMGR_SetWritePos (SSTR_BUFFERMGR_t *hBufferMgr, HUINT64 ullWritePos);

/* End global function prototypes */

#ifdef  __cplusplus
}
#endif

#endif /* ! __SSTR_BUFFERMGR_H__ */

/*@}*/

