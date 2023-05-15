/***************************************************************************
 *            connections.c
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

#ifdef HUMAX_FTP
#include <sys/time.h>
#include <time.h>
#endif

#include "defines.h"
#include "cmdparser.h"
#include "fileutils.h"

#define	PRINTF		//printf

#ifdef	HUMAX_FTP

#define	MAX_DELAY_TIME	30
/*
 * CAS 업체에 따라 FTP를 통한 파일 다운로드 정책을 달리 할 수 있다.
 *
 * IRDETO: Encryption 레코딩 파일은 FTP에서 조작할 수 없도록 해야 한다. 
 */
cas		cas_type;

/*
 * Pin 코드 해킹을 방지하기 위해 패스워드 자꾸 틀리면 늦게 응답해버린다.
 * 단, 접속자가 여럿일 경우.... 그 여럿 중... 아무나 틀려도 응답 늦게 해버린다.
 */
int		delay_reply;
#endif

int open_connections;
bool max_limit_notify;

int raiseerr(int err_code) {
	PRINTF("Error %d\n",err_code);
	return -1;
}

/** 
 * This is neccessary for future use of glib and gettext based localization.
 */
const char * _(const char* message) {
	return message;
}

/**
 * Guess the transfer type, given the client requested type.
 * Actually in unix there is no difference between binary and 
 * ascii mode when we work with file descriptors.
 * If #type is not recognized as a valid client request, -1 is returned.
 */
int get_type(const char *type) {
	if(type==NULL)
		return -1;
	int len = strlen(type);
	if(len==0)
		return -1;
	switch(type[0]) {
		case 'I':
			return 1;
		case 'A':
			return 2;
		case 'L':
			if(len<3)
				return -1;
			if(type[2]=='7')
				return 3;
			if(type[2]=='8')
				return 4;
	}
	return -1;
}

/**
 * Create a new connection to a (address,port) tuple, retrieved from
 * the PORT command. This connection will be used for data transfers
 * in commands like "LIST","STOR","RETR"
 */
