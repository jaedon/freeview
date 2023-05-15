#include <stdarg.h>
#include "vkernel.h"
//#include "uocto.h"

#include "LoaderCoreSPI_Stdlib.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup loadercorespis Loader Core SPIs
 *  Loader Core SPIs
 *
 *
 *  @{
 */

/** @defgroup stdlib Loader Core Standard Library
 *  Loader Core Standard Library Functions
 *
 * These functions are a direct mapping of the corresponding ANSI C runtime libraries.
 * In cases where these functions are already available on the platform, they can be used directly.
 * On platforms where full ANSI C runtime support is not provided, they need to be implemented manually.
 *  @{
 */

/**
 * CRT memcpy function
 */
void *
LoaderCoreSPI_Stdlib_memcpy(
    void *pDest,
    const void *pSource,
    lc_uint32 count)
{
	// TODO:  VK Conversion.
//	return HxSTD_memcpy(pDest, pSource, count);
	return VK_memcpy(pDest, pSource, count);
}

/**
 * CRT memset function
 */
void *
LoaderCoreSPI_Stdlib_memset(
    void *dest,
    lc_uchar c,
    lc_uint32 count)
{
//	return HxSTD_memset(dest, c, count);
	return VK_memset(dest, c, count);
}

/**
 * CRT memcmp function
 *
 * @param[in] pBuf1 Pointer to block of memory.
 * @param[in] pBuf2 Pointer to block of memory.
 * @param[in] len Number of bytes to compare.
 * @retval
 *     Returns an integral value indicating the relationship between the content of the memory blocks:
 *     A zero value indicates that the contents of both memory blocks are equal.
 *     A value greater than zero indicates that the first byte that does not match in both memory blocks has a greater value in ptr1 than in ptr2 as if evaluated as unsigned char values;
 *     And a value less than zero indicates the opposite.
 */
lc_sint32
LoaderCoreSPI_Stdlib_memcmp(
    const lc_uchar *pBuf1,
    const lc_uchar *pBuf2,
    lc_uint32 len)
{
//	return HxSTD_memcmp(pBuf1, pBuf2, len);
	return VK_memcmp(pBuf1, pBuf2, len);
}

/**
 * CRT printf function
 *
 * @param[in] pFormat C string that contains the text and format to be written to the buffer.
 * @retval
 *      On success, the total number of characters written is returned.
 *      On failure, a negative number is returned.
 */
lc_sint32
LoaderCoreSPI_Stdlib_printf(
    const lc_char *pFormat,
    ...)
{
	va_list		args;
	lc_char debugBuf[512+1]={0};
	lc_sint32 		ret;

	va_start(args, pFormat);
	VK_vsprintf(debugBuf, pFormat, args);
	va_end(args);

	ret = VK_printf("LC:PRT:%s\n", debugBuf);

	return ret;
}


/** @} */

/** @} */ /* End of Loader Core SPIs */

#ifdef __cplusplus
}
#endif



