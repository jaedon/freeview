/****************************************************************************/
/*                                                                          */
/*   FILE:    sesstat.c   	                                                */
/*   DATE:    30/06/97                                                      */
/*   PURPOSE: DVB-CI Session layer - state diagram for SESSION task			*/
/*   TAB STOPS : 4                       */
/*   HISTORY:                                                               */
/*   Version    Date  		    Written by	     Notes                  */
/*	 1.0	    30/06/1997      SCM              Initial implementation */
/*   1.5        28/12/1998      SCM              In function SESS_NotOpen() */
/*                                            add:   return iRet;           */
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*																		    */
/****************************************************************************/

//#include <string.h>

#include "sesstat.h"
#include "session.h"            /* Main Module			  	*/
#include "sesscod.h"			/* Coding/Decoding Module	*/
#include "itc.h"				/* Inter-Task Communication	*/
#include "ci.h"


/*--------------------------------------------------------------------------*/
/* External Variables		                                            	*/
/*--------------------------------------------------------------------------*/
extern xmsg s_infos;		/* received informations	 					*/
extern xmsg s_infos1;		/* other saved informations	 					*/
extern sess s_tab_ssnb[];   /* ssnb table   								*/


/* ======================================================================== */
/* NAME : SESS_Action					 									*/
/* 									    									*/
/* DESCRIPTION : Activate the action associated to the current state 		*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received SPDU or NULL		*/
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
int SESS_Action(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;			/* return code 				*/

	switch(s_tab_ssnb[index].state) {
		case ACTIVE : iRet = SESS_Active(index, evt, pinfos, mem);
			break;

		case IDLE : iRet = SESS_Idle(index, evt, pinfos, mem);
			break;

		case OPEN : iRet = SESS_Open(index, evt, pinfos, mem);
			break;

		case CREATION : iRet = SESS_Creation(index, evt, pinfos, mem);
			break;

		case CLOSE :  iRet = SESS_Close(index, evt, pinfos, mem);
			break;

		case RELEASE :  iRet = SESS_Release(index, evt, pinfos, mem);
			break;

		case DELETION :  iRet = SESS_Deletion(index, evt, pinfos, mem);
			break;

		default :
			PrintErr(("SESS_Action : Unknown state %d !!!\n", s_tab_ssnb[index].state));
			iRet = 0;	/* Error */
			break;
		}

	return iRet;
}




/* ======================================================================== */
/* NAME : SESS_Idle								 							*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the IDLE state						*/
/*		There is a new transport connection . The SESS Task is waiting for 	*/
/*		a session opening on this new tcid									*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received SPDU or NULL		*/
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
int SESS_Idle(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;		/* return code  											*/

	*mem = 1;	/* default */

	switch(evt) {
		case open_session_request :
			PrintDbg(("SESS_Idle : open_session_request Received\n"));

			/* Forward the open session request to the RM Task */
			/* =============================================== */
			/* Take the same memory buffer */
			s_header.trans = (SESS << 4) | RM;
			s_header.type  = RM_OPEN_SS_REQ;
			s_header.len   = sizeof(sr_open);
			s_msg.open.tcid = s_tab_ssnb[index].tcid;		/* Use by RM resource to assign tcid to resources declared on a module */
			s_msg.open.ssnb = s_tab_ssnb[index].ssnb;
	        VK_memcpy(s_msg.open.ress_id, pinfos->openss.ress_id , SSLENRESS);

			/* Send the message */
    		if (! SESS_Send_Msg())
    		{
				PrintErr(("SESS_Idle : Can't send the RM_OPEN_SS_REQ message for tcid %d to RM Task !!!\n", s_tab_ssnb[index].tcid));
				*mem = 0;		/* s_msg must be released */
				iRet = 0;		/* Error */
			}
			else
			{
				/* Save the ress_id */
	           	VK_memcpy(s_tab_ssnb[index].ress_id, pinfos->openss.ress_id , SSLENRESS);

			   	/* Set the new state */
				s_tab_ssnb[index].state = OPEN;
				*mem = 0;		/* s_msg must be released	 */
				iRet = 1;		/* Success; keep the memory  */
			}
			break;		/* end of open_session_request processing */

		default :	/* protocol error, ignore the event */
			PrintErr(("SESS_Idle : protocol error . event %d received in %d state !!!\n", evt, s_tab_ssnb[index].state));
			if ((evt > MSG_MAXVAL) || (evt == APDU))
				*mem = 0;		/* s_msg must be released if allocated */
			iRet = 1;
			break;
		}

	return iRet;
}	/* end of IDLE state */



