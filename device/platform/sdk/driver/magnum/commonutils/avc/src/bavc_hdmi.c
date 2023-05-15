/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bavc_hdmi.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 1/12/12 3:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: Q:/projects/7425/latest/magnum/commonutils/avc/7425/bavc_hdmi.c $
 * 
 * Hydra_Software_Devel/13   1/12/12 3:29p rgreen
 * SW7429-43: add HBR audio packet support
 * 
 * Hydra_Software_Devel/12   2/11/11 2:43p rgreen
 * SW35230-3014: Add support for YCC Qunatization and IT Content Type
 * fields in AVI InfoFrame
 * 
 * Hydra_Software_Devel/11   12/2/10 2:43p rgreen
 * SW35230-2374: add support to verify HDCP keys stored in OTP
 * 
 * Hydra_Software_Devel/10   4/27/10 10:20p rgreen
 * SW7405-3997: Update debug info for VSI fields
 * 
 * Hydra_Software_Devel/9   4/16/10 2:20p rgreen
 * SW7405-3997: Rename "Reserved for future use" text to "Top-and-Bottom"
 * 
 * Hydra_Software_Devel/8   4/5/10 11:57p rgreen
 * SW7405-3994,SW7405-3997: Fix compilation for HDMI Rx
 * 
 * Hydra_Software_Devel/SW7401-4363/1   4/5/10 6:25p rgreen
 * SW7405-3994,SW7405-3997: Fix compilation for HDMI Rx vs Tx
 * 
 * Hydra_Software_Devel/6   3/15/10 5:13p rgreen
 * SW3548-2820: Add support for  VS Infoframe 3D Structure and Ext Data
 * fields
 * 
 * Hydra_Software_Devel/5   1/13/10 4:51p rgreen
 * SW3548-2708,SW7401-4355:
 * Add enumerations for Vendor Specific InfoFrame
 * Add enumeration to string functions for Vendor Specific InfoFrame
 * 
 * Hydra_Software_Devel/4   10/13/09 1:07p rgreen
 * SW3548-2013: Add support for SPD InfoFrame debugging
 * 
 * Hydra_Software_Devel/3   10/5/09 10:30a rgreen
 * SW3548-2013: change packet type debug messages from numbers to text
 * 
 * Hydra_Software_Devel/2   5/15/09 11:48a rgreen
 * PR54000: Add conversion debug functions for Video Id Code (VIC) to
 * string
 * 
 * Hydra_Software_Devel/1   5/12/09 6:19p rgreen
 * PR54000: Add debug enum to string conversion functions
 * 
 ***************************************************************************/


#include "bavc.h"
#include "bavc_hdmi.h"


BDBG_MODULE(BAVC_HDMI) ;


static const char EnumError[] = "Enum Error" ;
static const char Reserved[] = "Reserved" ;
static const char BAVC_HDMI_P_PacketTypeUnknown[] = "Unknown" ;

typedef struct
{
	BAVC_HDMI_PacketType ePacketType ;
	const char *PacketName ;
} BAVC_HDMI_PacketTypes ;

static BAVC_HDMI_PacketTypes PacketTypes_Text[] =
{
	{BAVC_HDMI_PacketType_eAudioClockRegeneration, "Audio Clock Regeneration"},
	{BAVC_HDMI_PacketType_eAudioSample, "Audio Sample"},
	{BAVC_HDMI_PacketType_eGeneralControl, "General Control"},
	{BAVC_HDMI_PacketType_eAudioContentProtection, "ACP"},
	{BAVC_HDMI_PacketType_eISRC1, "ISRC1"},
	{BAVC_HDMI_PacketType_eISRC2, "ISRC2"},
	{BAVC_HDMI_PacketType_eDirectStream, "One Bit Audio Stream"},
	{BAVC_HDMI_PacketType_eHbrAudioPacket, "High Bitrate (HBR) Audio"},

#if BAVC_HDMI_1_3_SUPPORT 
	{BAVC_HDMI_PacketType_eGamutMetadataPacket, "Gamut Metadata"},
#endif 
	
	{BAVC_HDMI_PacketType_eVendorSpecificInfoframe, "Vendor Specific InfoFrame"},
	{BAVC_HDMI_PacketType_eAviInfoFrame, "AVI InfoFrame"},
	{BAVC_HDMI_PacketType_eSpdInfoFrame, "SPD InfoFrame"},
	{BAVC_HDMI_PacketType_eAudioInfoFrame, "Audio InfoFrame" },
	{BAVC_HDMI_PacketType_eMpegInfoFrame, "MPEG Source InfoFrame"}
} ;



