/****************************************************************************/
/*                                                                          */
/*   FILE:    transcod.c	                                                */
/*   DATE:    13/06/97                                                      */
/*   PURPOSE: DVB-CI Transport layer - TPDU Coding/Decoding Module			*/
/*			  conform to DVB-CI prEN 50221, june 1996						*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                 			    */
/*   Version    Date  		    Written by	     Notes	    	    		*/
/*	 1.0	    13/06/1997      SCM              Initial implementation 	*/
/*   1.5        29/12/1998      SCM            In function TRPT_SetLength() */
/*           the way to encode the length, if greater than 255, is modified */
/*                                             In function TRPT_GetLength() */
/*           the way to decode the length, if coded on 2 chars, is mofified */
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*		This module codes and decodes TPDU									*/
/*																		    */
/****************************************************************************/
//#include <memory.h>
//#include <string.h>

#include "dvb_cod.h"
#include "transcod.h"
#include "ci.h"

#include "ci_hwtest.h" //for CI HW_TEST

/* ======================================================================== */
/* NAME : TRPT_Code						 									*/
/* 									    									*/
/* DESCRIPTION : code a TPDU												*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		tag		: type of TPDU												*/
/*		tcid	: transport connection										*/
/*		pinfos	: parameters dependent of the tag							*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		tpdu	: pointer to coded TPDU	buffer								*/
/*									    									*/
/* RETURN CODE								    							*/
/*		 -1 -> 0 : error while coding											*/
/*		> 0 : length of coded TPDU											*/
/*									  										*/
/* ======================================================================== */
unsigned long TRPT_Code(unsigned char tag, unsigned char tcid, tc_infos *pinfos, unsigned char *ptpdu)
{
unsigned long iRet;				/* return code 				   */
unsigned char *pt;	    /* pointer to allocated memory */
unsigned char lenlen;				/* length of t_data_last / t_data_more length field	  */

	pt = ptpdu;		    /* points the allocated buffer */

	/* Coding */
	switch (tag) {
		case t_poll :
			*pt++ = t_data_last;	/* t_poll is a t_data_last without data field */
			*pt++ = 1;
			*pt   = tcid;
			iRet = 3;
			break;

//		여기서 처리되지 않는 것이다.
//		case t_sb :
//			iRet = 0;
//			break;

		case t_data_last :
		case t_data_more :
			*pt++ = tag;
			TRPT_SetLength(pt, pinfos->datatc.len, &lenlen);		/* code the length field */
			pt += lenlen;
			*pt++ = tcid;
			VK_memcpy(pt, pinfos->datatc.pbuff, pinfos->datatc.len-1); /* sub tcid length : 1 byte */
			iRet = 1 + lenlen + pinfos->datatc.len;
			break;

		case t_rcv:
		case create_t_c :
		case c_t_c_reply :
		case delete_t_c :
		case d_t_c_reply :
		case request_t_c :
			*pt++ = tag;
			*pt++ = 1;
			*pt   = tcid;
			iRet = 3;
			break;

		case new_t_c :
			*pt++ = tag;
			*pt++ = 2;
			*pt++ = tcid;
			*pt   = pinfos->newtc.new_t_c_id;
			iRet = 4;
			break;

		case t_c_error :
			*pt++ = tag;
			*pt++ = 2;
			*pt++ = tcid;
			*pt   = pinfos->errortc.error_code;
			iRet = 4;
			break;

		default :
			PrintErr(("TRPT_Code : Unknown Tag 0x%x !!!\n", tag));
			iRet = 0; // iRet = -1;
			break;
		}

	return iRet;				/* return the length of TPDU */
}