/* ======================================================================== */
/* NAME : SESS_Open			 												*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the OPEN state						*/
/*		- The SESS Task is waiting for a response of the RM Task for the	*/
/*		open session request from the module								*/
/*		- If the resource is on a distant module, the Task performs the		*/
/*		routing with session creation										*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received SPDU or NULL		*/
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
int SESS_Open(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;		/* return code  											*/
tc_infos infos;	/* information from received message						*/
unsigned long len;

	*mem = 1;	/* default */
	iRet = 1;

	switch(evt) {
		case RM_OPEN_SS_RSP :
			PrintDbg(("SESS_Open : RM_OPEN_SS_RSP evt. Received\n"));

			/* Get free buffer for SPDU message : the message is open_session_response or create_session */
			/* the greater size is the open_session_response size */
			s_msg.spdu.pspdu = ITC_DynAlloc(SSOPENRSPSIZE);
			if (s_msg.spdu.pspdu == NULL)
			{
				PrintErr(("SESS_Open : can't allocate buffer for coding SPDU !!!\n"));
				return 1;	/* Error but return = 1 since no memory to free */
			}

			if (pinfos->openrss.routing)
			{	/* the resource is on a DVB distant module ; routing is the tcid of the distant module */
				/* =================================================================================== */

				/* Get a new session number for the routing tcid */
				iRet = SESS_RoutSsnb(s_tab_ssnb[index].ssnb, pinfos->openrss.routing);
				if (iRet < 0)
				{	/* can't open the distant session ; close the first part of the session */
					/* ==================================================================== */
					infos.openrss.status = SS_NOTAVAILABLE;
       	        	VK_memcpy(infos.openrss.ress_id , s_tab_ssnb[index].ress_id, SSLENRESS);

					s_header.trans = (SESS << 4) | TRANS;
					s_header.type  = SPDU;
					s_header.len   = sizeof(st_spdu);
					s_msg.spdu.tcid= s_tab_ssnb[index].tcid;
					len = SESS_Code(open_session_response, s_tab_ssnb[index].ssnb, &infos, s_msg.spdu.pspdu);
    				if (len == 0) // if (len < 0)
    				{
						PrintErr(("SESS_Open : Can't code an open_session_response for 0x%x%x%x%x !!!\n",
						pinfos->openrss.ress_id[0], pinfos->openrss.ress_id[1],
						pinfos->openrss.ress_id[2], pinfos->openrss.ress_id[3]));
						*mem = 0;		/* s_msg must be released */
						return 0;		/* Error */
					}
					else
					{
						s_msg.spdu.len = len;
					}

				    /* Send the message */
    				if (! SESS_Send_Msg())
    				{
						PrintErr(("SESS_Open : Can't send the open_session_response message for tcid %d to RM Task !!!\n", s_tab_ssnb[index].tcid));
						*mem = 0;		/* s_msg must be released */
						iRet = 0;		/* Error */
					}
					else
					{
						/* Set the new state */
						s_tab_ssnb[index].state = IDLE;
						iRet = 1;		/* Success; keep the memory  */
					}
				}
				else
				{	/* create the second link of the distributed session */
					/* ================================================= */
    	        	VK_memcpy(infos.create.ress_id,  s_tab_ssnb[index].ress_id, SSLENRESS);

					s_header.trans = (SESS << 4) | TRANS;
					s_header.type  = SPDU;
					s_header.len   = sizeof(st_spdu);
					s_msg.spdu.tcid= pinfos->openrss.routing;
					len = SESS_Code(create_session, s_tab_ssnb[index].rssnb, &infos, s_msg.spdu.pspdu);
    				if (len == 0) // if (len < 0)
    				{
						PrintErr(("SESS_Open : Can't code a create_session for 0x%x%x%x%x !!!\n",
						pinfos->openrss.ress_id[0], pinfos->openrss.ress_id[1],
						pinfos->openrss.ress_id[2], pinfos->openrss.ress_id[3]));
						*mem = 0;		/* s_msg must be released */
						return 0;		/* Error */
					}
					else
					{
						s_msg.spdu.len = len;
					}

				    /* Send the message */
    				if (! SESS_Send_Msg())
    				{
						PrintErr(("SESS_Open : Can't send the create_session message for tcid %d to RM Task !!!\n", s_tab_ssnb[index].tcid));
						*mem = 0;		/* s_msg must be released */
						iRet = 0;		/* Error */
					}
					else
					{
						/* Set the timer waiting for the response for this ssnb */
						s_tab_ssnb[index].timerid = SESS_Set_Timer(TM_SESS);/* do nothing if the timer can't be created */

    					/* Set the new state */
						s_tab_ssnb[index].state = CREATION;
						iRet = 1;		/* Success; keep the memory  */
					}
				}
			}
			else
			{	/* the resource is on the local host */
				/* ================================= */
				infos.openrss.status = pinfos->openrss.status;
   	        	VK_memcpy(infos.openrss.ress_id , s_tab_ssnb[index].ress_id, SSLENRESS);

				/* Send the open session response to the TRANS Task */
				/* ================================================ */
				s_header.trans = (SESS << 4) | TRANS;
				s_header.type  = SPDU;
				s_header.len   = sizeof(st_spdu);
				s_msg.spdu.tcid= s_tab_ssnb[index].tcid;
				len = SESS_Code(open_session_response, s_tab_ssnb[index].ssnb, &infos, s_msg.spdu.pspdu);
    			if (len == 0) // if (len < 0)
    			{
					PrintErr(("SESS_Open : Can't code an open_session_response for 0x%x%x%x%x !!!\n",
					pinfos->openrss.ress_id[0], pinfos->openrss.ress_id[1],
					pinfos->openrss.ress_id[2], pinfos->openrss.ress_id[3]));
					*mem = 0;		/* s_msg must be released */
					return 0;		/* Error */
				}
				else
				{
					s_msg.spdu.len = len;
				}

			    /* Send the message */
    			if (! SESS_Send_Msg())
    			{
					PrintErr(("SESS_Open : Can't send the open_session_response message for tcid %d to RM Task !!!\n", s_tab_ssnb[index].tcid));
					*mem = 0;		/* s_msg must be released */
					iRet = 0;		/* Error */
				}
				else
				{
					if (pinfos->openrss.status == SS_OK)
					{
						/* Send a RM_SS_OPENED to the RM Task */
						/* ================================== */
						s_header1.trans    = (SESS << 4) | RM;
						s_header1.type     = RM_SS_OPENED;
						s_header1.len      = sizeof(sr_opened);
            			s_msg1.opened.tcid = s_tab_ssnb[index].tcid;
						s_msg1.opened.ssnb = s_tab_ssnb[index].ssnb;

			    		/* Send the message */
    					if (! SESS_Send_Msg1())
    					{
							PrintErr(("SESS_Open : Can't send the RM_SS_OPENED message for ssnb %d to RM Task !!!\n", s_tab_ssnb[index].ssnb));
							iRet = 0;		/* Error */
						}
						else
						{
					    	/* Set the new state */
							s_tab_ssnb[index].state = ACTIVE;
							iRet = 1;		/* Success; keep the memory  */
						}
					}
					else
					{	/* open_session_response(-) sent */
				    	/* Set the new state */
						s_tab_ssnb[index].state = IDLE;
						iRet = 1;		/* Success; keep the memory  */
					}
				}
			}
			break;		/* end of open_session_request processing */

		case SS_TC_DELETED :
			PrintDbg(("SESS_Open : SS_TC_DELETED evt. Received\n"));
	    	/* Set the new state */
			s_tab_ssnb[index].state = IDLE;
			break;	/* end of SS_TC_DELETED processing */

		default :	/* protocol error, ignore the event */
			PrintErr(("SESS_Open : protocol error . event %d received in %d state !!!\n", evt, s_tab_ssnb[index].state));
			if ((evt > MSG_MAXVAL) || (evt == APDU))
				*mem = 0;		/* s_msg must be released if allocated */
			iRet = 1;
			break;
		}

	return iRet;
}	/* end of OPEN state */


