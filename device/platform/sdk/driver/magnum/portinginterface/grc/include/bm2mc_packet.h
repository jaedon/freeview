/***************************************************************************
 *     Copyright (c) 2010-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bm2mc_packet.h $
 * $brcm_Revision: Hydra_Software_Devel/33 $
 * $brcm_Date: 12/16/11 6:54p $
 *
 * Module Description: M2MC Packet header
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/grc/7405/bm2mc_packet.h $
 * 
 * Hydra_Software_Devel/33   12/16/11 6:54p nissen
 * SW7038-3520: Added flag to force fixed scale factor of 0.
 * 
 * Hydra_Software_Devel/32   9/16/11 1:11p nissen
 * SW7425-1207: Added support for UpdateScaleBlit packet.
 * 
 * Hydra_Software_Devel/31   9/1/11 5:35p nissen
 * SW7405-3671: Added FixedScale macro.
 * 
 * Hydra_Software_Devel/30   7/27/11 5:58p nissen
 * SW7405-3671: Added support for fixed scaling with 420 destriping.
 * 
 * Hydra_Software_Devel/29   5/11/11 1:23p nissen
 * SW7401-4501: Added L8 pixel format.
 * 
 * Hydra_Software_Devel/28   4/25/11 3:59p nissen
 * SW7630-104: Added support for A8_Y8, Cb8 and Cr8 formats.
 * 
 * Hydra_Software_Devel/27   3/18/11 7:32p nissen
 * SW7038-3500: Fixing header size.
 * 
 * Hydra_Software_Devel/26   3/18/11 3:02p nissen
 * SW7420-1575: Switch format field back to uint8_t.
 * 
 * Hydra_Software_Devel/25   3/7/11 3:59p nissen
 * SW7420-1575: Added support for surface rectangle verification.
 * 
 * Hydra_Software_Devel/24   3/7/11 2:11p nissen
 * SW7038-3500: Switched header size back.
 * 
 * Hydra_Software_Devel/23   3/5/11 4:47p nissen
 * SW7038-3500: Changed header size to fit filter table.
 * 
 * Hydra_Software_Devel/22   3/4/11 1:41p nissen
 * SW7038-3500: Fixed header size.
 * 
 * Hydra_Software_Devel/21   3/3/11 4:49p nissen
 * SW7038-3500: Fixed build error for 7038.
 * 
 * Hydra_Software_Devel/20   3/3/11 3:11p nissen
 * SW7038-3500: Fixed build error for 7038.
 * 
 * Hydra_Software_Devel/19   3/2/11 4:55p nissen
 * SW7405-3671: Added new luma pixel formats.
 * 
 * Hydra_Software_Devel/18   3/1/11 1:39p nissen
 * SW7038-3500: Removed include file.
 * 
 * Hydra_Software_Devel/17   2/28/11 2:01p nissen
 * SW7038-3500: Added include file to fix structure sizing for filter
 * packet.
 * 
 * Hydra_Software_Devel/16   2/25/11 5:09p nissen
 * SW7038-3500: Added initial support for 7038.
 * 
 * Hydra_Software_Devel/15   11/15/10 3:07p nissen
 * SW7335-916: Added linear destripe field to source control packet.
 * 
 * Hydra_Software_Devel/14   11/11/10 1:41p nissen
 * SW7335-916: Added support for FixedScale packet.
 * 
 * Hydra_Software_Devel/13   10/6/10 2:23p nissen
 * SW7405-3671: Added DestinationColor packet.
 * 
 * Hydra_Software_Devel/12   9/27/10 7:15p nissen
 * SW7405-3671: Fixed typos in write macros.
 * 
 * Hydra_Software_Devel/11   9/23/10 2:42p nissen
 * SW7405-3671: Changed macro params to caps.
 * 
 * Hydra_Software_Devel/10   9/23/10 2:02p nissen
 * SW7405-3671: Fixed write macros.
 * 
 * Hydra_Software_Devel/9   8/9/10 5:05p nissen
 * SW7405-3671: Removed pixel formats that are not in PXL.
 * 
 * Hydra_Software_Devel/8   8/6/10 1:53p nissen
 * SW7405-3671: Added max value for packet type enum.
 * 
 * Hydra_Software_Devel/7   8/1/10 12:03p nissen
 * SW7405-3671: Fixed macro that writes scale blit packet.
 * 
 * Hydra_Software_Devel/6   7/21/10 4:31p nissen
 * SW7405-3671: Fixed init macro and removed dst color macro.
 * 
 * Hydra_Software_Devel/5   7/19/10 6:49p nissen
 * SW7405-3671: Added more packets and macros to write packets.
 * 
 * Hydra_Software_Devel/4   7/12/10 2:23p nissen
 * SW7405-3671: Updated packet structures and types and added
 * documentation.
 * 
 * Hydra_Software_Devel/3   6/30/10 9:26p nissen
 * SW7405-3671: Updated filter and color matrix packets.
 * 
 * Hydra_Software_Devel/2   6/30/10 7:49p nissen
 * SW7405-3671: Added blending and pixel format enums.
 * 
 * Hydra_Software_Devel/1   6/21/10 11:19a nissen
 * SW7405-3671: Initial version.
 * 
 ***************************************************************************/

#ifndef BM2MC_PACKET_H__
#define BM2MC_PACKET_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
PACKET TYPE ENUMERATION
 ***************************************************************************/
