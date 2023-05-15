/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef LRU_H
#define LRU_H

#include <map>
#include <list>
#include <assert.h>

namespace netflix {

template<typename T>
class LRUSizeof
{
public:
    int operator()(const T& t) { return sizeof(t); }
};

template<typename T>
class LRUOne
{
public:
    int operator()(const T&) { return 1; }
};

template<typename T>
class LRUDestructor
{
public:
    void operator()(T&) { }
};

template<typename T>
class LRUPurgeAllow
{
public:
    bool operator()(const T&) { return true; }
};

template<typename Key, typename Value, typename Cost = LRUSizeof<Value>, typename Destructor = LRUDestructor<Value>, typename PurgeAllow = LRUPurgeAllow<Value> >
class LRU
{
private:
    struct Entry;

public:
    class iterator;

    class const_iterator
    {
    public:
        typedef std::map<Key, Entry> Map;
        const Map* map;
        typename std::list<Key>::const_iterator listit;
        const_iterator(const Map& m, typename std::list<Key>::const_iterator at)
            : map(&m), listit(at)
        {
        }
        const_iterator(const const_iterator& other)
            : map(other.map), listit(other.listit)
        {
        }
        const_iterator(const iterator& other)
            : map(other.map), listit(other.listit)
        {
        }

        const Key& key() const { return *listit; }
        const Value& value() const { assert(map->find(*listit) != map->end()); return map->find(*listit)->second.value; }
        const Value& operator*() const { return value(); }
        const Value* operator->() const { return &value(); }

        bool operator==(const const_iterator& other) const { return listit == other.listit; }
        bool operator!=(const const_iterator& other) const { return listit != other.listit; }
        const_iterator& operator++() { ++listit; return *this; }
        const_iterator operator++(int) { const_iterator copy(listit); ++listit; return copy; }

    private:
        friend class LRU<Key, Value, Cost, Destructor, PurgeAllow>;
    };

    class iterator
    {
    public:
        typedef std::map<Key, Entry> Map;
        Map* map;
        typename std::list<Key>::iterator listit;
        iterator(Map& m,
                 typename std::list<Key>::iterator at)
            : map(&m), listit(at)
        {
        }
        iterator(const iterator& other)
            : map(other.map), listit(other.listit)
        {
        }
        iterator& operator=(const iterator& other)
        {
            map = other.map;
            listit = other.listit;
            return *this;
        }

        const Key& key() const { return *listit; }
        Value& value() { return (*map)[*listit].value; }
        const Value& constValue() const { assert(map->find(*listit) != map->end()); return map->find(*listit)->second.value; }
        const Value& value() const { return constValue(); }
        const Value& operator*() const { return value(); }
        const Value* operator->() const { return &value(); }
        Value& operator*() { return value(); }
        Value* operator->() { return &value(); }

        bool operator==(const iterator& other) const { return listit == other.listit; }
        bool operator!=(const iterator& other) const { return listit != other.listit; }
        bool operator==(const const_iterator& other) const { return listit == other.listit; }
        bool operator!=(const const_iterator& other) const { return listit != other.listit; }
        iterator& operator++() { ++listit; return *this; }
        iterator operator++(int) { iterator copy(listit); ++listit; return copy; }

    private:
        friend class LRU<Key, Value, Cost, Destructor, PurgeAllow>;
    };

    LRU(int maxSize = -1) : mCurSize(0), mMaxSize(maxSize) { }
    ~LRU() { }

    void setMaxSize(int max);

    std::pair<iterator, bool> insert(const Key& key, const Value& value);

    void erase(const Key& key);
    iterator erase(const iterator& it);

    bool purge(int targetSize);
    void purgeOne();

    void clear();

    int count() const { return mKeys.size(); }

    Value value(const Key& key) const;
    Value operator[](const Key& key) const { return value(key); }

    iterator find(const Key& key);
    const_iterator find(const Key& key) const;

    iterator begin() { return iterator(mValues, mKeys.begin()); }
    iterator end() { return iterator(mValues, mKeys.end()); }
    const_iterator begin() const { return const_iterator(mValues, mKeys.begin()); }
    const_iterator end() const { return const_iterator(mValues, mKeys.end()); }

    int maximumSize() const { return mMaxSize; }
    int currentSize() const { return mCurSize; }

private:
    void removeFront();

private:
    struct Entry
    {
        Entry() : cost(0) { }
        Entry(const typename std::list<Key>::iterator& k, const Value& v, int c)
            : key(k), value(v), cost(c)
        {
        }

        typename std::list<Key>::iterator key;
        Value value;
        int cost;
    };

