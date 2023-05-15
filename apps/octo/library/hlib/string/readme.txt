/*
 * 설명
 */












/*
 * 사용 방법
 */
#include <hlib.h>

void HxSTR_TestCode()
{
	HxSTR_t *pStr = NULL;

	printf("\n\n\n");

	pStr = HLIB_STR_New("Test String Tool");
	HxLOG_Assert(pStr);
	printf("[%s:%d] %s\n", __FUNCTION__, __LINE__, HLIB_STR_toUTF8(pStr));
	HLIB_STR_trimLeft(pStr, 5);
	printf("[%s:%d] %s\n", __FUNCTION__, __LINE__, HLIB_STR_toUTF8(pStr));
	pStr = HLIB_STR_Append(pStr, " Test OK");
	printf("[%s:%d] %s\n", __FUNCTION__, __LINE__, HLIB_STR_toUTF8(pStr));
	HLIB_STR_Delete(pStr);
	pStr = NULL;

	pStr = HLIB_STR_NewFormat("Test Format %s-%d", "string", 10);
	HxLOG_Assert(pStr);
	printf("[%s:%d] %s\n", __FUNCTION__, __LINE__, HLIB_STR_toUTF8(pStr));
	HLIB_STR_Delete(pStr);
	pStr = NULL;

	pStr = HLIB_STR_New("Test UTF8 to UCS2");
	HxLOG_Assert(pStr);
	HLIB_STR_Dump(pStr);
	HLIB_STR_Delete(pStr);
	pStr = NULL;


	pStr = HLIB_STR_New("");
	HxLOG_Assert(pStr);
	HLIB_STR_Dump(pStr);
	HLIB_STR_Delete(pStr);
	pStr = NULL;
}

int main(HINT32 argc, HCHAR **argv)
{
	VK_Init();
	HLIB_CP_Init();

	HxSTR_TestCode();


	return 0;
}



