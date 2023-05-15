/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef NFErrName_h
#define NFErrName_h

#include <nrdbase/NFErrorCode.h>

namespace netflix {

/**
 * @class NFErrName NFErrName.h
 * @brief Helper class for converting from NFErr values to names.
 */
class NFErrName
{
public:
    /**
     * Print the name/value pairs of all NFErr definitions.
     */
    static void listAll();

    /**
     * Lookup for the name which corresponds to the specified NFErr
     * value.
     *
     * @param[in] error the NFErr value.
     * @return a character string which contains the NFErr name or the
     *         empty string if the error is unknown.
     */
    static const char* lookup(NFErrorCode error);

private:
    static const char* lookupOrlistAll(NFErrorCode error, bool list);
};

} // namespace netflix

#endif
