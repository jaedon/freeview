/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "NFErrName.h"

#include <stdio.h>

using namespace netflix;


void NFErrName::listAll()
{
    // relies on CategorySuccess being first item so that the first case statement will
    // execute the printf's on all values
    lookupOrlistAll(NFErr_Category_Success, true);
}

const char* NFErrName::lookup(NFErrorCode error)
{
    return lookupOrlistAll(error, false);
}

const char* NFErrName::lookupOrlistAll(NFErrorCode error, bool list)
{
    switch (error)
    {
        // Use NFErrInternal to define the case statements
#define DEFINE_ERROR(_name_)                            \
        case _name_:                                    \
        {                                               \
            static const char *name = #_name_;          \
            if (list)                                   \
                printf("%-35s: 0x%X\n", name, _name_);  \
            else                                        \
                return name;                            \
        }
        // treat categories as errors
#define DEFINE_CATEGORY(cat, val) DEFINE_ERROR(cat)

#include <nrdbase/NFErrInternal.h>

#undef DEFINE_ERROR
#undef DEFINE_CATEGORY

        default:
            break;
    }

    return "";
}
