/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_tsmux.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/1/12 10:00a $
 *
 * Porting interface for TS muxing via playback. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/src/core28nm/bxpt_tsmux.c $
 * 
 * Hydra_Software_Devel/1   10/1/12 10:00a gmullen
 * SW7445-17: Created core28nm file tree
 * 
 * Hydra_Software_Devel/19   3/29/12 9:22a gmullen
 * SW7435-48: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7435-48/1   3/19/12 4:31p gmullen
 * SW7435-48: Configure playbacks for internal 27MHz timebase when in AFAP
 * mode
 * 
 * Hydra_Software_Devel/18   7/1/11 10:06a gmullen
 * SW7425-827: Merged fix to Hydra
 * 
 * Hydra_Software_Devel/SW7425-827/1   6/30/11 8:00p gmullen
 * SW7425-827: Fixed addition of playbacks to pacing and pause maps
 * 
 * Hydra_Software_Devel/17   6/15/11 10:27a gmullen
 * SW7425-712: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7425-712/1   6/14/11 2:13p gmullen
 * SW7425-712: Moved loading of pacing counter
 * 
 * Hydra_Software_Devel/16   6/8/11 4:34p gmullen
 * SW7425-653: Load pacing speed only when AFAP mode is enabled
 * 
 * Hydra_Software_Devel/15   6/3/11 4:34p gmullen
 * SW7425-653: Merged changes to Hydra branch
 * 
 * Hydra_Software_Devel/SW7425-653/1   6/2/11 10:35a gmullen
 * SW7425-653: Added non-realtime transcoding support
 * 
 * Hydra_Software_Devel/14   4/24/11 6:15p hongtaoz
 * SW7425-430: further fix the ts mux band pause logic; also don't need to
 * touch band pause for realtime mode;
 * 
 * Hydra_Software_Devel/13   4/24/11 1:21p hongtaoz
 * SW7425-430: merged to mainline; only enable tsmux band pause for non-
 * realtime mode;
 * 
 * Hydra_Software_Devel/12   4/24/11 11:11a gmullen
 * SW7425-15: Band hold and band pause maps are incorrect
 * 
 * Hydra_Software_Devel/11   1/28/11 9:36a gmullen
 * SW7425-15: Default the pacing error bounds to 0x3DAD for ts muxing
 * 
 * Hydra_Software_Devel/10   1/4/11 3:06p gmullen
 * SW7425-15: Fixed register addressing bug in reset of band pause and
 * pacing regs
 * 
 * Hydra_Software_Devel/9   12/30/10 5:12p gmullen
 * SW7425-15: Reset pacing pause map by writing 0
 * 
 * Hydra_Software_Devel/8   12/30/10 3:47p gmullen
 * SW7425-15: Added  BXPT_TsMux_RemovePlayback()
 * 
 * Hydra_Software_Devel/6   12/28/10 2:16p gmullen
 * SW7425-15: Removed settings from _Create() call
 * 
 * Hydra_Software_Devel/5   12/21/10 2:04p gmullen
 * SW7425-15: Switched uiMuxDelay back to mS units
 * 
 * Hydra_Software_Devel/4   12/21/10 9:25a gmullen
 * SW7425-15: Use PCR Offset handle to configure offset hw
 * 
 * Hydra_Software_Devel/3   12/7/10 2:22p gmullen
 * SW7425-15: Checked in implementation
 * 
 * Hydra_Software_Devel/2   12/1/10 8:01a gmullen
 * SW7425-15: Updated header and stub calls for TS mux
 * 
 * Hydra_Software_Devel/1   11/23/10 4:56p gmullen
 * SW7425-15: Added transcoding extensions
 * 
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"

#include "bxpt_priv.h"
#include "bxpt_playback.h"
#include "bxpt_tsmux.h"

#include "bchp_xpt_mcpb.h"
#include "bchp_xpt_mcpb_ch0.h"
#include "bchp_xpt_mcpb_ch1.h"
#include "bchp_xpt_memdma_mcpb.h"
#include "bchp_xpt_memdma_mcpb_ch0.h"

#include "bchp_xpt_fe.h"
#include "bchp_xpt_pcroffset.h"

#if( BDBG_DEBUG_BUILD == 1 )
BDBG_MODULE( xpt_tsmux );
#endif

typedef struct BXPT_P_TsMuxHandle
{
    BXPT_Handle hXpt;
    BXPT_Playback_Handle Playbacks[ BXPT_NUM_PLAYBACKS ];
    bool PbChanInMux[ BXPT_NUM_PLAYBACKS ];
    BXPT_TsMux_Settings Settings;
}
BXPT_P_TsMuxHandle;

