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
 * File Name  : bcmSysIF.h
 *
 * Description: System object functions that interface to the Broadcom library
 *              functions or the hardware configuration dependent
 *              features.
 *              
 * 
 * $Revision: 1.1 $
 * $Id: bcmSysIF.h,v 1.1 2005/11/18 19:25:47 broadcom Exp $
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../SOAPParser/CPEframework.h"
/*#include "bcmcfmsys.h"*/

TRX_STATUS tr69c_setDNSServers(char *value);
TRX_STATUS tr69c_getDNSServers(char **value);
TRX_STATUS tr69c_setDefaultGateway(char *value);
TRX_STATUS tr69c_getDefaultGateway(char **value);


