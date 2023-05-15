
#include <hlib.h>
#include "_dlib_port_tuneparam.h"

static	DxEnumStringTable_t	_stSATCodeRateTable =
{
	(HCHAR*)"DxSat_CodeRate_e",
	{
		{	"auto",		eDxSAT_CODERATE_AUTO},
		{	"none",		eDxSAT_CODERATE_NONE},
		{	"1/2",		eDxSAT_CODERATE_1_2 },
		{	"2/3",		eDxSAT_CODERATE_2_3	},
		{	"3/4",		eDxSAT_CODERATE_3_4	},
		{	"5/6",		eDxSAT_CODERATE_5_6	},
		{	"7/8",		eDxSAT_CODERATE_7_8	},
		{	"3/5",		eDxSAT_CODERATE_3_5	},
		{	"4/5",		eDxSAT_CODERATE_4_5 },
		{	"5/11",		eDxSAT_CODERATE_5_11},
		{	"6/7",		eDxSAT_CODERATE_6_7	},
		{	"8/9",		eDxSAT_CODERATE_8_9	},
		{	"9/10",		eDxSAT_CODERATE_9_10},
		{	NULL,					0					}
	}
};

static	DxEnumStringTable_t	_stSATPolarizationTable =
{
	(HCHAR*)"DxSat_Polarization_e",
	{
		{	"auto",		eDxSAT_POLAR_AUTO		},
		{	"hor",		eDxSAT_POLAR_HOR		},
		{	"ver",		eDxSAT_POLAR_VER		},
		{	"left",		eDxSAT_POLAR_LEFT		},
		{	"right",	eDxSAT_POLAR_RIGHT		},
		{	NULL,							0						}
	}
};

static	DxEnumStringTable_t	_stSATTransportSpecTable =
{
	(HCHAR*)"DxSat_TransportSpec_e",
	{
		{	"dvbs",		eDxSAT_TRANS_DVBS		},
		{	"dvbs2",	eDxSAT_TRANS_DVBS2		},
		{	NULL,							0						}
	}
};

static	DxEnumStringTable_t	_stSATPSKModulationTable =
{
	(HCHAR*)"DxSat_PskModulation_e",
	{
		{	"auto",		eDxSAT_PSK_AUTO 		},
		{	"qpsk", 	eDxSAT_PSK_QPSK 		},
		{	"8psk", 	eDxSAT_PSK_8PSK 		},
		{	NULL,							0						}
	}
};

static	DxEnumStringTable_t	_stSATPilotTable =
{
	(HCHAR*)"DxSat_Pilot_e",
	{
		{	"auto",		eDxSAT_PILOT_AUTO		},
		{	"off",		eDxSAT_PILOT_OFF		},
		{	"on", 		eDxSAT_PILOT_ON 		},
		{	NULL,							0						}
	}
};

static	DxEnumStringTable_t	_stSATRollOffTable =
{
	(HCHAR*)"DxSat_RollOff_e",
	{
		{	"0.20", 	eDxSAT_ROLL_020 		},
		{	"0.25", 	eDxSAT_ROLL_025 		},
		{	"0.35", 	eDxSAT_ROLL_035 		},
		{	NULL,							0						}
	}
};

