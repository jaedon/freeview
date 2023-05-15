/****************************************************************************/
/*                                                                          */
/*   FILE:    transtat.c                                                    */
/*   DATE:    13/06/97                                                      */
/*   PURPOSE: DVB-CI Transport layer - state diagram for TRANS task			*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                 			    */
/*   Version    Date  		    Written by	     Notes	    	    		*/
/*	 1.0	    13/06/1997      SCM              Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*																		    */
/****************************************************************************/

#include "ci.h"

#include "transtat.h"
#include "tranfifo.h"			/* FIFO Management Module 	*/
#include "trans.h"              /* Main Module			  	*/
#include "transcod.h"			/* Coding/Decoding Module	*/
#include "itc.h"				/* Inter-Task Communication	*/


/*--------------------------------------------------------------------------*/
/* External Variables		                                            	*/
/*--------------------------------------------------------------------------*/
extern xmsg t_infos;		/* received informations	 					*/
extern xmsg t_infos1;		/* other saved informations	 					*/
extern trpt t_tab_tcid[];   /* tcid table   								*/

#ifdef _INCLUDE_CI_TRPT_RECOVERY_
extern unsigned char g_aucCiTrptWaitCnt[MAXTCID];
extern unsigned char g_aucIsCamInComa[PAL_MAX_CISLOT_NUMBER];
#endif

/* ======================================================================== */
/* NAME : TRPT_Action					 									*/
/* 									    									*/
/* DESCRIPTION : Activate the action associated to the current state 		*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received TPDU or NULL		*/
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
int TRPT_Action(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;			/* return code 				*/
unsigned char tag;	/* tag got from fifo		*/
unsigned char *pt;	/* point to SPDU buffer		*/
int mem1;			/* local memory management  */

	PrintDbg(("TRPT_Action : tcid %d : state %d timer=0x%x timerpoll=0x%x\n",
		t_tab_tcid[index].tcid,
		t_tab_tcid[index].state,
		t_tab_tcid[index].timerid,
		t_tab_tcid[index].tmpollid));

	switch(t_tab_tcid[index].state) {
		case ACTIVE : 	 iRet = TRPT_Active(index, evt, pinfos, mem);
			PrintDbg(("TRPT_Action : ACTIVE Received\n"));
			break;

		case POLLING : 	 iRet = TRPT_Polling(index, evt, pinfos, mem);
			PrintDbg(("TRPT_Action : POLLING Received\n"));

			/* try to send a SPDU saved in fifo */
			/* ================================ */
			if ((iRet == 1) && (t_tab_tcid[index].state == ACTIVE))
			{
				if ((tag = TRPT_GetFifo(index, &(t_msg.spdu))))
				{
					pt = t_msg.spdu.pspdu;
					iRet = TRPT_Active(index, tag, NULL, &mem1);
					if (! mem1)
						ITC_DynFree(pt);
				}
			}
			break;

		case RECEPTION : iRet = TRPT_Reception(index, evt, pinfos, mem);
			PrintDbg(("TRPT_Action : RECEPTION Received\n"));

			/* try to send a SPDU saved in fifo */
			/* ================================ */
			if ((iRet == 1) && (t_tab_tcid[index].state == ACTIVE))
			{
				if ((tag = TRPT_GetFifo(index, &(t_msg.spdu))))
				{
					pt = t_msg.spdu.pspdu;
					iRet = TRPT_Active(index, tag, NULL, &mem1);
					if (! mem1)
						ITC_DynFree(pt);
				}
			}
			break;

		case POLLMORE :	 iRet = TRPT_Pollmore(index, evt, pinfos, mem);
			PrintDbg(("TRPT_Action : POLLMORE Received\n"));

			/* try to send a SPDU saved in fifo */
			/* ================================ */
			if ((iRet == 1) && (t_tab_tcid[index].state == ACTIVE))
			{
				if ((tag = TRPT_GetFifo(index, &(t_msg.spdu))))
				{
					pt = t_msg.spdu.pspdu;
					iRet = TRPT_Active(index, tag, NULL, &mem1);
					if (! mem1)
						ITC_DynFree(pt);
				}
			}
			break;

		case IDLE : 	 iRet = TRPT_Idle(index, evt, mem);
			PrintDbg(("TRPT_Action: IDLE Received\n"));
			break;

		case CREATION :  iRet = TRPT_Creation(index, evt, pinfos, mem);
			PrintDbg(("TRPT_Action : CREATION Received\n"));
			break;

		case DELETION :  iRet = TRPT_Deletion(index, evt, pinfos, mem);
			PrintDbg(("TRPT_Action : DELETION Received\n"));
			break;

		default :
			PrintErr(("TRPT_Action : Unknown state %d !!!\n", t_tab_tcid[index].state));
			iRet = 0;	/* Error */
			break;
		}

	return iRet;
}


