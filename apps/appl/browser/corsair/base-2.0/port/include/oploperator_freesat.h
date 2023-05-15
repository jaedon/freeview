/**************************************************************************************/
/**
 * @file oplquery.h
 *
 *
 *
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPLOPERATOR_H__
#define __OPLOPERATOR_H__

#include	"opltype.h"

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

typedef	enum
{
	eOpFSAT_MetaEventType_Xml,
	eOpFSAT_MetaEventType_Img
} OPL_OPERATOR_FSAT_MetaEventType_e;

typedef	enum
{
	eOpFSAT_MetaFileType_ChIcon,
	eOpFSAT_MetaFileType_Image,
	eOpFSAT_MetaFileType_All
} OPL_OPERATOR_FSAT_MetaFileType_e;

OPL_BOOL	OPL_Operator_FSAT_InitMetaEventListner();
OPL_BOOL	OPL_Operator_FSAT_RegisterMetaEventListener(void (*cbFunc)(int eType, int subParam));
OPL_BOOL	OPL_Operator_FSAT_RegisterMhegEventListener(void (*cbFunc)(int eType, int subParam));



#ifdef __cplusplus
};
#endif	//__cplusplus

#endif	//__OPLQUERY_H__

