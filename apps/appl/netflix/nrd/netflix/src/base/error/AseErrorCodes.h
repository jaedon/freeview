/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef ASE_ERROR_CODES
#define ASE_ERROR_CODES

// *************************************************************************
// If you add new codes here, please add them to aseerrorcodes.js as well
// *************************************************************************

/** Error codes.
 *
 *  +---------------------------+---------------------------------+
 *  | Modules                   | Range                           |
 *  +---------------------------+---------------------------------+
 *  | common                    | 0 ~ - 49                        |
 *  +---------------------------+---------------------------------+
 *  | Async HTTP Library        | -50 ~ -99                       |
 *  +---------------------------+---------------------------------+
 *  | HTTP Request Manager      | -100 ~ -149                     |
 *  +---------------------------+---------------------------------+
 *  | Streaming Heuristics      | -150 ~ -199                     |
 *  +---------------------------+---------------------------------+
 *  | Media Presentation Tree   | -200 ~ -299                     |
 *  +---------------------------+---------------------------------+
 *  | Domain Selector           | -300 ~ -399                     |
 *  +---------------------------+---------------------------------+
 *  | Media File Factory        | -400 ~ -499                     |
 *  +---------------------------+---------------------------------+
 */

typedef int AseErrorCode;

/** Common error codes. */
#define AS_NO_ERROR             (0)
#define AS_UNKNOWN_ERROR        (AS_NO_ERROR - 1) // this one is duplicated in network.js
#define AS_NOT_ENOUGH_MEMORY    (AS_UNKNOWN_ERROR - 1)
#define AS_NOT_FOUND            (AS_NOT_ENOUGH_MEMORY - 1)
#define AS_INVALID_VALUE        (AS_NOT_FOUND - 1)
#define AS_NOT_ALlOWED          (AS_INVALID_VALUE - 1)
#define AS_DEAD_LOCK            (AS_NOT_ALlOWED - 1)
#define AS_PERMISSION_ERROR     (AS_DEAD_LOCK - 1)
#define AS_BUSY                 (AS_PERMISSION_ERROR - 1)
#define AS_NOT_SUPPORTED        (AS_BUSY - 1)
#define AS_AGAIN                (AS_NOT_SUPPORTED - 1)
#define AS_IN_PROGRESS          (AS_AGAIN - 1)
#define AS_PENDING              (AS_IN_PROGRESS - 1)
#define AS_TIMEOUT              (AS_PENDING - 1)
#define AS_INTERRUPTED          (AS_TIMEOUT - 1)
#define AS_PARSE_ERROR          (AS_INTERRUPTED - 1)
#define AS_MORE                 (AS_PARSE_ERROR - 1)
#define AS_STALE_CREDENTIALS    (AS_MORE - 1)
#define AS_INIT_FAILURE         (AS_STALE_CREDENTIALS - 1)
#define AS_DRM_FAILURE          (AS_INIT_FAILURE - 1)
#define AS_COMPRESSION_ERROR    (AS_DRM_FAILURE - 1)
#define AS_ABANDONED            (AS_COMPRESSION_ERROR - 1)

/** Async HTTP Library error codes. */
#define AS_SOCKET_ERROR                     (-50)
#define AS_SELECT_ERROR                     (AS_SOCKET_ERROR - 1)                   // -51

#define AS_DNS_ERROR                        (AS_SELECT_ERROR - 1)                   // -52
#define AS_DNS_TIMEOUT                      (AS_DNS_ERROR - 1)                      // -53
#define AS_DNS_NOT_FOUND                    (AS_DNS_TIMEOUT - 1)                    // -54

#define AS_CONNECTION_NOT_OPEN              (AS_DNS_NOT_FOUND - 1)                  // -55
#define AS_CONNECTION_ERROR                 (AS_CONNECTION_NOT_OPEN - 1)            // -56
#define AS_CONNECTION_TIMEOUT               (AS_CONNECTION_ERROR - 1)               // -57
#define AS_CONNECTION_REFUSED               (AS_CONNECTION_TIMEOUT - 1)             // -58
#define AS_CONNECTION_NET_UNREACHABLE       (AS_CONNECTION_REFUSED - 1)             // -59
#define AS_CONNECTION_RESET                 (AS_CONNECTION_NET_UNREACHABLE - 1)     // -60
#define AS_CONNECTION_CLOSED                (AS_CONNECTION_RESET - 1)               // -61

