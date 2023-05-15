/**************************************************************
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <sys/time.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>

#include "dlib.h"
#include "apk_meta_antenna.h"
#include "nova_errcode.h"
#include "nova_utils.h"



/* list data : DxAntInfo_t * */
HERROR	NOVA_GetAntenaList(HxList_t **list)
{
	HINT32 		nAntCount = 0;
	HINT32		i = 0;
	HxList_t	*pAntList = NULL;
	DxAntInfo_t *pAntInfo = NULL;

	APK_META_ANTENNA_Load();

	nAntCount = APK_META_ANTENNA_Count();

	for(i=0; i < nAntCount; i++)
	{
		pAntInfo = APK_META_ANTENNA_GetInfo(APK_META_ANTENNA_GetUidAt(i));
		if(NULL != pAntInfo)
		{
			pAntList = HLIB_LIST_Append(pAntList,pAntInfo);
		}
	}

	*list = pAntList;

	return ERR_OK;
}

static void _nova_antenna_InfoFree (DxAntInfo_t *p)
{
	if (p != NULL)
	{
		HLIB_STD_MemFree(p);
	}
}

void	NOVA_ReleseAntenaList(HxList_t *list)
{
	HLIB_LIST_RemoveAllFunc(list, (void *)_nova_antenna_InfoFree);
}

double NOVA_getTime(void)
{
	struct timeval tvNow;
	double now;

	gettimeofday(&tvNow, NULL);
	now = tvNow.tv_sec + tvNow.tv_usec/1000000.0;

	return now;
}

int NOVA_SendFile(const char* source, const char* destination, mode_t permission)
{
	int			input = -1, output = -1;
	off_t		bytesCopied = 0;
	struct stat fileinfo = {0};
	int			result = -1;
	int			ret;

	ret = stat(source, &fileinfo);
	if (ret != 0)
	{
		HxLOG_Error("stat(%s) failed(errno:%d)\n", source, errno);
		goto EXIT;
	}

	if (S_ISDIR (fileinfo.st_mode) == true )
	{
		HxLOG_Error("## %s is directory. skip copying the file\n", source);
		goto EXIT;
	}

	if ((input = open(source, O_RDONLY)) == -1)
	{
		HxLOG_Error("Cannot open src file(%s) (errno:%d)\n", source ,  errno);
		goto EXIT;
	}
	if ((output = open(destination, O_RDWR | O_CREAT, permission)) == -1)
	{
		HxLOG_Error("Cannot open dst file(%s) (errno:%d)\n", destination,  errno);
		goto EXIT;
	}

	result = sendfile(output, input, &bytesCopied, fileinfo.st_size);

EXIT:
	if (input != -1)	close(input);
	if (output != -1)	close(output);

	return result;
}
