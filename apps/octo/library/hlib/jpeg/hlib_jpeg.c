/**
	@file     hxjpeg.c
	@brief   libjpeg interface function file

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <setjmp.h>



#include <jpeglib.h>

#include <dlfcn.h>

#include "_hlib_stdlib.h"
#include "_hlib_mem.h"
#include "_hlib_sem.h"
#include "_hlib_log.h"
#include "_hlib_macro.h"


#include "_hlib_jpeg.h"

//#define	SCALING_BILINEAR
#define	SCALING_BRESENHAM
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
struct hlib_jpeg_error_mgr
{
	struct jpeg_error_mgr 	pub;				/* "public" fields */
	jmp_buf 				setjmp_buffer;		/* for return to caller */
};

typedef struct hlib_jpeg_error_mgr * s_jpeg_error_ptr;

typedef struct
{
	void		*pvJpegLibHandle;
	HBOOL		bEnable;
	HUINT32		ulSemId;

	//	Symbol Link for LIBPNG Used function.
	void			(*fn_jpeg_create_decompress)	(j_decompress_ptr cinfo, int version, size_t structsize);
	void			(*fn_jpeg_destroy_decompress)	(j_decompress_ptr cinfo);
	void			(*fn_jpeg_stdio_src)			(j_decompress_ptr cinfo, FILE * infile);
	void			(*fn_jpeg_mem_src)			(j_decompress_ptr cinfo, unsigned char *inbuffer, unsigned long insize);
	int				(*fn_jpeg_read_header)			(j_decompress_ptr cinfo, boolean require_image);
	struct jpeg_error_mgr* (*fn_jpeg_std_error)		(struct jpeg_error_mgr * err);

	void			(*fn_jpeg_stdio_dest)			(j_compress_ptr cinfo, FILE *outfile);
	boolean			(*fn_jpeg_start_decompress)		(j_decompress_ptr cinfo);
	JDIMENSION		(*fn_jpeg_read_scanlines)		(j_decompress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION max_lines);
	boolean			(*fn_jpeg_finish_decompress)	(j_decompress_ptr cinfo);
	void			(*fn_jpeg_finish_compress)		(j_compress_ptr cinfo);
	void			(*fn_jpeg_set_defaults)			(j_compress_ptr cinfo);
	void			(*fn_jpeg_start_compress)		(j_compress_ptr cinfo, boolean write_all_tables);
	void			(*fn_jpeg_create_compress)		(j_compress_ptr cinfo, int version, size_t structsize);
	void			(*fn_jpeg_destroy_compress)		(j_compress_ptr cinfo);
	JDIMENSION		(*fn_jpeg_write_scanlines)		(j_compress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION num_lines);
	void			(*fn_jpeg_set_quality)			(j_compress_ptr cinfo, int quality, boolean force_baseline);
} jpeg_init_t;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/*******************************************************************************************
 * Static
 ******************************************************************************************/
