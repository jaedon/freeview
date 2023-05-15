/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  adaptation.h
	@brief	  MHEG Engine전용 adaptation header

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __MHEG_ENGINE_ADAPTATION_H__
#define __MHEG_ENGINE_ADAPTATION_H__

/*--------------------*/
/*--- 외부 headers ---*/
/*--------------------*/

#if 0
// OCTO platform common header
#include <octo_common.h>

// OCTO pal layer headers
#include <pal_gfx.h>
#include <pal_font.h>
#include <pal_plane.h>

// OCTO mw layer headers
#include <osd.h>
#include <osd_gfx.h>
#include <osd_font.h>

#include <svc_av.h>

#include <mwc_rm.h>
#include <mwc_util.h>

// OCTO gwm headers
#include <gwm.h>

// OCTO app layer headers
#include <app.h>

// CI+ common header
#include <ci_plus.h>
#endif

#include <apk.h>

/*-------------*/
/*--- WIN32 ---*/
/*-------------*/

// <stdbool.h>, <stdint.h> 등 대체

#if defined(WIN32)

// PATH_MAX
#ifndef PATH_MAX
#define PATH_MAX		256
#endif

// timeval
typedef struct timeval_win
{
	unsigned int		tv_sec;
	unsigned int		tv_usec;
};
#ifndef timeval
#define timeval 		timeval_win
#endif

// timezone
typedef struct timezone_win
{
	int		tz_minuteswest;
	int		tz_dsttime;
};
#ifndef timezone
#define timezone 		timezone_win
#endif

// bool
#ifndef false
#define false			FALSE
#endif
#ifndef true
#define true			TRUE
#endif

// int type
typedef unsigned char			uint8_t;
typedef signed short			int16_t;
typedef unsigned short			uint16_t;
typedef unsigned int			uint32_t;

// pi value.. 임시.. math.h에 들어 있지만 왠지 안 불러진다 -_-
#ifndef M_PI
#define M_PI	3.141592
#endif

// string
#define	 snprintf	_snprintf
#define	 vsnprintf	_vsnprintf

// network
typedef uint16_t			in_port_t;
typedef uint32_t			in_addr_t;
typedef unsigned short  		sa_family_t;
struct in_addr {
in_addr_t s_addr; /* IPv4 주소(uint32_t) */
};
struct sockaddr_in
{
sa_family_t sin_family; /* AF_INET */
in_port_t sin_port;
struct in_addr sin_addr; /* IPv4 주소 */
};
struct hostent
{
	char *h_name;                 /* Official name of host.  */
	char **h_aliases;             /* Alias list.  */
	int h_addrtype;               /* Host address type.  */
	int h_length;                 /* Length of address.  */
	char **h_addr_list;           /* List of addresses from name server.  */
	#define h_addr  h_addr_list[0]  /* Address, for backward compatibility.  */
};
#define AF_INET         2
#define INADDR_LOOPBACK         0x7f000001
#define SOCK_STREAM = 1;

// linux wrapping function
int gettimeofday(struct timeval *tv, struct timezone *tz);
uint16_t htons(
uint16_t hostnum /* 호스트 바이트 순서인 16비트 숫자 */
);
uint32_t htonl(
uint32_t hostnum /* 호스트 바이트 순서인 32비트 숫자 */
);

#endif	/* #if defined(WIN32) */

/*-----------------------------------------------------*/
/*--- 외부 definitions (TODO : 추후 삭제/변경 필요) ---*/
/*-----------------------------------------------------*/

// from "Render.h"

typedef unsigned long			Picture;
typedef unsigned long			PictFormat;

#define PictOpOver			    3

#define CPRepeat			    (1 << 0)

// from "X.h"

typedef unsigned long			XID;
typedef unsigned long			VisualID;
typedef unsigned long			Time;

typedef XID					Window;
typedef XID					Drawable;
typedef XID					_Font;
typedef XID					Pixmap;
typedef XID					Cursor;
typedef XID					Colormap;
typedef XID					GContext;

#ifndef None
#define None					0L	/* universal null resource or null atom */
#endif

#define InputOutput			1

#define Unsorted				0

#define CoordModeOrigin		0	/* relative to the origin */

#define Convex					2	/* wholly convex */

#define ArcPieSlice			1	/* join endpoints to center of arc */

#define GCForeground			(1L<<2)
#define GCLineWidth			(1L<<4)
#define GCArcMode				(1L<<22)

#define ZPixmap				2	/* depth == drawable depth */

// from "Xfuncproto.h"

#ifndef _Xconst
#define _Xconst const
#endif /* _Xconst */

// from "Xlib.h"

#define Bool int
#define True 1
#define False 0

typedef char *XPointer;

typedef struct _XExtData {
	int number;		/* number returned by XRegisterExtension */
	struct _XExtData *next;	/* next item on list of data for structure */
	int (*free_private)(	/* called to free private storage */
	struct _XExtData *extension
	);
	XPointer private_data;	/* data private to this extension. */
} XExtData;

