/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: videolist.cpp $
* $brcm_Revision: 14 $
* $brcm_Date: 10/11/12 9:37p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/videolist.cpp $
* 
* 14   10/11/12 9:37p jrubio
* SW7346-1004: add multiple records
* 
* 13   10/11/12 4:01p jrubio
* SW7346-1004: fix Brutus numbering issue
* 
* 12   10/10/12 10:08p jrubio
* SW7346-1004: add brutus file number generator
* 
* 11   10/10/12 5:31p tokushig
* SW7231-749: adjust debug output for release
* 
* 10   10/8/12 4:18p jrubio
* SW7346-1004: add sync functions that does not delete current
* playbackList
* 
* 9   10/3/12 5:15p jrubio
* SW7346-1004: add getVideoName function
* 
* 8   9/25/12 11:30a jrubio
* SW7346-1004: add record into video object
* 
* 7   9/18/12 5:45p jrubio
* SW7346-1004: temp add _recordPidMgr pointer
* 
* 6   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 5   5/25/12 4:13p jrubio
* SW7346-644: fix non index mpg files
* 
* 4   5/25/12 4:13p jrubio
* SW7346-644: fix non index mpg files
* 
* 3   5/25/12 3:57p jrubio
* SW7346-644: fix logic for MediaList iteration
* 
* 2   5/25/12 11:12a jrubio
* SW7346-644: fix info creation
* 
* 1   5/21/12 5:47p jrubio
* SW7346-644: break off video class into its own file
* 
* 12   5/15/12 4:51p jrubio
* SW7346-644: add write info,parse media files
*  
*********************************************************************************/
#include "band.h"
#include "brutus_board.h"
#include "playback.h"
#include "videolist.h"
#include "avtypes.h"
#include "xmltags.h"
#include "mxmlparser.h"
#include "bmedia_probe.h"
#include "bmpeg2ts_probe.h"
#include "bfile_stdio.h"
#include <unistd.h> //file i/o
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#if B_HAS_ASF
#include "basf_probe.h"
#endif
#if B_HAS_AVI
#include "bavi_probe.h"
#endif

#define MAXBUF 1024*20

BDBG_MODULE(brutus_videolist);

CVideo::CVideo():
    _encrypted(false),
    _active(false),
    _timeShifting(false),
    _position(0),
    _isRecordActive(false),
    _isPlaybackActive(false)
{
}

CVideo::~CVideo()
{
    /* Deleting the Video object */
    BDBG_MSG(("Video Name %s ", _mediaName.s()));
}

void CVideo::print()
{
    
    BDBG_WRN(("Video Name %s ", _mediaName.s()));
    BDBG_WRN(("Index Name %s ",_indexName.s() ));
    BDBG_WRN(("Path name %s", _path.s()));
}

void CVideo::closeVideo()
{
    _active=false;
    _timeShifting = false;
    _encrypted = false;
    _position =0;
}


bool CVideo::hasIndex()
{
    if(_mediaName.strncmp(_indexName) == 0)
        return false;
    else if(_indexName == NULL)
        return false;
    else
        return true;

}
eRet CVideo::write()
{

    MXmlElement              * xmlElemInfo        = new MXmlElement(NULL,XML_TAG_INFOFILE);
    FILE                     * file               = NULL;

    file = fopen(_path+_infoName,"w");
    if (!file ) 
    {
        BDBG_ERR(("Cannot open file for writing %s," ,_infoName.s()));
        return eRet_ExternalError;
    }
    
    
    xmlElemInfo->addAttr(XML_ATT_FILENAME,_mediaName);
    xmlElemInfo->addAttr(XML_ATT_INDEXNAME, _indexName);
    xmlElemInfo->addAttr(XML_ATT_PATH, _path);
    _pidMgr.writePidXML(xmlElemInfo);

    xmlElemInfo->print(file);

    if (file) 
    {
        fclose(file);
    }

    BDBG_MSG(("----------------------------------------------------------"));
    BDBG_MSG(("Saved channel list to file: %s", _infoName.s()));

    return eRet_Ok;

    

}