void BXPT_TsMux_P_ResetBandPauseMap( 
    BXPT_Handle hXpt
    )
{
#if 1
    BSTD_UNUSED( hXpt );
#else
    unsigned Index;

    uint32_t RegAddr = BCHP_XPT_MCPB_BAND_PAUSE_MAPPING_VECTOR_0;

    for( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ )
    {
        BREG_Write32( hXpt->hRegister, RegAddr, 1 << Index );
        RegAddr += 4;
    }
#endif
}

typedef enum MapAction
{
    MapAction_Add,
    MapAction_Remove,
    MapAction_Refresh
}
MapAction;

#if 0
static void UpdatePlaybackMap( 
    BXPT_TsMux_Handle hTsMux,               /* [in] Handle for the TsMux */
    BXPT_Playback_Handle hPb,        /* [in] Handle for allocated playback channel */
    uint32_t MapAddr,               /* [in] Base address of map to update */
    MapAction Action                      
    )
{
    unsigned Index;

    unsigned MapBits = 0;

    for( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ )
    {
        if( hTsMux->PbChanInMux[ Index ] ) 
            MapBits |= 1 << Index;
    }

    switch (Action) 
    {
        case MapAction_Add:
        MapBits |= 1 << hPb->ChannelNo;
        break;

        case MapAction_Remove:
        MapBits &= ~( 1 << hPb->ChannelNo );
        break;

        case MapAction_Refresh:
        default:
        break;
    }

    /* Walk through the list of playbacks in this mux. Each one must have it's map set. */
    for( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ )
    {
        if( hTsMux->Playbacks[ Index ] ) 
        {
            BXPT_Playback_Handle hPb = hTsMux->Playbacks[ Index ];
            BREG_Write32( hPb->hRegister, MapAddr + (4 * hPb->ChannelNo), MapBits );
        }
    }
}

static void UpdatePlaybackConfig(
    BXPT_TsMux_Handle hTsMux
    )
{
    unsigned Index;
    for( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ )
    {
        if( hTsMux->Playbacks[ Index ] ) 
        {
            BXPT_Playback_ChannelSettings ChannelSettings;
            BXPT_Playback_Handle hPb = hTsMux->Playbacks[ Index ] ;

            BXPT_Playback_GetChannelSettings( hPb, &ChannelSettings );
            if (hTsMux->Settings.bAFAPMode) 
            {
                ChannelSettings.UsePcrTimeBase = false;
            }
            BXPT_Playback_SetChannelSettings( hPb, &ChannelSettings );
        }
    }
}
#endif
                                                 
void BXPT_TsMux_P_ResetPacingPauseMap( 
    BXPT_Handle hXpt
    )
{
#if 1
    BSTD_UNUSED( hXpt );
#else
    unsigned Index;

    uint32_t RegAddr = BCHP_XPT_PB_TOP_PACING_PAUSE_MAP_VECT_PB0;

    for( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ )
    {
        BREG_Write32( hXpt->hRegister, RegAddr, 0 );
        RegAddr += 4;
    }
#endif
}

#if 0
static void LoadStcMuxDelayDiff( 
    BXPT_TsMux_Handle hTsMux               /* [in] Handle for the TsMux */
    )
{
    unsigned Index;
    uint32_t Reg;

	Reg = BCHP_FIELD_DATA( XPT_PB_TOP_PACING_COUNT_RELOAD_CTRL, LD_STC_MUX_DELAY_DIFF, 1 );
    for( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ )
    {
        if( hTsMux->Playbacks[ Index ] )
            Reg |= 1 << hTsMux->Playbacks[ Index ]->ChannelNo;
    }
    
    BREG_Write32( hTsMux->hXpt->hRegister, BCHP_XPT_PB_TOP_PACING_COUNT_RELOAD_CTRL, Reg );
}
#endif

#if 0
static void LoadPacingCount( 
    BXPT_TsMux_Handle hTsMux               /* [in] Handle for the TsMux */
    )
{
    unsigned Index;
    
    unsigned LoadMap = 0;

    /* Build up the bitmap for loading the count.*/
    for( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ )
    {
        if( hTsMux->Playbacks[ Index ] )
            LoadMap |= 1 << hTsMux->Playbacks[ Index ]->ChannelNo;
    }

    /* Pass down the count and bitmap to each of the playback instances, since
    ** we don't know which one will get encoder data first. 
    */ 
    for( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ )
    {
        if( hTsMux->Playbacks[ Index ] )
            BXPT_Playback_P_SetPacingCount( 
                hTsMux->Playbacks[ Index ], 
                LoadMap,
                hTsMux->Settings.AFAPSettings.uiPacingCounter
            );
    }
}
#endif

