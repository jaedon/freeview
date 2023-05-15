/****************************************************************************/
/*                                                                          */
/*   FILE:    trans.c	                                                    */
/*   DATE:    13/06/97                                                      */
/*   PURPOSE: DVB-CI Transport layer - entry point into TRANS task          */
/*			- Initialization Module				    					    */
/*			- Main Module					    							*/
/*		    - Ending Module												    */
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version	Date	  	    Written by	     Notes	    	   			*/
/*	 1.0	    13/06/1997      SCM              Initial implementation 	*/
/*	 1.03		19/09/1997		SCM         	 Windows 95 sample data		*/
/*					GL_TabSlot and GL_NbSlot management						*/
/*	 1.03			TRPT_Disconnect() : extracted slot management			*/
/*   1.05       28/12/1998      SCM              In function TRPT_Init()    */
/*        changed "if (ITC_Init(TRANS) < 0)" to "if (ITC_Init(TRANS) <= 0)" */
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/

#include "trans.h"
#include "itc.h"			/* Inter-Task Communication */
#include "transtat.h"		/* State Diagram Module		*/
#include "transcod.h"		/* Coding/Decoding Module	*/
#include "ci.h"

#include <_svc_cas_sub_dev_api.h>




/*--------------------------------------------------------------------------*/
/* External Variables		                                            	*/
/*--------------------------------------------------------------------------*/
extern HUINT8 g_aucIsCaSysId[CARM_MAX_CISLOT_NUMBER];
extern HUINT8 g_aucCamReplied[CARM_MAX_CISLOT_NUMBER]; // hmkim, 2010-03 : added.

/*--------------------------------------------------------------------------*/
/* Global Variables		                                            		*/
/*--------------------------------------------------------------------------*/
xmsg t_infos;				/* received informations	 					*/
xmsg t_infos1;				/* other saved informations	 					*/
trpt t_tab_tcid[MAXTCID];   /* tcid table   								*/
FIFO t_tab_fifo[MAXTCID];	/* FIFO table									*/

#if 1 // #ifdef _INCLUDE_CI_TRPT_RECOVERY_
#define MAX_NUM_TRPT_TIMEOUT_RETRY	1
unsigned char g_aucCiTrptTimoutRetryCnt[MAXTCID];
#endif

/*--------------------------------------------------------------------------*/
/* Local Variables		                                            		*/
/*--------------------------------------------------------------------------*/
static unsigned char t_evt;	/* current event 								*/
static short t_NbConnection;/* number of established transport connections  */

/* ======================================================================== */
/* NAME : ci_pack_transport							    							*/
/* 									    									*/
/* DESCRIPTION : entry point into TRANS task :				    			*/
/*		 - Initialization Module				   							*/
/*		 - Main Module						    							*/
/*		 - Ending Module				         							*/
/*	The Main Module is an infinite loop :								    */
/*		 - wait events (messages from LINK task or SESS task,   			*/
/*		   internal timeouts)					    						*/
/*		 - read the first received event  			   						*/
/*		 - call the Transport State Diagram TRPT_State	          			*/
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
void ci_pack_transport(void* vpParam)	// modified by shinhs
{
unsigned char trans;	/* sender/receiver of received event */

	/* Call the Initialization Module */
	if (! TRPT_Init()) {
		HxLOG_Error("ci_pack_transport : TRPT_Init error !!!\n");
		return /*-1*/;		/* error while Initialization */
	}

	/* Main Module */
	/* =========== */
	while(1)		/* infinite loop */
	{
		/* Blocking Wait for event */
#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
		if (! ITC_WaitEvt(ITC_INFINITE, &t_evt, &trans, &t_infos, CI_TASK_ID_TRANSPORT))
#else
		if (! ITC_WaitEvt(ITC_INFINITE, &t_evt, &trans, &t_infos))
#endif
		{
			HxLOG_Error("ci_pack_transport : ITC_WaitEvt . Error in return code !!!\n");

			#if 0 // org
			/* Ending Module */
			TRPT_End();
			return /*-1*/;
			#else
			continue;
			#endif
		}

		/* Reading the event (MESSAGE or TIMER) */
		if ((t_evt == MESSAGE) || (t_evt == TIMER))
		{
			/* Call the Transport State Diagram */
			if(t_evt == MESSAGE)
				HxLOG_Print(("ci_pack_transport : MESSAGE\n"));

			TRPT_State();
		}
		else
		{
			HxLOG_Error("ci_pack_transport : ITC_WaitEvt . Error in received event !!!\n");

			/* Ending Module */
			TRPT_End();
			return/* -1*/;
		}
	}

	/* Call the Ending Module : should never get executed */
	TRPT_End();
	return /*0*/;
}
/* End of Main Module */
/* ================== */