CVideo * CPlaybackList::checkName(MString name)
{
    MListItr <CVideo> itrVideo(&_videos);
    CVideo * video;
    MString compareName = name;

    
    for (video = itrVideo.first(); video;video= itrVideo.next())
    {
        if (video->_indexName== name) 
            goto duplicate;
        if (video->_mediaName== name) 
            goto duplicate;
        if (video->_infoName== name) 
            goto duplicate;
    }

    /* Not found return NULL */
    return NULL;

duplicate:
return video;
    
}


eRet CVideo::readInfo(MString infoName)
{
    MXmlElement * xmlElemInfo = NULL;
    MXmlParser    xmlParser;
    MXmlElement * xmlElemTop         = NULL;
    MString       path;
    MString       indexName;
    int           fd                 = 0;
    char        * buf                = NULL;
    int           nBufSize           = 0;
    eRet          ret                = eRet_Ok;

    _infoName = infoName;

    /* Please make sure the video PATH is set before calling this functions */
    if(!_path)
    {
        BDBG_ERR(("Path must be set before calling video readInfo()!"));
        return eRet_ExternalError;
    }

    fd = open(_path+_infoName, O_RDONLY, 0);
    if (!fd)
    {
        BDBG_ERR(("Failed to open file '%s'!", _infoName.s()));
        return eRet_ExternalError;
    }
    
    buf = (char *)malloc(sizeof(char) * MAXBUF);
    CHECK_PTR_GOTO("Error malloc parse buffer!", buf, ret, eRet_OutOfMemory, error);

    if (fd >= 0)
    {
        memset(buf, 0, sizeof(char) * MAXBUF);
        nBufSize = read(fd, buf, MAXBUF);
        close(fd);
        fd = 0;
    }

    if (nBufSize <= 0)
    {
        BDBG_WRN(("Cannot open XML channel list file: %s", infoName.s()));
        ret = eRet_ExternalError;
        goto error;
    }

    xmlElemTop = xmlParser.parse(buf);
    CHECK_PTR_GOTO("Syntax error parsing channel list XML", xmlElemTop, ret, eRet_ExternalError, error);

    {
        BDBG_Level dbgLevel = (BDBG_Level)0;
        BDBG_GetModuleLevel("brutus_videolist", &dbgLevel);

        if (BDBG_eMsg == dbgLevel)
        {
            BDBG_MSG(("----------------------------------------------------------"));
            BDBG_MSG(("xmlElemTop:"));
            xmlElemTop->print();
        }
    }

    xmlElemInfo = xmlElemTop->findChild(XML_TAG_INFOFILE);
    for (xmlElemInfo = xmlElemTop->firstChild(); 
             xmlElemInfo; 
             xmlElemInfo = xmlElemTop->nextChild())
    {
        if (xmlElemInfo)
        {
            _mediaName = xmlElemInfo->attrValue(XML_ATT_FILENAME);
            if (!_mediaName) 
            {
                BDBG_MSG((" No File Name Found, ingnore entry"));
                continue;
            }

            indexName = xmlElemInfo->attrValue(XML_ATT_INDEXNAME);
            if (indexName) 
            {
                BDBG_MSG(("Index File Name Found"));
                _indexName = indexName;
            }
            /* If index name does not exist set to MediaName */
            if(_indexName ==NULL)
                _indexName=_mediaName;

            _description = xmlElemInfo->attrValue(XML_ATT_DESCRIPTION);
            if (!_description) 
            {
                BDBG_MSG(("Default Descrition "));
                _description = _mediaName;
            }

            path = xmlElemInfo->attrValue(XML_ATT_PATH);
            if (path) 
            {
                /* Overwrite Path with oath from .nfo file */
                BDBG_MSG(("New Path %s", path.s()));
                _path = path;
            }
            _pidMgr.readPidXML(xmlElemInfo);

        }
        
    }


error: 
    if (fd)
    {
        close(fd);
        fd = 0;
    }

    if (xmlElemTop) 
    {
        delete xmlElemTop;
        xmlElemTop = NULL;
    }

    if (buf)
    {
        free(buf);
        buf = NULL;
    }return ret;

}