#if 0
static void LoadPacingSpeed( 
    BXPT_TsMux_Handle hTsMux               /* [in] Handle for the TsMux */
    )
{
    unsigned Index;
    
    unsigned Speed = hTsMux->Settings.AFAPSettings.uiPacingSpeed;

    for( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ )
    {
        if( hTsMux->Playbacks[ Index ] )
            BXPT_Playback_P_SetPacingSpeed( hTsMux->Playbacks[ Index ], Speed );
    }
}
#endif

void BXPT_TsMux_GetDefaultSettings(
    BXPT_TsMux_Settings *Defaults   /* [out] The defaults */
    )
{
    BDBG_ASSERT( Defaults );
    BKNI_Memset( (void *) Defaults, 0, sizeof( *Defaults ) );
    Defaults->uiMuxDelay = 40;  
    Defaults->AFAPSettings.uiPacingSpeed = 8;
}

BERR_Code BXPT_TsMux_Create(
    BXPT_Handle hXpt,                       /* [in] Handle for this transport */
    BXPT_TsMux_Handle *hTsMux              /* [out] Handle for opened record channel */
    )
{
    BXPT_P_TsMuxHandle *lhMux; 

    BERR_Code Ret = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );
    BDBG_ASSERT( hTsMux );

    lhMux = BKNI_Malloc( sizeof( *lhMux ) );
    if( !lhMux )    
    {
        BDBG_ERR(( "lhMux malloc failed" ));
        Ret = BERR_TRACE( BERR_OUT_OF_SYSTEM_MEMORY );
        goto Error;
    }

    BKNI_Memset( (void *) lhMux, 0, sizeof( *lhMux ) );
    lhMux->hXpt = hXpt;
    BXPT_TsMux_GetDefaultSettings( &lhMux->Settings );

    *hTsMux = lhMux;
    return Ret;                                               

    Error:
    *hTsMux = NULL;
    return Ret;
}

void BXPT_TsMux_Destroy(
    BXPT_TsMux_Handle hTsMux     /* [in] Handle for the TsMux to destroy */
    )
{
#if 1
    BSTD_UNUSED( hTsMux );
#else
    unsigned Index;

    BDBG_ASSERT( hTsMux );

    for( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ )
    {
        if( hTsMux->Playbacks[ Index ] )
        {
            UpdatePlaybackMap( hTsMux, hTsMux->Playbacks[ Index ], BCHP_XPT_PB_TOP_BAND_PAUSE_MAP_VECT_PB0, MapAction_Remove );
            UpdatePlaybackMap( hTsMux, hTsMux->Playbacks[ Index ], BCHP_XPT_PB_TOP_PACING_PAUSE_MAP_VECT_PB0, MapAction_Remove );
            hTsMux->Playbacks[ Index ] = NULL;
        }
    }

    BKNI_Free( (void *) hTsMux );
#endif
}

BERR_Code BXPT_TsMux_AddPlayback(
    BXPT_TsMux_Handle hTsMux,               /* [in] Handle for the TsMux */
    BXPT_Playback_Handle hPb        /* [in] Handle for allocated playback channel */
    )
{
#if 1
    BSTD_UNUSED( hTsMux );
    BSTD_UNUSED( hPb );
    return BERR_SUCCESS;
#else
    unsigned Index;

    BERR_Code Ret = BERR_SUCCESS;

    BDBG_ASSERT( hTsMux );
    BDBG_ASSERT( hPb );

    for( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ )
    {
        if( hTsMux->Playbacks[ Index ] == hPb )
            goto Done;  /* They already added it. */

        if( !hTsMux->Playbacks[ Index ] )
            break;      /* Found an empty slot */
    }

    if( Index == BXPT_NUM_PLAYBACKS )
    {
        /* 
        ** This probably should be an internal error. We took care of duplicated 
        ** additions above, so there should not be more playbacks than BXPT_NUM_PLAYBACKS .
        */
        BDBG_ERR(( "All playback slots are used" ));
        Ret = BERR_INVALID_PARAMETER;
        goto Done;
    }

    hTsMux->Playbacks[ Index ] = hPb;
    hTsMux->PbChanInMux[ hPb->ChannelNo ] = true;
    UpdatePlaybackMap( hTsMux, hPb, BCHP_XPT_PB_TOP_BAND_PAUSE_MAP_VECT_PB0, MapAction_Add );
    if( hTsMux->Settings.bAFAPMode )
    {
        UpdatePlaybackMap( hTsMux, hTsMux->Playbacks[ Index ], BCHP_XPT_PB_TOP_PACING_PAUSE_MAP_VECT_PB0, MapAction_Add );
        LoadPacingSpeed( hTsMux );
        LoadPacingCount( hTsMux );
    }

    UpdatePlaybackConfig( hTsMux );
    LoadStcMuxDelayDiff( hTsMux );

    /*
    ** Packets carrying the NEXT_PACKET_TIMESTAMP values will be ignored by the threshold comparison logic
    ** For Mux usage, the value of this threshold is calculated based on the worst case audio bit rate (4kbps)supported
    ** The value of 300 msec should be programmed in this register 
    ** For Binary format = 0x3DAD (dec 15789) Mod-300 = 0x6978 (dec 27002)
    */
    BXPT_Playback_SetPacingErrorBound( hPb, 0x3DAD );   /* We're using the binary format */

    Done:
    return Ret;
#endif
}