/* ======================================================================== */
/* NAME : TRPT_Init							    							*/
/* 									    									*/
/* DESCRIPTION : Initialization Module					    				*/
/*		 Transport variables initialization			    					*/
/*		 Transport State Diagram initialization 	            			*/
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
int TRPT_Init(void)
{
	int i;

	t_evt = 0;
	t_NbConnection = 0;

	if (ITC_Init(TRANS) <= 0)
		return 0;

	for (i = 0; i < MAXTCID; i++)
		t_tab_tcid[i].state = NOT_USED;

#if 1 // #ifdef _INCLUDE_CI_TRPT_RECOVERY_
	HxSTD_memset(g_aucCiTrptTimoutRetryCnt, 0, MAXTCID);
#endif

	return 1;
}
/* End of Initialization Module */
/* ============================ */


/* ======================================================================== */
/* NAME : TRPT_End							    							*/
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
void TRPT_End(void)
{
	ITC_End();

	return;
}
/* End of Ending Module */
/* ============================ */


/* ======================================================================== */
/* NAME : TRPT_State													    */
/* 									    									*/
/* DESCRIPTION : Entry point of the Transport State Diagram Module			*/
/*		Read the event					       							    */
/*		Read the message if the event = MESSAGE		    				    */
/*		Find the current TCID and the matching state diagram index 			*/
/*		Call the current transition matches the received event	   			*/
/* 									   										*/
/* INPUT PARAMETERS :							   							*/
/*	t_evt provides the received event (MESSAGE or TIMER)		   			*/
/*	t_tab_tcid[i].state gives the current state of the State Diagram  		*/
/*	for the current TCID (index i)					    					*/
/*									   										*/
/* OUTPUT PARAMETERS :							    						*/
/*	t_tab_tcid[i].state is updated					    					*/
/*									  										*/
/* RETURN CODE :							  							    */
/*									 									    */
/* ======================================================================== */
void TRPT_State(void)
{
	int iRet = 0;				/* return code */
	unsigned char *pt = NULL;	/* pointer to allocated memory */
#ifdef FASTALLOC
	int dyn = 0;				/* 1 if TPDU received with dynamically allocated memory */
#endif

	if (t_evt == MESSAGE)
	{
		HxLOG_Print("TRPT_State : TRPT Message Type <0x%x>\n", t_header.type);
		switch (t_header.type) {
#ifdef FASTALLOC
			case TPDUDYN :		/* TPDU with dynamically allocated memory */
				dyn = 1;
#endif
			case TPDU :			/* Link <--> Transport */
				pt = t_msg.tpdu.ptpdu;
				iRet = TRPT_Tpdu();
		    	break;

		    case SPDU :			/* Transport <--> Session */
#ifdef FASTALLOC
				dyn = 1;
#endif
				pt = t_msg.spdu.pspdu;
				iRet = TRPT_Spdu();
		    	break;

			case TP_CONNECT :   /* Link ---> Transport */
				iRet = TRPT_Connect();
				break;

			case TP_DISCONNECT :/* Link ---> Transport */
				iRet = TRPT_Disconnect();
				break;

		    case SS_DELETE_TC :	/* Transport <--- Session */
				iRet = TRPT_Delete();
		    	break;

			default :			/* unknown message */
				HxLOG_Error("TRPT_State : Unknown message[0x%x] is ignored (error) !!!\n", t_header.type);
				break;
			}
	}
	else	/* TIMER */
	{
		ITC_KillTimer(t_timer);		/* cancel the timer */

		if ((t_timer > TIMEOUT) && (t_timer <= TIMEOUT + MAXTCID))
			/* timeout for a transport protocol request */
			iRet = TRPT_Timeout();
		else if ((t_timer > TIMEPOLL) && (t_timer <= TIMEPOLL + MAXTCID))
			/* ending period of the polling function for transport protocol */
			iRet = TRPT_TmPoll();
		else
			/* Unknown timer id */
			HxLOG_Error("TRPT_State : Unknown timerId[0x%x] is ignored (error) !!!\n", t_timer);

		t_timer = 0;
	}

	if (!iRet && pt)	// modified by jhcho
	{
#ifdef FASTALLOC
		if (dyn)
			ITC_DynFree(pt);	/* free memory used with last message received */
		else
			ITC_Free(pt);	  	/* free memory used with last message received */
#else
		ITC_DynFree(pt);	 	/* free memory used with last message received */
#endif
	}

	return;
}
/* End of Transport State Diagram */
/* ============================== */