/* ======================================================================== */
/* NAME : SESS_Active		 												*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the ACTIVE state						*/
/*		- The SESS Task is waiting for APDU to send to the module 			*/
/*		- The SESS Task is waiting for SPDU to send to the local or distant */
/*		(routing) resource													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received SPDU or NULL		*/
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
int SESS_Active(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;		/* return code  											*/
tc_infos infos;	/* information for SPDU		 								*/
unsigned short ssnb;	/* current ssnb										*/
unsigned char tcid;		/* current tcid										*/
unsigned long len;

	*mem = 1;	/* default */

	switch(evt) {
		case APDU :
			PrintDbg(("SESS_Active : APDU Received\n"));

			/* Code the SPDU and Send it to the TRANS Task */
			/* =========================================== */
				/* Get a new free buffer for SPDU message (s_msg points the APDU) */
				s_msg1.spdu.pspdu = ITC_DynAlloc(s_msg.apdu.len + SSESSIONSIZE);
				if (s_msg1.spdu.pspdu == NULL)
				{
					PrintErr(("SESS_Active : can't allocate buffer for coding SPDU !!!\n"));
					return 0;	/* Error */
				}

				/* Assign pointer to APDU buffer */
				infos.datass.pbuff = s_msg.apdu.papdu;
				infos.datass.len   = s_msg.apdu.len;

				s_header1.trans = (SESS << 4) | TRANS;
				s_header1.type  = SPDU;
				s_header1.len   = sizeof(st_spdu);
				s_msg1.spdu.tcid= s_tab_ssnb[index].tcid;
				len = SESS_Code(session_number, s_tab_ssnb[index].ssnb, &infos, s_msg1.spdu.pspdu);
    			if (len == 0) // if (len < 0)
    			{
					PrintErr(("SESS_Active : Can't code a session_number for ssnb %d !!!\n", s_tab_ssnb[index].ssnb));
					ITC_DynFree(s_msg1.apdu.papdu);
					return 0;		/* Error */
				}
				else
				{
					s_msg1.spdu.len = len;
				}

				/* Send the message */
    			if (! SESS_Send_Msg1())
    			{
					PrintErr(("SESS_Active : Can't send the SPDU message for ssnb %d to RM Task !!!\n", s_tab_ssnb[index].ssnb));
					ITC_DynFree(s_msg1.apdu.papdu);
					iRet = 0;		/* Error */
				}
				else
				{
					*mem = 0;
					iRet = 1;		/* s_msg will be free by the calling function */
				}

			break;	/* end of APDU processing */

		case session_number :
			PrintDbg(("SESS_Active : session_number Received\n"));

			if (s_tab_ssnb[index].rssnb == 0)
			{	/* decode the SPDU and send the APDU to the RM Task */
				/* ================================================ */
				/* Get a new free buffer for APDU message (s_msg points the SPDU) */
				s_msg1.apdu.papdu = ITC_DynAlloc(pinfos->datass.len);
				if (s_msg1.apdu.papdu == NULL)
				{
					PrintErr(("SESS_Active : can't allocate buffer for coding APDU !!!\n"));
					return 0;	/* Error */
				}

				s_header1.trans = (SESS << 4) | RM;
				s_header1.type  = APDU;
				s_header1.len   = sizeof(sr_apdu);
				s_msg1.apdu.ssnb= s_tab_ssnb[index].ssnb;
				s_msg1.apdu.len = pinfos->datass.len;
				VK_memcpy(s_msg1.apdu.papdu, pinfos->datass.pbuff, s_msg1.apdu.len);

				/* Send the message */
    			if (! SESS_Send_Msg1())
    			{
					PrintErr(("SESS_Active: Can't send the APDU message for ssnb %d to RM Task !!!\n", s_tab_ssnb[index].ssnb));
					ITC_DynFree(s_msg1.apdu.papdu);
					iRet = 0;		/* Error */
				}
				else
				{
					*mem = 0;
					iRet = 1;		/* s_msg will be free by the calling function */
				}
			}
			else
			{	/* Foward the SPDU on the second part of the session (ssnb or rssnb) */
				/* =================================================				 */
				if (pinfos->datass.ssnb == s_tab_ssnb[index].rssnb)
				{
					s_msg.spdu.tcid = s_tab_ssnb[index].tcid;	/* send to the first part  */
					SESS_CodSsnb( s_tab_ssnb[index].ssnb, s_msg.spdu.pspdu+2);
					ssnb = s_tab_ssnb[index].ssnb;
				}
				else
				{
					s_msg.spdu.tcid = s_tab_ssnb[index].rtcid;	/* send to the second part */
					SESS_CodSsnb( s_tab_ssnb[index].rssnb, s_msg.spdu.pspdu+2);
					ssnb = s_tab_ssnb[index].rssnb;
				}

				s_header.trans = (SESS << 4) | TRANS;

				/* Send the message */
    			if (! SESS_Send_Msg())
    			{
					PrintErr(("SESS_Active : Can't send the SPDU message for ssnb %d to RM Task !!!\n", ssnb));
					iRet = 0;		/* Error */
				}
				else
					iRet = 1;		/* Success			*/
			}
			break;	/* end of session_number processing */

		case close_session_request :
			PrintDbg(("SESS_Active : close_session_request Received\n"));

			if (s_tab_ssnb[index].rssnb == 0)
			{	/* Send the response and send a RM_SS_CLOSED to the RM Task */
				/* ======================================================== */

				/* re-allocate memory since close_session_response size greater than close_session_request size */
#ifdef __ITC_USE_REALLOC
				s_msg.spdu.pspdu = ITC_DynReAlloc(s_msg.spdu.pspdu, SSCLOSERSPSIZE);
				if (s_msg.spdu.pspdu == NULL)
				{
					PrintErr(("SESS_Active : can't re-allocate buffer for coding SPDU !!!\n"));
					return 0;	/* Error */
				}
#else
{
				unsigned char *	pt;
				pt = ITC_DynAlloc(SSCLOSERSPSIZE);
				if (pt == NULL)
				{
					PrintErr(("SESS_Active : can't re-allocate buffer for coding SPDU !!!\n"));
					return 0;	/* Error */
				}

				if (s_msg.spdu.len <= SSCLOSERSPSIZE)
					VK_memcpy(pt, s_msg.spdu.pspdu, s_msg.spdu.len);
				else
					VK_memcpy(pt, s_msg.spdu.pspdu, SSCLOSERSPSIZE);

				ITC_DynFree(s_msg.spdu.pspdu);
				s_msg.spdu.pspdu = pt;
}
#endif
				infos.closers.status = SS_OK;

				s_header.trans = (SESS << 4) | TRANS;
				s_header.type  = SPDU;
				s_header.len   = sizeof(st_spdu);
				s_msg.spdu.tcid= s_tab_ssnb[index].tcid;
				len = SESS_Code(close_session_response, s_tab_ssnb[index].ssnb, &infos, s_msg.spdu.pspdu);
    			if (len == 0) // if (len < 0)
    			{
					PrintErr(("SESS_Active : Can't code an open_session_response for ssnb %d !!!\n", s_tab_ssnb[index].ssnb));
					return 0;		/* Error */
				}
				else
				{
					s_msg.spdu.len = len;
				}

				/* Send the message */
    			if (! SESS_Send_Msg())
    			{
					PrintErr(("SESS_Active : Can't send the open_session_response message for tcid %d to RM Task !!!\n", s_tab_ssnb[index].tcid));
					iRet = 0;		/* Error */
				}
				else
				{
					/* forward the closing to the RM Task with RM_SS_CLOSED */
					/* ==================================================== */
					s_header1.trans 	 = (SESS << 4) | RM;
					s_header1.type  	 = RM_SS_CLOSED;
					s_header1.len   	 = sizeof(sr_closed);
					s_msg1.closed.ssnb   = s_tab_ssnb[index].ssnb;
					s_msg1.closed.status = SS_OK;

					/* Send the message */
    				if (! SESS_Send_Msg1())
    				{
						PrintErr(("SESS_Active : Can't send the RM_SS_CLOSED message for ssnb %d to RM Task !!!\n", s_tab_ssnb[index].ssnb));
						iRet = 0;		/* Error */
					}

	                /* Set the new state */
					s_tab_ssnb[index].state = IDLE;
					iRet = 1;		/* Success */
				}
			}
			else
			{   /* Forward the close session request toward the other side of the session */
				/* ====================================================================== */
				s_header.trans 		= (SESS << 4) | TRANS;
				s_header.type  		= SPDU;
				s_header.len   		= sizeof(st_spdu);

				/* Update the receiver (tcid ant ssnb) */
				if (pinfos->closess.ssnb == s_tab_ssnb[index].rssnb)
				{
					s_msg.spdu.tcid = s_tab_ssnb[index].tcid;	/* send to the first part  */
					SESS_CodSsnb( s_tab_ssnb[index].ssnb, s_msg.spdu.pspdu+2);

					/* reverse the (tcid,ssnb) / (rtcid, rssnb) in order to assign the	   */
					/* closing part in the (tcid, ssnb) for response management			   */
					/* cf SESS_Close when receiving a SS_TC_DELETED						   */
					ssnb = s_tab_ssnb[index].rssnb;
					tcid = s_tab_ssnb[index].rtcid;
					s_tab_ssnb[index].rssnb = s_tab_ssnb[index].ssnb;
					s_tab_ssnb[index].rtcid = s_tab_ssnb[index].tcid;
					s_tab_ssnb[index].ssnb = ssnb;
					s_tab_ssnb[index].tcid = tcid;;
				}
				else
				{
					s_msg.spdu.tcid = s_tab_ssnb[index].rtcid;	/* send to the second part */
					SESS_CodSsnb( s_tab_ssnb[index].rssnb, s_msg.spdu.pspdu+2);
				}

				/* Send the message */
   				if (! SESS_Send_Msg())
   				{
					PrintErr(("SESS_Active : Can't send the close_session_request message for ssnb %d to RM Task !!!\n", s_tab_ssnb[index].ssnb));
					iRet = 0;		/* Error */
				}
                else
                {
                	/* Set the timer waiting for the response for this ssnb */
					s_tab_ssnb[index].timerid = SESS_Set_Timer(TM_SESS);/* do nothing if the timer can't be created */

					/* Set the new state */
					s_tab_ssnb[index].state = CLOSE;
					iRet = 1;		/* Success */
				}
			}

			break;	/* end of close_session_request processing */

		case RM_CLOSE_SS :
			PrintDbg(("SESS_Active : RM_CLOSE_SS Received\n"));

			/* Send the close session request to the TRANS Task */
			/* ================================================ */
				/* Get a buffer for SPDU message */
				s_msg.spdu.pspdu = ITC_DynAlloc(SSCLOSESIZE);
				if (s_msg.spdu.pspdu == NULL)
				{
					PrintErr(("SESS_Active : can't allocate buffer for coding SPDU !!!\n"));
					return 1;		/* Error but return = 1 since no allocated memory to free */
				}

				infos.closers.status = SS_OK;

				s_header.trans = (SESS << 4) | TRANS;
				s_header.type  = SPDU;
				s_header.len   = sizeof(st_spdu);
				s_msg.spdu.tcid= s_tab_ssnb[index].tcid;
				len = SESS_Code(close_session_request, s_tab_ssnb[index].ssnb, &infos, s_msg.spdu.pspdu);
    			if (len == 0) // if (len < 0)
    			{
					PrintErr(("SESS_Active : Can't code a close_session_request for ssnb %d !!!\n", s_tab_ssnb[index].ssnb));
					return 0;	/* Error */
				}
				else
				{
					s_msg.spdu.len = len;
				}

				/* Send the message */
    			if (! SESS_Send_Msg())
    			{
					PrintErr(("SESS_Active : Can't send the close_session_request message for tcid %d to RM Task !!!\n", s_tab_ssnb[index].tcid));
					return 0;	/* Error */
				}
				else
				{
                	/* Set the timer waiting for the response for this ssnb */
					s_tab_ssnb[index].timerid = SESS_Set_Timer(TM_SESS);/* do nothing if the timer can't be created */

					/* Set the new state */
					s_tab_ssnb[index].state = CLOSE;
					iRet = 1;		/* Success */
			    }
			break;	/* end of RM_CLOSE_SS processing */

		case SS_TC_DELETED :
			PrintDbg(("SESS_Active : SS_TC_DELETED Received\n"));

			if (s_tab_ssnb[index].rssnb == 0)
			{	/* no routing : free the state diagram instance */
				/* Set the new state */
				s_tab_ssnb[index].state = IDLE;
				iRet = 1;		/* Success */
			}
			else
			{	/* Send a close session request to the other side of the session */
				/* ============================================================= */
				if (s_tab_ssnb[index].rtcid == s_tab_ssnb[index].tcid)
				{
					/* the session is on the same transport connection : nothing to do */
					/* Set the new state */
					s_tab_ssnb[index].state = IDLE;
					return 1;		/* Success */
				}

				s_msg.spdu.pspdu = ITC_DynAlloc(SSCLOSESIZE);
				if (s_msg.spdu.pspdu == NULL)
				{
					PrintErr(("SESS_Active : can't allocate buffer for coding SPDU !!!\n"));
					return 1;		/* Error but return = 1 since no allocated memory to free */
				}

				/* close the session */
				/* ================= */
				infos.closers.status = SS_OK;
				if (pinfos->tcidss.tcid == s_tab_ssnb[index].rtcid)
				{
					tcid = s_tab_ssnb[index].tcid;
					ssnb = s_tab_ssnb[index].ssnb;		/* first side of the session ; the other tcid is deleted  */
				}
				else
				{
					tcid = s_tab_ssnb[index].rtcid;
					ssnb = s_tab_ssnb[index].rssnb;     /* second side of the session ; the other tcid is deleted */
				}

				s_header.trans = (SESS << 4) | TRANS;
				s_header.type  = SPDU;
				s_header.len   = sizeof(st_spdu);
				s_msg.spdu.tcid= tcid;
				len = SESS_Code(close_session_request, ssnb, &infos, s_msg.spdu.pspdu);
    			if (len == 0) // if (len < 0)
    			{
					PrintErr(("SESS_Active : Can't code a close_session_request for ssnb %d !!!\n", ssnb));
					*mem = 0;		/* s_msg must be released */
					return 0;		/* Error */
				}
				else
				{
					s_msg.spdu.len = len;
				}

				/* Send the message */
    			if (! SESS_Send_Msg())
    			{
					PrintErr(("SESS_Active : Can't send the close_session_request message for ssnb %d to RM Task !!!\n", ssnb));
					*mem = 0;		/* s_msg must be released */
					iRet = 0;		/* Error */
				}
				else
				{
                	/* Set the timer waiting for the response for this ssnb */
					s_tab_ssnb[index].timerid = SESS_Set_Timer(TM_SESS);/* do nothing if the timer can't be created */

                	/* Set the new state */
					s_tab_ssnb[index].state = DELETION;
					iRet = 1;		/* Success; keep the memory  */
				}
			}
			break;	/* end of SS_TC_DELETED processing */

		default :	/* protocol error, ignore the event */
			PrintErr(("SESS_Active : protocol error . event 0x%x received in %d state !!!\n", evt, s_tab_ssnb[index].state));
			if (evt > MSG_MAXVAL)
				*mem = 0;		/* s_msg must be released if allocated */
			iRet = 1;
			break;
		}
	return iRet;
}	/* end of ACTIVE state */




