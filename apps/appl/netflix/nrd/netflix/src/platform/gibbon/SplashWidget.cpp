/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "SplashWidget.h"
#include "GibbonApplication.h"
#include "GibbonConfiguration.h"
#include "Screen.h"

using namespace netflix;
using namespace netflix::gibbon;

static SplashWidget::SharedPtr sSplashWidget;

SplashWidget::SplashWidget() : Widget()
{
}

SplashWidget::~SplashWidget()
{
}

void SplashWidget::init()
{
    Screen::SharedPtr screen = GibbonApplication::instance()->getScreen();
    const Rect r(screen->getRect());

    Widget::init();
    setName("SplashWidget");
    setRenderCache(Widget::RenderCache_Disabled);
    setOpaque(true);
    setRect(r);

    mBackground.reset(new Widget);
    mBackground->init();
    mBackground->setName("SplashWidget::Background");
    mBackground->setRect(Rect(0, 0, 1280, 720));
    mBackground->setBackgroundColor(Color(5, 5, 5));
    mBackground->setRenderCache(Widget::RenderCache_Disabled);
    {
        Widget::SharedPtr backgroundImage(new Widget);
        backgroundImage->init();
        backgroundImage->setName("SplashWidget::Background::Image");
        backgroundImage->setRect(Rect(0, 320, 1280, 353));
        backgroundImage->setRenderCache(Widget::RenderCache_Disabled);
        {
            Image::SharedPtr image(new Image(-1, backgroundImage, Image::Type_Foreground));
            backgroundImage->addImage(image);
            image->load("http://localcontrol.netflix.com/img/Netflix_Background_Splash.png",
                        SurfaceLoader::Load_Synchronous);
        }
        mBackground->addChild(backgroundImage);
    }
    addChild(mBackground);

    mLogo.reset(new Widget);
    mLogo->init();
    mLogo->setName("SplashWidget::Logo");
    mLogo->setRect(Rect(440, 306, 400, 108));
    mLogo->setRenderCache(Widget::RenderCache_Disabled);
    mLogo->setTransformOrigin(Point(200, 54));
    mLogo->setSmoothScale(true);
    {
        Image::SharedPtr image(new Image(-1, mLogo, Image::Type_Foreground));
        mLogo->addImage(image);
        image->setHorizontalAlignment(Image::Align_Center);
        image->setVerticalAlignment(Image::Align_Center);
        image->load("http://localcontrol.netflix.com/img/Netflix_Logo_Splash.png",
                    SurfaceLoader::Load_Synchronous);
    }
    mBackground->addChild(mLogo);

    mShadow.reset(new Widget);
    mShadow->init();
    mShadow->setName("SplashWidget::Shadow");
    mShadow->setRect(Rect(380, 489, 521, 37));
    mShadow->setRenderCache(Widget::RenderCache_Disabled);
    mShadow->setTransformOrigin(Point(260, 18));
    mShadow->setSmoothScale(true);
    {
        Image::SharedPtr image(new Image(-1, mShadow, Image::Type_Foreground));
        mShadow->addImage(image);
        image->setHorizontalAlignment(Image::Align_Center);
        image->setVerticalAlignment(Image::Align_Center);
        image->load("http://localcontrol.netflix.com/img/Netflix_Shadow_Splash.png",
                    SurfaceLoader::Load_Synchronous);
    }
    mBackground->addChild(mShadow);

    if(GibbonConfiguration::splashScreen() == "animated") {
        mSpinner.reset(new Widget);
        mSpinner->init();
        mSpinner->setName("SplashWidget::Spinner");
        mSpinner->setRect(Rect(616, 585, 48, 48));
        mSpinner->setRenderCache(Widget::RenderCache_Disabled);
        {
            Image::SharedPtr image(new Image(-1, mSpinner, Image::Type_Foreground));
            mSpinner->addImage(image);
            image->setHorizontalAlignment(Image::Align_Center);
            image->setVerticalAlignment(Image::Align_Center);
            image->load("http://localcontrol.netflix.com/img/Spinner_Splash.mng",
                        SurfaceLoader::Load_Synchronous);
            image->setSourceRect(Rect(0, 0, 48, 48));
        }
        mBackground->addChild(mSpinner);

        Animation::start(mBackground, "opacity", 0, 1, Animation::Info(Animation::Info::Interpolator_Linear, 500));
        Animation::start(mSpinner, "opacity", 0, 1, Animation::Info(Animation::Info::Interpolator_Linear, 2000));
        // Animation::start(mLogo, "scale", 0.8, 1.0, Animation::Info(Animation::Info::Interpolator_EaseOutQuint, 2000));
        // Animation::start(mShadow, "scale", 0.8, 1.0, Animation::Info(Animation::Info::Interpolator_EaseOutQuint, 2000));
        // Animation::start(mShadow, "y", 461, 475, Animation::Info(Animation::Info::Interpolator_EaseOutQuint, 2000));
    }
    screen->addOverlay(static_pointer_cast<Widget>(shared_from_this()));
}