/* ======================================================================== */
/* NAME : TRPT_Connect						    							*/
/* 									    									*/
/* DESCRIPTION : Transport State Diagram : TP_CONNECT transition received 	*/
/*		- Create a new state diagram										*/
/*		- Call the action													*/
/*		- Update the state diagram											*/
/*									    									*/
/* INPUT PARAMETERS :							    						*/
/*		t_msg.conn contains the TP_CONNECT message received from LINK Task */
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*		t_tab_tcid[] updated												*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int TRPT_Connect(void)
{
int index;	/* index of t_tab_tcid[] for this new transport connection  	*/
int mem;				/* memory management : return value					*/

	mem = 1;

	HxLOG_Print(("TRPT_Connect Start\n"));

#if defined(WIN32) && 0
	GL_TabSlot[GL_NbSlot++] = t_msg.conn.slot_id;
#endif

	/* Add a new instance of the Transport State Diagram for the new transport connection 			*/
	index = TRPT_GetIndex();		/* get the new instance for the transport state diagram 		*/
	if (index < 0)
		return 1;					/* can't get new instance, the request is ignored 				*/

	/* Initialize new transport state diagram */
	t_tab_tcid[index].slot_id = t_msg.conn.slot_id;

	/* Call the transport state diagram */
	if (!TRPT_Action(index, t_header.type, NULL, &mem))
		TRPT_FreeIndex(index);		/* error : free the new instance */

	return mem;
}


/* ======================================================================== */
/* NAME : TRPT_Disconnect						    						*/
/* 									    									*/
/* DESCRIPTION : Transport State diagram : TP_DISCONNECT received 			*/
/*	- Find all connections match with the disconnected module				*/
/*	- For each transport connection	:										*/
/*		call the action														*/
/*		free the state diagram												*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		t_msg.deconn contains the TP_DISCONNECT message received from LINK  */
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
int TRPT_Disconnect(void)
{
int index;	/* index of t_tab_tcid[] for the current transport connection  	*/
int mem;				/* memory management : return value					*/
unsigned short slot;	/* extracted slot									*/

	mem = 1;
	slot = t_msg.deconn.slot_id;
	/* repeat for all transport connections */
	for (index=0; index < MAXTCID; index++)
	{
		if ((t_tab_tcid[index].state != NOT_USED) && (t_tab_tcid[index].slot_id == slot))
		{
			if (TRPT_Action(index, t_header.type, NULL, &mem))
				TRPT_FreeIndex(index);/* free the instance of state diagram */
		}
	}

	return mem;
}


