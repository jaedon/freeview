
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cassert>
#include <cstring>

#define GL_GLEXT_PROTOTYPES 1
#include <GL/glx.h>
using namespace netflix;


//#define USE_PBO 1

#ifdef USE_PBO
#define GLX_DIRECT_RENDERING    GL_TRUE
#else
#define GLX_DIRECT_RENDERING    GL_FALSE
#endif

#define LOG(s)      (std::cout << s << std::endl)
#define FAIL(e,s)   if (!e) { LOG(s); exit(1); }

static void GL_CHECK(const std::string & s = std::string())
{
    GLenum e = glGetError();
    FAIL(e == GL_NO_ERROR, "GL ERROR 0x" << std::hex << e << " : " << s);
    LOG(s);
}

class Platform
{
public:
    static Platform * initialize(unsigned width, unsigned height);

    virtual ~Platform() {}
    virtual bool hasCurrentContext() = 0;
    virtual void setCurrentContext() = 0;
    virtual void clearCurrentContext() = 0;
    virtual void swap() = 0;
    unsigned width() const { return mWidth; }
    unsigned height() const { return mHeight; }
protected:
    Platform(unsigned width, unsigned height) : mWidth(width), mHeight(height) {}
    unsigned mWidth;
    unsigned mHeight;
};




class PlatformGLX : public Platform
{
public:

    PlatformGLX( unsigned width , unsigned height )
    :
        Platform( width , height )
    {
        XInitThreads();

        LOG("Opening display");

        mXDisplay = XOpenDisplay(0);
        FAIL(mXDisplay, "Failed to open display");
        LOG("Display is good");

        int screen = DefaultScreen( mXDisplay );

        LOG( "GLX client "
            << glXGetClientString( mXDisplay , GLX_VERSION )
            << " : "
            << glXGetClientString( mXDisplay , GLX_VENDOR ));

        LOG( "GLX server "
            << glXQueryServerString( mXDisplay , screen , GLX_VERSION )
            << " : "
            << glXQueryServerString( mXDisplay , screen , GLX_VENDOR ));

        const int attributes[] =
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
        GLXFBConfig * fbc = glXChooseFBConfig( mXDisplay , screen , attributes , &nelements );
        FAIL(fbc, "Return value of glXChooseFBConfig is NULL");
        FAIL(nelements, "glXChooseFBConfig returned 0 elements");
        LOG("Got " << nelements << " FB configurations");

        XVisualInfo * vi = glXGetVisualFromFBConfig( mXDisplay , fbc[0] );
        FAIL(vi, "Got NULL visual from FB config 0");
        LOG("Got visual from FB config");

        Window root = RootWindow( mXDisplay , screen );
        FAIL(root, "Root window is NULL");

        /* Create a colormap */
        Colormap cmap = XCreateColormap( mXDisplay , root , vi->visual , AllocNone );
        LOG("Created color map");

        /* Create a window */
        XSetWindowAttributes swa;
        swa.colormap = cmap;
        swa.border_pixel = 0;
        swa.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask
                | PointerMotionMask | ButtonPressMask | ButtonReleaseMask;

        mXWindow = XCreateWindow( mXDisplay , root , 0, 0, width, height ,0 , vi->depth, InputOutput,
                            vi->visual, CWBorderPixel|CWColormap|CWEventMask, &swa);

        FAIL(mXWindow, "Failed to create X window");
        LOG("Created X window");

        /* Create a GLX context using the first FBConfig in the list. */
        mGLXContext = glXCreateNewContext( mXDisplay , fbc[0] , GLX_RGBA_TYPE , 0 , GLX_DIRECT_RENDERING );
        FAIL(mGLXContext, "Failed to create GLX context");
        LOG("Created GLX context");

        XFree( vi );
        LOG("Freed visual");

        /* Create a GLX window using the same FBConfig that we used for the */
        /* the GLX context.                                                 */
        mGLXWindow = glXCreateWindow( mXDisplay , fbc[0], mXWindow, 0);
        FAIL(mGLXWindow, "Failed to create GLX window");
        LOG("Created GLX window");

        XFree( fbc );
        LOG("Freed FB config");

        Atom wmDeleteMessage = XInternAtom( mXDisplay, "WM_DELETE_WINDOW", False);
        XSetWMProtocols( mXDisplay, mXWindow, &wmDeleteMessage, 1);
        LOG("Set delete window protocol");

        XSetStandardProperties( mXDisplay , mXWindow , "Netflix (GLX)" , "Netflix (GLX)" , None, NULL , 0 , NULL );
        LOG("Set window standard properties");

        XMapWindow( mXDisplay , mXWindow );
        LOG("Mapping window");

        LOG("Waiting for map notification");
        XEvent event;
        XIfEvent( mXDisplay , &event, WaitForNotify, reinterpret_cast< char* >( mXWindow )  );
        LOG("Mapped");

        /* Connect the context to the window for read and write */
        Bool r = glXMakeContextCurrent( mXDisplay, mGLXWindow, mGLXWindow, mGLXContext );
        FAIL(r, "Failed to make context current");
        LOG("Context is current, initialization is done");
    }

