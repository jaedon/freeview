
/********************************************************************
*	Including Headers
********************************************************************/
#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "vd_humax.h"

#include "ir_platform.h"
#include "ir_svc.h"

#include "s3_vdtmr.h"

/********************************************************************
*	Definitions (for this source file)
********************************************************************/

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


/********************************************************************
*	Static variables
********************************************************************/
static HVD_TIMER_INFO_STRUCT s_stVd_Timer_Info;
static HULONG	s_ulTMRAPISem;

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
static void local_VdTimer_Callback(HUINT32 tid, void *parg);

/********************************************************************
*	function name :	 local_VdTimer_Callback
*	input : timer ID, parameter.
*	output : none
*	description :
********************************************************************/
static void local_VdTimer_Callback(HUINT32 tid, void *parg)
{
	HUINT32	i;

	for( i = 0; i < HMX_VD_TIMER_MAX_NUM; i++ )
	{
		if( s_stVd_Timer_Info.RefInfo[i].TimerID == tid )
		{
			if( s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.fRepetitive == IA_FALSE )
			{
				s_stVd_Timer_Info.RefInfo[i].Status = 0;
			}

			s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.pfnTimeoutNotify(s_stVd_Timer_Info.RefInfo[i].ReferenceID, s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.wSoftCellVal);
			return;
		}
	}
}

/********************************************************************
*	function name :	 HDV_TIMER_Initialise
*	input : void(None)
*	output : ia_result
*	description :
********************************************************************/
ia_result HDV_TIMER_Initialise(void)
{
	ia_result	result = IA_SUCCESS;
	HUINT32		res;
	HUINT32		i;

	HxSTD_memset(&s_stVd_Timer_Info, 0, sizeof(HVD_TIMER_INFO_STRUCT));
	s_stVd_Timer_Info.ResourceID = VD_RSC_ID_INVALID;
	for( i = 0; i < HMX_VD_TIMER_MAX_NUM; i++ )
	{
		s_stVd_Timer_Info.RefInfo[i].ReferenceID = VD_REF_ID_ERROR;
	}

	res = VK_SEM_Create(&s_ulTMRAPISem, "s_ulTMRAPISem", VK_SUSPENDTYPE_FIFO);
	if( res != VK_OK )
	{
		HxLOG_Print(" Semaphore Create Error\n");
		result = IA_FAIL;
	}

	return result;
}

/********************************************************************
*	function name :	 HDV_TIMER_Open
*	input : resource ID, timer parameter, callback function.
*	output : reference ID
*	description :
********************************************************************/
vd_ref_id HDV_TIMER_Open( vd_rsc_id wResourceID, void *pvData )
{
	HUINT32 i;
	vd_timer_open_st	*ptimer_open_st;

	VK_SEM_Get(s_ulTMRAPISem);
	if( s_stVd_Timer_Info.ResourceID == VD_RSC_ID_INVALID )
		s_stVd_Timer_Info.ResourceID = wResourceID;
	else
	{
		// SoftCell 3.2.14는 timer resource를 하나만 사용한다고 하였으므로..
		if( s_stVd_Timer_Info.ResourceID != wResourceID )
		{
			HxLOG_Print(" wrong resourceID\n");
			VK_SEM_Release(s_ulTMRAPISem);
			return VD_REF_ID_ERROR;
		}
	}

	for( i = 0; i < HMX_VD_TIMER_MAX_NUM; i++ )
	{
		if( s_stVd_Timer_Info.RefInfo[i].ReferenceID == VD_REF_ID_ERROR )
			break;
	}

	if( i == HMX_VD_TIMER_MAX_NUM )
	{
		HxLOG_Print(" no more reference ID space\n");
		VK_SEM_Release(s_ulTMRAPISem);
		return VD_REF_ID_ERROR;
	}

	ptimer_open_st = (vd_timer_open_st *)pvData;

	HxLOG_Print(" Open - wTimeOut(0x%X), fRepetitive(%d)\n", (HUINT32)ptimer_open_st->wTimeout, ptimer_open_st->fRepetitive);
	// ONE TIME.
	if( ptimer_open_st->fRepetitive == IA_FALSE )
	{
		if( VK_OK != VK_TIMER_EventAfter(ptimer_open_st->wTimeout,
								(void *)local_VdTimer_Callback,
								(void *)NULL,
								0,
								&(s_stVd_Timer_Info.RefInfo[i].TimerID)) )
		{
			HxLOG_Print(" Timer Once Start Error\n");
			VK_SEM_Release(s_ulTMRAPISem);
			return VD_REF_ID_ERROR;
		}
		else
		{
			s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.pfnTimeoutNotify = ptimer_open_st->pfnTimeoutNotify;
			s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.wSoftCellVal = ptimer_open_st->wSoftCellVal;
			s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.fRepetitive = ptimer_open_st->fRepetitive;
			s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.wTimeout = ptimer_open_st->wTimeout;
			s_stVd_Timer_Info.RefInfo[i].Status = 1;
		}
	}
	// CONTINUE
	else
	{
		if( VK_OK != VK_TIMER_EventEvery(ptimer_open_st->wTimeout,
								(void *)local_VdTimer_Callback,
								NULL,
								0,
								&(s_stVd_Timer_Info.RefInfo[i].TimerID)) )
		{
			HxLOG_Print(" Timer Every Start Error\n");
			VK_SEM_Release(s_ulTMRAPISem);
			return VD_REF_ID_ERROR;
		}
		else
		{
			s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.pfnTimeoutNotify = ptimer_open_st->pfnTimeoutNotify;
			s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.wSoftCellVal = ptimer_open_st->wSoftCellVal;
			s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.fRepetitive = ptimer_open_st->fRepetitive;
			s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.wTimeout = ptimer_open_st->wTimeout;
			s_stVd_Timer_Info.RefInfo[i].Status = 1;
		}
	}

	s_stVd_Timer_Info.RefInfo[i].ReferenceID = i;

	VK_SEM_Release(s_ulTMRAPISem);
	return i;
}