/* ======================================================================== */
/* NAME : TRPT_Tpdu								    						*/
/* 									    									*/
/* DESCRIPTION : Transport State diagram : TPDU received 					*/
/*		- Decod the TPDU													*/
/*		- Find the transport connection										*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		t_msg.tpdu contains the TPDU message received from LINK   			*/
/*		Task 																*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int TRPT_Tpdu(void)
{
unsigned char *ptpdu;	/* pointer to TPDU									*/
unsigned long ltpdu;	/* length of TPDU pointed by ptpdu					*/
unsigned char tag;		/* TPDU type 										*/
unsigned char tcid;		/* Transport connection								*/
tc_infos pinfos;		/* parameters of received TPDU 						*/
int mem;				/* memory management : return value					*/
unsigned long len;				/* length of decoded tpdu							*/
int nbtag;              /* number of decoded tpdu (max 2 : tpdu + t_sb)		*/
unsigned char *pt;		/* pointer to TPDU end								*/
int index;	/* index of t_tab_tcid[] for the current transport connection  	*/

	nbtag = 0;
	ptpdu = t_msg.tpdu.ptpdu;
	ltpdu = t_msg.tpdu.len;
	tcid  = t_msg.tpdu.tcid;
	pt    = NULL;

	/* Decod the TPDU */
	do {
		if ((len = TRPT_Decode(ptpdu, ltpdu, tcid, &tag, &pinfos)) == 0) // < 0)
		{	/* decoding error : the event is ignored */
			/* Send IHM_WARNING to IHM Task 		 */
			HxLOG_Error("TRPT_Tpdu : decoding error tag=0x%x tcid=%d iRet=%ld !!!\n", tag, tcid, len);

#if 0 // #ifdef _CI_DEBUG_
			{
				int i;
				HxLOG_Print("\n --- TPDU : ltpdu = %ld ---\n", ltpdu);
				for (i = 0; i < ltpdu; i++)
				{
					HxLOG_Print(" %02X", ptpdu[i]);
				}
				HxLOG_Print("\n\n");
			}
#endif

			return 0;
		}

#if defined(WIN32) && 0
		if (GL_Traces)
		{
			int fd;
			int j;
			for (j=0;j<(int)ltpdu;j++)
				wsprintf((char *)(ptbuff+(3*j)),"%02x ",*(ptpdu+j));	// One byte gives three ASCII chars
			ptbuff[(3*len)-1] = '\0';

			fd = _open("\\tmp\\dvb_ci.tra", _O_RDWR | _O_CREAT | _O_APPEND | _O_TEXT, S_IWRITE);
			wsprintf(bufile, "\nTRANS::TRPT_Tpdu\t %ld : received TPDU %s", GetTickCount(), ptbuff);
			_write(fd , bufile, VK_strlen(bufile));
			_close(fd);

			if (len > 50)
				ptbuff[200] = '\0';
			HxLOG_Print(("TRPT_Tpdu : received TPDU %s\n", ptbuff));

		}
#endif

		/* Find the transport connection */
		index = TRPT_GetTcid(tcid);
		if (index < 0)
		{	/* not found : the event is ignored */
			/* Send IHM_WARNING to IHM Task 	*/
			HxLOG_Error("TRPT_Tpdu : Unknown tcid %d !!!\n", tcid);
			return 0;			/* free memory  */
		}

		nbtag++;	 		/* one decoded tpdu */
		ptpdu += len;		/* skip to the next tpdu to decode */
		ltpdu -= len;

		if (ltpdu > 0)		/* maybe tpdu + t_sb */
		{
			/* Optmization : keep the end of TPDU in local memory to re-use t_msg in TRPT_Action() */
			/* ===========																		   */
#ifdef FASTALLOC
			pt = ITC_Alloc();							/* length = ltpdu */
#else
			pt = ITC_DynAlloc(ltpdu);	/* length = ltpdu */
#endif
			if (pt == NULL)
			{
				/* Send IHM_WARNING to IHM Task 	*/
				HxLOG_Error("TRPT_Tpdu : can't allocate buffer for save TPDU end (ltpdu = %ld) !!!\n", ltpdu);
				return 0;	/* Error */
			}
			HxSTD_memcpy(pt, ptpdu, ltpdu);
			ptpdu = pt;
		}

		/* Call the transport state diagram */
		if (!TRPT_Action(index, tag, &pinfos, &mem))
		{
			HxLOG_Error("TRPT_Tpdu : TRPT_Action error for tag 0x%02x !!!\n", tag);
			if (pt)
#ifdef FASTALLOC
				ITC_Free(pt);	  	/* free memory used with TPDU end  */
#else
				ITC_DynFree(pt);	/* free memory used with TPDU end  */
#endif
			return 0;		/* free memory  */
		}

	} while ((nbtag < 2) && (ltpdu > 0)); /* maybe tpdu + t_sb */

	if (pt)
#ifdef FASTALLOC
		ITC_Free(pt);	  	/* free memory used with TPDU end  */
#else
		ITC_DynFree(pt);	/* free memory used with TPDU end  */
#endif

    if (ltpdu != 0)
    {
		/* Send IHM_WARNING to IHM Task */
		HxLOG_Error("TRPT_Tpdu : Error while decoding tpdu : bad length !!!\n");
    	mem = 0;	/* free memory 		*/
    }

	/* Release the state diagram instance if the transport connection doesn't exist */
	if (t_tab_tcid[index].state == IDLE)
		TRPT_FreeIndex(index);

	return mem;
}



