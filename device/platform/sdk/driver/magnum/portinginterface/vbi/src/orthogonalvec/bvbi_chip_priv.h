/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_chip_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/42 $
 * $brcm_Date: 7/24/12 5:21p $
 *
 * Module Description:
 *     Define features that are hardware-specific. For private use by BVBI
 *     software module.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7445/bvbi_chip_priv.h $
 * 
 * Hydra_Software_Devel/42   7/24/12 5:21p darnstein
 * SW7445-12: trivial port to 7445 chipset.
 * 
 * Hydra_Software_Devel/41   6/26/12 6:53p darnstein
 * SW7563-10: port BVBI to new chipset 7563.
 * 
 * Hydra_Software_Devel/40   4/25/12 6:00p darnstein
 * SW7360-16: support 7360 chipset in VBI software. I cloned the logic for
 * 7358 chipset.
 * 
 * Hydra_Software_Devel/39   4/25/12 5:12p darnstein
 * SW7584-22: support BVBI for 7584 chipset. I cloned the logic for 7231
 * chipset.
 * 
 * Hydra_Software_Devel/38   10/28/11 2:39p darnstein
 * SW7435-14: port to 7435. Same software behavior as for 7425.
 * 
 * Hydra_Software_Devel/37   9/9/11 7:12p darnstein
 * SW7429-15: trivial adaptation to 7429 chipset.
 * 
 * Hydra_Software_Devel/36   6/14/11 2:27p darnstein
 * SWDTV-7525: back out previous check-in.
 * 
 * Hydra_Software_Devel/35   6/13/11 4:30p darnstein
 * SWDTV-7525: trivially add support for 35330 chipset.
 * 
 * Hydra_Software_Devel/34   4/20/11 4:25p darnstein
 * SWBLURAY-23702: 7640 chipset has crossbar VEC.
 * 
 * Hydra_Software_Devel/33   4/4/11 4:19p darnstein
 * SWBLURAY-23702: add support for 7640 chipset.
 * 
 * Hydra_Software_Devel/32   3/24/11 5:25p darnstein
 * SWDTV-6195: Add references to new 35233 chipset.
 * 
 * Hydra_Software_Devel/31   11/23/10 1:55p darnstein
 * SW7552-15: port to 7552 chipset. Same code as for 7358.
 * 
 * Hydra_Software_Devel/30   11/19/10 5:45p darnstein
 * SW7231-3: 7344 chipset only has one ITU-R 656 output.
 * 
 * Hydra_Software_Devel/29   11/11/10 5:19p darnstein
 * SW7344-8: first cut at porting BVBI to 7344.
 * 
 * Hydra_Software_Devel/28   11/9/10 3:49p darnstein
 * SW35125-17: first pass at 35125 compatibility.
 * 
 * Hydra_Software_Devel/27   10/21/10 3:34p darnstein
 * SW35230-16: 35230 chipset is capable of CGMS-B output.
 * 
 * Hydra_Software_Devel/26   10/21/10 3:27p darnstein
 * SW7408-15: 7408 chipset is capable of CGMS-B output.
 * 
 * Hydra_Software_Devel/25   10/21/10 3:20p darnstein
 * SW7340-30: 7340 and 7342 chipsets are capable of CGMS-B output.
 * 
 * Hydra_Software_Devel/24   10/15/10 12:13p darnstein
 * SW7468-24: add support for CGMS-B.
 * 
 * Hydra_Software_Devel/23   10/12/10 6:38p darnstein
 * SW7358-16: initial port to 7358-A0.
 * 
 * Hydra_Software_Devel/22   9/29/10 11:32a vanessah
 * SW7425-32:  more for auto-test
 * 
 * Hydra_Software_Devel/21   7/15/10 6:59p darnstein
 * SW7422-46: very simple updates for 7422 compatibility.
 * 
 * Hydra_Software_Devel/20   6/25/10 3:25p darnstein
 * SW7125-17: 7125 chipset has newer version of CGMSAE cores.
 * 
 * Hydra_Software_Devel/19   5/11/10 1:09p darnstein
 * SW7420-45: 7420-C0 can do TVG-2X waveform.
 * 
 * Hydra_Software_Devel/18   5/3/10 1:54p darnstein
 * SW7468-24: 7125-C0 can output TVG2X waveform.
 * 
 * Hydra_Software_Devel/16   3/1/10 3:16p darnstein
 * SW7420-598: add definition for the new CGMSAE hardware found in 7420-
 * C0.
 * 
 * Hydra_Software_Devel/15   12/22/09 3:35p darnstein
 * SW7408-15: fix merge error in previous check-in.
 * 
 * Hydra_Software_Devel/14   12/22/09 12:29p darnstein
 * SW7408-15: #ifdefs for 7408 chipset.
 * 
 * Hydra_Software_Devel/13   11/24/09 4:34p darnstein
 * SW35230-16: first cut at 35230 support.
 * 
 * Hydra_Software_Devel/12   11/16/09 5:58p darnstein
 * SW7468-24: first step towards support of 7468 chipset.
 * 
 * Hydra_Software_Devel/11   11/16/09 5:04p darnstein
 * SW7408-15: first cut at 7408 support.
 * 
 * Hydra_Software_Devel/10   8/21/09 2:37p darnstein
 * PR47900: port to new 7125 chipset.
 * 
 * Hydra_Software_Devel/9   6/24/09 7:10p darnstein
 * PR53956: 7420-B0 has a new WSE core.
 * 
 * Hydra_Software_Devel/8   6/24/09 5:39p darnstein
 * PR56342: BVBI compiles for 7550 chipset now.
 * 
 * Hydra_Software_Devel/7   6/24/09 4:58p darnstein
 * PR56290: BVBI now compiles for 7342 chipset.
 * 
 * Hydra_Software_Devel/6   6/24/09 4:38p darnstein
 * PR56289: BVBI compiles for 7340 chipset now.
 * 
 * Hydra_Software_Devel/5   6/23/09 7:22p darnstein
 * PR56289: port to 7340 chipset.
 * 
 * Hydra_Software_Devel/4   5/13/09 1:18p darnstein
 * PR53956: 7420-Bx has fixed WSE core.
 * 
 * Hydra_Software_Devel/3   12/11/08 4:22p darnstein
 * PR45819: program VBI_ENC and VEC_CFG cores.
 * 
 * Hydra_Software_Devel/2   12/4/08 6:06p darnstein
 * PR45819: 7420 software will now compile, but not link.
 * 
 * Hydra_Software_Devel/2   12/3/08 7:56p darnstein
 * PR45819: New, more modular form of most BVBI source files.
 * 
 * Hydra_Software_Devel/1   10/3/08 7:08p darnstein
 * PR45819: Chip specific definitions for internal use by BVBI.
 *
 ***************************************************************************/

