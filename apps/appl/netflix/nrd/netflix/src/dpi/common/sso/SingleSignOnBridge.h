/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SingleSignOnBridge_h
#define SingleSignOnBridge_h

#include <nrd/NfObject.h>

namespace netflix {

class SingleSignOnBridge : public NfObject
{
public:
    SingleSignOnBridge();
    ~SingleSignOnBridge();

    virtual bool getProperty(int index, Variant *value) const;
    virtual bool invoke(int method, const Variant &data);
};
}

#endif
