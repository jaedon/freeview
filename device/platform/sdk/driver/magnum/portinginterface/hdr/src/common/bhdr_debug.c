/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr_debug.c $
 * $brcm_Revision: Hydra_Software_Devel/22 $
 * $brcm_Date: 9/24/12 5:53p $
 *
 * $brcm_Log: /magnum/portinginterface/hdr/src/common/bhdr_debug.c $
 * 
 * Hydra_Software_Devel/22   9/24/12 5:53p rgreen
 * SW7445-24:  Add 28nm support
 * 
 * Hydra_Software_Devel/21   4/5/12 11:43a rgreen
 * SW7425-2838:  Fix static overrun coverity error
 * 
 * Hydra_Software_Devel/20   8/3/11 2:08p rgreen
 * SW7425-833: Update to use BDBG_OBJECT_ASSERT
 * 
 * Hydra_Software_Devel/19   2/11/11 2:48p rgreen
 * SW35230-3014: Add support for YCC Qunatization and IT Content Type
 * fields in AVI InfoFrame
 * 
 * Hydra_Software_Devel/19   2/11/11 2:47p rgreen
 * SW35230-3014: Add support for YCC Qunatization and IT Content Type
 * fields in AVI InfoFrame
 * 
 * Hydra_Software_Devel/18   12/17/10 11:07p hongtaoz
 * SW7425-21: fixed compile warning for kernel mode;
 * 
 * Hydra_Software_Devel/17   12/8/10 3:00p rgreen
 * SW7425-21: Fix compilation error for kernel mode
 * 
 * Hydra_Software_Devel/16   12/7/10 4:59p rgreen
 * SW35230-2416: Move debug function to bhdr_debug.c
 * 
 * Hydra_Software_Devel/15   5/25/10 7:55p rgreen
 * SW3556-1117: Update reporting of No Valid Data when A0 specifies No
 * Data
 * 
 * Hydra_Software_Devel/14   5/11/10 5:40p rgreen
 * SW3548-2708: Merge
 * 
 * Hydra_Software_Devel/SW3548-2642/1   5/11/10 5:38p rgreen
 * SW3548-2708: Update IEEE RegID output
 * 
 * Hydra_Software_Devel/13   3/15/10 5:12p rgreen
 * SW3548-2820: Add support for  VS Infoframe 3D Structure and Ext Data
 * fields
 * 
 * Hydra_Software_Devel/12   1/13/10 4:48p rgreen
 * SW3548-2708: Add debug support for Vendor Specific InfoFrame
 * 
 * Hydra_Software_Devel/11   12/31/09 12:48p rgreen
 * SW3548-2013: Update ConfigureEventCounter and ResetAllEventCounter
 * functions
 * 
 * Hydra_Software_Devel/10   12/14/09 10:31p rgreen
 * SW3548-2013: Update packet debug messages to reduce output.  Add
 * private function to configure event counter.
 * 
 * Hydra_Software_Devel/9   11/30/09 2:25p rgreen
 * SW35230-15: Add support for 35230; remove unnecessary frontend includes
 * 
 * Hydra_Software_Devel/8   10/30/09 4:58p rgreen
 * SW3548-2013:  Add debug support for Audio Clock Regeneration Packet
 * 
 * Hydra_Software_Devel/7   10/13/09 1:13p rgreen
 * SW3548-2013: Add support for SPD InfoFrame debugging
 * 
 * Hydra_Software_Devel/6   9/15/09 11:18a rgreen
 * SW3548-2013: Minimize debug messages for Packet RAM dumps
 * 
 * Hydra_Software_Devel/5   5/27/09 3:33p rgreen
 * PR54000: Update debug messages to indicate source (channel) of messages
 * 
 * Hydra_Software_Devel/4   5/15/09 11:49a rgreen
 * PR54000: Add conversion debug functions for Video Id Code (VIC) to
 * string
 * 
 * Hydra_Software_Devel/3   5/12/09 7:52p rgreen
 * PR54000: Use debug enum to string conversion functions
 * 
 * Hydra_Software_Devel/2   5/1/09 4:17p rgreen
 * PR54000: Move debug functions to separate bhdr_debug.c file.
 * Add options to dump Infoframes, monitor format changes, monitor values
 * etc
 * 
 * Hydra_Software_Devel/1   4/8/09 10:07a rgreen
 * PR54000: Add HDR debug support
 * 
  ***************************************************************************/
  
#if BDBG_DEBUG_BUILD
#if defined LINUX && !defined __KERNEL__
#include "ctype.h"
#endif
#endif

#include "bavc_hdmi.h"

#include "bhdr.h"
#include "bhdr_priv.h"

#include "bhdr_debug.h"


#if BHDR_CONFIG_DEBUG_TIMER_S
#include "bchp_hd_dvi_0.h"
#endif 
 
BDBG_MODULE(BHDR_DEBUG) ;


