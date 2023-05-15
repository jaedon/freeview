/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: app_system_watchdog.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/6/12 6:08p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/audio/app_system_watchdog.c $
 * 
 * Hydra_Software_Devel/2   9/6/12 6:08p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <pthread.h>

#include "bchp.h"
#include "bchp_timer.h"

#define REG_IO_BASE_ADDRESS     0x10000000
#define REG_IO_SIZE             0x10000000
#define MAX_MEM_REGIONS         16
#define INVALID_MEM_REGION      0xFFFFFFFF

#define TIMER_WDTIMEOUT						((unsigned long int)REG_IO_BASE_ADDRESS + (unsigned long int)BCHP_TIMER_WDTIMEOUT)
#define TIMER_WDCMD							((unsigned long int)REG_IO_BASE_ADDRESS + (unsigned long int)BCHP_TIMER_WDCMD)
#define TIMER_TIMER_IE1						((unsigned long int)REG_IO_BASE_ADDRESS + (unsigned long int)BCHP_TIMER_TIMER_IE1)
#define TIMER_WDCTRL						((unsigned long int)REG_IO_BASE_ADDRESS + (unsigned long int)BCHP_TIMER_WDCTRL)

static pthread_mutex_t cs_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct mem_region_t {
    unsigned long base_addr;
    unsigned long size;
    void * virtual_base;
} mem_region_t;

mem_region_t mem_regions[MAX_MEM_REGIONS];

int map_mem_regions(void)
{
    FILE *pfMemMap;
    unsigned long region_size = 0xFFFFFFFF;
    int i, region_count=0;
    int ret;
    char add_str[128], size_str[128];   
    int mem_fd;
	
    memset(&mem_regions, 0, sizeof(mem_region_t) * MAX_MEM_REGIONS);

    /* first mem region is register i/o space */
    mem_fd = open("/dev/mem", O_RDWR|O_SYNC);
    if (mem_fd < 0) {
        printf("Error opening /dev/mem\n");
        return -1;
    }

    mem_regions[region_count].base_addr = REG_IO_BASE_ADDRESS;
    mem_regions[region_count].size = REG_IO_SIZE;
    mem_regions[region_count].virtual_base = mmap(0, mem_regions[region_count].size, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd,
        mem_regions[region_count].base_addr);
    if (mem_regions[region_count].virtual_base == MAP_FAILED) {
        printf("Error in mmap\n");
        return -1;
    }
    
    /* register i/o is first mem region */
    region_count++; 

    pfMemMap = fopen("/proc/bmem","r");
    if (pfMemMap == NULL) {
        printf("Error opening /proc/bmem\n");
        return -1;
    }

    while ((region_size > 0) && (region_count<MAX_MEM_REGIONS))
    {
        ret = fscanf(pfMemMap, "%s %lX %s %lX\n", (char *) &add_str, &mem_regions[region_count].base_addr, 
            (char *) &size_str, &region_size);
        if (ret != 4)
        {
            region_size = 0;
        }
        else if (region_size != 0)
        {
            mem_regions[region_count].virtual_base = mmap(0, region_size, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 
                mem_regions[region_count].base_addr);
            if (mem_regions[region_count].virtual_base == MAP_FAILED) {
                printf("Error in mmap region %d\n", region_count);
                mem_regions[region_count].virtual_base =NULL;
            }
        }
        mem_regions[region_count].size = region_size; 
        region_count++;
    }

    /*printf("Memory Regions:\n");*/
    for (i=0; i<MAX_MEM_REGIONS; i++)
    {
        if (mem_regions[i].size == 0)
        {   
            break;
        }
        /*else
        //{
        //    printf("%d Base:0x%lX Size:0x%lX Virt Base:0x%lX\n", i, mem_regions[i].base_addr, mem_regions[i].size, 
        //        (unsigned long) mem_regions[i].virtual_base);
        //}*/
    }        
    
    return 0;
}

