/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include "bignum.h"

ENTER_PK_NAMESPACE_CODE;

/*
      File digit_ogcd.c.   Version 06 October 2002.
*/
/***************************************************************************/
DRM_BOOL DRM_CALL digit_ogcd(const digit_t d1, const digit_t d2, digit_t *pgcd)
/*
        Return *pgcd = GCD(d1, d2), where at least one of d1, d2 is odd.
*/
{
    static const DRM_BYTE trailing_zeros[16]
            = {4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};
#define extract_odd_part(n) do {n >>= trailing_zeros[n & 15];} while ((n&1)==0)

    digit_t n1 = d1, n2 = d2;
    DRM_BOOL OK = TRUE;

    if (IS_EVEN(n1|n2)) {  // Both even
        OK = FALSE;
        SetMpErrno_clue(MP_ERRNO_INVALID_DATA,
                        "digit_ogcd");
    } else if  (n1 == 0 || n2 == 0) {
        n1 = n1 + n2;    // Return GCD(n1, n2) = n1 + n2
    } else {
        extract_odd_part(n1);  extract_odd_part(n2);

        while (n1 != n2) {
            const DRM_LONG sh1 = 1 + trailing_zeros[((n1 ^ n2) >> 1) & 15];
                                                      // Lower bound
            const digit_t n12max = MAX(n1, n2);  // Hope compiler avoids
                                            // branch misprediction
            n1 = (n1 ^ n2) ^ n12max;        // MIN(old n1, old n2)
            n2 = (n12max - n1) >> sh1;      // Remove some zeros from
                                            //    abs((old n1) - (old n2))
            extract_odd_part(n2);
            //assert (n1 & n2 & 1);           // Both odd
        }
    }
    if (OK) *pgcd = n1;
    return OK;
} /* digit_ogcd */

EXIT_PK_NAMESPACE_CODE;
