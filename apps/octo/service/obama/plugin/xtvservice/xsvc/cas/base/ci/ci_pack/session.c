/****************************************************************************/
/*                                                                          */
/*   FILE:    session.c	                                                    */
/*   DATE:    30/06/97                                                      */
/*   PURPOSE: DVB-CI Session layer - entry point into SESS task          	*/
/*			- Initialization Module				    					    */
/*			- Main Module					    							*/
/*		    - Ending Module												    */
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version	Date	  	    Written by	     Notes	    	   			*/
/*	 1.0	    30/06/1997      SCM              Initial implementation 	*/
/*	 1.03		19/09/1997		SCM         	 ssnb assign modification	*/
/*				- SESS_GetIndex :the ssnb are between 0 and MAXSSNB			*/
/*				- SESS_RoutSsnb :the rssnb are between MAXSSNB and MAXSSNB*2*/
/*       1.05  28/12/1998  In function int SESS_Init() first test modified: */
/*               if (ITC_Init(SESS) = 0)  becomes  if (ITC_Init(SESS) <= 0) */
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/

#include "ci.h"

#include "session.h"
#include "itc.h"			/* Inter-Task Communication */
#include "sesstat.h"		/* State Diagram Module		*/
#include "sesscod.h"		/* Coding/Decoding Module	*/




/*--------------------------------------------------------------------------*/
/* Global Variables		                                            		*/
/*--------------------------------------------------------------------------*/
xmsg s_infos;				/* received informations	 					*/
xmsg s_infos1;				/* other saved informations	 					*/
sess s_tab_ssnb[MAXSSNB];   /* ssnb table   								*/
static short s_NbSessions;	/* number of established sessions				*/

/*--------------------------------------------------------------------------*/
/* Local Variables		                                            		*/
/*--------------------------------------------------------------------------*/
static unsigned char s_evt;	/* current event 								*/
static unsigned short SESS_Timer[SSNBTIMER]; /* unique TIMESESS timer ident */

/* ======================================================================== */
/* NAME : ci_pack_session							    							*/
/* 									    									*/
/* DESCRIPTION : entry point into SESS task :				    			*/
/*		 - Initialization Module				   							*/
/*		 - Main Module						    							*/
/*		 - Ending Module				         							*/
/*	The Main Module is an infinite loop :								    */
/*		 - wait events (messages from TRANS task or RM task,   				*/
/*		   internal timeouts)					    						*/
/*		 - read the first received event  			   						*/
/*		 - call the Transport State Diagram SESS_State	          			*/
/*	                                                                  		*/
/* 									 										*/
/* INPUT PARAMETERS :							    						*/
/*									    									*/
/* OUTPUT PARAMETERS :							  							*/
/*									   										*/
/* RETURN CODE								   								*/
/*									   										*/
/* ======================================================================== */
void ci_pack_session(void *vpParam)		// modified by shinhs
{
unsigned char trans;	/* sender/receiver of received event */

	/* Call the Initialization Module */
	if (! SESS_Init()) {
		HxLOG_Error("session : SESS_Init error !!!\n");
		return;		/* error while Initialization */
	}

	/* Main Module */
	/* =========== */
	while(1)		/* infinite loop */
	{
		/* Blocking Wait for event */
#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND //20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
		if (! ITC_WaitEvt(ITC_INFINITE, &s_evt, &trans, &s_infos, CI_TASK_ID_SESSION))
#else
		if (! ITC_WaitEvt(ITC_INFINITE, &s_evt, &trans, &s_infos))
#endif
		{
			HxLOG_Error("session : ITC_WaitEvt . Error in return code !!!\n");

			#if 0 // org
			/* Ending Module */
			SESS_End();
			return;
			#else
			continue;
			#endif
		}

		/* Reading the event (MESSAGE or TIMER) */
		if ((s_evt == MESSAGE) || (s_evt == TIMER))
		{
			HxLOG_Print(("session : ITC_WaitEvt . MESSAGE or TIMER\n"));

			/* Call the Transport State Diagram */
			SESS_State();
		}
		else
		{
			HxLOG_Error("session : ITC_WaitEvt . Error in received event !!!\n");

			/* Ending Module */
			SESS_End();
			return;
		}
	}

	/* Call the Ending Module : should never get executed */
	SESS_End();
	return;
}
/* End of Main Module */
/* ================== */


