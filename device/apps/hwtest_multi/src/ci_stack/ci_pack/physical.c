/****************************************************************************/
/*                                                                          */
/*   FILE:    physical.c	                                                */
/*   DATE:    31/07/97                                                      */
/*   PURPOSE: DVB-CI Physical layer - entry point into PHYSICAL task        */
/*			- Initialization Module				    					    */
/*			- Main Module					    							*/
/*		    - Ending Module												    */
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version	Date	  	    Written by	     Notes	    	   			*/
/*	 1.0	    31/07/1997      TGA, Teamlog     Initial implementation 	*/
/*	1.04		19/11/1997		TGA, Teamlog	 Add st_drvheader structure	*/
/*					in PHYSICAL_State										*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*																		    */
/****************************************************************************/

#include "ci.h"
#include "physical.h"
#include "phystat.h"
#include "physdrv.h"
#include "itc.h"			/* Inter-Task Communication */
#include "pal_ci.h"
/*--------------------------------------------------------------------------*/
/* Global Variables		                                            		*/
/*--------------------------------------------------------------------------*/
xmsg  ph_infos;				 /* received informations	 					*/
pslot ph_tab_slot[PAL_MAX_CISLOT_NUMBER];/* slot table									*/

/*--------------------------------------------------------------------------*/
/* Local Variables		                                            		*/
/*--------------------------------------------------------------------------*/
static unsigned char ph_evt;		/* current event 						*/
static unsigned char *ph_buff;		/* PHYSICAL buffer						*/
static unsigned short ph_len;		/* PHYSICAL buffer size					*/

/*--------------------------------------------------------------------------*/
/* External Variables		                                           		*/
/*--------------------------------------------------------------------------*/

/* ======================================================================== */
/* NAME : physical							    							*/
/* 									    									*/
/* DESCRIPTION : entry point into PHYSICAL task :				    		*/
/*		 - Initialization Module				   							*/
/*		 - Main Module						    							*/
/*		 - Ending Module				         							*/
/*	The Main Module is an infinite loop :								    */
/*		 - wait events (messages from PHYSICAL task or signals from PHYSICAL*/
/*		   driver)															*/
/*		 - read the first received event  			   						*/
/*		 - call the PHYSICAL State Diagram PHYSICAL_State	  	        	*/
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
void physical(void* vpParam)
{
unsigned char trans;	/* sender/receiver of received event */
int iRet;				/* return code						 */

	/* Call the Initialization Module */
	if (! PHYSICAL_Init())
	{
		PrintErr(("physical : PHYSICAL_Init error !!!\n"));
		return;		/* error while Initialization */
	}

	/* Main Module */
	/* =========== */
	while(1)		/* infinite loop */
	{
		/* Blocking Wait for MESSAGE with timeout */
		/* ====================================== */
		ph_evt = 0;

#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId�� ������ ���� �ʾ� �Ʒ��� ���� �ӽ÷� �ڵ带 �ۼ���.
		if (! ITC_WaitEvt(ITC_TIMEOUT, &ph_evt, &trans, &ph_infos, CI_TASK_ID_PHYSICAL))
#else
		if (! ITC_WaitEvt(ITC_TIMEOUT, &ph_evt, &trans, &ph_infos))
#endif
		{
			PrintErr(("physical : ITC_WaitEvt . Error in return code !!!\n"));

			/* Ending Module */
			PHYSICAL_End();
			return;
		}

		/* Process the received MESSAGE */
		/* ============================ */
		if (ph_evt == MESSAGE)
		{
			/* Call the link State Diagram */
			PHYSICAL_State();
		}
		else
		{
			/* Poll the PHYSICAL Driver */
			/* ======================== */
			ph_infos.phwinfos = (DRV *)ph_buff;

			iRet = PHYS_DRVRead((unsigned char *)ph_infos.phwinfos, &ph_len);
			if (iRet)
			{
				/* Process the received SIGNAL */
				/* =========================== */
				/* Call the link State Diagram */
				PHYSICAL_State();
			}
			else
			{
				if (iRet < 0)
				{
					PrintErr(("physical : PHYS_DRVRead . Error in received event !!!\n"));

					/* Ending Module */
					PHYSICAL_End();
					return;
				}
				/* else : nothing received from the PHYSICAL Driver */
			}
		}
	}

	/* Call the Ending Module : should never get executed */
	PHYSICAL_End();
	return;
}
/* End of Main Module */
/* ================== */