#if BHDR_CONFIG_DEBUG_HDCP_VALUES
void BHDR_P_DebugHdcpValues_isr(BHDR_Handle hHDR) 
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset   ;
	
	uint8_t AvMute, PacketProcessorAvMute ;
	uint32_t An0, An1, Aksv0, Aksv1;
	
	BDBG_ENTER(BHDR_P_DebugHdcpValues_isr) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_MISC_STATUS + ulOffset) ;
	AvMute = (uint8_t) BCHP_GET_FIELD_DATA(Register, 
		HDMI_RX_0_MISC_STATUS, AV_MUTE) ;

	PacketProcessorAvMute = (uint8_t) BCHP_GET_FIELD_DATA(Register, 
		HDMI_RX_0_MISC_STATUS, PKT_PROCESSOR_AV_MUTE) ;

	BDBG_WRN(("CH%d HDCP Auth Request (Load keys)... AvMute: %d ; Packet AvMute: %d", 
		hHDR->eCoreId, AvMute, PacketProcessorAvMute)) ;

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_MON_AN_1 + ulOffset) ;	
	An1 = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_HDCP_MON_AN_1, HDCP_RX_MON_AN) ;

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_MON_AN_0 + ulOffset) ;	
	An0 = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_HDCP_MON_AN_0, HDCP_RX_MON_AN) ;

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_MON_AKSV_1 + ulOffset) ;
	Aksv1 = 	BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_HDCP_MON_AKSV_1, HDCP_RX_MON_AKSV) ;

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_MON_AKSV_0 + ulOffset) ;
	Aksv0 = 	BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_HDCP_MON_AKSV_0, HDCP_RX_MON_AKSV) ;

	BDBG_WRN((" CH%d Recevied Tx An:   %x%x", hHDR->eCoreId, An1, An0)) ;
	BDBG_WRN((" CH%d Recevied Tx Aksv: %x%x",   hHDR->eCoreId, Aksv1, Aksv0)) ;

	BDBG_LEAVE(BHDR_P_DebugHdcpValues_isr) ;
}
#endif


