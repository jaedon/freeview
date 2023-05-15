 /***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_mpod.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/1/12 10:00a $
 *
 * Porting interface code for the data transport core.
 *
 * Revision History:
 * 
 * $brcm_Log: /magnum/portinginterface/xpt/src/core28nm/bxpt_mpod.c $
 * 
 * Hydra_Software_Devel/1   10/1/12 10:00a gmullen
 * SW7445-17: Created core28nm file tree
 * 
 * Hydra_Software_Devel/6   12/12/11 2:03p gmullen
 * SW7425-1940: Force output clock rate to 27 MHz when in M-Card mode
 * 
 * Hydra_Software_Devel/5   8/22/11 5:45p gmullen
 * SW7231-319: Merged to Hydra branch
 * 
 * Hydra_Software_Devel/SW7231-319/3   8/22/11 5:16p gmullen
 * SW7231-319: Fixed compilation bugs and bad exit code check in
 * _AddPidChannel()
 * 
 * Hydra_Software_Devel/SW7231-319/2   8/18/11 11:26a gmullen
 * SW7231-319: Fixed compilation error
 * 
 * Hydra_Software_Devel/SW7231-319/1   8/16/11 2:15p gmullen
 * SW7231-319: Initial support for B0
 * 
 * Hydra_Software_Devel/4   4/15/11 4:49p gmullen
 * SW7425-313: Merged to mainline
 * 
 * Hydra_Software_Devel/SW7425-313/1   4/13/11 2:05p gmullen
 * SW7425-313: Added parser remapping support
 * 
 * Hydra_Software_Devel/3   10/28/10 6:01p gmullen
 * SW7425-15: Ported files
 * 
 * Hydra_Software_Devel/1   10/28/10 2:08p gmullen
 * SW7422-20: Checkin ported files
 * 
 * Hydra_Software_Devel/37   9/7/10 3:28p gmullen
 * SW7420-1044: Merged to Hydra_Software_Devel
 * 
 * Hydra_Software_Devel/SW7420-1044/1   9/7/10 3:20p gmullen
 * SW7420-1044: Added BXPT_Mpod_RouteToMpodPidFiltered()
 * 
 * Hydra_Software_Devel/36   9/16/09 10:46a gmullen
 * SW35230-2: Removed RMXP_MPOD_MUX
 * 
 * Hydra_Software_Devel/35   8/19/09 12:15p piyushg
 * PR56771: Add support for 7342. Hence the previous usage
 * of constant PARSER_REG_STEPSIZE does not work for this chip.
 * Added new function to calculate offset.
 * 
 * Hydra_Software_Devel/34   8/11/09 10:39a piyushg
 * PR55216: Added initial 7340 XPT support.
 * 
 * Hydra_Software_Devel/33   8/5/09 4:52p piyushg
 * PR55545: Add 7125 XPT PI support
 * Added file element "bxpt_rave_ihex.c".
 * Added file element "bxpt_rave_ihex.h".
 * 
 * Hydra_Software_Devel/32   7/31/09 3:22p piyushg
 * PR56771: Add support for 7342.
 * 
 * Hydra_Software_Devel/31   7/31/09 2:44p gmullen
 * PR54331: Added 35130 to XPT support.
 * 
 * Hydra_Software_Devel/30   7/14/09 10:21a piyushg
 * PR56771: Add XPT PI code for 7342.
 * 
 * Hydra_Software_Devel/29   6/17/09 6:46p gmullen
 * PR56110: Added support.xpt/7550/uif_image/a0
 * 
 * Hydra_Software_Devel/28   5/12/09 11:54a anilmm
 * PR54832:  Add support for 7413
 * 
 * Hydra_Software_Devel/27   4/7/09 5:25p piyushg
 * PR52986: Add support for 7635
 * Added directory element "7635".
 * 
 * Hydra_Software_Devel/26   1/27/09 1:09p gmullen
 * PR51625: Added 7336 support
 * 
 * Hydra_Software_Devel/25   11/26/08 4:14p gmullen
 * PR47755: Added support for 7420.
 * 
 * Hydra_Software_Devel/24   11/23/08 4:29p gmullen
 * PR49369: Initialized config struct in BXPT_Mpod_GetDefaultConfig()
 * 
 * Hydra_Software_Devel/23   10/20/08 11:25a gmullen
 * PR48130: Updated for MPOD register name change.
 * 
 * Hydra_Software_Devel/22   9/12/08 2:20p gmullen
 * PR46659: Added 3556/3548 B0 support
 * 
 * Hydra_Software_Devel/21   9/11/08 11:34a piyushg
 * PR44486: Input Interface Formatter Control Register being
 * initialized for 3556 and 3548.
 * 
 * Hydra_Software_Devel/20   7/7/08 11:32a gmullen
 * PR44486: Added support to control Pod2Chip MCLK select.
 * 
 * Hydra_Software_Devel/19   6/27/08 7:52a gmullen
 * PR42923: BXPT_Mpod_Init didn't access HOST_RSVD_EN fields.
 * 
 * Hydra_Software_Devel/19   6/27/08 7:51a gmullen
 * PR42923: #if ( BCHP_CHIP == 3563 ) || (( BCHP_CHIP == 7400 ) && (
 * BCHP_VER >= BCHP_VER_B0 )) || ( BCHP_CHIP == 7405 )  ||  ( BCHP_CHIP
 * == 7325 ) ||  ( BCHP_CHIP == 7335 ) ||  ( BCHP_CHIP == 3548 ) ||  (
 * BCHP_CHIP == 3556 )
 * 
 * Hydra_Software_Devel/18   5/21/08 6:20p gmullen
 * PR42923: Added support for S-Card mode. Updated comments for PbBand.
 * 
 * Hydra_Software_Devel/17   5/21/08 3:38p jrubio
 * PR42353: add 7325/7335 support to mpod
 * 
 * Hydra_Software_Devel/16   4/10/08 5:33p gmullen
 * PR41241: Added support for cardbus.
 * 
 * Hydra_Software_Devel/15   3/26/08 11:23a gmullen
 * PR38954: Added 3548 support to XPT PI.
 * 
 * Hydra_Software_Devel/14   12/19/07 8:58a gmullen
 * PR38184: Set MPOD_MUX_SEL only during init.
 * 
 * Hydra_Software_Devel/13   11/28/07 11:20a gmullen
 * PR36900: Added 7335 support
 * 
 * Hydra_Software_Devel/12   10/30/07 3:11p gmullen
 * PR35018: Added support for 7325
 * 
 * Hydra_Software_Devel/11   2/14/07 1:10p gmullen
 * PR27642: Added support for 7405.
 * 
 * Hydra_Software_Devel/10   2/6/07 7:05p gmullen
 * PR26217: Added B0 support.
 * 
 * Hydra_Software_Devel/9   1/18/07 4:55p katrep
 * PR27188: Enabled Parallel MPOD support for 7403
 * 
 * Hydra_Software_Devel/8   11/2/06 8:40a gmullen
 * PR23189: Added 3563 support.
 * 
 * Hydra_Software_Devel/7   10/19/06 1:43p gmullen
 * PR25027: Fixed compiler warning.
 * 
 * Hydra_Software_Devel/6   9/19/06 5:30p gmullen
 * PR24338: Parallel output support.,
 *
 * Hydra_Software_Devel/4   7/27/06 8:40a gmullen
 * PR22998: Incorp requested changes. Please see Description field in PR>
 *
 * Hydra_Software_Devel/3   4/12/06 10:15a gmullen
 * PR20631: Added parallel IO support
 *
 * Hydra_Software_Devel/2   3/31/06 4:12p gmullen
 * PR 15309: To use MPOD, parser band must be in all-pass mode.
 *
 * Hydra_Software_Devel/1   7/28/05 3:40p gmullen
 * PR15309: Initial version for building.
 *
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bxpt_priv.h"
#include "bxpt_mpod.h"

#include "bchp_xpt_mpod.h"
#include "bchp_xpt_fe.h"
#include "bchp_xpt_bus_if.h"

#include "bchp_xpt_mcpb_ch0.h"
#include "bchp_xpt_mcpb_ch1.h"
#include "bchp_xpt_memdma_mcpb_ch0.h"

#define PB_PARSER_STEP      ( BCHP_XPT_MCPB_CH1_DMA_DESC_CONTROL - BCHP_XPT_MCPB_CH0_DMA_DESC_CONTROL )
#define IB_PARSER_STEP      ( BCHP_XPT_FE_MINI_PID_PARSER1_CTRL1 - BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1 )

#if( BDBG_DEBUG_BUILD == 1 )
    BDBG_MODULE( xpt_mpod );
#endif

static BERR_Code SetOnlineState( BXPT_Handle hXpt, bool Online );

void BXPT_Mpod_GetDefaultConfig(
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    BXPT_Mpod_Config *Config
    )
{
    BDBG_ASSERT( hXpt );
    BDBG_ASSERT( Config );

    BSTD_UNUSED( hXpt );

    BKNI_Memset( Config, 0, sizeof( BXPT_Mpod_Config ));
    Config->ByteSync = 1;
    Config->ClkNrun = 0;
    Config->InvertClk = 0;
    Config->NshiftClk = 0;
    Config->OutputInvertSync = 0;
    Config->InputInvertSync = 0;

    Config->Loopback = false;

    Config->SmodeEn = false;
    Config->HostRsvd = 0;
    Config->HostRsvdEn = false;
    Config->ClkDelay = 0;
    Config->OutputInvertValid = false;
    Config->InputInvertValid = false;
    Config->InputInvertClk = false;

    Config->BandNo = 0;
    Config->PbBand = false;
    Config->BandEn = false;
    Config->TimestampInsertEn = false;

    Config->ParallelEn = false;

    /* Default for S-Card is 9 MHz. Not all chips support programmable output clocks. */
    Config->OutputClockRate = BXPT_Mpod_OutputClockRate_e108;
    Config->OutputClockDivider = BXPT_Mpod_OutputClockDivider_e12;
}