/********************************************************************
*	function name :	 HDV_TIMER_Close
*	input : resource ID
*	output : result
*	description :
********************************************************************/
ia_result HDV_TIMER_Close( vd_ref_id wReferenceID )
{
	HUINT32		i;

	VK_SEM_Get(s_ulTMRAPISem);
	for( i = 0; i < HMX_VD_TIMER_MAX_NUM; i++ )
	{
		if( s_stVd_Timer_Info.RefInfo[i].ReferenceID == wReferenceID )
		{
			if( s_stVd_Timer_Info.RefInfo[i].Status == 1 )
			{
				VK_TIMER_Cancel(s_stVd_Timer_Info.RefInfo[i].TimerID);
			}
			HxSTD_memset(&s_stVd_Timer_Info.RefInfo[i], 0, sizeof(HVD_TIMER_REF_INFO_STRUCT));
			s_stVd_Timer_Info.RefInfo[i].ReferenceID = VD_REF_ID_ERROR;
			break;
		}
	}

	if( i == HMX_VD_TIMER_MAX_NUM )
	{
		HxLOG_Print(" Close - invalid reference ID\n");
		VK_SEM_Release(s_ulTMRAPISem);
		return VD_WRONG_REF_ID;
	}

	VK_SEM_Release(s_ulTMRAPISem);
	return IA_SUCCESS;
}

/********************************************************************
*	function name :	 HDV_TIMER_IO
*	input : resource ID
*	output : result
*	description :
********************************************************************/
ia_result HDV_TIMER_IO( vd_ref_id wRefId, ia_word16 wOpCode, ia_word32 wDataWriteSize, void *pvDataWrite, ia_word32 *pwDataReadSize, void *pvDataRead )
{
	HUINT32		i;

	VK_SEM_Get(s_ulTMRAPISem);
	for( i = 0; i < HMX_VD_TIMER_MAX_NUM; i++ )
	{
		if( s_stVd_Timer_Info.RefInfo[i].ReferenceID == wRefId )
			break;
	}

	if( i == HMX_VD_TIMER_MAX_NUM )
	{
		HxLOG_Print(" IO - invalid reference ID\n");
		VK_SEM_Release(s_ulTMRAPISem);
		return VD_WRONG_REF_ID;
	}

	HxLOG_Print(": wRefId(0x%X), wOpCode(0x%X)\n", (HUINT32)wRefId, (HUINT32)wOpCode );
	switch(wOpCode)
	{
		case	VD_TIMER_START:
			if( s_stVd_Timer_Info.RefInfo[i].Status == 0 )
			{
				// ONE TIME.
				if( s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.fRepetitive == IA_FALSE )
				{
					if( VK_TIMER_EventAfter(s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.wTimeout,
											(void *)local_VdTimer_Callback,
											NULL,
											0,
											&(s_stVd_Timer_Info.RefInfo[i].TimerID)) != VK_OK )
					{
						HxLOG_Print(" IO - Timer Once Start Error\n");
						VK_SEM_Release(s_ulTMRAPISem);
						return (ia_result)VD_REF_ID_ERROR;
					}

					s_stVd_Timer_Info.RefInfo[i].Status = 1;
				}
				// CONTINUE
				else
				{
					if( VK_TIMER_EventEvery(s_stVd_Timer_Info.RefInfo[i].HUMAX_vd_timer_open_st.wTimeout,
											(void *)local_VdTimer_Callback,
											NULL,
											0,
											&(s_stVd_Timer_Info.RefInfo[i].TimerID)) != VK_OK )
					{
						HxLOG_Print(" IO - Timer Every Start Error\n");
						VK_SEM_Release(s_ulTMRAPISem);
						return (ia_result)VD_REF_ID_ERROR;
					}

					s_stVd_Timer_Info.RefInfo[i].Status = 1;
				}
			}
			break;

		case	VD_TIMER_STOP:
			if( s_stVd_Timer_Info.RefInfo[i].Status == 1 )
			{
				VK_TIMER_Cancel(s_stVd_Timer_Info.RefInfo[i].TimerID);
				s_stVd_Timer_Info.RefInfo[i].TimerID = 0;
				s_stVd_Timer_Info.RefInfo[i].Status = 0;
			}
			break;
	}

	VK_SEM_Release(s_ulTMRAPISem);
	return IA_SUCCESS;
}

/* end of file */

