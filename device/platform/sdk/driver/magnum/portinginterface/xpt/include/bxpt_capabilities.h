/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_capabilities.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 10/1/12 11:19a $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/include/bxpt_capabilities.h $
 * 
 * Hydra_Software_Devel/4   10/1/12 11:19a gmullen
 * SW7445-17: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7445-17/1   10/1/12 10:00a gmullen
 * SW7445-17: Created core28nm file tree
 * 
 * Hydra_Software_Devel/3   7/6/12 4:29p gmullen
 * SW7563-11: Added support
 * 
 * Hydra_Software_Devel/2   6/20/12 2:58p gmullen
 * SW7429-186: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7429-186/1   6/20/12 2:48p gmullen
 * SW7429-186: Added B0 support
 * 
 * Hydra_Software_Devel/1   6/18/12 11:27a gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 10:59a gmullen
 * SW7425-2914: Moved and updated files to new home
 * 
 *
 ***************************************************************************/

/*= Module Overview *********************************************************

Overview

***************************************************************************/

#ifndef BXPT_CAPABILITIES_H__
#define BXPT_CAPABILITIES_H__

#include "bchp.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
Used to define the number of each type of resource that is supported.
****************************************************************************/

/* Mostly chip-specific. */

#if( BCHP_CHIP == 7445 )
	#define BXPT_HAS_RMX_NULL_THRESHOLD			1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define	BXPT_HAS_TIMER_TICK					1
	#define	BXPT_HAS_SVC_MVC					1
	#define BXPT_HAS_BPP_SEARCH					1
	#define BXPT_HAS_EMM_RAVE					1
	#define BXPT_HAS_EXTENDED_ES_COUNT			1
	#define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
	#define BXPT_HAS_PCR_PACING					1

	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_REDUCED_TIMEREFS			1
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PARALLEL_MPOD				1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MPOD_OUTPUT_CLOCK_RATE		1
	#define BXPT_HAS_FULL_PID_PARSER			1
	#define BXPT_HAS_AVS    1
	#define BXPT_HAS_WAKEUP_PKT_SUPPORT			1
	#define BXPT_HAS_PIPELINE_ERROR_REPORTING	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_ATS	1
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
	#define BXPT_HAS_LOCAL_ATS	                1
#endif

    #define BXPT_HAS_TSMUX                      1
    #define BXPT_HAS_DPCRS						1
    #define BXPT_HAS_RMX_NULL_THRESHOLD         1
    
    #define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
    #define BXPT_HAS_PCR_PACING		 			1
    #define BXPT_HAS_MESG_BUFFERS				1
    #define BXPT_HAS_FIXED_RSBUF_CONFIG         1
    #define BXPT_HAS_FIXED_XCBUF_CONFIG         1
    #define BXPT_HAS_DIRECTV_SUPPORT			1
    #define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
    #define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
    #define BXPT_HAS_REMUX_PCR_OFFSET			1
    #define BXPT_HAS_MPOD_SCARD_SUPPORT			1
    #define BXPT_HAS_MOSAIC_SUPPORT				1
    #define BXPT_HAS_PID2BUF_MAPPING			1
    #define BXPT_HAS_IB_PID_PARSERS             1
    #define BXPT_HAS_REMUX                      1
    #define BXPT_HAS_PACKETSUB                  1
    #define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
    #define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
    #define BXPT_HAS_RAVE                       1
    #define BXPT_HAS_PLAYBACK_PARSERS           1
    #define BXPT_HAS_STARTCODE_BUFFER_WORKAROUND    1

    #define BXPT_HAS_MTSIF                      1
    #define BXPT_NUM_MTSIF                  ( 2 )
    #define BXPT_HAS_PARSER_REMAPPING           1
    #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 15 )
    
    /* These changes don't directly affect the API definition */
    #define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
    #define BXPT_SEPARATE_REMUX_IO				1
    #define BXPT_REC_SCD_MAPPING				1
    #define BXPT_NEXT_GEN_COMPARATORS			1
    #define BXPT_PER_CONTEXT_PIC_COUNTER		1
    #define BXPT_HAS_RAVE_PES_EXTENSIONS		1
    #define BXPT_PTS_CONTROL_IN_REC_REG			1
    			
    /* Number of filter banks, and the number of filters in each bank. */
    #ifdef  BXPT_FILTER_32
        #define BXPT_NUM_FILTER_BANKS         ( 2 )
    #else
        #define BXPT_NUM_FILTER_BANKS         ( 4 )
    #endif /*BXPT_FILTER_32 **/

	#define BXPT_MAX_EXTERNAL_TRIGS         ( 4 )

    #define BXPT_HAS_MPOD_RSBUF               1
    #define BXPT_NUM_INPUT_BANDS            ( 11 )
    
    /* There are 1024 channels defined, but the upper 256 are reserved for MEMDMA. 
    ** NOTE: The RDB incorrectly states that there are 1024 PID channels. 
    */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    #ifndef ENABLE_PLAYBACK_MUX
        #define ENABLE_PLAYBACK_MUX
    #endif
#endif
    #define BXPT_HAS_MEMDMA                     1
    #define BXPT_P_PID_TABLE_SIZE               ( 1024 )
    #define BXPT_NUM_PID_CHANNELS               ( 768 ) 
    #define BXPT_NUM_MEMDMA_PID_CHANNELS        ( BXPT_P_PID_TABLE_SIZE - BXPT_NUM_PID_CHANNELS )
    #if BXPT_HAS_MEMDMA && BXPT_NUM_MEMDMA_PID_CHANNELS <= 0
        #error "Internal error"
    #endif
    #define BXPT_P_MEMDMA_PID_CHANNEL_START     BXPT_NUM_PID_CHANNELS
    #define BXPT_P_HAS_SPID_EXTENSION_TABLE     1

    #define BXPT_HAS_MULTICHANNEL_PLAYBACK  1
    #define BXPT_NUM_PLAYBACKS              ( 32 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    #define BXPT_NUM_PACING_COUNTERS        ( 20 )
#endif

    #define BXPT_NUM_RAVE_CONTEXTS          ( 48 )
    #define BXPT_NUM_SCD                    ( 64 )
    #define BXPT_NUM_TPIT_PIDS              ( 32 )
    #define BXPT_NUM_TPIT                   ( 16 )
    #define BXPT_HAS_PB_PACKETSUB           1

    #define BXPT_NUM_PCRS                   ( 14 )

    #define BXPT_NUM_FILTERS_PER_BANK       ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_NUM_FILTERS_PER_BANK 
    #define BXPT_NUM_REMULTIPLEXORS         ( 2 )
    #define BXPT_NUM_PID_PARSERS            ( 24 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    #define BXPT_NUM_LEGACY_PID_PARSERS     ( 15 )
#endif
    #define BXPT_NUM_MESG_BUFFERS           ( 128 )
    #define BXPT_NUM_SAM_PID_CHANNEL        BXPT_NUM_MESG_BUFFERS
    #define BXPT_NUM_RAVE_CHANNELS          ( 1 )
    #define BXPT_NUM_CAP_FILTERS            ( 5 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_NUM_PACKETSUBS           ( 16 )
    #define BXPT_NUM_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_NUM_STCS                 ( 10 )
    #define STC_FREE_RUNNING              ( 15 )

    /* Values are in Mbps */
    #define BXPT_P_INITIAL_BUF_BITRATE    (30000000)

#elif( BCHP_CHIP == 7425 )
	#define BXPT_HAS_RMX_NULL_THRESHOLD			1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define	BXPT_HAS_TIMER_TICK					1
	#define	BXPT_HAS_SVC_MVC					1
	#define BXPT_HAS_BPP_SEARCH					1
	#define BXPT_HAS_EMM_RAVE					1
	#define BXPT_HAS_EXTENDED_ES_COUNT			1
	#define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
	#define BXPT_HAS_PCR_PACING					1

	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_REDUCED_TIMEREFS			1
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PARALLEL_MPOD				1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MPOD_OUTPUT_CLOCK_RATE		1
	#define BXPT_HAS_FULL_PID_PARSER			1
	#define BXPT_HAS_AVS    1
	#define BXPT_HAS_WAKEUP_PKT_SUPPORT			1
	#define BXPT_HAS_PIPELINE_ERROR_REPORTING	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_ATS	1
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
	#define BXPT_HAS_LOCAL_ATS	                1
#endif

    #if (BCHP_CHIP == 7425 && BCHP_VER <= BCHP_VER_C0)
    #define BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND	1
    #endif

    #define BXPT_HAS_TSMUX                      1
    #define BXPT_HAS_DPCRS						1
    #define BXPT_HAS_RMX_NULL_THRESHOLD         1
    
    #define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
    #define BXPT_HAS_PCR_PACING		 			1
    #define BXPT_HAS_MESG_BUFFERS				1
    #define BXPT_HAS_FIXED_RSBUF_CONFIG         1
    #define BXPT_HAS_FIXED_XCBUF_CONFIG         1
    #define BXPT_HAS_DIRECTV_SUPPORT			1
    #define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
    #define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
    #define BXPT_HAS_REMUX_PCR_OFFSET			1
    #define BXPT_HAS_MPOD_SCARD_SUPPORT			1
    #define BXPT_HAS_MOSAIC_SUPPORT				1
    #define BXPT_HAS_PID2BUF_MAPPING			1
    #define BXPT_HAS_IB_PID_PARSERS             1
    #define BXPT_HAS_REMUX                      1
    #define BXPT_HAS_PACKETSUB                  1
    #define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
    #define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
    #define BXPT_HAS_RAVE                       1
    #define BXPT_HAS_PLAYBACK_PARSERS           1
    #define BXPT_HAS_STARTCODE_BUFFER_WORKAROUND    1

    #define BXPT_HAS_MTSIF                      1
    #define BXPT_NUM_MTSIF                  ( 2 )
    #define BXPT_HAS_PARSER_REMAPPING           1
    #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 12 )
    
    /* These changes don't directly affect the API definition */
    #define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
    #define BXPT_SEPARATE_REMUX_IO				1
    #define BXPT_REC_SCD_MAPPING				1
    #define BXPT_NEXT_GEN_COMPARATORS			1
    #define BXPT_PER_CONTEXT_PIC_COUNTER		1
    #define BXPT_HAS_RAVE_PES_EXTENSIONS		1
    #define BXPT_PTS_CONTROL_IN_REC_REG			1
    			
    /* Number of filter banks, and the number of filters in each bank. */
    #ifdef  BXPT_FILTER_32
        #define BXPT_NUM_FILTER_BANKS         ( 2 )
    #else
        #define BXPT_NUM_FILTER_BANKS         ( 4 )
    #endif /*BXPT_FILTER_32 **/

	#define BXPT_MAX_EXTERNAL_TRIGS     ( 2 )

	#if BCHP_VER >= BCHP_VER_B0
	    #define BXPT_HAS_MPOD_RSBUF               1
	    #define BXPT_NUM_INPUT_BANDS            ( 11 )
	    #define BXPT_NUM_PID_CHANNELS           ( 512 ) 
	    #define BXPT_NUM_PLAYBACKS              ( 16 )
	    #define BXPT_NUM_RAVE_CONTEXTS          ( 48 )
	    #define BXPT_NUM_SCD                    ( 64 )
	    #define BXPT_NUM_TPIT_PIDS              ( 32 )
	    #define BXPT_NUM_TPIT                   ( 16 )
	    #define BXPT_HAS_PB_PACKETSUB           1

		#if BCHP_VER == BCHP_VER_B0
			#define BXPT_SW7425_1323_WORKAROUND		1
		#endif
	#else
	    #define BXPT_NUM_INPUT_BANDS            ( 10 )
	    #define BXPT_NUM_PID_CHANNELS           ( 256 ) 
	    #define BXPT_NUM_PLAYBACKS              ( 13 )
	    #define BXPT_NUM_RAVE_CONTEXTS          ( 24 )
	    #define BXPT_NUM_SCD                    ( 33 )
	    #define BXPT_NUM_TPIT_PIDS              ( 16 )
	    #define BXPT_NUM_TPIT                   ( 8 )
	#endif

    #define BXPT_NUM_PCRS                   ( 8 )

    #define BXPT_NUM_FILTERS_PER_BANK       ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_NUM_FILTERS_PER_BANK 
    #define BXPT_NUM_REMULTIPLEXORS         ( 2 )
    #define BXPT_NUM_PID_PARSERS            ( 16 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    #define BXPT_NUM_LEGACY_PID_PARSERS     ( 11 )
