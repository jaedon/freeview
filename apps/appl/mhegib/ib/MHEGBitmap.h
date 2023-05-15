/*
 * MHEGBitmap.h
 */

#ifndef __MHEGBITMAP_H__
#define __MHEGBITMAP_H__

typedef struct
{
	unsigned char	*pImage;			/* for MHEG_ContentHook_Bitmap_PNG */
	unsigned char	*pMpegImage;		/* for MHEG_ContentHook_Bitmap_MPEG (I-frame) */
	unsigned int	ulMpegImageSize;
} MHEGBitmap;

#endif	/* __MHEGBITMAP_H__ */

