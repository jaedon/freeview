/**************************************************************************************/
/**
 * @file oplsearchresult.h
 *
 * The meta search result interfaces
 *
 * @author  ultracat(leecw@humaxdigital.com)
 * @date    2011/11/17
 * @attention   Copyright (c) 2011 Humax - All rights reserved.
 *
 **************************************************************************************
 **/


#ifndef	__OPL_SEARCHRESULT_H__
#define	__OPL_SEARCHRESULT_H__

#include "bpltype.h"

#include "oplchannel.h"
#include "oplprogramme.h"
/******************************************************************
	Define
******************************************************************/

/******************************************************************
	Enum
 ******************************************************************/

/******************************************************************
	Typedef
******************************************************************/


/******************************************************************
	Interfaces
******************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create the VoD instance
 *
 * @return the handle of VoD instance on success, and NULL on error
 * // TODO: This is temporary!!!!!. We have to implement, right way....
 */
BPLStatus	DUMMY_OPL_SEARCHRESULT_GetProgramme(void *ch, void *programme);



#ifdef __cplusplus
};

/******************************************************************
	class XXXX
******************************************************************/

#endif

#endif


