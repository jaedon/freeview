#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "ir_platform.h"
#include "vd_humax.h"

#include "s3_osdrv.h"
#include "s3_vd.h"
#include "s3_vd_loader.h"

#include "ir_api.h"







/********************************************************************
*	External variables
********************************************************************/

/********************************************************************
*	External function prototypes
********************************************************************/
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
extern	HIR_ERR CAS_IR_SECURECHIP_UpdateFusingInfo(void);
extern	HIR_ERR xsvc_cas_IrSecureChipGetCSSN(HUINT32 *pwCSSN);
extern	HIR_ERR CAS_IR_SECURECHIP_GetCrypto(ia_byte *pbCrypto);
extern	HIR_ERR CAS_IR_SECURECHIP_GetRsaBoot(ia_word32 *pwRsaBoot);
extern	HIR_ERR CAS_IR_SECURECHIP_GetJtag(ia_word32 *pwJtag);
extern	HIR_ERR CAS_IR_SECURECHIP_GetCwMode(ia_word32 *pwCwMode);
#endif

/********************************************************************
*	Global variables
********************************************************************/


/********************************************************************
*	Static variables
********************************************************************/
static IHL_STATUS s_stIrStatus;

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
/********************************************************************
*	function name :	 HUMAX_VD_Loader_GetResources
*	input : ..
*	output : Available resource count/list & result.
*	description : VD_Loader의 get resource는 사용 안됨.
********************************************************************/
ia_result HUMAX_VD_Loader_Initialise( void )
{
	HxLOG_Info("\n");
#if defined(IRPLAT_TEST_INCOMPLETED_LOADER)
	return IA_SUCCESS;
#endif
	if( HIR_OK != xsvc_cas_IrLoaderGetStatusInfo(&s_stIrStatus) )
	{
		return IA_FAIL;
	}

	HxLOG_Print("\t u16SysID=(0x%x)\n", s_stIrStatus.u16SysID);
	HxLOG_Print("\t s16SysIDValid=(0x%x)\n", s_stIrStatus.s16SysIDValid);
	HxLOG_Print("\t u16KeyVer=(0x%x)\n", s_stIrStatus.u16KeyVer);
	HxLOG_Print("\t s16KeyVerValid=(0x%x)\n", s_stIrStatus.s16KeyVerValid);
	HxLOG_Print("\t u16SigVer=(0x%x)\n", s_stIrStatus.u16SigVer);
	HxLOG_Print("\t s16SigVerValid=(0x%x)\n", s_stIrStatus.s16SigVerValid);
	HxLOG_Print("\t u16Variant=(0x%x)\n", s_stIrStatus.u16Variant);
	HxLOG_Print("\t s16VariantValid=(0x%x)\n", s_stIrStatus.s16VariantValid);
	HxLOG_Print("\t u8BlockFailMask=(0x%x)\n", s_stIrStatus.u8BlockFailMask);


	return IA_SUCCESS;

}

/********************************************************************
*	function name :	 HUMAX_VD_Loader_GetResources
*	input : ..
*	output : Available resource count/list & result.
*	description : VD_Loader의 get resource는 사용 안됨.
********************************************************************/
ia_result HUMAX_VD_Loader_GetResources(void **ppvList, ia_word16 *pwCount )
{
	HxLOG_Info("\n");
	return eHVD_OK;
}

/********************************************************************
*	function name :	 HUMAX_VD_Loader_Open
*	input : wResourceID
*			- 일반적으로 loader는 하나 이므로 wResourceID는 0
*	output : None
*	description :
********************************************************************/
vd_ref_id HUMAX_VD_Loader_Open( vd_rsc_id wResourceID, void *pvData )
{
	HxLOG_Info("\n");
	// 들어온 Resource ID를 그대로 reference ID로 넘겨주자.
	return wResourceID;
}

