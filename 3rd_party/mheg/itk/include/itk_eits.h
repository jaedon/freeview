/*
 * Copyright (c) 2012 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */
#if !defined ITK_EVENTINFO_H__
#define ITK_EVENTINFO_H__

/**
 * Component type, as siganlled by the EITs or EITp/f Component Descriptor
 */
typedef enum itk_component_t
{
    ITK_COMPONENT_SUBS,
    ITK_COMPONENT_AUDIO_DESC,
    ITK_COMPONENT_ALT_LANG,
    ITK_COMPONENT_DOLBY,
    ITK_COMPONENT_HD
} itk_component_t;


struct itk_service_event_t;



/**
 * @file 
 * Scheduled Event Info Iterator
 */

/**
 * @ingroup service
 * @defgroup eventinfo Scheduled Event Info Iterator
 * @{
 */

typedef struct itk_eits_iterator_t
{
    /** Moves the scheduled event info iterator to the next event.
     *  @param[in]  thiz The eits iterator.
     *  @return     The result of the operation. ITK_VALID if successful ITK_NOT_VALID if unsuccesful.
     *
     *  @remark On returning from the call the iterator should have been moved to the next
     *  cached event in the EITs data base. This may the next event chronologically or, if the  
     *  next chronological event has not been cached, it may at a time and date further ahead. 
     *  If no further events are found then the calls is unsuccesful and shall return ITK_NOT_VALID.
     */
    itk_validation_t (*nextEvent)( struct itk_eits_iterator_t *thiz );

    /** Returns the event id of the event currently referenced by the iterator.
     *  @param[in]  thiz The eits iterator.
     *  @param[out] eventId Reference to variable to store the eventId.
     *  @return     The result of the operation. ITK_VALID if successful ITK_NOT_VALID if unsuccesful.
     */
    itk_validation_t (*getEventId)( struct itk_eits_iterator_t *thiz, itk_uint16_t *eventId );

    /** Returns the event info of the event currently referenced by the iterator.
     *  @param[in]  thiz The eits iterator.
     *  @param[out] eventId Reference to variable to store the event info.
     *  @return     The result of the operation. ITK_VALID if successful ITK_NOT_VALID if unsuccesful.
     */
    itk_validation_t (*getEventInfo)( struct itk_eits_iterator_t *thiz, struct itk_service_event_t *info );

    /** Returns if a given component is signalled for the event currently referenced by the iterator.
     *  @param[in]  thiz The eits iterator.
     *  @return     The result of the operation. ITK_PRESENT if the component is signalled, ITK_NOT_PRESENT if not.
     *
     *  @remark See EN300468, Clause 6.2.8 Component Descriptors.
     */
    itk_present_t (*getComponentInfo)( struct itk_eits_iterator_t *thiz, itk_component_t component);

    /** Returns the number of component descriptors signalled by the event currently referenced by the iterator.
     *  @param[in]  thiz The eits iterator.
     *  @return     The number of descriptors.
     *
     *  @remark See EN300468. The data returned is extracted from Component Descriptors as defined in clause 6.2.8.
     */
    itk_uint16_t (*getNumComponentDescs)( struct itk_eits_iterator_t *thiz);

    /** Returns space separated lists of char hex values in ascii representing the values of stream_content 
     *  and component_type for each component of the event.
     *  @param[in]  thiz The eits iterator.
     *  @param[out] streamContent A pointer to store the stream_content list.
     *  @param[in]  scLength The length of stream content buffer.
     *  @param[out] componentType A pointer to store the component type list.
     *  @param[in]  ctLength The length of component type buffer.
     *  @return     The result of the operation. ITK_VALID if successful ITK_NOT_VALID if unsuccesful.
     *
     *  The function returns two lists extracted from Component Descriptors as defined in EN300468 clause 6.2.8. 
     *  The stream content list contains a space separated list of 1 char hex values in ascii representing the 
     *  values of stream_content for each component of the event. The component_type list contains a space 
     *  separated list of 2 char hex values in ascii representing the values of component_type for each 
     *  component of the event.
     *
     *  @remark See EN300468. The data returned is extracted from Component Descriptors as defined in clause 6.2.8.
     */
    itk_validation_t (*getComponentDescs)( struct itk_eits_iterator_t *thiz, char *streamContent, itk_uint16_t scLength,
                                char *componentType, itk_uint16_t ctLength);

    /** Releases the scheduled event info iterator.
     *  @param[in]  thiz The eits iterator.
     */
    void (*release)( struct itk_eits_iterator_t *thiz );
} itk_eits_iterator_t;

/** 
 * @}
 */
#endif