int make_client_connection(int sock_fd,int client_port,const char* client_addr) {
	if(client_port<1) {
		send_repl(sock_fd, 425, REPL_425);
		return -1;
	}
	int sock=-1;
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(client_addr);
	servaddr.sin_port = htons (client_port);
	if ((sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		send_repl(sock,425, REPL_425);
		raiseerr(15);
		return -1;
	}
	int status = connect (sock, (struct sockaddr *)&servaddr, sizeof (servaddr));
	if(status!=0) {
		send_repl(sock, 425, REPL_425);
		return -1;
	}
	return sock;
}

/**
 * Close the connection to the client and exit the child proccess.
 * Although it is the same as close(sock_fd), in the future it can be used for 
 * logging some stats about active and closed sessions.
 */
void close_conn(int sock_fd) {
	if (close(sock_fd) < 0) { 
		raiseerr (5);
	}
	exit(0);
}

/**
 * Get the next command from the client socket.
 */
int get_command(int conn_fd,char *read_buff1,char *data_buff) {
	char read_buff[RCVBUFSIZE];
	memset((char *)&read_buff, 0, RCVBUFSIZE);
	read_buff[0]='\0';
	char *rcv=read_buff;
	int cmd_status = -1;
	int recvbuff = recv(conn_fd,read_buff,RCVBUFSIZE,0);
	if(recvbuff<1) {
		return CMD_CLOSE;
	}
	if(recvbuff==RCVBUFSIZE) {
		return CMD_UNKNOWN;
	}
	PRINTF("Received:%s\n",rcv);
	cmd_status = parse_input(rcv,data_buff);
	return cmd_status;
}

/**
 * A handler, which is called on child proccess exit.
 */
void sig_chld_handler(void) {
	open_connections--;
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

#ifdef HUMAX_FTP
void sig_delay_reply_handler(int sig)
{
	if (sig == SIGUSR1)	// wrong password
	{
		PRINTF("WRONG_PASSWORD: %d\n", delay_reply);	// I don't know who you are... but DELAY DELAY!!!
		if (delay_reply < MAX_DELAY_TIME)
		{
			delay_reply++;
		}
	}
	else if (sig == SIGUSR2)	// correct password
	{
		PRINTF("CORRECT_PASSWORD: %d\n", delay_reply);
		delay_reply = 0;
	}
}
#endif

/**
 * Send reply to the client socket, given the reply.
 */
int send_repl(int send_sock, int errCode, char *msg)
{
#ifdef	HUMAX_FTP
	char	szMessage[512];

	if (errCode == 0)
		sprintf(szMessage, "%s", msg);
	else
		sprintf(szMessage, "%d %s", errCode, msg);
	msg = szMessage;
#endif

	if (send(send_sock, msg, strlen(msg),0) < 0) { 
		raiseerr (4);
		close(send_sock);
		exit(0);
	}
	return 0;
}

/**
 * Send single reply to the client socket, given the reply and its length.
 */
int send_repl_client_len(int send_sock,char *msg,int len) {
	if (send(send_sock, msg, len,0) < 0) { 
		raiseerr (4);
		close(send_sock);
		return 1;
	}
	return 0;
}

/*
Izprashtane na edinishen otgovor do dopulnitelnia socket za transfer
*/
int send_repl_client(int send_sock,char *msg) 
{
	send_repl_client_len(send_sock,msg,strlen(msg));
	return 0;
}

/**
 * Send single reply to the additional transfer socket, given the raply and its length.
 */
int send_repl_len(int send_sock,int errCode, char *msg,int len) 
{
#ifdef	HUMAX_FTP
	char	szMessage[512];

	len = sprintf(szMessage, "%d %s", errCode, msg);
	msg = szMessage;
#endif

	if (send(send_sock, msg, len,0) < 0) { 
		raiseerr (4);
		close(send_sock);
		exit(0);
	}
	return 0;
}

/**
 * Parses the results from the PORT command, writes the
 * address in "client_addt" and returnes the port
 */
int parse_port_data(char *data_buff,char *client_addr) {
	client_addr[0]='\0';
	int len=0;
	int port=0;
	int _toint=0;
	char *result;
	result = strtok(data_buff, PORTDELIM);
	_toint=toint(result,FALSE);
	if(_toint<1 || _toint>254)
		return -1;
	len += strlen(result);
	strcpy(client_addr,result);
	client_addr[len]='\0';
	strcat(client_addr,".");
	len++;

	result = strtok(NULL, PORTDELIM);
	_toint=toint(result,FALSE);
	if(_toint<0 || _toint>254)
		return -1;
	len += strlen(result);
	strcat(client_addr,result);
	client_addr[len]='\0';
	strcat(client_addr,".");
	len++;

	result = strtok(NULL, PORTDELIM);
	if(_toint<0 || _toint>254)
		return -1;
	len += strlen(result);
	strcat(client_addr,result);
	client_addr[len]='\0';
	strcat(client_addr,".");
	len++;

	result = strtok(NULL, PORTDELIM);
	if(_toint<0 || _toint>254)
		return -1;
	len += strlen(result);
	strcat(client_addr,result);
	client_addr[len]='\0';
	
	result = strtok(NULL, PORTDELIM);
	len = toint(result,FALSE);
	if(_toint<0 || _toint>255)
		return -1;
	port = 256*len;
	result = strtok(NULL, PORTDELIM);
	len = toint(result,FALSE);
	if(_toint<0 || _toint>255)
		return -1;
	port +=len;
	return port;
}

void print_help(int sock)
{
	send_repl(sock, 0, "  HUMAX PVR FTP Server. \r\n");
}

void print_result(int result)
{
	static char *szCmdStr[] = {
			"CMD_USER",	"CMD_PASS",	"CMD_ACCT",	"CMD_CWD",	"CMD_CDUP",
			"CMD_SMNT",	"CMD_QUIT",	"CMD_REIN",	"CMD_PORT",	"CMD_PASV",
			"CMD_TYPE",	"CMD_STRU",	"CMD_MODE",	"CMD_RETR",	"CMD_STOR",
			"CMD_STOU",	"CMD_APPE",	"CMD_ALLO",	"CMD_REST",	"CMD_RNFR",
			"CMD_RNTO",	"CMD_ABOR",	"CMD_DELE",	"CMD_RMD",	"CMD_MKD",
			"CMD_PWD",	"CMD_LIST",	"CMD_NLST",	"CMD_SITE",	"CMD_SYST",
			"CMD_STAT",	"CMD_HELP",	"CMD_NOOP",	"CMD_UNKNOWN",	"CMD_EMPTY",
			"CMD_CLOSE" };
			
	PRINTF("result - [%08x][%s]\n", result, szCmdStr[result]);
}

 
/**
 * Main cycle for client<->server communication. 
 * This is done synchronously. On each client message, it is parsed and recognized,
 * certain action is performed. After that we wait for the next client message
 * 
 */
int interract(int conn_fd,cmd_opts *opts) {
	static int BANNER_LEN = strlen(REPL_220);
	int userid = opts->userid;
	int client_fd=-1;
	int len;
	int _type ;
	int type = 2; // ASCII TYPE by default
	if(userid>0) {
		int status = setreuid(userid,userid);
		if(status != 0) {
			switch(errno) {
				case EPERM:
					break;
				case EAGAIN:
					break;
				default:
					break;
			}
			close_conn(conn_fd);
		}
		
	}
	if(max_limit_notify) {
		send_repl(conn_fd, 120, REPL_120);
		close_conn(conn_fd);
	}
	char current_dir[MAXPATHLEN];
	char parent_dir[MAXPATHLEN];
	char virtual_dir[MAXPATHLEN];
	char reply[SENDBUFSIZE];
	char data_buff[DATABUFSIZE];
	char read_buff[RCVBUFSIZE];
	char *str;
	bool is_loged = FALSE;
	bool state_user = FALSE;
	char rename_from[MAXPATHLEN];
	
	memset((char *)&current_dir, 0, MAXPATHLEN);
	strcpy(current_dir,opts->chrootdir);
	strcpy(parent_dir,opts->chrootdir);
	free(opts);
	chdir(current_dir);
	if((getcwd(current_dir,MAXPATHLEN)==NULL)) {
		raiseerr(19);
		close_conn(conn_fd);
	}
	memset((char *)&data_buff, 0, DATABUFSIZE);
	memset((char *)&read_buff, 0, RCVBUFSIZE);
	
	reply[0]='\0';
	int client_port = 0;
	char client_addr[ADDRBUFSIZE];

#ifdef HUMAX_FTP
 	cas_type		= opts->cas_type;
	int delay_reply_conn = 0;	// connection 상태에서 자꾸 Password 틀리면 늦게 응답한다
#endif

	send_repl_len(conn_fd, 220, REPL_220, BANNER_LEN);
	while(1) {
		data_buff[0]='\0';
		int result = get_command(conn_fd,read_buff,data_buff);
		if(result != CMD_RNFR && result != CMD_RNTO && result != CMD_NOOP)
			rename_from[0]='\0';

PRINTF("result - [%08x]\n", result);

#ifdef HUMAX_FTP
		print_result(result);


		if (cas_type == CAS_IRDETO)
		{
			if (result == CMD_RNFR || result == CMD_RNTO || result == CMD_SITE)
			{
				if (!is_loged)
				{
					send_repl(conn_fd, 530, REPL_530);
				}
				else
				{
					send_repl(conn_fd, 502, REPL_502_NOT_SUPPORT);
				}
				continue;
			}
		}
#endif
		switch(result) {
			case CMD_UNKNOWN:
			case -1:
				send_repl(conn_fd, 500, REPL_500);
				break;
			case CMD_EMPTY:
			case CMD_CLOSE:
				close_conn(conn_fd);
				break;
			case CMD_USER:
#ifndef	HUMAX_FTP
				if(data_buff==NULL || strcmp(data_buff,ANON_USER)==0) {
					state_user = TRUE;
					send_repl(conn_fd, 331, REPL_331_ANON);
				} else
#else
				if (data_buff)
				{
					//	Set data to low case
					int		i;
					for (i = 0; i < strlen(data_buff); i++)
					{
						if ((data_buff[i] >= 0x41) && (data_buff[i] <= 0x5A))
							data_buff[i] += 0x20;
					}
				}

				if (data_buff == NULL || strcmp(data_buff, opts->acceptUserId) == 0)
				{
					state_user = TRUE;
					sprintf(reply, REPL_331, data_buff);
					send_repl(conn_fd, 331, reply);
				} else
#endif
				{
					send_repl(conn_fd, 332, REPL_332);
				}
				break;
			case CMD_PASS:
				if(!state_user) {
					send_repl(conn_fd, 503, REPL_503);
				}
				else 
				{
#ifdef	HUMAX_FTP
					if (data_buff != NULL && strcmp(data_buff, opts->password) == 0)
					{
						kill(getppid(), SIGUSR2); // 부모한테 나 로그인 한다고 갈켜준다.
						is_loged = TRUE;
						state_user = FALSE;
						send_repl(conn_fd, 230, REPL_230);
					} else
					{
						kill(getppid(), SIGUSR1); // 틀렸으니까, 담번에 또 물어볼 때 늦게 갈켜준다.
						state_user = FALSE;

						if (delay_reply < MAX_DELAY_TIME)
						{
							delay_reply++;	// 자식놈 메모리 공간에 있으므로, 부모가 증가시키는 거랑 따로 논다.
						}
							
						sleep(delay_reply);
						send_repl(conn_fd, 503, REPL_503);
					}
#else
					is_loged = TRUE;
					state_user = FALSE;
					send_repl(conn_fd, 230, REPL_230);
#endif
				}
				break;
			case CMD_PORT:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					client_port = parse_port_data(data_buff,client_addr);
					if(client_port<0) {
						send_repl(conn_fd, 501, REPL_501);
						client_port = 0;
					} else {
						send_repl(conn_fd, 200, REPL_200);
					}
				}
				break;
			case CMD_PASV:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					send_repl(conn_fd, 502, REPL_502);
				}
				break;
			case CMD_SYST:
				reply[0]='\0';
				len = sprintf(reply,REPL_215,"UNIX");
				reply[len] = '\0';
				send_repl(conn_fd, 215, reply);
				break;
			case CMD_LIST:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					client_fd = make_client_connection(conn_fd, client_port,client_addr);
					if(client_fd!=-1){
						write_list(conn_fd,client_fd,current_dir);
					}
					client_fd = -1;
				}
				break;
			case CMD_RETR:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd, 501, REPL_501);
					} else {
						client_fd = make_client_connection(conn_fd, client_port,client_addr);
						if(client_fd!=-1){
#ifdef HUMAX_FTP							
							if (cas_type == CAS_IRDETO && is_copy_protected_contents(current_dir, data_buff))
							{
								send_repl(conn_fd, 560, REPL_560);
								//close_conn(client_fd);
								//close_conn(conn_fd);
								client_fd = -1;
								break;
							}
#endif
							retrieve_file(conn_fd,client_fd, type,data_buff);
						}
						client_fd = -1;
					}
				}
				break;
			case CMD_STOU:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					int fd = mkstemp("XXXXX");
					client_fd = make_client_connection(conn_fd, client_port,client_addr);
					if(client_fd!=-1){
						stou_file(conn_fd,client_fd, type,fd);
					}
					client_fd = -1;
				}
				break;
			case CMD_STOR:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd, 501, REPL_501);
					} else {
						client_fd = make_client_connection(conn_fd, client_port,client_addr);
						if(client_fd!=-1){
#ifdef HUMAX_FTP
							if (cas_type == CAS_IRDETO && is_irdeto_session_file(data_buff))
							{
								send_repl(conn_fd, 561, REPL_561);
								//close_conn(client_fd);
								//close_conn(conn_fd);
								client_fd = -1;
								break;
							}
#endif
							store_file(conn_fd,client_fd, type,data_buff);
						}
						client_fd = -1;
					}
				}
				break;
			case CMD_SITE:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd, 501, REPL_501);
					} else {
						send_repl(conn_fd, 202, REPL_202);
					}
				}
				break;
			case CMD_PWD:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else 
				{
					reply[0]='\0';
					len = sprintf(reply,REPL_257_PWD, current_dir);
					reply[len] = '\0';
					PRINTF("PWD : [%s]\n", current_dir);
					send_repl(conn_fd, 257, reply);
				}
				break;
			case CMD_CDUP:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					change_dir(conn_fd,parent_dir,current_dir,virtual_dir,"..");
				}
				break;
			case CMD_CWD:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd, 501, REPL_501);
					} else {
						change_dir(conn_fd,parent_dir,current_dir,virtual_dir,data_buff);
					}
				}
				break;
			case CMD_QUIT:
				send_repl(conn_fd, 221, REPL_221);
				if(client_fd!=-1){
					close_conn(client_fd);
				}
				close_conn(conn_fd);
				break;
			case CMD_TYPE:
				_type = get_type(data_buff);
				if(_type ==-1) {
					send_repl(conn_fd, 500, REPL_500);
				}
				else {
					type=_type;
					send_repl(conn_fd, 200, REPL_200);
				}
				break;
			case CMD_STAT:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd, 501, REPL_501);
					}
					else {
						stat_file(conn_fd,data_buff,reply);
					}
				}
				break;
			case CMD_ABOR:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
