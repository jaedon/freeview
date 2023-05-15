/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_macrovisiontable_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/99 $
 * $brcm_Date: 8/10/12 3:27p $
 *
 * Module Description:
 *   Contains tables for Macrovision settings.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/modularvec/bvdc_macrovisiontable_priv.c $
 * 
 * Hydra_Software_Devel/99   8/10/12 3:27p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/98   5/21/12 4:17p darnstein
 * SW7552-270: Integrate VEC IT microcode for Macrovision TEST02, PAL and
 * PAL-N.
 *
 * Hydra_Software_Devel/97   7/27/11 1:53p tdo
 * SW7405-5425: MACROVISION_SUPPORT=y causes flicker on PAL-M
 *
 * Hydra_Software_Devel/96   2/7/11 2:04p darnstein
 * SW7405-5081: Implement ARIB option for NTSC.
 *
 * Hydra_Software_Devel/95   10/14/10 7:19p darnstein
 * SW7340-193: integrate many VEC IT microcode arrays.
 *
 * Hydra_Software_Devel/94   6/28/10 2:27p darnstein
 * SW7335-696: Work-around for 7335 video shift issue.
 *
 * Hydra_Software_Devel/93   6/24/10 3:46p darnstein
 * SW7325-389: New microcode for SECAM/WSE fix needs new microcontroller
 * start addresses.
 *
 * Hydra_Software_Devel/92   6/23/10 4:26p darnstein
 * SW7325-389: extend software work-around for WSE problem to SECAM video
 * output.
 *
 * Hydra_Software_Devel/91   1/21/10 7:25p darnstein
 * SW7325-389: change symbol from BVBI_P_WSE_VER3 to BVDC_P_WSE_VER3.
 *
 * Hydra_Software_Devel/90   1/13/10 4:26p darnstein
 * SW7335-636,SW7325-675: integrate PAL microcode and VEC IT start
 * addresses from Nutan. This applies only to 7325-B0 and 7335. #ifdefs
 * are used for this.
 *
 * Hydra_Software_Devel/89   11/13/09 1:06p darnstein
 * SW7400-2607,SW7405-3132: New analog and digital VEC IT microcode for
 * PAL-M.
 *
 * Hydra_Software_Devel/88   10/22/09 5:12p darnstein
 * SW7405-3132,SW7405-3133,HW7413-238: I missed one merge point in the
 * previous check-in.
 *
 * Hydra_Software_Devel/87   10/22/09 5:04p darnstein
 * SW7405-3132,SW7405-3133,HW7413-238: fix breakage introduced by previous
 * check-in.
 *
 * Hydra_Software_Devel/86   10/22/09 4:35p darnstein
 * SW7405-3132,SW7405-3133,HW7413-238: Updates for PAL-M, PAL-N, and
 * Macrovision. Fix small error in NTSC/Macrovision_TEST0?. This check-in
 * breaks the build, but is needed for record keeping. Next check-in will
 * fix the build.
 *
 * Hydra_Software_Devel/85   10/13/09 5:48p darnstein
 * SW7405-3004: install 480P ARIB output via BVDC display settings.
 *
 * Hydra_Software_Devel/84   10/7/09 3:32p darnstein
 * SW7405-3133: Put a NO-OP instruction at end of VEC IT microcode arrays
 * for NMacrovision TEST01 and TEST01. This is a temporary measure. The
 * microcode experts in Bangalore should release official arrays for
 * integration.
 *
 * Hydra_Software_Devel/83   10/7/09 1:11p darnstein
 * HW7413-238: Integrate new analog VEC IT microcode arrays for PAL-N. All
 * forms of Macrovision for PAL-N are thus updated.
 *
 * Hydra_Software_Devel/82   6/4/09 12:08p darnstein
 * PR44592,PR54152,PR55140: update analog and digital VEC IT microcode for
 * PAL-G and similar formats. Update corresponding microcontroller start
 * addresses too.
 *
 * Hydra_Software_Devel/81   6/2/09 3:21p darnstein
 * PR48884: More resistance to invalid user inputs.
 *
 * Hydra_Software_Devel/80   4/2/09 6:13p darnstein
 * PR51325: Workaround for WSE bug: toggle VF_MISC.VBI_ENABLE every video
 * field.
 *
 * Hydra_Software_Devel/79   12/17/08 6:20p darnstein
 * PR50075: For PAL Macrovision TYPE1, TYPE2, TYPE3: enable pseudosync
 * pulses via PCL_0.NEGSYNC_AND_TERM_3=LINE_D, NOT
 * PCL_2.SEC_NEG_SYNC_ENABLE. Per Nutan Raj.
 *
 * Hydra_Software_Devel/78   12/8/08 12:20p darnstein
 * PR50075: For all PAL Macrovision, use SEC_NEG_SYNC_ENABLE = ENABLED,
 * from observations by Lin Ma and Bo Zhang.
 *
 * Hydra_Software_Devel/77   11/10/08 4:40p darnstein
 * PR46484: fix gross copy-and-paste error in diagnostic routines.
 *
 * Hydra_Software_Devel/76   11/7/08 5:41p darnstein
 * PR46484: In new register table dump facility, print out one more value.
 *
 * Hydra_Software_Devel/75   11/7/08 5:34p darnstein
 * PR46484: Add functions to print out contents of some important register
 * value tables. Must be activated with #define BVDC_P_DISPLAY_DUMP.
 *
 * Hydra_Software_Devel/74   11/7/08 11:04a darnstein
 * PR46484: fix errors in new macro usage, more.
 *
 * Hydra_Software_Devel/73   11/6/08 7:50p darnstein
 * PR46484: fix errors in new macro usage, more.
 *
 * Hydra_Software_Devel/72   11/6/08 5:16p darnstein
 * PR46484: fix errors in new macro usage, more.
 *
 * Hydra_Software_Devel/71   11/5/08 4:21p darnstein
 * PR46484: fix errors in new macro usage.
 *
 * Hydra_Software_Devel/70   10/28/08 7:42p darnstein
 * PR46847,PR48207: New PAL Macrovision type1, type2, type3, test01, and
 * test02.
 *
 * Hydra_Software_Devel/69   10/23/08 10:12p tdo
 * PR46484: Add back some comments
 *
 * Hydra_Software_Devel/68   10/23/08 9:40p tdo
 * PR46484:Initial VDC bringup for 7420
 *
 * Hydra_Software_Devel/67   8/22/08 6:09p darnstein
 * PR45745: new VEC IT microcode for Macrovision TEST01 and TEST02.
 *
 * Hydra_Software_Devel/66   8/8/08 2:24p darnstein
 * PR44592: Complete integration of new VEC IT microcode.
 *
 * Hydra_Software_Devel/65   7/10/08 8:15p darnstein
 * PR44592: partial completion of microcode update.
 * PAL / Macrovision Type1, Type2, and Type3.
 *
 * Hydra_Software_Devel/64   5/2/08 3:04p rpan
 * PR42109: More work for supporting SECAM variants.
 *
 * Hydra_Software_Devel/63   5/1/08 2:38p rpan
 * PR42109: Initial work for SECAM variants.
 *
 * Hydra_Software_Devel/62   3/10/08 9:14p darnstein
 * PR25971: first try at DCS for 7400
 *
 * Hydra_Software_Devel/61   2/22/08 5:12p darnstein
 * PR39783: Use VEC timing microcode for SECAM that includes Macrovision
 * algorithms.
 *
 * Hydra_Software_Devel/60   2/21/08 3:42p darnstein
 * PR39783: Improve selection logic for Macrovision features (mostly
 * cosmetic). Fix starting addresses for VEC microcode for Macrovision /
 * PAL_M.
 *
 * Hydra_Software_Devel/59   10/30/07 7:21p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/58   10/10/07 5:19p darnstein
 * PR35195: For PAL test01 and test02 Macrovision, restore the colorstripe
 * waveform. Although there is no CS waveform for type[012], there is CS
 * waveform for test0[12].
 *
 * Hydra_Software_Devel/57   10/5/07 5:45p darnstein
 * PR35195: these are the same changes I made on the dedicated 7401
 * branch. Three new Macrovision types for PAL.
 *
 * Hydra_Software_Devel/56   9/21/07 2:15p darnstein
 * PR35195: the NTSC and NTSC_704 work is ready for test.
 *
 * Hydra_Software_Devel/55   7/5/07 6:20p hongtaoz
 * PR26039: rolled back NTSC Macrovision test01/02 microcodes;
 *
 * Hydra_Software_Devel/54   1/2/07 1:45p maivu
 * PR 26051: Added 7400B0 support
 *
 * Hydra_Software_Devel/53   12/18/06 11:31p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/5   12/14/06 4:26p pntruong
 * PR26039:  Removed build warnings.
 *
 * Hydra_Software_Devel/52   12/14/06 4:16p pntruong
 * PR26039: Removed build warnings.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/4   12/13/06 5:22p hongtaoz
 * PR26039: updated 480i/720p/1080i analog microcodes for the new shaper
 * settings;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   10/27/06 1:09p hongtaoz
 * PR25279: merge PAL01/02 microcodes from main branch;
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   9/29/06 2:18p tdo
 * PR 24608:  Add 240p mode support
 *
 * Hydra_Software_Devel/47   8/29/06 10:58a hongtaoz
 * PR23032, PR23246: updated microcode for NTSC to support Gemstar; add
 * new QDAC support for 3563; centralize some chip-specific display
 * macroes into bvdc_common_priv.h;
 *
 * Hydra_Software_Devel/46   7/11/06 7:01p hongtaoz
 * PR22062: fixed PAL_M tg_config setting; updated PAL_N microcode; PAL_NC
 * both u/v burst amplitude have to be negative;
 *
 * Hydra_Software_Devel/45   6/30/06 5:46p hongtaoz
 * PR22081, PR20395, PR22062, PR22246: added SECAM Macrovision support;
 * fixed PAL_M dvi microcode; fixed PAL_M sub-carrier frequency setting;
 * PAL_NC should use the same microcode as normal PAL; fixed SECAM and
 * various PAL's csc matrices according to BT.470-6 spec; changed SRC
 * filter setting to beneifit higher freq response for HD output on
 * PRIM_VEC; fixed PAL sync tip setting for non-Macrovision build;
 *
 * Hydra_Software_Devel/44   5/19/06 1:48p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/43   5/15/06 2:26p hongtaoz
 * PR18360: updated PAL analog microcode to fix h-timing;
 *
 * Hydra_Software_Devel/42   4/28/06 12:26p hongtaoz
 * PR20126: updated 7038Bx PAL microcodes to comply with vbi encode timing
 * spec;
 *
 * Hydra_Software_Devel/41   3/23/06 4:52p hongtaoz
 * PR19812, PR20359: updated NTSC macrovision stack registers settings;
 *
 * Hydra_Software_Devel/40   3/22/06 7:40p hongtaoz
 * PR19812, PR20359: updated NTSC microcodes;
 *
 * Hydra_Software_Devel/39   3/20/06 3:03p hongtaoz
 * PR19223, PR18360: updated PAL test01/02 microcodes;
 *
 * Hydra_Software_Devel/38   3/7/06 4:15p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/37   12/14/05 6:37p hongtaoz
 * PR18025: updated NTSC test01/02 Macrovision modes microcodes;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/1   12/14/05 6:35p hongtaoz
 * PR18025: updated NTSC test01/02 Macrovision modes microcodes and fixed
 * colorstripes;
 *
 * Hydra_Software_Devel/36   11/29/05 4:58p hongtaoz
 * PR16298, PR16781, PR17804, PR18025, PR18232: implemented 704-sample
 * NTSC analog output; added two more Macrovision control modes; added
 * RGB SCART mode (G channel w/o sync) DAC config for Macrovision
 * validation test;
 *
 * Hydra_Software_Devel/35   11/9/05 2:21p hongtaoz
 * PR17779: updated NTSC analog microcode;
 *
 * Hydra_Software_Devel/33   9/21/05 6:28p albertl
 * PR16459:  Display format code restructured, moved to
 * bvdc_displayfmt_priv.c and bvdc_displayfmt.h, and changed to be  table
 * driven  to allow ease of adding new formats.
 *
 * Hydra_Software_Devel/32   9/14/05 4:53p hongtaoz
 * PR15749: add Hsync support for 3560;
 *
 * Hydra_Software_Devel/31   9/12/05 2:24p hongtaoz
 * PR16822, PR15749: updated PAL microcode; fixed Macrovision
 * certification control process;
 *
 * Hydra_Software_Devel/30   8/1/05 4:06p hongtaoz
 * PR16142, PR16394: added 656in analog out vbi pass-thru support for NTSC
 * format; fixed simul mode vdec vbi pass-thru; updated vec NTSC
 * microcode(080105); software workaround 656in hibernation mode
 * artifact;
 *
 * Hydra_Software_Devel/29   3/17/05 6:39p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/28   3/17/05 2:39p hongtaoz
 * PR14327, PR14336: updated NTSC microcode and Macrovision stack
 * registers; changed start addresses;
 *
 * Hydra_Software_Devel/27   1/31/05 12:29p hongtaoz
 * PR13782: fixed NTSC microcode sync level reduction at line 20 of bottom
 * field for Macrovision;
 *
 * Hydra_Software_Devel/26   1/21/05 6:19p hongtaoz
 * PR13887: fixing 576p Macrovision microcode; fixed 480p and PAL
 * Macrovision switch problem; fixed C0 Macrovision RGB output problem;
 *
 ***************************************************************************/

#include "bvdc_displayfmt_priv.h"
#include "bvdc_macrovision.h"

/****************************************************************
 *  Tables
 ****************************************************************/
/* NTSC macrovision cert test 01 it_microinstructions,
 * meets ITU-R BT.470 */
/* From vec_ntsc_macrovision_test_01.bss */
static const uint32_t
g_RamBVBInput_NTSC_Macrovision_Test01[BVDC_P_RAM_TABLE_SIZE] =
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288059, /* 2 */
	0x0028001b, /* 3 */
	0x0024000d, /* 4 */
	0x0020703f, /* 5 */
	0x00205040, /* 6 */
	0x00204014, /* 7 */
	0x00214044, /* 8 */
	0x00284021, /* 9 */
	0x0028c00f, /* 10 */
	0x0028c1d2, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c284, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00460003, /* 20 */
	0x00ad8003, /* 21 */
	0x00462001, /* 22 */
	0x00442005, /* 23 */
	0x00441006, /* 24 */
	0x00442006, /* 25 */
	0x0063700b, /* 26 */
	0x0063c001, /* 27 */
	0x00214018, /* 28 */
	0x0041c1e2, /* 29 */
	0x0021c018, /* 30 */
	0x00414001, /* 31 */
	0x0082d001, /* 32 */
	0x0020200f, /* 33 */
	0x00242010, /* 34 */
	0x00442006, /* 35 */
	0x004c1006, /* 36 */
	0x004c2006, /* 37 */
	0x004c4001, /* 38 */
	0x0064100b, /* 39 */
	0x00646001, /* 40 */
	0x0049c1e0, /* 41 */
	0x0049c001, /* 42 */
	0x004bc001, /* 43 */
	0x00815000, /* 44 */
	0x00444002, /* 45 */
	0x00442005, /* 46 */
	0x004c1006, /* 47 */
	0x004c2006, /* 48 */
	0x0064100b, /* 49 */
	0x00646001, /* 50 */
	0x0049c1e0, /* 51 */
	0x0049c001, /* 52 */
	0x004bc001, /* 53 */
	0x00815000, /* 54 */
	0x0083a002, /* 55 */
	0x00454001, /* 56 */
	0x00554001, /* 57 */
	0x0041c001, /* 58 */
	0x0051c001, /* 59 */
	0x0083f002, /* 60 */
	0x00414001, /* 61 */
	0x00514001, /* 62 */
	0x0041c001, /* 63 */
	0x0051c001, /* 64 */
	0x00844001, /* 65 */
	0x004d4001, /* 66 */
	0x005d4001, /* 67 */
	0x0049c001, /* 68 */
	0x0059c001, /* 69 */
	0x00849001, /* 70 */
	0x00494001, /* 71 */
	0x00594001, /* 72 */
	0x0049c001, /* 73 */
	0x0059c001, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00018, /* 81 */
	0x0085c004, /* 82 */
	0x00400801, /* 83 */
	0x0020005a, /* 84 */
	0x00657008, /* 85 */
	0x00400801, /* 86 */
	0x00859008, /* 87 */
	0x003000db, /* 88 */
	0x0020103d, /* 89 */
	0x00208052, /* 90 */
	0x0030004c, /* 91 */
	0x00400801, /* 92 */
	0x0020006a, /* 93 */
	0x00660008, /* 94 */
	0x00400801, /* 95 */
	0x00862010, /* 96 */
	0x003000cb, /* 97 */
	0x00201035, /* 98 */
	0x00208052, /* 99 */
	0x00300044, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00064, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08020, /* 114 */
	0x00c08020, /* 115 */
	0x00c01040, /* 116 */
	0x00c01040, /* 117 */
	0x004008f7, /* 118 */
	0x00a00064, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08020, /* 122 */
	0x00c08020, /* 123 */
	0x00c01040, /* 124 */
	0x00c01040, /* 125 */
	0x004008f0, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08020, /* 129 */
	0x00d01040, /* 130 */
	0x00000000, /* 131 */
	0x00400017, /* 132 */
	0x00691001, /* 133 */
	0x00400010, /* 134 */
	0x00691001, /* 135 */
	0x00690009, /* 136 */
	0x00400018, /* 137 */
	0x00400022, /* 138 */
	0x00691001, /* 139 */
	0x00400010, /* 140 */
	0x00691001, /* 141 */
	0x00690009, /* 142 */
	0x0040000e, /* 143 */
	0x00400010, /* 144 */
	0x00a00080, /* 145 */
	0x00504005, /* 146 */
	0x00000000, /* 147 */
	0x00400001, /* 148 */
	0x0020006d, /* 149 */
	0x00c03080, /* 150 */
	0x0020300b, /* 151 */
	0x00204014, /* 152 */
	0x0020c03c, /* 153 */
	0x0020401a, /* 154 */
	0x00400001, /* 155 */
	0x00894000, /* 156 */
	0x00000000, /* 157 */
	0x00200035, /* 158 */
	0x00202036, /* 159 */
	0x00200018, /* 160 */
	0x006af002, /* 161 */
	0x00400003, /* 162 */
	0x006af001, /* 163 */
	0x00400002, /* 164 */
	0x004000f2, /* 165 */
	0x00400006, /* 166 */
	0x006b3006, /* 167 */
	0x006af003, /* 168 */
	0x00400003, /* 169 */
	0x006af001, /* 170 */
	0x00400002, /* 171 */
	0x004000f1, /* 172 */
	0x00400006, /* 173 */
	0x006b3006, /* 174 */
	0x00400001, /* 175 */
	0x0020003f, /* 176 */
	0x00202036, /* 177 */
	0x00300018, /* 178 */
	0x00400001, /* 179 */
	0x0020007f, /* 180 */
	0x00202036, /* 181 */
	0x00300018, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00100709, /* 254 */
	0x5ae882f2, /* 255 */
};

