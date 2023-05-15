/***************************************************************************
 *	   (c)2007-2011 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").	Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.	   This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *	2.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.	   TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: bhdm_edid_3d.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 3/28/12 2:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdm/7038/bhdm_edid_3d.c $
 * 
 * Hydra_Software_Devel/6   3/28/12 2:24p rgreen
 * SW7425-2760: Fix parse of invalid bytes when checking for 3D support
 * 
 * Hydra_Software_Devel/5   11/21/11 6:25p rgreen
 * SW7125-1129: Fix non debug build warning
 * 
 * Hydra_Software_Devel/4   11/21/11 6:00p rgreen
 * SW7125-1129: Fix coverity overrun error
 * 
 * Hydra_Software_Devel/3   10/13/11 3:55p vle
 * SW7125-1110: Merge to mainline - Fix coverity issue
 * 
 * Hydra_Software_Devel/SW7420-2056/2   10/7/11 4:39p vle
 * SW7231-345, SW7125-1110: Fix coverity issue.
 * 
 * Hydra_Software_Devel/2   9/15/11 6:07p vle
 * SW7420-2056: merge to mainline
 * 
 * Hydra_Software_Devel/SW7420-2056/1   9/15/11 3:51p vle
 * SW7420-2056: Fix incorrect Hdmi 3D formats support from parsing EDID
 * 
 * Hydra_Software_Devel/1   6/17/11 1:46p vle
 * SW7405-5358, SW7405-5312: EDID parser for 3D and HDMI 1.4 features
 * 
 ***************************************************************************/

#include "bhdm_priv.h"
#include "bhdm_edid_3d.h"


BDBG_MODULE(BHDM_EDID) ;


#define BHDM_CHECK_RC( rc, func )				  \
do												  \
{												  \
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{											  \
		goto done;								  \
	}											  \
} while(0)


static void BHDM_EDID_P_SetSupportedHdmi3DFormats(BHDM_Handle hHDMI);


/******************************************************************************
Summary:
Return all 3D video formats supported by the attached monitor.
*******************************************************************************/
BERR_Code BHDM_EDID_GetSupported3DFormats(
	BHDM_Handle hHDMI,					 /* [in] HDMI handle  */
	BHDM_EDID_3D_Structure_ALL *_3DFormats					 /* [out] supported true/false */
)
{
	BERR_Code rc = BERR_SUCCESS ;

	BDBG_ENTER(BHDM_EDID_GetSupported3DFormats) ;

	BKNI_Memset(_3DFormats, 0, sizeof(BHDM_EDID_3D_Structure_ALL)) ;

	/* Need HDMI for 3D Support */
	if (!hHDMI->AttachedEDID.RxHasHdmiSupport)		
		goto done ;
 
	if (hHDMI->edidStatus == BHDM_EDID_STATE_eInvalid) {
		rc = BHDM_EDID_NOT_FOUND ;
		goto done ;
	}

	if (hHDMI->AttachedEDID.Bcm3DFormatsChecked == 0)
	{
		rc = BHDM_EDID_VIDEO_FORMATS_UNAVAILABLE ;
		goto done ;
	}

	BKNI_Memcpy(_3DFormats, &hHDMI->AttachedEDID.BcmSupported3DFormats,
		sizeof(hHDMI->AttachedEDID.BcmSupported3DFormats)) ;

done:
	BDBG_LEAVE(BHDM_EDID_GetSupported3DFormats) ;
	return rc ;

} /* BHDM_EDID_GetSupported3DFormats */