static jpeg_init_t*	jpeg_getInstance(void)
{
	static jpeg_init_t	*s_pstJpegInit = NULL;

	if (s_pstJpegInit)
	{
		HxSEMT_Get(s_pstJpegInit->ulSemId);

		if (s_pstJpegInit->bEnable)
			return s_pstJpegInit;

		HxSEMT_Release(s_pstJpegInit->ulSemId);
		return NULL;
	} else
	{
		jpeg_init_t	*jpeg = HLIB_MEM_Calloc(sizeof(jpeg_init_t));

		s_pstJpegInit = jpeg;

		HxSEMT_Create(&jpeg->ulSemId, "jpgsem", 0);
		HxSEMT_Get(jpeg->ulSemId);

		jpeg->pvJpegLibHandle = dlopen("/usr/lib/libjpeg.so", RTLD_LAZY);
		if (jpeg->pvJpegLibHandle == NULL)
		{
			HxLOG_Critical("[%s](%d) can't find libexif.so in /usr/lib directory!!!\n");
			HxLOG_Assert(0);

			return NULL;
		}
		jpeg->fn_jpeg_create_decompress		= dlsym(jpeg->pvJpegLibHandle, "jpeg_CreateDecompress");
		jpeg->fn_jpeg_destroy_decompress	= dlsym(jpeg->pvJpegLibHandle, "jpeg_destroy_decompress");
		jpeg->fn_jpeg_stdio_src				= dlsym(jpeg->pvJpegLibHandle, "jpeg_stdio_src");
		jpeg->fn_jpeg_mem_src				= dlsym(jpeg->pvJpegLibHandle, "jpeg_mem_src");
		jpeg->fn_jpeg_read_header			= dlsym(jpeg->pvJpegLibHandle, "jpeg_read_header");
		jpeg->fn_jpeg_std_error				= dlsym(jpeg->pvJpegLibHandle, "jpeg_std_error");

		jpeg->fn_jpeg_stdio_dest			= dlsym(jpeg->pvJpegLibHandle, "jpeg_stdio_dest");
		jpeg->fn_jpeg_start_decompress		= dlsym(jpeg->pvJpegLibHandle, "jpeg_start_decompress");
		jpeg->fn_jpeg_read_scanlines		= dlsym(jpeg->pvJpegLibHandle, "jpeg_read_scanlines");
		jpeg->fn_jpeg_finish_decompress		= dlsym(jpeg->pvJpegLibHandle, "jpeg_finish_decompress");
		jpeg->fn_jpeg_set_defaults			= dlsym(jpeg->pvJpegLibHandle, "jpeg_set_defaults");
		jpeg->fn_jpeg_start_compress		= dlsym(jpeg->pvJpegLibHandle, "jpeg_start_compress");
		jpeg->fn_jpeg_create_compress		= dlsym(jpeg->pvJpegLibHandle, "jpeg_CreateCompress");
		jpeg->fn_jpeg_destroy_compress		= dlsym(jpeg->pvJpegLibHandle, "jpeg_destroy_compress");
		jpeg->fn_jpeg_write_scanlines		= dlsym(jpeg->pvJpegLibHandle, "jpeg_write_scanlines");
		jpeg->fn_jpeg_finish_compress		= dlsym(jpeg->pvJpegLibHandle, "jpeg_finish_compress");
		jpeg->fn_jpeg_set_quality			= dlsym(jpeg->pvJpegLibHandle, "jpeg_set_quality");

		HxLOG_Assert(jpeg->fn_jpeg_create_decompress);
		HxLOG_Assert(jpeg->fn_jpeg_destroy_decompress);
		HxLOG_Assert(jpeg->fn_jpeg_stdio_src);
		HxLOG_Assert(jpeg->fn_jpeg_mem_src);
		HxLOG_Assert(jpeg->fn_jpeg_read_header);
		HxLOG_Assert(jpeg->fn_jpeg_std_error);
		HxLOG_Assert(jpeg->fn_jpeg_stdio_dest);
		HxLOG_Assert(jpeg->fn_jpeg_read_scanlines);
		HxLOG_Assert(jpeg->fn_jpeg_finish_decompress);
		HxLOG_Assert(jpeg->fn_jpeg_start_compress);
		HxLOG_Assert(jpeg->fn_jpeg_create_compress);
		HxLOG_Assert(jpeg->fn_jpeg_destroy_compress);
		HxLOG_Assert(jpeg->fn_jpeg_write_scanlines);
		HxLOG_Assert(jpeg->fn_jpeg_finish_compress);
		HxLOG_Assert(jpeg->fn_jpeg_set_quality);

		jpeg->bEnable = TRUE;

		s_pstJpegInit = jpeg;
	}

	return s_pstJpegInit;
}

void 	hlib_jpeg_error_exit(j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	s_jpeg_error_ptr myerr = (s_jpeg_error_ptr)cinfo->err;

	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	//(*cinfo->err->output_message)(cinfo);

	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}

#ifdef	SCALING_BRESENHAM
static void		jpeg_resize_scanline(HINT32 nColorDepth, HUINT8 *pDst, const HUINT8 *pSrc, HINT32 nSrcWidth, HINT32 nDstWidth)
{
	HINT32	nNumPixels = nDstWidth;
	HINT32	nIntPart   = (nSrcWidth / nDstWidth);
	HINT32	nFracPart  = (nSrcWidth % nDstWidth);
	HINT32	E = 0, i;

	while (nNumPixels-- > 0)
	{
		for (i = 0; i < nColorDepth; i++)
			pDst[i] = pSrc[i];
		pDst += nColorDepth;
		pSrc += (nIntPart * nColorDepth);
		E    += nFracPart;
		if (E >= nDstWidth)
		{
			E -= (nDstWidth);
			pSrc += nColorDepth;
		}
	}
}

