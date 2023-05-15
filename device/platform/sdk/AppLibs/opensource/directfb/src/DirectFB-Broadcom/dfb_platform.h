/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: dfb_platform.h $
 * $brcm_Revision: DirectFB_1_4_15_Port/8 $
 * $brcm_Date: 3/7/12 5:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/dfb_platform.h $
 *
 * DirectFB_1_4_15_Port/8   3/7/12 5:25p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 *
 * DirectFB_1_4_15_Port/7   3/1/12 7:53p christ
 * SW7420-2273: DirectFB-1.4.15: Use correct Nexus heap for secondary
 *  display frame buffers
 *
 * DirectFB_1_4_15_Port/6   2/24/12 10:51p robertwm
 * SW7420-2262: DirectFB-1.4.15: DFB_Platform_GetStatus() does not return
 *  correct "master/slave" status.
 *
 * DirectFB_1_4_15_Port/5   11/15/11 1:51a christ
 * SW7346-556: DirectFB: Display fails to open on 7346 B1 using Phase 4.5
 *  refsw
 *
 * DirectFB_1_4_15_Port/4   10/5/11 12:09p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 * SW7420-2065: DirectFB: Need DFB Platform Version Macros for external
 *  applications to query.
 *
 * DirectFB_1_4_14_Port/3   9/23/11 12:54p christ
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file
 *
 * DirectFB_1_4_14_Port/2   8/19/11 5:04p christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/7   9/22/11 4:31p robertwm
 * SW7420-2065: DirectFB: Need DFB Platform Version Macros for external
 *  applications to query.
 *
 * DirectFB_1_4_5_Port/6   9/19/11 3:09p robertwm
 * SW7420-2065: DirectFB: Need DFB Platform Version Macros for external
 *  applications to query.
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 *
 * DirectFB_1_4_5_Port/5   9/13/11 4:02p robertwm
 * SW7420-2058: DirectFB: Change DFB Platform code to not rely on
 *  directfb.h header file.
 *
 * DirectFB_1_4_5_Port/4   8/15/11 12:44p robertwm
 * SW7420-2033: DirectFB: need to expose Nexus VideoWindow handle in dfb
 *  platform code.
 *
 * DirectFB_1_4_14_Port/1   7/28/11 11:24a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 *
 * DirectFB_1_4_5_Port/3   7/8/11 4:51p robertwm
 * SW7630-105: DirectFB-1.4.5: dfb_platform.h has incorrect init settings
 *  for BCC.
 *
 * DirectFB_1_4_5_Port/2   7/7/11 6:43p tross
 * SWBLURAY-26478: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_5_Port/1   6/14/11 5:12p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/2   7/1/11 2:47p tross
 * SWBLURAY-26422: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/2   7/1/11 2:46p tross
 * SWBLURAY-26422: Merge in Bluray libinit support.
 *
 * DirectFB_1_4_Port/DirectFB_1_4_1/1   6/10/11 5:08p christ
 * SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
 *  display.
 *
 * DirectFB_1_4_Port/4   6/2/11 4:29p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/3   5/17/11 6:47p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/2   5/13/11 10:54p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 * DirectFB_1_4_Port/1   2/4/11 8:37p robertwm
 * SW7400-3000: DirectFB: Support non-DFB master application in
 *  platform_init code.
 * SW7400-3002: DirectFB: Support non-DFB master application in
 *  platform_init code.
 *
 *
 *****************************************************************************/
#ifndef __DFB_PLATFORM_H__
#define __DFB_PLATFORM_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__cplusplus) && !defined(__bool_true_false_are_defined)
   typedef uint8_t bool;
#  ifndef false
#   define false (0)
#  endif
#  ifndef true
#   define true (!false)
#  endif
#endif

/**
Macros for the DirectFB Platform version
**/
#define DFB_PLATFORM_P_VERSION_MAJ_SHIFT    8
#define DFB_PLATFORM_P_VERSION_MAJ_MASK     (0xff << DFB_PLATFORM_P_VERSION_MAJ_SHIFT)
#define DFB_PLATFORM_P_VERSION_MIN_SHIFT    0
#define DFB_PLATFORM_P_VERSION_MIN_MASK     (0xff << DFB_PLATFORM_P_VERSION_MIN_SHIFT)

#define DFB_PLATFORM_MAKE_VERSION(maj,min)  (((maj) << DFB_PLATFORM_P_VERSION_MAJ_SHIFT) + ((min) << DFB_PLATFORM_P_VERSION_MIN_SHIFT))
#define DFB_PLATFORM_GET_VERSION()          DFB_PLATFORM_VERSION

#define DFB_PLATFORM_P_GET_STRUCT_SIZE()    (sizeof(DFB_PlatformSettings))

/**
Summary:
Define the Broadcom DirectFB Platform version

Description:
Minor number change means API has changed, but is backwardly compatible with existing
versions of the API of the same major number.  A major number change means that the
API is significantly different and is not compatible with previous versions of the
platform API.
**/
#define DFB_PLATFORM_VERSION_MAJOR          6
#define DFB_PLATFORM_VERSION_MINOR          1

#define DFB_PLATFORM_VERSION                DFB_PLATFORM_MAKE_VERSION(DFB_PLATFORM_VERSION_MAJOR, DFB_PLATFORM_VERSION_MINOR)

/**
Summary:
This defines the maximum number of displays that DirectFB
can handle currently.
**/
#define DFB_PLATFORM_NUM_DISPLAYS_MAX       2

/**
Summary:
This defines the maximum number of display windows that DirectFB
can handle currently.
**/
#define DFB_PLATFORM_NUM_DISPLAY_WINDOWS_MAX         4

/**
Summary:
This defines the maximum number of display outputs that DirectFB
can handle currently.
**/
#define DFB_PLATFORM_NUM_DISPLAY_OUTPUTS_MAX        16

/**
Summary:
This defines the maximum number of heaps that DirectFB
can handle currently.
**/
#define DFB_PLATFORM_MAX_HEAPS              8

/**
Summary:
DFB_PLATFORM_MEMORY_TYPE macros are used to control memory mapping.

Description:
Memory mapping requirements depend on the type of access in the driver and application (for kernel mode,
the driver is in the kernel and the application is in user space).
**/
#define DFB_PLATFORM_MEMORY_TYPE_DRIVER_UNCACHED        0x0001
#define DFB_PLATFORM_MEMORY_TYPE_DRIVER_CACHED          0x0002
#define DFB_PLATFORM_MEMORY_TYPE_APPLICATION_CACHED     0x0004
#define DFB_PLATFORM_MEMORY_TYPE_SECURE                 0x0008

/**
Summary:
DFB_PlatformMemoryType macros are used to assemble commonly used DFB_PLATFORM_MEMORY_TYPE combinations.

Description:
Cached and uncached CPU access from the driver only.
Note that some driver code may only require cached or only required uncached.
The minimal memory mapping is preferred if it is known.
**/
#define DFB_PlatformMemoryType_eDriver                  (DFB_PLATFORM_MEMORY_TYPE_DRIVER_UNCACHED | \
                                                         DFB_PLATFORM_MEMORY_TYPE_DRIVER_CACHED)

