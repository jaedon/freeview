/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#ifndef __BIGPRIV_H
#define __BIGPRIV_H 1
/*
      File bigpriv.h.   Version 20 September, 2002

      Declarations accessible to bignum library but invisible to application.
      Also see fieldpriv.h
*/

#include "bignum.h"


ENTER_PK_NAMESPACE

#if defined (_MSC_VER)

    #pragma warning(disable: 4146 4514) 
         /* 4146 -- unary minus operator applied
            to unsigned type, result still unsigned.
            4514 -- unreferenced inline function
          */
#endif



/*
     Some routines allow the caller to supply temps, but
     accept a NULL argument to say "Allocate them yourself!".
     possible_digit_allocate assists in doing the allocate
     if the caller passed NULL.
*/

typedef struct digit_tempinfo_t {
    digit_t *address;  
                        
                         
    DRM_DWORD nelmt;      
    DRM_BOOL  need_to_free;
                            
} digit_tempinfo_t;

DRM_API DRM_BOOL DRM_CALL possible_digit_allocate(digit_tempinfo_t*,
                                                 const DRM_CHAR*, struct bigctx_t *f_pBigCtx);

DRM_API DRM_BOOL DRM_CALL modmul_choices1(mp_modulus_t*, DRM_INT*);

DRM_API DRM_BOOL DRM_CALL modmul_from_right_default( 
    IN      const digit_t      *a,
    IN      const digit_t      *b,
    OUT           digit_t      *c,
    IN      const mp_modulus_t *pmodulo,   
    IN OUT        digit_t      *temps );

DRM_API DRM_BOOL DRM_CALL modmul_from_right_default_modulo8( 
    IN      const digit_t      *a,
    IN      const digit_t      *b,
    OUT           digit_t      *c,
    IN      const mp_modulus_t *pmodulo,   
    IN OUT        digit_t      *temps );

EXIT_PK_NAMESPACE


#endif
