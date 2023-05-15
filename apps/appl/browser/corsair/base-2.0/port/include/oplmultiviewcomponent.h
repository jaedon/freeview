/**************************************************************************************/
/**
 * @file oplmultiviewcomponent.h
 *
 * Video Broadcast
 *
 * @author  seung bum kim(sbkim@humaxdigital.com)
 * @date    2012/08/10
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/

#ifndef __OPL_MULTIVIEWCOMPONENT_H__
#define __OPL_MULTIVIEWCOMPONENT_H__

#include "bpltype.h"

/******************************************************************
	Define
******************************************************************/
#define OPL_MULTI_VIEW_COMPONENT_TEXT_SIZE				17
#define OPL_MULTI_VIEW_MAX_COMPONENT					16

/******************************************************************
	Enum
 ******************************************************************/

/******************************************************************
	Typedef
******************************************************************/
typedef struct
{
	HINT32				index;
	HINT32				compNum;
	OplAVComponent_t		hComponents[OPL_MULTI_VIEW_MAX_COMPONENT];
	HCHAR		 		szName[OPL_MULTI_VIEW_COMPONENT_TEXT_SIZE];
} OplMultiViewComponent_t;

/******************************************************************
	Interfaces
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
};
#endif

#endif

