/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
 *  
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *   
 *  Except as expressly set forth in the Authorized License,
 *   
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *   
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *  
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 *  ANY LIMITED REMEDY.
 * 
 * $brcm_Workfile: nexus_file_posix.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 5/3/12 6:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/file/src/linuxuser/nexus_file_posix.h $
 * 
 * 7   5/3/12 6:09p vsilyaev
 * SW7425-2821: Added function to allow run-time control of directIo mode
 * 
 * 6   2/15/12 11:18a erickson
 * SW7405-5609: move unlink system call into posix layer
 * 
 * 5   4/7/11 6:13p vsilyaev
 * SW7425-232: Updated file mux module
 * 
 * 4   4/6/11 6:53p vsilyaev
 * SW7425-232: Removed MuxFileIo API to avoid naming clashes
 * 
 * 3   4/6/11 4:45p vsilyaev
 * SW7425-232: Added MuxFile interface
 * 
 * 2   12/3/08 12:33p vsilyaev
 * PR 49501: Added NEXUS_FileRecord_AppendPosix
 * 
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/2   11/13/07 1:01p vsilyaev
 * PR 37015: Improved use of O_DIRECT in API
 * 
 * Nexus_Devel/1   11/13/07 11:09a vsilyaev
 * PR 37015: Initial checking for timeshifting/FIFO file
  * 
 **************************************************************************/
#ifndef NEXUS_FILE_POSIX_H__
#define NEXUS_FILE_POSIX_H__

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO)  /* VFIO */
#if defined(CONFIG_MEDIA20)
#include "vfio20.h"
#else 
#include "vfio.h"
#endif 
#endif

#include "nexus_file_muxio.h"


#ifdef __cplusplus
extern "C" {
#endif

struct bfile_io_read_posix {
	struct bfile_io_read self;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO) /* VFIO */
	VFIO_Handle_t tHandle;
	bool	isLocalFile;
#else
	int fd;
#endif
	bool direct;
};

NEXUS_Error bfile_io_read_posix_open(struct bfile_io_read_posix *file, const char *fname, bool direct);
void bfile_io_read_posix_close(struct bfile_io_read_posix *file);

struct bfile_io_write_posix {
	struct bfile_io_write self;
	bool direct;
	int fd;
#if defined(HUMAX_PLATFORM_BASE) && (defined(CONFIG_TRANSCODER_MEMIO) || defined(CONFIG_REC_MEMIO))
	int (*cbAddr)(int deviceId, const void *buf, size_t length);
	int deviceId;
#endif
#ifdef FILEIO_TIMING
	struct {
		struct timeval begin, cur, last_good;
		int writes, good_cnt;
	}st;
#endif
};

struct bfile_io_mux_posix {
    struct NEXUS_MuxFile self;
	int fd;
	bool direct;
    BDBG_OBJECT(bfile_io_mux_posix)
};

NEXUS_Error bfile_io_write_posix_open(struct bfile_io_write_posix *file, const char *fname, bool direct);
void bfile_io_write_posix_close(struct bfile_io_write_posix *file);
off_t bfile_io_write_posix_seek(struct bfile_io_write_posix *file, off_t offset, int whence);
NEXUS_Error b_file_io_mux_posix_open(struct bfile_io_mux_posix *file, const char *fname, unsigned flags, bool direct);

NEXUS_Error bfile_io_posix_unlink(const char *fname);

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_FILE_POSIX_H__ */

