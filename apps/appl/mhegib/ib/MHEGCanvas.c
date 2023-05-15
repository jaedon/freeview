/*
 * MHEGCanvas.c
 */

#include <math.h>

#include "MHEGEngine.h"
#include "MHEGCanvas.h"
#include "utils.h"

/* internal functions */

/*
 * convert the MHEGColour to a pixel value
 */

static unsigned long
pixel_value(MHEGColour *colour)
{
	unsigned long pixel;

	MHEGIB_OSD_GetColor((int*)&pixel, colour->r, colour->g, colour->b, colour->t);

	return pixel;
}

MHEGCanvas *
new_MHEGCanvas(unsigned int width, unsigned int height)
{
	MHEGCanvas *c = safe_mallocz(sizeof(MHEGCanvas));
	MHEGDisplay *d = MHEGEngine_getDisplay();

	/* scale width/height if fullscreen */
	c->width = MHEGDisplay_scaleX(d, width);
	c->height = MHEGDisplay_scaleY(d, height);

	/* no border set yet */
	c->border = 0;

	/* create a Pixmap to draw on */
	c->contents = MHEGIB_CreatePixmap(d->win, c->width, c->height, 32);
	/* associate a Picture with it */
	c->contents_pic = MHEGIB_RenderCreatePicture(c->contents);

	/* and a Graphics Context */
	c->gc = (GC)MHEGIB_CreateGC(c->contents, 0, NULL);

	return c;
}

void
free_MHEGCanvas(MHEGCanvas *c)
{
	/* assert */
	if(c == NULL)
	{
		fatal("free_MHEGCanvas: passed a NULL canvas");
		return;
	}

	MHEGIB_FreeGC(c->gc);

	safe_free(c);

	return;
}

/*
 * set a border, no drawing will be done in the border (apart from the border itself)
 * width is in pixels (and will be scaled up by this routine in full screen mode)
 * style should be one of:
 * LineStyle_solid
 * LineStyle_dashed
 * LineStyle_dotted
 * (note: UK MHEG Profile says we can treat ALL line styles as solid)
 */

void
MHEGCanvas_setBorder(MHEGCanvas *c, int width, int style, MHEGColour *colour)
{
	MHEGDisplay *d = MHEGEngine_getDisplay();
	XGCValues gcvals;
	XRectangle clip_rect;

	if(width <= 0)
		return;

	if(style != LineStyle_solid)
		error("MHEGCanvas_setBorder: LineStyle %d not supported (using a solid line)", style);

	/* scale width if fullscreen */
	c->border = MHEGDisplay_scaleX(d, width);

	/* draw the border */
	gcvals.foreground = pixel_value(colour);
	MHEGIB_ChangeGC(c->gc, GCForeground, &gcvals);
	/* top */
	MHEGIB_FillRectangle(c->contents, c->gc, 0, 0, c->width, c->border);
	/* bottom */
	MHEGIB_FillRectangle(c->contents, c->gc, 0, c->height - c->border, c->width, c->border);
	/* left */
	MHEGIB_FillRectangle(c->contents, c->gc, 0, 0, c->border, c->height);
	/* right */
	MHEGIB_FillRectangle(c->contents, c->gc, c->width - c->border, 0, c->border, c->height);

	/* set a clip mask, so no futher drawing will change the border */
	clip_rect.x = c->border;
	clip_rect.y = c->border;
	clip_rect.width = c->width - (2 * c->border);
	clip_rect.height = c->height - (2 * c->border);
	MHEGIB_SetClipRectangles(c->gc, 0, 0, &clip_rect, 1, Unsorted);

	return;
}

/*
 * drawing routine notes:
 * if border width is W, then coord {W,W} is the first pixel that will not be hidden by the border
 * all coords will be scaled up by these drawing routines if we are using full screen mode
 * the line width will also be scaled up in full screen mode (based on the resolution in the X direction)
 * line style should be one of:
 * LineStyle_solid
 * LineStyle_dashed
 * LineStyle_dotted
 * but, the UK MHEG Profile says we can treat ALL line styles as solid
 * UK MHEG Profile says no alpha blending is done within the DynamicLineArtClass canvas
 * ie all pixel values are put directly onto the canvas, replacing what was there before
 */

/*
 * fill the image (excluding the border) with the given colour
 */

