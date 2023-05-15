/**************************************************************************************/
/**
 * @file NativeJlabsContentSearch.cpp
 *
 * jlabsDTTChanne module
 *
 * @author 
 * @date    
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#include "NativeJlabsContentSearch.h"
#include "macros.h"
#include "opldmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	__TRACE()					do{OOIF_LOG_DUMMY("[%s:%d] trace\n", __FUNCTION__, __LINE__);}while(0)
#define	__RETURN_ERROR_IF(cond)		do{if(cond){OOIF_LOG_ERROR("[%s:%d] error \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_ERROR;}}while(0)
#define	__RETURN_NULL_IF(cond)		do{if(cond){OOIF_LOG_WARNING("[%s:%d] null \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_NULL;}}while(0)
#define	__RETURN_UNDEFINED_IF(cond)	do{if(cond){OOIF_LOG_WARNING("[%s:%d] undefined \'%s\'\n", __FUNCTION__, __LINE__, #cond);return OOIF_RETURN_VALUE_UNDEFINED;}}while(0)
#define	__RETURN_OK_IF(cond)		do{if(cond)return OOIF_RETURN_ERROR;}while(0)


#define JLABS_CONTENTSEARCH_TRACE     OOIF_LOG_DEBUG("\tjlabsContentResults::%s(%d)\n", __FUNCTION__, __LINE__)

OOIFReturnCode native_jlabsContentSearchGetQuery(jlabsContentSearchHandle identifier, QueryHandle *retval)
{
	JLABS_CONTENTSEARCH_TRACE;
	//Dummy Implementation TODO

	*retval = NULL;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsContentSearchGetResult(jlabsContentSearchHandle identifier, jlabsContentResultsHandle *retval)
{
	JLABS_CONTENTSEARCH_TRACE;
	//Dummy Implementation TODO

	*retval = NULL;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsContentSearchCreateQuery(const char *field, OOIFNumber comparison, const char *value, QueryHandle *retval)
{
	JLABS_CONTENTSEARCH_TRACE;
	//Dummy Implementation TODO

	*retval = NULL;
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsContentSearchOrderBy(jlabsContentSearchHandle identifier, const char *field, OOIFBoolean ascending)
{
	JLABS_CONTENTSEARCH_TRACE;
	//Dummy Implementation TODO
	
	return OOIF_RETURN_OK;
}

OOIFReturnCode native_jlabsContentSearchSetQuery(jlabsContentSearchHandle identifier, QueryHandle *query)
{
	JLABS_CONTENTSEARCH_TRACE;
	//Dummy Implementation TODO
	
	return OOIF_RETURN_OK;
}
