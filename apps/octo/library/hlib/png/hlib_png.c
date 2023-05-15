/**
	@file     hxpng.c
	@brief   libpng interface function file

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

#include <dlfcn.h>

#include "_hlib_stdlib.h"
#include "_hlib_mem.h"
#include "_hlib_sem.h"
#include "_hlib_log.h"

#include "_hlib_png.h"

#include <png.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#undef	png_jmpbuf
#ifdef	PNG_SETJMP_SUPPORTED
	#define png_jmpbuf(png, png_ptr) 	(*((png_init_t*)png)->fn_png_set_longjmp_fn((png_ptr), longjmp, sizeof (jmp_buf)))
#else
	#define	png_jmpbuf(png, png_ptr)
#endif

#define	PNG_CHECK_SIG(png, sig, n)			!((png_init_t*)png)->fn_png_sig_cmp((sig), 0, (n))

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	void		*pvPNGLibHandle;
	HBOOL		bEnable;
	HUINT32		ulSemId;

	//	Symbol Link for LIBPNG Used function.
	HINT32		(*fn_png_sig_cmp)				(png_const_bytep sig, png_size_t start, png_size_t num_to_check);
	png_structp	(*fn_png_create_read_struct)	(png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);
	png_infop	(*fn_png_create_info_struct)	(png_structp png_ptr);
	void		(*fn_png_destroy_read_struct)	(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr, png_infopp end_info_ptr_ptr);
	void		(*fn_png_init_io)				(png_structp png_ptr, png_FILE_p fp);
	void		(*fn_png_set_sig_bytes)			(png_structp png_ptr, int num_bytes);
	void		(*fn_png_read_info)				(png_structp png_ptr, png_infop info_ptr);
	void		(*fn_png_get_IHDR)				(png_structp png_ptr, png_infop info_ptr, png_uint_32 *width, png_uint_32 *height, int *bit_depth, int *color_type, int *interlace_method, int *compression_method, int *filter_method);
	jmp_buf*	(*fn_png_set_longjmp_fn)		(png_structp png_ptr, png_longjmp_ptr longjmp_fn, size_t jmp_buf_size);

	png_structp	(*fn_png_create_write_struct)	(png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);
	void		(*fn_png_destroy_write_struct)	(png_structpp png_ptr_ptr, png_infopp info_ptr_ptr);
	void		(*fn_png_set_IHDR)				(png_structp png_ptr, png_infop info_ptr, png_uint_32 width, png_uint_32 height, int bit_depth, int color_type, int interlace_method, int compression_method, int filter_method);
	png_voidp	(*fn_png_malloc)				(png_structp png_ptr, png_alloc_size_t size);
	void		(*fn_png_free)					(png_structp png_ptr, png_voidp ptr);
	void		(*fn_png_set_rows)				(png_structp png_ptr, png_infop info_ptr, png_bytepp row_pointers);
	void		(*fn_png_write_png)				(png_structp png_ptr, png_infop info_ptr,    int transforms, png_voidp params);
} png_init_t;
/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      extern functions     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/*******************************************************************************************
 * Static
 ******************************************************************************************/