static void BHDM_EDID_P_ParseVsdb3dStruct(BHDM_Handle hHDMI, uint8_t DataBlockIndex, uint8_t *index)
{
	hHDMI->AttachedEDID.BcmSupported3DStructureAll =
		  (hHDMI->AttachedEDID.Block[DataBlockIndex + *index] << 8)
		| hHDMI->AttachedEDID.Block[DataBlockIndex + *index + 1] ;
	BDBG_WRN(("3D_Structure_ALL: %#04x located at offset %#02x (%d)",
		hHDMI->AttachedEDID.BcmSupported3DStructureAll, DataBlockIndex + *index, DataBlockIndex + *index)) ;
	*index = *index + 2 ;

#if BHDM_CONFIG_DEBUG_EDID_3DSTRUCT
	if (hHDMI->AttachedEDID.BcmSupported3DStructureAll & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_FRAME_PACKING) {
		BDBG_WRN(("Rx Supports 3D FramePacking")) ;
	}

	if (hHDMI->AttachedEDID.BcmSupported3DStructureAll & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_FIELD_ALTERNATIVE) {
		BDBG_WRN(("Rx Supports 3D Field Alternative")) ;
	}

	if (hHDMI->AttachedEDID.BcmSupported3DStructureAll & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_LINE_ALTERNATIVE) {
		BDBG_WRN(("Rx Supports 3D Line Alternative")) ;
	}

	if (hHDMI->AttachedEDID.BcmSupported3DStructureAll & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_SBS_FULL) {
		BDBG_WRN(("Rx Supports 3D Side by Side Full")) ;
	}

	if (hHDMI->AttachedEDID.BcmSupported3DStructureAll & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_LDEPTH) {
		BDBG_WRN(("Rx Supports 3D Line Depth")) ;
	}

	if (hHDMI->AttachedEDID.BcmSupported3DStructureAll & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_LDEPTH_GFX) {
		BDBG_WRN(("Rx Supports 3D LDepth + Graphics")) ;
	}

	if (hHDMI->AttachedEDID.BcmSupported3DStructureAll & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_TOP_BOTTOM) {
		BDBG_WRN(("Rx Supports 3D Top and Bottom")) ;
	}

	if (hHDMI->AttachedEDID.BcmSupported3DStructureAll & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_SBS_HALF_HORIZ) {
		BDBG_WRN(("Rx Supports 3D Side by Side Half")) ;
	}
#endif

	return;
}


static void BHDM_EDID_P_ParseVsdb3dMask(BHDM_Handle hHDMI, uint8_t DataBlockIndex, uint8_t *index)
{
	uint16_t Edid3DMask ;
	uint16_t i;
	BHDM_EDID_P_VideoDescriptor  *pVideoDescriptor ;

	Edid3DMask =
		  (hHDMI->AttachedEDID.Block[DataBlockIndex + *index] << 8)
		| hHDMI->AttachedEDID.Block[DataBlockIndex + *index + 1] ;
	BDBG_WRN(("3D_Structure_MASK: %#04x located at offset %#02x (%d)",
		Edid3DMask, DataBlockIndex + *index, DataBlockIndex + *index)) ;
	*index = *index + 2 ;

#if BHDM_CONFIG_DEBUG_EDID_3DMASK
	if (Edid3DMask)
		BDBG_WRN(("Rx Supports 3D_Structure_ALL for the following formats:"));
#endif
	
	pVideoDescriptor = BLST_Q_FIRST(&hHDMI->AttachedEDID.VideoDescriptorList);
	for (i=0; i<16 && pVideoDescriptor; i++)
	{
		if (hHDMI->AttachedEDID.First16VideoDescriptorsMask & (1 << i))
		{
			if (Edid3DMask & (1 << i)) 
			{
				hHDMI->AttachedEDID.BcmSupported3DFormats[(BFMT_VideoFmt) pVideoDescriptor->eVideoFmt] |=
					hHDMI->AttachedEDID.BcmSupported3DStructureAll;
				
#if BHDM_CONFIG_DEBUG_EDID_3DMASK
				BDBG_WRN((" 	 %s",
					(char *) BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr((uint8_t) pVideoDescriptor->VideoIdCode)));
#endif
			}
			
			pVideoDescriptor = BLST_Q_NEXT(pVideoDescriptor, link);
		}
	}

	return;
}