void SplashWidget::cleanup()
{
    if(mSpinner)
        mSpinner->stopImages();
    setParent(Widget::SharedPtr());
}

class SplashWidgetCloseListener : public Widget::Listener
{
public:
    SplashWidgetCloseListener(SplashWidget::SharedPtr splash) : mSplash(splash) { }
    void onAnimationFinished(Widget::Property, const AnimationFinish &) {
        if(SplashWidget::SharedPtr splash = mSplash.lock()) {
            if(splash->done())
                splash->cleanup();
        }
    }
protected:
    SplashWidget::WeakPtr mSplash;
};

void SplashWidget::close()
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(GibbonConfiguration::splashScreen() == "animated" &&
       GibbonApplication::instance()->getLocation() != "http://localcontrol.netflix.com/js/error.js") {
        setOpaque(false);

        Widget::Listener::SharedPtr closeListener(new SplashWidgetCloseListener(static_pointer_cast<SplashWidget>(shared_from_this())));

        Animation::start(mBackground, "opacity", NAN, 0, Animation::Info(Animation::Info::Interpolator_EaseInOutQuint, 1200));
        mBackground->addListener(closeListener);

        Animation::start(mSpinner, "opacity", NAN, 0, Animation::Info(Animation::Info::Interpolator_Linear, 300));

        Animation::start(mLogo, "opacity", NAN, 0, Animation::Info(Animation::Info::Interpolator_EaseInCubic, 600));
        Animation::start(mLogo, "scale", NAN, 1.2, Animation::Info(Animation::Info::Interpolator_EaseInOutCubic, 1000));
        mShadow->addListener(closeListener);

        Animation::start(mShadow, "opacity", NAN, 0, Animation::Info(Animation::Info::Interpolator_EaseInCubic, 600));
        Animation::start(mShadow, "scale", NAN, 1.2, Animation::Info(Animation::Info::Interpolator_EaseInOutCubic, 1000));
        Animation::start(mShadow, "y", 489, 503, Animation::Info(Animation::Info::Interpolator_EaseInOutCubic, 1000));
        return;
    }
    cleanup();
}

#if !defined(GIBBON_NO_THREADANIMATION)
class GibbonShowSplash : public Application::Event
{
public:
    GibbonShowSplash() : mutex(ZERO_MUTEX, "GibbonShowSplash::Mutex"), finished(false) { }

    virtual void eventFired()
    {
        gibbon_oem_set_splash(true);
        ScopedMutex lock(mutex);
        finished = true;
        cond.broadcast();
    }

    virtual std::string describe() const { return "GibbonShowSplash"; };

    void wait()
    {
        ScopedMutex lock(mutex);
        while(!finished)
            cond.wait(&mutex);
    }

private:
    Mutex mutex;
    bool finished;
    ConditionVariable cond;
};
static weak_ptr<GibbonShowSplash> sPendingSplashShow;
#endif

void gibbon_oem_set_splash(bool show)
{
    Screen::SharedPtr screen = GibbonApplication::instance()->getScreen();
    if(!screen) {
        ScopedMutex _widgetLock(Widget::lock());
        sSplashWidget.reset();
        return;
    }

    if(show) {
        if(GibbonConfiguration::benchmark().empty() &&
           (GibbonConfiguration::splashScreen() == "static" ||
            GibbonConfiguration::splashScreen() == "animated")) {
            ScopedMutex _lock(Widget::lock());
#if !defined(GIBBON_NO_THREADANIMATION)
            if (!Animation::isAnimationThread()) {
                assert(!sPendingSplashShow.lock());
                shared_ptr<GibbonShowSplash> event(new GibbonShowSplash);
                sPendingSplashShow = event;
                Animation::sendEvent(event);
                return;
            }
            sPendingSplashShow.reset();
#endif
            if(sSplashWidget)
                sSplashWidget->close();
            sSplashWidget.reset(new SplashWidget);
            sSplashWidget->init();
        }
    } else {
        ScopedMutex _widgetLock(Widget::lock());
#if !defined(GIBBON_NO_THREADANIMATION)
        if(shared_ptr<GibbonShowSplash> event = sPendingSplashShow.lock())
            event->wait();
#endif
        if(sSplashWidget) {
            ScopedMutex _lock(Animation::lock());
            sSplashWidget->close();
            sSplashWidget.reset();
        }
    }
}