void BHDR_P_DEBUG_AviInfoFrame(
	BAVC_HDMI_AviInfoFrame *OldAvi, 
	BAVC_HDMI_AviInfoFrame *NewAvi)
{
	BDBG_WRN(("-------------------- PARSED AVI INFOFRAME -------------------")) ;
	if (OldAvi->ePixelEncoding != NewAvi->ePixelEncoding)
	{
		BDBG_WRN(("(Y1Y0)     PixelEncoding/Colorspace  : %s (%d) was %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ColorspaceToStr(NewAvi->ePixelEncoding), 
			NewAvi->ePixelEncoding,
			BAVC_HDMI_AviInfoFrame_ColorspaceToStr(OldAvi->ePixelEncoding),
			OldAvi->ePixelEncoding)) ;
	}
	else
	{
		BDBG_WRN(("(Y1Y0)     PixelEncoding/Colorspace  : %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ColorspaceToStr(NewAvi->ePixelEncoding),
			NewAvi->ePixelEncoding)) ;
	}
 
	
	if (OldAvi->eActiveInfo != NewAvi->eActiveInfo)
	{
		BDBG_WRN(("(A0)       Active Format Info Valid  : %s (%d) was %s (%d)",
			BAVC_HDMI_AviInfoFrame_ActiveFormatToStr(NewAvi->eActiveInfo), 
			NewAvi->eActiveInfo,
			BAVC_HDMI_AviInfoFrame_ActiveFormatToStr(OldAvi->eActiveInfo),
			OldAvi->eActiveInfo)) ;
	}
	else
	{
		BDBG_WRN(("(A0)       Active Format Info Valid  : %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ActiveFormatToStr(NewAvi->eActiveInfo),
			NewAvi->eActiveInfo)) ;
	}

		
	if (OldAvi->eBarInfo != NewAvi->eBarInfo)
	{
		BDBG_WRN(("(B1B0)     Bar Info Valid            : %s (%d) was %s (%d)", 
			BAVC_HDMI_AviInfoFrame_BarInfoToStr(NewAvi->eBarInfo), 
			NewAvi->eBarInfo,
			BAVC_HDMI_AviInfoFrame_BarInfoToStr(OldAvi->eBarInfo),
			OldAvi->eBarInfo)) ;
	}
	else
	{
		BDBG_WRN(("(B1B0)     Bar Info Valid            : %s (%d)", 
			BAVC_HDMI_AviInfoFrame_BarInfoToStr(NewAvi->eBarInfo),
			NewAvi->eBarInfo)) ;
	}
		
		
	if (OldAvi->eScanInfo != NewAvi->eScanInfo)
	{
		BDBG_WRN(("(S1S0)     Scan Information          : %s (%d) was %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ScanInfoToStr(NewAvi->eScanInfo),
			NewAvi->eScanInfo,
			BAVC_HDMI_AviInfoFrame_ScanInfoToStr(OldAvi->eScanInfo),
			OldAvi->eScanInfo)) ;
	}
	else
	{
		BDBG_WRN(("(S1S0)     Scan Information          : %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ScanInfoToStr(NewAvi->eScanInfo),
			NewAvi->eScanInfo)) ;
	}
		
		
	if (OldAvi->eColorimetry != NewAvi->eColorimetry)
	{
		BDBG_WRN(("(C1C0)     Colorimetry               : %s (%d) was %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ColorimetryToStr(NewAvi->eColorimetry),
			NewAvi->eColorimetry,
			BAVC_HDMI_AviInfoFrame_ColorimetryToStr(OldAvi->eColorimetry), 
			OldAvi->eColorimetry)) ;
	}
	else
	{
		BDBG_WRN(("(C1C0)     Colorimetry               : %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ColorimetryToStr(NewAvi->eColorimetry),
			NewAvi->eColorimetry	)) ;
	}
	
	
	if (OldAvi->ePictureAspectRatio != NewAvi->ePictureAspectRatio)
	{
		BDBG_WRN(("(M1M0)     Picture AR                : %s (%d) was %s (%d)", 
			BAVC_HDMI_AviInfoFrame_PictureAspectRatioToStr(NewAvi->ePictureAspectRatio), 
			NewAvi->ePictureAspectRatio,
			BAVC_HDMI_AviInfoFrame_PictureAspectRatioToStr(OldAvi->ePictureAspectRatio),
			OldAvi->ePictureAspectRatio)) ;
	}
	else
	{
		BDBG_WRN(("(M1M0)     Picture AR                : %s (%d)", 
			BAVC_HDMI_AviInfoFrame_PictureAspectRatioToStr(NewAvi->ePictureAspectRatio),
			NewAvi->ePictureAspectRatio)) ;
	}
	
	
	if (OldAvi->eActiveFormatAspectRatio != NewAvi->eActiveFormatAspectRatio)
	{
		BDBG_WRN(("(R3..R0)   Active Format AR          : %s (%d) was %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ActiveFormatAspectRatioToStr(NewAvi->eActiveFormatAspectRatio - 8 ), 
			NewAvi->eActiveFormatAspectRatio,
			OldAvi->eActiveInfo ? 
				BAVC_HDMI_AviInfoFrame_ActiveFormatAspectRatioToStr(OldAvi->eActiveFormatAspectRatio - 8) :
				"No Data", 
			OldAvi->eActiveFormatAspectRatio)) ;
	}
	else
	{
		BDBG_WRN(("(R3..R0)   Active Format AR          : %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ActiveFormatAspectRatioToStr(NewAvi->eActiveFormatAspectRatio-8),
			NewAvi->eActiveFormatAspectRatio)) ;
	}	
		
		
	if (OldAvi->eITContent != NewAvi->eITContent)
	{
		BDBG_WRN(("(ITC)      IT Content                : %s (%d) was %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ITContentToStr(NewAvi->eITContent), 
			NewAvi->eITContent,
			BAVC_HDMI_AviInfoFrame_ITContentToStr(OldAvi->eITContent),
			OldAvi->eITContent)) ;
	}
	else
	{
		BDBG_WRN(("(ITC)      IT Content                : %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ITContentToStr(NewAvi->eITContent),
			NewAvi->eITContent)) ;
	}
		
		

	if (OldAvi->eExtendedColorimetry != NewAvi->eExtendedColorimetry)
	{
		BDBG_WRN(("(EC2..EC0) Extended Colorimetry      : %s (%d) was %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ExtendedColorimetryToStr(NewAvi->eExtendedColorimetry), 
			NewAvi->eExtendedColorimetry,
			BAVC_HDMI_AviInfoFrame_ExtendedColorimetryToStr(OldAvi->eExtendedColorimetry),
			OldAvi->eExtendedColorimetry)) ;
	}
	else
	{
		BDBG_WRN(("(EC2..EC0) Extended Colorimetry      : %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ExtendedColorimetryToStr(NewAvi->eExtendedColorimetry),
			NewAvi->eExtendedColorimetry)) ;
	}
	
	
	if (OldAvi->eRGBQuantizationRange != NewAvi->eRGBQuantizationRange)
	{
		BDBG_WRN(("(Q1Q0)     RGB Quantization Range    : %s (%d) was %s (%d)", 
			BAVC_HDMI_AviInfoFrame_RGBQuantizationRangeToStr(NewAvi->eRGBQuantizationRange),
			NewAvi->eRGBQuantizationRange,
			BAVC_HDMI_AviInfoFrame_RGBQuantizationRangeToStr(OldAvi->eRGBQuantizationRange),
			OldAvi->eRGBQuantizationRange)) ;
	}
	else
	{
		BDBG_WRN(("(Q1Q0)     RGB Quantization Range    : %s (%d)", 
			BAVC_HDMI_AviInfoFrame_RGBQuantizationRangeToStr(NewAvi->eRGBQuantizationRange),
			NewAvi->eRGBQuantizationRange)) ;
	}
	
	
	if (OldAvi->eScaling != NewAvi->eScaling)
	{
		BDBG_WRN(("(SC1SC0)   Picture Scaling           : %s (%d) was %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ScalingToStr(NewAvi->eScaling),
			NewAvi->eScaling,
			BAVC_HDMI_AviInfoFrame_ScalingToStr(OldAvi->eScaling),
			OldAvi->eScaling)) ;
	}
	else
	{
		BDBG_WRN(("(SC1SC0)   Picture Scaling           : %s (%d)", 
			BAVC_HDMI_AviInfoFrame_ScalingToStr(NewAvi->eScaling),
			NewAvi->eScaling)) ;
	}
	
/****************************/

	if (OldAvi->VideoIdCode != NewAvi->VideoIdCode)
	{
		BDBG_WRN(("(VIC6..0)  Video Code                : %s (%d) was %s (%d) ",
			BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr(NewAvi->VideoIdCode),
			NewAvi->VideoIdCode, 
			BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr(OldAvi->VideoIdCode),
			OldAvi->VideoIdCode)) ;
	}
	else
	{
		BDBG_WRN(("(VIC6..0)  Video Code                : %s (%d) ",
			BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr(NewAvi->VideoIdCode),
			NewAvi->VideoIdCode)) ;
	}
	
	
	if (OldAvi->PixelRepeat != NewAvi->PixelRepeat)
	{
		BDBG_WRN(("(PR3..PR0) Pixel Repeat              : %d was %d ",
			NewAvi->PixelRepeat, OldAvi->PixelRepeat)) ;
	}
	else
	{
		BDBG_WRN(("(PR3..PR0) Pixel Repeat              : %d ", 
		NewAvi->PixelRepeat)) ;
	}

	if (OldAvi->eContentType != NewAvi->eContentType)
	{
		BDBG_WRN(("(CN1CN0)  IT Content Type            : %s (%d) was %s (%d) ",
			BAVC_HDMI_AviInfoFrame_ContentTypeToStr(NewAvi->eContentType),
			NewAvi->eContentType, 
			BAVC_HDMI_AviInfoFrame_ContentTypeToStr(OldAvi->eContentType),
			OldAvi->eContentType)) ;
	}
	else
	{
		BDBG_WRN(("(CN1CN0)  IT Content Type            : %s (%d) ",
			BAVC_HDMI_AviInfoFrame_ContentTypeToStr(NewAvi->eContentType),
			NewAvi->eContentType)) ;
	}
	
	
	if (OldAvi->eYccQuantizationRange != NewAvi->eYccQuantizationRange)
	{
		BDBG_WRN(("(YQ1YQ0)  YCC Quantization Range     : %s (%d) was %s (%d) ",
			BAVC_HDMI_AviInfoFrame_YccQuantizationRangeToStr(NewAvi->eYccQuantizationRange),
			NewAvi->eYccQuantizationRange, 
			BAVC_HDMI_AviInfoFrame_YccQuantizationRangeToStr(OldAvi->eYccQuantizationRange),
			OldAvi->eYccQuantizationRange)) ;
	}
	else
	{
		BDBG_WRN(("(YQ1YQ0)  YCC Quantization Range     : %s (%d) ",
			BAVC_HDMI_AviInfoFrame_YccQuantizationRangeToStr(NewAvi->eYccQuantizationRange),
			NewAvi->eYccQuantizationRange)) ;
	}		
		
		
	if (OldAvi->TopBarEndLineNumber != NewAvi->TopBarEndLineNumber)
	{
		BDBG_WRN(("Top Bar End Line #                   : %d was %d ",
			NewAvi->TopBarEndLineNumber, OldAvi->TopBarEndLineNumber )) ;
	}
	else
	{
		BDBG_WRN(("Top Bar End Line #                   : %d ",
		NewAvi->TopBarEndLineNumber)) ;
	}
	

	if (OldAvi->BottomBarStartLineNumber != NewAvi->BottomBarStartLineNumber)
	{
		BDBG_WRN(("Bottom Bar Start Line #              : %d was %d ",
			NewAvi->BottomBarStartLineNumber, OldAvi->BottomBarStartLineNumber )) ;
	}
	else
	{
		BDBG_WRN(("Bottom Bar Start Line #              : %d ",
			NewAvi->BottomBarStartLineNumber )) ;
	}
	
	
	if (OldAvi->LeftBarEndPixelNumber != NewAvi->LeftBarEndPixelNumber)
	{
		BDBG_WRN(("Left Bar End Pixel #                 : %d was %d ",
			NewAvi->LeftBarEndPixelNumber, OldAvi->LeftBarEndPixelNumber )) ;
	}
	else
	{
		BDBG_WRN(("Left Bar End Pixel #                 : %d ",
			NewAvi->LeftBarEndPixelNumber )) ;
	}
	

	if (OldAvi->RightBarEndPixelNumber != NewAvi->RightBarEndPixelNumber)
	{
		BDBG_WRN(("Right Bar End Pixel #                : %d was %d ",
			NewAvi->RightBarEndPixelNumber, OldAvi->RightBarEndPixelNumber )) ;
	}
	else
	{
		BDBG_WRN(("Right Bar End Pixel #                : %d ",
			NewAvi->RightBarEndPixelNumber)) ;
	}
		
	BDBG_WRN(("--------------------- END AVI INFOFRAME  -------------------")) ;
	
}



