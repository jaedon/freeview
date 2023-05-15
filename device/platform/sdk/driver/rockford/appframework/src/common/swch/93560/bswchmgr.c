/***************************************************************************
*     Copyright (c) 2003-2005, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bswchmgr.c $
* $brcm_Revision: Hydra_Software_Devel/6 $
* $brcm_Date: 9/19/05 4:57p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/93560/linuxuser/bswchmgr.c $
* 
* Hydra_Software_Devel/6   9/19/05 4:57p darnstein
* PR16885: Only compile on 93560.
* 
* Hydra_Software_Devel/5   9/13/05 4:07p darnstein
* PR16885: Remove reference to unnecessary header files.
* 
* Hydra_Software_Devel/4   9/13/05 2:10p darnstein
* PR16885: Improve #ifdefs for chip type.
* 
* Hydra_Software_Devel/3   9/12/05 7:44p darnstein
* PR16885: Remove unused lines of code.
* 
* Hydra_Software_Devel/2   9/12/05 7:18p darnstein
* PR16885: BSWCH support for simple programs.
* 
* Hydra_Software_Devel/1   9/8/05 5:26p darnstein
* PR16885: BSWCH support for simple programs.
* 
***************************************************************************/

#if (BCHP_CHIP==3560) /** { **/

#include "bstd.h"
#include "bswchmgr.h"

#include "bswch.h"
static BSWCH_Handle s_hSwch = 0;


BERR_Code
bswchmgr_set (BFramework_Info* pFrmInfo, Bswchmgr_format bsw_format, 
	BAVC_SourceId sourceId)
{
	BERR_Code rc;
	BSWCH_OutputSettings swchOutputSetting;
	bool autodetect = false;

	if (s_hSwch == 0)
	{
		if ((rc = BSWCH_Open (&s_hSwch, pFrmInfo->ahI2cRegHandles[3], 0)) !=
			BERR_SUCCESS)
		{
			s_hSwch = 0;
			return rc;
		}
	}

	switch (bsw_format) {
	default:
	case Bswchmgr_format_cvbs:
		rc = BSWCH_GetOutput( s_hSwch, 
			(sourceId == BAVC_SourceId_eVdec0 ? 
				BSWCH_OutputNum_e0: BSWCH_OutputNum_e1),
			&swchOutputSetting  );
		if (rc!=BERR_SUCCESS) { return rc; }
		swchOutputSetting.type = BSWCH_InputType_eComposite;
		swchOutputSetting.videoSource =	swchOutputSetting.audioSource =	
			(sourceId == BAVC_SourceId_eVdec0 ? 
				BSWCH_InputSource_eInput1 : BSWCH_InputSource_eInput2);
		rc = BSWCH_SetOutput( s_hSwch, 
			(sourceId == BAVC_SourceId_eVdec0 ?
				BSWCH_OutputNum_e0: BSWCH_OutputNum_e1),
			&swchOutputSetting );
		if (rc!=BERR_SUCCESS) { return rc; }
		break;

	case Bswchmgr_format_svideo:
		rc = BSWCH_GetOutput( s_hSwch, 
			(sourceId == BAVC_SourceId_eVdec0 ?
				BSWCH_OutputNum_e0: BSWCH_OutputNum_e1),
			&swchOutputSetting  );
		/* TODO: Not tested */
		if (rc!=BERR_SUCCESS) { return rc; }
		swchOutputSetting.type = BSWCH_InputType_eSVideo;
		swchOutputSetting.videoSource =	swchOutputSetting.audioSource = 
			(sourceId == BAVC_SourceId_eVdec0 ? 
				BSWCH_InputSource_eInput1 : BSWCH_InputSource_eInput2);
		rc = BSWCH_SetOutput( s_hSwch, 
			(sourceId == BAVC_SourceId_eVdec0 ?
				BSWCH_OutputNum_e0: BSWCH_OutputNum_e1),
			&swchOutputSetting );
		rc = BSWCH_SetOutput( s_hSwch, (0 == 0 ?
			BSWCH_OutputNum_e0: BSWCH_OutputNum_e1), &swchOutputSetting );
		if (rc!=BERR_SUCCESS) { return rc; }
		break;
	case Bswchmgr_format_component:
		autodetect = true;
		rc = BSWCH_GetOutput( s_hSwch, BSWCH_OutputNum_e0, &swchOutputSetting  );
		if (rc!=BERR_SUCCESS) { return rc; }
		/* Only need to set the audio, 
		   but video must initialized to something */
		swchOutputSetting.type = BSWCH_InputType_eComposite;
		swchOutputSetting.videoSource =	BSWCH_InputSource_eInput5;
		swchOutputSetting.audioSource = BSWCH_InputSource_eInput5;
		rc = BSWCH_SetOutput( s_hSwch, BSWCH_OutputNum_e0, &swchOutputSetting );
		if (rc!=BERR_SUCCESS) { return rc; }
		break;
	}

	return BERR_SUCCESS;
}

#endif /** } (BCHP_CHIP==3560) **/
