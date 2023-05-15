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
 * $brcm_Workfile: transport_test1.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/transport_test1.c $
 * 
 * 2   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

int main() {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bband_t band;
	bmessage_stream_t msg;
	bmessage_stream_params params;
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bstream_mpeg mpeg;
	int i;
	
	if (rc) goto error;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x1122;
	mpeg.video[0].format = bvideo_codec_h264;

	band = bstreamer_attach(B_ID(0), bstream_mpeg_type_ts);
	if (!band) goto error;
	
	/* run this test with msg_modules=base_xpt and verify the refcnt on the pidparser goes from 0->2 then
	from 2->0. */
	
	bmessage_stream_params_init(&params, NULL);
	params.band = band;
	params.pid = 0x0;
	msg = bmessage_open(bmessage_format_psi);
	if (!msg) goto error;
	if (bmessage_start(msg, &params)) goto error;
	
	for (i=0;i<3;i++) {
		bstream_t stream = bstream_open(band, &mpeg);
		bdecode_start(decode, stream, window);
		BKNI_Sleep(3000);
		bdecode_stop(decode);
		bstream_close(stream);
	}
	
	bmessage_stop(msg);
	bmessage_close(msg);
	bdecode_window_close(window);
	bdecode_close(decode);
	bdisplay_close(display);
	bsettop_uninit();

	return 0;
error:
	return 1;
}