void BHDR_P_DEBUG_AudioInfoFrame(
	BAVC_HDMI_AudioInfoFrame *OldAudioInfoFrame, 
	BAVC_HDMI_AudioInfoFrame *NewAudioInfoFrame)
{
	BDBG_WRN(("-------------------- PARSED AUDIO INFOFRAME -------------------")) ;
		
	if (OldAudioInfoFrame->CodingType != NewAudioInfoFrame->CodingType)
	{
		BDBG_WRN(("(CT3_CT0)       Coding Type          : %s (%d) was %s (%d)", 
			BAVC_HDMI_AudioInfoFrame_CodingTypeToStr(NewAudioInfoFrame->CodingType), 
			NewAudioInfoFrame->CodingType,
			BAVC_HDMI_AudioInfoFrame_CodingTypeToStr(OldAudioInfoFrame->CodingType),
			OldAudioInfoFrame->CodingType)) ;
	}
	else
	{
		BDBG_WRN(("(CT3_CT0)       Coding Type         : %s (%d)",
		BAVC_HDMI_AudioInfoFrame_CodingTypeToStr(NewAudioInfoFrame->CodingType),
		NewAudioInfoFrame->CodingType)) ;
	}

	
	if (OldAudioInfoFrame->ChannelCount != NewAudioInfoFrame->ChannelCount)
	{
		BDBG_WRN(("(CC2_CC0)       Channel Count       : %s (%d) was %s (%d)", 
			BAVC_HDMI_AudioInfoFrame_ChannelCountToStr(NewAudioInfoFrame->ChannelCount), 
			NewAudioInfoFrame->ChannelCount,
			BAVC_HDMI_AudioInfoFrame_ChannelCountToStr(OldAudioInfoFrame->ChannelCount),
			OldAudioInfoFrame->ChannelCount)) ;
	}
	else
	{	
		BDBG_WRN(("(CC2_CC0)       Channel Count       : %s (%d)", 
			BAVC_HDMI_AudioInfoFrame_ChannelCountToStr(NewAudioInfoFrame->ChannelCount),
			NewAudioInfoFrame->ChannelCount)) ;
	}
	
	
	if (OldAudioInfoFrame->SampleFrequency != NewAudioInfoFrame->SampleFrequency)
	{
		BDBG_WRN(("(SF2_SF0)       Sample Frequency    : %s (%d) was %s (%d)", 
			BAVC_HDMI_AudioInfoFrame_SampleFrequencyToStr(NewAudioInfoFrame->SampleFrequency), 
			NewAudioInfoFrame->SampleFrequency,
			BAVC_HDMI_AudioInfoFrame_SampleFrequencyToStr(OldAudioInfoFrame->SampleFrequency),
			OldAudioInfoFrame->SampleFrequency)) ;
	}
	else
	{
		BDBG_WRN(("(SF2_SF0)       Sample Frequency    : %s (%d)", 
			BAVC_HDMI_AudioInfoFrame_SampleFrequencyToStr(NewAudioInfoFrame->SampleFrequency),
			NewAudioInfoFrame->SampleFrequency)) ;
	}
		
		
	if (OldAudioInfoFrame->SampleSize != NewAudioInfoFrame->SampleSize)
	{
		BDBG_WRN(("(SS1SS0)        Sample Size         : %s (%d) was %s (%d)", 
			BAVC_HDMI_AudioInfoFrame_SampleSizeToStr(NewAudioInfoFrame->SampleSize), 
			NewAudioInfoFrame->SampleSize,
			BAVC_HDMI_AudioInfoFrame_SampleSizeToStr(OldAudioInfoFrame->SampleSize),
			OldAudioInfoFrame->SampleSize)) ;
	}
	else
	{
		BDBG_WRN(("(SS1SS0)        Sample Size         : %s (%d)", 
			BAVC_HDMI_AudioInfoFrame_SampleSizeToStr(NewAudioInfoFrame->SampleSize),
			NewAudioInfoFrame->SampleSize)) ;
	}

	
	if (OldAudioInfoFrame->LevelShift != NewAudioInfoFrame->LevelShift)
	{
		BDBG_WRN(("(LSV3_LSV0)     Level Shift Value   : %s (%d) was %s (%d)", 
			BAVC_HDMI_AudioInfoFrame_LevelShiftToStr(NewAudioInfoFrame->LevelShift),
			NewAudioInfoFrame->LevelShift,
			BAVC_HDMI_AudioInfoFrame_LevelShiftToStr(OldAudioInfoFrame->LevelShift),
			OldAudioInfoFrame->LevelShift)) ;
	}
	else
	{	BDBG_WRN(("(LSV3_LSV0)     Level Shift Value   : %s (%d)", 
		BAVC_HDMI_AudioInfoFrame_LevelShiftToStr(NewAudioInfoFrame->LevelShift),
		NewAudioInfoFrame->LevelShift)) ;
	}
	
		
	if (OldAudioInfoFrame->DownMixInhibit != NewAudioInfoFrame->DownMixInhibit)
	{
		BDBG_WRN(("(DM)            Down Mix            : %s (%d) was %s (%d)", 
			BAVC_HDMI_AudioInfoFrame_DownMixInhibitToStr(NewAudioInfoFrame->DownMixInhibit),
			NewAudioInfoFrame->DownMixInhibit,
			BAVC_HDMI_AudioInfoFrame_DownMixInhibitToStr(OldAudioInfoFrame->DownMixInhibit),
			OldAudioInfoFrame->DownMixInhibit)) ;
	}
	else
	{
		BDBG_WRN(("(DM)            Down Mix            : %s (%d)", 
			BAVC_HDMI_AudioInfoFrame_DownMixInhibitToStr(NewAudioInfoFrame->DownMixInhibit),
			NewAudioInfoFrame->DownMixInhibit)) ;
	}
	BDBG_WRN(("--------------------- END AUDIO INFOFRAME  -------------------")) ;
	
}


