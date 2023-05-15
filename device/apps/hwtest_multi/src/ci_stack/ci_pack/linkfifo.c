/****************************************************************************/
/*                                                                          */
/*   FILE:    linkfifo.c	                                                */
/*   DATE:    27/06/97                                                      */
/*   PURPOSE: DVB-CI Link layer - tpdu FIFO management for LINK task  		*/
/*		there is a fifo by transport connection								*/
/*		The TPDU are not copied, only the pointer is copied					*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                 			    */
/*   Version    Date  		    Written by	     Notes	    	    		*/
/*	 1.0	    27/06/1997      SCM             Initial implementation 	    */
/*   1.05       28/12/1998      SCM             Function LINK_RazFifo()     */
/*                                                  renamed LINK_ResetFifo()*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*																		    */
/****************************************************************************/

#include "cipack_msg.h"		/* messages definitions 						*/
#include "itc.h"			/* Inter-Task Communication						*/
#include "linkfifo.h"
#include "ci.h"


/*--------------------------------------------------------------------------*/
/* External Variables		                                            	*/
/*--------------------------------------------------------------------------*/
extern LFIFO ln_tab_fifo[];	/* FIFO table									*/


/* ======================================================================== */
/* NAME : LINK_ResetFifo					 									*/
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
void LINK_ResetFifo(int index)
{
int iRet;	/* return code from LINK_GetFifo */
st_tpdu pt;	/* fifo entry					 */

	if (ln_tab_fifo[index].entry)
	{
		/* fifo not empty : delete the entries and the allocated memory */
		do
		{
			iRet = LINK_GetFifo(index, &pt);
			if (iRet)
				ITC_DynFree(pt.ptpdu);		/* free the allocated memory */
		}
		while (iRet);
	}

	ln_tab_fifo[index].head  = 0;
	ln_tab_fifo[index].tail  = 0;
	ln_tab_fifo[index].entry = 0;
	/* the fifo is empty */

	return;
}
/* ======================================================================== */
/* NAME : LINK_PutFifo					 									*/
/* 									    									*/
/* DESCRIPTION : Put a TPDU message in fifo		    						*/
/*		index :		instance of the current state diagram and for the fifo	*/
/*		pt :		TPDU to put in fifo										*/
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
int LINK_PutFifo(int index, st_tpdu *pt)
{

	if (ln_tab_fifo[index].entry >= MAXFIFOELT)
		return 0;		/* full fifo : Error */

	ln_tab_fifo[index].entry++;	/* one more element */

    /* element's copy in fifo */
    ln_tab_fifo[index].fifo[ln_tab_fifo[index].tail].tcid  = pt->tcid;
    ln_tab_fifo[index].fifo[ln_tab_fifo[index].tail].ptpdu = pt->ptpdu;
    ln_tab_fifo[index].fifo[ln_tab_fifo[index].tail].len   = pt->len;

	ln_tab_fifo[index].tail++;	/* tail is updated  */
	if (ln_tab_fifo[index].tail >= MAXFIFOELT)
		ln_tab_fifo[index].tail = 0;

	PrintDbg(("LINK_PutFifo : %d element(s) TPDU %d %ld 0x%p\n", ln_tab_fifo[index].entry, pt->tcid, pt->len, pt->ptpdu));

	return 1;	/* Success */
}

/* ======================================================================== */
/* NAME : LINK_GetFifo					 									*/
/* 									    									*/
/* DESCRIPTION : Get a TPDU saved in fifo	    							*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		index :		instance of the current state diagram and for the fifo	*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		pt :		TPDU get out the fifo									*/
/*									    									*/
/* RETURN CODE								    							*/
/*		1 Success															*/
/*		0 Error	(empty)														*/
/*									  										*/
/* ======================================================================== */
int LINK_GetFifo(int index, st_tpdu *pt)
{
	if (ln_tab_fifo[index].entry <= 0)
		return 0;		/* empty : Error */

	ln_tab_fifo[index].entry--;

    /* element's copy out fifo */
    pt->tcid  = ln_tab_fifo[index].fifo[ln_tab_fifo[index].head].tcid;
    pt->ptpdu = ln_tab_fifo[index].fifo[ln_tab_fifo[index].head].ptpdu;
    pt->len   = ln_tab_fifo[index].fifo[ln_tab_fifo[index].head].len;

	ln_tab_fifo[index].head++;	/* head is updated  */
	if (ln_tab_fifo[index].head >= MAXFIFOELT)
		ln_tab_fifo[index].head = 0;

	PrintDbg(("LINK_GetFifo : %d element(s) TPDU %d %ld 0x%p\n", ln_tab_fifo[index].entry, pt->tcid, pt->len, pt->ptpdu));

	return 1;	/* Success */
}

