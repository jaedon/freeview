/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef I_PERSISTENT_STORE_H
#define I_PERSISTENT_STORE_H

#include <string>
#include <nrdase/AseCommonDataTypes.h>
#include <nrdbase/Variant.h>
#include <nrdbase/AseErrorCodes.h>

namespace netflix {
namespace ase {
/**
 * @class IPersistentStore
 * @brief The interface to store/retrieve values to/from a persistent store service.
 */
class IPersistentStore
{
public:
    typedef shared_ptr<IPersistentStore> IPersistentStorePtr;

    enum PersistentStoreKey
    {
        DOMAIN_THROUGHPUT, // key for store/fetch domains' measured throughput
        PIPELINE_DETECTION_RESULTS, // format: 0/1:timestamp_in_seconds
        NUM_OF_CONSECUTIVE_PLAYBACK_FAILURES, // number of previous consecutive playback failures
        LKG_NUM_CONNECTIONS // last known good number of network connections
    };

    virtual ~IPersistentStore() {}

    /**
     * retrieve the value based on the given key
     */
    virtual std::string loadValue( PersistentStoreKey key ) = 0;
    virtual Variant loadVariantValue( PersistentStoreKey key ) = 0;

    /** Store a value into the persistent store
     *  @param key [in] The key against which to store the value
     *  @param value [in] The value
     *  @param pSpace [out] The amount of space available, on failure
     *  @return AS_NO_ERROR on success, AS_NOT_ENOUGH_SPACE if not enough space for supplied value
     */
    virtual AseErrorCode saveValue( PersistentStoreKey key, std::string const& value ) = 0;

    /** Store a value into the persistent store
     *  @param key [in] The key against which to store the value
     *  @param value [in] The value
     *  @return AS_NO_ERROR on success, AS_NOT_ENOUGH_SPACE if not enough space for supplied value
     */
    virtual AseErrorCode saveVariantValue( PersistentStoreKey key, Variant const& value ) = 0;


};

typedef IPersistentStore::IPersistentStorePtr IPersistentStorePtr;

int32_t constructPersistentStore(IPersistentStorePtr& pPersistentStore);

}}

#endif
