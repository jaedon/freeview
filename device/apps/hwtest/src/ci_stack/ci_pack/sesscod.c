/****************************************************************************/
/*                                                                          */
/*   FILE:    sesscod.c		                                                */
/*   DATE:    02/07/97                                                      */
/*   PURPOSE: DVB-CI Session layer - SPDU Coding/Decoding Module			*/
/*			  conform to DVB-CI prEN 50221, june 1996						*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                 			    */
/*   Version    Date  		    Written by	     Notes	    	    		*/
/*	 1.0	    02/07/1997      SCM              Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*		This module codes and decodes TPDU									*/
/*																		    */
/****************************************************************************/
//#include <memory.h>

#include "dvb_cod.h"
#include "sesscod.h"
#include "ci.h"

/* ======================================================================== */
/* NAME : SRPT_Code						 									*/
/* 									    									*/
/* DESCRIPTION : code a SPDU												*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		tag		: type of SPDU												*/
/*		ssnb	: session number											*/
/*		pinfos	: parameters dependent of the tag							*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		spdu	: pointer to coded SPDU	buffer								*/
/*									    									*/
/* RETURN CODE								    							*/
/*		 -1 -> 0 : error while coding											*/
/*		> 0 : length of coded SPDU											*/
/*									  										*/
/* ======================================================================== */
unsigned long SESS_Code(unsigned char tag, unsigned short ssnb, tc_infos *pinfos, unsigned char *pspdu)

{
unsigned long iRet;				/* return code 				   */
unsigned char *pt;	    /* pointer to allocated memory */

	pt = pspdu;		    /* points the allocated buffer */

	/* Coding */
	switch (tag) {
		case session_number :
			*pt++ = tag;					/* tag 				*/
			*pt++ = 2;		            	/* length_field 	*/
			SESS_CodSsnb(ssnb, pt);     	/* session_number	*/
			pt += 2;
			VK_memcpy(pt, pinfos->datass.pbuff, pinfos->datass.len);	/* APDU */
			iRet = 4 + pinfos->datass.len;
			break;

		case open_session_response :
			*pt++ = tag;     				/* tag				*/
			*pt++ = 3 + SSLENRESS;     	 	/* length_field 	*/
			*pt++ = pinfos->openrss.status; /* session_status	*/
			VK_memcpy(pt, pinfos->openrss.ress_id, SSLENRESS);	/* resource_identifier */
			pt += SSLENRESS;
			SESS_CodSsnb(ssnb, pt);     	/* session_number	*/
			iRet = 5 + SSLENRESS;
			break;

		case close_session_request :
			*pt++ = tag;     				/* tag				*/
			*pt++ = 2;			     	 	/* length_field 	*/
			SESS_CodSsnb(ssnb, pt);     	/* session_number	*/
			iRet = 4;
			break;

		case close_session_response :
			*pt++ = tag;     				/* tag				*/
			*pt++ = 3;			     	 	/* length_field 	*/
			*pt++ = pinfos->closers.status; /* session_status	*/
			SESS_CodSsnb(ssnb, pt);     	/* session_number	*/
			iRet = 5;
			break;

		case create_session :
			*pt++ = tag;     				/* tag				*/
			*pt++ = 2 + SSLENRESS;     	 	/* length_field 	*/
			VK_memcpy(pt, pinfos->create.ress_id, SSLENRESS);	/* resource_identifier */
			pt += SSLENRESS;
			SESS_CodSsnb(ssnb, pt);     	/* session_number	*/
			iRet = 4 + SSLENRESS;
			break;

		default :
			PrintErr(("SESS_Code : Unknown Tag 0x%x !!!\n", tag));
			iRet = 0; // iRet = -1;
			break;
		}

	return iRet;				/* return the length of SPDU */
}


