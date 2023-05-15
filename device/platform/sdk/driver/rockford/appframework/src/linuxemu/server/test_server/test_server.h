/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: test_server.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/28/09 2:46p $
 *
 * Module Description:
 *   See Module Overview below.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/appframework/src/linuxclient/server/test_server/test_server.h $
 * 
 * Hydra_Software_Devel/1   4/28/09 2:46p jessem
 * PR 54380: Initial version.
 *
 ***************************************************************************/
#ifndef TEST_SERVER_PRIV_H__
#define TEST_SERVER_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bstd_defs.h"
#include "server.h"

#include "systeminit.h"

typedef struct BEMU_Settings
{
	uint32_t ulBlackMagic;
} BEMU_Settings;

typedef struct BEMU_P_Context
{
	/* Black magic */
	uint32_t           ulBlackMagic;

	BEMU_Settings      stSettings;

	BSystem_Info       stSysInfo;
} BEMU_P_Context;

#if BDBG_DEBUG_BUILD
#define BEMU_P_GENERIC_GET_CONTEXT(handle, context, structname) \
do { \
	(context) = (handle); \
	BDBG_ASSERT(context); \
} while(0)
#else
#define BEMU_P_GENERIC_GET_CONTEXT(handle, context, structname) \
	(context) = (handle)
#endif

#define BEMU_P_GET_CONTEXT(handle, context) \
	BEMU_P_GENERIC_GET_CONTEXT(handle, context, BEMU_P_Context)
	
#ifdef __cplusplus
}
#endif

#endif /* #ifndef TEST_SERVER_PRIV_H__ */