#define AS_HTTP_CONNECTION_STALL            (AS_CONNECTION_CLOSED - 1)              // -62
#define AS_HTTP_PROTOCOL_ERROR              (AS_HTTP_CONNECTION_STALL - 1)          // -63
#define AS_HTTP_PROTOCOL_1_0                (AS_HTTP_PROTOCOL_ERROR - 1)            // -64
#define AS_HTTP_RESPONSE_3XX                (AS_HTTP_PROTOCOL_1_0 - 1)              // -65
#define AS_HTTP_RESPONSE_4XX                (AS_HTTP_RESPONSE_3XX - 1)              // -66
#define AS_HTTP_RESPONSE_5XX                (AS_HTTP_RESPONSE_4XX - 1)              // -67
#define AS_HTTP_PIPELINING_NOT_SUPPORTED    (AS_HTTP_RESPONSE_5XX - 1)              // -68
#define AS_HTTP_PIPELINING_NOT_STABLE       (AS_HTTP_PIPELINING_NOT_SUPPORTED - 1)  // -69

#define AS_DNS_NO_DATA                      (AS_HTTP_PIPELINING_NOT_STABLE - 1)     // -70
#define AS_DNS_QUERY_MALFORMED              (AS_DNS_NO_DATA - 1)                    // -71
#define AS_DNS_SERVER_FAILURE               (AS_DNS_QUERY_MALFORMED - 1)            // -72
#define AS_DNS_NOT_IMPLEMENTED              (AS_DNS_SERVER_FAILURE - 1)             // -73
#define AS_DNS_QUERY_REFUSED                (AS_DNS_NOT_IMPLEMENTED - 1)            // -74
#define AS_DNS_BAD_NAME                     (AS_DNS_QUERY_REFUSED - 1)              // -75
#define AS_DNS_CONNECTION_REFUSED           (AS_DNS_BAD_NAME - 1)                   // -76
#define AS_DNS_NO_MEMORY                    (AS_DNS_CONNECTION_REFUSED - 1)         // -77
#define AS_DNS_CHANNEL_DESTROYED            (AS_DNS_NO_MEMORY - 1)                  // -78

#define AS_CONNECTION_INVALID_DESTINATION   (AS_DNS_CHANNEL_DESTROYED - 1)          // -79

#define AS_HTTP_INVALID_URL                 (AS_CONNECTION_INVALID_DESTINATION - 1) // -80
#define AS_HTTP_CONNECTION_ERROR            (AS_HTTP_INVALID_URL - 1 )              // -81
#define AS_HTTP_CONNECTION_TIMEOUT          (AS_HTTP_CONNECTION_ERROR  - 1 )       // -82

#define AS_DNS_RESPONSE_MALFORMED           (AS_HTTP_CONNECTION_TIMEOUT - 1)        // -83

#define AS_HTTP_TOO_MANY_REDIRECTS          (AS_DNS_RESPONSE_MALFORMED - 1)         // -84
#define AS_HTTP_TRANSACTION_TIMEOUT         (AS_HTTP_TOO_MANY_REDIRECTS - 1)        // -85

#define AS_SSL_ERROR                        (AS_HTTP_TRANSACTION_TIMEOUT - 1)       // -86

#define AS_HTTP_MESSAGE_LENGTH_ERROR        (AS_SSL_ERROR - 1)                      // -87

#define AS_DNS_FORMAT_ERROR                 (AS_HTTP_MESSAGE_LENGTH_ERROR - 1)      // -88
#define AS_DNS_BAD_FAMILY                   (AS_DNS_FORMAT_ERROR - 1)               // -89
#define AS_DNS_EOF                          (AS_DNS_BAD_FAMILY - 1)                 // -90
#define AS_DNS_FILE                         (AS_DNS_EOF - 1)                        // -91
#define AS_DNS_BAD_STRING                   (AS_DNS_FILE - 1)                       // -92
#define AS_DNS_BAD_FLAGS                    (AS_DNS_BAD_STRING - 1)                 // -93
#define AS_DNS_NO_NAME                      (AS_DNS_BAD_FLAGS - 1)                  // -94
#define AS_DNS_BAD_HINTS                    (AS_DNS_NO_NAME - 1)                    // -95
#define AS_DNS_NOT_INITIALIZED              (AS_DNS_BAD_HINTS - 1)                  // -96
#define AS_DNS_CANCELLED                    (AS_DNS_NOT_INITIALIZED - 1)            // -97

