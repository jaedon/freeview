/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#ifndef ECURVE_H
#define ECURVE_H 1
#include "bignum.h"
#include "field.h"
#include "bigpriv.h"

ENTER_PK_NAMESPACE

Future_Struct(ecurve_t);

typedef DRM_BOOL DRM_CALL ec_exponentiation_t(const digit_t *, const digit_t *, const DRM_DWORD,
                                        digit_t *, const struct ecurve_t*,
                                        struct bigctx_t *f_pBigCtx);


typedef struct ecurve_t {
             const field_desc_t     *fdesc;
             digit_t           *a;
             digit_t           *b;
             digit_t        *generator;  
                                        
             digit_t           *gorder; 
             digit_t           *deallocate;
             ec_exponentiation_t *fexpon;
                                        
             DRM_BOOL               free_field;
                                           
             DRM_BOOL               biszero;  
             DRM_DWORD           ndigtemps;  
                                            
                                           
                                          
                                         
                                        
                                       
                                      
                                     
                                    
                                   
                                  
                                 
                                
             DRM_DWORD           lnggorder; 
                                           
             reciprocal_1_t     grecip;   
} ecurve_t;

/*
** This number (MAX_ECTEMPS) was 5, but has been increased to 8
** because we have gone from 160bit to 256 bit encryption
** and this assumes that the minimum size of digit_t/digit_t 
** types is a 32 bit type
**
** Maximum number of field temporaries needed
** by any EC addition/subtraction/doubling routine,
** including
**
**     ecaffine_addition,
**     ecaffine_addition_subtraction
**     ecaffine_on_curve
**     ecaffine_PPQ
**     ecaffine_random
*/
#define MAX_ECTEMPS 8

DRM_API DRM_BOOL DRM_CALL ecaffine_addition
      (const digit_t *, const digit_t *, digit_t *,
       const DRM_INT, const struct ecurve_t*, digit_t *, struct bigctx_t *f_pBigCtx);

DRM_API DRM_BOOL DRM_CALL ecaffine_addition_subtraction
      (const digit_t *, const digit_t *, digit_t *,
       digit_t *, const struct ecurve_t*, digit_t *, struct bigctx_t *f_pBigCtx);

DRM_API DRM_BOOL DRM_CALL ecaffine_exponentiation
          (const digit_t *, const digit_t *, const DRM_DWORD, digit_t *,
           const struct ecurve_t*, struct bigctx_t *f_pBigCtx);

DRM_API DRM_BOOL DRM_CALL ecaffine_is_infinite(const digit_t *, const struct ecurve_t*, struct bigctx_t *f_pBigCtx);

DRM_API DRM_BOOL DRM_CALL ecaffine_multiply_pm1
          (const digit_t *, digit_t *, const DRM_INT, const struct ecurve_t*, struct bigctx_t *f_pBigCtx);

DRM_API DRM_BOOL DRM_CALL ecaffine_negate(const digit_t *, digit_t *,
                                   const struct ecurve_t*, struct bigctx_t *f_pBigCtx);

DRM_API DRM_BOOL DRM_CALL ecaffine_on_curve(const digit_t *, const struct ecurve_t*,
                                     const DRM_CHAR*, digit_t *, struct bigctx_t *f_pBigCtx);

DRM_API DRM_BOOL DRM_CALL ecaffine_PPQ(const digit_t *, const digit_t *,
                                digit_t *, const DRM_INT,
                                const struct ecurve_t*, digit_t *, struct bigctx_t *f_pBigCtx);

DRM_API DRM_BOOL DRM_CALL ecaffine_random(digit_t *, const struct ecurve_t*,
                                   digit_t *, struct bigctx_t *f_pBigCtx);

DRM_API DRM_BOOL DRM_CALL ecaffine_set_infinite(digit_t *,
                                         const struct ecurve_t*, struct bigctx_t *f_pBigCtx);

DRM_API DRM_BOOL DRM_CALL ec_initialize(const digit_t *, const digit_t *,
                                const field_desc_t*, struct ecurve_t*, struct bigctx_t *f_pBigCtx, struct bigctx_t*);

DRM_API DRM_BOOL DRM_CALL ec_free(struct ecurve_t*, struct bigctx_t *f_pBigCtx);   

DRM_EXTERN_INLINE DRM_BOOL DRM_CALL ecaffine_exponentiation_tabular
          (const digit_t *, const DRM_DWORD, const DRM_DWORD, const digit_t *,
           const DRM_DWORD, digit_t *, const struct ecurve_t*, struct bigctx_t *f_pBigCtx);

DRM_API DRM_BOOL DRM_CALL ecaffine_table_construction(const digit_t *, const DRM_DWORD,
                                               const DRM_DWORD, digit_t *,
                                               const struct ecurve_t*, struct bigctx_t *f_pBigCtx);

EXIT_PK_NAMESPACE

#endif