void BHDR_P_DEBUG_SpdInfoFrame(
	BAVC_HDMI_SPDInfoFrame *OldSpdInfoFrame, 
	BAVC_HDMI_SPDInfoFrame *NewSpdInfoFrame)
{
	BSTD_UNUSED(OldSpdInfoFrame) ;
	
	BDBG_WRN(("-------------------- PARSED SPD INFOFRAME -------------------")) ;
	BDBG_WRN(("Vendor Name: <%s>", NewSpdInfoFrame->VendorName)) ;
	BDBG_WRN(("Description: <%s>",  NewSpdInfoFrame->ProductDescription)) ;
	BDBG_WRN(("Type: %s (%d)", 
		BAVC_HDMI_SpdInfoFrame_SourceTypeToStr(NewSpdInfoFrame->SourceDeviceInfo),
		NewSpdInfoFrame->SourceDeviceInfo)) ;
	
	/* use a marker to separate packet data */
	BDBG_WRN(("--------------------- END SPD INFOFRAME ---------------------")) ;	
}


void BHDR_P_DEBUG_VsInfoFrame(
	BAVC_HDMI_VendorSpecificInfoFrame *OldVsInfoFrame, 
	BAVC_HDMI_VendorSpecificInfoFrame *NewVsInfoFrame)
{
	BSTD_UNUSED(OldVsInfoFrame) ;
	
	BDBG_WRN(("-------------------- PARSED VS INFOFRAME -------------------")) ;
	
	BDBG_WRN(("IEEE RegId: <0x%02x%02X%02X>", 
		NewVsInfoFrame->uIEEE_RegId[2],
		NewVsInfoFrame->uIEEE_RegId[1],
		NewVsInfoFrame->uIEEE_RegId[0])) ;
	
	BDBG_WRN(("HDMI Video Format: <%s> (%d)", 
		BAVC_HDMI_VsInfoFrame_HdmiVideoFormatToStr(
			NewVsInfoFrame->eHdmiVideoFormat),
			NewVsInfoFrame->eHdmiVideoFormat)) ;

	switch (NewVsInfoFrame->eHdmiVideoFormat)
	{
	default :
	case BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eNone :
		break ;
		
	case BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_eExtendedResolution:
		BDBG_WRN(("HDMI VIC: <%s> (%d)", 
			BAVC_HDMI_VsInfoFrame_HdmiVicToStr(NewVsInfoFrame->eHdmiVic),
				NewVsInfoFrame->eHdmiVic)) ;
		break ;
		
	case BAVC_HDMI_VSInfoFrame_HDMIVideoFormat_e3DFormat :
		BDBG_WRN(("3D Struct: <%s> (%d)", 
			BAVC_HDMI_VsInfoFrame_3DStructureToStr(NewVsInfoFrame->e3DStructure),
			NewVsInfoFrame->e3DStructure)) ;
		
		if ((NewVsInfoFrame->e3DStructure <=  BAVC_HDMI_VSInfoFrame_3DStructure_eSidexSideHalf) 
		&& (NewVsInfoFrame->e3DStructure !=  BAVC_HDMI_VSInfoFrame_3DStructure_eReserved))
		{
			BDBG_WRN(("3D Ext Data: <%s> (%d)", 
				BAVC_HDMI_VsInfoFrame_3DExtDataToStr(NewVsInfoFrame->e3DExtData),
				NewVsInfoFrame->e3DExtData)) ;
		}
		else
		{
			BDBG_WRN(("3D Ext Data: %d is not used", NewVsInfoFrame->e3DExtData)) ;
		}
		break ;
		
	}

	/* use a marker to separate packet data */
	BDBG_WRN(("--------------------- END VS INFOFRAME ---------------------")) ;	
}


