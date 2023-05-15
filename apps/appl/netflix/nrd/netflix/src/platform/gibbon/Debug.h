/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <gibbon/config.h>

#include <string>

#include <nrdbase/Variant.h>
#include <nrdbase/Console.h>

namespace netflix {
namespace gibbon {

class Debug
{
public:
    enum {
        DUMP_CONSOLE    = 0x01,
        DUMP_HIDDEN     = 0x02
    };
    static void init();
    static Variant values();
    static void dump(unsigned char flags);
    static int setProperty(std::string property, Variant value=Variant());
    static std::vector<Console::Command::Help> help();

    //Widget
    static bool DrawWidgetCache;
    static bool DrawWidgetRectangles;
    static bool DrawWidgetBlendMode;
    static bool DebugWidgetLayout;
    static bool DebugWidgetSize;
    static bool DebugWidgetProperties;
    static bool DebugWidgetPropertiesPush;
    static bool DebugWidgetSync;
    static bool DebugWidgetCache;
    static bool DebugWidgetSendRenderProperties;
    static bool InstrumentWidgetAnimations;
    static bool InstrumentWidgetRender;
    //Text
    static bool DrawTextRectangles;
    static bool DrawTextRunRectangles;
    static bool DrawTextBaselines;
    static bool TestTextCalculatePadding;
    static bool DebugTextLayout;
    static bool DebugTextLayoutParams;
    //Harfbuzz
    static bool DebugHarfbuzzClusters;
    static bool DebugHarfbuzzPositions;
    static bool DebugHarfbuzzNames;
    //Screen
    static bool DrawScreenFrameGraph;
    static bool DrawScreenRectangles;
    static bool DebugScreenRender;
    static bool DebugScreenIdle;
    static bool TestScreenFullRender;
    static bool TestScreenFlush;
    static bool TestScreenCoordinateScale;
    //DiskCache
    static bool TestDiskCache;
    //SurfaceCache
    static bool DebugSurfaceLoad;
    static bool TestSurfaceLoadFork;
    static int  TestSurfaceLoadDelay;
    static bool TestSurfaceCacheKeysDisabled;
    //Animation
    static bool DebugAnimationFPS;
    static bool TestAnimationNoIdle;
    static bool TestAnimationNoDecode;
    static bool TestAnimationCoordIsIntegral;
    //Network
    static bool TestNetworkSecure;
    //Playback
    static bool SendStreamInfo;
    //Remote
    static bool DebugRemoteFrames;
    static bool DebugRemoteSurface;
    //ScriptEngine
    static bool DebugScriptEvents;
    static bool TestScriptDumpHeapNoGC;
#ifdef GIBBON_GRAPHICSENGINE_GLES2
    //GL
    static bool DebugGLCheck;
#endif
};

}} // namespace netflix::gibbon

#endif // DEBUG_H
