#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<string.h>
#endif
#include <errno.h>

#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_power.h"
#include "di_rtc.h"
#include "di_front_panel.h"
#include "di_led.h"
#include "di_err.h"
#include "di_uart.h"
#include "drv_err.h"
#include "drv_micom.h"
#include "drv_fan.h"
#include "drv_smart.h"
#include "pdriver.h"
#include "drv_demux.h"
#include "di_demux.h"
#include "drv_rcu_type.h"
#include "di_key.h"
#include "di_system.h"
#include "di_scart.h"
#include "di_nvram.h"
#include "di_ethernet.h"
#include "di_hdd.h"

#include "di_flash.h"

#include "di_crypt.h"
#include "di_pvr.h"

#include "bstd_defs.h"
#include "breg_mem.h"
#if (BCHP_CHIP == 7250)|| ((NEXUS_VERSION >= 1502) && (BCHP_CHIP == 7241))
#else
#include "bchp_usb_ehci.h"
#endif
#if (CONFIG_USB_HOSTCONTROLLER_CNT > 1)
#if ((BCHP_CHIP == 7439) && (BCHP_VER == BCHP_VER_B0)) || ((BCHP_CHIP == 7252) && (BCHP_VER == BCHP_VER_B0)) || ((NEXUS_VERSION >= 1502) && (BCHP_CHIP == 7241))
#else
#include "bchp_usb1_ehci.h"
#endif
#endif

#include "flash_map.h"

#if defined(BDBG_DEBUG_BUILD)
#include "linden_trace.h"
#endif

#include <fcntl.h>
#include <sys/ioctl.h>

#if defined(CONFIG_CAS_NA)
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "drv_csd.h"

#if defined(CONFIG_CAS_NA_NOCS_3X)
#include "nocs_csd.h"
#include "nocs_csd_impl.h"
#include "ca_sec.h"
#if defined(CONFIG_CAS_NA_NOCS_DPT)
#include "ca_dpt.h"
#endif
#else
#include "nocs1x_csd.h"
#include "nocs1x_csd_impl.h"
#endif
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define TEST_EXT_DEBUG
#define TEST_EXT_ENTER_LEAVE_DEBUG

#ifdef TEST_EXT_DEBUG
#define PrintDebug			DI_UART_Print
#define PrintError			DI_UART_Print
#else
#define PrintDebug			while (0) ((int (*)(char *, ...)) 0)
#define PrintError			DI_UART_Print
#endif