#endif
    #define BXPT_NUM_MESG_BUFFERS           ( 128 )
    #define BXPT_NUM_SAM_PID_CHANNEL        BXPT_NUM_MESG_BUFFERS
    #define BXPT_NUM_RAVE_CHANNELS          ( 1 )
    #define BXPT_NUM_CAP_FILTERS            ( 5 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_NUM_PACKETSUBS           ( 8 )
    #define BXPT_NUM_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_NUM_STCS                 ( 8 )
    #define STC_FREE_RUNNING              ( 8 )

    /* Values are in Mbps */
    #define BXPT_P_INITIAL_BUF_BITRATE    (30000000)

#elif( BCHP_CHIP == 7422 )
	#define BXPT_HAS_RMX_NULL_THRESHOLD			1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define	BXPT_HAS_TIMER_TICK					1
	#define	BXPT_HAS_SVC_MVC					1
	#define BXPT_HAS_BPP_SEARCH					1
	#define BXPT_HAS_EMM_RAVE					1
	#define BXPT_HAS_EXTENDED_ES_COUNT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_REDUCED_TIMEREFS			1
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PARALLEL_MPOD				1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MPOD_OUTPUT_CLOCK_RATE		1
	#define BXPT_HAS_FULL_PID_PARSER			1
	#define BXPT_HAS_AVS    1
	#define BXPT_HAS_WAKEUP_PKT_SUPPORT			1
	#define BXPT_HAS_PIPELINE_ERROR_REPORTING	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_ATS	1

    #define BXPT_HAS_DPCRS						1
    #define BXPT_HAS_RMX_NULL_THRESHOLD         1
    
    #define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
    #define BXPT_HAS_PCR_PACING		 			1
    #define BXPT_HAS_MESG_BUFFERS				1
    #define BXPT_HAS_FIXED_RSBUF_CONFIG         1
    #define BXPT_HAS_FIXED_XCBUF_CONFIG         1
    #define BXPT_HAS_DIRECTV_SUPPORT			1
    #define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
    #define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
    #define BXPT_HAS_REMUX_PCR_OFFSET			1
    #define BXPT_HAS_MPOD_SCARD_SUPPORT			1
    #define BXPT_HAS_MOSAIC_SUPPORT				1
    #define BXPT_HAS_PID2BUF_MAPPING			1
    #define BXPT_HAS_IB_PID_PARSERS             1
    #define BXPT_HAS_REMUX                      1
    #define BXPT_HAS_PACKETSUB                  1
    #define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
    #define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
    #define BXPT_HAS_RAVE                       1
    #define BXPT_HAS_PLAYBACK_PARSERS           1
    #define BXPT_HAS_STARTCODE_BUFFER_WORKAROUND    1
    
    /* These changes don't directly affect the API definition */
    #define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
    #define BXPT_SEPARATE_REMUX_IO				1
    #define BXPT_REC_SCD_MAPPING				1
    #define BXPT_NEXT_GEN_COMPARATORS			1
    #define BXPT_PER_CONTEXT_PIC_COUNTER		1
    #define BXPT_HAS_RAVE_PES_EXTENSIONS		1
    #define BXPT_PTS_CONTROL_IN_REC_REG			1
    			
    /* Number of filter banks, and the number of filters in each bank. */
    #ifdef  BXPT_FILTER_32
        #define BXPT_NUM_FILTER_BANKS         ( 2 )
    #else
        #define BXPT_NUM_FILTER_BANKS         ( 4 )
    #endif /*BXPT_FILTER_32 **/
    
    #define BXPT_HAS_MTSIF                      1
    #define BXPT_NUM_MTSIF                  ( 2 )
    #define BXPT_HAS_PARSER_REMAPPING           1
    #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 12 )

    #define BXPT_NUM_PCRS                   ( 8 )

    #define BXPT_NUM_FILTERS_PER_BANK       ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_NUM_FILTERS_PER_BANK 
    #define BXPT_NUM_TPIT_PIDS              ( 16 )
    #define BXPT_NUM_REMULTIPLEXORS         ( 2 )
    #define BXPT_NUM_PID_CHANNELS          ( 256 ) 
    #define BXPT_NUM_PID_PARSERS            ( 16 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    #define BXPT_NUM_LEGACY_PID_PARSERS     ( 11 )
#endif
    #define BXPT_NUM_MESG_BUFFERS           ( 128 )
    #define BXPT_NUM_SAM_PID_CHANNEL        BXPT_NUM_MESG_BUFFERS
    #define BXPT_NUM_RAVE_CHANNELS          ( 1 )
    #define BXPT_NUM_RAVE_CONTEXTS          ( 24 )
    #define BXPT_NUM_SCD                    ( 33 )
    #define BXPT_NUM_TPIT                   ( 8 )
    #define BXPT_NUM_CAP_FILTERS            ( 5 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_NUM_PACKETSUBS           ( 8 )
    #define BXPT_NUM_INPUT_BANDS          ( 10 )
    #define BXPT_NUM_PLAYBACKS            ( 13 )
    #define BXPT_NUM_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_NUM_STCS                 ( 4 )
    #define STC_FREE_RUNNING              ( 8 )

    /* Values are in Mbps */
    #define BXPT_P_INITIAL_BUF_BITRATE    (30000000)

#elif( BCHP_CHIP == 7435 )
	#define BXPT_HAS_RMX_NULL_THRESHOLD			1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define	BXPT_HAS_TIMER_TICK					1
	#define	BXPT_HAS_SVC_MVC					1
	#define BXPT_HAS_BPP_SEARCH					1
	#define BXPT_HAS_EMM_RAVE					1
	#define BXPT_HAS_EXTENDED_ES_COUNT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_REDUCED_TIMEREFS			1
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PARALLEL_MPOD				1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MPOD_OUTPUT_CLOCK_RATE		1
	#define BXPT_HAS_FULL_PID_PARSER			1
	#define BXPT_HAS_AVS    1
	#define BXPT_HAS_WAKEUP_PKT_SUPPORT			1
	#define BXPT_HAS_PIPELINE_ERROR_REPORTING	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_ATS	1
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
	#define BXPT_HAS_LOCAL_ATS	1
#endif

    #if (BCHP_CHIP == 7435 && BCHP_VER <= BCHP_VER_B0)
    #define BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND	1
    #endif

    #define BXPT_HAS_TSMUX                      1
    #define BXPT_HAS_DPCRS						1
    #define BXPT_HAS_RMX_NULL_THRESHOLD         1
    
    #define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
    #define BXPT_HAS_PCR_PACING		 			1
    #define BXPT_HAS_MESG_BUFFERS				1
    #define BXPT_HAS_FIXED_RSBUF_CONFIG         1
    #define BXPT_HAS_FIXED_XCBUF_CONFIG         1
    #define BXPT_HAS_DIRECTV_SUPPORT			1
    #define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
    #define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
    #define BXPT_HAS_REMUX_PCR_OFFSET			1
    #define BXPT_HAS_MPOD_SCARD_SUPPORT			1
    #define BXPT_HAS_MOSAIC_SUPPORT				1
    #define BXPT_HAS_PID2BUF_MAPPING			1
    #define BXPT_HAS_IB_PID_PARSERS             1
    #define BXPT_HAS_REMUX                      1
    #define BXPT_HAS_PACKETSUB                  1
    #define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
    #define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
    #define BXPT_HAS_RAVE                       1
    #define BXPT_HAS_PLAYBACK_PARSERS           1
    #define BXPT_HAS_STARTCODE_BUFFER_WORKAROUND    1
	#define BXPT_PCR_OFFSET_STC_SKIP			1

    #define BXPT_HAS_MTSIF                      1
    #define BXPT_NUM_MTSIF                  ( 2 )
    #define BXPT_HAS_PARSER_REMAPPING           1
    #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 12 )
    
    /* These changes don't directly affect the API definition */
    #define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
    #define BXPT_SEPARATE_REMUX_IO				1
    #define BXPT_REC_SCD_MAPPING				1
    #define BXPT_NEXT_GEN_COMPARATORS			1
    #define BXPT_PER_CONTEXT_PIC_COUNTER		1
    #define BXPT_HAS_RAVE_PES_EXTENSIONS		1
    #define BXPT_PTS_CONTROL_IN_REC_REG			1
    			
    /* Number of filter banks, and the number of filters in each bank. */
    #ifdef  BXPT_FILTER_32
        #define BXPT_NUM_FILTER_BANKS         ( 2 )
    #else
        #define BXPT_NUM_FILTER_BANKS         ( 4 )
    #endif /*BXPT_FILTER_32 **/

	#define BXPT_MAX_EXTERNAL_TRIGS     ( 4 )

    #define BXPT_HAS_MPOD_RSBUF               1
    #define BXPT_NUM_INPUT_BANDS            ( 11 )
    #define BXPT_NUM_PID_CHANNELS           ( 512 ) 
    #define BXPT_NUM_PLAYBACKS              ( 22 )
    #define BXPT_NUM_RAVE_CONTEXTS          ( 48 )
    #define BXPT_NUM_SCD                    ( 64 )
    #define BXPT_NUM_TPIT_PIDS              ( 32 )
    #define BXPT_NUM_TPIT                   ( 16 )

    #define BXPT_NUM_PCRS                   ( 10 )

    #define BXPT_NUM_FILTERS_PER_BANK       ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_NUM_FILTERS_PER_BANK 
    #define BXPT_NUM_REMULTIPLEXORS         ( 2 )
    #define BXPT_NUM_PID_PARSERS            ( 16 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    #define BXPT_NUM_LEGACY_PID_PARSERS     ( 11 )
#endif
    #define BXPT_NUM_MESG_BUFFERS           ( 128 )
    #define BXPT_NUM_SAM_PID_CHANNEL        BXPT_NUM_MESG_BUFFERS
    #define BXPT_NUM_RAVE_CHANNELS          ( 1 )
    #define BXPT_NUM_CAP_FILTERS            ( 5 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_NUM_PACKETSUBS           ( 8 )
    #define BXPT_NUM_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_NUM_STCS                 ( 8 )
    #define STC_FREE_RUNNING              ( 8 )

    /* Values are in Mbps */
    #define BXPT_P_INITIAL_BUF_BITRATE    (30000000)