eRet CPlaybackList::readInfo(MString infoName)
{
    MListItr <CVideo> itrVideo(&_videos);
    CVideo * video;
    eRet     ret = eRet_Ok;

    /* if video does not exist create it and add it to the list */
    for (video=itrVideo.first();video;video=itrVideo.next()) 
    {
        if (video->_infoName == infoName) 
        {
            /* check to see if PIDS are created if not Create them */
            if(video->_pidMgr.isEmpty())
            {
                BDBG_MSG((" Pids are NOT  created"));
                video->readInfo(infoName);
            }

            /* Found Video already on the list*/
            BDBG_MSG((" Found Video in list"));
            return ret;
        }
    }

    /* did not find video, create new video and add to the list */
    video = new CVideo();
    ret = video->readInfo(infoName);
    if (ret ==eRet_Ok) 
        addVideo(video);
    else
        delete video;

    return ret;
}


int CPlaybackList::saveInfo(MString &mediafile, CVideo * video,const bmedia_probe_stream *stream)
{
    int retVal = 0;
    char *dot = strrchr(mediafile, '.');
    MString infofile;

    BDBG_MSG(("Writing Info File"));

    if(dot) {
        infofile.strncpy(mediafile, dot - mediafile);
    } else {
        infofile = mediafile;
    }

    infofile += ".nfo";

    BDBG_MSG(("writeInfo(%s, %s) video:0x%02x audio:0x%02x", mediafile.s(), infofile.s(), video->_pidMgr.getPid(0,ePidType_Video),video->_pidMgr.getPid(0,ePidType_Audio) ));

    const char *strStart = strrchr(mediafile, '/');
    if (strStart) {
        strStart++;
    }
    if(!video->_mediaName)
        video->_mediaName = mediafile;
    

    /* TODO: title, description, artist */
    if (0) 
    {
        BDBG_MSG(("Unable to determine Brutus .info format for %s", (const char *)mediafile));
        retVal = -1;
    }else 
    {
        // generate .nfo file.
        BDBG_MSG(("write info file: %s", infofile.s()));
        video->_infoName = infofile;
        retVal = video->write();
    }

    /* Now add to Videos  ??/*/
    return retVal;

}