/* ======================================================================== */
/* NAME : SESS_Init							    							*/
/* 									    									*/
/* DESCRIPTION : Initialization Module					    				*/
/*		 Session variables initialization			    					*/
/*		 Session State Diagram initialization 	            				*/
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
int SESS_Init(void)
{
int i;

	if (ITC_Init(SESS) <= 0)
		return 0;

	s_evt = 0;

	for (i=0; i < MAXSSNB; i++)
		s_tab_ssnb[i].state = NOT_USED;

	for (i=0; i < SSNBTIMER; i++)
		SESS_Timer[i] = 0;

	return 1;
}
/* End of Initialization Module */
/* ============================ */


/* ======================================================================== */
/* NAME : SESS_End							    							*/
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
void SESS_End(void)
{
	ITC_End();

	return;
}
/* End of Initialization Module */
/* ============================ */


/* ======================================================================== */
/* NAME : SESS_State													    */
/* 									    									*/
/* DESCRIPTION : Entry point of the Session State Diagram Module			*/
/*		Read the event					       							    */
/*		Read the message if the event = MESSAGE		    				    */
/*		Find the current SSNB and the matching state diagram index 			*/
/*		Call the current transition matches the received event	   			*/
/* 									   										*/
/* INPUT PARAMETERS :							   							*/
/*	s_evt provides the received event (MESSAGE or TIMER)		   			*/
/*	s_tab_ssnb[i].state gives the current state of the State Diagram  		*/
/*	for the current SSNB (index i)					    					*/
/*									   										*/
/* OUTPUT PARAMETERS :							    						*/
/*	s_tab_ssnb[i].state is updated					    					*/
/*									  										*/
/* RETURN CODE :							  							    */
/*									 									    */
/* ======================================================================== */
void SESS_State(void)
{
int iRet; /* return code 			 			   */
unsigned char *pt;	/* pointer to allocated memory */

	iRet = 0;
	pt = NULL;	// added by jhcho

	if (s_evt == MESSAGE)
	{
		switch (s_header.type) {
		    case SPDU :					/* Transport <--> Session */
				pt = s_msg.spdu.pspdu;
				iRet = SESS_Spdu();
		    	break;

			case APDU :					/* Session <--> RM */
				pt = s_msg.apdu.papdu;
				iRet = SESS_Apdu();
				break;

			case SS_TC_CREATED :   		/* Transport ---> Session */
				iRet = SESS_Created();
				break;

			case SS_TC_DELETED :		/* Transport ---> Session */
				iRet = SESS_Deleted();
				break;

			case RM_DELETE_TC :			/* RM ---> Session */
				iRet = SESS_RMDelete();
				break;

			case RM_OPEN_SS_RSP :		/* RM ---> Session */
				iRet = SESS_OpenRsp();
				break;

			case RM_CLOSE_SS :			/* RM ---> Session */
				iRet = SESS_RMClose();
				break;

			default :			/* unknown message */
				HxLOG_Error("SESS_State : Unknown message[0x%x] is ignored (error) !!!\n", s_header.type);
				break;
		}
	}
	else	/* TIMER */
	{
		if ((s_timer >= TIMESESS) && (s_timer < TIMESESS + SSNBTIMER))
		{
			/* timeout for a transport protocol request */
			iRet = SESS_Timeout();
		}
		else
		{
			HxLOG_Error("SESS_State : Unknown timerId[0x%x] is ignored (error) !!!\n", s_timer);
			iRet = 1;
		}
	}

	if (!iRet && pt)	// modified by jhcho
			ITC_DynFree(pt);  	/* free memory used with last message received */

	return;
}
/* End of Session State Diagram */
/* ============================ */



