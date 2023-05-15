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
 * $brcm_Workfile: xpt_xvd_test1.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/tests/magnum/xpt_xvd_test1.c $
 * 
 * 3   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop_board.h"
#include "bsettop_os.h"
#include "bvdc.h" /* specific magnum api required by this app */
#include "bsettop_base.h"
#include "bsettop_base_xpt.h"
#include "bsettop_base_xvd.h"
#include <stdio.h>

void *my_func(void *data)
{
	int i;
	for (i=0;i<10;i++) {
		BKNI_Printf("task: %d\n", i);
		BKNI_Sleep(100);
	}
	return NULL;
}

int main(void) 
{
	bresult rc;
	bstream_mpeg mpeg, mpeg_dst;
	struct b_base_pidparser *xpt_output;
	int bandnum = 0; /* streamer input */
	BXVD_DecodeSettings cfg;

	rc = bsettop_boot_board();
	BDBG_ASSERT(!rc);

	/* from bsettop_boot */	
	rc = bsettop_p_transport_init(&b_board.b7038,&b_root.transport);
	if (rc!=b_ok) { return rc; }
	rc = bsettop_p_router_init(&b_root.transport, &b_root.router);
	if (rc!=b_ok) { return rc; }
	rc = bsettop_p_xvd_init(&b_board.b7411[0], &b_root.xvd[0]);
	if (rc!=b_ok) { return rc; }
	b_root.xvd[0].chn[0].id = BAVC_SourceId_eHdDvi0;
	b_root.xvd[0].chn[0].pid_base = 0x100;
	b_root.xvd[0].chn[0].remux_in = BXPT_RemuxInput_eA;
#if 0
	rc = bsettop_p_xvd_init(&b_board.b7411[1], &b_root.xvd[1]);
	if (rc!=b_ok) { return rc; }
	b_root.xvd[1].chn[0].id = BAVC_SourceId_eHdDvi1;
	b_root.xvd[1].chn[0].pid_base = 0x200;
	b_root.xvd[1].chn[0].remux_in = BXPT_RemuxInput_eB;
#endif

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x134;
	mpeg.video[0].format = bvideo_codec_h264;

	rc = bsettop_p_transport_lock(&b_root.transport, bandnum, 
		&mpeg, &xpt_output, NULL);
	if (rc) return rc;

	rc = bsettop_p_router_route(&b_root.router, 
		xpt_output, &mpeg, &mpeg_dst, &b_root.xvd[0].chn[0]);
	if (rc) return rc;

	/* you should be able to decode mpeg_dst.video[0].pid using XVD now */	
	BXVD_GetDecodeSettings(b_root.xvd[0].chn[0].dec, &cfg);
	cfg.eVideoCmprStd = BAVC_VideoCompressionStd_eH264;
	cfg.eStreamType = BAVC_StreamType_eTsMpeg;
	cfg.ulVideoPid = mpeg_dst.video[0].pid;
	cfg.ulPcrPid = mpeg_dst.pcr_pid;
	BXVD_StartDecode(b_root.xvd[0].chn[0].dec, &cfg);
	getchar();
	
	/* stop XVD decode */
	BXVD_StopDecode(b_root.xvd[0].chn[0].dec);
	bsettop_p_router_unroute(&b_root.router, xpt_output, &b_root.xvd[0].chn[0]);
	bsettop_p_transport_release(&b_root.transport, xpt_output);
	
	/* bring down system */
	bsettop_p_xvd_shutdown(&b_root.xvd[0]);
	bsettop_p_transport_shutdown(&b_root.transport);
	bsettop_shutdown_board();
	return 0;
}
