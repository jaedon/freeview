/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: regscope_server.c $
 * $brcm_Revision: Hydra_Software_Devel/14 $
 * $brcm_Date: 10/16/12 1:47p $
 *
 * Module Description:
 *   The source code for the stub ikos server.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/applications/regscope_server/regscope_server.c $
 * 
 * Hydra_Software_Devel/14   10/16/12 1:47p hongtaoz
 * SW7445-8, SW7445-56: added 3rd memc for 7445;
 * 
 * Hydra_Software_Devel/13   10/16/12 10:49a hongtaoz
 * SW7445-56: adapted regscope_server to 7445 register address space;
 * 
 * Hydra_Software_Devel/12   10/20/10 11:43a hongtaoz
 * SW7425-10: added MEMC1 support to regscope and stub server;
 * 
 * Hydra_Software_Devel/11   9/29/10 6:09p hongtaoz
 * SW7425-10: make customizable registers runtime option; support up to
 * 256 customizable registers;
 * 
 * Hydra_Software_Devel/10   9/29/10 1:27p hongtaoz
 * SW7425-10: added stub_server compile option for user customizable reg
 * read value;
 * 
 * Hydra_Software_Devel/9   9/28/10 11:23p hongtaoz
 * SW7425-10: added stub server back door for register read;
 * 
 * Hydra_Software_Devel/8   9/8/10 1:33p hongtaoz
 * SW7425-10, SW7425-28: fixed the faked chip id;
 * 
 * Hydra_Software_Devel/7   9/8/10 11:42a hongtaoz
 * SW7425-10, SW7425-28: faked ikos server always returns bmem guard bytes
 * 0xac to pass bmem validation;
 * 
 * Hydra_Software_Devel/6   9/8/10 10:54a hongtaoz
 * SW7425-10, SW7425-28: added faked ikos server capability;
 * 
 * Hydra_Software_Devel/5   8/31/10 1:56p tdo
 * SW7425-28: Add regscope support for 7425
 *
 * Hydra_Software_Devel/4   4/26/07 10:37p pntruong
 * PR 29285 : adapt to new prototype.
 *
 * Hydra_Software_Devel/3   1/4/07 3:44p hongtaoz
 * PR22568: updated regscope server to be appframework compliant;
 *
 * Hydra_Software_Devel/2   4/28/06 3:26p hongtaoz
 * PR17038: allow user to use absolute physical address instead of offset
 * address for memory access;
 *
 * Hydra_Software_Devel/1   9/7/05 8:37p hongtaoz
 * PR17038: move regscope_server into rockford using appframework;
 *
 * Hydra_Software_Devel/17   8/3/05 3:07p hongtaoz
 * PR16397: added 7401 support; changed kmem to mem to map memory/reg
 * space;
 *
 * Hydra_Software_Devel/16   3/21/05 10:44a hongtaoz
 * PR14494: added support for 3560 platform;
 *
 * Hydra_Software_Devel/15   10/14/04 11:16a hongtaoz
 * for stub server, use vdec revision id to test B0, B1 and C0;
 *
 * Hydra_Software_Devel/14   5/12/04 12:45p hongtaoz
 * added version ID message with server start up;
 * temporary fix for regscope_server to work around chip ID guess problem;
 *
 * Hydra_Software_Devel/13   4/2/04 5:40p hongtaoz
 * fixed compile error for ikos emulation.
 *
 * Hydra_Software_Devel/12   2/27/04 12:16p hongtaoz
 * PR9858: increase heap size from 32 MB to 96MB.
 *
 * Hydra_Software_Devel/11   2/10/04 3:07p hongtaoz
 * Temporary fix of repeated memory write problem for regscope_server
 * build running on set-top box.
 *
 * Hydra_Software_Devel/10   12/18/03 4:16p hongtaoz
 * Fixed "-d" option bug.
 *
 * Hydra_Software_Devel/9   12/16/03 3:47p hongtaoz
 * Ported to 7038 set-top box running linux; now it behaves as a thin
 * regscope server;
 * to compile it for set-top box:
 *    path_to_your_cross_compiler_gcc -D BCM7038 ikos_stub_server.c -o
 * regscope_server
 * to launch it on set-top box:
 *    regscope_server port_num
 *
 * Hydra_Software_Devel/8   9/15/03 4:26p hongtaoz
 * modified tb_Init_Capture()'s last argument type to char[] to comply
 * with the IKOS function prototype.
 *
 * Hydra_Software_Devel/7   9/12/03 4:50p hongtaoz
 * 1) implemented IKOS_INIT_CAPTURE/IKOS_START_CAPTURE/IKOS_STOP_CAPTURE
 * commands;
 * 2) always return chip id 0x70380000;
 * 3) used macro ikos commands.
 *
 * Hydra_Software_Devel/6   9/10/03 1:11p hongtaoz
 * fixed compile error.
 *
 * Hydra_Software_Devel/5   9/10/03 12:59p hongtaoz
 * added memc_init() whenever a new connection from a fresh new host is
 * allowed.
 *
 * Hydra_Software_Devel/4   9/10/03 11:47a hongtaoz
 * added support for multiple connections service; added compile option
 * for real IKOS emulation; added Ack response immediately after a
 * successful client connection.
 * Note: when it runs on 'usul', multiple clients from multiple hosts are
 * allowed, and no register address check; when it runs on real IKOS
 * emulator, multiple clients from ONLY ONE host are allowed, AND there
 * would be address check for register access.
 *
 * Hydra_Software_Devel/3   9/5/03 3:37p hongtaoz
 * fixed 'usul'(running on pc) byte order issues with IKOS_MEM_READ &
 * IKOS_MEM_WRITE.
 *
 * Hydra_Software_Devel/2   9/4/03 6:24p hongtaoz
 * changed the command line to accept user-assigned port number.
 ***************************************************************************/
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IKOS_NAK                 0
#define IKOS_ACK                 1

