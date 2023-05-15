/***************************************************************************
 *     Copyright (c) 2004,2005 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 ***************************************************************************
 *  Video Decoder API
 ***************************************************************************/

/* vdec_api.h
 * AVD API
 */

#ifndef  __INC_BXVD_VDEC_API_H__
#define  __INC_BXVD_VDEC_API_H__

#include "bxvd_platform.h"

#define BXVD_P_MAX_HST_CMDQ_SIZE       (64)

#define BXVD_P_DBGLOG_RD_PTR           (0)
#define BXVD_P_DBGLOG_WR_PTR           (4)
#define BXVD_P_DBGLOG_BUFFER_OVERFLOW  (0x80000000)
#define BXVD_P_DBGLOG_ITEM_SIZE        (4)
#define BXVD_P_DBGLOG_INITIAL_INDEX    (2)

/*
Summary:
This is the basic format of all command issued to the AVC
*/
typedef struct
{
      uint32_t 	cmd;
      uint32_t	params[BXVD_P_MAX_HST_CMDQ_SIZE-2];
} BXVD_Cmd, BXVD_Rsp;

/* common response structure */
typedef struct 
{
      unsigned long             ulCmd;
      unsigned long             ulStatus;
} BXVD_P_RspGeneric;


/* Decoder context structure */
typedef struct
{
      BINT_CallbackHandle pCbAVC_MBX_ISR;    	  /* AVC Mailbox L2 ISR */
      BINT_CallbackHandle pCbAVC_PicDataRdy_ISR;  /* AVC Picture Data Ready Display 0 L2 ISR */
      BINT_CallbackHandle pCbAVC_PicDataRdy1_ISR; /* AVC Picture Data Ready Display 1 L2 ISR */
      BINT_CallbackHandle pCbAVC_StillPicRdy_ISR; /* AVC Still Picture L2 ISR */
      BINT_CallbackHandle pCbAVC_Watchdog_ISR;    /* AVC Watchdog L2 ISR */
      BINT_CallbackHandle pCbAVC_VICHReg_ISR;     /* AVC VICH Register L2 ISR */
      BINT_CallbackHandle pCbAVC_VICHSCB_ISR;     /* AVC VICH SCB L2 ISR */
      BINT_CallbackHandle pCbAVC_VICHInstrRd_ISR; /* AVC VICH instruction read L2 ISR */
      BINT_CallbackHandle pCbAVC_VICHILInstrRd_ISR; /* AVC VICH IL instruction read L2 ISR */
      BINT_CallbackHandle pCbAVC_VICHBLInstrRd_ISR; /* AVC VICH BL instruction read L2 ISR */
      BINT_CallbackHandle pCbAVC_StereoSeqError_ISR; /* AVC StereoSeqError L2 ISR */
      BKNI_EventHandle    hFWCmdDoneEvent;        /* FW Command handshake done event */
      bool                bIfBusy;		  /* TRUE if awaiting AVC response */
      bool                bInitialized;	          /* Initialized already? */
      BXVD_Handle 	  hXvd;                   /* Pointer back to XVD command structure */
      unsigned long       ulCmdBufferAddr;	  /* addr of cmd buffer */
      unsigned long       ulRspBufferAddr;	  /* addr of rsp buffer */
} BXVD_DecoderContext;

#define  BXVD_CMD_INITIALIZE         (0x73760001)
#define  BXVD_CMD_CHANNELOPEN        (0x73760002)
#define  BXVD_CMD_CHANNELCLOSE       (0x73760003)
#define  BXVD_CMD_CHANNELSPEED       (0x73760004)
#define  BXVD_CMD_CHANNELSTART       (0x73760005)
#define  BXVD_CMD_CHANNELSTOP        (0x73760006)
#define  BXVD_CMD_CONFIG             (0x73760007)
#define  BXVD_CMD_DBGLOGCONTROL      (0x73760008)
#define  BXVD_CMD_DBGLOGCOMMAND      (0x73760009)

#if BXVD_P_FW_HIM_API
#define  BXVD_CMD_RESPONSE           (0x80000000)
#else
#define  BXVD_CMD_RESPONSE           (0x00000000)
#endif

