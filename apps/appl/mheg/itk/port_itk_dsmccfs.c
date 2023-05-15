/**
	@file	  port_itk_dsmcc.c													\n
	@brief	  itk file for DSMCCFS porting layer								\n

	Description: ITK DSMCC File System Api										\n
	Module: MW/mheg	/itk								 						\n

	Copyright (c) 2011 HUMAX Co., Ltd.											\n
	All rights reserved.														\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include "port_itk_main.h"
#include "port_itk_dsmccfs.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static itk_dsmccfs_t			*s_stDsmccFs = NULL;


/*******************************************************************/
/********************      Function Prototypes   *******************/
/*******************************************************************/

/*******************************************************************/
/********************      Local Functions       *******************/
/*******************************************************************/
static void local_itk_DSMCCFS_File_Callback(void *context, struct itk_dsmccfs_file_t *dsmccfs_file, itk_errcode_t result, itk_dsmccfs_file_data_t *fileData)
{
	ITK_DsmccFile_t *dsmcc_file = (ITK_DsmccFile_t *)context;

	switch(result)
	{
		case ITKE_OK:
			HxLOG_Debug("MHEG_DSMCC : Load Callback - size(%d), total(%d)\n",fileData->size, fileData->total);

			if(fileData->size == fileData->total)
			{
				// Normal Case : Need not copy this data (itk_dsmccfs_file_data_t structure and data will persist in memory until the file is released.)
				// TODO: check memory leak.
				PORT_ITK_PostMsg(eMHEG_DSMCCFS_FILE_DATA, 0/*ViewId*/, (HINT32)fileData->data, (HINT32)fileData->size, (HINT32) dsmcc_file, sizeof(ITK_DsmccFile_t));
				//HLIB_STD_MemFree(pEvtWindow);
			}
			else
			{
				// ToDo : This is abnormal case. This should not be occurred in RCT function.
				//	This occurs when resources are limited, so MAKING LARGE BUFFER IS NOT GOOD IDEA.
				//	We should temporary copy this data, and should make extension APIs for handle this.
				// TODO: check memory leak.
				PORT_ITK_PostMsg(eMHEG_DSMCCFS_FILE_DATA, 0/*ViewId*/, 0, 0, (HINT32) dsmcc_file, sizeof(ITK_DsmccFile_t));
				//HLIB_STD_MemFree(pEvtWindow);
			}
			return;

		// case ITKE_FAIL:
		// case ITKE_NOT_SUPPORTED:
		// case ITKE_NOT_READY:
		// case ITKE_NOT_FOUND:
		// case ITKE_NO_RESOURCE:
		// case ITKE_TIMEOUT:
		default:
			HxLOG_Debug("MHEG_DSMCC : Load Error (%d)\n", result);
			// TODO: check memory leak.
			PORT_ITK_PostMsg(eMHEG_DSMCCFS_FILE_DATA, 0/*ViewId*/, 0, 0, (HINT32) dsmcc_file, sizeof(ITK_DsmccFile_t));
			break;
	}
}

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

void PORT_ITK_DSMCCFS_Init(void)
{
	s_stDsmccFs = newItkDsmccFs(NULL);	

	HxLOG_Assert(s_stDsmccFs);
}

ITK_DsmccFile_t *PORT_ITK_DSMCCFS_LoadFile(const char *name)
{
	ITK_DsmccFile_t		*dsmcc_file = NULL;
	itk_dsmccfs_file_t		*dsmccfs_file = NULL;
	itk_errcode_t			errcode;

	if(s_stDsmccFs == NULL || name == NULL)
	{
		HxLOG_Debug(s_stDsmccFs == NULL ? "MHEG_DSMCC : DsmccFs is not Ready.\n" : "name is NULL");
		return NULL;
	}

	HxLOG_Debug(" MHEG_DSMCC : File Name- %s \n", name);
		
	// Create a DsmccFile (Porting Layer) Object
	dsmcc_file = HLIB_STD_MemAlloc(sizeof(ITK_DsmccFile_t));
	if(dsmcc_file != NULL)
	{
		// Create a DsmccfsFile (Redkey Engine) Object
		dsmccfs_file = s_stDsmccFs->newFile(s_stDsmccFs, name, local_itk_DSMCCFS_File_Callback, (void *)dsmcc_file);
		if(dsmccfs_file == NULL)
		{
			HxLOG_Error("MHEG_DSMCC : File Create Error - %s\n", name);
			HLIB_STD_MemFree(dsmcc_file);
			return NULL;
		}

		dsmcc_file->dsmccfs_file = dsmccfs_file;
	
		// Load data request
		errcode = dsmccfs_file->load(dsmccfs_file);
		switch(errcode)
		{
			case ITKE_OK:
				break;

			// case ITKE_FAIL:
			// case ITKE_NOT_SUPPORTED:
			// case ITKE_NOT_READY:
			// case ITKE_NOT_FOUND:
			// case ITKE_NO_RESOURCE:
			// case ITKE_TIMEOUT:
			default:
				HxLOG_Error("MHEG_DSMCC : Load request Error (%d).\n", errcode);
				dsmccfs_file->release(dsmccfs_file);
				HLIB_STD_MemFree(dsmcc_file);
				return NULL;
		}
	}
	
	return dsmcc_file;
}


HERROR PORT_ITK_DSMCCFS_Release(ITK_DsmccFile_t *dsmccFile)
{
	if(dsmccFile != NULL )
	{
		if(dsmccFile->dsmccfs_file != NULL)
		{
			dsmccFile->dsmccfs_file->release(dsmccFile->dsmccfs_file);
			dsmccFile->dsmccfs_file = NULL;
		}
		
		HLIB_STD_MemFree(dsmccFile);
	}

	return ERR_OK;
}

