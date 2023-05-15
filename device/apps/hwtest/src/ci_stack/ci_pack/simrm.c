/****************************************************************************/
/*                                                                          */
/*   FILE:    simrm.c														*/
/*   DATE:    13/06/97                                                      */
/*   PURPOSE: Simultation of DVB-CI RM layer								*/
/*			- Initialization Module				    					    */
/*			- Main Module					    							*/
/*		    - Ending Module												    */
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version	Date	  	    Written by	     Notes	    	   			*/
/*	 1.0	    16/07/1997      SCM              Initial implementation 	*/
/*	 1.03		19/09/1997		SCM         	 ssnb management modif		*/
/*   1.05       28/12/1998      SCM              In function SimulRM_Init() */
/*              changed "if (ITC_Init(RM) < 0)" to "if (ITC_Init(RM) <= 0)" */
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/

//#include "string.h"
#include "session.h"
#include "itc.h"			/* Inter-Task Communication */

#define st_msg 		st_infos.message.body	/* access to received message 	*/
#define st_header 	st_infos.message.header	/* access to received message 	*/

#define	LPSTR		char*

int  SimulRM_Init(void);
void SimulRM_End(void);
void SimulRM_State(void);
int  SimulRM_Send_Msg(unsigned short ssnb, unsigned short len, unsigned char *buff);
int  SimulRM_Delete(unsigned char tcid);
int  SimulRM_OpenRsp(unsigned char tcid, unsigned short ssnb, unsigned char status);
int  SimulRM_Close(unsigned short ssnb);

void SimulRM_Dvb(unsigned short ssnb, int rang);
void SimulRM_Profile(unsigned short ssnb);
void SimulRM_Appli(unsigned short ssnb);
void SimulRM_CA(unsigned short ssnb);

static void response(unsigned short ssnb, unsigned char *req, unsigned short len);

typedef struct {
	unsigned short profile;
	unsigned short appli;
	unsigned short ca;
} stresource;

/*--------------------------------------------------------------------------*/
/* Global Variables		                                            		*/
/*--------------------------------------------------------------------------*/
unsigned char GL_TabTcid[MAXTCID];
int GL_NbTcid;
unsigned short GL_TabSsnb[MAXSSNB];
int GL_NbSsnb;

/*--------------------------------------------------------------------------*/
/* Local Variables		                                            		*/
/*--------------------------------------------------------------------------*/
static xmsg st_infos;				/* received informations	 			*/
static unsigned char t_evt;	/* current event 								*/
//static HWND hWnd;
static stresource TabRess[MAXTCID+1];	/* ssnb table						*/

/*--------------------------------------------------------------------------*/
/* External Variables		                                            	*/
/*--------------------------------------------------------------------------*/
//extern void GL_Text(LPSTR buff);
//extern int GL_Mode;
//extern unsigned char GL_SimulRMhost[];

/* ======================================================================== */
/* NAME : SimulRM							    							*/
/* 									    									*/
/* DESCRIPTION : entry point into RM task :				    				*/
/*		 - Initialization Module				   							*/
/*		 - Main Module						    							*/
/*		 - Ending Module				         							*/
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
void simulRM(void *lparameters)		// modified by shinhs
{
unsigned char trans;	/* sender/receiver of received event */

	/* Call the Initialization Module */
	if (! SimulRM_Init())
	{
		PrintErr(("simulRM : SimulRM_Init error !!!\n"));
		return/* -1*/;		/* error while Initialization */
	}

	/* Main Module */
	/* =========== */
	while(1)		/* infinite loop */
	{
		/* Blocking Wait for event */
#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
		if (! ITC_WaitEvt(ITC_INFINITE, &t_evt, &trans, &st_infos, CI_TASK_ID_RH))
#else
		if (! ITC_WaitEvt(ITC_INFINITE, &t_evt, &trans, &st_infos))
#endif
		{
			PrintDbg(("SIMRM::ITC_WaitEvt . Error in return code\n"));

			/* Ending Module */
			SimulRM_End();
			return/* -1*/;
		}

		/* Reading the event (MESSAGE or TIMER) */
		if ((t_evt == MESSAGE) || (t_evt == TIMER))
		{
			/* Call the RM State Diagram */
			SimulRM_State();
		}
		else
		{
			PrintDbg(("SIMRM::ITC_WaitEvt . Error in received event\n"));

			/* Ending Module */
			SimulRM_End();
			return /*-1*/;
		}
	}

	/* Call the Ending Module : should never get executed */
	SimulRM_End();
	return /*0*/;
}
/* End of Main Module */
/* ================== */


