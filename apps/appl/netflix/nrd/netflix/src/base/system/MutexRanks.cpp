/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MutexRanks.h"

#include <assert.h>
#include <stdarg.h>

using namespace netflix;

// helper function to return the max rank of the passed in arguments
static int maxRankOf(int n_args, ...)
{
    va_list ap;

    va_start(ap, n_args);
    int max = va_arg(ap, int);
    for (int i=1; i<n_args; ++i)
    {
        int a = va_arg(ap, int);
        if (a > max)
            max = a;
    }
    va_end(ap);

    return max + 1;
}

#define p1(val) val + 1
#define UNTRACKED_MUTEX_VAL                      -1
#define ZERO_MUTEX_VAL                           0

// we don't track this mutex because it is used by OpenSSL's lock_func and is used in
// an array of mutex's. We can't MutexRank this because they can be interleave locks
// and unlocks from OpenSSL. We're assuming that OpenSSL is not going to deadlock or
// contend with nrdlib mutex's
#define OPENSSLLIB_MUTEX_VAL                       UNTRACKED_MUTEX_VAL

// This is one that we don't lock any mutex while it is locked. So we don't track it.
#define CURLMULTITHREAD_SIGNAL_BYTE_MUTEX_VAL      UNTRACKED_MUTEX_VAL
#define HTTP_SERVICE_THREAD_SIGNAL_MUTEX_VAL       UNTRACKED_MUTEX_VAL
#define OBJECT_COUNT_MUTEX_VAL                     UNTRACKED_MUTEX_VAL

#define ASPEVENTLISTENER_MUTEX_VAL                 ZERO_MUTEX_VAL
#define ASPMETADATA_MUTEX_VAL                      ZERO_MUTEX_VAL
#define ASPOPENCOMPLETE_MUTEX_VAL                  ZERO_MUTEX_VAL
#define ASPSTREAMINGSTAT_MUTEX_VAL                 ZERO_MUTEX_VAL
#define EVENTCONNECTION_MUTEX_VAL                  ZERO_MUTEX_VAL
#define EVENTID_MUTEX_VAL                          ZERO_MUTEX_VAL
#define GIBBON_CONSOLE_DIRTY_MUTEX_VAL             ZERO_MUTEX_VAL
#define LOGFILE_MUTEX_VAL                          ZERO_MUTEX_VAL
#define MEDIAFILECACHECONFIG_MUTEX_VAL             ZERO_MUTEX_VAL
#define NRDAPPLICATION_MUTEX_VAL                   ZERO_MUTEX_VAL
#define NRDPISYSTEMEVENTDISPATCHER_MUTEX_VAL       ZERO_MUTEX_VAL
#define READWRITELOCK_MUTEX_VAL                    ZERO_MUTEX_VAL
#define SEMAPHORE_MUTEX_VAL                        ZERO_MUTEX_VAL
#define SRMWQUEUE_MUTEX_VAL                        ZERO_MUTEX_VAL
#define THREADSTART_MUTEX_VAL                      ZERO_MUTEX_VAL
#define TIME_MUTEX_VAL                             ZERO_MUTEX_VAL
#define TRACEAREAS_MUTEX_VAL                       ZERO_MUTEX_VAL
#define GIBBON_ANIMATION_THREAD_MUTEX_VAL          ZERO_MUTEX_VAL
#define GIBBON_RESOURCEMANAGER_MUTEX_VAL           ZERO_MUTEX_VAL
#define GIBBON_APPLICATION_INPUT_MANAGER_MUTEX_VAL ZERO_MUTEX_VAL
#define GIBBON_INPUTMANAGER_MUTEX_VAL              ZERO_MUTEX_VAL
#define LOG_SINKS_MUTEX_VAL                        ZERO_MUTEX_VAL

#define CONSOLESINK_MUTEX_VAL                      p1(TRACEAREAS_MUTEX_VAL)

#define PLAYERBRIDGE_EVENT_MUTEX_VAL          p1(EVENTID_MUTEX_VAL)

#define GIBBON_EVENTCONNECTIONJSC_MUTEX_VAL   p1(EVENTCONNECTION_MUTEX_VAL)

#define ESP_REFERENCECLOCK_MUTEX_VAL          p1(TIME_MUTEX_VAL)

#define LOGCATCHER_MUTEX_VAL                  p1(LOG_SINKS_MUTEX_VAL)
#define MESSAGESENDER_MUTEX_VAL               CONSOLESINK_MUTEX_VAL