    static Cost sCost;
    int mCurSize, mMaxSize;
    std::map<Key, Entry> mValues;
    mutable std::list<Key> mKeys;
};

template<typename Key, typename Value, typename Cost, typename Destructor, typename PurgeAllow>
Cost LRU<Key, Value, Cost, Destructor, PurgeAllow>::sCost;

template<typename Key, typename Value, typename Cost, typename Destructor, typename PurgeAllow>
inline void LRU<Key, Value, Cost, Destructor, PurgeAllow>::setMaxSize(int max)
{
    mMaxSize = max;
    if (mMaxSize >= 0 && mCurSize > mMaxSize) {
        purge(mMaxSize);
    }
}

template<typename Key, typename Value, typename Cost, typename Destructor, typename PurgeAllow>
inline std::pair<typename LRU<Key, Value, Cost, Destructor, PurgeAllow>::iterator, bool>
LRU<Key, Value, Cost, Destructor, PurgeAllow>::insert(const Key& key, const Value& value)
{
    typename std::map<Key, Entry>::iterator prev = mValues.find(key);
    if (prev != mValues.end()) { // remove existing
        mCurSize -= prev->second.cost;
        assert(mCurSize >= 0);
        mKeys.erase(prev->second.key);
        Destructor dtor;
        dtor(prev->second.value);
        mValues.erase(prev);
    }

    const int cost = sCost(value);
    if (mMaxSize >= 0 && mCurSize + cost > mMaxSize && !purge(mMaxSize - cost)) {
        return std::pair<iterator, bool>(end(), false);
    }
    mCurSize += cost;
    assert(mCurSize <= mMaxSize || mMaxSize < 0);
    Entry e(mKeys.insert(mKeys.end(), key), value, cost);
    mValues.insert(std::make_pair(key, e));
    return std::pair<iterator, bool>(iterator(mValues, e.key), true);
}

template<typename Key, typename Value, typename Cost, typename Destructor, typename PurgeAllow>
inline void LRU<Key, Value, Cost, Destructor, PurgeAllow>::removeFront()
{
    Key& key = mKeys.front();
    typename std::map<Key, Entry>::iterator value = mValues.find(key);
    assert(value != mValues.end());
    mCurSize -= value->second.cost;
    assert(mCurSize >= 0);
    Destructor dtor;
    dtor(value->second.value);
    mValues.erase(value);
    mKeys.pop_front();
}

template<typename Key, typename Value, typename Cost, typename Destructor, typename PurgeAllow>
inline bool LRU<Key, Value, Cost, Destructor, PurgeAllow>::purge(int targetSize)
{
    if (targetSize < 0)
        return false;
    else if (!targetSize) {
        clear();
        return true;
    }
    PurgeAllow allow;
    typename LRU<Key, Value, Cost, Destructor, PurgeAllow>::iterator it = LRU<Key, Value, Cost, Destructor, PurgeAllow>::begin();
    while (it != LRU<Key, Value, Cost, Destructor, PurgeAllow>::end()) {
        if (mCurSize <= targetSize)
            break;
        if (allow(*it))
            it = LRU<Key, Value, Cost, Destructor, PurgeAllow>::erase(it);
        else
            ++it;
    }
    return mCurSize <= targetSize;
}

template<typename Key, typename Value, typename Cost, typename Destructor, typename PurgeAllow>
inline void LRU<Key, Value, Cost, Destructor, PurgeAllow>::purgeOne()
{
    if (mKeys.empty())
        return;
    removeFront();
}

template<typename Key, typename Value, typename Cost, typename Destructor, typename PurgeAllow>
inline void LRU<Key, Value, Cost, Destructor, PurgeAllow>::clear()
{
    mKeys.clear();
    Destructor dtor;
    typename std::map<Key, Entry>::iterator it = mValues.begin();
    while (it != mValues.end()) {
        dtor(it->second.value);
        ++it;
    }
    mValues.clear();
    mCurSize = 0;
}

template<typename Key, typename Value, typename Cost, typename Destructor, typename PurgeAllow>
inline Value LRU<Key, Value, Cost, Destructor, PurgeAllow>::value(const Key& key) const
{
    typename std::map<Key, Entry>::const_iterator value = mValues.find(key);
    if (value == mValues.end())
        return Value();
    mKeys.splice(mKeys.end(), mKeys, value->second.key);
    return value->second.value;
}

template<typename Key, typename Value, typename Cost, typename Destructor, typename PurgeAllow>
void LRU<Key, Value, Cost, Destructor, PurgeAllow>::erase(const Key& key)
{
    typename std::map<Key, Entry>::iterator value = mValues.find(key);
    if (value == mValues.end())
        return;
    (void)erase(value);
}

template<typename Key, typename Value, typename Cost, typename Destructor, typename PurgeAllow>
inline typename LRU<Key, Value, Cost, Destructor, PurgeAllow>::iterator LRU<Key, Value, Cost, Destructor, PurgeAllow>::erase(const iterator& it)
{
    assert(it != end());
    iterator copy(it);
    Destructor dtor;
    dtor(copy.value());
    ++copy;

    typename std::map<Key, Entry>::iterator value = mValues.find(it.key());
    assert(value != mValues.end());
    mCurSize -= value->second.cost;
    mKeys.erase(value->second.key);
    mValues.erase(value);
    return copy;
}

template<typename Key, typename Value, typename Cost, typename Destructor, typename PurgeAllow>
inline typename LRU<Key, Value, Cost, Destructor, PurgeAllow>::iterator LRU<Key, Value, Cost, Destructor, PurgeAllow>::find(const Key& key)
{
    typename std::map<Key, Entry>::iterator value = mValues.find(key);
    if (value == mValues.end())
        return end();
    assert(value->second.cost);
    return iterator(mValues, value->second.key);
}

template<typename Key, typename Value, typename Cost, typename Destructor, typename PurgeAllow>
inline typename LRU<Key, Value, Cost, Destructor, PurgeAllow>::const_iterator LRU<Key, Value, Cost, Destructor, PurgeAllow>::find(const Key& key) const
{
    typename std::map<Key, Entry>::const_iterator value = mValues.find(key);
    if (value == mValues.end())
        return end();
    return const_iterator(mValues, value->second.key);
}

}

#endif
