
#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"

#include "ir_platform.h"
#include "ir_svc.h"
#include "ir_main.h"

#if defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
/********************************************************************
*	Definitions (for this source file)
********************************************************************/

#ifdef CONFIG_DEBUG
HUINT32 VDDvbOobDebugLevel = DBG_ASSERT|DBG_TRACE;//|DBG_PRINT );
#endif

#ifdef CONFIG_DEBUG
#define VDDvbOobPrint(level, message)		(((level) & VDDvbOobDebugLevel) ? ((VK_Print("\t[VDDvbOob][%d][%s], ", __LINE__, __FUNCTION__)), (VK_Print message )) : 0)
#define VDDvbOobError(level, message)		(((level) & VDDvbOobDebugLevel) ? ((VK_Print("\t[VDDvbOob][%d][%s] ERROR, ", __LINE__, __FUNCTION__)), (VK_Print message)) : 0)
#define VDDvbOobAssert(c) 				((c) ? 0 : (VK_Print("\n VDDvbOob assertion on %s: line%d\n\n", __FILE__, __LINE__)))
#define VDDvbOobData(level, message)		(((level) & VDDvbOobDebugLevel) ? ( (VK_Print message)) : 0)
#else
#define VDDvbOobPrint(level, message)
#define VDDvbOobError(level, message)
#define VDDvbOobAssert(c)
#define VDDvbOobData(level, message)
#endif

/********************************************************************
*	External variables
********************************************************************/

/********************************************************************
*	External function prototypes
********************************************************************/

/********************************************************************
*	Global variables
********************************************************************/
HUMAX_VD_DVB_OOB_st Humax_VD_DvbOob;


/********************************************************************
*	Static variables
********************************************************************/
HUINT32	s_ulDvbOobAPISem;

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/


/********************************************************************
*	function name :	 HUMAX_VD_DvbOob_Initialise
*	input : void(none)
*	output : result
*	description : External storage관련하여 생성된 file들에 대한 정보를 가지고 있는 Humax_VD_DvbOob라는 변수를
*			  HDD에서 읽어와서 copy해둠.
********************************************************************/
ia_result HUMAX_VD_DvbOob_Initialise(void)
{
	ia_result					result = IA_SUCCESS;

	VDDvbOobPrint(DBG_FUNC, ("-->>\n") );

	if( VK_SEM_Create((unsigned long*)&s_ulDvbOobAPISem, "s_ulDvbOobAPISem", VK_SUSPENDTYPE_FIFO ) != VK_OK )
	{
		VDDvbOobPrint(DBG_PRINT,("\t Init - Semaphore Create Error\n"));
		return IA_FAIL;
	}

	HxSTD_memset(&Humax_VD_DvbOob, 0, sizeof(HUMAX_VD_DVB_OOB_st));
	Humax_VD_DvbOob.referenceID = VD_REF_ID_ERROR;

	VDDvbOobPrint(DBG_FUNC, ("<<--\n") );
	return result;
}

#define PVR_HIDDEN_CRID_DIR_NAME					".crid"

