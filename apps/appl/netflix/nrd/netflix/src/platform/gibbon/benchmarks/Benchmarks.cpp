
#include <Image.h>
#include <Widget.h>
#include <Screen.h>
#include <Benchmarks.h>
#include <GibbonConfiguration.h>
#include <GibbonApplication.h>

#include <nrdbase/Time.h>
#include <nrdbase/tr1.h>
#include <nrdbase/MutexRanks.h>

#include <nrd/AppLog.h>

#include <iostream>

using namespace netflix;
using namespace netflix::gibbon;

static const char *BENCHMARK_IMAGE_URLS[] = {
    "http://cdn-1.nflximg.com/en_us/boxshots/150/70188961.jpg",
    "http://cdn-1.nflximg.com/en_us/boxshots/150/1155067.jpg",
    "http://cdn-1.nflximg.com/en_us/boxshots/150/70244455.jpg",
    "http://cdn-1.nflximg.com/en_us/boxshots/150/70138483.jpg",
    "http://cdn-1.nflximg.com/en_us/boxshots/150/70130481.jpg",
    "http://cdn-0.nflximg.com/en_us/boxshots/150/70236628.jpg",
    "http://cdn-0.nflximg.com/en_us/boxshots/150/70141586.jpg",
    "http://cdn-0.nflximg.com/en_us/boxshots/150/70111470.jpg",
    "http://cdn-1.nflximg.com/en_us/boxshots/150/70205063.jpg",
    "http://cdn-0.nflximg.com/en_us/boxshots/150/18957298.jpg",
    "http://cdn-0.nflximg.com/en_us/boxshots/150/60036164.jpg",
    "http://cdn-0.nflximg.com/en_us/boxshots/150/70075482.jpg",
    "http://cdn-1.nflximg.com/en_us/boxshots/150/70129195.jpg",
    "http://cdn-1.nflximg.com/en_us/boxshots/150/70118305.jpg",
    "http://cdn-1.nflximg.com/en_us/boxshots/150/70180293.jpg",
    "http://cdn-0.nflximg.com/en_us/boxshots/150/70205024.jpg",
    "http://cdn-0.nflximg.com/en_us/boxshots/150/70140358.jpg",
    "http://cdn-1.nflximg.com/en_us/boxshots/150/70140403.jpg",
    "http://cdn-0.nflximg.com/en_us/boxshots/150/70136128.jpg",
    "http://cdn-0.nflximg.com/en_us/boxshots/150/70136120.jpg",
    "http://cdn-1.nflximg.com/en_us/boxshots/150_new/70136107.jpg",
    "http://cdn-1.nflximg.com/en_us/boxshots/150/70155569.jpg",
    "http://cdn-1.nflximg.com/en_us/boxshots/150/70154929.jpg",
    "http://cdn-0.nflximg.com/en_us/boxshots/150/70224944.jpg",
    0
};
const unsigned BENCHMARK_IMAGE_WIDTH  = 150;
const unsigned BENCHMARK_IMAGE_HEIGHT = 214;


//=============================================================================

class Benchmark
{
public:
    virtual ~Benchmark() {}
    virtual void operator()(const std::vector<std::string> &arguments) = 0;
};

//=============================================================================

DECLARE_THREAD(SCHEDULER_BENCHMARK);
DEFINE_THREAD(SCHEDULER_BENCHMARK, 0, 0);

class SchedulerBenchmark : public Benchmark
{
    class ProducerThread : public Thread
    {
    public:
        inline ProducerThread(int id, int _every, int _sleep): Thread(&THREAD_SCHEDULER_BENCHMARK), mID(id), mMutex(UNTRACKED_MUTEX, "Producer"),
                                                               mEvery(_every), mSleep(_sleep), mCounter(0), mLastCounter(0) { }
        virtual void Run() {
            ullong start = Time::mono().ms();
            while(true) {
                bool doSleep = false;
                {
                    ScopedMutex _lock(mMutex);
                    ++mCounter;
                    doSleep = !(mCounter % mEvery);
                }
                if(doSleep) {
                    Log::warn(TRACE_GIBBON_BENCHMARK) << "Producer Hit(" << mID << "): " << Time::mono().ms() - start;
                    usleep(mSleep*1000);
                    start = Time::mono().ms();
                }
            }
        }
        inline int getID() const { return mID; }
        inline int getCount(int &lastCount) {
            ScopedMutex _lock(mMutex);
            lastCount = mCounter - mLastCounter;
            mLastCounter = mCounter;
            return mCounter;
        }
    protected:
        int mID;
        Mutex mMutex;
        int mEvery, mSleep;
        int mCounter, mLastCounter;
    };

