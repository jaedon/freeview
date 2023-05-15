#include <hapi.h>
#include <directfb.h>

void	main(int argc, char* arg[])
{
	int			nSleep, nCount = 1;
	HUINT32		ulStart, ulDiff;

	VK_Init();
	HLIB_DBG_Init();
	ulStart = HLIB_STD_GetSystemTick();
	
	if(argc < 2)
	{
		nSleep = 5;		
	}
	else
	{
		nSleep = atoi(arg[1]);		
	}
	
	printf("\n#############################################\n");
	printf("## START Zapping Channel....! peried[%d s] ##\n", nSleep);
	printf("#############################################\n\n");

	while (1)
	{
		ulDiff = (HLIB_STD_GetSystemTick() - ulStart) / 1000;
		printf("\n############################################################\n");
		printf("[channelzapping(%d)] nCount = %d, Elapsed(%02d:%02d:%02d)\n", nSleep, nCount++, ulDiff / 3600, (ulDiff % 3600) / 60,  (ulDiff % 3600) % 60);
		printf("############################################################\n\n");
		HAPI_SendRemoteRCUKey(DIKS_CHANNEL_UP);
		sleep(nSleep);
	}
}