/********************************************************************
*	function name :	 HUMAX_VD_DvbOob_Open
*	input : wResourceID / callback
*		   GetResource를 사용안하므로 resource ID는 항상 0일 것인가...
*	output : reference ID
*		   Open이 불리울 때 마다 file을 하나 생성하고 reference ID를 다르게 할당.
-->resourceID에 해당하는 파일을 open.
*	description : pvData를 통해서 callback function이 오게 되고, 이 callback function을 통해서
*			   smartcard in/out/error 등의 event를 softcell에 알려준다.
********************************************************************/
vd_ref_id HUMAX_VD_DvbOob_Open( vd_rsc_id wResourceID, void *pvData )
{
	vd_ref_id					referenceID = 0;
	vd_dvb_oob_open_st			*do_open_st = NULL;

	VDDvbOobPrint(DBG_FUNC, ("Resource ID=-0x%X-->\n", (HUINT32)wResourceID) );

	VK_SEM_Get(s_ulDvbOobAPISem);

	if(Humax_VD_DvbOob.referenceID == VD_REF_ID_ERROR)
	{
		do_open_st = (vd_dvb_oob_open_st *)pvData;

		VDDvbOobPrint(DBG_PRINT,("\t wSoftcellVal=0x%X\n", (HUINT32)do_open_st->wSoftCellVal));
		VDDvbOobPrint(DBG_PRINT,("\t pfnNotify=0x%X\n", (HUINT32)do_open_st->pfnNotify));

		Humax_VD_DvbOob.referenceID = referenceID;
		Humax_VD_DvbOob.do_open_st.wSoftCellVal = do_open_st->wSoftCellVal;
		Humax_VD_DvbOob.do_open_st.pfnNotify = do_open_st->pfnNotify;
	}
	else
	{
		VDDvbOobPrint(DBG_ASSERT,("\n\t @@@ DVB OOB channel is already opened..!!\n\n"));
	}

	VK_SEM_Release(s_ulDvbOobAPISem);
	VDDvbOobPrint(DBG_FUNC, ("<<--\n") );
	return referenceID;
}

