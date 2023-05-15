/****************************************************************************/
/*                                                                          */
/*   FILE:    linkstat.c	                                                */
/*   DATE:    26/06/97                                                      */
/*   PURPOSE: DVB-CI Link layer - state diagram for LINK task				*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                 			    */
/*   Version    Date  		    Written by	     Notes	    	    		*/
/*	 1.0	    26/06/1997      SCM              Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*																		    */
/****************************************************************************/

#include "link.h"               /* Main Module			  	*/
#include "itc.h"				/* Inter-Task Communication	*/
#include "linkstat.h"
#include "linkfifo.h"			/* FIFO Management Module 	*/
#include "ci.h"


/*--------------------------------------------------------------------------*/
/* External Variables		                                            	*/
/*--------------------------------------------------------------------------*/
extern xmsg ln_infos;		/* received informations	 					*/
extern xmsg ln_infos1;		/* other informations	 						*/
extern lslot ln_tab_slot[]; /* slot table   								*/
extern ltcid ln_tab_tcid[]; /* tcid table   								*/


/* ======================================================================== */
/* NAME : LINK_Action					 									*/
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
int LINK_Action(int index, unsigned char evt, int info, int *mem)
{
int iRet;			/* return code 				*/

	PrintDbg(("LINK_Action : index=%d state=%d evt=0x%x info=%d\n", index, ln_tab_slot[index].state, evt, info));

	switch(ln_tab_slot[index].state) {
		case CONNECTED : iRet = LINK_Connected(index, evt, info, mem);
			break;

		case SEND : 	 iRet = LINK_Send(index, evt, info, mem);
			break;

		case IDLE : 	 iRet = LINK_Idle(index, evt, mem);
			break;

		default :
			PrintErr(("LINK_Action : Unknown state %d !!!\n", ln_tab_slot[index].state));
			iRet = 0;	/* Error */
			break;
		}

	return iRet;
}


/* ======================================================================== */
/* NAME : LINK_Idle						 									*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the IDLE state						*/
/*		There is a new DVB module inserted . The Link layer sends the		*/
/*		connection indication to the TRANS layer							*/
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
int LINK_Idle(int index, unsigned char evt, int *mem)
{
int iRet;		/* return code  */

	*mem = 1;	/* default : keep memory */
	switch(evt) {
		case LN_CONNECT :
			PrintDbg(("LINK_Idle : LN_CONNECT Received\n"));

			/* send TP_CONNECT to the Transport Task */
			/* =============== */
			ln_header.trans		= (LINK << 4) | TRANS;
			ln_header.type     	= TP_CONNECT;
			ln_header.len      	= sizeof(st_deconn);
			ln_msg.conn.slot_id  = ln_tab_slot[index].slot;

		    /* Send the message */
    		if (! LINK_Send_Msg())
    		{
				PrintErr(("LINK_Idle : Can't send the TP_CONNECT for slot %d to TRANS Task !!!\n", ln_tab_slot[index].slot));
				return 0;		/* Error */
			}
			else
			{
		    	/* Set the new state */
				ln_tab_slot[index].state = CONNECTED;
				iRet = 1;		/* Success */
			}
			break;	/* end of LN_CONNECT processing */

		default :	/* protocol error, ignore the event */
			PrintErr(("LINK_Idle : protocol error . event %d received in %d state !!!\n", evt, ln_tab_slot[index].state));
			iRet = 0;
			break;
		}

	return iRet;
}	/* end of IDLE state */


