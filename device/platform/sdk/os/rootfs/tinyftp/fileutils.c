/***************************************************************************
 *            fileutils.c
 *
 *  Copyright 2005 Dimitur Kirov
 *  dkirov@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <stdio.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <netdb.h>
#include <errno.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include "defines.h"
#include "cmdparser.h"
#include "fileutils.h"

#define	PRINTF		//printf




#if !defined(jhlee4) && defined(HUMAX_FTP)

static bool is_special_dir(const char *dir);



extern	cas 	cas_type;

#define	IR_SESSION_FILEEXT	".ses"


/**
 * filename에 해당하는 파일이 irdeto session 파일인지 여부
 * 
 * @param	filename	
 * @return	TRUE / FALSE
 */
bool	is_irdeto_session_file (const char *filename)
{
	size_t	len;
	char	*extension;
	
	if (cas_type != CAS_IRDETO || filename == NULL)
	{
		return FALSE;
	}

	if (is_special_dir(filename))
	{
		return FALSE;
	}

	extension = strrchr(filename, '.');
	if (extension == NULL)
	{
		return FALSE;
	}
	extension = strrchr(extension, '.');
	if (extension == NULL)
	{
		return FALSE;
	}

	len = strlen(IR_SESSION_FILEEXT);

	if (strncmp(extension, IR_SESSION_FILEEXT, len) != 0)
	{
		return FALSE;
	}

	return TRUE;
}


/**
 * path/filename에 해당되는 파일이 보호되어야 하는 파일인지 검사
 * 
 * @param	path		file path
 * @param	filename	file name
 * @return	TRUE / FALSE
 */
bool	is_copy_protected_contents(const char *path, const char *filename)
{
	int			i;
	size_t		len;
	size_t		extlen;
	struct stat	st;
	char 		fullname[MAXPATHLEN], *filenamefromfullname;
	char		*extension1, *extension2;

	if (cas_type != CAS_IRDETO || filename == NULL)
	{
		return FALSE;
	}
	
	if(is_special_dir(filename)) // . ..
	{
		PRINTF("[copy_protected_contents, FALSE]: special dir(%s)\n", filename);
		return FALSE;
	}

	extlen	= strlen(IR_SESSION_FILEEXT);

	if (path == NULL)
	{
		strcpy(fullname, filename);
	}
	else
	{
		sprintf(fullname, "%s/%s", path, filename);
	}

	if (stat(fullname, &st) != 0 || !S_ISREG(st.st_mode))	// file not exist
	{
		PRINTF("[copy_protected_contents, FALSE]: file not exist(%s)\n", fullname);
		return FALSE;
	}

	len = strlen(fullname);
	extension1 = strrchr(fullname, '.');
	if (extension1 == NULL)
	{
		PRINTF("[copy_protected_contents, FALSE]: there is no extension(%s)\n", fullname);
		return FALSE;
	}

	extension2 = extension1 - strlen(IR_SESSION_FILEEXT);
	if (extension2 < fullname)
		extension2 = extension1;

	if (extension2 == NULL)
	{
		PRINTF("[copy_protected_contents, FALSE]: there is no extension(%s)\n", fullname);
		return FALSE;
	}

	if (strncmp(extension2, IR_SESSION_FILEEXT, extlen) == 0)
		strcpy(extension2, IR_SESSION_FILEEXT);
	else
		strcpy(extension1, IR_SESSION_FILEEXT);

	if (path == NULL)
		filenamefromfullname = fullname;
	else
		filenamefromfullname = fullname + strlen(path) + 1;
	PRINTF("--> %s\n", filenamefromfullname);

	len = strlen (filenamefromfullname);
#if	1
	{
		DIR	*DIR_P;
		struct	dirent	*dir_entry;
		int	temp = 0;

		if ((DIR_P = opendir(path)) == NULL)
		{
			PRINTF("[copy_protected_contents, FALSE]: there is no extension(%s)\n", fullname);
			return FALSE;
		}

		while ((dir_entry = readdir(DIR_P)) != NULL)
		{
			if (strncmp(dir_entry->d_name, filenamefromfullname, len) == 0)
			{
				PRINTF("[copy_protected_contents, TRUE]: %s\n", fullname);
				return TRUE;
			}
		}

		closedir(DIR_P);

		return FALSE;
	}
#else
	if (stat(fullname, &st) != 0 || !S_ISREG(st.st_mode))	// file not exist
	{
		PRINTF("[copy_protected_contents, FALSE]: file not exist(ses, %s)\n", fullname);
		return FALSE;
	}
#endif

	PRINTF("[copy_protected_contents, TRUE]: %s\n", fullname);
	return TRUE;
}
#endif