#elif (BCHP_CHIP == 7358) || (BCHP_CHIP == 7552)
	#define BXPT_HAS_RMX_NULL_THRESHOLD			1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define	BXPT_HAS_TIMER_TICK					1
	#define	BXPT_HAS_SVC_MVC					1
	#define BXPT_HAS_BPP_SEARCH					1
	#define BXPT_HAS_EMM_RAVE					1
	#define BXPT_HAS_EXTENDED_ES_COUNT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_REDUCED_TIMEREFS			1
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PARALLEL_MPOD				1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MPOD_OUTPUT_CLOCK_RATE		1
	#define BXPT_HAS_FULL_PID_PARSER			1
	#define BXPT_HAS_AVS    1
	#define BXPT_HAS_WAKEUP_PKT_SUPPORT			1
	#define BXPT_HAS_PIPELINE_ERROR_REPORTING	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_ATS	1

    #if (BCHP_CHIP == 7358 && BCHP_VER <= BCHP_VER_B0) || (BCHP_CHIP == 7552 && BCHP_VER <= BCHP_VER_C0)
	    #define BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND	1
    #endif

    #define BXPT_HAS_DPCRS						1
    #define BXPT_HAS_RMX_NULL_THRESHOLD         1
    
    #define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
    #define BXPT_HAS_PCR_PACING		 			1
    #define BXPT_HAS_MESG_BUFFERS				1
    #define BXPT_HAS_FIXED_RSBUF_CONFIG         1
    #define BXPT_HAS_FIXED_XCBUF_CONFIG         1
    #define BXPT_HAS_DIRECTV_SUPPORT			1
    #define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
    #define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
    #define BXPT_HAS_REMUX_PCR_OFFSET			1
    #define BXPT_HAS_MPOD_SCARD_SUPPORT			1
    #define BXPT_HAS_MOSAIC_SUPPORT				1
    #define BXPT_HAS_PID2BUF_MAPPING			1
    #define BXPT_HAS_IB_PID_PARSERS             1
    #define BXPT_HAS_REMUX                      1
    #define BXPT_HAS_PACKETSUB                  1
    #define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
    #define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
    #define BXPT_HAS_RAVE                       1
    #define BXPT_HAS_PLAYBACK_PARSERS           1
    #define BXPT_HAS_STARTCODE_BUFFER_WORKAROUND    1

	#if BCHP_CHIP == 7552 && BCHP_VER >= BCHP_VER_B0
	    #define BXPT_NUM_INPUT_BANDS            ( 10 )
	    #define BXPT_NUM_PID_PARSERS            ( 16 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
	    #define BXPT_NUM_LEGACY_PID_PARSERS     ( 10 )
#endif
	    #define BXPT_HAS_MTSIF                      1
	    #define BXPT_NUM_MTSIF                  ( 2 )
	    #define BXPT_HAS_PARSER_REMAPPING           1
	    #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 10 )
	#else
	    #define BXPT_NUM_INPUT_BANDS            ( 5 )
	    #define BXPT_NUM_PID_PARSERS            ( 10 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
	    #define BXPT_NUM_LEGACY_PID_PARSERS     ( 7 )
	#endif
#endif

    /* These changes don't directly affect the API definition */
    #define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
    #define BXPT_SEPARATE_REMUX_IO				1
    #define BXPT_REC_SCD_MAPPING				1
    #define BXPT_NEXT_GEN_COMPARATORS			1
    #define BXPT_PER_CONTEXT_PIC_COUNTER		1
    #define BXPT_HAS_RAVE_PES_EXTENSIONS		1
    #define BXPT_PTS_CONTROL_IN_REC_REG			1
    			
    /* Number of filter banks, and the number of filters in each bank. */
    #ifdef  BXPT_FILTER_32
        #define BXPT_NUM_FILTER_BANKS         ( 2 )
    #else
        #define BXPT_NUM_FILTER_BANKS         ( 4 )
    #endif /*BXPT_FILTER_32 **/
    
    #define BXPT_NUM_PCRS                   ( 8 )

    #define BXPT_NUM_FILTERS_PER_BANK       ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_NUM_FILTERS_PER_BANK 
    #define BXPT_NUM_TPIT_PIDS              ( 16 )
    #define BXPT_NUM_REMULTIPLEXORS         ( 2 )
    #define BXPT_NUM_PID_CHANNELS           ( 256 ) 
    #define BXPT_NUM_MESG_BUFFERS           ( 128 )
    #define BXPT_NUM_SAM_PID_CHANNEL        BXPT_NUM_MESG_BUFFERS
    #define BXPT_NUM_RAVE_CHANNELS          ( 1 )
    #define BXPT_NUM_RAVE_CONTEXTS          ( 24 )
    #define BXPT_NUM_SCD                    ( 33 )
    #define BXPT_NUM_TPIT                   ( 8 )
    #define BXPT_NUM_CAP_FILTERS            ( 5 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_NUM_PACKETSUBS           ( 8 )
    #define BXPT_NUM_PLAYBACKS            ( 4 )
    #define BXPT_NUM_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_NUM_STCS                 ( 4 )
    #define STC_FREE_RUNNING              ( 8 )

    /* Values are in Mbps */
    #define BXPT_P_INITIAL_BUF_BITRATE    (30000000)

#elif (BCHP_CHIP == 7231) || (BCHP_CHIP == 7346)
	#define BXPT_HAS_RMX_NULL_THRESHOLD			1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define	BXPT_HAS_TIMER_TICK					1
	#define	BXPT_HAS_SVC_MVC					1
	#define BXPT_HAS_BPP_SEARCH					1
	#define BXPT_HAS_EMM_RAVE					1
	#define BXPT_HAS_EXTENDED_ES_COUNT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_REDUCED_TIMEREFS			1
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PARALLEL_MPOD				1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MPOD_OUTPUT_CLOCK_RATE		1
	#define BXPT_HAS_FULL_PID_PARSER			1
	#define BXPT_HAS_AVS    1
	#define BXPT_HAS_WAKEUP_PKT_SUPPORT			1
	#define BXPT_HAS_PIPELINE_ERROR_REPORTING	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_ATS	1

    #if (BCHP_CHIP == 7231 && BCHP_VER <= BCHP_VER_C0) || (BCHP_CHIP == 7346 && BCHP_VER <= BCHP_VER_C0)
	    #define BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND	1
    #endif

    #define BXPT_HAS_DPCRS						1
    #define BXPT_HAS_RMX_NULL_THRESHOLD         1
    
    #define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
    #define BXPT_HAS_PCR_PACING		 			1
    #define BXPT_HAS_MESG_BUFFERS				1
    #define BXPT_HAS_FIXED_RSBUF_CONFIG         1
    #define BXPT_HAS_FIXED_XCBUF_CONFIG         1
    #define BXPT_HAS_DIRECTV_SUPPORT			1
    #define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
    #define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
    #define BXPT_HAS_REMUX_PCR_OFFSET			1
    #define BXPT_HAS_MPOD_SCARD_SUPPORT			1
    #define BXPT_HAS_MOSAIC_SUPPORT				1
    #define BXPT_HAS_PID2BUF_MAPPING			1
    #define BXPT_HAS_IB_PID_PARSERS             1
    #define BXPT_HAS_REMUX                      1
    #define BXPT_HAS_PACKETSUB                  1
    #define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
    #define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
    #define BXPT_HAS_RAVE                       1
    #define BXPT_HAS_PLAYBACK_PARSERS           1
    #define BXPT_HAS_STARTCODE_BUFFER_WORKAROUND    1
    
    #define BXPT_HAS_PARSER_REMAPPING           1

    /* These changes don't directly affect the API definition */
    #define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
    #define BXPT_SEPARATE_REMUX_IO				1
    #define BXPT_REC_SCD_MAPPING				1
    #define BXPT_NEXT_GEN_COMPARATORS			1
    #define BXPT_PER_CONTEXT_PIC_COUNTER		1
    #define BXPT_HAS_RAVE_PES_EXTENSIONS		1
    #define BXPT_PTS_CONTROL_IN_REC_REG			1
    			
	#if BCHP_CHIP == 7231 && BCHP_VER >= BCHP_VER_B0
	    #define BXPT_HAS_TSMUX                      1
	    #define BXPT_NUM_INPUT_BANDS            ( 11 )
	    #define BXPT_NUM_PID_PARSERS            ( 16 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
	    #define BXPT_NUM_LEGACY_PID_PARSERS     ( 13 )
#endif
	    #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 16 )
	    #define BXPT_HAS_MTSIF                      1
	    #define BXPT_NUM_MTSIF                  ( 2 )
	    #define BXPT_NUM_STCS                   ( 8 )
	    #define BXPT_HAS_MPOD_RSBUF                 1
	    #define BXPT_NUM_PID_CHANNELS           ( 256 ) 
	    #define BXPT_NUM_PLAYBACKS              ( 6 )
	    #define BXPT_NUM_RAVE_CONTEXTS          ( 24 )
	    #define BXPT_NUM_SCD                    ( 33 )
	    #define BXPT_NUM_TPIT                   ( 8 )
	    #define BXPT_NUM_TPIT_PIDS              ( 32 )
	    #define BXPT_NUM_PCRS                   ( 4 )

		#if BCHP_VER == BCHP_VER_B0
			#define BXPT_SW7425_1323_WORKAROUND		1
		#endif
		#define BXPT_MAX_EXTERNAL_TRIGS     ( 2 )

	#elif BCHP_CHIP == 7346 && BCHP_VER >= BCHP_VER_B0
	    #define BXPT_HAS_TSMUX                      1
	    #define BXPT_NUM_INPUT_BANDS            ( 11 )
	    #define BXPT_NUM_PID_PARSERS            ( 16 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
	    #define BXPT_NUM_LEGACY_PID_PARSERS     ( 11 )
#endif
	    #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 12 )
	    #define BXPT_HAS_MTSIF                      1
	    #define BXPT_NUM_MTSIF                  ( 2 )
	    #define BXPT_NUM_STCS                   ( 8 )
	    #define BXPT_HAS_MPOD_RSBUF                 1
	    #define BXPT_NUM_PID_CHANNELS           ( 512 ) 
	    #define BXPT_NUM_PLAYBACKS              ( 16 )
	    #define BXPT_NUM_RAVE_CONTEXTS          ( 48 )
	    #define BXPT_NUM_SCD                    ( 64 )
	    #define BXPT_NUM_TPIT                   ( 16 )
	    #define BXPT_NUM_TPIT_PIDS              ( 32 )
	    #define BXPT_NUM_PCRS                   ( 8 )

		#if BCHP_VER == BCHP_VER_B0
			#define BXPT_SW7425_1323_WORKAROUND		1
		#endif
		#define BXPT_MAX_EXTERNAL_TRIGS     ( 2 )

	#else
	    #define BXPT_NUM_INPUT_BANDS            ( 8 )
	    #define BXPT_NUM_PID_PARSERS            ( 10 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
	    #define BXPT_NUM_LEGACY_PID_PARSERS     ( 10 )
#endif
	    #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 10 )
	    #define BXPT_NUM_STCS                   ( 4 )
	    #define BXPT_NUM_PID_CHANNELS           ( 256 ) 
	    #define BXPT_NUM_PLAYBACKS              ( 6 )
	    #define BXPT_NUM_RAVE_CONTEXTS          ( 24 )
	    #define BXPT_NUM_SCD                    ( 33 )
	    #define BXPT_NUM_TPIT                   ( 8 )
	    #define BXPT_NUM_TPIT_PIDS              ( 16 )
	    #define BXPT_NUM_PCRS                   ( 4 )
	#endif

    /* Number of filter banks, and the number of filters in each bank. */
    #ifdef  BXPT_FILTER_32
        #define BXPT_NUM_FILTER_BANKS         ( 2 )
    #else
        #define BXPT_NUM_FILTER_BANKS         ( 4 )
    #endif /*BXPT_FILTER_32 **/
    

    #define BXPT_NUM_FILTERS_PER_BANK       ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_NUM_FILTERS_PER_BANK 
    #define BXPT_NUM_REMULTIPLEXORS         ( 2 )
    #define BXPT_NUM_MESG_BUFFERS           ( 128 )
    #define BXPT_NUM_SAM_PID_CHANNEL        BXPT_NUM_MESG_BUFFERS
    #define BXPT_NUM_RAVE_CHANNELS          ( 1 )
    #define BXPT_NUM_CAP_FILTERS            ( 5 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_NUM_PACKETSUBS           ( 8 )
    #define BXPT_NUM_PCR_OFFSET_CHANNELS  ( 16 )
    #define STC_FREE_RUNNING              ( 8 )

    /* Values are in Mbps */
    #define BXPT_P_INITIAL_BUF_BITRATE    (30000000)

