/******************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsid_msg.c $
* $brcm_Revision: Hydra_Software_Devel/12 $
* $brcm_Date: 11/2/12 4:40p $
*
* Module Description:
*   See Module Overview below.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/sid/src/bsid_msg.c $
* 
* Hydra_Software_Devel/12   11/2/12 4:40p fbasso
* SW7425-4146: removed incorrect flush call.
* 
* Hydra_Software_Devel/11   9/17/12 2:59p fbasso
* SW7445-34: added support for 7445 to sid.
*
* Hydra_Software_Devel/10   8/24/12 5:02p fbasso
* SW7425-3620: replaced un-cached memory access with cached memory
* access.
*
* Hydra_Software_Devel/9   8/1/12 1:41p fbasso
* SW7425-2449: minor clean up.
*
* Hydra_Software_Devel/8   7/31/12 5:02p jtna
* SW7425-2449: fix BSID power management
*
* Hydra_Software_Devel/7   5/1/12 1:08p fbasso
* SW7584-18: added support for 7584 to SID
*
* Hydra_Software_Devel/6   4/19/12 12:13p fbasso
* SW7429-128: added SID PI support.
*
* Hydra_Software_Devel/5   4/2/12 10:26a fbasso
* SW7425-2780: added support for JPEG-B decode to SID. Although SID has
* code in place to decode both images contained in the JPEG-B frame,
* only the first image is decoded for now and sent to XDM. As soon as
* interlaced mode is added in the SID->XDM->VDC path, this limitation
* will be removed and both images will be decoded and sent to display.
*
* Hydra_Software_Devel/4   3/5/12 10:45a fbasso
* SW7435-26: added support for 7435 to SID PI.
*
* Hydra_Software_Devel/3   2/23/12 5:12p fbasso
* SW7425-2112: fixed function used to calculate distance between ITB read
* and valid pointers. Fixed raced condition in fw when accessing CDB
* wrap pointer around wrap time.
*
* Hydra_Software_Devel/2   2/21/12 2:20p fbasso
* SW7425-2112: added support for MJPEG video decode. Merging new feature
* to Hydra_Software_Devel.
*
* Hydra_Software_Devel/SW7425-2112/8   2/20/12 7:17p fbasso
* SW7425-2112: addressed big endianess issue in itb parsing.
*
* Hydra_Software_Devel/SW7425-2112/7   2/20/12 3:51p fbasso
* SW7425-2112: remove run time option to configure channel change output
* behavior; rather than sid, the behavior upon channel change is decided
* by xdm configuration; sid will simply follow xdm.
*
* Hydra_Software_Devel/SW7425-2112/6   2/16/12 4:51p fbasso
* SW7425-2112: API clean-up; added mechanism to share SID FW/HW between
* still channels and motion channel; added support for channel change
* output mode (hold last picture)
*
* Hydra_Software_Devel/SW7425-2112/5   2/15/12 11:32a fbasso
* SW7425-2112: configure ePulldown variable in unified picture structure
* to frame progressive to resolve incorrect cropping at display time.
* Introduced new function BSID_GetRaveItbCbdConfigInfo to retrieve
* default RAVE configuration to use during video decode.
*
* Hydra_Software_Devel/SW7425-2112/4   2/14/12 3:18p fbasso
* SW7425-2112: correct the time at which SID prepare the unified picture
* structure. It must be before SID declares that it has one new picture
* available for display rather than at GetNextPicture time.
*
* Hydra_Software_Devel/SW7425-2112/3   2/10/12 4:16p fbasso
* SW7425-2112: update SID to XDM path to use latest BAVC_MFD_Picture
* format protocol to send buffer to VDC.
*
* Hydra_Software_Devel/SW7425-2112/2   2/10/12 10:54a fbasso
* SW7425-2112: added support for motion jpeg decode using XPT RAVE to
* provide input data to SID fw/hw and Xdm interface to supply SID output
* pictures to VDC.
*
* Hydra_Software_Devel/SW7425-2112/1   1/26/12 5:46p fbasso
* SW7425-2112: rave support first draft
*
* Hydra_Software_Devel/1   1/20/12 2:05p fbasso
* SW7425-1619: release SID multi channel interface to
* Hydra_Software_Devel
*
* Hydra_Software_Devel/sid_channel_interface_devel/3   1/3/12 4:53p fbasso
* SW7425-1619: added isr version of BSID_SetDmaChunkInfo; added support
* for 7346
*
* Hydra_Software_Devel/sid_channel_interface_devel/2   1/2/12 4:52p fbasso
* SW7425-674: modified handling ofinput buffer in streaming mode.
* Extended to support up to sixteen channels
*
* Hydra_Software_Devel/sid_channel_interface_devel/1   10/27/11 12:43p fbasso
* SW7425-1619: add queue based interface and multi channel support
*
* Hydra_Software_Devel/19   8/23/11 10:33a fbasso
* SW7231-320: Add SID PI support for 7231 B0
*
* Hydra_Software_Devel/18   8/22/11 4:29p fbasso
* SWDTV-8264: returning Transparency RGB color for PNG decode when
* present.
*
* Hydra_Software_Devel/17   6/21/11 11:13a parijat
* SW7425-674: Added $brcm_Log: /magnum/portinginterface/sid/src/bsid_msg.c $
* SW7425-674: Added 
* SW7425-674: Added Hydra_Software_Devel/12   11/2/12 4:40p fbasso
* SW7425-674: Added SW7425-4146: removed incorrect flush call.
* SW7425-674: Added 
* SW7425-674: Added Hydra_Software_Devel/11   9/17/12 2:59p fbasso
* SW7425-674: Added SW7445-34: added support for 7445 to sid.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/10   8/24/12 5:02p fbasso
* SW7425-674: Added SW7425-3620: replaced un-cached memory access with cached memory
* SW7425-674: Added access.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/9   8/1/12 1:41p fbasso
* SW7425-674: Added SW7425-2449: minor clean up.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/8   7/31/12 5:02p jtna
* SW7425-674: Added SW7425-2449: fix BSID power management
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/7   5/1/12 1:08p fbasso
* SW7425-674: Added SW7584-18: added support for 7584 to SID
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/6   4/19/12 12:13p fbasso
* SW7425-674: Added SW7429-128: added SID PI support.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/5   4/2/12 10:26a fbasso
* SW7425-674: Added SW7425-2780: added support for JPEG-B decode to SID. Although SID has
* SW7425-674: Added code in place to decode both images contained in the JPEG-B frame,
* SW7425-674: Added only the first image is decoded for now and sent to XDM. As soon as
* SW7425-674: Added interlaced mode is added in the SID->XDM->VDC path, this limitation
* SW7425-674: Added will be removed and both images will be decoded and sent to display.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/4   3/5/12 10:45a fbasso
* SW7425-674: Added SW7435-26: added support for 7435 to SID PI.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/3   2/23/12 5:12p fbasso
* SW7425-674: Added SW7425-2112: fixed function used to calculate distance between ITB read
* SW7425-674: Added and valid pointers. Fixed raced condition in fw when accessing CDB
* SW7425-674: Added wrap pointer around wrap time.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/2   2/21/12 2:20p fbasso
* SW7425-674: Added SW7425-2112: added support for MJPEG video decode. Merging new feature
* SW7425-674: Added to Hydra_Software_Devel.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/8   2/20/12 7:17p fbasso
* SW7425-674: Added SW7425-2112: addressed big endianess issue in itb parsing.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/7   2/20/12 3:51p fbasso
* SW7425-674: Added SW7425-2112: remove run time option to configure channel change output
* SW7425-674: Added behavior; rather than sid, the behavior upon channel change is decided
* SW7425-674: Added by xdm configuration; sid will simply follow xdm.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/6   2/16/12 4:51p fbasso
* SW7425-674: Added SW7425-2112: API clean-up; added mechanism to share SID FW/HW between
* SW7425-674: Added still channels and motion channel; added support for channel change
* SW7425-674: Added output mode (hold last picture)
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/5   2/15/12 11:32a fbasso
* SW7425-674: Added SW7425-2112: configure ePulldown variable in unified picture structure
* SW7425-674: Added to frame progressive to resolve incorrect cropping at display time.
* SW7425-674: Added Introduced new function BSID_GetRaveItbCbdConfigInfo to retrieve
* SW7425-674: Added default RAVE configuration to use during video decode.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/4   2/14/12 3:18p fbasso
* SW7425-674: Added SW7425-2112: correct the time at which SID prepare the unified picture
* SW7425-674: Added structure. It must be before SID declares that it has one new picture
* SW7425-674: Added available for display rather than at GetNextPicture time.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/3   2/10/12 4:16p fbasso
* SW7425-674: Added SW7425-2112: update SID to XDM path to use latest BAVC_MFD_Picture
* SW7425-674: Added format protocol to send buffer to VDC.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/2   2/10/12 10:54a fbasso
* SW7425-674: Added SW7425-2112: added support for motion jpeg decode using XPT RAVE to
* SW7425-674: Added provide input data to SID fw/hw and Xdm interface to supply SID output
* SW7425-674: Added pictures to VDC.
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/SW7425-2112/1   1/26/12 5:46p fbasso
* SW7425-674: Added SW7425-2112: rave support first draft
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/1   1/20/12 2:05p fbasso
* SW7425-674: Added SW7425-1619: release SID multi channel interface to
* SW7425-674: Added Hydra_Software_Devel
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/sid_channel_interface_devel/3   1/3/12 4:53p fbasso
* SW7425-674: Added SW7425-1619: added isr version of BSID_SetDmaChunkInfo; added support
* SW7425-674: Added for 7346
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/sid_channel_interface_devel/2   1/2/12 4:52p fbasso
* SW7425-674: Added SW7425-674: modified handling ofinput buffer in streaming mode.
* SW7425-674: Added Extended to support up to sixteen channels
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/sid_channel_interface_devel/1   10/27/11 12:43p fbasso
* SW7425-674: Added SW7425-1619: add queue based interface and multi channel support
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/19   8/23/11 10:33a fbasso
* SW7425-674: Added SW7231-320: Add SID PI support for 7231 B0
* SW7425-674: Added
* SW7425-674: Added Hydra_Software_Devel/18   8/22/11 4:29p fbasso
* SW7425-674: Added SWDTV-8264: returning Transparency RGB color for PNG decode when
* SW7425-674: Added present.
*
* Hydra_Software_Devel/1   5/03/06 09:19a fbasso
* - Created this file
******************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#include "bsid.h"
#include "bsid_priv.h"
#include "bsid_msg.h"

BDBG_MODULE(BSID_MSG);

#if ((BCHP_CHIP == 7440) && (BCHP_VER >= BCHP_VER_A0))
#define ARC_UART_FREQUENCY                                       (200*1000000) /* sid arc uart frequency */
#elif (((BCHP_CHIP==3548) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==3556) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==35230) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==35125) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==7422) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==7425) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==35233) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==7231) && (BCHP_VER >= BCHP_VER_B0)) || \
       ((BCHP_CHIP==7344) && (BCHP_VER >= BCHP_VER_B0)) || \
       ((BCHP_CHIP==7346) && (BCHP_VER >= BCHP_VER_B0)) || \
       ((BCHP_CHIP==7435) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==7584) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==7429) && (BCHP_VER >= BCHP_VER_A0)) || \
       ((BCHP_CHIP==7445) && (BCHP_VER >= BCHP_VER_A0)))
#define ARC_UART_FREQUENCY                                       (216*1000000) /* sid arc uart frequency */
#endif

#define ARC_UART_BAUDRATE                                             (115200) /* sid arc uart baud rate */
#define BSID_SIGNAL_MBX_RESPONSE_READY                              0x80000000 /* response ready isr */
#define BSID_INTGEN_ENABLE_PINS                                     0x8000ffff /* mbx | sixteen channels enabled */

#define BSID_ARC_INIT_MSG_ENABLE                                         false /* turn on arc dbg message on init command */
#define BSID_ARC_DECODE_MSG_ENABLE                                       false /* turn on arc dbg message on decode command */
#define BSID_ARC_GET_INFO_MSG_ENABLE                                     false /* turn on arc dbg message on getinfo command */
#define BSID_ARC_DECODE_SEGMENT_MSG_ENABLE                               false /* turn on arc dbg message on decode command */
#define BSID_ARC_OPENCHANNEL_MSG_ENABLE                                  false /* turn on arc dbg message on open channel */
#define BSID_ARC_CLOSECHANNEL_MSG_ENABLE                                 false /* turn on arc dbg message on close channel */
#define BSID_ARC_FLUSHCHANNEL_MSG_ENABLE                                 false /* turn on arc dbg message on flush channel */
#define BSID_ARC_SYNCCHANNEL_MSG_ENABLE                                  false /* turn on arc dbg message on flush channel */
#define BSID_ARC_STARTDECODE_MSG_ENABLE                                  false /* turn on arc dbg message on startdecode channel */
#define BSID_ARC_STOPDECODE_MSG_ENABLE                                   false /* turn on arc dbg message on stopdecode channel */
#define BSID_BITPERPIXEL_MAX                                                 9
/*#define BSID_TRACE_PLAYBACK*/
/*#define BSID_SAVE_BUFFER*/
#define BSID_MAILBOX_MAX_WAITING_TIME                                     5000 /* max time to wait on mailbox answer before erroring out */

enum {
  SID_OUT_YUVA       = 0,
  SID_OUT_YUYV       = 1,
  SID_OUT_GREY       = 2,
  SID_OUT_PALETTE    = 3,
  SID_OUT_RGBA       = 4,
  SID_OUT_GREY_ALPHA = 5,
  SID_OUT_LAST
} BSID_ArcOutFormat;

static BPXL_Format ae_ClutPxlFmtPi2Pxl[2][2] =
{
    {
        BPXL_eA8_R8_G8_B8,
        BPXL_eA8_Y8_Cb8_Cr8
    },

    {
        BPXL_eB8_G8_R8_A8,
        BPXL_eCr8_Cb8_Y8_A8
    }
};

typedef enum BSID_Pxl {
    BSID_Pxl_eInv = 0,
    BSID_Pxl_eACbCr,
    BSID_Pxl_eYCbYCr,
    BSID_Pxl_eP1,
    BSID_Pxl_eP2,
    BSID_Pxl_eP4,
    BSID_Pxl_eP8,
    BSID_Pxl_eARGB,
    BSID_Pxl_eAP
} BSID_Pxl;

static BPXL_Format ae_PxlFmtPi2Pxl[2][BSID_BITPERPIXEL_MAX] =
{
    /* 0: false if for little endian */
    {
        BERR_UNKNOWN,
#if ((BCHP_CHIP==7440) && (BCHP_VER == BCHP_VER_A0))
        BPXL_eA8_Cr8_Cb8_Y8,  /* OUT_YUVA, 8 */
#else
        BPXL_eA8_Y8_Cb8_Cr8,  /* OUT_YUVA, 8 */
#endif
        BPXL_eY18_Cr8_Y08_Cb8,
        BPXL_eP1,
        BPXL_eP2,
        BPXL_eP4,
        BPXL_eP8,
#if ((BCHP_CHIP==7440) && (BCHP_VER == BCHP_VER_A0))
        BPXL_eA8_B8_G8_R8,  /* OUT_RGBA, 8 */
#else
        BPXL_eA8_R8_G8_B8,  /* OUT_RGBA, 8 */
#endif
        BPXL_eA8_P8
    }

    /* 1: true if for big endian */
    ,
    {
        BERR_UNKNOWN,
#if ((BCHP_CHIP==7440) && (BCHP_VER == BCHP_VER_A0))
        BPXL_eY8_Cb8_Cr8_A8,  /* OUT_YUVA, 8 */
#else
        BPXL_eCr8_Cb8_Y8_A8,  /* OUT_YUVA, 8 */
#endif
        BPXL_eCb8_Y08_Cr8_Y18,
        BPXL_eP1,
        BPXL_eP2,
        BPXL_eP4,
        BPXL_eP8,
#if ((BCHP_CHIP==7440) && (BCHP_VER == BCHP_VER_A0))
        BPXL_eR8_G8_B8_A8,  /* OUT_RGBA, 8 */
#else
        BPXL_eB8_G8_R8_A8,  /* OUT_RGBA, 8 */
#endif
        BPXL_eA8_P8
    }
};

