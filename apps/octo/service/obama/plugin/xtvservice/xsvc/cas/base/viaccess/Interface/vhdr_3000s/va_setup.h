/******************************************************************************
    Copyright
    This code is strictly confidential and the receiver is obliged to use it
    exclusively for his or her own purposes. No part of Viaccess code may be
    reproduced or transmitted in any form or by any means, electronic or
    mechanical, including photocopying, recording, or by any information storage
    and retrieval system, without permission in writing from Viaccess.
    The information in this code is subject to change without notice. Viaccess
    does not warrant that this code is error free. If you find any problems
    with this code or wish to make comments, please report them to Viaccess.

    Trademarks
    Viaccess is a registered trademark of Viaccess ® in France and/or other
    countries. All other product and company names mentioned herein are the
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights
    or other intellectual property rights over the code hereafter. Unless
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over
    these patents, trademarks, copyrights or other intellectual property.

******************************************************************************/

/*
    $HeadURL$
    $Revision: 4641 $
    $Author $
    $Date: 2008-11-24 17:29:01 +0100 (lun., 24 nov. 2008) $

*/


/**
    @file       va_setup.h
    @ingroup    ACS Interface
    @brief      This is the configuration file
*/



#ifndef _VA_SETUP_H_
#define _VA_SETUP_H_


#define kVA_SETUP_NBMAX_ACS                         4
#define kVA_SETUP_NBMAX_SC                          1

#define kVA_SETUP_SEMAPHORE_SIZE_IN_DWORD           6
#define kVA_SETUP_MUTEX_SIZE_IN_DWORD               6

#define kVA_SETUP_NVM_SIZE_IN_BYTE                  (8 * 1024)

#define kVA_SETUP_NBMAX_SECTIONBUFFERS              64
#define kVA_SETUP_NBMAX_SECTIONFILTERS              16
#define kVA_SETUP_NBMAX_PID_CHANNELS                8
#define kVA_SETUP_NBMAX_FILTERS_PER_CHANNEL         5

#define kVA_SETUP_MULTICAST_DATAGRAM_MAX_SIZE       1536
#define kVA_SETUP_MULTICAST_MINIMUM_BUFFER_SIZE     (4 * 1024)
#define kVA_SETUP_UNICAST_MINIMUM_BUFFER_SIZE       (2 * 1024)
#define kVA_SETUP_MAX_MULTICAST_SOCKETS             10
#define kVA_SETUP_MAX_UNICAST_SOCKETS               10

#define kVA_SETUP_DNS_ENABLED                       1

/*---------------------------------------------------------------------------------

If you are using an old driver, please set :

#define kVA_SETUP_CHIPSET_TYPE                      2

If you are using any ST chipset please set :

#define kVA_SETUP_CWPT                              1

If you are using any other chipset please set :

#define kVA_SETUP_CWPT                              0

------------------------------------------------------------------------------------*/

#define kVA_SETUP_CWPT                              1

#endif /* _VA_SETUP_H_ */