static const char *BAVC_HDMI_AviInfoFrame_Text_Y1Y0[] =
{
	"RGB (Default)",
	"YCbCr 4:2:2",
	"YCbCr 4:4:4",
	"Future",
} ;

static const char *BAVC_HDMI_AviInfoFrame_Text_A0[] =
{
	"No Data",
	"Valid Data"
} ;

static const char *BAVC_HDMI_AviInfoFrame_Text_B1B0[] =
{
	"Invalid Bar Data",
	"Valid Vert Bar Info",
	"Vaild Horz Bar Info",
	"Valid Bar Info"
} ;


static const char *BAVC_HDMI_AviInfoFrame_Text_S1S0[] =
{
	"No Data",
	"Overscan",
	"Underscan",
	"Future"
} ;


static const char *BAVC_HDMI_AviInfoFrame_Text_C1C0[] =
{
	"No Data",
	"ITU601",
	"ITU709",
	"Extended Colorimetry Info Valid"
} ;


static const char *BAVC_HDMI_AviInfoFrame_Text_M1M0[] =
{
	"No Data",
	"4:3",
	"16:9",
	"Future"
} ;


static const char *BAVC_HDMI_AviInfoFrame_Text_R3_R0[] =
{
	"Same as Picture AR",
	"4:3 Center",
	"16:9 Center",
	"14:9 Center",
	"Per DVB AFD active_format in ETSI"
} ;


static const char *BAVC_HDMI_AviInfoFrame_Text_ITC[] =
{
	"No Data",
	"IT content",
} ;


static const char *BAVC_HDMI_AviInfoFrame_Text_EC2_EC0[] =
{
	"xvYCC 601",
	"xvYCC 709",
	"Reserved"
} ;


static const char *BAVC_HDMI_AviInfoFrame_Text_Q1Q0[] =
{
	"Based on format",
	"Limited Range",
	"Full Range",
	"Reserved"
} ;


static const char *BAVC_HDMI_AviInfoFrame_Text_SC1SC0[] =
{
	"No Known non-uniform scaling",
	"Picture Horizontally Scaled",
	"Picture Vertically Scaled",
	"Picute Scaled Vertically and Horizontally"
} ;

static const char *BAVC_HDMI_AviInfoFrame_Text_VIC[] =
{
	"Unknown/IT Format",
	"640x480p  59.94Hz/60Hz", 
	"720x480p  59.94Hz/60Hz", 
	"720x480p  59.94Hz/60Hz", 
	"1280x720p  59.94Hz/60Hz",
	"1920x1080i  59.94Hz/60Hz", 
	"720(1440)x480i 59.94Hz/60Hz", 
	"720(1440)x480i 59.94Hz/60Hz", 
	"720(1440)x240p 59.94Hz/60Hz", 
	"720(1440)x240p 59.94Hz/60Hz", 
	"2880x480i  59.94Hz/60Hz", 
	"2880x480i  59.94Hz/60Hz", 
	"2880x240p  59.94Hz/60Hz", 
	"2880x240p  59.94Hz/60Hz", 
	"1440x480p  59.94Hz/60Hz", 
	"1440x480p  59.94Hz/60Hz", 
	"1920x1080p 59.94Hz/60Hz", 
	"720x576p 50Hz",  
	"720x576p 50Hz",  
	"1280x720p 50Hz",  
	"1920x1080i 50Hz",  
	"720(1440)x576i 50Hz", 
	"720(1440)x576i 50Hz", 
	"720(1440)x288p 50Hz", 
	"720(1440)x288p 50Hz", 
	"2880x576i 50Hz", 
	"2880x576i 50Hz", 
	"2880x288p 50Hz", 
	"2880x288p 50Hz", 
	"1440x576p 50Hz", 
	"1440x576p 50Hz", 
	"1920x1080p 50Hz", 
	"1920x1080p 23.97Hz/24Hz", 
	"1920x1080p 25Hz", 
	"1920x1080p 29.97Hz/30Hz", 
	"2880x480p 59.94Hz/60Hz", 
	"2880x480p 59.94Hz/60Hz", 
	"2880x576p 50Hz",
	"2880x576p 50Hz",
	"1920x1080i (1250 total) 50Hz",
	"1920x1080i 100Hz", 
	"1280x720p  100Hz", 
	"720x576p 100Hz", 
	"720x576p 100Hz", 
	"720(1440)x576i 100Hz", 
	"720(1440)x576i 100Hz", 
	"1920x1080i 119.88/120Hz", 
	"1280x720p  119.88/120Hz", 
	"720x480p   119.88/120Hz", 
	"720x480p   119.88/120Hz",
	"720(1440)x480i 119.88/120Hz", 
	"720(1440)x480i 119.88/120Hz", 
	"720x576p 200Hz",  
	"720x576p 200Hz", 
	"720(1440)x576i 200Hz", 
	"720(1440)x576i 200Hz", 
	"720x480p 239.76/240Hz", 
	"720x480p 239.76/240Hz", 
	"720(1440)x480i 239.76/240Hz",
	"720(1440)x480i 239.76/240Hz", 
	"1280x720p 23.97Hz/24Hz", 
	"1280x720p 25Hz", 
	"1280x720p 29.97Hz/30Hz",
	"1920x1080p 119.88/120Hz",
	"1920x1080p 100Hz"
} ;


