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
 * $brcm_Workfile: indexfrompts_test.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/7/12 10:07a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/tests/indexfrompts_test.cpp $
 * 
 * 2   9/7/12 10:07a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "bcmplayer.h"

int main(int argc, char **argv) {
	if (argc < 4) {
		printf("Usage: ./indexfrompts_test INDEXFILE HEXPTS DECIMAL_SEARCH_WINDOW\n");
		exit(1);
	}

	const char *indexfile = argv[1];
	unsigned long gotopts = strtoul(argv[2], NULL, 16);
	unsigned long searchwindow = atoi(argv[3]);
	unsigned long pts;
	
	printf("Params:\n");
	printf("  bcmindex file: %s\n", indexfile);
	printf("  goto pts: %#x\n", gotopts);
	printf("  search window: %d\n", searchwindow);

	FILE *f = fopen(argv[1], "rw");

	BNAV_Player_Handle bcmp;
	BNAV_Player_Settings settings;
	
	BNAV_Player_GetDefaultSettings(&settings);
	settings.readCb = (BP_READ_CB)fread;
	settings.seekCb = (BP_SEEK_CB)fseek;
	settings.tellCb = (BP_TELL_CB)ftell;
	settings.filePointer = f;
	settings.videoPid = 1; // doesn't matter

	if (BNAV_Player_Open(&bcmp, &settings))
		exit(1);

	long index = BNAV_Player_FindIndexFromPts(bcmp, gotopts, searchwindow);
	if (index != -1) {
		BNAV_Player_Position pos;
		BNAV_Player_GetPosition(bcmp, &pos);
		printf("Found: %d, pts %#x\n", index, pos.pts);
	}
	else
		printf("Failed.\n");


	BNAV_Player_Close(bcmp);
	return 0;
}