unsigned int get_mem_region(unsigned long address)
{
    int i;

    for (i=0; i<MAX_MEM_REGIONS; i++)
    {
        if ((address>=mem_regions[i].base_addr) && 
            (address<(mem_regions[i].base_addr + mem_regions[i].size)))
        {
            return i;
        }
    }
    
    return INVALID_MEM_REGION;
}


void write_reg_region(unsigned long address, unsigned long value, unsigned int region)
{
	unsigned long *pVal;
	
	pVal = (unsigned long *)((unsigned long)(mem_regions[region].virtual_base) + (address - mem_regions[region].base_addr));
	*pVal = value;
}

unsigned int write_reg(unsigned long address, unsigned long value)
{
	unsigned int region=INVALID_MEM_REGION;
	unsigned long *pVal;
	
	region = get_mem_region(address);
	if (region == INVALID_MEM_REGION)            
	{
		return 1; /* Error Case */
	}
	else
	{        
		pVal = (unsigned long *)((unsigned long)(mem_regions[region].virtual_base) + (address - mem_regions[region].base_addr));
		*pVal = value;
		
		return 0;
	}	
}


unsigned int read_reg(unsigned long address)
{
	unsigned int region=INVALID_MEM_REGION;
	unsigned long *pVal;
	
	region = get_mem_region(address);
	if (region == INVALID_MEM_REGION)            
	{
		return 0; /* Error Case */
	}
	else
	{        
		pVal = (unsigned long *)((unsigned long)(mem_regions[region].virtual_base) + (address - mem_regions[region].base_addr));
		return *pVal;
	}	
}

void sigstop_handler (int signum)
{
	/* Stop and Exit */
	printf ("Received Signal : %d\n", signum);
	write_reg(TIMER_WDCMD, 0x0000EE00);
	write_reg(TIMER_WDCMD, 0x000000EE);
	
	printf ("Stopping Watchdog Chip Reset\n");
	exit (0);
}

int main(int argc, char *argv[])
{
	int ret;
	int timeout;
	int count;
	
	unsigned int region_to = INVALID_MEM_REGION;
	unsigned int region_cmd = INVALID_MEM_REGION;

	unsigned long reg_val;
	
	if (argc != 2)
	{
		printf ("Usage: %s <timeout in secs>\n", argv[0]);
		return 0;
	}
	sscanf (argv[1], "%d", &timeout);
	count = timeout >> 4;	/* Min timeout = 16 secs */
	count++;
	
	signal(SIGALRM, sigstop_handler);
	
	ret = map_mem_regions();
	if (ret!=0)
	{
		return ret;
	}
	
	/* Set Watchdog event mode and count mode */
	write_reg(TIMER_WDCTRL, 0x00000000);
	
	/* Disable the Watchdog interrupt mask */
	reg_val = read_reg(TIMER_TIMER_IE1);
	reg_val &= 0xFFFFFFEF; 
	write_reg(TIMER_TIMER_IE1, reg_val);

	region_cmd = get_mem_region(TIMER_WDCMD);
	region_to = get_mem_region(TIMER_WDTIMEOUT);

	if ((region_to == INVALID_MEM_REGION) || (region_cmd == INVALID_MEM_REGION))
	{
		printf ("Could not determine region number for watchdog registers");
		exit (1);
	}	

	while (count != 0)
	{
		/* Enter the critical section */
		pthread_mutex_lock( &cs_mutex );
		
		/* First Stop */
		write_reg_region(TIMER_WDCMD, 0x0000EE00, region_cmd);
		write_reg_region(TIMER_WDCMD, 0x000000EE, region_cmd);
		write_reg_region(TIMER_WDTIMEOUT, 0x1FFFFFFF, region_to); /* 32 Seconds */
		/* Then Start*/
		write_reg_region(TIMER_WDCMD, 0x0000FF00, region_cmd);
		write_reg_region(TIMER_WDCMD, 0x000000FF, region_cmd);
		
		/*Leave the critical section */
		pthread_mutex_unlock( &cs_mutex );
			
		sleep (16);
		count--;
	}
	/* printf ("Gone in 16 seconds\n");*/

	return 0;
}
