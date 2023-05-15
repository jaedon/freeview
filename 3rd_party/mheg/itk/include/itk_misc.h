/*
 * Copyright (c) 2007 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */
#if !defined ITK_MISC_H__
#define ITK_MISC_H__

#include "itk_types.h"
/**
 * @file
 * Miscellaneous Host Functions
 */

/**
 * @ingroup host
 * @defgroup misc Miscellaneous Host Functions
 * @{
 */

/**
 * Defines the language code array (ISO 3 character code + '\0')
 */
typedef char itk_lang_code_t[4];

/**
 * Date and time structure with local offset.
 * Date and time is provided in UTC and local time can be calculated by adding 
 * local offset to it.
 */
typedef struct itk_date_t
{
    itk_uint8_t hour;   /**< Hour, Midnight = 0 */
    itk_uint8_t minute; /**< Minute */
    itk_uint8_t second; /**< Second */
    itk_uint8_t year;   /**< Year, 1900 = 0 */
    itk_uint8_t month;  /**< Month, January = 0 */
    itk_uint8_t day;    /**< Day */
    itk_int16_t offset; /**< Local offset in Minutes (signed) as signalled in TOT */
} itk_date_t;

/**
 * User settings that can be changed dynamically, changes to these will
 * cause a callback to be invoked
 */
typedef enum itk_user_setting_t
{
    ITK_USER_SETTING_SUBTITLES = 0, /**< Subtitles */
    ITK_USER_SETTING_AUDIO_DESC,    /**< Audio description */
    ITK_USER_SETTING_PIN,           /**< Not used */
    ITK_USER_SETTING_SMARTCARD      /**< Not used */
} itk_user_setting_t;

/** 
 * The various possible values for PIN settings
 */
typedef enum itk_pin_setting_t
{
    ITK_PIN_NOT_SUPPORTED = 0,  /**< The receiver has no support for PIN entry */
    ITK_PIN_DISABLED,           /**< PIN entry is supported but disabled */
    ITK_PIN_RESTRICTED,         /**< PIN entry is supported and enabled for restricted content */
    ITK_PIN_ALWAYS              /**< PIN entry is supported and enabled for all (streamed) content */
} itk_pin_setting_t;

/**
 * Prototype for a user settings callback handler function
 */
typedef void (*itk_user_setting_callback_t)(void *context, itk_user_setting_t setting);

/**
 * Miscellaneous functions jump table
 */