typedef enum BM2MC_PACKET_PacketType
{
	BM2MC_PACKET_PacketType_eSourceFeeder = 1,           /* source plane and color */
	BM2MC_PACKET_PacketType_eSourceFeeders,              /* source planes and color */
	BM2MC_PACKET_PacketType_eSourceColor,                /* source color */
	BM2MC_PACKET_PacketType_eSourceNone,                 /* source none (default) */
	BM2MC_PACKET_PacketType_eSourceControl,              /* source control of zero_pad and chroma_filter */
	BM2MC_PACKET_PacketType_eDestinationFeeder,          /* destination plane and color */
	BM2MC_PACKET_PacketType_eDestinationColor,           /* destination color */
	BM2MC_PACKET_PacketType_eDestinationNone,            /* destination none (default) */
	BM2MC_PACKET_PacketType_eDestinationControl,         /* destination control of zero_pad and chroma_filter */
	BM2MC_PACKET_PacketType_eOutputFeeder,               /* output plane */
	BM2MC_PACKET_PacketType_eOutputControl,              /* output control of dither and chroma_filter */
	BM2MC_PACKET_PacketType_eBlend,                      /* color and alpha blend, and blend color */
	BM2MC_PACKET_PacketType_eBlendColor,                 /* blend color */
	BM2MC_PACKET_PacketType_eRop,                        /* ternary raster operation */
	BM2MC_PACKET_PacketType_eSourceColorkey,             /* source colorkeys, masks, and replacements */
	BM2MC_PACKET_PacketType_eSourceColorkeyEnable,       /* source colorkey enable */
	BM2MC_PACKET_PacketType_eDestinationColorkey,        /* destination colorkeys, masks, and replacements */
	BM2MC_PACKET_PacketType_eDestinationColorkeyEnable,  /* destination colorkey enable */
	BM2MC_PACKET_PacketType_eFilter,                     /* filter coefficient table */
	BM2MC_PACKET_PacketType_eFilterEnable,               /* filter enable */
	BM2MC_PACKET_PacketType_eFilterControl,              /* filter control of rounding, channel adjustment and ordering */
	BM2MC_PACKET_PacketType_eSourceColorMatrix,          /* source color matrix */
	BM2MC_PACKET_PacketType_eSourceColorMatrixEnable,    /* source color matrix enable */
	BM2MC_PACKET_PacketType_eSourcePalette,              /* source palette address */
	BM2MC_PACKET_PacketType_eAlphaPremultiply,           /* multiply source pixel color by pixel alpha before blending */
	BM2MC_PACKET_PacketType_eAlphaDemultiply,            /* divide source pixel color by pixel alpha after blending */
	BM2MC_PACKET_PacketType_eDestAlphaPremultiply,       /* multiply destination pixel color by pixel alpha before blending */
	BM2MC_PACKET_PacketType_eMirror,                     /* reverse read/write order for source, destination and output feeders */
	BM2MC_PACKET_PacketType_eFixedScale,                 /* initial phase and fixed scale factor (cannot be used with execute=true) */
	BM2MC_PACKET_PacketType_eDestripeFixedScale,         /* initial phase and fixed scale factor for YCbCr420 destriping (cannot be used with execute=true) */
	BM2MC_PACKET_PacketType_eFillBlit,                   /* fill output rectangle */
	BM2MC_PACKET_PacketType_eCopyBlit,                   /* copy source rectangle to output point */
	BM2MC_PACKET_PacketType_eBlendBlit,                  /* blend/rop/colorkey source rectangle to output point */
	BM2MC_PACKET_PacketType_eScaleBlit,                  /* scale source rectangle to output rectangle */
	BM2MC_PACKET_PacketType_eScaleBlendBlit,             /* scale and blend/rop/colorkey source rectangle to output rectangle */
	BM2MC_PACKET_PacketType_eUpdateScaleBlit,            /* scale source rectangle to output rectangle with partial update to output */
	BM2MC_PACKET_PacketType_eDestripeBlit,               /* YCbCr420 destripe blit */
	BM2MC_PACKET_PacketType_eResetState,                 /* reset state to default */
	BM2MC_PACKET_PacketType_eSaveState,                  /* save state (state must be restored before sync/checkpoint) */
	BM2MC_PACKET_PacketType_eRestoreState,               /* restore state */
	BM2MC_PACKET_PacketType_eMax                         /* maximum enum value */
}
BM2MC_PACKET_PacketType;

/***************************************************************************
PACKET HEADER STRUCTURE
 ***************************************************************************/
typedef struct
{
	uint8_t type;                    /* BM2MC_PACKET_PacketType enum */
	uint8_t size;                    /* size of packet structure */
	bool execute;                    /* execute blit operation after packet */
	bool sync;                       /* reserved */
}
BM2MC_PACKET_Header;

/***************************************************************************
PACKET MACROS

Usage:  BM2MC_PACKET_INIT( packet, BlendColor, false );
        (write blend color packet)
        BM2MC_PACKET_TERM( packet );
        BM2MC_PACKET_INIT( packet, FillBlit, true );
        (write fill packet)
        BM2MC_PACKET_TERM( packet );
 ***************************************************************************/
#define BM2MC_PACKET_INIT( \
	PACKET,                         /* ptr to packet */ \
	TYPE,                           /* BM2MC_PACKET_PacketType enum */ \
	EXECUTE )                       /* execute blit operation after packet */ \
