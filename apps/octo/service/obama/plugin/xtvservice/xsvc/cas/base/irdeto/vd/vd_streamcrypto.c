/********************************************************************
*	Including Headers
********************************************************************/
#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"
#include "ir_svc.h"

#include "s3_vd_stream_crypto.h"

/********************************************************************
*	Definitions (for this source file)
********************************************************************/
//#define VD_CRYPTO_DEBUG



#define	PrintError		HxLOG_Error
#define	PrintFunc		HxLOG_Info




/********************************************************************
*	External variables
********************************************************************/

/********************************************************************
*	External function prototypes
********************************************************************/
#if	defined(IRPLAT_SYS_PVR)		//	secure define needed
extern HUINT32 CAS_IR_DRV_SaveSecureKey(HUINT8 *pucCSSK);
extern HUINT32 CAS_IR_DRV_SaveCryptoKey(HUINT16 usIrServiceHandle, HUINT8 type, HUINT8* CryptoKey, HUINT32 len);
extern HUINT32 CAS_IR_DRV_ClearSecuredKey(void);
#endif


/********************************************************************
*	Global variables
********************************************************************/


/********************************************************************
*	Static variables
********************************************************************/


/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/

typedef enum
{
	AES,
	DES3
}CRYPTO_ALGORITHM;

typedef struct CRYPTO_INFO_T
{
	HUINT32				referenceId;
	CRYPTO_ALGORITHM 	algorithm;
	HUINT8				mode;
	HUINT8				key[IR_CRYPTO_KEY_LEN];
	HINT32				keyLen;

	HUINT8				aucEvenKey[IR_CRYPTO_KEY_LEN];
	HUINT8				aucOddKey [IR_CRYPTO_KEY_LEN];

	struct CRYPTO_INFO_T	*next;
}CRYPTO_INFO;

static CRYPTO_INFO		gCryptoInfoHead;


static CRYPTO_INFO* FindCryptoInfoByReferenceId( vd_ref_id wRefId)
{
	CRYPTO_INFO *ptr;

	ptr = gCryptoInfoHead.next;

	while(ptr)
	{
		if(ptr->referenceId ==  wRefId)
		{
			return ptr;
		}
		ptr = ptr->next;
	}
	return NULL;
}


