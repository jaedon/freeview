#include <hlib.h>
#include <dapi.h>
#include <dlib.h>
#include <apk.h>
#include <unistd.h>


extern void OAPI_Tool(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	VK_Init();
	HLIB_DBG_Init();
	DLIB_Init();

//	OAPI_Tool(argc, argv);
	return 0;
}

