/**
**  @file ca_defsx.h
**
**  @brief
**    This file defines specific types and constants used by the
**    NagraVision CA software.
**
**  @ingroup GLB
**
**  $Revision: 1.6 $
**
**  COPYRIGHT:
**    2004 Nagravision S.A.
**
** HISTORY:
**   $Log: ca_defsx.h,v $
**   Revision 1.6  2009/09/17 11:29:10  bieber
**   PRMS ID: NONE
**   - Fixed typo in comments.
**
**   Revision 1.5  2009/08/28 13:49:06  bieber
**   PRMS ID: NONE
**   - Removed definition of TEmi
**
**   Revision 1.4  2009/08/03 12:41:52  bieber
**   PRMS ID: NONE
**   - Added TEmi
**
**   Revision 1.3  2007/03/26 12:15:08  aceriali
**   PRMS ID: 39161
**   - Moved (from tlsdate.h) TCalendarTime TCaDuration and TUnixDate
**
**   Revision 1.2  2004/05/28 14:45:06  erard
**   PRMS ID: NONE
**   - Clean up.
**
**   Revision 1.1  2004/05/28 14:32:17  robyr
**   PRMS ID: NONE
**   - First issue.
**
**/

#ifndef CA_DEFSX_H
#define CA_DEFSX_H

#include "ca_defs.h"

/**
 *  @brief
 *    Transport session identifier.
 *
 *    An advanced device may manage more than one input stream. For example,
 *    a box may have a stream feed coming from a hard drive (DVR box) or a
 *    second tuner (for the PIP feature). <BR>
 *    In these cases, it is necessary to identify each of these streams
 *    individually. We will call these streams transport session. They are
 *    identified with a transport session identifier provided by the decoder
 *    manufacturer.<BR>
 *    The transport session identifier comes from this interface and is passed
 *    as is to the demultiplexer and descrambler drivers.<BR>
 *    ECMs as well as EMMs can be in any transport session
 *
 *  @see TRANSPORT_SESSION_ID_INVALID
*/
typedef TUnsignedInt32 TTransportSessionId;

/**
 *  @brief
 *    Defines an invalid transport session identifier.
 *
 *  @see  TTransportSessionId
*/
#define TRANSPORT_SESSION_ID_INVALID  ((TTransportSessionId) -1)

/**
 *  @brief
 *    Calendar time structure.
*/
typedef struct
{
  TUnsignedInt8  year;
    /**<  Years since 1900.
    */
  TUnsignedInt8  month;
    /**<  Month of the year (1 to 12).
    */
  TUnsignedInt8  day;
    /**<  Day of the month (1 to 31).
    */
  TUnsignedInt8  hour;
    /**<  Hours since midnight (0 to 23).
    */
  TUnsignedInt8  minute;
    /**<  Minute after the hour (0 to 59).
    */
  TUnsignedInt8  second;
    /**<  Second after the minute (0 to 59).
    */
} TCalendarTime;

/**
 *  @brief
 *    A duration in seconds.
*/
typedef TUnsignedInt32 TCaDuration;

/**
 * @brief
 *   Number of seconds since 1st of January 1970, 00:00:00
*/
typedef TUnsignedInt32    TUnixDate;

#endif /* CA_DEFSX_H */

/* end of file */
