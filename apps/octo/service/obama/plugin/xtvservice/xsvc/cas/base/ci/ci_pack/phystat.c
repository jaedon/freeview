/****************************************************************************/
/*                                                                          */
/*   FILE:    phystat.c														*/
/*   DATE:    31/07/97                                                      */
/*   PURPOSE: DVB-CI Physical layer - state diagram for PHYSICAL task       */
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version	Date	  	    Written by	     Notes	    	   			*/
/*	 1.0	    31/07/1997      TGA, Teamlog     Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*																		    */
/****************************************************************************/

#include "physical.h"           /* Main Module			  					*/
#include "physdrv.h"			/* High Level PHYSICAL Driver Access		*/
#include "itc.h"				/* Inter-Task Communication	*/
#include "phystat.h"




/*--------------------------------------------------------------------------*/
/* External Variables		                                            	*/
/*--------------------------------------------------------------------------*/
extern xmsg ph_infos;		/* received informations	 					*/
extern pslot ph_tab_slot[]; /* slot table   								*/

#ifdef	_PHY_DEBUG_
static char *_state[] =
{
	{""},
	{"NOT_USED"},
	{"IDLE"},
	{"CONNECTED"},
};
#endif

/* ======================================================================== */
/* NAME : PHYSICAL_Action					 								*/
/* 									    									*/
/* DESCRIPTION : Activate the action associated to the current state 		*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		info :		conditionnal information according to the event			*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		mem :		memory management										*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*									  										*/
/* ======================================================================== */
int PHYSICAL_Action(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;			/* return code 				*/

#ifdef	_PHY_DEBUG_
	HxLOG_Print(("PHYSICAL_Action : index=%d state=%s evt=0x%x\n", index, _state[ph_tab_slot[index].state], evt));
#endif

	switch(ph_tab_slot[index].state) {
		case CONNECTED :
			iRet = PHYSICAL_Connected(index, evt, pinfos, mem);
			break;

		case IDLE :
			iRet = PHYSICAL_Idle(index, evt, pinfos, mem);
			break;

		default :
			HxLOG_Error("PHYSICAL_Action : Unknown state %d !!!\n", ph_tab_slot[index].state);
			iRet = 0;	/* Error */
			break;
		}

	return iRet;
}


/* ======================================================================== */
/* NAME : PHYSICAL_Idle						 								*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the IDLE state						*/
/*		There is a new DVB module inserted . The PHYSICAL layer sends the	*/
/*		connection indication to the LINK layer								*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received event or NULL		*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		mem :		memory management										*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*									  										*/
/* ======================================================================== */
int PHYSICAL_Idle(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;		/* return code  */

	*mem = 1;	/* default : keep memory */

	switch(evt) {
		case PH_INSERT :

			/* send LN_CONNECT to the Link Task */
			/* =============== */
			ph_header.trans		= (PHYSICAL << 4) | LINK;
			ph_header.type     	= LN_CONNECT;
			ph_header.len      	= sizeof(sl_conn);
			ph_msg.lconn.slot_id= ph_tab_slot[index].slot;
			ph_msg.lconn.status = pinfos->inserthw.status;
			ph_msg.lconn.size   = pinfos->inserthw.size;

			HxLOG_Print("PHYSICAL_Idle : to send LN_CONNECT for slot %d to LINK Task\n", ph_tab_slot[index].slot);

		    /* Send the message */
    		if (! PHYSICAL_Send_Msg())
    		{
				HxLOG_Error("PHYSICAL_Idle : Can't send the LN_CONNECT for slot %d to LINK Task !!!\n", ph_tab_slot[index].slot);
				iRet = 0;		/* Error */
			}
			else
			{
		    	/* Set the new state */
				ph_tab_slot[index].state = CONNECTED;
				iRet = 1;		/* Success */
			}
			break;	/* end of LN_CONNECT processing */

		default :	/* protocol error, ignore the event */
			HxLOG_Error("PHYSICAL_Idle : protocol error . event %d received in %d state !!!\n", evt, ph_tab_slot[index].state);
			iRet = 0;
			break;
		}

	return iRet;
}	/* end of IDLE state */