/* ======================================================================== */
/* NAME : SESS_Creation		 												*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the CREATION state					*/
/*		- The SESS Task is waiting for a create session response from the	*/
/*		module																*/
/*		- If the response is negative, the first part of the session is 	*/
/*		closed																*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received SPDU or NULL		*/
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
int SESS_Creation(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;		/* return code  											*/
tc_infos infos;	/* information for SPDU		 								*/
unsigned long len;

	*mem = 1;	/* default */


	switch(evt) {
		case create_session_response :
			PrintDbg(("SESS_Creation : SS_TC_DELETED Received\n"));

        	/* kill pending timer 	*/
			SESS_Kill_Timer(index);

			/* Check the received parameters */
			if (s_tab_ssnb[index].rssnb != pinfos->creatr.ssnb)
				return 0;	/* Error */

			if (VK_memcmp(s_tab_ssnb[index].ress_id, pinfos->creatr.ress_id, SSLENRESS))
				return 0;	/* Error */

			/* Get the parameters and copy them into open_session_response */
			infos.openrss.status = pinfos->creatr.status;
			/* Don't check that ress_id into create_session_response is the same as the saved ress_id */
    	    VK_memcpy(infos.openrss.ress_id, s_tab_ssnb[index].ress_id , SSLENRESS);

			/* Foward the response on first part of the session to the TRANS Task */
			/* ================================================================== */
			s_header.trans = (SESS << 4) | TRANS;
			s_header.type  = SPDU;
			s_header.len   = sizeof(st_spdu);
			s_msg.spdu.tcid= s_tab_ssnb[index].tcid;
			len = SESS_Code(open_session_response, s_tab_ssnb[index].ssnb, &infos, s_msg.spdu.pspdu);
    		if (len == 0) // if (len < 0)
    		{
				PrintErr(("SESS_Creation : Can't code an open_session_response for 0x%x%x%x%x !!!\n",
				s_tab_ssnb[index].ress_id[0], s_tab_ssnb[index].ress_id[1],
				s_tab_ssnb[index].ress_id[2], s_tab_ssnb[index].ress_id[3]));
				return 0;		/* Error */
			}
			else
			{
				s_msg.spdu.len = len;
			}

			/* Send the message */
    		if (! SESS_Send_Msg())
    		{
				PrintErr(("SESS_Creation : Can't send the open_session_response message for tcid %d to RM Task !!!\n", s_tab_ssnb[index].tcid));
				*mem = 0;		/* Free memory */
				iRet = 0;		/* Error */
			}
			else
			{
				/* open_session_response sent */
				if (pinfos->creatr.status == SS_OK)
			    	/* Set the new state */
					s_tab_ssnb[index].state = ACTIVE;
				else
					/* open_session_response(-) sent */
			    	/* Set the new state */
					s_tab_ssnb[index].state = IDLE;
				iRet = 1;		/* Success; keep the memory  */
			}
			break;	/* end of create_session_response processing */

		case TIMESESS :
			PrintDbg(("SESS_Creation : SS_TC_DELETED Received\n"));

			/* close the session */
			/* ================= */
			/* Get a buffer for SPDU message */
			s_msg.spdu.pspdu = ITC_DynAlloc(SSOPENRSPSIZE);
			if (s_msg.spdu.pspdu == NULL)
			{
				PrintErr(("SESS_Creation : can't allocate buffer for coding SPDU !!!\n"));
				return 1;		/* Error but return = 1 since no allocated memory to free */
			}

			infos.openrss.status = SS_NOTAVAILABLE;
    	    VK_memcpy(infos.openrss.ress_id, s_tab_ssnb[index].ress_id , SSLENRESS);

			s_header.trans = (SESS << 4) | TRANS;
			s_header.type  = SPDU;
			s_header.len   = sizeof(st_spdu);
			s_msg.spdu.tcid= s_tab_ssnb[index].tcid;
			len = SESS_Code(open_session_response, s_tab_ssnb[index].ssnb, &infos, s_msg.spdu.pspdu);
    		if (len == 0) // if (len < 0)
    		{
				PrintErr(("SESS_Creation : Can't code an open_session_response for 0x%x%x%x%x !!!\n",
				s_tab_ssnb[index].ress_id[0], s_tab_ssnb[index].ress_id[1],
				s_tab_ssnb[index].ress_id[2], s_tab_ssnb[index].ress_id[3]));
				return 0;		/* Error */
			}
			else
			{
				s_msg.spdu.len = len;
			}

			/* Send the message */
    		if (! SESS_Send_Msg())
    		{
				PrintErr(("SESS_Creation : Can't send the open_session_response message for tcid %d to RM Task !!!\n", s_tab_ssnb[index].tcid));
				iRet = 0;		/* Error */
			}
			else
			{
	            /* Set the new state */
				s_tab_ssnb[index].state = IDLE;
				iRet = 1;		/* Success; keep the memory  */
			}
			break;	/* end of TIMESESS processing */

		case SS_TC_DELETED :
			PrintDbg(("SESS_Creation : SS_TC_DELETED Received\n"));

			if (pinfos->tcidss.tcid == s_tab_ssnb[index].tcid)
			{	/* the deleted tcid is the first part of a session . When the distant module will respond to the */
				/* creation of the second part of the session on the second tcid, this session will be released	 */
               	/* Set the new state */
				s_tab_ssnb[index].state = RELEASE;
				iRet = 1;		/* Success; keep the memory  */
			}
			else
			{   /* the deleted tcid is the second part of a session . The session must not be opened, the first  */
				/* part of the session is closed																 */
				/* Get free buffer for SPDU message */
				s_msg.spdu.pspdu = ITC_DynAlloc(SSOPENRSPSIZE);
				if (s_msg.spdu.pspdu == NULL)
				{
					PrintErr(("SESS_Creation : can't allocate buffer for coding SPDU !!!\n"));
					return 0;	/* Error */
				}

				/* close the session */
				/* ================= */
				infos.openrss.status = SS_NOTAVAILABLE;
    	    	VK_memcpy(infos.openrss.ress_id, s_tab_ssnb[index].ress_id , SSLENRESS);

				s_header.trans = (SESS << 4) | TRANS;
				s_header.type  = SPDU;
				s_header.len   = sizeof(st_spdu);
				s_msg.spdu.tcid= s_tab_ssnb[index].tcid;
				len = SESS_Code(open_session_response, s_tab_ssnb[index].ssnb, &infos, s_msg.spdu.pspdu);
    			if (len == 0) // if (len < 0)
    			{
					PrintErr(("SESS_Creation : Can't code an open_session_response for 0x%x%x%x%x !!!\n",
					s_tab_ssnb[index].ress_id[0], s_tab_ssnb[index].ress_id[1],
					s_tab_ssnb[index].ress_id[2], s_tab_ssnb[index].ress_id[3]));
					*mem = 0;		/* s_msg must be released */
					return 0;		/* Error */
				}
				else
				{
					s_msg.spdu.len = len;
				}

				/* Send the message */
    			if (! SESS_Send_Msg())
    			{
					PrintErr(("SESS_Creation : Can't send the open_session_response message for tcid %d to RM Task !!!\n", s_tab_ssnb[index].tcid));
					*mem = 0;		/* s_msg must be released */
					iRet = 0;		/* Error */
				}
				else
				{
                	/* Set the new state */
					s_tab_ssnb[index].state = IDLE;
					iRet = 1;		/* Success; keep the memory  */
				}
			}
			break;	/* end of SS_TC_DELETED processing */

		default :	/* protocol error, ignore the event */
			PrintErr(("SESS_Creation : protocol error . event 0x%x received in %d state !!!\n", evt, s_tab_ssnb[index].state));
			if ((evt > MSG_MAXVAL) || (evt == APDU))
				*mem = 0;		/* s_msg must be released if allocated */
			iRet = 1;
			break;
		}

	return iRet;
}	/* end of CREATION state */



