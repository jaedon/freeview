#ifndef __OBAMA_MEMCONFIG_H__
#define __OBAMA_MEMCONFIG_H__

#ifndef AUTOCONF_INCLUDED
#include "autoconf.h"
#endif

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
//	�⺻ ������ HLIB�������� HxMEM_ALLOC���� �Լ��� �ٷ� ����Ѵ�.
//	�� �ܿ��� HxSTD_MemXXX�� ����ϰų��Ʒ��� ���� redefine�ؼ� ����Ѵ�.
//	HxSTD_MemXXX���� debug�� ���ؼ��� Makefile�� HxMEMDBG_ENABLE�� Ǯ��� �Ѵ�.
//	HxMEM_MODULE_XXXX���� HxMEMDBG_ENABLE�� Ǯ�� �ʰ� �ٷ� ��� �����ϴ�.

#define	OxRPC_Malloc				HLIB_STD_MemAlloc
#define	OxRPC_Calloc				HLIB_STD_MemCalloc
#define	OxRPC_Realloc				HLIB_STD_MemRealloc
#define	OxRPC_StrDup				HLIB_STD_StrDup
#define	OxRPC_MemDup				HLIB_STD_MemDup
#define	OxRPC_Free					HLIB_STD_MemFree
#define	OxRPC_Malloc_CB				HLIB_STD_MemAlloc_CB
#define	OxRPC_Free_CB				HLIB_STD_MemFree_CB

#define OxMGR_Malloc				HLIB_STD_MemAlloc
#define OxMGR_Calloc				HLIB_STD_MemCalloc
#define	OxMGR_Realloc				HLIB_STD_MemRealloc
#define	OxMGR_StrDup				HLIB_STD_StrDup
#define	OxMGR_MemDup				HLIB_STD_MemDup
#define OxMGR_Free					HLIB_STD_MemFree
#define	OxMGR_Malloc_CB				HLIB_STD_MemAlloc_CB
#define	OxMGR_Free_CB				HLIB_STD_MemFree_CB

#define OxAP_Malloc					HLIB_STD_MemAlloc
#define OxAP_Calloc					HLIB_STD_MemCalloc
#define	OxAP_Realloc				HLIB_STD_MemRealloc
#define	OxAP_StrDup					HLIB_STD_StrDup
#define	OxAP_MemDup					HLIB_STD_MemDup
#define OxAP_Free					HLIB_STD_MemFree

#define OxDB_Malloc					HLIB_STD_MemAlloc
#define OxDB_Calloc					HLIB_STD_MemCalloc
#define	OxDB_Realloc				HLIB_STD_MemRealloc
#define	OxDB_StrDup					HLIB_STD_StrDup
#define	OxDB_MemDup					HLIB_STD_MemDup
#define OxDB_Free					HLIB_STD_MemFree

#define OxFS_Malloc					HLIB_STD_MemAlloc
#define OxFS_Calloc					HLIB_STD_MemCalloc
#define	OxFS_Realloc				HLIB_STD_MemRealloc
#define	OxFS_StrDup					HLIB_STD_StrDup
#define	OxFS_MemDup					HLIB_STD_MemDup
#define OxFS_Free					HLIB_STD_MemFree

#define OxDSMCC_Malloc				HLIB_STD_MemAlloc
#define OxDSMCC_Calloc				HLIB_STD_MemCalloc
#define	OxDSMCC_Realloc				HLIB_STD_MemRealloc
#define	OxDSMCC_StrDup				HLIB_STD_StrDup
#define	OxDSMCC_MemDup				HLIB_STD_MemDup
#define OxDSMCC_Free				HLIB_STD_MemFree

#define OxEPG_Malloc				HLIB_STD_MemAlloc
#define OxEPG_Calloc				HLIB_STD_MemCalloc
#define	OxEPG_Realloc				HLIB_STD_MemRealloc
#define	OxEPG_StrDup				HLIB_STD_StrDup
#define	OxEPG_MemDup				HLIB_STD_MemDup
#define OxEPG_Free					HLIB_STD_MemFree

#define OxEX_Malloc					HLIB_STD_MemAlloc
#define OxEX_Calloc					HLIB_STD_MemCalloc
#define	OxEX_Realloc				HLIB_STD_MemRealloc
#define	OxEX_StrDup					HLIB_STD_StrDup
#define	OxEX_MemDup					HLIB_STD_MemDup
#define OxEX_Free					HLIB_STD_MemFree

#define OxMW_Malloc					HLIB_STD_MemAlloc
#define OxMW_Calloc					HLIB_STD_MemCalloc
#define	OxMW_Realloc				HLIB_STD_MemRealloc
#define	OxMW_StrDup					HLIB_STD_StrDup
#define	OxMW_MemDup					HLIB_STD_MemDup
#define OxMW_Free					HLIB_STD_MemFree

#define OxPAL_Malloc				HLIB_STD_MemAlloc
#define OxPAL_Calloc				HLIB_STD_MemCalloc
#define	OxPAL_Realloc				HLIB_STD_MemRealloc
#define	OxPAL_StrDup				HLIB_STD_StrDup
#define	OxPAL_MemDup				HLIB_STD_MemDup
#define OxPAL_Free					HLIB_STD_MemFree

#define OxSI_Malloc					HLIB_STD_MemAlloc
#define OxSI_Calloc					HLIB_STD_MemCalloc
#define	OxSI_Realloc				HLIB_STD_MemRealloc
#define	OxSI_StrDup					HLIB_STD_StrDup
#define	OxSI_MemDup					HLIB_STD_MemDup
#define OxSI_Free					HLIB_STD_MemFree
#define OxSI_Free_CB				HLIB_STD_MemFree_CB

#define OxCAS_Malloc				HLIB_STD_MemAlloc
#define OxCAS_Calloc				HLIB_STD_MemCalloc
#define	OxCAS_Realloc				HLIB_STD_MemRealloc
#define	OxCAS_StrDup				HLIB_STD_StrDup
#define	OxCAS_MemDup				HLIB_STD_MemDup
#define OxCAS_Free					HLIB_STD_MemFree


#endif /* __OBAMA_MEMCONFIG_H__ */