/********************************************************************
*	function name :	 HUMAX_VD_DvbOob_Close
*	input : wRefId
*	output : result
*	description :
********************************************************************/
ia_result HUMAX_VD_DvbOob_Close( vd_ref_id wRefId )
{
	HUINT16		i;
	VDDvbOobPrint(DBG_FUNC, ("(-->>\n") );

	if (wRefId != Humax_VD_DvbOob.referenceID)
	{
		VDDvbOobPrint(DBG_PRINT,("[HUMAX_VD_DvbOob_Close] error wRefId(%d)\n", (HUINT32)wRefId));
		return IA_FAIL;
	}

	VK_SEM_Get(s_ulDvbOobAPISem);

	for(i = 0; i < MAX_DVB_OOB_SECTION; i++)
	{
		if(Humax_VD_DvbOob.pSecData[i] != NULL)
		{
			VK_MEM_Free(Humax_VD_DvbOob.pSecData[i]);
			Humax_VD_DvbOob.pSecData[i] = NULL;
		}
	}

	HxSTD_memset(&Humax_VD_DvbOob, 0, sizeof(HUMAX_VD_DVB_OOB_st));
	Humax_VD_DvbOob.referenceID = VD_REF_ID_ERROR;

	VDDvbOobPrint(DBG_FUNC, ("<<--\n") );
	VK_SEM_Release(s_ulDvbOobAPISem);

	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 HUMAX_VD_DvbOob_IO
*	input : wRefId
*	output : result
*	description : Open할 때 등록된 callback함수를 close.
********************************************************************/
//extern	HUINT32 HUMAX_VD_DvbOob_DelayWrite(vd_ref_id wRefId, ia_word32 ulStart, ia_word16 ulLen, void *pucData);

ia_result HUMAX_VD_DvbOob_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	vd_dvb_oob_status_st	*pStatus = NULL;
	ia_result				result = IA_SUCCESS;
#if defined(CONFIG_DEBUG)
	/* for debugging */
	HUINT32 i;
#endif

	VDDvbOobPrint(DBG_FUNC, ("-->>\n") );

	if (wRefId != Humax_VD_DvbOob.referenceID)
	{
		VDDvbOobPrint(DBG_PRINT,("[HUMAX_VD_DvbOob_IO] error wRefId(%d)\n", (HUINT32)wRefId));
		return IA_FAIL;
	}

	VK_SEM_Get(s_ulDvbOobAPISem);

	switch(wOpCode)
	{
		case VD_DVB_OOB_READ_SECTTION:
			VDDvbOobPrint(DBG_TRACE,(" READ_SEC - wOpCode(0x%X), pwDataReadSize(0x%X), pvDataRead(0x%X)\n", wOpCode, (HUINT32)*pwDataReadSize, (HUINT32)pvDataRead));

			if(Humax_VD_DvbOob.usSecSize[Humax_VD_DvbOob.usHead] > *pwDataReadSize)
			{
				VDDvbOobPrint(DBG_ASSERT, ("\n @@@ DVB OOB Read-Buffer is full..!!\n"));
				*pwDataReadSize = Humax_VD_DvbOob.usSecSize[Humax_VD_DvbOob.usHead];
				pvDataRead = 0;

				result = IA_FULL;
			}
			else
			{
				*pwDataReadSize = Humax_VD_DvbOob.usSecSize[Humax_VD_DvbOob.usHead];
				if( (Humax_VD_DvbOob.usSecSize[Humax_VD_DvbOob.usHead] != 0) && (Humax_VD_DvbOob.pSecData[Humax_VD_DvbOob.usHead] != NULL) )
				{
					HxSTD_memcpy(pvDataRead, Humax_VD_DvbOob.pSecData[Humax_VD_DvbOob.usHead], Humax_VD_DvbOob.usSecSize[Humax_VD_DvbOob.usHead]);

#if defined(CONFIG_DEBUG)
					/* for debugging */
					VK_Print("\n\n VD_DVB_OOB_READ_SECTTION EMM Data[%d] : \n", (HUINT32)Humax_VD_DvbOob.usSecSize[Humax_VD_DvbOob.usHead]);
					for(i =0; i < Humax_VD_DvbOob.usSecSize[Humax_VD_DvbOob.usHead]; i++)
					{
						VK_Print(" %02X", ((HUINT8*)pvDataRead)[i]);
					}
					VK_Print("\n\n");
#endif
				}
			}

			/* Discard section data */
			if(Humax_VD_DvbOob.pSecData[Humax_VD_DvbOob.usHead] != NULL)
			{
				VDDvbOobPrint(DBG_TRACE, (" READ_SEC-(head:tail) == (%d, %d)\n", Humax_VD_DvbOob.usHead, Humax_VD_DvbOob.usTail) );
				VK_MEM_Free(Humax_VD_DvbOob.pSecData[Humax_VD_DvbOob.usHead]);
				Humax_VD_DvbOob.pSecData[Humax_VD_DvbOob.usHead] = NULL;
				Humax_VD_DvbOob.usSecSize[Humax_VD_DvbOob.usHead] = 0;

				Humax_VD_DvbOob.usHead++;
				Humax_VD_DvbOob.usHead %= MAX_DVB_OOB_SECTION;
			}
			break;

		case VD_DVB_OOB_SET_STATUS:
			VDDvbOobPrint(DBG_TRACE, ("SET_STATUS\n"));

			pStatus = (vd_dvb_oob_status_st*)pvDataWrite;

			switch(pStatus->bOobStatus)
			{
				case OOB_STATUS_SC_EMM:
					VDDvbOobPrint(DBG_TRACE, ("\t\t OOB_STATUS_SC_EMM\n"));
					break;
				case OOB_STATUS_IRD_EMM:
					VDDvbOobPrint(DBG_TRACE, ("\t\t OOB_STATUS_IRD_EMM\n"));
					break;
				case OOB_STATUS_CAM_EMM:
					VDDvbOobPrint(DBG_TRACE, ("\t\t OOB_STATUS_CAM_EMM\n"));
					break;
				case OOB_STATUS_Format_error:
					VDDvbOobPrint(DBG_TRACE, ("\t\t OOB_STATUS_Format_error\n"));
					break;
				case OOB_STATUS_Address_error:
					VDDvbOobPrint(DBG_TRACE, ("\t\t OOB_STATUS_Address_error\n"));
					break;
				case OOB_STATUS_SoftCell_error:
					VDDvbOobPrint(DBG_TRACE, ("\t\t OOB_STATUS_SoftCell_error\n"));
					break;
				case OOB_STATUS_No_SC:
					VDDvbOobPrint(DBG_TRACE, ("\t\t OOB_STATUS_No_SC\n"));
					break;
				default:
					VDDvbOobPrint(DBG_ASSERT, ("\n Unkown OOB Status(0x%X)..!!\n", pStatus->bOobStatus));
					break;
			}

			if(pStatus->bScStatus == 0)
			{
				VDDvbOobPrint(DBG_TRACE, ("\n t EMM was accepted by SC!!\n"));
			}
			else
			{
				VDDvbOobPrint(DBG_TRACE, ("\n t EMM was rejected by SC!!\n"));
			}

			if( (Humax_VD_DvbOob.usSecSize[Humax_VD_DvbOob.usHead] != 0) && (Humax_VD_DvbOob.pSecData[Humax_VD_DvbOob.usHead] != NULL) )
			{
				VDDvbOobPrint(DBG_TRACE, (" Humax_VD_DvbOob.do_open_st.pfnNotify....(%d:%d)\n", Humax_VD_DvbOob.usHead, Humax_VD_DvbOob.usTail) );
				Humax_VD_DvbOob.do_open_st.pfnNotify(Humax_VD_DvbOob.do_open_st.wSoftCellVal);
			}
			break;

		default:
			VDDvbOobPrint(DBG_ASSERT, ("\n Unkown OpCode(0x%X)..!!\n", wOpCode));
			result = VD_NOT_SUPPORTED;
			break;
	}

	VK_SEM_Release(s_ulDvbOobAPISem);
	VDDvbOobPrint(DBG_FUNC, ("<--\n") );
	return result;
}


