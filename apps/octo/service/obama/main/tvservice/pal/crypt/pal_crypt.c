/**
	@file     pal_crypt.c
	@brief   crypt interface function file

	Description: libxml interface function file		\n
	Module: XML API			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include "des.h"
#include "pal_crypt.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#ifdef CONFIG_DEBUG
	HUINT32 g_PalCrypt_Level;
#endif


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/





/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#if 0
static HERROR		pal_crypt_Deskey(HUINT8 *pucKey, HUINT16 usMode);
static HERROR		pal_crypt_Des3key(HUINT8 *pucHexKey, HUINT16 usMode);
static HERROR		pal_crypt_Des(HUINT8 *pucInblock, HUINT8 *pucOutblock);





HERROR		PAL_CRYPT_Deskey(HUINT8 *pucKey, HUINT16 usMode)
{
	HERROR			hError = ERR_FAIL;

	if(pucKey == NULL)
	{
		HxLOG_Print("[PAL_CRYPT_Deskey] param null!!\n");
		goto EXIT_CRYPT_FUNC;
	}

	hError = pal_crypt_Deskey(pucKey, usMode);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[PAL_CRYPT_Deskey] Deskey fail!!\n");
		goto EXIT_CRYPT_FUNC;
	}

      	hError = ERR_OK;

EXIT_CRYPT_FUNC :

	return hError;
}



HERROR		PAL_CRYPT_Des3key(HUINT8 *pucHexKey, HUINT16 usMode)
{
	HERROR			hError = ERR_FAIL;

	if(pucHexKey == NULL)
	{
		HxLOG_Print("[PAL_CRYPT_Des3key] param null!!\n");
		goto EXIT_CRYPT_FUNC;
	}

	hError = pal_crypt_Des3key(pucHexKey, usMode);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[PAL_CRYPT_Des3key] Deskey fail!!\n");
		goto EXIT_CRYPT_FUNC;
	}

      	hError = ERR_OK;

EXIT_CRYPT_FUNC :

	return hError;
}




HERROR		PAL_CRYPT_Des(HUINT8 *pucInblock, HUINT8 *pucOutblock)
{
	HERROR			hError = ERR_FAIL;

	if(pucInblock == NULL ||pucOutblock == NULL )
	{
		HxLOG_Print("[PAL_CRYPT_Des] param null!!\n");
		goto EXIT_CRYPT_FUNC;
	}

	hError = pal_crypt_Des(pucInblock, pucOutblock);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[PAL_CRYPT_Des] Des fail!!\n");
		goto EXIT_CRYPT_FUNC;
	}

      	hError = ERR_OK;

EXIT_CRYPT_FUNC :

	return hError;
}


#define ______LOCAL_CRYPT_FUNC______
/**
  @brief     LAYER_MODULE_DoSomething ÇÔ¼ö.

  simple comment \n

  @return    return °ª.
  */
static HERROR		pal_crypt_Deskey(HUINT8 *pucKey, HUINT16 usMode)
{
	HERROR			hError = ERR_FAIL;

	if(pucKey == NULL)
	{
		HxLOG_Print("[pal_crypt_Deskey] param null!!\n");
		goto EXIT_CRYPT_FUNC;
	}

	deskey((unsigned char*)pucKey, (short)usMode);

      	hError = ERR_OK;

EXIT_CRYPT_FUNC :

	return hError;
}


static HERROR		pal_crypt_Des2key(HUINT8 *pucHexKey, HUINT16 usMode)
{
	HERROR			hError = ERR_FAIL;

	if(pucHexKey == NULL)
	{
		HxLOG_Print("[pal_crypt_Des2key] param null!!\n");
		goto EXIT_CRYPT_FUNC;
	}

	des2key((unsigned char*)pucHexKey, (short)usMode);

      	hError = ERR_OK;

EXIT_CRYPT_FUNC :

	return hError;
}




static HERROR		pal_crypt_Des3key(HUINT8 *pucHexKey, HUINT16 usMode)
{
	HERROR			hError = ERR_FAIL;

	if(pucHexKey == NULL)
	{
		HxLOG_Print("[pal_crypt_Des3key] param null!!\n");
		goto EXIT_CRYPT_FUNC;
	}

	des3key((unsigned char*)pucHexKey, (short)usMode);
      	hError = ERR_OK;

EXIT_CRYPT_FUNC :

	return hError;
}





static HERROR		pal_crypt_Des(HUINT8 *pucInblock, HUINT8 *pucOutblock)
{
	HERROR			hError = ERR_FAIL;

	if(pucInblock == NULL ||pucOutblock == NULL )
	{
		HxLOG_Print("[pal_crypt_Des] param null!!\n");
		goto EXIT_CRYPT_FUNC;
	}

	des((unsigned char*)pucInblock, (unsigned char*)pucOutblock);

      	hError = ERR_OK;

EXIT_CRYPT_FUNC :

	return hError;
}
#endif



