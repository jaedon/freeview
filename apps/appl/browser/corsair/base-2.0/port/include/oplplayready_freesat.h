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

OPL_BOOL	OPL_FSAT_PR_SetNeedLicenceURL_RegisterCallback(void (*cbFunc)(const char *value));
OPL_BOOL	OPL_FSAT_PR_SetLicenceURL(const char *licenseURL);

#ifdef __cplusplus
};
#endif	//__cplusplus

#endif	//__OPLQUERY_H__