void HUMAX_VD_DvbOob_SetEMMData(HUINT32 ulSecSize, void *pSecData)
{
#if 0//defined(CONFIG_DEBUG)
	/* for debugging */
	HUINT32 i;
#endif

	VDDvbOobPrint(DBG_FUNC, ("-->>\n") );

	VDDvbOobPrint(DBG_TRACE, ("(head:tail) == (%d, %d)\n", Humax_VD_DvbOob.usHead, Humax_VD_DvbOob.usTail) );
	if( (Humax_VD_DvbOob.usHead == Humax_VD_DvbOob.usTail)
		 && (Humax_VD_DvbOob.pSecData[Humax_VD_DvbOob.usTail] != NULL) )// Queue Full
	{
		VDDvbOobPrint(DBG_ASSERT, ("\n @@@ EMM-Section Queue is full..!!\n"));
		VDDvbOobPrint(DBG_FUNC, ("<--\n") );
		return;
	}

	Humax_VD_DvbOob.pSecData[Humax_VD_DvbOob.usTail] = VK_MEM_Alloc(ulSecSize);
	if(Humax_VD_DvbOob.pSecData[Humax_VD_DvbOob.usTail] == NULL)
	{
		VDDvbOobPrint(DBG_ASSERT, ("\n @@@ MemAlloc is fail..!!\n"));
		return;
	}

	Humax_VD_DvbOob.usSecSize[Humax_VD_DvbOob.usTail] = (HUINT16)ulSecSize;
	HxSTD_memcpy(Humax_VD_DvbOob.pSecData[Humax_VD_DvbOob.usTail], pSecData, ulSecSize);

#if 0//defined(CONFIG_DEBUG)
	/* for debugging */
	VK_Print("\n\n Set EMM Data[%d] : \n", ulSecSize);
	for(i =0; i < ulSecSize; i++)
	{
		VK_Print(" %02X", ((HUINT8*)Humax_VD_DvbOob.pSecData[Humax_VD_DvbOob.usTail])[i]);
	}
	VK_Print("\n\n");
#endif

	if(Humax_VD_DvbOob.usHead == Humax_VD_DvbOob.usTail) // Empty Queue
	{
		VDDvbOobPrint(DBG_TRACE, (" Humax_VD_DvbOob.do_open_st.pfnNotify....(%d)\n", Humax_VD_DvbOob.usHead) );
		/* Notify to SoftCell */
		Humax_VD_DvbOob.do_open_st.pfnNotify(Humax_VD_DvbOob.do_open_st.wSoftCellVal);
	}

	Humax_VD_DvbOob.usTail++;
	Humax_VD_DvbOob.usTail %= MAX_DVB_OOB_SECTION;

	VDDvbOobPrint(DBG_FUNC, ("<--\n") );
	return;
}

#endif //defined(CONFIG_OP_ZIGGO_AVN_CLIENT)
/* end of file */

