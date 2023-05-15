/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * OTP CMD layer abstraction.
 *
 * $brcm_Workfile: bsp_s_otp_common.h $
 * $brcm_Revision: Aegis_Firmware_Devel/Aegis_Zeus_2_0_Bseck_Release/3 $
 * $brcm_Date: 6/4/12 2:05p $
 * Broadcom Security Processor Code
 ***************************************************************************/
 
#ifndef BSP_S_OTP_COMMON_H__
#define BSP_S_OTP_COMMON_H__

typedef enum BCMD_Otp_CmdMsp_e
{
    BCMD_Otp_CmdMsp_eReserved0                              = 0,    /* previously eFlashType*/
	BCMD_Otp_CmdMsp_eReserved1							=	1,
	BCMD_Otp_CmdMsp_eReserved2							=	2,
	BCMD_Otp_CmdMsp_eReserved3							=	3,
	BCMD_Otp_CmdMsp_eReserved4							=	4,
    BCMD_Otp_CmdMsp_eCrLockEnable                           = 5,
    BCMD_Otp_CmdMsp_eCrDisable                              = 6,
	BCMD_Otp_CmdMsp_eReserved7							=	7,
	BCMD_Otp_CmdMsp_eReserved8							=	8,
#if (BCHP_CHIP == 7425)
    BCMD_Otp_CmdMsp_ePcieHostProtect                        = 9,
    BCMD_Otp_CmdMsp_ePcieClientProtect                      = 10,
#else
	BCMD_Otp_CmdMsp_eReserved9							=	9,
	BCMD_Otp_CmdMsp_eReserved10							=	10,
#endif
    BCMD_Otp_CmdMsp_eSecurityPinBoundaryScanProtect         = 11,
    BCMD_Otp_CmdMsp_eGenericPinBoundaryScanProtect          = 12,
	BCMD_Otp_CmdMsp_eReserved13							=	13,
	BCMD_Otp_CmdMsp_eReserved14							=	14,
	BCMD_Otp_CmdMsp_eReserved15							=	15,
    BCMD_Otp_CmdMsp_eSpdifI2sDisable                        = 16,
	BCMD_Otp_CmdMsp_eReserved17							=	17,
    BCMD_Otp_CmdMsp_eForceDramScrambler                     = 18,
	BCMD_Otp_CmdMsp_eReserved19							=	19,
	BCMD_Otp_CmdMsp_eReserved20							=	20,
	BCMD_Otp_CmdMsp_eReserved21							=	21,
	BCMD_Otp_CmdMsp_eReserved22							=	22,
	BCMD_Otp_CmdMsp_eReserved23							=	23,
	BCMD_Otp_CmdMsp_eReserved24							=	24,
	BCMD_Otp_CmdMsp_eReserved25							=	25,
	BCMD_Otp_CmdMsp_eReserved26							=	26,
	BCMD_Otp_CmdMsp_eReserved27							=	27,
	BCMD_Otp_CmdMsp_eReserved28							=	28,
	BCMD_Otp_CmdMsp_eReserved29							=	29,
	BCMD_Otp_CmdMsp_eReserved30							=	30,
	BCMD_Otp_CmdMsp_eReserved31							=	31,
	BCMD_Otp_CmdMsp_eReserved32							=	32,
	BCMD_Otp_CmdMsp_eReserved33							=	33,
	BCMD_Otp_CmdMsp_eReserved34							=	34,
    BCMD_Otp_CmdMsp_eDataSectionLockByte                    = 35,
    BCMD_Otp_CmdMsp_eDataSectionReadProtectBits             = 36,
	BCMD_Otp_CmdMsp_eReserved37							=	37,
	BCMD_Otp_CmdMsp_eReserved38							=	38,
	BCMD_Otp_CmdMsp_eReserved39							=	39,
	BCMD_Otp_CmdMsp_eReserved40							=	40,
	BCMD_Otp_CmdMsp_eReserved41							=	41,
	BCMD_Otp_CmdMsp_eReserved42							=	42,
	BCMD_Otp_CmdMsp_eReserved43							=	43,
	BCMD_Otp_CmdMsp_eReserved44							=	44,
    BCMD_Otp_CmdMsp_eCaControlBits                          = 45,
    BCMD_Otp_CmdMsp_eCpM2MControlBits                       = 46,
    BCMD_Otp_CmdMsp_eCaKeyLadderDisable                     = 47,
    BCMD_Otp_CmdMsp_eCpKeyLadderDisable                     = 48,
    BCMD_Otp_CmdMsp_eCustKeytoCaKeyLadderDisable            = 49,
    BCMD_Otp_CmdMsp_eCustKeytoCpKeyLadderDisable            = 50,
	BCMD_Otp_CmdMsp_eReserved51							=	51,
	BCMD_Otp_CmdMsp_eReserved52							=	52,
    BCMD_Otp_CmdMsp_eCaRsaRootKeyDisable                    = 53,
    BCMD_Otp_CmdMsp_eCpRsaRootKeyDisable                    = 54,
    BCMD_Otp_CmdMsp_eEncryptedK3VerifyEnable                = 55,
	BCMD_Otp_CmdMsp_eReserved56							=	56,
    BCMD_Otp_CmdMsp_e1DesKeyLadderDisable                   = 57,
    BCMD_Otp_CmdMsp_eRouted3DesKeyK1K2CheckEnable           = 58,
    BCMD_Otp_CmdMsp_eCaSoftwareKeyDisable                   = 59,
    BCMD_Otp_CmdMsp_eCpAesEcbMscSoftwareKeyDisable          = 60,
    BCMD_Otp_CmdMsp_eCpM2mDesSoftwareKeyDisable             = 61,
    BCMD_Otp_CmdMsp_eCpM2m3DesSoftwareKeyDisable            = 62,
    BCMD_Otp_CmdMsp_eCpM2mAesSoftwareKeyDisable             = 63,
    BCMD_Otp_CmdMsp_eCpM2mAesCounterSoftwareKeyDisable      = 64,
    BCMD_Otp_CmdMsp_eM2mC2CssSoftwareKeyDisable             = 65,
    BCMD_Otp_CmdMsp_eM2mM6SoftwareKeyDisable                = 66,
	BCMD_Otp_CmdMsp_eReserved67							=	67,
	BCMD_Otp_CmdMsp_eReserved68							=	68,
	BCMD_Otp_CmdMsp_eReserved69							=	69,
	BCMD_Otp_CmdMsp_eReserved70							=	70,
	BCMD_Otp_CmdMsp_eReserved71							=	71,
	BCMD_Otp_CmdMsp_eReserved72							=	72,
    BCMD_Otp_CmdMsp_ePublicKey0Index                        = 73,
    BCMD_Otp_CmdMsp_eSecureBootEnable                       = 74,
	BCMD_Otp_CmdMsp_eReserved75							=	75,
    BCMD_Otp_CmdMsp_eAudioVerifyEnable                      = 76,
    BCMD_Otp_CmdMsp_eVideoVerifyEnable                      = 77,
    BCMD_Otp_CmdMsp_eRaveVerifyEnable                       = 78,
	BCMD_Otp_CmdMsp_eReserved79							=	79,
	BCMD_Otp_CmdMsp_eReserved80							=	80,
    BCMD_Otp_CmdMsp_eHostOutOfResetOnErrorDisable           = 81,
	BCMD_Otp_CmdMsp_eReserved82							=	82,
    BCMD_Otp_CmdMsp_eBootSequenceSelect                     = 83,
    BCMD_Otp_CmdMsp_eForceSha256                            = 84,
    BCMD_Otp_CmdMsp_eKey0PrimeSigningRights                 = 85,
    BCMD_Otp_CmdMsp_eKey0SigningRights                      = 86,
    BCMD_Otp_CmdMsp_eSystemEpoch                            = 87,
	BCMD_Otp_CmdMsp_eReserved88							=	88,
    BCMD_Otp_CmdMsp_eBseckEnable                            = 89,
	BCMD_Otp_CmdMsp_eReserved90							=	90,
	BCMD_Otp_CmdMsp_eReserved91							=	91,
	BCMD_Otp_CmdMsp_eReserved92							=	92,
    BCMD_Otp_CmdMsp_eBseckFwEpoch                           = 93,
    BCMD_Otp_CmdMsp_eMarketId                               = 94,
    BCMD_Otp_CmdMsp_eMpodDisable                            = 95,
	BCMD_Otp_CmdMsp_eReserved96							=	96,
	BCMD_Otp_CmdMsp_eReserved97							=	97,
	BCMD_Otp_CmdMsp_eReserved98							=	98,
	BCMD_Otp_CmdMsp_eReserved99							=	99,
	BCMD_Otp_CmdMsp_eReserved100							=	100,
	BCMD_Otp_CmdMsp_eReserved101							=	101,
	BCMD_Otp_CmdMsp_eReserved102							=	102,
	BCMD_Otp_CmdMsp_eReserved103							=	103,
    BCMD_Otp_CmdMsp_eUserReg                                = 104,
	BCMD_Otp_CmdMsp_eReserved105							=	105,
	BCMD_Otp_CmdMsp_eReserved106							=	106,
	BCMD_Otp_CmdMsp_eReserved107							=	107,
	BCMD_Otp_CmdMsp_eReserved108							=	108,
	BCMD_Otp_CmdMsp_eReserved109							=	109,
	BCMD_Otp_CmdMsp_eReserved110							=	110,
	BCMD_Otp_CmdMsp_eReserved111							=	111,
	BCMD_Otp_CmdMsp_eReserved112							=	112,
	BCMD_Otp_CmdMsp_eReserved113							=	113,
	BCMD_Otp_CmdMsp_eReserved114							=	114,
	BCMD_Otp_CmdMsp_eReserved115							=	115,
	BCMD_Otp_CmdMsp_eReserved116							=	116,
	BCMD_Otp_CmdMsp_eReserved117							=	117,
	BCMD_Otp_CmdMsp_eReserved118							=	118,
	BCMD_Otp_CmdMsp_eReserved119							=	119,
	BCMD_Otp_CmdMsp_eReserved120							=	120,

	BCMD_Otp_CmdMsp_eReserved121							=	121,
	BCMD_Otp_CmdMsp_eReserved122							=	122,
	BCMD_Otp_CmdMsp_eReserved123							=	123,
	BCMD_Otp_CmdMsp_eReserved124							=	124,
	BCMD_Otp_CmdMsp_eReserved125							=	125,
	BCMD_Otp_CmdMsp_eReserved126							=	126,

	BCMD_Otp_CmdMsp_eReserved127							=	127,
	BCMD_Otp_CmdMsp_eReserved128							=	128,
	BCMD_Otp_CmdMsp_eReserved129							=	129,
	BCMD_Otp_CmdMsp_eReserved130							=	130,
#if (BCHP_CHIP == 7425) || (BCHP_CHIP == 7435)
    BCMD_Otp_CmdMsp_eViceVerifyEnable                       = 131,
#else
	BCMD_Otp_CmdMsp_eReserved131							=	131,
#endif
	BCMD_Otp_CmdMsp_eReserved132							=	132,
#if (BCHP_CHIP == 7425)
    BCMD_Otp_CmdMsp_ePcieGwinDisable                        = 133,
#else
	BCMD_Otp_CmdMsp_eReserved133							=	133,
#endif
	BCMD_Otp_CmdMsp_eReserved134							=	134,
	BCMD_Otp_CmdMsp_eReserved135							=	135,
#if (BCHP_CHIP == 7425)
    BCMD_Otp_CmdMsp_ePcieMwinDisable                        = 136,
    BCMD_Otp_CmdMsp_ePcieMwinRestrictEnable                 = 137,
#else
	BCMD_Otp_CmdMsp_eReserved136							=	136,
	BCMD_Otp_CmdMsp_eReserved137							=	137,
#endif
    BCMD_Otp_CmdMsp_eCaAesEcbMscSoftwareKeyDisable          = 138,
	BCMD_Otp_CmdMsp_eReserved139							=	139,
	BCMD_Otp_CmdMsp_eReserved140							=	140,
    BCMD_Otp_CmdMsp_eReserved141                            = 141,      /* previously eOtpOptionSpare*/
    BCMD_Otp_CmdMsp_eSidVerifyEnable                        = 142,
    BCMD_Otp_CmdMsp_eBgCheckResetEnable                     = 143,
	BCMD_Otp_CmdMsp_eReserved144							=	144,
	BCMD_Otp_CmdMsp_eReserved145							=	145,
	BCMD_Otp_CmdMsp_eReserved146							=	146,
	BCMD_Otp_CmdMsp_eReserved147							=	147,
	BCMD_Otp_CmdMsp_eReserved148							=	148,
	BCMD_Otp_CmdMsp_eReserved149							=	149,
	BCMD_Otp_CmdMsp_eReserved150							=	150,
	BCMD_Otp_CmdMsp_eReserved151							=	151,
/*-Zeus 2.0 additions-*/
    BCMD_Otp_CmdMsp_eDramScramblerKeyReuseDisable           = 152,
#if (BCHP_CHIP == 7425) || (BCHP_CHIP == 7435)
    BCMD_Otp_CmdMsp_eMMCBootDisable                         = 153,
#else
	BCMD_Otp_CmdMsp_eReserved153							=	153,
#endif
	BCMD_Otp_CmdMsp_eReserved154							=	154,
	BCMD_Otp_CmdMsp_eReserved155							=	155,
#if (BCHP_CHIP == 7231)
    BCMD_Otp_CmdMsp_eEbiCsSwapDisable                       = 156,
#else
	BCMD_Otp_CmdMsp_eReserved156							=	156,
#endif
	BCMD_Otp_CmdMsp_eReserved157							=	157,
	BCMD_Otp_CmdMsp_eReserved158							=	158,
	BCMD_Otp_CmdMsp_eReserved159							=	159,
	BCMD_Otp_CmdMsp_eReserved160							=	160,
	BCMD_Otp_CmdMsp_eReserved161							=	161,
	BCMD_Otp_CmdMsp_eReserved162							=	162,
	BCMD_Otp_CmdMsp_eReserved163							=	163,
	BCMD_Otp_CmdMsp_eReserved164							=	164,
	BCMD_Otp_CmdMsp_eReserved165							=	165,
	BCMD_Otp_CmdMsp_eReserved166							=	166,
	BCMD_Otp_CmdMsp_eReserved167							=	167,
	BCMD_Otp_CmdMsp_eReserved168							=	168,
	BCMD_Otp_CmdMsp_eReserved169							=	169,
	BCMD_Otp_CmdMsp_eReserved170							=	170,
	BCMD_Otp_CmdMsp_eReserved171							=	171,
	BCMD_Otp_CmdMsp_eReserved172							=	172,
	BCMD_Otp_CmdMsp_eReserved173							=	173,
	BCMD_Otp_CmdMsp_eReserved174							=	174,
	BCMD_Otp_CmdMsp_eReserved175							=	175,
	BCMD_Otp_CmdMsp_eReserved176							=	176,
	BCMD_Otp_CmdMsp_eReserved177							=	177,
	BCMD_Otp_CmdMsp_eReserved178							=	178,
	BCMD_Otp_CmdMsp_eReserved179							=	179,
	BCMD_Otp_CmdMsp_eReserved180							=	180,
	BCMD_Otp_CmdMsp_eReserved181							=	181,
	BCMD_Otp_CmdMsp_eReserved182							=	182,
	BCMD_Otp_CmdMsp_eReserved183							=	183,
	BCMD_Otp_CmdMsp_eReserved184							=	184,
	BCMD_Otp_CmdMsp_eReserved185							=	185,
    BCMD_Otp_CmdMsp_eSecureRSA2KpkRsaRootDisable            = 186,
	BCMD_Otp_CmdMsp_eReserved187							=	187,
	BCMD_Otp_CmdMsp_eReserved188							=	188,
    BCMD_Otp_CmdMsp_eSecureRsa2RenewRsaKeyVerifyEnable      = 189,
#if (BCHP_CHIP == 7435)
    BCMD_Otp_CmdMsp_eDualCoreAcpuMode                       = 190,
    BCMD_Otp_CmdMsp_eSecureSandboxEnable                    = 191,
    BCMD_Otp_CmdMsp_eWebCpuVerifyEnable                     = 192,
    BCMD_Otp_CmdMsp_eRaagaBVerifyEnable                     = 193,
#else
	BCMD_Otp_CmdMsp_eReserved190							=	190,
	BCMD_Otp_CmdMsp_eReserved191							=	191,
	BCMD_Otp_CmdMsp_eReserved192							=	192,
	BCMD_Otp_CmdMsp_eReserved193							=	193,
#endif
	BCMD_Otp_CmdMsp_eReserved194							=	194,
#if (BCHP_CHIP == 7435)
    BCMD_Otp_CmdMsp_eExternalKeyTableDisable                = 195,
#else
	BCMD_Otp_CmdMsp_eReserved195							=	195,
#endif
    BCMD_Otp_CmdMsp_eOneWayProgramEnable                    = 196,
	BCMD_Otp_CmdMsp_eReserved197							=	197,
	BCMD_Otp_CmdMsp_eReserved198							=	198,
    BCMD_Otp_CmdMsp_eReserved199                            = 199, /* Was ScmVerifyEnable */
	BCMD_Otp_CmdMsp_eReserved200							=	200,
	BCMD_Otp_CmdMsp_eReserved201							=	201,
    BCMD_Otp_CmdMsp_eReserved202                            = 202,
    BCMD_Otp_CmdMsp_eReserved203                            = 203,
    BCMD_Otp_CmdMsp_eReserved204                            = 204,
    BCMD_Otp_CmdMsp_eReserved205                            = 205,
    BCMD_Otp_CmdMsp_eReserved206                            = 206,
    BCMD_Otp_CmdMsp_eReserved207                            = 207,
    BCMD_Otp_CmdMsp_eReserved208                            = 208,
    BCMD_Otp_CmdMsp_eReserved209                            = 209,
    BCMD_Otp_CmdMsp_eReserved210                            = 210,
    BCMD_Otp_CmdMsp_eReserved211                            = 211,
    BCMD_Otp_CmdMsp_eReserved212                            = 212,
    BCMD_Otp_CmdMsp_eReserved213                            = 213,
    BCMD_Otp_CmdMsp_eReserved214                            = 214,
    BCMD_Otp_CmdMsp_eReserved215                            = 215,
    BCMD_Otp_CmdMsp_eReserved216                            = 216,
    BCMD_Otp_CmdMsp_eReserved217                            = 217,
    BCMD_Otp_CmdMsp_eReserved218                            = 218,
    BCMD_Otp_CmdMsp_eReserved219                            = 219,
    BCMD_Otp_CmdMsp_eReserved220                            = 220,
	BCMD_Otp_CmdMsp_eReserved221							=	221,
	BCMD_Otp_CmdMsp_eReserved222							=	222,
    BCMD_Otp_CmdMsp_eSize                                           /* 223 */
} BCMD_Otp_CmdMsp_e ;

#endif /* BSP_S_OTP_COMMON_H__ */