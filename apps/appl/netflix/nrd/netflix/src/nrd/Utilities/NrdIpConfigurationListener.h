/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef NRD_IP_CONFIGURATION_LISTENER
#define NRD_IP_CONFIGURATION_LISTENER

#include <SystemDataListener.h>

namespace netflix
{

namespace util
{

class NrdIpConnectivityManager;

class NrdIpConfigurationListener : public SystemDataListener
{
public:
    /** Constructor. */
    NrdIpConfigurationListener(
                    NrdIpConnectivityManager* pIpConnectivityManager );

    /** Destructor. */
    ~NrdIpConfigurationListener();

    /** The listener callback. */
    virtual void datumChanged( SystemDataKey::Key key );

protected:

    NrdIpConnectivityManager* mNrdIpConnectivityManagerPtr;
};

}} // namespace netflix::util

#endif //NRD_IP_CONFIGURATION_LISTENER