#ifndef BVBI_CHIP_PRIV_H__
#define BVBI_CHIP_PRIV_H__

/* 
 * Explanation of VEC/VDEC counts and capabilities:
 * BVBI_P_NUM_VDEC:         Number of analog VDECs. Does not include ITU-R 656 
                            input.
 * BVBI_P_NUM_VEC:          Number of full VBI encoders. Does not include 
 *                          ITU-R 656 passthrough output.
 * BVBI_P_NUM_PTVEC:        Number of passthrough or ancillary VBI encoders.
 * BVBI_P_NUM_AMOLE:        Number of AMOL encoder cores, not including 
 * 							ITU-R 656.
 * BVBI_P_NUM_AMOLE_656:    Number of AMOL encoder cores that are specifically
 * 							ITU-R 656.
 * BVBI_P_NUM_CCE:          Number of closed caption encoder cores, not 
 * 							including ITU-R 656.
 * BVBI_P_NUM_CCE_656:		Number of closed caption encoder cores that are
 * 							specifically for ITU-R 656 output.
 * BVBI_P_NUM_CGMSAE:       Number of CGMS encoder cores, not including ITU-R
 * 							656.
 * BVBI_P_NUM_CGMSAE_656:	Number of CGMS encoder cores that are specifically 
 * 							for ITU-R 656 output.
 * BVBI_P_NUM_GSE:          Number of Gemstar encoder cores, not including 
 * 							ITU-R 656.
 * BVBI_P_NUM_GSE_656:		Number of Gemstar encoder cores that are 
 * 							specifically for ITU-R 656 output.
 * BVBI_P_NUM_SCTEE:        Number of SCTE encoder cores. Assume first core
 *                          that has one is on the primary VEC path.
 * BVBI_P_NUM_SCTEE_656:    Number of 656 SCTE encoder cores. At present time,
 *                          always zero.
 * BVBI_P_NUM_TTE:          Number of teletext encoder cores. Assume first core
 *                          that has one is on the primary VEC path. Does not
 *                          include 656 (bypass) cores.
 * BVBI_P_NUM_TTE_656:      Number of 656 (bypass) teletext encoder cores. 
 * 							Assume first core that has one is on the primary 
 * 							VEC path.
 * BVBI_P_NUM_WSE:          Number of WSE cores, not including ITU-R 656.
 * BVBI_P_NUM_WSE_656:		Number of WSE cores that are specifically for 
 * 							ITU-R 656 output.
 * BVBI_P_NUM_IN656:        Number of ITU-R 656 inputs (IN656 cores)
 * BVBI_P_NUM_ANCI656_656:  Number of ANCI656_656 or ANCI656_Ancil VEC cores.
 * BVBI_P_HAS_GSD:          VDEC(s) have Gemstar decoder(s).
 * BVBI_P_HAS_VPSD:         VDEC(s) have VPS decoder(s).
 * BVBI_P_HAS_WSE_PARITY    VEC(s) have WSS parity bit generation capability.
 * BVBI_P_HAS_SCTEE_CO      SCTE encoder has component only registers.
 * BVBI_P_HAS_COMPON_INPUT: VDEC(s) have component intput capability.
 * BVBI_P_HAS_XSER_TT:      TTX core has serial output capability.
 * BVBI_P_HAS_CROSSBAR_VEC: VEC has crossbar architecture, first appearing in
 *                          7420-A0.
 * BVBI_P_TTXADR_WAROUND:   TTX core has the PR22720 bug in accessing DRAM. A 
 *                          software fix is provided.
 * BVBI_P_CGMSAE_VER2:      CGMSAE core is version first appearing in 3548-A0.
 *                          Capable of CGMS-B output.
 * BVBI_P_CGMSAE_VER3:      CGMSAE core is version first appearing in 3548-B0.
 *                          Capable of CEA-805-D style output.
 * BVBI_P_CGMSAE_VER4:      CGMSAE core is version first appearing in 7420-A0.
 *                          The BIT_ORDER bitfields were removed.
 * BVBI_P_CGMSAE_VER5:      CGMSAE core is version first appearing in 7420-B0.
 *                          The BIT_ORDER bitfields were restored.
 * BVBI_P_WSE_VER2:         WSE core is version first appearing in 3548-A0.
 *                          ITU-R 656 output is handled in a different way.
 * BVBI_P_WSE_VER3:         WSE core is version first appearing in 7601-A0.
 *                          Capable of IEC-62375 output on 576P video.
 * BVBI_P_WSE_VER4:         WSE core is version first appearing in 3548-B2.
 * 							Has a bug fix related to WSS and VPS output.
 * BVBI_P_WSE_VER5:         WSE core is version first appearing in 7420-B0.
 *                          Register file is identical to VER3 cores.
 * BVBI_P_WSSD_VER2:		WSSD core with bug fixes, first appearing in
 * 							3548-B0.
 * BVBI_P_GSE_VER2:         GSE core is version first appearing in 7408-A0.
 *                          Capable of TVGX2 output.
 * BVBI_P_CCE_VER2:         CCE core version first appearing in 7422-A0. Has
 *                          capability to support subset of SCTE-20 and 
 *                          SCTE-21 specs.
 * BVBI_P_TTD_SCB2_ERROR    TTD core writes its captured data to wrong
 *                          locations in DRAM.
 * BVBI_P_ENC_NUM_CROSSBAR_REG 
 *                          Number of analog paths through the VBI_ENC
 *                          crossbar that are available for use by BVBI
 *                          software. Two are reserved for use by BVDC.
 * BVBI_P_ENC_NUM_CROSSBAR_REG_656 
 *                          Number of ITU-R 656 paths through the VBI_ENC
 *                          crossbar that are available for use by BVBI
 *                          software. Two are reserved for use by BVDC.
 * BVDC_P_3DCOMB_TTD_CONFLICT 
 *                          Frame comb (3D comb) filter in VDEC interferes
 *                          with operation of teletext decoder (TTD).
 */

