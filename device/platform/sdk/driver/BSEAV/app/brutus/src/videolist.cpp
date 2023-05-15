/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: videolist.cpp $
 * $brcm_Revision: 32 $
 * $brcm_Date: 10/31/11 4:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/videolist.cpp $
 * 
 * 32   10/31/11 4:10p jtna
 * SW7344-216: if .nav file exists, use it as index
 * 
 * 31   2/16/10 5:41p jtna
 * SW3556-1051: use video decoder timestamp reordering
 * 
 * 30   7/13/09 1:49p vsilyaev
 * PR 55989: Handle 'self-indexed' MPEG-2 TS files
 * 
 * 29   6/25/09 5:36p katrep
 * PR52753: Allow brutus is create info files, even if file has encryption
 * flags is turned on and DRM suppoort is not enabled. Some file have
 * incorrect encrytption flag
 * 
 * 28   6/25/09 11:32a erickson
 * PR54342: use media probe to determine if there's an embedded index
 *
 * 27   1/28/09 7:27p vsilyaev
 * PR 51670: Sort audio tracks prior to writing into info file
 *
 * 26   1/28/09 12:10p vsilyaev
 * PR 51166: Fixed mapping of programs to info files
 *
 * 25   12/9/08 2:59p vsilyaev
 * PR 49704: Write multiple audio tracks to info file
 *
 * 24   8/6/08 7:19p vsilyaev
 * PR 45201: Use 64 bit ready functions to support DIVX HD (where files
 * could be larger than 4GB)
 *
 * 23   7/22/08 11:32a jtna
 * PR44493: Add 192byte TS playback support in Brutus
 *
 * 22   5/7/08 11:51a erickson
 * PR42329: remove false warning
 *
 * 21   3/18/08 4:05p erickson
 * PR40307: added thumbnail feature
 *
 * 20   3/17/08 3:17p erickson
 * PR40307: remove old, unused code before adding new
 *
 * 19   3/4/08 12:52p vsilyaev
 * PR 39818: Added MKV format
 *
 * 18   2/21/08 2:13p ahulse
 * PR25542: Enable retreval of video object by media filename
 *
 * 17   12/6/07 2:08p erickson
 * PR37038: add CONTINUOUS_RECORD_DURATION, remove unused fixedSize and
 * CIRCFILE.
 *
 * 16   9/25/07 2:48p gmohile
 * PR 34640 : Add Divx DRM support
 *
 * 15   8/28/07 1:19p ahulse
 * PR33997: Fix stream info corruption
 *
 * 14   7/26/07 8:40p erickson
 * PR33552: added strcmp option if sortIndexes match
 *
 * 13   7/26/07 7:19p erickson
 * PR32813: make .info failure report the file
 *
 * 12   7/25/07 1:51p vsilyaev
 * PR 32813: Separated public and internal API of bmedia_probe
 *
 * 11   7/19/07 11:09p vsilyaev
 * PR 32813: Don't scan index files. Verify strings before dereferencing
 * them.
 *
 * 10   7/19/07 12:17p vsilyaev
 * PR 32813: Renamed bmedia_track to bmedia_track_type
 *
 * 9   7/18/07 7:06p vsilyaev
 * PR 32813: Modified to scan all supported media files, fixed memory leak
 *
 * 8   7/10/07 12:44p erickson
 * PR32813: convert from asfpars to bmedia_probe
 *
 * 7   7/9/07 3:47p erickson
 * PR32256: added wma_pro
 *
 * 6   6/7/07 12:03p gmohile
 * PR 29933 : fix info file creation
 *
 * 5   5/31/07 2:48p gmohile
 * PR 25109: disable AVI info file creation
 *
 * 4   4/25/07 11:27a gmohile
 * PR 29933: add index file for AVI
 *
 * 3   4/18/07 4:25p gmohile
 * PR 29933: Create info files for AVI files.
 *
 * 2   3/26/07 11:05a erickson
 * PR26348: don't hardcode wma std. read the codec_id and assign
 * appropriately.
 *
 * 1   3/14/07 10:16a erickson
 * PR26267: refactor Brutus into separate VideoList and PlaybackScreen
 *
 ***************************************************************************/