/* ======================================================================== */
/* NAME : TRPT_Idle						 									*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the IDLE state						*/
/*		There is a new DVB module inserted . The Transport layer must create*/
/*		the first transport (tcid = 1) connection for this physical link	*/
/* 		The LINK task has already checked that the slot_id is valid			*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
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
int TRPT_Idle(int index, unsigned char evt, int *mem)
{
int iRet;			/* return code  */
unsigned long len;

	*mem = 1;	/* default */

	switch(evt) {
		case TP_CONNECT :
			PrintDbg(("TRPT_Idle : TP_CONNECT Received\n"));

			/* Get free buffer for TPDU message */
#ifdef FASTALLOC
			t_msg.tpdu.ptpdu = ITC_Alloc();						/* length = sizeof(create_t_c) */
#else
			t_msg.tpdu.ptpdu = ITC_DynAlloc(S_CREATE_T_C);		/* length = sizeof(create_t_c) */
#endif
			if (t_msg.tpdu.ptpdu == NULL)
			{
				PrintErr(("TRPT_Idle : can't allocate buffer for coding TPDU !!!\n"));
				return 0;	/* Error */
			}

			/* Create new transport connection with tcid = 1 and with allocated memory for the TPDU message */
			/* ============================================= */
			t_header.trans		= (TRANS << 4) | LINK;
			t_header.type     	= TPDU;
			t_header.len      	= sizeof(st_tpdu);
			t_msg.tpdu.slot_id  = t_tab_tcid[index].slot_id;
			t_msg.tpdu.tcid     = t_tab_tcid[index].tcid;
			len  	= TRPT_Code(create_t_c, t_msg.tpdu.tcid, NULL, t_msg.tpdu.ptpdu);
    		if (len == 0) // if (len < 0)
    		{
				PrintErr(("TRPT_Idle : Can't code a create_t_c for slot %d !!!\n", t_tab_tcid[index].slot_id));
				*mem = 0;		/* t_msg.tpdu.ptpdu must be released */
				return 0;		/* Error */
			}
			else
			{
				t_msg.tpdu.len = len;
			}

		    /* Send the message */
    		if (! TRPT_Send_Msg())
    		{
				PrintErr(("TRPT_Idle : Can't send the create_t_c for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
				*mem = 0;		/* t_msg.tpdu.ptpdu must be released */
				iRet = 0;		/* Error */
			}
			else
			{
				/* Set the timer waiting for the response for this tcid */
				//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
				TRPT_Set_Timer(index);

		    	/* Set the new state */
				t_tab_tcid[index].state = CREATION;
				iRet = 1;		/* Success */
			}
			break;	/* end of TP_CONNECT processing */

		case t_sb :	/* t_sb received after a delete_t_c : ignored */
			PrintDbg(("TRPT_Idle : t_sb Received\n"));

   			if (!t_tab_tcid[index].tag)
				*mem = 0;		/* free received buffer */
			iRet = 1;
			break;

		default :	/* protocol error, ignore the event */
			PrintErr(("TRPT_Idle : protocol error . event 0x%x received in %d state !!!\n", evt, t_tab_tcid[index].state));
			iRet = 0;
			break;
		}

	return iRet;
}	/* end of IDLE state */


/* ======================================================================== */
/* NAME : TRPT_Creation					 									*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the CREATION state					*/
/*	The transport layer waits for a c_t_c_reply								*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received TPDU or NULL		*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		mem :		memory management										*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*	If the calling function is TRPT_Tpdu() or TRPT_Tpdu() and 				*/
/*		if the return code is 0 then mem is assigned with 0 in TRPT_Tpdu() 	*/
/*		--> free t_psmg.tpdu												*/
/*									  										*/
/* ======================================================================== */
int TRPT_Creation(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;	/* return code */
unsigned long len;

		*mem = 1;		/* default */

		switch(evt) {
		case TIMEOUT :
			PrintDbg(("TRPT_Creation : TIMEOUT Received\n"));

    		/* Set the new state */
			t_tab_tcid[index].state = IDLE;
			iRet = 1;		/* Success */
			break;	/* end of TIMEOUT processing */

		case TP_DISCONNECT :
			PrintDbg(("TRPT_Creation : TP_DISCONNECT Received\n"));

	      	/* kill pending timer 	*/
			TRPT_Kill_Timer(index);

    		/* Set the new state */
			t_tab_tcid[index].state = IDLE;
			iRet = 1;		/* Success */
			break;	/* end of TP_DISCONNECT processing */

		case c_t_c_reply :
        	/* Verify parameters  : only tcid, already made */

        	/* kill pending timer 	*/
			TRPT_Kill_Timer(index);

			/* send SS_TC_CREATED to Session Task */
			/* ================== */
			t_header.trans		= (TRANS << 4) | SESS;
			t_header.type     	= SS_TC_CREATED;
			t_header.len      	= sizeof(st_creat);
			t_msg.creat.tcid    = t_tab_tcid[index].tcid;

			PrintDbg(("TRPT_Creation : c_t_c_reply Start tcid = %d, Slot_id = %d\n", t_msg.creat.tcid, t_tab_tcid[index].slot_id /* t_msg.creat.slot_id */));

		    /* Send the message */
    		if (! TRPT_Send_Msg())
    		{
				PrintErr(("TRPT_Creation : Can't send the SS_TC_CREATED for tcid %d to SESS Task !!!\n", t_tab_tcid[index].tcid));
				*mem = 0;		/* free received buffer */
				return 0;		/* Error */
			}

    		/* state unchanged since the host is waiting for t_sb following the c_t_c_reply before to change the state */
			/* Set the timer waiting for the t_sb */
			//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
			TRPT_Set_Timer(index);

			*mem = 0;		/* free received buffer */
			iRet = 1;		/* Success */
            break;	/* end of c_t_c_reply processing */

		case t_sb :
			PrintDbg(("TRPT_Creation : t_sb Received\n"));

        	/* kill pending timer */
			TRPT_Kill_Timer(index);

			if (pinfos->sbtc.SB_value == DATA_AVAILABLE)
			{   /* t_sb(1) received : data available on module */
				/* send a t_rcv  : the same buffer tpmsg.tpdu is used for TPDU */
				/* ============ */
				t_header.trans		= (TRANS << 4) | LINK;
				t_header.type     	= TPDU;
				t_header.len      	= sizeof(st_tpdu);
				t_msg.tpdu.slot_id  = t_tab_tcid[index].slot_id;
				t_msg.tpdu.tcid     = t_tab_tcid[index].tcid;
				len  	= TRPT_Code(t_rcv, t_tab_tcid[index].tcid, NULL, t_msg.tpdu.ptpdu);
    			if (len == 0) // if (len < 0)
    			{
					PrintErr(("TRPT_Creation : Can't code a t_rcv for slot %d !!!\n", t_tab_tcid[index].slot_id));
					return 0;		/* Error */
				}
				else
				{
					t_msg.tpdu.len = len;
				}

			    /* Send the message */
    			if (! TRPT_Send_Msg())
    			{
					PrintErr(("TRPT_Creation : Can't send the t_rcv for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
					iRet = 0;		/* Error */
				}
				else
				{
					/* Set the timer waiting for the response for this tcid */
					//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
					TRPT_Set_Timer(index);

		    		/* Set the new state */
					t_tab_tcid[index].state = RECEPTION;

					iRet = 1;		/* Success */
				}
			}	/* end of t_sb(1) processing */
			else
			{   /* t_sb(0) received : no data available on module */
				/* send a tpdu(poll) */
				/* ================= */
				t_header.trans		= (TRANS << 4) | LINK;
				t_header.type     	= TPDU;
				t_header.len      	= sizeof(st_tpdu);
				t_msg.tpdu.slot_id  = t_tab_tcid[index].slot_id;
				t_msg.tpdu.tcid     = t_tab_tcid[index].tcid;
				len  	= TRPT_Code(t_poll, t_tab_tcid[index].tcid, NULL, t_msg.tpdu.ptpdu);
    			if (len == 0) // if (len < 0)
    			{
					PrintErr(("TRPT_Creation : Can't code a t_data_last(poll) for slot %d !!!\n", t_tab_tcid[index].slot_id));
					return 0;		/* Error 	   */
				}
				else
				{
					t_msg.tpdu.len = len;
				}

		    	/* Send the message */
    			if (! TRPT_Send_Msg())
    			{
					PrintErr(("TRPT_Creation : Can't send the tpdu(poll) for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
					iRet = 0;		/* Error 	   */
				}
				else
				{
					/* Set the timer waiting for the response for this tcid */
					//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
					TRPT_Set_Timer(index);
					iRet = 1;		/* Success */
				}

    			/* Set the new state */
    			t_tab_tcid[index].state = POLLING;

				iRet = 1;		   /* Success */
			}   /* end of t_sb(0) processing  */
			break;	/* end of t_sb processing */

		case delete_t_c :
			PrintDbg(("TRPT_Creation : delete_t_c Received\n"));

        	/* Verify parameters  : only tcid, already made */

        	/* kill pending timer 	*/
			TRPT_Kill_Timer(index);

			/* Get free buffer for TPDU message */
#ifdef FASTALLOC
			t_msg1.tpdu.ptpdu = ITC_Alloc();						/* length = sizeof(d_t_c_reply) */
#else
			t_msg1.tpdu.ptpdu = ITC_DynAlloc(S_D_T_C_REPLY);		/* length = sizeof(d_t_c_reply) */
#endif
			if (t_msg1.tpdu.ptpdu == NULL)
			{
				PrintErr(("TRPT_Creation : can't allocate buffer for coding TPDU !!!\n"));
				return 0;	/* Error */
			}

			/* send a d_t_c_reply  : the same buffer tpmsg.tpdu is used for TPDU */
			/* ================== */
			t_header1.trans		= (TRANS << 4) | LINK;
			t_header1.type     	= TPDU;
			t_header1.len      	= sizeof(st_tpdu);
			t_msg1.tpdu.slot_id = t_tab_tcid[index].slot_id;
			t_msg1.tpdu.tcid    = t_tab_tcid[index].tcid;
			len  	= TRPT_Code(d_t_c_reply, t_tab_tcid[index].tcid, NULL, t_msg1.tpdu.ptpdu);
    		if (len == 0) // if (len < 0)
    		{
				PrintErr(("TRPT_Creation : Can't code a d_t_c_reply for slot %d !!!\n", t_tab_tcid[index].slot_id));
				*mem = 0;		/* free received buffer */
				return 0;		/* Error */
			}
			else
			{
				t_msg1.tpdu.len = len;
			}

		    /* Send the message */
    		if (! TRPT_Send_Msg1())
    		{
				PrintErr(("TRPT_Creation : Can't send the d_t_c_reply for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
				*mem = 0;		/* free received buffer */
				iRet = 0;		/* Error */
			}
			else
			{
		    	/* Set the new state and keep memory (*mem = 1) */
				t_tab_tcid[index].state = IDLE;
				*mem = 0;		/* free received buffer */
				iRet = 1;		/* Success */
			}
            break;	/* end of delete_t_c processing */

		default :	/* protocol error, ignore the event */
			PrintErr(("TRPT_Creation : protocol error . event 0x%x received in %d state !!!\n", evt, t_tab_tcid[index].state));
			iRet = 0;
			break;
		}

	return iRet;
}



/* ======================================================================== */
/* NAME : TRPT_Active					 									*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the ACTIVE state						*/
/*	a transport connection exists and TPDU are exchanged on the tcid		*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received TPDU or NULL		*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		mem :		memory management										*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*	If the calling function is TRPT_Tpdu() or TRPT_Spdu() and 				*/
/*		if the return code is 0 then mem is assigned with 0 in TRPT_Tpdu() 	*/
/*		or TRPT_Spdu --> free t_psmg.tpdu									*/
/*									  										*/
/* ======================================================================== */
int TRPT_Active(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;	/* return code 													*/
int alloc;	/* memory allocation strategy 									*/
tc_infos infos;	/* information for SPDU		 								*/
unsigned long len;

	*mem = 1;		/* default */
	iRet = 1;


	switch(evt) {
		case TP_DISCONNECT :
			PrintDbg(("TRPT_Active : TP_DISCONNECT Received\n"));

			/* send the SS_TC_DELETED message */
			/* ============================== */
			TRPT_Kill_PollTimer(index);	/* kill the pending TIMEPOLL timer	*/

			iRet = TRPT_Disc(index);
			if (iRet)
		    	/* Set the new state */
				t_tab_tcid[index].state = IDLE;
			break;	/* end of TP_DISCONNECT processing */

		case SPDU:
			PrintDbg(("TRPT_Active : SPDU Received\n"));

			TRPT_Kill_PollTimer(index);	/* kill the pending TIMEPOLL timer	*/

			/* Get free buffer for TPDU message (t_msg1 since t_msg points the received SPDU) */
			t_msg1.tpdu.ptpdu = ITC_DynAlloc(t_msg.spdu.len + T_DATA_HEADER);
			if (t_msg1.tpdu.ptpdu == NULL)
			{
				PrintErr(("TRPT_Active : can't allocate buffer for coding TPDU !!!\n"));
				return 0;	/* Error */
			}

			if (t_msg.spdu.len <= MAXTPDUSIZE)
			{
				/* Code the only TPDU and send it */
				/* ============================== */
				infos.datatc.len    = t_msg.spdu.len + 1;		 /* Add 1 byte for Tcid */
				infos.datatc.pbuff  = t_msg.spdu.pspdu;          /* SPDU buffer to copy into TPDU buffer */
				t_header1.trans		= (TRANS << 4) | LINK;
#ifdef FASTALLOC
				t_header1.type     	= TPDUDYN;
#else
				t_header1.type     	= TPDU;
#endif
				t_header1.len      	= sizeof(st_tpdu);
				t_msg1.tpdu.slot_id = t_tab_tcid[index].slot_id;
				t_msg1.tpdu.tcid    = t_tab_tcid[index].tcid;
				len  	= TRPT_Code(t_data_last, t_tab_tcid[index].tcid, &infos, t_msg1.tpdu.ptpdu);
   				if (len == 0) // if (len < 0)
   				{
					PrintErr(("TRPT_Active : Can't code a t_data_last for slot %d !!!\n", t_tab_tcid[index].slot_id));
					ITC_DynFree(t_msg1.tpdu.ptpdu);    /* free memory */
					return 0;		/* Error */
				}
				else
				{
					t_msg1.tpdu.len = len;
				}

				t_tab_tcid[index].len   = 0;	/* SPDU entirely sent */
				t_tab_tcid[index].pspdu = NULL;

	    		/* Set the new state */
				t_tab_tcid[index].state = POLLING;
				*mem = 0;		/* free t_msg buffer since TPDU is into the t_msg1 buffer */
			}	/* end of sending the TPDU */

			else
			{   /* split SPDU into TPDUs, send the first TPDU */

				/* Code the first TPDU and send it */
				/* =============================== */
				infos.datatc.len    = MAXTPDUSIZE;
				infos.datatc.pbuff  = t_msg.spdu.pspdu;          /* SPDU buffer to copy into TPDU buffer */
				t_header1.trans		= (TRANS << 4) | LINK;
				t_header1.type     	= TPDU;
				t_header1.len      	= sizeof(st_tpdu);
				t_msg1.tpdu.slot_id = t_tab_tcid[index].slot_id;
				t_msg1.tpdu.tcid    = t_tab_tcid[index].tcid;
				len  	= TRPT_Code(t_data_more, t_tab_tcid[index].tcid, &infos, t_msg1.tpdu.ptpdu);
   				if (len == 0) // if (len < 0)
   				{
					PrintErr(("TRPT_Active : Can't code a t_data_more for slot %d !!!\n", t_tab_tcid[index].slot_id));
					ITC_DynFree(t_msg1.tpdu.ptpdu);  	/* free memory */
					return 0;		/* Error */
				}
				else
				{
					t_msg1.tpdu.len = len;
				}

				/* Save and Update the SPDU pointer for the next TPDU send ; Keep memory (*mem = 1) */
				t_tab_tcid[index].psend = t_msg.spdu.pspdu;						/* save the complete SPDU for release when the SPDU is completly sent */
				t_tab_tcid[index].pspdu = t_msg.spdu.pspdu + MAXTPDUSIZE - 1;	/* Sub 1 byte for Tcid */
				t_tab_tcid[index].len = t_msg.spdu.len - (MAXTPDUSIZE - 1);		/* Sub 1 byte for Tcid */

				/* Set the new state */
				t_tab_tcid[index].state = POLLMORE;
			}	/* end of sending the first TPDU of splitted SPDU */

			/* Send the message */
			if (! TRPT_Send_Msg1())
			{
				PrintErr(("TRPT_Active : Can't send the t_data_last for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
				ITC_DynFree(t_msg1.tpdu.ptpdu);  	/* free memory */
				iRet = 0;		/* Error */
			}
			else
				/* Set the timer waiting for the response for this tcid */
				//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
				TRPT_Set_Timer(index);
			break;

		case SS_DELETE_TC :
			PrintDbg(("TRPT_Active : SS_DELETE_TC Received\n"));

			TRPT_Kill_PollTimer(index);	/* kill the pending TIMEPOLL timer	*/

        	/* Verify parameters  : only tcid, already made */

			/* Get free buffer for TPDU message */
#ifdef FASTALLOC
			t_msg.tpdu.ptpdu = ITC_Alloc();						/* length = sizeof(delete_t_c) */
#else
			t_msg.tpdu.ptpdu = ITC_DynAlloc(S_DELETE_T_C);		/* length = sizeof(delete_t_c) */
#endif
			if (t_msg.tpdu.ptpdu == NULL)
			{
				PrintErr(("TRPT_Active : can't allocate buffer for coding TPDU !!!\n"));
				return 0;	/* Error */
			}

			/* send a delete_t_c */
			/* ================= */
			t_header.trans		= (TRANS << 4) | LINK;
			t_header.type     	= TPDU;
			t_header.len      	= sizeof(st_tpdu);
			t_msg.tpdu.slot_id  = t_tab_tcid[index].slot_id;
			t_msg.tpdu.tcid     = t_tab_tcid[index].tcid;
			len  	= TRPT_Code(delete_t_c, t_tab_tcid[index].tcid, NULL, t_msg.tpdu.ptpdu);
    		if (len == 0) // if (len < 0)
    		{
				PrintErr(("TRPT_Active : Can't code a delete_t_c for slot %d !!!\n", t_tab_tcid[index].slot_id));
				return 0;		/* Error */
			}
			else
			{
				t_msg.tpdu.len = len;
			}

		    /* Send the message */
    		if (! TRPT_Send_Msg())
    		{
				PrintErr(("TRPT_Active : Can't send the delete_t_c for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
				iRet = 0;		/* Error */
			}
			else
			{
				/* Set the timer waiting for the response for this tcid */
				//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
				TRPT_Set_Timer(index);

	    		/* Set the new state */
				t_tab_tcid[index].state = DELETION;
				iRet = 1;		/* Success */
			}
            break;	/* end of delete_t_c processing */

		case TIMEPOLL :
			PrintDbg(("TRPT_Active : TIMEPOLL Received\n"));

			/* Get free buffer for TPDU message */
#ifdef FASTALLOC
			t_msg.tpdu.ptpdu = ITC_Alloc();					/* length = sizeof(t_poll) */
#else
			t_msg.tpdu.ptpdu = ITC_DynAlloc(S_POLL);		/* length = sizeof(t_poll) */
#endif
			if (t_msg.tpdu.ptpdu == NULL)
			{
				PrintErr(("TRPT_Active : can't allocate buffer for coding TPDU !!!\n"));
				return 0;	/* Error */
			}

			/* send a tpdu(poll) */
			/* ================= */
			t_header.trans		= (TRANS << 4) | LINK;
			t_header.type     	= TPDU;
			t_header.len      	= sizeof(st_tpdu);
			t_msg.tpdu.slot_id  = t_tab_tcid[index].slot_id;
			t_msg.tpdu.tcid     = t_tab_tcid[index].tcid;
			len  	= TRPT_Code(t_poll, t_tab_tcid[index].tcid, NULL, t_msg.tpdu.ptpdu);
    		if (len == 0) // if (len < 0)
    		{
				PrintErr(("TRPT_Active : Can't code a t_data_last(poll) for slot %d !!!\n", t_tab_tcid[index].slot_id));
				*mem = 0;		/* free memory */
				return 0;		/* Error */
			}
			else
			{
				t_msg.tpdu.len = len;
			}

		    /* Send the message */
    		if (! TRPT_Send_Msg())
    		{
				PrintErr(("TRPT_Active : Can't send the tpdu(poll) for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
				*mem = 0;		/* free memory */
				iRet = 0;		/* Error */
			}
			else
			{
				/* Set the timer waiting for the response for this tcid */
				//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
				TRPT_Set_Timer(index);

	    		/* Set the new state */
				t_tab_tcid[index].state = POLLING;
				iRet = 1;		/* Success */
			}
            break;	/* end of TM_POLL processing */

		default :	/* protocol error, release the transport connection */
			PrintErr(("TRPT_Active : protocol error . event 0x%x received in %d state !!!\n", evt, t_tab_tcid[index].state));

			/* release the transport connection */
			/* ================================ */
			if (evt < MSG_MAXVAL )
				alloc = 1;      /* not TPDU or SPDU : memory must be allocated to send TPDU */
			else
				alloc = 0;		/* TPDU or SPDU : memory already exist */
			iRet = TRPT_Release(index, alloc, mem);
			if (iRet)
		    	/* Set the new state */
				t_tab_tcid[index].state = DELETION;
			break;	/* end of default processing */
		}

	return iRet;
}


/* ======================================================================== */
/* NAME : TRPT_Polling					 									*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the POLLING state					*/
/*	a transport connection exists and a polling or complete data 			*/
/*	have been sent by the host : wait for an answer to t_data_last 			*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received TPDU or NULL		*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		mem :		memory management										*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*	If the calling function is TRPT_Tpdu() or TRPT_Spdu() and 				*/
/*		if the return code is 0 then mem is assigned with 0 in TRPT_Tpdu() 	*/
/*		or TPRT_Spdu() --> free t_psmg.tpdu									*/
/*									  										*/
/* ======================================================================== */
int TRPT_Polling(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;		/* return code 												*/
int	new_index;	/* instance of the new state diagram						*/
tc_infos infos;	/* information for t_c_error / new_t_c						*/
int alloc;		/* memory management										*/
unsigned long len;

	*mem = 1;	/* default 													*/
	iRet = 1;

	switch(evt) {
		case TP_DISCONNECT :
			PrintDbg(("TRPT_Polling : TP_DISCONNECT Received\n"));

        	/* kill pending timer */
			TRPT_Kill_Timer(index);

			/* send the SS_TC_DELETED message */
			/* ============================== */
			iRet = TRPT_Disc(index);
			if (iRet)
		    	/* Set the new state */
				t_tab_tcid[index].state = IDLE;
			break;	/* end of TP_DISCONNECT processing */

		case t_sb :
			PrintDbg(("TRPT_Polling : t_sb Received\n"));

#ifdef _INCLUDE_CI_TRPT_RECOVERY_
			g_aucCiTrptWaitCnt[index] = 0;
			g_aucIsCamInComa[t_tab_tcid[index].slot_id] = 0;
#endif

        	/* kill pending timer */
			TRPT_Kill_Timer(index);

			if (pinfos->sbtc.SB_value == DATA_AVAILABLE)
			{   /* t_sb(1) received : data available on module */
				/* send a t_rcv  : the same buffer tpmsg.tpdu is used for TPDU */
				/* ============ */
				t_header.trans		= (TRANS << 4) | LINK;
				t_header.type     	= TPDU;
				t_header.len      	= sizeof(st_tpdu);
				t_msg.tpdu.slot_id  = t_tab_tcid[index].slot_id;
				t_msg.tpdu.tcid     = t_tab_tcid[index].tcid;
				len      = TRPT_Code(t_rcv, t_tab_tcid[index].tcid, NULL, t_msg.tpdu.ptpdu);
    			if (len == 0) // if (len < 0)
    			{
					PrintErr(("TRPT_Polling : Can't code a t_rcv for slot %d !!!\n", t_tab_tcid[index].slot_id));
					return 0;		/* Error */
				}
				else
				{
					t_msg.tpdu.len = len;
				}

			    /* Send the message */
    			if (! TRPT_Send_Msg())
    			{
					PrintErr(("TRPT_Polling : Can't send the t_rcv for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
					iRet = 0;		/* Error */
				}
				else
				{
					/* Set the timer waiting for the response for this tcid */
					//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
					TRPT_Set_Timer(index);

		    		/* Set the new state */
					t_tab_tcid[index].state = RECEPTION;
				}
			}	/* end of t_sb(1) processing */
			else
			{   /* t_sb(0) received : no data available on module */

				/* if a SPDU is being sent, continue to send it (next TPDU) */
				if ((t_tab_tcid[index].len > 0) && (t_tab_tcid[index].pspdu != NULL))
				{

					infos.datatc.pbuff = t_tab_tcid[index].pspdu;    /* SPDU buffer to copy into TPDU buffer */

					t_header1.trans	    = (TRANS << 4) | LINK;
#ifdef FASTALLOC
					t_header1.type      = TPDUDYN;
#else
					t_header1.type      = TPDU;
#endif
					t_header1.len       = sizeof(st_tpdu);
					t_msg1.tpdu.slot_id = t_tab_tcid[index].slot_id;	/* re-use the current buffer */
					t_msg1.tpdu.tcid    = t_tab_tcid[index].tcid;

					if (t_tab_tcid[index].len > MAXTPDUSIZE)
					{	/* several TPDU to send ; send the first here */
						/* ========================================== */
						/* Get free buffer for TPDU message */
						t_msg1.tpdu.ptpdu = ITC_DynAlloc(MAXTPDUSIZE + T_DATA_HEADER);
						if (t_msg1.tpdu.ptpdu == NULL)
						{
							PrintErr(("TRPT_Polling : can't allocate buffer for coding TPDU !!!\n"));
							return 0;	/* Error */
						}

						infos.datatc.len = MAXTPDUSIZE;
						len = TRPT_Code(t_data_more, t_tab_tcid[index].tcid, &infos, t_msg1.tpdu.ptpdu);
    					if (len == 0) // if (len < 0)
    					{
							PrintErr(("TRPT_Polling : Can't code a t_data_more for slot %d !!!\n", t_tab_tcid[index].slot_id));
							return 0;		/* Error */
						}
						else
						{
							t_msg1.tpdu.len = len;
						}

						/* Save and Update the SPDU pointer for the next TPDU send ; Keep memory (*mem = 1) */
						t_tab_tcid[index].pspdu += (MAXTPDUSIZE-1);	/* Sub 1 byte for Tcid */
						t_tab_tcid[index].len -= (MAXTPDUSIZE-1);	/* Sub 1 byte for Tcid */

				    	/* Set the new state */
						t_tab_tcid[index].state = POLLMORE;
					}
					else
					{	/* last TPDU to send */
						/* ================= */
						/* Get free buffer for TPDU message */
						t_msg1.tpdu.ptpdu = ITC_DynAlloc(t_tab_tcid[index].len + T_DATA_HEADER);
						if (t_msg1.tpdu.ptpdu == NULL)
						{
							PrintErr(("TRPT_Polling : can't allocate buffer for coding TPDU !!!\n"));
							return 0;	/* Error */
						}

						infos.datatc.len = t_tab_tcid[index].len + 1;		  /* Add 1 byte for Tcid */;
						len = TRPT_Code(t_data_last, t_tab_tcid[index].tcid, &infos, t_msg1.tpdu.ptpdu);
						if (len == 0) // if (len < 0)
						{
							PrintErr(("TRPT_Polling : Can't code a t_data_last for slot %d !!!\n", t_tab_tcid[index].slot_id));
							return 0;		/* Error */
						}
						else
						{
							t_msg1.tpdu.len = len;
						}

						t_tab_tcid[index].len   = 0;	/* SPDU entirely sent */
                    	t_tab_tcid[index].pspdu = NULL;

						ITC_DynFree(t_tab_tcid[index].psend);
						t_tab_tcid[index].psend = NULL;

				    	/* Set the new state */
						t_tab_tcid[index].state = POLLING;
					}

				    /* Send the message */
    				if (! TRPT_Send_Msg1())
    				{
						PrintErr(("TRPT_Polling : Can't send the tpdu(poll) for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
						iRet = 0;		/* Error */
					}
					else
					{
						/* Set the timer waiting for the response for this tcid */
						//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
						TRPT_Set_Timer(index);
						*mem = 0;  /* free the received TPDU */
					}
				}
				else
				{
	    			if (!t_tab_tcid[index].tag)
					{
						/* Set the Polling Timer */
	    				/* ===================== */
						//t_tab_tcid[index].tmpollid = ITC_SetTimer((unsigned short)(TIMEPOLL + t_tab_tcid[index].tcid), TM_POLLING);/* do nothing if the timer can't be created */
						TRPT_Set_PollTimer(index);

						/* Set the new state */
						t_tab_tcid[index].state = ACTIVE;

#ifdef FASTALLOC
						ITC_Free(t_msg.tpdu.ptpdu);		/* free memory since no TPDU is sent, only a timeout */
#else
						ITC_DynFree(t_msg.tpdu.ptpdu);	/* free memory since no TPDU is sent, only a timeout */
#endif
					}
					else
						/* request_t_c and delete_t_c : garbage the polling */
						t_tab_tcid[index].tag = 0;
				}
			}   /* end of t_sb(0) processing  */
			break;	/* end of t_sb processing */

		case delete_t_c :
			PrintDbg(("TRPT_Polling : delete_t_c Received\n"));

        	/* Verify parameters  : only tcid, already made */

			// t_sb following delete_t_c management */
			t_tab_tcid[index].tag = delete_t_c;

			/* send a d_t_c_reply  : the same buffer tpmsg.tpdu is used for TPDU */
			/* ================== */
			t_header.trans		= (TRANS << 4) | LINK;
			t_header.type     	= TPDU;
			t_header.len      	= sizeof(st_tpdu);
			t_msg.tpdu.slot_id  = t_tab_tcid[index].slot_id;
			t_msg.tpdu.tcid     = t_tab_tcid[index].tcid;
			len  	= TRPT_Code(d_t_c_reply, t_tab_tcid[index].tcid, NULL, t_msg.tpdu.ptpdu);
    		if (len == 0) // if (len < 0)
    		{
				PrintErr(("TRPT_Polling : Can't code a d_t_c_reply for slot %d !!!\n", t_tab_tcid[index].slot_id));
				return 0;		/* Error */
			}
			else
			{
				t_msg.tpdu.len = len;
			}

		    /* Send the message */
    		if (! TRPT_Send_Msg())
    		{
				PrintErr(("TRPT_Polling : Can't send the d_t_c_reply for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
				iRet = 0;		/* Error */
			}
			else
				/* send the SS_TC_DELETED message */
				/* ============================== */
				iRet = TRPT_Disc(index);
				if (iRet)
		    		/* Set the new state */
					t_tab_tcid[index].state = IDLE;
			break;	/* end of delete_t_c processing */

		case request_t_c :
			PrintDbg(("TRPT_Polling : request_t_c Received\n"));

        	/* Verify parameters  : only tcid, already made */

        	/* kill pending timer 	*/
			TRPT_Kill_Timer(index);

			// t_sb following request_t_c management */
			t_tab_tcid[index].tag = request_t_c;

			/* Add a new instance of the Transport State Diagram for the new transport connection 			*/
			/* ==================================================================================			*/
			new_index = TRPT_GetIndex();	/* get the new instance for the transport state diagram 		*/
			if (new_index < 0)
			{
				/* can't get new instance, a t_c_error is sent to the module */
				/* ========================================================= */
				infos.errortc.error_code = NOTRANS;

				t_header.trans		= (TRANS << 4) | LINK;
				t_header.type     	= TPDU;
				t_header.len      	= sizeof(st_tpdu);
				t_msg.tpdu.slot_id  = t_tab_tcid[index].slot_id;
				t_msg.tpdu.tcid     = t_tab_tcid[index].tcid;
				len      = TRPT_Code(t_c_error, t_tab_tcid[index].tcid, &infos, t_msg.tpdu.ptpdu);
    			if (len == 0) // if (len < 0)
    			{
					PrintErr(("TRPT_Polling : Can't code a t_c_error for slot %d !!!\n", t_tab_tcid[index].slot_id));
					TRPT_FreeIndex(new_index);		/* free the new instance */
					return 0;						/* Error */
				}
				else
				{
					t_msg.tpdu.len = len;
				}

			    /* Send the message */
    			if (! TRPT_Send_Msg())
    			{
					PrintErr(("TRPT_Polling : Can't send the t_c_error for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
					TRPT_FreeIndex(new_index);		/* free the new instance */
					iRet = 0;						/* Error */
				}
				else
				{
					/* Set the timer waiting for the response for this tcid */
					//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
					TRPT_Set_Timer(index);
				}

	    		/* Don't change the current state */
				return 1;
            }	/* end of t_c_error management */

			infos.newtc.new_t_c_id = t_tab_tcid[new_index].tcid;
			t_tab_tcid[new_index].slot_id = t_tab_tcid[index].slot_id;

			/* send a new_t_c  : the same buffer tpmsg.tpdu is used for TPDU */
			/* ============== */
			t_header.trans		= (TRANS << 4) | LINK;
			t_header.type     	= TPDU;
			t_header.len      	= sizeof(st_tpdu);
			t_msg.tpdu.slot_id  = t_tab_tcid[index].slot_id;
			t_msg.tpdu.tcid     = t_tab_tcid[index].tcid;
			len  	= TRPT_Code(new_t_c, t_tab_tcid[index].tcid, &infos, t_msg.tpdu.ptpdu);
    		if (len == 0) // if (len < 0)
    		{
				PrintErr(("TRPT_Polling : Can't code a new_t_c for slot %d !!!\n", t_tab_tcid[index].slot_id));
				TRPT_FreeIndex(new_index);		/* free the new instance */
				return 0;						/* Error */
			}
			else
			{
				t_msg.tpdu.len = len;
			}

		    /* Send the message */
    		if (! TRPT_Send_Msg())
    		{
				PrintErr(("TRPT_Polling : Can't send the new_t_c for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
				TRPT_FreeIndex(new_index);		/* free the new instance */
				iRet = 0;						/* Error */
			}
			else
			{
#ifdef WIN32
#ifdef _DEBUG
				ITC_Sleep(500);
#else
				ITC_Sleep(50);
#endif
#endif

				/* Manage the second transport connection */
				/* ====================================== */
				iRet = TRPT_Idle(new_index, TP_CONNECT, mem);

				/* Set the polling timer for this tcid */
				//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
				TRPT_Set_Timer(index);

    			/* Don't change the state : waiting for t_sb */
			}
            break;	/* end of request_t_c processing */

		case SPDU :
			PrintDbg(("TRPT_Polling : SPDU Received\n"));

			/* Put the spdu in fifo */
			iRet = TRPT_PutFifo(index, evt, &(t_msg.spdu));

			/* the state is unchanged */
			break;	/* end of spdu processing */

		case SS_DELETE_TC :
			PrintDbg(("TRPT_Polling : SS_DELETE_TC Received\n"));

			/* Put the message in fifo */
			iRet = TRPT_PutFifo(index, evt, NULL);

			/* the state is unchanged */
			break;	/* end of spdu processing */

		case TIMEOUT :
			PrintDbg(("TRPT_Polling : TIMEOUT Received\n"));

			/* Get free buffer for TPDU message */
#ifdef FASTALLOC
			t_msg.tpdu.ptpdu = ITC_Alloc();						/* length = sizeof(delete_t_c) */
#else
			t_msg.tpdu.ptpdu = ITC_DynAlloc(S_DELETE_T_C);		/* length = sizeof(delete_t_c) */
#endif
			if (t_msg.tpdu.ptpdu == NULL)
			{
				PrintErr(("TRPT_Polling : can't allocate buffer for coding TPDU !!!\n"));
				return 0;	/* Error */
			}

			/* send a delete_t_c */
			/* ================= */
			t_header.trans		= (TRANS << 4) | LINK;
			t_header.type     	= TPDU;
			t_header.len      	= sizeof(st_tpdu);
			t_msg.tpdu.slot_id  = t_tab_tcid[index].slot_id;
			t_msg.tpdu.tcid     = t_tab_tcid[index].tcid;
			len  	= TRPT_Code(delete_t_c, t_tab_tcid[index].tcid, NULL, t_msg.tpdu.ptpdu);
    		if (len == 0) // if (len < 0)
    		{
				PrintErr(("TRPT_Polling : Can't code a delete_t_c for slot %d !!!\n", t_tab_tcid[index].slot_id));
				*mem = 0;		/* free memory */
				return 0;		/* Error */
			}
			else
			{
				t_msg.tpdu.len = len;
			}

		    /* Send the message */
    		if (! TRPT_Send_Msg())
    		{
				PrintErr(("TRPT_Polling : Can't send the delete_t_c for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
				*mem = 0;		/* free memory */
				iRet = 0;		/* Error */
			}
			else
			{
				/* Set the timer waiting for the response for this tcid */
				//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
				TRPT_Set_Timer(index);

	    		/* Set the new state */
				t_tab_tcid[index].state = DELETION;
			}
            break;	/* end of TIMEOUT processing */

		case TIMEPOLL :
			PrintDbg(("TRPT_Polling : TIMEPOLL Received\n"));
			break;

		default :	/* protocol error, release the transport connection */
			PrintErr(("TRPT_Polling : protocol error . event 0x%x received in %d state !!!\n", evt, t_tab_tcid[index].state));

			/* release the transport connection */
			/* ================================ */
			if (evt < MSG_MAXVAL )
				alloc = 1;      /* not TPDU or SPDU : memory must be allocated to send TPDU */
			else
				alloc = 0;		/* TPDU or SPDU : memory already exist */
			iRet = TRPT_Release(index, alloc, mem);
			if (iRet)
		    	/* Set the new state */
				t_tab_tcid[index].state = DELETION;
			break;	/* end of default processing */
		}

	return iRet;
}



/* ======================================================================== */
/* NAME : TRPT_Pollmore					 									*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the POLLMORE state					*/
/*	a transport connection exists and uncomplete data have been sent 		*/
/*	by the host : wait for an answer to t_data_more				 			*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received TPDU or NULL		*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		mem :		memory management										*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*	If the calling function is TRPT_Tpdu() or SRPT_Tpdu() and 				*/
/*		if the return code is 0 then mem is assigned with 0 in TRPT_Tpdu() 	*/
/*		or TRPT_Spdu() --> free t_psmg.tpdu									*/
/*									  										*/
/* ======================================================================== */
int TRPT_Pollmore(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;		/* return code 												*/
int alloc;		/* memory management										*/

	*mem = 1;	/* default 													*/

	switch(evt) {

		case t_sb :
			PrintDbg(("TRPT_Pollmore : t_sb Received\n"));

			/* a module can't ask for sending data while the host sends its own data with t_data_more */
			pinfos->sbtc.SB_value = 0;	/* ignore a t_sb(1) */

			/* same process as POLLING state t_sb(0) */
			iRet = TRPT_Polling(index, evt, pinfos, mem);
			break;

		case TP_DISCONNECT :
			PrintDbg(("TRPT_Pollmore : TP_DISCONNECT Received\n"));

		case TIMEOUT :
			PrintDbg(("TRPT_Pollmore : TIMEOUT Received\n"));

		case delete_t_c :
			PrintDbg(("TRPT_Pollmore : delete_t_c Received\n"));

		case SS_DELETE_TC :
			PrintDbg(("TRPT_Pollmore : SS_DELETE_TC Received\n"));

		case SPDU :
			PrintDbg(("TRPT_Pollmore : SPDU Received\n"));

			/* same process that for the POLLING state */
			iRet = TRPT_Polling(index, evt, pinfos, mem);
			break;

		default :	/* protocol error, release the transport connection (in particular : request_t_c) */
			PrintErr(("TRPT_Pollmore : protocol error . event 0x%x received in %d state !!!\n", evt, t_tab_tcid[index].state));

			/* release the transport connection */
			/* ================================ */
			if (evt < MSG_MAXVAL )
				alloc = 1;      /* not TPDU or SPDU : memory must be allocated to send TPDU */
			else
				alloc = 0;		/* TPDU or SPDU : memory already exist */
			iRet = TRPT_Release(index, alloc, mem);
			if (iRet)
		    	/* Set the new state */
				t_tab_tcid[index].state = DELETION;
			break;	/* end of default processing */
		}

	return iRet;
}



/* ======================================================================== */
/* NAME : TRPT_Reception				 									*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the RECEPTION state					*/
/*	a transport connection exists and a t_rcv have been sent by the host :	*/
/*	wait for a t_data_last or t_data_more									*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received TPDU or NULL		*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		mem :		memory management										*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*	If the calling function is TRPT_Tpdu() or TRPT_Spdu() and 				*/
/*		if the return code is 0 then mem is assigned with 0 in TRPT_Tpdu() 	*/
/*		or TRPT_Spdu() --> free t_psmg.tpdu									*/
/*									  										*/
/* ======================================================================== */
int TRPT_Reception(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;		/* return code 												*/
int alloc;		/* memory management										*/
unsigned long len;

	*mem = 1;		/* default */
	iRet = 1;		/* Success */

	switch(evt) {
		case TP_DISCONNECT :
			PrintDbg(("TRPT_Reception : TP_DISCONNECT Received\n"));

        	/* kill pending timer */
			TRPT_Kill_Timer(index);

			/* send the SS_TC_DELETED message */
			/* ============================== */
			iRet = TRPT_Disc(index);
			if (iRet)
		    	/* Set the new state */
				t_tab_tcid[index].state = IDLE;
			break;	/* end of TP_DISCONNECT processing */

		case t_data_last :
			PrintDbg(("TRPT_Reception : t_data_last Received\n"));

        	/* kill pending timer */
			TRPT_Kill_Timer(index);

			if (t_tab_tcid[index].rlen == 0)
			{	/* t_data_last without t_data_more */
				/* Get free variable size buffer for SPDU message (t_msg1 since t_msg points the received TPDU) */
				/* The size's buffer is variable because it can be greater than the fixed size returned by ITC_Alloc */

				t_msg1.spdu.pspdu = ITC_DynAlloc(pinfos->datatc.len);
				if (t_msg1.spdu.pspdu == NULL)
				{
					PrintErr(("TRPT_Reception : can't allocate buffer for coding SPDU !!!\n"));
					return 0;	/* Error */
				}
			}
            else
            {	/* append new memory to the already allocated memory */
#ifdef __ITC_USE_REALLOC
				t_msg1.spdu.pspdu = ITC_DynReAlloc(t_tab_tcid[index].rpspdu, t_tab_tcid[index].rlen + pinfos->datatc.len);
#else
{
				unsigned char		*pt;
				pt = ITC_DynAlloc(t_tab_tcid[index].rlen + pinfos->datatc.len);
				if (pt == NULL)
				{
					PrintErr(("TRPT_Reception : can't reallocate buffer for coding SPDU !!!\n"));
					return 0;	/* Error */
				}

				VK_memcpy(pt, t_tab_tcid[index].rpspdu, t_tab_tcid[index].rlen);
				t_msg1.spdu.pspdu = pt;
}
#endif
				if (t_msg1.spdu.pspdu == NULL)
				{
					PrintErr(("TRPT_Reception : can't reallocate buffer for coding SPDU !!!\n"));
					return 0;	/* Error */
				}
				else
					t_tab_tcid[index].rpspdu = t_msg1.spdu.pspdu;
            }

			/* Send a SPDU to SESS Task */
			/* ======================== */
			t_header1.trans		= (TRANS << 4) | SESS;
			t_header1.type     	= SPDU;
			t_header1.len      	= sizeof(st_spdu);
			t_msg1.spdu.tcid    = t_tab_tcid[index].tcid;
			t_msg1.spdu.len     = pinfos->datatc.len + t_tab_tcid[index].rlen;

			VK_memcpy(t_msg1.spdu.pspdu + t_tab_tcid[index].rlen, pinfos->datatc.pbuff, pinfos->datatc.len);

		    /* Send the message */
    		if (! TRPT_Send_Msg1())
    		{
				PrintErr(("TRPT_Reception : Can't send the SPDU for tcid %d to SESS Task !!!\n", t_tab_tcid[index].tcid));
				ITC_DynFree(t_msg1.spdu.pspdu);	  	/* free memory  */
				iRet = 0;							/* Error 		*/
			}
			else
			{
				/* Set the timer waiting for the following t_sb */
				//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
				TRPT_Set_Timer(index);
				t_tab_tcid[index].rlen   = 0;
				t_tab_tcid[index].rpspdu = NULL;	/* spdu has been entirely sent  */
				*mem = 0;							/* *mem = 0 to free tpdu memory */
			}
			/* the state is unchanged : wait for t_sb */
			break;	/* end of t_data_last processing  */

		case t_data_more :
			PrintDbg(("TRPT_Reception : t_data_more Received\n"));

        	/* kill pending timer */
			TRPT_Kill_Timer(index);

			if (t_tab_tcid[index].rlen == 0)
			{	/* first t_data_more for a new spdu */
				/* Get free variable size buffer for SPDU message (t_msg1 since t_msg points the received TPDU) */
				/* The size's buffer is variable because it can be greater than the fixed size returned by ITC_Alloc */
				t_msg1.spdu.pspdu = ITC_DynAlloc(pinfos->datatc.len);
				if (t_msg1.spdu.pspdu == NULL)
				{
					PrintErr(("TRPT_Reception : can't allocate buffer for coding SPDU !!!\n"));
					return 0;	/* Error */
				}
				else
					t_tab_tcid[index].rpspdu = t_msg1.spdu.pspdu;
			}
            else
            {	/* append new memory to the already allocated memory */
#ifdef __ITC_USE_REALLOC
				t_msg1.spdu.pspdu = ITC_DynReAlloc(t_tab_tcid[index].rpspdu, t_tab_tcid[index].rlen + pinfos->datatc.len);
#else
{
				unsigned char		*pt;
				pt = ITC_DynAlloc(t_tab_tcid[index].rlen + pinfos->datatc.len);
				if (pt == NULL)
				{
					PrintErr(("TRPT_Reception : can't allocate buffer for coding SPDU !!!\n"));
					return 0;	/* Error */
				}

				VK_memcpy(pt, t_tab_tcid[index].rpspdu, t_tab_tcid[index].rlen);
				t_msg1.spdu.pspdu = pt;
}
#endif
				if (t_msg1.spdu.pspdu == NULL)
				{
					PrintErr(("TRPT_Reception : can't allocate buffer for coding SPDU !!!\n"));
					return 0;	/* Error */
				}
				else
					t_tab_tcid[index].rpspdu = t_msg1.spdu.pspdu;
            }

			VK_memcpy(t_tab_tcid[index].rpspdu + t_tab_tcid[index].rlen, pinfos->datatc.pbuff, pinfos->datatc.len);
			t_tab_tcid[index].rlen += pinfos->datatc.len;

			/* Set the timer waiting for the following t_sb */
			//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
			TRPT_Set_Timer(index);

			*mem = 0;	/* *mem = 0 to free tpdu memory */
			/* the state is unchanged : wait for t_sb   */
			break;	/* end of t_data_more processing    */

		case t_sb :
			PrintDbg(("TRPT_Reception : t_sb Received\n"));

        	/* kill pending timer */
			TRPT_Kill_Timer(index);

			if (pinfos->sbtc.SB_value == DATA_AVAILABLE)
			{   /* t_sb(1) received : data available on module */
				/* send a t_rcv  : the same buffer tpmsg.tpdu is used for TPDU */
				/* ============ */
				t_header.trans		= (TRANS << 4) | LINK;
				t_header.type     	= TPDU;
				t_header.len      	= sizeof(st_tpdu);
				t_msg.tpdu.slot_id  = t_tab_tcid[index].slot_id;
				t_msg.tpdu.tcid     = t_tab_tcid[index].tcid;
				len  	= TRPT_Code(t_rcv, t_tab_tcid[index].tcid, NULL, t_msg.tpdu.ptpdu);
    			if (len == 0) // if (len < 0)
    			{
					PrintErr(("TRPT_Reception : Can't code a t_rcv for slot %d !!!\n", t_tab_tcid[index].slot_id));
					return 0;		/* Error */
				}
				else
				{
					t_msg.tpdu.len = len;
				}

			    /* Send the message */
    			if (! TRPT_Send_Msg())
    			{
					PrintErr(("TRPT_Reception : Can't send the t_rcv for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
					iRet = 0;		/* Error */
				}
				else
				{
					/* Set the timer waiting for the response for this tcid */
					//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
					TRPT_Set_Timer(index);

		    		/* the new state is unchanged */
				}
			}	/* end of t_sb(1) processing */
			else
			{   /* t_sb(0) received : no data available on module */
				if (! t_tab_tcid[index].tag)
				{
    				/* Set the Polling Timer */
    				/* ===================== */
					//t_tab_tcid[index].tmpollid = ITC_SetTimer((unsigned short)(TIMEPOLL + t_tab_tcid[index].tcid), TM_POLLING);/* do nothing if the timer can't be created */
					TRPT_Set_PollTimer(index);

					*mem = 0;							/* *mem = 0 to free tpdu memory */

					/* Set the new state */
					t_tab_tcid[index].state = ACTIVE;
				}
				else
					/* delete_t_c : garbage the polling */
					t_tab_tcid[index].tag = 0;
			}   /* end of t_sb(0) processing  */
			break;	/* end of t_sb processing */

		case TIMEOUT :
			PrintDbg(("TRPT_Reception : TIMEOUT Received\n"));

		case delete_t_c :
			PrintDbg(("TRPT_Reception : delete_t_c Received\n"));

		case SS_DELETE_TC :
			PrintDbg(("TRPT_Reception : SS_DELETE_TC Received\n"));

		case SPDU :
			PrintDbg(("TRPT_Reception : SPDU Received\n"));

			/* same process that for the POLLING state */
			iRet = TRPT_Polling(index, evt, pinfos, mem);
			break;

		default :
			PrintErr(("TRPT_Reception : protocol error . event 0x%x received in %d state !!!\n", evt, t_tab_tcid[index].state));

			/* release the transport connection */
			/* ================================ */
			if (evt < MSG_MAXVAL )
				alloc = 1;      /* not TPDU or SPDU : memory must be allocated to send TPDU */
			else
				alloc = 0;		/* TPDU or SPDU : memory already exist */
			iRet = TRPT_Release(index, alloc, mem);
			if (iRet)
		    	/* Set the new state */
				t_tab_tcid[index].state = DELETION;
			break;	/* end of default processing */
		}

	return iRet;
}



/* ======================================================================== */
/* NAME : TRPT_Deletion					 									*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the DELETION state					*/
/*	The transport layer waits for a d_t_c_reply								*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received TPDU or NULL		*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		mem :		memory management										*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*	If the calling function is TRPT_Tpdu() or SRPT_Tpdu() and 				*/
/*		if the return code is 0 then mem is assigned with 0 in TRPT_Tpdu() 	*/
/*		or TRPT_Spdu() --> free t_psmg.tpdu									*/
/*									  										*/
/* ======================================================================== */
int TRPT_Deletion(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;	/* return code */

	*mem = 1;		/* default */

	switch(evt) {
		case TIMEOUT :
			PrintDbg(("TRPT_Deletion : TIMEOUT Received\n"));

			/* send the SS_TC_DELETED message */
			iRet = TRPT_Disc(index);
			if (iRet)
		    	/* Set the new state */
				t_tab_tcid[index].state = IDLE;
			break;	/* end of TIMEOUT processing */

		case TP_DISCONNECT :
			PrintDbg(("TRPT_Deletion : TP_DISCONNECT Received\n"));

        	/* kill pending timer */
			TRPT_Kill_Timer(index);

			/* send the SS_TC_DELETED message */
			iRet = TRPT_Disc(index);
			if (iRet)
		    	/* Set the new state */
				t_tab_tcid[index].state = IDLE;
			break;	/* end of TP_DISCONNECT processing */

		case d_t_c_reply :
			PrintDbg(("TRPT_Deletion : d_t_c_reply Received\n"));

        	/* Verify parameters  : only tcid, already made */

        	/* kill pending timer 	*/
			TRPT_Kill_Timer(index);

			/* send the SS_TC_DELETED message */
			iRet = TRPT_Disc(index);
			if (iRet)
	    		/* Set the new state */
				t_tab_tcid[index].state = IDLE;
			*mem = 0;	/* free the TPDU buffer		 */
            break;	/* end of d_t_c_reply processing */

		case t_sb :
			PrintDbg(("TRPT_Deletion : Garbage previous received t_sb\n"));
			iRet = 1;
			break;

		default :	/* protocol error, ignore the event */
			PrintErr(("TRPT_Deletion : protocol error . event 0x%x received in %d state !!!\n", evt, t_tab_tcid[index].state));
			iRet = 0;
			break;
		}

	return iRet;
}





/* ======================================================================== */
/* NAME : TRPT_Disc						 									*/
/* 									    									*/
/* DESCRIPTION : Action for the current transport connection associated with*/
/*	a received TP_DISCONNECT or delete_tc  									*/
/*	- Notify the SESSION Task for the current transport connection			*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*									  										*/
/* ======================================================================== */
int TRPT_Disc(int index)
{
	/* make the SS_TC_DELETED message */
	t_header.trans		= (TRANS << 4) | SESS;
	t_header.type     	= SS_TC_DELETED;
	t_header.len      	= sizeof(st_deleted);
	t_msg.deleted.tcid    = t_tab_tcid[index].tcid;

	PrintDbg(("TRPT_Disc Start\n"));

    /* Send the message */
	if (! TRPT_Send_Msg())
	{
		PrintErr(("TRPT_Disc : Can't send the SS_TC_DELETED for tcid %d to SESS Task !!!\n", t_tab_tcid[index].tcid));
		return 0;		/* Error */
	}
	else return 1;		/* Success */
}


/* ======================================================================== */
/* NAME : TRPT_Release					 									*/
/* 									    									*/
/* DESCRIPTION : Action for the current transport connection when there is	*/
/*	a protocol error 														*/
/*		- Release the transport connection									*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		alloc :		1 : memory allocation must be made						*/
/*					0 : take the current buffer								*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		mem :		memory management										*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*	the state is unchanged													*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*									  										*/
/* ======================================================================== */
int TRPT_Release(int index, int alloc, int *mem)
{
int iRet;	/* return code */
unsigned long len;

	*mem = 1;		/* default */
	iRet = 1;

	PrintDbg(("TRPT_Release Start\n"));

	if (alloc)
	{
		/* Get free buffer for TPDU message */
#ifdef FASTALLOC
		t_msg.tpdu.ptpdu = ITC_Alloc();						/* length = sizeof(delete_t_c) */
#else
		t_msg.tpdu.ptpdu = ITC_DynAlloc(S_DELETE_T_C);		/* length = sizeof(delete_t_c) */
#endif
		if (t_msg.tpdu.ptpdu == NULL)
		{
			PrintErr(("TRPT_Release : can't allocate buffer for coding TPDU !!!\n"));
			return 0;	/* Error */
		}
	}

	/* make the delete_t_c message */
	t_header.trans		= (TRANS << 4) | LINK;
	t_header.type     	= TPDU;
	t_header.len      	= sizeof(st_tpdu);
	t_msg.tpdu.slot_id = t_tab_tcid[index].slot_id;
	t_msg.tpdu.tcid    = t_tab_tcid[index].tcid;
	len = TRPT_Code(delete_t_c, t_tab_tcid[index].tcid, NULL, t_msg.tpdu.ptpdu);
	if (len == 0) // if (len < 0)
	{
		PrintErr(("TRPT_Release : Can't code a delete_t_c for slot %d !!!\n", t_tab_tcid[index].slot_id));
		*mem = 0;		/* t_msg.tpdu.ptpdu must be released */
		return 0;		/* Error */
	}
	else
	{
		t_msg.tpdu.len = len;
	}

    /* Send the message */
	if (! TRPT_Send_Msg())
	{
		PrintErr(("TRPT_Release : Can't send the delete_t_c for slot %d to LINK Task !!!\n", t_tab_tcid[index].slot_id));
		*mem = 0;		/* t_msg.tpdu.ptpdu must be released */
		iRet = 0;		/* Error */
	}
	else
		/* Set the timer waiting for the response for this tcid */
		//t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);/* do nothing if the timer can't be created */
		TRPT_Set_Timer(index);

	/* Send IHM_WARNING to IHM Task */

	return iRet;
}