/* ======================================================================== */
/* NAME : LINK_Connected						 							*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the CONNECTED state					*/
/*		- Send TPDU to PHYSICAL task into LPDU if no LPDU are waiting for	*/
/*		acknowlegde															*/
/*		- Receive LPDU from PHYSICAL task, assemble LPDU into complete		*/
/*		TPDU and send them to TRANS Task									*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		info :		conditionnal information according to the event			*/
/*			evt = TPDU /LPDU :  info = index of ln_tab_tcid					*/
/*			evt = LN_ERROR : 	info = status of error						*/
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
int LINK_Connected(int index, unsigned char evt, int info, int *mem)
{
int iRet = 1;		/* return code  								*/
int index_tcid;	/* index of ln_tab_tcid							*/
unsigned short len;		/* length of received LPDU		 		*/
unsigned char *plpdu;	/* pointer to the received LPDU			*/
unsigned char *pt;		/* pointer to reallocated memory		*/
unsigned char flag;		/* flag More/Last of the received LPDU 	*/

	*mem = 0;	/* default : free memory */

	switch(evt) {
		case LPDU :
			PrintDbg(("LINK_Connected : LPDU received\n"));

			index_tcid = info;
			flag  = ln_msg.lpdu.plpdu[1] & LN_MORE;
			len   = ln_msg.lpdu.len-2;
			plpdu = ln_msg.lpdu.plpdu + 2;

			if (flag ==	LN_MORE)
			{
				/* size control . the size must be equal to the negociated size since flag = MORE */
				if (ln_tab_slot[index].size != ln_msg.lpdu.len)
				{
					PrintErr(("LINK_Connected : bad size (%d) for intermediate received LPDU on slot %d . The negociated size is %d\n",
						ln_msg.lpdu.len, ln_tab_slot[index].slot, ln_tab_slot[index].size));
					return 0;	/* Error */
				}

				/* begin or concatenate a TPDU */
				if (ln_tab_tcid[index_tcid].lrecv == 0)
				{
					/* new TPDU to begin */
					/* ================= */
					ln_tab_tcid[index_tcid].precv = ITC_DynAlloc(len);
					if (ln_tab_tcid[index_tcid].precv == NULL)
					{
						PrintErr(("LINK_Connected : can't allocate buffer for coding TPDU !!!\n"));
						return 0;	/* Error */
					}
					ln_tab_tcid[index_tcid].lrecv = len;
					VK_memcpy(ln_tab_tcid[index_tcid].precv, plpdu, len);
				}
				else
				{
					/* Add the LPDU to the already received LPDU : reallocate the memory */
					/* =========================================						 */
#ifdef __ITC_USE_REALLOC
					pt = ITC_DynReAlloc(ln_tab_tcid[index_tcid].precv, ln_tab_tcid[index_tcid].lrecv + len);
					if (pt == NULL)
					{
						PrintErr(("LINK_Connected : can't reallocate buffer for coding TPDU !!!\n"));
						return 0;	/* Error */
					}
#else
					pt = ITC_DynAlloc(ln_tab_tcid[index_tcid].lrecv + len);
					if (pt == NULL)
					{
						PrintErr(("LINK_Connected : can't reallocate buffer for coding TPDU !!!\n"));
						return 0;	/* Error */
					}

					VK_memcpy(pt, ln_tab_tcid[index_tcid].precv, ln_tab_tcid[index_tcid].lrecv);
					ITC_DynFree(ln_tab_tcid[index_tcid].precv);
#endif
					ln_tab_tcid[index_tcid].precv = pt;
					VK_memcpy(ln_tab_tcid[index_tcid].precv + ln_tab_tcid[index_tcid].lrecv, plpdu, len);
					ln_tab_tcid[index_tcid].lrecv += len;
				}
#if 0 // #ifdef _CI_DEBUG_
				{
					int i;
					VK_Print("\n --- LN_MORE : lrecv = %d ---\n", ln_tab_tcid[index_tcid].lrecv);
					for (i = 0; i < ln_tab_tcid[index_tcid].lrecv; i++)
					{
						VK_Print(" %02X", ln_tab_tcid[index_tcid].precv[i]);
					}
					VK_Print("\n\n");
				}
#endif
			}
			else
			{	/* end of LPDU  : send the complete TPDU */
				if (ln_tab_tcid[index_tcid].lrecv == 0)
				{	/* one LPDU to code in TPDU and to send to the TRANS Task			*/
					/* ======================================================			*/
					/* Get ln_msg1 free buffer for TPDU message since ln_msg / ln_msg is for the LPDU */
				    /* Optimization : Fast small allocation since an only LPDU is more little than the fixed size of a memory segment */
				    /* ============	 (The fixed size of a memory segment is equal to the maximum negociated physical buffer size) added with a header message */
#ifdef FASTALLOC
					ln_msg1.tpdu.ptpdu = ITC_Alloc();		/* length = len */
#else
					ln_msg1.tpdu.ptpdu = ITC_DynAlloc(len);	/* length = len */
#endif
					if (ln_msg1.tpdu.ptpdu == NULL)
					{
						PrintErr(("LINK_Connected : can't allocate buffer for coding TPDU !!!\n"));
						return 0;	/* Error */
					}

					ln_header1.trans	 = (LINK << 4) | TRANS;
					ln_header1.type      = TPDU;
					ln_header1.len       = sizeof(st_tpdu);
					ln_msg1.tpdu.slot_id = ln_tab_slot[index].slot;
					ln_msg1.tpdu.tcid    = ln_tab_tcid[index_tcid].tcid;
					ln_msg1.tpdu.len	 = len;
					VK_memcpy(ln_msg1.tpdu.ptpdu, plpdu, len);
				}
				else
				{
					/* Add the LPDU to the already received LPDU : reallocate the memory */
					/* =========================================						 */
#ifdef __ITC_USE_REALLOC
					pt = ITC_DynReAlloc(ln_tab_tcid[index_tcid].precv, ln_tab_tcid[index_tcid].lrecv + len);
					if (pt == NULL)
					{
						PrintErr(("LINK_Connected : can't reallocate buffer for coding TPDU !!!\n"));
						return 0;	/* Error */
					}
#else
					pt = ITC_DynAlloc(ln_tab_tcid[index_tcid].lrecv + len);
					if (pt == NULL)
					{
						PrintErr(("LINK_Connected : can't reallocate buffer for coding TPDU !!!\n"));
						return 0;	/* Error */
					}

					VK_memcpy(pt, ln_tab_tcid[index_tcid].precv, ln_tab_tcid[index_tcid].lrecv);
					ITC_DynFree(ln_tab_tcid[index_tcid].precv);
#endif
					ln_tab_tcid[index_tcid].precv = pt;
					VK_memcpy(ln_tab_tcid[index_tcid].precv + ln_tab_tcid[index_tcid].lrecv, plpdu, len);
					ln_tab_tcid[index_tcid].lrecv += len;

					/* Send concatenated TPDU to TRANS Task */
					/* ==================================== */
					ln_header1.trans	 = (LINK << 4) | TRANS;
#ifdef FASTALLOC
					ln_header1.type      = TPDUDYN;			/* TPDU with dynamically allocated memory */
#else
					ln_header1.type      = TPDU;			/* TPDU with dynamically allocated memory */
#endif
					ln_header1.len       = sizeof(st_tpdu);
					ln_msg1.tpdu.slot_id = ln_tab_slot[index].slot;
					ln_msg1.tpdu.tcid    = ln_tab_tcid[index_tcid].tcid;
					ln_msg1.tpdu.len	 = ln_tab_tcid[index_tcid].lrecv;
					ln_msg1.tpdu.ptpdu   = ln_tab_tcid[index_tcid].precv;
#if 0 // #ifdef _CI_DEBUG_
					{
						int i;
						VK_Print("\n --- LN_LAST : lrecv = %d ---\n", ln_tab_tcid[index_tcid].lrecv);
						for (i = 0; i < ln_tab_tcid[index_tcid].lrecv; i++)
						{
							VK_Print(" %02X", ln_tab_tcid[index_tcid].precv[i]);
						}
						VK_Print("\n\n");
					}
#endif
				}

				/* Send the complete TPDU to TRANS Task */
				/* ==================================== */
    			if (! LINK_Send_Msg1())
    			{
					PrintErr(("LINK_Connected : Can't send the TPDU for slot %d to TRANS Task !!!\n", ln_tab_slot[index].slot));
#ifdef FASTALLOC
					if (ln_header1.type == TPDUDYN)
						ITC_DynFree(ln_msg1.tpdu.ptpdu);  	/* free memory used for the TPDU to send */
					else
						ITC_Free(ln_msg1.tpdu.ptpdu);	  	/* free memory used for the TPDU to send */
#else
					ITC_DynFree(ln_msg1.tpdu.ptpdu);	  	/* free memory used for the TPDU to send */
#endif
					iRet = 0;		/* Error   */
				}
				else
				{
					/* Reinitialize the precv since the TPDU is complete and sent to the TRANSPORT task */
					ln_tab_tcid[index_tcid].lrecv = 0;
					ln_tab_tcid[index_tcid].precv = NULL;

					/* Don't change the state  */
					iRet = 1;		/* Success */
				}
			}

			break;	/* end of LPDU processing */

		case TPDU :
			PrintDbg(("LINK_Connected : TPDU received\n"));

			index_tcid = info;

			/* Send LPDU to PHYSICAL Task */
			/* ========================== */
			iRet = LINK_SendLPDU(index, index_tcid, ln_tab_tcid[index_tcid].tcid, ln_msg.tpdu.ptpdu, ln_msg.tpdu.len);
			if (iRet)
			{	/* Success */
				if (ln_tab_tcid[index_tcid].lsend != 0)
					/* only a part of the TPDU has been sent */
					*mem = 1;	/* keep TPDU in memory for the next LPDU until complete sending */

			    	/* Set the new state */
				ln_tab_slot[index].state = SEND;	/* wait for the LN_ACK from PHYSICAL Task	*/
			}
			break;	/* end of TPDU processing */

		case LN_ERROR :
			PrintDbg(("LINK_Connected : LN_ERROR Received\n"));

			/* Send IHM_WARNING to IHM Task */
			if ((unsigned char)info == LN_PHYSICAL)
			{
				/* send TP_DISCONNECT to the Transport Task */
				/* ================== */
				ln_header.trans		= (LINK << 4) | TRANS;
				ln_header.type     	= TP_DISCONNECT;
				ln_header.len      	= sizeof(st_deconn);
				ln_msg.conn.slot_id = ln_tab_slot[index].slot;

			    /* Send the message */
    			if (! LINK_Send_Msg())
    			{
					PrintErr(("LINK_Connected : Can't send the TP_DISCONNECT for slot %d to TRANS Task !!!\n", ln_tab_slot[index].slot));
					return 0;		/* Error */
				}
				else
				{
			    	/* Set the new state */
					ln_tab_slot[index].state = IDLE;
					iRet = 1;		/* Success : LN_PHYSICAL */
				}
			}
			else
				iRet = 0;		/* Success : LN_I2C */
			break;	/* end of LN_ERROR processing   */

		case LN_DISCONNECT :
			PrintDbg(("LINK_Connected : LN_DISCONNECT Received\n"));

			/* send TP_DISCONNECT to the Transport Task */
			/* ================== */
			ln_header.trans		= (LINK << 4) | TRANS;
			ln_header.type     	= TP_DISCONNECT;
			ln_header.len      	= sizeof(st_deconn);
			ln_msg.disc.slot_id = ln_tab_slot[index].slot;

		    /* Send the message */
    		if (! LINK_Send_Msg())
    		{
				PrintErr(("LINK_Connected : Can't send the TP_DISCONNECT for slot %d to TRANS Task !!!\n", ln_tab_slot[index].slot));
				return 0;		/* Error */
			}
			else
		    	/* Set the new state */
				ln_tab_slot[index].state = IDLE;

			iRet = 1;		/* Success */
			break;	/* end of LN_DISCONNECT processing */

		default :	/* protocol error, ignore the event */
			PrintErr(("LINK_Connected : protocol error . event %d received in %d state !!!\n", evt, ln_tab_slot[index].state));
			iRet = 0;
			break;
		}

	return iRet;
}	/* end of CONNECTED state */


