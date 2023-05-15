#include	<hlib.h>
#include	<hapi.h>
#include	<uapi.h>
#include	<dlib.h>
#include	<apk.h>

HERROR	test_noti1(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	printf("(%s) nArgc [%d]\n", __FUNCTION__, nArgc);
}

HERROR	test_noti2(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	printf("(%s) nArgc [%d]\n", __FUNCTION__, nArgc);
}

HERROR	test_noti3(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	printf("(%s) nArgc [%d]\n", __FUNCTION__, nArgc);
}

HERROR	onStateChanged(HINT32 nItemState, const HCHAR *pszName, void *pvUser)
{
	printf("<--- [%d] [%s]\n", nItemState, pszName);
}

HERROR	test(void *pvData)
{
	int	nValue;
	printf("[%d]\n", HLIB_STD_GetSystemTick());
	HAPI_GetRegistryInt("test1", &nValue);
	printf("nValue : [%d]\n", nValue);
}

void	cbOnData(void *pvDAta, HINT32 sock, HUINT32 ulRequestId, HINT32 size, void *pvData)
{
	HLIB_LOG_Dump(pvData, size, 0, 0);
}

#include <sys/statvfs.h>

typedef struct
{
	HCHAR	data[128];
}	test_t;

char	*astList[7] =
{
	"Test1",
	"test2",
	"test3",
	"test4",
	"test5",
	"test6",
	"tetst7",
	"test8"
};

int	test_func3(void)
{
	char	buf[1024];
	int		i;

	printf("---%p\n", &i);
}

int	test_func2(void)
{
	char	buf[512];
	int		i;

	printf("---%p\n", &i);
	test_func3();
}

int	test_func(void)
{
	int		i = 0;

	printf("%p\n", &i);
	test_func2();
}

int main(void)
{
	int		i = 0;

	printf("%p -- \n", &i);
	test_func();
	VK_Init();

	HAPI_BroadcastSignal("signal:onSubtitleState", TRUE, "i", 1);
	sleep(1000);
//	HAPI_BroadcastSignal("signal:onTeletextState", FALSE, NULL);
//extern HERROR	HAPI_BroadcastSignal(const HCHAR *pszSignalName, HBOOL bBuffered, const HCHAR *pszSignature, ...);
	return 0;
#if 0
	int		i, nValue;
	HxJSON_t	json, j1;
	HCHAR		szBuffer[512];
	HINT32		sock;
	struct statvfs	vfs;

	VK_Init();
	if (statvfs("/tmp", &vfs) != 0)
	{
		HxLOG_Critical("[%s](%d) invalid mount path : check your cache folder! (%s)\n", __FUNCTION__, __LINE__, "/tmp");
	}

	printf("[%d : %lld] [%d : %lld]\n", vfs.f_blocks, vfs.f_bsize, vfs.f_blocks, vfs.f_bfree);

	HLIB_DBG_Init(NULL);

//	UAPI_Copy("/mnt/hd2/Media/Video/Dead.Set.S01E02.WS.PDTV.XviD-RiVER.avi", "/media/ehd1/Dead.Set.S01E02.WS.PDTV.XviD-RiVER.avi", NULL, FALSE, NULL, NULL);
//	UAPI_Monitor(NULL, onStateChanged);
//	while (1)
//		VK_TASK_Sleep(1000);
#if	1
	for (i = 0; i < 256; i++)
		szBuffer[i] = i;
	sock = HLIB_SOCKET_OpenClientEx(HxSOCKET_INET, NULL, 60011, -1);
	HLIB_SOCKET_Write(sock, szBuffer, 256, 10);
	VK_TASK_Sleep(10000);
//	HLIB_SOCKET_Listen(sock, TRUE, cbOnData, NULL, NULL);
#else
	sock = HLIB_SOCKET_OpenServer(HxSOCKET_INET, 60011);
	HLIB_SOCKET_Listen(sock, TRUE, cbOnData, NULL, NULL);
#endif

#if	0

	HAPI_Init();
	APK_DB_Init();

	APK_DB_SetInt("testDB", 123213);
	APK_DB_SetStr("testStr", "TEstStr...");
	APK_DB_GetInt("testDB", &nValue);
	APK_DB_GetStr("testStr", szBuffer, 512);

	printf("nValue [%d] buff[%s]\n", nValue, szBuffer);

#if	0
	HAPI_SetRegistryInt("test1", 235);
	HAPI_SetRegistryInt("test2", 111);
	HAPI_SetRegistryStr("test3", "object ooo");
	HAPI_SetRegistryStr("test4", "object ooo");
#endif
	VK_TASK_Sleep(2000);
#endif
#endif
}

