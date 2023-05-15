
#define REMOTE_API_LOGGING
#include "interface/khronos/common/khrn_client_mangle.h" 
#include "interface/khronos/common/khrn_api_interposer.h" 
#include "interface/khronos/common/khrn_api_command_ids.h" 

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <sys/types.h>

#include <platform.h>
#include <map>

#include "packet.h"
#include "remote.h"
#include "control.h"
#include "archive.h"

#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __attribute__((visibility("default")))
#endif

// The version of the remote API (Driver<->SpyHook)
// Update this when the interface between the driver & SpyHook changes
#define REMOTE_API_MAJOR_VER 1
#define REMOTE_API_MINOR_VER 7

// The version of the SpyHook API (SpyHook<->SpyTool)
// Update this when the interface between the SpyHook & SpyTool changes
#define SPYHOOK_MAJOR_VER 1
#define SPYHOOK_MINOR_VER 9

// The version of the binary capture format
// Update this when the capture data changes
#define CAPTURE_MAJOR_VER 1
#define CAPTURE_MINOR_VER 0

// Set the XML buffer size to a reasonable amount
#define XML_BUFSIZE     ( 5 * 1024 )

#define FOURCC(a,b,c,d)		(((a) << 24) | ((b) << 16) | ((c) << 8) | (d))

#define DLLEXPORTENTRY

struct DriverEvent
{
   uint32_t secs;
   uint32_t ns;
   uint32_t eventType;
   uint32_t eventCode;
   uint32_t eventRow;
   uint32_t eventData;
   std::string desc;
};

typedef std::vector<DriverEvent> EventBlock;

static EventBlock       sEventBlock;
static MutexHandle      sEventMutex;

typedef void (EGLAPIENTRYP PFNEGLINITDRIVERMONITORBRCMPROC)(EGLDisplay display, EGLint hw_bank, EGLint l3c_bank);
typedef void (EGLAPIENTRYP PFNEGLGETDRIVERMONITORXMLBRCMPROC)(EGLDisplay display, EGLint bufSize, EGLint *length, char *xmlStats);
typedef void (EGLAPIENTRYP PFNEGLTERMDRIVERMONITORBRCMPROC)(EGLDisplay display);

static PFNEGLINITDRIVERMONITORBRCMPROC    sEglInitDriverMonitorBRCM   = 0;
static PFNEGLGETDRIVERMONITORXMLBRCMPROC  sEglGetDriverMonitorXMLBRCM = 0;
static PFNEGLTERMDRIVERMONITORBRCMPROC    sEglTermDriverMonitorBRCM   = 0;

static Packet *sCurPacket = NULL;
static bool    sEnabled = true;
static bool    sOrphaned = false;
static bool    sSendNextRet = true;
static uint32_t sBottleneckMode = Control::eUnset;
static bool    sCaptureEvents = false;

static bool    sCaptureStream = false;
static Archive *sCaptureArchive = NULL;

static std::map< uint32_t, std::vector<uint32_t> > varset;

static REAL_GL_API_TABLE sRealFuncs;

#define Real(f) sRealFuncs.real_##f

#ifdef ANDROID
#include <cutils/log.h>
#define printf LOGD
#endif


class Buffer
{
public:
   Buffer() : m_maxByteIndx(0), m_maxShortIndx(0) {}
   GLuint m_maxByteIndx;
   GLuint m_maxShortIndx;
};

class Context
{
public:
   std::map<GLuint, Buffer>   m_buffers;
};

std::map<GLuint, Context>  sContexts;

static void Packetize()
{
}

template <typename T1>
void Packetize(T1 u)
{
   sCurPacket->AddItem(u);
}

template <typename T1, typename T2>
void Packetize(T1 t1, T2 t2)
{
   sCurPacket->AddItem(t1);
   sCurPacket->AddItem(t2);
}

template <typename T1, typename T2, typename T3>
void Packetize(T1 t1, T2 t2, T3 t3)
{
   sCurPacket->AddItem(t1);
   sCurPacket->AddItem(t2);
   sCurPacket->AddItem(t3);
}

