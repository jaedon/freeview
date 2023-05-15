/******************************************************************************
 *    (c)2008 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: encrypt_decrypt_in_xpt.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 11/16/12 6:11p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/security/encrypt_decrypt_in_xpt.c $
 * 
 * Hydra_Software_Devel/2   11/16/12 6:11p mward
 * SW7435-56:  Stub security tests if !NEXUS_HAS_SECURITY.
 * 
 * Hydra_Software_Devel/1   6/8/10 12:22p mphillip
 * SW7550-398: Add blockmode example for 7550
 * 
 *
 */

#include "nexus_platform.h"
#include "nexus_recpump.h"
#include "nexus_playpump.h"
#if NEXUS_HAS_SECURITY
#include "nexus_security.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* This is a chip-specific example application to encrypt (or decrypt) a block of data 
 * using transport.  It was specifically written as an example for 7550 and may not 
 * be applicable to other boards.
 */

/* re-use single-des test, w/same key for key 1 and key 2 */
const uint8_t tdes_cipher_vector[]={
        0x3f, 0xa4, 0x0e, 0x8a, 0x98, 0x4d, 0x48, 0x15, 
        0x6a, 0x27, 0x17, 0x87, 0xab, 0x88, 0x83, 0xf9, 
        0x89, 0x3d, 0x51, 0xec, 0x4b, 0x56, 0x3b, 0x53 };
const uint8_t tdes_plain_vector[24]="Now is the time for all ";
const uint8_t tdes_key[16]={ 0x01, 0x23, 0x45, 0x67,  0x89, 0xab, 0xcd, 0xef,
                             0x01, 0x23, 0x45, 0x67,  0x89, 0xab, 0xcd, 0xef };

#define ENCRYPT 1
#if !ENCRYPT
#define DECRYPT 1
#endif

static void dataready_callback(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}


int main(int argc, char **argv)
{
#if NEXUS_HAS_SECURITY
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_RecpumpSettings recpumpSettings;
    NEXUS_RecpumpAddPidChannelSettings addPidChannelSettings;
    NEXUS_RecpumpHandle recpump;
    BKNI_EventHandle dataready_event;
    NEXUS_PidChannelHandle cryptoPidChannel;
    NEXUS_Error rc;

    NEXUS_KeySlotHandle keyHandle;
    NEXUS_SecurityAlgorithmSettings algConfig;
    NEXUS_SecurityClearKey key;
    NEXUS_SecurityKeySlotSettings keySettings;

#if 0
    unsigned feed_zeros = 256;
#else
    unsigned feed_zeros = 4*1024 *256;
#endif
    bool feed_data = true;


    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Allocate AV keyslots */
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eCp;
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
    if(!keyHandle) {
        printf("\nAllocate keyslot failed \n");
        return 1;
    }
    NEXUS_Security_GetDefaultAlgorithmSettings(&algConfig);
    algConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCps; /* CPS implies encryption */
    algConfig.bEncryptBeforeRave = true; /* needs to be configured, otherwise CPS does its work too late */
    algConfig.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
    algConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
    algConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
    NEXUS_Security_ConfigAlgorithm(keyHandle, &algConfig);

    /* Load AV keys */
    NEXUS_Security_GetDefaultClearKey(&key);
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
#if 0
    key.keySize = sizeof(tdes_key);
#else
    key.keySize = 16;
#endif
    BKNI_Memcpy(key.keyData, tdes_key, sizeof(tdes_key));
    if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) {
        printf("\nLoad encryption key failed \n");
        return 1;
    }
    key.keyEntryType = NEXUS_SecurityKeyType_eEven;
    if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) {
        printf("\nLoad encryption key failed \n");
        return 1;
    }
    key.keyEntryType = NEXUS_SecurityKeyType_eClear;
    if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0) {
        printf("\nLoad encryption key failed \n");
        return 1;
    }

    /* configure transport and still decoder */
    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);
    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.transportType = NEXUS_TransportType_eEs;
#if DECRYPT
    playpumpSettings.securityContext = keyHandle;
#endif
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

    cryptoPidChannel = NEXUS_Playpump_OpenPidChannel(playpump, 0x1, NULL);

    BKNI_CreateEvent(&dataready_event);
    recpump = NEXUS_Recpump_Open(0, NULL);
    NEXUS_Recpump_GetSettings(recpump, &recpumpSettings);
    recpumpSettings.data.dataReady.callback = dataready_callback;
    recpumpSettings.data.dataReady.context = dataready_event;
    recpumpSettings.index.dataReady.callback = NULL;
    recpumpSettings.index.dataReady.context = NULL;
    recpumpSettings.data.overflow.callback = NULL;
    recpumpSettings.data.overflow.context = NULL;
    recpumpSettings.index.overflow.callback = NULL;
    recpumpSettings.index.overflow.context = NULL;
    recpumpSettings.outputTransportType = NEXUS_TransportType_eEs;
#if ENCRYPT
    recpumpSettings.securityContext = keyHandle;
#endif
    NEXUS_Recpump_SetSettings(recpump, &recpumpSettings);

    NEXUS_Recpump_GetDefaultAddPidChannelSettings(&addPidChannelSettings);
    addPidChannelSettings.pidType = NEXUS_PidType_eOther;
    NEXUS_Recpump_AddPidChannel(recpump, cryptoPidChannel, &addPidChannelSettings);

    printf("press <ENTER> to start play/recpump\n");
    getchar();

    rc = NEXUS_Playpump_Start(playpump);
    rc = NEXUS_Recpump_Start(recpump);

    while (feed_data || feed_zeros) {
        size_t size;
        void *buffer;

        rc = NEXUS_Playpump_GetBuffer(playpump, &buffer, &size);
        assert(!rc);
        if (!size) {
            BKNI_Sleep(10);
            continue;
        }
        if (feed_data) {
            assert(size > sizeof(tdes_plain_vector));
            BKNI_Memcpy(buffer, tdes_plain_vector, sizeof(tdes_plain_vector));
            size = sizeof(tdes_plain_vector);
            feed_data = false;
        }
        else if (!feed_data && feed_zeros) {
            if (size > feed_zeros) size = feed_zeros;
            BKNI_Memset(buffer, 0, size);
            feed_zeros -= size;
        }
        rc = NEXUS_Playpump_ReadComplete(playpump, 0, size);
        assert(!rc);
    }

    rc = BKNI_WaitForEvent(dataready_event, 3000);
    if (rc) {
        printf("No data received!\n");
        goto done;
    }
    {
        const void *data_buffer;
        size_t data_buffer_size=0;
        if (!NEXUS_Recpump_GetDataBuffer(recpump, &data_buffer, &data_buffer_size)) {
            int ix;
            const uint8_t* buffer = (uint8_t *)data_buffer;
            printf("Got recpump buffer (buffer: %p, size: %d)!\n",data_buffer,data_buffer_size);
            for (ix=0; ix < 64; ix++) {
                if (ix%4 == 0) printf(" ");
                printf("%02x", buffer[ix]);
                if (ix%16 == 15) printf("\n");
            }
            printf("\n");
        }
    }

    printf("press <ENTER> to continue\n");
    getchar();

done:
    NEXUS_Recpump_RemoveAllPidChannels(recpump);
    NEXUS_Recpump_Stop(recpump);
    NEXUS_Recpump_Close(recpump);
    NEXUS_Playpump_Stop(playpump);
    NEXUS_Playpump_Close(playpump);
    NEXUS_Platform_Uninit();
#else
	BSTD_UNUSED(argc);
	BSTD_UNUSED(argv);
#endif /* NEXUS_HAS_SECURITY */

    return 0;
}

