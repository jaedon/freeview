/***************************************************************************
 *     Copyright (c) 2004-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: memview.c $s
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/15/07 9:34a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/memview.c $
 * 
 * Hydra_Software_Devel/1   10/15/07 9:34a rpan
 * PR34857: Ported from mvd_vdc_test.
 * 
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "bstd.h"
#include "bsur.h"
#include "interactive.h"


BDBG_MODULE(MEMVIEW_DUMP);


/***************************************************************************
 *
 */
bool BTST_DumpToMemViewFile
	( BSUR_Surface_Handle              hCapMem,
	  const char                      *pchMemFname )
{
	uint32_t i,j;
	uint32_t ulPitch;
	uint32_t ulWidth;
	uint32_t ulHeigth;
	uint32_t ulDwords;
	uint32_t *pulAddr;
	uint32_t ulPixelPerDword;

	/* TODO: Error checks! */
	FILE *fpMem;
	BPXL_Format ePxlFmt;
	const char *pchPxlFmt;

	BDBG_ASSERT(hCapMem);
	BDBG_ASSERT(pchMemFname);

	/* Extract informat to write to file.! */
	BSUR_Surface_GetFormat(hCapMem, &ePxlFmt);
	BSUR_Surface_GetAddress(hCapMem, (void*)&pulAddr, &ulPitch);
	BSUR_Surface_GetDimensions(hCapMem, &ulWidth, &ulHeigth);

	/* Derive value */
	BPXL_GetBytesPerNPixels(ePxlFmt, (ulHeigth * ulPitch), &ulDwords);
	ulDwords  = ulDwords/sizeof(uint32_t);
	pchPxlFmt = BPXL_ConvertFmtToStr(ePxlFmt);
	ulPixelPerDword = 32 / BPXL_BITS_PER_PIXEL(ePxlFmt);

	/* Number of dwords */
	ulDwords /= sizeof(uint32_t);

	if((fpMem = fopen(pchMemFname, "w")) == NULL)
	{
		return false;
	}

	fprintf(fpMem, "// Memory2\n");
	fprintf(fpMem, "%ul\n", ulDwords);
	fprintf(fpMem, "// block   BvnCaptureMem\n");
	fprintf(fpMem, "// type    Texture\n");
	fprintf(fpMem, "// format  %s\n", pchPxlFmt+6); /* removed the BPXL_e */
	fprintf(fpMem, "// width   d:%d\n", ulWidth);
	fprintf(fpMem, "// height  d:%d\n", ulHeigth);
	fprintf(fpMem, "// address x:%08x\n", 0);
	fprintf(fpMem, "@%08x\n",             0);

	for(i = 0; i < ulHeigth; i++)
	{
		for(j = 0; j < ulWidth/ulPixelPerDword; j++)
		{
			/* stepping one dword at at time, and each dword is two pixels */
			fprintf(fpMem, "%08x\n", pulAddr[i * ulPitch/sizeof(uint32_t) + j]);
		}
	}

	fclose(fpMem);

	return true;
}

/* end of file */