    static void dump(const char *desc, const std::vector<ProducerThread*> &producers)
    {
        static int counter = 0;
        const ullong start = Time::mono().ms();
        sleep(1);
        const ullong end = Time::mono().ms();
        Log::warn(TRACE_GIBBON_BENCHMARK) << "Ran(" << desc << "): " << end-start;
        for(size_t i = 0; i < producers.size(); ++i) {
            int l, c = producers[i]->getCount(l);
            Log::warn(TRACE_GIBBON_BENCHMARK) << " + Producer(" << desc << "):" << counter++ << ": " << producers[i]->getID() << " " << c << "(" << l << ")";
        }
    }

    class ConsumerThread : public Thread
    {
    public:
        inline ConsumerThread(const std::vector<ProducerThread*> &_producers) : Thread(&THREAD_SCHEDULER_BENCHMARK), producers(_producers) { }
        virtual void Run() {
            while(true)
                SchedulerBenchmark::dump("CONSUMER", producers);
        }
    protected:
        std::vector<ProducerThread*> producers;
    };


public:
    virtual void operator()(const std::vector<std::string> &arguments)
    {
        int producer_count = 5;
        int producer_sleep_every = 1000000;
        int producer_sleep_ms = 400;
        if(arguments.size() >= 1)
            producer_count = atoi(arguments[0].c_str());
        if(arguments.size() >= 2)
            producer_sleep_every = atoi(arguments[1].c_str());
        if(arguments.size() >= 3)
            producer_sleep_ms = atoi(arguments[2].c_str());

        std::vector<ProducerThread*> producers;
        for(int i = 0; i < producer_count; ++i) {
            ProducerThread *producer = new ProducerThread(i, producer_sleep_every, producer_sleep_ms);
            producer->Start();
            producers.push_back(producer);
        }
        ConsumerThread *consumer = new ConsumerThread(producers);
        consumer->Start();
        while (true) {
            sleep(1);
        }
    }
};

//=============================================================================

