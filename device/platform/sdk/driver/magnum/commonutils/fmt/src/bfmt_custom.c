/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bfmt_custom.c $
 * $brcm_Revision: Hydra_Software_Devel/19 $
 * $brcm_Date: 6/9/11 10:42a $
 *
 * Module Description:
 *   Custom Video format info file; it contains raster size, front porch,
 * back porch etc format info, and DVO microcode as well as rate manager
 * settings.
 *  NOTE: This file is to be replace by customer specific timing data!
 *  following are for reference board only.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/fmt/bfmt_custom.c $
 * 
 * Hydra_Software_Devel/19   6/9/11 10:42a pntruong
 * SW7425-101: Moved rate structure definition to bfmt_custom.h to avoid
 * copying.
 * 
 * Hydra_Software_Devel/19   6/9/11 10:41a pntruong
 * SW7425-101: Moved rate structure definition to bfmt_custom.h to avoid
 * copying.
 *
 * Hydra_Software_Devel/18   9/7/10 3:03p yuxiaz
 * SW7422-28: Add 3D formats to FMT plus additional data structures
 *
 * Hydra_Software_Devel/SW7422-28/1   9/2/10 4:09p yuxiaz
 * SW7422-28: Add 3D formats to FMT plus additional data structures
 *
 * Hydra_Software_Devel/17   10/16/09 5:22p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 *
 * Hydra_Software_Devel/16   10/15/09 4:58p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 *
 * Hydra_Software_Devel/15   1/15/09 6:17p hongtaoz
 * PR50621: repositioned trigger locations to as early as possible for DTV
 * display timing to have enough head room for the longest RULs;
 * corrected ulTopActive values for the custom formats to reflect the
 * microcode timing;
 *
 * Hydra_Software_Devel/14   9/20/08 11:25a pntruong
 * PR47115: Added some note for ease of adding new format.
 *
 * Hydra_Software_Devel/13   6/24/08 10:04p pntruong
 * PR43819: Fixed build warnings.
 *
 * Hydra_Software_Devel/12   6/24/08 5:07p pntruong
 * PR43819: Temporary support of runtime in app.
 *
 * Hydra_Software_Devel/11   9/19/07 6:06p pntruong
 * PR34692: Used friendly macro values for specify reference panel
 * support.
 *
 * Hydra_Software_Devel/10   9/19/07 5:56p pntruong
 * PR34692: Support 3552 reference panel timing in portingInterface.
 *
 * Hydra_Software_Devel/9   8/16/07 6:22p tdo
 * PR34007: [VDEC] Add support for 50Hz formats
 * (576p/720p/1080i/1080p/50Hz).
 *
 * Hydra_Software_Devel/8   6/25/07 5:19p tdo
 * PR32193: update RM settings for 3563C0
 *
 * Hydra_Software_Devel/7   6/21/07 1:42p pntruong
 * PR32193: Backout new rrm changes, 59.94hz does not work.
 *
 * Hydra_Software_Devel/6   6/20/07 4:38p tdo
 * PR32193: Update RM settings for custom formats
 *
 * Hydra_Software_Devel/5   1/19/07 7:59p hongtaoz
 * PR27139, PR27222: updated dvo pll settings for 1080p@59.94;
 *
 * Hydra_Software_Devel/4   1/16/07 4:20p hongtaoz
 * PR23260, PR23280, PR23204: initial bringup 59.94 and 50Hz 1080p panel
 * custom format support;
 *
 * Hydra_Software_Devel/3   1/15/07 1:20p hongtaoz
 * PR23280, PR23196: increased 1080p60 microcode's vertical back porch to
 * 36 to leave more vertical blanking to long RUL;
 *
 * Hydra_Software_Devel/2   1/11/07 6:58p hongtaoz
 * PR23188, PR23196: initial bringup of 3563 DVO; 1080p custom format as
 * default;
 *
 * Hydra_Software_Devel/1   12/18/06 11:40p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   10/4/06 6:58p hongtaoz
 * PR23204, PR23279, PR23280: add user-defined custom formats;
 *
 ***************************************************************************/
#include "bstd.h"
#include "bfmt.h"
#include "bkni.h"
#include "bfmt_custom.h"

