/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "GibbonQABridge.h"
#include <GibbonApplication.h>
#include <SurfaceCache.h>
#include <nrdnet/ResourceManager.h>
#include <nrdnet/DiskCache.h>
#include <nrdbase/ReadDir.h>
#include <errno.h>

using namespace netflix::gibbon;
using namespace netflix::mediacontrol;
using namespace netflix;

#define GibbonQABridgeMethodList(Method)        \
    Method(createFile)                          \
    Method(diskcache)                           \
    Method(listDirectory)                       \
    Method(mkdir)                               \
    Method(remove)

MethodTable(GibbonQABridge, GibbonQABridgeMethodList);

GibbonQABridge::GibbonQABridge()
    : NfObject("qa", Methods::GibbonQABridge::methods, Methods::GibbonQABridge::MethodCount)
{

}


Variant GibbonQABridge::invoke(int method, const Variant &data)
{
    switch (method) {
    case Methods::GibbonQABridge::diskcache: {
        bool ok;
        int id = data.mapValue<int>("id", &ok);
        if (!ok) {
            invalidArgumentError(method, "id");
            return false;
        }
        Variant response;
        response["id"] = id;
        switch (method) {
        case Methods::GibbonQABridge::diskcache:
            if (shared_ptr<DiskCache> diskCache = GibbonApplication::instance()->resourceManager()->diskCache())
                response["data"] = diskCache->toVariant();
            break;

        default:
            return false;
        }
        sendEvent("result", response);
        break; }
    case Methods::GibbonQABridge::createFile: {
        bool ok;
        const std::string file = data.mapValue<std::string>("file", &ok);
        if (!ok) {
            invalidArgumentError(method, "file");
            return false;
        }
        FILE *f = fopen(file.c_str(), "w");
        if (!f) {
            Variant result;
            result["success"] = false;
            result["error"] = "fopen failed";
            result["errno"] = errno;
            return result;
        }
        const DataBuffer contents = data.mapValue<DataBuffer>("contents");
        if (contents.size() && !fwrite(contents.data(), contents.size(), 1, f)) {
            Variant result;
            result["success"] = false;
            result["error"] = "fwrite failed";
            result["errno"] = errno;
            return result;
        }

        fclose(f);
        Variant result;
        result["success"] = true;
        return result; }
    case Methods::GibbonQABridge::mkdir: {
        bool ok;
        const std::string dir = data.mapValue<std::string>("dir", &ok);
        if (!ok) {
            invalidArgumentError(method, "dir");
            return false;
        }
        Variant result;
        if (!mkdir(dir.c_str(), S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)) {
            result["success"] = true;
        } else {
            result["success"] = false;
            result["errno"] = errno;
        }
        return result; }
    case Methods::GibbonQABridge::listDirectory: {
        bool ok;
        const std::string dir = data.mapValue<std::string>("dir", &ok);
        if (!ok) {
            invalidArgumentError(method, "dir");
            return false;
        }

        const std::vector<ReadDir::Entry> entries = ReadDir::entries(dir);
        Variant result;
        Variant &e = result["entries"];
        e = Variant(Variant::Type_StringMap);
        for (std::vector<ReadDir::Entry>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
            Variant entry;
            entry["path"] = it->absolutePath;
            const struct stat &st = it->statEntry();
            switch (it->type) {
            case ReadDir::Invalid:
                break;
            case ReadDir::File:
                entry["type"] = "file";
                entry["size"] = st.st_size;
                break;
            case ReadDir::Directory:
                entry["type"] = "directory";
                break;
            case ReadDir::Other:
                entry["type"] = "other";
                break;
            }
            entry["access"] = st.st_atime;
            entry["modified"] = st.st_mtime;
            entry["statusChange"] = st.st_ctime;
            entry["mode"] = st.st_mode;
            e[it->fileName()] = entry;
        }

        return result; }
    case Methods::GibbonQABridge::remove: {
        bool ok;
        const std::string file = data.mapValue<std::string>("file", &ok);
        if (!ok) {
            invalidArgumentError(method, "file");
            return false;
        }
        Variant result;
        int res = unlink(file.c_str());
        if (res && (errno == EISDIR || errno == EPERM))
            res = rmdir(file.c_str());

        result["success"] = res == 0;
        if (res)
            result["errno"] = errno;
        return result; }
    }

    return true;
}

unsigned GibbonQABridge::requestFlags(const Request &request) const
{
    switch (request.method()) {
    case Methods::GibbonQABridge::createFile:
    case Methods::GibbonQABridge::listDirectory:
    case Methods::GibbonQABridge::mkdir:
    case Methods::GibbonQABridge::remove:
        return Request::Synchronous;
    }
    return 0;
}