void BXPT_TsMux_RemovePlayback(
    BXPT_TsMux_Handle hTsMux,               /* [in] Handle for the TsMux */
    BXPT_Playback_Handle hPb    /* [in] Handle for allocated playback channel */
    )
{
#if 1
    BSTD_UNUSED( hTsMux );
    BSTD_UNUSED( hPb );
#else
    unsigned Index;

    BDBG_ASSERT( hTsMux );
    BDBG_ASSERT( hPb );

    /* UpdatePlaybackMap() needs to be called before removing the hPb handles from hTsMux. */
    if( hTsMux->Settings.bAFAPMode )
    {
        UpdatePlaybackMap( hTsMux, hPb, BCHP_XPT_PB_TOP_BAND_PAUSE_MAP_VECT_PB0, MapAction_Remove );
        UpdatePlaybackMap( hTsMux, hPb, BCHP_XPT_PB_TOP_PACING_PAUSE_MAP_VECT_PB0, MapAction_Remove );
    }

    for( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ )
    {
        if( hTsMux->Playbacks[ Index ] == hPb )
        {
            hTsMux->Playbacks[ Index ] = NULL;
            hTsMux->PbChanInMux[ hPb->ChannelNo ] = false;
        }
    }
#endif
}

void BXPT_TsMux_GetSettings(
    BXPT_TsMux_Handle hTsMux,               /* [in] Handle for the TsMux */
    BXPT_TsMux_Settings *Settings   /* [out] The current settings  */
    )
{
    BDBG_ASSERT( hTsMux );
    BDBG_ASSERT( Settings );

    *Settings = hTsMux->Settings;
}

BERR_Code BXPT_TsMux_SetSettings(
    BXPT_TsMux_Handle hTsMux,               /* [in] Handle for the TsMux */
    const BXPT_TsMux_Settings *pSettings /* [in] New settings to use */
    )
{
#if 1
    BSTD_UNUSED( hTsMux );
    BSTD_UNUSED( pSettings );
    return BERR_SUCCESS;
#else
    uint32_t Reg;

    BERR_Code Ret = BERR_SUCCESS;

    BDBG_ASSERT( hTsMux );
    BDBG_ASSERT( pSettings );
    BDBG_ASSERT( pSettings->hPcrOffset );

    /* Largest delay (in mS) that hw supports: MUX_DELAY is 28 bits wide, and runs at 27 MHz*/
    #define MAX_MUX_DELAY   (9942)
    if( pSettings->uiMuxDelay > MAX_MUX_DELAY ) 
    {
        BDBG_ERR(( "uiMuxDelay out of range (0 to %u)", MAX_MUX_DELAY ));
        Ret = BERR_TRACE( BERR_INVALID_PARAMETER );
        goto Done;
    }

    hTsMux->Settings = *pSettings;

    /* STC_MUX_DELAY is in the PCR Offset block */
    Reg = BREG_Read32( hTsMux->hXpt->hRegister, BCHP_XPT_PCROFFSET_STC_MUX_DELAY );
    Reg &= ~( 
        BCHP_MASK( XPT_PCROFFSET_STC_MUX_DELAY, MUX_DELAY ) |
        BCHP_MASK( XPT_PCROFFSET_STC_MUX_DELAY, STC_SEL )
    );

    UpdatePlaybackMap( hTsMux, NULL, BCHP_XPT_PB_TOP_BAND_PAUSE_MAP_VECT_PB0, MapAction_Refresh );
    if (pSettings->bAFAPMode) 
    {
        UpdatePlaybackMap( hTsMux, NULL, BCHP_XPT_PB_TOP_PACING_PAUSE_MAP_VECT_PB0, MapAction_Refresh );
        LoadPacingCount( hTsMux );
        LoadPacingSpeed( hTsMux );
        UpdatePlaybackConfig( hTsMux );
    }

    /* The MUX_DELAY bitfield is a 27 MHz domain, straight binary number. */
    Reg |= BCHP_FIELD_DATA( XPT_PCROFFSET_STC_MUX_DELAY, MUX_DELAY, pSettings->uiMuxDelay * 27000 );
    Reg |= BCHP_FIELD_DATA( XPT_PCROFFSET_STC_MUX_DELAY, STC_SEL, pSettings->hPcrOffset->WhichStc );
    BREG_Write32( hTsMux->hXpt->hRegister, BCHP_XPT_PCROFFSET_STC_MUX_DELAY, Reg );

    LoadStcMuxDelayDiff( hTsMux );
    Done:
    return BERR_SUCCESS;
#endif
}

