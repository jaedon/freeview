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
 * $brcm_Workfile: transport_test3.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/transport_test3.c $
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
	bmessage_stream_params params;
	bdecode_t decode0 = bdecode_open(B_ID(0));
	bdecode_t decode1 = bdecode_open(B_ID(1));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window0 = bdecode_window_open(B_ID(0), display);
	bdecode_window_t window1 = bdecode_window_open(B_ID(1), display); /* pip */
	bstream_mpeg mpeg;
	int i;
	
	if (rc) goto error;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x21;
	mpeg.pcr_pid = 0x21;

	band = bstreamer_attach(B_ID(0), bstream_mpeg_type_ts);
	if (!band) goto error;
	
	/* run this test with msg_modules=base_xpt and verify 
	both use same input_band, different parser_bands. */
	
	bstream_t stream0 = bstream_open(band, &mpeg);
	bstream_t stream1 = bstream_open(band, &mpeg);
	
	bdecode_start(decode0, stream0, window0);
	bdecode_start(decode1, stream1, window1);
	
	BKNI_Sleep(3000);

	bdecode_stop(decode0);
	bstream_close(stream0);	
	bdecode_stop(decode1);
	bstream_close(stream1);	
	bdecode_window_close(window0);
	bdecode_close(decode0);
	bdecode_window_close(window1);
	bdecode_close(decode1);
	bdisplay_close(display);
	bsettop_uninit();

	return 0;
error:
	return 1;
}