#ifdef HUMAX_FTP						
					send_repl(conn_fd, 552, REPL_552);
#endif
					if(client_fd!=-1){
						close_connection(client_fd);
					} 
					send_repl(conn_fd, 226, REPL_226);
				}
				break;
			case CMD_MKD:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd, 501, REPL_501);
					} else {
						make_dir(conn_fd,data_buff,reply);
					}
				}
				break;
			case CMD_RMD:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd, 501, REPL_501);
					} else {
						remove_dir(conn_fd,data_buff);
					}
				}
				break;
			case CMD_DELE:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd, 501, REPL_501);
					} else {
#ifdef HUMAX_FTP							
						if (cas_type == CAS_IRDETO && is_copy_protected_contents(NULL, data_buff))
						{
							send_repl(conn_fd, 562, REPL_562);
							break;
						}
#endif					
						delete_file(conn_fd,data_buff);
					}
				}
				break;				
			case CMD_RNFR:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd, 501, REPL_501);
					} else {
						strcpy(rename_from,data_buff);
						send_repl(conn_fd, 350, REPL_350);
					}
				}
				break;
			case CMD_RNTO:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd, 501, REPL_501);
					} else {
						if(rename_from==NULL || strlen(rename_from)==0 || rename_from[0]=='\0') {
							send_repl(conn_fd, 501, REPL_501);
						} else {
							rename_fr(conn_fd,rename_from,data_buff);
						}
					}
					rename_from[0]='\0';
				}
				break;
			case CMD_NOOP:
				send_repl(conn_fd, 200, REPL_200);
				break;
			case CMD_STRU:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd, 501, REPL_501);
					} else {
						switch(data_buff[0]) {
							case 'F':
								send_repl(conn_fd, 200, REPL_200);
								break;
							case 'P':
							case 'R':
								send_repl(conn_fd, 504, REPL_504);
								break;
							default:
								send_repl(conn_fd, 501, REPL_501);
							
						}
					}
				}
				break;
			case CMD_HELP:
			//	if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
					send_repl(conn_fd, 214, REPL_214);
					print_help(conn_fd);
					send_repl(conn_fd, 214, REPL_214_END);
			//	}
			// XXX separate HELP without arguments from HELP for a single command
				break;
			case CMD_MODE:
				if(!is_loged) send_repl(conn_fd, 530, REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd, 501, REPL_501);
					} else {
						switch(data_buff[0]) {
							case 'S':
								send_repl(conn_fd, 200, REPL_200);
								break;
							case 'B':
							case 'C':
								send_repl(conn_fd, 504, REPL_504);
								break;
							default:
								send_repl(conn_fd, 501, REPL_501);
							
						}
					}
				}
				break;
			default:
				send_repl(conn_fd, 502, REPL_502);
		}
	}
	
	free(data_buff);
	free(read_buff);
	free(current_dir);
	free(parent_dir);
	free(virtual_dir);
	free(rename_from);
	close_conn(conn_fd);
}