/* NTSC macrovision cert test type 02 it_microinstructions,
 * meets ITU-R BT.470 */
/* From vec_ntsc_macrovision_test_02.bss */
static const uint32_t
g_RamBVBInput_NTSC_Macrovision_Test02[BVDC_P_RAM_TABLE_SIZE] =
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288059, /* 2 */
	0x0028001b, /* 3 */
	0x0024000d, /* 4 */
	0x0020703f, /* 5 */
	0x00205040, /* 6 */
	0x00204014, /* 7 */
	0x00214044, /* 8 */
	0x00284021, /* 9 */
	0x0028c00f, /* 10 */
	0x0028c1d2, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c284, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00460003, /* 20 */
	0x00ad8003, /* 21 */
	0x00462001, /* 22 */
	0x00442005, /* 23 */
	0x00441006, /* 24 */
	0x00442006, /* 25 */
	0x0063700b, /* 26 */
	0x0063c001, /* 27 */
	0x00214018, /* 28 */
	0x0041c1e2, /* 29 */
	0x0021c018, /* 30 */
	0x00414001, /* 31 */
	0x0082d001, /* 32 */
	0x0020200f, /* 33 */
	0x00242010, /* 34 */
	0x00442006, /* 35 */
	0x004c1006, /* 36 */
	0x004c2006, /* 37 */
	0x004c4001, /* 38 */
	0x0064100b, /* 39 */
	0x00646001, /* 40 */
	0x0049c1e0, /* 41 */
	0x0049c001, /* 42 */
	0x004bc001, /* 43 */
	0x00815000, /* 44 */
	0x00444002, /* 45 */
	0x00442005, /* 46 */
	0x004c1006, /* 47 */
	0x004c2006, /* 48 */
	0x0064100b, /* 49 */
	0x00646001, /* 50 */
	0x0049c1e0, /* 51 */
	0x0049c001, /* 52 */
	0x004bc001, /* 53 */
	0x00815000, /* 54 */
	0x0083a002, /* 55 */
	0x00454001, /* 56 */
	0x00554001, /* 57 */
	0x0041c001, /* 58 */
	0x0051c001, /* 59 */
	0x0083f002, /* 60 */
	0x00414001, /* 61 */
	0x00514001, /* 62 */
	0x0041c001, /* 63 */
	0x0051c001, /* 64 */
	0x00844001, /* 65 */
	0x004d4001, /* 66 */
	0x005d4001, /* 67 */
	0x0049c001, /* 68 */
	0x0059c001, /* 69 */
	0x00849001, /* 70 */
	0x00494001, /* 71 */
	0x00594001, /* 72 */
	0x0049c001, /* 73 */
	0x0059c001, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00018, /* 81 */
	0x0085c004, /* 82 */
	0x00400801, /* 83 */
	0x0020006a, /* 84 */
	0x00657008, /* 85 */
	0x00400801, /* 86 */
	0x00859008, /* 87 */
	0x003000cb, /* 88 */
	0x00201041, /* 89 */
	0x00208052, /* 90 */
	0x00300038, /* 91 */
	0x00400801, /* 92 */
	0x0020007a, /* 93 */
	0x00660008, /* 94 */
	0x00400801, /* 95 */
	0x00862010, /* 96 */
	0x003000eb, /* 97 */
	0x00201041, /* 98 */
	0x00208052, /* 99 */
	0x00300058, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00064, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08020, /* 114 */
	0x00c08020, /* 115 */
	0x00c01040, /* 116 */
	0x00c01040, /* 117 */
	0x004008f7, /* 118 */
	0x00a00064, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08020, /* 122 */
	0x00c08020, /* 123 */
	0x00c01040, /* 124 */
	0x00c01040, /* 125 */
	0x004008f0, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08020, /* 129 */
	0x00d01040, /* 130 */
	0x00000000, /* 131 */
	0x0040002f, /* 132 */
	0x00691001, /* 133 */
	0x00400025, /* 134 */
	0x00691001, /* 135 */
	0x00690008, /* 136 */
	0x00400018, /* 137 */
	0x0040001b, /* 138 */
	0x00691001, /* 139 */
	0x00400031, /* 140 */
	0x00691001, /* 141 */
	0x00690008, /* 142 */
	0x00400021, /* 143 */
	0x0040000d, /* 144 */
	0x00a00080, /* 145 */
	0x00504005, /* 146 */
	0x00000000, /* 147 */
	0x00400001, /* 148 */
	0x0020007e, /* 149 */
	0x00c03080, /* 150 */
	0x0020300b, /* 151 */
	0x0020c00c, /* 152 */
	0x0020400c, /* 153 */
	0x0020c050, /* 154 */
	0x00208030, /* 155 */
	0x00400001, /* 156 */
	0x00894000, /* 157 */
	0x00200035, /* 158 */
	0x00202036, /* 159 */
	0x00200018, /* 160 */
	0x006ab002, /* 161 */
	0x00400003, /* 162 */
	0x004000f5, /* 163 */
	0x00400008, /* 164 */
	0x006af004, /* 165 */
	0x006ab003, /* 166 */
	0x00400003, /* 167 */
	0x004000f4, /* 168 */
	0x00400008, /* 169 */
	0x006af004, /* 170 */
	0x00400001, /* 171 */
	0x0020003f, /* 172 */
	0x00202036, /* 173 */
	0x00300018, /* 174 */
	0x00400001, /* 175 */
	0x0020007f, /* 176 */
	0x00202036, /* 177 */
	0x00300018, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00100709, /* 254 */
	0x34cb96dd, /* 255 */
};

/* NTSC macrovision cert test 01 it_microinstructions,
 * meets ARIB requirements. */
/* From vec_ntsc_drop1_macrovision_test_01.bss */
static const uint32_t
g_RamBVBInput_NTSCARIB_Macrovision_Test01[BVDC_P_RAM_TABLE_SIZE] =
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288059, /* 2 */
	0x0028001b, /* 3 */
	0x0024000d, /* 4 */
	0x0020703f, /* 5 */
	0x00205040, /* 6 */
	0x00204014, /* 7 */
	0x00214044, /* 8 */
	0x00284021, /* 9 */
	0x0028c00f, /* 10 */
	0x0028c1d2, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c284, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00460003, /* 20 */
	0x00ad8003, /* 21 */
	0x00462001, /* 22 */
	0x00442005, /* 23 */
	0x00441006, /* 24 */
	0x00442006, /* 25 */
	0x0063a00b, /* 26 */
	0x0063f001, /* 27 */
	0x00214018, /* 28 */
	0x00414002, /* 29 */
	0x0041c1e0, /* 30 */
	0x0021c018, /* 31 */
	0x00414001, /* 32 */
	0x0082f001, /* 33 */
	0x0020200f, /* 34 */
	0x00242010, /* 35 */
	0x00442006, /* 36 */
	0x004c1006, /* 37 */
	0x004c2006, /* 38 */
	0x004c4001, /* 39 */
	0x0064400b, /* 40 */
	0x00649001, /* 41 */
	0x00494002, /* 42 */
	0x0049c1de, /* 43 */
	0x0049c001, /* 44 */
	0x004bc001, /* 45 */
	0x00815000, /* 46 */
	0x00444002, /* 47 */
	0x00442005, /* 48 */
	0x004c1006, /* 49 */
	0x004c2006, /* 50 */
	0x0064400b, /* 51 */
	0x00649001, /* 52 */
	0x00494002, /* 53 */
	0x0049c1de, /* 54 */
	0x0049c001, /* 55 */
	0x004bc001, /* 56 */
	0x00815000, /* 57 */
	0x0083d002, /* 58 */
	0x00454001, /* 59 */
	0x00554001, /* 60 */
	0x0041c001, /* 61 */
	0x0051c001, /* 62 */
	0x00842002, /* 63 */
	0x00414001, /* 64 */
	0x00514001, /* 65 */
	0x0041c001, /* 66 */
	0x0051c001, /* 67 */
	0x00847001, /* 68 */
	0x004d4001, /* 69 */
	0x005d4001, /* 70 */
	0x0049c001, /* 71 */
	0x0059c001, /* 72 */
	0x0084c001, /* 73 */
	0x00494001, /* 74 */
	0x00594001, /* 75 */
	0x0049c001, /* 76 */
	0x0059c001, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00018, /* 81 */
	0x0085c004, /* 82 */
	0x00400801, /* 83 */
	0x0020005a, /* 84 */
	0x00657008, /* 85 */
	0x00400801, /* 86 */
	0x00859008, /* 87 */
	0x003000db, /* 88 */
	0x0020103d, /* 89 */
	0x00208052, /* 90 */
	0x0030004c, /* 91 */
	0x00400801, /* 92 */
	0x0020006a, /* 93 */
	0x00660008, /* 94 */
	0x00400801, /* 95 */
	0x00862010, /* 96 */
	0x003000cb, /* 97 */
	0x00201035, /* 98 */
	0x00208052, /* 99 */
	0x00300044, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00064, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08020, /* 114 */
	0x00c08020, /* 115 */
	0x00c01040, /* 116 */
	0x00c01040, /* 117 */
	0x004008f7, /* 118 */
	0x00a00064, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08020, /* 122 */
	0x00c08020, /* 123 */
	0x00c01040, /* 124 */
	0x00c01040, /* 125 */
	0x004008f0, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08020, /* 129 */
	0x00d01040, /* 130 */
	0x00000000, /* 131 */
	0x00400017, /* 132 */
	0x00691001, /* 133 */
	0x00400010, /* 134 */
	0x00691001, /* 135 */
	0x00690009, /* 136 */
	0x00400018, /* 137 */
	0x00400022, /* 138 */
	0x00691001, /* 139 */
	0x00400010, /* 140 */
	0x00691001, /* 141 */
	0x00690009, /* 142 */
	0x0040000e, /* 143 */
	0x00400010, /* 144 */
	0x00a00080, /* 145 */
	0x00504005, /* 146 */
	0x00000000, /* 147 */
	0x00400001, /* 148 */
	0x0020006d, /* 149 */
	0x00c03080, /* 150 */
	0x0020300b, /* 151 */
	0x00204014, /* 152 */
	0x0020c03c, /* 153 */
	0x0020401a, /* 154 */
	0x00400001, /* 155 */
	0x00894000, /* 156 */
	0x00000000, /* 157 */
	0x00200035, /* 158 */
	0x00202036, /* 159 */
	0x00200018, /* 160 */
	0x006af002, /* 161 */
	0x00400003, /* 162 */
	0x006af001, /* 163 */
	0x00400002, /* 164 */
	0x004000f2, /* 165 */
	0x00400006, /* 166 */
	0x006b3006, /* 167 */
	0x006af003, /* 168 */
	0x00400003, /* 169 */
	0x006af001, /* 170 */
	0x00400002, /* 171 */
	0x004000f1, /* 172 */
	0x00400006, /* 173 */
	0x006b3006, /* 174 */
	0x00400001, /* 175 */
	0x0020003f, /* 176 */
	0x00202036, /* 177 */
	0x00300018, /* 178 */
	0x00400001, /* 179 */
	0x0020007f, /* 180 */
	0x00202036, /* 181 */
	0x00300018, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00011911, /* 254 */
	0x1a5377fc, /* 255 */
};

/* NTSC macrovision cert test 01 it_microinstructions,
 * meets ARIB requirements. */
/* From vec_ntsc_drop1_macrovision_test_02.bss */
static const uint32_t
g_RamBVBInput_NTSCARIB_Macrovision_Test02[BVDC_P_RAM_TABLE_SIZE] =
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288059, /* 2 */
	0x0028001b, /* 3 */
	0x0024000d, /* 4 */
	0x0020703f, /* 5 */
	0x00205040, /* 6 */
	0x00204014, /* 7 */
	0x00214044, /* 8 */
	0x00284021, /* 9 */
	0x0028c00f, /* 10 */
	0x0028c1d2, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c284, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00460003, /* 20 */
	0x00ad8003, /* 21 */
	0x00462001, /* 22 */
	0x00442005, /* 23 */
	0x00441006, /* 24 */
	0x00442006, /* 25 */
	0x0063a00b, /* 26 */
	0x0063f001, /* 27 */
	0x00214018, /* 28 */
	0x00414002, /* 29 */
	0x0041c1e0, /* 30 */
	0x0021c018, /* 31 */
	0x00414001, /* 32 */
	0x0082f001, /* 33 */
	0x0020200f, /* 34 */
	0x00242010, /* 35 */
	0x00442006, /* 36 */
	0x004c1006, /* 37 */
	0x004c2006, /* 38 */
	0x004c4001, /* 39 */
	0x0064400b, /* 40 */
	0x00649001, /* 41 */
	0x00494002, /* 42 */
	0x0049c1de, /* 43 */
	0x0049c001, /* 44 */
	0x004bc001, /* 45 */
	0x00815000, /* 46 */
	0x00444002, /* 47 */
	0x00442005, /* 48 */
	0x004c1006, /* 49 */
	0x004c2006, /* 50 */
	0x0064400b, /* 51 */
	0x00649001, /* 52 */
	0x00494002, /* 53 */
	0x0049c1de, /* 54 */
	0x0049c001, /* 55 */
	0x004bc001, /* 56 */
	0x00815000, /* 57 */
	0x0083d002, /* 58 */
	0x00454001, /* 59 */
	0x00554001, /* 60 */
	0x0041c001, /* 61 */
	0x0051c001, /* 62 */
	0x00842002, /* 63 */
	0x00414001, /* 64 */
	0x00514001, /* 65 */
	0x0041c001, /* 66 */
	0x0051c001, /* 67 */
	0x00847001, /* 68 */
	0x004d4001, /* 69 */
	0x005d4001, /* 70 */
	0x0049c001, /* 71 */
	0x0059c001, /* 72 */
	0x0084c001, /* 73 */
	0x00494001, /* 74 */
	0x00594001, /* 75 */
	0x0049c001, /* 76 */
	0x0059c001, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00018, /* 81 */
	0x0085c004, /* 82 */
	0x00400801, /* 83 */
	0x0020006a, /* 84 */
	0x00657008, /* 85 */
	0x00400801, /* 86 */
	0x00859008, /* 87 */
	0x003000cb, /* 88 */
	0x00201041, /* 89 */
	0x00208052, /* 90 */
	0x00300038, /* 91 */
	0x00400801, /* 92 */
	0x0020007a, /* 93 */
	0x00660008, /* 94 */
	0x00400801, /* 95 */
	0x00862010, /* 96 */
	0x003000eb, /* 97 */
	0x00201041, /* 98 */
	0x00208052, /* 99 */
	0x00300058, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00064, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08020, /* 114 */
	0x00c08020, /* 115 */
	0x00c01040, /* 116 */
	0x00c01040, /* 117 */
	0x004008f7, /* 118 */
	0x00a00064, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08020, /* 122 */
	0x00c08020, /* 123 */
	0x00c01040, /* 124 */
	0x00c01040, /* 125 */
	0x004008f0, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08020, /* 129 */
	0x00d01040, /* 130 */
	0x00000000, /* 131 */
	0x0040002f, /* 132 */
	0x00691001, /* 133 */
	0x00400025, /* 134 */
	0x00691001, /* 135 */
	0x00690008, /* 136 */
	0x00400018, /* 137 */
	0x0040001b, /* 138 */
	0x00691001, /* 139 */
	0x00400031, /* 140 */
	0x00691001, /* 141 */
	0x00690008, /* 142 */
	0x00400021, /* 143 */
	0x0040000d, /* 144 */
	0x00a00080, /* 145 */
	0x00504005, /* 146 */
	0x00000000, /* 147 */
	0x00400001, /* 148 */
	0x0020007e, /* 149 */
	0x00c03080, /* 150 */
	0x0020300b, /* 151 */
	0x0020c00c, /* 152 */
	0x0020400c, /* 153 */
	0x0020c050, /* 154 */
	0x00208030, /* 155 */
	0x00400001, /* 156 */
	0x00894000, /* 157 */
	0x00200035, /* 158 */
	0x00202036, /* 159 */
	0x00200018, /* 160 */
	0x006ab002, /* 161 */
	0x00400003, /* 162 */
	0x004000f5, /* 163 */
	0x00400008, /* 164 */
	0x006af004, /* 165 */
	0x006ab003, /* 166 */
	0x00400003, /* 167 */
	0x004000f4, /* 168 */
	0x00400008, /* 169 */
	0x006af004, /* 170 */
	0x00400001, /* 171 */
	0x0020003f, /* 172 */
	0x00202036, /* 173 */
	0x00300018, /* 174 */
	0x00400001, /* 175 */
	0x0020007f, /* 176 */
	0x00202036, /* 177 */
	0x00300018, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00011911, /* 254 */
	0xdb6d2c27, /* 255 */
};