    virtual ~PlatformGLX()
    {
        LOG("Destroying platform");
        glXMakeCurrent( mXDisplay , None , NULL );
        LOG("Destroying GLX window");
        glXDestroyWindow( mXDisplay , mGLXWindow );
        LOG("Destroying context");
        glXDestroyContext( mXDisplay , mGLXContext );
        LOG("Destroying X window");
        XDestroyWindow( mXDisplay , mXWindow );
        LOG("Closing display");
        XCloseDisplay( mXDisplay );
        LOG("Done");
    }

    virtual bool hasCurrentContext()
    {
        return mGLXContext == glXGetCurrentContext();
    }

    virtual void setCurrentContext()
    {
        glXMakeContextCurrent( mXDisplay , mGLXWindow , mGLXWindow , mGLXContext );
    }

    virtual void clearCurrentContext()
    {
        glXMakeCurrent( mXDisplay , None , NULL );
    }

    virtual void swap()
    {
        glXSwapBuffers( mXDisplay , mGLXWindow );
    }

private:

    static Bool WaitForNotify( Display * , XEvent *e , char * arg )
    {
       return ( e->type == MapNotify ) && ( e->xmap.window == (Window) arg );
    }

    Display *   mXDisplay;
    Window      mXWindow;
    GLXWindow   mGLXWindow;
    GLXContext  mGLXContext;
};


static GLuint createShader(GLenum type, const char * source)
{
    LOG("Creating " << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << " shader");

    GLuint shader = glCreateShader( type );
    GL_CHECK("Created shader");
    FAIL(shader,"glCreateShader returned 0");
    glShaderSource( shader , 1 , & source , 0 );
    GL_CHECK("Set shader source");
    glCompileShader( shader );
    GL_CHECK("Compiled shader");
    GLint status = GL_TRUE;
    glGetShaderiv( shader , GL_COMPILE_STATUS ,  & status );
    GL_CHECK("Got shader compile status");
    LOG("Compile status is " << status);
    if ( GL_FALSE == status )
    {
        LOG("Failed to compile shader, source follows");
        LOG(source);
        GLint length = 0;
        glGetShaderiv( shader , GL_INFO_LOG_LENGTH , & length );
        GL_CHECK("Got shader info log length");
        if ( length <= 1 )
        {
            LOG("No info log available");
        }
        else
        {
            char * log = new char[ length ];
            glGetShaderInfoLog( shader , length , 0 , log );
            LOG("Info log is : " << log);
            delete [] log;
        }
        FAIL(false,"Failed to compile shader");
    }
    return shader;
}

static GLuint createProgram( const char * vertexShaderSource , const char * fragmentShaderSource )
{
    GLuint vertexShader = createShader( GL_VERTEX_SHADER , vertexShaderSource );
    GLuint fragmentShader = createShader( GL_FRAGMENT_SHADER , fragmentShaderSource );

    GLuint program = glCreateProgram();
    GL_CHECK("Create program");
    glAttachShader( program , vertexShader );
    GL_CHECK("Attach vertex shader");
    glAttachShader( program , fragmentShader );
    GL_CHECK("Attach fragment shader");
    glLinkProgram( program );
    GL_CHECK("Link program");

    GLint status = GL_TRUE;
    glGetProgramiv( program , GL_LINK_STATUS , & status );
    GL_CHECK("Got program link status");
    LOG("Program link status is " << status);
    if ( GL_FALSE == status )
    {
        LOG("Failed to link program");
        GLint length = 0;
        glGetProgramiv( program , GL_INFO_LOG_LENGTH , & length );
        GL_CHECK("Get program info log");
        if ( length <= 1 )
        {
            LOG("No info log available");
        }
        else
        {
            char * log = new char[ length ];
            glGetProgramInfoLog( program , length , 0 , log );
            LOG("Info log is : " << log);
            delete [] log;
        }
        FAIL(false, "Failed to link program");
    }
    return program;
}


