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
 * $brcm_Workfile: nexus_ccir656_input.c $
 * $brcm_Revision: 11 $
 * $brcm_Date: 6/19/12 12:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/src/nexus_ccir656_input.c $
 * 
 * 11   6/19/12 12:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 10   4/20/11 6:15p jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 * 
 * 9   7/1/09 5:41p katrep
 * PR53453: compiler warning
 * 
 * 8   6/30/09 3:22p erickson
 * PR53453: fix DTV
 * 
 * 7   6/30/09 8:48a erickson
 * PR53453: request L2 interrupt from INT, then call VBI to read 656 vbi
 *  data
 *
 * 6   4/9/09 9:46a erickson
 * PR54033: turn on power for 656 shutdown
 *
 * 5   3/27/09 2:36p erickson
 * PR53453: add 656 vbi support
 *
 * 4   3/24/09 2:34p erickson
 * PR52818: added 656 input power management
 *
 * 3   11/17/08 12:28p erickson
 * PR49203: ensure there are no unnecessary unguarded BVDC_ApplyChanges
 *  calls in Nexus
 *
 * 2   6/30/08 10:02a erickson
 * PR44326: fix warning
 *
 * 1   6/11/08 12:44p vsilyaev
 * PR 43491: Added support for CCIR-656 input
 *
 **************************************************************************/
#include "nexus_base.h"
#include "nexus_display_module.h"
#include "nexus_power_management.h"
#include "bvbi.h"
#if NEXUS_NUM_656_INPUTS
#if (BCHP_CHIP == 3563) || (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)
#include "bchp_int_id_vdec_l2.h"
#else
#include "bchp_int_id_bvnf_intr2_5.h"
#endif
#endif

BDBG_MODULE(nexus_ccir656_input);
#define pVideo (&g_NEXUS_DisplayModule_State)
static unsigned NEXUS_Ccir656Input_P_FreeSpace_isr(NEXUS_Ccir656InputHandle ccir656Input);
static void NEXUS_Ccir656Input_P_AddData_isr(NEXUS_Ccir656InputHandle ccir656Input, void *data, unsigned size);
#if NEXUS_NUM_656_INPUTS
static void NEXUS_Ccir656Input_P_External_isr(void* parm1, int parm2);
#endif

/*
Define TEST_656_VBI to provide an artificial internal data feed for 656 vbi testing.

#define TEST_656_VBI 1
*/
#if TEST_656_VBI
static void NEXUS_Ccir656Input_P_TestVbi(void *context);
#endif

struct NEXUS_Ccir656Input {
    NEXUS_OBJECT(NEXUS_Ccir656Input);
    unsigned index;
    NEXUS_Ccir656InputSettings settings;
    NEXUS_VideoInputObject input;
    BAVC_SourceId sourceId;
    BVBI_Decode_Handle vbiDecode;
    uint8_t *buffer;
    unsigned rptr, wptr;
    unsigned lastGetBuffer;
    NEXUS_IsrCallbackHandle dataReady;
    BVBI_Field_Handle dummyField; /* required by VBI PI, but not used */
    BINT_CallbackHandle tf_isr, bf_isr;
};

void
NEXUS_Ccir656Input_GetDefaultSettings(NEXUS_Ccir656InputSettings *pSettings)
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->format = NEXUS_VideoFormat_eNtsc;
    pSettings->vbi.format = NEXUS_Ccir656VbiFormat_eSaa7113;
    pSettings->vbi.brokenDataCount = true;
}

NEXUS_Ccir656InputHandle
NEXUS_Ccir656Input_Open(unsigned index, const NEXUS_Ccir656InputSettings *pSettings)
{
#if NEXUS_NUM_656_INPUTS
    NEXUS_Ccir656InputHandle ccir656Input;
    NEXUS_Ccir656InputSettings settings;
    NEXUS_Error rc;

    if(index>=NEXUS_NUM_656_INPUTS) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED); goto err_config;}

    if(!pSettings) {
        NEXUS_Ccir656Input_GetDefaultSettings(&settings);
        pSettings = &settings;
    }
    ccir656Input = BKNI_Malloc(sizeof(*ccir656Input));
    if(!ccir656Input) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}

    NEXUS_OBJECT_INIT(NEXUS_Ccir656Input, ccir656Input);
    NEXUS_VIDEO_INPUT_INIT(&ccir656Input->input, NEXUS_VideoInputType_eCcir656, ccir656Input);
    ccir656Input->settings = *pSettings;
    ccir656Input->index = index;
    ccir656Input->sourceId = BAVC_SourceId_e656In0 + ccir656Input->index;

    ccir656Input->dataReady = NEXUS_IsrCallback_Create(ccir656Input, NULL);
    if (!ccir656Input->dataReady) {BERR_TRACE(NEXUS_UNKNOWN); goto err_callback_create;}

    rc = BVBI_Decode_Create(pVideo->vbi, ccir656Input->sourceId, &ccir656Input->vbiDecode);
    if (rc) {rc = BERR_TRACE(rc); goto err_vbi_create;}

    rc = BVBI_Field_Create(&ccir656Input->dummyField, pVideo->vbi);
    if (rc) {rc = BERR_TRACE(rc); goto err_vbi_create;}

