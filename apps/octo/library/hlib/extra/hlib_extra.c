/**
	@file     hlib_extra.c

	Description: Security Extra Function.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hlib.h>

//#define	TEST_SECURITY

#if defined(CONFIG_DROP_ROOT_PRIVILEGES) || defined(TEST_SECURITY)
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/stat.h>
#endif

#include <fcntl.h>
#include <sys/mman.h>

#if defined(CONFIG_DEBUG)
#include <sys/prctl.h>
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	HLIB_IPSFILE_PATH				"/etc/ipc.def"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Function Body        ********************/
/*******************************************************************/
#if	defined(CONFIG_DROP_ROOT_PRIVILEGES) || defined(TEST_SECURITY)
static HERROR hlib_extra_Readgroups (const HCHAR *pszUserName, gid_t gid, HUINT32 *pulGrpCnt, gid_t **ppGroups)
{
	int val;
	int ngroups = 0;
	gid_t *groups = NULL;

	*pulGrpCnt = 0;

	HxLOG_Print("User Name[%s], gid [%d]\n", pszUserName, gid);

	val = getgrouplist(pszUserName, gid, NULL, &ngroups);
	if(ngroups <= 1)
	{
		HxLOG_Error("getgrouplist val:[%d], ngroups[%d]\n", val, ngroups);
		return ERR_FAIL;
	}

	groups = (gid_t *) HLIB_MEM_Malloc(ngroups * sizeof (gid_t));
	if (groups == NULL)
	{
		HxLOG_Error("Malloc is NULL\n");
		return ERR_FAIL;
	}

	val = getgrouplist(pszUserName, gid, groups, &ngroups);
	if (val < 0)
	{
		HxLOG_Error("getgrouplist no groups\n");
		HLIB_MEM_Free(groups);
		return ERR_FAIL;
	}

#if 0//defined (CONFIG_DEBUG)
	{
		gid_t tmp_groups[10];
		int val, i;

		HxLOG_Print("###########################################################################\n");
		for(i = 0; i < ngroups; i++)
		{
			struct group *gr;

			HxLOG_Print("getgrouplist: [%d], gid [%u]", i, groups[i]);
			gr = getgrgid(groups[i]);
			if (gr != NULL)
				HxLOG_Print(", gname[%s]\n", gr->gr_name);
		}
		HxLOG_Print("###########################################################################\n");
	}
#endif

	*pulGrpCnt = ngroups;
	*ppGroups = groups;

	return ERR_OK;
}

static HERROR hlib_extra_ChangeGroups(const HCHAR *pszUserName, gid_t gid)
{
	HUINT32 ulGrpCnt;
	HERROR	hErr;
	gid_t	*gids;

	hErr = hlib_extra_Readgroups(pszUserName, gid, &ulGrpCnt, &gids);

	if(hErr != ERR_OK || ulGrpCnt <= 1)
	{
		return ERR_FAIL;
	}
	else
	{
		if(setgroups(ulGrpCnt, gids) != 0)
		{
			HLIB_MEM_Free(gids);
			HxLOG_Error("setgroups failed %s\n", strerror(errno));
			return ERR_FAIL;
		}

		HxLOG_Print("setgroups OK\n");
		HLIB_MEM_Free(gids);
	}

	return ERR_OK;
}
#endif

HERROR	HLIB_EXTRA_DropRootPrivileges(const HCHAR *pszUserName)
{
#if	defined(CONFIG_DROP_ROOT_PRIVILEGES) || defined(TEST_SECURITY)
	struct passwd *u_info;
//	struct group  *g_info;

	u_info = getpwnam(pszUserName);
//	g_info = getgrnam(pszUserName);

	{
		gid_t gid = (gid_t)u_info->pw_gid;
		uid_t uid = (uid_t)u_info->pw_uid;

		if(hlib_extra_ChangeGroups(pszUserName, gid) != ERR_OK)
		{
			HxLOG_Error("hlib_extra_ChangeGroups failed\n");
		}

		return HLIB_EXTRA_DropRootPrivilegesWithParam(uid, gid);
	}
#endif

	return ERR_OK;
}