/* ======================================================================== */
/* NAME : TRPT_Decode						 								*/
/* 									    									*/
/* DESCRIPTION : decode a TPDU												*/
/* 									    									*/
/* INPUT PARAMETERS :							    						*/
/*		tpdu	: pointer to TPDU buffer to decode							*/
/*		len		: length of TPDU buffer to decode							*/
/*		tcid	: transport connection										*/
/*									 										*/
/* OUTPUT PARAMETERS :							   							*/
/*		tag		: type of TPDU												*/
/*		pinfos	: parameters dependent of the tag							*/
/*									    									*/
/* RETURN CODE								    							*/
/*		 -1 -> 0 : error while decoding											*/
/*		> 0 : length of decoded TPDU 										*/
/*									  										*/
/* ======================================================================== */
unsigned long TRPT_Decode(unsigned char *ptpdu, unsigned long len, unsigned char tcid, unsigned char *tag, tc_infos *pinfos)
{
unsigned char *pt;		/* pointer to TPDU 				*/
unsigned long length;	/* length of TPDU data field 	*/
unsigned char lenlen;	/* length of the length field	*/

	pt = ptpdu;

	*tag = *pt++;							/* TPDU's type 	*/

	if (TRPT_GetLength(pt, &length, &lenlen) < 0)	/* decode and get the length field */
	{
		PrintErr(("TRPT_Decode : TRPT_GetLength error !!!\n"));
		return 0; // return -1;
	}

	pt += lenlen;

	if (*pt++ != tcid)						/* Test the tcid */
	{
		PrintErr(("TRPT_Decode : tcid error !!!\n"));
		return 0; // return -1;
	}

#if 0
	switch(*tag) {
		case t_sb :
			pinfos->sbtc.SB_value = *pt & 0x80;	/* don't control the reserved field which must be set to 0 */
			break;

		case t_data_last :
		case t_data_more :
			pinfos->datatc.pbuff = pt;
			pinfos->datatc.len   = length - 1;
			break;

		case c_t_c_reply :
		case delete_t_c :
		case d_t_c_reply :
		case request_t_c :		/* no more informations */
			break;

		default :
			return 0; // return -1;		/* Error : unknown tpdu */
		}
#else
	switch(*tag) {
		case t_sb :
			if(length != (len - 2)) 			/* Test the length_field	added by thyou 2005. 10. 30 */
			{
				PrintErr(("TRPT_Decode : length error at %d lines !!!\n", __LINE__));
				return 0; // return -1;
			}
			pinfos->sbtc.SB_value = *pt & 0x80; /* don't control the reserved field which must be set to 0 */
			break;

		case t_data_last :
		case t_data_more :
			if((int)(len-2) < (int)length ) 		   /* Test the length_field    added by thyou 2005. 10. 30 */
			{
				PrintErr(("TRPT_Decode : length error at %d lines !!!\n", __LINE__));
				return 0; // return -1;
			}
			if(length == 0) 				   /* Test the length_field    added by thyou 2006. 05. 11 */
			{
				PrintErr(("TRPT_Decode : length error at %d lines !!!\n", __LINE__));
				return 0; // return -1;
			}
			pinfos->datatc.pbuff = pt;
			pinfos->datatc.len	 = length - 1;
			break;

		case c_t_c_reply :
		case delete_t_c :
		case d_t_c_reply :
		case request_t_c :		/* no more informations */
			if(lenlen != 0x01)		/* Test the length_field	added by thyou 2005. 10. 30 */
			{
				PrintErr(("TRPT_Decode : length error at %d lines !!!\n", __LINE__));
				return 0; // return -1;
			}
			break;

		default :
			PrintErr(("TRPT_Decode : unknown tpdu !!!\n"));
			return 0; // return -1;		/* Error : unknown tpdu */
		}
#endif

	return length + lenlen + 1; /* maybe less than len since t_sb can follow the first tpdu */
}


