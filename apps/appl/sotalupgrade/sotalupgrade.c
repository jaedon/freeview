/**
	@file     sotalupgrade.c
	@brief    sotalupgrade.c

	Description: \n
	Module: \n
	Remarks :\n

	Copyright (c) 2013 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <mntent.h>
#include <dirent.h>

#include <signal.h>

#include <vkernel.h>
#include <hlib.h>
#include <hapi.h>

#include <zlib.h>
#include <libgen.h>
#include "version.h"


#if 0
#undef	HxLOG_Print
#undef	HxLOG_Critical
#undef	HxLOG_Warning
#undef	HxLOG_Message
#undef	HxLOG_Info
#undef	HxLOG_Debug

#define HxLOG_Print		HxLOG_Error
#define HxLOG_Critical		HxLOG_Error
#define HxLOG_Warning		HxLOG_Error
#define HxLOG_Message		HxLOG_Error
#define HxLOG_Info		HxLOG_Error
#define HxLOG_Debug		HxLOG_Error
#endif
//#define USE_TEST_SERVER

#if !defined(USE_TEST_SERVER)
#define SOTAL_UPGRADE_SERVER_BASE_URL	"http://download.sotal.com/HD9000i"
#else
#define SOTAL_UPGRADE_SERVER_BASE_URL	"http://10.0.11.149/HD9000i"
#endif

#define SOTAL_UPGRADE_SERVER_USB_URL	"/media/drive1/webapp_upgrade.txt"			//http://192.168.90.15/HD9000i

#define SOTAL_UPGRADE_SERVER_DOWNLOAD_BASE_URL	SOTAL_UPGRADE_SERVER_BASE_URL

#define SOTAL_UPGRADE_DOWNLOAD_PATH	"/tmp"
#define SOTAL_UPGRADE_DIR_NAME		"upgrade"
#define SOTAL_UPGRADE_DIR		"/usr/browser/webapps/" SOTAL_UPGRADE_DIR_NAME
#if defined(CONFIG_DEBUG)
#define SOTAL_APPS_DIR_NAME		"apps_hmx"
#else
#define SOTAL_APPS_DIR_NAME		"apps"
#endif
#define SOTAL_APPS_DIR			"/usr/browser/webapps/" SOTAL_APPS_DIR_NAME
#define SOTAL_APPS_MOUNT_FILE	"/var/lib/humaxtv/appsmnt"
#define SOTAL_APPS_MOUNT_EXT		"ext "
#define SOTAL_APPS_MOUNT_EXT2	"ext2"
#define CURRENT_EXT_MOUNT_DIR_NAME	"/var/lib/humaxtv/curext"
#define HTTP_TIMEOUT				30	/* 30 seconds */

#define MMOUNT					"/proc/mounts"
//#define SIMULATE_DOWNLOAD

typedef enum
{
	eSotup_result = 0,
	eSotup_progress
} Sotup_Event_e;

typedef enum
{
	eSotupResult_Success = ERR_OK,			
	eSotupResult_Fail_ManifestReadFail,
	eSotupResult_Stop_AlreadyLatestVersion,
	eSotupResult_Stop_AlreadyUpdated_NeedReboot,
	eSotupResult_Fail_ManifestConvertFail,
	eSotupResult_Fail_NetworkProblem_1,
	eSotupResult_Fail_NetworkProblem_2,
	eSotupResult_Fail_NetworkProblem_3,
	eSotupResult_Fail_NetworkProblem_4,
	eSotupResult_Fail_NetworkProblem_5,
	eSotupResult_Fail_NetworkProblem_6,		//10
	eSotupResult_Fail_FileProblem_1,
	eSotupResult_Fail_FileProblem_2,
	eSotupResult_Fail_FileProblem_3,
	eSotupResult_Fail_FileProblem_4,
	eSotupResult_Fail_FileProblem_5,
	eSotupResult_Fail_FileProblem_6,
	eSotupResult_Fail_FileProblem_7,
	eSotupResult_Fail_FileProblem_8,
	eSotupResult_Fail_UserStopped			//user stopped
} Sotup_Result_e;


typedef struct {
	HxTRANSPORT_Http_t *pHttp;
	HUINT8 szCurManifestPathName[256];	/* /usr/browser/webapps/apps(_hmx)/MANIFEST_X.Y.Z */
	HUINT8 szTmpManifestPathName[256];	/* /tmp/MANIFEST_X.Y.Z */
	HUINT8 szNewManifestPathName[256];	/* /usr/browser/webapps/upgrade/MANIFEST_X.Y.Z */
	HUINT8 szRemoteManifestUrl[256];	/* http://download.sotal.com/HD9000i/MANIFEST_X.Y.Z */
	HFLOAT64	fDownloadSize;
	HBOOL		bStopNow;
	HBOOL		bRunning;
	HBOOL		bRunningByUser;
	HBOOL		bNewAppsUpgraded;
	unsigned long		ulUpgradeTaskId;
	HBOOL bStartDownloadManifest;
	HUINT32 cntTotalAppsObj;
	HUINT32 cntCopyAppObj;
	HUINT32 totalProgress;
	HBOOL bProgressDuplicate;
} SubUpgradeStatus_t;

SubUpgradeStatus_t s_Status;


#define TRASH_INTERVAL 50
#define MAX_TRUNCATE_SIZE 1000000


static void sotup_remove_file_slowly(const char *pathname, struct stat *pst)
{
	off_t file_size;
#if defined(CONFIG_DEBUG)
	HUINT32 prev_tick;
#endif

	file_size = pst->st_size;
	if (pst->st_nlink > 1 && S_ISREG(pst->st_mode)) {
		/* if it is hardlink, removing does not take time. */
#if defined(CONFIG_DEBUG)
		prev_tick = VK_TIMER_GetSystemTick();
#endif
		if (remove(pathname) != 0) {
			HxLOG_Error("cannot remove hardlink \"%s\": %s\n",
						pathname, strerror(errno));
		}
#if defined(CONFIG_DEBUG)
		HxLOG_Print("remove hardlink \"%s\" (size %lld) -> %lu msec\n", pathname, file_size, VK_TIMER_GetSystemTick() - prev_tick);
#endif
		return;
	}

	while (file_size > MAX_TRUNCATE_SIZE) {
#if defined(CONFIG_DEBUG)
		prev_tick = VK_TIMER_GetSystemTick();
#endif
		file_size -= MAX_TRUNCATE_SIZE;
		if (truncate(pathname, file_size) != 0) {
			HxLOG_Error("cannot truncate \"%s\" from %lld to %lld: %s\n",
						pathname, pst->st_size, file_size, strerror(errno));
		}
#if defined(CONFIG_DEBUG)
		HxLOG_Print("truncate \"%s\" to size %lld -> %lu msec\n", pathname, file_size, VK_TIMER_GetSystemTick() - prev_tick);
#endif
		VK_TASK_Sleep(TRASH_INTERVAL);
	}

#if defined(CONFIG_DEBUG)
		prev_tick = VK_TIMER_GetSystemTick();
#endif
	if (remove(pathname) != 0) {
		HxLOG_Error("cannot remove \"%s\": %s\n",
					pathname, strerror(errno));
	}
#if defined(CONFIG_DEBUG)
	HxLOG_Print("remove   \"%s\" (size %lld) -> %lu msec\n", pathname, file_size, VK_TIMER_GetSystemTick() - prev_tick);
#endif
}


