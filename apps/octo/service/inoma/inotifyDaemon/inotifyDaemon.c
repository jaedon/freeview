/**
    @file     inotifyDaemon.c
    @brief

    Description:\n
    Module: 	inoma\n

    Copyright (c) 2014 HUMAX Co., Ltd.              \n
    All rights reserved.                            \n
*/

/*
 * (c) 2014 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
 */

/* The Descriptions of this */
/* We dont need to get all file list in directory.
 * And there are 4 file groups in directory.
 * 1. already existed files to watch
 *    : We just watch the directory.
 * 2. already existed files not to watch
 *    : We just keep the exception list.
 * 3. not existed files to watch, newly added in runtime
 *    : We just watch the directory.
 * 4. not existed files not to watch, newly added in runtime
 *    : We just keep the exception list.
 * So we keep watching and distigulish the event action or not.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <bits/errno.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/inotify.h>
#include <syslog.h>

/* for HxLog */
#include <hlib.h>

/* internal */
#include "event_queue.h"

#define _DEBUG_

#ifdef _DEBUG_
#define	CONLOG_NOTICE(fmt, ...)	\
	fprintf(stderr, "[inoma][%s] ", __PRETTY_FUNCTION__);	\
	fprintf(stderr ,fmt, ##__VA_ARGS__);	\
	fprintf(stderr , "\n");
#define	CONLOG_ERROR(fmt, ...)	\
	fprintf(stderr, "[inoma][ERROR][%s] ", __PRETTY_FUNCTION__);	\
	fprintf(stderr ,fmt, ##__VA_ARGS__);	\
	fprintf(stderr , "\n");
#define	CONLOG_CRITICAL(fmt, ...)	\
	fprintf(stderr, "[inoma][CRITICAL][%s] ", __PRETTY_FUNCTION__);	\
	fprintf(stderr ,fmt, ##__VA_ARGS__);	\
	fprintf(stderr , "\n");
#else
#define CONLOG_NOTICE(fmt, ...)
#define CONLOG_ERROR(fmt, ...)
#define CONLOG_CRITICAL(fmt, ...)
#endif

#define SYSLOG_NOTICE(fmt,...)	syslog(LOG_NOTICE, fmt, ##__VA_ARGS__); CONLOG_NOTICE(fmt, ##__VA_ARGS__);
#define SYSLOG_ERROR(fmt,...)	syslog(LOG_ERR, fmt, ##__VA_ARGS__); CONLOG_ERROR(fmt, ##__VA_ARGS__);
#define SYSLOG_CRITICAL(fmt,...)	syslog(LOG_CRIT, fmt, ##__VA_ARGS__); CONLOG_CRITICAL(fmt, ##__VA_ARGS__);

#define RUNNING_DIR	"/var/log"
#define LOCK_FILE	"inoma.lock"
#define LOG_FILE	"inoma.log"

#define	EXCEPTION_FILE	"inomaExceptions.txt"

#ifdef _DEBUG_
#define WATCH_DIRECTORY	"/home/inoma/testWatchDir"
#define BACKUP_DIRECTORY "/home/inoma/testBackupDir"
#else
#define WATCH_DIRECTORY	CONFIG_PRODUCT_DATA_DIR	/* CONFIG_PRODUCT_DATA_DIR */
#define BACKUP_DIRECTORY CONFIG_PRODUCT_BACKUPDATA_DIR /* CONFIG_PRODUCT_BACKUPDATA_DIR */
#endif

#define	WATCH_SUBDIR_MAX	10	/* limitations, the max depth of subdir */
#define	WATCH_DIR_MAX	512	/* limitations, the max of watch directory counts */
#define WATCH_EXCEPTION_MAX	1000 /* limitations, the max of exceptions per watch point */

typedef struct _option {
	int bDaemonize;
	char* watchDir;
	char* backupDir;
} cmd_option_t;

typedef struct _watchDirectory {
	int	wd;
	char path[PATH_MAX];	/* defined in limits.h */
	unsigned int exceptFileCnt;
	char** exceptFiles;
} watchDirectory_t;

static watchDirectory_t s_watchDirs[WATCH_DIR_MAX];

static unsigned int s_watchDirCnt = 0;
static int s_keepRunning = 0;

//static FILE* s_logFileFd = 0;
//static cmd_option_t	s_cmdOptions;

#ifndef errno
extern int errno;
#endif

int copy_file(char* source, char* target)
{
	int FDSource;
	int FDTarget;
	int err;
	char tmpBuf[1024];
	ssize_t readLen = 0;
	struct stat stat_buf;

	if(!source || !target)
	{
		HxLOG_Critical("parameter is null");
		return -1;
	}

	err = stat(source, &stat_buf);
	if(err < 0 || !stat_buf.st_size)
	{
		/* dont need to copy */
		HxLOG_Print("ssource %s is abnormal errno(%d)(%s), so need to be check, anyway do nothing now~.", source, errno, strerror(errno));
		return 0;
	}

	FDSource = open(source, O_RDONLY);
	FDTarget = open(target, O_WRONLY|O_CREAT|O_TRUNC, 0640);

	memset(tmpBuf, 0, sizeof(char)*1024);
	while((readLen=read(FDSource, tmpBuf, sizeof(char)*1024)))
	{
		write(FDTarget, tmpBuf, readLen);
		memset(tmpBuf, 0, sizeof(char)*1024);
	}

	close(FDSource);
	close(FDTarget);

	return 0;
}

void do_something(struct inotify_event *inotiEvt)
{
	char sourceFilePath[PATH_MAX];
	char targetFilePath[PATH_MAX];
	unsigned int idx;
	unsigned int foundedIdx= 0xFFFF;
	char* lastPath = NULL;
	pid_t pid;

	memset(sourceFilePath, 0, sizeof(char)*PATH_MAX);
	memset(targetFilePath, 0, sizeof(char)*PATH_MAX);

	for(idx = 0; idx < s_watchDirCnt; idx++)
	{
		/* already this is passed through the exception filter */
		HxLOG_Print("(%d) %d %s trying to match wiht WD=%d", idx, s_watchDirs[idx].wd, s_watchDirs[idx].path, inotiEvt->wd);
		if(s_watchDirs[idx].wd == inotiEvt->wd)
		{
			HxLOG_Print("find the WD %d directory %s", s_watchDirs[idx].wd, s_watchDirs[idx].path);
			foundedIdx = idx;
			break;
		}
	}

	if(foundedIdx == 0xFFFF)
		/* something wrong */
		return;

	strncpy(sourceFilePath, s_watchDirs[foundedIdx].path, strlen(s_watchDirs[foundedIdx].path));
	strncat(sourceFilePath, "/", 1);
	strncat(sourceFilePath, inotiEvt->name, strlen(inotiEvt->name));

	lastPath = strstr(s_watchDirs[foundedIdx].path, WATCH_DIRECTORY);
	if(!lastPath)
	{
		/* somethin wrong */
		HxLOG_Warning("input parameter is null");
		return ;
	}

	lastPath = s_watchDirs[foundedIdx].path+sizeof(WATCH_DIRECTORY);

	strncpy(targetFilePath, BACKUP_DIRECTORY, sizeof(BACKUP_DIRECTORY));
	strncat(targetFilePath, "/", 1);
	strncat(targetFilePath, lastPath, strlen(lastPath));
	strncat(targetFilePath, "/", 1);
	strncat(targetFilePath, inotiEvt->name, inotiEvt->len);

	HxLOG_Print("Source(%s) will be copied to Target(%s)", sourceFilePath, targetFilePath);

	/* make child for dedicate job something */
	pid = fork();
	if(pid > 0)	/* parent */
		return;

	/* child, file copy */
	copy_file(sourceFilePath, targetFilePath);
	exit(0);
}

int match_exceptions(struct inotify_event* inotiEvt)
{
	unsigned int index;
	unsigned int foundIdx = 0xFFFF;
	unsigned int exceptionFileCount = 0;
	int matched = 0;
	char *ignoreExt[] = {"swp", "swx"};
	char** exceptionFiles = NULL;
	char* extension = NULL;

	if(!inotiEvt)
	{
		HxLOG_Warning("match_exceptions got null input parameter");
		return 0;
	}

	HxLOG_Print("event WD:%d, mask 0x%08x, len:%d name:%s",
		inotiEvt->wd, inotiEvt->mask, inotiEvt->len, inotiEvt->name);

	/* exception file */
	if(!strncmp(inotiEvt->name, EXCEPTION_FILE, sizeof(EXCEPTION_FILE)))
	{
		HxLOG_Print("matched! this is exception file");
		return 1;
	}

	/* ignore file type */
	extension = strrchr(inotiEvt->name, '.');
	if(extension)
	{
		extension+=1; /* skip the dot */
		HxLOG_Print("ignore file type(%s)", extension);
		for(index = 0;  index < sizeof(ignoreExt)/sizeof(char*); index++)
		{
			if(!strncasecmp(extension, ignoreExt[index], strlen(ignoreExt[index])))
			{
				HxLOG_Print("matched! extensions dont need to backup");
				return 1;
			}
		}
	}

	/* find the matched WD */
	for(index = 0; index < s_watchDirCnt; index++)
	{
		if(s_watchDirs[index].wd == inotiEvt->wd)
		{
			exceptionFiles = s_watchDirs[index].exceptFiles;
			exceptionFileCount = s_watchDirs[index].exceptFileCnt;
			foundIdx = index;
			matched = 1;
			HxLOG_Print("found matched watch %d", s_watchDirs[index].wd);
			break;
		}
	}

	if(!matched)
		return 0;

	/* there is no exceptions */
	if(!exceptionFiles)
		return 0;

	HxLOG_Print("find match WD(%d) %s", s_watchDirs[foundIdx].wd, s_watchDirs[foundIdx].path);

	for(index = 0; index < exceptionFileCount; index++)
	{
		/* exceptions */
		if(!strncmp(inotiEvt->name, exceptionFiles[index], strlen(exceptionFiles[index])))
		{
			HxLOG_Print("matched! this file is in list");
			return 1;
		}
	}

	return 0;
}

void handle_event(queue_entry_t event)
{
	/* If the event was associated with a filename, we will store it here */
	char *cur_event_filename = NULL;
	char *cur_event_file_or_dir = NULL;

	/* This is the watch descriptor the event occurred on */
	int cur_event_wd = event->inot_ev.wd;
	int cur_event_cookie = event->inot_ev.cookie;

	unsigned long flags;

	if( event->inot_ev.mask & IN_ISDIR )
	{
		cur_event_file_or_dir = "Dir";

		/* we only care the files in watch direcotries. */
		return;
	}
	else
	{
		cur_event_file_or_dir = "File";
	}

	if(event->inot_ev.len == 0)
	{
		/* we cannot check the event, there is no any information about evented file */
		return ;
	}

	cur_event_filename = event->inot_ev.name;
	if(match_exceptions(&event->inot_ev))
	{
		HxLOG_Print("Exception Matches: %s \"%s\" on WD #%i\n", cur_event_file_or_dir, cur_event_filename, cur_event_wd);

		/* there is no needed to watch the exceptions */
		return;
	}

	HxLOG_Print("handle event for %s", cur_event_filename);

	flags = event->inot_ev.mask & ~(IN_ALL_EVENTS | IN_UNMOUNT | IN_Q_OVERFLOW | IN_IGNORED );

	/* Perform event dependent handler routines */
	/* The mask is the magic that tells us what file operation occurred */
	switch (event->inot_ev.mask & (IN_ALL_EVENTS | IN_UNMOUNT | IN_Q_OVERFLOW | IN_IGNORED))
	{
	/* File was accessed */
	case IN_ACCESS:
		HxLOG_Print("ACCESS: %s \"%s\" on WD #%i\n", cur_event_file_or_dir, cur_event_filename, cur_event_wd);
		break;

	/* File was modified */
	case IN_MODIFY:
		HxLOG_Print("MODIFY: %s \"%s\" on WD #%i\n", cur_event_file_or_dir, cur_event_filename, cur_event_wd);
		break;

	/* File changed attributes */
	case IN_ATTRIB:
		HxLOG_Print("ATTRIB: %s \"%s\" on WD #%i\n", cur_event_file_or_dir, cur_event_filename, cur_event_wd);
		break;

	/* File open for writing was closed */
	case IN_CLOSE_WRITE:
		HxLOG_Print("CLOSE_WRITE: %s \"%s\" on WD #%i\n", cur_event_file_or_dir, cur_event_filename, cur_event_wd);
		do_something(&event->inot_ev);
		break;

	/* File open read-only was closed */
	case IN_CLOSE_NOWRITE:
		HxLOG_Print("CLOSE_NOWRITE: %s \"%s\" on WD #%i\n", cur_event_file_or_dir, cur_event_filename, cur_event_wd);
		break;

	/* File was opened */
	case IN_OPEN:
		HxLOG_Print("OPEN: %s \"%s\" on WD #%i\n", cur_event_file_or_dir, cur_event_filename, cur_event_wd);
		break;

	/* File was moved from X */
	case IN_MOVED_FROM:
		HxLOG_Print("MOVED_FROM: %s \"%s\" on WD #%i. Cookie=%d\n", cur_event_file_or_dir, cur_event_filename, cur_event_wd, cur_event_cookie);
		break;

	/* File was created */
	case IN_CREATE:
		HxLOG_Print("CREATE: %s \"%s\" on WD #%i\n", cur_event_file_or_dir, cur_event_filename, cur_event_wd);
		break;

	/* File was deleted */
	case IN_DELETE:
		HxLOG_Print("DELETE: %s \"%s\" on WD #%i\n", cur_event_file_or_dir, cur_event_filename, cur_event_wd);
		break;

	/* File was deleted */
	case IN_DELETE_SELF:
		HxLOG_Print("IN_DELETE_SELF: %s \"%s\" on WD #%i\n", cur_event_file_or_dir, cur_event_filename, cur_event_wd);
		break;

	/* Watch was removed explicitly by inotify_rm_watch or automatically
	because file was deleted, or file system was unmounted.  */
	case IN_IGNORED:
		s_watchDirCnt--;
		HxLOG_Print("IGNORED: WD #%d\n", cur_event_wd);
		HxLOG_Print("Watching = %d items\n",s_watchDirCnt);
		break;

	/* Some unknown message received */
	default:
		HxLOG_Print("UNKNOWN EVENT \"%X\" OCCURRED for file \"%s\" on WD #%i\n", event->inot_ev.mask, cur_event_filename, cur_event_wd);
		break;
	}

	/* If any flags were set other than IN_ISDIR, report the flags */
	if (flags & (~IN_ISDIR))
	{
		flags = event->inot_ev.mask;
		HxLOG_Print("Flags=%lX\n", flags);
	}
}

void handle_events(queue_t q)
{
	queue_entry_t event;
	while(!queue_empty(q))
	{
		event = queue_dequeue(q);
		handle_event(event);
		free(event);
	}
}

int read_events (queue_t q, int fd)
{
	char buffer[512*1024]; /* big, because it depends on how many files we should watch */
	size_t buffer_i;
	struct inotify_event *pevent;
	queue_entry_t event;
	ssize_t r;
	size_t event_size, q_event_size;
	int count = 0;

	r = read (fd, buffer, 512*1024);
	if(r <= 0)
		return r;

	buffer_i = 0;
	while(buffer_i < r)
	{
		/* Parse events and queue them. */
		pevent = (struct inotify_event *)&buffer[buffer_i];
		event_size = offsetof(struct inotify_event, name) + pevent->len;
		q_event_size = offsetof(struct queue_entry, inot_ev.name) + pevent->len;
		event = malloc (q_event_size);
		memmove(&(event->inot_ev), pevent, event_size);
		queue_enqueue(event, q);
		buffer_i += event_size;
		count++;
	}
	HxLOG_Warning("\n%d events queued\n", count);

	return count;
}

int check_events (int fd)
{
	fd_set rfds;
	FD_ZERO (&rfds);
	FD_SET (fd, &rfds);

	/* Wait until an event happens or we get interrupted
	by a signal that we catch */
	return select (FD_SETSIZE, &rfds, NULL, NULL, NULL);
}

int process_events(queue_t q, int inotiFd)
{
	while(s_keepRunning && (s_watchDirCnt > 0))
	{
		/* inotify device is just one */
		if(check_events(inotiFd) > 0)
		{
			int r;
			r = read_events(q, inotiFd);
			if(r < 0)
			{
				break;
			}
			else
			{
				handle_events(q);
			}
		}
	}

	return 0;
}

void check_backup_directory(char* watchSubDir)
{
	int err;
	char backupPath[PATH_MAX];
	char* lastPath = NULL;
	struct stat stat_buf;

	if(!watchSubDir)
	{
		HxLOG_Critical("input parameter is null");
		return ;
	}

	/* exchange to the backup path, should be not null */
	lastPath = strstr(watchSubDir, WATCH_DIRECTORY);
	if(!lastPath)
	{
		/* somethin wrong */
		HxLOG_Critical("input parameter is null");
		return ;
	}

	/* make backup path */
	lastPath = watchSubDir+sizeof(WATCH_DIRECTORY);
	memset(backupPath, 0, sizeof(char)*PATH_MAX);
	strncpy(backupPath, BACKUP_DIRECTORY, sizeof(BACKUP_DIRECTORY));
	strncat(backupPath, "/", 1);
	strncat(backupPath, lastPath, strlen(lastPath));

	HxLOG_Print("backup path is %s", backupPath);

	err = stat(backupPath, &stat_buf);
	if(err >= 0 && S_ISDIR(stat_buf.st_mode))
	{
		/* dont need to mkdir */
		HxLOG_Print("%s is already there", backupPath);
		return ;
	}

	err = mkdir(backupPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	return ;
}


void read_exceptions(char* watchDir, int exceptEntryIdx)
{
	char	readBuf[PATH_MAX];
	unsigned int	readLen = 0;
	unsigned int	exceptionFileCount = 0, index;
	char	exceptionFilePath[PATH_MAX];
	char	tmpExceptionFiles[WATCH_EXCEPTION_MAX][PATH_MAX];
	FILE *exceptionFile;

	if(!watchDir)
	{
		/* very something wrong */
		HxLOG_Critical("There is no dir to watch");
		exit(-1);
	}

	/* exception file path */
	memset(exceptionFilePath, 0, sizeof(char)*PATH_MAX);
	strncpy(exceptionFilePath, watchDir, strlen(watchDir));
	strncat(exceptionFilePath, "/", 1);
	strncat(exceptionFilePath, EXCEPTION_FILE, sizeof(EXCEPTION_FILE));

	exceptionFile = fopen(exceptionFilePath, "r");
	if(!exceptionFile)
	{
		HxLOG_Warning("%s there is no exceptions", exceptionFilePath);
		return ;
	}

	/* tmp buffer */
	memset(tmpExceptionFiles, 0, sizeof(char)*WATCH_EXCEPTION_MAX*PATH_MAX);

	/* set and read file */
	memset(readBuf, 0, sizeof(char)*PATH_MAX);
	while(fgets(readBuf, sizeof(char)*PATH_MAX, exceptionFile))
	{
		readLen = strlen(readBuf);
		/* getting the exception file to not check, without whaitespace */
		strncpy(tmpExceptionFiles[exceptionFileCount], readBuf, readLen-1);
		exceptionFileCount++;

		HxLOG_Print("except to watch (%s)", (char*)readBuf);

		/* reset */
		memset(readBuf, 0, sizeof(char)*PATH_MAX);
	}

	fclose(exceptionFile);

	s_watchDirs[exceptEntryIdx].exceptFileCnt = exceptionFileCount;

	/* allocation runtime buffer */
	s_watchDirs[exceptEntryIdx].exceptFiles = (char**)malloc(sizeof(char*)*exceptionFileCount);
	if(!s_watchDirs[exceptEntryIdx].exceptFiles)
	{
		/* almost never happens */
		HxLOG_Critical("alloc memory failed for exception list");
		exit(-1);
	}

	/* copy */
	memset(s_watchDirs[exceptEntryIdx].exceptFiles, 0, sizeof(char*)*exceptionFileCount);
	for(index = 0; index < exceptionFileCount; index++)
	{
		s_watchDirs[exceptEntryIdx].exceptFiles[index] = (char*)malloc(sizeof(char)*strlen(tmpExceptionFiles[index])+1);
		memset(s_watchDirs[exceptEntryIdx].exceptFiles[index], 0, sizeof(char)*strlen(tmpExceptionFiles[index])+1);
		strncpy(s_watchDirs[exceptEntryIdx].exceptFiles[index], (const char*)tmpExceptionFiles[index], strlen(tmpExceptionFiles[index]));
		HxLOG_Print("add %d:%s exceptions", index, s_watchDirs[exceptEntryIdx].exceptFiles[index]);
	}
}


int watch_items(int fd, char* watchDir, unsigned long mask, unsigned int subDirDepth)
{
	int wd;
	unsigned int index;
	DIR* watchDirEntry;
	struct dirent* entry;

	/* the protection for recusive call */
	if(WATCH_SUBDIR_MAX < subDirDepth)
	{
		/* this is an error, but can be done for right rebavior */
		HxLOG_Error("Cannot watch the sub direcory no more");
		return -10;
	}

	/* check the limitation */
	if(WATCH_DIR_MAX < s_watchDirCnt)
	{
		HxLOG_Error("Cannot watch the item no more");
		return -10;
	}

	/* add watch point */
	wd = inotify_add_watch(fd, watchDir, mask);
	if (wd < 0)
	{
		HxLOG_Critical("Cannot add watch for \"%s\" with event mask %lX", watchDir, mask);
		fflush (stdout);
		perror (" ");

		return wd;
	}

	/* write the watch point */
	s_watchDirs[s_watchDirCnt].wd = wd;
	strncpy(s_watchDirs[s_watchDirCnt].path, watchDir, strlen(watchDir));
	HxLOG_Print("Watching %s WD=%d", s_watchDirs[s_watchDirCnt].path, s_watchDirs[s_watchDirCnt].wd);

	/* watch point is increased */
	s_watchDirCnt++;
	HxLOG_Print("Watching = %d items", s_watchDirCnt);

	/* the exception list */
	read_exceptions(watchDir, s_watchDirCnt-1/* to index */);

	/* find sub directories to watch */
	watchDirEntry = opendir(watchDir);
	if(!watchDirEntry)
	{
		HxLOG_Critical("Cannot open dirEntry watch for \"%s\"", watchDir);
		return wd;
	}

	/* go into subdirs */
	++subDirDepth;
	HxLOG_Print("find sub dirs (%d)depth\n", subDirDepth);

	/* add watch point recusively and need to mkdir in backup direcory */
	while((entry = readdir(watchDirEntry)) != NULL)
	{
		unsigned int exceptFileCnt = 0;
		int bFound = 0;
		struct stat dirEntryStat;
		char watchSubDir[PATH_MAX];

		HxLOG_Print("entry %s", entry->d_name);

		/* skip the references */
		if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
		{
			HxLOG_Print("skip %s, it is ref", entry->d_name);
			continue;
		}

		/* make absolute path */
		memset(watchSubDir, 0, sizeof(char)*PATH_MAX);
		strncpy(watchSubDir, watchDir, strlen(watchDir));
		strncat(watchSubDir, "/", 1);
		strncat(watchSubDir, entry->d_name, strlen(entry->d_name));

		/* if we found the directory to watch, call self */
		lstat(watchSubDir, &dirEntryStat);
		if(S_ISDIR(dirEntryStat.st_mode))
		{
			HxLOG_Print("check entry %s directory to watch", entry->d_name);

			/* compare the name with the exceptions */
			exceptFileCnt = s_watchDirs[s_watchDirCnt-1].exceptFileCnt;
			for(index = 0; index < exceptFileCnt; index++)
			{
				char * tmpStr = NULL;
				tmpStr = s_watchDirs[s_watchDirCnt-1].exceptFiles[index];
				if(!strncmp(tmpStr, entry->d_name, strlen(entry->d_name)))
				{
					/* this entry should be excepted to watch */
					bFound = 1;
				}
			}

			if(bFound)
			{
				continue;
			}

			/* firstly, to backup the files in that sub directory, we should mkdir that sub dir */
			check_backup_directory(watchSubDir);
			wd = watch_items(fd, watchSubDir, mask, subDirDepth);
			if(wd < 0)
			{
				if(wd == -10)
				{
					HxLOG_Warning("Met the limitations of watching");
				}
				else
				{
					HxLOG_Critical("Cannot add watch for \"%s\" with event mask %lX", watchDir, mask);
					fflush (stdout);
					perror (" ");
				}
			}
//			SYSLOG_NOTICE("\n");
		}
	}

	closedir(watchDirEntry);

	return wd;
}

void terminate_watching(void)
{
	unsigned int	idx = 0;
	unsigned int	jdx = 0;

	for(jdx = 0; jdx < s_watchDirCnt; jdx++)
	{

		if(s_watchDirs[jdx].exceptFiles)
		{

			for(idx = 0; idx < s_watchDirs[jdx].exceptFileCnt; idx++)
			{
				if(s_watchDirs[jdx].exceptFiles[idx])
					free(s_watchDirs[jdx].exceptFiles[idx]);
			}

			free(s_watchDirs[jdx].exceptFiles);
		}
	}
}

int watching(void)
{
	/* This is the file descriptor for the inotify watch */
	int inotify_fd;

	/* This is the watch descriptor returned for each item we are
		watching. A real application might keep these for some use
		in the application. This sample only makes sure that none of
		the watch descriptors is less than 0.
	*/
	int watchDescriptor = 0;
	queue_t q;

	HxLOG_Print("start watching");

	/* initialize inotify */
	inotify_fd = inotify_init();
	if(inotify_fd < 0)
	{
		HxLOG_Critical("cannot use inotify()");
		perror("inotify_init () = ");
		return -1;
	}

	/* initialize event queue */
	q = queue_create();

	/* it makes process termination smooth */
	s_keepRunning = 1;

	/* First we open the inotify dev entry */
	s_watchDirCnt = 0;

	/* initialize the exceptioin list */
	memset(&s_watchDirs, 0, sizeof(watchDirectory_t)*WATCH_DIR_MAX);

	/* add directories to watch events, and sub directories are added by recusive call */
	watchDescriptor = watch_items(inotify_fd, WATCH_DIRECTORY, IN_ALL_EVENTS, 0);
	if(watchDescriptor < 0)
	{
		/* if there is any descriptor returned, there is no watching */
		HxLOG_Critical("watching cannot happen");
		return -1;
	}

	/* main process loop */
	process_events(q, inotify_fd);

	/* Finish up by closing the fd,
	 * destroying the queue,
	 * and returning a proper code
	*/
	inotify_rm_watch (inotify_fd, watchDescriptor);
	close(inotify_fd);
	queue_destroy (q);

	/* wrap up the memory */
	terminate_watching();

	HxLOG_Print("stop watching");

	return 0;
}

void signal_handler(int sig)
{
	switch(sig)
	{
	case SIGHUP:
		/* go through the normal program termination steps */
		s_keepRunning = 0;
		HxLOG_Warning("hangup signal catched");
		break;

	case SIGTERM:
		HxLOG_Warning("terminate signal catched");
		terminate_watching();
		exit(0);
		break;

	case SIGINT:
	case SIGKILL:
		s_keepRunning = 0;
		HxLOG_Warning("kill signal catched");
		break;

	default:
		/* to avoid warning from code convention tool */
		break;
	}
}

void check_daemonize(void)
{
	pid_t ppid;
	pid_t pid;
	pid_t pgid;
	pid_t sid;
	pid_t tpgid;

	ppid = getppid();
	pid = getpid();
	pgid = getpgid();
	sid = getsid();
	tpgid = tcgetpgrp(STDIN_FILENO);

	HxLOG_Print("|PPID\t|PID\t|PGID\t|SID\t|TPGID");
	HxLOG_Print("%d\t%d\t%d\t%d\t%d", ppid, pid, pgid, sid, tpgid);
}

void daemonize(void)
{
	int i, lfp;
	char str[10];

	HxLOG_Print("deamonize start");

	if(getppid() == 1)
	{
		HxLOG_Warning("there is trying to re-daemonize");
		return; /* already a daemon */
	}

	i = fork();
	if (i < 0)
	{
		HxLOG_Error("fork error");
		exit(1); /* fork error */
	}

	if (i > 0)
	{
		HxLOG_Print("parent exit");
		exit(0); /* parent exits */
	}

	HxLOG_Print("child start");

	/* child (daemon) continues */
	setsid(); /* obtain a new process group */

#if 0	/* original code */
	for (i = getdtablesize(); i >= 0; --i)
		close(i); /* close all descriptors */
	/* i is decresed, it's 0 */
	i = open("/dev/null",O_RDWR);
#ifndef _DEBUG_
	dup(i);
	dup(i); /* handle standart I/O */
#else
    i = open("/dev/stdout",O_WRONLY);
	i = open("/dev/stderr",O_WRONLY);
#endif
#else
	/* Why we do like this,
	 * becase this platform dont support /dev/std* deivces
	 * and should use HxLOG which use the stdout.
	 * So we do not close stdout and stderr only.
	 */
	for (i = getdtablesize(); i >= 3; --i)
		close(i); /* close all descriptors, except stdout, stderr */
	close(0); /* close stdin */
#endif

	umask(027); /* set newly created file permissions: 0750 */

#ifndef _DEBUG_
	/* and We dont need to change a working directory */
	chdir(RUNNING_DIR); /* change running directory */
#endif
	lfp = open(LOCK_FILE,O_RDWR|O_CREAT,0640);

	if (lfp < 0)
	{
		HxLOG_Error("Lock file open error");
		exit(1); /* can not open */
	}

	if (lockf(lfp, F_TLOCK, 0) < 0)
	{
		HxLOG_Error("lockf error");
		exit(0); /* can not lock */
	}

	/* first instance continues */
	sprintf(str, "%d\n", getpid());

	write(lfp, str, strlen(str)); /* record pid to lockfile */

	signal(SIGCHLD, SIG_IGN); /* ignore child */
	signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);

	/* we will overwrite the behavior of sig mask for debugging */
	signal(SIGHUP, signal_handler); /* catch hangup signal */
	signal(SIGTERM, signal_handler); /* catch kill signal */

	HxLOG_Print("darmonize done!");
}


/*	The constraints of this program, "watch the directory".
	1. This program is permitted only one copy can be run as a daemon.
	2. This program should know the list to watch, because it can be made an un-known file by system.
	And that auto-generated file is automatically backuped by this program.

	The limitations

	#define	WATCH_SUBDIR_MAX	10
	We can only watch the depth of sub-directory from the start point.

	#define	WATCH_DIR_MAX	512
	We can only watch the maximum counts of directory

	#define WATCH_EXCEPTION_MAX	1024
	We can only check the exceptions file 1024.

*/

int main(int argc, char** argv)
{
	/* Open the log file */
    openlog ("inoma", LOG_PID, LOG_DAEMON);
	HxLOG_Warning("# %s start#", argv[0]);

	/* to be daemon */
	daemonize();
	check_daemonize();

	/* watching */
	watching();

	HxLOG_Warning("# %s terminate#d", argv[0]);
	closelog();

	return 0;
}

/* EOF */