/* ======================================================================== */
/* NAME : SimulRM_Init							    						*/
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
int SimulRM_Init(void)
{
	int i;

	t_evt = 0;

	for (i=0; i <= MAXTCID; i++)
	{
		TabRess[i].profile = 0;
		TabRess[i].appli = 0;
		TabRess[i].ca = 0;
	}

	if (ITC_Init(RM) <= 0)
		return 0;

	return 1;
}
/* End of Initialization Module */
/* ============================ */


/* ======================================================================== */
/* NAME : SimulRM_End								    					*/
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
void SimulRM_End(void)
{
	ITC_End();

	return;
}
/* End of Initialization Module */
/* ============================ */


/* ======================================================================== */
/* NAME : SimulRM_State														*/
/* 									    									*/
/* DESCRIPTION : display the received messages								*/
/* 									   										*/
/* INPUT PARAMETERS :							   							*/
/*									   										*/
/* OUTPUT PARAMETERS :							    						*/
/*									  										*/
/* RETURN CODE :							  							    */
/*									 									    */
/* ======================================================================== */
void SimulRM_State(void)
{
static char buff[1000];
static unsigned char buff1[400];
char rec[10];
int i;
int j;
unsigned short len;
unsigned short ssnb;
char apdu;
char appli;
char profile;
char ca;
char sess;
char rout;
char nr[5];

	nr[0] = 0;
	nr[1] = 0x55;
//	nr[1] = 0x40;		// MMI simulation on a remote module
	nr[2] = 0;
	nr[3] = 0x41;

	sess = profile = ca = appli = apdu = 0;

	if (t_evt == MESSAGE)
		{
		switch (st_header.type) {
		case RM_TC_CREATED :
				sprintf(buff, "RM_TC_CREATED(tcid=%d) received",st_msg.screat.tcid);
				GL_TabTcid[GL_NbTcid++] = st_msg.screat.tcid;
				break;

		case RM_TC_DELETED :
				sprintf(buff, "RM_TC_DELETED(tcid=%d) received",st_msg.sdeleted.tcid);
				for (i=0; i < GL_NbTcid; i++)
				{
					if (GL_TabTcid[i] == st_msg.sdeleted.tcid)
					{
					for (j=i; j < GL_NbTcid-1; j++)
						GL_TabTcid[j] = GL_TabTcid[j+1];
					}
				}
				TabRess[st_msg.sdeleted.tcid].profile = 0;
				TabRess[st_msg.sdeleted.tcid].appli = 0;
				TabRess[st_msg.sdeleted.tcid].ca = 0;
				GL_NbTcid--;
				break;

		case RM_OPEN_SS_REQ :
				sprintf(buff, "RM_OPEN_SS_REQ received tcid=%d ssnb=%d resource=0x%02x%02x%02x%02x",st_msg.open.tcid, st_msg.open.ssnb, st_msg.open.ress_id[0],
					st_msg.open.ress_id[1], st_msg.open.ress_id[2], st_msg.open.ress_id[3]);
				GL_TabSsnb[GL_NbSsnb] = st_msg.open.ssnb;
				if (!VK_memcmp(st_msg.open.ress_id, nr, 4))
				{
					if (st_msg.open.tcid == 1)
						rout = 2;
					else
						rout = 1;
				}
				else
					rout = 0;
				sess = 1;
				break;

		case RM_SS_OPENED :
				sprintf(buff, "RM_SS_OPENED received ssnb=%d tcid=%d", st_msg.opened.ssnb, st_msg.opened.tcid);
				GL_NbSsnb++;
/*
				if (GL_Mode)
				{
					if (TabRess[st_msg.opened.tcid].profile == 0)
					{
						TabRess[st_msg.opened.tcid].profile = st_msg.opened.ssnb;
						profile = 1;
					}
					else
					if (TabRess[st_msg.opened.tcid].appli == 0)
					{
						TabRess[st_msg.opened.tcid].appli = st_msg.opened.ssnb;
						appli = 1;
					}
					else
					if (TabRess[st_msg.opened.tcid].ca == 0)
					{
						TabRess[st_msg.opened.tcid].ca = st_msg.opened.ssnb;
						ca = 1;
					}
#ifdef OLD
					if (st_msg.opened.ssnb == 1)
						profile = 1;
					if (st_msg.opened.ssnb == 3)
						appli = 1;
					if (st_msg.opened.ssnb == 5)
						ca = 1;
#ifndef PCMODE
					if (st_msg.opened.ssnb == 7)
						profile = 1;
					if (st_msg.opened.ssnb == 9)
						appli = 1;
					if (st_msg.opened.ssnb == 11)
						ca = 1;
#endif
#endif
				}
*/
				break;

		case APDU :
				VK_memcpy(buff1, st_msg.apdu.papdu, st_msg.apdu.len);
				buff1[st_msg.apdu.len] = '\0';
				sprintf(buff, "APDU(ssnb=%d) received (%d bytes) : ",st_msg.apdu.ssnb, st_msg.apdu.len);
				for (i=0; i < st_msg.apdu.len; i++)
				{
					sprintf(rec, "%x ", buff1[i]);
					VK_strcat(buff, rec);
				}

				len = st_msg.apdu.len;
				ssnb= st_msg.apdu.ssnb;
				apdu = 1;

				ITC_DynFree(st_msg.apdu.papdu);
				break;

		case RM_SS_CLOSED :
				sprintf(buff, "RM_SS_CLOSED received ssnb=%d status=%d", st_msg.closed.ssnb, st_msg.closed.status);
				for (i=0; i < GL_NbSsnb; i++)
				{
					if (GL_TabSsnb[i] == st_msg.closed.ssnb)
					{
					for (j=i; j < GL_NbSsnb; j++)
						GL_TabSsnb[j] = GL_TabSsnb[j+1];
					}
				}
				GL_NbSsnb--;
				for (i=0; i < MAXTCID; i++)
				{
					if (TabRess[i].profile == st_msg.closed.ssnb)
					{
						TabRess[i].profile = 0;
						break;
					}
					if (TabRess[i].appli == st_msg.closed.ssnb)
					{
						TabRess[i].appli = 0;
						break;
					}
					if (TabRess[i].ca == st_msg.closed.ssnb)
					{
						TabRess[i].ca = 0;
						break;
					}
				}
				break;

			default :			/* unknown message */
				PrintDbg(("SimulRM_State : Unknown message is ignored\n"));
				VK_strcpy(buff, "SimulRM_State : Unknown message is ignored\n");
				break;
			}
//		GL_Text("(R)\t");
//		GL_Text(buff);
//		GL_Text("\r\n");
		}
/*
	if (GL_Mode && sess)
		// Routing / No routing, status = OK
		SimulRM_OpenRsp(rout, st_msg.open.ssnb, 0);
*/
	if (sess)
		// Routing / No routing, status = OK
		SimulRM_OpenRsp(rout, st_msg.open.ssnb, 0);

	if (apdu)
		// Response from host to module
		response(ssnb, buff1, len);

	if (profile)
		// Profile_Enquiry
		SimulRM_Profile(st_msg.opened.ssnb);

	if (appli)
		// Appli_Info
		SimulRM_Appli(st_msg.opened.ssnb);

	if (ca)
		// CA support
		SimulRM_CA(st_msg.opened.ssnb);

	return;
}
/* End of RM State Diagram */
/* ======================= */



