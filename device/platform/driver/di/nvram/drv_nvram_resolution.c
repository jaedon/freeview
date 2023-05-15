#include  <string.h>

#include "htype.h"

#include "vkernel.h"
#include "di_err.h"

#include "di_nvram.h"
#include "drv_nvram.h"
#include "di_uart.h"

#define STR_720x240P	"720x240P"
#define STR_2880x240P	"2880x240P"
#define STR_640x480P	"640x480P"
#define STR_720x480I	"720x480I"
#define STR_720x480P	"720x480P"
#define STR_1440x480P	"1440x480P"
#define STR_2880x480I	"2880x480I"
#define STR_2880x480P	"2880x480P"
#define STR_720x288P	"720x288P"
#define STR_2880x288P	"2880x288P"
#define STR_720x576I	"720x576I"
#define STR_720x576P	"720x576P"
#define STR_1440x576P	"1440x576P"
#define STR_2880x576I	"2880x576I"
#define STR_2880x576P	"2880x576P"
#define STR_1080x720P	"1080x720P"
#define STR_1280x720P	"1280x720P"
#define STR_1920x1080I	"1920x1080I"
#define STR_1920x1080P	"1920x1080P"
#define STR_3840x2160P	"3840x2160P"

#define STR_24HZ		"24HZ"
#define STR_25HZ		"25HZ"
#define STR_30HZ		"30HZ"
#define STR_50HZ		"50HZ"
#define STR_60HZ		"60HZ"
#define STR_100HZ		"100HZ"
#define STR_120HZ		"120HZ"
#define STR_200HZ		"200HZ"
#define STR_240HZ		"240HZ"

#define STR_DUMMY		""


typedef struct DRV_NVRAM_RESOLUTION_Table
{
	DI_NVRAM_VIDEO_RESOLUTION_E				videoResolution;
	HUINT8									szNvramHdResolution[NVRAM_SAVED_TEXT_SIZE];
} DRV_NVRAM_RESOLUTION_TABLE;

static const DRV_NVRAM_RESOLUTION_TABLE s_nvramResolutionTable[] =
{
	{DI_NVRAM_RESOLUTION_720x240P  , STR_720x240P  },
	{DI_NVRAM_RESOLUTION_2880x240P , STR_2880x240P },
	{DI_NVRAM_RESOLUTION_640x480P  , STR_640x480P  },
	{DI_NVRAM_RESOLUTION_720x480I  , STR_720x480I  },
	{DI_NVRAM_RESOLUTION_720x480P  , STR_720x480P  },
	{DI_NVRAM_RESOLUTION_1440x480P , STR_1440x480P },
	{DI_NVRAM_RESOLUTION_2880x480I , STR_2880x480I },
	{DI_NVRAM_RESOLUTION_2880x480P , STR_2880x480P },
	{DI_NVRAM_RESOLUTION_720x288P  , STR_720x288P  },
	{DI_NVRAM_RESOLUTION_2880x288P , STR_2880x288P },
	{DI_NVRAM_RESOLUTION_720x576I  , STR_720x576I  },
	{DI_NVRAM_RESOLUTION_720x576P  , STR_720x576P  },
	{DI_NVRAM_RESOLUTION_1440x576P , STR_1440x576P },
	{DI_NVRAM_RESOLUTION_2880x576I , STR_2880x576I },
	{DI_NVRAM_RESOLUTION_2880x576P , STR_2880x576P },
	{DI_NVRAM_RESOLUTION_1080x720P , STR_1080x720P },
	{DI_NVRAM_RESOLUTION_1280x720P , STR_1280x720P },
	{DI_NVRAM_RESOLUTION_1920x1080I, STR_1920x1080I},
	{DI_NVRAM_RESOLUTION_1920x1080P, STR_1920x1080P},
	{DI_NVRAM_RESOLUTION_3840x2160P, STR_3840x2160P},
	{-1,								STR_DUMMY}
};
HINT32		s_nvramResolutionTableSize = sizeof(s_nvramResolutionTable)/sizeof(s_nvramResolutionTable[0]);

typedef struct DRV_NVRAM_FIELD_RATE_Table
{
	DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E				videoFieldRate;
	HUINT8											szNvramHdFieldRate[NVRAM_SAVED_TEXT_SIZE];
} DRV_NVRAM_FIELD_RATE_TABLE;

