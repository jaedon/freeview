/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NFERRORCODE_H__
#define  __NFERRORCODE_H__

namespace netflix {

// Error categories
#define NFERR_CATEGORY_MASK              0xFFFF0000
#define NFERR_CATEGORY_SUCCESS_VAL       0x00000000
#define NFERR_CATEGORY_GENERICERROR_VAL  0xF0000000
#define NFERR_CATEGORY_MCERROR_VAL       0xF0010000
#define NFERR_CATEGORY_DEVICEACCOUNT_VAL 0xF0020000
#define NFERR_CATEGORY_NCCP_VAL          0xF0030000
#define NFERR_CATEGORY_CRYPTO_VAL        0xF0040000

// define the errors from the internal include file
enum NFErrorCode
{
#define DEFINE_CATEGORY(cat, val)   cat = val,
#define DEFINE_ERROR(val)           val,
#include <nrdbase/NFErrInternal.h>
    NFError_Last
#undef DEFINE_CATEGORY
#undef DEFINE_ERROR
};


inline bool isSuccessful(NFErrorCode err)
{ return ((err & NFERR_CATEGORY_MASK) == NFERR_CATEGORY_SUCCESS_VAL); }

inline bool isGenericError(NFErrorCode err)
{ return ((err & NFERR_CATEGORY_MASK) == NFERR_CATEGORY_GENERICERROR_VAL); }

inline bool isMediaControlError(NFErrorCode err)
{ return ((err & NFERR_CATEGORY_MASK) == NFERR_CATEGORY_MCERROR_VAL); }

}

#endif // __NFERRORCODE_H__
