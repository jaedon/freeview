/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bods_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 5/25/12 2:42p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ods/src/bods_priv.h $
 * 
 * Hydra_Software_Devel/1   5/25/12 2:42p atanugul
 * SW3462-7: Initial Version
 *
 ***************************************************************************/
#ifndef BODS_PRIV_H__
#define BODS_PRIV_H__

#include "bchp.h"
#include "bods.h"

BDBG_OBJECT_ID_DECLARE(BODS);
BDBG_OBJECT_ID_DECLARE(BODS_CHANNEL);

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
	The handle for ODS module.

Description:

See Also:
	BODS_Open()

****************************************************************************/
typedef struct BODS_P_Handle
{
    BDBG_OBJECT(BODS)
    BODS_Settings settings; /* ODS Settings */
    void *pImpl;            /* Device specific structure */
} BODS_P_Handle;

/***************************************************************************
Summary:
	The handle for ODS module channel handle

Description:

See Also:
	BODS_OpenChannel()

****************************************************************************/
typedef struct BODS_P_ChannelHandle
{
    BDBG_OBJECT(BODS_CHANNEL)
    BODS_Handle hOds;

    void *pImpl;                            /* Device specific structure */
} BODS_P_ChannelHandle;


#ifdef __cplusplus
}
#endif
 
#endif
