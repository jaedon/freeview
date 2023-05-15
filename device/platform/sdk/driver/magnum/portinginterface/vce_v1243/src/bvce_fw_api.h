/***************************************************************************
 *     Copyright (c) 2010-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * ***************************************************************************/
/*
    Title: ViCE2 Firmware API definitions (bvce_fw_api.h)

    This file defines the interface between the Host and the ViCE firmware.
    It contains definitions for the commands formatting.
*/

#ifndef BVCE_FW_API_H__
#define BVCE_FW_API_H__

/* ==========================  INCLUDES =========================== */

/* NOTE: the definitions of uint8_t, uint16_t, ...  are coming from "defs.h"
 *       when the firmware includes this file and from "bstd.h" when the
 *       PI is using this file. */

/* NOTE2: This file can be compiled by our customer on C90 compilers
 *        where C++-style comments are not recognized, Please use only C-style
 *        comments in this file. */

/* ==========================  DEFINITIONS ======================== */

#ifdef __cplusplus
extern "C"
{
#endif


/*
 * API Version  MM.mm.pp.bb
 */

#define VICE2_API_VERSION                  0x05020100

/*
 * Size of the command buffer between host (PI) and FW in bytes
 */
#define HOST_CMD_BUFFER_SIZE                      128

/*
 * ViCE2 to Host interrupts
 */
#define HOST_CMD_MAILBOX_INTERRUPT_LEVEL           28
#define HOST_EVENTS_INTERRUPT_LEVEL                27
#define HOST_FW_WDOG_PIC_INTERRUPT_LEVEL           0
#define HOST_FW_WDOG_MB_INTERRUPT_LEVEL            1

/*
 * Offset of the ViCE2 Mailboxes from the start of DCCM
 */
#define HOST2VICE_MBOX_OFFSET                       0
#define VICE2HOST_MBOX_OFFSET                       4
#define BVN2VICE_MBOX_OFFSET                        8
#define SCRATCH_MBOX_OFFSET                        12
#define BVN2VICE_MBOX_PAYLOAD_OFFSET               16


/*
 * Minimum size of the CDB and ITB buffers which is required by the FW.
 * The host may allocate larger buffers than that.
 */
#define MIN_CDB_SIZE_IN_BYTES                               0x00600000    /*  06MB */
#define MIN_ITB_SIZE_IN_BYTES                               0x00030200    /* 192.5kB */


/*
 * Minimum size of the secure and non secure buffer that are required
 * for correct operation of the encoder. The host may allocate larger
 * buffers.
 */
#define MIN_SECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V1_AND_V2_0   0x00400000    /*   4MB */
#define MIN_SECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V2_1          0x00580000    /* 5.5MB */
#define MIN_NONSECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V1     0x02800000    /*  40MB */
#define MIN_NONSECURE_BUFFER_SIZE_IN_BYTES_FOR_VICE2_V2     0x03100000    /*  49MB */
#define MIN_CMD_BUFFER_SIZE_IN_BYTES                        0x00004000    /*  16kB */


/* =====================================================*/
/* -------------- Host command opcode ------------------*/

/* NOTE: Enumerations cannot be used here because their size
         is not guaranteed to be the same on different compilers
*/

/*
 * VICE 2 Boot Status
 * It is returned in the HOST2VICE mail box register
 */
#define VICE2_BOOT_STATUS_INIT                           (0xFFFFFFFF)
#define VICE2_BOOT_STATUS_INIT_PICARC_UART                      (0x1)
#define VICE2_BOOT_STATUS_INIT_PICARC_INTERRUPTS                (0x2)
#define VICE2_BOOT_STATUS_INIT_ICACHE                           (0x3)
#define VICE2_BOOT_STATUS_INIT_HOST_IF                          (0x4)
#define VICE2_BOOT_STATUS_INIT_BVN_IF                           (0x5)
#define VICE2_BOOT_STATUS_BOOT_MBARC                            (0x6)
#define VICE2_BOOT_STATUS_INIT_DINO                             (0x7)
#define VICE2_BOOT_STATUS_INIT_CMD_BUFFER                       (0x8)
#define VICE2_BOOT_STATUS_CHECK_VERSION                         (0x9)
#define VICE2_BOOT_STATUS_INIT_SCHED                            (0xA)
#define VICE2_BOOT_STATUS_INIT_HG                               (0xB)
#define VICE2_BOOT_STATUS_COMPLETE                              (0x0)


/*
 * Error return codes for ViCE2 Command
 */
#define VICE2_CMD_RETURN_OK                                          (0)    /* command was processed by FW successfully  */
#define VICE2_CMD_RETURN_ERR_UNKNOWN_COMMAND_ID                      (1)    /* the command opcode could not be recognised by the FW */
#define VICE2_CMD_RETURN_ERR_NO_RESOURCES                            (2)    /* resources required to run the command are not available */
#define VICE2_CMD_RETURN_ERR_INVALID_CHANNEL_ID                      (3)    /* the channel is invalid */
#define VICE2_CMD_RETURN_ERR_NULL_ADDRESS                            (4)    /* Secure ot non secure buffer has NULL address */
#define VICE2_CMD_RETURN_ERR_SECURE_BUFFER_SIZE_IS_TOO_SMALL         (5)    /* Secure buffer size is too small for FW needs */
#define VICE2_CMD_RETURN_ERR_IN_CDB_OR_ITB_BUFFER                    (6)    /* CDB/ITB buffer is too small or not aligned properly */
#define VICE2_CMD_RETURN_ERR_ENCODER_BUFFER_SIZE_IS_TOO_SMALL        (7)    /* Encoder dram buffer size is too small for the EPM needs */
#define VICE2_CMD_RETURN_ERR_CHANNEL_IS_ALREADY_INITIALIZED          (8)    /* Trying to initialize a channel which was already initialized */
#define VICE2_CMD_RETURN_ERR_CHANNEL_IS_ALREADY_OPENED               (9)    /* Trying to open a channel which was already opened */
#define VICE2_CMD_RETURN_ERR_CHANNEL_WAS_NOT_INITIALIZED            (10)    /* Trying to open a channel which was not initialized */
#define VICE2_CMD_RETURN_ERR_CHANNEL_WAS_NOT_OPENED                 (11)    /* Trying to configure/start/close a channel which was not opened */
#define VICE2_CMD_RETURN_ERR_VICE2_IS_IN_FLUSHING_MODE              (12)    /* ViCE2 can not configure/start/close a channel while it is flushing data */
#define VICE2_CMD_RETURN_ERR_CHANNEL_IS_ALREADY_STARTED             (13)    /* Trying to start a channel which was already started */
#define VICE2_CMD_RETURN_ERR_CHANNEL_WAS_NOT_CONFIGURED             (14)    /* Trying to start a channel which was not configured */
#define VICE2_CMD_RETURN_ERR_CHANNEL_WAS_NOT_STARTED                (15)    /* Trying to stop a channel which was not started */
#define VICE2_CMD_RETURN_ERR_CHANNEL_WAS_NOT_STOPPED                (16)    /* Trying to close a channel which was not stopped */
#define VICE2_CMD_RETURN_ERR_UNSUPPORTED_FRAME_RATE                 (17)    /* Configure an unsupported frame rate */
#define VICE2_CMD_RETURN_ERR_UNSUPPORTED_DEE                        (18)    /* DDE value given by the host is invalid */
#define VICE2_CMD_RETURN_ERR_FRAMERATE_CHANGE_INVALID_IN_INTERLACE  (19)    /* ViCE2 does not support frame rate changes in interlaced mode */
#define VICE2_CMD_RETURN_ERR_UNSUPPORTED_INTERLACED_TYPE_LOW_DELAY  (20)    /* Low delay mode is supported only for progressive input */
#define VICE2_CMD_RETURN_ERR_UNSUPPORTED_B_PICTURES_IN_LOW_DELAY    (21)    /* Low delay mode can not support B picture ( reordering ) */
#define VICE2_CMD_RETURN_ERR_UNSUPPORTED_B_PICTURES_IN_STD          (22)    /* Vice2 does not support B picture for MPEG4 or VP8 standards */
#define VICE2_CMD_RETURN_ERR_UNSUPPORTED_INTERLACED_IN_STD          (23)    /* Vice2 does not support interlaced type for MPEG4 or VP8 standards */
#define VICE2_CMD_RETURN_ERR_INVALID_DEBUG_COMMAND                  (24)    /* Command String passed to the Debug Command is not valid */
#define VICE2_CMD_RETURN_ERR_SG_CMD_BUFFER_SIZE_IS_TOO_SMALL        (25)    /* SG-CABAC command buffer size is too small (< MIN_CMD_BUFFER_SIZE_IN_BYTES) */
#define VICE2_CMD_RETURN_ERR_MAX_RESOLUTION_ERROR                   (26)    /* MaxPictureHeight/MaxPictureWidth are larger than the largest supported res.*/
#define VICE2_CMD_RETURN_ERR_MAX_FRAMERATE_LIMIT_TOO_LARGE          (27)    /* MaxFrameRateLimit is larger than the largest supported frame rate */
#define VICE2_CMD_RETURN_ERR_MIN_FRAMERATE_LIMIT_TOO_SMALL          (28)    /* MinFrameRateLimit is smaller than the smallest supported frame rate */
#define VICE2_CMD_RETURN_ERR_MIN_BVN_FRAMERATE_LIMIT_TOO_SMALL      (29)    /* BvnFrameRate is smaller than the smallest supported frame rate */
#define VICE2_CMD_RETURN_ERR_FRAMERATE_LESS_THAN_LIMIT              (30)    /* Given frame rate is smaller than the limit (MinFrameRateLimit) */
#define VICE2_CMD_RETURN_ERR_FRAMERATE_MORE_THAN_LIMIT              (31)    /* Given frame rate is larger than the limit (MaxFrameRateLimit) */
#define VICE2_CMD_RETURN_ERR_WRONG_GOP_LENGTH                       (32)    /* Gop length is wrong ( should be 1 for I_ONLY and 1+3N for IBBP */
#define VICE2_CMD_RETURN_ERR_DEBUG_COMMAND_USE_IN_INVALID_STATE     (33)    /* Cmd String passed to the Debug Command used in state in which it is invalid*/
#define VICE2_CMD_RETURN_ERR_MULTIPLE_SLICES_NOT_SUPPORTED          (34)    /* Multiple slice not supported for Vice2 V1 */
#define VICE2_CMD_RETURN_ERR_INVALID_FORCE_INTRA_CONFIGURATION      (35)    /* Invlaid Forced Intra configuration setting */
#define VICE2_CMD_RETURN_ERR_LOW_DELAY_SUPPORTED_ONLY_CHANNEL_0     (36)    /* Low delay is supported only for single channel and the channel must be 0 */
#define VICE2_CMD_RETURN_ERR_LOW_DELAY_UNSUPPORTED_ON_TARGET        (37)    /* Low delay mode is not supported for this core version */
#define VICE2_CMD_RETURN_ERR_RESTART_GOP_SCENE_INVALID_GOP          (38)    /* Restarting GOP is only valid w/ I or IP GOP structures */
#define VICE2_CMD_RETURN_ERR_ITFP_INVALID_IN_LOW_DELAY              (39)    /* ITFP command is not valid in Low-Delay mode */
#define VICE2_CMD_RETURN_ERR_TARGET_BITRATE_LARGER_MAX              (40)    /* Target bitrate is larger than the maximum bitrate */
#define VICE2_CMD_RETURN_ERR_WRONG_NUM_PARALLEL_ENC                 (41)    /* Too many parallel FNRT encode specified for the given version */
#define VICE2_CMD_RETURN_ERR_UNSUPPORTED_FINITE_GOP_IN_LOW_DELAY    (42)    /* Low delay mode does not support finite GOP */
#define VICE2_CMD_RETURN_ERR_MAX_NUM_CH_NOT_SUPPORTED               (43)    /* The max number of channels given in the open cmd is too high on this target */
#define VICE2_CMD_RETURN_ERR_MAX_NUM_CH_INCONSISTENT                (44)    /* The max number of channels should be the same on all channels */
#define VICE2_CMD_RETURN_ERR_UNSUPPORTED_ENCODING_STD               (45)    /* The encoding standard in not supported for this core */

/*
 * VICE2 Commands
 */
#define VICE2_COMMAND_SIGNATURE                           (0x42434D00)
#define VICE2_CMD_INITIALIZE                              (0x42434D01)
#define VICE2_CMD_OPEN_CHANNEL                            (0x42434D02)
#define VICE2_CMD_START_CHANNEL                           (0x42434D03)
#define VICE2_CMD_STOP_CHANNEL                            (0x42434D04)
                                                          
                                                          
#define VICE2_CMD_CLOSE_CHANNEL                           (0x42434D07)
#define VICE2_CMD_CONFIG_CHANNEL                          (0x42434D08)
#define VICE2_CMD_DEBUG_CHANNEL                           (0x42434D09)
#define VICE2_CMD_GET_CHANNEL_STATUS                      (0x42434D0A)
#define VICE2_CMD_GET_DEVICE_STATUS                       (0x42434D0B)


/*
 * Channel-specific Event codes returned by the ViCE through the Status and Event interface
 */
/* Errors (LSBs) */
#define VICE2_ERROR_INVALID_INPUT_DIMENSION_BIT                     (0)
#define VICE2_ERROR_USER_DATA_LATE_BIT                              (1)
#define VICE2_ERROR_USER_DATA_DUPLICATE_BIT                         (2)
#define VICE2_ERROR_FW_ADJUSTS_WRONG_FRAME_RATE                     (3)
#define VICE2_ERROR_UNSUPPORTED_BVN_FRAME_RATE                      (4)
#define VICE2_ERROR_UNSUPPORTED_RESOLUTION                          (5)
#define VICE2_ERROR_BVN_FRAMERATE_IS_SMALLER_THAN_THE_MINIMUM_ALLOWED        (6)
#define VICE2_ERROR_MISMATCH_BVN_PIC_RESOLUTION                     (7)
#define VICE2_ERROR_FW_INCREASED_BITRATE_ABOVE_MAX                  (8)
#define VICE2_ERROR_BIN_BUFFER_IS_FULL                              (9)
#define VICE2_ERROR_CDB_IS_FULL                                     (10)
#define VICE2_ERROR_PICARC_TO_CABAC_DINO_BUFFER_IS_FULL             (11)
#define VICE2_ERROR_EBM_IS_FULL                                     (12)
#define VICE2_ERROR_NUM_SLICES_ADJUSTED_TO_MAX_ALLOWED              (13)
#define VICE2_ERROR_NUM_ENTRIES_INTRACODED_ADJUSTED_TO_MAX_ALLOWED  (14)
#define VICE2_ERROR_IBBP_NOT_SUPPORTED_FOR_THIS_RESOLUTION          (15)
#define VICE2_ERROR_MBARC_BOOT_FAILURE                              (16)
#define VICE2_ERROR_MEASURED_ENCODER_DELAY_LONGER_THAN_ESTIMATED    (17)
#define VICE2_ERROR_CRITICAL                                        (18)

/* Events (MSBs) */
#define VICE2_EVENT_EOS_SENT_BIT                                    (30)
#define VICE2_EVENT_BVN_METADATA_CHANGE_BIT                         (31)


/*
 * Channel-independent Error codes located in a word which location is returned from Initialize cmd
 */
#define VICE2_ERROR_STACK_OVERFLOW_BIT                      0x00


/*
 * Watchdog error codes
 *
 * Note that there is a convention that the "AFTER" codes are always the previous Code + 1
 */
typedef enum
{
    VICE2_WDOG_TRACE_ISR_WAIT_VIP_BUSY                          = 0x010,
    VICE2_WDOG_TRACE_ISR_WAIT_VIP_BUSY_AFTER                    = 0x011,
    VICE2_WDOG_TRACE_ISR_WAIT_VIP_SHADOW                        = 0x020,
    VICE2_WDOG_TRACE_ISR_WAIT_VIP_SHADOW_AFTER                  = 0x021,
    VICE2_WDOG_TRACE_ISR_ITFP_AND_IPB                           = 0x030,
    VICE2_WDOG_TRACE_ISR_ITFP_AND_IPB_AFTER                     = 0x031,
    VICE2_WDOG_TRACE_SCHED_WAIT_START_PIC_TO_MBARC              = 0x040,
    VICE2_WDOG_TRACE_SCHED_WAIT_START_PIC_TO_MBARC_AFTER        = 0x041,
    VICE2_WDOG_TRACE_SCHED_COLOCATED_WAIT_ARC_TO_MC_EMPTY       = 0x050,
    VICE2_WDOG_TRACE_SCHED_COLOCATED_WAIT_ARC_TO_MC_EMPTY_AFTER = 0x051,
    VICE2_WDOG_TRACE_SCHED_COLOCATED_WAIT_DMA_IDLE              = 0x060,
    VICE2_WDOG_TRACE_SCHED_COLOCATED_WAIT_DMA_IDLE_AFTER        = 0x061,
    VICE2_WDOG_TRACE_SCHED_COLOCATED_WAIT_SEND_MC_DINO          = 0x070,
    VICE2_WDOG_TRACE_SCHED_COLOCATED_WAIT_SEND_MC_DINO_AFTER    = 0x071,
    VICE2_WDOG_TRACE_SCHED_COLOCATED_WAIT_DMA1_IDLE             = 0x080,
    VICE2_WDOG_TRACE_SCHED_COLOCATED_WAIT_DMA1_IDLE_AFTER       = 0x081,
    VICE2_WDOG_TRACE_ENCODE_WAIT_FME_SHADOW                     = 0x090,
    VICE2_WDOG_TRACE_ENCODE_WAIT_FME_SHADOW_AFTER               = 0x091,
    VICE2_WDOG_TRACE_ENCODE_WAIT_IMD_SHADOW                     = 0x0A0,
    VICE2_WDOG_TRACE_ENCODE_WAIT_IMD_SHADOW_AFTER               = 0x0A1,
    VICE2_WDOG_TRACE_ENCODE_WAIT_MC_SHADOW                      = 0x0B0,
    VICE2_WDOG_TRACE_ENCODE_WAIT_MC_SHADOW_AFTER                = 0x0B1,
    VICE2_WDOG_TRACE_ENCODE_WAIT_HA_SHADOW                      = 0x0C0,
    VICE2_WDOG_TRACE_ENCODE_WAIT_HA_SHADOW_AFTER                = 0x0C1,
    VICE2_WDOG_TRACE_ENCODE_WAIT_XQ_SHADOW                      = 0x0D0,
    VICE2_WDOG_TRACE_ENCODE_WAIT_XQ_SHADOW_AFTER                = 0x0D1,
    VICE2_WDOG_TRACE_ENCODE_WAIT_SG_SHADOW                      = 0x0E0,
    VICE2_WDOG_TRACE_ENCODE_WAIT_SG_SHADOW_AFTER                = 0x0E1,
    VICE2_WDOG_TRACE_ENCODE_WAIT_DBLK_SHADOW                    = 0x0F0,
    VICE2_WDOG_TRACE_ENCODE_WAIT_DBLK_SHADOW_AFTER              = 0x0F1,
    VICE2_WDOG_TRACE_ENCODE_WAIT_XQ_DMA                         = 0x100,
    VICE2_WDOG_TRACE_ENCODE_WAIT_XQ_DMA_AFTER                   = 0x101,
    VICE2_WDOG_TRACE_ENCODE_WAIT_XQ_DMA1                        = 0x110,
    VICE2_WDOG_TRACE_ENCODE_WAIT_XQ_DMA1_AFTER                  = 0x111,
    VICE2_WDOG_TRACE_ENCODE_WAIT_CME_SHADOW                     = 0x120,
    VICE2_WDOG_TRACE_ENCODE_WAIT_CME_SHADOW_AFTER               = 0x121,
    VICE2_WDOG_TRACE_ENCODE_WAIT_MAU_SHADOW                     = 0x130, 
    VICE2_WDOG_TRACE_ENCODE_WAIT_MAU_SHADOW_AFTER               = 0x131,
    VICE2_WDOG_TRACE_ENCODE_WAIT_PAINT_DMA                      = 0x140,
    VICE2_WDOG_TRACE_ENCODE_WAIT_PAINT_DMA_AFTER                = 0x141,
    VICE2_WDOG_TRACE_HG_WAIT_USERDATA_DMA1                      = 0x150, /* 0x152 , 0x154, 0x156, 0x158 */
    VICE2_WDOG_TRACE_HG_WAIT_USERDATA_DMA1_AFTER                = 0x151, /* 0x153 , 0x155, 0x157, 0x159 */
    VICE2_WDOG_TRACE_HG_WAIT_USERDATA_DMA2                      = 0x160,
    VICE2_WDOG_TRACE_HG_WAIT_USERDATA_DMA2_AFTER                = 0x161,
    VICE2_WDOG_TRACE_RC_WAIT_SEND_DINO                          = 0x190,
    VICE2_WDOG_TRACE_RC_WAIT_SEND_DINO_AFTER                    = 0x191,
    VICE2_WDOG_TRACE_RC_WAIT_GET_DINO                           = 0x1A0,
    VICE2_WDOG_TRACE_RC_WAIT_GET_DINO_AFTER                     = 0x1A1,
    VICE2_WDOG_TRACE_RC_WAIT_SEND_DINO_UF                       = 0x1B0,
    VICE2_WDOG_TRACE_RC_WAIT_SEND_DINO_UF_AFTER                 = 0x1B1,
    VICE2_WDOG_TRACE_RC_WAIT_SEND_DINO_UF2                      = 0x1C0,
    VICE2_WDOG_TRACE_RC_WAIT_SEND_DINO_UF2_AFTER                = 0x1C1,
    VICE2_WDOG_TRACE_BOOT_WAIT_MBARC                            = 0x1D0,
    VICE2_WDOG_TRACE_BOOT_WAIT_MBARC_AFTER                      = 0x1D1,
    VICE2_WDOG_TRACE_TELEM_WAIT_DMA1                            = 0x1E0,
    VICE2_WDOG_TRACE_TELEM_WAIT_DMA1_AFTER                      = 0x1E1,
    VICE2_WDOG_TRACE_TELEM_WAIT_DMA2                            = 0x1F0,
    VICE2_WDOG_TRACE_TELEM_WAIT_DMA2_AFTER                      = 0x1F1,
    VICE2_WDOG_TRACE_TELEM_WAIT_DMA3                            = 0x200,
    VICE2_WDOG_TRACE_TELEM_WAIT_DMA3_AFTER                      = 0x201,
    VICE2_WDOG_TRACE_TELEM_WAIT_DMA4                            = 0x210,
    VICE2_WDOG_TRACE_TELEM_WAIT_DMA4_AFTER                      = 0x211,
    VICE2_WDOG_TRACE_UART_WAIT_TX                               = 0x220,
    VICE2_WDOG_TRACE_UART_WAIT_TX_AFTER                         = 0x221,
    VICE2_WDOG_TRACE_SCHED_FORCE_INTRA_WAIT_DINO_EMPTY          = 0x230,
    VICE2_WDOG_TRACE_SCHED_FORCE_INTRA_WAIT_DINO_EMPTY_AFTER    = 0x231,
    VICE2_WDOG_TRACE_SCHED_FORCE_INTRA_WAIT_SEND_DINO           = 0x240,
    VICE2_WDOG_TRACE_SCHED_FORCE_INTRA_WAIT_SEND_DINO_AFTER     = 0x241,
    VICE2_WDOG_TRACE_HOSTCMD_WAIT_DMA1                          = 0x250,
    VICE2_WDOG_TRACE_HOSTCMD_WAIT_DMA1_AFTER                    = 0x251,
    VICE2_WDOG_TRACE_ENCODE_WAIT_DBLK_IDLE                      = 0x260,
    VICE2_WDOG_TRACE_ENCODE_WAIT_DBLK_IDLE_AFTER                = 0x261,
    VICE2_WDOG_TRACE_TELEM_WAIT_DMA5                            = 0x270,
    VICE2_WDOG_TRACE_TELEM_WAIT_DMA5_AFTER                      = 0x271
} WdogTraceCode_e;










/* Add more here, note: codes are bit-mapped (max: 32) */


/*
 * MACRO: VCE_PTR(data_type)
 *  This macro does not use the argument (data_type) and redefines it as a uint32_t
 *  it is used to indicate what data type the pointer is intended to point to.
 */
#define VCE_PTR(data_type)    uint32_t




/* ---- ERROR ---- */

typedef struct Vice2GenericCmd_t
{
    uint32_t    Command;                                    /* OpCode of the command: VICE2_CMD_*  */
} Vice2GenericCmd_t;

typedef struct Vice2GenericCmdResponse_t
{
    uint32_t    Command;                                    /* OpCode of the command: VICE2_CMD_* */
    uint32_t    Status;                                     /* Status: VICE2_CMD_ERR_UNKNOWN_COMMAND_ID */
} Vice2GenericCmdResponse_t;



/* ---- INIT ---- */


#define E_STRIPE_WIDTH_64         0
#define E_STRIPE_WIDTH_128        1
#define E_STRIPE_WIDTH_256        2

#define E_STRIPE_MULTI_32         0
#define E_STRIPE_MULTI_64         1
#define E_STRIPE_MULTI_128        2

/* VerificationModeFlags Enumerators */
#define INIT_CMD_A2N_MASK                                   0x00000002
#define INIT_CMD_A2N_SHIFT                                  0x00000001
#define INIT_CMD_VERIFICATION_MODE_MASK                     0x00000001
#define INIT_CMD_VERIFICATION_MODE_SHIFT                    0x00000000


typedef struct Vice2CmdInit_t
{
    uint32_t    Command;                                    /* OpCode of the command: VICE2_CMD_*                               */
    uint32_t    API_Version;                                /* version of the API, this can remain a 32-bit number              */
    uint32_t    nStripeWidth;                               /* DRAM Stripe width for the given platform (E_STRIPE_WIDTH_*)      */
    uint32_t    nStripeMultiple;                            /* DRAM Stripe multiple for the given platform (E_STRIPE_MULTI_*)   */
    uint32_t    DeviceEndianess;                            /* CABAC Endianness flag 0:Big-endian (HW default), 1:Little-endian */
    uint32_t    DeviceSG_CABACCmdBuffPtr;                   /* SG-CABAC Command Buffer Address in DRAM                          */
    uint32_t    DeviceSG_CABACCmdBuffSize;                  /* SG-CABAC Command Buffer size in bytes                            */
    uint32_t    VerificationModeFlag;                       /* turns Verification mode On (1) or Off (0, default)               */
}Vice2CmdInit_t;

typedef struct Vice2DebugBufferInfo_t                       /* //NS : style check ignored: required for backwards compatibility   */
{
    uint32_t  uiReadOffset;                                 /* PI or FW will update this, depending on the mode */
    uint32_t  uiWriteOffset;                                /* FW will update this */
    uint32_t  uiMode;                                       /* VICE_DEBUG_BUFFER_MODE_STANDARD or VICE2_DEBUG_BUFFER_MODE_OVERWRITE (see below) */
    uint32_t  uiSize;                                       /* size of debug buffer in bytes (DWORD-multiple) */
    uint32_t  uiPhysicalOffset;                             /* physical offset in DRAM of debug log. (DWORD-aligned) */
} Vice2DebugBufferInfo_t;

/* Debug Buffer Mode Description */
#define VICE_DEBUG_BUFFER_MODE_STANDARD     (0)             /* Standard Circular FIFO  FW writes to the buffer as long as space is available. */
                                                            /* When the buffer is full, FW drops any data that would have been written to the buffer. */
                                                            /* The host would poll and read the debug log on regular time intervals. */
                                                            /* It is the host's responsibility to poll/read the buffer at a fast enough rate. */
                                                            /* This can be used with the debug FW command to implement a virtual UART interface. */
                                                            /* The FW updates the write offset and the host updates the read offset. */

#define VICE2_DEBUG_BUFFER_MODE_OVERWRITE   (1)             /* Circular FIFO  FW continually writes to the buffer, overwriting the oldest data as needed. */
                                                            /* The latest data is always available in the buffer. The FW updates the write offset. */
                                                            /* The FW also updates the read offset (equal to (write_offset-1) ) when overwriting old data. */
                                                            /* This mode is typically used to obtain a UART log history to help debug errors such as watchdog */
                                                            /* reset or an error interrupt.                                                                 */
                                                            /* If the host wants to read the data, the host needs to: */
                                                            /* 1.    Set the physical offset to NULL */
                                                            /* 2.    Wait XX ms  */
                                                            /* 3.    Read the data  */
                                                            /* 4.    Set the physical offset back  */

/* Response to the VICE2_CMD_INITIALIZE command (sent back to the Host) */
typedef struct Vice2CmdInitResponse_t
{
    uint32_t                        Command;                /* OpCode of the command: VICE2_CMD_* */
    uint32_t                        Status;                 /* Status: VICE2_CMD_OK, or VICE2_CMD_ERR_* */
    uint32_t                        Version;                /* Firmware version: format is 0xMMmmppbb (where: MM=major, mm=minor, pp=patch, bb=build) */
    VCE_PTR(Vice2DebugBufferInfo_t) pPicArcDebugBuff;       /* Buffer used to send debug information back up to the host for offline processing (PicARC) */
    VCE_PTR(Vice2DebugBufferInfo_t) pMbArcDebugBuff;        /* Buffer used to send debug information back up to the host for offline processing (MbARC) */
    VCE_PTR(uint32)                 pStatusBase;            /* Pointer to a location containing a bit-mapped u32 indicating which channel encounter an error */
    uint32_t                        FwApiVersion;           /* Version of the API that the Firmware has been built with */
    VCE_PTR(const char*)            pszVersionStr;          /* Pointer to a zero-terminated string containing information about the build */
    uint32_t                        BvnApiVersion;          /* Version of the BVN Mailbox API that the Firmware has been build with */
    VCE_PTR(uint32)                 pPicArcWdogErrCodeBase; /* Pointer to a 32-bit error code which identifies a loop where a watchdog error could happen */
    VCE_PTR(uint32)                 pMbArcWdogErrCodeBase;  /* Pointer to a 32-bit error code which identifies a loop where a watchdog error could happen */
} Vice2CmdInitResponse_t;


#define DEVICE_ERROR_CHANNEL_ID     31                      /* channel to use to indicate a device error */


/* ---- OPEN ---- */

/* User Data */
#define BVCE_FW_P_UserData_Payload_CC_608Metadata_LineOffset_MASK       0xF8
#define BVCE_FW_P_UserData_Payload_CC_608Metadata_LineOffset_SHIFT      3

#define BVCE_FW_P_UserData_Payload_CC_608Metadata_Priority_MASK         0x06
#define BVCE_FW_P_UserData_Payload_CC_608Metadata_Priority_SHIFT        1

#define BVCE_FW_P_UserData_Payload_CC_608Metadata_Valid_MASK            0x01
#define BVCE_FW_P_UserData_Payload_CC_608Metadata_Valid_SHIFT           0

#define BVCE_FW_P_UserData_Payload_CC_608_BYTES_PER_LINE_MAX            2
#define BVCE_FW_P_UserData_Payload_CC_608_LINES_PER_FIELD_MAX           5
#define BVCE_FW_P_UserData_Payload_CC_608_LENGTH                        ( 1 + ( 1 + BVCE_FW_P_UserData_Payload_CC_608_BYTES_PER_LINE_MAX ) *    \
                                                                                BVCE_FW_P_UserData_Payload_CC_608_LINES_PER_FIELD_MAX )

