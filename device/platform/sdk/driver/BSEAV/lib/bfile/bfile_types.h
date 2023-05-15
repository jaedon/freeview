/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bfile_types.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 8/5/11 12:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bfile/bfile_types.h $
 * 
 * 9   8/5/11 12:35p vsilyaev
 * SW7420-1849: Fixed off_t compatibility issue on Android Bionic C.
 * 
 * SW7420-1849/1   8/5/11 11:38a franktcc
 * SW7420-1849: Fixed off_t compatibility issue on Android Bionic C.
 * 
 * 8   11/15/10 1:03p rrlee
 * SW7405-3705: Remove invalid header in WCE build
 * 
 * 7   9/28/10 11:56a agin
 * SWNOOS-425:  Add typedef for ssize_t for GHS.
 * 
 * 6   6/10/10 1:35p vsilyaev
 * SW7468-69: Fixed Win32/MINGW build
 * 
 * 5   6/2/09 7:38p vsilyaev
 * PR 55417: Added support for read method returning no data or completing
 * with a partial read
 * 
 * 4   6/9/08 5:18p rjlewis
 * PR40352: use standard definition for vxworks.
 * 
 * 3   7/25/07 11:27a vsilyaev
 * PR 32813: Added Windows MINGW support
 * 
 * 2   4/18/06 1:27p rjlewis
 * PR20680: can't use typedef for vxworks -- compile error on multiple
 * defs.
 * 
 * 1   4/7/06 12:32p vsilyaev
 * PR 20680: Added bfile library
 * 
 * 
 ***************************************************************************/
#ifndef __BFILE_TYPES_H__
#define __BFILE_TYPES_H__


#if defined __KERNEL__
/* do nothing */
#elif defined B_REFSW_ANDROID
/* First, get the bionic typedef of off_t out of the way. */
#define off_t __android_off_t
#include <sys/types.h>
/* Now define our own. */
#undef off_t
#define off_t off64_t

#elif defined __unix__
#include <sys/types.h>
#elif defined __vxworks
/**
The vxworks libraries do not support 64 bit off_t, but our reference software
assumes it. As an alternative, we redefine it to 64 bits, but ensure that we 
don't exceed 32 bit values when making vxworks system calls.
**/
/* First, get the vxworks typedef of off_t out of the way. */
#define off_t vxworks_off_t
#include <vxWorks.h>
/* Now define our own. */
#undef off_t
#define off_t int64_t

#elif defined(_WIN32_WCE)
/* trap WIN32 case below */
#elif defined(__MINGW32__)
#include <sys/types.h>
typedef long long off_t;
#elif defined(WIN32)
/* First, get the win32 typedef of off_t out of the way. */
#define off_t win32_off_t
/* include all windows */
#include <sys/types.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
typedef long ssize_t;
/* Now define our own. */
#undef off_t
typedef int64_t off_t;
#elif defined(GHS)
typedef int ssize_t;
#endif

/* define whence if O/S would define different warning it would case a warning */
/* we don't want to include stdio.h or unistd.h to avoid namespace pollution */
#define SEEK_SET       0
#define SEEK_CUR       1
#define SEEK_END       2

#define BFILE_ERROR_UNKNOWN    (-1)
/* error code returned by the read functions if there is temporary no data in the file, 35 to match EAGAIN */
#define BFILE_ERROR_NO_DATA    (-35)

   
#endif /* __BFILE_TYPES_H__ */


