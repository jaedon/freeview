/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SignupBridge_h
#define SignupBridge_h

#include <nrd/NfObject.h>

namespace netflix {
namespace device {

class SignupBridge : public NfObject
{
public:
    SignupBridge();
    ~SignupBridge();

    void init();

    virtual bool getProperty(int index, Variant *result) const;
    virtual Variant invoke(int method, const Variant &data);
protected:
    void event(ApplicationEvent::Type type, const Variant &/*data*/);
private:
    bool isAuthorized;
};
}
}

#endif // SignupBridge_h