#elif (BCHP_CHIP == 7344)
	#define BXPT_HAS_RMX_NULL_THRESHOLD			1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define	BXPT_HAS_TIMER_TICK					1
	#define	BXPT_HAS_SVC_MVC					1
	#define BXPT_HAS_BPP_SEARCH					1
	#define BXPT_HAS_EMM_RAVE					1
	#define BXPT_HAS_EXTENDED_ES_COUNT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_REDUCED_TIMEREFS			1
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PARALLEL_MPOD				1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MPOD_OUTPUT_CLOCK_RATE		1
	#define BXPT_HAS_FULL_PID_PARSER			1
	#define BXPT_HAS_AVS    1
	#define BXPT_HAS_WAKEUP_PKT_SUPPORT			1
	#define BXPT_HAS_PIPELINE_ERROR_REPORTING	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_ATS	1

    #if (BCHP_CHIP == 7344 && BCHP_VER <= BCHP_VER_C0)
	    #define BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND	1
    #endif

    #define BXPT_HAS_DPCRS						1
    #define BXPT_HAS_RMX_NULL_THRESHOLD         1
    
    #define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
    #define BXPT_HAS_PCR_PACING		 			1
    #define BXPT_HAS_MESG_BUFFERS				1
    #define BXPT_HAS_FIXED_RSBUF_CONFIG         1
    #define BXPT_HAS_FIXED_XCBUF_CONFIG         1
    #define BXPT_HAS_DIRECTV_SUPPORT			1
    #define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
    #define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
    #define BXPT_HAS_REMUX_PCR_OFFSET			1
    #define BXPT_HAS_MPOD_SCARD_SUPPORT			1
    #define BXPT_HAS_MOSAIC_SUPPORT				1
    #define BXPT_HAS_PID2BUF_MAPPING			1
    #define BXPT_HAS_IB_PID_PARSERS             1
    #define BXPT_HAS_REMUX                      1
    #define BXPT_HAS_PACKETSUB                  1
    #define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
    #define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
    #define BXPT_HAS_RAVE                       1
    #define BXPT_HAS_PLAYBACK_PARSERS           1
    #define BXPT_HAS_STARTCODE_BUFFER_WORKAROUND    1
    
    #define BXPT_HAS_PARSER_REMAPPING           1

    /* These changes don't directly affect the API definition */
    #define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
    #define BXPT_SEPARATE_REMUX_IO				1
    #define BXPT_REC_SCD_MAPPING				1
    #define BXPT_NEXT_GEN_COMPARATORS			1
    #define BXPT_PER_CONTEXT_PIC_COUNTER		1
    #define BXPT_HAS_RAVE_PES_EXTENSIONS		1
    #define BXPT_PTS_CONTROL_IN_REC_REG			1
    			
    #if BCHP_VER >= BCHP_VER_B0
        #define BXPT_NUM_PID_PARSERS            ( 16 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
        #define BXPT_NUM_LEGACY_PID_PARSERS     ( 10 )
#endif
        #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 12 )
        #define BXPT_HAS_MTSIF                      1
        #define BXPT_NUM_MTSIF                  ( 2 )
    #else
        #define BXPT_NUM_PID_PARSERS            ( 10 )
        #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 10 )
    #endif

    /* Number of filter banks, and the number of filters in each bank. */
    #ifdef  BXPT_FILTER_32
        #define BXPT_NUM_FILTER_BANKS         ( 2 )
    #else
        #define BXPT_NUM_FILTER_BANKS         ( 4 )
    #endif /*BXPT_FILTER_32 **/
    
    #define BXPT_NUM_PCRS                   ( 4 )

    #define BXPT_NUM_FILTERS_PER_BANK       ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_NUM_FILTERS_PER_BANK 
    #define BXPT_NUM_TPIT_PIDS              ( 16 )
    #define BXPT_NUM_REMULTIPLEXORS         ( 2 )
    #define BXPT_NUM_PID_CHANNELS           ( 256 ) 
    #define BXPT_NUM_MESG_BUFFERS           ( 128 )
    #define BXPT_NUM_SAM_PID_CHANNEL        BXPT_NUM_MESG_BUFFERS
    #define BXPT_NUM_RAVE_CHANNELS          ( 1 )
    #define BXPT_NUM_RAVE_CONTEXTS          ( 24 )
    #define BXPT_NUM_SCD                    ( 33 )
    #define BXPT_NUM_TPIT                   ( 8 )
    #define BXPT_NUM_CAP_FILTERS            ( 5 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_NUM_PACKETSUBS           ( 8 )
    #define BXPT_NUM_INPUT_BANDS          ( 5 )
    #define BXPT_NUM_PLAYBACKS            ( 4 )
    #define BXPT_NUM_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_NUM_STCS                 ( 4 )
    #define STC_FREE_RUNNING              ( 8 )

    /* Values are in Mbps */
    #define BXPT_P_INITIAL_BUF_BITRATE    (30000000)

#elif( BCHP_CHIP == 7640 )
	#define BXPT_HAS_RMX_NULL_THRESHOLD			1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define	BXPT_HAS_TIMER_TICK					1
	#define	BXPT_HAS_SVC_MVC					1
	#define BXPT_HAS_BPP_SEARCH					1
	#define BXPT_HAS_EMM_RAVE					1
	#define BXPT_HAS_EXTENDED_ES_COUNT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_REDUCED_TIMEREFS			1
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PARALLEL_MPOD				1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MPOD_OUTPUT_CLOCK_RATE		1
	#define BXPT_HAS_FULL_PID_PARSER			1
	#define BXPT_HAS_AVS    1
	#define BXPT_HAS_WAKEUP_PKT_SUPPORT			1
	#define BXPT_HAS_PIPELINE_ERROR_REPORTING	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_ATS	1

    #define BXPT_HAS_DPCRS						1
    #define BXPT_HAS_RMX_NULL_THRESHOLD         1
    
    #define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
    #define BXPT_HAS_PCR_PACING		 			1
    #define BXPT_HAS_MESG_BUFFERS				1
    #define BXPT_HAS_FIXED_RSBUF_CONFIG         1
    #define BXPT_HAS_FIXED_XCBUF_CONFIG         1
    #define BXPT_HAS_DIRECTV_SUPPORT			1
    #define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
    #define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
    #define BXPT_HAS_REMUX_PCR_OFFSET			1
    #define BXPT_HAS_MPOD_SCARD_SUPPORT			1
    #define BXPT_HAS_MOSAIC_SUPPORT				1
    #define BXPT_HAS_PID2BUF_MAPPING			1
    #define BXPT_HAS_IB_PID_PARSERS             1
    #define BXPT_HAS_REMUX                      1
    #define BXPT_HAS_PACKETSUB                  1
    #define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
    #define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
    #define BXPT_HAS_RAVE                       1
    #define BXPT_HAS_PLAYBACK_PARSERS           1
    #define BXPT_HAS_STARTCODE_BUFFER_WORKAROUND    1
    
    #define BXPT_HAS_PARSER_REMAPPING           1
    #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 10 )

    /* These changes don't directly affect the API definition */
    #define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
    #define BXPT_SEPARATE_REMUX_IO				1
    #define BXPT_REC_SCD_MAPPING				1
    #define BXPT_NEXT_GEN_COMPARATORS			1
    #define BXPT_PER_CONTEXT_PIC_COUNTER		1
    #define BXPT_HAS_RAVE_PES_EXTENSIONS		1
    #define BXPT_PTS_CONTROL_IN_REC_REG			1
    			
    /* Number of filter banks, and the number of filters in each bank. */
    #ifdef  BXPT_FILTER_32
        #define BXPT_NUM_FILTER_BANKS         ( 2 )
    #else
        #define BXPT_NUM_FILTER_BANKS         ( 4 )
    #endif /*BXPT_FILTER_32 **/
    
    #define BXPT_NUM_PCRS                   ( 4 )

    #define BXPT_NUM_FILTERS_PER_BANK       ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_NUM_FILTERS_PER_BANK 
    #define BXPT_NUM_TPIT_PIDS              ( 16 )
    #define BXPT_NUM_REMULTIPLEXORS         ( 2 )
    #define BXPT_NUM_PID_CHANNELS          ( 256 ) 
    #define BXPT_NUM_PID_PARSERS            ( 10 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    #define BXPT_NUM_LEGACY_PID_PARSERS     ( 10 )
#endif
    #define BXPT_NUM_MESG_BUFFERS           ( 128 )
    #define BXPT_NUM_SAM_PID_CHANNEL        BXPT_NUM_MESG_BUFFERS
    #define BXPT_NUM_RAVE_CHANNELS          ( 1 )
    #define BXPT_NUM_RAVE_CONTEXTS          ( 24 )
    #define BXPT_NUM_SCD                    ( 33 )
    #define BXPT_NUM_TPIT                   ( 8 )
    #define BXPT_NUM_CAP_FILTERS            ( 5 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_NUM_PACKETSUBS           ( 8 )
    #define BXPT_NUM_INPUT_BANDS          ( 8 )
    #define BXPT_NUM_PLAYBACKS            ( 6 )
    #define BXPT_NUM_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_NUM_STCS                 ( 4 )
    #define STC_FREE_RUNNING              ( 8 )

    /* Values are in Mbps */
    #define BXPT_P_INITIAL_BUF_BITRATE    (30000000)

#elif (BCHP_CHIP == 7429) 
	#define BXPT_HAS_RMX_NULL_THRESHOLD			1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define	BXPT_HAS_TIMER_TICK					1
	#define	BXPT_HAS_SVC_MVC					1
	#define BXPT_HAS_BPP_SEARCH					1
	#define BXPT_HAS_EMM_RAVE					1
	#define BXPT_HAS_EXTENDED_ES_COUNT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_REDUCED_TIMEREFS			1
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PARALLEL_MPOD				1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MPOD_OUTPUT_CLOCK_RATE		1
	#define BXPT_HAS_FULL_PID_PARSER			1
	#define BXPT_HAS_AVS    1
	#define BXPT_HAS_WAKEUP_PKT_SUPPORT			1
	#define BXPT_HAS_PIPELINE_ERROR_REPORTING	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_ATS	1
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
	#define BXPT_HAS_LOCAL_ATS	1
#endif

    #if (BCHP_CHIP == 7429 && BCHP_VER < BCHP_VER_B0)
	    #define BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND	1
    #endif

    #define BXPT_HAS_DPCRS						1
    #define BXPT_HAS_RMX_NULL_THRESHOLD         1
    
    #define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
    #define BXPT_HAS_PCR_PACING		 			1
    #define BXPT_HAS_MESG_BUFFERS				1
    #define BXPT_HAS_FIXED_RSBUF_CONFIG         1
    #define BXPT_HAS_FIXED_XCBUF_CONFIG         1
    #define BXPT_HAS_DIRECTV_SUPPORT			1
    #define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
    #define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
    #define BXPT_HAS_REMUX_PCR_OFFSET			1
    #define BXPT_HAS_MPOD_SCARD_SUPPORT			1
    #define BXPT_HAS_MOSAIC_SUPPORT				1
    #define BXPT_HAS_PID2BUF_MAPPING			1
    #define BXPT_HAS_IB_PID_PARSERS             1
    #define BXPT_HAS_REMUX                      1
    #define BXPT_HAS_PACKETSUB                  1
    #define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
    #define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
    #define BXPT_HAS_RAVE                       1
    #define BXPT_HAS_PLAYBACK_PARSERS           1
    #define BXPT_HAS_STARTCODE_BUFFER_WORKAROUND    1
    
    #define BXPT_HAS_PARSER_REMAPPING           1

    /* These changes don't directly affect the API definition */
    #define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
    #define BXPT_SEPARATE_REMUX_IO				1
    #define BXPT_REC_SCD_MAPPING				1
    #define BXPT_NEXT_GEN_COMPARATORS			1
    #define BXPT_PER_CONTEXT_PIC_COUNTER		1
    #define BXPT_HAS_RAVE_PES_EXTENSIONS		1
    #define BXPT_PTS_CONTROL_IN_REC_REG			1
    			
    #define BXPT_HAS_TSMUX                      1
    #define BXPT_NUM_INPUT_BANDS            ( 11 )
    #define BXPT_NUM_PID_PARSERS            ( 16 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    #define BXPT_NUM_LEGACY_PID_PARSERS     ( 13 )
