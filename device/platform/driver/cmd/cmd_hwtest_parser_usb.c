#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif

#include "htype.h"
#include "di_err.h"

#include "cmdtool.h"
#include "di_usb.h"

#include "nexus_platform.h"

#include "bstd_defs.h"
#include "breg_mem.h"
#if (BCHP_CHIP == 7250) || ((NEXUS_VERSION >= 1502) && (BCHP_CHIP == 7241))
#else
#include "bchp_usb_ehci.h"
#endif
#if (CONFIG_USB_HOSTCONTROLLER_CNT > 1)
#if ((BCHP_CHIP == 7439) && (BCHP_VER == BCHP_VER_B0)) || ((BCHP_CHIP == 7252) && (BCHP_VER == BCHP_VER_B0)) || ((NEXUS_VERSION >= 1502) && (BCHP_CHIP == 7241))
#else
#include "bchp_usb1_ehci.h"
#endif
#endif


/* global variables */
/* static variables */

/* return value of command function */
#define CMD_OK					0
#define CMD_ERR					1

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define UNUSED(x) (void)(x)

/* kernel driver 관련 임시 코드..   */
//#include "autoconf.h"
#define CONFIG_BCM_KERNEL_MODE
#ifdef CONFIG_BCM_KERNEL_MODE
#define FS_REG_ACCESS	0
#define FS_MEM_ACCESS	1
#define FS_RD_MODE		0
#define FS_WR_MODE		1

/* static functions */
static unsigned long FSREG_Read(void *RegHandle, unsigned long ofs)
{
	UNUSED(RegHandle);
	unsigned long rdValue;
	NEXUS_Platform_Registers_Read_Access(ofs, (unsigned long *)&rdValue);
	return rdValue;
}

static void FSREG_Write(void *RegHandle, unsigned int ofs, unsigned int data)
{
	UNUSED(RegHandle);
	//VK_printf("[hee.usermode] write at 0x%lx value=0x%lx\n", ofs, data);
	NEXUS_Platform_Registers_Write_Access(ofs, data);
}
#else
#define FSREG_Read	BREG_Read32
#define FSREG_Write	BREG_Write32
#endif

/* extern functions */