static uint8_t ae_PxlFmtArc2Pi[SID_OUT_LAST][BSID_BITPERPIXEL_MAX] =
{
    {
        BSID_Pxl_eInv,  /* OUT_YUVA, 0 */
        BSID_Pxl_eInv,  /* OUT_YUVA, 1 */
        BSID_Pxl_eInv,  /* OUT_YUVA, 2 */
        BSID_Pxl_eInv,  /* OUT_YUVA, 3 */
        BSID_Pxl_eInv,  /* OUT_YUVA, 4 */
        BSID_Pxl_eInv,  /* OUT_YUVA, 5 */
        BSID_Pxl_eInv,  /* OUT_YUVA, 6 */
        BSID_Pxl_eInv,  /* OUT_YUVA, 7 */
        BSID_Pxl_eACbCr /* OUT_YUVA, 7 */
    },
    {
        BSID_Pxl_eInv,   /* OUT_YUYV, 0 */
        BSID_Pxl_eInv,   /* OUT_YUYV, 1 */
        BSID_Pxl_eInv,   /* OUT_YUYV, 2 */
        BSID_Pxl_eInv,   /* OUT_YUYV, 3 */
        BSID_Pxl_eInv,   /* OUT_YUYV, 4 */
        BSID_Pxl_eInv,   /* OUT_YUYV, 5 */
        BSID_Pxl_eInv,   /* OUT_YUYV, 6 */
        BSID_Pxl_eInv,   /* OUT_YUYV, 7 */
        BSID_Pxl_eYCbYCr /* OUT_YUVA, 8 */
    },
    {
        BSID_Pxl_eInv, /* OUT_GREY, 0 */
        BSID_Pxl_eP1,  /* OUT_GREY, 1 */
        BSID_Pxl_eP2,  /* OUT_GREY, 2 */
        BSID_Pxl_eInv, /* OUT_GREY, 3 */
        BSID_Pxl_eP4,  /* OUT_GREY, 4 */
        BSID_Pxl_eInv, /* OUT_GREY, 5 */
        BSID_Pxl_eInv, /* OUT_GREY, 6 */
        BSID_Pxl_eInv, /* OUT_GREY, 7 */
        BSID_Pxl_eP8   /* OUT_GREY, 8 */
    },
    {
        BSID_Pxl_eInv, /* OUT_PALETTE, 0 */
        BSID_Pxl_eP1,  /* OUT_PALETTE, 1 */
        BSID_Pxl_eP2,  /* OUT_PALETTE, 2 */
        BSID_Pxl_eInv, /* OUT_PALETTE, 3 */
        BSID_Pxl_eP4,  /* OUT_PALETTE, 4 */
        BSID_Pxl_eInv, /* OUT_PALETTE, 5 */
        BSID_Pxl_eInv, /* OUT_PALETTE, 6 */
        BSID_Pxl_eInv, /* OUT_PALETTE, 7 */
        BSID_Pxl_eP8   /* OUT_PALETTE, 8 */
    },
    {
        BSID_Pxl_eInv, /* OUT_RGBA, 0 */
        BSID_Pxl_eInv, /* OUT_RGBA, 1 */
        BSID_Pxl_eInv, /* OUT_RGBA, 2 */
        BSID_Pxl_eInv, /* OUT_RGBA, 3 */
        BSID_Pxl_eInv, /* OUT_RGBA, 4 */
        BSID_Pxl_eInv, /* OUT_RGBA, 5 */
        BSID_Pxl_eInv, /* OUT_RGBA, 6 */
        BSID_Pxl_eInv, /* OUT_RGBA, 7 */
        BSID_Pxl_eARGB /* OUT_RGBA, 7 */
    },
    {
        BSID_Pxl_eInv, /* OUT_GREY_ALPHA, 0 */
        BSID_Pxl_eInv, /* OUT_GREY_ALPHA, 1 */
        BSID_Pxl_eInv, /* OUT_GREY_ALPHA, 2 */
        BSID_Pxl_eInv, /* OUT_GREY_ALPHA, 3 */
        BSID_Pxl_eInv, /* OUT_GREY_ALPHA, 4 */
        BSID_Pxl_eInv, /* OUT_GREY_ALPHA, 5 */
        BSID_Pxl_eInv, /* OUT_GREY_ALPHA, 6 */
        BSID_Pxl_eInv, /* OUT_GREY_ALPHA, 7 */
        BSID_Pxl_eAP   /* OUT_GREY_ALPHA, 8 */
    }
};

#ifdef BSID_SAVE_BUFFER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static pthread_t SaveImagesThread;
void *fSaveImagesThread(void *pCtx);
void BSID_SaveImageData(BSID_ChannelHandle  hSidCh, uint32_t image_index,
    uint32_t output_buffer_offset, uint32_t output_buffer_width,
    uint32_t output_buffer_pitch, uint32_t output_buffer_height,
    uint32_t input_buffer_offset[2], uint32_t input_buffer_size[2],
    uint32_t num_input_buffer);
#endif

void BSID_UpdatePlaybackInfo(BSID_ChannelHandle  hSidCh);
void BSID_PrepareNextUnifiedPicture(BSID_ChannelHandle  hSidCh, BSID_PlaybackQueueEntry *psPbQueueEntry);

/******************************************************************************
* Function name: BSID_P_SetMailboxInfo
*
* Comments:
*
******************************************************************************/
BERR_Code BSID_P_SetMailboxInfo(BSID_Handle hSid)
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BSID_P_SetMailboxInfo);

    hSid->sMailbox.hMailboxEvent = NULL;
    hSid->sMailbox.pv_CmdQVirtAddr = hSid->sFwHwConfig.sMbxMemory.pui32_VirtAddr;
    hSid->sMailbox.pv_RspQVirtAddr = hSid->sFwHwConfig.sMbxMemory.pui32_VirtAddr + (hSid->sFwHwConfig.sMbxMemory.ui32_Size >> 3);
    hSid->sMailbox.ui32_CmdQPhisAddr  = hSid->sFwHwConfig.sMbxMemory.ui32_PhisAddr;
    hSid->sMailbox.ui32_RspQPhisAddr  = hSid->sFwHwConfig.sMbxMemory.ui32_PhisAddr + (hSid->sFwHwConfig.sMbxMemory.ui32_Size >> 1);

#ifdef BSID_USE_CACHED_MEMORY
    retCode = BMEM_ConvertAddressToCached(hSid->hMem, hSid->sMailbox.pv_CmdQVirtAddr, &hSid->sMailbox.pv_CmdQCachedAddr);
    if (retCode != BERR_SUCCESS)
    {
       BDBG_ERR(("BSID_P_SetMailboxInfo: BMEM_ConvertAddressToCached for CmdQ failed"));
       return BERR_TRACE(BERR_UNKNOWN);
    }

    retCode = BMEM_ConvertAddressToCached(hSid->hMem, hSid->sMailbox.pv_RspQVirtAddr, &hSid->sMailbox.pv_RspQCachedAddr);
    if (retCode != BERR_SUCCESS)
    {
       BDBG_ERR(("BSID_P_SetMailboxInfo: BMEM_ConvertAddressToCached for CmdQ failed"));
       return BERR_TRACE(BERR_UNKNOWN);
    }
#endif

    BDBG_LEAVE(BSID_P_SetMailboxInfo);

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_P_SendCommand
*   -
* Comments:
*   -
******************************************************************************/
static BERR_Code BSID_P_SendCommand(
    BSID_Handle hSid,
    const void *pCmd,
    uint32_t    ui32_CmdSize,
    void       *pRsp,
    uint32_t    ui32_RspSize,
    bool        waitForRsp)
{
   BERR_Code retCode = BERR_SUCCESS;
#ifndef BSID_USE_CACHED_MEMORY
   BSID_Cmd_Hdr ui32_ReadBack;
#endif

   BDBG_ENTER(BSID_P_SendCommand);

   BDBG_ASSERT(pCmd);

#ifndef BSID_USE_CACHED_MEMORY
   BSTD_UNUSED( ui32_CmdSize );
   BKNI_Memcpy((void *)&ui32_ReadBack, (void *)pCmd, sizeof(BSID_Cmd_Hdr));
#else
   /* flush command to dram to make available to SID */
   BMEM_FlushCache( hSid->hMem, (void *)pCmd, ui32_CmdSize);
#endif

   /* reset mailbox event */
   BKNI_ResetEvent(hSid->sMailbox.hMailboxEvent);

   /* fire command to hw */
   BREG_Write32(hSid->hReg, BCHP_SID_ARC_HST2CPU_MBX, hSid->sMailbox.ui32_CmdQPhisAddr);

   /* wait for response? */
   if (waitForRsp == true)
   {
       BDBG_ASSERT(pRsp);

       /* wait for mailbox event */
       retCode = BKNI_WaitForEvent(hSid->sMailbox.hMailboxEvent, BSID_MAILBOX_MAX_WAITING_TIME);
       if (retCode != BERR_SUCCESS)
       {
           BDBG_ERR(("BSID_P_SendCommand: wait for event timeout reached"));
           return BERR_TRACE(BERR_UNKNOWN);
       }

       if (pRsp)
       {
#ifndef BSID_USE_CACHED_MEMORY
           BSTD_UNUSED( ui32_RspSize );
#else
           /* flush to invalidate rsp contents and force access to sdram to pick SID rsp */
           BMEM_FlushCache( hSid->hMem, pRsp, ui32_RspSize);
#endif /* read uncached */

           /* sanity check: are we receiving the rsp corresponding to the cmd just sent? */
           if (((BSID_Rsp_Hdr *)pRsp)->header != (((BSID_Cmd_Hdr *)pCmd)->header))
           {
               BDBG_ERR(("BSID_P_SendCommand: rsp header mismatch or header trashed rcvd: 0x%x, exp: 0x%x",
               ((BSID_Rsp_Hdr *)pRsp)->header,
               (((BSID_Cmd_Hdr *)pCmd)->header)));
               return BERR_TRACE(BERR_UNKNOWN);
           }

           /* has the command failed? */
           if (((BSID_Rsp_Hdr *)pRsp)->errCode != BERR_SUCCESS)
           {
               BDBG_ERR(("BSID_P_SendCommand: cmd returned with errCode: 0x%x", ((BSID_Rsp_Hdr *)pRsp)->errCode));
               return BERR_TRACE(BERR_UNKNOWN);
           }
       }
   }

   BDBG_LEAVE(BSID_P_SendCommand);
   return BERR_TRACE(retCode);
}

void BSID_UpdateMemoryReport(BSID_ChannelHandle  hSidCh);