#define MAX_PATHNAME	512


static void sotup_remove_recursively(const char *pathname, HBOOL bRemoveSelf)
{
	struct stat st;
	int pathname_len;
	char *fullpath;

	if (-1 == lstat(pathname, &st)) {
		HxLOG_Error("cannot stat \"%s\": %s\n", pathname, strerror(errno));
		return;
	}

	pathname_len = strlen(pathname);
	fullpath = malloc(pathname_len + MAX_PATHNAME + 2);
	if (fullpath == NULL) {
		HxLOG_Error("cannot alloc fullpath\n");
		return;
	}
	strncpy(fullpath, pathname, pathname_len);
	fullpath[pathname_len] = '/';

	if (S_ISDIR(st.st_mode)) {
		DIR *dp;
		struct dirent *dirent;

		dp = opendir(pathname);
		if (NULL == dp) {
			free(fullpath);
			return;
		}

		while ((dirent = readdir(dp)) != NULL) {
			if (dirent->d_name[0] == '.'
			 && ((dirent->d_name[1] == '\0')
			  || (dirent->d_name[1] == '.' && dirent->d_name[2] == '\0')))
				continue;

			strncpy(fullpath + pathname_len + 1, dirent->d_name, MAX_PATHNAME);
			fullpath[pathname_len + 1 + MAX_PATHNAME] = '\0';
			sotup_remove_recursively(fullpath, TRUE);
		}
		closedir(dp);
	}

	if (bRemoveSelf == TRUE) {
		sotup_remove_file_slowly(pathname, &st);
	}
	free(fullpath);
}

/* Function with behaviour like `mkdir -p'  */
int mkpath(const char *s, mode_t mode)
{
	char *q, *r = NULL, *path = NULL, *up = NULL;
	int rv;

	rv = -1;
	if (strcmp(s, ".") == 0 || strcmp(s, "/") == 0)
		return (0);

	if ((path = strdup(s)) == NULL)
		exit(1);

	if ((q = strdup(s)) == NULL)
		exit(1);

	if ((r = dirname(q)) == NULL)
		goto out;

	if ((up = strdup(r)) == NULL)
		exit(1);

	if ((mkpath(up, mode) == -1) && (errno != EEXIST))
		goto out;

	if ((mkdir(path, mode) == -1) && (errno != EEXIST))
		rv = -1;
	else
		rv = 0;

out:
	if (up != NULL)
		free(up);
	free(q);
	free(path);
	return (rv);
}

static HERROR sotup_mkdir(char *fname)
{
	char* p;
	HERROR 	hErr = ERR_FAIL;

	p = strdup(fname);
	p = dirname(p);

	if (mkpath(p, 0770) == 0)
	{
		hErr = ERR_OK;
	}

	free(p);
	return hErr;
}

/*---------------------------------------------------------------------
 * Prototype  : static int sotup_findNextMountPoint(void)
 * Returns    : 0 - error
 *              1 - EXT
 *              2 - EXT2
 * Parameters : none
 * Description: 1. Find ubi1:dbuserext
 *              2. check whether mount directory is apps(_hmx) or not
 *              3. if it is apps(_hmx), that means next mount point is ext2
 *              4. if it is upgrade, that means next mount point is ext
 *-------------------------------------------------------------------*/
static int sotup_findNextMountPoint(void)
{
	struct mntent	*tMntEnt;
	FILE			*fp;
	int				rv = 0;

	fp = setmntent("/proc/mounts","r");
	if (!fp)
	{
		HxLOG_Error("[%s:%d]cannot open /proc/mounts\n", __FUNCTION__, __LINE__);
		return (rv);
	}

	while(1)
	{
		tMntEnt = getmntent(fp);
		if (!tMntEnt)
		{
			HxLOG_Error("[%s:%d]getmntent fail !!!\n");
			break;
		}

		if (HxSTD_StrNCmp(tMntEnt->mnt_fsname, "ubi1:dbuserext", HxSTD_StrLen("ubi1:dbuserext")) == 0)
		{
			HxLOG_Info("[%s:%d]tMntEnt->mnt_fsname : %s\n", __FUNCTION__, __LINE__, tMntEnt->mnt_fsname);
			HxLOG_Info("[%s:%d]tMntEnt->mnt_dir : %s\n", __FUNCTION__, __LINE__, tMntEnt->mnt_dir);
			if (HxSTD_StrNCmp(tMntEnt->mnt_dir, SOTAL_APPS_DIR, HxSTD_StrLen(SOTAL_APPS_DIR)) == 0)
			{
				HxLOG_Info("[%s:%d]next mount point is ext2\n");
				rv = 2;
				break;
			}
			else
			{
				HxLOG_Info("[%s:%d]next mount point is ext1\n");
				rv = 1;
				break;
			}
		}
	}

	endmntent(fp);
	return (rv);
}
/* End of sotup_findNextMountPoint */

#define	MAX_IN_SIZE		16384
#define	MAX_OUT_SIZE	8192
static unsigned char inBuffer[MAX_IN_SIZE];
static unsigned char outBuffer[MAX_OUT_SIZE];

static void sotup_copyOneFile(FILE *fp, FILE *op, unsigned long csize)
{
	unsigned long rsize;

	while (csize > 0)
	{
		if(csize > MAX_IN_SIZE)
			rsize = MAX_IN_SIZE;
		else
			rsize = csize;
		rsize = fread(inBuffer, 1, rsize, fp);
		if (rsize <= 0) break;
		fwrite (inBuffer, 1, rsize, op);
		csize -= rsize;
	}
}

