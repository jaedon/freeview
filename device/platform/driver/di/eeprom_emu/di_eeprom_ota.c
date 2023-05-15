#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "htype.h"
#include "vkernel.h"
#include "drv_err.h"
#include "di_err.h"
#include "di_eeprom.h"
#include "di_eeprom_ota.h"
#include "di_nvram.h"
#include "drv_nvram.h"
#include "linden_trace.h"

typedef struct eeprom_map_table
{
	DI_EEPROM_FIELD_t	eepromField;
	DI_NVRAM_FIELD_T	nvramField;
} DRV_EEPROM_TABLE;

static const DRV_EEPROM_TABLE s_DrvEepromTable[] =
{
	{DI_EEPROM_FIELD_SYSTEM_ID,				DI_NVRAM_FIELD_SYSTEM_ID		   },
	{DI_EEPROM_FIELD_LANGUAGE,				DI_NVRAM_FIELD_LANGUAGE		       },
	{DI_EEPROM_FIELD_PANEL_STATUS,			DI_NVRAM_FIELD_DUMMY}, //DI_NVRAM_FIELD_PANEL_STATUS	       },
	{DI_EEPROM_FIELD_TV_STANDARD,			DI_NVRAM_FIELD_TV_STANDARD	       },
	{DI_EEPROM_FIELD_SCART_RGB,				DI_NVRAM_FIELD_SCART_RGB		   },
	{DI_EEPROM_FIELD_HD_RESOULTION,			DI_NVRAM_FIELD_HD_RESOULTION	   },
	{DI_EEPROM_FIELD_SW_UPGRADE,			DI_NVRAM_FIELD_OTA_FLAG1           },
	{DI_EEPROM_FIELD_OTA_TYPE,				DI_NVRAM_FIELD_OTA_TYPE1	       },
	{DI_EEPROM_FIELD_CH_TYPE,				DI_NVRAM_FIELD_CH_TYPE1		       },
	{DI_EEPROM_FIELD_TUNER_ID,				DI_NVRAM_FIELD_TUNER_ID1	       },
	{DI_EEPROM_FIELD_ANTENA_POWER,			DI_NVRAM_FIELD_ANTENA_POWER1       },
	{DI_EEPROM_FIELD_ES_PID,				DI_NVRAM_FIELD_ES_PID1		       },
	{DI_EEPROM_FIELD_TRANSACTION_ID,		DI_NVRAM_FIELD_TRANSACTION_ID1     },
	{DI_EEPROM_FIELD_APP_VERSION,			DI_NVRAM_FIELD_APP_VERSION	       },
	{DI_EEPROM_FIELD_OTALDR_VERSION,		DI_NVRAM_FIELD_OTALDR_VERSION      },
	{DI_EEPROM_FIELD_CUR_OTAIMAGE_VERSION,	DI_NVRAM_FIELD_APP_VERSION_NEXT    },
	{DI_EEPROM_FIELD_OAD_VERSION,			DI_NVRAM_FIELD_OAD_VERSION	       },
	{DI_EEPROM_FIELD_OTA_FILE,				DI_NVRAM_FIELD_DUMMY}, //DI_NVRAM_FIELD_OTA_FILE		       },
	{DI_EEPROM_FIELD_NET_IP_CONFIG, 		DI_NVRAM_FIELD_NET_IP_CONFIG 	   },
	{DI_EEPROM_FIELD_NET_IP_SETTING,		DI_NVRAM_FIELD_NET_IP_SETTING      },
	{DI_EEPROM_FIELD_RCU_CUSTOM_CODE,		DI_NVRAM_FIELD_RCU_CUSTOM_CODE     },
	{DI_EEPROM_FIELD_RCU_CUSTOM_CODE2,		DI_NVRAM_FIELD_RCU_CUSTOM_CODE2    },
	{DI_EEPROM_FIELD_PANEL_DISPLAY,			DI_NVRAM_FIELD_PANEL_DISPLAY	   },
	{DI_EEPROM_FIELD_PANEL_DIMMING, 		DI_NVRAM_FIELD_PANEL_DIMMING 	   },
	{DI_EEPROM_FIELD_ASPECT_RATIO, 			DI_NVRAM_FIELD_ASPECT_RATIO        },
	{DI_EEPROM_FIELD_COUNTRY_CODE,			DI_NVRAM_FIELD_COUNTRY_CODE	       },
	{DI_EEPROM_FIELD_USAGE_ID,				DI_NVRAM_FIELD_USAGE_ID		       },
	{DI_EEPROM_FIELD_OTA_MODE,				DI_NVRAM_FIELD_DUMMY}, //DI_NVRAM_FIELD_OTA_MODE		       },
	{DI_EEPROM_FIELD_CH_INFO_S,				DI_NVRAM_FIELD_CH_INFO_S		   },
	{DI_EEPROM_FIELD_CH_INFO_T,				DI_NVRAM_FIELD_CH_INFO_T		   },
	{DI_EEPROM_FIELD_CH_INFO_C,				DI_NVRAM_FIELD_CH_INFO_C		   },
	{DI_EEPROM_FIELD_NET_PORTAL_IP,			DI_NVRAM_FIELD_NET_PORTAL_IP	   },
	{DI_EEPROM_FIELD_NET_DOWN_PATH, 		DI_NVRAM_FIELD_NET_DOWN_PATH 	   },
	{DI_EEPROM_FIELD_LOADER_VERSION,		DI_NVRAM_FIELD_LOADER_VERSION      },
	{DI_EEPROM_FIELD_OSD_DISPLAY,			DI_NVRAM_FIELD_OSD_DISPLAY	       },
	{DI_EEPROM_FIELD_FTP_OTA_INFO,			DI_NVRAM_FIELD_OTA_FTP_INFO        },
	{DI_EEPROM_FIELD_DISPLAY_FACTORY_INFO,	DI_NVRAM_FIELD_DISPLAY_FACTORY_INFO},
	{DI_EEPROM_FIELD_DUMMY, 				DI_NVRAM_FIELD_DUMMY               }
};