static	DxEnumStringTable_t	_stCABContellationTable =
{
	(HCHAR*)"DxCab_Constellation_e",
	{
		{	"auto",		eDxCAB_CONSTELLATION_AUTO	},
		{	"16qam",	eDxCAB_CONSTELLATION_16QAM	},
		{	"32qam",	eDxCAB_CONSTELLATION_32QAM	},
		{	"64qam",	eDxCAB_CONSTELLATION_64QAM	},
		{	"128qam",	eDxCAB_CONSTELLATION_128QAM	},
		{	"256qam",	eDxCAB_CONSTELLATION_256QAM	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stCABSpectrumTable =
{
	(HCHAR*)"DxCab_Spectrum_e",
	{
		{	"auto",		eDxCAB_SPECTRUM_AUTO	},
		{	"normal",	eDxCAB_SPECTRUM_NORMAL	},
		{	"inverted",	eDxCAB_SPECTRUM_INVERTED	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTERBandwidthTable =
{
	(HCHAR*)"DxTer_BandWidth_e",
	{
		{	"bw8m",		eDxTER_BANDWIDTH_8M	},
		{	"bw7m",		eDxTER_BANDWIDTH_7M	},
		{	"bw6m",		eDxTER_BANDWIDTH_6M	},
		{	"t2bw5m",	eDxTER_T2BANDWIDTH_5MHZ	},
		{	"t2bw10m",	eDxTER_T2BANDWIDTH_10MHZ	},
		{	"t2bw1p7m",	eDxTER_T2BANDWIDTH_1P7MHZ	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTERContellationTable =
{
	(HCHAR*)"DxTer_Constellation_e",
	{
		{	"auto",		eDxTER_CONSTELLATION_AUTO	},
		{	"qpsk",		eDxTER_CONSTELLATION_QPSK	},
		{	"16qam",	eDxTER_CONSTELLATION_16QAM	},
		{	"64qam",	eDxTER_CONSTELLATION_64QAM	},
		{	"256qam",	eDxTER_CONSTELLATION_256QAM	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTERHierachyTable =
{
	(HCHAR*)"DxTer_Hierachy_e",
	{
		{	"auto",	eDxTER_HIERACHY_AUTO	},
		{	"none",	eDxTER_HIERACHY_NONE	},
		{	"1",	eDxTER_HIERACHY_1	},
		{	"2",	eDxTER_HIERACHY_2	},
		{	"4",	eDxTER_HIERACHY_4	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTERCodeRateTable =
{
	(HCHAR*)"DxTer_CodeRate_e",
	{
		{	"auto",	eDxTER_CODERATE_AUTO	},
		{	"1/2",	eDxTER_CODERATE_1_2	},
		{	"2/3",	eDxTER_CODERATE_2_3	},
		{	"3/4",	eDxTER_CODERATE_3_4	},
		{	"5/6",	eDxTER_CODERATE_5_6	},
		{	"7/8",	eDxTER_CODERATE_7_8	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTERTransModeTable =
{
	(HCHAR*)"DxTer_TransMode_e",
	{
		{	"auto",		eDxTER_TRANSMODE_AUTO	},
		{	"2k",		eDxTER_TRANSMODE_2K	},
		{	"8k",		eDxTER_TRANSMODE_8K	},
		{	"t2-1k",	eDxTER_T2TRANSMODE_1K	},
		{	"t2-4k",	eDxTER_T2TRANSMODE_4K	},
		{	"t2-16k",	eDxTER_T2TRANSMODE_16K	},
		{	"t2-32k",	eDxTER_T2TRANSMODE_32K	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTERGuardIntervalTable =
{
	(HCHAR*)"DxTer_GuardInterval_e",
	{
		{	"auto",			eDxTER_GUARDINTERVAL_AUTO	},
		{	"1/4",			eDxTER_GUARDINTERVAL_1_4	},
		{	"1/8",			eDxTER_GUARDINTERVAL_1_8	},
		{	"1/16",			eDxTER_GUARDINTERVAL_1_16	},
		{	"1/32",			eDxTER_GUARDINTERVAL_1_32	},
		{	"t2-1/128",		eDxTER_T2GUARDINTERVAL_1_128	},
		{	"t2-19/128",	eDxTER_T2GUARDINTERVAL_19_128	},
		{	"t2-19/256",	eDxTER_T2GUARDINTERVAL_19_256	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTERStreamTable =
{
	(HCHAR*)"DxTer_Stream_e",
	{
		{	"hp",	eDxTER_STREAM_HP	},
		{	"lp",	eDxTER_STREAM_LP	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTEROffsetTable =
{
	(HCHAR*)"DxTer_Offset_e",
	{
		{	"auto",	eDxTER_OFFSET_AUTO	},
		{	"none",	eDxTER_OFFSET_NONE	},
		{	"1/6P",	eDxTER_OFFSET_1_6_P	},
		{	"1/6N",	eDxTER_OFFSET_1_6_N	},
		{	"2/6P",	eDxTER_OFFSET_2_6_P	},
		{	"2/6N",	eDxTER_OFFSET_2_6_N	},
		{	"3/6P",	eDxTER_OFFSET_3_6_P	},
		{	"3/6N",	eDxTER_OFFSET_3_6_N	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTERSystemTable =
{
	(HCHAR*)"DxTer_System_e",
	{
		{	"dvbt",		eDxTER_DVB_T	},
		{	"dvbt2",	eDxTER_DVB_T2	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTERPreambleFormatTable =
{
	(HCHAR*)"DxTer_PreambleFormat_e",
	{
		{	"siso",	eDxTER_SISO	},
		{	"miso",	eDxTER_MISO	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTERMixedTable =
{
	(HCHAR*)"DxTer_Mixed_e",
	{
		{	"off",		eDxTER_NOT_MIXED	},
		{	"mixed",	eDxTER_MIXED	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTERPilotPatternTable =
{
	(HCHAR*)"DxTer_Pilot_Pattern_e",
	{
		{	"auto",	eDxTER_PP_AUTO	},
		{	"pp1",	eDxTER_PP1	},
		{	"pp2",	eDxTER_PP2	},
		{	"pp3",	eDxTER_PP3	},
		{	"pp4",	eDxTER_PP4	},
		{	"pp5",	eDxTER_PP5	},
		{	"pp6",	eDxTER_PP6	},
		{	"pp7",	eDxTER_PP7	},
		{	"pp8",	eDxTER_PP8	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTERExtendedCarrierTable =
{
	(HCHAR*)"DxTer_ExtendedCarrier_e",
	{
		{	"notused",	eDxTER_T2NOT_USED	},
		{	"used",		eDxTER_T2USED	},
		{	NULL,																		0				}
	}
};

static	DxEnumStringTable_t	_stTERPAPRReductionTable =
{
	(HCHAR*)"DxTer_PAPR_Reduction_e",
	{
		{	"none",		eDxTER_REDUCTION_NONE	},
		{	"ace",		eDxTER_ACE	},
		{	"tr",		eDxTER_TR	},
		{	"ave-tr",	eDxTER_AVE_TR	},
		{	NULL,				0}
	}
};

static HERROR	dlib_tuneparam_Encoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HINT32			jw;
	DxTuneParam_t		*pstInfo = (DxTuneParam_t*)src;

	HxLOG_Assert(sizeof(DxTuneParam_t) == srcSize);

	jw = HLIB_JSONWRITER_Open();
	HxJSONWRITER_ObjectBegin(jw);
		HxJSONWRITER_String(jw, "eDeliType", dlib_ConvertEnum2Str(DxNAMEOF(DxDeliveryType_e), pstInfo->eDeliType));

		switch (pstInfo->eDeliType)
		{
			case eDxDELIVERY_TYPE_SAT:
				HxJSONWRITER_Object(jw, "sat");
					HxJSONWRITER_Integer(jw, "ulTunerGroupId", pstInfo->sat.ulTunerGroupId);
					HxJSONWRITER_String (jw, "antennaType", dlib_ConvertEnum2Str(DxNAMEOF(DxAntennaType_e), pstInfo->sat.antennaType));
					HxJSONWRITER_Object(jw, "tuningInfo");
						HxJSONWRITER_Integer(jw, "ulFrequency", pstInfo->sat.tuningInfo.ulFrequency);
						HxJSONWRITER_Integer(jw, "ulSymbolRate", pstInfo->sat.tuningInfo.ulSymbolRate);
						HxJSONWRITER_String (jw, "fecCodeRate", dlib_ConvertEnum2Str(DxNAMEOF(DxSat_CodeRate_e), pstInfo->sat.tuningInfo.eFecCodeRate));
						HxJSONWRITER_String (jw, "polarization", dlib_ConvertEnum2Str(DxNAMEOF(DxSat_Polarization_e), pstInfo->sat.tuningInfo.ePolarization));
						HxJSONWRITER_String (jw, "transSpec", dlib_ConvertEnum2Str(DxNAMEOF(DxSat_TransportSpec_e), pstInfo->sat.tuningInfo.eTransSpec));
						HxJSONWRITER_String (jw, "pskMod", dlib_ConvertEnum2Str(DxNAMEOF(DxSat_PskModulation_e), pstInfo->sat.tuningInfo.ePskMod));
						HxJSONWRITER_String (jw, "pilot", dlib_ConvertEnum2Str(DxNAMEOF(DxSat_Pilot_e), pstInfo->sat.tuningInfo.ePilot));
						HxJSONWRITER_String (jw, "rollOff", dlib_ConvertEnum2Str(DxNAMEOF(DxSat_RollOff_e), pstInfo->sat.tuningInfo.eRollOff));
						HxJSONWRITER_String (jw, "satType", pstInfo->sat.tuningInfo.satType);
						HxJSONWRITER_Integer (jw, "antuid", pstInfo->sat.tuningInfo.antuid);
					HxJSONWRITER_ObjectEnd(jw);

					//	antinfo.
					{
						DxDECFunc	encoder;

						encoder = dlib_GetDECModule(DxNAMEOF(DxAntInfo_t), TRUE);
						if (encoder)
						{
							HCHAR	*dstString;
							HUINT32	dstStringSize;
							if (encoder((void*)&pstInfo->sat.antInfo, sizeof(DxAntInfo_t), (void**)&dstString, &dstStringSize) == ERR_OK)
							{
								HxJSONWRITER_UserJSON(jw, "antInfo", dstString);

								DLIB_Free(dstString);
							}
						}
					}
				HxJSONWRITER_ObjectEnd(jw);
				break;

			case eDxDELIVERY_TYPE_CAB:
				HxJSONWRITER_Object(jw, "cab");
				HxJSONWRITER_String (jw, "deliveryType", dlib_ConvertEnum2Str(DxNAMEOF(DxDeliveryType_e), pstInfo->cab.deliveryType));
				HxJSONWRITER_Integer(jw, "ulTunerGroupId", pstInfo->cab.ulTunerGroupId);
				HxJSONWRITER_Integer(jw, "ulFrequency", pstInfo->cab.ulFrequency);
				HxJSONWRITER_Integer(jw, "ulSymbolRate", pstInfo->cab.ulSymbolRate);
				HxJSONWRITER_String (jw, "constellation", dlib_ConvertEnum2Str(DxNAMEOF(DxCab_Constellation_e), pstInfo->cab.constellation));
				HxJSONWRITER_String (jw, "spectrum", dlib_ConvertEnum2Str(DxNAMEOF(DxCab_Spectrum_e), pstInfo->cab.spectrum));
				HxJSONWRITER_Integer(jw, "nTsId", pstInfo->cab.nTsId);
				HxJSONWRITER_Integer(jw, "nNetId", pstInfo->cab.nNetId);

				HxJSONWRITER_ObjectEnd(jw);
				break;

			case eDxDELIVERY_TYPE_TER:
				HxJSONWRITER_Object(jw, "ter");
				HxJSONWRITER_String (jw, "deliveryType", dlib_ConvertEnum2Str(DxNAMEOF(DxDeliveryType_e), pstInfo->ter.deliveryType));
				HxJSONWRITER_Integer(jw, "ulTunerGroupId", pstInfo->ter.ulTunerGroupId);
				HxJSONWRITER_Integer(jw, "ulFrequency", pstInfo->ter.ulFrequency);
				HxJSONWRITER_String (jw, "bandWidth", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_BandWidth_e), pstInfo->ter.eBandWidth));
				HxJSONWRITER_String (jw, "constellation", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_Constellation_e), pstInfo->ter.eConstellation));
				HxJSONWRITER_String (jw, "hierachy", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_Hierachy_e), pstInfo->ter.eHierachy));
				HxJSONWRITER_String (jw, "codeRate", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_CodeRate_e), pstInfo->ter.eCodeRate));

				HxJSONWRITER_String (jw, "transMode", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_TransMode_e), pstInfo->ter.eTransMode));
				HxJSONWRITER_String (jw, "guardInterval", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_GuardInterval_e), pstInfo->ter.eGuardInterval));
				HxJSONWRITER_String (jw, "stream", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_Stream_e), pstInfo->ter.eStream));
				HxJSONWRITER_String (jw, "offset", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_Offset_e), pstInfo->ter.eOffset));

				/* DVB-T2 */
				HxJSONWRITER_String (jw, "system", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_System_e), pstInfo->ter.eSystem));
				HxJSONWRITER_Object(jw, "t2param");
					HxJSONWRITER_Integer(jw, "ulPlpId", pstInfo->ter.stT2Param.ulPlpId);
					HxJSONWRITER_String (jw, "preambleFormat", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_PreambleFormat_e), pstInfo->ter.stT2Param.preambleFormat));
					HxJSONWRITER_String (jw, "mixed", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_Mixed_e), pstInfo->ter.stT2Param.mixed));
					HxJSONWRITER_String (jw, "pilotPattern", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_Pilot_Pattern_e), pstInfo->ter.stT2Param.pilotPattern));
					HxJSONWRITER_String (jw, "extenedCarrier", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_ExtendedCarrier_e), pstInfo->ter.stT2Param.extenedCarrier));
					HxJSONWRITER_String (jw, "PAPRreduction", dlib_ConvertEnum2Str(DxNAMEOF(DxTer_PAPR_Reduction_e), pstInfo->ter.stT2Param.PAPRreduction));
					HxJSONWRITER_Integer(jw, "numPlpBlocks", pstInfo->ter.stT2Param.numPlpBlocks);
				HxJSONWRITER_ObjectEnd(jw);

				HxJSONWRITER_ObjectEnd(jw);
				break;

			default:
				break;
		}
	HxJSONWRITER_ObjectEnd(jw);

	if (HLIB_JSONWRITER_Error(jw))
	{
		HxLOG_Error("JSON Error : (%s)\n", HLIB_JSONWRITER_Error(jw));

		*dstSize = 0;
		*dst = NULL;
	} else
	{
		*dstSize = HLIB_JSONWRITER_GetLength(jw);
		*dst     = DLIB_StrDup(HLIB_JSONWRITER_GetJSON(jw));
		HLIB_JSONWRITER_Close(jw);
	}

	if (*dstSize > 0)
		return ERR_OK;

	return ERR_FAIL;
}

static HERROR	dlib_tuneparam_Decoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HxJSON_t		root;
	DxTuneParam_t		*pstInfo;

	HxLOG_Assert(srcSize && src);

	root = HLIB_JSON_Decode(src);
	HxLOG_Assert(root);

	pstInfo = (DxTuneParam_t*)DLIB_Calloc(sizeof(DxTuneParam_t));
	pstInfo->eDeliType = dlib_ConvertStr2Enum(DxNAMEOF(DxDeliveryType_e), HLIB_JSON_Object_GetString(root, "eDeliType"));
	switch (pstInfo->eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
			{
				HxJSON_t	sat, tuneInfo, antInfo;

				sat = HxJSON_Object_Get(root, "sat");
				pstInfo->sat.ulTunerGroupId = HLIB_JSON_Object_GetInteger(sat, "ulTunerGroupId");
				pstInfo->sat.antennaType    = dlib_ConvertStr2Enum(DxNAMEOF(DxAntennaType_e), HLIB_JSON_Object_GetString(sat, "antennaType"));
				tuneInfo = HxJSON_Object_Get(sat, "tuningInfo");
				pstInfo->sat.tuningInfo.ulFrequency = HLIB_JSON_Object_GetInteger(tuneInfo, "ulFrequency");
				pstInfo->sat.tuningInfo.ulSymbolRate= HLIB_JSON_Object_GetInteger(tuneInfo, "ulSymbolRate");
				pstInfo->sat.tuningInfo.eFecCodeRate= dlib_ConvertStr2Enum(DxNAMEOF(DxSat_CodeRate_e), HLIB_JSON_Object_GetString(tuneInfo, "fecCodeRate"));
				pstInfo->sat.tuningInfo.ePolarization= dlib_ConvertStr2Enum(DxNAMEOF(DxSat_Polarization_e), HLIB_JSON_Object_GetString(tuneInfo, "polarization"));
				pstInfo->sat.tuningInfo.eTransSpec= dlib_ConvertStr2Enum(DxNAMEOF(DxSat_TransportSpec_e), HLIB_JSON_Object_GetString(tuneInfo, "transSpec"));
				pstInfo->sat.tuningInfo.ePskMod= dlib_ConvertStr2Enum(DxNAMEOF(DxSat_PskModulation_e), HLIB_JSON_Object_GetString(tuneInfo, "pskMod"));
				pstInfo->sat.tuningInfo.ePilot= dlib_ConvertStr2Enum(DxNAMEOF(DxSat_Pilot_e), HLIB_JSON_Object_GetString(tuneInfo, "pilot"));
				pstInfo->sat.tuningInfo.eRollOff= dlib_ConvertStr2Enum(DxNAMEOF(DxSat_RollOff_e), HLIB_JSON_Object_GetString(tuneInfo, "rollOff"));

				HxSTD_StrNCpy(pstInfo->sat.tuningInfo.satType, HLIB_JSON_Object_GetString(tuneInfo, "satType"), DxANTINFO_NAME_LEN - 1);
				pstInfo->sat.tuningInfo.antuid = HLIB_JSON_Object_GetInteger(tuneInfo, "antuid");

				antInfo = HxJSON_Object_Get(sat, "antInfo");
				if (antInfo)
				{
					HCHAR		*encoded;
					DxDECFunc	decoder;

					decoder = dlib_GetDECModule(DxNAMEOF(DxAntInfo_t), FALSE);
					if (decoder)
					{
						encoded = HLIB_JSON_Encode(antInfo, NULL, 0);
						if (encoded)
						{
							void	*pvVoid = NULL;
							HUINT32	dstDataSize;

							decoder(encoded, HxSTD_StrLen(encoded), (void**)&pvVoid, &dstDataSize);
							HxLOG_Assert(dstDataSize == sizeof(DxAntInfo_t));
							HxSTD_MemCopy(&pstInfo->sat.antInfo, pvVoid, dstDataSize);
							DLIB_Free(pvVoid);
							HLIB_JSON_FreeMemory(encoded);
						}
					}
				}
			}
			break;

		case eDxDELIVERY_TYPE_CAB:
		{
			HxJSON_t	cab;
			cab = HxJSON_Object_Get(root, "cab");
			pstInfo->cab.deliveryType = dlib_ConvertStr2Enum(DxNAMEOF(DxDeliveryType_e), HLIB_JSON_Object_GetString(cab, "deliveryType"));
			pstInfo->cab.ulTunerGroupId = HLIB_JSON_Object_GetInteger(cab, "ulTunerGroupId");
			pstInfo->cab.ulFrequency = HLIB_JSON_Object_GetInteger(cab, "ulFrequency");
			pstInfo->cab.ulSymbolRate = HLIB_JSON_Object_GetInteger(cab, "ulSymbolRate");
			pstInfo->cab.constellation = dlib_ConvertStr2Enum(DxNAMEOF(DxCab_Constellation_e), HLIB_JSON_Object_GetString(cab, "constellation"));
			pstInfo->cab.spectrum = dlib_ConvertStr2Enum(DxNAMEOF(DxCab_Spectrum_e), HLIB_JSON_Object_GetString(cab, "spectrum"));
			pstInfo->cab.nTsId = HLIB_JSON_Object_GetInteger(cab, "nTsId");
			pstInfo->cab.nNetId = HLIB_JSON_Object_GetInteger(cab, "nNetId");
		}
			break;

		case eDxDELIVERY_TYPE_TER:
		{
			HxJSON_t	ter, t2param;
			ter = HxJSON_Object_Get(root, "ter");
			pstInfo->ter.deliveryType = dlib_ConvertStr2Enum(DxNAMEOF(DxDeliveryType_e), HLIB_JSON_Object_GetString(ter, "deliveryType"));
			pstInfo->ter.ulTunerGroupId = HLIB_JSON_Object_GetInteger(ter, "ulTunerGroupId");
			pstInfo->ter.ulFrequency = HLIB_JSON_Object_GetInteger(ter, "ulFrequency");
			pstInfo->ter.eBandWidth = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_BandWidth_e), HLIB_JSON_Object_GetString(ter, "bandWidth"));
			pstInfo->ter.eConstellation = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_Constellation_e), HLIB_JSON_Object_GetString(ter, "constellation"));
			pstInfo->ter.eHierachy = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_Hierachy_e), HLIB_JSON_Object_GetString(ter, "hierachy"));
			pstInfo->ter.eCodeRate = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_CodeRate_e), HLIB_JSON_Object_GetString(ter, "codeRate"));
			pstInfo->ter.eTransMode = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_TransMode_e), HLIB_JSON_Object_GetString(ter, "transMode"));
			pstInfo->ter.eGuardInterval = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_GuardInterval_e), HLIB_JSON_Object_GetString(ter, "guardInterval"));
			pstInfo->ter.eStream = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_Stream_e), HLIB_JSON_Object_GetString(ter, "stream"));
			pstInfo->ter.eOffset = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_Offset_e), HLIB_JSON_Object_GetString(ter, "offset"));

			/* DVB-T2 */
			pstInfo->ter.eSystem = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_System_e), HLIB_JSON_Object_GetString(ter, "system"));
			t2param = HxJSON_Object_Get(ter, "t2param");
			pstInfo->ter.stT2Param.ulPlpId = HLIB_JSON_Object_GetInteger(t2param, "ulPlpId");
			pstInfo->ter.stT2Param.preambleFormat = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_PreambleFormat_e), HLIB_JSON_Object_GetString(t2param, "preambleFormat"));
			pstInfo->ter.stT2Param.mixed = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_Mixed_e), HLIB_JSON_Object_GetString(t2param, "mixed"));
			pstInfo->ter.stT2Param.pilotPattern = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_Pilot_Pattern_e), HLIB_JSON_Object_GetString(t2param, "pilotPattern"));
			pstInfo->ter.stT2Param.extenedCarrier = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_ExtendedCarrier_e), HLIB_JSON_Object_GetString(t2param, "extenedCarrier"));
			pstInfo->ter.stT2Param.PAPRreduction = dlib_ConvertStr2Enum(DxNAMEOF(DxTer_PAPR_Reduction_e), HLIB_JSON_Object_GetString(t2param, "PAPRreduction"));
			pstInfo->ter.stT2Param.numPlpBlocks = HLIB_JSON_Object_GetInteger(t2param, "numPlpBlocks");
		}
			break;

		default:
			HxLOG_Critical("Not supported Delivery Type [%s]\n", dlib_ConvertEnum2Str(DxNAMEOF(DxDeliveryType_e), pstInfo->eDeliType));
			break;
	}

	HLIB_JSON_Delete(root);

	*dst = (void*)pstInfo;
	*dstSize = sizeof(DxTuneParam_t);

	return ERR_OK;
}