void BHDM_EDID_P_ParseVSDB3D(BHDM_Handle hHDMI, uint8_t DataBlockIndex, uint8_t *offset, uint8_t DataBlockLength)
{
	uint8_t i;
	uint8_t offsetByte;
	uint8_t remaining3dFmtBytes;
	uint8_t _2dVicOrderX, _3dStructureX, _3dDetailX=0x0F;
	BHDM_EDID_P_VideoDescriptor  *pVideoDescriptor ;


	/* check for HDMI VICs and 3D support info */
	offsetByte = hHDMI->AttachedEDID.Block[DataBlockIndex + *offset] ;
	(*offset)++;


	/* retrieve 3D_present, 3D_Multi_present fields */
	hHDMI->AttachedEDID.RxVSDB.HDMI_3D_Present = (offsetByte & 0x80) >> 7 ; 
	hHDMI->AttachedEDID.RxVSDB.HDMI_3D_Multi_Present = (offsetByte & 0x60) >> 5 ;
	hHDMI->AttachedEDID.RxVSDB.HDMI_Image_Size = (offsetByte & 0x18) >> 3 ;

	BDBG_MSG(("HDMI_3D_Present=%d, HDMI_3D_Multi_Present=%d, HDMI_Image_Size=%d", 
		hHDMI->AttachedEDID.RxVSDB.HDMI_3D_Present, hHDMI->AttachedEDID.RxVSDB.HDMI_3D_Multi_Present,
		hHDMI->AttachedEDID.RxVSDB.HDMI_Image_Size));

	/************************
	 * Rx has 3D support. 
	 * It must support all the mandatory formats below
	 *************************/
	if (hHDMI->AttachedEDID.RxVSDB.HDMI_3D_Present)
	{
		hHDMI->AttachedEDID.BcmSupported3DFormats[BFMT_VideoFmt_e1080p_24Hz] =
			  BHDM_EDID_VSDB_3D_STRUCTURE_ALL_FRAME_PACKING
			| BHDM_EDID_VSDB_3D_STRUCTURE_ALL_TOP_BOTTOM ;


		if ((50 >= hHDMI->AttachedEDID.MonitorRange.MinVertical) &&
			(50 <= hHDMI->AttachedEDID.MonitorRange.MaxVertical))
		{
			hHDMI->AttachedEDID.BcmSupported3DFormats[BFMT_VideoFmt_e720p_50Hz] =
			  BHDM_EDID_VSDB_3D_STRUCTURE_ALL_FRAME_PACKING
			| BHDM_EDID_VSDB_3D_STRUCTURE_ALL_TOP_BOTTOM ;

			hHDMI->AttachedEDID.BcmSupported3DFormats[BFMT_VideoFmt_e1080i_50Hz] =
				BHDM_EDID_VSDB_3D_STRUCTURE_ALL_SBS_HALF_HORIZ ;
		}

		if ((59 >= hHDMI->AttachedEDID.MonitorRange.MinVertical) &&
			(59 <= hHDMI->AttachedEDID.MonitorRange.MaxVertical))
		{
			hHDMI->AttachedEDID.BcmSupported3DFormats[BFMT_VideoFmt_e720p] =
				  BHDM_EDID_VSDB_3D_STRUCTURE_ALL_FRAME_PACKING
				| BHDM_EDID_VSDB_3D_STRUCTURE_ALL_TOP_BOTTOM ;
			hHDMI->AttachedEDID.BcmSupported3DFormats[BFMT_VideoFmt_e1080i] =
				BHDM_EDID_VSDB_3D_STRUCTURE_ALL_SBS_HALF_HORIZ ;
		}
	}


	offsetByte = hHDMI->AttachedEDID.Block[DataBlockIndex + *offset] ;
	(*offset)++;

	hHDMI->AttachedEDID.RxVSDB.HDMI_VIC_Len = (offsetByte & 0xE0) >> 5 ;
	hHDMI->AttachedEDID.RxVSDB.HDMI_3D_Len = (offsetByte & 0x1F) ;
	remaining3dFmtBytes = hHDMI->AttachedEDID.RxVSDB.HDMI_3D_Len;

	/* finished parsing 3D_present, 3D_Multi_present, HDMI_VIC_LEN, and HDMI_3D_LEN */
	if (*offset > DataBlockLength)
		goto done ;  /* no more optional data */


	/****** 
	  TODO HDMI Extended Resolutions are currently not supported 
	*******/
	for (i = 0 ; i < hHDMI->AttachedEDID.RxVSDB.HDMI_VIC_Len; i++)
	{
		uint8_t hdmiVic ;

		hdmiVic = hHDMI->AttachedEDID.Block[DataBlockIndex + *offset] ;
		(*offset)++ ;
		BDBG_WRN(("No Support for HDMI VIC %d", hdmiVic)) ;
	}


	/******** 
	Parse 3D Timing/Structure 
	
	if 3D_Multi_Present = 00
		3D_Structure_ALL_15...0 and
		3D_MASK_15..0 are not present
	
	if 3D_Multi_Present = 01
		3D_Structure_ALL_15...0 is present and assigns 3D formats to all the VICs listed in the first 16 entires in the EDID
		3D_MASK_15..0 is not present
	
	if 3D_Multi_Present = 10
		3D_Structure_ALL_15...0 and
		3D_MASK_15..0 are present and assign 3D formats to some of the VICs listed in the first 16 entires in the EDID
	
	if 3D_Multi_Present = 11
		Reserved for future use
		3D_Structure_ALL_15...0 and
		3D_MASK_15..0 are not present
	********/
#define BHDM_EDID_P_VSDB_3D_MULTI_NONE 0x0
#define BHDM_EDID_P_VSDB_3D_MULTI_STRUCT 0x1
#define BHDM_EDID_P_VSDB_3D_MULTI_STRUCT_AND_MASK 0x2
#define BHDM_EDID_P_VSDB_3D_MULTI_RESERVED 0x3

	switch(hHDMI->AttachedEDID.RxVSDB.HDMI_3D_Multi_Present)
	{
	case BHDM_EDID_P_VSDB_3D_MULTI_STRUCT :
		BHDM_EDID_P_ParseVsdb3dStruct(hHDMI, DataBlockIndex, offset) ;
		remaining3dFmtBytes -= 2;

#if BHDM_CONFIG_DEBUG_EDID_3DSTRUCT
		BDBG_WRN(("All 3D Structures are supported on the first 16th Short Video Descriptors"));
#endif
		/* Set 3D Structure support for ALL of the first 16 short video descriptors */
		pVideoDescriptor = BLST_Q_FIRST(&hHDMI->AttachedEDID.VideoDescriptorList);
		for (i=0; i<16 && pVideoDescriptor; i++)
		{
			if (hHDMI->AttachedEDID.First16VideoDescriptorsMask & (1 << i))
			{
				hHDMI->AttachedEDID.BcmSupported3DFormats[(BFMT_VideoFmt) pVideoDescriptor->eVideoFmt] |=
					hHDMI->AttachedEDID.BcmSupported3DStructureAll;
				pVideoDescriptor = BLST_Q_NEXT(pVideoDescriptor, link);
			}
		}
		break ;

	case BHDM_EDID_P_VSDB_3D_MULTI_STRUCT_AND_MASK :
		BHDM_EDID_P_ParseVsdb3dStruct(hHDMI, DataBlockIndex, offset) ;
		BHDM_EDID_P_ParseVsdb3dMask(hHDMI, DataBlockIndex, offset) ;
		remaining3dFmtBytes -= 4;
		break;

	case BHDM_EDID_P_VSDB_3D_MULTI_NONE :
		/* do nothing */
		break ;

	default :
	case BHDM_EDID_P_VSDB_3D_MULTI_RESERVED :
		BDBG_WRN(("Use of 3D_Multi_Present Reserved Value %#x; No 3D formats detected",
			hHDMI->AttachedEDID.RxVSDB.HDMI_3D_Multi_Present)) ;
		break ;
	}


	/* Parse any remaining 3D Timing/Structure Support (bytes) and indicate support if applicable */
	while (remaining3dFmtBytes > 0)
	{
		offsetByte = hHDMI->AttachedEDID.Block[DataBlockIndex + *offset] ;
		(*offset)++ ;
		remaining3dFmtBytes-- ;

		_2dVicOrderX = (offsetByte & 0xF0) >> 4 ;
		_3dStructureX = (offsetByte & 0x0F);
		if (_3dStructureX == BHDM_EDID_VSDB_3D_STRUCTURE_X_SBS_HALF)
		{
			offsetByte = hHDMI->AttachedEDID.Block[DataBlockIndex + *offset] ;
			(*offset)++;
			_3dDetailX = (offsetByte & 0xF0) >> 4 ;
			remaining3dFmtBytes--;
		}

		/* Set the supported 3D Structure for the specific 2D video format */
		pVideoDescriptor = BLST_Q_FIRST(&hHDMI->AttachedEDID.VideoDescriptorList);
		for (i=0; i<16 && pVideoDescriptor; i++)
		{
			if (!(hHDMI->AttachedEDID.First16VideoDescriptorsMask & (1 << i)))
				continue;
				
			if (i != _2dVicOrderX)
				goto next_format;

			switch (_3dStructureX)
			{
			case BHDM_EDID_VSDB_3D_STRUCTURE_X_FRAME_PACKING:
				hHDMI->AttachedEDID.BcmSupported3DFormats[(BFMT_VideoFmt) pVideoDescriptor->eVideoFmt] |=
					BHDM_EDID_VSDB_3D_STRUCTURE_ALL_FRAME_PACKING;
				BDBG_MSG(("Rx supports 3D Frame Packing for %s format",
					(char *) BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr((uint8_t) pVideoDescriptor->VideoIdCode)));
				break;

			case BHDM_EDID_VSDB_3D_STRUCTURE_X_FIELD_ALTERNATIVE:
				hHDMI->AttachedEDID.BcmSupported3DFormats[(BFMT_VideoFmt) pVideoDescriptor->eVideoFmt] |=
					BHDM_EDID_VSDB_3D_STRUCTURE_ALL_FIELD_ALTERNATIVE;
				BDBG_MSG(("Rx supports 3D Field Alternative for %s format",
					(char *) BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr((uint8_t) pVideoDescriptor->VideoIdCode)));
				break;

			case BHDM_EDID_VSDB_3D_STRUCTURE_X_LINE_ALTERNATIVE:
				hHDMI->AttachedEDID.BcmSupported3DFormats[(BFMT_VideoFmt) pVideoDescriptor->eVideoFmt] |=
					BHDM_EDID_VSDB_3D_STRUCTURE_ALL_LINE_ALTERNATIVE;
				BDBG_MSG(("Rx supports 3D Line Alternative for %s format",
					(char *) BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr((uint8_t) pVideoDescriptor->VideoIdCode)));
				break;

			case BHDM_EDID_VSDB_3D_STRUCTURE_X_SBS_FULL:
				hHDMI->AttachedEDID.BcmSupported3DFormats[(BFMT_VideoFmt) pVideoDescriptor->eVideoFmt] |=
					BHDM_EDID_VSDB_3D_STRUCTURE_ALL_SBS_FULL;
				BDBG_MSG(("Rx supports 3D SidebySide Full for %s format",
					(char *) BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr((uint8_t) pVideoDescriptor->VideoIdCode)));
				break;

			case BHDM_EDID_VSDB_3D_STRUCTURE_X_LDEPTH:
				hHDMI->AttachedEDID.BcmSupported3DFormats[(BFMT_VideoFmt) pVideoDescriptor->eVideoFmt] |=
					BHDM_EDID_VSDB_3D_STRUCTURE_ALL_LDEPTH;
				BDBG_MSG(("Rx supports 3D LDepth for %s format",
					(char *) BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr((uint8_t) pVideoDescriptor->VideoIdCode)));
				break;

			case BHDM_EDID_VSDB_3D_STRUCTURE_X_LDEPTH_GFX:
				hHDMI->AttachedEDID.BcmSupported3DFormats[(BFMT_VideoFmt) pVideoDescriptor->eVideoFmt] |=
					BHDM_EDID_VSDB_3D_STRUCTURE_ALL_LDEPTH_GFX;
				BDBG_MSG(("Rx supports 3D LDepth GFX for %s format",
					(char *) BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr((uint8_t) pVideoDescriptor->VideoIdCode)));
				break;

			case BHDM_EDID_VSDB_3D_STRUCTURE_X_TOP_BOTTOM:
				hHDMI->AttachedEDID.BcmSupported3DFormats[(BFMT_VideoFmt) pVideoDescriptor->eVideoFmt] |=
					BHDM_EDID_VSDB_3D_STRUCTURE_ALL_TOP_BOTTOM;
				BDBG_MSG(("Rx supports 3D TopAndBottom for %s format",
					(char *) BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr((uint8_t) pVideoDescriptor->VideoIdCode)));
				break;

			case BHDM_EDID_VSDB_3D_STRUCTURE_X_SBS_HALF:
				if (_3dDetailX <= BHDM_EDID_VSDB_3D_EXT_DATA_HORIZ_SUB_SAMPLING) {
					hHDMI->AttachedEDID.BcmSupported3DFormats[(BFMT_VideoFmt) pVideoDescriptor->eVideoFmt] |=
						BHDM_EDID_VSDB_3D_STRUCTURE_ALL_SBS_HALF_HORIZ;
					BDBG_MSG(("Rx supports 3D SidexSide Half with Horizontal sub-sampling for %s format",
						(char *) BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr((uint8_t) pVideoDescriptor->VideoIdCode)));
				}
				else if (_3dDetailX <= BHDM_EDID_VSDB_3D_EXT_DATA_QUINCUNX_MATRIX) {
					hHDMI->AttachedEDID.BcmSupported3DFormats[(BFMT_VideoFmt) pVideoDescriptor->eVideoFmt] |=
						BHDM_EDID_VSDB_3D_STRUCTURE_ALL_SBS_HALF_QUINC;
					BDBG_MSG(("Rx supports 3D SidexSide Half Quincunx matrix for %s format",
						(char *) BAVC_HDMI_AviInfoFrame_VideoIdCodeToStr((uint8_t) pVideoDescriptor->VideoIdCode)));
				}
				break;

			case BHDM_EDID_VSDB_3D_STRUCTURE_X_RESERVED:
			default:
				BDBG_MSG(("RESERVED 3D_Structure_X"));
				break;
			}

			/* Already found the corresponding 2D_VIC and set the supported 3D format. 
			Stop searching and move on to the next 2D_VIC, break out of the for loop. */
			break;

next_format:				
			/* Move to next supported format in the list */
			pVideoDescriptor = BLST_Q_NEXT(pVideoDescriptor, link); 			
			
		}
	}

	/* Check HDMI 3D formats for supported */
	BHDM_EDID_P_SetSupportedHdmi3DFormats(hHDMI);

done:	
	
	hHDMI->AttachedEDID.Bcm3DFormatsChecked = true;
	return;
}