#endif
    #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 16 )
    #define BXPT_HAS_MTSIF                      1
    #define BXPT_NUM_MTSIF                  ( 2 )
    #define BXPT_NUM_STCS                   ( 8 )
    #define BXPT_HAS_MPOD_RSBUF                 1
    #define BXPT_NUM_PID_CHANNELS           ( 256 ) 
    #define BXPT_NUM_PLAYBACKS              ( 6 )
    #define BXPT_NUM_RAVE_CONTEXTS          ( 24 )
    #define BXPT_NUM_SCD                    ( 48 )
    #define BXPT_NUM_TPIT                   ( 8 )
    #define BXPT_NUM_TPIT_PIDS              ( 32 )
	#define BXPT_MAX_EXTERNAL_TRIGS     ( 2 )

    /* Number of filter banks, and the number of filters in each bank. */
    #ifdef  BXPT_FILTER_32
        #define BXPT_NUM_FILTER_BANKS         ( 2 )
    #else
        #define BXPT_NUM_FILTER_BANKS         ( 4 )
    #endif /*BXPT_FILTER_32 **/
    
    #define BXPT_NUM_PCRS                   ( 5 )
    #define BXPT_NUM_FILTERS_PER_BANK       ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_NUM_FILTERS_PER_BANK 
    #define BXPT_NUM_REMULTIPLEXORS         ( 2 )
    #define BXPT_NUM_MESG_BUFFERS           ( 128 )
    #define BXPT_NUM_SAM_PID_CHANNEL        BXPT_NUM_MESG_BUFFERS
    #define BXPT_NUM_RAVE_CHANNELS          ( 1 )
    #define BXPT_NUM_CAP_FILTERS            ( 5 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_NUM_PACKETSUBS           ( 8 )
    #define BXPT_NUM_PCR_OFFSET_CHANNELS  ( 16 )
    #define STC_FREE_RUNNING              ( 8 )

    /* Values are in Mbps */
    #define BXPT_P_INITIAL_BUF_BITRATE    (30000000)

#elif ( BCHP_CHIP == 7360 ) || ( BCHP_CHIP == 7563 )
	#define BXPT_HAS_RMX_NULL_THRESHOLD			1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define	BXPT_HAS_TIMER_TICK					1
	#define	BXPT_HAS_SVC_MVC					1
	#define BXPT_HAS_BPP_SEARCH					1
	#define BXPT_HAS_EMM_RAVE					1
	#define BXPT_HAS_EXTENDED_ES_COUNT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_REDUCED_TIMEREFS			1
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PARALLEL_MPOD				1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MPOD_OUTPUT_CLOCK_RATE		1
	#define BXPT_HAS_FULL_PID_PARSER			1
	#define BXPT_HAS_AVS    1
	#define BXPT_HAS_WAKEUP_PKT_SUPPORT			1
	#define BXPT_HAS_PIPELINE_ERROR_REPORTING	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_ATS	1

    #if (BCHP_CHIP == 7360 && BCHP_VER <= BCHP_VER_B0)
	    #define BXPT_HAS_DPCR_INTEGRATOR_WORKAROUND	1
    #endif

    #define BXPT_HAS_DPCRS						1
    #define BXPT_HAS_RMX_NULL_THRESHOLD         1
    
    #define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
    #define BXPT_HAS_PCR_PACING		 			1
    #define BXPT_HAS_MESG_BUFFERS				1
    #define BXPT_HAS_FIXED_RSBUF_CONFIG         1
    #define BXPT_HAS_FIXED_XCBUF_CONFIG         1
    #define BXPT_HAS_DIRECTV_SUPPORT			1
    #define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
    #define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
    #define BXPT_HAS_REMUX_PCR_OFFSET			1
    #define BXPT_HAS_MPOD_SCARD_SUPPORT			1
    #define BXPT_HAS_MOSAIC_SUPPORT				1
    #define BXPT_HAS_PID2BUF_MAPPING			1
    #define BXPT_HAS_IB_PID_PARSERS             1
    #define BXPT_HAS_REMUX                      1
    #define BXPT_HAS_PACKETSUB                  1
    #define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
    #define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
    #define BXPT_HAS_RAVE                       1
    #define BXPT_HAS_PLAYBACK_PARSERS           1
    #define BXPT_HAS_STARTCODE_BUFFER_WORKAROUND    1

    #define BXPT_HAS_MTSIF                      1
    #define BXPT_NUM_MTSIF                  ( 2 )
    #define BXPT_HAS_PARSER_REMAPPING           1
    #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 12 )
    
    /* These changes don't directly affect the API definition */
    #define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
    #define BXPT_SEPARATE_REMUX_IO				1
    #define BXPT_REC_SCD_MAPPING				1
    #define BXPT_NEXT_GEN_COMPARATORS			1
    #define BXPT_PER_CONTEXT_PIC_COUNTER		1
    #define BXPT_HAS_RAVE_PES_EXTENSIONS		1
    #define BXPT_PTS_CONTROL_IN_REC_REG			1
    			
    /* Number of filter banks, and the number of filters in each bank. */
    #ifdef  BXPT_FILTER_32
        #define BXPT_NUM_FILTER_BANKS         ( 2 )
    #else
        #define BXPT_NUM_FILTER_BANKS         ( 4 )
    #endif /*BXPT_FILTER_32 **/

    #define BXPT_NUM_INPUT_BANDS            ( 10 )
    #define BXPT_NUM_PID_CHANNELS           ( 256 ) 
    #define BXPT_NUM_PLAYBACKS              ( 4 )
    #define BXPT_NUM_RAVE_CONTEXTS          ( 24 )
    #define BXPT_NUM_SCD                    ( 33 )
    #define BXPT_NUM_TPIT_PIDS              ( 16 )
    #define BXPT_NUM_TPIT                   ( 8 )

    #define BXPT_NUM_PCRS                   ( 4 )

    #define BXPT_NUM_FILTERS_PER_BANK       ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_NUM_FILTERS_PER_BANK 
    #define BXPT_NUM_REMULTIPLEXORS         ( 2 )
    #define BXPT_NUM_PID_PARSERS            ( 16 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    #define BXPT_NUM_LEGACY_PID_PARSERS     ( 10 )
#endif
    #define BXPT_NUM_MESG_BUFFERS           ( 128 )
    #define BXPT_NUM_SAM_PID_CHANNEL        BXPT_NUM_MESG_BUFFERS
    #define BXPT_NUM_RAVE_CHANNELS          ( 1 )
    #define BXPT_NUM_CAP_FILTERS            ( 5 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_NUM_PACKETSUBS           ( 8 )
    #define BXPT_NUM_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_NUM_STCS                 ( 4 )
    #define STC_FREE_RUNNING              ( 8 )

    /* Values are in Mbps */
    #define BXPT_P_INITIAL_BUF_BITRATE    (30000000)

#elif (BCHP_CHIP == 7584) 
	#define BXPT_HAS_RMX_NULL_THRESHOLD			1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define	BXPT_HAS_TIMER_TICK					1
	#define	BXPT_HAS_SVC_MVC					1
	#define BXPT_HAS_BPP_SEARCH					1
	#define BXPT_HAS_EMM_RAVE					1
	#define BXPT_HAS_EXTENDED_ES_COUNT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_REDUCED_TIMEREFS			1
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PARALLEL_MPOD				1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MPOD_OUTPUT_CLOCK_RATE		1
	#define BXPT_HAS_FULL_PID_PARSER			1
	#define BXPT_HAS_AVS    1
	#define BXPT_HAS_WAKEUP_PKT_SUPPORT			1
	#define BXPT_HAS_PIPELINE_ERROR_REPORTING	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_ATS	1
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
	#define BXPT_HAS_LOCAL_ATS	1
#endif

    #define BXPT_HAS_DPCRS						1
    #define BXPT_HAS_RMX_NULL_THRESHOLD         1
    
    #define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
    #define BXPT_HAS_PCR_PACING		 			1
    #define BXPT_HAS_MESG_BUFFERS				1
    #define BXPT_HAS_FIXED_RSBUF_CONFIG         1
    #define BXPT_HAS_FIXED_XCBUF_CONFIG         1
    #define BXPT_HAS_DIRECTV_SUPPORT			1
    #define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
    #define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
    #define BXPT_HAS_REMUX_PCR_OFFSET			1
    #define BXPT_HAS_MPOD_SCARD_SUPPORT			1
    #define BXPT_HAS_MOSAIC_SUPPORT				1
    #define BXPT_HAS_PID2BUF_MAPPING			1
    #define BXPT_HAS_IB_PID_PARSERS             1
    #define BXPT_HAS_REMUX                      1
    #define BXPT_HAS_PACKETSUB                  1
    #define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
    #define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
    #define BXPT_HAS_RAVE                       1
    #define BXPT_HAS_PLAYBACK_PARSERS           1
    #define BXPT_HAS_STARTCODE_BUFFER_WORKAROUND    1
    #define BXPT_NUM_REMULTIPLEXORS         ( 2 )
    
    #define BXPT_HAS_PARSER_REMAPPING           1

    /* These changes don't directly affect the API definition */
    #define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
    #define BXPT_SEPARATE_REMUX_IO				1
    #define BXPT_REC_SCD_MAPPING				1
    #define BXPT_NEXT_GEN_COMPARATORS			1
    #define BXPT_PER_CONTEXT_PIC_COUNTER		1
    #define BXPT_HAS_RAVE_PES_EXTENSIONS		1
    #define BXPT_PTS_CONTROL_IN_REC_REG			1
    			
    #define BXPT_HAS_TSMUX                      1
    #define BXPT_NUM_INPUT_BANDS            ( 12 )
    #define BXPT_NUM_PID_PARSERS            ( 16 )
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    #define BXPT_NUM_LEGACY_PID_PARSERS     ( 13 )
#endif
    #define BXPT_NUM_REMAPPABLE_FE_PARSERS  ( 16 )
    #define BXPT_HAS_MTSIF                      1
    #define BXPT_NUM_MTSIF                  ( 2 )
    #define BXPT_NUM_STCS                   ( 8 )
    #define BXPT_HAS_MPOD_RSBUF                 1
    #define BXPT_NUM_PID_CHANNELS           ( 256 ) 
    #define BXPT_NUM_PLAYBACKS              ( 6 )
    #define BXPT_NUM_RAVE_CONTEXTS          ( 24 )
    #define BXPT_NUM_SCD                    ( 33 )
    #define BXPT_NUM_TPIT                   ( 8 )
    #define BXPT_NUM_TPIT_PIDS              ( 32 )
    #define BXPT_NUM_PCRS                   ( 4 )
	#define BXPT_MAX_EXTERNAL_TRIGS     	( 4 )

    /* Number of filter banks, and the number of filters in each bank. */
    #ifdef  BXPT_FILTER_32
        #define BXPT_NUM_FILTER_BANKS         ( 2 )
    #else
        #define BXPT_NUM_FILTER_BANKS         ( 4 )
    #endif /*BXPT_FILTER_32 **/

    #define BXPT_NUM_FILTERS_PER_BANK       ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_NUM_FILTERS_PER_BANK 
    #define BXPT_NUM_REMULTIPLEXORS         ( 2 )
    #define BXPT_NUM_MESG_BUFFERS           ( 128 )
    #define BXPT_NUM_SAM_PID_CHANNEL        BXPT_NUM_MESG_BUFFERS
    #define BXPT_NUM_RAVE_CHANNELS          ( 1 )
    #define BXPT_NUM_CAP_FILTERS            ( 5 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_NUM_PACKETSUBS           ( 8 )
    #define BXPT_NUM_PCR_OFFSET_CHANNELS  ( 16 )
    #define STC_FREE_RUNNING              ( 8 )

    /* Values are in Mbps */
    #define BXPT_P_INITIAL_BUF_BITRATE    (30000000)