/**
Summary:
DFB_PlatformMemoryType macros are used to assemble commonly used DFB_PLATFORM_MEMORY_TYPE combinations.

Description:
Cached CPU access in application (user space). No CPU access in the driver. No uncached access in application.
Can be used for graphics surfaces, record buffers, etc.
HW access is still allowed with an eApplication heap. For instance, you can allocate a graphics framebuffer from
an eApplication heap because only the app needs CPU access; the driver simply needs to program the offset to the GFD.
**/
#define DFB_PlatformMemoryType_eApplication             (DFB_PLATFORM_MEMORY_TYPE_APPLICATION_CACHED)

/**
Summary:
DFB_PlatformMemoryType macros are used to assemble commonly used DFB_PLATFORM_MEMORY_TYPE combinations.

Description:
Full CPU access from driver and application.
**/
#define DFB_PlatformMemoryType_eFull                    (DFB_PLATFORM_MEMORY_TYPE_DRIVER_UNCACHED | \
                                                         DFB_PLATFORM_MEMORY_TYPE_DRIVER_CACHED   | \
                                                         DFB_PLATFORM_MEMORY_TYPE_APPLICATION_CACHED)
/**
Summary:
DFB_PlatformMemoryType macros are used to assemble commonly used DFB_PLATFORM_MEMORY_TYPE combinations.

Description:
Accessible by HW devices only. No CPU access in driver or application; therefore no memory
mapping required.
**/
#define DFB_PlatformMemoryType_eDeviceOnly              (0x0)

/**
Summary:
DFB_PlatformMemoryType macros are used to assemble commonly used DFB_PLATFORM_MEMORY_TYPE combinations.

Description:
Restricted access heap for secure processor. Limited device access and no CPU access.
**/
#define DFB_PlatformMemoryType_eSecure                  (DFB_PLATFORM_MEMORY_TYPE_SECURE)

/**
Summary:
DFB_PlatformMemoryType is used to determine the type of heap memory to configure
during the initialisation stage.

Description:
See the macros above for the different types of memory.
**/
typedef unsigned DFB_PlatformMemoryType;

/**
Summary:
Return code for most DFB_Platform_xxx() functions.

Description:
Actual enums are defined below and are NOT the same as the
DirectFB error codes.
**/
typedef enum {
     DFB_PLATFORM_OK              = 0x00000000,  /* No error occured. */
     DFB_PLATFORM_VERSIONMISMATCH,               /* Some versions didn't match. */
     DFB_PLATFORM_FAILURE,                       /* A general or unknown error occured. */
     DFB_PLATFORM_INIT,                          /* A general initialization error occured. */
     DFB_PLATFORM_UNSUPPORTED,                   /* The requested operation or an argument is (currently) not supported. */
     DFB_PLATFORM_UNIMPLEMENTED,                 /* The requested operation is not implemented, yet. */
     DFB_PLATFORM_ACCESSDENIED,                  /* Access to the resource is denied. */
     DFB_PLATFORM_INVARG,                        /* An invalid argument has been specified. */
     DFB_PLATFORM_NOSYSTEMMEMORY,                /* There's not enough system memory. */
     DFB_PLATFORM_NOSHAREDMEMORY,                /* There's not enough shared memory. */
     DFB_PLATFORM_LOCKED,                        /* The resource is (already) locked. */
     DFB_PLATFORM_BUFFEREMPTY,                   /* The buffer is empty. */
     DFB_PLATFORM_FILENOTFOUND,                  /* The specified file has not been found. */
     DFB_PLATFORM_IO,                            /* A general I/O error occured. */
     DFB_PLATFORM_BUSY,                          /* The resource or device is busy. */
     DFB_PLATFORM_TIMEOUT,                       /* The operation timed out. */
     DFB_PLATFORM_EOF,                           /* Reached end of file. */
     DFB_PLATFORM_IDNOTFOUND,                    /* No resource has been found by the specified id. */
     DFB_PLATFORM_DESTROYED,                     /* The underlying object (e.g. a window or surface) has been destroyed. */
     DFB_PLATFORM_BUFFERTOOLARGE,                /* Buffer is too large. */
     DFB_PLATFORM_INTERRUPTED,                   /* The operation has been interrupted. */
     DFB_PLATFORM_LIMITEXCEEDED,                 /* Attempted to exceed limit, i.e. any kind of maximum size, count etc. */
} DFB_PlatformResult;

/**
Summary:
DFB_PlatformNexusHandle is the opaque handle that is used to reference
different Nexus handles.  It should never be dereferenced.

Description:
Each Nexus handle that DirectFB needs is declared to be of this
type.  This is to ensure binary compatibility between the application
and the Broadcom DirectFB Platform layer.  The application needs to
typecast to the appropriate type.
**/
typedef void *DFB_PlatformNexusHandle;

/**
Summary:
DFB_PlatformClientType is used to specify the type of client
that is using the DFB Platform API's and whether Nexus has already been
initialised/joined or not.

Description:
A "master" client is normally the first application in the system to be
executed.  All other applications are known as "slave" clients.

The "master" client application can use the "DFB_Platform_Init()"
function to initialise the platform.  In this case, the client type is
known as "DFB_PlatformClientType_eMasterNexusUninitialized".

A "master" client application can also call "NEXUS_Platform_Init()" first
before making any "DFB_Platform_xxx()" API calls.  In this case, the
client type is known as "DFB_PlatformClientType_eMasterNexusInitialized".

A "slave" client application can use the "DFB_Platform_Init()" function
to initialise the platform.  In this case, the client type is known as
"DFB_PlatformClientType_eSlaveNexusUninitialized".

A "slave" client application is also free to call "NEXUS_Platform_Join()"
first before it makes any "DFB_Platform_xxx()" calls.  In this case
the client type is known as "DFB_PlatformClientType_eSlaveNexusInitialized".
**/
typedef enum DFB_PlatformClientType
{
    DFB_PlatformClientType_eMasterNexusUninitialized = 0,
    DFB_PlatformClientType_eMasterNexusInitialized,
    DFB_PlatformClientType_eSlaveNexusUninitialized,
    DFB_PlatformClientType_eSlaveNexusInitialized,
    DFB_PlatformClientType_eMax

} DFB_PlatformClientType;

/**
Summary:
DFB_PlatformDisplayOutputType is used to specify the type
of output connected to a display.

Description:
The output types are listed below.
**/
typedef enum DFB_PlatformDisplayOutputType
{
    DFB_PlatformDisplayOutputType_eHdmi = 1,               /* HDMI output connector type */
    DFB_PlatformDisplayOutputType_eComponent,              /* Component output connector type */
    DFB_PlatformDisplayOutputType_eComposite,              /* CVBS output connector type */
    DFB_PlatformDisplayOutputType_eSvideo,                 /* SVHS output connector type */
    DFB_PlatformDisplayOutputType_eScart,                  /* SCART output connector type */
    DFB_PlatformDisplayOutputType_eRfm,                    /* RF modulator output connector type */
    DFB_PlatformDisplayOutputType_eMax,                    /* Total number of output types */

} DFB_PlatformDisplayOutputType;

