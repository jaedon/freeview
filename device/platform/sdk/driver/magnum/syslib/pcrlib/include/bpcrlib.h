/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpcrlib.h $
 * $brcm_Revision: Hydra_Software_Devel/32 $
 * $brcm_Date: 6/27/11 4:41p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/pcrlib/7038/bpcrlib.h $
 * 
 * Hydra_Software_Devel/32   6/27/11 4:41p bandrews
 * SW7425-654: merge to main
 * 
 * Hydra_Software_Devel/SW7425-654/2   6/24/11 12:25a bandrews
 * SW7425-654: add pairing to config
 * 
 * Hydra_Software_Devel/SW7425-654/1   6/23/11 11:41p bandrews
 * SW7425-654: add non-real-time behavior and setting to pcrlib
 * 
 * Hydra_Software_Devel/31   11/12/10 5:47p bandrews
 * SW35230-2125: renamed config option to refresh_stc_on_invalidate, fixed
 * warning
 * 
 * Hydra_Software_Devel/30   11/11/10 9:00p bandrews
 * SW35230-2125: allow pcrlib to seed STC from lowest PTS during
 * invalidate
 * 
 * Hydra_Software_Devel/29   7/23/08 3:46p jgarrett
 * PR 39452: Merging to main branch
 * 
 * Hydra_Software_Devel/Nexus_Audio_Descriptors/1   5/15/08 4:13p jgarrett
 * PR 42360: Adding tertiary decoder for audio descriptors
 * 
 * Hydra_Software_Devel/28   2/15/08 9:50p bandrews
 * PR39635: Update to handle non-TSM tsm mode
 * 
 * Hydra_Software_Devel/27   10/17/07 5:45p bandrews
 * PR33892: Fixed by using dropped frame count to determine errors
 * unrecoverable by DM and recover in PCRlib
 * 
 * Hydra_Software_Devel/26   9/19/07 4:10p bandrews
 * PR33851: Add decoder flush request when outside sync limit
 * 
 * Hydra_Software_Devel/25   8/27/07 8:07p bandrews
 * PR33851: Audio master mode and prep for adaptive tsm
 * 
 * Hydra_Software_Devel/24   6/28/07 8:01p bandrews
 * PR31796: PCRlib implementation of audio and video master modes
 * 
 * Hydra_Software_Devel/23   12/4/06 1:39p erickson
 * PR26295: only call BXPT_PcrOffset_RegenOffset_isr on request_stc, not
 * pts_error. this requires a new param to UpdateStc.
 * 
 * Hydra_Software_Devel/22   10/2/06 4:24p vsilyaev
 * PR 23826: Added functions to allow user control STC
 * 
 * Hydra_Software_Devel/21   8/25/06 3:21p jjordan
 * PR23128: fix build for non-IP STB
 * 
 * Hydra_Software_Devel/20   8/4/06 12:46p erickson
 * PR23098: convert b_stc and b_delta_stc to uint32_t and int32_t. these
 * are standard values for PTS math. also made BPCRlib_StcDiff_isr
 * unconditionally public.
 * 
 * Hydra_Software_Devel/19   3/1/06 3:26p jjordan
 * PR18981: fix "live playback" and add config option IP_USE_LIVE_PLAYBACK
 * 
 * Hydra_Software_Devel/18   2/28/06 1:47p jjordan
 * PR18981: PR18981: prototype "live playback"
 * 
 * Hydra_Software_Devel/17   2/3/06 3:50p vsilyaev
 * PR 18642: Update PCRlibrary to support playback of constant bitrate
 * streams
 * 
 * Hydra_Software_Devel/PR18642/2   1/27/06 2:28p vsilyaev
 * PR18642: Added code to use PCR for STC update
 * 
 * Hydra_Software_Devel/PR18642/1   1/19/06 6:15p vsilyaev
 * PR18642: Use PTS offset to control buffer depth in the decoders
 * 
 * Hydra_Software_Devel/16   12/20/05 2:12p vsilyaev
 * PR 18642: Added BPCRlib_Channel_LoadStc function to load user defined
 * STC
 * 
 * Hydra_Software_Devel/15   12/15/05 3:58p vsilyaev
 * PR 18642: Added support for the PTS offset and CDB check
 * 
 * Hydra_Software_Devel/14   8/29/05 6:23p vsilyaev
 * PR 16685,PR15967: Added use of UpdateStc function.
 * 
 * Hydra_Software_Devel/13   8/26/05 12:32p erickson
 * PR15967: Added Invalidate function to cover decoder based trickmodes
 * 
 * Hydra_Software_Devel/12   8/17/05 7:56p vsilyaev
 * PR 15967: Added BPCRlib_Channel_AudioPtsError_isr function.
 * 
 * Hydra_Software_Devel/11   8/10/05 4:14p vsilyaev
 * PR 15967: Removed references to BMVD_.
 * 
 * Hydra_Software_Devel/10   8/9/05 2:31p vsilyaev
 * PR 15967: Use both aux transport and decoder to access STC.
 * 
 * Hydra_Software_Devel/9   6/28/05 1:16p vsilyaev
 * PR 15697: Improved interface to update STC.
 * 
 * Hydra_Software_Devel/8   6/24/05 3:54p vsilyaev
 * PR 15967: Use audio/video interface to read STC and PTS.
 * 
 * Hydra_Software_Devel/7   3/16/05 11:09a vsilyaev
 * PR 14413:  Removed reference to xpt_record.h (it's not needed for 7038
 * as well).
 * 
 * Hydra_Software_Devel/6   3/4/04 5:03p vsilyaev
 * PR 8915: Added VideoPtsError handler
 * 
 * Hydra_Software_Devel/5   2/1/04 6:01p vsilyaev
 * PR 8915: Use video specific structur for passing in PTS value (video
 * PTS could be either coded(a real one) or iterpolated.
 * 
 * Hydra_Software_Devel/4   12/10/03 6:35p vsilyaev
 * PR 8915: Changed BPCRlib_Channel_GetConfig to return a void.
 * 
 * Hydra_Software_Devel/3   12/9/03 4:13p vsilyaev
 * PR 8915:Added comments into for arguments of XXX_isr functions.
 * 
 * Hydra_Software_Devel/2   12/9/03 2:30p vsilyaev
 * PR 8915: PCRLib module for 7038.
 * 
 * Hydra_Software_Devel/1   12/3/03 6:38p vsilyaev
 * 7038 PCRlib module
 * 
 ***************************************************************************/
#ifndef BPCRLIB_H_
#define BPCRLIB_H_

#include "bxpt.h"
#include "bxpt_pcr.h"
#include "bchp.h"
#include "bavc.h"
#include "bkni.h"

/*= Module Overview *********************************************************
Overview
PCRlib module manages local timebase, aka STC (System Time Clock) of 
a decoder channel. Decoder channel is a combination of single MPEG video
decoder and audio decoder. Task of PCRlib is to keep decoders reference
time in sync with each other (to maintain lip-sync) and with incoming
MPEG stream.

User controls PCRLib module by connecting video and audio decoders
into the single timebase. For PVR type of applications users could
choose to connect record channel. For systems with dual audio
output user have to connect secondary audio decoder to the 
selected timebase channel.  For systems with three audio decodes,
the user will need to connect the tertiary audio decoder as well.

***************************************************************************/

/* This tyoe is used to represent instance of PCRLib module */
typedef struct BPCRlib_P_Handle *BPCRlib_Handle;

/***************************************************************************
Summary:
    Activates PCRLib module
Description:
    This function initializes internal structures of the PCRLib module
    and returns handle to the module instance.
See Also:
    BPCRlib_Close
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code 
BPCRlib_Open(
        BPCRlib_Handle *handle, /* [out] the BPCRlib handle */
        BCHP_Handle hChip   /* [in] the chip handle */
        );

