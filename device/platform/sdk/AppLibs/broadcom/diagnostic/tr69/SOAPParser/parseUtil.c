
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
 * File Name  : parseUtil.c
 *
 * Description: parser support routine
 *   
 *  
 * $Revision: 1.3 $
 * $Id: parseUtil.c,v 1.3 2005/10/04 19:51:27 dmounday Exp $
 *----------------------------------------------------------------------*/

 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>

#include "../nanoxml/nanoxml.h"
#include "parseUtil.h"
//#include "xmlParserSM.h"
#include "xmlTables.h"
#ifdef DMALLOC
#include "dmalloc.h"
#endif

extern int verbose;
extern XmlNodeDesc envelopeDesc[];
#define NO_ERROR    0
#define PARSE_ERROR 1

/*----------------------------------------------------------------------*/
static void parse_error(char *errfmt, ...) {
    va_list ap;

    va_start(ap, errfmt);
    vlog(LOG_ERR, errfmt, ap);
    va_end(ap);
    return;
}

/*******************************************************************/
/*----------------------------------------------------------------------*
 * parse from file or in-memory data
 *   parse_generic("/apa", NULL, 0, ...)   parses content in file /apa
 *   parse_generic(NULL, ptr, size, ...)      parses content pointed to by ptr
 *   parse_generic("/apa", ptr, size, ...)    error, illegal usage return NULL
 */
ParseResult *parse_generic(char *path, char *memory, int size, XmlNodeDesc *startNode, NameSpace *nspace)
{
    char buf[BUFSIZE];
    int done;
    int file;
    nxml_settings settings;
    nxml_t parser;
    char *xmlEnd;
    int error = NO_ERROR;

    if (path != NULL && memory != NULL) {
        slog(LOG_ERR, "parser", "internal error: parse_generic() can not parse both from file and memory\n");
        return NULL;
    }
    settings.tag_begin = xmlTagBegin;
    settings.tag_end = xmlTagEnd;
    settings.attribute_begin = xmlAttr;
    settings.attribute_value = xmlValue;
    settings.data = xmlData;

    if (nxml_open(&parser,&settings)) {
        parser->node = parser->nodestack[0] = startNode;
        parser->nameSpaces = nspace;        /* nameSpace table to use */
        parser->parse_error = parse_error;  /* set error handler */
        if (path != NULL) {
            if ( (file = open(path, O_RDONLY, 0 ))== -1){
                slog(LOG_ERR, "Parser:Could not open file %s", path);
                return NULL;
            }
        }
        do {
            if (path != NULL) {
                /* from file */
                size_t len = read(file, buf, sizeof(buf));
                done = len < sizeof(buf);
                if ( nxml_write(parser, buf, len, &xmlEnd)<1) {
                    slog(LOG_ERR, "Parser:invalid xml config in file %s",
                        path);
                    error = PARSE_ERROR;
                }
            } else {
                /* from memory */
                done = 1;
                if ( nxml_write(parser, memory, size, &xmlEnd)<1) {
                    slog(LOG_ERR, "parser", "invalid xml config in memory");
                    /* need line number of error here */
                    error = PARSE_ERROR;
                }
            }
        } while (error == NO_ERROR && !done);
    }
    if (path != NULL) {
        close(file);
    }
    nxml_close(parser);

    switch (start) {
    case S_SCANFILE:
        break;
    default:
        break;
    }
    return NULL;
}


/*----------------------------------------------------------------------*/
void slog(int level, const char* fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    if (verbose) {
        vfprintf(stderr, fmt, ap);
    }
    vsyslog(level, fmt, ap);
    va_end(ap);
}

void vlog(int level, const char* fmt, va_list ap)
{
    if (verbose) {
        vfprintf(stderr, fmt, ap);
    }
    vsyslog(level, fmt, ap);
}
