/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef GIBBONCONFIGURATION_H
#define GIBBONCONFIGURATION_H

#include <string>
#include <nrdbase/StdInt.h>
#include <gibbon/config.h>

#include <nrd/NrdConfiguration.h>

namespace netflix {
namespace gibbon {

class GibbonConfiguration
{
public:
    static bool isValid();

    static ConfigurationOptions defaultOptions();
    static bool init(int argc, char **argv) { return init(argc, argv, defaultOptions()); }
    static bool init(int argc, char **argv, const ConfigurationOptions &options);

    static std::string keyRepeatStyle() { return sKeyRepeatStyle; }
    static llong keyAutoRepeatInterval() { return sKeyAutoRepeatInterval; }
    static llong keyAutoRepeatDelay() { return sKeyAutoRepeatDelay; }

    static llong fpsTarget() { return sFPSTarget; }

    static bool screenRenderDisplayList() { return sScreenRenderDisplayList; }

    static std::string userName() { return sUserName; }
    static std::string userPassword() { return sUserPassword; }

    static std::string textFontPath() { return sTextFontPath; }
    static bool cacheTextRender() { return sCacheTextRender; }

    static llong surfaceCacheCapacity() { return sSurfaceCacheCapacity; }
    static llong surfaceCachePlaybackCapacity() { return sSurfaceCachePlaybackCapacity; }
    static llong fontFaceCacheCapacity() { return sFontFaceCacheCapacity; }
    static llong fontGlyphCacheWidth() { return sFontGlyphCacheWidth; }
    static llong fontGlyphCacheHeight() { return sFontGlyphCacheHeight; }
    static llong fontStoreCapacity() { return sFontStoreCapacity; }
    static bool fontVerifyGlyphCacheCapacity() { return sFontVerifyCapacity; }
    static bool richTextWarnings() { return sRichTextWarnings; }

    static llong surfaceAtlasWidth() { return sSurfaceAtlasWidth; }
    static llong surfaceAtlasHeight() { return sSurfaceAtlasHeight; }

    static bool sendSecureCookies() { return sSendSecureCookies; }

    static std::string platform() { return sPlatform; }
    static std::string benchmark() { return sBenchmark; }
    static std::string debugFlags() { return sDebugFlags; }

    static bool remoteScreenEnabled() { return sRemoteScreenEnabled; }
    static llong remoteScreenPort() { return sRemoteScreenPort; }

    static bool libEditEnabled() { return sLibEditEnabled; }

    static std::string defaultLocale() { return sDefaultLocale; }
    static void setDefaultLocale(const std::string &language) { sDefaultLocale = language; }

    static std::string defaultDirection() { return sDefaultDirection; }
    static void setDefaultDirection(const std::string &direction) { sDefaultDirection = direction; }

    static std::string textSurfaceFormat() { return sTextSurfaceFormat; }
    static std::string opaqueSurfaceFormat() { return sOpaqueSurfaceFormat; }
    static std::string alphaSurfaceFormat() { return sAlphaSurfaceFormat; }

    static bool watchdogEnabled() { return sWatchdogEnabled; }

    static std::string splashScreen() { return sSplashScreen; }

    static llong surfaceLoadIdle() { return sSurfaceLoadIdle; }
    static llong surfaceLoadThreads() { return sSurfaceLoadThreads; }
    static llong animationSurfaceLoadThreads() { return sAnimationSurfaceLoadThreads; }

#if defined(GIBBON_SURFACEDECODER_PLATFORM)
    static bool platformSurfaceDecoder() { return sPlatformSurfaceDecoder; }
#endif

    static bool backdoorGarbageCollect() { return sBackdoorGarbageCollect; }
    static llong garbageCollectMaxBytesPerCycle() { return sGarbageCollectMaxBytesPerCycle; }
    static llong garbageCollectTimeout() { return sGarbageCollectTimeout; }
    static llong garbageCollectInitialTimeout() { return sGarbageCollectInitialTimeout; }
    static std::vector<std::string> cookieFilters() { return sCookieFilters; }
    static Variant injectJS() { return sInjectJS; }

    static llong consoleJSDepth() { return sConsoleJSDepth; }
    static llong consoleJSMaxLength() { return sConsoleJSMaxLength; }

    static llong jsHighWaterMark() { return sJSHighWaterMark; }
    static llong jsReportStack() { return sJSReportStack; }
#ifdef NRDP_USE_JSC_FILE_LINE_TRACKING
    static std::string jsAllocLogPath() { return sJSAllocLogPath; }
#endif

    static llong defaultNetworkTimeout() { return sDefaultNetworkTimeout; }
    static llong defaultNetworkConnectTimeout() { return sDefaultNetworkConnectTimeout; }
    static llong defaultNetworkStallTimeout() { return sDefaultNetworkStallTimeout; }

    static bool surfaceAtlasEnabled() { return sSurfaceAtlasEnabled; }

    static bool shutdownThreadEnabled() { return sShutdownThreadEnabled; }

