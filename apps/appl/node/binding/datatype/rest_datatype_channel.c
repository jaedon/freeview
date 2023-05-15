/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_datatype_channel.c
	@brief

	Description:  									\n
	Module: node/binding/datatype_convert		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <apk.h>
#include <rest_bind_api.h>
#include <rest_datatype.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
// Converting Types:
STATIC RestEnumStringTable_t	 s_stRestDataType_SvcType =
{
	(HCHAR*)DxNAMEOF(DxSvcType_e),
		
	{
		{	"all",			eDxSVC_TYPE_All		},
		{	"tv",			eDxSVC_TYPE_TV		},
		{	"radio",		eDxSVC_TYPE_RADIO	},
		{	"data",			eDxSVC_TYPE_DATA	},
		{	NULL,			0					}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_DeliType =
{
	(HCHAR*)DxNAMEOF(DxDeliveryType_e),
		
	{
		{	"sat",			eDxDELIVERY_TYPE_SAT	},
		{	"cab",			eDxDELIVERY_TYPE_TER	},
		{	"ter",			eDxDELIVERY_TYPE_CAB	},
		{	"ip",			eDxDELIVERY_TYPE_MULTICAST	},
		{	NULL,			0					}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_VideoType =
{
	(HCHAR*)DxNAMEOF(DxVideoType_e),
		
	{
		{	"sd",			eDxVIDEO_TYPE_SD	},
		{	"hd",			eDxVIDEO_TYPE_HD	},
		{	NULL,			0					}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_VideoCodec =
{
	(HCHAR*)DxNAMEOF(DxVideoCodec_e),
		
	{
		{	"unknown",		eDxVIDEO_CODEC_UNKNOWN			},
		{	"mpeg1",		eDxVIDEO_CODEC_MPEG1			},
		{	"mpeg2",		eDxVIDEO_CODEC_MPEG2			},
		{	"mpeg4",		eDxVIDEO_CODEC_MPEG4_PART2		},
		{	"h261",			eDxVIDEO_CODEC_H261				},
		{	"h263",			eDxVIDEO_CODEC_H263				},
		{	"h264",			eDxVIDEO_CODEC_H264				},
		{	"h264_svc",		eDxVIDEO_CODEC_H264_SVC			},
		{	"h264_mvc",		eDxVIDEO_CODEC_H264_MVC			},
		{	"vc1",			eDxVIDEO_CODEC_VC1				},
		{	"vc1_sm",		eDxVIDEO_CODEC_VC1_SIMPLEMAIN	},
		{	"avs", 			eDxVIDEO_CODEC_AVS 				},
		{	"rv40", 		eDxVIDEO_CODEC_RV40 			},
		{	"vp6", 			eDxVIDEO_CODEC_VP6 				},
		{	"divx311", 		eDxVIDEO_CODEC_DIVX311 			},
		{	"xvid",			eDxVIDEO_CODEC_XVID				},
		{	NULL,			0								}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_AudioCodec =
{
	(HCHAR*)DxNAMEOF(DxAudioCodec_e),
		
	{
		{	"unknown",			eDxAUDIO_CODEC_UNKNOWN			},
		{	"mpeg",				eDxAUDIO_CODEC_MPEG				},
		{	"mp3",				eDxAUDIO_CODEC_MP3				},
		{	"dolby_ac3",		eDxAUDIO_CODEC_DOLBY_AC3		},
		{	"dolby_ac3p",		eDxAUDIO_CODEC_DOLBY_AC3P		},
		{	"pcm",				eDxAUDIO_CODEC_PCM				},
		{	"aac",				eDxAUDIO_CODEC_AAC				},
		{	"aac_loas",			eDxAUDIO_CODEC_AAC_LOAS			},
		{	"aac_plus",			eDxAUDIO_CODEC_AAC_PLUS			},
		{	"aac_plus_adts",	eDxAUDIO_CODEC_AAC_PLUS_ADTS	},
		{	"dts",				eDxAUDIO_CODEC_DTS				},
		{	"dtshd", 			eDxAUDIO_CODEC_DTS_HD 			},
		{	"lpcm_dvd", 		eDxAUDIO_CODEC_LPCM_DVD 		},
		{	"lpcm_hddvd", 		eDxAUDIO_CODEC_LPCM_HD_DVD 		},
		{	"lpcm_bluray", 		eDxAUDIO_CODEC_LPCM_BLUERAY 	},
		{	"wma_std",			eDxAUDIO_CODEC_WMA_STD			},
		{	"wma_pro",			eDxAUDIO_CODEC_WMA_PRO			},
		{	"pcm_wave", 		eDxAUDIO_CODEC_PCMWAVE 			},
		{	"dra", 				eDxAUDIO_CODEC_DRA 				},
		{	"aiff", 			eDxAUDIO_CODEC_AIFF 			},
		{	"riff", 			eDxAUDIO_CODEC_RIFF 			},
		{	NULL,			0									}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_CasType =
{
	(HCHAR*)DxNAMEOF(DxCasType_e),
		
	{
		{	"fta",			eDxCAS_TYPE_FTA			},
		{	"nagra",		eDxCAS_TYPE_NAGRA		},
		{	"irdeto",		eDxCAS_TYPE_IRDETO		},
		{	"nds",			eDxCAS_TYPE_NDS			},
		{	"viaccess",		eDxCAS_TYPE_VIACCESS	},
		{	"conax",		eDxCAS_TYPE_CONAX		},
		{	"mediaguard",	eDxCAS_TYPE_MEDIAGUARD	},
		{	"cryptoworks",	eDxCAS_TYPE_CRYPTOWORKS	},
		{	"bcas",			eDxCAS_TYPE_JPBCAS		},
		{	"ccas",			eDxCAS_TYPE_JPCCAS		},
		{	"ciplus",		eDxCAS_TYPE_CIPLUS		},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_SatTransportSpec =
{
	(HCHAR*)DxNAMEOF(DxSat_TransportSpec_e),
		
	{
		{	"dvb",			eDxSAT_TRANS_DVBS		},
		{	"dvb2",			eDxSAT_TRANS_DVBS2		},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_SatPskModulation =
{
	(HCHAR*)DxNAMEOF(DxSat_PskModulation_e),
		
	{
		{	"auto",			eDxSAT_PSK_AUTO			},
		{	"qpsk",			eDxSAT_PSK_QPSK			},
		{	"8psk", 		eDxSAT_PSK_8PSK			},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_SatPolarization =
{
	(HCHAR*)DxNAMEOF(DxSat_Polarization_e),
		
	{
		{	"auto",			eDxSAT_POLAR_AUTO		},
		{	"hor", 			eDxSAT_POLAR_HOR 		},
		{	"ver", 			eDxSAT_POLAR_VER 		},
		{	"left", 		eDxSAT_POLAR_LEFT 		},
		{	"right", 		eDxSAT_POLAR_RIGHT 		},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_SatCodeRate =
{
	(HCHAR*)DxNAMEOF(DxSat_CodeRate_e),
		
	{
		{	"auto",			eDxSAT_CODERATE_AUTO	},
		{	"none", 		eDxSAT_CODERATE_NONE	},
		{	"1/2", 			eDxSAT_CODERATE_1_2		},
		{	"2/3", 			eDxSAT_CODERATE_2_3 	},
		{	"3/4", 			eDxSAT_CODERATE_3_4 	},
		{	"5/6", 			eDxSAT_CODERATE_5_6		},
		{	"7/8",			eDxSAT_CODERATE_7_8		},
		{	"3/5",			eDxSAT_CODERATE_3_5		},
		{	"4/5", 			eDxSAT_CODERATE_4_5		},
		{	"5/11",			eDxSAT_CODERATE_5_11	},
		{	"6/7",			eDxSAT_CODERATE_6_7 	},
		{	"8/9",			eDxSAT_CODERATE_8_9 	},
		{	"9/10", 		eDxSAT_CODERATE_9_10	},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_SatLnbVolt =
{
	(HCHAR*)DxNAMEOF(DxSAT_LnbVoltage_e),
		
	{
		{	"std",			eDxLNB_VOLT_STD		},
		{	"high", 		eDxLNB_VOLT_HIGH	},
		{	NULL,			0					}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_SatDiseqcVer =
{
	(HCHAR*)DxNAMEOF(DxSAT_DiseqcVer_e),
		
	{
		{	"1.0",			eDxDISEQC_VER_1_0	},
		{	"1.1", 			eDxDISEQC_VER_1_1	},
		{	"1.2",			eDxDISEQC_VER_1_2	},
		{	"1.3",			eDxDISEQC_VER_1_3	},
		{	"2.0", 			eDxDISEQC_VER_2_0	},
		{	NULL,			0					}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_SatDiseqcInput =
{
	(HCHAR*)DxNAMEOF(DxSAT_DiseqcInput_e),
		
	{
		{	"none",			eDxDISEQC_INPUT_NONE	},
		{	"a", 			eDxDISEQC_INPUT_A		},
		{	"b",			eDxDISEQC_INPUT_B		},
		{	"c",			eDxDISEQC_INPUT_C		},
		{	"d", 			eDxDISEQC_INPUT_D		},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_SatToneBurst =
{
	(HCHAR*)DxNAMEOF(DxSAT_ToneBurst_e),
		
	{
		{	"none",			eDxTONE_BURST_NONE		},
		{	"a", 			eDxTONE_BURST_A			},
		{	"b",			eDxTONE_BURST_B			},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_CabConstellation =
{
	(HCHAR*)DxNAMEOF(DxCab_Constellation_e),
		
	{
		{	"auto",			eDxCAB_CONSTELLATION_AUTO		},
		{	"16qam", 		eDxCAB_CONSTELLATION_16QAM		},
		{	"32qam",		eDxCAB_CONSTELLATION_32QAM		},
		{	"64qam", 		eDxCAB_CONSTELLATION_64QAM		},
		{	"128qam",		eDxCAB_CONSTELLATION_128QAM		},
		{	"256qam",		eDxCAB_CONSTELLATION_256QAM		},
		{	"1024qam", 		eDxCAB_CONSTELLATION_1024QAM	},
		{	"4096qam",		eDxCAB_CONSTELLATION_4096QAM	},
		{	NULL,			0								}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_CabSpectrum =
{
	(HCHAR*)DxNAMEOF(DxCab_Spectrum_e),
		
	{
		{	"auto",			eDxCAB_SPECTRUM_AUTO		},
		{	"normal", 		eDxCAB_SPECTRUM_NORMAL		},
		{	"inverted",		eDxCAB_SPECTRUM_INVERTED	},
		{	NULL,			0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerBandwidth =
{
	(HCHAR*)DxNAMEOF(DxTer_BandWidth_e),
		
	{
		{	"8MHz",			eDxTER_BANDWIDTH_8M			},
		{	"7MHz", 		eDxTER_BANDWIDTH_7M			},
		{	"6MHz",			eDxTER_BANDWIDTH_6M			},
		{	"5MHz", 		eDxTER_T2BANDWIDTH_5MHZ		},
		{	"10MHz",		eDxTER_T2BANDWIDTH_10MHZ	},
		{	"1.7MHz",	 	eDxTER_T2BANDWIDTH_1P7MHZ	},
		{	NULL,			0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerConstellation =
{
	(HCHAR*)DxNAMEOF(DxTer_Constellation_e),
		
	{
		{	"auto",			eDxTER_CONSTELLATION_AUTO	},
		{	"qpsk", 		eDxTER_CONSTELLATION_QPSK	},
		{	"16qam",		eDxTER_CONSTELLATION_16QAM	},
		{	"64qam", 		eDxTER_CONSTELLATION_64QAM	},
		{	"256qam",		eDxTER_CONSTELLATION_256QAM	},
		{	NULL,			0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerHierarchy =
{
	(HCHAR*)DxNAMEOF(DxTer_Hierachy_e),
		
	{
		{	"auto",			eDxTER_HIERACHY_AUTO		},
		{	"none", 		eDxTER_HIERACHY_NONE		},
		{	"1",			eDxTER_HIERACHY_1			},
		{	"2", 			eDxTER_HIERACHY_2			},
		{	"4",			eDxTER_HIERACHY_4			},
		{	NULL,			0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerCodeRate =
{
	(HCHAR*)DxNAMEOF(DxTer_CodeRate_e),
		
	{
		{	"auto",			eDxTER_CODERATE_AUTO		},
		{	"1/2", 			eDxTER_CODERATE_1_2			},
		{	"2/3",			eDxTER_CODERATE_2_3			},
		{	"3/4", 			eDxTER_CODERATE_3_4			},
		{	"5/6",			eDxTER_CODERATE_5_6			},
		{	"7/8",			eDxTER_CODERATE_7_8			},
		{	NULL,			0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerTransMode =
{
	(HCHAR*)DxNAMEOF(DxTer_TransMode_e),
		
	{
		{	"auto",			eDxTER_TRANSMODE_AUTO		},
		{	"2k", 			eDxTER_TRANSMODE_2K			},
		{	"8k",			eDxTER_TRANSMODE_8K			},
		{	"1k", 			eDxTER_T2TRANSMODE_1K		},
		{	"4k",			eDxTER_T2TRANSMODE_4K		},
		{	"16k",			eDxTER_T2TRANSMODE_16K		},
		{	"32k",			eDxTER_T2TRANSMODE_32K 		},
		{	NULL,			0							}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerGuardInterval =
{
	(HCHAR*)DxNAMEOF(DxTer_GuardInterval_e),
		
	{
		{	"auto",			eDxTER_GUARDINTERVAL_AUTO		},
		{	"1/4", 			eDxTER_GUARDINTERVAL_1_4		},
		{	"1/8",			eDxTER_GUARDINTERVAL_1_8		},
		{	"1/16", 		eDxTER_GUARDINTERVAL_1_16		},
		{	"1/32",			eDxTER_GUARDINTERVAL_1_32		},
		{	"1/128",		eDxTER_T2GUARDINTERVAL_1_128	},
		{	"19/128",		eDxTER_T2GUARDINTERVAL_19_128 	},
		{	"19/256",		eDxTER_T2GUARDINTERVAL_19_256	},
		{	NULL,			0								}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerStream =
{
	(HCHAR*)DxNAMEOF(DxTer_Stream_e),
		
	{
		{	"hp",			eDxTER_STREAM_HP		},
		{	"lp", 			eDxTER_STREAM_LP		},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerOffset =
{
	(HCHAR*)DxNAMEOF(DxTer_Offset_e),
		
	{
		{	"auto",			eDxTER_OFFSET_AUTO		},
		{	"none", 		eDxTER_OFFSET_NONE		},
		{	"+1/6",			eDxTER_OFFSET_1_6_P		},
		{	"-1/6", 		eDxTER_OFFSET_1_6_N		},
		{	"+2/6", 		eDxTER_OFFSET_2_6_P 	},
		{	"-2/6", 		eDxTER_OFFSET_2_6_N 	},
		{	"+3/6", 		eDxTER_OFFSET_3_6_P 	},
		{	"-3/6", 		eDxTER_OFFSET_3_6_N 	},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerSystem =
{
	(HCHAR*)DxNAMEOF(DxTer_System_e),
		
	{
		{	"dvb-t",		eDxTER_DVB_T			},
		{	"dvb-t2", 		eDxTER_DVB_T2			},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerPreambleFormat =
{
	(HCHAR*)DxNAMEOF(DxTer_PreambleFormat_e),
		
	{
		{	"siso",			eDxTER_SISO				},
		{	"miso", 		eDxTER_MISO				},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerMixed =
{
	(HCHAR*)DxNAMEOF(DxTer_Mixed_e),
		
	{
		{	"not_mixed",	eDxTER_NOT_MIXED		},
		{	"mixed", 		eDxTER_MIXED			},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerPilotPattern =
{
	(HCHAR*)DxNAMEOF(DxTer_Pilot_Pattern_e),

	{
		{	"auto",			eDxTER_NOT_MIXED		},
		{	"1", 			eDxTER_PP1				},
		{	"2", 			eDxTER_PP2				},
		{	"3",			eDxTER_PP3				},
		{	"4", 			eDxTER_PP4				},
		{	"5",			eDxTER_PP5				},
		{	"6", 			eDxTER_PP6				},
		{	"7",			eDxTER_PP7				},
		{	"8",			eDxTER_PP8				},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerExtendedCarrier =
{
	(HCHAR*)DxNAMEOF(DxTer_ExtendedCarrier_e),

	{
		{	"not_used",		eDxTER_T2NOT_USED		},
		{	"used", 		eDxTER_T2USED			},
		{	NULL,			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_TerPaprReduction =
{
	(HCHAR*)DxNAMEOF(DxTer_PAPR_Reduction_e),

	{
		{	"none",			eDxTER_REDUCTION_NONE	},
		{	"ace", 			eDxTER_ACE				},
		{	"tr", 			eDxTER_TR				},
		{	"ave_tr", 		eDxTER_AVE_TR			},
		{	NULL,			0						}
	}
};

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___
#define _____SERVICE_____
HINT32 rest_datatype_JsonizeServiceWithoutObject (HINT32 nJsonWriter, DxService_t *pstSvc, HCHAR *szSelect)
{
	HINT32		 nWriter;

	if (NULL == pstSvc)
	{
		HxLOG_Error("pstSvc NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	(void)rest_datatype_SetJsonInteger(nWriter, "channelUid", (HINT32)pstSvc->uid, szSelect);
	(void)rest_datatype_SetJsonInteger(nWriter, "tsUid", (HINT32)pstSvc->tsuid, szSelect);
	if (eDxDELIVERY_TYPE_SAT == pstSvc->deliType)
	{
		(void)rest_datatype_SetJsonInteger(nWriter, "antennaUid", (HINT32)pstSvc->antuid, szSelect);
	}
	(void)rest_datatype_SetJsonEnum(nWriter, "channelType", DxNAMEOF(DxSvcType_e), (HINT32)pstSvc->svcType, szSelect);
	(void)rest_datatype_SetJsonEnum(nWriter, "deliveryType", DxNAMEOF(DxDeliveryType_e), (HINT32)pstSvc->deliType, szSelect);
	(void)rest_datatype_SetJsonInteger(nWriter, "onId", (HINT32)pstSvc->onid, szSelect);
	(void)rest_datatype_SetJsonInteger(nWriter, "tsId", (HINT32)pstSvc->tsid, szSelect);
	(void)rest_datatype_SetJsonInteger(nWriter, "svcId", (HINT32)pstSvc->svcid, szSelect);
	(void)rest_datatype_SetJsonInteger(nWriter, "channelNumber", (HINT32)pstSvc->lcn, szSelect);
	(void)rest_datatype_SetJsonString(nWriter, "name", pstSvc->name, szSelect);
	(void)rest_datatype_SetJsonEnum(nWriter, "videoType", DxNAMEOF(DxVideoType_e), (HINT32)pstSvc->highDefinition, szSelect);
	(void)rest_datatype_SetJsonEnum(nWriter, "videoCodec", DxNAMEOF(DxVideoCodec_e), (HINT32)pstSvc->videoCodec, szSelect);
	(void)rest_datatype_SetJsonEnum(nWriter, "audioCodec", DxNAMEOF(DxAudioCodec_e), (HINT32)pstSvc->audioCodec, szSelect);
	(void)rest_datatype_SetJsonInteger(nWriter, "isLcn", (HINT32)pstSvc->lcnFlag, szSelect);
	(void)rest_datatype_SetJsonEnum(nWriter, "casType", DxNAMEOF(DxCasType_e), (HINT32)pstSvc->casType, szSelect);

	return nWriter;	
}

HINT32 rest_datatype_JsonizeService (HINT32 nJsonWriter, DxService_t *pstSvc, RestChanExtInfo_t *pstExtInfo, HCHAR *szSelect)
{
	HINT32		 nWriter;

	if (NULL == pstSvc)
	{
		HxLOG_Error("pstSvc NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		rest_datatype_JsonizeServiceWithoutObject(nWriter, pstSvc, szSelect);

		if (NULL != pstExtInfo)
		{
			if ('\0' != pstExtInfo->stLiveStreamInfo.szLiveStreamUrl[0])
			{
				(void)HxJSONWRITER_StringValue(nJsonWriter, "liveStream");
				rest_datatype_JsonizeLiveStreamInfo(nWriter, &(pstExtInfo->stLiveStreamInfo), szSelect);
			}

			if ('\0' != pstExtInfo->stThumbnailInfo.szThumbnailUrl[0])
			{
				(void)HxJSONWRITER_StringValue(nJsonWriter, "thumbnail");
				rest_datatype_JsonizeThumbnailInfo(nWriter, &(pstExtInfo->stThumbnailInfo), szSelect);
			}
		}
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

HERROR rest_datatype_DejsonizeService (HxJSON_t *pstJson, DxService_t *pstSvc, HBOOL bClearObj)
{

	if (NULL == pstSvc)
	{
		HxLOG_Error("pstSvc NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstSvc, 0, sizeof(DxService_t));
		pstSvc->typeOf = DxNAMEOF(DxService_t);
	}

	(void)rest_datatype_GetJsonInteger(pstJson, "channelUid", (HINT32 *)&(pstSvc->uid));
	(void)rest_datatype_GetJsonInteger(pstJson, "tsUid", (HINT32 *)&(pstSvc->tsuid));
	(void)rest_datatype_GetJsonInteger(pstJson, "antennaUid", (HINT32 *)&(pstSvc->antuid));
	(void)rest_datatype_GetJsonEnum(pstJson, "channelType", DxNAMEOF(DxSvcType_e), (HINT32 *)&(pstSvc->svcType));
	(void)rest_datatype_GetJsonEnum(pstJson, "deliveryType", DxNAMEOF(DxDeliveryType_e), (HINT32 *)&(pstSvc->svcType));
	(void)rest_datatype_GetJsonInteger(pstJson, "onId", (HINT32 *)&(pstSvc->onid));
	(void)rest_datatype_GetJsonInteger(pstJson, "tsId", (HINT32 *)&(pstSvc->tsid));
	(void)rest_datatype_GetJsonInteger(pstJson, "svcId", (HINT32 *)&(pstSvc->svcid));
	(void)rest_datatype_GetJsonInteger(pstJson, "channelNumber", (HINT32 *)&(pstSvc->lcn));
	(void)rest_datatype_GetJsonString(pstJson, "name", pstSvc->name, DxNAME_LEN);
	(void)rest_datatype_GetJsonEnum(pstJson, "videoType", DxNAMEOF(DxVideoType_e), (HINT32 *)&(pstSvc->highDefinition));
	(void)rest_datatype_GetJsonEnum(pstJson, "videoCodec", DxNAMEOF(DxVideoCodec_e), (HINT32 *)&(pstSvc->videoCodec));
	(void)rest_datatype_GetJsonEnum(pstJson, "audioCodec", DxNAMEOF(DxAudioCodec_e), (HINT32 *)&(pstSvc->audioCodec));
	(void)rest_datatype_GetJsonInteger(pstJson, "isLcn", (HINT32 *)&(pstSvc->lcnFlag));
	(void)rest_datatype_GetJsonEnum(pstJson, "casType", DxNAMEOF(DxCasType_e), (HINT32 *)&(pstSvc->casType));

	return ERR_OK;	
}

HINT32 rest_datatype_JsonizeLiveStreamInfo (HINT32 nJsonWriter, RestLiveStreamInfo_t *pstLiveStreamInfo, HCHAR *szSelect)
{
	HINT32					 nWriter;

	if (NULL == pstLiveStreamInfo)
	{
		HxLOG_Error("pstLiveStreamInfo NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonString(nWriter, "mimeType", pstLiveStreamInfo->szMimeType, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "url", pstLiveStreamInfo->szLiveStreamUrl, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}


HINT32 rest_datatype_JsonizeThumbnailInfo (HINT32 nJsonWriter, RestThumbnailInfo_t *pstThumbnailInfo, HCHAR *szSelect)
{
	HINT32					 nWriter;

	if (NULL == pstThumbnailInfo)
	{
		HxLOG_Error("pstThumbnailInfo NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonString(nWriter, "mimeType", pstThumbnailInfo->szMimeType, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "url", pstThumbnailInfo->szThumbnailUrl, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}


HINT32 rest_datatype_JsonizeChannelExt (HINT32 nJsonWriter, RestChannelExt_t *pstChanExt, HCHAR *szSelect)
{
	HINT32		 nWriter;

	if (NULL == pstChanExt)
	{
		HxLOG_Error("pstChanExt NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		rest_datatype_JsonizeServiceWithoutObject(nWriter, &(pstChanExt->stSvcInfo), szSelect);

		if ('\0' != pstChanExt->stLiveStreamInfo.szLiveStreamUrl[0])
		{
			(void)HxJSONWRITER_StringValue(nJsonWriter, "liveStream");
			rest_datatype_JsonizeLiveStreamInfo(nWriter, &pstChanExt->stLiveStreamInfo, szSelect);

		}
		if ('\0' != pstChanExt->stThumbnailInfo.szThumbnailUrl[0])
		{
			(void)HxJSONWRITER_StringValue(nJsonWriter, "thumbnail");
			rest_datatype_JsonizeThumbnailInfo(nWriter, &pstChanExt->stThumbnailInfo, szSelect);
		}
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

#define _____TRANSPORT_STREAM_____
HINT32 rest_datatype_JsonizeTransponder (HINT32 nJsonWriter, DxTransponder_t *pstTsInfo, HCHAR *szSelect)
{
	HINT32		 nWriter;

	if (NULL == pstTsInfo)
	{
		HxLOG_Error("pstTsInfo NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonInteger(nWriter, "tsUid", (HINT32)pstTsInfo->uid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "networkUid", (HINT32)pstTsInfo->netuid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "onId", (HINT32)pstTsInfo->onid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "tsId", (HINT32)pstTsInfo->tsid, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "tunerId", (HINT32)pstTsInfo->tunerid, szSelect);
		if (eDxDELIVERY_TYPE_SAT == pstTsInfo->tuningParam.eDeliType)
		{
			(void)rest_datatype_SetJsonInteger(nWriter, "antennaUid", (HINT32)pstTsInfo->tuningParam.sat.antInfo.uid, szSelect);
		}
		(void)rest_datatype_SetJsonEnum(nWriter, "deliveryType", DxNAMEOF(DxDeliveryType_e), (HINT32)pstTsInfo->tuningParam.eDeliType, szSelect);

		if (rest_datatype_CheckWriteSelected("tuningParam", szSelect))
		{
			HxJSONWRITER_StringValue(nWriter, "tuningParam");
			{
				(void)rest_datatype_JsonizeTuningParam(nWriter, &(pstTsInfo->tuningParam), NULL);
			}
		}
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

HERROR rest_datatype_DejsonizeTransponder (HxJSON_t *pstJson, DxTransponder_t *pstTsInfo, HBOOL bClearObj)
{
	HxJSON_t		*pjsTuningParam;

	if (NULL == pstTsInfo)
	{
		HxLOG_Error("pstTsInfo NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstTsInfo, 0, sizeof(DxTransponder_t));
		pstTsInfo->typeOf = DxNAMEOF(DxTransponder_t);
	}

	(void)rest_datatype_GetJsonInteger(pstJson, "tsUid", (HINT32 *)&(pstTsInfo->uid));
	(void)rest_datatype_GetJsonInteger(pstJson, "networkUid", (HINT32 *)&(pstTsInfo->netuid));
	(void)rest_datatype_GetJsonInteger(pstJson, "onId", (HINT32 *)&(pstTsInfo->onid));
	(void)rest_datatype_GetJsonInteger(pstJson, "tsId", (HINT32 *)&(pstTsInfo->tsid));
	(void)rest_datatype_GetJsonInteger(pstJson, "tunerId", (HINT32 *)&(pstTsInfo->tunerid));
	
	pjsTuningParam = HLIB_JSON_Get(pstJson, "tuningParam");
	if (NULL != pjsTuningParam)
	{
		rest_datatype_DejsonizeTuningParam(pjsTuningParam, &(pstTsInfo->tuningParam), FALSE);
	}

	return ERR_OK;	
}


HINT32 rest_datatype_JsonizeTuningParam (HINT32 nJsonWriter, DxTuneParam_t *pstTuneParam, HCHAR *szSelect)
{
	if (NULL == pstTuneParam)
	{
		HxLOG_Error("pstTuneParam NULL\n");
		return nJsonWriter;
	}

	switch(pstTuneParam->eDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
		return rest_datatype_JsonizeSatTuningParam(nJsonWriter, pstTuneParam, szSelect);

	case eDxDELIVERY_TYPE_CAB:
		return rest_datatype_JsonizeCabTuningParam(nJsonWriter, pstTuneParam, szSelect);

	case eDxDELIVERY_TYPE_TER:
		return rest_datatype_JsonizeTerTuningParam(nJsonWriter, pstTuneParam, szSelect);

	default:
		break;		
	}

	return nJsonWriter;
}

HERROR rest_datatype_DejsonizeTuningParam (HxJSON_t *pstJson, DxTuneParam_t *pstTuneParam, HBOOL bClearObj)
{
	if (NULL == pstTuneParam)
	{
		HxLOG_Error("pstTuneParam NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstTuneParam, 0, sizeof(DxTuneParam_t));
	}

	(void)rest_datatype_GetJsonEnum(pstJson, "deliveryType", DxNAMEOF(DxDeliveryType_e), (HINT32 *)&(pstTuneParam->eDeliType));

	switch (pstTuneParam->eDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
		return rest_datatype_DejsonizeSatTuningParam(pstJson, &(pstTuneParam->sat), FALSE);
		
	case eDxDELIVERY_TYPE_CAB:
		return rest_datatype_DejsonizeCabTuningParam(pstJson, &(pstTuneParam->cab), FALSE);
		
	case eDxDELIVERY_TYPE_TER:
		return rest_datatype_DejsonizeTerTuningParam(pstJson, &(pstTuneParam->ter), FALSE);

	default:
		HxLOG_Error(">>> Delivery Type wrong: (0x%08x)\n", pstTuneParam->eDeliType);
		break;
	}

	return ERR_FAIL;
}

HINT32 rest_datatype_JsonizeSatTuningParam (HINT32 nJsonWriter, DxTuneParam_t *pstTuneParam, HCHAR *szSelect)
{
	HINT32					 nWriter;
	DxSat_TuningTPInfo_t	*pstSatTune;

	if (NULL == pstTuneParam)
	{
		HxLOG_Error("pstTuneParam NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	pstSatTune = &(pstTuneParam->sat.tuningInfo);

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonEnum(nWriter, "transportSpec", DxNAMEOF(DxSat_TransportSpec_e), (HINT32)pstSatTune->eTransSpec, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "frequency", (HINT32)pstSatTune->ulFrequency, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "moduleationMode", DxNAMEOF(DxSat_PskModulation_e), (HINT32)pstSatTune->ePskMod, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "symbolRate", (HINT32)pstSatTune->ulSymbolRate, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "polarization", DxNAMEOF(DxSat_Polarization_e), (HINT32)pstSatTune->ePolarization, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "codeRate", DxNAMEOF(DxSat_CodeRate_e), (HINT32)pstSatTune->eFecCodeRate, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "antennaUid", (HINT32)pstTuneParam->sat.antInfo.uid, szSelect);
		if (rest_datatype_CheckWriteSelected("antennaParam", szSelect))
		{
			HxJSONWRITER_StringValue(nWriter, "antennaParam");
			{
				(void)rest_datatype_JsonizeAntenna(nJsonWriter, &(pstTuneParam->sat.antInfo), NULL);
			}
		}
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

HERROR rest_datatype_DejsonizeSatTuningParam (HxJSON_t *pstJson, DxSatTuningInfo_t *pstTuneInfo, HBOOL bClearObj)
{
	HxJSON_t				*pjsAntenna;
	DxSat_TuningTPInfo_t	*pstTpInfo;

	if (NULL == pstTuneInfo)
	{
		HxLOG_Error("pstTuneInfo NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstTuneInfo, 0, sizeof(DxSatTuningInfo_t));
	}

	pstTpInfo = &(pstTuneInfo->tuningInfo);

	(void)rest_datatype_GetJsonEnum(pstJson, "deliveryType", DxNAMEOF(DxDeliveryType_e), (HINT32 *)&(pstTuneInfo->deliveryType));
	(void)rest_datatype_GetJsonEnum(pstJson, "transportSpec", DxNAMEOF(DxSat_TransportSpec_e), (HINT32 *)&(pstTpInfo->eTransSpec));
	(void)rest_datatype_GetJsonInteger(pstJson, "frequency", (HINT32 *)&(pstTpInfo->ulFrequency));
	(void)rest_datatype_GetJsonEnum(pstJson, "moduleationMode", DxNAMEOF(DxSat_PskModulation_e), (HINT32 *)&(pstTpInfo->ePskMod));
	(void)rest_datatype_GetJsonInteger(pstJson, "symbolRate", (HINT32 *)&(pstTpInfo->ulSymbolRate));
	(void)rest_datatype_GetJsonEnum(pstJson, "polarization", DxNAMEOF(DxSat_Polarization_e), (HINT32 *)&(pstTpInfo->ePolarization));
	(void)rest_datatype_GetJsonEnum(pstJson, "codeRate", DxNAMEOF(DxSat_CodeRate_e), (HINT32 *)&(pstTpInfo->eFecCodeRate));
	(void)rest_datatype_GetJsonInteger(pstJson, "antennaUid", (HINT32 *)&(pstTuneInfo->antInfo.uid));

	pjsAntenna = HLIB_JSON_Get(pstJson, "antennaParam");
	if (NULL != pjsAntenna)
	{
		rest_datatype_DejsonizeAntenna(pjsAntenna, &(pstTuneInfo->antInfo), FALSE);
	}

	return ERR_OK;	
}

HINT32 rest_datatype_JsonizeCabTuningParam (HINT32 nJsonWriter, DxTuneParam_t *pstTuneParam, HCHAR *szSelect)
{
	HINT32					 nWriter;
	DxCabTuningInfo_t		*pstCabTune;

	if (NULL == pstTuneParam)
	{
		HxLOG_Error("pstTuneParam NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	pstCabTune = &(pstTuneParam->cab);

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonEnum(nWriter, "deliveryType", DxNAMEOF(DxDeliveryType_e), (HINT32)pstTuneParam->eDeliType, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "frequency", (HINT32)pstCabTune->ulFrequency, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "symbolRate", (HINT32)pstCabTune->ulSymbolRate, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "constellation", DxNAMEOF(DxCab_Constellation_e), (HINT32)pstCabTune->constellation, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "spectrum", DxNAMEOF(DxCab_Spectrum_e), (HINT32)pstCabTune->spectrum, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

HERROR rest_datatype_DejsonizeCabTuningParam (HxJSON_t *pstJson, DxCabTuningInfo_t *pstTuneInfo, HBOOL bClearObj)
{
	if (NULL == pstTuneInfo)
	{
		HxLOG_Error("pstTuneInfo NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstTuneInfo, 0, sizeof(DxCabTuningInfo_t));
	}

	(void)rest_datatype_GetJsonEnum(pstJson, "deliveryType", DxNAMEOF(DxDeliveryType_e), (HINT32 *)&(pstTuneInfo->deliveryType));
	(void)rest_datatype_GetJsonInteger(pstJson, "frequency", (HINT32 *)&(pstTuneInfo->ulFrequency));
	(void)rest_datatype_GetJsonInteger(pstJson, "symbolRate", (HINT32 *)&(pstTuneInfo->ulSymbolRate));
	(void)rest_datatype_GetJsonEnum(pstJson, "constellation", DxNAMEOF(DxCab_Constellation_e), (HINT32 *)&(pstTuneInfo->constellation));
	(void)rest_datatype_GetJsonEnum(pstJson, "spectrum", DxNAMEOF(DxCab_Spectrum_e), (HINT32 *)&(pstTuneInfo->spectrum));

	return ERR_OK;	
}

HINT32 rest_datatype_JsonizeTerTuningParam (HINT32 nJsonWriter, DxTuneParam_t *pstTuneParam, HCHAR *szSelect)
{
	HINT32					 nWriter;
	DxTerTuningInfo_t		*pstTerTune;

	if (NULL == pstTuneParam)
	{
		HxLOG_Error("pstTuneParam NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	pstTerTune = &(pstTuneParam->ter);

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonEnum(nWriter, "deliveryType", DxNAMEOF(DxDeliveryType_e), (HINT32)pstTuneParam->eDeliType, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "frequency", (HINT32)pstTerTune->ulFrequency, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "bandwidth", DxNAMEOF(DxTer_BandWidth_e), (HINT32)pstTerTune->eBandWidth, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "constellation", DxNAMEOF(DxTer_Constellation_e), (HINT32)pstTerTune->eConstellation, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "hierarchy", DxNAMEOF(DxTer_Hierachy_e), (HINT32)pstTerTune->eHierachy, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "codeRate", DxNAMEOF(DxTer_CodeRate_e), (HINT32)pstTerTune->eCodeRate, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "transMode", DxNAMEOF(DxTer_TransMode_e), (HINT32)pstTerTune->eTransMode, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "guardInterval", DxNAMEOF(DxTer_GuardInterval_e), (HINT32)pstTerTune->eGuardInterval, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "stream", DxNAMEOF(DxTer_Stream_e), (HINT32)pstTerTune->eStream, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "offset", DxNAMEOF(DxTer_Offset_e), (HINT32)pstTerTune->eOffset, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "system", DxNAMEOF(DxTer_System_e), (HINT32)pstTerTune->eSystem, szSelect);

		if (eDxTER_DVB_T2 == pstTerTune->eSystem)
		{
			(void)rest_datatype_SetJsonInteger(nWriter, "plpId", (HINT32)pstTerTune->stT2Param.ulPlpId, szSelect);
			(void)rest_datatype_SetJsonEnum(nWriter, "preambleFormat", DxNAMEOF(DxTer_PreambleFormat_e), (HINT32)pstTerTune->stT2Param.preambleFormat, szSelect);
			(void)rest_datatype_SetJsonEnum(nWriter, "mixed", DxNAMEOF(DxTer_Mixed_e), (HINT32)pstTerTune->stT2Param.mixed, szSelect);
			(void)rest_datatype_SetJsonEnum(nWriter, "pilotPattern", DxNAMEOF(DxTer_Pilot_Pattern_e), (HINT32)pstTerTune->stT2Param.pilotPattern, szSelect);
			(void)rest_datatype_SetJsonEnum(nWriter, "extendedCarrier", DxNAMEOF(DxTer_ExtendedCarrier_e), (HINT32)pstTerTune->stT2Param.extenedCarrier, szSelect);
			(void)rest_datatype_SetJsonEnum(nWriter, "parpReduction", DxNAMEOF(DxTer_PAPR_Reduction_e), (HINT32)pstTerTune->stT2Param.PAPRreduction, szSelect);
			(void)rest_datatype_SetJsonInteger(nWriter, "numPlpBlocks", (HINT32)pstTerTune->stT2Param.numPlpBlocks, szSelect);
		}
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

HERROR rest_datatype_DejsonizeTerTuningParam (HxJSON_t *pstJson, DxTerTuningInfo_t *pstTuneInfo, HBOOL bClearObj)
{
	if (NULL == pstTuneInfo)
	{
		HxLOG_Error("pstTuneInfo NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstTuneInfo, 0, sizeof(DxTerTuningInfo_t));
	}

	(void)rest_datatype_GetJsonEnum(pstJson, "deliveryType", DxNAMEOF(DxDeliveryType_e), (HINT32 *)&(pstTuneInfo->deliveryType));
	(void)rest_datatype_GetJsonInteger(pstJson, "frequency", (HINT32 *)&(pstTuneInfo->ulFrequency));
	(void)rest_datatype_GetJsonEnum(pstJson, "bandwidth", DxNAMEOF(DxTer_BandWidth_e), (HINT32 *)&(pstTuneInfo->eBandWidth));
	(void)rest_datatype_GetJsonEnum(pstJson, "constellation", DxNAMEOF(DxTer_Constellation_e), (HINT32 *)&(pstTuneInfo->eConstellation));
	(void)rest_datatype_GetJsonEnum(pstJson, "hierarchy", DxNAMEOF(DxTer_Hierachy_e), (HINT32 *)&(pstTuneInfo->eHierachy));
	(void)rest_datatype_GetJsonEnum(pstJson, "codeRate", DxNAMEOF(DxTer_CodeRate_e), (HINT32 *)&(pstTuneInfo->eCodeRate));
	(void)rest_datatype_GetJsonEnum(pstJson, "transMode", DxNAMEOF(DxTer_TransMode_e), (HINT32 *)&(pstTuneInfo->eTransMode));
	(void)rest_datatype_GetJsonEnum(pstJson, "guardInterval", DxNAMEOF(DxTer_GuardInterval_e), (HINT32 *)&(pstTuneInfo->eGuardInterval));
	(void)rest_datatype_GetJsonEnum(pstJson, "stream", DxNAMEOF(DxTer_Stream_e), (HINT32 *)&(pstTuneInfo->eStream));
	(void)rest_datatype_GetJsonEnum(pstJson, "offset", DxNAMEOF(DxTer_Offset_e), (HINT32 *)&(pstTuneInfo->eOffset));
	(void)rest_datatype_GetJsonEnum(pstJson, "system", DxNAMEOF(DxTer_System_e), (HINT32 *)&(pstTuneInfo->eSystem));

	if (eDxTER_DVB_T2 == pstTuneInfo->eSystem)
	{
		DxTer_T2_Param_t		*pstT2Param = &(pstTuneInfo->stT2Param);

		(void)rest_datatype_GetJsonInteger(pstJson, "plpId", (HINT32 *)&(pstT2Param->ulPlpId));
		(void)rest_datatype_GetJsonEnum(pstJson, "preambleFormat", DxNAMEOF(DxTer_PreambleFormat_e), (HINT32 *)&(pstT2Param->preambleFormat));
		(void)rest_datatype_GetJsonEnum(pstJson, "mixed", DxNAMEOF(DxTer_Mixed_e), (HINT32 *)&(pstT2Param->mixed));
		(void)rest_datatype_GetJsonEnum(pstJson, "pilotPattern", DxNAMEOF(DxTer_Pilot_Pattern_e), (HINT32 *)&(pstT2Param->pilotPattern));
		(void)rest_datatype_GetJsonEnum(pstJson, "extendedCarrier", DxNAMEOF(DxTer_ExtendedCarrier_e), (HINT32 *)&(pstT2Param->extenedCarrier));
		(void)rest_datatype_GetJsonEnum(pstJson, "parpReduction", DxNAMEOF(DxTer_PAPR_Reduction_e), (HINT32 *)&(pstT2Param->PAPRreduction));
		(void)rest_datatype_GetJsonInteger(pstJson, "numPlpBlocks", (HINT32 *)&(pstT2Param->numPlpBlocks));
	}

	return ERR_OK;	
}

HBOOL rest_datatype_CheckAntennaTuningParamSame (DxAntInfo_t *pstAnt1, DxAntInfo_t *pstAnt2)
{
	if (pstAnt1->antType != pstAnt2->antType)
	{
		return FALSE;
	}

	switch (pstAnt1->antType)
	{
	case eDxANT_TYPE_LNB_ONLY:
		{
			DxLnbAntenna_t			*pstLnb1 = &(pstAnt1->info.lnb);
			DxLnbAntenna_t			*pstLnb2 = &(pstAnt2->info.lnb);

			if ((pstLnb1->ulLnbFreq != pstLnb2->ulLnbFreq) ||
				(pstLnb1->lnbVoltage != pstLnb2->lnbVoltage))
			{
				return FALSE;
			}
		}

		break;

	case eDxANT_TYPE_DISEQC:
		{
			DxDiseqcAntenna_t		*pstDiseqc1 = &(pstAnt1->info.diseqc);
			DxDiseqcAntenna_t		*pstDiseqc2 = &(pstAnt2->info.diseqc);

			if ((pstDiseqc1->ulLnbFreq != pstDiseqc2->ulLnbFreq) ||
				(pstDiseqc1->lnbVoltage != pstDiseqc2->lnbVoltage))
			{
				return FALSE;
			}

			if ((pstDiseqc1->ulLnbFreq != DxSAT_UNIVERSAL_LNB_FREQ) &&
				(pstDiseqc1->b22kTone != pstDiseqc2->b22kTone))
			{
				return FALSE;
			}

			if ((pstDiseqc1->diseqcVersion != pstDiseqc2->diseqcVersion) ||
				(pstDiseqc1->toneBurst != pstDiseqc2->toneBurst))
			{
				return FALSE;
			}

			switch (pstDiseqc1->diseqcVersion)
			{
			case eDxDISEQC_VER_1_2:
				if (pstDiseqc1->ucMotorPosition != pstDiseqc2->ucMotorPosition)
				{
					return FALSE;
				}

				break;

			case eDxDISEQC_VER_1_3:
				if (pstDiseqc1->sSatLonggitude != pstDiseqc2->sSatLonggitude)
				{
					return FALSE;
				}

				break;

			case eDxDISEQC_VER_1_0:
			case eDxDISEQC_VER_2_0:
			default:
				if (pstDiseqc1->diseqcInput != pstDiseqc2->diseqcInput)
				{
					return FALSE;
				}

				break;
			}
		}

		break;

	case eDxANT_TYPE_SCD:
		{
			DxScdAntenna_t			*pstScd1 = &(pstAnt1->info.scd);
			DxScdAntenna_t			*pstScd2 = &(pstAnt2->info.scd);

			if ((pstScd1->ulLnbFreq != pstScd2->ulLnbFreq) ||
				(pstScd1->lnbVoltage != pstScd2->lnbVoltage) ||
				(pstScd1->b22kTone != pstScd2->b22kTone))
			{
				return FALSE;
			}

			// except. scdInput
			if ((pstScd1->ulScdUserBandNum != pstScd2->ulScdUserBandNum) ||
				(pstScd1->ulScdUserBandFreq != pstScd2->ulScdUserBandFreq) ||
				(pstScd1->ulScdUserBandNum1 != pstScd2->ulScdUserBandNum1) ||
				(pstScd1->ulScdUserBandFreq1 != pstScd2->ulScdUserBandFreq1) ||
				(pstScd1->ulScdUserBandNum2 != pstScd2->ulScdUserBandNum2) ||
				(pstScd1->ulScdUserBandFreq2 != pstScd2->ulScdUserBandFreq2) ||
				(pstScd1->ulScdUserBandNum3 != pstScd2->ulScdUserBandNum3) ||
				(pstScd1->ulScdUserBandFreq3 != pstScd2->ulScdUserBandFreq3))
			{
				return FALSE;
			}
		}

		break;

	default:
		return FALSE;
	}

	return TRUE;
}

HBOOL rest_datatype_CheckSatTuningParamSame (DxSatTuningInfo_t *pstTune1, DxSatTuningInfo_t *pstTune2)
{
	HINT32						 nFreqDiff;
	DxAntInfo_t					*pstAnt1 = &(pstTune1->antInfo);
	DxAntInfo_t					*pstAnt2 = &(pstTune2->antInfo);
	DxSat_TuningTPInfo_t		*pstTpInfo1 = &(pstTune1->tuningInfo);
	DxSat_TuningTPInfo_t		*pstTpInfo2 = &(pstTune2->tuningInfo);

	if (rest_datatype_CheckAntennaTuningParamSame(pstAnt1, pstAnt2) != TRUE)
	{
		return FALSE;
	}

	if ((pstTpInfo1->ulSymbolRate != pstTpInfo2->ulSymbolRate) ||
		(pstTpInfo1->eTransSpec != pstTpInfo2->eTransSpec) )

/*
 * author : hsseo
 * note : rollOff 값이 DB, 값 convert 중에 어긋나는 부분이 존재. 항상 FALSE
 	      추후 삭제 필요
 */
#if 0
		(pstTpInfo1->rollOff != pstTpInfo2->rollOff))
#endif
	{
		return FALSE;
	}

	if ((eDxSAT_CODERATE_AUTO != pstTpInfo1->eFecCodeRate) && (eDxSAT_CODERATE_AUTO != pstTpInfo2->eFecCodeRate))
	{
		if (pstTpInfo1->eFecCodeRate != pstTpInfo2->eFecCodeRate)
		{
			return FALSE;
		}
	}

	if ((eDxSAT_POLAR_AUTO != pstTpInfo1->ePolarization) && (eDxSAT_POLAR_AUTO != pstTpInfo2->ePolarization))
	{
		if (pstTpInfo1->ePolarization != pstTpInfo2->ePolarization)
		{
			return FALSE;
		}
	}

	if ((eDxSAT_PSK_AUTO != pstTpInfo1->ePskMod) && (eDxSAT_PSK_AUTO != pstTpInfo2->ePskMod))
	{
		if (pstTpInfo1->ePskMod != pstTpInfo2->ePskMod)
		{
			return FALSE;
		}
	}

	if ((eDxSAT_PILOT_AUTO != pstTpInfo1->ePilot) && (eDxSAT_PILOT_AUTO != pstTpInfo2->ePilot))
	{
		if (pstTpInfo1->ePilot != pstTpInfo2->ePilot)
		{
			return FALSE;
		}
	}

	//////////////// Frequency 비교 ////////////////
	//		45~18Ms/s	=	+/- (8MHz) search
	//		18 ~ 11Ms/s =	+/- (6MHz) search
	//		11 ~ 8Ms/s	=	+/- (5MHz) search
	//		8 ~ 4Ms/s	=	+/- (4MHz) search
	//		4 ~ 2Ms/s	=	+/- (3.8MHz) search
	//	=> 2003.09.09, 권기호 과장으로부터 입수
	nFreqDiff = (HINT32)pstTpInfo1->ulFrequency - (HINT32)pstTpInfo2->ulFrequency;

	if (pstTpInfo1->ulSymbolRate >= 18000)
	{
		if ((nFreqDiff >= 8) || (nFreqDiff <= -8))
		{
			return FALSE;
		}
	}
	else if (pstTpInfo1->ulSymbolRate >= 11000)
	{
		if ((nFreqDiff >= 6) || (nFreqDiff <= -6))
		{
			return FALSE;
		}
	}
	else if (pstTpInfo1->ulSymbolRate >= 8000)
	{
		if ((nFreqDiff >= 5) || (nFreqDiff <= -5))
		{
			return FALSE;
		}
	}
	else if (pstTpInfo1->ulSymbolRate >= 2000)
	{
		if ((nFreqDiff >= 4) || (nFreqDiff <= -4))
		{
			return FALSE;
		}
	}
	else
	{
		if (nFreqDiff != 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}

HBOOL rest_datatype_CheckCabTuningParamSame (DxCabTuningInfo_t *pstTune1, DxCabTuningInfo_t *pstTune2)
{
	HINT32						 nFreqDiff;

	if (pstTune1->ulSymbolRate != pstTune2->ulSymbolRate)
	{
		return FALSE;
	}

	if ((eDxCAB_CONSTELLATION_AUTO != pstTune1->constellation) && (eDxCAB_CONSTELLATION_AUTO != pstTune2->constellation))
	{
		if (pstTune1->constellation != pstTune2->constellation)
		{
			return FALSE;
		}
	}

	if ((eDxCAB_SPECTRUM_AUTO != pstTune1->spectrum) && (eDxCAB_SPECTRUM_AUTO != pstTune2->spectrum))
	{
		if (pstTune1->spectrum != pstTune2->spectrum)
		{
			return FALSE;
		}
	}

	nFreqDiff = (HINT32)pstTune1->ulFrequency - (HINT32)pstTune2->ulFrequency;
	if ((nFreqDiff > 32) || (nFreqDiff < -32))
	{
		return FALSE;
	}

	return TRUE;
}

HBOOL rest_datatype_CheckTerTuningParamSame (DxTerTuningInfo_t *pstTune1, DxTerTuningInfo_t *pstTune2)
{
	HINT32						 nFreqDiff;

	if ((eDxTER_TRANSMODE_AUTO != pstTune1->eTransMode) && (eDxTER_TRANSMODE_AUTO != pstTune2->eTransMode))
	{
		if (pstTune1->eTransMode != pstTune2->eTransMode)
		{
			return FALSE;
		}
	}

	if ((eDxTER_GUARDINTERVAL_AUTO != pstTune1->eGuardInterval) && (eDxTER_GUARDINTERVAL_AUTO != pstTune2->eGuardInterval))
	{
		if (pstTune1->eGuardInterval != pstTune2->eGuardInterval)
		{
			return FALSE;
		}
	}

	if ((eDxTER_DVB_T2 == pstTune1->eSystem) && (0x0000FFFF != pstTune1->stT2Param.ulPlpId) &&
		(eDxTER_DVB_T2 == pstTune2->eSystem) && (0x0000FFFF != pstTune2->stT2Param.ulPlpId))
	{
		if (pstTune1->stT2Param.ulPlpId != pstTune2->stT2Param.ulPlpId)
		{
			return FALSE;
		}
	}

	nFreqDiff = (HINT32)pstTune1->ulFrequency - (HINT32)pstTune2->ulFrequency;
	if ((nFreqDiff > 3000) || (nFreqDiff < -3000))
	{
		return FALSE;
	}

	return TRUE;
}

HBOOL rest_datatype_CheckTuningParamSame (DxTuneParam_t *pstRfTune, DxTuneParam_t *pstTpTune)
{
	switch (pstTpTune->eDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
		return rest_datatype_CheckSatTuningParamSame(&(pstRfTune->sat), &(pstTpTune->sat));

	case eDxDELIVERY_TYPE_CAB:
		return rest_datatype_CheckCabTuningParamSame(&(pstRfTune->cab), &(pstTpTune->cab));

	case eDxDELIVERY_TYPE_TER:
		return rest_datatype_CheckTerTuningParamSame(&(pstRfTune->ter), &(pstTpTune->ter));

	default:
		break;
	}

	return FALSE;
}

// Transponders Tunable:
HINT32 rest_datatype_JsonizeTpTunable (HINT32 nJsonWriter, RestTsTunable_t *pstTunableArray, HUINT32 ulArrayLen, HCHAR *szObjectName, HCHAR *szSelect)
{
	HINT32		 nWriter;
	HUINT32		 ulCnt;

	if ((NULL == pstTunableArray) || (0 == ulArrayLen))
	{
		HxLOG_Error("pstSetting NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	if (NULL == szObjectName)
	{
		// No Object: Nothing to do
	}
	else if ('\0' == *szObjectName)
	{
		// Make the object, but no name
		HxJSONWRITER_ArrayBegin(nWriter);
	}
	else
	{
		// Make the object
		HxJSONWRITER_Array(nWriter, szObjectName);
	}

	// Contents:
	{
		for (ulCnt = 0; ulCnt < ulArrayLen; ulCnt++)
		{
			RestTsTunable_t		*pstTunable = &(pstTunableArray[ulCnt]);

			HxJSONWRITER_ObjectBegin(nWriter);
			
			(void)rest_datatype_SetJsonInteger(nWriter, "tsUid", pstTunable->ulTsUid, NULL);
			(void)rest_datatype_SetJsonInteger(nWriter, "mainLive", pstTunable->abTunable[eTUNE_CHECK_DST_LiveMain], szSelect);
			(void)rest_datatype_SetJsonInteger(nWriter, "record", pstTunable->abTunable[eTUNE_CHECK_DST_Rec], szSelect);
			(void)rest_datatype_SetJsonInteger(nWriter, "livestream", pstTunable->abTunable[eTUNE_CHECK_DST_LiveStream], szSelect);

			HxJSONWRITER_ObjectEnd(nWriter);
		}
	}

	if (NULL != szObjectName)
	{
		HxJSONWRITER_ArrayEnd(nWriter);
	}

	return nWriter;
}


#define _____ANTENNA_____
HINT32 rest_datatype_JsonizeAntenna (HINT32 nJsonWriter, DxAntInfo_t *pstAntInfo, HCHAR *szSelect)
{
	if (NULL == pstAntInfo)
	{
		HxLOG_Error("pstAntInfo NULL\n");
		return nJsonWriter;
	}

	switch(pstAntInfo->antType)
	{
	case eDxANT_TYPE_LNB_ONLY:
		return rest_datatype_JsonizeLnbAntenna(nJsonWriter, pstAntInfo, szSelect);

	case eDxANT_TYPE_DISEQC:
		if ((eDxDISEQC_VER_1_2 == pstAntInfo->info.diseqc.diseqcVersion) ||
			(eDxDISEQC_VER_1_3 == pstAntInfo->info.diseqc.diseqcVersion))
		{
			return rest_datatype_JsonizeMotorAntenna(nJsonWriter, pstAntInfo, szSelect);
		}
		else
		{
			return rest_datatype_JsonizeDiseqcAntenna(nJsonWriter, pstAntInfo, szSelect);
		}

		break;

	case eDxANT_TYPE_SCD:
		return rest_datatype_JsonizeScdAntenna(nJsonWriter, pstAntInfo, szSelect);

	default:
		break;		
	}

	return nJsonWriter;
}

HERROR rest_datatype_DejsonizeAntenna (HxJSON_t *pstJson, DxAntInfo_t *pstAntInfo, HBOOL bClearObj)
{
	HCHAR			 szAntType[32];

	if (NULL == pstAntInfo)
	{
		HxLOG_Error("pstTuneParam NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstAntInfo, 0, sizeof(DxAntInfo_t));
	}

	(void)rest_datatype_GetJsonInteger(pstJson, "antennaUid", (HINT32 *)&(pstAntInfo->uid));
	(void)rest_datatype_GetJsonString(pstJson, "satelliteName", pstAntInfo->satName, DxANTINFO_NAME_LEN);
	(void)rest_datatype_GetJsonString(pstJson, "antennaType", szAntType, 32);

	if (HxSTD_StrCmp(szAntType, "lnb") == 0)
	{
		pstAntInfo->antType = eDxANT_TYPE_LNB_ONLY;
		return rest_datatype_DejsonizeLnbAntenna(pstJson, &(pstAntInfo->info.lnb), FALSE); 
	}
	else if (HxSTD_StrCmp(szAntType, "diseqc") == 0)
	{
		pstAntInfo->antType = eDxANT_TYPE_DISEQC;
		return rest_datatype_DejsonizeDiseqcAntenna(pstJson, &(pstAntInfo->info.diseqc), FALSE); 
	}
	else if (HxSTD_StrCmp(szAntType, "motor") == 0)
	{
		pstAntInfo->antType = eDxANT_TYPE_DISEQC;
		return rest_datatype_DejsonizeMotorAntenna(pstJson, &(pstAntInfo->info.diseqc), FALSE); 
	}
	else if (HxSTD_StrCmp(szAntType, "scd") == 0)
	{
		pstAntInfo->antType = eDxANT_TYPE_SCD;
		return rest_datatype_DejsonizeScdAntenna(pstJson, &(pstAntInfo->info.scd), FALSE); 
	}

	return ERR_FAIL;
}

HINT32 rest_datatype_JsonizeLnbAntenna (HINT32 nJsonWriter, DxAntInfo_t *pstAntInfo, HCHAR *szSelect)
{
	HINT32					 nWriter;

	if (NULL == pstAntInfo)
	{
		HxLOG_Error("pstAntInfo NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		DxLnbAntenna_t		*pstLnb = &(pstAntInfo->info.lnb);

		(void)rest_datatype_SetJsonInteger(nWriter, "antennaUid", (HINT32)pstAntInfo->uid, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "satelliteName", pstAntInfo->satName, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "antennaType", "lnb", szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "lnbFrequency", (HINT32)pstLnb->ulLnbFreq, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "lnbVoltage", DxNAMEOF(DxSAT_LnbVoltage_e), (HINT32)pstLnb->lnbVoltage, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

HERROR rest_datatype_DejsonizeLnbAntenna (HxJSON_t *pstJson, DxLnbAntenna_t *pstLnbAnt, HBOOL bClearObj)
{
	if (NULL == pstLnbAnt)
	{
		HxLOG_Error("pstLnbAnt NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstLnbAnt, 0, sizeof(DxLnbAntenna_t));
	}

	(void)rest_datatype_GetJsonInteger(pstJson, "lnbFrequency", (HINT32 *)&(pstLnbAnt->ulLnbFreq));
	(void)rest_datatype_GetJsonEnum(pstJson, "lnbVoltage", DxNAMEOF(DxSAT_LnbVoltage_e), (HINT32 *)&(pstLnbAnt->lnbVoltage));

	return ERR_OK;	
}

HINT32 rest_datatype_JsonizeDiseqcAntenna (HINT32 nJsonWriter, DxAntInfo_t *pstAntInfo, HCHAR *szSelect)
{
	HINT32					 nWriter;

	if (NULL == pstAntInfo)
	{
		HxLOG_Error("pstAntInfo NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		DxDiseqcAntenna_t		*pstDiseqc = &(pstAntInfo->info.diseqc);

		(void)rest_datatype_SetJsonInteger(nWriter, "antennaUid", (HINT32)pstAntInfo->uid, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "satelliteName", pstAntInfo->satName, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "antennaType", "diseqc", szSelect);
		
		(void)rest_datatype_SetJsonInteger(nWriter, "lnbFrequency", (HINT32)pstDiseqc->ulLnbFreq, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "lnbVoltage", DxNAMEOF(DxSAT_LnbVoltage_e), (HINT32)pstDiseqc->lnbVoltage, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "22kTone", (HINT32)pstDiseqc->b22kTone, szSelect);

		(void)rest_datatype_SetJsonEnum(nWriter, "diseqcVer", DxNAMEOF(DxSAT_DiseqcVer_e), (HINT32)pstDiseqc->diseqcVersion, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "diseqcInput", DxNAMEOF(DxSAT_DiseqcInput_e), (HINT32)pstDiseqc->diseqcInput, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "toneBurst", DxNAMEOF(DxSAT_ToneBurst_e), (HINT32)pstDiseqc->toneBurst, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

HERROR rest_datatype_DejsonizeDiseqcAntenna (HxJSON_t *pstJson, DxDiseqcAntenna_t *pstDiseqcAnt, HBOOL bClearObj)
{
	if (NULL == pstDiseqcAnt)
	{
		HxLOG_Error("pstDiseqcAnt NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstDiseqcAnt, 0, sizeof(DxDiseqcAntenna_t));
	}

	(void)rest_datatype_GetJsonInteger(pstJson, "lnbFrequency", (HINT32 *)&(pstDiseqcAnt->ulLnbFreq));
	(void)rest_datatype_GetJsonEnum(pstJson, "lnbVoltage", DxNAMEOF(DxSAT_LnbVoltage_e), (HINT32 *)&(pstDiseqcAnt->lnbVoltage));
	(void)rest_datatype_GetJsonInteger(pstJson, "22kTone", (HINT32 *)&(pstDiseqcAnt->b22kTone));
	(void)rest_datatype_GetJsonEnum(pstJson, "diseqcVer", DxNAMEOF(DxSAT_DiseqcVer_e), (HINT32 *)&(pstDiseqcAnt->diseqcVersion));
	(void)rest_datatype_GetJsonEnum(pstJson, "diseqcInput", DxNAMEOF(DxSAT_DiseqcInput_e), (HINT32 *)&(pstDiseqcAnt->diseqcInput));
	(void)rest_datatype_GetJsonEnum(pstJson, "toneBurst", DxNAMEOF(DxSAT_ToneBurst_e), (HINT32 *)&(pstDiseqcAnt->toneBurst));

	return ERR_OK;	
}

HINT32 rest_datatype_JsonizeMotorAntenna (HINT32 nJsonWriter, DxAntInfo_t *pstAntInfo, HCHAR *szSelect)
{
	HINT32					 nWriter;

	if (NULL == pstAntInfo)
	{
		HxLOG_Error("pstAntInfo NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		DxDiseqcAntenna_t		*pstDiseqc = &(pstAntInfo->info.diseqc);

		(void)rest_datatype_SetJsonInteger(nWriter, "antennaUid", (HINT32)pstAntInfo->uid, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "satelliteName", pstAntInfo->satName, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "antennaType", "diseqc", szSelect);
		
		(void)rest_datatype_SetJsonInteger(nWriter, "lnbFrequency", (HINT32)pstDiseqc->ulLnbFreq, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "lnbVoltage", DxNAMEOF(DxSAT_LnbVoltage_e), (HINT32)pstDiseqc->lnbVoltage, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "22kTone", (HINT32)pstDiseqc->b22kTone, szSelect);

		(void)rest_datatype_SetJsonEnum(nWriter, "diseqcVer", DxNAMEOF(DxSAT_DiseqcVer_e), (HINT32)pstDiseqc->diseqcVersion, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "toneBurst", DxNAMEOF(DxSAT_ToneBurst_e), (HINT32)pstDiseqc->toneBurst, szSelect);

		(void)rest_datatype_SetJsonInteger(nWriter, "motorPosition", (HINT32)pstDiseqc->ucMotorPosition, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "satelliteLongitude", (HINT32)pstDiseqc->sSatLonggitude, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

HERROR rest_datatype_DejsonizeMotorAntenna (HxJSON_t *pstJson, DxDiseqcAntenna_t *pstDiseqcAnt, HBOOL bClearObj)
{
	if (NULL == pstDiseqcAnt)
	{
		HxLOG_Error("pstDiseqcAnt NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstDiseqcAnt, 0, sizeof(DxDiseqcAntenna_t));
	}

	(void)rest_datatype_GetJsonInteger(pstJson, "lnbFrequency", (HINT32 *)&(pstDiseqcAnt->ulLnbFreq));
	(void)rest_datatype_GetJsonEnum(pstJson, "lnbVoltage", DxNAMEOF(DxSAT_LnbVoltage_e), (HINT32 *)&(pstDiseqcAnt->lnbVoltage));
	(void)rest_datatype_GetJsonInteger(pstJson, "22kTone", (HINT32 *)&(pstDiseqcAnt->b22kTone));
	(void)rest_datatype_GetJsonEnum(pstJson, "diseqcVer", DxNAMEOF(DxSAT_DiseqcVer_e), (HINT32 *)&(pstDiseqcAnt->diseqcVersion));
	(void)rest_datatype_GetJsonEnum(pstJson, "toneBurst", DxNAMEOF(DxSAT_ToneBurst_e), (HINT32 *)&(pstDiseqcAnt->toneBurst));
	(void)rest_datatype_GetJsonInteger(pstJson, "motorPosition", (HINT32 *)&(pstDiseqcAnt->ucMotorPosition));
	(void)rest_datatype_GetJsonInteger(pstJson, "satelliteLongitude", (HINT32 *)&(pstDiseqcAnt->sSatLonggitude));

	return ERR_OK;	
}

HINT32 rest_datatype_JsonizeScdAntenna (HINT32 nJsonWriter, DxAntInfo_t *pstAntInfo, HCHAR *szSelect)
{
	HINT32					 nWriter;

	if (NULL == pstAntInfo)
	{
		HxLOG_Error("pstAntInfo NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		DxScdAntenna_t		*pstScd = &(pstAntInfo->info.scd);

		(void)rest_datatype_SetJsonInteger(nWriter, "antennaUid", (HINT32)pstAntInfo->uid, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "satelliteName", pstAntInfo->satName, szSelect);
		(void)rest_datatype_SetJsonString(nWriter, "antennaType", "diseqc", szSelect);
		
		(void)rest_datatype_SetJsonInteger(nWriter, "lnbFrequency", (HINT32)pstScd->ulLnbFreq, szSelect);
		(void)rest_datatype_SetJsonEnum(nWriter, "lnbVoltage", DxNAMEOF(DxSAT_LnbVoltage_e), (HINT32)pstScd->lnbVoltage, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "22kTone", (HINT32)pstScd->b22kTone, szSelect);

		(void)rest_datatype_SetJsonEnum(nWriter, "scdInput", DxNAMEOF(DxSAT_ScdInput_e), (HINT32)pstScd->scdInput, szSelect);

		(void)rest_datatype_SetJsonInteger(nWriter, "scdUserBandNum0", (HINT32)pstScd->ulScdUserBandNum, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "scdUserBandFreq0", (HINT32)pstScd->ulScdUserBandFreq, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "scdUserBandNum1", (HINT32)pstScd->ulScdUserBandNum1, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "scdUserBandFreq1", (HINT32)pstScd->ulScdUserBandFreq1, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "scdUserBandNum2", (HINT32)pstScd->ulScdUserBandNum2, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "scdUserBandFreq2", (HINT32)pstScd->ulScdUserBandFreq2, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "scdUserBandNum3", (HINT32)pstScd->ulScdUserBandNum3, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "scdUserBandFreq3", (HINT32)pstScd->ulScdUserBandFreq3, szSelect);
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

HERROR rest_datatype_DejsonizeScdAntenna (HxJSON_t *pstJson, DxScdAntenna_t *pstScdAnt, HBOOL bClearObj)
{
	if (NULL == pstScdAnt)
	{
		HxLOG_Error("pstDiseqcAnt NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		HxSTD_MemSet(pstScdAnt, 0, sizeof(DxScdAntenna_t));
	}

	(void)rest_datatype_GetJsonInteger(pstJson, "lnbFrequency", (HINT32 *)&(pstScdAnt->ulLnbFreq));
	(void)rest_datatype_GetJsonEnum(pstJson, "lnbVoltage", DxNAMEOF(DxSAT_LnbVoltage_e), (HINT32 *)&(pstScdAnt->lnbVoltage));
	(void)rest_datatype_GetJsonInteger(pstJson, "22kTone", (HINT32 *)&(pstScdAnt->b22kTone));
	(void)rest_datatype_GetJsonEnum(pstJson, "scdInput", DxNAMEOF(DxSAT_ScdInput_e), (HINT32 *)&(pstScdAnt->scdInput));
	(void)rest_datatype_GetJsonInteger(pstJson, "scdUserBandNum0", (HINT32 *)&(pstScdAnt->ulScdUserBandNum));
	(void)rest_datatype_GetJsonInteger(pstJson, "scdUserBandFreq0", (HINT32 *)&(pstScdAnt->ulScdUserBandFreq));
	(void)rest_datatype_GetJsonInteger(pstJson, "scdUserBandNum1", (HINT32 *)&(pstScdAnt->ulScdUserBandNum1));
	(void)rest_datatype_GetJsonInteger(pstJson, "scdUserBandFreq1", (HINT32 *)&(pstScdAnt->ulScdUserBandFreq1));
	(void)rest_datatype_GetJsonInteger(pstJson, "scdUserBandNum2", (HINT32 *)&(pstScdAnt->ulScdUserBandNum2));
	(void)rest_datatype_GetJsonInteger(pstJson, "scdUserBandFreq2", (HINT32 *)&(pstScdAnt->ulScdUserBandFreq2));
	(void)rest_datatype_GetJsonInteger(pstJson, "scdUserBandNum3", (HINT32 *)&(pstScdAnt->ulScdUserBandNum3));
	(void)rest_datatype_GetJsonInteger(pstJson, "scdUserBandFreq3", (HINT32 *)&(pstScdAnt->ulScdUserBandFreq3));

	return ERR_OK;	
}

#define _____CHANNEL_LIST_STATUS_____
HINT32 rest_datatype_JsonizeChanListStatus (HINT32 nJsonWriter, RestChanListStatus_t *pstStatus, HCHAR *szSelect)
{
	HINT32					 nWriter;

	if (NULL == pstStatus)
	{
		HxLOG_Error("pstStatus NULL\n");
		return nJsonWriter;
	}

	nWriter = (0 == nJsonWriter) ? HLIB_JSONWRITER_Open() : nJsonWriter;
	if (0 == nWriter)
	{
		HxLOG_Error(">> HLIB_JSONWRITER_Open failed:\n");
		return 0;
	}

	HxJSONWRITER_ObjectBegin(nWriter);
	{
		(void)rest_datatype_SetJsonInteger(nWriter, "allChannelNumber", (HINT32)pstStatus->ulSvcAllNum, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "tvChannelNumber", (HINT32)pstStatus->ulSvcTvNum, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "radioChannelNumber", (HINT32)pstStatus->ulSvcRadioNum, szSelect);
		(void)rest_datatype_SetJsonInteger(nWriter, "maxTransferableChannel", REST_BIND_CHANNEL_TRANSFER_MAX, szSelect);

		if (pstStatus->nMinTvLcn >= 0)
		{
			(void)rest_datatype_SetJsonInteger(nWriter, "minTvChannelNo", pstStatus->nMinTvLcn, szSelect);
		}
		
		if (pstStatus->nMaxTvLcn >= 0)
		{
			(void)rest_datatype_SetJsonInteger(nWriter, "maxTvChannelNo", pstStatus->nMaxTvLcn, szSelect);
		}
		
		if (pstStatus->nMinRadioLcn >= 0)
		{
			(void)rest_datatype_SetJsonInteger(nWriter, "minRadioChannelNo", pstStatus->nMinRadioLcn, szSelect);
		}
		
		if (pstStatus->nMaxRadioLcn >= 0)
		{
			(void)rest_datatype_SetJsonInteger(nWriter, "maxRadioChannelNo", pstStatus->nMaxRadioLcn, szSelect);
		}
	}
	HxJSONWRITER_ObjectEnd(nWriter);

	return nWriter;	
}

#define _____INITIALIZATION_____
HERROR rest_datatype_InitChannel (void)
{
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SvcType);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_DeliType);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_VideoType);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_VideoCodec);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_AudioCodec);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_CasType);
	
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SatTransportSpec);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SatPskModulation);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SatPolarization);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SatCodeRate);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SatLnbVolt);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SatDiseqcVer);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SatDiseqcInput);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_SatToneBurst);
	
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_CabConstellation);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_CabSpectrum);
	
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerBandwidth);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerConstellation);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerHierarchy);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerCodeRate);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerTransMode);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerGuardInterval);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerStream);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerOffset);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerSystem);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerPreambleFormat);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerMixed);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerPilotPattern);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerExtendedCarrier);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_TerPaprReduction);

	return ERR_OK;
}


#define ___GLOBAL_FUNCTIONS___

