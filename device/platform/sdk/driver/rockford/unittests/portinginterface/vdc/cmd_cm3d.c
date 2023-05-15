/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: cmd_cm3d.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 8/25/11 10:44a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/cmd_cm3d.c $
 * 
 * Hydra_Software_Devel/8   8/25/11 10:44a tdo
 * SW7420-2030: Add apply changes error tracking capability
 *
 * Hydra_Software_Devel/7   7/10/09 5:53p rpan
 * PR55188: Convert scanf() to BTST_P_Scanf().
 *
 * Hydra_Software_Devel/6   5/13/09 12:34a pntruong
 * PR51108: Coverity Defect ID:13602 OVERRUN_STATIC.  Fixed unitialized
 * values.
 *
 * Hydra_Software_Devel/5   6/6/08 5:33p jessem
 * PR 39335: Corrected user interface.
 *
 * Hydra_Software_Devel/4   6/5/08 9:38a jessem
 * PR 39335: Modified CM3D tests.
 *
 * Hydra_Software_Devel/3   6/4/08 11:20a jessem
 * PR 39335: Modified non-overlapped region data. Added enable/disable
 * region and demo functions.
 *
 * Hydra_Software_Devel/2   3/31/08 7:40p pntruong
 * PR 39335: Fixed build errors for non-3548.
 *
 * Hydra_Software_Devel/1   3/31/08 3:39p jessem
 * PR 39335: Initial version.
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "test_util.h"
#include "commands.h"
#include "memview.h"
#include "interactive.h"
#include "bvdc_dbg.h"
#include "bkni_multi.h"
#include "bdbg.h"
#include "bstd_defs.h"
#include "bvdc.h"

typedef struct
{
	uint32_t ulPwlInput;
	uint32_t aulPoints[BVDC_MAX_CM3D_PWL_POINTS];
} BTST_P_Cm3dPwlCfg;

