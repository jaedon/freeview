/**
	@file	  port_itk_nvs.h													\n
	@brief	  itk nvs header file for NVS porting layer						\n

	Description: ITK에서 NVS Porting 에 사용되는 기능을 정의한 header 파일  	\n
	Module: mheg	/itk								 						\n

	Copyright (c) 2008 HUMAX Co., Ltd.											\n
	All rights reserved.														\n
*/


#ifndef _PORT_ITK_NVS_H
#define _PORT_ITK_NVS_H

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include "itk_nvs.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eMHEG_NVS_FS_DVR = 0x01,		/**< File System used for Freesat DVR */
	eMHEG_NVS_FS_NVM = 0x02,		/**< File System used for NVM interface */
	eMHEG_NVS_FS_FSA = 0x04,		/**< File System used for File System Acceleration */
	eMHEG_NVS_FS_RAM = 0x08,		/**< File System used for D-Book 6 RAM File System */
	eMHEG_NVS_FS_PST = 0x10,     	/**< File System used for True Persistent Storage */
	eMHEG_NVS_FACTORY_RESET = 0x80
} Mheg_Nvs_Fs_Type_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

void PORT_ITK_Nvs_ResetFs(Mheg_Nvs_Fs_Type_t MhegFsType);
void	PORT_ITK_Nvs_CreateObject(redkey_profile_t eItkProfile);


#endif 	/* end of _PORT_ITK_NVS_H */