static HERROR	jpeg_resize_rect(HINT32 nColorDepth, HINT32 nSrcWidth, HINT32 nSrcHeight, HINT32 nDstWidth, HINT32 nDstHeight, const HUINT8 *pSrc, HUINT8 *pDst)
{
	HINT32	nNumPixels = nDstHeight;
	HINT32	nIntPart   = (nSrcHeight / nDstHeight) * nSrcWidth;
	HINT32	nFracPart  = (nSrcHeight % nDstHeight);
	HINT32	E = 0;
	HUINT8	*pucPrevSource = NULL;

	while (nNumPixels-- > 0)
	{
		if (pucPrevSource == pSrc)
		{
			HxSTD_MemCopy(pDst, pDst - nDstWidth * nColorDepth, nDstWidth * nColorDepth);
		} else
		{
			jpeg_resize_scanline(nColorDepth, pDst, pSrc, nSrcWidth, nDstWidth);
			pucPrevSource = (HUINT8*)pSrc;
		}

		pDst += (nDstWidth * nColorDepth);
		pSrc += (nIntPart * nColorDepth);
		E    += (nFracPart);
		if (E >= nDstHeight)
		{
			E -= nDstHeight;
			pSrc += (nSrcWidth * nColorDepth);
		}
	}

	return ERR_OK;
}
#endif

#ifdef	SCALING_BILINEAR
static HINT32	jpeg_resize_bilinear(struct jpeg_decompress_struct *dinfo, struct jpeg_compress_struct *cinfo, const HUINT8 *p, HUINT8 **o)
{
	HFLOAT64	factor, fraction_x, fraction_y, one_minus_x, one_minus_y;
	HINT32		ceil_x, ceil_y, floor_x, floor_y, s_row_width;
	HINT32		tcx, tcy, tfx, tfy, tx, ty, t_row_width, x, y;
	HUINT8		*q;

	/* RGB images have 3 components, grayscale images have only one. */
	s_row_width = dinfo->num_components * dinfo->output_width;
	t_row_width = dinfo->num_components * cinfo->image_width;

	factor = (double)dinfo->output_width / (double)cinfo->image_width;

	if (*o == NULL)
		return (-1);

	/* No scaling needed. */
	if (dinfo->output_width == cinfo->image_width) {
		HxSTD_MemCopy(*o, p, s_row_width * dinfo->output_height);
		return (0);
	}

	q = *o;

	for (y = 0; y < cinfo->image_height; y++) {
		for (x = 0; x < cinfo->image_width; x++) {
			floor_x = (int)(x * factor);
			floor_y = (int)(y * factor);
			ceil_x = (floor_x + 1 > cinfo->image_width)
			    ? floor_x
			    : floor_x + 1;
			ceil_y = (floor_y + 1 > cinfo->image_height)
			    ? floor_y
			    : floor_y + 1;
			fraction_x = (x * factor) - floor_x;
			fraction_y = (y * factor) - floor_y;
			one_minus_x = 1.0 - fraction_x;
			one_minus_y = 1.0 - fraction_y;

			tx  = x * dinfo->num_components;
			ty  = y * t_row_width;
			tfx = floor_x * dinfo->num_components;
			tfy = floor_y * s_row_width;
			tcx = ceil_x * dinfo->num_components;
			tcy = ceil_y * s_row_width;

			q[tx + ty] = one_minus_y *
			    (one_minus_x * p[tfx + tfy] +
			    fraction_x * p[tcx + tfy]) +
			    fraction_y * (one_minus_x * p[tfx + tcy] +
			    fraction_x  * p[tcx + tcy]);

			if (dinfo->num_components != 1) {
				q[tx + ty + 1] = one_minus_y *
				    (one_minus_x * p[tfx + tfy + 1] +
				    fraction_x * p[tcx + tfy + 1]) +
				    fraction_y * (one_minus_x *
				    p[tfx + tcy + 1] + fraction_x *
				    p[tcx + tcy + 1]);

				q[tx + ty + 2] = one_minus_y *
				    (one_minus_x * p[tfx + tfy + 2] +
				    fraction_x * p[tcx + tfy + 2]) +
				    fraction_y * (one_minus_x *
				    p[tfx + tcy + 2] + fraction_x *
				    p[tcx + tcy + 2]);
			}
		}
	}

	return (0);
}
#endif

