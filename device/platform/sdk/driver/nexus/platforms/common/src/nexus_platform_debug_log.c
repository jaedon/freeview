/***************************************************************************
*     (c)2011-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_platform_debug_log.c $
* $brcm_Revision: 6 $
* $brcm_Date: 7/24/12 4:10p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/linuxuser/nexus_platform_debug_log.c $
* 
* 6   7/24/12 4:10p vsilyaev
* SW7425-3488: Allow to disable debug log for the nexus kernel mode
* 
* 5   8/18/11 3:18p jrubio
* SW7346-459: add coverity tags for false positives
* 
* 4   8/9/11 12:50p vsilyaev
* SW7420-1465: Use smaller buffer size for the kernel
* 
* 3   8/4/11 8:02p vsilyaev
* SW7420-1465: Added protocol to ensure that all buffered debug output
*  gets written out
* 
* 2   8/3/11 6:03p vsilyaev
* SW7420-1465: In default configuration kernel/proxy mode should route
*  debug output through FIFO
* 
* 1   8/3/11 5:19p vsilyaev
* SW7420-1465: Module to control external debug logging process
* 
*
***************************************************************************/
#include "nexus_platform.h"
#include "nexus_platform_priv.h"
#include "nexus_platform_debug_log.h"
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>

BDBG_MODULE(nexus_platform_debug_log);

void NEXUS_Platform_P_DebugLog_Init(NEXUS_Platform_P_DebugLog *debugLog, const char *driver)
{
    BDBG_Fifo_CreateSettings logSettings;
    const char *debug_log_size;
    static const char fname[] = "/tmp/nexus.log";
    const char *logger = "./logger";
    int urc;
    BERR_Code rc;
    struct stat st;

    /* coverity[fs_check_call: FALSE] */
    urc = lstat(logger, &st);
    if(urc<0) {goto err_logger;}

    BKNI_Memcpy(debugLog->fname, fname, sizeof(fname));
    BDBG_Fifo_GetDefaultCreateSettings(&logSettings);
    BDBG_Log_GetElementSize(&logSettings.elementSize);
    logSettings.nelements = 0;
    if(driver) { /* enable by default if using kernel mode driver */
        logSettings.nelements = 1024; 
    }
    debug_log_size = NEXUS_GetEnv("debug_log_size");
    if (debug_log_size) {
        logSettings.nelements = 2*NEXUS_atoi(debug_log_size);
        if(logSettings.nelements && logSettings.nelements<8) {
            logSettings.nelements = 2048;
        }
    }
    if(logSettings.nelements==0) {
        goto err_elements;
    }
    debugLog->logSize = logSettings.nelements;
    unlink(debugLog->fname);
    debugLog->fd = open(fname, O_RDWR|O_CREAT|O_TRUNC, 0666);
    if(debugLog->fd<0) { (void)BERR_TRACE(NEXUS_OS_ERROR);goto err_fd;}
    fcntl(debugLog->fd, FD_CLOEXEC, 1);
    debugLog->bufferSize = (logSettings.nelements+1) *  logSettings.elementSize;

    urc = ftruncate(debugLog->fd, debugLog->bufferSize);
    if(urc<0) { (void)BERR_TRACE(NEXUS_OS_ERROR);goto err_truncate;}

    debugLog->shared = mmap(0, debugLog->bufferSize, PROT_READ|PROT_WRITE, MAP_SHARED, debugLog->fd, 0);
    if(debugLog->shared==MAP_FAILED) { (void)BERR_TRACE(NEXUS_OS_ERROR);goto err_mmap;}

    logSettings.buffer = debugLog->shared;
    logSettings.bufferSize = debugLog->bufferSize;
    rc = BDBG_Fifo_Create(&debugLog->logWriter, &logSettings);
    if(rc!=BERR_SUCCESS) {(void)BERR_TRACE(rc);goto err_fifo;}
    debugLog->logger = fork();
    if(debugLog->logger<0) {
        goto err_fork;
    } else if(debugLog->logger==0) {
        /* child */
        char *argv[4];

        argv[0] = (void *)logger;
        argv[1] = debugLog->fname;
        argv[2] = (void *)driver;
        argv[3] = NULL;
        /* coverity[toctou: FALSE] */
        execv(logger, argv);
        /* only reached on error */
        _exit(-1);
        /* shouldn't reach here */
        return;
    }
    BDBG_Log_SetFifo(debugLog->logWriter);

    return;

err_fork:
    BDBG_Fifo_Destroy(debugLog->logWriter);
    debugLog->logWriter = NULL;
err_fifo:
    munmap(debugLog->shared, debugLog->bufferSize);
err_mmap:
err_truncate:
    unlink(debugLog->fname);
    close(debugLog->fd);
err_fd:
err_elements:
err_logger:
    return;
}

void NEXUS_Platform_P_DebugLog_Uninit(NEXUS_Platform_P_DebugLog *debugLog)
{
    int status;
    pid_t target;

    if(debugLog->logWriter==NULL) {
        return;
    }
    BDBG_Log_SetFifo(NULL);
    BKNI_Sleep(10); /* there is no any good way to synchronize with the writer */
    status = kill(debugLog->logger, SIGUSR1);
    BDBG_MSG(("kill:%d", status));
    target = waitpid(debugLog->logger, &status, 0);
    BDBG_MSG(("%d waitpid:%d, %d", (int)target, (int)debugLog->logger, (int)status));
    BDBG_Fifo_Destroy(debugLog->logWriter);
    munmap(debugLog->shared, debugLog->bufferSize);
    close(debugLog->fd);
    return;
}