void
MHEGCanvas_clear(MHEGCanvas *c, MHEGColour *colour)
{
	XGCValues gcvals;

	gcvals.foreground = pixel_value(colour);
	MHEGIB_ChangeGC(c->gc, GCForeground, &gcvals);

	/* width/height are already scaled as needed */
	MHEGIB_FillRectangle(c->contents, c->gc, c->border, c->border, c->width - (2 * c->border), c->height - (2 * c->border));

	return;
}

/*
 * draw an arc enclosed by the given box, starting at start (0 = 3 o' clock) for arc degrees anticlockwise
 * start and arc should be in degrees * 64
 * the arc is drawn width pixels wide (ie it may stick out of the box) in the given colour
 */

void
MHEGCanvas_drawArc(MHEGCanvas *c, XYPosition *pos, OriginalBoxSize *box, int start, int arc, int width, int style, MHEGColour *colour)
{
	MHEGDisplay *d = MHEGEngine_getDisplay();
	XGCValues gcvals;
	int x, y, w, h;

	if(width <= 0)
		return;

	if(style != LineStyle_solid)
		error("MHEGCanvas_drawArc: LineStyle %d not supported (using a solid line)", style);

	/* scale up if fullscreen */
	x = MHEGDisplay_scaleX(d, pos->x_position);
	y = MHEGDisplay_scaleY(d, pos->y_position);
	w = MHEGDisplay_scaleX(d, box->x_length);
	h = MHEGDisplay_scaleY(d, box->y_length);
	width = MHEGDisplay_scaleX(d, width);

	/* set up the GC values */
	gcvals.foreground = pixel_value(colour);
	gcvals.line_width = width;
	MHEGIB_ChangeGC(c->gc, GCForeground | GCLineWidth, &gcvals);

	/* luckily X uses the same params as MHEG */
	MHEGIB_DrawArc(c->contents, c->gc, x, y, w, h, start, arc);

	return;
}

/*
 * draw a pie chart sector enclosed by the given box, starting at start (0 = 3 o' clock) for arc degrees anticlockwise
 * start and arc should be in degrees * 64
 * the sector is drawn in fill_col
 * the sector is outlined with a line width pixels wide (ie it may stick out of the box) in line_col
 */

void
MHEGCanvas_drawSector(MHEGCanvas *c,
		      XYPosition *pos, OriginalBoxSize *box, int start, int arc,
		      int width, int style, MHEGColour *line_col, MHEGColour *fill_col)
{
	MHEGDisplay *d = MHEGEngine_getDisplay();
	XGCValues gcvals;
	int x, y, w, h;
	int cx, cy;
	double start_rads, end_rads;
	int edgex, edgey;

	if(style != LineStyle_solid)
		error("MHEGCanvas_drawSector: LineStyle %d not supported (using a solid line)", style);

	/* scale up if fullscreen */
	x = MHEGDisplay_scaleX(d, pos->x_position);
	y = MHEGDisplay_scaleY(d, pos->y_position);
	w = MHEGDisplay_scaleX(d, box->x_length);
	h = MHEGDisplay_scaleY(d, box->y_length);
	width = MHEGDisplay_scaleX(d, width);

	/* fill it */
	gcvals.foreground = pixel_value(fill_col);
	gcvals.arc_mode = ArcPieSlice;
	MHEGIB_ChangeGC(c->gc, GCForeground | GCArcMode, &gcvals);

	/* luckily X uses the same params as MHEG */
	MHEGIB_FillArc(c->contents, c->gc, x, y, w, h, start, arc);

	/* draw the outline */
	if(width <= 0)
		return;

	gcvals.foreground = pixel_value(line_col);
	gcvals.line_width = width;
	MHEGIB_ChangeGC(c->gc, GCForeground | GCLineWidth, &gcvals);

	/* easy bit */
	MHEGIB_DrawArc(c->contents, c->gc, x, y, w, h, start, arc);

	/* lines from the centre to the start and end of the arc */
	cx = x + (w / 2);
	cy = y + (h / 2);
	start_rads = ((double) start / 64.0) * M_PI / 180.0;
	edgex = cos(start_rads) * (w / 2);
	edgey = sin(start_rads) * (h / 2);
	/* cy - edgey, because Y increases as we go down the screen */
	MHEGIB_DrawLine(c->contents, c->gc, cx, cy, cx + edgex, cy - edgey);

	end_rads = ((double) (start + arc) / 64.0) * M_PI / 180.0;
	edgex = cos(end_rads) * (w / 2);
	edgey = sin(end_rads) * (h / 2);
	MHEGIB_DrawLine(c->contents, c->gc, cx, cy, cx + edgex, cy - edgey);

/* TODO */
/* make proper joins between the arc and the 2 lines */
/* ends of the arc are on a tangent to the ellipse */
/* can't join to a length 0 line because it has no direction, but a 1 pixel long line may be too long */

	return;
}