#define BVCE_FW_P_UserData_Payload_CC_708Metadata_Reserved_MASK         0xFC
#define BVCE_FW_P_UserData_Payload_CC_708Metadata_Reserved_SHIFT        2

#define BVCE_FW_P_UserData_Payload_CC_708Metadata_PacketStart_MASK      0x02
#define BVCE_FW_P_UserData_Payload_CC_708Metadata_PacketStart_SHIFT     1

#define BVCE_FW_P_UserData_Payload_CC_708Metadata_Valid_MASK            0x01
#define BVCE_FW_P_UserData_Payload_CC_708Metadata_Valid_SHIFT           0

#define BVCE_FW_P_UserData_Payload_CC_708_BYTES_PER_LINE_MAX            2
#define BVCE_FW_P_UserData_Payload_CC_708_LINES_PER_FIELD_MAX           9
#define BVCE_FW_P_UserData_Payload_CC_708_LENGTH                        ( 1 + ( 1 + BVCE_FW_P_UserData_Payload_CC_708_BYTES_PER_LINE_MAX ) *     \
                                                                                BVCE_FW_P_UserData_Payload_CC_708_LINES_PER_FIELD_MAX )

#define BVCE_FW_P_UserData_Payload_CC_MAX_LENGTH                        ( BVCE_FW_P_UserData_Payload_CC_608_LENGTH + \
                                                                          BVCE_FW_P_UserData_Payload_CC_708_LENGTH )

