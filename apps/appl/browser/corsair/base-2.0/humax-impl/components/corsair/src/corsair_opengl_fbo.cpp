/** **********************************************************************************************************
	@file 		corsair_opengl_fbo.cpp
	
	@date		2013/11/28
	@author		STB-Component(tstbcomp@humaxdigital.com)
	@breif		
	
	(c) 2011-2014 Humax Co., Ltd. 
	This program is produced by Humax Co., Ltd. ("Humax") and 
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
	non-assignable, non-transferable and non-exclusive license to use this Software. 
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
	you agree to the responsibility to take all reasonable efforts to protect the any information 
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
	reverse engineer or extract the source code of this Software unless you have Humax's written permission 
	to do so.
	If you have no authorized license, discontinue using this Software immediately. 
	
	THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR 
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR 
	INABILITY TO USE THE SOFTWARE.
	
	This License is effective until terminated. You may terminate this License at any time 
	by destroying all copies of the Software including all documentation. This License will terminate 
	immediately without notice from Humax to you if you fail to comply with any provision of this License. 
	Upon termination, you must destroy all copies of the Software and all documentation.  
	
	The laws of the Republic of Korea will apply to any disputes arising out of or relating to 
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*********************************************************************************************************** */

#include "corsair_opengl_fbo.h"
#include "corsair_opengl_debug.h"
#include "corsair_gfx_egl.h"

struct Vertex3 {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};

struct Vertex2 {
	GLfloat x;
	GLfloat y;
};

static const int VERTEX_ARRAY = 0;
static const int TEXCOORD_ARRAY = 1;

/*----------------------------------------------------------------------------------
 * CorsairOpenglFbo
 *---------------------------------------------------------------------------------*/
CorsairOpenglFbo::CorsairOpenglFbo(IDirectFBSurface *dfbSurface, int width, int height)
	: m_dfb_surface(dfbSurface),
	m_egl_surface(0),
	m_width(width),
	m_height(height),
	m_fbo(0),
	m_texture(0),
	m_vertex_shader(0),
	m_fragment_shader(0),
	m_shader_program(0)
{
	m_proj_mat[0] = '\0';
}

CorsairOpenglFbo::~CorsairOpenglFbo()
{
	EGLDisplay gfx_display = CorsairGfxEgl::getEglDisplay();

	eglMakeCurrent(gfx_display, m_egl_surface, m_egl_surface, m_context);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteShader(m_vertex_shader);
	glDeleteShader(m_fragment_shader);
	glDeleteProgram(m_shader_program);
	glDeleteFramebuffers(1, &m_fbo);
	glDeleteTextures(1, &m_texture);
	eglDestroyContext(gfx_display, m_context);
	m_vertex_shader = 0;
	m_fragment_shader = 0;
	m_shader_program = 0;
	m_texture = 0;
	m_context = EGL_NO_CONTEXT;

	eglMakeCurrent(gfx_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(gfx_display, m_egl_surface);
}

bool CorsairOpenglFbo::init()
{
	EGLDisplay gfx_display = CorsairGfxEgl::getEglDisplay();
	EGLConfig gfx_config = CorsairGfxEgl::getEglConfig();
	EGLContext gfx_context = CorsairGfxEgl::getEglContext();

	m_egl_surface = eglCreateWindowSurface(gfx_display, gfx_config, m_dfb_surface, NULL);
	if ( m_egl_surface == EGL_NO_SURFACE ) return false;

	if ( !eglSurfaceAttrib(gfx_display, m_egl_surface, EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED) )
		return false;

	if ( !initScreen() ) return false;

	EGLTEST(eglMakeCurrent(gfx_display, m_egl_surface, m_egl_surface, gfx_context));
}

void CorsairOpenglFbo::draw(int x, int y, int w, int h)
{
	EGLDisplay gfx_display = CorsairGfxEgl::getEglDisplay();

	EGLTEST(eglMakeCurrent(gfx_display, m_egl_surface, m_egl_surface, m_context));

	GLfloat texX = (GLfloat)x/m_width;
	GLfloat texY = (GLfloat)(m_height - y)/m_height;
	GLfloat texWidth = (GLfloat)w/m_width;
	GLfloat texHeight = (GLfloat)h/m_height;

	Vertex3 vertices[4] = {
		{x, y, 0},
		{x+w, y, 0},
		{x, y+h, 0},
		{x+w, y+h, 0}
	};

	Vertex2 texCoords[4] = {
		{texX, texY},
		{texX+texWidth, texY},
		{texX, texY-texHeight},
		{texX+texWidth, texY-texHeight}
	};

	GLuint prevProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&prevProgram);
	glUseProgram(m_shader_program);

	GLuint prevTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&prevTexture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glUniform1i(glGetUniformLocation(m_shader_program, "sampler2d"), 0);
	glEnableVertexAttribArray(VERTEX_ARRAY);
	glVertexAttribPointer(VERTEX_ARRAY, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), vertices);
	glEnableVertexAttribArray(TEXCOORD_ARRAY);
	glVertexAttribPointer(TEXCOORD_ARRAY, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), texCoords);
	GLint location = glGetUniformLocation(m_shader_program, "inProjMat");
	glUniformMatrix4fv(location, 1, GL_FALSE, m_proj_mat);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindTexture(GL_TEXTURE_2D, prevTexture);
	glUseProgram(prevProgram);
}