/* ======================================================================== */
/* NAME : SESS_Decode						 								*/
/* 									    									*/
/* DESCRIPTION : decode a SPDU												*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		spdu	: pointer to SPDU buffer to decode							*/
/*		len		: length of SPDU buffer to decode							*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		tag		: type of SPDU												*/
/*		ssnb	: session number											*/
/*		pinfos	: parameters dependent of the tag							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		 -1 -> 0 : error while decoding											*/
/*		 -2 -> 0 : bad length_field value										*/
/*		> 0 : length of decoded SPDU 										*/
/*									  										*/
/* ======================================================================== */
unsigned long SESS_Decode(unsigned char *pspdu, unsigned long len, unsigned char *tag, unsigned short *ssnb, tc_infos *pinfos)

{
unsigned char *pt;						/* pointer to SPDU 				*/
unsigned long length = 0;				/* length of SPDU data field 	*/

	pt = pspdu;

	*tag = *pt++;						/* SPDU's type 	*/

	length = *pt++;						/* length_field */

	switch(*tag) {
		case session_number :
			if (length != 2)
				return 0; // return -2;
			SESS_DecodSsnb(pt, ssnb);	/* session_number */
			pt += 2;
			pinfos->datass.pbuff = pt;	/* APDU			  */
			pinfos->datass.len = len - 4;
			pinfos->datass.ssnb = *ssnb;
			length = len;
			break;

		case open_session_request :
			if (length != SSLENRESS)
				return 0; // return -2;
			VK_memcpy(pinfos->openss.ress_id, pt, SSLENRESS);	/* resource_identifier */
			if(len != SSLENRESS + 2)
				return 0; // return -2;
			else
				length = len;
			break;

		case create_session_response :
			if (length != SSLENRESS + 3)
				return 0; // return -2;
			pinfos->creatr.status = *pt++;					/* session_status 	   */
			VK_memcpy(pinfos->creatr.ress_id, pt, SSLENRESS);	/* resource_identifier */
			pt += SSLENRESS;
			SESS_DecodSsnb(pt, ssnb);						/* session_number	   */
			pinfos->creatr.ssnb = *ssnb;
			if(len != SSLENRESS + 5)
				return 0; // return -2;
			else
				length = len;
			break;

		case close_session_request :
			if (length != 2)
				return 0; // return -2;
			SESS_DecodSsnb(pt, ssnb);						/* session_number	   */
			pinfos->closess.ssnb = *ssnb;
			if(len != 4)
				return 0; // return -2;
			else
				length = len;
			break;

		case close_session_response :
			if (length != 3)
				return 0; // return -2;
			pinfos->closers.status = *pt++;					/* session_status 	   */
			SESS_DecodSsnb(pt, ssnb);						/* session_number	   */
			pinfos->closers.ssnb = *ssnb;
			if(len != 5)
				return 0; // return -2;
			else
				length = len;
			break;

		default :
			length = 0; // length = -1;		/* Error : unknown spdu */
			break;
		}

	return length;
}


/* ======================================================================== */
/* NAME : SESS_CodSsnb						 								*/
/* 									    									*/
/* DESCRIPTION : code the session number field in SPDU						*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		ssnb : SSNB to code in SPDU											*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		pspdu :	 coded SPDU													*/
/*									    									*/
/* RETURN CODE								    							*/
/* ======================================================================== */
void SESS_CodSsnb(unsigned short ssnb, unsigned char *pspdu)
{
	*pspdu     = (unsigned char)ssnb >> 8;
	*(pspdu+1) = (unsigned char)ssnb;

	return;
}



/* ======================================================================== */
/* NAME : SESS_DecodSsnb						 							*/
/* 									    									*/
/* DESCRIPTION : decode the session number field of SPDU					*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		pspdu :	 coded SPDU													*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		ssnb : SSNB to decode in SPDU										*/
/*									    									*/
/* RETURN CODE								    							*/
/* ======================================================================== */
void SESS_DecodSsnb(unsigned char *pspdu, unsigned short *ssnb)
{
	*ssnb = ((unsigned short)*pspdu * 256) + (unsigned short)*(pspdu+1);

	return;
}


