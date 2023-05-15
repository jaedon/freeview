/***************************************************************************
*     Copyright (c) 2005-2006, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: main.c $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 6/16/06 4:08p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/linuxuser/main.c $
* 
* Hydra_Software_Devel/1   6/16/06 4:08p maivu
* PR 22212: Move to linuxuser directory
* 
* Hydra_Software_Devel/2   7/27/05 6:06p jasonh
* PR 16397: Fixed define of app_main.
* 
* Hydra_Software_Devel/1   7/27/05 3:53p jasonh
* PR 16397: Initial version.
* 
***************************************************************************/

#include "framework.h"

/* 
 * main() is included here to allow implementations to work as-is on
 * platforms that don't expose main() to the end-user (IKOS, VxWorks)
 */
int main(int argc, char *argv[])
{
	/* In this case, simply call the application main routine */
	return app_main(argc, argv);
}

/* End of File */