#if (BCHP_CHIP==7420)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 3
	#define BVBI_P_NUM_AMOLE 3
	#define BVBI_P_NUM_CCE 3
	#define BVBI_P_NUM_CGMSAE 3
	#define BVBI_P_NUM_GSE 3
	#define BVBI_P_NUM_AMOLE_656 1
	#define BVBI_P_NUM_CCE_656 1
	#define BVBI_P_NUM_CGMSAE_656 1
	#define BVBI_P_NUM_GSE_656 1
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 1
	#define BVBI_P_NUM_SCTEE 2
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 3
	#define BVBI_P_NUM_TTE_656 1
	#define BVBI_P_NUM_WSE 3
	#define BVBI_P_NUM_WSE_656 1
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 1
	#define BVBI_P_HAS_XSER_TT 1
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 6
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 5
	#define BVBI_P_NUM_PTVEC 1
	#if (BCHP_VER >= BCHP_VER_B0)
		#define BVBI_P_CGMSAE_VER5 1
		#define BVBI_P_WSE_VER5 1
	#else
		#define BVBI_P_CGMSAE_VER4 1
		#define BVBI_P_WSE_VER3 1
	#endif
	#if (BCHP_VER >= BCHP_VER_C0)
		#define BVBI_P_GSE_VER2 1
	#endif

