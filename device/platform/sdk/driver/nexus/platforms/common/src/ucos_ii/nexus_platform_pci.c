/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: nexus_platform_pci.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 8/20/11 7:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/97400/src/ucos_ii/nexus_platform_pci.c $
 * 
 * 1   8/20/11 7:40p agin
 * SWNOOS-479:  Add playpump support from a SATA drive.
 * 
 * 1   7/25/11 4:42p agin
 * SWNOOS-476:  Relevel 97346 no-os to 20110719 Phase 3.0.
 * 
 * 4   2/10/09 12:37p shyam
 * PR51749 : Update PCI to support enalbing the PCI slot
 * 
 * 3   1/26/09 1:55p erickson
 * PR51468: global variable naming convention
 *
 * 2   4/4/08 4:35p erickson
 * PR41382: check fp for errors, close fp
 *
 * 1   3/29/08 10:51p shyam
 * PR 39899 : Add PCI files for detecting transcoder
 *
 * PR39899/1   2/27/08 5:52p shyam
 * PR 39899 : Integrate 7043 XCD into Nexus
 *
 *****************************************************************************/

#if 0
#include "nexus_types.h"
#include "nexus_base.h"

#include "nexus_platform_pci.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


/* The Pci structure holds the pci info of the matching device */
#define NUM_BARS        7
static struct tagPciInfo {
    int bus;
    int deviceFunction;
    unsigned int venDevId;
    int irq;
    uint32_t bar[NUM_BARS];
    uint32_t barSize[NUM_BARS];

    int fdCfgReg ;

} g_NEXUS_pciInfo;


#define PCI_SLOT(devfn)		(((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)		((devfn) & 0x07)

#define NEXUS_PCI_COMMAND_REG			0x04
#define NEXUS_PCI_COMMAND_IO			0x4
#define NEXUS_PCI_COMMAND_MEMORY		0x2
#define NEXUS_PCI_COMMAND_MASTER		0x1

BDBG_MODULE(nexus_platform_pci);

/***************************************************************************
Summary:
    Detect a PCI device with specified VendorId-DeviceId and make it
    current
***************************************************************************/
NEXUS_Error NEXUS_Platform_PCI_P_Detect(uint32_t venDevId)
{
    int i, entrysize ;
    unsigned int temp;
    char *ptr, *end, *tempEnd;
    uint32_t curVenDevId;
    char buf[512];

    FILE *fp = fopen("/proc/bus/pci/devices", "r");
    if (!fp) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    /* The proc devices format as follows
    BusDevFn VenIdDevId IRQ Bar0 Bar1 ... Bar7 BarSize0 .. BarSize7 DrvName
    */
    while( (entrysize = fgets(buf,512,fp) != 0 ) )
    {
        ptr = &buf[0];
        end = tempEnd = strchr( ptr, '\t' );
        temp = (unsigned int)strtoul( ptr, &tempEnd, 16 );
        g_NEXUS_pciInfo.bus = (temp >> 8 ) & 0xFF;
        g_NEXUS_pciInfo.deviceFunction = temp & 0xFF;

        ptr = ++end;        /* move past \t */
        end = tempEnd = strchr( ptr, '\t' );
        curVenDevId = (unsigned int)strtoul( ptr, &tempEnd, 16 );
        if(curVenDevId == venDevId)
        {
            g_NEXUS_pciInfo.venDevId = curVenDevId ;

            ptr = ++end;
            end = tempEnd = strchr( ptr, '\t' );
            g_NEXUS_pciInfo.irq = (unsigned int)strtoul( ptr, &tempEnd, 16 );
            BDBG_MSG(("IRQ = %x", g_NEXUS_pciInfo.irq));

            for( i=0; i < NUM_BARS; i++ ) {
                ptr = ++end;
                end = tempEnd = strchr( ptr, '\t' );
                g_NEXUS_pciInfo.bar[i] = (unsigned int)strtoul( ptr, &tempEnd, 16 );
                BDBG_MSG(("Bar[%d] = %08x", g_NEXUS_pciInfo.bar[i]));
            }

            for( i=0; i < NUM_BARS; i++ ) {
                ptr = ++end;
                end = tempEnd = strchr( ptr, '\t' );
                g_NEXUS_pciInfo.barSize[i] = (unsigned int)strtoul( ptr, &tempEnd, 16 );
                BDBG_MSG(("Bar Size[%d] = %08x", g_NEXUS_pciInfo.barSize[i]));
            }
            break ;
        }
    }

    fclose(fp);

    if(!g_NEXUS_pciInfo.venDevId)
       return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return BERR_SUCCESS;
}


