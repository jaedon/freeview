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
    Viaccess is a registered trademark of Viaccess ?in France and/or other
    countries. All other product and company names mentioned herein are the
    trademarks of their respective owners.
    Viaccess may hold patents, patent applications, trademarks, copyrights
    or other intellectual property rights over the code hereafter. Unless
    expressly specified otherwise in a Viaccess written license agreement, the
    delivery of this code does not imply the concession of any license over
    these patents, trademarks, copyrights or other intellectual property.

******************************************************************************/

#ifndef _VA_SETUP_H_
#define _VA_SETUP_H_

/**
 @ingroup common
*/

/**
 @defgroup setup va_setup.h
    @brief      This is the setup file associated to the STB.
 @{
*/

/* 
 * General
 * ------- */
/**
 * ACS instances number max : 1 instance per source (tuner).
 */
#define kVA_SETUP_NBMAX_ACS                         10

/*
 * General
 * ------- */
/**
 * SmartCard reader number max : 1 per slot
 * @note For PurpleBox or BroadcastCardless project this value MUST be 0 !!!
 */
#define kVA_SETUP_NBMAX_SC                          1

/*
 * Platform
 * -------- */
/**
 * Semaphore size (in DWORD) required on the OS platform.
 */
#define kVA_SETUP_SEMAPHORE_SIZE_IN_DWORD           6

/*
 * Platform
 * -------- */
/**
 * Mutex size (in DWORD) required on the OS platform.
 */
#define kVA_SETUP_MUTEX_SIZE_IN_DWORD               6

/*
 * Non volatile memory
 * ------------------- */
/**
 * NVM data segment size:
 *  - Basically 8 Ko are required.
 *    For PurpleBox 40 Ko are required.
 *    For Broadcast Cardless 32 Ko are required.
 *          - If CVOD feature is used, more 16 ko are required
 *          - If SecureHybrid feature is used (IPCAS option), more 10 Ko are required.
 *  - The backup size is optional (only required for Broadcast Cardless project) :
 *    define kVA_SETUP_ACS_DATA_BACKUP_SIZE         (4  * 1024)
 */
#define kVA_SETUP_ACS_DATA_SEGMENT_SIZE             (48 * 1024)
#define kVA_SETUP_ACS_DATA_BACKUP_SIZE         		(4  * 1024)


#define kVA_SETUP_NVM_SIZE_IN_BYTE                  (8 * 1024)  // for just compile, hjlee3.  to be remove

/*
 * File system
 * ----------- */
/**
 * Minimal size required by Viaccess for the file system.
 */
#define kVA_SETUP_FS_SIZE_IN_BYTE                   (50 * 1024 * 1024)

/*
 * Demux setup
 * -----------
 * - kVA_SETUP_NBMAX_SECTIONFILTERS      : Number max active filters per ACS instance
 * - kVA_SETUP_NBMAX_FILTERS_PER_CHANNEL : Number max active filters per PID
 * - kVA_SETUP_NBMAX_PID_CHANNELS        : Number max PIDs recommended per ACS instance
 */
#define kVA_SETUP_NBMAX_SECTIONBUFFERS              64 // for just compile, hjlee3.  to be remove

/**
 * Number PIDs EMMs ACS has to filter per instance = 3
 */
#define _kVA_SETUP_NBPID_EMM_PER_INSTANCE 3
/**
 * Number PIDs ECMs ACS has to filter per instance = 5
 */
#define _kVA_SETUP_NBPID_ECM_PER_INSTANCE 5
/**
 * Number of section filters per EMM channels      = GA + DU + GH + FT + (U*NbCard) + (S*NbCard)
 *                                                 = 4 + (2*NbCard)
 *                                                 = 6 (for PurpleBox or BroadcastCardless projects)
 * - Hybrid solution here (NbSc = 1 and NbSsm = 1), so NbCard = 2
 */
#define _kVA_SETUP_NBSECTIONFILTERS_PER_EMM_CHANNEL  8
/**
 * Number of section filters per ECM channels      = 1
 */
#define _kVA_SETUP_NBSECTIONFILTERS_PER_ECM_CHANNEL  1
/**
 * Number max active filters per ACS instance
 */
#define kVA_SETUP_NBMAX_SECTIONFILTERS                                                    \
    ((_kVA_SETUP_NBSECTIONFILTERS_PER_EMM_CHANNEL * _kVA_SETUP_NBPID_EMM_PER_INSTANCE) + \
     (_kVA_SETUP_NBSECTIONFILTERS_PER_ECM_CHANNEL * _kVA_SETUP_NBPID_ECM_PER_INSTANCE))
/**
 * Number max active filters per PID
 */
#define kVA_SETUP_NBMAX_FILTERS_PER_CHANNEL         _kVA_SETUP_NBSECTIONFILTERS_PER_EMM_CHANNEL
/**
 * Number max PIDs recommended per ACS instance
 */
#define kVA_SETUP_NBMAX_PID_CHANNELS               (_kVA_SETUP_NBPID_EMM_PER_INSTANCE + _kVA_SETUP_NBPID_ECM_PER_INSTANCE)

/*
 * Network setup
 * ------------- */
/**
 * Multicast datagram max size
 */
#define kVA_SETUP_MULTICAST_DATAGRAM_MAX_SIZE       1536
/**
 * Minimum buffer size allocated for every Multicast connections
 */
#define kVA_SETUP_MULTICAST_MINIMUM_BUFFER_SIZE     (4 * 1024)
/**
 * Number max of multicast sockets for the STB
 */
#define kVA_SETUP_MAX_MULTICAST_SOCKETS             10
/**
 * Number max of unicast sockets for the STB
 */
#define kVA_SETUP_MAX_UNICAST_SOCKETS               10

#define kVA_SETUP_UNICAST_MINIMUM_BUFFER_SIZE       (4 * 1024) // for just compile, hjlee3.  to be remove

/**
 * Control Word processing type */
/* ---------------------------- */
/** @n Ask Viaccess for the value of your chipset (0 or 1).
 *  @n If you are using an old VA_SCHIP driver, please only set :
 *     define kVA_SETUP_CHIPSET_TYPE                2
 */
#define kVA_SETUP_CWPT                              0


// for just compile, hjlee3.  to be remove
#define kVA_SETUP_LOADER_DATA_SEGMENT_SIZE          (8 * 1024)
#define kVA_SETUP_SOFTWARE_SEGMENT_SIZE             (8 * 1024 * 1024)

/** @} */

#endif /* _VA_SETUP_H_ */