template <typename T1, typename T2, typename T3, typename T4>
void Packetize(T1 t1, T2 t2, T3 t3, T4 t4)
{
   sCurPacket->AddItem(t1);
   sCurPacket->AddItem(t2);
   sCurPacket->AddItem(t3);
   sCurPacket->AddItem(t4);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
void Packetize(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
{
   sCurPacket->AddItem(t1);
   sCurPacket->AddItem(t2);
   sCurPacket->AddItem(t3);
   sCurPacket->AddItem(t4);
   sCurPacket->AddItem(t5);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
void Packetize(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
{
   sCurPacket->AddItem(t1);
   sCurPacket->AddItem(t2);
   sCurPacket->AddItem(t3);
   sCurPacket->AddItem(t4);
   sCurPacket->AddItem(t5);
   sCurPacket->AddItem(t6);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
void Packetize(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7)
{
   sCurPacket->AddItem(t1);
   sCurPacket->AddItem(t2);
   sCurPacket->AddItem(t3);
   sCurPacket->AddItem(t4);
   sCurPacket->AddItem(t5);
   sCurPacket->AddItem(t6);
   sCurPacket->AddItem(t7);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
void Packetize(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8)
{
   sCurPacket->AddItem(t1);
   sCurPacket->AddItem(t2);
   sCurPacket->AddItem(t3);
   sCurPacket->AddItem(t4);
   sCurPacket->AddItem(t5);
   sCurPacket->AddItem(t6);
   sCurPacket->AddItem(t7);
   sCurPacket->AddItem(t8);
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
void Packetize(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9)
{
   sCurPacket->AddItem(t1);
   sCurPacket->AddItem(t2);
   sCurPacket->AddItem(t3);
   sCurPacket->AddItem(t4);
   sCurPacket->AddItem(t5);
   sCurPacket->AddItem(t6);
   sCurPacket->AddItem(t7);
   sCurPacket->AddItem(t8);
   sCurPacket->AddItem(t9);
}

static uint32_t TextureSize(uint32_t w, uint32_t h, uint32_t format, uint32_t type, uint32_t unpackAlignment)
{
   uint32_t bitsPerChannel = 0;
   uint32_t channels = 0;
   uint32_t bitsPerPixel = 0;
   uint32_t bytesPerRow = 0;
   uint32_t upaMinus1 = unpackAlignment - 1;

   switch (format)
   {
   case GL_LUMINANCE :        channels = 1; break;
   case GL_ALPHA :            channels = 1; break;
   case GL_LUMINANCE_ALPHA :  channels = 2; break;
   case GL_RGB :              channels = 3; break;
   case GL_RGBA :             channels = 4; break;
   }

   switch (type)
   {
   case GL_UNSIGNED_BYTE            : bitsPerChannel = 8; break;
   case GL_UNSIGNED_SHORT_5_6_5     : bitsPerPixel = 16; break;
   case GL_UNSIGNED_SHORT_4_4_4_4   : bitsPerPixel = 16; break;
   case GL_UNSIGNED_SHORT_5_5_5_1   : bitsPerPixel = 16; break;
   }

   if (bitsPerPixel == 0)
      bitsPerPixel = bitsPerChannel * channels;

   // Round to a multiple of 8 bits per pixel
   bytesPerRow = (bitsPerPixel * w + 7) / 8;

   // Round to unpackAlignment b
   bytesPerRow = (bytesPerRow + upaMinus1) & (~upaMinus1);

   return bytesPerRow * h;
}

static uint32_t UnpackAlignment()
{
   GLint v[4];
   Real(glGetIntegerv)(GL_UNPACK_ALIGNMENT, v);
   return v[0];
}

static Remote *remote = NULL;

/******************************************************************************/
/**
   Search the named set for a given value.
   
   This search returns true under these conditions:
   i.  The set is empty (empty set matches all).
   ii. The value is in the set.
   
   The empty set is a special case.  It means that no selection is to be made,
   so match everything.
**/
DLLEXPORT bool DLLEXPORTENTRY remote_inVarSet(uint32_t fourcc, uint32_t val)
{
   std::vector<uint32_t> set = varset[fourcc];
   std::vector<uint32_t>::iterator it;
   
   if ( set.size() == 0 )
      return true;
         
   for ( it = set.begin(); it < set.end(); it++ )
      if ( *it == val )
         return true;
 
   return false;
}
static bool PostPacketize(Packet *p);

static void GetGLBuffer(const Packet &packet)
{
   GLint v[4];

   v[0] = v[1] = 0;

   uint32_t fb = packet.Item(1).GetUInt32();

   EGLContext curContext = Real(eglGetCurrentContext)();
   EGLContext context = EGL_NO_CONTEXT;
   EGLSurface oDrawSurf = Real(eglGetCurrentSurface)(EGL_DRAW);
   EGLSurface oReadSurf = Real(eglGetCurrentSurface)(EGL_READ);
   EGLDisplay disp = Real(eglGetDisplay)(EGL_DEFAULT_DISPLAY);
   GLint      curFB = 0;
   EGLSurface drawSurf = oDrawSurf;

   Real(glGetIntegerv)(GL_FRAMEBUFFER_BINDING, &curFB);

   if (packet.NumItems() > 2)
   {
      context = (EGLContext)packet.Item(2).GetUInt32();
      v[2] = packet.Item(3).GetUInt32();
      v[3] = packet.Item(4).GetUInt32();
      EGLSurface drawSurf = (EGLSurface)packet.Item(5).GetUInt32();

      // Change context
      Real(eglMakeCurrent)(disp, drawSurf, drawSurf, context);

      // And change framebuffer
      if (fb != curFB)
         Real(glBindFramebuffer)(GL_FRAMEBUFFER, fb);
   }

   if (fb == 0)   // Main framebuffer
   {
      EGLDisplay disp = Real(eglGetDisplay)(EGL_DEFAULT_DISPLAY);
      Real(eglQuerySurface)(disp, drawSurf, EGL_WIDTH, &v[2]);
      Real(eglQuerySurface)(disp, drawSurf, EGL_HEIGHT, &v[3]);
   }

   uint8_t *buf = NULL;
   if (v[2] != 0 && v[3] != 0)
   {
      buf = new uint8_t[v[2] * v[3] * 4];
   Real(glReadPixels)(v[0], v[1], v[2], v[3], GL_RGBA, GL_UNSIGNED_BYTE, buf);
   }

   Packet dataPacket(eBUFFER);
   dataPacket.AddItem(packet.Item(1)); // id
   dataPacket.AddItem(PacketItem(v[0]));
   dataPacket.AddItem(PacketItem(v[1]));
   dataPacket.AddItem(PacketItem(v[2]));
   dataPacket.AddItem(PacketItem(v[3]));
   dataPacket.AddItem(PacketItem(buf, v[2] * v[3] * 4));

   // Restore changed state
   if (packet.NumItems() > 2)
   {
      Real(eglMakeCurrent)(disp, oDrawSurf, oReadSurf, curContext);

      if (fb != curFB)
         Real(glBindFramebuffer)(GL_FRAMEBUFFER, curFB);
   }

   if (remote)
      dataPacket.Send(remote);

   delete [] buf;
}

static void GetPerfData(const Packet &packet)
{
   EGLDisplay disp = Real(eglGetDisplay)(EGL_DEFAULT_DISPLAY);

   Real(eglInitialize)( disp, NULL, NULL );

   Control::ePerfAction action = (Control::ePerfAction)packet.Item(1).GetUInt32();

   if (action == Control::ePerfStart)
   {
      Packet dataPacket(ePERFDATASET);
      sEglInitDriverMonitorBRCM(disp, packet.Item(2).GetUInt32(), packet.Item(3).GetUInt32());
      if (remote)
         dataPacket.Send(remote);
   }
   else if (action == Control::ePerfStop)
   {
      Packet dataPacket(ePERFDATASET);
      sEglTermDriverMonitorBRCM(disp);
      if (remote)
         dataPacket.Send(remote);
   }
   else if (action == Control::ePerfGet)
   {
      Packet dataPacket(ePERF_DATA);

      char xml[XML_BUFSIZE];

      sEglGetDriverMonitorXMLBRCM(disp, sizeof(xml), NULL, (char*)&xml);

      dataPacket.AddItem(PacketItem(&xml, strlen(xml) + 1));
      dataPacket.AddItem(PacketItem(plGetTimeNowMs()));

      // Restart the counters
      sEglTermDriverMonitorBRCM(disp);
      sEglInitDriverMonitorBRCM(disp, packet.Item(2).GetUInt32(), packet.Item(3).GetUInt32());

      if (remote)
         dataPacket.Send(remote);
   }
   else if (action == Control::ePerfNames)
   {
      Packet dataPacket(ePERF_DATA_NAMES);

      char xml[XML_BUFSIZE];

      sEglTermDriverMonitorBRCM(disp);
      sEglInitDriverMonitorBRCM(disp, packet.Item(2).GetUInt32(), packet.Item(3).GetUInt32());
      sEglGetDriverMonitorXMLBRCM(disp, sizeof(xml), NULL, (char*)&xml);
      sEglTermDriverMonitorBRCM(disp);

      dataPacket.AddItem(PacketItem(packet.Item(2).GetUInt32()));
      dataPacket.AddItem(PacketItem(packet.Item(3).GetUInt32()));
      dataPacket.AddItem(PacketItem(&xml, strlen(xml) + 1));

      if (remote)
         dataPacket.Send(remote);
   }
}

static void GetEventData(const Packet &packet)
{
   Packet            p(eEVENT_DATA);

   if (sCaptureEvents)
   {
      uint32_t s, ns;

      plLockMutex(&sEventMutex);

   uint32_t num = sEventBlock.size();

   p.AddItem(PacketItem(1));                    // Version 1 of event block data

      plGetAccurateTime(&s, &ns);

      p.AddItem(s);                                // Actual time now
      p.AddItem(ns);

   p.AddItem(PacketItem(num));                  // Number of events

   for (uint32_t i = 0; i < num; i++)
   {
      p.AddItem(PacketItem(sEventBlock[i].secs));
      p.AddItem(PacketItem(sEventBlock[i].ns));
      p.AddItem(PacketItem(sEventBlock[i].eventType));
      p.AddItem(PacketItem(sEventBlock[i].eventCode));
      p.AddItem(PacketItem(sEventBlock[i].eventRow));
      p.AddItem(PacketItem(sEventBlock[i].eventData));
      p.AddItem(PacketItem(sEventBlock[i].desc.c_str()));
   }

   sEventBlock.clear();

      plUnlockMutex(&sEventMutex);
   }
   else
   {
      p.AddItem(PacketItem(1));                  // Version 1 of event block data
      p.AddItem(0);
      p.AddItem(0);
      p.AddItem(PacketItem(0));                  // Number of events

      plLockMutex(&sEventMutex);
      sEventBlock.clear();
      plUnlockMutex(&sEventMutex);
   }

   if (remote)
      p.Send(remote);
}

static void GetUniforms(const Packet &packet)
{
   uint32_t progId = packet.Item(1).GetUInt32();

   Packet dataPacket(eUNIFORMS);
   dataPacket.AddItem(PacketItem(progId));
   dataPacket.AddItem(PacketItem((GLuint)Real(glIsProgram(progId))));

   GLint activeUniforms, linkStatus, maxLen, infoLen;
   char **nameBufs = 0;
   char  *logBuf = 0;
   char  *shLogBuf = 0;

   if (Real(glIsProgram)(progId))
   {
      Real(glGetProgramiv)(progId, GL_LINK_STATUS, &linkStatus);
      Real(glGetProgramiv)(progId, GL_ACTIVE_UNIFORMS, &activeUniforms);
      Real(glGetProgramiv)(progId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);

      dataPacket.AddItem(PacketItem(linkStatus));
      dataPacket.AddItem(PacketItem(activeUniforms));

      Real(glGetProgramiv)(progId, GL_INFO_LOG_LENGTH, &infoLen);
      dataPacket.AddItem(PacketItem(infoLen));
      if (infoLen > 0)
      {
         logBuf = new char[infoLen];
         Real(glGetProgramInfoLog)(progId, infoLen, NULL, logBuf);
         dataPacket.AddItem(PacketItem(logBuf, infoLen));
      }

      if (activeUniforms > 0)
         nameBufs = new char*[activeUniforms];

      for (GLint u = 0; u < activeUniforms; u++)
      {
         GLint  size;
         GLenum type;
         uint32_t c;

         nameBufs[u] = new char[maxLen];
         Real(glGetActiveUniform)(progId, u, maxLen, NULL, &size, &type, nameBufs[u]);
         GLint location = Real(glGetUniformLocation)(progId, nameBufs[u]);

         dataPacket.AddItem(PacketItem(nameBufs[u], strlen(nameBufs[u])));
         dataPacket.AddItem(PacketItem(location));
         dataPacket.AddItem(PacketItem(type));
         dataPacket.AddItem(PacketItem(size));

         GLfloat f[16];
         GLint   i[4];

         switch (type)
         {
         case GL_FLOAT:
            Real(glGetUniformfv)(progId, location, f);
            dataPacket.AddItem(PacketItem(f[0]));
            break;
         case GL_FLOAT_VEC2:
            Real(glGetUniformfv)(progId, location, f);
            for (c = 0; c < 2; c++)
               dataPacket.AddItem(PacketItem(f[c]));
            break;
         case GL_FLOAT_VEC3:
            Real(glGetUniformfv)(progId, location, f);
            for (c = 0; c < 3; c++)
               dataPacket.AddItem(PacketItem(f[c]));
            break;
         case GL_FLOAT_VEC4:
            Real(glGetUniformfv)(progId, location, f);
            for (c = 0; c < 4; c++)
               dataPacket.AddItem(PacketItem(f[c]));
            break;
         case GL_INT:
         case GL_BOOL:
            Real(glGetUniformiv)(progId, location, i);
            dataPacket.AddItem(PacketItem(i[0]));
            break;
         case GL_INT_VEC2:
         case GL_BOOL_VEC2:
            Real(glGetUniformiv)(progId, location, i);
            for (c = 0; c < 2; c++)
               dataPacket.AddItem(PacketItem(i[c]));
            break;
         case GL_INT_VEC3:
         case GL_BOOL_VEC3:
            Real(glGetUniformiv)(progId, location, i);
            for (c = 0; c < 3; c++)
               dataPacket.AddItem(PacketItem(i[c]));
            break;
         case GL_INT_VEC4:
         case GL_BOOL_VEC4:
            Real(glGetUniformiv)(progId, location, i);
            for (c = 0; c < 4; c++)
               dataPacket.AddItem(PacketItem(i[c]));
            break;
         case GL_FLOAT_MAT2:
            Real(glGetUniformfv)(progId, location, f);
            for (c = 0; c < 4; c++)
               dataPacket.AddItem(PacketItem(f[c]));
            break;
         case GL_FLOAT_MAT3:
            Real(glGetUniformfv)(progId, location, f);
            for (c = 0; c < 9; c++)
               dataPacket.AddItem(PacketItem(f[c]));
            break;
         case GL_FLOAT_MAT4:
            Real(glGetUniformfv)(progId, location, f);
            for (c = 0; c < 16; c++)
               dataPacket.AddItem(PacketItem(f[c]));
            break;
         case GL_SAMPLER_2D:
         case GL_SAMPLER_CUBE:
            Real(glGetUniformiv)(progId, location, i);
            dataPacket.AddItem(PacketItem(i[0]));
            break;
         }
      }
   }

   if (remote)
      dataPacket.Send(remote);

   if (nameBufs)
   {
      for (GLint u = 0; u < activeUniforms; u++)
         delete [] nameBufs[u];

      delete [] nameBufs;
   }

   if (logBuf)
      delete [] logBuf;

   if (shLogBuf)
      delete [] shLogBuf;
}

static void GetState(const Packet &packet)
{
   uint32_t numStates = packet.Item(1).GetUInt32();

   Packet dataPacket(eSTATE);
   dataPacket.AddItem(PacketItem(numStates));

   GLint     iv[16];
   GLfloat   fv[16];
   GLboolean bv[16];
   uint32_t  j, k = 2;

   for (uint32_t i = 0; i < numStates; i++)
   {
      uint32_t type = packet.Item(k++).GetUInt32();
      uint32_t num  = packet.Item(k++).GetUInt32();
      uint32_t enm  = packet.Item(k++).GetUInt32();

      dataPacket.AddItem(PacketItem(type));
      dataPacket.AddItem(PacketItem(num));
      dataPacket.AddItem(PacketItem(enm));

      switch (type)
      {
      case 0:  memset(iv, 0, num * sizeof(GLint));
               Real(glGetIntegerv(enm, iv)); 
               for (j = 0; j < num; j++)
                  dataPacket.AddItem(PacketItem(iv[j]));
               break;
      case 1:  memset(fv, 0, num * sizeof(GLfloat));
               Real(glGetFloatv(enm, fv)); 
               for (j = 0; j < num; j++)
                  dataPacket.AddItem(PacketItem(fv[j]));
               break;
      case 2:  memset(bv, 0, num * sizeof(GLboolean));
               Real(glGetBooleanv(enm, bv)); 
               for (j = 0; j < num; j++)
                  dataPacket.AddItem(PacketItem((GLuint)bv[j]));
               break;
      case 3:  dataPacket.AddItem(PacketItem((GLuint)Real(glIsEnabled(enm)))); 
               break;
      }
   }
   
   if (remote)
      dataPacket.Send(remote);
}

static void GetMemory(const Packet &packet)
{
   void     *addr = (void*)packet.Item(1).GetUInt32();
   uint32_t numBytes = packet.Item(2).GetUInt32();

   if (numBytes == 0)
   {
      // We actually want a NULL terminated array at this pointer, so find out how big this will be
      uint8_t *ptr = (uint8_t*)addr;
      while (*ptr != 0)
      {
         ptr++;
         numBytes++;
      }
      numBytes++; // Include the NULL
   }
   else if (numBytes == 0xFFFFFFFF)
   {
      // We actually want a NULL terminated attrib array at this pointer (Odd word NULL)
      uint32_t *ptr = (uint32_t*)addr;
      bool     odd = true;
      while (!odd || *ptr != 0)
      {
         ptr++;
         numBytes += 4;
         odd = !odd;
      }
      numBytes += 4; // Include the NULL
   }

   Packet dataPacket(eMEMORY);
   dataPacket.AddItem(PacketItem((uint32_t)addr));
   dataPacket.AddItem(PacketItem((uint32_t)numBytes));
   dataPacket.AddItem(PacketItem(addr, numBytes));

   if (remote)
      dataPacket.Send(remote);
}

static void SetCapture(bool tf)
{
   sCaptureStream = tf;

   if (sCaptureStream && sCaptureArchive == NULL)
   {
      uint32_t maj = CAPTURE_MAJOR_VER;
      uint32_t min = CAPTURE_MINOR_VER;
      uint32_t ident = 0xBCCA97DA;
      char     file[256];
      sprintf(file, "bcmcapture_%d.mon", getpid());

      printf("********** CAPTURE %d ********* \n", getpid());
      sCaptureArchive = new Archive(file);
      sCaptureArchive->Connect();
      sCaptureArchive->Send((uint8_t*)&ident, sizeof(uint32_t), false);
      sCaptureArchive->Send((uint8_t*)&maj, sizeof(uint32_t), false);
      sCaptureArchive->Send((uint8_t*)&min, sizeof(uint32_t), false);
   }
   else if (!sCaptureStream)
   {
      // Capture stopped
      if (sCaptureArchive)
         sCaptureArchive->Flush();
   }
}

static void ChangePort(const Packet &packet)
{
   uint32_t port = packet.Item(1).GetUInt32();
   bool capture = false;

   if (packet.NumItems() > 2)
      capture = packet.Item(2).GetBoolean();

   delete remote;
   remote = new Remote(port);
   if (remote->Connect())
   {
      printf("GPU Monitor initialized on port %d\n", port);

      SetCapture(capture);
   }
   else
      sOrphaned = true;
}

static void SetBottleneckMode(const Packet &packet)
{
   sBottleneckMode = packet.Item(1).GetUInt32();

   Packet dataPacket(eBOTTLENECKSET);
   dataPacket.AddItem(PacketItem(sBottleneckMode));

   if (remote)
      dataPacket.Send(remote);
}

static void SetVarSet(const Packet &packet)
{
   uint32_t num_items = packet.NumItems();
   uint32_t key       = (uint32_t)packet.Item(1).GetUInt32();
   uint32_t i;
   
   varset[key].clear();
   for ( i = 2; i < num_items; i++ )
      varset[key].push_back( (uint32_t)packet.Item(i).GetUInt32() );
      
   Packet dataPacket(eSETVARSET);
   dataPacket.AddItem(PacketItem(key));
   
   if (remote)
      dataPacket.Send(remote);

   // Cache certain variable results for performance reasons
   if (key == FOURCC('E','V','T','D'))
      sCaptureEvents = !remote_inVarSet(FOURCC('E','V','T','D'), 1);

   if (key == FOURCC('C','A','P','T'))
      SetCapture(remote_inVarSet(FOURCC('C','A','P','T'), 1));
}

static void ChangeShader(const Packet &packet)
{
   uint32_t context = packet.Item(1).GetUInt32();
   uint32_t progID = packet.Item(2).GetUInt32();
   uint32_t shaderID = packet.Item(3).GetUInt32();
   const char *shaderSrc = packet.Item(4).GetCharPtr();

   bool changedOk = false;
   char linkError[512];
   linkError[0] = '\0';

   if (context != 0 && progID != 0 && shaderID != 0 && shaderSrc != 0)
   {
      EGLContext curCtx = Real(eglGetCurrentContext()); 
      GLboolean  isShader = Real(glIsShader(shaderID));
      GLboolean  isProgram = Real(glIsProgram(progID));

      if (context == (uint32_t)curCtx && isShader && isProgram)
      {
         GLint linkedOK = 0;
         Real(glShaderSource(shaderID, 1, &shaderSrc, NULL));
         Real(glCompileShader(shaderID));
         Real(glLinkProgram(progID));
         Real(glGetProgramiv(progID, GL_LINK_STATUS, &linkedOK));
         if (linkedOK)
         {
            changedOk = true;
         }
         else
         {
            Real(glGetProgramInfoLog(progID, 512, NULL, linkError));
            linkError[511] = '\0';
         }
      }
   }

   Packet dataPacket(eSHADER_CHANGE_ACK);
   dataPacket.AddItem(PacketItem(changedOk));
   dataPacket.AddItem(PacketItem(linkError));

   if (remote)
      dataPacket.Send(remote);
}

#if !defined(ANDROID) && !defined(WIN32) 
static void BackTrace(const Packet &packet)
{
   Packet dataPacket(eBACKTRACE);

   void *buffer[512];
   char *result[512];

   int n = backtrace(buffer, 512);

   char **dump = backtrace_symbols(buffer, n);

   dataPacket.AddItem(PacketItem(n));

   for (uint32_t i = 0; i < n; ++i)
   {
      int status;
      char *demangled = NULL;
      char *symbol = NULL;

      char *first = strchr(dump[i], '(');
      if (first)
      {
         symbol = strdup(first + 1);
         char *plus = strchr(symbol, '+');
         if (plus)
            *plus = '\0';
         else
         {
            free(symbol);
            symbol = NULL;
         }
      }

      if (symbol)
         demangled = __cxxabiv1::__cxa_demangle(symbol, NULL, 0, &status);

      if (demangled)
         result[i] = strdup(demangled);
      else if (symbol)
         result[i] = strdup(symbol);
      else
         result[i] = strdup("??");

      if (symbol)
         free(symbol);

      if (demangled)
         free(demangled);

      dataPacket.AddItem(PacketItem(dump[i]));
      dataPacket.AddItem(PacketItem(result[i]));
   }

   if (remote)
      dataPacket.Send(remote);

   free(dump);

   for (uint32_t i = 0; i < n; ++i)
      free(result[i]);
}
#else
static void BackTrace(const Packet &packet)
{
   /* send an empty backtrace */
   Packet dataPacket(eBACKTRACE);
   if (remote)
      dataPacket.Send(remote);
}
#endif

static bool PostPacketize(Packet *p)
{
   if (sCaptureStream && p->Type() != eINIT)
   {
      bool doRemoteToo = false;

      p->Send(sCaptureArchive);

      if (p->Item(0).GetFunc() == cmd_eglSwapBuffers)
      {
         // In capture mode, we'll send a swap once every second as a heartbeat.
         // Without it, the UI in SpyTool will not be able to stop us.
         static unsigned int  prev = 0;
         unsigned int         now = plGetTimeNowMs();

         if (now - prev > 1000)
            doRemoteToo = true;
   
         prev = now;
      }

      if (!doRemoteToo)
         return true;
   }

   if (remote == NULL)
      return false;

   bool gotDone = false;
   bool skip = false;

   sSendNextRet = false;

   p->Send(remote);

   while (!gotDone)
   {
      Packet retPacket;
      if (!remote->ReceivePacket(&retPacket))
      {
         fprintf(stderr, "Didn't receive a return control packet\n");
         sOrphaned = true;
         gotDone = true;
         break;
      }

      if (retPacket.NumItems() > 0)
      {
         Control::eControlAction action = (Control::eControlAction)retPacket.Item(0).GetUInt32();
         switch (action)
         {
         case Control::eDone        : gotDone = true; break;
         case Control::eSkip        : gotDone = true; skip = true; break;
         case Control::eDisable     : sEnabled = false; gotDone = true; break;
         case Control::eEnable      : sEnabled = true; gotDone = true; break;
         case Control::eGetGLBuffer : GetGLBuffer(retPacket); break;
         case Control::eGetUniforms : GetUniforms(retPacket); break;
         case Control::eBottleneck  : SetBottleneckMode(retPacket); break;
         case Control::eGetPerfData : GetPerfData(retPacket); break;
         case Control::eGetEventData   : GetEventData(retPacket); break;
         case Control::eDisconnect  : sOrphaned = true; gotDone = true; delete remote; remote = NULL; break;
         case Control::eChangePort     : ChangePort(retPacket); gotDone = true; break;
         case Control::eGetState    : GetState(retPacket); break;
         case Control::eGetMemory   : GetMemory(retPacket); break;
         case Control::eSetVarSet   : SetVarSet(retPacket); break;
         case Control::eChangeShader: ChangeShader(retPacket); break;
         case Control::eBackTrace      : BackTrace(retPacket); break;
         }
      }
   }

   return !skip;
}

static uint32_t AttribListSize(const EGLint *attribList)
{
   uint32_t size = 0;

   if (attribList == NULL)
      return 0;

   while (*attribList++ != EGL_NONE)
      size += sizeof(EGLint);

   return size + sizeof(EGLint); // Include the EGL_NONE token
}

static uint32_t LightParamsSize(GLenum pname)
{
   switch (pname)
   {
   case GL_AMBIENT :
   case GL_DIFFUSE :
   case GL_SPECULAR :
   case GL_POSITION :         return 4;
   case GL_SPOT_DIRECTION :   return 3;
   default :                  return 1;
   }

   return 1;
}

static uint32_t MaterialParamsSize(GLenum pname)
{
   switch (pname)
   {
   case GL_SHININESS :     return 1;
   //case GL_COLOR_INDEXES : return 3;
   default :               return 4;
   }

   return 4;
}

static uint32_t GetESMajorVersion()
{
   EGLint val;
   Real(eglQueryContext)(Real(eglGetCurrentDisplay)(), Real(eglGetCurrentContext)(), EGL_CONTEXT_CLIENT_VERSION, &val);
   return val;
}

static uint32_t BytesForType(GLenum type)
{
   switch (type)
   {
   case GL_BYTE :
   case GL_UNSIGNED_BYTE :  return 1;
   case GL_SHORT :
   case GL_UNSIGNED_SHORT : return 2;
   case GL_INT :
   case GL_UNSIGNED_INT :
   case GL_FLOAT :          return 4;
   }

   return 0;
}

extern "C"
{

#define Func(func, args) \
{ \
   bool ret = true; \
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)\
   {\
      Packet   p(eAPI_FUNCTION); \
      sCurPacket = &p; \
      p.AddItem(cmd_##func); \
      Packetize args; \
      ret = PostPacketize(&p);\
   }\
   return ret;\
}

#define FuncExtra1(func, args, item1) \
   { \
   bool ret = true; \
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)\
   {\
      Packet   p(eAPI_FUNCTION); \
      sCurPacket = &p; \
      p.AddItem(cmd_##func); \
      Packetize args; \
      if (sCaptureStream) \
         sCurPacket->AddItem(PacketItem item1); \
      ret = PostPacketize(&p);\
   }\
   return ret;\
}

#define FuncExtra2(func, args, item1, item2) \
{ \
   bool ret = true; \
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)\
   {\
      Packet   p(eAPI_FUNCTION); \
      sCurPacket = &p; \
      p.AddItem(cmd_##func); \
      Packetize args; \
      if (sCaptureStream) \
      {\
         sCurPacket->AddItem(PacketItem item1); \
         sCurPacket->AddItem(PacketItem item2); \
      }\
      ret = PostPacketize(&p);\
   }\
   return ret;\
}

// AlwaysFunc always sends command, regardless of sEnabled status. That's our heartbeat effectively.
#define AlwaysFunc(func, args) \
{ \
   bool ret = true; \
   if (!sOrphaned)\
   {\
      Packet   p(eAPI_FUNCTION); \
      sCurPacket = &p; \
      p.AddItem(cmd_##func); \
      Packetize args; \
      ret = PostPacketize(&p);\
      sSendNextRet = true;\
   }\
   return ret;\
}

// AlwaysFunc always sends command, regardless of sEnabled status. That's our heartbeat effectively.
#define DebugModeOnlyFunc(func, args) \
{ \
   bool ret = true; \
   if (!sOrphaned && sBottleneckMode == Control::eUnset)\
   {\
      Packet   p(eAPI_FUNCTION); \
      sCurPacket = &p; \
      p.AddItem(cmd_##func); \
      Packetize args; \
      ret = PostPacketize(&p);\
      sSendNextRet = true;\
   }\
   return ret;\
}

#define RetFunc(func, type, ns, args) \
{ \
   if (!sOrphaned && (sEnabled || sSendNextRet) && sBottleneckMode == Control::eUnset)\
   {\
      Packet   p(eRET_CODE); \
      sCurPacket = &p; \
      p.AddItem(0); \
      Packetize args; \
      p.AddItem(ns); \
      PostPacketize(&p);\
   }\
}

#define RetEnumFunc(func, type, ns, args) \
{ \
   if (!sOrphaned && (sEnabled || sSendNextRet) && sBottleneckMode == Control::eUnset)\
   {\
      Packet   p(eRET_CODE); \
      sCurPacket = &p; \
      p.AddItem(1); \
      Packetize args; \
      p.AddItem(ns); \
      PostPacketize(&p);\
   }\
}

#define RetVoidFunc(ns) \
{ \
   if (!sOrphaned && (sEnabled || sSendNextRet) && sBottleneckMode == Control::eUnset)\
   {\
      Packet   p(eRET_CODE); \
      sCurPacket = &p; \
      p.AddItem(2); \
      p.AddItem(ns); \
      PostPacketize(&p);\
   }\
}

#define ErrFunc(func, args) \
{ \
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)\
   {\
      Packet   p(eGL_ERROR); \
      sCurPacket = &p; \
      Packetize args; \
      PostPacketize(&p);\
   }\
}

DLLEXPORT bool DLLEXPORTENTRY remote_log_initialize(REAL_GL_API_TABLE *table, uint32_t size, uint32_t debugBuild,
                                                    uint32_t majorVer, uint32_t minorVer)
{
   static bool inited = false;
   uint32_t pid = (uint32_t)getpid();

   if (inited)
   {
      // If we've already been initialized, the likely scenario is that the client app has torn down Egl and
      // unregistered the platform layer and is now bringing it all back up again.
      // In this case, we'll add a REINIT packet in the capture file if we are capturing.
      if (sCaptureStream)
      {
         Packet   p(eREINIT);
         sCurPacket = &p;
         Packetize(pid, debugBuild, SPYHOOK_MAJOR_VER, SPYHOOK_MINOR_VER);
         PostPacketize(&p);
      }

      return true;
   }
   inited = true;

   plCreateMutex(&sEventMutex);

   memset(&sRealFuncs, 0, sizeof(REAL_GL_API_TABLE));

   if (majorVer != REMOTE_API_MAJOR_VER || minorVer != REMOTE_API_MINOR_VER)
   {
      printf("GPU Monitor DLL version (API ver = %d.%d) does not match GL driver version (API ver = %d.%d). Monitoring disabled.\n\n",
             REMOTE_API_MAJOR_VER, REMOTE_API_MINOR_VER, majorVer, minorVer);
      sOrphaned = true;
      return false;
   }
   else if (size != sizeof(REAL_GL_API_TABLE))
   {
      printf("GPU Monitor DLL API table does not match driver API table. You may need to rebuild SpyHook. Monitoring disabled.\n\n");
      sOrphaned = true;
      return false;
   }
   else
   {
      sRealFuncs = *table;
      printf("Waiting for GPU Monitor connection...\n");
   }

   sEglInitDriverMonitorBRCM =   (PFNEGLINITDRIVERMONITORBRCMPROC  )Real(eglGetProcAddress)("eglInitDriverMonitorBRCM");
   sEglGetDriverMonitorXMLBRCM = (PFNEGLGETDRIVERMONITORXMLBRCMPROC)Real(eglGetProcAddress)("eglGetDriverMonitorXMLBRCM");
   sEglTermDriverMonitorBRCM =   (PFNEGLTERMDRIVERMONITORBRCMPROC  )Real(eglGetProcAddress)("eglTermDriverMonitorBRCM");

   remote = new Remote(28015);
   if (remote->Connect())
   {
      printf("GPU Monitor initializing...\n");

      // Send our init packet
      Packet   p(eINIT);
      sCurPacket = &p;
      Packetize(pid, debugBuild, SPYHOOK_MAJOR_VER, SPYHOOK_MINOR_VER);
      PostPacketize(&p);
      plGetTimeNowMs(); // Initialize the internal timebase
   }
   else
   {
      sOrphaned = true;
      return false;
   }

   return true;
}

DLLEXPORT uint32_t DLLEXPORTENTRY remote_inBottleneckMode(void)
{
   return (sBottleneckMode != Control::eUnset) ? 1 : 0;
}

DLLEXPORT uint32_t DLLEXPORTENTRY remote_getBottleneckMode(void)
{
   if (sBottleneckMode == Control::eUnset)
      return Control::eNone;
   return sBottleneckMode;
}

DLLEXPORT void DLLEXPORTENTRY remote_error(GLenum error) ErrFunc(remote_error, (error))

DLLEXPORT void DLLEXPORTENTRY remote_return_void(uint32_t ns) RetVoidFunc(ns)
DLLEXPORT void DLLEXPORTENTRY remote_return_GLboolean(GLboolean b, uint32_t ns) RetFunc(remote_return, GLboolean, ns, (b))
DLLEXPORT void DLLEXPORTENTRY remote_return_GLbitfield(GLbitfield b, uint32_t ns) RetFunc(remote_return, GLbitfield, ns, (b))
DLLEXPORT void DLLEXPORTENTRY remote_return_GLenum(GLenum b, uint32_t ns) RetEnumFunc(remote_return, GLenum, ns, (b))
DLLEXPORT void DLLEXPORTENTRY remote_return_GLint(GLint b, uint32_t ns) RetFunc(remote_return, GLint, ns, (b))
DLLEXPORT void DLLEXPORTENTRY remote_return_GLuint(GLuint b, uint32_t ns) RetFunc(remote_return, GLuint, ns, (b))

DLLEXPORT void DLLEXPORTENTRY remote_return_EGLBoolean(EGLBoolean b, uint32_t ns) RetFunc(remote_return, EGLBoolean, ns, (b))
DLLEXPORT void DLLEXPORTENTRY remote_return_EGLDisplay(EGLDisplay b, uint32_t ns) RetFunc(remote_return, EGLDisplay, ns, (b))
DLLEXPORT void DLLEXPORTENTRY remote_return_EGLSurface(EGLSurface s, uint32_t ns)
{
   if (!sOrphaned && (sEnabled || sSendNextRet) && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eRET_CODE);
      sCurPacket = &p;
      p.AddItem(0);
      Packetize(s);

      // Since this is a surface, we'll attach the dimensions etc as extra data
      EGLDisplay disp = Real(eglGetDisplay)(EGL_DEFAULT_DISPLAY);
      EGLint w, h, configId, r, g, b, a;

      Real(eglQuerySurface)(disp, s, EGL_WIDTH, &w);
      Real(eglQuerySurface)(disp, s, EGL_HEIGHT, &h);
      Real(eglQuerySurface)(disp, s, EGL_CONFIG_ID, &configId);

      Real(eglGetConfigAttrib)(disp, (EGLConfig)configId, EGL_RED_SIZE, &r);
      Real(eglGetConfigAttrib)(disp, (EGLConfig)configId, EGL_GREEN_SIZE, &g);
      Real(eglGetConfigAttrib)(disp, (EGLConfig)configId, EGL_BLUE_SIZE, &b);
      Real(eglGetConfigAttrib)(disp, (EGLConfig)configId, EGL_ALPHA_SIZE, &a);

      Packetize(w, h, r, g, b, a);

      p.AddItem(ns);

      PostPacketize(&p);
   }
}
DLLEXPORT void DLLEXPORTENTRY remote_return_EGLEnum(EGLenum b, uint32_t ns) RetEnumFunc(remote_return, EGLenum, ns, (b))
DLLEXPORT void DLLEXPORTENTRY remote_return_EGLContext(EGLContext b, uint32_t ns) RetFunc(remote_return, EGLContext, ns, (b))
DLLEXPORT void DLLEXPORTENTRY remote_return_EGLint(EGLint b, uint32_t ns) RetFunc(remote_return, EGLint, ns, (b))
DLLEXPORT void DLLEXPORTENTRY remote_return_EGLenum(EGLenum b, uint32_t ns) RetEnumFunc(remote_return, EGLenum, ns, (b))
DLLEXPORT void DLLEXPORTENTRY remote_return_EGLImageKHR(EGLImageKHR b, uint32_t ns) RetFunc(remote_return, EGLImageKHR, ns, (b))
DLLEXPORT void DLLEXPORTENTRY remote_return_EGLSyncKHR(EGLSyncKHR b, uint32_t ns) RetFunc(remote_return, EGLSyncKHR, ns, (b))

DLLEXPORT void DLLEXPORTENTRY remote_event_notify(EventData *ev)
{
   if (!sOrphaned && sEnabled && sBottleneckMode != Control::eUnset && sCaptureEvents)
   {
      DriverEvent event;

      uint32_t s, ns;
      plGetAccurateTime(&s, &ns);

      event.eventCode = ev->eventCode;
      event.eventType = ev->eventType;
      event.eventRow  = ev->eventRow;
      event.eventData = ev->eventData;
      if (ev->desc != NULL)
         event.desc = ev->desc;
      event.secs = s;
      event.ns = ns;

      plLockMutex(&sEventMutex);
      sEventBlock.push_back(event);
      plUnlockMutex(&sEventMutex);
   }
}

DLLEXPORT bool DLLEXPORTENTRY remote_glIsBuffer (GLuint buffer) Func(glIsBuffer, (buffer))
DLLEXPORT bool DLLEXPORTENTRY remote_glIsEnabled (GLenum cap) Func(glIsEnabled, (cap))
DLLEXPORT bool DLLEXPORTENTRY remote_glIsFramebuffer (GLuint framebuffer) Func(glIsFramebuffer, (framebuffer))
DLLEXPORT bool DLLEXPORTENTRY remote_glIsProgram (GLuint program) Func(glIsProgram, (program))
DLLEXPORT bool DLLEXPORTENTRY remote_glIsRenderbuffer (GLuint renderbuffer) Func(glIsRenderbuffer, (renderbuffer))
DLLEXPORT bool DLLEXPORTENTRY remote_glIsShader (GLuint shader) Func(glIsShader, (shader))
DLLEXPORT bool DLLEXPORTENTRY remote_glIsTexture (GLuint texture) Func(glIsTexture, (texture))
DLLEXPORT bool DLLEXPORTENTRY remote_glCheckFramebufferStatus (GLenum target) Func(glCheckFramebufferStatus , (target))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetError (void) Func(glGetError, ())
DLLEXPORT bool DLLEXPORTENTRY remote_glCreateProgram (void) Func(glCreateProgram, ())
DLLEXPORT bool DLLEXPORTENTRY remote_glCreateShader (GLenum type) Func(glCreateShader ,(type))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetString (GLenum name) Func(glGetString, (name))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetAttribLocation (GLuint program, const GLchar* name) Func(glGetAttribLocation , (program, name))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetUniformLocation (GLuint program, const GLchar* name) Func(glGetUniformLocation , (program, name))
DLLEXPORT bool DLLEXPORTENTRY remote_glActiveTexture (GLenum texture) Func(glActiveTexture, (texture))
DLLEXPORT bool DLLEXPORTENTRY remote_glAlphaFunc (GLenum func, GLclampf ref) Func(glAlphaFunc, (func, ref))
DLLEXPORT bool DLLEXPORTENTRY remote_glAlphaFuncx (GLenum func, GLclampx ref) Func(glAlphaFuncx, (func, ref))
DLLEXPORT bool DLLEXPORTENTRY remote_glAttachShader (GLuint program, GLuint shader) Func(glAttachShader, (program, shader))
DLLEXPORT bool DLLEXPORTENTRY remote_glBindAttribLocation (GLuint program, GLuint index, const GLchar* name) Func(glBindAttribLocation, (program, index, name))
DLLEXPORT bool DLLEXPORTENTRY remote_glBindBuffer (GLenum target, GLuint buffer) Func(glBindBuffer, (target, buffer))
DLLEXPORT bool DLLEXPORTENTRY remote_glBindFramebuffer (GLenum target, GLuint framebuffer) DebugModeOnlyFunc(glBindFramebuffer, (target, framebuffer))
DLLEXPORT bool DLLEXPORTENTRY remote_glBindRenderbuffer (GLenum target, GLuint renderbuffer) Func(glBindRenderbuffer, (target, renderbuffer))
DLLEXPORT bool DLLEXPORTENTRY remote_glBindTexture (GLenum target, GLuint texture) Func(glBindTexture, (target, texture))
DLLEXPORT bool DLLEXPORTENTRY remote_glBlendColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) Func(glBlendColor, (red, green, blue, alpha))
DLLEXPORT bool DLLEXPORTENTRY remote_glBlendEquation ( GLenum mode ) Func(glBlendEquation, (mode))
DLLEXPORT bool DLLEXPORTENTRY remote_glBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha) Func(glBlendEquationSeparate, (modeRGB, modeAlpha))
DLLEXPORT bool DLLEXPORTENTRY remote_glBlendFunc (GLenum sfactor, GLenum dfactor) Func(glBlendFunc, (sfactor, dfactor))
DLLEXPORT bool DLLEXPORTENTRY remote_glBlendFuncSeparate (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) Func(glBlendFuncSeparate, (srcRGB, dstRGB, srcAlpha, dstAlpha))
DLLEXPORT bool DLLEXPORTENTRY remote_glBufferData (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) 
{
   bool ret = true;
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eAPI_FUNCTION);
      sCurPacket = &p;
      p.AddItem(cmd_glBufferData);
      Packetize(target, size, data, usage);
      if (sCaptureStream)
         sCurPacket->AddItem(PacketItem((void*)data, size));
      ret = PostPacketize(&p);

      if (sCaptureStream)
      {
         // For index buffers, we need to store a record of the max index
         if (target == GL_ELEMENT_ARRAY_BUFFER && data != NULL)
         {
            EGLContext curCtx = Real(eglGetCurrentContext)();
            GLint      elementBind = 0;
            GLuint     maxByteIndx = 0;
            GLuint     maxShortIndx = 0;

            Real(glGetIntegerv)(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementBind);

            for (GLint i = 0; i < size; i++)
            {
               if (((GLubyte*)data)[i] > maxByteIndx)
                  maxByteIndx = ((GLubyte*)data)[i];

               if ((i & 1) == 0)
               {
                  if (((GLushort*)data)[i/2] > maxShortIndx)
                     maxShortIndx = ((GLushort*)data)[i/2];
               }
            }

            Buffer *buffer = &sContexts[(GLuint)curCtx].m_buffers[elementBind];
            buffer->m_maxByteIndx = maxByteIndx;
            buffer->m_maxShortIndx = maxShortIndx;
         }
      }
   }
   return ret;
}
DLLEXPORT bool DLLEXPORTENTRY remote_glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
   bool ret = true;
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eAPI_FUNCTION);
      sCurPacket = &p;
      p.AddItem(cmd_glBufferSubData);
      Packetize(target, offset, size, data);
      if (sCaptureStream)
         sCurPacket->AddItem(PacketItem((void*)data, size));
      ret = PostPacketize(&p);

      if (sCaptureStream)
      {
         // Update max indices for indexed buffers
         if (target == GL_ELEMENT_ARRAY_BUFFER && data != NULL)
         {
            EGLContext curCtx = Real(eglGetCurrentContext)();
            GLint      elementBind = 0;
            GLuint     maxByteIndx = 0;
            GLuint     maxShortIndx = 0;

            Real(glGetIntegerv)(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementBind);

            for (GLint i = 0; i < size; i++)
            {
               if (((GLubyte*)data)[i] > maxByteIndx)
                  maxByteIndx = ((GLubyte*)data)[i];

               if ((i & 1) == 0)
               {
                  if (((GLushort*)data)[i/2] > maxShortIndx)
                     maxShortIndx = ((GLushort*)data)[i/2];
               }
            }

            Buffer *buffer = &sContexts[(GLuint)curCtx].m_buffers[elementBind];
            if (maxByteIndx > buffer->m_maxByteIndx)
               buffer->m_maxByteIndx = maxByteIndx;
            if (maxShortIndx > buffer->m_maxShortIndx)
               buffer->m_maxShortIndx = maxShortIndx;
         }
      }
   }
   return ret;
}
DLLEXPORT bool DLLEXPORTENTRY remote_glClear (GLbitfield mask) Func(glClear, (mask))
DLLEXPORT bool DLLEXPORTENTRY remote_glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) Func(glClearColor, (red, green, blue, alpha))
DLLEXPORT bool DLLEXPORTENTRY remote_glClearColorx (GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha) Func(glClearColorx, (red, green, blue, alpha))
DLLEXPORT bool DLLEXPORTENTRY remote_glClearDepthf (GLclampf depth) Func(glClearDepthf, (depth))
DLLEXPORT bool DLLEXPORTENTRY remote_glClearDepthx (GLclampx depth) Func(glClearDepthx, (depth))
DLLEXPORT bool DLLEXPORTENTRY remote_glClearStencil (GLint s) Func(glClearStencil, (s))
DLLEXPORT bool DLLEXPORTENTRY remote_glClientActiveTexture (GLenum texture) Func(glClientActiveTexture, (texture))
DLLEXPORT bool DLLEXPORTENTRY remote_glClipPlanef (GLenum plane, const GLfloat *equation) 
   FuncExtra1(glClipPlanef, (plane, equation), ((void*)equation, 4 * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glClipPlanex (GLenum plane, const GLfixed *equation) 
   FuncExtra1(glClipPlanex, (plane, equation), ((void*)equation, 4 * sizeof(GLfixed)))
DLLEXPORT bool DLLEXPORTENTRY remote_glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) Func(glColor4f, (red, green, blue, alpha))
DLLEXPORT bool DLLEXPORTENTRY remote_glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha) Func(glColor4ub, (red, green, blue, alpha))
DLLEXPORT bool DLLEXPORTENTRY remote_glColor4x (GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha) Func(glColor4x, (red, green, blue, alpha))
DLLEXPORT bool DLLEXPORTENTRY remote_glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) Func(glColorMask, (red, green, blue, alpha))
DLLEXPORT bool DLLEXPORTENTRY remote_glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) 
{
   bool ret = true;
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eAPI_FUNCTION);
      sCurPacket = &p;
      p.AddItem(cmd_glColorPointer);
      Packetize(size, type, stride, pointer);

      if (sCaptureStream)
      {
         GLint v[4];
         Real(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, v);
         Packetize(v[0] != 0);
      }

      ret = PostPacketize(&p);
   }
   return ret;
}
DLLEXPORT bool DLLEXPORTENTRY remote_glCompileShader (GLuint shader) Func(glCompileShader, (shader))
DLLEXPORT bool DLLEXPORTENTRY remote_glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data) 
   FuncExtra1(glCompressedTexImage2D, (target, level, internalformat, width, height, border, imageSize, data),
                                      ((void*)data, imageSize))