#elif ( BCHP_CHIP == 7400 )
	#define BXPT_HAS_PSUB_IN_REMUX				1
	#define BXPT_HAS_STATIC_PID2BUF				1
	#define BXPT_HAS_DPCRS						1
	#define BXPT_HAS_REMUX_PID_REMAPPING		1
	#define BXPT_HAS_MESG_BUFFERS				1
	#define BXPT_HAS_RSBUF						1
	#define BXPT_HAS_XCBUF						1
	#define BXPT_HAS_DIRECTV_SUPPORT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1	  
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_IB_PID_PARSERS             1
	#define BXPT_HAS_REMUX                      1
	#define BXPT_HAS_PACKETSUB                  1

	#if  BCHP_VER >= BCHP_VER_B0
		#define BXPT_HAS_MOSAIC_SUPPORT				1
		#define BXPT_REC_SCD_MAPPING			1
		#define BXPT_PER_CONTEXT_PIC_COUNTER			1
	    #define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
	#endif

	#if BCHP_VER >= BCHP_VER_D0
		#define BXPT_HAS_PID2BUF_MAPPING			1
		#define BXPT_HAS_RAVE_PES_EXTENSIONS			1
		#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#endif

	#define BXPT_RAVE_AUDIO_STARTCODES          1

	    /* Number of filter banks, and the number of filters in each bank. */
	#ifdef  BXPT_FILTER_32
	    #define BXPT_P_MAX_FILTER_BANKS         ( 2 )
	#else
	    #define BXPT_P_MAX_FILTER_BANKS         ( 4 )
	#endif /*BXPT_FILTER_32 **/

    #define BXPT_P_MAX_FILTERS_PER_BANK     ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_P_MAX_FILTERS_PER_BANK 
    #define BXPT_P_MAX_TPIT_PIDS            ( 15 )
    #define BXPT_P_MAX_REMULTIPLEXORS       ( 2 )
    #define BXPT_P_MAX_PCRS                 ( 4 )
    #define BXPT_P_MAX_REMUX_PID_MAPS       ( 32 )     
    #define BXPT_P_MAX_REMUX_PSUB_TABLES    ( 2 )      
    #define BXPT_P_REMUX_SUB_TABLE_SIZE     ( 188 )
    #define BXPT_P_REMUX_PSUB_PAYLOAD_SIZE  ( 184 )
    #define BXPT_P_MAX_PID_CHANNELS         ( 256 ) 
    #define BXPT_P_MAX_PID_PARSERS          ( 7 )
    #define BXPT_P_MAX_MESG_BUFFERS         ( 128 )
    #define BXPT_P_MAX_SAM_PID_CHANNEL      BXPT_P_MAX_MESG_BUFFERS
    #define BXPT_P_MAX_RAVE_CHANNELS        ( 1 )
    #define BXPT_P_MAX_RAVE_CONTEXTS        ( 24 )
    #define BXPT_P_MAX_SCD                  ( 33 )

	#if BCHP_VER >= BCHP_VER_B0
	    #define BXPT_P_MAX_TPIT                 ( 6 )
	#else
	    #define BXPT_P_MAX_TPIT                 ( 5 )
	#endif
    #define BXPT_P_MAX_CAP_FILTERS          ( 5 )
    #define BXPT_P_MAX_PACKETIZERS_PER_PB   ( 4 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #if ( BCHP_VER == BCHP_VER_A0 )
        #define BXPT_P_MAX_PACKETSUBS           ( 5 )
        #define BXPT_P_MAX_INPUT_BANDS          ( 5 )
        #define BXPT_P_MAX_PLAYBACKS            ( 4 )
        #define BXPT_P_MAX_PCR_OFFSET_CHANNELS  ( 2 )
    #else
        #define BXPT_P_MAX_PACKETSUBS           ( 6 )
        #define BXPT_P_MAX_INPUT_BANDS          ( 6 )
        #define BXPT_P_MAX_PLAYBACKS            ( 5 )
        #define BXPT_P_MAX_PCR_OFFSET_CHANNELS  ( 16 )
        #define BXPT_P_MAX_STCS                 ( 4 )
    #endif

#elif ( BCHP_CHIP == 7405 ) 
	#define BXPT_HAS_PSUB_IN_REMUX				1
	#define BXPT_HAS_DPCRS						1
	#define BXPT_HAS_STATIC_PID2BUF				1

	#if( BCHP_VER >= BCHP_VER_B0 ) 
		#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL 	1
		#define BXPT_HAS_PID2BUF_MAPPING		 	1
		#define BXPT_NEXT_GEN_COMPARATORS			1
		#define BXPT_REC_SCD_MAPPING		 	1
		#define BXPT_PER_CONTEXT_PIC_COUNTER		 	1
		#define BXPT_PTS_CONTROL_IN_REC_REG			1
	    #define BXPT_HAS_AVS                    1
	    #define BXPT_HAS_32BIT_PB_TIMESTAMPS    1
	#endif

	#define BXPT_HAS_REMUX_PID_REMAPPING		1
	#define BXPT_HAS_MESG_BUFFERS				1
	#define BXPT_HAS_RSBUF						1
	#define BXPT_HAS_XCBUF						1
	#define BXPT_HAS_DIRECTV_SUPPORT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_REMUX                      1
	#define BXPT_HAS_PACKETSUB                  1

	#define BXPT_HAS_STARTCODE_BUFFER_WORKAROUND    1
	#define BXPT_REC_SCD_MAPPING				1
	#define BXPT_HAS_RAVE_PES_EXTENSIONS		1
	#define BXPT_HAS_IB_PID_PARSERS             1
	#define BXPT_2_BYTE_MINIMUM_PLAYBACK_BUFFER    1
		 
	    /* Number of filter banks, and the number of filters in each bank. */
	#ifdef  BXPT_FILTER_32
	    #define BXPT_P_MAX_FILTER_BANKS         ( 2 )
	#else
	    #define BXPT_P_MAX_FILTER_BANKS         ( 4 )
	#endif /*BXPT_FILTER_32 **/

    #define BXPT_P_MAX_FILTERS_PER_BANK     ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_P_MAX_FILTERS_PER_BANK 
    #define BXPT_P_MAX_TPIT_PIDS            ( 15 )
    #define BXPT_P_MAX_REMULTIPLEXORS       ( 2 )
    #define BXPT_P_MAX_PCRS                 ( 4 )
    #define BXPT_P_MAX_REMUX_PID_MAPS       ( 32 )     
    #define BXPT_P_MAX_REMUX_PSUB_TABLES    ( 2 )      
    #define BXPT_P_REMUX_SUB_TABLE_SIZE     ( 188 )
    #define BXPT_P_REMUX_PSUB_PAYLOAD_SIZE  ( 184 )
    #define BXPT_P_MAX_PID_CHANNELS         ( 256 ) 
    #define BXPT_P_MAX_PID_PARSERS          ( 7 )
    #define BXPT_P_MAX_MESG_BUFFERS         ( 128 )
    #define BXPT_P_MAX_SAM_PID_CHANNEL      BXPT_P_MAX_MESG_BUFFERS
    #define BXPT_P_MAX_RAVE_CHANNELS        ( 1 )
    #define BXPT_P_MAX_RAVE_CONTEXTS        ( 24 )
    #define BXPT_P_MAX_SCD                  ( 33 )
    #define BXPT_P_MAX_TPIT                 ( 5 )
    #define BXPT_P_MAX_CAP_FILTERS          ( 5 )
    #define BXPT_P_MAX_PACKETIZERS_PER_PB   ( 4 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_P_MAX_PACKETSUBS           ( 6 )
    #define BXPT_P_MAX_INPUT_BANDS          ( 6 )
    #define BXPT_P_MAX_PLAYBACKS            ( 5 )
    #define BXPT_P_MAX_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_P_MAX_STCS                 ( 4 )

#elif ( BCHP_CHIP == 7325 )
	#define BXPT_HAS_DPCRS						1
	#define BXPT_HAS_STATIC_PID2BUF				1

	#if( BCHP_VER >= BCHP_VER_B0 )
		#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL 1
	#endif
	#define BXPT_HAS_MESG_BUFFERS				1
	#define BXPT_HAS_RSBUF						1
	#define BXPT_HAS_XCBUF						1
	#define BXPT_HAS_DIRECTV_SUPPORT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_PID2BUF_MAPPING			1
	#define BXPT_HAS_REMUX                      1
	#define BXPT_HAS_PACKETSUB                  1

	/* These defines don't directly affect the API */
	#define BXPT_SEPARATE_REMUX_IO				1
	#define BXPT_REC_SCD_MAPPING				1
	#define BXPT_NEXT_GEN_COMPARATORS			1
	#define BXPT_PER_CONTEXT_PIC_COUNTER		1
	#define BXPT_HAS_RAVE_PES_EXTENSIONS		1
	#define BXPT_PTS_CONTROL_IN_REC_REG			1
	#define BXPT_HAS_IB_PID_PARSERS             1
	#define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
	#define BXPT_2_BYTE_MINIMUM_PLAYBACK_BUFFER    1

	    /* Number of filter banks, and the number of filters in each bank. */
	#ifdef  BXPT_FILTER_32
	    #define BXPT_P_MAX_FILTER_BANKS         ( 2 )
	#else
	    #define BXPT_P_MAX_FILTER_BANKS         ( 4 )
	#endif /*BXPT_FILTER_32 **/

    #define BXPT_P_MAX_FILTERS_PER_BANK     ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_P_MAX_FILTERS_PER_BANK 
    #define BXPT_P_MAX_TPIT_PIDS            ( 15 )
    #define BXPT_P_MAX_REMULTIPLEXORS       ( 2 )
    #define BXPT_P_MAX_PCRS                 ( 2 )
    #define BXPT_P_MAX_PID_CHANNELS         ( 256 ) 
    #define BXPT_P_MAX_PID_PARSERS          ( 7 )
    #define BXPT_P_MAX_MESG_BUFFERS         ( 128 )
    #define BXPT_P_MAX_SAM_PID_CHANNEL      BXPT_P_MAX_MESG_BUFFERS
    #define BXPT_P_MAX_RAVE_CHANNELS        ( 1 )
    #define BXPT_P_MAX_RAVE_CONTEXTS        ( 24 )
    #define BXPT_P_MAX_SCD                  ( 33 )
    #define BXPT_P_MAX_TPIT                 ( 5 )
    #define BXPT_P_MAX_CAP_FILTERS          ( 5 )
    #define BXPT_P_MAX_PACKETIZERS_PER_PB   ( 4 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_P_MAX_PACKETSUBS           ( 6 )
    #define BXPT_P_MAX_INPUT_BANDS          ( 6 )
    #define BXPT_P_MAX_PLAYBACKS            ( 2 )
    #define BXPT_P_MAX_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_P_MAX_STCS                 ( 4 )

