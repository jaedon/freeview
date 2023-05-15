/*************************************************************
 * @file		privilege.c
 * @date		
 * @author		
 * @brief		

 * http://www.humaxdigital.com
 *************************************************************/

#define ___HEADER________________________________________________________________________________________

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

/* Privilege */
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/resource.h>

/* Capability */
#include <linux/capability.h>
#include <sys/capability.h>
#include <sys/prctl.h>

#include "privilege.h"

#define ___DEFINE________________________________________________________________________________________

#if defined(CONFIG_DEBUG)
#define PRINTF        printf
#else
#define PRINTF        while (0) ((int (*)(const char *, ...)) 0)
#endif

#define DONE(ret) {						\
		if(ret) {						\
			PRINTF("ERROR %s,%d: %s\n", __FUNCTION__, __LINE__, strerror(errno)); \
			goto done;				\
		}							\
	}

#define ___TYPEDEF_______________________________________________________________________________________

#define ___STATIC_VARIABLES______________________________________________________________________________

#define	CAPSIZE		(sizeof(s_snCapability) / sizeof(s_snCapability[0]))
static PRVLG_CAP_T	s_snCapability[] =
{
	{	CAP_AUDIT_CONTROL,		PRVLG_CAP_AUDIT_CONTROL    },	// Control kernel auditing [ > 2.6.11]
	{	CAP_AUDIT_WRITE,		PRVLG_CAP_AUDIT_WRITE      },	// Write records to kernel audit log [ > 2.6.11 ]
	{	CAP_CHOWN,				PRVLG_CAP_CHOWN            },	// Change file user and group
	{	CAP_DAC_OVERRIDE,		PRVLG_CAP_DAC_OVERRIDE     },	// Bypass file permission checks
	{	CAP_DAC_READ_SEARCH,	PRVLG_CAP_DAC_READ_SEARCH  },	// Read all files and directories
	{	CAP_FOWNER,				PRVLG_CAP_FOWNER           },	// Perform operations on files owned by other users such as chmod, chattr or utime
	{	CAP_FSETID,				PRVLG_CAP_FSETID           },	// Keep suid and guid permissions when modifying a file
	{	CAP_IPC_LOCK,			PRVLG_CAP_IPC_LOCK         },	// Lock memory (mlock, mlockall, mmap, shmctl)
	{	CAP_IPC_OWNER,			PRVLG_CAP_IPC_OWNER        },	// Bypass shared memory permission checks
	{	CAP_KILL,				PRVLG_CAP_KILL             },	// Send signals to any process
	{	CAP_LEASE,				PRVLG_CAP_LEASE            },	// Establish leases on any file
	{	CAP_LINUX_IMMUTABLE,	PRVLG_CAP_LINUX_IMMUTABLE  },	// Make files immutable and remove ability append to file using chattr
	{	CAP_MAC_ADMIN,			PRVLG_CAP_MAC_ADMIN        },	// Perform MAC administration (for Smack LSM) [ > 2.6.25 ]
	{	CAP_MAC_OVERRIDE,		PRVLG_CAP_MAC_OVERRIDE     },	// Override MAC (for Smack LSM) [ > 2.6.25 ]
	{	CAP_MKNOD,				PRVLG_CAP_MKNOD            },	// Create files using mknod()
	{	CAP_NET_ADMIN,			PRVLG_CAP_NET_ADMIN        },	// Perform network administration such as configuring interfaces or routing tables
	{	CAP_NET_BIND_SERVICE,	PRVLG_CAP_NET_BIND_SERVICE },	// Bind socket to privileged port (< 1024)
	{	CAP_NET_BROADCAST,		PRVLG_CAP_NET_BROADCAST    },	// Make socket broadcasts and listen to multicasts
	{	CAP_NET_RAW,			PRVLG_CAP_NET_RAW          },	// Use RAW and PACKET sockets
	{	CAP_SETGID,				PRVLG_CAP_SETGID           },	// Change to any Group ID
	{	CAP_SETFCAP,			PRVLG_CAP_SETFCAP          },	// Set file capabilities [ > 2.6.24 ]
	{	CAP_SETPCAP,			PRVLG_CAP_SETPCAP          },	// Set capabilities if file capabilities not present
	{	CAP_SETUID,				PRVLG_CAP_SETUID           },	// Change to any User ID
	{	CAP_SYS_ADMIN,			PRVLG_CAP_SYS_ADMIN        },	// Perform privileged system administration operations such as mount, swapon, sethostname and keyctl
	{	CAP_SYS_BOOT,			PRVLG_CAP_SYS_BOOT         },	// Can reboot and kexec_load
	{	CAP_SYS_CHROOT,			PRVLG_CAP_SYS_CHROOT       },	// Can chroot
	{	CAP_SYS_MODULE,			PRVLG_CAP_SYS_MODULE       },	// Can load and unload kernel modules
	{	CAP_SYS_NICE,			PRVLG_CAP_SYS_NICE         },	// Change process priority levels and scheduling class (e.g. to real-time)
	{	CAP_SYS_PACCT,			PRVLG_CAP_SYS_PACCT        },	// Can use acct
	{	CAP_SYS_PTRACE,			PRVLG_CAP_SYS_TRACE        },	// ptrace any process
	{	CAP_SYS_RAWIO,			PRVLG_CAP_SYS_RAWIO        },	// Perform I/O port operations and access /proc/kcore
	{	CAP_SYS_RESOURCE,		PRVLG_CAP_SYS_RESOURCE     },	// Set or override resource limits
	{	CAP_SYS_TIME,			PRVLG_CAP_SYS_TIME         },	// Can set the system clock and real-time hardware clock
	{	CAP_SYS_TTY_CONFIG,		PRVLG_CAP_SYS_TTY_CONFIG   },	// Can hangup the current tty
	{	CAP_SYSLOG,             PRVLG_CAP_SYSLOG},
	{	CAP_WAKE_ALARM,         PRVLG_CAP_WAKE_ALARM},
	{	CAP_BLOCK_SUSPEND,      PRVLG_CAP_BLOCK_SUSPEND}
};