DLLEXPORT bool DLLEXPORTENTRY remote_glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data) 
   FuncExtra1(glCompressedTexSubImage2D, (target, level, xoffset, yoffset, width, height, format, imageSize, data),
                                         ((void*)data, imageSize))
DLLEXPORT bool DLLEXPORTENTRY remote_glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) Func(glCopyTexImage2D, (target, level, internalformat, x, y, width, height, border))
DLLEXPORT bool DLLEXPORTENTRY remote_glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) Func(glCopyTexSubImage2D, (target, level, xoffset, yoffset, x, y, width, height))
DLLEXPORT bool DLLEXPORTENTRY remote_glCullFace (GLenum mode) Func(glCullFace, (mode))
DLLEXPORT bool DLLEXPORTENTRY remote_glDeleteBuffers (GLsizei n, const GLuint *buffers) 
   FuncExtra1(glDeleteBuffers, (n, buffers), ((void*)buffers, n * sizeof(GLuint)))
DLLEXPORT bool DLLEXPORTENTRY remote_glDeleteFramebuffers (GLsizei n, const GLuint* framebuffers) 
   FuncExtra1(glDeleteFramebuffers, (n, framebuffers), ((void*)framebuffers, n * sizeof(GLuint)))
DLLEXPORT bool DLLEXPORTENTRY remote_glDeleteProgram (GLuint program) Func(glDeleteProgram, (program))
DLLEXPORT bool DLLEXPORTENTRY remote_glDeleteRenderbuffers (GLsizei n, const GLuint* renderbuffers) 
   FuncExtra1(glDeleteRenderbuffers, (n, renderbuffers), ((void*)renderbuffers, n * sizeof(GLuint)))