HERROR	hlib_jpeg_default_convert(HUINT8 *pstBuffer, const HUINT8 *pucSrc, HINT32 nWidth, HINT32 nScanline)
{
	HINT32	i;
	HUINT8	*pSrc;
	HUINT8	*pucBuffer = (HUINT8*)pstBuffer;

	pSrc = (HUINT8*)(pucSrc + nWidth * nScanline * 3);		//	32BIT Pixel
	for (i = 0; i < nWidth; i++)
	{
		*pucBuffer++ = *pSrc++;
		*pucBuffer++ = *pSrc++;
		*pucBuffer++ = *pSrc++;
	}

	return ERR_OK;
}

/* This gets JPEG's dimension from its header regardless of having EXIF data or not. */
HERROR	HLIB_JPEG_GetFileInfo(const HCHAR *pszFileName, HUINT32 *out_width, HUINT32 *out_height)
{
	jpeg_init_t *jpeg = jpeg_getInstance();
	struct jpeg_decompress_struct cinfo;
	struct hlib_jpeg_error_mgr jerr;

	FILE * infile = NULL;		/* source file */

	if (jpeg == NULL)
		return ERR_FAIL;

	if ((infile = fopen(pszFileName, "rb")) == NULL)
	{
		HxLOG_Debug("can't open %s\n", pszFileName);
		HxSEMT_Release(jpeg->ulSemId);

		return ERR_FAIL;
	}

	cinfo.err = jpeg->fn_jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = hlib_jpeg_error_exit;
	if (setjmp(jerr.setjmp_buffer))
	{
		HxLOG_Critical("\n\n");
		jpeg->fn_jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		HxSEMT_Release(jpeg->ulSemId);

		return ERR_FAIL;
	}
	/* Now we can initialize the JPEG decompression object. */
	jpeg->fn_jpeg_create_decompress(&cinfo, JPEG_LIB_VERSION, sizeof(struct jpeg_decompress_struct));

	/* specify data source */
	jpeg->fn_jpeg_stdio_src(&cinfo, infile);

	/* read file parameters with jpeg_read_header() */
	jpeg->fn_jpeg_read_header(&cinfo, TRUE);
	HxLOG_Debug("original image resolution is %d x %d\n", cinfo.image_width, cinfo.image_height);

	/* give size info back */
	*out_width = cinfo.image_width;
	*out_height = cinfo.image_height;

	/* release JPEG decompression object */
	jpeg->fn_jpeg_destroy_decompress(&cinfo);
	fclose(infile);

	HxSEMT_Release(jpeg->ulSemId);

	return ERR_OK;
}

/* This gets JPEG's dimension from its header regardless of having EXIF data or not. */
HERROR	HLIB_JPEG_GetBufferInfo(HUINT8 *pucData, HINT32 size, HUINT32 *out_width, HUINT32 *out_height)
{
	jpeg_init_t *jpeg = jpeg_getInstance();

	struct jpeg_decompress_struct cinfo;
	struct hlib_jpeg_error_mgr jerr;

	if (jpeg == NULL)
		return ERR_FAIL;

	cinfo.err = jpeg->fn_jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = hlib_jpeg_error_exit;
	if (setjmp(jerr.setjmp_buffer))
	{
		HxLOG_Critical("\n\n");
		jpeg->fn_jpeg_destroy_decompress(&cinfo);
		HxSEMT_Release(jpeg->ulSemId);
		return ERR_FAIL;
	}
	/* Now we can initialize the JPEG decompression object. */
	jpeg->fn_jpeg_create_decompress(&cinfo, JPEG_LIB_VERSION, sizeof(struct jpeg_decompress_struct));

	/* specify data source */
	jpeg->fn_jpeg_mem_src(&cinfo, pucData, size);

	/* read file parameters with jpeg_read_header() */
	jpeg->fn_jpeg_read_header(&cinfo, TRUE);
	HxLOG_Debug("original image resolution is %d x %d\n", cinfo.image_width, cinfo.image_height);

	/* give size info back */
	*out_width = cinfo.image_width;
	*out_height = cinfo.image_height;

	/* release JPEG decompression object */
	jpeg->fn_jpeg_destroy_decompress(&cinfo);

	HxSEMT_Release(jpeg->ulSemId);

	return ERR_OK;
}