BERR_Code BXPT_Mpod_Init(
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    const BXPT_Mpod_Config *Config
    )
{
    uint32_t Reg;
    unsigned ii;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );
    BDBG_ASSERT( Config );

    /* MPOD Configuration Register */
    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_MPOD_CFG );
    Reg &= ~(
        BCHP_MASK( XPT_MPOD_CFG, CRC_INIT_VALUE ) |
        BCHP_MASK( XPT_MPOD_CFG, SMODE_EN ) |
        BCHP_MASK( XPT_MPOD_CFG, MPOD_EXT_EN ) |
        BCHP_MASK( XPT_MPOD_CFG, MPOD_EN )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_MPOD_CFG, CRC_INIT_VALUE, 0xFF ) |
        BCHP_FIELD_DATA( XPT_MPOD_CFG, SMODE_EN, Config->SmodeEn ) |
        BCHP_FIELD_DATA( XPT_MPOD_CFG, MPOD_EXT_EN, Config->Loopback == true ? 0 : 1 ) |
        BCHP_FIELD_DATA( XPT_MPOD_CFG, MPOD_EN, 0 )
    );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_MPOD_CFG, Reg );


    /* Output Interface Formatter Control Register */
    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_MPOD_OCTRL );
    Reg &= ~(
        BCHP_MASK( XPT_MPOD_OCTRL, HOST_RSVD ) |
        BCHP_MASK( XPT_MPOD_OCTRL, CLK_DELAY ) |
        BCHP_MASK( XPT_MPOD_OCTRL, HOST_RSVD_EN ) |
        BCHP_MASK( XPT_MPOD_OCTRL, INVERT_VALID ) |
        BCHP_MASK( XPT_MPOD_OCTRL, BYTE_SYNC ) |
        BCHP_MASK( XPT_MPOD_OCTRL, CLK_NRUN ) |
        BCHP_MASK( XPT_MPOD_OCTRL, INVERT_CLK ) |
        BCHP_MASK( XPT_MPOD_OCTRL, NSHIFT_CLK ) |
        BCHP_MASK( XPT_MPOD_OCTRL, INVERT_SYNC ) |
        BCHP_MASK( XPT_MPOD_OCTRL, MUTE ) |
        BCHP_MASK( XPT_MPOD_OCTRL, OUTPUT_FORMATTER_EN )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_MPOD_OCTRL, HOST_RSVD, Config->HostRsvd ) |
        BCHP_FIELD_DATA( XPT_MPOD_OCTRL, CLK_DELAY, Config->ClkDelay ) |
        BCHP_FIELD_DATA( XPT_MPOD_OCTRL, HOST_RSVD_EN, Config->HostRsvdEn ) |
        BCHP_FIELD_DATA( XPT_MPOD_OCTRL, INVERT_VALID, Config->OutputInvertValid ) |
        BCHP_FIELD_DATA( XPT_MPOD_OCTRL, BYTE_SYNC, Config->ByteSync ) |
        BCHP_FIELD_DATA( XPT_MPOD_OCTRL, CLK_NRUN, Config->ClkNrun ) |
        BCHP_FIELD_DATA( XPT_MPOD_OCTRL, INVERT_CLK, Config->InvertClk ) |
        BCHP_FIELD_DATA( XPT_MPOD_OCTRL, NSHIFT_CLK, Config->NshiftClk ) |
        BCHP_FIELD_DATA( XPT_MPOD_OCTRL, INVERT_SYNC, Config->OutputInvertSync ) |
        BCHP_FIELD_DATA( XPT_MPOD_OCTRL, MUTE, 0 ) |
        BCHP_FIELD_DATA( XPT_MPOD_OCTRL, OUTPUT_FORMATTER_EN, 0 )
    );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_MPOD_OCTRL, Reg );

    /* MPOD_PKT_DLY_CNT moved on some chips. */