eRet CPlaybackList::createInfo(MString &mediafile, bmedia_probe_t probe,CVideo * video)
{
    eRet retVal = eRet_Ok;
    BDBG_ASSERT(probe);
    
    bmedia_probe_config probe_config;
    const bmedia_probe_stream *stream;
    const bmedia_probe_track *track;
    bfile_io_read_t  fd=NULL;


    FILE *fin = fopen64(video->_path+mediafile,"rb");
    if (!fin) {
        BDBG_ERR(("can't open media file '%s'", mediafile.s()));
        return eRet_ExternalError;
    }

    /* FUNCTION BELOW is from SETTOP API */
    fd =bfile_stdio_read_attach(fin);

    
    bmedia_probe_default_cfg(&probe_config);
    probe_config.file_name = mediafile;
    probe_config.type = bstream_mpeg_type_unknown;
    stream = bmedia_probe_parse(probe, fd, &probe_config);

    /* now stream is either NULL, or stream descriptor with linked list of audio/video tracks */
    bfile_stdio_read_detach(fd);

    fclose(fin);
    if(!stream) {
        BDBG_WRN(("can't parse stream '%s'", mediafile.s()));
        return eRet_ExternalError;
    }

    if( video == NULL)
        video = new CVideo();
    CPid   * pid;
    video->_mediaName = mediafile;

    if (stream->type == bstream_mpeg_type_ts) {
        if ((((bmpeg2ts_probe_stream*)stream)->pkt_len) == 192) {
            video->_timestamp_enabled = true;
        }
    }
    /* populate other structure with first audio and video tracks */
    unsigned prev_program=0;
    unsigned audio_program=0;
    unsigned video_program=0;
    for(track=BLST_SQ_FIRST(&stream->tracks);track;track=BLST_SQ_NEXT(track, link)) {
        unsigned i;
        if(track!=BLST_SQ_FIRST(&stream->tracks)&& prev_program!=track->program) {
            //retVal = writeInfo(mediafile, prev_program, infos, video, stream);
            audio_program = 0;
            video_program = 0;
            video = new CVideo;
            BDBG_MSG(("New Video Track"));
        }
        prev_program = track->program;
#if B_HAS_ASF
        if(stream->type == bstream_mpeg_type_asf) {
            video->_isForPurchase = video->_isForPurchase || ((basf_probe_track *)track)->encryptedContentFlag;
        }
#endif
#if B_HAS_AVI
        if(stream->type == bstream_mpeg_type_avi) {
            video->_isForPurchase = video->_isForPurchase || ((bavi_probe_track *)track)->encryptedContentFlag;
        }
#endif

        switch(track->type) {
            case bmedia_track_type_audio:
                if(track->info.audio.codec != baudio_format_unknown && audio_program < 1) 
                {
                    /* TODO: multiple audio tracks*/
                    /* Set the pid to audio here */
                    pid = new CPid(track->number,ePidType_Audio);
                    pid->setAudioCodec(bmediaToAudioCodec(track->info.audio.codec));
                    pid->setPlayback(true);
                    video->_pidMgr.addPid(pid);
                    audio_program++;
                    BDBG_MSG(("Audio Track added pid 0x%x",pid->getPid()));
                }
                break;
            case bmedia_track_type_video:
                if (video_program >= 1) 
                {
                    BDBG_WRN((" Program contains more then one video Pid"));
                    break;
                }
                if(track->info.video.codec != bvideo_codec_unknown) {
                    pid = new CPid(track->number,ePidType_Video);
                    pid->setVideoCodec(bmediaToVideoCodec(track->info.video.codec));
                    pid->setPlayback(true);
                    video->_pidMgr.addPid(pid);
                    video_program++;
                    pid->setPcrType(true); /* TODO*/
                    BDBG_MSG(("Video Track added pid 0x%x",pid->getPid()));
                }
                break;
            case bmedia_track_type_pcr:
                pid = new CPid(track->number,ePidType_Pcr);
                pid->setPlayback(true);
                video->_pidMgr.addPid(pid);
                BDBG_MSG(("Audio Track added pid 0x%x",pid->getPid()));
                break;
            default:
                break;
        }

    }

    
    saveInfo(mediafile, video,stream);
    bmedia_probe_stream_free(probe, stream);
    return retVal;

}

/* This function just creates the nfo file based on a video that is complete */
void CPlaybackList::createInfo(CVideo * video)
{
   if (video->_mediaName.isEmpty())
      goto error; 

   saveInfo(video->_mediaName, video,NULL);
   return;

error:
BDBG_ERR((" Incomplete Video object , cannot create a nfo file!"));

}

void CPlaybackList::addVideo(CVideo * video)
{
    BDBG_MSG(("Adding Video"));
    video->print();

    _videos.add(video);

}

void CPlaybackList::removeVideo(CVideo * video)
{
    BDBG_MSG(("Removing Video"));
    video->print();
    _videos.remove(video);
}

void CPlaybackList::removeVideo(int index)
{
    MListItr <CVideo> itrVideo(&_videos);
    CVideo * video;

    BDBG_MSG(("Removing Video"));
    video = itrVideo.at(index);
    if (video != NULL) 
    {
        BDBG_MSG(("Removing Video"));
        _videos.remove(video);
        return;
    }
    
    BDBG_MSG(("Video at index %d not found",index));


}


void CPlaybackList::print()
{
    CVideo * video;
    MListItr <CVideo> itrVideo(&_videos);
    int index;

    for (video = itrVideo.first(); video;video= itrVideo.next()) 
    {
       
        video->print();
        
    }

}

void CPlaybackList::sync(void)
{
  notifyObservers(eNotify_PlaybackListChanged, this);
}