/* Bit0 is used for picture interrupt. */
#define  BXVD_INTERRUPT_PICTURE      (0x00000001)
#define  BXVD_INTERRUPT_USER_DATA    (0x00000002)
#define  BXVD_INTERRUPT_DISP_MGR     (0x00000004) /* used only for 7401 */
#define  BXVD_INTERRUPT_STILL_PIC    (0x00000008) /* used only for 7401 */
#define  BXVD_INTERRUPT_MAILBOX      (0x00000800) /* used only for 7401 */

/* Error codes */
enum
{
   BXVD_ErrUnknownCommand = -1,

   BXVD_ErrInvalidChannelId = 1,
   BXVD_ErrInvalidParameter,
   BXVD_ErrInvalidMemoryConfig,
   BXVD_ErrTestFailed,
   BXVD_ErrUnableToRunTest,
   BXVD_ErrNoChannels,
   BXVD_ErrInsufficientResources,
   BXVD_ErrInvalidPortNum,
   BXVD_ErrChanInuse
};

/*------------------------------------------------------*
 *    Initialize                                        *
 *------------------------------------------------------*/

#define VDEC_INIT_STRIPE_WIDTH_64       0       /* stripe_width=64 bytes  */
#define VDEC_INIT_STRIPE_WIDTH_128      1       /* stripe_width=128 bytes */
#define VDEC_INIT_STRIPE_WIDTH_256      2       /* stripe_width=256 bytes */

typedef struct
{
   unsigned long  command; /* 0x73760001 */
   unsigned long  cdb_little_endian;
   unsigned long  stripe_width; /* 0=>64, 1=>128 */
   unsigned long  stripe_height;
   unsigned long  bvnf_intr_context_base;
   unsigned long  host_L2_intr_set;
   unsigned long  chip_prod_revision;
   unsigned long  rave_context_reg_size;
   unsigned long  rave_cx_hold_clr_status;
   unsigned long  rave_packet_count;
} BXVD_Cmd_Initialize;


typedef struct
{
   unsigned long  command;
   unsigned long  status;
   unsigned long  sw_version;
#if BXVD_P_FW_HIM_API
   unsigned long  display_info_0_offset;
   unsigned long  display_info_1_offset;
#else
   unsigned long  dms_delivery_address0;
   unsigned long  dms_delivery_address1;
#endif
} BXVD_Rsp_Initialize;


/*------------------------------------------------------*
 *    ChannelOpen                                       *
 *------------------------------------------------------*/

typedef struct
{
   unsigned long  command; /* 0x73760002 */
   unsigned long  channel_number;
   unsigned long  max_resolution_enum;
   unsigned long  still_picture_mode;
   unsigned long  context_memory_base;
   unsigned long  context_memory_size;
   unsigned long  video_memory_base;
   unsigned long  video_block_size;
   unsigned long  video_block_count;
   unsigned long  cabac_memory_base;
   unsigned long  cabac_memory_size;
   unsigned long  bl_mv_store_base;
   unsigned long  bl_mv_store_size;
   unsigned long  reserved;
   unsigned long  cabac_wl_base;
   unsigned long  cabac_wl_size;
   unsigned long  direct_mode_storage_base;
   unsigned long  direct_mode_storage_size;
   unsigned long  il_wl_base;
   unsigned long  il_wl_size;
   unsigned long  bl_video_store_base;
   unsigned long  bl_video_store_size;
} BXVD_Cmd_ChannelOpen;


#if BXVD_P_FW_HIM_API

typedef struct
{
   unsigned long  command;
   unsigned long  status;
   unsigned long  picture_delivery_buffer;
   unsigned long  picture_release_buffer;
   unsigned long  drop_count_byte_offset;
   unsigned long  avd_status_addr;
   unsigned long  shadow_write_byte_offset;
   unsigned long  delivery_q_read_byte_offset;
   unsigned long  delivery_q_write_byte_offset;
   unsigned long  release_q_read_byte_offset;
   unsigned long  release_q_write_byte_offset;
} BXVD_Rsp_ChannelOpen;

#else