/* ======================================================================== */
/* NAME : SESS_Spdu								    						*/
/* 									    									*/
/* DESCRIPTION : Session State diagram : SPDU received 						*/
/*		- Find the Session Number											*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		s_msg.spdu contains the SPDU message received from TRANS Task		*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int SESS_Spdu(void)
{
unsigned char tag;		/* SPDU type 										*/
unsigned short ssnb;	/* session number									*/
tc_infos pinfos;		/* parameters of received SPDU 						*/
int mem;				/* memory management : return value					*/
unsigned long len;		/* length of decoded spdu							*/
int index;			/* index of s_tab_ssnb[] for the current session number */
int iRet;				/* return code 			 							*/

	/* Decode the SPDU */
	if ((len = SESS_Decode(s_msg.spdu.pspdu, s_msg.spdu.len, &tag, &ssnb, &pinfos)) == 0) // < 0)
	{	/* decoding error : the event is ignored */
		/* Send IHM_WARNING to IHM Task 		 */
		HxLOG_Error("SESS_Spdu : Error while decoding a SPDU tag=0x%x ssnb=%d iRet=%ld !!!\n", tag, ssnb, len);
		return 0;
	}

	if (tag != open_session_request)
	{
		/* Find the session number */
		index = SESS_GetSsnb(ssnb);
		if (index < 0)
		{	/* not found : the event is ignored */
    		/* Send IHM_WARNING to IHM Task 	*/
			HxLOG_Error("SESS_Spdu : Unknown ssnb %d !!!\n", ssnb);
			return 0;			/* free memory  */
		}
	}
	else
	{
		/* try to reserve a nex ssnb entry in s_tab_ssnb */
		pinfos.openss.tcid = s_msg.spdu.tcid;
		iRet = SESS_GetIndex(pinfos.openss.tcid);
		if (iRet < 0)
		{
    		/* Send IHM_WARNING to IHM Task */
			HxLOG_Error("SESS_Spdu : No more available ssnb !!!\n");
			pinfos.openss.status = SS_NOTAVAILABLE;
			iRet = SESS_NotOpen(&pinfos);
			return iRet;
		}
		else
			index = iRet;		/* new ssnb index */
	}

	/* Call the transport state diagram */
	if (!SESS_Action(index, tag, &pinfos, &mem))
		return 0;				/* free memory  */

	/* Release the state diagram instance if the session doesn't exist */
    if (s_tab_ssnb[index].state == IDLE)
   		SESS_FreeIndex(index);

	return mem;
}



/* ======================================================================== */
/* NAME : SESS_Apdu								    						*/
/* 									    									*/
/* DESCRIPTION : Session State diagram : APDU received 						*/
/*		- Find the Session Number											*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		s_msg.apdu contains the APDU message received from RM Task			*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int SESS_Apdu(void)
{
int mem;				/* memory management : return value					*/
int index;	/* index of s_tab_ssnb[] for the current transport connection  	*/

	/* Find the session number */
	index = SESS_GetSsnb(s_msg.apdu.ssnb);
	if (index < 0)
	{	/* not found : the event is ignored */
		/* Send IHM_WARNING to IHM Task */
		mem = 0;			/* free memory  */
	}
	else
	{
		/* Call the transport state diagram */
		if (!SESS_Action(index, s_header.type, NULL, &mem))
			mem = 0;		/* free memory */
	}

	// prevent fix (added)
	if (index >= 0)
	{
		/* Release the state diagram instance if the session doesn't exist */
	    if (s_tab_ssnb[index].state == IDLE)
	    	SESS_FreeIndex(index);
	}
	// the end of prevent fix

	return mem;
}



/* ======================================================================== */
/* NAME : SESS_Created						    							*/
/* 									    									*/
/* DESCRIPTION : Session State Diagram : SS_TC_CREATED transition received 	*/
/*		- Create a new state diagram										*/
/*		- Call the action													*/
/*		- Update the state diagram											*/
/*									    									*/
/* INPUT PARAMETERS :							    						*/
/*		s_msg.conn contains the SS_TC_CREATED message received from 		*/
/*		TRANS Task															*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*		s_tab_ssnb[] updated												*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int SESS_Created(void)
{
	/* Foward the request to delete the transport connection to the TRANS Task */
	/* ======================================================================= */
	SESS_TCCreated(s_msg.creat.tcid);

	return 1;
}


/* ======================================================================== */
/* NAME : SESS_Deleted							    						*/
/* 									    									*/
/* DESCRIPTION : Session State diagram : SS_TC_DELETED received from TRANS	*/
/*				 Task														*/
/*	- Find all sessions that match the disconnected module					*/
/*	- For each session :													*/
/*		call the action														*/
/*		free the state diagram												*/
/*	- Call the state diagram a last time to warn the RM Task				*/
/*	The sessions can be directly associated with the deleted tcid or		*/
/* indirectly (associated by routing : distant resources or applications	*/
/* using distant resources)													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		s_msg.deleted contains the SS_TC_DELETED message received from TRANS*/
/*		Task 																*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*			(not used in this function)										*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int SESS_Deleted(void)
{
int index;	/* index of s_tab_ssnb[] for the current transport connection  	*/
int mem;				/* memory management : return value					*/
tc_infos infos;			/* parameters of received SPDU 						*/

	infos.tcidss.tcid = s_msg.deleted.tcid;

	/* repeat for all sessions */
	for (index=0; index < MAXSSNB; index++)
		if ((s_tab_ssnb[index].state != NOT_USED) &&
			((s_tab_ssnb[index].tcid == s_msg.deleted.tcid) ||		/* direct sessions   */
			 (s_tab_ssnb[index].rtcid == s_msg.deleted.tcid)))     /* indirect sessions */
		{
			SESS_Action(index, s_header.type, &infos, &mem);

			/* Release the state diagram instance if the session doesn't exist */
    		if (s_tab_ssnb[index].state == IDLE)
    			SESS_FreeIndex(index);
        }

	/* Forward the deletion of the transport connection to the RM Task */
	/* =============================================================== */
	SESS_TCDeleted(infos.tcidss.tcid);

	return mem;
}