#define BSID_ISR_DBG
/******************************************************************************
* Function name: BSID_P_Dispatch_Irq
*
* Comments:
*
******************************************************************************/
void BSID_P_Dispatch_Irq(void *pv_Param1, int iParam2)
{
    BSID_Handle hSid = (BSID_Handle)pv_Param1;
    uint32_t    ui32_IrqSource = BREG_Read32_isr(hSid->hReg, BCHP_SID_ARC_CPU_INTGEN_CLR);
    uint32_t    ui32_ChannelNumber = 0;

    BSTD_UNUSED(iParam2);

#ifdef BSID_ISR_DBG
    BDBG_MSG(("BSID_P_Dispatch_Irq IrqSource 0x%x\n", ui32_IrqSource));
#endif

   /* get out when the irq all served */
    while (ui32_IrqSource & BSID_INTGEN_ENABLE_PINS)
    {
        /* test each channel for irq */
        if (ui32_IrqSource & (1<<ui32_ChannelNumber))
        {
            /*BERR_Code retCode     = BERR_SUCCESS;*/
            BSID_ChannelHandle      hSidCh = hSid->ahChannel[ui32_ChannelNumber];
#ifdef BSID_USE_CACHED_MEMORY
            BSID_CommandQueueHeader       *psQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sRelQueue.sBuf.pv_CachedAddr;
#else
            BSID_CommandQueueHeader       *psQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sRelQueue.sBuf.pui32_VirtAddr;
#endif
            BSID_RelQueueEntry     *psQueueEntry;
            BSID_CallbackFunc       pvCallbackFunc;
            uint32_t                ui32_CallbackEvents;
            void                   *pv_CallbackData;
            BSID_NotificationEvent  eEvent = 0x0;
            bool                    ChannelOperationsCompleted = false;

#ifdef BSID_USE_CACHED_MEMORY
            BMEM_FlushCache(hSid->hMem, (void *)psQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif

            while (psQueueHeader->ui32_ReadIndex != psQueueHeader->ui32_WriteIndex)
            {
                /* access an entry in the release queue */
                psQueueEntry = (BSID_RelQueueEntry *)((uint32_t)psQueueHeader + sizeof(BSID_CommandQueueHeader) + (psQueueHeader->ui32_ReadIndex * sizeof(BSID_RelQueueEntry)));
#ifdef BSID_USE_CACHED_MEMORY
                BMEM_FlushCache(hSid->hMem, (void *)psQueueEntry, sizeof(BSID_RelQueueEntry));
#endif
                pvCallbackFunc = hSidCh->sChSettings.u_ChannelSpecific.still.p_CallbackFunc_Isr;
                ui32_CallbackEvents = hSidCh->sChSettings.u_ChannelSpecific.still.ui32_CallbackEvents;
                pv_CallbackData = hSidCh->sChSettings.u_ChannelSpecific.still.pv_CallbackData;

#ifdef BSID_ISR_DBG
                BDBG_MSG(("BSID_Test_Isr: 0xheader %x channel %d decode/getinfo done rd %d wr %d or waitfordata %d\n",
                psQueueEntry->rspHeader.header,
                ui32_ChannelNumber, psQueueHeader->ui32_ReadIndex, psQueueHeader->ui32_WriteIndex,
                psQueueHeader->ui32_WaitForData));
#endif

                /* decode and decode segment events */
                if (psQueueEntry->rspHeader.header == BSID_MSG_HDR_DECODE || psQueueEntry->rspHeader.header == BSID_MSG_HDR_DECODESEGMENT)
                {
                    eEvent = (psQueueEntry->rspHeader.errCode == 0) ? BSID_NotificationEvent_eDecodeDone : BSID_NotificationEvent_eDecodeError;

#ifdef BSID_ISR_DBG
                    BDBG_MSG(("servicing decode events seq %d crc 0x%x status %s\n", psQueueEntry->rspHeader.sequence, psQueueEntry->checkSumValue,
                    (eEvent == BSID_NotificationEvent_eDecodeDone) ? "DecodeDone" : "DecodeError"));
#endif

                    ChannelOperationsCompleted = psQueueEntry->operationCompleted;
                }

                /* get info events */
                if (psQueueEntry->rspHeader.header == BSID_MSG_HDR_GETSTREAMINFO)
                {
                    /*BSID_ImageHeader *ps_SwapImageHeader = (BSID_ImageHeader *)hSid->sFwHwConfig.sDataMemory[ui32_ChannelNumber].pui32_VirtAddr;*/ /* address where the ARC has deposited the image header */
                    BSID_ImageHeader *ps_SwapImageHeader = (BSID_ImageHeader *)psQueueEntry->sidMemStreamInfoBufVirtAddr; /* address where the ARC has deposited the image header */
                    BSID_ImageHeader *pUserImageHeaderBuffer = (BSID_ImageHeader *)psQueueEntry->streamInfoBufVirtAddr;

#ifdef BSID_USE_CACHED_MEMORY
                    BMEM_FlushCache(hSid->hMem, (void *)ps_SwapImageHeader, sizeof(BSID_ImageHeader));
#endif
                    ps_SwapImageHeader->e_PixelFormat = ae_PxlFmtPi2Pxl[hSid->sFwHwConfig.b_EndianessSwap][ae_PxlFmtArc2Pi[ps_SwapImageHeader->e_PixelFormat][ps_SwapImageHeader->ui32_BitPerPixel]];
                    ps_SwapImageHeader->e_ClutPixelFormat = (ps_SwapImageHeader->e_ClutPixelFormat == (BPXL_Format)SID_OUT_RGBA) ? ae_ClutPxlFmtPi2Pxl[hSid->sFwHwConfig.b_EndianessSwap][0]: ae_ClutPxlFmtPi2Pxl[hSid->sFwHwConfig.b_EndianessSwap][1];
#ifdef BSID_USE_CACHED_MEMORY
                    BMEM_FlushCache(hSid->hMem, (void *)ps_SwapImageHeader, sizeof(BSID_ImageHeader));
#endif
                    BKNI_Memcpy((void *)pUserImageHeaderBuffer, (void *)ps_SwapImageHeader, sizeof(BSID_ImageHeader));

                    if (pUserImageHeaderBuffer->e_PixelFormat == BERR_UNKNOWN)
                    {
                        BDBG_MSG(("BSID_P_Dispatch_Irq: ERROR pixel format unknown"));
                    }

                    eEvent = (psQueueEntry->rspHeader.errCode == 0) ? BSID_NotificationEvent_eGetInfoDone : BSID_NotificationEvent_eGetInfoError;
#ifdef BSID_ISR_DBG
                    BDBG_MSG(("servicing getinfo events seq %d status %s\n", psQueueEntry->rspHeader.sequence,
                    (eEvent == BSID_NotificationEvent_eGetInfoDone) ? "GetInfoDone" : "GetInfoError"));
#endif

                    ChannelOperationsCompleted = psQueueEntry->operationCompleted;
                }

                if (psQueueEntry->rspHeader.header == BSID_MSG_HDR_SYNCCHANNEL)
                {
                    BKNI_SetEvent(hSidCh->hSyncEvent);
                }

                if (psQueueEntry->rspHeader.header == BSID_MSG_HDR_DECODE ||
                    psQueueEntry->rspHeader.header == BSID_MSG_HDR_DECODESEGMENT ||
                    psQueueEntry->rspHeader.header == BSID_MSG_HDR_GETSTREAMINFO)
                {
                    /* notify the caller via callback */
                    if ((pvCallbackFunc != NULL) && ((ui32_CallbackEvents & eEvent) == eEvent))
                    {
                        (pvCallbackFunc)(hSidCh, eEvent, pv_CallbackData);
                    }
                }

                BSID_INCR_INDEX(psQueueHeader->ui32_ReadIndex, hSidCh->ui32_QueueTrueDepth);

#ifdef BSID_USE_CACHED_MEMORY
                BMEM_FlushCache(hSid->hMem, (void *)psQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
            }

            if (psQueueHeader->ui32_WaitForData)
            {
                if (hSidCh->e_ChannelType == BSID_ChannelType_eStill)
                {
                    if (hSidCh->b_FlushPending == false)
                    {
                        pvCallbackFunc = hSidCh->sChSettings.u_ChannelSpecific.still.p_CallbackFunc_Isr;
                        ui32_CallbackEvents = hSidCh->sChSettings.u_ChannelSpecific.still.ui32_CallbackEvents;
                        pv_CallbackData = hSidCh->sChSettings.u_ChannelSpecific.still.pv_CallbackData;
                        if ((pvCallbackFunc != NULL) &&
                            ((ui32_CallbackEvents & BSID_NotificationEvent_eWaitingForMoreInputData) \
                              == BSID_NotificationEvent_eWaitingForMoreInputData))
                        {
#ifdef BSID_ISR_DBG
                            BDBG_MSG(("wait for data callback\n"));
#endif
                            (pvCallbackFunc)(
                                hSidCh,
                                BSID_NotificationEvent_eWaitingForMoreInputData,
                                pv_CallbackData
                             );
                        }
                    }
                    else
                    {
                       BSID_DmaChunkInfo dmaInfo = { 0xFFFFFFF, 0x0, true, true };
                       BSID_P_SetDmaChunkInfo_Isr( hSidCh, &dmaInfo );
                    }
                }
            }

            /* clear channel request */
            {
#ifdef BSID_USE_CACHED_MEMORY
                BSID_CommandQueueHeader *psQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pv_CachedAddr;
#else
                BSID_CommandQueueHeader *psQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pui32_VirtAddr;
#endif
#ifdef BSID_USE_CACHED_MEMORY
                BMEM_FlushCache(hSid->hMem, (void *)psQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif

                if (psQueueHeader->ui32_ReadIndex == psQueueHeader->ui32_WriteIndex)
                {
                    uint32_t ui32_hst2cpu = BREG_Read32_isr(hSid->hReg, BCHP_SID_ARC_HST2CPU_STAT);
                    ui32_hst2cpu &= ~(1 << ui32_ChannelNumber);
                    BREG_Write32_isr(hSid->hReg, BCHP_SID_ARC_HST2CPU_STAT, ui32_hst2cpu);
                }
            }

            /* clear current channel request */
            BREG_Write32_isr(hSid->hReg, BCHP_SID_ARC_CPU_INTGEN_CLR, (1 << ui32_ChannelNumber));

            /*
             * If the last operations serviced were all completed change channel state to ready
             * OperationCompleted status is set by the FW based. Typically GetInfo or DecodeImage
             * gets executed in one pass so the operation is completed always. DecodeSegment are
             * executed in more than one pass. While they are executed the completed status is
             * set to zero to indicate that they are not done. Only the very last operation will
             * report completed state set to true. NOTE that DecodeSegment belonging to the same
             * decode sections can't be preempt by GetInfo or DecodeImage operations.
             */
            if (ChannelOperationsCompleted == true)
            {
                hSidCh->e_ChannelState = BSID_ChannelState_eReady;
            }
        }

        /* get ready to test next channel */
        BSID_INCR_INDEX(ui32_ChannelNumber, hSid->ui32_TotalChannels);

        /* test mbx irq */
        if (ui32_IrqSource & BSID_SIGNAL_MBX_RESPONSE_READY)
        {
            BKNI_SetEvent(hSid->sMailbox.hMailboxEvent);
            BREG_Write32_isr(hSid->hReg, BCHP_SID_ARC_CPU_INTGEN_CLR, (ui32_IrqSource & BSID_SIGNAL_MBX_RESPONSE_READY));
            ui32_IrqSource &= ~BSID_SIGNAL_MBX_RESPONSE_READY;
        }


        /* check if we have received a new irq */
        ui32_IrqSource = BREG_Read32_isr(hSid->hReg, BCHP_SID_ARC_CPU_INTGEN_CLR);
    }
}

/*#define BSID_SCI_DBG*/
/******************************************************************************
* Function name: BSID_P_SendCmdInit
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_SendCmdInit(BSID_Handle hSid)
{
   BERR_Code      retCode     = BERR_SUCCESS;
#ifdef BSID_USE_CACHED_MEMORY
   BSID_Cmd_Init  *ps_InitCmd = (BSID_Cmd_Init *)hSid->sMailbox.pv_CmdQCachedAddr;
   BSID_Rsp_Init  *ps_InitRsp = (BSID_Rsp_Init *)hSid->sMailbox.pv_RspQCachedAddr;
#else
   BSID_Cmd_Init  *ps_InitCmd = (BSID_Cmd_Init *)hSid->sMailbox.pv_CmdQVirtAddr;
   BSID_Rsp_Init  *ps_InitRsp = (BSID_Rsp_Init *)hSid->sMailbox.pv_RspQVirtAddr;
#endif
   bool needAck = true;

   BDBG_ENTER(BSID_P_SendCmdInit);

   /* prepare cmd body */
   {
       ps_InitCmd->cmdHdr.header    = BSID_MSG_HDR_INIT;
       ps_InitCmd->cmdHdr.needAck   = needAck;

       ps_InitCmd->frequency        = ARC_UART_FREQUENCY;
       ps_InitCmd->baudRate         = ARC_UART_BAUDRATE;
       ps_InitCmd->mbxCmdBufAddr    = hSid->sMailbox.ui32_CmdQPhisAddr;
       ps_InitCmd->mbxRspBufAddr    = hSid->sMailbox.ui32_RspQPhisAddr;
       ps_InitCmd->mbxCmdRspBufSize = (hSid->sFwHwConfig.sMbxMemory.ui32_Size >> 1);
       ps_InitCmd->memMode          = hSid->sFwHwConfig.eMemoryMode;
       ps_InitCmd->memBase          = hSid->sFwHwConfig.sDataMemory[0].ui32_PhisAddr;
       ps_InitCmd->memSize          = hSid->sFwHwConfig.sDataMemory[0].ui32_Size;
       ps_InitCmd->msBufBase        = 0x00000000; /* provided decode time */
       ps_InitCmd->msBufSize        = 0x00000000; /* provided decode time */
       ps_InitCmd->dmaInfoBufAddr   = hSid->sFwHwConfig.sInpDmaMemory.ui32_PhisAddr;
       ps_InitCmd->dmaInfoBufSize   = hSid->sFwHwConfig.sInpDmaMemory.ui32_Size;
       ps_InitCmd->endianessSwap    = hSid->sFwHwConfig.b_EndianessSwap;
       ps_InitCmd->jpegFiltMode     = hSid->sFwHwConfig.ui16_JPEGHorizAndVerFilt;
       ps_InitCmd->alphaValue       = hSid->sFwHwConfig.ui8_AlphaValue;
       ps_InitCmd->dbgMsgOn         = BSID_ARC_INIT_MSG_ENABLE;
   }

#ifdef BSID_SCI_DBG
   BDBG_MSG(("cmd init sent"));
   BDBG_MSG(("cmd init payload"));
   BDBG_MSG(("frequency     = 0x%x", ps_InitCmd->frequency));
   BDBG_MSG(("baudRate      = 0x%x", ps_InitCmd->baudRate));
   BDBG_MSG(("cmd buf       = 0x%x", ps_InitCmd->mbxCmdBufAddr));
   BDBG_MSG(("rsp buf       = 0x%x", ps_InitCmd->mbxRspBufAddr));
   BDBG_MSG(("cmd rsp size  = 0x%x", ps_InitCmd->mbxCmdRspBufSize));
   BDBG_MSG(("memMode       = 0x%x", ps_InitCmd->memMode));
   BDBG_MSG(("memBase       = 0x%x", ps_InitCmd->memBase));
   BDBG_MSG(("memSize       = 0x%x", ps_InitCmd->memSize));
   BDBG_MSG(("endianessSwap = 0x%x", ps_InitCmd->endianessSwap));
   BDBG_MSG(("jpegFiltMode  = 0x%x", ps_InitCmd->jpegFiltMode));
   BDBG_MSG(("alphaValue    = 0x%x", ps_InitCmd->alphaValue));
#endif

   /* send cmd and wait for response. */
   retCode = BSID_P_SendCommand(hSid, (void *)ps_InitCmd, sizeof(BSID_Cmd_Init), (void *)ps_InitRsp, sizeof(BSID_Rsp_Init), needAck);
   if (retCode != BERR_SUCCESS)
   {
       return BERR_TRACE(retCode);
   }

#ifdef BSID_SCI_DBG
   BDBG_MSG(("cmd init done"));
#endif

   BDBG_LEAVE(BSID_P_SendCmdInit);
   return BERR_TRACE(retCode);
}

/*#define BSID_SCOC_DBG*/
/******************************************************************************
* Function name: BSID_P_SendCmdOpenChannel
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_SendCmdOpenChannel(BSID_ChannelHandle hSidCh)
{
   BERR_Code retCode = BERR_SUCCESS;
   BSID_Handle hSid = hSidCh->hSid;
#ifdef BSID_USE_CACHED_MEMORY
   BSID_Cmd_OpenChannel *ps_OpenChannelCmd = (BSID_Cmd_OpenChannel *)hSid->sMailbox.pv_CmdQCachedAddr;
   BSID_Rsp_OpenChannel *ps_OpenChannelRsp = (BSID_Rsp_OpenChannel *)hSid->sMailbox.pv_RspQCachedAddr;
#else
   BSID_Cmd_OpenChannel *ps_OpenChannelCmd = (BSID_Cmd_OpenChannel *)hSid->sMailbox.pv_CmdQVirtAddr;
   BSID_Rsp_OpenChannel *ps_OpenChannelRsp = (BSID_Rsp_OpenChannel *)hSid->sMailbox.pv_RspQVirtAddr;
#endif
   bool needAck = true;

   BDBG_ENTER(BSID_P_SendCmdOpenChannel);

   /* prepare cmd body */
   {
       ps_OpenChannelCmd->cmdHdr.header   = BSID_MSG_HDR_OPENCHANNEL;
       ps_OpenChannelCmd->cmdHdr.needAck  = needAck;

       ps_OpenChannelCmd->channelID       = hSidCh->ui32_ChannelID;
       ps_OpenChannelCmd->channelNumber   = hSidCh->ui32_ChannelNum;
       ps_OpenChannelCmd->priority        = hSidCh->sChSettings.ui32_ChannelPriority;
       ps_OpenChannelCmd->reqQueueAddr    = hSidCh->sReqQueue.sBuf.ui32_PhisAddr;
       ps_OpenChannelCmd->relQueueAddr    = hSidCh->sRelQueue.sBuf.ui32_PhisAddr;
       ps_OpenChannelCmd->queueDepth      = hSidCh->ui32_QueueTrueDepth;
       ps_OpenChannelCmd->dbgMsgOn        = BSID_ARC_OPENCHANNEL_MSG_ENABLE;
   }

#ifdef BSID_SCOC_DBG
   BDBG_MSG(("cmd open channel sent\n"));
   BDBG_MSG(("channelID    = 0x%x\n", ps_OpenChannelCmd->channelID));
   BDBG_MSG(("priority     = 0x%x\n", ps_OpenChannelCmd->priority));
   BDBG_MSG(("reqQueueAddr = 0x%x\n", ps_OpenChannelCmd->reqQueueAddr));
   BDBG_MSG(("relQueueAddr = 0x%x\n", ps_OpenChannelCmd->relQueueAddr));
   BDBG_MSG(("queueDepth   = 0x%x\n", ps_OpenChannelCmd->queueDepth));
#endif

   /* send command and wait for response. */
   retCode = BSID_P_SendCommand(hSid, (void *)ps_OpenChannelCmd, sizeof(BSID_Cmd_OpenChannel), (void *)ps_OpenChannelRsp, sizeof(BSID_Rsp_OpenChannel), needAck);
   if (retCode != BERR_SUCCESS)
   {
      BDBG_ERR(("BSID_P_SendCmdOpenChannel: BSID_P_SendCommand failed\n"));
      return BERR_TRACE(retCode);
   }

#ifdef BSID_SCOC_DBG
   BDBG_MSG(("rsp open channel done\n"));
#endif

   BDBG_LEAVE(BSID_P_SendCmdOpenChannel);
   return BERR_TRACE(retCode);
}

/*#define BSID_SCCC_DBG*/
/******************************************************************************
* Function name: BSID_P_SendCmdCloseChannel
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_SendCmdCloseChannel(BSID_ChannelHandle hSidCh)
{
   BERR_Code retCode = BERR_SUCCESS;
   BSID_Handle hSid = hSidCh->hSid;
#ifdef BSID_USE_CACHED_MEMORY
   BSID_Cmd_CloseChannel *ps_CloseChannelCmd = (BSID_Cmd_CloseChannel *)hSid->sMailbox.pv_CmdQCachedAddr;
   BSID_Rsp_CloseChannel *ps_CloseChannelRsp = (BSID_Rsp_CloseChannel *)hSid->sMailbox.pv_RspQCachedAddr;
#else
   BSID_Cmd_CloseChannel *ps_CloseChannelCmd = (BSID_Cmd_CloseChannel *)hSid->sMailbox.pv_CmdQVirtAddr;
   BSID_Rsp_CloseChannel *ps_CloseChannelRsp = (BSID_Rsp_CloseChannel *)hSid->sMailbox.pv_RspQVirtAddr;
#endif
   bool needAck = true;

   BDBG_ENTER(BSID_P_SendCmdCloseChannel);

   /* prepare cmd body */
   {
       ps_CloseChannelCmd->cmdHdr.header    = BSID_MSG_HDR_CLOSECHANNEL;
       ps_CloseChannelCmd->cmdHdr.needAck   = needAck;

       ps_CloseChannelCmd->channelID        = hSidCh->ui32_ChannelID;
       ps_CloseChannelCmd->channelNumber    = hSidCh->ui32_ChannelNum;
       ps_CloseChannelCmd->dbgMsgOn         = BSID_ARC_CLOSECHANNEL_MSG_ENABLE;
   }

#ifdef BSID_SCCC_DBG
   BDBG_MSG(("cmd close channel sent\n"));
   BDBG_MSG(("channelID         = 0x%x\n", ps_CloseChannelCmd->channelID));
#endif

   /* send command and wait for response. */
   retCode = BSID_P_SendCommand(hSid, (void *)ps_CloseChannelCmd, sizeof(BSID_Cmd_CloseChannel), (void *)ps_CloseChannelRsp, sizeof(BSID_Rsp_CloseChannel), needAck);
   if (retCode != BERR_SUCCESS)
   {
      BDBG_ERR(("BSID_P_SendCmdCloseChannel: BSID_P_SendCommand failed\n"));
      return BERR_TRACE(retCode);
   }

#ifdef BSID_SCCC_DBG
   BDBG_MSG(("rsp close channel done\n"));
#endif

   BDBG_LEAVE(BSID_P_SendCmdCloseChannel);
   return BERR_TRACE(retCode);
}

/* #define BSID_FC_DBG */
/******************************************************************************
* Function name: BSID_P_FlushChannel
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_FlushChannel(BSID_ChannelHandle  hSidCh,
                            const BSID_FlushSettings *ps_FlushSettings)
{
   BERR_Code retCode = BERR_SUCCESS;
   BSID_Handle hSid = hSidCh->hSid;
#ifdef BSID_USE_CACHED_MEMORY
   BSID_Cmd_FlushChannel *ps_FlushChannelCmd = (BSID_Cmd_FlushChannel *)hSid->sMailbox.pv_CmdQCachedAddr;
   BSID_Rsp_FlushChannel *ps_FlushChannelRsp = (BSID_Rsp_FlushChannel *)hSid->sMailbox.pv_RspQCachedAddr;
   BSID_CommandQueueHeader *psReqQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pv_CachedAddr;
   BSID_CommandQueueHeader *psRelQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sRelQueue.sBuf.pv_CachedAddr;
   BSID_CommandQueueHeader *psDataQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sDataQueue.sBuf.pv_CachedAddr;
#else
   BSID_Cmd_FlushChannel *ps_FlushChannelCmd = (BSID_Cmd_FlushChannel *)hSid->sMailbox.pv_CmdQVirtAddr;
   BSID_Rsp_FlushChannel *ps_FlushChannelRsp = (BSID_Rsp_FlushChannel *)hSid->sMailbox.pv_RspQVirtAddr;
   BSID_CommandQueueHeader *psReqQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pui32_VirtAddr;
   BSID_CommandQueueHeader *psRelQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sRelQueue.sBuf.pui32_VirtAddr;
   BSID_CommandQueueHeader *psDataQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sDataQueue.sBuf.pui32_VirtAddr;
#endif

   bool needAck = true;

   BSTD_UNUSED(ps_FlushSettings);

   BDBG_ENTER(BSID_P_FlushChannel);

   hSidCh->b_FlushPending = true;

   /* prepare cmd body */
   {
       ps_FlushChannelCmd->cmdHdr.header    = BSID_MSG_HDR_FLUSHCHANNEL;
       ps_FlushChannelCmd->cmdHdr.needAck   = needAck;

       ps_FlushChannelCmd->channelID        = hSidCh->ui32_ChannelID;
       ps_FlushChannelCmd->channelNumber    = hSidCh->ui32_ChannelNum;
       ps_FlushChannelCmd->dbgMsgOn         = BSID_ARC_FLUSHCHANNEL_MSG_ENABLE;
   }

#ifdef BSID_FC_DBG
   BDBG_MSG(("cmd close channel sent\n"));
   BDBG_MSG(("channelID         = 0x%x\n", ps_FlushChannelCmd->channelID));
#endif

   /* send command and wait for response. */
   retCode = BSID_P_SendCommand(hSid, (void *)ps_FlushChannelCmd, sizeof(BSID_Cmd_FlushChannel), (void *)ps_FlushChannelRsp, sizeof(BSID_Rsp_FlushChannel), needAck);
   if (retCode != BERR_SUCCESS)
   {
      hSidCh->b_FlushPending = false;
      BDBG_ERR(("BSID_P_FlushChannel: BSID_P_SendCommand failed\n"));
      return BERR_TRACE(retCode);
   }

#ifdef BSID_USE_CACHED_MEMORY
   BMEM_FlushCache(hSid->hMem, (void *)psReqQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
   /* reset queue rd/wr index */
   {
       /* sanity check, should always not be verified! */
       if (psReqQueueHeader->ui32_ReadIndex != psReqQueueHeader->ui32_WriteIndex)
       {
           BDBG_MSG(("BSID_P_FlushChannel: reqQ rd and wr index are not the same\n"));
       }

       /* reset to beginning of the request queue */
       psReqQueueHeader->ui32_ReadIndex = psReqQueueHeader->ui32_WriteIndex = 0;

       /* reset to beginning of the release queue */
       psRelQueueHeader->ui32_ReadIndex = psRelQueueHeader->ui32_WriteIndex = 0;

       /* reset to beginning of the data queue */
       psDataQueueHeader->ui32_ReadIndex = psDataQueueHeader->ui32_WriteIndex = 0;
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psReqQueueHeader, sizeof(BSID_CommandQueueHeader));
       BMEM_FlushCache(hSid->hMem, (void *)psRelQueueHeader, sizeof(BSID_CommandQueueHeader));
       BMEM_FlushCache(hSid->hMem, (void *)psDataQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
   }

   hSidCh->b_FlushPending = false;

#ifdef BSID_FC_DBG
   BDBG_MSG(("rsp close channel done\n"));
#endif

   BDBG_LEAVE(BSID_P_FlushChannel);
   return BERR_TRACE(retCode);
}

