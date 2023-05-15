/* $Header:   //BASE/archives/STING_T/include/vkernel.h-arc   1.2   Sep 02 2005 17:09:52   djkim  $ */
/********************************************************************
 * $Workfile:   vkernel.h  $
 * Project    : EMMA2
 * Author     :	Jin Won Kang
 * Description: virtual kernel interface
 *
 *                              Copyright (c) 2002 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/
#ifndef _VKERNEL_H_
#define _VKERNEL_H_

#include <htype.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if	defined(CONFIG_DEBUG)
//#define VKERNEL_DEBUG
#endif

	 /*************************************************************
		 Macro definition
	 *************************************************************/
#define VK_ERR_CODE		unsigned int

#define VK_OK			0
#define VK_ERROR		1
#define VK_TIMEOUT		2
#define VK_INVALID_ID		3
#define VK_ALLOC_ERROR		4
#define VK_NO_MSG			5
#define VK_CREATE_ERROR		10
#define VK_DESTROY_ERROR	11
#define VK_LOCK_ERROR		12
#define VK_UNLOCK_ERROR		13
#define VK_TIME_INFINITE    14
#define VK_INVALID_CLOCK	15
#define VK_MSG_FULL			17

#define VK_TIMERID_NULL		0

	 // Add quinny
#define	VK_TASK_STATE_ACTIVE			0
#define	VK_TASK_STATE_SUSPENDING		1
#define	VK_TASK_STATE_SUSPENDED			2
#define   VK_TASK_PRIORITY_UNKNOWN	0xffff
#define VK_TASK_JOINABLE			0x9ed8e122

#define VK_TASK_PRIORITY_MW_REF			40
#define VK_TASK_PRIORITY_DI_REF			60

 /*************************************************************
	 Type definition
 *************************************************************/
 typedef enum
 {
	 VK_SUSPEND_FIFO=1, // To make it different from NULL
	 VK_SUSPEND_PRIORITY,
	 VK_SUSPEND_UNKNOWN
 } VK_SUSPEND_TYPE;

 typedef enum
 {
	 VK_TIMER_TYPE_ONCE = 0,
	 VK_TIMER_TYPE_EVERY
 } VK_TIMER_TYPE ;

 typedef enum
 {
	 VK_TIMER_DISABLE=0,
	 VK_TIMER_ENABLE
 } VK_TIMER_CONTROL;

 typedef enum
 {
	 VK_MEM_ALLOC_FIRST_MATCH,
	 VK_MEM_ALLOC_BEST_MATCH
 } VK_MEM_ALLOC_METHOD;


typedef enum
{
	VK_SUSPENDTYPE_FIFO,
	VK_SUSPENDTYPE_PRIORITY,
	VK_SUSPENDTYPE_MAX
}VK_SUSPENDTYPE;

typedef enum
{
	VK_CLOCK_DST_AUTO,
	VK_CLOCK_DST_ON_ALWAYS,
	VK_CLOCK_DST_OFF
}VK_CLOCK_DST_MODE;

typedef enum
{
	VK_CLOCK_SETBY_NONE = 0,

	VK_CLOCK_SETBY_SI,
	VK_CLOCK_SETBY_USERINPUT,
	VK_CLOCK_SETBY_MICOM

}VK_CLOCK_SET_MODE;

typedef enum
{
	VK_INTR_TASK0 = 0,	// for general purpose like hotplug event which needs slow action.
	VK_INTR_TASK1,		// for general purpose
	VK_INTR_TASK2,      // for vkernel socket
	VK_INTR_TASK_MAX
}VK_INTR_TASK_E;

typedef struct
{
	unsigned long ulMemTotal;
	unsigned long ulMemFree;
	unsigned long ulCommitted_AS;
} VK_MEM_INFO_t;

typedef struct
{
	int				sig_signo;
	int				sig_code;
	int				sig_errno;
	unsigned long	sig_pid;
	unsigned long	sig_uid;
	void			*sig_addr;
	int				sig_status;
	int				sig_band;
} VK_SIG_INFO_t;

// TODO: enable by build configuration is arranged
#if	defined(VKERNEL_DEBUG)
 typedef enum MEMDBG_GROUP_Enum
 {
	 MEMDBG_GROUP_AP,
	 MEMDBG_GROUP_API,
	 MEMDBG_GROUP_MD,
	 MEMDBG_GROUP_MW,
	 MEMDBG_GROUP_PL,
	 MEMDBG_GROUP_SI,
	 MEMDBG_GROUP_CA,
	 MEMDBG_GROUP_EX,
	 MEMDBG_GROUP_DB,
	 MEMDBG_GROUP_DD,
	 MEMDBG_GROUP_FS,
	 MEMDBG_GROUP_ZZ,
	 MEMDBG_GROUP_DSMCC,
	 MEMDBG_GROUP_EPG,

	 MAX_MEMDBG_GROUP
 } MEMDBG_GROUP_E;

#define AP_MEMORY_DEBUG
#define API_MEMORY_DEBUG
#define	MD_MEMORY_DEBUG
#define MW_MEMORY_DEBUG
#define PL_MEMORY_DEBUG
#define SI_MEMORY_DEBUG
#define CA_MEMORY_DEBUG
#define DD_MEMORY_DEBUG
#define EX_MEMORY_DEBUG
#define DB_MEMORY_DEBUG
#define FS_MEMORY_DEBUG
#define ZZ_MEMORY_DEBUG
#define DSMCC_MEMORY_DEBUG
#define EPG_MEMORY_DEBUG

#if (defined(MD_MEMORY_DEBUG) || defined(AP_MEMORY_DEBUG) || defined(MW_MEMORY_DEBUG) || defined(PL_MEMORY_DEBUG) || defined(SI_MEMORY_DEBUG) || defined(CA_MEMORY_DEBUG) || defined(DD_MEMORY_DEBUG) || defined(DL_MEMORY_DEBUG) || defined(EX_MEMORY_DEBUG) || defined(DB_MEMORY_DEBUG) || defined(FS_MEMORY_DEBUG) || defined(ZZ_MEMORY_DEBUG))
#define HUMAX_MEMORY_DEBUG
#define MEMORY_DEBUG
#endif

//#define VKTIMER_DEBUG
//#define VK_MSG_DEBUG
//#define VK_SEM_DEBUG
//#define VK_TASK_DEBUG
//#define VK_CALLBACK_DEBUG_

void VK_Panic( void );
#endif


/*************************************************************
 Global function prototype
*************************************************************/
int VK_Init(void);

