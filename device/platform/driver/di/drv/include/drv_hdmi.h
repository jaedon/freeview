/**
 * drv_i2c.h
*/

/**
 * @defgroup		DRV_HDMI
 * @author
 * @note			
 * @brief			
 * @file 			drv_hdmi.h
*/

#ifndef __DRV_HDMI_H__
#define __DRV_HDMI_H__


/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "nexus_hdmi_output_hdcp.h"
#include "htype.h"
#include "drv_err.h"

#if defined(CONFIG_HDMI_CEC)
#include "nexus_cec.h"
#endif

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define DRV_HDMI_HDCPKEY_VERTOR_SIZE							NEXUS_HDMI_OUTPUT_HDCP_KSV_LENGTH
#define DRV_HDMI_HDCPKEY_DUMMY_SIZE							3
#define DRV_HDMI_HDCPKEY_STRUCT_SIZE							28
#define DRV_HDMI_HDCPKEY_ENCRYPTED_TOTAL_SIZE			1128
#define DRV_HDMI_SAGE_FW_FLAG_FILE						"/var/lib/humaxtv/sage"

/*
 * The following defines are from bhdm_edid.h
 *
 * 2009.8.10 dcchung
 */

#define DRV_HDMI_EDID_BLOCKSIZE	128
#define DRV_HDMI_EDID_EXTENSION 0x7E
#define DRV_HDMI_EDID_CHECKSUM                0x7F  

#define DRV_HDMI_EDID_EXT_TAG            0x00
	#define DRV_HDMI_EDID_EXT_LCD_TIMINGS          0x01
	#define DRV_HDMI_EDID_EXT_TIMING_DATA 	       0x02 
	#define DRV_HDMI_EDID_EXT_EDID_2_0             0x20 
	#define DRV_HDMI_EDID_EXT_COLOR_INFO           0x30 
	#define DRV_HDMI_EDID_EXT_DVI_FEATURE          0x40 
	#define DRV_HDMI_EDID_EXT_TOUCH_SCREEN         0x50 
	#define DRV_HDMI_EDID_EXT_BLOCK_MAP            0xF0 
	#define DRV_HDMI_EDID_EXT_MANUFACTURER         0xFF 

#define DRV_HDMI_EDID_MAX_BLOCK_MAP_EXTENSIONS	0xFF 

#define DRV_HDMI_EDID_EXT_VERSION                  0x01
	#define DRV_HDMI_EDID_TIMING_VERSION_1             0x01
	#define DRV_HDMI_EDID_TIMING_VERSION_2             0x02
	#define DRV_HDMI_EDID_TIMING_VERSION_3             0x03
	#define DRV_HDMI_EDID_TIMING_VERSION_4             0x04
#define DRV_HDMI_EDID_EXT_DATA_OFFSET              0x02
#define DRV_HDMI_EDID_EXT_MONITOR_SUPPORT          0x03
#define DRV_HDMI_EDID_EXT_DATA_BLOCK_COLLECTION    0x04

#define DRV_HDMI_EDID_EXT_CHECKSUM                 0x7F


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum DRV_HDMI_EDID_CeaDataBlockTag
{
	DRV_HDMI_EDID_CeaDataBlockTag_eReserved0,
	DRV_HDMI_EDID_CeaDataBlockTag_eAudioDB,
	DRV_HDMI_EDID_CeaDataBlockTag_eVideoDB,
	DRV_HDMI_EDID_CeaDataBlockTag_eVSDB,
	DRV_HDMI_EDID_CeaDataBlockTag_eSpeakerDB,
	DRV_HDMI_EDID_CeaDataBlockTag_eReserved5,
	DRV_HDMI_EDID_CeaDataBlockTag_eReserved6,
	DRV_HDMI_EDID_CeaDataBlockTag_eExtendedDB
} DRV_HDMI_EDID_CeaDataBlockTag ;

typedef enum
{
	DRV_HDMI_ColorDepth_e8bit=8,
	DRV_HDMI_ColorDepth_e10bit=10,
	DRV_HDMI_ColorDepth_e12bit=12,
	DRV_HDMI_ColorDepth_e16bit=16
} DRV_HDMI_ColorDepth;

