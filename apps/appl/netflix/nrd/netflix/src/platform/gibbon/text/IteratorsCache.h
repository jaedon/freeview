/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ITERATORSCACHE_H_
#define ITERATORSCACHE_H_

#include <nrd/AppLog.h>
#include <gibbon/config.h>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <unicode/brkiter.h>


namespace netflix {
namespace gibbon {

class IteratorsCache
{
public:
    class Iterator
    {
    public:
        enum { DONE = -1 };

        Iterator(const std::string& locale);
        ~Iterator();

        bool isValid() const;

        //void setText(const char* text, int length);
        void setText(const icu::UnicodeString &text);


        int first();
        int next();
        int last();

    private:
        icu::BreakIterator* icu;
    };

    typedef std::map< std::string,std::vector<Iterator*> > Cache;

    static Iterator* getLineIterator(const std::string& locale);
    static void putIterator(const std::string& locale, Iterator* iterator);
    static void clearIterators();
};

}} // namespace netflix::gibbon

#endif /* ITERATORSCACHE_H_ */
