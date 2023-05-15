/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: async_tune_sds.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/async_tune_sds.c $
 * 
 * 4   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <string.h>
#include <bstd.h>
#include <bkni.h>

void locked(void *data)
{
    printf(" Locked Callback called\n");
	BKNI_SetEvent((BKNI_EventHandle)data);
}

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner = btuner_open(bconfig->resources.sds.objects[0]);
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	btuner_sds_params sds;
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;
	BKNI_EventHandle event;
	if (rc) goto error;
	
	BKNI_CreateEvent(&event);

    btuner_sds_params_init(&sds, tuner);
    sds.lock_callback = locked;
    sds.callback_context = event;
    sds.wait_for_lock = true;
    sds.mode = btuner_sds_mode_qpsk_dss; 
    sds.diseqc_polarity = btuner_sds_diseqc_polarity_18v;

	band = btuner_tune_sds(tuner,1396820000, &sds);

    if (!band) goto error;

	/* BKNI_WaitForEvent(event, 0xFFFFFFFF); */

	bstream_mpeg_init(&mpeg);
    mpeg.mpeg_type = bstream_mpeg_type_dss_es;
	mpeg.video[0].pid = 0x78;
	mpeg.audio[0].pid = 0x79;
	mpeg.audio[0].format = baudio_format_mpeg; 
	mpeg.pcr_pid = 0x78;
	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;

	getchar(); /* press enter to stop decode */
	return 0;
error:
	return 1;
}