/**
Summary:
DFB_PlatformDisplayType is used when opening a Display
**/
typedef enum DFB_PlatformDisplayType
{
    DFB_PlatformDisplayType_eAuto = 0,              /* Configure display for VEC output. The VEC output is used to drive analog outputs like
                                                       component, composite, svideo as well as HDMI output.
                                                       This is called eAuto because the index of the VEC (e.g. primary or secondary VEC) is
                                                       automatically chosen based on internal configuration. */
    DFB_PlatformDisplayType_eLvds,                  /* Configure display for LVDS output by means of the DVO port.
                                                       This is used to drive an LCD/PDP panel directly.
                                                       This is only valid for some chips and Display indexes. */
    DFB_PlatformDisplayType_eDvo,                   /* Configure display for DVO output.
                                                       This is used to drive an LCD/PDP panel with an external LVDS daughter card.
                                                       This is only valid for some chips and Display indexes. */
    DFB_PlatformDisplayType_eBypass,                /* Configure display for bypass output to CCIR656 or composite output. No graphics. */
    DFB_PlatformDisplayType_eMax

} DFB_PlatformDisplayType;

/*
 * A dimension specified by width and height.
 */
typedef struct {
     int            w;   /* width of it */
     int            h;   /* height of it */
} DFB_PlatformDimension;

