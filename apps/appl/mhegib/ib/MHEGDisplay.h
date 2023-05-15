/*
 * MHEGDisplay.h
 */

#ifndef __MHEGDISPLAY_H__
#define __MHEGDISPLAY_H__

#if !defined(WIN32)
#include <stdbool.h>
#endif

#include "MHEGColour.h"
#include "MHEGBitmap.h"

#if 0 /* resolution defined in UK MHEG Profile */
#define MHEG_XRES	720
#define MHEG_YRES	576
#endif

/* tab stops are this number of pixels apart */
#define MHEG_TAB_WIDTH	43	// org is 45 pixels

typedef struct
{
	Window win;				/* Window to display our Picture */
	unsigned int xres;			/* Window resolution in pixels */
	unsigned int yres;
	int depth;				/* colour depth of the Window */
	Pixmap contents;			/* current contents of the Window */
	Picture contents_pic;			/* XRender wrapper for the contents, this is what we composite on */
} MHEGDisplay;

int MHEGDisplay_init(MHEGDisplay *);
int MHEGDisplay_fini(MHEGDisplay *);

void MHEGDisplay_refresh(MHEGDisplay *, XYPosition *, OriginalBoxSize *);
void MHEGDisplay_clearScreen(MHEGDisplay *);

/* drawing routines */
void MHEGDisplay_setClipRectangle(MHEGDisplay *, XYPosition *, OriginalBoxSize *);
void MHEGDisplay_unsetClipRectangle(MHEGDisplay *);

void MHEGDisplay_drawHoriLine(MHEGDisplay *, XYPosition *, unsigned int, int, int, MHEGColour *);
void MHEGDisplay_drawVertLine(MHEGDisplay *, XYPosition *, unsigned int, int, int, MHEGColour *);
void MHEGDisplay_fillTransparentRectangle(MHEGDisplay *, XYPosition *, OriginalBoxSize *);
void MHEGDisplay_fillRectangle(MHEGDisplay *, XYPosition *, OriginalBoxSize *, MHEGColour *);
void MHEGDisplay_drawBitmap(MHEGDisplay *, XYPosition *, OriginalBoxSize *, MHEGBitmap *, XYPosition *, bool);
void MHEGDisplay_drawCanvas(MHEGDisplay *, XYPosition *, OriginalBoxSize *, MHEGCanvas *, XYPosition *);
void MHEGDisplay_drawTextElement(MHEGDisplay *, XYPosition *, MHEGFont *, MHEGTextElement *, bool, unsigned int , unsigned int );

void MHEGDisplay_useOverlay(MHEGDisplay *);

/* convert PNG and MPEG I-frames to internal format */
MHEGBitmap *MHEGDisplay_newPNGBitmap(MHEGDisplay *, OctetString *);
MHEGBitmap *MHEGDisplay_newMPEGBitmap(MHEGDisplay *, OctetString *);
void MHEGDisplay_freeBitmap(MHEGDisplay *, MHEGBitmap *);

/* utils */
bool intersects(XYPosition *, OriginalBoxSize *, XYPosition *, OriginalBoxSize *, XYPosition *, OriginalBoxSize *);

/* convert X/Y coords from MHEG resolution (0->720, 0->576) to output resolution */
#define MHEGDisplay_scaleX(DPY, X)	(((X) * (DPY)->xres) / MHEG_XRES)
#define MHEGDisplay_scaleY(DPY, Y)	(((Y) * (DPY)->yres) / MHEG_YRES)

#endif	/* __MHEGDISPLAY_H__ */

