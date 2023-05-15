/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "OpenGLContext.h"
#include <nrd/AppLog.h>
#include <string.h>
#import "OpenGLContextOSX.h"
#import <Carbon/Carbon.h>

extern NSOpenGLContext* gibbonGLContext;
extern NSOpenGLPixelFormat* gibbonGLPixelFormat;

using namespace netflix;
using namespace netflix::gibbon;

bool OpenGLContext::init(OpenGLContext* share)
{
    mPrivate = new OpenGLContextPrivate;
    memset(mPrivate, '\0', sizeof(OpenGLContextPrivate));
    if (!share) {
        // activate the global context
        mPrivate->ctx = gibbonGLContext;
        mPrivate->fmt = gibbonGLPixelFormat;
        [mPrivate->ctx retain];
        [mPrivate->fmt retain];
        [mPrivate->ctx makeCurrentContext];
    } else {
        // create a new shared context
        mPrivate->ctx = [[[NSOpenGLContext alloc] initWithFormat:share->mPrivate->fmt shareContext:share->mPrivate->ctx] autorelease];
        mPrivate->fmt = share->mPrivate->fmt;
        [mPrivate->ctx retain];
        [mPrivate->fmt retain];
        [mPrivate->ctx makeCurrentContext];
    }
    return false;
}

void OpenGLContext::cleanup()
{
    [mPrivate->ctx release];
    [mPrivate->fmt release];
}

void OpenGLContext::flip()
{
    [mPrivate->ctx flushBuffer];
}

void* OpenGLContext::display()
{
    return 0;
}

bool OpenGLContext::renderVideo()
{
    return false;
}

void* OpenGLContext::procAddr(const std::string& proc)
{
    static CFBundleRef openGLBundle = 0;
    if (!openGLBundle) {
        openGLBundle = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));
    }

    void* function = 0;

    CFStringRef functionName = CFStringCreateWithCString(kCFAllocatorDefault, proc.c_str(), kCFStringEncodingASCII);
    if (functionName) {
        function = CFBundleGetFunctionPointerForName(openGLBundle, functionName);
        CFRelease(functionName);
    }

    return function;
}

bool OpenGLContext::supportsPlatformExtension(const std::string& /*ext*/)
{
    return false;
}
