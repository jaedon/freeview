/****************************************************************************/
/*                                                                          */
/*   FILE:    itc.h		                                                    */
/*   DATE:    25/06/97                                                      */
/*   PURPOSE: Header File Inter-Task Communication							*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version    Date	  	    Written by	     Notes	    	    		*/
/*	 1.0	    25/06/1997      SCM              Initial implementation 	*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/*									    									*/
/****************************************************************************/
#ifndef __ITC_H
#define __ITC_H

//#define __ITC_USE_REALLOC
/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/
#define MESSAGE	1		/* received message 							*/
#define TIMER		2       /* Timeout										*/

/*--------------------------------------------------------------------------*/
/* Symbol Definitions                                                       */
/*--------------------------------------------------------------------------*/
/* polling on PHYSICAL Driver period in ms -> see TM_POLLING also */
/* 수정 이력 :
   modified by shinhs : 10 -> 50
   modified by thyou : 50 -> 100
   kimjh2, hmm, if 100 msec transport layer timeout occurs!! : 100 -> 50
   hmkim, 2009-06-17 : TM_POLLING 400 -> 100, ITC_TIMEOUT 50 -> 20 으로 변경 (부팅에서 디스크램블링까지 2초 단축) 했다가 Douglas 의 경우 CI+ 캠 초기화시 SAC 에러 내지는 full authentication 시 에러 발생하여 원복함.
   hmkim, 2010-01-28 : 브로드컴의 경우에도 더글라스와 마찬가지... 20 ms 적용하면 Canal Ready 용 Neotion CI+ 캠의 경우 NFS 모드시 캠 삽입한 상태에서 init notify 가 안올라옴. */
#if 1
#define ITC_TIMEOUT	50
#else
#define ITC_TIMEOUT	20	/* org : 50 */
#endif

/* blocking ITC_WaitEvt call */
#define ITC_INFINITE	0	/* modified by shinhs -1 --> 0 */

/*--------------------------------------------------------------------------*/
/* Structures Definitions                                                   */
/*--------------------------------------------------------------------------*/

#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
typedef enum
{
	CI_TASK_ID_PHYSICAL,
	CI_TASK_ID_LINK,
	CI_TASK_ID_TRANSPORT,
	CI_TASK_ID_SESSION,
	CI_TASK_ID_RH
}CI_TASK_ID;
#endif

/*--------------------------------------------------------------------------*/
/* Function Prototypes                                                      */
/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Initialize the ITC Module */
int ITC_Init(unsigned char task);

/* End of the ITC Module */
int ITC_End(void);

#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
int ITC_WaitEvt(unsigned short timeOut, unsigned char * evt, unsigned char * trans, xmsg *infos, CI_TASK_ID eTaskID);
#else
/* Wait for a message, a timeout or a signal from the OS 					*/
int ITC_WaitEvt(unsigned short timeout, unsigned char *evt, unsigned char *sender, xmsg *infos);
#endif

/* Send a message to another Task											*/
int ITC_MsgSend(unsigned char trans, unsigned char *msg);

#ifdef CI_VK_TASK_GETCURRENTID_WORKAROUND // 20070305 thyou : VK_TASK_GetCurrentId가 정상동작 하지 않아 아래와 같이 임시로 코드를 작성합.
int ITC_SetTimer(unsigned short ident, unsigned short value, CI_TASK_ID eCITaskID);
#else
/* Set a Timer */
int ITC_SetTimer(unsigned short ident, unsigned short value);
#endif

/* Destroy a Timer */
int ITC_KillTimer(int timerid);

#ifdef FASTALLOC
/* Reserve fixed size segment */
unsigned char *ITC_Alloc(void);
#endif

/* Reserve variable size memory */
unsigned char *ITC_DynAlloc(unsigned long size);

/* Increase the allocated memory size */
unsigned char *ITC_DynReAlloc(unsigned char *pt, unsigned long size);

#ifdef FASTALLOC
/* Free a segment */
int ITC_Free(unsigned char *pt);
#endif

/* Free allocated memory */
int ITC_DynFree(unsigned char *pt);

/* Wait a delay in milliseconds */
void ITC_Sleep(int delay);

/************************************************************
*	add API to customize ITC
*************************************************************/
/* Global value initialize the ITC Module */
void ItcKernelInit(void);

#ifdef __cplusplus
}
#endif

/*--------------------------------------------------------------------------*/
/* External Variables		                                            	*/
/*--------------------------------------------------------------------------*/

#endif	// !__ITC_H