#elif (BCHP_CHIP==7422) ||(BCHP_CHIP==7425) || (BCHP_CHIP==7435)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_AMOLE 2
	#define BVBI_P_NUM_CCE 2
	#define BVBI_P_NUM_CGMSAE 2
	#define BVBI_P_NUM_GSE 2
	#define BVBI_P_NUM_AMOLE_656 2
	#define BVBI_P_NUM_CCE_656 2
	#define BVBI_P_NUM_CGMSAE_656 2
	#define BVBI_P_NUM_GSE_656 2
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 2
	#define BVBI_P_NUM_SCTEE 0
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 2
	#define BVBI_P_NUM_TTE_656 2
	#define BVBI_P_NUM_WSE 2
	#define BVBI_P_NUM_WSE_656 2
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 1
	#define BVBI_P_HAS_XSER_TT 1
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 6
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 5
	#define BVBI_P_NUM_PTVEC 2
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER5 1
	#define BVBI_P_GSE_VER2 1
	#define BVBI_P_CCE_VER2 1

#elif (BCHP_CHIP==7445)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_AMOLE 2
	#define BVBI_P_NUM_CCE 2
	#define BVBI_P_NUM_CGMSAE 2
	#define BVBI_P_NUM_GSE 2
	#define BVBI_P_NUM_AMOLE_656 1
	#define BVBI_P_NUM_CCE_656 1
	#define BVBI_P_NUM_CGMSAE_656 1
	#define BVBI_P_NUM_GSE_656 1
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 1
	#define BVBI_P_NUM_SCTEE 0
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 2
	#define BVBI_P_NUM_TTE_656 1
	#define BVBI_P_NUM_WSE 2
	#define BVBI_P_NUM_WSE_656 1
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 1
	#define BVBI_P_HAS_XSER_TT 1
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 6
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 5
	#define BVBI_P_NUM_PTVEC 1
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER5 1
	#define BVBI_P_GSE_VER2 1
	#define BVBI_P_CCE_VER2 1

#elif (BCHP_CHIP==7344) ||(BCHP_CHIP==7346) || (BCHP_CHIP==7231) || \
      (BCHP_CHIP == 7429) || (BCHP_CHIP == 7584)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_AMOLE 2
	#define BVBI_P_NUM_CCE 2
	#define BVBI_P_NUM_CGMSAE 2
	#define BVBI_P_NUM_GSE 2
	#define BVBI_P_NUM_AMOLE_656 1
	#define BVBI_P_NUM_CCE_656 1
	#define BVBI_P_NUM_CGMSAE_656 2
	#define BVBI_P_NUM_GSE_656 1
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 1
	#define BVBI_P_NUM_SCTEE 0
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 2
	#define BVBI_P_NUM_TTE_656 1
	#define BVBI_P_NUM_WSE 2
	#define BVBI_P_NUM_WSE_656 1
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 1
	#define BVBI_P_HAS_XSER_TT 1
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 6
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 5
	#define BVBI_P_NUM_PTVEC 1
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER5 1
	#define BVBI_P_GSE_VER2 1
	#define BVBI_P_CCE_VER2 1