do { \
	BDBG_CASSERT((sizeof (*(PACKET)) % sizeof (uint32_t)) == 0); \
	BDBG_CASSERT((sizeof (*(PACKET)) == sizeof (BM2MC_PACKET_Packet##TYPE))); \
	(PACKET)->header.type = BM2MC_PACKET_PacketType_e##TYPE; \
	(PACKET)->header.size = sizeof (*(PACKET)) / sizeof (uint32_t); \
	(PACKET)->header.execute = EXECUTE; \
	(PACKET)->header.sync = 0; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_TERM( packet ) (packet)++

/***************************************************************************
PACKET STRUCTURES
 ***************************************************************************/
typedef enum BM2MC_PACKET_PixelFormat
{
	BM2MC_PACKET_PixelFormat_eUnknown = 0,
	BM2MC_PACKET_PixelFormat_eR5_G6_B5,              /* 16-bit RGB565, (constant alpha) */
	BM2MC_PACKET_PixelFormat_eB5_G6_R5,              /* 16-bit BGR565, (constant alpha) */
	BM2MC_PACKET_PixelFormat_eA1_R5_G5_B5,           /* 16-bit ARGB1555 */
	BM2MC_PACKET_PixelFormat_eX1_R5_G5_B5,           /* 16-bit XRGB1555 */
	BM2MC_PACKET_PixelFormat_eA1_B5_G5_R5,           /* 16-bit ABGR1555 */
	BM2MC_PACKET_PixelFormat_eX1_B5_G5_R5,           /* 16-bit XBGR1555 */
	BM2MC_PACKET_PixelFormat_eR5_G5_B5_A1,           /* 16-bit RGBA5551 */
	BM2MC_PACKET_PixelFormat_eR5_G5_B5_X1,           /* 16-bit RGBX5551 */
	BM2MC_PACKET_PixelFormat_eB5_G5_R5_A1,           /* 16-bit BGRA5551 */
	BM2MC_PACKET_PixelFormat_eB5_G5_R5_X1,           /* 16-bit BGRX5551 */
	BM2MC_PACKET_PixelFormat_eA4_R4_G4_B4,           /* 16-bit ARGB4444 */
	BM2MC_PACKET_PixelFormat_eX4_R4_G4_B4,           /* 16-bit XRGB4444 */
	BM2MC_PACKET_PixelFormat_eA4_B4_G4_R4,           /* 16-bit ABGR4444 */
	BM2MC_PACKET_PixelFormat_eX4_B4_G4_R4,           /* 16-bit XBGR4444 */
	BM2MC_PACKET_PixelFormat_eR4_G4_B4_A4,           /* 16-bit RGBA4444 */
	BM2MC_PACKET_PixelFormat_eR4_G4_B4_X4,           /* 16-bit RGBX4444 */
	BM2MC_PACKET_PixelFormat_eB4_G4_R4_A4,           /* 16-bit BGRA4444 */
	BM2MC_PACKET_PixelFormat_eB4_G4_R4_X4,           /* 16-bit BGRX4444 */
	BM2MC_PACKET_PixelFormat_eA8_R8_G8_B8,           /* 32-bit ARGB8888 */
	BM2MC_PACKET_PixelFormat_eX8_R8_G8_B8,           /* 32-bit XRGB8888 */
	BM2MC_PACKET_PixelFormat_eA8_B8_G8_R8,           /* 32-bit ABGR8888 */
	BM2MC_PACKET_PixelFormat_eX8_B8_G8_R8,           /* 32-bit XBGR8888 */
	BM2MC_PACKET_PixelFormat_eR8_G8_B8_A8,           /* 32-bit RGBA8888 */
	BM2MC_PACKET_PixelFormat_eR8_G8_B8_X8,           /* 32-bit RGBX8888 */
	BM2MC_PACKET_PixelFormat_eB8_G8_R8_A8,           /* 32-bit BGRA8888 */
	BM2MC_PACKET_PixelFormat_eB8_G8_R8_X8,           /* 32-bit BGRX8888 */
	BM2MC_PACKET_PixelFormat_eA8,                    /* 8-bit alpha-only surface (constant color) */
	BM2MC_PACKET_PixelFormat_eA4,                    /* 4-bit alpha-only surface (constant color) */
	BM2MC_PACKET_PixelFormat_eA2,                    /* 2-bit alpha-only surface (constant color) */
	BM2MC_PACKET_PixelFormat_eA1,                    /* 1-bit alpha-only surface (constant color) */
	BM2MC_PACKET_PixelFormat_eW1,                    /* 1-bit window alpha-only surface (constant color) */
	BM2MC_PACKET_PixelFormat_eA8_P8,                 /* 8-bit alpha and 8-bit palette index */
	BM2MC_PACKET_PixelFormat_eP8,                    /* 8-bit palette index */
	BM2MC_PACKET_PixelFormat_eP4,                    /* 4-bit palette index */
	BM2MC_PACKET_PixelFormat_eP2,                    /* 2-bit palette index */
	BM2MC_PACKET_PixelFormat_eP1,                    /* 1-bit palette index */
	BM2MC_PACKET_PixelFormat_eY8_P8,                 /* 8-bit luma and 8-bit palette index */
	BM2MC_PACKET_PixelFormat_eY8,                    /* 8-bit luma-only, for YCbCr420 */
	BM2MC_PACKET_PixelFormat_eA8_Y8,                 /* 8-bit alpha and 8-bit luma, for YCbCr420 */
	BM2MC_PACKET_PixelFormat_eCb8_Cr8,               /* 16-bit chroma-only, for YCbCr420 */
	BM2MC_PACKET_PixelFormat_eCr8_Cb8,               /* 16-bit chroma-only, for YCbCr420 */
	BM2MC_PACKET_PixelFormat_eCb8,                   /* 8-bit chroma-only (Cb) */
	BM2MC_PACKET_PixelFormat_eCr8,                   /* 8-bit chroma-only (Cr) */
	BM2MC_PACKET_PixelFormat_eY08_Cb8_Y18_Cr8,       /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
	BM2MC_PACKET_PixelFormat_eY08_Cr8_Y18_Cb8,       /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
	BM2MC_PACKET_PixelFormat_eY18_Cb8_Y08_Cr8,       /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
	BM2MC_PACKET_PixelFormat_eY18_Cr8_Y08_Cb8,       /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
	BM2MC_PACKET_PixelFormat_eCb8_Y08_Cr8_Y18,       /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
	BM2MC_PACKET_PixelFormat_eCb8_Y18_Cr8_Y08,       /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
	BM2MC_PACKET_PixelFormat_eCr8_Y18_Cb8_Y08,       /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
	BM2MC_PACKET_PixelFormat_eCr8_Y08_Cb8_Y18,       /* 32-bit for 2 pixels, YCbCr422 w/ 8-bit values */
	BM2MC_PACKET_PixelFormat_eX2_Cr10_Y10_Cb10,      /* 32-bit for 1 pixel,  YCbCr444 w/ 10-bit values */
	BM2MC_PACKET_PixelFormat_eA8_Y8_Cb8_Cr8,         /* YCbCr444 w/ 8-bit values */
	BM2MC_PACKET_PixelFormat_eA8_Cr8_Cb8_Y8,         /* YCbCr444 w/ 8-bit values */
	BM2MC_PACKET_PixelFormat_eCr8_Cb8_Y8_A8,         /* YCbCr444 w/ 8-bit values */
	BM2MC_PACKET_PixelFormat_eY8_Cb8_Cr8_A8,         /* YCbCr444 w/ 8-bit values */
	BM2MC_PACKET_PixelFormat_eY010_Cb10_Y110_Cr10,   /* YCbCr422 w/ 10-bit values */
	BM2MC_PACKET_PixelFormat_eY010_Cr10_Y110_Cb10,   /* YCbCr422 w/ 10-bit values */
	BM2MC_PACKET_PixelFormat_eY110_Cb10_Y010_Cr10,   /* YCbCr422 w/ 10-bit values */
	BM2MC_PACKET_PixelFormat_eY110_Cr10_Y010_Cb10,   /* YCbCr422 w/ 10-bit values */
	BM2MC_PACKET_PixelFormat_eCb10_Y010_Cr10_Y110,   /* YCbCr422 w/ 10-bit values */
	BM2MC_PACKET_PixelFormat_eCb10_Y110_Cr10_Y010,   /* YCbCr422 w/ 10-bit values */
	BM2MC_PACKET_PixelFormat_eCr10_Y110_Cb10_Y010,   /* YCbCr422 w/ 10-bit values */
	BM2MC_PACKET_PixelFormat_eCr10_Y010_Cb10_Y110,   /* YCbCr422 w/ 10-bit values */
	BM2MC_PACKET_PixelFormat_eL8,                    /* 8-bit luma */
	BM2MC_PACKET_PixelFormat_eL4_A4,                 /* 4-bit luma and 4-bit alpha */
	BM2MC_PACKET_PixelFormat_eL8_A8,                 /* 8-bit luma and 8-bit alpha */
	BM2MC_PACKET_PixelFormat_eL15_L05_A6,            /* 5-bit luma1, 5-bit luma0, 6-bit alpha */
	BM2MC_PACKET_PixelFormat_eMax
}
BM2MC_PACKET_PixelFormat;

/***************************************************************************/
typedef struct
{
	uint32_t address;                /* physical memory offset of plane */
	uint16_t pitch;                  /* pitch of plane */
	uint8_t format;                  /* BM2MC_PACKET_PixelFormat enum */
	uint16_t width;                  /* surface width, optional for verification */
	uint16_t height;                 /* surface height, optional for verification */
}
BM2MC_PACKET_Plane;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	BM2MC_PACKET_Plane plane;        /* source plane (defaults to none) */
	uint32_t color;                  /* 32-bit color (A, R/Y, G/Cb, B/Cr/P) (defaults to 0) */
}
BM2MC_PACKET_PacketSourceFeeder;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	BM2MC_PACKET_Plane plane0;       /* primary source plane (defaults to none) */
	BM2MC_PACKET_Plane plane1;       /* secondary source plane (defaults to none) */
	uint32_t color;                  /* 32-bit color (A, R/Y, G/Cb, B/Cr/P) (defaults to 0) */
}
BM2MC_PACKET_PacketSourceFeeders;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	uint32_t color;                  /* 32-bit color (A, R/Y, G/Cb, B/Cr/P) (defaults to 0) */
}
BM2MC_PACKET_PacketSourceColor;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
}
BM2MC_PACKET_PacketSourceNone;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	bool zero_pad;                   /* zero pad lower bits when expanding pixel channels */
	                                 /* instead of replicating upper bits (defaults to false) */
	bool chroma_filter;              /* filter chroma's when converting YCbCr422 source */
	                                 /* format into internal YCbCr444 format (defaults to true) */
	bool linear_destripe;            /* YCbCr 420 image is linear (not striped) */
	uint8_t reserved[1];             /* align to 4-bytes */
}
BM2MC_PACKET_PacketSourceControl;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	BM2MC_PACKET_Plane plane;        /* destination plane (defaults to none) */
	uint32_t color;                  /* 32-bit color (A, R/Y, G/Cb, B/Cr/P) (defaults to 0) */
}
BM2MC_PACKET_PacketDestinationFeeder;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	uint32_t color;                  /* 32-bit color (A, R/Y, G/Cb, B/Cr/P) (defaults to 0) */
}
BM2MC_PACKET_PacketDestinationColor;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
}
BM2MC_PACKET_PacketDestinationNone;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	bool zero_pad;                   /* zero pad lower bits when expanding pixel channels */
	                                 /* instead of replicating upper bits (defaults to false) */
	bool chroma_filter;              /* filter chroma's when converting YCbCr422 source */
	                                 /* format into internal YCbCr444 format (defaults to true) */
	uint8_t reserved[2];             /* align to 4-bytes */
}
BM2MC_PACKET_PacketDestinationControl;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	BM2MC_PACKET_Plane plane;        /* output plane  (defaults to none) */
}
BM2MC_PACKET_PacketOutputFeeder;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	bool dither;                     /* dither pixel channels less than 8-bits (defaults to false) */
	bool chroma_filter;              /* filter chroma's when converting YCbCr422 source */
	                                 /* format into output YCbCr444 format (defaults to true) */
	uint8_t reserved[2];             /* align to 4-bytes */
}
BM2MC_PACKET_PacketOutputControl;