#if BCHP_CHIP == 7405
#define BCHP_INT_ID_IN656_TopField BCHP_INT_ID_IN656_0_FILED_0_INTR
#define BCHP_INT_ID_IN656_BotField BCHP_INT_ID_IN656_0_FILED_1_INTR
#else
#define BCHP_INT_ID_IN656_TopField BCHP_INT_ID_Video_Decoder_Ext_656_Field_0_INTR
#define BCHP_INT_ID_IN656_BotField BCHP_INT_ID_Video_Decoder_Ext_656_Field_1_INTR
#endif

    rc = BINT_CreateCallback(&ccir656Input->tf_isr, g_pCoreHandles->bint, BCHP_INT_ID_IN656_TopField,
        NEXUS_Ccir656Input_P_External_isr, ccir656Input, BAVC_Polarity_eTopField);
    if(rc!=NEXUS_SUCCESS) { rc=BERR_TRACE(rc);goto err_settings;}
    rc = BINT_CreateCallback(&ccir656Input->bf_isr, g_pCoreHandles->bint, BCHP_INT_ID_IN656_BotField,
        NEXUS_Ccir656Input_P_External_isr, ccir656Input, BAVC_Polarity_eBotField);
    if(rc!=NEXUS_SUCCESS) { rc=BERR_TRACE(rc);goto err_settings;}

    rc = NEXUS_Ccir656Input_SetSettings(ccir656Input, pSettings);
    if(rc!=NEXUS_SUCCESS) { rc=BERR_TRACE(rc);goto err_settings;}

#if TEST_656_VBI
    NEXUS_ScheduleTimer(1000, NEXUS_Ccir656Input_P_TestVbi, ccir656Input);
#endif

    return ccir656Input;

err_settings:
    if (ccir656Input->vbiDecode) {
        BVBI_Decode_Destroy(ccir656Input->vbiDecode);
    }
err_vbi_create:
    if (ccir656Input->dataReady) {
        NEXUS_IsrCallback_Destroy(ccir656Input->dataReady);
    }
err_callback_create:
    BKNI_Free(ccir656Input);
err_alloc:
err_config:
    return NULL;
#else
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);
    return NULL;
#endif
}

static void
NEXUS_Ccir656Input_P_Finalizer(NEXUS_Ccir656InputHandle ccir656Input)
{
    NEXUS_OBJECT_ASSERT(NEXUS_Ccir656Input, ccir656Input);

    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eCcir656Input, true);
    if (ccir656Input->dummyField) {
        BVBI_Field_Destroy(ccir656Input->dummyField);
    }
    if (ccir656Input->vbiDecode) {
        BVBI_Decode_Destroy(ccir656Input->vbiDecode);
    }
    if (ccir656Input->tf_isr) {
        BINT_DestroyCallback(ccir656Input->tf_isr);
    }
    if (ccir656Input->bf_isr) {
        BINT_DestroyCallback(ccir656Input->bf_isr);
    }
    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eCcir656Input, false);

    if (ccir656Input->dataReady) {
        NEXUS_IsrCallback_Destroy(ccir656Input->dataReady);
    }
    if (ccir656Input->buffer) {
        BMEM_Free(g_pCoreHandles->heap[0], ccir656Input->buffer);
    }

    NEXUS_OBJECT_ASSERT(NEXUS_Ccir656Input, ccir656Input);
    BKNI_Free(ccir656Input);
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_Ccir656Input, NEXUS_Ccir656Input_Close);

void
NEXUS_Ccir656Input_GetSettings( NEXUS_Ccir656InputHandle ccir656Input, NEXUS_Ccir656InputSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(ccir656Input, NEXUS_Ccir656Input);
    BDBG_ASSERT(pSettings);
    *pSettings = ccir656Input->settings;
    return;
}

