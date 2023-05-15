/****************************************************************************/
/*                                                                          */
/*   FILE:    link.c	                                                    */
/*   DATE:    25/06/97                                                      */
/*   PURPOSE: DVB-CI Link layer - entry point into LINK task          		*/
/*			- Initialization Module				    					    */
/*			- Main Module					    							*/
/*		    - Ending Module												    */
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version	Date	  	    Written by	     Notes	    	   			*/
/*	 1.0	    25/06/1997      SCM              Initial implementation 	*/
/*	 1.04		31/07/1997		SCM         	 Acces to PHYSICAL Task		*/
/*								instead of PHYSICAL Driver					*/
/*   1.05       28/12/1998      SCM              In function LINK_NewTcid() */
/*                                            and function LINK_FreeIndex() */
/*              changed "LINK_RazFifo()" to "LINK_ResetFifo()"				*/
/*                                                                   	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*																		    */
/****************************************************************************/

#include "ci.h"

#include "link.h"
#include "itc.h"			/* Inter-Task Communication */
#include "linkstat.h"		/* State Diagram Module		*/
#include "linkfifo.h"		/* Fifo Management Module	*/
#include "pal_ci.h"

/*--------------------------------------------------------------------------*/
/* Global Variables		                                            		*/
/*--------------------------------------------------------------------------*/
xmsg  ln_infos;				 /* received informations	 					*/
xmsg  ln_infos1;			 /* other informations	 						*/
lslot ln_tab_slot[PAL_MAX_CISLOT_NUMBER];/* slot table									*/
ltcid ln_tab_tcid[MAXTCID];  /* tcid table   								*/
LFIFO ln_tab_fifo[MAXTCID];  /* FIFO table									*/

/*--------------------------------------------------------------------------*/
/* Local Variables		                                            		*/
/*--------------------------------------------------------------------------*/
static unsigned char ln_evt;		/* current event 						*/

/*--------------------------------------------------------------------------*/
/* External Variables		                                           		*/
/*--------------------------------------------------------------------------*/

/* ======================================================================== */
/* NAME : link								    							*/
/* 									    									*/
/* DESCRIPTION : entry point into LINK task :				    			*/
/*		 - Initialization Module				   							*/
/*		 - Main Module						    							*/
/*		 - Ending Module				         							*/
/*	The Main Module is an infinite loop :								    */
/*		 - wait events (messages from TRANS task or PHYSICAL Task		 	*/
/*		 - read the first received event  			   						*/
/*		 - call the LINK State Diagram LINK_State	  	        			*/
/*	                                                                  		*/
/* 									 										*/
/* INPUT PARAMETERS :							    						*/
/*									    									*/
/* OUTPUT PARAMETERS :							  							*/
/*									   										*/
/* RETURN CODE								   								*/
/*	 0    	 	success														*/
/*  -1			error														*/
/*									   										*/
/* ======================================================================== */
void link(void *vpParam)
{
unsigned char trans;	/* sender/receiver of received event */

	/* Call the Initialization Module */

	if (! LINK_Init()) {
		PrintErr(("link : LINK_Init error !!!\n"));
//		return -1;		/* error while Initialization */
	}

	/* Main Module */
	/* =========== */
	while(1)		/* infinite loop */
	{
		/* Blocking Wait for event */
#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
		if (! ITC_WaitEvt(ITC_INFINITE, &ln_evt, &trans, &ln_infos, CI_TASK_ID_LINK))
#else
		if (! ITC_WaitEvt(ITC_INFINITE, &ln_evt, &trans, &ln_infos))
#endif
		{
			PrintErr(("link : ITC_WaitEvt . Error in return code !!!\n"));

			/* Ending Module */
			LINK_End();
			return /*-1*/;
		}

		/* Reading the event (MESSAGE) */
		if (ln_evt == MESSAGE)
		{
			/* Call the link State Diagram */
			LINK_State();
		}
		else
		{
			PrintErr(("link : ITC_WaitEvt . Error in received event !!!\n"));

			/* Ending Module */
			LINK_End();
			return /*-1*/;
		}
	}

	/* Call the Ending Module : should never get executed */
	LINK_End();
	return /*0*/;
}
/* End of Main Module */
/* ================== */


