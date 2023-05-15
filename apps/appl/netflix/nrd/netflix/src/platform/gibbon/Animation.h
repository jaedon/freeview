/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ANIMATION_H
#define ANIMATION_H

#include <Rect.h>
#include <Debug.h>
#include <GibbonApplication.h>

#include <nrdbase/Time.h>
#include <nrdbase/Thread.h>
#include <nrdbase/Semaphore.h>

#include <vector>
#include <queue>

#if defined(BUILD_DEBUG)
#if 1
#  define DEBUG_ANIMATION_FPS 1 //animation start + animation ends
#else
#  define DEBUG_ANIMATION_FPS 2 //above + work done for each frame
# endif
#endif

namespace netflix {
namespace gibbon {

class AnimationStart;
class AnimationFinish;

class AnimationObject : public enable_shared_from_this<AnimationObject>
{
public:
    typedef shared_ptr<AnimationObject> SharedPtr;
    typedef weak_ptr<AnimationObject> WeakPtr;

    AnimationObject() { }
    virtual ~AnimationObject();
    virtual std::string describe() const { return std::string(); }

    virtual void startAnimatingProperty(int /*property*/, const AnimationStart &) { }
    virtual void endAnimatingProperty(int /*property*/, const AnimationFinish &) { }

    virtual std::string getAnimatedPropertyName(int property) const = 0;
    virtual int getAnimatedPropertyID(const std::string &property) const = 0;
    virtual bool isIntegralProperty(int /*property*/) const { return false; }

    virtual float getAnimatedProperty(int property) const = 0;
    virtual bool setAnimatedProperty(int property, float value) = 0;

    inline AnimationObject::SharedPtr sharedPtr() { return shared_from_this(); }
};

class Animation
{
public:

    static void init();
    static void shutdown();
    static void dump();
    static Mutex &lock() { return sLock; }

    static void setAnimationThreadPtr(void* id);

    enum Stop
    {
        Stop_Current,
        Stop_End,
        Stop_Pending
    };

    struct Info {
        enum Round
        {
            Round_None,
            Round_Floor,
            Round_Ceil,
            Round_Round, //baby, right round
            Round_Default
        };

        class InterpolatorParams {
        public:
        };

        enum Interpolator
        {
            Interpolator_None = 0,
            Interpolator_Linear,
            Interpolator_CubicBezier,
            Interpolator_EaseIn,
            Interpolator_EaseOut,
            Interpolator_EaseInOut,
            Interpolator_EaseInBounce,
            Interpolator_EaseOutBounce,
            Interpolator_EaseInOutBounce,
            Interpolator_EaseInQuad,
            Interpolator_EaseOutQuad,
            Interpolator_EaseInOutQuad,
            Interpolator_EaseInCubic,
            Interpolator_EaseOutCubic,
            Interpolator_EaseInOutCubic,
            Interpolator_EaseInQuint,
            Interpolator_EaseOutQuint,
            Interpolator_EaseInOutQuint,
            Interpolator_EaseInQuart,
            Interpolator_EaseOutQuart,
            Interpolator_EaseInOutQuart,
            Interpolator_BlendInOut
        };


        Info(const Variant &desc);
        inline Info(Interpolator _interpolator=Interpolator_Linear, int _duration=0, Round _round=Round_Default) :
            interpolator(_interpolator), combine(false), duration(_duration), round(_round) { }

        static std::string convertRound(Round round);
        static Round convertRound(const std::string &round);

        static std::string convertInterpolator(Interpolator interp);
        static Interpolator convertInterpolator(const std::string &interp);

        shared_ptr<InterpolatorParams> interpolatorParams;
        Interpolator interpolator;
        bool combine;
        int duration;
        Round round;
        int id;
    };

    struct KeyFrame {
        enum { NoOp = 0x01 };
        inline KeyFrame() : startValue(NAN), endValue(NAN), flags(0) { }
        inline KeyFrame(float _startValue, float _endValue, const Info &info) : startValue(_startValue), endValue(_endValue), flags(0), info(info)  {
            updateFlags();
        }
        inline void updateFlags() {
            flags = 0;
            if(gibbon_float_compare(startValue, endValue))
                flags |= NoOp;
        }
        float startValue, endValue;
        uint8_t flags;
        Info info;
    };

