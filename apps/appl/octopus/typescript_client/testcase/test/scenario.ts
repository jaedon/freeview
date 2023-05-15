/// <reference path="../node_modules/octopus-client/octopus.d.ts" />

/// <reference path="../def/node.d.ts" />
/// <reference path="../def/mocha.d.ts" />
/// <reference path="../def/async.d.ts" />
/// <reference path="../def/should.d.ts" />

var should = require('should');
var octopus = require('octopus-client');

describe('scenario', function() {
    var mediaplay;
    var metaservice;

    before((done) => {
        mediaplay = new octopus.CMediaPlay();
        metaservice = new octopus.CMetaService();

        setTimeout(() => {
            console.log('fixture set up');
            done();
        }, 500);
    });

    it('live_allchannelchange', (done) => {

        metaservice.GetServiceList(serviceList => {

               var svc = serviceList[0];
               var liveinfo : octopus.TDATA.TStartInfoLive = {
                    liveType : 1,
                    masterSvcUid : svc.uid,
                    suppleSvcUid : svc.uid,
                    suppSvcType : 3,
                    majorCHNum : 110
                };

                mediaplay.StartLive(0, liveinfo, (aSessionId:number) => {});
                mediaplay.StartEvent((msgInt: number, msgStr: string, p1: number, p2: number, p3: number) => {
                    if (msgStr == "eOxMP_EVENT_AV_STARTED")
                        done();
                });
        });
    });
});