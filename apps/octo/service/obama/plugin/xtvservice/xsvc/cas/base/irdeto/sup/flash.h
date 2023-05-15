/*TITLE FLASH Driver API */
/*SUBTITLE Header      */
/*****************************************************************************
******************************************************************************
**                                                                          **
**   (c) 1998 IRDETO Consultants  B.V.  All Rights Reserved.                **
**   This Software Code is provided "AS IS" without a warranty of any kind. **
**   All express or implied representations and warranties, including any   **
**   implied warranty of merchantability, fitness for a particular purpose, **
**   non-infringement are hereby excluded. All liabilities are disclaimed,  **
**   including express or implied liabilities, direct or indirect, and      **
**   consequential damages.                                                 **
**                                                                          **
*****************************************************************************/
/*****************************************************************************
*                                                                            *
*        File : FLASH.H                                                      *
*                                                                            *
* Description : Contains flash definitions                                   *
*                                                                            *
*      Author : W Keyser/A Rossouw                                           *
*                                                                            *
*      Status : Draft                                                        *
*                                                                            *
*     History :                                                              * 
* ------------ ------------------------------------------------------------  *
*  25/06/99    Jon Darrington                                                *
*              Minimal definitions for Mindport STB support code             *
*  02/07/98    Rajesh Madhanlala                                             *
*              Re-arranged boot control block, CRC control block structs.    *
*              Minimal Multiple image support.                               * 
*  09/07/96    A_Rossouw Revision 1.1 10:21:28                               *
*              Irdeto's OpenTV High Level Port, Alpha release                *
*                                                                            *
*****************************************************************************/
#ifndef _FLASH_H_
#define _FLASH_H_

#include "sysdefs.h"
//#include "flashfun.h"

/**** DEFINITIONS ************************************************************/
#define RANDOM_NUMBER_LEN      (4)
#define SERIAL_NUMBER_SIZE     (4)
#define FLASH_BB_KEY_LEN       (SERIAL_NUMBER_SIZE + RANDOM_NUMBER_LEN)

#if 0
/* jfd 25/6/99 Note that FLASH_BB_VERSION_STRUCTURE does not imply a
   specific ordering or layout of the boot block control block. Each
   item should therefore be copied individually from the boot block control
   block (as opposed to a block copy of the entire structure). */
typedef struct
{
   BYTE           bManufactureCode;                    
   BYTE           bHW_Version;                         
   BYTE           bLDR_Version_Major;                  
   BYTE           bLDR_Version_Minor;                  
   BYTE           abSerialNumber[SERIAL_NUMBER_SIZE];  
   BYTE           abRandomNumber[RANDOM_NUMBER_LEN];   
   WORD           ulCRC;                               
} FLASH_BB_VERSION_STRUCTURE;                         


/*****************************************************************************/
/* jfd 25/6/99 stFlashLoaderData shall be declared within manufacturer source
   and filled with data from the boot block control block during loader
   initialisation */
extern FLASH_BB_VERSION_STRUCTURE stFlashLoaderData;
#endif
// flash functions		//kimdu
//extern FLASHFUNCTIONSTYPE	_flash1Fun, _flash2Fun;

#endif      /* _FLASH_H_ */
