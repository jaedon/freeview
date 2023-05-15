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
 * $brcm_Workfile: mfont.cpp $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/7/12 10:06a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mfont.cpp $
 * 
 * 4   9/7/12 10:06a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "mfont.h"
#include "mapplication.h"
#include "mapp_priv.h"
#include "bwin.h"
#include <stdlib.h>

#define MAXFONTNAME 128

BDBG_MODULE(mfont);

MFont::MFont(MApplication *app, const char *name, int size, bool antialias, bool kerning) {
	char buf[MAXFONTNAME];
	char *s;

	_app = app;

	/* check for encoded name: "name;size;AA;PK" */
	if ((s = strchr(name, ';')) && size == 0) {
		char *end;

		antialias = false; // switch the default

		// get the name
		strncpy(buf, name, MAXFONTNAME);
		s = buf + (s-name);
		*s++ = 0;
		name = buf;

		// get the size
		end = strchr(s, ';');
		if (end) *end = 0;
		size = atoi(s);

		// get optional AA or PK
		while (end) {
			s = end+1;
			end = strchr(s, ';');
			if (end) *end = 0;
			if (!strcmp(s, "AA"))
				antialias = true;
			if (!strcmp(s, "PK"))
				kerning = true;
		}
	}

	_id = (int)bwin_open_font(app->_private->win_engine, name, size, antialias);
	BDBG_MSG(("font: %s, %d, AA=%d, PK=%d, id=%d\n", name, size, antialias, kerning, _id));
}