void BXPT_TsMux_GetStatus(
    BXPT_TsMux_Handle hTsMux,               /* [in] Handle for the TsMux */
    BXPT_TsMux_Status *Status 		/* [out] Channel status. */
    )
{
#if 1
    BSTD_UNUSED( hTsMux );
    BSTD_UNUSED( Status );
#else
    #define STC_REG_STEP ( BCHP_XPT_PCROFFSET_STC1_HI - BCHP_XPT_PCROFFSET_STC0_HI )

    uint32_t Reg, RegAddr;
    uint64_t Temp;

    BDBG_ASSERT( hTsMux );
    BDBG_ASSERT( Status );

    RegAddr = BCHP_XPT_PCROFFSET_STC0_HI + hTsMux->Settings.hPcrOffset->WhichStc * STC_REG_STEP;
    Reg = BREG_Read32( hTsMux->hXpt->hRegister, RegAddr );
    Temp = BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_HI, COUNT );
    Status->uiSTC = Temp << 32;

    RegAddr = BCHP_XPT_PCROFFSET_STC0_LO + hTsMux->Settings.hPcrOffset->WhichStc * STC_REG_STEP;
    Reg = BREG_Read32( hTsMux->hXpt->hRegister, RegAddr );
    Status->uiSTC |= BCHP_GET_FIELD_DATA( Reg, XPT_PCROFFSET_STC0_LO, COUNT );

    /* Return the PACING_COUNTER value of the first PB channel that is registered
     * with TsMux.  All PB channels should have the same value, so we just use the
     * first one.
     */
    {
       unsigned Index;
       BXPT_Playback_MuxingInfo Info;

       for( Index = 0; Index < BXPT_NUM_PLAYBACKS; Index++ )
       {
           if( hTsMux->Playbacks[ Index ] )
           {
               BXPT_Playback_GetMuxingInfo( hTsMux->Playbacks[ Index ], &Info );
               Status->uiESCR = Info.uiPacingCounter;
               break;
           }
       }
       if ( BXPT_NUM_PLAYBACKS == Index )
       {
          BDBG_ERR(("No PB Channels associated with BXPT_TsMux.  ESCR Valid is not valid."));
          Status->uiESCR = 0;
       }
    }
#endif
}

void BXPT_Tsmux_ConfigDesc( 
    BXPT_PvrDescriptor8 *Desc,              /* [in] Descriptor to be configured */
    const BAVC_TsMux_DescConfig *Config     /* [in] Data to be loaded */
    )
{
    BDBG_ASSERT( Desc );
    BDBG_ASSERT( Config );
    
    Desc->Reserved0 = 0;
    Desc->MuxingFlags = Config->bRandomAccessIndication ? 1 << 2 : 0;	 
    if( Config->bNextPacketPacingTimestampValid )
    {
        Desc->MuxingFlags |= 1 << 1;	 
        Desc->NextPacketPacingTimestamp = Config->uiNextPacketPacingTimestamp;
    }
    if( Config->bPacket2PacketTimestampDeltaValid )
    {
        Desc->MuxingFlags |= 1;	 
        Desc->Pkt2PktPacingTimestampDelta = Config->uiPacket2PacketTimestampDelta;
    }
}