HERROR	HLIB_EXTRA_DropRootPrivilegesWithParam(HINT32 nUserId, HINT32 nGroupId)
{
#if	defined(CONFIG_DROP_ROOT_PRIVILEGES) || defined(TEST_SECURITY)
	uid_t	real, eff, saved, uid;
	gid_t	gid;

	uid = (uid_t)nUserId;
	gid = (gid_t)nGroupId;
	HxLOG_Debug("Running as UID %u (effective %u) GID %u (effective %u)\n", getuid(), geteuid(), getgid(), getegid());

	if (setresgid(gid, gid, gid) != 0)
	{
		HxLOG_Error("Failed changing GID to %d with error %s\n", gid, strerror(errno));
		exit(-1);
	}

	if (setresuid(uid, uid, uid) != 0)
	{
		HxLOG_Error("Failed changing UID to %d with error %s\n", uid, strerror(errno));
		exit(-1);
	}

	if (getresuid(&real, &eff, &saved) != 0)
	{
		HxLOG_Error("Failed reading UID with error %s\n", strerror(errno));
		exit(-1);
	}

	if (real != uid || eff != uid || saved != uid)
	{
		HxLOG_Error("UID sanity check failed\n");
		exit(-1);
	}

	if (getresgid(&real, &eff, &saved) != 0)
	{
		HxLOG_Error("Failed reading GID with error %s\n", strerror(errno));
		exit(-1);
	}

	if (real != gid || eff != gid || saved != gid)
	{
		HxLOG_Error("GID sanity check failed\n");
		exit(-1);
	}

#if defined(CONFIG_DEBUG)
 	prctl(PR_SET_DUMPABLE, 1);
#endif

	HxLOG_Debug("Dropped root priviliges. Now running as UID %u GID %u\n", uid, gid);

#endif

	return ERR_OK;
}

HERROR	HLIB_EXTRA_GetUserAndGroupIdFromUserInfo(const HCHAR *pszUserName, HUINT32 *pulUid, HUINT32 *pulGid)
{
#if	defined(CONFIG_DROP_ROOT_PRIVILEGES) || defined(TEST_SECURITY)
	struct passwd *u_info;

	u_info = getpwnam(pszUserName);

	if(u_info == NULL)
	{
		HxLOG_Error("[%s][getpwnam] Get User Info from etc/passwd\n", __FUNCTION__);

		if(pulUid != NULL)
			*pulUid = 0;

		if(pulGid != NULL)
			*pulGid = 0;
		return ERR_FAIL;
	}

	if(pulUid != NULL)
		*pulUid = (HUINT32)u_info->pw_uid;

	if(pulGid != NULL)
		*pulGid = (HUINT32)u_info->pw_gid;
#else
	if(pulUid != NULL)
		*pulUid = 0;

	if(pulGid != NULL)
		*pulGid = 0;
#endif

	return ERR_OK;
}

HERROR	HLIB_EXTRA_GetGroupIdFromGroupInfo(const HCHAR *pszGroupName, HUINT32 *pulGid)
{
#if	defined(CONFIG_DROP_ROOT_PRIVILEGES) || defined(TEST_SECURITY)
	struct group  *g_info;

	g_info = getgrnam(pszGroupName);

	if(g_info == NULL)
	{
		HxLOG_Error("[getgrnam] Get Group Info from etc/group\n");
		if(pulGid != NULL)
			*pulGid = 0;
		return ERR_FAIL;
	}

	if(pulGid != NULL)
		*pulGid = (HUINT32)g_info->gr_gid;
#else
	if(pulGid != NULL)
		*pulGid = 0;
#endif

	return ERR_OK;
}