    static bool advance(ullong now);
    static float getValue(const AnimationObject::SharedPtr &object, int property, float *minValue=0, float *maxValue=0);

    inline static bool isAnimating() { return sActive; }
    static void destroy(AnimationObject *object);
    static void stop(const AnimationObject::SharedPtr &object, int property, Stop stopType);
    inline static void start(const AnimationObject::SharedPtr &object, const std::string &property_name, float startValue, float endValue, const Info &info, bool append=false) {
        const int property = object->getAnimatedPropertyID(property_name);
        start(object, property, startValue, endValue, info, append);
    }
    inline static void start(const AnimationObject::SharedPtr &object, int property, float startValue, float endValue, const Info &info, bool append=false) {
        std::vector<KeyFrame> keyFrames;
        keyFrames.push_back(KeyFrame(startValue, endValue, info));
        start(object, property, startValue, keyFrames, append);
    }
    static void start(const AnimationObject::SharedPtr &object, int property, float startValue, const std::vector<KeyFrame> &frames, bool append);

#if !defined(GIBBON_NO_THREADANIMATION)
    static bool isAnimationThread();
    static void update(const Rect &);
    static bool sendEvent(const GibbonApplication::Event::SharedPtr &event);
    static bool postEvent(const GibbonApplication::Event::SharedPtr &event);
    inline static bool postEvent(GibbonApplication::Event *event)
    {
        return postEvent(GibbonApplication::Event::SharedPtr(event));
    }
    static bool startTimer(const GibbonApplication::Timer::SharedPtr &timer);
    static void stopTimer(const GibbonApplication::Timer::SharedPtr &timer);
#endif

    struct Task
    {
        int property;
        bool integral;
        float deltaValue;
        float currentValue;
        KeyFrame currentFrame;
        std::vector<KeyFrame> keyFrames;

        ullong startTime, lastTime;

        float nextValue(ullong elapsedTime) const;
        inline bool finished(ullong elapsedTime) const {
            return elapsedTime >= (ullong)currentFrame.info.duration;
        }

        bool popFrame(ullong now);
        void appendFrame(AnimationObject *object, KeyFrame newFrame);

        struct Tick {
            Tick(ullong _start, float _value) : start(_start), value(_value) { }
            ullong start;
            float value;
        };
        inline void addTick(const Tick &tick) {
#ifndef DEBUG_ANIMATION_FPS
            if(Debug::DebugAnimationFPS)
#endif
                ticks.push_back(tick);
        }
        float dumpTicks(AnimationObject *object, const char *desc, ullong started) const;
        mutable std::list<Tick> ticks;

#ifdef DEBUG_ANIMATION_FPS
        std::string description;
#endif
    };


private:
    typedef std::map<int, Task> Tasks;
    typedef std::map<AnimationObject *, Tasks> EntryContainer;

    static EntryContainer sEntries;
    static Mutex sLock;
    static Mutex sThreadPtrLock;
    static void* sThreadPtr;
    static ullong sLastTime;
    static bool sActive;

    static void setAnimating(const AnimationObject::SharedPtr &object, bool animating);
    static void stop(const AnimationObject::SharedPtr &object, Task &task, Stop stopType);
    static void cancel(const AnimationObject::SharedPtr &object, const Task &task, float endValue, const char *desc="STOP");
};

class AnimationFinish
{
public:
    enum State { State_None, State_Aborted, State_Restarting, State_Done };
    inline AnimationFinish() : value(NAN), state(State_None), fps(NAN) { }
    inline AnimationFinish(const Animation::KeyFrame &_frame, float _value, State _state, float _fps) : frame(_frame), value(_value), state(_state), fps(_fps) { }
    Animation::KeyFrame frame;
    float value;
    State state;
    float fps;
};

class AnimationStart
{
public:
    inline AnimationStart() { }
    inline AnimationStart(const Animation::KeyFrame &_frame) : frame(_frame) { }
    Animation::KeyFrame frame;
};

}} // namespace netflix::gibbon

#endif // ANIMATION_H
