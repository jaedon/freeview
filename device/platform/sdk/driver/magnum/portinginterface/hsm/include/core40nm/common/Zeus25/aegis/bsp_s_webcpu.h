/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsp_s_webcpu.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 4/2/12 10:13a $
 * Security Processor Code for Register Access
 ***************************************************************************/

#ifndef BSP_S_WEBCPU_H__
#define BSP_S_WEBCPU_H__

#include "bsp_s_hmac_sha1.h"
#include "bsp_s_mem_auth.h"

/* BCMD_MICH_RESIZE command is below sandbox config defines */

/*

BCMD_WEBCPU_SANDBOX_CONFIG_ExecuteCommand - Command used to configure the
    various regions/address/bus checkers prior to letting the WebCPU (CORE_1)
    out of reset. If the WebCPU OTP is set indicating it must run in sandbox
    mode, this command and MEM_AUTH command must BOTH be executed before
    letting CORE_1 out of reset.

Parameters passed:
eGisbRangeCount    - number of GISB regions to configure (0-8)
eGisbRangeXControl - GISB mode select register for WebCPU region X [0:7]
eGisbRangeXStart   - GISB end address register for WebCPU region X
eGisbRangeXEnd     - GISB start address register for WebCPU region X
eGisbRangeXClients - GISB client select register for WebCPU region X
eBiuArchXControl   - BIU ARCH blocker control register for WebCPU region X [0-3]
eBiuArchXStartAddr - BIU ARCH blocker start address register for WebCPU region X
eBiuArchXEndAddr   - BIU ARCH blocker end address register for WebCPU region X
eBiuArchXRights    - BIU ARCH blocker permissions register for WebCPU region X
eWebCpuClients0    - Clients bitmap for MEMC (Bits 00-31)
eWebCpuClients1    - Clients bitmap for MEMC (Bits 32-63)
eWebCpuClients2    - Clients bitmap for MEMC (Bits 64-95)
eWebCpuClients3    - Clients bitmap for MEMC (Bits 96-127)
eMemcArchXContol   - Control reg for WebCPU MEMC Arch for region X [0-3]
eMemcArchXStartAddr- Start address for WebCPU MEMC Arch for region X
eMemcArchXEndAddr  - End address for WebCPU MEMC Arch for region X

Returned:
eStatus - Status of command.

BCMD_SandboxConfig_InCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0              | 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           | unused            | unused            |  eGisbRangeCount  | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange0Control   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange0StartAddr - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange0EndAddr   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange0Clients   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange1Control   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange1StartAddr - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange1EndAddr   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange1Clients   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange2Control   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange2StartAddr - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange2EndAddr   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange2Clients   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange3Control   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange3StartAddr - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange3EndAddr   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange3Clients   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange4Control   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange4StartAddr - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange4EndAddr   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange4Clients   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange5Control   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange5StartAddr - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange5EndAddr   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange5Clients   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange6Control   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange6StartAddr - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange6EndAddr   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange6Clients   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange7Control   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange7StartAddr - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange7EndAddr   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eGisbRange7Clients   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch0Control     - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch0StartAddr   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch0EndAddr     - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch0Rights      - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch1Control     - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch1StartAddr   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch1EndAddr     - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch1Rights      - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch2Control     - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch2StartAddr   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch2EndAddr     - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch2Rights      - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch3Control     - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch3StartAddr   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch3EndAddr     - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eBuiArch3Rights      - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eWebCpuScbClients0   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eWebCpuScbClients1   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eWebCpuScbClients2   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eWebCpuScbClients3   - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eWebCpuReserved0     - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eWebCpuReserved1     - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eWebCpuReserved2     - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eWebCpuReserved3     - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eMemcArch0Control    - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eMemcArch0StartAddr  - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eMemcArch0EndAddr    - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eMemcArch1Control    - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eMemcArch1StartAddr  - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eMemcArch1EndAddr    - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eMemcArch2Control    - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eMemcArch2StartAddr  - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eMemcArch2EndAddr    - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eMemcArch3Control    - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eMemcArch3StartAddr  - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 
|  eMemcArch3EndAddr    - 32                                                    | 
|-------------------|-------------------|-------------------|-------------------| 


BCMD_SandboxConfig_OutCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0              | 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           |  unused           |  unused           | eStatus -    8    |
|-------------------|-------------------|-------------------|-------------------| 

*/
typedef enum BCMD_SandboxConfig_InCmdField_e
{

    BCMD_SandboxConfig_InCmdField_eGisbRangeCount      = ( 5 << 2) + 3, /* 8-bit index */
    BCMD_SandboxConfig_InCmdField_eGisbRange0Control   = ( 6 << 2),     /* 32-bit index */
    BCMD_SandboxConfig_InCmdField_eGisbRange0StartAddr = ( 7 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange0EndAddr   = ( 8 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange0Clients   = ( 9 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange1Control   = (10 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange1StartAddr = (11 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange1EndAddr   = (12 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange1Clients   = (13 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange2Control   = (14 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange2StartAddr = (15 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange2EndAddr   = (16 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange2Clients   = (17 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange3Control   = (18 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange3StartAddr = (19 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange3EndAddr   = (20 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange3Clients   = (21 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange4Control   = (22 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange4StartAddr = (23 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange4EndAddr   = (24 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange4Clients   = (25 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange5Control   = (26 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange5StartAddr = (27 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange5EndAddr   = (28 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange5Clients   = (29 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange6Control   = (30 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange6StartAddr = (31 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange6EndAddr   = (32 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange6Clients   = (33 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange7Control   = (34 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange7StartAddr = (35 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange7EndAddr   = (36 << 2),
    BCMD_SandboxConfig_InCmdField_eGisbRange7Clients   = (37 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch0Control     = (38 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch0StartAddr   = (39 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch0EndAddr     = (40 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch0Rights      = (41 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch1Control     = (42 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch1StartAddr   = (43 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch1EndAddr     = (44 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch1Rights      = (45 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch2Control     = (46 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch2StartAddr   = (47 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch2EndAddr     = (48 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch2Rights      = (49 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch3Control     = (50 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch3StartAddr   = (51 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch3EndAddr     = (52 << 2),
    BCMD_SandboxConfig_InCmdField_eBuiArch3Rights      = (53 << 2),
    BCMD_SandboxConfig_InCmdField_eWebCpuScbClients0   = (54 << 2),
    BCMD_SandboxConfig_InCmdField_eWebCpuScbClients1   = (55 << 2),
    BCMD_SandboxConfig_InCmdField_eWebCpuScbClients2   = (56 << 2),
    BCMD_SandboxConfig_InCmdField_eWebCpuScbClients3   = (57 << 2),
    BCMD_SandboxConfig_InCmdField_eWebCpuReserved0     = (58 << 2),
    BCMD_SandboxConfig_InCmdField_eWebCpuReserved1     = (59 << 2),
    BCMD_SandboxConfig_InCmdField_eWebCpuReserved2     = (60 << 2),
    BCMD_SandboxConfig_InCmdField_eWebCpuReserved3     = (61 << 2),
    BCMD_SandboxConfig_InCmdField_eMemcArch0Control    = (62 << 2),
    BCMD_SandboxConfig_InCmdField_eMemcArch0StartAddr  = (63 << 2),
    BCMD_SandboxConfig_InCmdField_eMemcArch0EndAddr    = (64 << 2),
    BCMD_SandboxConfig_InCmdField_eMemcArch1Control    = (65 << 2),
    BCMD_SandboxConfig_InCmdField_eMemcArch1StartAddr  = (66 << 2),
    BCMD_SandboxConfig_InCmdField_eMemcArch1EndAddr    = (67 << 2),
    BCMD_SandboxConfig_InCmdField_eMemcArch2Control    = (68 << 2),
    BCMD_SandboxConfig_InCmdField_eMemcArch2StartAddr  = (69 << 2),
    BCMD_SandboxConfig_InCmdField_eMemcArch2EndAddr    = (70 << 2),
    BCMD_SandboxConfig_InCmdField_eMemcArch3Control    = (71 << 2),
    BCMD_SandboxConfig_InCmdField_eMemcArch3StartAddr  = (72 << 2),
    BCMD_SandboxConfig_InCmdField_eMemcArch3EndAddr    = (73 << 2)
} BCMD_SandboxConfig_InCmdField_e;

typedef enum BCMD_SandboxConfig_OutCmdField_e
{
    BCMD_SandboxConfig_OutCmdField_eStatus        = (5<<2) + 3   /* 8 bit index */
} BCMD_SandboxConfig_OutCmdField_e;

/*

BCMD_WEBCPU_MICH_RESIZE_ExecuteCommand - For a given memory region for the
    WebCPU, having been configured and verify by Mem Auth, this command will
    change the boundaries of the instruction checker.

Parameters passed:
eRegionNumber    - MICH region Id (from BPI_HmacSha1_Context_e)
eRegionStartAddr - MICH region start address
eRegionEndAddr   - MICH region end address

Returned:
eStatus - Status of command.

BCMD_MichResize_InCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0              | 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           |  unused           |  unused           | eRegionNumber - 8 |
|-------------------|-------------------|-------------------|-------------------| 
|  eResizeStartAddr - 32                                                        |
|-------------------|-------------------|-------------------|-------------------| 
|  eResizeEndAddr   - 32                                                        |
|-------------------|-------------------|-------------------|-------------------|

BCMD_MichResize_OutCmdField_e: 
|-------------------|-------------------|-------------------|-------------------| 
|  31..24           | 23..16            | 15..8             | 7..0              | 
|-------------------|-------------------|-------------------|-------------------| 
|  unused           |  unused           |  unused           | eStatus -    8    |
|-------------------|-------------------|-------------------|-------------------| 

*/
typedef enum BCMD_MichResize_InCmdField_e
{
    BCMD_MichResize_InCmdField_eRegionNumber    = ( 5 << 2) + 3,  /*  8-bit value */
    BCMD_MichResize_InCmdField_eRegionStartAddr = ( 6 << 2),      /* 32-bit values */
    BCMD_MichResize_InCmdField_eRegionEndAddr   = ( 7 << 2)
} BCMD_MichResize_InCmdField_e;

typedef enum BCMD_MichResize_OutCmdField_e
{
    BCMD_MichResize_OutCmdField_eStatus        = (5<<2) + 3   /* 8 bit index */
} BCMD_MichResize_OutCmdField_e;

#endif /* BSP_S_WEBCPU_H__ */