/***************************************************************************
Summary:
    Deactivates PCRlib module
Description:
    This function release all resources allocated during call to 
    the BPCRlib_Open function.
See Also:
    BPCRlib_Open
Returns:
    N/A
***************************************************************************/
void 
BPCRlib_Close(
        BPCRlib_Handle handle /* [in] the BPCRlib handle */
        );

/***************************************************************************
Summary:
    Standard decoder interface for time-related functions
***************************************************************************/
typedef struct BPCRlib_StcDecIface {
    BERR_Code (*getPts)(void *dec, BAVC_PTSInfo *pts);
    BERR_Code (*getStc)(void *trp, void *dec, uint32_t *stc);
    BERR_Code (*getCdbLevel)(void *dec, unsigned *level);
    BERR_Code (*setStc)(void *trp, void *dec, bool dss, uint32_t stc);
    BERR_Code (*updateStc)(void *trp, bool is_request_stc);
    bool useAuxTrp;
} BPCRlib_StcDecIface;

/***************************************************************************
Summary:
    <Needs summary>
***************************************************************************/
typedef enum BPCRlib_Mode {
    BPCRlib_Mode_eAutoPts,
    BPCRlib_Mode_eConstantDelay
} BPCRlib_Mode;

/***************************************************************************
Summary:
    Synchronization mode
***************************************************************************/
typedef enum BPCRlib_TsmMode 
{
    BPCRlib_TsmMode_eSTCMaster = 0,             /* STC is seeded with either the video or audio PTS, depending on which decoder makes an STC request first */
    BPCRlib_TsmMode_eVideoMaster,           /* STC is seeded with the video PTS */
    BPCRlib_TsmMode_eAudioMaster,           /* STC is seeded with the audio PTS */
    BPCRlib_TsmMode_eOutputMaster,          /* No tsm is performed.  Output clock pulls data through decoder.  Also called VSYNC mode. */
    BPCRlib_TsmMode_eMax
} BPCRlib_TsmMode;