#define	SCALE_FACTOR_NUM		16
static double	s_fScaleFactor[SCALE_FACTOR_NUM] = { 0.125, 0.25, 0.375, 0.5,
													 0.625, 0.75, 0.875, 1.0,
													 1.125, 1.25, 1.375, 1.5,
													 1.625, 1.75, 1.875, 2.0 };

HERROR	HLIB_JPEG_CreateScaledJPEG(const HCHAR *pszInFileName, const HCHAR *pszOutFileName, HINT32 nScaledWidth, HINT32 nScaledHeight)
{
	jpeg_init_t *jpeg = jpeg_getInstance();

	HINT32		i;
	HINT32		scaleWidth, scaleHeight;
	HINT32		row_width, n = 0;
	HUINT8		*pucOutput = NULL, *p = NULL;
	HFLOAT64	wFactor, hFactor, factor;
	FILE		*inFile = NULL, *outFile = NULL;

	struct jpeg_decompress_struct	dinfo;
	struct jpeg_compress_struct		cinfo;
	struct jpeg_error_mgr			jerr;

	JSAMPARRAY	samp;
	JSAMPROW	row_pointer[1];

	if (jpeg == NULL)
		return ERR_FAIL;

	dinfo.err = jpeg->fn_jpeg_std_error(&jerr);
	cinfo.err = jpeg->fn_jpeg_std_error(&jerr);

	inFile = fopen(pszInFileName, "rb");
	if (inFile == NULL)
		goto err_rtn;
	jpeg->fn_jpeg_create_decompress(&dinfo, JPEG_LIB_VERSION, sizeof(struct jpeg_decompress_struct));
	jpeg->fn_jpeg_stdio_src(&dinfo, inFile);
	jpeg->fn_jpeg_read_header(&dinfo, TRUE);

	scaleHeight = nScaledHeight;
	scaleWidth  = nScaledWidth;

	wFactor = (HFLOAT64)scaleWidth / (HFLOAT64)dinfo.image_width;
	hFactor = (HFLOAT64)scaleHeight / (HFLOAT64)dinfo.image_height;

	factor = HxMACRO_MIN(wFactor, hFactor);
	for (i = 0; i < SCALE_FACTOR_NUM; i++)
	{
		if (factor < s_fScaleFactor[i])
			break;
	}
	if (i == SCALE_FACTOR_NUM)
		i = SCALE_FACTOR_NUM - 1;

	dinfo.scale_denom = 8;
	dinfo.scale_num   = i + 1;

	jpeg->fn_jpeg_start_decompress(&dinfo);

	row_width = dinfo.output_width * dinfo.num_components;
	pucOutput = p = HLIB_MEM_Malloc(row_width * dinfo.output_height);
	if (p == NULL)
		goto err_rtn;
	samp = (*dinfo.mem->alloc_sarray)((j_common_ptr)&dinfo, JPOOL_IMAGE, row_width, 1);

	while (dinfo.output_scanline < dinfo.output_height)
	{
		jpeg->fn_jpeg_read_scanlines(&dinfo, samp, 1);

		HxSTD_MemCopy(p, *samp, row_width);
		p += row_width;
	}

	scaleWidth  = dinfo.output_width;
	scaleHeight = dinfo.output_height;

	jpeg->fn_jpeg_finish_decompress(&dinfo);
	fclose(inFile);	inFile = NULL;
	jpeg->fn_jpeg_destroy_decompress(&dinfo);

	//	Scale and write file.
	outFile = fopen(pszOutFileName, "wb");
	if (outFile == NULL)
		goto err_rtn;

	jpeg->fn_jpeg_create_compress(&cinfo, JPEG_LIB_VERSION, sizeof(struct jpeg_compress_struct));
	jpeg->fn_jpeg_stdio_dest(&cinfo, outFile);
	cinfo.image_width  = scaleWidth;
	cinfo.image_height = scaleHeight;
	cinfo.input_components = dinfo.num_components;
	cinfo.in_color_space = dinfo.out_color_space;
	jpeg->fn_jpeg_set_defaults(&cinfo);

	jpeg->fn_jpeg_start_compress(&cinfo, TRUE);
	while (cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = &pucOutput[cinfo.input_components * cinfo.image_width * cinfo.next_scanline];
		jpeg->fn_jpeg_write_scanlines(&cinfo, row_pointer, 1);
		n++;
	}
	jpeg->fn_jpeg_finish_compress(&cinfo);
	fclose(outFile);
	jpeg->fn_jpeg_destroy_compress(&cinfo);

	if (pucOutput)
		HLIB_MEM_Free(pucOutput);

	HxSEMT_Release(jpeg->ulSemId);

	return ERR_OK;

err_rtn:
	if (inFile)
		(void)fclose(inFile);
	if (outFile)
		(void)fclose(outFile);
	if (pucOutput)
		HLIB_MEM_Free(pucOutput);

	HxSEMT_Release(jpeg->ulSemId);

	return ERR_FAIL;
}

