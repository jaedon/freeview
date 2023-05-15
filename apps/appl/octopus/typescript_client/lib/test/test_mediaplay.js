/**
* Created by hsseo on 14. 4. 9.
*/
/// <reference path="../def/node.d.ts" />
/// <reference path="../def/mocha.d.ts" />
/// <reference path="../def/async.d.ts" />
/// <reference path="../def/should.d.ts" />
var octopus = require('../octopus');
var should = require('should');

describe('CMediaPlay', function () {
    var mediaplay;
    var viewCount = 2;
    var mainViewId = 0;
    before(function (done) {
        mediaplay = new octopus.CMediaPlay();
        setTimeout(function () {
            console.log('fixture set up : mediaPlay');
            done();
        }, 500);
    });

    it('GetViewNumber', function (done) {
        mediaplay.GetViewNumber(function (aViewNum) {
            console.log("viewCount : " + aViewNum);
            should(aViewNum > 0).ok;
            viewCount = aViewNum;
            setTimeout(function () {
                done();
            }, 500);
        });
    });

    it('GetMainViewId', function (done) {
        mediaplay.GetMainViewId(function (aViewId) {
            console.log("mainViewId : " + aViewId);
            should(aViewId < viewCount).ok;
            mainViewId = aViewId;
            setTimeout(function () {
                done();
            }, 500);
        });
    });

    it('GetPlayType', function (done) {
        mediaplay.GetPlayType(mainViewId, function (aMediaType) {
            console.log("mainView MediaType : " + aMediaType);
            should(aMediaType < 5).ok;
            setTimeout(function () {
                done();
            }, 500);
        });
    });

    it('GetNeedRadioBg', function (done) {
        mediaplay.GetNeedRadioBg(mainViewId, function (aIsNeed) {
            console.log("GetNeedRadioBg ret : " + aIsNeed);
            should(aIsNeed < 2).ok;
            setTimeout(function () {
                done();
            }, 500);
        });
    });

    it('GetSessionId', function (done) {
        mediaplay.GetSessionId(mainViewId, function (aSessionId) {
            console.log("GetSessionId ret : " + aSessionId);
            setTimeout(function () {
                done();
            }, 500);
        });
    });

    it('GetRequestId', function (done) {
        mediaplay.GetRequestId(mainViewId, 1, function (aRequestId) {
            console.log("GetRequestId (live:1) ret : " + aRequestId);
            setTimeout(function () {
                done();
            }, 500);
        });
    });

    it('GetStreamAspectRatio', function (done) {
        mediaplay.GetStreamAspectRatio(mainViewId, function (aAspectRatio) {
            console.log("GetStreamAspectRatio ret : " + aAspectRatio);
            setTimeout(function () {
                done();
            }, 500);
        });
    });
    /*
    it('GetMhegDisplayPoint', (done) => {
    mediaplay.GetMhegDisplayPoint(mainViewId, 0, 0, 1024, 768, (scaledX: number, scaledY: number) => {
    console.log("GetMhegDisplayPoint ret : x: " + scaledX + " y: " + scaledY);
    //should().ok;
    setTimeout(() => {
    done();
    }, 500);
    });
    })
    
    
    it('GetComponentNum', (done) => {
    mediaplay.GetComponentNum(0, 3, (aCompCount: number) => {
    console.log("GetComponentNum (l3:subtitle)ret : " + aCompCount);
    //should().ok;
    setTimeout(function () {
    done();
    }, 500);
    });
    });
    
    it('GetSubtitleComponent', (done) => {
    mediaplay.GetSubtitleComponent(mainViewId, 0, (aComponet: octopus.TSubtitleTrack) => {
    console.log("GetSubtitleComponent ret : " + aComponet.pid); // iiyiiiis
    //should().ok;
    setTimeout(() => {
    done();
    }, 500);
    });
    });
    
    it('GetComponentIndex', (done) => {
    mediaplay.GetComponentIndex(mainViewId, 1, (aCompIndex: number) => {
    console.log("GetComponentIndex (1:video) ret : " + aCompIndex);
    //should().ok;
    setTimeout(() => {
    done();
    }, 500);
    });
    });
    
    it('GetPlaySpeed', (done) => {
    mediaplay.GetPlaySpeed(mainViewId, (aSpeed: number) => {
    console.log("GetPlaySpeed ret : " + aSpeed);
    //should().ok;
    setTimeout(() => {
    done();
    }, 500);
    });
    });
    
    it('GetPlayPosition', (done) => {
    mediaplay.GetPlayPosition(mainViewId, (aPosition: number) => {
    console.log("GetPlayPosition ret : " + aPosition);
    //should().ok;
    setTimeout(() => {
    done();
    }, 500);
    });
    });
    
    it('GetPlayError', (done) => {
    mediaplay.GetPlayError(mainViewId, (aError: number) => {
    console.log("GetPlayError ret : " + aError);
    //should().ok;
    setTimeout(() => {
    done();
    }, 500);
    });
    });
    
    it('GetBufferedTime', (done) => {
    mediaplay.GetBufferedTime(mainViewId, (aBufferred: number) => {
    console.log("GetBufferedTime ret : " + aBufferred);
    //should().ok;
    setTimeout(() => {
    done();
    }, 500);
    });
    });
    
    it('GetDurationTime', (done) => {
    mediaplay.GetDurationTime(mainViewId, (aDurationTime: number) => {
    console.log("GetDurationTime ret : " + aDurationTime);
    //should().ok;
    setTimeout(() => {
    done();
    }, 500);
    });
    });
    
    it('GetTsrStartTime', (done) => {
    mediaplay.GetTsrStartTime(mainViewId, (aTstStartTime: number) => {
    console.log("GetTsrStartTime ret : " + aTstStartTime);
    //should().ok;
    setTimeout(() => {
    done();
    }, 500);
    });
    });
    
    it('GetTrickRestrictMode', (done) => {
    mediaplay.GetTrickRestrictMode(mainViewId, (aTrickRestrictMode: number) => {
    console.log("GetTrickRestrictMode ret : " + aTrickRestrictMode);
    //should().ok;
    setTimeout(() => {
    done();
    }, 500);
    });
    });
    
    it('GetVideoSize', (done) => {
    mediaplay.GetVideoSize(mainViewId, (startX: number, startY: number, width: number, height: number) => {
    console.log("GetVideoSize ret : " + startX);
    //should().ok;
    setTimeout(() => {
    done();
    }, 500);
    });
    });
    
    it('GetTSREnable', (done) => {
    mediaplay.GetTSREnable((aIsTSR: number) => {
    console.log("GetTSREnable ret : " + aIsTSR);
    //should().ok;
    setTimeout(() => {
    done();
    }, 500);
    });
    });
    
    it('GetSubtitleEnable', (done) => {
    mediaplay.GetSubtitleEnable(mainViewId, (aIsSubtitle: number) => {
    console.log("GetSubtitleEnable ret : " + aIsSubtitle);
    //should().ok;
    setTimeout(() => {
    done();
    }, 500);
    });
    });
    */
});
//# sourceMappingURL=test_mediaplay.js.map