typedef enum
{
	/* CEA Short Audio Descriptor for Audio Codes 2 to 8 */
	DRV_HDMI_SamplingRate_Unknown	= 0x00,
	DRV_HDMI_SamplingRate_32000	= 0x01,	// 01
	DRV_HDMI_SamplingRate_44100	= 0x02, 	// 02
	DRV_HDMI_SamplingRate_48000	= 0x04, 	// 04
	DRV_HDMI_SamplingRate_88200	= 0x08, 	// 08
	DRV_HDMI_SamplingRate_96000	= 0x10, 	// 10
	DRV_HDMI_SamplingRate_176400	= 0x20, 	// 20
	DRV_HDMI_SamplingRate_192000	= 0x40, 	// 40
	DRV_HDMI_SamplingRate_Reserved
}DRV_HDMI_SupportSamplingRate_t;	/* AUDIO_SamplingFreq_t와는 다름 */

typedef enum
{	
	DRV_HDMI_AudioSamplingRate_e8k=8000,
	DRV_HDMI_AudioSamplingRate_e11_025k=11025,
	DRV_HDMI_AudioSamplingRate_e12k=12000,
	DRV_HDMI_AudioSamplingRate_e16k=16000,
	DRV_HDMI_AudioSamplingRate_e22_05k=22050,
	DRV_HDMI_AudioSamplingRate_e24k=24000,
	DRV_HDMI_AudioSamplingRate_e32k=32000,
	DRV_HDMI_AudioSamplingRate_e44_1k=44100,
	DRV_HDMI_AudioSamplingRate_e48k=48000,
	DRV_HDMI_AudioSamplingRate_e64k=64000,
	DRV_HDMI_AudioSamplingRate_e88_2k=88200,
	DRV_HDMI_AudioSamplingRate_e96k=96000,
	DRV_HDMI_AudioSamplingRate_e128k=128000,
	DRV_HDMI_AudioSamplingRate_e176_4k=176400,
	DRV_HDMI_AudioSamplingRate_e192k=192000
}DRV_HDMI_AudioSamplingRate;

#if defined(CONFIG_HDMI_CUSTOM_PREEMPHASIS)
typedef struct
{
	NEXUS_VideoFormat       video_format;
	NEXUS_HdmiColorDepth 	color_depth;
	HUINT16 				custum_preemp;
} DRV_HDMI_Preemp_Setting_t;
#endif


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

DRV_Error DRV_HDMI_Init(void);
DRV_Error DRV_HDMI_DumpRawEdid(void);
DRV_Error DRV_HDMI_PrintEdidInfo(void);
DRV_Error DRV_HDMI_GetStatus(char* pStr);
DRV_Error DRV_HDMI_SetActiveFormat(void);
DRV_Error DRV_HDMI_GetHdcpKey(HUINT8** ppHdcpKey);
void DRV_HDMI_GetHdcpStateStr(HUINT32 ulIndex, char* pStr);
void DRV_HDMI_SendMsg(HUINT32 uiState);
DRV_Error DRV_HDMI_PrintInfo(void);
DRV_Error DRV_HDMI_SetAVMute(HBOOL bMute);
DRV_Error DRV_HDMI_SetAvBlank(HBOOL bBlank);
DRV_Error DRV_HDMI_SetTmds(HBOOL bEnable);
DRV_Error DRV_HDMI_SetAudioMute(HBOOL bMute);
DRV_Error DRV_HDMI_AudioEnable(HBOOL audioEnable);
DRV_Error DRV_HDMI_VideoEnable(HBOOL videoEnable);
DRV_Error DRV_HDMI_GetMonitorName(char *pucVendorId);
DRV_Error DRV_HDMI_GetHdmiOutputHandle(NEXUS_HdmiOutputHandle *hHdmi);
DRV_Error DRV_HDMI_EnableHDCP(HBOOL bEnable);
DRV_Error DRV_HDMI_SetDisplayOutput(HBOOL bEnable);
DRV_Error DRV_HDMI_CheckSupportedResolution(NEXUS_VideoFormat eVideoFormat, HBOOL *pbSupported);
DRV_Error DRV_HDMI_SetPreEmphasisConfiguration(void);
#if defined(CONFIG_HDMI_CEC)
DRV_Error DRV_HDMI_CEC_GetLogicalAddress(NEXUS_CecHandle hCec, HUINT32 *pulLogicalAddress);
#endif

#endif /* !__DRV_HDMI_H__ */