/* ======================================================================== */
/* NAME : PHYSICAL_Init							    						*/
/* 									    									*/
/* DESCRIPTION : Initialization Module					    				*/
/*		 physical variables initialization			    					*/
/*		 physical State Diagram initialization 	            				*/
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
int PHYSICAL_Init(void)
{
int i;

	PrintDbg(("PHYSICAL_Init\n"));

	ph_evt = 0;

	if (! ITC_Init(PHYSICAL))
		return 0;

	/* PHYSICAL Driver Initialization */
	if (PHYS_DRVOpen() <= 0)
	{
		PrintErr(("PHYSICAL_Init : PHYS_DRVOpen error !!!\n"));
		return 0;
	}

	for (i=0; i < PAL_MAX_CISLOT_NUMBER; i++)
	{
		ph_tab_slot[i].state = NOT_USED;
		ph_tab_slot[i].slot  = 0;
	}

	ph_buff = ITC_DynAlloc(MAXLNSIZE + sizeof(ph_data) + 5);	/* Maximum buffer size and header PH_DATA message */
	if (ph_buff == NULL)
	{
		PrintErr(("PHYSICAL_Init : not enought memory (%d) !!!\n", MAXLNSIZE + sizeof(ph_data) + 5));
		return 0;
	}
	VK_memset(ph_buff, 0, MAXLNSIZE + sizeof(ph_data) + 5);

	return 1;
}
/* End of Initialization Module */
/* ============================ */


/* ======================================================================== */
/* NAME : PHYSICAL_End							    						*/
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
void PHYSICAL_End(void)
{
	ITC_DynFree(ph_buff);
	ph_buff = NULL;

	PHYS_DRVClose();
	ITC_End();

	return;
}
/* End of Initialization Module */
/* ============================ */


/* ======================================================================== */
/* NAME : PHYSICAL_State													*/
/* 									    									*/
/* DESCRIPTION : Entry point of the physical State Diagram Module			*/
/*		Read the event					       							    */
/*		Read the message if the event = MESSAGE		    				    */
/*		query the PHYSICAL driver if the event = SIGNAL						*/
/*		Find the current slot_id and the matching state diagram index 		*/
/*		Call the current transition matches the received event	   			*/
/* 									   										*/
/* INPUT PARAMETERS :							   							*/
/*	ph_evt provides the received event (MESSAGE or SIGNAL)		   			*/
/*	ph_tab_slot[i].state gives the current state of the State Diagram  		*/
/*	for the current physical slot (index i)					    			*/
/*									   										*/
/* OUTPUT PARAMETERS :							    						*/
/*	ph_tab_slot[i].state is updated					    					*/
/*									  										*/
/* RETURN CODE :							  							    */
/*									 									    */
/* ======================================================================== */
void PHYSICAL_State(void)
{
int iRet;			/* return code 		  		   */
unsigned char *pt;	/* pointer to allocated memory */

	PrintDbg(("PHYSICAL_State\n"));

	iRet = 0;
	// leesy
	pt = NULL;
	if (ph_evt == MESSAGE)
		{
		switch (ph_header.type) {
			case LPDU :			/* Physical <-- Link */
				pt = ph_msg.lpdu.plpdu;
				iRet = PHYSICAL_Lpdu();
		    	break;

			default :			/* unknown message */
				PrintDbg(("PHYSICAL_State : Unknown message is ignored 0x%x\n", ph_header.type));
				break;
			}
		// leesy
		if (pt) // hmkim : org is if (!iRet && pt)
			ITC_DynFree(pt);  	/* free memory used with last received message */
	}
	else	/* received event from the PHYSICAL Driver */
	{
		switch(ph_infos.phwinfos->header.type) {
			case PH_DATA :			/* Physical <-- Driver */
				iRet = PHYSICAL_Data();
				break;

			case PH_ERROR :			/* Physical <-- Driver */
				iRet = PHYSICAL_Error();
				break;

			case PH_INSERT :		/* Physical <-- Driver */
				iRet = PHYSICAL_Insert();
				break;

			case PH_EXTRACT :		/* Physical <-- Driver */
				iRet = PHYSICAL_Extract();
				break;

			default :			/* unknown event  */
				PrintDbg(("PHYSICAL_State : Unknown event is ignored 0x%x\n", ph_infos.phwinfos->header.type));
				break;
			}
		}

	return;
}
/* End of physical State Diagram */
/* ============================= */


