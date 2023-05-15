#include <hlib.h>
#include "fsat_types.h"
/**************************************************************
	global tag and property information
 **************************************************************/
#define	__log		printf
#define	return_if_void(expr)	do{if(expr){__log("[%s:%d] %s failed!!\n", __FUNCTION__, __LINE__, #expr);return;}}while(0)
#define	return_if(expr, err)	do{if(expr){__log("[%s:%d] %s failed!!\n", __FUNCTION__, __LINE__, #expr);return (err);}}while(0)
#define	goto_if(expr, pos)		do{if(expr){__log("[%s:%d] %s failed!!\n", __FUNCTION__, __LINE__, #expr);goto pos;}}while(0)

#define	RECLIST_PR_TAG			    "parentalrating"
#define	RECLIST_BOOKMARK_TAG		"bookmark"
#define	RECLIST_GUIDANCE_TAG		"guidance"
#define	RECLIST_EVT_TAG			    "event"
#define	RECLIST_EXT_EVT_TAG		    "ext_event"
#define	RECLIST_EXT_EVT_TXT_TAG	    "ext_event_text"

typedef	enum
{
	efsatDxTAG_Pair,
	efsatDxTAG_Integer,
	efsatDxTAG_Float,
	efsatDxTAG_String,
	efsatDxTAG_Bool,
	efsatDxTAG_Binary
} fsatDxTAGTYPE_e;

typedef	struct
{
	HCHAR		*tag;
	HUINT32		value;
} fsatDxTAGLIST_t;

typedef	struct
{
	fsatDxTAGTYPE_e	tag_type;
	fsatDxTAGLIST_t	taglist[];
} fsatDxTAG2ENUM_t;

typedef	struct
{
	HCHAR		    *property;
	fsatDxTAG2ENUM_t	*tagtable;
	HUINT32		offset;
	HUINT32		sizeofdata;
} fsatDxPROP2ENUM_t;

typedef	struct	_PROP2DATA
{
	HCHAR			*property;
	fsatDxTAG2ENUM_t	*enumtable;
	HINT32			offset;
	HINT32			size;
} fsatDxPROP2DATA_t;



#if !defined(CONFIG_MW_MM_AUTO_BOOKMARK) || defined(CONFIG_OP_UPC) || defined(CONFIG_OP_ZIGGO) || defined(CONFIG_OP_RUSSIA_TVT)
#define PVR_NUM_BOOKMARK					4
#else
#define PVR_NUM_BOOKMARK					32
#endif

typedef	struct	_PROP2ENUM
{
	HCHAR			*property;
	fsatDxTAG2ENUM_t	*enumtable;
	HUINT32			offset;

	HINT32				sizeofstructure;
	struct	_PROP2ENUM	*propTable;

	HINT32				ObjectCount;
} fsatDxPROP2ENUMex_t;


static fsatDxTAG2ENUM_t		fsatDxOBJDEC_BOOL_Table =
{
	efsatDxTAG_Bool,
	{
	}
};

static fsatDxTAG2ENUM_t		fsatDxOBJDEC_INT_Table =
{
	efsatDxTAG_Integer,
	{
	}
};

static fsatDxTAG2ENUM_t		fsatDxOBJDEC_STRING_Table =
{
	efsatDxTAG_String,
	{
	}
};

static fsatDxTAG2ENUM_t		fsatDxOBJDEC_FLOAT_Table =
{
	efsatDxTAG_Float,
	{
	}
};

static fsatDxTAG2ENUM_t		fsatDxOBJDEC_BINARY_Table =
{
	efsatDxTAG_Binary,
	{
	}
};
static fsatDxTAG2ENUM_t		fsatDxOBJDEC_SVCTYPE_Table =
{
	efsatDxTAG_Pair,
	{
		{	"tv",			efsatDxSVC_TYPE_TV			},
		{	"radio",		efsatDxSVC_TYPE_RADIO		},
		{	"data",			efsatDxSVC_TYPE_DATA		},
		{	"subfeed",		efsatDxSVC_TYPE_SUBFEED		},
		{	"replacement",	efsatDxSVC_TYPE_REPLACEMENT	},
		{	"hidden",		efsatDxSVC_TYPE_HIDDEN		},
		{	"mhptv",		efsatDxSVC_TYPE_MHPTV		},
		{	"engineering",	efsatDxSVC_TYPE_ENGINEERING	},
		{	"rfvod",		efsatDxSVC_TYPE_RFVOD		},
		{	NULL,			efsatDxSVC_TYPE_All			}
	}
};

static fsatDxTAG2ENUM_t		fsatDxOBJDEC_VIDEOTYPE_Table =
{
	efsatDxTAG_Pair,
	{
		{	"hd",		efsatDxVIDEO_TYPE_HD		},
		{	"sd",		efsatDxVIDEO_TYPE_SD		},
		{	"all",		efsatDxVIDEO_TYPE_ALL		},
		{	NULL,		FALSE	}
	}
};
static fsatDxTAG2ENUM_t		fsatDxOBJDEC_OPTYPE_Table =
{
	efsatDxTAG_Pair,
	{
		{	"all",			efsatDxOPTYPE_ALL	},
		{	"none",			efsatDxOPTYPE_NONE},
		{	"apshd",		efsatDxOPTYPE_APSHD},
		{	"freesat",		efsatDxOPTYPE_FREESAT},
		{	"canalsat", 	efsatDxOPTYPE_CANALSAT},
		{	"ntvplus",		efsatDxOPTYPE_NTVPLUS},
		{	"upc",			efsatDxOPTYPE_UPC},
		{	"adtv",			efsatDxOPTYPE_ADTV},
		{	"aljazeera",	efsatDxOPTYPE_ALJAZEERA},
		{	"cne",			efsatDxOPTYPE_CNE},
		{	"digiturk",		efsatDxOPTYPE_DIGITURK},
		{	"non-digiturk",	efsatDxOPTYPE_NONDIGITURK},	// 이거 있어야 하나???
		{	"tivusat",		efsatDxOPTYPE_TIVUSAT},
		{	"mostelecom",	efsatDxOPTYPE_MOSTELECOM},
		{	NULL,			efsatDxOPTYPE_ALL	}
	}
};
static fsatDxTAG2ENUM_t		fsatDxOBJDEC_CASTYPE_Table =
{
	efsatDxTAG_Pair,
	{
		{	"fta",			efsatDxCAS_TYPE_FTA			},
		{	"nagra",		efsatDxCAS_TYPE_NAGRA		},
		{	"irdeto",		efsatDxCAS_TYPE_IRDETO		},
		{	"nds",			efsatDxCAS_TYPE_NDS			},
		{	"viaccess",		efsatDxCAS_TYPE_VIACCESS	},
		{	"conax",		efsatDxCAS_TYPE_CONAX		},
		{	"mediaguard",	efsatDxCAS_TYPE_MEDIAGUARD	},
		{	"cryptoworks",	efsatDxCAS_TYPE_CRYPTOWORKS	},
		{	"jpbcas",		efsatDxCAS_TYPE_JPBCAS		},
		{	"jpccas",		efsatDxCAS_TYPE_JPCCAS		},
		{	NULL,			efsatDxCAS_TYPE_All			}
	}
};

static fsatDxTAG2ENUM_t		fsatDxOBJDEC_VideoCodec_Table =
{
	efsatDxTAG_Pair,
	{
		{	"MPEG-1",		efsatDxVIDEO_CODEC_MPEG1		},
		{	"MPEG-2",		efsatDxVIDEO_CODEC_MPEG2		},
		{	"MPEG-4",		efsatDxVIDEO_CODEC_MPEG4Part2	},
		{	"H.261",		efsatDxVIDEO_CODEC_H261			},
		{	"H.263",		efsatDxVIDEO_CODEC_H263			},
		{	"H.264", 		efsatDxVIDEO_CODEC_H264			},
		{	"H.264-SVC",	efsatDxVIDEO_CODEC_H264_Svc		},
		{	"H.264-MVC",	efsatDxVIDEO_CODEC_H264_Mvc		},
		{	"VC-1",			efsatDxVIDEO_CODEC_VC1			},
		{	"VC-1-Simple",	efsatDxVIDEO_CODEC_VC1_SimpleMain},
		{	"AVS",			efsatDxVIDEO_CODEC_AVS			},
		{	"RV4.0",		efsatDxVIDEO_CODEC_RV40			},
		{	"VP6",			efsatDxVIDEO_CODEC_VP6			},
		{	"DivX-3.11",	efsatDxVIDEO_CODEC_DIVX311		},
		{   "XVID",			efsatDxVIDEO_CODEC_XVID			},
		{   "MAX",			efsatDxVIDEO_CODEC_Max			},
		{	NULL,			efsatDxVIDEO_CODEC_UNKNOWN		}
	}
};


static fsatDxTAG2ENUM_t		fsatDxOBJDEC_AudioCodec_Table =
{
	efsatDxTAG_Pair,
	{
		{	"MPEG",			efsatDxAUDIO_CODEC_MPEG	},
		{	"MP3",			efsatDxAUDIO_CODEC_MP3	},
		{	"AC3",			efsatDxAUDIO_CODEC_DOLBY_AC3 },
		{	"DDP",			efsatDxAUDIO_CODEC_DOLBY_AC3P	},
		{	"PCM", 			efsatDxAUDIO_CODEC_PCM	},
		{	"AAC",			efsatDxAUDIO_CODEC_AAC	},
		{	"AAC-LOAS",		efsatDxAUDIO_CODEC_AAC_LOAS },
		{	"AAC+",			efsatDxAUDIO_CODEC_AAC_PLUS	},
		{	"AAC+ADTS",		efsatDxAUDIO_CODEC_AAC_PLUS_ADTS },
		{	"DTS",			efsatDxAUDIO_CODEC_DTS },
		{	"DTS-HD",		efsatDxAUDIO_CODEC_DTS_HD },
		{	"LPCM-DVD",		efsatDxAUDIO_CODEC_LPCM_DVD },
		{	"LPCM-HDDVD",	efsatDxAUDIO_CODEC_LPCM_HD_DVD },
		{	"LPCM-BD",		efsatDxAUDIO_CODEC_LPCM_BLUERAY },
		{	"WMA",			efsatDxAUDIO_CODEC_WMA_STD	},
		{	"WMA-PRO", 		efsatDxAUDIO_CODEC_WMA_PRO	},
		{	"PCMWAVE",		efsatDxAUDIO_CODEC_PCMWAVE	},
		{	"DRA",			efsatDxAUDIO_CODEC_DRA },
		{	"AIFF",			efsatDxAUDIO_CODEC_AIFF	},
		{	NULL,			efsatDxAUDIO_CODEC_UNKNOWN}
	}
};

static fsatDxTAG2ENUM_t		fsatDxOBJDEC_StereoCfg_Table =
{
	efsatDxTAG_Pair,
	{
		{	"stereo",		efsatDxAUDIO_CONFIG_STEREO		},
		{	"left", 		efsatDxAUDIO_CONFIG_MONOLEFT	},
		{	"right",		efsatDxAUDIO_CONFIG_MONORIGHT	},
		{	"mix",			efsatDxAUDIO_CONFIG_MIX			},

		{	NULL,			efsatDxAUDIO_CONFIG_UNKNOWN		}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_DeliveryType_Table =
{
	efsatDxTAG_Pair,
	{
		{	"sat",			efsatDxDELIVERY_SAT		},
		{	"cab",			efsatDxDELIVERY_CAB		},
		{	"ter",			efsatDxDELIVERY_TER		},
		{	"analog",		efsatDxDELIVERY_ANALOG	},
		{	"net",			efsatDxDELIVERY_NET		},
		{	NULL,			efsatDxDELIVERY_ALL		}
	}
};

static fsatDxTAG2ENUM_t		fsatDxaudioconfig_table =
{
	efsatDxTAG_Pair,
	{
		{	"stereo",			efsatDxAUDIO_CONFIG_STEREO		},
		{	"monoleft",			efsatDxAUDIO_CONFIG_MONOLEFT	},
		{	"monoright",		efsatDxAUDIO_CONFIG_MONORIGHT	},
		{	"mix",				efsatDxAUDIO_CONFIG_MIX			},
		{	NULL,				efsatDxAUDIO_CONFIG_UNKNOWN		}
	}
};



static fsatDxPROP2ENUM_t		fsatDxmeta_service_table[] =
{
	{	"svc_uid",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxService_t, uid),		4	},
	{	"ts_uid",		&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, tsuid),	4	},
	{	"prv_uid",		&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, prvuid),	4	},
	{	"ant_uid",		&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, antuid),	4	},
	{	"svcid",		&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, svcid),	4	},
	{	"tsid", 		&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, tsid), 	4	},
	{	"onid", 		&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, onid),		4	},
	{	"lcn",			&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, lcn),		4	},
	{	"pmt_pid",		&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, pmtPid),	4	},
	{	"pcr_pid",		&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, pcrPid),	4	},
	{	"video_pid",	&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, videoPid),	4	},
	{	"audio_pid",	&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, audioPid),	4	},
	{	"ttx_pid",		&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, ttxPid),	4	},
	{	"dolby_pid",	&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, dolbyPid),	4	},
	{	"audio_aux_pid",	&fsatDxOBJDEC_INT_Table, 	offsetof(fsatDxService_t, audioAuxPid),	4	},

	{	"name",			&fsatDxOBJDEC_STRING_Table,	            offsetof(fsatDxService_t, name),	fsatDxNAME_LEN	},
	{	"video_codec",		&fsatDxOBJDEC_VideoCodec_Table,		offsetof(fsatDxService_t, videoCodec),		4	},
	{	"audio_codec",		&fsatDxOBJDEC_AudioCodec_Table,		offsetof(fsatDxService_t, audioCodec),		4	},
	{	"audio_aux_codec",	&fsatDxOBJDEC_AudioCodec_Table,		offsetof(fsatDxService_t, audioAuxCodec),	4	},
	{	"dolby_codec",		&fsatDxOBJDEC_AudioCodec_Table,		offsetof(fsatDxService_t, dolbyCodec),		4	},

	{	"svctype",		&fsatDxOBJDEC_SVCTYPE_Table,		offsetof(fsatDxService_t, svcType),	4	},
	{	"org_svctype",	&fsatDxOBJDEC_SVCTYPE_Table,		offsetof(fsatDxService_t, orgSvcType),	4	},
	{	"videotype",	&fsatDxOBJDEC_VIDEOTYPE_Table,	    offsetof(fsatDxService_t, highDefinition),	4	},
	{	"castype",		&fsatDxOBJDEC_CASTYPE_Table,		offsetof(fsatDxService_t, casType),	4	},
	//{	"section",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxService_t, section),	4	},
	{	"optype",		&fsatDxOBJDEC_OPTYPE_Table,		offsetof(fsatDxService_t, optype),	4	},
	{	"delitype",		&fsatDxCH_DeliveryType_Table,	offsetof(fsatDxService_t, deliType),	4	},
	{	"sattype",		&fsatDxOBJDEC_STRING_Table,		offsetof(fsatDxService_t, satType),	fsatDxANTINFO_NAME_LEN	},
	{	"lcnflag",		&fsatDxOBJDEC_BOOL_Table,		offsetof(fsatDxService_t, lcnFlag),	4	},
	{	"visiblesvcflag",	&fsatDxOBJDEC_BOOL_Table,	offsetof(fsatDxService_t, visibleFlag),	4	},
	{	"dolbyflag",	&fsatDxOBJDEC_BOOL_Table,		offsetof(fsatDxService_t, dolbyFlag),	4	},
	{	"locked",		&fsatDxOBJDEC_BOOL_Table,		offsetof(fsatDxService_t, locked),	4	},
	{	"stereo_config",	&fsatDxaudioconfig_table,	offsetof(fsatDxService_t, audioConfig),	4	},
	{	"deleted",		&fsatDxOBJDEC_BOOL_Table,		offsetof(fsatDxService_t, removed),	4	},

	{	NULL,			NULL,						0,									0	}
};



