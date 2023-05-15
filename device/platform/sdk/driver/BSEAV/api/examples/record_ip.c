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
 * $brcm_Workfile: record_ip.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/record_ip.c $
 * 
 * 3   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "bstd.h"
#include "bsettop.h"
#include <stdio.h>
#include <string.h>

#ifdef __vxworks
#include <usrConfig.h>
#include <routeLib.h>
#endif

BDBG_MODULE(record_ip_app);

char record_path[256];

void process_cmd_line(int argc, char *argv[]);
void usage(void);

int main(int argc, char **argv) {
	bresult rc = bsettop_init(BSETTOP_VERSION);
	brecord_t record = brecord_ip_open();
	brecord_file_t file;
	brecord_params  params;
	brecord_socket_params socket_params;
	bstream_t socket;

	if (rc) goto error;

	process_cmd_line(argc, argv);

	brecord_socket_params_init(&socket_params, record);
	strcpy(socket_params.ip_addr, "224.0.0.10");
	socket_params.port = 1234;

	socket = brecord_socket_open(record, &socket_params);
	if (!socket) goto error;

	file = brecord_file_open(record_path, NULL);
	
	brecord_params_init(&params, record);
	params.index_format = bindex_format_none;

	if (brecord_start(record, NULL, socket, file,  &params) != b_ok) 
	{
		BDBG_ERR(("brecord_start() returned failure!!! IP recording not working..."));
		goto error;
	}

	getchar(); /* press enter to stop playback */
	return 0;
error:
	return 1;
}

/* 
 * Processes the command line parameters
 */
void process_cmd_line(int argc, char *argv[])
{
	int i;

	if(argc < 3)
	{
		usage();
		exit(1);
	}

	for (i=1; i < argc; i++) 
	{
		if (!strcmp("-r", argv[i])) 
		{
			if (++i >= argc)
			{
				usage();
			}
			strcpy(record_path, argv[i]);
		}
		else 
		{
			usage();
		}
	}
}


void usage(void) 
{
	BDBG_MSG(("Usage: record_ip -r <record directory>"));
}

#ifdef __vxworks
/* this gives a flat entry point for vxworks */
int record_ip(char *arg1,char *arg2,char *arg3,char *arg4,char *arg5,
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