#elif ( BCHP_CHIP == 7335 ) || ( BCHP_CHIP == 7336  )
	#define BXPT_HAS_STATIC_PID2BUF				1
	#define BXPT_HAS_DPCRS						1

	#if (BCHP_CHIP == 7335 && BCHP_VER >= BCHP_VER_B0 ) || ( BCHP_CHIP == 7336  )
		#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#endif

	#if BCHP_CHIP == 7336
	    #define BXPT_HAS_AVS                    1
	#endif

	#define BXPT_HAS_MESG_BUFFERS				1
	#define BXPT_HAS_RSBUF						1
	#define BXPT_HAS_XCBUF						1
	#define BXPT_HAS_DIRECTV_SUPPORT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_PID2BUF_MAPPING			1
	#define BXPT_HAS_IB_PID_PARSERS             1
	#define BXPT_HAS_REMUX                      1
	#define BXPT_HAS_PACKETSUB                  1

	/* These defines don't directly affect the API */
	#define BXPT_SEPARATE_REMUX_IO				1
	#define BXPT_REC_SCD_MAPPING				1
	#define BXPT_NEXT_GEN_COMPARATORS			1
	#define BXPT_PER_CONTEXT_PIC_COUNTER		1
	#define BXPT_HAS_RAVE_PES_EXTENSIONS		1
	#define BXPT_PTS_CONTROL_IN_REC_REG			1
	#define BXPT_2_BYTE_MINIMUM_PLAYBACK_BUFFER    1

	    /* Number of filter banks, and the number of filters in each bank. */
	#ifdef  BXPT_FILTER_32
	    #define BXPT_P_MAX_FILTER_BANKS         ( 2 )
	#else
	    #define BXPT_P_MAX_FILTER_BANKS         ( 4 )
	#endif /*BXPT_FILTER_32 **/

    #define BXPT_P_MAX_FILTERS_PER_BANK     ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_P_MAX_FILTERS_PER_BANK 
    #define BXPT_P_MAX_TPIT_PIDS            ( 15 )
    #define BXPT_P_MAX_REMULTIPLEXORS       ( 2 )
    #define BXPT_P_MAX_PCRS                 ( 4 )
    #define BXPT_P_MAX_PID_CHANNELS         ( 256 ) 
    #define BXPT_P_MAX_PID_PARSERS          ( 7 )
    #define BXPT_P_MAX_MESG_BUFFERS         ( 128 )
    #define BXPT_P_MAX_SAM_PID_CHANNEL      BXPT_P_MAX_MESG_BUFFERS
    #define BXPT_P_MAX_RAVE_CHANNELS        ( 1 )
    #define BXPT_P_MAX_RAVE_CONTEXTS        ( 24 )
    #define BXPT_P_MAX_SCD                  ( 33 )
    #define BXPT_P_MAX_TPIT                 ( 5 )
    #define BXPT_P_MAX_CAP_FILTERS          ( 5 )
    #define BXPT_P_MAX_PACKETIZERS_PER_PB   ( 4 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_P_MAX_PACKETSUBS           ( 6 )
    #define BXPT_P_MAX_INPUT_BANDS          ( 6 )
    #define BXPT_P_MAX_PLAYBACKS            ( 5 )
    #define BXPT_P_MAX_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_P_MAX_STCS                 ( 4 )

#elif ( BCHP_CHIP == 7342 )
	#define BXPT_HAS_STATIC_PID2BUF				1
	#define BXPT_HAS_DPCRS						1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define BXPT_HAS_MESG_BUFFERS				1
	#define BXPT_HAS_RSBUF						1
	#define BXPT_HAS_XCBUF						1
	#define BXPT_HAS_DIRECTV_SUPPORT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_PID2BUF_MAPPING			1
	#define BXPT_HAS_IB_PID_PARSERS             1
	#define BXPT_HAS_REMUX                      1
	#define BXPT_HAS_PACKETSUB                  1
	#define BXPT_HAS_AVS                        1
	#define BXPT_2_BYTE_MINIMUM_PLAYBACK_BUFFER    1

	/* These defines don't directly affect the API */
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_SEPARATE_REMUX_IO				1
	#define BXPT_REC_SCD_MAPPING				1
	#define BXPT_NEXT_GEN_COMPARATORS			1
	#define BXPT_PER_CONTEXT_PIC_COUNTER		1
	#define BXPT_HAS_RAVE_PES_EXTENSIONS		1
	#define BXPT_PTS_CONTROL_IN_REC_REG			1
	#define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1

	    /* Number of filter banks, and the number of filters in each bank. */
	#ifdef  BXPT_FILTER_32
	    #define BXPT_P_MAX_FILTER_BANKS         ( 2 )
	#else
	    #define BXPT_P_MAX_FILTER_BANKS         ( 4 )
	#endif /*BXPT_FILTER_32 **/

    #define BXPT_P_MAX_FILTERS_PER_BANK     ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_P_MAX_FILTERS_PER_BANK 
    #define BXPT_P_MAX_TPIT_PIDS            ( 15 )
    #define BXPT_P_MAX_REMULTIPLEXORS       ( 2 )
    #define BXPT_P_MAX_PCRS                 ( 7 )
    #define BXPT_P_MAX_PID_CHANNELS         ( 256 ) 
    #define BXPT_P_MAX_PID_PARSERS          ( 6 )
    #define BXPT_P_MAX_MESG_BUFFERS         ( 128 )
    #define BXPT_P_MAX_SAM_PID_CHANNEL      BXPT_P_MAX_MESG_BUFFERS
    #define BXPT_P_MAX_RAVE_CHANNELS        ( 1 )
    #define BXPT_P_MAX_RAVE_CONTEXTS        ( 24 )
    #define BXPT_P_MAX_SCD                  ( 33 )
    #define BXPT_P_MAX_TPIT                 ( 8 )
    #define BXPT_P_MAX_CAP_FILTERS          ( 5 )
    #define BXPT_P_MAX_PACKETIZERS_PER_PB   ( 4 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_P_MAX_PACKETSUBS           ( 8 )
    #define BXPT_P_MAX_INPUT_BANDS          ( 8 )

	#if (BCHP_VER == BCHP_VER_A0) 
	    #define BXPT_P_MAX_PLAYBACKS ( 2 ) 
	#else 
	    #define BXPT_P_MAX_PLAYBACKS ( 5 ) 
	#endif 

    #define BXPT_P_MAX_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_P_MAX_STCS                 ( 4 )

#elif ( BCHP_CHIP == 7340 )
	#define BXPT_HAS_STATIC_PID2BUF				1
	#define BXPT_HAS_DPCRS						1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL 1
	#define BXPT_HAS_MESG_BUFFERS				1
	#define BXPT_HAS_RSBUF						1
	#define BXPT_HAS_XCBUF						1
	#define BXPT_HAS_DIRECTV_SUPPORT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_PID2BUF_MAPPING			1
	#define BXPT_HAS_IB_PID_PARSERS             1
	#define BXPT_HAS_REMUX                      1
	#define BXPT_HAS_PACKETSUB                  1
	#define BXPT_2_BYTE_MINIMUM_PLAYBACK_BUFFER    1

	/* These defines don't directly affect the API */
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_SEPARATE_REMUX_IO				1
	#define BXPT_REC_SCD_MAPPING				1
	#define BXPT_NEXT_GEN_COMPARATORS			1
	#define BXPT_PER_CONTEXT_PIC_COUNTER		1
	#define BXPT_HAS_RAVE_PES_EXTENSIONS		1
	#define BXPT_PTS_CONTROL_IN_REC_REG			1
	#define BXPT_PLAYBACK_TS_ERROR_BOUNDS		1
	#define BXPT_HAS_PCR_PACING		 			1

	    /* Number of filter banks, and the number of filters in each bank. */
	#ifdef  BXPT_FILTER_32
	    #define BXPT_P_MAX_FILTER_BANKS         ( 2 )
	#else
	    #define BXPT_P_MAX_FILTER_BANKS         ( 4 )
	#endif /*BXPT_FILTER_32 **/

    #define BXPT_P_MAX_FILTERS_PER_BANK     ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_P_MAX_FILTERS_PER_BANK 
    #define BXPT_P_MAX_TPIT_PIDS            ( 15 )
    #define BXPT_P_MAX_REMULTIPLEXORS       ( 2 )
    #define BXPT_P_MAX_PCRS                 ( 6 )
    #define BXPT_P_MAX_PID_CHANNELS         ( 256 ) 
    #define BXPT_P_MAX_PID_PARSERS          ( 6 )
    #define BXPT_P_MAX_MESG_BUFFERS         ( 128 )
    #define BXPT_P_MAX_SAM_PID_CHANNEL      BXPT_P_MAX_MESG_BUFFERS
    #define BXPT_P_MAX_RAVE_CHANNELS        ( 1 )
    #define BXPT_P_MAX_RAVE_CONTEXTS        ( 24 )
    #define BXPT_P_MAX_SCD                  ( 33 )
    #define BXPT_P_MAX_TPIT                 ( 8 )
    #define BXPT_P_MAX_CAP_FILTERS          ( 5 )
    #define BXPT_P_MAX_PACKETIZERS_PER_PB   ( 4 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_P_MAX_PACKETSUBS           ( 8 )
    #define BXPT_P_MAX_INPUT_BANDS          ( 7 )
    #define BXPT_P_MAX_PLAYBACKS            ( 2 )
    #define BXPT_P_MAX_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_P_MAX_STCS                 ( 4 )

#elif ( BCHP_CHIP == 7125 )
	#define BXPT_HAS_STATIC_PID2BUF				1
	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define BXPT_HAS_MESG_BUFFERS				1
	#define BXPT_HAS_RSBUF						1
	#define BXPT_HAS_XCBUF						1
	#define BXPT_HAS_DIRECTV_SUPPORT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_PID2BUF_MAPPING			1
	#define BXPT_HAS_IB_PID_PARSERS             1
	#define BXPT_HAS_REMUX                      1
	#define BXPT_HAS_PACKETSUB                  1
	#define BXPT_HAS_AVS                        1
	#define BXPT_HAS_STARTCODE_BUFFER_WORKAROUND    1
	#define BXPT_2_BYTE_MINIMUM_PLAYBACK_BUFFER    1
	#define BXPT_HAS_32BIT_PB_TIMESTAMPS		1

	/* These defines don't directly affect the API */
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_SEPARATE_REMUX_IO				1
	#define BXPT_REC_SCD_MAPPING				1
	#define BXPT_NEXT_GEN_COMPARATORS			1
	#define BXPT_PER_CONTEXT_PIC_COUNTER		1
	#define BXPT_HAS_RAVE_PES_EXTENSIONS		1
	#define BXPT_PTS_CONTROL_IN_REC_REG			1
	#define BXPT_PLAYBACK_TS_ERROR_BOUNDS		1

		/* These changes don't directly affect the API definition */
	#define BXPT_HAS_DPCRS				1
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_SEPARATE_REMUX_IO				1
				
	    /* Number of filter banks, and the number of filters in each bank. */
	#ifdef  BXPT_FILTER_32
	    #define BXPT_P_MAX_FILTER_BANKS         ( 2 )
	#else
	    #define BXPT_P_MAX_FILTER_BANKS         ( 4 )
	#endif /*BXPT_FILTER_32 **/

    #define BXPT_P_MAX_FILTERS_PER_BANK     ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_P_MAX_FILTERS_PER_BANK 
    #define BXPT_P_MAX_TPIT_PIDS            ( 15 )
    #define BXPT_P_MAX_REMULTIPLEXORS       ( 2 )
    #define BXPT_P_MAX_PCRS                 ( 2 )
    #define BXPT_P_MAX_REMUX_PID_MAPS       ( 32 )     
    #define BXPT_P_MAX_REMUX_PSUB_TABLES    ( 2 )      
    #define BXPT_P_REMUX_SUB_TABLE_SIZE     ( 188 )
    #define BXPT_P_REMUX_PSUB_PAYLOAD_SIZE  ( 184 )
    #define BXPT_P_MAX_PID_CHANNELS         ( 256 ) 
    #define BXPT_P_MAX_PID_PARSERS          ( 6 )
    #define BXPT_P_MAX_MESG_BUFFERS         ( 128 )
    #define BXPT_P_MAX_SAM_PID_CHANNEL      BXPT_P_MAX_MESG_BUFFERS
    #define BXPT_P_MAX_RAVE_CHANNELS        ( 1 )
    #define BXPT_P_MAX_RAVE_CONTEXTS        ( 24 )
    #define BXPT_P_MAX_SCD                  ( 33 )
    #define BXPT_P_MAX_TPIT                 ( 5 )
    #define BXPT_P_MAX_CAP_FILTERS          ( 5 )
    #define BXPT_P_MAX_PACKETIZERS_PER_PB   ( 4 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_P_MAX_PACKETSUBS           ( 8 )
    #define BXPT_P_MAX_INPUT_BANDS          ( 6 )
    #define BXPT_P_MAX_PLAYBACKS            ( 2 )
    #define BXPT_P_MAX_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_P_MAX_STCS                 ( 4 )