/* ======================================================================== */
/* NAME : PHYSICAL_Lpdu								    					*/
/* 									    									*/
/* DESCRIPTION : Physical State diagram : LPDU received 					*/
/*		- Decod the LPDU													*/
/*		- Find the physical slot											*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		ph_msg.lpdu contains the LPDU message received from LINK Task		*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int PHYSICAL_Lpdu(void)
{
int index;	/* index of ph_tab_slot[] for the current module connection  	*/
tc_infos infos;	/* information for PH_INSERT 								*/
int mem;	/* memory management : return value								*/

	PrintDbg(("PHYSICAL_Lpdu\n"));

	index = PHYSICAL_GetSlot(ph_msg.lpdu.slot_id);
	if (index >= 0)
	{
		infos.datahw.len = ph_msg.lpdu.len;
		infos.datahw.pdata = ph_msg.lpdu.plpdu;
		PHYSICAL_Action(index, LPDU, &infos, &mem);
	}
	else
		mem = 0;

	return mem;
}


/* ======================================================================== */
/* NAME : PHYSICAL_Insert							    					*/
/* 									    									*/
/* DESCRIPTION : Physical State diagram : Module Insertion Event received 	*/
/*		- Find the physical slot											*/
/*		- Initialize the link between the module and the host				*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int PHYSICAL_Insert(void)
{
int index;		/* index of ph_tab_slot[] for the current module connection	*/
tc_infos infos;	/* information for PH_INSERT 								*/
int mem;		/* memory management : return value							*/
unsigned short slot_id;		/* current slot identifier						*/

	PrintDbg(("PHYSICAL_Insert\n"));

	slot_id = ph_hw.insert.slot_id;

	/* Add a new instance of the Physical State Diagram for the new module					 			*/
	index = PHYSICAL_GetIndex(slot_id);		/* get the new instance for the physical state diagram 		*/
	if (index < 0)
	{
		/* Send IHM_WARNING to IHM Task */
		return 1;	/* can't get new instance or module already connected, the request is ignored ; no memory management */
    }

	infos.inserthw.status = ph_hw.insert.status;
	infos.inserthw.size   = ph_hw.insert.size;

	/* Call the physical state diagram */
	if (!PHYSICAL_Action(index, PH_INSERT, &infos, &mem))
		PHYSICAL_FreeIndex(index, slot_id);		/* error : free the new instance */
    else
    {	/* no DVB module, warning 		*/
		/* Send IHM_WARNING to IHM Task */
    }
	return 1;		/* no memory management */
}


/* ======================================================================== */
/* NAME : PHYSICAL_Error							    					*/
/* 									    									*/
/* DESCRIPTION : Physical State diagram : Module Error Event received 		*/
/*		- Find the physical slot											*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int PHYSICAL_Error(void)
{
int index;		/* index of ph_tab_slot[] for the current module connection	*/
tc_infos infos;	/* information for PH_ERROR 								*/
int mem;		/* memory management : return value							*/
unsigned short slot_id;		/* current slot identifier						*/

	PrintDbg(("PHYSICAL_Error\n"));

	slot_id = ph_hw.error.slot_id;

	/* Add a new instance of the Physical State Diagram for the new module					 			*/
	index = PHYSICAL_GetIndex(slot_id);		/* get the new instance for the physical state diagram 		*/
	if (index < 0)
	{
		/* Send IHM_WARNING to IHM Task */
		return 1;	/* can't get new instance or module already connected, the request is ignored ; no memory management */
    }

	infos.errorhw.status = ph_hw.errhw.status;

	/* Call the physical state diagram */
	PHYSICAL_Action(index, PH_ERROR, &infos, &mem);

	return 1;		/* no memory management */
}


/* ======================================================================== */
/* NAME : PHYSICAL_Data							    						*/
/* 									    									*/
/* DESCRIPTION : Physical State diagram : received data from Physical Driver*/
/*		- Find the physical slot											*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int PHYSICAL_Data(void)
{
int index;		/* index of ph_tab_slot[] for the current module connection	*/
tc_infos infos;	/* information for PH_DATA	 								*/
int mem;		/* memory management : return value							*/
unsigned short slot_id;		/* current slot identifier						*/

	PrintDbg(("PHYSICAL_Data\n"));

	slot_id = ph_hw.data.slot_id;

	index = PHYSICAL_GetSlot(slot_id);
	if (index < 0)
	{
		/* Send IHM_WARNING to IHM Task */
		return 1;	/* can't get new instance or module already connected, the request is ignored ; no memory management */
    }

	infos.datahw.len = ph_hw.data.len;
	infos.datahw.pdata = ph_hw.data.pdata;

	/* Call the physical state diagram */
	PHYSICAL_Action(index, PH_DATA, &infos, &mem);

	return 1;		/* no memory management */
}


