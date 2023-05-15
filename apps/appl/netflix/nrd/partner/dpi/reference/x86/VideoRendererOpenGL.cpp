/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "VideoRendererOpenGL.h"
#include "../ESPlayerConstants.h"
#include <nrdbase/Exportable.h>

#include <nrddpi/Common.h>


#include <sys/prctl.h>
#include <pthread.h>

using namespace netflix::device::esplayer;
using namespace netflix;

const int    IMAGE_WIDTH = 4096;
const int    IMAGE_HEIGHT = 4096;
const int    CHANNEL_COUNT = 4;
#define      DATA_SIZE IMAGE_WIDTH*IMAGE_HEIGHT*4
const GLenum PIXEL_FORMAT = GL_BGRA;

struct PBOentry {
    GLuint                  pboId;
    GLubyte *               pboPtr;
};

PBOentry               PBOlist[8];

VideoRenderer *createVideoRenderer(const VideoAttributes& attributes,
                                   uint32_t maxImageWidth,
                                   uint32_t maxImageHeight)
{
    NRDP_UNUSED(maxImageWidth);
    NRDP_UNUSED(maxImageHeight);
    return new VideoRendererOpenGL(attributes,
                                ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH,
                                ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT);
}

VideoRendererOpenGL::VideoRendererOpenGL(const VideoAttributes& attributes,
                                   uint32_t maxImageWidth,
                                   uint32_t maxImageHeight)

    : VideoRenderer(maxImageWidth, maxImageHeight)
    , mAttributes(attributes)
    , dstRect_(new Rect)
{
    // This call happens in the MC_PUMPING thread.


}

VideoRendererOpenGL::~VideoRendererOpenGL()
{
    delete dstRect_;
}

bool VideoRendererOpenGL::init()
{
    // Calculate the destination rectangle on the primary surface
    //
    Rect r;
    r.x = 0;
    r.y = 0;
    r.width = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH;
    r.height = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT;
    setVideoWindow(r);

    // 8-bit planar YUV in 8-bit samples
    uint32_t pixelFormat = _FOURCC_LE_('Y','V','1','2');

//    10-bit planar YUV in 16-bit samples.
//    uint32_t pixelFormat = _FOURCC_LE_('P','0','1','0');
//    Mythical fourcc.  8-bit planar YUV in 16-bit samples.
//    Vanguard outputs this.
//    uint32_t pixelFormat = _FOURCC_LE_('P','0','0','8');
    // All scaling is done by GL HW
    VideoRenderer::init(ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS, pixelFormat, true);
    return true;
}

bool VideoRendererOpenGL::lockBackBuffer(uint32_t       backBufferIndex,
                                         void*          &/*priv*/,
                                         unsigned char* &plane0,
                                         unsigned char* &plane1,
                                         unsigned char* &plane2,
                                         unsigned char* &/*plane3*/,
                                         uint32_t &pitch0,
                                         uint32_t &pitch1,
                                         uint32_t &pitch2,
                                         uint32_t &/*pitch3*/)
{
    assert(backBufferIndex < getNumFrameBuffers());

    // TODO: Use seperate PBOs for Y,U,V data
    plane0 = PBOlist[backBufferIndex].pboPtr;
    plane1 = PBOlist[backBufferIndex].pboPtr;
    plane2 = PBOlist[backBufferIndex].pboPtr;
    pitch0 = mAttributes.mImageWidth*4;
    pitch1 = mAttributes.mImageWidth*4;
    pitch2 = mAttributes.mImageWidth*4;

    return true;
}

bool VideoRendererOpenGL::unlockBackBuffer(
    uint32_t backBufferIndex, void* /*priv*/)
{
    assert(backBufferIndex < getNumFrameBuffers());
    NRDP_UNUSED(backBufferIndex);
    return true;
}

bool VideoRendererOpenGL::flipBackBuffer(
    uint32_t backBufferIndex,
    uint32_t /*width*/,
    uint32_t /*height*/,
    void* /*priv*/)
{
    assert(backBufferIndex < getNumFrameBuffers());

    unmapPBO(backBufferIndex);
    renderPBO(backBufferIndex);
    mapPBO(backBufferIndex);

    glBindTexture(GL_TEXTURE_2D, 0);
    glXSwapBuffers(mXDisplay, mGLXWindow);
    return true;
}

