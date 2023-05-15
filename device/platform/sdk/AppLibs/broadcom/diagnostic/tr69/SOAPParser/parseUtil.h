#ifndef PARSE_UTIL_H
#define PARSE_UTIL_H

/*----------------------------------------------------------------------*
<:copyright-broadcom 
 
 Copyright (c) 2005 Broadcom Corporation 
 All Rights Reserved 
 No portions of this material may be reproduced in any form without the 
 written permission of: 
          Broadcom Corporation 
          16215 Alton Parkway 
          Irvine, California 92619 
 All information contained in this document is Broadcom Corporation 
 company private, proprietary, and trade secret. 
 
:>
 *----------------------------------------------------------------------*
 * File Name  : parseUtil.h
 *
 * Description: parser support routine prototype 
 *   
 *  
 * $Revision: 1.2 $
 * $Id: parseUtil.h,v 1.2 2005/04/28 22:29:42 dmounday Exp $
 *----------------------------------------------------------------------*/
 

typedef enum {
    S_SCANFILE
}XMLPARSEREQ;
#define BUFSIZE 4096

void slog(int level, const char* fmt, ...);
void vlog(int level, const char* fmt, va_list ap);
void *parse_generic(char *path, char *memory, int size, XMLPARSEREQ start);



#endif