#define IKOS_STOP_SERVER         0
#define IKOS_STOP_CLIENT         1
#define IKOS_REG_READ            2
#define IKOS_REG_WRITE           4
#define IKOS_MEM_READ            6
#define IKOS_MEM_WRITE           8
#define IKOS_TIMEOUT            10
#define IKOS_INIT_CAPTURE       12
#define IKOS_START_CAPTURE      14
#define IKOS_STOP_CAPTURE       16

#define SERVER_VERSION_ID       "Hydra_Software_Devel/12"

/***********************************
  if on real set-top box
 */
#ifdef SETTOPBOX/* to run on real set-top box or stub_server */

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include "systeminit.h"
#include "memmgr.h"
#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"
#include "bstd.h"

BSystem_Info g_stSysInfo;
bool         g_bFail;
#endif

/***********************************
  if real ikos simulation
 */
#ifdef TIPSIM
#include <testbench.h>
#include <host_api.h>

#define IKOS_PORT 9999

#define pout(s) do { fprintf( stderr, \
	"%s line %d errno %d:" s "\n", __FILE__, __LINE__, errno ); \
} while( 0 )

/*============= Modules List ===============================
	add/remove the modules according to emulator's capacity;
	if the module is not compiled in the emulator, GISB access
	timeout might terminate the server.
 */
#define IS_VALID_ADDR( addr ) 1

/***********************************
  else stub server on 'usul' or real stb
 */
#else

#define pout(s) do { printf( \
	"%s line %d errno %d:" s "\n", __FILE__, __LINE__, errno ); \
} while( 0 )

#define IS_VALID_ADDR( addr ) 1

/* Hydra chips used to have reg base address of 0x10000000; 7445 changed to 0xF0000000;
 * The regscope client (perlextlib) hard-coded the reg base addr of 0x10000000; we need to correct it for newer chips via RDB.
 * NOTE: we assume all Hydra chips will have the register offset range within the lower 28-bit.
 */
#define BTST_REG_OFFSET_MASK (~0xF0000000)
#ifdef BCHP_SUN_TOP_CTRL_PROD_REVISION
define IS_CHIP_ID_ADDR(addr) ((addr & BTST_REG_OFFSET_MASK) == BCHP_SUN_TOP_CTRL_PROD_REVISION)
#else
#define IS_CHIP_ID_ADDR(addr) ((addr & BTST_REG_OFFSET_MASK) == BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID)
#endif

typedef unsigned char u8;

#ifdef EMULATION /* back door to control stub server behavior of register read */
#define BTST_MAX_USER_SET_REG_CNT         256
static bool bRegCustom = false;
static unsigned  aulUserSetReg[BTST_MAX_USER_SET_REG_CNT] = {0,};
static unsigned  aulUserSetVal[BTST_MAX_USER_SET_REG_CNT] = {0,};
static bool abUseVal[BTST_MAX_USER_SET_REG_CNT] = {false,};
#endif
/* stub tb_r() function to return dummy register read value */
unsigned long tb_r(unsigned long addr)
{
#ifdef SETTOPBOX/* to run on real 7038 set-top box */

	/* regscope client somehow tries to guess chip ID twice.
	   Here is workaround. */
	static int iChipIdRead = 2;

	if(iChipIdRead)
	{
		if(!IS_CHIP_ID_ADDR(addr))
		{
			return 0;
		} else
		{
			iChipIdRead--;
		}
	}
	#ifdef EMULATION/* to run on faked ikos */
	if(IS_CHIP_ID_ADDR(addr))
		return (((BCHP_CHIP/1000)<<28) + (((BCHP_CHIP/100)%10)<<24) + (((BCHP_CHIP/10)%10)<<20) + ((BCHP_CHIP%10)<<16));
    else
	{
		if(bRegCustom) /* back door to control stub server behavior of register read */
		{
			int i;
			for(i=0; i<BTST_MAX_USER_SET_REG_CNT; i++)
			{
				if(abUseVal[i] && (addr == aulUserSetReg[i]))
					return aulUserSetVal[i];
			}
		}
		return (0);/* default to 0 */
	}
	#else
	return *((volatile unsigned long *)((uint32_t)g_stSysInfo.bregBaseAddr + (addr & BTST_REG_OFFSET_MASK)));
	#endif
#else /* legacy hard-coded fake ikos server is unused any more. TODO: remove this */
	/* for fake ikos erver on 'usul', always return chip id as 7038A0 */
	if(addr == 0x10404000)
		return 0x70380000;
	if(addr == 0x101c1178)
	#ifdef BCHP_REV_B0
		return 0x500;
	#elif defined(BCHP_REV_C0)
		return 0x520;
	#else /* B1 */
		return 0x501;
	#endif
	return addr;
#endif
}