/*PAL macrovision cert test 01 it_microinstructions, meets ITU-R BT.470 */
/* From vec_pal_macrovision_test_01.bss */
static const uint32_t
g_RamBVBInput_PAL_Macrovision_Test01[BVDC_P_RAM_TABLE_SIZE] =
#ifdef BVDC_P_WSE_VER3
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00208058, /* 2 */
	0x00240028, /* 3 */
	0x00207040, /* 4 */
	0x00205040, /* 5 */
	0x00204018, /* 6 */
	0x0021403e, /* 7 */
	0x0020402e, /* 8 */
	0x00812080, /* 9 */
	0x0020400d, /* 10 */
	0x0020c00f, /* 11 */
	0x0020c1b5, /* 12 */
	0x00208075, /* 13 */
	0x0024800b, /* 14 */
	0x0020e040, /* 15 */
	0x0020c28a, /* 16 */
	0x00801000, /* 17 */
	0x0028400d, /* 18 */
	0x0028c00f, /* 19 */
	0x0028c1b5, /* 20 */
	0x00288075, /* 21 */
	0x002c800b, /* 22 */
	0x0028e040, /* 23 */
	0x0028c295, /* 24 */
	0x00a24000, /* 25 */
	0x0028804d, /* 26 */
	0x00803000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00462003, /* 30 */
	0x00ad8000, /* 31 */
	0x00461001, /* 32 */
	0x00441004, /* 33 */
	0x00442003, /* 34 */
	0x00a00080, /* 35 */
	0x00442002, /* 36 */
	0x00444002, /* 37 */
	0x00454002, /* 38 */
	0x00a00080, /* 39 */
	0x00454020, /* 40 */
	0x00254018, /* 41 */
	0x0041c23c, /* 42 */
	0x0040c002, /* 43 */
	0x0020c018, /* 44 */
	0x004c2005, /* 45 */
	0x004c1005, /* 46 */
	0x004c2004, /* 47 */
	0x00a00080, /* 48 */
	0x004c2001, /* 49 */
	0x004c4001, /* 50 */
	0x004d4002, /* 51 */
	0x00a00080, /* 52 */
	0x004d4020, /* 53 */
	0x002d4018, /* 54 */
	0x0049c23c, /* 55 */
	0x0048c002, /* 56 */
	0x00404001, /* 57 */
	0x00204018, /* 58 */
	0x00442004, /* 59 */
	0x00462001, /* 60 */
	0x00a98000, /* 61 */
	0x00461001, /* 62 */
	0x00441004, /* 63 */
	0x00442003, /* 64 */
	0x00a00080, /* 65 */
	0x00442002, /* 66 */
	0x00454004, /* 67 */
	0x00a00080, /* 68 */
	0x00454020, /* 69 */
	0x00254018, /* 70 */
	0x0041c23c, /* 71 */
	0x0041c002, /* 72 */
	0x0021c018, /* 73 */
	0x004c2005, /* 74 */
	0x004c1005, /* 75 */
	0x004c2004, /* 76 */
	0x00a00080, /* 77 */
	0x004c2001, /* 78 */
	0x004c4003, /* 79 */
	0x00a00080, /* 80 */
	0x004d4020, /* 81 */
	0x002d4018, /* 82 */
	0x0049c23e, /* 83 */
	0x00404001, /* 84 */
	0x00204018, /* 85 */
	0x00442004, /* 86 */
	0x00462001, /* 87 */
	0x0081f000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00200012, /* 112 */
	0x00a00006, /* 113 */
	0x0087f001, /* 114 */
	0x00400001, /* 115 */
	0x00200101, /* 116 */
	0x0067a008, /* 117 */
	0x00000000, /* 118 */
	0x00000000, /* 119 */
	0x0020000b, /* 120 */
	0x00400801, /* 121 */
	0x0087c002, /* 122 */
	0x0030007a, /* 123 */
	0x00200019, /* 124 */
	0x0020102d, /* 125 */
	0x00308034, /* 126 */
	0x00400001, /* 127 */
	0x002000c5, /* 128 */
	0x00686008, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x0020000b, /* 132 */
	0x00400801, /* 133 */
	0x00888004, /* 134 */
	0x0030009a, /* 135 */
	0x00200035, /* 136 */
	0x00201031, /* 137 */
	0x00308034, /* 138 */
	0x00000000, /* 139 */
	0x00400806, /* 140 */
	0x00a00019, /* 141 */
	0x0069e00f, /* 142 */
	0x00400801, /* 143 */
	0x00c08008, /* 144 */
	0x00c08008, /* 145 */
	0x00c01010, /* 146 */
	0x00c01010, /* 147 */
	0x00400928, /* 148 */
	0x00a00019, /* 149 */
	0x0069e00f, /* 150 */
	0x00400801, /* 151 */
	0x00c08008, /* 152 */
	0x00c08008, /* 153 */
	0x00c01010, /* 154 */
	0x00c01010, /* 155 */
	0x00400923, /* 156 */
	0x0088c000, /* 157 */
	0x00400801, /* 158 */
	0x00c08008, /* 159 */
	0x00d01010, /* 160 */
	0x00000000, /* 161 */
	0x0040001b, /* 162 */
	0x006af001, /* 163 */
	0x00400020, /* 164 */
	0x006af001, /* 165 */
	0x006ae006, /* 166 */
	0x0040002d, /* 167 */
	0x0040002c, /* 168 */
	0x006af001, /* 169 */
	0x00400020, /* 170 */
	0x006af001, /* 171 */
	0x006ae006, /* 172 */
	0x0040001d, /* 173 */
	0x00400020, /* 174 */
	0x00a00060, /* 175 */
	0x00504002, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00400001, /* 193 */
	0x00200077, /* 194 */
	0x00c01020, /* 195 */
	0x00c02040, /* 196 */
	0x0020300b, /* 197 */
	0x0020c020, /* 198 */
	0x0020c020, /* 199 */
	0x00400001, /* 200 */
	0x008c1000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00400003, /* 223 */
	0x00400126, /* 224 */
	0x00400009, /* 225 */
	0x006f8004, /* 226 */
	0x006f4002, /* 227 */
	0x00400003, /* 228 */
	0x00400127, /* 229 */
	0x00400009, /* 230 */
	0x006f8004, /* 231 */
	0x006f4002, /* 232 */
	0x00400003, /* 233 */
	0x00400126, /* 234 */
	0x00400009, /* 235 */
	0x006f8004, /* 236 */
	0x006f4002, /* 237 */
	0x00400003, /* 238 */
	0x00400127, /* 239 */
	0x00400009, /* 240 */
	0x006f8004, /* 241 */
	0x006f4002, /* 242 */
	0x008df000, /* 243 */
	0x00400001, /* 244 */
	0x0020003f, /* 245 */
	0x00202036, /* 246 */
	0x00300018, /* 247 */
	0x00400001, /* 248 */
	0x0020007f, /* 249 */
	0x00202036, /* 250 */
	0x00300018, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00011310, /* 254 */
	0xd812b3e1, /* 255 */
};
#else
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288058, /* 2 */
	0x00240028, /* 3 */
	0x00207040, /* 4 */
	0x00205040, /* 5 */
	0x00204018, /* 6 */
	0x0021403e, /* 7 */
	0x00204039, /* 8 */
	0x0028400d, /* 9 */
	0x0028c00f, /* 10 */
	0x0028c1b5, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c28a, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00462003, /* 20 */
	0x00ad8000, /* 21 */
	0x00461001, /* 22 */
	0x00441004, /* 23 */
	0x00442005, /* 24 */
	0x00444002, /* 25 */
	0x00454022, /* 26 */
	0x0041c23c, /* 27 */
	0x0040c002, /* 28 */
	0x004c2005, /* 29 */
	0x004c1005, /* 30 */
	0x004c2005, /* 31 */
	0x004c4001, /* 32 */
	0x004d4022, /* 33 */
	0x0049c23c, /* 34 */
	0x0048c002, /* 35 */
	0x00404001, /* 36 */
	0x00442004, /* 37 */
	0x00462001, /* 38 */
	0x00a98000, /* 39 */
	0x00461001, /* 40 */
	0x00441004, /* 41 */
	0x00442005, /* 42 */
	0x00454024, /* 43 */
	0x0041c23c, /* 44 */
	0x0041c002, /* 45 */
	0x004c2005, /* 46 */
	0x004c1005, /* 47 */
	0x004c2005, /* 48 */
	0x004c4003, /* 49 */
	0x004d4020, /* 50 */
	0x002d4018, /* 51 */
	0x0049c23e, /* 52 */
	0x00404001, /* 53 */
	0x00442004, /* 54 */
	0x00462001, /* 55 */
	0x00815000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00006, /* 81 */
	0x0085f001, /* 82 */
	0x00400001, /* 83 */
	0x00200101, /* 84 */
	0x0065a008, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x0020000b, /* 88 */
	0x00400801, /* 89 */
	0x0085c002, /* 90 */
	0x0030007a, /* 91 */
	0x00200019, /* 92 */
	0x0020102d, /* 93 */
	0x00308034, /* 94 */
	0x00400001, /* 95 */
	0x002000c5, /* 96 */
	0x00666008, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x0020000b, /* 100 */
	0x00400801, /* 101 */
	0x00868004, /* 102 */
	0x0030009a, /* 103 */
	0x00200035, /* 104 */
	0x00201031, /* 105 */
	0x00308034, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00019, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08008, /* 114 */
	0x00c08008, /* 115 */
	0x00c01010, /* 116 */
	0x00c01010, /* 117 */
	0x00400928, /* 118 */
	0x00a00019, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08008, /* 122 */
	0x00c08008, /* 123 */
	0x00c01010, /* 124 */
	0x00c01010, /* 125 */
	0x00400923, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08008, /* 129 */
	0x00d01010, /* 130 */
	0x00000000, /* 131 */
	0x0040001b, /* 132 */
	0x00691001, /* 133 */
	0x00400020, /* 134 */
	0x00691001, /* 135 */
	0x00690006, /* 136 */
	0x0040002d, /* 137 */
	0x0040002c, /* 138 */
	0x00691001, /* 139 */
	0x00400020, /* 140 */
	0x00691001, /* 141 */
	0x00690006, /* 142 */
	0x0040001d, /* 143 */
	0x00400020, /* 144 */
	0x00a00060, /* 145 */
	0x00504002, /* 146 */
	0x00000000, /* 147 */
	0x00400001, /* 148 */
	0x00200077, /* 149 */
	0x00c01020, /* 150 */
	0x00c02040, /* 151 */
	0x0020300b, /* 152 */
	0x0020c020, /* 153 */
	0x0020c020, /* 154 */
	0x00400001, /* 155 */
	0x00894000, /* 156 */
	0x00000000, /* 157 */
	0x00400003, /* 158 */
	0x00400126, /* 159 */
	0x00400009, /* 160 */
	0x006b7004, /* 161 */
	0x006b3002, /* 162 */
	0x00400003, /* 163 */
	0x00400127, /* 164 */
	0x00400009, /* 165 */
	0x006b7004, /* 166 */
	0x006b3002, /* 167 */
	0x00400003, /* 168 */
	0x00400126, /* 169 */
	0x00400009, /* 170 */
	0x006b7004, /* 171 */
	0x006b3002, /* 172 */
	0x00400003, /* 173 */
	0x00400127, /* 174 */
	0x00400009, /* 175 */
	0x006b7004, /* 176 */
	0x006b3002, /* 177 */
	0x0089e000, /* 178 */
	0x00400001, /* 179 */
	0x0020003f, /* 180 */
	0x00202036, /* 181 */
	0x00300018, /* 182 */
	0x00400001, /* 183 */
	0x0020007f, /* 184 */
	0x00202036, /* 185 */
	0x00300018, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00060110, /* 254 */
	0x64c1862e, /* 255 */
};
#endif

