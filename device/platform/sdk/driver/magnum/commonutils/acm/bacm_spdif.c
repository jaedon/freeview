/***************************************************************************
*     Copyright (c) 2003-2007, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bacm_spdif.c $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 3/15/07 3:17p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/commonutils/acm/bacm_spdif.c $
* 
* Hydra_Software_Devel/1   3/15/07 3:17p rgreen
* PR27181: Initial audio common support for SPDIF Channel Status
* types/functions
* 
* Hydra_Software_Devel/2   12/22/05 6:39p bandrews
* PR18809: Cleaned up compiler warnings
* 
* Hydra_Software_Devel/1   9/22/05 9:42p bandrews
* PR17068: Refactored common spdif stuff.  Improved status reporting.
***************************************************************************/

#include "bacm_spdif.h"


BDBG_MODULE(BACM) ;


void BACM_SPDIF_InitChannelStatus(BACM_SPDIF_ChannelStatus * psChannelStatus)
{
	if (psChannelStatus)
	{
		psChannelStatus->eApplication = BACM_SPDIF_Application_eConsumer;
		psChannelStatus->eStreamType = BACM_SPDIF_StreamType_eLinear;
		psChannelStatus->bCopyrighted = false;
		psChannelStatus->uAdditionalFormatInfo.ePCM = BACM_SPDIF_PCM_AdditionalFormatInfo_2ChannelNoPreEmphasis;
		psChannelStatus->eChannelStatusMode = BACM_SPDIF_ChannelStatusMode_e0;
		psChannelStatus->uiCategoryCode = 0x0;
		psChannelStatus->eSourceNumber = BACM_SPDIF_SourceNumber_eUnspecified;
		psChannelStatus->eChannelNumber = BACM_SPDIF_ChannelNumber_eUnspecified;
		psChannelStatus->eSamplingFrequency = BACM_SPDIF_SamplingFrequency_eUnspecified;
		psChannelStatus->eClockAccuracy = BACM_SPDIF_ClockAccuracy_eLevel2;
		psChannelStatus->eWordLength = BACM_SPDIF_WordLength_eUnspecifiedOf24;
		psChannelStatus->eOriginalSamplingFrequency = BACM_SPDIF_OriginalSamplingFrequency_eUnspecified;
	}
}


void BACM_SPDIF_ParseChannelStatus(BACM_SPDIF_ChannelStatus * psChannelStatus, uint8_t uiChannelStatusSize, uint8_t auiChannelStatus[])
{
	if (psChannelStatus)
	{
		if (uiChannelStatusSize > 0)
		{
			psChannelStatus->eApplication = 
				BACM_SPDIF_GET_FIELD_DATA(auiChannelStatus[0], CHAN_STAT0, APPLICATION);				
			psChannelStatus->eStreamType = 
				BACM_SPDIF_GET_FIELD_DATA(auiChannelStatus[0], CHAN_STAT0, STREAM_TYPE);
			psChannelStatus->bCopyrighted = 
				BACM_SPDIF_GET_FIELD_DATA(auiChannelStatus[0], CHAN_STAT0, COPYRIGHT);

			if (psChannelStatus->eStreamType == BACM_SPDIF_StreamType_eLinear)
			{
				psChannelStatus->uAdditionalFormatInfo.ePCM = 
					BACM_SPDIF_GET_FIELD_DATA(auiChannelStatus[0], CHAN_STAT0, ADDED_FORMAT_INFO);
			}
			else
			{
				psChannelStatus->uAdditionalFormatInfo.eNonlinear = 
					BACM_SPDIF_GET_FIELD_DATA(auiChannelStatus[0], CHAN_STAT0, ADDED_FORMAT_INFO);
			}
			
			psChannelStatus->eChannelStatusMode = 
				BACM_SPDIF_GET_FIELD_DATA(auiChannelStatus[0], CHAN_STAT0, MODE);
		}
		if (uiChannelStatusSize > 1)
		{
			psChannelStatus->uiCategoryCode = 
				BACM_SPDIF_GET_FIELD_DATA(auiChannelStatus[1], CHAN_STAT1, CATEGORY_CODE);
		}
		if (uiChannelStatusSize > 2)
		{
			psChannelStatus->eSourceNumber = 
				BACM_SPDIF_GET_FIELD_DATA(auiChannelStatus[2], CHAN_STAT2, SOURCE_NUM);
			psChannelStatus->eChannelNumber = 
				BACM_SPDIF_GET_FIELD_DATA(auiChannelStatus[2], CHAN_STAT2, CHAN_NUM);
		}
		if (uiChannelStatusSize > 3)
		{
			psChannelStatus->eSamplingFrequency = 
				BACM_SPDIF_GET_FIELD_DATA(auiChannelStatus[3], CHAN_STAT3, FS);
			psChannelStatus->eClockAccuracy = 
				BACM_SPDIF_GET_FIELD_DATA(auiChannelStatus[3], CHAN_STAT3, CLK_ACCURACY);
		}
		if (uiChannelStatusSize > 4)
		{
			psChannelStatus->eWordLength = 
				BACM_SPDIF_GET_FIELD_DATA(auiChannelStatus[4], CHAN_STAT4, WORD_LEN);
			psChannelStatus->eOriginalSamplingFrequency = 
				BACM_SPDIF_GET_FIELD_DATA(auiChannelStatus[4], CHAN_STAT4, ORIG_FS);
		}

	}
}


void BACM_SPDIF_ParsePreambleC(BACM_SPDIF_PreambleC * psPreambleC, uint16_t uiPreambleC)
{
	if (psPreambleC)
	{
		psPreambleC->eDataType = 
			BACM_SPDIF_GET_FIELD_DATA(uiPreambleC, PC, DATA_TYPE);
			
		psPreambleC->bPayloadMayContainErrors = 
			BACM_SPDIF_GET_FIELD_DATA(uiPreambleC, PC, PAYLOAD_ERRORS);
			
		psPreambleC->uiDataTypeDependentInfo = 
			BACM_SPDIF_GET_FIELD_DATA(uiPreambleC, PC, TYPE_DEP_INFO);
			
		psPreambleC->uiBitstreamNumber = 
			BACM_SPDIF_GET_FIELD_DATA(uiPreambleC, PC, BS_NUM);
	}
}