static int sotup_unZipOneFile(FILE *fp, FILE *op, unsigned long csize, unsigned long usize)
{
	z_stream stream;
	unsigned long	rsize;
	unsigned long	size;
	unsigned long bsize;
	int flush = Z_SYNC_FLUSH;
	int	flag=0;
	int err;

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;
	stream.next_in = (Bytef*)0;
	stream.avail_in = 0;
	stream.data_type = 0;
	stream.msg = (char *)"";

	inflateInit2(&stream, -MAX_WBITS);

	stream.total_in = 0;
	stream.next_out = NULL;
	stream.avail_out = 0;
	stream.total_out = 0;

	while (csize > 0) {
		if (csize > MAX_IN_SIZE)
			size = MAX_IN_SIZE;
		else
			size = csize;
		rsize = fread(inBuffer, 1, size, fp);
		if (rsize != size) {
			if (flag) {
				HxLOG_Error("%s(%d) cannot read for unzipping\n", __FUNCTION__, __LINE__);
				inflateEnd(&stream);
			}
			return -1;
		}
		csize -= rsize;

//		stream.next_in = (z_const Bytef *)inBuffer;
		stream.next_in = (Bytef *)inBuffer;
		stream.avail_in = rsize;

		while (stream.avail_in > 0 && usize > 0) {
			stream.next_out = (Bytef *)outBuffer;
			bsize = (usize > MAX_OUT_SIZE) ? MAX_OUT_SIZE : usize;
			stream.avail_out = bsize;
			err = inflate(&stream, flush);
			if (err == Z_STREAM_ERROR || err == Z_DATA_ERROR) {
				HxLOG_Error("%s(%d) error while unzipping\n", __FUNCTION__, __LINE__);
				return -1;
			}
			flag = 1;

			bsize -= stream.avail_out;
			fwrite(outBuffer, 1, bsize, op);

			usize -= bsize;

			if (err != Z_OK) {
				break;
			}
		}
		if (stream.avail_in > 0) {
			HxLOG_Error("%s(%d) suspicious finish", __FUNCTION__, __LINE__);
			inflateEnd(&stream);
			return -1;
		}
	}
	inflateEnd(&stream);

	return 0;
}

static int sotup_unzipDownloadedFile(const char *ifname, const char *dir)
{
	unsigned long hd;
	unsigned long csize;	/* compressed size */
	unsigned long usize;	/* uncompressed size */
	unsigned short flen;	/* filename length */
	unsigned short elen;	/* extra field length */
	unsigned long	crc32;
	unsigned short cmethod;
	FILE 	*fp;
	FILE	*op;
	size_t	rsize;
	char fname[MAX_PATHNAME];
	int				err = 0;
	unsigned char header[30];
	char 	szOutFileName[MAX_PATHNAME];

	HxLOG_Print("%s() extract file %s to %s\n", __FUNCTION__, ifname, dir);

	fp = fopen(ifname, "rb");
	if (fp == NULL) {
		HxLOG_Error("%s(%d) cannot open input file %s (%m)\n", __FUNCTION__, __LINE__, ifname);
		return eSotupResult_Fail_FileProblem_8;	//ERR_FAIL;
	}

	while(!feof(fp))
	{
		if(s_Status.bStopNow)
		{
			fclose(fp);
			return eSotupResult_Fail_UserStopped;	//ERR_FAIL;
		}
		rsize = fread(header, 30, 1, fp);
		if (rsize != 1) {
			HxLOG_Error("%s(%d) cannot read header of input file %s (%m)\n", __FUNCTION__, __LINE__, ifname);
			fclose(fp);
			return eSotupResult_Fail_FileProblem_1;	//ERR_FAIL;
		}

		hd = header[0]|(header[1]<<8)|(header[2]<<16)|(header[3]<<24);
		if (hd == 0x08074b50) {
			fseek(fp, 12, SEEK_CUR);
			continue;
		} else if (hd == 0x02014b50) {
			HxLOG_Print("%s() sucessfully finished\n", __FUNCTION__);
			break;
		} else if (hd != 0x04034b50) {
			HxLOG_Error("%s(%d) suspicious header: [%08x]\n", __FUNCTION__, __LINE__, (unsigned int)hd);
			fclose(fp);
			return eSotupResult_Fail_FileProblem_2;		//ERR_FAIL;
		}
		//ver = header[4] | (header[5]<<8);
		//flag = header[6] | (header[7]<<8);	/* general purpose flag */
		cmethod = header[8] | (header[9]<<8);
		//tm = header[10] | (header[11]<<8);	/* last time */
		//dt = header[12] | (header[13]<<8);	/* last date */
		crc32 = header[14] | (header[15]<<8) | (header[16]<<16) | (header[17]<<24);
		csize = header[18] | (header[19]<<8) | (header[20]<<16) | (header[21]<<24);
		usize = header[22] | (header[23]<<8) | (header[24]<<16) | (header[25]<<24);
		flen = header[26] | (header[27]<<8);
		elen = header[28] | (header[29]<<8);

		if (fread(fname, flen, 1, fp) != 1) {
			HxLOG_Error("%s(%d) cannot read filename in input file %s (%m)\n", __FUNCTION__, __LINE__, ifname);
			fclose(fp);
			return eSotupResult_Fail_FileProblem_3;	//ERR_FAIL;
		}
		fname[flen]=0x00;
		if (elen > 0) {
			fseek(fp, elen, SEEK_CUR);
		}

		if (usize > 0) {
			snprintf(szOutFileName, MAX_PATHNAME, "%s/%s", dir, fname);
			sotup_mkdir(szOutFileName);
			op = fopen (szOutFileName, "wb");
			if (op == NULL) {
				HxLOG_Error("%s(%d) cannot open file %s for write (%m)\n", __FUNCTION__, __LINE__, szOutFileName);
				fclose(fp);
				return eSotupResult_Fail_FileProblem_4;	//ERR_FAIL;
			} else {
				if (cmethod == 8) {
					err = sotup_unZipOneFile(fp, op, csize, usize);
					if (err) {
						HxLOG_Error("%s(%d) cannot unzip for file %s\n", __FUNCTION__, __LINE__, szOutFileName);
						fclose(op);
						fclose(fp);
						return eSotupResult_Fail_FileProblem_5;	//ERR_FAIL;
					} else {
						HxLOG_Print("%s() unzipped file %s\n", __FUNCTION__, szOutFileName);
					}
				} else if (cmethod==0 && csize==usize) {
					sotup_copyOneFile(fp, op, csize);
					HxLOG_Print("%s() copied file %s\n", __FUNCTION__, szOutFileName);
				} else {
					fseek(fp, csize, SEEK_CUR);
					HxLOG_Error("%s() skipped file %s\n", __FUNCTION__, szOutFileName);
				}

				fclose(op);
			}
		} else {
			fseek(fp, csize, SEEK_CUR);
		}
	}
	fclose(fp);

	return err;
}