/*PAL macrovision cert test 02 it_microinstructions, meets ITU-R BT.470 */
/* From vec_pal_macrovision_test_02.bss */
static const uint32_t
g_RamBVBInput_PAL_Macrovision_Test02[BVDC_P_RAM_TABLE_SIZE] =
#ifdef BVDC_P_WSE_VER3
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00208058, /* 2 */
	0x00240028, /* 3 */
	0x00207040, /* 4 */
	0x00205040, /* 5 */
	0x00204018, /* 6 */
	0x0021403e, /* 7 */
	0x0020402e, /* 8 */
	0x00812080, /* 9 */
	0x0020400d, /* 10 */
	0x0020c00f, /* 11 */
	0x0020c1b5, /* 12 */
	0x00208075, /* 13 */
	0x0024800b, /* 14 */
	0x0020e040, /* 15 */
	0x0020c28a, /* 16 */
	0x00801000, /* 17 */
	0x0028400d, /* 18 */
	0x0028c00f, /* 19 */
	0x0028c1b5, /* 20 */
	0x00288075, /* 21 */
	0x002c800b, /* 22 */
	0x0028e040, /* 23 */
	0x0028c295, /* 24 */
	0x00a24000, /* 25 */
	0x0028804d, /* 26 */
	0x00803000, /* 27 */
	0x00000000, /* 28 */
	0x00000000, /* 29 */
	0x00462003, /* 30 */
	0x00ad8000, /* 31 */
	0x00461001, /* 32 */
	0x00441004, /* 33 */
	0x00442003, /* 34 */
	0x00a00080, /* 35 */
	0x00442002, /* 36 */
	0x00444002, /* 37 */
	0x00454002, /* 38 */
	0x00a00080, /* 39 */
	0x00454020, /* 40 */
	0x00254018, /* 41 */
	0x0041c23c, /* 42 */
	0x0040c002, /* 43 */
	0x0020c018, /* 44 */
	0x004c2005, /* 45 */
	0x004c1005, /* 46 */
	0x004c2004, /* 47 */
	0x00a00080, /* 48 */
	0x004c2001, /* 49 */
	0x004c4001, /* 50 */
	0x004d4002, /* 51 */
	0x00a00080, /* 52 */
	0x004d4020, /* 53 */
	0x002d4018, /* 54 */
	0x0049c23c, /* 55 */
	0x0048c002, /* 56 */
	0x00404001, /* 57 */
	0x00204018, /* 58 */
	0x00442004, /* 59 */
	0x00462001, /* 60 */
	0x00a98000, /* 61 */
	0x00461001, /* 62 */
	0x00441004, /* 63 */
	0x00442003, /* 64 */
	0x00a00080, /* 65 */
	0x00442002, /* 66 */
	0x00454004, /* 67 */
	0x00a00080, /* 68 */
	0x00454020, /* 69 */
	0x00254018, /* 70 */
	0x0041c23c, /* 71 */
	0x0041c002, /* 72 */
	0x0021c018, /* 73 */
	0x004c2005, /* 74 */
	0x004c1005, /* 75 */
	0x004c2004, /* 76 */
	0x00a00080, /* 77 */
	0x004c2001, /* 78 */
	0x004c4003, /* 79 */
	0x00a00080, /* 80 */
	0x004d4020, /* 81 */
	0x002d4018, /* 82 */
	0x0049c23e, /* 83 */
	0x00404001, /* 84 */
	0x00204018, /* 85 */
	0x00442004, /* 86 */
	0x00462001, /* 87 */
	0x0081f000, /* 88 */
	0x00000000, /* 89 */
	0x00000000, /* 90 */
	0x00000000, /* 91 */
	0x00000000, /* 92 */
	0x00000000, /* 93 */
	0x00000000, /* 94 */
	0x00000000, /* 95 */
	0x00000000, /* 96 */
	0x00000000, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x00000000, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00000000, /* 110 */
	0x00000000, /* 111 */
	0x00200012, /* 112 */
	0x00a00006, /* 113 */
	0x0087f001, /* 114 */
	0x00400001, /* 115 */
	0x00200085, /* 116 */
	0x0067a008, /* 117 */
	0x00000000, /* 118 */
	0x00000000, /* 119 */
	0x0020000b, /* 120 */
	0x00400801, /* 121 */
	0x0087c002, /* 122 */
	0x003000aa, /* 123 */
	0x00200045, /* 124 */
	0x00201031, /* 125 */
	0x00308034, /* 126 */
	0x00400001, /* 127 */
	0x002000a1, /* 128 */
	0x00686008, /* 129 */
	0x00000000, /* 130 */
	0x00000000, /* 131 */
	0x0020000b, /* 132 */
	0x00400801, /* 133 */
	0x00888004, /* 134 */
	0x0030008a, /* 135 */
	0x00200029, /* 136 */
	0x0020102d, /* 137 */
	0x00308034, /* 138 */
	0x00000000, /* 139 */
	0x00400806, /* 140 */
	0x00a00019, /* 141 */
	0x0069e00f, /* 142 */
	0x00400801, /* 143 */
	0x00c08008, /* 144 */
	0x00c08008, /* 145 */
	0x00c01010, /* 146 */
	0x00c01010, /* 147 */
	0x00400928, /* 148 */
	0x00a00019, /* 149 */
	0x0069e00f, /* 150 */
	0x00400801, /* 151 */
	0x00c08008, /* 152 */
	0x00c08008, /* 153 */
	0x00c01010, /* 154 */
	0x00c01010, /* 155 */
	0x00400923, /* 156 */
	0x0088c000, /* 157 */
	0x00400801, /* 158 */
	0x00c08008, /* 159 */
	0x00d01010, /* 160 */
	0x00000000, /* 161 */
	0x0040001b, /* 162 */
	0x006af001, /* 163 */
	0x0040001d, /* 164 */
	0x006af001, /* 165 */
	0x006ae006, /* 166 */
	0x0040002a, /* 167 */
	0x0040002c, /* 168 */
	0x006af001, /* 169 */
	0x0040001d, /* 170 */
	0x006af001, /* 171 */
	0x006ae006, /* 172 */
	0x0040001a, /* 173 */
	0x0040001d, /* 174 */
	0x00a00060, /* 175 */
	0x00504005, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00400001, /* 193 */
	0x00200066, /* 194 */
	0x00c01020, /* 195 */
	0x00c02040, /* 196 */
	0x0020300b, /* 197 */
	0x0020401c, /* 198 */
	0x0020c030, /* 199 */
	0x00204018, /* 200 */
	0x00400001, /* 201 */
	0x008c1000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00400003, /* 223 */
	0x00400126, /* 224 */
	0x006f400d, /* 225 */
	0x006f0002, /* 226 */
	0x00400003, /* 227 */
	0x00400127, /* 228 */
	0x006f400d, /* 229 */
	0x006f0002, /* 230 */
	0x00400003, /* 231 */
	0x00400126, /* 232 */
	0x006f400d, /* 233 */
	0x006f0002, /* 234 */
	0x00400003, /* 235 */
	0x00400127, /* 236 */
	0x006f400d, /* 237 */
	0x006f0002, /* 238 */
	0x008df000, /* 239 */
	0x00400001, /* 240 */
	0x0020003f, /* 241 */
	0x00202036, /* 242 */
	0x00300018, /* 243 */
	0x00400001, /* 244 */
	0x0020007f, /* 245 */
	0x00202036, /* 246 */
	0x00300018, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00011310, /* 254 */
	0xb7786f06, /* 255 */
};
#else
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288058, /* 2 */
	0x00240028, /* 3 */
	0x00207040, /* 4 */
	0x00205040, /* 5 */
	0x00204018, /* 6 */
	0x0021403e, /* 7 */
	0x00204039, /* 8 */
	0x0028400d, /* 9 */
	0x0028c00f, /* 10 */
	0x0028c1b5, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c28a, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00462003, /* 20 */
	0x00ad8000, /* 21 */
	0x00461001, /* 22 */
	0x00441004, /* 23 */
	0x00442005, /* 24 */
	0x00444002, /* 25 */
	0x00454022, /* 26 */
	0x0041c23c, /* 27 */
	0x0040c002, /* 28 */
	0x004c2005, /* 29 */
	0x004c1005, /* 30 */
	0x004c2005, /* 31 */
	0x004c4001, /* 32 */
	0x004d4022, /* 33 */
	0x0049c23c, /* 34 */
	0x0048c002, /* 35 */
	0x00404001, /* 36 */
	0x00442004, /* 37 */
	0x00462001, /* 38 */
	0x00a98000, /* 39 */
	0x00461001, /* 40 */
	0x00441004, /* 41 */
	0x00442005, /* 42 */
	0x00454024, /* 43 */
	0x0041c23c, /* 44 */
	0x0041c002, /* 45 */
	0x004c2005, /* 46 */
	0x004c1005, /* 47 */
	0x004c2005, /* 48 */
	0x004c4003, /* 49 */
	0x004d4020, /* 50 */
	0x002d4018, /* 51 */
	0x0049c23e, /* 52 */
	0x00404001, /* 53 */
	0x00442004, /* 54 */
	0x00462001, /* 55 */
	0x00815000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00006, /* 81 */
	0x0085f001, /* 82 */
	0x00400001, /* 83 */
	0x00200085, /* 84 */
	0x0065a008, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x0020000b, /* 88 */
	0x00400801, /* 89 */
	0x0085c002, /* 90 */
	0x003000aa, /* 91 */
	0x00200045, /* 92 */
	0x00201031, /* 93 */
	0x00308034, /* 94 */
	0x00400001, /* 95 */
	0x002000a1, /* 96 */
	0x00666008, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x0020000b, /* 100 */
	0x00400801, /* 101 */
	0x00868004, /* 102 */
	0x0030008a, /* 103 */
	0x00200029, /* 104 */
	0x0020102d, /* 105 */
	0x00308034, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00019, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08008, /* 114 */
	0x00c08008, /* 115 */
	0x00c01010, /* 116 */
	0x00c01010, /* 117 */
	0x00400928, /* 118 */
	0x00a00019, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08008, /* 122 */
	0x00c08008, /* 123 */
	0x00c01010, /* 124 */
	0x00c01010, /* 125 */
	0x00400923, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08008, /* 129 */
	0x00d01010, /* 130 */
	0x00000000, /* 131 */
	0x0040001b, /* 132 */
	0x00691001, /* 133 */
	0x0040001d, /* 134 */
	0x00691001, /* 135 */
	0x00690006, /* 136 */
	0x0040002a, /* 137 */
	0x0040002c, /* 138 */
	0x00691001, /* 139 */
	0x0040001d, /* 140 */
	0x00691001, /* 141 */
	0x00690006, /* 142 */
	0x0040001a, /* 143 */
	0x0040001d, /* 144 */
	0x00a00060, /* 145 */
	0x00504005, /* 146 */
	0x00000000, /* 147 */
	0x00400001, /* 148 */
	0x00200066, /* 149 */
	0x00c01020, /* 150 */
	0x00c02040, /* 151 */
	0x0020300b, /* 152 */
	0x0020401f, /* 153 */
	0x0020c030, /* 154 */
	0x00204018, /* 155 */
	0x00400001, /* 156 */
	0x00894000, /* 157 */
	0x00400003, /* 158 */
	0x00400126, /* 159 */
	0x006b300d, /* 160 */
	0x006af002, /* 161 */
	0x00400003, /* 162 */
	0x00400127, /* 163 */
	0x006b300d, /* 164 */
	0x006af002, /* 165 */
	0x00400003, /* 166 */
	0x00400126, /* 167 */
	0x006b300d, /* 168 */
	0x006af002, /* 169 */
	0x00400003, /* 170 */
	0x00400127, /* 171 */
	0x006b300d, /* 172 */
	0x006af002, /* 173 */
	0x0089e000, /* 174 */
	0x00400001, /* 175 */
	0x0020003f, /* 176 */
	0x00202036, /* 177 */
	0x00300018, /* 178 */
	0x00400001, /* 179 */
	0x0020007f, /* 180 */
	0x00202036, /* 181 */
	0x00300018, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00052112, /* 254 */
	0x39bce9b2, /* 255 */
};
#endif

/* PAL-N macrovision cert test 01 it_microinstructions */
/* From vec_paln_macrovision_test_01.bss */
static const uint32_t
g_RamBVBInput_PALN_Macrovision_Test01[BVDC_P_RAM_TABLE_SIZE] =
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288059, /* 2 */
	0x0028001b, /* 3 */
	0x00240018, /* 4 */
	0x00207040, /* 5 */
	0x00205040, /* 6 */
	0x00204014, /* 7 */
	0x00214044, /* 8 */
	0x00204021, /* 9 */
	0x0020c00f, /* 10 */
	0x0028c1d2, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c284, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00462001, /* 20 */
	0x00ad8000, /* 21 */
	0x00461001, /* 22 */
	0x00441004, /* 23 */
	0x00442005, /* 24 */
	0x00444002, /* 25 */
	0x00454022, /* 26 */
	0x00254018, /* 27 */
	0x0041c23c, /* 28 */
	0x0040c002, /* 29 */
	0x0020c018, /* 30 */
	0x004c2005, /* 31 */
	0x004c1005, /* 32 */
	0x004c2005, /* 33 */
	0x004c4001, /* 34 */
	0x004d4022, /* 35 */
	0x002d4018, /* 36 */
	0x0049c23c, /* 37 */
	0x0048c002, /* 38 */
	0x00404001, /* 39 */
	0x00442004, /* 40 */
	0x00462001, /* 41 */
	0x00a98000, /* 42 */
	0x00461001, /* 43 */
	0x00441004, /* 44 */
	0x00442005, /* 45 */
	0x00454024, /* 46 */
	0x00254018, /* 47 */
	0x0041c23c, /* 48 */
	0x0041c002, /* 49 */
	0x0021c018, /* 50 */
	0x004c2005, /* 51 */
	0x004c1005, /* 52 */
	0x004c2005, /* 53 */
	0x004c4003, /* 54 */
	0x004d4020, /* 55 */
	0x002d4018, /* 56 */
	0x0049c23e, /* 57 */
	0x00404001, /* 58 */
	0x00442004, /* 59 */
	0x00462001, /* 60 */
	0x00815000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00006, /* 81 */
	0x0085f001, /* 82 */
	0x00400001, /* 83 */
	0x00200101, /* 84 */
	0x0065a008, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x0020000b, /* 88 */
	0x00400801, /* 89 */
	0x0085c002, /* 90 */
	0x0030007a, /* 91 */
	0x00200019, /* 92 */
	0x0020102d, /* 93 */
	0x00308034, /* 94 */
	0x00400001, /* 95 */
	0x002000c5, /* 96 */
	0x00666008, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x0020000b, /* 100 */
	0x00400801, /* 101 */
	0x00868004, /* 102 */
	0x0030009a, /* 103 */
	0x00200035, /* 104 */
	0x00201031, /* 105 */
	0x00308034, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00019, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08008, /* 114 */
	0x00c08008, /* 115 */
	0x00c01010, /* 116 */
	0x00c01010, /* 117 */
	0x00400928, /* 118 */
	0x00a00019, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08008, /* 122 */
	0x00c08008, /* 123 */
	0x00c01010, /* 124 */
	0x00c01010, /* 125 */
	0x00400923, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08008, /* 129 */
	0x00d01010, /* 130 */
	0x00000000, /* 131 */
	0x0040001b, /* 132 */
	0x00691001, /* 133 */
	0x00400020, /* 134 */
	0x00691001, /* 135 */
	0x00690006, /* 136 */
	0x0040002d, /* 137 */
	0x0040002c, /* 138 */
	0x00691001, /* 139 */
	0x00400020, /* 140 */
	0x00691001, /* 141 */
	0x00690006, /* 142 */
	0x0040001d, /* 143 */
	0x00400020, /* 144 */
	0x00a00060, /* 145 */
	0x00504002, /* 146 */
	0x00000000, /* 147 */
	0x00400001, /* 148 */
	0x00200073, /* 149 */
	0x00c01020, /* 150 */
	0x00c02040, /* 151 */
	0x0020300b, /* 152 */
	0x0020c020, /* 153 */
	0x0020c020, /* 154 */
	0x00400001, /* 155 */
	0x00894000, /* 156 */
	0x00000000, /* 157 */
	0x00400003, /* 158 */
	0x00400126, /* 159 */
	0x00400009, /* 160 */
	0x006b7004, /* 161 */
	0x006b3002, /* 162 */
	0x00400003, /* 163 */
	0x00400127, /* 164 */
	0x00400009, /* 165 */
	0x006b7004, /* 166 */
	0x006b3002, /* 167 */
	0x00400003, /* 168 */
	0x00400126, /* 169 */
	0x00400009, /* 170 */
	0x006b7004, /* 171 */
	0x006b3002, /* 172 */
	0x00400003, /* 173 */
	0x00400127, /* 174 */
	0x00400009, /* 175 */
	0x006b7004, /* 176 */
	0x006b3002, /* 177 */
	0x0089e000, /* 178 */
	0x00400001, /* 179 */
	0x0020003f, /* 180 */
	0x00202036, /* 181 */
	0x00300018, /* 182 */
	0x00400001, /* 183 */
	0x0020007f, /* 184 */
	0x00202036, /* 185 */
	0x00300018, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00060110, /* 254 */
	0x18ebb5f3, /* 255 */
};

