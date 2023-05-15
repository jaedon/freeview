/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
/*
       File kdiv.c.    Version 10 April 2002
*/

#define DRM_BUILDING_KDIV_C
#include "bignum.h"
#include "fieldpriv.h"
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL Kdiv
        (const digit_t *f1,        // f3 = f1/f2
         const digit_t *f2,
         digit_t*  f3,
         const field_desc_t *fdesc,
         digit_t *supplied_temps,
         struct bigctx_t *f_pBigCtx)
{
    DRM_BOOL OK = TRUE;
    digit_tempinfo_t tempinfo;
    const DRM_DWORD elng = fdesc->elng;

    tempinfo.address = supplied_temps;
    tempinfo.need_to_free = FALSE;
    tempinfo.nelmt = fdesc->ndigtemps_arith;

    OK = OK && possible_digit_allocate(&tempinfo, f_pBigCtx);

    if (OK) {
        digit_t *f2inv = tempinfo.address;
        digit_t *ftemps = f2inv + elng;

        OK = OK && Kinvert(f2, f2inv, fdesc, ftemps, f_pBigCtx);
        OK = OK && Kmul(f1, f2inv, f3, fdesc, ftemps, f_pBigCtx);
    }
    if (tempinfo.need_to_free) {
        Free_Temporaries(tempinfo.address, f_pBigCtx);
    }
    return OK;
} /* Kdiv */

EXIT_PK_NAMESPACE_CODE;
