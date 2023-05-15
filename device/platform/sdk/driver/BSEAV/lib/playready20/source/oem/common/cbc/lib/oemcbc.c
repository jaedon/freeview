/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <byteorder.h>
#include <drmcrt.h>
#include <oemcbc.h>

ENTER_PK_NAMESPACE_CODE;

// Iterate over a buffer performing the MP_C_STEP_P_COMBINED on each
// set of 2 32 bit values. The buffer pointer may be UNALIGNED.
// The cbcstate->t and cbcstate->sum values are updated, as well as
// the count (of remaining 8 byte groups,) which is provided by
// pointer.
#define MP_C_STEP_P_COMBINED_LOOP(cbcstate, ppbData, count, key)                                                        \
    do {                                                                                                                \
        while( count > 0 )                                                                                              \
        {                                                                                                               \
            MP_C_STEP_P_COMBINED( ppbData, key, cbcstate->t, cbcstate->sum );                                           \
            count--;                                                                                                    \
        }                                                                                                               \
    } while( FALSE )

EXIT_PK_NAMESPACE_CODE;

#include <oemcbcimpl.c>

