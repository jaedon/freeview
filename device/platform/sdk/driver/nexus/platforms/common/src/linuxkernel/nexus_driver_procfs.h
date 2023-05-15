/***************************************************************************
*     (c)2009-2011 Broadcom Corporation
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
* $brcm_Workfile: nexus_driver_procfs.h $
* $brcm_Revision: 4 $
* $brcm_Date: 1/27/11 4:45p $
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/97400/src/linuxkernel/nexus_driver_procfs.h $
* 
* 4   1/27/11 4:45p mphillip
* SW7425-63: Update procfs interface to allow larger outputs and to work
*  on newer kernels
* 
* 3   8/4/09 1:15p gmohile
* PR 56512 : Fix kernel mode build for 7401/03
* 
* 2   6/24/09 2:58p mphillip
* PR54926: Lock module before calling dbgPrint function
* 
* 1   6/23/09 11:55p mphillip
* PR54926: Add procfs files
* 
*
***************************************************************************/

#ifndef NEXUS_DRIVER_PROCFS_H
#define NEXUS_DRIVER_PROCFS_H
#ifdef CONFIG_PROC_FS

#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>

extern struct proc_dir_entry* brcm_proc_entry;

#endif

/* When used to pass an array of name/value pairs, the last entry in the 
 * array will have a zero-length name to indicate it is the final entry. */ 

typedef struct proc_pair {
    const char *name;
    const char *value;
} proc_pair_t;

int nexus_driver_proc_init(void);
void nexus_driver_proc_done(void);

int nexus_driver_proc_register_status(const char *filename, NEXUS_ModuleHandle handle, const char *dbg_module_name, void (*callback)(void *context), void *context, int dbg_buf_size);
int nexus_driver_proc_register_config(const char *filename, NEXUS_ModuleHandle handle, void (*read_callback)(void **pair_list, void *read_context), void *read_context, void (*write_callback)(void **pair_list, void *write_context), void *write_context);

int nexus_driver_proc_unregister_status(const char *filename);
int nexus_driver_proc_unregister_config(const char *filename);

#endif /* !defined NEXUS_DRIVER_PROCFS_H */
