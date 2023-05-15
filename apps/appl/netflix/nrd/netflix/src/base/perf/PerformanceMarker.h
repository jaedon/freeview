/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef PerformanceMarker_h
#define PerformanceMarker_h

namespace netflix {

#if !defined( NRDP_HAS_PERFORMANCE_MARKERS ) || ( defined( BUILD_PRODUCTION ) && !defined( NOVA_BUILD_PERFORMANCE ) )

#define PERFORMANCE_MARKER_START(name)          do{}while(0)
#define PERFORMANCE_MARKER_END()                do{}while(0)
#define PERFORMANCE_MARKER_SCOPED(...)          do{}while(0)

#else

#define PERFORMANCE_MARKER_COMBINE1(x, y)       x##y
#define PERFORMANCE_MARKER_COMBINE(x, y)        PERFORMANCE_MARKER_COMBINE1(x,y)
#define PERFORMANCE_MARKER_START(name)          netflix::performanceMarkerStart(name)
#define PERFORMANCE_MARKER_END()                netflix::performanceMarkerEnd()
#define PERFORMANCE_MARKER_SCOPED(...)          netflix::ScopedPerformanceMarker PERFORMANCE_MARKER_COMBINE(_ispm, __LINE__)(__VA_ARGS__)

// these are not provided by nrdp, they must be implemented by the platform
void performanceMarkerStart(const char * name, bool callerOwnsString = true);
void performanceMarkerEnd();

struct ScopedPerformanceMarker
{
    inline explicit ScopedPerformanceMarker(const char * name, bool callerOwnsString = true) { performanceMarkerStart(name, callerOwnsString); }
    inline ~ScopedPerformanceMarker() { performanceMarkerEnd(); }
};

#endif

} // namespace netflix

#endif