DLLEXPORT bool DLLEXPORTENTRY remote_glDeleteShader (GLuint shader) Func(glDeleteShader, (shader))
DLLEXPORT bool DLLEXPORTENTRY remote_glDeleteTextures (GLsizei n, const GLuint* textures) 
   FuncExtra1(glDeleteTextures, (n, textures), ((void*)textures, n * sizeof(GLuint)))
DLLEXPORT bool DLLEXPORTENTRY remote_glDepthFunc (GLenum func) Func(glDepthFunc, (func))
DLLEXPORT bool DLLEXPORTENTRY remote_glDepthMask (GLboolean flag) Func(glDepthMask, (flag))
DLLEXPORT bool DLLEXPORTENTRY remote_glDepthRangef (GLclampf zNear, GLclampf zFar) Func(glDepthRangef, (zNear, zFar))
DLLEXPORT bool DLLEXPORTENTRY remote_glDepthRangex (GLclampx zNear, GLclampx zFar) Func(glDepthRangex, (zNear, zFar))
DLLEXPORT bool DLLEXPORTENTRY remote_glDetachShader (GLuint program, GLuint shader) Func(glDetachShader, (program, shader))
DLLEXPORT bool DLLEXPORTENTRY remote_glDisable (GLenum cap) Func(glDisable, (cap))
DLLEXPORT bool DLLEXPORTENTRY remote_glDisableClientState (GLenum array) Func(glDisableClientState, (array))
DLLEXPORT bool DLLEXPORTENTRY remote_glDisableVertexAttribArray (GLuint index) Func(glDisableVertexAttribArray, (index))
DLLEXPORT bool DLLEXPORTENTRY remote_glDrawArrays (GLenum mode, GLint first, GLsizei count) 
{
   bool ret = true;
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eAPI_FUNCTION);
      sCurPacket = &p;
      p.AddItem(cmd_glDrawArrays);
      Packetize(mode, first, count);

      if (sCaptureStream)
      {
         {
            GLint type, bytesPerVert, stride, norm, size;
            
            // Are we ES1 or ES2?
            uint32_t esMajVer = GetESMajorVersion();
            Packetize(esMajVer);

            if (esMajVer == 1)
            {
               void *ptr;

               GLint v[4];
               Real(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, v);
               Packetize(v[0] != 0);

               if (v[0] == 0) // Need to send vertex array data along with this packet
               {
                  // Attach the data for any active ES1 client-side arrays
                  Real(glGetPointerv)(GL_VERTEX_ARRAY_POINTER, &ptr);
                  if (ptr != NULL)
                  {
                     Real(glGetIntegerv)(GL_VERTEX_ARRAY_SIZE, &size);
                     Real(glGetIntegerv)(GL_VERTEX_ARRAY_TYPE, &type);
                     Real(glGetIntegerv)(GL_VERTEX_ARRAY_STRIDE, &stride);

                     Packetize(GL_VERTEX_ARRAY_POINTER, size, type, stride);
                     if (stride != 0)
                        bytesPerVert = stride;
                     else
                        bytesPerVert = BytesForType(type) * size;
                     sCurPacket->AddItem(PacketItem(ptr, bytesPerVert * (first + count)));
                  }

                  Real(glGetPointerv)(GL_COLOR_ARRAY_POINTER, &ptr);
                  if (ptr != NULL)
                  {
                     Real(glGetIntegerv)(GL_COLOR_ARRAY_SIZE, &size);
                     Real(glGetIntegerv)(GL_COLOR_ARRAY_TYPE, &type);
                     Real(glGetIntegerv)(GL_COLOR_ARRAY_STRIDE, &stride);

                     Packetize(GL_COLOR_ARRAY_POINTER, size, type, stride);
                     if (stride != 0)
                        bytesPerVert = stride;
                     else
                        bytesPerVert = BytesForType(type) * size;
                     sCurPacket->AddItem(PacketItem(ptr, bytesPerVert * (first + count)));
                  }

                  Real(glGetPointerv)(GL_TEXTURE_COORD_ARRAY_POINTER, &ptr);
                  if (ptr != NULL)
                  {
                     Real(glGetIntegerv)(GL_TEXTURE_COORD_ARRAY_SIZE, &size);
                     Real(glGetIntegerv)(GL_TEXTURE_COORD_ARRAY_TYPE, &type);
                     Real(glGetIntegerv)(GL_TEXTURE_COORD_ARRAY_STRIDE, &stride);

                     Packetize(GL_TEXTURE_COORD_ARRAY_POINTER, size, type, stride);
                     if (stride != 0)
                        bytesPerVert = stride;
                     else
                        bytesPerVert = BytesForType(type) * size;
                     sCurPacket->AddItem(PacketItem(ptr, bytesPerVert * (first + count)));
                  }

                  Real(glGetPointerv)(GL_NORMAL_ARRAY_POINTER, &ptr);
                  if (ptr != NULL)
                  {
                     Real(glGetIntegerv)(GL_NORMAL_ARRAY_TYPE, &type);
                     Real(glGetIntegerv)(GL_NORMAL_ARRAY_STRIDE, &stride);

                     Packetize(GL_NORMAL_ARRAY_POINTER, type, stride);
                     if (stride != 0)
                        bytesPerVert = stride;
                     else
                        bytesPerVert = BytesForType(type) * size;
                     sCurPacket->AddItem(PacketItem(ptr, bytesPerVert * (first + count)));
                  }

                  Real(glGetPointerv)(GL_POINT_SIZE_ARRAY_POINTER_OES, &ptr);
                  if (ptr != NULL)
                  {
                     Real(glGetIntegerv)(GL_POINT_SIZE_ARRAY_TYPE_OES, &type);
                     Real(glGetIntegerv)(GL_POINT_SIZE_ARRAY_STRIDE_OES, &stride);

                     Packetize(GL_POINT_SIZE_ARRAY_POINTER_OES, type, stride);
                     if (stride != 0)
                        bytesPerVert = stride;
                     else
                        bytesPerVert = BytesForType(type) * size;
                     sCurPacket->AddItem(PacketItem(ptr, bytesPerVert * (first + count)));
                  }

                  Packetize(-1);
               }
            }
            else if (esMajVer == 2)
            {
               static GLint maxAttribs = 0;
               if (maxAttribs == 0)
                  Real(glGetIntegerv)(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);

               for (GLint a = 0; a < maxAttribs; a++)
               {
                  void *attribs = NULL;
                  GLint binding;
                  GLint enabled;

                  Real(glGetVertexAttribiv)(a, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &binding);
                  Real(glGetVertexAttribiv)(a, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);

                  if (binding == 0 && enabled)
                  {
                     Real(glGetVertexAttribPointerv)(a, GL_VERTEX_ATTRIB_ARRAY_POINTER, &attribs);
                     Real(glGetVertexAttribiv)(a, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
                     Real(glGetVertexAttribiv)(a, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);
                     Real(glGetVertexAttribiv)(a, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &norm);
                     Real(glGetVertexAttribiv)(a, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);

                     if (stride != 0)
                        bytesPerVert = stride;
                     else
                        bytesPerVert = BytesForType(type) * size;

                     if (attribs != NULL)
                     {
                        Packetize(a, size, type, norm, stride);
                        sCurPacket->AddItem(PacketItem(attribs, bytesPerVert * (first + count)));
                     }
                  }
               }

               Packetize(-1);
            }
         }
      }

      ret = PostPacketize(&p);
   }
   return ret;
}