void	*VK_MEM_Alloc(unsigned long size);
int		VK_MEM_Free(void *ap);
void	*VK_MEM_Calloc(unsigned long size);
void	*VK_MEM_Realloc(void *p, unsigned long size);

#if defined(HUMAX_MEMORY_DEBUG)
	 int	VK_DBGMEM_Init(void);
	 void	*VK_DBGMEM_Alloc(unsigned long size, char* file, int line, unsigned short group);
	 void	*VK_DBGMEM_Calloc(unsigned long size, char* file, int line, unsigned short group);
	 void	*VK_DBGMEM_Realloc(void *p, unsigned long size, char* file, int line, unsigned short group);
	 int	VK_DBGMEM_Free(void *p, char* file, int line, unsigned short group);


	 int VK_DBGMEM_GroupInit(unsigned short group);
	 int VK_DBGMEM_EnableTrace(unsigned short group);
	 int VK_DBGMEM_DisableTrace(unsigned short group);
	 int VK_DBGMEM_Group_PrintList(unsigned short group);
	 int VK_DBGMEM_Group_DisplayUsage(unsigned short group);
	 int VK_DBGMEM_Group_DisplayUsageVK(void);

	 int VK_DBGMEM_CreateAllocPointerPool(unsigned short group, unsigned short nPoolSize);

	 // debug memory pointer pool size
	#define AP_DBGMEM_ALLOC_POOL_SIZE		10000
	#define API_DBGMEM_ALLOC_POOL_SIZE		10000
	#define	MD_DBGMEM_ALLOC_POOL_SIZE		10000
	#define MW_DBGMEM_ALLOC_POOL_SIZE		40000
	#define PL_DBGMEM_ALLOC_POOL_SIZE		12000
	#define SI_DBGMEM_ALLOC_POOL_SIZE		11000
	#define CA_DBGMEM_ALLOC_POOL_SIZE		4000
	#define DD_DBGMEM_ALLOC_POOL_SIZE		12000
	#define EX_DBGMEM_ALLOC_POOL_SIZE		10000
	#define DB_DBGMEM_ALLOC_POOL_SIZE		10000
	#define FS_DBGMEM_ALLOC_POOL_SIZE		10000
	#define ZZ_DBGMEM_ALLOC_POOL_SIZE		10000
	#define DSMCC_DBGMEM_ALLOC_POOL_SIZE	10000
	#define EPG_DBGMEM_ALLOC_POOL_SIZE		10000

#define AP_MEM_Init()			VK_DBGMEM_GroupInit(MEMDBG_GROUP_AP)
#define AP_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_AP)
#define AP_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_AP)
#define AP_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_AP)
#define AP_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_AP)

#define API_MEM_Init()			VK_DBGMEM_GroupInit(MEMDBG_GROUP_API)
#define API_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_API)
#define API_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_API)
#define API_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_API)
#define API_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_API)

#define MD_MEM_Init()			VK_DBGMEM_GroupInit(MEMDBG_GROUP_MD)
#define MD_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_MD)
#define MD_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_MD)
#define MD_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_MD)
#define MD_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_MD)

#define MW_MEM_Init()			VK_DBGMEM_GroupInit(MEMDBG_GROUP_MW)
#define MW_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_MW)
#define MW_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_MW)
#define MW_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_MW)
#define MW_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_MW)

#define PL_MEM_Init()			VK_DBGMEM_GroupInit(MEMDBG_GROUP_PL)
#define PL_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_PL)
#define PL_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_PL)
#define PL_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_PL)
#define PL_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_PL)

#define SI_MEM_Init()				VK_DBGMEM_GroupInit(MEMDBG_GROUP_SI)
#define SI_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_SI)
#define SI_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_SI)
#define SI_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_SI)
#define SI_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_SI)

#define CA_MEM_Init()			VK_DBGMEM_GroupInit(MEMDBG_GROUP_CA)
#define CA_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_CA)
#define CA_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_CA)
#define CA_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_CA)
#define CA_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_CA)

#define DD_MEM_Init()			VK_DBGMEM_GroupInit(MEMDBG_GROUP_DD)
#define DD_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_DD)
#define DD_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_DD)
#define DD_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_DD)
#define DD_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_DD)

#define EX_MEM_Init()			VK_DBGMEM_GroupInit(MEMDBG_GROUP_EX)
#define EX_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_EX)
#define EX_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_EX)
#define EX_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_EX)
#define EX_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_EX)

#define DB_MEM_Init()			VK_DBGMEM_GroupInit(MEMDBG_GROUP_DB)
#define DB_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_DB)
#define DB_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_DB)
#define DB_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_DB)
#define DB_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_DB)

#define FS_MEM_Init()			VK_DBGMEM_GroupInit(MEMDBG_GROUP_FS)
#define FS_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_FS)
#define FS_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_FS)
#define FS_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_FS)
#define FS_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_FS)

#define ZZ_MEM_Init()			VK_DBGMEM_GroupInit(MEMDBG_GROUP_ZZ)
#define ZZ_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_ZZ)
#define ZZ_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_ZZ)
#define ZZ_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_ZZ)
#define ZZ_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_ZZ)

#define DSMCC_MEM_Init()			VK_DBGMEM_GroupInit(MEMDBG_GROUP_DSMCC)
#define DSMCC_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_DSMCC)
#define DSMCC_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_DSMCC)
#define DSMCC_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_DSMCC)
#define DSMCC_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_DSMCC)

#define EPG_MEM_Init()			VK_DBGMEM_GroupInit(MEMDBG_GROUP_EPG)
#define EPG_MEM_EnableTrace()	VK_DBGMEM_EnableTrace(MEMDBG_GROUP_EPG)
#define EPG_MEM_DisableTrace()	VK_DBGMEM_DisableTrace(MEMDBG_GROUP_EPG)
#define EPG_MEM_PrintList()		VK_DBGMEM_Group_PrintList(MEMDBG_GROUP_EPG)
#define EPG_MEM_DisplayUsage()	VK_DBGMEM_Group_DisplayUsage(MEMDBG_GROUP_EPG)

#ifdef AP_MEMORY_DEBUG
    #define AP_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_AP)
    #define AP_MEM_Calloc(a)		VK_DBGMEM_Calloc(a, __FILE__, __LINE__, MEMDBG_GROUP_AP)
    #define AP_MEM_Realloc(a, b)	VK_DBGMEM_Realloc(a, b, __FILE__, __LINE__, MEMDBG_GROUP_AP)
    #define AP_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_AP)
#else
    #define AP_MEM_Alloc			VK_MEM_Alloc
    #define AP_MEM_Calloc			VK_MEM_Calloc
    #define AP_MEM_Realloc			VK_MEM_Realloc
    #define AP_MEM_Free				VK_MEM_Free