/* ======================================================================== */
/* NAME : TRPT_Spdu								    						*/
/* 									    									*/
/* DESCRIPTION : Transport State diagram : SPDU received 					*/
/*		- Find the transport connection										*/
/*		- Call the action													*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		t_msg.spdu contains the SPDU message received from SESS Task		*/
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int TRPT_Spdu(void)
{
int mem;				/* memory management : return value					*/
int index;	/* index of t_tab_tcid[] for the current transport connection  	*/

	/* Find the transport connection */
	index = TRPT_GetTcid(t_msg.spdu.tcid);
	if (index < 0)
	{	/* not found : the event is ignored */
		/* Send IHM_WARNING to IHM Task */
		HxLOG_Error("TRPT_Spdu : Unknown tcid %d !!!\n", t_msg.spdu.tcid);
		mem = 0;			/* free memory  */
	}
	else
	{
		/* Call the transport state diagram */
		if (!TRPT_Action(index, t_header.type, NULL, &mem))
			mem = 0;		/* free memory */
	}

	return mem;
}



/* ======================================================================== */
/* NAME : TRPT_Delete						    							*/
/* 									    									*/
/* DESCRIPTION : Transport State Diagram :SS_DELETE_TC transition received 	*/
/*		- Find the transport connection										*/
/*		- Call the action													*/
/*									    									*/
/* INPUT PARAMETERS :							    						*/
/*		t_msg.conn contains a SS_DELETE_TC message received from SESS Task */
/*									    									*/
/* OUTPUT PARAMETERS :							    						*/
/*		t_tab_tcid[] updated												*/
/*									    									*/
/* RETURN CODE :							    							*/
/*		0 : the calling function must free the memory containing the message*/
/*		1 : the calling function do nothing 								*/
/*									   										*/
/* ======================================================================== */
int TRPT_Delete(void)
{
int mem;				/* memory management : return value					*/
int index;	/* index of t_tab_tcid[] for the current transport connection  	*/

	/* Find the transport connection */
	index = TRPT_GetTcid(t_msg.del.tcid);
	if (index < 0)
	{	/* not found : the event is ignored */
		/* Send IHM_WARNING to IHM Task */
		mem = 0;			/* free memory  */
	}
	else
	{
		/* Call the transport state diagram */
		if (!TRPT_Action(index, t_header.type, NULL, &mem))
			mem = 0;		/* free memory */
	}

	return mem;
}