/*PAL-N macrovision cert test 02 it_microinstructions */
/* From vec_paln_macrovision_test_02.bss */
static const uint32_t
g_RamBVBInput_PALN_Macrovision_Test02[BVDC_P_RAM_TABLE_SIZE] =
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288059, /* 2 */
	0x0028001b, /* 3 */
	0x00240018, /* 4 */
	0x00207040, /* 5 */
	0x00205040, /* 6 */
	0x00204014, /* 7 */
	0x00214044, /* 8 */
	0x00204021, /* 9 */
	0x0020c00f, /* 10 */
	0x0028c1d2, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c284, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00462001, /* 20 */
	0x00ad8000, /* 21 */
	0x00461001, /* 22 */
	0x00441004, /* 23 */
	0x00442005, /* 24 */
	0x00444002, /* 25 */
	0x00454022, /* 26 */
	0x00254018, /* 27 */
	0x0041c23c, /* 28 */
	0x0040c002, /* 29 */
	0x0020c018, /* 30 */
	0x004c2005, /* 31 */
	0x004c1005, /* 32 */
	0x004c2005, /* 33 */
	0x004c4001, /* 34 */
	0x004d4022, /* 35 */
	0x002d4018, /* 36 */
	0x0049c23c, /* 37 */
	0x0048c002, /* 38 */
	0x00404001, /* 39 */
	0x00442004, /* 40 */
	0x00462001, /* 41 */
	0x00a98000, /* 42 */
	0x00461001, /* 43 */
	0x00441004, /* 44 */
	0x00442005, /* 45 */
	0x00454024, /* 46 */
	0x00254018, /* 47 */
	0x0041c23c, /* 48 */
	0x0041c002, /* 49 */
	0x0021c018, /* 50 */
	0x004c2005, /* 51 */
	0x004c1005, /* 52 */
	0x004c2005, /* 53 */
	0x004c4003, /* 54 */
	0x004d4020, /* 55 */
	0x002d4018, /* 56 */
	0x0049c23e, /* 57 */
	0x00404001, /* 58 */
	0x00442004, /* 59 */
	0x00462001, /* 60 */
	0x00815000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00006, /* 81 */
	0x0085f001, /* 82 */
	0x00400001, /* 83 */
	0x00200085, /* 84 */
	0x0065a008, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x0020000b, /* 88 */
	0x00400801, /* 89 */
	0x0085c002, /* 90 */
	0x003000aa, /* 91 */
	0x00200045, /* 92 */
	0x00201031, /* 93 */
	0x00308034, /* 94 */
	0x00400001, /* 95 */
	0x002000a1, /* 96 */
	0x00666008, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x0020000b, /* 100 */
	0x00400801, /* 101 */
	0x00868004, /* 102 */
	0x0030008a, /* 103 */
	0x00200029, /* 104 */
	0x0020102d, /* 105 */
	0x00308034, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00019, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08008, /* 114 */
	0x00c08008, /* 115 */
	0x00c01010, /* 116 */
	0x00c01010, /* 117 */
	0x00400928, /* 118 */
	0x00a00019, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08008, /* 122 */
	0x00c08008, /* 123 */
	0x00c01010, /* 124 */
	0x00c01010, /* 125 */
	0x00400923, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08008, /* 129 */
	0x00d01010, /* 130 */
	0x00000000, /* 131 */
	0x0040001b, /* 132 */
	0x00691001, /* 133 */
	0x0040001d, /* 134 */
	0x00691001, /* 135 */
	0x00690006, /* 136 */
	0x0040002a, /* 137 */
	0x0040002c, /* 138 */
	0x00691001, /* 139 */
	0x0040001d, /* 140 */
	0x00691001, /* 141 */
	0x00690006, /* 142 */
	0x0040001a, /* 143 */
	0x0040001d, /* 144 */
	0x00a00060, /* 145 */
	0x00504005, /* 146 */
	0x00000000, /* 147 */
	0x00400001, /* 148 */
	0x00200062, /* 149 */
	0x00c01020, /* 150 */
	0x00c02040, /* 151 */
	0x0020300b, /* 152 */
	0x0020401f, /* 153 */
	0x0020c030, /* 154 */
	0x00204018, /* 155 */
	0x00400001, /* 156 */
	0x00894000, /* 157 */
	0x00400003, /* 158 */
	0x00400126, /* 159 */
	0x006b300d, /* 160 */
	0x006af002, /* 161 */
	0x00400003, /* 162 */
	0x00400127, /* 163 */
	0x006b300d, /* 164 */
	0x006af002, /* 165 */
	0x00400003, /* 166 */
	0x00400126, /* 167 */
	0x006b300d, /* 168 */
	0x006af002, /* 169 */
	0x00400003, /* 170 */
	0x00400127, /* 171 */
	0x006b300d, /* 172 */
	0x006af002, /* 173 */
	0x0089e000, /* 174 */
	0x00400001, /* 175 */
	0x0020003f, /* 176 */
	0x00202036, /* 177 */
	0x00300018, /* 178 */
	0x00400001, /* 179 */
	0x0020007f, /* 180 */
	0x00202036, /* 181 */
	0x00300018, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00052112, /* 254 */
	0x0b22d84d, /* 255 */
};

/* PAL-M macrovision cert test 01 it_microinstructions */
/* From vec_palm_macrovision_test_01.bss */
static const uint32_t
g_RamBVBInput_PALM_Macrovision_Test01[BVDC_P_RAM_TABLE_SIZE] =
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288059, /* 2 */
	0x0028001b, /* 3 */
	0x0024000c, /* 4 */
	0x00207040, /* 5 */
	0x00205040, /* 6 */
	0x0020400f, /* 7 */
	0x00214044, /* 8 */
	0x00204026, /* 9 */
	0x0020c00f, /* 10 */
	0x0028c1d2, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c284, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x004e2001, /* 20 */
	0x00ad8000, /* 21 */
	0x00461001, /* 22 */
	0x00441005, /* 23 */
	0x00442006, /* 24 */
	0x00444004, /* 25 */
	0x00454014, /* 26 */
	0x00214018, /* 27 */
	0x0041c1e2, /* 28 */
	0x0021c018, /* 29 */
	0x00404001, /* 30 */
	0x0020200f, /* 31 */
	0x00242010, /* 32 */
	0x00442006, /* 33 */
	0x004c1006, /* 34 */
	0x004c2005, /* 35 */
	0x004c4004, /* 36 */
	0x004d4016, /* 37 */
	0x0049c1e0, /* 38 */
	0x0029c018, /* 39 */
	0x004cc002, /* 40 */
	0x004c2005, /* 41 */
	0x004e2001, /* 42 */
	0x00ad8000, /* 43 */
	0x00461001, /* 44 */
	0x00441005, /* 45 */
	0x00442006, /* 46 */
	0x00444002, /* 47 */
	0x00454016, /* 48 */
	0x00214018, /* 49 */
	0x0041c1e0, /* 50 */
	0x0021c018, /* 51 */
	0x0040c002, /* 52 */
	0x00404001, /* 53 */
	0x0020200f, /* 54 */
	0x00242010, /* 55 */
	0x00442006, /* 56 */
	0x004c1006, /* 57 */
	0x004c2005, /* 58 */
	0x004c4002, /* 59 */
	0x004d4018, /* 60 */
	0x0049c1e2, /* 61 */
	0x0029c018, /* 62 */
	0x004c2005, /* 63 */
	0x004e2001, /* 64 */
	0x00815000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00006, /* 81 */
	0x0085c001, /* 82 */
	0x00400801, /* 83 */
	0x0020005a, /* 84 */
	0x00657008, /* 85 */
	0x00400801, /* 86 */
	0x00859002, /* 87 */
	0x003000db, /* 88 */
	0x0020103d, /* 89 */
	0x00208052, /* 90 */
	0x0030004c, /* 91 */
	0x00400801, /* 92 */
	0x0020006a, /* 93 */
	0x00660008, /* 94 */
	0x00400801, /* 95 */
	0x00862004, /* 96 */
	0x003000cb, /* 97 */
	0x00201035, /* 98 */
	0x00208052, /* 99 */
	0x00300044, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400803, /* 110 */
	0x00a00019, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08008, /* 114 */
	0x00c08008, /* 115 */
	0x00c01010, /* 116 */
	0x00c01010, /* 117 */
	0x004008f7, /* 118 */
	0x00a00019, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08008, /* 122 */
	0x00c08008, /* 123 */
	0x00c01010, /* 124 */
	0x00c01010, /* 125 */
	0x004008f3, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08008, /* 129 */
	0x00d01010, /* 130 */
	0x00000000, /* 131 */
	0x00400014, /* 132 */
	0x00691001, /* 133 */
	0x00400010, /* 134 */
	0x00691001, /* 135 */
	0x00690009, /* 136 */
	0x00400018, /* 137 */
	0x00400022, /* 138 */
	0x00691001, /* 139 */
	0x00400010, /* 140 */
	0x00691001, /* 141 */
	0x00690009, /* 142 */
	0x0040000e, /* 143 */
	0x00400013, /* 144 */
	0x00a00020, /* 145 */
	0x00504005, /* 146 */
	0x00000000, /* 147 */
	0x00400001, /* 148 */
	0x00200068, /* 149 */
	0x00c03020, /* 150 */
	0x0020300b, /* 151 */
	0x00204014, /* 152 */
	0x0020c03c, /* 153 */
	0x0020401a, /* 154 */
	0x00400001, /* 155 */
	0x00894000, /* 156 */
	0x00000000, /* 157 */
	0x004000fc, /* 158 */
	0x00400001, /* 159 */
	0x006ab006, /* 160 */
	0x006a7003, /* 161 */
	0x004000fc, /* 162 */
	0x00400001, /* 163 */
	0x006ab006, /* 164 */
	0x006a7003, /* 165 */
	0x0089e000, /* 166 */
	0x00400001, /* 167 */
	0x0020003f, /* 168 */
	0x00202036, /* 169 */
	0x00300018, /* 170 */
	0x00400001, /* 171 */
	0x0020007f, /* 172 */
	0x00202036, /* 173 */
	0x00300018, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00111209, /* 254 */
	0xdb18c15d, /* 255 */
};

/* PAL-M macrovision cert test 02 it_microinstructions */
/* From vec_palm_macrovision_test_02.bss */
static const uint32_t
g_RamBVBInput_PALM_Macrovision_Test02[BVDC_P_RAM_TABLE_SIZE] =
{
	0x0020400c, /* 0 */
	0x00a24000, /* 1 */
	0x00288059, /* 2 */
	0x0028001b, /* 3 */
	0x0024000c, /* 4 */
	0x00207040, /* 5 */
	0x00205040, /* 6 */
	0x0020400f, /* 7 */
	0x00214044, /* 8 */
	0x00204026, /* 9 */
	0x0020c00f, /* 10 */
	0x0028c1d2, /* 11 */
	0x00288075, /* 12 */
	0x002c800b, /* 13 */
	0x0028e040, /* 14 */
	0x0028c284, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x004e2001, /* 20 */
	0x00ad8000, /* 21 */
	0x00461001, /* 22 */
	0x00441005, /* 23 */
	0x00442006, /* 24 */
	0x00444004, /* 25 */
	0x00454014, /* 26 */
	0x00214018, /* 27 */
	0x0041c1e2, /* 28 */
	0x0021c018, /* 29 */
	0x00404001, /* 30 */
	0x0020200f, /* 31 */
	0x00242010, /* 32 */
	0x00442006, /* 33 */
	0x004c1006, /* 34 */
	0x004c2005, /* 35 */
	0x004c4004, /* 36 */
	0x004d4016, /* 37 */
	0x0049c1e0, /* 38 */
	0x0029c018, /* 39 */
	0x004cc002, /* 40 */
	0x004c2005, /* 41 */
	0x004e2001, /* 42 */
	0x00ad8000, /* 43 */
	0x00461001, /* 44 */
	0x00441005, /* 45 */
	0x00442006, /* 46 */
	0x00444002, /* 47 */
	0x00454016, /* 48 */
	0x00214018, /* 49 */
	0x0041c1e0, /* 50 */
	0x0021c018, /* 51 */
	0x0040c002, /* 52 */
	0x00404001, /* 53 */
	0x0020200f, /* 54 */
	0x00242010, /* 55 */
	0x00442006, /* 56 */
	0x004c1006, /* 57 */
	0x004c2005, /* 58 */
	0x004c4002, /* 59 */
	0x004d4018, /* 60 */
	0x0049c1e2, /* 61 */
	0x0029c018, /* 62 */
	0x004c2005, /* 63 */
	0x004e2001, /* 64 */
	0x00815000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00006, /* 81 */
	0x0085c001, /* 82 */
	0x00400801, /* 83 */
	0x0020006a, /* 84 */
	0x00657008, /* 85 */
	0x00400801, /* 86 */
	0x00859002, /* 87 */
	0x003000cb, /* 88 */
	0x00201041, /* 89 */
	0x00208052, /* 90 */
	0x00300038, /* 91 */
	0x00400801, /* 92 */
	0x0020007a, /* 93 */
	0x00660008, /* 94 */
	0x00400801, /* 95 */
	0x00862004, /* 96 */
	0x003000eb, /* 97 */
	0x00201041, /* 98 */
	0x00208052, /* 99 */
	0x00300058, /* 100 */
	0x00000000, /* 101 */
	0x00000000, /* 102 */
	0x00000000, /* 103 */
	0x00000000, /* 104 */
	0x00000000, /* 105 */
	0x00000000, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400803, /* 110 */
	0x00a00019, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08008, /* 114 */
	0x00c08008, /* 115 */
	0x00c01010, /* 116 */
	0x00c01010, /* 117 */
	0x004008f7, /* 118 */
	0x00a00019, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08008, /* 122 */
	0x00c08008, /* 123 */
	0x00c01010, /* 124 */
	0x00c01010, /* 125 */
	0x004008f3, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08008, /* 129 */
	0x00d01010, /* 130 */
	0x00000000, /* 131 */
	0x0040002c, /* 132 */
	0x00691001, /* 133 */
	0x00400025, /* 134 */
	0x00691001, /* 135 */
	0x00690008, /* 136 */
	0x00400018, /* 137 */
	0x0040001b, /* 138 */
	0x00691001, /* 139 */
	0x00400031, /* 140 */
	0x00691001, /* 141 */
	0x00690008, /* 142 */
	0x00400021, /* 143 */
	0x00400010, /* 144 */
	0x00a00020, /* 145 */
	0x00504005, /* 146 */
	0x00000000, /* 147 */
	0x00400001, /* 148 */
	0x00200079, /* 149 */
	0x00c03020, /* 150 */
	0x0020300b, /* 151 */
	0x0020c00c, /* 152 */
	0x0020400c, /* 153 */
	0x0020c050, /* 154 */
	0x00208030, /* 155 */
	0x00400001, /* 156 */
	0x00894000, /* 157 */
	0x004000fe, /* 158 */
	0x00400001, /* 159 */
	0x006ab004, /* 160 */
	0x006a7003, /* 161 */
	0x004000fe, /* 162 */
	0x00400001, /* 163 */
	0x006ab004, /* 164 */
	0x006a7003, /* 165 */
	0x0089e000, /* 166 */
	0x00400001, /* 167 */
	0x0020003f, /* 168 */
	0x00202036, /* 169 */
	0x00300018, /* 170 */
	0x00400001, /* 171 */
	0x0020007f, /* 172 */
	0x00202036, /* 173 */
	0x00300018, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00111209, /* 254 */
	0xb72826f0, /* 255 */
};

/*SECAM macrovision cert test type 01 is the same as type 1 */

