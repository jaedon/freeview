/***************************************************************************
 *     Copyright (c) 1999-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bchp_leap_rom_patch.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/21/11 3:58p $
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on         Mon Nov 21 15:38:47 2011
 *                 MD5 Checksum         e7ea6c23a967c588a901532a5078ba2a
 *
 * Compiled with:  RDB Utility          combo_header.pl
 *                 RDB Parser           3.0
 *                 unknown              unknown
 *                 Perl Interpreter     5.008008
 *                 Operating System     linux
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/chp/3462/rdb/a0/bchp_leap_rom_patch.h $
 * 
 * Hydra_Software_Devel/1   11/21/11 3:58p farshidf
 * SW3461-98: add the 3462 A0 Header files
 *
 ***************************************************************************/

#ifndef BCHP_LEAP_ROM_PATCH_H__
#define BCHP_LEAP_ROM_PATCH_H__

/***************************************************************************
 *LEAP_ROM_PATCH - Rom Patching Configuration
 ***************************************************************************/
#define BCHP_LEAP_ROM_PATCH_CONFIG0              0x00080400 /* Configuration of patch 0 */
#define BCHP_LEAP_ROM_PATCH_VALUE0               0x00080404 /* Value of patch 0 */
#define BCHP_LEAP_ROM_PATCH_CONFIG1              0x00080408 /* Configuration of patch 1 */
#define BCHP_LEAP_ROM_PATCH_VALUE1               0x0008040c /* Value of patch 1 */
#define BCHP_LEAP_ROM_PATCH_CONFIG2              0x00080410 /* Configuration of patch 2 */
#define BCHP_LEAP_ROM_PATCH_VALUE2               0x00080414 /* Value of patch 2 */
#define BCHP_LEAP_ROM_PATCH_CONFIG3              0x00080418 /* Configuration of patch 3 */
#define BCHP_LEAP_ROM_PATCH_VALUE3               0x0008041c /* Value of patch 3 */
#define BCHP_LEAP_ROM_PATCH_CONFIG4              0x00080420 /* Configuration of patch 4 */
#define BCHP_LEAP_ROM_PATCH_VALUE4               0x00080424 /* Value of patch 4 */
#define BCHP_LEAP_ROM_PATCH_CONFIG5              0x00080428 /* Configuration of patch 5 */
#define BCHP_LEAP_ROM_PATCH_VALUE5               0x0008042c /* Value of patch 5 */
#define BCHP_LEAP_ROM_PATCH_CONFIG6              0x00080430 /* Configuration of patch 6 */
#define BCHP_LEAP_ROM_PATCH_VALUE6               0x00080434 /* Value of patch 6 */
#define BCHP_LEAP_ROM_PATCH_CONFIG7              0x00080438 /* Configuration of patch 7 */
#define BCHP_LEAP_ROM_PATCH_VALUE7               0x0008043c /* Value of patch 7 */

/***************************************************************************
 *CONFIG0 - Configuration of patch 0
 ***************************************************************************/
/* LEAP_ROM_PATCH :: CONFIG0 :: reserved0 [31:17] */
#define BCHP_LEAP_ROM_PATCH_CONFIG0_reserved0_MASK                 0xfffe0000
#define BCHP_LEAP_ROM_PATCH_CONFIG0_reserved0_SHIFT                17

/* LEAP_ROM_PATCH :: CONFIG0 :: ENABLE [16:16] */
#define BCHP_LEAP_ROM_PATCH_CONFIG0_ENABLE_MASK                    0x00010000
#define BCHP_LEAP_ROM_PATCH_CONFIG0_ENABLE_SHIFT                   16
#define BCHP_LEAP_ROM_PATCH_CONFIG0_ENABLE_DEFAULT                 0