/***************************************************************************
Summary:
    Return the BAR information from the current PCI device.
***************************************************************************/
NEXUS_Error NEXUS_Platform_PCI_P_GetBarInfo(uint32_t venDevId,
    unsigned int barNumber,
    uint32_t *pBarBaseAddr,
    uint32_t *pBarSize)
{
    if(!g_NEXUS_pciInfo.venDevId || venDevId != g_NEXUS_pciInfo.venDevId)
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);

    *pBarBaseAddr = g_NEXUS_pciInfo.bar[barNumber] ;
    *pBarSize = g_NEXUS_pciInfo.barSize[barNumber] ;
    return BERR_SUCCESS;
}

uint16_t NEXUS_Platform_PCI_P_ReadConfigurationReg(uint16_t addr)
{
    int rc ;
    uint16_t data ;
    if(lseek(g_NEXUS_pciInfo.fdCfgReg, addr, SEEK_SET) < 0)
    {
        BDBG_ERR(("ReadConfigurationReg : Cannot seek"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    rc = read(g_NEXUS_pciInfo.fdCfgReg, &data, sizeof(uint16_t) ) ;
    if(rc < (int)sizeof(uint16_t))
    {
        BDBG_ERR(("ReadConfigurationReg : read failed"));
        BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return 0 ;
    }
    BDBG_ERR(("data = %08x rc=%d", data, rc));
    return data ;
}

void NEXUS_Platform_PCI_P_WriteConfigurationReg(uint16_t addr, uint16_t data)
{
    int rc ;
    if(lseek(g_NEXUS_pciInfo.fdCfgReg, addr, SEEK_SET) < 0)
    {
        BDBG_ERR(("ReadConfigurationReg : Cannot seek"));
        BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return ;
    }
    rc = write(g_NEXUS_pciInfo.fdCfgReg, &data, 2);
    if(rc < (int)sizeof(uint16_t))
    {
        BDBG_ERR(("WriteConfigurationReg : write failed %d", rc));
        BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return ;
    }
}

/***************************************************************************
Summary:
    Enable the current PCI device.
***************************************************************************/
NEXUS_Error NEXUS_Platform_PCI_P_EnableSlot(uint32_t venDevId)
{
    int slot, func ;
    char buf[256] ;
    uint16_t reg ;
   
    if(!g_NEXUS_pciInfo.venDevId || venDevId != g_NEXUS_pciInfo.venDevId)
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    slot = PCI_SLOT(g_NEXUS_pciInfo.deviceFunction) ;
    func = PCI_FUNC(g_NEXUS_pciInfo.deviceFunction);

    if (snprintf(buf, sizeof(buf), "/proc/bus/pci/%02x/%02x.%d",
        g_NEXUS_pciInfo.bus, slot, func) == sizeof(buf))
    {
        /* File name too long */
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

#error bb
    g_NEXUS_pciInfo.fdCfgReg= open(buf, O_RDWR) ;
    if (g_NEXUS_pciInfo.fdCfgReg < 0)
    {
        BDBG_ERR(("Cannot open %s", buf));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    reg = NEXUS_Platform_PCI_P_ReadConfigurationReg(NEXUS_PCI_COMMAND_REG) ;
    NEXUS_Platform_PCI_P_WriteConfigurationReg(NEXUS_PCI_COMMAND_REG, 
		reg |NEXUS_PCI_COMMAND_IO | NEXUS_PCI_COMMAND_MEMORY | 
		NEXUS_PCI_COMMAND_MASTER) ;
    return NEXUS_SUCCESS ;
}
#endif
