/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvde_channel.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 6/10/11 7:52p $
 *
 * Module Description: Video Channel Interface for DSP
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vde/7425/bvde_channel.h $
 * 
 * Hydra_Software_Devel/3   6/10/11 7:52p speter
 * SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
 * Video Decode using Raaga DSP
 * 
 * Hydra_Software_Devel/2   6/2/11 3:56p speter
 * SW7425-409: [7425] Modifying the frame buffer allocation part
 * 
 * Hydra_Software_Devel/1   5/31/11 4:44p speter
 * SW7425-409: [7425] Adding Video Decode Engine - Porting Interface for
 * Video Decode using Raaga DSP
 * 
 ***************************************************************************/

#ifndef BVDE_CHANNEL_H_
#define BVDE_CHANNEL_H_

#include "bxdm_decoder.h"

/***************************************************************************
Summary:
Channel Handle
***************************************************************************/
typedef struct BVDE_Channel *BVDE_ChannelHandle;

/***************************************************************************
Summary:
Picture frame sizes
***************************************************************************/
typedef enum BVDE_Resolution
{
   BVDE_Resolution_eFullHD,         /* 1920 x 1080 */
   BVDE_Resolution_eHD,             /* 1280 x 720  */
   BVDE_Resolution_ePAL,            /* 720 x 576   */
   BVDE_Resolution_eSD,             /* 720 x 480   */   
   BVDE_Resolution_eCIF,            /* 352  288   */
   BVDE_Resolution_eQCIF,           /* 176  144   */
   BVDE_Resolution_eMaxModes        
} BVDE_Resolution;

/***************************************************************************
Summary:
Channel Open Settings
***************************************************************************/
typedef struct BVDE_ChannelOpenSettings
{

    BMEM_Handle memPicHandle;
    BVDE_Resolution resolution;       /* Decoder Resolution */
    BAVC_VideoCompressionStd *pCodecList;   /* List of Video compression standards used by this channel */
    uint32_t codecCount;                    /* Number of video compression standards in list */
} BVDE_ChannelOpenSettings;

/***************************************************************************
Summary:
Get default settings for a video channel
***************************************************************************/
void BVDE_Channel_GetDefaultOpenSettings(
    BVDE_ChannelOpenSettings *pSettings     /* [out] */
    );

/***************************************************************************
Summary:
Opens a video channel
***************************************************************************/
BERR_Code BVDE_Channel_Open(
    BVDE_Handle deviceHandle,
    unsigned index,
    const BVDE_ChannelOpenSettings *pSettings, 
    BVDE_ChannelHandle *pHandle                 /* [out] */
    );

/***************************************************************************
Summary:
Closes a video channel
***************************************************************************/
void BVDE_Channel_Close(
    BVDE_ChannelHandle handle
    );

/***************************************************************************
Summary:
Start-time settings for a video channel
***************************************************************************/
typedef struct BVDE_ChannelStartSettings
{
    BAVC_VideoCompressionStd codec;         
    const BAVC_XptContextMap *pContextMap;  /* What RAVE context should be read while decoding. */

    /* TBD: Error hanlding Mode might have to be added here */
    
} BVDE_ChannelStartSettings;

/***************************************************************************
Summary:
Get default settings for a video channel start
***************************************************************************/
void BVDE_Channel_GetDefaultStartSettings(
    BVDE_ChannelStartSettings *pSettings    /* [out] */
    );

/***************************************************************************
Summary:
Start a video channel
***************************************************************************/
BERR_Code BVDE_Channel_Start(
    BVDE_ChannelHandle handle,
    const BVDE_ChannelStartSettings *pSettings
    );

/***************************************************************************
Summary:
Stop a video channel
***************************************************************************/
void BVDE_Channel_Stop(
    BVDE_ChannelHandle handle
    );

/***************************************************************************
Summary:
Provide the Display Manager (DM) public decoder interface function pointers

Description:
The DM mandates that all external video decoders support a list of predefined 
functions. This is provided as a structure of function pointers. The VDE 
implements each of these functions and populates the addresses of these 
functions in this call.
***************************************************************************/
BERR_Code BVDE_Channel_GetDMInterface(
    BVDE_ChannelHandle handle,
    BXDM_Decoder_Interface *pDMInterface,
    void **pContext    
    );


/***************************************************************************
Summary:
Get Default CDB/ITB configuration for decoding
***************************************************************************/
void BVDE_Channel_GetDefaultCdbItbConfig(
    BVDE_ChannelHandle handle,
    BAVC_CdbItbConfig *pConfig  /* [out] */
    );

#endif