/* LEAP_ROM_PATCH :: CONFIG0 :: ADDR [15:02] */
#define BCHP_LEAP_ROM_PATCH_CONFIG0_ADDR_MASK                      0x0000fffc
#define BCHP_LEAP_ROM_PATCH_CONFIG0_ADDR_SHIFT                     2
#define BCHP_LEAP_ROM_PATCH_CONFIG0_ADDR_DEFAULT                   0

/* LEAP_ROM_PATCH :: CONFIG0 :: reserved1 [01:00] */
#define BCHP_LEAP_ROM_PATCH_CONFIG0_reserved1_MASK                 0x00000003
#define BCHP_LEAP_ROM_PATCH_CONFIG0_reserved1_SHIFT                0

/***************************************************************************
 *VALUE0 - Value of patch 0
 ***************************************************************************/
/* LEAP_ROM_PATCH :: VALUE0 :: VALUE [31:00] */
#define BCHP_LEAP_ROM_PATCH_VALUE0_VALUE_MASK                      0xffffffff
#define BCHP_LEAP_ROM_PATCH_VALUE0_VALUE_SHIFT                     0
#define BCHP_LEAP_ROM_PATCH_VALUE0_VALUE_DEFAULT                   0

/***************************************************************************
 *CONFIG1 - Configuration of patch 1
 ***************************************************************************/
/* LEAP_ROM_PATCH :: CONFIG1 :: reserved0 [31:17] */
#define BCHP_LEAP_ROM_PATCH_CONFIG1_reserved0_MASK                 0xfffe0000
#define BCHP_LEAP_ROM_PATCH_CONFIG1_reserved0_SHIFT                17

/* LEAP_ROM_PATCH :: CONFIG1 :: ENABLE [16:16] */
#define BCHP_LEAP_ROM_PATCH_CONFIG1_ENABLE_MASK                    0x00010000
#define BCHP_LEAP_ROM_PATCH_CONFIG1_ENABLE_SHIFT                   16
#define BCHP_LEAP_ROM_PATCH_CONFIG1_ENABLE_DEFAULT                 0

/* LEAP_ROM_PATCH :: CONFIG1 :: ADDR [15:02] */
#define BCHP_LEAP_ROM_PATCH_CONFIG1_ADDR_MASK                      0x0000fffc
#define BCHP_LEAP_ROM_PATCH_CONFIG1_ADDR_SHIFT                     2
#define BCHP_LEAP_ROM_PATCH_CONFIG1_ADDR_DEFAULT                   0

/* LEAP_ROM_PATCH :: CONFIG1 :: reserved1 [01:00] */
#define BCHP_LEAP_ROM_PATCH_CONFIG1_reserved1_MASK                 0x00000003
#define BCHP_LEAP_ROM_PATCH_CONFIG1_reserved1_SHIFT                0

/***************************************************************************
 *VALUE1 - Value of patch 1
 ***************************************************************************/
/* LEAP_ROM_PATCH :: VALUE1 :: VALUE [31:00] */
#define BCHP_LEAP_ROM_PATCH_VALUE1_VALUE_MASK                      0xffffffff
#define BCHP_LEAP_ROM_PATCH_VALUE1_VALUE_SHIFT                     0
#define BCHP_LEAP_ROM_PATCH_VALUE1_VALUE_DEFAULT                   0

/***************************************************************************
 *CONFIG2 - Configuration of patch 2
 ***************************************************************************/
/* LEAP_ROM_PATCH :: CONFIG2 :: reserved0 [31:17] */
#define BCHP_LEAP_ROM_PATCH_CONFIG2_reserved0_MASK                 0xfffe0000
#define BCHP_LEAP_ROM_PATCH_CONFIG2_reserved0_SHIFT                17

/* LEAP_ROM_PATCH :: CONFIG2 :: ENABLE [16:16] */
#define BCHP_LEAP_ROM_PATCH_CONFIG2_ENABLE_MASK                    0x00010000
#define BCHP_LEAP_ROM_PATCH_CONFIG2_ENABLE_SHIFT                   16
#define BCHP_LEAP_ROM_PATCH_CONFIG2_ENABLE_DEFAULT                 0