typedef struct BVCE_FW_P_UserData_Payload_CC
{
        uint8_t uiNumValid608Lines; /* Should be 1 for A/53.  Can be more than 1 for SCTE20/21 */
        /* If multiple lines of CC data exist, they must be added in ascending line_offset */
        struct
        {
                uint8_t uiMetadata; /* Bits[7:3] = line_offset (see PacketType) (Not for A/53), Bits[2:1] - priority (Only for SCTE 20), Bit[0] = cc_valid */
                uint8_t uiData[BVCE_FW_P_UserData_Payload_CC_608_BYTES_PER_LINE_MAX]; /* 2 bytes of CC */
        } st608Line[BVCE_FW_P_UserData_Payload_CC_608_LINES_PER_FIELD_MAX];

        uint8_t uiNumValid708Lines; /* Should be 0 for SCTE20/21 */
        struct
        {
               uint8_t uiMetadata; /* Bits[7:2] = Reserved, Bit[1] - Packet Start, Bit[0] = cc_valid */
               uint8_t uiData[BVCE_FW_P_UserData_Payload_CC_708_BYTES_PER_LINE_MAX]; /* 2 bytes of CC */
        } st708Line[BVCE_FW_P_UserData_Payload_CC_708_LINES_PER_FIELD_MAX];
} BVCE_FW_P_UserData_Payload_CC;