/*
Summary:
DFB Video formats

Description:
The video formats that the DirectFB Platform
code understands is listed below.  Note, that
these do not necessarily have a 1:1 relationship
with the equivalent Nexus video formats.
*/
typedef enum DFB_PlatformVideoFormat
{
    DFB_PlatformVideoFormat_eUnknown = 0,           /* unknown/not supported video format */
    DFB_PlatformVideoFormat_eNtsc,                  /* NTSC Interlaced */
    DFB_PlatformVideoFormat_eNtsc443,               /* NTSC encoding with the PAL color carrier frequency. */
    DFB_PlatformVideoFormat_eNtscJapan,             /* Japan NTSC, no pedestal level */
    DFB_PlatformVideoFormat_ePalM,                  /* PAL Brazil */
    DFB_PlatformVideoFormat_ePalN,                  /* PAL_N */
    DFB_PlatformVideoFormat_ePalNc,                 /* PAL_N, Argentina */
    DFB_PlatformVideoFormat_ePalB,                  /* Australia */
    DFB_PlatformVideoFormat_ePalB1,                 /* Hungary */
    DFB_PlatformVideoFormat_ePalD,                  /* China */
    DFB_PlatformVideoFormat_ePalD1,                 /* Poland */
    DFB_PlatformVideoFormat_ePalDK1,                /* Eastern Europe */
    DFB_PlatformVideoFormat_ePalDK2,                /* Eastern Europe */
    DFB_PlatformVideoFormat_ePalDK3,                /* Eastern Europe */
    DFB_PlatformVideoFormat_ePalG,                  /* PAL Europe */
    DFB_PlatformVideoFormat_ePalH,                  /* Europe */
    DFB_PlatformVideoFormat_ePalK,                  /* Europe */
    DFB_PlatformVideoFormat_ePalI,                  /* U.K. */
    DFB_PlatformVideoFormat_ePal60hz,               /* 60Hz PAL */
    DFB_PlatformVideoFormat_eSecamL,                /* France */
    DFB_PlatformVideoFormat_eSecamB,                /* Middle East */
    DFB_PlatformVideoFormat_eSecamG,                /* Middle East */
    DFB_PlatformVideoFormat_eSecamD,                /* Eastern Europe */
    DFB_PlatformVideoFormat_eSecamK,                /* Eastern Europe */
    DFB_PlatformVideoFormat_eSecamH,                /* Line SECAM */
    DFB_PlatformVideoFormat_e480p,                  /* NTSC Progressive (27Mhz) */
    DFB_PlatformVideoFormat_e576p,                  /* HD PAL Progressive 50hz for Australia */
    DFB_PlatformVideoFormat_e1080i,                 /* HD 1080 Interlaced */
    DFB_PlatformVideoFormat_e1080i50hz,             /* European 50hz HD 1080 */
    DFB_PlatformVideoFormat_e1080p24hz,             /* HD 1080 Progressive, 24hz */
    DFB_PlatformVideoFormat_e1080p25hz,             /* HD 1080 Progressive, 25hz */
    DFB_PlatformVideoFormat_e1080p30hz,             /* HD 1080 Progressive, 30hz */
    DFB_PlatformVideoFormat_e1080p50hz,             /* HD 1080 Progressive, 50hz. */
    DFB_PlatformVideoFormat_e1080p60hz,             /* HD 1080 Progressive, 60hz */
    DFB_PlatformVideoFormat_e1250i50hz,             /* HD 1250 Interlaced, 50hz */
    DFB_PlatformVideoFormat_e720p,                  /* HD 720 Progressive */
    DFB_PlatformVideoFormat_e720p50hz,              /* HD 720p 50hz for Australia */
    DFB_PlatformVideoFormat_e720p24hz,              /* HD 720p 24hz */
    DFB_PlatformVideoFormat_e720p25hz,              /* HD 720p 25hz */
    DFB_PlatformVideoFormat_e720p30hz,              /* HD 720p 30hz */
    DFB_PlatformVideoFormat_e240p60hz,              /* NTSC 240p */
    DFB_PlatformVideoFormat_e288p50hz,              /* PAL 288p */
    DFB_PlatformVideoFormat_e1440x480p60hz,         /* CEA861B */
    DFB_PlatformVideoFormat_e1440x576p50hz,         /* CEA861B */
    DFB_PlatformVideoFormat_eCustomer1440x240p60hz, /* 240p 60hz 7411 custom format. */
    DFB_PlatformVideoFormat_eCustomer1440x288p50hz, /* 288p 50hz 7411 custom format. */
    DFB_PlatformVideoFormat_eCustomer1366x768p60hz, /* Custom 1366x768 mode */
    DFB_PlatformVideoFormat_eCustomer1366x768p50hz, /* Custom 1366x768 50hz mode */
    DFB_PlatformVideoFormat_eVesa640x480p60hz,      /* DVI Safe mode for computer monitors */

    /* DVI VESA mode for computer monitors */
    DFB_PlatformVideoFormat_eVesa800x600p60hz,
    DFB_PlatformVideoFormat_eVesa1024x768p60hz,
    DFB_PlatformVideoFormat_eVesa1280x768p60hz,
    DFB_PlatformVideoFormat_eVesa1280x768p60hzRed,
    DFB_PlatformVideoFormat_eVesa1280x720p50hz,
    DFB_PlatformVideoFormat_eVesa1280x720p60hz,
    DFB_PlatformVideoFormat_eVesa1280x720pReducedBlank,
    DFB_PlatformVideoFormat_eVesa640x350p60hz,
    DFB_PlatformVideoFormat_eVesa640x350p70hz,
    DFB_PlatformVideoFormat_eVesa640x350p72hz,
    DFB_PlatformVideoFormat_eVesa640x350p75hz,
    DFB_PlatformVideoFormat_eVesa640x350p85hz,
    DFB_PlatformVideoFormat_eVesa640x400p60hz,
    DFB_PlatformVideoFormat_eVesa640x400p70hz,
    DFB_PlatformVideoFormat_eVesa640x400p72hz,
    DFB_PlatformVideoFormat_eVesa640x400p75hz,
    DFB_PlatformVideoFormat_eVesa640x400p85hz,
    DFB_PlatformVideoFormat_eVesa640x480p66hz,
    DFB_PlatformVideoFormat_eVesa640x480p70hz,
    DFB_PlatformVideoFormat_eVesa640x480p72hz,
    DFB_PlatformVideoFormat_eVesa640x480p75hz,
    DFB_PlatformVideoFormat_eVesa640x480p85hz,
    DFB_PlatformVideoFormat_eVesa720x400p60hz,
    DFB_PlatformVideoFormat_eVesa720x400p70hz,
    DFB_PlatformVideoFormat_eVesa720x400p72hz,
    DFB_PlatformVideoFormat_eVesa720x400p75hz,
    DFB_PlatformVideoFormat_eVesa720x400p85hz,
    DFB_PlatformVideoFormat_eVesa800x600p56hz,
    DFB_PlatformVideoFormat_eVesa800x600p59hzRed,
    DFB_PlatformVideoFormat_eVesa800x600p70hz,
    DFB_PlatformVideoFormat_eVesa800x600p72hz,
    DFB_PlatformVideoFormat_eVesa800x600p75hz,
    DFB_PlatformVideoFormat_eVesa800x600p85hz,
    DFB_PlatformVideoFormat_eVesa848x480p60hz,
    DFB_PlatformVideoFormat_eVesa1024x768p66hz,
    DFB_PlatformVideoFormat_eVesa1024x768p70hz,
    DFB_PlatformVideoFormat_eVesa1024x768p72hz,
    DFB_PlatformVideoFormat_eVesa1024x768p75hz,
    DFB_PlatformVideoFormat_eVesa1024x768p85hz,
    DFB_PlatformVideoFormat_eVesa1064x600p60hz,
    DFB_PlatformVideoFormat_eVesa1280x720p70hz,
    DFB_PlatformVideoFormat_eVesa1280x720p72hz,
    DFB_PlatformVideoFormat_eVesa1280x720p75hz,
    DFB_PlatformVideoFormat_eVesa1280x720p85hz,
    DFB_PlatformVideoFormat_eVesa1024x768i87hz,
    DFB_PlatformVideoFormat_eVesa1152x864p75hz,
    DFB_PlatformVideoFormat_eVesa1280x768p75hz,
    DFB_PlatformVideoFormat_eVesa1280x768p85hz,
    DFB_PlatformVideoFormat_eVesa1280x800p_60Hz,
    DFB_PlatformVideoFormat_eVesa1280x960p60hz,
    DFB_PlatformVideoFormat_eVesa1280x960p85hz,
    DFB_PlatformVideoFormat_eVesa1280x1024p60hz,
    DFB_PlatformVideoFormat_eVesa1280x1024p69hz,
    DFB_PlatformVideoFormat_eVesa1280x1024p75hz,
    DFB_PlatformVideoFormat_eVesa1280x1024p85hz,
    DFB_PlatformVideoFormat_eVesa832x624p75hz,
    DFB_PlatformVideoFormat_eVesa1360x768p60hz,
    DFB_PlatformVideoFormat_eVesa1366x768p60hz,
    DFB_PlatformVideoFormat_eVesa1400x1050p60hz,
    DFB_PlatformVideoFormat_eVesa1400x1050p60hzReducedBlank,
    DFB_PlatformVideoFormat_eVesa1400x1050p75hz,
    DFB_PlatformVideoFormat_eVesa1440x900p60hz,
    DFB_PlatformVideoFormat_eVesa1600x1200p60hz,
    DFB_PlatformVideoFormat_eVesa1920x1080p60hzReducedBlank,
    DFB_PlatformVideoFormat_eVesa1680x1050p_60Hz,
    DFB_PlatformVideoFormat_eVesa1280x800p_60Hz_Red,
    DFB_PlatformVideoFormat_eVesa1600x1200p_75Hz,
    DFB_PlatformVideoFormat_eVesa1600x900p_60Hz_Red,
    DFB_PlatformVideoFormat_eVesa1680x1050p_60Hz_Red,
    DFB_PlatformVideoFormat_eVesa1920x1200p_60Hz,
    DFB_PlatformVideoFormat_eVesa1920x1200p_60Hz_Red,
    DFB_PlatformVideoFormat_eVesa1152x720p_60Hz,
    DFB_PlatformVideoFormat_eVesa1152x720p_75Hz,
    DFB_PlatformVideoFormat_eVesa1152x720p_85Hz,
    DFB_PlatformVideoFormat_eVesa1152x864p_60Hz,
    DFB_PlatformVideoFormat_eVesa1152x864p_85Hz,
    DFB_PlatformVideoFormat_eVesa1152x870p_75Hz,
    DFB_PlatformVideoFormat_eVesa1152x900p_66Hz,
    DFB_PlatformVideoFormat_eVesa1152x900p_76Hz,
    DFB_PlatformVideoFormat_eVesa1170x584p_50Hz,
    DFB_PlatformVideoFormat_eVesa1280x1024p_70Hz,
    DFB_PlatformVideoFormat_eVesa1280x1024p_72Hz,
    DFB_PlatformVideoFormat_eVesa1280x1024p_76Hz,
    DFB_PlatformVideoFormat_eVesa1280x768p_50Hz,
    DFB_PlatformVideoFormat_eVesa1280x960p_75Hz,
    DFB_PlatformVideoFormat_eVesa1600x1024p_60Hz,
    DFB_PlatformVideoFormat_eVesa1600x1024p_76Hz,
    DFB_PlatformVideoFormat_eVesa1728x1080p_60Hz,
    DFB_PlatformVideoFormat_eVesa800x600p_100Hz,
    DFB_PlatformVideoFormat_eVesa800x600p_90Hz,
    DFB_PlatformVideoFormat_eVesa848x480p_75Hz,
    DFB_PlatformVideoFormat_eVesa848x480p_85Hz,
    DFB_PlatformVideoFormat_eVesa852x480p_60Hz,

    DFB_PlatformVideoFormat_e720p_3DOU_AS,          /* HD 3D 720p */
    DFB_PlatformVideoFormat_e720p50hz_3DOU_AS,      /* HD 3D 720p50 */
    DFB_PlatformVideoFormat_e720p30hz_3DOU_AS,      /* HD 3D 720p30 */
    DFB_PlatformVideoFormat_e720p24hz_3DOU_AS,      /* HD 3D 720p24 */
    DFB_PlatformVideoFormat_e1080p24hz_3DOU_AS,     /* HD 1080p 24Hz, 2750 sample per line, SMPTE 274M-1998 */
    DFB_PlatformVideoFormat_e1080p30hz_3DOU_AS,     /* HD 1080p 30Hz, 2200 sample per line, SMPTE 274M-1998 */

    /* dynamics: custom format (see Nexus for more detail) */
    DFB_PlatformVideoFormat_eCustom0,
    DFB_PlatformVideoFormat_eCustom1,
    DFB_PlatformVideoFormat_eCustom2,

    /* 3D source & display formats. These are all full-res O/U 3D formats. Half-res does not require a special 3D format. */
    DFB_PlatformVideoFormat_eCustom1920x2160i_48Hz, /* 3DTV panel format */

    /* Legacy 3D timing and display formats */
    DFB_PlatformVideoFormat_eCustom_3D_720p,        /* HD 3D 720p with custom timing */
    DFB_PlatformVideoFormat_eCustom_3D_720p_50hz,   /* HD 3D 720p50hz with custom timing */
    DFB_PlatformVideoFormat_eCustom_3D_720p_30hz,   /* HD 3D 720p30hz with custom timing */
    DFB_PlatformVideoFormat_eCustom_3D_720p_24hz,   /* HD 3D 720p24hz with custom timing */
    DFB_PlatformVideoFormat_eCustom_3D_1080p_24hz,  /* HD 3D 1080p24hz with custom timing */
    DFB_PlatformVideoFormat_eCustom_3D_1080p_30hz,  /* HD 3D 1080p30hz with custom timing */

    DFB_PlatformVideoFormat_eMax                    /* Total number of video formats */

} DFB_PlatformVideoFormat;

