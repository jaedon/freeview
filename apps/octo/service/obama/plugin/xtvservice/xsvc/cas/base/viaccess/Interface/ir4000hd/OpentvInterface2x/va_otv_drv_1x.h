/*
    COPYRIGHT VIACCESS S.A.
    date        : 06/02/2006
    file        : va_otv_drv.h
    Description : It is the Viaccess'API of CAS functionalities

    $Revision: 1 $
*/

#ifndef _VA_OTV_DRV_H
#define _VA_OTV_DRV_H


#include "opentvx.h"



#ifdef VAOPT_CA_MANAGER_INTEFACE

typedef enum
{
    CA_PROGRAM_SWITCH_OFF = PROGRAM_SWITCH_OFF,
    CA_PROGRAM_SWITCH_ON  = PROGRAM_SWITCH_ON ,
    CA_PROGRAM_UPDATE     = PROGRAM_UPDATE    ,
    CA_STREAM_REMOVED     = STREAM_REMOVED    ,
    CA_STREAM_ADDED       = STREAM_ADDED      ,
    CA_STREAM_UPDATE      = STREAM_UPDATE     ,
    CA_CAT_UPDATE         = CAT_UPDATE        ,
    CA_STREAM_QUERY
}   CA_program_cond_t;

typedef int16 CA_handle_t;

typedef enum
{
    CA_STREAM_ERROR                             = STREAM_ERROR  ,
    CA_STREAM_WAITING                           = STREAM_WAITING,
    CA_STREAM_RUNNING                           = STREAM_RUNNING,
    CA_STREAM_WAITING_FOR_USER_AGREEMENT                        ,
    CA_STREAM_INFO_NO_CA_DESCRIPTOR                             ,
    CA_STREAM_INFO_VIACCESS_NOT_APPLICABLE                      ,
    CA_STREAM_INFO_NO_RIGHT                                     ,
    CA_STREAM_ACCESS_INTO_PREVIEW_MODE                          ,
    CA_STREAM_ACCESS_GEOGRAPHICAL_BLACKOUT_MODE                 ,
    CA_STREAM_MAY_BE_DESCRAMBLED                                ,
    CA_STREAM_CARD_NEEDED
}   CA_answer_t;

#endif /* VAOPT_CA_MANAGER_INTEFACE */



/*======================================================================*/
/*	Viaccess external constants and macros											*/
/*======================================================================*/

/*** general ***/

#define GOOD                    (0)
#define BAD                     (-1)


#endif /* _VA_OTV_DRV_H */