/***************************************************************************/
typedef enum BM2MC_PACKET_BlendFactor
{
	BM2MC_PACKET_BlendFactor_eZero = 0,                /* Zero */
	BM2MC_PACKET_BlendFactor_eHalf,                    /* 1/2 */
	BM2MC_PACKET_BlendFactor_eOne,                     /* One */
	BM2MC_PACKET_BlendFactor_eSourceColor,             /* Color from source */
	BM2MC_PACKET_BlendFactor_eInverseSourceColor,      /* 1-color from source */
	BM2MC_PACKET_BlendFactor_eSourceAlpha,             /* Alpha from source */
	BM2MC_PACKET_BlendFactor_eInverseSourceAlpha,      /* 1-alpha from source */
	BM2MC_PACKET_BlendFactor_eDestinationColor,        /* Color from destination */
	BM2MC_PACKET_BlendFactor_eInverseDestinationColor, /* 1-color from destination */
	BM2MC_PACKET_BlendFactor_eDestinationAlpha,        /* Alpha from destination */
	BM2MC_PACKET_BlendFactor_eInverseDestinationAlpha, /* 1-alpha from destination */
	BM2MC_PACKET_BlendFactor_eConstantColor,           /* Color from blend color */
	BM2MC_PACKET_BlendFactor_eInverseConstantColor,    /* 1-color from blend color */
	BM2MC_PACKET_BlendFactor_eConstantAlpha,           /* Alpha from blend color */
	BM2MC_PACKET_BlendFactor_eInverseConstantAlpha     /* 1-alpha from blend color */
}
BM2MC_PACKET_BlendFactor;

