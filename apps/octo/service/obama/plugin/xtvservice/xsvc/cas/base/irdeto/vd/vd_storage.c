
// TODO: NVRAM Management.

/********************************************************************
*	Including Headers
********************************************************************/
#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"

#include "ir_nvr.h"
#include "ir_svc.h"
#include "ir_main.h"

#include "s3_vdps.h"

//#include "mxi_main.h"
/********************************************************************
*	Definitions (for this source file)
********************************************************************/







/********************************************************************
*	External variables
********************************************************************/

/********************************************************************
*	External function prototypes
********************************************************************/

/********************************************************************
*	Global variables
********************************************************************/

// Smartcard slot이 늘어나면 Open을 여러번 하고 reference ID로 이를 관리하도록... (smartcard하나당 512 byte의 memory가 필요)
// 실제 smartcard slot이 늘어나더라도 Open을 여러번 하는 식으로 동작하는 것이 아니라,
// Open은 한번만 하되 size가 512에서 1024로 늘어나는 식으로 동작할 것이라고 하지만..
// 그건 그 때 생각하기로 하고 우선은 이렇게 구현하자.
HUMAX_VD_PersistentStorage_st Humax_VD_PersistentStorage[HUMAX_VD_SC_SLOT_COUNT];


/********************************************************************
*	Static variables
********************************************************************/
static unsigned long		s_ulPersistentStorageAPISem;

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
/********************************************************************
*	function name :	 HUMAX_VD_PersistentStorage_Initialise
*	input : void(none)
*	output : result
*	description : External storage관련하여 생성된 file들에 대한 정보를 가지고 있는 Humax_VD_PersistentStorage라는 변수를
*			  HDD에서 읽어와서 copy해둠.
********************************************************************/
ia_result HUMAX_VD_PersistentStorage_Initialise(void)
{
	HUINT32						i;
	ia_result					result = IA_SUCCESS;

	HxLOG_Info("\n");

	if( VK_OK != VK_SEM_Create(&s_ulPersistentStorageAPISem, "s_ulPersistentStorageAPISem", VK_SUSPENDTYPE_FIFO)  )
	{
		HxLOG_Error("\t\t[VD_PersistentStorage] Init - Semaphore Create Error\n");
		return IA_FAIL;
	}

	// Init하고.
	HxSTD_memset(Humax_VD_PersistentStorage, 0, sizeof(HUMAX_VD_PersistentStorage_st) * HUMAX_VD_SC_SLOT_COUNT);
	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		Humax_VD_PersistentStorage[i].resourceID = VD_RSC_ID_INVALID;
		Humax_VD_PersistentStorage[i].referenceID = VD_REF_ID_ERROR;
	}

	return result;
}


/********************************************************************
*	function name :	 HUMAX_VD_PersistentStorage_GetResources
*	input : ..
*	output : Available resource count/list & result.
*	description : VD_PersistentStorage의 get resource는 사용 안됨 - 그러므로 resource ID는 0으로 고정일 것으로 생각.
********************************************************************/
HUINT32 HUMAX_VD_PersistentStorage_GetResources(void **ppvList, ia_word16 *pwCount )
{
	HxLOG_Info("\n");
	return HIR_OK;
}

