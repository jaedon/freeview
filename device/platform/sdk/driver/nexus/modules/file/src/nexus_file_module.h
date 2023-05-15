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
 * $brcm_Workfile: nexus_file_module.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 8/28/12 3:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/file/src/nexus_file_module.h $
 * 
 * 6   8/28/12 3:24p erickson
 * SW7346-994: don't rely on bcmplayer BNAV_Version detection because it's
 *  not possible for file fifos with AVC
 * 
 * 5   9/6/11 10:10a erickson
 * SWNOOS-482: correct internal spelling
 * 
 * 4   4/6/11 6:53p vsilyaev
 * SW7425-232: Removed MuxFileIo API to avoid naming clashes
 * 
 * 3   9/22/10 6:56p vsilyaev
 * SW7405-4419: Include all API into the module file
 * 
 * 2   4/22/10 6:57p mphillip
 * SW7405-4251: Merge chunked PVR support from branch to main
 * 
 * SW7405-3587/1   4/15/10 11:31a mphillip
 * SW7405-3587: Update with header changes
 * 
 * 1   1/18/08 2:16p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/4   11/13/07 11:12a vsilyaev
 * PR 37015: Initial checking for timeshifting/FIFO file
 * 
 * Nexus_Devel/3   10/11/07 6:24p vsilyaev
 * PR 35824: Fixed naming
 * 
 * Nexus_Devel/2   10/10/07 4:03p vsilyaev
 * PR 35824: Added syncthunk support
 * 
 * Nexus_Devel/1   10/10/07 3:57p vsilyaev
 * PR 35824: File I/O module
 * 
 **************************************************************************/
#ifndef NEXUS_FILE_MODULE_H__
#define NEXUS_FILE_MODULE_H__

#include "nexus_base.h"
#include "nexus_file_thunks.h"
#include "nexus_file_init.h"
#include "nexus_file.h"
#include "nexus_file_fifo.h"
#include "nexus_file_chunk.h"
#include "nexus_file_fifo_chunk.h"
#include "nexus_file_pvr.h"
#include "nexus_file_muxio.h"
#include "bcmindexer.h"
#include "bcmindexer_nav.h"

#ifdef __cplusplus
extern "C" {
#endif

extern NEXUS_ModuleHandle NEXUS_FileModule;
#define NEXUS_MODULE_SELF NEXUS_FileModule
NEXUS_Error NEXUS_File_P_Scheduler_Start(const NEXUS_FileModuleSettings  *settings);
void NEXUS_File_P_Scheduler_Stop(void);
int NEXUS_P_ReadFirstNavEntry(bfile_io_read_t f, BNAV_Entry *pNavEntry);

#ifdef __cplusplus
}
#endif


#endif /* NEXUS_FILE_MODULE_H__ */
