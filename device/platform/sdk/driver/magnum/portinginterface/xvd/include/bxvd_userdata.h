/***************************************************************************
 *    Copyright (c) 2004-2010, Broadcom Corporation
 *    All Rights Reserved
 *    Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd_userdata.h $
 * $brcm_Revision: Hydra_Software_Devel/18 $
 * $brcm_Date: 3/3/10 5:27p $
 *
 * Module Description:
 *	 This module controls and returns the User Data coming in the stream 
 * and captured by the decoder.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/7401/bxvd_userdata.h $
 * 
 * Hydra_Software_Devel/18   3/3/10 5:27p davidp
 * SW7400-2704: Add SEI message frame packing support.
 * 
 * Hydra_Software_Devel/17   4/3/08 4:33p pblanco
 * PR41311: Made queue depth and item size runtime settable parameters
 * 
 * Hydra_Software_Devel/16   1/17/07 12:36p pblanco
 * PR27168: Incorporated changes suggested in code review on 16-Jan-2007
 * 
 * Hydra_Software_Devel/15   10/25/06 8:28a pblanco
 * PR20017: Fix c++ ifdef. It didn't span the entire header.
 * 
 * Hydra_Software_Devel/14   6/9/06 3:15p pblanco
 * PR20017: Fixed error return value in the documentation. We return
 * BXVD_ERR_USERDATA_NONE instead of the value originally stated.
 * 
 * Hydra_Software_Devel/13   6/9/06 1:16p pblanco
 * PR20017: Removed unused parameters from uninstall callback function.
 * 
 * Hydra_Software_Devel/12   6/9/06 8:54a pblanco
 * PR20017: Remove reference to unused BXVD_FlagReadDone function.
 * 
 * Hydra_Software_Devel/11   3/23/06 2:43p pblanco
 * PR19877: Cleaned up userdata settings structure so DocJet will catch
 * it.
 * 
 * Hydra_Software_Devel/10   3/10/06 9:36a pblanco
 * PR20080: Added missing parameter direction tags to prototypes.
 * 
 * Hydra_Software_Devel/9   12/14/05 10:00a pblanco
 * PR16052: Changed install/uninstall callback function protoypes to
 * reflect tha changes made to the API functions.
 * 
 * Hydra_Software_Devel/8   11/22/05 9:42a pblanco
 * PR16052: Added third parameter to userdata callback to make it XVD
 * callback compliant.
 * 
 * Hydra_Software_Devel/7   11/18/05 9:21a pblanco
 * PR16052: Fixed API argument for read call. Was void *, now
 * BAVC_USERDATA_info *
 * 
 * Hydra_Software_Devel/5   11/2/05 9:44a pblanco
 * PR16052: Added function prototypes for install/uninstall interrupt
 * callback.
 * 
 * Hydra_Software_Devel/4   7/7/05 4:13p pblanco
 * PR16052: Check in after fixing CC problems.
 * 
 * Hydra_Software_Devel/3   6/23/05 5:50p maivu
 * PR 15894: Modifications based on feedback from XVD API review for
 * Phase1
 * 
 * Hydra_Software_Devel/2   6/21/05 2:36p maivu
 * PR 15894: Removed userdata install/uninstall functions, added generic
 * interrupt install/uninstall function instead. Added modifications to
 * be more compatible with mvd.
 * 
 * Hydra_Software_Devel/1   6/17/05 11:37a maivu
 * PR 15894: Initial revision
 * 
  ***************************************************************************/
#ifndef BXVD_USERDATA_H__
#define BXVD_USERDATA_H__

#include "bfmt.h"		   /* Video timing format */
#include "bavc.h"		   /* Analog Video Common */


