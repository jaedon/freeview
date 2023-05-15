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
 * File Name  : event.h
 *
 * Description: 
 *   Event mechanism that supports simultaneous dispatching of events
 *   from several input sources and timers.
 *----------------------------------------------------------------------*
 * $Revision: 1.3 $
 *
 * $Id: event.h,v 1.3 2005/08/01 16:20:04 broadcom Exp $
 *
 * $Log: event.h,v $
 * Revision 1.3  2005/08/01 16:20:04  broadcom
 *
 * define closeAllFds() function.
 *
 * Revision 1.2  2005/04/28 22:29:42  dmounday
 * Added ACS ConnReq
 *
 * Revision 1.8  2004/05/13 19:43:32  dmounday
 * Changes for generalized TBOX conditional.
 * Misc. changes to log statements.
 * Changed return from settimer to a int inplace of void.
 *
 * Revision 1.7  2003/10/28 12:24:56  jan
 * fixed timer issues
 *
 * Revision 1.6  2003/09/10 16:10:05  jan
 * the gw software now support transparent proxying http-https for user settings
 *
 * Revision 1.5  2003/04/01 09:46:04  sparud
 * NOW proxy.c compiles again
 *
 * Revision 1.4  2003/02/12 07:11:46  pelle
 * Added write/exception listeners too.
 *
 * Revision 1.3  2002/12/02 10:50:08  sparud
 * added synchronized notification
 *
 * Revision 1.2  2002/11/29 13:48:56  sparud
 * added notification/callback support to events
 *
 * Revision 1.1  2002/11/25 13:17:13  pelle
 * added cli3 and ttftpd applications
 *
 *----------------------------------------------------------------------*/

#ifndef EVENT_H
#define EVENT_H

/**********************************************************************
 * typedefs
 *********************************************************************/

/*----------------------------------------------------------------------
 * type of listener, see select() for more information
 */
typedef enum {
  iListener_Read,
  iListener_Write,
  iListener_Except,
  iListener_ReadWrite,
  iListener_Max=0x7fffffff
} tListenerType;


/*
 * listeners are all on form
 *    void f(void *) {}
 */
typedef void (*EventHandler)(void*);

/**********************************************************************
 * public functions
 *********************************************************************/

extern void stepTime(struct timeval *tv);

extern void stopTimer (EventHandler func, void *handle);
extern int  checkTimer(EventHandler func, void *handle);
extern int  setTimer  (EventHandler func, void *handle, int ms);

extern void stopListener(int fd);
extern void setListener (int fd, EventHandler func, void* handle);
extern void setListenerType(int fd, EventHandler func, void* handle, tListenerType type);

extern void stopCallback(void *target, EventHandler func, void *handle);
extern void setCallback(void *target, EventHandler func, void *handle);
extern void notifyCallbacks(void *target);
extern void notifyCallbacksAndWait(void *target);

extern void eventLoop(void);

extern void closeAllFds(void);


#endif
