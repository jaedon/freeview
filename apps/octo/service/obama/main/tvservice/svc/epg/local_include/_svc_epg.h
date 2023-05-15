
#ifndef __SVC_EPG_INT_H__
#define __SVC_EPG_INT_H__


#include <ctype.h>
#include <assert.h>


#include <octo_common.h>

#include <db_param.h>

#include <pal_sef.h>
#include <util.h>
#include <pal_pipe.h>
#include <svc_epg.h>
#include <isosvc.h>

#include "_svc_epg_raw_mgr.h"
#include "_svc_epg_common.h"
#include "_svc_epg_common_lib.h"
#include "_svc_epg_db_raw_lib.h"
#include "_svc_epg_context_mgr.h"
#include "_svc_epg_eit_lib.h"


#define	OxEPG_HOTFIX
#define	SVC_EPG_OPT_QUERY_OPTIMIZE

#define	SVC_EPG_OPT_FAST_KEYWORD_QUERY
//#define	OxEPG_FLUSH_TEST		30

#define	RETURN_IF_VOID(expr)	do{if(expr)return;}while(0)
#define	RETURN_IF(expr, err)	do{if(expr)return (err);}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr)goto pos;}while(0)

#define	D_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return;}}while(0)
#define	D_RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)
#define	D_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);goto pos;}}while(0)

#define	W_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);return;}}while(0)
#define	W_RETURN_IF(expr, err)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);return (err);}}while(0)
#define	W_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);goto pos;}}while(0)

#define	E_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return;}}while(0)
#define	E_RETURN_IF(expr, err)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return (err);}}while(0)
#define	E_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);goto pos;}}while(0)

#if defined(CONFIG_DEBUG) && 0
#if 0
struct sema {
	HUINT32	sem;
	const char *func;
	int			line;
};
static struct sema	s_sem[50];
static int			s_size;

static int		OxEPG_SEM_GET(HUINT32 sem, HUINT32 timeout, const char *func, int line)
{
	int ret = VK_SEM_GetTimeout(sem, timeout);
	int i;
	if (ret == 0)
	{
		for (i = 0 ; i < 50 ; i++)
		{
			if (s_sem[i].sem == 0)
			{
				s_sem[i].sem  = sem;
				s_sem[i].func = func;
				s_sem[i].line = line;
				break;
			}
		}
	}
	else
	{
		for (i = 0 ; i < 50 ; i++)
		{
			if (s_sem[i].sem == sem)
			{
				HxLOG_Warning("semaphore locked by %s:%d\n", s_sem[i].func, s_sem[i].line);
				break;
			}
		}
	}
	return ret;
}

static int		OxEPG_SEM_REL(HUINT32 sem)
{
	int	ret;
	int	i;

	for (i = 0 ; i < 50 ; i++)
	{
		if (s_sem[i].sem == sem)
		{
			s_sem[i].sem = 0;
			break;
		}
	}
	VK_SEM_Release(sem);
}
#endif

#define	OxEPG_SEM_ENTER(sem)	\
	do {\
		if(VK_SEM_GetTimeout(sem, 5000)){\
			HxLOG_Critical("semaphore dead lock?\n");\
			VK_SEM_Get(sem);\
		}\
	}while(0)
#else
#define	OxEPG_SEM_ENTER(sem)	VK_SEM_Get(sem)
#endif
#define	OxEPG_SEM_LEAVE(sem)	VK_SEM_Release(sem)

PostMsgToMgrCb_t svc_epg_GetPostMsgCb(void);
HERROR	svc_epg_GetMsgQueueId(HUINT32 *pulMsgQueueId);

HERROR svc_epg_SiGetTableTimeOut(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout);


#endif /* __SVC_EPG_INT_H__ */