/***************************************************************************
Summary:  
    This structure is used for run-time configuration of PCRLib channel
Description:
    This structure has a information about managed timebase channel. 
    It's an user role to fill in this structure with values appropriate for
    desired configuration.
***************************************************************************/

typedef struct BPCRlib_Config {
    bool playback; /* true if PCRlib channel is used to manage playback type of sources, false otherwise */
    BAVC_StreamType stream; /* Type of the managed stream */
    void *audio; /* Audio decoder handle for the primary audio channel */
    const BPCRlib_StcDecIface *audio_iface; /* interface to control the primary audio decoder */
    void *video; /* Video decoder handle */
    const BPCRlib_StcDecIface *video_iface; /* interface to control the video decoder */
    void *secondary_audio; /* Audio decode handle for the secondary audio channel */
    const BPCRlib_StcDecIface *secondary_audio_iface; /* interface to control the secondary audio decoder */
    void *tertiary_audio; /* Audio decode handle for the tertiary audio channel */
    const BPCRlib_StcDecIface *tertiary_audio_iface; /* interface to control the tertiary audio decoder */
    void *aux_transport; /* pointer for the auxilary transport engine */
    unsigned video_cdb_level; /* threshold for the video decoder's  compressed buffer, this is used to detect underflow condition in the decoder */
    int video_pts_offset; /* this offset is used in playback mode, when STC initially loaded from the video PTS, 45KHz units */
    int video_stc_discard; /* this delta is a discard threashold between video PTS and STC, 45KHz units */
    unsigned audio_cdb_level; /* threshold for the video decoder's  compressed buffer, this is used to detect underflow condition in the decoder */
    int audio_pts_offset; /* this offset is used in playback mode, when STC initially loaded from the audio PTS, 45KHz units */
    int audio_stc_discard; /* this delta is a discard threashold between audio PTS and STC, 45KHz units  */
    int pcr_offset; /* offset between PCR and STC. thise offset is used to set an additional dealy in the decoders in order to compoensate fro the jitter, 45KHz units */
    int pcr_discard; /* threshold to discard PCR-PTS , to discard PCR value , 45KHz units */
    BPCRlib_Mode mode; /* operation mode for the PCR channel */
    bool is_playback_ip; /* true if PCRlib channel is used to manage IP playback type of sources, false otherwise */
    BPCRlib_TsmMode tsm_mode; /* allows the user to control which decoder will control the STC --> defaults to whoever requests an STC first */
    unsigned int sync_limit; /* the limit outside of which the decoders will not apply master mode TSM */
    BKNI_EventHandle flush; /* requests that decoder CDBs be flushed */
    uint32_t consecutive_pts_error_limit; /* how many consecutive pts errors before we do something different */
    bool refresh_stc_on_invalidate; /* force a refresh of the STC when invalidate is called, defaults true */
    bool non_real_time; /* causes various changes to behavior when in non real time mode */
    bool paired; /* causes changes to behavior when paired with another pcrlib instance */
} BPCRlib_Config;

