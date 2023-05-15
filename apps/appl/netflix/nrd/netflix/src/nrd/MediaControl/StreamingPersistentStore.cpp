/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "StreamingPersistentStore.h"
#include "nrdbase/StringCompressor.h"
#include "NrdApplication.h"

using namespace netflix;
using namespace netflix::ase;
using namespace netflix::mediacontrol;

using namespace std;

string StreamingPersistentStore::loadValue( PersistentStoreKey key)
{
    string value;
    switch ( key )
    {
        case DOMAIN_THROUGHPUT:
            value = nrdApp()->readSystemConfiguration(SystemKeys::streamDownloadSpeed);
            break;
        case PIPELINE_DETECTION_RESULTS:
            value = nrdApp()->readSystemConfiguration(SystemKeys::pipelineDetection);
            break;
        case NUM_OF_CONSECUTIVE_PLAYBACK_FAILURES:
            value = nrdApp()->readSystemConfiguration(SystemKeys::playbackFailureRecord);
            break;
        case LKG_NUM_CONNECTIONS:
            value = nrdApp()->readSystemConfiguration(SystemKeys::lkgNumConnections);
            break;
        default:
            value = "";
            break;
    }
    return value;
}

Variant StreamingPersistentStore::loadVariantValue( PersistentStoreKey key )
{
    std::string json;
    NFErr res = StringCompressor::inflateB64( json, loadValue( key ) );
    return res.ok() ? Variant::fromJSON( json ) : Variant();
}

AseErrorCode StreamingPersistentStore::saveValue( PersistentStoreKey key, string const& value )
{
    std::map<PersistentStoreKey,size_t>::const_iterator it = m_maxsizes.find(key);
    if ( it != m_maxsizes.end() )
    {
        size_t const maxsize = it->second;
        if ( maxsize > 0 && value.size() > maxsize )
        {
            return AS_NOT_ENOUGH_SPACE;
        }
    }

    switch ( key )
    {
        case DOMAIN_THROUGHPUT:

            NTRACE(TRACE_MEDIACONTROL,"StreamingPersistentStore: Storing domain throughput history size %zu", value.size() );

            nrdApp()->writeSystemConfiguration(SystemKeys::streamDownloadSpeed, value);
            break;
        case PIPELINE_DETECTION_RESULTS:
            nrdApp()->writeSystemConfiguration(SystemKeys::pipelineDetection, value);
            break;
        case NUM_OF_CONSECUTIVE_PLAYBACK_FAILURES:
            nrdApp()->writeSystemConfiguration(SystemKeys::playbackFailureRecord, value);
            break;
        case LKG_NUM_CONNECTIONS:
            nrdApp()->writeSystemConfiguration(SystemKeys::lkgNumConnections, value);
            break;
        default:
            break;
    }
    return AS_NO_ERROR;
}

AseErrorCode StreamingPersistentStore::saveVariantValue( PersistentStoreKey key, Variant const& value )
{
    std::string compressed;
    NFErr res = StringCompressor::deflateB64( compressed, value.toJSON() );
    if ( !res.ok() )
    {
        NTRACE(TRACE_MEDIACONTROL,"StreamingPersistentStore: encode error %s", res.toString().c_str() );
        return AS_ENCODE_ERROR;
    }

    return saveValue( key, compressed );
}

void StreamingPersistentStore::setMaximumSize( PersistentStoreKey key, size_t maxsize )
{
    m_maxsizes[ key ] = maxsize;
}
