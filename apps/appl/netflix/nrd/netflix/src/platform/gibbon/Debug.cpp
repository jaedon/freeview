/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Debug.h"

#include <Debug.h>
#include <Screen.h>
#include <Widget.h>
#include <GibbonApplication.h>
#include <GibbonConfiguration.h>

#include <nrdbase/StringTokenizer.h>

#include <iostream>

using namespace netflix;
using namespace netflix::gibbon;

bool Debug::DrawWidgetCache = false;
bool Debug::DrawWidgetRectangles = false;
bool Debug::DrawWidgetBlendMode = false;
bool Debug::DebugWidgetLayout = false;
bool Debug::DebugWidgetSize = false;
bool Debug::InstrumentWidgetAnimations = false;
bool Debug::InstrumentWidgetRender = false;
bool Debug::DebugWidgetSendRenderProperties = false;
bool Debug::DebugWidgetProperties = false;
bool Debug::DebugWidgetPropertiesPush = false;
bool Debug::DebugWidgetSync = false;
bool Debug::DebugWidgetCache = false;
bool Debug::DebugSurfaceLoad = false;
#if defined(GIBBON_SURFACEDECODER_FORK)
bool Debug::TestSurfaceLoadFork = false;
#endif
bool Debug::TestSurfaceCacheKeysDisabled = false;
int  Debug::TestSurfaceLoadDelay = 0;
bool Debug::DrawTextRectangles = false;
bool Debug::DrawTextRunRectangles = false;
bool Debug::DrawTextBaselines = false;
#ifdef GIBBON_TEXT_CALC_PADDING
bool Debug::TestTextCalculatePadding = true;
#else
bool Debug::TestTextCalculatePadding = false;
#endif
bool Debug::DebugTextLayout = false;
bool Debug::DebugTextLayoutParams = false;
bool Debug::DebugHarfbuzzClusters = true;
bool Debug::DebugHarfbuzzPositions = false;
bool Debug::DebugHarfbuzzNames = true;
bool Debug::DrawScreenRectangles = false;
bool Debug::DrawScreenFrameGraph = false;
bool Debug::TestScreenFullRender = false;
bool Debug::TestScreenFlush = false;
bool Debug::TestScreenCoordinateScale = false;
bool Debug::DebugScreenRender = false;
bool Debug::DebugScreenIdle = false;
bool Debug::TestDiskCache = false;
bool Debug::DebugAnimationFPS = false;
bool Debug::TestAnimationNoIdle = false;
bool Debug::TestAnimationNoDecode = false;
bool Debug::TestAnimationCoordIsIntegral = true;
bool Debug::TestNetworkSecure = false;
bool Debug::SendStreamInfo = false;
bool Debug::DebugRemoteFrames = false;
bool Debug::DebugRemoteSurface = false;
bool Debug::DebugScriptEvents = false;
bool Debug::TestScriptDumpHeapNoGC = false;
#ifdef GIBBON_GRAPHICSENGINE_GLES2
#if defined(BUILD_DEBUG)
bool Debug::DebugGLCheck = true;
#else
bool Debug::DebugGLCheck = false;
#endif
#endif

enum {
    DUMP_PROPERTY_REDRAW = 0x01,
    DUMP_PROPERTY_ALIAS  = 0x02,
    DUMP_PROPERTY_HIDDEN = 0x04,