/*#define BSID_SCD_DBG*/
/******************************************************************************
* Function name: BSID_P_SendCmdDecodeStillImage
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_SendCmdDecodeStillImage(
    BSID_ChannelHandle hSidCh,
    const BSID_DecodeImage *ps_DecodeImage)
{
   BERR_Code        retCode       = BERR_SUCCESS;
   BSID_Handle      hSid          = hSidCh->hSid;
#ifdef BSID_USE_CACHED_MEMORY
   BSID_CommandQueueHeader *psQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pv_CachedAddr;
#else
   BSID_CommandQueueHeader *psQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pui32_VirtAddr;
#endif
   BSID_ReqQueueEntry *psQueueEntry = NULL;
   BSID_Cmd_Decode *ps_DecodeCmd = NULL;
   const BSID_ImageBuffer *ps_ImageBuffer  = &ps_DecodeImage->s_ImageBuffer;
   bool             needAck = false;
   unsigned char i;

   BDBG_ENTER(BSID_P_SendCmdDecodeStillImage);

   /* prepare request queue elem */
   {
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
       psQueueEntry = (BSID_ReqQueueEntry *)((uint32_t)psQueueHeader + sizeof(BSID_CommandQueueHeader) + \
                      (psQueueHeader->ui32_WriteIndex * sizeof(BSID_ReqQueueEntry)));
       BKNI_Memset((void *)psQueueEntry, 0x00, sizeof(BSID_ReqQueueEntry));
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psQueueEntry, sizeof(BSID_ReqQueueEntry));
#endif
       ps_DecodeCmd   = (BSID_Cmd_Decode *)psQueueEntry;
   }

   /* prepare cmd body */
   {
       ps_DecodeCmd->cmdHdr.header   = BSID_MSG_HDR_DECODE;
       ps_DecodeCmd->cmdHdr.needAck  = needAck;
       ps_DecodeCmd->cmdHdr.sequence = hSidCh->ui32_SequenceNumber++;

       ps_DecodeCmd->imgFormat         = hSid->sFwHwConfig.ae_ImgFmtPi2Arc[ps_DecodeImage->e_ImageFormat];
       ps_DecodeCmd->imgWidth          = ps_ImageBuffer->ui32_Width;
       ps_DecodeCmd->imgHeight         = ps_ImageBuffer->ui32_Height;
       ps_DecodeCmd->imgPitch          = ps_ImageBuffer->ui32_Pitch;
       ps_DecodeCmd->imgBpp            = BPXL_BITS_PER_PIXEL(ps_ImageBuffer->e_Format);
       ps_DecodeCmd->sidMemImgBufAddr  = ps_ImageBuffer->ui32_Offset;
       ps_DecodeCmd->sidMemImgBufSize  = ps_ImageBuffer->ui32_Pitch * ps_ImageBuffer->ui32_Height;
       ps_DecodeCmd->sidMemInpBufAddr0 = ((BSID_LinDataMap *)ps_DecodeImage->pv_InputDataMap)->ui32_Offset;
       ps_DecodeCmd->sidMemInpBufSize0 = ((BSID_LinDataMap *)ps_DecodeImage->pv_InputDataMap)->ui32_Size;
       ps_DecodeCmd->sidMemInpBufAddr1 = ps_DecodeCmd->sidMemInpBufAddr0 + ps_DecodeCmd->sidMemInpBufSize0;
       ps_DecodeCmd->sidMemInpBufSize1 = 0;
       ps_DecodeCmd->segMaxHeight      = 0;
       ps_DecodeCmd->dbgRleParseHeader = false;
       ps_DecodeCmd->dbgMsgOn          = BSID_ARC_DECODE_MSG_ENABLE;
       ps_DecodeCmd->lastDMAchunk      = ps_DecodeImage->b_LastDmaChunk;
       ps_DecodeCmd->msBufBase         = ps_DecodeImage->ui32_MultiScanBufAddr;
       ps_DecodeCmd->msBufSize         = ps_DecodeImage->ui32_MultiScanBufSize;
       ps_DecodeCmd->bypass            = false;
       ps_DecodeCmd->doCheckSum        = false;

	   if (ps_DecodeImage->pui8_IntraQuantTable)
	   {
	   		ps_DecodeCmd->isIntraQuantTablePresent = 1;
	   		for (i = 0; i < BSID_MPEG2IFRAME_INTRA_QUANT_ENRIES; i++)
	   		{
	   			ps_DecodeCmd->intra_quant_table[i] = ps_DecodeImage->pui8_IntraQuantTable[i];

	   		}
	   }
	   else
	   {
			ps_DecodeCmd->isIntraQuantTablePresent = 0;
	   }


       BSID_P_BuildGammaCorrectionTable(hSid, ps_DecodeCmd->gamma_correction_table);
   }

#ifdef BSID_SCD_DBG
   BDBG_MSG(("cmd dec sent\n"));
   BDBG_MSG(("imgFormat         = 0x%x", ps_DecodeCmd->imgFormat));
   BDBG_MSG(("imgWidth          = 0x%x", ps_DecodeCmd->imgWidth));
   BDBG_MSG(("imgHeight         = 0x%x", ps_DecodeCmd->imgHeight));
   BDBG_MSG(("imgPitch          = 0x%x", ps_DecodeCmd->imgPitch));
   BDBG_MSG(("sidMemImgBufAddr  = 0x%x", ps_DecodeCmd->sidMemImgBufAddr));
   BDBG_MSG(("sidMemImgBufSize  = 0x%x", ps_DecodeCmd->sidMemImgBufSize));
   BDBG_MSG(("sidMemInpBufAddr0 = 0x%x", ps_DecodeCmd->sidMemInpBufAddr0));
   BDBG_MSG(("sidMemInpBufSize0 = 0x%x", ps_DecodeCmd->sidMemInpBufSize0));
   BDBG_MSG(("sidMemInpBufAddr1 = 0x%x", ps_DecodeCmd->sidMemInpBufAddr1));
   BDBG_MSG(("sidMemInpBufSize1 = 0x%x", ps_DecodeCmd->sidMemInpBufSize1));
#endif

#ifdef BSID_USE_CACHED_MEMORY
   BMEM_FlushCache(hSid->hMem, (void *)ps_DecodeCmd, sizeof(BSID_Cmd_Decode));
#endif

   /* signal new queue enty to the ARC */
   {
       uint32_t ui32_hst2cpu;

       BKNI_EnterCriticalSection();
       ui32_hst2cpu = BREG_Read32(hSid->hReg, BCHP_SID_ARC_HST2CPU_STAT);
       BSID_INCR_INDEX(psQueueHeader->ui32_WriteIndex, hSidCh->ui32_QueueTrueDepth);
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
       BREG_Write32(hSid->hReg, BCHP_SID_ARC_HST2CPU_STAT, (ui32_hst2cpu | (1 << hSidCh->ui32_ChannelNum)));
       BKNI_LeaveCriticalSection();
   }

#ifdef BSID_SCD_DBG
   BDBG_MSG(("rsp dec done"));
#endif

   BDBG_LEAVE(BSID_P_SendCmdDecodeStillImage);
   return BERR_TRACE(retCode);
}

/*#define BSID_SGS_DBG*/
/******************************************************************************
* Function name: BSID_P_SendCmdGetStreamInfo
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_SendCmdGetStreamInfo(
    BSID_ChannelHandle hSidCh,
    BSID_StreamInfo *ps_StreamInfo)

{
   BERR_Code               retCode       = BERR_SUCCESS;
   BSID_Handle             hSid          = hSidCh->hSid;
#ifdef BSID_USE_CACHED_MEMORY
   BSID_CommandQueueHeader *psReqQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pv_CachedAddr;
   BSID_CommandQueueHeader *psDataQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sDataQueue.sBuf.pv_CachedAddr;
#else
   BSID_CommandQueueHeader *psReqQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pui32_VirtAddr;
   BSID_CommandQueueHeader *psDataQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sDataQueue.sBuf.pui32_VirtAddr;
#endif
   BSID_Cmd_GetStreamInfo *ps_GetInfoCmd = NULL;
   BSID_ReqQueueEntry *psReqQueueEntry = NULL;
   BSID_DataQueueEntry *psDataQueueEntry = NULL;
   uint32_t            dataQueueEntryPhisAddr = 0x0;
   bool                    needAck = false;

   BDBG_ENTER(BSID_P_SendCmdGetStreamInfo);

   /* prepare request queue elem */
   {
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psReqQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
       psReqQueueEntry = (BSID_ReqQueueEntry *)((uint32_t)psReqQueueHeader + sizeof(BSID_CommandQueueHeader) + \
                      (psReqQueueHeader->ui32_WriteIndex * sizeof(BSID_ReqQueueEntry)));
       BKNI_Memset((void *)psReqQueueEntry, 0x00, sizeof(BSID_ReqQueueEntry));
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psReqQueueEntry, sizeof(BSID_ReqQueueEntry));
#endif
       ps_GetInfoCmd   = (BSID_Cmd_GetStreamInfo *)psReqQueueEntry;
   }

   /* prepare data queue elem */
   {
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psDataQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
       psDataQueueEntry = (BSID_DataQueueEntry *)((uint32_t)psDataQueueHeader + sizeof(BSID_CommandQueueHeader) + \
                      (psDataQueueHeader->ui32_WriteIndex * sizeof(BSID_DataQueueEntry)));

       retCode = BMEM_Heap_ConvertAddressToOffset(hSid->hMem, (void *)psDataQueueEntry, &dataQueueEntryPhisAddr);
       if (retCode != BERR_SUCCESS)
       {
           BDBG_ERR(("BSID_P_SendCmdGetStreamInfo: BMEM_Heap_ConvertAddressToOffset failed rc = 0x%x\n", retCode));
           return BERR_TRACE(retCode);
       }

       BKNI_Memset((void *)psDataQueueEntry, 0x00, sizeof(BSID_DataQueueEntry));
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psDataQueueEntry, sizeof(BSID_DataQueueEntry));
#endif
   }

   /* prepare cmd body */
   {
       ps_GetInfoCmd->cmdHdr.header   = BSID_MSG_HDR_GETSTREAMINFO;
       ps_GetInfoCmd->cmdHdr.needAck  = needAck;
       ps_GetInfoCmd->cmdHdr.sequence = hSidCh->ui32_SequenceNumber++;

       ps_GetInfoCmd->imgFormat                   = hSid->sFwHwConfig.ae_ImgFmtPi2Arc[ps_StreamInfo->e_ImageFormat];
       ps_GetInfoCmd->sidMemInpBufAddr0           = ((BSID_LinDataMap *)ps_StreamInfo->pv_InputDataMap)->ui32_Offset;
       ps_GetInfoCmd->sidMemInpBufSize0           = ((BSID_LinDataMap *)ps_StreamInfo->pv_InputDataMap)->ui32_Size;
       ps_GetInfoCmd->sidMemInpBufAddr1           = ps_GetInfoCmd->sidMemInpBufAddr0 + ps_GetInfoCmd->sidMemInpBufSize0;
       ps_GetInfoCmd->sidMemInpBufSize1           = 0;
       ps_GetInfoCmd->sidMemStreamInfoBufSize     = sizeof(BSID_ImageHeader);
       ps_GetInfoCmd->dbgMsgOn                    = BSID_ARC_GET_INFO_MSG_ENABLE;
       ps_GetInfoCmd->sidMemStreamInfoBufPhisAddr = dataQueueEntryPhisAddr/*hSid->sFwHwConfig.sDataMemory[hSidCh->ui32_ChannelNum].ui32_PhisAddr*/;
       ps_GetInfoCmd->sidMemStreamInfoBufVirtAddr = (uint32_t)psDataQueueEntry;
       ps_GetInfoCmd->streamInfoBufVirtAddr       = (uint32_t)ps_StreamInfo->ps_OutImageInfo;
       ps_GetInfoCmd->lastDMAchunk                = ps_StreamInfo->b_LastDmaChunk;
   }

#ifdef BSID_SGS_DBG
   BDBG_MSG(("cmd getinfo sent\n"));
   BDBG_MSG(("imgFormat               = 0x%x", ps_GetInfoCmd->imgFormat));
   BDBG_MSG(("sidMemInpBufAddr0       = 0x%x", ps_GetInfoCmd->sidMemInpBufAddr0));
   BDBG_MSG(("sidMemInpBufSize0       = 0x%x", ps_GetInfoCmd->sidMemInpBufSize0));
   BDBG_MSG(("sidMemInpBufAddr1       = 0x%x", ps_GetInfoCmd->sidMemInpBufAddr1));
   BDBG_MSG(("sidMemInpBufSize1       = 0x%x", ps_GetInfoCmd->sidMemInpBufSize1));
   BDBG_MSG(("sidMemStreamInfoBufAddr = 0x%x", ps_GetInfoCmd->sidMemStreamInfoBufPhisAddr));
   BDBG_MSG(("sidMemStreamInfoBufSize = 0x%x", ps_GetInfoCmd->sidMemStreamInfoBufSize));
#endif

#ifdef BSID_USE_CACHED_MEMORY
   BMEM_FlushCache(hSid->hMem, (void *)ps_GetInfoCmd, sizeof(BSID_Cmd_GetStreamInfo));
#endif

   /* signal new queue enty to the ARC */
   {
       uint32_t ui32_hst2cpu;

       BKNI_EnterCriticalSection();
       ui32_hst2cpu = BREG_Read32(hSid->hReg, BCHP_SID_ARC_HST2CPU_STAT);
       BSID_INCR_INDEX(psReqQueueHeader->ui32_WriteIndex, hSidCh->ui32_QueueTrueDepth);
       BSID_INCR_INDEX(psDataQueueHeader->ui32_WriteIndex, hSidCh->ui32_QueueTrueDepth);
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psReqQueueHeader, sizeof(BSID_CommandQueueHeader));
       BMEM_FlushCache(hSid->hMem, (void *)psDataQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
       BREG_Write32(hSid->hReg, BCHP_SID_ARC_HST2CPU_STAT, (ui32_hst2cpu | (1 << hSidCh->ui32_ChannelNum)));
       BKNI_LeaveCriticalSection();
   }

#ifdef BSID_SGS_DBG
   BDBG_MSG(("rsp getinfo done"));
#endif

   BDBG_LEAVE(BSID_P_SendCmdGetStreamInfo);
   return BERR_TRACE(retCode);
}

/*#define BSID_SCDS_DBG*/
/******************************************************************************
* Function name: BSID_P_SendCmdDecodeSegment
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_SendCmdDecodeSegment(
    BSID_ChannelHandle hSidCh,
    const BSID_DecodeSegment *ps_DecodeSegment)
{
   BERR_Code                retCode       = BERR_SUCCESS;
   BSID_Handle              hSid          = hSidCh->hSid;
#ifdef BSID_USE_CACHED_MEMORY
   BSID_CommandQueueHeader *psQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pv_CachedAddr;
#else
   BSID_CommandQueueHeader *psQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pui32_VirtAddr;
#endif
   BSID_ReqQueueEntry *psQueueEntry = NULL;
   BSID_Cmd_DecodeSegment   *ps_DecodeCmd = NULL;
   const BSID_ImageBuffer   *ps_ImageBuffer  = &ps_DecodeSegment->s_ImageBuffer;
   bool                     needAck = false;

   BDBG_ENTER(BSID_P_SendCmdDecodeStillImage);

   /* prepare request queue elem */
   {
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
       psQueueEntry = (BSID_ReqQueueEntry *)((uint32_t)psQueueHeader + sizeof(BSID_CommandQueueHeader) + \
                      (psQueueHeader->ui32_WriteIndex * sizeof(BSID_ReqQueueEntry)));
       BKNI_Memset((void *)psQueueEntry, 0x00, sizeof(BSID_ReqQueueEntry));
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psQueueEntry, sizeof(BSID_ReqQueueEntry));
#endif
       ps_DecodeCmd   = (BSID_Cmd_Decode *)psQueueEntry;
   }

   /* prepare cmd body */
   {
       ps_DecodeCmd->cmdHdr.header     = BSID_MSG_HDR_DECODESEGMENT;
       ps_DecodeCmd->cmdHdr.needAck    = needAck;
       ps_DecodeCmd->cmdHdr.sequence   = hSidCh->ui32_SequenceNumber++;

       ps_DecodeCmd->imgFormat         = hSid->sFwHwConfig.ae_ImgFmtPi2Arc[ps_DecodeSegment->e_ImageFormat];
       ps_DecodeCmd->imgWidth          = ps_ImageBuffer->ui32_Width;
       ps_DecodeCmd->imgHeight         = ps_ImageBuffer->ui32_Height;
       ps_DecodeCmd->imgPitch          = ps_ImageBuffer->ui32_Pitch;
       ps_DecodeCmd->imgBpp            = BPXL_BITS_PER_PIXEL(ps_ImageBuffer->e_Format);
       ps_DecodeCmd->sidMemImgBufAddr  = ps_ImageBuffer->ui32_Offset;
       ps_DecodeCmd->sidMemImgBufSize  = ps_ImageBuffer->ui32_Pitch * ps_ImageBuffer->ui32_Height;
       ps_DecodeCmd->sidMemInpBufAddr0 = ((BSID_LinDataMap *)ps_DecodeSegment->pv_InputDataMap)->ui32_Offset;
       ps_DecodeCmd->sidMemInpBufSize0 = ((BSID_LinDataMap *)ps_DecodeSegment->pv_InputDataMap)->ui32_Size;
       ps_DecodeCmd->sidMemInpBufAddr1 = ps_DecodeCmd->sidMemInpBufAddr0 + ps_DecodeCmd->sidMemInpBufSize0;
       ps_DecodeCmd->sidMemInpBufSize1 = 0;
       ps_DecodeCmd->segMaxHeight      = ps_DecodeSegment->ui32_SegmentHeight;
       ps_DecodeCmd->firstSegment      = ps_DecodeSegment->b_FirstSegment;
       ps_DecodeCmd->dbgRleParseHeader = false;
       ps_DecodeCmd->dbgMsgOn          = BSID_ARC_DECODE_SEGMENT_MSG_ENABLE;
       ps_DecodeCmd->lastDMAchunk      = ps_DecodeSegment->b_LastDmaChunk;
       ps_DecodeCmd->msBufBase         = ps_DecodeSegment->ui32_MultiScanBufAddr;
       ps_DecodeCmd->msBufSize         = ps_DecodeSegment->ui32_MultiScanBufSize;
       ps_DecodeCmd->bypass            = false;
       ps_DecodeCmd->doCheckSum        = false;
   }

