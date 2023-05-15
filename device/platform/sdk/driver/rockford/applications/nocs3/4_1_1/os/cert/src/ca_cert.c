/***************************************************************************
*     (c)2003-2013 Broadcom Corporation
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
* 
* Module Description:
*  Initial Implementation of DAL functionality
*	Compatibility NOCS3.0
*	Compatibility NASC1.5
*
* Revision History:
* 
*  26/11/13 - Version 1.0
*  Initial version internally tested with DALTS Version 3.12.0
* 	Based on DALAPI Version 7.1.0
*	Based on Nagra header ca_cert.h Revision 1.3.0
*
*
***************************************************************************/


#include "bchp_sun_top_ctrl.h"
#include "bchp_nand.h"

#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_types.h"
#include "nexus_memory.h"
#include "nexus_security_datatypes.h"
#include "nexus_security.h"
#include "nexus_security_types.h"
#include "nexus_otpmsp.h"
#include "nexus_keyladder.h"
#include "nocs_csd_impl.h"
#include "nocs_csd.h"
#include "bdbg.h"
#include "bstd.h"
#include "bkni.h"
#include "ca_cert.h"
#include "ca_cert_impl.h"
#include "bchp_bsp_pkl.h"

#define CERT_DATA_IN ( BCHP_BSP_PKL_PKLi_ARRAY_BASE) /* PKL Register Address 0 */
#define CERT_DATA_OUT (CERT_DATA_IN+32)
#define CERT_STATUS_AKL (CERT_DATA_IN+64)
#define CERT_COMMAND (CERT_DATA_IN+68)
#define CERT_INTERRUPT (CERT_DATA_IN+72)

#define CERT_UNUSED(x) (void)(x)

BDBG_MODULE(NCERT);

/* stubs */
static TCertStatus certInit(void);

#if 0 /* HUMAX_PLATFORM_BASE */ /* Call from Application, current not needed */
static TCertFunctionTable certTable;
#endif
static struct SCertResourceHandle ResourceHandle;

/**************************************** private functions ***************************************** */

#if CFG_BIG_ENDIAN == 0
static void swap_bytes(unsigned char *input)
{
    unsigned char temp;

    temp = input[0];
    input[0] = input[3];
    input[3] = temp;

    temp = input[2];
    input[2] = input[1];
    input[1] = temp;

    return;
}

static void write_reg(uint32_t address, uint32_t data)
{
    swap_bytes((unsigned char *)&data);
    NEXUS_Platform_WriteRegister(address, data);
}

static void read_reg(uint32_t address, uint32_t *data)
{
    NEXUS_Platform_ReadRegister(address, data);
    swap_bytes((unsigned char *)data);
}
#else
#define write_reg NEXUS_Platform_WriteRegister
#define read_reg NEXUS_Platform_ReadRegister
#endif


static TCertStatus certInit(
    void
)
{
    TCertStatus status = CERT_ERROR;
    static bool firstTime=true;

    BDBG_MSG(("Enter: %s", __FUNCTION__)) ;

    if(firstTime)
    {
        /* Init the PKL */
        NEXUS_Security_PklInit();
        firstTime=false;

        BKNI_Memset(&ResourceHandle,0,sizeof(struct SCertResourceHandle));
        BKNI_CreateMutex( &(ResourceHandle.mutexLock));
    }

    status = CERT_NO_ERROR;

    BDBG_MSG(("Leave: %s", __FUNCTION__)) ;
    return status;
}

/**************************************** cert API functions ***************************************** */



TCertStatus certLock(
    TCertResourceHandle *pxResourceHandle
)
{
    BDBG_MSG(("Enter: %s", __FUNCTION__)) ;

    if(!pxResourceHandle) {
        BDBG_ERR(("%s Invalid pxResourceHandle", __FUNCTION__)) ;
        return CERT_ERROR;
    }

    certInit();

    BKNI_AcquireMutex(ResourceHandle.mutexLock);
    ResourceHandle.certLocked = 1;

    *pxResourceHandle = &ResourceHandle;

    BDBG_MSG(("Leave: %s pxResourceHandle = %p", __FUNCTION__, pxResourceHandle)) ;
    return CERT_NO_ERROR;
}

TCertStatus certUnlock(
    TCertResourceHandle  xResourceHandle
)
{
    BDBG_MSG(("Enter: %s xResourceHandle %p", __FUNCTION__, xResourceHandle)) ;

    if(!xResourceHandle) {
        BDBG_ERR(("%s Invalid parameter", __FUNCTION__)) ;
        return CERT_ERROR_BAD_HANDLE;
    }
    if(xResourceHandle != &ResourceHandle) {
        BDBG_ERR(("%s Invalid xResourceHandle", __FUNCTION__)) ;
        return CERT_ERROR_BAD_HANDLE;
    }
    if(!xResourceHandle->certLocked) {
        BDBG_ERR(("%s cert not locked ", __FUNCTION__)) ;
        return CERT_ERROR_BAD_HANDLE;
    }

    xResourceHandle->certLocked = 0;
    BKNI_ReleaseMutex(xResourceHandle->mutexLock);

    BDBG_MSG(("Leave: %s", __FUNCTION__)) ;
    return CERT_NO_ERROR;

}