static NEXUS_Error
NEXUS_VideoCcir656Input_P_ApplyVdcSettings(NEXUS_VideoInput_P_Link *link, const NEXUS_Ccir656InputSettings *pSettings)
{
    NEXUS_Error rc;
    BFMT_VideoFmt formatVdc;

    BDBG_ASSERT(link->sourceVdc);

    rc = NEXUS_P_VideoFormat_ToMagnum(pSettings->format, &formatVdc);
    if (rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}

    rc = BVDC_Source_SetVideoFormat(link->sourceVdc, formatVdc);
    if (rc!=BERR_SUCCESS) {return BERR_TRACE(rc);}

    /* no ApplyChanges here */

    return NEXUS_SUCCESS;
}

static BERR_Code NEXUS_Ccir656Input_P_CaptureVbi_isr(void *context,
    BFMT_VideoFmt eVideoFormat, BAVC_Polarity polarity, uint8_t data_id, uint8_t second_id,
    uint8_t data_count, uint8_t *packet, uint8_t **vbiData, BVBI_SMPTE291M_Description *pktDesc)
{
    NEXUS_Ccir656InputHandle ccir656Input = context;
    NEXUS_Ccir656InputVbiHeader vbiHeader;

    BDBG_OBJECT_ASSERT(ccir656Input, NEXUS_Ccir656Input);

    BSTD_UNUSED(eVideoFormat);

    BDBG_MSG(("VBI: field=%d, ids=%d,%d, count=%d", polarity, data_id, second_id, data_count));

    /* queue data and notify the application that data is available */
    if (NEXUS_Ccir656Input_P_FreeSpace_isr(ccir656Input) >= sizeof(vbiHeader) + data_count) {
        vbiHeader.topField = (polarity == BAVC_Polarity_eTopField);
        vbiHeader.dataId = data_id;
        vbiHeader.secondId = second_id;
        vbiHeader.payload = data_count;
        NEXUS_Ccir656Input_P_AddData_isr(ccir656Input, &vbiHeader, sizeof(vbiHeader));
        NEXUS_Ccir656Input_P_AddData_isr(ccir656Input, packet, data_count);
        NEXUS_IsrCallback_Fire_isr(ccir656Input->dataReady);
    }
    else {
        BDBG_WRN(("656 ancillary data overflow. %d bytes lost.", data_count));
    }

    /* By returning a fail to the VBI PI, it will not parse the data, which is what is desired. */
    BSTD_UNUSED(vbiData);
    BSTD_UNUSED(pktDesc);
    return -1;
}

NEXUS_Error
NEXUS_Ccir656Input_SetSettings(NEXUS_Ccir656InputHandle ccir656Input, const NEXUS_Ccir656InputSettings *pSettings)
{
    NEXUS_Error rc;
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_Get(&ccir656Input->input);

    BDBG_OBJECT_ASSERT(ccir656Input, NEXUS_Ccir656Input);
    BDBG_ASSERT(pSettings);

    if (link) {
        rc = NEXUS_VideoCcir656Input_P_ApplyVdcSettings(link, pSettings);
        if (rc) return BERR_TRACE(rc);
    }

    /* apply VBI settings */
    BDBG_CASSERT(NEXUS_Ccir656VbiFormat_eMax == (NEXUS_Ccir656VbiFormat)BVBI_656Fmt_LAST);
    rc = BVBI_Decode_656_SetFormat(ccir656Input->vbiDecode, pSettings->vbi.format);
    if (rc) return BERR_TRACE(rc);

    rc = BVBI_Decode_656_SetSMPTE291Moptions(ccir656Input->vbiDecode, NEXUS_Ccir656Input_P_CaptureVbi_isr, ccir656Input,
        ccir656Input->settings.vbi.longHeader,
        ccir656Input->settings.vbi.brokenDataCount);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_Display_P_ApplyChanges();
    if (rc) {return BERR_TRACE(rc);}

    if (pSettings->vbi.bufferSize) {
        if (ccir656Input->buffer && pSettings->vbi.bufferSize != ccir656Input->settings.vbi.bufferSize) {
            BMEM_Free(g_pCoreHandles->heap[0], ccir656Input->buffer);
            ccir656Input->buffer = NULL;
        }
        ccir656Input->buffer = BMEM_Alloc(g_pCoreHandles->heap[0], pSettings->vbi.bufferSize);
        if (!ccir656Input->buffer) return BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY);

        NEXUS_Ccir656Input_Flush(ccir656Input);
    }
    else {
        if (ccir656Input->buffer) {
            BMEM_Free(g_pCoreHandles->heap[0], ccir656Input->buffer);
            ccir656Input->buffer = NULL;
        }
    }

    NEXUS_IsrCallback_Set(ccir656Input->dataReady, &pSettings->vbi.dataReady);

    ccir656Input->settings = *pSettings;
    return NEXUS_SUCCESS;
}

