/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_cas_si_des.h						\n
	@brief	  cas module header file		\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SVC_CAS_SI_DES_H__
#define	__SVC_CAS_SI_DES_H__

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************************
* Header Files :  헤더 파일은 가능한 .c 에서 include 할 것 !
********************************************************************/
#define ___01_Header_Files___
#include <octo_common.h>
#include <svc_cas.h>

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___02_Extern_Vars_Func_Prototypes___


/********************************************************************
* Module Internal Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Module_Internal_Definitions___

// Descriptor
typedef struct tagBASE_DES_FORM
{
	HUINT8		ucDesTag;
} svcCas_BaseDesForm_t;

#ifdef __cplusplus
}
#endif


#endif /* !__SVC_CAS_SI_DES_H__ */