STATIC void sotup_downloadCallback(HxTRANSPORT_e event, HINT32 param1, HINT32 param2)
{
	HxTRANSPORT_Http_t *pHttp = s_Status.pHttp;
	HINT32	nProgress, calcProgress;

	if (param1 != (HINT32)&s_Status) {
		/* insane. stop!!! */
		HBOOL bStop;
		HxLOG_Error("%s(%d) insane parameter\n", __FUNCTION__, __LINE__);
		HLIB_TRANSPORT_HttpDownloadStop(pHttp, &bStop);
		return;
	}

	if (s_Status.bStopNow) {
		HBOOL bStop;
		HxLOG_Warning("%s(%d) operation is stopped!!!\n", __FUNCTION__, __LINE__);
		HLIB_TRANSPORT_HttpDownloadStop(pHttp, &bStop);
		HAPI_BroadcastSignal("signal:sotupResult", 0, "ii", eSotup_result ,eSotupResult_Fail_UserStopped);
		return;
	}

	switch(event) {
	case eHxTRANSPORT_EVENT_PROGRESS_CHANGED:
		if (s_Status.fDownloadSize == 0) {
			HLIB_TRANSPORT_HttpHttpGetFileSize(pHttp, &(s_Status.fDownloadSize));
		}
		
		nProgress = 0;
		calcProgress = 0;
		if(s_Status.bStartDownloadManifest==FALSE)
		{
			if (s_Status.fDownloadSize) {
				nProgress = ((HFLOAT64)param2 * 100) / (HFLOAT64)s_Status.fDownloadSize;
				if(nProgress>1)
				{
					calcProgress = (nProgress/s_Status.cntTotalAppsObj)-1;
					if(calcProgress < 0)
					{
						calcProgress = 0;
					}
				}
				else
				{
					nProgress = 0;
				}
			}
			
				
			if(nProgress==100)
			{
				if(s_Status.bProgressDuplicate == FALSE)
				{
					s_Status.totalProgress += calcProgress;
					s_Status.bProgressDuplicate = TRUE;
						
					HxLOG_Print("%s() download progress(%d)\n", __FUNCTION__,nProgress);
					//HAPI_BroadcastSignal("signal:sotupResult", 0, "ii", eSotup_progress,nProgress);
					HAPI_BroadcastSignal("signal:sotupResult", 0, "ii", eSotup_progress,(s_Status.totalProgress));
				}				
			}
			else
			{
				s_Status.bProgressDuplicate = FALSE;
				
				HxLOG_Print("%s() download progress(%d)\n", __FUNCTION__,nProgress);
				//HAPI_BroadcastSignal("signal:sotupResult", 0, "ii", eSotup_progress,nProgress);
				HAPI_BroadcastSignal("signal:sotupResult", 0, "ii", eSotup_progress,(s_Status.totalProgress+calcProgress));
			}
			
			
			
		}
		else		//s_Status.bStartDownloadManifest==TRUE
		{
			s_Status.bStartDownloadManifest=FALSE;
			HxLOG_Print("%s() download manifest result progress not returned\n", __FUNCTION__);
		}
			
		break;
	case eHxTRANSPORT_EVENT_COMPLETED:
		HxLOG_Print("%s() eHxTRANSPORT_EVENT_COMPLETED\n", __FUNCTION__);
		break;
	case eHxTRANSPORT_EVENT_DOWNLOADSTOP:
		HxLOG_Print("%s() eHxTRANSPORT_EVENT_DOWNLOADSTOP\n", __FUNCTION__);
		break;
	case eHxTRANSPORT_EVENT_UNDEFINED:
		HxLOG_Print("%s() eHxTRANSPORT_EVENT_UNDEFINED\n", __FUNCTION__);
		break;
	default:
		HxLOG_Print("%s() event %d (0x%x)\n", __FUNCTION__, event, event);
		break;
	}
}

HERROR sotup_downloadManifest(HUINT32 ulCoreVersion)
{
	HERROR ret;
	HxTRANSPORT_Http_t *pHttp;
	HFLOAT64 ullDownloadSize = 0;

	HxLOG_Debug("%s(ulCoreVersion = 0x%x)\n", __FUNCTION__, ulCoreVersion);

#if defined(SIMULATE_DOWNLOAD)
	{
		char buf[256];
		sprintf(buf, "cp /test/MANIFEST_%d.%02d.%02d %s",
			ulCoreVersion / 0x10000,
			(ulCoreVersion / 0x100) % 0x100,
			ulCoreVersion % 0x100,
			s_Status.szTmpManifestPathName);
		HxLOG_Error("SIMULATE_DOWNLOAD: %s\n", buf);
		ret = system(buf);
		if (ret) {
			HxLOG_Error("%s(%d) failed: [%s]\n", __FUNCTION__, __LINE__, buf);
			return ERR_FAIL;
		}
	}
#else
	pHttp = HLIB_TRANSPORT_HttpOpen();
	if (pHttp == NULL) {
		HxLOG_Error("%s(%d) cannot get transport handle\n", __FUNCTION__, __LINE__);
		return eSotupResult_Fail_NetworkProblem_4;	//ERR_FAIL;
	}

	s_Status.pHttp = pHttp;
	HLIB_TRANSPORT_HttpSetListener(pHttp, sotup_downloadCallback, (void *)&s_Status);

	ret = HLIB_TRANSPORT_HttpSetTimeout(pHttp, HTTP_TIMEOUT);
	if (ret != ERR_OK) {
		HxLOG_Error("[%s:%d]HLIB_TRANSPORT_HttpSetTimeout ERROR\n", __FUNCTION__, __LINE__);
		return eSotupResult_Fail_NetworkProblem_5;	//ERR_FAIL;
	}

	s_Status.fDownloadSize = 0;
	HxLOG_Info("[%s:%d]Download Server Manifest URL : %s\n", __FUNCTION__, __LINE__, s_Status.szRemoteManifestUrl);
	ret = HLIB_TRANSPORT_HttpGetAndStore(pHttp, s_Status.szRemoteManifestUrl, s_Status.szTmpManifestPathName, &ullDownloadSize);
	if (ret != ERR_OK) {
		HLIB_TRANSPORT_HttpClose(pHttp);
		s_Status.pHttp = NULL;
		HxLOG_Error("[%s:%d]HLIB_TRANSPORT_HttpGetAndStore ERROR\n", __FUNCTION__, __LINE__);
		return eSotupResult_Fail_NetworkProblem_6;	//ERR_FAIL;
	}

	HLIB_TRANSPORT_HttpClose(pHttp);
	s_Status.pHttp = NULL;
#endif

	return ERR_OK;
}