/********************************************************************
*	function name :	 HUMAX_VD_CRYPTO_Initialise
*	input : void(none)
*	output : result
*	description : ..
********************************************************************/
ia_result HUMAX_VD_CRYPTO_Initialise(void)
{
	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 HUMAX_VD_CRYPTO_GetResources
*	input : ..
*	output : Available resource count/list & result.
*	description : Not used for this virtual driver.
********************************************************************/
HUINT32 HUMAX_VD_CRYPTO_GetResources(void **ppvList, ia_word16 *pwCount )
{
	return HIR_OK;
}


/********************************************************************
*	function name :	 HUMAX_VD_CRYPTO_Open
*	input : wResourceID
*	output : None
*	description :
********************************************************************/
vd_ref_id HUMAX_VD_CRYPTO_Open( vd_rsc_id wResourceID, void *pvData )
{
	vd_stream_crypto_open_st	*crypto_open;
	CRYPTO_INFO *ptr, *newCryptoInfo;

	crypto_open  = (vd_stream_crypto_open_st*)pvData;
	HxLOG_Debug("[VD_CRYPTO]Open - wResourceID=0x%x, crypto algorithm=%d, operation=%d\n", (HUINT32)wResourceID, crypto_open->eAlgorithm, crypto_open->eOperation);

	if(crypto_open->eAlgorithm == VD_CRYPTO_ALGORITHM_AES)
		HxLOG_Debug("\t[VD_CRYPTO_ALGORITHM_AES]\n");
	else if(crypto_open->eAlgorithm == VD_CRYPTO_ALGORITHM_3DES)
		HxLOG_Debug("\t[VD_CRYPTO_ALGORITHM_3DES]\n");

	//One referenceId per resource(recId)

	newCryptoInfo = OxCAS_Malloc(sizeof(CRYPTO_INFO));
	if(newCryptoInfo == NULL)
	{
		PrintError("[VD_CRYPT] Open - alloc error\n");
		return VD_REF_ID_ERROR;
	}

	newCryptoInfo->referenceId = wResourceID;
	newCryptoInfo->algorithm = crypto_open->eAlgorithm;
	newCryptoInfo->mode = crypto_open->eOperation;
	newCryptoInfo->next = NULL;

	ptr = gCryptoInfoHead.next;

	if(ptr == NULL)
		gCryptoInfoHead.next = newCryptoInfo;
	else
	{
		while(ptr->next)
			ptr = ptr->next;

		ptr->next = newCryptoInfo;
	}

	return wResourceID;
}

/********************************************************************
*	function name :	 HUMAX_VD_CRYPTO_Close
*	input : wRefId
*	output : result
*	description : ..
********************************************************************/
ia_result HUMAX_VD_CRYPTO_Close( vd_ref_id wRefId )
{
	CRYPTO_INFO *ptr, *prev;

	HxLOG_Debug("[VD_CRYPTO]Close wRefId=0x%x\n", (HUINT32)wRefId);

	ptr = gCryptoInfoHead.next;
	prev = NULL;

	while(ptr)
	{
		if(ptr->referenceId == wRefId)
		{
			if(prev == NULL)
			{
				gCryptoInfoHead.next = ptr->next;
			}
			else
			{
				prev->next = ptr->next;
			}

			if(ptr != NULL)
			{
				OxCAS_Free(ptr);
				gCryptoInfoHead.next = NULL;
			}
			return IA_SUCCESS;
		}
		prev = ptr;
		ptr = ptr->next;

	}

	return IA_FAIL;
}

/********************************************************************
*	function name :	 HUMAX_VD_CRYPTO_IO
*	input : wRefId
*	output : result
*	description :
********************************************************************/
ia_result HUMAX_VD_CRYPTO_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{

	vd_stream_crypto_key_st		*crypto_key;
	vd_stream_crypto_key_by_type_st	*crypto_key_type;

	HUINT8						*p = (HUINT8 *)pvDataWrite;
	CRYPTO_INFO					*pCryptoInfo;

	PrintFunc("\t[VD_CRYPTO] ++HUMAX_VD_CRYPTO_IO : wRefId(0x%X), wOpCode(0x%X), wDataWriteSize(0x%X)\n", (HUINT32)wRefId, wOpCode, (HUINT32)wDataWriteSize);
	switch(wOpCode)
	{
		case	VD_STREAM_CRYPTO_SET_KEY_BY_TYPE:
			pCryptoInfo = FindCryptoInfoByReferenceId(wRefId);
			if (pCryptoInfo == NULL)
			{
				PrintError("[VD_CRYPTO] ERROR-invalid referenceId wRefId = 0x%x\n", (HUINT32)wRefId);
				return IA_FAIL;
			}
			crypto_key_type = (vd_stream_crypto_key_by_type_st*)p;
			pCryptoInfo->keyLen = crypto_key_type->wKeyLength;

			p += sizeof(vd_stream_crypto_key_by_type_st);
			if (crypto_key_type->eKeyType == VD_STREAM_CRYPTO_KEY_ODD)
				HxSTD_memcpy(pCryptoInfo->aucOddKey, p, pCryptoInfo->keyLen);
			else
				HxSTD_memcpy(pCryptoInfo->aucEvenKey, p, pCryptoInfo->keyLen);

#if	defined(DEBUG_PRINT_ENC_KEY_INFO)
			HxLOG_Print("\t[VD_CRYPTO] SET_KEY_BY_TYPE [%s] keyLen=%d\n", (crypto_key_type->eKeyType == VD_STREAM_CRYPTO_KEY_ODD) ? "ODD" : "EVEN", pCryptoInfo->keyLen);
			{
				HINT32		i;
				for (i = 0; i < pCryptoInfo->keyLen; i++)
				{
					HxLOG_Print(".%02X ", p[i]);
					if (i % 16 == 15)
						HxLOG_Print("\n");
				}
				HxLOG_Print("\n");
			}
#endif

#if	defined(IRPLAT_SYS_PVR)
			if (crypto_key_type->eKeyType == VD_STREAM_CRYPTO_KEY_ODD)
				CAS_IR_DRV_SaveCryptoKey(pCryptoInfo->referenceId, STREAM_CRYPTO_TYPE_ODD, pCryptoInfo->aucOddKey, pCryptoInfo->keyLen);
			else
				CAS_IR_DRV_SaveCryptoKey(pCryptoInfo->referenceId, STREAM_CRYPTO_TYPE_EVEN, pCryptoInfo->aucEvenKey, pCryptoInfo->keyLen);
#endif
			break;
		case	VD_STREAM_CRYPTO_SET_SKEY:
			if (wDataWriteSize < 0)
			{
				HxLOG_Debug("\t[VD_CRYPTO] VD_STREAM_CRYPTO_SET_SKEY : Invalid Parameter\n");
				return IA_FAIL;
			}

			if (wDataWriteSize == 0)
			{
				//	TODO: Clear Protection Key
#if	defined(IRPLAT_SYS_PVR)		//	secure define needed
				CAS_IR_DRV_ClearSecuredKey();
#endif
			} else
			{
				HINT32		i;
				HUINT8		aucEncryptCSSK[IR_CRYPTO_KEY_LEN];

				if (wDataWriteSize != IR_CRYPTO_KEY_LEN)
				{
					PrintError("\t[VD_CRYPTO] SKEY Length is not valid!!!\n");
					return IA_FAIL;
				}
				HxSTD_memset(aucEncryptCSSK, 0, wDataWriteSize);
				//	DI에서 Swap한다고 함.
				for (i = 0; i < 8; i++)
				{
					aucEncryptCSSK[i + 8]	= p[i];
					aucEncryptCSSK[i]		= p[i + 8];
				}

				//	TODO: Set SKEY!
#if	defined(IRPLAT_SYS_PVR)		//	secure define needed
				CAS_IR_DRV_SaveSecureKey(aucEncryptCSSK);
#endif

				HxLOG_Debug("\tDump SKEY\n");
				for (i = 0; i < wDataWriteSize; i++)
					HxLOG_Debug("\t\t%02X ", aucEncryptCSSK[i]);
				HxLOG_Debug("\n");
			}

			break;
		case	VD_STREAM_CRYPTO_SET_KEY:
			HxLOG_Debug("[VD_CRYPTO]VD_STREAM_CRYPTO_SET_KEY!\n");
			pCryptoInfo = FindCryptoInfoByReferenceId(wRefId);
			if(pCryptoInfo == NULL)
			{
				PrintError("[VD_CRYPTO] ERROR-invalid referenceId wRefId=0x%x\n", (HUINT32)wRefId);
				return IA_FAIL;
			}

			crypto_key = (vd_stream_crypto_key_st*)p;
			pCryptoInfo->keyLen = crypto_key->wKeyLength;

			p += sizeof(vd_stream_crypto_key_st);
			HxSTD_memcpy((HUINT8 *)pCryptoInfo->key, p, pCryptoInfo->keyLen);

#if defined(DEBUG_PRINT_ENC_KEY_INFO)
{
			HINT32 i;
			HUINT8* p;

			HxLOG_Print("\t[VD_CRYPTO]SET_KEY keyLen=%d\n", pCryptoInfo->keyLen);

			p = (HUINT8*)pCryptoInfo->key;
			for( i = 0; i < pCryptoInfo->keyLen; i++ )
			{
					HxLOG_Print(".%02X ", p[i]);
					if( i % 16 == 15 ) HxLOG_Print("\n");
			}
			HxLOG_Print("\n");
}
#endif
#if defined(IRPLAT_SYS_PVR)
			CAS_IR_DRV_SaveCryptoKey(pCryptoInfo->referenceId, 0, pCryptoInfo->key, pCryptoInfo->keyLen);
#endif

			break;

	}

	return IA_SUCCESS;
}

/* end of file */

