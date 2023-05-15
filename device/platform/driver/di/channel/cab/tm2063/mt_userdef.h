/*****************************************************************************
**
**  Name: mt_userdef.h
**
**  Copyright 2006-2008 Microtune, Inc. All Rights Reserved
**
**  This source code file contains confidential information and/or trade
**  secrets of Microtune, Inc. or its affiliates and is subject to the
**  terms of your confidentiality agreement with Microtune, Inc. or one of
**  its affiliates, as applicable and the terms of the end-user software
**  license agreement (EULA).
**
**  Protected by US Patent 7,035,614 and additional patents pending or filed.
**
**  BY INSTALLING, COPYING, OR USING THE MICROTUNE SOFTWARE, YOU AGREE TO BE
**  BOUND BY ALL OF THE TERMS AND CONDITIONS OF THE MICROTUNE, INC. END-USER
**  SOFTWARE LICENSE AGREEMENT (EULA), INCLUDING BUT NOT LIMITED TO, THE
**  CONFIDENTIALITY OBLIGATIONS DESCRIBED THEREIN. IF YOU DO NOT AGREE, DO NOT
**  INSTALL, COPY, OR USE THE MICROTUNE SOFTWARE AND IMMEDIATELY RETURN OR
**  DESTROY ANY COPIES OF MICROTUNE SOFTWARE OR CONFIDENTIAL INFORMATION THAT
**  YOU HAVE IN YOUR POSSESSION.
*****************************************************************************/

/*****************************************************************************
**
**  Name: mt_userdef.h
**
**  Description:    User-defined data types needed by MicroTuner source code.
**
**                  Customers must provide the code for these functions
**                  in the file "mt_userdef.c".
**
**                  Customers must verify that the typedef's in the 
**                  "Data Types" section are correct for their platform.
**
**  Functions
**  Requiring
**  Implementation: MT_WriteSub
**                  MT_ReadSub
**                  MT_Sleep
**
**  References:     None
**
**  Exports:        None
**
**  CVS ID:         $Id: mt_userdef.h,v 1.2 2008/11/05 13:46:20 software Exp $
**  CVS Source:     $Source: /export/vol0/cvsroot/software/tuners/MT2063/mt_userdef.h,v $
**	               
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   03-25-2004    DAD    Original
**   082   12-06-2004    JWS    Multi-tuner support - requires MTxxxx_CNT 
**                              declarations
**   N/A   04-13-2007    JWS    Added Signed 8-bit type - S8Data
**   200   08-07-2008    JWS    Included definition of DECT avoid constants.
**
*****************************************************************************/
#if !defined( __MT_USERDEF_H )
#define __MT_USERDEF_H

#include "mt_errordef.h"