/**
 * Check if directory exists. If it exists return an open descriptor to that directory.
 * If it is not a directory, or does not exists return NULL.
 */
static DIR * ensure_dir_exists(int sock,const char*path) {
PRINTF("ensure_dir_exists : [%s] \n", path);
	DIR *dir =opendir(path) ;
	if(dir==NULL) {
		PRINTF("Error openning directory \"%s\", error was:\n  ",path);
		//send_repl(sock,REPL_550);
		switch(errno) {
			case EACCES:
				PRINTF("Access denied.\n");
				closedir(dir);
				return NULL;
			case EMFILE:
				PRINTF("Too many file descriptors in use by process.\n");
				closedir(dir);
				return NULL;
			case ENFILE:
				PRINTF("Too many files are currently open in the system.\n");
				closedir(dir);
				break;
			case ENOENT:
				PRINTF("Directory does not exist, or is an empty string.\n");
				closedir(dir);
				return NULL;
			case ENOMEM:
				PRINTF("Insufficient memory to complete the operation..\n");
				closedir(dir);
				return NULL;
			default:
			case ENOTDIR:
				PRINTF("\"%s\" is not a directory.\n",path);
				closedir(dir);
				return NULL;
		}
	}
	return dir;
}


/**
 * Writes the file statics in a formated string, given a pointer to the string.
 */
static int write_file(char *line,const char *mode,int num,const char *user,const char * group,long long size,const char *date,const char*fl_name) {
	sprintf(line,"%s %3d %-4s %-4s %lld %12s %s\r\n",mode,num,user,group, size,date,fl_name);

	//free(line);
	return 0;
}

/**
 * Check if the given directory is none of ".", or ".."
 */
static bool is_special_dir(const char *dir) {
	if(dir==NULL)
		return TRUE;
	int len = strlen(dir);
	if(len>2)
		return FALSE;
	if(dir[0]!='.')
		return FALSE;
	if(len==1)
		return TRUE;
	if(dir[1]=='.')
		return TRUE;
	return FALSE;
}

/**
 * Write file statics in a line using the buffer from stat(...) primitive
 */
