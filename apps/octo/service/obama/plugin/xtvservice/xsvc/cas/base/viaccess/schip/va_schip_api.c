/*
	Viaccess Secured Chipset API
		Secured Chipset Driver API Document Supplement ACS 3.0 - Doc. Ref. 2316, p.3
*/

/********************************************************************
*	Including Headers
********************************************************************/
#include "va_def.h"
#include "va_schip.h"
//#include "ddi_err.h"
//#include "ddi_dsc.h"

#ifdef CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW
#include "va_setup.h"
#endif

//#define VA_SCHIP_TEST_MODE // test only!
#ifdef VA_SCHIP_TEST_MODE
#include "va_util.h"
#endif

#if defined(CONFIG_DEBUG) && !defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
#include "va_main.h"
#include "va_util.h"
#endif

//#include <_svc_cas_mgr_common.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/





#define VA_SCHIP_SESSION_KEY_LENGTH			16

/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/
static tVA_SCHIP_CipheringMode st_nCwMode;
static HUINT8 st_aucSessionKey[VA_SCHIP_SESSION_KEY_LENGTH];

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/

/*
	initialize variables and start task
*/
void VA_SCHIP_Init()
{
	HxLOG_Print("[Enter]\n");

	st_nCwMode = eINACTIVE;
	// È¤½Ã locked ¸ðµåÀÏ¼öµµ ÀÖÀ¸´Ï±î È®ÀÎ..
	VA_SCHIP_GetChipsetMode(&st_nCwMode);

#ifdef CONFIG_DEBUG

	HxLOG_Print("[01;34m \n==================== [00m");

#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST
	HxLOG_Print("[01;34m \n schipid 0x%08X [00m", VA_SCHIP_GetChipsetId());
#else
	HxLOG_Print("[01;34m \n Model Name : %s [00m", CONFIG_PRODUCT_NAME);
	HxLOG_Print("[01;34m \n Project ID : %d [00m", VA_MODEL_IDENTIFIER);
	HxLOG_Print("[01;34m \n S/N        : [00m");
	{
		HUINT8 i, aucStbSerial[HUMAX_SERIAL_NUMBER_SIZE]; // = {0x31,0x31,0x54,0x31,0x31,0x31,0x31,0x31,0x31,0x30,0x30,0x31,0x30,0x30};
		VA_memset(aucStbSerial, 0x00, HUMAX_SERIAL_NUMBER_SIZE);
		if (svc_cas_PalPortSysGetSystemSerialNumber(aucStbSerial, HUMAX_SERIAL_NUMBER_SIZE) == ERR_OK && aucStbSerial[0] != 0xff)
		{
			for (i = 0; i < HUMAX_SERIAL_NUMBER_SIZE; i++)
				HxLOG_Print("[01;34m%c[00m", aucStbSerial[i]);
		}
		else
		{
			HxLOG_Print("[01;34mN/C[00m");
		}
	}
	HxLOG_Print("[01;34m \n Chipset ID : 0x%08X (%d) [00m", VA_SCHIP_GetChipsetId(), VA_SCHIP_GetChipsetId());
#endif

	if (st_nCwMode == eLOCKED)
		HxLOG_Print("[01;34m \n Secured chipset has been set in locked mode [00m");

	HxLOG_Print("[01;34m \n====================\n\n [00m");

#endif

	memset(st_aucSessionKey, 0, VA_SCHIP_SESSION_KEY_LENGTH);

	HxLOG_Print("[Exit]\n");
}

int VA_SCHIP_GetCwMode()
{
#if !defined(CONFIG_PROD_CPU_BCM) // BXR-HD´Â descrambler handle¸¶´Ù session key¸¦ ³»·Á¾ß ÇÏ¹Ç·Î, º°µµ·Î Ã³¸®ÇÑ´Ù (BRCM)
	if (st_nCwMode != eINACTIVE)
	{
		// OCTO¿¡¼­´Â Encrypted modeÀÏ °æ¿ì Set key ÇÏ±â Àü¿¡ set protection key¸¦ ³»·ÁÁÖ¾î¾ß ÇÑ´Ù...
		svc_cas_DevDscSetProtectionKey(0, st_aucSessionKey);
	}
#endif

	return st_nCwMode;
}