/*SECAM macrovision cert test type 02 it_microinstructions */
/* From vec_secam_macrovision_test_02.bss */
static const uint32_t
g_RamBVBInput_SECAM_Macrovision_Test02[BVDC_P_RAM_TABLE_SIZE] =
#ifdef BVDC_P_WSE_VER3
{
	0x0020400c, /* 0 */
	0x00a04000, /* 1 */
	0x00218055, /* 2 */
	0x0020001f, /* 3 */
	0x0024000c, /* 4 */
	0x00207040, /* 5 */
	0x00205040, /* 6 */
	0x0020401a, /* 7 */
	0x00214055, /* 8 */
	0x00812080, /* 9 */
	0x00214022, /* 10 */
	0x0021c00b, /* 11 */
	0x0021c1b9, /* 12 */
	0x00218075, /* 13 */
	0x0025800b, /* 14 */
	0x0021e040, /* 15 */
	0x0021c28a, /* 16 */
	0x00801000, /* 17 */
	0x00294022, /* 18 */
	0x0029c00b, /* 19 */
	0x0029c1b9, /* 20 */
	0x00298075, /* 21 */
	0x002d800b, /* 22 */
	0x0029e040, /* 23 */
	0x0029c295, /* 24 */
	0x00a04000, /* 25 */
	0x00298055, /* 26 */
	0x00280014, /* 27 */
	0x00804000, /* 28 */
	0x00000000, /* 29 */
	0x00462003, /* 30 */
	0x00a98000, /* 31 */
	0x00461001, /* 32 */
	0x00441004, /* 33 */
	0x00442003, /* 34 */
	0x00a00080, /* 35 */
	0x00442002, /* 36 */
	0x00444002, /* 37 */
	0x00454002, /* 38 */
	0x00a00080, /* 39 */
	0x00454010, /* 40 */
	0x00444010, /* 41 */
	0x0040c23e, /* 42 */
	0x00442005, /* 43 */
	0x004c1005, /* 44 */
	0x004c2004, /* 45 */
	0x00a00080, /* 46 */
	0x004c2001, /* 47 */
	0x004c4003, /* 48 */
	0x002d400b, /* 49 */
	0x00a00080, /* 50 */
	0x004d4012, /* 51 */
	0x004c400e, /* 52 */
	0x0048c23e, /* 53 */
	0x0048c001, /* 54 */
	0x00442004, /* 55 */
	0x00462001, /* 56 */
	0x0081f000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00400003, /* 60 */
	0x00409002, /* 61 */
	0x00400002, /* 62 */
	0x0083d000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00006, /* 81 */
	0x0085f001, /* 82 */
	0x00400001, /* 83 */
	0x00200085, /* 84 */
	0x0065a008, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x0020000b, /* 88 */
	0x00400801, /* 89 */
	0x0085c002, /* 90 */
	0x003000aa, /* 91 */
	0x00200045, /* 92 */
	0x00201031, /* 93 */
	0x00308034, /* 94 */
	0x00400001, /* 95 */
	0x002000a1, /* 96 */
	0x00666008, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x0020000b, /* 100 */
	0x00400801, /* 101 */
	0x00868004, /* 102 */
	0x0030008a, /* 103 */
	0x00200029, /* 104 */
	0x0020102d, /* 105 */
	0x00308034, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00019, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08008, /* 114 */
	0x00c08008, /* 115 */
	0x00c01010, /* 116 */
	0x00c01010, /* 117 */
	0x00400928, /* 118 */
	0x00a00019, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08008, /* 122 */
	0x00c08008, /* 123 */
	0x00c01010, /* 124 */
	0x00c01010, /* 125 */
	0x00400923, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08008, /* 129 */
	0x00d01010, /* 130 */
	0x00000000, /* 131 */
	0x00400002, /* 132 */
	0x00400126, /* 133 */
	0x0069100d, /* 134 */
	0x0068d002, /* 135 */
	0x00400003, /* 136 */
	0x00400127, /* 137 */
	0x0069100d, /* 138 */
	0x0068d002, /* 139 */
	0x00884000, /* 140 */
	0x00400001, /* 141 */
	0x0020003f, /* 142 */
	0x00204036, /* 143 */
	0x00300018, /* 144 */
	0x00400001, /* 145 */
	0x0020007f, /* 146 */
	0x00204036, /* 147 */
	0x00300018, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00062310, /* 254 */
	0x97765401, /* 255 */
};
#else
{
	0x0020400c, /* 0 */
	0x00a04000, /* 1 */
	0x00298055, /* 2 */
	0x0028001f, /* 3 */
	0x0024000c, /* 4 */
	0x00207040, /* 5 */
	0x00205040, /* 6 */
	0x0020401a, /* 7 */
	0x00214060, /* 8 */
	0x00294022, /* 9 */
	0x0029c00b, /* 10 */
	0x0029c1b9, /* 11 */
	0x00298075, /* 12 */
	0x002d800b, /* 13 */
	0x0029e040, /* 14 */
	0x0029c28a, /* 15 */
	0x00801000, /* 16 */
	0x00000000, /* 17 */
	0x00000000, /* 18 */
	0x00000000, /* 19 */
	0x00462003, /* 20 */
	0x00a98000, /* 21 */
	0x00461001, /* 22 */
	0x00441004, /* 23 */
	0x00442005, /* 24 */
	0x00444002, /* 25 */
	0x00454012, /* 26 */
	0x00444010, /* 27 */
	0x0040c23e, /* 28 */
	0x00442005, /* 29 */
	0x004c1005, /* 30 */
	0x004c2004, /* 31 */
	0x004c2001, /* 32 */
	0x004c4003, /* 33 */
	0x004d4012, /* 34 */
	0x004c400e, /* 35 */
	0x0048c23e, /* 36 */
	0x0048c001, /* 37 */
	0x00442004, /* 38 */
	0x00462001, /* 39 */
	0x00815000, /* 40 */
	0x00000000, /* 41 */
	0x00000000, /* 42 */
	0x00000000, /* 43 */
	0x00000000, /* 44 */
	0x00000000, /* 45 */
	0x00000000, /* 46 */
	0x00000000, /* 47 */
	0x00000000, /* 48 */
	0x00000000, /* 49 */
	0x00400003, /* 50 */
	0x00409002, /* 51 */
	0x00400002, /* 52 */
	0x00833000, /* 53 */
	0x00000000, /* 54 */
	0x00000000, /* 55 */
	0x00000000, /* 56 */
	0x00000000, /* 57 */
	0x00000000, /* 58 */
	0x00000000, /* 59 */
	0x00000000, /* 60 */
	0x00000000, /* 61 */
	0x00000000, /* 62 */
	0x00000000, /* 63 */
	0x00000000, /* 64 */
	0x00000000, /* 65 */
	0x00000000, /* 66 */
	0x00000000, /* 67 */
	0x00000000, /* 68 */
	0x00000000, /* 69 */
	0x00000000, /* 70 */
	0x00000000, /* 71 */
	0x00000000, /* 72 */
	0x00000000, /* 73 */
	0x00000000, /* 74 */
	0x00000000, /* 75 */
	0x00000000, /* 76 */
	0x00000000, /* 77 */
	0x00000000, /* 78 */
	0x00000000, /* 79 */
	0x00200012, /* 80 */
	0x00a00006, /* 81 */
	0x0085f001, /* 82 */
	0x00400001, /* 83 */
	0x00200085, /* 84 */
	0x0065a008, /* 85 */
	0x00000000, /* 86 */
	0x00000000, /* 87 */
	0x0020000b, /* 88 */
	0x00400801, /* 89 */
	0x0085c002, /* 90 */
	0x003000aa, /* 91 */
	0x00200045, /* 92 */
	0x00201031, /* 93 */
	0x00308034, /* 94 */
	0x00400001, /* 95 */
	0x002000a1, /* 96 */
	0x00666008, /* 97 */
	0x00000000, /* 98 */
	0x00000000, /* 99 */
	0x0020000b, /* 100 */
	0x00400801, /* 101 */
	0x00868004, /* 102 */
	0x0030008a, /* 103 */
	0x00200029, /* 104 */
	0x0020102d, /* 105 */
	0x00308034, /* 106 */
	0x00000000, /* 107 */
	0x00000000, /* 108 */
	0x00000000, /* 109 */
	0x00400806, /* 110 */
	0x00a00019, /* 111 */
	0x0068000f, /* 112 */
	0x00400801, /* 113 */
	0x00c08008, /* 114 */
	0x00c08008, /* 115 */
	0x00c01010, /* 116 */
	0x00c01010, /* 117 */
	0x00400928, /* 118 */
	0x00a00019, /* 119 */
	0x0068000f, /* 120 */
	0x00400801, /* 121 */
	0x00c08008, /* 122 */
	0x00c08008, /* 123 */
	0x00c01010, /* 124 */
	0x00c01010, /* 125 */
	0x00400923, /* 126 */
	0x0086e000, /* 127 */
	0x00400801, /* 128 */
	0x00c08008, /* 129 */
	0x00d01010, /* 130 */
	0x00000000, /* 131 */
	0x00400002, /* 132 */
	0x00400126, /* 133 */
	0x0069100d, /* 134 */
	0x0068d002, /* 135 */
	0x00400003, /* 136 */
	0x00400127, /* 137 */
	0x0069100d, /* 138 */
	0x0068d002, /* 139 */
	0x00884000, /* 140 */
	0x00400001, /* 141 */
	0x0020003f, /* 142 */
	0x00204036, /* 143 */
	0x00300018, /* 144 */
	0x00400001, /* 145 */
	0x0020007f, /* 146 */
	0x00204036, /* 147 */
	0x00300018, /* 148 */
	0x00000000, /* 149 */
	0x00000000, /* 150 */
	0x00000000, /* 151 */
	0x00000000, /* 152 */
	0x00000000, /* 153 */
	0x00000000, /* 154 */
	0x00000000, /* 155 */
	0x00000000, /* 156 */
	0x00000000, /* 157 */
	0x00000000, /* 158 */
	0x00000000, /* 159 */
	0x00000000, /* 160 */
	0x00000000, /* 161 */
	0x00000000, /* 162 */
	0x00000000, /* 163 */
	0x00000000, /* 164 */
	0x00000000, /* 165 */
	0x00000000, /* 166 */
	0x00000000, /* 167 */
	0x00000000, /* 168 */
	0x00000000, /* 169 */
	0x00000000, /* 170 */
	0x00000000, /* 171 */
	0x00000000, /* 172 */
	0x00000000, /* 173 */
	0x00000000, /* 174 */
	0x00000000, /* 175 */
	0x00000000, /* 176 */
	0x00000000, /* 177 */
	0x00000000, /* 178 */
	0x00000000, /* 179 */
	0x00000000, /* 180 */
	0x00000000, /* 181 */
	0x00000000, /* 182 */
	0x00000000, /* 183 */
	0x00000000, /* 184 */
	0x00000000, /* 185 */
	0x00000000, /* 186 */
	0x00000000, /* 187 */
	0x00000000, /* 188 */
	0x00000000, /* 189 */
	0x00000000, /* 190 */
	0x00000000, /* 191 */
	0x00000000, /* 192 */
	0x00000000, /* 193 */
	0x00000000, /* 194 */
	0x00000000, /* 195 */
	0x00000000, /* 196 */
	0x00000000, /* 197 */
	0x00000000, /* 198 */
	0x00000000, /* 199 */
	0x00000000, /* 200 */
	0x00000000, /* 201 */
	0x00000000, /* 202 */
	0x00000000, /* 203 */
	0x00000000, /* 204 */
	0x00000000, /* 205 */
	0x00000000, /* 206 */
	0x00000000, /* 207 */
	0x00000000, /* 208 */
	0x00000000, /* 209 */
	0x00000000, /* 210 */
	0x00000000, /* 211 */
	0x00000000, /* 212 */
	0x00000000, /* 213 */
	0x00000000, /* 214 */
	0x00000000, /* 215 */
	0x00000000, /* 216 */
	0x00000000, /* 217 */
	0x00000000, /* 218 */
	0x00000000, /* 219 */
	0x00000000, /* 220 */
	0x00000000, /* 221 */
	0x00000000, /* 222 */
	0x00000000, /* 223 */
	0x00000000, /* 224 */
	0x00000000, /* 225 */
	0x00000000, /* 226 */
	0x00000000, /* 227 */
	0x00000000, /* 228 */
	0x00000000, /* 229 */
	0x00000000, /* 230 */
	0x00000000, /* 231 */
	0x00000000, /* 232 */
	0x00000000, /* 233 */
	0x00000000, /* 234 */
	0x00000000, /* 235 */
	0x00000000, /* 236 */
	0x00000000, /* 237 */
	0x00000000, /* 238 */
	0x00000000, /* 239 */
	0x00000000, /* 240 */
	0x00000000, /* 241 */
	0x00000000, /* 242 */
	0x00000000, /* 243 */
	0x00000000, /* 244 */
	0x00000000, /* 245 */
	0x00000000, /* 246 */
	0x00000000, /* 247 */
	0x00000000, /* 248 */
	0x00000000, /* 249 */
	0x00000000, /* 250 */
	0x00000000, /* 251 */
	0x00000000, /* 252 */
	0x008fd000, /* 253 */
	0x00060110, /* 254 */
	0xafcf4c7e, /* 255 */
};
#endif