static bool get_file_info_stat(const char *file_name, char *line,struct stat *s_buff){
	char date[16];
	char mode[11]	= "----------";
	line[0]='\0';
	struct passwd * pass_info = getpwuid(s_buff->st_uid);
	if(pass_info!=NULL) {
		struct group * group_info = getgrgid(s_buff->st_gid);
		if(group_info!=NULL) {
			int b_mask = s_buff->st_mode & S_IFMT;
			if(b_mask == S_IFDIR) {
				mode[0]='d';
			} else if(b_mask == S_IFREG){
				mode[0]='-';
			} else {
				return FALSE;
			}
			mode[1] = (s_buff->st_mode & S_IRUSR)?'r':'-';
			mode[2] = (s_buff->st_mode & S_IWUSR)?'w':'-';
			mode[3] = (s_buff->st_mode & S_IXUSR)?'x':'-';
			mode[4] = (s_buff->st_mode & S_IRGRP)?'r':'-';
			mode[5] = (s_buff->st_mode & S_IWGRP)?'w':'-';
			mode[6] = (s_buff->st_mode & S_IXGRP)?'x':'-';
			mode[7] = (s_buff->st_mode & S_IROTH)?'r':'-';
			mode[8] = (s_buff->st_mode & S_IWOTH)?'w':'-';
			mode[9] = (s_buff->st_mode & S_IXOTH)?'x':'-';
			strftime(date,13,"%b %d %H:%M",localtime(&(s_buff->st_mtime)));

			write_file(
				line,mode,s_buff->st_nlink,
				pass_info->pw_name,
				group_info->gr_name,
				s_buff->st_size,date,
				file_name
			);
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * Empty function , similara to python's "pass" operator.
 */
static void do_nothing() { if(TRUE==FALSE) do_nothing();}

/**
 * Writes the file statistics in a line "line"
 */
static bool get_file_info(const char *file_name, char *line){
	if(line==NULL)
		return FALSE;
	struct stat s_buff;
	if(is_special_dir(file_name))
		do_nothing();//return FALSE;
	int status = stat(file_name,&s_buff);

	if(status==0) {
		return get_file_info_stat(file_name,line,&s_buff);
	}
	return FALSE;
}

/**
 * Get and send the info for the current directory
 */
bool write_list(int sock, int client_sock, const char *current_dir) {
	
	if(client_sock>0) {
		if(sock!=client_sock) {
			send_repl(sock, 150, REPL_150);
		}
	}
	else {
		if(sock!=client_sock) {
			send_repl(sock, 425, REPL_425);
		}
		return FALSE;
	}
	DIR *dir = ensure_dir_exists(sock,current_dir);
	if(dir==NULL) {
		if(sock!=client_sock) {
			close(client_sock);
			send_repl(sock, 451, REPL_451);
		}
		return FALSE;
	}
	
	char line[MAXPATHLEN * 2];
	while(1) {
		struct dirent *d_next = readdir(dir);
		if(d_next==NULL)
			break;
		line[0]='\0';

#ifdef HUMAX_FTP
		if (cas_type == CAS_IRDETO && is_copy_protected_contents(current_dir, d_next->d_name))
		{
			continue;
		}
#endif
		if(get_file_info(d_next->d_name,line)) {
			if(send_repl_client(client_sock,line)) {
				if(sock!=client_sock)
					send_repl(sock, 451, REPL_451);
			}
		}
	}

	if(sock!=client_sock) {
		close(client_sock);
		send_repl(sock, 226, REPL_226);
		//free(line);
	}
	//free(line);
	closedir(dir);
	return TRUE;
}

bool	check_availibility(char *path)
{
	char	szPath[MAXPATHLEN];
	char	*pszPtr;

	if (path)
	{
		strcpy(szPath, path);
		pszPtr = strrchr(szPath, '/');
		if (pszPtr != NULL)
		{
			*pszPtr = 0;
	//PRINTF("szPath : [%s]\n", szPath);
			if (strcmp(szPath, "/media") == 0)
				return FALSE;

			return TRUE;
		}
	}

	if (getcwd(szPath, MAXPATHLEN) != NULL)
	{
		if (strcmp(szPath, "/media") == 0)
			return FALSE;

		return TRUE;
	}

	return FALSE;
}

/**
 * Create a directory, called "new_dir"
 */
bool make_dir(int sock,const char *new_dir,char *reply) {
	
	struct stat s_buff;

	if (check_availibility(NULL) == FALSE)
	{
		send_repl(sock, 550, REPL_550);
		return FALSE;
	}

	int status = stat(new_dir,&s_buff);
	if(status==0) {
		send_repl(sock, 550, REPL_550);
		return FALSE;
	}
	status = mkdir(new_dir,0755);
	if(status!=0) {
		send_repl(sock, 550, REPL_550);
		return FALSE;
	}
	reply[0]='\0';
	int len = sprintf(reply,REPL_257,new_dir);
	reply[len] ='\0';
	send_repl_len(sock,257, reply,len);
	return TRUE;
}

/**
 * Delete the directory called "removed_dir"
 */
bool remove_dir(int sock,const char *removed_dir) 
{
	if (check_availibility(NULL) == FALSE)
	{
		send_repl(sock, 550, REPL_550);
		return FALSE;
	}

	if(is_special_dir(removed_dir)) {
		send_repl(sock, 550, REPL_550);
		return FALSE;
	} 
	struct stat s_buff;
	int status = stat(removed_dir,&s_buff);
	if(status!=0) {
		send_repl(sock, 550, REPL_550);
		return FALSE;
	} 
	int b_mask = s_buff.st_mode & S_IFMT;
	if(b_mask != S_IFDIR) {
		send_repl(sock, 550, REPL_550);
		return FALSE;
	}
	status = rmdir(removed_dir);
	if(status!=0) {
		send_repl(sock, 550, REPL_550);
		return FALSE;
	}
	send_repl(sock, 250, REPL_250);
	return TRUE;
}

/**
 * Rename a file or directory. If operation is not successfull - return FALSE
 */
bool rename_fr(int sock,const char *from,const char *to) {

	if (check_availibility(NULL) == FALSE)
	{
		send_repl(sock, 550, REPL_550);
		return FALSE;
	}

	if(is_special_dir(from)) 
	{
		send_repl(sock, 553, REPL_553);
		return FALSE;
	} 
	struct stat s_buff;
	int status = stat(from,&s_buff);
	if(status!=0) {
		send_repl(sock, 553, REPL_553);
		return FALSE;
	} 
	status = stat(to,&s_buff);
	if(status==0) {
		send_repl(sock, 553, REPL_553);
		return FALSE;
	} 
	int b_mask = s_buff.st_mode & S_IFMT;
	if(b_mask == S_IFDIR || b_mask == S_IFREG) {
		int status = rename(from,to);
		if(status!=0) {
			send_repl(sock, 553, REPL_553);
			return FALSE;
		}
	} else {
		send_repl(sock, 553, REPL_553);
		return FALSE;
	}
	send_repl(sock, 250, REPL_250);
	return TRUE;
}

/**
 * Delete a file, given its name
 */
bool delete_file(int sock,const char *delete_file) {

	struct stat s_buff;

printf("file delete in FTP : [%s]\n", delete_file);
	if (check_availibility(NULL) == FALSE)
	{
printf("[%d]\n", __LINE__);
		send_repl(sock, 550, REPL_550);
		return FALSE;
	}

	int status = stat(delete_file,&s_buff);
	if(status!=0) {
		printf("[%d]\n", __LINE__);
		send_repl(sock, 550, REPL_550);
		return FALSE;
	} 
	int b_mask = s_buff.st_mode & S_IFMT;
	if(b_mask != S_IFREG) {
		printf("[%d]\n", __LINE__);
		send_repl(sock, 550, REPL_550);
		return FALSE;
	}
	status = unlink(delete_file);
	if(status!=0) {
		printf("[%d]\n", __LINE__);
		send_repl(sock, 450, REPL_450);
		return FALSE;
	}
	printf("[%d]\n", __LINE__);
	send_repl(sock, 250, REPL_250);
	return TRUE;
}

/**
 * Show stats about a file. If the file is a directory show stats about its content.
 */
bool stat_file(int sock, const char *file_path,char *reply) {
	char line[300];
	struct stat s_buff;
	int status = stat(file_path,&s_buff);
	if(status==0) {
		reply[0]='\0';
		int len = sprintf(reply,REPL_211_STATUS,file_path);
		send_repl_len(sock, 211, reply,len);
		int b_mask = s_buff.st_mode & S_IFMT;
		if(b_mask == S_IFDIR) {
			if(getcwd(line,300)!=NULL) {	
				int status = chdir(file_path);
				if(status != 0) {
					send_repl(sock, 450, REPL_450);
					//free(line);
					return FALSE;
				}
				else {
					if(!write_list(sock, sock, file_path)) {
						send_repl(sock, 450, REPL_450);
						return FALSE;
					}
					int status = chdir(line);
					if(status!=0) {
						send_repl(sock, 450, REPL_450);
						//free(line);
						return FALSE;
					}
				}
			} else {
				send_repl(sock,450, REPL_450);
				//free(line);
				return FALSE;
					
			}
		} else if(b_mask == S_IFREG){
			if(get_file_info_stat(file_path,line,&s_buff)) {
				if(send_repl_client(sock,line)) {
					send_repl(sock, 450, REPL_450);
					//free(line);
					return FALSE;
				}
			}
		} 
		send_repl(sock, 211, REPL_211_END);
	}
	else {
		send_repl(sock, 450, REPL_450);
		return FALSE;
	}
	free(line);
	return TRUE;
}

#define	AUTHOR_DIR_NUM			4
char	*aszAuthorizedList[AUTHOR_DIR_NUM] =
{
	"/media",
	"/mnt/hd2/My Video",
	"/mnt/hd2/My Music",
	"/mnt/hd2/My Photo"
};

bool	check_authorizedFolder(char *currentDir)
{
	int		i, nFlag;
	char	*pszPtr;

//PRINTF("[%s] - [%s]\n", __FUNCTION__, currentDir);

	nFlag = 0;
	for (i = 0; i < AUTHOR_DIR_NUM; i++)
	{
		if ((pszPtr = strstr(currentDir, aszAuthorizedList[i])))
		{
			pszPtr += strlen(aszAuthorizedList[i]);
			if ((*pszPtr == '/') || (*pszPtr == 0))
			{
				nFlag = 1;
				break;
			}
		}
	}

	if (nFlag == 1)
		return TRUE;

	return FALSE;
}

/**
 * Change current working dir.
 */
bool change_dir(int sock,const char *parent_dir,char *current_dir,char *virtual_dir,char *data_buff) 
{
	char	szFullPath[MAXPATHLEN];

	//	Full Path. Check Validity
	if (data_buff[0] == '/')
	{
		if (check_authorizedFolder(data_buff) == FALSE)
		{
			strcpy(szFullPath, "/media");
			strcat(szFullPath, data_buff);
			data_buff = szFullPath;
		}
	}

PRINTF("change_dir : [%s], parent_dir : [%s], target [%s]\n", current_dir, parent_dir, data_buff);
	DIR *dir = ensure_dir_exists(sock,data_buff);
	if(dir!=NULL) 
	{
		closedir(dir);

		int status = chdir(current_dir);
		if(status==0) {
			int status = chdir(data_buff);
			if(status == 0) {
				if(getcwd(current_dir,MAXPATHLEN)!=NULL) 
				{
					if( check_authorizedFolder(current_dir) == FALSE)
					{
						strcpy(current_dir, "/media");
						chdir(current_dir);
						send_repl(sock, 250,REPL_250);
						return TRUE;
					}

					send_repl(sock, 250, REPL_250);
					return TRUE;
				}
			}
		} 
	}

#ifdef	HUMAX_FTP
	send_repl(sock, 530, REPL_550);//REPL_550);
#else
	send_repl(sock, 530, REPL_550);
#endif
	return FALSE;
}

/**
 * Writes the contet of a file to the given client socket.
 * This is used for file download in ACTIVE mode.
 */
bool retrieve_file(int sock, int client_sock, int type, const char * file_name) 
{
	char read_buff[SENDBUFSIZE];
	char szFullPath[MAXPATHLEN];

	if(client_sock>0) {
		//close(client_sock);
		send_repl(sock, 150, REPL_150);
	}
	else {
		close(client_sock);
		send_repl(sock, 425, REPL_425);
		free(read_buff);
		return FALSE;
	}
	struct stat s_buff;

	//	Full Path. Check Validity
	if (file_name[0] == '/')
	{
		if (check_authorizedFolder(file_name) == FALSE)
		{
			strcpy(szFullPath, "/media");
			strcat(szFullPath, file_name);
			file_name = szFullPath;
		}
	}

	int status = stat(file_name,&s_buff);
	if(status!=0) {
		close(client_sock);
		send_repl(sock, 450, REPL_450);
		free(read_buff);
		return FALSE;
	}

	int b_mask = s_buff.st_mode & S_IFMT;
	if(b_mask != S_IFREG){
		close(client_sock);
		send_repl(sock, 451, REPL_451);
		free(read_buff);
		return FALSE;
	}

	char mode[3] ="r ";
	switch(type){
		case 1:
		case 3:
		case 4:
			mode[1]='b';
			break;
		case 2:
		default:
			mode[1]='t';
	}

	int fpr = open(file_name,O_RDONLY);
	if(fpr<0) {
		close(client_sock);
		send_repl(sock, 451, REPL_451);
		free(mode);
		free(read_buff);
		return FALSE;
	}

	// make transfer unbuffered
	int opt = fcntl(client_sock, F_GETFL, 0);
        if (fcntl(client_sock, F_SETFL, opt | O_ASYNC) == -1)
		{
			send_repl(sock, 426, REPL_426);
			close_connection(client_sock);
			free(read_buff);
			return FALSE;
		}
	while(1){
		int len = read(fpr,read_buff,SENDBUFSIZE);
		if(len>0) 
		{
			if (send_repl_client_len(client_sock,read_buff,len))
			{
				PRINTF("error : connection or send data error!!\n");
				break;
			}
		}
		else 
		{
			break;
		}
	}
	close(fpr);

	send_repl(sock, 226, REPL_226);
	close_connection(client_sock);
	return TRUE;
}

/**
 * Writes a file on the server, given an open client socket descriptor.
 * We are waiting for file contents on this descriptor.
 */
bool stou_file(int sock, int client_sock, int type, int fpr) {
	char read_buff[SENDBUFSIZE];

	if(fpr<0) {
		close_connection(client_sock);
		send_repl(sock, 451, REPL_451);
		free(read_buff);
		return FALSE;
	}

	// make transfer unbuffered
	int opt = fcntl(client_sock, F_GETFL, 0);
        if (fcntl(client_sock, F_SETFL, opt | O_ASYNC) == -1) {
		send_repl(sock, 426, REPL_426);
		close_connection(client_sock);
		free(read_buff);
		return FALSE;
	}

	while(1){
		
		int len = recv(client_sock,read_buff,SENDBUFSIZE,0);
		if(len>0) {
			write(fpr,read_buff,len);
		}
		else {
			break;
		}
	}

	close_connection(client_sock);
	close(fpr);
	send_repl(sock, 226, REPL_226);

	return TRUE;
}

/**
 * Writes a file, given a file path(name and location) and open client socket.
 */
bool store_file(int sock, int client_sock, int type, const char * file_name) {
	char read_buff[SENDBUFSIZE];

	if (check_availibility(file_name) == FALSE)
	{
		send_repl(sock, 550, REPL_550);
		return FALSE;
	}

	if(client_sock>0) {
		//close(client_sock);
		send_repl(sock, 150, REPL_150);
	}
	else {
		close_connection(client_sock);
		send_repl(sock, 425, REPL_425);
		free(read_buff);
		return FALSE;
	}
	struct stat s_buff;
	int status = stat(file_name,&s_buff);
	if(status==0) {
		int b_mask = s_buff.st_mode & S_IFMT;
		if(b_mask != S_IFREG){
			free(read_buff);
			close_connection(client_sock);
			send_repl(sock, 451, REPL_451);
			
			return FALSE;
		}
	}

	char mode[3] ="w ";
	switch(type){
		case 1:
		case 3:
		case 4:
			mode[1]='b';
			break;
		case 2:
		default:
			mode[1]='t';
	}

	int fpr = open(file_name,O_WRONLY|O_CREAT,0644);
	return stou_file(sock, client_sock,type,fpr);
}
