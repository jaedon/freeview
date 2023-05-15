/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef QAMDXLIBBRIDGE_H
#define QAMDXLIBBRIDGE_H

#include <nrd/NfObject.h>


namespace netflix {

class QAMdxLibBridge : public NfObject
{
public:
    QAMdxLibBridge(NfObject *parent);
    ~QAMdxLibBridge();

    virtual Response invoke(const Request &request);
    Variant start(const Request &request);
    Variant stop(const Request &request);
    Variant getVersion(const Request &request);
};
}

#endif // QAMDXLIBBRIDGE_H