HINT32		s_DrvEepromTableSize = sizeof(s_DrvEepromTable)/sizeof(s_DrvEepromTable[0]);

typedef struct DRV_EEPROM_RESOLUTION_table
{
	DI_EEPROM_HD_RESOLUTION				eepromHdResolution;
	DI_NVRAM_VIDEO_RESOLUTION_E			nvramHdResolution;
	DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E	nvramHdFieldRate;
} DRV_EEPROM_RESOLUTION_TABLE;

static const DRV_EEPROM_RESOLUTION_TABLE s_DrvEepromResolutionTable[] =
{
	{DI_EEPROM_HD_RESOLUTION_480I,			DI_NVRAM_RESOLUTION_720x480I,	DI_NVRAM_HDMI_FIELD_RATE_60HZ},
	{DI_EEPROM_HD_RESOLUTION_480P,			DI_NVRAM_RESOLUTION_720x480P,	DI_NVRAM_HDMI_FIELD_RATE_60HZ},
	{DI_EEPROM_HD_RESOLUTION_576I,			DI_NVRAM_RESOLUTION_720x576I,	DI_NVRAM_HDMI_FIELD_RATE_50HZ},
	{DI_EEPROM_HD_RESOLUTION_576P,			DI_NVRAM_RESOLUTION_720x576P,	DI_NVRAM_HDMI_FIELD_RATE_50HZ},
	{DI_EEPROM_HD_RESOLUTION_720P,			DI_NVRAM_RESOLUTION_1280x720P,	DI_NVRAM_HDMI_FIELD_RATE_60HZ},
	{DI_EEPROM_HD_RESOLUTION_1080I,			DI_NVRAM_RESOLUTION_1920x1080I,	DI_NVRAM_HDMI_FIELD_RATE_60HZ},
	{DI_EEPROM_HD_RESOLUTION_720P_50HZ,		DI_NVRAM_RESOLUTION_1280x720P,	DI_NVRAM_HDMI_FIELD_RATE_50HZ},
	{DI_EEPROM_HD_RESOLUTION_1080I_50HZ,	DI_NVRAM_RESOLUTION_1920x1080I,	DI_NVRAM_HDMI_FIELD_RATE_50HZ},
	{DI_EEPROM_HD_RESOLUTION_1080P_50HZ,	DI_NVRAM_RESOLUTION_1920x1080P,	DI_NVRAM_HDMI_FIELD_RATE_50HZ},
	{DI_EEPROM_HD_RESOLUTION_480I_J,		DI_NVRAM_RESOLUTION_720x480I,	DI_NVRAM_HDMI_FIELD_RATE_60HZ},
	{DI_EEPROM_HD_RESOLUTION_1080P_24HZ,	DI_NVRAM_RESOLUTION_1920x1080P,	DI_NVRAM_HDMI_FIELD_RATE_24HZ},
	{DI_EEPROM_HD_RESOLUTION_1080P_30HZ,	DI_NVRAM_RESOLUTION_1920x1080P,	DI_NVRAM_HDMI_FIELD_RATE_30HZ},
	{DI_EEPROM_HD_RESOLUTION_1080P_60HZ,	DI_NVRAM_RESOLUTION_1920x1080P,	DI_NVRAM_HDMI_FIELD_RATE_60HZ},
	{-1,									DI_NVRAM_RESOLUTION_DUMMY,		DI_NVRAM_HDMI_FIELD_RATE_DUMMY}
};