static HERROR		pal_crypt_desinit(HUINT8 *pucKey);
static HERROR		pal_crypt_des3init(HUINT8 *pucKey);
static HERROR		pal_crypt_desdecryptprocess(HUINT8 *pucInblock, HUINT8 *pucOutblock);
static HERROR		pal_crypt_des3decryptprocess(HUINT8 *pucInblock, HUINT8 *pucOutblock);

static HERROR		pal_crypt_desencryptprocess(HUINT8 *pucInblock, HUINT8 *pucOutblock);
static HERROR		pal_crypt_des3encryptprocess(HUINT8 *pucInblock, HUINT8 *pucOutblock);


HERROR		PAL_CRYPT_Desinit(HUINT8 *pucKey)
{
	return pal_crypt_desinit(pucKey);
}

HERROR		PAL_CRYPT_Des3init(HUINT8 *pucKey)
{
	return pal_crypt_des3init(pucKey);
}

HERROR		PAL_CRYPT_DesDecryptProcess(HUINT8 *pucInblock, HUINT8 *pucOutblock)
{
	return pal_crypt_desdecryptprocess(pucInblock, pucOutblock);
}

HERROR		PAL_CRYPT_Des3DecryptProcess(HUINT8 *pucInblock, HUINT8 *pucOutblock)
{
	return pal_crypt_des3decryptprocess(pucInblock, pucOutblock);
}


HERROR		PAL_CRYPT_DesEncryptProcess(HUINT8 *pucInblock, HUINT8 *pucOutblock)
{
	return pal_crypt_desencryptprocess(pucInblock, pucOutblock);
}

HERROR		PAL_CRYPT_Des3EncryptProcess(HUINT8 *pucInblock, HUINT8 *pucOutblock)
{
	return pal_crypt_des3encryptprocess(pucInblock, pucOutblock);
}





static HERROR		pal_crypt_desinit(HUINT8 *pucKey)
{
	HERROR			hError = ERR_FAIL;

	if(pucKey == NULL)
	{
		HxLOG_Print("[pal_crypt_desinit] param null!!\n");
		goto EXIT_CRYPT_FUNC;
	}

	des_init((char*)pucKey);

      	hError = ERR_OK;

EXIT_CRYPT_FUNC :

	return hError;
}

static HERROR		pal_crypt_des3init(HUINT8 *pucKey)
{
	HERROR			hError = ERR_FAIL;

	if(pucKey == NULL)
	{
		HxLOG_Print("[pal_crypt_des3init] param null!!\n");
		goto EXIT_CRYPT_FUNC;
	}

	des3_init((char*)pucKey);

      	hError = ERR_OK;

EXIT_CRYPT_FUNC :

	return hError;
}


static HERROR		pal_crypt_desdecryptprocess(HUINT8 *pucInblock, HUINT8 *pucOutblock)
{
	HERROR			hError = ERR_FAIL;

	if(pucInblock == NULL ||pucOutblock == NULL )
	{
		HxLOG_Print("[pal_crypt_desdecryptprocess] param null!!\n");
		goto EXIT_CRYPT_FUNC;
	}

	decrypt_process((char*)pucInblock, (char*)pucOutblock);

      	hError = ERR_OK;

EXIT_CRYPT_FUNC :

	return hError;
}

static HERROR		pal_crypt_des3decryptprocess(HUINT8 *pucInblock, HUINT8 *pucOutblock)
{
	HERROR			hError = ERR_FAIL;

	if(pucInblock == NULL ||pucOutblock == NULL )
	{
		HxLOG_Print("[pal_crypt_des3decryptprocess] param null!!\n");
		goto EXIT_CRYPT_FUNC;
	}

	decrypt3_process((char*)pucInblock, (char*)pucOutblock);

      	hError = ERR_OK;

EXIT_CRYPT_FUNC :

	return hError;
}



static HERROR		pal_crypt_desencryptprocess(HUINT8 *pucInblock, HUINT8 *pucOutblock)
{
	HERROR			hError = ERR_FAIL;

	if(pucInblock == NULL ||pucOutblock == NULL )
	{
		HxLOG_Print("[pal_crypt_desencryptprocess] param null!!\n");
		goto EXIT_CRYPT_FUNC;
	}

	encrypt_process((char*)pucInblock, (char*)pucOutblock);

      	hError = ERR_OK;

EXIT_CRYPT_FUNC :

	return hError;
}

static HERROR		pal_crypt_des3encryptprocess(HUINT8 *pucInblock, HUINT8 *pucOutblock)
{
	HERROR			hError = ERR_FAIL;

	if(pucInblock == NULL ||pucOutblock == NULL )
	{
		HxLOG_Print("[pal_crypt_des3encryptprocess] param null!!\n");
		goto EXIT_CRYPT_FUNC;
	}

	encrypt3_process((char*)pucInblock, (char*)pucOutblock);

      	hError = ERR_OK;

EXIT_CRYPT_FUNC :

	return hError;
}



/*********************** End of File ******************************/