/* LEAP_ROM_PATCH :: CONFIG2 :: ADDR [15:02] */
#define BCHP_LEAP_ROM_PATCH_CONFIG2_ADDR_MASK                      0x0000fffc
#define BCHP_LEAP_ROM_PATCH_CONFIG2_ADDR_SHIFT                     2
#define BCHP_LEAP_ROM_PATCH_CONFIG2_ADDR_DEFAULT                   0

/* LEAP_ROM_PATCH :: CONFIG2 :: reserved1 [01:00] */
#define BCHP_LEAP_ROM_PATCH_CONFIG2_reserved1_MASK                 0x00000003
#define BCHP_LEAP_ROM_PATCH_CONFIG2_reserved1_SHIFT                0

/***************************************************************************
 *VALUE2 - Value of patch 2
 ***************************************************************************/
/* LEAP_ROM_PATCH :: VALUE2 :: VALUE [31:00] */
#define BCHP_LEAP_ROM_PATCH_VALUE2_VALUE_MASK                      0xffffffff
#define BCHP_LEAP_ROM_PATCH_VALUE2_VALUE_SHIFT                     0
#define BCHP_LEAP_ROM_PATCH_VALUE2_VALUE_DEFAULT                   0

/***************************************************************************
 *CONFIG3 - Configuration of patch 3
 ***************************************************************************/
/* LEAP_ROM_PATCH :: CONFIG3 :: reserved0 [31:17] */
#define BCHP_LEAP_ROM_PATCH_CONFIG3_reserved0_MASK                 0xfffe0000
#define BCHP_LEAP_ROM_PATCH_CONFIG3_reserved0_SHIFT                17

/* LEAP_ROM_PATCH :: CONFIG3 :: ENABLE [16:16] */
#define BCHP_LEAP_ROM_PATCH_CONFIG3_ENABLE_MASK                    0x00010000
#define BCHP_LEAP_ROM_PATCH_CONFIG3_ENABLE_SHIFT                   16
#define BCHP_LEAP_ROM_PATCH_CONFIG3_ENABLE_DEFAULT                 0

/* LEAP_ROM_PATCH :: CONFIG3 :: ADDR [15:02] */
#define BCHP_LEAP_ROM_PATCH_CONFIG3_ADDR_MASK                      0x0000fffc
#define BCHP_LEAP_ROM_PATCH_CONFIG3_ADDR_SHIFT                     2
#define BCHP_LEAP_ROM_PATCH_CONFIG3_ADDR_DEFAULT                   0

/* LEAP_ROM_PATCH :: CONFIG3 :: reserved1 [01:00] */
#define BCHP_LEAP_ROM_PATCH_CONFIG3_reserved1_MASK                 0x00000003
#define BCHP_LEAP_ROM_PATCH_CONFIG3_reserved1_SHIFT                0

/***************************************************************************
 *VALUE3 - Value of patch 3
 ***************************************************************************/
/* LEAP_ROM_PATCH :: VALUE3 :: VALUE [31:00] */
#define BCHP_LEAP_ROM_PATCH_VALUE3_VALUE_MASK                      0xffffffff
#define BCHP_LEAP_ROM_PATCH_VALUE3_VALUE_SHIFT                     0
#define BCHP_LEAP_ROM_PATCH_VALUE3_VALUE_DEFAULT                   0

/***************************************************************************
 *CONFIG4 - Configuration of patch 4
 ***************************************************************************/
/* LEAP_ROM_PATCH :: CONFIG4 :: reserved0 [31:17] */
#define BCHP_LEAP_ROM_PATCH_CONFIG4_reserved0_MASK                 0xfffe0000
#define BCHP_LEAP_ROM_PATCH_CONFIG4_reserved0_SHIFT                17

