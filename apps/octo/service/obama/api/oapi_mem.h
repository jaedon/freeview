/**************************************************************
 *	@file		oapi_mem.h
 *	This module for OCTO 2.0's Open API Interface
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2011/09/07
 *	@author			humax
 *	@brief			OCTO Open API Interface
 **************************************************************/

#ifndef	__OAPI_MEM_H__
#define	__OAPI_MEM_H__

#include	<hlib.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define OxMEM_Malloc				HLIB_STD_MemAlloc
#define OxMEM_Calloc				HLIB_STD_MemCalloc
#define	OxMEM_Realloc				HLIB_STD_MemRealloc
#define	OxMEM_StrDup				HLIB_STD_StrDup
#define	OxMEM_MemDup				HLIB_STD_MemDup
#define OxMEM_Free					HLIB_STD_MemFree
#define	OxMEM_Malloc_CB				HLIB_STD_MemAlloc_CB
#define	OxMEM_Free_CB				HLIB_STD_MemFree_CB


#ifdef	__cplusplus
}
#endif

#endif

