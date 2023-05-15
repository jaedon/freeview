#include <direct/messages.h>
#include <directfb.h>

#include "dfb_platform.h"

#include "htype.h"
#include "di_err.h"
#include "di_uart.h"
#include "di_init.h"
#if defined(CONFIG_DI_SERVER_CLIENT)
#include "vkernel.h"
#include "di_power_server_client.h"
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/

#ifndef DEBUG_MODULE
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#endif

int main(int argc, char *argv[])
{
	DI_ERR_CODE eDiErr = DI_ERR_OK;
	DFBResult  eDfbError = DFB_OK;
	IDirectFB *pDfbMaster;
	sigset_t   sigBlock;
#if defined(CONFIG_JAPAN_SPEC)
	DFBScreenEncoderConfig encoderCfg;
	IDirectFBScreen *primary_screen;
	IDirectFBScreen *secondary_screen;
#endif

	eDiErr=DI_PLATFORM_Init();
	if (eDiErr != DI_ERR_OK)
	{
		PrintError("%s->DI_PLATFORM_Init: Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDiErr, __LINE__, __FILE__);
		return DI_ERR_ERROR;
	}
	eDfbError = DirectFBInit( &argc, &argv );
	if (eDfbError) {

		PrintError("%s->DirectFBInit: Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDfbError, __LINE__, __FILE__);
		return DI_ERR_ERROR;
	}


	eDfbError = DirectFBCreate( &pDfbMaster );
	if (eDfbError) {
		PrintError("%s->DirectFBCreate(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, eDfbError, __LINE__, __FILE__);
		return DI_ERR_ERROR;
	}

	{
		int fd;
		fd=open("/tmp/.dfbmaster.init", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		if(fd<0)
		{
			PrintError("open : /tmp/.dfbmaster.init error , Line(%d), %s\n",__LINE__, __FILE__);
		}
		else
		{
			close(fd);
		}
	}

#if defined(CONFIG_DI_SERVER_CLIENT)
	(void)VK_Init();
	(void)DI_POWER_SERVER_Init();
#endif

	sigemptyset( &sigBlock );
	sigsuspend( &sigBlock );

	pDfbMaster->Release( pDfbMaster );

	return DI_ERR_OK;
}

