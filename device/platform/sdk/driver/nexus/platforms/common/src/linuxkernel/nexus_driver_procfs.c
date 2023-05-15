/***************************************************************************
 *     (c)2009-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_driver_procfs.c $
 * $brcm_Revision: 12 $
 * $brcm_Date: 9/24/12 11:16a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/common/src/linuxkernel/nexus_driver_procfs.c $
 * 
 * 12   9/24/12 11:16a erickson
 * SW7435-369: add BDBG_WRN for user feedback, allow "name=" to
 *  /proc/brcm/config to clear SetEnv
 * 
 * 11   2/1/12 11:38a erickson
 * SW7425-2231: fix shutdown
 * 
 * 10   5/3/11 5:09p erickson
 * SW7420-1819: repartition so NFE is a standard feature
 * 
 * 9   4/19/11 5:36p nickh
 * SW7425-365: Add 2.6.37 kernel support
 * 
 * 8   1/27/11 4:45p mphillip
 * SW7425-63: Update procfs interface to allow larger outputs and to work
 *  on newer kernels
 * 
 * 7   9/23/10 2:16p erickson
 * SW7420-943: refactor #includes
 *
 * 6   8/12/10 1:38p ttrammel
 * SW7420-943: Merge NFE to main branch.
 *
 * NFE/1   5/11/10 11:34a ttrammel
 * SW7405-4315: Initial NFE check-in.
 *
 * 5   9/16/09 5:10p mward
 * SW7400-2524: procfs is not useful in non-debug build, don't create it.
 *
 * 4   9/11/09 12:26p erickson
 * SW7420-228: temp remove proc fs for linux 2.6.30. fix warnings.
 *
 * 3   8/5/09 4:46p gqli
 * PR57347: Fix for Kernel mode driver reports error when removed via
 *  rmmod
 *
 * 2   6/24/09 2:58p mphillip
 * PR54926: Lock module before calling dbgPrint function
 *
 * 1   6/23/09 11:55p mphillip
 * PR54926: Add procfs files
 *
 *
 ************************************************************/
#include "bstd.h"
#include "nexus_base_types.h"

/* TODO: rework this file for NFE */
#ifndef NEXUS_BASE_OS_bare
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
#define USE_LEGACY_PROCFS 1
#include <linux/config.h>
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include "nexus_driver_procfs.h"
#include "bdbg.h"
#endif

BDBG_MODULE(nexus_driver_procfs);

#if defined(CONFIG_PROC_FS) && BDBG_DEBUG_BUILD 
#define NEXUS_DEBUG_PROCFS 1
#endif

#if NEXUS_DEBUG_PROCFS
struct proc_dir_entry *brcm_dir_entry;          /* root brcm proc directory */
struct proc_dir_entry *brcm_debug_entry;        /* brcm/debug proc file */
struct proc_dir_entry *brcm_config_entry;       /* brcm/config proc file */
#endif



#if NEXUS_DEBUG_PROCFS
typedef
struct brcm_bdbg_context {
    char *buf;
    int *len;
    int bufsize;
    char *modules;
    int *modules_len;
} brcm_bdbg_context;


static const char *lvl_str[BDBG_P_eLastEntry] = {
        "unk", "trc", "msg", "wrn", "err"
};

void brcm_bdbg_callback(void *ctx, const char *module, BDBG_Instance instance, const char *inst_name)
{
    brcm_bdbg_context *context = (brcm_bdbg_context *)ctx;
    int l;

    /* We can't do the lookup here, so simply append all the module names
     * into a buffer for later lookup */
    l = snprintf(context->modules, context->bufsize - *context->modules_len, "%s,",module); *context->modules_len += l; context->modules += l;

}

static void brcm_bdbg_fetch_state(char *buf, int *len, int bufsize)
{
    brcm_bdbg_context ctx;
    char modules[bufsize];
    int modules_len = 0;
    int l = 0;
    char *p;

    ctx.buf = buf;
    ctx.len = len;
    ctx.bufsize = bufsize;
    ctx.modules = modules;
    ctx.modules_len = &modules_len;
    BDBG_EnumerateAll(brcm_bdbg_callback, &ctx);

    modules_len--;
    modules[modules_len] = 0;
    p = modules;
    while (p && *p) {
        BDBG_Level lvl;
        char *q = strpbrk(p,",");
        char *m = p;
        p = q;
        if (q) {
            *q = 0;
            p++;
        }
        BDBG_GetModuleLevel(m,&lvl);
        l = snprintf(buf, bufsize - *len, "%s\t%s\n",m,lvl_str[lvl]); *len += l; buf += l;
    }
}


static int brcm_bdbg_read(char *buf, char **start, off_t off,
                          int bufsize, int *eof, void *unused)
{
    int len = 0;

    if (off > 0) return 0;

    brcm_bdbg_fetch_state(buf, &len, bufsize);

    return len;
}