/***************************************************************************/
/* The blend equation is "a*b +/- c*d +/- e" */
typedef struct
{
	uint8_t a;                       /* BM2MC_PACKET_BlendFactor */
	uint8_t b;                       /* BM2MC_PACKET_BlendFactor */
	bool sub_cd;                     /* add or subtract cd*/
	uint8_t c;                       /* BM2MC_PACKET_BlendFactor */
	uint8_t d;                       /* BM2MC_PACKET_BlendFactor */
	bool sub_e;                      /* add or subtract e*/
	uint8_t e;                       /* BM2MC_PACKET_BlendFactor */
}
BM2MC_PACKET_Blend;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	BM2MC_PACKET_Blend color_blend;  /* color blend equation (defaults to eSourceColor) */
	BM2MC_PACKET_Blend alpha_blend;  /* alpha blend equation (defaults to eSourceAlpha)*/
	uint32_t color;                  /* 32-bit color (A, R/Y, G/Cb, B/Cr/P) (defaults to 0) */
}
BM2MC_PACKET_PacketBlend;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	uint32_t color;                  /* 32-bit color (A, R/Y, G/Cb, B/Cr/P) (defaults to 0) */
}
BM2MC_PACKET_PacketBlendColor;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	uint32_t rop;                    /* ternary raster operation (defaults to 0) */
	uint32_t pattern0;               /* output aligned 1-bit pattern 0 (8x4) top (defaults to 0) */
	uint32_t pattern1;               /* output aligned 1-bit pattern 1 (8x4) bottom (defaults to 0) */
	uint32_t color0;                 /* 32-bit pattern color 0 (A, R/Y, G/Cb, B/Cr/P) (defaults to 0) */
	uint32_t color1;                 /* 32-bit pattern color 1 (A, R/Y, G/Cb, B/Cr/P) (defaults to 0) */
}
BM2MC_PACKET_PacketRop;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	uint32_t high;                   /* 32-bit colorkey high range (defaults to 0) */
	uint32_t low;                    /* 32-bit colorkey low range (defaults to 0) */
	uint32_t mask;                   /* 32-bit mask, used an pixel before compare (defaults to 0) */
	uint32_t replacement;            /* 32-bit replacement color, when colorkeyed (defaults to 0) */
	uint32_t replacement_mask;       /* 32-bit replacement mask, when colorkeyed (defaults to 0) */
	bool exclusive;                  /* exclusive high compare (defaults to false) */
}
BM2MC_PACKET_PacketSourceColorkey;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	bool enable;                     /* enable colorkey (defaults to false) */
	uint8_t reserved[3];             /* align to 4-bytes */
}
BM2MC_PACKET_PacketSourceColorkeyEnable;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	uint32_t high;                   /* 32-bit colorkey high range (defaults to 0) */
	uint32_t low;                    /* 32-bit colorkey low range (defaults to 0) */
	uint32_t mask;                   /* 32-bit mask, used an pixel before compare (defaults to 0) */
	uint32_t replacement;            /* 32-bit replacement color, when colorkeyed (defaults to 0) */
	uint32_t replacement_mask;       /* 32-bit replacement mask, when colorkeyed (defaults to 0) */
	bool exclusive;                  /* exclusive high compare (defaults to false) */
}
BM2MC_PACKET_PacketDestinationColorkey;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	bool enable;                     /* enable colorkey (defaults to false) */
	uint8_t reserved[3];             /* align to 4-bytes */
}
BM2MC_PACKET_PacketDestinationColorkeyEnable;

/***************************************************************************/
typedef struct
{
#if (BCHP_CHIP!=7038)
	int16_t coeffs[2][3];            /* fixed point in S1.8 format */
#else
	int16_t coeffs[8][8];            /* fixed point in S1.8 format */
#endif
}
BM2MC_PACKET_FilterCoeffs;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	BM2MC_PACKET_FilterCoeffs hor;   /* horizontal filter coefficients (defaults to bilinear coeffs) */
	BM2MC_PACKET_FilterCoeffs ver;   /* vertical filter coefficients (defaults to bilinear coeffs) */
}
BM2MC_PACKET_PacketFilter;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	bool enable;                     /* enable filter (defaults to false) */
	uint8_t reserved[3];             /* align to 4-bytes */
}
BM2MC_PACKET_PacketFilterEnable;

/***************************************************************************/
typedef enum BM2MC_PACKET_eFilterOrder
{
	BM2MC_PACKET_eFilterOrder_FilterColorkeyMatrix = 0,
	BM2MC_PACKET_eFilterOrder_FilterMatrixColorkey,
	BM2MC_PACKET_eFilterOrder_ColorkeyMatrixFilter,
	BM2MC_PACKET_eFilterOrder_ColorkeyFilterMatrix,
	BM2MC_PACKET_eFilterOrder_MatrixFilterColorkey,
	BM2MC_PACKET_eFilterOrder_MatrixColorkeyFilter,
	BM2MC_PACKET_eFilterOrder_BypassAll
}
BM2MC_PACKET_eFilterOrder;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	bool round;                      /* round pixel channels instead of truncating when filtering (defaults to true) */
	bool adjust_color;               /* subtract value from color channels before filtering (defaults to false) */
	bool sub_alpha;                  /* use alpha/2 as subtract value instead of 128 (defaults to false) */
	uint8_t order;                   /* colorkey/filter/matrix ordering (defaults to colorkey/filter/matrix) */
}
BM2MC_PACKET_PacketFilterControl;