typedef enum BVCE_FW_P_UserData_PacketType
{
        BVCE_FW_P_UserData_PacketType_eSCTE_20 = 0,
        BVCE_FW_P_UserData_PacketType_eSCTE_21,
        BVCE_FW_P_UserData_PacketType_eATSC_A53,

        /* Add new user data encapsulation type ABOVE this line */
        BVCE_FW_P_UserData_PacketType_eMax
} BVCE_FW_P_UserData_PacketType;

/* BVCE_FW_P_UserData_PacketDescriptor contains the information necessary to create a single user data packet of ePacketType */
#define BVCE_FW_P_UserData_PacketDescriptor_MAX_LENGTH ( 4 + BVCE_FW_P_UserData_Payload_CC_MAX_LENGTH )

typedef struct BVCE_FW_P_UserData_PacketDescriptor
{
        uint16_t uiPacketLength; /* size of the entire packet including this field */
        uint16_t uiPacketType; /* See BVCE_FW_P_UserData_PacketType */
        BVCE_FW_P_UserData_Payload_CC stCC;
} BVCE_FW_P_UserData_PacketDescriptor;

#define BVCE_FW_P_UserData_QueueEntry_Metadata_STGPictureID_MASK    0xFF000000
#define BVCE_FW_P_UserData_QueueEntry_Metadata_STGPictureID_SHIFT   24