/* ======================================================================== */
/* NAME : PHYSICAL_Extract							    					*/
/* 									    									*/
/* DESCRIPTION : Physical State diagram : Module Extraction Event received 	*/
/*		- Find the physical slot											*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int PHYSICAL_Extract(void)
{
int index;		/* index of ph_tab_slot[] for the current module connection	*/
int mem;		/* memory management : return value							*/
unsigned short slot_id;		/* current slot identifier						*/

	PrintDbg(("PHYSICAL_Extract\n"));

	slot_id = ph_hw.extract.slot_id;

	index = PHYSICAL_GetSlot(slot_id);
	if (index < 0)
	{
		/* Send IHM_WARNING to IHM Task */
		return 1;	/* can't find the slot identifier, no memory management */
    }

	/* Call the physical state diagram */
	PHYSICAL_Action(index, PH_EXTRACT, NULL, &mem);

	PHYSICAL_FreeIndex(index, slot_id);		/* free the new instance */

	return 1;		/* no memory management */
}


/* ======================================================================== */
/* NAME : PHYSICAL_GetIndex					 								*/
/* 									    									*/
/* DESCRIPTION : Get a new entry in ph_tab_slot								*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		slot_in : physical slot identifier									*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		ph_tab_slot is updated												*/
/*									    									*/
/* RETURN CODE								    							*/
/*		 -1 : no more entry													*/
/*		 -2 : already connected												*/
/*		>=0 : index in ln_tab_slot											*/
/*									  										*/
/* ======================================================================== */
int PHYSICAL_GetIndex(unsigned short slot_id)
{
int index;				/* return code : entry found */

	PrintDbg(("PHYSICAL_GetIndex\n"));

	for (index=0; index < PAL_CI_GetNumberOfModule(); index++)
	{
		if (ph_tab_slot[index].state == NOT_USED)   /* take the first free entry, reserve it */
		{
			ph_tab_slot[index].slot	   = slot_id;
			ph_tab_slot[index].state   = IDLE;
			return index;
		}
		else
		{
			if (ph_tab_slot[index].slot == slot_id)
				return -2;		/* slot already connected */
		}
    }

	PrintDbg(("PHYSICAL_GetIndex : no more entry"));
	return -1;				/* no more entry */
}


/* ======================================================================== */
/* NAME : PHYSICAL_GetSlot					 								*/
/* 									    									*/
/* DESCRIPTION : Get the current entry in ph_tab_slot						*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		slot_id : slot identifier											*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		 -1 : entry not found												*/
/*		>=0 : index in ph_tab_slot											*/
/*									  										*/
/* ======================================================================== */
int PHYSICAL_GetSlot(unsigned short slot_id)
{
int index;				/* return code : entry found */

	PrintDbg(("PHYSICAL_GetSlot\n"));

	for (index=0; index < PAL_CI_GetNumberOfModule(); index++)
	{
		if ((ph_tab_slot[index].state != NOT_USED) && (ph_tab_slot[index].slot == slot_id))
			return index;	/* entry found 	   */
	}
	return -1;				/* entry not found */
}


/* ======================================================================== */
/* NAME : PHYSICAL_FreeIndex				 								*/
/* 									    									*/
/* DESCRIPTION :Delete an entry in ph_tab_slot								*/
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
int PHYSICAL_FreeIndex(int index, unsigned short slot)
{
	PrintDbg(("PHYSICAL_FreeIndex\n"));

	if ((index < 0) || (index >= PAL_CI_GetNumberOfModule())) // if ((index < 0) && (index >= PAL_CI_GetNumberOfModule()))
    	return -1;

	ph_tab_slot[index].state = NOT_USED;				/* free the entry 	*/
	ph_tab_slot[index].slot  = 0;

	return index;
}


/* ======================================================================== */
/* NAME : PHYSICAL_Send_Msg					 								*/
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
int PHYSICAL_Send_Msg(void)
{
	PrintDbg(("PHYSICAL_Send_Msg\n"));

	return ITC_MsgSend(ph_header.trans, (unsigned char *)&ph_infos);
}