#define ___PRIVATE_FUNCTION_PROTOTYPE____________________________________________________________________

#define	___PRIVATE_API___________________________________________________________________________________

static int privilege_p_ReadGroups (const char *pszUserName, gid_t gid, unsigned long *pulGrpCnt, gid_t **ppGroups)
{
	int rc = -1;
	int val;
	int ngroups = 0;
	gid_t *groups = NULL;

	*pulGrpCnt = 0;	

	val = getgrouplist(pszUserName, gid, NULL, &ngroups);
	DONE((ngroups <= 1));

	groups = (gid_t*)malloc(ngroups * sizeof (gid_t));
	DONE((!groups));

	val = getgrouplist(pszUserName, gid, groups, &ngroups);
	DONE((val < 0));

	*pulGrpCnt = ngroups;
	*ppGroups = groups;

	rc = 0;
done:
	return rc;
}

static int privilege_p_DropRootPrivilegesWithParam(unsigned long nUserId, unsigned long nGroupId)
{
	int rc = -1;
	uid_t real, eff, saved;
	uid_t uid = (uid_t)nUserId;
	gid_t gid = (gid_t)nGroupId;

	rc = setresgid(gid, gid, gid);
	DONE((rc!=0));

	rc = setresuid(uid, uid, uid);
	DONE((rc!=0));

	rc = getresuid(&real, &eff, &saved);
	DONE((rc!=0));

	DONE((real != uid || eff != uid || saved != uid));

	rc = getresgid(&real, &eff, &saved);
	DONE((rc!=0));

	DONE((real != gid || eff != gid || saved != gid));

	rc = 0;
done:
	return rc;
}

static int privilege_FindKernelCap(PRVLG_CAP_e eCap, cap_value_t *pnCap)
{
	int rc = -1;
	int	i;	

	DONE((!pnCap));

	for (i = 0; i < (int)CAPSIZE; i++)
	{
		if (eCap == s_snCapability[i].ePrvlgCap)
		{
			*pnCap = s_snCapability[i].nKernelCap;
			break;
		}
	}
	DONE((i==(int)CAPSIZE));

	rc = 0;
done:
	return rc;
}

#define	___PUBLIC_API____________________________________________________________________________________