#elif (BCHP_CHIP==7340)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_AMOLE 2
	#define BVBI_P_NUM_CCE 2
	#define BVBI_P_NUM_CGMSAE 2
	#define BVBI_P_NUM_GSE 2
	#define BVBI_P_NUM_AMOLE_656 0
	#define BVBI_P_NUM_CCE_656 0
	#define BVBI_P_NUM_CGMSAE_656 0
	#define BVBI_P_NUM_GSE_656 0
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 0
	#define BVBI_P_NUM_SCTEE 1
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 2
	#define BVBI_P_NUM_TTE_656 0
	#define BVBI_P_NUM_WSE 2
	#define BVBI_P_NUM_WSE_656 0
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 0
	#define BVBI_P_HAS_XSER_TT 0
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 6
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 5
	#define BVBI_P_NUM_PTVEC 0
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER3 1

#elif (BCHP_CHIP==7342)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_AMOLE 2
	#define BVBI_P_NUM_CCE 2
	#define BVBI_P_NUM_CGMSAE 2
	#define BVBI_P_NUM_GSE 2
	#define BVBI_P_NUM_AMOLE_656 1
	#define BVBI_P_NUM_CCE_656 1
	#define BVBI_P_NUM_CGMSAE_656 1
	#define BVBI_P_NUM_GSE_656 1
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 1
	#define BVBI_P_NUM_SCTEE 0
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 2
	#define BVBI_P_NUM_TTE_656 1
	#define BVBI_P_NUM_WSE 2
	#define BVBI_P_NUM_WSE_656 1
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 0
	#define BVBI_P_HAS_XSER_TT 1
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 6
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 5
	#define BVBI_P_NUM_PTVEC 1
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER3 1

#elif (BCHP_CHIP==7358) || (BCHP_CHIP==7552) || (BCHP_CHIP==7360)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_AMOLE 2
	#define BVBI_P_NUM_CCE 2
	#define BVBI_P_NUM_CGMSAE 2
	#define BVBI_P_NUM_GSE 2
	#define BVBI_P_NUM_AMOLE_656 0
	#define BVBI_P_NUM_CCE_656 0
	#define BVBI_P_NUM_CGMSAE_656 0
	#define BVBI_P_NUM_GSE_656 0
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 0
	#define BVBI_P_NUM_SCTEE 0
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 2
	#define BVBI_P_NUM_TTE_656 0
	#define BVBI_P_NUM_WSE 2
	#define BVBI_P_NUM_WSE_656 0
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 0
	#define BVBI_P_HAS_XSER_TT 0
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 6
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 0
	#define BVBI_P_NUM_PTVEC 0
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER5 1
	#define BVBI_P_GSE_VER2 1
	#define BVBI_P_CCE_VER2 1

#elif (BCHP_CHIP==7563)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 1
	#define BVBI_P_NUM_AMOLE 1
	#define BVBI_P_NUM_CCE 1
	#define BVBI_P_NUM_CGMSAE 1
	#define BVBI_P_NUM_GSE 1
	#define BVBI_P_NUM_AMOLE_656 0
	#define BVBI_P_NUM_CCE_656 0
	#define BVBI_P_NUM_CGMSAE_656 0
	#define BVBI_P_NUM_GSE_656 0
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 0
	#define BVBI_P_NUM_SCTEE 0
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 1
	#define BVBI_P_NUM_TTE_656 0
	#define BVBI_P_NUM_WSE 1
	#define BVBI_P_NUM_WSE_656 0
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 0
	#define BVBI_P_HAS_XSER_TT 0
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 6
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 0
	#define BVBI_P_NUM_PTVEC 0
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER5 1
	#define BVBI_P_GSE_VER2 1
	#define BVBI_P_CCE_VER2 1

#elif (BCHP_CHIP==7550)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_AMOLE 2
	#define BVBI_P_NUM_CCE 2
	#define BVBI_P_NUM_CGMSAE 2
	#define BVBI_P_NUM_GSE 2
	#define BVBI_P_NUM_AMOLE_656 0
	#define BVBI_P_NUM_CCE_656 0
	#define BVBI_P_NUM_CGMSAE_656 0
	#define BVBI_P_NUM_GSE_656 0
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 0
	#define BVBI_P_NUM_SCTEE 0
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 0
	#define BVBI_P_NUM_TTE_656 0
	#define BVBI_P_NUM_WSE 2
	#define BVBI_P_NUM_WSE_656 0
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 0
	#define BVBI_P_HAS_XSER_TT 0
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 6
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 5
	#define BVBI_P_NUM_PTVEC 0
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER3 1