typedef struct {
	int function;		/* logical operation */
	unsigned long plane_mask;/* plane mask */
	unsigned long foreground;/* foreground pixel */
	unsigned long background;/* background pixel */
	int line_width;		/* line width */
	int line_style;	 	/* LineSolid, LineOnOffDash, LineDoubleDash */
	int cap_style;	  	/* CapNotLast, CapButt,
				   CapRound, CapProjecting */
	int join_style;	 	/* JoinMiter, JoinRound, JoinBevel */
	int fill_style;	 	/* FillSolid, FillTiled,
				   FillStippled, FillOpaeueStippled */
	int fill_rule;	  	/* EvenOddRule, WindingRule */
	int arc_mode;		/* ArcChord, ArcPieSlice */
	Pixmap tile;		/* tile pixmap for tiling operations */
	Pixmap stipple;		/* stipple 1 plane pixmap for stipping */
	int ts_x_origin;	/* offset for tile or stipple operations */
	int ts_y_origin;
	_Font font;	        /* default text font for text operations */
	int subwindow_mode;     /* ClipByChildren, IncludeInferiors */
	Bool graphics_exposures;/* boolean, should exposures be generated */
	int clip_x_origin;	/* origin for clipping */
	int clip_y_origin;
	Pixmap clip_mask;	/* bitmap clipping; other calls for rects */
	int dash_offset;	/* patterned/dashed line information */
	char dashes;
} XGCValues;

typedef struct _XGC {
    XExtData *ext_data;	/* hook for extension to hang data */
    GContext gid;	/* protocol ID for graphics context */
    /* there is more to this structure, but it is private to Xlib */
} *GC;

typedef struct {
    short x, y;
} XPoint;

typedef struct {
    short x, y;
    unsigned short width, height;
} XRectangle;

/*------------------------------------*/
/*--- Dynamic line art 관련 함수들 ---*/
/*------------------------------------*/

Pixmap MHEGIB_CreatePixmap(
    Drawable             d ,
    unsigned int         width ,
    unsigned int         height ,
    unsigned int         depth
);


Picture
MHEGIB_RenderCreatePicture (Drawable d);

GC MHEGIB_CreateGC(
    Drawable             d ,
    unsigned long        valuemask ,
    XGCValues*           values
);

int MHEGIB_FreeGC(GC gc);

int MHEGIB_ChangeGC(
    GC                   gc ,
    unsigned long        valuemask ,
    XGCValues*           values
);

int MHEGIB_FillRectangle( /* MHEGIB_DrawRectangle(), MHEGIB_DrawRectFill() 와 다른 함수임에 유의할 것 */
    Drawable             d ,
    GC                   gc ,
    int                  x ,
    int                  y ,
    unsigned int         width ,
    unsigned int         height
);

int MHEGIB_DrawRectangle( /* MHEGIB_FillRectangle(), MHEGIB_DrawRectangle() 와 다른 함수임에 유의할 것 */
    Drawable             d ,
    GC                   gc ,
    int                  x ,
    int                  y ,
    unsigned int         width ,
    unsigned int         height
);

int MHEGIB_SetClipRectangles(
    GC                   gc ,
    int                  clip_x_origin ,
    int                  clip_y_origin ,
    XRectangle*          rectangles ,
    int                  n ,
    int                  ordering
);

int MHEGIB_FillArc(
    Drawable             d ,
    GC                   gc ,
    int                  x ,
    int                  y ,
    unsigned int         width ,
    unsigned int         height ,
    int                  angle1 ,
    int                  angle2
);

int MHEGIB_FillPolygon(
    Drawable             d ,
    GC                   gc ,
    XPoint*              points ,
    int                  npoints ,
    int                  shape ,
    int                  mode
);

int MHEGIB_DrawArc(
    Drawable             d ,
    GC                   gc ,
    int                  x ,
    int                  y ,
    unsigned int         width ,
    unsigned int         height ,
    int                  angle1 ,
    int                  angle2
);

int MHEGIB_DrawLine(
    Drawable             d ,
    GC                   gc ,
    int                  x1 ,
    int                  y1 ,
    int                  x2 ,
    int                  y2
);

int MHEGIB_DrawLines(
    Drawable             d ,
    GC                   gc ,
    XPoint*              points ,
    int                  npoints ,
    int                  mode
);

/*------------------------*/
/*--- type definitions ---*/
/*------------------------*/

#ifdef bool
#undef bool
#endif

#define	bool	HBOOL

#ifdef INLINE
#undef INLINE
#endif

#if defined(WIN32)
#define	INLINE	__inline
#else
#define	INLINE	inline
#endif

/*------------------------------------------------*/
/*--- 이하 OCTO OSD 관련 function, definitions ---*/
/*------------------------------------------------*/

/* resolution defined in UK MHEG Profile (D-BOOK 5.0) and CI+ MHEG Profile (CI+ spec) */
#define MHEG_XRES	720
#define MHEG_YRES	576