static uint32_t FindMaxIndex(GLsizei count, GLenum type, const GLvoid* indices)
{
   // Work out what the max index is
   GLint maxIndx = 0;
   GLint elementBind;
   Real(glGetIntegerv)(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementBind);

   if (elementBind == 0)
   {
      // Scan the client side index array
      int32_t i;
      if (type == GL_BYTE || type == GL_UNSIGNED_BYTE)
      {
         for (i = 0; i < count; i++)
            if (((uint8_t*)indices)[i] > maxIndx)
               maxIndx = ((uint8_t*)indices)[i];
      }
      else
      {
         for (i = 0; i < count; i++)
            if (((uint16_t*)indices)[i] > maxIndx)
               maxIndx = ((uint16_t*)indices)[i];
      }
   }
   else
   {
      // Need to use the VBO index buffer
      EGLContext curCtx = Real(eglGetCurrentContext)();

      Buffer *buffer = &sContexts[(GLuint)curCtx].m_buffers[elementBind];

      switch (type)
      {
      case GL_BYTE :
      case GL_UNSIGNED_BYTE :  maxIndx = buffer->m_maxByteIndx; break;
      case GL_SHORT :
      case GL_UNSIGNED_SHORT :
      default :                maxIndx =  buffer->m_maxShortIndx; break;
      }
   }

   return maxIndx;
}

DLLEXPORT bool DLLEXPORTENTRY remote_glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices) 
{
   bool ret = true;
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eAPI_FUNCTION);
      sCurPacket = &p;
      p.AddItem(cmd_glDrawElements);
      Packetize(mode, count, type, indices);

      if (sCaptureStream)
      {
         GLint elementBind;
         Real(glGetIntegerv)(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementBind);

         Packetize(elementBind != 0);
         
         // Do we need to send the client-side indices array?
         if (elementBind == 0) 
         {
            uint32_t bytesPerIndex = 0;
            switch (type)
            {
            case GL_UNSIGNED_BYTE  : bytesPerIndex = 1; break;
            case GL_UNSIGNED_SHORT : bytesPerIndex = 2; break;
            }

            sCurPacket->AddItem(PacketItem((void*)indices, bytesPerIndex * count));
         }
         else
            sCurPacket->AddItem(PacketItem((void*)NULL, 0));


         // Are we ES1 or ES2?
         uint32_t esMajVer = GetESMajorVersion();
         Packetize(esMajVer);

         // Do we need to send vertex array data along with this packet
         {
            GLint typ, bytesPerVert, stride, norm, size;

            if (esMajVer == 1)
            {
               GLint arrayBind;
               void *ptr;

               Real(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, &arrayBind);
               Packetize(arrayBind != 0);

               if (arrayBind == 0)
               {
                  // Work out what the max index is (and therefore how many attribs to send)
                  GLint maxIndx = FindMaxIndex(count, type, indices);

                  // Ensure we get the last element
                  maxIndx += 1;

                  // Attach the data for any active ES1 client-side arrays
                  Real(glGetPointerv)(GL_VERTEX_ARRAY_POINTER, &ptr);
                  if (ptr != NULL)
                  {
                     Real(glGetIntegerv)(GL_VERTEX_ARRAY_SIZE, &size);
                     Real(glGetIntegerv)(GL_VERTEX_ARRAY_TYPE, &typ);
                     Real(glGetIntegerv)(GL_VERTEX_ARRAY_STRIDE, &stride);

                     Packetize(GL_VERTEX_ARRAY_POINTER, size, typ, stride);
                     if (stride != 0)
                        bytesPerVert = stride;
                     else
                        bytesPerVert = BytesForType(typ) * size;
                     sCurPacket->AddItem(PacketItem(ptr, bytesPerVert * maxIndx));
                  }

                  Real(glGetPointerv)(GL_COLOR_ARRAY_POINTER, &ptr);
                  if (ptr != NULL)
                  {
                     Real(glGetIntegerv)(GL_COLOR_ARRAY_SIZE, &size);
                     Real(glGetIntegerv)(GL_COLOR_ARRAY_TYPE, &typ);
                     Real(glGetIntegerv)(GL_COLOR_ARRAY_STRIDE, &stride);

                     Packetize(GL_COLOR_ARRAY_POINTER, size, typ, stride);
                     if (stride != 0)
                        bytesPerVert = stride;
                     else
                        bytesPerVert = BytesForType(typ) * size;
                     sCurPacket->AddItem(PacketItem(ptr, bytesPerVert * maxIndx));
                  }

                  Real(glGetPointerv)(GL_TEXTURE_COORD_ARRAY_POINTER, &ptr);
                  if (ptr != NULL)
                  {
                     Real(glGetIntegerv)(GL_TEXTURE_COORD_ARRAY_SIZE, &size);
                     Real(glGetIntegerv)(GL_TEXTURE_COORD_ARRAY_TYPE, &typ);
                     Real(glGetIntegerv)(GL_TEXTURE_COORD_ARRAY_STRIDE, &stride);

                     Packetize(GL_TEXTURE_COORD_ARRAY_POINTER, size, typ, stride);
                     if (stride != 0)
                        bytesPerVert = stride;
                     else
                        bytesPerVert = BytesForType(typ) * size;
                     sCurPacket->AddItem(PacketItem(ptr, bytesPerVert * maxIndx));
                  }

                  Real(glGetPointerv)(GL_NORMAL_ARRAY_POINTER, &ptr);
                  if (ptr != NULL)
                  {
                     Real(glGetIntegerv)(GL_NORMAL_ARRAY_TYPE, &typ);
                     Real(glGetIntegerv)(GL_NORMAL_ARRAY_STRIDE, &stride);

                     Packetize(GL_NORMAL_ARRAY_POINTER, typ, stride);
                     if (stride != 0)
                        bytesPerVert = stride;
                     else
                        bytesPerVert = BytesForType(typ) * 3;
                     sCurPacket->AddItem(PacketItem(ptr, bytesPerVert * maxIndx));
                  }

                  Real(glGetPointerv)(GL_POINT_SIZE_ARRAY_POINTER_OES, &ptr);
                  if (ptr != NULL)
                  {
                     Real(glGetIntegerv)(GL_POINT_SIZE_ARRAY_TYPE_OES, &typ);
                     Real(glGetIntegerv)(GL_POINT_SIZE_ARRAY_STRIDE_OES, &stride);

                     Packetize(GL_POINT_SIZE_ARRAY_POINTER_OES, typ, stride);
                     if (stride != 0)
                        bytesPerVert = stride;
                     else
                        bytesPerVert = BytesForType(typ) * 2;
                     sCurPacket->AddItem(PacketItem(ptr, bytesPerVert * maxIndx));
                  }

                  Packetize(-1);
               }
            }
            else if (esMajVer == 2)
            {
               GLint maxIndx = -1;

               static GLint maxAttribs = 0;
               if (maxAttribs == 0)
                  Real(glGetIntegerv)(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);

               for (GLint a = 0; a < maxAttribs; a++)
               {
                  void *attribs = NULL;
                  GLint binding;
                  GLint enabled;

                  Real(glGetVertexAttribiv)(a, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &binding);
                  Real(glGetVertexAttribiv)(a, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);

                  if (binding == 0 && enabled)
                  {
                     if (maxIndx == -1)
                     {
                        // Work out what the max index is (and therefore how many attribs to send)
                        maxIndx = FindMaxIndex(count, type, indices);

                        // Ensure we get the last element
                        maxIndx += 1;
                     }

                     Real(glGetVertexAttribPointerv)(a, GL_VERTEX_ATTRIB_ARRAY_POINTER, &attribs);
                     Real(glGetVertexAttribiv)(a, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size);
                     Real(glGetVertexAttribiv)(a, GL_VERTEX_ATTRIB_ARRAY_TYPE, &typ);
                     Real(glGetVertexAttribiv)(a, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &norm);
                     Real(glGetVertexAttribiv)(a, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride);

                     if (stride != 0)
                        bytesPerVert = stride;
                     else
                        bytesPerVert = BytesForType(typ) * size;

                     if (attribs != NULL)
                     {
                        Packetize(a, size, typ, norm, stride);
                        sCurPacket->AddItem(PacketItem(attribs, bytesPerVert * maxIndx));
                     }
                  }
               }

               Packetize(-1);
            }
         }
      }

      ret = PostPacketize(&p);
   }
   return ret;
}