/**
Summary:
DFB Video aspect ratio

Description:
**/
typedef enum DFB_PlatformDisplayAspectRatio
{
    DFB_PlatformDisplayAspectRatio_eAuto, /* 4x3 for SD and 480p, 16x9 for HD (including 720p, 1080i, etc.) */
    DFB_PlatformDisplayAspectRatio_e4x3,
    DFB_PlatformDisplayAspectRatio_e16x9,
    DFB_PlatformDisplayAspectRatio_eMax

} DFB_PlatformDisplayAspectRatio;


/**
Summary:
DFB Stereoscopic buffer modes.

Description:
The enums are defined below to be the same as the part of the HDMI 1.4 specification we support in DirectFB
and describe how the data is stored inside the buffer.
**/
typedef enum DFB_PlatformStereoscopicMode
{
    DFB_PlatformStereoscopicMode_e2d = 0,
    DFB_PlatformStereoscopicMode_eSideBySideHalf,
    DFB_PlatformStereoscopicMode_eTopBottom,
    DFB_PlatformStereoscopicMode_eFramePacking,
    DFB_PlatformStereoscopicMode_eSideBySideFull,
    DFB_PlatformStereoscopicMode_eMax

} DFB_PlatformStereoscopicMode;

/**
Summary:
A Timebase provides a clock for various cores like decoders, display and mixers.

Description:
The enums are defined below to be the same as Nexus.
**/
typedef enum DFB_PlatformTimebase
{
    DFB_PlatformTimebase_eInvalid = 0,
    DFB_PlatformTimebase_e0,
    DFB_PlatformTimebase_e1,
    DFB_PlatformTimebase_e2,
    DFB_PlatformTimebase_e3,
    DFB_PlatformTimebase_e4,
    DFB_PlatformTimebase_e5,
    DFB_PlatformTimebase_e6,
    DFB_PlatformTimebase_e7,
    DFB_PlatformTimebase_eMax

} DFB_PlatformTimebase;


/**
Summary:
DFB_PlatformColor defines the individual alpha, red, green and blue components of a color.

Description:
A color defined by channels with 8bit each.
**/
typedef struct {
     uint8_t a;   /* alpha channel */
     uint8_t r;   /* red channel */
     uint8_t g;   /* green channel */
     uint8_t b;   /* blue channel */
} DFB_PlatformColor;

/**
Summary:
DFB_PlatformDisplayInitSettings is used to open/initialise a
display correctly.

Description:
The actual structure is defined below.
**/
typedef struct DFB_PlatformDisplayInitSettings
{
    DFB_PlatformDisplayType         displayType;    /* The type of display to open. Cannot be changed after initialisation. */
    int                             vecIndex;       /* If displayType is DFB_PlatformDisplayType_eAuto, this selects the VEC for this display.
                                                       The default value is -1, which allows Nexus to select the mapping.
                                                       Nexus will use the NEXUS_DisplayModuleSettings.vecSwap setting only if vecIndex is -1. */
    DFB_PlatformVideoFormat         format;         /* Output format of video display */
    DFB_PlatformDisplayAspectRatio  aspectRatio;    /* Aspect ratio of display, if applicable */
    DFB_PlatformTimebase            timebase;       /* Timebase that will drive the outputs */
    DFB_PlatformColor               background;     /* Background color in DFB DSPF_ARGB colorspace */
    bool                            xvYccEnabled;   /* If true, Nexus will check the HdmiInput for xvYCC colorspace and, if detected,
                                                       will output xvYcc to an HdmiOutput or PanelOutput. */
} DFB_PlatformDisplayInitSettings;

/**
Summary:
DFB_PlatformDisplayOutputInitSettings is used to initialise an
output connector correctly.

Description:
The actual structure is defined below.
**/
typedef struct DFB_PlatformDisplayOutputInitSettings
{
    DFB_PlatformDisplayOutputType outputType;       /* Specify output connector type */
    int                           outputIndex;      /* Specify which output connector of a given type */
    int                           displayIndex;     /* Which display to connect this output to  (e.g. 0 = primary) */

} DFB_PlatformDisplayOutputInitSettings;

/**
Summary:
DFB_PlatformDisplayWindowInitSettings is used to initialise a
Window that can be connected to a display.

Description:
The actual structure is defined below.
**/
typedef struct DFB_PlatformDisplayWindowInitSettings
{
    int                           displayIndex;     /* Which display to connect this output to  (e.g. 0 = primary display) */
    int                           windowIndex;      /* Which window of the display to connect   (e.g. 0 = primary window) */

} DFB_PlatformDisplayWindowInitSettings;

/**
Summary:
Settings used for opening graphics2d
**/
typedef struct DFB_PlatformGraphics2DInitSettings
{
    unsigned    maxOperations;                      /* Maximum number of simultaneous asynchronous blits. A larger number will require
                                                       more heap allocation.
                                                       If you are doing async blits, you will need to use NEXUS_Graphics2D_Checkpoint to ensure
                                                       you don't exceed this number, otherwise your blit or fill will fail. */
} DFB_PlatformGraphics2DInitSettings;

/**
Summary:
Settings used for opening graphics3d
**/
typedef struct DFB_PlatformGraphics3DInitSettings
{
    void *reserved;                                 /* Reserved for future use */

} DFB_PlatformGraphics3DInitSettings;

/**
Summary:
Settings used to open a picture decoder.

Description:
Max capabilities must be specified at open time so that memory can be allocated then.
**/
typedef struct DFB_PlatformPictureDecoderInitSettings
{
    unsigned bufferSize;                            /* size of the compressed buffer */
    unsigned multiScanBufferSize;                   /* size of the decoded picture buffer that is required for multi scan decoding */

} DFB_PlatformPictureDecoderInitSettings;

/**
Summary:
DFB_PlatformSettings are used to initialise the Broadcom DirectFB
Platform interface. Some of the  Nexus platform initialisation settings
can be specified here, making the NEXUS_Platform_Init() function
redundant in most cases.

It is still possible for an application to initialise Nexus explicitly
and then use this function to pass in information necessary for DFB
to run correctly (e.g. heaps).

Description:
The actual structure is defined below.
**/