HINT32		s_DrvEepromResolutionTableSize = sizeof(s_DrvEepromResolutionTable)/sizeof(s_DrvEepromResolutionTable[0]);

static DI_ERR_CODE P_EEPROM_ConvertNvramTable(DI_EEPROM_FIELD_t eepromField, DI_NVRAM_FIELD_T *pNvramField)
{
	HINT32	i;

	for (i=0; i<s_DrvEepromTableSize; i++)
	{
		if (eepromField == s_DrvEepromTable[i].eepromField)
		{
			if (DI_NVRAM_FIELD_DUMMY == s_DrvEepromTable[i].nvramField)
			{
				return DI_ERR_NO_RESOURCE;
			}
			*pNvramField = s_DrvEepromTable[i].nvramField;
			return DI_ERR_OK;
		}
	}
	return DI_ERR_NO_RESOURCE;
}

static DI_ERR_CODE P_EEPROM_ConvertNvramVideoResolutionTable(DI_EEPROM_HD_RESOLUTION eepromField, DI_NVRAM_VIDEO_RESOLUTION_E *pNvramField, DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E *pFieldRate)
{
	HINT32	i;

	*pNvramField = DI_NVRAM_RESOLUTION_1920x1080I;
	*pFieldRate = DI_NVRAM_HDMI_FIELD_RATE_50HZ;
	for (i=0; i<s_DrvEepromResolutionTableSize; i++)
	{
		if (eepromField == s_DrvEepromResolutionTable[i].eepromHdResolution)
		{
			if (DI_NVRAM_RESOLUTION_DUMMY == s_DrvEepromResolutionTable[i].nvramHdResolution)
			{
				return DI_ERR_NO_RESOURCE;
			}
			*pNvramField = s_DrvEepromResolutionTable[i].nvramHdResolution;
			*pFieldRate = s_DrvEepromResolutionTable[i].nvramHdFieldRate;
			return DI_ERR_OK;
		}
	}
	return DI_ERR_NO_RESOURCE;
}

#if 0
static DI_ERR_CODE P_EEPROM_ConvertEepromVideoResolutionTable(DI_NVRAM_VIDEO_RESOLUTION_E nvramField, DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E fieldRate, DI_EEPROM_HD_RESOLUTION *pEepromField)
{
	HINT32	i;

	*pEepromField = DI_EEPROM_HD_RESOLUTION_1080I_50HZ;
	for (i=0; i<s_DrvEepromResolutionTableSize; i++)
	{
		if (nvramField == s_DrvEepromResolutionTable[i].nvramHdResolution)
		{
			if (DI_NVRAM_RESOLUTION_DUMMY == s_DrvEepromResolutionTable[i].nvramHdResolution)
			{
				return DI_ERR_NO_RESOURCE;
			}
			if (fieldRate == s_DrvEepromResolutionTable[i].nvramHdFieldRate)
			{
				*pEepromField = s_DrvEepromResolutionTable[i].eepromHdResolution;
				return DI_ERR_OK;
			}
		}
	}
	return DI_ERR_NO_RESOURCE;
}
#endif

DI_ERR_CODE DRV_EEPROM_GetField(DI_EEPROM_FIELD_t type, void *data, HINT32 nDataSize)
{
	DI_ERR_CODE errCode = DI_ERR_ERROR;
	DI_NVRAM_FIELD_T	nvramField;

	if( data == NULL)
	{
		return DI_ERR_INVALID_PARAM;
	}

	errCode = P_EEPROM_ConvertNvramTable(type, &nvramField);
	if (errCode != DI_ERR_OK)
	{
		TraceError(TRACE_MODULE_DI_EEPROM, "[DRV_EEPROM_GetField] type(%d) error(%08X) : P_EEPROM_ConvertNvramTable\n", type, errCode);
		return errCode;
	}

	errCode = DRV_NVRAM_GetField(nvramField, 0, data, nDataSize);
	if (errCode != DI_ERR_OK)
	{
		TraceError(TRACE_MODULE_DI_EEPROM, "[DRV_EEPROM_GetField] type(%d) error(%08X) : DRV_NVRAM_GetField\n", type, errCode);
		return errCode;
	}

	return errCode;
}


/*********************************************************************
 * Function 	DI_EEPROM_ReadPanelInfo
 * Description
 * Inputs
 * Outputs
 * Returns
 *********************************************************************/