/* ======================================================================== */
/* NAME : LINK_Send								 							*/
/* 									    									*/
/* DESCRIPTION : Execute the action of the SEND state						*/
/*		- Put received TPDU into FIFO since the Link layer is waiting for a	*/
/*		acknowlegde of the precedent sent LPDU								*/
/*		- On received acknowlegde, send a LPDU of another tcid : 			*/
/*		multiplexing on the same slot										*/
/*		- Receive LPDU from PHYSICAL task, assemble LPDU into complete		*/
/*		TPDU and send them to TRANS Task									*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram					*/
/*		evt :		current event (transition)								*/
/*		info :		conditionnal information according to the event			*/
/*			evt = TPDU / LN_ACK : info = index of ln_tab_tcid				*/
/*			evt = LN_ERROR : 	  info = status of error					*/
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
int LINK_Send(int index, unsigned char evt, int info, int *mem)
{
int iRet = 1;				/* return code  		*/
int index_tcid;			/* index of ln_tab_tcid	*/
int i;					/* working index		*/
unsigned short slot;	/* current slot 		*/
int nb;					/* number of multiplexing try */

	*mem = 0;	/* default : free memory		*/

	switch(evt) {
		case LN_ACK:
			PrintDbg(("LINK_Send : LN_ACK received\n"));

			index_tcid = info;
			ln_tab_slot[index].send = 0;		/* no more LN_ACK to receive on this slot */

			/* Perform multiplexing on the current slot 			*/
			/* ========================================				*/
			/* Look for a tcid beginning by the tcid that follows 	*/
			/* the current one acknowledged .                       */
			/* A tcid is eligible if on this tcid  : 				*/
			/*		- a TPDU is being sent							*/
			/*		- a TPDU is in FIFO to be sent					*/

			slot = ln_tab_slot[index].slot;

			/* optimization if there is only one tcid on the current slot */
			/* ============												  */
			if (ln_tab_slot[index].nbtcid == 1)
				i = index_tcid;		/* take the same */
			else
			{
				i = index_tcid + 1;	/* beginning by the next tcid */
				if (i == MAXTCID)
					i = 0;
			}

			/* Perform the multiplexing on the current slot */
			/* ============================================ */
			nb = 0;
			do {
				if (ln_tab_tcid[i].slot == slot)
				{
					/* Search on the current slot */
					if (ln_tab_tcid[i].lsend > 0)
					{
						/* a TPDU is partially sent, take the next LPDU and */
						/* Send LPDU to PHYSICAL Task */
						/* ========================== */
						iRet = LINK_SendLPDU(index, i, ln_tab_tcid[i].tcid, NULL, 0);
//KJW - 2003.08.11, Should free tpdu memory after all relevant LPDUs are sent -> Solves descrambling prob. for AB sat. with Viaccess RED CAM
						if((ln_tab_tcid[i].lsend == 0) || (iRet != 1))
						{
#ifdef FASTALLOC
							ITC_Free(ln_tab_tcid[i].ptpdu);		/* Complete TPDU sent : free the memory */
#else
							ITC_DynFree(ln_tab_tcid[i].ptpdu);	/* Complete TPDU sent : free the memory */
#endif
						}
						break;		/* stop the loop : next eligible tcid found  */
					}
					else
					{   /* Search into FIFO, if a TPDU exists, Send the first LPDU */
						/* take mn_msg1 since ln_msg is for the received LN_ACK    */
						iRet = LINK_GetFifo(i, &(ln_msg1.tpdu));
						if (iRet)
						{
							/* Send LPDU to PHYSICAL Task */
							/* ========================== */
							iRet = LINK_SendLPDU(index, i, ln_tab_tcid[i].tcid, ln_msg1.tpdu.ptpdu, ln_msg1.tpdu.len);
							if((ln_tab_tcid[i].lsend == 0) || (iRet != 1))
							{
#ifdef FASTALLOC
								ITC_Free(ln_tab_tcid[i].ptpdu);		/* Complete TPDU sent : free the memory */
#else
								ITC_DynFree(ln_tab_tcid[i].ptpdu);	/* Complete TPDU sent : free the memory */
#endif
							}
							break;	/* stop the loop : next eligible tcid found  */
						}
					}
				}

				i++;	/* next tcid : multiplexing */
				nb++;
				if (i == MAXTCID)
					i = 0;
			}
			while (nb < MAXTCID);

			if (! ln_tab_slot[index].send)
		    	/* Set the new state */
				ln_tab_slot[index].state = CONNECTED;	/* no more LPDU to send on this slot and all LN_ACK received */

			break;  /* end of LN_ACK processing */

		case TPDU :
			PrintDbg(("LINK_Send : TPDU Received\n"));

			/* wait for an LN_ACK from a precedent LPDU sending */
			iRet = LINK_PutFifo(info, &(ln_msg.tpdu));
			if (iRet)
				*mem = 1;	/* Success : keep memory since the TPDU is not copied in FIFO, only the pointer */
			break;	/* end of TPDU processing */

		case LPDU :
			PrintDbg(("LINK_Send : LPDU Received\n"));

		case LN_ERROR :
			PrintDbg(("LINK_Send : LN_ERROR Received\n"));

		case LN_DISCONNECT :
			PrintDbg(("LINK_Send : LN_DISCONNECT Received\n"));

			/* same case that for the CONNECTED state */
			iRet = LINK_Connected(index, evt, info, mem);
			break;	/* end of LPDU / LN_ERROR / LN_DISCONNECT processing */

		default :	/* protocol error, ignore the event */
			PrintErr(("LINK_Send : protocol error . event %d received in %d state !!!\n", evt, ln_tab_slot[index].state));
			iRet = 0;
			break;
		}

	return iRet;
}	/* end of SEND state */