typedef struct DFB_PlatformSettings
{
    bool                                        initOrJoinNexus;          /* Init/Join NEXUS? False usually means that Nexus has already been
                                                                             initialised or joined by the application.
                                                                             True, means that DFB will attempt to initialise or join Nexus. */
    bool                                        allowUnauthenticatedClients; /* Set to true if we want to allow unauthenticated clients during
                                                                                initialisation phase. */
    bool                                        trustUnauthenticatedClients; /* Set to true to allow unauthenticated clients to be trusted. */

    bool                                        slave;                       /* Are we a slave NEXUS App?  (i.e. has another Nexus app has initialised
                                                                                Nexus)? */
    /* Heap related fields. These should be setup if another process/app initialises NEXUS.  NULL means automatically determine heap */
    DFB_PlatformNexusHandle                     displayHeapHandle;         /* Framebuffer surface heap handle */
    DFB_PlatformNexusHandle                     secondaryDisplayHeapHandle;/* Framebuffer surface heap handle for secondary display*/
    DFB_PlatformNexusHandle                     offscreenHeapHandle;       /* Offscreen surface(s) heap handle */
    DFB_PlatformNexusHandle                     paletteHeapHandle;         /* Palette heap handle  */
    DFB_PlatformNexusHandle                     videoHeapHandle;           /* Video heap handle  */
    DFB_PlatformNexusHandle                     pictureDecoderHeapHandle;  /* Picture Decoder heap handle */

    /* Display related fields */
    struct {
        bool                                    init;                     /* Set to "true" to initialise the display module and
                                                                             use the "initSettings" field. If "handle" is NULL, then
                                                                             the display will automatically be opened and initialised
                                                                             with the "initSettings" field.
                                                                             If "handle" is not NULL, then this implies that the display
                                                                             has already been opened externally to DFB and that the
                                                                             "initSettings" field should be used for the initial settings. */

        DFB_PlatformNexusHandle                 handle;                   /* Display handle already obtained by a prior open.
                                                                             If this field is NULL and "init" is "false", then
                                                                             don't attempt to open and initialise the module at all. */
        DFB_PlatformDisplayInitSettings         initSettings;             /* Display settings that are used if "init" is "true". */
    } display[DFB_PLATFORM_NUM_DISPLAYS_MAX];

    /* Display Window related fields */
    struct {
        bool                                    init;                     /* Set to "true" to initialise the display window.  If "handle"
                                                                             is NULL, then the display window will automatically be
                                                                             opened and connected to the display in the "initSettings" field.
                                                                             If "handle" is not NULL, then this implies that the display window
                                                                             has already been opened externally to DFB and that
                                                                             the "initSettings" field should be used for the initial settings.
                                                                             Set to "false" to ignore this display window. */

        DFB_PlatformNexusHandle                 handle;                   /* Abstract display window handle already obtained by a prior Nexus call.
                                                                             If this field is NULL and "init" is false, then don't attempt
                                                                             to open and connect this output to the display. */
        DFB_PlatformDisplayWindowInitSettings  initSettings;              /* Display window initial settings that are used if "init" is "true". */
    } displayWindow[DFB_PLATFORM_NUM_DISPLAY_WINDOWS_MAX];

    /* Display Output connection related fields */
    struct {
        bool                                    init;                     /* Set to "true" to initialise the output connector.  If "handle"
                                                                             is NULL, then the output connector will automatically be
                                                                             opened and connected to the display in the "initSettings" field.
                                                                             If "handle" is not NULL, then this implies that the output
                                                                             connector has already been opened externally to DFB and that
                                                                             the "initSettings" field should be used for the initial settings.
                                                                             Set to "false" to ignore this output connection. */

        DFB_PlatformNexusHandle                 handle;                   /* Abstract Video output connector handle already obtained by a prior Nexus call.
                                                                             If this field is NULL and "init" is false, then don't attempt
                                                                             to open and connect this output to the display. */
        DFB_PlatformDisplayOutputInitSettings   initSettings;             /* Output initial settings that are used if "init" is "true". */
    } displayOutput[DFB_PLATFORM_NUM_DISPLAY_OUTPUTS_MAX];

    /* Graphics2D related fields */
    struct {
        bool                                    init;                     /* Set to "true" to initialise the graphics2d module and
                                                                             use the "initSettings" field. If "handle" is NULL, then
                                                                             the graphics2d module will automatically be opened and
                                                                             initialised with the "initSettings" field.
                                                                             If "handle" is not NULL, then this implies that the
                                                                             graphics2d module has already been opened externally to
                                                                             DFB and that the "initSettings" field should be used for
                                                                             the initial settings. */

        DFB_PlatformNexusHandle                 handle;                   /* 2D graphics handle already obtained by a prior open.
                                                                             If this field is NULL and "init" is "false", then
                                                                             don't attempt to open and initialise the module at all. */
        DFB_PlatformGraphics2DInitSettings      initSettings;             /* Graphics2D settings that are used if "init" is true". */
    } graphics2d;

    /* Graphics3D related fields */
    struct {
        bool                                    init;                     /* Set to "true" to initialise the graphics3d module and
                                                                             use the "initSettings" field. If "handle" is NULL, then
                                                                             the graphics3d module will automatically be opened and
                                                                             initialised with the "initSettings" field.
                                                                             If "handle" is not NULL, then this implies that the
                                                                             graphics3d module has already been opened externally to
                                                                             DFB and that the "initSettings" field should be used for
                                                                             the initial settings. */

        DFB_PlatformNexusHandle                 handle;                   /* 3D graphics handle already obtained by a prior open.
                                                                             If this field is NULL and "init" is "false", then
                                                                             don't attempt to open and initialise the module at all. */
        DFB_PlatformGraphics3DInitSettings      initSettings;             /* Graphics3D settings that are used if "init" is "true". */

    } graphics3d;

    /* Picture Decoder related fields */
    struct {
        bool                                    init;                     /* Set to "true" to initialise the picture decoder module and
                                                                             use the "initSettings" field. If "handle" is NULL, then
                                                                             the picture decoder module will automatically be opened and
                                                                             initialised with the "initSettings" field.
                                                                             If "handle" is not NULL, then this implies that the
                                                                             picture decoder module has already been opened externally to
                                                                             DFB and that the "initSettings" field should be used for
                                                                             the initial settings. */

        DFB_PlatformNexusHandle                 handle;                   /* Still image decoder handle already obtained by a prior open.
                                                                             If this field is NULL and "init" is "false", then
                                                                             don't attempt to open and initialise the module at all. */
        DFB_PlatformPictureDecoderInitSettings  initSettings;             /* Picture decoder settings that are used if "init" is "true". */
    } pictureDecoder;

    /* If "initOrJoinNexus" is set to "true" and "slave" is set to "false", then this means we are a master Nexus application.  In this
       case the following nexus platform initialisation settings are effective. If we are a slave Nexus application, then these
       settings will be ignored. */
    struct {
        /* User-customized heap management.
           This is used for shared analog/digital heap, secure heaps, or creating multiple heaps to contain fragmentation. */
        struct {
            unsigned                            memcIndex;                /* memory controller (memc) index */
            unsigned                            subIndex;                 /* addressing region within the MEMC.
                                                                             Most MEMC's have only one addressing region. */
            int                                 size;                     /* If size is 0 (default), processing of the heap[] array is terminated.
                                                                             If size is -1, all remaining memory on the memc is allocated to that heap.
                                                                             A default heap is created for every memc without an explicit heap. */
            unsigned                            alignment;                /* Minimum alignment of allocations in this heap. */
            bool                                guardBanding;             /* If true, add debug guard bands around each allocation.
                                                                             Only applies to CPU-accessible heaps.
                                                                             Set to false if your heap size calculations are precise with no extra space. */
            DFB_PlatformMemoryType              memoryType;               /* see DFB_PLATFORM_MEMORY_TYPE bitmasks and DFB_PlatformMemoryType above. */
        } heap[DFB_PLATFORM_MAX_HEAPS];

        bool                                    openI2c;                  /* If true, DFB_Platform_Init will open the I2C channels for this platform. */
        bool                                    openFpga;                 /* If true, DFB_Platform_Init will open the FPGA.
                                                                             This is needed for some frontends. */
        bool                                    openFrontend;             /* If true, DFB_Platform_Init will scan the reference board for
                                                                             frontend devices. */
        bool                                    pipPbpEnabled;            /* If true, the system will PIP/PBP */
        bool                                    startPanelPwm;            /* If true, DFB_Platform_Init will start the PWM
                                                                             for driving panel backlight. If false, only
                                                                             initialization will be done for the PWM. */
        unsigned int                            platforSettingsValid;     /* If true, the DFB platform settings are valid and can be used by */
                                                                          /*   the DFB client app */

    } platformSettings;

} DFB_PlatformSettings;