/* ======================================================================== */
/* NAME : LINK_Init							    							*/
/* 									    									*/
/* DESCRIPTION : Initialization Module					    				*/
/*		 link variables initialization			    						*/
/*		 link State Diagram initialization 	            					*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*	1   	 	successful initialization			   						*/
/*  0			error while initialization			    					*/
/*									   										*/
/* ======================================================================== */
int LINK_Init(void)
{
int i;

	ln_evt = 0;

	if (! ITC_Init(LINK))
		return 0;

	for (i=0; i < PAL_MAX_CISLOT_NUMBER; i++)
	{
		ln_tab_slot[i].state = NOT_USED;
		ln_tab_slot[i].slot  = 0;
	}

	for (i=0; i < MAXTCID; i++)
	{
		ln_tab_tcid[i].state = NOT_USED;
		ln_tab_tcid[i].slot  = 0;
		ln_tab_tcid[i].tcid  = 0;
	}

	return 1;
}
/* End of Initialization Module */
/* ============================ */


/* ======================================================================== */
/* NAME : LINK_End							    							*/
/* 									    									*/
/* DESCRIPTION : Ending Module							    				*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*									   										*/
/* ======================================================================== */
void LINK_End(void)
{
	ITC_End();

	return;
}
/* End of Initialization Module */
/* ============================ */


/* ======================================================================== */
/* NAME : LINK_State													    */
/* 									    									*/
/* DESCRIPTION : Entry point of the link State Diagram Module				*/
/*		Read the event					       							    */
/*		Read the message							    				    */
/*		Find the current TCID and the matching state diagram index 			*/
/*		Call the current transition matches the received event	   			*/
/* 									   										*/
/* INPUT PARAMETERS :							   							*/
/*	ln_evt provides the received event (MESSAGE)				   			*/
/*	ln_tab_tcid[i].state gives the current state of the State Diagram  		*/
/*	for the current TCID (index i)					    					*/
/*									   										*/
/* OUTPUT PARAMETERS :							    						*/
/*	ln_tab_tcid[i].state is updated					    					*/
/*									  										*/
/* RETURN CODE :							  							    */
/*									 									    */
/* ======================================================================== */
void LINK_State(void)
{
int iRet;			/* return code 		  		   */
unsigned char *pt;	/* pointer to allocated memory */
#ifdef FASTALLOC
int dyn;			/* 1 if TPDU received with dynamically allocated memory */

	dyn  = 0;
#endif

	iRet = 0;
	pt = NULL;	// added by jhcho

	if (ln_evt == MESSAGE)
	{
		switch (ln_header.type) {
#ifdef FASTALLOC
			case TPDUDYN :		/* TPDU with dynamically allocated memory */
				dyn = 1;
#endif
			case TPDU :			/* Link <--> Trans */
				pt = ln_msg.tpdu.ptpdu;
				iRet = LINK_Tpdu();
		    	break;

			case LPDU :	    	/* Link <--> Physical */
				pt = ln_msg.lpdu.plpdu;
				iRet = LINK_Lpdu();
				break;

			case LN_ACK :       /* Link <--- Physical */
				iRet = LINK_Ack();
				break;

			case LN_CONNECT :   /* Link <--- Physical */
				iRet = LINK_Connect();
				break;

			case LN_ERROR :     /* Link <--- Physical */
				iRet = LINK_Error();
				break;

			case LN_DISCONNECT :/* Link <--- Physical */
				iRet = LINK_Disconnect();
				break;

			default :			/* unknown message */
				PrintErr(("LINK_State : Unknown message is ignored 0x%x !!!\n", ln_header.type));
				break;
			}

//		PrintDbg(("LINK STATE TRANSITION MESSAGE TYPE <0x%x> : Ret = %d\n", ln_header.type,iRet));
		if (!iRet && pt)	// modified by jhcho
		{
#ifdef FASTALLOC
			if (dyn)
				ITC_DynFree(pt);/* free memory used with last received message */
			else
				ITC_Free(pt);  	/* free memory used with last received message */
#else
			ITC_DynFree(pt);	/* free memory used with last received message */
#endif
		}
	}
	else
	{
		PrintErr(("LINK_State : Unknown event is ignored 0x%x !!!\n", ln_evt));
	}

	return;
}
/* End of link State Diagram */
/* ========================= */


