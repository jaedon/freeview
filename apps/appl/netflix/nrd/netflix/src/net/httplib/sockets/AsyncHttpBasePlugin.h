/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ASYNC_HTTP_BASE_PLUGIN_H
#define ASYNC_HTTP_BASE_PLUGIN_H

#include <nrdbase/tr1.h>

#include <nrdbase/AseErrorCodes.h>
#include <nrdnet/AseTimeVal.h>

namespace netflix {

/* @class
 * @brief
 */
class AsyncHttpBasePlugin
{
public:
    typedef shared_ptr<AsyncHttpBasePlugin> AsyncHttpBasePluginPtr;

public:
    /** Destructor. */
    virtual ~AsyncHttpBasePlugin()
    {
    }

    /** Initialize the plugin */
    virtual int32_t init() = 0;

    /** Deinitialize the plugin */
    virtual int32_t deinit() = 0;

    /** Prepare the data for calling select(). This function is called before select()
     *  to fill up the FD sets and when the plugin is interested in a timeout signal.
     *
     *  @param timeNow [In] The current time.
     *  @param maxSelectingFd [Out] The max selecting FD to select.
     *  @param pReadFdSet [Out] The read FD set.
     *  @param pWriteFdSet [Out] The write FD set.
     *  @param timeout [Out] The time the connection is interested in a timeout signal.
     */
    virtual void prepareSelect(
                        const AseTimeVal& timeNow,
                        int& maxSelectingFd,
                        fd_set* pReadFdSet,
                        fd_set* pWriteFdSet,
                        AseTimeVal& timeout ) = 0;

    /** Process the select events. This function is called to process events returned
     *  from select().
     *
     *  @param timeNow [In] The current time.
     *  @param pReadFdSet [In] The read FD set.
     *  @param pWriteFdSet [In] The write FD set.
     */
    virtual void processSelect(
                        const AseTimeVal& timeNow,
                        fd_set* pReadFdSet,
                        fd_set* pWriteFdSet ) = 0;

};

typedef AsyncHttpBasePlugin::AsyncHttpBasePluginPtr AsyncHttpBasePluginPtr;

}
#endif //ASYNC_HTTP_BASE_PLUGIN_H