/********************************************************************
*	function name :	 HUMAX_VD_PersistentStorage_Open
*	input : wResourceID / callback
*		   GetResource를 사용안하므로 resource ID는 항상 0일 것인가...
*	output : reference ID
*		   Open이 불리울 때 마다 file을 하나 생성하고 reference ID를 다르게 할당.
*	description : pvData를 통해서 callback function이 오게 되고, 이 callback function을 통해서
*			   smartcard in/out/error 등의 event를 softcell에 알려준다.
********************************************************************/
vd_ref_id HUMAX_VD_PersistentStorage_Open( vd_rsc_id wResourceID, void *pvData )
{
	HUINT32						i;
	//static vd_ref_id				referenceID = 0;
	vd_ps_open_st				*ps_open_st;

	HxLOG_Info("wResourceID=0x%08X\n", (HUINT32)wResourceID);

	VK_SEM_Get(s_ulPersistentStorageAPISem);

	ps_open_st = (vd_ps_open_st *)pvData;

	// 빈 reference ID를 하나 선택.
	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		if( Humax_VD_PersistentStorage[i].referenceID == VD_REF_ID_ERROR )
			break;
	}

	if( i == HUMAX_VD_SC_SLOT_COUNT )
	{
		HxLOG_Error("\t[VD_PersistentStorage] Open - ERROR no more empty reference ID space\n");
		VK_SEM_Release(s_ulPersistentStorageAPISem);
		return VD_REF_ID_ERROR;
	}

	// 아래 값들은 power를 off/on할 때마다 바뀔 수 있다.
	Humax_VD_PersistentStorage[i].ps_open_st.pfnErrorNotify = ps_open_st->pfnErrorNotify;
	Humax_VD_PersistentStorage[i].ps_open_st.wSoftCellVal = ps_open_st->wSoftCellVal;

	// resource ID와 reference ID는 init할 때 clear해주므로 새로 적어주자.
	Humax_VD_PersistentStorage[i].resourceID = wResourceID;
	Humax_VD_PersistentStorage[i].referenceID = i;

	VK_SEM_Release(s_ulPersistentStorageAPISem);
	return i;
}

