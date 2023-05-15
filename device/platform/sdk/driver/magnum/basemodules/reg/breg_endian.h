/***************************************************************************
 *	   Copyright (c) 2003, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: breg_endian.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 1/16/04 10:37a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/reg/breg_endian.h $
 * 
 * Hydra_Software_Devel/5   1/16/04 10:37a vsilyaev
 * PR 9372: Replaced BREG_BEXX on BE and BREG_LEXX on LE plaforms with no-
 * ops.
 * 
 * Hydra_Software_Devel/4   4/4/03 3:16p marcusk
 * Updated references of BSTD_XXX_ENDIAN to BSTD_ENDIAN_XXX
 * 
 * Hydra_Software_Devel/3   3/31/03 8:57a marcusk
 * Added comments.
 * 
 * Hydra_Software_Devel/2   3/5/03 4:19p marcusk
 * Fixed minor issues (got it to compile).
 * 
 * Hydra_Software_Devel/1   3/5/03 3:34p marcusk
 * Initial version.
 *
 ***************************************************************************/

/*= Module Overview ********************************************************
This module supplies standard macros that are used to correct endianess for
hardware registers that do not match the endian-ness of the CPU.  An example
of this would be a register that is always big endian regardless of whether
the CPU was little endian or big endian.

It is recommended that these macros be used on a variable after
the BREG_ReadXX() functions are called, NOT around the functions themselves.
This is to prevent executing the BREG_ReadXX() multiple times inside the
macro (which could also cause inconsistent results if the register happens
to change while executing the macro).

These macros depend on the BSTD_CPU_ENDIAN define to be set properly.

For a big endian CPU: BSTD_CPU_ENDIAN should equal BSTD_ENDIAN_BIG
For a little endian CPU: BSTD_CPU_ENDIAN should equal BSTD_ENDIAN_LITTLE
***************************************************************************/

#ifndef BREG_ENDIAN_H
#define BREG_ENDIAN_H

#ifdef __cplusplus
extern "C" {
#endif

/* 
This macro is used to swap bytes of a 32 bit value.
*/
#define BREG_SWAP32( a )  do{a=((a&0xFF)<<24|(a&0xFF00)<<8|(a&0xFF0000)>>8|(a&0xFF000000)>>24);}while(0)

/* 
This macro is used to swap bytes of a 16 bit value.
*/
#define BREG_SWAP16( a )  do{a=((a&0xFF)<<8|(a&0xFF00)>>8);}while(0)

#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG

/* 
This macro is used to ensure that a 32-bit value is properly formatted for
a big endian register access.
*/
#define BREG_BE32( value ) 

/* 
This macro is used to ensure that a 16-bit value is properly formatted for
a big endian register access.
*/
#define BREG_BE16( value )

/* 
This macro is used to ensure that a 32-bit value is properly formatted for
a little endian register access.
*/
#define BREG_LE32( value ) BREG_SWAP32( value )

/* 
This macro is used to ensure that a 16-bit value is properly formatted for
a little endian register access.
*/
#define BREG_LE16( value ) BREG_SWAP16( value )

#else
#define BREG_BE32( value ) BREG_SWAP32( value )
#define BREG_BE16( value ) BREG_SWAP16( value )
#define BREG_LE32( value )
#define BREG_LE16( value )
#endif

#ifdef __cplusplus
}
#endif
 
#endif
/* End of File */