static const uint32_t s_aulItTable_NTSC_Macrovision_Test01[] =
{
	BVDC_P_MAKE_IT_ADDR(110, 80, 0x14, 0, 158, 148, 132),
	/*
	1) STACK0 is EVEN stack(NTSC microcode specific)
	2) STACK1 is ODD stack(NTSC microcode specific)
	3) STACK2 is AGC_SEL
	4) STACK3 is AGC_H_A
	5) STACK4 is AGC_H_B
	6) STACK5 is AGC_ON
	7) STACK6 is PS_ON
	8) STACK7 is UV_Reg
	*/
	BVDC_P_MAKE_IT_STACK(0, 0, 0xf, 0x7e07, 0xff8, 0xe, 0x13, 0xff8),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_NTSC_Macrovision_Test02[] =
{
	BVDC_P_MAKE_IT_ADDR(110, 80, 0x14, 0, 158, 148, 132),
	BVDC_P_MAKE_IT_STACK(0, 0, 0x23, 0x6dcf, 0x36b8, 0x13, 3, 0x36b8),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_NTSC_Macrovision_1[] =
{
	BVDC_P_MAKE_IT_ADDR(105, 80, 0x14, 0, 175, 253, 253),
	BVDC_P_MAKE_IT_STACK(0, 0, 0xf, 0, 0x7f8, 0xf, 0xf, 0x7f8),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_NTSC_Macrovision_2[] =
{
	BVDC_P_MAKE_IT_ADDR(105, 80, 0x14, 0, 225, 215, 200),
	BVDC_P_MAKE_IT_STACK(0, 0, 0xf, 0, 0x7f8, 0xf, 0x3, 0x7f8),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_NTSC_Macrovision_3[] =
{
	BVDC_P_MAKE_IT_ADDR(105, 80, 0x14, 0, 153, 143, 127),
	BVDC_P_MAKE_IT_STACK(0, 0, 0xf, 0, 0x7f8, 0xf, 0xf, 0x7f8),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_PAL_M_Macrovision_Test01[] =
/* Copied from NTSC array */
{
	BVDC_P_MAKE_IT_ADDR(110, 80, 0x14, 0, 158, 148, 132),
	/*
	1) STACK0 is EVEN stack(PAL_M microcode specific)
	2) STACK1 is ODD stack(PAL_M microcode specific)
	3) STACK2 is AGC_SEL
	4) STACK3 is AGC_H_A
	5) STACK4 is AGC_H_B
	6) STACK5 is AGC_ON
	7) STACK6 is PS_ON
	8) STACK7 is UV_Reg
	*/
	BVDC_P_MAKE_IT_STACK(0xf, 0x7e07, 0xff8, 0xe, 0x13, 0xff8, 0x0, 0x0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_PAL_M_Macrovision_Test02[] =
/* Copied from NTSC array */
{
	BVDC_P_MAKE_IT_ADDR(110, 80, 0x14, 0, 158, 148, 132),
	BVDC_P_MAKE_IT_STACK(0x23, 0x6dcf, 0x36b8, 0x13, 0x3, 0x36b8, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_PAL_M_Macrovision_1[] =
{
	BVDC_P_MAKE_IT_ADDR(0x69, 0x50, 0x14, 0, 0xAF, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0xf, 0, 0x7f8, 0xf, 0x0, 0x7f8, 0x0, 0x0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_PAL_M_Macrovision_2[] =
/* Copied from NTSC array, except for MC_3_START_ADDR and MC_6_START_ADDR. */
{
	BVDC_P_MAKE_IT_ADDR(0x69, 0x50, 0x14, 0, 0xe1, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0xf, 0, 0x7f8, 0xf, 0x0, 0x7f8, 0x0, 0x0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_PAL_M_Macrovision_3[] =
{
	BVDC_P_MAKE_IT_ADDR(0x69, 0x50, 0x14, 0, 0x99, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0xf, 0, 0x7f8, 0xf, 0x0, 0x7f8, 0x0, 0x0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t  s_aulItTable_480p_Macrovision[] =
{
	BVDC_P_MAKE_IT_ADDR(0x40, 0x28, 0x14, 0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ZERO),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_PAL_Macrovision_Type_1[] =
{
#ifdef BVDC_P_WSE_VER3
	BVDC_P_MAKE_IT_ADDR(140, 112,  30,   0, 162, 253, 253),
	BVDC_P_MAKE_IT_STACK(0xFE, 0x154, 0x3FE, 0x7e, 0, 0x3FE, 0, 0),
#else
	BVDC_P_MAKE_IT_ADDR(0x6e, 0x50, 0x14, 0, 0x87, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0xFE, 0x154, 0x3FE, 0x7e, 0, 0x3FE, 0, 0),
#endif
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(0, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_PAL_Macrovision_Type_2[] =
{
#ifdef BVDC_P_WSE_VER3
	BVDC_P_MAKE_IT_ADDR(140, 112,  30,   0, 193, 253, 253),
	BVDC_P_MAKE_IT_STACK(0xFE, 0x154, 0x3FE, 0x7e, 0, 0x3FE, 0, 0),
#else
	BVDC_P_MAKE_IT_ADDR(0x6e, 0x50, 0x14, 0, 0xaa, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0xFE, 0x154, 0x3FE, 0x7e, 0, 0x3FE, 0, 0),
#endif
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(0, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_PAL_Macrovision_Type_3[] =
{
#ifdef BVDC_P_WSE_VER3
	BVDC_P_MAKE_IT_ADDR(140, 112,  30,   0, 223, 253, 253),
	BVDC_P_MAKE_IT_STACK(0xFE, 0x154, 0x3FE, 0x7e, 0, 0x3FE, 0, 0),
#else
	BVDC_P_MAKE_IT_ADDR(0x6e, 0x50, 0x14, 0, 0xc8, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0xFE, 0x154, 0x3FE, 0x7e, 0, 0x3FE, 0, 0),
#endif
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(0, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, DISABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_PAL_Macrovision_Test01[] =
{
#ifdef BVDC_P_WSE_VER3
	BVDC_P_MAKE_IT_ADDR(140, 112,  30,   0, 223, 193, 162),
	BVDC_P_MAKE_IT_STACK(0xFE, 0x154, 0x3FE, 0x7e, 0x3, 0x3FE, 0, 0),
#else
	BVDC_P_MAKE_IT_ADDR(0x6e, 0x50, 0x14, 0, 0x9e, 0x94, 0x84),
	BVDC_P_MAKE_IT_STACK(0xFE, 0x154, 0x3FE, 0x7e, 0x3, 0x3FE, 0, 0),
#endif
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t s_aulItTable_PAL_Macrovision_Test02[] =
{
#ifdef BVDC_P_WSE_VER3
	BVDC_P_MAKE_IT_ADDR(140, 112,  30,   0, 223, 193, 162),
	BVDC_P_MAKE_IT_STACK(0x53, 0x1f43, 0x78c6, 0xa3, 0x19, 0x78c6, 0, 3),
#else
	BVDC_P_MAKE_IT_ADDR(0x6e, 0x50, 0x14, 0, 0x9e, 0x94, 0x84),
	BVDC_P_MAKE_IT_STACK(0x53, 0x1f43, 0x78c6, 0xa3, 0x19, 0x78c6, 0, 3),
#endif
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, LINE_A),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 3, 0),
};

/* SECAM type 1 */
static const uint32_t s_aulItTable_SECAM_Macrovision[] =
{
#ifdef BVDC_P_WSE_VER3 /** { **/
	BCHP_FIELD_DATA(PRIM_IT_ADDR_0_3,MC_3_START_ADDR, 110) |
	BCHP_FIELD_DATA(PRIM_IT_ADDR_0_3,MC_2_START_ADDR, 80) |
	BCHP_FIELD_DATA(PRIM_IT_ADDR_0_3,MC_1_START_ADDR, 30) |
	BCHP_FIELD_DATA(PRIM_IT_ADDR_0_3,MC_0_START_ADDR, 0),

	BCHP_FIELD_DATA(PRIM_IT_ADDR_4_6,MC_6_START_ADDR,  60) |
	BCHP_FIELD_DATA(PRIM_IT_ADDR_4_6,MC_5_START_ADDR, 253) |
	BCHP_FIELD_DATA(PRIM_IT_ADDR_4_6,MC_4_START_ADDR, 132),

	BCHP_FIELD_DATA(PRIM_IT_STACK_reg_0_1, REG_0, 0x154) |
	BCHP_FIELD_DATA(PRIM_IT_STACK_reg_0_1, REG_1, 0xFE),

	BCHP_FIELD_DATA(PRIM_IT_STACK_reg_2_3, REG_2, 0x7e) |
	BCHP_FIELD_DATA(PRIM_IT_STACK_reg_2_3, REG_3, 0x3FE),

	BCHP_FIELD_DATA(PRIM_IT_STACK_reg_4_5, REG_4, 0x3FE) |
	BCHP_FIELD_DATA(PRIM_IT_STACK_reg_4_5, REG_5, 0),

	BCHP_FIELD_DATA(PRIM_IT_STACK_reg_6_7, REG_6, 0) |
	BCHP_FIELD_DATA(PRIM_IT_STACK_reg_6_7, REG_7, 0),

	BCHP_FIELD_DATA(PRIM_IT_EVENT_SELECTION, MC_6, 6) |
	BCHP_FIELD_DATA(PRIM_IT_EVENT_SELECTION, MC_5, 0) |
	BCHP_FIELD_DATA(PRIM_IT_EVENT_SELECTION, MC_4, 0) |
	BCHP_FIELD_DATA(PRIM_IT_EVENT_SELECTION, MC_3, 0) |
	BCHP_FIELD_DATA(PRIM_IT_EVENT_SELECTION, MC_2, 0) |
	BCHP_FIELD_DATA(PRIM_IT_EVENT_SELECTION, MC_1, 6),

	BCHP_FIELD_ENUM(PRIM_IT_PCL_0, VBI_DATA_ACTIVE_ENABLE,         ON) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_0, VBI_DATA_ACTIVE_MUX_SELECT ,    3) |
#if BVDC_P_SUPPORT_EXT_SYNC_PCL_0
	BCHP_FIELD_ENUM(PRIM_IT_PCL_0, EXT_VSYNC_ENABLE,               ON) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_0, EXT_VSYNC_MUX_SELECT,           0) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_0, EXT_HSYNC_ENABLE,               ON) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_0, EXT_HSYNC_MUX_SELECT,           0) |
#endif
	BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_4 ,            ZERO) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_3 ,            LINE_D) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_2 ,            LINE_A) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_1 ,            LINE_B) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_0 ,            LINE_C) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_E_SELECT,           0) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_D_SELECT,           3) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_4_SELECT,           0) |
	(uint32_t)BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_3_SELECT, 2),

	BCHP_FIELD_DATA(PRIM_IT_PCL_1, reserved0,                0) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_1, BOTTLES_ENABLE,           DISABLED) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_1, BOTTLES_MUX_A_SELECT,     2) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_1, BOTTLES_MUX_0_SELECT,     2) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_1, reserved1,                0) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_1, COLOR_BURST_AND_TERM_2 ,  LINE_C) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_1, COLOR_BURST_AND_TERM_1 ,  ZERO) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_1, COLOR_BURST_AND_TERM_0 ,  ONE) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_1, COLOR_BURST_MUX_C_SELECT, 1) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_1, COLOR_BURST_MUX_B_SELECT, 0) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_1, COLOR_BURST_MUX_2_SELECT, 2) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_1, COLOR_BURST_MUX_1_SELECT, 0),

	BCHP_FIELD_DATA(PRIM_IT_PCL_2, reserved0 ,           0) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_2, V_FLIP_ENABLE,        ENABLED) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_2, V_FLIP_MUX_A_SELECT , 5) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_2, V_FLIP_MUX_0_SELECT , 5) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_2, SEC_NEG_SYNC_ENABLE, ENABLED) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_2, SEC_NEG_SYNC_MUX_A_SELECT, 0) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_2, SEC_NEG_SYNC_MUX_0_SELECT, 0) |
#if !BVDC_P_SUPPORT_EXT_SYNC_PCL_0
	BCHP_FIELD_ENUM(PRIM_IT_PCL_2, EXT_VSYNC_ENABLE,               ON) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_2, EXT_VSYNC_MUX_SELECT,           0) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_2, EXT_HSYNC_ENABLE,               ON) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_2, EXT_HSYNC_MUX_SELECT,           0) |
#endif
	BCHP_FIELD_ENUM(PRIM_IT_PCL_2, U_FLIP_ENABLE,        ENABLED) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_2, U_FLIP_MUX_A_SELECT , 6) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_2, U_FLIP_MUX_0_SELECT , 7),

	BCHP_FIELD_DATA(PRIM_IT_PCL_3, reserved0,                 0) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_3, LINE_COUNT_CLEAR_ENABLE,  ON) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_3, LINE_COUNT_CLEAR_SELECT,  1) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_3, NEW_LINE_ENABLE,          ON) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_3, NEW_LINE_MUX_SELECT,      0) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_3, V_ACTIVE_ENABLE,          ON) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_3, V_ACTIVE_MUX_SELECT,      1) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_3, H_ACTIVE_ENABLE,          ON) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_3, H_ACTIVE_MUX_SELECT,     	1) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_3, ODD_EVEN_ENABLE,       	ON) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_3, ODD_EVEN_MUX_SELECT,      3) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_3, VSYNC_ENABLE,             ON) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_3, VSYNC_MUX_SELECT,         0) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_3, VBLANK_ENABLE,            ON) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_3, VBLANK_MUX_SELECT,        3),

	BCHP_FIELD_DATA(PRIM_IT_PCL_4, reserved0,         0) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_4, PSA_AND_TERM_1,    ZERO) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_4, PSA_AND_TERM_0,    LINE_A) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_4, PSA_MUX_B_SELECT , 0) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_4, PSA_MUX_A_SELECT , 3) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_4, PSA_MUX_1_SELECT , 0) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_4, PSA_MUX_0_SELECT , 3) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_4, reserved1,         0) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_4, PSB_AND_TERM_2,    ONE) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_4, PSB_AND_TERM_1,    ZERO) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_4, PSB_AND_TERM_0,    ZERO) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_4, PSB_MUX_C_SELECT,  0) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_4, PSB_MUX_B_SELECT,  0) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_4, PSB_MUX_2_SELECT , 0) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_4, PSB_MUX_1_SELECT , 0),

#if !BVDC_P_SUPPORT_MODULAR_VEC
	BCHP_FIELD_DATA(PRIM_IT_PCL_5, reserved0 ,             0) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_5, PG_END_LINE_MUX_SELECT, 3) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_5, PG2_ENABLE,             ENABLED) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_5, PG2_MUX_A_SELECT ,      7) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_5, PG2_MUX_0_SELECT ,      2) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_5, PG1_ENABLE,             ENABLED) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_5, PG1_MUX_A_SELECT ,      7) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_5, PG1_MUX_0_SELECT ,      2) |
	BCHP_FIELD_ENUM(PRIM_IT_PCL_5, PG0_ENABLE,             ENABLED) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_5, PG0_MUX_A_SELECT ,      7) |
	BCHP_FIELD_DATA(PRIM_IT_PCL_5, PG0_MUX_0_SELECT ,      3)
#endif
#else /** } { **/
	BVDC_P_MAKE_IT_ADDR(0x50, 0x32, 0x14, 0, 0x6e, 0, 0x84),
	BVDC_P_MAKE_IT_STACK(0xFE, 0x154, 0x3FE, 0x7e, 0, 0x3FE, 0, 0),
	BVDC_P_MAKE_IT_EVENT(6, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ONE),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 6, 7, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 0, 0),
#endif /** } **/
};

/* SECAM test01 has the same settings as type 1 */

/* SECAM test02 */
static const uint32_t s_aulItTable_SECAM_Macrovision_Test02[] =
{
#ifdef BVDC_P_WSE_VER3
	BVDC_P_MAKE_IT_ADDR(110, 80, 30, 0, 60, 0, 132),
	BVDC_P_MAKE_IT_STACK(0x53, 0x1f43, 0x78c6, 0xa3, 0x19, 0x78c6, 0, 3),
	BVDC_P_MAKE_IT_EVENT(6, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ONE),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 6, 7, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 0, 0),
#else
	BVDC_P_MAKE_IT_ADDR(0x50, 0x32, 0x14, 0, 0x6e, 0, 0x84),
	BVDC_P_MAKE_IT_STACK(0x53, 0x1f43, 0x78c6, 0xa3, 0x19, 0x78c6, 0, 3),
	BVDC_P_MAKE_IT_EVENT(6, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, LINE_D, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ONE),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 6, 7, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ONE, ZERO, ZERO, 0, 0, 0, 0),
#endif
};

static const uint32_t  s_aulItTable_576p_Macrovision[] =
{
	BVDC_P_MAKE_IT_ADDR(0x40, 0x2a, 0x14, 0, 0xfd, 0xfd, 0xfd),
	BVDC_P_MAKE_IT_STACK(0, 0, 0, 0, 0, 0, 0, 0),
	BVDC_P_MAKE_IT_EVENT(0, 0, 0, 0, 0, 6),
	BVDC_P_MAKE_IT_PCL_0(3, ZERO, ZERO, LINE_A, LINE_B, LINE_C, 0, 3, 0, 2),
	BVDC_P_MAKE_IT_PCL_1(LINE_C, ZERO, ZERO),
	BVDC_P_MAKE_IT_PCL_2_3(ENABLED, 5, 5, ENABLED, ENABLED, 4, 5, 0),
	BVDC_P_MAKE_IT_PCL_4(ZERO, LINE_A, 0, 3, 0, 3, ZERO, ZERO, ZERO, 0, 0, 3, 0),
};

static const uint32_t* s_apulItTable_NTSC_MacrovisionTbl[] =
{
	NULL,                                 /* BVDC_MacrovisionType_eNoProtection */
	s_aulItTable_NTSC_Macrovision_1,      /* BVDC_MacrovisionType_eAgcOnly */
	s_aulItTable_NTSC_Macrovision_2,      /* BVDC_MacrovisionType_eAgc2Lines */
	s_aulItTable_NTSC_Macrovision_3,      /* BVDC_MacrovisionType_eAgc4Lines */
	s_aulItTable_NTSC_Macrovision_1,      /* BVDC_MacrovisionType_eCustomized, dummy */
	s_aulItTable_NTSC_Macrovision_1,      /* BVDC_MacrovisionType_eAgcOnly_Rgb */
	s_aulItTable_NTSC_Macrovision_2,      /* BVDC_MacrovisionType_eAgc2Lines_Rgb */
	s_aulItTable_NTSC_Macrovision_3,      /* BVDC_MacrovisionType_eAgc4Lines_Rgb */
	s_aulItTable_NTSC_Macrovision_Test01, /* BVDC_MacrovisionType_eTest01 */
	s_aulItTable_NTSC_Macrovision_Test02  /* BVDC_MacrovisionType_eTest02 */
};

static const uint32_t* s_apulItTable_PAL_M_MacrovisionTbl[] =
{
	NULL,                                 /* BVDC_MacrovisionType_eNoProtection */
	s_aulItTable_PAL_M_Macrovision_1,      /* BVDC_MacrovisionType_eAgcOnly */
	s_aulItTable_PAL_M_Macrovision_2,      /* BVDC_MacrovisionType_eAgc2Lines */
	s_aulItTable_PAL_M_Macrovision_3,      /* BVDC_MacrovisionType_eAgc4Lines */
	s_aulItTable_PAL_M_Macrovision_1,      /* BVDC_MacrovisionType_eCustomized, dummy */
	s_aulItTable_PAL_M_Macrovision_1,      /* BVDC_MacrovisionType_eAgcOnly_Rgb */
	s_aulItTable_PAL_M_Macrovision_2,      /* BVDC_MacrovisionType_eAgc2Lines_Rgb */
	s_aulItTable_PAL_M_Macrovision_3,      /* BVDC_MacrovisionType_eAgc4Lines_Rgb */
	s_aulItTable_PAL_M_Macrovision_Test01, /* BVDC_MacrovisionType_eTest01 */
	s_aulItTable_PAL_M_Macrovision_Test02  /* BVDC_MacrovisionType_eTest02 */
};

static const uint32_t* s_apulItTable_PAL_MacrovisionTbl[] =
{
	NULL,                                /* BVDC_MacrovisionType_eNoProtection */
	s_aulItTable_PAL_Macrovision_Type_1, /* BVDC_MacrovisionType_eType1 */
	s_aulItTable_PAL_Macrovision_Type_2, /* BVDC_MacrovisionType_eType2 */
	s_aulItTable_PAL_Macrovision_Type_3, /* BVDC_MacrovisionType_eType3 */
	s_aulItTable_PAL_Macrovision_Type_1, /* BVDC_MacrovisionType_eCustomized, dummy */
	s_aulItTable_PAL_Macrovision_Type_1, /* BVDC_MacrovisionType_eType1_Rgb */
	s_aulItTable_PAL_Macrovision_Type_2, /* BVDC_MacrovisionType_eType2_Rgb */
	s_aulItTable_PAL_Macrovision_Type_3, /* BVDC_MacrovisionType_eType3_Rgb */
	s_aulItTable_PAL_Macrovision_Test01, /* BVDC_MacrovisionType_eTest01 */
	s_aulItTable_PAL_Macrovision_Test02  /* BVDC_MacrovisionType_eTest02 */
};

static const uint32_t* s_apulItTable_SECAM_MacrovisionTbl[] =
{
	NULL,                                  /* BVDC_MacrovisionType_eNoProtection */
	s_aulItTable_SECAM_Macrovision,        /* BVDC_MacrovisionType_eAgcOnly */
	s_aulItTable_SECAM_Macrovision,        /* dummy */
	s_aulItTable_SECAM_Macrovision,        /* dummy */
	s_aulItTable_SECAM_Macrovision,        /* BVDC_MacrovisionType_eCustomized, dummy */
	s_aulItTable_SECAM_Macrovision,        /* BVDC_MacrovisionType_eAgcOnly_Rgb */
	s_aulItTable_SECAM_Macrovision,        /* dummy */
	s_aulItTable_SECAM_Macrovision,        /* dummy */
	s_aulItTable_SECAM_Macrovision,        /* BVDC_MacrovisionType_eTest01 */
	s_aulItTable_SECAM_Macrovision_Test02  /* BVDC_MacrovisionType_eTest02 */
};

