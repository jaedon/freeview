
#include <hlib.h>
#include "_dlib_port_dbsetup.h"

void			DLIB_DBSETUP_Init(void)
{
//	dlib_RegisterDECModule(DxNAMEOF(DxDBSetup_t), dlib_provider_Encoder, dlib_provider_Decoder);
}

HINT32			DLIB_DBSETUP_GetFieldCount (void)
{
	return 0;
}

DxField_t *		DLIB_DBSETUP_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	return NULL;
}

HERROR			DLIB_DBSETUP_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return ERR_FAIL;
}

HERROR			DLIB_DBSETUP_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return ERR_FAIL;
}

HERROR			DLIB_DBSETUP_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 데이터 전송 시, 효율을 높이기 위해 압축이 필요한 경우 이 함수 호출됨
	// 압축 방식은 진짜 압축이 아닌, 그냥 string을 binary로 바꾸는 등
	// 아무튼 데이터를 줄이기만 하면 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}

HERROR			DLIB_DBSETUP_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 압축된 데이터 원복
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}


HERROR			DLIB_DBSETUP_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	return ERR_FAIL;
}

void			DLIB_DBSETUP_FreeType (void *type)
{
}