DLLEXPORT bool DLLEXPORTENTRY remote_glEnable (GLenum cap) Func(glEnable, (cap))
DLLEXPORT bool DLLEXPORTENTRY remote_glEnableClientState (GLenum array) Func(glEnableClientState, (array))
DLLEXPORT bool DLLEXPORTENTRY remote_glEnableVertexAttribArray (GLuint index) Func(glEnableVertexAttribArray, (index))
DLLEXPORT bool DLLEXPORTENTRY remote_glFinish (void) AlwaysFunc(glFinish, ())
DLLEXPORT bool DLLEXPORTENTRY remote_glFlush (void) Func(glFlush, ())
DLLEXPORT bool DLLEXPORTENTRY remote_glFogf (GLenum pname, GLfloat param) Func(glFogf, (pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glFogfv (GLenum pname, const GLfloat *params) 
   FuncExtra1(glFogfv, (pname, params), ((void*)params, (pname == GL_FOG_COLOR ? 4 : 1) * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glFogx (GLenum pname, GLfixed param) Func(glFogx, (pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glFogxv (GLenum pname, const GLfixed *params)
   FuncExtra1(glFogxv, (pname, params), ((void*)params, (pname == GL_FOG_COLOR ? 4 : 1) * sizeof(GLfixed)))
DLLEXPORT bool DLLEXPORTENTRY remote_glFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) Func(glFramebufferRenderbuffer, (target, attachment, renderbuffertarget, renderbuffer))
DLLEXPORT bool DLLEXPORTENTRY remote_glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) Func(glFramebufferTexture2D, (target, attachment, textarget, texture, level))
DLLEXPORT bool DLLEXPORTENTRY remote_glFrontFace (GLenum mode) Func(glFrontFace, (mode))
DLLEXPORT bool DLLEXPORTENTRY remote_glFrustumf (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) Func(glFrustumf, (left, right, bottom, top, zNear, zFar))
DLLEXPORT bool DLLEXPORTENTRY remote_glFrustumx (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar) Func(glFrustumx, (left, right, bottom, top, zNear, zFar))
DLLEXPORT bool DLLEXPORTENTRY remote_glGenBuffers (GLsizei n, GLuint* buffers) Func(glGenBuffers, (n, buffers))
DLLEXPORT bool DLLEXPORTENTRY remote_glGenFramebuffers (GLsizei n, GLuint* framebuffers) Func(glGenFramebuffers, (n, framebuffers))
DLLEXPORT bool DLLEXPORTENTRY remote_glGenRenderbuffers (GLsizei n, GLuint* renderbuffers) Func(glGenRenderbuffers, (n, renderbuffers))
DLLEXPORT bool DLLEXPORTENTRY remote_glGenTextures (GLsizei n, GLuint *textures) Func(glGenTextures, (n, textures))
DLLEXPORT bool DLLEXPORTENTRY remote_glGenerateMipmap (GLenum target) Func(glGenerateMipmap, (target))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) Func(glGetActiveAttrib, (program, index, bufsize, length, size, type, name))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetActiveUniform (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) Func(glGetActiveUniform, (program, index, bufsize, length, size, type, name))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetAttachedShaders (GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders) Func(glGetAttachedShaders, (program, maxcount, count, shaders))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetBooleanv (GLenum pname, GLboolean* params) Func(glGetBooleanv, (pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetBufferParameteriv (GLenum target, GLenum pname, GLint* params) Func(glGetBufferParameteriv, (target, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetClipPlanef (GLenum pname, GLfloat eqn[4]) Func(glGetClipPlanef, (pname, eqn))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetClipPlanex (GLenum pname, GLfixed eqn[4]) Func(glGetClipPlanex, (pname, eqn))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetFixedv (GLenum pname, GLfixed *params) Func(glGetFixedv, (pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetFloatv (GLenum pname, GLfloat *params) Func(glGetFloatv, (pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint* params) Func(glGetFramebufferAttachmentParameteriv, (target, attachment, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetIntegerv (GLenum pname, GLint *params) Func(glGetIntegerv, (pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetLightfv (GLenum light, GLenum pname, GLfloat *params) Func(glGetLightfv, (light, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetLightxv (GLenum light, GLenum pname, GLfixed *params) Func(glGetLightxv, (light, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params) Func(glGetMaterialfv, (face, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetMaterialxv (GLenum face, GLenum pname, GLfixed *params) Func(glGetMaterialxv, (face, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetPointerv (GLenum pname, GLvoid **params) Func(glGetPointerv, (pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetProgramInfoLog (GLuint program, GLsizei bufsize, GLsizei* length, GLchar* infolog) Func(glGetProgramInfoLog, (program, bufsize, length, infolog))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetProgramiv (GLuint program, GLenum pname, GLint* params) Func(glGetProgramiv, (program, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetRenderbufferParameteriv (GLenum target, GLenum pname, GLint* params) Func(glGetRenderbufferParameteriv, (target, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetShaderInfoLog (GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* infolog) Func(glGetShaderInfoLog, (shader, bufsize, length, infolog))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetShaderPrecisionFormat (GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision) Func(glGetShaderPrecisionFormat, (shadertype, precisiontype, range, precision))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetShaderSource (GLuint shader, GLsizei bufsize, GLsizei* length, GLchar* source) Func(glGetShaderSource, (shader, bufsize, length, source))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetShaderiv (GLuint shader, GLenum pname, GLint* params) Func(glGetShaderiv, (shader, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetTexEnvfv (GLenum env, GLenum pname, GLfloat *params) Func(glGetTexEnvfv, (env, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetTexEnviv (GLenum env, GLenum pname, GLint *params) Func(glGetTexEnviv, (env, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetTexEnvxv (GLenum env, GLenum pname, GLfixed *params) Func(glGetTexEnvxv, (env, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params) Func(glGetTexParameterfv, (target, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetTexParameteriv (GLenum target, GLenum pname, GLint *params) Func(glGetTexParameteriv, (target, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetTexParameterxv (GLenum target, GLenum pname, GLfixed *params) Func(glGetTexParameterxv, (target, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetUniformfv (GLuint program, GLint location, GLfloat* params) Func(glGetUniformfv, (program, location, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetUniformiv (GLuint program, GLint location, GLint* params) Func(glGetUniformiv, (program, location, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetVertexAttribPointerv (GLuint index, GLenum pname, GLvoid** pointer) Func(glGetVertexAttribPointerv, (index, pname, pointer))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetVertexAttribfv (GLuint index, GLenum pname, GLfloat* params) Func(glGetVertexAttribfv, (index, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetVertexAttribiv (GLuint index, GLenum pname, GLint* params) Func(glGetVertexAttribiv, (index, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glHint (GLenum target, GLenum mode) Func(glHint, (target, mode))
DLLEXPORT bool DLLEXPORTENTRY remote_glLightModelf (GLenum pname, GLfloat param) Func(glLightModelf, (pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glLightModelfv (GLenum pname, const GLfloat *params) 
   FuncExtra1(glLightModelfv, (pname, params), ((void*)params, (pname == GL_LIGHT_MODEL_AMBIENT ? 4 : 1) * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glLightModelx (GLenum pname, GLfixed param) Func(glLightModelx, (pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glLightModelxv (GLenum pname, const GLfixed *params)
   FuncExtra1(glLightModelxv, (pname, params), ((void*)params, (pname == GL_LIGHT_MODEL_AMBIENT ? 4 : 1) * sizeof(GLfixed)))
DLLEXPORT bool DLLEXPORTENTRY remote_glLightf (GLenum light, GLenum pname, GLfloat param) Func(glLightf, (light, pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glLightfv (GLenum light, GLenum pname, const GLfloat *params) 
   FuncExtra1(glLightfv, (light, pname, params), ((void*)params, LightParamsSize(pname) * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glLightx (GLenum light, GLenum pname, GLfixed param) Func(glLightx, (light, pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glLightxv (GLenum light, GLenum pname, const GLfixed *params)
   FuncExtra1(glLightxv, (light, pname, params), ((void*)params, LightParamsSize(pname) * sizeof(GLfixed)))
DLLEXPORT bool DLLEXPORTENTRY remote_glLineWidth (GLfloat width) Func(glLineWidth, (width))
DLLEXPORT bool DLLEXPORTENTRY remote_glLineWidthx (GLfixed width) Func(glLineWidthx, (width))
DLLEXPORT bool DLLEXPORTENTRY remote_glLinkProgram (GLuint program) Func(glLinkProgram, (program))
DLLEXPORT bool DLLEXPORTENTRY remote_glLoadIdentity (void) Func(glLoadIdentity, ())
DLLEXPORT bool DLLEXPORTENTRY remote_glLoadMatrixf (const GLfloat *m) 
   FuncExtra1(glLoadMatrixf, (m), ((void*)m, 16 * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glLoadMatrixx (const GLfixed *m)
   FuncExtra1(glLoadMatrixx, (m), ((void*)m, 16 * sizeof(GLfixed)))
DLLEXPORT bool DLLEXPORTENTRY remote_glLogicOp (GLenum opcode) Func(glLogicOp, ( opcode))
DLLEXPORT bool DLLEXPORTENTRY remote_glMaterialf (GLenum face, GLenum pname, GLfloat param) Func(glMaterialf, (face, pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glMaterialfv (GLenum face, GLenum pname, const GLfloat *params) 
   FuncExtra1(glMaterialfv, (face, pname, params), ((void*)params, MaterialParamsSize(pname) * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glMaterialx (GLenum face, GLenum pname, GLfixed param) Func(glMaterialx, (face, pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glMaterialxv (GLenum face, GLenum pname, const GLfixed *params)
   FuncExtra1(glMaterialxv, (face, pname, params), ((void*)params, MaterialParamsSize(pname) * sizeof(GLfixed)))
DLLEXPORT bool DLLEXPORTENTRY remote_glMatrixMode (GLenum mode) Func(glMatrixMode, (mode))
DLLEXPORT bool DLLEXPORTENTRY remote_glMultMatrixf (const GLfloat *m) 
   FuncExtra1(glMultMatrixf, (m), ((void*)m, 16 * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glMultMatrixx (const GLfixed *m) 
   FuncExtra1(glMultMatrixx, (m), ((void*)m, 16 * sizeof(GLfixed)))
DLLEXPORT bool DLLEXPORTENTRY remote_glMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) Func(glMultiTexCoord4f, (target, s, t, r, q))
DLLEXPORT bool DLLEXPORTENTRY remote_glMultiTexCoord4x (GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q) Func(glMultiTexCoord4x, (target, s, t, r, q))
DLLEXPORT bool DLLEXPORTENTRY remote_glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz) Func(glNormal3f, (nx, ny, nz))
DLLEXPORT bool DLLEXPORTENTRY remote_glNormal3x (GLfixed nx, GLfixed ny, GLfixed nz) Func(glNormal3x, (nx, ny, nz))
DLLEXPORT bool DLLEXPORTENTRY remote_glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer) 
{
   bool ret = true;
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eAPI_FUNCTION);
      sCurPacket = &p;
      p.AddItem(cmd_glNormalPointer);
      Packetize(type, stride, pointer);

      if (sCaptureStream)
      {
         GLint v[4];
         Real(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, v);
         Packetize(v[0] != 0);
      }

      ret = PostPacketize(&p);
   }
   return ret;
}
DLLEXPORT bool DLLEXPORTENTRY remote_glOrthof (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) Func(glOrthof, (left, right, bottom, top, zNear, zFar))
DLLEXPORT bool DLLEXPORTENTRY remote_glOrthox (GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar) Func(glOrthox, (left, right, bottom, top, zNear, zFar))
DLLEXPORT bool DLLEXPORTENTRY remote_glPixelStorei (GLenum pname, GLint param) Func(glPixelStorei, (pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glPointParameterf (GLenum pname, GLfloat param) Func(glPointParameterf, (pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glPointParameterfv (GLenum pname, const GLfloat *params) 
   FuncExtra1(glPointParameterfv, (pname, params), ((void*)params, (pname == GL_POINT_DISTANCE_ATTENUATION ? 3 : 1) * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glPointParameterx (GLenum pname, GLfixed param) Func(glPointParameterx, (pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glPointParameterxv (GLenum pname, const GLfixed *params)
   FuncExtra1(glPointParameterxv, (pname, params), ((void*)params, (pname == GL_POINT_DISTANCE_ATTENUATION ? 3 : 1) * sizeof(GLfixed)))
DLLEXPORT bool DLLEXPORTENTRY remote_glPointSize (GLfloat size) Func(glPointSize, (size))
DLLEXPORT bool DLLEXPORTENTRY remote_glPointSizex (GLfixed size) Func(glPointSizex, (size))
DLLEXPORT bool DLLEXPORTENTRY remote_glPolygonOffset (GLfloat factor, GLfloat units) Func(glPolygonOffset, (factor, units))
DLLEXPORT bool DLLEXPORTENTRY remote_glPolygonOffsetx (GLfixed factor, GLfixed units) Func(glPolygonOffsetx, (factor, units))
DLLEXPORT bool DLLEXPORTENTRY remote_glPopMatrix (void) Func(glPopMatrix, ())
DLLEXPORT bool DLLEXPORTENTRY remote_glPushMatrix (void) Func(glPushMatrix, ())
DLLEXPORT bool DLLEXPORTENTRY remote_glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels) Func(glReadPixels, (x, y, width, height, format, type, pixels))
DLLEXPORT bool DLLEXPORTENTRY remote_glReleaseShaderCompiler (void) Func(glReleaseShaderCompiler, ())
DLLEXPORT bool DLLEXPORTENTRY remote_glRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height) Func(glRenderbufferStorage, (target, internalformat, width, height))
DLLEXPORT bool DLLEXPORTENTRY remote_glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z) Func(glRotatef, (angle, x, y, z))
DLLEXPORT bool DLLEXPORTENTRY remote_glRotatex (GLfixed angle, GLfixed x, GLfixed y, GLfixed z) Func(glRotatex, (angle, x, y, z))
DLLEXPORT bool DLLEXPORTENTRY remote_glSampleCoverage (GLclampf value, GLboolean invert) Func(glSampleCoverage, (value, invert))
DLLEXPORT bool DLLEXPORTENTRY remote_glSampleCoveragex (GLclampx value, GLboolean invert) Func(glSampleCoveragex, (value, invert))
DLLEXPORT bool DLLEXPORTENTRY remote_glScalef (GLfloat x, GLfloat y, GLfloat z) Func(glScalef, (x, y, z))
DLLEXPORT bool DLLEXPORTENTRY remote_glScalex (GLfixed x, GLfixed y, GLfixed z) Func(glScalex, (x, y, z))
DLLEXPORT bool DLLEXPORTENTRY remote_glScissor (GLint x, GLint y, GLsizei width, GLsizei height) Func(glScissor, (x, y, width, height))
DLLEXPORT bool DLLEXPORTENTRY remote_glShadeModel (GLenum mode) Func(glShadeModel, (mode))
DLLEXPORT bool DLLEXPORTENTRY remote_glShaderBinary (GLsizei n, const GLuint* shaders, GLenum binaryformat, const GLvoid* binary, GLsizei length) Func(glShaderBinary, (n, shaders, binaryformat, binary, length))
DLLEXPORT bool DLLEXPORTENTRY remote_glShaderSource (GLuint shader, GLsizei count, const GLchar** string, const GLint* length) 
{
   bool ret = true;
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eAPI_FUNCTION);
      int32_t  i;

      sCurPacket = &p;
      p.AddItem(cmd_glShaderSource);
      Packetize(shader, count, string, length);

      if (length == NULL)
      {
         // Strings are NULL terminated
         for (i = 0; i < count; i++)
            sCurPacket->AddItem(PacketItem((void*)string[i], strlen(string[i])));
      }
      else
      {
         for (i = 0; i < count; i++)
         {
            int32_t len = length[i];
            if (len > 0)
               sCurPacket->AddItem(PacketItem((void*)string[i], len));
            else
               sCurPacket->AddItem(PacketItem((void*)string[i], strlen(string[i])));
         }
      }

      ret = PostPacketize(&p);
   }
   return ret;
}
DLLEXPORT bool DLLEXPORTENTRY remote_glStencilFunc (GLenum func, GLint ref, GLuint mask) Func(glStencilFunc, (func, ref, mask))
DLLEXPORT bool DLLEXPORTENTRY remote_glStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask) Func(glStencilFuncSeparate, (face, func, ref, mask))
DLLEXPORT bool DLLEXPORTENTRY remote_glStencilMask (GLuint mask) Func(glStencilMask, (mask))
DLLEXPORT bool DLLEXPORTENTRY remote_glStencilMaskSeparate (GLenum face, GLuint mask) Func(glStencilMaskSeparate, (face, mask))
DLLEXPORT bool DLLEXPORTENTRY remote_glStencilOp (GLenum fail, GLenum zfail, GLenum zpass) Func(glStencilOp, (fail, zfail, zpass))
DLLEXPORT bool DLLEXPORTENTRY remote_glStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass) Func(glStencilOpSeparate, (face, fail, zfail, zpass))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) 
{
   bool ret = true;
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eAPI_FUNCTION);
      sCurPacket = &p;
      p.AddItem(cmd_glTexCoordPointer);
      Packetize(size, type, stride, pointer);

      if (sCaptureStream)
      {
         GLint v[4];
         Real(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, v);
         Packetize(v[0] != 0);
      }

      ret = PostPacketize(&p);
   }
   return ret;
}
DLLEXPORT bool DLLEXPORTENTRY remote_glTexEnvf (GLenum target, GLenum pname, GLfloat param) Func(glTexEnvf, (target, pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params) 
   FuncExtra1(glTexEnvfv, (target, pname, params), ((void*)params, (pname == GL_TEXTURE_ENV_COLOR ? 4 : 1) * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexEnvi (GLenum target, GLenum pname, GLint param) Func(glTexEnvi, (target, pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexEnviv (GLenum target, GLenum pname, const GLint *params)
   FuncExtra1(glTexEnviv, (target, pname, params), ((void*)params, (pname == GL_TEXTURE_ENV_COLOR ? 4 : 1) * sizeof(GLint)))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexEnvx (GLenum target, GLenum pname, GLfixed param) Func(glTexEnvx, (target, pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexEnvxv (GLenum target, GLenum pname, const GLfixed *params)
   FuncExtra1(glTexEnvxv, (target, pname, params), ((void*)params, (pname == GL_TEXTURE_ENV_COLOR ? 4 : 1) * sizeof(GLfixed)))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) 
   FuncExtra1(glTexImage2D, (target, level, internalformat, width, height, border, format, type, pixels),
                             ((void*)pixels, TextureSize(width, height, format, type, UnpackAlignment())))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexParameterf (GLenum target, GLenum pname, GLfloat param) Func(glTexParameterf, (target, pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
   FuncExtra1(glTexParameterfv, (target, pname, params), ((void*)params, sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexParameteri (GLenum target, GLenum pname, GLint param) Func(glTexParameteri, (target, pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexParameteriv (GLenum target, GLenum pname, const GLint *params) 
   FuncExtra1(glTexParameteriv, (target, pname, params), ((void*)params, sizeof(GLint)))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexParameterx (GLenum target, GLenum pname, GLfixed param) Func(glTexParameterx, (target, pname, param))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexParameterxv (GLenum target, GLenum pname, const GLfixed *params) 
   FuncExtra1(glTexParameterxv, (target, pname, params), ((void*)params, sizeof(GLfixed)))
DLLEXPORT bool DLLEXPORTENTRY remote_glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels) 
   FuncExtra1(glTexSubImage2D, (target, level, xoffset, yoffset, width, height, format, type, pixels),
                               ((void*)pixels, TextureSize(width, height, format, type, UnpackAlignment())))
DLLEXPORT bool DLLEXPORTENTRY remote_glTranslatef (GLfloat x, GLfloat y, GLfloat z) Func(glTranslatef, (x, y, z))
DLLEXPORT bool DLLEXPORTENTRY remote_glTranslatex (GLfixed x, GLfixed y, GLfixed z) Func(glTranslatex, (x, y, z))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform1f (GLint location, GLfloat x) Func(glUniform1f, (location, x))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform1fv (GLint location, GLsizei count, const GLfloat* v) 
   FuncExtra1(glUniform1fv, (location, count, v), ((void*)v, count * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform1i (GLint location, GLint x) Func(glUniform1i, (location, x))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform1iv (GLint location, GLsizei count, const GLint* v) 
   FuncExtra1(glUniform1iv, (location, count, v), ((void*)v, count * sizeof(GLint)))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform2f (GLint location, GLfloat x, GLfloat y) Func(glUniform2f, (location, x, y))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform2fv (GLint location, GLsizei count, const GLfloat* v) 
   FuncExtra1(glUniform2fv, (location, count, v), ((void*)v, 2 * count * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform2i (GLint location, GLint x, GLint y) Func(glUniform2i, (location, x, y))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform2iv (GLint location, GLsizei count, const GLint* v) 
   FuncExtra1(glUniform2iv, (location, count, v), ((void*)v, 2 * count * sizeof(GLint)))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform3f (GLint location, GLfloat x, GLfloat y, GLfloat z) Func(glUniform3f, (location, x, y, z))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform3fv (GLint location, GLsizei count, const GLfloat* v) 
   FuncExtra1(glUniform3fv, (location, count, v), ((void*)v, 3 * count * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform3i (GLint location, GLint x, GLint y, GLint z) Func(glUniform3i, (location, x, y, z))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform3iv (GLint location, GLsizei count, const GLint* v) 
   FuncExtra1(glUniform3iv, (location, count, v), ((void*)v, 3 * count * sizeof(GLint)))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform4f (GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) Func(glUniform4f, (location, x, y, z, w))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform4fv (GLint location, GLsizei count, const GLfloat* v) 
   FuncExtra1(glUniform4fv, (location, count, v), ((void*)v, 4 * count * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform4i (GLint location, GLint x, GLint y, GLint z, GLint w) Func(glUniform4i, (location, x, y, z, w))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniform4iv (GLint location, GLsizei count, const GLint* v) 
   FuncExtra1(glUniform4iv, (location, count, v), ((void*)v, 4 * count * sizeof(GLint)))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) 
   FuncExtra1(glUniformMatrix2fv, (location, count, transpose, value), ((void*)value, 4 * count * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) 
   FuncExtra1(glUniformMatrix3fv, (location, count, transpose, value), ((void*)value, 9 * count * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value) 
   FuncExtra1(glUniformMatrix4fv, (location, count, transpose, value), ((void*)value, 16 * count * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glUseProgram (GLuint program) Func(glUseProgram, (program))
DLLEXPORT bool DLLEXPORTENTRY remote_glValidateProgram (GLuint program) Func(glValidateProgram, (program))
DLLEXPORT bool DLLEXPORTENTRY remote_glVertexAttrib1f (GLuint indx, GLfloat x) Func(glVertexAttrib1f, (indx, x))
DLLEXPORT bool DLLEXPORTENTRY remote_glVertexAttrib1fv (GLuint indx, const GLfloat* values) 
   FuncExtra1(glVertexAttrib1fv, (indx, values), ((void*)values, 1 * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glVertexAttrib2f (GLuint indx, GLfloat x, GLfloat y) Func(glVertexAttrib2f, (indx, x, y))
DLLEXPORT bool DLLEXPORTENTRY remote_glVertexAttrib2fv (GLuint indx, const GLfloat* values)
   FuncExtra1(glVertexAttrib2fv, (indx, values), ((void*)values, 2 * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glVertexAttrib3f (GLuint indx, GLfloat x, GLfloat y, GLfloat z) Func(glVertexAttrib3f, (indx, x, y, z))
DLLEXPORT bool DLLEXPORTENTRY remote_glVertexAttrib3fv (GLuint indx, const GLfloat* values)
   FuncExtra1(glVertexAttrib3fv, (indx, values), ((void*)values, 3 * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glVertexAttrib4f (GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w) Func(glVertexAttrib4f, (indx, x, y, z, w))
DLLEXPORT bool DLLEXPORTENTRY remote_glVertexAttrib4fv (GLuint indx, const GLfloat* values) 
   FuncExtra1(glVertexAttrib4fv, (indx, values), ((void*)values, 4 * sizeof(GLfloat)))
DLLEXPORT bool DLLEXPORTENTRY remote_glVertexAttribPointer (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr) 
{
   bool ret = true;
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eAPI_FUNCTION);
      sCurPacket = &p;
      p.AddItem(cmd_glVertexAttribPointer);
      Packetize(indx, size, type, normalized, stride, ptr);

      if (sCaptureStream)
      {
         GLint v[4];
         Real(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, v);
         Packetize(v[0] != 0);
      }

      ret = PostPacketize(&p);
   }
   return ret;
}
DLLEXPORT bool DLLEXPORTENTRY remote_glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) 
{
   bool ret = true;
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eAPI_FUNCTION);
      sCurPacket = &p;
      p.AddItem(cmd_glVertexPointer);
      Packetize(size, type, stride, pointer);

      if (sCaptureStream)
      {
         GLint v[4];
         Real(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, v);
         Packetize(v[0] != 0);
      }

      ret = PostPacketize(&p);
   }
   return ret;
}
DLLEXPORT bool DLLEXPORTENTRY remote_glViewport (GLint x, GLint y, GLsizei width, GLsizei height) Func(glViewport, (x, y, width, height))

/* GL Extensions */
DLLEXPORT bool DLLEXPORTENTRY remote_glEGLImageTargetTexture2DOES (GLenum target, GLeglImageOES image) Func(glEGLImageTargetTexture2DOES, (target, image))
DLLEXPORT bool DLLEXPORTENTRY remote_glEGLImageTargetRenderbufferStorageOES (GLenum target, GLeglImageOES image) Func(glEGLImageTargetRenderbufferStorageOES, (target, image))

DLLEXPORT bool DLLEXPORTENTRY remote_glDrawTexsOES (GLshort x, GLshort y, GLshort z, GLshort width, GLshort height) Func(glDrawTexsOES, (x, y, z, width, height))
DLLEXPORT bool DLLEXPORTENTRY remote_glDrawTexiOES (GLint x, GLint y, GLint z, GLint width, GLint height) Func(glDrawTexiOES, (x, y, z, width, height))
DLLEXPORT bool DLLEXPORTENTRY remote_glDrawTexxOES (GLfixed x, GLfixed y, GLfixed z, GLfixed width, GLfixed height) Func(glDrawTexxOES, (x, y, z, width, height))
DLLEXPORT bool DLLEXPORTENTRY remote_glDrawTexsvOES (const GLshort *coords) 
   FuncExtra1(glDrawTexsvOES, (coords), ((void*)coords, 5 * sizeof(GLshort)))
DLLEXPORT bool DLLEXPORTENTRY remote_glDrawTexivOES (const GLint *coords)
   FuncExtra1(glDrawTexivOES, (coords), ((void*)coords, 5 * sizeof(GLint)))
DLLEXPORT bool DLLEXPORTENTRY remote_glDrawTexxvOES (const GLfixed *coords)
   FuncExtra1(glDrawTexivOES, (coords), ((void*)coords, 5 * sizeof(GLfixed)))
DLLEXPORT bool DLLEXPORTENTRY remote_glDrawTexfOES (GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height) Func(glDrawTexfOES, (x, y, z, width, height))
DLLEXPORT bool DLLEXPORTENTRY remote_glDrawTexfvOES (const GLfloat *coords)
   FuncExtra1(glDrawTexfvOES, (coords), ((void*)coords, 5 * sizeof(GLfloat)))

DLLEXPORT bool DLLEXPORTENTRY remote_glIsRenderbufferOES (GLuint renderbuffer) Func(glIsRenderbufferOES, (renderbuffer))
DLLEXPORT bool DLLEXPORTENTRY remote_glBindRenderbufferOES (GLenum target, GLuint renderbuffer) Func(glBindRenderbufferOES, (target, renderbuffer))
DLLEXPORT bool DLLEXPORTENTRY remote_glDeleteRenderbuffersOES (GLsizei n, const GLuint* renderbuffers) 
   FuncExtra1(glDeleteRenderbuffersOES, (n, renderbuffers), ((void*)renderbuffers, n * sizeof(GLuint)))
DLLEXPORT bool DLLEXPORTENTRY remote_glGenRenderbuffersOES (GLsizei n, GLuint* renderbuffers) Func(glGenRenderbuffersOES, (n, renderbuffers))
DLLEXPORT bool DLLEXPORTENTRY remote_glRenderbufferStorageOES (GLenum target, GLenum internalformat, GLsizei width, GLsizei height) Func(glRenderbufferStorageOES, (target, internalformat, width, height))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetRenderbufferParameterivOES (GLenum target, GLenum pname, GLint* params) Func(glGetRenderbufferParameterivOES, (target, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glIsFramebufferOES (GLuint framebuffer) Func(glIsFramebufferOES, (framebuffer))
DLLEXPORT bool DLLEXPORTENTRY remote_glBindFramebufferOES (GLenum target, GLuint framebuffer) DebugModeOnlyFunc(glBindFramebufferOES, (target, framebuffer))
DLLEXPORT bool DLLEXPORTENTRY remote_glDeleteFramebuffersOES (GLsizei n, const GLuint* framebuffers) 
   FuncExtra1(glDeleteFramebuffersOES, (n, framebuffers), ((void*)framebuffers, n * sizeof(GLuint)))
DLLEXPORT bool DLLEXPORTENTRY remote_glGenFramebuffersOES (GLsizei n, GLuint* framebuffers) Func(glGenFramebuffersOES, (n, framebuffers))
DLLEXPORT bool DLLEXPORTENTRY remote_glCheckFramebufferStatusOES (GLenum target) Func(glCheckFramebufferStatusOES , (target))
DLLEXPORT bool DLLEXPORTENTRY remote_glFramebufferRenderbufferOES (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) Func(glFramebufferRenderbufferOES, (target, attachment, renderbuffertarget, renderbuffer))
DLLEXPORT bool DLLEXPORTENTRY remote_glFramebufferTexture2DOES (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) Func(glFramebufferTexture2DOES, (target, attachment, textarget, texture, level))
DLLEXPORT bool DLLEXPORTENTRY remote_glGetFramebufferAttachmentParameterivOES (GLenum target, GLenum attachment, GLenum pname, GLint* params) Func(glGetFramebufferAttachmentParameterivOES, (target, attachment, pname, params))
DLLEXPORT bool DLLEXPORTENTRY remote_glGenerateMipmapOES (GLenum target) Func(glGenerateMipmapOES, (target))

DLLEXPORT bool DLLEXPORTENTRY remote_glDiscardFramebufferEXT (GLenum target, GLsizei numAttachments, const GLenum *attachments) 
   FuncExtra1(glDiscardFramebufferEXT, (target, numAttachments, attachments), ((void*)attachments, numAttachments * sizeof(GLenum)))
DLLEXPORT bool DLLEXPORTENTRY remote_glInsertEventMarkerEXT(GLsizei length, const GLchar *marker) DebugModeOnlyFunc(glInsertEventMarkerEXT, (length, marker))
DLLEXPORT bool DLLEXPORTENTRY remote_glPushGroupMarkerEXT(GLsizei length, const GLchar *marker) DebugModeOnlyFunc(glPushGroupMarkerEXT, (length, marker))
DLLEXPORT bool DLLEXPORTENTRY remote_glPopGroupMarkerEXT(void) DebugModeOnlyFunc(glPopGroupMarkerEXT, ())

/*
DLLEXPORT GLbitfield DLLEXPORTENTRY remote_glQueryMatrixxOES (GLfixed mantissa[16], GLint exponent[16]) GLbitfieldFunc(glQueryMatrixxOES , (mantissa, exponent))
*/
DLLEXPORT bool DLLEXPORTENTRY remote_glPointSizePointerOES (GLenum type, GLsizei stride, const GLvoid *pointer) 
{
   bool ret = true;
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eAPI_FUNCTION);
      sCurPacket = &p;
      p.AddItem(cmd_glPointSizePointerOES);
      Packetize(type, stride, pointer);

      if (sCaptureStream)
      {
         GLint v[4];
         Real(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, v);
         Packetize(v[0] != 0);
      }

      ret = PostPacketize(&p);
   }
   return ret;
}

/* EGL */
DLLEXPORT bool EGLAPIENTRY remote_eglGetError(void) Func(eglGetError, ())
DLLEXPORT bool EGLAPIENTRY remote_eglGetDisplay(EGLNativeDisplayType display_id) Func(eglGetDisplay, (display_id))
DLLEXPORT bool EGLAPIENTRY remote_eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor) Func(eglInitialize, (dpy, major, minor))
DLLEXPORT bool EGLAPIENTRY remote_eglTerminate(EGLDisplay dpy) Func(eglTerminate, (dpy))
DLLEXPORT bool EGLAPIENTRY remote_eglQueryString(EGLDisplay dpy, EGLint name) Func(eglQueryString, (dpy, name))
DLLEXPORT bool EGLAPIENTRY remote_eglGetConfigs(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config) 
   FuncExtra1(eglGetConfigs, (dpy, configs, config_size, num_config), 
                             (configs, config_size * sizeof(EGLConfig)))
DLLEXPORT bool EGLAPIENTRY remote_eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
   FuncExtra2(eglChooseConfig, (dpy, attrib_list, configs, config_size, num_config), 
                               ((void*)attrib_list, AttribListSize(attrib_list)), 
                               (configs, config_size * sizeof(EGLConfig)))
DLLEXPORT bool EGLAPIENTRY remote_eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value) Func(eglGetConfigAttrib, (dpy, config, attribute, value))
DLLEXPORT bool EGLAPIENTRY remote_eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list) 
   FuncExtra1(eglCreateWindowSurface, (dpy, config, win, attrib_list),
                                      ((void*)attrib_list, AttribListSize(attrib_list)))
DLLEXPORT bool EGLAPIENTRY remote_eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list) 
   FuncExtra1(eglCreatePbufferSurface, (dpy, config, attrib_list), ((void*)attrib_list, AttribListSize(attrib_list)))
DLLEXPORT bool EGLAPIENTRY remote_eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list)
   FuncExtra1(eglCreatePixmapSurface, (dpy, config, pixmap, attrib_list), ((void*)attrib_list, AttribListSize(attrib_list)))
DLLEXPORT bool EGLAPIENTRY remote_eglDestroySurface(EGLDisplay dpy, EGLSurface surface) Func(eglDestroySurface, (dpy, surface))
DLLEXPORT bool EGLAPIENTRY remote_eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value) Func(eglQuerySurface, (dpy, surface, attribute, value))
DLLEXPORT bool EGLAPIENTRY remote_eglBindAPI(EGLenum api) DebugModeOnlyFunc(eglBindAPI, (api))
DLLEXPORT bool EGLAPIENTRY remote_eglQueryAPI(void) Func(eglQueryAPI, ())
DLLEXPORT bool EGLAPIENTRY remote_eglWaitClient(void) DebugModeOnlyFunc(eglWaitClient, ())
DLLEXPORT bool EGLAPIENTRY remote_eglReleaseThread(void) Func(eglReleaseThread, ())
DLLEXPORT bool EGLAPIENTRY remote_eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint *attrib_list) 
   FuncExtra1(eglCreatePbufferFromClientBuffer, (dpy, buftype, buffer, config, attrib_list),
                                                ((void*)attrib_list, AttribListSize(attrib_list)))
DLLEXPORT bool EGLAPIENTRY remote_eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value) Func(eglSurfaceAttrib, (dpy, surface, attribute, value))
DLLEXPORT bool EGLAPIENTRY remote_eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer) Func(eglBindTexImage, (dpy, surface, buffer))
DLLEXPORT bool EGLAPIENTRY remote_eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer) Func(eglReleaseTexImage, (dpy, surface, buffer))
DLLEXPORT bool EGLAPIENTRY remote_eglSwapInterval(EGLDisplay dpy, EGLint interval) Func(eglSwapInterval, (dpy, interval))
DLLEXPORT bool EGLAPIENTRY remote_eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list)
{
   bool ret = true;
   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eAPI_FUNCTION);
      sCurPacket = &p;
      p.AddItem(cmd_eglCreateContext);
      Packetize(dpy, config, share_context, attrib_list);

      const EGLint *att_list = attrib_list;
      EGLint value = 1;

      if (att_list)
      {
         if (*att_list++ == EGL_CONTEXT_CLIENT_VERSION)
            value = *att_list;
      }
      sCurPacket->AddItem(value);
      sCurPacket->AddItem(PacketItem((void*)attrib_list, AttribListSize(attrib_list)));
      ret = PostPacketize(&p);
   }
   return ret;
}
DLLEXPORT bool EGLAPIENTRY remote_eglDestroyContext(EGLDisplay dpy, EGLContext ctx) Func(eglDestroyContext, (dpy, ctx))
DLLEXPORT bool EGLAPIENTRY remote_eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
   bool ret = true;

   if (!sOrphaned && sBottleneckMode == Control::eUnset)
   {
      Packet    p(eAPI_FUNCTION);
      EGLConfig configId;
      EGLint    depthSize, stencilSize;

      sCurPacket = &p;
      p.AddItem(cmd_eglMakeCurrent);
      Packetize(dpy, draw, read, ctx);

      // Add extra data for the config
      Real(eglQueryContext)(dpy, ctx, EGL_CONFIG_ID, (EGLint*)&configId);
      Real(eglGetConfigAttrib)(dpy, configId, EGL_DEPTH_SIZE, &depthSize);
      Real(eglGetConfigAttrib)(dpy, configId, EGL_STENCIL_SIZE, &stencilSize);
      sCurPacket->AddItem(depthSize);
      sCurPacket->AddItem(stencilSize);

      ret = PostPacketize(&p);
      sSendNextRet = true;
   }
   return ret;
}
DLLEXPORT bool EGLAPIENTRY remote_eglGetCurrentContext(void) Func(eglGetCurrentContext, ())
DLLEXPORT bool EGLAPIENTRY remote_eglGetCurrentSurface(EGLint readdraw) Func(eglGetCurrentSurface, (readdraw))
DLLEXPORT bool EGLAPIENTRY remote_eglGetCurrentDisplay(void) Func(eglGetCurrentDisplay, ())
DLLEXPORT bool EGLAPIENTRY remote_eglQueryContext(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value) Func(eglQueryContext, (dpy, ctx, attribute, value))
DLLEXPORT bool EGLAPIENTRY remote_eglWaitGL(void) AlwaysFunc(eglWaitGL, ())
DLLEXPORT bool EGLAPIENTRY remote_eglWaitNative(EGLint engine) AlwaysFunc(eglWaitNative, (engine))
DLLEXPORT bool EGLAPIENTRY remote_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
   bool ret = true;

   if (!sOrphaned)
   {
      Packet   p(eAPI_FUNCTION);
      sCurPacket = &p;
      p.AddItem(cmd_eglSwapBuffers);
      Packetize(dpy, surface);
      ret = PostPacketize(&p);
      sSendNextRet = true;
   }
   return ret;
}
DLLEXPORT bool EGLAPIENTRY remote_eglCopyBuffers(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target) Func(eglCopyBuffers, (dpy, surface, target))
DLLEXPORT bool EGLAPIENTRY remote_eglLockSurfaceKHR(EGLDisplay dpy, EGLSurface surface, const EGLint *attrib_list) 
   FuncExtra1(eglLockSurfaceKHR, (dpy, surface, attrib_list), ((void*)attrib_list, AttribListSize(attrib_list)))
DLLEXPORT bool EGLAPIENTRY remote_eglUnlockSurfaceKHR(EGLDisplay dpy, EGLSurface surface) Func(eglUnlockSurfaceKHR, (dpy, surface))
DLLEXPORT bool EGLAPIENTRY remote_eglCreateImageKHR(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list) 
   FuncExtra1(eglCreateImageKHR, (dpy, ctx, target, buffer, attrib_list), ((void*)attrib_list, AttribListSize(attrib_list)))
   // TODO : do something about client buffer
DLLEXPORT bool EGLAPIENTRY remote_eglDestroyImageKHR(EGLDisplay dpy, EGLImageKHR image) Func(eglDestroyImageKHR, (dpy, image))
DLLEXPORT bool EGLAPIENTRY remote_eglCreateSyncKHR(EGLDisplay dpy, EGLenum type, const EGLint *attrib_list) 
   FuncExtra1(eglCreateSyncKHR, (dpy, type, attrib_list), ((void*)attrib_list, AttribListSize(attrib_list)))
DLLEXPORT bool EGLAPIENTRY remote_eglDestroySyncKHR(EGLDisplay dpy, EGLSyncKHR sync) Func(eglDestroySyncKHR, (dpy, sync))
DLLEXPORT bool EGLAPIENTRY remote_eglClientWaitSyncKHR(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout) 
{
   bool ret = true;

   if (!sOrphaned && sEnabled && sBottleneckMode == Control::eUnset)
   {
      Packet   p(eAPI_FUNCTION);
      sCurPacket = &p;
      p.AddItem(cmd_eglClientWaitSyncKHR);
      Packetize(dpy, sync, flags, (uint32_t)timeout);
      ret = PostPacketize(&p);
   }
   return ret;
}
DLLEXPORT bool EGLAPIENTRY remote_eglSignalSyncKHR(EGLDisplay dpy, EGLSyncKHR sync, EGLenum mode) Func(eglSignalSyncKHR, (dpy, sync, mode))
DLLEXPORT bool EGLAPIENTRY remote_eglGetSyncAttribKHR(EGLDisplay dpy, EGLSyncKHR sync, EGLint attribute, EGLint *value) Func(eglGetSyncAttribKHR, (dpy, sync, attribute, value))

DLLEXPORT bool EGLAPIENTRY remote_eglImageUpdateParameterivBRCM(EGLDisplay dpy, EGLImageKHR image, EGLenum pname, const EGLint *params) 
   FuncExtra1(eglImageUpdateParameterivBRCM, (dpy, image, pname, params), 
              ((void*)params, (pname == EGL_IMAGE_UPDATE_CONTROL_CHANGED_REGION_BRCM ? 4 : 1) * sizeof(EGLint)))
DLLEXPORT bool EGLAPIENTRY remote_eglImageUpdateParameteriBRCM(EGLDisplay dpy, EGLImageKHR image, EGLenum pname, EGLint param) Func(eglImageUpdateParameteriBRCM, (dpy, image, pname, param))


}
