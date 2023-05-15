/*
 * (c) 2011-2012 Humax Co., Ltd. 
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies 
 * of the Software including all documentation. This License will terminate immediately without notice from Humax 
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies 
 * of the Software and all documentation.  

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice. 
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court, 
 * in the Republic of Korea.
*/
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
