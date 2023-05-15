/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "QAMdxLibBridge.h"
#include "QAHelperFuncs.h"

#include "Request.h"
#include "Response.h"

#include <mdxlib/MdxLib.h>

using namespace std;
using namespace netflix;
//using namespace netflix::mdx;
using namespace netflix::qa;

// ---------------------------------------------------------------------------

Variant QAMdxLibBridge::start(const Request &request)
{

    NFErr err = MdxLib::instance()->start();
    Variant error("nferr", err.toString());
    return error;
}

Variant QAMdxLibBridge::stop(const Request &request)
{
    NFErr err = MdxLib::instance()->stop();
    Variant error("nferr", err.toString());
    return error;
}

Variant QAMdxLibBridge::getVersion(const Request &request)
{
    return Variant("version", netflix::MdxLib::instance()->getVersion());
}

// ---------------------------------------------------------------------------

DispatchTableBegin(methods)
DispatchTableFunc("start", "")
DispatchTableFunc("stop", "")
DispatchTableFunc("getVersion", "")
DispatchTableEnd()

// ---------------------------------------------------------------------------

QAMdxLibBridge::QAMdxLibBridge(NfObject *parent) : NfObject(QAMdxLib, "mdxlib", parent, 0, methods())
{
}

QAMdxLibBridge::~QAMdxLibBridge()
{
}

Response QAMdxLibBridge::invoke(const Request &request)
{
    Variant vm;

    const std::string method = request.method();
    if (method == "start") {
        vm = start(request);
    } else if (method == "stop") {
        vm = stop(request);
    } else if (method == "getVersion") {
        vm = getVersion(request);
    } else {
        return NfObject::invoke(request);
    }

    bool ok;
    const int idx = request.value<int>("IDX", &ok);
    if (ok) {
        Variant data;
        data["IDX"] = idx;
        data["data"] = vm;
        parent()->sendEvent(name(), data);
    } else {
        parent()->sendEvent(name(), vm);
    }

    return Response::createMethodCall(this, method);
}
