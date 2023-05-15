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
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/23/12 12:36p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /brcm_wince/97400/Private/BMSIPTV/nexus/modules/file/src/wince/nexus_file_posix.h $
 * 
 * 2   2/23/12 12:36p rrlee
 * SW7405-5608: Add unlink
 * 
 * SW7405-5270/2   2/15/12 4:28p rrlee
 * SW7405-5608: Add unlink
 * 
 * SW7405-5270/1   6/1/11 5:36p rrlee
 * SW7405-5270:WinCE build compliance
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

#include "nexus_file_muxio.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int off_t;

struct bfile_io_read_posix {
	struct bfile_io_read self;
	HANDLE hFile;
	bool direct;
};

NEXUS_Error bfile_io_read_posix_open(struct bfile_io_read_posix *file, const char *fname, bool direct);
void bfile_io_read_posix_close(struct bfile_io_read_posix *file);

struct bfile_io_write_posix {
	struct bfile_io_write self;
	bool direct;
	HANDLE hFile;
};

struct bfile_io_mux_posix {
    struct NEXUS_MuxFile self;
	HANDLE hFile;
	bool direct;
    BDBG_OBJECT(bfile_io_mux_posix)
};

NEXUS_Error bfile_io_write_posix_open(struct bfile_io_write_posix *file, const char *fname, bool direct);
NEXUS_Error bfile_io_write_posix_append(struct bfile_io_write_posix *file, const char *fname, bool direct);
void bfile_io_write_posix_close(struct bfile_io_write_posix *file);
off_t bfile_io_write_posix_seek(struct bfile_io_write_posix *file, off_t offset, int whence);
NEXUS_Error b_file_io_mux_posix_open(struct bfile_io_mux_posix *file, const char *fname, unsigned flags, bool direct);

NEXUS_Error bfile_io_posix_unlink(const char *fname);

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_FILE_POSIX_H__ */