HERROR sotup_upgradeWebApp(const char *szAppName, const char *szAppVersion, const char *szRemoteFileName, const char *szSha1Sum)
{
	HxTRANSPORT_Http_t *pHttp;
	HFLOAT64 ullDownloadSize = 0;
	HUINT8 szUrl[256], szDownloadPathName[256], szExtractPath[256];
	HERROR ret;

	static HCHAR	s_szURLName[128];
	FILE	*fUrlFile;
	fUrlFile = fopen(SOTAL_UPGRADE_SERVER_USB_URL, "r");

	if(fUrlFile)
	{	
		fgets(s_szURLName, 128, fUrlFile);
		s_szURLName[127] = 0;
		fclose(fUrlFile);
	}


	/* download */
	if (HxSTD_StrLen(s_szURLName) > 0)
	{
		snprintf(szUrl, sizeof(szUrl), "%s/%s", s_szURLName, szRemoteFileName);
		snprintf(szDownloadPathName, sizeof(szDownloadPathName), "%s/%s_%s.zip", SOTAL_UPGRADE_DOWNLOAD_PATH, szAppName, szAppVersion);
	}
	else
	{
		snprintf(szUrl, sizeof(szUrl), "%s/%s", SOTAL_UPGRADE_SERVER_DOWNLOAD_BASE_URL, szRemoteFileName);
		snprintf(szDownloadPathName, sizeof(szDownloadPathName), "%s/%s_%s.zip", SOTAL_UPGRADE_DOWNLOAD_PATH, szAppName, szAppVersion);
	}

	
	HxLOG_Debug("%s() download web app from [%s] to [%s]\n", __FUNCTION__, szUrl, szDownloadPathName);

#if defined(SIMULATE_DOWNLOAD)
	{
		char buf[256];
		sprintf(buf, "cp /test/%s %s", szRemoteFileName, szDownloadPathName);
		HxLOG_Error("SIMULATE_DOWNLOAD: %s\n", buf);
		ret = system(buf);
		if (ret) {
			HxLOG_Error("%s(%d) failed: [%s]\n", __FUNCTION__, __LINE__, buf);
			return ERR_FAIL;
		}
	}
#else
	pHttp = HLIB_TRANSPORT_HttpOpen();
	if (pHttp == NULL) {
		HxLOG_Error("%s(%d) cannot get transport handle\n", __FUNCTION__, __LINE__);
		return eSotupResult_Fail_NetworkProblem_1;		//ERR_FAIL;	//result 5-1 : fail _ network problem 1(http open)
	}

	s_Status.pHttp = pHttp;
	HLIB_TRANSPORT_HttpSetListener(s_Status.pHttp, sotup_downloadCallback, (void *)&s_Status);

	ret = HLIB_TRANSPORT_HttpSetTimeout(pHttp, HTTP_TIMEOUT);
	if (ret != ERR_OK) {
		return eSotupResult_Fail_NetworkProblem_2;	//ERR_FAIL;	//result 5-2 : fail _ network problem 2(set timer fail)
	}

	s_Status.fDownloadSize = 0;
	ret = HLIB_TRANSPORT_HttpGetAndStore(pHttp, szUrl, szDownloadPathName, &ullDownloadSize);
	if (ret != ERR_OK) {
		HxLOG_Error("%s(%d) cannot download web app from [%s] to [%s]\n", __FUNCTION__, __LINE__, szUrl, szDownloadPathName);
		HLIB_TRANSPORT_HttpClose(pHttp);
		s_Status.pHttp = NULL;
		return eSotupResult_Fail_NetworkProblem_3;		//ERR_FAIL;	//result 5-3 : fail _ network problem 3(set timer fail)
	}

	HLIB_TRANSPORT_HttpClose(pHttp);
	s_Status.pHttp = NULL;
#endif

	/* extract */
	snprintf(szExtractPath, sizeof(szExtractPath), "%s/%s", SOTAL_UPGRADE_DIR, szAppName);
	HxLOG_Debug("%s() extract web app from [%s] to [%s]\n", __FUNCTION__, szDownloadPathName, szExtractPath);
	if ((mkdir(szExtractPath, 0770) == -1) && (errno != EEXIST)) {
		HxLOG_Error("%s(%d) cannot make directory [%s]\n", __FUNCTION__, __LINE__, szExtractPath);
		return ERR_FAIL;
	}
	ret = sotup_unzipDownloadedFile(szDownloadPathName, szExtractPath);
	if (ret != ERR_OK) {
			//result 5-4 : fail _ unzip download file failed
		HxLOG_Error("%s(%d) cannot extract web app from [%s] to [%s], ret(%d)\n", __FUNCTION__, __LINE__, szDownloadPathName, szExtractPath, ret);
	}
	else
	{
		s_Status.totalProgress +=1;
		HAPI_BroadcastSignal("signal:sotupResult", 0, "ii", eSotup_progress,s_Status.totalProgress);
	}
	

	/* remove the downloaded file */
	HxLOG_Debug("%s() remove download file[%s]\n", __FUNCTION__, szDownloadPathName);
	remove(szDownloadPathName);

	return ret;	//if ret==ERR_FAIL === result 5-4 : fail _ unzip download file failed
}