#define AS_CONNECTION_SLOW                  (AS_DNS_CANCELLED -1)                   // -98

#define AS_HTTP_HEADER_LENGTH_ERROR         (AS_CONNECTION_SLOW -1)                 // -99

#define AS_CONNECTION_NO_ROUTE_TO_HOST      (AS_HTTP_HEADER_LENGTH_ERROR - 1 )      // -100
#define AS_CONNECTION_NETWORK_DOWN          (AS_CONNECTION_NO_ROUTE_TO_HOST - 1 )   // -101
#define AS_CONNECTION_NO_ADDRESS            (AS_CONNECTION_NETWORK_DOWN - 1)        // -102
#define AS_CONNECTION_RESET_ON_CONNECT      (AS_CONNECTION_NO_ADDRESS - 1)          // -103
#define AS_CONNECTION_RESET_WHILE_RECEIVING (AS_CONNECTION_RESET_ON_CONNECT - 1)    // -104

/** Byte range downloader error codes. */
#define AS_MEDIA_BUFFER_UNAVAILABLE (-120)
#define AS_NO_RECEIVING_TASK        (AS_MEDIA_BUFFER_UNAVAILABLE - 1)

/** HTTP Request Manager */
#define AS_NETWORK_FAILURE          (-150)
#define AS_HTTP_CLIENT_ERROR        (AS_NETWORK_FAILURE - 1)
#define AS_HTTP_SERVER_ERROR        (AS_HTTP_CLIENT_ERROR - 1)
#define AS_DOMAIN_TIMEOUT           (AS_HTTP_SERVER_ERROR - 1)
#define AS_DOMAIN_SLOW              (AS_DOMAIN_TIMEOUT - 1)
#define AS_DOMAIN_CONNECTION_ERROR  (AS_DOMAIN_SLOW - 1 )
#define AS_INCOMPATIBLE_URL         (AS_DOMAIN_CONNECTION_ERROR - 1)
#define AS_HTTP_ALL_URLS_FAILED     (AS_INCOMPATIBLE_URL - 1)

/** Media Presentation Tree */
#define AS_MEDIA_PRESENTATION_FAILURE (-200)
#define AS_MEDIA_DURATION_ERROR (AS_MEDIA_PRESENTATION_FAILURE - 1)
#define AS_MEDIA_HEADER_ERROR (AS_MEDIA_DURATION_ERROR - 1)
#define AS_MEDIA_INDEX_ERROR (AS_MEDIA_HEADER_ERROR - 1)
#define AS_MEDIA_FILE_REQUEST_PENDING (AS_MEDIA_INDEX_ERROR - 1)
#define AS_MEDIA_FILE_NO_REQUEST_NEEDED (AS_MEDIA_FILE_REQUEST_PENDING - 1)
#define AS_MEDIA_END (AS_MEDIA_FILE_NO_REQUEST_NEEDED - 1)
#define AS_MEDIA_NOT_SUPPORTED (AS_MEDIA_END - 1)


/** Domain Selector */
#define AS_DOMAIN_SELECTION_FAILURE (-300)
#define AS_DOMAIN_SELECTION_FAILURE_FAILED_TOO_LONG (-302)
#define AS_DOMAIN_SELECTION_FAILURE_FAILED_TOO_MANY (-303)
#define AS_DOMAIN_SELECTION_FAILURE_BAD_STREAM (-304)
#define AS_DOMAIN_SELECTION_FAILURE_NO_AVAILABLE_LOCATION (-305)
#define AS_DOMAIN_SELECTION_FAILURE_FAILED_TOO_MANY_CONSECUTIVE_TIMES (-306)
#define AS_LAST_DOMAIN_SELECTION_FAILURE (-350)

/** Media File Factory */
#define AS_UNSUPPORTED_MIME_TYPE (-400)

/** Dash parser */
#define AS_DASH_INCOMPATIBLE_STREAMS_IN_TRACK (-500)

/** Persistent store */
#define AS_NOT_ENOUGH_SPACE (-600)
#define AS_ENCODE_ERROR (-601)

#define AS_REQUEST_CREATION_FAILURE (-602)
#define AS_ILLEGAL_REQUEST (-605)

#endif //ASE_ERROR_CODES