#if defined( __cplusplus )     
extern "C"                     /* Use "C" external linkage                  */
{
#endif

/*
**  Data Types
*/
typedef unsigned char   U8Data;         /*  type corresponds to 8 bits      */
typedef signed char     S8Data;         /*  type corresponds to 8 bits      */
typedef unsigned int    UData_t;        /*  type must be at least 32 bits   */
typedef int             SData_t;        /*  type must be at least 32 bits   */
typedef void *          Handle_t;       /*  memory pointer type             */
typedef double          FData_t;        /*  floating point data type        */

#define MAX_UDATA         (4294967295)  /*  max value storable in UData_t   */

/*
** Define an MTxxxx_CNT macro for each type of tuner that will be built
** into your application (e.g., MT2121, MT2060). MT_TUNER_CNT
** must be set to the SUM of all of the MTxxxx_CNT macros.
**
** #define MT2050_CNT  (1)
** #define MT2060_CNT  (1)
** #define MT2111_CNT  (1)
** #define MT2121_CNT  (3)
*/
 #define MT2063_CNT  (2)


#if !defined( MT_TUNER_CNT )
#define MT_TUNER_CNT               (2)  /*  total num of MicroTuner tuners  */
#endif


/*
** DECT Frequency Avoidance:
** These constants are used to avoid interference from DECT frequencies.
*/
typedef enum
{
    MT_NO_DECT_AVOIDANCE = 0,              /* Do not create DECT exclusion zones.     */
    MT_AVOID_US_DECT     = 0x00000001,     /* Avoid US DECT frequencies.              */
    MT_AVOID_EURO_DECT   = 0x00000002,     /* Avoid European DECT frequencies.        */
    MT_AVOID_BOTH                          /* Avoid both regions. Not typically used. */
} MT_DECT_Avoid_Type;



/*
**  Optional user-defined Error/Info Codes  (examples below)
**
**  This is the area where you can define user-specific error/info return
**  codes to be returned by any of the functions you are responsible for
**  writing such as MT_WriteSub() and MT_ReadSub.  There are four bits
**  available in the status field for your use.  When set, these
**  bits will be returned in the status word returned by any tuner driver
**  call.  If you OR in the MT_ERROR bit as well, the tuner driver code
**  will treat the code as an error.
**
**  The following are a few examples of errors you can provide.
**
**  Example 1:
**  You might check to see the hUserData handle is correct and issue 
**  MY_USERDATA_INVALID which would be defined like this:
**
**  #define MY_USERDATA_INVALID  (MT_USER_ERROR | MT_USER_DEFINED1)
**
**
**  Example 2:
**  You might be able to provide more descriptive two-wire bus errors:
**
**  #define NO_ACK   (MT_USER_ERROR | MT_USER_DEFINED1)
**  #define NO_NACK  (MT_USER_ERROR | MT_USER_DEFINED2)
**  #define BUS_BUSY (MT_USER_ERROR | MT_USER_DEFINED3)
**
**
**  Example 3:
**  You can also provide information (non-error) feedback:
**
**  #define MY_INFO_1   (MT_USER_DEFINED1)
**
**
**  Example 4:
**  You can combine the fields together to make a multi-bit field.
**  This one can provide the tuner number based off of the addr
**  passed to MT_WriteSub or MT_ReadSub.  It assumes that
**  MT_USER_DEFINED4 through MT_USER_DEFINED1 are contiguously. If
**  TUNER_NUM were OR'ed into the status word on an error, you could
**  use this to identify which tuner had the problem (and whether it
**  was during a read or write operation).
**
**  #define TUNER_NUM  ((addr & 0x07) >> 1) << MT_USER_SHIFT
**
*/

/*****************************************************************************
**
**  Name: MT_WriteSub
**
**  Description:    Write values to device using a two-wire serial bus.
**
**  Parameters:     hUserData  - User-specific I/O parameter that was
**                               passed to tuner's Open function.
**                  addr       - device serial bus address  (value passed
**                               as parameter to MTxxxx_Open)
**                  subAddress - serial bus sub-address (Register Address)
**                  pData      - pointer to the Data to be written to the 
**                               device 
**                  cnt        - number of bytes/registers to be written
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_COMM_ERR      - Serial bus communications error
**                      user-defined
**
**  Notes:          This is a callback function that is called from the
**                  the tuning algorithm.  You MUST provide code for this
**                  function to write data using the tuner's 2-wire serial 
**                  bus.
**
**                  The hUserData parameter is a user-specific argument.
**                  If additional arguments are needed for the user's
**                  serial bus read/write functions, this argument can be
**                  used to supply the necessary information.
**                  The hUserData parameter is initialized in the tuner's Open
**                  function.
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   03-25-2004    DAD    Original
**
*****************************************************************************/
UData_t MT_WriteSub(Handle_t hUserData, 
                    UData_t addr, 
                    U8Data subAddress, 
                    U8Data *pData, 
                    UData_t cnt);


/*****************************************************************************
**
**  Name: MT_ReadSub
**
**  Description:    Read values from device using a two-wire serial bus.
**
**  Parameters:     hUserData  - User-specific I/O parameter that was
**                               passed to tuner's Open function.
**                  addr       - device serial bus address  (value passed
**                               as parameter to MTxxxx_Open)
**                  subAddress - serial bus sub-address (Register Address)
**                  pData      - pointer to the Data to be written to the 
**                               device 
**                  cnt        - number of bytes/registers to be written
**
**  Returns:        status:
**                      MT_OK            - No errors
**                      MT_COMM_ERR      - Serial bus communications error
**                      user-defined
**
**  Notes:          This is a callback function that is called from the
**                  the tuning algorithm.  You MUST provide code for this
**                  function to read data using the tuner's 2-wire serial 
**                  bus.
**
**                  The hUserData parameter is a user-specific argument.
**                  If additional arguments are needed for the user's
**                  serial bus read/write functions, this argument can be
**                  used to supply the necessary information.
**                  The hUserData parameter is initialized in the tuner's Open
**                  function.
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   03-25-2004    DAD    Original
**
*****************************************************************************/
UData_t MT_ReadSub(Handle_t hUserData, 
                   UData_t addr, 
                   U8Data subAddress, 
                   U8Data *pData, 
                   UData_t cnt);


/*****************************************************************************
**
**  Name: MT_Sleep
**
**  Description:    Delay execution for "nMinDelayTime" milliseconds
**
**  Parameters:     hUserData     - User-specific I/O parameter that was
**                                  passed to tuner's Open function.
**                  nMinDelayTime - Delay time in milliseconds
**
**  Returns:        None.
**
**  Notes:          This is a callback function that is called from the
**                  the tuning algorithm.  You MUST provide code that
**                  blocks execution for the specified period of time. 
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   03-25-2004    DAD    Original
**
*****************************************************************************/
void MT_Sleep(Handle_t hUserData,
              UData_t nMinDelayTime);


#if defined(MT2060_CNT)
#if MT2060_CNT > 0
/*****************************************************************************
**
**  Name: MT_TunerGain  (for MT2060 only)
**
**  Description:    Measure the relative tuner gain using the demodulator
**
**  Parameters:     hUserData  - User-specific I/O parameter that was
**                               passed to tuner's Open function.
**                  pMeas      - Tuner gain (1/100 of dB scale).
**                               ie. 1234 = 12.34 (dB)
**
**  Returns:        status:
**                      MT_OK  - No errors
**                      user-defined errors could be set
**
**  Notes:          This is a callback function that is called from the
**                  the 1st IF location routine.  You MUST provide
**                  code that measures the relative tuner gain in a dB
**                  (not linear) scale.  The return value is an integer
**                  value scaled to 1/100 of a dB.
**
**  Revision History:
**
**   SCR      Date      Author  Description
**  -------------------------------------------------------------------------
**   N/A   06-16-2004    DAD    Original
**   N/A   11-30-2004    DAD    Renamed from MT_DemodInputPower.  This name
**                              better describes what this function does.
**
*****************************************************************************/
UData_t MT_TunerGain(Handle_t hUserData,
                     SData_t* pMeas);
#endif
#endif

#if defined( __cplusplus )     
}
#endif

#endif
