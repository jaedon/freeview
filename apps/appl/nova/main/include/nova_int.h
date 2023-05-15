/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


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
* ¨Ï 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#ifndef ____NOVA_INT_H____
#define ____NOVA_INT_H____

#include	<stdbool.h>
#include    <vkernel.h>

#include	<hlib.h>
#include	<hapi.h>
#include	<apk.h>
#include    <sapi.h>

#include    <NovaspreadServer.h>

#include    <nova_message.h>
#include    <nova_system.h>
#include    <nova_rpc.h>
#include    <nova_port.h>
#include    <nova_cmd.h>

#define	NOVA_FRIENDLY_NAME_LENGTH	1024

//#define	NOVA_DEBUG //for debugging in release mode (Not NFS mode)
#ifdef	NOVA_DEBUG
	#ifdef HxLOG_Error
		#undef	HxLOG_Error
		#define HxLOG_Error printf
	#endif

	#ifdef HxLOG_Warning
		#undef	HxLOG_Warning
		#define HxLOG_Warning printf
	#endif
#endif

//#define	NOVA_MEM_DEBUG
#ifdef	NOVA_MEM_DEBUG
	#define	NOVA_MEM_CATEGORY				0

	#define	AP_Malloc(size)				HLIB_MEMDBG_Malloc(size, AP_MEM_CATEGORY, __FILE__, __LINE__)
	#define	AP_Calloc(size)				HLIB_MEMDBG_Calloc(size, sizeof(char), AP_MEM_CATEGORY, __FILE__, __LINE__)
	#define	AP_Realloc(mem,size)		HLIB_MEMDBG_Realloc(mem, size, AP_MEM_CATEGORY, __FILE__, __LINE__)
	#define	AP_StrDup(str)				HLIB_MEMDBG_StrDup(str, AP_MEM_CATEGORY, __FILE__, __LINE__)
	#define	AP_MemDup(mem,size)			HLIB_MEMDBG_MemDup(mem, size, AP_MEM_CATEGORY, __FILE__, __LINE__)
	#define	AP_Free(mem)				HLIB_MEMDBG_Free(mem, __FILE__, __LINE__)
#else
	#define AP_Malloc(size)				malloc(size)
	#define AP_Calloc(size)				calloc(size, sizeof(char))
	#define	AP_Realloc(mem,size)		realloc(mem, size)
	#define	AP_StrDup(str)				__HLIB_MEM_StrDup(str)
	#define	AP_MemDup(mem,size)			__HLIB_MEM_MemDup(mem, size)
	#define AP_Free(mem)				free(mem)
#endif

typedef struct
{
	bool isServerActive;
	bool isNetworkActive;
	bool isInitialzed;
	bool isRunnging;
	char modelName[NOVA_FRIENDLY_NAME_LENGTH];

} NOVA_Server_t;

extern bool		NOVA_ServerInit(void);
extern bool		NOVA_ServerDeinit(void);

extern bool		NOVA_ServerStart(char* ModelName);
extern bool		NOVA_ServerShutdown(void);
extern bool		NOVA_ServerSetFriendlyName(char* ModelName);
extern bool		NOVA_ServerRestart(void);
extern bool		NOVA_ServerFactoryReset(void);
extern HBOOL		NOVA_IsActiveNetwork(void);

extern int DI_MI_Init(void);
#endif