#include "videolist.h"
#ifndef _WIN32_WCE
#include <sys/stat.h>
#include <dirent.h>
#endif
#include "bstd.h"
#include "bkni.h"
#include "bmedia_probe.h"
#include "bmpeg2ts_probe.h"
#if B_HAS_ASF
#include "basf_probe.h"
#endif
#if B_HAS_AVI
#include "bavi_probe.h"
#endif
#include "bfile_stdio.h"
#include "bhash.h"

BDBG_MODULE(videolist);

#ifdef _WIN32_WCE
#define DEFAULT_VIDEODIR "\\HardDisk\\Brutus\\videos"
#else
#define DEFAULT_VIDEODIR "videos"
#endif

#define DEFAULT_VIDEODIR_LABEL "local"

static int sortVideos(Video *v1, Video *v2) {
    /* sortIndex takes precedence */
    if (v1->sortIndex > v2->sortIndex) {
        return -1;
    }
    else if (v1->sortIndex < v2->sortIndex) {
        return 1;
    }
    else {
        /* otherwise alphabetize */
        return strcmp(v1->title, v2->title);
    }
}

VideoList::VideoList(bthumbnail_manager_t thumbnail_manager)
{
    _thumbnail_manager = thumbnail_manager;
    _lastFileNumber = 0;
}

struct filename {
    BHASH_ENTRY(filename) hash_link;
    char name[240];
};