#elif (BCHP_CHIP==7125)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_AMOLE 2
	#define BVBI_P_NUM_CCE 2
	#define BVBI_P_NUM_CGMSAE 2
	#define BVBI_P_NUM_GSE 2
	#define BVBI_P_NUM_AMOLE_656 0
	#define BVBI_P_NUM_CCE_656 0
	#define BVBI_P_NUM_CGMSAE_656 0
	#define BVBI_P_NUM_GSE_656 0
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 0
	#define BVBI_P_NUM_SCTEE 1
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 2
	#define BVBI_P_NUM_TTE_656 0
	#define BVBI_P_NUM_WSE 2
	#define BVBI_P_NUM_WSE_656 0
	#define BVBI_P_NUM_IN656 1
	#define BVBI_P_HAS_EXT_656 1
	#define BVBI_P_HAS_XSER_TT 0
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 3
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 0
	#define BVBI_P_NUM_PTVEC 0
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER5 1
	#if (BCHP_VER >= BCHP_VER_C0)
		#define BVBI_P_GSE_VER2 1
	#endif

#elif (BCHP_CHIP==7408)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_AMOLE 1
	#define BVBI_P_NUM_CCE 1
	#define BVBI_P_NUM_CGMSAE 2
	#define BVBI_P_NUM_GSE 1
	#define BVBI_P_NUM_AMOLE_656 0
	#define BVBI_P_NUM_CCE_656 0
	#define BVBI_P_NUM_CGMSAE_656 0
	#define BVBI_P_NUM_GSE_656 0
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 0
	#define BVBI_P_NUM_SCTEE 0
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 0
	#define BVBI_P_NUM_TTE_656 0
	#define BVBI_P_NUM_WSE 2
	#define BVBI_P_NUM_WSE_656 0
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 0
	#define BVBI_P_HAS_XSER_TT 0
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 6
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 0
	#define BVBI_P_NUM_PTVEC 0
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER5 1

#elif (BCHP_CHIP==7468)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_AMOLE 1
	#define BVBI_P_NUM_CCE 1
	#define BVBI_P_NUM_CGMSAE 2
	#define BVBI_P_NUM_GSE 1
	#define BVBI_P_NUM_AMOLE_656 0
	#define BVBI_P_NUM_CCE_656 0
	#define BVBI_P_NUM_CGMSAE_656 0
	#define BVBI_P_NUM_GSE_656 0
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 0
	#define BVBI_P_NUM_SCTEE 0
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 1
	#define BVBI_P_NUM_TTE_656 0
	#define BVBI_P_NUM_WSE 2
	#define BVBI_P_NUM_WSE_656 0
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 0
	#define BVBI_P_HAS_XSER_TT 0
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 6
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 0
	#define BVBI_P_NUM_PTVEC 0
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER5 1
	#define BVBI_P_GSE_VER2 1

#elif (BCHP_CHIP==35230)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 1
	#define BVBI_P_NUM_AMOLE 0
	#define BVBI_P_NUM_CCE 1
	#define BVBI_P_NUM_CGMSAE 1
	#define BVBI_P_NUM_GSE 0
	#define BVBI_P_NUM_AMOLE_656 0
	#define BVBI_P_NUM_CCE_656 0
	#define BVBI_P_NUM_CGMSAE_656 0
	#define BVBI_P_NUM_GSE_656 0
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 0
	#define BVBI_P_NUM_SCTEE 0
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 1
	#define BVBI_P_NUM_TTE_656 0
	#define BVBI_P_NUM_WSE 1
	#define BVBI_P_NUM_WSE_656 0
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 0
	#define BVBI_P_HAS_XSER_TT 0
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 3
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 0
	#define BVBI_P_NUM_PTVEC 0
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER3 1