#endif

#ifdef API_MEMORY_DEBUG
    #define API_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_API)
    #define API_MEM_Calloc(a)		VK_DBGMEM_Calloc(a, __FILE__, __LINE__, MEMDBG_GROUP_API)
    #define API_MEM_Realloc(a, b)	VK_DBGMEM_Realloc(a, b, __FILE__, __LINE__, MEMDBG_GROUP_API)
    #define API_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_API)
#else
    #define API_MEM_Alloc			VK_MEM_Alloc
    #define API_MEM_Calloc			VK_MEM_Calloc
    #define API_MEM_Realloc			VK_MEM_Realloc
    #define API_MEM_Free				VK_MEM_Free
#endif

#ifdef MD_MEMORY_DEBUG
    #define MD_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_MD)
    #define MD_MEM_Calloc(a)		VK_DBGMEM_Calloc(a, __FILE__, __LINE__, MEMDBG_GROUP_MD)
    #define MD_MEM_Realloc(a, b)	VK_DBGMEM_Realloc(a, b, __FILE__, __LINE__, MEMDBG_GROUP_MD)
    #define MD_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_MD)
#else
    #define MD_MEM_Alloc			VK_MEM_Alloc
    #define MD_MEM_Calloc			VK_MEM_Calloc
    #define MD_MEM_Realloc			VK_MEM_Realloc
    #define MD_MEM_Free				VK_MEM_Free
#endif

#ifdef MW_MEMORY_DEBUG
    #define MW_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_MW)
    #define MW_MEM_Calloc(a)		VK_DBGMEM_Calloc(a, __FILE__, __LINE__, MEMDBG_GROUP_MW)
    #define MW_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_MW)
#else
    #define MW_MEM_Alloc			VK_MEM_Alloc
    #define MW_MEM_Calloc			VK_MEM_Calloc
    #define MW_MEM_Free				VK_MEM_Free
#endif

#ifdef PL_MEMORY_DEBUG
    #define PL_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_PL)
    #define PL_MEM_Realloc(a, b)	VK_DBGMEM_Realloc(a, b, __FILE__, __LINE__, MEMDBG_GROUP_PL)
    #define PL_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_PL)
#else
    #define PL_MEM_Alloc			VK_MEM_Alloc
    #define PL_MEM_Realloc			VK_MEM_Realloc
    #define PL_MEM_Free				VK_MEM_Free
#endif

#ifdef SI_MEMORY_DEBUG
    #define SI_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_SI)
    #define SI_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_SI)
#else
    #define SI_MEM_Alloc			VK_MEM_Alloc
    #define SI_MEM_Free				VK_MEM_Free
#endif

#ifdef CA_MEMORY_DEBUG
    #define CA_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_CA)
    #define CA_MEM_Calloc(a)		VK_DBGMEM_Calloc(a, __FILE__, __LINE__, MEMDBG_GROUP_CA)
    #define CA_MEM_Realloc(a, b)	VK_DBGMEM_Realloc(a, b, __FILE__, __LINE__, MEMDBG_GROUP_CA)
    #define CA_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_CA)
#else
    #define CA_MEM_Alloc			VK_MEM_Alloc
    #define CA_MEM_Calloc			VK_MEM_Calloc
    #define CA_MEM_Realloc			VK_MEM_Realloc
    #define CA_MEM_Free				VK_MEM_Free
#endif

#ifdef DD_MEMORY_DEBUG
    #define DD_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_DD)
    #define DD_MEM_Calloc(a)		VK_DBGMEM_Calloc(a, __FILE__, __LINE__, MEMDBG_GROUP_DD)
    #define DD_MEM_Realloc(a, b)	VK_DBGMEM_Realloc(a, b, __FILE__, __LINE__, MEMDBG_GROUP_DD)
    #define DD_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_DD)
#else
    #define DD_MEM_Alloc			VK_MEM_Alloc
    #define DD_MEM_Calloc			VK_MEM_Calloc
    #define DD_MEM_Realloc			VK_MEM_Realloc
    #define DD_MEM_Free				VK_MEM_Free
#endif

#ifdef EX_MEMORY_DEBUG // extension module
    #define EX_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_EX)
	#define EX_MEM_Calloc(a) 		VK_DBGMEM_Calloc(a, __FILE__, __LINE__, MEMDBG_GROUP_EX)
	#define EX_MEM_Realloc(a,b) 	VK_DBGMEM_Realloc(a, b,__FILE__, __LINE__, MEMDBG_GROUP_EX)
    #define EX_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_EX)
#else
    #define EX_MEM_Alloc			VK_MEM_Alloc	//VK_MEM_Alloc_EX
	#define EX_MEM_Calloc			VK_MEM_Calloc
	#define EX_MEM_Realloc 			VK_MEM_Realloc
    #define EX_MEM_Free				VK_MEM_Free
#endif

#ifdef DB_MEMORY_DEBUG
	#define DB_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_DB)
    #define DB_MEM_Realloc(a, b)	VK_DBGMEM_Realloc(a, b, __FILE__, __LINE__, MEMDBG_GROUP_DB)
    #define DB_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_DB)
#else
    #define DB_MEM_Alloc			VK_MEM_Alloc
    #define DB_MEM_Realloc			VK_MEM_Realloc
    #define DB_MEM_Free				VK_MEM_Free
#endif

#ifdef FS_MEMORY_DEBUG
	#define FS_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_FS)
    #define FS_MEM_Realloc(a, b)	VK_DBGMEM_Realloc(a, b, __FILE__, __LINE__, MEMDBG_GROUP_FS)
    #define FS_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_FS)
#else
    #define FS_MEM_Alloc			VK_MEM_Alloc
    #define FS_MEM_Realloc			VK_MEM_Realloc
    #define FS_MEM_Free				VK_MEM_Free
#endif

#ifdef ZZ_MEMORY_DEBUG
	#define ZZ_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_ZZ)
    #define ZZ_MEM_Calloc(a)		VK_DBGMEM_Calloc(a, __FILE__, __LINE__, MEMDBG_GROUP_ZZ)
    #define ZZ_MEM_Realloc(a, b)	VK_DBGMEM_Realloc(a, b, __FILE__, __LINE__, MEMDBG_GROUP_ZZ)
    #define ZZ_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_ZZ)
#else
    #define ZZ_MEM_Alloc			VK_MEM_Alloc
    #define ZZ_MEM_Calloc			VK_MEM_Calloc
    #define ZZ_MEM_Realloc			VK_MEM_Realloc
    #define ZZ_MEM_Free				VK_MEM_Free
