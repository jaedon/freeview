/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_datatype_settings.c
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
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
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
#include <rest_datatype_base.h>

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
STATIC RestEnumStringTable_t	 s_stRestDataType_Operator =
{
	(HCHAR*)DxNAMEOF(DxOperator_e),
		
	{
		{	"none",			eDxOPERATOR_None		},
		{	"ziggo",		eDxOPERATOR_Ziggo		},
		{	"comhem",		eDxOPERATOR_Comhem		},
		{	"kdg",			eDxOPERATOR_KDG			},
		{	"skyd",			eDxOPERATOR_SkyD 		},
		{	"ses",			eDxOPERATOR_Ses 		},
		{	"ficom",		eDxOPERATOR_FiCom		},
		{	"freesat",		eDxOPERATOR_Freesat		},
		{	"canalready",	eDxOPERATOR_CanalReady 	},
		{	"rcs", 			eDxOPERATOR_Rcs			},
		{	"ntvplus",		eDxOPERATOR_NtvPlus 	},
		{	"bbc",			eDxOPERATOR_BBC			},
		{	"upc",			eDxOPERATOR_UPC			},
		{	"digiturk",		eDxOPERATOR_Digiturk	},
		{	"adtv",			eDxOPERATOR_ADTV 		},
		{	"aljazeera",	eDxOPERATOR_Aljazeera 	},
		{	"cne",			eDxOPERATOR_CNE			},
		{	"spaintdt",		eDxOPERATOR_SpainTdt	},
		{	"truevisions",	eDxOPERATOR_Truevisions	},
		{	"boxer", 		eDxOPERATOR_Boxer		},
		{	"tivu",			eDxOPERATOR_TIVU 		},
		{	"jcon",			eDxOPERATOR_JCOM		},
		{	"skapa",		eDxOPERATOR_SKAPA		},
		{	"mostelcom",	eDxOPERATOR_Mostelecom	},
		{	"swisscanal",	eDxOPERATOR_SwissCanal 	},
		{	"russiatvt",	eDxOPERATOR_RussiaTvt 	},
		{	"almajd",		eDxOPERATOR_Almajd		},
		{	"nova",			eDxOPERATOR_NOVA		},
		{	"skycable",		eDxOPERATOR_SkyCable	},
		{	"germancab", 	eDxOPERATOR_GermanCab	},
		{	"primacom",		eDxOPERATOR_Primacom 	},
		{	"vtc",			eDxOPERATOR_VTC			},
		{	"ertelecom", 	eDxOPERATOR_ERtelecom	},
		{	"middleeast",	eDxOPERATOR_MiddleEast 	},

		{	NULL,			0 	}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_MaturityRating =
{
	(HCHAR*)DxNAMEOF(DxRatingAge_e),
		
	{
		{	"view_all",		eDxRATING_VIEW_ALL		},
		{	"4",			eDxRATING_AGE_4			},
		{	"5",			eDxRATING_AGE_5			},
		{	"6",			eDxRATING_AGE_6			},
		{	"7",			eDxRATING_AGE_7 		},
		{	"8",			eDxRATING_AGE_8 		},
		{	"9",			eDxRATING_AGE_9 		},
		{	"10",			eDxRATING_AGE_10 		},
		{	"11",			eDxRATING_AGE_11		},
		{	"12",			eDxRATING_AGE_12		},
		{	"13",			eDxRATING_AGE_13		},
		{	"14",			eDxRATING_AGE_14		},
		{	"15",			eDxRATING_AGE_15		},
		{	"16",			eDxRATING_AGE_16		},
		{	"17",			eDxRATING_AGE_17		},
		{	"18",			eDxRATING_AGE_18		},
		{	"19",			eDxRATING_AGE_19		},
		{	"20",			eDxRATING_AGE_20		},
		{	"all_age", 		eDxRATING_AGE_ALL_AGE	},
		{	"children",		eDxRATING_AGE_CHILDREN	},
		{	"x_rated", 		eDxRATING_AGE_X_RATED	},
		{	"lock_all",		eDxRATING_AGE_LOCK_ALL	},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_ResolSelect =
{
	(HCHAR*)DxNAMEOF(DxResolutionSelect_e),
		
	{
		{	"unknown",		eDxRESOLUTION_SELECT_UNKNOWN	},
		{	"480i",			eDxRESOLUTION_SELECT_480I		},
		{	"576i",			eDxRESOLUTION_SELECT_576I		},
		{	"480p",			eDxRESOLUTION_SELECT_480P		},
		{	"576p",			eDxRESOLUTION_SELECT_576P 		},
		{	"720p",			eDxRESOLUTION_SELECT_720P 		},
		{	"1080i",		eDxRESOLUTION_SELECT_1080I 		},
		{	"1080p",		eDxRESOLUTION_SELECT_1080P 		},
		{	"original",		eDxRESOLUTION_SELECT_ORIGINAL	},
		{	"edid_max",		eDxRESOLUTION_SELECT_EDID_MAX	},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_ResolStatus =
{
	(HCHAR*)DxNAMEOF(DxResolutionStatus_e),
		
	{
		{	"unknown",		eDxRESOLUTION_STATUS_UNKNOWN	},
		{	"480i",			eDxRESOLUTION_STATUS_480I		},
		{	"576i",			eDxRESOLUTION_STATUS_576I		},
		{	"480p",			eDxRESOLUTION_STATUS_480P		},
		{	"576p",			eDxRESOLUTION_STATUS_576P 		},
		{	"720p",			eDxRESOLUTION_STATUS_720P 		},
		{	"1080i",		eDxRESOLUTION_STATUS_1080I 		},
		{	"1080p",		eDxRESOLUTION_STATUS_1080P 		},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_AspectRatio =
{
	(HCHAR*)DxNAMEOF(DxAspectRatio_e),
		
	{
		{	"unknown",		eDxASPECTRATIO_UNKNOWN	},
		{	"4x3",			eDxASPECTRATIO_4X3		},
		{	"16x9",			eDxASPECTRATIO_16X9		},
		{	"14x9",			eDxASPECTRATIO_14X9		},
		{	"1x1",			eDxASPECTRATIO_1X1 		},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_PictureRate =
{
	(HCHAR*)DxNAMEOF(DxPictureRate_e),
		
	{
		{	"unknown",		eDxPICTURERATE_UNKNOWN	},
		{	"24p",			eDxPICTURERATE_24P		},
		{	"25p",			eDxPICTURERATE_25P		},
		{	"30p",			eDxPICTURERATE_30P		},
		{	"50i",			eDxPICTURERATE_50I 		},
		{	"60i",			eDxPICTURERATE_60I		},
		{	"50p", 			eDxPICTURERATE_50P		},
		{	"60p", 			eDxPICTURERATE_60P		},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_DfcSelect =
{
	(HCHAR*)DxNAMEOF(DxPictureRate_e),
		
	{
		{	"unknown",				eDxDFC_SELECT_UNKNOWN			},
		{	"default",				eDxDFC_SELECT_DEFAULT			},
		{	"widesignal",			eDxDFC_SELECT_WIDESIGNAL		},
		{	"squeeze",				eDxDFC_SELECT_SQUEEZE			},
		{	"squeeze_14x9",			eDxDFC_SELECT_SQUEEZE149 		},
		{	"centercutout",			eDxDFC_SELECT_CENTERCUTOUT		},
		{	"centercutout_14x9",	eDxDFC_SELECT_CENTERCUTOUT149	},
		{	"fullscreen", 			eDxDFC_SELECT_FULLSCREEN		},
		{	"letterbox",			eDxDFC_SELECT_LETTERBOX			},
		{	"letterbox_14x9",		eDxDFC_SELECT_LETTERBOX149		},
		{	"pillarbox",			eDxDFC_SELECT_PILLARBOX			},
		{	"pillarbox_14x9",		eDxDFC_SELECT_PILLARBOX149		},
		{	"zoom",					eDxDFC_SELECT_ZOOM				},
		{	"zoom_14x9",			eDxDFC_SELECT_ZOOM149			},
		{	"sidecut",				eDxDFC_SELECT_SIDECUT			},
		{	"sidecut_14x9",			eDxDFC_SELECT_SIDECUT149		},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_DfcStatus =
{
	(HCHAR*)DxNAMEOF(DxDfcStatus_b),
		
	{
		{	"full",				eDxDFC_STATUS_FULL			},
		{	"letterbox",		eDxDFC_STATUS_LETTERBOX		},
		{	"letterbox_14x9",	eDxDFC_STATUS_LETTERBOX149	},
		{	"pillarbox",		eDxDFC_STATUS_PILLARBOX		},
		{	"pillarbox_14x9",	eDxDFC_STATUS_PILLARBOX149	},
		{	"zoom",				eDxDFC_STATUS_ZOOM			},
		{	"zoom_14x9",		eDxDFC_STATUS_ZOOM149		},
		{	"sidecut", 			eDxDFC_STATUS_SIDECUT		},
		{	"sidecut_14x9",		eDxDFC_STATUS_SIDECUT149	},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_StereoSelect =
{
	(HCHAR*)DxNAMEOF(DxStereoSelect_e),
		
	{
		{	"unknown",		eDxSTEREO_SELECT_UNKNOWN	},
		{	"stereo",		eDxSTEREO_SELECT_STEREO		},
		{	"monoleft",		eDxSTEREO_SELECT_MONOLEFT	},
		{	"monoright",	eDxSTEREO_SELECT_MONORIGHT	},
		{	"monomixed",	eDxSTEREO_SELECT_MONOMIXED	},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_DualMonoSelect =
{
	(HCHAR*)DxNAMEOF(DxDualMonoSelect_e),
		
	{
		{	"unknown",		eDxDUALMONO_SELECT_UNKNOWN	},
		{	"both",			eDxDUALMONO_SELECT_BOTH		},
		{	"left",			eDxDUALMONO_SELECT_LEFT		},
		{	"right",		eDxDUALMONO_SELECT_RIGHT	},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_CompositeStandard =
{
	(HCHAR*)DxNAMEOF(DxCompositeStandard_e),
		
	{
		{	"unknown",		eDxCOMPOSITE_STANDARD_UNKNOWN	},
		{	"pal",			eDxCOMPOSITE_STANDARD_PAL		},
		{	"pal-m",		eDxCOMPOSITE_STANDARD_PAL_M		},
		{	"pal-n",		eDxCOMPOSITE_STANDARD_PAL_N		},
		{	"ntsc",			eDxCOMPOSITE_STANDARD_NTSC		},
		{	"secam", 		eDxCOMPOSITE_STANDARD_SECAM 	},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_ScartFormat =
{
	(HCHAR*)DxNAMEOF(DxScartFormat_e),
		
	{
		{	"unknown",		eDxSCART_FORMAT_UNKNOWN		},
		{	"cvbs",			eDxSCART_FORMAT_CVBS		},
		{	"rgb",			eDxSCART_FORMAT_RGB			},
		{	"svideo",		eDxSCART_FORMAT_SVIDEO		},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_ScartPath =
{
	(HCHAR*)DxNAMEOF(DxScartPath_e),
		
	{
		{	"unknown",		eDxSCART_PATH_UNKNOWN		},
		{	"stb2tv",		eDxSCART_PATH_STB2TV		},
		{	"vcr2tv",		eDxSCART_PATH_VCR2TV		},
		{	"bypass",		eDxSCART_PATH_HWBYPASS		},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_DigitalAudioFormat =
{
	(HCHAR*)DxNAMEOF(DxDigitalAudioFormat_e),
		
	{
		{	"unknown",		eDxDIGITALAUDIO_FORMAT_UNKNOWN		},
		{	"pcm",			eDxDIGITALAUDIO_FORMAT_PCM			},
		{	"multichannel",	eDxDIGITALAUDIO_FORMAT_MULTICHANNEL	},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_HdcpState =
{
	(HCHAR*)DxNAMEOF(DxHdcpStatus_e),
		
	{
		{	"off",			eDxHDCPSTATUS_OFF		},
		{	"try",			eDxHDCPSTATUS_TRY		},
		{	"ok",			eDxHDCPSTATUS_OK		},
		{	"fail",			eDxHDCPSTATUS_FAIL		},
		{	"no_device",	eDxHDCPSTATUS_NODEVICE	},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_Hdmi3DSelect =
{
	(HCHAR*)DxNAMEOF(DxHdmi3DSelect_e),
		
	{
		{	"off",			eDxHDMI_3DSELECT_OFF			},
		{	"sidebyside",	eDxHDMI_3DSELECT_SIDEBYSIDE		},
		{	"topandbottom",	eDxHDMI_3DSELECT_TOPANDBOTTOM	},
		{	"framepacking",	eDxHDMI_3DSELECT_FRAMEPACKING	},
		{	"auto",			eDxHDMI_3DSELECT_AUTO			},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_Video3DFormat =
{
	(HCHAR*)DxNAMEOF(DxVideo3DFormat_e),
		
	{
		{	"none",			eDxVIDEO_3DFORMAT_NONE			},
		{	"sidebyside",	eDxVIDEO_3DFORMAT_SIDEBYSIDE	},
		{	"topandbottom",	eDxVIDEO_3DFORMAT_TOPANDBOTTOM	},
		{	"framepacking",	eDxVIDEO_3DFORMAT_FRAMEPACKING	},

		{	NULL, 			0						}
	}
};

STATIC RestEnumStringTable_t	 s_stRestDataType_AntConnect =
{
	(HCHAR*)DxNAMEOF(DxAntennaConnection_e),
		
	{
		{	"one_cable",	eDxANTCONN_ONECABLE			},
		{	"two_same",		eDxANTCONN_TWOSAME			},
		{	"two_diff",		eDxANTCONN_TWODIFF			},
		{	"one_input",	eDxANTCONN_NONSATONECABLE	},
		{	"two_input",	eDxANTCONN_NONSATTWOCABLE	},

		{	NULL,			0 	}
	}
};

#define ___LOCAL_FUNCTIONS___

#define ___MEMBER_FUNCTIONS___
#define _____SYSTEM_ENVIRONMENT_____
HERROR rest_datatype_CleanSettings (RestSetting_t *pstSetting)
{
	if (NULL == pstSetting)
	{
		HxLOG_Error("pstSvc NULL\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstSetting, 0, sizeof(RestSetting_t));

	pstSetting->nCountry = -1;
	pstSetting->nOperator = -1;
	pstSetting->nGmtOffset = -255;
	pstSetting->nMaturityRating = -1;
	pstSetting->nVideoOnOff = -1;
	pstSetting->nResolutionSetting = -1;
	pstSetting->nAppliedResolution = -1;
	pstSetting->nTvAspectRatio = -1;
	pstSetting->nPictureRate = -1;
	pstSetting->nDisplayFormatSetting = -1;
	pstSetting->nAppliedDisplayFormat = -1;
	pstSetting->nBgColorRed = -1;
	pstSetting->nBgColorGreen = -1;
	pstSetting->nBgColorBlue = -1;
	pstSetting->nAudioMute = -1;
	pstSetting->nMaxVolume = -1;
	pstSetting->nVolume = -1;
	pstSetting->nLipSyncDelay = -1000000;
	pstSetting->nAudioDescription = -1;
	pstSetting->nStereoSelect = -1;
	pstSetting->nDualMonoSelect = -1;
	pstSetting->nCompositeSupport = -1;
	pstSetting->nCompositeStandard = -1;
	pstSetting->nTvScartSupportedFormat = -1;
	pstSetting->nVcrScartSupportedFormat = -1;
	pstSetting->nTvScartFormat = -1;
	pstSetting->nVcrScartFormat = -1;
	pstSetting->nScartPath = -1;
	pstSetting->nHdmiDigitalAudioFormat = -1;
	pstSetting->nCecSupport = -1;
	pstSetting->nCecOnOff = -1;
	pstSetting->nHdcpState = -1;
	pstSetting->nHdmi3dSelect = -1;
	pstSetting->nHdmi3dState = -1;
	pstSetting->nEdid3dFormatCap = -1;
	pstSetting->nEdidResolCap = -1;
	pstSetting->nSpdifSupport = -1;
	pstSetting->nSpdifDigitalAudioFormat = -1;
	pstSetting->nAntConnectType = -1;
	
	return ERR_OK;
}

HERROR rest_datatype_CleanAdminSettingFlag (RestSettingAdminFlag_t *pstAdminFlag)
{
	if (NULL == pstAdminFlag)
	{
		HxLOG_Error("pstAdminFlag NULL\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstAdminFlag, 0, sizeof(RestSettingAdminFlag_t));
	return ERR_OK;
}

HINT32 rest_datatype_JsonizeSettings (HINT32 nJsonWriter, RestSetting_t *pstSetting, HCHAR *szObjectName, HCHAR *szSelect)
{
	HINT32		 nWriter;

	if (NULL == pstSetting)
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
		HxJSONWRITER_ObjectBegin(nWriter);
	}
	else
	{
		// Make the object
		HxJSONWRITER_Object(nWriter, szObjectName);
	}

	// Contents:
	{
		(void)rest_datatype_base_JsonizeSettings(nWriter, pstSetting, szSelect);
	}

	if (NULL != szObjectName)
	{
		HxJSONWRITER_ObjectEnd(nWriter);
	}

	return nWriter;	
}

HERROR rest_datatype_DejsonizeSettings (HxJSON_t *pstJson, RestSetting_t *pstSetting, HBOOL bClearObj)
{
	if (NULL == pstSetting)
	{
		HxLOG_Error("pstSetting NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstJson)
	{
		HxLOG_Error("pstJson NULL\n");
		return ERR_FAIL;
	}

	if (TRUE == bClearObj)
	{
		rest_datatype_CleanSettings(pstSetting);
	}

	// Contents:
	{
		(void)rest_datatype_base_DejsonizeSettings(pstJson, pstSetting);
	}

	return ERR_OK;	
}

HINT32 rest_datatype_JsonizeAdminSettingItems (HINT32 nJsonWriter, RestSettingAdminFlag_t *pstAdminFlag, HCHAR *szObjectName, HCHAR *szSelect)
{
	HINT32		 nWriter;

	if (NULL == pstAdminFlag)
	{
		HxLOG_Error("pstAdminFlag NULL\n");
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
		(void)rest_datatype_base_JsonizeAdminSettingItems(nWriter, pstAdminFlag, szSelect);
	}

	if (NULL != szObjectName)
	{
		HxJSONWRITER_ArrayEnd(nWriter);
	}

	return nWriter;	
}


#define _____INITIALIZATION_____
HERROR rest_datatype_InitSettings (void)
{
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_Operator);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_MaturityRating);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_ResolSelect);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_ResolStatus);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_AspectRatio);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_PictureRate);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_DfcSelect);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_DfcStatus);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_StereoSelect);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_DualMonoSelect);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_CompositeStandard);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_ScartFormat);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_ScartPath);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_DigitalAudioFormat);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_HdcpState);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_Hdmi3DSelect);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_Video3DFormat);
	(void)rest_datatype_RegisterEnumTable(&s_stRestDataType_AntConnect);

	return ERR_OK;
}


#define ___GLOBAL_FUNCTIONS___