/**
Summary:
DFB_PlatformStatus is used to hold information about the platform status.

Description:
The actual structure is defined below.
**/
typedef struct DFB_PlatformStatus
{
    bool isMaster;                                                        /* Are we a master Nexus process (true) or not? */
    bool initOrJoinNexus;                                                 /* Has Nexus already been initialised/joined in any process? */
    bool isXS;                                                            /* Has platform display layer been compiled to use the Surface Compositor server? */
    bool isNIR;                                                           /* Has platform input layer been compiled to use the Input Router server? */

} DFB_PlatformStatus;

/**
Summary:
DirectFB Display platform settings

Description:
The actual structure is defined below.
**/
typedef struct DFB_PlatformDisplaySettings
{
    DFB_PlatformVideoFormat         format;                               /* Output format of video display */
    DFB_PlatformDisplayAspectRatio  aspectRatio;                          /* Aspect ratio of display, if applicable */
    DFB_PlatformStereoscopicMode    stereoMode;                           /* Stereoscopic display mode */
    DFB_PlatformColor               background;                           /* Background color in DFB DSPF_ARGB colorspace */

} DFB_PlatformDisplaySettings;

/***************************************************************************
 Functions that an external application can call to pass information to DirectFB.
 The DFB_Platform_Init() function is automatically called by the the Broadcom
 DirectFB system driver during the "system_initialize() / system_join()" stages.
 This passes in default values, so if an application requires non-default values
 then it should link with libinit.so and make the appropriate calls directly.
***************************************************************************/

/***************************************************************************
Summary:
Get default settings to pass into DFB_Platform_Init for the type of client.
The client can be a master or slave client.  A master client is the first
ever DFB or Nexus client in the complete system to initialise the platform.
An app that calls NEXUS_Platform_Init() directly/indirectly, is a "master"
client.  All other Nexus or DFB apps would be classed as "slave" clients.

Description:
Do not call DFB_Platform_GetDefaultSettings_tagged directly. Instead, call
DFB_Platform_GetDefaultSettings(). This will perform basic version checking
to make sure you have a properly configured system.
***************************************************************************/
void DFB_Platform_GetDefaultSettings_tagged(
    DFB_PlatformClientType clientType,      /* Indicates whether we are a master or slave nexus client */
    DFB_PlatformSettings  *pSettings,       /* [out] */
    unsigned               versionCheck,    /* set by macro */
    unsigned               structSizeCheck  /* set by macro */
    );

#define DFB_Platform_GetDefaultSettings(CLIENT_TYPE, PSETTINGS) \
    DFB_Platform_GetDefaultSettings_tagged((CLIENT_TYPE), (PSETTINGS), DFB_PLATFORM_GET_VERSION(), DFB_PLATFORM_P_GET_STRUCT_SIZE())

/***************************************************************************
Summary:
Get settings that were passed into DFB_Platform_Init() along with modifications
made by Nexus/DirectFB. Be aware this is a different meaning of GetSettings
than the standard definition.

Description:
For some settings, 0 means that Nexus/DirectFB should select a default.
DFB_Platform_GetSettings will not return that 0 but will return the default
that was chosen.  This function can be called before DFB_Platform_Init()
has been called in the current process.  However, DFB_Platform_Init() must
have been called previously (in any other process) for it to succeed.

Returns:
DFB_OK if arguments were valid and DFB platform is initialised.
DFB_INIT if DFB platform code is not initialised.
DFB_INVARG if pSettings is NULL.
***************************************************************************/
DFB_PlatformResult DFB_Platform_GetSettings(
    DFB_PlatformSettings *pSettings         /* [out] */
    );

/***************************************************************************
Summary:
Get the display window handle for the provided display and index.
If it is NULL, then this means that the window is NOT opened.

Description:
This will only provide what display window(s) DFB knows that
is associated with a display.  If another process or application uses
the Nexus APIs directly to open/close a window explicitly,
then this API function will not provide accurate information.

Returns:
DFB_OK if the arguments are valid.
DFB_INVARG is any arguments are invalid.
DFB_INIT if DFB platform code not already initialised.

See Also:
DFB_Platform_SetDisplayOutput
***************************************************************************/
DFB_PlatformResult DFB_Platform_GetDisplayWindow(
    DFB_PlatformNexusHandle        displayHandle,
    int                            windowIndex,
    DFB_PlatformNexusHandle       *pWindowHandle  /* [out] */
    );

/***************************************************************************
Summary:
Get the output handle for a particular output connector belonging to the
provided display. If it is NULL, then this means that the output is
NOT connected.

Description:
This will only provide what output connection DFB knows that
is associated with a display.  If another process or application uses
the Nexus APIs directly to connect/disconnect an output with a display,
then this API function will not provide accurate information.

Returns:
DFB_OK if the arguments are valid.
DFB_INVARG is any arguments are invalid.
DFB_INIT if DFB platform code not already initialised.

See Also:
DFB_Platform_SetDisplayOutput
***************************************************************************/
DFB_PlatformResult DFB_Platform_GetDisplayOutput(
    DFB_PlatformNexusHandle        displayHandle,
    DFB_PlatformDisplayOutputType  outputType,
    int                            outputIndex,
    DFB_PlatformNexusHandle       *pOutputHandle  /* [out] */
    );

/***************************************************************************
Summary:
Associate an output handle with a display without physically adding or
removing the output connection.

If NULL is passed as the handle, then this means
the particular output is no longer connected to the given display.

Description:
This will allow an external application that has already connected a given
output to a display, to signal this connection status to DFB.  This API
would typically be used if the application used the NEXUS API to obtain
a handle to an output and connected this to the display.   Similarly, if
the application removed the output from the display using the NEXUS API,
then this function can be used to signal to DFB that the connection no
longer exists.

Returns:
DFB_OK if all arguments are valid.
DFB_INVARG if any arguments are invalid.
DFB_INIT if DFB platform code not already initialised.

See Also:
DFB_Platform_GetDisplayOutput
***************************************************************************/
DFB_PlatformResult DFB_Platform_SetDisplayOutput(
    DFB_PlatformNexusHandle        displayHandle,
    DFB_PlatformDisplayOutputType  outputType,
    int                            outputIndex,
    DFB_PlatformNexusHandle        outputHandle
    );

/***************************************************************************
Summary:
DFB_Platform_AddDisplayOutput will add an output to an already opened display.

Description:
This function allows the client application to specify what output should
be associated with a display.  If an output is already connected, then this
will return an error.

Returns:
DFB_OK if all arguments were valid and the output was successfully added.
DFB_FAILURE if the output could not be added (e.g. already connected).
DFB_INVARG if any of the arguments are invalid.
DFB_INIT if DFB platform code not already initialised.

See Also:
DFB_Platform_RemoveDisplayOutput
***************************************************************************/
DFB_PlatformResult DFB_Platform_AddDisplayOutput(
    DFB_PlatformNexusHandle       displayHandle,
    DFB_PlatformDisplayOutputType outputType,
    int                           outputIndex
    );

