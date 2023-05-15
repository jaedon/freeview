/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name:		$Workfile:	 soc_mem.h  $
 * Version:			$Revision:	 $
 * Original Author:
 * Current Author:	$Author:  $
 * Date:			$Date: 2016-12-01
 * File Description:		 SoC memory abstration interface
 * Module:
 * Remarks:
 */

/**
 * @defgroup SOC_MEM
 * @brief Provie abstracted memory interface for SoC
 */

/**
 * @author Jeehune Park (parkjh4@humaxdigital.com)
 * @date 01 Dec 2016
 */

/**
 * @brief Provie abstracted memory interface for SoC
 */

/**
 * @note
 * Copyright (C) 2016 Humax Corporation. All Rights Reserved. <br>
 * This software is the confidential and proprietary information
 * of Humax Corporation. You may not use or distribute this software
 * except in compliance with the terms and conditions of any applicable license
 * agreement in writing between Humax Corporation and you.
 */

 /*@{*/

/**
 * @file soc_mem.h
 */

#ifndef	__SOC_MEM_H__
#define	__SOC_MEM_H__

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum _soc_mem_type_of_chunk
{
	eSOC_MEM_PROCESS_HEAP=0,
	eSOC_MEM_SOC_HEAP,
	eSOC_MEM_SOC_SECURE
} SOC_MEM_t;

typedef enum _soc_mem_spec
{
	eSOC_MEM_ABLE_TO_MERGE				=0x00000001,
	eSOC_MEM_ABLE_TO_OFFSET_ADDRESSING	=0x00000002,
	eSOC_MEM_SUPPORT_PROCESS_HEAP		=0x00000004,
	eSOC_MEM_SUPPORT_SOC_HEAP			=0x00000008,
	eSOC_MEM_SUPPORT_SOC_SECURE			=0x00000010,
} SOC_MEM_SPEC_t;

typedef enum _soc_mem_ret
{
	eSOC_MEM_OK=0,
	eSOC_MEM_ERR,
} SOC_MEM_RET_t;

typedef void* SOC_MEM_PRIV_t;

/**********************************************************************************
 * Function protypes (Implementation must be defined below : currently libmi.so)
 **********************************************************************************/
#ifdef FUNCTION_REFERENCE_FOR_MI //This is just reference. Do not enable
int 				SOC_MEM_GetSpecImpl(void);
/* SOC_MEM_PRIV_t must not be NULL except create failed. */
SOC_MEM_PRIV_t 		SOC_MEM_CreateImpl(void);
void 				SOC_MEM_DestroyImpl(SOC_MEM_PRIV_t hHandle);

void *				SOC_MEM_AllocImpl(SOC_MEM_PRIV_t hHandle, size_t nSize, SOC_MEM_t eType);
void 				SOC_MEM_FreeImpl(SOC_MEM_PRIV_t hHandle, void *pBuf, SOC_MEM_t eType);
SOC_MEM_RET_t		SOC_MEM_MemcpyImpl(SOC_MEM_PRIV_t hHandle, void* pDest, SOC_MEM_t eDestType, void* pSrc, SOC_MEM_t eSrcType, size_t nSize);
#endif

/***********************************************************************************
 * Default function if the upper symbols does not exist
 ***********************************************************************************/
int					SOC_MEM_GetSpecDefault(void);
SOC_MEM_PRIV_t		SOC_MEM_CreateDefault(void);
void				SOC_MEM_DestroyDefault(SOC_MEM_PRIV_t hHandle);
void *				SOC_MEM_AllocDefault(SOC_MEM_PRIV_t hHandle, size_t nSize, SOC_MEM_t eType);
void				SOC_MEM_FreeDefault(SOC_MEM_PRIV_t hHandle, void *pBuf, SOC_MEM_t eType);
SOC_MEM_RET_t		SOC_MEM_MemcpyDefault(SOC_MEM_PRIV_t hHandle, void* pDest, SOC_MEM_t eDestType, void* pSrc, SOC_MEM_t eSrcType, size_t nSize);

/* End global function prototypes */

#ifdef	__cplusplus
}
#endif

#endif /* !__SOC_MEM_H__ */

