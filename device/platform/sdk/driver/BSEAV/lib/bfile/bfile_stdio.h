/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bfile_stdio.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 1/28/11 2:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bfile/bfile_stdio.h $
 * 
 * 5   1/28/11 2:36p jtna
 * SW7422-232: merge Android changes
 * 
 * 4   8/21/08 4:18p rjlewis
 * PR32813: vxworks too.
 * 
 * 3   8/12/08 7:07p vsilyaev
 * PR 32813: Fixed MINGW support
 * 
 * 2   7/10/07 12:45p erickson
 * PR32813: file compilation issues while integrating with Brutus
 * 
 * 1   4/7/06 12:32p vsilyaev
 * PR 20680: Added bfile library
 * 
 ***************************************************************************/
#ifndef __BFILE_STDIO_H__
#define __BFILE_STDIO_H__
#include "bfile_io.h"
#include <stdio.h>

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO) 
#if defined(CONFIG_MEDIA20)
#include "vfio20.h"
#else 
#include "vfio.h"
#endif 
#endif

#if defined(_WIN32) || defined(__vxworks) || defined (B_REFSW_ANDROID)
#define fopen64 fopen
#define fseeko fseek
#define ftello ftell
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO)
bfile_io_read_t  bfile_stdio_read_attach(VFIO_Handle_t tHandle);
#else
bfile_io_read_t bfile_stdio_read_attach(FILE *fin);
#endif
void bfile_stdio_read_detach(bfile_io_read_t file);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO)
/* TODO :: comment by junsy */
/* omit below function in VFIO */
#else
bfile_io_write_t bfile_stdio_write_attach(FILE *fout);
void bfile_stdio_write_detach(bfile_io_read_t file);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BFILE_STDIO_H__ */