typedef struct itk_misc_t
{
    /**
     * Gets the current date and time.
     *
     * @param[in] thiz The host
     *
     * @return The current date and time. The date is expressed as UTC and the itk_date_t::offset value is that
     * signalled in the TOT for the present country/region.
     * 
     */
    itk_date_t (*getDate)(struct itk_misc_t *thiz);
    
    /**
     * Returns the number of ms since the host platform was powered up.
     *
     * @note The returned value must not be derived from the system time and must increment
	 *       continuously. It must not loop with the time of recorded transport streams.
     *
     * @param[in] thiz The host
     *
     * @return the number of ms since power on
     */
    itk_uint32_t (*getUptime)(struct itk_misc_t *thiz);

    /**
     * Returns a pointer to a host identification string
     *
     * The host version string is in the following format:
     * "mmmcccvvv" with a terminating '\\0', where 
     * - mmm is a code uniquely identifying the manufacturer
     * - ccc is a manufacturer specific product code
     * - vvv is a version of software for the product
     * 
     * The product and version codes have no specific meaning and are 
     * only used to identify particular system revisions to the extent
     * where they can affect the operation of an MHEG application acting 
     * on the system (e.g. changes of screen size on a particular chassis
     * are not identified but different processors or middleware stacks 
     * should be).
     *
     * @note The manufacturer code must be a globally unique identifier.
     *       Normally, manufacturer codes are issued by the authority
     *       managing conformance testing, e.g. DTG Testing Ltd in the UK.
     *
     * @param[in] thiz The host
     *
     * @return A const pointer to a host id string
     */
    const char *(*getIdString)(struct itk_misc_t *thiz);

    /**
     * Returns a receiver-specific unique identification string.
     *
     * For details regarding the id string refer to the iTK User Guide.
     *
     * @param[in] thiz The host
     *
     * @return A const pointer to a '\0'-terminated id string or NULL
     * if the id string is not available or has been tampered with. 
     */
    const char *(*getUniqueIdString)(struct itk_misc_t *thiz);

    /**
     * Retrieves the preferred language for application, audio and subtitles.
     *
     * The host provides the preferred languages as ISO 3 character codes
     * ('\0' terminated). If no preferred language is available, "und" shall be used.
     *
     * @param[in] thiz               The host
     * @param[out] applicationLang   Buffer for preferred MHEG application language
     * @param[out] audioLang         Buffer for preferred audio language (ZA only; is NULL otherwise)
     * @param[out] subtitleLang      Buffer for preferred subtitle language (ZA only; is NULL otherwise)
     */
    void (*getPreferredLanguage)(struct itk_misc_t *thiz, 
                                        itk_lang_code_t applicationLang, 
                                        itk_lang_code_t audioLang, 
                                        itk_lang_code_t subtitleLang);

    /**
     * Sets the preferred language for audio & subtitles.
     *
     * Provides the preferred audio and subtitle language to the host so that 
     * it can present the correct language acording to the user preference. 
     * Language codes are ISO 3 character codes with '\0' terminated. 
     *
     * @param[in] thiz              The host
     * @param[in] audioLang         Preferred audio language
     * @param[in] subtitleLang      Preferred subtitle language
     */
    void (*setPreferredLanguage)(struct itk_misc_t *thiz, itk_lang_code_t audioLang, itk_lang_code_t subtitleLang);

    /**
     * Adds a callback to invoke when a viewer setting changes
     *
     * @param[in] thiz The host
     * @param[in] callback The callback function to link to
     * @param[in] callbackContext A context pointer to send when the callback is
     *            invoked
     *
     * @return ITKE_OK if the callback is registered
     * @return ITKE_NO_RESOURCE if there is already a callback registered
     */
    itk_errcode_t (*addSettingsCallback)(struct itk_misc_t *thiz, 
                                         itk_user_setting_callback_t callback,
                                         void *callbackContext);

    /**
     * Removes a callback to invoke when a user setting changes
     *
     * @param[in] thiz The host
     */
    void (*removeSettingsCallback)(struct itk_misc_t *thiz);

    /**
     * Identifies if the host has subtitles enabled
     *
     * This is not affected by the presence of subtitles in the service,
     * only that the viewer wishes to see them if they are present
     *
     * @param[in] thiz The host
     *
     * @return ITK_ENABLE if the host has subtitles enabled
     */
    itk_control_t (*getSubtitleSetting)(struct itk_misc_t *thiz);

    /**
     * Identifies if the host has Audio Description enabled
     *
     * This is not affected by the presence of Audio Description in the service,
     * only that the viewer wishes to see hear if they are present
     *
     * @param[in] thiz The host
     *
     * @return ITK_ENABLE if the host has Audio Description enabled
     */
    itk_control_t (*getADSetting)(struct itk_misc_t *thiz);

    /**
     * Identifies if the host has a Viewer Restriction PIN enabled
     *
     * @param[in] thiz The host
     *
     * @return The itk_pin_setting_t value that most accurately represents
     *         the current PIN setting.
     */
    itk_pin_setting_t (*getPINSetting)(struct itk_misc_t *thiz);

    /**
     * Confirms if a PIN string is equivalent to one stored in the host
     *
     * @param[in] thiz The host
     * @param[in] pin The '\0' terminated string containing the PIN
     *
     * @return ITK_VALID if the PIN is equivalent to the host's stored PIN
     * @return ITK_NOT_VALID if the PIN is not equivalent (including if no PIN is held)
     */
    itk_validation_t (*isValidPIN)(struct itk_misc_t *thiz, const char *pin);

    /**
     * Confirms that an item of content can be presented to the viewer based on
     * the restriction advice provided. The method of confirming that the content
     * may be presented is not defined but shall not cause the MHEG application 
     * to be terminated. As an example if the receiver implements a "parental 
     * control" PIN the host could display a dialogue to indicate that the content
     * is restricted, using the restriction string to indicate why it is restricted,
     * and ask the viewer to enter the PIN to allow presentation. If the PIN is not
     * provided then the function returns ITK_NOT_VALID. Receivers that do not 
     * implement any parental control mechanism the resident shall always return 
     * ITK_VALID. The restriction parameter contains a string that may be presented 
     * to the viewer. The string shall use the UTF-8 representation of character 
     * code. Any mark-up or other unrecognised bytes in the string may be ignored 
     * and the string shall be limited to 50 characters.
     * @param[in] thiz The host.
     * @param[in] restriction String indicating why content is restricted.
     * @return ITK_VALID if the content is acceptable e.g. valid PIN entered.
     * @return ITK_NOT_VALID if the restriction is NOT acceptable e.g. valid PIN is not entered.
     */
    itk_validation_t (*promptForGuidance)(struct itk_misc_t *thiz, const char *restriction);

    /** 
     * Initiates a new application with the specified arguments.
     * 
     * The function initiates execution of an application of the specified appType
     * at the specified appReference. The meaning of the appType field is currently
     * undefined.
     * Arguments are passed to the application as an array of strings whose
     * size is specified by argCount. See User Guide for details.
     * 
     * @param[in] thiz          The host.
     * @param[in] appType       Application type (TBD).
     * @param[in] appReference  Application reference.
     * @param[in] argCount      The number of arguments in args[].
     * @param[in] args          Array of strings.
     * @return ITKE_OK if the application is found and its launch has been initiated.
     * @return ITKE_NOT_FOUND if the application cannot be found.
     * @return ITKE_FAIL if the application cannot be launched for other reasons.
     * @note the args pointer does not persist once the function has returned.
     */
    itk_errcode_t (*launchApplication)(struct itk_misc_t *thiz, 
                                        itk_int32_t appType,
                                        const itk_uint8_t *appReference,
                                        itk_int32_t argCount,
                                        itk_uint8_t *args[]);    

    /**
     * Confirms if a given mime type represents an application that the host
     * platform can launch via launchApplication(). 
     *
     * @param[in] thiz The host
     * @param[in] mimeType The '\0' terminated string containing the mime type
     *
     * @return ITK_VALID if the mime type is a compatible application type 
     * @return ITK_NOT_VALID if the mime type is not a compatible application type.
     */
    itk_validation_t (*isValidAppMimeType)(struct itk_misc_t *thiz, const char *mimeType );

    /**
     * Gets host settings with respect to the service info banner displayed on channel
     * changes etc. South Africa profile only.
     *
     * @param[in] thiz The host
     * @param[out] transparent The transparency level of the info banner as a percentage
     * from 0 (solid) to 100 (fully transparent).
     * @param[out] timeOut The number of seconds the banner is displayed for before timing out
     * and being removed.
     * @param[out] language A four byte string representing the language used for the OSD banner.
     * This is the 3 byte ISO 639-2 language code plus a null terminator byte. The char* buffer
     * pointed to shall be at least 4 bytes in size.
     * @return ITKE_OK if the output values have been set appropriately. Any other appropriate 
     * error code if not.
     */
    itk_errcode_t (*getBannerSettings)(struct itk_misc_t *thiz, itk_int32_t *transparency,
        itk_int32_t *timeOut, char *language );

    /**
     * Provides support for extensions to Engine Support Strings. The function passes a free 
     * format string to the Host which indicates if the string is understood and the feature
     * is supported.
     * @param[in] thiz      The host
     * @param[in] feature   A free format string identifying the feature
     * @param[out] known    ITK_VALID if the feature string is understood by the host, 
     *                      otherwise ITK_NOT_VALID.
     * @param[out] support  ITK_VALID if the feature identified by the string is supported,
     *                      otherwise ITK_NOT_VALID.
     *
     * @return ITKE_OK if the output parameters have been set appropriately.
     * @return ITK_NOT_OK if output parameters have NOT been set.
     */
    itk_errcode_t (*queryHostSupportString)(struct itk_misc_t *thiz,
                                            const char *feature,
                                            itk_validation_t *known,
                                            itk_validation_t *supported);
} itk_misc_t;

/**
 * @}
 */
#endif