#define DECODERLOCK_MUTEX_VAL                           p1(MESSAGESENDER_MUTEX_VAL)
#define EVENTLOOP_MUTEX_VAL                             p1(MESSAGESENDER_MUTEX_VAL)
#define GIBBON_GL_MUTEX_VAL                             p1(MESSAGESENDER_MUTEX_VAL)
#define GIBBON_GL_PRIVATE_MUTEX_VAL                     p1(MESSAGESENDER_MUTEX_VAL)
#define GIBBON_REMOTESCREENSERVER_CONNECTION_MUTEX_VAL  p1(MESSAGESENDER_MUTEX_VAL)
#define GIBBON_REMOTESCREEN_CONNECTION_SOCKET_MUTEX_VAL p1(MESSAGESENDER_MUTEX_VAL)
#define GIBBON_SCRIPTENGINE_MUTEX_VAL                   p1(MESSAGESENDER_MUTEX_VAL)
#define GIBBON_SCRIPTENGINE_OSALLOCATOR_MUTEX_VAL       p1(MESSAGESENDER_MUTEX_VAL)
#define GIBBON_SURFACEATLAS_MUTEX_VAL                   p1(MESSAGESENDER_MUTEX_VAL)
#define NETWORKBRIDGE_MUTEX_VAL                         p1(MESSAGESENDER_MUTEX_VAL)
#define PLAYBACKDEVICEEVENT_MUTEX_VAL                   p1(MESSAGESENDER_MUTEX_VAL)
#define PRODUCERCONSUMERLIST_MUTEX_VAL                  p1(MESSAGESENDER_MUTEX_VAL)
#define SYSTEMDATA_MUTEX_VAL                            p1(MESSAGESENDER_MUTEX_VAL)
#define THREADSTATE_MUTEX_VAL                           p1(MESSAGESENDER_MUTEX_VAL)
#define WRITELIMITER_MUTEX_VAL                          p1(MESSAGESENDER_MUTEX_VAL)
#define TEEAPICOMM_MUTEX_VAL                            p1(MESSAGESENDER_MUTEX_VAL)

#define SECURESTORE_MUTEX_VAL                           p1(TEEAPICOMM_MUTEX_VAL)

#define INTEGRATIONWRITER_MUTEX_VAL                     p1(WRITELIMITER_MUTEX_VAL)

#define MCMEDIASINK_MUTEX_VAL                     p1(MCMEDIABUFFERPOOL_MUTEX_VAL)

#define DRMSYSTEM_MUTEX_VAL                       p1(EVENTLOOP_MUTEX_VAL)
#define DRMCONTEXT_MUTEX_VAL                      p1(DRMSYSTEM_MUTEX_VAL)
#define DRMMANAGER_MUTEX_VAL                      p1(DRMCONTEXT_MUTEX_VAL)
#define DRMSYSTEMBRIDGE_MUTEX_VAL                 p1(DRMMANAGER_MUTEX_VAL)

#define GIBBON_ANIMATIONLOOP_MUTEX_VAL        p1(GIBBON_GL_MUTEX_VAL)

#define GIBBON_APPLICATION_MUTEX_VAL          p1(SYSTEMDATA_MUTEX_VAL)

#define GIBBON_REMOTESCREEN_STATS_MUTEX_VAL       p1(EVENTLOOP_MUTEX_VAL)

#define GIBBON_REMOTESCREEN_CONNECTION_WRITER_MUTEX_VAL  p1(GIBBON_REMOTESCREEN_STATS_MUTEX_VAL)

#define EVENTSOURCEEC_MUTEX_VAL               maxRankOf(2, \
                                                          MESSAGESENDER_MUTEX_VAL, \
                                                          EVENTLOOP_MUTEX_VAL)
#define MEDIAREQUESTBRIDGE_MUTEX_VAL          maxRankOf(2, \
                                                        EVENTLOOP_MUTEX_VAL, \
                                                        MCMEDIASINK_MUTEX_VAL)

#define NRDP_MEDIASOURCEBUFFER_MUTEX_VAL      p1(MCMEDIABUFFERPOOL_MUTEX)
#define NRDP_MEDIASOURCE_MUTEX_VAL            p1(NRDP_MEDIASOURCEBUFFER_MUTEX_VAL)
#define NRDP_MEDIACONSUMER_MUTEX_VAL          p1(MEDIAREQUESTBRIDGE_MUTEX_VAL)

#define MEDIAREQUESTBRIDGE_POOL_MUTEX_VAL     p1(NRDP_MEDIACONSUMER_MUTEX_VAL)

#define NRDP_VIDEO_MUTEX_VAL                  p1(NRDP_MEDIASOURCE_MUTEX_VAL)