static const char *BAVC_HDMI_AviInfoFrame_Text_YQ1YQ0[] =
{
	"Limited Range",
	"Full Range",
	"Reserved (2)"
	"Reserved(3)"
} ;


static const char *BAVC_HDMI_AviInfoFrame_Text_CN1CN0[] =
{
	"Graphics",
	"Photo",
	"Cinema"
	"Game"
} ;



/************************************* Audio Info Frame ****************************************
***********************************************************************************************/

static const char *BAVC_HDMI_AudioInfoFrame_Text_CT3_CT0[] =
{
	"Refer To Stream Header",
	"PCM [24, 25]",
	"AC3",
	"MPEG1",
	"MPEG 2 [multi-chan]",
	"AAC",
	"DTS",
	"ATRAC",
	"One-bit",
	"DDP",
	"DTS-HD",
	"MAT[MLP]",
	"DST",
	"WMA Pro",
	"Reserved"			
} ;


static const char *BAVC_HDMI_AudioInfoFrame_Text_CC2_CC0[] =
{
	"Refer To Stream Header",	"2 ch",
	"3 ch",	"4 ch",
	"5 ch",	"6 ch",
	"7 ch",	"8 ch"	
} ;

static const char *BAVC_HDMI_AudioInfoFrame_Text_SF2_SF0[] =
{
	"Refer To Stream Header",
	"32 kHz",
	"44.1 kHz (CD)",
	"48 kHz",
	"88.2 kHz",
	"96 kHz",
	"176.4 kHz",
	"192 kHz",	
} ;

static const char *BAVC_HDMI_AudioInfoFrame_Text_SS1SS0[] =
{
	"Refer To Stream Header",
	"16 bit",
	"20 bit",
	"24 bit"	
} ;

static const char *BAVC_HDMI_AudioInfoFrame_Text_LSV3_LSV0[] =
{
	"0 db",	"1 db",	"2 db",	"3 db",
	"4 db",	"5 db",	"6 db",	"7 db",
	"8 db",	"9 db",	"10 db",	"11 db",
	"12 db",	"13 db",	"14 db",	"15 db"	
} ;

static const char *BAVC_HDMI_AudioInfoFrame_Text_DM[] =
{
	"Permitted",
	"Prohibited"
} ;

static const char *BAVC_HDMI_SpdInfoFrame_Text_SourceType[] =
{
	"unknown",
	"Digital STB",
	"DVD player",
	"D-VHS",
	"HDD Videorecorder",
	"DVC",
	"DSC",
	"Video CD",
	"Game",
	"PC general",
	"Blu-Ray Disc (BD)",
	"Super Audio CD",
	"HD DVD",
	"PMP"
} ;


static const char *BAVC_HDMI_VsInfoFrame_Text_HdmiVideoFormat[] =
{
	"No Add'l HDMI Format",
	"Extended Resolution Format",
	"3D Format"
} ;


static const char *BAVC_HDMI_VsInfoFrame_Text_3DStructure[]=
{
	"FramePacking",
	"FieldAlternative",
	"LineAlternative",
	"SidexSideFull",
	"LDepth",
	"LDepthGraphics",
	"Top-and-Bottom",
	"Reserved for future use",
	"SidexSideHalf",
	"3D Structure MAX"
} ;

