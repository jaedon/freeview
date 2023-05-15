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
 * $brcm_Workfile: vchip_createindex_test.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/7/12 10:07a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/tests/vchip_createindex_test.cpp $
 * 
 * 2   9/7/12 10:07a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "bcmindexer.h"
#include "bcmplayer.h"

/**
This test creates a NAV file from an SCT file, incrementing a vchip state.
The NAV file is then read back using bcmplayer. The output should be
the same as running the printindex utility as followed:

./printindex -o index,vchip NAVFILE

**/

/**
* Create the NAV file from an SCT file, inserting vchip information.
**/
void createindex(const char *sctfile, const char *navfile)
{

	FILE *sct = fopen(sctfile, "r");
	if (!sct) {
		printf("Cannot open %s to read: %d\n", sctfile, errno);
		exit(1);
	}

	FILE *nav = fopen(navfile, "w+");
	if (!nav) {
		printf("Cannot open %s to read: %d\n", navfile, errno);
		exit(1);
	}

	BNAV_Indexer_Handle bcmi;
	BNAV_Indexer_Settings settings;

	BNAV_Indexer_GetDefaultSettings(&settings);
	settings.writeCallback = (BNAV_WRITE_CB)fwrite;
	settings.filePointer = nav;

	if (BNAV_Indexer_Open(&bcmi, &settings))
		exit(1);

	int vchip = 0;
	while (!feof(sct)) {
#define BUFSIZE 4096
		char buf[BUFSIZE];
		int n = fread(buf, 1, BUFSIZE, sct);
		if (n == -1) {
			printf("fread error: %d\n", errno);
			exit(1);
		}
		if (BNAV_Indexer_SetVChipState(bcmi, (vchip++ | 0x4040)))
			printf("Error setting vchip\n");
		BNAV_Indexer_Feed(bcmi, (BSCT_Entry*)buf, n / sizeof(BSCT_Entry));
	}

	BNAV_Indexer_Close(bcmi);
	fclose(sct);
	fclose(nav);
}

/**
* Read the vchip information back out using the player.
**/
void readindex(const char *navfile)
{
	BNAV_Player_Handle handle;
	BNAV_Player_Settings settings;
	FILE *f = fopen(navfile, "r");
	
	if (!f)
		exit(1);

	BNAV_Player_GetDefaultSettings(&settings);
	settings.readCb = (BP_READ_CB)fread;
	settings.seekCb = (BP_SEEK_CB)fseek;
	settings.tellCb = (BP_TELL_CB)ftell;
	settings.filePointer = f;
	if (BNAV_Player_Open(&handle, &settings))
		exit(1);

	for (int i=0;!BNAV_Player_SetCurrentIndex(handle, i);i++) {
		BNAV_Player_Position pos;
		BNAV_Player_GetPosition(handle, &pos);
		printf("%d: 0x%04x\n", i, pos.vchipState);
	}

	BNAV_Player_Close(handle);
}

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Usage: ./vchip_createindex_test SCTFILE NAVFILE\n");
		exit(1);
	}

	const char *sctfile = argv[1];
	const char *navfile = argv[2];

	createindex(sctfile, navfile);
	readindex(navfile);
	return 0;
}