void BHDR_P_DEBUG_AcrPacket(
	BAVC_HDMI_AudioClockRegenerationPacket *OldACR, 
	BAVC_HDMI_AudioClockRegenerationPacket *NewACR)
{
	BSTD_UNUSED(OldACR) ;
	
	BDBG_WRN(("-------------------- PARSED ACR PACKET -------------------")) ;
	BDBG_WRN(("ACR:       N= %8d        CTS= %8d", NewACR->N, NewACR->CTS)) ;
	
	/* use a marker to separate packet data */
	BDBG_WRN(("--------------------- END ACR PACKET ---------------------")) ;	
}

#if BHDR_CONFIG_DEBUG_TIMER_S
/******************************************************************************
void BHDR_P_DebugMonitorHdmiRx_isr 
Summary: Debug Monitor function to display information at BHDR_CONFIG_DEBUG_TIMER_S 
intervals
*******************************************************************************/
void BHDR_P_DebugMonitorHdmiRx_isr (BHDR_Handle hHDR) 
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;
	
	BDBG_ENTER(BHDR_P_DebugMonitorHdmiRx_isr) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	ulOffset = hHDR->ulOffset ;
	hRegister = hHDR->hRegister ;

		
	Register = BREG_Read32(hRegister, BCHP_HD_DVI_0_V0_COEFF_C01_C00 + ulOffset) ;
	if (hHDR->ulVoCooef_C01_C00 != Register) 
	{
		BDBG_WRN(("HD_DVI_0_V0_COEFF_C01_C00	changed from %x to %x", 
			hHDR->ulVoCooef_C01_C00, Register)) ;
		hHDR->ulVoCooef_C01_C00 = Register;
	}

	Register = BREG_Read32(hRegister, BCHP_HD_DVI_0_INPUT_CNTRL  + ulOffset) ;
	Register = BCHP_GET_FIELD_DATA(Register, HD_DVI_0_INPUT_CNTRL , USE_RGB_TO_YCRCB) ;
	if (hHDR->ulUseRgbToRcbCr != Register) 
	{
		BDBG_WRN(("USE RGB TO YCBCR changed from %x", 
			hHDR->ulUseRgbToRcbCr, Register)) ;
		hHDR->ulUseRgbToRcbCr = (uint8_t) Register ;
	}
	
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_MISC_STATUS + ulOffset) ;
	Register = BCHP_GET_FIELD_DATA(Register, HDMI_RX_0_MISC_STATUS, AV_MUTE) ;
	if (hHDR->ulAvMute != Register) 
	{
		BDBG_WRN(("AV MUTE STATUS changed from %x to %x", 
			hHDR->ulAvMute, Register)) ;
		hHDR->ulAvMute = Register;
	}
	BDBG_LEAVE(BHDR_P_DebugMonitorHdmiRx_isr) ;

}