NEXUS_VideoInput
NEXUS_Ccir656Input_GetConnector( NEXUS_Ccir656InputHandle ccir656Input)
{
    BDBG_OBJECT_ASSERT(ccir656Input, NEXUS_Ccir656Input);
    return &ccir656Input->input;
}

#if NEXUS_NUM_656_INPUTS

static void
NEXUS_Ccir656Input_P_External_isr(void* parm1, int parm2)
{
    NEXUS_Ccir656InputHandle ccir656Input = (NEXUS_Ccir656InputHandle)parm1;
    BAVC_Polarity field = (BAVC_Polarity)parm2;
    BERR_Code rc;

    BDBG_OBJECT_ASSERT(ccir656Input, NEXUS_Ccir656Input);

    rc = BVBI_Decode_Data_isr(ccir656Input->vbiDecode, ccir656Input->dummyField, field);
    if (rc!=BERR_SUCCESS) {
        BDBG_ERR(("BVBI_Decode_Data_isr returned error %d", rc));
    }
}

static NEXUS_Error
NEXUS_VideoInput_P_ConnectCcir656Input(NEXUS_VideoInput_P_Link *link)
{
    NEXUS_Ccir656InputHandle ccir656Input;
    BERR_Code rc;

    BDBG_ASSERT(link->input);
    BDBG_ASSERT(link->input->type == NEXUS_VideoInputType_eCcir656);
    ccir656Input = link->input->source;
    BDBG_OBJECT_ASSERT(ccir656Input, NEXUS_Ccir656Input);

    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eCcir656Input, true);

    rc = BINT_EnableCallback(ccir656Input->tf_isr);
    if (rc) return BERR_TRACE(rc);
    rc = BINT_EnableCallback(ccir656Input->bf_isr);
    if (rc) return BERR_TRACE(rc);

    return NEXUS_VideoCcir656Input_P_ApplyVdcSettings(link, &ccir656Input->settings);
}

static void
NEXUS_VideoInput_P_DisconnectCcir656Input(NEXUS_VideoInput_P_Link *link)
{
    NEXUS_Ccir656InputHandle ccir656Input;

    BDBG_ASSERT(link->input);
    BDBG_ASSERT(link->input->type == NEXUS_VideoInputType_eCcir656);
    ccir656Input = link->input->source;
    BDBG_OBJECT_ASSERT(ccir656Input, NEXUS_Ccir656Input);

    BINT_DisableCallback(ccir656Input->tf_isr);
    BINT_DisableCallback(ccir656Input->bf_isr);

    NEXUS_PowerManagement_SetCoreState(NEXUS_PowerManagementCore_eCcir656Input, false);
    return;
}

NEXUS_VideoInput_P_Link *
NEXUS_VideoInput_P_OpenCcir656(NEXUS_VideoInput input)
{
    NEXUS_VideoInput_P_Iface iface;
    NEXUS_VideoInput_P_Link *link;
    NEXUS_VideoInput_P_LinkData data;
    NEXUS_Ccir656InputHandle ccir656Input;

    BDBG_ASSERT(input->type == NEXUS_VideoInputType_eCcir656);
    ccir656Input = input->source;
    BDBG_OBJECT_ASSERT(ccir656Input, NEXUS_Ccir656Input);

    iface.connect = NEXUS_VideoInput_P_ConnectCcir656Input;
    iface.disconnect = NEXUS_VideoInput_P_DisconnectCcir656Input;
    NEXUS_VideoInput_P_LinkData_Init(&data, ccir656Input->sourceId);
    link = NEXUS_VideoInput_P_CreateLink(input, &data, &iface);
    if(!link) {
        return NULL;
    }
    return link;
}

#endif /* NEXUS_NUM_656_INPUTS */

static unsigned NEXUS_Ccir656Input_P_FreeSpace_isr(NEXUS_Ccir656InputHandle ccir656Input)
{
    if (ccir656Input->buffer) {
        if (ccir656Input->wptr < ccir656Input->rptr) {
            return ccir656Input->rptr - ccir656Input->wptr - 1;
        }
        else {
            return (ccir656Input->settings.vbi.bufferSize - ccir656Input->wptr) + ccir656Input->rptr - 1;
        }
    }
    return 0; /* no space */
}