#define HTTP_SERVICE_MUTEX_VAL                p1(NBPEVENTCONNECTIONS_MUTEX_VAL)
#define HTTPS_SERVICE_MUTEX_VAL               p1(NBPEVENTCONNECTIONS_MUTEX_VAL)
#define HTTP_SERVICE_THREAD_LOCK_MUTEX_VAL    maxRankOf(3, \
                                                          DNSMANAGER_INSTANCE_MUTEX_VAL, \
                                                          HTTP_SERVICE_MUTEX_VAL, \
                                                          HTTPS_SERVICE_MUTEX_VAL)

#define DNSMANAGER_RECORDS_MUTEX_VAL           maxRankOf(2, \
                                                         THREADSTATE_MUTEX_VAL, \
                                                         EVENTLOOP_MUTEX_VAL)

#define DNSMANAGER_CLIENTS_MUTEX_VAL           p1(DNSMANAGER_RECORDS_MUTEX_VAL)

#define DNSMANAGER_INSTANCE_MUTEX_VAL          p1(DNSMANAGER_CLIENTS_MUTEX_VAL)
#define MEDIAREQUESTBRIDGESTART_MUTEX_VAL      maxRankOf(2,   \
                                                           CONNECTION_POOL_MUTEX_VAL, \
                                                           DNSMANAGER_CLIENTS_MUTEX_VAL)

#define CONNECTION_POOL_MUTEX_VAL              p1(DNSMANAGER_INSTANCE_MUTEX_VAL)
#define CURLMULTITHREAD_MUTEX_VAL              p1(DNSMANAGER_INSTANCE_MUTEX_VAL)
#define HTTPPIPELINING_MUTEX_VAL               p1(DNSMANAGER_INSTANCE_MUTEX_VAL)
#define HTTPSTREAMBUFABORT_MUTEX_VAL           p1(DNSMANAGER_INSTANCE_MUTEX_VAL) // remove me please


#define DISKSTORE_MUTEX_VAL                    p1(SECURESTORE_MUTEX_VAL)
#define SYSTEMDATAPARAM_MUTEX_VAL              p1(SECURESTORE_MUTEX_VAL)
#define WEBCRYPTO_MUTEX_VAL                    p1(SECURESTORE_MUTEX_VAL)
#define WEBCRYPTOTEE_MUTEX_VAL                 p1(WEBCRYPTO_MUTEX_VAL)

#define TEXTTOSPEECH_MUTEX_VAL                p1(CONSOLESINK_MUTEX_VAL)

#define GIBBON_SURFACECONTROLLER_MUTEX_VAL    p1(GIBBON_SURFACEDECODER_THREAD_MUTEX_VAL)

#define ESP_CALLBACK_MUTEX_VAL                p1(PLAYBACKDEVICEEVENT_MUTEX_VAL)

#define GIBBON_SURFACE_MUTEX_VAL              p1(GIBBON_SURFACECONTROLLER_MUTEX_VAL)

#define ESP_RENDERER_MUTEX_VAL                maxRankOf(4,              \
                                                        GIBBON_SURFACE_MUTEX_VAL, \
                                                        EVENTLOOP_MUTEX_VAL, \
                                                        ESP_REFERENCECLOCK_MUTEX_VAL, \
                                                        MESSAGESENDER_MUTEX_VAL)
#define ESP_DECODER_MUTEX_VAL                 p1(ESP_RENDERER_MUTEX_VAL)

#define ESP_PLAYBACKGROUP_MUTEX_VAL           p1(ESP_DECODER_MUTEX_VAL)

#define ESP_PLAYER_MUTEX_VAL                  maxRankOf(3,              \
                                                        ESP_CALLBACK_MUTEX_VAL, \
                                                        ESP_DECODER_MUTEX_VAL, \
                                                        DECODERLOCK_MUTEX_VAL)

#define GIBBON_SURFACECACHE_MUTEX_VAL         maxRankOf(2, \
                                                            GIBBON_SURFACE_MUTEX_VAL, \
                                                          GIBBON_REMOTESCREEN_CONNECTION_WRITER_MUTEX_VAL)

#define GIBBON_FREETYPE_MUTEX_VAL             p1(GIBBON_SURFACECACHE_MUTEX_VAL) \

#define GIBBON_EVENTCONNECTIONV8_MUTEX_VAL    p1(MESSAGESENDER_MUTEX_VAL)
#define GIBBON_EVENTCONNECTIONMOZJS_MUTEX_VAL p1(MESSAGESENDER_MUTEX_VAL)
#define NBPEVENTCONNECTIONS_MUTEX_VAL         p1(MESSAGESENDER_MUTEX_VAL) // remove me please
#define GIBBON_COOKIE_MUTEX_VAL               p1(MESSAGESENDER_MUTEX_VAL)