class BoxShotScrollBenchmark : public Benchmark
{
public:
    virtual void operator()(const std::vector<std::string> &arguments)
    {
        int COL    = 8;
        int ROW    = 3;
        if(arguments.size() >= 1)
            COL = std::min(atoi(arguments[0].c_str()), COL);
        if(arguments.size() >= 2)
            ROW = std::min(atoi(arguments[1].c_str()), ROW);

        const unsigned COUNT  = COL * ROW;
        const unsigned PAD    = 30;
        const float PPS       = 120.0f;
        const unsigned EXTRA  = 0;

        Surface::SharedPtr screenSurface(GibbonApplication::instance()->getScreen()->getSurface());

        GraphicsEngine ge(screenSurface);

        typedef std::vector<Image::SharedPtr> ImageVector;

        ImageVector images;

        for(unsigned i = 0; i < COUNT; ++i) {
            ScopedMutex _lock(Widget::lock());
            Widget::SharedPtr w(new Widget());
            Image::SharedPtr image(new Image(-1, w, Image::Type_Foreground));
            image->load(BENCHMARK_IMAGE_URLS[i], SurfaceLoader::Load_Synchronous);
            images.push_back(image);
        }

        unsigned left = 0;
        unsigned top = 10;

        Rect *rects = new Rect[COUNT];

        for(size_t i = 0; i < COUNT; ++i) {
            if(i && !(i % COL)) {
                left = 0;
                top += BENCHMARK_IMAGE_HEIGHT + PAD;
            }

           rects[i].x = left;
           rects[i].y = top;
           rects[i].width = BENCHMARK_IMAGE_WIDTH;
           rects[i].height = BENCHMARK_IMAGE_HEIGHT;

           left += BENCHMARK_IMAGE_WIDTH + PAD;
        }

        ge.clearClip();

        ullong start = Time::mono().ms();
        ullong last = start;
        ullong end;
        ullong ss;
        ullong flip = 0;
        unsigned frames = 0;
        float dx;
        Rect sr( screenSurface->getRect() );
        float t;
        int bc = 0;
        int extra;

        struct { int c; int cd; } color[3] = { { 128 , 1 } , { 0 , 1 } , { 0 , 1 } };

        while(true) {
            // Calculate the time since the last run of the loop and
            // use it to figure out how much to move (dx)

            end = Time::mono().ms();

            dx = ceil(PPS * ( ( end - last ) / 1000.0f));

            last = end;

            // Fill the screen with the current color

            ge.fill( sr , Color( color[0].c , color[1].c , color[2].c , 255 ) );

            // Update the color

            for(int i = 0; i < 3; ++i) {
                color[i].c += color[i].cd;

                if(color[i].c > 255 || color[i].c < 0) {
                    color[i].cd = -color[i].cd;
                    color[i].c += color[i].cd;
                }
            }

            // Blit all the surfaces

            extra = EXTRA;

            for (size_t i = 0; i < COUNT; ++i)
            {
                Surface::SharedPtr surface;
                {
                    ScopedMutex _lock(Widget::lock());
                    surface = images[i]->getSurface();
                }
                Rect & r(rects[i]);

                if(surface) {
                    ge.blit(surface, surface->getRect(), r);
                    ++bc;

                    if ( extra )
                    {
                        ge.blit(surface, surface->getRect(), r);
                        ++bc;
                        --extra;
                    }
                }
                if (r.x + r.width < 0)
                    r.x = ( (BENCHMARK_IMAGE_WIDTH + PAD) * (COL - 1) ) + PAD + ( r.x + r.width );
                r.x -= dx;
            }

            ss = Time::mono().ms();

            ge.end();
            GibbonApplication::instance()->getScreen()->flip(sr);

            // Calculate FPS

            end = Time::mono().ms();

            flip += end - ss;

            ++frames;

            t = ( end - start ) / 1000.0f;

            if(t >= 1) {
                Log::warn(TRACE_GIBBON_BENCHMARK) << frames << " frames " << (end-start) / frames << "ms per frame, " << ceil( frames / t ) << "fps, "
                                                  << flip / ( float ) frames << "ms per flip, "
                                                  << bc / ( float ) frames  << " blits (" << (BENCHMARK_IMAGE_WIDTH*BENCHMARK_IMAGE_HEIGHT*bc) / (float) frames << " pixels) per frame";

                start = end;
                frames = 0;
                bc = 0;
                flip = 0;
            }
        }

        delete [] rects;
    }
};

//=============================================================================

