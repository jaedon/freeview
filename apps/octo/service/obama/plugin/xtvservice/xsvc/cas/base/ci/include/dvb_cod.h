/****************************************************************************/
/*                                                                          */
/*   FILE:    dvb_cod.h	                                                    */
/*   DATE:    13/06/97                                                      */
/*   PURPOSE: DVC-CI structures definitions	conform to prEN 50221, june 1996*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    13/06/1997      SCM              Initial implementation 	*/
/*	 1.01		31/07/1997		SCM          	 Acces to PHYSICAL Task		*/
/*								instead of PHYSICAL Driver					*/
/*	1.03		19/09/1997		SCM          	 special I2C tpt timeout	*/
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __DVB_COD_H
#define __DVB_COD_H

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/
/* Link Layer */
#define LN_MORE		0x80	/* bit More for LPDU */
#define LN_LAST		0x00	/* bit Last for LPDU */

#if defined(CONFIG_MW_CI_PLUS)
#define MAXLNSIZE		65535	/* CI+ spec에 맞춘것으로 DI도 65535 bytes 로 buffer를 가져가기로함..  최소는 1024bytes 만 그냥 놓아 두도록 한다. "ci_plus_specification_v1.3.1 G.1  Buffer Size"  */
#else
#define MAXLNSIZE		2048	/* maximum physical buffer size : 256 (org) -> 512 (for SmarDTV CI+ test kit) -> 1024 (for Digital Keystone HPNX DVB CI+ test kit) -> 2048 (for SmarCAM-3) */
#endif
#define MINLNSIZE		16		/* minimum physical buffer size in case of module (host shall support a minimum buffer size of 256 bytes) */

/* Transport Layer */
#define t_sb			0x80
#define t_rcv			0x81
#define create_t_c		0x82
#define c_t_c_reply	0x83
#define delete_t_c		0x84
#define d_t_c_reply	0x85
#define request_t_c	0x86
#define new_t_c		0x87
#define t_c_error		0x88
#define t_data_last	0xa0
#define t_poll			0xa2    /* special t_data_last without data field */
#define t_data_more	0xa1

#define NOTRANS		1		/* error_code in t_c_error */

#define DATA_AVAILABLE 0x80	/* t_sb(1) */

#define MAXTPDUSIZE	4096	/* maximum size for a TPDU (t_data_last or t_data_more) */ /* 2013.11.05. 1024byte -> 4096byte (불필요하게 TPDU단에서 data를 fragment 하여 전송하는것 방지) */
#define T_DATA_HEADER 10		/* overhead in t_data_last / t_data_more before SPDU (tag + length_field + tcid) */

#define S_CREATE_T_C	3
#define S_DELETE_T_C	3
#define S_D_T_C_REPLY	3
#define S_POLL			3

#if 0 // org

#ifdef _DEBUG
#define TM_TRANS	10000	/* Timeout Value in ms */
#define TM_POLLING	400		/* Polling Timeout in ms */
#else
#ifdef I2C
#define TM_TRANS	3000	/* Timeout Value in ms */
#else
#define TM_TRANS	10000	/* modified by shinhs 09-24-01 (5000) */ /* Timeout Value in ms (recommend is 300ms.) */
#endif
#define TM_POLLING	400		/* modified by shinhs 09-24-01 (100) */ /* Polling Timeout in ms (recommend is 10ms.) */
#endif
/* 여기 ...TM_POLLING이랑 TM_TRANS가 값이 변경됨...몬지 알아봐....*/ /* --> 이때 각기 400, 10000 으로 변경된 것으로 보임 */

#else
/* 수정 이력 :
   hmkim, 2008-10-01 : TM_TRANS 값 변경. TM_POLLING 값 검토.
   hmkim, 2009-06-17 : TM_POLLING 400 -> 100, ITC_TIMEOUT 50 -> 20 으로 변경 (부팅에서 디스크램블링까지 2초 단축) 했다가 Douglas 의 경우 CI+ 캠 초기화시 SAC 에러 내지는 full authentication 시 에러 발생하여 원복함.
   hmkim, 2010-01-28 : 브로드컴의 경우에도 더글라스와 마찬가지... 100 ms 적용하면 Canal Ready 용 Neotion CI+ 캠의 경우 NFS 모드시 캠 삽입한 상태에서 init notify 가 안올라옴. */

