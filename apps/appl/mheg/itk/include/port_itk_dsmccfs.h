/**
	@file	  port_itk_dsmcc.h													\n
	@brief	  itk main header file for DSMCCFS porting layer					\n

	Description: ITK DSMCC File System Api Header file							\n
	Module: MW/mheg	/itk								 						\n

	Copyright (c) 2011 HUMAX Co., Ltd.											\n
	All rights reserved.														\n
*/


#ifndef _PORT_ITK_DSMCCFS_H_
#define _PORT_ITK_DSMCCFS_H_


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include "itk.h"
#include "itk_dsmccfs.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/**
  Container for file.
*/
typedef struct ITK_DsmccFile_t
{
	itk_dsmccfs_file_t					*dsmccfs_file;
} ITK_DsmccFile_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/**
  @remark   Initializes DSMCC File System
  @remark	type : sync
  @return	NONE
*/
void PORT_ITK_DSMCCFS_Init(void);


/**
  @remark   Creates DSMCC File Object.
  @remark	type : async
  @param	name			Fully qualified file name. (ex: "DSM://...")
  @return	A valid object if load request success.
  			NULL if load request failed.
*/
ITK_DsmccFile_t *PORT_ITK_DSMCCFS_LoadFile(const char *name);


/**
  @remark
  @remark	type : sync
  @return	ERR_OK if success,
            ERR_FAIL otherwise.
*/
HERROR PORT_ITK_DSMCCFS_Release(ITK_DsmccFile_t *dsmccFile);


#endif	/* end of ITK_DSMCCFS_H_ */

