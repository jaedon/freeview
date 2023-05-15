
#ifndef	__HLIB_EXTRA_H__
#define	__HLIB_EXTRA_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	HxEXTRA_CAP_AUDIT_CONTROL,		// Control kernel auditing [ > 2.6.11]
	HxEXTRA_CAP_AUDIT_WRITE,		// Write records to kernel audit log [ > 2.6.11 ]
	HxEXTRA_CAP_CHOWN,				// Change file user and group
	HxEXTRA_CAP_DAC_OVERRIDE,		// Bypass file permission checks
	HxEXTRA_CAP_DAC_READ_SEARCH,	// Read all files and directories
	HxEXTRA_CAP_FOWNER,				// Perform operations on files owned by other users such as chmod, chattr or utime
	HxEXTRA_CAP_FSETID,				// Keep suid and guid permissions when modifying a file
	HxEXTRA_CAP_IPC_LOCK,			// Lock memory (mlock, mlockall, mmap, shmctl)
	HxEXTRA_CAP_IPC_OWNER,			// Bypass shared memory permission checks
	HxEXTRA_CAP_KILL,				// Send signals to any process
	HxEXTRA_CAP_LEASE,				// Establish leases on any file
	HxEXTRA_CAP_LINUX_IMMUTABLE,	// Make files immutable and remove ability append to file using chattr
	HxEXTRA_CAP_MAC_ADMIN,			// Perform MAC administration (for Smack LSM) [ > 2.6.25 ]
	HxEXTRA_CAP_MAC_OVERRIDE,		// Override MAC (for Smack LSM) [ > 2.6.25 ]
	HxEXTRA_CAP_MKNOD,				// Create files using mknod()
	HxEXTRA_CAP_NET_ADMIN,			// Perform network administration such as configuring interfaces or routing tables
	HxEXTRA_CAP_NET_BIND_SERVICE,	// Bind socket to privileged port (< 1024)
	HxEXTRA_CAP_NET_BROADCAST,		// Make socket broadcasts and listen to multicasts
	HxEXTRA_CAP_NET_RAW,			// Use RAW and PACKET sockets
	HxEXTRA_CAP_SETGID,				// Change to any Group ID
	HxEXTRA_CAP_SETFCAP,			// Set file capabilities [ > 2.6.24 ]
	HxEXTRA_CAP_SETPCAP,			// Set capabilities if file capabilities not present
	HxEXTRA_CAP_SETUID,				// Change to any User ID
	HxEXTRA_CAP_SYS_ADMIN,			// Perform privileged system administration operations such as mount, swapon, sethostname and keyctl
	HxEXTRA_CAP_SYS_BOOT,			// Can reboot and kexec_load
	HxEXTRA_CAP_SYS_CHROOT,			// Can chroot
	HxEXTRA_CAP_SYS_MODULE,			// Can load and unload kernel modules
	HxEXTRA_CAP_SYS_NICE,			// Change process priority levels and scheduling class (e.g. to real-time)
	HxEXTRA_CAP_SYS_PACCT,			// Can use acct
	HxEXTRA_CAP_SYS_TRACE,			// ptrace any process
	HxEXTRA_CAP_SYS_RAWIO,			// Perform I/O port operations and access /proc/kcore
	HxEXTRA_CAP_SYS_RESOURCE,		// Set or override resource limits
	HxEXTRA_CAP_SYS_TIME,			// Can set the system clock and real-time hardware clock
	HxEXTRA_CAP_SYS_TTY_CONFIG,		// Can hangup the current tty
} HxEXTRA_CAP_e;

typedef struct
{
	HCHAR	*pszUDSName;
	HUINT32	ulIPAddr;
	HUINT32	nPort;
} HxEXTRA_IPC_t;

typedef enum
{
	HxEXTRA_MEM_NaPd,		// NAGRA PAIRING DATA BLOCK
	//.....
	HxEXTRA_MEM_Max
}HxEXTRA_MEM_e;

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HERROR	HLIB_EXTRA_DropRootPrivileges(const HCHAR *pszUserName);
extern HERROR	HLIB_EXTRA_DropRootPrivilegesWithParam(HINT32 nUserId, HINT32 nGroupId);
extern HERROR	HLIB_EXTRA_GetUserAndGroupIdFromUserInfo(const HCHAR *pszUserName, HUINT32 *pulUid, HUINT32 *pulGid);
extern HERROR	HLIB_EXTRA_GetGroupIdFromGroupInfo(const HCHAR *pszGroupName, HUINT32 *pulGid);

extern HERROR	HLIB_EXTRA_LimitRootCapability(HINT32 nCap, ...);

extern HINT32	HLIB_EXTRA_OpenServer(const HCHAR *pszServerName);
extern HINT32	HLIB_EXTRA_OpenClient(const HCHAR *pszServerName);
const HCHAR*	HLIB_EXTRA_GetIpAddr(const HCHAR *pszServerName);
extern HINT32	HLIB_EXTRA_GetPortNo(const HCHAR *pszServerName);
const HCHAR*	HLIB_EXTRA_GetUDSName(const HCHAR *pszServerName);

extern HERROR	HLIB_EXTRA_AmIRoot(HBOOL *pbRoot);
extern HERROR	HLIB_EXTRA_SetUmask(HUINT32 ulMask);
extern HERROR	HLIB_EXTRA_Chown(const HCHAR *pszPathName, const HCHAR *pszUserName, const HCHAR *pszGroupName);
extern HERROR	HLIB_EXTRA_ChownWithParam(const HCHAR *pszPathName, uid_t uid, gid_t gid);
extern HERROR	HLIB_EXTRA_Chmod(const HCHAR *pszPathName, HUINT32 ulMode);
extern HERROR	HLIB_EXTRA_Chroot(const HCHAR *pszPathName);
extern HERROR	HLIB_EXTRA_ReadSpecialMemory(HxEXTRA_MEM_e eMemType);
extern HERROR	HLIB_EXTRA_GetSpecialMemory(HxEXTRA_MEM_e eMemType, HUINT8 **pucBuffer, HUINT32 *pulSize);
extern HERROR	HLIB_EXTRA_ReleaseSpecialMemory(HxEXTRA_MEM_e eMemType);

#endif /* !__HLIB_RINGBUF_H___ */