static int brcm_bdbg_write(struct file * file, const char *buffer, unsigned long count,
    void *data)
{
#define WHITESPACE " \t="
    char *endofmodule, *level;
#define MAX 64
    char name[MAX+1];
    int debug_level;
    int rc;

    if (copy_from_user(name, buffer, count))
    {
        count = -1; /* do not flush buffer, bad copy from user */
        goto end;
    }

    if (count && name[count] == '\n')
        name[count - 1] = 0;
    else
        name[count] = 0;

    endofmodule = strpbrk(name, WHITESPACE);
    if (!endofmodule) {
        printk("No level specified\n");
        goto end;
    }
    *endofmodule++ = 0;
    level = endofmodule + strspn(endofmodule, WHITESPACE);

    if (!strnicmp(level, "err", 3))
        debug_level = BDBG_eErr;
    else if (!strnicmp(level, "wrn", 3))
        debug_level = BDBG_eWrn;
    else if (!strnicmp(level, "msg", 3))
        debug_level = BDBG_eMsg;
    else if (!strnicmp(level, "none", 4))
        debug_level = BDBG_eErr;
    else {
        printk("Invalid level. Should be: err, wrn, msg or none.\n");
        goto end;
    }

    rc = BDBG_SetModuleLevel(name, debug_level);

end:
    return count;
}
#endif

#if NEXUS_DEBUG_PROCFS
static int brcm_proc_config_read(char *buf, char **start, off_t off,
                          int bufsize, int *eof, void *unused)
{
    /* currently no way to read the full list */
    return 0;
}

static int brcm_proc_config_write(struct file * file, const char *buffer, unsigned long count,
    void *data)
{
#define WHITESPACE " \t\n="
    char *value;
#define MAX 64
    char name[MAX+1];

    if (copy_from_user(name, buffer, count))
    {
        count = -1; /* do not flush buffer, bad copy from user */
        goto end;
    }
    if (count && name[count] == '\n')
        name[count - 1] = 0;
    else
        name[count] = 0;

    if (count && name[count] == '\n')
        name[count - 1] = 0;
    else
        name[count] = 0;

    value = strpbrk(name, WHITESPACE);
    /* NULL is OK to clear the value */
    if (value) {
        *value++ = 0;
        
        /* allow "name=" and "name" to clear */
        if (*value == '\n' || *value == 0) value = NULL;
    }

    if (*name) {
        BDBG_WRN(("calling NEXUS_SetEnv(%s,%s)", name, value));
        NEXUS_SetEnv(name, value);
    }

end:
    return count;
}
#endif

int nexus_driver_proc_init()
{
#if NEXUS_DEBUG_PROCFS

    /* Root directory */
#if USE_LEGACY_PROCFS
    brcm_dir_entry = proc_mkdir("brcm", &proc_root);
#else
    brcm_dir_entry = proc_mkdir("brcm", NULL);
#endif
    if (!brcm_dir_entry) goto error;

    brcm_debug_entry = create_proc_entry("debug", S_IFREG|S_IRUGO, brcm_dir_entry);
    if (!brcm_debug_entry) goto error;
    brcm_debug_entry->read_proc = brcm_bdbg_read;
    brcm_debug_entry->write_proc = brcm_bdbg_write;

    brcm_config_entry = create_proc_entry("config", S_IFREG|S_IRUGO, brcm_dir_entry);
    if (!brcm_config_entry) goto error;
    brcm_config_entry->read_proc = brcm_proc_config_read;
    brcm_config_entry->write_proc = brcm_proc_config_write;

    return 0;

error:
    nexus_driver_proc_done();
    return -1;
#else
    return 0;
#endif
}

typedef struct nexus_driver_proc_data {
    const char *dbg_modules;
    const char *filename;
    void (*callback)(void *context);
    void *context;
    NEXUS_ModuleHandle handle;
    void *dbg_buf;
    int dbg_buf_size;
    int dbg_buf_used;
} nexus_driver_proc_data;

#if NEXUS_DEBUG_PROCFS
static int proc_data_count = 0;
static int proc_data_max = 16;
static nexus_driver_proc_data g_proc_data[16] = {{NULL, NULL, NULL, NULL, NULL, NULL, 0}};
#endif

void nexus_driver_proc_done()
{
#if NEXUS_DEBUG_PROCFS
    int i;
    if (brcm_debug_entry) {
        remove_proc_entry("debug", brcm_dir_entry);
    }

    if (brcm_config_entry) {
        remove_proc_entry("config", brcm_dir_entry);
    }

    for (i=0; i < proc_data_max; i++) {
        if ( g_proc_data[i].filename !=NULL) {
            remove_proc_entry(g_proc_data[i].filename, brcm_dir_entry);
        }
    }

    if (brcm_dir_entry) {
        remove_proc_entry("brcm", NULL);
    }
#endif
}

typedef
struct brcm_bdbgprint_context {
    char *buf;
    int *len;
    int bufsize;
} brcm_bdbgprint_context;

#if NEXUS_DEBUG_PROCFS
static brcm_bdbgprint_context g_cur_context;