typedef struct _BHDM_EDID_HDMI_3D_FORMAT_
{
	BFMT_VideoFmt eVideoFmt;
} BHDM_EDID_HDMI_3D_FORMAT;

static const BHDM_EDID_HDMI_3D_FORMAT BHDM_EDID_3DFormats[]=
{
	{	BFMT_VideoFmt_e720p_60Hz_3DOU_AS	},		   /* HD 3D 720p */
	{	BFMT_VideoFmt_e720p_50Hz_3DOU_AS	},		   /* HD 3D 720p50 */
	{	BFMT_VideoFmt_e720p_30Hz_3DOU_AS	},		   /* HD 3D 720p30 */
	{	BFMT_VideoFmt_e720p_24Hz_3DOU_AS	},		   /* HD 3D 720p24 */
	{	BFMT_VideoFmt_e1080p_24Hz_3DOU_AS	},		   /* HD 1080p 24Hz, 2750 sample per line, SMPTE 274M-1998 */
	{	BFMT_VideoFmt_e1080p_30Hz_3DOU_AS	}		   /* HD 1080p 30Hz, 2200 sample per line, SMPTE 274M-1998 */
};

static void BHDM_EDID_P_SetSupportedHdmi3DFormats(BHDM_Handle hHDMI)
{
	BFMT_VideoFmt eVideoFmt = 0 ;
#if BHDM_CONFIG_DEBUG_EDID_3DSTRUCT
	BFMT_VideoInfo	 stVideoFmtInfo ;
#endif

	uint8_t j=0;
	uint8_t NumBcmSupported3DFormats ;
	bool OverUnderSupport  ;

	NumBcmSupported3DFormats = 
		sizeof(BHDM_EDID_3DFormats) / sizeof(BHDM_EDID_3DFormats[0]) ;

	for (eVideoFmt = 0 ; eVideoFmt < BFMT_VideoFmt_eMaxCount ; eVideoFmt++)
	{
		/* if format is not supported, continue  */
		if (!hHDMI->AttachedEDID.BcmSupportedVideoFormats[eVideoFmt])
			continue;
		
		/* if frame packing not supported, continue */
		if (!(hHDMI->AttachedEDID.BcmSupported3DFormats[eVideoFmt] & BHDM_EDID_VSDB_3D_STRUCTURE_ALL_FRAME_PACKING))
			continue ;

		OverUnderSupport = false ;

		switch (eVideoFmt)
		{
		case BFMT_VideoFmt_e1080p_24Hz:
			for (j=0 ; j < NumBcmSupported3DFormats ; j++)	
				if (BHDM_EDID_3DFormats[j].eVideoFmt == BFMT_VideoFmt_e1080p_24Hz_3DOU_AS)	
				{
					OverUnderSupport = true ;
					hHDMI->AttachedEDID.BcmSupportedVideoFormats[BFMT_VideoFmt_e1080p_24Hz_3DOU_AS] = true;
					break;
				}		
			break;
		
		case BFMT_VideoFmt_e1080p_30Hz:
			for (j=0 ; j < NumBcmSupported3DFormats ; j++)	
				if (BHDM_EDID_3DFormats[j].eVideoFmt == BFMT_VideoFmt_e1080p_30Hz_3DOU_AS)	
				{
					OverUnderSupport = true ;
					hHDMI->AttachedEDID.BcmSupportedVideoFormats[BFMT_VideoFmt_e1080p_30Hz_3DOU_AS] = true;
					break;
				}		
			break;

		case BFMT_VideoFmt_e720p:
			for (j=0 ; j < NumBcmSupported3DFormats ; j++)
				if (BHDM_EDID_3DFormats[j].eVideoFmt == BFMT_VideoFmt_e720p_60Hz_3DOU_AS)	
				{
					OverUnderSupport = true ;
					hHDMI->AttachedEDID.BcmSupportedVideoFormats[BFMT_VideoFmt_e720p_60Hz_3DOU_AS] = true;
					break;
				}		
			break;
			
		case BFMT_VideoFmt_e720p_50Hz:
			for (j=0 ; j < NumBcmSupported3DFormats ; j++)	
				if (BHDM_EDID_3DFormats[j].eVideoFmt == BFMT_VideoFmt_e720p_50Hz_3DOU_AS)	
				{
					OverUnderSupport = true ;
					hHDMI->AttachedEDID.BcmSupportedVideoFormats[BFMT_VideoFmt_e720p_50Hz_3DOU_AS] = true;
					break;
				}		
			break;
			
		case BFMT_VideoFmt_e720p_24Hz:
			for (j=0 ; j < NumBcmSupported3DFormats ; j++)
				if (BHDM_EDID_3DFormats[j].eVideoFmt == BFMT_VideoFmt_e720p_24Hz_3DOU_AS)	
				{
					OverUnderSupport = true ;
					hHDMI->AttachedEDID.BcmSupportedVideoFormats[BFMT_VideoFmt_e720p_24Hz_3DOU_AS] = true;
					break;
				}		
			break;
			
		case BFMT_VideoFmt_e720p_30Hz:
			for (j=0 ; NumBcmSupported3DFormats ; j++)
				if (BHDM_EDID_3DFormats[j].eVideoFmt == BFMT_VideoFmt_e720p_30Hz_3DOU_AS)	
				{
					OverUnderSupport = true ;
					hHDMI->AttachedEDID.BcmSupportedVideoFormats[BFMT_VideoFmt_e720p_30Hz_3DOU_AS] = true;
					break;
				}		
			break;
			
		default:
			break;			
		}/* end switch */

#if BHDM_CONFIG_DEBUG_EDID_3DSTRUCT
		if (OverUnderSupport)
		{
			/* there is a separate FMT enumeration for OU formats; just print corresponding 2D format */
			BFMT_GetVideoFormatInfo(eVideoFmt, &stVideoFmtInfo) ;
			BDBG_WRN(("3D Top/Bottom support found for %s", stVideoFmtInfo.pchFormatStr)) ;
		}
#endif
	}
	
	return;
}