static png_init_t*	png_getInstance(void)
{
	static png_init_t	*s_pstPNGInit = NULL;

	if (s_pstPNGInit)
	{
		HxSEMT_Get(s_pstPNGInit->ulSemId);

		if (s_pstPNGInit->bEnable)
			return s_pstPNGInit;

		HxSEMT_Release(s_pstPNGInit->ulSemId);

		return NULL;
	} else
	{
		png_init_t	*png = HLIB_MEM_Calloc(sizeof(png_init_t));

		s_pstPNGInit = png;
		HxSEMT_Create(&png->ulSemId, "pngsem", 0);
		HxSEMT_Get(s_pstPNGInit->ulSemId);
		png->pvPNGLibHandle = dlopen("/usr/lib/libpng.so", RTLD_LAZY);
		if (png->pvPNGLibHandle == NULL)
		{
			HxLOG_Critical("[%s](%d) can't find libpng.so in /usr/lib directory!!!\n");
			HxLOG_Assert(0);

			return NULL;
		}
		png->fn_png_sig_cmp				= dlsym(png->pvPNGLibHandle, "png_sig_cmp");
		png->fn_png_create_read_struct	= dlsym(png->pvPNGLibHandle, "png_create_read_struct");
		png->fn_png_create_info_struct	= dlsym(png->pvPNGLibHandle, "png_create_info_struct");
		png->fn_png_destroy_read_struct	= dlsym(png->pvPNGLibHandle, "png_destroy_read_struct");
		png->fn_png_init_io				= dlsym(png->pvPNGLibHandle, "png_init_io");
		png->fn_png_set_sig_bytes		= dlsym(png->pvPNGLibHandle, "png_set_sig_bytes");
		png->fn_png_read_info			= dlsym(png->pvPNGLibHandle, "png_read_info");
		png->fn_png_get_IHDR			= dlsym(png->pvPNGLibHandle, "png_get_IHDR");
		png->fn_png_set_longjmp_fn		= dlsym(png->pvPNGLibHandle, "png_set_longjmp_fn");
		png->fn_png_create_write_struct	= dlsym(png->pvPNGLibHandle, "png_create_write_struct");
		png->fn_png_destroy_write_struct= dlsym(png->pvPNGLibHandle, "png_destroy_write_struct");
		png->fn_png_set_IHDR			= dlsym(png->pvPNGLibHandle, "png_set_IHDR");
		png->fn_png_malloc				= dlsym(png->pvPNGLibHandle, "png_malloc");
		png->fn_png_free				= dlsym(png->pvPNGLibHandle, "png_free");
		png->fn_png_set_rows			= dlsym(png->pvPNGLibHandle, "png_set_rows");
		png->fn_png_write_png			= dlsym(png->pvPNGLibHandle, "png_write_png");

		HxLOG_Assert(png->fn_png_sig_cmp && png->fn_png_create_read_struct && png->fn_png_create_info_struct);
		HxLOG_Assert(png->fn_png_destroy_read_struct && png->fn_png_init_io && png->fn_png_set_sig_bytes);
		HxLOG_Assert(png->fn_png_read_info && png->fn_png_get_IHDR && png->fn_png_set_longjmp_fn);
		HxLOG_Assert(png->fn_png_create_write_struct && png->fn_png_destroy_write_struct);
		HxLOG_Assert(png->fn_png_set_IHDR && png->fn_png_malloc && png->fn_png_free);
		HxLOG_Assert(png->fn_png_set_rows && png->fn_png_write_png);

		png->bEnable = TRUE;
	}

	return s_pstPNGInit;
}


/*******************************************************************************************
 * Interfaces
 ******************************************************************************************/
HERROR	HLIB_PNG_GetFileInfo(const HCHAR *pszFileName, HxPNG_Info_t *pstInfo)
{
	png_init_t	*png = png_getInstance();

	HUINT8	proof_sig_png[8];
	HUINT32	pngWidth = 0;
	HUINT32	pngHeight = 0;
	png_structp 	png_ptr;
	png_infop		png_info;
	png_infop		png_end;
	HINT32 			bit_depth, color_type;
	FILE 			*img_file = NULL;

	HINT32 is_not_png = 0;
	HINT32 readn = 0;

	if (png == NULL)
		return ERR_FAIL;

	HxLOG_Trace();
	if (pszFileName == NULL || pstInfo == NULL)
	{
		HxSEMT_Release(png->ulSemId);
		return ERR_FAIL;
	}

	memset(proof_sig_png, 0, sizeof(proof_sig_png));

	if ((img_file = fopen(pszFileName, "rb")) == NULL)
	{
		HxLOG_Print("can't open %s\n", pszFileName);
		HxSEMT_Release(png->ulSemId);
		return ERR_FAIL;
	}
	readn = (HINT32)fread(proof_sig_png, 1, sizeof(proof_sig_png), img_file);

	is_not_png = PNG_CHECK_SIG(png, proof_sig_png, sizeof(proof_sig_png));
	if (!is_not_png)
	{
		HxLOG_Print("Target image is not PNG format, we can not support \n");
		fclose(img_file);

		HxSEMT_Release(png->ulSemId);
		return ERR_FAIL;
	}

	/* Initialize decoder */
	png_ptr = png->fn_png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		HxLOG_Error("can't make PNG_image ptr\n");
		fclose(img_file);

		HxSEMT_Release(png->ulSemId);
		return ERR_FAIL;
	}
	png_info = png->fn_png_create_info_struct(png_ptr);
	if (png_info == NULL)
	{
		HxLOG_Error("can't make PNG_image info structure\n");
		png->fn_png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(img_file);

		HxSEMT_Release(png->ulSemId);
		return ERR_FAIL;
	}
	png_end = png->fn_png_create_info_struct(png_ptr);
	if (png_end == NULL)
	{
		HxLOG_Error("cant make PNG_image end structure\n");
		png->fn_png_destroy_read_struct(&png_ptr, &png_info, (png_infopp)NULL);
		fclose(img_file);

		HxSEMT_Release(png->ulSemId);
		return ERR_FAIL;
	}

	if(setjmp(png_jmpbuf(png, png_ptr)))
	{
		HxLOG_Error("called setjmp!! something is wrong!!\n");
		png->fn_png_destroy_read_struct(&png_ptr, &png_info, &png_end);
		fclose(img_file);

		HxSEMT_Release(png->ulSemId);
		return ERR_FAIL;
	}

	png->fn_png_init_io(png_ptr, img_file);
	png->fn_png_set_sig_bytes(png_ptr, sizeof(proof_sig_png));

	png->fn_png_read_info(png_ptr, png_info);
	png->fn_png_get_IHDR(png_ptr, png_info, &pngWidth, &pngHeight, &bit_depth, &color_type, NULL, NULL, NULL);

	HxLOG_Print("png image width(%ld) height(%ld)\n", pngWidth, pngHeight);

	pstInfo->nWidth = pngWidth;
	pstInfo->nHeight = pngHeight;
	pstInfo->ucFormat = color_type;
	pstInfo->ucBitPerPixel = bit_depth;

	png->fn_png_destroy_read_struct(&png_ptr, &png_info, &png_end);

	fclose(img_file);

	HxSEMT_Release(png->ulSemId);

	return ERR_OK;
}

