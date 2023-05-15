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
 * $brcm_Workfile: record_ip_new.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:07p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/record_ip_new.c $
 * 
 * 2   9/6/12 6:07p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bsettop.h"
#include <stdio.h>
#include <string.h>

#ifdef __vxworks
#include <usrConfig.h>
#include <routeLib.h>
#endif

int main(int argc, char **argv) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
    brecpump_t recpump = brecpump_open(B_ID(0), NULL);
	brecord_t record = brecord_open();
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bplaypump_t playpump;
	bplayback_t playback = bplayback_ip_open();
	bplayback_socket_params socket_params;
	bplayback_file_t socket;
	bstream_mpeg mpeg;
	bstream_t stream;
	bplayback_params playback_params;
	bplaypump_open_params playpump_open_params;
    /*  Will record in the current directory. If you would like to change to hardrive change the path 
         e.g. "/data/videos/ip_stream.mpg" */
    brecord_file_t file = brecord_file_open("/data/videos/ip_stream.mpg", "/data/videos/ip_stream.nav");
	brecord_params recparams;

	BSTD_UNUSED(argc);
	BSTD_UNUSED(argv);

	if (rc) goto error;

	/* increase the buffering for IP STB to accomodate adequate "pre-charge" (see bsettop_playback_ip.c) */
	bplaypump_get_open_params(B_ID(0), &playpump_open_params);
	playpump_open_params.buffer_size *= 2;
	playpump_open_params.num_descriptors *= 2;
	playpump = bplaypump_open(B_ID(0), &playpump_open_params);


    /* Set IP Address and Port */
	bplayback_socket_params_init(&socket_params, playback);
	strcpy(socket_params.open_params.ip_addr, "192.168.1.104");
	socket_params.open_params.port = 1234;

	socket = bplayback_socket_open(playback, &socket_params);
	if (!socket) goto error;

    /* Setup PID information */
	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x44;
	mpeg.audio[0].pid = 0x45;
	mpeg.audio[0].format = baudio_format_ac3;
	mpeg.pcr_pid = 0x44;

    mpeg.video[0].format = bvideo_codec_mpeg2;
	/* mpeg.video[0].format = bvideo_codec_h264; */

	bplayback_params_init(&playback_params, playback);
	playback_params.index_format = bindex_format_none;

    brecord_params_init(&recparams, record);
	recparams.index_format = bindex_format_bcm;

	stream = bplayback_start(playback, playpump, &mpeg, socket, &playback_params);
	if (!stream) goto error;

    printf(" Recording stream!\n");
    if (brecord_start(record, recpump, stream, file, &recparams))
		goto error;

   /*  if (bdecode_start(decode, stream, window))
		goto error; */ 

	getchar(); /* press enter to stop playback and record ( not a clean exit) */
	return 0;
error:
	return 1;
}

#ifdef __vxworks
/* this gives a flat entry point for vxworks */
int playback_ip(char *arg1,char *arg2,char *arg3,char *arg4,char *arg5,
	char *arg6,char *arg7,char *arg8,char *arg9,char *arg10)
{
	char *argv[11] = {
		"userio",arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10};
	int argc;
	for (argc=1;argc<11;argc++)
		if (!argv[argc]) break;

	/* for HDD support */
	usrAtaConfig(0,0,"/ata0");
	
	/* for multicast support */
	routeAdd("0.0.0.0", "192.168.1.1");

	return main(argc,argv);
}
#endif