#ifdef TEST_EXT_ENTER_LEAVE_DEBUG
#define PrintEnter				DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit				DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter				while (0) ((int (*)(char *, ...)) 0)
#define PrintExit				while (0) ((int (*)(char *, ...)) 0)
#endif

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {DI_UART_Print(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define AssertDRV(arg1, arg2) if(arg1 != DRV_OK) {DI_UART_Print(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}


#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define HWTEST_PARAM3	szParam3
#define HWTEST_PARAM4	szParam4
#define HWTEST_PARAM5	szParam5
#define HWTEST_PARAM6	szParam6
#define HWTEST_PARAM7	szParam7
#define HWTEST_PARAM8	szParam8


#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM3=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM4=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM5=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM6=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM7=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM8=CMD_NextArg((char **)&szArgs);

#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define PARAM3_IS(x)	(HWTEST_PARAM3!=NULL && VK_strcmp(HWTEST_PARAM3, x)==0)
#define PARAM4_IS(x)	(HWTEST_PARAM4!=NULL && VK_strcmp(HWTEST_PARAM4, x)==0)
#define PARAM5_IS(x)	(HWTEST_PARAM5!=NULL && VK_strcmp(HWTEST_PARAM5, x)==0)
#define PARAM6_IS(x)	(HWTEST_PARAM6!=NULL && VK_strcmp(HWTEST_PARAM6, x)==0)
#define PARAM7_IS(x)	(HWTEST_PARAM7!=NULL && VK_strcmp(HWTEST_PARAM7, x)==0)
#define PARAM9_IS(x)	(HWTEST_PARAM8!=NULL && VK_strcmp(HWTEST_PARAM8, x)==0)

#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)
#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgs)


/* kernel driver 관련 임시 코드..   */
//#include "autoconf.h"
#define UNREFENCED_PARAM(x) (x=x)

#if defined(CONFIG_CAS_NA)
#if defined(CONFIG_RAM_ROOTFS)
#define NAGRA_PK_RAM_PHY_ADDR		(0x20000000)
#define NAGRA_PK_RAM_CACHED_ADDR	(0x60000000)
#else
#define NAGRA_PK_RAM_PHY_ADDR		(0x00A00000)
#define NAGRA_PK_RAM_CACHED_ADDR	(0x00A00000)
#endif

//#define NAGRA_PK_LOGICAL_OFFSET		(NAGRA_PK_LOGICAL_OFFSET)
#define NAGRA_PK_SIZE				(NAGRA_PK_BLOCK_SIZE) /* 1020 bytes */
/*PK_LENGTH_SIZE(4) + STB_CA_SN(4) + PK(N) + PK_SIGN(128) + CRC(2)*/
#define PK_LENGTH_SIZE				(4)
#define STB_CA_SN_SIZE				(4)

#define DUMP_NA_PK
#define DUMP_NA_CSC
#endif	// CONFIG_CAS_NA

#define FILE_MODE	(0644)
#define FILE_MIN_SIZE	(192) /*timestamp(4) + TS(188) = 192 */
#define FILE_CHUNK_SIZE	(192*5461) /* 192*5461 := 1MB */
static HUINT8 s_aucDecKey[DI_PVR_CRYPTO_KEY_LEN] = {0x0, 0x10, 0x32, 0x54, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
static HUINT8 s_aucDecIV[DI_PVR_CRYPTO_KEY_LEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

static HUINT8 s_aucEncKey[DI_PVR_CRYPTO_KEY_LEN] = {0x0, 0x10, 0x32, 0x54, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

#if defined(CONFIG_CAS_NA)
static int P_PK_GetCaSn(HUINT8 *pucCaSn)
{
	int mem_fd;
	void *mem_addr;
	int i;
	HUINT8 *pk_addr;
	HUINT8 pk_data;

	if((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
		CMD_Printf("/dev/mem open failed.\n");
		return -1;
	}
	else {
		CMD_Printf("/dev/mem opened.\n");
	}

	mem_addr = mmap(0, NAGRA_PK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, NAGRA_PK_RAM_PHY_ADDR);
	if(mem_addr < 0) {
		CMD_Printf("mmap failed.\n");
		close(mem_fd);
		return -1;
	}
	else {
		CMD_Printf("mmap mapped at address : 0x%08lx\n", mem_addr);
	}

	pk_addr = (HUINT8 *)mem_addr;
	/* the paring key file is made-up as PK_LENGTH_SIZE(4) + STB_CA_SN(4) + PK(N) + PK_SIGN(128) + CRC(2) */
	//*pulCaSn = (HUINT32)(pk_addr[4] << 24 | pk_addr[5] << 16 | pk_addr[6] << 8 | pk_addr[7]);
	pucCaSn[0] = pk_addr[4];
	pucCaSn[1] = pk_addr[5];
	pucCaSn[2] = pk_addr[6];
	pucCaSn[3] = pk_addr[7];

#if 0 /* for debugging paring key */
	for(i=1; i<=(NAGRA_PK_SIZE); i++)
	{
		CMD_Printf("0x%02x ", pk_addr[i-1]);
		if(i%8 == 0)
			CMD_Printf("\n");
	}
	CMD_Printf("\n");
#endif
	if(munmap(mem_addr, NAGRA_PK_SIZE) == -1)
	{
		CMD_Printf("unmap failed.\n");
	}

	close(mem_fd);
	return 0;
}
#endif

int CMD_HwTest_Parser_Crypto(void *szArgs)
{
	GET_ARGS;

	if CMD_IS("info")
	{
		HUINT32 chipID;
		DI_SYS_FUSING_INFO_t fusingInfo;
		HUINT8 chipRev[2];
		DI_ERR_CODE errCode;
#if defined(CONFIG_CAS_NA)
		TCsdStatus status;
		TCsd4BytesVector vector;
		TCsd20CharsString revision;
		TCsdBootMode bootMode;
		TCsdScsActivationFlag scsFlag;
		TCsdDebugInterfaceAccessMode debugMode;
#if defined(CONFIG_CAS_NA_NOCS_DPT)
#define CERT_CHECKNUM_SIZE 8
		TUnsignedInt8 pxCertCheckNumber[CERT_CHECKNUM_SIZE] = {0,};
#endif
#endif

#if defined(CONFIG_CAS_NA)
		status = csdGetNuid(vector);
		if (status != CSD_NO_ERROR)
		{
			CMD_Printf("csdGetNuid error(0x%x)\n", status);

			return CMD_ERR;
		}
		else
		{
			CMD_Printf("NUID        : 0x%02x%02x%02x%02x\n", vector[0], vector[1], vector[2], vector[3]);
		}

		status = csdGetNUIDCheckNumber(vector);
		if (status != CSD_NO_ERROR)
		{
			CMD_Printf("csdGetNUIDCheckNumber error(0x%x)\n", status);

			return CMD_ERR;
		}
		else
		{
			CMD_Printf("NUID CN     : 0x%02x%02x%02x%02x\n", vector[0], vector[1], vector[2], vector[3]);
		}
#else
		/* Read chip ID */
		errCode = DI_SYSTEM_GetSystemInfo(DI_SYS_CHIP_ID, &chipID, sizeof(HUINT32));
		if (errCode != DI_ERR_OK)
		{
			CMD_Printf("DI_SYSTEM_GetSystemInfo(DI_SYS_CHIP_ID) error(0x%x)\n", errCode);

			return CMD_ERR;
		}
		else
		{
			CMD_Printf("Chip ID     : 0x%x\n", chipID);
		}
#endif

#if defined(CONFIG_CAS_NA)
		status = csdGetChipRevision(revision);
		if (status != CSD_NO_ERROR)
		{
			CMD_Printf("csdGetChipRevision error(0x%x)\n", status);

			return CMD_ERR;
		}
		else
		{
			CMD_Printf("Chip rev.   : %c%c\n", revision[0], revision[1]);
		}
#else
		/* Read chip revision */
		errCode = DI_SYSTEM_GetSystemInfo(DI_SYS_CHIP_REVISION, chipRev, sizeof(chipRev));
		if (errCode != DI_ERR_OK)
		{
			CMD_Printf("DI_SYSTEM_GetSystemInfo(DI_SYS_CHIP_REVISION) error(0x%x)\n", errCode);

			return CMD_ERR;
		}
		else
		{
			CMD_Printf("Chip rev.   : %d.%d\n", chipRev[0], chipRev[1]);
		}
#endif

#if defined(CONFIG_CAS_NA)
		status = csdGetBootMode(&bootMode);
		if (status != CSD_NO_ERROR)
		{
			CMD_Printf("csdGetBootMode error(0x%x)\n", status);

			return CMD_ERR;
		}
		else
		{
#if defined(CONFIG_CAS_NA_NOCS_3X)
			CMD_Printf("Boot mode   : %s\n", (bootMode == CSD_BOOT_FROM_UNKNOWN) ? "NOT SET" :
												((bootMode == CSD_BOOT_FROM_NAND) ? "NAND" :
													((bootMode == CSD_BOOT_FROM_SPI) ? "SPI" :
														((bootMode == CSD_BOOT_FROM_NOR) ? "NOR" : "ERR"))));
#else
			CMD_Printf("Boot mode   : %s\n", (bootMode == CSD_BOOT_NOT_SET) ? "NOT SET" :
												((bootMode == CSD_BOOT_FROM_NAND) ? "NAND" :
													((bootMode == CSD_BOOT_FROM_SPI) ? "SPI" :
														((bootMode == CSD_BOOT_FROM_NOR) ? "NOR" : "ERR"))));
#endif
		}

		status = csdGetScsActivationFlag(&scsFlag);
		if (status != CSD_NO_ERROR)
		{
			CMD_Printf("csdGetScsActivationFlag error(0x%x)\n", status);

			return CMD_ERR;
		}
		else
		{
#if defined(CONFIG_CAS_NA_NOCS_3X)
			CMD_Printf("Secure Boot : %s\n", (scsFlag == CSD_ACTIVATION_FLAG_SET) ? "Enabled" : "Disabled");
#else
			CMD_Printf("Secure Boot : %s\n", (scsFlag == CSD_SCS_ACTIVATION_FLAG_SET) ? "Enabled" : "Disabled");
#endif
		}

		status = csdGetDebugInterfaceProtectionLevel(&debugMode);
		if (status != CSD_NO_ERROR)
		{
			CMD_Printf("csdGetDebugInterfaceProtectionLevel error(0x%x)\n", status);

			return CMD_ERR;
		}
		else
		{
			CMD_Printf("JTAG        : %s\n", (debugMode == CSD_DEBUG_INTERFACE_ACCESS_MODE_CLOSED) ? "Permanent Locked" :
											 ((debugMode == CSD_DEBUG_INTERFACE_ACCESS_MODE_PASSWORD_PROTECTED) ? "Password locked" : "Unlocked"));
		}

		status = csdGetMarketSegmentId(vector);
		if (status != CSD_NO_ERROR)
		{
			CMD_Printf("csdGetMarketSegmentId error(0x%x)\n", status);

			return CMD_ERR;
		}
		else
		{
			CMD_Printf("Market ID   : 0x%02x%02x%02x%02x\n", vector[0], vector[1], vector[2], vector[3]);
		}

		status = csdGetStbCaSn(vector);
		if (status != CSD_NO_ERROR)
		{
			CMD_Printf("csdGetStbCaSn error(0x%x)\n", status);

			return CMD_ERR;
		}
		else
		{
			CMD_Printf("STB CA SN   : 0x%02x%02x%02x%02x\n", vector[0], vector[1], vector[2], vector[3]);
		}
#if defined(CONFIG_CAS_NA_NOCS_DPT)
		status = dptGetCertCheckNumber(DPT_CERT_REPORT_CHECK_NUMBER, pxCertCheckNumber);
		if(status != CSD_NO_ERROR)
		{
			CMD_Printf("dptGetCertCheckNumber error(0x%x)\n", status);

			return CMD_ERR;
		}
		else
		{
			CMD_Printf("CERT REPORT Check Number   : 0x%02x%02x%02x%02x%02x%02x%02x%02x\n",
				pxCertCheckNumber[0], pxCertCheckNumber[1], pxCertCheckNumber[2], pxCertCheckNumber[3],
				pxCertCheckNumber[4], pxCertCheckNumber[5], pxCertCheckNumber[6], pxCertCheckNumber[7]);
		}
#endif
		/* Read fusing info */
		errCode = DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO, &fusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
		if (errCode != DI_ERR_OK)
		{
			CMD_Printf("DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO) error(0x%x)\n", errCode);

			return CMD_ERR;
		}
		else
		{
 			CMD_Printf("RAM Scramble: %s\n", (fusingInfo.ucRAMSCR != 0) ? "Enabled" : "Disabled");
#if defined(CONFIG_OTP_LOCK_NORFLASH)
			CMD_Printf("OTP         : %s\n", (fusingInfo.ucOTP != 0) ? "Enabled" : "Disabled");
#endif
 			CMD_Printf("SCS Version : 0x%x\n", fusingInfo.ucSCSVersioning);
			CMD_Printf("Flash Prot. : %s\n", (fusingInfo.ucFlashProtection != 0) ? "Enabled" : "Disabled");
		}
#else
		/* Read fusing info */
		errCode = DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO, &fusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
		if (errCode != DI_ERR_OK)
		{
			CMD_Printf("DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO) error(0x%x)\n", errCode);

			return CMD_ERR;
		}
		else
		{
			CMD_Printf("Secure boot : %s\n", (fusingInfo.ucSCS != 0) ? "Enabled" : "Disabled");
			CMD_Printf("JTAG        : %s\n", (fusingInfo.ucJTAG != 0) ? "Locked" : "Unlocked");
			CMD_Printf("Secure CW   : %s\n", (fusingInfo.ucCWE != 0) ? "Enabled" : "Disabled");
			CMD_Printf("RAM Scramble : %s\n", (fusingInfo.ucRAMSCR != 0) ? "Enabled" : "Disabled");
			CMD_Printf("Market ID   : 0x%x\n", fusingInfo.ucMarketID);
			CMD_Printf("Market ID 32bit : 0x%x\n", fusingInfo.ulMarketID);
			CMD_Printf("STB Owner ID   : 0x%x\n", fusingInfo.usSTBOwnerID);
#if defined(CONFIG_OTP_LOCK_NORFLASH)
			CMD_Printf("OTP         : %s\n", (fusingInfo.ucOTP != 0) ? "Enabled" : "Disabled");
#endif
			CMD_Printf("Chipset OTP : %s\n", (fusingInfo.ucOTPChipset != 0) ? "Enabled" : "Disabled");
			CMD_Printf("SCS Version : 0x%x\n", fusingInfo.ucSCSVersioning);
			CMD_Printf("Flash Prot. : %s\n", (fusingInfo.ucFlashProtection != 0) ? "Enabled" : "Disabled");
		}
#endif

#if defined(CONFIG_CAS_CX)
		{
			HUINT32	ulBootCodeDecryption;
			errCode = DRV_CSDGetHostBootCodeDescryptionSelect(&ulBootCodeDecryption);
			if (errCode != DI_ERR_OK)
			{
				CMD_Printf("DI_SYSTEM_GetSystemInfo(DI_SYS_FUSING_INFO) error(0x%x)\n", errCode);
				return CMD_ERR;
			}
			CMD_Printf("HostBootCodeDescryption : 0x%x\n", ulBootCodeDecryption);
		}
#endif

#if defined(CONFIG_CAS_NA_NOCS_3X)
		/* ChipsetExtension */
		{
			DI_ERR_CODE eDiErr = 0;
			unsigned char *pusChipsetExtension = NULL;

			pusChipsetExtension = (unsigned char *)VK_MEM_Alloc(DI_SYS_CHIPSET_EXTENTION_SIZE + 1);
			if(pusChipsetExtension == NULL)
			{
				CMD_Printf("[DI_SYS_CHIPSET_EXTENSION]: Out of Memory!\n");
				return CMD_ERR;
			}

			VK_memset((void *)pusChipsetExtension, 0, DI_SYS_CHIPSET_EXTENTION_SIZE);

			eDiErr = DI_SYSTEM_GetSystemInfo(DI_SYS_CHIPSET_EXTENSION, pusChipsetExtension, DI_SYS_CHIPSET_EXTENTION_SIZE);
			if (eDiErr != DI_ERR_OK)
			{
				CMD_Printf("DI_SYSTEM_GetSystemInfo(DI_SYS_CHIPSET_EXTENSION) error\n");
				VK_MEM_Free(pusChipsetExtension);
				return CMD_ERR;
			}

			CMD_Printf("CHIPSET_EXTENSION : %s\n", pusChipsetExtension);

			VK_MEM_Free(pusChipsetExtension);
		}

		/* ChipsetCut */
		{
			DI_ERR_CODE eDiErr = 0;
			unsigned char *pusChipsetCut = NULL;

			pusChipsetCut = (unsigned char *)VK_MEM_Alloc(DI_SYS_CHIPSET_CUT_SIZE + 1);
			if(pusChipsetCut == NULL)
			{
			    CMD_Printf("[DI_SYS_CHIPSET_CUT]: Out of Memory!\n");
			    return CMD_ERR;
			}

			VK_memset((void *)pusChipsetCut, 0, DI_SYS_CHIPSET_CUT_SIZE + 1);

			eDiErr = DI_SYSTEM_GetSystemInfo(DI_SYS_CHIPSET_CUT, pusChipsetCut, DI_SYS_CHIPSET_CUT_SIZE);
			if (eDiErr != DI_ERR_OK)
			{
			    CMD_Printf("DI_SYSTEM_GetSystemInfo(DI_SYS_CHIPSET_CUT) error\n");
			    VK_MEM_Free(pusChipsetCut);
			    return CMD_ERR;
			}

			CMD_Printf("CHIPSET_CUT : %s\n", pusChipsetCut);

			VK_MEM_Free(pusChipsetCut);
		}

#if defined(CONFIG_CAS_NA_NOCS_DPT)
		/* CertReportCheckNum */
		{
			DI_ERR_CODE eDiErr = 0;
			unsigned char *pusCertReportCheckNum = NULL;

			pusCertReportCheckNum = (unsigned char *)VK_MEM_Alloc(DI_SYS_CERT_REPORT_CHECK_NUM_SIZE);
			if(pusCertReportCheckNum == NULL)
			{
			    CMD_Printf("[DI_SYS_CERT_REPORT_CHECK_NUM]: Out of Memory!\n");
			}

			VK_memset((void *)pusCertReportCheckNum, 0, DI_SYS_CERT_REPORT_CHECK_NUM_SIZE);

			eDiErr = DI_SYSTEM_GetSystemInfo(DI_SYS_CERT_REPORT_CHECK_NUM, pusCertReportCheckNum, DI_SYS_CERT_REPORT_CHECK_NUM_SIZE);
			if (eDiErr != DI_ERR_OK)
			{
			    CMD_Printf("DI_SYSTEM_GetSystemInfo(DI_SYS_CERT_REPORT_CHECK_NUM) error\n");
			    VK_MEM_Free(pusCertReportCheckNum);
			}

			CMD_Printf("CERT REPORT Check Number   : 0x%02x%02x%02x%02x0x%02x%02x%02x%02x\n",
							pusCertReportCheckNum[0], pusCertReportCheckNum[1], pusCertReportCheckNum[2], pusCertReportCheckNum[3],
							pusCertReportCheckNum[4], pusCertReportCheckNum[5], pusCertReportCheckNum[6], pusCertReportCheckNum[7]);

			VK_MEM_Free(pusCertReportCheckNum);
		}
#endif
#endif
		iResult = CMD_OK;
	}
	else if CMD_IS("fuse")
	{
		DI_SYS_FUSING_INFO_t fusingInfo;
		DI_ERR_CODE errCode;

		VK_memset(&fusingInfo, 0, sizeof(DI_SYS_FUSING_INFO_t));

		if PARAM_IS("scs")
		{
			fusingInfo.ucSCS = TRUE;
			fusingInfo.ulParms[0] = 0;	/* Block size, if we uses BSECK, don't need block size */
			READABLE_IN_DEC(HWTEST_PARAM1, fusingInfo.ulParms[1]);	/* Key index */
			errCode = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &fusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
			if (errCode != DI_ERR_OK)
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, SCS) error(0x%x)\n", errCode);

				return CMD_ERR;
			}
			else
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, SCS) OK\n");
			}
		}
		else if PARAM_IS("jtag")
		{
			fusingInfo.ucJTAG = TRUE;
			errCode = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &fusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
			if (errCode != DI_ERR_OK)
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, JTAG) error(0x%x)\n", errCode);

				return CMD_ERR;
			}
			else
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, JTAG) OK\n");
			}
		}
		else if PARAM_IS("cwe")
		{
			fusingInfo.ucCWE = TRUE;
			errCode = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &fusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
			if (errCode != DI_ERR_OK)
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, CWE) error(0x%x)\n", errCode);

				return CMD_ERR;
			}
			else
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, CWE) OK\n");
			}
		}
		else if PARAM_IS("ramscr")
		{
			fusingInfo.ucRAMSCR = TRUE;
			errCode = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &fusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
			if (errCode != DI_ERR_OK)
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, RAM Scrambling) error(0x%x)\n", errCode);

				return CMD_ERR;
			}
			else
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, RAM Scrambling) OK\n");
			}
		}
		else if PARAM_IS("stbownerid")
		{
			READABLE_IN_HEX(HWTEST_PARAM1, fusingInfo.usSTBOwnerID);
			errCode = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &fusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
			if (errCode != DI_ERR_OK)
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, STBOwnerID) error(0x%x)\n", errCode);

				return CMD_ERR;
			}
			else
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, STBOwnerID) OK\n");
			}
		}

		else if PARAM_IS("datasection")
		{
			DI_SYS_OTP_DATA_SECTION_INFO_t	otpDataSectionInfo;
			unsigned char dataSectionData[32] = {
				0x95, 0x34, 0x8a, 0x0b, 0xa9, 0xa6, 0xac, 0x9e,
				0xf5, 0x05, 0xbf, 0x37, 0x92, 0xaa, 0x56, 0x88,
				0x01, 0xeb, 0xf8, 0xc0, 0x85, 0x23, 0x9b, 0x11,
				0x86, 0xcd, 0x57, 0x1c, 0xca, 0x7e, 0xdb, 0xc5
			};

			otpDataSectionInfo.ucIndex = 0;
			VK_MEM_Memcpy((void *)&otpDataSectionInfo.ucDataSection[0], (void *)&dataSectionData[0], 32);

			errCode = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSE_OTP_DATA_SECTION, &otpDataSectionInfo, sizeof(DI_SYS_OTP_DATA_SECTION_INFO_t));
			if (errCode != DI_ERR_OK)
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSE_OTP_DATA_SECTION) error(0x%x)\n", errCode);

				return CMD_ERR;
			}
			else
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSE_OTP_DATA_SECTION) OK\n");
			}

			return CMD_OK;
		}