/* stub tb_w() function to perform dummy register write */
void tb_w(unsigned long addr, unsigned long data)
{
#ifdef SETTOPBOX
#ifndef EMULATION/* to run on real set-top box */
	*((volatile unsigned long *)((unsigned int)g_stSysInfo.bregBaseAddr + (addr & BTST_REG_OFFSET_MASK))) = data;
#else/* to run on faked ikos */
	BSTD_UNUSED(addr);
	BSTD_UNUSED(data);
#endif
#endif
}

void* convert_mem_offset_to_vaddr(uint32_t offset, uint32_t size)
{
	void *pAddr;
	if((offset >= g_stSysInfo.bmemOffset) && 
	    (offset + size < g_stSysInfo.bmemOffset + g_stSysInfo.bmemSize))
	{
		pAddr = (void*)((uint32_t)g_stSysInfo.bmemAddress + offset - g_stSysInfo.bmemOffset);
		g_bFail = false;
	}
#ifdef MEMC_1_MEM_PBASE
	else if((offset >= g_stSysInfo.bmemOffset1) && 
	    (offset + size < g_stSysInfo.bmemOffset1 + g_stSysInfo.bmemSize1))
	{
		pAddr = (void*)((uint32_t)g_stSysInfo.bmemAddress1 + offset - g_stSysInfo.bmemOffset1);
		g_bFail = false;
	}
#ifdef MEMC_2_MEM_PBASE
	else if((offset >= g_stSysInfo.bmemOffset2) && 
	    (offset + size < g_stSysInfo.bmemOffset2 + g_stSysInfo.bmemSize2))
	{
		pAddr = (void*)((uint32_t)g_stSysInfo.bmemAddress2 + offset - g_stSysInfo.bmemOffset2);
		g_bFail = false;
	}
#endif
#endif
	else
	{
		printf("!!! regscope_server MemoryRead addr 0x%x size0x%x is outside mapped range:\n",
			offset, size);
		printf("[0x%x, 0x%x)", 
			g_stSysInfo.bmemOffset, g_stSysInfo.bmemOffset +g_stSysInfo.bmemSize);
#ifdef MEMC_1_MEM_PBASE
		printf(", [0x%x, 0x%x)", 
			g_stSysInfo.bmemOffset1, g_stSysInfo.bmemOffset1 + g_stSysInfo.bmemSize1);
#ifdef MEMC_2_MEM_PBASE
		printf(", [0x%x, 0x%x)", 
			g_stSysInfo.bmemOffset2, g_stSysInfo.bmemOffset2 + g_stSysInfo.bmemSize2);
#endif
#endif
		printf("!!!\n");
		g_bFail = true;
		pAddr = NULL;
	}
	return pAddr;
}

/* stub tb_zmr8_block() function to perform memory read;
   inputs:
     addr - starting address;
     size - in bytes;
       *x - unused;
   output:
    *data - bytes array;
 */
void tb_zmr8_block( u8 * data, uint32_t addr, uint32_t size, uint32_t *x )
{
#ifdef SETTOPBOX/* to run on real 7038 set-top box */
	uint32_t i;
	/* NOTE: we are going to take the absolute physical address from now upon. */
	u8* pAddr;
	BSTD_UNUSED(x);

	/* don't probe the memory region beyond the mapped ranged! */
	pAddr = (u8*)convert_mem_offset_to_vaddr(addr, size);
	if(g_bFail) return;

	for(i = 0; i < size; i += sizeof(uint32_t))
	{
	#ifdef EMULATION/* to run on faked ikos */
	/*
	 * BMEM_GUARD_BYTE (modifyable)
	 *
	 * Guard bytes are set to this value when a mem block is created. In addition to
	 * the formal guard areas, the back scrap and front scrap are filled with
	 * this value. The default is 0xac (for "All Clear"). This is to pass the BMEM guard check
	 * for the faked ikos; the original define is in bmem_priv.h; please make sure it's matched!
	 */
	#define BMEM_GUARD_BYTE      0xac
		*(data + i)     = BMEM_GUARD_BYTE;
		*(data + i + 1) = BMEM_GUARD_BYTE;
		*(data + i + 2) = BMEM_GUARD_BYTE;
		*(data + i + 3) = BMEM_GUARD_BYTE;
	BSTD_UNUSED(pAddr);
	#else
		/* note: linux might run at little or big endian */
		#if (BSTD_CPU_ENDIAN==BSTD_ENDIAN_LITTLE)
		*(data + i)     = *(pAddr + i + 3);
		*(data + i + 1) = *(pAddr + i + 2);
		*(data + i + 2) = *(pAddr + i + 1);
		*(data + i + 3) = *(pAddr + i);
		#elif (BSTD_CPU_ENDIAN==BSTD_ENDIAN_BIG)
		*(data + i)     = *(pAddr + i);
		*(data + i + 1) = *(pAddr + i + 1);
		*(data + i + 2) = *(pAddr + i + 2);
		*(data + i + 3) = *(pAddr + i + 3);
		#else
		#error "Please specify the CPU endianess!"
		#endif
	#endif
	}
#endif
}