#ifdef BSID_SCDS_DBG
   BDBG_MSG(("cmd seg dec sent\n"));
   BDBG_MSG(("imgFormat         = 0x%x\n", ps_DecodeCmd->imgFormat));
   BDBG_MSG(("imgWidth          = 0x%x\n", ps_DecodeCmd->imgWidth));
   BDBG_MSG(("imgHeight         = 0x%x\n", ps_DecodeCmd->imgHeight));
   BDBG_MSG(("imgPitch          = 0x%x\n", ps_DecodeCmd->imgPitch));
   BDBG_MSG(("sidMemImgBufAddr  = 0x%x\n", ps_DecodeCmd->sidMemImgBufAddr));
   BDBG_MSG(("sidMemImgBufSize  = 0x%x\n", ps_DecodeCmd->sidMemImgBufSize));
   BDBG_MSG(("sidMemInpBufAddr0 = 0x%x\n", ps_DecodeCmd->sidMemInpBufAddr0));
   BDBG_MSG(("sidMemInpBufSize0 = 0x%x\n", ps_DecodeCmd->sidMemInpBufSize0));
   BDBG_MSG(("sidMemInpBufAddr1 = 0x%x\n", ps_DecodeCmd->sidMemInpBufAddr1));
   BDBG_MSG(("sidMemInpBufSize1 = 0x%x\n", ps_DecodeCmd->sidMemInpBufSize1));
   BDBG_MSG(("segMaxHeight      = 0x%x (ns %d)\n", ps_DecodeCmd->segMaxHeight, ps_DecodeCmd->segMaxHeight/16));
#endif

#ifdef BSID_USE_CACHED_MEMORY
   BMEM_FlushCache(hSid->hMem, (void *)ps_DecodeCmd, sizeof(BSID_Cmd_DecodeSegment));
#endif

   /* signal new queue enty to the ARC */
   {
       uint32_t ui32_hst2cpu;

       BKNI_EnterCriticalSection();
       ui32_hst2cpu = BREG_Read32(hSid->hReg, BCHP_SID_ARC_HST2CPU_STAT);
       BSID_INCR_INDEX(psQueueHeader->ui32_WriteIndex, hSidCh->ui32_QueueTrueDepth);
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
       BREG_Write32(hSid->hReg, BCHP_SID_ARC_HST2CPU_STAT, (ui32_hst2cpu | (1 << hSidCh->ui32_ChannelNum)));
       BKNI_LeaveCriticalSection();
   }

#ifdef BSID_SCDS_DBG
   BDBG_MSG(("rsp dec done"));
#endif

   BDBG_LEAVE(BSID_P_SendCmdDecodeStillImage);
   return BERR_TRACE(retCode);
}

/*#define BSID_SCH_DBG*/
/******************************************************************************
* Function name: BSID_P_SendSyncChannel
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_SendSyncChannel(
    BSID_ChannelHandle hSidCh)
{
   BERR_Code                retCode       = BERR_SUCCESS;
   BSID_Handle              hSid          = hSidCh->hSid;
#ifdef BSID_USE_CACHED_MEMORY
   BSID_CommandQueueHeader *psQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pv_CachedAddr;
#else
   BSID_CommandQueueHeader *psQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pui32_VirtAddr;
#endif
   BSID_ReqQueueEntry *psQueueEntry = NULL;
   BSID_Cmd_SyncChannel   *ps_SyncChannelCmd = NULL;
   bool                     needAck = false;

   BDBG_ENTER(BSID_P_SendSyncChannel);

   /* prepare request queue elem */
   {
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
       psQueueEntry = (BSID_ReqQueueEntry *)((uint32_t)psQueueHeader + sizeof(BSID_CommandQueueHeader) + \
                      (psQueueHeader->ui32_WriteIndex * sizeof(BSID_ReqQueueEntry)));
       BKNI_Memset((void *)psQueueEntry, 0x00, sizeof(BSID_ReqQueueEntry));
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psQueueEntry, sizeof(BSID_ReqQueueEntry));
#endif
       ps_SyncChannelCmd   = (BSID_Cmd_SyncChannel *)psQueueEntry;
   }

   /* prepare cmd body */
   {
       ps_SyncChannelCmd->cmdHdr.header     = BSID_MSG_HDR_SYNCCHANNEL;
       ps_SyncChannelCmd->cmdHdr.needAck    = needAck;
       ps_SyncChannelCmd->cmdHdr.sequence   = hSidCh->ui32_SequenceNumber++;
       ps_SyncChannelCmd->dbgMsgOn          = BSID_ARC_SYNCCHANNEL_MSG_ENABLE;
       ps_SyncChannelCmd->bypass            = true;
   }

#ifdef BSID_SCH_DBG
   BDBG_MSG(("cmd sync ch sent\n"));
#endif

#ifdef BSID_USE_CACHED_MEMORY
   BMEM_FlushCache(hSid->hMem, (void *)ps_SyncChannelCmd, sizeof(BSID_Cmd_SyncChannel));
#endif

   /* signal new queue enty to the ARC */
   {
       uint32_t ui32_hst2cpu;

       BKNI_EnterCriticalSection();
       ui32_hst2cpu = BREG_Read32(hSid->hReg, BCHP_SID_ARC_HST2CPU_STAT);
       BSID_INCR_INDEX(psQueueHeader->ui32_WriteIndex, hSidCh->ui32_QueueTrueDepth);
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
       BREG_Write32(hSid->hReg, BCHP_SID_ARC_HST2CPU_STAT, (ui32_hst2cpu | (1 << hSidCh->ui32_ChannelNum)));
       BKNI_LeaveCriticalSection();
   }

#ifdef BSID_SCDS_DBG
   BDBG_MSG(("rsp dec done"));
#endif

   BDBG_LEAVE(BSID_P_SendCmdSyncChannel);
   return BERR_TRACE(retCode);
}


/*#define BSID_SDCI_DBG*/
/******************************************************************************
* Function name: BSID_P_SetDmaChunkInfo
*   -
* Comments:
*   -
******************************************************************************/
void BSID_P_SetDmaChunkInfo_Isr(
    BSID_ChannelHandle  hSidCh,
    const BSID_DmaChunkInfo *ps_DmaChunkInfo)
{
    BSID_Handle hSid = hSidCh->hSid;
    BSID_LinearBuffer *sDataMemory = &hSid->sFwHwConfig.sInpDmaMemory;
#ifdef BSID_USE_CACHED_MEMORY
    BSID_DmaChunkInfo *pDmaInfo = (BSID_DmaChunkInfo *)sDataMemory->pv_CachedAddr;
    BSID_CommandQueueHeader  *psQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sRelQueue.sBuf.pv_CachedAddr;
#else
    BSID_DmaChunkInfo *pDmaInfo = (BSID_DmaChunkInfo *)sDataMemory->pui32_VirtAddr;
    BSID_CommandQueueHeader  *psQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sRelQueue.sBuf.pui32_VirtAddr;
#endif

    if (ps_DmaChunkInfo->ui32_AbortDma == 0)
    {
        pDmaInfo->ui32_Offset      = ps_DmaChunkInfo->ui32_Offset;
        pDmaInfo->ui32_Size         = ps_DmaChunkInfo->ui32_Size;
        pDmaInfo->ui32_LastDmaChunk = (ps_DmaChunkInfo->ui32_LastDmaChunk > 0) ? 0x1 : 0x0;
        pDmaInfo->ui32_AbortDma     = 0x0;
    }
    else
    {
        pDmaInfo->ui32_Offset = 0xFFFFFFFF;
        pDmaInfo->ui32_Size = 0x0;
        pDmaInfo->ui32_LastDmaChunk = 0x1;
        pDmaInfo->ui32_AbortDma     = 0x1;
    }

#ifdef BSID_SDCI_DBG
    BDBG_MSG(("next dma is off 0x%x size 0x%x last %d\n", pDmaInfo->ui32_Offset, pDmaInfo->ui32_Size, ps_DmaChunkInfo->ui32_LastDmaChunk));
#endif

    psQueueHeader->ui32_WaitForData = 0x0;

#ifdef BSID_USE_CACHED_MEMORY
   BMEM_FlushCache(hSid->hMem, (void *)pDmaInfo, sizeof(BSID_DmaChunkInfo));
   BMEM_FlushCache(hSid->hMem, (void *)psQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
    return;
}


/******************************************************************************
* Function name: BSID_P_SetDmaChunkInfo
*   -
* Comments:
*   -
******************************************************************************/
void BSID_P_SetDmaChunkInfo(
    BSID_ChannelHandle  hSidCh,
    const BSID_DmaChunkInfo *ps_DmaChunkInfo)
{
    BKNI_EnterCriticalSection();

    BSID_P_SetDmaChunkInfo_Isr(hSidCh, ps_DmaChunkInfo);

    BKNI_LeaveCriticalSection();

    return;
}


/******************************************************************************
* Function name: BSID_P_BuildGammaCorrectionTable
*   -
* Comments:
*   -
******************************************************************************/
void BSID_P_BuildGammaCorrectionTable(
    BSID_Handle hSid,
    unsigned char *gamma_correction_table
    )
{
    BSTD_UNUSED(hSid);
    BSTD_UNUSED(gamma_correction_table);

    return;
}


/******************************************************************************
* Function name: BSID_InitPlayback
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_InitPlayback(
    BSID_ChannelHandle  hSidCh,
    const BSID_StartDecodeSettings *ps_StartDecodeSettings)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSID_Handle hSid = hSidCh->hSid;

    /* RAVE related */
    {
        BSID_Handle hSid = hSidCh->hSid;
        BAVC_XptContextMap *pContextMap = ps_StartDecodeSettings->ps_RaveContextMap;
#ifdef BSID_USE_CACHED_MEMORY
        BSID_RaveStatusReport *pMemoryReport = (BSID_RaveStatusReport *)hSidCh->sRaveReport.pv_CachedAddr;
#else
        BSID_RaveStatusReport *pMemoryReport = (BSID_RaveStatusReport *)hSidCh->sRaveReport.pui32_VirtAddr;
#endif

#if 0
        BKNI_Printf("SID ITB: b 0x%x, e 0x%x, r 0x%x, v 0x%x, w 0x%x\n",
            BREG_Read32(hSid->hReg, pContextMap->ITB_Base),
            BREG_Read32(hSid->hReg, pContextMap->ITB_End),
            BREG_Read32(hSid->hReg, pContextMap->ITB_Read),
            BREG_Read32(hSid->hReg, pContextMap->ITB_Valid),
            BREG_Read32(hSid->hReg, pContextMap->ITB_Wrap));


        BKNI_Printf("SID CDB: b 0x%x, e 0x%x, r 0x%x, v 0x%x, w 0x%x\n",
            BREG_Read32(hSid->hReg, pContextMap->CDB_Base),
            BREG_Read32(hSid->hReg, pContextMap->CDB_End),
            BREG_Read32(hSid->hReg, pContextMap->CDB_Read),
            BREG_Read32(hSid->hReg, pContextMap->CDB_Valid),
            BREG_Read32(hSid->hReg, pContextMap->CDB_Wrap));
#endif

        pMemoryReport->CDB_Base                 = BREG_Read32(hSid->hReg, pContextMap->CDB_Read);
        pMemoryReport->CDB_End                  = BREG_Read32(hSid->hReg, pContextMap->CDB_End);
        pMemoryReport->from_pi.CDB_Read         = BREG_Read32(hSid->hReg, pContextMap->CDB_Read);
        pMemoryReport->from_pi.CDB_Valid        = BREG_Read32(hSid->hReg, pContextMap->CDB_Valid);
        pMemoryReport->from_pi.CDB_Wrap         = BREG_Read32(hSid->hReg, pContextMap->CDB_Wrap);
        pMemoryReport->from_fw.CDB_Read         = pMemoryReport->from_pi.CDB_Read;

        BKNI_Memcpy((void *)&hSidCh->sContextMap, (void *)pContextMap, sizeof(BAVC_XptContextMap));
        hSidCh->last_ITB_Read = BREG_Read32(hSid->hReg, pContextMap->ITB_Read);
#ifdef BSID_USE_CACHED_MEMORY
        BMEM_FlushCache(hSid->hMem, (void *)pMemoryReport, sizeof(BSID_RaveStatusReport));
#endif
    }

    /* Decode and Xdm related */
    {
#ifdef BSID_USE_CACHED_MEMORY
        BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pv_CachedAddr;
#else
        BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pui32_VirtAddr;
#endif
        BSID_PlaybackQueueEntry *psPbQueueEntry;
        unsigned int queue_elem = 0;

        psPbQueueHeader->ui32_InitPlayback = true;

        switch (hSidCh->e_ChannelChangeOutputMode)
        {
            case BSID_ChannelChangeOutputMode_eMute:

                psPbQueueHeader->ui32_NewPictWriteIndex = psPbQueueHeader->ui32_DecodedReadIndex = psPbQueueHeader->ui32_DisplayReadIndex = 0;

                for (queue_elem = 0; queue_elem < psPbQueueHeader->ui32_QueueSize; queue_elem++)
                {
                    psPbQueueEntry = (BSID_PlaybackQueueEntry *)((uint32_t)psPbQueueHeader + sizeof(BSID_PlaybackQueueHeader) + \
                                        (queue_elem * sizeof(BSID_PlaybackQueueEntry)));

                    psPbQueueEntry->ui32_OutputState = BSID_OutputBufferState_eIdle;
                    psPbQueueEntry->ui32_ChannelState = BSID_ChannelState_eReady;
                }

            break;

            case BSID_ChannelChangeOutputMode_eLastFramePreviousChannel:

                psPbQueueHeader->ui32_NewPictWriteIndex = psPbQueueHeader->ui32_DecodedReadIndex = psPbQueueHeader->ui32_DisplayReadIndex;

                for (queue_elem = 0; queue_elem < psPbQueueHeader->ui32_QueueSize; queue_elem++)
                {
                    psPbQueueEntry = (BSID_PlaybackQueueEntry *)((uint32_t)psPbQueueHeader + sizeof(BSID_PlaybackQueueHeader) + \
                                        (queue_elem * sizeof(BSID_PlaybackQueueEntry)));

                    if (psPbQueueEntry->ui32_OutputState != BSID_OutputBufferState_eXdmed)
                    {
                        psPbQueueEntry->ui32_OutputState = BSID_OutputBufferState_eIdle;
                        psPbQueueEntry->ui32_ChannelState = BSID_ChannelState_eReady;
                    }
                }

            break;

            default:
                BDBG_ERR(("Unsupported channel change mode"));
                return BERR_TRACE(BERR_UNKNOWN);
            break;
        }

        hSidCh->e_ChannelState = psPbQueueHeader->ui32_ChannelState = BSID_ChannelState_eDecode;
#ifdef BSID_USE_CACHED_MEMORY
        BMEM_FlushCache(hSid->hMem, (void *)psPbQueueHeader, sizeof(BSID_PlaybackQueueHeader));
#endif
    }

    return BERR_TRACE(retCode);
}


/*#define BSID_STARTDECODE_DBG*/
/******************************************************************************
* Function name: BSID_P_StartDecode
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_StartDecode(
    BSID_ChannelHandle  hSidCh,
    const BSID_StartDecodeSettings *ps_StartDecodeSettings)

{
   BERR_Code               retCode       = BERR_SUCCESS;
   BSID_Handle             hSid          = hSidCh->hSid;
#ifdef BSID_USE_CACHED_MEMORY
   BSID_CommandQueueHeader *psReqQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pv_CachedAddr;
   BSID_CommandQueueHeader *psDataQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sDataQueue.sBuf.pv_CachedAddr;
#else
   BSID_CommandQueueHeader *psReqQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sReqQueue.sBuf.pui32_VirtAddr;
   BSID_CommandQueueHeader *psDataQueueHeader = (BSID_CommandQueueHeader *)hSidCh->sDataQueue.sBuf.pui32_VirtAddr;
#endif
   BSID_Cmd_StartDecode *ps_StartDecodeCmd = NULL;
   BSID_ReqQueueEntry *psReqQueueEntry = NULL;
   BSID_DataQueueEntry *psDataQueueEntry = NULL;
   uint32_t            dataQueueEntryPhisAddr = 0x0;
   bool                    needAck = false;

   BDBG_ENTER(BSID_P_StartDecode);

   /* init rave, decode, xdm and channel data  */
   retCode = BSID_InitPlayback(hSidCh, ps_StartDecodeSettings);
   if (retCode != BERR_SUCCESS)
   {
       BDBG_ERR(("BSID_InitPlayback failed"));
       return BERR_TRACE(retCode);
   }

   /* prepare request queue elem */
   {
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psReqQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
       psReqQueueEntry = (BSID_ReqQueueEntry *)((uint32_t)psReqQueueHeader + sizeof(BSID_CommandQueueHeader) + \
                      (psReqQueueHeader->ui32_WriteIndex * sizeof(BSID_ReqQueueEntry)));
       BKNI_Memset((void *)psReqQueueEntry, 0x00, sizeof(BSID_ReqQueueEntry));
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psReqQueueEntry, sizeof(BSID_ReqQueueEntry));
#endif
       ps_StartDecodeCmd   = (BSID_Cmd_StartDecode *)psReqQueueEntry;
   }

   /* prepare data queue elem */
   {
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psDataQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
       psDataQueueEntry = (BSID_DataQueueEntry *)((uint32_t)psDataQueueHeader + sizeof(BSID_CommandQueueHeader) + \
                      (psDataQueueHeader->ui32_WriteIndex * sizeof(BSID_DataQueueEntry)));

       retCode = BMEM_Heap_ConvertAddressToOffset(hSid->hMem, (void *)psDataQueueEntry, &dataQueueEntryPhisAddr);
       if (retCode != BERR_SUCCESS)
       {
           BDBG_ERR(("BSID_P_SendCmdGetStreamInfo: BMEM_Heap_ConvertAddressToOffset failed rc = 0x%x\n", retCode));
           return BERR_TRACE(retCode);
       }

       BKNI_Memset((void *)psDataQueueEntry, 0x00, sizeof(BSID_DataQueueEntry));
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psDataQueueEntry, sizeof(BSID_DataQueueEntry));
#endif
   }

   /* prepare cmd body */
   {
       ps_StartDecodeCmd->cmdHdr.header   = BSID_MSG_HDR_STARTDECODE;
       ps_StartDecodeCmd->cmdHdr.needAck  = needAck;
       ps_StartDecodeCmd->cmdHdr.sequence = hSidCh->ui32_SequenceNumber++;

       ps_StartDecodeCmd->imgFormat                   = hSid->sFwHwConfig.ae_ImgFmtPi2Arc[BSID_ImageFormat_eJPEG];
       ps_StartDecodeCmd->raveReportBufAddr           = hSidCh->sRaveReport.ui32_PhisAddr;
       ps_StartDecodeCmd->playbackQueueAddr           = hSidCh->sPlaybackQueue.sBuf.ui32_PhisAddr;
       ps_StartDecodeCmd->queueDepth                  = hSidCh->sPlaybackQueue.sHdr.ui32_QueueSize;
       ps_StartDecodeCmd->dbgMsgOn                    = BSID_ARC_STARTDECODE_MSG_ENABLE;
       ps_StartDecodeCmd->sidMemStreamInfoBufPhisAddr = dataQueueEntryPhisAddr;
       ps_StartDecodeCmd->sidMemStreamInfoBufVirtAddr = (uint32_t)psDataQueueEntry;
       ps_StartDecodeCmd->sidMemStreamInfoBufSize     = sizeof(BSID_ImageHeader);
       ps_StartDecodeCmd->numPicToDecode              = 0xFFFFFFFF;
       ps_StartDecodeCmd->endianessInput              = !BSID_RAVE_CDB_ENDIAN; /* fw uses reverse notation */
       ps_StartDecodeCmd->endianessOutput             = hSid->sFwHwConfig.b_EndianessSwap;
       ps_StartDecodeCmd->bypass                      = false;
   }