static const char *BAVC_HDMI_VsInfoFrame_Text_3DExtData[]=
{
	"None/HorzOLOR = 0",
	"HorzOLER",
	"HorzELOR",
	"HorzELER",
	"QuinOLOR",
	"QuinOLER",
	"QuinELOR",
	"QuinELER",
	"Max"
} ;


static const char *BAVC_HDMI_VsInfoFrame_Text_HDMIVIC[]=
{
	"Reserved",
	"4Kx2K_2997_30Hz",
	"4Kx2K_25Hz",
	"4Kx2K_2398_24Hz",
	"4Kx2K_SMPTE_24Hz",
	"MAX HDMI VICs"
} ;


/******************************************************************************
Summary:
Return character string for Colorspace (Y1Y0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_ColorspaceToStr(
	BAVC_HDMI_AviInfoFrame_Colorspace uiY1Y0)
{
	uint8_t entries ;
	
	entries = 
		sizeof(BAVC_HDMI_AviInfoFrame_Text_Y1Y0) / 
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_Y1Y0) ;

	if (uiY1Y0 < entries)
		return BAVC_HDMI_AviInfoFrame_Text_Y1Y0[uiY1Y0] ;
	else
		return EnumError ;
}
	

/******************************************************************************
Summary:
Return character string for Active Format (A0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_ActiveFormatToStr(
	BAVC_HDMI_AviInfoFrame_ActiveInfo uiA0)
{
	uint8_t entries ;
	
	entries =
		sizeof(BAVC_HDMI_AviInfoFrame_Text_A0) /
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_A0) ;
		
	if (uiA0 < entries)
		return BAVC_HDMI_AviInfoFrame_Text_A0[uiA0] ;
	else
		return EnumError ;
}


/******************************************************************************
Summary:
Return character string for Bar Info (B1B0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_BarInfoToStr(
	BAVC_HDMI_AviInfoFrame_BarInfo uiB1B0)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AviInfoFrame_Text_B1B0) /
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_B1B0) ;
	
	if (uiB1B0 < entries)
		return BAVC_HDMI_AviInfoFrame_Text_B1B0[uiB1B0] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for Scan Information (S1S0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_ScanInfoToStr(
	BAVC_HDMI_AviInfoFrame_ScanInfo uiS1S0)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AviInfoFrame_Text_S1S0) /
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_S1S0) ;
		
	if (uiS1S0 < entries)
		return BAVC_HDMI_AviInfoFrame_Text_S1S0[uiS1S0] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for Colorimetry (C1C0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_ColorimetryToStr(
	BAVC_HDMI_AviInfoFrame_Colorimetry uiC1C0)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AviInfoFrame_Text_C1C0) /
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_C1C0) ;
		
	if (uiC1C0 < entries)
		return BAVC_HDMI_AviInfoFrame_Text_C1C0[uiC1C0] ;
	else
		return EnumError;
}

/******************************************************************************
Summary:
Return character string for Picture Aspect Ratio (M1M0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_PictureAspectRatioToStr(
	BAVC_HDMI_AviInfoFrame_PictureAspectRatio uiM1M0)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AviInfoFrame_Text_M1M0) /
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_M1M0) ;
		
	if (uiM1M0 < entries)
		return BAVC_HDMI_AviInfoFrame_Text_M1M0[uiM1M0] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for Active Format Aspect Ratio (R3_R0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_ActiveFormatAspectRatioToStr(
	BAVC_HDMI_AviInfoFrame_ActiveFormatAspectRatio uiR3_R0)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AviInfoFrame_Text_R3_R0) /
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_R3_R0) ;
		
	if (uiR3_R0 < entries)
		return BAVC_HDMI_AviInfoFrame_Text_R3_R0[uiR3_R0] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for IT Content (ITC) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_ITContentToStr(
	BAVC_HDMI_AviInfoFrame_ITContent uiITC)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AviInfoFrame_Text_ITC) /
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_ITC) ;
		
	if (uiITC < entries)
		return BAVC_HDMI_AviInfoFrame_Text_ITC[uiITC] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for Extended Colorimetry (EC2_EC0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_ExtendedColorimetryToStr(
	BAVC_HDMI_AviInfoFrame_ExtendedColorimetry uiEC2_EC0)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AviInfoFrame_Text_EC2_EC0) /
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_EC2_EC0) ;
		
	if (uiEC2_EC0 < entries)
		return BAVC_HDMI_AviInfoFrame_Text_EC2_EC0[uiEC2_EC0] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for RGB Quantization Range (Q1Q0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_RGBQuantizationRangeToStr(
	BAVC_HDMI_AviInfoFrame_RGBQuantizationRange uiQ1Q0)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AviInfoFrame_Text_Q1Q0) /
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_Q1Q0) ;
		
	if (uiQ1Q0 < entries)
		return BAVC_HDMI_AviInfoFrame_Text_Q1Q0[uiQ1Q0] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for Scaling (SC1SC0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_ScalingToStr(
	BAVC_HDMI_AviInfoFrame_Scaling uiSC1SC0)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AviInfoFrame_Text_SC1SC0) /
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_SC1SC0) ;
		
	if (uiSC1SC0 < entries)
		return BAVC_HDMI_AviInfoFrame_Text_SC1SC0[uiSC1SC0] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for Video ID Code (VIC) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr(
	BAVC_HDMI_AviInfoFrame_Scaling uiVIC)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AviInfoFrame_Text_VIC) /
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_VIC) ;
		
	if (uiVIC < entries)
		return BAVC_HDMI_AviInfoFrame_Text_VIC[uiVIC] ;
	else
		return EnumError;
}



/******************************************************************************
Summary:
Return character string for IT Content Type enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_ContentTypeToStr(
	BAVC_HDMI_AviInfoFrame_ContentType eContentType)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AviInfoFrame_Text_CN1CN0) /
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_CN1CN0) ;
		
	if (eContentType < entries)
		return BAVC_HDMI_AviInfoFrame_Text_CN1CN0[eContentType] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for Ycc Quantization Range enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AviInfoFrame_YccQuantizationRangeToStr(
	BAVC_HDMI_AviInfoFrame_ContentType eYccQuantizationRange)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AviInfoFrame_Text_YQ1YQ0) /
		sizeof(*BAVC_HDMI_AviInfoFrame_Text_YQ1YQ0) ;
		
	if (eYccQuantizationRange < entries)
		return BAVC_HDMI_AviInfoFrame_Text_YQ1YQ0[eYccQuantizationRange] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for Audio Coding Type (CT3_CT0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AudioInfoFrame_CodingTypeToStr(
	BAVC_HDMI_AudioInfoFrame_CodingType uiCT3_CT0)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AudioInfoFrame_Text_CT3_CT0) /
		sizeof(*BAVC_HDMI_AudioInfoFrame_Text_CT3_CT0) ;
		
	if (uiCT3_CT0 < entries)
		return BAVC_HDMI_AudioInfoFrame_Text_CT3_CT0[uiCT3_CT0] ;
	else
		return EnumError;
}

/******************************************************************************
Summary:
Return character string for Audio Channel Count (CC2_CC0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AudioInfoFrame_ChannelCountToStr(
	BAVC_HDMI_AudioInfoFrame_ChannelCount uiCC2_CC0)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AudioInfoFrame_Text_CC2_CC0) /
		sizeof(*BAVC_HDMI_AudioInfoFrame_Text_CC2_CC0) ;
		
	if (uiCC2_CC0 < entries)
		return BAVC_HDMI_AudioInfoFrame_Text_CC2_CC0[uiCC2_CC0] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for Sampling Frequency (SF2_SF0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AudioInfoFrame_SampleFrequencyToStr(
	BAVC_HDMI_AudioInfoFrame_SampleFrequency uiSF2_SF0)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AudioInfoFrame_Text_SF2_SF0) /
		sizeof(*BAVC_HDMI_AudioInfoFrame_Text_SF2_SF0) ;
		
	if (uiSF2_SF0 < entries)
		return BAVC_HDMI_AudioInfoFrame_Text_SF2_SF0[uiSF2_SF0] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for Sample Size (SS1SS0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AudioInfoFrame_SampleSizeToStr(
	BAVC_HDMI_AudioInfoFrame_SampleSize uiSS1SS0)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AudioInfoFrame_Text_SS1SS0) /
		sizeof(*BAVC_HDMI_AudioInfoFrame_Text_SS1SS0) ;
		
	if (uiSS1SS0 < entries)
		return BAVC_HDMI_AudioInfoFrame_Text_SS1SS0[uiSS1SS0] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for Level Shift Values (LSV3_LSV0) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AudioInfoFrame_LevelShiftToStr(
	BAVC_HDMI_AudioInfoFrame_LevelShift uiLSV3_LSV0)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AudioInfoFrame_Text_LSV3_LSV0) /
		sizeof(*BAVC_HDMI_AudioInfoFrame_Text_LSV3_LSV0) ;
		
	if (uiLSV3_LSV0 < entries)
		return BAVC_HDMI_AudioInfoFrame_Text_LSV3_LSV0[uiLSV3_LSV0] ;
	else
		return EnumError;
}


/******************************************************************************
Summary:
Return character string for Colorspace (DM) enumeration 
*******************************************************************************/
const char * BAVC_HDMI_AudioInfoFrame_DownMixInhibitToStr(
	BAVC_HDMI_AudioInfoFrame_DownMixInhibit uiDM)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_AudioInfoFrame_Text_DM) /
		sizeof(*BAVC_HDMI_AudioInfoFrame_Text_DM) ;
		
	if (uiDM < entries)
		return BAVC_HDMI_AudioInfoFrame_Text_DM[uiDM] ;
	else
		return EnumError;
}