/********************************************************************
*	function name :	 HUMAX_VD_PersistentStorage_Close
*	input : wRefId
*	output : result
*	description :
********************************************************************/
ia_result HUMAX_VD_PersistentStorage_Close( vd_ref_id wRefId )
{
	HxLOG_Info("wRefId=0x%08X\n", (HUINT32)wRefId);
	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 HUMAX_VD_PersistentStorage_IO
*	input : wRefId
*	output : result
*	description : Open할 때 등록된 callback함수를 close.
********************************************************************/
ia_result HUMAX_VD_PersistentStorage_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	HUMAX_ps_write_st 		received_ps_write_st;
	vd_es_read_st 			*received_ps_read_st;
	HUINT32					i;
	ia_result				result = IA_SUCCESS;
	HUINT8					*pWrite = NULL;

	HxLOG_Info("wRefId=0x%08X, wOpCode= %d\n", (HUINT32)wRefId, wOpCode);

	VK_SEM_Get(s_ulPersistentStorageAPISem);

	switch(wOpCode)
	{
	case VD_PS_WRITE:
		if( pwDataReadSize != NULL )
			HxLOG_Print("[VD_PersistentStorage] Write - wRefId(0x%X), wOpCode(0x%X), wDataWriteSize(0x%X), pwDataReadSize(0x%X)\n", (HUINT32)wRefId, wOpCode, (HUINT32)wDataWriteSize, (HUINT32)*pwDataReadSize);
		else
			HxLOG_Print("[VD_PersistentStorage] Write - wRefId(0x%X), wOpCode(0x%X), wDataWriteSize(0x%X)\n", (HUINT32)wRefId, wOpCode, (HUINT32)wDataWriteSize);
		HxSTD_memcpy(&received_ps_write_st, (HUMAX_ps_write_st *)pvDataWrite, sizeof(HUMAX_ps_write_st));
		pWrite = (HUINT8 *)pvDataWrite;
		received_ps_write_st.abDataToWrite = (HUINT8 *)(pWrite + sizeof(ia_word32) * 2);
		HxLOG_Print("\t[VD_PersistentStorage] Write - wStart = 0x%X, wLength = 0x%X, @ = 0x%X]\n", (HUINT32)received_ps_write_st.ps_write_st.wStart, (HUINT32)received_ps_write_st.ps_write_st.wLength, *received_ps_write_st.abDataToWrite);
#if 0 //for debug.
		{
			HUINT32 i;

			DDI_UART_Print("HONG - WRITE2 : length = %d\n", received_ps_write_st.ps_write_st.wLength);
			for( i = 0; i < received_ps_write_st.ps_write_st.wLength; i++ )
			{
				DDI_UART_Print("%02X ", received_ps_write_st.abDataToWrite[i]);
				if( i % 16 == 15 )
					DDI_UART_Print("\n");
			}
			DDI_UART_Print("\n");
		}
#endif

		for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
		{
			if( Humax_VD_PersistentStorage[i].referenceID == wRefId )
				break;
		}

		if( i == HUMAX_VD_SC_SLOT_COUNT )
		{
			HxLOG_Error("\t[VD_PersistentStorage] Write - ERROR invalid wRefId\n");
			result = VD_WRONG_REF_ID;
			break;
		}

// TODO: Direct write 가능한지 test 필요.
#if 1
	{
		CAS_IR_BASE_MSG 	*pMsg;

		pMsg = (CAS_IR_BASE_MSG *)OxCAS_Malloc(sizeof(CAS_IR_BASE_MSG));
		if(pMsg == NULL)
		{
			VK_SEM_Release(s_ulPersistentStorageAPISem);
			return VD_OUT_OF_MEMORY;
		}
		HxSTD_memset(pMsg, 0, sizeof(CAS_IR_BASE_MSG));
		pMsg->msg.NVRAM.RefId = wRefId;
		pMsg->msg.NVRAM.pucData = (HUINT8 *)OxCAS_Malloc(received_ps_write_st.ps_write_st.wLength);

		if( NULL == pMsg->msg.NVRAM.pucData )
		{
			// TODO: flag up . assertion.
			// add to prevent from PREVENT Tools issue....
			if(pMsg != NULL)
			{
				OxCAS_Free(pMsg);
				pMsg = NULL;
			}
			VK_SEM_Release(s_ulPersistentStorageAPISem);
			return VD_OUT_OF_MEMORY;
		}

		HxSTD_memcpy((HUINT8 *)(pMsg->msg.NVRAM.pucData), received_ps_write_st.abDataToWrite, received_ps_write_st.ps_write_st.wLength);

		pMsg->msg.NVRAM.ulStart = received_ps_write_st.ps_write_st.wStart;
		pMsg->msg.NVRAM.ulLen = received_ps_write_st.ps_write_st.wLength;

		CAS_IR_SendMessage( eIR_MSG_CLASS_CAS, IR_CATASK_NVRAM_WRITE, (void *)pMsg);
	}

#else
		result = DI_EEPROM_Write(received_ps_write_st.ps_write_st.wStart, received_ps_write_st.abDataToWrite, received_ps_write_st.ps_write_st.wLength);
#endif
		break;

	case VD_PS_READ:
		if( pwDataReadSize != NULL )
			HxLOG_Print("[VD_PersistentStorage] Read - wRefId(0x%X), wOpCode(0x%X), wDataWriteSize(0x%X), pwDataReadSize(0x%X)\n", (HUINT32)wRefId, wOpCode, (HUINT32)wDataWriteSize, (HUINT32)*pwDataReadSize);
		else
			HxLOG_Print("[VD_PersistentStorage] Read - wRefId(0x%X), wOpCode(0x%X), wDataWriteSize(0x%X)\n", (HUINT32)wRefId, wOpCode, (HUINT32)wDataWriteSize);
		received_ps_read_st = (vd_es_read_st *)pvDataWrite;
		HxLOG_Print("\t[VD_PersistentStorage] Read - wStart = 0x%X, wLength = 0x%X\n", (HUINT32)received_ps_read_st->wStart, (HUINT32)received_ps_read_st->wLength);

		for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
		{
			if( Humax_VD_PersistentStorage[i].referenceID == wRefId )
				break;
		}

		if( i == HUMAX_VD_SC_SLOT_COUNT )
		{
			HxLOG_Error("\t[VD_PersistentStorage] Read - ERROR invalid wRefId\n");
			VK_SEM_Release(s_ulPersistentStorageAPISem);
			return VD_WRONG_REF_ID;
		}

		// 원래는 20ms안에 이 함수를 return해주기 위해서 실제 write/read동작은 다른 task에서 수행해야 하지만..
//		if(ERR_OK !=  MW_EEP_ExtZoneReadData(received_ps_read_st->wStart+IR_PARTITION_CA_OFFSET, received_ps_read_st->wLength, pvDataRead) )
//		if(ERR_OK !=  DI_EEPROM_Read(received_ps_read_st->wStart+IR_PARTITION_CA_OFFSET, pvDataRead, received_ps_read_st->wLength) )
		if(ERR_OK != CAS_IR_NVR_Partition_Read(IRNVR_PART_CA, received_ps_read_st->wStart, pvDataRead, received_ps_read_st->wLength ) )
		{
			HxLOG_Error("\t[VD_PersistentStorage] Read - ERROR\n");
			result = IA_FAIL;
		}

		if (pwDataReadSize)
			*pwDataReadSize = received_ps_read_st->wLength;

#if defined(IRPLAT_TEST_INCOMPLETED_LOADER)
		HxLOG_Print("[VD_PersistentStorage] Read - USE VIRTURAL LOADER \n");
		memset(pvDataRead, 0 , received_ps_read_st->wLength );
		result = IA_SUCCESS;
#endif

		break;

	case VD_PS_DELETE:
		HxLOG_Print("[VD_PersistentStorage] Delete - wRefId(0x%X), wOpCode(0x%X), wDataWriteSize(0x%X)\n", (HUINT32)wRefId, wOpCode, (HUINT32)wDataWriteSize);
		break;
	}

	VK_SEM_Release(s_ulPersistentStorageAPISem);
	return result;

}