/* 스펙에는 300ms로 규정되어 있으나 기존에 10000ms나 사용하고 있었음. 그러나 poll response가 이보다 늦는 경우가 있어 값을 더 늘림 (15000) */
#define TM_TRANS		15000

/* 스펙에는 100ms로 규정되어 있으나 기존에 400ms를 사용하고 있었음. 별다른 문제가 없으므로 기존 값 그대로 유지함 -> see ITC_TIMEOUT also */
#if 1
#define TM_POLLING		400
#else
#define TM_POLLING		100		/* org : 400 */
#endif

#endif

/* Session Layer */
#define open_session_request		0x91
#define open_session_response	0x92
#define create_session			0x93
#define create_session_response	0x94
#define close_session_request	0x95
#define close_session_response	0x96
#define session_number			0x90

#define SSLENRESS			4	/* length of a resource identifier				*/

#define SSOPENRSPSIZE		9	/* open_session_response size					*/
#define SSCLOSESIZE		4	/* close_session_request size					*/
#define SSCLOSERSPSIZE	5	/* close_session_response size					*/
#define SSESSIONSIZE		4	/* session_number size wihtout data				*/

#ifdef _DEBUG
#define TM_SESS	10000		/* Timeout Value in ms 							*/
#else
#define TM_SESS	1000		/* Timeout Value in ms 							*/
#endif

/* session_status values */
#define SS_OK				0x00
#define SS_NOTALLOCATED	0xf0
#define SS_NOTAVAILABLE	0xf1
#define SS_BADVERSION		0xf2
#define SS_BUSY			0xf3

/*--------------------------------------------------------------------------*/
/* Macros Definitions                                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/

/* Link Layer */
typedef struct {
	unsigned char status;
	} error_hw;

typedef struct {
	unsigned char *pdata;
	unsigned short len;
	} data_hw;

typedef struct {
	unsigned char status;
	unsigned short size;
	} insert_hw;

/* Transport Layer */

typedef struct {
	unsigned char *pbuff;      /* data_field of tpdu */
	unsigned long len;        /* len is the length_field - 1 (without the byte reserved for tcid) */
	} data_tc;

typedef struct {
	unsigned char new_t_c_id;
	} new_tc;

typedef struct {
	unsigned char error_code;
	} error_tc;

typedef struct {
	unsigned char SB_value;
	} sb_tc;

typedef struct {
	unsigned char tcid;
	unsigned char ress_id[SSLENRESS];
	unsigned char status;
	} open_ss;

typedef struct {
	unsigned char ress_id[SSLENRESS];
	unsigned char status;
	unsigned char routing;
	} openr_ss;

typedef struct {
	unsigned char ress_id[SSLENRESS];
	} create_ss;

typedef struct {
	unsigned char ress_id[SSLENRESS];
	unsigned char status;
	unsigned short ssnb;	   /* second ssnb */
	} creatr_ss;

typedef struct {
	unsigned char tcid;
	} tcid_ss;

typedef struct {
	unsigned char *pbuff;      /* data_field of session_number (APDU) */
	unsigned long len;        /* len of the APDU 					  */
	unsigned short ssnb;	   /* sender ssnb	  					  */
	} data_ss;

typedef struct {
	unsigned short ssnb;
	} close_ss;

typedef struct {
	unsigned short ssnb;
	unsigned char status;
	} closer_ss;


/* DVB parameters */
typedef union {
    error_hw  errorhw;      /* link layer  */
    insert_hw inserthw;
    data_hw   datahw;
	data_tc	  datatc;		/* transport layer */
	new_tc    newtc;
	error_tc  errortc;
	sb_tc	  sbtc;
	open_ss	  openss;		/* session layer   */
	openr_ss  openrss;
	create_ss create;
	creatr_ss creatr;
	tcid_ss	  tcidss;
	data_ss   datass;
	close_ss  closess;
	closer_ss closers;
	} tc_infos;

/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/

#endif	// !__DVB_COD_H