HERROR	HLIB_JPEG_CreateScaledJPEGMEM(HUINT8 *pucData, HINT32 size, const HCHAR *pszOutFileName, HINT32 nScaledWidth, HINT32 nScaledHeight)
{
	jpeg_init_t *jpeg = jpeg_getInstance();

	HINT32		scaleWidth, scaleHeight, i;
	HINT32		row_width, n = 0;
	HUINT8		*pucOutput = NULL, *p = NULL;
	HFLOAT64	wFactor, hFactor, factor;
	FILE		*outFile = NULL;

	struct jpeg_decompress_struct	dinfo;
	struct jpeg_compress_struct		cinfo;
	struct jpeg_error_mgr			jerr;

	JSAMPARRAY	samp;
	JSAMPROW	row_pointer[1];

	if (jpeg == NULL)
		return ERR_FAIL;

	dinfo.err = jpeg->fn_jpeg_std_error(&jerr);
	cinfo.err = jpeg->fn_jpeg_std_error(&jerr);

	jpeg->fn_jpeg_create_decompress(&dinfo, JPEG_LIB_VERSION, sizeof(struct jpeg_decompress_struct));
	jpeg->fn_jpeg_mem_src(&dinfo, pucData, size);
	jpeg->fn_jpeg_read_header(&dinfo, TRUE);

	scaleHeight = nScaledHeight;
	scaleWidth  = nScaledWidth;//(HINT32)((HFLOAT64)scaleHeight * ratio + 0.5);

	wFactor = (HFLOAT64)scaleWidth / (HFLOAT64)dinfo.image_width;
	hFactor = (HFLOAT64)scaleHeight / (HFLOAT64)dinfo.image_height;

	factor = HxMACRO_MIN(wFactor, hFactor);
	for (i = 0; i < SCALE_FACTOR_NUM; i++)
	{
		if (factor < s_fScaleFactor[i])
			break;
	}
	if (i == SCALE_FACTOR_NUM)
		i = SCALE_FACTOR_NUM - 1;

	dinfo.scale_denom = 8;
	dinfo.scale_num   = i + 1;

	jpeg->fn_jpeg_start_decompress(&dinfo);

	row_width = dinfo.output_width * dinfo.num_components;

	pucOutput = p = HLIB_MEM_Malloc(row_width * dinfo.output_height);
	if (p == NULL)
		goto err_rtn;
	samp = (*dinfo.mem->alloc_sarray)((j_common_ptr)&dinfo, JPOOL_IMAGE, row_width, 1);

	while (dinfo.output_scanline < dinfo.output_height)
	{
		jpeg->fn_jpeg_read_scanlines(&dinfo, samp, 1);
		HxSTD_MemCopy(p, *samp, row_width);
		p += row_width;
	}

	scaleWidth  = dinfo.output_width;
	scaleHeight = dinfo.output_height;

	jpeg->fn_jpeg_finish_decompress(&dinfo);
	jpeg->fn_jpeg_destroy_decompress(&dinfo);

	//	Scale and write file.
	outFile = fopen(pszOutFileName, "wb");
	if (outFile == NULL)
		goto err_rtn;

	jpeg->fn_jpeg_create_compress(&cinfo, JPEG_LIB_VERSION, sizeof(struct jpeg_compress_struct));
	jpeg->fn_jpeg_stdio_dest(&cinfo, outFile);
	cinfo.image_width  = scaleWidth;
	cinfo.image_height = scaleHeight;
	cinfo.input_components = dinfo.num_components;
	cinfo.in_color_space = dinfo.out_color_space;
	jpeg->fn_jpeg_set_defaults(&cinfo);

	jpeg->fn_jpeg_start_compress(&cinfo, TRUE);
	while (cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = &pucOutput[cinfo.input_components * cinfo.image_width * cinfo.next_scanline];
		jpeg->fn_jpeg_write_scanlines(&cinfo, row_pointer, 1);
		n++;
	}
	jpeg->fn_jpeg_finish_compress(&cinfo);
	fclose(outFile);
	jpeg->fn_jpeg_destroy_compress(&cinfo);

	if (pucOutput)
		HLIB_MEM_Free(pucOutput);

	HxSEMT_Release(jpeg->ulSemId);

	return ERR_OK;

err_rtn:
	if (outFile)
		(void)fclose(outFile);
	if (pucOutput)
		HLIB_MEM_Free(pucOutput);

	HxSEMT_Release(jpeg->ulSemId);

	return ERR_FAIL;
}

