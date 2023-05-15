/*
 * videoout_null.c
 */

#include "MHEGEngine.h"
#include "MHEGVideoOutput.h"
#include "videoout_null.h"
#include "utils.h"

typedef struct
{
	GC gc;				/* GC to draw on the content Pixmap */
	unsigned int width;		/* size to output blank frame */
	unsigned int height;
} vo_null_ctx;

static void *vo_null_init(void);
static void vo_null_fini(void *);
static void vo_null_prepareFrame(void *, VideoFrame *, unsigned int, unsigned int);
static void vo_null_drawFrame(void *, int, int);

MHEGVideoOutputMethod vo_null_fns =
{
	vo_null_init,
	vo_null_fini,
	vo_null_prepareFrame,
	vo_null_drawFrame
};

static void *
vo_null_init(void)
{
	vo_null_ctx *v = safe_mallocz(sizeof(vo_null_ctx));
	MHEGDisplay *d = MHEGEngine_getDisplay();
	XGCValues gcvals;

//	gcvals.foreground = BlackPixel(d->dpy, DefaultScreen(d->dpy));
	v->gc = (GC)MHEGIB_CreateGC(d->contents, GCForeground, &gcvals);

	return v;
}

static void
vo_null_fini(void *ctx)
{
	vo_null_ctx *v = (vo_null_ctx *) ctx;
	MHEGDisplay *d = MHEGEngine_getDisplay();

	if(v->gc != None)
		MHEGIB_FreeGC(v->gc);

	safe_free(ctx);

	return;
}

static void
vo_null_prepareFrame(void *ctx, VideoFrame *f, unsigned int out_width, unsigned int out_height)
{
	vo_null_ctx *v = (vo_null_ctx *) ctx;

	v->width = out_width;
	v->height = out_height;

	return;
}

static void
vo_null_drawFrame(void *ctx, int x, int y)
{
	vo_null_ctx *v = (vo_null_ctx *) ctx;
	MHEGDisplay *d = MHEGEngine_getDisplay();

	/* draw an empty rectangle onto the Window contents Pixmap */
	MHEGIB_FillRectangle(d->contents, v->gc, x, y, v->width, v->height);

	/* get it drawn straight away */
//	XFlush(d->dpy);
	MHEGIB_UpdateScreen(0, 0, MHEG_XRES, MHEG_YRES);

	return;
}
