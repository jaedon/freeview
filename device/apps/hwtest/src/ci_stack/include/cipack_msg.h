/****************************************************************************/
/*                                                                          */
/*   FILE:    message.h	                                                    */
/*   DATE:    13/06/97                                                      */
/*   PURPOSE: Messages Definitions 											*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    13/06/1997      SCM              Initial implementation 	*/
/*	 2.0		31/07/1997		SCM         	 Acces to PHYSICAL Task		*/
/*								instead of PHYSICAL Driver					*/
/*	 1.06		01/02/1999		SCM         	 Add st_drvheader structure	*/
/*									 st_drvheader.type : char ---> short	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __MESSAGE_H
#define __MESSAGE_H

//#include	<stdlib.h>
//#include	<string.h>
#include	<vkernel.h>

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/
#define MAXTCID		8       /* Max count of transport connections 					*/
#define MAXSSNB	   32		/* Max count of sessions								*/

#ifndef NULL
#define NULL		(void *)0
#endif

#define PHYSICAL	1		/* Physical Task								*/
#define LINK		2      	/* Link Task									*/
#define TRANS		3       /* Transport Task								*/
#define SESS		4       /* Session Task                                 */
#define RM			5       /* RM Task										*/
#define CIRH		5		/* added by shinhs to port SCM src  */

/*--------------------------------------------------------------------------*/
/* Messages Definitions                                                     */
/*--------------------------------------------------------------------------*/

/* Messages tags */
/* Physical Layer */
#define PH_INSERT		0x01
#define PH_DATA			0x02
#define PH_ERROR		0x03
#define PH_EXTRACT		0x04
/* end og physical layer */

/* Link Layer */
#define LN_CONNECT		0x11
#define LN_DISCONNECT	0x12
#define LPDU			0x13
#define LN_ERROR		0x14
#define LN_ACK			0x15

/* status from LN_ERROR */
#define LN_OK			1	/* Success 				*/
#define LN_PHYSICAL		2	/* hardware layer error */
#define LN_I2C			3	/* I2C layer error		*/
/* end of link layer */

/* Transport Layer */
#define TP_CONNECT		0x21
#define TP_DISCONNECT	0x22
#define TPDU			0x23
#ifdef FASTALLOC
#define TPDUDYN			0x24
#endif
/* end of transport layer */

/* Session Layer */
#define SS_TC_CREATED	0x31
#define SS_DELETE_TC	0x32
#define SS_TC_DELETED	0x33
#define SPDU			0x34
/* end of session layer */

/* Resource Manager layer */
#define RM_TC_CREATED	0x41
#define RM_OPEN_SS_REQ	0x42
#define RM_OPEN_SS_RSP	0x43
#define RM_SS_OPENED	0x44
#define APDU			0x45
#define RM_CLOSE_SS		0x46
#define RM_SS_CLOSED	0x47
#define RM_DELETE_TC	0x48
#define RM_TC_DELETED	0x49


/* Timeout Messages Base */
#define TIMEOUT			0x50	/* Transport layer timeout					*/
#define TIMEPOLL		0x60	/* Transport polling timeout				*/
/* Ex : TIMEOUT/TIMEPOLL for tcid 3 = TIMEOUT/TIMEPOLL + tcid = 0x53/0x63	*/

#define TIMESESS		0x70	/* Session layer timeout					*/
#define SSNBTIMER		  16	/* simultaneous TIMESESS timer count		*/
/* Ex : TIMESESS : unique identifier between TIMESESS and TIMESESS + 16	--> 16 simultaneous active TIMESESS timers */

/* WARNING : General messages values must be less than DVB messages values  */
#define MSG_MAXVAL		0x7f

/* DVB-CI Messages */
#include "dvb_cod.h"


/* Messages structures */
/* Physical Layer */
typedef struct {        		/* PH_INSERT                                */
	unsigned short slot_id;		/* physical reference for DVB module        */
	unsigned char  status;		/* result of module's connection			*/
	unsigned short size;		/* negociated physical buffer size			*/
	} ph_insert;

typedef struct {        		/* PH_ERROR                                 */
	unsigned short slot_id;		/* physical reference for DVB module        */
	unsigned char  status;		/* error's type								*/
	} ph_error;

typedef struct {        		/* PH_DATA                                  */
	unsigned short slot_id;		/* physical reference for DVB module        */
	unsigned short len;			/* length of the LPDU buffer following the structure */
	unsigned char pdata[1];		/* physical buffer beginning				*/
	} ph_data;

typedef struct {        		/* PH_EXTRACT                               */
	unsigned short slot_id;		/* physical reference for DVB module        */
	} ph_extract;

/* Link Layer */
typedef struct {        		/* LN_CONNECT                               */
	unsigned short slot_id;		/* physical reference for DVB module        */
	unsigned char  status;		/* result of module's connection			*/
	unsigned short size;		/* negociated physical buffer size			*/
	} sl_conn;

typedef struct {        		/* LPDU                                     */
	unsigned short slot_id;		/* physical reference for DVB module        */
	unsigned short len;			/* length of the LPDU buffer                */
	unsigned char  *plpdu;		/* pointer of a LPDU buffer                 */
	} sl_lpdu;

typedef struct {		        /* LN_ERROR     	                        */
	unsigned short slot_id;		/* physical reference for DVB module        */
	unsigned char  status;		/* error's type								*/
	} sl_error;

typedef struct {		        /* LN_ACK		                            */
	unsigned short slot_id;		/* physical reference for DVB module        */
	} sl_ack;

typedef struct {		        /* LN_DISCONNECT                            */
	unsigned short slot_id;		/* physical reference for DVB module        */
	} sl_disc;
/* end of Link layer */