#ifdef BCHP_XPT_MPOD_OCTRL2_MPOD_PKT_DLY_CNT_MASK
    {
        BXPT_Mpod_OutputClockRate OutputClockRate;
        BXPT_Mpod_OutputClockDivider OutputClockDivider;

        if( Config->SmodeEn == false )
        {
            /* The MCard spec requires 27 MHz output clock, so override the user's request if we're in MCard mode. */
            OutputClockRate = BXPT_Mpod_OutputClockRate_e108;
            OutputClockDivider = BXPT_Mpod_OutputClockDivider_e4;
        }
        else
        {
            OutputClockRate = Config->OutputClockRate;
            OutputClockDivider = Config->OutputClockDivider;
        }

        Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_MPOD_OCTRL2 );
        Reg &= ~(
            BCHP_MASK( XPT_MPOD_OCTRL2, MPOD_CLK_SEL ) |
            BCHP_MASK( XPT_MPOD_OCTRL2, MPOD_CLK_DIV_SEL ) |
            BCHP_MASK( XPT_MPOD_OCTRL2, MPOD_PKT_DLY_CNT ) 
        );
        Reg |= (
            BCHP_FIELD_DATA( XPT_MPOD_OCTRL2, MPOD_CLK_SEL, OutputClockRate ) |
            BCHP_FIELD_DATA( XPT_MPOD_OCTRL2, MPOD_CLK_DIV_SEL, OutputClockDivider ) |
            BCHP_FIELD_DATA( XPT_MPOD_OCTRL2, MPOD_PKT_DLY_CNT, Config->OutputPacketDelayCount ) 
        );
        BREG_Write32( hXpt->hRegister, BCHP_XPT_MPOD_OCTRL2, Reg );
    }