/* ======================================================================== */
/* NAME : LINK_SendLPDU							 							*/
/* 									    									*/
/* DESCRIPTION : Send a LPDU to the PHYSICAL Task							*/
/*									 										*/
/* INPUT PARAMETERS :							    						*/
/*		index :		 instance of the current state diagram / slot			*/
/*		index_tcid : instance of the current tcid							*/
/*		lpdu :		 pointer to the LPDU to send							*/
/*					 or NULL if the pointer is already in ln_tab_tcid		*/
/*		length :	 length of LPDU to send if lpdu != NULL					*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*	If the LPDU is the last (or the only one), free the memory containing	*/
/*	the complete TPDU														*/
/*	If the LPDU is not the last, update the ln_tab_tcid entry with pointer	*/
/*	to the next LPDU to send 												*/
/*	ln_tab_tcid[].tcid/send are updated										*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 : Success															*/
/*		0 : Error															*/
/*									  										*/
/* ======================================================================== */
int LINK_SendLPDU(int index, int index_tcid, unsigned char tcid, unsigned char *lpdu, unsigned long length)
{
unsigned char *pt;		/* pointer to LPDU to send 							*/
unsigned long len;		/* length of LPDU to send							*/
unsigned char flag;		/* MORE / LAST flag									*/

	PrintDbg(("LINK_SendLPDU Start\n"));

	if (lpdu == NULL)
	{	/* pointer to LPDU already in ln_tab_tcid */
		pt  = ln_tab_tcid[index_tcid].psend;
		len = ln_tab_tcid[index_tcid].lsend;
	}
	else
	{
		pt  = lpdu;
		len = length;

		/* Initialize ln_tab_tcid and save the TPDU pointer for further memory release */
        ln_tab_tcid[index_tcid].ptpdu = pt;
        ln_tab_tcid[index_tcid].psend = pt;
        ln_tab_tcid[index_tcid].lsend = len;
	}

	/* Check if the LPDU's size is greater then the negociateg physical buffer size */
	if ((len + 2) > ln_tab_slot[index].size)
	{	/* more than one LPDU */
		flag = LN_MORE;
		len = ln_tab_slot[index].size - 2;

		/* Update the ln_tab_tcid entry for the next LPDU to send */
		ln_tab_tcid[index_tcid].psend = pt + len;
		ln_tab_tcid[index_tcid].lsend -= len;
	}
	else
	{
		/* only one LPDU */
		flag = LN_LAST;

		ln_tab_tcid[index_tcid].lsend = 0;
//KJW - 2003.08.11, Should free tpdu memory after all relevant LPDUs are sent -> Solves descrambling prob. for AB sat. with Viaccess RED CAM
#if 0
		/* free TPDU memory since the TPDU is already sent */
		if (lpdu == NULL)
		{	/* TPDU pointer in ln_tab_tcid */
#ifdef FASTALLOC
			ITC_Free(ln_tab_tcid[index_tcid].ptpdu);
#else
			ITC_DynFree(ln_tab_tcid[index_tcid].ptpdu);
#endif
			ln_tab_tcid[index_tcid].ptpdu = NULL;
		}
#endif
	}

	/* send LPDU to the PHYSICAL Task */
	/* ============================== */
#ifdef FASTALLOC
	ln_msg1.lpdu.plpdu = ITC_Alloc();			/* A fast small allocation is used since the allocation size is equal to the maximum negociated size added to the header message . length = len + 2 */
#else
	ln_msg1.lpdu.plpdu = ITC_DynAlloc(len+2);	/* The allocation size is equal to the maximum negociated size added to the header message . length = len + 2 */
#endif
	if (ln_msg1.lpdu.plpdu == NULL)
	{
		PrintErr(("LINK_SendLPDU : can't allocate buffer for coding TPDU !!!\n"));
		return 0;	/* Error */
	}

	ln_header1.trans	 = (LINK << 4) | PHYSICAL;
	ln_header1.type      = LPDU;
	ln_header1.len       = sizeof(sl_lpdu);
	ln_msg1.lpdu.slot_id = ln_tab_slot[index].slot;
	ln_msg1.lpdu.len	 = len + 2;

	*(ln_msg1.lpdu.plpdu)     = tcid;
	*(ln_msg1.lpdu.plpdu + 1) = flag;
	VK_memcpy(ln_msg1.lpdu.plpdu + 2, pt, len);

	if (! LINK_Send_Msg1())
	{
		PrintErr(("LINK_SendLPDU : Can't send the LPDU for slot %d to PHYSICAL Task !!!\n", ln_tab_slot[index].slot));
#ifdef FASTALLOC
		ITC_Free(ln_msg1.lpdu.plpdu);	  	/* free memory used for the LPDU to send */
#else
		ITC_DynFree(ln_msg1.lpdu.plpdu);  	/* free memory used for the LPDU to send */
#endif
		return 0;		/* Error */
	}

	ln_tab_slot[index].tcid = tcid;		/* TPDU is sent on this tcid */
	ln_tab_slot[index].send = 1;		/* LPDU sending	in progress	 */

	return 1;			/* Success */
}
