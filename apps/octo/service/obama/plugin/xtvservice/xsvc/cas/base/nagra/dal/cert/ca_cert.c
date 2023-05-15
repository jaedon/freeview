/**************************************************************************************************/
#define ________CA_CERT_Private_Include_____________________________________________________
/**************************************************************************************************/
#include "na_glob.h"

#include <_svc_cas_mgr_common.h>


/**************************************************************************************************/
#define ________CA_CERT_Golbal_Value________________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define ________CA_RHP_Private_Define_______________________________________________________
/**************************************************************************************************/






/**************************************************************************************************/
#define ________CA_CERT_Private_Type_Define_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CERT_Private_Static_Value_________________________________________________

#if defined(CONFIG_PROD_HGS1000S)
/* 
     nocs3 certGetFunctionTable()을 사용하도록 변경되어 우선  _TEMP_NA_NOCS_OVERRIDE_FUNCTION을 사용해서 nocs3 certGetFunctionTable()을 사용하도록 수정함.
     추후 ca_sec도 secGetFunctionTable()을 사용해서 사용하도록 수정한후 _TEMP_NA_NOCS_OVERRIDE_FUNCTION를 제거한후 
     _NA_NOCS_OVERRIDE_FUNCTION로 묶어서  사용하도록 하자
*/
#define _TEMP_NA_NOCS_OVERRIDE_FUNCTION
#endif

/**************************************************************************************************/
#if !defined(_NA_NOCS_OVERRIDE_FUNCTION)  && !defined(_TEMP_NA_NOCS_OVERRIDE_FUNCTION)
static TCertFunctionTable	s_stCertFunctionTable;
#endif
static HULONG				s_ulCertSemId;
static TCertResourceHandle	s_pxResourceHandle;

/**************************************************************************************************/
#define ________CA_CERT_Private_Static_Prototype______________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CERT_Private_Static_Prototype_Body_________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CERT_Externel_Functions_Prototype_______________________________________________
/**************************************************************************************************/
extern TCertStatus certLock(TCertResourceHandle *pxResourceHandle);
extern TCertStatus certUnlock(TCertResourceHandle xResourceHandle);
extern TCertStatus certExchange(TCertResourceHandle xResourceHandle, size_t xNumOfCommands, const TCertCommand* pxCommands, size_t* pxNumOfProcessedCommands);


/**************************************************************************************************/
#define ________CA_CERT_Public_Functions_Body_______________________________________________
/**************************************************************************************************/
STATIC TCertStatus _caCert_Lock(TCertResourceHandle *pxResourceHandle)
{
	TCertStatus status;

	if(pxResourceHandle == NULL)
		return CERT_ERROR;

	VK_SEM_Get(s_ulCertSemId);
	status = certLock(pxResourceHandle);

	if(status == CERT_NO_ERROR)
		s_pxResourceHandle = *pxResourceHandle;
	else
		VK_SEM_Release(s_ulCertSemId);

	return status;
}

STATIC TCertStatus _caCert_Unlock(TCertResourceHandle xResourceHandle)
{
	TCertStatus status;

	if(xResourceHandle == NULL || s_pxResourceHandle == NULL || xResourceHandle != s_pxResourceHandle)
		return CERT_ERROR_BAD_HANDLE;

	status = certUnlock(xResourceHandle);
	if(status == CERT_NO_ERROR)
	{
		s_pxResourceHandle = NULL;
		VK_SEM_Release(s_ulCertSemId);
	}

	return status;
}

STATIC TCertStatus _caCert_Exchange(TCertResourceHandle xResourceHandle, size_t xNumOfCommands, const TCertCommand* pxCommands, size_t* pxNumOfProcessedCommands)
{
	TCertStatus status;

	if( (xResourceHandle == NULL) || (xResourceHandle != s_pxResourceHandle) )
		return CERT_ERROR_BAD_HANDLE;

	if( (xNumOfCommands == 0) || (pxCommands == NULL) || (pxNumOfProcessedCommands == NULL) )
		return CERT_ERROR;

	status = certExchange(xResourceHandle, xNumOfCommands, pxCommands, pxNumOfProcessedCommands);

	return status;
}

#if defined(_NA_NOCS_OVERRIDE_FUNCTION)  || defined(_TEMP_NA_NOCS_OVERRIDE_FUNCTION)
STATIC HERROR local_na_cert_InitFunctionTable(void)
{
	TCertFunctionTable* pCertFunctionTable = NULL;
	pCertFunctionTable = certGetFunctionTable();

	pCertFunctionTable->certApiVersion = CERTAPI_VERSION_INT;
	/**< Version of the CERT API. Use the macro CERTAPI_VERSION_INT to assign the
	*   the right value.
	*/

	pCertFunctionTable->certLock = _caCert_Lock;
	/**< Function to reserve the CERT resource for exclusive use
	*/
	pCertFunctionTable->certUnlock = _caCert_Unlock;
	/**< Function to release the CERT resource
	*/
	pCertFunctionTable->certExchange = _caCert_Exchange;
	/**< Function to perform transactions with the CERT block
	*/
	
	return ERR_OK;
}
#else
	
STATIC HERROR local_na_cert_InitFunctionTable(void)
{
	HxSTD_memset(&s_stCertFunctionTable, 0, sizeof(TCertFunctionTable));

	s_stCertFunctionTable.certApiVersion = CERTAPI_VERSION_INT;
	/**< Version of the CERT API. Use the macro CERTAPI_VERSION_INT to assign the
	*   the right value.
	*/

	s_stCertFunctionTable.certLock = _caCert_Lock;
	/**< Function to reserve the CERT resource for exclusive use
	*/
	s_stCertFunctionTable.certUnlock = _caCert_Unlock;
	/**< Function to release the CERT resource
	*/
	s_stCertFunctionTable.certExchange = _caCert_Exchange;
	/**< Function to perform transactions with the CERT block
	*/
	s_stCertFunctionTable.certUseEncryptionKey;
	/**< @deprecated
	*/
	s_stCertFunctionTable.certUseDecryptionKey;
	/**< @deprecated
	*/

	return ERR_OK;
}

TCertFunctionTable* certGetFunctionTable(void)
{
	return &s_stCertFunctionTable;
}
#endif

HERROR CAS_NA_CertInitialize(void)
{
	if(local_na_cert_InitFunctionTable() != ERR_OK)
	{
		HxLOG_Error("Error in local_na_cert_InitFunctionTable()");
		return ERR_FAIL;
	}

	if(VK_SEM_Create(&s_ulCertSemId, "CaCertSem", VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		HxLOG_Error("Error in VK_SEM_Create()");
		return ERR_FAIL;
	}

	s_pxResourceHandle = NULL;

	return ERR_OK;
}