    SET_PROPERTY_INT     = 0x10,
    SET_PROPERTY_STRING  = 0x20
};
static const struct DebugProperty {
    const char *name;
    const char *desc;
    void *property;
    uint8_t flags;
    inline std::string value() const {
        if(flags & SET_PROPERTY_INT)
            return StringFormatter::sformat("%d", *((int*)property));
        if(flags & SET_PROPERTY_STRING)
            return *((std::string*)property);
        return *((bool*)property) ? "enabled" : "disabled";
    }
} sDebugProperties[] = {
    { "group", "Widgets:", 0, 0 },
    { "drawWidgetRectangles", "Render: area", &Debug::DrawWidgetRectangles, DUMP_PROPERTY_REDRAW },
    { "widget", 0, &Debug::DrawWidgetRectangles, DUMP_PROPERTY_REDRAW|DUMP_PROPERTY_ALIAS  },
    { "drawWidgetCache", "Render: cache updates", &Debug::DrawWidgetCache, DUMP_PROPERTY_REDRAW  },
    { "widgetcache", 0, &Debug::DrawWidgetCache, DUMP_PROPERTY_REDRAW|DUMP_PROPERTY_ALIAS  },
    { "drawWidgetBlendMode", "Render: blended widgets", &Debug::DrawWidgetBlendMode, DUMP_PROPERTY_REDRAW },
    { "debugWidgetSize", "Debug: content size", &Debug::DebugWidgetSize, 0 },
    { "debugWidgetLayout", "Debug: layout", &Debug::DebugWidgetLayout, 0 },
    { "layout", 0, &Debug::DebugWidgetLayout, DUMP_PROPERTY_ALIAS },
    { "debugWidgetProperties", "Debug: properties", &Debug::DebugWidgetProperties, 0 },
    { "properties", 0, &Debug::DebugWidgetProperties, DUMP_PROPERTY_ALIAS },
    { "debugWidgetPropertiesPush", "Debug: property push", &Debug::DebugWidgetPropertiesPush, 0 },
    { "push", 0, &Debug::DebugWidgetPropertiesPush, DUMP_PROPERTY_ALIAS },
    { "debugWidgetSync", "Debug: sync", &Debug::DebugWidgetSync, 0 },
    { "sync", 0, &Debug::DebugWidgetSync, DUMP_PROPERTY_ALIAS },
    { "debugWidgetCache", "Debug: cache", &Debug::DebugWidgetCache, 0 },
    { "instrumentWidgetAnimation", "Instrument: widget animation time", &Debug::InstrumentWidgetAnimations, 0 },
    { "instrumentWidgetRender", "Instrument: widget render time", &Debug::InstrumentWidgetRender, 0 },
    { "debugWidgetSendRenderProperties", "Debug: send render properties", &Debug::DebugWidgetSendRenderProperties, 0 },

    { "group", "Text:", 0, 0 },
    { "drawTextRectangles", "Render: boxes", &Debug::DrawTextRectangles, DUMP_PROPERTY_REDRAW },
    { "text", 0, &Debug::DrawTextRectangles, DUMP_PROPERTY_REDRAW|DUMP_PROPERTY_ALIAS  },
    { "drawTextRunRectangles", "Render: text run boxes", &Debug::DrawTextRunRectangles, DUMP_PROPERTY_REDRAW  },
    { "textruns", 0, &Debug::DrawTextRunRectangles, DUMP_PROPERTY_REDRAW|DUMP_PROPERTY_ALIAS  },
    { "drawTextBaselines", "Render: baseline", &Debug::DrawTextBaselines, DUMP_PROPERTY_REDRAW  },
    { "textbaselines", 0, &Debug::DrawTextBaselines, DUMP_PROPERTY_REDRAW|DUMP_PROPERTY_ALIAS  },
    { "debugTextLayout", "Debug: layout", &Debug::DebugTextLayout, 0 },
    { "debugTextLayoutParams", "Debug: layout parameters", &Debug::DebugTextLayoutParams, 0 },
    { "testTextCalcPadding", "Test: enable padding calculation per run", &Debug::TestTextCalculatePadding, 0 },

    { "group", "Harfbuzz:", 0, 0 },
    { "showClusters", "Debug: show cluster IDs when dumping Harfbuzz buffers", &Debug::DebugHarfbuzzClusters, 1 },
    { "showPositions", "Debug: show glyph positions when dumping Harfbuzz buffers", &Debug::DebugHarfbuzzPositions, 0 },
    { "showNames", "Debug: show glyph names when dumping Harfbuzz buffers", &Debug::DebugHarfbuzzNames, 1 },

    { "group", "Screen:", 0, 0 },
    { "drawScreenRectangles", "Render: area", &Debug::DrawScreenRectangles, DUMP_PROPERTY_REDRAW  },
    { "drawScreenFrameGraph", "Render: frame graph", &Debug::DrawScreenFrameGraph, DUMP_PROPERTY_REDRAW  },
    { "graph", 0, &Debug::DrawScreenFrameGraph, DUMP_PROPERTY_REDRAW|DUMP_PROPERTY_ALIAS  },
    { "debugScreenRender", "Debug: render", &Debug::DebugScreenRender, 0 },
    { "debugScreenIdle", "Debug: idle", &Debug::DebugScreenIdle, DUMP_PROPERTY_HIDDEN },
    { "testScreenFullRender", "Test: full screen draws", &Debug::TestScreenFullRender, DUMP_PROPERTY_REDRAW },
    { "testScreenFlush", "Test: flush screen draws", &Debug::TestScreenFlush, DUMP_PROPERTY_REDRAW },
    { "testScreenCoordinateScale", "Test: screen coordinate scale", &Debug::TestScreenCoordinateScale, 0 },

    { "group", "DiskCache:", 0, 0 },
    { "testDiskCache", "Test: forced disk cache", &Debug::TestDiskCache, DUMP_PROPERTY_HIDDEN },

    { "group", "SurfaceCache:", 0, 0 },
    { "debugSurfaceLoad", "Debug: surface loading", &Debug::DebugSurfaceLoad, 0 },
    { "images", 0, &Debug::DebugSurfaceLoad, DUMP_PROPERTY_ALIAS  },
    { "testSurfaceLoadDelay", "Test: introduce ms during surfaceload", &Debug::TestSurfaceLoadDelay, SET_PROPERTY_INT|DUMP_PROPERTY_HIDDEN },
    { "testSurfaceCacheKeysDisabled", "Test: disable surface cache reuse", &Debug::TestSurfaceCacheKeysDisabled, DUMP_PROPERTY_HIDDEN },
#if defined(GIBBON_SURFACEDECODER_FORK)
    { "testSurfaceLoadFork", "Test: load surfaces in a separate process", &Debug::TestSurfaceLoadFork, 0},
    { "fork", 0, &Debug::TestSurfaceLoadFork, DUMP_PROPERTY_ALIAS },
#endif

    { "group", "Animation:", 0, 0 },
    { "debugAnimationFPS", "Debug: animation fps", &Debug::DebugAnimationFPS, 0 },
    { "fps", 0, &Debug::DebugAnimationFPS, DUMP_PROPERTY_ALIAS },
    { "testAnimationNoIdle", "Test: animation without sleep", &Debug::TestAnimationNoIdle, DUMP_PROPERTY_REDRAW },
    { "testAnimationNoDecode", "Test: animation without decode", &Debug::TestAnimationNoDecode, 0 },
    { "testAnimationCoordIsIntegral", "Test: animation with subpixels", &Debug::TestAnimationCoordIsIntegral, DUMP_PROPERTY_HIDDEN },
    { "integral", 0, &Debug::TestAnimationCoordIsIntegral, DUMP_PROPERTY_ALIAS  },

    { "group", "Network:", 0, 0 },
    { "testNetworkSecure", "Test: trusted network", &Debug::TestNetworkSecure, DUMP_PROPERTY_HIDDEN },

    { "group", "Playback:", 0, 0 },
    { "sendStreamInfo", "Playback: send stream info", &Debug::SendStreamInfo, 0 },

    { "group", "Remote:", 0, 0 },
    { "debugRemoteFrames", "Debug: frame render", &Debug::DebugRemoteFrames, 0 },
    { "debugRemoteSurfaces", "Debug: surface load", &Debug::DebugRemoteSurface, 0 },

    { "group", "ScriptEngine:", 0, 0 },
    { "debugScriptEvents", "Debug: events between JS", &Debug::DebugScriptEvents, 0 },
    { "events", 0, &Debug::DebugScriptEvents, DUMP_PROPERTY_ALIAS  },
    { "testScriptDumpHeapNoGC", "Test: heap dump without GC", &Debug::TestScriptDumpHeapNoGC, 0 },

#ifdef GIBBON_GRAPHICSENGINE_GLES2
    { "group", "GLES2:", 0, 0 },
    { "debugGLCheck", "Debug: enable glError debugging", &Debug::DebugGLCheck, 0 },
#endif

    { 0, 0, 0, 0 }
};

