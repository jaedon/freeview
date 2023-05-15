# (c) 1997-2015 Netflix, Inc.  All content herein is protected by
# U.S. copyright and other applicable intellectual property laws and
# may not be copied without the express permission of Netflix, Inc.,
# which reserves all rights.  Reuse of any of this content for any
# purpose without the permission of Netflix, Inc. is strictly
# prohibited.

set(FILES
    SimpleStreamingHeuristics.cpp
    SimpleLocationSelector.cpp
    SimpleStreamSelector.cpp
    SimpleConnectionConfiguration.cpp
    AseSimpleJsonArraySerializer.cpp
    BaseConnectionConfiguration.cpp
    AutoTuneConnectionConfiguration.cpp
    SimpleStreamingStatistics.cpp
    SingleStreamingLocationSet.cpp
    StreamingLocationLevel.cpp
    OrigStreamingLocationLevel.cpp
    ContStreamingLocationLevel.cpp
    LocationSetMonitor.cpp
    SimpleLocationSetMonitor.cpp
    ServingUrl.cpp
    ServingStream.cpp
    ThroughputDistribution.cpp
    ThroughputHistory.cpp
    LocationSetMonitorAggregate.cpp
    SimpleLocationSetMonitorAggregate.cpp
    ThroughputDistributionPredictor.cpp
)

add_library(nrdase_heuristics ${FILES})

create_install_target(
    TARGET nrdase_heuristics
    DESCRIPTION "StreamingEngine library"
    COMPONENT "NRDP"
    PUBLIC_HEADERS "${PUBLIC_HEADERS}"
)