/* LEAP_ROM_PATCH :: CONFIG4 :: ENABLE [16:16] */
#define BCHP_LEAP_ROM_PATCH_CONFIG4_ENABLE_MASK                    0x00010000
#define BCHP_LEAP_ROM_PATCH_CONFIG4_ENABLE_SHIFT                   16
#define BCHP_LEAP_ROM_PATCH_CONFIG4_ENABLE_DEFAULT                 0

/* LEAP_ROM_PATCH :: CONFIG4 :: ADDR [15:02] */
#define BCHP_LEAP_ROM_PATCH_CONFIG4_ADDR_MASK                      0x0000fffc
#define BCHP_LEAP_ROM_PATCH_CONFIG4_ADDR_SHIFT                     2
#define BCHP_LEAP_ROM_PATCH_CONFIG4_ADDR_DEFAULT                   0

/* LEAP_ROM_PATCH :: CONFIG4 :: reserved1 [01:00] */
#define BCHP_LEAP_ROM_PATCH_CONFIG4_reserved1_MASK                 0x00000003
#define BCHP_LEAP_ROM_PATCH_CONFIG4_reserved1_SHIFT                0

/***************************************************************************
 *VALUE4 - Value of patch 4
 ***************************************************************************/
/* LEAP_ROM_PATCH :: VALUE4 :: VALUE [31:00] */
#define BCHP_LEAP_ROM_PATCH_VALUE4_VALUE_MASK                      0xffffffff
#define BCHP_LEAP_ROM_PATCH_VALUE4_VALUE_SHIFT                     0
#define BCHP_LEAP_ROM_PATCH_VALUE4_VALUE_DEFAULT                   0

/***************************************************************************
 *CONFIG5 - Configuration of patch 5
 ***************************************************************************/
/* LEAP_ROM_PATCH :: CONFIG5 :: reserved0 [31:17] */
#define BCHP_LEAP_ROM_PATCH_CONFIG5_reserved0_MASK                 0xfffe0000
#define BCHP_LEAP_ROM_PATCH_CONFIG5_reserved0_SHIFT                17

/* LEAP_ROM_PATCH :: CONFIG5 :: ENABLE [16:16] */
#define BCHP_LEAP_ROM_PATCH_CONFIG5_ENABLE_MASK                    0x00010000
#define BCHP_LEAP_ROM_PATCH_CONFIG5_ENABLE_SHIFT                   16
#define BCHP_LEAP_ROM_PATCH_CONFIG5_ENABLE_DEFAULT                 0

/* LEAP_ROM_PATCH :: CONFIG5 :: ADDR [15:02] */
#define BCHP_LEAP_ROM_PATCH_CONFIG5_ADDR_MASK                      0x0000fffc
#define BCHP_LEAP_ROM_PATCH_CONFIG5_ADDR_SHIFT                     2
#define BCHP_LEAP_ROM_PATCH_CONFIG5_ADDR_DEFAULT                   0

/* LEAP_ROM_PATCH :: CONFIG5 :: reserved1 [01:00] */
#define BCHP_LEAP_ROM_PATCH_CONFIG5_reserved1_MASK                 0x00000003
#define BCHP_LEAP_ROM_PATCH_CONFIG5_reserved1_SHIFT                0

/***************************************************************************
 *VALUE5 - Value of patch 5
 ***************************************************************************/
/* LEAP_ROM_PATCH :: VALUE5 :: VALUE [31:00] */
#define BCHP_LEAP_ROM_PATCH_VALUE5_VALUE_MASK                      0xffffffff
#define BCHP_LEAP_ROM_PATCH_VALUE5_VALUE_SHIFT                     0
#define BCHP_LEAP_ROM_PATCH_VALUE5_VALUE_DEFAULT                   0

/***************************************************************************
 *CONFIG6 - Configuration of patch 6
 ***************************************************************************/
/* LEAP_ROM_PATCH :: CONFIG6 :: reserved0 [31:17] */
#define BCHP_LEAP_ROM_PATCH_CONFIG6_reserved0_MASK                 0xfffe0000
#define BCHP_LEAP_ROM_PATCH_CONFIG6_reserved0_SHIFT                17