#endif

    /* Input Interface Formatter Control Register */
    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_MPOD_ICTRL );
    Reg &= ~(
        BCHP_MASK( XPT_MPOD_ICTRL, INVERT_VALID ) |
        BCHP_MASK( XPT_MPOD_ICTRL, INVERT_CLK ) |
        BCHP_MASK( XPT_MPOD_ICTRL, BAND_NO ) |
        BCHP_MASK( XPT_MPOD_ICTRL, PB_BAND ) |
        BCHP_MASK( XPT_MPOD_ICTRL, BAND_EN ) |
        BCHP_MASK( XPT_MPOD_ICTRL, TIMESTAMP_INSERT_EN ) |
        BCHP_MASK( XPT_MPOD_ICTRL, INVERT_SYNC ) |
        BCHP_MASK( XPT_MPOD_ICTRL, MUTE ) |
        BCHP_MASK( XPT_MPOD_ICTRL, INPUT_FORMATTER_EN )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_MPOD_ICTRL, INVERT_VALID, Config->InputInvertValid ) |
        BCHP_FIELD_DATA( XPT_MPOD_ICTRL, INVERT_CLK, Config->InputInvertClk ) |
        BCHP_FIELD_DATA( XPT_MPOD_ICTRL, BAND_NO, Config->BandNo ) |
        BCHP_FIELD_DATA( XPT_MPOD_ICTRL, PB_BAND, Config->PbBand ) |
        BCHP_FIELD_DATA( XPT_MPOD_ICTRL, BAND_EN, Config->BandEn ) |
        BCHP_FIELD_DATA( XPT_MPOD_ICTRL, TIMESTAMP_INSERT_EN, Config->TimestampInsertEn ) |
        BCHP_FIELD_DATA( XPT_MPOD_ICTRL, INVERT_SYNC, Config->InputInvertSync ) |
        BCHP_FIELD_DATA( XPT_MPOD_ICTRL, MUTE, 0 ) |
        BCHP_FIELD_DATA( XPT_MPOD_ICTRL, INPUT_FORMATTER_EN, 0 )
    );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_MPOD_ICTRL, Reg );

    /* All parsers *bypass* the interface */
    for( ii = 0; ii < hXpt->MaxPidParsers; ii++ )
    {
        BXPT_Mpod_RouteToMpod( hXpt, BXPT_ParserType_eIb, ii, false );
        BXPT_Mpod_AllPass( hXpt, BXPT_ParserType_eIb, ii, false );
    }
    for( ii = 0; ii < hXpt->MaxPlaybacks; ii++ )
    {
        BXPT_Mpod_RouteToMpod( hXpt, BXPT_ParserType_ePb, ii, false );
        BXPT_Mpod_AllPass( hXpt, BXPT_ParserType_ePb, ii, false );
    }

    /* Enable the interface. */
    ExitCode = SetOnlineState( hXpt, true );

    return( ExitCode );
}