/***************************************************************************
Summary:
DFB_Platform_RemoveDisplayOutput will remove an output from a display.

Description:
This function allows the client application to specify what output should
be removed from a display.  If an output is already removed, then this
will return an error.

Returns:
DFB_OK if all arguments were valid and the output was successfully removed.
DFB_FAILURE if the output could not be removed (e.g. already removed).
DFB_INVARG if any of the arguments are invalid.
DFB_INIT if DFB platform code not already initialised.

See Also:
DFB_Platform_AddDisplayOutput
***************************************************************************/
DFB_PlatformResult DFB_Platform_RemoveDisplayOutput(
    DFB_PlatformNexusHandle       displayHandle,
    DFB_PlatformDisplayOutputType outputType,
    int                           outputIndex
    );

/***************************************************************************
Summary:
DFB_Platform_RemoveDisplayOutputs will remove all outputs that are
currently connected to a given display.

Description:
This function allows the client application to easily remove all outputs
that are connected to a display.  The outputs that are removed are the
ones that DirectFB already knows are connected.

Returns:
DFB_OK if all arguments were valid and the outputs were successfully removed.
DFB_FAILURE if the outputs could not be removed (e.g. already removed).
DFB_INVARG if any of the arguments are invalid.
DFB_INIT if DFB platform code not already initialised.

See Also:
DFB_Platform_AddDisplayOutput, DFB_Platform_RemoveDisplayOutput
***************************************************************************/
DFB_PlatformResult DFB_Platform_RemoveDisplayOutputs(
    DFB_PlatformNexusHandle displayHandle
    );

/***************************************************************************
Summary:
DFB_Platform_HdmiHotplugHandler allows a client application to call and
run the HDMI hotplug handler logic in the DFB platform layer.

Description:
The function would typically be called by a client application that has
registered to receive the Nexus HDMI callback event(s).   The client app
should call this function after receiving the callback to ensure that the
internal state of the DFB platform layer code is consistent.

The default HDMI "hotplug" callback is handled internally within the
DFB platform code.

The HDMI handle can be obtained with a call to DFB_Platform_GetDisplayOutput().

Returns:
DFB_OK if no error found during this call.
DFB_FAILURE if an error was encountered trying to set the handler.
DFB_INVARG if any of the arguments are invalid.
DFB_INIT if DFB platform code not already initialised.

See Also:
DFB_Platform_GetDisplayOutput
***************************************************************************/
DFB_PlatformResult DFB_Platform_HdmiHotplugHandler(
    DFB_PlatformNexusHandle hdmiHandle,
    DFB_PlatformNexusHandle displayHandle
    );

/***************************************************************************
Summary:
DFB_Platform_DisplayGetSettings will retrieve the current display settings.

Description:
This function allows the client application to query what the current
display settings are.  For example, it is possible to query what the current
aspect ratio is and video format (e.g. 720p/60Hz).

Returns:
DFB_OK if arguments passed were valid.
DFB_INVARG if any of the arguments passed are invalid.
DFB_INIT if DFB platform code not already initialised.

See Also:
DFB_Platform_SetDisplaySettings
***************************************************************************/
DFB_PlatformResult DFB_Platform_GetDisplaySettings(
    DFB_PlatformNexusHandle      displayHandle,
    DFB_PlatformDisplaySettings *pSettings
    );

/***************************************************************************
Summary:
DFB_Platform_SetDisplaySettings will allow a client application to notify
DirectFB of the current display state.  The display will then be updated
with these settings.

Description:
This function allows the client application(s) to setup the display
so that apps and DirectFB can remain in sync with each other.

An application will typically call DFB_Platform_GetDisplaySettings()
after DFB_Platform_Init() to determine the current display settings.
Then the settings can be modified and written back using this function.

Returns:
DFB_OK if no errors found during setting of display.
DFB_FAILURE if error found during setting of the display.
DFB_INVARG if any argument passed is invalid.
DFB_INIT if DFB platform code not already initialised.

See Also:
DFB_Platform_GetDisplaySettings
***************************************************************************/
DFB_PlatformResult DFB_Platform_SetDisplaySettings(
    DFB_PlatformNexusHandle      displayHandle,
    DFB_PlatformDisplaySettings *pSettings
    );

/***************************************************************************
Summary:
Get status about the platform layer.

Description:
This will return the DFB platform status.  The status includes whether
this is a master application or not and whether Nexus has already been
initialised/joined or not.  This function can be called before
DFB_Platform_Init() has been called.

Returns:
DFB_OK if no problems encountered getting the status
DFB_NOSHAREDMEMORY if trying to setup shared memory failed
***************************************************************************/
DFB_PlatformResult DFB_Platform_GetStatus(
    DFB_PlatformStatus *pStatus             /* [out] */
    );

/***************************************************************************
Summary:
Get Broadcom DirectFB Platform Version

Description:
This will return the DFB platform version number as a separate
major and minor number. A Change in major number means that the
API is not binary compatible, whereas a change in minor number
means ABI compatibility.
***************************************************************************/
void DFB_Platform_GetVersion(
    unsigned *pMajor,                       /* [out] */
    unsigned *pMinor                        /* [out] */
    );

/***************************************************************************
Summary:
DFB_Platform_Init will initialize the Broadcom DirectFB Platform Interface.

Description:
DFB_Platform_Init can initialize platform specifics like Nexus heap memory
configuration, display configuration and open Nexus modules like graphics2d.

This function is normally automatically called indirectly by the system
driver during the "system_initialize() / system_join()" stage.

All applications can call DFB_Platform_Init() directly to provide non-default
values.  All applications should call this function first before making other
platform specific calls (other than DFB_Platform_GetDefaultSettings(),
and DFB_Platform_GetStatus()).

Do not call DFB_Platform_Init_tagged directly. Instead, call DFB_Platform_Init.
This will perform basic version checking to make sure you have a properly
configured system.

Returns:
DFB_OK if no errors found during initialisation.
DFB_INVARG if any invalid argument was passed in pSettings or pSettings is NULL.
DFB_INIT if error found during initialisation.

See Also:
DFB_Platform_Uninit
***************************************************************************/
DFB_PlatformResult DFB_Platform_Init_tagged(
    DFB_PlatformSettings *pSettings,        /* Do NOT pass NULL */
    unsigned              versionCheck,     /* set by DFB_Platform_Init macro */
    unsigned              structSizeCheck   /* set by DFB_Platform_Init macro */
    );

#define DFB_Platform_Init(PSETTINGS) \
    DFB_Platform_Init_tagged((PSETTINGS), DFB_PLATFORM_GET_VERSION(), DFB_PLATFORM_P_GET_STRUCT_SIZE())


/***************************************************************************
Summary:
Uninitialize DirectFB Platform interface

Description:
This function will cleanup any resources that were acquired during
DFB_Platform_Init() for that process.

After calling DFB_Platform_Uninit, you can call DFB_Platform_Init to bring up
DirectFB Platform layer again.

Returns:
DFB_OK if DFB platform code was uninitialised successfully.
DFB_INIT if DFB platform code not already initialised.

See Also:
DFB_Platform_Init
***************************************************************************/
DFB_PlatformResult DFB_Platform_Uninit(void);

#ifdef __cplusplus
} /*extern "C" */
#endif

#endif