void CPlaybackList::generateVideoName(MString path)
{

   MString brutus; 
   MString name;
   MStringList brutusList;
   int number=1;
   

   const char *dirstr = path.s();
   DIR *dir = opendir((char *) dirstr);
   if (!dir) {
      BDBG_WRN(("Directory '%s' not found", dirstr));
      return ;
    }

    struct dirent *d;
            
    while ((d = readdir(dir))) {
        if(strcmp(d->d_name, ".")==0 || strcmp(d->d_name, "..")==0) {
            continue; /* skip "." and ".." */
        }
        char *dot = rindex(d->d_name, '.');
        name =  d->d_name;
        BDBG_MSG(("name file %s",name.s()));
        
        /* Brutus must be at the beginning of the FileName*/
        if(name.strncmp("brutus",6) != 0 )
        {
           continue;
        }
        brutus = name;
        BDBG_MSG(("name file after compare %s",brutus.s()));
        brutus.truncate(brutus.find(".",0, false));
        BDBG_MSG(("filename after cut of .mpg/nav/etc %s",brutus.s()));

        brutus = brutus.mid(6);
        BDBG_MSG(("Brutus now should be a number %s, number in int %d",brutus.s(),brutus.toInt()));

        if(number <= brutus.toInt())  
        {
           number = brutus.toInt()+1;
        }
    }

    BDBG_MSG(("new Number is %d", number));
    _nextVideoNumber=number;
    closedir(dir);
}

MString CPlaybackList::getNextBrutusName()
{
   MString brutusName;
   brutusName = "brutus";
   brutusName = brutusName+MString(_nextVideoNumber)+MString(".mpg");
   _nextVideoNumber++;
   return brutusName;
}