/**
 * Close a socket and return a statsu of the close operation.
 * Although it is equivalent to close(connection) in the future it can be used
 * for writing logs about opened and closed sessions.
 */
int close_connection(int connection) {
	return close(connection);
}

/**
 * Creates new server listening socket and make the main loop , which waits
 * for new connections.
 */
int create_socket(struct cmd_opts *opts) {
	if(opts==NULL)
		return 10;
	int status = chdir(opts->chrootdir);
	if(status!=0) {
		raiseerr(15);
	}
	int servaddr_len =  0;
	int connection = 0;
	int sock = 0;
	int pid  = 0;
	open_connections=0;
	
	struct sockaddr_in servaddr;
	pid = getuid();	
	if(pid != 0 && opts->port <= 1024)
	{
		PRINTF(_(" Access denied:\n     Only superuser can listen to ports (1-1024).\n You can use \"-p\" option to specify port, greater than 1024.\n"));
		exit(1);
	}
	memset((char *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = PF_INET;
	if(opts->listen_any==TRUE) {
		servaddr.sin_addr.s_addr =  htonl(INADDR_ANY);
	}
	else if(opts->listen_addr==NULL) {
		return 9;
	} else {
		struct hostent *host = gethostbyname(opts->listen_addr);
		if(host==NULL) {
			PRINTF(_("Cannot create socket on server address: %s\n"),opts->listen_addr);
			return 11;
		}
		bcopy(host->h_addr, &servaddr.sin_addr, host->h_length);
	}
	servaddr.sin_port = htons (opts->port);
	servaddr_len = sizeof(servaddr);
	if ((sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		raiseerr(ERR_CONNECT);
		return 1;
	}
	int flag = 1;
	setsockopt(sock, SOL_SOCKET,SO_REUSEADDR,(char *) &flag, sizeof(int));
	
	// remove the Nagle algorhytm, which improves the speed of sending data.
	setsockopt(sock, IPPROTO_TCP,TCP_NODELAY,(char *) &flag, sizeof(int));
	
	if(bind (sock, (struct sockaddr *)&servaddr, sizeof(servaddr))<0) {
		if(opts->listen_any==FALSE) {
			PRINTF(_("Cannot bind address: %s\n"),opts->listen_addr);
		}else {
			PRINTF(_("Cannot bind on default address\n"));
		}
		return raiseerr(8);
	}
	if(listen(sock,opts->max_conn) <0) {
		return raiseerr(2);
	}
	#ifdef __USE_GNU
		signal(SIGCHLD, (sighandler_t )sig_chld_handler);
	#endif
	#ifdef __USE_BSD
		signal(SIGCHLD, (sig_t )sig_chld_handler);
	#endif
	
#ifdef HUMAX_FTP	
	#ifdef __USE_GNU
		signal(SIGUSR1, (sighandler_t )sig_delay_reply_handler);
		signal(SIGUSR2, (sighandler_t )sig_delay_reply_handler);
	#endif
	#ifdef __USE_BSD
		signal(SIGUSR1, (sig_t )sig_delay_reply_handler);
		signal(SIGUSR2, (sig_t )sig_delay_reply_handler);
	#endif

	time_t	cur_connection;
	time_t	last_connection;
	struct timeval	tv;
	gettimeofday(&tv, NULL);

	last_connection = cur_connection = tv.tv_sec;
#endif
	for (;;) {
		max_limit_notify = FALSE;
		
		if ((connection = accept(sock, (struct sockaddr *) &servaddr, &servaddr_len)) < 0) {
			raiseerr(3);
			return -1;
		}
		
#ifdef HUMAX_FTP
		last_connection = cur_connection;
		
		gettimeofday(&tv, NULL);
		cur_connection = tv.tv_sec;
		
		// 마지막 접속을 하지 MAX_DELAY_TIME초 이상 지나면 Delay reply를 하지 않는다. 
		if (cur_connection - last_connection > MAX_DELAY_TIME) 
		{
			delay_reply = 0;
		}
#endif		
		pid = fork();
		if(pid==0) {
			if(open_connections >= opts->max_conn)
				max_limit_notify=TRUE;
			interract(connection,opts);
		} else if(pid>0) {
			open_connections++;
			assert(close_connection(connection)>=0);
		}
		else {
			 
			close(connection);
			close(sock);
			assert(0);
		}
	}
}