HUINT32 HUMAX_VD_PersistentStorage_DelayWrite(vd_ref_id wRefId, HUINT16 usStart, HUINT16 usLen, HUINT8 *pucData)
{
	HUINT32	i;
	HERROR	res;

	VK_SEM_Get(s_ulPersistentStorageAPISem);

	HxLOG_Info("wRefId=0x%08X \n", (HUINT32)wRefId);

	HxLOG_Print("[VD_PersistentStorage] DelayWrite - wRefId(0x%X), usStart(0x%X), usLen(0x%X)\n", (HUINT32)wRefId, usStart, usLen);
	for( i = 0; i < HUMAX_VD_SC_SLOT_COUNT; i++ )
	{
		if( Humax_VD_PersistentStorage[i].referenceID == wRefId )
			break;
	}

	if( i == HUMAX_VD_SC_SLOT_COUNT )
	{
		HxLOG_Error("\t[VD_PersistentStorage] Delay Write - ERROR invalid wRefId (%d)\n", (HUINT32)wRefId);
		VK_SEM_Release(s_ulPersistentStorageAPISem);
		return 1;
	}

	//set output value
	res = HIR_OK;
	if( (pucData != NULL) && (usLen != 0) )
	{
		res = CAS_IR_NVR_Partition_Write(IRNVR_PART_CA, usStart, pucData, usLen );
	}

	if( HIR_OK == res )
	{
		HxLOG_Print("\t[VD_PersistentStorage] Delay Write - SUCCESS (callback Func address = 0x%X)\n", (HUINT32)Humax_VD_PersistentStorage[i].ps_open_st.pfnErrorNotify);
		if (Humax_VD_PersistentStorage[i].ps_open_st.pfnErrorNotify != NULL)
		{
			(*(Humax_VD_PersistentStorage[i].ps_open_st.pfnErrorNotify))\
			(wRefId, Humax_VD_PersistentStorage[i].ps_open_st.wSoftCellVal, IA_SUCCESS);
		}
	}
	else
	{
		HxLOG_Print("\t[VD_PersistentStorage] Delay Write - FAIL (callback Func address = 0x%X)\n", (HUINT32)Humax_VD_PersistentStorage[i].ps_open_st.pfnErrorNotify);
		if (Humax_VD_PersistentStorage[i].ps_open_st.pfnErrorNotify != NULL)
		{
			(*(Humax_VD_PersistentStorage[i].ps_open_st.pfnErrorNotify))\
			(wRefId, Humax_VD_PersistentStorage[i].ps_open_st.wSoftCellVal, IA_FAIL);
		}
	}

	VK_SEM_Release(s_ulPersistentStorageAPISem);

	return ERR_OK;

}

/* end of file */