#if defined(CONFIG_CAS_NA)
		else if PARAM_IS("casn")
		{
			DRV_Error 	eDrvErr=DRV_ERR;
			HUINT8		*buf = NULL;
			HUINT8		*pk_addr;
			HUINT8		ucCaSn[4] = {0x00,0x00,0x00,0x00};
			HUINT8		ucCaSnTmp[4] = {0x00,0x00,0x00,0x00};
			stCrypt_t	stCryptInfo;

			/* Initialize */
			VK_MEM_Memset(&stCryptInfo, 0x00, sizeof(stCrypt_t));

			/* Memory Allocation */
			stCryptInfo.pSrcBuffer = (HUINT8*)DI_CRYPT_MemAlloc(NAGRA_PK_SIZE);
			if(stCryptInfo.pSrcBuffer == NULL)
			{
				CMD_Printf("[%s:%d] pSrcBuffer is still NULL!! \n\r", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			stCryptInfo.pDstBuffer = (HUINT8*)DI_CRYPT_MemAlloc(NAGRA_PK_SIZE);
			if(stCryptInfo.pDstBuffer == NULL)
			{
				CMD_Printf("[%s:%d] pDstBuffer is still NULL!! \n\r", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			buf = (HUINT8*)VK_MEM_Alloc(NAGRA_PK_LOGICAL_OFFSET + NAGRA_PK_SIZE);
			if(buf == NULL)
			{
				CMD_Printf("[%s:%d] buf is still NULL!! \n\r", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			/* Read PK from flash memory according to flash map */
#if defined(CONFIG_NOR_FLASH)
			errCode = DI_FLASH_Read(NAGRA_PK_LOGICAL_OFFSET,
									buf,
									NAGRA_PK_LOGICAL_OFFSET + NAGRA_PK_SIZE);
#else
			errCode = DI_NANDFLASH_Read(NAGRA_PK_LOGICAL_OFFSET,
									buf,
									NAGRA_PK_LOGICAL_OFFSET + NAGRA_PK_SIZE);
#endif
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_NANDFLASH_Read() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			/* Copy to Enc/Dec source buffer */
			VK_memcpy(stCryptInfo.pSrcBuffer, (HUINT8*)buf, NAGRA_PK_SIZE);

			/* Set crypt info struct */
			stCryptInfo.etCryptKey = DI_CRYPT_INTERNAL_CAS_KEY;
			stCryptInfo.ulBuffSize = NAGRA_PK_SIZE;
			stCryptInfo.etCryptMode = DI_CRYPT_TDES;

			/* Use decrypt api */
			errCode = DI_CRYPT_Decrypt(&stCryptInfo);
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_CRYPT_Decrypt() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			pk_addr = (HUINT8 *)stCryptInfo.pDstBuffer;
			/* the paring key file is made-up as PK_LENGTH_SIZE(4) + STB_CA_SN(4) + PK(N) + PK_SIGN(128) + CRC(2) */
			//*pulCaSn = (HUINT32)(pk_addr[4] << 24 | pk_addr[5] << 16 | pk_addr[6] << 8 | pk_addr[7]);
			ucCaSn[0] = pk_addr[4];
			ucCaSn[1] = pk_addr[5];
			ucCaSn[2] = pk_addr[6];
			ucCaSn[3] = pk_addr[7];

			CMD_Printf("serialNumber[0] : %02x\n", ucCaSn[0]);
			CMD_Printf("serialNumber[1] : %02x\n", ucCaSn[1]);
			CMD_Printf("serialNumber[2] : %02x\n", ucCaSn[2]);
			CMD_Printf("serialNumber[3] : %02x\n", ucCaSn[3]);

			eDrvErr=DRV_CSDSetStbCaSn(ucCaSn);
			if(eDrvErr != DRV_OK)
			{
				CMD_Printf("\nError = STB CA SN is not set \n");
				return CMD_ERR;
			}

			eDrvErr=DRV_CSDGetStbCaSn(ucCaSnTmp);
			if(eDrvErr != DRV_OK)
			{
				CMD_Printf("\nError = STB CA SN is not get \n");
				return CMD_ERR;
			}

			if((ucCaSnTmp[0] != pk_addr[4]) || (ucCaSnTmp[1] != pk_addr[5]) || (ucCaSnTmp[2] != pk_addr[6]) || (ucCaSnTmp[3] != pk_addr[7]))
			{
				CMD_Printf("\nError = STB CA SN is not matched 0x0%x0%x0%x0%x\n",ucCaSnTmp[3],ucCaSnTmp[2],ucCaSnTmp[1],ucCaSnTmp[0]);
			}

			return CMD_OK;
		}
		else if PARAM_IS("bootmode")
		{
			DRV_Error eDrvErr = DRV_ERR;
			DRV_CSD_BootMode eCsdBootMode = DRV_CSD_BOOT_FROM_NAND;

			if PARAM1_IS("nand")
			{
				CMD_Printf("set boot mode to nand\n");
				eCsdBootMode = DRV_CSD_BOOT_FROM_NAND;
			}
			else if PARAM1_IS("nor")
			{
				CMD_Printf("set boot mode to nor\n");
				eCsdBootMode = DRV_CSD_BOOT_FROM_NOR;
			}
			else if PARAM1_IS("spi")
			{
				CMD_Printf("set boot mode to spi\n");
				eCsdBootMode = DRV_CSD_BOOT_FROM_SPI;
			}
			else
			{
				CMD_Printf("\nError = Invalid choice. shell be set to nand, nor and spi only\n");
				return CMD_ERR;
			}

			eDrvErr = DRV_CSDSetBootMode(eCsdBootMode);
			if(eDrvErr != DRV_OK)
			{
				CMD_Printf("\nError = eCsdBootMode is not set \n");
				return CMD_ERR;
			}

			return CMD_OK;
		}
#if defined(CONFIG_CAS_NA_NOCS_3X)
		else if PARAM_IS("ddrscramble")
		{
			DRV_Error eDrvErr=DRV_ERR;
			eDrvErr=DRV_CSDSetRamScrambling();
			if(eDrvErr != DRV_OK)
			{
				CMD_Printf("\nError = RamScrambling is not set \n");
				return CMD_ERR;
			}
			CMD_Printf("\nRamScrambling is set \n");
			return CMD_OK;
		}
		else if PARAM_IS("encfpk")
		{
			DRV_Error 	eDrvErr=DRV_ERR;
			eDrvErr = DRV_CSDSetEncryptFPK();
			if(eDrvErr != DRV_OK)
			{
				CMD_Printf("[%s:%d] DRV_CSDSetEncryptFPK error !!\n\r",__FUNCTION__,__LINE__);
				return CMD_ERR;
			}

			return CMD_OK;

		}
		else if PARAM_IS("scsversioning")
		{
			DRV_Error eDrvErr = DRV_ERR;
			HUINT8 ucVersion = 0;
			HUINT32 ulVersion = 0;

			if (HWTEST_PARAM1 == NULL)
			{
				CMD_Printf("[Usage] crypto scsversioning [write value(hex)]\n");
				return CMD_ERR;
			}

			if (READABLE_IN_HEX(HWTEST_PARAM1, ulVersion) == 1)
			{
				ucVersion = ulVersion;

				eDrvErr = DRV_CSDSetSCSVersioning(1, &ucVersion);
				if (eDrvErr != DRV_OK)
				{
					CMD_Printf("\nError = ucVersion is not set \n");
					return CMD_ERR;
				}

				eDrvErr = DRV_CSDGetSCSVersioning(1, &ucVersion);
				if (eDrvErr != DRV_OK)
				{
					CMD_Printf("\nError = ucVersion is not get \n");
					return CMD_ERR;
				}
				CMD_Printf("\n SCS Versioning is 0x0%x\n", ucVersion);
			}
			else
			{
				CMD_Printf("Invalid Parameter : [Usage] crypto scsversioning [write value(hex)]\n");
				return CMD_ERR;
			}
			return CMD_OK;
		}
		else if PARAM_IS("flashprotection")
		{
			DRV_Error eDrvErr=DRV_ERR;
			eDrvErr = DRV_CSDSetFlashProtection();
			if(eDrvErr != DRV_OK)
			{
				CMD_Printf("\nError = Flash Protection is not set \n");
				return CMD_ERR;
			}
			CMD_Printf("\n Flash Protection is set \n");
			return CMD_OK;
		}
#endif
#endif
#if 1//defined(CONFIG_CAS_NA) || defined(CONFIG_CAS_CX) || defined(CONFIG_CAS_VE)
		else if PARAM_IS("marketid")
		{
			DRV_Error eDrvErr = DRV_ERR;
			HUINT8 ucMarketID[4] = {0,};;
			HUINT32 ulMarketID = 0;

			if (HWTEST_PARAM1 == NULL)
			{
				CMD_Printf("[Usage] crypto marketid [write value(hex)]\n");
				return CMD_ERR;
			}

			if (READABLE_IN_HEX(HWTEST_PARAM1, ulMarketID) == 1)
			{
				eDrvErr = DRV_CSDSetMarketID(ulMarketID);
				if (eDrvErr != DRV_OK)
				{
					CMD_Printf("\nError = MartketSegmentId is not set \n");
					return CMD_ERR;
				}

				eDrvErr = DRV_CSDGetMarketID(ucMarketID);
				if (eDrvErr != DRV_OK)
				{
					CMD_Printf("\nError = MartketSegmentId is not get \n");
					return CMD_ERR;
				}
				CMD_Printf("\n MarketID is 0x%02x%02x%02x%02x\n",ucMarketID[0],ucMarketID[1],ucMarketID[2],ucMarketID[3]);
			}
			else
			{
				CMD_Printf("Invalid Parameter : [Usage] crypto marketid [write value(hex)]\n");
				return CMD_ERR;
			}
			return CMD_OK;
		}
#endif
#if defined(CONFIG_CAS_CX)
		else if PARAM_IS("otpchipset")
		{
			fusingInfo.ucOTPChipset=TRUE;
			errCode = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &fusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
			if (errCode != DI_ERR_OK)
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, JTAG) error(0x%x)\n", errCode);

				return CMD_ERR;
			}
			else
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, JTAG) OK\n");
			}
			return CMD_OK;
		}
#endif
#if defined(CONFIG_OTP_LOCK_NORFLASH)
		else if PARAM_IS("norotplock")
		{
			fusingInfo.ucOTP = TRUE;
			errCode = DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, &fusingInfo, sizeof(DI_SYS_FUSING_INFO_t));
			if (errCode != DI_ERR_OK)
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, ucOTP) error(0x%x)\n", errCode);

				return CMD_ERR;
			}
			else
			{
				CMD_Printf("DI_SYSTEM_SetSystemInfo(DI_SYS_FUSING_INFO, ucOTP) OK\n");
			}
		}
