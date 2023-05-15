#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<string.h>
#endif

#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_flash.h"
#include "flash_map.h"
#include "drv_err.h"
#include "cmd_hwtest_interface.h"

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
#define PrintEnter()			DI_UART_Print("(+)%s\n", __FUNCTION__)
#define PrintExit()				DI_UART_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter()			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit()				while (0) ((int (*)(char *, ...)) 0)
#endif

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {DI_UART_Print(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define AssertDRV(arg1, arg2) if(arg1 != DRV_OK) {DI_UART_Print(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define UNUSED(x) (void)(x)

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define HWTEST_PARAM3	szParam3

#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM3=CMD_NextArg((char **)&szArgs);

#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define PARAM3_IS(x)	(HWTEST_PARAM3!=NULL && VK_strcmp(HWTEST_PARAM3, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)


int CMD_HwTest_Parser_EmmcFlash(void *szArgs)
{
	DI_ERR_CODE di_err = DI_ERR_OK;
	GET_ARGS;

	if (CMD_IS("info"))
	{
		iResult = CMD_OK;
	}
	else if (CMD_IS("check"))
	{
		HBOOL bReadOnly;

		if (PARAM_IS("db"))
		{
			if(DI_EMMC_CheckReadonlyMount(FLASH_DB,&bReadOnly)==0)
			{
				if(bReadOnly)
				{
					CMD_Printf("DB Mount : RO mode\n");
				}
				else
				{
					CMD_Printf("DB Mount : RW mode\n");
				}
			}
		}
		else if (PARAM_IS("backup"))
		{
			if(DI_EMMC_CheckReadonlyMount(FLASH_DBBACKUP,&bReadOnly)==0)
			{
				if(bReadOnly)
				{
					CMD_Printf("DB-Backup Mount : RO mode\n");
				}
				else
				{
					CMD_Printf("DB-Backup Mount : RW mode\n");
				}
			}
		}
		else if (PARAM_IS("user"))
		{
			if(DI_EMMC_CheckReadonlyMount(FLASH_DBUSER,&bReadOnly)==0)
			{
				if(bReadOnly)
				{
					CMD_Printf("DB-user Mount : RO mode\n");
				}
				else
				{
					CMD_Printf("DB-user Mount : RW mode\n");
				}
			}
		}
		else
		{
			CMD_Printf("emmcflash check db||backup||user\n");

		}
		iResult = 0;
		CMD_Printf("\n");
	}
	else if (CMD_IS("recreate"))
	{
		HBOOL bReadOnly;

		/* get information */
		if (PARAM_IS("db"))
		{
			DI_EMMC_RecreatePartition(FLASH_DB);
		}
		else if (PARAM_IS("backup"))
		{
			DI_EMMC_RecreatePartition(FLASH_DBBACKUP);
		}
		else if (PARAM_IS("user"))
		{
			DI_EMMC_RecreatePartition(FLASH_DBUSER);
		}
		else
		{
			CMD_Printf("emmcflash recreate db||backup||user\n");

		}

		iResult = 0;
		CMD_Printf("\n");
	}
	else if (CMD_IS("recovery"))
	{
		HUINT32 unSize;

		/* get information */
		if (PARAM_IS("create"))
		{
			READABLE_IN_DEC(HWTEST_PARAM1, unSize);
			DI_EMMC_CreateRecoveryPartition(unSize);
		}
		else if (PARAM_IS("delete"))
		{
			DI_EMMC_DeleteRecoveryPartition();
		}

		iResult = 0;
		CMD_Printf("\n");
	}
	else if (CMD_IS("nvram"))
	{
		/* get information */
		if (PARAM_IS("verify"))
		{
			#include "di_nvram.h"

			#define TMP_BUFFER 0x100000
			size_t fileSize;
			HUINT8 *tmp = NULL;
			HUINT32 ulCnt = 0;
			HUINT32 ulRest = 0;
			int i = 0;
			HUINT32 ulBaseAddr = 0;
			HUINT32 ulCRC32 = 0;
			HUINT8 aucMacAddr[6];
			char strMac[40];
			char ulSavedCRC32[20];
			unsigned char *ulSavingCRC32;
			HUINT32 ulSavedCRC32_Size = 4;

			VK_memset(strMac,0,sizeof(strMac));
			VK_memset(ulSavedCRC32,0,sizeof(ulSavedCRC32));

			ulBaseAddr = N_PART_NVRAM_RO_OFFSET;
			fileSize = (size_t)N_PART_NVRAM_RO_SIZE;

			tmp = DD_MEM_Alloc(fileSize);

			VK_memset(tmp,0,fileSize);

			di_err = DI_EMMC_Read(ulBaseAddr,tmp,fileSize);
			if(di_err != DI_ERR_OK)
			{
				CMD_Printf("DI_EMMC_Read error \n");
				DD_MEM_Free(tmp);
				goto end;
			}

			ulCRC32 = DRV_UTIL_GetCrc32(tmp,fileSize);
			CMD_Printf("######## NVRAM CRC32 0x%x\n",ulCRC32);

			DI_NVRAM_GetField(DI_NVRAM_FIELD_MAC_ADDR, 0, aucMacAddr, 6);
			VK_snprintf(strMac, sizeof(strMac), "CRC32_%02x%02x",aucMacAddr[4],aucMacAddr[5]);

			ulSavingCRC32 = (unsigned char*)&ulCRC32;
			di_err = DI_NVRAM_Read(DI_NVRAM_FS_PARTITION_RW,strMac,0,ulSavedCRC32,ulSavedCRC32_Size);
			if(di_err == 0)
			{
				CMD_Printf("######## SAVED Name %s CRC32 0x%02x%02x%02x%02x\n",strMac,ulSavedCRC32[0],ulSavedCRC32[1],ulSavedCRC32[2],ulSavedCRC32[3]);
				if(VK_memcmp(ulSavingCRC32,ulSavedCRC32,4))
				{
					CMD_Printf("###### CRC32 Error~~~~~~~~~~\n");
					di_err = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW,strMac,0,ulSavingCRC32,4);
				}
				else
				{
					CMD_Printf("###### CRC32 OK~~~~~~~~~~\n");
				}
			}
			else
			{
				di_err = DI_NVRAM_Write(DI_NVRAM_FS_PARTITION_RW,strMac,0,ulSavingCRC32,4);
			}
			DD_MEM_Free(tmp);

		}
		iResult = 0;
		CMD_Printf("\n");
	}
	else if (CMD_IS("image"))
	{
		#define TMP_BUFFER 0x100000
		int fileFd;
		off_t seekPos;
		size_t fileSize;
		HUINT8 *tmp = NULL;
		HUINT8 *Output = NULL;
		HUINT32 ulCnt = 0;
		HUINT32 ulRest = 0;
		int i = 0;
		HUINT32 ulBaseAddr = 0;

		if (PARAM_IS("write"))
		{
			if(PARAM1_IS("uocto"))
			{
				CMD_Printf("@@Image write uocto \n");
				fileFd = open("/media/uocto.bin", O_RDONLY);
				if (fileFd < 0)
				{
					CMD_Printf("Unable to open bin file\n");
					goto end;
				}
				ulBaseAddr = N_PART_MMS_OFFSET;
				fileSize = (size_t)N_PART_MMS_SIZE;
			}
			else if(PARAM1_IS("rootfs"))
			{
				CMD_Printf("@@Image write rootfs \n");
				fileFd = open("/media/root.squa", O_RDONLY);
				if (fileFd < 0)
				{
					CMD_Printf("Unable to open bin file\n");
					goto end;
				}
				ulBaseAddr = (N_PART_IMAGE0_OFFSET+N_IMAGE_ROOTFS_OFFSET);
				fileSize = (size_t)N_IMAGE_ROOTFS_SIZE;
			}
			else if(PARAM1_IS("kernel"))
			{
				CMD_Printf("@@Image write kernel \n");
				fileFd = open("/media/vmlinuz", O_RDONLY);
				if (fileFd < 0)
				{
					CMD_Printf("Unable to open bin file\n");
					goto end;
				}
				ulBaseAddr = (N_PART_IMAGE0_OFFSET+N_IMAGE_KERNEL_OFFSET);
				fileSize = (size_t)N_IMAGE_KERNEL_SIZE;
			}
			else if(PARAM1_IS("nvram_ro"))
			{
				CMD_Printf("@@Image write kernel \n");
				fileFd = open("/media/nvram_ro.bin", O_RDONLY);
				if (fileFd < 0)
				{
					CMD_Printf("Unable to open bin file\n");
					goto end;
				}
				ulBaseAddr = N_PART_NVRAM_RO_OFFSET;
				fileSize = (size_t)N_PART_NVRAM_RO_SIZE;
			}
			else if(PARAM1_IS("nvram_rw"))
			{
				CMD_Printf("@@Image write kernel \n");
				fileFd = open("/media/nvram_rw.bin", O_RDONLY);
				if (fileFd < 0)
				{
					CMD_Printf("Unable to open bin file\n");
					goto end;
				}
				ulBaseAddr = N_PART_NVRAM_RW_OFFSET;
				fileSize = (size_t)N_PART_NVRAM_RW_SIZE;
			}
			else
			{
				goto end;
			}

			CMD_Printf("@@ Image BaseAddr 0x%x \n",ulBaseAddr);
			CMD_Printf("@@ Image Size 0x%x \n",fileSize);

			seekPos = lseek(fileFd, 0, SEEK_END);
			if (seekPos < 0)
			{
				CMD_Printf("Unable to seek bin file size\n");
				close(fileFd);
				goto end;
			}
			fileSize = (size_t)seekPos;

			if (lseek(fileFd, 0, SEEK_SET) < 0)
			{
				CMD_Printf("Unable to get back to origin\n");
				close(fileFd);
				goto end;
			}

			tmp = DD_MEM_Alloc(TMP_BUFFER);
			if(tmp == NULL)
			{
				CMD_Printf("Error : DD_MEM_Alloc(Input), size(%d)\n", TMP_BUFFER);
				close(fileFd);
				goto end;

			}
			ulCnt = fileSize/TMP_BUFFER;
			ulRest = fileSize%TMP_BUFFER;

			for( i = 0 ; i < ulCnt; i++)
			{
				CMD_Printf("WRITE Addr 0x%x Size 0x%x\n",ulBaseAddr+(i*TMP_BUFFER),TMP_BUFFER);
				if (read(fileFd, (void *)tmp, TMP_BUFFER) != (ssize_t)TMP_BUFFER)
				{
					CMD_Printf("Unable to read all binfile\n");
					DD_MEM_Free(tmp);
					close(fileFd);
					goto end;
				}
				di_err = DI_EMMC_Write(ulBaseAddr+(i*TMP_BUFFER),tmp,TMP_BUFFER);
				if(di_err != DI_ERR_OK)
				{
					CMD_Printf("DI_EMMC_Write error \n");
					DD_MEM_Free(tmp);
					close(fileFd);
					goto end;
				}
			}

			if(ulRest !=0)
			{
				CMD_Printf("WRITE Addr 0x%x Size 0x%x\n",ulBaseAddr+(i*TMP_BUFFER),ulRest);
				if (read(fileFd, (void *)tmp, ulRest) != (ssize_t)ulRest)
				{
					CMD_Printf("Unable to read all binfile\n");
					DD_MEM_Free(tmp);
					close(fileFd);
					goto end;
				}
				di_err = DI_EMMC_Write(ulBaseAddr+(i*TMP_BUFFER),tmp,ulRest);
				if(di_err != DI_ERR_OK)
				{
					CMD_Printf("DI_EMMC_Write error \n");
					DD_MEM_Free(tmp);
					close(fileFd);
					goto end;
				}

			}
			close(fileFd);

		}
		else if (PARAM_IS("erase"))
		{
			if(PARAM1_IS("uocto"))
			{
				CMD_Printf("@@Image erase uocto \n");
				ulBaseAddr = N_PART_MMS_OFFSET;
				fileSize = (size_t)N_PART_MMS_SIZE;
			}
			else if(PARAM1_IS("rootfs"))
			{
				CMD_Printf("@@Image erase rootfs \n");
				ulBaseAddr = (N_PART_IMAGE0_OFFSET+N_IMAGE_ROOTFS_OFFSET);
				fileSize = (size_t)N_IMAGE_ROOTFS_SIZE;
			}
			else if(PARAM1_IS("kernel"))
			{
				CMD_Printf("@@Image erase kernel \n");
				ulBaseAddr = (N_PART_IMAGE0_OFFSET+N_IMAGE_KERNEL_OFFSET);
				fileSize = (size_t)N_IMAGE_KERNEL_SIZE;
			}
			else
			{
				goto end;
			}

			CMD_Printf("@@ Image BaseAddr 0x%x \n",ulBaseAddr);
			CMD_Printf("@@ Image Size 0x%x \n",fileSize);

			tmp = DD_MEM_Alloc(TMP_BUFFER);
			if(tmp == NULL)
			{
				CMD_Printf("Error : DD_MEM_Alloc(Input), size(%d)\n", TMP_BUFFER);
				goto end;

			}
			VK_memset(tmp,0,TMP_BUFFER);
			ulCnt = fileSize/TMP_BUFFER;
			ulRest = fileSize%TMP_BUFFER;

			for( i = 0 ; i < ulCnt; i++)
			{
				CMD_Printf("ERASE Addr 0x%x Size 0x%x\n",ulBaseAddr+(i*TMP_BUFFER),TMP_BUFFER);
				di_err = DI_EMMC_Write(ulBaseAddr+(i*TMP_BUFFER),tmp,TMP_BUFFER);
				if(di_err != DI_ERR_OK)
				{
					CMD_Printf("DI_EMMC_Write error \n");
					DD_MEM_Free(tmp);
					goto end;
				}
			}

			if(ulRest !=0)
			{
				CMD_Printf("ERASE Addr 0x%x Size 0x%x\n",ulBaseAddr+(i*TMP_BUFFER),ulRest);
				di_err = DI_EMMC_Write(ulBaseAddr+(i*TMP_BUFFER),tmp,ulRest);
				if(di_err != DI_ERR_OK)
				{
					CMD_Printf("DI_EMMC_Write error \n");
					DD_MEM_Free(tmp);
					goto end;
				}

			}

		}
		else if (PARAM_IS("read"))
		{
			if(PARAM1_IS("uocto"))
			{
				fileFd = open("/media/uocto.bin", O_CREAT|O_WRONLY|O_SYNC);
				if (fileFd < 0)
				{
					CMD_Printf("Unable to open bin file 0x%x\n",fileFd);
					goto end;
				}
				ulBaseAddr = N_PART_MMS_OFFSET;
				fileSize = (size_t)N_PART_MMS_SIZE;

			}
			else if(PARAM1_IS("rootfs"))
			{
				CMD_Printf("@@Image write rootfs \n");
				fileFd = open("/media/root.squa",O_CREAT|O_WRONLY|O_SYNC);
				if (fileFd < 0)
				{
					CMD_Printf("Unable to open bin file\n");
					goto end;
				}
				ulBaseAddr = (N_PART_IMAGE0_OFFSET+N_IMAGE_ROOTFS_OFFSET);
				fileSize = (size_t)N_IMAGE_ROOTFS_SIZE;
			}
			else if(PARAM1_IS("kernel"))
			{
				CMD_Printf("@@Image write kernel \n");
				fileFd = open("/media/vmlinuz", O_CREAT|O_WRONLY|O_SYNC);
				if (fileFd < 0)
				{
					CMD_Printf("Unable to open bin file\n");
					goto end;
				}
				ulBaseAddr = (N_PART_IMAGE0_OFFSET+N_IMAGE_KERNEL_OFFSET);
				fileSize = (size_t)N_IMAGE_KERNEL_SIZE;
			}			
			else if(PARAM1_IS("app0"))
			{
				fileFd = open("/media/app0.bin", O_CREAT|O_WRONLY|O_SYNC);
				if (fileFd < 0)
				{
					CMD_Printf("Unable to open bin file 0x%x\n",fileFd);
					goto end;
				}
				ulBaseAddr = N_PART_IMAGE0_OFFSET;
				fileSize = (size_t)N_PART_IMAGE0_SIZE;
			}
			else if(PARAM1_IS("app1"))
			{
				fileFd = open("/media/app1.bin", O_CREAT|O_WRONLY|O_SYNC);
				if (fileFd < 0)
				{
					CMD_Printf("Unable to open bin file 0x%x\n",fileFd);
					goto end;
				}
				ulBaseAddr = N_PART_IMAGE0_OFFSET;
				fileSize = (size_t)N_PART_IMAGE0_SIZE;
			}
			else if(PARAM1_IS("nvram_ro"))
			{
				fileFd = open("/media/nvram_ro.bin", O_CREAT|O_WRONLY|O_SYNC);
				if (fileFd < 0)
				{
					CMD_Printf("Unable to open bin file 0x%x\n",fileFd);
					goto end;
				}
				ulBaseAddr = N_PART_NVRAM_RO_OFFSET;
				fileSize = (size_t)N_PART_NVRAM_RO_SIZE;
			}

			tmp = DD_MEM_Alloc(TMP_BUFFER);

			VK_memset(tmp,0,TMP_BUFFER);
			ulCnt = fileSize/TMP_BUFFER;
			ulRest = fileSize%TMP_BUFFER;

			for( i = 0 ; i < ulCnt; i++)
			{
				CMD_Printf("Read Addr 0x%x Size 0x%x\n",ulBaseAddr+(i*TMP_BUFFER),TMP_BUFFER);
				di_err = DI_EMMC_Read(ulBaseAddr+(i*TMP_BUFFER),tmp,TMP_BUFFER);
				if(di_err != DI_ERR_OK)
				{
					CMD_Printf("DI_EMMC_Read error \n");
					DD_MEM_Free(tmp);
					goto end;
				}

				write(fileFd, tmp,TMP_BUFFER);
				fsync(fileFd);
			}

			if(ulRest !=0)
			{
				CMD_Printf("Read Addr 0x%x Size 0x%x\n",ulBaseAddr+(i*TMP_BUFFER),ulRest);
				di_err = DI_EMMC_Read(ulBaseAddr+(i*TMP_BUFFER),tmp,ulRest);
				if(di_err != DI_ERR_OK)
				{
					CMD_Printf("DI_EMMC_Read error \n");
					DD_MEM_Free(tmp);
					goto end;
				}
				write(fileFd, tmp,ulRest);
				fsync(fileFd);

			}
			close(fileFd);
			DD_MEM_Free(tmp);
		}
		iResult = CMD_OK;
	}
	end:

	return iResult;
}