/* stub tb_zmw8_block() function to perform dummy memory write */
void tb_zmw8_block( u8 * data, uint32_t addr, uint32_t size, uint32_t *x )
{
#ifdef SETTOPBOX/* to run on real 7038 set-top box */
	/* for regscope_server, command 'mw addr data size' is writing the same 32-bit data to size of continuous bytes
	   pointed by addr */
	uint32_t i;
	/* NOTE: we are going to take the absolute physical address from now upon. */
	u8* pAddr;
	BSTD_UNUSED(x);

	/* don't probe the memory region beyond the mapped ranged! */
	pAddr = (u8*)convert_mem_offset_to_vaddr(addr, size);
	if(g_bFail) return;

	#ifndef EMULATION/* to run on faked ikos */
	for(i = 0; i < size; i += sizeof(uint32_t))
	{
		/* note: linux is running at little endian, network is big endian. */
		#if (BSTD_CPU_ENDIAN==BSTD_ENDIAN_LITTLE)
		*(pAddr + i + 3) = *(data);
		*(pAddr + i + 2) = *(data + 1);
		*(pAddr + i + 1) = *(data + 2);
		*(pAddr + i)     = *(data + 3);
		#elif (BSTD_CPU_ENDIAN==BSTD_ENDIAN_BIG)
		*(pAddr + i)     = *(data);
		*(pAddr + i + 1) = *(data + 1);
		*(pAddr + i + 2) = *(data + 2);
		*(pAddr + i + 3) = *(data + 3);
		#else
		#error "Please specify the CPU endianess!"
		#endif
	}
	#else
	BSTD_UNUSED(pAddr);
	if(bRegCustom) /* back door to control stub server behavior of register read */
	{
		if(0 == (i = addr%0x1000)) return; /* emu client always on page boundary -> do nothing */

		/* rescope client may send command via non-page-boundary-aligned addr:
		 *   4*i + 4: custom reg read enable/disable[i]; (i<BTST_MAX_USER_SET_REG_CNT)
		 *   4*i + 8: custom reg addr[i];
		 *   4*i + c: custom reg value[i];
		 */
		i = (i - sizeof(uint32_t))/(4*sizeof(uint32_t));
		if(i < BTST_MAX_USER_SET_REG_CNT)
		{
			int index = ((addr%0x1000 - sizeof(uint32_t))/sizeof(uint32_t)) % 4;
			#if (BSTD_CPU_ENDIAN==BSTD_ENDIAN_LITTLE)
			uint32_t ulVal = (((uint32_t)*(data)) << 24) + 
			       (((uint32_t)*(data+1)) << 16) + 
			       (((uint32_t)*(data+2)) << 8)  +
			       (((uint32_t)*(data+3)) << 0);
			#elif (BSTD_CPU_ENDIAN==BSTD_ENDIAN_BIG)
			uint32_t ulVal = (((uint32_t)*(data+3)) << 24) + 
			       (((uint32_t)*(data+2)) << 16) + 
			       (((uint32_t)*(data+1)) << 8)  +
			       (((uint32_t)*(data)) << 0);
			#else
				#error "Please specify the CPU endianess!"
			#endif
			switch(index)
			{
			case 0: /* enable/disable */
				abUseVal[i] = ulVal != 0; printf("ENABLE  [%d]: addr = %8x, data = %8x\n", i, addr, abUseVal[i]); break;
			case 1: /* reg addr */
				aulUserSetReg[i] = ulVal; 
	            printf("REGISTER[%d]: addr = %8x, data = %8x\n", i, addr, aulUserSetReg[i]); break;
			case 2: /* reg value */
				aulUserSetVal[i] = ulVal; printf("VALUE   [%d]: addr = %8x, data = %8x\n", i, addr, aulUserSetVal[i]); break;
			case 3:
			default: printf(">>> error!\n"); break;
	        }
	 	}
	}
	#endif
#endif
}

int timeout_ns /* stub func */
(
	uint32_t size
)
{
#if !SETTOPBOX/* not to run on real 7038 set-top box */
/* to run on real 7038 set-top box */
	printf( "timeout_ns(0x%8x)\n", size );
#else
	BSTD_UNUSED(size);
#endif
	return 0;
}