/***************************************************************************/
/* Color Matrix Formula:
	C2out = m[0][0]*C2in + m[0][1]*C1in + m[0][2]*C0in + m[0][3]*C3in + m[0][4]
	C1out = m[1][0]*C2in + m[1][1]*C1in + m[1][2]*C0in + m[1][3]*C3in + m[1][4]
	C0out = m[2][0]*C2in + m[2][1]*C1in + m[2][2]*C0in + m[2][3]*C3in + m[2][4]
	C3out = m[3][0]*C2in + m[3][1]*C1in + m[3][2]*C0in + m[3][3]*C3in + m[3][4]
*/
typedef struct
{
	int16_t m[4][5];        /* fixed point in S3.10 format for multipliers,
	                           fixed point in S9.4 format for adders */
}
BM2MC_PACKET_ColorMatrix;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	BM2MC_PACKET_ColorMatrix matrix; /* color matrix (defaults to all 0s) */
}
BM2MC_PACKET_PacketSourceColorMatrix;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	bool enable;                     /* enable color matrix (defaults to false) */
	uint8_t reserved[3];             /* align to 4-bytes */
}
BM2MC_PACKET_PacketSourceColorMatrixEnable;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	uint32_t address;                /* physical memory offset of palette (defaults to null) */
}
BM2MC_PACKET_PacketSourcePalette;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	bool enable;                     /* multiply source pixel's color by its alpha before blending (defaults to false) */
	uint8_t reserved[3];             /* align to 4-bytes */
}
BM2MC_PACKET_PacketAlphaPremultiply;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	bool enable;                     /* divide source pixel's color by its alpha after blending (defaults to false) */
	uint8_t reserved[3];             /* align to 4-bytes */
}
BM2MC_PACKET_PacketAlphaDemultiply;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	bool enable;                     /* multiply destination pixel's color by its alpha (defaults to false) */
	uint8_t reserved[3];             /* align to 4-bytes */
}
BM2MC_PACKET_PacketDestAlphaPremultiply;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	bool src_hor;                    /* reverse horizontal direction of src (defaults to false) */
	bool src_ver;                    /* reverse vertical direction of src (defaults to false) */
	bool dst_hor;                    /* reverse horizontal direction of dst (defaults to false) */
	bool dst_ver;                    /* reverse vertical direction of dst (defaults to false) */
	bool out_hor;                    /* reverse horizontal direction of out (defaults to false) */
	bool out_ver;                    /* reverse vertical direction of out (defaults to false) */
	uint8_t reserved[2];             /* align to 4-bytes */
}
BM2MC_PACKET_PacketMirror;

/***************************************************************************/
typedef struct
{
	uint16_t x;                      /* x coordinate (0-8191) */
	uint16_t y;                      /* y coordinate (0-8191) */
}
BM2MC_PACKET_Point;

/***************************************************************************/
typedef struct
{
	uint16_t width;                  /* width (1-8191) */
	uint16_t height;                 /* height (1-8191) */
}
BM2MC_PACKET_Size;

/***************************************************************************/
typedef struct
{
	uint16_t x;                      /* x coordinate (0-8191) */
	uint16_t y;                      /* y coordinate (0-8191) */
	uint16_t width;                  /* width (1-8191) */
	uint16_t height;                 /* height (1-8191) */
}
BM2MC_PACKET_Rectangle;

/***************************************************************************/
#define BM2MC_PACKET_FIXED_SCALE_STEP_ZERO  0x80000000  /* set hor_step or ver_step to this value to use 0 as fixed scale */

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	int32_t hor_phase;               /* horizontal initial phase adjustment (0=ignore) */
	int32_t ver_phase;               /* vertical initial phase adjustment (0=ignore) */
	uint32_t hor_step;               /* horizontal fixed source step (0=ignore) */
	uint32_t ver_step;               /* vertical fixed source step (0=ignore) */
	uint32_t shift;                  /* amount of factional bits in values (max is 20) */
}
BM2MC_PACKET_PacketFixedScale;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;         /* packet header */
	BM2MC_PACKET_Rectangle chroma_rect; /* source chroma rect */
	int32_t hor_luma_phase;             /* horizontal luma initial phase adjustment (0=ignore) */
	int32_t ver_luma_phase;             /* vertical luma initial phase adjustment (0=ignore) */
	int32_t hor_chroma_phase;           /* horizontal chroma initial phase adjustment (0=ignore) */
	int32_t ver_chroma_phase;           /* vertical chroma initial phase adjustment (0=ignore) */
	uint32_t hor_luma_step;             /* horizontal luma fixed source step (0=ignore) */
	uint32_t ver_luma_step;             /* vertical luma fixed source step (0=ignore) */
	uint32_t shift;                     /* amount of factional bits in values (max is 20) */
}
BM2MC_PACKET_PacketDestripeFixedScale;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	BM2MC_PACKET_Rectangle rect;     /* output rect */
}
BM2MC_PACKET_PacketFillBlit;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	BM2MC_PACKET_Rectangle src_rect; /* source rect */
	BM2MC_PACKET_Point out_point;    /* output point */
}
BM2MC_PACKET_PacketCopyBlit;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	BM2MC_PACKET_Rectangle src_rect; /* source rect */
	BM2MC_PACKET_Point out_point;    /* output point */
	BM2MC_PACKET_Point dst_point;    /* destintation point */
}
BM2MC_PACKET_PacketBlendBlit;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	BM2MC_PACKET_Rectangle src_rect; /* source rect */
	BM2MC_PACKET_Rectangle out_rect; /* output rect */
}
BM2MC_PACKET_PacketScaleBlit;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	BM2MC_PACKET_Rectangle src_rect; /* source rect */
	BM2MC_PACKET_Rectangle out_rect; /* output rect */
	BM2MC_PACKET_Point dst_point;    /* destintation point */
}
BM2MC_PACKET_PacketScaleBlendBlit;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;         /* packet header */
	BM2MC_PACKET_Rectangle src_rect;    /* source rect */
	BM2MC_PACKET_Rectangle out_rect;    /* output rect */
	BM2MC_PACKET_Rectangle update_rect; /* update rect for output */
}
BM2MC_PACKET_PacketUpdateScaleBlit;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
	BM2MC_PACKET_Rectangle src_rect; /* source rect */
	BM2MC_PACKET_Rectangle out_rect; /* output rect */
	BM2MC_PACKET_Point dst_point;    /* destintation point */
	uint16_t source_stripe_width;    /* source stripe width */
	uint16_t luma_stripe_height;     /* luma (Y) stripe height */
	uint16_t chroma_stripe_height;   /* chroma (CbCr) stripe height */
	uint8_t reserved[2];             /* align to 4-bytes */
}
BM2MC_PACKET_PacketDestripeBlit;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
}
BM2MC_PACKET_PacketResetState;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
}
BM2MC_PACKET_PacketSaveState;

/***************************************************************************/
typedef struct
{
	BM2MC_PACKET_Header header;      /* packet header */
}
BM2MC_PACKET_PacketRestoreState;

