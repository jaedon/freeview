/*
 *(c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef SYSTEMBRIDGE_H_
#define SYSTEMBRIDGE_H_

#include <nrdbase/tr1.h>
#include <nrdbase/Time.h>
#include <nrd/NfObject.h>
#include <nrd/Request.h>

namespace netflix {

class SystemBridge: public NfObject
{
public:
    SystemBridge();
    virtual bool getProperty(int index, Variant* result) const;
    virtual bool setProperty(int index, const Variant &value);
private:
    std::string mUiQueryString;
};

}

#endif /* SYSTEMBRIDGE_H_ */