/* Support different format timings.  Application can either replace this
 * file with its bfmt_custom.c for specific panel's timing.  The two timings
 * below are for Reference Software / Board / Panel.
 *
 * Since we have two Reference Panels
 *   (1) Full 1080p HD Panel
 *   (2) WXGA Panel.
 *
 * Both timings are included here.  To compile with with WXGA simply pass the
 * compile flag -DBFMT_CUSTOM_PANEL_TIMING=1.  If using non-reference panel
 * there is no need to pass the above compile flag. */
#define BFMT_P_CUSTOM_PANEL_TIMING_FULL_1080P_HD (3563)
#define BFMT_P_CUSTOM_PANEL_TIMING_FULL_WXGA     (3552)

#ifndef BFMT_CUSTOM_PANEL_TIMING
#define BFMT_CUSTOM_PANEL_TIMING      (BFMT_P_CUSTOM_PANEL_TIMING_FULL_1080P_HD)
#endif

static const uint32_t s_vec_tb_noprim_dvim1920x1080p_60hz_bvb_input_bss_wxga[] =
{
	 0x0064A001, /*  64 */
	 0x00650009, /*  65 */
	 0x00656001, /*  66 */
	 0x0065C00F, /*  67 */
	 0x00668300, /*  68 */
	 0x0065C007, /*  69 */
	 0x00662001, /*  70 */
	 0x00840000, /*  71 */
	 0x00000000, /*  72 */
	 0x00000000, /*  73 */
	 0x00284038, /*  74 */
	 0x002D400C, /*  75 */
	 0x0024406A, /*  76 */
	 0x0034554B, /*  77 */
	 0x00000000, /*  78 */
	 0x00000000, /*  79 */
	 0x00244038, /*  80 */
	 0x0025400C, /*  81 */
	 0x0024406A, /*  82 */
	 0x0034554B, /*  83 */
	 0x00000000, /*  84 */
	 0x00000000, /*  85 */
	 0x00244038, /*  86 */
	 0x0021400C, /*  87 */
	 0x0020406A, /*  88 */
	 0x0030554B, /*  89 */
	 0x00000000, /*  90 */
	 0x00000000, /*  91 */
	 0x00204038, /*  92 */
	 0x0021400C, /*  93 */
	 0x0020406A, /*  94 */
	 0x0030554B, /*  95 */
	 0x00000000, /*  96 */
	 0x00000000, /*  97 */
	 0x00204038, /*  98 */
	 0x0021400C, /*  99 */
	 0x0020406A, /* 100 */
	 0x00305540, /* 101 */
	 0x00000000, /* 102 */
	 0x00000000, /* 103 */
	 0x00202038, /* 104 */
	 0x0021200C, /* 105 */
	 0x0020206A, /* 106 */
	 0x0032354B, /* 107 */
	 0x00000000, /* 108 */
	 0x00000000, /* 109 */
	 0x00202038, /* 110 */
	 0x0021200C, /* 111 */
	 0x0020206A, /* 112 */
	 0x00323540, /* 113 */
	 0x00000000, /* 114 */
	 0x00000000, /* 115 */
	 0x00000000, /* 116 */
	 0x00000000, /* 117 */
	 0x00000000, /* 118 */
	 0x00000000, /* 119 */
	 0x00000000, /* 120 */
	 0x00000000, /* 121 */
	 0x00000000, /* 122 */
	 0x00000000, /* 123 */
	 0x00000000, /* 124 */
	 0x00000000, /* 125 */
	 0x00072507, /* 126 */
	 0x005D4D0E, /* 127 */
};