/* ======================================================================== */
/* NAME : LINK_Connect						    							*/
/* 									    									*/
/* DESCRIPTION : LINK State Diagram : LN_CONNECT transition received 		*/
/*		- Create a new state diagram										*/
/*		- Call the action													*/
/*		- Update the state diagram											*/
/*									    									*/
/* INPUT PARAMETERS :							    						*/
/*		ln_msg.lconn contains the LN_CONNECT message received from PHYSICAL	*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*		ln_tab_tcid[] updated												*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int LINK_Connect(void)
{
int index;	/* index of ln_tab_tcid[] for the current module				*/
int mem;	/* memory management : return value								*/
unsigned short slot_id;	/* current slot_id									*/

	PrintDbg(("LINK_Connect : Start <Physical Status : %d>\n", ln_msg.lconn.status));

	if (ln_msg.lconn.status == LN_OK)
	{
		/* Add a new instance of the Link State Diagram for the new module					 			*/
		index = LINK_GetIndex();		/* get the new instance for the link state diagram 		*/
		if (index < 0)
		{
			/* Send IHM_WARNING to IHM Task */
			PrintDbg(("LINK_Connect : can't get new instance or module already connected\n"));
			return 0;	/* can't get new instance or module already connected, the request is ignored 	*/
	    }
		slot_id = ln_msg.lconn.slot_id;

		/* Call the link state diagram */
		if (!LINK_Action(index, LN_CONNECT, 0, &mem))
			LINK_FreeIndex(index, slot_id);		/* error : free the new instance */
    }
    else
    {	/* no DVB module, warning 		*/
		/* Send IHM_WARNING to IHM Task */
    }
	return 1;
}


/* ======================================================================== */
/* NAME : LINK_Disconnect						    						*/
/* 									    									*/
/* DESCRIPTION : Link State diagram : LN_DISCONNECT received 				*/
/*		find the module														*/
/*		call the action														*/
/*		free the state diagram												*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		ln_msg.disc contains the LN_DISCONNECT message received from 		*/
/*		PHYSICAL Task	 													*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int LINK_Disconnect(void)
{
int index;	/* index of ln_tab_slot[] for the current module connection  	*/
int mem;	/* memory management : return value								*/
unsigned short slot_id;	/* current slot_id									*/

	index = LINK_GetSlot(ln_msg.disc.slot_id);
	if (index >= 0)
	{	/* module found */
		slot_id = ln_msg.disc.slot_id;

		LINK_Action(index, LN_DISCONNECT, 0, &mem);

		/* free the instance of state diagram and all associated tcid */
		LINK_FreeIndex(index, slot_id);
	}

	return 1;
}


/* ======================================================================== */
/* NAME : LINK_Error							    						*/
/* 									    									*/
/* DESCRIPTION : Link State diagram : LN_ERROR received 					*/
/*		find the module														*/
/*		call the action														*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		ln_msg.error contains the LN_ERROR message received from PHYSICAL   */
/*		Task 																*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int LINK_Error(void)
{
int index;		/* index of ln_tab_slot[] for the current module connection */
int mem;		/* memory management : return value							*/
unsigned short slot_id;	/* current slot_id									*/

	index = LINK_GetSlot(ln_msg.error.slot_id);
	if (index >= 0)
	{
		slot_id = ln_msg.error.slot_id;
		if (LINK_Action(index, LN_ERROR, (int)ln_msg.error.status, &mem))
			/* free the instance of state diagram and all associated tcid if the error is LN_PHYSICAL */
			LINK_FreeIndex(index, slot_id);
	}

	return 1;
}


/* ======================================================================== */
/* NAME : LINK_Lpdu								    						*/
/* 									    									*/
/* DESCRIPTION : Link State diagram : LPDU received 						*/
/*		- Decod the LPDU													*/
/*		- Find the transport connection										*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		ln_msg.lpdu contains the LPDU message received from PHYSICAL Task	*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int LINK_Lpdu(void)
{
int index;	/* index of ln_tab_slot[] for the current module connection  	*/
int index_tcid;	/* index of ln_tab_tcid[] for the current transport connection  */
int mem =0;	/* memory management : return value								*/

	index = LINK_GetSlot(ln_msg.lpdu.slot_id);
	if (index >= 0)
	{   /* find the tcid coded in LPDU and his index into ln_tab_tcid 		*/
	 	index_tcid = LINK_GetTcid(ln_msg.lpdu.slot_id, ln_msg.lpdu.plpdu[0]);
	 	if (index_tcid >= 0)
			LINK_Action(index, LPDU, index_tcid, &mem);
	}
	else
		mem = 0;

	return mem;
}


/* ======================================================================== */
/* NAME : LINK_Ack								    						*/
/* 									    									*/
/* DESCRIPTION : Link State diagram : LN_ACK received 						*/
/*		find the module														*/
/*		call the action														*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		ln_msg.ack contains the LN_ACK message received from PHYSICAL Task  */
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int LINK_Ack(void)
{
int index;		/* index of ln_tab_slot[] for the current module connection  	*/
int index_tcid;	/* index of ln_tab_tcid[] for the current transport connection  */
int mem;		/* memory management : return value								*/

	index = LINK_GetSlot(ln_msg.ack.slot_id);
	if (index >= 0)
	{   /* Get the current tcid saved in ln_tab_slot and find his index			*/
	 	index_tcid = LINK_GetTcid(ln_msg.ack.slot_id, ln_tab_slot[index].tcid);
	 	if (index_tcid >= 0)
			LINK_Action(index, LN_ACK, index_tcid, &mem);
	}

	return 1;
}