HBOOL	HLIB_PNG_WriteFile(const HCHAR *pszFileName, HUINT32 ulWidth, HUINT32 ulHeight, HUINT32 *pulBuffer)
{
	png_init_t	*png = png_getInstance();

	size_t		x, y;
	png_structp	png_ptr = NULL;
	png_infop	info_ptr = NULL;
	png_uint_32	bytes_per_row;
	png_byte	**row_pointers = NULL;
	FILE		*fp;

	if (png == NULL)
		return FALSE;

	fp = fopen(pszFileName, "wb");
	if (fp == NULL || pulBuffer == NULL)
	{
		goto err_rtn;
	}

	/* Initialize the write struct. */
	png_ptr = png->fn_png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		goto err_rtn;
	}

	/* Initialize the info struct. */
	info_ptr = png->fn_png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		png->fn_png_destroy_write_struct(&png_ptr, NULL);
		goto err_rtn;
	}

	/* Set up error handling. */
	if (setjmp(png_jmpbuf(png, png_ptr)))
	{
		png->fn_png_destroy_write_struct(&png_ptr, &info_ptr);
		goto err_rtn;
	}

	/* Set image attributes. */
	png->fn_png_set_IHDR(png_ptr,
						 info_ptr, ulWidth, ulHeight, 8,
						 PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	/* Initialize rows of PNG. */
	bytes_per_row = ulWidth * 4;
	row_pointers = png->fn_png_malloc(png_ptr, ulHeight * sizeof(png_byte *));
	for (y = 0; y < ulHeight; ++y)
	{
		HUINT8 *row = png->fn_png_malloc(png_ptr, ulWidth * sizeof(HUINT8) * 4);
		row_pointers[y] = (png_byte *)row;
		for (x = 0; x < ulWidth; ++x)
		{
			*row++ = (pulBuffer[x + (y * ulWidth)] >> 16) & 0xFF;
			*row++ = (pulBuffer[x + (y * ulWidth)] >> 8) & 0xFF;
			*row++ = (pulBuffer[x + (y * ulWidth)] >> 0) & 0xFF;
			*row++ = (pulBuffer[x + (y * ulWidth)] >> 24) & 0xFF;
		}
	}

	/* Actually write the image data. */
	png->fn_png_init_io(png_ptr, fp);

	png->fn_png_set_rows(png_ptr, info_ptr, row_pointers);

	png->fn_png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	/* Cleanup. */
	for (y = 0; y < ulHeight; y++)
	{
		png->fn_png_free(png_ptr, row_pointers[y]);
	}
	png->fn_png_free(png_ptr, row_pointers);

	/* Finish writing. */
	png->fn_png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fp);

	HxSEMT_Release(png->ulSemId);

	return TRUE;

err_rtn:
	if (fp)
	{
		fclose(fp);
	}

	HxSEMT_Release(png->ulSemId);

	return FALSE;
}

/*********************** End of File ******************************/