/* stub func */
int tb_Init_Capture( int iCmd, char *acPath )
{
	BSTD_UNUSED(iCmd);
	BSTD_UNUSED(acPath);
	return 0;
}

/* stub func */
int tb_Start_Capture( void )
{
	return 0;
}

/* stub func */
int tb_Stop_Capture( void )
{
	return 0;
}

#endif /* ifdef TIPSIM */

int my_send(int s, void *buf, int len, int flag)
{
    int total = 0;        /* how many bytes we've sent */
    int bytesleft = len; /* how many we have left to send*/
    int n;

    while(total < len) {
        n = send(s, (char *)buf+total, bytesleft, flag);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    return total; /* return -1 on failure, 0 on success*/
}

int my_recv
(
	int s,
	void *buf,
	int len,
	int flags
)
{
	int result;
	int original_len = len;

	for( ;; )
	{
		result = recv( s, buf, len, flags );
		if( result <= 0 )
		{
			break;
		}

		buf = (char *) buf + result;
		len -= result;

		if( !len )
		{
			result = original_len;
			break;
		}
	}

	return result;
}

/*****************************************************************************/

void memc_init( void )
{
#if !SETTOPBOX/* to run on real 7038 set-top box */
	uint32_t data;

	/* set the GISB timeout value to something reasonable */

	tb_w( 0x10404010, 0x00000000 ); /* BCHP_SUN_TOP_CTRL_SW_RESET:Unresets everything*/
	tb_w( 0x1040600c, 0x00000100 ); /* BCHP_SUN_GISB_ARB_TIMER*/

	/* memory programming */

	data = tb_r( 0x10106000 ); /* BCHP_MEMC_CORE_REV_ID*/
	printf( "MEMC_CORE_REV_ID = %08X\n", data );

	data = tb_r( 0x10106800 ); /* BCHP_MEMC_DDR_IOBUF_REV_ID*/
	printf( "MEMC_DDR_IOBUF_REV_ID = %08X\n", data );

	data = tb_r( 0x10106844 ); /* BCHP_MEMC_DDR_DRAM_MODE*/
	printf( "MEMC_DDR_DRAM_MODE = %08X\n", data );

	tb_w( 0x10106844, 0x00000003 ); /* BCHP_MEMC_DDR_DRAM_MODE*/

	data = tb_r( 0x10106844 ); /* BCHP_MEMC_DDR_DRAM_MODE*/
	printf( "MEMC_DDR_DRAM_MODE = %08X\n", data );

	data = tb_r( 0x10106848 ); /*BCHP_MEMC_DDR_DRAM_TIMING*/
	printf( "BCHP_MEMC_DDR_DRAM_TIMING = %08X\n", data );

	tb_w( 0x10106840, 0x00000002 ); /* BCHP_MEMC_DDR_CNTRLR_CONFIG*/
	tb_w( 0x10106854, 0x00000001 ); /*BCHP_MEMC_DDR_CNTRLR_START_SEQ*/
#endif
}

/*============ main ======================================
 	Usage: ikos_mux_server <port_num>
 	Description:
 		the server will allow multiple clients on the same host to
 		talk to it simultaneously.
 */
#ifndef TIPSIM
/* to silence compiler warning; */
extern int strcmp(const char *s1, const char *s2);
extern int atoi(const char *nptr);

int app_main(int argc, char *argv[])
#else
int sim( )
#endif
{
    fd_set master;   /* master file descriptor list*/
    fd_set read_fds; /* temp file descriptor list for select()*/
    struct sockaddr_in myaddr;     /* server address*/
    struct sockaddr_in remoteaddr; /* client address*/
    int fdmax;        /* maximum file descriptor number*/
    int listener;  /* listening socket descriptor*/
    int newfd;        /* newly accept()ed socket descriptor*/
    int nbytes;
    int yes=1;        /* for setsockopt() SO_REUSEADDR, below*/
    uint32_t addrlen;
    int i;
	int bConnected = 0;
	uint32_t client_addr = 0;
	uint32_t client_num = 0;
    char *achPort;

#ifndef TIPSIM
	int bDump = 0;

	if(argc == 1)
	{
        #ifdef EMULATION
		printf("Usage: %s [-d] [-c] port_num\n", argv[0]);
        #else
		printf("Usage: %s [-d] port_num\n", argv[0]);
        #endif
		printf("Option:\n");
		printf("       -d      turn on debug dump info\n");
		printf("       -c      turn on register customization (for stub_server)\n");
		return 0;
	} 

	achPort = argv[argc - 1];
	if(argc >= 3)
	{
		if(!strcmp(argv[1], "-d"))
			bDump = 1;
        #ifdef EMULATION
		else if(!strcmp(argv[1], "-c"))
			bRegCustom = true;
        #endif
	}
	if(argc == 4)
	{
		if(!strcmp(argv[2], "-d"))
			bDump = 1;
        #ifdef EMULATION
		else if(!strcmp(argv[2], "-c"))
			bRegCustom = true;
        #endif
	}
#endif
	timeout_ns( 1000000 );
#ifdef SETTOPBOX/* to run on real 7038 set-top box */
	printf("Initialize regscope_server.\n");
	memmgr_Init(argc, argv, &g_stSysInfo);
#else
	printf("Initialize ikos_server.\n");
	tb_w(0x1040600c, 0x100); /* SUN_GISB_ARB_TIMER: force timeout value = 256 clks */
#endif
	printf("Version %s.\n", SERVER_VERSION_ID);

	FD_ZERO(&master);    /* clear the master and temp sets*/
    FD_ZERO(&read_fds);

	if( (listener = socket( PF_INET, SOCK_STREAM, 0 )) < 0 )
	{
		pout( "socket()" );
		goto done;
	}

    /* lose the "address already in use" error message*/
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(int)) == -1) {
        perror("setsockopt");
        goto fail_close;
    }


	myaddr.sin_family = AF_INET;