/* ======================================================================== */
/* NAME : LINK_Tpdu							    							*/
/* 									    									*/
/* DESCRIPTION : Link State diagram : TPDU received 						*/
/*		- Find the module													*/
/*		- Find the transport connection										*/
/*		- If the tcid exists in ln_tab_tcid, do nothing						*/
/*		- If the tcid don't exist in ln_tab_tcid, create an entry into		*/
/*			ln_tab_tcid since it's a new transport connection (create_t_c	*/
/*			sent by the TRANS task)											*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		ln_msg.tpdu contains the TPDU message received from TRANS Task		*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int LINK_Tpdu(void)
{
int index;		/* index of ln_tab_slot[] for the current module connection  	*/
int tcid_index;	/* index of ln_tab_tcid[] for the current transport connection  */
int mem;		/* memory management : return value								*/

	index = LINK_GetSlot(ln_msg.tpdu.slot_id);
	if (index >= 0)
	{
		tcid_index = LINK_GetTcid(ln_msg.tpdu.slot_id, ln_msg.tpdu.tcid);
		if (tcid_index < 0)
		{
			/* create a new entry for a new transport connection */
			tcid_index = LINK_NewTcid(index, ln_msg.tpdu.slot_id, ln_msg.tpdu.tcid);
			if (tcid_index < 0)
			{	/* no more available transport connection */
				/* Send IHM_WARNING to IHM Task */
				PrintDbg(("LINK_Tpdu : no more available transport connection\n"));
				return 0;
			}
		}
		/* Call the state diagram */
		LINK_Action(index, TPDU, tcid_index, &mem);
		PrintDbg(("LINK_Tpdu : LINK_Action <TPDU:%d>\n", mem));
	}
	else
		mem = 0;

	return mem;
}



/* ======================================================================== */
/* NAME : LINK_GetSlot					 									*/
/* 									    									*/
/* DESCRIPTION : Get the current entry in ln_tab_slot						*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		slot_id : slot identifier											*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		 -1 : entry not found												*/
/*		>=0 : index in ln_tab_slot											*/
/*									  										*/
/* ======================================================================== */
int LINK_GetSlot(unsigned short slot_id)
{
int index;				/* return code : entry found */

	for (index=0; index < PAL_CI_GetNumberOfModule(); index++)
	{
		if ((ln_tab_slot[index].state != NOT_USED) && (ln_tab_slot[index].slot == slot_id))
			return index;	/* entry found 	   */
	}
	return -1;				/* entry not found */
}


/* ======================================================================== */
/* NAME : LINK_GetTcid					 									*/
/* 									    									*/
/* DESCRIPTION : Get the current entry in ln_tab_tcid						*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		slot : slot identifier												*/
/*		tcid : transport connection identifier								*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		 -1 : entry not found												*/
/*		>=0 : index in ln_tab_tcid											*/
/*									  										*/
/* ======================================================================== */
int LINK_GetTcid(unsigned short slot, unsigned char tcid)
{
int index;				/* return code : entry found */

	for (index=0; index < MAXTCID; index++)
	{
		if ((ln_tab_tcid[index].state != NOT_USED) &&
			(ln_tab_tcid[index].slot == slot) && (ln_tab_tcid[index].tcid == tcid))
			return index;	/* entry found 	   */
	}
	return -1;				/* entry not found */
}



/* ======================================================================== */
/* NAME : LINK_NewTcid					 									*/
/* 									    									*/
/* DESCRIPTION : Reserve a new entry in ln_tab_tcid							*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index: index in ln_tab_slot											*/
/*		slot : slot identifier												*/
/*		tcid : transport connection identifier								*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		update the number of tcid for the current slot						*/
/*									    									*/
/* RETURN CODE								    							*/
/*		 -1 : no more free entry 											*/
/*		>=0 : index in ln_tab_tcid											*/
/*									  										*/
/* ======================================================================== */
int LINK_NewTcid(int index, unsigned short slot, unsigned char tcid)
{
int index_tcid;			/* return code : entry found */

	for (index_tcid=0; index_tcid < MAXTCID; index_tcid++)
	{
		if (ln_tab_tcid[index_tcid].state == NOT_USED)
		{
			ln_tab_tcid[index_tcid].tcid  = tcid;
			ln_tab_tcid[index_tcid].slot  = slot;
			ln_tab_tcid[index_tcid].state = CONNECTED;

			LINK_ResetFifo(index_tcid);		/* Reset Fifo for incoming TPDU*/

			ln_tab_slot[index].nbtcid++;	/* one more tcid for this slot */

			return index_tcid;	/* entry found 	   */
		}
	}
	return -1;					/* entry not found */
}