#endif


/******************************************************************************
Summary: Debug Function to dump Packet Contents at isr time
*******************************************************************************/
BERR_Code BHDR_P_DEBUG_DumpPacketRam_isr(
   BHDR_Handle hHDR, uint8_t PacketNumber, BAVC_HDMI_Packet *Packet 
) 
{
	BREG_Handle hRegister ;	
	uint32_t rc = BERR_SUCCESS ;
	uint32_t Register ;	
	uint32_t RegAddr ;
	uint32_t ulOffset ;
	uint8_t i ;

	BDBG_ENTER(BHDR_P_DEBUG_DumpPacketRam_isr) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset;

	BDBG_WRN(("Packet Register Dump:")) ;
	BDBG_WRN(("")) ;

	RegAddr = BCHP_HDMI_RX_0_RAM_PACKET_0_HEADER  + ulOffset 
		+ PacketNumber * 4 * BHDR_P_PACKET_WORDS  ;
	Register = BREG_Read32_isr(hRegister, RegAddr) ;

	BDBG_WRN(("H [%#x] = 0x%08x | Type: 0x%02x Ver: 0x%02x Len: %2d bytes",
		RegAddr, Register, Packet->Type, Packet->Version, Packet->Length)) ;

	for (i = 0 ; i < BHDR_P_PACKET_WORDS; i++)
	{
		Register = BREG_Read32_isr(hRegister, RegAddr) ;
		BDBG_WRN(("%d [%#x] = 0x%08x", 
			i, RegAddr, Register)) ;

		RegAddr = RegAddr + 4 ;
	}

	/* disable dump for kernel mode */
#if defined LINUX && !defined __KERNEL__	
	BDBG_WRN(("")) ;
	BDBG_WRN(("Packet Data:")) ;

	for (i = 0 ; i < BHDR_P_PACKET_BYTES ; i = i + 4)
	{
		BDBG_WRN(("Bytes %02d..%02d : %02x %02x %02x %02x       %c%c%c%c", 
			i + 3, i,
			Packet->DataBytes[i+3], Packet->DataBytes[i+2], Packet->DataBytes[i+1], Packet->DataBytes[i],
			isprint(Packet->DataBytes[i+3]) ? Packet->DataBytes[i+3] : '.',
			isprint(Packet->DataBytes[i+2]) ? Packet->DataBytes[i+2] : '.',
			isprint(Packet->DataBytes[i+1]) ? Packet->DataBytes[i+1] : '.',
			isprint(Packet->DataBytes[i])    ? Packet->DataBytes[i]   : '.')) ;
	}
#endif 

	BDBG_LEAVE(BHDR_P_DEBUG_DumpPacketRam_isr) ;
	return rc ;
}