void VideoRendererOpenGL::initGLX()
{
    //PBOlist =  new PBOentry[ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS];
    memset(&PBOlist[0], 0, sizeof(PBOlist));

    XInitThreads();

    mXDisplay = XOpenDisplay(0);

    assert(mXDisplay);

    int screen = DefaultScreen( mXDisplay );

    std::cout << "GLX CLIENT "
        << glXGetClientString( mXDisplay , GLX_VERSION )
        << " : "
        << glXGetClientString( mXDisplay , GLX_VENDOR )
        << std::endl;

    std::cout << "GLX SERVER "
        << glXQueryServerString( mXDisplay , screen , GLX_VERSION )
        << " : "
        << glXQueryServerString( mXDisplay , screen , GLX_VENDOR )
        << std::endl;

    const int glx_attributes[] =
    {
            GLX_DOUBLEBUFFER,   True,
            GLX_STENCIL_SIZE,   1,
            GLX_RED_SIZE,       1,
            GLX_GREEN_SIZE,     1,
            GLX_BLUE_SIZE,      1,
            GLX_ALPHA_SIZE,     1,
            GLX_DEPTH_SIZE,     1,
            GLX_RENDER_TYPE,    GLX_RGBA_BIT,
            GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,

            None,
    };

    int nelements = 0;

    /* Find a FBConfig that uses RGBA.  Note that no attribute list is */
    /* needed since GLX_RGBA_BIT is a default attribute.               */
    GLXFBConfig * fbc = glXChooseFBConfig( mXDisplay , screen , glx_attributes , &nelements );
    assert( fbc );
    assert( nelements );

    XVisualInfo * vi = glXGetVisualFromFBConfig( mXDisplay , fbc[0] );
    assert( vi );

    Window root = RootWindow( mXDisplay , screen );
    assert( root );

    /* Create a colormap */
    Colormap cmap = XCreateColormap( mXDisplay , root , vi->visual , AllocNone );

    /* Create a window */
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask
            | PointerMotionMask | ButtonPressMask | ButtonReleaseMask;

    mXWindow = XCreateWindow( mXDisplay, root, 0, 0, ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH, ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT,
                              0 , vi->depth, InputOutput, vi->visual, CWBorderPixel|CWColormap|CWEventMask, &swa);
    assert( mXWindow );

    /* Fix the output size */
    XSizeHints * hints = XAllocSizeHints();
    hints->flags = PMinSize | PMaxSize;
    hints->min_width =  ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH;
    hints->max_width =  ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH;
    hints->min_height = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT;
    hints->max_height = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT;
    XSetWMNormalHints(mXDisplay, mXWindow, hints);
    XFree(hints);

    /* Create a GLX context using the first FBConfig in the list. */
    mGLXContext = glXCreateNewContext( mXDisplay , fbc[0] , GLX_RGBA_TYPE , 0 , GL_FALSE );

    XFree( vi );

    /* Create a GLX window using the same FBConfig that we used for the */
    /* the GLX context.                                                 */
    mGLXWindow = glXCreateWindow( mXDisplay , fbc[0], mXWindow, 0);

    XFree( fbc );

    Atom wmDeleteMessage = XInternAtom( mXDisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols( mXDisplay, mXWindow, &wmDeleteMessage, 1);

    XSetStandardProperties( mXDisplay , mXWindow , "Netflix OpenGL renderer" , "Netflix OpenGL renderer" , None, NULL , 0 , NULL );

    XMapWindow( mXDisplay , mXWindow );

    XEvent event;
    XIfEvent( mXDisplay , &event, WaitForNotify, reinterpret_cast< char* >( mXWindow )  );

    /* Connect the context to the window for read and write */
    glXMakeContextCurrent( mXDisplay, mGLXWindow, mGLXWindow, mGLXContext );
}

void VideoRendererOpenGL::initGL()
{
    glShadeModel(GL_FLAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(0, 0, 0, 0);
    glClearStencil(0);
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, IMAGE_WIDTH, IMAGE_HEIGHT, 0, PIXEL_FORMAT, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    for (uint32_t i = 0; i < ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS; i++) {
        glGenBuffersARB(1, &PBOlist[i].pboId);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, PBOlist[i].pboId);
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, DATA_SIZE, NULL, GL_STREAM_DRAW_ARB);
        PBOlist[i].pboPtr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
    }
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    glViewport(0, 0, ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH, ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT);
}

void VideoRendererOpenGL::unmapPBO(int i)
{
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, PBOlist[i].pboId);
    glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
}

void VideoRendererOpenGL::mapPBO(int i)
{
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, PBOlist[i].pboId);
    glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, DATA_SIZE, NULL, GL_STREAM_DRAW_ARB);
    PBOlist[i].pboPtr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
}

void VideoRendererOpenGL::renderPBO(int i)
{
    // Copy pixels from PBO to texture object asynchronously
    // Graphics driver can DMA the data without blocking.
    // It appears PBO to texture operations require GLX context to be created in "direct" mode.
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, PBOlist[i].pboId);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, NULL);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // TODO: Use shaders to convert YUV date to RGB and remove immediate mode rendering.
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glColor4f(1, 1, 1, 1);
    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0.0f, 0.0f);   glVertex3f(-1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);   glVertex3f( 1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);   glVertex3f( 1.0f,  1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f);   glVertex3f(-1.0f,  1.0f, 0.0f);
    glEnd();
}

void VideoRendererOpenGL::threadStart()
{
    initGLX();
    initGL();
}

void VideoRendererOpenGL::threadEnd()
{
    glDeleteTextures(1, &mTextureId);

    for (uint32_t i = 0; i < ESPlayerConstants::VIDEO_REN_FRAME_BUFFERS; i++) {
        glDeleteBuffersARB(1, &PBOlist[i].pboId);
    }

    glXMakeCurrent( mXDisplay , None , NULL );
    glXDestroyWindow( mXDisplay , mGLXWindow );
    glXDestroyContext( mXDisplay , mGLXContext );
    XDestroyWindow( mXDisplay , mXWindow );
    XCloseDisplay( mXDisplay );
}

void VideoRendererOpenGL::setVideoWindow(Rect r)
{
    uint32_t dstRectX, dstRectY, dstRectWidth, dstRectHeight;

    if(r.width == 0)
        r.width = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH;
    if(r.height == 0)
        r.height = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT;

    calcDstRectOnScreen(mAttributes.mPixelAspectRatioX,
                        mAttributes.mPixelAspectRatioY,
                        ESPlayerConstants::VIDEO_REN_SCREEN_PAR_X,
                        ESPlayerConstants::VIDEO_REN_SCREEN_PAR_Y,
                        mAttributes.mImageWidth,
                        mAttributes.mImageHeight,
                        r.width,
                        r.height,
                        dstRectX,
                        dstRectY,
                        dstRectWidth,
                        dstRectHeight);

    dstRect_->x = r.x + dstRectX;
    dstRect_->y = r.y + dstRectY;
    dstRect_->width = dstRectWidth;
    dstRect_->height = dstRectHeight;

//    glViewport(dstRectX, dstRectY, dstRectWidth, dstRectHeight);
//    glViewport(0, 0, 1280, 720);
}