/* This tyoe is used to control PCRLib initial configuration */
typedef void *BPCRlib_ChannelSettings;

/***************************************************************************
Summary:  
    This function initializes PCRlib channel initial configuration
Description:
    This function is used to populate channel initial configuration
    with default parameters.
Returns:
    BERR_SUCCESS                - if successful 
***************************************************************************/
BERR_Code BPCRlib_Channel_GetChannelDefSettings(
        BPCRlib_Handle handle,  /* [in] PCRlib handle */
        BPCRlib_ChannelSettings *config /* [out] default PCRlib channel settings */
    );

/* This tyoe is used to represent instance of the PCRlib channel */
typedef struct BPCRlib_P_Channel *BPCRlib_Channel;

/***************************************************************************
Summary:
    Activates PCRlib channel
Description:
    This function initializes internal structures of the PCRLib channel
    and returns handle to the channel instance.
See Also:
    BPCRlib_DestroyChannel
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code
BPCRlib_Channel_Create(
        BPCRlib_Handle handle,  /* [in] PCRlib handle */
        BXPT_PCR_Handle pcr,    /* [in] XPT_PCR handle */
        BPCRlib_Channel *channel, /* [out] PCRlib channel handle */
        const BPCRlib_ChannelSettings *settings /* [in[ PCRlib channel configuration */
    );

/***************************************************************************
Summary:
    Deactivates PCRlib channel
Description:
    This function release all resources allocated during call to 
    the BPCRlib_Channel_Create function.
See Also:
    BPCRlib_Channel_Create
Returns:
    N/A
***************************************************************************/
void
BPCRlib_Channel_Destroy(
        BPCRlib_Channel channel /* [in] PCRlib channel handle */
    );

/***************************************************************************
Summary:
    Get current configuration for PCRlib channel
Description:
    This function is used to retrieve current configuration of the PCRlib channel
See Also:
    BPCRlib_Channel_SetConfig
Returns:
    BERR_SUCCESS                - if successful 
***************************************************************************/
void
BPCRlib_Channel_GetConfig(
        BPCRlib_Channel channel, /* [in] PCRlib channel handle */
        BPCRlib_Config *config /* [out] current configuration of the PCRlib channel  */
        );

/***************************************************************************
Summary: 
    Set new configuration to PCRlib channel
Description:
    This function is used to sent new configuration to the PCRlib channel
See Also:
    BPCRlib_Channel_SetConfig
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code
BPCRlib_Channel_SetConfig(
        BPCRlib_Channel channel,/* [in] PCRlib channel handle */
        const BPCRlib_Config *config /* [out] new configuration for the PCRlib channel  */
        );

/***************************************************************************
Summary: 
    Invalidates state of PCRlib channel
Description:
    This function is used to invalidate state of all decoders attached to the PCRlib channels, usually it cases 
    to reload STC to the decoders on earliest opportunity.
See Also:
    BPCRlib_Channel_SetConfig
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code
BPCRlib_Channel_Invalidate(
        BPCRlib_Channel channel /* [in] PCRlib channel handle */
        );

