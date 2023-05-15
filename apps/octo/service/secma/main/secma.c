/**************************************************************************************************/
#define ________Secure_Handler_Private_Include______________________________________________
/**************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>

#include "secma_init.h"


#ifdef CONFIG_DEBUG
#if !defined(VK_Print)
#define VK_Print(_fmt, ...)				printf(_fmt, ## __VA_ARGS__)
#endif
#else
#if !defined(VK_Print)
#define VK_Print(_fmt, ...)				((void)0)
#endif
#endif

#if defined(VK_Print)
#undef VK_Print
#define VK_Print(_fmt, ...)				printf(_fmt, ## __VA_ARGS__)
#endif


/**************************************************************************************************/
#define ________Secure_Handler_Golbal_Value________________________________________________
/**************************************************************************************************/
extern char **environ;


/**************************************************************************************************/
#define ________Secure_Handler_Private_Define_______________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Secure_Handler_Private_Type_Define__________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Secure_Handler_Private_Static_Value__________________________________________
/**************************************************************************************************/
static HINT32 s_ulMsgQId = 0;


/**************************************************************************************************/
#define ________Secure_Handler_Private_Static_Prototype______________________________________
/**************************************************************************************************/
static int local_secma_RunProcess(const unsigned char *szPath, const unsigned char *szName);
static int local_secma_ForkChild(void *pvMsg, HUINT32 ulSize);
static int local_secma_InitSecma(void);


/**************************************************************************************************/
#define ________Secure_Handler_Private_Static_Prototype_Body_________________________________
/**************************************************************************************************/
static int local_secma_RunProcess(const unsigned char *szPath, const unsigned char *szName)
{
	int nPid, nErr;

	nPid = vfork();
	if (nPid == -1)
	{
		VK_Print("[%d] error in vfork....\n", __LINE__);
		return nPid;
	}

	if (nPid == 0)		// child process
	{
		HUINT8 ucFullPath[100];

		memset(ucFullPath, 0, 100);
		snprintf(ucFullPath, 100, "%s%s\n", szPath, szName);
		nErr = execvp((const char*)ucFullPath, NULL);
		if (nErr == -1)
		{
			VK_Print("[%d] error in execl....\n", __LINE__);
			return nErr;
		}
		exit(-1);
	}

	return nPid;
}


static int local_secma_ForkChild(void *pvMsg, HUINT32 ulSize)
{
	int nPid, nErr = 0;
	IpcSecManager_t stSecMgr;

	VK_memset(&stSecMgr, 0, sizeof(IpcSecManager_t));
	VK_memcpy(&stSecMgr, pvMsg, sizeof(IpcSecManager_t));

	VK_Print("\n\n\t ===============> local_secma_ForkChild : stSecMgr.eMsgType = %d\n", stSecMgr.eMsgType);

	nPid = vfork();
	if (nPid == -1)
	{
		VK_Print("[%s, %d] error in vfork....\n", __FUNCTION__, __LINE__);
		return nPid;
	}

	if (nPid == 0)		// child process
	{
		char *argv[3];
		char *pucTarget;
		unsigned long len = 0;

		len = strlen(stSecMgr.ucTarget);
		pucTarget = HLIB_STD_MemAlloc(len + 1);
		strncpy(pucTarget, stSecMgr.ucTarget, len);
		pucTarget[len] = '\0';

		argv[0] = "secma";
		argv[1] = pucTarget;
		argv[2] = 0;

		execve("/usr/bin/secma", argv, environ);
		HLIB_STD_MemFree(pucTarget);
		exit(0);
	}

	VK_Print("\n\n\t ===============> local_secma_ForkChild :nPid = %d\n", nPid);

	return nErr;
}


static int local_secma_InitSecma(void)
{
	int  nErr = ERR_OK;

	nErr = VK_IPC_MSG_Open(0, IPC_MSG_Q_BUFF_MAX, IPC_SYS_MSGQ_PATH_SECMA, &s_ulMsgQId);
	if (nErr != VK_OK)
	{
		VK_Print("[%s] error in VK_IPC_MSG_Open....\n", __FUNCTION__);
	}

	nErr |= VK_IPC_MSG_RegisterCallback(s_ulMsgQId, IPC_SEC_CONV_FN_InitSec, local_secma_ForkChild);

	return nErr;
}


int main(int argc, char** argv)
{
	int rc;

	(void)argc;
	(void)argv;

	rc = VK_Init();
	if (rc != 0)
	{
		VK_Print("%s-VK_init(): Error Code(%d), Line(%d), %s\n", __FUNCTION__, rc, __LINE__, __FILE__);
		exit(-1);
	}

	if (argc <= 1)
	{
		local_secma_InitSecma();

#if defined(CONFIG_DEBUG)
		local_secma_RunProcess("/usr/bin/", "cmdmanager");
#endif

		//dfbmaster를 여기서 실행한다.
		local_secma_RunProcess("/usr/bin/", "dfbmaster");

		//dama를 여기서 실행한다.
		local_secma_RunProcess("/usr/bin/", "dama");

		//hama를 여기서 실행한다.
		local_secma_RunProcess("/usr/bin/", "hama");
	}
	else
	{
		IpcSecManager_t stSecMgr;

		if (SECMA_HANDLE_FindMsgByTarget(argv[1], &stSecMgr) != ERR_OK)
		{
			VK_Print("%s: Error Code(%d), Line(%d), %s\n", __FUNCTION__, rc, __LINE__, __FILE__);
			exit(-1);
		}

		SECMA_HANDLE_Init(stSecMgr.eMsgType);
		SECMA_HANDLE_InitRegisterFunctions(stSecMgr.eMsgType);
	}


	/* Wait forever */
	while(1)
	{
		VK_TASK_Sleep(360000);
	}

   	exit(EXIT_SUCCESS);
}


/**************************************************************************************************/
#define __________Secure_Handler_Public_Functions_Body______________________________________
/**************************************************************************************************/


