/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/
#ifndef __PAL_MMC_H__
#define __PAL_MMC_H__

/**************************************************************************************************/
#define _________PAL_MMC_Private_Include_________________________________________________
/**************************************************************************************************/
#include <htype.h>

/**************************************************************************************************/
#define _________PAL_MMC_Private_Macro_________________________________________________
/**************************************************************************************************/
#define MMC_MOUNT_PATH "/mnt/sd0"


typedef enum {
	ePAL_MMC_EVT_START = 0,

	ePAL_MMC_EVT_CONNECTED,
	ePAL_MMC_EVT_DISCONNECTED,

	ePAL_MMC_EVT_FORMAT_DONE,
	ePAL_MMC_EVT_FORMAT_FAIL,
	ePAL_MMC_EVT_MOUNT_DONE,
	ePAL_MMC_EVT_MOUNT_FAIL,
	ePAL_MMC_EVT_UMOUNT_DONE,
	ePAL_MMC_EVT_UMOUNT_FAIL,

	ePAL_MMC_EVT_MAX
} PalMmc_Event_e;

typedef struct {
	HINT32			nPort;
	HINT32			nMounted;
	HUINT32			uiDiskSize;
	HUINT32			uiAvailableSize;
	HUINT32			uiUsedSize;
} PalMmc_Info_e;

/**************************************************************************************************/
#define _________PAL_MMC_Private_Struct_________________________________________________
/**************************************************************************************************/
typedef void (*PalMmc_Callback_t) (PalMmc_Event_e dEvent, HINT32 dPort, HCHAR* pszName);

/**************************************************************************************************/
#define _________PAL_MMC_Public_Functions__________________________________________________________
/**************************************************************************************************/
HERROR PAL_MMC_Initialize (void);
void   PAL_MMC_RegisterNotifier(PalMmc_Callback_t fsCallback);
HERROR PAL_MMC_Mount(HINT32 Port,HCHAR* pszMountath);
HERROR PAL_MMC_UnMount(HINT32 Port);
HERROR PAL_MMC_GetMMCInfo(HINT32 Port,PalMmc_Info_e *pInfo);
HERROR PAL_MMC_Format(HINT32 Port);

#endif //end of __PAL_MMC_H__

