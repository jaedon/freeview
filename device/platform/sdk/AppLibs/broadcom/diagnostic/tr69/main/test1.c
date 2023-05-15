

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
 * File Name  : test1.c
 *
 * Description: test root for SOAP parser 
 *   
 *  
 * $Revision: 1.2 $
 * $Id: test1.c,v 1.2 2005/04/28 22:29:42 dmounday Exp $
 *----------------------------------------------------------------------*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <syslog.h>

#include "../SOAPParser/xmlParserSM.h"
#include "../SOAPParser/xmlTables.h"

int verbose;

void testFile(char *inf)
{
    eParseStatus    status;
    ParseHow    parseReq;

    parseReq.topLevel = envelopeDesc;
    parseReq.nameSpace = nameSpaces;
    status = parseGeneric(inf,NULL,0,&parseReq);
    fprintf(stderr, "Parse status of %s is %s\n", inf, status==NO_ERROR? "OK":
        "Bad");
    if (status==NO_ERROR) {
        runRPC();
    }
}
int main(int argc, char** argv)
{
    int verbose = 0;
    char    *inf;
    char    c;

    while ((c=getopt(argc, argv, "vf:")) != -1) {
        switch(c){
        case 'v':
            verbose = 1;
            break;
        case 'f':
            inf = optarg;
            break;
        default:
            break;
        }
    }
    
    openlog( "user", LOG_CONS,LOG_DAEMON);
    testFile(inf);
}