HERROR	HLIB_EXTRA_AmIRoot(HBOOL *pbRoot)
{
#if defined(CONFIG_DROP_ROOT_PRIVILEGES) || defined(TEST_SECURITY)
	uid_t	real, eff, saved, uid;
	struct	passwd *u_info;

	*pbRoot = FALSE;

	u_info = getpwnam("root");
	if(u_info == NULL)
	{
		HxLOG_Warning("[%s][getpwnam] Get User Info from etc/passwd\n", __FUNCTION__);
		return ERR_FAIL;
	}

	uid = (uid_t)u_info->pw_uid;
	if (getresuid(&real, &eff, &saved) != 0)
	{
		HxLOG_Warning("[getresuid] Failed reading UID with error %s\n", strerror(errno));
		return ERR_FAIL;
	}

	if (real == uid || eff == uid || saved == uid)
	{
		*pbRoot = TRUE;
	}
#endif

	return ERR_OK;
}


HERROR HLIB_EXTRA_SetUmask(HUINT32 ulMask)
{
#if defined(CONFIG_DROP_ROOT_PRIVILEGES) || defined(TEST_SECURITY)
	mode_t old_mask;

	old_mask = umask(ulMask);

	HxLOG_Print("Changed mask value from [%04o] to [%04o]\n", old_mask, ulMask);
#endif

	return ERR_OK;
}