typedef struct
{
   unsigned long  command;
   unsigned long  status;
   unsigned long  picture_delivery_buffer;
   unsigned long  picture_release_buffer;
   unsigned long  dm_return_address;
   unsigned long  avd_status_address;
} BXVD_Rsp_ChannelOpen;

#endif /* ~BXVD_P_FW_HIM_API */


/*------------------------------------------------------*
 *    ChannelClose                                      *
 *------------------------------------------------------*/

typedef struct
{
   unsigned long  command; /* 0x73760003 */
   unsigned long  channel_number;
} BXVD_Cmd_ChannelClose;

typedef struct
{
   unsigned long  command;
   unsigned long  status;
} BXVD_Rsp_ChannelClose;


/*------------------------------------------------------*
 *    ChannelSpeed                                      *
 *------------------------------------------------------*/

#define VDEC_SPEED_NORMAL       0       /* Normal playback                */
#define VDEC_SPEED_REFONLY      1       /* Decode reference pictures only */
#define VDEC_SPEED_IPONLY       2       /* Decode IP pictures only        */
#define VDEC_SPEED_IONLY        3       /* Decode I pictures only         */

typedef struct
{
   unsigned long  command; /* 0x73760004 */
   unsigned long  channel_number;
   unsigned long  speed;
} BXVD_Cmd_ChannelSpeed;

typedef struct
{
   unsigned long  command;
   unsigned long  status;
} BXVD_Rsp_ChannelSpeed;


/*------------------------------------------------------*
 *    ChannelStart                                      *
 *------------------------------------------------------*/

#define VDEC_CHANNEL_MODE_SPARSE_NORMAL               0x0000  /* Normal playback                      */
#define VDEC_CHANNEL_MODE_SPARSE_NOSKIP               0x0001  /* Decode IPB pictures, no skip         */
#define VDEC_CHANNEL_MODE_SPARSE_REFONLY              0x0002  /* Decode Ref pictures only             */
#define VDEC_CHANNEL_MODE_SPARSE_IPONLY               0x0003  /* Decode IP pictures only              */
#define VDEC_CHANNEL_MODE_SPARSE_IONLY                0x0004  /* Decode I pictures only               */
#define VDEC_CHANNEL_MODE_NON_LEGACY                  0x0008  /* Non Legacy mode enable               */
#define VDEC_CHANNEL_MODE_CLEAN_HITS                  0x0010  /* Clean HITS mode enable               */
#define VDEC_CHANNEL_MODE_HITS                        0x0020  /* HITS mode enable                     */ 
#define VDEC_CHANNEL_MODE_ZERO_DELAY                  0x0040  /* Zero delay output mode               */ 
#define VDEC_CHANNEL_MODE_BLU_RAY_DECODE              0x0080  /* Blu-ray decode mode                  */ 
#define VDEC_CHANNEL_MODE_TIMESTAMP_DISPLAY_ORDER     0x0100  /* Timestamp decode mode                */ 
#define VDEC_CHANNEL_MODE_IFRAME_AS_RAP               0x0200  /* Treat I-Frame as RAP for AVC         */ 
#define VDEC_CHANNEL_MODE_ENA_ERROR_CONCEALMENT       0x0400  /* Enable AVC error concealment         */ 
#define VDEC_CHANNEL_MODE_ENA_IONLY_FIELD_OUTPUT      0x0800  /* Enable I Only Field output           */ 
#define VDEC_CHANNEL_MODE_DISABLE_DPB_OUTPUT_DELAY    0x1000  /* Ignore the DPB output delay syntax   */
#define VDEC_CHANNEL_MODE_ENA_SEI_FRAME_PACK          0x2000  /* Enable SEI Frame Packing             */
#define VDEC_CHANNEL_MODE_DISABLE_P_SKIP              0x4000  /* Disable P-Skip mode                  */
#define VDEC_CHANNEL_MODE_FILE_FORMAT                 0x8000  /* MKV File format hack h264            */
#define VDEC_CHANNEL_MODE_3D_SVC_DECODE              0x10000  /* Enable 3D SVC mode                   */
#define VDEC_CHANNEL_MODE_SW_COEF_AVC_DECODE         0x40000  /* Enable SW coefficient AVC decode     */
#define VDEC_CHANNEL_MODE_IGN_NUM_REORDR_FRM_ZERO    0x80000  /* Ignore num reorder frame equal zero  */
#define VDEC_CHANNEL_MODE_EARLY_PIC_DELIVERY        0x100000  /* Pre-mature picture delivery mode     */
#define VDEC_CHANNEL_MODE_ENA_UD_BTP                0x200000  /* Enable UD in BTP mode                */