static const DRV_NVRAM_FIELD_RATE_TABLE s_nvramFieldRateTable[] =
{
	{DI_NVRAM_HDMI_FIELD_RATE_24HZ , STR_24HZ },
	{DI_NVRAM_HDMI_FIELD_RATE_25HZ , STR_25HZ },
	{DI_NVRAM_HDMI_FIELD_RATE_30HZ , STR_30HZ },
	{DI_NVRAM_HDMI_FIELD_RATE_50HZ , STR_50HZ },
	{DI_NVRAM_HDMI_FIELD_RATE_60HZ , STR_60HZ },
	{DI_NVRAM_HDMI_FIELD_RATE_100HZ, STR_100HZ},
	{DI_NVRAM_HDMI_FIELD_RATE_120HZ, STR_120HZ},
	{DI_NVRAM_HDMI_FIELD_RATE_200HZ, STR_200HZ},
	{DI_NVRAM_HDMI_FIELD_RATE_240HZ, STR_240HZ},
  	{-1,							STR_DUMMY}
};
HINT32		s_nvramFieldRateTableSize = sizeof(s_nvramFieldRateTable)/sizeof(s_nvramFieldRateTable[0]);

DRV_Error DRV_NVRAM_ConvertVideoResolutionText(DI_NVRAM_VIDEO_RESOLUTION_E videoResolution, HUINT8 *pszNvramResolution)
{
	HINT32	i;
	HUINT32	str_len;

	for (i=0; i<s_nvramResolutionTableSize; i++)
	{
		if (videoResolution == s_nvramResolutionTable[i].videoResolution)
		{
			str_len = VK_strlen((const char *)s_nvramResolutionTable[i].szNvramHdResolution);
			VK_strncpy((char*)pszNvramResolution, (char*)(s_nvramResolutionTable[i].szNvramHdResolution), str_len);
			pszNvramResolution[str_len] = 0;
			return DI_ERR_OK;
		}
	}
	str_len = VK_strlen(STR_1920x1080I);
	VK_strncpy((char*)pszNvramResolution, STR_1920x1080I, str_len);
	pszNvramResolution[str_len] = 0;

	return DI_ERR_NO_RESOURCE;
}

DRV_Error DRV_NVRAM_ConvertVideoResolutionField(const HUINT8 *pszNvramResolution, DI_NVRAM_VIDEO_RESOLUTION_E *pVideoResolution)
{
	HINT32	i;

	for (i=0; i<s_nvramResolutionTableSize; i++)
	{
		if (VK_memcmp(s_nvramResolutionTable[i].szNvramHdResolution, pszNvramResolution, VK_strlen((const char *)s_nvramResolutionTable[i].szNvramHdResolution))==0)
		{
			*pVideoResolution = s_nvramResolutionTable[i].videoResolution;
			return DI_ERR_OK;
		}
	}
	*pVideoResolution = DI_NVRAM_RESOLUTION_1920x1080I;

	return DI_ERR_NO_RESOURCE;
}

DRV_Error DRV_NVRAM_ConvertFieldRateText(DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E fieldRate, HUINT8 *pszNvramResolution)
{
	HINT32	i;
	HUINT32	str_len;

	for (i=0; i<s_nvramFieldRateTableSize; i++)
	{
		if (fieldRate == s_nvramFieldRateTable[i].videoFieldRate)
		{
			str_len = VK_strlen((const char *)s_nvramFieldRateTable[i].szNvramHdFieldRate);
			VK_strncpy((char*)pszNvramResolution, (char*)(s_nvramFieldRateTable[i].szNvramHdFieldRate), str_len);
			pszNvramResolution[str_len] = 0;
			return DI_ERR_OK;
		}
	}
	str_len = VK_strlen((const char *)s_nvramFieldRateTable[i].szNvramHdFieldRate);
	VK_strncpy((char*)pszNvramResolution, STR_1920x1080I, str_len);
	pszNvramResolution[str_len] = 0;

	return DI_ERR_NO_RESOURCE;
}

DRV_Error DRV_NVRAM_ConvertFieldRate(const HUINT8 *pszNvramResolution, DI_NVRAM_HDMI_VIDEO_FIELD_RATE_E *pFieldRate)
{
	HINT32	i;

	for (i=0; i<s_nvramFieldRateTableSize; i++)
	{
		if (VK_memcmp(s_nvramFieldRateTable[i].szNvramHdFieldRate, pszNvramResolution, VK_strlen((const char *)s_nvramFieldRateTable[i].szNvramHdFieldRate))==0)
		{
			*pFieldRate = s_nvramFieldRateTable[i].videoFieldRate;
			return DI_ERR_OK;
		}
	}
	*pFieldRate = DI_NVRAM_HDMI_FIELD_RATE_50HZ;

	return DI_ERR_NO_RESOURCE;
}