#define BVCE_FW_P_UserData_QueueEntry_Metadata_Polarity_MASK        0x00008000
#define BVCE_FW_P_UserData_QueueEntry_Metadata_Polarity_SHIFT       15

#define BVCE_FW_P_UserData_QueueEntry_Metadata_Length_MASK          0x000000FF
#define BVCE_FW_P_UserData_QueueEntry_Metadata_Length_SHIFT         0

typedef struct BVCE_FW_P_UserData_QueueEntry 
{
   uint32_t uiMetadata; /* Bits[31:24] - LSB of STG Picture ID
                         * Bits[23:16] - Reserved
                         *  Bit[15]    - Polarity (0=Top,1=Bottom)
                         * Bits[14:8]  - Reserved
                         * Bits[ 7:0]  - Length
                         */
   uint32_t uiOffset; /* Absolute offset into DRAM of the user data packet descriptor. See BVCE_FW_P_UserData_PacketDescriptor */
} BVCE_FW_P_UserData_QueueEntry;

#define BVCE_FW_P_USERDATA_QUEUE_LENGTH 36

typedef struct BVCE_FW_P_UserData_Queue 
{
   uint32_t uiReadOffset;
   uint32_t uiWriteOffset; /* Offset where 0 is the first entry in the queue, and BVCE_FW_P_USERDATA_QUEUE_LENGTH-1 is the last */
   BVCE_FW_P_UserData_QueueEntry astQueue[BVCE_FW_P_USERDATA_QUEUE_LENGTH];
} BVCE_FW_P_UserData_Queue;

typedef struct Vice2CmdOpenChannel_t
{
    uint32_t            Command;                            /* OpCode of the command: VICE2_CMD_* */
    uint32_t            uiChannel_id;                       /* channel number to open (0..65535) */
    VCE_PTR(uint8_t)    pNonSecureBufferBase;               /* pointer to the location that will contain the non-secure buffer */
    uint32_t            uiNonSecureBufferSize;              /* size of the non-secure buffer */
    VCE_PTR(uint8_t)    pSecureBufferBase;                  /* pointer to the location that will contain the secure buffer */
    uint32_t            uiSecureBufferSize;                 /* size of the secure buffer */
    uint32_t            uiMaxNumChannels;                   /* maximum number of channels, 0: max num of ch supported on current platform */
} Vice2CmdOpenChannel_t;


typedef struct Vice2CmdOpenChannelResponse_t
{
    uint32_t                          Command;              /* OpCode of the command: VICE2_CMD_* */
    uint32_t                          Status;               /* Status: VICE2_CMD_OK, or VICE2_CMD_ERR_* (see definition above) */
    VCE_PTR(BVCE_FW_P_UserData_Queue) pUserDataQInfoBase;   /* Base DCCM buffer address for the user data queue */
} Vice2CmdOpenChannelResponse_t;




/* ---- START ---- */

typedef struct Vice2CmdStartChannel_t
{
    uint32_t    Command;                                    /* OpCode of the command: VICE2_CMD_* */
    uint32_t    uiChannel_id;                               /* channel number to open (0..65535) */

} Vice2CmdStartChannel_t;

typedef struct Vice2CmdStartChannelResponse_t
{
    uint32_t    Command;                                    /* OpCode of the command: VICE2_CMD_* */
    uint32_t    Status;                                     /* Status: VICE2_CMD_OK, or VICE2_CMD_ERR_* (see definition above) */

} Vice2CmdStartChannelResponse_t;




/* ---- STOP  ---- */

typedef struct Vice2CmdStopChannel_t
{
    uint32_t     Command;                                   /* OpCode of the command: VICE2_CMD_* */
    uint32_t     uiChannel_id;                              /* channel number to open (0..65535) */
    uint32_t     Flags;                                     /* 32 flags (describe below STOP_FLAG_*) */

} Vice2CmdStopChannel_t;

/* Flag indicating if FW should stop the current encode as fast as possible and be ready to accept a START immediately 
   or normal stop. 0: normal stop, 1: fast stop.  */
#define STOP_FLAG_FAST_CHANNEL_STOP_POS                 0

typedef struct Vice2CmdStopChannelResponse_t
{
    uint32_t     Command;                                   /* OpCode of the command: VICE2_CMD_* */
    uint32_t     Status;                                    /* Status: VICE2_CMD_OK, or VICE2_CMD_ERR_* (see definition above) */

} Vice2CmdStopChannelResponse_t;




/* ---- CLOSE ---- */

typedef struct Vice2CmdCloseChannel_t
{
    uint32_t    Command;                                    /* OpCode of the command: VICE2_CMD_* */
    uint32_t    uiChannel_id;                               /* channel number to open (0..65535) */

} Vice2CmdCloseChannel_t;

typedef struct Vice2CmdCloseChannelResponse_t
{
    uint32_t    Command;                                    /* OpCode of the command: VICE2_CMD_* */
    uint32_t    Status;                                     /* Status: VICE2_CMD_OK, or VICE2_CMD_ERR_* (see definition above) */

} Vice2CmdCloseChannelResponse_t;




/* ---- CONFIG ---- */

/* Encoding Standards */
/* WARNING: These value correspond to values written to hardware registers
 *          DO NOT CHANGE THEM !
 */