#define BXVD_P_VEC_UNUSED 0xff

typedef struct
{
   unsigned long  command; /* 0x73760005 */
   unsigned long  protocol;
   unsigned long  channel_mode; /* See VDEC_CHANNEL_MODES defined above */
   unsigned long  vec_index;
   unsigned long  channel_number;

   unsigned long  rave_ctxt_base;
   unsigned long  rave_ctxt_base_ext;
} BXVD_Cmd_ChannelStart;

typedef struct
{
   unsigned long  command;
   unsigned long  status;
   unsigned long  frames_outstanding;
} BXVD_Rsp_ChannelStart;


/*------------------------------------------------------*
 *    ChannelStop                                       *
 *------------------------------------------------------*/

typedef struct
{
   unsigned long  command; /* 0x73760006 */
   unsigned long  channel_number;
} BXVD_Cmd_ChannelStop;

typedef struct
{
   unsigned long  command;
   unsigned long  status;
} BXVD_Rsp_ChannelStop;

/*------------------------------------------------------*
 *    ChannelConfig                                     *
 *------------------------------------------------------*/

typedef struct
{
   unsigned long  command; /* 0x73760007 */
   unsigned long  vec_index;
   unsigned long  interrupt_mask;
} BXVD_Cmd_Config;

typedef struct
{
   unsigned long  command;
   unsigned long  status;
} BXVD_Rsp_Config;

/*---------------------------------------------*
 *    Decoder Debug LOG Start/Stop Command     *
 *---------------------------------------------*/

typedef enum
{
   BXVD_DBGLOG_eSTOP = 0,
   BXVD_DBGLOG_eSTART = 1
} BXVD_DBG_LOG;

typedef struct
{
   unsigned int  command;  /* 0x73760008 */
   unsigned int  logStart;
   unsigned int  dbglog_memory_base; /* 4 byte aligned memory address */
   unsigned int  dbglog_memory_size; /* Size in bytes */

} BXVD_Cmd_DbgLogControl;

typedef struct
{
   unsigned int	command;
   unsigned int	status;
} BXVD_Rsp_DbgLogControl;


/*---------------------------------------------*
 *    Decoder DEBUG LOG Command                *
 *---------------------------------------------*/

typedef struct
{
   unsigned int  command;  /* 0x73760009 */
   char logCmd[40];
} BXVD_Cmd_DbgLogCommand;

typedef struct
{
   unsigned int command;
   unsigned int status;
} BXVD_Rsp_DbgLogCommand;


typedef union
{
      BXVD_Cmd cmd;
      BXVD_Cmd_Initialize    init;
      BXVD_Cmd_ChannelOpen   channelOpen;
      BXVD_Cmd_ChannelClose  channelClose;
      BXVD_Cmd_ChannelSpeed  channelSpeed;
      BXVD_Cmd_ChannelStart  channelStart;
      BXVD_Cmd_ChannelStop   channelStop;
      BXVD_Cmd_Config        config;
      BXVD_Cmd_DbgLogControl dbgLogControl;
      BXVD_Cmd_DbgLogCommand dbgLogCommand;
} BXVD_FW_Cmd;

typedef union
{
      BXVD_Rsp rsp;
      BXVD_Rsp_Initialize    init;
      BXVD_Rsp_ChannelOpen   channelOpen;
      BXVD_Rsp_ChannelClose  channelClose;
      BXVD_Rsp_ChannelSpeed  channelSpeed;
      BXVD_Rsp_ChannelStart  channelStart;
      BXVD_Rsp_ChannelStop   channelStop;
      BXVD_Rsp_Config        config;
      BXVD_Rsp_DbgLogControl dbgLogControl;
      BXVD_Rsp_DbgLogCommand dbgLogCommand;
} BXVD_FW_Rsp;


#endif /*__INC_BXVD_VDEC_API_H__ */
