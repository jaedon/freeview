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
/*****************************************************************************
******************************************************************************
**                                                                          **
**   (c) 1999 Mindport-STB  All Rights Reserved.                            **
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
*        File : SYSDEFS.H                                                    *
*                                                                            *
* Description : This is a skeleton of the file required to define basic      *
*               types for the manufacturer's platform. This file should be   *
*               checked and, if necessary, modified by the manufacturer      *
*               prior to use.                                                *
*                                                                            *
*      Author : Jon Darrington                                               *
*                                                                            *
*      Status : Development                                                  *
*                                                                            *
*     History : Version 1.0 - 25 August 1999                                 *
*                                                                            *
*****************************************************************************/

#ifndef _SYSDEFS_H_
#define _SYSDEFS_H_

/*
** Standard type definitions.
** THIS IS THE RESPONSIBILITY OF THE MANUFACTURER
*/


/*
** VOID
** Leave defined as 'void'
*/
#ifndef VOID  
#define VOID void
#endif

/*
** BOOL
** Min: 0
** Max: 1
*/
#ifndef BOOL
#define BOOL	unsigned char
#endif

/*
** BYTE
** Min: 0
** Max: 255
*/
#ifndef BYTE
#define BYTE 	unsigned char
#endif

/*
** WORD
** Min: 0
** Max: 65535
*/
#ifndef WORD
#define WORD 	unsigned short
#endif

/*
** INT
** Min: -2147483648
** Max: 2147483647
*/
#ifndef INT
#define INT signed int
#endif

/*
** ULONG
** Min: 0
** Max: 4294967295
*/
#ifndef ULONG
#define ULONG unsigned long
#endif

/*
** LONG
** Min: -2147483648
** Max: 2147483647
*/
#ifndef LONG
#define LONG signed long
#endif

/*
** Pointer type definitions derived from standard defs above.
** DO NOT MODIFY THESE
*/
typedef VOID   			*PVOID;
typedef BYTE			*PBYTE;


/*
** Boolean definitions
** DO NOT MODIFY THESE
*/
#ifndef TRUE
#define TRUE    ((BOOL)1)
#endif /* TRUE */

#ifndef FALSE
#define FALSE   ((BOOL)0)
#endif /* FALSE */

#endif /* _SYSDEFS_H_ */