#elif (BCHP_CHIP==35233)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 1
	#define BVBI_P_NUM_AMOLE 0
	#define BVBI_P_NUM_CCE 1
	#define BVBI_P_NUM_CGMSAE 1
	#define BVBI_P_NUM_GSE 0
	#define BVBI_P_NUM_AMOLE_656 0
	#define BVBI_P_NUM_CCE_656 0
	#define BVBI_P_NUM_CGMSAE_656 0
	#define BVBI_P_NUM_GSE_656 0
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 0
	#define BVBI_P_NUM_SCTEE 0
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 1
	#define BVBI_P_NUM_TTE_656 0
	#define BVBI_P_NUM_WSE 1
	#define BVBI_P_NUM_WSE_656 0
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 0
	#define BVBI_P_HAS_XSER_TT 0
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 4
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 0
	#define BVBI_P_NUM_PTVEC 0
	#define BVBI_P_CCE_VER2 1
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER3 1

#elif (BCHP_CHIP==35125)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 1
	#define BVBI_P_NUM_AMOLE 0
	#define BVBI_P_NUM_CCE 1
	#define BVBI_P_NUM_CGMSAE 1
	#define BVBI_P_NUM_GSE 0
	#define BVBI_P_NUM_AMOLE_656 0
	#define BVBI_P_NUM_CCE_656 0
	#define BVBI_P_NUM_CGMSAE_656 0
	#define BVBI_P_NUM_GSE_656 0
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 0
	#define BVBI_P_NUM_SCTEE 0
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 1
	#define BVBI_P_NUM_TTE_656 0
	#define BVBI_P_NUM_WSE 1
	#define BVBI_P_NUM_WSE_656 0
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_HAS_EXT_656 0
	#define BVBI_P_HAS_XSER_TT 0
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 3
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 0
	#define BVBI_P_NUM_PTVEC 0
	#define BVBI_P_CGMSAE_VER5 1
	#define BVBI_P_WSE_VER3 1

#elif (BCHP_CHIP == 7640)
	#define BVBI_P_NUM_VEC 2
    #define BVBI_P_NUM_VDEC 0
    #define BVBI_P_NUM_IN656 1
	#define BVBI_P_HAS_CROSSBAR_VEC 1
	#define BVBI_P_ENC_NUM_CROSSBAR_REG 5
	#define BVBI_P_ENC_NUM_CROSSBAR_REG_656 0
	#define BVBI_P_NUM_ANCI656_656 0
    #define BVBI_P_NUM_PTVEC 0
    #define BVBI_P_NUM_AMOLE 0
    #define BVBI_P_NUM_AMOLE_656 0
    #define BVBI_P_NUM_CCE 2
    #define BVBI_P_NUM_CCE_656 0
    #define BVBI_P_NUM_GSE 2
    #define BVBI_P_NUM_GSE_656 0
    #define BVBI_P_NUM_CGMSAE 2
    #define BVBI_P_NUM_CGMSAE_656 0
    #define BVBI_P_NUM_SCTEE 0
    #define BVBI_P_NUM_SCTEE_656 0
    #define BVBI_P_NUM_TTE 2
    #define BVBI_P_NUM_TTE_656 0
    #define BVBI_P_NUM_WSE 2
    #define BVBI_P_NUM_WSE_656 0
    #define BVBI_P_NEWER_SOFT_RESET 1
    #define BVBI_P_CGMSAE_VER5 1
    #define BVBI_P_WSE_VER5 1
	#define BVBI_P_GSE_VER2 1
	#define BVBI_P_CCE_VER2 1
    #define BVBI_P_HAS_FE_BE 1
	#define BVBI_P_HAS_XSER_TT 0

#else
	#error Unknown video chip name
#endif

/* I should have started these series in a different way */
#if !defined(BVBI_P_CGMSAE_VER2) && !defined(BVBI_P_CGMSAE_VER3) && \
	!defined(BVBI_P_CGMSAE_VER4) && !defined(BVBI_P_CGMSAE_VER5)
		#define BVBI_P_CGMSAE_VER1 1
#endif
#if !defined(BVBI_P_WSE_VER2) && !defined(BVBI_P_WSE_VER3) && \
    !defined(BVBI_P_WSE_VER4) && !defined(BVBI_P_WSE_VER5)
	#define BVBI_P_WSE_VER1 1
#endif
#if !defined(BVBI_P_WSSD_VER2)
	#define BVBI_P_WSSD_VER1 1
#endif
#if !defined(BVBI_P_GSE_VER2)
	#define BVBI_P_GSE_VER1 1
#endif
#if !defined(BVBI_P_CCE_VER2)
	#define BVBI_P_CCE_VER1 1
#endif

#endif /* BVBI_CHIP_PRIV_H__ */
