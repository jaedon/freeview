/*************************************************************
 * @file		privilege.h
 * @date		
 * @author		
 * @brief		

 * http://www.humaxdigital.com
 *************************************************************/
#ifndef __UTILS_PRIVILEGE__
#define __UTILS_PRIVILEGE__


#define ___HEADER________________________________________________________________________________________

#define ___DEFINE________________________________________________________________________________________

#define ___TYPEDEF_______________________________________________________________________________________

typedef enum
{
	PRVLG_CAP_AUDIT_CONTROL,		// Control kernel auditing [ > 2.6.11]
	PRVLG_CAP_AUDIT_WRITE,		// Write records to kernel audit log [ > 2.6.11 ]
	PRVLG_CAP_CHOWN,				// Change file user and group
	PRVLG_CAP_DAC_OVERRIDE,		// Bypass file permission checks
	PRVLG_CAP_DAC_READ_SEARCH,	// Read all files and directories
	PRVLG_CAP_FOWNER,				// Perform operations on files owned by other users such as chmod, chattr or utime
	PRVLG_CAP_FSETID,				// Keep suid and guid permissions when modifying a file
	PRVLG_CAP_IPC_LOCK,			// Lock memory (mlock, mlockall, mmap, shmctl)
	PRVLG_CAP_IPC_OWNER,			// Bypass shared memory permission checks
	PRVLG_CAP_KILL,				// Send signals to any process
	PRVLG_CAP_LEASE,				// Establish leases on any file
	PRVLG_CAP_LINUX_IMMUTABLE,	// Make files immutable and remove ability append to file using chattr
	PRVLG_CAP_MAC_ADMIN,			// Perform MAC administration (for Smack LSM) [ > 2.6.25 ]
	PRVLG_CAP_MAC_OVERRIDE,		// Override MAC (for Smack LSM) [ > 2.6.25 ]
	PRVLG_CAP_MKNOD,				// Create files using mknod()
	PRVLG_CAP_NET_ADMIN,			// Perform network administration such as configuring interfaces or routing tables
	PRVLG_CAP_NET_BIND_SERVICE,	// Bind socket to privileged port (< 1024)
	PRVLG_CAP_NET_BROADCAST,		// Make socket broadcasts and listen to multicasts
	PRVLG_CAP_NET_RAW,			// Use RAW and PACKET sockets
	PRVLG_CAP_SETGID,				// Change to any Group ID
	PRVLG_CAP_SETFCAP,			// Set file capabilities [ > 2.6.24 ]
	PRVLG_CAP_SETPCAP,			// Set capabilities if file capabilities not present
	PRVLG_CAP_SETUID,				// Change to any User ID
	PRVLG_CAP_SYS_ADMIN,			// Perform privileged system administration operations such as mount, swapon, sethostname and keyctl
	PRVLG_CAP_SYS_BOOT,			// Can reboot and kexec_load
	PRVLG_CAP_SYS_CHROOT,			// Can chroot
	PRVLG_CAP_SYS_MODULE,			// Can load and unload kernel modules
	PRVLG_CAP_SYS_NICE,			// Change process priority levels and scheduling class (e.g. to real-time)
	PRVLG_CAP_SYS_PACCT,			// Can use acct
	PRVLG_CAP_SYS_TRACE,			// ptrace any process
	PRVLG_CAP_SYS_RAWIO,			// Perform I/O port operations and access /proc/kcore
	PRVLG_CAP_SYS_RESOURCE,		// Set or override resource limits
	PRVLG_CAP_SYS_TIME,			// Can set the system clock and real-time hardware clock
	PRVLG_CAP_SYS_TTY_CONFIG,		// Can hangup the current tty
	PRVLG_CAP_SYSLOG,
	PRVLG_CAP_WAKE_ALARM,
	PRVLG_CAP_BLOCK_SUSPEND,
} PRVLG_CAP_e;

typedef struct
{
	int nKernelCap;
	PRVLG_CAP_e	ePrvlgCap;
} PRVLG_CAP_T;

#define ___STATIC_VARIABLES______________________________________________________________________________

#define ___PRIVATE_FUNCTION_PROTOTYPE____________________________________________________________________

#define	___PRIVATE_API___________________________________________________________________________________

#define	___PUBLIC_API____________________________________________________________________________________

int PRVLG_DropRoot(const char *pszUserName);
int PRVLG_ChangeGroup(const char *pszUserName);
int PRVLG_LimitRootCapability(unsigned long num_caps, ...);
int PRVLG_Chown(const char *pszPathName, const char *pszUserName, const char *pszGroupName);
int PRVLG_GetId(const char *pszUserName, unsigned long *pUsrId, unsigned long *pnGrpId);

#endif
