/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __SCREENBRIDGE_H__
#define __SCREENBRIDGE_H__

#include <GibbonBaseBridge.h>

#include <nrd/NfObject.h>

namespace netflix {
namespace gibbon {

class Widget;

class ScreenBridge : public GibbonBaseBridge
{
public:
    ScreenBridge();
    ~ScreenBridge();

    void clearScene();
    inline shared_ptr<Widget> getRoot() const { return mRoot; }
    inline shared_ptr<Widget> getOverlay() const { return mOverlay; }

    virtual bool getProperty(int index, Variant *result) const;
    virtual bool setProperty(int index, const Variant &value);
    virtual Variant invoke(int method, const Variant &args);
    virtual unsigned requestFlags(const Request &request) const;
protected:
    shared_ptr<Widget> mRoot, mOverlay;
};

}} // namespace netflix::gibbon

#endif /* __SCREENBRIDGE_H__ */
