/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ReadDir.h"
#include <unistd.h>
#include <errno.h>
#include <nrdbase/Log.h>

using namespace netflix;

bool ReadDir::visitorHelper(std::string path, bool *stop)
{
    *stop = false;
    DIR *d = opendir(path.c_str());
    if (!d)
        return false;

    char buf[PATH_MAX + sizeof(dirent) + 1];
    dirent *dbuf = reinterpret_cast<dirent*>(buf);

    dirent *p;
    if (!endsWith(path, '/'))
        path += '/';
    const int s = path.size();
    path.reserve(s + 128);
    std::vector<std::string> recurseDirs;
    while (!readdir_r(d, dbuf, &p) && p) {
        if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
            continue;
        path.resize(s);
        path.append(p->d_name);

        Entry file;
        if (stat(path.c_str(), &file.mStat) == -1)
            continue;
        file.absolutePath = path;
        file.mFileNameLen = strlen(p->d_name);

        switch (file.mStat.st_mode & S_IFMT) { // not following symlinks
        case S_IFDIR:
            file.type = Directory;
            assert(!endsWith(path, "/"));
            file.absolutePath.append("/");
            path.append("/");
            ++file.mFileNameLen;
            break;
        case S_IFREG:
            file.type = File;
            break;
        default:
            file.type = Other;
            break;
        }

        switch (visitEntry(file)) {
        case Stop:
            closedir(d);
            *stop = true;
            return true;
        case Recurse:
            if (file.type == Directory)
                recurseDirs.push_back(path);
            break;
        case Continue:
            break;
        }
    }
    closedir(d);
    for (std::vector<std::string>::const_iterator it = recurseDirs.begin(); it != recurseDirs.end(); ++it) {
        visitorHelper(*it, stop);
        if (*stop)
            break;
    }
    return true;
}

bool ReadDir::visit(const std::string &pp)
{
    bool ignored;
    return ReadDir::visitorHelper(pp, &ignored);
}

class EntriesHelper : public ReadDir
{
public:
    virtual Result visitEntry(const Entry &file)
    {
        switch (file.type) {
        case Invalid:
            break;
        case File:
            if (!(flags & SkipFiles)) {
                entries.push_back(file);
            }
            break;
        case Directory:
            if (!(flags & SkipDirs))
                entries.push_back(file);
            return flags & RecurseDirectories ? Recurse : Continue;
        case Other:
            break;
        }
        return Continue;
    }

    unsigned int flags;
    std::vector<Entry> entries;
};

std::vector<ReadDir::Entry> ReadDir::entries(const std::string &dir, unsigned int flags)
{
    EntriesHelper helper;
    helper.flags = flags;
    helper.visit(dir);
    return helper.entries;
}

class RecursiveRemoveDirectory : public ReadDir
{
public:
    RecursiveRemoveDirectory()
        : success(true)
    {}
    virtual Result visitEntry(const Entry &file)
    {
        if (file.type == Directory) {
            if (!ReadDir::recursiveRemoveDirectory(file.absolutePath)) {
                success = false;
                return Stop;
            }
        } else if (unlink(file.absolutePath.c_str())) {
            success = false;
            return Stop;
        }
        return Continue;
    }
    bool success;
};

bool ReadDir::recursiveRemoveDirectory(const std::string &path)
{
    RecursiveRemoveDirectory d;
    return d.visit(path) && d.success && !rmdir(path.c_str());
}

bool ReadDir::recursiveMakeDirectory(const std::string &dir, mode_t mode)
{
    struct stat statBuf;
    if (!::stat(dir.c_str(), &statBuf))
        return true;

    std::string subdir = dir;
    if (subdir.size() < PATH_MAX && subdir.length()) {
        if (subdir[subdir.length()-1] != '/')
            subdir += '/';
        for (size_t i = 1; i < subdir.length(); ++i) {
            if (subdir[i] == '/') {
                subdir[i] = 0;
                const int r = mkdir(subdir.c_str(), mode);
                if (r && errno != EEXIST && errno != EISDIR)
                    return false;
                subdir[i] = '/';
            }
        }
        return true;
    }
    return false;
}

ReadDir::FileType ReadDir::exists(const std::string &path)
{
    struct stat st;
    if (stat(path.c_str(), &st) == -1)
        return Invalid;
    switch (st.st_mode & S_IFMT) { // not following symlinks
    case S_IFDIR:
        return Directory;
    case S_IFREG:
        return File;
    default:
        break;
    }
    return Other;
}

const char *ReadDir::fileTypeToString(FileType type)
{
    switch (type) {
    case Invalid: return "Invalid";
    case File: return "File";
    case Directory: return "Directory";
    case Other: return "Other";
    }
    return "";
}
