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
 * $brcm_Workfile: sds_retune.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/sds_retune.c $
 * 
 * 2   9/6/12 6:04p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>


int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	btuner_t tuner = btuner_open(B_ID(2)); /* first SDS tuner for97038 board */
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	btuner_sds_params sds;
	bstream_t stream;
	bstream_mpeg mpeg;
	bband_t band;
	if (rc) goto error;

	btuner_sds_params_init(&sds, tuner);
	sds.symbol_rate = 20000000;
	sds.mode = btuner_sds_mode_qpsk_dvb;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x1b22;
	mpeg.audio[0].pid = 0x1b23;
	mpeg.audio[0].format = baudio_format_mpeg;
	mpeg.pcr_pid = 0x1b22;

	band = btuner_tune_sds(tuner, 1120000000, &sds);

	if (!band) goto error;

	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;

	printf("Press any key to channel change\n");
	getchar(); /* press enter to stop decode */
	bstream_close(stream);

	btuner_sds_params_init(&sds, tuner);
	sds.symbol_rate = 20000000;
	sds.mode = btuner_sds_mode_qpsk_dvb;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x1722;
	mpeg.audio[0].pid = 0x1723;
	mpeg.audio[0].format = baudio_format_mpeg;
	mpeg.pcr_pid = 0x1722;

	band = btuner_tune_sds(tuner, 1120000000, &sds);

	if (!band) goto error;

	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;

	printf("Press any key to channel change\n");
	getchar(); /* press enter to stop decode */
	bstream_close(stream);

	btuner_sds_params_init(&sds, tuner);
	sds.symbol_rate = 20000000;
	sds.mode = btuner_sds_mode_qpsk_dvb;
	sds.diseqc_polarity = btuner_sds_diseqc_polarity_18v;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x1122;
	mpeg.audio[0].pid = 0x1123;
	mpeg.audio[0].format = baudio_format_mpeg;
	mpeg.pcr_pid = 0x1122;

	band = btuner_tune_sds(tuner, 1134000000, &sds);

	if (!band) goto error;

	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;

	printf("Press any key to continue\n");
	getchar(); /* press enter to stop decode */
	bstream_close(stream);

	return 0;
error:
	return 1;
}
