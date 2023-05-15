/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_chip_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 1/28/10 5:12p $
 *
 * Module Description:
 *     Define features that are hardware-specific. For private use by BVBI
 *     software module.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7335/bvbi_chip_priv.h $
 * 
 * Hydra_Software_Devel/6   1/28/10 5:12p darnstein
 * SW7335-666: For 7335-B0, account for updated CGMSAE core.
 * 
 * Hydra_Software_Devel/5   10/26/09 4:16p darnstein
 * AR35230-15,HW7403-261,SW7325-489: handle TTE bitmask the same way on
 * 7325 and 7335: assume the problem with large number of teletext lines
 * is in effect. Do not swap bytes on any system, little endian or big
 * endian.
 * 
 * Hydra_Software_Devel/4   1/31/09 1:01a jrubio
 * PR51629: add 7336 support
 * 
 * Hydra_Software_Devel/3   1/20/09 12:12p darnstein
 * PR51074: For the problem with 15 lines or more teletext (output), apply
 * the same software workaround that was used on 7401-C0. Note that this
 * workaround itself causes problems for some chipsets, such as 3563-C0.
 * So the workaround is controlled by an #ifdef.
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
 * BVBI_P_TTXADR_WAROUND:   TTX core has the PR22720 bug in accessing DRAM. A 
 *                          software fix is provided.
 * BVBI_P_CGMSAE_VER2:      CGMSAE core is version first appearing in 3548-A0.
 *                          Capable of CGMS-B output.
 * BVBI_P_CGMSAE_VER3:      CGMSAE core is version first appearing in 3548-B0.
 *                          Capable of CEA-805-D style output.
 * BVBI_P_WSE_VER2:         WSE core is version first appearing in 3548-A0.
 *                          ITU-R 656 output is handled in a different way.
 * BVBI_P_WSE_VER3:         WSE core is version first appearing in 7601-A0.
 *                          Capable of IEC-62375 output on 576P video.
 * BVBI_P_WSSD_VER2:		WSSD core with bug fixes, first appearing in
 * 							3548-B0.
 * BVBI_P_TTD_SCB2_ERROR    TTD core writes its captured data to wrong
 *                          locations in DRAM.
 * BVBI_P_TTE_WA15          TTE core has trouble with 15 lines or more of
 *                          teletext data per field. Workaround involves
 *                          wasting some DRAM.
 * BVDC_P_3DCOMB_TTD_CONFLICT Frame comb (3D comb) filter in VDEC interferes
 *                          with operation of teletext decoder (TTD).
 */

#if (BCHP_CHIP==7325)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_PTVEC 1
	#define BVBI_P_NUM_AMOLE 1
	#define BVBI_P_NUM_AMOLE_656 1
	#define BVBI_P_NUM_CCE 2
	#define BVBI_P_NUM_CCE_656 1
	#define BVBI_P_NUM_CGMSAE 2
	#define BVBI_P_NUM_CGMSAE_656 1
	#define BVBI_P_NUM_GSE 2
	#define BVBI_P_NUM_GSE_656 0
	#define BVBI_P_NUM_SCTEE 1
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 2
	#define BVBI_P_NUM_TTE_656 1
	#define BVBI_P_NUM_WSE 2
	#define BVBI_P_NUM_WSE_656 1
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_NUM_ANCI656_656 1
	#define BVBI_P_HAS_EXT_656 1
	#define BVBI_P_HAS_XSER_TT 1
	#define BVBI_P_HAS_WSE_PARITY 1
	#if (BCHP_VER >= BCHP_VER_B0)
		#define BVBI_P_CGMSAE_VER2 1
		#define BVBI_P_WSE_VER3 1
	#endif
	#define BVBI_P_TTE_WA15 1
#elif (BCHP_CHIP==7335) ||  (BCHP_CHIP==7336)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_PTVEC 1
	#define BVBI_P_NUM_AMOLE 1
	#define BVBI_P_NUM_AMOLE_656 1
	#define BVBI_P_NUM_CCE 2
	#define BVBI_P_NUM_CCE_656 1
	#define BVBI_P_NUM_CGMSAE 2
	#define BVBI_P_NUM_CGMSAE_656 1
	#define BVBI_P_NUM_GSE 2
	#define BVBI_P_NUM_GSE_656 0
	#define BVBI_P_NUM_SCTEE 1
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 2
	#define BVBI_P_NUM_TTE_656 1
	#define BVBI_P_NUM_WSE 2
	#define BVBI_P_NUM_WSE_656 1
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_NUM_ANCI656_656 1
	#define BVBI_P_HAS_EXT_656 1
	#define BVBI_P_HAS_XSER_TT 1
	#define BVBI_P_HAS_WSE_PARITY 1
	#if (BCHP_VER < BCHP_VER_B0)
		#define BVBI_P_CGMSAE_VER2 1
	#else
		#define BVBI_P_CGMSAE_VER3 1
	#endif
	#define BVBI_P_WSE_VER3 1
	#define BVBI_P_TTE_WA15 1