#if defined(CONFIG_PROD_CPU_BCM)
#if defined(CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW)
int VA_SCHIP_SetProtectKey(HUINT32 ulDscIndex)
{
	// OCTO¿¡¼­´Â Encrypted modeÀÏ °æ¿ì Set key ÇÏ±â Àü¿¡ set protection key¸¦ ³»·ÁÁÖ¾î¾ß ÇÑ´Ù...
	// ST Chip°ú´Â ´Þ¸® BRCM¿¡¼­´Â DI ±¸Á¶»ó °¢ Descrambler index¸¶´Ù session key¸¦ ³»·Á ÁÖ¾î¾ß ÇÑ´Ù (ST´Â ÇÑ¹ø¸¸ ³»¸²)
	if (st_nCwMode != eINACTIVE)
	{
		HxLOG_Print("ulDscIndex(%d) st_nCwMode(%d) set protection key now \n", ulDscIndex, st_nCwMode);
		return svc_cas_DevDscSetProtectionKey(ulDscIndex, st_aucSessionKey);
	}

	return 0;
}
#endif
#endif


/*--------------------------------------------------------------------------------------
				VIACCESS ACS 3.0 API
--------------------------------------------------------------------------------------*/
/*
	VA_SCHIP_GetChipsetMode
		return the ciphering activation mode of the chipset
	arguments
		pChipsetMode
	return
		kVA_OK
		kVA_NOT_IMPLEMENTED
*/
INT VA_SCHIP_GetChipsetMode(tVA_SCHIP_CipheringMode *pChipsetMode)
{
	HBOOL bEncryptedMode;
	HUINT8 ucSCS, ucJTAG, ucOTP, ucCWE;

	HxLOG_Print("[Enter]\n");

#ifdef CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW

	// get chipset mode
	#ifdef VA_SCHIP_TEST_MODE // test only!
	HxLOG_Print("[01;33m GetChipsetMode : test mode !!\n [00m");
	ucCWE = VA_GetCWE();
	#else
	if (svc_cas_PalPortSysGetFusingInfo(&ucSCS, &ucJTAG, &ucOTP, &ucCWE) != ERR_OK)
	{
		HxLOG_Error("fail to get fuse statue\n");
		return kVA_NOT_IMPLEMENTED;
	}
	#endif

	if (ucCWE)
	{
		st_nCwMode = eLOCKED;
	}

	*pChipsetMode = st_nCwMode;

	HxLOG_Info("[Exit(%d)]\n", *pChipsetMode);
	return kVA_OK;

#else

	HxLOG_Error("secured chipset is not present\n");
	return kVA_NOT_IMPLEMENTED;

#endif
}

/*
	VA_SCHIP_SetChipsetMode
		set the ciphering activation mode
	arguments
		eChipsetMode
	return
		kVA_OK
		kVA_NOT_IMPLEMENTED
		kVA_INVALID_PARAMETER
*/
INT VA_SCHIP_SetChipsetMode(tVA_SCHIP_CipheringMode eChipsetMode)
{
	HUINT8 ucSCS, ucJTAG, ucOTP, ucCWE;

	HxLOG_Info("[Enter(%d)]\n", eChipsetMode);

#ifdef CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW

	// get chipset mode
	#ifdef VA_SCHIP_TEST_MODE // test only!
	HxLOG_Print("[01;33m SetChipsetMode : test mode !!\n [00m");
	ucCWE = VA_GetCWE();
	#else
	{
		Sys_FusingInfo_t	stFusingInfo;

		HxSTD_MemSet(&stFusingInfo, 0, sizeof(Sys_FusingInfo_t));

		if (PAL_FUSE_GetInfo(&stFusingInfo) != ERR_OK)
		{
			HxLOG_Error("fail to get fuse statue\n");
			return kVA_NOT_IMPLEMENTED;
		}

		ucSCS = (HUINT8)stFusingInfo.ucSCS;
		ucJTAG = (HUINT8)stFusingInfo.ucJTAG;
		ucOTP = (HUINT8)stFusingInfo.ucOTP;
		ucCWE = (HUINT8)stFusingInfo.ucCWE;
	}
	#endif

	// set chipset mode
	switch (eChipsetMode)
	{
	case eINACTIVE:
		if (ucCWE)
		{
			HxLOG_Error("not allowed transition(lock->inactive)\n");
			return kVA_NOT_IMPLEMENTED;
		}

		st_nCwMode = eINACTIVE;
		break;

	case eSESSION:
		if (ucCWE)
		{
			HxLOG_Error("not allowed transition(lock->session)\n");
			return kVA_NOT_IMPLEMENTED;
		}

		st_nCwMode = eSESSION;
		break;

	case eLOCKED:
		HxLOG_Print("[01;34m Secured chipset is set in locked mode\n [00m");

		#ifdef VA_SCHIP_TEST_MODE // test only!
		VA_SetCWE();
		#else
//		PAL_SYS_SetFusingInfo(TRUE);
		PAL_FUSE_SetSecureCWInfo(TRUE);
		#endif

		st_nCwMode = eLOCKED;
		break;

	default:
		HxLOG_Error("INVALID PARAMETER (%d)\n", eChipsetMode);
		return kVA_INVALID_PARAMETER;
	}

	HxLOG_Print("[Exit]\n");
	return kVA_OK;

#else

	HxLOG_Error("secured chipset is not present\n");
	return kVA_NOT_IMPLEMENTED;

#endif
}