#endif


#ifdef DSMCC_MEMORY_DEBUG
	#define DSMCC_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_DSMCC)
	#define DSMCC_MEM_Calloc(a)		VK_DBGMEM_Calloc(a, __FILE__, __LINE__, MEMDBG_GROUP_DSMCC)
	#define DSMCC_MEM_Realloc(a, b)	VK_DBGMEM_Realloc(a, b, __FILE__, __LINE__, MEMDBG_GROUP_DSMCC)
	#define DSMCC_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_DSMCC)
#else
	#define DSMCC_MEM_Alloc			VK_MEM_Alloc
	#define DSMCC_MEM_Calloc			VK_MEM_Calloc
	#define DSMCC_MEM_Realloc			VK_MEM_Realloc
	#define DSMCC_MEM_Free				VK_MEM_Free
#endif

#ifdef EPG_MEMORY_DEBUG
	#define EPG_MEM_Alloc(a)			VK_DBGMEM_Alloc(a, __FILE__, __LINE__, MEMDBG_GROUP_EPG)
	#define EPG_MEM_Calloc(a)		VK_DBGMEM_Calloc(a, __FILE__, __LINE__, MEMDBG_GROUP_EPG)
	#define EPG_MEM_Realloc(a, b)	VK_DBGMEM_Realloc(a, b, __FILE__, __LINE__, MEMDBG_GROUP_EPG)
	#define EPG_MEM_Free(a)			VK_DBGMEM_Free(a, __FILE__, __LINE__, MEMDBG_GROUP_EPG)
#else
	#define EPG_MEM_Alloc			VK_MEM_Alloc
	#define EPG_MEM_Calloc			VK_MEM_Calloc
	#define EPG_MEM_Realloc			VK_MEM_Realloc
	#define EPG_MEM_Free				VK_MEM_Free
#endif
#else	// HUMAX_MEMORY_DEBUG
#define AP_MEM_Alloc		VK_MEM_Alloc
#define AP_MEM_Calloc		VK_MEM_Calloc
#define AP_MEM_Realloc		VK_MEM_Realloc
#define AP_MEM_Free			VK_MEM_Free

#define MD_MEM_Alloc		VK_MEM_Alloc
#define MD_MEM_Calloc		VK_MEM_Calloc
#define MD_MEM_Realloc		VK_MEM_Realloc
#define MD_MEM_Free			VK_MEM_Free

#define MW_MEM_Alloc		VK_MEM_Alloc
#define MW_MEM_Calloc		VK_MEM_Calloc
#define MW_MEM_Free			VK_MEM_Free

#define PL_MEM_Alloc		VK_MEM_Alloc
#define PL_MEM_Realloc		VK_MEM_Realloc
#define PL_MEM_Free			VK_MEM_Free

#define SI_MEM_Alloc		VK_MEM_Alloc
#define SI_MEM_Free			VK_MEM_Free

#define CA_MEM_Alloc		VK_MEM_Alloc
#define CA_MEM_Calloc		VK_MEM_Calloc
#define CA_MEM_Realloc		VK_MEM_Realloc
#define CA_MEM_Free			VK_MEM_Free

#define DD_MEM_Alloc		VK_MEM_Alloc
#define DD_MEM_Calloc		VK_MEM_Calloc
#define DD_MEM_Realloc		VK_MEM_Realloc
#define DD_MEM_Free			VK_MEM_Free

#define EX_MEM_Alloc		VK_MEM_Alloc
#define EX_MEM_Free			VK_MEM_Free
#define EX_MEM_Calloc		VK_MEM_Calloc
#define EX_MEM_Realloc	 	VK_MEM_Realloc

#define DB_MEM_Alloc		VK_MEM_Alloc
#define DB_MEM_Realloc		VK_MEM_Realloc
#define DB_MEM_Free			VK_MEM_Free

#define FS_MEM_Alloc		VK_MEM_Alloc
#define FS_MEM_Realloc		VK_MEM_Realloc
#define FS_MEM_Free			VK_MEM_Free

#define ZZ_MEM_Alloc		VK_MEM_Alloc
#define ZZ_MEM_Realloc		VK_MEM_Realloc
#define ZZ_MEM_Free			VK_MEM_Free


#define DSMCC_MEM_Alloc		VK_MEM_Alloc
#define DSMCC_MEM_Realloc		VK_MEM_Realloc
#define DSMCC_MEM_Free			VK_MEM_Free

#define EPG_MEM_Alloc		VK_MEM_Alloc
#define EPG_MEM_Realloc		VK_MEM_Realloc
#define EPG_MEM_Free			VK_MEM_Free

#define API_MEM_Alloc			VK_MEM_Alloc
#define API_MEM_Calloc			VK_MEM_Calloc
#define API_MEM_Realloc			VK_MEM_Realloc
#define API_MEM_Free				VK_MEM_Free
#endif	// HUMAX_MEMORY_DEBUG

/*************************************************************
 TASK
*************************************************************/
int vk_TASK_Init(void);

int VK_TASK_Create(void (*start_func)(void*), unsigned long prio, unsigned long size, const char *name, void *args, unsigned long * taskId ,   int  IsJoinable);

int VK_TASK_Join(unsigned long taskId);

int VK_TASK_CreateEx(unsigned long *taskId, const char *name,	void (*start_func)(unsigned long ulParam1, void* pParam2), unsigned long argc, void *argv, void* pStack, unsigned long stack_size, unsigned long prio);
int VK_TASK_Start(unsigned long taskId);
int VK_TASK_Stop(unsigned long taskId);
int VK_TASK_Sleep(unsigned long ultime);	 /* ultime in milliseconds */
int VK_TASK_GetCurrentId(unsigned long *taskId);
int VK_TASK_GetCurrentIdEx(unsigned long *taskId, void** ppArg);
int VK_TASK_kill(unsigned long taskId, int iParam);
int VK_TASK_Destroy(unsigned long taskId);
int VK_TASK_DestroyEx(unsigned long taskId);
int VK_TASK_JoinEx(unsigned long taskId);
void VK_TASK_Lock(void);
void VK_TASK_Unlock(void);
int VK_TASK_GetCallingTaskPriority(void);
int VK_TASK_GetCurrentPriority(unsigned long taskId, unsigned long *currpriority);
int VK_TASK_SetCurrentPriority(unsigned long taskId, unsigned long newpriority);
int VK_TASK_GetState(unsigned long taskId, unsigned char *pStatus);
int VK_TASK_GetCountTasks (int *pnTaskNum);
int VK_TASK_GetAllTaskIds (int nArraySize, int *pnTaskNum, unsigned long *pulTaskIdArray);
void VK_TASK_PrintCallTrace(void);