int main(int argc, char * argv[] )
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    Platform * platform = new PlatformGLX(screenWidth, screenHeight);

    FAIL(platform->hasCurrentContext(), "Platform does not have current context");
    LOG("Platform current context is good");

    GL_CHECK("After initialization");

    LOG("GL vendor " << glGetString(GL_VENDOR));
    LOG("GL renderer " << glGetString(GL_RENDERER));
    LOG("GL version " << glGetString(GL_VERSION));
    LOG("GLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION));

    GLint maxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    LOG("Max texture size " << maxTextureSize);

    //.........................................................................

    GLuint  mVertexBuffer;
    GLuint  mIndexBuffer;
    GLuint  mTextureCoordinatesBuffer;

    glBlendFunc( GL_ONE , GL_ONE_MINUS_SRC_ALPHA );
    glDisable( GL_BLEND );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glViewport( 0 , 0 , screenWidth , screenHeight );
    glClearColor( 0 , 0 , 0 , 0 );
    glActiveTexture( GL_TEXTURE0 );
    glDisable( GL_DEPTH_TEST );

    GL_CHECK("Initial GL state");

    //.........................................................................
    // Create the vertex and index buffers for a rectangle

    glGenBuffers( 1 , & mVertexBuffer );
    glGenBuffers( 1 , & mIndexBuffer );
    glGenBuffers( 1 , & mTextureCoordinatesBuffer );


    GLfloat vertices[12] =
    {
        -1.0,  1.0, 0.0,
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0,
        1.0,  1.0, 0.0,
    };

    glBindBuffer( GL_ARRAY_BUFFER , mVertexBuffer );
    glBufferData( GL_ARRAY_BUFFER , sizeof( vertices ) , vertices , GL_STATIC_DRAW );

    GL_CHECK("Set up vertex buffer");

    GLubyte textureCoordinates[8] =
    {
            0, 0,
            0, 1,
            1, 1,
            1, 0
    };

    glBindBuffer( GL_ARRAY_BUFFER , mTextureCoordinatesBuffer );
    glBufferData( GL_ARRAY_BUFFER , sizeof( textureCoordinates ) , textureCoordinates , GL_STATIC_DRAW );

    GL_CHECK("Set up default texture coordinate buffer");

    glBindBuffer( GL_ARRAY_BUFFER , 0 );

    GLubyte indices[6] = {0, 1, 2, 0, 2, 3};

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER , mIndexBuffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER , sizeof( indices ) , indices , GL_STATIC_DRAW );

    GL_CHECK("Set up index buffer");

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER , 0 );

    GLfloat identity[4][4];
    memset(identity, 0x0, sizeof(identity));
    identity[0][0] = 1.0f;
    identity[1][1] = 1.0f;
    identity[2][2] = 1.0f;
    identity[3][3] = 1.0f;

    const char * svertex  =
            "uniform mat4   u_projectionMatrix;\n"
            "uniform mat4   u_mvMatrix;\n"
            "uniform vec4   u_ts;\n"
            "attribute vec4 a_position;\n"
            "attribute vec2 a_texCoord;\n"
            "varying vec2   v_texCoord;\n"
            "void main()\n"
            "{\n"
            "  gl_Position = ( u_mvMatrix * u_projectionMatrix ) * a_position;\n"
            "  v_texCoord.x = ( a_texCoord.x * u_ts.z ) + u_ts.x;\n"
            "  v_texCoord.y = ( a_texCoord.y * u_ts.w ) + u_ts.y;\n"
            "}\n";


    const char * sfragment =
                "#ifdef GL_ES\n"
                "precision highp float;\n"
                "#endif\n"
                "varying vec2 v_texCoord;\n"
                "uniform float  u_opacity;\n"
                "uniform sampler2D s_texture;\n"
                "void main()\n"
                "{\n"
                "  gl_FragColor = texture2D( s_texture, v_texCoord ) * u_opacity;\n"
                "}\n";

    LOG( "Creating surface program" );
    GLuint mSurfaceProgram = createProgram( svertex , sfragment );

    LOG( "Getting attribute and uniform locations" );
    GLint mSurfacePositionAttributeLocation = glGetAttribLocation( mSurfaceProgram , "a_position" );
    GLint mSurfaceTextureCoordinatesAttributeLocation = glGetAttribLocation( mSurfaceProgram , "a_texCoord" );
    GLint mSurfaceMatrixUniformLocation = glGetUniformLocation( mSurfaceProgram , "u_mvMatrix" );
    GLint mSurfaceTextureTransformUniformLocation = glGetUniformLocation( mSurfaceProgram , "u_ts" );
    GLint mSurfaceOpacityUniformLocation = glGetUniformLocation( mSurfaceProgram , "u_opacity" );

    glUseProgram( mSurfaceProgram );
    GL_CHECK( "Use surface program" );

    glUniformMatrix4fv( glGetUniformLocation( mSurfaceProgram , "u_projectionMatrix" ) , 1 , GL_FALSE, & identity[0][0] );
    glUniform1i( glGetUniformLocation( mSurfaceProgram , "s_texture" ) , 0 );
    GL_CHECK( "Set surface uniforms" );

    glUseProgram( 0 );
    GL_CHECK( "Use no program" );

    //LOG("\t\t\tIF WE GOT HERE, WE'RE IN GOOD SHAPE");

    // A 2x2 red, greeN, blue, white RGB image
    const GLubyte pixels[] = {
        0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00,
        0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF
    };

    const size_t pixelBufferSize = sizeof(pixels) * sizeof(GLubyte);