#define PLAYERBRIDGE_ASPLICENSE_MUTEX_VAL     maxRankOf(2, \
                                                        GIBBON_REMOTESCREEN_CONNECTION_WRITER_MUTEX_VAL, \
                                                        EVENTLOOP_MUTEX_VAL)

#define MDXINTERNALLISTENER_MUTEX_VAL         maxRankOf(2,            \
                                                          EVENTSOURCEEC_MUTEX_VAL, \
                                                          EVENTLOOP_MUTEX_VAL)

#define MDXWEBSOCKETSERVER_MUTEX_VAL          p1(MDXINTERNALLISTENER_MUTEX_VAL)
#define MDXWEBSOCKETCLIENT_MUTEX_VAL          maxRankOf(2,              \
                                                        MDXINTERNALLISTENER_MUTEX_VAL, \
                                                        DNSMANAGER_CLIENTS_MUTEX_VAL)
#define WEBSOCKETTHREAD_MUTEX_VAL             maxRankOf(2,            \
                                                          MDXWEBSOCKETSERVER_MUTEX_VAL, \
                                                          MDXWEBSOCKETCLIENT_MUTEX_VAL)
#define MDXINTERNALSTATE_MUTEX_VAL            maxRankOf(2,              \
                                                        WEBSOCKETTHREAD_MUTEX_VAL, \
                                                        CURLMULTITHREAD_MUTEX_VAL)

#define MDXCONTROL_MUTEX_VAL                  p1(MDXINTERNALSTATE_MUTEX_VAL)
#define MDXCONTEXT_MUTEX_VAL                  p1(MDXCONTROL_MUTEX_VAL)
#define MDXNETWORKMANAGER_MUTEX_VAL           p1(MDXCONTROL_MUTEX_VAL)
#define MDXWEBSOCKETCONNECTIONS_MUTEX_VAL     p1(MDXINTERNALLISTENER_MUTEX_VAL)
#define WEBSOCKETBRIDGE_MUTEX_VAL             p1(WEBSOCKETTHREAD_MUTEX_VAL)
#define MCMEDIABUFFERPOOL_MUTEX_VAL           p1(MESSAGESENDER_MUTEX_VAL)
#define MEDIABUFFERPOOL_MUTEX_VAL             p1(MESSAGESENDER_MUTEX_VAL)

#define NPTICKET_MUTEX_VAL                    p1(MDXNETWORKMANAGER_MUTEX_VAL)

#define PLAYBACKREPORTERBRIDGE_MUTEX_VAL      maxRankOf(3,            \
                                                        GIBBON_REMOTESCREEN_CONNECTION_WRITER_MUTEX_VAL, \
                                                        EVENTLOOP_MUTEX_VAL, \
                                                        EVENTSOURCEEC_MUTEX_VAL)
#define MPCMD_MUTEX_VAL                       p1(PLAYBACKREPORTERBRIDGE_MUTEX_VAL)
#define LOCSEL_CMD_MUTEX_VAL                  p1(PLAYBACKREPORTERBRIDGE_MUTEX_VAL)

#define NETWORKMONITORSTATE_MUTEX_VAL         maxRankOf(2,SECURESTORE_MUTEX_VAL, EVENTLOOP_MUTEX_VAL)

#define PERIODICWORKEREXECUTE_MUTEX_VAL       p1(HTTPSTREAMBUFABORT_MUTEX_VAL) // remove me please

#define HTTPREQUESTMGRSIGNAL_MUTEX_VAL        p1(PLAYBACKREPORTERBRIDGE_MUTEX_VAL)

#define HTTPREQUESTMGRCOMMAND_MUTEX_VAL       p1(HTTPREQUESTMGRSIGNAL_MUTEX_VAL)
#define HTTPREQUESTMGRASYNCCOMMAND_MUTEX_VAL  p1(HTTPREQUESTMGRSIGNAL_MUTEX_VAL)
#define TRAFFICSHAPER_MUTEX_VAL               p1(HTTPREQUESTMGRCOMMAND_MUTEX_VAL)
#define MEDIAFILECACHE_MUTEX_VAL              p1(HTTPREQUESTMGRCOMMAND_MUTEX_VAL)

#define DEVICELIB_MUTEX_VAL                   maxRankOf(2,            \
                                                          NPTICKET_MUTEX_VAL, \
                                                          WEBCRYPTO_MUTEX_VAL)

#define ASPPLAYBACK_MUTEX_VAL                 maxRankOf(4,    \
                                                          THREADSTATE_MUTEX_VAL, \
                                                          ESP_CALLBACK_MUTEX_VAL, \
                                                          ESP_PLAYER_MUTEX_VAL, \
                                                          NPTICKET_MUTEX_VAL)