#ifdef TIPSIM
	myaddr.sin_port = htons( IKOS_PORT );
#else
	myaddr.sin_port = htons( (unsigned short)atoi(achPort) );
#endif
	myaddr.sin_addr.s_addr = htonl( INADDR_ANY );

	if( bind( listener, (struct sockaddr *) &myaddr, sizeof myaddr ) < 0 )
	{
		pout( "bind()" );
		goto fail_close;
	}

	if( listen( listener, SOMAXCONN ) < 0 )
	{
		pout( "listen()" );
		goto fail_close;
	}
	puts( "Waiting for connection..." );

    /* add the listener to the master set*/
    FD_SET(listener, &master);

    /* keep track of the biggest file descriptor*/
    fdmax = listener; /* so far, it's this one*/

	for( ;; )
	{
		unsigned char buffer[ 9 ];
		uint32_t addr;
		char acPath[241];
		int size;
		int iCapCmd;
		int stop_server = 0;
		int stop_client = 0;
		uint32_t data[1024] = {0};

        read_fds = master; /* copy it*/
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            pout("select");
            goto fail_close;
        }

		/*puts("activities detected");*/
        /* run through the existing connections looking for data to read*/
        for(i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))
            { /* we got one!!*/
                if (i == listener)
                {
                    /* handle new connections*/
                    /*puts("new connection...");*/
                    addrlen = sizeof(remoteaddr);
                    if ((newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen)) == -1)
                    {
                        perror("accept");
                    } else
                    {
						/*printf("client_num = %d, bConnect = %d, client_addr = 0x%x, remote_addr= 0x%x\n", client_num,bConnected, client_addr, remoteaddr.sin_addr.s_addr);*/
						/* to prevent a different host trying to connect */
#ifdef TIPSIM
						if( bConnected && (client_addr != remoteaddr.sin_addr.s_addr) )
						{
							close( newfd );
							continue;
						}
#endif
						if( !bConnected )
						{/* The reason to reset system is to prevent the following GISB access to IKOS,
							from timeout error, which might terminate the server prematurely!
							Only reset chip for the first successful connection from the fresh new host */
							memc_init();
						}
						my_send(newfd, (char *)data, 1, 0); /* ack*/

						client_addr = remoteaddr.sin_addr.s_addr;
						bConnected = 1;
						client_num ++;

                        FD_SET(newfd, &master); /* add to master set*/
                        if (newfd > fdmax)
                        {   /* keep track of the maximum*/
                            fdmax = newfd;
                        }
                        printf("New connection from %s on "
                                "socket %d\n", inet_ntoa(remoteaddr.sin_addr), newfd);
                    }
                } else {
                    /* handle data from a client*/
					if( (nbytes = my_recv( i, buffer, 1, 0 )) < 1 )
					{
						printf( "client at socket %d hang up.\n", i );
						if( --client_num == 0 )
						{
							client_addr = 0;
							bConnected = 0;
						}
                        close(i); /* bye!*/
                        FD_CLR(i, &master); /* remove from master set*/
						continue;
					}

					/* the real data service */
					switch( buffer[ 0 ] )
					{
					case IKOS_STOP_SERVER:
						stop_server = 1;
					case IKOS_STOP_CLIENT:
						stop_client = 1;
						break;
					case IKOS_REG_READ:
						timeout_ns( 10000 );
						if( my_recv( i, &buffer[ 1 ], 4, 0 ) < 4 )
						{
							pout( "IKOS_REG_READ recv()" );
							stop_client = 1;
							break;
						}
						addr =
							((unsigned long) buffer[ 1 ] << 24) |
							((unsigned long) buffer[ 2 ] << 16) |
							((unsigned long) buffer[ 3 ] <<  8) |
							((unsigned long) buffer[ 4 ] <<  0);

						/* read register here */
#ifdef SETTOPBOX/* to run on real 7038 set-top box */
						if(bDump)
#endif
							printf( "IKOS_REG_READ addr %08X\n", addr );
						fflush( stdout );
						if( IS_VALID_ADDR( addr ) )
						{
							data[0] = tb_r(addr);
#ifdef SETTOPBOX/* to run on real 7038 set-top box */
							if(bDump)
#endif
							printf( "IKOS_REG_READ data %08X\n", data[0] );
							fflush( stdout );
						} else
						{
							pout( "IKOS_REG_READ addr out of range!" );
						}

						buffer[ 0 ] |= IKOS_ACK;
						buffer[ 1 ] = (unsigned char) (data[0] >> 24);
						buffer[ 2 ] = (unsigned char) (data[0] >> 16);
						buffer[ 3 ] = (unsigned char) (data[0] >>  8);
						buffer[ 4 ] = (unsigned char) (data[0] >>  0);
						if( my_send( i, buffer, 5, 0 ) < 5 )
						{
							pout( "IKOS_REG_READ send()" );
							stop_client = 1;
						}
						break;
					case IKOS_REG_WRITE:
						if( my_recv( i, &buffer[ 1 ], 8, 0 ) < 8 )
						{
							pout( "IKOS_REG_WRITE recv()" );
							stop_client = 1;
							break;
						}
						addr =
							((unsigned long) buffer[ 1 ] << 24) |
							((unsigned long) buffer[ 2 ] << 16) |
							((unsigned long) buffer[ 3 ] <<  8) |
							((unsigned long) buffer[ 4 ] <<  0);
						data[0] =
							((unsigned long) buffer[ 5 ] << 24) |
							((unsigned long) buffer[ 6 ] << 16) |
							((unsigned long) buffer[ 7 ] <<  8) |
							((unsigned long) buffer[ 8 ] <<  0);

						/* write register here */
#ifdef SETTOPBOX/* to run on real 7038 set-top box */
						if(bDump)
#endif
							printf( "IKOS_REG_WRITE addr %08X\n", addr );
						fflush( stdout );
						if( IS_VALID_ADDR( addr ) )
						{
							tb_w(addr,data[0]);
#ifdef SETTOPBOX/* to run on real 7038 set-top box */
							if(bDump)
#endif
								printf( "IKOS_REG_WRITE data %08X\n", data[0] );
							fflush( stdout );
						} else
						{
							pout( "IKOS_REG_WRITE addr out of range!" );
						}

						buffer[ 0 ] |= IKOS_ACK;
						if( my_send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "IKOS_REG_WRITE send()" );
							stop_client = 1;
						}
						break;
					case IKOS_MEM_READ:
						if( my_recv( i, &buffer[ 1 ], 8, 0 ) < 8 )
						{
							pout( "IKOS_MEM_READ recv()" );
							stop_client = 1;
							break;
						}
						addr =
							((unsigned long) buffer[ 1 ] << 24) |
							((unsigned long) buffer[ 2 ] << 16) |
							((unsigned long) buffer[ 3 ] <<  8) |
							((unsigned long) buffer[ 4 ] <<  0);
						size =
							((unsigned long) buffer[ 5 ] << 24) |
							((unsigned long) buffer[ 6 ] << 16) |
							((unsigned long) buffer[ 7 ] <<  8) |
							((unsigned long) buffer[ 8 ] <<  0);

						/* read memory here */
#ifdef SETTOPBOX/* to run on real 7038 set-top box */
						if(bDump)
#endif
							printf( "IKOS_MEM_READ: addr = 0x%x, size = 0x%x\n", addr, size );
						fflush( stdout );
						tb_zmr8_block( (u8 *) data, addr, size, NULL );
						if(!g_bFail)
						{
							buffer[ 0 ] |= IKOS_ACK;
						}
						else
						{
							buffer[ 0 ] = IKOS_NAK;
#ifdef SETTOPBOX/* to run on real 7038 set-top box */
							if(bDump)
#endif
							printf( "IKOS_MEM_READ: addr = 0x%x, size = 0x%x failed!\n", addr, size );
							g_bFail = false;
						}

						if( my_send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "IKOS_MEM_READ send() ack" );
							stop_client = 1;
							break;
						}
						if( my_send( i, (u8 *) data, size, 0 ) < size )
						{
							pout( "IKOS_MEM_READ send() data" );
							printf( "my_send sent < %d bytes due to the error.\n", size );
							stop_client = 1;
						}
						break;
					case IKOS_MEM_WRITE:
						if( my_recv( i, &buffer[ 1 ], 8, 0 ) < 8 )
						{
							pout( "IKOS_MEM_WRITE recv()" );
							stop_client = 1;
							break;
						}
						addr =
							((unsigned long) buffer[ 1 ] << 24) |
							((unsigned long) buffer[ 2 ] << 16) |
							((unsigned long) buffer[ 3 ] <<  8) |
							((unsigned long) buffer[ 4 ] <<  0);
						size =
							((unsigned long) buffer[ 5 ] << 24) |
							((unsigned long) buffer[ 6 ] << 16) |
							((unsigned long) buffer[ 7 ] <<  8) |
							((unsigned long) buffer[ 8 ] <<  0);

						if( my_recv( i, data, size, 0 ) < size )
						{
							pout( "IKOS_MEM_WRITE recv() data" );
							stop_client = 1;
							break;
						}