/* ======================================================================== */
/* NAME : SESS_Release		 												*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the RELEASE state					*/
/*	A tcid is deleted on the first part of a session . When the response to */
/*	the creation session request will be received from the resource provider*/
/*	module, the SESS Task will release this session							*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received SPDU or NULL		*/
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
int SESS_Release(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet = 1;		/* return code  											*/
tc_infos infos;	/* information for SPDU		 								*/
unsigned long len;

	*mem = 1;	/* default */

	switch(evt) {
		case create_session_response :

			PrintDbg(("SESS_Release : create_session_response Received\n"));

        	/* kill pending timer 	*/
			SESS_Kill_Timer(index);

			if (pinfos->creatr.status == SS_OK)
			{
				/* close the session on the second part */
				/* ==================================== */
				s_header.trans = (SESS << 4) | TRANS;
				s_header.type  = SPDU;
				s_header.len   = sizeof(st_spdu);
				s_msg.spdu.tcid= s_tab_ssnb[index].rtcid;
				len = SESS_Code(close_session_request, s_tab_ssnb[index].rssnb, &infos, s_msg.spdu.pspdu);
   				if (len == 0) // if (len < 0)
   				{
					PrintErr(("SESS_Release : Can't code a close_session_request for 0x%x%x%x%x !!!\n",
					s_tab_ssnb[index].ress_id[0], s_tab_ssnb[index].ress_id[1],
					s_tab_ssnb[index].ress_id[2], s_tab_ssnb[index].ress_id[3]));
					return 0;		/* Error */
				}
				else
				{
					s_msg.spdu.len = len;
				}

				/* Send the message */
   				if (! SESS_Send_Msg())
   				{
					PrintErr(("SESS_Release : Can't send the close_session_request message for tcid %d to RM Task !!!\n", s_tab_ssnb[index].tcid));
					iRet = 0;		/* Error */
				}
				else
				{
					/* Set the timer waiting for the response for this ssnb */
					s_tab_ssnb[index].timerid = SESS_Set_Timer(TM_SESS);/* do nothing if the timer can't be created */

					/* Set the new state */
					s_tab_ssnb[index].state = DELETION;
					iRet = 1;		/* Success; keep the memory  */
				}
			}
			break;	/* end of create_session_response processing */

		case TIMESESS :
			PrintDbg(("SESS_Release : TIMESESS Received\n"));
			/* same processing that in CREATION state */
			iRet = SESS_Creation(index, evt, pinfos, mem);
			break;	/* end of TIMESESS processing */

		default :	/* protocol error, ignore the event */
			PrintErr(("SESS_Release : protocol error . event 0x%x received in %d state !!!\n", evt, s_tab_ssnb[index].state));
			if ((evt > MSG_MAXVAL) || (evt == APDU))
				*mem = 0;		/* s_msg must be released if allocated */
			iRet = 1;
			break;
		}
	return iRet;
}	/* end of RELEASE state */


