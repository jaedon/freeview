/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: systeminit.c $
* $brcm_Revision: Hydra_Software_Devel/7 $
* $brcm_Date: 9/14/12 2:29p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/common/appframework/systeminit.c $
* 
* Hydra_Software_Devel/7   9/14/12 2:29p darnstein
* SW7346-572: intmgr init/uninit now handled in BFramework.
* 
* Hydra_Software_Devel/6   10/29/10 7:18p hongtaoz
* SW7425-9: fixed client shutdown order;
* 
* Hydra_Software_Devel/5   9/28/10 2:51p hongtaoz
* SW7425-10: adding generic interrupt polling support for all PI modules;
* 
* Hydra_Software_Devel/4   4/28/09 3:15p jessem
* PR 54380: Changed all references to IKOS to EMULATION.
* 
* Hydra_Software_Devel/3   10/16/07 3:51p jessem
* PR 36129: Added support for IKOS emulation platform.
* 
* Hydra_Software_Devel/2   3/31/07 5:47p shyam
* PR 29285 : Added linuxclient support for simulations/emulation
* 
* Hydra_Software_Devel/1   6/16/06 4:01p maivu
* PR 22212: Move to common directory
* 
* Hydra_Software_Devel/2   7/27/05 3:55p jasonh
* PR 16397: General clean-up. Removed SetMemoryOffset
* 
* Hydra_Software_Devel/8   10/7/04 5:26p pntruong
* PR12728: Fixed -pdantic build warnings and errors.
* 
* Hydra_Software_Devel/7   9/20/04 1:44p darnstein
* PR 10943: Fix a stupid copy-and-paste error (previous revision).
* 
* Hydra_Software_Devel/6   9/20/04 1:34p darnstein
* PR 10943: Add function to allow changing the heap starting address
* from 0x02000000 to anything else.  Use this for dual PCI board.
* 
* Hydra_Software_Devel/5   1/21/04 5:36p marcusk
* PR9436: Properly return error.
* 
* Hydra_Software_Devel/4   9/29/03 12:04p aram
* integrated with int1
* 
* Hydra_Software_Devel/3   9/16/03 3:06p aram
* fixed typo
* 
* Hydra_Software_Devel/2   9/16/03 2:46p aram
* added argc and argv
* 
* Hydra_Software_Devel/1   9/15/03 10:14a aram
* genesis of these files
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bstd.h"
#include "systeminit.h"
#include "memmgr.h"
#include "intmgr.h"
#ifdef EMULATION
#include "client.h"
#endif

int
BSystem_Init(int argc, char *argv[], BSystem_Info *pSystemInfo)
{
	int	ret;

#ifdef EMULATION /* client socket ready before intmgr in case register access in intmgr */
	{
		int i;
		bool bNoIp = true;

		for (i=0; i<argc; i++)
		{
			if (!(strcmp(argv[i],"-S")))
			{
				bNoIp = false;
				BEMU_Client_Open();
				/* connect to emulator server */
				if (BEMU_Client_OpenSocket(argv[++i]) == 0)
				{
					ret = 1;
				}
			}
		}

		if (bNoIp)
		{
			fprintf(stderr, "Specify emulator server's IP address\n");
			ret = 1;
		}
	}
#endif

	/* Initialize memory mappings */
	ret = memmgr_Init(argc, argv, pSystemInfo);
	if( ret != 0 )
	{
		goto done;
	}

done:
	return ret;
}

int
BSystem_Uninit(BSystem_Info *pSystemInfo)
{
	BSTD_UNUSED(pSystemInfo);
	
	memmgr_Done();

#ifdef EMULATION
	/* disconnect from emulator server */
	BEMU_Client_CloseSocket();
	BEMU_Client_Close();
#endif

	return 0;
}

