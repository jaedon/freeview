/* -*- Mode: c; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 1995-2011 Opera Software ASA. All rights reserved.
 *
 * It is prohibited for you to distribute this file to anybody without Operas
 * separate written consent.
 */

#ifndef __NATIVE_MULTIVIEW_COMPONENT
#define __NATIVE_MULTIVIEW_COMPONENT

#include "NativeOOIFTypes.h"

#include "NativeAVComponent.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
  \defgroup jlabsMultiViewComponent jlabsMultiViewComponent class
  @{

   This is the native interface for the The jlabsMultiViewComponent class as
   defined in c.2.4.2.2 ( jcom 운용사항 )
 */

/**
   Opaque reference to an jlabsMultiViewComponent in the native platform.
 */
typedef void* jlabsMultiViewComponentHandle;

typedef struct {
	int	index;
	char *name;
	int 	compNum;
	void	**avcomponents;
} jlabsMultiViewComponentData;

/**
   Called when the jlabsMultiViewComponent instance is destroyed in the OOIF plugin.
   \param id jlabsMultiViewComponent instance identifier
   \return OOIF_RETURN_OK
 */
OOIFReturnCode native_jlabsMultiViewComponentReleaseHandle(jlabsMultiViewComponentHandle id);

/**
   Populate the data struct fields with the property values of an AV
   Component instance as defined in DAE 7.13.4.2.1 and
   7.14.4.2.1. The OOIF Plugin will handle all getter calls.
   \param identifier The handle of the jlabsMultiViewComponent
   \param data The data struct to be initialized
   \return OOIF_RETURN_OK
*/

OOIFReturnCode native_jlabsMultiViewComponentGetComponents(jlabsMultiViewComponentData *data, int *count, AVComponentHandle **handles);

OOIFReturnCode native_jlabsMultiViewComponentConvert( void *vpstSrc, jlabsMultiViewComponentData *pstDst);

OOIFReturnCode native_jlabsMultiViewComponentInitialize(jlabsMultiViewComponentHandle identifier, jlabsMultiViewComponentData *data);

/*@}*/

/** 
	Opl Struct Type conver to Navtive Struct Type 
*/

/** 
	Opl Strut get multiview string collection data 
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NATIVE_MULTIVIEW_COMPONENT */