void CPlaybackList::refreshFromDisk(MString path) 
{
    MListItr <CVideo> itrVideo(&_videos);
    CVideo * video;
    MString name;
    bool match = false;

    MStringList infoList;
    MStringList mediaList;
    MStringList indexList;

    MString infoName;
    MString mediaName;
    MString indexName;
    MString mediaExtension;

    /* Get rid of this and let probe do the extensions */
    MStringList mediaExtensions;
    mediaExtensions.add("mpg");
    mediaExtensions.add("ts");
    mediaExtensions.add("divx");
    mediaExtensions.add("avi");
    mediaExtensions.add("wmv");
    mediaExtensions.add("wma");


    if (!path && _videos.total() == 0) {
        BDBG_MSG((" First Entry , List is empty"));
    } else
    {
        BDBG_MSG((" Empty List"));
        _videos.clear();
    }

    bmedia_probe_t probe;
    probe = bmedia_probe_create();
    BDBG_ASSERT(probe);

    generateVideoName(path);

    const char *dirstr = path.s();
    DIR *dir = opendir((char *) dirstr);
    if (!dir) {
        BDBG_WRN(("Directory '%s' not found", dirstr));
        return;
    }
        
    struct dirent *d;
            
    while ((d = readdir(dir))) {
        if(strcmp(d->d_name, ".")==0 || strcmp(d->d_name, "..")==0) {
            continue; /* skip "." and ".." */
        }
        char *dot = rindex(d->d_name, '.');
        name =  d->d_name;
        BDBG_MSG(("name file %s",name.s()));

        if(dot && (strcmp(dot,".nfo")==0))  
        {
            BDBG_MSG((" added an info file %s",name.s()));
            infoList.add(name.s());
            
        } else if(dot && (strcmp(dot,".nav")==0))  
        {
            BDBG_MSG((" added an index file %s",name.s()));
            indexList.add(name.s());
        } else 
        {
            /* Check for different types of Media files */
            BDBG_MSG((" added an media file %s",name.s()));
            mediaList.add(name.s());
        }       
    }
    
    /* Filter out ALL  nonrecognized media File  extensions */
    mediaName = mediaList.first();
    while (mediaName) 
    {
        match=false;
        for (mediaExtension = mediaExtensions.first();mediaExtension;mediaExtension = mediaExtensions.next()) 
        {
            MString temp = mediaName.mid(mediaName.find(".")+1,mediaExtension.length());
            BDBG_MSG(("Comparing %s, %s, media . @%d",mediaExtension.s(),mediaName.s(),mediaName.find(".")));
            BDBG_MSG(("mediaName extension %s ", temp.s()));
            if(temp == mediaExtension)
            {
                BDBG_MSG(("Match %s, %s, media . @%d",mediaExtension.s(),mediaName.s(),mediaName.find(".")));
                match=true;
                break;
            }
            /* TODO create a function that checks to see what brutus# there is so we can start recording after that number example: brutus1.mpg,brutus2.mpg next
               update the counter to brutus3.mpg */
            
        }

        if(match == false)
        {            
            BDBG_MSG(("Remove %s",mediaName.s()));
            mediaList.remove(mediaName);
            mediaName = mediaList.current();
        } else
        {
            mediaName = mediaList.next();
        }

    }

    /* All files are now in 3 lists,Valid media Files, info Files, index Files. Link index/media the files */
    for (indexName = indexList.first();indexName;indexName = indexList.next()) 
    {
        match=false;
        for (mediaName = mediaList.first();mediaName;mediaName = mediaList.next()) 
        {
            BDBG_MSG(("Comparing %s, %s, index . @%d",indexName.s(),mediaName.s(),indexName.find(".")));
            /* do they match*/
            if (mediaName.strncmp(indexName.s(), indexName.find("."))==0)
            {
                BDBG_MSG(("We have a match %s, %s ",indexName.s(),mediaName.s()));
                video = new CVideo();
                video->_mediaName = mediaName;
                video->_indexName = indexName;
                video->_path = path;
                _videos.add(video);
                match=true;
                break;
            }
        }

        if (match) 
        {
            BDBG_MSG(("Remove %s Index %s ",mediaName.s(), indexName.s()));
            mediaList.remove(mediaName);
        }
     }

    /* Ignore the rest fo the index list, no Media Files to go with them */
    indexList.clear();

    /* Now Lets Match up info files with added Videos. */
    for (infoName = infoList.first();infoName;infoName = infoList.next()) 
    {
        video = NULL;
        for (video = itrVideo.first();video;video = itrVideo.next()) 
        {
            BDBG_MSG(("Comparing %s, %s",infoName.s(),video->_mediaName.s()));
            /* do they match*/
            if (video->_mediaName.strncmp(infoName.s(), infoName.find("."))==0)
            {
                BDBG_MSG(("We have a match %s, %s ",infoName.s(),video->_mediaName.s()));
                video->_infoName = infoName;
                readInfo(infoName);
                break;
            }
        }
        
     }


    /* This goes through the Video list and finds index+mpg combos without an info file*/
    /* Set Path while iterating through the list */
    video = NULL;
    for (video = itrVideo.first();video;video = itrVideo.next()) 
    {
        BDBG_MSG(("Media File %s ",video->_mediaName.s()));
        video->_path = path;
        if (video->_infoName==NULL)
        {
            BDBG_MSG(("Creating info file for %s ",video->_mediaName.s()));
            createInfo(video->_mediaName,probe,video);
            continue;
        }
    }

    /* TODO Rouge non Index Videos will be probed below and possible added index */
    /* This goes through the left over media Files without an index and creates an info file and 
       sets the indexName to MediaName */
    for (mediaName = mediaList.first();mediaName;mediaName = mediaList.next()) 
    {
        video = NULL;
        video = new CVideo();
        video->_mediaName = mediaName;
        video->_indexName = mediaName;
        video->_path = path;

        BDBG_MSG(("MediaName without Index %s ",mediaName.s()));
        if(createInfo(mediaName,probe,video)==eRet_Ok)
        {
            _videos.add(video);
        } else
            delete video;
    }
    
    /* now Line up */
    closedir(dir);


    bmedia_probe_destroy(probe);

    notifyObservers(eNotify_PlaybackListChanged, this);
    //print();
}