/* ======================================================================== */
/* NAME : SESS_RMDelete						    							*/
/* 									    									*/
/* DESCRIPTION : Session State Diagram :RM_DELETE_TC transition received 	*/
/*									    									*/
/* INPUT PARAMETERS :							    						*/
/*		s_msg.conn contains a RM_DELETE_TC message received from RM Task 	*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*		s_tab_ssnb[] updated												*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int SESS_RMDelete(void)
{
	/* Foward the request to delete the transport connection to the TRANS Task */
	/* ======================================================================= */
	SESS_DeleteTC(s_msg.sdeleted.tcid);

	return 1;
}


/* ======================================================================== */
/* NAME : SESS_OpenRsp							    						*/
/* 									    									*/
/* DESCRIPTION : Session State diagram : RM_OPEN_SS_RSP received from RM 	*/
/*				 Task														*/
/*		- Find the session number											*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		s_msg.openrsp contains the RM_OPEN_SS_RSP message received from 	*/
/*		RM Task																*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int SESS_OpenRsp(void)
{
int mem;				/* memory management : return value					*/
int index;	/* index of s_tab_ssnb[] for the current transport connection  	*/
tc_infos infos;			/* parameters of received message					*/

	/* Find the Session Number */
	index = SESS_GetSsnb(s_msg.openrsp.ssnb);
	if (index < 0)
	{	/* not found : the event is ignored */
		/* Send IHM_WARNING to IHM Task */
		mem = 1;			/* no allocated memory  */
	}
	else
	{
		/* Call the transport state diagram */
		infos.openrss.status = s_msg.openrsp.status;
		infos.openrss.routing = s_msg.openrsp.rtcid;

		if (!SESS_Action(index, s_header.type, &infos, &mem))
			mem = 0;		/* free memory */
	}

	// prevent fix (added)
	if (index >= 0)
	{
		/* Release the state diagram instance if the session doesn't exist */
		if (s_tab_ssnb[index].state == IDLE)
			SESS_FreeIndex(index);
	}
	// the end of prevent fix

	return mem;
}



/* ======================================================================== */
/* NAME : SESS_RMClose							    						*/
/* 									    									*/
/* DESCRIPTION : Session State diagram : RM_CLOSE_SS received from RM Task	*/
/*		- Find the session number											*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		s_msg.close contains the RM_CLOSE_SS message received from RM Task 	*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int SESS_RMClose(void)
{
int mem;				/* memory management : return value					*/
int index;	/* index of s_tab_ssnb[] for the current transport connection  	*/

	/* Find the Session Number */
	index = SESS_GetSsnb(s_msg.close.ssnb);
	if (index < 0)
	{	/* not found : the event is ignored */
		/* Send IHM_WARNING to IHM Task */
		mem = 1;			/* no allocated memory  */
	}
	else
	{
		/* Call the transport state diagram */
		if (!SESS_Action(index, s_header.type, NULL, &mem))
			mem = 0;		/* free memory */
	}

	// prevent fix (added)
	if (index >= 0)
	{
		/* Release the state diagram instance if the session doesn't exist */
		if (s_tab_ssnb[index].state == IDLE)
			SESS_FreeIndex(index);
	}
	// the end of prevent fix

	return mem;
}