#if BAVC_HDMI_RECEIVER
/***************************************************************************
Summary:
	Get Color Matrix based on CEA 861 AviInfoFrame information
***************************************************************************/
uint8_t BAVC_HDMI_AviInfoFrameToColorMatrix(
	BAVC_HDMI_AviInfoFrame *pAviInfoFrame,
	uint8_t uiVideoIdCode)
{
    BAVC_HDMI_AviInfoFrame_VideoFormat eAviInfoFrameFormat ;
    bool limitedRange ;
    BAVC_CscMode     cscMode ;

    BSTD_UNUSED(uiVideoIdCode) ;
	

    /* set the color matrix */
    if (pAviInfoFrame->VideoIdCode == 0 )
        /* IT FORMAT */
        eAviInfoFrameFormat = BAVC_HDMI_AviInfoFrame_VideoFormat_eIT ;
    
    else if (((pAviInfoFrame->VideoIdCode >=  2) && (pAviInfoFrame->VideoIdCode <=  3))
    || ((pAviInfoFrame->VideoIdCode >=  6) && (pAviInfoFrame->VideoIdCode <= 15))
    || ((pAviInfoFrame->VideoIdCode >= 17) && (pAviInfoFrame->VideoIdCode <= 18))
    || ((pAviInfoFrame->VideoIdCode >= 21) && (pAviInfoFrame->VideoIdCode <= 30))
    || ((pAviInfoFrame->VideoIdCode >= 35) && (pAviInfoFrame->VideoIdCode <= 38))
    || ((pAviInfoFrame->VideoIdCode >= 42) && (pAviInfoFrame->VideoIdCode <= 45))
    || ((pAviInfoFrame->VideoIdCode >= 48) && (pAviInfoFrame->VideoIdCode <= 50)))
            /* SD FORMAT */
            eAviInfoFrameFormat = BAVC_HDMI_AviInfoFrame_VideoFormat_eSD ;
    
    else if (((pAviInfoFrame->VideoIdCode >=  4) && (pAviInfoFrame->VideoIdCode <=  5))
    || ((pAviInfoFrame->VideoIdCode ==  16))
    || ((pAviInfoFrame->VideoIdCode >=  19) && (pAviInfoFrame->VideoIdCode <= 20))
    || ((pAviInfoFrame->VideoIdCode >=  31) && (pAviInfoFrame->VideoIdCode <= 34))
    || ((pAviInfoFrame->VideoIdCode >=  39) && (pAviInfoFrame->VideoIdCode <= 41))
    || ((pAviInfoFrame->VideoIdCode >=  46) && (pAviInfoFrame->VideoIdCode <= 47)))
            /* HD FORMAT */
            eAviInfoFrameFormat = BAVC_HDMI_AviInfoFrame_VideoFormat_eHD ;
    
    else 
    {
        /* New/Unknown Format */
        BDBG_WRN(("Unknown/Unsupported VIDEO ID Code %d; Assuming HD format", 
			pAviInfoFrame->VideoIdCode)) ;
            eAviInfoFrameFormat = BAVC_HDMI_AviInfoFrame_VideoFormat_eHD ;
    }

    
    /* YCbCr */
    if ((pAviInfoFrame->ePixelEncoding == BAVC_HDMI_AviInfoFrame_Colorspace_eYCbCr422) 
    || (pAviInfoFrame->ePixelEncoding == BAVC_HDMI_AviInfoFrame_Colorspace_eYCbCr444))
    {
        /* Unspecified colorimetry, but SD Format */
        if (((pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eNoData)
        && (eAviInfoFrameFormat == BAVC_HDMI_AviInfoFrame_VideoFormat_eSD))
    	
        /* OR Colorimetry is specified as BT.601 */
        || (pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eSmpte170)
    	
        /* OR Colorimetry is specified as  xvYCC601 */
        || ((pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended)
        && (pAviInfoFrame->eExtendedColorimetry == BAVC_HDMI_AviInfoFrame_ExtendedColorimetry_exvYCC601)))
        { 
            /* Set the BT.601 YCbCr flag */
	     cscMode = BAVC_CscMode_e601YCbCr ;
        }
        else
        {     
            /* Set the BT.709 YCbCr flag */
            cscMode = BAVC_CscMode_e709YCbCr ;
          }
    }
    /****** RGB ******/
    else if (pAviInfoFrame->ePixelEncoding == BAVC_HDMI_AviInfoFrame_Colorspace_eRGB) 
    {
        /* determine whether color range limited vs full */
    	
        /* if xv.Color (always scaled as limited range) */			
        /* if ((C=3) Or (Q=1) Or  ((Q=0) And (FormatIsSD Or FormatIsHD)))		 */
		
        if ((pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended)
    		
        /* OR limited range is expressly specified */
        ||  (pAviInfoFrame->eRGBQuantizationRange == BAVC_HDMI_AviInfoFrame_RGBQuantizationRange_eLimitedRange) 
    	      
        /* OR video format is used to select */
        ||  ((pAviInfoFrame->eRGBQuantizationRange == BAVC_HDMI_AviInfoFrame_RGBQuantizationRange_eDefault) 
        &&    ((eAviInfoFrameFormat == BAVC_HDMI_AviInfoFrame_VideoFormat_eSD)
        || (eAviInfoFrameFormat == BAVC_HDMI_AviInfoFrame_VideoFormat_eHD))))
        { 
    	    limitedRange = true ;
#if 0			
           if (rgbQuantization == BAVC_HDMI_AviInfoFrame_RGBQuantizationRange_eDefault)
               limitedRange = false ;
#endif		   
        }
        else
        { 
            limitedRange = false ;
        }
    
    
    	/* NOW determine which colorimetry */
    	
    	/* Unspecified Colorimetry, but SD Format */
        if (((pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eNoData)
        && (eAviInfoFrameFormat == BAVC_HDMI_AviInfoFrame_VideoFormat_eSD))
    
        /* OR Colorimetry is specified as BT.601 */
        || (pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eSmpte170)
        
        /* OR Colorimetry is specified as  xvYCC601 */
        || ((pAviInfoFrame->eColorimetry == BAVC_HDMI_AviInfoFrame_Colorimetry_eExtended)
        && (pAviInfoFrame->eExtendedColorimetry == BAVC_HDMI_AviInfoFrame_ExtendedColorimetry_exvYCC601)))
        {     
            if (limitedRange)
                /****** BT.601 LIMITED ******/
                cscMode = BAVC_CscMode_e601RgbLimitedRange ;
            else
    
                /****** BT.601 FULL  ******/			
                cscMode = BAVC_CscMode_e601RgbFullRange ;
            }
            else
            {
                if (limitedRange)
                    /****** BT.709 LIMITED ******/
                    cscMode = BAVC_CscMode_e709RgbLimitedRange ;
                else
                    /****** BT.709 FULL  ******/
                    cscMode = BAVC_CscMode_e709RgbFullRange ;
            }
        }
	
	return 0 ;
}
#endif