void VK_TASK_GetTaskName(unsigned long ulTaskId, char *pszName);
int VK_TASK_GetStackInfo(unsigned long taskId, void **ppvStactStartAddr, unsigned long *pStackSize);
int VK_TASK_GetArgument(unsigned long taskId, void **ppTaskArguement);
int VK_TASK_SendPMMsg(int sig);
int VK_TASK_RegisterPM(unsigned long taskId);
#if defined(VK_TASK_DEBUG)
void VK_TASK_PrintTaskList(void);
#endif
void VK_TASK_CheckGuardArea( void );
#if defined(VK_CALLBACK_DEBUG)
void VK_TASK_EnterCallback(unsigned long addrFunc);
void VK_TASK_CheckCallbackBody(char *file, int line);
#define VK_TASK_CheckCallback() VK_TASK_CheckCallbackBody(__FILE__, __LINE__)
void VK_TASK_LeaveCallback(void);
#else
#define VK_TASK_EnterCallback(a)
#define VK_TASK_CheckCallback()
#define VK_TASK_LeaveCallback()
#endif

/*************************************************************
 MESSAGE
*************************************************************/
int vk_MSG_Init(void);
int VK_MSG_Create(unsigned long q_count,unsigned long msg_size, const char *q_name, unsigned long *q_id,VK_SUSPENDTYPE etSuspendType);
int VK_MSG_CreateEx(unsigned long q_count, unsigned long msg_size, const char *q_name, unsigned long *q_id, unsigned char suspend_type);
int VK_MSG_GetMessageCount(unsigned long ulMsgQId, unsigned long *pulMsgCount, unsigned long *pulMaxMsgCount);
int VK_MSG_Send(unsigned long q_id, const void *msg, unsigned long size);
int VK_MSG_SendTimeout(unsigned long q_id, const void *msg, unsigned long size, unsigned long timeout);
int VK_MSG_Receive(unsigned long q_id, void *msg, unsigned long size);
int VK_MSG_ReceiveTimeout(unsigned long q_id, void *msg, unsigned long size, unsigned long timeout);
int VK_MSG_ReceiveLastMessage(unsigned long q_id, void *msg, unsigned long size);
int VK_MSG_Find(unsigned long q_id, const void *msg, unsigned long size);
int VK_MSG_Destroy(unsigned long q_id);
int VK_MSG_Clear(unsigned long q_id);
int VK_MSG_SendUrgent(unsigned long q_id, const void *msg, unsigned long size);
int VK_MSG_SendUrgentTimeout(unsigned long q_id, const void *msg, unsigned long size, unsigned long timeout);
int VK_MSG_Delete(unsigned long q_id, const void* msg, unsigned long size);
int VK_MSG_GetStatus(unsigned long q_id, unsigned long *pulMsgCount, unsigned long *pulQueCount );
#if defined(VK_MSG_DEBUG)
void VK_MSG_PrintMsgQList(void);
#endif

/*************************************************************
 EVENT
*************************************************************/
int VK_EVENT_Create(unsigned long *eventId, char eventName[]);
int VK_EVENT_Send(unsigned long eventId, unsigned long event);
int VK_EVENT_Receive(unsigned long eventId, unsigned long *event);  /* timeout:wait_forever*/
int VK_EVENT_ReceiveTimeout(unsigned long	 eventId, unsigned long  *event, unsigned long	 timeout); /* timeout:0-> no_wait*/
int VK_EVENT_Destroy(unsigned long eventId);


/*************************************************************
 SEMOPHORE
*************************************************************/
#if defined(VK_SEM_DEBUG)
int VK_SEM_PrintSemList(void);
int VK_DBGSEM_Get(unsigned long sem_id, char *file, int line);
int VK_DBGSEM_Release(unsigned long semId, char *file, int line);
int VK_DBGSEM_GetTimeout(unsigned long sem_id, unsigned long timeout, char *file, int line);

#define VK_SEM_Get(semId) VK_DBGSEM_Get(semId, __FILE__, __LINE__)
#define VK_SEM_Release(semId) VK_DBGSEM_Release(semId, __FILE__, __LINE__)
#define VK_SEM_GetTimeout(semId, timeout) VK_DBGSEM_GetTimeout(semId, timeout, __FILE__, __LINE__)
#define VK_SEM_GetTimeoutEx(semId, timeout) VK_DBGSEM_GetTimeout(semId, timeout, __FILE__, __LINE__)
#define VK_SEM_ReleaseEx(semId) VK_DBGSEM_Release(semId, __FILE__, __LINE__)
#else
int VK_SEM_Get(unsigned long semId);
int VK_SEM_Release(unsigned long semId);
int VK_SEM_GetTimeout(unsigned long sem_id, unsigned long timeout );
int VK_SEM_GetTimeoutEx(unsigned long sem_id, unsigned long timeout);
int VK_SEM_ReleaseEx(unsigned long sem_id);
#endif
int VK_SEM_Create(unsigned long *sem_id,const char *name, VK_SUSPENDTYPE etSuspendtype);
int VK_SEM_Destroy(unsigned long semId);
int VK_SEM_CreateWithCount(unsigned long *sem_id, unsigned long count, const char *name, VK_SUSPENDTYPE etSuspendType);
int VK_SEM_CreateWithCountEx(unsigned long *sem_id, unsigned long count, const char *name, unsigned char suspend_type);
int VK_SEM_DestroyEx(unsigned long sem_id);

 /*************************************************************
	 TIMER
 *************************************************************/
typedef enum
{
	VK_TIMER_RUNNING,
	VK_TIMER_EXPIRED
} VK_TIMER_STATE;

int VK_TIMER_EventAfter(unsigned long duration, void (*cbfn)(unsigned long, void *), void *param, int size, unsigned long *id);
int VK_TIMER_EventEvery(unsigned long duration, void (*cbfn)(unsigned long, void *), void *param, int size, unsigned long *id);
int VK_TIMER_Cancel(unsigned long timerId);
int VK_TIMER_Check(unsigned long TimerID);
unsigned long VK_TIMER_GetTickFreq(void);
unsigned  long VK_TIMER_GetSystemTick(void);
int VK_TIMER_ModifyEx(unsigned long TimeOut, void (*CallBackFunc)(unsigned long, void *), void *param, int size, unsigned char type, unsigned long *TimerID);
int VK_TIMER_ControlEx(unsigned char control, unsigned long *TimerID);
int VK_TIMER_GetClockEx(unsigned long *ticks);
int VK_TIMER_SetClockEx(unsigned long ticks);
VK_TIMER_STATE VK_TIMER_TimeAfter(unsigned long whenTick, unsigned long nowTick);
unsigned long VK_TIMER_GetMaxSystemTick(void); /* does not fixed yet, need discuss */