int PRVLG_DropRoot(const char *pszUserName)
{
	int rc = -1;
	struct passwd *u_info;

	u_info = getpwnam(pszUserName);

	(void)PRVLG_ChangeGroup(pszUserName);
	
	rc = privilege_p_DropRootPrivilegesWithParam(u_info->pw_uid, u_info->pw_gid);
	DONE((rc<0));

	rc = 0;
done:
	PRINTF("[%s,%d] DONE(rc=%d)\n", __FUNCTION__, __LINE__, rc);
	return rc;
}

int PRVLG_ChangeGroup(const char *pszUserName)
{
	int rc = -1;
	unsigned long group_cnt;
	gid_t *gids = NULL;
	struct passwd *u_info;

	DONE((!pszUserName));
	
	u_info = getpwnam(pszUserName);
	DONE((!u_info));

	rc = privilege_p_ReadGroups(pszUserName, u_info->pw_gid, &group_cnt, &gids);
	DONE((rc<0));

	rc = setgroups(group_cnt, gids);
	DONE((rc<0));

	rc = 0;
done:
	if (gids) free(gids);

	PRINTF("[%s,%d] DONE(rc=%d)\n", __FUNCTION__, __LINE__, rc);
	return rc;
}

int PRVLG_LimitRootCapability(unsigned long num_caps, ...)
{
	int rc = -1;
	int	i = 0;
	cap_t caps;
	cap_value_t *cap_values = NULL;
	va_list ap;

	DONE((num_caps == 0));

	cap_values = malloc(num_caps * sizeof(cap_value_t));

	va_start(ap, num_caps);
	while (num_caps--) 	{
		cap_value_t	value;
		PRVLG_CAP_e eExtraCap;

		eExtraCap = va_arg(ap, PRVLG_CAP_e);
		if (privilege_FindKernelCap(eExtraCap, &value) == 0)
		{
			cap_values[i++] = value;
		}
	}
	va_end(ap);

	num_caps = i;

	caps = cap_init();
	DONE((!caps));

	rc = cap_clear(caps);
	DONE((rc<0));

	rc = cap_set_flag(caps, CAP_EFFECTIVE, num_caps, cap_values, CAP_SET);
	DONE((rc<0));

	rc = cap_set_flag(caps, CAP_INHERITABLE, num_caps, cap_values, CAP_SET);
	DONE((rc<0));

	rc = cap_set_flag(caps, CAP_PERMITTED, num_caps, cap_values, CAP_SET);
	DONE((rc<0));

	rc = cap_set_proc(caps);
	DONE((rc<0));

	/* Remain capabilities after an identify(UID) changing */
	rc = prctl(PR_SET_KEEPCAPS, 1L);
	DONE((rc!=0));

	rc = cap_free(caps);
	DONE((rc<0));

	rc = 0;
done:
	if (cap_values) free(cap_values);

	PRINTF("[%s,%d] DONE(rc=%d)\n", __FUNCTION__, __LINE__, rc);
	return rc;
}

int	PRVLG_Chown(const char *pszPathName, const char *pszUserName, const char *pszGroupName)
{
	int	rc = -1;
	struct passwd *u_info = NULL;
	struct group  *g_info = NULL;
	gid_t gid;
	uid_t uid;

	u_info = getpwnam(pszUserName);
	DONE((!u_info));

	uid = (uid_t)u_info->pw_uid;
	gid = (gid_t)u_info->pw_gid;

	g_info = getgrnam(pszGroupName);
	if(g_info)
	{
		gid = (gid_t)g_info->gr_gid;
	}

	rc = chown(pszPathName, uid, gid);
	DONE((rc < 0));

	rc = 0;
done:
	PRINTF("[%s,%d] DONE(rc=%d)\n", __FUNCTION__, __LINE__, rc);
	return rc;
}

int PRVLG_GetId(const char *pszUserName, unsigned long *pUsrId, unsigned long *pnGrpId)
{
	int	rc = -1;
	struct passwd *u_info = NULL;

	u_info = getpwnam(pszUserName);
	DONE((!u_info));

	*pUsrId = (uid_t)u_info->pw_uid;
	*pnGrpId = (gid_t)u_info->pw_gid;

	rc = 0;
done:
	PRINTF("[%s,%d] DONE(rc=%d)\n", __FUNCTION__, __LINE__, rc);
	return rc;
}