void BHDR_DEBUG_P_ConfigureEventCounter(
	BHDR_Handle hHDR, BHDR_DEBUG_P_EventCounter  *pEventCounter) 
{
	BREG_Handle hRegister ;
	uint32_t Register, 
		EventStatsEnable_n0_Addr ,  /* Bits 00..31 */
		EventStatsEnable_n1_Addr ;  /* Bits 32..63 */
	
	uint32_t ulOffset ;

	BDBG_ENTER(BHDR_DEBUG_P_ConfigureEventCounter) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;
	
	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;

	/* configure for Counters for BCH Events */
	Register = BREG_Read32(hHDR->hRegister, BCHP_HDMI_RX_0_EVENT_STATS_CONFIG + ulOffset) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_EVENT_STATS_CONFIG, SELECT_BCH_EVENTS) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_EVENT_STATS_CONFIG, SELECT_BCH_EVENTS, 
		pEventCounter->bBchEvent) ;
	BREG_Write32(hHDR->hRegister,  BCHP_HDMI_RX_0_EVENT_STATS_CONFIG + ulOffset, Register) ;
	
	EventStatsEnable_n0_Addr = 
		BCHP_HDMI_RX_0_EVENT_STATS_ENABLE_00 + pEventCounter->uiCounter * 8 ;
	EventStatsEnable_n1_Addr = 
		BCHP_HDMI_RX_0_EVENT_STATS_ENABLE_01 + pEventCounter->uiCounter * 8 ;
	
	Register = BREG_Read32(hRegister, EventStatsEnable_n0_Addr + ulOffset) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_EVENT_STATS_ENABLE_00, BITS_31_0) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_EVENT_STATS_ENABLE_00, BITS_31_0,
		pEventCounter->ulEventBitMask31_00) ;
	BREG_Write32(hRegister,  EventStatsEnable_n0_Addr+ ulOffset, Register) ;
		
	Register = BREG_Read32(hHDR->hRegister, EventStatsEnable_n1_Addr + ulOffset) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_EVENT_STATS_ENABLE_01, BITS_63_32) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_EVENT_STATS_ENABLE_01, BITS_63_32, 
		pEventCounter->ulEventBitMask63_32) ;
	BREG_Write32(hHDR->hRegister,  EventStatsEnable_n1_Addr + ulOffset, Register) ;

	BDBG_LEAVE(BHDR_DEBUG_P_ConfigureEventCounter) ;
	
}


void BHDR_DEBUG_P_ResetAllEventCounters_isr(BHDR_Handle hHDR)
{
   	BREG_Handle hRegister  ;
	uint32_t Register ;
	uint32_t ulOffset  ;

	BDBG_ENTER(BHDR_DEBUG_P_ResetAllEventCounters_isr) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;
	
	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset;
	
	/* reset all counters */
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_PERT_CONFIG + ulOffset) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_PERT_CONFIG, REGISTER_AND_CLEAR_EVENT_COUNTERS) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_PERT_CONFIG, REGISTER_AND_CLEAR_EVENT_COUNTERS, 1) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_PERT_CONFIG + ulOffset, Register) ;

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_PERT_CONFIG + ulOffset) ;
	Register &= ~BCHP_MASK(HDMI_RX_0_PERT_CONFIG, REGISTER_AND_CLEAR_EVENT_COUNTERS) ;
	Register |= BCHP_FIELD_DATA(HDMI_RX_0_PERT_CONFIG, REGISTER_AND_CLEAR_EVENT_COUNTERS, 0) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_PERT_CONFIG + ulOffset, Register) ;
	
	BDBG_LEAVE(BHDR_DEBUG_P_ResetAllEventCounters_isr) ;
}


#if BDBG_DEBUG_BUILD
#if HDMI_RX_GEN != 7445
BERR_Code BHDR_DEBUG_EnableHdmiRxStandaloneMode(BHDR_Handle hHDR) 
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset   ;

	BDBG_ENTER(BHDR_DEBUG_EnableHdmiRxStandaloneMode) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	ulOffset = hHDR->ulOffset ;
	hRegister = hHDR->hRegister ;
	
	BDBG_WRN(("CH%d HDMI Rx is in standalone mode... video data will not display",
				hHDR->eCoreId)) ;

	Register = BREG_Read32(hRegister, BCHP_DVP_HR_DVP_FLOW_CONTROL + ulOffset) ;
	Register |= BCHP_MASK(DVP_HR_DVP_FLOW_CONTROL, HDMI_RX_0_ALWAYS_ACCEPT) ;
	BREG_Write32(hRegister, BCHP_DVP_HR_DVP_FLOW_CONTROL + ulOffset, Register) ;

	/* ALWAYS ACCEPT should not be set for normal operation */
	Register = BREG_Read32(hRegister, BCHP_DVP_HR_DVP_FLOW_CONTROL + ulOffset) ;
	Register |= BCHP_MASK(DVP_HR_DVP_FLOW_CONTROL, HD_DVI_0_ALWAYS_ACCEPT) ;
	BREG_Write32(hRegister, BCHP_DVP_HR_DVP_FLOW_CONTROL + ulOffset, Register) ;


	BDBG_LEAVE(BHDR_DEBUG_EnableHdmiRxStandaloneMode) ;
	return BERR_SUCCESS ;
}
#endif
#endif


