/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <directfb.h>
#include <DFBSurface.h>
#include <Point.h>
#include <Timer.h>
#include <Benchmark.h>
#include <iostream>

using namespace std;
using namespace netflix;


#define DFBCHECK(x...)                                         \
{                                                              \
    DFBResult err = x;                                         \
                                                               \
    if (err != DFB_OK)                                         \
    {                                                          \
        fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
        DirectFBErrorFatal( #x, err );                         \
    }                                                          \
}

static void dumpDirectFBInfo(IDirectFB* dfb)
{
    DFBGraphicsDeviceDescription dgdd;
    DFBResult result = dfb->GetDeviceDescription(dfb, &dgdd);
    if (result == DFB_OK)
    {
        cout << "DirectFB Device Description -------------------------------------" << endl;
        cout << "Device Name: " << dgdd.name << endl;
        cout << "Device Vendor: " << dgdd.vendor << endl;
        cout << "Driver Name: " << dgdd.driver.name << endl;
        cout << "Driver Vendor: " << dgdd.driver.vendor << endl;
        cout << "Driver Version: " << dgdd.driver.major << "." << dgdd.driver.minor << endl;
        cout << "Video Memory: " << dgdd.video_memory << " bytes" << endl;
        cout << "Acceleration Flags:" << endl;
        cout << "    FillRectangle: " << ((dgdd.acceleration_mask & DFXL_FILLRECTANGLE) != 0 ? "accelerated" : "slow") << endl;
        cout << "    DrawRectangle: " << ((dgdd.acceleration_mask & DFXL_DRAWRECTANGLE) != 0 ? "accelerated" : "slow") << endl;
        cout << "    DrawLine: " << ((dgdd.acceleration_mask & DFXL_DRAWLINE) != 0 ? "accelerated" : "slow") << endl;
        cout << "    FillTriangle: " << ((dgdd.acceleration_mask & DFXL_FILLTRIANGLE) != 0 ? "accelerated" : "slow") << endl;
        cout << "    Blit: " << ((dgdd.acceleration_mask & DFXL_BLIT) != 0 ? "accelerated" : "slow") << endl;
        cout << "    StretchBlit: " << ((dgdd.acceleration_mask & DFXL_STRETCHBLIT) != 0 ? "accelerated" : "slow") << endl;
        cout << "    TexTriangles: " << ((dgdd.acceleration_mask & DFXL_TEXTRIANGLES) != 0 ? "accelerated" : "slow") << endl;
  //        cout << "    Blit2: " << ((dgdd.acceleration_mask & DFXL_BLIT2) != 0 ? "accelerated" : "slow") << endl;
        cout << "    DrawString: " << ((dgdd.acceleration_mask & DFXL_DRAWSTRING) != 0 ? "accelerated" : "slow") << endl;
        cout << "Blitting Flags:" << endl;
        cout << "    DSBLIT_BLEND_ALPHACHANNEL: " << ((dgdd.blitting_flags & DSBLIT_BLEND_ALPHACHANNEL) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSBLIT_BLEND_COLORALPHA: " << ((dgdd.blitting_flags & DSBLIT_BLEND_COLORALPHA) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSBLIT_COLORIZE: " << ((dgdd.blitting_flags & DSBLIT_COLORIZE) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSBLIT_SRC_COLORKEY: " << ((dgdd.blitting_flags & DSBLIT_SRC_COLORKEY) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSBLIT_DST_COLORKEY: " << ((dgdd.blitting_flags & DSBLIT_DST_COLORKEY) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSBLIT_SRC_PREMULTIPLY: " << ((dgdd.blitting_flags & DSBLIT_SRC_PREMULTIPLY) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSBLIT_DST_PREMULTIPLY: " << ((dgdd.blitting_flags & DSBLIT_DST_PREMULTIPLY) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSBLIT_DEMULTIPLY: " << ((dgdd.blitting_flags & DSBLIT_DEMULTIPLY) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSBLIT_DEINTERLACE: " << ((dgdd.blitting_flags & DSBLIT_DEINTERLACE) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSBLIT_SRC_PREMULTCOLOR: " << ((dgdd.blitting_flags & DSBLIT_SRC_PREMULTCOLOR) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSBLIT_XOR: " << ((dgdd.blitting_flags & DSBLIT_XOR) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSBLIT_INDEX_TRANSLATION: " << ((dgdd.blitting_flags & DSBLIT_INDEX_TRANSLATION) != 0 ? "supported" : "n/a") << endl;
  //        cout << "    DSBLIT_ROTATE90: " << ((dgdd.blitting_flags & DSBLIT_ROTATE90) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSBLIT_ROTATE180: " << ((dgdd.blitting_flags & DSBLIT_ROTATE180) != 0 ? "supported" : "n/a") << endl;
  //        cout << "    DSBLIT_ROTATE270: " << ((dgdd.blitting_flags & DSBLIT_ROTATE270) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSBLIT_COLORKEY_PROTECT: " << ((dgdd.blitting_flags & DSBLIT_COLORKEY_PROTECT) != 0 ? "supported" : "n/a") << endl;
  //        cout << "    DSBLIT_SRC_MASK_ALPHA: " << ((dgdd.blitting_flags & DSBLIT_SRC_MASK_ALPHA) != 0 ? "supported" : "n/a") << endl;
  //        cout << "    DSBLIT_SRC_MASK_COLOR: " << ((dgdd.blitting_flags & DSBLIT_SRC_MASK_COLOR) != 0 ? "supported" : "n/a") << endl;
  //        cout << "    DSBLIT_SOURCE2: " << ((dgdd.blitting_flags & DSBLIT_SOURCE2) != 0 ? "supported" : "n/a") << endl;
  //        cout << "    DSBLIT_FLIP_HORIZONTAL: " << ((dgdd.blitting_flags & DSBLIT_FLIP_HORIZONTAL) != 0 ? "supported" : "n/a") << endl;
  //        cout << "    DSBLIT_FLIP_VERTICAL: " << ((dgdd.blitting_flags & DSBLIT_FLIP_VERTICAL) != 0 ? "supported" : "n/a") << endl;
        cout << "Drawing Flags:" << endl;
        cout << "    DSDRAW_BLEND: " << ((dgdd.drawing_flags & DSDRAW_BLEND) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSDRAW_DST_COLORKEY: " << ((dgdd.drawing_flags & DSDRAW_DST_COLORKEY) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSDRAW_SRC_PREMULTIPLY: " << ((dgdd.drawing_flags & DSDRAW_SRC_PREMULTIPLY) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSDRAW_DST_PREMULTIPLY: " << ((dgdd.drawing_flags & DSDRAW_DST_PREMULTIPLY) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSDRAW_DEMULTIPLY: " << ((dgdd.drawing_flags & DSDRAW_DEMULTIPLY) != 0 ? "supported" : "n/a") << endl;
        cout << "    DSDRAW_XOR: " << ((dgdd.drawing_flags & DSDRAW_XOR) != 0 ? "supported" : "n/a") << endl;
        cout << "-----------------------------------------------------------------" << endl;
    }

}


struct BenchmarkTest {
    const char* name;
    Benchmark::SharedPtr bm;
};

static BenchmarkTest sBenchTests[] = {
    {"Fill", Benchmark::SharedPtr(new FillBenchmark())},
    {"Fill + Alpha", Benchmark::SharedPtr(new FillBenchmark(FillBenchmark::FILL_ALPHA))},
    {"Blit", Benchmark::SharedPtr(new BlitBenchmark())},
    {"Blit + Alpha", Benchmark::SharedPtr(new BlitBenchmark(BlitBenchmark::BLIT_ALPHA))},
    {"StretchBlit", Benchmark::SharedPtr(new BlitBenchmark(BlitBenchmark::BLIT_STRETCH))},
    {"StretchBlit + Alpha", Benchmark::SharedPtr(new BlitBenchmark(BlitBenchmark::BLIT_ALPHA | BlitBenchmark::BLIT_STRETCH))},
    {"Text", Benchmark::SharedPtr(new TextBenchmark())},
    { NULL, Benchmark::SharedPtr() }, // end
};


int main (int argc, char **argv)
{
    IDirectFB *dfb = NULL;

    DFBCHECK (DirectFBInit (&argc, &argv));
    DFBCHECK (DirectFBCreate (&dfb));
    //DFBCHECK (dfb->SetCooperativeLevel (dfb, DFSCL_FULLSCREEN));

    dumpDirectFBInfo(dfb);

    DFBSurface::setDirectFB(dfb);

    DFBFontDescription fontDesc;
    memset(&fontDesc, 0, sizeof(fontDesc));
    fontDesc.flags = (DFBFontDescriptionFlags)(DFDESC_HEIGHT|DFDESC_ATTRIBUTES);
    fontDesc.height = 30;
    IDirectFBFont* font = 0;

    DFBResult err = dfb->CreateFont(dfb, "Arial Bold.ttf", &fontDesc, &font);
    if (err == DFB_OK) {
        DFBSurface::setDirectFBFont(font);
    } else {
        cout << "Error ("<< err << ") loading font" << endl;
    }


    ISurface::SharedPtr screenSurface = ISurface::create(1280, 720, ISurface::PIXELFORMAT_DEFAULT, ISurface::FLAGS_SCREEN_SURFACE);

    BlitBenchmark::init();

    unsigned count = 0;
    Rect screenRect(0, 0, screenSurface->getWidth(), screenSurface->getHeight());
    Color fillColor;

    cout << "##> benchmark start" << endl;

    for (BenchmarkTest* test = sBenchTests; test->name != 0; ++test) {
        count = test->bm->run(*screenSurface);

        screenSurface->fill(screenRect, fillColor);

        int fps = count / Benchmark::TEST_DURATION_SECONDS;
        cout << "##> " << test->name << " FPS: " << fps << endl;
    }

    cout << "##> benchmark end" << endl;

    BlitBenchmark::shutdown();

    dfb->Release(dfb);

    return 0;
}

