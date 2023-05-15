/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef INSTRUMENTATIONBRIDGE_H_
#define INSTRUMENTATIONBRIDGE_H_

#include <nrd/NfObject.h>
#include <nrd/Request.h>

namespace netflix {

class InstrumentationBridge : public NfObject
{
public:
    InstrumentationBridge();
    void init();
    virtual unsigned requestFlags(const Request &) const { return Request::Synchronous; }
    virtual Variant invoke(int method, const Variant &data);
    virtual bool getProperty(int index, Variant *result) const;
    void setVerbose(bool verbose);
private:
    bool mVerbose;

};

} // netflix



#endif /* INSTRUMENTATIONBRIDGE_H_ */