DI_ERR_CODE DI_EEPROM_SetField(DI_EEPROM_FIELD_t type, void *data, HINT32 nDataSize)
{
	DI_ERR_CODE errCode = DI_ERR_ERROR;
	DI_NVRAM_FIELD_T	nvramField;

	if( data == NULL)
	{
		return DI_ERR_INVALID_PARAM;
	}

	if (type == DI_EEPROM_FIELD_HD_RESOULTION)
	{
		DI_EEPROM_HD_RESOLUTION				*pEepromRes;
		DI_NVRAM_VIDEO_RESOLUTION_E 		pNvramField;
		DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E 	pFieldRate;

		pEepromRes = (DI_EEPROM_HD_RESOLUTION*)data;
		errCode = P_EEPROM_ConvertNvramVideoResolutionTable(*pEepromRes, &pNvramField, &pFieldRate);
		if (errCode != DI_ERR_OK)
		{
			TraceError(TRACE_MODULE_DI_EEPROM, "[DI_EEPROM_SetField] type(%d) error(%08X) : P_EEPROM_ConvertNvramVideoResolutionTable\n", type, errCode);
		}
		errCode = DRV_NVRAM_SetField(DI_NVRAM_FIELD_HD_RESOULTION, 0, &pNvramField, sizeof(DI_NVRAM_VIDEO_RESOLUTION_E));
		if (errCode != DI_ERR_OK)
		{
			TraceError(TRACE_MODULE_DI_EEPROM, "[DI_EEPROM_SetField] type(%d) error(%08X) : DRV_NVRAM_SetField\n", type, errCode);
			return errCode;
		}
		errCode = DRV_NVRAM_SetField(DI_NVRAM_FIELD_FIELD_RATE, 0, &pFieldRate, sizeof(DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E));
		if (errCode != DI_ERR_OK)
		{
			TraceError(TRACE_MODULE_DI_EEPROM, "[DI_EEPROM_SetField] type(%d) error(%08X) : DRV_NVRAM_SetField\n", type, errCode);
			return errCode;
		}
	}
	else
	{
		errCode = P_EEPROM_ConvertNvramTable(type, &nvramField);
		if (errCode != DI_ERR_OK)
		{
			TraceError(TRACE_MODULE_DI_EEPROM, "[DI_EEPROM_SetField] type(%d) error(%08X) : P_EEPROM_ConvertNvramTable\n", type, errCode);
			return errCode;
		}

		errCode = DRV_NVRAM_SetField(nvramField, 0, data, nDataSize);
		if (errCode != DI_ERR_OK)
		{
			TraceError(TRACE_MODULE_DI_EEPROM, "[DI_EEPROM_SetField] type(%d) error(%08X) : DRV_NVRAM_SetField\n", type, errCode);
			return errCode;
		}
	}
	return errCode;
}


/*********************************************************************
 * Function 	DI_EEPROM_ReadPanelInfo
 * Description
 * Inputs
 * Outputs
 * Returns
 *********************************************************************/
DI_ERR_CODE DI_EEPROM_GetField(DI_EEPROM_FIELD_t type, void *data, HINT32 nDataSize)
{
	return DRV_EEPROM_GetField(type, data, nDataSize);
}

/*********************************************************************
 * Function 	DI_EEPROM_ReadPanelInfo
 * Description
 * Inputs
 * Outputs
 * Returns
 *********************************************************************/
unsigned long DI_EEPROM_GetFieldSize(DI_EEPROM_FIELD_t type)
{
	DI_ERR_CODE errCode = DI_ERR_ERROR;
	DI_NVRAM_FIELD_T	nvramField;
	HUINT32				nLen;

	errCode = P_EEPROM_ConvertNvramTable(type, &nvramField);
	if (errCode != DI_ERR_ERROR)
	{
		TraceError(TRACE_MODULE_DI_EEPROM, "[DI_EEPROM_GetFieldSize] type(%d) error(%08X) : DRV_EEPROM_GetFieldInfo\n", type, errCode);
		return 0;
	}

	errCode = DRV_NVRAM_GetFieldLength(nvramField, &nLen);
	if (errCode != DI_ERR_ERROR)
	{
		TraceError(TRACE_MODULE_DI_EEPROM, "[DI_EEPROM_GetFieldSize] type(%d) error(%08X) : DRV_NVRAM_GetLength\n", type, errCode);
		return 0;
	}

	return nLen;
}

/*********************************************************************
 * Function 	DRV_EEPROM_ResolutionNvramToEeprom
 * Description
 * Inputs
 * Outputs
 * Returns
 *********************************************************************/
DI_EEPROM_HD_RESOLUTION DRV_EEPROM_ResolutionNvramToEeprom(HUINT8 *nvramHdResolution, HUINT8 *nvramHdFieldRate)
{
	// di_video.c 에서 호출 하고 있어서 임시로 넣어 놨음. di_video.c 에서 DRV_NVRAM으로 호출 후 삭제 필요 함.
	HAPPY(nvramHdResolution);
	HAPPY(nvramHdFieldRate);
	return DI_EEPROM_HD_RESOLUTION_1080I_50HZ;
}