/*
 * draw a line between p1 and p2
 */

void
MHEGCanvas_drawLine(MHEGCanvas *c, XYPosition *p1, XYPosition *p2, int width, int style, MHEGColour *colour)
{
	MHEGDisplay *d = MHEGEngine_getDisplay();
	XGCValues gcvals;
	int x1, y1, x2, y2;

	if(width <= 0)
		return;

	if(style != LineStyle_solid)
		error("MHEGCanvas_drawLine: LineStyle %d not supported (using a solid line)", style);

	/* scale up if fullscreen */
	x1 = MHEGDisplay_scaleX(d, p1->x_position);
	y1 = MHEGDisplay_scaleY(d, p1->y_position);
	x2 = MHEGDisplay_scaleX(d, p2->x_position);
	y2 = MHEGDisplay_scaleY(d, p2->y_position);
	width = MHEGDisplay_scaleX(d, width);

	/* set up the GC values */
	gcvals.foreground = pixel_value(colour);
	gcvals.line_width = width;
	MHEGIB_ChangeGC(c->gc, GCForeground | GCLineWidth, &gcvals);

	MHEGIB_DrawLine(c->contents, c->gc, x1, y1, x2, y2);

	return;
}

/*
 * draw an oval enclosed by the given box
 * the outline is drawn width pixels wide (ie it may stick out of the box) in line_col
 * the oval is filled with fill_col
 */

void
MHEGCanvas_drawOval(MHEGCanvas *c, XYPosition *pos, OriginalBoxSize *box, int width, int style, MHEGColour *line_col, MHEGColour *fill_col)
{
	MHEGDisplay *d = MHEGEngine_getDisplay();
	XGCValues gcvals;
	int x, y, w, h;

	if(style != LineStyle_solid)
		error("MHEGCanvas_drawOval: LineStyle %d not supported (using a solid line)", style);

	/* scale up if fullscreen */
	x = MHEGDisplay_scaleX(d, pos->x_position);
	y = MHEGDisplay_scaleY(d, pos->y_position);
	w = MHEGDisplay_scaleX(d, box->x_length);
	h = MHEGDisplay_scaleY(d, box->y_length);
	width = MHEGDisplay_scaleX(d, width);

	/* fill it */
	gcvals.foreground = pixel_value(fill_col);
	MHEGIB_ChangeGC(c->gc, GCForeground, &gcvals);

	MHEGIB_FillArc(c->contents, c->gc, x, y, w, h, 0, 360 * 64);

	/* draw the outline */
	if(width <= 0)
		return;

	gcvals.foreground = pixel_value(line_col);
	gcvals.line_width = width;
	MHEGIB_ChangeGC(c->gc, GCForeground | GCLineWidth, &gcvals);

	MHEGIB_DrawArc(c->contents, c->gc, x, y, w, h, 0, 360 * 64);

	return;
}

/*
 * draw a closed polygon
 * the outline is drawn width pixels wide (ie it may stick out of the box) in line_col
 * it is filled with fill_col
 * UK MHEG Profile says polygons must be convex
 */

void
MHEGCanvas_drawPolygon(MHEGCanvas *c, LIST_OF(XYPosition) *xy_list, int width, int style, MHEGColour *line_col, MHEGColour *fill_col)
{
	MHEGDisplay *d = MHEGEngine_getDisplay();
	LIST_TYPE(XYPosition) *pos;
	unsigned int nxpts;
	XPoint *xpts;
	unsigned int i;
	XGCValues gcvals;

	if(style != LineStyle_solid)
		error("MHEGCanvas_drawPolygon: LineStyle %d not supported (using a solid line)", style);

	/* scale up if fullscreen */
	width = MHEGDisplay_scaleX(d, width);

	/* convert the XYPosition list into an array of XPoint's */
	nxpts = 0;
	for(pos=xy_list; pos; pos=pos->next)
		nxpts ++;

	if(nxpts == 0)
		return;

	/* +1 so we can close it for XDrawLines */
	xpts = safe_malloc((nxpts + 1) * sizeof(XPoint));

	pos = xy_list;
	for(i=0; i<nxpts; i++)
	{
		/* scale up if fullscreen */
		xpts[i].x = MHEGDisplay_scaleX(d, pos->item.x_position);
		xpts[i].y = MHEGDisplay_scaleY(d, pos->item.y_position);
		pos = pos->next;
	}

	/* fill it */
	gcvals.foreground = pixel_value(fill_col);
	MHEGIB_ChangeGC(c->gc, GCForeground, &gcvals);

	MHEGIB_FillPolygon(c->contents, c->gc, xpts, nxpts, Convex, CoordModeOrigin);

	/* draw the outline */
	if(width > 0)
	{
		/* close the polygon */
		xpts[nxpts].x = MHEGDisplay_scaleX(d, xy_list->item.x_position);
		xpts[nxpts].y = MHEGDisplay_scaleY(d, xy_list->item.y_position);
		/* set the line width and colour */
		gcvals.foreground = pixel_value(line_col);
		gcvals.line_width = width;
		MHEGIB_ChangeGC(c->gc, GCForeground | GCLineWidth, &gcvals);
		/* draw it */
		MHEGIB_DrawLines(c->contents, c->gc, xpts, nxpts + 1, CoordModeOrigin);
	}

	/* clean up */
	safe_free(xpts);

	return;
}