#ifdef USE_PBO

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    GL_CHECK("Bind no PBO");

    GLuint pbo;
    glGenBuffers(1, &pbo);
    LOG("PBO IS " << pbo);
    GL_CHECK("Gen PBO");

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    GL_CHECK("Bind PBO");

    glBufferData(GL_PIXEL_UNPACK_BUFFER, pixelBufferSize, 0, GL_DYNAMIC_DRAW);
    GL_CHECK("Reset PBO data");

    void * pboMemory = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    GL_CHECK("Map PBO");
    FAIL(pboMemory, "Failed to map buffer memory");
    memcpy(pboMemory, pixels, pixelBufferSize);

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    GL_CHECK("Unmap PBO");

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    GL_CHECK("Bind no PBO");

#endif

    GLuint texture;
    glGenTextures(1, &texture);
    GL_CHECK("Gen texture");

    glBindTexture(GL_TEXTURE_2D, texture);
    GL_CHECK("Bind texture");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#ifdef USE_PBO
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    GL_CHECK("Bind PBO");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    GL_CHECK("Upload pixels");

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    GL_CHECK("Bind no PBO");
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    GL_CHECK("Upload pixels directly");
#endif

    glBindTexture(GL_TEXTURE_2D, 0);
    GL_CHECK("Bind no texture");

    // Now set-up for rendering

    glClearColor(0xFF, 0, 0, 0xFF);
    glUseProgram(mSurfaceProgram);

    glUniformMatrix4fv(mSurfaceMatrixUniformLocation, 1, GL_FALSE, & identity[0][0]);
    glUniform1f(mSurfaceOpacityUniformLocation, 1);
    const GLfloat textureTransform[4] = { 0 , 0 , 1 , 1 };
    glUniform4fv(mSurfaceTextureTransformUniformLocation, 1, textureTransform);


    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glVertexAttribPointer(mSurfacePositionAttributeLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(mSurfacePositionAttributeLocation);

    glBindBuffer(GL_ARRAY_BUFFER, mTextureCoordinatesBuffer);
    glVertexAttribPointer(mSurfaceTextureCoordinatesAttributeLocation, 2, GL_UNSIGNED_BYTE, GL_FALSE, 2 * sizeof(GLubyte), 0);
    glEnableVertexAttribArray(mSurfaceTextureCoordinatesAttributeLocation);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, texture);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);

    while (1) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

        platform->swap();
    }

    delete platform;

    return 0;
}