/* ======================================================================== */
/* NAME : SimulRM_Send_Msg					 								*/
/* 									    									*/
/* DESCRIPTION : Send a APDU message to SESS Task							*/
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
int SimulRM_Send_Msg(unsigned short ssnb, unsigned short len, unsigned char *buff)
{
char buff1[1000];
char rec[10];
int i;

	st_msg.apdu.papdu = ITC_DynAlloc(len);
	if (st_msg.apdu.papdu == NULL)
		return 0;

	st_header.trans		= (RM << 4) | SESS;
	st_header.type     	= APDU;
	st_header.len      	= sizeof(sr_apdu);
	st_msg.apdu.ssnb    = ssnb;
	st_msg.apdu.len  	= len;
	VK_memcpy(st_msg.apdu.papdu, buff, len);

	sprintf(buff1, "(S)\tAPDU(ssnb=%d, %d bytes) : ", ssnb, len);
	for (i=0; i < len; i++)
	{
		sprintf(rec, "%x ", *(buff+i));
		VK_strcat(buff1, rec);
	}
//	GL_Text(buff1);
//	GL_Text("\r\n");

	return ITC_MsgSend(st_header.trans, (unsigned char *)&st_infos);
}


/* ======================================================================== */
/* NAME : SimulRM_Delete						 							*/
/* 									    									*/
/* DESCRIPTION : Send a RM_DELETE_TC message to SESS Task					*/
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
int SimulRM_Delete(unsigned char tcid)
{
char buff[100];

	st_header.trans	= (RM << 4) | SESS;
	st_header.type  = RM_DELETE_TC;
	st_header.len   = sizeof(sr_delete);
	st_msg.sdel.tcid = tcid;

	sprintf(buff, "(S)\tRM_DELETE_TC(%d)\r\n", tcid);
//	GL_Text(buff);

	return ITC_MsgSend(st_header.trans, (unsigned char *)&st_infos);
}