BHASH_HEAD(filename_set, filename, 128);
/* One-at-a-Time Hash by Bob Jenkins */
static unsigned string_hash(const char *str)
{
    unsigned hash;
    unsigned char ch;
    for( hash=0; '\0' != (ch=*str++);) {
        hash += ch;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

static inline unsigned string_cmp(const char *s, const filename *f)
{
    return strcmp(s,f->name)==0;
}

BHASH_GENERATE(filename_set, const char *, filename, hash_link, string_hash, string_cmp)

static int b_write_info(MString &mediafile, unsigned program, filename_set *infos, Video *video, const bmedia_probe_stream *stream)
{
    int retVal = 0;
    char *dot = strrchr(mediafile, '.');
    MString infofile;

    if(dot) {
        infofile.strncpy(mediafile, dot - mediafile);
    } else {
        infofile = mediafile;
    }

    if(program>0) {
        char buf[8];
        BKNI_Snprintf(buf,sizeof(buf),".%u", program);
        infofile += buf;
    }
    infofile += ".info";

    BDBG_MSG(("b_write_info(%s, %s) video:0x%02x audio:0x%02x", mediafile.s(), infofile.s(), video->mpeg.video[0].pid, video->mpeg.audio[0].pid ));

    const char *strStart = strrchr(mediafile, '/');
    if (strStart) {
        strStart++;
        video->mediafile = strStart;
    } else {
        video->mediafile = mediafile;
    }

    video->mpeg.mpeg_type = stream->type;

    if (stream->index == bmedia_probe_index_available || stream->index == bmedia_probe_index_required || stream->index == bmedia_probe_index_self) {
        MString idxfile = mediafile;
        idxfile.truncate(idxfile.findRev('.'));
        idxfile += ".nav";
        if (access((const char*)idxfile, F_OK) != -1) { /* search with full name */
            idxfile = video->mediafile;
            idxfile.truncate(idxfile.findRev('.'));
            idxfile += ".nav";
            video->indexfile = (const char*)idxfile; /* add only relative name */
        }
        else {
            video->indexfile = video->mediafile;
        }
    }

    /* TODO: title, description, artist */

    if(stream->duration) {
        video->fixedLength = stream->duration;
    } else {
        video->fixedLength = 10 * 1000; /* temp: hard coded to 10 seconds */
    }

#if !B_HAS_DRM
    //check if not asf file before adding
    if (video->mpeg.mpeg_type == bstream_mpeg_type_asf && video->isForPurchase)
    {
        BDBG_WRN(("Encrypted ASF file"));
        retVal = -1;
    } else
#endif /* B_HAS_DRM */

#if !B_HAS_DIVX_DRM
    if (video->mpeg.mpeg_type == bstream_mpeg_type_avi && video->isForPurchase)
    {
        BDBG_WRN(("Encrypted AVI file"));
        retVal = -1;
    } 
#endif

    if (video->deriveFormatName()) {
        BDBG_ERR(("Unable to determine Brutus .info format for %s", (const char *)mediafile));
        retVal = -1;
    }
    else {
        // generate .info file, then throw the Video away. it will be recreated.
        BDBG_MSG(("write info file: %s", infofile.s()));
        retVal = video->write(infofile.s());
        if(retVal==0) {
            struct filename *f = new filename();
            BDBG_ASSERT(f);
            strncpy(f->name, infofile.s(), sizeof(f->name)-1);
            f->name[sizeof(f->name)-1]='\0';
            BHASH_ADD(filename_set, infos, f->name, f);
        }
    }

    delete video;

    return retVal;

}

static int b_create_info(MString &mediafile, filename_set *infos, bmedia_probe_t probe)
{
    int retVal = 0;
    BDBG_ASSERT(infos);
    BDBG_ASSERT(probe);

    bmedia_probe_config probe_config;
    const bmedia_probe_stream *stream;
    const bmedia_probe_track *track;
    bfile_io_read_t  fd=NULL;

    FILE *fin = fopen64(mediafile,"rb");
    if (!fin) {
        BDBG_MSG(("can't open media file '%s'", mediafile.s()));
        return -1;
    }

    fd = bfile_stdio_read_attach(fin);

    bmedia_probe_default_cfg(&probe_config);
    probe_config.file_name = mediafile;
    probe_config.type = bstream_mpeg_type_unknown;
    stream = bmedia_probe_parse(probe, fd, &probe_config);

    /* now stream is either NULL, or stream descriptor with linked list of audio/video tracks */
    bfile_stdio_read_detach(fd);

    fclose(fin);
    if(!stream) {
        BDBG_WRN(("can't parse stream '%s'", mediafile.s()));
        return -1;
    }

    Video *video = new Video;

    if (stream->type == bstream_mpeg_type_ts) {
        if ((((bmpeg2ts_probe_stream*)stream)->pkt_len) == 192) {
            video->timestamp_enabled = true;
        }
    }
    /* populate other structure with first audio and video tracks */
    unsigned prev_program=0;
    unsigned audio_program=0;
    unsigned video_program=0;
    for(track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link)) {
        unsigned i;
        if(track!=BLST_SQ_FIRST(&stream->tracks)&& prev_program!=track->program) {
            retVal = b_write_info(mediafile, prev_program, infos, video, stream);
            audio_program = 0;
            video_program = 0;
            video = new Video;
        }
        prev_program = track->program;
#if B_HAS_ASF
        if(stream->type == bstream_mpeg_type_asf) {
            video->isForPurchase = video->isForPurchase || ((basf_probe_track *)track)->encryptedContentFlag;
        }
#endif
#if B_HAS_AVI
        if(stream->type == bstream_mpeg_type_avi) {
            video->isForPurchase = video->isForPurchase || ((bavi_probe_track *)track)->encryptedContentFlag;
        }
#endif
        switch(track->type) {
            case bmedia_track_type_audio:
                if(track->info.audio.codec != baudio_format_unknown && audio_program<sizeof(video->mpeg.audio)/sizeof(video->mpeg.audio[0])) {
                    for(i=audio_program;i>0;i--) { /* sort audio programms */
                        if(track->number > video->mpeg.audio[i-1].pid) {
                            /* list was already sorted so stop here */
                            break;
                        }
                       /* swap two entries */
                        video->mpeg.audio[i].pid = video->mpeg.audio[i-1].pid;
                        video->mpeg.audio[i].format = video->mpeg.audio[i-1].format;
                    }
                    video->mpeg.audio[i].pid = track->number;
                    video->mpeg.audio[i].format = track->info.audio.codec;
                    audio_program++;
                }
                break;
            case bmedia_track_type_video:
                if(track->info.video.codec != bvideo_codec_unknown && video_program<sizeof(video->mpeg.video)/sizeof(video->mpeg.video[0])) {
                    video->mpeg.video[video_program].pid = track->number;
                    video->mpeg.video[video_program].format = track->info.video.codec;
                    video->mpeg.timestamp_order = (bdecode_timestamp_order) track->info.video.timestamp_order;
                    video_program++;
                }
                break;
            case bmedia_track_type_pcr:
                video->mpeg.pcr_pid=track->number;
                break;
            default:
                break;
        }
    }
    retVal = b_write_info(mediafile, prev_program, infos, video, stream);
    bmedia_probe_stream_free(probe, stream);
    return retVal;

}

Video *
VideoList::parse_infofile(MString &infofilename, const char *dirstr, const char *label)
{
    Video *vid = find(infofilename);
    if (vid) {
        vid->marked = 1;
        return vid;
    }
    BDBG_MSG(("new Video for %s", infofilename.s()));
    vid = new Video();
    if (vid->read(infofilename.s()) == -1)
    {
        BDBG_WRN(("cannot read video info file: %s", infofilename.s()));
        delete vid;
        return NULL;
    }
    vid->setPath(dirstr);
    vid->dirLabel = label;
    _videoList.add(vid);
    vid->marked = 1;

    vid->createThumbnails(_thumbnail_manager);

    return vid;
}

void VideoList::refreshFromDisk() {
    if (_dirs.total() == 0) {
        addDir(defaultVideoDirLabel(), defaultVideoDir());
    }

    // unmark all videos
    for (Video *vid = _videoList.first(); vid; vid = _videoList.next()) {
        vid->marked = 0;
    }
    bmedia_probe_t probe;
    probe = bmedia_probe_create();
    BDBG_ASSERT(probe);

    filename_set *infos = new filename_set();
    BDBG_ASSERT(infos);
    filename_set *data = new filename_set();
    BDBG_ASSERT(data);


    const char *dirstr, *label;
    for (dirstr = _dirs.first(), label = _labels.first();
        dirstr;
        dirstr = _dirs.next(), label = _labels.next())
    {
#ifndef _WIN32_WCE
        DIR *dir = opendir((char *) dirstr);
        if (!dir) {
            BDBG_WRN(("Directory '%s' not found", dirstr));
            continue;
        }
        BHASH_INIT(filename_set, infos);
        BHASH_INIT(filename_set, data);

        struct dirent *d;
        struct filename *f;
        while ((d = readdir(dir))) {
            if(strcmp(d->d_name, ".")==0 || strcmp(d->d_name, "..")==0) {
                continue; /* skip "." and ".." */
            }
            char *dot = rindex(d->d_name, '.');
            struct filename *f = new filename();
            BDBG_ASSERT(f);
            strncpy(f->name, d->d_name, sizeof(f->name)-1);
            f->name[sizeof(f->name)-1]='\0';

            if(dot && (strcmp(dot,".info")==0)) {
                BHASH_ADD(filename_set, infos, f->name, f);
            } else {
                BHASH_ADD(filename_set, data, f->name, f);
            }
        }
        closedir(dir);

        /* 1. parse existing info files */
        BHASH_FOREACH_BEGIN(filename_set,infos,filename,f,hash_link)
            BDBG_MSG(("old infofile %s", f->name));
            MString infofilename = MString(dirstr) + f->name;
            Video *video = parse_infofile(infofilename, dirstr, label);

            if(video) {
                /* remove from the list known media and index files */
                if(!video->mediafile.isNull()) {
                    filename *media = BHASH_FIND(filename_set, data, video->mediafile);
                    /* look for the mediafile and delete it from the data container */
                    if(media) {
                        BDBG_MSG(("known media file %s", media->name));
                        BHASH_REMOVE(filename_set, data, media->name);
                        delete media;
                    }
                }
                if(!video->indexfile.isNull()) {
                    BDBG_MSG(("index file %s", video->indexfile.s()));
                    filename *index= BHASH_FIND(filename_set, data, video->indexfile);
                    if(index) {
                        BDBG_MSG(("known index file %s", video->indexfile.s()));
                        BHASH_REMOVE(filename_set, data, index->name);
                        delete index;
                    }
                }
            }
            BHASH_REMOVE(filename_set, infos, f->name);
            delete f;
        BHASH_FOREACH_END()

        /* 2. parse media files */
        BHASH_FOREACH_BEGIN(filename_set,data,filename,f,hash_link)
            MString mediafilename = MString(dirstr) + f->name;
            BDBG_MSG(("new media file %s", f->name));
            BHASH_REMOVE(filename_set, data, f->name);
            b_create_info(mediafilename, infos, probe);
            delete f;
        BHASH_FOREACH_END()

        /* 3. parse new info files */
        BHASH_FOREACH_BEGIN(filename_set,infos,filename,f,hash_link)
            BDBG_MSG(("new infofile %s", f->name));
            MString infofilename = f->name;
            parse_infofile(infofilename, dirstr, label);
            BHASH_REMOVE(filename_set, infos, f->name);
            delete f;
        BHASH_FOREACH_END()

#if 0

            MString infofilename = MString(dirstr) + d->d_name;
            bool bNewInfoFile = false;

            bNewInfoFile = false;

            //TODO: expand this list, or scan all non-.info's which aren't referenced by .info's. that would be a two-pass algo.
            if (dot &&
                ((!strcmp(dot, ".wmv")) ||
                (!strcmp(dot, ".wma")) ||
                (!strcmp(dot, ".mp4")) ||
                (!strcmp(dot, ".avi")) ||
                (!strcmp(dot, ".asf"))))
            {
                MString newinfofilename;
                if (!b_create_info(probe, infofilename, newinfofilename)) {
                    //.info file has been generated from media file so
                    //change infofilename to new .info file
                    bNewInfoFile = true;
                    infofilename = newinfofilename;
                }
            }

            if (dot && !strcmp(dot, ".info") || bNewInfoFile) {
        }
        closedir(dir);
#endif
#else
    TCHAR*     wchFileName;
    CHAR        chFileName[MAX_PATH];
    WIN32_FIND_DATA findData;
    BOOL    bKeepSearching = TRUE;
    int str_size = strlen(dirstr);

    // Alloc enough space to hold dir name + "\*.info\0"
    wchFileName = (TCHAR*)malloc(sizeof(TCHAR) *(str_size +9));
    mbstowcs(wchFileName, dirstr, str_size+9);
    wcscat(wchFileName, L"\\*.info");

    HANDLE hFile = FindFirstFile(wchFileName, &findData);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        while(bKeepSearching)
        {
            wcstombs(chFileName, findData.cFileName, MAX_PATH);
            MString infofilename = dirstr + MString(chFileName);
            Video *vid = find(infofilename);
            if (!vid) {
                BDBG_MSG(("new Video for %s", infofilename.s()));
                vid = new Video();
                vid->read(infofilename.s());
                vid->setPath(dirstr);
                vid->dirLabel = label;
                _videoList.add(vid);
            }
            vid->marked = 1;
            bKeepSearching = FindNextFile(hFile, &findData);
        }
        FindClose(hFile);
    }
    free(wchFileName);
#endif
    }
    delete infos;
    delete data;
    bmedia_probe_destroy(probe);

    // delete all unmarked videos
    for (Video *vid = _videoList.first(); vid; ) {
        if (!vid->marked) {
            removeVideo(vid);
            vid = _videoList.current();
        }
        else {
            vid = _videoList.next();
        }
    }

    _videoList.sort(sortVideos);

    FIRE(Update);
}