/*
 * draw a set of joined lines
 * the lines are drawn width pixels wide in the given colour
 */

void
MHEGCanvas_drawPolyline(MHEGCanvas *c, LIST_OF(XYPosition) *xy_list, int width, int style, MHEGColour *colour)
{
	MHEGDisplay *d = MHEGEngine_getDisplay();
	LIST_TYPE(XYPosition) *pos;
	unsigned int nxpts;
	XPoint *xpts;
	unsigned int i;
	XGCValues gcvals;

	if(width <= 0)
		return;

	if(style != LineStyle_solid)
		error("MHEGCanvas_drawPolyline: LineStyle %d not supported (using a solid line)", style);

	/* scale up if fullscreen */
	width = MHEGDisplay_scaleX(d, width);

	/* convert the XYPosition list into an array of XPoint's */
	nxpts = 0;
	for(pos=xy_list; pos; pos=pos->next)
		nxpts ++;

	if(nxpts == 0)
		return;

	xpts = safe_malloc(nxpts * sizeof(XPoint));

	pos = xy_list;
	for(i=0; i<nxpts; i++)
	{
		/* scale up if fullscreen */
		xpts[i].x = MHEGDisplay_scaleX(d, pos->item.x_position);
		xpts[i].y = MHEGDisplay_scaleY(d, pos->item.y_position);
		pos = pos->next;
	}

	/* set the line width and colour */
	gcvals.foreground = pixel_value(colour);
	gcvals.line_width = width;
	MHEGIB_ChangeGC(c->gc, GCForeground | GCLineWidth, &gcvals);

	/* draw it */
	MHEGIB_DrawLines(c->contents, c->gc, xpts, nxpts, CoordModeOrigin);

	/* clean up */
	safe_free(xpts);

	return;
}

/*
 * draw a rectangle
 * the outline is drawn width pixels wide (ie it may stick out of the box) in line_col
 * it is filled with fill_col
 */

void
MHEGCanvas_drawRectangle(MHEGCanvas *c, XYPosition *pos, OriginalBoxSize *box, int width, int style, MHEGColour *line_col, MHEGColour *fill_col)
{
	MHEGDisplay *d = MHEGEngine_getDisplay();
	XGCValues gcvals;
	int x, y, w, h;

	if(style != LineStyle_solid)
		error("MHEGCanvas_drawRectangle: LineStyle %d not supported (using a solid line)", style);

	/* scale up if fullscreen */
	x = MHEGDisplay_scaleX(d, pos->x_position);
	y = MHEGDisplay_scaleY(d, pos->y_position);
	w = MHEGDisplay_scaleX(d, box->x_length);
	h = MHEGDisplay_scaleY(d, box->y_length);
	width = MHEGDisplay_scaleX(d, width);

	/* fill it */
	gcvals.foreground = pixel_value(fill_col);
	MHEGIB_ChangeGC(c->gc, GCForeground, &gcvals);

	MHEGIB_FillRectangle(c->contents, c->gc, x, y, w, h);

	/* draw the outline */
	if(width <= 0)
		return;

	gcvals.foreground = pixel_value(line_col);
	gcvals.line_width = width;
	MHEGIB_ChangeGC(c->gc, GCForeground | GCLineWidth, &gcvals);

	MHEGIB_DrawRectangle(c->contents, c->gc, x, y, w, h);

	return;
}