#define ENCODING_STD_H264       0
#define ENCODING_STD_MPEG2      1
#define ENCODING_STD_MPEG4      2
#define ENCODING_STD_VP8        3

#define ENCODING_AVC_PROFILE_HIGH               100
#define ENCODING_AVC_PROFILE_MAIN               77
#define ENCODING_AVC_PROFILE_BASELINE           66
#define ENCODING_MPEG2_PROFILE_MAIN             4
#define ENCODING_MPEG4_PROFILE_SIMPLE           0
#define ENCODING_MPEG4_PROFILE_ADVANCED_SIMPLE  1
#define ENCODING_VP8_PROFILE_STANDARD_LF        0

typedef enum 
{
    ENCODING_AVC_LEVEL_10       = 10,
    ENCODING_AVC_LEVEL_11       = 11,
    ENCODING_AVC_LEVEL_12       = 12,
    ENCODING_AVC_LEVEL_13       = 13,
    ENCODING_AVC_LEVEL_20       = 20,
    ENCODING_AVC_LEVEL_21       = 21,
    ENCODING_AVC_LEVEL_22       = 22,
    ENCODING_AVC_LEVEL_30       = 30,
    ENCODING_AVC_LEVEL_31       = 31,
    ENCODING_AVC_LEVEL_32       = 32,
    ENCODING_AVC_LEVEL_40       = 40,
    ENCODING_MPEG2_LEVEL_LOW    = 10,
    ENCODING_MPEG2_LEVEL_MAIN   = 8,
    ENCODING_MPEG2_LEVEL_HIGH   = 4,
    ENCODING_MPEG4_LEVEL_1      = 1,
    ENCODING_MPEG4_LEVEL_2      = 2,
    ENCODING_MPEG4_LEVEL_3      = 3,
    ENCODING_MPEG4_LEVEL_4      = 4,
    ENCODING_MPEG4_LEVEL_5      = 5
}EncoderLevel_e;

typedef enum 
{
    ENCODING_FRAME_RATE_1498 = 1498,
    ENCODING_FRAME_RATE_1500 = 1500,
    ENCODING_FRAME_RATE_2000 = 2000,
    ENCODING_FRAME_RATE_2397 = 2397,
    ENCODING_FRAME_RATE_2400 = 2400,
    ENCODING_FRAME_RATE_2500 = 2500,
    ENCODING_FRAME_RATE_2997 = 2997,
    ENCODING_FRAME_RATE_3000 = 3000,
    ENCODING_FRAME_RATE_5000 = 5000,
    ENCODING_FRAME_RATE_5994 = 5994,
    ENCODING_FRAME_RATE_6000 = 6000
}FrameRate_e;

typedef enum 
{
    REFRESH_ROW = 0,
    REFRESH_COLUMN = 1
}ForceIntraPattern_e;

/* Valid GOP Structures */
/* GOP struct defines ordered from lowest delay (no B pictures) to highest delay (most consecutive B pictures) with track_input and infinite_IP always last */
#define ENCODING_GOP_STRUCT_I               0               /* [I][I][I][I], Gop Length ignored */
#define ENCODING_GOP_STRUCT_IP              1               /* [IPPPP][IPPPP]                   */
#define ENCODING_GOP_STRUCT_IBP             2               /* [IBPBP][IBPBP]                   */
#define ENCODING_GOP_STRUCT_IBBP            3               /* [IBBPBB][IBBPBB]                 */
#define ENCODING_GOP_STRUCT_IBBBP           4               /* [IBBBPBBBP][IBBBPBBBP]           */
#define ENCODING_GOP_STRUCT_TRACK_INPUT     5               /* Uses  the same GOP structure as the input stream. */
#define ENCODING_GOP_STRUCT_INFINITE_IP     6               /* [IPPPPPPPPPPPPP] until host send RAP         */

/* Mode of Operation */
#define ENCODER_MODE_HIGH_DELAY             0               /* high delay mode (picture by picture) */
#define ENCODER_MODE_LOW_DELAY              1               /* Low delay mode (line by line)        */
#define ENCODER_MODE_AFAP                   2               /* as-fast-as-possible mode (encoder back-pressure) */

/* Input type */
#define ENCODER_INPUT_TYPE_PROGRESSIVE      0
#define ENCODER_INPUT_TYPE_INTERLACED       1

/* Rate mode */
#define FIXED_FRAME_RATE_MODE               0
#define VARIABLE_FRAME_RATE_MODE            1

typedef struct Vice2CmdConfigChannel_t
{
    uint32_t    Command;                                    /* OpCode of the command: VICE2_CMD_* */
    uint32_t    uiChannel_id;                               /* channel number to open (0..65535) */
    uint32_t    Protocol;                                   /* encoding standard (see ENCODING_STD_* above) */
    uint32_t    Profile;                                    /* See ENCODING_STD_PROFILE_* above */
    uint32_t    Level;                                      /* See ENCODING_STD_LEVEL_* above */
    uint32_t    FrameRate;                                  /* Frame rate in unit of integer (100*frames/sec) */
    uint32_t    MaxBitrate;                                 /* Peak output bit rate in bits/sec */
    uint32_t    GopStructure;                               /* See GOP Structure definitions above (GOP_STRUCT_*) and GOP STRUCTURE flags */
    uint32_t    GopLength;                                  /* Number of pictures in a GOP ( e.g. Gop length of IBBPBBP is 7 ) */
    uint32_t    Mode;                                       /* High Delay, Low Delay, AFAP (ENCODER_MODE_*) */
    uint32_t    InputType;                                  /* Interlaced/Progressive (ENCODER_INPUT_TYPE_*) */
    uint32_t    EventMask;                                  /* indicates which events can generate an interrupt */
    uint32_t    Flags;                                      /* 32 flags (describe below CONFIG_FLAG_*) */
    uint32_t    StcID;                                      /* ID of the STC that this channel should use */
    uint32_t    ContextID;                                  /* ID of the ITB/CDB Context that the FW should use. */
    uint32_t    ITBBufPtr;                                  /* pointer to the ITB buffer base */
    uint32_t    ITBBufSize;                                 /* size of the ITB buffer in bytes */
    uint32_t    CDBBufPtr;                                  /* pointer to the CDB buffer base */
    uint32_t    CDBBufSize;                                 /* size of the CDB buffer in bytes */
    uint32_t    A2PDelay;                                   /* Desired End-to-end delay given by the Host */

    /* A2P optimizations arguments */
    uint32_t    FrameRateLimit;                             /* output frame rate limitations in the current session */
    uint32_t    RateBufferDelayInMs;                        /* Delay in ms to give for the Rate Control Buffer (higher=better quality)  */
    uint32_t    MinAllowedBvnFrameRate;                     /* The minimum allowed input frame rate (BVN) during the whole session */
    uint32_t    MaxPictureSizeInPels;                       /* Maximum picture size during the current session  */
    uint32_t    MaxAllowedGopStructure;                     /* Maximum allowed Gop Structure to indicate maximum picture reorder delay */

    uint32_t    NumSlicesPerPicture;                        /* Number of slices per picture */
    uint32_t    ForceIntraMode;                             /* Force Intra Mode configuration */
    uint32_t    TargetBitrate;                              /* Target output bit rate in bits/sec. If 0 : means TargetBitrate == MaxBitrate */
    uint32_t    NumParallelEncodes;                         /* Number of simultaneous NRT encode. If > 1: mean FNRT mode is on */
    
} Vice2CmdConfigChannel_t;


/* Flag that indicates the start of a new RAP point     1:new RAP point, 0: other                                       */
#define CONFIG_FLAG_NEW_RAP_POS                             0
/* Flag indicating that bitrate change is effective only when there is a BVN input change  1: true, 0:false             */
#define CONFIG_FLAG_APPLY_BITRATE_BVN_CHANGE_POS            1
/* Flag indicating if the encoder should encode at a fixed-rate or variable-rate. In a variable-rate mode the encoder
 * could decide to encode at a lower frame-rate if it detects a 3:2 pull-down for example.
 * Values are 0: fixed rate, 1: variable rate                                                                           */