#if defined(VK_TIMER_DEBUG)
void VK_TIMER_PrintTimerList(void);
#endif

/*************************************************************
	 Clock
*************************************************************/
unsigned long VK_CLOCK_GetTime(unsigned long *pulUnixTime);
unsigned long VK_CLOCK_GetTimeSource(VK_CLOCK_SET_MODE *peSetMode);
unsigned long VK_CLOCK_SetTime(VK_CLOCK_SET_MODE eSetMode, unsigned long ulUnixTime);
unsigned long VK_CLOCK_GetOffsetTime(int *pnHour, int *pnMin);
unsigned long VK_CLOCK_SetOffsetTime(int nHour, int nMin);

int VK_CLOCK_GetDstMode (VK_CLOCK_DST_MODE *peDstMode);
int VK_CLOCK_SetDstMode (VK_CLOCK_DST_MODE eDstMode);


/*************************************************************
	 mutex
*************************************************************/

int VK_MUTEX_Create(unsigned long *mutex_id, const char *name);
int VK_MUTEX_Destroy(unsigned long mutex_id);
int VK_MUTEX_Lock(unsigned long mutexId);
int VK_MUTEX_Trylock(unsigned long mutexId);
int VK_MUTEX_Unlock(unsigned long mutexId);

/*************************************************************
	 MEMORY
*************************************************************/
void *VK_MEM_Memset(void *dest, unsigned char c, unsigned long count );
void *memset32by32(void *dest, unsigned long c, unsigned long count );
void *VK_MEM_Memcpy(void *dst0, void *src0, unsigned long len0);
int VK_MEM_GetInfo(VK_MEM_INFO_t *pstMemInfo);

#define VK_memset32(a, b, c)		VK_MEM_Memset((void *)a, (unsigned char)b, (unsigned long)c)
#define VK_memset32by32(a, b, c)	memset32by32((void *)a, (unsigned long)b, (unsigned long)c)
#define VK_memcpy32(a, b, c)		VK_MEM_Memcpy((void *)a, (void *)b, (unsigned long)c)

/*
 * stdarg.h는 compiler에서 지원하는 header. 입니다.
 * 다른 stdio.h/string.h header는 절대 추가하지마세요.
 *
 * "stdarg.h" is supported by the compiler.
 * The only "stdarg.h" added.
 * Do not add stdio.h/string.h file.
 */
#include <stdarg.h> /* va_list */

#if defined(CONFIG_VK_STDLIB)
	#if defined(FILE)
	#else
		typedef HINT32	FILE;
	#endif

HINT32 		VK_vsprintf(HCHAR *outbuf,const HCHAR *templat, va_list marker);
HINT32 		VK_vsnprintf(HCHAR *outbuf, HINT32 n, const HCHAR *templat, va_list marker);
HINT32 		VK_vprintf(const HCHAR *templat, va_list marker);

#define 	VK_STDIN		((void *)0)
#define		VK_STDOUT		((void *)1)
#define 	VK_STDERR		((void *)2)
#define 	VK_malloc 		VK_MEM_Alloc
#define 	VK_free 		VK_MEM_Free

void 		*VK_memcpy(const void *dst, const void *src, HUINT32 size);
void 		*VK_memset(const void *data, HUINT8 c, HUINT32 size);
HINT32		VK_memcmp(const void *dst, const void *src, HUINT32 size);

#define 	VK_StrDup		VK_strdup
#define 	VK_Isprint		VK_isprint
#define		VK_StrToUpper	VK_strtoupper
#define		VK_StrCaseCmp 	VK_strcasecmp

HINT32		VK_fflush (void *fp);
HINT32		VK_scanf (const HCHAR *templat, ...);
HINT32		VK_sscanf (const HCHAR *buf, const HCHAR *templat, ...);
HCHAR 		*VK_strreplacechar (HCHAR *dst, const HCHAR *src, HCHAR from, HCHAR to);
HCHAR		*VK_strreplace (HCHAR *dst, const HCHAR *src, const HCHAR *from, const HCHAR *to);
HCHAR		*VK_strcasestr(const HCHAR *s1, const HCHAR *s2);
HCHAR 		*VK_strdupex (const HCHAR *szOld, HUINT32 n, void *(*_alloc)(HUINT32));
void 		*VK_memdupex(const void *data, HUINT32 size, void *(_alloc)(HUINT32));
void 		*VK_memdup(const void *data, HUINT32 size);
HUINT32		VK_atoh(const HCHAR *s);
HUINT32		VK_atoui (const HCHAR *s);
HBOOL		VK_strisnumber (const HCHAR *s);
HBOOL 		VK_ishexdigit(const HUINT8 ch);
HCHAR		*VK_strupper (HCHAR *dest, const HCHAR *src);
HCHAR		*VK_strlower (HCHAR *dest, const HCHAR *src);

HCHAR 		*VK_strcpy(HCHAR *dest, const HCHAR *src);
HCHAR 		*VK_strncpy(HCHAR *dest, const HCHAR *src, HUINT32 cnt);
HCHAR		*VK_strstr(const HCHAR *dest, const HCHAR *find);
HCHAR 		*VK_strchr(const HCHAR *dest, HINT32 c);
HCHAR 		*VK_strdup (const HCHAR *szOld);
HCHAR 		*VK_strdupex (const HCHAR *szOld, HUINT32 n, void *(*_alloc)(HUINT32));


HINT32		VK_strncasecmp (const HCHAR *s1, const HCHAR *s2, HUINT32 n);
HINT32 		VK_strncmp(const HCHAR *s1, const HCHAR *s2, HUINT32 n);
HCHAR 		VK_toupper(HCHAR c);
HCHAR 		VK_tolower(HCHAR c);
HINT32		VK_strcmp (const HCHAR *s1, const HCHAR *s2);
HINT32		VK_strcasecmp (const HCHAR *s1, const HCHAR *s2);
HUINT32 	VK_strlen(const HCHAR *str);
HCHAR		*VK_strcat(HCHAR *s1, const HCHAR *s2);
HCHAR		*VK_strncat(HCHAR *s1, const HCHAR *s2, HINT32 cnt);
HINT32		VK_atoi (const HCHAR *s);
HBOOL 		VK_isdigit(const HCHAR ch);
HBOOL 		VK_isprint(const HCHAR ch);
HINT32 		VK_puts(const HCHAR *str);
HINT32 		VK_putc(HINT32 c, void *fp);
HINT32 		VK_putchar(HINT32 ch);
HINT32		VK_getchar(void);
HINT32 		VK_getch(void);
HINT32 		VK_printf(const HCHAR *templat, ...);
HINT32 		VK_sprintf(HCHAR *buf, const HCHAR *templat, ...);
HINT32 		VK_snprintf(HCHAR *buf, HINT32 len, const HCHAR *templat, ...);
HINT32 		VK_fprintf(void *fp, const HCHAR *templat, ...);
HCHAR		*VK_fgets (HCHAR *buf, HINT32 size, void *fp);
HCHAR 		*VK_strtok(HCHAR *s, HCHAR *delim);
HCHAR 		*VK_strtok_r(HCHAR *s, const HCHAR *delim, HCHAR **lasts);