/* ======================================================================== */
/* NAME : SimulRM_OpenRsp						 							*/
/* 									    									*/
/* DESCRIPTION : Send a RM_OPEN_SS_RSP message to SESS Task					*/
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
int SimulRM_OpenRsp(unsigned char tcid, unsigned short ssnb, unsigned char status)
{
char buff[100];

	st_header.trans	= (RM << 4) | SESS;
	st_header.type  = RM_OPEN_SS_RSP;
	st_header.len   = sizeof(sr_openr);
	st_msg.openrsp.rtcid  = tcid;
	st_msg.openrsp.ssnb   = ssnb;
	if (!status)
		st_msg.openrsp.status = SS_OK;
	else
		st_msg.openrsp.status = SS_NOTAVAILABLE;

	sprintf(buff, "(S)\tRM_OPEN_SS_RSP(%d, %d, %d)\r\n", ssnb, status, tcid);
//	GL_Text(buff);

	return ITC_MsgSend(st_header.trans, (unsigned char *)&st_infos);
}


/* ======================================================================== */
/* NAME : SimulRM_Close							 							*/
/* 									    									*/
/* DESCRIPTION : Send a RM_CLOSE_SS message to SESS Task					*/
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
int SimulRM_Close(unsigned short ssnb)
{
char buff[100];

	st_header.trans	= (RM << 4) | SESS;
	st_header.type  = RM_CLOSE_SS;
	st_header.len   = sizeof(sr_close);
	st_msg.close.ssnb = ssnb;

	sprintf(buff, "(S)\tRM_CLOSE_SS(%d)\r\n", ssnb);
//	GL_Text(buff);

	return ITC_MsgSend(st_header.trans, (unsigned char *)&st_infos);
}


