/**************************************************************************************************/
#define ________Secure_Handler_Private_Include______________________________________________
/**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <vkernel.h>

#include "secma_init.h"


#ifdef DI_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				printf("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				printf("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)    		while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)			VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)
#endif


/**************************************************************************************************/
#define ________Secure_Handler_Golbal_Value________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Secure_Handler_Private_Define_______________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Secure_Handler_Private_Type_Define__________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Secure_Handler_Private_Static_Value__________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Secure_Handler_Private_Static_Prototype______________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Secure_Handler_Private_Static_Prototype_Body_________________________________
/**************************************************************************************************/
HINT32 SECMA_HANDLE_DiInit(void)
{
	int rc;

	rc = DRV_HOTPLUG_Init();
	if (rc != 0)
	{
		PrintError("%s->DRV_HDD_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, rc, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_HDD_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	rc = DRV_HDD_Init();
	if (rc != 0)
	{
		PrintError("%s->DRV_HDD_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, rc, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_HDD_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	rc = DRV_FAN_Init();
	if (rc != 0)
	{
		PrintError("%s->DRV_FAN_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, rc, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_FAN_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	rc = DRV_USB_Init();
	if (rc != 0)
	{
		PrintError("%s->DRV_USB_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, rc, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_USB_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	rc = DRV_USB_HID_Init();
	if (rc != 0)
	{
		PrintError("%s->DRV_USB_HID_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, rc, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_USB_HID_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	rc = DRV_USB_WIRELESS_Init();
	if (rc != 0)
	{
		PrintError("%s->DRV_USB_WIRELESS_Init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, rc, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_USB_WIRELESS_Init(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	rc = DRV_HOTPLUG_ReadSysUsb();
	if (rc != 0)
	{
		PrintError("%s->DRV_HOTPLUG_ReadSysUsb(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, rc, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DRV_HOTPLUG_ReadSysUsb(): OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	rc = DRV_FS_ENCR_Init();
	if (rc != 0)
	{
		PrintError("%s->DI_FS_ENCR_Init() : Error Code(%d), Line(%d), %s\n", __FUNCTION__, rc, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DI_FS_ENCR_Init() : OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	rc = DRV_MMC_Init();
	if (rc != 0)
	{
		PrintError("%s->DI_MMC_Init() : Error Code(%d), Line(%d), %s\n", __FUNCTION__, rc, __LINE__, __FILE__);
	}
	else
	{
		PrintDebug("%s->DI_MMC_Init() : OK! Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
	}

	return 0;
}