#define CONFIG_FLAG_RATE_MODE_POS                           2
/* Flag indicating that the firmware should use the given Cabac context pointer and program the CABAC registers         */
#define CONFIG_FLAG_RESET_CABAC_CONTEXT_POS                 3
/* Flag indicating if ITFP algorithm is enabled or not. 0: enabled, 1: disabled. By default it is enabled.              */
#define CONFIG_FLAG_ITFP_DISABLED_POS                       4
/* Flag indicating if next start is fast channel change or not. 0: NO fast channel change, 1: YES fast channel change.  */
#define CONFIG_FLAG_FAST_CHANNEL_CHANGE_POS                 5
/* Indicates if rate control is allowed to restart the GOP structure when a scene change is detected. This is only valid
   in IP GOP. In other GOP structures, the flag is ignored
    0: The rate control should follow the GOP structure, 1: YES RC is allowed to change the GOP structure.              */
#define CONFIG_FLAG_NEW_GOP_ON_SCENE_CHANGE_POS             6


#define GOP_LENGTH_MASK                                     0x0000FFFF          /* Gop length is limited to 16 bits     */

/* GOP STRUCTURE flags */
/* This flags are part of the GopStructure field in Vice2CmdConfigChannel_t */
#define GOP_STRUCTURE_MASK                                  0x0000000F          /* 4 LSBs are for the GOP STRUCTURE definitions */
#define ALLOW_OPEN_GOP_STRUCTURE_MASK                       0x04000000          /* Allows the FW to use an open GOP     */

/* Minimum output frame rate in the current session */
#define CONFIG_FRAME_RATE_LIMIT_MAX_MASK                    0xFFFF0000
#define CONFIG_FRAME_RATE_LIMIT_MAX_SHIFT                   16

/* Minimum output frame rate in the current session */
#define CONFIG_FRAME_RATE_LIMIT_MIN_MASK                    0x0000FFFF
#define CONFIG_FRAME_RATE_LIMIT_MIN_SHIFT                   0

/* Maximum picture height during the current session */
#define CONFIG_MAX_PICTURE_SIZE_IN_PELS_HEIGHT_MASK         0xFFFF0000
#define CONFIG_MAX_PICTURE_SIZE_IN_PELS_HEIGHT_SHIFT        16

/* Maximum picture width during the current session  */
#define CONFIG_MAX_PICTURE_SIZE_IN_PELS_WIDTH_MASK          0x0000FFFF
#define CONFIG_MAX_PICTURE_SIZE_IN_PELS_WIDTH_SHIFT         0

/* Intra refresh frame distance */
#define CONFIG_FORCE_INTRA_MODE_INTRA_REFRESH_FRAME_DISTANCE_MASK 0xFF000000
#define CONFIG_FORCE_INTRA_MODE_INTRA_REFRESH_FRAME_DISTANCE_SHIFT  24

/* Number of Entries To refresh */
#define CONFIG_FORCE_INTRA_MODE_NUMBER_OF_ENTRIES_TO_REFRESH_MASK 0x00FF0000
#define CONFIG_FORCE_INTRA_MODE_NUMBER_OF_ENTRIES_TO_REFRESH_SHIFT  16

/* Refresh Pattern Selected - see ForceIntraPattern_e definition */
#define CONFIG_FORCE_INTRA_MODE_REFRESH_PATTERN_SELECTED_MASK 0x00000002
#define CONFIG_FORCE_INTRA_MODE_REFRESH_PATTERN_SELECTED_SHIFT  1

/* Enable Force Intra Mode */
#define CONFIG_FORCE_INTRA_MODE_ENABLE_MASK                 0x00000001
#define CONFIG_FORCE_INTRA_MODE_ENABLE_SHIFT                0


typedef struct Vice2CmdConfigChannelResponse_t
{
    uint32_t    Command;                                    /* OpCode of the command: VICE2_CMD_*                                */
    uint32_t    Status;                                     /* Status: VICE2_CMD_OK, or VICE2_CMD_ERR_* (see definition above)   */

} Vice2CmdConfigChannelResponse_t;


/* Default A2P parameters values */
#define DEFAULT_A2P_RATE_BUFFER          0                                      /* Default value for RateBufferDelayInMs (0: use FW determined default) */
#define DEFAULT_A2P_MIN_FRAMERATE        ENCODING_FRAME_RATE_1498               /* Default value for MinFrameRateLimit  */
#define DEFAULT_A2P_MAX_FRAMERATE        ENCODING_FRAME_RATE_6000               /* Default value for MaxFrameRateLimit  */
#define DEFAULT_A2P_BVN_RATE             ENCODING_FRAME_RATE_2397               /* Default value for MinAllowedBvnFrameRate    */
#define DEFAULT_A2P_DISABLE_ITFP         0                                      /* Default value for CONFIG_FLAG_ITFP_DISABLED value */
#define DEFAULT_A2P_MAX_PIC_WIDTH_PELS   1920                                   /* Default value for MaxPictureWidthInPels */
#define DEFAULT_A2P_MAX_PIC_HEIGT_PELS   1088                                   /* Default value for MaxPictureHeightInPels */



uint32_t
BVCE_FW_P_CalcVideoA2Pdelay(
    uint32_t            *MaxAllowedA2Pdelay,                /* output max A2P delay allowed for Video */
    uint32_t            Protocol,                           /* encoding standard  */
    uint32_t            Profile,                            /* encoding profile   */
    EncoderLevel_e      Level,                              /* Profile Level      */
    FrameRate_e         FrameRate,                          /* Frame rate in unit of integer (100*frames/sec) */
    uint32_t            MaxBitrate,                         /* Peak output bit rate in bits/sec */
    uint8_t             Mode,                               /* Encoder mode of operation */
    uint32_t            rateBufferDelay,                    /* Input rate buffer delay   */
    FrameRate_e         minFrameRateLimit,                  /* Minimum frame rate allowed during dynamic rate change */
    FrameRate_e         MinAllowedBvnFrameRate,             /* Input BVN frame rate  */
    uint8_t             MultiChannelEnable,                 /* Flag dual channel operation  */
    uint8_t             ITFPenable,                         /* ITFP enable */
    uint8_t             InputType,                          /* Interlaced or Progressive input type */
    uint8_t             GopStruct,                          /* Gop structure */
    uint32_t            PictureWidthInPels,                 /* Max picture resolution allowed */
    uint32_t            PictureHeightInPels,
    uint8_t             MaxNumFNRT
);

uint32_t BVCE_FW_P_CalcNonSecureMemory(
    uint8_t             InputType,                          /* Interlaced or Progressive input type */
    uint8_t             DRAM_STRIPE_WIDTH_Enum,             /* Stripe Width E_STRIPE_WIDTH_128 or E_STRIPE_WIDTH_64 */
    uint32_t            MaxPictureWidthInPels,              /* Max picture resolution allowed */
    uint32_t            MaxPictureHeightInPels
        );
        
uint32_t
BVCE_FW_P_CalcHRDbufferSize(
    uint32_t            Protocol,                           /* encoding standard */
    uint32_t            Profile,                           /* encoding profile  */
    EncoderLevel_e      Level                              /* Profile Level     */
);


/* ---- DEBUG ---- */

/* Processor ID */
#define PROC_ID_PICARC      0
#define PROC_ID_MBARC       1

/* size of the command buffer in bytes. We make sure it's a multiple of 4
 * so that the structure is properly aligned.
 * */
#define COMMAND_BUFFER_SIZE_BYTES   ((32/sizeof(uint32_t))*sizeof(uint32_t))

typedef struct Vice2CmdDebugChannel_t
{
    uint32_t    Command;                                    /* OpCode of the command: VICE2_CMD_* */
    uint32_t    uiChannel_id;                               /* channel number to open (0..65535) */
    uint32_t    ProcID;                                     /* Processor ID see PROC_ID_* above */
    uint8_t     aCommands[COMMAND_BUFFER_SIZE_BYTES];       /* null-terminated string of debug uart commands (for systems without UARTs) */

} Vice2CmdDebugChannel_t;

typedef struct Vice2CmdDebugChannelResponse_t
{
    uint32_t    Command;                                    /* OpCode of the command: VICE2_CMD_* */
    uint32_t    Status;                                     /* Status: VICE2_CMD_OK, or VICE2_CMD_ERR_* (see definition above) */

} Vice2CmdDebugChannelResponse_t;




/* ---- CHANNEL STATUS ---- */

