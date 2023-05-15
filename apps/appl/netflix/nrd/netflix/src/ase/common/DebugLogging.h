/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEBUG_LOGGING_H
#define DEBUG_LOGGING_H

#include <stdio.h>
#include <string.h>
#include <vector>
#include <nrdnet/AseTimeVal.h>

#define USE_PATRACE

// Simple debug logging framework with two log levels, ERROR and DEBUG
// ERROR logs are always dropped (if the framework is enabled at all)
// DEBUG is enabled at run-time on a per logger basis
//

// Like anything else, you need to DECLARE and DEFINE each logger
#define DECLARE_LOGGER( __logger__ )                    \
extern const char* __logger__##_NAME;                   \
extern bool __logger__##_ENABLED

#define DEFINE_LOGGER( __logger__ )                                     \
    bool __logger__##_ENABLED = true;                                   \
    const char* __logger__##_NAME = addLogger( #__logger__, &__logger__##_ENABLED ) \


#define ENABLE_LOGGER( __logger__ )                     \
do { __logger__##_ENABLED = 1; } while( 0 )

#define CONFIGURE_LOGGER( __config__, __logger__ )                        \
do { if ( !__config__.queryParameter( "logger." #__logger__ ).empty() ) \
        ENABLE_LOGGER( __logger__ ); } while( 0 )

namespace netflix {
namespace ase
{
class IAseConfigParameter;

std::vector<std::pair<const char*,bool*> >& getLoggers();

inline const char* addLogger( const char* name, bool *flag )
{
    getLoggers().push_back( std::make_pair( name, flag ) );
    return name;
}

inline const char* debugBasename( const char* s )
{
    const char *slash = strrchr(s, '/');
    if (!slash)
        slash = strrchr(s, '\\');
    if (!slash)
        return s;

    return slash+1;
}

void configureLoggers( IAseConfigParameter const& configuration );

class DebugTrace
{
    public:
        DebugTrace( bool enabled, const char* name, const char* file, const char* function )
            : mEnabled( enabled ), mName( name ), mFile( file ), mFunction( function )
        {
            Trace( "Entering" );
        }

        ~DebugTrace()
        {
            Trace( "Exiting" );
        }

        void Trace( const char * action )
        {
            if ( mEnabled )
                printf( "%llu %s [%s] %s %s\n",
                            AseTimeVal::now().ms(),
                            mName, mFile, action, mFunction );
        }

    private:
        bool mEnabled;
        const char* mName;
        const char* mFile;
        const char* mFunction;
};

}} // end namespace ase

#define MAKE_BUFFER(...) \
    int MAKE_BUFFER_size = snprintf(0, 0, __VA_ARGS__);                 \
    char *MAKE_BUFFER_buffer = new char[MAKE_BUFFER_size+1];            \
    snprintf(MAKE_BUFFER_buffer, MAKE_BUFFER_size+1, __VA_ARGS__);      \
    MAKE_BUFFER_buffer[MAKE_BUFFER_size] = 0;                           \

#define FREE_BUFFER() \
    delete [] MAKE_BUFFER_buffer

#ifndef USE_PATRACE

#define DEBUGLOG( __logger__, ... )                             \
    do { if ( __logger__##_ENABLED )                            \
        {                                                       \
            MAKE_BUFFER(__VA_ARGS__);                           \
            printf( "%llu %s [%s:%d] %s\n",                     \
                    AseTimeVal::now().ms(),                     \
                    __logger__##_NAME,                          \
                    debugBasename( __FILE__ ),                  \
                    __LINE__,                                   \
                    MAKE_BUFFER_buffer );                       \
            FREE_BUFFER();                                      \
        }                                                       \
    } while( 0 )

#define ERRORLOG( __logger__, ... )                         \
    do {                                                    \
        MAKE_BUFFER(__VA_ARGS__);                           \
        printf( "%llu %s [%s:%d] %s\n" ,                    \
                AseTimeVal::now().ms(),                     \
                __logger__##_NAME,                          \
                debugBasename( __FILE__ ),                  \
                __LINE__,                                   \
                MAKE_BUFFER_buffer );                       \
        FREE_BUFFER();                                      \
    } while( 0 )

#define TRACE( __logger__ )                         \
    DebugTrace trace( __logger__##_ENABLED,         \
                      #__logger__,                  \
                      debugBasename( __FILE__ ),    \
                      __FUNCTION__ );

#else   /* USE_PATRACE */

#include <nrdbase/Log.h>

#if defined(NRDP_HAS_TRACING)
#define DEBUGLOG( __logger__, ... )                                     \
    do { if ( __logger__##_ENABLED )                                    \
        {                                                               \
            MAKE_BUFFER(__VA_ARGS__);                                   \
            NTRACE( TRACE_STREAMINGMANAGER,                             \
                    "[%s:%d] %s %s",                                    \
                    debugBasename( __FILE__ ),                          \
                    __LINE__,                                           \
                    __logger__##_NAME,                                  \
                    MAKE_BUFFER_buffer );                               \
            FREE_BUFFER();                                              \
        }                                                               \
    } while( 0 )
#else
#define DEBUGLOG( __logger__, ...)
#endif

#define ERRORLOG( __logger__, ... )                                     \
    do {                                                                \
        MAKE_BUFFER(__VA_ARGS__);                                       \
        Log::error(TRACE_STREAMINGMANAGER, "%s: %s",     \
                                  __logger__##_NAME,                    \
                                  MAKE_BUFFER_buffer );                 \
        FREE_BUFFER();                                                  \
    } while(0)
#define TRACE( __logger__ )

#endif



#endif /* LOGGING_H */