static void
brcm_proc_dbgprint_capture(
        BDBG_ModulePrintKind kind, /* type of the output */
        BDBG_Level level, /* level of the debug output */
        const BDBG_DebugModuleFile *module, /* pointer to the debug module */
        const char *fmt,  /* format */
        va_list ap /* variable list of arguments */
)
{
    int l = 0;
    if ((level != BDBG_eTrace) && (kind != BDBG_ModulePrintKind_eHeader)) {
        l = vsnprintf(g_cur_context.buf, g_cur_context.bufsize, fmt, ap);
        g_cur_context.buf += l; g_cur_context.bufsize -= l; *g_cur_context.len += l;
        l = vsnprintf(g_cur_context.buf, g_cur_context.bufsize, "\n", NULL);
        g_cur_context.buf += l; g_cur_context.bufsize -= l; *g_cur_context.len += l;
    }
}

static int brcm_proc_dbgprint_read(char *buf, char **start, off_t off,
                          int bufsize, int *eof, void *data)
{
    int len = 0;
    nexus_driver_proc_data *p = (nexus_driver_proc_data *)data;

    if (off < 0) {
        /* This shouldn't ever happen */
        goto clear_buf;
    }
    if (off > 0) {
        int remainder = 0;
        if (!p->dbg_buf) { goto clear_buf; }
        if (off > p->dbg_buf_used) { goto clear_buf; }
        remainder = p->dbg_buf_used - off;
        if (remainder <= 0) { goto clear_buf; }
        if (remainder > bufsize) {
            remainder = bufsize;
        } else {
            *eof = 1;
        }
        BKNI_Memcpy(buf, ((unsigned char*)p->dbg_buf) + off, remainder);
        *start = buf;
        return remainder;
    }

    if (p->dbg_buf) {
        g_cur_context.buf = p->dbg_buf;
        g_cur_context.bufsize = p->dbg_buf_size;
    } else {
        g_cur_context.buf = buf;
        g_cur_context.bufsize = bufsize;
    }
    g_cur_context.len = &len;
    BDBG_SetModulePrintFunction(p->dbg_modules,brcm_proc_dbgprint_capture);
    NEXUS_Module_Lock(p->handle);
    p->callback(p->context);
    NEXUS_Module_Unlock(p->handle);
    BDBG_SetModulePrintFunction(p->dbg_modules,NULL);
    if (p->dbg_buf) {
        p->dbg_buf_used = len;
        if (len > bufsize) {
            len = bufsize;
        } else {
            *eof = 1;
        }
        BKNI_Memcpy(buf, p->dbg_buf, len);
        *start = buf;
    } else {
        *eof = 1;
    }
    return len;

clear_buf:
    p->dbg_buf_used = 0;
    *eof = 1;
    return 0;
}
#endif

int nexus_driver_proc_register_status(const char *filename, NEXUS_ModuleHandle handle, const char *dbg_module_name, void (*callback)(void *context), void *context, int dbg_buf_size)
{
#if NEXUS_DEBUG_PROCFS
    struct proc_dir_entry *p;
    int i;
    if (proc_data_count > proc_data_max) return -1;
    for (i=0; i < proc_data_max; i++) {
        if (!g_proc_data[i].filename) {
            g_proc_data[i].filename = filename;
            g_proc_data[i].dbg_modules = dbg_module_name;
            g_proc_data[i].callback = callback;
            g_proc_data[i].context = context;
            g_proc_data[i].handle = handle;
            g_proc_data[i].dbg_buf = NULL;
            g_proc_data[i].dbg_buf_size = dbg_buf_size;
            if (dbg_buf_size > 0) {
                g_proc_data[i].dbg_buf = BKNI_Malloc(dbg_buf_size);
            }
            break;
        }
    }
    p = create_proc_entry(filename, S_IFREG|S_IRUGO, brcm_dir_entry);
    if (!p) { g_proc_data[i].filename = NULL; return -1; }
    p->read_proc = brcm_proc_dbgprint_read;
    p->data = &g_proc_data[i];

#endif
    return 0;
}

int nexus_driver_proc_register_config(const char *filename, NEXUS_ModuleHandle handle, void (*read_callback)(void **pair_list, void *read_context), void *read_context, void (*write_callback)(void **pair_list, void *write_context), void *write_context)
{
#if NEXUS_DEBUG_PROCFS
#endif
    return 0;
}

int nexus_driver_proc_unregister_status(const char *filename)
{
#if NEXUS_DEBUG_PROCFS
    int i;
    for (i=0; i < proc_data_max; i++) {
        if (g_proc_data[i].filename && !strnicmp(g_proc_data[i].filename, filename, strlen(filename))) {
            g_proc_data[i].filename = NULL;
            g_proc_data[i].dbg_modules = NULL;
            g_proc_data[i].callback = NULL;
            g_proc_data[i].context = NULL;
            if (g_proc_data[i].dbg_buf) {
                BKNI_Free(g_proc_data[i].dbg_buf);
                g_proc_data[i].dbg_buf = NULL;
            }
            g_proc_data[i].dbg_buf_size = 0;
            break;
        }
    }
    if (i < proc_data_max) {
        remove_proc_entry(filename, brcm_dir_entry);
    }
#endif
    return 0;
}

int nexus_driver_proc_unregister_config(const char *filename)
{
#if NEXUS_DEBUG_PROCFS
    remove_proc_entry(filename, brcm_dir_entry);
#endif
    return 0;
}