void Debug::init()
{
    const std::string debugFlags = GibbonConfiguration::debugFlags();
    if(!debugFlags.empty()) {
        std::vector<std::string> properties;
        StringTokenizer::tokenize(debugFlags, properties, " ,");
        for(size_t i = 0; i < properties.size(); ++i)
            setProperty(properties[i]);
        dump(DUMP_CONSOLE);
    }
}

std::vector<Console::Command::Help> Debug::help()
{
    std::vector<Console::Command::Help> result;
    for (int i = 0; sDebugProperties[i].name; ++i) {
        if(sDebugProperties[i].property && sDebugProperties[i].desc)
            result.push_back(Console::Command::Help(sDebugProperties[i].name, sDebugProperties[i].desc));
    }
    return result;
}

int Debug::setProperty(std::string property, Variant variant)
{
    if(variant.isNull()) {
        const size_t equal = property.find("=");
        if(equal != std::string::npos) {
            variant = property.substr(equal+1, property.length()-equal);
            property = property.substr(0, equal);
        }
    }
    if(property.empty() || property == "help" || property == "help_all") {
        dump(property == "help_all" ? DUMP_HIDDEN : 0);
        return 1;
    } else if(property == "sendStreamInfo") {
        NrdConfiguration::setSendStreamInfoEvents(variant.value<bool>(0, true));
        return 1;
    }

    for(int i = 0; sDebugProperties[i].name; ++i) {
        const DebugProperty *p = sDebugProperties+i;
        if(p->property && property == p->name) {
            if(p->flags & SET_PROPERTY_INT)
                *((int*)p->property) = variant.value<int>(0, 0);
            else if(p->flags & SET_PROPERTY_STRING)
                *((std::string*)p->property) = variant.value<std::string>(0, "");
            else
                *((bool*)p->property) = variant.value<bool>(0, !*((bool*)sDebugProperties[i].property));
            if((p->flags & DUMP_PROPERTY_REDRAW) && GibbonApplication::instance()->getScreen()) {
                Widget::purgeAll(Widget::Purge_Force);
                GibbonApplication::instance()->getScreen()->update();
            }
            if(shared_ptr<GibbonBridge> bridge = GibbonApplication::instance()->getBridge())
                bridge->updateDebug();
            Log::warn(TRACE_UI_ENGINE, "Debug: %s: %s", property.c_str(), p->value().c_str());
            return 1;
        }
    }
    Log::error(TRACE_UI_ENGINE, "Cannot find debug: %s", property.c_str());
    return -1;
}