/* ======================================================================== */
/* NAME : SESS_Timeout							    						*/
/* 									    									*/
/* DESCRIPTION : Transport State diagram : TIMEOUT received 				*/
/*		- Find the transport connection										*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		t_timer contains the Timer Identifier 					  			*/
/*		Task 																*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*			(not used in this function)										*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int SESS_Timeout(void)
{
int mem;				/* memory management : return value					*/
int index;	/* index of s_tab_ssnb[] for the current transport connection  	*/

	/* Find the session number */
	index = SESS_GetTMSsnb(s_timer);
	if (index < 0)
	{	/* not found : the event is ignored */
		/* Send IHM_WARNING to IHM Task */
		return 1;
	}

	HxLOG_Print("SESS_Timeout : index=%d, state=%d !!!\n", index, s_tab_ssnb[index].state);

	SESS_Kill_Timer(index);		/* cancel the timer */

	/* Call the transport state diagram */
	SESS_Action(index, TIMESESS, NULL, &mem);

	/* Release the state diagram instance if the session doesn't exist */
	if (s_tab_ssnb[index].state == IDLE)
		SESS_FreeIndex(index);

	return mem;
}



/* ======================================================================== */
/* NAME : SESS_GetIndex					 									*/
/* 									    									*/
/* DESCRIPTION : Get a new entry in s_tab_ssnb								*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		tcid : transport connection of the new session						*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		the ssnb is assigned into s_tab_ssnb  								*/
/*									    									*/
/* RETURN CODE								    							*/
/*		-1 : no more entry													*/
/*		>=0 : index in s_tab_ssnb											*/
/*									  										*/
/* ======================================================================== */
int SESS_GetIndex(unsigned char tcid)
{
int index;				/* return code : entry found */

	if (s_NbSessions >= MAXSSNB)
		return -1;						/* too many established sessions, no more entry 	*/

	for (index=0; index < MAXSSNB; index++)
		if (s_tab_ssnb[index].state == NOT_USED)   /* take the first free entry, reserve it */
		{
			/* Reserve a new ssnb */
			s_tab_ssnb[index].state    = IDLE;

			s_tab_ssnb[index].tcid 	   = tcid;
			s_tab_ssnb[index].ssnb 	   = index+1;	/* 0 < ssnb < MAXSSNB ; 0 is reserved */
//			s_tab_ssnb[index].ssnb 	   = (index*2) + 1;	/* odd session number : 0 is reserved */
			/* the even ssnb are reserved for the routing ssnb on routing tcid 				  */

			s_tab_ssnb[index].rtcid    = 0;
			s_tab_ssnb[index].rssnb    = 0;

			s_tab_ssnb[index].timerid  = 0;

			s_NbSessions++;

			HxLOG_Print("SESS_GetIndex : new entry %d ssnb=%d\n", index, (index*2)+1);
			return index;
		}

	return -1;							   			/* no more entry */
}


/* ======================================================================== */
/* NAME : SESS_FreeIndex					 								*/
/* 									    									*/
/* DESCRIPTION :Delete an entry in s_tab_ssnb								*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index : index of the entry to delete								*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		s_NbSessions is updated												*/
/*									    									*/
/* RETURN CODE								    							*/
/*		< 0 : entry not found												*/
/*		>=0 : entry deleted													*/
/*									  										*/
/* ======================================================================== */
int SESS_FreeIndex(int index)
{
    if ((index < 0) || (index >= MAXSSNB)) // if ((index < 0) && (index >= MAXSSNB)) : 2009.05.21 bspark prevent defect
    	return -1;

	SESS_Kill_Timer(index);							/* kill pending TIMEOUT timer */

	s_tab_ssnb[index].state = NOT_USED;				/* free the entry 		*/
    s_NbSessions--;

	return index;
}


/* ======================================================================== */
/* NAME : SESS_GetSsnb					 									*/
/* 									    									*/
/* DESCRIPTION : Get the index in s_tab_ssnb								*/
/*	ssnb is looking for in ssnb and in rssnb (routing)						*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*	ssnb : session number													*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		< 0 : ssnb not found in s_tab_ssnb									*/
/*		>=0 : index in s_tab_ssnb											*/
/*									  										*/
/* ======================================================================== */
int SESS_GetSsnb(unsigned  short ssnb)
{
int index;		/* return code : entry found */

	for (index=0; index < MAXSSNB; index++)
		if ((s_tab_ssnb[index].state != NOT_USED) &&
			((s_tab_ssnb[index].ssnb == ssnb) || (s_tab_ssnb[index].rssnb == ssnb)))
			return index;   /* matching ssnb  */

	return -1;	   			/* ssnb not found */
}


