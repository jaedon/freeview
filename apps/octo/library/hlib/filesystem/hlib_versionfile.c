
/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */

#include <hlib.h>
#include <_hlib_versionfile.h>


/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */
#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"HxVERSIONFILE"

#define	LOG				HxLOG_Print
#define	ERR_LOG			HxLOG_Error



/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */





/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
struct _HxVersionFile
{
	/* version */
	HINT32	major;
	HINT32	minor;

	/* title */
	HxSTR_t	*title;
	HxSTR_t	*contents;
	HxSTR_t	*tail;
};



/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___STATIC_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */


static void _hxVersionfile_Read_Info(HxVERFILE_t *thiz, HxFILE_t fp, HCHAR *buf)
{
	HINT32	major = 0;
	HINT32	minor = 0;

	HCHAR *version_str= NULL;
	HCHAR *major_str = NULL;
	HCHAR *minor_str = NULL;
	HCHAR *close_str = NULL;

	version_str = HxSTD_StrStr(buf, "version");
	if(version_str == NULL) return;

	major_str = HxSTD_StrChar(version_str, '=');
	if(major_str == NULL) return;
	major_str++;

	minor_str = HxSTD_StrChar(major_str, '.');
	if(minor_str == NULL) return;
	*minor_str = 0;
	minor_str++;

	close_str = HxSTD_StrChar(minor_str, '/');
	if(close_str == NULL)	return;
	*close_str = 0;

 	major = HLIB_STD_StrToUINT32(major_str);
	minor = HLIB_STD_StrToUINT32(minor_str);

	thiz->minor = minor;
	thiz->major = major;
}

static void _hxVersionfile_Read_Title(HxVERFILE_t *thiz, HxFILE_t fp, HCHAR *buf)
{
	HCHAR *name_str = NULL;
	HCHAR *title_str = NULL;
	HCHAR *close_str = NULL;

	name_str = HxSTD_StrStr(buf, "name");
	HxLOG_Assert(name_str);

	title_str = HxSTD_StrChar(name_str, '=');
	if(title_str == NULL) return;

	title_str++;

	close_str = HxSTD_StrChar(title_str, '/');
	if(close_str == NULL)	return;
	*close_str = 0;

	thiz->title = HLIB_STR_New((const HUINT8*)title_str);
}


static void _hxVersionfile_Read_Conetents(HxVERFILE_t *thiz, HxFILE_t fp, HCHAR *buf)
{
	HCHAR 	*tmp = NULL;
	HUINT8 	*ret = NULL;
	HxSTR_t	*pStr = NULL;

	tmp = HLIB_MEM_Malloc(5120);
	HxLOG_Assert(tmp);

	while(HLIB_FILE_EOF(fp)==0)
	{
		ret = HLIB_FILE_GetString(fp, (const HUINT8*)tmp, 5120);
		if(ret == NULL)
		{
			break;
		}

		if(HLIB_STD_StrNCaseCmp(tmp, "</contents>", HxSTD_StrLen("</contents>")) == 0)
		{
			break;
		}

		if(pStr == NULL)
		{
			pStr = HLIB_STR_New((const HUINT8*)tmp);
		}
		else
		{
			pStr = HLIB_STR_Append(pStr, (const HUINT8*)tmp);
		}
	}

	thiz->contents = pStr;

	if(tmp)
	{
		HLIB_MEM_Free(tmp);
		tmp = NULL;
	}
}



/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */




/**
*
*
*
* @param
* @return
*/
HxVERFILE_t *HLIB_VERFILE_New(void)
{
	HxVERFILE_t *propFile = NULL;

	propFile = (HxVERFILE_t *)HLIB_MEM_Malloc(sizeof(HxVERFILE_t));
	HxSTD_MemSet(propFile, 0, sizeof(HxVERFILE_t));


	return propFile;
}


