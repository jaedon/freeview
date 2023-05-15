/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef SYSTEMDATALISTENER_H
#define SYSTEMDATALISTENER_H

#include <string>

#include "SystemDataKey.h"

namespace netflix {

/**
 * @class SystemDataListener SystemDataListener.h
 * @brief SystemDataListeners are synchronously notified of changes to
 *        the SystemData.
 */
class SystemDataListener
{
public:
    /** Constructor. */
    SystemDataListener() {}

    virtual ~SystemDataListener() {}

    /**
     * Called whenever a value has changed.
     *
     * @param key the datum that has changed.
     * @param newValue the new datum value.
     * @param oldValue the original datum value.
     */
    virtual void datumChanged(SystemDataKey::Key) {}
};

} // namespace netflix::config

#endif