Variant Debug::values()
{
    Variant result;
    for(int i = 0; sDebugProperties[i].name; ++i) {
        if(sDebugProperties[i].flags & DUMP_PROPERTY_ALIAS)
            continue;
        if(const DebugProperty *p = sDebugProperties+i) {
            if(!p->property)
                continue;
            if(p->flags & SET_PROPERTY_INT)
                result[p->name] = *((int*)p->property);
            else if(p->flags & SET_PROPERTY_STRING)
                result[p->name] = *((std::string*)p->property);
            else
                result[p->name] = *((bool*)p->property);
        }
    }
    return result;
}

void Debug::dump(unsigned char flags)
{
    std::string group;
    for(int i = 0; sDebugProperties[i].name; ++i) {
        if(sDebugProperties[i].flags & DUMP_PROPERTY_ALIAS)
            continue;
        if((sDebugProperties[i].property) ||
           (sDebugProperties[i].desc && ((flags & DUMP_HIDDEN) || !(sDebugProperties[i].flags & DUMP_PROPERTY_HIDDEN)))) {
            if(sDebugProperties[i].property) {
                if(!group.empty()) {
                    if(flags & DUMP_CONSOLE)
                        Log::printf("\n%s\n", group.c_str());
                    else
                        Log::warn(TRACE_UI_ENGINE) << " ";
                        Log::warn(TRACE_UI_ENGINE) << " " << group;
                    group.clear();
                }
                std::string aliases;
                for(int i2 = i+1; sDebugProperties[i2].name && sDebugProperties[i2].flags & DUMP_PROPERTY_ALIAS; ++i2) {
                    if(!aliases.empty())
                        aliases += ", ";
                    aliases += sDebugProperties[i2].name;
                }
                if(flags & DUMP_CONSOLE) {
                    Log::printf(" Debug: %-25s: %s: %s\n", sDebugProperties[i].name,
                                sDebugProperties[i].desc ? sDebugProperties[i].desc : "N/A",
                                sDebugProperties[i].value().c_str());
                } else {
                    Log::warn(TRACE_UI_ENGINE) << std::setw(36) << sDebugProperties[i].name << " - "
                                               << (sDebugProperties[i].desc ? sDebugProperties[i].desc : "N/A")
                                               << (aliases.empty() ? "" : (" (" + aliases + ")"))
                                               << " [" << sDebugProperties[i].value().c_str() << "] ";
                }
            } else {
                group = sDebugProperties[i].desc;
            }
        }
    }
}