static const uint32_t s_vec_tb_noprim_dvim1920x1080p_60hz_bvb_input_bss_fhd[] =
{
	0x0064A001, /*  64 */
	0x00650004, /*  65 */
	0x00656001, /*  66 */
	0x0065C023, /*  67 */
	0x00668438, /*  68 */
	0x0065C003, /*  69 */
	0x00662001, /*  70 */
	0x00840000, /*  71 */
	0x00000000, /*  72 */
	0x00000000, /*  73 */
	0x002840C8, /*  74 */
	0x002D402C, /*  75 */
	0x00244024, /*  76 */
	0x00345775, /*  77 */
	0x00000000, /*  78 */
	0x00000000, /*  79 */
	0x002440C8, /*  80 */
	0x0025402C, /*  81 */
	0x00244024, /*  82 */
	0x00345775, /*  83 */
	0x00000000, /*  84 */
	0x00000000, /*  85 */
	0x002440C8, /*  86 */
	0x0021402C, /*  87 */
	0x00204024, /*  88 */
	0x00305775, /*  89 */
	0x00000000, /*  90 */
	0x00000000, /*  91 */
	0x002040C8, /*  92 */
	0x0021402C, /*  93 */
	0x00204024, /*  94 */
	0x00305775, /*  95 */
	0x00000000, /*  96 */
	0x00000000, /*  97 */
	0x002040C8, /*  98 */
	0x0021402C, /*  99 */
	0x00204024, /* 100 */
	0x0030576A, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x002020C8, /* 104 */
	0x0021202C, /* 105 */
	0x00202024, /* 106 */
	0x00323775, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x002020C8, /* 110 */
	0x0021202C, /* 111 */
	0x00202024, /* 112 */
	0x0032376A, /* 113 */
	0x00000000, /* 114 */
	0x00000000, /* 115 */
	0x00000000, /* 116 */
	0x00000000, /* 117 */
	0x00000000, /* 118 */
	0x00000000, /* 119 */
	0x00000000, /* 120 */
	0x00000000, /* 121 */
	0x00000000, /* 122 */
	0x00000000, /* 123 */
	0x00000000, /* 124 */
	0x00000000, /* 125 */
	0x00011207, /* 126 */
	0x005D605D, /* 127 */
};

/* The following default settings are from 3563C0 1366x768 DVO bringup:
' Pixel Clock = h_total * v_total * refresh_rate
'             = 1540 * 802 * 60
'             = 74104800 Hz
' VCO = 2074934400.00 Hz
BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_FEEDBACK_PRE_DIVIDER = 1 ' (p2)
BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_INPUT_PRE_DIVIDER = 2    ' (p1)
BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_2.ndiv_mode = 1
BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_1 = &h382C24A0&
BCM3563.DVPO_0.FIFO_CTL.MASTER_OR_SLAVE_N = 1
 */
static const BFMT_P_RateInfo s_stDvoRmTbl0_wxga =
	{BFMT_PXL_148_5MHz,           2, 0x13365CEF,  99, 2,  11327,    19649,  1,  1,  1, "74.10"}; /* 74.10 MHz */

/* The following default settings are from 3563C0 1366x768 DVO bringup:
' Pixel Clock = h_total * v_total * refresh_rate
'             = 1540 * 802 * 60 * 1000 / 1001
'             = 74030769 Hz
' VCO = 2072861532.00 Hz
BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_INPUT_PRE_DIVIDER = 2    ' (p1)
BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_2.ndiv_mode = 1
BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_1 = &h382C24A0&
BCM3563.DVPO_0.FIFO_CTL.MASTER_OR_SLAVE_N = 1
 */

static const BFMT_P_RateInfo s_stDvoRmTbl1_wxga =
	{BFMT_PXL_148_5MHz_DIV_1_001,           2, 0x13317316,  112, 2,  368,    401,  1,  1,  1, "74.03"}; /* 74.03 MHz */

/* The following default settings are from 3563C0 1366x768 DVO bringup:
' 1366x768p @ 60Hz: 1540*802*60
' Pixel Clock = h_total * v_total * refresh_rate
'             = 1540 * 802 * 50
'             = 61754000 Hz
BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_FEEDBACK_PRE_DIVIDER = 1 ' (p2)
BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_INPUT_PRE_DIVIDER = 2    ' (p1)
BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_2.ndiv_mode = 1
BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_1 = &h382C24A0&
BCM3563.DVPO_0.FIFO_CTL.MASTER_OR_SLAVE_N = 1
 */
static const BFMT_P_RateInfo s_stDvoRmTbl2_wxga =
	{BFMT_PXL_148_5MHz_DIV_1_001,           2, 0x1002A2C7,  99, 2,  1803,    19649,  1,  1,  1, "61.75"}; /* 61.75 MHz */