/* ======================================================================== */
/* NAME : SESS_Deletion		 												*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the DELETION state					*/
/*	A tcid is deleted on the first part of a session . The SESS Task has	*/
/*	close the session on the second part and is waiting for the response	*/
/*	from the TRANS Task . The RM Task is forwarded when the response is		*/
/*	received																*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received SPDU or NULL		*/
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
int SESS_Deletion(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;		/* return code  											*/

	*mem = 1;	/* default */

	switch(evt) {

		case close_session_response :
			PrintDbg(("SESS_Deletion : close_session_response Received\n"));

        	/* kill pending timer 	*/
			SESS_Kill_Timer(index);

			*mem = 0;		/* Free memory */

           	/* Set the new state */
			s_tab_ssnb[index].state = IDLE;
			iRet = 1;			/* Success */
			break;	/* end of close_session_response processing */

		case SS_TC_DELETED :
			PrintDbg(("SESS_Deletion : SS_TC_DELETED Received\n"));

        	/* kill pending timer 	*/
			SESS_Kill_Timer(index);

           	/* Set the new state */
			s_tab_ssnb[index].state = IDLE;
			iRet = 1;			/* Success */
			break;	/* end of SS_TC_DELETED processing */

		case TIMESESS :
			PrintDbg(("SESS_Deletion : TIMESESS Received\n"));

           	/* Set the new state */
			s_tab_ssnb[index].state = IDLE;
			iRet = 1;			/* Success */
			break;		/* end of TIMESESS processing */

		default :	/* protocol error, ignore the event */
			PrintErr(("SESS_Deletion : protocol error . event %d received in %d state !!!\n", evt, s_tab_ssnb[index].state));
			if ((evt > MSG_MAXVAL) || (evt == APDU))
				*mem = 0;		/* s_msg must be released if allocated */
			iRet = 1;
			break;
		}
	return iRet;
}	/* end of DELETION state */