#endif
		else
		{
			HWTEST_PARAM1 = HWTEST_PARAM1;

			CMD_Printf("Wrong crypto Command : crypto [info/fuse [scs/jtag/cwe/datasection/stbownerid]/read datasection]\n");
		}
	}
	else if CMD_IS("read")
	{
		DI_ERR_CODE errCode;

		if PARAM_IS("datasection")
		{
			DI_SYS_OTP_DATA_SECTION_INFO_t	otpDataSectionInfo;
			int 							i;

			otpDataSectionInfo.ucIndex = 0;
			VK_MEM_Memset(&otpDataSectionInfo.ucDataSection[0], 0x0, 32);

			errCode = DI_SYSTEM_GetSystemInfo(DI_SYS_FUSE_OTP_DATA_SECTION, &otpDataSectionInfo, sizeof(DI_SYS_OTP_DATA_SECTION_INFO_t));
			if (errCode != DI_ERR_OK)
			{
				CMD_Printf("DI_SYSTEM_GetSystemInfo(DI_SYS_FUSE_OTP_DATA_SECTION) error(0x%x)\n", errCode);

				return CMD_ERR;
			}
			else
			{
				CMD_Printf("DI_SYSTEM_GetSystemInfo(DI_SYS_FUSE_OTP_DATA_SECTION) OK\n");
			}

			CMD_Printf("[Data section]\n");
			for (i = 0; i < 32; i += 8)
			{
				CMD_Printf("(0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x)\n",
						otpDataSectionInfo.ucDataSection[i],
						otpDataSectionInfo.ucDataSection[i + 1],
						otpDataSectionInfo.ucDataSection[i + 2],
						otpDataSectionInfo.ucDataSection[i + 3],
						otpDataSectionInfo.ucDataSection[i + 4],
						otpDataSectionInfo.ucDataSection[i + 5],
						otpDataSectionInfo.ucDataSection[i + 6],
						otpDataSectionInfo.ucDataSection[i + 7]);
			}
		}
#if defined(CONFIG_CAS_NA)
		else if PARAM_IS("pk")
		{
			DI_SYS_OTP_DATA_SECTION_INFO_t	otpDataSectionInfo;
			int 							i = 0;
			int								fd = 0;

			HUINT8		buf[NAGRA_PK_SIZE] = {0, };

			stCrypt_t stCryptInfo;

			/* Initialize */
			VK_MEM_Memset(&stCryptInfo, 0x00, sizeof(stCrypt_t));

			/* Memory Allocation */
			stCryptInfo.pSrcBuffer = (HUINT8*)DI_CRYPT_MemAlloc(NAGRA_PK_SIZE);
			if(stCryptInfo.pSrcBuffer == NULL)
			{
				CMD_Printf("[%s:%d] pSrcBuffer is still NULL!! \n\r", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			stCryptInfo.pDstBuffer = (HUINT8*)DI_CRYPT_MemAlloc(NAGRA_PK_SIZE);
			if(stCryptInfo.pDstBuffer == NULL)
			{
				CMD_Printf("[%s:%d] pDstBuffer is still NULL!! \n\r", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			/* Read PK from flash memory according to flash map */
#if defined(CONFIG_NOR_FLASH)
			errCode = DI_FLASH_Read(NAGRA_PK_LOGICAL_OFFSET, buf, NAGRA_PK_SIZE);
#else
			errCode = DI_NANDFLASH_Read(NAGRA_PK_LOGICAL_OFFSET, buf, NAGRA_PK_SIZE);
#endif
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_NANDFLASH_Read() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

#if defined(DUMP_NA_PK)
			/* DEBUG::Dump */
			CMD_Printf("[%s:%d] FLASH DUMP", __FUNCTION__, __LINE__, buf);
			errCode = DI_UART_Dump(buf,	NAGRA_PK_SIZE, 16);
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_UART_Dump() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}
#endif

			/* Copy to Enc/Dec source buffer */
			VK_memcpy(stCryptInfo.pSrcBuffer, buf, NAGRA_PK_SIZE);

#if defined(DUMP_NA_PK)
			/* DEBUG::Dump */
			CMD_Printf("[%s:%d] SOURCE BUFFER DUMP", __FUNCTION__, __LINE__, buf);
			errCode = DI_UART_Dump(stCryptInfo.pSrcBuffer, NAGRA_PK_SIZE, 16);
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_UART_Dump() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}
#endif

			/* Set crypt info struct */
			stCryptInfo.etCryptKey = DI_CRYPT_INTERNAL_CAS_KEY;
			stCryptInfo.ulBuffSize = NAGRA_PK_SIZE;

			/* Use decrypt api */
			errCode = DI_CRYPT_Decrypt(&stCryptInfo);
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_CRYPT_Decrypt() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			/* Print output to terminal */
			CMD_Printf("[%s:%d] DST BUFFER DUMP", __FUNCTION__, __LINE__, buf);
			errCode = DI_UART_Dump(stCryptInfo.pDstBuffer, NAGRA_PK_SIZE, 16);
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_UART_Dump() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

#if defined(DUMP_NA_PK)
			/* Save PK to USB memory stick */
			fd = fopen("/media/decrypt_pk.bin", "wb");
			if (fd == NULL)
			{
				CMD_Printf("[%s:%d] can not open decrypt_pk.bin!\r\n", __FUNCTION__, __LINE__);
				return 1;
			}

			i = fwrite(stCryptInfo.pDstBuffer, 1, NAGRA_PK_SIZE, fd);
			if(i != NAGRA_PK_SIZE)
			{
				CMD_Printf("[%s:%d] write decrypt_pk.bin failed!! write len = (0x%X)\r\n", __FUNCTION__, __LINE__, i);
				return CMD_ERR;
			}

			CMD_Printf("%s:%d] Check /media/decrypt_pk.bin!!\r\n", __FUNCTION__, __LINE__);

			fclose(fd);
#endif
			return CMD_OK;
		}
#if defined(CONFIG_CAS_NA_NOCS_3X)
		else if PARAM_IS("fpk")
		{
			DRV_Error 	eDrvErr=DRV_ERR;
			HUINT8		*buf = NULL;

			buf = (HUINT8*)VK_MEM_Alloc(NAGRA_FPK_BLOCK_SIZE);
			if(buf == NULL)
			{
				CMD_Printf("[%s:%d] buf is still NULL!! \n\r", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			/* Read PK from flash memory according to flash map */
#if defined(CONFIG_NOR_FLASH)
			errCode = DI_FLASH_Read(NAGRA_FPK_LOGICAL_OFFSET,buf,NAGRA_FPK_BLOCK_SIZE);
#else
			errCode = DI_NANDFLASH_Read(NAGRA_FPK_LOGICAL_OFFSET,buf,NAGRA_FPK_BLOCK_SIZE);
#endif
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_NANDFLASH_Read() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			{
				int i=0;
				CMD_Printf("========== FPK 16 bytes ===========\n");
				for(i=0;i<NAGRA_FPK_BLOCK_SIZE;i++)
					CMD_Printf("0x%x ",buf[i]);

				CMD_Printf("\n===================================\n");
			}

			return CMD_OK;

		}
#endif
		else if PARAM_IS("csc")
		{
			DRV_Error 	eDrvErr = DRV_ERR;
			HUINT8		*pBuffer = NULL;
			HUINT32		ulCheckNumber = 0;
#if defined(CONFIG_CAS_NA_NOCS_DPT)
			TCsdStatus status = CSD_NO_ERROR;
			TUnsignedInt8  aComputeCheckNumber[4];
#endif

			pBuffer = (HUINT8*)VK_MEM_Alloc(NAGRA_CSC_BLOCK_SIZE);
			if(pBuffer == NULL)
			{
				CMD_Printf("[%s:%d] buf is still NULL!! \n\r", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			/* Read CSC from flash memory according to flash map */
#if defined(CONFIG_NOR_FLASH)
			errCode = DI_FLASH_Read(NAGRA_CSC_LOGICAL_OFFSET, pBuffer, NAGRA_CSC_BLOCK_SIZE);
#else
			errCode = DI_NANDFLASH_Read(NAGRA_CSC_LOGICAL_OFFSET, pBuffer, NAGRA_CSC_BLOCK_SIZE);
#endif
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_NANDFLASH_Read() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				VK_MEM_Free(pBuffer);
				return CMD_ERR;
			}

#if defined(DUMP_NA_CSC)
			/* DEBUG::Dump */
			CMD_Printf("[%s: %d] FLASH DUMP", __FUNCTION__, __LINE__);
			errCode = DI_UART_Dump(pBuffer,	NAGRA_CSC_BLOCK_SIZE, 16);
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_UART_Dump() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				VK_MEM_Free(pBuffer);
				return CMD_ERR;
			}
#endif
#if defined(CONFIG_CAS_NA_NOCS_DPT)
			if(dptVerifyCscData(pBuffer))
			{
				CMD_Printf("[%s:%d]  dptVerifyCscData returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				VK_MEM_Free(pBuffer);
				return CMD_ERR;
			}
			else
			{
				status = csdGetDataIntegrityCheckNumber(pBuffer, 16, aComputeCheckNumber);
				if(status != CSD_NO_ERROR)
				{
					CMD_Printf("[%s:%d]  csdGetDataIntegrityCheckNumber returned ERROR!!(%d)\r\n", __FUNCTION__, __LINE__, status);
					VK_MEM_Free(pBuffer);
					return CMD_ERR;
				}
			}
			CMD_Printf("[%s: %d] CSC Check Number : 0x%02X%02X%02X%02X\n", __FUNCTION__, __LINE__,
				aComputeCheckNumber[0], aComputeCheckNumber[1], aComputeCheckNumber[2], aComputeCheckNumber[3]);
#else
			eDrvErr = DRV_CSDGetCSCCheckNumber(&ulCheckNumber, pBuffer);
			if(eDrvErr != DRV_OK)
			{
				CMD_Printf("[%s:%d] DRV_CSDGetCSCCheckNumber() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				VK_MEM_Free(pBuffer);
				return CMD_ERR;
			}


			CMD_Printf("[%s: %d] CSC Check Number : 0x%X\n", __FUNCTION__, __LINE__, ulCheckNumber);
#endif

			VK_MEM_Free(pBuffer);
			return CMD_OK;
		}
#endif
	}
#if defined(CONFIG_CAS_NA)
	else if CMD_IS("write")
	{
		DI_ERR_CODE errCode;

		if PARAM_IS("pk")
		{
			DI_SYS_OTP_DATA_SECTION_INFO_t	otpDataSectionInfo;
			int		i = 0;
			FILE 	*fd;

			HUINT8		buf[NAGRA_PK_SIZE] = {0, };

			stCrypt_t stCryptInfo;

			/* Initialize */
			VK_MEM_Memset(&stCryptInfo, 0x00, sizeof(stCrypt_t));

			/* Memory Allocation */
			stCryptInfo.pSrcBuffer = (HUINT8*)DI_CRYPT_MemAlloc(NAGRA_PK_SIZE);
			if(stCryptInfo.pSrcBuffer == NULL)
			{
				CMD_Printf("[%s:%d] pSrcBuffer is still NULL!! \n\r", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			stCryptInfo.pDstBuffer = (HUINT8*)DI_CRYPT_MemAlloc(NAGRA_PK_SIZE);
			if(stCryptInfo.pDstBuffer == NULL)
			{
				CMD_Printf("[%s:%d] pDstBuffer is still NULL!! \n\r", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			/* Read PK from USB memory stick */
			fd = fopen("/media/pure_pk.bin", "r+b");
			if (fd == NULL)
			{
				CMD_Printf("[%s:%d] pure_pk.bin not found!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}


			fseek(fd, 0, SEEK_SET);
			i = fread(buf, 1, NAGRA_PK_SIZE, fd);
			if(i <0 || i > NAGRA_PK_SIZE)
			{
				CMD_Printf("[%s:%d] read pure_pk.bin failed!! write len = (0x%X)\r\n", __FUNCTION__, __LINE__, i);
				return CMD_ERR;
			}
#if defined(DUMP_NA_PK)
			/* DEBUG::Dump */
			CMD_Printf("[%s:%d] FLASH DUMP", __FUNCTION__, __LINE__, buf);
			errCode = DI_UART_Dump(buf,	NAGRA_PK_SIZE, 16);
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_UART_Dump() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}
#endif
			fclose(fd);

			/* Copy to Enc/Dec source buffer */
			VK_MEM_Memcpy(stCryptInfo.pSrcBuffer, buf, NAGRA_PK_SIZE);

#if defined(DUMP_NA_PK)
			/* DEBUG::Dump */
			CMD_Printf("[%s:%d] SOURCE BUFFER DUMP", __FUNCTION__, __LINE__, buf);
			errCode = DI_UART_Dump(stCryptInfo.pSrcBuffer, NAGRA_PK_SIZE, 16);
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_UART_Dump() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}
#endif

			/* Set crypt info struct */
			stCryptInfo.etCryptKey = DI_CRYPT_INTERNAL_CAS_KEY;
			stCryptInfo.ulBuffSize = NAGRA_PK_SIZE;

			/* Use encrypt api */
			errCode = DI_CRYPT_Encrypt(&stCryptInfo);
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_CRYPT_Encrypt() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			/* Write PK to flash memory according to flash map */
#if defined(CONFIG_NOR_FLASH)
			errCode = DI_FLASH_WriteNotAligned(NAGRA_PK_LOGICAL_OFFSET, stCryptInfo.pDstBuffer, NAGRA_PK_SIZE);
#else
			errCode = DI_NANDFLASH_Write(NAGRA_PK_LOGICAL_OFFSET, stCryptInfo.pDstBuffer, NAGRA_PK_SIZE);
#endif
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_NANDFLASH_Write() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			VK_MEM_Memset(buf, 0x00, NAGRA_PK_SIZE);

			/* Write PK to flash memory according to flash map */
#if defined(CONFIG_NOR_FLASH)
			errCode = DI_FLASH_Read(NAGRA_PK_LOGICAL_OFFSET, buf, NAGRA_PK_SIZE);
#else
			errCode = DI_NANDFLASH_Read(NAGRA_PK_LOGICAL_OFFSET, buf, NAGRA_PK_SIZE);
#endif
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_NANDFLASH_Write() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			/* Print output to terminal */
			CMD_Printf("[%s:%d] DST BUFFER DUMP", __FUNCTION__, __LINE__, buf);
			errCode = DI_UART_Dump(buf, NAGRA_PK_SIZE, 16);
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_UART_Dump() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

#if defined(DUMP_NA_PK)
			/* Save PK to USB memory stick */
			fd = fopen("/media/encrypt_pk.bin", "wb");
			if (fd == NULL)
			{
				CMD_Printf("[%s:%d] can not open encrypt_pk.bin!\r\n", __FUNCTION__, __LINE__);
				return 1;
			}

			i = fwrite(buf, 1, NAGRA_PK_SIZE, fd);
			if(i != NAGRA_PK_SIZE)
			{
				CMD_Printf("[%s:%d] write encrypt_pk.bin failed!! write len = (0x%X)\r\n", __FUNCTION__, __LINE__, i);
				return CMD_ERR;
			}

			CMD_Printf("%s:%d] Check /media/encrypt_pk.bin!!\r\n", __FUNCTION__, __LINE__);

			fclose(fd);
#endif
			return CMD_OK;
		}
		else if PARAM_IS("csc")
		{
			DI_SYS_OTP_DATA_SECTION_INFO_t	otpDataSectionInfo;
			int		i = 0;
			FILE 	*fd;
			DRV_Error	eDrvErr = DRV_ERR;
			HUINT8* buf = NULL;
#if defined(CONFIG_CAS_NA_NOCS_DPT)
			TCsdStatus status = CSD_NO_ERROR;
			TUnsignedInt8  aComputeCheckNumber[4];
#else
			HUINT32		ulCheckNumber = 0;
#endif
			/* Memory Allocation */
			buf = (HUINT8*)DI_CRYPT_MemAlloc(NAGRA_CSC_BLOCK_SIZE);
			if(buf == NULL)
			{
				CMD_Printf("[%s:%d] buf is still NULL!! \n\r", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			/* Read PK from USB memory stick */
			fd = fopen("/media/chip_csc.bin", "r+b");
			if (fd == NULL)
			{
				CMD_Printf("[%s:%d] chip_csc.bin not found!\r\n", __FUNCTION__, __LINE__);
				DI_CRYPT_MemFree(buf);
				return CMD_ERR;
			}

			fseek(fd, 0, SEEK_SET);
			i = fread(buf, 1, NAGRA_CSC_BLOCK_SIZE, fd);
			if(i < 0)
			{
				CMD_Printf("[%s:%d] read chip_csc.bin failed!! write len = (0x%X)\r\n", __FUNCTION__, __LINE__, i);
				DI_CRYPT_MemFree(buf);
				return CMD_ERR;
			}
#if defined(DUMP_NA_CSC)
			/* DEBUG::Dump */
			CMD_Printf("[%s:%d] FLASH DUMP", __FUNCTION__, __LINE__, buf);
			errCode = DI_UART_Dump(buf,	i, 16);
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_UART_Dump() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				DI_CRYPT_MemFree(buf);
				return CMD_ERR;
			}
#endif
			fclose(fd);

#if defined(CONFIG_CAS_NA_NOCS_DPT)
			if(dptVerifyCscData(buf))
			{
				CMD_Printf("[%s:%d]  dptVerifyCscData returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				DI_CRYPT_MemFree(buf);
				return CMD_ERR;
			}
			else
			{
				CMD_Printf("[%s:%d]  dptVerifyCscData Pass!!\r\n", __FUNCTION__, __LINE__);
				status = csdGetDataIntegrityCheckNumber(buf, 16, aComputeCheckNumber);
				if(status != CSD_NO_ERROR)
				{
					CMD_Printf("[%s:%d]  csdGetDataIntegrityCheckNumber returned ERROR!!\r\n", __FUNCTION__, __LINE__);
					DI_CRYPT_MemFree(buf);
					return CMD_ERR;
				}
			}

			CMD_Printf("[%s: %d] CSC Check Number : 0x%02X%02X%02X%02X\n", __FUNCTION__, __LINE__,
				aComputeCheckNumber[0], aComputeCheckNumber[1], aComputeCheckNumber[2], aComputeCheckNumber[3]);
#else
			eDrvErr = DRV_CSDGetCSCCheckNumber(&ulCheckNumber, buf);
			if(eDrvErr != DRV_OK)
			{
				CMD_Printf("[%s:%d] DRV_CSDGetCSCCheckNumber() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				DI_CRYPT_MemFree(buf);
				return CMD_ERR;
			}
			
			CMD_Printf("[%s: %d] CSC Check Number : 0x%X\n", __FUNCTION__, __LINE__, ulCheckNumber);		
#endif
			
			/* Write CSC to flash memory according to flash map */
#if defined (CONFIG_NOR_FLASH)
			errCode = DI_FLASH_Write(NAGRA_CSC_LOGICAL_OFFSET, buf, i);
#else			
			errCode = DI_NANDFLASH_Write(NAGRA_CSC_LOGICAL_OFFSET, buf, i);
#endif
			if(errCode != DI_ERR_OK)
			{
				CMD_Printf("[%s:%d] DI_NANDFLASH_Write() returned ERROR!!\r\n", __FUNCTION__, __LINE__);
				DI_CRYPT_MemFree(buf);
				return CMD_ERR;
			}

			DI_CRYPT_MemFree(buf);
			return CMD_OK;
		}
	}
#endif
	else if CMD_IS("dec")
	{
		DI_ERR_CODE errCode;
		HUINT8 ucReadFileName[128];
		HUINT8 ucWriteFileName[128];
		HUINT8 ucBaseName[128];
		HUINT8 ucCMD[256];
		stCrypt_t stCryptInfo;
		stCrypt_t stCryptInfoEnc;
		int nNameSize;
		int nReadFD;
		int nWriteFD;
		int nRet;
		HINT64 nFileSize;
		HUINT32 unOperationSize;
		struct stat FileStat;
		int nLoopCnt;
		int nRest;
		int n;
		HUINT32 ulInputChipId = 0;
		HUINT8 szInputSerialNoStr[12];
		HUINT32 ulInputSNlen = 0;
		HUINT8 aucDecKey[DI_PVR_CRYPTO_KEY_LEN];
		HBOOL bReEncryt = FALSE;
		HUINT8 *pInputBuf;
		HUINT8 *pDecBuf;
		HUINT8 *pOutputBuf;
		unsigned long ulStartTime;
		unsigned long ulEndTime;

		if PARAM_IS("ts")
		{
			if(!HWTEST_PARAM1)
			{
				CMD_Printf("Please insert file name!! \n\r");
				CMD_Printf("ex) crypto dec ts [TS filename] [re-encrypt(true/false)] [CHIP ID(HEX)] [SN]\n\r");
				CMD_Printf("    1) decrypt only with fixed key : crypto dec ts /mnt/hd2/hdd_write_test1.ts false\n\n\r");
				CMD_Printf("    2) decrypt and re-encrypt with fixed key : crypto dec ts /mnt/hd2/hdd_write_test1.ts true\n\n\r");
				CMD_Printf("    3) decrypt only with CHIP ID and SN : crypto dec ts /mnt/hd2/hdd_write_test1.ts false dc5ce99b 19T140200000\n\n\r");
				CMD_Printf("    4) decrypt and re-encrypt with CHIP ID and SN : crypto dec ts /mnt/hd2/hdd_write_test1.ts true dc5ce99b 19T140200000\n\n\r");
				return CMD_ERR;

			}

			nNameSize = VK_strlen(HWTEST_PARAM1) - VK_strlen(".ts");
			VK_MEM_Memcpy(ucBaseName, HWTEST_PARAM1, nNameSize);
			ucBaseName[nNameSize] = '\0';
			CMD_Printf("BASE NAME : %s\n", ucBaseName);

			VK_sprintf(ucReadFileName, "%s%s", ucBaseName, ".ts");

			if PARAM2_IS("true")
			{
				bReEncryt = TRUE;
				VK_sprintf(ucCMD, "cp -rf %s%s %s%s", ucBaseName, ".info", ucBaseName, "_encrypt.info");
				VK_SYSTEM_Command(ucCMD);
				VK_sprintf(ucCMD, "cp -rf %s%s %s%s", ucBaseName, ".nts", ucBaseName, "_encrypt.nts");
				VK_SYSTEM_Command(ucCMD);
				VK_sprintf(ucWriteFileName, "%s%s", ucBaseName,"_encrypt.ts");
			}
			else
			{
				bReEncryt = FALSE;
				VK_sprintf(ucCMD, "cp -rf %s%s %s%s", ucBaseName, ".info", ucBaseName, "_decrypt.info");
				VK_SYSTEM_Command(ucCMD);
				VK_sprintf(ucCMD, "cp -rf %s%s %s%s", ucBaseName, ".nts", ucBaseName, "_decrypt.nts");
				VK_SYSTEM_Command(ucCMD);
				VK_sprintf(ucWriteFileName, "%s%s", ucBaseName,"_decrypt.ts");
			}

			unOperationSize = FILE_CHUNK_SIZE;

			ulStartTime = 	VK_TIMER_GetSystemTick()/1000;
			CMD_Printf("[CRYPTO] Descrpt Operation!(%d)\n", ulStartTime);
			CMD_Printf(" - Read File : %s\n", ucReadFileName);
			CMD_Printf(" - Write File : %s\n", ucWriteFileName);

			//nReadFD = open(ucReadFileName, O_RDONLY | O_LARGEFILE | O_DIRECT, FILE_MODE);
			nReadFD = open(ucReadFileName, O_RDONLY | O_LARGEFILE, FILE_MODE);
			if(nReadFD < 0)
			{
				CMD_Printf("Error(%s) : Read File(open) : %s\n", strerror(errno), ucReadFileName);
				return CMD_ERR;
			}

			if(-1 == fstat(nReadFD, &FileStat))
			{
				CMD_Printf("Error : fstat(%s) : %s\n", strerror(errno), ucReadFileName);
				return CMD_ERR;
			}
	     	nFileSize = (HINT64)FileStat.st_size;
			nLoopCnt = 	nFileSize / unOperationSize;
			nRest = 	nFileSize % unOperationSize;
			CMD_Printf(" - Read File Size : %lld Bytes, unOperationSize(%d), LoopCnt(%d), rest(%d)\n", nFileSize, unOperationSize, nLoopCnt, nRest);

			//nWriteFD = open(ucWriteFileName, O_CREAT | O_TRUNC | O_WRONLY | O_LARGEFILE | O_DIRECT | O_SYNC, FILE_MODE);
			nWriteFD = open(ucWriteFileName, O_CREAT | O_TRUNC | O_WRONLY | O_LARGEFILE | O_SYNC, FILE_MODE);
			if(nWriteFD < 0)
			{
				CMD_Printf("Error : Write File(open) : %s\n", ucWriteFileName);
				return CMD_ERR;
			}

			VK_MEM_Memset(&stCryptInfo, 0x00, sizeof(stCrypt_t));

			pInputBuf = (HUINT8*)DI_CRYPT_MemAlloc(unOperationSize);
			if(pInputBuf == NULL)
			{
				CMD_Printf("[%s:%d] pInputBuf is still NULL!! \n\r", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			pOutputBuf = (HUINT8*)DI_CRYPT_MemAlloc(unOperationSize);
			if(pOutputBuf == NULL)
			{
				CMD_Printf("[%s:%d] pOutputBuf is still NULL!! \n\r", __FUNCTION__, __LINE__);
				return CMD_ERR;
			}

			if(	bReEncryt == TRUE)
			{
				pDecBuf = (HUINT8*)DI_CRYPT_MemAlloc(unOperationSize);
				if(pDecBuf == NULL)
				{
					CMD_Printf("[%s:%d] pDecBuf is still NULL!! \n\r", __FUNCTION__, __LINE__);
					return CMD_ERR;
				}
				/* decrypt */
				stCryptInfo.pSrcBuffer = pInputBuf;
				stCryptInfo.pDstBuffer = pDecBuf;

				stCryptInfo.etCryptMode = DI_CRYPT_TDES;
				stCryptInfo.etCryptKey = DI_CRYPT_USER_KEY;
				stCryptInfo.etCryptData = DI_CRYPT_STREAM;
				stCryptInfo.ulKeySize = DI_PVR_CRYPTO_KEY_LEN;  // 16 byte
				stCryptInfo.ulBuffSize = unOperationSize;


				/* re-encrypt */
				stCryptInfoEnc.pSrcBuffer = pDecBuf;
				stCryptInfoEnc.pDstBuffer = pOutputBuf;

				stCryptInfoEnc.etCryptMode = DI_CRYPT_TDES;
				stCryptInfoEnc.etCryptKey = DI_CRYPT_USER_KEY;
				stCryptInfoEnc.etCryptData = DI_CRYPT_STREAM;
				stCryptInfoEnc.ulKeySize = DI_PVR_CRYPTO_KEY_LEN;  // 16 byte
				stCryptInfoEnc.ulBuffSize = unOperationSize;

				stCryptInfoEnc.pKey = s_aucEncKey;
			}
			else
			{
				stCryptInfo.pSrcBuffer = pInputBuf;
				stCryptInfo.pDstBuffer = pOutputBuf;

				stCryptInfo.etCryptMode = DI_CRYPT_TDES;
				stCryptInfo.etCryptKey = DI_CRYPT_USER_KEY;
				stCryptInfo.etCryptData = DI_CRYPT_STREAM;
				stCryptInfo.ulKeySize = DI_PVR_CRYPTO_KEY_LEN;  // 16 byte
				stCryptInfo.ulBuffSize = unOperationSize;
			}

			if((HWTEST_PARAM3) && (HWTEST_PARAM4))
			{
				if((READABLE_IN_HEX(HWTEST_PARAM3, ulInputChipId) != 1))
				{
					CMD_Printf("error get ChipID(%s)\n", HWTEST_PARAM3);
					return CMD_ERR;
				}

				if(VK_strlen(HWTEST_PARAM4)<sizeof(szInputSerialNoStr))
				{
					CMD_Printf("SN length must be %d\n", sizeof(szInputSerialNoStr));
					return CMD_ERR;
				}

				VK_MEM_Memcpy(szInputSerialNoStr, HWTEST_PARAM4, sizeof(szInputSerialNoStr));

				VK_memset( aucDecKey, 0, 16 );
				VK_memcpy( aucDecKey, (char *)&ulInputChipId, 4 );
				VK_memcpy( &(aucDecKey[4]), (char *)szInputSerialNoStr, sizeof(szInputSerialNoStr) );

				for(n=0; n<DI_PVR_CRYPTO_KEY_LEN; n++)
				{
					CMD_Printf("DecKEY[%d] = 0x%x\n\r", n, aucDecKey[n]);
				}

				stCryptInfo.pKey = aucDecKey;

			}
			else
			{
				stCryptInfo.pKey = s_aucDecKey;
			}


			for(n=0; n<nLoopCnt; n++)
			{
		//		CMD_Printf(" Operation Count : %d / %d\n", n, nLoopCnt);

				nRet = read(nReadFD, stCryptInfo.pSrcBuffer, unOperationSize);
				if(nRet < 0)
				{
					CMD_Printf("Error(%s) : File(read) : %s\n", strerror(errno), ucReadFileName);
					return CMD_ERR;
				}

				errCode = DI_CRYPT_Decrypt( &stCryptInfo );

				if(errCode != DI_ERR_OK )
				{
					CMD_Printf("error DI_CRYPT_Decrypt: 0x%X\n", errCode);
					return CMD_ERR;
				}

				if(	bReEncryt == TRUE)
				{
					errCode = DI_CRYPT_Encrypt( &stCryptInfoEnc );

					if(errCode != DI_ERR_OK )
					{
						CMD_Printf("error DI_CRYPT_Encrypt: 0x%X\n", errCode);
						return CMD_ERR;
					}
				}


				nRet = write(nWriteFD, pOutputBuf, unOperationSize);
				if(nRet < 0)
				{
					CMD_Printf("Error(%s) : File(write) : %s\n", strerror(errno), ucWriteFileName);
				}
				fsync(nWriteFD);
			}

			if(nRest)
			{
				unOperationSize = (nRest / FILE_MIN_SIZE)*FILE_MIN_SIZE;
				CMD_Printf(" - File Rest Size : %d Bytes, unOperationSize(%d), dummy(%d)\n", nRest, unOperationSize, (nRest%FILE_MIN_SIZE));

				nRet = read(nReadFD, stCryptInfo.pSrcBuffer, unOperationSize);
				if(nRet < 0)
				{
					CMD_Printf("Error(%s) : File(read) : %s\n", strerror(errno), ucReadFileName);
					return CMD_ERR;
				}

				errCode = DI_CRYPT_Decrypt( &stCryptInfo );
				if(errCode != DI_ERR_OK )
				{
					CMD_Printf("error DI_CRYPT_Decrypt: 0x%X\n", errCode);
					return CMD_ERR;
				}

				if(	bReEncryt == TRUE)
				{
					errCode = DI_CRYPT_Encrypt( &stCryptInfoEnc );

					if(errCode != DI_ERR_OK )
					{
						CMD_Printf("error DI_CRYPT_Encrypt: 0x%X\n", errCode);
						return CMD_ERR;
					}
				}

				nRet = write(nWriteFD, pOutputBuf, unOperationSize);
				if(nRet < 0)
				{
					CMD_Printf("Error(%s) : File(write) : %s\n", strerror(errno), ucWriteFileName);
				}
				fsync(nWriteFD);
			}
			DI_CRYPT_MemFree( (void *)pInputBuf);
			DI_CRYPT_MemFree( (void *)pOutputBuf);
			if(	bReEncryt == TRUE)
			{
				DI_CRYPT_MemFree( (void *)pDecBuf);
			}
			fsync(nWriteFD);
			close(nReadFD);
			close(nWriteFD);
			ulEndTime = 	VK_TIMER_GetSystemTick()/1000;
			CMD_Printf("[CRYPTO] END of Descrpt Operation!(%d)-operation(%ds)\n", ulEndTime, ulEndTime - ulStartTime);
		}
	}
	else
	{
		HWTEST_PARAM1 = HWTEST_PARAM1;

		CMD_Printf("Wrong crypto Command : crypto [info/fuse [scs/jtag/cwe/datasection/stbownerid]/read datasection]\n");
	}

	return iResult;
}