/* The following default settings are from 3563C0 1080p DVO bringup:
' 1920x1080p @ 60Hz: 2200*1125*60
' Pixel Clock = 148500000 Hz
' VCO = 2079000000 Hz
' M = 1, N = 77, P1 = 2; P2 = 1
' For RDIV=112, Rate Manager sees 9281250 Hz
' Sample_Inc = 2, NUM/DEN = 10/11
' BCM3563 Register Programming:
  BCM3563.DVPO_RM_0.RATE_RATIO.DENOMINATOR = 11
  BCM3563.DVPO_RM_0.SAMPLE_INC.NUMERATOR = 10
  BCM3563.DVPO_RM_0.SAMPLE_INC.SAMPLE_INC = 2
  BCM3563.DVPO_RM_0.OFFSET = &h13400000&
  BCM3563.DVPO_RM_0.FORMAT.SHIFT = 3
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.LINKDIV_CTRL = 0
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_R_DIV = 112
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_VCO_RANGE = 1
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_M_DIV = 1
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_FEEDBACK_PRE_DIVIDER = 1
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_INPUT_PRE_DIVIDER = 2
  BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_2.ndiv_mode = 1
  BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_1 = &h382C24A0&
  BCM3563.DVPO_0.FIFO_CTL.MASTER_OR_SLAVE_N = 1
 */
static const BFMT_P_RateInfo s_stDvoRmTbl0_fhd =
	{BFMT_PXL_148_5MHz,           1, 0x13400000,  112, 2,  10,    11,  1,  0,  1, "148.5"}; /* 148.50000 MHz */

/* The following default settings are from 3563C0 1080p DVO bringup:
' 1920x1080p @ 59.94Hz: 2200*1125*59.94
' Pixel Clock = 148351648.351648 Hz
' VCO = 2076923076.92308 Hz
' M = 1, N = 76.9230769230769, P1 = 2; P2 = 1
' For RDIV=112, Rate Manager sees 9271978.02197802 Hz
' Sample_Inc = 2, NUM/DEN = 114/125
' BCM3563 Register Programming:
  BCM3563.DVPO_RM_0.RATE_RATIO.DENOMINATOR = 125
  BCM3563.DVPO_RM_0.SAMPLE_INC.NUMERATOR = 114
  BCM3563.DVPO_RM_0.SAMPLE_INC.SAMPLE_INC = 2
  BCM3563.DVPO_RM_0.OFFSET = &H133b13b1&
  BCM3563.DVPO_RM_0.FORMAT.SHIFT = 3
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.LINKDIV_CTRL = 0
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_R_DIV = 112
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_VCO_RANGE = 1
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_M_DIV = 1
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_FEEDBACK_PRE_DIVIDER = 1 ' (p2)
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_INPUT_PRE_DIVIDER = 2    ' (p1)
  BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_2.ndiv_mode = 1
  BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_1 = &h382C24A0&
  BCM3563.DVPO_0.FIFO_CTL.MASTER_OR_SLAVE_N = 1
 */
static const BFMT_P_RateInfo s_stDvoRmTbl1_fhd =
	{BFMT_PXL_148_5MHz_DIV_1_001, 1, 0x133B13B1, 112, 2, 114,   125,  1,  0,  1, "148.3"}; /* 148.35 MHz */

/* The following default settings are from 3563C0 1080p DVO bringup:
' 1920x1080p @ 50Hz: 2200*1125*50
' Pixel Clock = 123750000 Hz
' VCO = 1732500000 Hz
' M = 1, N = 64.1666666666667, P1 = 2; P2 = 1
' For RDIV=112, Rate Manager sees 7734375 Hz
' Sample_Inc = 3, NUM/DEN = 27/55
' BCM3563 Register Programming:
  BCM3563.DVPO_RM_0.RATE_RATIO.DENOMINATOR = 55
  BCM3563.DVPO_RM_0.SAMPLE_INC.NUMERATOR = 27
  BCM3563.DVPO_RM_0.SAMPLE_INC.SAMPLE_INC = 3
  BCM3563.DVPO_RM_0.OFFSET = &H100aaaaa&
  BCM3563.DVPO_RM_0.FORMAT.SHIFT = 3
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.LINKDIV_CTRL = 0
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_R_DIV = 112
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_VCO_RANGE = 1
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_M_DIV = 1
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_FEEDBACK_PRE_DIVIDER = 1 ' (p2)
  BCM3563.LVDS_PHY_0.LVDS_PLL_CFG.PLL_INPUT_PRE_DIVIDER = 2    ' (p1)
  BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_2.ndiv_mode = 1
  BCM3563.LVDS_PHY_0.LVDS_PLL_CTL_1 = &h382C24A0&
  BCM3563.DVPO_0.FIFO_CTL.MASTER_OR_SLAVE_N = 1
 */