class BoxShotScrollWidgetsBenchmark : public Benchmark
{
public:
    void execute(const std::vector<std::string> &arguments)
    {
        int COL    = 8;
        int ROW    = 3;
        if(arguments.size() >= 1)
            COL = std::min(atoi(arguments[0].c_str()), COL);
        if(arguments.size() >= 2)
            ROW = std::min(atoi(arguments[1].c_str()), ROW);

        const unsigned COUNT  = COL * ROW;
        const unsigned PAD    = 30;
        const float PPS       = 120.0f;

        Screen::SharedPtr screen = GibbonApplication::instance()->getScreen();
        typedef std::vector<Widget::SharedPtr> WidgetVector;
        WidgetVector widgets;

        Widget::SharedPtr root;
        {
            ScopedMutex _lock(Widget::lock());

            root.reset( new Widget() );
            root->setOpaque(true);
            root->setWidth(1280);
            root->setHeight(720);

            unsigned left = 0, top = 10;
            for(unsigned i = 0; i < COUNT; ++i) {
                if(i && !(i % COL)) {
                    left = 0;
                    top += BENCHMARK_IMAGE_HEIGHT + PAD;
                }

                Widget::SharedPtr widget(new Widget());
                Image::SharedPtr image(new Image(-1, widget, Image::Type_Foreground));
                image->load(BENCHMARK_IMAGE_URLS[i], SurfaceLoader::Load_Synchronous);
                widget->setParent(root);
                widget->addImage(image);
                widget->setX(left);
                widget->setY(top);
                widget->setWidth(BENCHMARK_IMAGE_WIDTH);
                widget->setHeight(BENCHMARK_IMAGE_HEIGHT);
                widgets.push_back(widget);
                left += BENCHMARK_IMAGE_WIDTH + PAD;
            }

            {
                Widget::SharedPtr focus(new Widget());
                Image::SharedPtr image( new Image( -1, focus, Image::Type_Foreground ) );
                image->load("http://cdn-0.nflximg.com/us/ffe/htmltvui/plus/720/home/150x214/home_focus_ring_4.png",
                            SurfaceLoader::Load_Synchronous);
                focus->setParent(root);
                focus->addImage(image);
                focus->setX(250);
                focus->setY(330);
            }
        }

        ullong start = Time::mono().ms();
        ullong last = start;
        ullong end;
        ullong ss;
        ullong render = 0;
        unsigned frames = 0;
        float dx;
        float t;

        struct { int c; int cd; } color[3] = { { 128 , 1 } , { 0 , 1 } , { 0 , 1 } };

        while(true) {
            {
                ScopedMutex _lock(Widget::lock());
                screen->setRoot(root);

                // Calculate the time since the last run of the loop and
                // use it to figure out how much to move (dx)

                end = Time::mono().ms();

                dx = ceil(PPS * ((end - last) / 1000.0f));

                last = end;

                // Fill the screen with the current color

                root->setBackgroundColor(Color( color[0].c , color[1].c , color[2].c , 255));

                // Update the root color

                for(int i = 0; i < 3; ++i) {
                    color[i].c += color[i].cd;

                    if(color[i].c > 255 || color[i].c < 0) {
                        color[i].cd = -color[i].cd;
                        color[i].c += color[i].cd;
                    }
                }

                // Move all the widgets

                for(size_t i = 0; i < COUNT; ++i) {
                    Widget::SharedPtr widget = widgets[i];
                    Coordinate x = widget->getX(), width = widget->getWidth();
                    if (x+width < 0)
                        x = ((BENCHMARK_IMAGE_WIDTH + PAD) * (COL - 1)) + PAD + (x+width);
                    x -= dx;
                    widget->setX(x);
                }
            }
            ss = Time::mono().ms();

            // Render

            screen->render();

            // Calculate FPS

            end = Time::mono().ms();

            render += end -ss;

            ++frames;

            t = (end - start) / 1000.0f;

            if(t >= 1) {
                Log::warn(TRACE_GIBBON_BENCHMARK) << frames << " frames " << (end-start) / frames << "ms per frame, " << ceil( frames / t ) << "fps, "
                                                  << render / ( float ) frames << "ms per render ";

                start = end;
                frames = 0;
                render = 0;
            }
        }
    }
    virtual void operator()(const std::vector<std::string> &arguments)
    {
        execute(arguments);
    }
};

//=============================================================================
static int sBenchmarkRunning = 0;

void Benchmarks::run(const std::string &name, const std::vector<std::string> &arguments)
{
    shared_ptr<Benchmark> b;
    if(name == "scroll")
        b.reset(new BoxShotScrollBenchmark());
    if(name == "scrollWidgets")
        b.reset(new BoxShotScrollWidgetsBenchmark());
    if(name == "scheduler")
        b.reset(new SchedulerBenchmark());
    if(b) {
        assert(!MutexStack::locked(Widget::lock().name())); //really can't hold it forever
        ++sBenchmarkRunning;
        (*b)(arguments);
        --sBenchmarkRunning;
    }
}

std::vector<Console::Command::Help> Benchmarks::help()
{
    std::vector<Console::Command::Help> result;
    result.push_back(Console::Command::Help("scroll", "Render through GraphicsEngine."));
    result.push_back(Console::Command::Help("scrollWidgets", "Render through render tree onto GraphicsEngine."));
    result.push_back(Console::Command::Help("scheduler", "Test producer/consumer threads"));
    return result;
}

bool Benchmarks::isRunning()
{
    return sBenchmarkRunning;
}
