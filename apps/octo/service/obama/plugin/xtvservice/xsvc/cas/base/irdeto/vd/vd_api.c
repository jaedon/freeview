
/********************************************************************
*	Including Headers
********************************************************************/
//#include "ir_common.h"
#include "octo_common.h"

#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"
#include "ir_svc.h"

//from softcell
#include "s3_vd.h"

/********************************************************************
*	Definitions (for this source file)
********************************************************************/

/*************************************************************************************************************************************/
/*	Following defs. must be used with EXCLUSIVE when using S3 debug library.                                                 */
/* 	If not Debug library, All defs. MUST not be declared  !!!                                                                              */
//		#define USING_S3_DBGLIB_PRINT  /* when using irdeto softcell debug library, and need to print out logging data */
//		#define USING_S3_DBGLIB_NOT_PRINT  /* when using irdeto softcell debug library, and NO need logging data  */
//		#define USING_S3_DBGLIB_PRINT_ONLY_S3LOG /*only S3 Log */
/**************************************************************************************************************************************/






/********************************************************************
*	External variables
********************************************************************/

/********************************************************************
*	External function prototypes
********************************************************************/
extern ia_result HUMAX_VD_Loader_Initialise( void );
extern ia_result HUMAX_VD_PersistentStorage_Initialise(void);
extern ia_result HUMAX_VD_SC_Initialise(void);
extern ia_result HUMAX_VD_MACROVISION_Initialise(void);

extern HUINT32 HUMAX_VD_SC_GetResources(void **ppvList, ia_word16 *pwCount );
extern HUINT32 HUMAX_VD_MACROVISION_GetResources(void **ppvList, ia_word16 *pwCount );

extern vd_ref_id HUMAX_VD_SC_Open( vd_rsc_id wResourceID, void *pvData );
extern vd_ref_id HUMAX_VD_PersistentStorage_Open( vd_rsc_id wResourceID, void *pvData );
extern vd_ref_id HUMAX_VD_Loader_Open( vd_rsc_id wResourceID, void *pvData );
extern vd_ref_id HUMAX_VD_MACROVISION_Open( vd_rsc_id wResourceID, void *pvData );
extern vd_ref_id HUMAX_VD_CRYPTO_Open( vd_rsc_id wResourceID, void *pvData );

extern ia_result HUMAX_VD_SC_Close( vd_ref_id wRefId );
extern ia_result HUMAX_VD_PersistentStorage_Close( vd_ref_id wRefId );
extern ia_result HUMAX_VD_Loader_Close( vd_ref_id wRefId );
extern ia_result HUMAX_VD_MACROVISION_Close( vd_ref_id wRefId );
extern ia_result HUMAX_VD_CRYPTO_Close( vd_ref_id wRefId );

extern ia_result HUMAX_VD_SC_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
extern ia_result HUMAX_VD_PersistentStorage_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
extern ia_result HUMAX_VD_Loader_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
extern ia_result HUMAX_VD_MACROVISION_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
extern ia_result HUMAX_VD_CRYPTO_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
#if defined(IRPLAT_SYS_PVR)
extern ia_result HUMAX_VD_ExtStorage_Initialise(void);
extern ia_result HUMAX_VD_ExtStorage_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead );
extern ia_result HUMAX_VD_ExtStorage_Close( vd_ref_id wRefId );
extern vd_ref_id HUMAX_VD_ExtStorage_Open( vd_rsc_id wResourceID, void *pvData );
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