/* ======================================================================== */
/* NAME : TRPT_Timeout							    						*/
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
int TRPT_Timeout(void) // hmkim, 2010-03 : modified newly.
{
	int mem = 1;	/* memory management : return value */
	int index;		/* index of t_tab_tcid[] for the current transport connection */

	/* Find the transport connection */
	index = TRPT_GetTMTcid(t_timer);
	if (index < 0)
	{	/* not found : the event is ignored */
		/* Send IHM_WARNING to IHM Task */
		return 1;
	}

	HxLOG_Error("[CI_RW_ERROR] TRPT_Timeout : TM_TRANS=%d index=%d slot=%d state=%d tcid=%d timerid=%02x pollid=%02x tag=%02x IsCaSysId=%d CamReplied=%02x\n",
		TM_TRANS, index,		t_tab_tcid[index].slot_id, t_tab_tcid[index].state, t_tab_tcid[index].tcid, t_tab_tcid[index].timerid, t_tab_tcid[index].tmpollid, t_tab_tcid[index].tag,
		g_aucIsCaSysId[t_tab_tcid[index].slot_id],
		g_aucCamReplied[t_tab_tcid[index].slot_id]);

	/* Reset timerid (타이머 자체는 이 함수 진입 전에 이미 kill 된 상태) */
	t_tab_tcid[index].timerid = 0;

	/* 1. 캠이 응답을 주었으나 이후 호스트가 pending 되어 있는 경우 => 호스트 스택 상의 플로우 오류로 이런 일이 발생하면 디버깅 필요하나 이런 경우를 본 적이 없음. */
	/* 2. 캠이 응답을 주지 않는 경우 => 일단 polling 을 다시 해본다. 그러나 이런 경우에 복구되는 일은 거의 없다. */
	/* ================================================================== */

	if (t_tab_tcid[index].state == POLLING && g_aucCiTrptTimoutRetryCnt[index] < MAX_NUM_TRPT_TIMEOUT_RETRY)
	{
		unsigned short tmpollid = t_tab_tcid[index].tmpollid;
		int	nRet;

		g_aucCiTrptTimoutRetryCnt[index]++;

		HxLOG_Error("TRPT_Timeout : maybe %s is going to fall in coma... retry polling...\n", g_aucCamReplied[t_tab_tcid[index].slot_id] == 0 ? "cam" : "host");

		TRPT_Kill_PollTimer(index);

		nRet = TRPT_Active(index, TIMEPOLL, NULL, &mem);
		if ((nRet == 0) || (mem == 0))
		{
			HxLOG_Error("TRPT_Timeout : TRPT_Active(TIMEPOLL) return error -> nRet = %d, mem = %d\n", nRet, mem);

			if (mem == 0)
			{
				#ifdef FASTALLOC
				ITC_Free(t_msg.tpdu.ptpdu);
				#else
				ITC_DynFree(t_msg.tpdu.ptpdu);
				#endif
			}

			return 0;
		}

		if (tmpollid)
		{
			HxLOG_Error("TRPT_Timeout : TRPT_Set_PollTimer...\n");
			TRPT_Set_PollTimer(index);
		}

		return 1;
	}

	/* 결국 캠 리셋 */
	/* ======= */

	HxLOG_Error("TRPT_Timeout : maybe cam is going to fall in coma... reset cam...\n");

	/* Call the transport state diagram */
	TRPT_Action(index, TIMEOUT, NULL, &mem);

	/* Release the state diagram instance if the transport connection doesn't exist */
	if (t_tab_tcid[index].state == IDLE)
	{
		HxLOG_Error("TRPT_Timeout : TRPT_FreeIndex...\n");
		TRPT_FreeIndex(index);
	}

	/* Reset the module */
	if (CARM_CI_CheckModule((unsigned short)(t_tab_tcid[index].slot_id)) != eCAS_DEV_CI_EXTRACTED)
	{
		HxLOG_Error("TRPT_Timeout : svc_cas_DevCiResetModule(%d)...\n", t_tab_tcid[index].slot_id);

		if (svc_cas_DevCiResetModule(t_tab_tcid[index].slot_id) != ERR_OK)
		{
			HxLOG_Error("TRPT_Timeout : fail to reset slot %d !!!\n", t_tab_tcid[index].slot_id);
		}
	}

	return mem;
}


/* ======================================================================== */
/* NAME : TRPT_TmPoll							    						*/
/* 									    									*/
/* DESCRIPTION : Transport State diagram : TM_POLL received 				*/
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
int TRPT_TmPoll(void)
{
	int mem;	/* memory management : return value */
	int index;	/* index of t_tab_tcid[] for the current transport connection */

	/* Find the transport connection */
	index = TRPT_GetTMPollTcid(t_timer);
	if (index < 0)
	{	/* not found : the event is ignored */
		/* Send IHM_WARNING to IHM Task */
		return 1;
	}

	/* Reset tmpollid (타이머 자체는 이 함수 진입 전에 이미 kill 된 상태) */
	t_tab_tcid[index].tmpollid = 0;

	/* Call the transport state diagram */
	TRPT_Action(index, TIMEPOLL, NULL, &mem);

	return mem;
}


/* ======================================================================== */
/* NAME : TRPT_GetIndex					 									*/
/* 									    									*/
/* DESCRIPTION : Get a new entry in t_tab_tcid								*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		t_NbConnection is updated											*/
/*		the transport connection is reserved (t_tab_tcid[].tcid)			*/
/*									    									*/
/* RETURN CODE								    							*/
/*		< 0 : no more entry													*/
/*		>=0 : index in t_tab_tcid											*/
/*									  										*/
/* ======================================================================== */
int TRPT_GetIndex(void)
{
int index;				/* return code : entry found */
unsigned char newtcid;	/* new tcid to reserve 		 */
int i;					/* working index 			 */

	if (t_NbConnection >= MAXTCID)
		return -1;						/* too many established connections, no more entry 	*/

	for (index=0; index < MAXTCID; index++)
		if (t_tab_tcid[index].state == NOT_USED)   /* take the first free entry, reserve it */
		{
			/* Reserve a new tcid */
			for (newtcid=TCID; newtcid < MAXTCID+1; newtcid++)
				{
				for (i=0; i < MAXTCID; i++)
					if ((t_tab_tcid[i].state != NOT_USED) && (t_tab_tcid[i].tcid == newtcid))
						break;	/* this tcid already exists, try the next */
				if (i >= MAXTCID)
					break;		/* new tcid found */
				}

			if (newtcid == MAXTCID+1)
				return -1;		/* new tcid not found */

			t_tab_tcid[index].tcid 	   = newtcid;

			t_tab_tcid[index].state    = IDLE;
			t_tab_tcid[index].timerid  = 0;
			t_tab_tcid[index].tmpollid = 0;
			t_tab_tcid[index].len	   = 0;
			t_tab_tcid[index].pspdu	   = NULL;
			t_tab_tcid[index].rlen	   = 0;
			t_tab_tcid[index].rpspdu   = NULL;
			t_tab_tcid[index].tag      = 0;

			TRPT_ResetFifo(index);

			t_NbConnection++;

			HxLOG_Print("TRPT_GetIndex : new entry %d with tcid %d\n", index, newtcid);
			return index;
		}

	return -1;							   			/* no more entry */
}