void SimulRM_Dvb(unsigned short ssnb, int rang)
{
unsigned char BufResp[200];
unsigned short lgBuf;
unsigned char res[100];
char req_searched[10];		// request to send
int i;
unsigned char *pt;

	// Fetch the string APDU from file
	sprintf(req_searched, "apdu%d", rang);
//	GetPrivateProfileString ("APDU", req_searched, "Absent",(char *)res,100, GL_SimulRMhost);

	if (VK_strcmp((char *)res,"Absent") != 0)
	{
		i = 0;
//		pt = (unsigned char*) strtok((char *)res, " ");
		while (pt != NULL)
		{
			sscanf(pt, "%x", BufResp+i);
			i++;
//			pt = (unsigned char*) strtok(NULL, " ");
		}
		lgBuf = i;			// Length of the string
	}
	else
		return;		// No string found

	// Finally send the APDU
	SimulRM_Send_Msg(ssnb, lgBuf, BufResp);
	return;
}


void response(unsigned short ssnb, unsigned char *req, unsigned short len)
{
int no_resp;				// number associated to the request/response
unsigned char res[1000];
char req_searched[10];			// sent request
char resp_searched[10];
int i;					// index used to parse the array
unsigned char buf_tpdu[1000];
unsigned char BufResp[200];
unsigned char *pt;
unsigned short lgBuf;

	// Translation of the received buffer: a space is inserted between bytes
	for (i=0;i<(int)len;i++)
		sprintf((char *)(buf_tpdu+(3*i)),"%02x ",*(req+i));			// one byte gives three bytes
	buf_tpdu[(3*len)-1] = '\0';

	// Search this tpdu string in the file containing requests/answers
	no_resp=1;
	sprintf(req_searched, "req%d",no_resp);
//	GetPrivateProfileString ("Request", req_searched, "Absent",(char *)res,1000, GL_SimulRMhost);

	while ((VK_strcmp((const char *)res,"Absent") != 0) && (strcmp((const char *)res,(const char *)buf_tpdu) != 0))
	{
		no_resp ++;
		sprintf(req_searched, "req%d",no_resp);
//		GetPrivateProfileString ("Request", req_searched, "Absent",(char *)res,1000, GL_SimulRMhost);
	}
	if (VK_strcmp((char *)res,"Absent") != 0)
	{
		// Search the matching answer (of same number)
		sprintf(resp_searched, "resp%d",no_resp);
//		GetPrivateProfileString ("Response", resp_searched, "Absent",(char *)res,1000, GL_SimulRMhost);
	}
	else
		return;	// Request not found in the file
	if (VK_strcmp((char *)res,"Absent") != 0)
	{
		i = 0;
//		pt = (unsigned char*) strtok((char *)res, " ");
		while (pt != NULL)
		{
			sscanf(pt, "%x", BufResp+i);
			i++;
//			pt = (unsigned char*)strtok(NULL, " ");
		}
		lgBuf = i;			// keep the length of the answer
	}
	else
		return;		// response not found

	// Send the answer
	SimulRM_Send_Msg(ssnb, lgBuf, BufResp);
	return;
}



void SimulRM_Profile(unsigned short ssnb)
{
unsigned char BufResp[20];
unsigned short lgBuf;

	BufResp[0] = 0x9f;
	BufResp[1] = 0x80;
	BufResp[2] = 0x10;
	BufResp[3] = 0x0;

	lgBuf = 4;

	// Send the Profile_enq
	SimulRM_Send_Msg(ssnb, lgBuf, BufResp);
	return;
}


void SimulRM_Appli(unsigned short ssnb)
{
unsigned char BufResp[20];
unsigned short lgBuf;

	BufResp[0] = 0x9f;
	BufResp[1] = 0x80;
	BufResp[2] = 0x20;
	BufResp[3] = 0x0;

	lgBuf = 4;

	// Sending the Appli_Information
	SimulRM_Send_Msg(ssnb, lgBuf, BufResp);
	return;
}

void SimulRM_CA(unsigned short ssnb)
{
unsigned char BufResp[20];
unsigned short lgBuf;

	BufResp[0] = 0x9f;
	BufResp[1] = 0x80;
	BufResp[2] = 0x30;
	BufResp[3] = 0x0;

	lgBuf = 4;

	// Sending the CA_Info_Enquiry
	SimulRM_Send_Msg(ssnb, lgBuf, BufResp);
	return;
}