BERR_Code BXPT_Mpod_Shutdown(
    BXPT_Handle hXpt                            /* [in] Handle for this transport */
    )
{
    unsigned ii;
    uint32_t Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );

    /* Disable the interface. */
    SetOnlineState( hXpt, false );

    /* All parsers *bypass* the interface */
    for( ii = 0; ii < hXpt->MaxPidParsers; ii++ )
    {
        BXPT_Mpod_RouteToMpod( hXpt, BXPT_ParserType_eIb, ii, false );
        BXPT_Mpod_AllPass( hXpt, BXPT_ParserType_eIb, ii, false );
    }
    for( ii = 0; ii < hXpt->MaxPlaybacks; ii++ )
    {
        BXPT_Mpod_RouteToMpod( hXpt, BXPT_ParserType_ePb, ii, false );
        BXPT_Mpod_AllPass( hXpt, BXPT_ParserType_ePb, ii, false );
    }

    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_MPOD_CFG );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_MPOD_CFG, Reg & 0xfffffff8);

    BREG_Write32( hXpt->hRegister, BCHP_XPT_MPOD_OCTRL, 0);
    BREG_Write32( hXpt->hRegister, BCHP_XPT_MPOD_ICTRL, 0);

    return( ExitCode );
}

unsigned int BXPT_Mpod_GetPodRes(
    BXPT_Handle hXpt                /* [in] Handle for this transport */
    )
{
    unsigned int Reg;

    BDBG_ASSERT( hXpt );

    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_MPOD_RES_FIELD );
    return BCHP_GET_FIELD_DATA( Reg, XPT_MPOD_RES_FIELD, POD_RES );
}

BERR_Code RouteMultichannelPlaybackToMpod(
    BXPT_Handle hXpt,           /* [in] Handle for this instance of transport. */
    unsigned ParserNum,         /* [in] Which parser to get data from */
    bool Enable                 /* [in] Route data to the MPOD interface if true */
    )
{
    uint32_t RegAddr, Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );

    RegAddr = BCHP_XPT_MCPB_CH0_SP_PARSER_CTRL + ParserNum * PB_PARSER_STEP;
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    Reg &= ~(
        BCHP_MASK( XPT_MEMDMA_MCPB_CH0_SP_PARSER_CTRL, MPOD_EN ) |
        BCHP_MASK( XPT_MEMDMA_MCPB_CH0_SP_PARSER_CTRL, PARSER_ALL_PASS_CTRL_PRE_MPOD )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_MEMDMA_MCPB_CH0_SP_PARSER_CTRL, MPOD_EN, Enable == true ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_MEMDMA_MCPB_CH0_SP_PARSER_CTRL, PARSER_ALL_PASS_CTRL_PRE_MPOD, Enable == true ? 1 : 0 )
    );
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );

    return ExitCode;
}