/* ======================================================================== */
/* NAME : TRPT_FreeIndex					 								*/
/* 									    									*/
/* DESCRIPTION :Delete an entry in t_tab_tcid								*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index : index of the entry to delete								*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		t_NbConnection is updated											*/
/*									    									*/
/* RETURN CODE								    							*/
/*		< 0 : entry not found												*/
/*		>=0 : entry deleted													*/
/*									  										*/
/* ======================================================================== */
int TRPT_FreeIndex(int index)
{
    if ((index < 0) || (index >= MAXTCID)) // if ((index < 0) && (index >= MAXTCID))
    	return -1;

	HxLOG_Print("TRPT_FreeIndex : delete entry %d with tcid %d\n", index, t_tab_tcid[index].tcid);

	TRPT_Kill_Timer(index);							/* kill pending TIMEOUT timer */

	TRPT_Kill_PollTimer(index);						/* kill pending TM_POLL timer */

	TRPT_ResetFifo(index);

	t_tab_tcid[index].state = NOT_USED;				/* free the entry 		*/

	t_NbConnection--;

	return index;
}


/* ======================================================================== */
/* NAME : TRPT_GetTcid					 									*/
/* 									    									*/
/* DESCRIPTION : Get the index in t_tab_tcid								*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*	tcid :	transport connection (tcid)										*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		< 0 : tcid not found in t_tab_tcid									*/
/*		>=0 : index in t_tab_tcid											*/
/*									  										*/
/* ======================================================================== */
int TRPT_GetTcid(unsigned  char tcid)
{
int index;		/* return code : entry found */

	for (index=0; index < MAXTCID; index++)
		if ((t_tab_tcid[index].state != NOT_USED) && (t_tab_tcid[index].tcid == tcid))
			return index;   /* matching tcid  */

	return -1;	   			/* tcid not found */
}


/* ======================================================================== */
/* NAME : TRPT_GetTMTcid					 								*/
/* 									    									*/
/* DESCRIPTION : Get the index in t_tab_tcid according to a TIMEOUT timer   */
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*	timerid :	timer identifier											*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		< 0 : timerid not found in t_tab_tcid								*/
/*		>=0 : index in t_tab_tcid											*/
/*									  										*/
/* ======================================================================== */
int TRPT_GetTMTcid(unsigned short timerid)
{
	int index;		/* return code : entry found */

	for (index = 0; index < MAXTCID; index++)
	{
		if ((t_tab_tcid[index].state != NOT_USED) && (t_tab_tcid[index].timerid == timerid))
			return index;   /* matching timerid  */
	}

	HxLOG_Error("TRPT_GetTMTcid() : timerId[0x%02x] not found\n", timerid);

#if 1
	for (index = 0; index < MAXTCID; index++)
	{
		HxLOG_Print("%d : slot=%d state=%d tcid=%d timerid=%02x pollid=%02x tag=%02x\n",
			index, t_tab_tcid[index].slot_id, t_tab_tcid[index].state, t_tab_tcid[index].tcid, t_tab_tcid[index].timerid, t_tab_tcid[index].tmpollid, t_tab_tcid[index].tag);
	}
#endif

	return -1;	   			/* timerid not found */
}