/* ======================================================================== */
/* NAME : TRPT_SetLength					 				*/
/* 									    			*/
/* DESCRIPTION : code the length field in TPDU						*/
/*												*/
/* INPUT PARAMETERS :							    		*/
/*		pt : 	 TPDU to code								*/
/*		length : length to code							*/
/*									 			*/
/* OUTPUT PARAMETERS :							   		*/
/*		lenlen : length field length						*/
/*												*/
/* NOTE :											*/
/* 		The MOST SIGNIFICANT BYTE is positionned first in the TPDU.		*/
/*									    			*/
/* RETURN CODE								    		*/
/* ======================================================================== */
void TRPT_SetLength(unsigned char *pt, unsigned long length, unsigned char *lenlen)
{
	/* <TODO_CI/TODO_CI_PLUS> : 우선 65535 바이트 이상의 데이터를 보낼 수 있도록 unsigned short 를 unsigned long 으로 수정하고 length_field_size 3, 4 인 경우에 대해 추가함.
								원칙적으로 스펙에 맞게 구현하려면 보다 근본적으로 수정해야 하는데...
								실제로 CI+ MHEG에서도 4294967295 바이트 이상의 데이터를 처리하지는 않을 것 같으므로 일단 이 정도로만 수정하도록 하자. */

	if (length <= 127)
	{
		*pt = (unsigned char)length;
		*lenlen = 1;
	}
	else if (length <= 255)
	{
		*pt = 0x81;
		*lenlen = 2;
		*(pt+1) = (unsigned char)length;
	}
	else if (length <= 65535)
	{
		*pt = 0x82;
		*lenlen = 3;
		*(pt+1) = (unsigned char) (length >> 8);
		*(pt+2) = (unsigned char) (length & 0x00FF);
	}
	else if (length <= 16777215)
	{
		*pt = 0x83;
		*lenlen = 4;
		*(pt+1) = (unsigned char) (length >> 16);
		*(pt+2) = (unsigned char) (length >> 8);
		*(pt+3) = (unsigned char) (length & 0x00FF);
	}
	else // if (length <= 4294967295)
	{
		*pt = 0x84;
		*lenlen = 5;
		*(pt+1) = (unsigned char) (length >> 24);
		*(pt+2) = (unsigned char) (length >> 16);
		*(pt+3) = (unsigned char) (length >> 8);
		*(pt+4) = (unsigned char) (length & 0x00FF);
	}

	return;
}


/* ======================================================================== */
/* NAME : TRPT_GetLength					 				*/
/* 												*/
/* DESCRIPTION : decode the length field in TPDU					*/
/* 									    			*/
/* INPUT PARAMETERS :									*/
/*		pt : 	 TPDU to decode							*/
/*												*/
/* OUTPUT PARAMETERS :									*/
/*		length : length field value						*/
/*		lenlen : length field length						*/
/*												*/
/* NOTE :											*/
/* 		The MOST SIGNIFICANT BYTE is positionned first in the TPDU.		*/
/*												*/
/* RETURN CODE										*/
/*		  1 : Success									*/
/*		 -1 : too big value	(> 65535 -> 4294967295)						*/
/*									  			*/
/* ======================================================================== */
int TRPT_GetLength(unsigned char *pt, unsigned long *length, unsigned char *lenlen)
{
	/* <TODO_CI/TODO_CI_PLUS> : 우선 65535 바이트 이상의 데이터를 받을 수 있도록 unsigned short 를 unsigned long 으로 수정하여 length_field_size 3, 4 인 경우에 대해 대응할 수 있도록 함.
								원칙적으로 스펙에 맞게 구현하려면 보다 근본적으로 수정해야 하는데...
								실제로 CI+ MHEG에서도 4294967295 바이트 이상의 데이터를 처리하지는 않을 것 같으므로 일단 이 정도로만 수정하도록 하자. */

#if 0 // org

	int iRet = 1;	/* return code */

	if ((*pt & 0x80) == 0x80)
	{							/* more than 1 byte 		  */
		*lenlen = *pt & 0x7f;
		if (*lenlen == 1)		/* value coded with 1 byte    */
		{
			*length = *(pt+1);
			(*lenlen)++;	    /* adjust length field length */
		}
		else
		{
			if (*lenlen == 2)   /* value coded with 2 bytes   */
			{
			*length = (((unsigned short)(*(pt+1))) << 8) +
                               (unsigned short)(*(pt+2));
			(*lenlen)++;		/* adjust length field length */
			}
			else
				iRet = -1;		/* too big value 			  */
		}
	}
	else
	{
		*length = *pt;
		*lenlen = 1;
	}

	return iRet;

#else

	unsigned char ucCnt;

	if (*pt & 0x80)
	{
		ucCnt = *lenlen = (*pt++ & 0x7f);
		if (ucCnt > 4)
			return -1; // 만약 이런 에러가 발생하는 상황이면 위 주석과 같이 근본적인 수정이 필요하다 !!
		(*lenlen)++;
		*length = 0;
		while(ucCnt--)
			*length = ((*length << 8) | *pt++);
	}
	else
	{
		*length = *pt;
		*lenlen = 1;
	}

	return 1;

#endif
}