#elif (BCHP_CHIP==7400)
	/* NOTE: chip revision -A0 is not supported */
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 3
	#define BVBI_P_NUM_AMOLE 3
	#define BVBI_P_NUM_CCE 3
	#define BVBI_P_NUM_CGMSAE 3
	#define BVBI_P_NUM_GSE 3
	#define BVBI_P_NUM_AMOLE_656 1
	#define BVBI_P_NUM_CCE_656 1
	#define BVBI_P_NUM_CGMSAE_656 1
	#define BVBI_P_NUM_GSE_656 0
	#define BVBI_P_HAS_WSE_PARITY 1
	#define BVBI_P_NUM_ANCI656_656 1
	#define BVBI_P_NUM_SCTEE 3
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 3
	#define BVBI_P_NUM_TTE_656 1
	#define BVBI_P_NUM_WSE 3
	#define BVBI_P_NUM_WSE_656 1
	#define BVBI_P_NUM_IN656 2
	#define BVBI_P_HAS_EXT_656 1
	#define BVBI_P_HAS_XSER_TT 1
	#define BVBI_P_NUM_PTVEC 1
	#if (BCHP_VER >= BCHP_VER_E0)
		#define BVBI_P_CGMSAE_VER3 1
	#endif
#elif (BCHP_CHIP==7405)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_PTVEC 1
	#define BVBI_P_NUM_AMOLE 1
	#define BVBI_P_NUM_AMOLE_656 1
	#define BVBI_P_NUM_CCE 2
	#define BVBI_P_NUM_CCE_656 1
	#define BVBI_P_NUM_CGMSAE 2
	#define BVBI_P_NUM_CGMSAE_656 1
	#define BVBI_P_NUM_GSE 2
	#define BVBI_P_NUM_GSE_656 1
	#define BVBI_P_NUM_SCTEE 1
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_NUM_TTE 2
	#define BVBI_P_NUM_TTE_656 1
	#define BVBI_P_NUM_WSE 2
	#define BVBI_P_NUM_WSE_656 1
	#define BVBI_P_NUM_IN656 1
	#define BVBI_P_NUM_ANCI656_656 1
	#define BVBI_P_HAS_EXT_656 1
	#define BVBI_P_HAS_XSER_TT 1
	#define BVBI_P_HAS_WSE_PARITY 1
#elif (BCHP_CHIP==7601)
	#define BVBI_P_NUM_VDEC 0
	#define BVBI_P_NUM_IN656 0
	#define BVBI_P_NUM_ANCI656_656 0
	#define BVBI_P_NUM_PTVEC 0
	#define BVBI_P_NUM_VEC 2
	#define BVBI_P_NUM_PTVEC 0
	#if (BCHP_VER < BCHP_VER_B0)
		#define BVBI_P_NUM_AMOLE 0
		#define BVBI_P_NUM_AMOLE_656 0
		#define BVBI_P_NUM_CCE 1
		#define BVBI_P_NUM_CCE_656 0
		#define BVBI_P_NUM_CGMSAE 1
		#define BVBI_P_NUM_CGMSAE_656 0
		#define BVBI_P_NUM_GSE 1
		#define BVBI_P_NUM_GSE_656 0
		#define BVBI_P_NUM_TTE 1
		#define BVBI_P_NUM_TTE_656 0
		#define BVBI_P_NUM_WSE 1
		#define BVBI_P_NUM_WSE_656 0
	#else
		#define BVBI_P_NUM_AMOLE 0
		#define BVBI_P_NUM_AMOLE_656 0
		#define BVBI_P_NUM_CCE 2
		#define BVBI_P_NUM_CCE_656 0
		#define BVBI_P_NUM_CGMSAE 2
		#define BVBI_P_NUM_CGMSAE_656 0
		#define BVBI_P_NUM_GSE 2
		#define BVBI_P_NUM_GSE_656 0
		#define BVBI_P_NUM_TTE 2
		#define BVBI_P_NUM_TTE_656 0
		#define BVBI_P_NUM_WSE 2
		#define BVBI_P_NUM_WSE_656 0
	#endif
	#define BVBI_P_NUM_SCTEE 0
	#define BVBI_P_NUM_SCTEE_656 0
	#define BVBI_P_CGMSAE_VER2 1
	#define BVBI_P_WSE_VER3 1
	#define BVBI_P_HAS_FE_BE 1
	#define BVBI_P_HAS_WSE_PARITY 1
#else
	#error Unknown video chip name
#endif

/* I should have started these series in a different way */
#if !defined(BVBI_P_CGMSAE_VER2) && !defined(BVBI_P_CGMSAE_VER3)
	#define BVBI_P_CGMSAE_VER1 1
#endif
#if !defined(BVBI_P_WSE_VER2) && !defined(BVBI_P_WSE_VER3)
	#define BVBI_P_WSE_VER1 1
#endif
#if !defined(BVBI_P_WSSD_VER2)
	#define BVBI_P_WSSD_VER1 1
#endif

#endif /* BVBI_CHIP_PRIV_H__ */