extern BREG_Handle g_hReg;
int CMD_HwTest_Parser_USB(void *szArgs)
{
//	int iResult=CMD_ERR;
	DI_ERR_CODE errorCode = DI_ERR_ERROR;
	HUINT32 atrlen;
	HUINT32 unDeviceId=0;

	GET_ARGS;

	UNUSED(szParam1);
	UNUSED(szParam2);

	if CMD_IS("power")
	{
		DI_USB_PWR_t OnOff;

		if (PARAM_IS("0") || PARAM_IS("off"))
		{
			OnOff = DI_USB_PWR_OFF;
			VK_printf("[CMD_HwTest_Parser_USB] DI_USB_PWR_Ctrl off \r\n");
		}
		else if (PARAM_IS("1") || PARAM_IS("on"))
		{
			VK_printf("[CMD_HwTest_Parser_USB] DI_USB_PWR_Ctrl on \r\n");
			OnOff = DI_USB_PWR_ON;
		}
		else
		{
			VK_printf("[CMD_HwTest_Parser_USB] error invalid command parameter on or off \r\n");
			OnOff = DI_USB_PWR_ON;
			return 1;
		}

		errorCode = DI_USB_PWR_Ctrl(OnOff);
		if (errorCode != DI_ERR_OK)
		{
			VK_printf("[CMD_HwTest_Parser_USB] ERROR: DI_USB_PWR_Ctrl \r\n");
			return iResult;
		}

		iResult = CMD_OK;
	}
	else 	if CMD_IS("info")
	{
		if PARAM_IS("0")
		{
			unDeviceId = 0;
		}
		else if PARAM_IS("1")
		{
			unDeviceId = 1;
		}

		iResult = CMD_OK;
	}
	else 	if CMD_IS("procinfo")
	{
		DI_USB_HOSTCONTROLLER_INFO_t stGetUsbHostControllerInfo;
		DI_USB_DEVICE_INFO_t stGetUsbDevInfo;
		DI_USB_DEVICE_CONFIGURATION_t *pstGetUsbDevConfigurationsInfo = NULL;
		DI_USB_DEVICE_INTERFACE_t *pstGetUsbDevInterfacesInfo = NULL;

		HUINT32 unUsbHostNum=0, unConfigNum=0, unInterfaceNum=0;
		HUINT32 i=0, j=0;

		DI_USB_GetUsbHostCount(&unUsbHostNum);

		for(i=1; i<=unUsbHostNum; i++)
		{
			VK_MEM_Memset(&stGetUsbHostControllerInfo, 0, sizeof(DI_USB_HOSTCONTROLLER_INFO_t));
			DI_USB_GetUsbHostInfo(i, &stGetUsbHostControllerInfo);

			CMD_Printf("============================================\n");
			CMD_Printf("Host[%d] Name          : %s\n", i, stGetUsbHostControllerInfo.aucHostName);
			CMD_Printf("Host[%d] Type          : %d\n", i, stGetUsbHostControllerInfo.eHostTye);
			CMD_Printf("Host[%d] USB Version  : %s\n", i, stGetUsbHostControllerInfo.aucHostUsbVer);
			CMD_Printf("Host[%d] Device Number : %d\n", i, stGetUsbHostControllerInfo.unDeviceNumOfEntries);

			if(stGetUsbHostControllerInfo.unDeviceNumOfEntries)
			{
				for(j=1; j<=stGetUsbHostControllerInfo.unDeviceNumOfEntries; j++)
				{
					VK_MEM_Memset(&stGetUsbDevInfo, 0, sizeof(DI_USB_DEVICE_INFO_t));
					DI_USB_GetUsbDeviceInfo(i, j, &stGetUsbDevInfo);

					CMD_Printf("********************************************\n");
					CMD_Printf("Host[%d] Dev[%d] DeviceNumber                 : %d\n", i, j, stGetUsbDevInfo.unDevNum);
					CMD_Printf("Host[%d] Dev[%d] USBVersion                   : %s\n", i, j, stGetUsbDevInfo.aucDevUsbVer);
					CMD_Printf("Host[%d] Dev[%d] DeviceClass                  : %02x\n", i, j, stGetUsbDevInfo.ucDevClass);
					CMD_Printf("Host[%d] Dev[%d] DeviceSubClass               : %02x\n", i, j, stGetUsbDevInfo.ucDevSubClass);
					CMD_Printf("Host[%d] Dev[%d] DeviceVersion                : %2x.%02x\n", i, j, stGetUsbDevInfo.unDevVer>>8, stGetUsbDevInfo.unDevVer&0xff);
					CMD_Printf("Host[%d] Dev[%d] DeviceProtocol               : %02x\n", i, j, stGetUsbDevInfo.ucDevProtocol);
					CMD_Printf("Host[%d] Dev[%d] ProductID                    : %02x\n", i, j, stGetUsbDevInfo.unDevProductId);
					CMD_Printf("Host[%d] Dev[%d] VendorID                     : %02x\n", i, j, stGetUsbDevInfo.unDevVendorId);
					CMD_Printf("Host[%d] Dev[%d] Manufacturer                 : %s\n", i, j, stGetUsbDevInfo.aucDevManufacturer);
					CMD_Printf("Host[%d] Dev[%d] ProductClass                 : %s\n", i, j, stGetUsbDevInfo.aucDevProduct);
					CMD_Printf("Host[%d] Dev[%d] SerialNumber                 : %s\n", i, j, stGetUsbDevInfo.aucDevSerialNumber);
					CMD_Printf("Host[%d] Dev[%d] Port                         : %02d\n", i, j, stGetUsbDevInfo.unDevPort);
					CMD_Printf("Host[%d] Dev[%d] Rate                         : %x\n", i, j, stGetUsbDevInfo.eDevSpeed);
					CMD_Printf("Host[%d] Dev[%d] Parent                       : %02d\n", i, j, stGetUsbDevInfo.ucDevParent);
					CMD_Printf("Host[%d] Dev[%d] Mxchildren                   : %d\n", i, j, stGetUsbDevInfo.unDevMxch);
					CMD_Printf("Host[%d] Dev[%d] IsSuspended                  : %x\n", i, j, stGetUsbDevInfo.bDevIsSuspended);
					CMD_Printf("Host[%d] Dev[%d] IsSelpPowered                : %x\n", i, j, stGetUsbDevInfo.bDevIsSelfPowered);
					CMD_Printf("Host[%d] Dev[%d] ConfigurationNumberOfEntries : %x\n", i, j, stGetUsbDevInfo.unDevConfigurationNumOfEntries);

					if(stGetUsbDevInfo.unDevConfigurationNumOfEntries)
					{
						pstGetUsbDevConfigurationsInfo = (DI_USB_DEVICE_CONFIGURATION_t*)VK_MEM_Alloc(sizeof(DI_USB_DEVICE_CONFIGURATION_t)*stGetUsbDevInfo.unDevConfigurationNumOfEntries);
						VK_MEM_Memset(pstGetUsbDevConfigurationsInfo, 0, (sizeof(DI_USB_DEVICE_CONFIGURATION_t)*stGetUsbDevInfo.unDevConfigurationNumOfEntries));

						DI_USB_GetUsbDevConfigurationsInfo(i, j, pstGetUsbDevConfigurationsInfo);

						for(unConfigNum=0; unConfigNum<stGetUsbDevInfo.unDevConfigurationNumOfEntries; unConfigNum++)
						{
							CMD_Printf("Host[%d] Dev[%d] Configuration[%d] ConfigurationNumber          : %d\n", i, j, unConfigNum, pstGetUsbDevConfigurationsInfo[unConfigNum].unDevConfigurationNum);
							CMD_Printf("Host[%d] Dev[%d] Configuration[%d] InterfaceNumberOfEntries     : %d\n", i, j, unConfigNum, pstGetUsbDevConfigurationsInfo[unConfigNum].unDevInterfaceNumOfEntries);

							if(pstGetUsbDevConfigurationsInfo[unConfigNum].unDevInterfaceNumOfEntries)
							{
								pstGetUsbDevInterfacesInfo = (DI_USB_DEVICE_INTERFACE_t*)VK_MEM_Alloc(sizeof(DI_USB_DEVICE_INTERFACE_t)*pstGetUsbDevConfigurationsInfo[unConfigNum].unDevInterfaceNumOfEntries);
								VK_MEM_Memset(pstGetUsbDevInterfacesInfo, 0, (sizeof(DI_USB_DEVICE_INTERFACE_t)*pstGetUsbDevConfigurationsInfo[unConfigNum].unDevInterfaceNumOfEntries));

								DI_USB_GetUsbDevInterfacesInfo(i, j, pstGetUsbDevInterfacesInfo);

								for(unInterfaceNum=0; unInterfaceNum<pstGetUsbDevConfigurationsInfo[unConfigNum].unDevInterfaceNumOfEntries; unInterfaceNum++)
								{
									CMD_Printf("Host[%d] Dev[%d] Configuration[%d] InterfaceNum[%d] InterfaceNumber              : %d\n", i, j, unConfigNum, unInterfaceNum, pstGetUsbDevInterfacesInfo[unInterfaceNum].unDevInterfaceNum);
									CMD_Printf("Host[%d] Dev[%d] Configuration[%d] InterfaceNum[%d] InterfaceClass               : %x\n", i, j, unConfigNum, unInterfaceNum, pstGetUsbDevInterfacesInfo[unInterfaceNum].ucDevInterfaceClass);
									CMD_Printf("Host[%d] Dev[%d] Configuration[%d] InterfaceNum[%d] InterfaceSubClass            : %x\n", i, j, unConfigNum, unInterfaceNum, pstGetUsbDevInterfacesInfo[unInterfaceNum].ucDevInterfaceSubClass);
									CMD_Printf("Host[%d] Dev[%d] Configuration[%d] InterfaceNum[%d] InterfaceProtocol            : %x\n", i, j, unConfigNum, unInterfaceNum, pstGetUsbDevInterfacesInfo[unInterfaceNum].ucDevInterfaceProtocol);
								}
								VK_MEM_Free(pstGetUsbDevInterfacesInfo);
							}
						}
						VK_MEM_Free(pstGetUsbDevConfigurationsInfo);
					}
					CMD_Printf("********************************************\n");
				}
			}
			CMD_Printf("============================================\n\n");
		}

		iResult = CMD_OK;
	} else if CMD_IS("overcurrent") {
		if PARAM_IS("stop")
		{
			DI_UART_Print("Watching USB_OVERRENT stops\n");
			//g_ulHwTestTaskJob&=~TJ_USB_OVERCURRENT;
		}
		else
		{
			DI_UART_Print("Watching USB_OVERRENT starts\n");
			//g_ulHwTestTaskJob|=TJ_USB_OVERCURRENT;
		}
		iResult=CMD_OK;
	} else if CMD_IS("test") {
		if PARAM_IS("port")
		{
			int ulPortIndex;
			if (READABLE_IN_DEC(HWTEST_PARAM1, ulPortIndex))
			{
				unsigned long ulVal;
				CMD_Printf("BCIP : %d Port = %d \n", BCHP_CHIP, ulPortIndex);

#if (BCHP_CHIP == 7325)
				ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_0);
				ulVal|=(1<<BCHP_USB_EHCI_PORTSC_0_SUSPEND_SHIFT);
				BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);
				VK_TASK_Sleep(100);

				ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_USBCMD);
				ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
				BREG_Write32(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
				VK_TASK_Sleep(100);

				ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_0);
				ulVal&=~BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_MASK;
				// pattern 4
				ulVal|=(4<<BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_SHIFT);
				BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);

				iResult=CMD_OK;