/* ======================================================================== */
/* NAME : SESS_Close		 												*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the CLOSE state						*/
/*		- The SESS Task is waiting for a close session response from the	*/
/*		module																*/
/*		- There are two cases :												*/
/*		  1) The close session request was made by a local resource on the 	*/
/*			 host (ex : MMI) ; There isn't routing strategy, the SESS Task	*/
/*			 forwards the response to the RM Task with RM_SS_CLOSED message */
/*		  2) The close session was made by an application and routed by the */
/*			 host to the remote resource (in ACTIVE state) . The remote		*/
/*			 resource sends the response is this state and the host must	*/
/*			 forward the response to the distant application (not to the RM */
/*			 Task)															*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		pinfos :	pointer to parameters of the received SPDU or NULL		*/
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
int SESS_Close(int index, unsigned char evt, tc_infos *pinfos, int *mem)
{
int iRet;		/* return code  											*/
tc_infos infos;	/* information for SPDU		 								*/
unsigned long len;

	*mem = 1;	/* default */
	iRet = 1;

	switch(evt) {
		case close_session_response :
			PrintDbg(("SESS_Close : close_session_response Received\n"));
        	/* kill pending timer 	*/
			SESS_Kill_Timer(index);
			*mem = 0;		/* free memory */

		case TIMESESS :
			PrintDbg(("SESS_Close : TIMESESS Received\n"));

			if (s_tab_ssnb[index].rssnb == 0)
			{	/* first case . See DESCRIPTION above */
				/* forward the closing to the RM Task with RM_SS_CLOSED */
				/* ==================================================== */
				s_header.trans 		= (SESS << 4) | RM;
				s_header.type  		= RM_SS_CLOSED;
				s_header.len   		= sizeof(sr_closed);
				s_msg.closed.ssnb   = s_tab_ssnb[index].ssnb;
				s_msg.closed.status = SS_OK;

				/* Send the message */
				if (! SESS_Send_Msg())
				{
					PrintErr(("SESS_Close : Can't send the RM_SS_CLOSED message for ssnb %d to RM Task !!!\n", s_tab_ssnb[index].ssnb));
					return 1;	/* for memory management */
				}
	           	else
	           	{
					/* Set the new state */
					s_tab_ssnb[index].state = IDLE;
	           	}
			}
			else
			{	/* second case . See DESCRIPTION above */
				/* forward the closing to the other side of the session */
				/* ==================================================== */
				/* Get free buffer for SPDU message */
				s_msg1.spdu.pspdu = ITC_DynAlloc(SSCLOSERSPSIZE);
				if (s_msg1.spdu.pspdu == NULL)
				{
					PrintErr(("SESS_Close : can't allocate buffer for coding SPDU !!!\n"));
					return 1;	/* for memory management */
				}

				s_header1.trans	= (SESS << 4) | TRANS;
				s_header1.type	= SPDU;
				s_header1.len 	= sizeof(st_spdu);
				if (evt == TIMESESS)
				{
					infos.closers.status = SS_OK;
					/* problem with TIMESESS : unknown session to free */
					s_msg1.spdu.tcid= s_tab_ssnb[index].tcid;	/* send to the first part  */
					len = SESS_Code(close_session_response, s_tab_ssnb[index].ssnb, &infos, s_msg1.spdu.pspdu);
				}
				else
				{
					infos.closers.status = pinfos->closers.status;

					if (pinfos->closers.ssnb == s_tab_ssnb[index].rssnb)
					{
						s_msg1.spdu.tcid= s_tab_ssnb[index].tcid;	/* send to the first part  */
						len = SESS_Code(close_session_response, s_tab_ssnb[index].ssnb, &infos, s_msg1.spdu.pspdu);
					}
					else
					{
						s_msg1.spdu.tcid= s_tab_ssnb[index].rtcid;	/* send to the second part  */
						len = SESS_Code(close_session_response, s_tab_ssnb[index].rssnb, &infos, s_msg1.spdu.pspdu);
					}
				}

   				if (len == 0) // if (len < 0)
   				{
					PrintErr(("SESS_Close : Can't code a close_session_response for tcid %d !!!\n", s_msg1.spdu.tcid));
					ITC_DynFree(s_msg1.spdu.pspdu);
					return 1;		/* for memory management */
				}
				else
				{
					s_msg1.spdu.len = len;
				}

				/* Send the message */
				if (! SESS_Send_Msg1())
				{
					PrintErr(("SESS_Close : Can't send the close_session_response message for ssnb %d to RM Task !!!\n", s_tab_ssnb[index].ssnb));
					ITC_DynFree(s_msg1.spdu.pspdu);
					return 1;		/* for memory management */
				}

	           	/* Set the new state */
				s_tab_ssnb[index].state = IDLE;
				iRet = 1;			/* Success */
			}
			break;		/* end of TIMESESS / close_session_response processing */

		case SS_TC_DELETED :
			PrintDbg(("SESS_Close : SS_TC_DELETED Received\n"));

			if (s_tab_ssnb[index].rssnb == 0)
			{	/* first case . See DESCRIPTION above */
				/* Send a RM_SS_CLOSED to the RM Task */
				/* ================================== */
				s_header.trans 		= (SESS << 4) | RM;
				s_header.type  		= RM_SS_CLOSED;
				s_header.len   		= sizeof(sr_closed);
				s_msg.closed.ssnb   = s_tab_ssnb[index].ssnb;
				s_msg.closed.status = SS_OK;

				/* Send the message */
				if (! SESS_Send_Msg())
				{
					PrintErr(("SESS_Close : Can't send the RM_SS_CLOSED message for ssnb %d to RM Task !!!\n", s_tab_ssnb[index].ssnb));
					return 1;		/* Error : return = 1 for memory management */
				}
				else
				{
	           		/* Set the new state */
					s_tab_ssnb[index].state = IDLE;
				}
			}
			else
			{
				if (s_tab_ssnb[index].tcid == s_tab_ssnb[index].rtcid)
				{
					/* no routing strategy since the two parts of the session are on the same transport connection */
	           		/* Set the new state */
					s_tab_ssnb[index].state = IDLE;
					return 1;		/* Success */
				}

				/* the closing part (first part) of the session is (tcid,ssnb) : cf SESS_Active */
				if (s_msg.deleted.tcid == s_tab_ssnb[index].tcid)
				{	/* the first part of the session is deleted, wait for the close_session_response on the second part */
		           	/* Set the new state */
					s_tab_ssnb[index].state = DELETION;
				}
				else
				{	/* the second part is deleted, a close_session_response will close the first part */
					/* Get free buffer for SPDU message */
					s_msg.spdu.pspdu = ITC_DynAlloc(SSCLOSERSPSIZE);
					if (s_msg.spdu.pspdu == NULL)
					{
						PrintErr(("SESS_Close : can't allocate buffer for coding SPDU !!!\n"));
						return 1;		/* Error : return = 1 for memory management */
					}

					infos.closers.status = SS_OK;
					s_header.trans	= (SESS << 4) | TRANS;
					s_header.type	= SPDU;
					s_header.len 	= sizeof(st_spdu);
					s_msg.spdu.tcid = s_tab_ssnb[index].tcid;	/* send to the first part  */
					len  = SESS_Code(close_session_response, s_tab_ssnb[index].ssnb, &infos, s_msg.spdu.pspdu);
   					if (len == 0) // if (len < 0)
   					{
						PrintErr(("SESS_Close : Can't code a close_session_response for tcid %d !!!\n", s_msg.spdu.tcid));
						ITC_DynFree(s_msg.spdu.pspdu);
						return 1;		/* Error : return = 1 for memory management */
					}
					else
					{
						s_msg.spdu.len = len;
					}

					/* Send the message */
					if (! SESS_Send_Msg())
					{
						PrintErr(("SESS_Close : Can't send the close_session_response message for ssnb %d to RM Task !!!\n", s_tab_ssnb[index].ssnb));
						ITC_DynFree(s_msg.spdu.pspdu);
					}

					/* Set the new state */
					s_tab_ssnb[index].state = IDLE;
				}
			}
			break;		/* end of SS_TC_DELETED processing */

		default :	/* protocol error, ignore the event */
			PrintErr(("SESS_Close : protocol error . event 0x%x received in %d state !!!\n", evt, s_tab_ssnb[index].state));
			if ((evt > MSG_MAXVAL) || (evt == APDU))
				*mem = 0;		/* s_msg must be released if allocated */
			break;
		}
	return iRet;
}	/* end of CLOSE state */