#ifdef BSID_STARTDECODE_DBG
   BDBG_MSG(("cmd startdecode sent\n"));
   BDBG_MSG(("imgFormat               = 0x%x\n", ps_StartDecodeCmd->imgFormat));
   BDBG_MSG(("memoryReportBufAddr     = 0x%x\n", ps_StartDecodeCmd->raveReportBufAddr ));
   BDBG_MSG(("sidMemStreamInfoBufAddr = 0x%x\n", ps_StartDecodeCmd->sidMemStreamInfoBufPhisAddr));
   BDBG_MSG(("sidMemStreamInfoBufSize = 0x%x\n", ps_StartDecodeCmd->sidMemStreamInfoBufSize));
   BDBG_MSG(("endianessInput          = 0x%x\n", ps_StartDecodeCmd->endianessInput));
#endif

#ifdef BSID_USE_CACHED_MEMORY
   BMEM_FlushCache(hSid->hMem, (void *)ps_StartDecodeCmd, sizeof(BSID_Cmd_StartDecode));
#endif

   /* signal new queue enty to the ARC */
   {
       uint32_t ui32_hst2cpu;

       BKNI_EnterCriticalSection();
       ui32_hst2cpu = BREG_Read32(hSid->hReg, BCHP_SID_ARC_HST2CPU_STAT);
       BSID_INCR_INDEX(psReqQueueHeader->ui32_WriteIndex, hSidCh->ui32_QueueTrueDepth);
       BSID_INCR_INDEX(psDataQueueHeader->ui32_WriteIndex, hSidCh->ui32_QueueTrueDepth);
#ifdef BSID_USE_CACHED_MEMORY
       BMEM_FlushCache(hSid->hMem, (void *)psReqQueueHeader, sizeof(BSID_CommandQueueHeader));
       BMEM_FlushCache(hSid->hMem, (void *)psDataQueueHeader, sizeof(BSID_CommandQueueHeader));
#endif
       BREG_Write32(hSid->hReg, BCHP_SID_ARC_HST2CPU_STAT, (ui32_hst2cpu | (1 << hSidCh->ui32_ChannelNum)));
       BKNI_LeaveCriticalSection();
   }

#ifdef BSID_SAVE_BUFFER
    pthread_create(&SaveImagesThread, NULL, &fSaveImagesThread, (void *)hSidCh);
#endif

#ifdef BSID_STARTDECODE_DBG
   BDBG_MSG(("start decode done\n"));
#endif

   BDBG_LEAVE(BSID_P_StartDecode);
   return BERR_TRACE(retCode);
}


/*#define BSID_STOPDECODE_DBG*/
/******************************************************************************
* Function name: BSID_P_StopDecode
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_StopDecode(BSID_ChannelHandle  hSidCh)
{
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BSID_P_StopDecode);

#ifdef BSID_STOPDECODE_DBG
   BDBG_MSG(("stop decode cmd\n"));
#endif

    BSTD_UNUSED(hSidCh);

#ifdef BSID_STOPDECODE_DBG
   BDBG_MSG(("stop decode done\n"));
#endif

    return BERR_TRACE(retCode);
}


#define BSID_GET_ADDRESS(offset, address) { \
    BMEM_ConvertOffsetToAddress(hSid->hMem, offset, address); \
}\

#define BSID_ITB_ENTRY_BYTE_SIZE 16
#define BSID_ITB_BASEADDRESS_HDR 0x20000000
#define BSID_ITB_PTSDTS_HDR      0x21000000
#define BSID_GET_ADDRESS(offset, address) { \
    BMEM_ConvertOffsetToAddress(hSid->hMem, offset, address); \
}\


/******************************************************************************
* Function name: BSID_ReadItb4ByteAt
*   -
* Comments:
*   -
******************************************************************************/
int BSID_ReadValidByteDistance(
    int oBase,
    int oRead,
    int oValid,
    int oWrap
)
{
    if (oValid < oRead)
    {
        return ((oWrap - oRead + 1) + (oValid - oBase + 1));
    }
    else
    {
        return (oValid - oRead + 1);
    }
}


/******************************************************************************
* Function name: BSID_ReadItb4ByteAt
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_ReadItb4ByteAt(
    BSID_ChannelHandle hSidCh,
    uint32_t ItbRead,
    uint32_t *WordRead,
    uint32_t *newReadPtr)
{
    BSID_Handle hSid = hSidCh->hSid;
    BAVC_XptContextMap *pCtxMap = &hSidCh->sContextMap;
    uint32_t oBase  = BREG_Read32(hSid->hReg, pCtxMap->ITB_Base);
    uint32_t oValid = BREG_Read32(hSid->hReg, pCtxMap->ITB_Valid);
    uint32_t oRead  = ItbRead;
    uint32_t oWrap  = BREG_Read32(hSid->hReg, pCtxMap->ITB_Wrap);
    uint32_t word = 0;
    uint32_t auxWord = 0;
    unsigned char *pByte;
    int i;

    if (BSID_ReadValidByteDistance(oBase, oRead, oValid, oWrap) < 4)
    {
        return BERR_UNKNOWN;
    }

    BSID_GET_ADDRESS(oRead, (void *)&pByte);

#ifdef BSID_USE_CACHED_MEMORY
    BMEM_ConvertAddressToCached(hSid->hMem, pByte, (void *)&pByte);
    BMEM_FlushCache(hSid->hMem, (void *)pByte, sizeof(unsigned char));
#endif
    /*BKNI_Printf("ReadItb4ByteAt: Read 0x%x, Valid 0x%x, Wrap 0x%x\n",
    oRead, oValid, oWrap);*/

    for (i = 0; i < 32; i+=8)
    {
        auxWord = *pByte;
        word = (auxWord << i) | word;

        if (oRead == oWrap)
        {
            oRead = oBase;
            BSID_GET_ADDRESS(oRead, (void *)&pByte);
#ifdef BSID_USE_CACHED_MEMORY
            BMEM_ConvertAddressToCached(hSid->hMem, pByte, (void *)&pByte);
            BMEM_FlushCache(hSid->hMem, (void *)pByte, sizeof(unsigned char));
#endif
        }
        else
        {
            oRead++;
            pByte++;
        }
    }

    *newReadPtr = oRead;
    *WordRead = word;

    return BERR_SUCCESS;
}


/******************************************************************************
* Function name: BSID_GetNextPicItbInfo
*   -
* Comments:
*   -
******************************************************************************/
bool BSID_GetNextPicItbInfo(
    BSID_Handle hSid,
    BSID_ChannelHandle hSidCh,
    BAVC_XptContextMap *pCtxMap,
    uint32_t *BaseAddress,
    uint32_t *Pts)
{
    bool validItbInfo    = false;
    bool foundBaseAddress = false;
    bool foundPts         = false;
    uint32_t oRead   = BREG_Read32(hSid->hReg, pCtxMap->ITB_Read);
    uint32_t oValid  = BREG_Read32(hSid->hReg, pCtxMap->ITB_Valid);
    uint32_t header = 0x0;
    uint32_t oNewRead = 0x0;
    uint32_t lBaseAddress = 0x0;
    uint32_t lPts = 0x0;
    uint32_t lDummy = 0x0;
    BERR_Code rc = BERR_SUCCESS;

    oRead = hSidCh->last_ITB_Read;

    if ((oRead-1) == oValid)
    {
        return validItbInfo;
    }

    /* printf("oRead 0x%x oValid 0x%x\n", oRead, oValid); */

    /*
       Advance from current BaseAddress to next BaseAddress,
       extract BaseAddress for current picture and PTS
    */
    do
    {
        rc = BSID_ReadItb4ByteAt(hSidCh, oRead, &header, &oNewRead);
        if (rc != BERR_SUCCESS) {return validItbInfo;} else { oRead = oNewRead; }

        switch (header)
        {
            case BSID_ITB_BASEADDRESS_HDR:
                rc = BSID_ReadItb4ByteAt(hSidCh, oRead, &lBaseAddress, &oNewRead);
                if (rc != BERR_SUCCESS) {return validItbInfo;} else { oRead = oNewRead; }
                rc = BSID_ReadItb4ByteAt(hSidCh, oRead, &lDummy, &oNewRead);
                if (rc != BERR_SUCCESS) {return validItbInfo;} else { oRead = oNewRead; }
                rc = BSID_ReadItb4ByteAt(hSidCh, oRead, &lDummy, &oNewRead);
                if (rc != BERR_SUCCESS) {return validItbInfo;} else { oRead = oNewRead; }
                foundBaseAddress = true;
            break;
            case BSID_ITB_PTSDTS_HDR:
                rc = BSID_ReadItb4ByteAt(hSidCh, oRead, &lPts, &oNewRead);
                if (rc != BERR_SUCCESS) {return validItbInfo;} else { oRead = oNewRead; }
                rc = BSID_ReadItb4ByteAt(hSidCh, oRead, &lDummy, &oNewRead);
                if (rc != BERR_SUCCESS) {return validItbInfo;} else { oRead = oNewRead; }
                rc = BSID_ReadItb4ByteAt(hSidCh, oRead, &lDummy, &oNewRead);
                if (rc != BERR_SUCCESS) {return validItbInfo;} else { oRead = oNewRead; }
                foundPts = true;
                validItbInfo = true;
            break;

            default:
                rc = BSID_ReadItb4ByteAt(hSidCh, oRead, &lDummy, &oNewRead);
                if (rc != BERR_SUCCESS) {return validItbInfo;} else { oRead = oNewRead; }
                rc = BSID_ReadItb4ByteAt(hSidCh, oRead, &lDummy, &oNewRead);
                if (rc != BERR_SUCCESS) {return validItbInfo;} else { oRead = oNewRead; }
                rc = BSID_ReadItb4ByteAt(hSidCh, oRead, &lDummy, &oNewRead);
                if (rc != BERR_SUCCESS) {return validItbInfo;} else { oRead = oNewRead; }
            break;
        }

    }
    while (validItbInfo == false);

    BREG_Write32(hSid->hReg, pCtxMap->ITB_Read, (oRead-1));

    hSidCh->last_ITB_Read = oRead;

    *BaseAddress = lBaseAddress;
    *Pts = lPts;

    /*BKNI_Printf("BaseAddress 0x%x, PTS 0x%x\n", *BaseAddress, *Pts);*/

    return validItbInfo;
}


/*#define BSID_TRACE_PLAYBACK*/
#ifdef BSID_TRACE_PLAYBACK
#include <sys/time.h>
static uint32_t gettimeus()
{
     struct timeval tv;

     gettimeofday (&tv, NULL);

     return (tv.tv_sec * 1000000 + tv.tv_usec);
}

static uint32_t previous_time;
static uint32_t current_time;
static uint32_t vsync_index = 0;
static uint32_t next_pic_time;
static uint32_t display_time_current = 0;
static uint32_t itb_update_time = 0;
static uint32_t display_time_previous = 0;
#endif