/**
*
*
*
* @param
* @return
*/
HxVERFILE_t *HLIB_VERFILE_NewFromFile(HxFILE_t fp)
{
	HxVERFILE_t *propFile = NULL;

	propFile = (HxVERFILE_t *)HLIB_MEM_Malloc(sizeof(HxVERFILE_t));
	HxSTD_MemSet(propFile, 0, sizeof(HxVERFILE_t));

	HLIB_VERFILE_Read(propFile, fp);

	return propFile;
}


/**
*
*
*
* @param
* @return
*/
void HLIB_VERFILE_Delete(HxVERFILE_t *thiz)
{
	if(thiz)
	{
		if(thiz->title)		HLIB_STR_Delete(thiz->title);
		if(thiz->contents)	HLIB_STR_Delete(thiz->contents);
		if(thiz->tail)		HLIB_STR_Delete(thiz->tail);

		HLIB_MEM_Free(thiz);
	}
}


/**
*
*
*
* @param
* @return
*/
HERROR	HLIB_VERFILE_SetVersion(HxVERFILE_t *thiz, HINT32 major, HINT32 minor)
{
	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	if(thiz == NULL)
	{
		HxLOG_Print("[%s:%d] error : do not init thiz (-)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	thiz->major = major;
	thiz->minor	= minor;

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}



/**
*
*
*
* @param
* @return
*/
HERROR	HLIB_VERFILE_GetVersion(HxVERFILE_t *thiz, HINT32 *major, HINT32 *minor)
{
	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	if(thiz == NULL)
	{
		HxLOG_Print("[%s:%d] error : do not init thiz (-)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	if(major)
	{
		*major = thiz->major;
	}

	if(minor)
	{
		*minor = thiz->minor;
	}

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}




/**
*
*
*
* @param
* @return
*/
HERROR HLIB_VERFILE_SetTitle(HxVERFILE_t *thiz, HCHAR *title)
{
	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	if(thiz == NULL)
	{
		HxLOG_Print("[%s:%d] error : do not init thiz (-)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxLOG_Assert(title);

	thiz->title = HLIB_STR_New((const HUINT8*)title);

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}



/**
*
*
*
* @param
* @return
*/
HERROR HLIB_VERFILE_GetTitle(HxVERFILE_t *thiz, HCHAR **title)
{
	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	if(thiz == NULL)
	{
		HxLOG_Print("[%s:%d] error : do not init thiz (-)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxLOG_Assert(title);

	*title = (HCHAR*)HLIB_STR_GetBuffer(thiz->title);

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}


/**
*
*
*
* @param
* @return
*/
HERROR HLIB_VERFILE_GetTitleToHxSTR(HxVERFILE_t *thiz, HxSTR_t **title)
{
	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	if(thiz == NULL)
	{
		HxLOG_Print("[%s:%d] error : do not init thiz (-)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxLOG_Assert(title);

	*title = thiz->title;

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}



/**
*
*
*
* @param
* @return
*/
HERROR HLIB_VERFILE_SetContents(HxVERFILE_t *thiz, HCHAR *contents)
{
	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	if(thiz == NULL)
	{
		HxLOG_Print("[%s:%d] error : do not init thiz (-)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxLOG_Assert(contents);

	thiz->contents = HLIB_STR_New((const HUINT8*)contents);

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}



/**
*
*
*
* @param
* @return
*/
HERROR HLIB_VERFILE_GetContents(HxVERFILE_t *thiz, HCHAR **contents)
{
	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	if(thiz == NULL)
	{
		HxLOG_Print("[%s:%d] error : do not init thiz (-)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxLOG_Assert(contents);

	*contents = (HCHAR*)HLIB_STR_GetBuffer(thiz->contents);

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}


/**
*
* 주의 : 할당해제하면 안된다!!
*
* @param
* @return
*/
HERROR HLIB_VERFILE_GetContentsToHxSTR(HxVERFILE_t *thiz, HxSTR_t **contents)
{
	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	if(thiz == NULL)
	{
		HxLOG_Print("[%s:%d] error : do not init thiz (-)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	HxLOG_Assert(contents);

	*contents = thiz->contents;

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}




/**
*
*
*
* @param
* @return
*/
HERROR HLIB_VERFILE_SetTail(HxVERFILE_t *thiz, HCHAR *tail)
{
	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	if(thiz == NULL)
	{
		HxLOG_Print("[%s:%d] error : do not init thiz (-)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	// TODO:: 사용하려면 read와 write 수정해야 함.

	HxLOG_Assert(tail);

	thiz->tail = HLIB_STR_New((const HUINT8*)tail);

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}



/**
*
*
*
* @param
* @return
*/
HERROR HLIB_VERFILE_GetTail(HxVERFILE_t *thiz, HCHAR **tail)
{
	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	if(thiz == NULL)
	{
		HxLOG_Print("[%s:%d] error : do not init thiz (-)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	// TODO:: 사용하려면 read와 write 수정해야 함.

	HxLOG_Assert(tail);

	*tail = (HCHAR*)HLIB_STR_GetBuffer(thiz->tail);

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}


/**
*
*
*
* @param
* @return
*/
HERROR HLIB_VERFILE_GetTailToHxSTR(HxVERFILE_t *thiz, HxSTR_t **tail)
{
	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	if(thiz == NULL)
	{
		HxLOG_Print("[%s:%d] error : do not init thiz (-)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	// TODO:: 사용하려면 read와 write 수정해야 함.


	HxLOG_Assert(tail);

	*tail = thiz->tail;

	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}



/**
*
*
*
* @param
* @return
*/
HERROR HLIB_VERFILE_Write(HxVERFILE_t *thiz, HxFILE_t fp)
{
	HxLOG_Print("[%s:%d] (+)\n", __FUNCTION__, __LINE__);
	if(thiz == NULL)
	{
		HxLOG_Print("[%s:%d] error : do not init thiz (-)\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}


	HLIB_FILE_Printf(fp, "<!-- This file should be generated by Humax -->\r\n");
	HLIB_FILE_Printf(fp, "<info product=\"HUMAX Co.\" version=%d.%d/>\r\n" , thiz->major, thiz->minor);
	HLIB_FILE_Printf(fp, "<title name=\"%s\"/>\r\n",HLIB_STR_toUTF8(thiz->title));
	HLIB_FILE_Printf(fp, "<contents>\r\n");
	HLIB_FILE_Printf(fp, (const char*)HLIB_STR_toUTF8(thiz->contents));
	HLIB_FILE_Printf(fp, "</contents>\r\n");


	HxLOG_Print("[%s:%d] (-)\n", __FUNCTION__, __LINE__);

	return ERR_OK;
}


/**
*
*
*
* @param
* @return
*/
HERROR HLIB_VERFILE_Read(HxVERFILE_t *thiz, HxFILE_t fp)
{
	HCHAR	*buf = NULL;
	HUINT8	*ret = NULL;

	if(thiz == NULL)
	{
		return ERR_FAIL;
	}

	buf = HLIB_MEM_Malloc(512);
	HxLOG_Assert(buf);

	while(HLIB_FILE_EOF(fp)==0)
	{
		ret = HLIB_FILE_GetString(fp, (const HUINT8*)buf, 512);
		if(ret == NULL)
		{
			break;
		}

		if(HLIB_STD_StrNCaseCmp(buf, "<info", HxSTD_StrLen("<info")) == 0)
		{
			_hxVersionfile_Read_Info(thiz, fp, buf);
			continue;
		}

		if(HLIB_STD_StrNCaseCmp(buf, "<title", HxSTD_StrLen("<title")) == 0)
		{
			_hxVersionfile_Read_Title(thiz, fp, buf);
			continue;
		}

		if(HLIB_STD_StrNCaseCmp(buf, "<contents>", HxSTD_StrLen("<contents>")) == 0)
		{
			_hxVersionfile_Read_Conetents(thiz, fp, buf);
			continue;
		}
	}

	if(buf)
	{
		HLIB_MEM_Free(buf);
		buf = NULL;
	}

	return ERR_OK;
}



#if defined(CONFIG_DEBUG)
void	HLIB_VERFILE_Dump(HxVERFILE_t *thiz)
{
	if(thiz == NULL)
	{
		return;
	}

	LOG("============================================================\r\n");
	LOG("thiz(0x%x)\n", (HUINT32)thiz);
	LOG("  + version : %d.%d\r\n", thiz->major, thiz->minor);
	if(thiz->title)
		LOG("  + title   : %s\r\n", HLIB_STR_toUTF8(thiz->title));
	if(thiz->contents)
		LOG("  + contents: %s\r\n", HLIB_STR_toUTF8(thiz->contents));
	if(thiz->tail)
		LOG("  + tail    : %s\r\n", HLIB_STR_toUTF8(thiz->tail));
	LOG("============================================================\r\n");
}

void 	HLIB_VERFILE_Test(void *pszArgs)
{
	HxVERFILE_t *versionFile = NULL;
	HxFILE_t fp = NULL;

	/*************************************************/
	/******************** Write **********************/
	/*************************************************/

	versionFile = HLIB_VERFILE_New();
	HxLOG_Assert(versionFile);

	HLIB_VERFILE_SetVersion(versionFile, 10, 20);
	HLIB_VERFILE_SetTitle(versionFile, "eVideoCodec, nVideoBitrateKbps, fFrameRate, nScreenWidth, nScreenHeight, eVideoAspectRatio");
	HLIB_VERFILE_SetContents(versionFile,"profile_480\r\neDxVIDEO_CODEC_H264, 64, 320, 25.0, 480, 320, eDxASPECTRATIO_4X3\r\neDxVIDEO_CODEC_H264, 64, 96, 10.0, 240, 160, eDxASPECTRATIO_4X3\r\neDxVIDEO_CODEC_H264, 64, 192, 15.0, 480, 320, eDxASPECTRATIO_4X3\r\neDxVIDEO_CODEC_H264, 64, 384, 29.97, 480, 320, eDxASPECTRATIO_4X3\r\neDxVIDEO_CODEC_H264, 64, 768, 29.97, 480, 320, eDxASPECTRATIO_4X3\r\neDxVIDEO_CODEC_H264, 64, 1152, 29.97, 480, 320, eDxASPECTRATIO_4X3\r\neDxVIDEO_CODEC_H264, 64, 1536, 29.97, 480, 320, eDxASPECTRATIO_4X3\r\neDxVIDEO_CODEC_H264, 64, 1920, 29.97, 480, 320, eDxASPECTRATIO_4X3\r\neDxVIDEO_CODEC_H264, 64, 2688, 29.97, 480, 320, eDxASPECTRATIO_4X3\r\neDxVIDEO_CODEC_H264, 64, 2688, 29.97, 480, 320, eDxASPECTRATIO_4X3\r\n\r\niphone3g:profile_480\r\n");

	fp = HLIB_FILE_Open((const HUINT8*)"test.hxm", (const HUINT8*)"wb");
	HLIB_VERFILE_Write(versionFile, fp);
	HLIB_FILE_Close(fp);

	HLIB_VERFILE_Dump(versionFile);

	HLIB_VERFILE_Delete(versionFile);
	versionFile = NULL;



	/*************************************************/
	/******************** Read **********************/
	/*************************************************/

	fp = HLIB_FILE_Open((const HUINT8*)"test.hxm", (const HUINT8*)"rb");
	versionFile = HLIB_VERFILE_NewFromFile(fp);
	HLIB_FILE_Close(fp);

	HxLOG_Assert(versionFile);

	HLIB_VERFILE_Dump(versionFile);

	HLIB_VERFILE_Delete(versionFile);
	versionFile = NULL;

}
#endif



