/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmuxlib_file_asf_header.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 5/11/11 4:03p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/muxlib/7425/bmuxlib_file_asf_header.h $
 * 
 * Hydra_Software_Devel/3   5/11/11 4:03p nilesh
 * SW7425-447: Offset presentation by pre-roll
 * 
 * Hydra_Software_Devel/2   5/11/11 3:22p nilesh
 * SW7425-447: Post-Process ASF.
 * 
 * Hydra_Software_Devel/1   5/11/11 11:56a nilesh
 * SW7425-447: ASF Mux
 *
 ***************************************************************************/

#ifndef BMUXLIB_FILE_ASF_HEADER_H_
#define BMUXLIB_FILE_ASF_HEADER_H_

#include "bmuxlib_file_asf.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

#define BMUXlib_File_ASF_P_FilePropertiesObject_PREROLL_DEFAULT 2000

/* BMUXlib_File_ASF_SetHeader - will return true when the header has been written
 */
bool
BMUXlib_File_ASF_P_Header_WriteHeader(
         BMUXlib_File_ASF_Handle hASFMux
         );

typedef enum BMUXlib_File_ASF_P_PostProcess_State
{
   /* File Properties */
   BMUXlib_File_ASF_P_PostProcess_State_eStart,
   BMUXlib_File_ASF_P_PostProcess_State_eFileSize,
   BMUXlib_File_ASF_P_PostProcess_State_eDataPacketsCount_File,
   BMUXlib_File_ASF_P_PostProcess_State_ePlayDuration,
   BMUXlib_File_ASF_P_PostProcess_State_eSendDuration,
   BMUXlib_File_ASF_P_PostProcess_State_eBroadcastFlag,

   /* Data Object */
   BMUXlib_File_ASF_P_PostProcess_State_eDataObjectSize,
   BMUXlib_File_ASF_P_PostProcess_State_eTotalDataPackets,
   BMUXlib_File_ASF_P_PostProcess_State_eDone
} BMUXlib_File_ASF_P_PostProcess_State;

bool
BMUXlib_File_ASF_P_Header_PostProcessHeader(
         BMUXlib_File_ASF_Handle hASFMux
         );

#ifdef __cplusplus
}
#endif

#endif /* BMUXLIB_FILE_ASF_HEADER_H_ */