static const BFMT_P_RateInfo s_stDvoRmTbl2_fhd =
	{BFMT_PXL_148_5MHz,           1, 0x100aaaaa, 112, 3,  27,    55,  1,  0,  1, "123.7"}; /* 123.75 MHz */

/*
    DTRAM values for Custom 1366x768p

    h_total = h_valid + hsync_width + h_frontporch + h_backporch
            = 1366 + 12 + 56 + 106
            = 1540

    v_total = v_valid + vsync_width + v_frontporch + v_backporch
            = 768 + 10 + 8 + 16
            = 802

    ulTopActive = vsync_width + v_backporch + 1
            = 10 + 16 + 1
            = 27

    Pixel clock frequency in Hz:
    pixel_clk_Hz = h_total * v_total * 60 * 1000 / 1001
                 = 1540 * 802 * 60 * 1000 / 1001
                 = 74030769 Hz for 59.94Hz refresh

    line_time    = 1001/(60*802*1000)
                 = 20.802 us

    pixel_clk_Hz = h_total * v_total * 60
                 = 1540 * 802 * 60
                 = 74104800 Hz for 60Hz refresh

    line_time    = 1/(60*802)
                 = 20.781 us
*/

static BFMT_CustomFormatInfo s_stCustomInfo0_wxga =
{
	(void*)s_vec_tb_noprim_dvim1920x1080p_60hz_bvb_input_bss_wxga,
	(void*)&s_stDvoRmTbl0_wxga,
	(void*)&s_stDvoRmTbl1_wxga
};

static BFMT_CustomFormatInfo s_stCustomInfo1_wxga =
{
	(void*)s_vec_tb_noprim_dvim1920x1080p_60hz_bvb_input_bss_wxga,
	(void*)&s_stDvoRmTbl2_wxga,
	(void*)&s_stDvoRmTbl2_wxga
};

static BFMT_CustomFormatInfo s_stCustomInfo0_fhd =
{
	(void*)s_vec_tb_noprim_dvim1920x1080p_60hz_bvb_input_bss_fhd,
	(void*)&s_stDvoRmTbl0_fhd,
	(void*)&s_stDvoRmTbl1_fhd
};

static BFMT_CustomFormatInfo s_stCustomInfo1_fhd =
{
	(void*)s_vec_tb_noprim_dvim1920x1080p_60hz_bvb_input_bss_fhd,
	(void*)&s_stDvoRmTbl2_fhd,
	(void*)&s_stDvoRmTbl2_fhd
};

/* Default custom formats: 1366x768p 60Hz and 50Hz; */
/* 59.94/60 Hz */
static const BFMT_VideoInfo s_stFormatInfoCustom0_wxga =
{
	BFMT_VideoFmt_eCustom0,
	1366,
	768,
	1366,
	768,
	1540,
	802,
	27,
	0,
	0,
	0,
	BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
	6000,
	BFMT_PXL_148_5MHz_DIV_1_001 | BFMT_PXL_148_5MHz,
	false,
	BFMT_AspectRatio_e16_9,
	BFMT_Orientation_e2D,
	7403,
	"BFMT_VideoFmt_eCustom0",
	&s_stCustomInfo0_wxga
};

/* 50 Hz */
static const BFMT_VideoInfo s_stFormatInfoCustom1_wxga =
{
	BFMT_VideoFmt_eCustom1,
	1366,
	768,
	1366,
	768,
	1540,
	802,
	27,
	0,
	0,
	0,
	BFMT_VERT_50Hz,
	5000,
	BFMT_PXL_148_5MHz,
	false,
	BFMT_AspectRatio_e16_9,
	BFMT_Orientation_e2D,
	7410,
	"BFMT_VideoFmt_eCustom1",
	&s_stCustomInfo1_wxga
};

