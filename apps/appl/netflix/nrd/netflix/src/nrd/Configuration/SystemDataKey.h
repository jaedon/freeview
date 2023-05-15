/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __SYSTEM_DATA_KEY_H__
#define __SYSTEM_DATA_KEY_H__

#include <string>

namespace netflix {

/*
 * Key for listening on SystemData changes
 */
class SystemDataKey
{
public:
    enum Key
    {
        SYSTEM_FILE,
        UI_LANGUAGE
    };

    static std::string toString(Key key);
};

}

#endif // __SYSTEM_DATA_KEY_H__