/***************************************************************************
Summary:
    Update STC in audio decoder
Description:
    This function shall be called by an user when audio decoder
    has requested STC. Usually user have to install ISR callback
    and call this function from a callback.
See Also:
    BPCRlib_Channel_SetConfig, BPCRlib_Channel_VideoRequestStc_isr
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code
BPCRlib_Channel_AudioRequestStc_isr(
    BPCRlib_Handle handle, /* [in] PCRlib channel handle */
    void *audio,           /* [in] audio decoder handle */
    uint32_t audio_pts     /* [in] last PTS from the audio stream, in 45KHz units */
    );

/***************************************************************************
Summary:
    Update STC in video decoder
Description:
    This function shall be called by an user when video decoder
    has requested STC. Usually user have to install ISR callback
    and call this function from a callback.
See Also:
    BPCRlib_Channel_SetConfig, BPCRlib_Channel_AudioRequestStc_isr
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code
BPCRlib_Channel_VideoRequestStc_isr(
    BPCRlib_Handle handle, /* [in] PCRlib channel handle */
    void *video, /* [in] video decoder handle */
    const BAVC_PTSInfo *video_pts /* [in] last PTS from the video stream, in 45KHz units */
    );

/***************************************************************************
Summary:
    Resynchronize video decoder
Description:
    This function shall be called by an user when video decoder
    has received PTS Error. Usually user have to install ISR callback
    and call this function from a callback.
See Also:
    BPCRlib_Channel_SetConfig, BPCRlib_Channel_VideoRequestStc_isr
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code
BPCRlib_Channel_VideoPtsError_isr(
    BPCRlib_Handle handle, /* [in] PCRlib channel handle */
    void *video, /* [in] video decoder handle */
    const BAVC_PTSInfo *video_pts, /* [in] last PTS from the video stream, in 45KHz units */
    uint32_t video_stc /* [in] snapshot value of STC from the video decoder, in 45KHz units */ 
    );

/***************************************************************************
Summary:
    Resynchronize audio decoder
Description:
    This function shall be called by an user when video decoder
    has received PTS Error. Usually user have to install ISR callback
    and call this function from a callback.
See Also:
    BPCRlib_Channel_SetConfig, BPCRlib_Channel_AudioRequestStc_isr
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code
BPCRlib_Channel_AudioPtsError_isr(
    BPCRlib_Handle handle, /* [in] PCRlib channel handle */
    void *audio, /* [in] audio decoder handle */
    const BAVC_PTSInfo *audio_pts, /* [in] last PTS from the audio stream, in 45KHz units */
    uint32_t audio_stc /* [in] snapshot value of STC from the audio decoder, in 45KHz units */ 
    );

/***************************************************************************
Summary: 
    Set new STC
Description:
    This function is used to update STC value for the PCRLib channel
See Also:
    BPCRlib_Channel_GetStc 
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code
BPCRlib_Channel_PcrUpdate(
        BPCRlib_Channel channel,/* [in] PCRlib channel handle */
        uint32_t stc/* [in] new stc */
        );

/***************************************************************************
Summary: 
    Get current STC
Description:
    This function is used to retrieve STC  value for the PCRLib channel
See Also:
    BPCRlib_Channel_PcrUpdate
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code
BPCRlib_Channel_GetStc(BPCRlib_Channel channel, uint32_t *stc);

/***************************************************************************
Summary: 
    Calculate STC difference
Description:
    This function takes stc in the 45KHz domain (MPEG) or 27MHz domain (DSS)
    as 32 bit unsigned and returns delta in the 22.5KHz domain, 32 bit signed
Returns:

***************************************************************************/
int32_t BPCRlib_StcDiff_isr(bool dss, uint32_t stc1, uint32_t stc2);

/***************************************************************************
Summary: 
    Check to see if PCR discontinuity has occured
Description:
Returns:
    BERR_SUCCESS                - if successful 
    BERR_INVALID_PARAMETER      - Bad input parameter
***************************************************************************/
BERR_Code
BPCRlib_HasPcrDisco(
    BPCRlib_Handle handle, /* [in] PCRlib channel handle */
    void *decoder, /* [in] decoder handle */
    bool *has_disco
    );

#endif /* BPCRLIB_H_ */


