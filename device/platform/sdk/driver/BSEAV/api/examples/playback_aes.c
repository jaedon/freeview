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
 * $brcm_Workfile: playback_aes.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 9/6/12 6:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/playback_aes.c $
 * 
 * 10   9/6/12 6:05p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
/*
 * This example shows how to re-route a network-encrypted (odd + even key) stream
 * through playback to use the conditional access (CA) engine to do the decryption.
 * 
 * This specific example demonstrates AES-CBC decryption.
 * 
 * This example is NOT intended for playing back encrypted PVR files.  For those, 
 * the correct method is to initialize the mpeg.encryption structure when initializing 
 * the file for playback.
 */

#include "bsettop.h"
#include <stdio.h>
#ifdef __vxworks
#include "usrConfig.h"
#endif

#define REROUTE 1
#define DECRYPT 1

typedef struct ca_key_t {
	struct {
		uint8_t odd[16], even[16];
	} video;
} ca_key_t;

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bplaypump_t playpump = bplaypump_open(B_ID(0), NULL);
	bplayback_t playback = bplayback_open();
	bplayback_file_t file;
	bstream_mpeg mpeg;
	bstream_t stream;
	bplayback_params playback_params;
	bencryption_params encryption;

	ca_key_t keys;

	if (rc) goto error;

#ifdef __vxworks
	usrAtaConfig(0,0,"/ata0");
#endif	

#ifndef __vxworks	
	file = bplayback_file_open("spiderman_aes.ts", NULL);
#else
	file = bplayback_file_open("/ata0/aes_stream.mpg", NULL);
#endif	
	if (!file) goto error;

	bstream_mpeg_init(&mpeg);
	mpeg.video[0].pid = 0x11;
	mpeg.audio[0].pid = 0x14;
	mpeg.pcr_pid = 0x11;
	mpeg.audio[0].format = baudio_format_ac3;

	/* This loads the encryption keys.
	 *
	 * Note that this example uses matching keys.  This is unlikely in 
	 * most scenarios, but by assigning the keys separately, the normal 
	 * use can be preserved.
	 *
	 * It is *required* that a pidparser band be set up during playback 
	 * in order to route the playback such that it goes through the 
	 * CA block.
	 *
	 * Previously, this was handled by assigning a band in the playback
	 * structure.  Now, it is a simple boolean.
	 */

	bencryption_params_init(&encryption);

	mpeg.encryption.type = bencryption_type_none;
	mpeg.encryption.key_length = 0;

	encryption.type = bencryption_type_aes;
	encryption.blockmode = bcrypto_blockmode_cbc;
	encryption.residue = bencryption_residue_block_term;

	encryption.key_length = 128;
	encryption.odd = true;
	encryption.even = true;

	keys.video.odd[ 0] = 0x12; /* Odd video */
	keys.video.odd[ 1] = 0x34;
	keys.video.odd[ 2] = 0x56;
	keys.video.odd[ 3] = 0x78;
	keys.video.odd[ 4] = 0x9a;
	keys.video.odd[ 5] = 0xbc;
	keys.video.odd[ 6] = 0xde;
	keys.video.odd[ 7] = 0xf0;
	keys.video.odd[ 8] = 0x12;
	keys.video.odd[ 9] = 0x34;
	keys.video.odd[10] = 0xfe;
	keys.video.odd[11] = 0xed;
	keys.video.odd[12] = 0xba;
	keys.video.odd[13] = 0xbe;
	keys.video.odd[14] = 0xbe;
	keys.video.odd[15] = 0xef;

	keys.video.even[ 0] = 0x12; /* Even video */
	keys.video.even[ 1] = 0x34;
	keys.video.even[ 2] = 0x56;
	keys.video.even[ 3] = 0x78;
	keys.video.even[ 4] = 0x9a;
	keys.video.even[ 5] = 0xbc;
	keys.video.even[ 6] = 0xde;
	keys.video.even[ 7] = 0xf0;
	keys.video.even[ 8] = 0x12;
	keys.video.even[ 9] = 0x34;
	keys.video.even[10] = 0xfe;
	keys.video.even[11] = 0xed;
	keys.video.even[12] = 0xba;
	keys.video.even[13] = 0xbe;
	keys.video.even[14] = 0xbe;
	keys.video.even[15] = 0xef;

	bplayback_params_init(&playback_params, playback);
#if REROUTE
	playback_params.route_through_parser_band = true;
#endif /* REROUTE */
	playback_params.index_format = bindex_format_bcm;

	stream = bplayback_start(playback, playpump, &mpeg, file, &playback_params);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;

#if DECRYPT
	/* Technically, since the keys are the same for both odd and even for the test 
	 * stream, both encryption.odd and encryption.even can be true in this case.
	 *
	 * However, this template allows them to be adjusted for other streams with only 
	 * the key data being replaced in the structure above.
	 */
	memcpy(encryption.key, keys.video.odd, 16);
	encryption.pid = 0x11; /* video pid */
	encryption.odd = true; encryption.even = false;
	if (bstream_set_encryption(stream, &encryption))
		goto error;

	memcpy(encryption.key, keys.video.even, 16);
	encryption.pid = 0x11; /* video pid */
	encryption.odd = false; encryption.even = true;
	if (bstream_set_encryption(stream, &encryption))
		goto error;
#endif /* DECRYPT */

	getchar(); /* press enter to stop playback */
	return 0;
error:
	return 1;
}
