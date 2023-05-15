/*
 * Copyright (c) 2011 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */
#if !defined ITK_REMINDER_H__
#define ITK_REMINDER_H__

/**
 * @file 
 *
 * iTK Reminder manager.
 * The Reminder manager allows a client to set, update, get and cancel a reminder.
 * The reminder is a native fucntion which notifies the user that an event
 * is starting, for exmaple with an on-screen pop-up menu. When the reminder has been 
 * triggered it shall be deleted.
 * 
 * VAST profile: The manager need support only a single reminder, hence a reminder identifier 
 * is not passed across the interface. The location string is a URI containing a DVB Triple and 
 * UTC Time/Duration as defined in the profile. The userData contains the event synopsis.
 *
 * South Africa: The manager must support multiple reminders for EIT schedule events.
 * The userData is not used in this profile and can be ignored.
 * The location string identifies the service index and the id of the event for which 
 * to set the reminder. The location string is NULL-terminated and is defined as follows:
 *
 * rec://svc/idx/{service_index};{event_id}
 *
 * Example: rec://svc/idx/122;5 (digits are decimal)
 */
 
#include "itk_types.h"


/**Interface to Reminders.
 */
typedef struct itk_reminder_t
{
    /**
     * Sets the reminder.
     * Sets the reminder for the event identified by the location string.  
     * See iTK User Guide for the format of this string.
     * @param thiz The reminder manager interface.
     * @param location The location of the event (NULL-terminated string).
     * @param userData User data to store with the reminder.
     * @param userDataLength The length of the user data.
     * @return ITKE_OK The reminder was set successfully.
     * @return ITKE_NOT_FOUND The service identified by the location string was not found.
     * @return ITKE_FAIL The reminder failed for some other reason.
     * @remark The location string and userData should be copied by the manager as they 
     * may not persist beyond the function call. In VAST profile, only a single reminder 
     * is stored - if a reminder already exists, the manager should overwrite it with the 
     * new location and userData and return ITKE_OK.
     * In South Africa profile, multiple reminders are stored, identified by the 
     * service index and event id passed in the location string. If a reminder for
     * the specified event is already set, the function must return ITKE_OK. The 
     * userData is not used and can be ignored in this profile.
     */
    itk_errcode_t (*set)( struct itk_reminder_t *thiz, const char *location, void *userData, size_t userDataLength );

    /**
     * Returns the user data length associated with the reminder.
     * @param thiz The reminder manager interface.
     * @return The length of the user data string passed to itk_reminder_t::setReminder()
     * or '0' if the reminder is not set, triggered or cancelled.
     * @remark Only used in VAST profile. Not required in South Africa profile.
     */
    size_t (*getUserDataLength)( struct itk_reminder_t *thiz );

    /**
     * Copies the userData associated with the reminder into the supplied buffer.
     * @param thiz The reminder manager interface.
     * @param userData Memory location to store user data.
     * @param userDataLen Size of memory location for user data.
     * @return The length of the reminder user data copied.
     * @remark Only used in VAST profile. Not required in South Africa profile.
     */
    size_t (*getUserData)( struct itk_reminder_t *thiz, void *userData, size_t userDataLen);

    /**
     * Cancels the reminder.
     * @param thiz The reminder manager interface.
     * @param location The location of the event (NULL-terminated string).
     * @return ITKE_OK The reminder was cancelled successfully.
     * @return ITKE_NOT_FOUND No reminder was set.
     * @return ITKE_FAIL The cancel failed for some other reason.
     * @remark The manager should free all the location and user data strings associated 
     * with the reminder. In VAST profile, the 'location' string will be NULL and can
     * be ignored. In South Africa profile, the 'location' string specifies which
     * reminder to cancel.
     */
    itk_errcode_t (*cancel)( struct itk_reminder_t *thiz, const char *location );

    /**
     * Checks if the reminder is set for the identified event.
     * @param thiz The reminder manager interface.
     * @param location The location of the event (NULL-terminated string).
     * @return ITKE_PRESENT A reminder exists for the event.
     * @return ITKE_NOT_PRESENT No reminder exists for the event.
     * @remark Only used in South Africa profile. Not required in VAST profile.
     */
    itk_present_t (*exists)( struct itk_reminder_t *thiz, const char *location );
} itk_reminder_t;

/**
 * @}
 */
#endif