void			DLIB_TUNEPARAM_Init(void)
{
	dlib_RegisterEnumTable(&_stSATCodeRateTable);
	dlib_RegisterEnumTable(&_stSATPolarizationTable);
	dlib_RegisterEnumTable(&_stSATTransportSpecTable);
	dlib_RegisterEnumTable(&_stSATPSKModulationTable);
	dlib_RegisterEnumTable(&_stSATPilotTable);
	dlib_RegisterEnumTable(&_stSATRollOffTable);

	dlib_RegisterEnumTable(&_stCABContellationTable);
	dlib_RegisterEnumTable(&_stCABSpectrumTable);

	dlib_RegisterEnumTable(&_stTERBandwidthTable);
	dlib_RegisterEnumTable(&_stTERContellationTable);
	dlib_RegisterEnumTable(&_stTERHierachyTable);
	dlib_RegisterEnumTable(&_stTERCodeRateTable);
	dlib_RegisterEnumTable(&_stTERTransModeTable);
	dlib_RegisterEnumTable(&_stTERGuardIntervalTable);
	dlib_RegisterEnumTable(&_stTERStreamTable);
	dlib_RegisterEnumTable(&_stTEROffsetTable);
	dlib_RegisterEnumTable(&_stTERSystemTable);
	dlib_RegisterEnumTable(&_stTERPreambleFormatTable);
	dlib_RegisterEnumTable(&_stTERMixedTable);
	dlib_RegisterEnumTable(&_stTERPilotPatternTable);
	dlib_RegisterEnumTable(&_stTERExtendedCarrierTable);
	dlib_RegisterEnumTable(&_stTERPAPRReductionTable);

	dlib_RegisterDECModule(DxNAMEOF(DxTuneParam_t), dlib_tuneparam_Encoder, dlib_tuneparam_Decoder);
}

HINT32			DLIB_TUNEPARAM_GetFieldCount (void)
{
	return 0;
}

DxField_t *		DLIB_TUNEPARAM_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	return NULL;
}

HERROR			DLIB_TUNEPARAM_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_tuneparam_Encoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_TUNEPARAM_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_tuneparam_Decoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_TUNEPARAM_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 데이터 전송 시, 효율을 높이기 위해 압축이 필요한 경우 이 함수 호출됨
	// 압축 방식은 진짜 압축이 아닌, 그냥 string을 binary로 바꾸는 등
	// 아무튼 데이터를 줄이기만 하면 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}

HERROR			DLIB_TUNEPARAM_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 압축된 데이터 원복
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}


HERROR			DLIB_TUNEPARAM_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	return ERR_FAIL;
}

void			DLIB_TUNEPARAM_FreeType (void *type)
{
}