HERROR	HLIB_JPEG_ConvertBGRA2RGB(HUINT8 *pstBuffer, const HUINT8 *pucSrc, HINT32 nWidth, HINT32 nScanline)
{
	HINT32	i;
	HUINT8	*pSrc;
	HUINT8	*pucBuffer = (HUINT8*)pstBuffer;

	pSrc = (HUINT8*)(pucSrc + nWidth * nScanline * 4);		//	32BIT Pixel
	for (i = 0; i < nWidth; i++)
	{
		pucBuffer[2] = *pSrc++;
		pucBuffer[1] = *pSrc++;
		pucBuffer[0] = *pSrc++;
		pucBuffer += 3;
		pSrc++;
	}

	return ERR_OK;
}

HERROR	HLIB_JPEG_CreateFile(const HCHAR *pszOutName, HINT32 nWidth, HINT32 nHeight, const HUINT8 *pucBuffer, HERROR (*cvtFunc)(HUINT8*, const HUINT8 *, HINT32, HINT32))
{
	jpeg_init_t *jpeg = jpeg_getInstance();

	struct jpeg_compress_struct	cinfo;
	struct jpeg_error_mgr		jerr;

	JSAMPROW	row_pointer[1];
	int			row_stride;
	JSAMPLE		*buffer;

	FILE	*outFile;

	if (jpeg == NULL)
		return ERR_FAIL;

	cinfo.err = jpeg->fn_jpeg_std_error(&jerr);
	jpeg->fn_jpeg_create_compress(&cinfo, JPEG_LIB_VERSION, sizeof(struct jpeg_compress_struct));
	outFile = fopen(pszOutName, "wb");
	if (outFile == NULL)
	{
		HxSEMT_Release(jpeg->ulSemId);
		return ERR_FAIL;
	}

	buffer = (JSAMPLE*)HLIB_MEM_Malloc(nWidth * 3);		//	RGB

	jpeg->fn_jpeg_stdio_dest(&cinfo, outFile);
	cinfo.image_width  = nWidth;
	cinfo.image_height = nHeight;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	jpeg->fn_jpeg_set_defaults(&cinfo);
	jpeg->fn_jpeg_set_quality(&cinfo, 64, TRUE);		//	È­ÁúÀÌ ²¿Áö¸é Quality Factor¸¦ ³ô¿©ÁÜ --> default´Â 75
	jpeg->fn_jpeg_start_compress(&cinfo, TRUE);

	row_stride = nWidth * 3;
	while (cinfo.next_scanline < cinfo.image_height)
	{
		if (cvtFunc)
			cvtFunc((HUINT8*)buffer, pucBuffer, nWidth, cinfo.next_scanline);
		else
			hlib_jpeg_default_convert((HUINT8*)buffer, pucBuffer, nWidth, cinfo.next_scanline);

		row_pointer[0] = &buffer[0];//&buffer[cinfo.next_scanline * row_stride];
		(void)jpeg->fn_jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	HLIB_MEM_Free(buffer);

	jpeg->fn_jpeg_finish_compress(&cinfo);
	fclose(outFile);

	jpeg->fn_jpeg_destroy_compress(&cinfo);

	HxSEMT_Release(jpeg->ulSemId);

	return ERR_OK;
}