/******************************************************************************
Summary:
Return character string for HDMI Packet Type 
*******************************************************************************/
const char * BAVC_HDMI_PacketTypeToStr(
	BAVC_HDMI_PacketType ePacketType)
{
	uint8_t entries, i ;


	entries =
		sizeof(PacketTypes_Text) /
		sizeof(*PacketTypes_Text) ;

	for (i = 0 ; i < entries; i++)
	{
		if (ePacketType == PacketTypes_Text[i].ePacketType)
			return PacketTypes_Text[i].PacketName;
	}
	return BAVC_HDMI_P_PacketTypeUnknown ;
}


/******************************************************************************
Summary:
Return character string for SPD Device Type enumeration 
*******************************************************************************/
const char * BAVC_HDMI_SpdInfoFrame_SourceTypeToStr(
	BAVC_HDMI_SpdInfoFrame_SourceType eSourceType)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_SpdInfoFrame_Text_SourceType) /
		sizeof(*BAVC_HDMI_SpdInfoFrame_Text_SourceType) ;
		
	if (eSourceType < entries)
		return BAVC_HDMI_SpdInfoFrame_Text_SourceType[eSourceType] ;
	else
		return Reserved ;
}


/******************************************************************************
Summary:
Return character string for VS Hdmi Video Format  enumeration 
*******************************************************************************/
const char * BAVC_HDMI_VsInfoFrame_HdmiVideoFormatToStr(
	BAVC_HDMI_VSInfoFrame_HDMIVideoFormat eFormatType)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_VsInfoFrame_Text_HdmiVideoFormat	) /
		sizeof(*BAVC_HDMI_VsInfoFrame_Text_HdmiVideoFormat) ;
		
	if (eFormatType < entries)
		return BAVC_HDMI_VsInfoFrame_Text_HdmiVideoFormat[eFormatType] ;
	else
		return Reserved ;
}