/* ======================================================================== */
/* NAME : TRPT_GetTMPollTcid					 							*/
/* 									    									*/
/* DESCRIPTION : Get the index in t_tab_tcid according to a TM_POLL timer   */
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*	timerid :	timer identifier											*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		< 0 : timerid not found in t_tab_tcid								*/
/*		>=0 : index in t_tab_tcid											*/
/*									  										*/
/* ======================================================================== */
int TRPT_GetTMPollTcid(unsigned short timerid)
{
	int index;		/* return code : entry found */

	for (index = 0; index < MAXTCID; index++)
	{
		if ((t_tab_tcid[index].state != NOT_USED) && (t_tab_tcid[index].tmpollid == timerid))
			return index;   /* matching timerid  */
	}

	HxLOG_Error("TRPT_GetTMPollTcid() : timerId[0x%02x] not found\n", timerid);

#if 1
	for (index = 0; index < MAXTCID; index++)
	{
		HxLOG_Print("%d : slot=%d state=%d tcid=%d timerid=%02x pollid=%02x tag=%02x\n",
			index, t_tab_tcid[index].slot_id, t_tab_tcid[index].state, t_tab_tcid[index].tcid, t_tab_tcid[index].timerid, t_tab_tcid[index].tmpollid, t_tab_tcid[index].tag);
	}
#endif

	return -1;	   			/* timerid not found */
}

// add by chojh
// for robust handing Timeout
void TRPT_Set_Timer(int index)
{
	if (t_tab_tcid[index].timerid != 0)
	{
		ITC_KillTimer(t_tab_tcid[index].timerid);
		t_tab_tcid[index].timerid = 0;
	}

#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
	t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS, CI_TASK_ID_TRANSPORT);	/* do nothing if the timer can't be created *//* ? */
#else
	t_tab_tcid[index].timerid = ITC_SetTimer((unsigned short)(TIMEOUT + t_tab_tcid[index].tcid), TM_TRANS);	/* do nothing if the timer can't be created *//* ? */
#endif

	return;
}

void TRPT_Set_PollTimer(int index)
{
	if (t_tab_tcid[index].tmpollid != 0)
	{
		ITC_KillTimer(t_tab_tcid[index].tmpollid);
		t_tab_tcid[index].tmpollid = 0;
	}

#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
	t_tab_tcid[index].tmpollid = ITC_SetTimer((unsigned short)(TIMEPOLL + t_tab_tcid[index].tcid), TM_POLLING, CI_TASK_ID_TRANSPORT);	/* do nothing if the timer can't be created *//* ? */
#else
	t_tab_tcid[index].tmpollid = ITC_SetTimer((unsigned short)(TIMEPOLL + t_tab_tcid[index].tcid), TM_POLLING);	/* do nothing if the timer can't be created *//* ? */
#endif

	return;
}

/* ======================================================================== */
/* NAME : TRPT_Kill_Timer				 									*/
/* 									    									*/
/* DESCRIPTION : Kill the pending TIMEOUT timer if exists					*/
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
void TRPT_Kill_Timer(int index)
{
	if (t_tab_tcid[index].timerid != 0)
	{
		ITC_KillTimer(t_tab_tcid[index].timerid);
		t_tab_tcid[index].timerid = 0;
	}
	return;
}


/* ======================================================================== */
/* NAME : TRPT_Kill_PollTimer				 								*/
/* 									    									*/
/* DESCRIPTION : Kill the pending TIMEPOLL timer if exists					*/
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
void TRPT_Kill_PollTimer(int index)
{
	if (t_tab_tcid[index].tmpollid != 0)
	{
		ITC_KillTimer(t_tab_tcid[index].tmpollid);
		t_tab_tcid[index].tmpollid = 0;
	}
	return;
}


/* ======================================================================== */
/* NAME : TRPT_Send_Msg					 									*/
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
int TRPT_Send_Msg(void)
{
	HxLOG_Print("TRPT_Send_Msg <%x>\n", t_header.trans);

	return ITC_MsgSend(t_header.trans, (unsigned char *)&t_infos);
}


/* ======================================================================== */
/* NAME : TRPT_Send_Msg1				 									*/
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
int TRPT_Send_Msg1(void)
{
	HxLOG_Print("TRPT_Send_Msg1 <%x>\n", t_header1.trans);

	return ITC_MsgSend(t_header1.trans, (unsigned char *)&t_infos1);
}