HERROR sotup_processManifest(void)
{
	HCHAR		*pszData, *pszBakData;
	HUINT32		ulSize, ulBakSize;
	HxJSON_t	curManifestObj = NULL, newManifestObj = NULL;
	HxJSON_t	curAppsObj = NULL, newAppsObj = NULL;
	HxJSON_t	curAppInfoObj = NULL, newAppInfoObj = NULL;
	HxJSON_t	curAppVersionObj = NULL, newAppVersionObj = NULL;
	HxJSON_t	newAppFileObj = NULL, newAppSha1SumObj = NULL;
	void		*newIter = NULL;
	const char		*curAppVersionStr = NULL, *newAppVersionStr = NULL, *newAppFileStr = NULL, *newAppSha1SumStr = NULL;
	const char		*newAppNameStr = NULL;
	char		*pAppsMnt;
	HERROR		ret;
	HBOOL		bCopyIt;
	HBOOL		bNewAppsUpgraded = FALSE;
	int			rv = 0;

	/* read tmp manifest */
	pszData = NULL;
	ret = HLIB_FILE_ReadFile(s_Status.szTmpManifestPathName, (void**)&pszData, &ulSize);
	if (ret != ERR_OK || pszData == NULL) {
		HxLOG_Error("%s(%d) cannot read file %s (%m)\n", __FUNCTION__, __LINE__, s_Status.szTmpManifestPathName);
		// result 1:fail _ manifest read fail(did not download or file error)
		return eSotupResult_Fail_ManifestReadFail;	//ERR_FAIL;
	}

	/* read manifest in 'app_hmx' directory */
	pszBakData = NULL;
	ret = HLIB_FILE_ReadFile(s_Status.szCurManifestPathName, (void**)&pszBakData, &ulBakSize);
	if (ret == ERR_OK && pszBakData && ulBakSize != 0) {
		if (ulBakSize == ulSize && memcmp(pszData, pszBakData, ulSize) == 0) {
			HxLOG_Debug("%s(%d) same manifest file -> don't need to upgrade...(current version - server version same)\n", __FUNCTION__, __LINE__);
			/* manifest in 'upgrade' directory and manifest in tmp is same. We don't need to upgrade */
			HLIB_STD_MemFree(pszData);
			HLIB_STD_MemFree(pszBakData);
			return eSotupResult_Stop_AlreadyLatestVersion;		//ERR_OK;	//result 2 : don't need to update. current version is latest
		}
	}
	if (pszBakData) {
		HLIB_STD_MemFree(pszBakData);
	}


	/* read manifest in 'upgrade' directory */
	pszBakData = NULL;
	ret = HLIB_FILE_ReadFile(s_Status.szNewManifestPathName, (void**)&pszBakData, &ulBakSize);
	if (ret == ERR_OK && pszBakData && ulBakSize != 0) {
		if (ulBakSize == ulSize && memcmp(pszData, pszBakData, ulSize) == 0) {
			HxLOG_Debug("%s(%d) same manifest file -> don't need to upgrade...\n", __FUNCTION__, __LINE__);
			/* manifest in 'upgrade' directory and manifest in tmp is same. We don't need to upgrade */
			HLIB_STD_MemFree(pszData);
			HLIB_STD_MemFree(pszBakData);
			return eSotupResult_Stop_AlreadyUpdated_NeedReboot;	//ERR_OK;		//result 3 : don't need to update. already upgraded. need rebooting.
		}
	}
	if (pszBakData) {
		HLIB_STD_MemFree(pszBakData);
	}

	/* convert tmp manifest to json */
	newManifestObj = HLIB_JSON_Decode(pszData);
	HLIB_STD_MemFree(pszData);

	if (newManifestObj == NULL) {
		HxLOG_Error("%s(%d) cannot decode newManifestObj\n", __FUNCTION__, __LINE__);
		return eSotupResult_Fail_ManifestConvertFail;		//ERR_FAIL;	//result 4 : fail _ converting manifest faild.
	}
	newAppsObj = HLIB_JSON_Get(newManifestObj, "apps");

	/* read current manifest and convert to json */
	pszData = NULL;
	ret = HLIB_FILE_ReadFile(s_Status.szCurManifestPathName, (void**)&pszData, &ulSize);
	if (ret == ERR_OK && pszData != NULL) {
		curManifestObj = HLIB_JSON_Decode(pszData);
		if (curManifestObj == NULL) {
			HxLOG_Warning("%s(%d) cannot decode curManifestObj\n", __FUNCTION__, __LINE__);
		} else {
			curAppsObj = HLIB_JSON_Get(curManifestObj, "apps");
			if (curAppsObj == NULL) {
				HxLOG_Warning("%s(%d) curapps does not exist\n", __FUNCTION__, __LINE__);
			}
		}
	} else {
		HxLOG_Warning("%s(%d) cur MANIFEST file does not exist.\n", __FUNCTION__, __LINE__);
	}
	if (pszData) {
		HLIB_STD_MemFree(pszData);
	}

	/* clean 'upgrade' directory silently */
	HxLOG_Debug("%s() clean up directory %s\n", __FUNCTION__, SOTAL_UPGRADE_DIR);
	sotup_remove_recursively(SOTAL_UPGRADE_DIR, FALSE);

	/* for each app, check version then copy or download */
	s_Status.cntTotalAppsObj=0;
	s_Status.cntCopyAppObj=0;
	s_Status.totalProgress=0;
	
	for (newIter = HLIB_JSON_Object_Iterator(newAppsObj); newIter; newIter = HLIB_JSON_Object_NextIterator(newAppsObj, newIter))
	{
		s_Status.cntTotalAppsObj++;
		newAppNameStr = HLIB_JSON_Object_IteratorKey(newIter);
		newAppInfoObj = HLIB_JSON_Object_IteratorValue(newIter);
		newAppVersionObj = HLIB_JSON_Get(newAppInfoObj, "version");
		newAppVersionStr = HxJSON_String_Get(newAppVersionObj);

		if (curAppsObj) {
			curAppInfoObj = HLIB_JSON_Get(curAppsObj, (void *)newAppNameStr);
			if (curAppInfoObj) {
				curAppVersionObj = HLIB_JSON_Get(curAppInfoObj, "version");
				curAppVersionStr = HxJSON_String_Get(curAppVersionObj);
				if (strcmp(newAppVersionStr, curAppVersionStr) == 0) {
					/* same version: copy from apps(_hmx) to 'upgrade' directory */
					s_Status.cntCopyAppObj++;
					HxLOG_Info("%s() cur app [%s] has same version [%s]\n", __FUNCTION__, newAppNameStr, curAppVersionStr);
				} else {
					HxLOG_Info("%s() cur app [%s] has different version [%s]\n", __FUNCTION__, newAppNameStr, curAppVersionStr);
				}
			} else {
				HxLOG_Info("%s() cur app [%s] does not exist\n", __FUNCTION__, newAppNameStr);
			}
		}
	}
	
	for (newIter = HLIB_JSON_Object_Iterator(newAppsObj); newIter; newIter = HLIB_JSON_Object_NextIterator(newAppsObj, newIter)) {
		newAppNameStr = HLIB_JSON_Object_IteratorKey(newIter);
		newAppInfoObj = HLIB_JSON_Object_IteratorValue(newIter);
		newAppVersionObj = HLIB_JSON_Get(newAppInfoObj, "version");
		newAppVersionStr = HxJSON_String_Get(newAppVersionObj);

		HxLOG_Info("%s() checking new app [%s] version [%s]\n", __FUNCTION__, newAppNameStr, newAppVersionStr);

		/* check if the same version of this app already exists in apps(_hmx) */
		bCopyIt = FALSE;
		if (curAppsObj) {
			curAppInfoObj = HLIB_JSON_Get(curAppsObj, (void *)newAppNameStr);
			if (curAppInfoObj) {
				curAppVersionObj = HLIB_JSON_Get(curAppInfoObj, "version");
				curAppVersionStr = HxJSON_String_Get(curAppVersionObj);
				if (strcmp(newAppVersionStr, curAppVersionStr) == 0) {
					/* same version: copy from apps(_hmx) to 'upgrade' directory */
					bCopyIt = TRUE;
					HxLOG_Info("%s() cur app [%s] has same version [%s]\n", __FUNCTION__, newAppNameStr, curAppVersionStr);
				} else {
					HxLOG_Info("%s() cur app [%s] has different version [%s]\n", __FUNCTION__, newAppNameStr, curAppVersionStr);
				}
			} else {
				HxLOG_Info("%s() cur app [%s] does not exist\n", __FUNCTION__, newAppNameStr);
			}
		}

		if (bCopyIt == TRUE) {
			char buf[512];
			HxLOG_Critical("%s() TODO: cp -r SOTAL_APPS_DIR+newAppNameStr SOTAL_UPGRADE_DIR\n", __FUNCTION__);
			snprintf(buf, sizeof(buf), "cp -r %s/%s %s", SOTAL_APPS_DIR, newAppNameStr, SOTAL_UPGRADE_DIR);
			if (VK_SYSTEM_Command(buf) != 0) {
				HxLOG_Error("%s(%d) cannot copy app from [%s/%s] to [%s]... Try downloading...\n", SOTAL_APPS_DIR, newAppNameStr, SOTAL_UPGRADE_DIR);
				bCopyIt = FALSE;
			}
			else
			{
				s_Status.totalProgress +=(100/s_Status.cntTotalAppsObj);
				HAPI_BroadcastSignal("signal:sotupResult", 0, "ii", eSotup_progress,s_Status.totalProgress);
			}
		}
		if (bCopyIt == FALSE) {
			newAppFileObj = HLIB_JSON_Get(newAppInfoObj, "file");
			newAppFileStr = HxJSON_String_Get(newAppFileObj);
			newAppSha1SumObj = HLIB_JSON_Get(newAppInfoObj, "sha1sum");
			newAppSha1SumStr = HxJSON_String_Get(newAppSha1SumObj);
			ret = sotup_upgradeWebApp(newAppNameStr, newAppVersionStr, newAppFileStr, newAppSha1SumStr);/*****UPGRADE************/
			if (ret != ERR_OK) {
				HLIB_JSON_Delete(newManifestObj);
				if (curManifestObj) {
					HLIB_JSON_Delete(curManifestObj);
				}
				return ret;	//ERR_FAIL;	//result 5 : fail _ upgrade fail.(detail result comes from sotup_upgradeWebApp)
			} else {
				bNewAppsUpgraded = TRUE;
			}
		}
	}

	HLIB_JSON_Delete(newManifestObj);
	if (curManifestObj) {
		HLIB_JSON_Delete(curManifestObj);
	}

	/*
	 * switch next mount position.
	 */
#if 0
	pAppsMnt = SOTAL_APPS_MOUNT_EXT2;
	pszData = NULL;
	ret = HLIB_FILE_ReadFile(CURRENT_EXT_MOUNT_DIR_NAME, (void**)&pszData, &ulSize);
	if (ret == ERR_OK && pszData && strncmp(pszData, SOTAL_UPGRADE_DIR_NAME, HxSTD_StrLen(SOTAL_UPGRADE_DIR_NAME)) == 0) {
		/*
		 * if dbuserext partition is currently mounted on 'upgrade' directory,
		 * next mount point of dbuserext partition must be 'apps' or 'apps_hmx' directory.
		 */
		pAppsMnt = SOTAL_APPS_MOUNT_EXT;
	}
	if (pszData) {
		HLIB_STD_MemFree(pszData);
	}
#else
	rv = sotup_findNextMountPoint();

	if (rv == 1)
	{
		pAppsMnt = SOTAL_APPS_MOUNT_EXT;
	}
	else
	{
		pAppsMnt = SOTAL_APPS_MOUNT_EXT2;
	}
#endif

	HxLOG_Debug("%s() determine next mount partition: write \'%s\' to %s\n", __FUNCTION__, pAppsMnt, SOTAL_APPS_MOUNT_FILE);
	ret = HLIB_FILE_WriteFile(SOTAL_APPS_MOUNT_FILE, pAppsMnt, strlen(pAppsMnt));
	if (ret != ERR_OK) {
		//result 5 : fail _ upgrade fail.(detail result comes from sotup_upgradeWebApp)
		return eSotupResult_Fail_FileProblem_6;	//ERR_FAIL;
	}

	HxLOG_Debug("%s() copy %s to %s\n", __FUNCTION__, s_Status.szTmpManifestPathName, s_Status.szNewManifestPathName);
	ret = HLIB_FILE_Copy(s_Status.szTmpManifestPathName, s_Status.szNewManifestPathName);
	if (ret != ERR_OK) {
		HxLOG_Error("%s(%d) cannot copy %s to %s (%m)\n", __FUNCTION__, __LINE__, s_Status.szTmpManifestPathName, s_Status.szNewManifestPathName);
		return eSotupResult_Fail_FileProblem_7;	//ERR_FAIL;		//result 6 : fail _ upgrade fail.(file copy fail)
	}

	HxLOG_Debug("%s() clean up tmp manifest file %s\n", __FUNCTION__, s_Status.szTmpManifestPathName);
	ret = HLIB_FILE_Delete(s_Status.szTmpManifestPathName);
	if (ret != ERR_OK) {
		HxLOG_Error("%s(%d) cannot delete %s (%m)\n", __FUNCTION__, __LINE__, s_Status.szTmpManifestPathName);
	}

	s_Status.bNewAppsUpgraded = bNewAppsUpgraded;
	return eSotupResult_Success;	//ERR_OK;		//result   0: upgrade_success_done
}