BERR_Code BXPT_Mpod_RouteToMpod(
    BXPT_Handle hXpt,           /* [in] Handle for this instance of transport. */
    BXPT_ParserType ParserType, /* [in] Playback or front-end parser */
    unsigned ParserNum,         /* [in] Which parser to get data from */
    bool Enable                 /* [in] Route data to the MPOD interface if true */
    )
{
    uint32_t Ctrl1Addr, Ctrl2Addr, Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );

    if( BXPT_ParserType_ePb == ParserType )
    {   
        return RouteMultichannelPlaybackToMpod( hXpt, ParserNum, Enable );
    }

    Ctrl1Addr = BXPT_P_GetParserCtrlRegAddr( hXpt, ParserNum, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1 );
    Ctrl2Addr = BXPT_P_GetParserCtrlRegAddr( hXpt, ParserNum, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL2 );
    if( !Ctrl1Addr || !Ctrl2Addr )
    {
        BDBG_ERR(( "Invalid ParserNum and ParserType combination!" ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
        goto Done;
    }

    Reg = BREG_Read32( hXpt->hRegister, Ctrl1Addr );
    Reg &= ~(
        BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ALL_PASS_CTRL_PRE_MPOD )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ALL_PASS_CTRL_PRE_MPOD, Enable == true ? 1 : 0 )
    );
    BREG_Write32( hXpt->hRegister, Ctrl1Addr, Reg );

    Reg = BREG_Read32( hXpt->hRegister, Ctrl2Addr );
    Reg &= ~(
        BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL2, MPOD_EN )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL2, MPOD_EN, Enable == true ? 1 : 0 )
    );
    BREG_Write32( hXpt->hRegister, Ctrl2Addr, Reg );

    Done:
    return ExitCode;
}

BERR_Code RouteMultichannelPlaybackToMpodPidFiltered(
    BXPT_Handle hXpt,           /* [in] Handle for this instance of transport. */
    unsigned ParserNum,         /* [in] Which parser to get data from */
    bool MpodPidFilter,         /* [in] enable pid filtering prior to the MCARD */
    bool Enable                 /* [in] Route data to the MPOD interface if true */
    )
{
    uint32_t RegAddr, Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );

    RegAddr = BCHP_XPT_MCPB_CH0_SP_PARSER_CTRL + ParserNum * PB_PARSER_STEP;
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    Reg &= ~(
        BCHP_MASK( XPT_MEMDMA_MCPB_CH0_SP_PARSER_CTRL, MPOD_EN ) |
        BCHP_MASK( XPT_MEMDMA_MCPB_CH0_SP_PARSER_CTRL, PARSER_ALL_PASS_CTRL_PRE_MPOD )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_MEMDMA_MCPB_CH0_SP_PARSER_CTRL, MPOD_EN, Enable == true ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_MEMDMA_MCPB_CH0_SP_PARSER_CTRL, PARSER_ALL_PASS_CTRL_PRE_MPOD, ((false == MpodPidFilter) && Enable) ? 1 : 0 )
    );
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );

    return ExitCode;
}

BERR_Code BXPT_Mpod_RouteToMpodPidFiltered(
    BXPT_Handle hXpt,           /* [in] Handle for this instance of transport. */
    BXPT_ParserType ParserType, /* [in] Playback or front-end parser */
    unsigned ParserNum,         /* [in] Which parser to get data from */
    bool MpodPidFilter,         /* [in] enable pid filtering prior to the MCARD */
    bool ContinuityCountCheck,  /* [in] enable CC checking after the MCARD */
    bool Enable                 /* [in] Route data to the MPOD interface if true */
    )
{
    uint32_t Ctrl1Addr, Ctrl2Addr, Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );

    BSTD_UNUSED( ContinuityCountCheck );
    BDBG_ERR(( "ContinuityCountCheck is not supported. Control CC checking through the PID channel API" ));

    if( BXPT_ParserType_ePb == ParserType )
    {   
        return RouteMultichannelPlaybackToMpodPidFiltered( hXpt, ParserNum, MpodPidFilter, Enable );
    }

    Ctrl1Addr = BXPT_P_GetParserCtrlRegAddr( hXpt, ParserNum, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL1 );
    Ctrl2Addr = BXPT_P_GetParserCtrlRegAddr( hXpt, ParserNum, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL2 );
    if( !Ctrl1Addr || !Ctrl2Addr )
    {
        BDBG_ERR(( "Invalid ParserNum and ParserType combination!" ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
        goto Done;
    }

    /* This works since PB and FE parser control reg 2 are identical */
    Reg = BREG_Read32( hXpt->hRegister, Ctrl1Addr );
    Reg &= ~(
        BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ALL_PASS_CTRL_PRE_MPOD )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL1, PARSER_ALL_PASS_CTRL_PRE_MPOD, ((MpodPidFilter==false) && Enable) ? 1 : 0 )
    );
    BREG_Write32( hXpt->hRegister, Ctrl1Addr, Reg );
    
    Reg = BREG_Read32( hXpt->hRegister, Ctrl2Addr );
    Reg &= ~(
        BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL2, MPOD_EN )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL2, MPOD_EN, Enable == true ? 1 : 0 )
    );
    BREG_Write32( hXpt->hRegister, Ctrl2Addr, Reg );

    Done:
    return ExitCode;
}