int VideoList::total() const {
    return _videoList.total();
}

Video *VideoList::find(const char *infofile) const {
    MListItr<Video> itr(&_videoList);
    for (Video *vid = itr.first(); vid; vid = itr.next()) {
        if (vid->infofile == infofile)
            return vid;
    }
    return NULL;
}

Video *VideoList::findMedia(const char *mediafile) const {
    MListItr<Video> itr(&_videoList);
    for (Video *vid = itr.first(); vid; vid = itr.next()) {
        if (vid->mediafile == mediafile) {
            BDBG_WRN(("Found mediafile %s", mediafile));
            return vid;
        }
    }
    /* This is a MSG because the script engine calls findMedia() before calling get() */
    BDBG_MSG(("can't find mediafile %s", mediafile));
    return NULL;
}

Video *VideoList::get(int index) const {
    MListItr<Video> itr(&_videoList);
    // They can select from the other end of list with negative value.
    if (index<0) {
        int count = 0;
        for (Video *vid = itr.first(); vid; vid = itr.next()) count++;
        index = count + index + 1;
    }
    Video *video = itr.at(index);
    return video;
}

void VideoList::clear() {
    _videoList.clear();
    FIRE(Update);
}

void VideoList::add(Video *newvideo) {
    int i=0;
    Video *v = _videoList.first();
    while (v) {
        v = _videoList.next();
    }

    _videoList.insert(i,newvideo);
    FIRE(Update);
}