/*
    DTRAM values for Custom 1920x1080p

    h_total = h_valid + hsync_width + h_backporch + h_frontporch
            = 1920 + 44 + 200 + 36
            = 2200

    v_total = v_valid + vsync_width + v_backporch + v_frontporch
            = 1080 + 5 + 36 + 4
            = 1125

    ulTopActive = vsync_width + v_backporch + 1
            = 5 + 36 + 1
            = 42

    Pixel clock frequency in Hz:
    pixel_clk_Hz = h_total * v_total * 60 * 1000 / 1001
                 = 2200 * 1125 * 60 * 1000 / 1001
                 = 148351648 Hz for 59.94Hz refresh

    line_time    = 1001/(60*1125*1000)
                 = 14.830 us

    pixel_clk_Hz = h_total * v_total * 60
                 = 2200 * 1125 * 60
                 = 148500000 Hz for 60Hz refresh

    line_time    = 1/(60*1125)
                 = 14.815 us
*/

/* Default custom formats: 1366x768p 60Hz and 50Hz; */
/* 59.94/60 Hz */
static const BFMT_VideoInfo s_stFormatInfoCustom0_fhd =
{
	BFMT_VideoFmt_eCustom0,
	1920,
	1080,
	1920,
	1080,
	2200,
	1125,
	42,
	0,
	0,
	0,
	BFMT_VERT_59_94Hz | BFMT_VERT_60Hz,
	6000,
	BFMT_PXL_148_5MHz_DIV_1_001 | BFMT_PXL_148_5MHz,
	false,
	BFMT_AspectRatio_e16_9,
	BFMT_Orientation_e2D,
	14850,
	"BFMT_VideoFmt_eCustom0",
	&s_stCustomInfo0_fhd
};

/* 50 Hz */
static const BFMT_VideoInfo s_stFormatInfoCustom1_fhd =
{
	BFMT_VideoFmt_eCustom1,
	1920,
	1080,
	1920,
	1080,
	2200,
	1125,
	42,
	0,
	0,
	0,
	BFMT_VERT_50Hz,
	5000,
	BFMT_PXL_148_5MHz,
	false,
	BFMT_AspectRatio_e16_9,
	BFMT_Orientation_e2D,
	12375,
	"BFMT_VideoFmt_eCustom1",
	&s_stCustomInfo1_fhd
};

#if (BFMT_CUSTOM_PANEL_TIMING == BFMT_P_CUSTOM_PANEL_TIMING_FULL_WXGA)
#define s_stFormatInfoCustom0 s_stFormatInfoCustom0_wxga
#define s_stFormatInfoCustom1 s_stFormatInfoCustom1_wxga
#else
#define s_stFormatInfoCustom0 s_stFormatInfoCustom0_fhd
#define s_stFormatInfoCustom1 s_stFormatInfoCustom1_fhd
#endif


/* WARNINGS: below code are for internal use only! */
/*
 * This is a one-way runtime switch to go from WXGA to FHD.
 * Apps can call this function before BVDC_Open if they want this feature.
 * BFMT_P_SetFhd() is not part of the public API and is subject to change or
 * removal. Add an extern to your own code to use it.
*/
void BFMT_P_SetFhd
	( void )
{
#if (BFMT_CUSTOM_PANEL_TIMING == BFMT_P_CUSTOM_PANEL_TIMING_FULL_WXGA)
    BKNI_Memcpy((void*)&s_stFormatInfoCustom0, &s_stFormatInfoCustom0_fhd, sizeof(s_stFormatInfoCustom0));
    BKNI_Memcpy((void*)&s_stFormatInfoCustom1, &s_stFormatInfoCustom1_fhd, sizeof(s_stFormatInfoCustom1));
#else
    /* BDBG_ERR(("You must compile with BFMT_P_CUSTOM_PANEL_TIMING_FULL_WXGA to
     * have runtime switch from WXGA to FHD")); */
#endif
	return;
}

/* End of File */

