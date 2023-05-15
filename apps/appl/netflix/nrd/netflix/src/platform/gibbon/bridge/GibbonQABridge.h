/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __GIBBONQABRIDGE_H__
#define __GIBBONQABRIDGE_H__

#include <nrd/NfObject.h>
#include <nrdbase/Variant.h>

namespace netflix {
namespace gibbon {

class GibbonQABridge : public NfObject
{
public:
    GibbonQABridge();

    virtual Variant invoke(int method, const Variant &args);
    virtual unsigned requestFlags(const Request &) const;
};

}} // namespace netflix::gibbon

#endif /* __GIBBONQABRIDGE_H__ */