/* LEAP_ROM_PATCH :: CONFIG6 :: ENABLE [16:16] */
#define BCHP_LEAP_ROM_PATCH_CONFIG6_ENABLE_MASK                    0x00010000
#define BCHP_LEAP_ROM_PATCH_CONFIG6_ENABLE_SHIFT                   16
#define BCHP_LEAP_ROM_PATCH_CONFIG6_ENABLE_DEFAULT                 0

/* LEAP_ROM_PATCH :: CONFIG6 :: ADDR [15:02] */
#define BCHP_LEAP_ROM_PATCH_CONFIG6_ADDR_MASK                      0x0000fffc
#define BCHP_LEAP_ROM_PATCH_CONFIG6_ADDR_SHIFT                     2
#define BCHP_LEAP_ROM_PATCH_CONFIG6_ADDR_DEFAULT                   0

/* LEAP_ROM_PATCH :: CONFIG6 :: reserved1 [01:00] */
#define BCHP_LEAP_ROM_PATCH_CONFIG6_reserved1_MASK                 0x00000003
#define BCHP_LEAP_ROM_PATCH_CONFIG6_reserved1_SHIFT                0

/***************************************************************************
 *VALUE6 - Value of patch 6
 ***************************************************************************/
/* LEAP_ROM_PATCH :: VALUE6 :: VALUE [31:00] */
#define BCHP_LEAP_ROM_PATCH_VALUE6_VALUE_MASK                      0xffffffff
#define BCHP_LEAP_ROM_PATCH_VALUE6_VALUE_SHIFT                     0
#define BCHP_LEAP_ROM_PATCH_VALUE6_VALUE_DEFAULT                   0

/***************************************************************************
 *CONFIG7 - Configuration of patch 7
 ***************************************************************************/
/* LEAP_ROM_PATCH :: CONFIG7 :: reserved0 [31:17] */
#define BCHP_LEAP_ROM_PATCH_CONFIG7_reserved0_MASK                 0xfffe0000
#define BCHP_LEAP_ROM_PATCH_CONFIG7_reserved0_SHIFT                17

/* LEAP_ROM_PATCH :: CONFIG7 :: ENABLE [16:16] */
#define BCHP_LEAP_ROM_PATCH_CONFIG7_ENABLE_MASK                    0x00010000
#define BCHP_LEAP_ROM_PATCH_CONFIG7_ENABLE_SHIFT                   16
#define BCHP_LEAP_ROM_PATCH_CONFIG7_ENABLE_DEFAULT                 0

/* LEAP_ROM_PATCH :: CONFIG7 :: ADDR [15:02] */
#define BCHP_LEAP_ROM_PATCH_CONFIG7_ADDR_MASK                      0x0000fffc
#define BCHP_LEAP_ROM_PATCH_CONFIG7_ADDR_SHIFT                     2
#define BCHP_LEAP_ROM_PATCH_CONFIG7_ADDR_DEFAULT                   0

/* LEAP_ROM_PATCH :: CONFIG7 :: reserved1 [01:00] */
#define BCHP_LEAP_ROM_PATCH_CONFIG7_reserved1_MASK                 0x00000003
#define BCHP_LEAP_ROM_PATCH_CONFIG7_reserved1_SHIFT                0

/***************************************************************************
 *VALUE7 - Value of patch 7
 ***************************************************************************/
/* LEAP_ROM_PATCH :: VALUE7 :: VALUE [31:00] */
#define BCHP_LEAP_ROM_PATCH_VALUE7_VALUE_MASK                      0xffffffff
#define BCHP_LEAP_ROM_PATCH_VALUE7_VALUE_SHIFT                     0
#define BCHP_LEAP_ROM_PATCH_VALUE7_VALUE_DEFAULT                   0

#endif /* #ifndef BCHP_LEAP_ROM_PATCH_H__ */

/* End of File */