/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ReadDir_h
#define ReadDir_h

#include <dirent.h>
#include <sys/stat.h>
#include <assert.h>
#include <string>
#include <nrdbase/Noncopyable.h>
#include <nrdbase/StringUtils.h>

namespace netflix {

class ReadDir : public Noncopyable
{
public:
    ReadDir() {}
    virtual ~ReadDir() {}

    bool visit(const std::string &path);

    enum FileType {
        Invalid,
        File,
        Directory,
        Other
    };

    const char *fileTypeToString(FileType type);

    static FileType exists(const std::string &path);
    static bool isFile(const std::string &path) { return exists(path) == File; }
    static bool isDir(const std::string &path) { return exists(path) == Directory; }

    struct Entry {
        FileType type;
        std::string absolutePath;
        const char *fileName() const { return absolutePath.c_str() + (absolutePath.size() - mFileNameLen); }
        const struct stat &statEntry() const { return mStat; }
    private:
        int mFileNameLen;
        struct stat mStat;
        friend class ReadDir;
    };

    enum Result {
        Stop,
        Continue,
        Recurse
    };
    virtual Result visitEntry(const Entry &file) = 0;

    enum EntriesFlag {
        None = 0x0,
        RecurseDirectories = 0x1,
        SkipFiles = 0x2,
        SkipDirs = 0x4,
        SkipOther = 0x8
    };

    static std::vector<Entry> entries(const std::string &dir, unsigned int flags = None);
    static bool recursiveRemoveDirectory(const std::string &path);
    static bool recursiveMakeDirectory(const std::string &path, mode_t mode = S_IRWXU);
private:
    bool visitorHelper(std::string path, bool *stop);
};
}

#endif
