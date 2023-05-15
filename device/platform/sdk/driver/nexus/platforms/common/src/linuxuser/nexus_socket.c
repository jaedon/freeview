/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_socket.c $
* $brcm_Revision: 2 $
* $brcm_Date: 8/31/12 3:28p $
*
* API Description:
*   API name: Platform (private)
*    Common part of all kernel servers
*
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/linuxuser/nexus_socket.c $
* 
* 2   8/31/12 3:28p erickson
* SW7425-3418: rename nexus_socket functions to avoid linker collision
*  with os lib
* 
* 1   8/31/12 2:54p erickson
* SW7425-3418: rework user mode callbacks: filter redundant callbacks on
*  server. sync NEXUS_StopCallbacks across all callback threads.
* 
***************************************************************************/
#include "bstd.h"
#include "server/nexus_ipc_api.h"
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

/* copied from nexus/lib/ipc/bipc_util.c */
ssize_t b_nexus_read(int fd, void *buf, size_t buf_size)
{
    int result;
    for(result=0;;) {
        int rc;
        if(buf_size==0) {
            break;
        }
        rc = read(fd, buf, buf_size);
        if(rc>0) {
            BDBG_ASSERT((unsigned)rc<=buf_size);
            result += rc;
/* TODO: current impl allows read of <buf_size. this works with unix domain sockets.
but for general impl, required for internet domain, we need to know expected size to read. 
ipc must be reworked so that variable size params can be loaded into fixed size header.
then read can be done in two passes: fixed size, variable size.
*/
#if 0
            if((unsigned)rc==buf_size) {
                break;
            }
            BDBG_ASSERT((unsigned)rc<=buf_size);
            buf_size -= rc;
            buf = (uint8_t *)buf + rc;
#else
            break;
#endif
        } else {
            if(rc==0) {
                result = -1;
                break;
            }
            rc = errno;
            if(rc==EINTR) {
                continue;
            } else if((rc==EAGAIN || rc==EWOULDBLOCK) && result==0) {
                break;
            } else {
                result = -rc;
                break;
            }
        }
    }
    return result;
}

/* blocking write. any non-blocking write should be handled directly with write().
returns amount written. anything other than 'size' is an error. */
ssize_t b_nexus_write(int fd, const void *buf, size_t buf_size)
{
    int result;

    for(result=0;;) {
        int rc;
        if(buf_size==0) {
            break;
        }
        rc = write(fd, buf, buf_size);
        if(rc>0) {
            BDBG_ASSERT((unsigned)rc<=buf_size);
            result += rc;
            if((unsigned)rc==buf_size) {
                break;
            }
            BDBG_ASSERT((unsigned)rc<=buf_size);
            buf_size -= rc;
            buf = (uint8_t *)buf + rc;
        } else {
            rc = errno;
            if(rc==EINTR) {
                continue;
            } else if((rc==EAGAIN || rc==EWOULDBLOCK) && result==0) {
                break;
            } else {
                result = -rc;
                break;
            }
        }
    }
    return result;
}

/* for nexus ipc socket close, always do shutdown;close */
int b_nexus_socket_close(int fd)
{
    shutdown(fd, SHUT_RDWR);
    return close(fd);
}