#define THREADCONFIG_MUTEX_VAL                p1(THREADSTATE_MUTEX_VAL)
#define THREADPOOL_MUTEX_VAL                  p1(THREADSTATE_MUTEX_VAL)

#define GIBBON_SURFACECACHE_THREADPOOL_MUTEX_VAL p1(THREADPOOL_MUTEX_VAL)
#define GIBBON_SURFACEDECODER_THREAD_MUTEX_VAL   p1(THREADPOOL_MUTEX_VAL)

#define HTTPNBP_MUTEX_VAL                     p1(NBPEVENTCONNECTIONS_MUTEX_VAL)

#define RESOURCE_MUTEX_VAL                    p1(HTTP_SERVICE_THREAD_LOCK_MUTEX_VAL)

#define DISKCACHE_MUTEX_VAL                   p1(RESOURCE_MUTEX_VAL)

#define RESOURCEMANAGER_MUTEX_VAL             p1(DISKCACHE_MUTEX_VAL)

#define AUDIOBRIDGE_MUTEX_VAL                 p1(RESOURCEMANAGER_MUTEX_VAL)

#define GIBBON_FLIP_MUTEX_VAL                 maxRankOf(2, \
                                                        RESOURCE_MUTEX_VAL, \
                                                        RESOURCEMANAGER_MUTEX_VAL)

#define GIBBON_SCREEN_MUTEX_VAL               p1(GIBBON_FLIP_MUTEX_VAL)

#define GIBBON_IMAGE_MUTEX_VAL                p1(GIBBON_SCREEN_MUTEX_VAL)

#define GIBBON_ANIMATION_MUTEX_VAL            p1(GIBBON_IMAGE_MUTEX_VAL)
#define GIBBON_BRIDGE_MUTEX_VAL               p1(GIBBON_IMAGE_MUTEX_VAL)

#define GIBBON_SURFACECACHE_DECODE_MUTEX_VAL  p1(GIBBON_ANIMATION_MUTEX_VAL)

#define GIBBON_SURFACECACHE_TOUCH_MUTEX_VAL   p1(GIBBON_SURFACECACHE_DECODE_MUTEX_VAL)

#define PERIODICWORKER_MUTEX_VAL              p1(PERIODICWORKEREXECUTE_MUTEX_VAL) // 9 // remove me please

#define STORAGEBRIDGE_MUTEX_VAL               maxRankOf(2,\
                                                          NBPEVENTCONNECTIONS_MUTEX_VAL, \
                                                          PERIODICWORKER_MUTEX_VAL)

#define ASPMANIFEST_MUTEX_VAL                 maxRankOf(2,                  \
                                                    ASPPLAYBACK_MUTEX_VAL,  \
                                                    MPCMD_MUTEX_VAL)

#define ASPSTREAMING_MUTEX_VAL                maxRankOf(2,                  \
                                                    ASPMANIFEST_MUTEX_VAL, \
                                                    MCMEDIASINK_MUTEX_VAL)

#define ASPTRACKSWITCH_MUTEX_VAL              maxRankOf(5,            \
                                                    ASPMANIFEST_MUTEX_VAL, \
                                                    ASPSTREAMING_MUTEX_VAL, \
                                                    MCMEDIASINK_MUTEX_VAL, \
                                                    NBPEVENTCONNECTIONS_MUTEX_VAL, \
                                                    NRDP_MEDIASOURCE_MUTEX_VAL )

#define ASPSTATE_MUTEX_VAL                    maxRankOf(3,                  \
                                                    ASPPLAYBACK_MUTEX_VAL,  \
                                                    ASPTRACKSWITCH_MUTEX_VAL, \
                                                    SRMWQUEUE_MUTEX_VAL)

#define MCTHREAD_MUTEX_VAL                    maxRankOf(3,                  \
                                                    ASPSTREAMING_MUTEX_VAL, \
                                                    ASPSTATE_MUTEX_VAL,   \
                                                    ASPTRACKSWITCH_MUTEX_VAL)

#define ASPTHREAD_MUTEX_VAL                   p1(MCTHREAD_MUTEX_VAL)

#define ASPCONTROL_MUTEX_VAL                  maxRankOf(4,                  \
                                                    ASPEVENTLISTENER_MUTEX_VAL, \
                                                    ASPTHREAD_MUTEX_VAL,    \
                                                    ASPSTATE_MUTEX_VAL,   \
                                                    MCTHREAD_MUTEX_VAL)