void sotup_doUpgrade(void *arg)
{
	HERROR ret = ERR_FAIL;
	HUINT32 ulNewCoreVersion = (HUINT32)arg;


	static HCHAR	s_szURLName[128];
	FILE	*fUrlFile;
	fUrlFile = fopen(SOTAL_UPGRADE_SERVER_USB_URL, "r");

	if(fUrlFile)
	{	
		fgets(s_szURLName, 128, fUrlFile);
		s_szURLName[127] = 0;
		fclose(fUrlFile);
	}

	if (HxSTD_StrLen(s_szURLName) > 0)
	{
		snprintf(s_Status.szRemoteManifestUrl, sizeof(s_Status.szRemoteManifestUrl), "%s/MANIFEST_%d.%02d.%02d",
			s_szURLName,
			ulNewCoreVersion / 0x10000,
			(ulNewCoreVersion / 0x100) % 0x100,
			ulNewCoreVersion % 0x100);
	}
	else
	{
		snprintf(s_Status.szRemoteManifestUrl, sizeof(s_Status.szRemoteManifestUrl), "%s/MANIFEST_%d.%02d.%02d",
			SOTAL_UPGRADE_SERVER_BASE_URL,
			ulNewCoreVersion / 0x10000,
			(ulNewCoreVersion / 0x100) % 0x100,
			ulNewCoreVersion % 0x100);
	}

	
	/* prolog */
	/*
	snprintf(s_Status.szRemoteManifestUrl, sizeof(s_Status.szRemoteManifestUrl), "%s/MANIFEST_%d.%02d.%02d",
			SOTAL_UPGRADE_SERVER_BASE_URL,
			ulNewCoreVersion / 0x10000,
			(ulNewCoreVersion / 0x100) % 0x100,
			ulNewCoreVersion % 0x100);
	*/
	HxLOG_Debug("%s() szRemoteManifestUrl: %s\n", __FUNCTION__, s_Status.szRemoteManifestUrl);

	snprintf(s_Status.szTmpManifestPathName, sizeof(s_Status.szTmpManifestPathName), "%s/MANIFEST_%d.%02d.%02d",
			SOTAL_UPGRADE_DOWNLOAD_PATH,
			ulNewCoreVersion / 0x10000,
			(ulNewCoreVersion / 0x100) % 0x100,
			ulNewCoreVersion % 0x100);
	HxLOG_Debug("%s() szTmpManifestPathName: %s\n", __FUNCTION__, s_Status.szTmpManifestPathName);

	snprintf(s_Status.szNewManifestPathName, sizeof(s_Status.szNewManifestPathName), "%s/MANIFEST_%d.%02d.%02d",
			SOTAL_UPGRADE_DIR,
			ulNewCoreVersion / 0x10000,
			(ulNewCoreVersion / 0x100) % 0x100,
			ulNewCoreVersion % 0x100);
	HxLOG_Debug("%s() szNewManifestPathName: %s\n", __FUNCTION__, s_Status.szNewManifestPathName);

	snprintf(s_Status.szCurManifestPathName, sizeof(s_Status.szCurManifestPathName), "%s/MANIFEST_%d.%02d.%02d",
			SOTAL_APPS_DIR,
			SYSTEM_APP_VERSION / 0x10000,
			(SYSTEM_APP_VERSION / 0x100) % 0x100,
			SYSTEM_APP_VERSION % 0x100);
	HxLOG_Debug("%s() szCurManifestPathName: %s\n", __FUNCTION__, s_Status.szCurManifestPathName);

	/* main procedure */
	do {
		s_Status.bStartDownloadManifest = TRUE;
		ret = sotup_downloadManifest(ulNewCoreVersion);
		
		if (ret != ERR_OK) {
			s_Status.bStartDownloadManifest = FALSE;
			HxLOG_Error("%s(%d) cannot download manifest file\n", __FUNCTION__, __LINE__);
			break;
		}
		ret = sotup_processManifest();
		if (ret != ERR_OK) {
			HxLOG_Error("%s(%d) cannot process manifest file, ret(%d)\n", __FUNCTION__, __LINE__, ret);
			break;
		}
	} while (0);

	/* epilog */
	HxLOG_Debug("%s() finish\n", __FUNCTION__);

	if (s_Status.bRunningByUser == TRUE || s_Status.bNewAppsUpgraded) {
		/* report result */
		//HAPI_BroadcastSignal("signal:sotupResult", 0, "i", ret);
		if(ret == eSotupResult_Success)
		{
			HAPI_BroadcastSignal("signal:sotupResult", 0, "ii", eSotup_progress,100);
		}
		HAPI_BroadcastSignal("signal:sotupResult", 0, "ii", eSotup_result ,ret);
	}

	s_Status.bRunningByUser = FALSE;
	s_Status.bRunning = FALSE;
}


