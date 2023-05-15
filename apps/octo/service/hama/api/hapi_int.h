/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hapi_int.h
	@brief

	Description:  									\n
	Module: HaMa Internal Header                    \n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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

#ifndef __HAPI_INT_H__
#define __HAPI_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<hlib.h>
#include	<hapi.h>

#include	<directfb.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HCHAR	*pszKey;
	HxObjectTag_e	eType;
	union
	{
		HINT32	integer;
		HCHAR	*pszString;
	} u;
} HREGINFO_t;

typedef struct
{
	void	*pvUser;
	CBHAPIRegHandler	pfnHandler;
} HREGHandler_t;

typedef struct
{
	HUINT32		ulSemId;
	HUINT32		ulKeyBufferOn;
	HINT32		nClientHandle;
	HINT32		nRPCHandle;

	struct
	{
		void		*pstObj;
		HxQueue_t	*pstJob;
	} stPool;

	HxHASH_t	*pstSignalTable;
	HxList_t		*pstKeyHandlerList;

	HBOOL			bRegistryLoaded;			//	for registry
	HINT32			nEncodeStream;				//	for registry
	HxHASH_t	*pstRegistry;				//	for registry
	HxList_t		*pstRegHandlerList;			//	for registry
} HAPI_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
extern HAPI_t*	hapi_getInstance(void);
extern void		hapi_releaseInstance(HAPI_t *pstHapi);

//	HaMa Registry Function
extern HERROR	HxAPI_RegistryInit(HAPI_t *hapi);
extern HERROR	HxAPI_SetRegistryListener(HAPI_t *hapi, void *pvUser, CBHAPIRegHandler pfnRegHandler);
extern HERROR	HxAPI_SetRegistryEx(HAPI_t *hapi, const HCHAR *pszKey, HxObjectTag_e eType, const void *pvData);
extern HERROR	HxAPI_GetRegistryEx(HAPI_t *hapi, const HCHAR *pszKey, HxObjectTag_e eType, void *pvData, HINT32 nMax);

#endif


