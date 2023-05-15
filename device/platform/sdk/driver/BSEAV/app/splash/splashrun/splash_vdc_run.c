/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: splash_vdc_run.c $
 * $brcm_Revision: 15 $
 * $brcm_Date: 10/31/12 7:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashrun/splash_vdc_run.c $
 * 
 * 15   10/31/12 7:03p mward
 * SW7435-114:  Back out dual surface support.  Move to branch pending CFE
 * support.
 * 
 * 13   9/10/12 4:41p jessem
 * SW7425-3872: Removed hMem1 use with 7425.
 * 
 * 12   9/4/12 4:40p jessem
 * SW7425-3872: NULLed hMem1 for 7425 to reflect reassignment of GFD0 and
 * GFD1 to MEMC0.
 *
 * 11   4/19/12 4:41p mward
 * SW7435-114:  Add support for 7435.  Uses MEMC0 for graphics surface in
 * 4 transcode RTS use-case, MEMC1 for others.
 *
 * 10   4/12/12 5:05p randyjew
 * SWCFE-769: Memc0 only support
 *
 * 9   4/10/12 2:15p jessem
 * SW7425-2828: Used NEXUS_Platform_GetFrameBufferHeap to determine
 * correct heap to use.
 *
 * 8   8/23/11 10:01a jrubio
 * SW7340-277: add Support for 7340
 *
 * 7   8/22/11 5:14p jessem
 * SW7425-878: Added MEMC1 support for 7425.
 *
 * 6   8/8/11 11:54p nickh
 * SW7425-878: Add 7425 splash support
 *
 * 5   9/21/09 5:38p nickh
 * SW7420-351: Pass in the appropriate heaps for 7420
 *
 * 4   5/14/09 5:42p shyam
 * PR52592 : Add support for ARGB8888 surfaces
 *
 * 3   10/8/07 7:45p shyam
 * PR 30741 : Simplify the bitmap filename
 *
 * 2   6/28/07 5:07p shyam
 * PR 30741 : Syncup with cfe portability
 *
 * 1   5/14/07 7:17p shyam
 * PR 30741 : Add reference support for generic portable splash
 *
 * Hydra_Software_Devel/2   2/6/06 7:36a dkaufman
 * PR3481: Moved surface update code to new file
 *
 * Hydra_Software_Devel/1   11/17/05 4:49p dkaufman
 * PR3481: Added file
 *
 *
 ***************************************************************************/

#include <stdio.h>

#include "bstd.h"
#include "bmem.h"
#include "bkni.h"
#include "breg_mem.h"

#include "splash_bmp.h"

#include "splash_script_load.h"
#include "splash_surf_update.h"


uint8_t *splash_open_bmp(char *name)
{
	unsigned long end = 0;
	uint8_t *buffer ;
	int rc;
	FILE *fp = fopen(name, "rb");
	if(!fp)
	{
		perror("Failed opening file ");
		return NULL;
	}
	rc = fseek(fp, 0, SEEK_END );
	if(rc)
	{
		printf("Error fseek\n");
		return NULL ;
	}
	end = ftell(fp);

	rc = fseek(fp, 0, SEEK_SET );

 	buffer = (uint8_t *)BKNI_Malloc(end);
	rc = fread(buffer, sizeof(char), end, fp);
	if(!rc)
	{
		printf("fread failed\n");
		return NULL ;
	}
	return buffer;
}



/**************************
 * The functions that follow are only required if you are compiling with
 * versions of RDC or REG that have been modified for dumping (vdc_dump).
 */
int splash_script_run(BREG_Handle hReg, BMEM_Handle hMem0, BMEM_Handle hMem1)
{
	char*      surfaceMemory;
	uint32_t   splashHeight;
	uint32_t   splashWidth;
	void*      splashAddress;
	uint32_t   splashPitch;
	uint32_t   splashPxlformat;

	printf("******************* Splash BVN Script Loader !!!! *************************\n");

	surfaceMemory = BMEM_Alloc(
#if (BCHP_CHIP == 7420) || ((BCHP_CHIP==7435) && !NEXUS_PLATFORM_7435_BOX_TYPE_1u4t)
					hMem1,
#else
					hMem0,
#endif
					splash_get_surf_alloc_size());

	printf("** Allocating Surface Memory done **\n");
#if BCHP_CHIP == 7420 || BCHP_CHIP==7435
	splash_bvn_init(hReg, hMem0, hMem1, surfaceMemory);
#else
	splash_bvn_init(hReg, hMem0, surfaceMemory);
#endif
	printf("** BVN Hardware Init done - Start showing colors **\n");

 	/* Make something happen */
	splash_get_surf_dimensions(&splashWidth, &splashHeight);
	splash_get_surf_address(&splashAddress, &splashPitch);
	BKNI_Memset(splashAddress, 0x1A, splash_get_surf_alloc_size());

	splash_get_surf_format(&splashPxlformat) ;
	splash_setparams(splashPxlformat, splashPitch, splashWidth, splashHeight) ;

	{
		BMP_HEADER_INFO myBmpInfo ;
		int x,y ;
		uint8_t *buffer = splash_open_bmp("splash.bmp") ;
		if(buffer)
		{
			splash_bmp_getinfo(buffer, &myBmpInfo);
			printf("*******************************\n");
			printf("Width = %d Height = %d\n", myBmpInfo.info.width, myBmpInfo.info.height);
			printf("*******************************\n");
			x = (splashWidth-myBmpInfo.info.width)/2 ;
			y = (splashHeight-myBmpInfo.info.height)/2 ;
			splash_render_bmp(x, y, buffer, splashAddress) ;
			BKNI_Free(buffer);
		}
		else
			ShowColorsOnSurface(splashAddress, splashPitch, splashWidth, splashHeight);
		printf("Press any key to continue ...");
		getchar();
	}

 	printf("** Show colors on Surface done **\n");

	return 0;
}

/* end of file */