#ifdef SETTOPBOX/* to run on real 7038 set-top box */
						if(bDump)
#endif
							printf( "IKOS_MEM_WRITE: addr = 0x%x, size = 0x%x, data = 0x%x\n", addr, size, *data );
						/* write memory here */
						/*printf( "before tb_zmw8_block\n" );*/
						fflush( stdout );
						tb_zmw8_block( (u8 *) data, addr, size, NULL );

						if(!g_bFail)
						{
							buffer[ 0 ] |= IKOS_ACK;
						}
						else
						{
							buffer[ 0 ] = IKOS_NAK;
#ifdef SETTOPBOX/* to run on real 7038 set-top box */
							if(bDump)
#endif
							printf( "IKOS_MEM_READ: addr = 0x%x, size = 0x%x failed!\n", addr, size );
							g_bFail = false;
						}
						if( my_send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "IKOS_MEM_WRITE send() ack" );
							stop_client = 1;
						}
						break;
					case IKOS_TIMEOUT:
						if( my_recv( i, &size, sizeof size, 0 ) < (int)sizeof size )
						{
							pout( "IKOS_TIMEOUT recv() size" );
							stop_client = 1;
						}

						timeout_ns( size );

						buffer[ 0 ] |= IKOS_ACK;
						if( my_send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "IKOS_TIMEOUT send() ack" );
							stop_client = 1;
						}
						break;
					case IKOS_INIT_CAPTURE:
						if( my_recv( i, &buffer[ 1 ], 8, 0 ) < 8 )
						{
							pout( "IKOS_INIT_CAPTURE recv() cmd and size" );
							stop_client = 1;
							break;
						}
						iCapCmd =
							((unsigned long) buffer[ 1 ] << 24) |
							((unsigned long) buffer[ 2 ] << 16) |
							((unsigned long) buffer[ 3 ] <<  8) |
							((unsigned long) buffer[ 4 ] <<  0);
						size =
							((unsigned long) buffer[ 5 ] << 24) |
							((unsigned long) buffer[ 6 ] << 16) |
							((unsigned long) buffer[ 7 ] <<  8) |
							((unsigned long) buffer[ 8 ] <<  0);

						if( size >= (int)sizeof acPath )
						{
							pout( "IKOS_INIT_CAPTURE recv() size is wrong" );
							stop_client = 1;
							break;
						}

						if( my_recv( i, acPath, size, 0 ) < size )
						{
							pout( "IKOS_INIT_CAPTURE recv() path" );
							stop_client = 1;
						}
						acPath[size + 1] = '\0';
						printf( "IKOS_INIT_CAPTURE(iCapCmd = %d, size = %d, path = %s)\n",\
							iCapCmd, size, acPath );

						/* start to init VEC's capture output data */
						tb_Init_Capture(iCapCmd, acPath);

						buffer[ 0 ] |= IKOS_ACK;
						if( my_send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "IKOS_INIT_CAPTURE send() ack" );
							stop_client = 1;
						}
						break;
					case IKOS_START_CAPTURE:
						/* start to capture VEC's output data */
						tb_Start_Capture();

						buffer[ 0 ] |= IKOS_ACK;
						if( my_send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "IKOS_START_CAPTURE send() ack" );
							stop_client = 1;
						}
						break;
					case IKOS_STOP_CAPTURE:
						/* stop to capture VEC's output data */
						tb_Stop_Capture( );

						buffer[ 0 ] |= IKOS_ACK;
						if( my_send( i, buffer, 1, 0 ) < 1 )
						{
							pout( "IKOS_STOP_CAPTURE send() ack" );
							stop_client = 1;
						}
						break;
					default:
						stop_client = 1;
					}/* end of switch*/

				}/* end of else: Handle data from client*/
			}/* end of if FD_ISSET*/

			if( stop_client )
			{
				stop_client = 0;
				if( --client_num == 0 )
				{
					client_addr = 0;
					bConnected = 0;
				}

				if( shutdown( i, 2 ) < 0 )
				{
					pout( "shutdown(cs)" );
				}

				if( close( i ) < 0 )
				{
					pout( "close(cs)" );
				}
                FD_CLR(i, &master); /* remove from master set*/

				printf( "Socket %d is disconnected.\n", i );
			}
		}/* end of for( i <= fd_max; )*/


		if( stop_server )
		{
			break;
		}
	}/* end of infinite for(;;)*/

fail_close:
	if( close( listener ) < 0 )
	{
		pout( "close()" );
		return 1;
	}
	puts( "Server is stopped." );

done:
#ifdef SETTOPBOX/* to run on real 7038 set-top box */
	memmgr_Done();
#endif
	puts( "Done." );
	return 0;
}

/* end of file */