bool VideoList::removeVideo(Video *vid)
{
    int index = _videoList.index(vid);
    if (index == -1) return false;

    _videoList.remove(vid);
    delete vid;
    FIRE(Update);
    return true;
}

bool VideoList::deleteVideo(Video *vid) {
    if (vid->deleteFiles())
        return false;
    return removeVideo(vid);
}

void VideoList::deleteAll()
{
    int i=0;
    _videoList.first();
    while (i < _videoList.total()) {
        Video *video = _videoList.at(i);
        if (video->deleteFiles()) {
            i++; /* skip it. can't remove for some reason. */
        }
        else {
            _videoList.remove(video);
            delete video;
        }
    }
    FIRE(Update);
}

void VideoList::findNextVideoIndex(int *fileId, int *sortIndex) {
    // find next fileId
#ifndef _WIN32_WCE
    struct stat s;

    while (1) {
        char fName[256];
        BKNI_Snprintf(fName, 256, "%sbrutus%d.info", defaultVideoDir(), ++_lastFileNumber);
        if (stat(fName, &s)) {
            break;
        }
        /* file exists, get another */
    }
#else
   TCHAR*     wchFileName;
   int str_size;
   WIN32_FIND_DATA findData;
   HANDLE hFile;

   while (1) {
        char fName[256];
        BKNI_Snprintf(fName, 256, "%s\\brutus%d.info", defaultVideoDir(), ++_lastFileNumber);
        str_size = strlen(fName);
        wchFileName = (TCHAR*)malloc(sizeof(TCHAR) *(str_size +1));
        mbstowcs(wchFileName, fName, str_size+1);

        hFile = FindFirstFile(wchFileName, &findData);
        free(wchFileName);
        if(hFile == INVALID_HANDLE_VALUE)
        {
            FindClose(hFile);
            break;
        }
        /* file exists, get another */
    }

#endif
    *fileId = _lastFileNumber;

    // find next sortIndex
    Video *v = _videoList.first();
    if (v)
        *sortIndex = v->sortIndex +1;
    else
        *sortIndex = 0;
}

void VideoList::addDir(const char *label, const char *dir)
{
    // normalize directories here...they must have a trailing /
    // if not blank
    MString d = dir;
    if (d.length() && d[d.length()-1] != '/')
        d += "/";

    _labels.add(label);
    _dirs.add(d);
}

const char *VideoList::getMediaFileName(int index)
{
    MListItr<Video> itr(&_videoList);

    /* check index for out of range */
    Video *vid = itr.at(index);

    return vid->mediafile;
}

const char *VideoList::defaultVideoDirLabel()
{
    if (_labels.total())
        return _labels[0];
    else
        return DEFAULT_VIDEODIR_LABEL;
}

const char *VideoList::defaultVideoDir()
{
    if (_dirs.total())
        return _dirs[0];
    else
        return DEFAULT_VIDEODIR;
}

FIRE_EV_METHOD(VideoList, Update)