TCertStatus certExchange(
    TCertResourceHandle xResourceHandle,
    size_t              xNumOfCommands,
    const TCertCommand* pxCommands,
    size_t*             pxNumOfProcessedCommands
)
{
    uint32_t cmd_complete;
    TCertStatus status;
    unsigned int j, i = 0;
 
    status = CERT_ERROR;

    BDBG_MSG(("Enter: %s xResourceHandle %p xNumOfCommands %d pxCommands %p", __FUNCTION__, xResourceHandle, xNumOfCommands, pxCommands)) ;

#if CFG_BIG_ENDIAN == 0	
    BDBG_MSG(("%s Little endian mode", __FUNCTION__)) ;
#else
    BDBG_MSG(("%s Big endian mode", __FUNCTION__)) ;
#endif

    if(!xResourceHandle) {
        BDBG_ERR(("%s Invalid xResourceHandle", __FUNCTION__)) ;
        return CERT_ERROR_BAD_HANDLE;
    }
    if(!xNumOfCommands || !pxCommands || !pxNumOfProcessedCommands) {
        BDBG_ERR(("%s Invalid Commands", __FUNCTION__)) ;
        return CERT_ERROR;
    }
    if(xResourceHandle != &ResourceHandle) {
        BDBG_ERR(("%s Invalid xResourceHandle 1", __FUNCTION__));
        return CERT_ERROR_BAD_HANDLE;
    }
    if(!xResourceHandle->certLocked) {
        /* error because cert should already be locked to call cert*/
        BDBG_ERR(("%s cert not locked ", __FUNCTION__)) ;
        return CERT_ERROR_BAD_HANDLE;
    }

    for(i=0; i<xNumOfCommands; i++)
    {
        BKNI_Memset((void*)pxCommands[i].outputData,0,32);
        BKNI_Memset((void*)pxCommands[i].status,0,4);
    }

    /* process each command in the buffer. */
    for(i=0; i<xNumOfCommands; i++)
    {
        BDBG_MSG(("input data for command %d",i ));	

        /* write the input data. */
        for(j=0; j<32; j+=4)
            write_reg( CERT_DATA_IN+j, *(uint32_t *)&(pxCommands[i].inputData[j]) );

        /* write the opcode to the CERT. */
        write_reg( CERT_COMMAND, *(uint32_t *)&(pxCommands[i].opcodes[0]));

        NEXUS_Platform_ReadRegister( CERT_COMMAND, &cmd_complete );
        BDBG_MSG(("cmd_complete = 0x%x",  cmd_complete));

#if POLL
        /* wait for command to complete. */
        do
        {
            NEXUS_Platform_ReadRegister( CERT_COMMAND, &cmd_complete );
        } while((cmd_complete&0x1) == 0x1);
#else
        if((pxCommands->timeout) == CERT_TIMEOUT_DEFAULT)
            BKNI_Sleep(CERT_DEFAULT_TIMEOUT_VALUE);
        else
            BKNI_Sleep(CERT_OTP_CMD_TIMEOUT_VALUE);

        NEXUS_Platform_ReadRegister( CERT_COMMAND, &cmd_complete );
        if((cmd_complete&0x1) == 0x1)
        {
            BDBG_ERR(("ERROR: Command did not complete in time!!!"));
            status = CERT_ERROR_TIMEOUT;
            goto Exit_TCertExchange;
        }
        else {
            BDBG_MSG(("Command completed cmd_complete = 0x%x",  cmd_complete));
        }
#endif

        /* clear the interrupt. */
        NEXUS_Platform_ReadRegister( CERT_INTERRUPT, &cmd_complete );
        NEXUS_Platform_WriteRegister( CERT_INTERRUPT, cmd_complete );

        /* read the status of the command. */
        read_reg( CERT_STATUS_AKL, (uint32_t *)&(pxCommands[i].status[0]) );

        /* read the output data. */
        for(j=0; j<32; j+=4)
            read_reg( CERT_DATA_OUT+j, (uint32_t*)&(pxCommands[i].outputData[j]));

        if( (*(TCertUInt32*)&(pxCommands[i].status[3]) & 0x1)== 0x1)
        {
            i++;
            status = CERT_ERROR;
            BDBG_ERR(("(*(TCertUInt32*)&(pxCommands[i].status[3]) & 0x1)== 0x1"));
            goto Exit_TCertExchange;
        }
    }

    status = CERT_NO_ERROR;

Exit_TCertExchange:
    BDBG_MSG(("Leave: %s", __FUNCTION__)) ;
    *pxNumOfProcessedCommands = i;
    return status;
}


TCertStatus certUseDecryptionKey(
    TCertUInt16           	xEmi,
    TCertKeyPathHandle*  	pxKeyPathHandle,
    TCertKeyParity        	xKeyParity
)
{
    CERT_UNUSED(xEmi);
    CERT_UNUSED(pxKeyPathHandle);
    CERT_UNUSED(xKeyParity);
    BDBG_ERR(("%s Deprecated", __FUNCTION__)) ;
    return CERT_ERROR;
}

TCertStatus certUseEncryptionKey(
    TCertUInt16           	xEmi,
    TCertKeyPathHandle*  	pxKeyPathHandle,
    TCertKeyParity        	xKeyParity
)
{
    CERT_UNUSED(xEmi);
    CERT_UNUSED(pxKeyPathHandle);
    CERT_UNUSED(xKeyParity);
    BDBG_ERR(("%s Deprecated", __FUNCTION__)) ;
    return CERT_ERROR;
}

#if 0 /* HUMAX_PLATFORM_BASE */ /* Call from Application, current not needed */
TCertFunctionTable* certGetFunctionTable(
    void
)
{
    certTable.certApiVersion = CERTAPI_VERSION_INT;
    certTable.certExchange = &certExchange;
    certTable.certLock = &certLock;
    certTable.certUnlock = &certUnlock;
    certTable.certUseDecryptionKey = &certUseEncryptionKey;
    certTable.certUseEncryptionKey = &certUseDecryptionKey;

    return &certTable;
}
#endif