static fsatDxTAG2ENUM_t		fsatDxCH_SATFEC_Table =
{
	efsatDxTAG_Pair,
	{
		{	"auto",			efsatDxSAT_CODERATE_AUTO		},
		{	"none",			efsatDxSAT_CODERATE_NONE		},
		{	"1/2",			efsatDxSAT_CODERATE_1_2			},
		{	"2/3",			efsatDxSAT_CODERATE_2_3			},
		{	"3/4",			efsatDxSAT_CODERATE_3_4			},
		{	"5/6",			efsatDxSAT_CODERATE_5_6			},
		{	"7/8",			efsatDxSAT_CODERATE_7_8			},
		{	"3/5",			efsatDxSAT_CODERATE_3_5			},
		{	"4/5",			efsatDxSAT_CODERATE_4_5			},
		{	"5/11",			efsatDxSAT_CODERATE_5_11		},
		{	"6/7",			efsatDxSAT_CODERATE_6_7			},
		{	"8/9",			efsatDxSAT_CODERATE_8_9			},
		{	"9/10",			efsatDxSAT_CODERATE_9_10		},
		{	NULL,			efsatDxSAT_CODERATE_AUTO		}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_SATPolar_Table =
{
	efsatDxTAG_Pair,
	{
		{	"auto",			efsatDxSAT_POLAR_AUTO			},
		{	"horizontal",	efsatDxSAT_POLAR_HOR			},
		{	"vertical",		efsatDxSAT_POLAR_VER			},
		{	"left",			efsatDxSAT_POLAR_LEFT			},
		{	"right",		efsatDxSAT_POLAR_RIGHT			},
		{	NULL,			efsatDxSAT_POLAR_AUTO			}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_SATTransSpec_Table =
{
	efsatDxTAG_Pair,
	{
		{	"dvb-s",		efsatDxSAT_TRANS_DVBS			},
		{	"dvb-s2",		efsatDxSAT_TRANS_DVBS2			},
		{	NULL,			efsatDxSAT_TRANS_DVBS			}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_SATPSKModu_Table =
{
	efsatDxTAG_Pair,
	{
		{	"auto",			efsatDxSAT_PSK_AUTO			},
		{	"qpsk",			efsatDxSAT_PSK_QPSK			},
		{	"8psk",			efsatDxSAT_PSK_8PSK			},
		{	NULL,			efsatDxSAT_PSK_AUTO			}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_SATPilot_Table =
{
	efsatDxTAG_Pair,
	{
		{	"auto",			efsatDxSAT_PILOT_AUTO			},
		{	"on",			efsatDxSAT_PILOT_ON				},
		{	"off",			efsatDxSAT_PILOT_OFF			},
		{	NULL,			efsatDxSAT_PILOT_AUTO			}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_SATRollOnOff_Table =
{
	efsatDxTAG_Pair,
	{
		{	"0.20",			efsatDxSAT_ROLL_020			},
		{	"0.25",			efsatDxSAT_ROLL_025			},
		{	"0.35",			efsatDxSAT_ROLL_035			},
		{	NULL,			efsatDxSAT_ROLL_020			}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_AntennaType_Table =
{
	efsatDxTAG_Pair,
	{
		{	"lnb",			efsatDxANT_TYPE_LNB_ONLY		},
		{	"diseqc",		efsatDxANT_TYPE_DISEQC			},
		{	"scd",			efsatDxANT_TYPE_SCD				},
		{	"smatv",		efsatDxANT_TYPE_SMATV			},
		{	"monoblock",	efsatDxANT_TYPE_MONOBLOCK_LNB	},
		{	NULL,			efsatDxANT_TYPE_LNB_ONLY		}
	}
};
static fsatDxTAG2ENUM_t		fsatDxCH_CABConst_Table =
{
	efsatDxTAG_Pair,
	{
		{	"auto",			efsatDxCAB_CONSTELLATION_AUTO		},
		{	"16qam",		efsatDxCAB_CONSTELLATION_16QAM		},
		{	"32qam",		efsatDxCAB_CONSTELLATION_32QAM		},
		{	"64qam",		efsatDxCAB_CONSTELLATION_64QAM		},
		{	"128qam",		efsatDxCAB_CONSTELLATION_128QAM	},
		{	"256qam",		efsatDxCAB_CONSTELLATION_256QAM	},
		{	NULL,			efsatDxCAB_CONSTELLATION_AUTO		}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_CABSpectrum_Table =
{
	efsatDxTAG_Pair,
	{
		{	"auto",			efsatDxCAB_SPECTRUM_AUTO			},
		{	"normal",		efsatDxCAB_SPECTRUM_NORMAL			},
		{	"inverted",		efsatDxCAB_SPECTRUM_INVERTED		},
		{	NULL,			efsatDxCAB_SPECTRUM_AUTO			}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_TERBandWidth_Table =
{
	efsatDxTAG_Pair,
	{
		{	"8m",			efsatDxTER_BANDWIDTH_8M		},
		{	"7m",			efsatDxTER_BANDWIDTH_7M		},
		{	"6m",			efsatDxTER_BANDWIDTH_6M		},
		{	"t2_5m",		efsatDxTER_T2BANDWIDTH_5MHZ	},
		{	"t2_10m",		efsatDxTER_T2BANDWIDTH_10MHZ	},
		{	"t2_7m",		efsatDxTER_T2BANDWIDTH_1P7MHZ	},
		{	NULL,			0						}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_TERConst_Table =
{
	efsatDxTAG_Pair,
	{
		{	"auto",			efsatDxTER_CONSTELLATION_AUTO		},
		{	"qpsk",			efsatDxTER_CONSTELLATION_QPSK		},
		{	"16qam",		efsatDxTER_CONSTELLATION_16QAM		},
		{	"64qam",		efsatDxTER_CONSTELLATION_64QAM		},
		{	"256qam",		efsatDxTER_CONSTELLATION_256QAM	},
		{	NULL,			0							}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_TERHier_Table =
{
	efsatDxTAG_Pair,
	{
		{	"auto",			efsatDxTER_HIERACHY_AUTO			},
		{	"none",			efsatDxTER_HIERACHY_NONE			},
		{	"1",			efsatDxTER_HIERACHY_1				},
		{	"2",			efsatDxTER_HIERACHY_2				},
		{	"4",			efsatDxTER_HIERACHY_4				},
		{	NULL,			efsatDxTER_HIERACHY_NONE			}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_TERCodeRate_Table =
{
	efsatDxTAG_Pair,
	{
		{	"auto",			efsatDxTER_CODERATE_AUTO			},
		{	"1/2",			efsatDxTER_CODERATE_1_2			},
		{	"2/3",			efsatDxTER_CODERATE_2_3			},
		{	"3/4",			efsatDxTER_CODERATE_3_4			},
		{	"5/6",			efsatDxTER_CODERATE_5_6			},
		{	"7/8",			efsatDxTER_CODERATE_7_8			},
		{	NULL,			efsatDxTER_CODERATE_AUTO			}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_TERTransMode_Table =
{
	efsatDxTAG_Pair,
	{
		{	"auto",			efsatDxTER_TRANSMODE_AUTO			},
		{	"2k",			efsatDxTER_TRANSMODE_2K			},
		{	"8k",			efsatDxTER_TRANSMODE_8K			},
		{	"t2_1k",		efsatDxTER_T2TRANSMODE_1K			},
		{	"t2_4k",		efsatDxTER_T2TRANSMODE_4K			},
		{	"t2_16k",		efsatDxTER_T2TRANSMODE_16K			},
		{	"t2_32k",		efsatDxTER_T2TRANSMODE_32K			},
		{	NULL,			efsatDxTER_TRANSMODE_AUTO			}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_TERGuardInter_Table =
{
	efsatDxTAG_Pair,
	{
		{	"auto",			efsatDxTER_GUARDINTERVAL_AUTO		},
		{	"1/4",			efsatDxTER_GUARDINTERVAL_1_4		},
		{	"1/8",			efsatDxTER_GUARDINTERVAL_1_8		},
		{	"1/16",			efsatDxTER_GUARDINTERVAL_1_16		},
		{	"1/32",			efsatDxTER_GUARDINTERVAL_1_32		},
		{	"1/128",		efsatDxTER_T2GUARDINTERVAL_1_128	},
		{	"19/128",		efsatDxTER_T2GUARDINTERVAL_19_128	},
		{	"19/256",		efsatDxTER_T2GUARDINTERVAL_19_256	},
		{	NULL,			efsatDxTER_GUARDINTERVAL_AUTO		}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_TERStream_Table =
{
	efsatDxTAG_Pair,
	{
		{	"hp",			efsatDxTER_STREAM_HP				},
		{	"lp",			efsatDxTER_STREAM_LP				},
		{	NULL,			0							}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_TEROffset_Table =
{
	efsatDxTAG_Pair,
	{
		{	"auto",			efsatDxTER_OFFSET_AUTO				},
		{	"none",			efsatDxTER_OFFSET_NONE				},
		{	"+1/6mhz",		efsatDxTER_OFFSET_1_6_P			},
		{	"-1/6mhz",		efsatDxTER_OFFSET_1_6_N			},
		{	"+2/6mhz",		efsatDxTER_OFFSET_2_6_P			},
		{	"-2/6mhz",		efsatDxTER_OFFSET_2_6_N			},
		{	"+3/6mhz",		efsatDxTER_OFFSET_3_6_P			},
		{	"-3/6mhz",		efsatDxTER_OFFSET_3_6_N			},
		{	NULL,			efsatDxTER_OFFSET_AUTO				}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_TERSystem_Table =
{
	efsatDxTAG_Pair,
	{
		{	"dvbt",			efsatDxTER_DVB_T					},
		{	"dvbt2",		efsatDxTER_DVB_T2					},
		{	NULL,			efsatDxTER_DVB_T					}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_TER2Preamble_Table =
{
	efsatDxTAG_Pair,
	{
		{	"siso",			efsatDxTER_SISO					},
		{	"miso",			efsatDxTER_MISO					},
		{	NULL,			0							}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_TER2Mixed_Table =
{
	efsatDxTAG_Pair,
	{
		{	"not-mixed",		efsatDxTER_NOT_MIXED				},
		{	"mixed",			efsatDxTER_MIXED					},
		{	NULL,				efsatDxTER_NOT_MIXED				}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_TER2PilotPattern_Table =
{
	efsatDxTAG_Pair,
	{
		{	"auto",			efsatDxTER_PP_AUTO					},
		{	"pp1",			efsatDxTER_PP1						},
		{	"pp2",			efsatDxTER_PP2						},
		{	"pp3",			efsatDxTER_PP3						},
		{	"pp4",			efsatDxTER_PP4						},
		{	"pp5",			efsatDxTER_PP5						},
		{	"pp6",			efsatDxTER_PP6						},
		{	"pp7",			efsatDxTER_PP7						},
		{	"pp8",			efsatDxTER_PP8						},
		{	NULL,			efsatDxTER_PP_AUTO					}
	}
};

static fsatDxTAG2ENUM_t		DXCH_TER2ExtendCarrier_Table =
{
	efsatDxTAG_Pair,
	{
		{	"used",			efsatDxTER_T2USED					},
		{	"notused",		efsatDxTER_T2NOT_USED				},
		{	NULL,			efsatDxTER_T2NOT_USED				}
	}
};

static fsatDxTAG2ENUM_t		fsatDxCH_TER2ParaReduction_Table =
{
	efsatDxTAG_Pair,
	{
		{	"none",			efsatDxTER_REDUCTION_NONE			},
		{	"ace",			efsatDxTER_ACE						},
		{	"tr",			efsatDxTER_TR						},
		{	"ave_tr",		efsatDxTER_AVE_TR					},
		{	NULL,			efsatDxTER_REDUCTION_NONE			}
	}
};

static fsatDxPROP2ENUM_t		fsatDxsqc_channel_sattpinfo_table[] =
{
	{	"frequency",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxSat_TuningTPInfo_t, ulFrequency),		4},
	{	"symbolrate",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxSat_TuningTPInfo_t, ulSymbolRate),		4},
	{	"fec",			&fsatDxCH_SATFEC_Table,			offsetof(fsatDxSat_TuningTPInfo_t, fecCodeRate),		4},
	{	"polar",			&fsatDxCH_SATPolar_Table,		offsetof(fsatDxSat_TuningTPInfo_t, polarization),		4},
	{	"transspec",		&fsatDxCH_SATTransSpec_Table,	offsetof(fsatDxSat_TuningTPInfo_t, transSpec),			4},
	{	"pskmod",			&fsatDxCH_SATPSKModu_Table,		offsetof(fsatDxSat_TuningTPInfo_t, pskMod),			4},
	{	"pilot",			&fsatDxCH_SATPilot_Table,		offsetof(fsatDxSat_TuningTPInfo_t, pilot),				4},
	{	"rolloff",		&fsatDxCH_SATRollOnOff_Table,	offsetof(fsatDxSat_TuningTPInfo_t, rollOff),			4},
//	{	"ant_uid",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxSat_TuningTPInfo_t, antuid),			4},
	{	"sattype",		&fsatDxOBJDEC_STRING_Table,		offsetof(fsatDxSat_TuningTPInfo_t, satType),		fsatDxANTINFO_NAME_LEN},
	{	NULL,			NULL,						0,													0}
};



static fsatDxPROP2ENUM_t		fsatDxsqc_channel_cabinfo_table[] =
{
	{	"type",			&fsatDxCH_DeliveryType_Table,	offsetof(fsatDxCabTuningInfo_t, deliveryType),		4},
	{	"group",			&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxCabTuningInfo_t, ulTunerGroupId),		4},
	{	"frequency",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxCabTuningInfo_t, ulFrequency),			4},
	{	"symbolrate",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxCabTuningInfo_t, ulSymbolRate),		4},
	{	"constellation",	&fsatDxCH_CABConst_Table,		offsetof(fsatDxCabTuningInfo_t, constellation),		4},
	{	"spectrum",		&fsatDxCH_CABSpectrum_Table,		offsetof(fsatDxCabTuningInfo_t, spectrum),			4},
	{	"tsid",			&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxCabTuningInfo_t, nTsId),				4},
	{	"netid",			&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxCabTuningInfo_t, nNetId),				4},
	{	NULL,			NULL,						0,													4}
};
static fsatDxPROP2ENUM_t		fsatDxsqc_channel_satinfo_table[] =
{
	{	"type",			&fsatDxCH_DeliveryType_Table,	offsetof(fsatDxSatTuningInfo_t, deliveryType),		4},
	{	"group",			&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxSatTuningInfo_t, ulTunerGroupId),		4},
	{	"antennatype",	&fsatDxCH_AntennaType_Table,		offsetof(fsatDxSatTuningInfo_t, antennaType),			4},
	{	NULL,			NULL,						0,													0}
};


static fsatDxPROP2ENUM_t		fsatDxsqc_channel_terinfo_table[] =
{
	{	"type",			&fsatDxCH_DeliveryType_Table,	offsetof(fsatDxTerTuningInfo_t, deliveryType),		4},
	{	"group",			&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxTerTuningInfo_t, ulTunerGroupId),		4},
	{	"frequency",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxTerTuningInfo_t, ulFrequency),			4},
	{	"bandwidth",		&fsatDxCH_TERBandWidth_Table,	offsetof(fsatDxTerTuningInfo_t, bandWidth),			4},
	{	"constellation",	&fsatDxCH_TERConst_Table,		offsetof(fsatDxTerTuningInfo_t, constellation),		4},
	{	"hierachy",		&fsatDxCH_TERHier_Table,			offsetof(fsatDxTerTuningInfo_t, hierachy),			4},
	{	"coderate",		&fsatDxCH_TERCodeRate_Table,		offsetof(fsatDxTerTuningInfo_t, codeRate),			4},
	{	"transmode",		&fsatDxCH_TERTransMode_Table,	offsetof(fsatDxTerTuningInfo_t, transMode),			4},
	{	"guardinterval",	&fsatDxCH_TERGuardInter_Table,	offsetof(fsatDxTerTuningInfo_t, guardInterval),		4},
	{	"stream",			&fsatDxCH_TERStream_Table,		offsetof(fsatDxTerTuningInfo_t, stream),				4},
	{	"offset",			&fsatDxCH_TEROffset_Table,		offsetof(fsatDxTerTuningInfo_t, offset),				4},
	{	"system",			&fsatDxCH_TERSystem_Table,		offsetof(fsatDxTerTuningInfo_t, system),				4},
	{	NULL,			NULL,						0,													0}
};
static	fsatDxPROP2ENUM_t		fsatDxmeta_transponder_table[] =
{
	{	"delitype",		&fsatDxCH_DeliveryType_Table,	offsetof(fsatDxTransponder_t, deliType),	4	},
	{	"ts_uid",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxTransponder_t, uid),		4	},
	{	"net_uid",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxTransponder_t, netuid),	4	},
	{	"tsid",			&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxTransponder_t, tsid),		4	},
	{	"onid",			&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxTransponder_t, onid),		4	},
	{	"tuner_id",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxTransponder_t, tunerid),	4	},

	{	NULL,			NULL,						0,									0	}
};

static	fsatDxPROP2ENUM_t		fsatDxmeta_provider_table[] =
{
	{	"prv_uid",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxProvider_t, uid),		4	},
	{	"prv_name",		&fsatDxOBJDEC_STRING_Table,	    offsetof(fsatDxProvider_t, name),	fsatDxNAME_LEN	},
	{	NULL,			NULL,						0,									0	}
};

static	fsatDxPROP2ENUM_t		fsatDxmeta_group_table[] =
{
	{	"group_uid",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxGroup_t, uid),		4	},
	{	"group_id",			&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxGroup_t, id),		4	},
	{	"group_name",		&fsatDxOBJDEC_STRING_Table,	    offsetof(fsatDxGroup_t, name),	fsatDxNAME_LEN	},
	{	NULL,			NULL,						0,									0	}
};

static	fsatDxPROP2ENUM_t		fsatDxmeta_bouquet_table[] =
{
	{	"bouquet_uid",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxBouquet_t, uid),		4	},
	{	"bouquet_id",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxBouquet_t, id),		4	},
	{	"bouquet_name",		&fsatDxOBJDEC_STRING_Table,	    offsetof(fsatDxBouquet_t, name), fsatDxNAME_LEN	},
	{	NULL,			NULL,						0,									0	}
};

static	fsatDxPROP2ENUM_t		fsatDxmeta_network_table[] =
{
	{	"net_uid",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxNetwork_t, uid),		    4	},
	{	"onid",			&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxNetwork_t, onid),		4	},
	{	"net_name",		&fsatDxOBJDEC_STRING_Table,	    offsetof(fsatDxNetwork_t, name),		fsatDxNAME_LEN	},
	{	"delitype",		&fsatDxCH_DeliveryType_Table,	offsetof(fsatDxNetwork_t, deliType),	4	},
	{	NULL,			NULL,						0,									0	}
};

static fsatDxTAG2ENUM_t		fsatDxmeta_action_table =
{
	efsatDxTAG_Pair,
	{
		{	"watch",	eDxSCHED_ACTION_WATCH	},
		{	"record",	eDxSCHED_ACTION_RECORD	},
		{	"power",	eDxSCHED_ACTION_POWER	},
		{	"check",	eDxSCHED_ACTION_CHECK	},
		{	NULL,		eDxSCHED_ACTION_NONE	}
	}
};

static fsatDxTAG2ENUM_t		fsatDxmeta_date_table =
{
	efsatDxTAG_Pair,
	{
		{	"sun",	eDxSCHED_DATE_SUN	},
		{	"mon",	eDxSCHED_DATE_MON	},
		{	"tue",	eDxSCHED_DATE_TUE	},
		{	"wed",	eDxSCHED_DATE_WED	},
		{	"thu", 	eDxSCHED_DATE_THU	},
		{	"fri", 	eDxSCHED_DATE_FRI	},
		{	"sat",	eDxSCHED_DATE_SAT	},
		{	NULL,	eDxSCHED_DATE_NONE	}
	}
};

static fsatDxTAG2ENUM_t		fsatDxmeta_cycle_table =
{
	efsatDxTAG_Pair,
	{
		{	"day",		eDxSCHED_CYCLE_DAYLY	},
		{	"week",		eDxSCHED_CYCLE_WEEKLY	},
		{	"month",	eDxSCHED_CYCLE_MONTHLY	},
		{	"year",		eDxSCHED_CYCLE_YEARLY	},
		{	NULL,		eDxSCHED_CYCLE_NONE		}
	}
};

static fsatDxPROP2ENUM_t 	fsatDxmeta_sched_power_table[] =
{
	{	"volume", 	&fsatDxOBJDEC_INT_Table,	offsetof(DxSCHED_POWER_t, volume),	sizeof(HINT32)},
	{	"onoff", 	&fsatDxOBJDEC_BOOL_Table,	offsetof(DxSCHED_POWER_t, onOff),	0},
	{	NULL,		NULL,				0,										0}
};


static fsatDxPROP2ENUM_t		fsatDxmeta_sched_watch_table[] =
{
	{"programid",	&fsatDxOBJDEC_INT_Table, 	offsetof(DxSCHED_WATCH_t, programId),	sizeof(HINT32)},
	{"seriesid", 	&fsatDxOBJDEC_INT_Table,	offsetof(DxSCHED_WATCH_t, seriesId),		sizeof(HINT32)},

	{NULL,		NULL,						0,										0}
};

static fsatDxPROP2ENUM_t		fsatDxmeta_sched_rec_table[] =
{
	{"programid",		&fsatDxOBJDEC_INT_Table, 	offsetof(DxSCHED_REC_t, programId),		sizeof(HINT32)},
	{"seriesid", 		&fsatDxOBJDEC_STRING_Table,	offsetof(DxSCHED_REC_t, seriesId),		DxPVR_TVANYTIME_CRID_TOTAL_LEN},
	{"seriesrec", 		&fsatDxOBJDEC_BOOL_Table,	offsetof(DxSCHED_REC_t, seriesRec),		0},
	{"startpadding", 	&fsatDxOBJDEC_INT_Table,	offsetof(DxSCHED_REC_t, startPadding),	sizeof(HUINT32)},
	{"endpadding", 		&fsatDxOBJDEC_INT_Table,	offsetof(DxSCHED_REC_t, endPadding),	sizeof(HUINT32)},

	{NULL,		NULL,						0,										0}
};


static fsatDxPROP2ENUM_t		fsatDxmeta_schedule_table[] =
{
	{"uid",			&fsatDxOBJDEC_INT_Table, 	offsetof(DxSCHEDULE_t, uid),			sizeof(HINT32)},
	{"eventbase",	&fsatDxOBJDEC_BOOL_Table, 	offsetof(DxSCHEDULE_t, eventBase),	0},
	{"eventid", 	&fsatDxOBJDEC_INT_Table,	offsetof(DxSCHEDULE_t, eventId),		sizeof(HINT32)},
	{"starttime", 	&fsatDxOBJDEC_INT_Table,	offsetof(DxSCHEDULE_t, startTime),	sizeof(HUINT32)},
	{"duration", 	&fsatDxOBJDEC_INT_Table,	offsetof(DxSCHEDULE_t, duration),	sizeof(HUINT32)},
	{"prestart", 	&fsatDxOBJDEC_INT_Table,	offsetof(DxSCHEDULE_t, preStart),	sizeof(HUINT32)},
	{"svcuid",		&fsatDxOBJDEC_INT_Table,	offsetof(DxSCHEDULE_t, svcUId),		sizeof(HINT32)},
	{"action",		&fsatDxmeta_action_table,	offsetof(DxSCHEDULE_t, action),		0},

	{NULL,		NULL,						0,										0}
};


#if defined(CONFIG_OP_FREESAT)
static	fsatDxPROP2ENUM_t		s_astReclist_RECGuidancePropertyTable[] =
{
	{	"type",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxFreesat_Pvr_Guidance_t, eType),	1},
	{	"langcode",	&fsatDxOBJDEC_STRING_Table,		offsetof(fsatDxFreesat_Pvr_Guidance_t, stLangCode),	fsatDxPVR_LANG_ISO_639_LEN+1},
	{	"data",	    &fsatDxOBJDEC_STRING_Table,		offsetof(fsatDxFreesat_Pvr_Guidance_t, stData),	fsatDxFREESAT_PVR_GUIDANCE_DATA_MAX + 1},
	{	NULL,				NULL,						0,											0}
};
#endif	// CONFIG_OP_FREESAT

static	fsatDxPROP2ENUM_t		s_astReclist_RECEventPropertyTable[] =
{
	{	"starttime",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxRecList_EVTData_t, ulStartTime),	4},
	{	"duration",		    &fsatDxOBJDEC_INT_Table,		offsetof(fsatDxRecList_EVTData_t, ulDuration),		4},
	{	"contentnibble",	&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxRecList_EVTData_t, ucContentNibble),	4},
	{	"usernibble",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxRecList_EVTData_t, ucUserNibble),		4},
	{	"text",				&fsatDxOBJDEC_STRING_Table,		offsetof(fsatDxRecList_EVTData_t, aucText),			fsatDxPVR_EVENT_TEXT},
	{	"seriesid", 		&fsatDxOBJDEC_STRING_Table, 	offsetof(fsatDxRecList_EVTData_t, aucSeriesId),	DxPVR_TVANYTIME_CRID_TOTAL_LEN},

	{	NULL,				NULL,						0,											0}
};

static	fsatDxPROP2ENUM_t		s_Reclist_RECExtEventPropertyTable[] =
{
	{	"desnum",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxRecList_ExtEvt_t, ucDesNum),		4},
	{	"lastdesnum",	&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxRecList_ExtEvt_t, ucLastDesNum),	4},
	{	"langcode",		&fsatDxOBJDEC_STRING_Table,	    offsetof(fsatDxRecList_ExtEvt_t, szIso639LangCode),fsatDxPVR_LANG_ISO_639_LEN+1},
	{	NULL,				NULL,						0,											0}
};

static	fsatDxPROP2ENUM_t		s_Reclist_RECExtEventItemPropertyTable[] =
{
	{	"desc",	&fsatDxOBJDEC_STRING_Table,	offsetof(fsatDxRecList_Ext_ItemData_t, aucDescription),	fsatDxPVR_EVENT_TEXT},
	{	"item",	&fsatDxOBJDEC_STRING_Table,	offsetof(fsatDxRecList_Ext_ItemData_t, aucItem),		fsatDxPVR_EVENT_TEXT},
	{	NULL,				NULL,						0,											0}
};

static fsatDxTAG2ENUM_t		fsatDxOBJDEC_REC_STS_Table =
{
	efsatDxTAG_Pair,
	{
		{	"rec_sts_start",		efsatDxREC_STS_RECORD_START },
		{	"rec_sts_recording",	efsatDxREC_STS_RECORDING},
		{	"rec_sts_recorded",		efsatDxREC_STS_RECORDED},
		{	"rec_sts_incompleted",	efsatDxREC_STS_INCOMPLETED},
		{	NULL,					-1}
	}
};

static fsatDxTAG2ENUM_t		fsatDxOBJDEC_REC_EndReason_Table =
{
	efsatDxTAG_Pair,
	{
		{	"rec_end_nofail",		efsatDxREC_FAILED_NOFAIL },
		{	"rec_end_hddfull",		efsatDxREC_FAILED_HDDFULL	},
		{	"rec_end_maxnum",		efsatDxREC_FAILED_MAXNUM	},
		{	"rec_end_maxrecordlistnum",efsatDxREC_FAILED_MAXRECORDLISTNUM},
		{	"rec_end_less30sec",	efsatDxREC_FAILED_LESS30SEC	},
		{	"rec_end_nosignal",		efsatDxREC_FAILED_NOSIGNAL	},
		{	"rec_end_nonservice",	efsatDxREC_FAILED_NONSERVICE	},
		{	"rec_end_searching",	efsatDxREC_FAILED_SEARCHING	},
		{	"rec_end_conflict_tp",	efsatDxREC_FAILED_CONFLICT_TP	},
		{	"rec_end_drm",			efsatDxREC_FAILED_DRM			},
		{	"rec_end_nosc_scr",		efsatDxREC_FAILED_NOSC_SCR	},
		{	"rec_end_noright_scr",	efsatDxREC_FAILED_NORIGHT_SCR	},
		{	"rec_end_nohdd",		efsatDxREC_FAILED_NOHDD		},
		{	"rec_end_hddfull",		efsatDxREC_INCOMPLETE_HDDFULL	},
		{	"rec_end_unknown",		efsatDxREC_FAILD_UNKNOWN	},
		{	"rec_end_nousbhdd",		efsatDxREC_FAILED_NOUSBHDD	},
		{	"rec_end_disconnectusbhdd",	efsatDxREC_FAILED_DISCONNECTUSBHDD	},
		{	"rec_end_extusb_copydel",	efsatDxREC_FAILED_EXTUSB_CopyDel	},
		{	"rec_end_same_svc_recording",	efsatDxREC_FAILED_SAME_SVC_RECORDING	},
		{	"rec_end_higher_priority",		efsatDxREC_FAILED_HIGHER_PRIORITY	},
		{	"rec_end_unable_tracking",		efsatDxREC_FAILED_UNABLE_TRACKING	},
		{	NULL,					-1	}
	}
};




static fsatDxPROP2ENUM_t 		s_Reclist_RECBookmarkPropertyTable[] =
{
	{"pos",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxRecList_Bookmark_t, ulPos), 	4},
	{"name",	&fsatDxOBJDEC_STRING_Table,	offsetof(fsatDxRecList_Bookmark_t, szName),	fsatDxRECLIST_BOOKMARK_NAME_LEN},
	{NULL,		NULL,						0,												0}
};

static	fsatDxPROP2ENUM_t		s_Reclist_RECParentPropertyTable[] =
{
	{	"isocountrycode",	&fsatDxOBJDEC_STRING_Table,	offsetof(fsatDxPvr_ParentalRating_t, aucIsoCountryCode),fsatDxPVR_LANG_ISO_639_LEN+1},
	{	"prvalue",			&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxPvr_ParentalRating_t, ucParentalRating),	4},
	{	NULL,				NULL,						0,											0}
};

static fsatDxPROP2ENUM_t 		s_Reclist_RECDataPropertyTable[] =
{
	{"url",			&fsatDxOBJDEC_STRING_Table,	offsetof(fsatDxRecListData_t, szUrl),			fsatDxRECLIST_URL},
	{"display name",&fsatDxOBJDEC_STRING_Table,	offsetof(fsatDxRecListData_t, szName),			fsatDxRECLIST_NAME_LEN},
	{"thumbnail",	&fsatDxOBJDEC_STRING_Table,	offsetof(fsatDxRecListData_t, szThumbnailPath),	fsatDxRECLIST_URL},

	{"start time",	&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxRecListData_t, ulRecStartTime), 	4},
	{"duration",	&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxRecListData_t, ulRecDuration), 	4},
	{"org duration",	&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxRecListData_t, ulRecOrgDuration), 	4},

	{"status",		&fsatDxOBJDEC_REC_EndReason_Table,	offsetof(fsatDxRecListData_t, eRecStatus), 		4},
	{"end reason",	&fsatDxOBJDEC_REC_EndReason_Table,	offsetof(fsatDxRecListData_t, eRecEndReason), 	4},
	{"scrambled",	&fsatDxOBJDEC_BOOL_Table,	offsetof(fsatDxRecListData_t, bScrambled), 		4},
	{"encrypted",	&fsatDxOBJDEC_BOOL_Table,	offsetof(fsatDxRecListData_t, bEncrypted), 		4},
	{"eventbased",	&fsatDxOBJDEC_BOOL_Table,	offsetof(fsatDxRecListData_t, bEventBased), 	4},
	{"eventid",		&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxRecListData_t, usEventId), 		4},
	{"seriesrec",	&fsatDxOBJDEC_BOOL_Table,	offsetof(fsatDxRecListData_t, bSeriesRec), 	4},

	{"last view time",	&fsatDxOBJDEC_INT_Table,		offsetof(fsatDxRecListData_t, ulLastViewTime), 	4},
	{"userlock",		&fsatDxOBJDEC_BOOL_Table,	offsetof(fsatDxRecListData_t, bUserLock), 		4},
	{"willbedeleted",	&fsatDxOBJDEC_BOOL_Table,	offsetof(fsatDxRecListData_t, bWillBeDeleted), 	4},
	{"autodeleted",		&fsatDxOBJDEC_BOOL_Table,	offsetof(fsatDxRecListData_t, bAudoDeleted), 	4},
	{"played",			&fsatDxOBJDEC_BOOL_Table,	offsetof(fsatDxRecListData_t, bPlayed), 		4},
	{"blocked",			&fsatDxOBJDEC_BOOL_Table,	offsetof(fsatDxRecListData_t, bBlocked), 		4},

	{"svc name",		&fsatDxOBJDEC_STRING_Table,		offsetof(fsatDxRecListData_t, szSvcName),		fsatDxRECLIST_NAME_LEN},
	{"svc type",		&fsatDxOBJDEC_SVCTYPE_Table,		offsetof(fsatDxRecListData_t, eSvcType), 		4},
	{"svc UId",			&fsatDxOBJDEC_INT_Table,			offsetof(fsatDxRecListData_t, svcUId), 			4},
	{"svc video type",	&fsatDxOBJDEC_VIDEOTYPE_Table,	offsetof(fsatDxRecListData_t, eVideoType), 		4},
	{"svc cas type",	&fsatDxOBJDEC_CASTYPE_Table,		offsetof(fsatDxRecListData_t, eCasType), 		4},

	{"numofbookmark",			&fsatDxOBJDEC_INT_Table,	offsetof(fsatDxRecListData_t, usNumOfBookmark), 		4},

	{NULL,			NULL,							0,									0}
};




static HCHAR*  fsatDxOBJDEC_EncodeEnum(fsatDxTAGLIST_t *taglist, HUINT32 data)
{
    while (taglist->tag)
    {
        if (taglist->value == data)
            return taglist->tag;

        taglist++;
    }
    return NULL;
}

static void    fsatDxOBJDEC_DecodeEnum(fsatDxTAG2ENUM_t *table, HxJSON_t object, HUINT32 write_addr, HUINT32 size)
{
    fsatDxTAGLIST_t   *taglist = table->taglist;

    if (size == 0)
        size = 4;

    switch (table->tag_type)
    {
        case    efsatDxTAG_Pair:
            if (HxJSON_IsString(object))
            {
                const HCHAR *value = HxJSON_String_Get(object);
                while (taglist->tag)
                {
                    if (HLIB_STD_StrCaseCmp(taglist->tag, value) == 0)
                    {
                        HxSTD_MemCopy((HUINT32*)write_addr, &taglist->value, size);
                        return;
                    }

                    taglist++;
                }
            }
            break;
        case    efsatDxTAG_Integer:
            if (HxJSON_IsInteger(object))
            {
                HUINT32 integer = HxJSON_Integer_Get(object);
                HxSTD_MemCopy((HUINT32*)write_addr, &integer, size);
            }
            break;
        case    efsatDxTAG_Float:
            if (HxJSON_IsReal(object))
            {
                float   value = HxJSON_Real_Get(object);
                HxSTD_MemCopy((HUINT32*)write_addr, &value, size);
            }
            break;
        case    efsatDxTAG_String:
            if (HxJSON_IsString(object))
            {
                const HCHAR *value = HxJSON_String_Get(object);
#ifdef USE_JSON_URL
                HLIB_STD_StrNCpySafe((HCHAR*)write_addr, value, size);
#else
                size = (size%4) == 0 ?  size: (4 - (size% 4))+ size;
                HLIB_MATH_DecodeBase64((HCHAR*)write_addr,value,size);
#endif
            }
            break;
        case    efsatDxTAG_Bool:
            if (HxJSON_IsBoolean(object))
            {
                HBOOL   value = HxJSON_Boolean_Get(object);
                HxSTD_MemCopy((HBOOL*)write_addr, &value, size);
            }
            break;
        case    efsatDxTAG_Binary:
            if (HxJSON_IsString(object))
            {
                const HCHAR *value = HxJSON_String_Get(object);
                HLIB_MATH_DecodeBase64Ex((void *)write_addr, size, value, strlen(value));
            }
            break;
    }
}

static HUINT8*		fsatDxUTIL_GetTemporaryBuffer(int size, HBOOL *pbAlloc)
{
#define		CIRCULAR_BUFFER_SIZE				0x100000		//	640KB
    static	HUINT8			*_pstCircularBuffer = NULL;
    static	HUINT8			*_pstCircularBufferTail = NULL;

	HUINT8	*p;
	HUINT32	ulTailPtr;

	if (size & 0x03)
		size = ((size >> 2) + 1) << 2;

	if (size >= CIRCULAR_BUFFER_SIZE)
	{
		p = (HUINT8*)HLIB_STD_MemAlloc(size);
		if (pbAlloc)
			*pbAlloc = TRUE;
		else
		{
			HxLOG_Critical("[%s](%d) Circular buffer limit occurred without pbAlloc Flag....\n", __FUNCTION__, __LINE__);
			HxLOG_Critical("[%s](%d) YOU MUST FREE BUFFER INTERNAL!!!\n", __FUNCTION__, __LINE__);
		}
		return p;
	}

	if (_pstCircularBuffer == NULL)
	{
		_pstCircularBuffer = (HUINT8*)HLIB_STD_MemAlloc(CIRCULAR_BUFFER_SIZE);
		_pstCircularBufferTail = _pstCircularBuffer;
	}

	ulTailPtr = CIRCULAR_BUFFER_SIZE - (HUINT32)(_pstCircularBufferTail - _pstCircularBuffer);
	if (ulTailPtr < size)
	{
		_pstCircularBufferTail = _pstCircularBuffer;
	}

	p = _pstCircularBufferTail;
	_pstCircularBufferTail += size;

	if (pbAlloc)
		*pbAlloc = FALSE;

	return p;
}

static	void	fsatDxOBJDEC_EncodeProperty(HINT32 stream, fsatDxPROP2ENUM_t *pstPropTable, HUINT32 base_addr)
{
	HBOOL		bFirst = TRUE;
  	HBOOL		bTmpAlloc = FALSE, bBufAlloc = FALSE,bEncodeAlloc = FALSE;
	HCHAR		*buf, *encodebuf = NULL, *temp;
	HUINT32		data,Len;
	HUINT32		read_addr;
	if(pstPropTable == NULL)
		return;

	temp =fsatDxUTIL_GetTemporaryBuffer(1024, &bTmpAlloc);
	while (pstPropTable->property)
	{
		*temp = 0;

		read_addr = (base_addr + pstPropTable->offset);
		data = *((HUINT32*)read_addr);
		if (pstPropTable->tagtable)
		{
			switch (pstPropTable->tagtable->tag_type)
			{
				case	efsatDxTAG_Pair:
                					buf = fsatDxOBJDEC_EncodeEnum(pstPropTable->tagtable->taglist, data);
                					if (buf)
                						HxSTD_snprintf(temp, 1024, "\"%s\":\"%s\"", pstPropTable->property, buf);
                                	break;
				case	efsatDxTAG_Integer:
                					HxSTD_snprintf(temp, 1024, "\"%s\":%d", pstPropTable->property, data);
                					break;
				case	efsatDxTAG_Float:
                					HxSTD_snprintf(temp, 1024, "\"%s\":%f", pstPropTable->property, *(float *)read_addr);
                					break;
				case	efsatDxTAG_String:
#ifdef USE_JSON_URL
                                    Len = HLIB_STD_URLStrLen( (HCHAR*)read_addr)+1;
                                    encodebuf = fsatDxUTIL_GetTemporaryBuffer(Len, &bEncodeAlloc);
                                    HLIB_STD_URLEncode(encodebuf,(HCHAR*)read_addr,Len);
#else
                                    encodebuf = fsatDxUTIL_GetTemporaryBuffer(1024, NULL);
                                    pstPropTable->sizeofdata = (pstPropTable->sizeofdata%4) == 0 ?  pstPropTable->sizeofdata: (4 - (pstPropTable->sizeofdata % 4))+ pstPropTable->sizeofdata;
                                    HLIB_MATH_EncodeBase64(encodebuf,(void*)read_addr,pstPropTable->sizeofdata );
#endif
                                    HxSTD_snprintf(temp, 1024, "\"%s\":\"%s\"", pstPropTable->property, encodebuf);
                                    if(bEncodeAlloc)
                					{
                						bEncodeAlloc = FALSE;
                						HLIB_STD_MemFree(encodebuf);
                					}
                                    break;
				case	efsatDxTAG_Bool:
                					HxSTD_snprintf(temp, 1024, "\"%s\":%s", pstPropTable->property, (data ? "true" : "false"));
                					break;
				case	efsatDxTAG_Binary:
                					buf = fsatDxUTIL_GetTemporaryBuffer(1024, &bBufAlloc);
                					if (HLIB_MATH_EncodeBase64Ex(buf, 1024, (void *)read_addr, pstPropTable->sizeofdata))
                					{
                						HxSTD_snprintf(temp, 1024, "\"%s\":\"%s\"", pstPropTable->property, buf);
                					}
                                    if(bBufAlloc)
                					{
                						bBufAlloc = FALSE;
                						HLIB_STD_MemFree(buf);
                					}
                					break;
			}
		}

		if (HxSTD_StrLen(temp) > 0)
		{
			if (bFirst)
			{
				bFirst = FALSE;
				HLIB_RWSTREAM_Print(stream, "%s", temp);
			} else
			{
				HLIB_RWSTREAM_Print(stream, ",%s", temp);
			}
		}

		pstPropTable++;
	}
   	if(bTmpAlloc)
		HLIB_STD_MemFree(temp);
}


static void	fsatDxOBJDEC_DecodeObject(void *pstInfo, HxJSON_t root, fsatDxPROP2ENUM_t *pstPropTable)
{
	HxJSON_t	object;

	HUINT32		write_addr, base_addr = (HUINT32)pstInfo;

	while (pstPropTable->property)
	{
		write_addr = (base_addr + pstPropTable->offset);
		object = HxJSON_Object_Get(root, pstPropTable->property);
		if (object)
		{
			if (pstPropTable->tagtable)
				fsatDxOBJDEC_DecodeEnum(pstPropTable->tagtable, object, write_addr, pstPropTable->sizeofdata);
		}

		pstPropTable++;
	}
}

static HCHAR*	fsatDxOBJDEC_EncodeObject(void *pstInfo, HCHAR *object_name, fsatDxPROP2ENUM_t *pstPropTable)
{
	HINT32		stream;
	HUINT32		base_addr = (HUINT32)pstInfo;

	stream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);

	if (object_name)
		HLIB_RWSTREAM_Print(stream, "\"%s\":{", object_name);
	else
		HLIB_RWSTREAM_Print(stream, "{");

	fsatDxOBJDEC_EncodeProperty(stream, pstPropTable, base_addr);

	HLIB_RWSTREAM_Print(stream, "}");

	return (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(stream);
}
static HCHAR*			fsatDxOBJDEC_EncodeObject_array(HCHAR *object_name, HINT32 max_cnt, HINT32 size, HINT32 offset_of_flag, void *pInfo, fsatDxPROP2ENUM_t *table)
{
	HINT32		stream, i;
	HUINT32		array_addr, base_addr = (HUINT32)pInfo;
	HBOOL		bFlag, bFirst = TRUE;

	stream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if (object_name)
		HLIB_RWSTREAM_Print(stream, "\"%s\":", object_name);
	HLIB_RWSTREAM_Print(stream, "[");

	array_addr = base_addr;
	for (i = 0; i < max_cnt; i++)
	{
		if (offset_of_flag == -1)
			bFlag = TRUE;
		else
			bFlag = *(HBOOL*)(array_addr + offset_of_flag);

		if (bFlag)
		{
			if (!bFirst)
				HLIB_RWSTREAM_Print(stream, ",");

			HLIB_RWSTREAM_Print(stream, "{");
			fsatDxOBJDEC_EncodeProperty(stream, table, array_addr);
			HLIB_RWSTREAM_Print(stream, "}");
			bFirst = FALSE;
		}

		array_addr += size;
	}

	HLIB_RWSTREAM_Print(stream, "]");

	return (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(stream);
}


static HCHAR*	fsatDxOBJDEC_EncodeObjectIncludeObject(void *pstInfo, HCHAR *object_name, fsatDxPROP2ENUM_t *pstPropTable, ...)
{
	HINT32		stream;
	HUINT32		base_addr = (HUINT32)pstInfo;

	va_list		ap;
	HCHAR		*object_container;

	stream = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);

	if (object_name)
		HLIB_RWSTREAM_Print(stream, "\"%s\":{", object_name);
	else
		HLIB_RWSTREAM_Print(stream, "{");

	fsatDxOBJDEC_EncodeProperty(stream, pstPropTable, base_addr);

	//
	va_start(ap, pstPropTable);
	object_container = va_arg(ap, HCHAR *);
	while (object_container)
	{
		HLIB_RWSTREAM_Print(stream, ",%s", object_container);
		object_container = va_arg(ap, HCHAR *);
	}
	va_end(ap);
	//
    HLIB_RWSTREAM_Print(stream, "}");

	return (HCHAR*)HLIB_RWSTREAM_CloseWithoutBuf(stream);
}


#define _FUNCTION_________________

#ifdef	CONFIG_OP_FREESAT
static void  local_freesat_service_Decode (void *obj, HxJSON_t json)
{
	fsatDxFreesatService_t	*data;
	const HCHAR	*text;

	data = (fsatDxFreesatService_t *)obj;
	if (data == NULL)
	{
		return;
	}
	memset(data, 0, sizeof(fsatDxFreesatService_t));

	data->svcId      = HLIB_JSON_Object_GetInteger(json, "fsat_svcid");
	data->regionId   = HLIB_JSON_Object_GetInteger(json, "fsat_rgnid");
	data->contentMgr = HLIB_JSON_Object_GetInteger(json, "fta_contentmgr");

	data->ucUserCustomizable = HLIB_JSON_Object_GetInteger(json, "fsat_custom");
	data->ucUserSelectable = HLIB_JSON_Object_GetInteger(json, "fsat_selectable");
	data->ucAdultSvc = HLIB_JSON_Object_GetInteger(json, "fsat_adult");
	data->ucVideoCodecH264 = HLIB_JSON_Object_GetInteger(json, "fsat_videocodec_h264");

	text = HLIB_JSON_Object_GetString(json, "default_auth");
	if (text && HxSTD_StrLen(text))
	{
#ifdef USE_JSON_URL
        HLIB_STD_StrNCpySafe(data->defaultAuth, text, fsatDxDEFAULT_AUTHORITY_LEN);
#else
        HLIB_MATH_DecodeBase64(data->defaultAuth, text, fsatDxDEFAULT_AUTHORITY_LEN +3);
#endif
	}
	text = HLIB_JSON_Object_GetString(json, "fsat_shortname");
	if (text && HxSTD_StrLen(text))
	{
#ifdef USE_JSON_URL
        HLIB_STD_StrNCpySafe(data->szShortName, text, fsatDxFSAT_SVC_SHORT_NAME_LEN);
#else
		HLIB_MATH_DecodeBase64(data->szShortName, text, fsatDxFSAT_SVC_SHORT_NAME_LEN +1);
#endif
	}
	data->ucGuidanceType = HLIB_JSON_Object_GetInteger(json, "guidance_type");
	data->ucGuidanceMode = HLIB_JSON_Object_GetInteger(json, "guidance_mode");
	text = HLIB_JSON_Object_GetString(json, "guidance_str");
	if (text && HxSTD_StrLen(text))
	{
#ifdef USE_JSON_URL
        HLIB_STD_StrNCpySafe(data->szGuidanceStr, text, fsatDxFSAT_SVC_GUIDANCE_LEN);
#else
		HLIB_MATH_DecodeBase64(data->szGuidanceStr, text, fsatDxFSAT_SVC_GUIDANCE_LEN +1);
#endif
	}
}

static HCHAR *	local_freesat_service_Encode (const void *obj)
{
	const fsatDxFreesatService_t *data = (const fsatDxFreesatService_t *)obj;
	HINT32	sd,Len;
    HCHAR* buf;
	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if (!sd)
	{
		HxLOG_Error("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	HLIB_RWSTREAM_Print(sd, "\"fsat_svcid\":%d", data->svcId);
	HLIB_RWSTREAM_Print(sd, ",\"fsat_rgnid\":%d", data->regionId);
	HLIB_RWSTREAM_Print(sd, ",\"fta_contentmgr\":%d", data->contentMgr);

	HLIB_RWSTREAM_Print(sd, ",\"fsat_custom\":%d", data->ucUserCustomizable);
	HLIB_RWSTREAM_Print(sd, ",\"fsat_selectable\":%d", data->ucUserSelectable);
	HLIB_RWSTREAM_Print(sd, ",\"fsat_adult\":%d", data->ucAdultSvc);
	HLIB_RWSTREAM_Print(sd, ",\"fsat_videocodec_h264\":%d", data->ucVideoCodecH264);

#ifdef USE_JSON_URL
    Len = HLIB_STD_URLStrLen(data->defaultAuth) + 1;
    buf = fsatDxUTIL_GetTemporaryBuffer(Len, NULL);
    HLIB_STD_URLEncode(buf,data->defaultAuth,Len);
    HLIB_RWSTREAM_Print(sd, ",\"default_auth\":\"%s\"",buf);
#else
	if (HxSTD_StrLen(data->defaultAuth))
	{
        buf = fsatDxUTIL_GetTemporaryBuffer(1024, NULL);
        HLIB_MATH_EncodeBase64(buf,data->defaultAuth,fsatDxDEFAULT_AUTHORITY_LEN +3);
		HLIB_RWSTREAM_Print(sd, ",\"default_auth\":\"%s\"",buf);
	}
#endif

#ifdef USE_JSON_URL
    Len = HLIB_STD_URLStrLen(data->szShortName) + 1;
    buf = fsatDxUTIL_GetTemporaryBuffer(Len, NULL);
    HLIB_STD_URLEncode(buf,data->szShortName,Len);
    HLIB_RWSTREAM_Print(sd, ",\"fsat_shortname\":\"%s\"",buf);
#else
	if (HxSTD_StrLen(data->szShortName))
	{
        buf = fsatDxUTIL_GetTemporaryBuffer(1024, NULL);
        HLIB_MATH_EncodeBase64(buf,data->szShortName,fsatDxFSAT_SVC_SHORT_NAME_LEN +1);
		HLIB_RWSTREAM_Print(sd, ",\"fsat_shortname\":\"%s\"",buf);
	}
#endif

	HLIB_RWSTREAM_Print(sd, ",\"guidance_type\":%d", data->ucGuidanceType);
	HLIB_RWSTREAM_Print(sd, ",\"guidance_mode\":%d", data->ucGuidanceMode);

#ifdef USE_JSON_URL
    Len = HLIB_STD_URLStrLen(data->szGuidanceStr) + 1;
    buf = fsatDxUTIL_GetTemporaryBuffer(Len, NULL);
    HLIB_STD_URLEncode(buf,data->szGuidanceStr,Len);
    HLIB_RWSTREAM_Print(sd, ",\"guidance_str\":\"%s\"", buf);
#else
	if(HxSTD_StrLen(data->szGuidanceStr))
	{
        buf = fsatDxUTIL_GetTemporaryBuffer(1024, NULL);
        HLIB_MATH_EncodeBase64(buf,data->szGuidanceStr,fsatDxFSAT_SVC_GUIDANCE_LEN +1);
        HLIB_RWSTREAM_Print(sd, ",\"guidance_str\":\"%s\"", buf);
	}
#endif


	// TODO: add more encode code here

	return HLIB_RWSTREAM_CloseWithoutBuf(sd);
}
#endif

static void  local_op_service_Decode (const fsatDxService_t* service, HxJSON_t json)
{
#ifdef	CONFIG_OP_FREESAT
    local_freesat_service_Decode((void *)&service->ex.freesat,json);
#endif

}
static HCHAR *	local_op_service_Encode (const fsatDxService_t *service)
{
#ifdef	CONFIG_OP_FREESAT
      return local_freesat_service_Encode((void *)&service->ex.freesat);
#endif
}

static void *	local_metasvc_DecodeService (void *self, HxJSON_t json)
{
	fsatDxService_t *thiz;
	HxJSON_t		list;
	HxJSON_t		item;

	thiz = (fsatDxService_t *)self;
	return_if(thiz == NULL, NULL);
	return_if(!json || !HxJSON_IsObject(json), NULL);

	fsatDxOBJDEC_DecodeObject(self, json, fsatDxmeta_service_table);

	if (HLIB_JSON_Object_GetInteger(json, "new_service") > 0)
	{
		thiz->updateSvcFlag |= efsatDxSVC_UPDATE_NEWCHANNEL;
	}

	list = HLIB_JSON_Object_GetArray(json, "group_uid");
	if (list)
	{
		HINT32	i = 0;
		HxJSON_ARRAY_EACH(list, item, {
			if (!item || !HxJSON_IsInteger(item))
				continue;
			if (i == fsatDxGROUP_LEN)
				break;
			thiz->grpuids[i++] = HxJSON_Integer_Get(item);
		});
	}
	list = HLIB_JSON_Object_GetArray(json, "bouquet_uid");
	if (list)
	{
		HINT32	i = 0;
		HxJSON_ARRAY_EACH(list, item, {
			if (!item || !HxJSON_IsInteger(item))
				continue;
			if (i == fsatDxBOUQUET_LEN)
				break;
			thiz->bqtuids[i++] = HxJSON_Integer_Get(item);
		});
	}
	return self;
}


static HCHAR *	local_metasvc_EncodeService (const void *self, const HCHAR *extension)
{
	fsatDxService_t	*thiz;
	HINT32			i, sd;
	HCHAR			*json;
	HCHAR			*additional;

	thiz = (fsatDxService_t *)self;
	return_if(self == NULL, NULL);
	sd = HLIB_RWSTREAM_Open(NULL, 0);
	if (sd)
	{
		HLIB_RWSTREAM_Print(sd, "\"group_uid\":[");
		for (i = 0 ; i < fsatDxGROUP_LEN && thiz->grpuids[i] ; i++)
		{
			if (i > 0)
				HLIB_RWSTREAM_PutChar(sd, ',');
			HLIB_RWSTREAM_Print(sd, "%d", thiz->grpuids[i]);
		}
		HLIB_RWSTREAM_Print(sd, "],\"bouquet_uid\":[");
		for (i = 0 ; i < fsatDxBOUQUET_LEN && thiz->bqtuids[i] ; i++)
		{
			if (i > 0)
				HLIB_RWSTREAM_PutChar(sd, ',');
			HLIB_RWSTREAM_Print(sd, "%d", thiz->bqtuids[i]);
		}
		HLIB_RWSTREAM_Print(sd, "]");
		additional = (HCHAR *)HLIB_RWSTREAM_GetBuf(sd);
	}
	else
	{
		additional = (HCHAR *)"\"group_uid\":[],\"bouquet_uid\":[]";
	}
	json = fsatDxOBJDEC_EncodeObjectIncludeObject(
				(void *)self
				, NULL
				, fsatDxmeta_service_table
				, additional
				, extension
				, NULL
			);
	if (sd)
		HLIB_RWSTREAM_Close(sd);
   return json;
}


static HCHAR *	local_metasvc_EncodeTuneParam (const fsatDxTuneParam_t *param, const HCHAR *name)
{
	HCHAR	*data,*buf =NULL;
    HINT32 sd;

	switch (param->eDeliType)
	{
	case efsatDxDELIVERY_SAT:
        sd = HLIB_RWSTREAM_OpenEx(NULL, 512, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
        if (!sd)         return NULL;
        HLIB_RWSTREAM_Print(sd, "\"ant_uid\":%d",		param->sat.antInfo.uid);
        buf =HLIB_RWSTREAM_CloseWithoutBuf(sd);
     	data = fsatDxOBJDEC_EncodeObjectIncludeObject(
					(void *)&param->sat.tuningInfo
					, (HCHAR *)name
					, fsatDxsqc_channel_sattpinfo_table
					, "\"delitype\":\"sat\""
					, buf
					, NULL
				);
        break;
	case efsatDxDELIVERY_CAB:
		data = fsatDxOBJDEC_EncodeObjectIncludeObject(
					(void *)&param->cab
					, (HCHAR *)name
					, fsatDxsqc_channel_cabinfo_table
					, "\"delitype\":\"cab\""
					, NULL
				);
		break;
	case efsatDxDELIVERY_TER:
		data = fsatDxOBJDEC_EncodeObjectIncludeObject(
					(void *)&param->ter
					, (HCHAR *)name
					, fsatDxsqc_channel_terinfo_table
					, "\"delitype\":\"ter\""
					, NULL
				);
		break;
	case efsatDxDELIVERY_ANALOG:
	case efsatDxDELIVERY_NET:
	default:
		data = NULL;
		break;
	}

    if(buf) HLIB_STD_MemFree(buf);

	return data;
}

static HERROR	local_metasvc_DecodeTuneParam (fsatDxTuneParam_t *param, HxJSON_t json, fsatDxDeliveryType_e type)
{
	if (json == NULL)
		return ERR_FAIL;
	switch (type)
	{
	case efsatDxDELIVERY_SAT:
        param->sat.antInfo.uid = HLIB_JSON_Object_GetInteger(json, "ant_uid");
        fsatDxOBJDEC_DecodeObject((void *)&param->sat.tuningInfo, json, fsatDxsqc_channel_sattpinfo_table);
		break;

	case efsatDxDELIVERY_CAB:
		fsatDxOBJDEC_DecodeObject((void *)&param->cab, json, fsatDxsqc_channel_cabinfo_table);
		break;

	case efsatDxDELIVERY_TER:
		fsatDxOBJDEC_DecodeObject((void *)&param->ter, json, fsatDxsqc_channel_terinfo_table);
		break;

	case efsatDxDELIVERY_ANALOG:
	case efsatDxDELIVERY_NET:
	default:
		return ERR_FAIL;
	}
	param->eDeliType = type;
	return ERR_OK;
}

#ifdef	CONFIG_OP_FREESAT

static void  local_freesat_Transponder_Decode (void *obj, HxJSON_t json)
{
    fsatDxFreesatTs_t   *data;
    const HCHAR *text;
    data = (fsatDxFreesatTs_t *)obj;
    if (data == NULL)
    {
        return;
    }
    memset(data, 0, sizeof(fsatDxFreesatTs_t));

    text = HLIB_JSON_Object_GetString(json, "default_auth");
    if (text && HxSTD_StrLen(text))
    {
#ifdef USE_JSON_URL
        HLIB_STD_StrNCpySafe(data->defaultAuth, text, fsatDxDEFAULT_AUTHORITY_LEN);
#else
        HLIB_MATH_DecodeBase64((void*)data->defaultAuth, text, fsatDxDEFAULT_AUTHORITY_LEN +3);
#endif
    }
}

static HCHAR *	local_freesat_Transponder_Encode (const void *obj)
{
    const fsatDxFreesatTs_t *data = (const fsatDxFreesatTs_t *)obj;
    HINT32  sd,Len;
    HCHAR* buf;
    sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
    if (!sd)
    {
        HxLOG_Error("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
        return NULL;
    }

#ifdef USE_JSON_URL
    Len = HLIB_STD_URLStrLen(data->defaultAuth) + 1;
    buf = fsatDxUTIL_GetTemporaryBuffer(Len, NULL);
    HLIB_STD_URLEncode(buf,data->defaultAuth,Len);
    HLIB_RWSTREAM_Print(sd, "\"default_auth\":\"%s\"", buf);
#else
	if (HxSTD_StrLen(data->defaultAuth))
	{
	    buf = fsatDxUTIL_GetTemporaryBuffer(1024, NULL);
        HLIB_MATH_EncodeBase64(buf,data->defaultAuth,fsatDxDEFAULT_AUTHORITY_LEN +3);
		HLIB_RWSTREAM_Print(sd, "\"default_auth\":\"%s\"", buf);
        HxLOG_Error("Encode :: default_auth! = %s\n", buf);
	}
	else
	{
		HLIB_RWSTREAM_Print(sd, "\"default_auth\":\"%s\"", "");
		HxLOG_Error("Encode :: default_auth! = %s\n", "");
	}
#endif

    return HLIB_RWSTREAM_CloseWithoutBuf(sd);
}
#endif


static void  local_op_Transponder_Decode (const fsatDxTransponder_t* transponder, HxJSON_t json)
{
#ifdef	CONFIG_OP_FREESAT
    local_freesat_Transponder_Decode((void *)&transponder->ex.freesat,json);
#endif

}
static HCHAR *	local_op_Transponder_Encode (const fsatDxTransponder_t *transponder)
{
#ifdef	CONFIG_OP_FREESAT
    return local_freesat_Transponder_Encode((void *)&transponder->ex.freesat);
#endif
}


static void *	local_metasvc_DecodeTransponder (void *self, HxJSON_t json)
{
	fsatDxTransponder_t *thiz;

	thiz = (fsatDxTransponder_t *)self;
	return_if(thiz == NULL, NULL);
	return_if(!json || !HxJSON_IsObject(json), NULL);

	fsatDxOBJDEC_DecodeObject(self, json, fsatDxmeta_transponder_table);
	return_if(local_metasvc_DecodeTuneParam(
				  &thiz->tuningParam
				, HLIB_JSON_Object_GetObject(json, "tuningparam")
				, thiz->deliType) != ERR_OK
			, NULL);

	return self;
}

static HCHAR *	local_metasvc_EncodeTransponder (const void *self, const HCHAR *extension)
{
	const fsatDxTransponder_t *thiz = (const fsatDxTransponder_t *)self;
	HCHAR	*tune;
	HCHAR	*json;

	return_if(thiz == NULL, NULL);

	tune = local_metasvc_EncodeTuneParam(&thiz->tuningParam, "tuningparam");
	return_if(tune == NULL, NULL);

	json = fsatDxOBJDEC_EncodeObjectIncludeObject(
				(void *)self
				, NULL
				, fsatDxmeta_transponder_table
				, tune
				, extension
				, NULL
			);
	HLIB_STD_MemFree(tune);
	return json;
}


static void *	local_metasvc_DecodeProvider (void *self, HxJSON_t json)
{
    fsatDxProvider_t *thiz;

    thiz = (fsatDxProvider_t *)self;
    return_if(thiz == NULL, NULL);
    return_if(!json || !HxJSON_IsObject(json), NULL);

    fsatDxOBJDEC_DecodeObject(self, json, fsatDxmeta_provider_table);

    return self;

}

static HCHAR *	local_metasvc_EncodeProvider (const void *self, const HCHAR *extension)
{
	return_if(self == NULL, NULL);
	return fsatDxOBJDEC_EncodeObjectIncludeObject(
				(void *)self
				, NULL
				, fsatDxmeta_provider_table
				, extension
				, NULL
			);
}

#ifdef	CONFIG_OP_FREESAT
static void  local_freesat_Group_Decode (void *obj, HxJSON_t json)
{
	fsatDxFreesatGroup_t	*data;
	const HCHAR	*text;


	data = (fsatDxFreesatGroup_t *)obj;
	if (data == NULL)
	{
		return;
	}
	memset(data, 0, sizeof(fsatDxFreesatGroup_t));

	data->ucGroupType				= HLIB_JSON_Object_GetInteger(json, "group_type");
	data->ucNondestructiveTuneFlag		= HLIB_JSON_Object_GetInteger(json, "ntune_flag");
	data->ucReturnChannelAccessFlag	= HLIB_JSON_Object_GetInteger(json, "chacs_flag");
	data->ucG2ExtensionFlag			= HLIB_JSON_Object_GetInteger(json, "g2ext_flag");
	data->ucDefaultVisableFlag			= HLIB_JSON_Object_GetInteger(json, "defvsb_flag");
	data->ucAdultChannelsGroupFlag		= HLIB_JSON_Object_GetInteger(json, "adult_flag");
	data->ucIpChannelsGroupFlag		= HLIB_JSON_Object_GetInteger(json, "ipgrp_flag");
	data->ucCaChannelsGroupFlag		= HLIB_JSON_Object_GetInteger(json, "cagrp_flag");

	text = HLIB_JSON_Object_GetString(json, "lang_code");
	if (text && HxSTD_StrLen(text))
	{
#ifdef USE_JSON_URL
        HLIB_STD_StrNCpySafe(data->szIso639LangCode, text, fsatDxFSAT_LANG_CODE_LEN +1);
#else
		HLIB_MATH_DecodeBase64(data->szIso639LangCode, text, fsatDxFSAT_LANG_CODE_LEN + 1);
#endif
	}
	// TODO: add more decode code here
}

static HCHAR *	local_freesat_Group_Encode (const void *obj)
{
	const fsatDxFreesatGroup_t *data = (const fsatDxFreesatGroup_t *)obj;
	HINT32	sd,Len;
    HCHAR *buf;
	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if (!sd)
	{
		HxLOG_Error("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	HLIB_RWSTREAM_Print(sd, "\"group_type\":%d",	(HINT32 )data->ucGroupType);
	HLIB_RWSTREAM_Print(sd, ",\"ntune_flag\":%d",		(HINT32 )data->ucNondestructiveTuneFlag);
	HLIB_RWSTREAM_Print(sd, ",\"chacs_flag\":%d",		(HINT32 )data->ucReturnChannelAccessFlag);
	HLIB_RWSTREAM_Print(sd, ",\"g2ext_flag\":%d",		(HINT32 )data->ucG2ExtensionFlag);
	HLIB_RWSTREAM_Print(sd, ",\"defvsb_flag\":%d", 	(HINT32 )data->ucDefaultVisableFlag);
	HLIB_RWSTREAM_Print(sd, ",\"adult_flag\":%d",		(HINT32 )data->ucAdultChannelsGroupFlag);
	HLIB_RWSTREAM_Print(sd, ",\"ipgrp_flag\":%d",		(HINT32 )data->ucIpChannelsGroupFlag);
	HLIB_RWSTREAM_Print(sd, ",\"cagrp_flag\":%d",		(HINT32 )data->ucCaChannelsGroupFlag);

#ifdef USE_JSON_URL
    Len = HLIB_STD_URLStrLen(data->szIso639LangCode) + 1;
    buf = fsatDxUTIL_GetTemporaryBuffer(Len, NULL);
    HLIB_STD_URLEncode(buf,data->szIso639LangCode,Len);
    HLIB_RWSTREAM_Print(sd, ",\"lang_code\":\"%s\"",	buf);
#else
	if (HxSTD_StrLen(data->szIso639LangCode))// && isalnum(data->szIso639LangCode[0])) // 유효하지 않는 language code 가 ...
    {
        buf = fsatDxUTIL_GetTemporaryBuffer(1024, NULL);
        HLIB_MATH_EncodeBase64(buf,data->szIso639LangCode,fsatDxFSAT_LANG_CODE_LEN +1);
        HLIB_RWSTREAM_Print(sd, ",\"lang_code\":\"%s\"",	buf);
    }
    else
        HLIB_RWSTREAM_Print(sd, ",\"lang_code\":\"%s\"",	"");
#endif

	// TODO: add more encode code here

	return HLIB_RWSTREAM_CloseWithoutBuf(sd);
}
#endif

static void  local_op_Group_Decode (const fsatDxGroup_t* group, HxJSON_t json)
{
#ifdef	CONFIG_OP_FREESAT
    local_freesat_Group_Decode((void *)&group->ex.freesat,json);
#endif
}
static HCHAR *	local_op_Group_Encode (const fsatDxGroup_t *group)
{
#ifdef	CONFIG_OP_FREESAT
      return local_freesat_Group_Encode((void *)&group->ex.freesat);
#endif
}





static void *	local_metasvc_DecodeGroup (void *self, HxJSON_t json)
{
    fsatDxGroup_t *thiz;

    thiz = (fsatDxGroup_t *)self;
    return_if(thiz == NULL, NULL);
    return_if(!json || !HxJSON_IsObject(json), NULL);

    fsatDxOBJDEC_DecodeObject(self, json, fsatDxmeta_group_table);
    return self;
}

static HCHAR *	local_metasvc_EncodeGroup (const void *self, const HCHAR *extension)
{
	return_if(self == NULL, NULL);

	return fsatDxOBJDEC_EncodeObjectIncludeObject(
				(void *)self
				, NULL
				, fsatDxmeta_group_table
				, extension
				, NULL
			);
}

static void *	local_metasvc_DecodeBouquet (void *self, HxJSON_t json)
{
	fsatDxBouquet_t *thiz;

	thiz = (fsatDxBouquet_t *)self;
	return_if(thiz == NULL, NULL);
	return_if(!json || !HxJSON_IsObject(json), NULL);

	fsatDxOBJDEC_DecodeObject(self, json, fsatDxmeta_bouquet_table);
	return self;
}

static HCHAR *	local_metasvc_EncodeBouquet (const void *self, const HCHAR *extension)
{
    return_if(self == NULL, NULL);

    return  fsatDxOBJDEC_EncodeObjectIncludeObject(
                (void *)self
                , NULL
                , fsatDxmeta_bouquet_table
                , extension
                , NULL
            );
}

#ifdef	CONFIG_OP_FREESAT

static void  local_freesat_Network_Decode (void *obj, HxJSON_t json)
{
	fsatDxFreesatNet_t	*data;
	const HCHAR	*text;

	data = (fsatDxFreesatNet_t *)obj;
	if (data == NULL)
	{
		return;
	}
	memset(data, 0, sizeof(fsatDxFreesatNet_t));

	text = HLIB_JSON_Object_GetString(json, "default_auth");
	if (text && HxSTD_StrLen(text))
	{
#ifdef USE_JSON_URL
        HLIB_STD_StrNCpySafe(data->defaultAuth, text, fsatDxDEFAULT_AUTHORITY_LEN);
#else
		HLIB_MATH_DecodeBase64(data->defaultAuth, text, fsatDxDEFAULT_AUTHORITY_LEN +3);
#endif
		HxLOG_Error("Decode :: default_auth! = %s\n", data->defaultAuth);
	}
	else
	{
		data->defaultAuth[0] = '\0';
	}

	// TODO: add more decode code here
}

static HCHAR *	local_freesat_Network_Encode (const void *obj)
{
    const fsatDxFreesatNet_t *data = (const fsatDxFreesatNet_t *)obj;
    HINT32  sd,Len;
    HCHAR *buf;
    sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
    if (!sd)
    {
        HxLOG_Error("[%s:%d] Out of memory!\n", __HxFILE__, __HxLINE__);
        return NULL;
    }
#ifdef USE_JSON_URL
    Len = HLIB_STD_URLStrLen(data->defaultAuth) + 1;
    buf = fsatDxUTIL_GetTemporaryBuffer(Len, NULL);
    HLIB_STD_URLEncode(buf,data->defaultAuth,Len);
    HLIB_RWSTREAM_Print(sd, "\"default_auth\":\"%s\"",buf);
#else
    if (HxSTD_StrLen(data->defaultAuth))
    {
        buf = fsatDxUTIL_GetTemporaryBuffer(1024, NULL);
        HLIB_MATH_EncodeBase64(buf,data->defaultAuth,fsatDxDEFAULT_AUTHORITY_LEN +3);
        HLIB_RWSTREAM_Print(sd, "\"default_auth\":\"%s\"",buf);
        HxLOG_Error("Encode :: default_auth! = %s\n", buf);
    }
    else
    {
        HLIB_RWSTREAM_Print(sd, "\"default_auth\":\"%s\"", "");
        HxLOG_Error("Encode :: default_auth! = %s\n", "");
    }
#endif

    // TODO: add more encode code here

    return HLIB_RWSTREAM_CloseWithoutBuf(sd);
}
#endif


static void  local_op_Network_Decode (const fsatDxNetwork_t* network, HxJSON_t json)
{
#ifdef	CONFIG_OP_FREESAT
    local_freesat_Network_Decode((void *)&network->ex.freesat,json);
#endif
}
static HCHAR *	local_op_Network_Encode (const fsatDxNetwork_t *network)
{
#ifdef	CONFIG_OP_FREESAT
      return local_freesat_Network_Encode((void *)&network->ex.freesat);
#endif
}


static void *	local_metasvc_DecodeNetwork (void *self, HxJSON_t json)
{
	fsatDxNetwork_t *thiz;

	thiz = (fsatDxNetwork_t *)self;
	return_if(thiz == NULL, NULL);
	return_if(!json || !HxJSON_IsObject(json), NULL);

	fsatDxOBJDEC_DecodeObject(self, json, fsatDxmeta_network_table);
	return self;

}

static HCHAR *	local_metasvc_EncodeNetwork (const void *self, const HCHAR *extension)
{
	return_if(self == NULL, NULL);

	return fsatDxOBJDEC_EncodeObjectIncludeObject(
				(void *)self
				, NULL
				, fsatDxmeta_network_table
				, extension
				, NULL
			);
}

static HCHAR *			local_dec_enum_to_value_safe(fsatDxTAGLIST_t *table, HUINT32 data)
{
	while (table->tag)
	{
		if (table->value == data)
			return table->tag;

		table++;
	}
	return NULL;
}

static HUINT32			local_dec_value_to_enum(fsatDxTAGLIST_t *table, const HCHAR *value)
{
	while (table->tag)
	{
		if (HLIB_STD_StrCaseCmp(table->tag, value) == 0)
			return table->value;

		table++;
	}

	return table->value;
}

static HCHAR *		local_metaSched_Encode_repeat (HINT32 sd, const DxSCHED_REPEAT_t *data)
{
	HINT32		garbage = 0;
	HINT32		i, c;
	HCHAR		*ret;

	HxLOG_Trace();

	ret = local_dec_enum_to_value_safe(fsatDxmeta_cycle_table.taglist, data->cycle);
	if (ret == NULL)
		return NULL;

	if (!sd)
	{
		garbage = sd = HLIB_RWSTREAM_OpenEx(NULL, 512, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
		if (!sd)
			return NULL;
	}

	HLIB_RWSTREAM_Print(sd, "\"repeat\":{\"every\":\"%s\"", ret);

	if (data->cycle == eDxSCHED_CYCLE_WEEKLY)
	{
		HLIB_RWSTREAM_Print(sd, ",\"on\":[");
		for (i = 0, c = 0 ; i < 7 ; i++)
		{
			if (!(data->date & (1 << i)))
				continue;

			ret = local_dec_enum_to_value_safe(fsatDxmeta_date_table.taglist, (1 << i));
			if (ret == NULL)
				continue;

			if (c > 0)
				HLIB_RWSTREAM_Print(sd, ",");
			HLIB_RWSTREAM_Print(sd, "\"%s\"", ret);
			c++;
		}
		HLIB_RWSTREAM_Print(sd, "]");
	}
	HLIB_RWSTREAM_Print(sd, "}");

	if (garbage)
	{
		return HLIB_RWSTREAM_CloseWithoutBuf(sd);
	}
	return HLIB_RWSTREAM_GetBuf(sd);

}


static HERROR		local_metaSched_Decode_repeat (HxJSON_t json, DxSCHED_REPEAT_t *data)
{
	HxJSON_t	value;
	HxJSON_t	date;
	const HCHAR		*str;
	HINT32		i, n;
	HxLOG_Trace();

	if (json == (HxJSON_t)NULL || data == NULL)
		return ERR_FAIL;

	memset(data, 0, sizeof(DxSCHED_REPEAT_t));

	str = (HCHAR*)HLIB_JSON_Object_GetString(json, "every");
	if (str == NULL)
		return ERR_FAIL;

	data->cycle = local_dec_value_to_enum(fsatDxmeta_cycle_table.taglist, str);
	switch(data->cycle)
	{
	case eDxSCHED_CYCLE_WEEKLY:
		date = HLIB_JSON_Object_GetArray(json, "on");
		if (!date)
			return ERR_FAIL;
		n = HLIB_JSON_GetSize(date);
		for (i = 0 ; i < n ; i++)
		{
			value = HxJSON_Array_Get(date, i);
			if (!value || !HxJSON_IsString(value))
				continue;
			data->date |= local_dec_value_to_enum(fsatDxmeta_date_table.taglist, HxJSON_String_Get(value));
		}
		if (data->date == eDxSCHED_DATE_NONE)
			return ERR_FAIL;
		break;

	default:
		break;
	}
	return ERR_OK;
}

static HERROR		local_metaSched_Decode ( DxSCHEDULE_t *data,HxJSON_t json)
{
    HCHAR* text;
	HxLOG_Trace();

	if (json == (HxJSON_t)NULL || data == NULL)
		return ERR_FAIL;

	memset(data, 0, sizeof(DxSCHEDULE_t));

	fsatDxOBJDEC_DecodeObject(data, json, fsatDxmeta_schedule_table);

	if ((text = (HCHAR*)HLIB_JSON_Object_GetString(json, "name")) != NULL)
		data->name = HLIB_STD_StrDup(text);
	if ((text = (HCHAR*)HLIB_JSON_Object_GetString(json, "description")) != NULL)
		data->description = HLIB_STD_StrDup(text);
	if ((text = (HCHAR*)HLIB_JSON_Object_GetString(json, "longDescription")) != NULL)
		data->longDescription = HLIB_STD_StrDup(text);

	(void) local_metaSched_Decode_repeat(HLIB_JSON_Object_GetObject(json, "repeat"), &data->repeat);

	switch(data->action)
	{
	case eDxSCHED_ACTION_WATCH:
		fsatDxOBJDEC_DecodeObject((void *)&data->param.watch, HLIB_JSON_Object_GetObject(json, "watch"), fsatDxmeta_sched_watch_table);
		break;

	case eDxSCHED_ACTION_RECORD:
		 fsatDxOBJDEC_DecodeObject((void *)&data->param.record, HLIB_JSON_Object_GetObject(json, "record"), fsatDxmeta_sched_rec_table);
		break;

	case eDxSCHED_ACTION_POWER:
		fsatDxOBJDEC_DecodeObject((void *)&data->param.power, HLIB_JSON_Object_GetObject(json, "power"), fsatDxmeta_sched_power_table);
		break;

	default:
		break;
	}
	return ERR_OK;
}


static HCHAR *  local_metaSched_Encode (const void *obj)
{
    HINT32      garbage = 0;
    HINT32      i,sd,sd2;
    HCHAR       *param[5];
    HCHAR       *root;
    const DxSCHEDULE_t *data = (const DxSCHEDULE_t *)obj;
    HxLOG_Trace();
    garbage = sd = HLIB_RWSTREAM_OpenEx(NULL, 512, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
    if (!sd)
       return NULL;
    i = 0;
    memset(param, 0, sizeof(param));

    if (data->repeat.cycle != eDxSCHED_CYCLE_NONE)
        param[i++] = local_metaSched_Encode_repeat(0, &data->repeat);

    switch(data->action)
    {
    case eDxSCHED_ACTION_WATCH:
        param[i++] = (HCHAR*)fsatDxOBJDEC_EncodeObject((void *)&data->param.watch, "watch", fsatDxmeta_sched_watch_table);
        break;

    case eDxSCHED_ACTION_RECORD:
        param[i++] = (HCHAR*)fsatDxOBJDEC_EncodeObject((void *)&data->param.record, "record", fsatDxmeta_sched_rec_table);
        break;

    case eDxSCHED_ACTION_POWER:
        param[i++] = (HCHAR*)fsatDxOBJDEC_EncodeObject((void *)&data->param.power, "power", fsatDxmeta_sched_power_table);
        break;

    default:
        break;
    }

    sd2 = HLIB_RWSTREAM_OpenEx(NULL, 512, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
    if (data->name)
       HLIB_RWSTREAM_Print(sd2, "\"%s\":\"%s\"", "name", data->name);
    if (data->description)
       HLIB_RWSTREAM_Print(sd2, ",\"%s\":\"%s\"", "description", data->description);
    if (data->longDescription)
       HLIB_RWSTREAM_Print(sd2, ",\"%s\":\"%s\"", "longDescription", data->longDescription);

    param[i++] = HLIB_RWSTREAM_CloseWithoutBuf(sd2);

    root = (HCHAR*)fsatDxOBJDEC_EncodeObjectIncludeObject((void *)data, NULL, fsatDxmeta_schedule_table, param[0], param[1], param[2],  param[3],NULL);
    param[i++] = root;
    if (root)
    {
        HLIB_RWSTREAM_Print(sd, root);
    }

    while (i--)
    {
        if (param[i])
            HLIB_STD_MemFree(param[i]);
    }

    if (root == NULL)
    {
        if (garbage)
            HLIB_RWSTREAM_Close(garbage);
        return NULL;
    }

    if (garbage)
    {
        return HLIB_RWSTREAM_CloseWithoutBuf(sd);
    }
    return HLIB_RWSTREAM_GetBuf(sd);
}


static HERROR  local_metaRec_Decode (fsatDxRecListData_t* data, HxJSON_t json)
{
    HxLOG_Trace();
    HxLOG_Assert(json && data);
    memset(data, 0, sizeof(fsatDxRecListData_t));
	fsatDxOBJDEC_DecodeObject((void *)data, json, s_Reclist_RECDataPropertyTable);

	if (data->usNumOfBookmark)
	{
		HxJSON_t	jBookmarkArray = HxJSON_Object_Get (json, RECLIST_BOOKMARK_TAG);
		if (jBookmarkArray)
		{
			HxJSON_t	jBookmark;
			HINT32	i;

			for (i = 0; i < data->usNumOfBookmark; ++i)
			{
				jBookmark = HxJSON_Array_Get (jBookmarkArray, i);
				if (jBookmark)
				{
					fsatDxOBJDEC_DecodeObject ((void *)&(data->astBookmarks[i]), jBookmark, s_Reclist_RECBookmarkPropertyTable);
				}
			}	// end for
		}
	}

	{	// parental rating
		HxJSON_t	jPRArray	= HxJSON_Object_Get (json, RECLIST_PR_TAG);
		if (jPRArray)
		{
			HINT32	i;
			HUINT32	ulPRCnt = HLIB_JSON_GetSize(jPRArray);

			for (i = 0; i < ulPRCnt; ++i)
			{
				HxJSON_t	jPR = HxJSON_Array_Get (jPRArray, i);
				if (jPR)
				{
					fsatDxOBJDEC_DecodeObject ((void *)&(data->stEvt.astParentalRating[i]),
						jPR, s_Reclist_RECParentPropertyTable);
				}
			}	// end for
		}
	}
#if defined(CONFIG_OP_FREESAT)

	{	// guidance info
		HxJSON_t	jGuidance	= HxJSON_Object_Get (json, RECLIST_GUIDANCE_TAG);
		if (jGuidance)
		{
			fsatDxOBJDEC_DecodeObject ((void *)&(data->extension.freesat.stEvt.astGuidance),
						jGuidance, s_astReclist_RECGuidancePropertyTable);
		}
	}
#endif
	{	// event data
		HxJSON_t	jEvent	= HxJSON_Object_Get (json, RECLIST_EVT_TAG);
		if (jEvent)
		{
			fsatDxOBJDEC_DecodeObject ((void *)&(data->stEvt), jEvent, s_astReclist_RECEventPropertyTable);
		}
	}

	{	// ext event data
		HxJSON_t	jExtEvtArray = HxJSON_Object_Get (json, RECLIST_EXT_EVT_TAG);
		if (jExtEvtArray)
		{
			HUINT32		ulExtEvtCnt = HLIB_JSON_GetSize(jExtEvtArray);
			HINT32		i;

			data->pstExtEvts = (fsatDxRecList_ExtEvt_t*)HLIB_STD_MemCalloc(ulExtEvtCnt
				* sizeof(fsatDxRecList_ExtEvt_t));
			if (NULL == data->pstExtEvts)
				return FALSE;

			data->ulExtEvtCnt = ulExtEvtCnt;
			HxLOG_Print("	extevt count (%d)\n", data->ulExtEvtCnt);
			for (i = 0; i < ulExtEvtCnt; ++i)
			{
				HxJSON_t	jItemArray;
				HxJSON_t	jExtEvt = HxJSON_Array_Get (jExtEvtArray, i);
				fsatDxRecList_ExtEvt_t	*pstExtEvt = &data->pstExtEvts[i];

				if (NULL == jExtEvt || NULL == pstExtEvt)		continue;

				fsatDxOBJDEC_DecodeObject ((void *)pstExtEvt, jExtEvt, s_Reclist_RECExtEventPropertyTable);
				HxLOG_Print("	(%d) idx extevt: decoded des num(%d) last num(%d)\n", i, pstExtEvt->ucDesNum, pstExtEvt->ucLastDesNum);

				jItemArray = HxJSON_Object_Get (jExtEvt, "items");
				if (jItemArray)
				{
					HUINT8		k;

					pstExtEvt->ucItemNum = HLIB_JSON_GetSize(jItemArray);
					HxLOG_Print("	(%d) idx extEvt, item num (%d)\n", i, pstExtEvt->ucItemNum);

					for (k = 0; k < pstExtEvt->ucItemNum; ++k)
					{
						HxJSON_t	jItem = HxJSON_Array_Get (jItemArray, (HINT32)k);
						if (NULL == jItem)		continue;

						fsatDxOBJDEC_DecodeObject ((void *)&pstExtEvt->astItem[k],
							jItem, s_Reclist_RECExtEventItemPropertyTable);

						HxLOG_Print("		(%d) idx item, desc (%s), item (%s)\n", k, pstExtEvt->astItem[k].aucDescription, pstExtEvt->astItem[k].aucItem);
					}	// end for k
				}
				else
				{
					pstExtEvt->ucItemNum = 0;
				}
			}	// end for i
		}	// if (jExtEvtArray)
    #ifdef META_RECLIST_EXT_TEST_DATA
		else
		{
			HUINT32		i, ulExtEvtCnt = 2;
			data->ulExtEvtCnt = ulExtEvtCnt;

			data->pstExtEvts = (fsatDxRecList_ExtEvt_t*)HLIB_STD_MemCalloc(ulExtEvtCnt
					* sizeof(fsatDxRecList_ExtEvt_t));

			for (i = 0; i < ulExtEvtCnt; ++i)
			{
				fsatDxRecList_ExtEvt_t	*pstExtEvt = &data->pstExtEvts[i];

				pstExtEvt->ucDesNum 	= i;
				pstExtEvt->ucLastDesNum = ulExtEvtCnt - 1;
				pstExtEvt->ucItemNum	= 2;
				{
					HUINT8		k;

					HxLOG_Print("	(%d) idx extEvt, item num (%d)\n", i, pstExtEvt->ucItemNum);

					for (k = 0; k < pstExtEvt->ucItemNum; ++k)
					{
						HxSTD_StrCpy(pstExtEvt->astItem[k].aucDescription, "ABC");
						HxSTD_StrCpy(pstExtEvt->astItem[k].aucItem, "ITEM");

						HxLOG_Print("		(%d) idx item, desc (%s), item (%s)\n",
							k, pstExtEvt->astItem[k].aucDescription, pstExtEvt->astItem[k].aucItem);
					}	// end for k
				}
			}	// end for i
		}
    #endif
	}

	{
		const HCHAR	*pszExtEvtTxt = HLIB_JSON_Object_GetString (json, RECLIST_EXT_EVT_TXT_TAG);
		if (pszExtEvtTxt)
		{
			HLIB_STD_StrUtf8NCpy(data->aucExtText, pszExtEvtTxt, fsatDxRECLIST_EXT_TEXT_BUFSIZE);
			HxLOG_Print("	extEvtText = (%s)\n", pszExtEvtTxt);
		}
	}

    return ERR_OK;
}
static HCHAR *	local_metaRec_Encode (const fsatDxRecListData_t *recData)
{
    HINT32  garbage = 0,sd;
	HCHAR	*ret= NULL,*buf;
    HBOOL   comma = FALSE;
    if (NULL == recData)
        return NULL;

    garbage = sd = HLIB_RWSTREAM_OpenEx(NULL, 0, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
    if (!sd)
        return NULL;

    // encode bookmarks
    if (recData->usNumOfBookmark)
    {
        ret = (HCHAR*)fsatDxOBJDEC_EncodeObject_array(RECLIST_BOOKMARK_TAG, DxPVR_NUM_BOOKMARK, sizeof(fsatDxRecList_Bookmark_t),
            -1, (void *)recData->astBookmarks, s_Reclist_RECBookmarkPropertyTable);
        if (ret)
        {
            HLIB_RWSTREAM_Print(sd, "%s", ret);
            comma = TRUE;
        }
    }

    // encode pr info
    if (recData->stEvt.ucPRNum)
    {   // encode parental rating values
        ret = (HCHAR*)fsatDxOBJDEC_EncodeObject_array(RECLIST_PR_TAG, recData->stEvt.ucPRNum, sizeof(fsatDxPvr_ParentalRating_t),
            -1, (void *)recData->stEvt.astParentalRating, s_Reclist_RECParentPropertyTable);

        if (ret)
        {
            if( comma == TRUE)
               HLIB_RWSTREAM_Print(sd, ", %s", ret);
            else
               HLIB_RWSTREAM_Print(sd, "%s", ret);
            comma = TRUE;
        }
    }
#if defined(CONFIG_OP_FREESAT)
    // encode guidance info
    if (HxSTD_StrLen(recData->extension.freesat.stEvt.astGuidance.stData)>0)
    {   // encode parental rating values
        ret = (HCHAR*)fsatDxOBJDEC_EncodeObject((void*)&recData->extension.freesat.stEvt.astGuidance, RECLIST_GUIDANCE_TAG, s_astReclist_RECGuidancePropertyTable);
        if (ret)
        {
             if( comma == TRUE)
               HLIB_RWSTREAM_Print(sd, ", %s", ret);
             else
               HLIB_RWSTREAM_Print(sd, "%s", ret);
             comma = TRUE;
        }
    }
#endif
    // encode event info
    ret = (HCHAR*)fsatDxOBJDEC_EncodeObject((void*)&recData->stEvt, RECLIST_EVT_TAG, s_astReclist_RECEventPropertyTable);
    if (ret)
    {
         if( comma == TRUE)
              HLIB_RWSTREAM_Print(sd, ", %s", ret);
         else
              HLIB_RWSTREAM_Print(sd, "%s", ret);
         comma = TRUE;
    }

    // encode ext event info - desc num, last desc num, iso code, item num, desc/item
    if (recData->ulExtEvtCnt)
    {
        HUINT32 i;
        HBOOL   bFirstExtEvt = TRUE;

        if( comma == TRUE)
           HLIB_RWSTREAM_Print(sd, ", \"%s\":[", RECLIST_EXT_EVT_TAG);    // open RECLIST_EXT_EVT_TAG
        else
           HLIB_RWSTREAM_Print(sd, " \"%s\":[", RECLIST_EXT_EVT_TAG);    // open RECLIST_EXT_EVT_TAG



        for (i = 0; i < recData->ulExtEvtCnt; ++i)
        {
            HUINT8  k = 0;
            fsatDxRecList_ExtEvt_t  *pstExtEvt = &recData->pstExtEvts[i];
            if (NULL == pstExtEvt)          continue;

            if (!bFirstExtEvt)
                HLIB_RWSTREAM_Print(sd, ",");

            HLIB_RWSTREAM_Print(sd, "{");  // open RECLIST_EXT_EVT_TAG item
            HLIB_RWSTREAM_Print(sd, "\"%s\":\"%d\"",   "desnum", pstExtEvt->ucDesNum);
            HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%d\"", "lastdesnum", pstExtEvt->ucLastDesNum);
            HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"", "langcode", pstExtEvt->szIso639LangCode);

            if (pstExtEvt->ucItemNum)
            {
                HBOOL   bFirstItem = TRUE;

                HLIB_RWSTREAM_Print(sd, ", \"%s\":", "items");
                HLIB_RWSTREAM_Print(sd, "[");  // open "items"

                for (k = 0; k < pstExtEvt->ucItemNum; ++k)
                {
                    if (!bFirstItem)
                        HLIB_RWSTREAM_Print(sd, ",");

                    HLIB_RWSTREAM_Print(sd, "{");
                    HLIB_RWSTREAM_Print(sd, "\"%s\":\"%s\"", "desc", &pstExtEvt->astItem[k].aucDescription);
                    HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"", "item", &pstExtEvt->astItem[k].aucItem);
                    HLIB_RWSTREAM_Print(sd, "}");

                    bFirstItem = FALSE;
                }   // end for item idx (k)

                HLIB_RWSTREAM_Print(sd, "]");  // close "items"
            }

            HLIB_RWSTREAM_Print(sd, "}");  // close RECLIST_EXT_EVT_TAG item
            bFirstExtEvt = FALSE;
        }   // end for i

        HLIB_RWSTREAM_Print(sd, "]");  // close RECLIST_EXT_EVT_TAG

        // ext event text
        HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"", RECLIST_EXT_EVT_TXT_TAG, recData->aucExtText);
    }

    //HLIB_RWSTREAM_Print(sd, "}");

    if (garbage)
    {
        buf = HLIB_RWSTREAM_CloseWithoutBuf(sd);
        return (HCHAR*)fsatDxOBJDEC_EncodeObjectIncludeObject((void *)recData,NULL,s_Reclist_RECDataPropertyTable,buf,NULL);
    }
    buf = HLIB_RWSTREAM_GetBuf(sd);
    return (HCHAR*)fsatDxOBJDEC_EncodeObjectIncludeObject((void *)recData,NULL,s_Reclist_RECDataPropertyTable,buf,NULL);
}







#define _END__FUNCTION__________________________________




