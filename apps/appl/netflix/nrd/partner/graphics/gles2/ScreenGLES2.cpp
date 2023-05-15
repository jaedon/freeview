/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <Screen.h>
#include "OpenGLContext.h"
#include "EffectsGLES2.h"

using namespace netflix::gibbon;
using namespace netflix;

Screen::Configuration Screen::sConfiguration(-1);

Effect::SharedPtr Screen::createEffect(Effect::Type type)
{
    switch (type) {
    case Effect::Type_Transform:
        return Effect::SharedPtr(new EffectTransformGLES2);
    case Effect::Type_Desaturate:
        return Effect::SharedPtr(new EffectDesaturateGLES2);
    case Effect::Type_Blur:
        return Effect::SharedPtr(new EffectBlurGLES2);
    case Effect::Type_Tint:
        return Effect::SharedPtr(new EffectTintGLES2);
    default:
        break;
    }
    return shared_ptr<Effect>();
}

Variant Screen::graphicsInfo() const
{
    OpenGLContext *ctx = OpenGLContext::context();
    assert(ctx);

    Variant result;
    {
        Variant glLibrary;
        glLibrary["name"] = "opengl";
        glLibrary["compiled"] = "";
        glLibrary["runtime"] = ctx->version();
        if(ctx->supportsExtension("GL_NVX_gpu_memory_info")) {
            glLibrary["options"]["GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX"] = ctx->getIntegerv(0x9047);
            glLibrary["options"]["GPU_MEMORY_INFO_TOTAL_AVAILABLE_VIDMEM_NVX"] = ctx->getIntegerv(0x9048);
            glLibrary["options"]["GPU_MEMORY_INFO_CURRENT_VIDMEM_NVX"] = ctx->getIntegerv(0x9049);
        }
        glLibrary["options"]["extensions"] = ctx->extensions();
        glLibrary["options"]["renderer"] = (char*)glGetString(GL_RENDERER);
        glLibrary["options"]["vendor"] = (char*)glGetString(GL_VENDOR);
        result["libraries"] = glLibrary;
    }
    result["opengl"]["samples"] = sConfiguration.targetSamples;
    return result;
}

int Screen::glyphPadding() const
{
    const bool scaled = (GibbonConfiguration::screenWidth() != GibbonConfiguration::uiWidth()
                         || GibbonConfiguration::screenHeight() != GibbonConfiguration::uiHeight());
    return scaled ? 2 : 1;
}

void Screen::setConfiguration(const Configuration &config)
{
    shared_ptr<Screen> screen;
    {
        ScopedMutex screenLock(Screen::lock());
        sConfiguration = config;
        if(GibbonApplication::instance()) {
            screen = GibbonApplication::instance()->getScreen();
            if(screen) {
                screen->mNeedsRestart = true;
            }
        }
    }
}

void Screen::init()
{
    if (sConfiguration.targetSamples == -1)
        sConfiguration.targetSamples = GibbonConfiguration::multisampleBuffers();

    // Grab a GL context so we have one for the current thread
    const Size size = getSize();
    OpenGLContext::setScreenSize(size.width, size.height);
    OpenGLContext::ensureContext();
}

void Screen::cleanup()
{
    OpenGLContext::shutdown();
}

void Screen::makeCurrent_sys(bool)
{
}

bool Screen::canScaleDirectly() const
{
    return true;
}

void Screen::handleUpdate(const Rect &)
{
}

void Screen::flip_sys(const Rect &/*rect*/)
{
    OpenGLContext::context()->flip();
}

void Screen::flip_sys(const DisplayList::SharedPtr &displayList, const Rect &rect)
{
    flip_internal(displayList, rect);
}

void Screen::vsync_sys()
{
    vsync_internal();
}
