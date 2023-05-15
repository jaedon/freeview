/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_VIDEORENDEREROPENGL_H
#define DEVICE_VIDEORENDEREROPENGL_H

#include "../VideoRenderer.h"

#define GL_GLEXT_PROTOTYPES

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

namespace netflix {
namespace device {
namespace esplayer {

/**
 * @class VideoRendererOpenGL VideoRendererOpenGL.h
 * @brief Implementation of video rendering over SDL.
 */
class VideoRendererOpenGL : public VideoRenderer
{
public:
    /**
     * Constructor.
     *
     * @param[in] attributes the video attributes.
     * @param[in] maxImageWidth the maximum width (in pixels) of all possible images.
     * @param[in] maxImageHeight the maximum height (in pixels) of all possible images.
     */
    VideoRendererOpenGL(const VideoAttributes& attributes,
                        uint32_t maxImageWidth, uint32_t maxImageHeight);

    /**
     * Destructor.
     */
    virtual ~VideoRendererOpenGL();

    virtual bool init();

    virtual bool lockBackBuffer(uint32_t       backBufferIndex,
                                void*          &priv,
                                unsigned char* &plane0,
                                unsigned char* &plane1,
                                unsigned char* &plane2,
                                unsigned char* &plane3,
                                uint32_t       &pitch0,
                                uint32_t       &pitch1,
                                uint32_t       &pitch2,
                                uint32_t       &pitch3);

    virtual bool unlockBackBuffer(uint32_t backBufferIndex, void* priv);

    virtual bool flipBackBuffer(uint32_t backBufferIndex,
                                uint32_t width,
                                uint32_t height,
                                void*    priv);

    virtual void threadStart();

    virtual void threadEnd();

    void setVideoWindow(Rect r);

private:
    void                    initGLX();
    void                    initGL();

    void                    mapPBO(int index);
    void                    unmapPBO(int index);
    void                    renderPBO(int index);

private:
    GLuint                  mTextureId;
    Display *               mXDisplay;
    Window                  mXWindow;
    GLXContext              mGLXContext;
    GLXWindow               mGLXWindow;

    const VideoAttributes   mAttributes;
    Rect *                  dstRect_;

    static Bool WaitForNotify( Display * , XEvent *e , char * arg )
    {
       return ( e->type == MapNotify ) && ( e->xmap.window == (Window) arg );
    }

};

} // namespace esplayer
} // namespace device
} // namespace netflix

#endif // DEVICE_VIDEORENDEREROPENGL_H