/********************************************************************
*	function name :	 HUMAX_VD_Loader_Close
*	input : wRefId
*	output : result
*	description : 별로 할 일이 없군.. 혹시 loader정보를 얻어오기 위해 memory를 alloc해서 썼다면 free?
********************************************************************/
ia_result HUMAX_VD_Loader_Close( vd_ref_id wRefId )
{
	HxLOG_Info("\n");
	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 HUMAX_VD_Loader_IO
*	input : wRefId
*	output : result
*	description :
********************************************************************/
#if defined(CONFIG_PROD_HD5500T)
ia_result HUMAX_VD_Loader_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	vd_loader_system_id_st 			*system_id_st;
	vd_loader_key_version_st 		*key_version_st;
	vd_loader_signature_version_st 	*signature_version_st;
	vd_loader_variant_st 			*variant_st;
	vd_loader_manufacturer_id_st 	*manufacturer_id_st;
	vd_loader_hardware_version_st 	*hardware_version_st;
	vd_loader_load_version_st		*loader_version_st;

	ia_word32 readsize= 0;
	HUINT8*	buf = NULL;
	ia_result result = IA_SUCCESS;

	IR3VDLoaderPrint( DBG_TRACE, ("\n") );

	IR3VDLoaderPrint(DBG_TRACE,("\t++HUMAX_VD_Loader_IO : wRefId(0x%X), wOpCode(0x%X), ReadSize(0x%X), pvDataRead@(0x%X)\n",\
		wRefId, wOpCode, *pwDataReadSize, pvDataRead) );

	if (pwDataReadSize == NULL)
	{
		return IA_FAIL;
	}

	readsize = *pwDataReadSize;

	switch(wOpCode)
	{
		case VD_LOADER_GET_SYSTEM_ID:

			IR3VDLoaderPrint(DBG_TRACE,("\tVD_LOADER_GET_SYSTEM_ID\n") );

			if( readsize < sizeof(vd_loader_system_id_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				IR3VDLoaderError(DBG_PRINT,("\tNot enough memory for VD_LOADER_GET_SYSTEM_ID : needed size(0x%X)\n", \
								sizeof(vd_loader_system_id_st)) );
				*pwDataReadSize = sizeof(system_id_st);
				result = IA_FAIL;
			}
			else
			{
				buf = (HUINT8 *)CA_MEM_Alloc(readsize);
				if(buf != NULL)
				{
					system_id_st = (vd_loader_system_id_st *)buf;
					system_id_st->wSystemId = -1;

					IR3VDLoaderPrint(DBG_PRINT,("\twSystemId = %d\n", system_id_st->wSystemId) );

					HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)system_id_st, sizeof(vd_loader_system_id_st));
					CA_MEM_Free(buf);
					result = IA_SUCCESS;
				}
				else
				{
					IR3VDLoaderAssert(0);
					result = IA_FAIL;
				}
			}
			break;

		case VD_LOADER_GET_KEY_VERSION:

			IR3VDLoaderPrint(DBG_TRACE,("VD_LOADER_GET_KEY_VERSION\n") );

			if( readsize < sizeof(vd_loader_key_version_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				IR3VDLoaderError(DBG_PRINT,("\tNot enough memory for VD_LOADER_GET_KEY_VERSION\n") );
				*pwDataReadSize = sizeof(vd_loader_key_version_st);
				result = IA_FAIL;
			}
			else
			{
				buf = (HUINT8 *)CA_MEM_Alloc(readsize);
				if(buf != NULL)
				{
					key_version_st = (vd_loader_key_version_st *)buf;
					key_version_st->wKeyVersion = 0;

					IR3VDLoaderPrint(DBG_PRINT,("\twKeyVersion = %d\n", key_version_st->wKeyVersion) );

					HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)key_version_st, sizeof(vd_loader_key_version_st));
					CA_MEM_Free(buf);
					result = IA_SUCCESS;
				}
				else
				{
					IR3VDLoaderAssert(0);
					result = IA_FAIL;
				}
			}
			break;

		case VD_LOADER_GET_SIGNATURE_VERSION:

			IR3VDLoaderPrint(DBG_TRACE,("\tVD_LOADER_GET_SIGNATURE_VERSION\n") );

			if( readsize < sizeof(vd_loader_signature_version_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				IR3VDLoaderError(DBG_PRINT,("\tNot enough memory for VD_LOADER_GET_SIGNATURE_VERSION\n") );
				*pwDataReadSize = sizeof(vd_loader_signature_version_st);
				result = IA_FAIL;
			}
			else
			{
				buf = (HUINT8 *)CA_MEM_Alloc(readsize);
				signature_version_st = (vd_loader_signature_version_st *)buf;
				signature_version_st->wSignatureVersion = 1;

				IR3VDLoaderPrint(DBG_PRINT,("\twSignatureVersion = %d\n", signature_version_st->wSignatureVersion) );

				HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)signature_version_st, sizeof(vd_loader_signature_version_st));
				CA_MEM_Free(buf);
				result = IA_SUCCESS;
			}
			break;

		case VD_LOADER_GET_VARIANT:

			IR3VDLoaderPrint(DBG_TRACE,("\tVD_LOADER_GET_VARIANT\n") );

			if( readsize < sizeof(vd_loader_variant_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				IR3VDLoaderError(DBG_PRINT,("\tNot enough memory for VD_LOADER_GET_VARIANT\n") );
				*pwDataReadSize = sizeof(vd_loader_variant_st);
				result = IA_FAIL;
			}
			else
			{
				buf = (HUINT8 *)CA_MEM_Alloc(readsize);
				variant_st = (vd_loader_variant_st *)buf;
				variant_st->wVariant = 0x0F;

				IR3VDLoaderPrint(DBG_PRINT,("\twVariant = %d\n", variant_st->wVariant) );

				HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)variant_st, sizeof(vd_loader_variant_st));
				CA_MEM_Free(buf);
				result = IA_SUCCESS;
			}
			break;

		case VD_LOADER_GET_MANUFACTURER_ID:

			IR3VDLoaderPrint(DBG_TRACE,("\tVD_LOADER_GET_MANUFACTURER_ID\n") );

			if( readsize < sizeof(vd_loader_manufacturer_id_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				IR3VDLoaderError(DBG_PRINT,("\tNot enough memory for VD_LOADER_GET_MANUFACTURER_ID\n") );
				*pwDataReadSize = sizeof(system_id_st);
				result = IA_FAIL;
			}
			else
			{
				HUINT8	ucManufacturerID = 0;

				buf = (HUINT8 *)CA_MEM_Alloc(readsize);
				manufacturer_id_st = (vd_loader_manufacturer_id_st *)buf;
				manufacturer_id_st->wManufacturerId = (ia_word16)0x0F;

				IR3VDLoaderPrint(DBG_PRINT,("\twManufacturerId = %d\n", manufacturer_id_st->wManufacturerId) );

				HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)manufacturer_id_st, sizeof(vd_loader_manufacturer_id_st));
				CA_MEM_Free(buf);
				result = IA_SUCCESS;
			}
			break;

		case VD_LOADER_GET_HARDWARE_VERSION:

			IR3VDLoaderPrint(DBG_TRACE,("\tVD_LOADER_GET_HARDWARE_VERSION\n") );

			if( readsize < sizeof(vd_loader_hardware_version_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				IR3VDLoaderError(DBG_PRINT,("\tNot enough memory for VD_LOADER_GET_HARDWARE_VERSION\n") );
				*pwDataReadSize = sizeof(vd_loader_hardware_version_st);
				result = IA_FAIL;
			}
			else
			{
				HUINT8	ucHardwareID = 0;

				buf = (HUINT8 *)CA_MEM_Alloc(readsize);
				hardware_version_st = (vd_loader_hardware_version_st *)buf;
				hardware_version_st->wHardwareVersion = (ia_word16)47;

				IR3VDLoaderPrint(DBG_PRINT,("\twHardwareVersion = %d\n", hardware_version_st->wHardwareVersion) );

				HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)hardware_version_st, sizeof(vd_loader_hardware_version_st));
				CA_MEM_Free(buf);
				result = IA_SUCCESS;
			}
			break;

		case VD_LOADER_GET_LOAD_VERSION:

			IR3VDLoaderPrint(DBG_TRACE,("\tVD_LOADER_GET_LOAD_VERSION\n") );

			if( readsize < sizeof(vd_loader_load_version_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				IR3VDLoaderError(DBG_PRINT,("\tNot enough memory for VD_LOADER_GET_LOAD_VERSION\n") );
				*pwDataReadSize = sizeof(vd_loader_load_version_st);
				result = IA_FAIL;
			}
			else
			{
				HUINT8	ucLoadVersion = 0;

				buf = (HUINT8 *)CA_MEM_Alloc(readsize);
				loader_version_st = (vd_loader_load_version_st *)buf;
				loader_version_st->wLoadVersion = (ia_word16)0x01;

				IR3VDLoaderPrint(DBG_PRINT,("\twLoadVersion = %d\n", loader_version_st->wLoadVersion) );

				HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)loader_version_st, sizeof(vd_loader_load_version_st));
				CA_MEM_Free(buf);
				result = IA_SUCCESS;
			}
			break;

		default:
			IR3VDLoaderError(DBG_PRINT,("HUMAX_VD_Loader_IO: what resource??? (OpCode=%d)\n",wOpCode ));
			result = VD_NOT_SUPPORTED;
			break;
	}


	IR3VDLoaderPrint(DBG_TRACE,("\t--HUMAX_VD_Loader_IO : result(0x%X)\n", result) );

	return result;
}
#else
ia_result HUMAX_VD_Loader_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	vd_loader_system_id_st 			*system_id_st;
	vd_loader_key_version_st 		*key_version_st;
	vd_loader_signature_version_st 	*signature_version_st;
	vd_loader_variant_st 			*variant_st;
	vd_loader_manufacturer_id_st 	*manufacturer_id_st;
	vd_loader_hardware_version_st 	*hardware_version_st;
	vd_loader_load_version_st		*loader_version_st;
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	vd_loader_secure_chip_st		 *secure_chip_st;
#endif

	ia_word32 readsize= 0;
	HUINT8*	buf = NULL;
	ia_result result = IA_SUCCESS;

	HxLOG_Info("\n");
