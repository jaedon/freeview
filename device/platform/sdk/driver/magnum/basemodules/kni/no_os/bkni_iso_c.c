/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni_iso_c.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 5/13/11 2:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/kni/no-os/bkni_iso_c.c $
 * 
 * Hydra_Software_Devel/4   5/13/11 2:44p agin
 * SWNOOS-458:  Fix compiler warnings.
 * 
 * Hydra_Software_Devel/3   5/12/11 6:03p agin
 * SWNOOS-458:  Fix compiler warnings.
 * 
 * Hydra_Software_Devel/2   4/22/10 7:19p agin
 * SWNOOS-405:  Fix MIPS exception for BE build using linux cross-
 * compiler.
 * 
 * Hydra_Software_Devel/1   1/6/04 7:09p brianlee
 * PR8921: Initial version for non-os.
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
/* needed to support tagged interface */
#undef BKNI_Malloc
#undef BKNI_Free
#include <string.h>
#include <stdlib.h>

void *BKNI_Memset(void *b, int c, size_t len)
{
#if 1	/*bwl*/
	int	i;
	for (i=0; i<(int)len; i++)
		*((uint8_t *)b + i) = c;
	return b;
#else
    return memset(b, c, len);
#endif
}

void *BKNI_Memcpy(void *dst, const void *src, size_t len)
{
#if 1	/*bwl*/
	int	i;
	for (i=0; i<(int)len; i++)
		*((uint8_t *)dst + i) = *((uint8_t *)src + i);
	return dst;
#else
    return memcpy(dst, src, len);
#endif
}

int BKNI_Memcmp(const void *b1, const void *b2, size_t len)
{
    return memcmp(b1, b2, len);
}

void *BKNI_Memchr(const void *b, int c, size_t len)
{
    return memchr(b, c, len);
}

void *BKNI_Memmove(void *dst, const void *src, size_t len)
{
    return memmove(dst, src, len);
}

#if BKNI_TRACK_MALLOCS
void * BKNI_Malloc_tagged(
	size_t size,
	const char *file,
	unsigned line)
{
	void *ptr = malloc(size);
	printf("BKNI_Malloc:  ptr=%p, size=%d, file=%s, line=%d\n", ptr, size, file, line);
	return ptr;

}
#else
void * BKNI_Malloc(size_t size)
{
	return malloc(size);
}
#endif

#if BKNI_TRACK_MALLOCS
void BKNI_Free_tagged(
	void *mem,			/* Pointer to memory allocated by BKNI_Malloc */
	const char *file,
	unsigned line
	)
{
	printf("BKNI_Free:  ptr=%p, file=%s, line=%d\n", mem, file, line);
    free(mem);
    return;
}
#else
void BKNI_Free(void *ptr)
{
    free(ptr);
    return;
}
#endif