/*
	VA_SCHIP_SetSessionKey
		provide the chipset with a session key that will be used later to decipher the received CW
	arguments
		uiSessionKeyLength
		pSessionKey
	return
		kVA_OK
		kVA_NOT_IMPLEMENTED
		kVA_INVALID_PARAMETER
*/
INT VA_SCHIP_SetSessionKey(UINT32 uiSessionKeyLength, const BYTE *pSessionKey)
{
	HxLOG_Info("[Enter(len:%d)]\n", uiSessionKeyLength);

#ifdef CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW

// (!!!) uiSessionKeyLength == 16 ÀÌ¾î¾ß ÇÑ´Ù.. VA¹®¼­¿¡µµ ±×·¸°í, PAL¿¡¼­µµ 16byte¸¸ di·Î ³»·ÁÁÖ´Ï±î..

	// if key is NULL or key length is zero
	// if key length exceeds kVA_SETUP_CHIPSET_CIPHERING_KEY_MAX_SIZE or --> this definition is removed (ACS 3.0.1.10)
	// if key length is not compatible with the secured chipset cryptographic protocol
	if (pSessionKey == NULL || uiSessionKeyLength != VA_SCHIP_SESSION_KEY_LENGTH)
	{
		#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // VA driver test Áß abnoral test case ½Ã¿¡ ÀÇµµµÈ ¿¡·¯ »óÈ²¿¡ ºÒÇÊ¿äÇÏ°Ô ·Î±× ÂïÁö ¾Êµµ·Ï...
		HxLOG_Info("INVALID PARAMETER (%x, %d)\n", pSessionKey, uiSessionKeyLength);
		#else
		HxLOG_Error("INVALID PARAMETER (%x, %d)\n", pSessionKey, uiSessionKeyLength);
		#endif
		return kVA_INVALID_PARAMETER;
	}

	// set session key
	VA_memcpy(st_aucSessionKey, pSessionKey, VA_SCHIP_SESSION_KEY_LENGTH);

	HxLOG_Print("[Exit]\n");
	return kVA_OK;

#else

	HxLOG_Error("secured chipset is not present\n");
	return kVA_NOT_IMPLEMENTED;

#endif
}

/*
	VA_SCHIP_GetChipsetId
		return the unique chipset identiier that is written at the manufacturing site by Vaiccess equipment (it is publicly accessible)
	arguments
		n/a
	return
		chipset identifier
		kVA_ILLEGAL_HANDLE
*/
DWORD VA_SCHIP_GetChipsetId()
{
	UINT32 dlChipId;

	HxLOG_Print("[Enter]\n");

#ifdef CONFIG_MW_CAS_VIACCESS_ENCRYPTED_CW

	// get chipset id
	if (PalParam_Activated())
	{
		if (PAL_PARAM_GetField(ePAL_PARAM_CHIP_ID, (void *)&dlChipId, sizeof(HUINT32)) != ERR_OK)
		{
			HxLOG_Error("fail to get chipset id\n");
			return kVA_ILLEGAL_HANDLE;
		}
	}
	else
	{
		if (PAL_SYS_GetChipId(&dlChipId) != ERR_OK)
		{
			HxLOG_Error("fail to get chipset id\n");
			return kVA_ILLEGAL_HANDLE;
		}
	}

	HxLOG_Print("[Exit(chipid=%08X])\n", dlChipId);
	return dlChipId;

#else

	HxLOG_Error("secured chipset is not present\n");
	return kVA_ILLEGAL_HANDLE;

#endif
}

