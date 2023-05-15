/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: systeminit.h $
* $brcm_Revision: Hydra_Software_Devel/5 $
* $brcm_Date: 10/16/12 1:09p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/common/appframework/systeminit.h $
* 
* Hydra_Software_Devel/5   10/16/12 1:09p hongtaoz
* SW7445-8: added 3rd memc;
* 
* Hydra_Software_Devel/4   4/25/11 11:47a tdo
* SW7425-315: Add rockford/appframework support for upper memory
* range(>256MB) of MEMC0
*
* Hydra_Software_Devel/3   2/5/07 4:36p syang
* PR 23354: add mmap for mem ctrl 1
*
* Hydra_Software_Devel/2   7/7/06 1:52p maivu
* PR 20495: Modified to support 7411E0 and future revisions
*
* Hydra_Software_Devel/1   6/16/06 4:01p maivu
* PR 22212: Move to common directory
*
* Hydra_Software_Devel/6   2/14/06 2:37p maivu
* PR 19599: Use BCHP_7411_VER
*
* Hydra_Software_Devel/5   2/13/06 1:56p maivu
* PR 19599: Fixed 97398 builts
*
* Hydra_Software_Devel/4   2/10/06 5:02p maivu
* PR 19599: Fixed errors with prevous check-in for non-97398 builts
*
* Hydra_Software_Devel/3   2/10/06 3:30p maivu
* PR 19599: Add 7411 support for 97398
*
* Hydra_Software_Devel/2   7/27/05 3:59p jasonh
* PR 16397: General clean-up
*
* Hydra_Software_Devel/7   11/3/04 6:42p hongtaoz
* PR13076: added cached memory support;
*
* Hydra_Software_Devel/6   9/20/04 1:34p darnstein
* PR 10943: Add function to allow changing the heap starting address
* from 0x02000000 to anything else.  Use this for dual PCI board.
*
* Hydra_Software_Devel/5   2/4/04 7:57a marcusk
* PR9588: Added support for fpga, tested and working.
*
* Hydra_Software_Devel/4   10/2/03 8:57p dlwin
* Added initialization code for Bcm3250 chip, Bcm3250 Interrupts, and
* Upg Interrupt.
*
* Hydra_Software_Devel/3   9/16/03 2:46p aram
* added argc and argv
*
* Hydra_Software_Devel/2   9/16/03 9:30a aram
* charged a data type
*
* Hydra_Software_Devel/1   9/15/03 10:14a aram
* genesis of these files
***************************************************************************/

#ifndef SYSTEMINIT_H__
#define SYSTEMINIT_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "platform.h"

typedef struct BSystem_Info
{
		/* memory manger info for main memory controler */
		void         *bmemAddress;
		void         *bmemCachedAddress;
		size_t        bmemOffset;
		size_t        bmemSize;
		unsigned int  bmemAlignment;

#ifdef MEMC_0_MEM_UBASE
		/* upper memory manger info for main memory controler */
		void         *bmemAddressU;
		void         *bmemCachedAddressU;
		size_t        bmemOffsetU;
		size_t        bmemSizeU;
		unsigned int  bmemAlignmentU;
#endif

#ifdef MEMC_1_MEM_PBASE
		/* memory manger info for 2nd memory controler */
		void         *bmemAddress1;
		void         *bmemCachedAddress1;
		size_t        bmemOffset1;
		size_t        bmemSize1;
		unsigned int  bmemAlignment1;
#endif

#ifdef MEMC_2_MEM_PBASE
		/* memory manger info for 2nd memory controler */
		void         *bmemAddress2;
		void         *bmemCachedAddress2;
		size_t        bmemOffset2;
		size_t        bmemSize2;
		unsigned int  bmemAlignment2;
#endif

		/* register info */
		void         *bregBaseAddr;
		size_t        bregMaxRegOffset;


#ifdef BCHP_7411_VER
		void         *breg7411BaseAddr[2];
		size_t        breg7411MaxRegOffset[2];
#endif

		void         *bregFpgaBaseAddr;
		size_t        bregFpgaMaxRegOffset;

} BSystem_Info;

int BSystem_Init(int argc, char *argv[], BSystem_Info *pSystemInfo);
int BSystem_Uninit(BSystem_Info *pSystemInfo);


#ifdef __cplusplus
}
#endif

#endif

