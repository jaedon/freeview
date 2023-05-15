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
 * $brcm_Workfile: streamer_dvb.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/streamer_dvb.c $
 * 
 * 5   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
/*
 * This example shows how to decrypt a network-encrypted (odd + even key) stream
 * from the streamer, using the conditional access (CA) engine.
 * 
 * This specific example demonstrates DVB decryption.
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

#define DECRYPT 1

typedef struct ca_key_t {
	struct {
		uint8_t odd[8], even[8];
	} video;
	struct {
		uint8_t odd[8], even[8];
	} audio;
} ca_key_t;

int main(void) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	bdecode_t decode = bdecode_open(B_ID(0));
	bdisplay_t display = bdisplay_open(B_ID(0));
	bdecode_window_t window = bdecode_window_open(B_ID(0), display);
	bstream_mpeg mpeg;
	bstream_t stream;
	bband_t band;
	bencryption_params encryption;

	ca_key_t keys;

	if (rc) goto error;

	bstream_mpeg_init(&mpeg);
	/* Assumes superscrambler_spider.dvb (11/14) */
#if 1
	mpeg.video[0].pid = 0x11;
	mpeg.audio[0].pid = 0x14;
	mpeg.pcr_pid = 0x11;
#else
	mpeg.video[0].pid = 0x21;
	mpeg.audio[0].pid = 0x24;
	mpeg.pcr_pid = 0x21;
#endif
	/* the non-scrambled pids in that file are 21/24 and 41/44 */
	mpeg.audio[0].format = baudio_format_ac3;

	/* This loads the encryption keys.
	 *
	 * Unlike earlier versions of bstream_set_encryption, the uint_8 
	 * version of the key eliminates the endian issues.
	 */

	bencryption_params_init(&encryption);

	mpeg.encryption.type = bencryption_type_none;
	mpeg.encryption.key_length = 0;

	encryption.type = bencryption_type_dvb;
	/* encryption.blockmode = bcrypto_blockmode_ecb;  -- This is not used for DVB */
	/* encryption.residue = bencryption_residue_none;  -- This is not used for DVB */

	encryption.key_length = 64;
	encryption.odd = true;
	encryption.even = true;

	keys.video.odd[0] = 0xbc; /* Odd video */
	keys.video.odd[1] = 0xfb;
	keys.video.odd[2] = 0xb2;
	keys.video.odd[3] = 0x69;
	keys.video.odd[4] = 0x13;
	keys.video.odd[5] = 0xba;
	keys.video.odd[6] = 0xbe;
	keys.video.odd[7] = 0x8b;

	keys.video.even[0] = 0xbe; /* Even video */
	keys.video.even[1] = 0xf9;
	keys.video.even[2] = 0xb0;
	keys.video.even[3] = 0x67;
	keys.video.even[4] = 0x13;
	keys.video.even[5] = 0xb8;
	keys.video.even[6] = 0xbc;
	keys.video.even[7] = 0x87;

	keys.audio.odd[0] = 0xd6; /* Odd audio */
	keys.audio.odd[1] = 0x91;
	keys.audio.odd[2] = 0xd8;
	keys.audio.odd[3] = 0x3f;
	keys.audio.odd[4] = 0x13;
	keys.audio.odd[5] = 0xd0;
	keys.audio.odd[6] = 0xd4;
	keys.audio.odd[7] = 0xb7;

	keys.audio.even[0] = 0xd3; /* Even audio */
	keys.audio.even[1] = 0x94;
	keys.audio.even[2] = 0xdd;
	keys.audio.even[3] = 0x44;
	keys.audio.even[4] = 0x13;
	keys.audio.even[5] = 0xd5;
	keys.audio.even[6] = 0xd1;
	keys.audio.even[7] = 0xb9;

	band = bstreamer_attach(B_ID(0),bstream_mpeg_type_ts);
	if (!band) goto error;

	stream = bstream_open(band, &mpeg);
	if (!stream) goto error;

	if (bdecode_start(decode, stream, window))
		goto error;
#if DECRYPT
	/* the decryption needs to be set after decode starts in order to allow 
	 * the consumer to be initialized -- without this, the internal lookup 
	 * to find the pid_channel will fail.
	 */
	memcpy(encryption.key, keys.video.odd, 8);
	encryption.pid = 0x11; /* video pid */
	encryption.odd = true; encryption.even = false;
	if (bstream_set_encryption(stream, &encryption))
		goto error;
		
	memcpy(encryption.key, keys.video.even, 8);
	encryption.pid = 0x11; /* video pid */
	encryption.odd = false; encryption.even = true;
	if (bstream_set_encryption(stream, &encryption))
		goto error;
		
	memcpy(encryption.key, keys.audio.odd, 8);
	encryption.pid = 0x14; /* audio pid */
	encryption.odd = true; encryption.even = false;
	if (bstream_set_encryption(stream, &encryption))
		goto error;
		
	memcpy(encryption.key, keys.audio.even, 8);
	encryption.pid = 0x14; /* audio pid */
	encryption.odd = false; encryption.even = true;
	if (bstream_set_encryption(stream, &encryption))
		goto error;
#endif /* DECRYPT */

	getchar(); /* press enter to stop playback */
	return 0;
error:
	return 1;
}