#define GIBBON_WIDGET_MUTEX_VAL               maxRankOf(2, \
                                                        GIBBON_SURFACECACHE_TOUCH_MUTEX_VAL, \
                                                        ASPCONTROL_MUTEX_VAL)

#define PLAYERBRIDGE_PLAYER_MUTEX_VAL         p1(ASPCONTROL_MUTEX_VAL)

#define PLAYERBRIDGE_MUTEX_VAL                maxRankOf(4,    \
                                                    PLAYERBRIDGE_PLAYER_MUTEX_VAL, \
                                                    ASPSTATE_MUTEX_VAL, \
                                                    EVENTLOOP_MUTEX_VAL, \
                                                    EVENTSOURCEEC_MUTEX_VAL)


//////////
//
// MutexRankValue
//
//////////
#define MAP_MUTEX_RANK(mutex) \
    case mutex: return mutex##_VAL

int netflix::MutexRankValue(MutexRank rank)
{
    switch (rank) {
        MAP_MUTEX_RANK(ASPCONTROL_MUTEX);
        MAP_MUTEX_RANK(ASPEVENTLISTENER_MUTEX);
        MAP_MUTEX_RANK(ASPMANIFEST_MUTEX);
        MAP_MUTEX_RANK(ASPMETADATA_MUTEX);
        MAP_MUTEX_RANK(ASPOPENCOMPLETE_MUTEX);
        MAP_MUTEX_RANK(ASPPLAYBACK_MUTEX);
        MAP_MUTEX_RANK(ASPSTATE_MUTEX);
        MAP_MUTEX_RANK(ASPSTREAMINGSTAT_MUTEX);
        MAP_MUTEX_RANK(ASPSTREAMING_MUTEX);
        MAP_MUTEX_RANK(ASPTHREAD_MUTEX);
        MAP_MUTEX_RANK(ASPTRACKSWITCH_MUTEX);
        MAP_MUTEX_RANK(AUDIOBRIDGE_MUTEX);
        MAP_MUTEX_RANK(CONNECTION_POOL_MUTEX);
        MAP_MUTEX_RANK(CONSOLESINK_MUTEX);
        MAP_MUTEX_RANK(CURLMULTITHREAD_MUTEX);
        MAP_MUTEX_RANK(CURLMULTITHREAD_SIGNAL_BYTE_MUTEX);
        MAP_MUTEX_RANK(DECODERLOCK_MUTEX);
        MAP_MUTEX_RANK(DEVICELIB_MUTEX);
        MAP_MUTEX_RANK(DISKCACHE_MUTEX);
        MAP_MUTEX_RANK(DISKSTORE_MUTEX);
        MAP_MUTEX_RANK(DNSMANAGER_CLIENTS_MUTEX);
        MAP_MUTEX_RANK(DNSMANAGER_INSTANCE_MUTEX);
        MAP_MUTEX_RANK(DNSMANAGER_RECORDS_MUTEX);
        MAP_MUTEX_RANK(DRMCONTEXT_MUTEX);
        MAP_MUTEX_RANK(DRMMANAGER_MUTEX);
        MAP_MUTEX_RANK(DRMSYSTEMBRIDGE_MUTEX);
        MAP_MUTEX_RANK(DRMSYSTEM_MUTEX);
        MAP_MUTEX_RANK(ESP_CALLBACK_MUTEX);
        MAP_MUTEX_RANK(ESP_DECODER_MUTEX);
        MAP_MUTEX_RANK(ESP_PLAYBACKGROUP_MUTEX);
        MAP_MUTEX_RANK(ESP_PLAYER_MUTEX);
        MAP_MUTEX_RANK(ESP_REFERENCECLOCK_MUTEX);
        MAP_MUTEX_RANK(ESP_RENDERER_MUTEX);
        MAP_MUTEX_RANK(EVENTCONNECTION_MUTEX);
        MAP_MUTEX_RANK(EVENTID_MUTEX);
        MAP_MUTEX_RANK(EVENTLOOP_MUTEX);
        MAP_MUTEX_RANK(EVENTSOURCEEC_MUTEX);
        MAP_MUTEX_RANK(GIBBON_ANIMATIONLOOP_MUTEX);
        MAP_MUTEX_RANK(GIBBON_ANIMATION_MUTEX);
        MAP_MUTEX_RANK(GIBBON_ANIMATION_THREAD_MUTEX);
        MAP_MUTEX_RANK(GIBBON_APPLICATION_INPUT_MANAGER_MUTEX);
        MAP_MUTEX_RANK(GIBBON_APPLICATION_MUTEX);
        MAP_MUTEX_RANK(GIBBON_BRIDGE_MUTEX);
        MAP_MUTEX_RANK(GIBBON_CONSOLE_DIRTY_MUTEX);
        MAP_MUTEX_RANK(GIBBON_COOKIE_MUTEX);
        MAP_MUTEX_RANK(GIBBON_EVENTCONNECTIONJSC_MUTEX);
        MAP_MUTEX_RANK(GIBBON_EVENTCONNECTIONMOZJS_MUTEX);
        MAP_MUTEX_RANK(GIBBON_EVENTCONNECTIONV8_MUTEX);
        MAP_MUTEX_RANK(GIBBON_FLIP_MUTEX);
        MAP_MUTEX_RANK(GIBBON_FREETYPE_MUTEX);
        MAP_MUTEX_RANK(GIBBON_GL_MUTEX);
        MAP_MUTEX_RANK(GIBBON_GL_PRIVATE_MUTEX);
        MAP_MUTEX_RANK(GIBBON_INPUTMANAGER_MUTEX);
        MAP_MUTEX_RANK(GIBBON_REMOTESCREENSERVER_CONNECTION_MUTEX);
        MAP_MUTEX_RANK(GIBBON_REMOTESCREEN_CONNECTION_SOCKET_MUTEX);
        MAP_MUTEX_RANK(GIBBON_REMOTESCREEN_CONNECTION_WRITER_MUTEX);
        MAP_MUTEX_RANK(GIBBON_REMOTESCREEN_STATS_MUTEX);
        MAP_MUTEX_RANK(GIBBON_RESOURCEMANAGER_MUTEX);
        MAP_MUTEX_RANK(GIBBON_SCREEN_MUTEX);
        MAP_MUTEX_RANK(GIBBON_SCRIPTENGINE_MUTEX);
        MAP_MUTEX_RANK(GIBBON_SCRIPTENGINE_OSALLOCATOR_MUTEX);
        MAP_MUTEX_RANK(GIBBON_SURFACEATLAS_MUTEX);
        MAP_MUTEX_RANK(GIBBON_SURFACECACHE_DECODE_MUTEX);
        MAP_MUTEX_RANK(GIBBON_SURFACECACHE_MUTEX);
        MAP_MUTEX_RANK(GIBBON_SURFACECACHE_THREADPOOL_MUTEX);
        MAP_MUTEX_RANK(GIBBON_SURFACECACHE_TOUCH_MUTEX);
        MAP_MUTEX_RANK(GIBBON_SURFACECONTROLLER_MUTEX);
        MAP_MUTEX_RANK(GIBBON_SURFACEDECODER_THREAD_MUTEX);
        MAP_MUTEX_RANK(GIBBON_SURFACE_MUTEX);
        MAP_MUTEX_RANK(GIBBON_WIDGET_MUTEX);
        MAP_MUTEX_RANK(HTTPNBP_MUTEX);
        MAP_MUTEX_RANK(HTTPPIPELINING_MUTEX);
        MAP_MUTEX_RANK(HTTPREQUESTMGRASYNCCOMMAND_MUTEX);
        MAP_MUTEX_RANK(HTTPREQUESTMGRCOMMAND_MUTEX);
        MAP_MUTEX_RANK(HTTPREQUESTMGRSIGNAL_MUTEX);
        MAP_MUTEX_RANK(HTTPS_SERVICE_MUTEX);
        MAP_MUTEX_RANK(HTTP_SERVICE_MUTEX);
        MAP_MUTEX_RANK(HTTP_SERVICE_THREAD_LOCK_MUTEX);
        MAP_MUTEX_RANK(HTTP_SERVICE_THREAD_SIGNAL_MUTEX);
        MAP_MUTEX_RANK(INTEGRATIONWRITER_MUTEX);
        MAP_MUTEX_RANK(LOCSEL_CMD_MUTEX);
        MAP_MUTEX_RANK(LOGCATCHER_MUTEX);
        MAP_MUTEX_RANK(LOGFILE_MUTEX);
        MAP_MUTEX_RANK(LOG_SINKS_MUTEX);
        MAP_MUTEX_RANK(MCMEDIABUFFERPOOL_MUTEX);
        MAP_MUTEX_RANK(MCMEDIASINK_MUTEX);
        MAP_MUTEX_RANK(MCTHREAD_MUTEX);
        MAP_MUTEX_RANK(MDXCONTEXT_MUTEX);
        MAP_MUTEX_RANK(MDXCONTROL_MUTEX);
        MAP_MUTEX_RANK(MDXINTERNALLISTENER_MUTEX);
        MAP_MUTEX_RANK(MDXINTERNALSTATE_MUTEX);
        MAP_MUTEX_RANK(MDXNETWORKMANAGER_MUTEX);
        MAP_MUTEX_RANK(MDXWEBSOCKETCLIENT_MUTEX);
        MAP_MUTEX_RANK(MDXWEBSOCKETCONNECTIONS_MUTEX);
        MAP_MUTEX_RANK(MDXWEBSOCKETSERVER_MUTEX);
        MAP_MUTEX_RANK(MEDIABUFFERPOOL_MUTEX);
        MAP_MUTEX_RANK(MEDIAFILECACHECONFIG_MUTEX);
        MAP_MUTEX_RANK(MEDIAFILECACHE_MUTEX);
        MAP_MUTEX_RANK(MEDIAREQUESTBRIDGESTART_MUTEX);
        MAP_MUTEX_RANK(MEDIAREQUESTBRIDGE_MUTEX);
        MAP_MUTEX_RANK(MEDIAREQUESTBRIDGE_POOL_MUTEX);
        MAP_MUTEX_RANK(MPCMD_MUTEX);
        MAP_MUTEX_RANK(NETWORKBRIDGE_MUTEX);
        MAP_MUTEX_RANK(NETWORKMONITORSTATE_MUTEX);
        MAP_MUTEX_RANK(NPTICKET_MUTEX);
        MAP_MUTEX_RANK(NRDAPPLICATION_MUTEX);
        MAP_MUTEX_RANK(NRDPISYSTEMEVENTDISPATCHER_MUTEX);
        MAP_MUTEX_RANK(NRDP_MEDIACONSUMER_MUTEX);
        MAP_MUTEX_RANK(NRDP_MEDIASOURCEBUFFER_MUTEX);
        MAP_MUTEX_RANK(NRDP_MEDIASOURCE_MUTEX);
        MAP_MUTEX_RANK(NRDP_VIDEO_MUTEX);
        MAP_MUTEX_RANK(OBJECT_COUNT_MUTEX);
        MAP_MUTEX_RANK(OPENSSLLIB_MUTEX);
        MAP_MUTEX_RANK(PLAYBACKDEVICEEVENT_MUTEX);
        MAP_MUTEX_RANK(PLAYBACKREPORTERBRIDGE_MUTEX);
        MAP_MUTEX_RANK(PLAYERBRIDGE_ASPLICENSE_MUTEX);
        MAP_MUTEX_RANK(PLAYERBRIDGE_EVENT_MUTEX);
        MAP_MUTEX_RANK(PLAYERBRIDGE_MUTEX);
        MAP_MUTEX_RANK(PLAYERBRIDGE_PLAYER_MUTEX);
        MAP_MUTEX_RANK(PRODUCERCONSUMERLIST_MUTEX);
        MAP_MUTEX_RANK(READWRITELOCK_MUTEX);
        MAP_MUTEX_RANK(RESOURCEMANAGER_MUTEX);
        MAP_MUTEX_RANK(SECURESTORE_MUTEX);
        MAP_MUTEX_RANK(SEMAPHORE_MUTEX);
        MAP_MUTEX_RANK(SRMWQUEUE_MUTEX);
        MAP_MUTEX_RANK(STORAGEBRIDGE_MUTEX);
        MAP_MUTEX_RANK(SYSTEMDATAPARAM_MUTEX);
        MAP_MUTEX_RANK(SYSTEMDATA_MUTEX);
        MAP_MUTEX_RANK(TEEAPICOMM_MUTEX);
        MAP_MUTEX_RANK(TEXTTOSPEECH_MUTEX);
        MAP_MUTEX_RANK(THREADCONFIG_MUTEX);
        MAP_MUTEX_RANK(THREADPOOL_MUTEX);
        MAP_MUTEX_RANK(THREADSTART_MUTEX);
        MAP_MUTEX_RANK(THREADSTATE_MUTEX);
        MAP_MUTEX_RANK(TIME_MUTEX);
        MAP_MUTEX_RANK(TRACEAREAS_MUTEX);
        MAP_MUTEX_RANK(TRAFFICSHAPER_MUTEX);
        MAP_MUTEX_RANK(UNTRACKED_MUTEX);
        MAP_MUTEX_RANK(WEBCRYPTOTEE_MUTEX);
        MAP_MUTEX_RANK(WEBCRYPTO_MUTEX);
        MAP_MUTEX_RANK(WEBSOCKETBRIDGE_MUTEX);
        MAP_MUTEX_RANK(WEBSOCKETTHREAD_MUTEX);
        MAP_MUTEX_RANK(WRITELIMITER_MUTEX);
        MAP_MUTEX_RANK(ZERO_MUTEX);
    }

    assert(false);
    return 0;
}