#if defined(IRPLAT_TEST_INCOMPLETED_LOADER)
	return IA_SUCCESS;
#else

	HxLOG_Info("\t++HUMAX_VD_Loader_IO : wRefId(0x%X), wOpCode(0x%X), ReadSize(0x%X), pvDataRead@(0x%X)\n",\
		wRefId, wOpCode, *pwDataReadSize, pvDataRead );
	if ((pwDataReadSize == NULL) || (pvDataRead == NULL))
	{
		return IA_FAIL;
	}
	readsize = *pwDataReadSize;

	switch(wOpCode)
	{
		case VD_LOADER_GET_SYSTEM_ID:

			HxLOG_Info("\tVD_LOADER_GET_SYSTEM_ID\n");

			if( readsize < sizeof(vd_loader_system_id_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				HxLOG_Print("\tNot enough memory for VD_LOADER_GET_SYSTEM_ID : needed size(0x%X)\n", \
								sizeof(vd_loader_system_id_st));
				*pwDataReadSize = sizeof(system_id_st);
				result = IA_FAIL;
			}
			else
			{
				buf = (HUINT8 *)OxCAS_Malloc(readsize);
				if(buf != NULL)
				{
					system_id_st = (vd_loader_system_id_st *)buf;
					system_id_st->wSystemId = s_stIrStatus.u16SysID;

					HxLOG_Print("\twSystemId = %d\n", system_id_st->wSystemId);

					HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)system_id_st, sizeof(vd_loader_system_id_st));
					OxCAS_Free(buf);
					result = IA_SUCCESS;
				}
				else
				{
					HxLOG_Critical("\n\n");
					result = IA_FAIL;
				}
			}
			break;

		case VD_LOADER_GET_KEY_VERSION:

			HxLOG_Info("VD_LOADER_GET_KEY_VERSION\n") ;

			if( readsize < sizeof(vd_loader_key_version_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				HxLOG_Print("\tNot enough memory for VD_LOADER_GET_KEY_VERSION\n");
				*pwDataReadSize = sizeof(vd_loader_key_version_st);
				result = IA_FAIL;
			}
			else
			{
				buf = (HUINT8 *)OxCAS_Malloc(readsize);
				if(buf != NULL)
				{
					key_version_st = (vd_loader_key_version_st *)buf;
					key_version_st->wKeyVersion = s_stIrStatus.u16KeyVer;

					HxLOG_Print("\twKeyVersion = %d\n", key_version_st->wKeyVersion );

					HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)key_version_st, sizeof(vd_loader_key_version_st));
					OxCAS_Free(buf);
					result = IA_SUCCESS;
				}
				else
				{
					HxLOG_Critical("\n\n");
					result = IA_FAIL;
				}
			}
			break;

		case VD_LOADER_GET_SIGNATURE_VERSION:

			HxLOG_Info("\tVD_LOADER_GET_SIGNATURE_VERSION\n") ;

			if( readsize < sizeof(vd_loader_signature_version_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				HxLOG_Print("\tNot enough memory for VD_LOADER_GET_SIGNATURE_VERSION\n");
				*pwDataReadSize = sizeof(vd_loader_signature_version_st);
				result = IA_FAIL;
			}
			else
			{
				buf = (HUINT8 *)OxCAS_Malloc(readsize);
				signature_version_st = (vd_loader_signature_version_st *)buf;
				signature_version_st->wSignatureVersion = s_stIrStatus.u16SigVer;

				HxLOG_Print("\twSignatureVersion = %d\n", signature_version_st->wSignatureVersion );

				HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)signature_version_st, sizeof(vd_loader_signature_version_st));
				OxCAS_Free(buf);
				result = IA_SUCCESS;
			}
			break;

		case VD_LOADER_GET_VARIANT:

			HxLOG_Info("\tVD_LOADER_GET_VARIANT\n") ;

			if( readsize < sizeof(vd_loader_variant_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				HxLOG_Print("\tNot enough memory for VD_LOADER_GET_VARIANT\n" );
				*pwDataReadSize = sizeof(vd_loader_variant_st);
				result = IA_FAIL;
			}
			else
			{
				buf = (HUINT8 *)OxCAS_Malloc(readsize);
				variant_st = (vd_loader_variant_st *)buf;
				variant_st->wVariant = s_stIrStatus.u16Variant;

				HxLOG_Print("\twVariant = %d\n", variant_st->wVariant) ;

				HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)variant_st, sizeof(vd_loader_variant_st));
				OxCAS_Free(buf);
				result = IA_SUCCESS;
			}
			break;

		case VD_LOADER_GET_MANUFACTURER_ID:

			HxLOG_Info("\tVD_LOADER_GET_MANUFACTURER_ID\n") ;

			if( readsize < sizeof(vd_loader_manufacturer_id_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				HxLOG_Print("\tNot enough memory for VD_LOADER_GET_MANUFACTURER_ID\n" );
				*pwDataReadSize = sizeof(system_id_st);
				result = IA_FAIL;
			}
			else
			{
				HUINT8	ucManufacturerID = 0;

				buf = (HUINT8 *)OxCAS_Malloc(readsize);
				manufacturer_id_st = (vd_loader_manufacturer_id_st *)buf;
				xsvc_cas_IrLoaderGetManuCode(&ucManufacturerID);
				manufacturer_id_st->wManufacturerId = (ia_word16)ucManufacturerID;

				HxLOG_Print("\twManufacturerId = %d\n", manufacturer_id_st->wManufacturerId );

				HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)manufacturer_id_st, sizeof(vd_loader_manufacturer_id_st));
				OxCAS_Free(buf);
				result = IA_SUCCESS;
			}
			break;

		case VD_LOADER_GET_HARDWARE_VERSION:

			HxLOG_Info("\tVD_LOADER_GET_HARDWARE_VERSION\n") ;

			if( readsize < sizeof(vd_loader_hardware_version_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				HxLOG_Print("\tNot enough memory for VD_LOADER_GET_HARDWARE_VERSION\n" );
				*pwDataReadSize = sizeof(vd_loader_hardware_version_st);
				result = IA_FAIL;
			}
			else
			{
				HUINT8	ucHardwareID = 0;

				buf = (HUINT8 *)OxCAS_Malloc(readsize);
				hardware_version_st = (vd_loader_hardware_version_st *)buf;
				xsvc_cas_IrLoaderGetHWCode(&ucHardwareID);
				hardware_version_st->wHardwareVersion = (ia_word16)ucHardwareID;

				HxLOG_Print("\twHardwareVersion = %d\n", hardware_version_st->wHardwareVersion);

				HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)hardware_version_st, sizeof(vd_loader_hardware_version_st));
				OxCAS_Free(buf);
				result = IA_SUCCESS;
			}
			break;

		case VD_LOADER_GET_LOAD_VERSION:

			HxLOG_Info("\tVD_LOADER_GET_LOAD_VERSION\n") ;

			if( readsize < sizeof(vd_loader_load_version_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				HxLOG_Print("\tNot enough memory for VD_LOADER_GET_LOAD_VERSION\n");
				*pwDataReadSize = sizeof(vd_loader_load_version_st);
				result = IA_FAIL;
			}
			else
			{
				HUINT8	ucLoadVersion = 0;

				buf = (HUINT8 *)OxCAS_Malloc(readsize);
				loader_version_st = (vd_loader_load_version_st *)buf;
				xsvc_cas_IrLoaderGetLoadSeqNum(&ucLoadVersion);
				loader_version_st->wLoadVersion = (ia_word16)ucLoadVersion;

				HxLOG_Print("\twLoadVersion = %d\n", loader_version_st->wLoadVersion);

				HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)loader_version_st, sizeof(vd_loader_load_version_st));
				OxCAS_Free(buf);
				result = IA_SUCCESS;
			}
			break;

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
		case VD_LOADER_GET_SECURE_CHIP_CSSN:
		{
			HxLOG_Info("\tVD_LOADER_GET_SECURE_CHIP_CSSN\n");

			if( readsize < sizeof(vd_loader_secure_chip_st) )
			{
				// SoftCell이 주는 size가 모자라면 pwDataReadSize에 필요한 size를 넣은 후에 IA_FAIL을 return.
				HxLOG_Print("\tNot enough memory for VD_LOADER_GET_SECURE_CHIP_CSSN\n");
				*pwDataReadSize = sizeof(vd_loader_secure_chip_st);
				result = IA_FAIL;
			}
			else
			{
				ia_word32 wCSSN;
				ia_byte bCrypto ;
				ia_word32 wRsaBoot ;
				ia_word32 wJtag ;
				ia_word32 wCwMode ;
				HINT32 bReadableCW;

				buf = (HUINT8 *)OxCAS_Malloc(readsize);
				if(buf == NULL)
				{
					HxLOG_Print("CAS_IR_SECURECHIP_GetCwMode: MemAlloc Error\n");
					return IA_FAIL;
				}

				HxSTD_memset(buf, 0, readsize);
				HxLOG_Print("readsize = 0x%02x\n",readsize );

				CAS_IR_SECURECHIP_UpdateFusingInfo();

				xsvc_cas_IrSecureChipGetCSSN((HUINT32*)&wCSSN);
				CAS_IR_SECURECHIP_GetCrypto(&bCrypto);
				CAS_IR_SECURECHIP_GetRsaBoot(&wRsaBoot);
				CAS_IR_SECURECHIP_GetJtag(&wJtag);

				bReadableCW = CAS_IR_SECURECHIP_GetCwMode(&wCwMode);

				secure_chip_st = (vd_loader_secure_chip_st *)buf;
				secure_chip_st->wCSSN= wCSSN;
				secure_chip_st->bCrypto = bCrypto;
				secure_chip_st->wRsaBoot = wRsaBoot;
				secure_chip_st->wJtag = wJtag;
				secure_chip_st->wCwMode =wCwMode ;


				HxLOG_Print("wCSSN = 0x%08x\n",secure_chip_st->wCSSN ) ;
				HxLOG_Print("bCrypto = 0x%02x\n",secure_chip_st->bCrypto) ;
				HxLOG_Print("wRsaBoot = 0x%08x\n",secure_chip_st->wRsaBoot );
				HxLOG_Print("wJtag = 0x%08x\n", secure_chip_st->wJtag);
				HxLOG_Print("wCwMode = 0x%08x\n", secure_chip_st->wCwMode);


				if(VD_LOADER_VERSION > VD_LOADER_VERSION_V2)
				{
					HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)buf, readsize);
					{
						HUINT16 i;
						for(i=0;i<readsize;i++)
						{
							HxLOG_Print(" %02x", *((HUINT8 *)(pvDataRead)+i));
						}
						HxLOG_Print(" \n");

						for(i=0;i<readsize;i++)
						{
							HxLOG_Print(" %02x", *((HUINT8 *)(buf)+i));
						}
						HxLOG_Print(" \n");
					}
					result = IA_SUCCESS;
				}
				else
				{
					if ( (bReadableCW != 0) || (wCwMode == 0) )
					{
						HxLOG_Print("CAS_IR_SECURECHIP_GetCwMode : bReadableCW= %d, wCwMode = %d \n", bReadableCW, wCwMode);
						result = VD_NOT_SUPPORTED;
					}
					else
					{
					HxSTD_memcpy((HUINT8 *)pvDataRead, (HUINT8 *)secure_chip_st, sizeof(vd_loader_secure_chip_st));
						result = IA_SUCCESS;
					}
				}
				OxCAS_Free(buf);
			}
		}
			break;
#endif // #if defined(CONFIG_MW_CAS_IRDETO_SECURE)


		//case VD_SC_SOFTCLIENT_EVENT:
		//	break;
		default:
			HxLOG_Print("HUMAX_VD_Loader_IO: what resource??? (OpCode=%d)\n",wOpCode );
			result = VD_NOT_SUPPORTED;
			break;
	}


	HxLOG_Info("\t--HUMAX_VD_Loader_IO : result(0x%X)\n", result );

	return result;
#endif
}
#endif
/* end of file */

