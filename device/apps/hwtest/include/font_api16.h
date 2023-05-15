#ifndef _FONT_API16_H_
#define _FONT_API16_H_

#include "htype.h"

#define	MAX_FONTNAME_LEN	20

#define		INT_MULT_NONZERO_B(a, b, t)	((a == 0) ? 0 : (a == 0xF) ? (b) : (b == 0xF) ? (a) : ((t) = (a) * (b) + 0x8, ((((t) >> 4) + (t)) >> 4)))
#define		INT_MULT(a, b, t)	((a == 0 || b == 0) ? 0 : (a == 0xF) ? (b) : (b == 0xF) ? (a) : ((t) = (a) * (b) + 0x8, ((((t) >> 4) + (t)) >> 4)))
#define		INT_INVERT_A(a)	(0xf - a)
#define		_GET_R(_ulColor)	((_ulColor & 0x0f00) >> 8)
#define		_GET_G(_ulColor)	((_ulColor & 0x00f0) >> 4)
#define		_GET_B(_ulColor)	((_ulColor & 0x000f))
#define		_GET_A(_ulColor)	((_ulColor & 0xf000) >> 12)
#define		_GET_ARGB(a,r,g,b)	(((a&0xf)<<12) | ((r&0xf)<<8) | ((g&0xf)<<4) | ((b&0xf)))


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
		0x0, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9,
		0xA, 0xB, 0xC, 0xD, 0xE, 0xE, 0xF, 0xf
};


#endif //_FONT_API16_H_
