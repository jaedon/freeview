#ifndef _FONT_API_H_
#define _FONT_API_H_

#include "htype.h"

#define	MAX_FONTNAME_LEN	20

#define		INT_MULT_NONZERO_B(a, b, t)	((a == 0) ? 0 : (a == 0xFF) ? (b) : (b == 0xFF) ? (a) : ((t) = (a) * (b) + 0x80, ((((t) >> 8) + (t)) >> 8)))
#define		INT_MULT(a, b, t)	((a == 0 || b == 0) ? 0 : (a == 0xFF) ? (b) : (b == 0xFF) ? (a) : ((t) = (a) * (b) + 0x80, ((((t) >> 8) + (t)) >> 8)))
#define		_GET_R(_ulColor)	((_ulColor & 0x00ff0000) >> 16)
#define		_GET_G(_ulColor)	((_ulColor & 0x0000ff00) >> 8)
#define		_GET_B(_ulColor)	((_ulColor & 0x000000ff))
#define		_GET_A(_ulColor)	((_ulColor & 0xff000000)>>24)

typedef struct _CHARTBL
{
	HUINT16  nWidth;
	HUINT32 nOffset;
} CHARTBL;

typedef struct _FONT
{
	HUINT8 szFaceName[MAX_FONTNAME_LEN];
	HUINT16 nPoints;
	HUINT16 nVertRes;
	HUINT16 nHorizRes;
	HUINT16 nAscent;
	HUINT16 nInternalLeading;
	HUINT16 nExternalLeading;

	HUINT8 bItalic;
	HUINT8 bUnderline;
	HUINT8 bStrikeOut;
	HUINT16 nWeight;

	HUINT8 nCharSet;

	HUINT16 nPixWidth;
	HUINT16 nPixHeight;

	HUINT8 nPitchAndFamily;

	HUINT16 nAvgWidth;
	HUINT16 nMaxWidth;

	HUINT8 nFirstChar;
	HUINT8 nLastChar;
	HUINT8 nDefaultChar;
	HUINT8 nBreakChar;

	HUINT16 nWidthBytes;

	CHARTBL *CharTable;
	HUINT8    *Bitmap;
} FONT;


extern FONT Myriad00_19Font[];
HUINT8	*g_BitmapList[1];

FONT 	*g_FontList[1] =
{
	&Myriad00_19Font
};

static	HUINT8		__FontTransLevel[0x10] = {
		0x00, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90,
		0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xE8, 0xF0, 0xff
};


#endif //_FONT_API_H_