#elif (BCHP_CHIP == 7335)
				if (ulPortIndex==0)
				{
					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_0);
					ulVal|=(1<<BCHP_USB_EHCI_PORTSC_0_SUSPEND_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_USBCMD);
					ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
					BREG_Write32(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_0);
					ulVal&=~BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_MASK;
					// pattern 4
					ulVal|=(4<<BCHP_USB_EHCI_PORTSC_1_PORT_TEST_CONTROL_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);

					iResult=CMD_OK;
				}
				else if (ulPortIndex==1)
				{
					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_1);
					ulVal|=(1<<BCHP_USB_EHCI_PORTSC_1_SUSPEND_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_1, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_USBCMD);
					ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
					BREG_Write32(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_1);
					ulVal&=~BCHP_USB_EHCI_PORTSC_1_PORT_TEST_CONTROL_MASK;
					// pattern 4
					ulVal|=(4<<BCHP_USB_EHCI_PORTSC_1_PORT_TEST_CONTROL_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_1, ulVal);

					iResult=CMD_OK;
				}
#elif (BCHP_CHIP == 7346)
				if (ulPortIndex==0)
				{
					ulVal=FSREG_Read(g_hReg, BCHP_USB_EHCI_PORTSC_0);
					ulVal|=(1<<BCHP_USB_EHCI_PORTSC_0_SUSPEND_SHIFT);
					FSREG_Write(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);
					VK_TASK_Sleep(100);

					ulVal=FSREG_Read(g_hReg, BCHP_USB_EHCI_USBCMD);
					ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
					FSREG_Write(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
					VK_TASK_Sleep(100);

					ulVal=FSREG_Read(g_hReg, BCHP_USB_EHCI_PORTSC_0);
					ulVal&=~BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_MASK;
					// pattern 4
					ulVal|=(4<<BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_SHIFT);
					FSREG_Write(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);

					iResult=CMD_OK;
				}
				else if (ulPortIndex==1)
				{
					ulVal=FSREG_Read(g_hReg, BCHP_USB1_EHCI_PORTSC_0);
					ulVal|=(1<<BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_SHIFT);
					FSREG_Write(g_hReg, BCHP_USB1_EHCI_PORTSC_0, ulVal);
					VK_TASK_Sleep(100);

					ulVal=FSREG_Read(g_hReg, BCHP_USB_EHCI_USBCMD);
					ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
					FSREG_Write(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
					VK_TASK_Sleep(100);

					ulVal=FSREG_Read(g_hReg, BCHP_USB1_EHCI_PORTSC_0);
					ulVal&=~BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_MASK;
					// pattern 4
					ulVal|=(4<<BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_SHIFT);
					FSREG_Write(g_hReg, BCHP_USB1_EHCI_PORTSC_0, ulVal);

					iResult=CMD_OK;
				}
#elif (BCHP_CHIP == 7405)
				if (ulPortIndex==0)
				{
					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_0);
					ulVal|=(1<<BCHP_USB_EHCI_PORTSC_0_SUSPEND_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_USBCMD);
					ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
					BREG_Write32(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_0);
					ulVal&=~BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_MASK;
					// pattern 4
					ulVal|=(4<<BCHP_USB_EHCI_PORTSC_0_PORT_TEST_CONTROL_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_0, ulVal);

					iResult=CMD_OK;
				}
				else if (ulPortIndex==1)
				{
					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_1);
					ulVal|=(1<<BCHP_USB_EHCI_PORTSC_1_SUSPEND_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_1, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_USBCMD);
					ulVal&=~BCHP_USB_EHCI_USBCMD_RUN_STOP_MASK;
					BREG_Write32(g_hReg, BCHP_USB_EHCI_USBCMD, ulVal);
					VK_TASK_Sleep(100);

					ulVal=BREG_Read32(g_hReg, BCHP_USB_EHCI_PORTSC_1);
					ulVal&=~BCHP_USB_EHCI_PORTSC_1_PORT_TEST_CONTROL_MASK;
					// pattern 4
					ulVal|=(4<<BCHP_USB_EHCI_PORTSC_1_PORT_TEST_CONTROL_SHIFT);
					BREG_Write32(g_hReg, BCHP_USB_EHCI_PORTSC_1, ulVal);

					iResult=CMD_OK;
				}
#else
				CMD_Printf("Command : usb, invalid chip.\n");
#endif
			}
		}
	}
	else
	{
		VK_printf("ERR! Bad Comman Input. \n");
	}

	return iResult;
}