/***************************/
#else /* CONFIG_VK_STDLIB */
/***************************/

#define VK_STDIN			stdin
#define VK_STDOUT			stdout
#define VK_STDERR			stderr

#define VK_malloc VK_MEM_Alloc
#define VK_free	  VK_MEM_Free
#define VK_memcpy memcpy
#define VK_memset memset
#define VK_memcmp memcmp

char *VK_StrDup(const char *p);

#define 	VK_strdup		VK_StrDup
#define 	VK_Isprint		VK_isprint
#define		VK_StrToUpper	VK_strtoupper
#define		VK_StrCaseCmp 	VK_strcasecmp


HCHAR 		*VK_strreplacechar (HCHAR *dst, const HCHAR *src, HCHAR from, HCHAR to);
HCHAR		*VK_strreplace (HCHAR *dst, const HCHAR *src, const HCHAR *from, const HCHAR *to);
HCHAR		*VK_strcasestr(const HCHAR *s1, const HCHAR *s2);
HCHAR 		*VK_strdupex (const HCHAR *szOld, HUINT32 n, void *(*_alloc)(HUINT32));
void 		*VK_memdupex(const void *data, HUINT32 size, void *(_alloc)(HUINT32));
void 		*VK_memdup(const void *data, HUINT32 size);
HUINT32		VK_atoh(const HCHAR *s);
HUINT32		VK_atoui (const HCHAR *s);
HBOOL		VK_strisnumber(const HCHAR *s);
HBOOL 		VK_ishexdigit(const HUINT8 ch);
HCHAR		*VK_strupper (HCHAR *dest, const HCHAR *src);
HCHAR		*VK_strlower (HCHAR *dest, const HCHAR *src);

#define VK_fflush			fflush
#define VK_scanf			scanf
#define VK_sscanf			sscanf
#define VK_strstr 			strstr
#define VK_strchr			strchr
#define VK_strncasecmp 		strncasecmp
#define VK_toupper 			toupper
#define VK_tolower 			tolower
#define VK_strcasecmp 		strcasecmp
#define VK_atoi 			atoi
#define VK_isdigit 			isdigit
#define VK_isprint 			isprint
#define VK_puts 			puts
#define VK_putc				putc
#define VK_putchar 			putchar
#define VK_getchar 			getchar
#define VK_printf 			printf
#define VK_sprintf 			sprintf
#define VK_snprintf 		snprintf
#define VK_fprintf 			fprintf
#define VK_fgets			fgets
#define VK_vsprintf			vsprintf
#define VK_vsnprintf		vsnprintf
#define VK_vprintf			vprintf
#define VK_strtok			strtok
#define VK_strtok_r			strtok_r
#define VK_strtoul			strtoul

#define VK_strcmp(a, b) strcmp((char*)a, (char*)b)
#define VK_strncmp(a, b, c) strncmp((char*)a, (char*)b, c)
#define VK_strcpy(a, b) strcpy((char*)a, (char*)b)
#define VK_strncpy(a, b, c) strncpy((char*)a, (char*)b, c)
#define VK_strlen(a) strlen((char*)a)
#define VK_strcat(a, b)	strcat((char*)a, (char*)b)
#define VK_strncat(a, b, c) strncat((char*)a, (char*)b, c)

/***************************/
#endif /* CONFIG_VK_STDLIB */
/***************************/

/*************************************************************
for uCOS
*************************************************************/
/*************************************************************
	 IPC
*************************************************************/
int VK_IPC_MSG_Open(unsigned long q_count,unsigned long msg_size, const char *q_name, int *q_id);
int VK_IPC_MSG_Send(unsigned int q_id, const void *msg, unsigned int size);
int VK_IPC_MSG_SendTimeout(unsigned int q_id, const void *msg, unsigned int size, unsigned int timeout);
int VK_IPC_MSG_Receive(unsigned int q_id, void *msg, unsigned int size);
int VK_IPC_MSG_ReceiveTimeout(unsigned int q_id, void *msg, unsigned int size, unsigned int timeout);
void VK_IPC_MSG_Close(unsigned int q_id);

typedef int (*pfnVK_IPC_MSG_NotifyFUNC)(void *msg, unsigned int size);
int VK_IPC_MSG_Call(unsigned int queue_id, unsigned int callbackId, unsigned int *pResult, void *msg, unsigned int size);
int VK_IPC_MSG_RegisterCallback(unsigned int queue_id, unsigned int callbackId, pfnVK_IPC_MSG_NotifyFUNC pfnNotify);
int VK_IPC_MSG_UnRegisterCallback(unsigned int queue_id, unsigned int callbackId);

int VK_INTR_Register(void (*CallBackFunc)(void *), const char *name, int LocalArgSize, unsigned long *intrId, VK_INTR_TASK_E task_num);
int VK_INTR_Unregister(unsigned long intrId);
int VK_INTR_Event(unsigned long intrId, void *pMsg);


/*************************************************************
	 SYSTEM
*************************************************************/
int VK_SYSTEM_Command(const char *command);


/*************************************************************
	 FTP
*************************************************************/
int VK_TCP_socket(void);
int VK_TCP_connect(int s, unsigned char * dest, unsigned short port);
int VK_TCP_close(int s);
int VK_TCP_send(int s, unsigned char * buf, int len);
int VK_TCP_recv(int s, unsigned char * buf, int len);
int VK_TCP_status(int s, int *connflag, int *rxready, int *rxeof);


/*************************************************************
	 SIGNAL
*************************************************************/
int VK_SIG_Init(void);
int VK_SIG_SetSigAction(int sig, void (*pfnSignalUserHandler)(int sig, VK_SIG_INFO_t *siginfo_t, void *context));
int VK_SIG_SetSigHooking(int sig);

