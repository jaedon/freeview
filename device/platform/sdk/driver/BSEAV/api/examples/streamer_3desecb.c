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
 * $brcm_Workfile: streamer_3desecb.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/streamer_3desecb.c $
 * 
 * 6   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
/*
 * This example shows how to decrypt a network-encrypted (odd + even key) stream
 * from the streamer, using the conditional access (CA) engine.
 * 
 * This specific example demonstrates Triple DES ECB decryption.
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
		uint8_t odd[16], even[16];
	} video;
	struct {
		uint8_t odd[16], even[16];
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
	/* Assumes spiderman_3des_validkey_15seconds.ts */
	mpeg.video[0].pid = 0x11;
	mpeg.audio[0].pid = 0x14;
	mpeg.pcr_pid = 0x21;
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

	encryption.type = bencryption_type_3des;
	encryption.blockmode = bcrypto_blockmode_ecb;
	encryption.residue = bencryption_residue_cipher_text;

	encryption.key_length = 128;
	encryption.odd = true;
	encryption.even = true;

	keys.video.odd[0] = 0x01; /* Odd video */
	keys.video.odd[1] = 0xB9;
	keys.video.odd[2] = 0x91;
	keys.video.odd[3] = 0x67;
	keys.video.odd[4] = 0xE0;
	keys.video.odd[5] = 0xE3;
	keys.video.odd[6] = 0xE9;
	keys.video.odd[7] = 0xAB;
	keys.video.odd[8] = 0x07; 
	keys.video.odd[9] = 0xB9;
	keys.video.odd[10] = 0x91;
	keys.video.odd[11] = 0x67;
	keys.video.odd[12] = 0xE0;
	keys.video.odd[13] = 0xE3;
	keys.video.odd[14] = 0xE9;
	keys.video.odd[15] = 0xAB;

	keys.video.even[0] = 0x01; /* Even video */
	keys.video.even[1] = 0xB9;
	keys.video.even[2] = 0x91;
	keys.video.even[3] = 0x67;
	keys.video.even[4] = 0xE0;
	keys.video.even[5] = 0xE3;
	keys.video.even[6] = 0xE9;
	keys.video.even[7] = 0xAB;
	keys.video.even[8] = 0x07; 
	keys.video.even[9] = 0xB9;
	keys.video.even[10] = 0x91;
	keys.video.even[11] = 0x67;
	keys.video.even[12] = 0xE0;
	keys.video.even[13] = 0xE3;
	keys.video.even[14] = 0xE9;
	keys.video.even[15] = 0xAB;

	keys.audio.odd[0] = 0x01; /* Odd audio */
	keys.audio.odd[1] = 0xB9;
	keys.audio.odd[2] = 0x91;
	keys.audio.odd[3] = 0x67;
	keys.audio.odd[4] = 0xE0;
	keys.audio.odd[5] = 0xE3;
	keys.audio.odd[6] = 0xE9;
	keys.audio.odd[7] = 0xAB;
	keys.audio.odd[8] = 0x07; 
	keys.audio.odd[9] = 0xB9;
	keys.audio.odd[10] = 0x91;
	keys.audio.odd[11] = 0x67;
	keys.audio.odd[12] = 0xE0;
	keys.audio.odd[13] = 0xE3;
	keys.audio.odd[14] = 0xE9;
	keys.audio.odd[15] = 0xAB;

	keys.audio.even[0] = 0x01; /* Even audio */
	keys.audio.even[1] = 0xB9;
	keys.audio.even[2] = 0x91;
	keys.audio.even[3] = 0x67;
	keys.audio.even[4] = 0xE0;
	keys.audio.even[5] = 0xE3;
	keys.audio.even[6] = 0xE9;
	keys.audio.even[7] = 0xAB;
	keys.audio.even[8] = 0x07; 
	keys.audio.even[9] = 0xB9;
	keys.audio.even[10] = 0x91;
	keys.audio.even[11] = 0x67;
	keys.audio.even[12] = 0xE0;
	keys.audio.even[13] = 0xE3;
	keys.audio.even[14] = 0xE9;
	keys.audio.even[15] = 0xAB;


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
		
	memcpy(encryption.key, keys.audio.odd, 16);
	encryption.pid = 0x14; /* audio pid */
	encryption.odd = true; encryption.even = false;
	if (bstream_set_encryption(stream, &encryption))
		goto error;
		
	memcpy(encryption.key, keys.audio.even, 16);
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
