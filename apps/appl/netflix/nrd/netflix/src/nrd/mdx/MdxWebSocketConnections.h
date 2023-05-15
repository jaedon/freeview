/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __MDXWEBSOCKETCONNECTIONS_H__
#define __MDXWEBSOCKETCONNECTIONS_H__

#include <nrdbase/tr1.h>
#include "WebSocketMdx.h"

namespace netflix
{
namespace mdx
{

class MdxServer;

class MdxWebSocketConnections
{
public:

    static bool                     addConnection(const std::string &host, shared_ptr<MdxServer> pServer);
    static shared_ptr<MdxServer>    getConnection(const std::string &host);
    static bool                     removeConnection(const std::string &host, shared_ptr<MdxServer> pServer);
    static void                     clearConnections();

private:

};

}}

#endif
