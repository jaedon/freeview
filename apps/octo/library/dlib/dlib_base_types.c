
#include "_dlib.h"

static	DxEnumStringTable_t	s_stBaseOPTypeTable =
{
	(HCHAR*)DxNAMEOF(DxOPTYPE_e),
	{
		{	DxSTR_OPTYPE_ALL,			eDxOPTYPE_ALL			},
		{	DxSTR_OPTYPE_NONE,			eDxOPTYPE_NONE			},
		{	DxSTR_OPTYPE_APSHD,			eDxOPTYPE_APSHD 		},
		{	DxSTR_OPTYPE_FREESAT,		eDxOPTYPE_FREESAT		},
		{	DxSTR_OPTYPE_CANALSAT,		eDxOPTYPE_CANALSAT		},
		{	DxSTR_OPTYPE_NTVPLUS,		eDxOPTYPE_NTVPLUS		},
		{	DxSTR_OPTYPE_UPC,			eDxOPTYPE_UPC			},
		{	DxSTR_OPTYPE_ADTV,			eDxOPTYPE_ADTV			},
		{	DxSTR_OPTYPE_ALJAZEERA,		eDxOPTYPE_ALJAZEERA 	},
		{	DxSTR_OPTYPE_CNE,			eDxOPTYPE_CNE			},
		{	DxSTR_OPTYPE_DIGITURK,		eDxOPTYPE_DIGITURK		},
		{	DxSTR_OPTYPE_NONDIGITURK,	eDxOPTYPE_NONDIGITURK	},
		{	DxSTR_OPTYPE_TIVUSAT,		eDxOPTYPE_TIVUSAT		},
		{	DxSTR_OPTYPE_MOSTELECOM, 	eDxOPTYPE_MOSTELECOM	},
		{	DxSTR_OPTYPE_MAX,			eDxOPTYPE_MAX			},


		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseSysOPTypeTable =
{
	(HCHAR*)DxNAMEOF(DxOperator_e),
	{
		{	DxSTR_SYS_OPTYPE_NONE,				eDxOPERATOR_None			},
		{	DxSTR_SYS_OPTYPE_NOT_DEFINED,		eDxOPERATOR_NOT_DEFINED		},
		{	DxSTR_SYS_OPTYPE_ZIGGO,				eDxOPERATOR_Ziggo			},
		{	DxSTR_SYS_OPTYPE_COMHEM,			eDxOPERATOR_Comhem			},
		{	DxSTR_SYS_OPTYPE_KDG,				eDxOPERATOR_KDG				},
		{	DxSTR_SYS_OPTYPE_SKYD,				eDxOPERATOR_SkyD			},
		{	DxSTR_SYS_OPTYPE_APSHDPLUS,			eDxOPERATOR_Ses		},
		{	DxSTR_SYS_OPTYPE_FICOM,				eDxOPERATOR_FiCom			},
		{	DxSTR_SYS_OPTYPE_FREESAT,			eDxOPERATOR_Freesat			},
		{	DxSTR_SYS_OPTYPE_CANALREADY,		eDxOPERATOR_CanalReady		},
		{	DxSTR_SYS_OPTYPE_RCS,				eDxOPERATOR_Rcs				},
		{	DxSTR_SYS_OPTYPE_NTVPLUS,			eDxOPERATOR_NtvPlus			},
		{	DxSTR_SYS_OPTYPE_BBC,				eDxOPERATOR_BBC				},
		{	DxSTR_SYS_OPTYPE_UPC,				eDxOPERATOR_UPC				},
		{	DxSTR_SYS_OPTYPE_DIGITURK,			eDxOPERATOR_Digiturk		},
		{	DxSTR_SYS_OPTYPE_ADTV,				eDxOPERATOR_ADTV			},
		{	DxSTR_SYS_OPTYPE_ALJAZEERA,			eDxOPERATOR_Aljazeera		},
		{	DxSTR_SYS_OPTYPE_CNE,				eDxOPERATOR_CNE				},
		{	DxSTR_SYS_OPTYPE_SPAINTDT,			eDxOPERATOR_SpainTdt		},
		{	DxSTR_SYS_OPTYPE_TRUEVISIONS,		eDxOPERATOR_Truevisions		},
		{	DxSTR_SYS_OPTYPE_BOXER,				eDxOPERATOR_Boxer			},
		{	DxSTR_SYS_OPTYPE_TIVU,				eDxOPERATOR_TIVU			},
		{	DxSTR_SYS_OPTYPE_JCOM,				eDxOPERATOR_JCOM			},
		{	DxSTR_SYS_OPTYPE_SKAPA,				eDxOPERATOR_SKAPA			},
		{	DxSTR_SYS_OPTYPE_MOSTELECOM,		eDxOPERATOR_Mostelecom		},
		{	DxSTR_SYS_OPTYPE_SWISSCANAL,		eDxOPERATOR_SwissCanal		},
		{	DxSTR_SYS_OPTYPE_RUSSIATVT,			eDxOPERATOR_RussiaTvt		},
		{	DxSTR_SYS_OPTYPE_ALMAJD,			eDxOPERATOR_Almajd			},
		{	DxSTR_SYS_OPTYPE_NOVA,				eDxOPERATOR_NOVA			},
		{	DxSTR_SYS_OPTYPE_SKYCABLE,			eDxOPERATOR_SkyCable		},
		{	DxSTR_SYS_OPTYPE_GERMANCAB,			eDxOPERATOR_GermanCab		},
		{	DxSTR_SYS_OPTYPE_PRIMACOM,			eDxOPERATOR_Primacom		},
		{	DxSTR_SYS_OPTYPE_VTC,				eDxOPERATOR_VTC				},
		{	DxSTR_SYS_OPTYPE_ERTELECOM,			eDxOPERATOR_ERtelecom		},
		{	DxSTR_SYS_OPTYPE_MIDDLEEAST,		eDxOPERATOR_MiddleEast		},
		{	DxSTR_SYS_OPTYPE_ORF,				eDxOPERATOR_ORF				},
		{	DxSTR_SYS_OPTYPE_TDC,				eDxOPERATOR_Tdc				},
		{	DxSTR_SYS_OPTYPE_TELENOR,			eDxOPERATOR_Telenor			},
		{	DxSTR_SYS_OPTYPE_AMX,				eDxOPERATOR_AMX 			},
		{	DxSTR_SYS_OPTYPE_MBC,				eDxOPERATOR_MBC 			},

		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseGuidancePolicyTable =
{
	(HCHAR*)DxNAMEOF(DxGuidancePolicy_e),
	{
		{	DxSTR_GUIDANCEPOLICY_OFF,				eDxGUIDANCEPOLICY_OFF					},
		{	DxSTR_GUIDANCEPOLICY_MARKEDCONTENTS,	eDxGUIDANCEPOLICY_MARKEDCONTENTS		},
		{	DxSTR_GUIDANCEPOLICY_INTERNETCONTENTS,	eDxGUIDANCEPOLICY_INTERNETCONTENTS		},
		{	DxSTR_GUIDANCEPOLICY_ALLCONTENTS,		eDxGUIDANCEPOLICY_ALLCONTENTS			},

		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseAdultChannelType =
{
	(HCHAR*)DxNAMEOF(DxAdultChannelType_e),
	{
		{	DxSTR_ADULTCHANNEL_DISPLAY,				eDxADULTCHANNEL_DISPLAY		},
		{	DxSTR_ADULTCHANNEL_HIDE,				eDxADULTCHANNEL_HIDE		},

		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseSiMode =
{
	(HCHAR*)DxNAMEOF(DxSIMode_e),
	{
		{	DxSTR_SIMODE_FREESAT_G1,		eDxSIMODE_FREESAT_G1	},
		{	DxSTR_SIMODE_FREESAT_G2,		eDxSIMODE_FREESAT_G2	},
		{	DxSTR_SIMODE_UNKNOWN,			eDxSIMODE_UNKNOWN		},

		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseDeliveryTypeTable =
{
	(HCHAR*)DxNAMEOF(DxDeliveryType_e),
	{
		{	DxSTR_DELIVERY_TYPE_ALL,		eDxDELIVERY_TYPE_ALL 		},
		{	DxSTR_DELIVERY_TYPE_SAT,		eDxDELIVERY_TYPE_SAT 		},
		{	DxSTR_DELIVERY_TYPE_TER,		eDxDELIVERY_TYPE_TER 		},
		{	DxSTR_DELIVERY_TYPE_CAB,		eDxDELIVERY_TYPE_CAB 		},
		{	DxSTR_DELIVERY_TYPE_MULTICAST,	eDxDELIVERY_TYPE_MULTICAST 	},


		/* end of struct */
		{	NULL,						0						}

	}
};

static	DxEnumStringTable_t	s_stBaseVideoTypeTable =
{
	(HCHAR*)DxNAMEOF(DxVideoType_e),
	{
		{	DxSTR_VIDEO_TYPE_ALL,	eDxVIDEO_TYPE_ALL	},
		{	DxSTR_VIDEO_TYPE_SD, 	eDxVIDEO_TYPE_SD	},
		{	DxSTR_VIDEO_TYPE_HD, 	eDxVIDEO_TYPE_HD	},

		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseCasTypeBitTable =
{
	(HCHAR*)DxNAMEOF(DxCasType_e),
	{
		{	DxSTR_CAS_TYPE_All,			eDxCAS_TYPE_All 		},
		{	DxSTR_CAS_TYPE_FTA,			eDxCAS_TYPE_FTA 		},
		{	DxSTR_CAS_TYPE_NAGRA,		eDxCAS_TYPE_NAGRA		},
		{	DxSTR_CAS_TYPE_IRDETO,		eDxCAS_TYPE_IRDETO		},
		{	DxSTR_CAS_TYPE_NDS,			eDxCAS_TYPE_NDS 		},
		{	DxSTR_CAS_TYPE_VIACCESS, 	eDxCAS_TYPE_VIACCESS	},
		{	DxSTR_CAS_TYPE_CONAX,		eDxCAS_TYPE_CONAX		},
		{	DxSTR_CAS_TYPE_MEDIAGUARD,	eDxCAS_TYPE_MEDIAGUARD	},
		{	DxSTR_CAS_TYPE_CRYPTOWORKS,	eDxCAS_TYPE_CRYPTOWORKS },
		{	DxSTR_CAS_TYPE_VERIMATRIX,	eDxCAS_TYPE_VERIMATRIX	},
		{	DxSTR_CAS_TYPE_JPBCAS,		eDxCAS_TYPE_JPBCAS		},
		{	DxSTR_CAS_TYPE_JPCCAS,		eDxCAS_TYPE_JPCCAS		},
		{	DxSTR_CAS_TYPE_ALLCAS,		eDxCAS_TYPE_ALLCAS		},

		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseVideoCodecTable =
{
	(HCHAR*)DxNAMEOF(DxVideoCodec_e),
	{
		{	DxSTR_VIDEO_CODEC_UNKNOWN,			eDxVIDEO_CODEC_UNKNOWN			},
		{	DxSTR_VIDEO_CODEC_MPEG1,			eDxVIDEO_CODEC_MPEG1			},
		{	DxSTR_VIDEO_CODEC_MPEG2,			eDxVIDEO_CODEC_MPEG2			},
		{	DxSTR_VIDEO_CODEC_MPEG4_PART2,		eDxVIDEO_CODEC_MPEG4_PART2		},
		{	DxSTR_VIDEO_CODEC_H261, 			eDxVIDEO_CODEC_H261 			},
		{	DxSTR_VIDEO_CODEC_H263, 			eDxVIDEO_CODEC_H263 			},
		{	DxSTR_VIDEO_CODEC_H264, 			eDxVIDEO_CODEC_H264 			},
		{	DxSTR_VIDEO_CODEC_H264_SVC, 		eDxVIDEO_CODEC_H264_SVC 		},
		{	DxSTR_VIDEO_CODEC_H264_MVC, 		eDxVIDEO_CODEC_H264_MVC 		},
		{	DxSTR_VIDEO_CODEC_VC1,				eDxVIDEO_CODEC_VC1				},
		{	DxSTR_VIDEO_CODEC_VC1_SIMPLEMAIN,	eDxVIDEO_CODEC_VC1_SIMPLEMAIN	},
		{	DxSTR_VIDEO_CODEC_AVS,				eDxVIDEO_CODEC_AVS				},
		{	DxSTR_VIDEO_CODEC_RV40, 			eDxVIDEO_CODEC_RV40 			},
		{	DxSTR_VIDEO_CODEC_VP6,				eDxVIDEO_CODEC_VP6				},
		{	DxSTR_VIDEO_CODEC_DIVX311,			eDxVIDEO_CODEC_DIVX311			},
		{	DxSTR_VIDEO_CODEC_XVID, 			eDxVIDEO_CODEC_XVID 			},
		{	DxSTR_VIDEO_CODEC_H265, 			eDxVIDEO_CODEC_H265 			},
		{	DxSTR_VIDEO_CODEC_VP8,				eDxVIDEO_CODEC_VP8				},
		{	DxSTR_VIDEO_CODEC_VP9,				eDxVIDEO_CODEC_VP9				},
		{	DxSTR_VIDEO_CODEC_MAX_NUM,			eDxVIDEO_CODEC_MAX_NUM			},

		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseAudioCodecTable =
{
	(HCHAR*)DxNAMEOF(DxAudioCodec_e),
	{
		{	DxSTR_AUDIO_CODEC_UNKNOWN,			eDxAUDIO_CODEC_UNKNOWN			},
		{	DxSTR_AUDIO_CODEC_MPEG, 			eDxAUDIO_CODEC_MPEG 			},
		{	DxSTR_AUDIO_CODEC_MP3,				eDxAUDIO_CODEC_MP3				},
		{	DxSTR_AUDIO_CODEC_DOLBY_AC3,		eDxAUDIO_CODEC_DOLBY_AC3		},
		{	DxSTR_AUDIO_CODEC_DOLBY_AC3P,		eDxAUDIO_CODEC_DOLBY_AC3P		},
		{	DxSTR_AUDIO_CODEC_PCM,				eDxAUDIO_CODEC_PCM				},
		{	DxSTR_AUDIO_CODEC_AAC,				eDxAUDIO_CODEC_AAC				},
		{	DxSTR_AUDIO_CODEC_AAC_LOAS,			eDxAUDIO_CODEC_AAC_LOAS			},
		{	DxSTR_AUDIO_CODEC_AAC_PLUS,			eDxAUDIO_CODEC_AAC_PLUS			},
		{	DxSTR_AUDIO_CODEC_AAC_PLUS_ADTS,	eDxAUDIO_CODEC_AAC_PLUS_ADTS	},
		{	DxSTR_AUDIO_CODEC_DTS,				eDxAUDIO_CODEC_DTS				},
		{	DxSTR_AUDIO_CODEC_DTS_HD,			eDxAUDIO_CODEC_DTS_HD			},
		{	DxSTR_AUDIO_CODEC_LPCM_DVD, 		eDxAUDIO_CODEC_LPCM_DVD			},
		{	DxSTR_AUDIO_CODEC_LPCM_HD_DVD,		eDxAUDIO_CODEC_LPCM_HD_DVD		},
		{	DxSTR_AUDIO_CODEC_LPCM_BLUERAY,		eDxAUDIO_CODEC_LPCM_BLUERAY		},
		{	DxSTR_AUDIO_CODEC_WMA_STD,			eDxAUDIO_CODEC_WMA_STD			},
		{	DxSTR_AUDIO_CODEC_WMA_PRO,			eDxAUDIO_CODEC_WMA_PRO			},
		{	DxSTR_AUDIO_CODEC_PCMWAVE,			eDxAUDIO_CODEC_PCMWAVE			},
		{	DxSTR_AUDIO_CODEC_DRA,				eDxAUDIO_CODEC_DRA				},
		{	DxSTR_AUDIO_CODEC_AIFF, 			eDxAUDIO_CODEC_AIFF 			},
		{	DxSTR_AUDIO_CODEC_MAX_NUM,			eDxAUDIO_CODEC_MAX_NUM			},


		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseAntennaTypeTable =
{
	(HCHAR*)DxNAMEOF(DxAntennaType_e),
	{
		{	DxSTR_ANT_TYPE_LNB_ONLY, 		eDxANT_TYPE_LNB_ONLY	},
		{	DxSTR_ANT_TYPE_DISEQC,			eDxANT_TYPE_DISEQC		},
		{	DxSTR_ANT_TYPE_SCD,				eDxANT_TYPE_SCD 		},
		{	DxSTR_ANT_TYPE_SMATV,			eDxANT_TYPE_SMATV		},
		{	DxSTR_ANT_TYPE_MONOBLOCK_LNB,	eDxANT_TYPE_MONOBLOCK_LNB},

		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseStereoSelectTable =
{
	(HCHAR*)DxNAMEOF(DxStereoSelect_e),
	{
		{	DxSTR_STEREO_SELECT_UNKNOWN,	eDxSTEREO_SELECT_UNKNOWN },
		{	DxSTR_STEREO_SELECT_STEREO,		eDxSTEREO_SELECT_STEREO	},
		{	DxSTR_STEREO_SELECT_MONOLEFT, 	eDxSTEREO_SELECT_MONOLEFT},
		{	DxSTR_STEREO_SELECT_MONORIGHT,	eDxSTEREO_SELECT_MONORIGHT},
		{	DxSTR_STEREO_SELECT_MONOMIXED,	eDxSTEREO_SELECT_MONOMIXED 	},

		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseDualMonoSelectTable =
{
	(HCHAR*)DxNAMEOF(DxDualMonoSelect_e),
	{
		{	DxSTR_DUALMONO_SELECT_UNKNOWN,	eDxDUALMONO_SELECT_UNKNOWN },
		{	DxSTR_DUALMONO_SELECT_BOTH,		eDxDUALMONO_SELECT_BOTH},
		{	DxSTR_DUALMONO_SELECT_LEFT,		eDxDUALMONO_SELECT_LEFT},
		{	DxSTR_DUALMONO_SELECT_RIGHT, 	eDxDUALMONO_SELECT_RIGHT},

		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseDigitalAudioFormatTable =
{
	(HCHAR*)DxNAMEOF(DxDigitalAudioFormat_e),
	{
		{	DxSTR_DIGITALAUDIO_FORMAT_UNKNOWN,		eDxDIGITALAUDIO_FORMAT_UNKNOWN },
		{	DxSTR_DIGITALAUDIO_FORMAT_PCM,			eDxDIGITALAUDIO_FORMAT_PCM},
		{	DxSTR_DIGITALAUDIO_FORMAT_MULTICHANNEL,	eDxDIGITALAUDIO_FORMAT_MULTICHANNEL},

		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseScartFormatTable =
{
	(HCHAR*)DxNAMEOF(DxScartFormat_e),
	{
		{	DxSTR_SCART_FORMAT_UNKNOWN,		eDxSCART_FORMAT_UNKNOWN },
		{	DxSTR_SCART_FORMAT_CVBS,		eDxSCART_FORMAT_CVBS},
		{	DxSTR_SCART_FORMAT_RGB,			eDxSCART_FORMAT_RGB},
		{	DxSTR_SCART_FORMAT_SVIDEO,		eDxSCART_FORMAT_SVIDEO},


		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseScartPathTable =
{
	(HCHAR*)DxNAMEOF(DxScartPath_e),
	{
		{	DxSTR_SCART_PATH_UNKNOWN,		eDxSCART_PATH_UNKNOWN },
		{	DxSTR_SCART_PATH_STB2TV,		eDxSCART_PATH_STB2TV},
		{	DxSTR_SCART_PATH_VCR2TV,		eDxSCART_PATH_VCR2TV},
		{	DxSTR_SCART_PATH_HWBYPASS,		eDxSCART_PATH_HWBYPASS},


		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBase3DSelectTable =
{
	(HCHAR*)DxNAMEOF(DxHdmi3DSelect_e),
	{
		{	DxSTR_HDMI_3DSELECT_OFF,				eDxHDMI_3DSELECT_OFF },
		{	DxSTR_HDMI_3DSELECT_SIDEBYSIDE,			eDxHDMI_3DSELECT_SIDEBYSIDE},
		{	DxSTR_HDMI_3DSELECT_TOPANDBOTTOM,		eDxHDMI_3DSELECT_TOPANDBOTTOM},
		{	DxSTR_HDMI_3DSELECT_FRAMEPACKING,		eDxHDMI_3DSELECT_FRAMEPACKING},
		{	DxSTR_HDMI_3DSELECT_AUTO,				eDxHDMI_3DSELECT_AUTO},
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseDfcSelectTable =
{
	(HCHAR*)DxNAMEOF(DxDfcSelect_e),
	{
		{	DxSTR_DFC_SELECT_UNKNOWN,				eDxDFC_SELECT_UNKNOWN },
		{	DxSTR_DFC_SELECT_DEFAULT,				eDxDFC_SELECT_DEFAULT },
		{	DxSTR_DFC_SELECT_WIDESIGNAL,			eDxDFC_SELECT_WIDESIGNAL },
		{	DxSTR_DFC_SELECT_SQUEEZE,				eDxDFC_SELECT_SQUEEZE },
		{	DxSTR_DFC_SELECT_SQUEEZE149,			eDxDFC_SELECT_SQUEEZE149 },
		{	DxSTR_DFC_SELECT_CENTERCUTOUT,			eDxDFC_SELECT_CENTERCUTOUT },
		{	DxSTR_DFC_SELECT_CENTERCUTOUT149,		eDxDFC_SELECT_CENTERCUTOUT149 },
		{	DxSTR_DFC_SELECT_FULLSCREEN,			eDxDFC_SELECT_FULLSCREEN },
		{	DxSTR_DFC_SELECT_LETTERBOX,				eDxDFC_SELECT_LETTERBOX },
		{	DxSTR_DFC_SELECT_LETTERBOX149,			eDxDFC_SELECT_LETTERBOX149 },
		{	DxSTR_DFC_SELECT_PILLARBOX,				eDxDFC_SELECT_PILLARBOX },
		{	DxSTR_DFC_SELECT_PILLARBOX149,			eDxDFC_SELECT_PILLARBOX149 },
		{	DxSTR_DFC_SELECT_ZOOM,					eDxDFC_SELECT_ZOOM },
		{	DxSTR_DFC_SELECT_ZOOM149,				eDxDFC_SELECT_ZOOM149 },
		{	DxSTR_DFC_SELECT_SIDECUT,				eDxDFC_SELECT_SIDECUT },
		{	DxSTR_DFC_SELECT_SIDECUT149,			eDxDFC_SELECT_SIDECUT149 },


		/* end of struct */
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseResolutionSelectTable =
{
	(HCHAR*)DxNAMEOF(DxResolutionSelect_e),
	{
		{	DxSTR_RESOLUTION_SELECT_UNKNOWN,	eDxRESOLUTION_SELECT_UNKNOWN },
		{	DxSTR_RESOLUTION_SELECT_480I,		eDxRESOLUTION_SELECT_480I },
		{	DxSTR_RESOLUTION_SELECT_576I,		eDxRESOLUTION_SELECT_576I },
		{	DxSTR_RESOLUTION_SELECT_480P,		eDxRESOLUTION_SELECT_480P },
		{	DxSTR_RESOLUTION_SELECT_576P,		eDxRESOLUTION_SELECT_576P },
		{	DxSTR_RESOLUTION_SELECT_720P,		eDxRESOLUTION_SELECT_720P },
		{	DxSTR_RESOLUTION_SELECT_1080I,		eDxRESOLUTION_SELECT_1080I },
		{	DxSTR_RESOLUTION_SELECT_1080P,		eDxRESOLUTION_SELECT_1080P },
		{	DxSTR_RESOLUTION_SELECT_2160P,		eDxRESOLUTION_SELECT_2160P },
		{	DxSTR_RESOLUTION_SELECT_ORIGINAL,	eDxRESOLUTION_SELECT_ORIGINAL },
		{	DxSTR_RESOLUTION_SELECT_EDID_MAX,	eDxRESOLUTION_SELECT_EDID_MAX },
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stBaseAspectRatioTable =
{
	(HCHAR*)DxNAMEOF(DxAspectRatio_e),
	{
		{	DxSTR_ASPECTRATIO_UNKNOWN,	eDxASPECTRATIO_UNKNOWN },
		{	DxSTR_ASPECTRATIO_4X3,		eDxASPECTRATIO_4X3 },
		{	DxSTR_ASPECTRATIO_16X9,		eDxASPECTRATIO_16X9 },
		{	DxSTR_ASPECTRATIO_14X9,		eDxASPECTRATIO_14X9 },
		{	DxSTR_ASPECTRATIO_1X1,		eDxASPECTRATIO_1X1 },
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stAntennaConnectionTable =
{
	(HCHAR*)DxNAMEOF(DxAntennaConnection_e),
	{
		{	DxSTR_ANTCONN_ONECABLE,				eDxANTCONN_ONECABLE },
		{	DxSTR_ANTCONN_TWOSAME,				eDxANTCONN_TWOSAME },
		{	DxSTR_ANTCONN_TWODIFF,				eDxANTCONN_TWODIFF },
		{	DxSTR_ANTCONN_NONSATONECABLE,		eDxANTCONN_NONSATONECABLE },
		{	DxSTR_ANTCONN_NONSATTWOCABLE,		eDxANTCONN_NONSATTWOCABLE },
		{	NULL,						0						}
	}
};

static	DxEnumStringTable_t	s_stLiveStreamPriorityTable =
{
	(HCHAR*)DxNAMEOF(DxLiveStreamPriority_e),
	{
		{	DxSTR_LIVESTREAMPRIORITY_EQUALREC,	eDxLsPriority_EQUAL_REC		},
		{	DxSTR_LIVESTREAMPRIORITY_UPPERREC,	eDxLsPriority_UPPER_REC 	},
		{	DxSTR_LIVESTREAMPRIORITY_EQUALVIEW,	eDxLsPriority_EQUAL_VIEW	},
		{	DxSTR_LIVESTREAMPRIORITY_BELOWVIEW,	eDxLsPriority_BELOW_VIEW	},
		{	NULL,								0							}
	}
};

HERROR	DLIB_BASE_Init(void)
{
	dlib_RegisterEnumTable(&s_stBaseOPTypeTable);
	dlib_RegisterEnumTable(&s_stBaseSysOPTypeTable);
	dlib_RegisterEnumTable(&s_stBaseGuidancePolicyTable);
	dlib_RegisterEnumTable(&s_stBaseSiMode);
	dlib_RegisterEnumTable(&s_stBaseAdultChannelType);
	dlib_RegisterEnumTable(&s_stBaseDeliveryTypeTable);
	dlib_RegisterEnumTable(&s_stBaseVideoTypeTable);
	dlib_RegisterEnumTable(&s_stBaseCasTypeBitTable);
	dlib_RegisterEnumTable(&s_stBaseVideoCodecTable);
	dlib_RegisterEnumTable(&s_stBaseAudioCodecTable);
	dlib_RegisterEnumTable(&s_stBaseAntennaTypeTable);

	dlib_RegisterEnumTable(&s_stBaseStereoSelectTable);
	dlib_RegisterEnumTable(&s_stBaseDualMonoSelectTable);
	dlib_RegisterEnumTable(&s_stBaseDigitalAudioFormatTable);

	dlib_RegisterEnumTable(&s_stBaseScartFormatTable);
	dlib_RegisterEnumTable(&s_stBaseScartPathTable);
	dlib_RegisterEnumTable(&s_stBase3DSelectTable);
	dlib_RegisterEnumTable(&s_stBaseDfcSelectTable);
	dlib_RegisterEnumTable(&s_stBaseResolutionSelectTable);
	dlib_RegisterEnumTable(&s_stBaseAspectRatioTable);
	dlib_RegisterEnumTable(&s_stAntennaConnectionTable);

	dlib_RegisterEnumTable(&s_stLiveStreamPriorityTable);

	//dlib_RegisterDECModule(const HCHAR * decType,DxDECFunc encoder,DxDECFunc decoder)

	//	for keep compatibility
	DLIB_BASE_Deprecated();

	return ERR_OK;
}

