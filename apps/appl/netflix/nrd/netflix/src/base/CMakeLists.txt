# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

cmake_minimum_required(VERSION 2.8)

project(nrdbase)

#=====================================================================================================

# Code to check if the system supports PTHREAD_MUTEX_RECURSIVE
# --------------------------------------------------
set(RECURSIVE_MUTEX_TEST "
#define _GNU_SOURCE 1
#include <pthread.h>

int main()
{
    pthread_mutex_t lock;
    pthread_mutexattr_t attr;

    pthread_mutex_init(&lock, 0);
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&lock, &attr);
    pthread_mutexattr_destroy(&attr);
    return 0;
}
")
check_c_source_compiles("${RECURSIVE_MUTEX_TEST}" NRDP_HAS_RECURSIVE_MUTEX)
# --------------------------------------------------

# Code to check if the system supports libc atomic operations
# --------------------------------------------------
set(ATOMICS_TEST "
int main()
{
     int m_reference;
     __sync_add_and_fetch(&m_reference, 1);
     __sync_sub_and_fetch(&m_reference, 1);
    return 0;
}")
check_c_source_compiles("${ATOMICS_TEST}" NRDP_HAS_ATOMICS)

# Code to check if the system supports CXA_DEMANGLE
# --------------------------------------------------
check_include_file("execinfo.h" NRDP_HAS_EXECINFO_H)
# --------------------------------------------------

# Code to check if the system supports CXA_DEMANGLE
# --------------------------------------------------
set(CXA_DEMANGLE_TEST "
#include <cxxabi.h>

int main()
{
    abi::__cxa_demangle(0, 0, 0, 0);
    return 0;
}
")

# --------------------------------------------------

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("${CXA_DEMANGLE_TEST}" NRDP_HAS_CXA_DEMANGLE)

# Code to check if the system has pthread_setname_np, and if so, what it looks like
# --------------------------------------------------
set(SETNAME_NP_TEST_1 "
#define _GNU_SOURCE
#include <pthread.h>

int main()
{
    pthread_setname_np(\"test\");
    return 0;
    }
    ")
    check_c_source_compiles("${SETNAME_NP_TEST_1}" NRDP_HAS_PTHREAD_SETNAME_1)

    set(SETNAME_NP_TEST_2 "
#define _GNU_SOURCE
#include <pthread.h>

    int main()
    {
        pthread_setname_np(pthread_self(), \"test\");
        return 0;
    }
    ")
    check_c_source_compiles("${SETNAME_NP_TEST_2}" NRDP_HAS_PTHREAD_SETNAME_2)

# Code to check if the system has syscall(SYS_gettid)
# --------------------------------------------------
set(GETTID_TEST "
#include <unistd.h>
#include <sys/syscall.h>

int main()
{
    syscall(SYS_gettid);
    return 0;
    }
    ")
check_c_source_compiles("${GETTID_TEST}" NRDP_HAS_GETTID)


# Code to check if the system has pthread_set_affinity_np
# --------------------------------------------------
set(PTHREAD_SET_AFFINITY_TEST "
#include <pthread.h>
#include <sched.h>

int main()
{
    cpu_set_t affinity;
    CPU_ZERO(&affinity);
    CPU_OR(&affinity, &affinity, &affinity);
    CPU_COUNT(&affinity);
    pthread_setaffinity_np(pthread_self(), sizeof(affinity), &affinity);
    return 0;
    }
    ")
check_c_source_compiles("${PTHREAD_SET_AFFINITY_TEST}" NRDP_HAS_PTHREAD_SET_AFFINITY)



# Code to check if the system has decent time foo
# --------------------------------------------------
    set(TIMEGM_TEST "
#include <time.h>

    int main()
    {
        struct tm t;
        timegm(&t);
        (void)t;
        return 0;
    }
    ")
    check_c_source_compiles("${TIMEGM_TEST}" NRDP_HAS_TIMEGM)

    set(TM_GMTOFF_TEST "
#include <time.h>

    int main()
    {
        struct tm t;
        t.tm_gmtoff = 0;
        (void)t;
        return 0;
    }
    ")
    check_c_source_compiles("${TM_GMTOFF_TEST}" NRDP_HAS_TM_GMTOFF)

    set(TM_ZONE_TEST "
#include <time.h>

    int main()
    {
        struct tm t;
        t.tm_zone = 0;
        (void)t;
        return 0;
    }
    ")
    check_c_source_compiles("${TM_ZONE_TEST}" NRDP_HAS_TM_ZONE)

#=====================================================================================================

    configure_file_if_different(config.h.in ${NRDP_BINARY_ROOT_DIR}/include/nrdbase/config.h)

    set(SOURCES

        app/Application.cpp
        app/EventLoop.cpp
        app/ConfigData.cpp
        app/Configuration.cpp
        app/Console.cpp
        app/EmbeddedResources.cpp
        app/WriteLimiter.cpp

        crypto/DigestAlgo.cpp
        crypto/Digester.cpp
        crypto/OpenSSLLib.cpp
        crypto/OpenSSLException.cpp
        crypto/Random.cpp

        data/Base64.cpp
        data/DataBuffer.cpp
        data/Dumper.cpp
        data/Grep.cpp
        data/JSONFormatter.cpp
        data/JSONParser.cpp
        data/ObjectCount.cpp
        data/ReadDir.cpp
        data/StringCompressor.cpp
        data/StringFormatter.cpp
        data/StringTokenizer.cpp
        data/Variant.cpp
        data/XMLParser.cpp

        error/ASError.cpp
        error/NFErrName.cpp
        error/NFError.cpp
        error/NFErrorStack.cpp
        error/NFErrorTimedOut.cpp
        error/SysError.cpp

        log/ConsoleSink.cpp
        log/Log.cpp
        log/LogSink.cpp
        log/TraceAreas.cpp

        perf/Instrumentation.cpp

        system/ConditionVariable.cpp
        system/MemoryStats.cpp
        system/Mutex.cpp
        system/MutexRanks.cpp
        system/MutexRanksIterator.cpp
        system/MutexStack.cpp
        system/Semaphore.cpp
        system/Thread.cpp
        system/ThreadPool.cpp
        system/Time.cpp
    )

    if(NRDP_CRASH_REPORTING_ENABLED)
        include_directories(minidump)
        list(APPEND SOURCES "minidump/guid_creator.cpp")
    endif()
    if(NRDP_CRASH_REPORTING_BREAKPAD)
        list(APPEND SOURCES "minidump/minidump.cpp" "minidump/guid_creator.cpp")
    endif()

    file(GLOB_RECURSE HEADERS *.h)

    add_library(nrdbase ${SOURCES} ${HEADERS})
    target_link_libraries(nrdbase ssl crypto expat)
    if(BUILD_GPERFTOOLS)
        target_link_libraries(nrdbase profiler)
    endif()

# Public headers
    file(GLOB_RECURSE PUBLIC_HEADERS *.h)

    list(APPEND PUBLIC_HEADERS "${NRDP_3RDPARTY_DIRECTORY}/utf8/utf8.h" "${NRDP_3RDPARTY_DIRECTORY}/utf8/utf8_core.h" "${NRDP_3RDPARTY_DIRECTORY}/utf8/utf8_checked.h" "${NRDP_3RDPARTY_DIRECTORY}/utf8/utf8_unchecked.h")
    if(NRDP_CRASH_REPORTING_ENABLED)
        list(APPEND PUBLIC_HEADERS "minidump/guid_creator.h" "minidump/breakpad_types.h" "minidump/minidump_format.h")
    endif()
    if(NRDP_CRASH_REPORTING_BREAKPAD)
        list(APPEND PUBLIC_HEADERS   "minidump/minidump_exception_linux.h" "minidump/minidump.h" "minidump/scoped_ptr.h" "minidump/using_std_string.h")
    endif()

    create_install_target(
        TARGET nrdbase
        DESCRIPTION "Base library"
        COMPONENT "NRDP"
        PUBLIC_HEADERS "${PUBLIC_HEADERS}"
    )

    add_documentation("nrdbase"
                      INPUT ${CMAKE_CURRENT_SOURCE_DIR}/
                      EXCLUDE_PATTERNS "*/_p*/"
                      EXTRACT_ALL
                      TITLE "NRD Base Library"
    )
