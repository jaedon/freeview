/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef STREAMING_PERSISTENT_STORE_H
#define STREAMING_PERSISTENT_STORE_H

#include <nrdbase/tr1.h>

#include <nrdase/IPersistentStore.h>

namespace netflix {
namespace mediacontrol {
/**
 * @class StreamingPersistentStore
 * @brief an implementation for IPersistentStore
 */
class StreamingPersistentStore : public netflix::ase::IPersistentStore
{
public:
    typedef shared_ptr<StreamingPersistentStore> StreamingPersistentStorePtr;

    StreamingPersistentStore() {}
    ~StreamingPersistentStore() {}

    virtual std::string loadValue( PersistentStoreKey key );
    virtual Variant loadVariantValue( PersistentStoreKey key );

    /** Store a value into the persistent store
     *  @param key [in] The key against which to store the value
     *  @param value [in] The value
     *  @param pSpace [out] The amount of space available, on failure
     *  @return AS_NO_ERROR on success, AS_NOT_ENOUGH_SPACE if not enough space for supplied value
     */
    virtual AseErrorCode saveValue( PersistentStoreKey key, std::string const& value );

    /** Store a value into the persistent store
     *  @param key [in] The key against which to store the value
     *  @param value [in] The value
     *  @return AS_NO_ERROR on success, AS_NOT_ENOUGH_SPACE if not enough space for supplied value
     */
    virtual AseErrorCode saveVariantValue( PersistentStoreKey key, Variant const& value );

    /** Set the maximum storage usage for a particular key
     *  @param key[in] The key to set the storage usage for
     *  @param maxsize[in] The maximum size in bytes
     */
    void setMaximumSize( PersistentStoreKey key, size_t maxsize );

protected:
    std::map<PersistentStoreKey,size_t> m_maxsizes;
};

typedef StreamingPersistentStore::StreamingPersistentStorePtr StreamingPersistentStorePtr;

}}

#endif
