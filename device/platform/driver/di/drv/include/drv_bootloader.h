/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2014/11/22
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

#ifndef _DRV_BLDRV_H_
#define _DRV_BLDRV_H_

/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/




/**********************************************************************************************************/
#define ____DEFINE________________________
/**********************************************************************************************************/
typedef enum
{
	eDrvLOADER_IMAGE_Boot_Normal,
	eDrvLOADER_IMAGE_Boot_NormalBackup,
	eDrvLOADER_IMAGE_Boot_Factory,
	eDrvLOADER_IMAGE_Boot_Ota,
	eDrvLOADER_IMAGE_Boot_OtaBackup,
	eDrvLOADER_IMAGE_Boot_NormalInFactory,
	eDrvLOADER_IMAGE_Boot_MAX
} DrvLOADER_IMAGE_Boot_e;

typedef enum
{
	eDrvLOADER_IMAGE_OS_Linux,
	eDrvLOADER_IMAGE_OS_Linux_Ramdisk,			/* need to return Ramdisk Size to DI */
//	eDrvLOADER_IMAGE_OS_Linux_LoadRamdisk,		/* need to return Ramdisk Size to DI */
	eDrvLOADER_IMAGE_OS_OS21,
	eDrvLOADER_IMAGE_OS_OS20,
	eDrvLOADER_IMAGE_OS_MAX
} DrvLOADER_IMAGE_OS_t;


typedef enum
{
	eDrvLOADER_DOWNLOAD_NONE = 0,
	eDrvLOADER_DOWNLOAD_BY_OTA,			/* via Linear/Network/Flash (CAB, SAT, TER, FTP, Background)*/
	eDrvLOADER_DOWNLOAD_BY_USB,			/* via USB disk */
	eDrvLOADER_DOWNLOAD_BY_SERIAL,		/* via Serial */
	eDrvLOADER_DOWNLOAD_BY_EMERGENCY,	/* for Emergency */
	eDrvLOADER_DOWNLOAD_BY_TFTP,			/* via Network(TFTP), for debug only */
	eDrvLOADER_DOWNLOAD_BY_USBDUMP,		/* for dump2usb debug only */

	eDrvLOADER_DOWNLOAD_DUMMY
} DrvLOADER_DOWNLOAD_TYPE_e;

typedef enum tagDRV_LOADER_BootOption_e
{
	eDRV_BOOTOPTION_None			= 0x000000,

	/*
	 * add BootParam
	 * CableModem MAC Addresss
	 */
	eDRV_BOOTOPTION_CM_MACADDR	= 0x000001,

	/* External RSA public key */
	eDRV_BOOTOPTION_RSA_KEY	= 0x000002,

	/* External Application */
	eDRV_BOOTOPTION_EXT_APP		= 0x000004,

	/* ex */
	eDRV_BOOTOPTION_xxxx3		= 0x000008,
	eDRV_BOOTOPTION_xxxx4		= 0x000010,

	/* all option */
	eDRV_BOOTOPTION_Max			= 0xFFFFFF
} DRV_LOADER_BootOption_e;

#define DEF_MAX_ETHNAME_LENGTH 128
#define DEF_MAX_EXTAPPNAME_LENGTH 16

typedef struct tagDRV_LOADER_BootOption
{
	HINT8 						ethernetName[DEF_MAX_ETHNAME_LENGTH];
	HINT8 						extAppName[DEF_MAX_EXTAPPNAME_LENGTH];
	DRV_LOADER_BootOption_e 	eOption;
} DRV_LOADER_BootOption_t;

/**********************************************************************************************************/
#define ____TYPEDEF________________________
/**********************************************************************************************************/


typedef HINT32	DRV_BLOADER_HANDLE;
#define DRV_BLOADER_HANDLE_NULL	(-1)

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_PROTOTYPE_______________________
/**********************************************************************************************************/
extern	DRV_BLOADER_HANDLE	DRV_BLOADER_open(HCHAR *name);

extern 	DRV_Error	DRV_BLOADER_SetBootOption(DRV_LOADER_BootOption_t	*pstOpt);
extern	DRV_Error	DRV_BLOADER_GetBootOption(DRV_LOADER_BootOption_t	*pstOpt);

extern	DRV_Error	DRV_BLOADER_close(DRV_BLOADER_HANDLE handle);
extern	DRV_Error	DRV_BLOADER_Init(void);
extern	DRV_Error	DRV_BLOADER_ioctl(DRV_BLOADER_HANDLE handle, HUINT32 ioctlnum, void *buffer, HUINT32 length, HUINT32 *retlen, HUINT64 offset);
extern	DRV_Error	DRV_BLOADER_inpstat(DRV_BLOADER_HANDLE handle);
extern	HINT32		DRV_BLOADER_docommands(const HCHAR *buf);
extern	DRV_Error	DRV_BLOADER_set_envdevice(HCHAR *name);
extern	DRV_Error	DRV_BLOADER_free_envdevname(void);
extern	DRV_Error	DRV_BLOADER_getenv(HCHAR *name, HCHAR *dest, HINT32 destlen);
extern	HINT32		DRV_BLOADER_console_status(void);
extern	DRV_Error	DRV_BLOADER_console_read(HCHAR *buffer, HINT32 len);
extern	DRV_Error  	DRV_BLOADER_console_readline(HCHAR *prompt, HCHAR *str, HINT32 len);

extern	HINT32		DRV_BLOADER_read(DRV_BLOADER_HANDLE handle, HUINT8 *buffer, HINT32 length);
extern	HINT32		DRV_BLOADER_readblk(DRV_BLOADER_HANDLE handle, HUINT64 offset, HUINT8 *buffer, HINT32 length);
extern	HINT32		DRV_BLOADER_write(DRV_BLOADER_HANDLE handle, HUINT8 *buffer, HINT32 length);
extern	HINT32		DRV_BLOADER_writeblk(DRV_BLOADER_HANDLE handle, HUINT64 offset, HUINT8 *buffer, HINT32 length);

extern	HUINT32		DRV_BLOADER_getticks(void);

extern	void 		DRV_BLOADER_DeInit(void);

extern	DRV_Error 	CMD_DRV_BLOADER_Init(void);

extern	HINT32		DRV_BLOADER_Autobooting(DrvLOADER_IMAGE_Boot_e etImageBoot);
extern	HINT32		DRV_BLOADER_Ramboot(void *elfImageBuff, HUINT32 elfSize, DrvLOADER_IMAGE_Boot_e etImageBoot);


/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_PROTOTYPE______________________
/**********************************************************************************************************/


#endif /* _DRV_BLDRV_H_ */


