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

/*This file must be syncronized with application ir_loader.h */
#ifndef ___IR_LOADER___
#define ___IR_LOADER___

#include "product/uc_setting.h"

typedef struct{
	unsigned short usCRC16;
	unsigned char	 ucManufacturerCode;
	unsigned char  ucHardwareCode;
	unsigned char  ucLoaderMajor;
	unsigned char  ucLoaderMinor;
	unsigned char  ucLoaderType;
	unsigned long  ulSerialNumber;
	unsigned long  ulRandomNumber;
	unsigned char  aucPadding[9];
}Boot_Block_Control_Block; // 24byte


typedef unsigned char IHL_U8;
typedef signed char IHL_S8;
typedef unsigned short IHL_U16;
typedef signed short IHL_S16;
typedef IHL_U8 *IHL_PU8;

typedef struct
{
   IHL_U16 u16SysID;          /* System ID                                 */
   IHL_S16 s16SysIDValid;     /* 0=u16SysID valid, -1=u16SysID invalid     */
   IHL_U16 u16KeyVer;         /* Key version                               */
   IHL_S16 s16KeyVerValid;    /* 0=u16KeyVer valid, -1=u16KeyVer invalid   */
   IHL_U16 u16SigVer;         /* Signature version                         */
   IHL_S16 s16SigVerValid;    /* 0=u16SigVer valid, -1=u16SigVer invalid   */
   IHL_U16 u16Variant;        /* Operator variant                          */
   IHL_S16 s16VariantValid;   /* 0=u16Variant valid, -1=u16Variant invalid */
   IHL_U8  u8BlockFailMask;   /* Bit mask of failed Irdeto NVRAM partition */
                              /* blocks (block0 = bit0)                    */
} IHL_STATUS, *IHL_PSTATUS;

#endif // end of file