typedef struct Vice2CmdGetChannelStatus_t
{
    uint32_t    Command;                                    /* OpCode of the command: VICE2_CMD_* */
    uint32_t    uiChannel_id;                               /* channel number to open (0..65535) */

} Vice2CmdGetChannelStatus_t;

/* Indicate the state of the encoder picture loop ( debug information ) */
typedef enum
{

    PIC_LOOP_STATE_INIT = 0,                                /* Initialization state, before the encoding starts                             */
    PIC_LOOP_STATE_WAIT_MBP_DEPENDENCIES,                   /* Regular state, waiting for MBP to have enough data and resources available   */
    PIC_LOOP_STATE_CABAC_REQUEST_FLUSH,                     /* Waiting for the Cabac to finish flushing its data at the end of a stream     */
    PIC_LOOP_STATE_BUSY,                                    /* Tasks ran while the MBP HW is working in the background ( 90% time )         */
    PIC_LOOP_STATE_WAIT_MBP_DONE,                           /* Waiting for the End-Of-Picture message from MB-ARC                           */
    PIC_LOOP_STATE_IDLE,                                    /* Tasks ran while the MBP HW is idle (mainly RC)  ( 10% time )                 */
    PIC_LOOP_STATE_WAIT_MBP_MSG_SENT,                       /* Waiting until the DINO message to MB-ARC has been sent                       */
    PIC_LOOP_STATE_PICTURE_DONE,                            /* Picture is complete                                                          */
    PIC_LOOP_STATE_MAX_NUM_STATES
} PicLoopState_t;


typedef struct StatusInfo_t 
{
    uint32_t        uiEventFlags;                           /* These bits are set by FW as events occur.  FW clears these bits */
                                                            /* only after it responds to a GET_STATUS command */
    /* The following fields are overwritten on-the-fly */
    uint32_t        uiTotalErrors;
    uint32_t        NumOfPicsMbArcFinished;
    uint32_t        NumOfPicsVipConfiguredToProcess;
    uint32_t        NumOfPicsVipDroppedDueToFRC;
    uint32_t        NumOfPicsVipDroppedDueToPerformance;
    uint32_t        NumOfPicsVipDroppedDueToChannelNotStarted;
    uint32_t        PicId;
    uint32_t        FwState;
    uint32_t        PictureLoopStatus;
    uint64_t        uiSTCSnapshot;
    uint32_t        NumOfPicsVipDroppedDueToHRDUnderFlow;
    uint32_t        uiEtsDtsOffset;                         /* The ETS to DTS offset for the encode session as determined by RC */
} StatusInfo_t;


typedef struct Vice2CmdGetChannelStatusResponse_t
{
    uint32_t      Command;                                  /* OpCode of the command: VICE2_CMD_* */
    uint32_t      Status;                                   /* Status: VICE2_CMD_OK, or VICE2_CMD_ERR_* (see definition above) */
    StatusInfo_t  StatusInfoStruct;                         /* Copy of the Status Info buffer for the given channel */

} Vice2CmdGetChannelStatusResponse_t;

/* ---- DEVICE STATUS ---- */

typedef struct Vice2CmdGetDeviceStatus_t
{
    uint32_t    Command;                                    /* OpCode of the command: VICE2_CMD_* */

} Vice2CmdGetDeviceStatus_t;

typedef struct DeviceStatusInfo_t 
{
    uint32_t uiEventFlags;                                  /* These bits are set by FW as Event occur.  FW clears these bits */
                                                            /* only after it responds to a GET_STATUS command */

    /* The following fields are overwritten on-the-fly */
    uint32_t uiTotalErrors;
} DeviceStatusInfo_t;


typedef struct Vice2CmdGetDeviceStatusResponse_t
{
    uint32_t            Command;                            /* OpCode of the command: VICE2_CMD_* */
    uint32_t            Status;                             /* Status: VICE2_CMD_OK, or VICE2_CMD_ERR_* (see definition above) */
    DeviceStatusInfo_t  DeviceStatusInfoStruct;             /* Copy of the Device Status Info buffer for the given channel */

} Vice2CmdGetDeviceStatusResponse_t;


/* ==========================================================*/
/* ------------------  COMMAND UNION ------------------- */
typedef union
{
                uint32_t CommandId;

                Vice2CmdInit_t                     Vice2CmdInit;
                Vice2CmdOpenChannel_t              Vice2CmdOpenChannel;
                Vice2CmdStartChannel_t             Vice2CmdStartChannel;
                Vice2CmdStopChannel_t              Vice2CmdStopChannel;
                Vice2CmdCloseChannel_t             Vice2CmdCloseChannel;
                Vice2CmdConfigChannel_t            Vice2CmdConfigChannel;
                Vice2CmdDebugChannel_t             Vice2CmdDebugChannel;
                Vice2CmdGetChannelStatus_t         Vice2CmdGetChannelStatus;

                Vice2CmdInitResponse_t             Vice2CmdInitResponse;
                Vice2CmdOpenChannelResponse_t      Vice2CmdOpenChannelResponse;
                Vice2CmdStartChannelResponse_t     Vice2CmdStartChannelResponse;
                Vice2CmdStopChannelResponse_t      Vice2CmdStopChannelResponse;
                Vice2CmdCloseChannelResponse_t     Vice2CmdCloseChannelResponse;
                Vice2CmdConfigChannelResponse_t    Vice2CmdConfigChannelResponse;
                Vice2CmdDebugChannelResponse_t     Vice2CmdDebugChannelResponse;
                Vice2CmdGetChannelStatusResponse_t Vice2CmdGetChannelStatusResponse;
} HostCommand_t;



/* ==========================================================*/
/* ------------------  MEMORY FOOTPRINTS ------------------- */

/* TODO: Might actually be generated in another file during the
         build process
 */


/* ==========================================================*/
/* ------------------  BUILD TIME CHECKS ------------------- */


#ifndef COMPILE_TIME_ASSERT
/* compile time assertion, can be used to verify sizeof() and offsetof()
 * for example:
 *
 * COMPILE_TIME_ASSERT(sizeof(int32_t) >= 32)
 *
 */
#define COMPILE_TIME_ASSERT(cond)  \
    typedef char VCE_UNIQUE_NAME[(cond)?1:-1]
#define VCE_UNIQUE_NAME         VCE_MAKE_NAME(__LINE__)
#define VCE_MAKE_NAME(line)     VCE_MAKE_NAME2(line)
#define VCE_MAKE_NAME2(line)    compile_time_assert_ ## line

#endif /* COMPILE_TIME_ASSERT */


/* Verify that the API structures don't get bigger than the command buffer size,
 * this will not impact the footprint. */
COMPILE_TIME_ASSERT(sizeof(Vice2CmdInitResponse_t)              <= HOST_CMD_BUFFER_SIZE);
COMPILE_TIME_ASSERT(sizeof(Vice2CmdOpenChannel_t)               <= HOST_CMD_BUFFER_SIZE);
COMPILE_TIME_ASSERT(sizeof(Vice2CmdStartChannel_t)              <= HOST_CMD_BUFFER_SIZE);
COMPILE_TIME_ASSERT(sizeof(Vice2CmdStopChannel_t)               <= HOST_CMD_BUFFER_SIZE);
COMPILE_TIME_ASSERT(sizeof(Vice2CmdCloseChannel_t)              <= HOST_CMD_BUFFER_SIZE);
COMPILE_TIME_ASSERT(sizeof(Vice2CmdConfigChannel_t)             <= HOST_CMD_BUFFER_SIZE);
COMPILE_TIME_ASSERT(sizeof(Vice2CmdDebugChannel_t)              <= HOST_CMD_BUFFER_SIZE);
COMPILE_TIME_ASSERT(sizeof(Vice2CmdGetChannelStatus_t)          <= HOST_CMD_BUFFER_SIZE);
COMPILE_TIME_ASSERT(sizeof(Vice2CmdGetChannelStatusResponse_t)  <= HOST_CMD_BUFFER_SIZE);
COMPILE_TIME_ASSERT(sizeof(StatusInfo_t)                        <= HOST_CMD_BUFFER_SIZE);
COMPILE_TIME_ASSERT((BVCE_FW_P_UserData_PacketDescriptor_MAX_LENGTH & 0x3) == 0);              /* NS, make sure that it's a DWORD multiple */


#ifdef __cplusplus
}
#endif


#endif /* BVCE_FW_API_H__ */