/******************************************************************************
* Function name: BSID_UpdatePlaybackInfo
*   -
* Comments:
*   -
******************************************************************************/
void BSID_UpdatePlaybackInfo(BSID_ChannelHandle  hSidCh)
{
    if (hSidCh->e_ChannelState == BSID_ChannelState_eDecode)
    {
        BSID_Handle hSid = hSidCh->hSid;
#ifdef BSID_USE_CACHED_MEMORY
        BSID_RaveStatusReport *pMemoryReport = (BSID_RaveStatusReport *)hSidCh->sRaveReport.pv_CachedAddr;
        BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pv_CachedAddr;
#else
        BSID_RaveStatusReport *pMemoryReport = (BSID_RaveStatusReport *)hSidCh->sRaveReport.pui32_VirtAddr;
        BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pui32_VirtAddr;
#endif
        BSID_PlaybackQueueEntry *psPbQueueEntryIdle = NULL;
        BSID_PlaybackQueueEntry *psPbQueueEntryDecode = NULL;
        bool decodedBufAvailable = false;
        bool idleBufAvailable = false;
        uint32_t new_pic_write_index = 0;
        uint32_t decoded_read_index = 0;
        BAVC_XptContextMap *pCtxMap = &hSidCh->sContextMap;

#ifdef BSID_TRACE_PLAYBACK
        display_time_current = gettimeus();
#endif

        /* update CDB valid pointers, always */
        {
            uint32_t CBD_Valid;
            uint32_t CBD_Wrap;

            pMemoryReport->from_pi.CDB_Read = BREG_Read32(hSid->hReg, pCtxMap->CDB_Read);
            CBD_Valid = BREG_Read32(hSid->hReg, pCtxMap->CDB_Valid);
            CBD_Wrap = BREG_Read32(hSid->hReg, pCtxMap->CDB_Wrap);

            pMemoryReport->from_pi.CDB_Wrap         = CBD_Wrap;
            pMemoryReport->from_pi.CDB_Valid        = CBD_Valid;
#ifdef BSID_USE_CACHED_MEMORY
            BMEM_FlushCache(hSid->hMem, (void *)pMemoryReport, sizeof(BSID_RaveStatusReport));
#endif
        }

        /* do I have a new output buffer available, if yes post it to fw/hw */
        {
#ifdef BSID_USE_CACHED_MEMORY
            BMEM_FlushCache(hSid->hMem, (void *)psPbQueueHeader, sizeof(BSID_PlaybackQueueHeader));
#endif
            psPbQueueEntryIdle = (BSID_PlaybackQueueEntry *)((uint32_t)psPbQueueHeader + sizeof(BSID_PlaybackQueueHeader) + \
                            (psPbQueueHeader->ui32_NewPictWriteIndex * sizeof(BSID_PlaybackQueueEntry)));

#ifdef BSID_USE_CACHED_MEMORY
            BMEM_FlushCache(hSid->hMem, (void *)psPbQueueEntryIdle, sizeof(BSID_PlaybackQueueEntry));
#endif
/*
            BKNI_Printf("ITB b 0x%x r 0x%x, v 0x%x, w 0x%x\n",
            BREG_Read32(hSid->hReg, pCtxMap->ITB_Base),
            BREG_Read32(hSid->hReg, pCtxMap->ITB_Read),
            BREG_Read32(hSid->hReg, pCtxMap->ITB_Valid),
            BREG_Read32(hSid->hReg, pCtxMap->ITB_Wrap));
*/

            if (psPbQueueEntryIdle->ui32_OutputState == BSID_OutputBufferState_eIdle)
            {
                bool validItbEntry = false;
                unsigned int CdbAddress;
                unsigned int Pts;

                /* find BaseAddress and PTS from ITB */
                validItbEntry = BSID_GetNextPicItbInfo(hSid, hSidCh, &hSidCh->sContextMap, &CdbAddress, &Pts);
                if (validItbEntry == true)
                {
                    psPbQueueEntryIdle->ui32_CdbAddress    = CdbAddress;
                    psPbQueueEntryIdle->ui32_Pts           = Pts;
                    psPbQueueEntryIdle->ui32_OutputState   = BSID_OutputBufferState_eBooked;
                    psPbQueueEntryIdle->ui32_UnifiedPictureReady = false;

                    new_pic_write_index = psPbQueueHeader->ui32_NewPictWriteIndex;
                    idleBufAvailable = true;

                    BSID_INCR_INDEX(psPbQueueHeader->ui32_NewPictWriteIndex, psPbQueueHeader->ui32_QueueSize);
#ifdef BSID_USE_CACHED_MEMORY
                    BMEM_FlushCache(hSid->hMem, (void *)psPbQueueEntryIdle, sizeof(BSID_PlaybackQueueEntry));
#endif
                }
            }
        }

#if 0
        /* update CDB valid pointers, always */
        if (
        {
            pMemoryReport->from_pi.CDB_Read         = BREG_Read32(hSid->hReg, pCtxMap->CDB_Read);
            pMemoryReport->from_pi.CDB_Valid        = BREG_Read32(hSid->hReg, pCtxMap->CDB_Valid);
            pMemoryReport->from_pi.CDB_Wrap         = BREG_Read32(hSid->hReg, pCtxMap->CDB_Wrap);
        }
#endif
        /* check if SID has a new decoded buffer, if yes advance Cdb Read*/
        {
#ifdef BSID_USE_CACHED_MEMORY
            BMEM_FlushCache(hSid->hMem, (void *)psPbQueueHeader, sizeof(BSID_PlaybackQueueHeader));
#endif
            psPbQueueEntryDecode = (BSID_PlaybackQueueEntry *)((uint32_t)psPbQueueHeader + sizeof(BSID_PlaybackQueueHeader) + \
                            (psPbQueueHeader->ui32_DisplayReadIndex * sizeof(BSID_PlaybackQueueEntry)));
#ifdef BSID_USE_CACHED_MEMORY
            BMEM_FlushCache(hSid->hMem, (void *)psPbQueueEntryDecode, sizeof(BSID_PlaybackQueueEntry));
#endif

            if (psPbQueueEntryDecode->ui32_OutputState == BSID_OutputBufferState_eDecoded)
            {
                decoded_read_index = psPbQueueHeader->ui32_DisplayReadIndex;
                decodedBufAvailable = true;

                if (psPbQueueEntryDecode->ui32_UnifiedPictureReady == false)
                {
                    BSID_PrepareNextUnifiedPicture(hSidCh, psPbQueueEntryDecode);

#ifndef BSID_SAVE_BUFFER/*
                    BKNI_Printf("[%d, %d]Read 0x%x a 0x%x Size 0x%x LastRead 0x%x valid 0x%x wrap 0x%x (0x%x, 0x%x, 0x%x, 0x%x)\n",
                    psPbQueueHeader->ui32_DisplayReadIndex,
                    psPbQueueHeader->ui32_DecodedReadIndex,
                    pMemoryReport->from_pi.CDB_Read,
                    psPbQueueEntryDecode->ui32_CdbAddress,
                    psPbQueueEntryDecode->ui32_CdbSize,
                    psPbQueueEntryDecode->ui32_LastCdbRead,
                    pMemoryReport->from_pi.CDB_Valid,
                    pMemoryReport->from_pi.CDB_Wrap,
                    pMemoryReport->from_fw.CDB_Wrap,
                    psPbQueueEntryDecode->ui32_PictureIdx,
                    psPbQueueEntryDecode->ui32_Address,
                    psPbQueueEntryDecode->ui32_WrapAroundOccurred);
*/
                    /* update CdbRead */
                    BREG_Write32(hSid->hReg, pCtxMap->CDB_Read, psPbQueueEntryDecode->ui32_LastCdbRead);
#endif
                    psPbQueueEntryDecode->ui32_UnifiedPictureReady = true;
#ifdef BSID_USE_CACHED_MEMORY
                    BMEM_FlushCache(hSid->hMem, (void *)psPbQueueEntryDecode, sizeof(BSID_PlaybackQueueEntry));
#endif
                }
            }
        }

#ifdef BSID_TRACE_PLAYBACK
        itb_update_time = gettimeus();
#endif
#ifdef BSID_USE_CACHED_MEMORY
        BMEM_FlushCache(hSid->hMem, (void *)pMemoryReport, sizeof(BSID_RaveStatusReport));
#endif

#ifdef BSID_TRACE_PLAYBACK
        if (pMemoryReport->from_pi.CDB_Read <= pMemoryReport->from_pi.CDB_Valid)
        {
            BKNI_Printf("[0x%08x,%d,%d]CDB: 0x%x,0x%x,0x%x,0x%x ",
            vsync_index,
            (display_time_current - display_time_previous)/1000,
            (itb_update_time - display_time_current)/1000,
            pMemoryReport->from_pi.CDB_Read,
            pMemoryReport->from_pi.CDB_Valid,
            pMemoryReport->from_pi.CDB_Wrap,
            (pMemoryReport->from_pi.CDB_Valid - pMemoryReport->from_pi.CDB_Read));
        }
        else
        {
            BKNI_Printf("[0x%08x,%d,%d]CDBw: 0x%x,0x%x,0x%x,0x%x ",
            vsync_index,
            (display_time_current - display_time_previous)/1000,
            (itb_update_time - display_time_current)/1000,
            pMemoryReport->from_pi.CDB_Read,
            pMemoryReport->from_pi.CDB_Valid,
            pMemoryReport->from_pi.CDB_Wrap,
            ((pMemoryReport->from_pi.CDB_Wrap - pMemoryReport->from_pi.CDB_Read) + (pMemoryReport->from_pi.CDB_Valid - pMemoryReport->CDB_Base)));
        }

        if (idleBufAvailable == true)
        {
            BKNI_Printf("BA[%d]:o 0x%x,i 0x%x ",
            new_pic_write_index,
            psPbQueueEntryIdle->ui32_OutputBufferAddress,
            psPbQueueEntryIdle->ui32_CdbAddress);
        }

        if (decodedBufAvailable == true)
        {
            BKNI_Printf("PR[%d,%d]:o 0x%x,p 0x%x,dt %d,st %d",
            decoded_read_index,
            psPbQueueEntryDecode->ui32_PictureIdx,
            psPbQueueEntryDecode->ui32_OutputBufferAddress,
            psPbQueueEntryDecode->ui32_Pts,
            psPbQueueEntryDecode->ui32_decode_time,
            psPbQueueEntryDecode->ui32_setup_time);
        }

        BKNI_Printf("\n");
#endif

#ifdef BSID_TRACE_PLAYBACK
        display_time_previous = display_time_current;
#endif

#ifdef BSID_SAVE_BUFFER
        if (decodedBufAvailable == true)
        {
            if (1/*psPbQueueEntryDecode->ui32_PictureIdx == 2 && psPbQueueEntryDecode->ui32_DecodeErrors == 0*/)
            {
                uint32_t input_buffer_offset[2] = {0, 0};
                uint32_t input_buffer_size[2] = {0, 0};
                uint32_t num_input_buffer = 0;

                if (((psPbQueueEntryDecode->ui32_CdbAddress + 0xA + psPbQueueEntryDecode->ui32_CdbSize - 1) <= pMemoryReport->from_pi.CDB_Valid) ||
                    ((psPbQueueEntryDecode->ui32_CdbAddress + 0xA + psPbQueueEntryDecode->ui32_CdbSize - 1) <= pMemoryReport->from_pi.CDB_Wrap))
                {
                    input_buffer_offset[0] = psPbQueueEntryDecode->ui32_CdbAddress + 0xA;
                    input_buffer_size[0] = psPbQueueEntryDecode->ui32_CdbSize;
                    num_input_buffer = 1;
                }
                else
                {
                    uint32_t CdbSize = psPbQueueEntryDecode->ui32_CdbSize;
                    input_buffer_offset[0] = psPbQueueEntryDecode->ui32_CdbAddress + 0xA;
                    input_buffer_size[0] = (pMemoryReport->from_pi.CDB_Wrap - input_buffer_offset[0] + 1);
                    CdbSize -= input_buffer_size[0];
                    input_buffer_offset[1] = pMemoryReport->CDB_Base;
                    input_buffer_size[1] = CdbSize;
                    num_input_buffer = 2;
                }

                BKNI_Printf("Save Image&Cdb[%d]: Cdb 0x%x Size 0x%x, Dma0 0x%x Size0 0x%x, Dma1 0x%x Size1 0x%x\n",
                psPbQueueEntryDecode->ui32_PictureIdx,
                psPbQueueEntryDecode->ui32_CdbAddress,
                psPbQueueEntryDecode->ui32_CdbSize,
                input_buffer_offset[0], input_buffer_size[0],
                input_buffer_offset[1], input_buffer_size[1]);

                BSID_SaveImageData(
                    hSidCh,
                    psPbQueueEntryDecode->ui32_PictureIdx,
                    psPbQueueEntryDecode->ui32_Address,
                    psPbQueueEntryDecode->ui32_Width,
                    psPbQueueEntryDecode->ui32_Pitch,
                    psPbQueueEntryDecode->ui32_Height,
                    input_buffer_offset, input_buffer_size, num_input_buffer);

                if (psPbQueueEntryDecode->ui32_DecodeErrors != 0)
                {
                    getchar();
                }

                getchar();

                BSID_INCR_INDEX(psPbQueueHeader->ui32_DisplayReadIndex, psPbQueueHeader->ui32_QueueSize);
                psPbQueueEntryDecode->ui32_OutputState = BSID_OutputBufferState_eIdle;
            }

            BKNI_Printf("Updating Cdb with 0x%x\n", psPbQueueEntryDecode->ui32_LastCdbRead);
            /* update CdbRead */
            BREG_Write32(hSid->hReg, pCtxMap->CDB_Read, psPbQueueEntryDecode->ui32_LastCdbRead);
        }
#endif
    }

    return;
}

/******************************************************************************
* Function name: BSID_P_DisableForFlush
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_DisableForFlush(BSID_ChannelHandle hSidCh)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSID_Handle hSid = hSidCh->hSid;
#ifdef BSID_USE_CACHED_MEMORY
    BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pv_CachedAddr;
#else
    BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pui32_VirtAddr;
#endif

    BDBG_ENTER(BSID_P_DisableForFlush);

    /*
     * Signal FW to suspend decode operation.
     */
    hSidCh->e_ChannelState = psPbQueueHeader->ui32_ChannelState = BSID_ChannelState_eSuspend;

#ifdef BSID_USE_CACHED_MEMORY
    BMEM_FlushCache(hSid->hMem, (void *)psPbQueueHeader, sizeof(BSID_PlaybackQueueHeader));
#endif
    /*
     * Wait for FW/HW acknoledge on suspend request.
     */
    while (psPbQueueHeader->ui32_ChannelState != BSID_ChannelState_eReady)
    {
        BKNI_Delay(500);
#ifdef BSID_USE_CACHED_MEMORY
        BMEM_FlushCache(hSid->hMem, (void *)psPbQueueHeader, sizeof(BSID_PlaybackQueueHeader));
#endif
    }

    /*
     * FW suspended.
     */

    BDBG_ENTER(BSID_P_DisableForFlush);

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_P_FlushDecode
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_FlushDecode(BSID_ChannelHandle hSidCh)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSID_StartDecodeSettings ps_StartDecodeSettings;

    BDBG_ENTER(BSID_P_FlushDecode);

    /* use current start decode state to re-start the channel */
    ps_StartDecodeSettings.ps_RaveContextMap = &hSidCh->sContextMap;

    BSID_P_StartDecode(hSidCh, &ps_StartDecodeSettings);

    BDBG_ENTER(BSID_P_FlushDecode);

    return BERR_TRACE(retCode);
}


/******************************************************************************
* Function name: BSID_P_GetPictureCount_isr
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_GetPictureCount_isr(void *pvHandle, uint32_t *puiPictureCount)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSID_ChannelHandle  hSidCh = (BSID_ChannelHandle)pvHandle;
    BSID_Handle hSid = hSidCh->hSid;

    BDBG_ENTER(BSID_P_GetPictureCount_isr);

#ifdef BSID_SAVE_BUFFER
    return BERR_TRACE(retCode); /* output from SID goes to file */
#endif

    if (hSidCh->e_ChannelState == BSID_ChannelState_eDecode)
    {
        unsigned int i;
#ifdef BSID_USE_CACHED_MEMORY
        BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pv_CachedAddr;
#else
        BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pui32_VirtAddr;
#endif
        BSID_PlaybackQueueEntry *psPbQueueEntry;

        *puiPictureCount = 0;
#ifdef BSID_USE_CACHED_MEMORY
        BMEM_FlushCache(hSid->hMem, (void *)psPbQueueHeader, sizeof(BSID_PlaybackQueueHeader));
#endif
        for (i = 0; i < psPbQueueHeader->ui32_QueueSize; i++)
        {
            psPbQueueEntry = (BSID_PlaybackQueueEntry *)((uint32_t)psPbQueueHeader + sizeof(BSID_PlaybackQueueHeader) + \
                            (i * sizeof(BSID_PlaybackQueueEntry)));

#ifdef BSID_USE_CACHED_MEMORY
            BMEM_FlushCache(hSid->hMem, (void *)psPbQueueEntry, sizeof(BSID_PlaybackQueueEntry));
#endif
            if (psPbQueueEntry->ui32_OutputState == BSID_OutputBufferState_eDecoded)
            {
                (*puiPictureCount)++;
            }
#ifdef BSID_USE_CACHED_MEMORY
            BMEM_FlushCache(hSid->hMem, (void *)psPbQueueHeader, sizeof(BSID_PlaybackQueueHeader));
#endif
        }

#ifdef BSID_TRACE_PLAYBACK
        BKNI_Printf("[0x%08x] pr (%d)\n",
        vsync_index,
        *puiPictureCount);
#endif
    }

    BDBG_ENTER(BSID_P_GetPictureCount_isr);

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_P_PeekAtPicture_isr
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_PeekAtPicture_isr(void *pvHandle, uint32_t uiIndex, const BXDM_Picture **pUnifiedPicture)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSID_ChannelHandle  hSidCh = (BSID_ChannelHandle)pvHandle;
    BSID_Handle hSid = hSidCh->hSid;

    BDBG_ENTER(BSID_P_PeekAtPicture_isr);

    if (hSidCh->e_ChannelState == BSID_ChannelState_eDecode)
    {
#ifdef BSID_USE_CACHED_MEMORY
        BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pv_CachedAddr;
#else
        BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pui32_VirtAddr;
#endif
        BSID_PlaybackQueueEntry *psPbQueueEntry;

#ifdef BSID_USE_CACHED_MEMORY
        BMEM_FlushCache(hSid->hMem, (void *)psPbQueueHeader, sizeof(BSID_PlaybackQueueHeader));
#endif
        psPbQueueEntry = (BSID_PlaybackQueueEntry *)((uint32_t)psPbQueueHeader + sizeof(BSID_PlaybackQueueHeader) + \
                        (psPbQueueHeader->ui32_DisplayReadIndex * sizeof(BSID_PlaybackQueueEntry)));

#ifdef BSID_USE_CACHED_MEMORY
        BMEM_FlushCache(hSid->hMem, (void *)psPbQueueEntry, sizeof(BSID_PlaybackQueueEntry));
#endif
        /*
         * Xdm wants to peek at a specific pic.
         */
        if (psPbQueueEntry->ui32_OutputState == BSID_OutputBufferState_eDecoded)
        {
            ((BXDM_Picture *)psPbQueueEntry->ps_UnifiedPicture)->stPTS.uiValue = psPbQueueEntry->ui32_Pts;
            *pUnifiedPicture = ((BXDM_Picture *)psPbQueueEntry->ps_UnifiedPicture);
        }
        else
        {
            *pUnifiedPicture = NULL;
        }

#ifdef BSID_TRACE_PLAYBACK
        BKNI_Printf("[0x%08x] pa (%d)\n", vsync_index, uiIndex);
#else
        BSTD_UNUSED(uiIndex);
#endif
    }

    BDBG_ENTER(BSID_P_PeekAtPicture_isr);

    return BERR_TRACE(retCode);
}

void BSID_PrepareNextUnifiedPicture(
    BSID_ChannelHandle  hSidCh,
    BSID_PlaybackQueueEntry *psPbQueueEntry)
{
    BSID_Handle hSid = hSidCh->hSid;
    BXDM_Picture *pUnifiedPicture = ((BXDM_Picture *)psPbQueueEntry->ps_UnifiedPicture);

    BKNI_Memset((void *)pUnifiedPicture, 0x0, sizeof(BXDM_Picture));

    /* get output buffer virtual address */
    BMEM_ConvertOffsetToAddress(hSidCh->hChHeap, psPbQueueEntry->ui32_OutputBufferAddress, &pUnifiedPicture->stBufferInfo.pLumaBufferVirtualAddress);

    /* pixel format */
    pUnifiedPicture->stBufferInfo.stPixelFormat.ePixelFormat = ae_PxlFmtPi2Pxl[hSid->sFwHwConfig.b_EndianessSwap][ae_PxlFmtArc2Pi[psPbQueueEntry->ui32_PixelFormat][psPbQueueEntry->ui32_BitPerPixel]];
    pUnifiedPicture->stBufferInfo.stPixelFormat.bValid = true;

    /* true image width */
    pUnifiedPicture->stBufferInfo.stSource.uiWidth = psPbQueueEntry->ui32_TrueWidth;
    pUnifiedPicture->stBufferInfo.stSource.uiHeight = psPbQueueEntry->ui32_TrueHeight;
    pUnifiedPicture->stBufferInfo.stSource.bValid = true;

    /* display image width */
    pUnifiedPicture->stBufferInfo.stDisplay.uiWidth = psPbQueueEntry->ui32_Width;
    pUnifiedPicture->stBufferInfo.stDisplay.uiHeight = psPbQueueEntry->ui32_Height;
    pUnifiedPicture->stBufferInfo.stDisplay.bValid = true;

    /* pitch */
    pUnifiedPicture->stBufferInfo.uiRowStride = psPbQueueEntry->ui32_Pitch;

    /* buffer info valid or not */
    pUnifiedPicture->stBufferInfo.bValid = (psPbQueueEntry->ui32_DecodeErrors == 0);

    pUnifiedPicture->stPTS.uiValue = psPbQueueEntry->ui32_Pts;
    pUnifiedPicture->stPTS.bValid = true;

    /* aspect ratio */
    pUnifiedPicture->stAspectRatio.eAspectRatio = BFMT_AspectRatio_eUnknown;
    pUnifiedPicture->stAspectRatio.bValid = false;

    pUnifiedPicture->stBufferInfo.hHeap = hSidCh->hChHeap;

    pUnifiedPicture->stFrameRate.eType = BXDM_Picture_FrameRateType_eUnknown;

    pUnifiedPicture->stBufferInfo.ePulldown = BXDM_Picture_PullDown_eFrameX1;

#if 1
    BDBG_MSG(("V %d, oAddr %p (off 0x%x), sWxH %dx%d, dWxH %dx%d, pitch 0x%x, pf 0x%x (0x%x), PTS 0x%x",
    pUnifiedPicture->stBufferInfo.bValid,
    pUnifiedPicture->stBufferInfo.pLumaBufferVirtualAddress,
    psPbQueueEntry->ui32_OutputBufferAddress,
    pUnifiedPicture->stBufferInfo.stSource.uiWidth,
    pUnifiedPicture->stBufferInfo.stSource.uiHeight,
    pUnifiedPicture->stBufferInfo.stDisplay.uiWidth,
    pUnifiedPicture->stBufferInfo.stDisplay.uiHeight,
    pUnifiedPicture->stBufferInfo.uiRowStride,
    pUnifiedPicture->stBufferInfo.stPixelFormat.ePixelFormat,
    BPXL_eY18_Cr8_Y08_Cb8,
    pUnifiedPicture->stPTS.uiValue));
#endif

    return;
}


