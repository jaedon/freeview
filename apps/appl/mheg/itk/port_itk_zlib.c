
/**
	@file	  port_itk_zlib.c													\n
	@brief	  itk file for ZLIB porting layer									\n

	Description: ITK에서 ZLIB inflate Porting 에 사용되는 기능을 정의한 Body파일  		\n
	Module: MW/mheg	/itk								 						\n

	Copyright (c) 2008 HUMAX Co., Ltd.											\n
	All rights reserved.														\n

	[Description]

*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include "zlib.h"
#include "port_itk_zlib.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/**
 * Implementation of ITK interface to zlib inflation.
 */
typedef struct itk_zlib_impl_t
{
    itk_zlib_inflator_t		itk_zlib;    		   /**< The public zlib interface */
    void* 					context;               /**< Opaque pointer for alloc, free functions */
    itk_zlib_alloc_fn_t 	alloc;
    itk_zlib_free_fn_t 		free;
    z_stream 				zstream;              /**< The zlib stream object */

} itk_zlib_impl_t;




/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static itk_zlib_t				s_stZlibObj;



/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/
static itk_zlib_impl_t* 		local_itk_zlibImpl(itk_zlib_inflator_t * zlib);
static voidpf 					local_itk_zalloc(voidpf opaque, uInt items, uInt size);
static void 					local_itk_zfree(voidpf opaque, voidpf address);
static void 					mheg_itk_port_zlibRelease(itk_zlib_inflator_t * zlib);
static itk_zlib_inflate_ret_t 	mheg_itk_port_zlibInflate(itk_zlib_inflator_t * zlib,
         											itk_uint8_t const * src, size_t src_size, itk_uint8_t * dst, size_t dst_size);


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/



/** Cast from public interface to private implementation pointer. */
static itk_zlib_impl_t * local_itk_zlibImpl(itk_zlib_inflator_t * zlib)
{
    return (itk_zlib_impl_t *)zlib;
}

/** Zlib allocator function */
static voidpf local_itk_zalloc(voidpf opaque, uInt items, uInt size)
{
    itk_zlib_impl_t * zlib_impl = (itk_zlib_impl_t *)opaque;
    return zlib_impl->alloc(zlib_impl->context, items * size);
}

/** Zlib free function */
static void local_itk_zfree(voidpf opaque, voidpf address)
{
    itk_zlib_impl_t * zlib_impl = (itk_zlib_impl_t *)opaque;
    zlib_impl->free(zlib_impl->context, address);
}

/** mheg_itk_port_zlibRelease a zlib interface object */
static void mheg_itk_port_zlibRelease(itk_zlib_inflator_t * zlib)
{
    itk_zlib_impl_t * zlib_impl = local_itk_zlibImpl(zlib);
    inflateEnd(&zlib_impl->zstream);
    zlib_impl->free(zlib_impl->context, zlib_impl);
}

/** Inflate src data into dst buffer */
static itk_zlib_inflate_ret_t mheg_itk_port_zlibInflate(itk_zlib_inflator_t * zlib,
         						itk_uint8_t const * src, size_t src_size, itk_uint8_t * dst, size_t dst_size)
{
    int zret = 0;
    itk_zlib_inflate_ret_t ret = { ITKE_OK, 0, 0 };
    itk_zlib_impl_t * zlib_impl = local_itk_zlibImpl(zlib);
    z_stream * zstream = &zlib_impl->zstream;

    zstream->next_in = (Bytef *)src;
    zstream->avail_in = (uInt)src_size;
    zstream->next_out = dst;
    zstream->avail_out = (uInt)dst_size;

    /* Flush all input data. Positive return codes are not errors
       (e.g. Z_STREAM_END). */
    if ((zret = inflate(zstream, Z_SYNC_FLUSH)) >= Z_OK)
    {
        ret.src_read = (uInt)src_size - zstream->avail_in;
        ret.dst_written = (uInt)dst_size - zstream->avail_out;
    }
    else
    {
        ret.error = zret == Z_MEM_ERROR ? ITKE_NO_RESOURCE : ITKE_FAIL;
    }
    return ret;
}


#define _____PUBLIC_APIs________________________________________________________________


/**
 * Create an ITK zlib implementation.
 */
itk_zlib_inflator_t* PORT_ITK_ZLIB_newInflator(itk_zlib_t * unused, itk_zlib_alloc_fn_t alloc_fn, itk_zlib_free_fn_t free_fn, void * context)
{
    itk_zlib_impl_t* zlib_impl = alloc_fn(context, sizeof(itk_zlib_impl_t));

    (void)unused;
    if (zlib_impl != NULL)
    {
        z_stream * zstream 	= &zlib_impl->zstream;
        zlib_impl->context 	= context;
        zlib_impl->alloc 	= alloc_fn;
        zlib_impl->free 	= free_fn;
        zstream->zalloc 	= local_itk_zalloc;
        zstream->zfree 		= local_itk_zfree;
        zstream->opaque 	= zlib_impl;

        if (inflateInit(zstream) == Z_OK)
        {
            /* Set the public interface */
            zlib_impl->itk_zlib.release = mheg_itk_port_zlibRelease;
            zlib_impl->itk_zlib.zinflate = mheg_itk_port_zlibInflate;
        }
        else
        {
            free_fn(context, zlib_impl);
            zlib_impl = NULL;
        }
    }

    return (itk_zlib_inflator_t *)zlib_impl;
}



void PORT_ITK_ZLIB_CreateObject()
{
	s_stZlibObj.newInflator = PORT_ITK_ZLIB_newInflator;
	ITK_registerManager(ITK_ZLIB, &s_stZlibObj);
}