/* ======================================================================== */
/* NAME : SESS_TCCreated						 							*/
/* 									    									*/
/* DESCRIPTION : Send a SS_TC_CREATED to the RM Task						*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*	tcid : transport connection												*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*									  										*/
/* ======================================================================== */
int SESS_TCCreated(unsigned char tcid)
{
int iRet;		/* Return code */

	/* Foward the creation of a new transport connection to the RM Task */
	/* ================================================================ */
	s_header.trans    = (SESS << 4) | RM;
	s_header.type     = RM_TC_CREATED;
	s_header.len      = sizeof(sr_creat);
    s_msg.screat.tcid  = tcid;

	/* Send the message */
    if (! SESS_Send_Msg())
    {
		PrintErr(("SESS_TCCreated : Can't send the RM_TC_CREATED message for tcid %d to RM Task !!!\n", tcid));
		iRet = 0;		/* Error */
	}
	else
		iRet = 1;		/* Success */

	return iRet;
}



/* ======================================================================== */
/* NAME : SESS_TCDeleted						 							*/
/* 									    									*/
/* DESCRIPTION : Send a RM_TC_DELETED to the RM Task						*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*	tcid : transport connection												*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*									  										*/
/* ======================================================================== */
int SESS_TCDeleted(unsigned char tcid)
{
int iRet;		/* Return code */

	/* Foward the deletion of the transport connection to the RM Task */
	/* ============================================================== */
	s_header.trans      = (SESS << 4) | RM;
	s_header.type       = RM_TC_DELETED;
	s_header.len        = sizeof(sr_deleted);
    s_msg.sdeleted.tcid = tcid;

	PrintDbg(("SESS_TCDeleted Start\n"));

	/* Send the message */
    if (! SESS_Send_Msg())
	{
		PrintErr(("SESS_TCDeleted : Can't send the RM_TC_DELETED message for tcid %d to RM Task !!!\n", tcid));
		iRet = 0;		/* Error */
	}
	else
		iRet = 1;		/* Success */

	return iRet;
}



/* ======================================================================== */
/* NAME : SESS_DeleteTC							 							*/
/* 									    									*/
/* DESCRIPTION : Send a SS_DELETE_TC to the TRANS Task						*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*	tcid : transport connection												*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*									  										*/
/* ======================================================================== */
int SESS_DeleteTC(unsigned char tcid)
{
int iRet;		/* Return code */

	/* Foward the request to delete the transport connection to the TRANS Task */
	/* ======================================================================= */
	s_header.trans  = (SESS << 4) | TRANS;
	s_header.type   = SS_DELETE_TC;
	s_header.len    = sizeof(st_delete);
    s_msg.sdel.tcid = tcid;

	PrintDbg(("SESS_DeleteTC Start\n"));

    /* Send the message */
	if (! SESS_Send_Msg())
	{
		PrintErr(("SESS_DeleteTC : Can't send the SS_DELETE_TC message for tcid %d to TRANS Task !!!\n", tcid));
		iRet = 0;		/* Error */
	}
	else
		/* No timeout : a transport timeout is managed into transport layer */
		iRet = 1;		/* Success */

	return iRet;
}


/* ======================================================================== */
/* NAME : SESS_NotOpen							 							*/
/* 									    									*/
/* DESCRIPTION : Send an open_session_response(-) to the RM Task				*/
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
int SESS_NotOpen(tc_infos *pinfos)
{
int iRet;		/* return code  											*/
tc_infos infos;	/* information for SPDU		 								*/
unsigned long len;

	/* can't open the session : resource not available */
	/* =============================================== */
	infos.openrss.status = pinfos->openss.status;
   	VK_memcpy(infos.openrss.ress_id, pinfos->openss.ress_id , SSLENRESS);

	PrintDbg(("SESS_NotOpen Start\n"));

	/* Take the buffer used for open_session_request   */
	s_header.trans = (SESS << 4) | TRANS;
	s_header.type  = SPDU;
	s_header.len   = sizeof(st_spdu);
	len = SESS_Code(open_session_response, 0, &infos, s_msg.spdu.pspdu);
	if (len == 0) // if (len < 0)
	{
		PrintErr(("SESS_NotOpen : Can't code an open_session_response for 0x%x%x%x%x !!!\n",
		pinfos->openss.ress_id[0], pinfos->openss.ress_id[1],
		pinfos->openss.ress_id[2], pinfos->openss.ress_id[3]));
		return 0;		/* Error */
	}
	else
	{
		s_msg.spdu.len = len;
	}

    /* Send the message */
	if (! SESS_Send_Msg())
    {
		PrintErr(("SESS_NotOpen : Can't send the open_session_response message for tcid %d to RM Task !!!\n", pinfos->openss.tcid));
		iRet = 0;		/* Error */
	}
	else
	{
	  	/* Don't change the state */
		iRet = 1;		/* Success; keep the memory  */
	}
	return iRet;
}