/* 480i IT_TG_CONFIG value with Macrovision enabled */
static const uint32_t g_ulItConfig_NTSC_Macrovision = BVDC_P_MAKE_IT_TG(1, 0x7f, 0);
static const uint32_t g_ulItConfig_PALM_Macrovision = BVDC_P_MAKE_IT_TG(3, 0x7f, 0);
/* PAL macrovision no color stripe (for type_1, type_2, type_3) TG_CONFIG */
static const uint32_t g_ulItConfig_PAL_Macrovision_NoCs = BVDC_P_MAKE_IT_TG(3, 0x4f, 0);
/* PAL macrovision with color stripe (for test_01, test_02) TG_CONFIG */
static const uint32_t g_ulItConfig_PAL_Macrovision_Cs = BVDC_P_MAKE_IT_TG(3, 0x7f, 0);
/* 480p IT_TG_CONFIG value with Macrovision enabled */
static const uint32_t g_ulItConfig_480p_Macrovision = BVDC_P_MAKE_IT_TG(1, 0xf, 0);

/* SD RGB VF_FORMAT_ADDER with MV on RGB */
static const uint32_t  s_ulFmtAdder_SD_RGB_Macrovision =
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, reserved0,          0) |
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, SECOND_NEGATIVE_SYNC, 1) |
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, CLAMP_MODE,         1) |
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, C2_POSITIVESYNC,    1) |
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, C2_COMP,            1) |
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, C2_OFFSET,          1) |
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, C1_POSITIVESYNC,    1) |
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, C1_COMP,            1) |
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, C1_OFFSET,          1) |
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, C0_POSITIVESYNC,    1) |
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, C0_COMP,            1) |
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, C0_SYNC,            1) |
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, ADD_SYNC_TO_OFFSET, 0) | /* When ADD_SYNC_TO_OFFSET = 0; */
	BCHP_FIELD_DATA(PRIM_VF_FORMAT_ADDER, OFFSET,             0xFB);
	/* 300mV no reduction, OFFSET needs to match with NEG_SYNC_VALUE */


/*************************************************************************
 *  {secret}
 *	Returns pointer to appropriate RamTable for display modes, which
 *  supports Macrovision(480i,480p,Pal). Should never get here otherwise!
 **************************************************************************/
const uint32_t* BVDC_P_GetRamTableMv_isr
(
	BVDC_P_VecPath               eVecPath,
	const BVDC_P_DisplayInfo     *pDispInfo,
	bool                         bArib480p
)
{
	const uint32_t *pRamTbl = NULL;
	BSTD_UNUSED (eVecPath);

	/* TODO: add Test01/Test02 types support */
	switch(pDispInfo->pFmtInfo->eVideoFmt)
	{
		case BFMT_VideoFmt_eNTSC:
		case BFMT_VideoFmt_eNTSC_J:
		case BFMT_VideoFmt_e720x482_NTSC:
		case BFMT_VideoFmt_e720x482_NTSC_J:
			if(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eTest01)
			{
				if(!pDispInfo->bWidthTrimmed && (true == bArib480p))
					pRamTbl = g_RamBVBInput_NTSCARIB_Macrovision_Test01;
				else
					pRamTbl = g_RamBVBInput_NTSC_Macrovision_Test01;
			}
			else if(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eTest02)
			{
				if(!pDispInfo->bWidthTrimmed && (true == bArib480p))
					pRamTbl = g_RamBVBInput_NTSCARIB_Macrovision_Test02;
				else
					pRamTbl = g_RamBVBInput_NTSC_Macrovision_Test02;
			}
			else
			{
				pRamTbl = BVDC_P_GetRamTable_isr(pDispInfo, bArib480p);
			}
			break;

		case BFMT_VideoFmt_ePAL_B:
		case BFMT_VideoFmt_ePAL_B1:
		case BFMT_VideoFmt_ePAL_D:
		case BFMT_VideoFmt_ePAL_D1:
		case BFMT_VideoFmt_ePAL_G:
		case BFMT_VideoFmt_ePAL_H:
		case BFMT_VideoFmt_ePAL_I:
		case BFMT_VideoFmt_ePAL_K:
		case BFMT_VideoFmt_ePAL_NC:
			if(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eTest01)
			{
				pRamTbl = g_RamBVBInput_PAL_Macrovision_Test01;
			}
			else if(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eTest02)
			{
				pRamTbl = g_RamBVBInput_PAL_Macrovision_Test02;
			}
			else
			{
				pRamTbl = BVDC_P_GetRamTable_isr(pDispInfo, bArib480p);
			}
			break;

		case BFMT_VideoFmt_ePAL_M:
			if(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eTest01)
			{
				pRamTbl = g_RamBVBInput_PALM_Macrovision_Test01;
			}
			else if(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eTest02)
			{
				pRamTbl = g_RamBVBInput_PALM_Macrovision_Test02;
			}
			else
			{
				pRamTbl = BVDC_P_GetRamTable_isr(pDispInfo, bArib480p);
			}
			break;

		case BFMT_VideoFmt_ePAL_N:
			if(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eTest01)
			{
				pRamTbl = g_RamBVBInput_PALN_Macrovision_Test01;
			}
			else if(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eTest02)
			{
				pRamTbl = g_RamBVBInput_PALN_Macrovision_Test02;
			}
			else
			{
				pRamTbl = BVDC_P_GetRamTable_isr(pDispInfo, bArib480p);
			}
			break;

		case BFMT_VideoFmt_eSECAM_L:
		case BFMT_VideoFmt_eSECAM_B:
		case BFMT_VideoFmt_eSECAM_G:
		case BFMT_VideoFmt_eSECAM_D:
		case BFMT_VideoFmt_eSECAM_K:
		case BFMT_VideoFmt_eSECAM_H:
		/* TODO: Do we need to differentiate SECAM variants? */
			pRamTbl =
				 (pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eTest02) ? g_RamBVBInput_SECAM_Macrovision_Test02
				: BVDC_P_GetRamTable_isr(pDispInfo, bArib480p);
			break;

		/* Only supports YPrPb */
		case BFMT_VideoFmt_e480p:
		case BFMT_VideoFmt_e720x483p:
		case BFMT_VideoFmt_e576p_50Hz:
			pRamTbl = BVDC_P_GetRamTable_isr(pDispInfo, bArib480p);
			break;

		default:
			BDBG_ASSERT(false);
	};

	return pRamTbl;
}

/*************************************************************************
 *  {secret}
 *	Returns pointer to appropriate ItTable for display modes, which
 *  supports Macrovision(480i,480p,Pal). Should never get here otherwise!
 **************************************************************************/
const uint32_t* BVDC_P_GetItTableMv_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo
)
{
	const uint32_t *pTable = NULL;

	switch(pDispInfo->pFmtInfo->eVideoFmt)
	{
		case BFMT_VideoFmt_eNTSC:
		case BFMT_VideoFmt_eNTSC_J:
		case BFMT_VideoFmt_e720x482_NTSC:
		case BFMT_VideoFmt_e720x482_NTSC_J:
			pTable =
				(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eNoProtection)
				? BVDC_P_GetItTable_isr(pDispInfo)
				: s_apulItTable_NTSC_MacrovisionTbl[pDispInfo->eMacrovisionType];
			break;
		case BFMT_VideoFmt_ePAL_M:
			pTable =
				(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eNoProtection)
				? BVDC_P_GetItTable_isr(pDispInfo)
				: s_apulItTable_PAL_M_MacrovisionTbl[pDispInfo->eMacrovisionType];
			break;

		case BFMT_VideoFmt_ePAL_B:
		case BFMT_VideoFmt_ePAL_B1:
		case BFMT_VideoFmt_ePAL_D:
		case BFMT_VideoFmt_ePAL_D1:
		case BFMT_VideoFmt_ePAL_G:
		case BFMT_VideoFmt_ePAL_H:
		case BFMT_VideoFmt_ePAL_I:
		case BFMT_VideoFmt_ePAL_K:
		case BFMT_VideoFmt_ePAL_N:
		case BFMT_VideoFmt_ePAL_NC:
			pTable =
				(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eNoProtection)
				? BVDC_P_GetItTable_isr(pDispInfo)
				: s_apulItTable_PAL_MacrovisionTbl[pDispInfo->eMacrovisionType];
			break;

		case BFMT_VideoFmt_eSECAM_L:
		case BFMT_VideoFmt_eSECAM_B:
		case BFMT_VideoFmt_eSECAM_G:
		case BFMT_VideoFmt_eSECAM_D:
		case BFMT_VideoFmt_eSECAM_K:
		case BFMT_VideoFmt_eSECAM_H:
			/* TODO: Do we need to differentiate SECAM variants? */
			pTable =
				(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eNoProtection)
				? BVDC_P_GetItTable_isr(pDispInfo)
				: s_apulItTable_SECAM_MacrovisionTbl[pDispInfo->eMacrovisionType];
			break;

		/* Macrovision standard only supports YPrPb */
		case BFMT_VideoFmt_e480p:
		case BFMT_VideoFmt_e720x483p:
			pTable =
				(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eNoProtection)
				? BVDC_P_GetItTable_isr(pDispInfo)
				: s_aulItTable_480p_Macrovision;
			break;

		case BFMT_VideoFmt_e576p_50Hz:
			pTable =
				(pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eNoProtection)
				? BVDC_P_GetItTable_isr(pDispInfo)
				: s_aulItTable_576p_Macrovision;
			break;
		default:
			BDBG_ASSERT(false);
	};

	return pTable;
}

/*************************************************************************
 *  {secret}
 *	Returns
 **************************************************************************/
uint32_t BVDC_P_GetItConfigMv_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo
)
{
	uint32_t     ulItConfig=0;

	if (pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eNoProtection)
	{
		return BVDC_P_GetItConfig_isr(pDispInfo);
	}

	switch(pDispInfo->pFmtInfo->eVideoFmt)
	{
		case BFMT_VideoFmt_eNTSC:
		case BFMT_VideoFmt_eNTSC_J:
		case BFMT_VideoFmt_e720x482_NTSC:
		case BFMT_VideoFmt_e720x482_NTSC_J:
			ulItConfig = g_ulItConfig_NTSC_Macrovision;
			break;

		case BFMT_VideoFmt_ePAL_M:
			ulItConfig = g_ulItConfig_PALM_Macrovision;
			break;

		case BFMT_VideoFmt_ePAL_B:
		case BFMT_VideoFmt_ePAL_B1:
		case BFMT_VideoFmt_ePAL_D:
		case BFMT_VideoFmt_ePAL_D1:
		case BFMT_VideoFmt_ePAL_G:
		case BFMT_VideoFmt_ePAL_H:
		case BFMT_VideoFmt_ePAL_I:
		case BFMT_VideoFmt_ePAL_K:
		case BFMT_VideoFmt_ePAL_N:
		case BFMT_VideoFmt_ePAL_NC:
			if ((pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eTest01) ||
			    (pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eTest02)  )
				ulItConfig = g_ulItConfig_PAL_Macrovision_Cs;
			else
				ulItConfig = g_ulItConfig_PAL_Macrovision_NoCs;
			break;

		/* Only supports YPrPb, same for 480p and 576p */
		case BFMT_VideoFmt_e480p:
		case BFMT_VideoFmt_e720x483p:
		case BFMT_VideoFmt_e576p_50Hz:
			ulItConfig = g_ulItConfig_480p_Macrovision;
			break;

		case BFMT_VideoFmt_eSECAM_L:
		case BFMT_VideoFmt_eSECAM_B:
		case BFMT_VideoFmt_eSECAM_G:
		case BFMT_VideoFmt_eSECAM_D:
		case BFMT_VideoFmt_eSECAM_K:
		case BFMT_VideoFmt_eSECAM_H:
		/* TODO: Do we need to differentiate SECAM variants? */
			ulItConfig = BVDC_P_GetItConfig_isr(pDispInfo);
			break;
		default:
			BDBG_ASSERT(false);
	};

	return ulItConfig;
}


/*************************************************************************
 *  {secret}
 *  This function is for SD RGB output only.
 *	Returns SEC_VF_FORMAT_ADDER value
 **************************************************************************/
uint32_t BVDC_P_GetFmtAdderValue
(
	BVDC_P_DisplayInfo     *pDispInfo
)
{
	uint32_t ulFmtAdderVal;
	const uint32_t *pTable;

	if(pDispInfo->stN0Bits.bRgb)
	{
		ulFmtAdderVal = s_ulFmtAdder_SD_RGB_Macrovision;
	}
	else
	{
		BVDC_P_GetVfTable_isr(pDispInfo, BVDC_P_Output_eRGB, &pTable, NULL);
		ulFmtAdderVal = pTable[0];
	}
	return ulFmtAdderVal;
}

/*************************************************************************
 *
 *
 */
#ifdef BVDC_P_DISPLAY_DUMP /** { **/

void BVDC_Display_MV_DumpAll_aulVfTable (void)
{
}

void BVDC_Display_MV_DumpAll_aulItTable (void)
{
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_NTSC_Macrovision_Test01",
		s_aulItTable_NTSC_Macrovision_Test01);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_NTSC_Macrovision_Test02",
		s_aulItTable_NTSC_Macrovision_Test02);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_NTSC_Macrovision_1", s_aulItTable_NTSC_Macrovision_1);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_NTSC_Macrovision_2", s_aulItTable_NTSC_Macrovision_2);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_NTSC_Macrovision_3", s_aulItTable_NTSC_Macrovision_3);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_PAL_M_Macrovision_Test01",
		s_aulItTable_PAL_M_Macrovision_Test01);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_PAL_M_Macrovision_Test02",
		s_aulItTable_PAL_M_Macrovision_Test02);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_PAL_M_Macrovision_1", s_aulItTable_PAL_M_Macrovision_1);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_PAL_M_Macrovision_2", s_aulItTable_PAL_M_Macrovision_2);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_PAL_M_Macrovision_3", s_aulItTable_PAL_M_Macrovision_3);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_480p_Macrovision", s_aulItTable_480p_Macrovision);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_PAL_Macrovision_Type_1",
		s_aulItTable_PAL_Macrovision_Type_1);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_PAL_Macrovision_Type_2",
		s_aulItTable_PAL_Macrovision_Type_2);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_PAL_Macrovision_Type_3",
		s_aulItTable_PAL_Macrovision_Type_3);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_PAL_Macrovision_Test01",
		s_aulItTable_PAL_Macrovision_Test01);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_PAL_Macrovision_Test02",
		s_aulItTable_PAL_Macrovision_Test02);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_SECAM_Macrovision", s_aulItTable_SECAM_Macrovision);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_SECAM_Macrovision_Test02",
		s_aulItTable_SECAM_Macrovision_Test02);
	BVDC_P_Display_Dump_aulItTable (
		"s_aulItTable_576p_Macrovision", s_aulItTable_576p_Macrovision);
}

void BVDC_Display_MV_DumpAll_ulItConfig (void)
{
	BVDC_P_Display_Dump_ulItConfig (
		"g_ulItConfig_NTSC_Macrovision", g_ulItConfig_NTSC_Macrovision);
	BVDC_P_Display_Dump_ulItConfig (
		"g_ulItConfig_PALM_Macrovision", g_ulItConfig_PALM_Macrovision);
	BVDC_P_Display_Dump_ulItConfig (
		"g_ulItConfig_PAL_Macrovision_NoCs", g_ulItConfig_PAL_Macrovision_NoCs);
	BVDC_P_Display_Dump_ulItConfig (
		"g_ulItConfig_PAL_Macrovision_Cs", g_ulItConfig_PAL_Macrovision_Cs);
	BVDC_P_Display_Dump_ulItConfig (
		"g_ulItConfig_480p_Macrovision", g_ulItConfig_480p_Macrovision);
}

void BVDC_Display_MV_DumpTables (void)
{
	BVDC_Display_MV_DumpAll_aulVfTable();
	BVDC_Display_MV_DumpAll_aulItTable();
	BVDC_Display_MV_DumpAll_ulItConfig();

	/* Special */
	BKNI_Printf ("//%s %d\n", "s_ulFmtAdder_SD_RGB_Macrovision", 1);
	BKNI_Printf (
		"//%s %08x\n",
		"PRIM_VF_FORMAT_ADDER", s_ulFmtAdder_SD_RGB_Macrovision);
}

#endif /** } BVDC_P_DISPLAY_DUMP **/

/* End of File */
