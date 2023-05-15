/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __SPLASHWIDGET_H__
#define __SPLASHWIDGET_H__

#include "Widget.h"

#include <nrdbase/Application.h>

namespace netflix {
namespace gibbon {

class SplashWidget : public Widget
{
public:
    typedef shared_ptr<SplashWidget> SharedPtr;
    typedef weak_ptr<SplashWidget> WeakPtr;

    SplashWidget();
    ~SplashWidget();

    virtual void init();

    inline bool done() const { return !mLogo->getOpacity() && !mBackground->getOpacity(); }
    void cleanup();
    void close();

protected:
    Widget::SharedPtr mBackground, mSpinner, mLogo, mShadow;
};


}} // namespace netflix::gibbon
#endif /* __SPLASHWIDGET_H__ */