#define MHEG_BITMAP_SCALING_XRES_1	720
#define MHEG_BITMAP_SCALING_XRES_2	540
#define MHEG_BITMAP_SCALING_YRES	576

/* Default Full OSD Size - ref. OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT */
#define MHEG_FULL_XRES		(1280-1)
#define MHEG_FULL_YRES		(720-1)

// LG Font는 base line이 약간 쳐지므로, 아래 값으로 y좌표를 minus 보정하여 그린다
#define MHEG_FONT_ADJUST_Y		(1)

/* Colors */
#define MHEG_COLOR_BLACK		0xff000000
#define MHEG_COLOR_WHITE		0xffffffff
#define MHEG_COLOR_TRANS		0x00000000

typedef enum
{
	MHEG_MAINFRAME_ID = 0,
	MHEG_ID_MAX
} MHEGIB_IDs_e;

#define	C_MHEG_MAINFRAME_BG		MHEG_COLOR_TRANS

#if 0
/* message types for AP MHEG IB Manager, IB -> MhegIbManager */
enum
{
	MSG_MHEGIB_START = MSG_H_MHEG_START,

	MSG_MHEGIB_STREAM_SET_SPEED,
	MSG_MHEGIB_AUDIO_SET_VOLUME,

	MSG_MHEGIB_FINISH,
	MSG_MHEGIB_END
};
#endif

typedef enum
{
	eMhegIb_Ad_ReqType_File = 0x00,
	eMhegIb_Ad_ReqType_Data = 0x01
} eMhegIb_Ad_ReqType_t; // CI+ 스펙

typedef enum
{
	eMhegIb_Ad_AbortReqCode_UseCancel = 0x01,
	eMhegIb_Ad_AbortReqCode_SystemCancel = 0x02
} eMhegIb_Ad_AbortReqCode_t; // CI+ 스펙

/*******************************
  * extern fucntion proto type
 ********************************/

/**
  *		OCTO UI prototype
 **/
//#define	ERR_OK					0
//#define	ERR_FAIL				1

extern HERROR		MHEGIB_OSD_GetColor(int *out, unsigned char in_r, unsigned char in_g, unsigned char in_b, unsigned char in_t);
extern HERROR		MHEGIB_UpdateScreen(int x, int y, int w, int h);
extern HERROR		MHEGIB_DrawString(int orgX, int orgY, int color, int fontSize, bool tabs, int tabWidth, unsigned char *orgData, unsigned int orgSize, unsigned int hj, unsigned int vj);
extern HERROR		MHEGIB_DrawRectFill(int x, int y, int w, int h, int color);
extern HERROR		MHEGIB_DrawBitmap(int x, int y, int w, int h, unsigned char *pucImg, bool bEnableTiling);

/**
  *		OCTO Font prototype
 **/
extern HERROR		MHEGIB_GetCharWidth(int fontSize, int charData, int *pCharWidth);
extern HERROR		MHEGIB_GetFontHeight(int fontSize, unsigned int *pFontHeight);

/**
  *		OCTO AV prototype
 **/
extern HERROR		MHEGIB_PlayIFrame(int x, int y, int w, int h, unsigned char *pucBuf, int lBufSize);
extern HERROR		MHEGIB_StopIFrame(void);
extern HERROR		MHEGIB_Audio_SetVolume(HINT32 ulVol, HBOOL bIsAbsoluteValue);
extern HERROR		MHEGIB_Video_Freeze(void);
extern HERROR		MHEGIB_Video_Resume(void);
extern int 			MHEGIB_GetDisplayAspectRatio(void);

/**
  *		OCTO SI prototype
 **/
extern HBOOL			MHEGIB_GetCurTripleID(int *plOnID, int *plTsID, int *plSID);
extern HBOOL			MHEGIB_GetTripleIDbyLCN(int lcn, int *plOnID, int *plTsID, int *plSID);
extern HBOOL			MHEGIB_IsServiceAvailableByTripleID(unsigned int onid, unsigned int tsid, unsigned int sid);
extern HBOOL			MHEGIB_GetNetIdByTripleID(unsigned int onid, unsigned int tsid, unsigned int sid, unsigned int *pNid);

/**
  *		OCTO Proc prototype
 **/
extern void			MHEGIB_CreateAppl(void);
extern void			MHEGIB_DestroyAppl(void);
extern HBOOL			MHEGIB_CheckEngineStartCondition(void);

extern HERROR		MHEGIB_FileRequst(HUINT16 usSession, eMhegIb_Ad_ReqType_t eMhegIbReqType, HUINT8 *pucReqName, HUINT32 ulReqNameLen);
extern HBOOL			MHEGIB_AppAbort(HUINT16 usSession, eMhegIb_Ad_AbortReqCode_t eMhegIbAbortReqCode);
extern void			MHEGIB_CreateWindow(int argc, char **argv);


#endif /* __MHEG_ENGINE_ADAPTATION_H__ */

/** End of file */