typedef struct
{
	uint16_t auiEnable[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiFormat[BVDC_MAX_CM3D_REGIONS];
	uint32_t aulMinX[BVDC_MAX_CM3D_REGIONS];
	uint32_t aulMaxX[BVDC_MAX_CM3D_REGIONS];
	uint32_t aulMinY[BVDC_MAX_CM3D_REGIONS];
	uint32_t aulMaxY[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj0_Gain0Input[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj0_Gain1Input[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj0_Gain2Input[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj1_Gain0Input[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj1_Gain1Input[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj1_Gain2Input[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj2_Gain0Input[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj2_Gain1Input[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj2_Gain2Input[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj3_Gain0Input[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj3_Gain1Input[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj3_Gain2Input[BVDC_MAX_CM3D_REGIONS];
	int32_t  ailGainAdj0_PeakGain[BVDC_MAX_CM3D_REGIONS];
	int32_t  ailGainAdj1_PeakGain[BVDC_MAX_CM3D_REGIONS];
	int32_t  ailGainAdj2_PeakGain[BVDC_MAX_CM3D_REGIONS];
	int32_t  ailGainAdj3_PeakGain[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj0_Algo[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj1_Algo[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj2_Algo[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiGainAdj3_Algo[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiCrOffsetSel[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiCbOffsetSel[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiSatGainSel[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiLumaOffsetSel[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiLumaGainSel[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiAlphaSel[BVDC_MAX_CM3D_REGIONS];
	uint16_t auiOutputFormat[BVDC_MAX_CM3D_REGIONS];
} BTST_P_Cm3dRegionCfg;

BTST_P_Cm3dRegionCfg astCm3dRegionCfg =
{
/*Enable*/	        {1,   1,   1,   1,   0,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0},
/*Format*/	        {1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0},
/*MinX*/	        {350, 440, 560, 640, 710, 140, 200, 280, 0,   0,   0,   0,   0,   0,   0,   0},
/*MaxX*/	        {439, 559, 639, 709, 859, 199, 279, 349, 0,   0,   0,   0,   0,   0,   0,   0},
/*MinY*/	        {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*MaxY*/	        {732, 732, 732, 732, 732, 732, 732, 732, 0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj0_Gain0In*/{2,   2,   2,   2,   2,   2,   2,   2,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj0_Gain1In*/{3,   3,   3,   3,   3,   3,   3,   3,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj0_Gain2In*/{1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj1_Gain0In*/{2,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj1_Gain1In*/{3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj1_Gain2In*/{1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj2_Gain0In*/{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj2_Gain1In*/{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj2_Gain2In*/{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj3_Gain0In*/{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj3_Gain1In*/{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj3_Gain2In*/{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj0_PeakGai*/{60,  30,  80,  20,  0,   20,  70,  25,  0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj1_PeakGai*/{12,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj2_PeakGai*/{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj3_PeakGai*/{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj0_Algo*/	{1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj1_Algo*/	{1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj2_Algo*/	{1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0},
/*GainAdj3_Algo*/	{1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0},
/*CrOffsetSel*/	    {3,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*CbOffsetSel*/	    {2,   2,   2,   2,   2,   2,   2,   2,   0,   0,   0,   0,   0,   0,   0,   0},
/*SatGainSel*/	    {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*LumaOffsetSel*/	{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*LumaGainSel*/	    {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},
/*AlphaSel*/	    {1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0},
/*OutputFormat*/	{1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0}
};

BTST_P_Cm3dPwlCfg astCm3dPwlCfg[] =
{
	/* Column: PWL input, X(odd row) or Y(even row) Point for 16 regions */
	/* Row: PWL # and PWL point alternating between X (odd row) and Y (even row) */
	{3, {350,       370,       390,       395,       396,       397,       430,       440} },
	{3, {  0,         0,    (1<<8),    (1<<8),    (1<<8),    (1<<8),    (1<<8),         0} },
	{4, {  0,       100,       200,       420,       480,       500,       650,       730} },
	{4, {  0,    (1<<8),    (1<<8),    (1<<8),    (1<<8),    (1<<8),         0,         0} },
	{4, {  0,       100,       200,       420,       480,       510,       712,       730} },
	{4, {  0,    (1<<8),    (1<<8),    (1<<8),    (1<<8),         0,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       110,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },

	{3, {440,       510,       520,       530,       541,       545,       550,       560} },
	{3, {  0,         0,    (1<<8),    (1<<8),    (1<<8),    (1<<8),    (1<<8),         0} },
	{4, {  0,       100,       200,       420,       520,       710,       712,       730} },
	{4, {  0,    (1<<8),    (1<<8),    (1<<8),    (1<<8),    (1<<8),         0,         0} },
	{2, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{2, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       110,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },

	{3, {560,       580,       590,       592,       600,       630,       636,       640} },
	{3, {  0,         0,    (1<<8),    (1<<8),    (1<<8),    (1<<8),         0,         0} },
	{4, {  0,       100,       200,       420,       540,       660,       712,       730} },
	{4, {  0,    (1<<8),    (1<<8),    (1<<8),    (1<<8),         0,         0,         0} },
	{2, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{2, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       110,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },

	{3, {640,       660,       664,       665,       669,       670,       700,       710} },
	{3, {  0,         0,    (1<<8),    (1<<8),    (1<<8),    (1<<8),    (1<<8),         0} },
	{4, {  0,       100,       200,       420,       520,       710,       712,       730} },
	{4, {  0,   -(1<<8),   -(1<<8),   -(1<<8),         0,         0,         0,         0} },
	{2, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{2, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },
	{3, {  0,       110,       200,       300,       400,       600,       800,      1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,         0} },

	{3, {710,       711,       712,       713,       718,       729,       745,       860} },
	{3, {  0,    (1<<8),    (1<<8),    (1<<8),    (1<<8),    (1<<8),    (1<<8),         0} },
	{4, {  0,       100,       200,       420,       520,       710,       712,       730} },
	{4, {  0,    (1<<8),    (1<<8),    (1<<8),    (1<<8),    (1<<8),         0,         0} },
	{2, {  0,       100,       200,       300,       400,       600,       800,      1023} },
	{2, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       110,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },

	{3, {140,       150,       160,       170,       180,       185,       190,        200} },
	{3, {  0,   ( 1<<8),    (1<<8),    (1<<8),    (1<<8),    (1<<8),    (1<<8),          0} },
	{4, {  0,       100,       200,       420,       550,       710,       712,        730} },
	{4, {  0,         0,    (1<<8),    (1<<8),         0,         0,         0,          0} },
	{2, {  0,       100,       200,       300,      400,       600,        800,       1023} },
	{2, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       110,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },

	{3, {200,       220,       240,       245,       250,       260,       270,        280} },
	{3, {  0,         0,    (1<<8),    (1<<8),    (1<<8),    (1<<8),    (1<<8),          0} },
	{4, {  0,       100,       200,       540,       600,       710,       712,        730} },
	{4, {  0,         0,    (1<<8),    (1<<8),         0,         0,         0,          0} },
	{2, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{2, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       110,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },

	{3, {280,       296,       306,       312,       313,       325,       335,        350} },
	{3, {  0,         0,    (1<<8),    (1<<8),    (1<<8),    (1<<8),         0,          0} },
	{4, {  0,       100,       200,      420,       520,        710,        712,       730} },
	{4, {  0,    (1<<8),    (1<<8),    (1<<8),    (1<<8),         0,         0,          0} },
	{2, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{2, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       100,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },
	{3, {  0,       110,       200,       300,       400,       600,       800,       1023} },
	{3, {  0, (2<<8)/10,    (1<<8),    (1<<8),    (1<<8), (8<<8)/10,         0,          0} },

	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },

	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },

	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },

	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },

	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },

	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },

	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },

	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
	{0, {  0,         1,        2,         3,          4,         5,         6,          7} },
	{0, {  0,         0,        0,         0,          0,         0,         0,          0} },
};

BDBG_MODULE(BTST);


void  BTST_P_Cm3dBlueStretch
	( BVDC_Cm3dOverlappedRegion *pstBlueStretch )
{
	/* clear struct */
	BKNI_Memset((void *)pstBlueStretch, 0x0, sizeof(BVDC_Cm3dOverlappedRegion));

#if 0
	/* setup region cfg */
	pstBlueStretch->ilCrPeakGain = -17;
	pstBlueStretch->ilCbPeakGain = 70;

	pstBlueStretch->astPwl[0].ePwlInput = BVDC_Cm3dPwlInput_eSat;
	pstBlueStretch->astPwl[1].ePwlInput = BVDC_Cm3dPwlInput_eLuma;

	/* setup PWLs */
	pstBlueStretch->ulPwlCnt = 2;
	pstBlueStretch->astPwl[0].astPwlPoint[0].ulX = 0 ;
	pstBlueStretch->astPwl[0].astPwlPoint[0].ilY = 1 << 8;
	pstBlueStretch->astPwl[0].astPwlPoint[1].ulX = 40;
	pstBlueStretch->astPwl[0].astPwlPoint[1].ilY = 1 << 8;
	pstBlueStretch->astPwl[0].astPwlPoint[2].ulX = 170;
	pstBlueStretch->astPwl[0].astPwlPoint[2].ilY = 0 << 8;
	pstBlueStretch->astPwl[0].astPwlPoint[3].ulX = 320;
	pstBlueStretch->astPwl[0].astPwlPoint[3].ilY = 0 << 8;

	pstBlueStretch->astPwl[1].astPwlPoint[0].ulX = 420;
	pstBlueStretch->astPwl[1].astPwlPoint[0].ilY = 0 << 8;
	pstBlueStretch->astPwl[1].astPwlPoint[1].ulX = 500;
	pstBlueStretch->astPwl[1].astPwlPoint[1].ilY = (1 << 8)/3;
	pstBlueStretch->astPwl[1].astPwlPoint[2].ulX = 600;
	pstBlueStretch->astPwl[1].astPwlPoint[2].ilY = 1 << 8;
	pstBlueStretch->astPwl[1].astPwlPoint[3].ulX = 1023;
	pstBlueStretch->astPwl[1].astPwlPoint[3].ilY = 1 << 8;
#else
	/* setup region cfg */
	pstBlueStretch->ilCrPeakGain = -17;
	pstBlueStretch->ilCbPeakGain = 500;

	pstBlueStretch->astPwl[0].ePwlInput = BVDC_Cm3dPwlInput_eSat;
	pstBlueStretch->astPwl[1].ePwlInput = BVDC_Cm3dPwlInput_eLuma;

	/* setup PWLs */
	pstBlueStretch->ulPwlCnt = 2;
	pstBlueStretch->astPwl[0].astPwlPoint[0].ulX = 0;
	pstBlueStretch->astPwl[0].astPwlPoint[0].ilY = 1 << 8;
	pstBlueStretch->astPwl[0].astPwlPoint[1].ulX = 1;
	pstBlueStretch->astPwl[0].astPwlPoint[1].ilY = 1 << 8;
	pstBlueStretch->astPwl[0].astPwlPoint[2].ulX = 2;
	pstBlueStretch->astPwl[0].astPwlPoint[2].ilY = 1 << 8;
	pstBlueStretch->astPwl[0].astPwlPoint[3].ulX = 3;
	pstBlueStretch->astPwl[0].astPwlPoint[3].ilY = 1 << 8;

	pstBlueStretch->astPwl[1].astPwlPoint[0].ulX = 0;
	pstBlueStretch->astPwl[1].astPwlPoint[0].ilY = 1 << 8;
	pstBlueStretch->astPwl[1].astPwlPoint[1].ulX = 1;
	pstBlueStretch->astPwl[1].astPwlPoint[1].ilY = 1 << 8;
	pstBlueStretch->astPwl[1].astPwlPoint[2].ulX = 2;
	pstBlueStretch->astPwl[1].astPwlPoint[2].ilY = 1 << 8;
	pstBlueStretch->astPwl[1].astPwlPoint[3].ulX = 3;
	pstBlueStretch->astPwl[1].astPwlPoint[3].ilY = 1 << 8;
#endif

	pstBlueStretch->bEnable = true;
}

void BTST_P_Cm3dRedBoost
	( BVDC_Cm3dRegion *pstRedBoost )
{
	/* clear struct */
	BKNI_Memset((void *)pstRedBoost, 0x0, sizeof(BVDC_Cm3dRegion));

	/* setup region cfg */
	pstRedBoost->eColorRegionFormat = BVDC_Cm3dFormat_ePolar;
	pstRedBoost->ulMaxX = 360;
	pstRedBoost->ulMinX = 186;
	pstRedBoost->ulMaxY = 720;
	pstRedBoost->ulMinY = 0;

	/* setup PWLs */
	pstRedBoost->ulPwlCnt = 4;

	pstRedBoost->astPwl[0].ePwlInput = BVDC_Cm3dPwlInput_eHue;
	pstRedBoost->astPwl[0].astPwlPoint[0].ulX = 170;
	pstRedBoost->astPwl[0].astPwlPoint[0].ilY = 0 << 8;
	pstRedBoost->astPwl[0].astPwlPoint[1].ulX = 206;
	pstRedBoost->astPwl[0].astPwlPoint[1].ilY = 1 << 8;
	pstRedBoost->astPwl[0].astPwlPoint[2].ulX = 240;
	pstRedBoost->astPwl[0].astPwlPoint[2].ilY = 0 << 8;
	pstRedBoost->astPwl[0].astPwlPoint[3].ulX = 260;
	pstRedBoost->astPwl[0].astPwlPoint[3].ilY = 0 << 8;
	pstRedBoost->astPwl[0].astPwlPoint[4].ulX = 296;
	pstRedBoost->astPwl[0].astPwlPoint[4].ilY = -1 << 8;
	pstRedBoost->astPwl[0].astPwlPoint[5].ulX = 330;
	pstRedBoost->astPwl[0].astPwlPoint[5].ilY = 0 << 8;
	pstRedBoost->astPwl[0].astPwlPoint[6].ulX = 350;
	pstRedBoost->astPwl[0].astPwlPoint[6].ilY = 0 << 8;
	pstRedBoost->astPwl[0].astPwlPoint[7].ulX = 361;
	pstRedBoost->astPwl[0].astPwlPoint[7].ilY = 0 << 8;

	pstRedBoost->astPwl[1].ePwlInput = BVDC_Cm3dPwlInput_eSat;
	pstRedBoost->astPwl[1].astPwlPoint[0].ulX = 0;
	pstRedBoost->astPwl[1].astPwlPoint[0].ilY = 1 << 8;
	pstRedBoost->astPwl[1].astPwlPoint[1].ulX = 20;
	pstRedBoost->astPwl[1].astPwlPoint[1].ilY = 1 << 8;
	pstRedBoost->astPwl[1].astPwlPoint[2].ulX = 100;
	pstRedBoost->astPwl[1].astPwlPoint[2].ilY = 0 << 8;
	pstRedBoost->astPwl[1].astPwlPoint[3].ulX = 150;
	pstRedBoost->astPwl[1].astPwlPoint[3].ilY = 0 << 8;
	pstRedBoost->astPwl[1].astPwlPoint[4].ulX = 520;
	pstRedBoost->astPwl[1].astPwlPoint[4].ilY = 0 << 8;
	pstRedBoost->astPwl[1].astPwlPoint[5].ulX = 610;
	pstRedBoost->astPwl[1].astPwlPoint[5].ilY = 0 << 8;
	pstRedBoost->astPwl[1].astPwlPoint[6].ulX = 650;
	pstRedBoost->astPwl[1].astPwlPoint[6].ilY = 0 << 8;
	pstRedBoost->astPwl[1].astPwlPoint[7].ulX = 730;
	pstRedBoost->astPwl[1].astPwlPoint[7].ilY = 0 << 8;

	pstRedBoost->astPwl[2].ePwlInput = BVDC_Cm3dPwlInput_eHue;
	pstRedBoost->astPwl[2].astPwlPoint[0].ulX = 186;
	pstRedBoost->astPwl[2].astPwlPoint[0].ilY = 0 << 8;
	pstRedBoost->astPwl[2].astPwlPoint[1].ulX = 218;
	pstRedBoost->astPwl[2].astPwlPoint[1].ilY = 0 << 8;
	pstRedBoost->astPwl[2].astPwlPoint[2].ulX = 237;
	pstRedBoost->astPwl[2].astPwlPoint[2].ilY = 1 << 8;
	pstRedBoost->astPwl[2].astPwlPoint[3].ulX = 274;
	pstRedBoost->astPwl[2].astPwlPoint[3].ilY = 0 << 8;
	pstRedBoost->astPwl[2].astPwlPoint[4].ulX = 290;
	pstRedBoost->astPwl[2].astPwlPoint[4].ilY = 0 << 8;
	pstRedBoost->astPwl[2].astPwlPoint[5].ulX = 306;
	pstRedBoost->astPwl[2].astPwlPoint[5].ilY = 0 << 8;
	pstRedBoost->astPwl[2].astPwlPoint[6].ulX = 350;
	pstRedBoost->astPwl[2].astPwlPoint[6].ilY = 0 << 8;
	pstRedBoost->astPwl[2].astPwlPoint[7].ulX = 361;
	pstRedBoost->astPwl[2].astPwlPoint[7].ilY = 0 << 8;

	pstRedBoost->astPwl[3].ePwlInput = BVDC_Cm3dPwlInput_eSat;
	pstRedBoost->astPwl[3].astPwlPoint[0].ulX = 0;
	pstRedBoost->astPwl[3].astPwlPoint[0].ilY = 0 << 8;
	pstRedBoost->astPwl[3].astPwlPoint[1].ulX = 200;
	pstRedBoost->astPwl[3].astPwlPoint[1].ilY = 1 << 8;
	pstRedBoost->astPwl[3].astPwlPoint[2].ulX = 360;
	pstRedBoost->astPwl[3].astPwlPoint[2].ilY = 1 << 8;
	pstRedBoost->astPwl[3].astPwlPoint[3].ulX = 480;
	pstRedBoost->astPwl[3].astPwlPoint[3].ilY = 1 << 8;
	pstRedBoost->astPwl[3].astPwlPoint[4].ulX = 520;
	pstRedBoost->astPwl[3].astPwlPoint[4].ilY = 1 << 8;
	pstRedBoost->astPwl[3].astPwlPoint[5].ulX = 550;
	pstRedBoost->astPwl[3].astPwlPoint[5].ilY = 1 << 8;
	pstRedBoost->astPwl[3].astPwlPoint[6].ulX = 650;
	pstRedBoost->astPwl[3].astPwlPoint[6].ilY = 0 << 8;
	pstRedBoost->astPwl[3].astPwlPoint[7].ulX = 730;
	pstRedBoost->astPwl[3].astPwlPoint[7].ilY = 0 << 8;

	/* set up gain adj */
	pstRedBoost->ulGainCnt = 2;

	pstRedBoost->astGain[0].ilPeakGain = 30;
	pstRedBoost->astGain[0].eAlgo = BVDC_Cm3dGainAlgorithm_eProduct;
	pstRedBoost->astGain[0].ulInputCnt = 3;
	pstRedBoost->astGain[0].aeInput[0] = BVDC_Cm3dGainInput_ePWL0;
	pstRedBoost->astGain[0].aeInput[1] = BVDC_Cm3dGainInput_ePWL1;
	pstRedBoost->astGain[0].aeInput[2] = BVDC_Cm3dGainInput_eValueOne;

	pstRedBoost->astGain[1].ilPeakGain = 60;
	pstRedBoost->astGain[1].eAlgo = BVDC_Cm3dGainAlgorithm_eProduct;
	pstRedBoost->astGain[1].ulInputCnt = 3;
	pstRedBoost->astGain[1].aeInput[0] = BVDC_Cm3dGainInput_ePWL2;
	pstRedBoost->astGain[1].aeInput[1] = BVDC_Cm3dGainInput_ePWL3;
	pstRedBoost->astGain[1].aeInput[2] = BVDC_Cm3dGainInput_eValueOne;

	/* set gain output */
	pstRedBoost->eCrOffset = BVDC_Cm3dOutputCfg_eAdjRslt0;
	pstRedBoost->eCbOffset = BVDC_Cm3dOutputCfg_eAdjRslt1;

	pstRedBoost->bEnable = true;

}

void BTST_P_Cm3dSetRegion
	( BVDC_Cm3dRegion *pstRegion,
	  uint32_t         ulRegCnt )
{
	uint32_t i, j, k;

	/* clear struct */
	BKNI_Memset((void *)pstRegion, 0x0, sizeof(BVDC_Cm3dRegion) * ulRegCnt);

	for (i=0; i<ulRegCnt; i++)
	{
		/* setup region cfg */
		pstRegion[i].eColorRegionFormat = astCm3dRegionCfg.auiFormat[i];
/*		printf("Region %d eColorRegionFormat = %d\n", i, pstRegion[i].eColorRegionFormat);*/
		pstRegion[i].ulMaxX = astCm3dRegionCfg.aulMaxX[i];
/*		printf("Region %d ulMaxX = %d\n", i, pstRegion[i].ulMaxX);*/
		pstRegion[i].ulMinX = astCm3dRegionCfg.aulMinX[i];
/*		printf("Region %d ulMinX = %d\n", i, pstRegion[i].ulMinX);*/
		pstRegion[i].ulMaxY = astCm3dRegionCfg.aulMaxY[i];
/*		printf("Region %d ulMaxY = %d\n", i, pstRegion[i].ulMaxY);*/
		pstRegion[i].ulMinY = astCm3dRegionCfg.aulMinY[i];
/*		printf("Region %d ulMinY = %d\n", i, pstRegion[i].ulMinY);*/

		/* setup PWLs */
		pstRegion[i].ulPwlCnt = 8;

		for (j=0; j<pstRegion[i].ulPwlCnt; j++)
		{
			uint32_t ulPwlIndex = (i * BVDC_MAX_CM3D_PWL * 2)+(j*2);
			pstRegion[i].astPwl[j].ePwlInput = astCm3dPwlCfg[ulPwlIndex].ulPwlInput;
/*			printf("Region %d PWL %d ePwlInput = %d\n", i, j, pstRegion[i].astPwl[j].ePwlInput);*/

			for (k=0; k<8; k++)
			{
				pstRegion[i].astPwl[j].astPwlPoint[k].ulX = astCm3dPwlCfg[ulPwlIndex].aulPoints[k];
/*				printf("\tPoint %d X = %d\n", k, pstRegion[i].astPwl[j].astPwlPoint[k].ulX);*/
				pstRegion[i].astPwl[j].astPwlPoint[k].ilY = astCm3dPwlCfg[ulPwlIndex+1].aulPoints[k];
/*				printf("\tPoint %d Y= %d\n", k, pstRegion[i].astPwl[j].astPwlPoint[k].ilY);	*/
			}
		}

		/* set up gain adj */
		pstRegion[i].ulGainCnt = 4;

		for (j=0;j<pstRegion[i].ulGainCnt; j++)
		{
			switch (j)
			{
				case 0:
					pstRegion[i].astGain[j].ilPeakGain = astCm3dRegionCfg.ailGainAdj0_PeakGain[i];
					pstRegion[i].astGain[j].eAlgo = astCm3dRegionCfg.auiGainAdj0_Algo[i];
					pstRegion[i].astGain[j].ulInputCnt = 3;

					for (k=0; k<pstRegion[i].astGain[j].ulInputCnt; k++)
					{
						switch (k)
						{
							case 0:
								pstRegion[i].astGain[j].aeInput[k] = astCm3dRegionCfg.auiGainAdj0_Gain0Input[i];
								break;
							case 1:
								pstRegion[i].astGain[j].aeInput[k] = astCm3dRegionCfg.auiGainAdj0_Gain1Input[i];
								break;
							case 2:
								pstRegion[i].astGain[j].aeInput[k] = astCm3dRegionCfg.auiGainAdj0_Gain2Input[i];
								break;
						}

/*						printf("Region %d Gain %d aeInput %d = %d\n", i, j, k,  pstRegion[i].astGain[j].aeInput[k]);*/

					}
					break;
				case 1:
					pstRegion[i].astGain[j].ilPeakGain = astCm3dRegionCfg.ailGainAdj1_PeakGain[i];
					pstRegion[i].astGain[j].eAlgo = astCm3dRegionCfg.auiGainAdj1_Algo[i];
					pstRegion[i].astGain[j].ulInputCnt = 3;

					for (k=0; k<pstRegion[i].astGain[j].ulInputCnt; k++)
					{
						switch (k)
						{
							case 0:
								pstRegion[i].astGain[j].aeInput[k] = astCm3dRegionCfg.auiGainAdj1_Gain0Input[i];
								break;
							case 1:
								pstRegion[i].astGain[j].aeInput[k] = astCm3dRegionCfg.auiGainAdj1_Gain1Input[i];
								break;
							case 2:
								pstRegion[i].astGain[j].aeInput[k] = astCm3dRegionCfg.auiGainAdj1_Gain2Input[i];
								break;
						}
/*						printf("Region %d Gain %d aeInput %d = %d\n", i, j, k,  pstRegion[i].astGain[j].aeInput[k]);*/
					}
					break;
				case 2:
					pstRegion[i].astGain[j].ilPeakGain = astCm3dRegionCfg.ailGainAdj2_PeakGain[i];
					pstRegion[i].astGain[j].eAlgo = astCm3dRegionCfg.auiGainAdj2_Algo[i];
					pstRegion[i].astGain[j].ulInputCnt = 3;

					for (k=0; k<pstRegion[i].astGain[j].ulInputCnt; k++)
					{
						switch (k)
						{
							case 0:
								pstRegion[i].astGain[j].aeInput[k] = astCm3dRegionCfg.auiGainAdj2_Gain0Input[i];
								break;
							case 1:
								pstRegion[i].astGain[j].aeInput[k] = astCm3dRegionCfg.auiGainAdj2_Gain1Input[i];
								break;
							case 2:
								pstRegion[i].astGain[j].aeInput[k] = astCm3dRegionCfg.auiGainAdj2_Gain2Input[i];
								break;
						}
/*						printf("Region %d Gain %d aeInput %d = %d\n", i, j, k,  pstRegion[i].astGain[j].aeInput[k]);*/
					}
					break;
				case 3:
					pstRegion[i].astGain[j].ilPeakGain = astCm3dRegionCfg.ailGainAdj3_PeakGain[i];
					pstRegion[i].astGain[j].eAlgo = astCm3dRegionCfg.auiGainAdj3_Algo[i];
					pstRegion[i].astGain[j].ulInputCnt = 3;

					for (k=0; k<pstRegion[i].astGain[j].ulInputCnt; k++)
					{
						switch (k)
						{
							case 0:
								pstRegion[i].astGain[j].aeInput[k] = astCm3dRegionCfg.auiGainAdj3_Gain0Input[i];
								break;
							case 1:
								pstRegion[i].astGain[j].aeInput[k] = astCm3dRegionCfg.auiGainAdj3_Gain1Input[i];
								break;
							case 2:
								pstRegion[i].astGain[j].aeInput[k] = astCm3dRegionCfg.auiGainAdj3_Gain2Input[i];
								break;
						}
/*						printf("Region %d Gain %d aeInput %d = %d\n", i, j, k,  pstRegion[i].astGain[j].aeInput[k]);*/
					}
					break;

			}

/*			printf("Region %d Gain %d ilPeakGain = %d\n", i, j, pstRegion[i].astGain[j].ilPeakGain);*/
/*			printf("Region %d Gain %d eAlgo = %d\n", i, j, pstRegion[i].astGain[j].eAlgo);*/

		}

		/* set gain output */
		pstRegion[i].eCrOffset = astCm3dRegionCfg.auiCrOffsetSel[i];
/*		printf("Region %d eCrOffset = %d\n", i, pstRegion[i].eCrOffset);	*/
		pstRegion[i].eCbOffset = astCm3dRegionCfg.auiCbOffsetSel[i];
/*		printf("Region %d eCbOffset = %d\n", i, pstRegion[i].eCbOffset);	*/
		pstRegion[i].eSatGain= astCm3dRegionCfg.auiSatGainSel[i];
/*		printf("Region %d eSatGain = %d\n", i, pstRegion[i].eSatGain);	*/
		pstRegion[i].eLumaGain = astCm3dRegionCfg.auiLumaGainSel[i];
/*		printf("Region %d eLumaGain = %d\n", i, pstRegion[i].eLumaGain);		*/
		pstRegion[i].eLumaOffset = astCm3dRegionCfg.auiLumaOffsetSel[i];
/*		printf("Region %d eLumaOffset = %d\n", i, pstRegion[i].eLumaOffset);		*/
		pstRegion[i].eAlpha = astCm3dRegionCfg.auiAlphaSel[i];
/*		printf("Region %d eAlpha = %d\n", i, pstRegion[i].eAlpha);		*/
		pstRegion[i].eOutputFormat = astCm3dRegionCfg.auiOutputFormat[i];
/*		printf("Region %d eOutputFormat = %d\n", i, pstRegion[i].eOutputFormat);	*/

		pstRegion[i].eColorRegionFormat = astCm3dRegionCfg.auiFormat[i];
/*		printf("Region %d eColorRegionFormat = %d\n", i, pstRegion[i].eColorRegionFormat);		*/
		pstRegion[i].bEnable = astCm3dRegionCfg.auiEnable[i];;
/*		printf("Region %d bEnable = %d\n", i, pstRegion[i].bEnable);		*/
	}

}

void BTST_P_Cm3dGetOverlappedRegion
	( BVDC_Window_Handle hVidWin )
{
	BVDC_Cm3dOverlappedRegion stOvlpRegion;
	uint32_t i, j;

	BVDC_Window_GetCm3dOverlappedRegion(hVidWin, &stOvlpRegion, 0);

	/* display region info */
	printf("Overlapped region info:\n");
	printf("Cb peak gain = %d\n", stOvlpRegion.ilCbPeakGain);
	printf("Cr peak gain = %d\n", stOvlpRegion.ilCrPeakGain);
	printf("# of PWLs = %d\n", stOvlpRegion.ulPwlCnt);
	printf("Enabled = %d\n", stOvlpRegion.bEnable);

	for (i=0; i<stOvlpRegion.ulPwlCnt; i++)
	{
		printf("PWL %d: input: %d\n", i, stOvlpRegion.astPwl[i].ePwlInput);
		for (j=0; j<BVDC_MAX_CM3D_OVERLAPPED_PWL_POINTS; j++)
		{
			printf("Point %d, X = %d\n", j, stOvlpRegion.astPwl[i].astPwlPoint[j].ulX );
			printf("Point %d, Y = %d\n", j, stOvlpRegion.astPwl[i].astPwlPoint[j].ilY >> 8);
		}
	}
}

void BTST_P_Cm3dGetRegion
	( BVDC_Window_Handle hVidWin )
{
	BVDC_Cm3dRegion stRegion;
	uint32_t i, j, k;

	for (i=0; i<BVDC_MAX_CM3D_REGIONS; i++)
	{
		BVDC_Window_GetCm3dRegion(hVidWin, &stRegion, i);

		/* display region info */
		printf("Region %d info:\n", i);

		printf("\tFormat = %d\n", stRegion.eColorRegionFormat);
		printf("\tMax X = %d\n", stRegion.ulMaxX);
		printf("\tMin X = %d\n", stRegion.ulMinX);
		printf("\tMax Y = %d\n", stRegion.ulMaxY);
		printf("\tMin Y = %d\n", stRegion.ulMinY);
		printf("\t# of PWLs = %d\n", stRegion.ulPwlCnt);

		for (j=0; j<stRegion.ulPwlCnt; j++)
		{
			printf("\t\tPWL %d input = %d\n", j, stRegion.astPwl[j].ePwlInput);
			for(k=0; k<BVDC_MAX_CM3D_PWL_POINTS; k++)
			{
				printf("\t\t\tPoint %d X = %d\n", k, stRegion.astPwl[j].astPwlPoint[k].ulX);
				printf("\t\t\tPoint %d Y = %d\n", k, stRegion.astPwl[j].astPwlPoint[k].ilY >> 8);
			}
		}

		printf("\t# of Gains = %d\n", stRegion.ulGainCnt);

		for (j=0; j<stRegion.ulGainCnt; j++)
		{
			printf("\t\tGain %d algo = %d\n", j, stRegion.astGain[j].eAlgo);
			printf("\t\tGain %d peak gain = %d\n", j, stRegion.astGain[j].ilPeakGain);

			printf("\t\tGain %d # of inputs = %d\n", j, stRegion.astGain[j].ulInputCnt);
			for (k=0; k<stRegion.astGain[j].ulInputCnt; k++)
			{
				printf("\t\t\tInput %d = %d\n", k, stRegion.astGain[j].aeInput[k]);
			}
		}

		printf("\tAlpha output = %d\n", stRegion.eAlpha);
		printf("\tCb offset output = %d\n", stRegion.eCbOffset);
		printf("\tCr offset output = %d\n", stRegion.eCrOffset);
		printf("\tLuma Gain output = %d\n", stRegion.eLumaGain);
		printf("\tLuma Offset output = %d\n", stRegion.eLumaOffset);
		printf("\tSat Gain output = %d\n", stRegion.eSatGain);
		printf("\tOutput Format = %d\n", stRegion.eOutputFormat);

		printf("\tEnable = %d\n", stRegion.bEnable);
	}

}

void BTST_P_EnableRegion
	( BVDC_Window_Handle hVidWin,
	  uint32_t           ulRegionId,
	  bool               bEnable,
	  bool               bOverlap )
{
	if (bOverlap)
	{
		BVDC_Cm3dOverlappedRegion stRegion;
		BVDC_Window_GetCm3dOverlappedRegion(hVidWin, &stRegion, ulRegionId);
		stRegion.bEnable = bEnable;
		BVDC_Window_SetCm3dOverlappedRegion(hVidWin, &stRegion, ulRegionId);

	}
	else
	{
		BVDC_Cm3dRegion stRegion;
		BVDC_Window_GetCm3dRegion(hVidWin, &stRegion, ulRegionId);
		stRegion.bEnable = bEnable;
		BVDC_Window_SetCm3dRegion(hVidWin, &stRegion, ulRegionId);
	}
}

void BTST_P_DisableCm3d
	( BVDC_Window_Handle  hVidWin )

{
	uint32_t i;
	BVDC_Cm3dRegion              stRegion;
	BVDC_Cm3dOverlappedRegion    stOvlpRegion;

	for (i=0; i<BVDC_MAX_CM3D_REGIONS; i++)
	{
		stRegion.bEnable = false;
		BVDC_Window_SetCm3dRegion(hVidWin, &stRegion, i);
	}

	for (i=0; i<BVDC_MAX_CM3D_OVERLAPPED_REGIONS; i++)
	{
		stOvlpRegion.bEnable = false;
		BVDC_Window_SetCm3dOverlappedRegion(hVidWin, &stOvlpRegion, i);
	}
}

void BTST_P_Cm3dDemo
	( BTST_P_Context          *pContext,
	  BVDC_Window_Handle  hVidWin )
{
	uint32_t ulEnable;
	BVDC_Window_SplitScreenSettings stSplitScreen;

	if(!hVidWin)
	{
		goto done;
	}

	printf("Enable CM3D demo? Disable(0), Enable(1): ");
	BTST_P_Scanf(pContext, "%u", &ulEnable);

	/* Demo mode */
	BVDC_Window_GetSplitScreenMode(hVidWin, &stSplitScreen);
	stSplitScreen.eCm3d = ulEnable;
	BVDC_Window_SetSplitScreenMode(hVidWin, &stSplitScreen);

done:
	return;

}

void BTST_P_Cm3d
	( BTST_P_Context          *pContext)
{
	uint32_t i, j;
	BVDC_Cm3dOverlappedRegion stBlueStretch;
	BVDC_Cm3dRegion astRegion[8];
	BVDC_Cm3dRegion stRedBoost;
	BVDC_Window_Handle hVidWin         =
		pContext->aWinContext[pContext->iSelectedComp][pContext->iSelectedWin].hWindow;

	printf("Select CM3D test:\n");
	printf("0. Set 8 Regions\n");
	printf("1. Blue stretch\n");
	printf("2. Red boost\n");
	printf("3. Get overlap region\n");
	printf("4. Get region\n");
	printf("5. Enable/disable a region\n");
	printf("6. Turn on demo mode\n");
	BTST_P_Scanf(pContext, "%u", &i);

	switch(i)
	{
		case 0:
			printf("Enable or disable (0=disable/1=enable)? ");
			BTST_P_Scanf(pContext, "%u", &i);
			if (i==1)
			{
				BTST_P_Cm3dSetRegion(astRegion, 8);
				for (j=0; j<8; j++)
				{
					BVDC_Window_SetCm3dRegion(hVidWin, &astRegion[i], i);
				}
			}
			else
			{
				for (j=0; j<8; j++)
				{
					BTST_P_EnableRegion(hVidWin, j, (bool)i, false);
				}
			}
			BTST_P_ApplyChanges(pContext);
			break;
		case 1:
			printf("Enable or disable (0=disable/1=enable)? ");
			BTST_P_Scanf(pContext, "%u", &i);
			if (i==1)
			{
				BTST_P_Cm3dBlueStretch(&stBlueStretch);
				BVDC_Window_SetCm3dOverlappedRegion(hVidWin, &stBlueStretch, 0);
			}
			else
			{
				BTST_P_EnableRegion(hVidWin, 0, (bool)i, true);
			}
			BTST_P_ApplyChanges(pContext);
			break;
		case 2:
			printf("Enable or disable (0=disable/1=enable)? ");
			BTST_P_Scanf(pContext, "%u", &i);
			if (i==1)
			{
				BTST_P_Cm3dRedBoost(&stRedBoost);
				BVDC_Window_SetCm3dRegion(hVidWin, &stRedBoost, 0);
			}
			else
			{
				for (j=0; j<3; j++)
				{
					BTST_P_EnableRegion(hVidWin, j, (bool)i, false);
				}
			}
			BTST_P_ApplyChanges(pContext);
			break;
		case 3:
			BTST_P_Cm3dGetOverlappedRegion(hVidWin);
			break;
		case 4:
			BTST_P_Cm3dGetRegion(hVidWin);
			break;
		case 5:
			{
			uint32_t ulRegionId;

			printf("Enable or disable a region (0=disable/1=enable)?");
			BTST_P_Scanf(pContext, "%u", &i);
			printf("Overlapped region? (0=non-overlapped/1=overlapped)");
			BTST_P_Scanf(pContext, "%u", &j);
			printf("Enter region id (0 to 15): ");
			BTST_P_Scanf(pContext, "%u", &ulRegionId);
			BTST_P_EnableRegion(hVidWin, ulRegionId, (bool)i, (bool)j);
			}
			BTST_P_ApplyChanges(pContext);
			break;
		case 6:
			BTST_P_Cm3dDemo(pContext, hVidWin);
			BTST_P_ApplyChanges(pContext);
			break;
	}

}

/* end of file */