/******************************************************************************
Summary:
Return character string for VS 3D Structure   
******************************************************************************/
const char * BAVC_HDMI_VsInfoFrame_3DStructureToStr(
	BAVC_HDMI_VSInfoFrame_3DStructure e3DStructure)
{
	uint8_t entries ;
	
	entries =
		sizeof(BAVC_HDMI_VsInfoFrame_Text_3DStructure) /
		sizeof(*BAVC_HDMI_VsInfoFrame_Text_3DStructure) ;
	
	if (e3DStructure < entries)
		return BAVC_HDMI_VsInfoFrame_Text_3DStructure[e3DStructure] ;
	else
		return Reserved ;
}


/******************************************************************************
Summary:
Return character string for VS 3D Ext Data enumeration 
*******************************************************************************/
const char * BAVC_HDMI_VsInfoFrame_3DExtDataToStr(
	BAVC_HDMI_VSInfoFrame_3DExtData e3DExtType)
{
	uint8_t entries ;
	
	entries =
		sizeof(BAVC_HDMI_VsInfoFrame_Text_3DExtData) /
		sizeof(*BAVC_HDMI_VsInfoFrame_Text_3DExtData) ;
		
	if (e3DExtType < entries)
		return BAVC_HDMI_VsInfoFrame_Text_3DExtData[e3DExtType] ;
	else
		return Reserved ;
}


