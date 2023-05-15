
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
 * File Name  : testSetup.c
 *
 * Description: Used to initialize some CPE parameters for testing
 *              the framework.
 * $Revision: 1.14 $
 * $Id: testSetup.c,v 1.14 2005/10/14 22:11:05 dmounday Exp $
 *----------------------------------------------------------------------*/
#include "../inc/tr69cdefs.h"
#include "../inc/appdefs.h"

extern TRX_STATUS setMSrvrURL(char *);
extern TRX_STATUS setMSrvrUsername(char *);
extern TRX_STATUS setMSrvrPassword(char *);
extern TRX_STATUS setMSrvrInformInterval(char *);
extern TRX_STATUS setMSrvrInformEnable(char *);
extern TRX_STATUS setConnectionUsername(char *);
extern TRX_STATUS setConnectionPassword(char *);
extern TRX_STATUS getSerialNumber(char **value);
extern TRX_STATUS getManufacturerOUI(char **value);

void initTestSetup(void)
{
     /*setMSrvrURL("http://supernova.devlab.motive.com:7001/devman/CPEMgt"); */
#ifdef SUPPORT_ACS_CISCO
     setMSrvrURL("http://192.168.10.106:9966");
     setMSrvrUsername("broadcom");
     setMSrvrPassword("broadcom");
#endif
#ifdef SUPPORT_ACS_DIMARK
     #ifdef USE_SSL
         setMSrvrURL("https://demo.dimark.com/dps/TR069");
     #else
         setMSrvrURL("http://demo.dimark.com:8888/dps/TR069");
     #endif
     setMSrvrUsername("dms");
     setMSrvrPassword("dms");
#endif
#ifdef SUPPORT_ACS_PIRELLI
     setMSrvrURL("http://82.105.195.112:8080/acs/service");
     setMSrvrUsername("broadcom");
     setMSrvrPassword("broadcom");
#endif
#ifdef SUPPORT_ACS_2WIRE
     char *oui = (char *) 0;
     char *sernum = (char *) 0;

     #ifdef USE_SSL
         setMSrvrURL("https://cwmp.demonoc-3.2wire.com");
     #else
         setMSrvrURL("http://cwmp.demonoc-3.2wire.com");
     #endif
     setMSrvrPassword("broadcom");
     getManufacturerOUI(&oui);
     getSerialNumber(&sernum);
     if( oui && sernum ) {
        char username[64];
        sprintf(username, "%s-%s", oui, sernum);
        setMSrvrUsername(username);
     }
     if(oui)
        free(oui);
     if(sernum)
        free(sernum);
#endif

     setMSrvrInformInterval("300");
     setMSrvrInformEnable("1");
     setConnectionUsername("admin");
     setConnectionPassword("admin");
}