/******************************************************************************
* Function name: BSID_P_GetNextPicture_isr
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_GetNextPicture_isr(void *pvHandle, const BXDM_Picture **pUnifiedPicture)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSID_ChannelHandle  hSidCh = (BSID_ChannelHandle)pvHandle;
    BSID_Handle hSid = hSidCh->hSid;

    BDBG_ENTER(BSID_P_GetNextPicture_isr);

#ifdef BSID_TRACE_PLAYBACK
    current_time = gettimeus();
#endif

    if (hSidCh->e_ChannelState == BSID_ChannelState_eDecode)
    {
#ifdef BSID_USE_CACHED_MEMORY
        BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pv_CachedAddr;
#else
        BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pui32_VirtAddr;
#endif
        BSID_PlaybackQueueEntry *psPbQueueEntry;

#ifdef BSID_USE_CACHED_MEMORY
        BMEM_FlushCache(hSid->hMem, (void *)psPbQueueHeader, sizeof(BSID_PlaybackQueueHeader));
#endif
        psPbQueueEntry = (BSID_PlaybackQueueEntry *)((uint32_t)psPbQueueHeader + sizeof(BSID_PlaybackQueueHeader) + \
                        (psPbQueueHeader->ui32_DisplayReadIndex * sizeof(BSID_PlaybackQueueEntry)));
#ifdef BSID_USE_CACHED_MEMORY
        BMEM_FlushCache(hSid->hMem, (void *)psPbQueueEntry, sizeof(BSID_PlaybackQueueEntry));
#endif

        /*
         * Do I have a new output buffer to display? if yes post it Xdm
         */
        if (psPbQueueEntry->ui32_OutputState == BSID_OutputBufferState_eDecoded)
        {

#ifdef BSID_TRACE_PLAYBACK
            BKNI_Printf("[0x%08x] ToD(%d): 0x%x, 0x%x\n",
            vsync_index,
            (current_time - next_pic_time)/1000,
            psPbQueueEntry->ui32_OutputBufferAddress, (*pUnifiedPicture)->stPTS.uiValue);
#endif

            *pUnifiedPicture = ((BXDM_Picture *)psPbQueueEntry->ps_UnifiedPicture);

            psPbQueueEntry->ui32_OutputState = BSID_OutputBufferState_eXdmed;

            BSID_INCR_INDEX(psPbQueueHeader->ui32_DisplayReadIndex, psPbQueueHeader->ui32_QueueSize);
#ifdef BSID_USE_CACHED_MEMORY
            BMEM_FlushCache(hSid->hMem, (void *)psPbQueueEntry, sizeof(BSID_PlaybackQueueEntry));
#endif
        }
        else
        {
            *pUnifiedPicture = NULL;
        }
    }

#ifdef BSID_TRACE_PLAYBACK
    next_pic_time = current_time;
#endif

    BDBG_ENTER(BSID_P_GetNextPicture_isr);

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_P_ReleasePicture_isr
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_ReleasePicture_isr(void *pvHandle, const BXDM_Picture *pUnifiedPicture, const BXDM_Decoder_ReleasePictureInfo *pReleasePictureInfo)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSID_ChannelHandle  hSidCh = (BSID_ChannelHandle)pvHandle;
    BSID_Handle         hSid = hSidCh->hSid;

    BSTD_UNUSED(pReleasePictureInfo);

    BDBG_ENTER(BSID_P_ReleasePicture_isr);

#ifdef BSID_TRACE_PLAYBACK
    previous_time = gettimeus();
#endif

    if (hSidCh->e_ChannelState == BSID_ChannelState_eDecode)
    {
#ifdef BSID_USE_CACHED_MEMORY
        BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pv_CachedAddr;
#else
        BSID_PlaybackQueueHeader *psPbQueueHeader = (BSID_PlaybackQueueHeader *)hSidCh->sPlaybackQueue.sBuf.pui32_VirtAddr;
#endif
        BSID_PlaybackQueueEntry *psPbQueueEntry;
        unsigned int buffer_index;

        /*
         * Look for the buffer that need to be released.
         */
        for (buffer_index = 0; buffer_index < psPbQueueHeader->ui32_QueueSize; buffer_index++)
        {
#ifdef BSID_USE_CACHED_MEMORY
            BMEM_FlushCache(hSid->hMem, (void *)psPbQueueHeader, sizeof(BSID_PlaybackQueueHeader));
#endif
            psPbQueueEntry = (BSID_PlaybackQueueEntry *)((uint32_t)psPbQueueHeader + sizeof(BSID_PlaybackQueueHeader) + \
                            (buffer_index * sizeof(BSID_PlaybackQueueEntry)));

#ifdef BSID_USE_CACHED_MEMORY
            BMEM_FlushCache(hSid->hMem, (void *)psPbQueueEntry, sizeof(BSID_PlaybackQueueEntry));
#endif
            if ((BXDM_Picture *)psPbQueueEntry->ps_UnifiedPicture == pUnifiedPicture)
            {
                psPbQueueEntry->ui32_OutputState = BSID_OutputBufferState_eIdle;

#ifdef BSID_TRACE_PLAYBACK
                BKNI_Printf("[0x%08x] ToR: 0x%x\n",
                vsync_index,
                psPbQueueEntry->ui32_OutputBufferAddress);
#endif
#ifdef BSID_USE_CACHED_MEMORY
                BMEM_FlushCache(hSid->hMem, (void *)psPbQueueEntry, sizeof(BSID_PlaybackQueueEntry));
#endif
                break;
            }
        }
    }

    BDBG_ENTER(BSID_P_ReleasePicture_isr);

    return BERR_TRACE(retCode);
}


/******************************************************************************
* Function name: BSID_P_RequestPictureDrop_isr
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_DisplayInterruptEvent_isr(void *pvHandle)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSID_ChannelHandle  hSidCh = (BSID_ChannelHandle)pvHandle;

#ifdef BSID_TRACE_PLAYBACK
    vsync_index++;
#endif

    BDBG_ENTER(BSID_P_DisplayInterruptEvent_isr);

#ifndef BSID_SAVE_BUFFER
    BSID_UpdatePlaybackInfo(hSidCh);
#else
    BSTD_UNUSED(hSidCh);
#endif

    BDBG_ENTER(BSID_P_DisplayInterruptEvent_isr);

    return BERR_TRACE(retCode);
}

#if 0 /* not used for now */
/******************************************************************************
* Function name: BSID_P_GetPictureDropPendingCount_isr
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_GetPictureDropPendingCount_isr(void *pvHandle, uint32_t *puiPictureDropPendingCount)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSID_ChannelHandle  hSidCh = (BSID_ChannelHandle)pvHandle;

    BSTD_UNUSED(puiPictureDropPendingCount);
    BSTD_UNUSED(hSidCh);

    BDBG_ENTER(BSID_P_GetPictureDropPendingCount_isr);

    BDBG_ENTER(BSID_P_GetPictureDropPendingCount_isr);

    return BERR_TRACE(retCode);
}

/******************************************************************************
* Function name: BSID_P_RequestPictureDrop_isr
*   -
* Comments:
*   -
******************************************************************************/
BERR_Code BSID_P_RequestPictureDrop_isr(void *pvHandle, uint32_t *puiPictureDropRequestCount)
{
    BERR_Code retCode = BERR_SUCCESS;
    BSID_ChannelHandle  hSidCh = (BSID_ChannelHandle)pvHandle;

    BSTD_UNUSED(puiPictureDropRequestCount);
    BSTD_UNUSED(hSidCh);

    BDBG_ENTER(BSID_P_RequestPictureDrop_isr);

    BDBG_ENTER(BSID_P_RequestPictureDrop_isr);

    return BERR_TRACE(retCode);
}
#endif

#ifdef BSID_SAVE_BUFFER
void *fSaveImagesThread(void *pCtx)
{
    BSID_ChannelHandle hSidCh = (BSID_ChannelHandle)pCtx;

    while (true)
    {
        BSID_UpdatePlaybackInfo(hSidCh);
        BKNI_Sleep(30);
    }

    return NULL;
}

#define shift 10
/* BT.709 -> sRGB */
/* R = Y * 1.164 + Cb * 0.000 + Cr * 1.787 - 247 */
/* G = Y * 1.164 - Cb * 0.531 - Cr * 0.222 + 76 */
/* B = Y * 1.164 + Cb * 2.112 + Cr * 0.000 - 289 */
static int64_t coeff[3][4] = {
    {(int64_t)(1.164*(1<<shift)), (int64_t)( 0.000*(1<<shift)), (int64_t)(  1.787*(1<<shift)), (int64_t)(-247.000*(1<<shift))},
    {(int64_t)(1.164*(1<<shift)), (int64_t)(-0.531*(1<<shift)), (int64_t)( -0.222*(1<<shift)), (int64_t)(  76.000*(1<<shift))},
    {(int64_t)(1.164*(1<<shift)), (int64_t)( 2.112*(1<<shift)), (int64_t)(  0.000*(1<<shift)), (int64_t)(-289.000*(1<<shift))}
};

void BSID_SaveOutputImageAsPpm(
    BSID_ChannelHandle hSidCh,
    uint32_t output_buffer_offset,
    uint32_t output_buffer_width,
    uint32_t output_buffer_pitch,
    uint32_t output_buffer_height,
    char *file_path)
{
    BERR_Code retCode = BERR_SUCCESS;
    FILE *pFile;
    void *bufAddr = NULL;
    uint32_t *bufAddr32 = NULL;
    uint32_t *pPixel;
    uint32_t Y08Cb8;
    uint32_t Y18Cr8;
    uint32_t Y, Cb, Cr;
    int64_t R, G, B;
    uint32_t i = 0;
    uint32_t n = 0;
    char header[30];
    uint8_t *pImage;
    uint8_t *pImageBase;

    pFile = fopen(file_path, "wb");
    if (pFile == NULL)
    {
        printf("failed to open pOutBuf file\n");
        return;
    }

    retCode = BMEM_ConvertOffsetToAddress(hSidCh->hChHeap, output_buffer_offset, &bufAddr);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_SaveOutputImageAsYuyv: BMEM_ConvertAddressToOffset returned with error 0x%x", retCode));
        return;
    }

    pImageBase = pImage = (uint8_t *)BKNI_Malloc(output_buffer_width*output_buffer_height*3);
    if (pImageBase == NULL)
    {
        BKNI_Printf("No memory to save %s\n");
        fclose(pFile);
        return;
    }

    bufAddr32 = (uint32_t *)bufAddr;

    snprintf(header, 30, "P6\n%d %d\n255\n", output_buffer_width, output_buffer_height);

    fwrite(header, 1, strlen(header), pFile);

    i = 0;
    n = 0;
    do
    {
        /*BKNI_Printf("\ni=%d\n", i);*/
        pPixel = bufAddr32 + i;
        Y08Cb8 = ((*pPixel) & 0xFFFF);
        Y18Cr8 = ((*pPixel) & 0xFFFF0000)>>16;

        /*BKNI_Printf("Pixel 0x%x, YCb 0x%x YCr 0x%x\n", *pPixel, Y08Cb8, Y18Cr8);*/

        Y  = (Y08Cb8>>8)&0xFF;
        Cb = Y08Cb8 & 0xFF;
        Cr = Y18Cr8 & 0xFF;

        /* BT.601 -> sRGB */
        /* R = Y * 1.164 + Cb * 0.000 + Cr * 1.596 - 223 */
        /* G = Y * 1.164 - Cb * 0.391 - Cr * 0.813 + 135 */
        /* B = Y * 1.164 + Cb * 2.018 + Cr * 0.000 - 277 */
        R = ((Y*coeff[0][0]) + (Cb*coeff[0][1]) + (Cr*coeff[0][2]) + coeff[0][3])/(1<<shift);
        G = ((Y*coeff[1][0]) + (Cb*coeff[1][1]) + (Cr*coeff[1][2]) + coeff[1][3])/(1<<shift);
        B = ((Y*coeff[2][0]) + (Cb*coeff[2][1]) + (Cr*coeff[2][2]) + coeff[2][3])/(1<<shift);
        *pImage = (uint8_t)R; pImage++;
        *pImage = (uint8_t)G; pImage++;
        *pImage = (uint8_t)B; pImage++;

        Y  = (Y18Cr8>>8)&0xFF;
        Cb = Y08Cb8 & 0xFF;
        Cr = Y18Cr8 & 0xFF;

        /*BKNI_Printf("AY1CbCr 0x%x\n", AYCbCr);*/

        R = ((Y*coeff[0][0]) + (Cb*coeff[0][1]) + (Cr*coeff[0][2]) + coeff[0][3])/(1<<shift);
        G = ((Y*coeff[1][0]) + (Cb*coeff[1][1]) + (Cr*coeff[1][2]) + coeff[1][3])/(1<<shift);
        B = ((Y*coeff[2][0]) + (Cb*coeff[2][1]) + (Cr*coeff[2][2]) + coeff[2][3])/(1<<shift);
        *pImage = (uint8_t)R; pImage++;
        *pImage = (uint8_t)G; pImage++;
        *pImage = (uint8_t)B; pImage++;

        n+=6;

        i++;
    } while (i < ((output_buffer_pitch * output_buffer_height)>>2));

    fwrite(pImageBase, sizeof(uint8_t), n, pFile);

    fclose(pFile);

    BKNI_Free(pImageBase);

    BKNI_Printf("Image %s saved\n", file_path);

    return;
}

void BSID_SaveOutputImageAsYuyv(
    BSID_ChannelHandle hSidCh,
    uint32_t output_buffer_offset,
    uint32_t output_buffer_width,
    uint32_t output_buffer_pitch,
    uint32_t output_buffer_height,
    char *file_path)
{
    BERR_Code retCode = BERR_SUCCESS;
    FILE *pFile;
    void *bufAddr = NULL;

    BSTD_UNUSED(output_buffer_width);

    pFile = fopen(file_path, "wb");
    if (pFile == NULL)
    {
        printf("failed to open pOutBuf file\n");
        return;
    }

    retCode = BMEM_ConvertOffsetToAddress(hSidCh->hChHeap, output_buffer_offset, &bufAddr);
    if (retCode != BERR_SUCCESS)
    {
        BDBG_ERR(("BSID_SaveOutputImageAsYuyv: BMEM_ConvertAddressToOffset returned with error 0x%x", retCode));
        return;
    }

    fwrite(bufAddr, sizeof(uint8_t), output_buffer_pitch * output_buffer_height, pFile);

    fclose(pFile);

    return;
}

void BSID_SaveCdb(
    BSID_ChannelHandle hSidCh,
    uint32_t input_buffer_offset[2],
    uint32_t input_buffer_size[2],
    uint32_t num_input_buffer,
    char *file_path)
{
    BERR_Code retCode = BERR_SUCCESS;
    FILE *pFile;
    void *bufAddr = NULL;
    unsigned int i = 0;

    /* save input */
    pFile = fopen(file_path, "wb");
    if (pFile == NULL)
    {
        printf("failed to open pOutBuf file\n");
        return;
    }

    for (i = 0; i < num_input_buffer; i++)
    {
        retCode = BMEM_ConvertOffsetToAddress(hSidCh->hChHeap, input_buffer_offset[i], &bufAddr);
        if (retCode != BERR_SUCCESS)
        {
            BDBG_ERR(("BSID_SaveImage: BMEM_ConvertAddressToOffset returned with error 0x%x", retCode));
            return;
        }

        fwrite(bufAddr, sizeof(uint8_t), input_buffer_size[i], pFile);
    }

    fclose(pFile);

    return;
}

void BSID_SaveImageData(
    BSID_ChannelHandle  hSidCh,
    uint32_t image_index,
    uint32_t output_buffer_offset,
    uint32_t output_buffer_width,
    uint32_t output_buffer_pitch,
    uint32_t output_buffer_height,
    uint32_t input_buffer_offset[2],
    uint32_t input_buffer_size[2],
    uint32_t num_input_buffer)
{
    char path[] ="/public/fbasso/mjpeg/decoded/";
    char file_path[256]="";
    char out_name[100]="out";
    char cdb_name[100]="cdb";

#if 0
    sprintf(out_name, "%s.%d.yuyv", out_name, image_index);
#else
    sprintf(out_name, "%s.%d.ppm", out_name, image_index);
#endif
    strcat(file_path, path);
    strcat(file_path, out_name);

    /* save output */
#if 0
    BSID_SaveOutputImageAsYuyv(
        hSidCh,
        output_buffer_offset,
        output_buffer_width,
        output_buffer_pitch,
        output_buffer_height,
        file_path);
#else
    BSID_SaveOutputImageAsPpm(
        hSidCh,
        output_buffer_offset,
        output_buffer_width,
        output_buffer_pitch,
        output_buffer_height,
        file_path);
#endif

    strcpy(file_path, path);
    sprintf(cdb_name, "%s.%d.jpg", cdb_name, image_index);
    strcat(file_path, cdb_name);

    BSID_SaveCdb(
        hSidCh,
        input_buffer_offset,
        input_buffer_size,
        num_input_buffer,
        file_path);

    return;
}
#endif /* BSID_SAVE_BUFFER*/



