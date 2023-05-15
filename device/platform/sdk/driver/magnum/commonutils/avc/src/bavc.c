/***************************************************************************
 *     Copyright (c) 2003-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bavc.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 6/7/06 12:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/avc/7038/bavc.c $
 * 
 * Hydra_Software_Devel/4   6/7/06 12:46p pntruong
 * PR21975: Need to remove CHP_REV from code in AVC.
 *
 * Hydra_Software_Devel/3   5/18/05 12:24a agin
 * PR14720: B2, C1, C2 compilation support.
 *
 * Hydra_Software_Devel/2   11/4/04 5:49p gmullen
 * PR11894: Added 6th parser band (and 5th, 7th, and 8th too) to
 * BAVC_XptOutputId.
 *
 * Hydra_Software_Devel/1   4/2/04 3:29p gmullen
 * PR8852: Added BAVC_GetXptId().
 *
 *
 ***************************************************************************/

#include "bstd_defs.h"
#include "berr.h"
#include "bdbg.h"
#include "bavc.h"

BDBG_MODULE( BAVC );

typedef struct
{
	BAVC_XptOutputId eOutputId;
	BAVC_XptId       XptId;
}
BAVC_P_XptOutputMap;

BAVC_XptId BAVC_GetXptId(
	BAVC_XptOutputId eOutputId
	)
{
	unsigned i;
	BAVC_XptId XptId = -1;
	static const BAVC_P_XptOutputMap s_astOutputMap[] =
	{
		{ BAVC_XptOutputId_eParserBand0, 0  },
		{ BAVC_XptOutputId_eParserBand1, 1  },
		{ BAVC_XptOutputId_eParserBand2, 2  },
		{ BAVC_XptOutputId_eParserBand3, 5  },
		{ BAVC_XptOutputId_eParserBand4, 6  },
		{ BAVC_XptOutputId_eParserBand5, 10 },
		{ BAVC_XptOutputId_eParserBand6, 11 },
		{ BAVC_XptOutputId_eParserBand7, 12 },
		{ BAVC_XptOutputId_eParserBand8, 13 },
		{ BAVC_XptOutputId_ePlayback0,   3  },
		{ BAVC_XptOutputId_ePlayback1,   4  },
		{ BAVC_XptOutputId_ePlayback2,   8  },
		{ BAVC_XptOutputId_ePlayback3,   9  }
	};

	for( i = 0; i < sizeof( s_astOutputMap ) / sizeof( BAVC_P_XptOutputMap ); i++ )
	{
		if( eOutputId == s_astOutputMap[i].eOutputId )
		{
			XptId = s_astOutputMap[i].XptId;
			break;
		}
	}

	return XptId;
}

/* End of File */