void CorsairOpenglFbo::swap()
{
	EGLDisplay gfx_display = CorsairGfxEgl::getEglDisplay();
	EGLContext gfx_context = CorsairGfxEgl::getEglContext();

	EGLTEST(eglMakeCurrent(gfx_display, m_egl_surface, m_egl_surface, gfx_context));

	eglSwapBuffers(gfx_display, m_egl_surface);
}

bool CorsairOpenglFbo::initScreen()
{
	EGLDisplay gfx_display = CorsairGfxEgl::getEglDisplay();
	EGLConfig gfx_config = CorsairGfxEgl::getEglConfig();
	EGLContext gfx_context = CorsairGfxEgl::getEglContext();

	const char* vertexShaderSrc =
		"attribute vec4 inVertex;\n"
		"attribute vec4 inTex;\n"
		"uniform mat4 inProjMat;\n"
		"varying vec2 texCoord;\n"
		"void main (void)\n"
		"{\n"
		"gl_Position = inProjMat * inVertex;\n"
		"texCoord = inTex.st;\n"
		"}\n";

	const char* fragmentShaderSrc =
		"#ifdef GL_ES\n"
		"precision mediump float;\n"
		"#endif\n"
		"uniform sampler2D sampler2d;\n"
		"varying vec2 texCoord;\n"
		"void main (void)\n"
		"{\n"
		"gl_FragColor = texture2D(sampler2d,texCoord);\n"
		"}\n";

	EGLint attrs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	m_context = eglCreateContext(gfx_display, gfx_config, gfx_context, attrs);
	if ( m_context == EGL_NO_CONTEXT ) return false;

	EGLTEST(eglMakeCurrent(gfx_display, m_egl_surface, m_egl_surface, m_context));

	m_texture = createTexture(GL_RGBA, m_width, m_height);
	if ( m_texture == EGL_NO_TEXTURE ) return false;

	m_fbo = createFBO(m_texture);

	m_vertex_shader = createAndCompileShader(vertexShaderSrc, GL_VERTEX_SHADER);
	if ( m_vertex_shader == 0 ) return false;

	m_fragment_shader = createAndCompileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER);
	if ( m_fragment_shader == 0 ) return false;

	m_shader_program = glCreateProgram();
	GLTEST(glAttachShader(m_shader_program, m_vertex_shader));
	GLTEST(glAttachShader(m_shader_program, m_fragment_shader));

	GLTEST(glBindAttribLocation(m_shader_program, VERTEX_ARRAY, "inVertex"));
	GLTEST(glBindAttribLocation(m_shader_program, TEXCOORD_ARRAY, "inTex"));

	GLTEST(glLinkProgram(m_shader_program));
	GLint linked = 0;
	glGetProgramiv(m_shader_program, GL_LINK_STATUS, &linked);
	if ( !linked )
	{
		glDeleteShader(m_vertex_shader);
		glDeleteShader(m_fragment_shader);
		glDeleteProgram(m_shader_program);
	}

	loadOrtho();
	glViewport(0, 0, m_width, m_height);

	return true;
}

GLuint CorsairOpenglFbo::createTexture(GLenum internalFormat, int width, int height)
{
	GLuint texture;
	GLTEST(glGenTextures(1, &texture));
	GLTEST(glBindTexture(GL_TEXTURE_2D, texture));
	GLTEST(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLTEST(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLTEST(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLTEST(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLTEST(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));

	return texture;
}

GLuint CorsairOpenglFbo::createFBO(GLuint texture)
{
	GLuint fbo;
	GLTEST(glGenFramebuffers(1, &fbo));
	GLTEST(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	GLTEST(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	return fbo;
}

GLuint CorsairOpenglFbo::createAndCompileShader(const char* src, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	GLTEST(glShaderSource(shader, 1, &src, NULL));
	GLTEST(glCompileShader(shader));

	GLint shaderIsCompiled = 0;
	GLTEST(glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderIsCompiled));
	if ( !shaderIsCompiled )
	{
		int logLength, charsWritten;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		char* infoLog = new char[logLength];
		glGetShaderInfoLog(shader, logLength, &charsWritten, infoLog);
		
		LOG_DEBUG("Failed to compile fragment shader: %s \n",infoLog);

		delete [] infoLog;
		glDeleteShader(shader);

		return 0;
	}

	return shader;
}

void CorsairOpenglFbo::loadOrtho()
{
	memset(m_proj_mat, 0, 16*sizeof(GLfloat));

	m_proj_mat[0] = 2.f/m_width;
	m_proj_mat[5] = -2.f/m_height;
	m_proj_mat[10] = -1;
	m_proj_mat[12] = -1;
	m_proj_mat[13] = 1;
	m_proj_mat[14] = 0;
	m_proj_mat[15] = 1;
}