/* ======================================================================== */
/* NAME : PHYSICAL_Connected						 						*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the CONNECTED state					*/
/*		- Receive LPDU from LINK task, send it to the PHYSICAL Driver and	*/
/*			send a LN_ACK acknowledge to the LNIK Task						*/
/*		- Receive event from PHYSICAL Driver and send it to the LINK Task	*/
/*			(Ex : LPDU)														*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received event or NULL		*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		mem :		memory management										*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*									  										*/
/* ======================================================================== */
int PHYSICAL_Connected(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
	//static unsigned long st;
	//unsigned long et;
int iRet;		/* return code  */

	*mem = 1;	/* default : keep memory */
	iRet = 1;

	switch(evt) {
		case LPDU :
			/* send LPDU to the PHYSICAL Driver */
			/* ========= */
			iRet = PHYS_DRVWrite(ph_tab_slot[index].slot, pinfos->datahw.pdata, pinfos->datahw.len);
			if (! iRet)
			{
				/* send a LN_ACK to the LINK Task */
				/* ======================= */
				ph_header.trans		= (PHYSICAL << 4) | LINK;
				ph_header.type     	= LN_ACK;
				ph_header.len      	= sizeof(sl_ack);
				ph_msg.ack.slot_id  = ph_tab_slot[index].slot;

				HxLOG_Print("PHYSICAL_Connected : to send LN_ACK for slot %d to LINK Task\n", ph_tab_slot[index].slot);

			    /* Send the message */
    			if (! PHYSICAL_Send_Msg())
    			{
					HxLOG_Error("PHYSICAL_Connected : Can't send the LN_ACK for slot %d to LINK Task !!!\n", ph_tab_slot[index].slot);
					iRet = 0;		/* Error */
				}
				else
				{
//					et = GetTick();
//					HxLOG_Print(("[%d]", et - st));
//					st = et;
//					OutCh('@');
					HxLOG_Print("PHYSICAL_Connected : LN_ACK for slot %d\n", ph_tab_slot[index].slot);
					*mem = 0;		/* Free memory */
					iRet = 1;		/* Success */
				}
			}
			else
			{
				HxLOG_Error("PHYSICAL_Connected : PHYS_DRVWrite error on slot %d !!!\n", ph_tab_slot[index].slot);
			}

			/* Don't change the state */
			break;

		case PH_DATA :

			/* send LPDU to the Link Task */
			/* =================== */
			ph_msg.lpdu.plpdu = ITC_DynAlloc(pinfos->datahw.len);	/* length = pinfos->datahw.len */
			if (ph_msg.lpdu.plpdu == NULL)
			{
				HxLOG_Error("PHYSICAL_Connected : can't allocate buffer for coding LPDU !!!\n");
				return 0;	/* Error */
			}

			ph_header.trans		= (PHYSICAL << 4) | LINK;
			ph_header.type     	= LPDU;
			ph_header.len      	= sizeof(sl_lpdu);
			ph_msg.lpdu.slot_id = ph_tab_slot[index].slot;
			ph_msg.lpdu.len     = pinfos->datahw.len;

// AlphaCrypt CAM 경우 TC ID 가 2인 경우 TPDU 의 TC ID 가 1이 되는 버그를 처리하기 위함.
// 참고로 SMiT CI+ 캠에서 CAM F/W upgrade 시에도 "[CI_RW_ERROR] TCID Error : pdata = [01 00 00 ... 00], 3 bytes !!!" 에러가 발생한다 (H/W ECO 안된 Douglas 플랫폼)
#if 1
			if ((pinfos->datahw.pdata[1] & 0x80) == 0) // LPDU 상의 M/L 체크
{
				if (pinfos->datahw.pdata[0] != pinfos->datahw.pdata[pinfos->datahw.len-2]) // LPDU 의 TC ID 와 R-TPDU 의 SB 상의 TC ID 가 같은지 체크 (TODO: R-TPDU 헤더 상의 TC ID 는 체크할 필요가 없나?)
				{
					HxLOG_Error("[CI_RW_ERROR] TCID Error : pdata = [%02x %02x %02x ... %02x], %d bytes !!!\n",
						pinfos->datahw.pdata[0], pinfos->datahw.pdata[1], pinfos->datahw.pdata[2], pinfos->datahw.pdata[pinfos->datahw.len-2], pinfos->datahw.len);

// TODO : 아래 코드는 나중에 2 slot 모델에서 좀더 확인하고 사용하도록 하자...
//					pinfos->datahw.pdata[pinfos->datahw.len-2] = pinfos->datahw.pdata[0];
				}
			}
#endif

			HxSTD_memcpy(ph_msg.lpdu.plpdu, pinfos->datahw.pdata, pinfos->datahw.len);
#if	0
			{
				char buf_lpdu[1000];
				int j;

				/* display the received buffer in ASCII */
				for (j=0;j<(int)ph_msg.lpdu.len;j++)
					sprintf((char *)(buf_lpdu+(3*j)),"%02x ",*(ph_msg.lpdu.plpdu+j));
				buf_lpdu[(3*ph_msg.lpdu.len)-1] = '\0';
				if (ph_msg.lpdu.len > 50)
					buf_lpdu[200] = '\0';
				HxLOG_Print(("PHYSICAL_Connected : PH_DATA : %d received bytes from the module %d : %s\n", ph_msg.lpdu.len, ph_msg.lpdu.slot_id, buf_lpdu));
			}
#endif

			HxLOG_Print("PHYSICAL_Connected : to send LPDU for slot %d to LINK Task\n", ph_tab_slot[index].slot);

		    /* Send the message */
    		if (! PHYSICAL_Send_Msg())
    		{
				ITC_DynFree(ph_msg.lpdu.plpdu);	  	/* free memory used for the LPDU to send */
				HxLOG_Error("PHYSICAL_Connected : Can't send the LPDU for slot %d to LINK Task !!!\n", ph_tab_slot[index].slot);
				iRet = 0;		/* Error */

			}

			/* Don't change the state */
			break;

		case PH_ERROR :

			/* send LN_ERROR to the Link Task */
			/* ======================= */
			ph_header.trans			= (PHYSICAL << 4) | LINK;
			ph_header.type			= LN_ERROR;
			ph_header.len			= sizeof(sl_error);
			ph_msg.error.slot_id	= ph_tab_slot[index].slot;
			ph_msg.error.status		= pinfos->errorhw.status;

			HxLOG_Print("PHYSICAL_Connected : to send LN_ERROR for slot %d to LINK Task\n", ph_tab_slot[index].slot);

		    /* Send the message */
    		if (! PHYSICAL_Send_Msg())
    		{
				HxLOG_Error("PHYSICAL_Connected : Can't send the LN_ERROR for slot %d to LINK Task !!!\n", ph_tab_slot[index].slot);
				iRet = 0;		/* Error */
			}

			/* Don't change the state */
			break;

		case PH_EXTRACT :

			/* send LN_DISCONNECT to the Link Task */
			/* =========================== */
			ph_header.trans		= (PHYSICAL << 4) | LINK;
			ph_header.type     	= LN_DISCONNECT;
			ph_header.len      	= sizeof(sl_disc);
			ph_msg.disc.slot_id= ph_tab_slot[index].slot;

			HxLOG_Print("PHYSICAL_Connected : to send LN_DISCONNECT for slot %d to LINK Task\n", ph_tab_slot[index].slot);

		    /* Send the message */
    		if (! PHYSICAL_Send_Msg())
    		{
				HxLOG_Error("PHYSICAL_Connected : Can't send the LN_DISCONNECT for slot %d to LINK Task !!!\n", ph_tab_slot[index].slot);
				iRet = 0;		/* Error */
			}
			else
			{
		    	/* Set the new state */
				ph_tab_slot[index].state = IDLE;
				iRet = 1;		/* Success */
			}
			break;

		default :	/* protocol error, ignore the event */
			HxLOG_Error("PHYSICAL_Connected : protocol error . event %d received in %d state !!!\n", evt, ph_tab_slot[index].state);
			iRet = 0;
			break;
		}

	return iRet;
}	/* end of CONNECTED state */