#ifdef __cplusplus
extern "C" {
#endif

/* SEI message T35 */
#define BXVD_USERDATA_H264_TYPE_REGISTERED           4
#define BXVD_USERDATA_H264_TYPE_UNREGISTERED         5
#define BXVD_USERDATA_H264_TYPE_FRAME_PACK          45

/***************************************************************************
Summary: 
	User Data Handle

Description: 
	An opaque handle for each channel
****************************************************************************/
typedef struct BXVD_P_UserDataContext *BXVD_Userdata_Handle;

/***************************************************************************
Summary: 
	User Data default settings structure

Description:
	Contains the maximum userdata buffer size and any other default settings as
 required. The maximum userdata default size is 4K.

****************************************************************************/
typedef struct BXVD_Userdata_Settings
{
      int maxDataSize;      /* Default is 4K */
      int maxQueueDepth;    /* Default is 64 entries */
      int maxQueueItemSize; /* Default is 512 bytes */
} BXVD_Userdata_Settings;

/***************************************************************************
Summary: 
	Returns default User data settings

Returns:
	BERR_SUCCESS - If function is successful.

See Also: 
	BXVD_Userdata_Open
	
****************************************************************************/
BERR_Code BXVD_Userdata_GetDefaultSettings (
	BXVD_Userdata_Settings *pDefSettings    /* [out] default User Data settings */
);

/***************************************************************************
Summary: 
	Opens user data channel

Description: 
	This function initializes data structures for collecting user
	data present in the stream. In MPEG stream user data can come
	after sequence header, after gop header and after picture 
	header. This function initializes user data handle for the
	channel passed. This handle should be used to make subsequent
	calls to other user data APIs.

Returns:
	BERR_SUCCESS - If function is successful.

See Also: 
	BXVD_Userdata_CloseUserData
	
****************************************************************************/
BERR_Code BXVD_Userdata_Open(
	BXVD_ChannelHandle            hXVDCh,      /* [in] XVD Channel handle */
	BXVD_Userdata_Handle         *phUserData,  /* [out] User data handle */
	const BXVD_Userdata_Settings *pDefSettings /* [in] default User Data settings */
);

/***************************************************************************
Summary: 
	Closes user data channel

Description: 
	This function closes the user data channel. It frees any 
	resources acquired by the channel and disables User Data extraction .

Returns:
	BERR_SUCCESS - If function is successful.

See Also: 
	BXVD_Userdata_Open
	
****************************************************************************/
BERR_Code BXVD_Userdata_Close (
	BXVD_Userdata_Handle   hUserData /* [In] User data handle */
);


/***************************************************************************
Summary: 
	Read buffer information

Description: 
	This function returns the next available packet of User Data. This is called
 from the application's userdata callback function.

Returns:
	BERR_SUCCESS              - If function is successful.
	BXVD_ERR_USERDATA_NONE    - Returns an error if no user data is available in
                             the buffer.
 BXVD_ERR_USERDATA_INVALID - The userdata buffer contains invalid data

See Also: 
	BXVD_Userdata_InstallInterruptCallback
	BXVD_Userdata_UninstallInterruptCallback
****************************************************************************/
BERR_Code BXVD_Userdata_Read(
	BXVD_Userdata_Handle   hUserData,      /* [In] User data handle */
	BAVC_USERDATA_info *pUserDataInfo  /* [Out] User data buffer info */
);


/***************************************************************************
Summary: 
	Read buffer information from an Isr

Description: 
	See description of BXVD_Userdata_Read()

Returns:
	BERR_SUCCESS - If function is successful.
	BXVD_ERR_USERDATA_NONE - Returns an error if no user data is
		available in the buffer.

See Also: 
	BXVD_USerdata_Read
****************************************************************************/
BERR_Code BXVD_Userdata_Read_isr(
	BXVD_Userdata_Handle   hUserData,     /* [In] User data handle */
	BAVC_USERDATA_info     *pUserDataInfo  /* [Out] User data buffer info */
);

/***************************************************************************
Summary: 
	Enable/Disable user data capture

Description: 
	This function enables or disables the capture of user data

Returns:
	BERR_SUCCESS - If opened XVD is successful.

See Also:
	
****************************************************************************/
BERR_Code BXVD_Userdata_Enable (
	BXVD_Userdata_Handle  hUserData, /* [In] User data handle */
	bool                  bEnable    /* [In] Enable/disable userdata */
	);

/***************************************************************************
Summary:
  Install user data callback

Description:
  This function installs a callback to the application that occurs when
  user data is available.

Returns:
  BERR_SUCCESS - If call is successful

See Also:
 	BXVD_Userdata_UninstallInterruptCallback
		BXVD_Userdata_Read
****************************************************************************/
BERR_Code BXVD_Userdata_InstallInterruptCallback
(
 BXVD_Userdata_Handle hUserData, /* [in] XVD userdata handle */
 BINT_CallbackFunc    xvdInterruptCallBack, /* [in] user's callback function */
 void                   *pParm1, /* [in] unused */
 int                     parm2  /* [in] unused */
 );

/***************************************************************************
Summary:
  Uninstall user data callback

Description:
  This function uninstalls an interrupt callback installed by
  BXVD_UserData_InstallInterruptCallback.

Returns:
  BERR_SUCCESS - If call was successful

See Also:
  BXVD_Userdata_InstallInterruptCallback
  BXVD_Userdata_Read
****************************************************************************/
BERR_Code BXVD_Userdata_UninstallInterruptCallback
(
 BXVD_Userdata_Handle    hUserData, /* [in] XVD userdata handle */
 BINT_CallbackFunc       xvdInterruptCallBack /* [in] callback to uninstall */
 );

/* Function Prototypes }}} */

#ifdef __cplusplus
}
#endif

#endif /* BXVD_USERDATA_H__ */
/* End of file. */
 