/* this functions assumes NEXUS_Ccir656Input_P_FreeSpace_isr was used first to qualify the addition */
static void NEXUS_Ccir656Input_P_AddData_isr(NEXUS_Ccir656InputHandle ccir656Input, void *data, unsigned size)
{
    unsigned n;

    if (ccir656Input->wptr < ccir656Input->rptr) {
        n = ccir656Input->rptr - ccir656Input->wptr - 1;
    }
    else {
        n = ccir656Input->settings.vbi.bufferSize - ccir656Input->wptr;
    }
    if (n > size) n = size;
    BKNI_Memcpy(&ccir656Input->buffer[ccir656Input->wptr], data, n);
    ccir656Input->wptr += n;
    if (ccir656Input->wptr == ccir656Input->settings.vbi.bufferSize) {
        ccir656Input->wptr = 0;
        size -= n;
        if (size) {
            BKNI_Memcpy(&ccir656Input->buffer[ccir656Input->wptr], &((uint8_t*)data)[n], size);
            ccir656Input->wptr += size;
        }
    }
    BDBG_ASSERT(ccir656Input->wptr < ccir656Input->settings.vbi.bufferSize);
}

NEXUS_Error NEXUS_Ccir656Input_GetBuffer( NEXUS_Ccir656InputHandle ccir656Input, void **buffer, unsigned *size )
{
    BDBG_OBJECT_ASSERT(ccir656Input, NEXUS_Ccir656Input);

    if (ccir656Input->buffer) {
        BKNI_EnterCriticalSection();
        *buffer = &ccir656Input->buffer[ccir656Input->rptr];
        if (ccir656Input->rptr <= ccir656Input->wptr) {
            *size = ccir656Input->wptr - ccir656Input->rptr;
        }
        else {
            *size = ccir656Input->settings.vbi.bufferSize - ccir656Input->rptr;
        }
        BKNI_LeaveCriticalSection();
        ccir656Input->lastGetBuffer = *size;
        return 0;
    }
    else {
        *buffer = NULL;
        *size = 0;
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
}

NEXUS_Error NEXUS_Ccir656Input_ReadComplete( NEXUS_Ccir656InputHandle ccir656Input, unsigned sizeConsumed )
{
    BDBG_OBJECT_ASSERT(ccir656Input, NEXUS_Ccir656Input);

    if (sizeConsumed > ccir656Input->lastGetBuffer) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    BKNI_EnterCriticalSection();
    ccir656Input->rptr += sizeConsumed;
    BDBG_ASSERT(ccir656Input->rptr <= ccir656Input->settings.vbi.bufferSize); /* if lastGetBuffer is accurate, this should be true */
    if (ccir656Input->rptr == ccir656Input->settings.vbi.bufferSize) {
        ccir656Input->rptr = 0;
    }
    BKNI_LeaveCriticalSection();
    return 0;
}

void NEXUS_Ccir656Input_Flush( NEXUS_Ccir656InputHandle ccir656Input )
{
    BDBG_OBJECT_ASSERT(ccir656Input, NEXUS_Ccir656Input);
    BKNI_EnterCriticalSection();
    ccir656Input->wptr = ccir656Input->rptr = 0;
    BKNI_LeaveCriticalSection();
    ccir656Input->lastGetBuffer = 0;
}

#if TEST_656_VBI
#include <stdlib.h>
static void NEXUS_Ccir656Input_P_TestVbi(void *context)
{
    int i;
    for (i=0;i<rand()%5;i++) {
        unsigned char buf[100];
        unsigned n = (rand() % 100) + 1;

        /* this test code will intentionally overflow. this tests nexus ability to guarantee atomic messages */
        BKNI_EnterCriticalSection();
        NEXUS_Ccir656Input_P_External_isr(context, BAVC_Polarity_eTopField);
        NEXUS_Ccir656Input_P_External_isr(context, BAVC_Polarity_eBotField);
        NEXUS_Ccir656Input_P_CaptureVbi_isr(context, BFMT_VideoFmt_eNTSC, BAVC_Polarity_eTopField, 0, 1, n, buf, NULL, NULL);
        BKNI_LeaveCriticalSection();
    }
    NEXUS_ScheduleTimer(rand()%1000, NEXUS_Ccir656Input_P_TestVbi, context);
}
#endif
