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
