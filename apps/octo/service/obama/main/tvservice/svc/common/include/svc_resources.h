/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  svc_resources.h
	@brief

	Description:

	Module: MWC/RM			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___SVC_RESOURCES_H___
#define	___SVC_RESOURCES_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <rlib.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HBOOL				 bAllocated;
	HUINT32				 ulResourceId;			// HUINT32
//	HUINT32				 eWeight;				// RM_RscWeight_t
//	HUINT32				 eUsage;				// RM_RscProperty_e
//	HUINT32		 ulResourceId;
//	RM_RscWeight_t		 eWeight;
//	RM_RscProperty_e	 eUsage;
} SvcRsc_Item_t;

typedef struct
{
	Handle_t			 hAction;
	SvcRsc_Item_t		 astRscItem[eRxRSCTYPE_MAX];
} SvcRsc_Info_t;

#endif /* ___SVC_RESOURCES_H___ */