/********************************************************************
*	function name :	 VD_Initialise
*	input : void(None)
*	output : result
*	description : Initialize the virtual driver.
********************************************************************/
static ia_result INT_VD_Initialise( void )
{
	ia_result result = IA_SUCCESS;

	HxLOG_Info(" VD_Initialise\n");

	result = HUMAX_VD_Loader_Initialise();
	if( result != IA_SUCCESS )
	{
		HxLOG_Print(" HUMAX_VD_Loader_Initialise, %08X\n", result);
	}
	result = HUMAX_VD_PersistentStorage_Initialise();
	if( result != IA_SUCCESS )
	{
		HxLOG_Print("HUMAX_VD_PersistentStorage_Initialise, %08X\n", result);
	}
	result = HUMAX_VD_SC_Initialise();
	if( result != IA_SUCCESS )
	{
		HxLOG_Print("HUMAX_VD_SC_Initialise, %08X\n", result);
	}
	result = HVD_DMX_Initialise();
	if( result != IA_SUCCESS )
	{
		HxLOG_Print("HVD_DMX_Initialise, %08X\n", result);
	}
	result = HUMAX_VD_MACROVISION_Initialise();
	if( result != IA_SUCCESS )
	{
		HxLOG_Print("HUMAX_VD_MACROVISION_Initialise, %08X\n", result);
	}
	result = HVD_DSC_Initialise();
	if( result != IA_SUCCESS )
	{
		HxLOG_Print("HVD_DSC_Initialise, %08X\n", result);
	}
	result = HDV_TIMER_Initialise();
	if( result != IA_SUCCESS )
	{
		HxLOG_Print("HDV_TIMER_Initialise, %08X\n", result);
	}
#if defined(IRPLAT_SYS_PVR)
	result = HUMAX_VD_ExtStorage_Initialise();
	if( result != IA_SUCCESS )
	{
		HxLOG_Print("HUMAX_VD_ExtStorage_Initialise,%08X\n", result);
	}
#endif

#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
	result = HUMAX_VD_DvbOob_Initialise();
	if( result != IA_SUCCESS )
	{
		HxLOG_Print("HUMAX_VD_DvbOob_Initialise,%08X\n", result);
	}
#endif //defined(CONFIG_OP_ZIGGO_AVN_CLIENT)

	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 VD_Terminate
*	input : void(None)
*	output : void(None)
*	description : Terminate the virtual driver.
********************************************************************/
static void INT_VD_Terminate( void )
{
	HxLOG_Info("VD_Terminate\n");
	return;
}

/********************************************************************
*	function name :	 VD_GetDriverVersion
*	input : vd_driver_type_e
*	output : driver version
*	description : Return the driver version for that virtual driver.
*				Version numbers start with 1.
				0 (VD_NOT_IMPLEMENTED) Means driver not implemented.
********************************************************************/
static ia_word16 INT_VD_GetDriverVersion( vd_driver_type_e eDriverType )
{
	HxLOG_Info(" : eDriverType(0x%X)\n", eDriverType);
	switch(eDriverType)
	{
	case VD_DRIVER_SMARTCARD:
		return VD_SMARTCARD_VERSION;
	case VD_DRIVER_PERSISTENT_STORAGE:
		return VD_PERSISTENT_STORAGE_VERSION;
	case VD_DRIVER_DVB_FILTER:
		return VD_FILTER_VERSION;
	case VD_DRIVER_DVB_DESCRAMBLER:
		return VD_DESCRAMBLER_VERSION;
	case VD_DRIVER_IP_SOCKET:
		return VD_NOT_IMPLEMENTED;
	case VD_DRIVER_TIMER:
		return VD_TIMER_VERSION;
	case VD_DRIVER_IP_DECRYPTION_UNIT:
		return VD_NOT_IMPLEMENTED;
	case VD_DRIVER_LOADER:
		return VD_LOADER_VERSION;
	case VD_DRIVER_MACROVISION:
		return VD_MACROVISION_VERSION;
	case VD_DRIVER_STREAM_CRYPTO:
		return VD_STREAMCRYPTO_VERSION;
	case VD_DRIVER_EXTERNAL_STORAGE:
		return VD_EXTERNAL_STORAGE_VERSION;
	case VD_DRIVER_DVB_OOB:
#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
		return VD_DVB_OOB_V1;
#else //defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
		return VD_NOT_IMPLEMENTED;
#endif
	default:
		HxLOG_Print("INT_VD_GetDriverVersion what driver? (eDriverType= %d)\n", eDriverType);
		return VD_NOT_IMPLEMENTED;
	}
}

/********************************************************************
*	function name :	 VD_GetResources
*	input : vd_driver_type_e
*	output : Available resource count/list & result.
*	description : Return the resources available for that virtual driver.
********************************************************************/
static ia_result INT_VD_GetResources( vd_driver_type_e eDriverType, void **ppvList, ia_word16 *pwCount )
{
#ifdef CONFIG_DEBUG // remove warning
	ia_result result = IA_SUCCESS;
#endif

	HxLOG_Info("eDriverType(0x%X)\n", eDriverType);
	switch(eDriverType)
	{
	case VD_DRIVER_SMARTCARD:
		HxLOG_Print("\t=>VD_DRIVER_SMARTCARD\n");
		if( HUMAX_VD_SC_GetResources(ppvList, pwCount) != eHVD_OK )
		{
			return VD_NOT_SUPPORTED;
		}
		return IA_SUCCESS;
	case VD_DRIVER_PERSISTENT_STORAGE:
		// SoftCell 3.1.24에서는 사용 안됨.
		HxLOG_Print("\t=>VD_DRIVER_PERSISTENT_STORAGE\n");
		return IA_SUCCESS;

	case VD_DRIVER_DVB_FILTER:
		HxLOG_Print("\t[VD_API] GetResources : VD_DRIVER_DVB_FILTER\n");
		if( eHVD_OK != HVD_DMX_GetResources(ppvList, pwCount) )
		{
			return VD_NOT_SUPPORTED;
		}
		return IA_SUCCESS;

	case VD_DRIVER_DVB_DESCRAMBLER:
		/*Doc describles that it will not be used for descrambler  */
		HxLOG_Print("\t=>VD_DRIVER_DVB_DESCRAMBLER\n");
		return VD_NOT_SUPPORTED;

	case VD_DRIVER_IP_SOCKET:
		HxLOG_Print("\t=>VD_DRIVER_IP_SOCKET\n");
		return VD_NOT_SUPPORTED;

	case VD_DRIVER_TIMER:
		// SoftCell 3.1.24에서는 사용 안됨.
		HxLOG_Print("\t=>VD_DRIVER_TIMER\n");
		return IA_SUCCESS;

	case VD_DRIVER_IP_DECRYPTION_UNIT:
		HxLOG_Print("\t=>VD_DRIVER_IP_DECRYPTION_UNIT\n");
		return VD_NOT_SUPPORTED;

	case VD_DRIVER_LOADER:
		HxLOG_Print("\t=>VD_DRIVER_LOADER\n");
		return IA_SUCCESS;

	case VD_DRIVER_MACROVISION:
		HxLOG_Print("\t[VD_API] GetResources : VD_DRIVER_MACROVISION\n");
		if( HUMAX_VD_MACROVISION_GetResources(ppvList, pwCount) != eHVD_OK )
		{
			return VD_NOT_SUPPORTED;
		}
		return IA_SUCCESS;

	case VD_DRIVER_STREAM_CRYPTO:
		HxLOG_Print("\t=>VD_DRIVER_STREAM_CRYPTO\n");
		// SoftCell 3.1.24에서는 사용 안됨.
		return VD_NOT_SUPPORTED;

	case VD_DRIVER_EXTERNAL_STORAGE:
		HxLOG_Print("\t=>VD_DRIVER_EXTERNAL_STORAGE\n");
		// SoftCell 3.1.24에서는 사용 안됨.
		// TODO: fix it when integrating HDD. jichoi.
		return VD_NOT_SUPPORTED;

	case VD_DRIVER_DVB_OOB:
		HxLOG_Print("\t=>VD_DRIVER_DVB_OOB\n");
		return VD_NOT_SUPPORTED;

	default:
		HxLOG_Print("\t=>INVALID\n");
		break;
	}
#ifdef CONFIG_DEBUG
	HxLOG_Print("[VD_API] --VD_GetResources : result(0x%X)\n", result);
#endif
	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 VD_Open
*	input : vd_driver_type_e / wResourceID
*	output : reference ID
*	description : Open a resource.
********************************************************************/
static vd_ref_id INT_VD_Open( vd_driver_type_e eDriverType, vd_rsc_id wResourceID, void *pvData )
{
	vd_ref_id ref_id[VD_DRIVER_IPDC_DESCRAMBLER] = {VD_REF_ID_ERROR};

	HxLOG_Info("\n[VD_API] ++VD_Open : eDriverType(0x%X), wResourceID(0x%X)\n", (HUINT32)eDriverType, (HUINT32)wResourceID);
	switch(eDriverType)
	{
	case VD_DRIVER_SMARTCARD:
		HxLOG_Print("\t=>VD_DRIVER_SMARTCARD\n");
		ref_id[VD_DRIVER_SMARTCARD] = HUMAX_VD_SC_Open(wResourceID, pvData);
		return ref_id[VD_DRIVER_SMARTCARD];
	case VD_DRIVER_PERSISTENT_STORAGE:
		HxLOG_Print("\t=>VD_DRIVER_PERSISTENT_STORAGE\n");
		ref_id[VD_DRIVER_PERSISTENT_STORAGE] = HUMAX_VD_PersistentStorage_Open(wResourceID, pvData);
		return ref_id[VD_DRIVER_PERSISTENT_STORAGE];

	case VD_DRIVER_DVB_FILTER:
		HxLOG_Print("\t=>VD_DRIVER_DVB_FILTER\n");
		ref_id[VD_DRIVER_DVB_FILTER] = HVD_DMX_Open(wResourceID, pvData);
		return ref_id[VD_DRIVER_DVB_FILTER];

	case VD_DRIVER_DVB_DESCRAMBLER:
		HxLOG_Print("\t=>VD_DRIVER_DVB_DESCRAMBLER\n");
		ref_id[VD_DRIVER_DVB_DESCRAMBLER] = HVD_DSC_Open(wResourceID, pvData);
		return ref_id[VD_DRIVER_DVB_DESCRAMBLER];

	case VD_DRIVER_IP_SOCKET:
		HxLOG_Print("\t=>VD_DRIVER_IP_SOCKET\n");
		return ref_id[VD_DRIVER_IP_SOCKET];

	case VD_DRIVER_TIMER:
		HxLOG_Print("\t=>VD_DRIVER_TIMER\n");
		ref_id[VD_DRIVER_TIMER] = HDV_TIMER_Open(wResourceID, pvData);
		return ref_id[VD_DRIVER_TIMER];

	case VD_DRIVER_IP_DECRYPTION_UNIT:
		HxLOG_Print("\t=>VD_DRIVER_IP_DECRYPTION_UNIT\n");
		return ref_id[VD_DRIVER_IP_DECRYPTION_UNIT];

	case VD_DRIVER_LOADER:
		HxLOG_Print("\t=>VD_DRIVER_LOADER\n");
		ref_id[VD_DRIVER_LOADER] = HUMAX_VD_Loader_Open(wResourceID, pvData);
		return ref_id[VD_DRIVER_LOADER];

	case VD_DRIVER_MACROVISION:
		HxLOG_Print("\t=>VD_DRIVER_MACROVISION\n");
		ref_id[VD_DRIVER_MACROVISION] = HUMAX_VD_MACROVISION_Open(wResourceID, pvData);
		return ref_id[VD_DRIVER_MACROVISION];

	case VD_DRIVER_STREAM_CRYPTO:
		HxLOG_Print("\t=>VD_DRIVER_STREAM_CRYPTO\n");
		ref_id[VD_DRIVER_STREAM_CRYPTO] = HUMAX_VD_CRYPTO_Open(wResourceID, pvData);
		return ref_id[VD_DRIVER_STREAM_CRYPTO];

	case VD_DRIVER_EXTERNAL_STORAGE:
		HxLOG_Print("\t=>VD_DRIVER_EXTERNAL_STORAGE\n");
#if defined(IRPLAT_SYS_PVR)
		ref_id[VD_DRIVER_EXTERNAL_STORAGE] = HUMAX_VD_ExtStorage_Open(wResourceID, pvData);
#endif
		return ref_id[VD_DRIVER_EXTERNAL_STORAGE];

	case VD_DRIVER_DVB_OOB:
#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
		ref_id[VD_DRIVER_DVB_OOB] = HUMAX_VD_DvbOob_Open(wResourceID, pvData);
		return ref_id[VD_DRIVER_DVB_OOB];
#else //defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
		HxLOG_Print("\t=>VD_DRIVER_DVB_OOB\n");
		return VD_REF_ID_ERROR;
#endif

	default:
		HxLOG_Print("\t=>INVALID\n");
		break;
	}

	HxLOG_Print("[VD_API] --VD_Open : ref_id(0x%X)\n", (HUINT32)ref_id);

	return VD_REF_ID_ERROR;
}

/********************************************************************
*	function name :	 VD_Close
*	input : vd_driver_type_e / wRefId
*	output : result
*	description : Close a resource.
********************************************************************/
static ia_result INT_VD_Close( vd_driver_type_e eDriverType, vd_ref_id wRefId )
{
	ia_result result[VD_DRIVER_IPDC_DESCRAMBLER] = {IA_SUCCESS};

	HxLOG_Print("\n[VD_API] ++VD_Close : eDriverType(0x%X), wRefId(0x%X)\n", (HUINT32)eDriverType, (HUINT32)wRefId);
	switch(eDriverType)
	{
	case VD_DRIVER_SMARTCARD:
		HxLOG_Print("\t=>VD_DRIVER_SMARTCARD\n");
		result[VD_DRIVER_SMARTCARD] = HUMAX_VD_SC_Close(wRefId);
		return result[VD_DRIVER_SMARTCARD];
	case VD_DRIVER_PERSISTENT_STORAGE:
		HxLOG_Print("\t=>VD_DRIVER_PERSISTENT_STORAGE\n");
		result[VD_DRIVER_PERSISTENT_STORAGE] = HUMAX_VD_PersistentStorage_Close(wRefId);
		return result[VD_DRIVER_PERSISTENT_STORAGE];

	case VD_DRIVER_DVB_FILTER:
		HxLOG_Print("\t=>VD_DRIVER_DVB_FILTER\n");
		result[VD_DRIVER_DVB_FILTER] = HVD_DMX_Close(wRefId);
		return result[VD_DRIVER_DVB_FILTER];

	case VD_DRIVER_DVB_DESCRAMBLER:
		HxLOG_Print("\t=>VD_DRIVER_DVB_DESCRAMBLER\n");
#if defined(CONFIG_WA_2SC_SAME_ENTITLEMENT)	// ewlee 20100825
		result[VD_DRIVER_DVB_DESCRAMBLER] = HVD_DSC_MultipleClose(wRefId);
#else
		result[VD_DRIVER_DVB_DESCRAMBLER] = HVD_DSC_Close(wRefId);
#endif
		return result[VD_DRIVER_DVB_DESCRAMBLER];

	case VD_DRIVER_IP_SOCKET:
		HxLOG_Print("\t=>VD_DRIVER_IP_SOCKET\n");
		return result[VD_DRIVER_IP_SOCKET];

	case VD_DRIVER_TIMER:
		HxLOG_Print("\t=>VD_DRIVER_TIMER\n");
		result[VD_DRIVER_TIMER] = HDV_TIMER_Close(wRefId);
		return result[VD_DRIVER_TIMER];

	case VD_DRIVER_IP_DECRYPTION_UNIT:
		HxLOG_Print("\t=>VD_DRIVER_IP_DECRYPTION_UNIT\n");
		return result[VD_DRIVER_IP_DECRYPTION_UNIT];

	case VD_DRIVER_LOADER:
		HxLOG_Print("\t=>VD_DRIVER_LOADER\n");
		result[VD_DRIVER_LOADER] = HUMAX_VD_Loader_Close(wRefId);
		return result[VD_DRIVER_LOADER];

	case VD_DRIVER_MACROVISION:
		HxLOG_Print("\t=>VD_DRIVER_MACROVISION\n");
		result[VD_DRIVER_MACROVISION] = HUMAX_VD_MACROVISION_Close(wRefId);
		return result[VD_DRIVER_MACROVISION];

	case VD_DRIVER_STREAM_CRYPTO:
		HxLOG_Print("\t=>VD_DRIVER_STREAM_CRYPTO\n");
		result[VD_DRIVER_STREAM_CRYPTO] = HUMAX_VD_CRYPTO_Close(wRefId);
		return result[VD_DRIVER_STREAM_CRYPTO];

	case VD_DRIVER_EXTERNAL_STORAGE:
		HxLOG_Print("\t=>VD_DRIVER_EXTERNAL_STORAGE\n");
#if defined(IRPLAT_SYS_PVR)
		result[VD_DRIVER_EXTERNAL_STORAGE] = HUMAX_VD_ExtStorage_Close(wRefId);
#endif
		return result[VD_DRIVER_EXTERNAL_STORAGE];

	case VD_DRIVER_DVB_OOB:
		HxLOG_Print("\t=>VD_DRIVER_DVB_OOB(C)\n");
#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
		result[VD_DRIVER_DVB_OOB] = HUMAX_VD_DvbOob_Close(wRefId);
		return result[VD_DRIVER_DVB_OOB];
#else //defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
		return IA_SUCCESS;
#endif //defined(CONFIG_OP_ZIGGO_AVN_CLIENT)


	default:
		HxLOG_Print("\t=>INVALID\n");
		break;
	}

	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 VD_IO
*	input : vd_driver_type_e / wRefId
*	output : result
*	description : send control messages to a resource.
********************************************************************/
static ia_result INT_VD_IO( vd_driver_type_e eDriverType, vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	ia_result result[VD_DRIVER_DVB_OOB] = {IA_SUCCESS};

	HxLOG_Print("\n[VD_API] ++VD_IO : eDriverType(0x%X), wRefId(0x%X), wOpCode(0x%X)\n", (HUINT32)eDriverType, (HUINT32)wRefId, wOpCode);
	if( pwDataReadSize != NULL && pvDataRead != NULL  )
	{
		if( *pwDataReadSize != 0 )
		{
			HxSTD_memset(pvDataRead, 0, *pwDataReadSize);
		}
	}

	switch(eDriverType)
	{
	case VD_DRIVER_SMARTCARD:
		HxLOG_Print("\t=>VD_DRIVER_SMARTCARD\n");
		result[VD_DRIVER_SMARTCARD] = HUMAX_VD_SC_IO(wRefId, wOpCode, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
		return result[VD_DRIVER_SMARTCARD];
	case VD_DRIVER_PERSISTENT_STORAGE:
		HxLOG_Print("\t=>VD_DRIVER_PERSISTENT_STORAGE\n");
		result[VD_DRIVER_PERSISTENT_STORAGE] = HUMAX_VD_PersistentStorage_IO(wRefId, wOpCode, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
		return result[VD_DRIVER_PERSISTENT_STORAGE];

	case VD_DRIVER_DVB_FILTER:
		HxLOG_Print("\t=>VD_DRIVER_DVB_FILTER\n");
		result[VD_DRIVER_DVB_FILTER] = HVD_DMX_IO(wRefId, wOpCode, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
		return result[VD_DRIVER_DVB_FILTER];

	case VD_DRIVER_DVB_DESCRAMBLER:
		HxLOG_Print("\t=>VD_DRIVER_DVB_DESCRAMBLER\n");
		result[VD_DRIVER_DVB_DESCRAMBLER] = HVD_DSC_IO(wRefId, wOpCode, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
		return result[VD_DRIVER_DVB_DESCRAMBLER];

	case VD_DRIVER_IP_SOCKET:
		HxLOG_Print("\t=>VD_DRIVER_IP_SOCKET\n");
		return result[VD_DRIVER_IP_SOCKET];

	case VD_DRIVER_TIMER:
		HxLOG_Print("\t=>VD_DRIVER_TIMER\n");
		result[VD_DRIVER_TIMER] = HDV_TIMER_IO(wRefId, wOpCode, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
		return result[VD_DRIVER_TIMER];

	case VD_DRIVER_IP_DECRYPTION_UNIT:
		HxLOG_Print("\t=>VD_DRIVER_IP_DECRYPTION_UNIT\n");
		return result[VD_DRIVER_IP_DECRYPTION_UNIT];

	case VD_DRIVER_LOADER:
		HxLOG_Print("\t=>VD_DRIVER_LOADER\n");
		result[VD_DRIVER_LOADER] = HUMAX_VD_Loader_IO(wRefId, wOpCode, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
		return result[VD_DRIVER_LOADER];

	case VD_DRIVER_MACROVISION:
		HxLOG_Print("\t=>VD_DRIVER_MACROVISION\n");
		result[VD_DRIVER_MACROVISION] = HUMAX_VD_MACROVISION_IO(wRefId, wOpCode, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
		return result[VD_DRIVER_MACROVISION];

	case VD_DRIVER_STREAM_CRYPTO:
		HxLOG_Print("\t=>VD_DRIVER_STREAM_CRYPTO\n");
		result[VD_DRIVER_STREAM_CRYPTO] = HUMAX_VD_CRYPTO_IO(wRefId, wOpCode, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
		return result[VD_DRIVER_STREAM_CRYPTO];

	case VD_DRIVER_EXTERNAL_STORAGE:
		HxLOG_Print("\t=>VD_DRIVER_EXTERNAL_STORAGE\n");
#if defined(IRPLAT_SYS_PVR)
		result[VD_DRIVER_EXTERNAL_STORAGE] = HUMAX_VD_ExtStorage_IO(wRefId, wOpCode, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
#endif
		return result[VD_DRIVER_EXTERNAL_STORAGE];

	case VD_DRIVER_DVB_OOB:
		HxLOG_Print("\t @@@ =>VD_DRIVER_DVB_OOB(I)\n");
#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
		result[VD_DRIVER_DVB_OOB] = HUMAX_VD_DvbOob_IO(wRefId, wOpCode, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
		return result[VD_DRIVER_DVB_OOB];
#else //defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
		return IA_SUCCESS;
#endif //defined(CONFIG_OP_ZIGGO_AVN_CLIENT)


	default:
		HxLOG_Print("\t=>INVALID\n");
		break;
	}

	return IA_SUCCESS;
}

ia_result VD_Initialise( void )
{
	ia_result result;

	result = INT_VD_Initialise();
	if (result != IA_SUCCESS)
	{
		HxLOG_Print("[VD_Initialise] Error : %d\n", result);
	}

	return result;
}

void VD_Terminate( void )
{
	INT_VD_Terminate();
}

ia_word16 VD_GetDriverVersion( vd_driver_type_e eDriverType )
{
	ia_word16 version;

	version = INT_VD_GetDriverVersion(eDriverType);

	return version;
}

ia_result VD_GetResources( vd_driver_type_e eDriverType, void **ppvList, ia_word16 *pwCount )
{
	ia_result result;

	result = INT_VD_GetResources(eDriverType, ppvList, pwCount);
	if (result != IA_SUCCESS)
	{
		HxLOG_Print("[VD_GetResources] Error : %d, eDriverType[%d], pwCount[%d]\n", result, (HUINT32)eDriverType, *pwCount);
	}

	return result;
}

vd_ref_id VD_Open( vd_driver_type_e eDriverType, vd_rsc_id wResourceID, void *pvData )
{
	vd_ref_id ref_id;

	ref_id = INT_VD_Open(eDriverType, wResourceID, pvData);
	if (ref_id == VD_REF_ID_ERROR)
	{
		HxLOG_Print(" %d, eDriverType[%d], wResourceID[%d]\n", (HUINT32)ref_id, (HUINT32)eDriverType, (HUINT32)wResourceID);
	}

	return ref_id;
}

ia_result VD_Close( vd_driver_type_e eDriverType, vd_ref_id wRefId )
{
	ia_result result;

	result = INT_VD_Close(eDriverType, wRefId);
	if (result != IA_SUCCESS)
	{
		HxLOG_Print("[VD_Close] Error : %d, eDriverType[%d], wRefId[%d]\n", result, eDriverType, (HUINT32)wRefId);
	}

	return result;
}

ia_result VD_IO( vd_driver_type_e eDriverType, vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	ia_result result;
//	HxLOG_Print(" VD_IO : OpCode : %d\n", wOpCode);
	result = INT_VD_IO(eDriverType, wRefId, wOpCode, wDataWriteSize, pvDataWrite, pwDataReadSize, pvDataRead);
	if (result != IA_SUCCESS)
	{
		HxLOG_Print("[VD_IO] Error : %d, eDriverType[%d], wRefId[%d], wOpCode[%d], wDataWriteSize[%d]\n",
			result, eDriverType, (HUINT32)wRefId, wOpCode, (HUINT32)wDataWriteSize);

		//HxLOG_Print("[VD_IO] Error : %d, eDriverType[%d], wRefId[%d], wOpCode[%d], wDataWriteSize[%d], pwDataReadSize[%d]\n",
			//result, eDriverType, (HUINT32)wRefId, wOpCode, (HUINT32)wDataWriteSize, (HUINT32)*pwDataReadSize);
	}

	return result;
}


#if !defined(CONFIG_MW_CAS_IRDETO_UCAS)
//*********************************************************/
/* Followings are for using SoftCell Debug Library */
/*********************************************************/

#if defined(CONFIG_DEBUG) && defined(USING_S3_DBGLIB_PRINT)
static HBOOL Switch_S3DebugPrint = FALSE; /* for register in debug build */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <ctype.h>
#ifndef WIN32
#include <termio.h>
#endif

int DBG_DRV_Printf(const char *format, ...)
{
	va_list args;
	unsigned char _SoftCellDebugBuf[2048]={0};

	if(Switch_S3DebugPrint  == 0 )
	{
		return 0;
	}

	va_start(args, format);

	vsprintf(_SoftCellDebugBuf, format, args);

	va_end(args);

	HxLOG_Print("[S3DBG_START]\n%s\n[S3DBG_END]\n",  _SoftCellDebugBuf);
	return 0;
}
#else //defined(CONFIG_DEBUG) && defined(USING_S3_DBGLIB_PRINT)
int DBG_DRV_Printf(const char *format, ...){return 0;}
#endif

#endif // }}} #if !defined(CONFIG_MW_CAS_IRDETO_UCAS)