    static Variant *prefetchUrls() { return sPrefetchUrlsSet ? &sPrefetchUrls : 0; }
    static std::string prefetchKey() { return sPrefetchKey; }
#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
    static bool useDFBVideo() { return sDFBVideo; }
#endif
#ifdef GIBBON_GRAPHICSENGINE_GLES2
    static llong multisampleBuffers() { return sMultisampleBuffers; }
    static bool uploadInDecodeThreads() { return sUploadInThreads; }
    static bool useDirectContext() { return sUseDirectContext; }
#endif

    inline static bool addInjectJS(const Variant &variant) {
        return addInjectJS(ConfigurationOption::Mode_Set, *const_cast<Variant*>(&variant));
    }
    static bool removeInjectJS(const std::string& url);

    enum AspectRatio {
        AspectDefault, Aspect16_9, Aspect4_3
    };

    static const std::string &uiCAFile() { return sUICAFile; }

    static llong screenWidth() { return sScreenWidth; }
    static void setScreenWidth(llong w) { sScreenWidth = w; }
    static llong screenHeight() { return sScreenHeight; }
    static void setScreenHeight(llong w) { sScreenHeight = w; }
    static bool showFps() { return sShowFps; }

    static llong uiWidth() { return sUIWidth; }
    static llong uiHeight() { return sUIHeight; }
    static AspectRatio uiAspectRatio() { return sUIAspectRatio; }
    static std::string uiAspectRatioString();
    static void setUIAspectRatio(AspectRatio r) { sUIAspectRatio = r; }

private:
    GibbonConfiguration();
    ~GibbonConfiguration();

private:
    static bool setWatchdogMode(ConfigurationOption::FunctionMode mode, Variant &variant);
    static bool setSplashScreenMode(ConfigurationOption::FunctionMode mode, Variant &variant);
    static bool addInjectJS(ConfigurationOption::FunctionMode mode, Variant &variant);
    static bool parsePrefetchUrl(ConfigurationOption::FunctionMode mode, Variant &variant);
    static bool setUIAspectRatio(ConfigurationOption::FunctionMode mode, Variant &variant);
    static bool setCookieFilters(ConfigurationOption::FunctionMode mode, Variant &variant);
    static bool hasInjectJS(const std::string& url);

    static llong sConsoleJSDepth;
    static llong sConsoleJSMaxLength;
    static bool sCacheAnimations;
    static std::string sUserName, sUserPassword;
    static llong sSurfaceCacheCapacity, sSurfaceCachePlaybackCapacity;
    static llong sFontFaceCacheCapacity;
    static llong sFontGlyphCacheWidth;
    static llong sFontGlyphCacheHeight;
    static llong sFontStoreCapacity;
    static bool sFontVerifyCapacity;
    static bool sRichTextWarnings;
    static llong sSurfaceAtlasWidth;
    static llong sSurfaceAtlasHeight;
    static bool sCacheTextRender;
    static std::string sTextFontPath;
    static std::string sKeyRepeatStyle;
    static llong sKeyAutoRepeatDelay, sKeyAutoRepeatInterval;
    static bool sScreenRenderDisplayList;
    static llong sFPSTarget;
    static bool sSendSecureCookies;
    static std::string sPlatform;
    static std::string sBenchmark;
    static std::string sDebugFlags;
    static std::string sSplashScreen;
    static std::vector<std::string> sCookieFilters;
#if defined(GIBBON_SURFACEDECODER_PLATFORM)
    static bool sPlatformSurfaceDecoder;
#endif
    static bool sWatchdogEnabled;
    static bool sLibEditEnabled;
    static bool sRemoteScreenEnabled;
    static llong sRemoteScreenPort;
    static std::string sDefaultLocale;
    static std::string sDefaultDirection;
    static std::string sTextSurfaceFormat;
    static std::string sAlphaSurfaceFormat;
    static std::string sOpaqueSurfaceFormat;
    static llong sSurfaceLoadThreads;
    static llong sSurfaceLoadIdle;
    static llong sGarbageCollectMaxBytesPerCycle;
    static llong sGarbageCollectTimeout;
    static llong sGarbageCollectInitialTimeout;
    static llong sAnimationSurfaceLoadThreads;
    static bool sBackdoorGarbageCollect;
    static Variant sInjectJS;
    static llong sDefaultNetworkTimeout;
    static llong sDefaultNetworkConnectTimeout;
    static llong sDefaultNetworkStallTimeout;
    static llong sJSHighWaterMark;
    static bool sJSReportStack;
#ifdef NRDP_USE_JSC_FILE_LINE_TRACKING
    static std::string sJSAllocLogPath;
#endif
    static bool sSurfaceAtlasEnabled;
    static bool sShutdownThreadEnabled;
#ifdef GIBBON_GRAPHICSENGINE_DIRECTFB
    static bool sDFBVideo;
#endif
#ifdef GIBBON_GRAPHICSENGINE_GLES2
    static llong sMultisampleBuffers;
    static bool sUploadInThreads;
#endif
    static bool sUseDirectContext;
    static bool sPrefetchUrlsSet;
    static std::string sPrefetchKey;
    static Variant sPrefetchUrls;

    static std::string sUICAFile;
    static llong sScreenWidth, sScreenHeight;
    static llong sUIWidth, sUIHeight;
    static AspectRatio sUIAspectRatio;
    static bool sShowFps;
};

}} // namespace netflix::gibbon

#endif // APPCONFIGURATION_H