/*************************************************************
       SOCKET
*************************************************************/
typedef enum {
	VK_SOCKET_TYPE_UDS,
	VK_SOCKET_TYPE_INET
} VK_SOCKET_TYPE_E;
typedef enum {
	VK_SOCKET_FLAG_SERVER,
	VK_SOCKET_FLAG_CLIENT
} VK_SOCKET_FLAG_E;

typedef void (* VK_SOCKET_DataListener_Cb)(void *userdata, HUINT32 sock, HUINT32 req_id, void *pvData, HUINT32 size);

int VK_SOCKET_SERVER_Open(VK_SOCKET_TYPE_E eSocketType, void *pServerPort, HUINT32 *hSocket);
int VK_SOCKET_CLIENT_Open(VK_SOCKET_TYPE_E eSocketType, const HCHAR *pszIPAddr, void *targetPort, HUINT32 nTry, HUINT32 *hSocket);
int VK_SOCKET_Close(HUINT32 sock);
int VK_SOCKET_Listen(HUINT32 sock, HUINT32 timeout, VK_SOCKET_DataListener_Cb cbPacketReceived, void *userdata);
int VK_SOCKET_Write(HUINT32 sock, const void *pvData, HUINT32 size, HUINT32 reqid);
int VK_SOCKET_Init(void);
void VK_SOCKET_PrintSockInfo(void);


/*************************************************************
       RPC
*************************************************************/
enum {
	VkRPC_CALLTYPE_CALL,
	VkRPC_CALLTYPE_REQUEST,
	VkRPC_CALLTYPE_RESPONSE,
	VkRPC_CALLTYPE_NOTIFY
};

typedef enum {
	VkOBJECT_TYPE_BIN	= 'b',
	VkOBJECT_TYPE_STR	= 's',
	VkOBJECT_TYPE_INT	= 'i',
	VkOBJECT_TYPE_INT64 = 'l'
} VKRPC_OBJECT_TYPE_E;

typedef struct {
	VKRPC_OBJECT_TYPE_E type;
	union {
		struct {
			HINT32 num;
		} i;
		struct {
			HINT64 num64;
		} l;
		struct {
			HCHAR *str;
		} s;
		struct {
			HUINT32 size;
			void	*data;
		} b;
	} u;
} VKRPC_OBJECT_T;

typedef HINT32 (*VkRPC_Func_t)(HUINT32 nSockClient, HINT32 nArgc, VKRPC_OBJECT_T **apArgv, HUINT32 nRequestId, void *pvUserdata);
typedef HINT32 (*VkRPC_Noti_t)(HINT32 nArgc, VKRPC_OBJECT_T **apArgv, HUINT32 nRequestId, void *pvUserData);
typedef HINT32 (*VkRPC_Response_t)(HINT32 nArgc, VKRPC_OBJECT_T **apArgv, HUINT32 nRequestId, void *pvUserData);

int VK_RPC_Open(HUINT32 nRoute, HINT32 nProgId, HUINT32 *hRpc);
int VK_RPC_Close(HUINT32 hRpc);
int VK_RPC_RegisterCall(HUINT32 hRpc, const HCHAR *pszMethodName, const HCHAR *pszSignature, VkRPC_Func_t pfnFunction, void *pvUserData);
int VK_RPC_RegisterNotifier(HUINT32 hRpc, const HCHAR *pszNotifierName, void *pvData, VkRPC_Noti_t pfnNotifierCb);
int VK_RPC_Notify(HUINT32 hRpc, const HCHAR *pszNotifierName, const HCHAR *pszSignature, ...);
int VK_RPC_Request(HUINT32 hRpc, VkRPC_Response_t pfnCallback, HUINT32 nTimeout, void *pvUserdata, const HCHAR *pszMethodName, const HCHAR *pszSignature, ...);
int VK_RPC_Call(HUINT32 hRpc, const HCHAR *pszMethodName, const HCHAR *pszSignature, ...);
int VK_RPC_Response(HUINT32 hRpc, HUINT32 hSockClient, HUINT32 nReqId, const HCHAR *pszMethodName, const HCHAR *pszSignature, ...);
int VK_RPC_ProcessRpc(HUINT32 hRpc, HUINT32 nReqId, void *pvData, HUINT32 ulSize, HUINT32 hClientSock);
int VK_RPC_Init(void);
void VK_RPC_PrintRpcInfo(void);



// to make angry compile be happy.
#define HAPPY(a) (void)(a)

#if defined(WIN32)
int VK_RegisterThreadAndMessageQueuePair(unsigned int tid, unsigned int qid);
#else
#define VK_RegisterThreadAndMessageQueuePair
#endif

#if defined(CONFIG_DEBUG)
#define CHECK	VK_printf("Check Line : %s(%d) tick[%lu]\n", __FILE__, __LINE__, VK_TIMER_GetSystemTick())
#else
#define CHECK
#endif

typedef enum
{
	eTASKP_CIMGR=50,
	eTASKP_PC_CARD,
	eTASKP_CIPHY=52,
	eTASKP_CILNK,
	eTASKP_CITRAN,
	eTASKP_CISESS,
	eTASKP_CIRES
}Humax_Task_Priority;

#if defined(VK_DBG_Print)
#undef VK_DBG_Print
#define VK_DBG_Print(...)	((void)0)
#else
extern void	VK_DBG_Print(const char *fmt, ...);
#endif


#if defined(CONFIG_DEBUG)
#define	CHECK_TIME			VK_printf("====> %s (%d): %ld\n", __FUNCTION__, __LINE__, VK_TIMER_GetSystemTick())
#else
#define	CHECK_TIME
#endif

#if defined(CONFIG_DEBUG)
void VK_DBG_SetRefTick(unsigned char *Str);
#endif
//#define CHECK_BOOTING_TIME

#if defined(CHECK_BOOTING_TIME)
extern unsigned long	BOOTING_StartTime;
extern unsigned long	Last_BOOTING_StartTime;
extern unsigned long	Check_Temp_Time;
#define	CHECK_BOOT_TIME(x)			\
		do									\
		{									\
			Check_Temp_Time = VK_TIMER_GetSystemTick();\
			VK_printf("==>BOOTING\t[%s]\t\t%ld\t%ld\n", x, (Check_Temp_Time - BOOTING_StartTime), (Check_Temp_Time-Last_BOOTING_StartTime));\
			Last_BOOTING_StartTime = Check_Temp_Time;\
		}while(0)
#else
#define	CHECK_BOOT_TIME(x)	((void)0)
#endif

#if defined(__cplusplus)
}; /* extern "C"; */
#endif

#endif /* _VKERNEL_H_ */

