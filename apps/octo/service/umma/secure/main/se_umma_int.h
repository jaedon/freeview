/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  umma_int.h
	@brief

	Description:  									\n
	Module: SE Umma Internal Header                    \n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef __SE_UMMA_INT_H__
#define __SE_UMMA_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<hlib.h>

#include	<sys/stat.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	SEUMMA_UNDEFINED			0
#define	SEUMMA_SIGNAL_UDS			"/tmp/.se_umma.sig"

#define	SEUMMA_PARTITION_MAX		8

#define	SEUMMA_ASSERT(expr)			\
	if (!(expr)) {\
		HxLOG_Error("SE_UMMA_ASSERT : Argument is not valid!!! \n");\
		abort();\
	}


//#define	CONFIG_ENHANCED_SECURITY	1
#define SEUMMA_HP_EVT_NUM	16

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	// Format 할 Partition의 정보 : Device Index & Partition Number
//	PAL_FS_StorDevIdx_t		 eDevIdx;
	HINT32		 eDevIdx;
	unsigned int			 ulPartitionNo;

	char					 szMountPath[32];
	char					 szVolumeName[16];

	unsigned char			 ePartFsType;
	unsigned long			 ulOption;
} SEUMMA_FormatItem_t;

typedef struct
{
	HUINT64	ulP1, ulP2, ulP3, ulP4;
	HCHAR	*pszStr;
}SE_UMMA_RPC_Item_t;


typedef struct
{
	HUINT32 ulDevCount;
	HUINT32 ulDevNameSize;
	HCHAR	szDevNameBuf[64];	//		char **devName;
	HUINT32 ulPortNum;
	HUINT32 ulSpeed;
	HBOOL bOverCurrent;
}SEUMMA_USB_Info_t;

typedef struct
{
	int		 nNumFormatPartition, nMaxFormatPartition;
	int		 nNumCurrPartition;

	SEUMMA_FormatItem_t		stPartParam[SEUMMA_PARTITION_MAX];
} SEUMMA_FormatList_t;


typedef struct
{
	HUINT32		ulSemId;

	HINT32		nServerHandle;
	HINT32		nRpcHandle;

	HxCBUF_t		nCBuf;
	HCHAR		szFlushName[1024];
	HCHAR		szRpcTemp[1024];
} SEUMMA_t;


#endif
/* end of file */