/* ======================================================================== */
/* NAME : SESS_RoutSsnb					 									*/
/* 									    									*/
/* DESCRIPTION : Get a new session number for the routing tcid 				*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*	ssnb : session number of the session (first part between the Application*/
/*			and the host)													*/
/*	rtcid: routing tcid between the host and the distant resource provider	*/
/*			module															*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		s_tab_ssnb[].rtcid and s_tab_ssnb[].rssnb are updated				*/
/*									    									*/
/* RETURN CODE								    							*/
/*		 -1 : ssnb not found in s_tab_ssnb									*/
/*		>=0 : index in s_tab_ssnb											*/
/*									  										*/
/* ======================================================================== */
int SESS_RoutSsnb(unsigned short ssnb, unsigned char rtcid)
{
int index;		/* return code : entry found */

	for (index=0; index < MAXSSNB; index++)
		if ((s_tab_ssnb[index].state != NOT_USED) && (s_tab_ssnb[index].ssnb == ssnb))
		{
			s_tab_ssnb[index].rtcid  = rtcid;
			s_tab_ssnb[index].rssnb = index+MAXSSNB+1;	/* MAXSSNB < rssnb MAXSSNB*2 ; 0 is reserved */
//			s_tab_ssnb[index].rssnb = (index*2) + 2;	/* even ssnb ; 0 is reserved 	*/
			/* the odd ssnb are reserved for ssnb between the application and the host  */

			return index;   /* matching ssnb */
		}

	return -1;	   			/* ssnb not found */
}



/* ======================================================================== */
/* NAME : SESS_GetTMSsnb					 								*/
/* 									    									*/
/* DESCRIPTION : Get the index in s_tab_ssnb according to a TIMEOUT timer   */
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*	timerid :	timer identifier											*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		< 0 : timerid not found in s_tab_ssnb								*/
/*		>=0 : index in s_tab_ssnb											*/
/*									  										*/
/* ======================================================================== */
int SESS_GetTMSsnb(unsigned  short timerid)
{
int index;		/* return code : entry found */

	for (index=0; index < MAXSSNB; index++)
		if ((s_tab_ssnb[index].state != NOT_USED) && (s_tab_ssnb[index].timerid == timerid))
			return index;   /* matching timerid  */

	return -1;	   			/* timerid not found */
}


/* ======================================================================== */
/* NAME : SESS_Set_Timer				 									*/
/* 									    									*/
/* DESCRIPTION : Set a unique timer identifier (base : TIMESESS)			*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*	value   : value in ms													*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*	   >0 : Timer identifier												*/
/*	    0 : error															*/
/*									  										*/
/* ======================================================================== */
unsigned short SESS_Set_Timer(unsigned short value)
{
unsigned short ident;	/* unique identifier */
int i;					/* loop index		 */

	ident = 0;
	for (i=0; i < SSNBTIMER; i++) // hmkim : org is for (i=0; i <= SSNBTIMER; i++)
	{
		if (! SESS_Timer[i])
		{
			SESS_Timer[i] = TIMESESS+i;
			ident = TIMESESS+i;
			break;
		}
	}

	if (ident)
	{
#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
		return ITC_SetTimer(ident, value, CI_TASK_ID_SESSION);
#else
		return ITC_SetTimer(ident, value);
#endif
	}
	else
		return 0;
}


/* ======================================================================== */
/* NAME : SESS_Kill_Timer				 									*/
/* 									    									*/
/* DESCRIPTION : Kill the pending TIMEOUT timer if it exists				*/
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
void SESS_Kill_Timer(int index)
{
int i;					/* loop index		 */

	if (s_tab_ssnb[index].timerid != 0)
	{
		ITC_KillTimer(s_tab_ssnb[index].timerid);

		/* free the unique timer identifier */
		for (i=0; i < SSNBTIMER; i++) // hmkim : org is for (i=0; i <= SSNBTIMER; i++)
		{
			if (SESS_Timer[i] == s_tab_ssnb[index].timerid)
			{
				SESS_Timer[i] = 0;
				break;
			}
		}
		s_tab_ssnb[index].timerid = 0;
	}
	return;
}


/* ======================================================================== */
/* NAME : SESS_Send_Msg					 									*/
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
int SESS_Send_Msg(void)
{
	HxLOG_Print("SESS_Send_Msg <%x>\n", s_header.trans);

	return ITC_MsgSend(s_header.trans, (unsigned char *)&s_infos);
}


/* ======================================================================== */
/* NAME : SESS_Send_Msg1				 									*/
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
int SESS_Send_Msg1(void)
{
	HxLOG_Print("SESS_Send_Msg1 <%x>\n", s_header1.trans);

	return ITC_MsgSend(s_header1.trans, (unsigned char *)&s_infos1);
}

