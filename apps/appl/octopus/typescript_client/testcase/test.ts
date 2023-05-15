
/// <reference path="./def/node.d.ts" />
/// <reference path="./def/mocha.d.ts" />
/// <reference path="./def/async.d.ts" />
/// <reference path="./def/should.d.ts" />
/// <reference path="./node_modules/octopus-client/octopus.d.ts" />


var octopus = require('octopus-client');


var mediaplay;
mediaplay = new octopus.CMediaPlay();

mediaplay.GetViewNumber((aViewNum:number) => {
    console.log("viewCount : " + aViewNum);
});

/*
mediaplay.Stop(0, () => {
    console.log("Stop!" );
});
*/
/*
/////////////////////////////////////////////////////////////////////////////////////////////////////
var liveinfo : octopus.TDATA.TStartInfoLive = {
    liveType : 2,
    masterSvcUid : 100,
    suppleSvcUid : 100,
    suppSvcType : 3,
    majorCHNum : 10
};

mediaplay.StartLive(0, liveinfo, (aSessionId:number) => {
    console.log("aSessionId : " + aSessionId);
});

/////////////////////////////////////////////////////////////////////////////////////////////////////
var mediainfo : octopus.TDATA.TStartInfoMediaPlay = {
    videoCodec : 0,
    audioCodec : 100,
    videoPid : 101,
    audioPid : 0,
    startTime : 0,
    url : "http://10.0.14.42/asgard/files/HTTP_LINUX/Master071209-2_1338973103.avi"
};

mediaplay.StartMedia(0, mediainfo, (aSessionId:number) => {
    console.log("aSessionId : " + aSessionId);
});

/////////////////////////////////////////////////////////////////////////////////////////////////////
var pvrinfo : octopus.TDATA.TStartInfoPvrPb = {
    contentId : 3,
    startTime : 0
};

mediaplay.StartPvrPb(0, pvrinfo, (aSessionId:number) => {
    console.log("aSessionId : " + aSessionId);
});
*/

/*
var filemgrStorage;
filemgrStorage = new octopus.CFilemgrStorage();

filemgrStorage.GetSize(0, (aTotalKb: number, aAvailableKb: number, aUsedKb: number, aReservedKb: number, aAvailablePvrKb: number) => {
    console.log("aTotalKb : " + aTotalKb);
    console.log("aAvailableKb : " + aAvailableKb);
    console.log("aUsedKb : " + aUsedKb);
    console.log("aReservedKb : " + aReservedKb);
    console.log("aAvailablePvrKb : " + aAvailablePvrKb);
});
*/




