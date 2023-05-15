/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "VideoRendererSink.h"
#include "../ESPlayerConstants.h"
#include <nrddpi/Common.h>
#include <nrdbase/Exportable.h>
#include <nrdbase/Log.h>

using namespace netflix::device::esplayer;
using namespace netflix;

extern "C" NRDP_EXPORTABLE VideoRenderer *createVideoRenderer(const VideoAttributes &attributes,
                                                            uint32_t maxImageWidth,
                                                            uint32_t maxImageHeight)
{
    return new VideoRendererSink(attributes, maxImageWidth, maxImageHeight);
}

VideoRendererSink::VideoRendererSink(const VideoAttributes& /*attributes*/,
                                     uint32_t maxImageWidth,
                                     uint32_t maxImageHeight)

  : VideoRenderer(maxImageWidth, maxImageHeight),
    frameBuffers_(new Frame[ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS])
{
    uint32_t i;

    for(i = 0; i < ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS; i ++)
    {
        frameBuffers_[i].y_lines = maxImageHeight;
        frameBuffers_[i].u_lines = (maxImageHeight + 1) >> 1;
        frameBuffers_[i].v_lines = (maxImageHeight + 1) >> 1;

        frameBuffers_[i].y_pitch = maxImageWidth;
        frameBuffers_[i].u_pitch = (maxImageWidth + 1) >> 1;
        frameBuffers_[i].v_pitch = (maxImageWidth + 1) >> 1;

        frameBuffers_[i].y = new unsigned char[frameBuffers_[i].y_lines * frameBuffers_[i].y_pitch];
        frameBuffers_[i].u = new unsigned char[frameBuffers_[i].u_lines * frameBuffers_[i].u_pitch];
        frameBuffers_[i].v = new unsigned char[frameBuffers_[i].v_lines * frameBuffers_[i].v_pitch];
    }

    VideoRenderer::init(ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS, _FOURCC_LE_('Y','V','1','2'), true);
}

VideoRendererSink::~VideoRendererSink()
{
    uint32_t i;

    for(i = 0; i < ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS; i ++)
    {
        delete [] frameBuffers_[i].y;
        delete [] frameBuffers_[i].u;
        delete [] frameBuffers_[i].v;
    }

    delete [] frameBuffers_;
}

bool VideoRendererSink::init()
{
    return true;
}

bool VideoRendererSink::lockBackBuffer(uint32_t       backBufferIndex,
                                       void*          &priv,
                                       unsigned char* &plane0,
                                       unsigned char* &plane1,
                                       unsigned char* &plane2,
                                       unsigned char* &plane3,
                                       uint32_t       &pitch0,
                                       uint32_t       &pitch1,
                                       uint32_t       &pitch2,
                                       uint32_t       &pitch3)
{
    assert(backBufferIndex < getNumFrameBuffers());
    Frame* frame = &frameBuffers_[backBufferIndex];

    priv = NULL; // not used

    plane0 = frame->y;
    plane1 = frame->u;
    plane2 = frame->v;
    plane3 = NULL;

    pitch0 = frame->y_pitch;
    pitch1 = frame->u_pitch;
    pitch2 = frame->v_pitch;
    pitch3 = 0;

    return true;
}

bool VideoRendererSink::unlockBackBuffer(uint32_t backBufferIndex, void* /*priv*/)
{
    NRDP_UNUSED(backBufferIndex);
    assert(backBufferIndex < getNumFrameBuffers());
    return true;
}

bool VideoRendererSink::flipBackBuffer(uint32_t backBufferIndex,
                                       uint32_t width,
                                       uint32_t height,
                                       void* /*priv*/)
{
    NRDP_UNUSED(backBufferIndex);
    NRDP_UNUSED(width);
    NRDP_UNUSED(height);
    /*
    assert(backBufferIndex < getNumFrameBuffers());

    assert(width  > 0 && width  <= maxImageWidth_ &&
           height > 0 && height <= maxImageHeight_ );
    */
    return true;
}


