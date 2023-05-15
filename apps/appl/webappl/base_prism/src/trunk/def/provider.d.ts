/**
 * Created by bglee on 2015-02-22.
 */
interface ifMetadataProvider {
    name;
    thumbnailURL;
    type;
    dummy;
    pluginObject;
    channel;
    setType(type);
    getName();
    getThumbnail();
    getType(): any;
    getStartTime();
    getEndTime();
    getDuration();
    getPercentage();
    getMediaType();
    hasSchedule();
    hasReminder();
    isRecorded();
    isPlayAvailable();
    isDummy();
    hasSeriesSchedule();
    getUserID();
    watchNow(aCallback?);
    getChannelNum();
}
interface ifProgrammeProvider extends ifMetadataProvider {
    _TYPE;
    _IMAGE;
    _showType;
    programmeID;
    genre;
    startTime;
    duration;
    endTime;
    parentalRating;
    episode;
    totalEpisodes;
    isHD;
    isAD;
    shortDescription;
    longDescription;
    seriesID;
    season;
    drm;
    isAC3;
    isAC3P;
    channel;
    channelType;
    ccid;
    groupCRIDs;
    actor;
    actress;
    director;
    guidance;
    linkage;
    load_thumbnail;
    defaultThumbnail;
    _userGenres;
    _complete;
    getPluginObject();
    getProgrammeName();
    getGuidance();
    getThumbnailUrlType();
    getThumbnailURL(noDefault?);
    getThumbnail();
    getType();
    getDefaultThumbnail();
    getShortDescription();
    getLongDescription();
    getStartTime();
    getEndTime();
    getDuration();
    getPercentage();
    getPercentageByTime(now);
    isLive();
    isPast();
    getChannelNum();
    getRecordingID();
    setStartTime(startTime);
    setDuration(duration);
    setEndTime(endTime);
    setChannelInformation(channel);
    setProgrammeName(name);
    isRecording();
    isPlayAvailable();
    hasSeriesSchedule();
    hasSchedule();
    hasReminder();
    getObjectType();
    getGenre();
    watchNow(callback?);
    getThumbnailLoadFlag();
    setThumbnailLoadFlag();
    getRecommendationCRID();
    getProgrammeID();
}