#elif( BCHP_CHIP == 7420 )
	#define BXPT_HAS_STATIC_PID2BUF				1
	#define BXPT_HAS_DPCRS						1
	#define BXPT_HAS_RMX_NULL_THRESHOLD         1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define BXPT_HAS_PCR_PACING		 			1
	#define BXPT_HAS_MESG_BUFFERS				1
	#define BXPT_HAS_RSBUF						1
	#define BXPT_HAS_XCBUF						1
	#define BXPT_HAS_DIRECTV_SUPPORT			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_PID2BUF_MAPPING			1
	#define BXPT_HAS_IB_PID_PARSERS             1
	#define BXPT_HAS_REMUX                      1
	#define BXPT_HAS_PACKETSUB                  1
	#define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
	#define BXPT_HAS_32BIT_PB_TIMESTAMPS		1

	/* These changes don't directly affect the API definition */
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_SEPARATE_REMUX_IO				1
	#define BXPT_REC_SCD_MAPPING				1
	#define BXPT_NEXT_GEN_COMPARATORS			1
	#define BXPT_PER_CONTEXT_PIC_COUNTER		1
	#define BXPT_HAS_RAVE_PES_EXTENSIONS		1
	#define BXPT_PTS_CONTROL_IN_REC_REG			1
				
		   /* Number of filter banks, and the number of filters in each bank. */
	#ifdef  BXPT_FILTER_32
	    #define BXPT_P_MAX_FILTER_BANKS         ( 2 )
	#else
	    #define BXPT_P_MAX_FILTER_BANKS         ( 4 )
	#endif /*BXPT_FILTER_32 **/

	#if (BCHP_VER >= BCHP_VER_B0)
	    #define BXPT_P_MAX_PCRS                 ( 8 )
	#else
	    #define BXPT_P_MAX_PCRS                 ( 4 )
	#endif

    #define BXPT_P_MAX_FILTERS_PER_BANK     ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_P_MAX_FILTERS_PER_BANK 
    #define BXPT_P_MAX_TPIT_PIDS            ( 15 )
    #define BXPT_P_MAX_REMULTIPLEXORS       ( 2 )
    #define BXPT_P_MAX_REMUX_PID_MAPS       ( 32 )     
    #define BXPT_P_MAX_REMUX_PSUB_TABLES    ( 2 )      
    #define BXPT_P_REMUX_SUB_TABLE_SIZE     ( 188 )
    #define BXPT_P_REMUX_PSUB_PAYLOAD_SIZE  ( 184 )
    #define BXPT_P_MAX_PID_CHANNELS         ( 256 ) 
    #define BXPT_P_MAX_PID_PARSERS          ( 6 )
    #define BXPT_P_MAX_MESG_BUFFERS         ( 128 )
    #define BXPT_P_MAX_SAM_PID_CHANNEL      BXPT_P_MAX_MESG_BUFFERS
    #define BXPT_P_MAX_RAVE_CHANNELS        ( 1 )
    #define BXPT_P_MAX_RAVE_CONTEXTS        ( 24 )
    #define BXPT_P_MAX_SCD                  ( 33 )
    #define BXPT_P_MAX_TPIT                 ( 8 )
    #define BXPT_P_MAX_CAP_FILTERS          ( 5 )
    #define BXPT_P_MAX_PACKETIZERS_PER_PB   ( 4 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_P_MAX_PACKETSUBS           ( 8 )
    #define BXPT_P_MAX_INPUT_BANDS          ( 7 )
    #define BXPT_P_MAX_PLAYBACKS            ( 8 )
    #define BXPT_P_MAX_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_P_MAX_STCS                 ( 4 )

#elif( BCHP_CHIP == 7550 )
	#define BXPT_HAS_STATIC_PID2BUF				1
	#define BXPT_HAS_DPCRS						1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define BXPT_HAS_PCR_PACING		 			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_REMUX_PCR_OFFSET			1
	#define BXPT_HAS_MPOD_SCARD_SUPPORT			1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_RAVE_PES_EXTENSIONS		1
	#define BXPT_HAS_IB_PID_PARSERS             1
	#define BXPT_HAS_REMUX                      1
	#define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
	#define BXPT_HAS_PACKETSUB                  1

	/* These changes don't directly affect the API definition */
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_SEPARATE_REMUX_IO				1
	#define BXPT_REC_SCD_MAPPING				1
	#define BXPT_NEXT_GEN_COMPARATORS			1
	#define BXPT_PER_CONTEXT_PIC_COUNTER		1
	#define BXPT_PTS_CONTROL_IN_REC_REG			1
	#define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
 		
    #define BXPT_P_MAX_TPIT_PIDS            ( 15 )
    #define BXPT_P_MAX_REMULTIPLEXORS       ( 1 )
    #define BXPT_P_MAX_PCRS                 ( 1 )
    #define BXPT_P_MAX_PID_CHANNELS         ( 96 ) 
    #define BXPT_P_MAX_PID_PARSERS          ( 3 )
    #define BXPT_P_MAX_RAVE_CHANNELS        ( 1 )
    #define BXPT_P_MAX_RAVE_CONTEXTS        ( 24 )
    #define BXPT_P_MAX_SCD                  ( 33 )
    #define BXPT_P_MAX_TPIT                 ( 3 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_P_MAX_PACKETSUBS           ( 1 )
    #define BXPT_P_MAX_INPUT_BANDS          ( 4 )
    #define BXPT_P_MAX_PLAYBACKS            ( 2 )
    #define BXPT_P_MAX_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_P_MAX_STCS                 ( 1 )

#elif ( BCHP_CHIP == 7468 )
	#define BXPT_HAS_STATIC_PID2BUF				1
	#define BXPT_HAS_DPCRS						1

	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define BXPT_HAS_PCR_PACING		 			1
	#define BXPT_HAS_MESG_BUFFERS				1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_PID2BUF_MAPPING			1
	#define BXPT_HAS_32BIT_PRESCALE				1
	#define BXPT_HAS_IB_PID_PARSERS             1
	#define BXPT_HAS_PACKETSUB                  1

	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_REC_SCD_MAPPING				1
	#define BXPT_NEXT_GEN_COMPARATORS			1
	#define BXPT_PER_CONTEXT_PIC_COUNTER		1
	#define BXPT_HAS_RAVE_PES_EXTENSIONS		1
	#define BXPT_HAS_DIRECTV_SUPPORT            1
	#define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
	#define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
					 
	/* These defines don't directly affect the API */
	#define BXPT_PTS_CONTROL_IN_REC_REG			1

	    /* Number of filter banks, and the number of filters in each bank. */
	#ifdef  BXPT_FILTER_32
	    #define BXPT_P_MAX_FILTER_BANKS         ( 2 )
	#else
	    #define BXPT_P_MAX_FILTER_BANKS         ( 4 )
	#endif /*BXPT_FILTER_32 **/

    #define BXPT_P_MAX_FILTERS_PER_BANK     ( 32 )
    #define BXPT_P_FILTER_TABLE_SIZE        BXPT_P_MAX_FILTERS_PER_BANK 
    #define BXPT_P_MAX_TPIT_PIDS            ( 15 )
    #define BXPT_P_MAX_PCRS                 ( 4 )
    #define BXPT_P_MAX_PID_CHANNELS         ( 128 ) 
    #define BXPT_P_MAX_PID_PARSERS          ( 2 )
    #define BXPT_P_MAX_MESG_BUFFERS         ( 128 )
    #define BXPT_P_MAX_SAM_PID_CHANNEL      BXPT_P_MAX_MESG_BUFFERS
    #define BXPT_P_MAX_RAVE_CHANNELS        ( 1 )
    #define BXPT_P_MAX_RAVE_CONTEXTS        ( 24 )
    #define BXPT_P_MAX_SCD                  ( 33 )
    #define BXPT_P_MAX_TPIT                 ( 5 )
    #define BXPT_P_MAX_CAP_FILTERS          ( 5 )
    #define BXPT_P_MAX_PACKETIZERS_PER_PB   ( 4 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_P_MAX_PACKETSUBS           ( 2 )
    #define BXPT_P_MAX_INPUT_BANDS          ( 2 )
    #define BXPT_P_MAX_PLAYBACKS            ( 3 )
    #define BXPT_P_MAX_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_P_MAX_STCS                 ( 4 )

#elif( BCHP_CHIP == 7408 )
	#define BXPT_HAS_STATIC_PID2BUF				1
	#define BXPT_HAS_DPCRS						1

	#define BXPT_HAS_DIRECTV_SUPPORT			1
	#define BXPT_HAS_RAVE_SCRAMBLING_CONTROL	1
	#define BXPT_HAS_PCR_PACING		 			1
	#define BXPT_HAS_PID_CHANNEL_PES_FILTERING	1
	#define BXPT_HAS_FIXED_PSUB_DMA_PRIORITY	1
	#define BXPT_HAS_PACKETSUB_FORCED_INSERTION	1
	#define BXPT_HAS_MOSAIC_SUPPORT				1
	#define BXPT_HAS_32BIT_PRESCALE             1

	/* These changes don't directly affect the API definition */
	#define BXPT_DPCR_GLOBAL_PACKET_PROC_CTRL	1
	#define BXPT_REC_SCD_MAPPING				1
	#define BXPT_NEXT_GEN_COMPARATORS			1
	#define BXPT_PER_CONTEXT_PIC_COUNTER		1
	#define BXPT_HAS_RAVE_PES_EXTENSIONS		1
	#define BXPT_PTS_CONTROL_IN_REC_REG			1
	#define BXPT_PLAYBACK_TS_ERROR_BOUNDS       1
	#define BXPT_HAS_32BIT_PB_TIMESTAMPS		1
				
	#define BXPT_P_MAX_PCRS                     ( 2 )

    #define BXPT_P_MAX_PID_PARSERS          ( 0 )
    #define BXPT_P_MAX_INPUT_BANDS          ( 0 )
    #define BXPT_P_MAX_PACKETSUBS           ( 0 )

    #define BXPT_P_MAX_TPIT_PIDS            ( 15 )
    #define BXPT_P_MAX_PID_CHANNELS         ( 64 ) 
    #define BXPT_P_MAX_RAVE_CHANNELS        ( 1 )
    #define BXPT_P_MAX_RAVE_CONTEXTS        ( 24 )
    #define BXPT_P_MAX_SCD                  ( 33 )
    #define BXPT_P_MAX_TPIT                 ( 5 )

    /* Number of splicing stacks. The stacks are shared between the AV and SCD contexts */
    #define BXPT_P_NUM_SPLICING_QUEUES      ( 3 )

    #define BXPT_P_MAX_PLAYBACKS            ( 2 )
    #define BXPT_P_MAX_PCR_OFFSET_CHANNELS  ( 16 )
    #define BXPT_P_MAX_STCS                 ( 4 )

#else

    #error "UNSUPPORTED CHIP"                                                                                           

#endif  /* BCHP_CHIP == ??? */
 
#ifdef __cplusplus
}
#endif

#endif /* #ifndef BXPT_H__ */

/* end of file */