/* Transport Layer */
typedef struct {        		/* TP_CONNECT                               */
	unsigned short slot_id;		/* physical reference for DVB module        */
	} st_conn;

typedef struct {		        /* TP_DISCONNECT                            */
	unsigned short slot_id;		/* physical reference for DVB module        */
	} st_deconn;

typedef struct {        		/* TPDU                                     */
	unsigned short slot_id;		/* physical reference for DVB module        */
	unsigned char tcid;			/* transport connection identifier          */
	unsigned char *ptpdu;		/* pointer of a TPDU buffer                 */
	unsigned long len;			/* length of the TPDU buffer                */
	} st_tpdu;
/* end of Transport layer */

/* Session Layer */
typedef struct {        		/* SS_TC_CREATED                            */
	unsigned char tcid;			/* transport connection identifier          */
	} st_creat;

typedef struct {       			/* SPDU                                     */
	unsigned char tcid;			/* transport connection identifier          */
	unsigned char *pspdu;		/* pointer of a SPDU buffer                 */
	unsigned long len;			/* length of the SPDU buffer                */
	} st_spdu;

typedef struct {       			/* SS_DELETE_TC                             */
	unsigned char tcid;			/* transport connection identifier          */
	} st_delete;

typedef struct {        		/* SS_TC_DELETED                            */
	unsigned char tcid;			/* transport connection identifier			*/
	} st_deleted;
/* end of Session layer */

/* Session Layer */
typedef struct {        		/* RM_TC_CREATED                            */
	unsigned char tcid;			/* transport connection identifier          */
	} sr_creat;

typedef struct {       			/* RM_DELETE_TC                             */
	unsigned char tcid;			/* transport connection identifier          */
	} sr_delete;

typedef struct {        		/* RM_TC_DELETED                            */
	unsigned char tcid;			/* transport connection identifier          */
	} sr_deleted;

typedef struct {        		/* RM_OPEN_SS_REQ                           */
	unsigned char tcid;			/* transport connection identifier          */
	unsigned short ssnb;		/* session number							*/
	unsigned char ress_id[SSLENRESS];	/* resource identifier				*/
	} sr_open;

typedef struct {        		/* RM_OPEN_SS_RSP                           */
	unsigned short ssnb;		/* session number							*/
	unsigned char status;		/* status of the response					*/
	unsigned char rtcid;		/* tcid routing								*/
 	} sr_openr;

typedef struct {				/* RM_SS_OPENED								*/
	unsigned short ssnb;		/* session number							*/
	unsigned char  tcid;		/* tcid 									*/
 	} sr_opened;

typedef struct {				/* RM_CLOSE_SS								*/
	unsigned short ssnb;		/* session number							*/
	} sr_close;

typedef struct {				/* RM_SS_CLOSED								*/
	unsigned short ssnb;		/* session number							*/
	unsigned char status;		/* status of the closing					*/
	} sr_closed;

typedef struct {				/* APDU										*/
	unsigned short ssnb;		/* session number							*/
	unsigned char *papdu;		/* pointer of a rPDU buffer                 */
	unsigned long len;			/* length of the RPDU buffer                */
	} sr_apdu;

// leesy OAK TFT
typedef struct					/* MPDU										*/
{
	unsigned char res;			/* receiver resource number					*/
	unsigned char cmd;			/* command									*/
	unsigned char rIndex;		/* resource index							*/
	unsigned char choice;
	unsigned char *ptr;
} XMSG_MPDU;

/* end of Resource Manager layer */

/* DVB Messages */
typedef union {
	  ph_insert  insert;		/* Physical Layer						*/
	  ph_data	 data;
	  ph_error	 error;
	  ph_error	 extract;
	  sl_conn	 lconn;			/* Link Layer							*/
	  sl_lpdu	 lpdu;
	  sl_error	 errhw;
	  sl_disc	 disc;
	  sl_ack	 ack;
	  st_conn    conn;			/* Transport Layer						*/
	  st_deconn  deconn;
	  st_tpdu    tpdu;
	  st_creat   creat;
	  st_spdu	 spdu;
	  st_delete  del;
	  st_deleted deleted;
	  sr_creat	 screat;		/* Session Layer						*/
	  sr_open	 open;
	  sr_openr	 openrsp;
	  sr_opened	 opened;
	  sr_apdu	 apdu;
	  sr_close	 close;
	  sr_closed  closed;
	  sr_delete  sdel;
	  sr_deleted sdeleted;
	  unsigned char *ptmess;	/* message too long : pointer on memory */
// leesy OAK TFT
	  XMSG_MPDU  mpdu;
	} dvb_msg;

/* Message's Header */
typedef struct {
	unsigned char trans;        /* Message's Sender (4 left bits) / Receiver (4 right bits)	*/
	unsigned char type;			/* DVB Message's Type										*/
	unsigned short len;	        /* DVB Message's Length										*/
	} st_header;

/* Inter-Task Message Structure */
typedef struct {
	st_header 	header;
	dvb_msg 	body;
	} MESSAGE;

/* Message from Physical Driver Header */
typedef struct {
	unsigned short type;		/* DVB Message's Type										*/
	unsigned short len;	        /* DVB Message's Length										*/
	} st_drvheader;

/* Received buffer from Physical Driver */
typedef struct {
	st_drvheader header;
	dvb_msg	body;
	} DRV;

/* Kind of received informations according to received evenements			*/
typedef union {
	MESSAGE	 message;		/* message 										*/
	unsigned short	timer;	/* timer identifier   							*/
	DRV *phwinfos;			/* pointer to physical data from PHYSICAL driver*/
	} xmsg;		/* received Informations									*/


#endif	// !__MESSAGE_H
