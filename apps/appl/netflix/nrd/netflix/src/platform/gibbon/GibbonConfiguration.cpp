/*
 * (c) 1997-2015 Netflix, Inc. All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <gibbon/config.h>

#include "Screen.h"
#include "GibbonConfiguration.h"

#include <stdlib.h>
#include <iostream>
#include <sstream>

#include <nrdbase/UnionCast.h>

#include <nrd/AppLog.h>

using namespace std;
using namespace netflix::gibbon;
using namespace netflix;

llong GibbonConfiguration::sConsoleJSDepth = 5;
llong GibbonConfiguration::sConsoleJSMaxLength = 0;
llong GibbonConfiguration::sFPSTarget = 60;
std::string GibbonConfiguration::sKeyRepeatStyle = "PRESS+RELEASE+PRESS+RELEASE";
llong GibbonConfiguration::sKeyAutoRepeatDelay = 200;
llong GibbonConfiguration::sKeyAutoRepeatInterval = 80;
bool GibbonConfiguration::sScreenRenderDisplayList = false;
bool GibbonConfiguration::sRemoteScreenEnabled = false;
llong GibbonConfiguration::sRemoteScreenPort = 0;
bool GibbonConfiguration::sCacheTextRender = false;
llong GibbonConfiguration::sSurfaceLoadThreads = 3;
llong GibbonConfiguration::sSurfaceLoadIdle = 0;
llong GibbonConfiguration::sSurfaceCacheCapacity = -1;
llong GibbonConfiguration::sFontStoreCapacity = 5 * 1024 * 1024;
llong GibbonConfiguration::sSurfaceCachePlaybackCapacity = -1;
llong GibbonConfiguration::sAnimationSurfaceLoadThreads = -1;
llong GibbonConfiguration::sFontFaceCacheCapacity = 10;
llong GibbonConfiguration::sFontGlyphCacheWidth = 1024;
llong GibbonConfiguration::sFontGlyphCacheHeight = 1024;
#if defined(BUILD_DEBUG)
bool GibbonConfiguration::sFontVerifyCapacity = false;
#else
bool GibbonConfiguration::sFontVerifyCapacity = true;
#endif
bool GibbonConfiguration::sRichTextWarnings = true;
llong GibbonConfiguration::sSurfaceAtlasWidth = 0;
llong GibbonConfiguration::sSurfaceAtlasHeight = 0;
std::string GibbonConfiguration::sUserName = "";
std::string GibbonConfiguration::sUserPassword = "";
bool GibbonConfiguration::sSendSecureCookies = false;
std::vector<std::string> GibbonConfiguration::sCookieFilters;
std::string GibbonConfiguration::sPlatform = GIBBON_PLATFORM;
std::string GibbonConfiguration::sBenchmark = "";
std::string GibbonConfiguration::sDebugFlags = "";
std::string GibbonConfiguration::sSplashScreen = "animated";
bool GibbonConfiguration::sWatchdogEnabled = false;
#if defined(GIBBON_SURFACEDECODER_PLATFORM)
bool GibbonConfiguration::sPlatformSurfaceDecoder = false;
#endif
#ifdef HAVE_LIBEDIT
bool GibbonConfiguration::sLibEditEnabled = true;
#else
bool GibbonConfiguration::sLibEditEnabled = false;
#endif

std::string GibbonConfiguration::sTextFontPath = "fonts";
std::string GibbonConfiguration::sDefaultLocale = "en-US";
std::string GibbonConfiguration::sDefaultDirection = "ltr";

std::string GibbonConfiguration::sAlphaSurfaceFormat;
std::string GibbonConfiguration::sOpaqueSurfaceFormat;
std::string GibbonConfiguration::sTextSurfaceFormat;

llong GibbonConfiguration::sGarbageCollectMaxBytesPerCycle = 0;
llong GibbonConfiguration::sGarbageCollectTimeout = 60000;
llong GibbonConfiguration::sGarbageCollectInitialTimeout = 30000;
bool GibbonConfiguration::sBackdoorGarbageCollect = true;

Variant GibbonConfiguration::sInjectJS;

llong GibbonConfiguration::sDefaultNetworkTimeout = 30000;
llong GibbonConfiguration::sDefaultNetworkConnectTimeout = 8000;
llong GibbonConfiguration::sDefaultNetworkStallTimeout = 8000;

llong GibbonConfiguration::sJSHighWaterMark = 60 * 1024 * 1024;
bool GibbonConfiguration::sJSReportStack = false;
#ifdef NRDP_USE_JSC_FILE_LINE_TRACKING
std::string GibbonConfiguration::sJSAllocLogPath;
#endif

bool GibbonConfiguration::sSurfaceAtlasEnabled = false;

bool GibbonConfiguration::sShutdownThreadEnabled = true;

Variant GibbonConfiguration::sPrefetchUrls;
std::string GibbonConfiguration::sPrefetchKey;
bool GibbonConfiguration::sPrefetchUrlsSet = false;
#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
bool GibbonConfiguration::sDFBVideo = false;
#endif
#ifdef GIBBON_GRAPHICSENGINE_GLES2
llong GibbonConfiguration::sMultisampleBuffers = 0;
bool GibbonConfiguration::sUploadInThreads = false;
#endif
bool GibbonConfiguration::sUseDirectContext = false;

std::string GibbonConfiguration::sUICAFile;
llong GibbonConfiguration::sScreenWidth = 1280;
llong GibbonConfiguration::sScreenHeight = 720;
llong GibbonConfiguration::sUIWidth = 1280;
llong GibbonConfiguration::sUIHeight = 720;
GibbonConfiguration::AspectRatio GibbonConfiguration::sUIAspectRatio = GibbonConfiguration::AspectDefault;
bool GibbonConfiguration::sShowFps = false;

bool GibbonConfiguration::hasInjectJS(const std::string& url)
{
    if (sInjectJS.type() == Variant::Type_Array) {
        Variant::ArrayIterator it = sInjectJS.arrayBegin();
        const Variant::ArrayIterator end = sInjectJS.arrayEnd();
        while (it != end) {
            if (it->mapValue<std::string>("url") == url)
                return true;
            ++it;
        }
    }
    return false;
}

bool GibbonConfiguration::removeInjectJS(const std::string& url)
{
    if (sInjectJS.type() == Variant::Type_Array) {
        int pos = 0;
        Variant::ArrayIterator it = sInjectJS.arrayBegin();
        const Variant::ArrayIterator end = sInjectJS.arrayEnd();
        while (it != end) {
            if (it->mapValue<std::string>("url") == url) {
                sInjectJS.remove(pos);
                return true;
            }
            ++pos;
            ++it;
        }
    }
    return false;
}

bool GibbonConfiguration::addInjectJS(ConfigurationOption::FunctionMode mode, Variant &variant)
{
    if(mode == ConfigurationOption::Mode_Set) {
        bool ok = true;
        Variant var;
        if (variant.isString()) {
            var = Variant::fromJSON(variant.value<std::string>(), &ok);
            if (!ok) {
                // assume the variant is the actual url
                std::string url = variant.string();
                Configuration::resolveUrlArgument(url);
                if (hasInjectJS(url))
                    return true;
                var["url"] = url;
                var["ignoreLocal"] = true;
                sInjectJS.push_back(var);
                return true;
            }
        } else if (variant.isStringMap()) {
            var = variant;
        }
        if (!var.isStringMap())
            return false;
        assert(var.isStringMap());
        std::string url = var.mapValue<std::string>("url");
        Configuration::resolveUrlArgument(url);
        if (hasInjectJS(url))
            return true;
        sInjectJS.push_back(var);
        sInjectJS.back()["url"] = url;
    } else if(mode == ConfigurationOption::Mode_Get) {
        variant = sInjectJS;
    } else {
        assert(false);
    }
    return true;
}

//------------------------------------------------------------------------------
// isValid
//
//------------------------------------------------------------------------------
bool GibbonConfiguration::isValid()
{
    {
        struct {
            llong value;
            const char *name;
        } const values[] = {
            { sFPSTarget, "--fps-target" },
            { sKeyAutoRepeatDelay, "--key-auto-repeat-delay" },
            { sKeyAutoRepeatInterval, "--key-auto-repeat-interval" },
            { sSurfaceLoadThreads, "--surface-load-threads" },
            { 0, 0 }
        };
        for (int i=0; values[i].name; ++i) {
            if(values[i].value < 0) {
                fprintf(stderr, "%s must be >= 0! (%lld)\n", values[i].name, values[i].value);
                return false;
            }
        }
    }
    if (sSurfaceCacheCapacity < -1) { // -1 is special
        fprintf(stderr, "--surface-cache-capacity must be >= -1! (%lld)\n", sSurfaceCacheCapacity);
        return false;
    }

    if(Screen::getFormat(sTextSurfaceFormat) == Surface::Format_None) {
        fprintf(stderr, "TextFormat must be valid: '%s'\n", sTextSurfaceFormat.c_str());
        return false;
    }
    if(Screen::getFormat(sAlphaSurfaceFormat) == Surface::Format_None) {
        fprintf(stderr, "AlphaFormat must be valid: '%s'\n", sAlphaSurfaceFormat.c_str());
        return false;
    }
    if(Screen::getFormat(sOpaqueSurfaceFormat) == Surface::Format_None) {
        fprintf(stderr, "OpaqueFormat must be valid: '%s'\n", sOpaqueSurfaceFormat.c_str());
        return false;
    }

    return NrdConfiguration::isValid();
}

bool GibbonConfiguration::init(int argc, char **argv, const ConfigurationOptions &options)
{
    Configuration::setConfigFile("gibbon");
    if(!NrdConfiguration::init(argc, argv, options))
        return false;
    if(!GibbonConfiguration::isValid()) {
        fprintf(stderr, "Invalid Gibbon Configuration!\n");
        return false;
    }
#ifdef HAVE_LIBEDIT
    if (!isatty(STDOUT_FILENO))
        sLibEditEnabled = false;
#endif
    return true;
}

bool GibbonConfiguration::setSplashScreenMode(ConfigurationOption::FunctionMode mode, Variant &variant)
{
    if (mode == ConfigurationOption::Mode_Set) {
        std::string value = variant.value<std::string>();
        if(value.empty() || value == "true" || value == "1")
            value = "animated";
        else if(value == "false" || value == "0")
            value = "none";
        GibbonConfiguration::sSplashScreen = value;
    } else if(mode == ConfigurationOption::Mode_Get) {
        variant = GibbonConfiguration::sSplashScreen;
    } else if(mode == ConfigurationOption::Mode_Flip) {
        if(GibbonConfiguration::sSplashScreen == "none")
            GibbonConfiguration::sSplashScreen = "animated";
        else
            GibbonConfiguration::sSplashScreen = "none";
    }
    return true;
}

bool GibbonConfiguration::parsePrefetchUrl(ConfigurationOption::FunctionMode mode, Variant &variant)
{
    if (mode == ConfigurationOption::Mode_Set) {
        bool ok;
        const Variant var = Variant::fromJSON(variant.value<std::string>(), &ok);
        if (!ok || !var.isStringMap())
            return false;
        sPrefetchUrls = var;
        sPrefetchUrlsSet = true;
    } else if(mode == ConfigurationOption::Mode_Get) {
        variant = sPrefetchUrls.toJSON();
    } else {
        assert(false);
    }
    return true;
}

ConfigurationOptions GibbonConfiguration::defaultOptions()
{
    ConfigurationOptions options = NrdConfiguration::defaultOptions();
    options.push_back("Gibbon Options");
    options.push_back(ConfigurationOption(0, "console-js-depth", ConfigurationOption::INT_ARGUMENT,
                                          &sConsoleJSDepth, "Depth to debug objects in the console."));
    options.push_back(ConfigurationOption(0, "console-js-max-length", ConfigurationOption::INT_ARGUMENT,
                                          &sConsoleJSMaxLength, "Max size of console JSON output for JS."));
    options.push_back(ConfigurationOption(0, "user-name", ConfigurationOption::STRING_ARGUMENT,
                                          &sUserName, "Login username."));
    options.push_back(ConfigurationOption(0, "user-password", ConfigurationOption::STRING_ARGUMENT,
                                          &sUserPassword, "Login username."));
    options.push_back(ConfigurationOption(0, "debug-flags", ConfigurationOption::STRING_ARGUMENT,
                                          &sDebugFlags, "Debug flags for gibbon rendering"));
    options.push_back(ConfigurationOption(0, "surface-cache-capacity", ConfigurationOption::SIZE_ARGUMENT,
                                          &sSurfaceCacheCapacity, "Surface cache capacity."));
    options.push_back(ConfigurationOption(0, "surface-cache-playback-capacity", ConfigurationOption::SIZE_ARGUMENT,
                                          &sSurfaceCachePlaybackCapacity, "Surface cache capacity during playback."));
    options.push_back(ConfigurationOption(0, "fps-target", ConfigurationOption::INT_ARGUMENT,
                                          &sFPSTarget, "FPS target rate."));
    options.push_back(ConfigurationOption(0, "key-repeat-style", ConfigurationOption::STRING_ARGUMENT,
                                          &sKeyRepeatStyle, "Style for key repeats."));
    options.push_back(ConfigurationOption(0, "key-repeat-interval", ConfigurationOption::TIME_ARGUMENT,
                                          &sKeyAutoRepeatInterval, "Interval for key repeats."));
    options.push_back(ConfigurationOption(0, "key-repeat-delay", ConfigurationOption::INT_ARGUMENT,
                                          &sKeyAutoRepeatDelay, "Delay before entering key repeat."));
    options.push_back(ConfigurationOption(0, "animation-image-threads", ConfigurationOption::INT_ARGUMENT,
                                          &sAnimationSurfaceLoadThreads, 0));
    options.push_back(ConfigurationOption(0, "animation-surface-load-threads", ConfigurationOption::INT_ARGUMENT,
                                          &sAnimationSurfaceLoadThreads, "Number of threads used for surface loading during animation."));
    options.push_back(ConfigurationOption(0, "image-threads", ConfigurationOption::INT_ARGUMENT,
                                          &sSurfaceLoadThreads, 0));
    options.push_back(ConfigurationOption(0, "surface-load-threads", ConfigurationOption::INT_ARGUMENT,
                                          &sSurfaceLoadThreads, "Number of threads used for surface cache loading."));
    options.push_back(ConfigurationOption(0, "surface-load-idle", ConfigurationOption::TIME_ARGUMENT,
                                          &sSurfaceLoadIdle, "Amount of time to consider idle and begin loading offscreen jobs."));
    options.push_back(ConfigurationOption(0, "screen-render-display-list", &sScreenRenderDisplayList, "Enable rendering to screen via DisplayList."));
    options.push_back(ConfigurationOption(0, "remote-screen", &sRemoteScreenEnabled, "Enable RemoteScreen."));
    options.push_back(ConfigurationOption(0, "remote-screen-port", ConfigurationOption::INT_ARGUMENT,
                                          &sRemoteScreenPort, "Set the RemoteScreen port to <value>."));
    options.push_back(ConfigurationOption(0, "watchdog", &sWatchdogEnabled, "Enable watchdog."));
    options.push_back(ConfigurationOption(0, "libedit", &sLibEditEnabled, "Enable libedit (readline-emulation)."));
    options.push_back(ConfigurationOption('K', "send-secure-cookies", &sSendSecureCookies, "Send secure cookies over non-secure connections."));
    options.push_back(ConfigurationOption('F', "cookie-filter", &GibbonConfiguration::setCookieFilters, "Add filter for cookie manager."));

    options.push_back(ConfigurationOption(0, "splash-screen", ConfigurationOption::BOOL_ARGUMENT, setSplashScreenMode, "Splash screen mode."));
#if defined(GIBBON_SURFACEDECODER_PLATFORM)
    options.push_back(ConfigurationOption(0, "platform-surface-decoders", &sPlatformSurfaceDecoder, "Enable platform surface decoders."));
#endif
    options.push_back(ConfigurationOption(0, "garbage-collect-max-bytes-per-cycle", ConfigurationOption::SIZE_ARGUMENT,
                                          &sGarbageCollectMaxBytesPerCycle, "Max bytes per cycle that GC is considered."));
    options.push_back(ConfigurationOption(0, "garbage-collect-timeout", ConfigurationOption::TIME_ARGUMENT,
                                          &sGarbageCollectTimeout, "Timeout for JS garbage collect (in ms)."));
    options.push_back(ConfigurationOption(0, "garbage-collect-initial-timeout", ConfigurationOption::TIME_ARGUMENT,
                                          &sGarbageCollectInitialTimeout, "Timeout for initial JS garbage collect (in ms)."));
    options.push_back(ConfigurationOption(0, "backdoor-garbage-collect", &sBackdoorGarbageCollect, "Use NRDP specific garbage collect"));
    options.push_back(ConfigurationOption(0, "inject-js", &GibbonConfiguration::addInjectJS, "Inject JS source file wrapped in a function"));
    options.push_back(ConfigurationOption(0, "js-capacity", ConfigurationOption::SIZE_ARGUMENT,
                                          &sJSHighWaterMark, "High water mark for JS"));
    options.push_back(ConfigurationOption(0, "js-report-stack",
                                          &sJSReportStack, "Report JS stack trace for JS page heap increases"));
#ifdef NRDP_USE_JSC_FILE_LINE_TRACKING
    options.push_back(ConfigurationOption(0, "js-alloc-log-path", ConfigurationOption::PATH_ARGUMENT,
                                          &sJSAllocLogPath, "Path to record all javascript allocations by callstack"));
#endif
#if GIBBON_BENCHMARKS
    options.push_back(ConfigurationOption(0, "run-benchmark", ConfigurationOption::STRING_ARGUMENT,
                                          &sBenchmark, "Run a benchmark"));
#endif
    options.push_back(ConfigurationOption(0, "gibbon-platform", ConfigurationOption::STRING_ARGUMENT,
                                          &sPlatform, "Selected Gibbon platform."));
    options.push_back(ConfigurationOption(0, "alpha-format", ConfigurationOption::STRING_ARGUMENT,
                                          &sAlphaSurfaceFormat, "Set alpha surface format."));
    options.push_back(ConfigurationOption(0, "opaque-format", ConfigurationOption::STRING_ARGUMENT,
                                          &sOpaqueSurfaceFormat, "Set opaque surface format."));
    options.push_back(ConfigurationOption(0, "text-format", ConfigurationOption::STRING_ARGUMENT,
                                          &sTextSurfaceFormat, "Set text surface format."));
    options.push_back(ConfigurationOption(0, "cache-text-render", &sCacheTextRender, "Whether to force rendering text into a surface"));
    options.push_back(ConfigurationOption(0, "default-network-timeout", ConfigurationOption::INT_ARGUMENT,
                                          &sDefaultNetworkTimeout, "Default network timeout"));
    options.push_back(ConfigurationOption(0, "default-network-connect-timeout", ConfigurationOption::TIME_ARGUMENT,
                                          &sDefaultNetworkConnectTimeout, "Default network connect timeout"));
    options.push_back(ConfigurationOption(0, "default-network-stall-timeout", ConfigurationOption::TIME_ARGUMENT,
                                          &sDefaultNetworkStallTimeout, "Default network stall timeout"));
    options.push_back(ConfigurationOption(0, "shutdown-thread-enabled", &sShutdownThreadEnabled, "Enable shutdown thread"));

    options.push_back("Gibbon UI Options");
    options.push_back(ConfigurationOption('c', "ui-cert", ConfigurationOption::DATAPATH_ARGUMENT,
                                          &sUICAFile, "Override location of UI CA file."));
    options.push_back(ConfigurationOption(0, "screen-width", ConfigurationOption::INT_ARGUMENT,
                                          &sScreenWidth, "Represents the output width of the screen to be rendered to."));
    options.push_back(ConfigurationOption(0, "screen-height", ConfigurationOption::INT_ARGUMENT,
                                          &sScreenHeight, "Represents the output height of the screen to be rendered to."));
    options.push_back(ConfigurationOption(0, "ui-width", ConfigurationOption::INT_ARGUMENT,
                                          &sUIWidth, "Represents the input width of the UI to be rendered from."));
    options.push_back(ConfigurationOption(0, "ui-height", ConfigurationOption::INT_ARGUMENT,
                                          &sUIHeight, "Represents the input height of the UI to be rendered from."));
    options.push_back(ConfigurationOption(0, "ui-aspect-ratio", &GibbonConfiguration::setUIAspectRatio,
                                          "Represents the input aspect ratio of the UI to be rendered from."));
    options.push_back(ConfigurationOption('f', "show-fps", &sShowFps, "Show FPS counter."));
    options.push_back(ConfigurationOption(0, "prefetch-url", &GibbonConfiguration::parsePrefetchUrl, "Override prefetch urls."));
    options.push_back(ConfigurationOption(0, "prefetch-key", ConfigurationOption::STRING_ARGUMENT,
                                          &sPrefetchKey, "Set prefetch keys."));

    options.push_back("Gibbon Text Engine Options");
    options.push_back(ConfigurationOption(0, "text-font-path", ConfigurationOption::DATAPATH_ARGUMENT,
                                          &sTextFontPath, "Font directory"));
    options.push_back(ConfigurationOption(0, "default-locale", ConfigurationOption::STRING_ARGUMENT,
                                          &sDefaultLocale, "Default locale"));
    options.push_back(ConfigurationOption(0, "default-direction", ConfigurationOption::STRING_ARGUMENT,
                                          &sDefaultDirection, "Default Direction"));
    options.push_back(ConfigurationOption(0, "font-face-cache-capacity", ConfigurationOption::INT_ARGUMENT,
                                          &sFontFaceCacheCapacity, "Font face cache capacity"));
    options.push_back(ConfigurationOption(0, "font-glyph-cache-width", ConfigurationOption::INT_ARGUMENT,
                                          &sFontGlyphCacheWidth, "Font glyph cache surface width"));
    options.push_back(ConfigurationOption(0, "font-glyph-cache-height", ConfigurationOption::INT_ARGUMENT,
                                          &sFontGlyphCacheHeight, "Font span cache surface height"));
    options.push_back(ConfigurationOption(0, "font-store-capacity", ConfigurationOption::SIZE_ARGUMENT,
                                          &sFontStoreCapacity, "Font storage capacity."));
    options.push_back(ConfigurationOption(0, "font-verify-glyph-cache-capacity",
                                          &sFontVerifyCapacity, "Verify glyph cache cacpacity."));

    options.push_back(ConfigurationOption(0, "rich-text-warnings",
                                          &sRichTextWarnings, "Warn when an invalid rich text tag or attribute is used."));

    options.push_back("Gibbon Surface Atlas Options");
    options.push_back(ConfigurationOption(0, "surface-atlas-enabled", &sSurfaceAtlasEnabled, "Enable surface atlasing"));
    options.push_back(ConfigurationOption(0, "surface-atlas-width", ConfigurationOption::INT_ARGUMENT,
                                          &sSurfaceAtlasWidth, "Surface atlas width"));
    options.push_back(ConfigurationOption(0, "surface-atlas-height", ConfigurationOption::INT_ARGUMENT,
                                          &sSurfaceAtlasHeight, "Surface atlas height"));

#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
    options.push_back("Gibbon Graphics DirectFB Options");
    options.push_back(ConfigurationOption(0, "dfb-video", &sDFBVideo, "Use DFBSurface in DPI to render video widget"));
#endif
#ifdef GIBBON_GRAPHICSENGINE_GLES2
    options.push_back("Gibbon Graphics GLES2 Options");
    options.push_back(ConfigurationOption(0, "gl-multisample-buffers", ConfigurationOption::INT_ARGUMENT,
                                          &sMultisampleBuffers, "GL Multisample buffers"));
    options.push_back(ConfigurationOption(0, "gl-upload-in-thread", &sUploadInThreads, "Upload GL textures in threads"));
#endif
    options.push_back(ConfigurationOption(0, "glx-use-direct-context", &sUseDirectContext, "Use direct context for GLX"));

    return options;
}

std::string GibbonConfiguration::uiAspectRatioString()
{
    AspectRatio ratio = uiAspectRatio();
    if (ratio == AspectDefault)
        ratio = (float(uiWidth()) / float(uiHeight()) < 1.54f) ? Aspect4_3 : Aspect16_9; // 1.54 is sqrt(4/3*16/9)
    std::string str;
    switch (ratio) {
    case Aspect4_3:
        str = "4_3";
        break;
    case AspectDefault:
    case Aspect16_9:
        str = "16_9";
        break;
    }
    return str;
}

bool GibbonConfiguration::setUIAspectRatio(ConfigurationOption::FunctionMode mode, Variant &variant)
{
    if (mode == ConfigurationOption::Mode_Set) {
        const std::string argument = variant.value<std::string>();
        if (argument == "16:9" || argument == "16_9") {
            GibbonConfiguration::sUIAspectRatio = Aspect16_9;
            return true;
        } else if (argument == "4:3" || argument == "4_3") {
            GibbonConfiguration::sUIAspectRatio = Aspect4_3;
            return true;
        }
        return false;
    } else if(mode == ConfigurationOption::Mode_Get) {
        variant = GibbonConfiguration::uiAspectRatioString();
    } else {
        assert(false);
    }
    return true;
}

bool GibbonConfiguration::setCookieFilters(ConfigurationOption::FunctionMode mode, Variant &variant)
{
    if (mode == ConfigurationOption::Mode_Set) {
        sCookieFilters.push_back(variant.value<std::string>());
    } else if(mode == ConfigurationOption::Mode_Get) {
        variant = sCookieFilters;
    } else {
        assert(false);
    }
    return true;
}
