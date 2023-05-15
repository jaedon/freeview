/****************************************************************************/
/*                                                                          */
/*   FILE:    tranfifo.c	                                                */
/*   DATE:    13/06/97                                                      */
/*   PURPOSE: DVB-CI Transport layer - spdu FIFO management for TRANS task  */
/*		there is a fifo by transport connection								*/
/*		The SPDU are not copied, only the pointer is copied					*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                 			    */
/*   Version    Date  		    Written by	     Notes	    	    		*/
/*	 1.0	    13/06/1997      SCM              Initial implementation 	*/
/*   1.05       28/12/1998      SCM              Renamed function:          */
/*                                     TRPT_RazFifo() into TRPT_ResetFifo() */
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*																		    */
/****************************************************************************/

#include "cipack_msg.h"		/* messages definitions 						*/
#include "itc.h"			/* Inter-Task Communication						*/
#include "tranfifo.h"
#include "ci.h"





/*--------------------------------------------------------------------------*/
/* External Variables		                                            	*/
/*--------------------------------------------------------------------------*/
extern FIFO t_tab_fifo[];	/* FIFO table									*/


/* ======================================================================== */
/* NAME : TRPT_ResetFifo					 								*/
/* 									    									*/
/* DESCRIPTION : clear the fifo associated with the transport connection	*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram and for the fifo	*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*									  										*/
/* ======================================================================== */
void TRPT_ResetFifo(int index)
{
unsigned char tag;	/* return code from TRPT_GetFifo */
st_spdu pt;			/* fifo entry					 */

	if (t_tab_fifo[index].entry)
	{
		/* fifo not empty : delete the entries and the allocated memory */
		do
		{
			tag = TRPT_GetFifo(index, &pt);
			if (tag == SPDU)
			{
				if(pt.pspdu != NULL)			/* 2009.05.21 bspark. prevent defect. */
				{
					ITC_DynFree(pt.pspdu);		/* free the allocated memory */
					pt.pspdu = NULL;
				}
			}
		}
		while (tag);
	}

	t_tab_fifo[index].head  = 0;
	t_tab_fifo[index].tail  = 0;
	t_tab_fifo[index].entry = 0;
	/* the fifo is empty */

	return;
}
/* ======================================================================== */
/* NAME : TRPT_PutFifo					 									*/
/* 									    									*/
/* DESCRIPTION : Put a SPDU message in fifo		    						*/
/*		index :		instance of the current state diagram and for the fifo	*/
/*		tag :		SPDU or SS_DELETE_TC									*/
/*		pt :		SPDU to put in fifo	if tag = SPDU						*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 Success															*/
/*		0 Error																*/
/*									  										*/
/* ======================================================================== */
int TRPT_PutFifo(int index, unsigned char tag, st_spdu *pt)
{
	if (t_tab_fifo[index].entry >= MAXFIFOELT)
		return 0;		/* full fifo : Error */

	t_tab_fifo[index].entry++;	/* one more element */

    /* element's copy in fifo */
    t_tab_fifo[index].tag[t_tab_fifo[index].tail] = tag;
    if (tag == SPDU)
	{
		t_tab_fifo[index].fifo[t_tab_fifo[index].tail].tcid  = pt->tcid;
		t_tab_fifo[index].fifo[t_tab_fifo[index].tail].pspdu = pt->pspdu;
		t_tab_fifo[index].fifo[t_tab_fifo[index].tail].len   = pt->len;
	}

	t_tab_fifo[index].tail++;	/* tail is updated  */
	if (t_tab_fifo[index].tail >= MAXFIFOELT)
		t_tab_fifo[index].tail = 0;

	if (tag == SPDU)
		HxLOG_Print("TRPT_PutFifo : %d element(s) SPDU %d %ld 0x%p\n", t_tab_fifo[index].entry, pt->tcid, pt->len, pt->pspdu);
	else
		HxLOG_Print("TRPT_PutFifo : %d element(s) SS_DELETE_TC\n", t_tab_fifo[index].entry);

	return 1;	/* Success */
}

/* ======================================================================== */
/* NAME : TRPT_GetFifo					 									*/
/* 									    									*/
/* DESCRIPTION : Get a SPDU saved in fifo	    							*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram and for the fifo	*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		pt :		SPDU get out the fifo									*/
/*									    									*/
/* RETURN CODE								    							*/
/*		>0 Success : Tag (SPDU or SS_DELETE_TC)								*/
/*		0 Error	(empty)														*/
/*									  										*/
/* ======================================================================== */
unsigned char TRPT_GetFifo(int index, st_spdu *pt)
{
	unsigned char tag;	/* SS_DELETE_TC or SPDU */

	if (t_tab_fifo[index].entry <= 0)
		return 0;		/* empty : Error */

	t_tab_fifo[index].entry--;

    /* element's copy out fifo */
	tag = t_tab_fifo[index].tag[t_tab_fifo[index].head];
	if (tag == SPDU)
	{
		pt->tcid  = t_tab_fifo[index].fifo[t_tab_fifo[index].head].tcid;
		pt->pspdu = t_tab_fifo[index].fifo[t_tab_fifo[index].head].pspdu;
		pt->len   = t_tab_fifo[index].fifo[t_tab_fifo[index].head].len;
	}

	t_tab_fifo[index].head++;	/* head is updated  */
	if (t_tab_fifo[index].head >= MAXFIFOELT)
		t_tab_fifo[index].head = 0;

	if (tag == SPDU)
		HxLOG_Print("TRPT_GetFifo : %d element(s) SPDU %d %ld 0x%p\n", t_tab_fifo[index].entry, pt->tcid, pt->len, pt->pspdu);
	else
		HxLOG_Print("TRPT_GetFifo : %d element(s) SS_DELETE_TC\n", t_tab_fifo[index].entry);

	return tag;
	/* Success */
}

