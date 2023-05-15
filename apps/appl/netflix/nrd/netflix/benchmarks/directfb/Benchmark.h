/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <ISurface.h>
#include <tr1.h>

namespace netflix {

class Benchmark
{
public:

    typedef shared_ptr<Benchmark> SharedPtr;

    enum {
        TEST_DURATION_MS = 10000,
        TEST_DURATION_SECONDS = TEST_DURATION_MS / 1000,
    };

    Benchmark() {}
    virtual ~Benchmark() {}

    virtual unsigned run(ISurface& screen) = 0;

private:


};


class BlitBenchmark : public Benchmark
{
public:

    enum {
        BLIT_NORMAL = 0,
        BLIT_ALPHA = 1,
        BLIT_STRETCH = 2,
    };

    BlitBenchmark(unsigned flags) : mRenderFlags(flags) {}
    BlitBenchmark() : mRenderFlags(BLIT_NORMAL) {}
    virtual ~BlitBenchmark() {}

    unsigned run(ISurface& screen);

    static bool init();
    static void shutdown();

private:

    unsigned mRenderFlags;

    static ISurface::SharedPtr mImage;

};


class FillBenchmark : public Benchmark
{
public:

    enum {
        FILL_NORMAL = 0,
        FILL_ALPHA = 1,
    };

    FillBenchmark(unsigned flags) : mRenderFlags(flags) {}
    FillBenchmark() : mRenderFlags(FILL_NORMAL) {}
    virtual ~FillBenchmark() {}

    unsigned run(ISurface& screen);

private:

    unsigned mRenderFlags;

};


class TextBenchmark : public Benchmark
{
public:

    TextBenchmark() {}
    virtual ~TextBenchmark() {}

    unsigned run(ISurface& screen);
};

}

#endif // BENCHMARK_H