/***************************************************************************
PACKET WRITE MACROS

The following macros are an optional method for writing packets and 
incrementing the packet pointer. Please be aware that the BUFFER param is 
an [in,out] param. Use as follows:

	current_buffer = first_buffer;
	BM2MC_PACKET_WRITE_SourceFeeder( current_buffer, plane, color, false );
	BM2MC_PACKET_WRITE_Blend( current_buffer, alphaBlend, colorBlend, color, true );
	size = current_buffer - first_buffer;
 ***************************************************************************/

#define BM2MC_PACKET_WRITE_SourceFeeder( BUFFER, PLANE, COLOR, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketSourceFeeder *packet = (BM2MC_PACKET_PacketSourceFeeder *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, SourceFeeder, EXECUTE ); \
	packet->plane = (PLANE); \
	packet->color = (COLOR); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_SourceFeeders( BUFFER, PLANE0, PLANE1, COLOR, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketSourceFeeders *packet = (BM2MC_PACKET_PacketSourceFeeders *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, SourceFeeders, EXECUTE ); \
	packet->plane0 = (PLANE0); \
	packet->plane1 = (PLANE1); \
	packet->color = (COLOR); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_SourceColor( BUFFER, COLOR, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketSourceColor *packet = (BM2MC_PACKET_PacketSourceColor *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, SourceColor, EXECUTE ); \
	packet->color = (COLOR); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_SourceNone( BUFFER, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketSourceNone *packet = (BM2MC_PACKET_PacketSourceNone *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, SourceNone, EXECUTE ); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_SourceControl( BUFFER, ZERO_PAD, CHROMA_FILTER, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketSourceControl *packet = (BM2MC_PACKET_PacketSourceControl *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, SourceControl, EXECUTE ); \
	packet->zero_pad = (ZERO_PAD); \
	packet->chroma_filter = (CHROMA_FILTER); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_DestinationFeeder( BUFFER, PLANE, COLOR, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketDestinationFeeder *packet = (BM2MC_PACKET_PacketDestinationFeeder *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, DestinationFeeder, EXECUTE ); \
	packet->plane = (PLANE); \
	packet->color = (COLOR); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_DestinationColor( BUFFER, COLOR, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketDestinationColor *packet = (BM2MC_PACKET_PacketDestinationColor *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, DestinationColor, EXECUTE ); \
	packet->color = (COLOR); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_DestinationNone( BUFFER, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketDestinationNone *packet = (BM2MC_PACKET_PacketDestinationNone *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, DestinationNone, EXECUTE ); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_DestinationControl( BUFFER, ZERO_PAD, CHROMA_FILTER, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketDestinationControl *packet = (BM2MC_PACKET_PacketDestinationControl *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, DestinationControl, EXECUTE ); \
	packet->zero_pad = (ZERO_PAD); \
	packet->chroma_filter = (CHROMA_FILTER); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_OutputFeeder( BUFFER, PLANE, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketOutputFeeder *packet = (BM2MC_PACKET_PacketOutputFeeder *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, OutputFeeder, EXECUTE ); \
	packet->plane = (PLANE); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_OutputControl( BUFFER, DITHER, CHROMA_FILTER, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketOutputControl *packet = (BM2MC_PACKET_PacketOutputControl *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, OutputControl, EXECUTE ); \
	packet->dither = (DITHER); \
	packet->chroma_filter = (CHROMA_FILTER); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_Blend( BUFFER, COLOR_BLEND, ALPHA_BLEND, COLOR, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketBlend *packet = (BM2MC_PACKET_PacketBlend *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, Blend, EXECUTE ); \
	packet->color_blend = (COLOR_BLEND); \
	packet->alpha_blend = (ALPHA_BLEND); \
	packet->color = (COLOR); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_BlendColor( BUFFER, COLOR, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketBlendColor *packet = (BM2MC_PACKET_PacketBlendColor *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, BlendColor, EXECUTE ); \
	packet->color = (COLOR); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_Rop( BUFFER, ROP, PATTERN0, PATTERN1, COLOR0, COLOR1, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketRop *packet = (BM2MC_PACKET_PacketRop *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, Rop, EXECUTE ); \
	packet->rop = (ROP); \
	packet->pattern0 = (PATTERN0); \
	packet->pattern1 = (PATTERN1); \
	packet->color0 = (COLOR0); \
	packet->color1 = (COLOR1); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_SourceColorkey( BUFFER, HIGH, LOW, MASK, REPLACEMENT, REPLACEMENT_MASK, EXCLUSIVE, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketSourceColorkey *packet = (BM2MC_PACKET_PacketSourceColorkey *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, SourceColorkey, EXECUTE ); \
	packet->high = (HIGH); \
	packet->low = (LOW); \
	packet->mask = (MASK); \
	packet->replacement = (REPLACEMENT); \
	packet->replacement_mask = (REPLACEMENT_MASK); \
	packet->exclusive = (EXCLUSIVE); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_SourceColorkeyEnable( BUFFER, ENABLE, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketSourceColorkeyEnable *packet = (BM2MC_PACKET_PacketSourceColorkeyEnable *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, SourceColorkeyEnable, EXECUTE ); \
	packet->enable = (ENABLE); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_DestinationColorkey( BUFFER, HIGH, LOW, MASK, REPLACEMENT, REPLACEMENT_MASK, EXCLUSIVE, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketDestinationColorkey *packet = (BM2MC_PACKET_PacketDestinationColorkey *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, DestinationColorkey, EXECUTE ); \
	packet->high = (HIGH); \
	packet->low = (LOW); \
	packet->mask = (MASK); \
	packet->replacement = (REPLACEMENT); \
	packet->replacement_mask = (REPLACEMENT_MASK); \
	packet->exclusive = (EXCLUSIVE); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_DestinationColorkeyEnable( BUFFER, ENABLE, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketDestinationColorkeyEnable *packet = (BM2MC_PACKET_PacketDestinationColorkeyEnable *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, DestinationColorkeyEnable, EXECUTE ); \
	packet->enable = (ENABLE); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_Filter( BUFFER, HOR, VER, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketFilter *packet = (BM2MC_PACKET_PacketFilter *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, Filter, EXECUTE ); \
	packet->hor = (HOR); \
	packet->ver = (VER); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_FilterEnable( BUFFER, ENABLE, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketFilterEnable *packet = (BM2MC_PACKET_PacketFilterEnable *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, FilterEnable, EXECUTE ); \
	packet->enable = (ENABLE); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_FilterControl( BUFFER, ROUNDING, ADJUST_COLOR, SUB_ALPHA, ORDER, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketFilterControl *packet = (BM2MC_PACKET_PacketFilterControl *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, FilterControl, EXECUTE ); \
	packet->rounding = (ROUNDING); \
	packet->adjust_color = (ADJUST_COLOR); \
	packet->sub_alpha = (SUB_ALPHA); \
	packet->order = (ORDER); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_SourceColorMatrix( BUFFER, MATRIX, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketSourceColorMatrix *packet = (BM2MC_PACKET_PacketSourceColorMatrix *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, SourceColorMatrix, EXECUTE ); \
	packet->matrix = (MATRIX); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_SourceColorMatrixEnable( BUFFER, ENABLE, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketSourceColorMatrixEnable *packet = (BM2MC_PACKET_PacketSourceColorMatrixEnable *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, SourceColorMatrixEnable, EXECUTE ); \
	packet->enable = (ENABLE); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_SourcePalette( BUFFER, ADDRESS, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketSourcePalette *packet = (BM2MC_PACKET_PacketSourcePalette *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, SourcePalette, EXECUTE ); \
	packet->address = (ADDRESS); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_AlphaPremultiply( BUFFER, ENABLE, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketAlphaPremultiply *packet = (BM2MC_PACKET_PacketAlphaPremultiply *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, AlphaPremultiply, EXECUTE ); \
	packet->enable = (ENABLE); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_Mirror( BUFFER, SRC_HOR, SRC_VER, DST_HOR, DST_VER, OUT_HOR, OUT_VER, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketMirror *packet = (BM2MC_PACKET_PacketMirror *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, Mirror, EXECUTE ); \
	packet->src_hor = (SRC_HOR); \
	packet->src_ver = (SRC_VER); \
	packet->dst_hor = (DST_HOR); \
	packet->dst_ver = (DST_VER); \
	packet->out_hor = (OUT_HOR); \
	packet->out_ver = (OUT_VER); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_FixedScale( BUFFER, HPHASE, VPHASE, HSTEP, VSTEP, SHIFT, EXECUTE ) do \
{ \
    BM2MC_PACKET_PacketFixedScale *packet = (BM2MC_PACKET_PacketFixedScale *) (BUFFER); \
    BM2MC_PACKET_INIT( packet, FixedScale, EXECUTE ); \
    packet->hor_phase = (HPHASE); \
    packet->ver_phase = (VPHASE); \
    packet->hor_step =  (HSTEP); \
    packet->ver_step =  (VSTEP); \
    packet->shift =     (SHIFT); \
    BM2MC_PACKET_TERM( packet ); \
    (BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_FillBlit( BUFFER, RECT, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketFillBlit *packet = (BM2MC_PACKET_PacketFillBlit *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, FillBlit, EXECUTE ); \
	packet->rect = (RECT); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_CopyBlit( BUFFER, SRC_RECT, OUT_POINT, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketCopyBlit *packet = (BM2MC_PACKET_PacketCopyBlit *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, CopyBlit, EXECUTE ); \
	packet->src_rect = (SRC_RECT); \
	packet->out_point = (OUT_POINT); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_BlendBlit( BUFFER, SRC_RECT, OUT_POINT, DST_POINT, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketBlendBlit *packet = (BM2MC_PACKET_PacketBlendBlit *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, BlendBlit, EXECUTE ); \
	packet->src_rect = (SRC_RECT); \
	packet->out_point = (OUT_POINT); \
	packet->dst_point = (DST_POINT); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_ScaleBlit( BUFFER, SRC_RECT, OUT_RECT, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketScaleBlit *packet = (BM2MC_PACKET_PacketScaleBlit *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, ScaleBlit, EXECUTE ); \
	packet->src_rect = (SRC_RECT); \
	packet->out_rect = (OUT_RECT); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_ScaleBlendBlit( BUFFER, SRC_RECT, OUT_RECT, DST_POINT, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketScaleBlendBlit *packet = (BM2MC_PACKET_PacketScaleBlendBlit *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, ScaleBlendBlit, EXECUTE ); \
	packet->src_rect = (SRC_RECT); \
	packet->out_rect = (OUT_RECT); \
	packet->dst_point = (DST_POINT); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_UpdateScaleBlit( BUFFER, SRC_RECT, OUT_RECT, UPDATE_RECT, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketUpdateScaleBlit *packet = (BM2MC_PACKET_PacketUpdateScaleBlit *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, UpdateScaleBlit, EXECUTE ); \
	packet->src_rect = (SRC_RECT); \
	packet->out_rect = (OUT_RECT); \
	packet->update_rect = (UPDATE_RECT); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_DestripeBlit( BUFFER, SRC_RECT, OUT_RECT, DST_POINT, SOURCE_STRIPE_WIDTH, LUMA_STRIPE_HEIGHT, CHROMA_STRIPE_HEIGHT, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketDestripeBlit *packet = (BM2MC_PACKET_PacketDestripeBlit *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, DestripeBlit, EXECUTE ); \
	packet->src_rect = (SRC_RECT); \
	packet->out_rect = (OUT_RECT); \
	packet->dst_point = (DST_POINT); \
	packet->source_stripe_width = (SOURCE_STRIPE_WIDTH); \
	packet->luma_stripe_height = (LUMA_STRIPE_HEIGHT); \
	packet->chroma_stripe_height = (CHROMA_STRIPE_HEIGHT); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_ResetState( BUFFER, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketResetState *packet = (BM2MC_PACKET_PacketResetState *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, ResetState, EXECUTE ); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_SaveState( BUFFER, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketSaveState *packet = (BM2MC_PACKET_PacketSaveState *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, SaveState, EXECUTE ); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/
#define BM2MC_PACKET_WRITE_RestoreState( BUFFER, EXECUTE ) do \
{ \
	BM2MC_PACKET_PacketRestoreState *packet = (BM2MC_PACKET_PacketRestoreState *) (BUFFER); \
	BM2MC_PACKET_INIT( packet, RestoreState, EXECUTE ); \
	BM2MC_PACKET_TERM( packet ); \
	(BUFFER) = (void *) packet; \
} \
while(0)

/***************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* BM2MC_PACKET_H__ */

/* end of file */