/* ======================================================================== */
/* NAME : LINK_GetIndex					 									*/
/* 									    									*/
/* DESCRIPTION : Get a new entry in ln_tab_slot								*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		ln_msg points the received informations								*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		ln_tab_slot is updated												*/
/*									    									*/
/* RETURN CODE								    							*/
/*		 -1 : no more entry													*/
/*		 -2 : already connected												*/
/*		 -3 : wrong physical buffer size									*/
/*		>=0 : index in ln_tab_slot											*/
/*									  										*/
/* ======================================================================== */
int LINK_GetIndex(void)
{
int index;				/* return code : entry found */

	for (index=0; index < PAL_CI_GetNumberOfModule(); index++)
	{
		if (ln_tab_slot[index].state == NOT_USED)   /* take the first free entry, reserve it */
		{
			ln_tab_slot[index].slot	   = ln_msg.lconn.slot_id;
			ln_tab_slot[index].state   = IDLE;
			ln_tab_slot[index].nbtcid  = 0;
			ln_tab_slot[index].send    = 0;

        	if ((ln_msg.lconn.size < MINLNSIZE) || (ln_msg.lconn.size > MAXLNSIZE))
        	{
				PrintErr(("LINK_GetIndex : wrong physical buffer size %d !!!\n", ln_msg.lconn.size));
				return -3;
        	}
        	else
        	{
				PrintDbg(("LINK_GetIndex : new entry %d with slot %d\n", index, ln_msg.lconn.slot_id));
				ln_tab_slot[index].size = ln_msg.lconn.size;
				return index;
			}
		}
		else
		{
			if (ln_tab_tcid[index].slot == ln_msg.lconn.slot_id)
				return -2;	/* slot already connected */
		}
    }

	PrintDbg(("LINK_GetIndex : no more entry\n"));
	return -1;				/* no more entry */
}


/* ======================================================================== */
/* NAME : LINK_FreeIndex					 								*/
/* 									    									*/
/* DESCRIPTION :Delete an entry in ln_tab_slot and all tcid entries in 		*/
/*				ln_tab_tcid													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index : index of the entry to delete								*/
/*		slot  : slot identifier												*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		< 0 : entry not found												*/
/*		>=0 : entry deleted													*/
/*									  										*/
/* ======================================================================== */
int LINK_FreeIndex(int index, unsigned short slot)
{
int i;	/* working index */

	if ((index < 0) || (index >= PAL_CI_GetNumberOfModule())) // if ((index < 0) && (index >= PAL_CI_GetNumberOfModule()))
    	return -1;

	ln_tab_slot[index].state = NOT_USED;				/* free the entry 	*/
	ln_tab_slot[index].slot  = 0;

	/* Look for all tcid entries and release them */
	for (i=0; i < MAXTCID; i++)
	{
		if (ln_tab_tcid[i].slot == slot)
		{
			ln_tab_tcid[i].tcid  = 0;
			ln_tab_tcid[i].slot  = 0;
			ln_tab_tcid[i].state = NOT_USED;

			LINK_ResetFifo(i);				/* Reset Fifo for incoming TPDU	*/
		}
	 }

	return index;
}


/* ======================================================================== */
/* NAME : LINK_Send_Msg					 									*/
/* 									    									*/
/* DESCRIPTION : Send a message to another Task								*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*									  										*/
/* ======================================================================== */
int LINK_Send_Msg(void)
{
	PrintDbg(("LINK_Send_Msg <%x>\n", ln_header.trans));
	return ITC_MsgSend(ln_header.trans, (unsigned char *)&ln_infos);
}


/* ======================================================================== */
/* NAME : LINK_Send_Msg1				 									*/
/* 									    									*/
/* DESCRIPTION : Send a message to another Task								*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*									  										*/
/* ======================================================================== */
int LINK_Send_Msg1(void)
{
	PrintDbg(("LINK_Send_Msg1 <%x>\n", ln_header1.trans));
	return ITC_MsgSend(ln_header1.trans, (unsigned char *)&ln_infos1);
}