HERROR	HLIB_EXTRA_Chown(const HCHAR *pszPathName, const HCHAR *pszUserName, const HCHAR *pszGroupName)
{
#if defined(CONFIG_DROP_ROOT_PRIVILEGES) || defined(TEST_SECURITY)
	HINT32	nErr;
	struct passwd *u_info = NULL;
	struct group  *g_info = NULL;
	gid_t gid;
	uid_t uid;

	u_info = getpwnam(pszUserName);
	if(u_info == NULL)
	{
		HxLOG_Error("[%s][getpwnam] Get User Info from etc/passwd\n", __FUNCTION__);
		return ERR_FAIL;
	}

	uid = (uid_t)u_info->pw_uid;

	g_info = getgrnam(pszGroupName);
	if(u_info == NULL)
	{
		HxLOG_Warning("[getgrnam] Get Group Info from etc/passwd\n");
		gid = (gid_t)u_info->pw_gid; /* group에서 찾을 수 없으면 passwd 에서 찾은 정보를 사용하자 */
	}
	else
	{
		gid = (gid_t)g_info->gr_gid;
	}

	nErr = chown(pszPathName, uid, gid);
	if (nErr < 0)
	{
		HxLOG_Error("error in chown (0x%x)\n", nErr);
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}

HERROR	HLIB_EXTRA_ChownWithParam(const HCHAR *pszPathName, uid_t uid, gid_t gid)
{
#if defined(CONFIG_DROP_ROOT_PRIVILEGES) || defined(TEST_SECURITY)
	HINT32	nErr;

	nErr = chown(pszPathName, uid, gid);
	if (nErr < 0)
	{
		HxLOG_Error("error in chown (0x%x)\n", nErr);
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}

HERROR	HLIB_EXTRA_Chmod(const HCHAR *pszPathName, HUINT32 ulMode)
{
#if defined(CONFIG_DROP_ROOT_PRIVILEGES) || defined(TEST_SECURITY)
	HINT32	nErr;

	nErr = chmod(pszPathName, ulMode);
	if (nErr < 0)
	{
		HxLOG_Error("error in chmod (0x%x)\n", nErr);
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}

HERROR	HLIB_EXTRA_Chroot(const HCHAR *pszPathName)
{
#if defined(CONFIG_DROP_ROOT_PRIVILEGES) || defined(TEST_SECURITY)
	HINT32	nErr;

	/* 1. Close open file descriptors */
#if 0 /* Blocked because of segmentation fault issue. */
	HINT32	i;
 	struct rlimit rlim;

	if (getrlimit(RLIMIT_NOFILE, &rlim))
	{
		HxLOG_Error("getrlimit() for RLIMIT_NOFILE failed with error %s\n", strerror(errno));
		return ERR_FAIL;
	}

	for (i = 0; i < rlim.rlim_max; i++)
	{
		while (close(i) != 0 && errno == EINTR);
	}
#endif

	/* 2. chroot */
	nErr = chroot(pszPathName);
	if (nErr != 0)
	{
		HxLOG_Error("error in chroot (0x%x)\n", nErr);
		return ERR_FAIL;
	}

	/* 3. chdir */

	nErr = chdir("/");
	if (nErr != 0)
	{
		HxLOG_Error("error in chdir (0x%x)\n", nErr);
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}


#if	defined(CONFIG_PROCESS_CAPABILITIES_CONTROL) || defined(TEST_SECURITY)		//	must have libcap.
#include <linux/capability.h>
#include <sys/capability.h>
#include <sys/prctl.h>

typedef struct
{
	HINT32			nKernelCap;
	HxEXTRA_CAP_e	nHxExtraCap;
} _HLIB_Cap_t;

#define	CAPSIZE		(sizeof(s_snCapability) / sizeof(s_snCapability[0]))
static _HLIB_Cap_t	s_snCapability[] =
{
	{	CAP_AUDIT_CONTROL,		HxEXTRA_CAP_AUDIT_CONTROL	},	// Control kernel auditing [ > 2.6.11]
	{	CAP_AUDIT_WRITE,		HxEXTRA_CAP_AUDIT_WRITE		},	// Write records to kernel audit log [ > 2.6.11 ]
	{	CAP_CHOWN,				HxEXTRA_CAP_CHOWN			},	// Change file user and group
	{	CAP_DAC_OVERRIDE,		HxEXTRA_CAP_DAC_OVERRIDE	},	// Bypass file permission checks
	{	CAP_DAC_READ_SEARCH,	HxEXTRA_CAP_DAC_READ_SEARCH	},	// Read all files and directories
	{	CAP_FOWNER,				HxEXTRA_CAP_FOWNER			},	// Perform operations on files owned by other users such as chmod, chattr or utime
	{	CAP_FSETID,				HxEXTRA_CAP_FSETID			},	// Keep suid and guid permissions when modifying a file
	{	CAP_IPC_LOCK,			HxEXTRA_CAP_IPC_LOCK		},	// Lock memory (mlock, mlockall, mmap, shmctl)
	{	CAP_IPC_OWNER,			HxEXTRA_CAP_IPC_OWNER		},	// Bypass shared memory permission checks
	{	CAP_KILL,				HxEXTRA_CAP_KILL			},	// Send signals to any process
	{	CAP_LEASE,				HxEXTRA_CAP_LEASE			},	// Establish leases on any file
	{	CAP_LINUX_IMMUTABLE,	HxEXTRA_CAP_LINUX_IMMUTABLE	},	// Make files immutable and remove ability append to file using chattr
	{	CAP_MAC_ADMIN,			HxEXTRA_CAP_MAC_ADMIN		},	// Perform MAC administration (for Smack LSM) [ > 2.6.25 ]
	{	CAP_MAC_OVERRIDE,		HxEXTRA_CAP_MAC_OVERRIDE	},	// Override MAC (for Smack LSM) [ > 2.6.25 ]
	{	CAP_MKNOD,				HxEXTRA_CAP_MKNOD			},	// Create files using mknod()
	{	CAP_NET_ADMIN,			HxEXTRA_CAP_NET_ADMIN		},	// Perform network administration such as configuring interfaces or routing tables
	{	CAP_NET_BIND_SERVICE,	HxEXTRA_CAP_NET_BIND_SERVICE},	// Bind socket to privileged port (< 1024)
	{	CAP_NET_BROADCAST,		HxEXTRA_CAP_NET_BROADCAST	},	// Make socket broadcasts and listen to multicasts
	{	CAP_NET_RAW,			HxEXTRA_CAP_NET_RAW			},	// Use RAW and PACKET sockets
	{	CAP_SETGID,				HxEXTRA_CAP_SETGID			},	// Change to any Group ID
	{	CAP_SETFCAP,			HxEXTRA_CAP_SETFCAP			},	// Set file capabilities [ > 2.6.24 ]
	{	CAP_SETPCAP,			HxEXTRA_CAP_SETPCAP			},	// Set capabilities if file capabilities not present
	{	CAP_SETUID,				HxEXTRA_CAP_SETUID			},	// Change to any User ID
	{	CAP_SYS_ADMIN,			HxEXTRA_CAP_SYS_ADMIN		},	// Perform privileged system administration operations such as mount, swapon, sethostname and keyctl
	{	CAP_SYS_BOOT,			HxEXTRA_CAP_SYS_BOOT		},	// Can reboot and kexec_load
	{	CAP_SYS_CHROOT,			HxEXTRA_CAP_SYS_CHROOT		},	// Can chroot
	{	CAP_SYS_MODULE,			HxEXTRA_CAP_SYS_MODULE		},	// Can load and unload kernel modules
	{	CAP_SYS_NICE,			HxEXTRA_CAP_SYS_NICE		},	// Change process priority levels and scheduling class (e.g. to real-time)
	{	CAP_SYS_PACCT,			HxEXTRA_CAP_SYS_PACCT		},	// Can use acct
//	Humax Kernel Doesn't support it.
//	{	CAP_SYS_TRACE,			HxEXTRA_CAP_SYS_TRACE		},	// ptrace any process
	{	CAP_SYS_RAWIO,			HxEXTRA_CAP_SYS_RAWIO		},	// Perform I/O port operations and access /proc/kcore
	{	CAP_SYS_RESOURCE,		HxEXTRA_CAP_SYS_RESOURCE	},	// Set or override resource limits
	{	CAP_SYS_TIME,			HxEXTRA_CAP_SYS_TIME		},	// Can set the system clock and real-time hardware clock
	{	CAP_SYS_TTY_CONFIG,		HxEXTRA_CAP_SYS_TTY_CONFIG	}	// Can hangup the current tty
};

static HBOOL hlib_extra_findKernelCap(HxEXTRA_CAP_e eCap, cap_value_t *pnCap)
{
	HINT32	i;

	if (pnCap == NULL)
		return FALSE;

	for (i = 0; i < CAPSIZE; i++)
	{
		if (eCap == s_snCapability[i].nHxExtraCap)
		{
			*pnCap = s_snCapability[i].nKernelCap;

			return TRUE;
		}
	}

	return FALSE;
}

HERROR	HLIB_EXTRA_LimitRootCapability(HINT32 nCap, ...)
{
	HINT32	i;
	cap_t	caps;
	cap_value_t			*cap_values;

	va_list		ap;

	if (nCap == 0)
	{
		HxLOG_Warning("nCap is zero\n");
		return ERR_OK;
	}

	cap_values = HLIB_MEM_Malloc(nCap * sizeof(cap_value_t));

	i = 0;
	va_start(ap, nCap);
	while (nCap--)
	{
		cap_value_t		value;
		HxEXTRA_CAP_e	eExtraCap;

		eExtraCap = va_arg(ap, HxEXTRA_CAP_e);
		if (hlib_extra_findKernelCap(eExtraCap, &value) == TRUE)
		{
			cap_values[i++] = value;
		} else
		{
			HxLOG_Error("fail to find root capabilities -- eExtraCap [%d]\n", eExtraCap);
		}
	}
	va_end(ap);

	nCap = i;

	caps = cap_init();
	if (caps == NULL)
	{
		HxLOG_Error("cap_init() failed!\n");
		exit(-1);
	}


	if (cap_clear(caps) == -1)
	{
		HxLOG_Error("cap_clear() failed\n");
		exit(-1);
	}

	if (cap_set_flag(caps, CAP_EFFECTIVE, nCap, cap_values, CAP_SET) == -1)
	{
		HxLOG_Error("cap_set_flag() effective failed!\n");
		exit(-1);
	}

	if (cap_set_flag(caps, CAP_INHERITABLE, nCap, cap_values, CAP_SET) == -1)
	{
		HxLOG_Error("cap_set_flag() inheritable failed!\n");
		exit(-1);
	}

	if (cap_set_flag(caps, CAP_PERMITTED, nCap, cap_values, CAP_SET) == -1)
	{
		HxLOG_Error("cap_set_flag() permitted failed\n");
		exit(-1);
	}

	if (cap_set_proc(caps) == -1)
	{
		HxLOG_Error("Setting capabilites by cap_set_proc() failed!\n");
		exit(-1);
	}

	/* Remain capabilities after an identify(UID) changing */
    if (prctl(PR_SET_KEEPCAPS, 1L))
    {
        HxLOG_Error("Unable to free capabilitues using cap_free() \n");
        exit(-1);
    }

	if (cap_free(caps) == -1)
	{
		HxLOG_Error("Unable to free capabilitues using cap_free() \n");
		exit(-1);
	}

	HxLOG_Debug("Dropped capabilities to minimum. Confirming.\n");

#if 0
	/* Check that capabilites have been dropped */
	if (check_caps(cap_values, nCap))
	{
		HxLOG_Error("Failed to drop all intended capabilities\n");
		exit(-1);
	}
#endif
	HxLOG_Debug("Dropped capabilities OK\n");

	HLIB_MEM_Free(cap_values);

	return ERR_OK;
}
#else
HERROR	HLIB_EXTRA_LimitRootCapability(HINT32 nCap, ...)
{
	HxLOG_Error("not supported...\n");

	return ERR_OK;
}
#endif

static HxJSON_t	hlib_extra_GetExtraInfo(void)
{
	static HxJSON_t		s_pstExtraInfo = NULL;

	if (s_pstExtraInfo == NULL)
	{
		s_pstExtraInfo = HLIB_JSON_DecodeFile(HLIB_IPSFILE_PATH);
		if (s_pstExtraInfo == NULL)
		{
			HxLOG_Critical("- can't find or corrupted /etc/ipc.def !\n");
			HxLOG_Assert(0);
		}
	}

	return s_pstExtraInfo;
}

HINT32	HLIB_EXTRA_OpenServer(const HCHAR *pszServerName)
{
	HxJSON_t	extraInfo = hlib_extra_GetExtraInfo();
	HxJSON_t	objInfo;
	const HCHAR	*pszIpc, *pszName;
	HINT32		nPort;

	objInfo = HxJSON_Object_Get(extraInfo, pszServerName);
	if (objInfo == NULL)
	{
		HxLOG_Critical("Can't find object named [%s]\n", pszServerName);
		HxLOG_Assert(0);
	}

	pszIpc = HLIB_JSON_Object_GetString(objInfo, "ipc");
	if (HxSTD_StrCmp(pszIpc, "uds") == 0)
	{
		pszName = HLIB_JSON_Object_GetString(objInfo, "name");
		return HLIB_SOCKET_OpenServer(HxSOCKET_UDS, (void*)pszName);
	} else
	if (HxSTD_StrCmp(pszIpc, "inet") == 0)
	{
		nPort     = HLIB_JSON_Object_GetInteger(objInfo, "port");
		return HLIB_SOCKET_OpenServer(HxSOCKET_INET, (void*)nPort);
	} else
	{
		HxLOG_Critical("not supported ipc type. [%s]\n", pszIpc);
		HxLOG_Assert(0);
	}

	return 0;
}

HINT32	HLIB_EXTRA_OpenClient(const HCHAR *pszServerName)
{
	HxJSON_t	extraInfo = hlib_extra_GetExtraInfo();
	HxJSON_t	objInfo;
	const HCHAR	*pszIpc, *pszName, *pszIpAddr;
	HINT32		nPort;

	objInfo = HxJSON_Object_Get(extraInfo, pszServerName);
	if (objInfo == NULL)
	{
		HxLOG_Critical("Can't find object named [%s]\n", pszServerName);
		HxLOG_Assert(0);
	}

	pszIpc = HLIB_JSON_Object_GetString(objInfo, "ipc");
	if (HxSTD_StrCmp(pszIpc, "uds") == 0)
	{
		pszName = HLIB_JSON_Object_GetString(objInfo, "name");
		return HLIB_SOCKET_OpenClientEx(HxSOCKET_UDS, NULL, (void*)pszName, 0);
	} else
	if (HxSTD_StrCmp(pszIpc, "inet") == 0)
	{
		pszIpAddr = HLIB_JSON_Object_GetString(objInfo, "addr");
		nPort     = HLIB_JSON_Object_GetInteger(objInfo, "port");
		return HLIB_SOCKET_OpenClientEx(HxSOCKET_INET, pszIpAddr, (void*)nPort, 0);
	} else
	{
		HxLOG_Critical("not supported ipc type. [%s]\n", pszIpc);
		HxLOG_Assert(0);
	}

	return 0;
}

const HCHAR*	HLIB_EXTRA_GetIpAddr(const HCHAR *pszServerName)
{
	HxJSON_t	extraInfo = hlib_extra_GetExtraInfo();
	HxJSON_t	objInfo;
	const HCHAR	*pszIpc;

	objInfo = HxJSON_Object_Get(extraInfo, pszServerName);
	if (objInfo == NULL)
	{
		HxLOG_Critical("Can't find object named [%s]\n", pszServerName);
		HxLOG_Assert(0);
	}

	pszIpc = HLIB_JSON_Object_GetString(objInfo, "ipc");
	if (HxSTD_StrCmp(pszIpc, "inet") == 0)
	{
		return HLIB_JSON_Object_GetString(objInfo, "addr");
	}

	return NULL;
}

HINT32			HLIB_EXTRA_GetPortNo(const HCHAR *pszServerName)
{
	HxJSON_t	extraInfo = hlib_extra_GetExtraInfo();
	HxJSON_t	objInfo;
	const HCHAR	*pszIpc;

	objInfo = HxJSON_Object_Get(extraInfo, pszServerName);
	if (objInfo == NULL)
	{
		HxLOG_Critical("Can't find object named [%s]\n", pszServerName);
		HxLOG_Assert(0);
	}

	pszIpc = HLIB_JSON_Object_GetString(objInfo, "ipc");
	if (HxSTD_StrCmp(pszIpc, "inet") == 0)
	{
		return HLIB_JSON_Object_GetInteger(objInfo, "port");
	}

	return 0;
}

const HCHAR*	HLIB_EXTRA_GetUDSName(const HCHAR *pszServerName)
{
	HxJSON_t	extraInfo = hlib_extra_GetExtraInfo();
	HxJSON_t	objInfo;
	const HCHAR	*pszIpc;

	objInfo = HxJSON_Object_Get(extraInfo, pszServerName);
	if (objInfo == NULL)
	{
		HxLOG_Critical("Can't find object named [%s]\n", pszServerName);
		HxLOG_Assert(0);
	}

	pszIpc = HLIB_JSON_Object_GetString(objInfo, "ipc");
	if (HxSTD_StrCmp(pszIpc, "uds") == 0)
	{
		return HLIB_JSON_Object_GetString(objInfo, "name");
	}

	return NULL;
}

typedef struct
{
	HxEXTRA_MEM_e	eType;
	HUINT8			*pucBuffer;	// HLIB_EXTRA_ReadSpecialMemory() 호출시 MemAlloc이 됨.
	HUINT32			ulAddress;
	HUINT32			ulSize;
}HxEXTRA_MEM_t;

static HxEXTRA_MEM_t s_astMemTable[HxEXTRA_MEM_Max] =
{
#if defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO)
	{ HxEXTRA_MEM_NaPd, NULL, 0x20000000, 1020 },
#endif
#if defined(CONFIG_PROD_HGS1000S)	// 추후 NAGRA PK address 보고 추가해야 함
	{ HxEXTRA_MEM_NaPd, NULL, 0x12000000, 1020 },
#endif
#if defined(CONFIG_PROD_HD9000I)
	{ HxEXTRA_MEM_NaPd, NULL, 0x00A00000, 1020 },
#endif
#if defined(CONFIG_PROD_HA7100S)
	{ HxEXTRA_MEM_NaPd, NULL, 0x00A00000, 1020 },
#endif
#if defined(WIN32)
	{ HxEXTRA_MEM_NaPd, NULL, 0x00000000, 1020 },
#endif
};

HERROR HLIB_EXTRA_ReadSpecialMemory(HxEXTRA_MEM_e eMemType)
{
#if defined(WIN32)
	//TODO::
	return ERR_OK;
#else
	int		mem_fd;
	int	ret=0;
	void	*mem_addr;

	if(eMemType >= HxEXTRA_MEM_Max)
	{
		HxLOG_Error("[%s:%d] Invalid memory type (%d)\n", eMemType);
		return ERR_FAIL;
	}

	s_astMemTable[eMemType].pucBuffer = (HUINT8 *)HLIB_MEM_Calloc(s_astMemTable[eMemType].ulSize);
	if(s_astMemTable[eMemType].pucBuffer == NULL)
	{
		HxLOG_Error("pucBuffer is NULL\n");
		return ERR_FAIL;
	}

	if((mem_fd = open("/dev/mem", O_RDONLY | O_SYNC)) == -1)
	{
		HxLOG_Error("/dev/mem open error.\n");
		return ERR_FAIL;
	}

	mem_addr = mmap(0, s_astMemTable[eMemType].ulSize, PROT_READ, MAP_SHARED, mem_fd, s_astMemTable[eMemType].ulAddress);
	if(mem_addr == MAP_FAILED)
		HxLOG_Error("mmap() error, %s(%d)\n", strerror(errno), errno);
	else if (mem_addr == NULL)
		HxLOG_Error("mem_addr NULL\n");
	else
		HxSTD_memcpy(s_astMemTable[eMemType].pucBuffer, (HUINT8 *)mem_addr, s_astMemTable[eMemType].ulSize);

	close(mem_fd);
	ret=munmap(mem_addr,s_astMemTable[eMemType].ulSize);
	if(ret < 0)
		HxLOG_Error("munmap() error, %s(%d)\n", strerror(errno), errno);

	return ERR_OK;
#endif
}

HERROR HLIB_EXTRA_GetSpecialMemory(HxEXTRA_MEM_e eMemType, HUINT8 **pucBuffer, HUINT32 *pulSize)
{
	if(eMemType >= HxEXTRA_MEM_Max)
	{
		HxLOG_Error("[%s:%d] Invalid memory type (%d)\n", eMemType);
		return ERR_FAIL;
	}

	if(pucBuffer == NULL || pulSize == NULL)
	{
		HxLOG_Error("[%s:%d] Parameter error.\n");
		return ERR_FAIL;
	}

	*pucBuffer	= s_astMemTable[eMemType].pucBuffer;
	*pulSize	= s_astMemTable[eMemType].ulSize;

	return ERR_OK;
}

HERROR HLIB_EXTRA_ReleaseSpecialMemory(HxEXTRA_MEM_e eMemType)
{
	if(eMemType >= HxEXTRA_MEM_Max)
	{
		HxLOG_Error("[%s:%d] Invalid memory type (%d)\n", eMemType);
		return ERR_FAIL;
	}

	if(s_astMemTable[eMemType].pucBuffer)
	{
		HLIB_MEM_Free(s_astMemTable[eMemType].pucBuffer);
		s_astMemTable[eMemType].pucBuffer = NULL;
	}

	return ERR_OK;
}

