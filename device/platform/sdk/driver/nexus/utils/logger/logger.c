/***************************************************************************
 *     (c)2011 Broadcom Corporation
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
 * $brcm_Workfile: logger.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 8/24/11 4:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/utils/logger/logger.c $
 * 
 * 9   8/24/11 4:29p vsilyaev
 * SW7346-466: Take 3 on fixing tool false positives
 * 
 * 8   8/22/11 11:30a vsilyaev
 * SW7346-466: Fixed tool false positives
 * 
 * 7   8/19/11 11:40a vsilyaev
 * SW7346-466: Added assert
 * 
 * 6   8/15/11 12:12p vsilyaev
 * SW7420-1465, SW7405-5221: Use -I to point deep into the platform
 *  directory
 * 
 * 5   8/9/11 12:50p vsilyaev
 * SW7420-1465: Improved handshake protocol
 * 
 * 4   8/5/11 1:02p vsilyaev
 * SW7420-1465: Added code to handle condition when logger was running
 *  prior to parent have activated the debug log
 * 
 * 3   8/4/11 8:02p vsilyaev
 * SW7420-1465: Added protocol to ensure that all buffered debug output
 *  gets written out
 * 
 * 2   8/3/11 5:33p vsilyaev
 * SW7420-1465, SW7405-5221: Route debug output to the special FIFO
 *  instead of syslog buffer
 * 
 * 1   6/3/11 5:35p vsilyaev
 * SW7405-4477: nexus logger tool
 * 
 **************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bdbg_fifo.h"
#include "bdbg_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "nexus_driver_ioctl.h"

BDBG_MODULE(logger);

static bool sigusr1_fired=false;

static void sigusr1_handler(int unused)
{
    BSTD_UNUSED(unused);
    sigusr1_fired = true;
    return;
}

static void usage(const char *name)
{
    fprintf(stderr, "Usage:\n%s log_file [device_node]\n", name);
    exit(0);
}

int main(int argc, const char *argv[])
{
    BERR_Code rc;
    BDBG_FifoReader_Handle logReader;
    BDBG_Fifo_Handle logWriter;
    int fd;
    int device_fd=-1;
    bool driver_ready = false;
    const char *fname;
    size_t size;
    void *shared;
    struct stat st;
    int urc;
    pid_t parent;
    PROXY_NEXUS_Log_Instance instance;
    
    if(argc<2) {
        usage(argv[0]);
    }
    BSTD_UNUSED(argv);

    rc = BKNI_Init();
    BDBG_ASSERT(rc==BERR_SUCCESS);
    rc = BDBG_Init();
    BDBG_ASSERT(rc==BERR_SUCCESS);

    /* coverity[var_assign_var] */
    fname = argv[1];
    BDBG_ASSERT(fname);
    /* coverity[tainted_string] */
    fd = open(fname, O_RDONLY);
    if(fd<0) {
        perror(fname);
        usage(argv[0]);
    }

    if(argc>2) {
        int ready;
        /*( device_fd = open(argv[2],O_RDWR); */
        device_fd = atoi(argv[2]);
        if(device_fd<0) {
            perror(argv[2]);
            usage(argv[0]);
        }
        urc = ioctl(device_fd, IOCTL_PROXY_NEXUS_Log_Test, &ready);
        if(urc!=0) {
            perror(argv[2]);
            usage(argv[0]);
        }
        if(ready) {
            urc = ioctl(device_fd, IOCTL_PROXY_NEXUS_Log_Create, &instance);
            if(urc==0) {
                driver_ready = true;
            }
        }
    }
    /* unlink(fname); don't remove file, allow multiple copies of logger */
    urc = fstat(fd, &st);
    if(urc<0) {
        perror("stat");
        usage(argv[0]);
    }
    parent = getppid();
    signal(SIGUSR1,sigusr1_handler);
    size = st.st_size;
    shared = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if(shared==MAP_FAILED) {
        perror("mmap");
        usage(argv[0]);
    }
    logWriter = (BDBG_Fifo_Handle)shared;
    rc = BDBG_FifoReader_Create(&logReader, logWriter);
    BDBG_ASSERT(rc==BERR_SUCCESS);
    for(;;) {
        unsigned timeout;

        for(;;) {
            char dbgStr[257]; 
            size_t dbgStrLen;


            timeout = 0;
            rc = BDBG_Log_Dequeue(logReader, &timeout, dbgStr, sizeof(dbgStr)-1, &dbgStrLen);
            if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);break;}
            if(dbgStrLen) {
                /* BDBG_ERR(("%u %u", dbgStrLen, strlen(dbgStr))); */
                dbgStr[dbgStrLen] = '\n';
                dbgStrLen++;
                urc = write(STDERR_FILENO, dbgStr, dbgStrLen); 
                /* BDBG_ERR(("%d %d %d", urc, dbgStrLen, strlen(dbgStr))); */
                /* BDBG_ASSERT(urc==(int)dbgStrLen); */
            }
            if(driver_ready) {
                PROXY_NEXUS_Log_Dequeue dequeue;
                dequeue.instance = instance;
                dequeue.buffer_size = sizeof(dbgStr)-1;
                dequeue.buffer = dbgStr;
                dequeue.timeout = 0;
                urc = ioctl(device_fd, IOCTL_PROXY_NEXUS_Log_Dequeue, &dequeue);
                if(urc!=0) {
                    BDBG_MSG(("Can't read data from the driver"));
                    dequeue.timeout = timeout;
                    dequeue.buffer_size = 0;
                }
                dbgStrLen = dequeue.buffer_size;
                if(dbgStrLen) {
                    dbgStr[dbgStrLen] = '\n';
                    dbgStrLen++;
                    urc = write(STDERR_FILENO, dbgStr, dbgStrLen); 
                }
                if(timeout>dequeue.timeout) {
                    timeout = dequeue.timeout;
                }
            }
            if(timeout!=0) {
                break;
            }
        }
        if(timeout) {
            if(sigusr1_fired) {
                goto done;
            }
            if(kill(parent, 0)) {
                static const char terminated[] = "_____ TERMINATED _____\n";
                write(STDERR_FILENO, terminated, sizeof(terminated)-1);
                break;
            }
            BKNI_Sleep(timeout);
            if(device_fd>=0 && !driver_ready) {
                int ready;
                /* if parent was dealyed keep trying to attach */
                urc = ioctl(device_fd, IOCTL_PROXY_NEXUS_Log_Test, &ready);
                if(urc!=0) {
                    goto done;
                }
                if(ready) {
                    urc = ioctl(device_fd, IOCTL_PROXY_NEXUS_Log_Create, &instance);
                    if(urc!=0) {
                        goto done;
                    }
                    driver_ready = true;
                }
            }
            continue;
        }
        break;
    }
done:
    if(device_fd>=0) {
        close(device_fd); 
    }
    close(fd);
    BDBG_FifoReader_Destroy(logReader);

    BDBG_Uninit();
    /* BKNI_Uninit(); Don't call it since this would print memory allocation stats */
    return 0;
}