static void sotup_stop(void)
{
	if (s_Status.bRunning) {
		s_Status.bStopNow = TRUE;
		HxLOG_Debug("%s() stop currently running upgrade process.\n", __FUNCTION__);
		while (s_Status.bRunning == TRUE) {
			HxLOG_Debug("%s() waiting for sotup_doUpgrade thread to be stopped...\n", __FUNCTION__);
			VK_TASK_Sleep(100);
		}
	}
}


static HERROR	sotup_onStart(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HUINT32 ulNewCoreVersion = SYSTEM_APP_VERSION;

	if (nArgc == 1) {
		//ulNewCoreVersion = HxOBJECT_INT(apArgv[0]);
		HxLOG_Debug("%s() start checking for version %d.%02d.%02d\n", __FUNCTION__,
			ulNewCoreVersion / 0x10000,
			(ulNewCoreVersion / 0x100) % 0x100,
			ulNewCoreVersion % 0x100);
	} else {
		HxLOG_Debug("%s() start checking for default version %d.%02d.%02d\n", __FUNCTION__,
			ulNewCoreVersion / 0x10000,
			(ulNewCoreVersion / 0x100) % 0x100,
			ulNewCoreVersion % 0x100);
	}

	s_Status.bNewAppsUpgraded = FALSE;	/* preventing invalid report */

	sotup_stop();

	s_Status.bStopNow = FALSE;
	s_Status.bRunning = TRUE;
	s_Status.bRunningByUser = TRUE;
	s_Status.bNewAppsUpgraded = FALSE;
	s_Status.cntTotalAppsObj=0;
	s_Status.cntCopyAppObj=0;
	s_Status.totalProgress = 0;
	
	VK_TASK_Create(sotup_doUpgrade, VK_TASK_PRIORITY_MW_REF, SIZE_32K, "sotup_doUpgrade", (void *)ulNewCoreVersion, &s_Status.ulUpgradeTaskId, 0);
	VK_TASK_Start(s_Status.ulUpgradeTaskId);

	return ERR_OK;
}


static HERROR	sotup_onStop(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	sotup_stop();

	return ERR_OK;
}


int main(int argc, char *argv[])
{
	VK_Init();
	HLIB_DBG_Init();

	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,
	HLIB_EXTRA_DropRootPrivileges("sotalupgrade");

	HLIB_TRANSPORT_HttpInit();

	HAPI_ConnectSignal("signal:sotupStart", NULL, sotup_onStart);
	HAPI_ConnectSignal("signal:sotupStop", NULL, sotup_onStop);

	while (1)
	{
		VK_TASK_Sleep(100000);
	}

	return 0;
}
/*  comment about progress refresh 
각 모듈별 percentage = download percentage+unzip percentage
               download percentage = (100/cntTotalAppsObj)-1 
               unzip percentage = 1

해당 모듈 file download중에 eHxTRANSPORT_EVENT_PROGRESS_CHANGED를 받아서 percentage 갱신
               ((downloaded file size/total file size)*100/cntTotalAppsObj) -1

copy하는 모듈 - copy과정의 percentage를 알수 없어서 완료되면 한번만 값 갱신
               (100/cntTotalAppsObj) 

success시에 total progress값이 딱 100이 아닐수 있어서 success직전에 progress 100을 날려주게함.

bProgressDuplicate : file download시 eHxTRANSPORT_EVENT_PROGRESS_CHANGED가 100%일때 두번 오는 경우가 있어서 추가함
*/