BERR_Code MultichannelPlaybackAllPassToMpod(
    BXPT_Handle hXpt,           /* [in] Handle for this instance of transport. */
    unsigned ParserNum,         /* [in] Which parser to get data from */
    bool Enable                 /* [in] All pass mode enabled if true, disabled if false */
    )
{
    uint32_t RegAddr, Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );

    RegAddr = BCHP_XPT_MCPB_CH0_SP_PARSER_CTRL + ParserNum * PB_PARSER_STEP;
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    Reg &= ~(
        BCHP_MASK( XPT_MEMDMA_MCPB_CH0_SP_PARSER_CTRL, PARSER_ALL_PASS_CTRL_POST_MPOD )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_MEMDMA_MCPB_CH0_SP_PARSER_CTRL, PARSER_ALL_PASS_CTRL_POST_MPOD, Enable == true ? 1 : 0 )
    );
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );

    return ExitCode;
}

BERR_Code BXPT_Mpod_AllPass(
    BXPT_Handle hXpt,           /* [in] Handle for this instance of transport. */
    BXPT_ParserType ParserType, /* [in] Playback or front-end parser */
    unsigned ParserNum,         /* [in] Which parser to get data from */
    bool Enable                 /* [in] All pass mode enabled if true, disabled if false */
    )
{
    uint32_t RegAddr, Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );

    if( BXPT_ParserType_ePb == ParserType )
    {   
        return MultichannelPlaybackAllPassToMpod( hXpt, ParserNum, Enable );
    }

    RegAddr = BXPT_P_GetParserCtrlRegAddr( hXpt, ParserNum, BCHP_XPT_FE_MINI_PID_PARSER0_CTRL2 );
    if( !RegAddr )
    {
        BDBG_ERR(( "Invalid ParserNum and ParserType combination!" ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
        goto Done;
    }

    /* This works since PB and FE parser control reg 2 are identical */
    Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    Reg &= ~(
        BCHP_MASK( XPT_FE_MINI_PID_PARSER0_CTRL2, PARSER_ALL_PASS_CTRL_POST_MPOD )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_FE_MINI_PID_PARSER0_CTRL2, PARSER_ALL_PASS_CTRL_POST_MPOD, Enable == true ? 1 : 0 )
    );
    BREG_Write32( hXpt->hRegister, RegAddr, Reg );

    Done:
    return ExitCode;
}

BERR_Code SetOnlineState(
    BXPT_Handle hXpt,             /* [in] Handle for this transport */
    bool Online                   /* [in] true if MPOD should be online, false otherwise */
    )
{
    int Enable;
    uint32_t Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hXpt );

    Enable = Online == true ? 1 : 0;

    /* MPOD Configuration Register */
    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_MPOD_CFG );
    Reg &= ~(
        BCHP_MASK( XPT_MPOD_CFG, MPOD_EN )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_MPOD_CFG, MPOD_EN, Enable )
    );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_MPOD_CFG, Reg );

    /* MPOD output interface register */
    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_MPOD_OCTRL );
    Reg &= ~(
        BCHP_MASK( XPT_MPOD_OCTRL, OUTPUT_FORMATTER_EN )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_MPOD_OCTRL, OUTPUT_FORMATTER_EN, Enable )
    );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_MPOD_OCTRL, Reg );

    /* MPOD input interface register */
    Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_MPOD_ICTRL );
    Reg &= ~(
        BCHP_MASK( XPT_MPOD_ICTRL, INPUT_FORMATTER_EN )
    );
    Reg |= (
        BCHP_FIELD_DATA( XPT_MPOD_ICTRL, INPUT_FORMATTER_EN, Enable )
    );
    BREG_Write32( hXpt->hRegister, BCHP_XPT_MPOD_ICTRL, Reg );

    return( ExitCode );
}