/******************************************************************************
Summary:
Return character string for VS 3D Ext Data enumeration 
*******************************************************************************/
const char * BAVC_HDMI_VsInfoFrame_HdmiVicToStr(
	BAVC_HDMI_VSInfoFrame_HDMIVIC  eHdmiVic)
{
	uint8_t entries ;
	
	entries=
		sizeof(BAVC_HDMI_VsInfoFrame_Text_HDMIVIC) /
		sizeof(*BAVC_HDMI_VsInfoFrame_Text_HDMIVIC) ;
		
	if (eHdmiVic < entries)
		return BAVC_HDMI_VsInfoFrame_Text_HDMIVIC[eHdmiVic] ;
	else
		return Reserved ;
}


/******************************************************************************
unsigned int BAVC_HDMI_HDCP_NumberOfSetBits
Summary: Get the number of Set Bits
*******************************************************************************/
unsigned int BAVC_HDMI_HDCP_NumberOfSetBits(const unsigned char *bytes, int nbytes)
{
	int i, j ;
	int bit ;
	int count = 0 ;
	uint8_t byteToCheck;

	count = 0 ;
	for (i = 0; i < nbytes; i++)
	{
		bit = 1 ;
		byteToCheck = bytes[i];
		for (j = 0; j < 8 ; j++)
		{
			if (bit & byteToCheck)
				count++ ;
			bit = bit << 1 ;
		}
	}
	return count ;
} /* end BAVC_HDMI_HDCP_NumberOfSetBits */


