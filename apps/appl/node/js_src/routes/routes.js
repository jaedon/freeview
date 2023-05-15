/**
 * Created by ssyun on 2014-04-02.
 * - URL Route를 위한 main file
 * - Client의 GET/PUT/POST/DELETE Request에 대하여 실제 처리를 담당할 Function과 연결하도록 해줌
 */
'use strict';

// 내부 모듈 import
var os = require('os');
var fs = require('fs');


// 외부 모듈 import



var API_ROOT_PATH = '/stb-api';
var API_V1_PATH = API_ROOT_PATH+'/v1';
RESTAPI.serverInfo.apirootpath = API_ROOT_PATH;
RESTAPI.serverInfo.apiv1path = API_V1_PATH;
//RESTAPI.serverInfo.serverurl = 'http://'+RESTAPI.serverInfo.address+':'+RESTAPI.serverInfo.httpPort;
//RESTAPI.serverInfo.apirooturl = 'http://'+RESTAPI.serverInfo.address+':'+RESTAPI.serverInfo.httpPort+API_ROOT_PATH;
//RESTAPI.serverInfo.apiv1url = 'http://'+RESTAPI.serverInfo.address+':'+RESTAPI.serverInfo.httpPort+API_V1_PATH;

// 서브 모듈 import
var channel = require('./channel');
var transponder = require('./transponder');
var antenna = require('./antenna');
var player = require('./player');
var scanner = require('./scanner');
var epg = require('./epg');
var schedule = require('./schedule');
var record = require('./record');
var power = require('./power');
var tuner = require('./tuner');
var video = require('./video');
var audio = require('./audio');
var storage = require('./storage');
var settings = require('./settings');
var recorder = require('./recorder');

var etc = require('./etc');
var devUtil = require('./devutil');
var test;

// Local Variable
//var API_V1_PATH = '/stb-api/v1';


/**
 * HTTP 요청에 대한 routing을 설정하는 함수
 * @param app - express 객체
 */
exports.setRouter = function (app) {
    if (!app) {
        RESTAPI.elog('Error app is ' + app);
        return;
    }

    // domain Root, api Root에 대한 기본 처리
    app.get('/', domainRootProc);
    app.get(API_ROOT_PATH, domainRootProc);
    app.get(API_V1_PATH, apiRootProc);


    // Channel 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/channels', channel.getChannels);
    app.get(API_V1_PATH + '/channels/:channelUid', channel.getChannelByUid);


    // Transponder 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/transponders', transponder.getTransponders);


    // Antenna 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/antennas', antenna.getAntennas);


    // Player 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/players', player.getPlayers);
    app.get(API_V1_PATH + '/players/:playerId', player.getPlayerById);
    // PUT Method - Update
    app.put(API_V1_PATH + '/players/:playerId', player.putPlayer);


    // Channel-Scanner 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/channel-scanners', scanner.getScanners);
    // PUT Method - Update
    app.put(API_V1_PATH + '/channel-scanners', scanner.putScanner);


    // EPG 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/epg', epg.getEpg);


    // Schedule 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/schedules', schedule.getSchedules);
    app.get(API_V1_PATH + '/schedules/:scheduleUid', schedule.getScheduleByUid);
    // POST Method - Create
    app.post(API_V1_PATH + '/schedules', schedule.postSchedule);
    // PUT Method - Update
    app.put(API_V1_PATH + '/schedules/:scheduleUid', schedule.putSchedule);
    // DELETE Method - Delete
    app.del(API_V1_PATH + '/schedules/:scheduleUid', schedule.deleteSchedule);


    // Record 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/records', record.getRecords);
    app.get(API_V1_PATH + '/records/:recordUid', record.getRecordByUid);
    // POST Method - Create
    app.post(API_V1_PATH + '/records', record.postRecord);
    // DELETE Method - Delete
    app.del(API_V1_PATH + '/records/:recordUid', record.deleteRecord);


    // Power 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/power', power.getPower);


    // Tuner 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/tuners', tuner.getTuners);
    app.get(API_V1_PATH + '/tuners/:tunerUid', tuner.getTunerByUid);


    // Video 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/videos', video.getVideos);
    app.get(API_V1_PATH + '/videos/:videoUid', video.getVideoByUid);


    // Audio 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/audios', audio.getAudios);
    app.get(API_V1_PATH + '/audios/:audioUid', audio.getAudioByUid);


    // Storage 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/storages', storage.getStorages);
    app.get(API_V1_PATH + '/storages/:storageUid', storage.getStorageByUid);
    app.get(API_V1_PATH + '/storages/:storageUid/:patitionId', storage.getPartitionById);


    // Settings 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/settings', settings.getSettings);
    // PUT Method - Update
    app.put(API_V1_PATH + '/settings', settings.putSettings);

    app.get(API_V1_PATH + '/access-settings', settings.getAccessSettings);
    app.put(API_V1_PATH + '/access-settings', settings.putAccessSettings);


    // Recorder 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/recorders', recorder.getRecorders);
    app.get(API_V1_PATH + '/recorders/:recorderId', recorder.getRecorderById);
    // PUT Method - Update
    app.put(API_V1_PATH + '/recorders/:recorderId', recorder.putRecorder);


    // ETC 관련 Method
    // GET Method - Read
    app.get(API_V1_PATH + '/info', etc.getInfo);
    app.get(API_V1_PATH + '/time', etc.getTime);


    // 개발용 Hidden URL 관련 Method
    // GET Method - Read
    if (os.platform() === "linux") {
        app.get('/dev/util/ps', devUtil.ps);
    }
    app.get('/health', devUtil.healthCheck);
    app.get('/dev/util/memusage', devUtil.memUsage);
    app.get('/dev/util/processinfo', devUtil.processinfo);
    app.get('/dev/util/osinfo', devUtil.osinfo);
    app.get('/dev/util/geterror', devUtil.getError);

    // Test용 URL 처리 ***********************************************************************************
    if ('development' === app.get('env')) {
        test = require('./test');
        app.get('/test/socketio', test.testSocketio);   // socket.io test
        app.get('/test/session', test.testSession);     // session test
        app.get('/test/exitserver', test.exitServer);    // exit node server
        app.get('/test/pincode', test.pincodeVerify);   // pincode verify test
        app.get('/jstbpage/*', test.testJSTB);          // JSTB API Test
        app.get('/webapps/*', test.testWebApp);         // HMS Web App
    }
    // ***************************************************************************************************

    // TODO : 향후 수정 예정이거나, DEMO를 위한 임시 API
    app.get('/thumbnails/channel/:name', channel.thumbnailProc);
    app.get('/thumbnails/epg/:name', channel.thumbnailProc);

    // Temp: IBC Show를 위해 USB를 공유
    var usb_http = require('./usb_http');
    app.get('/usb_http/*', usb_http.usbHttp);   // socket.io test



    // 모든 OPTIONS Method에 대한 응답 처리
    app.options('*', optionsMethodProc);

    // 앞의 route 조건과 일치하지 않는 URL 요청시 - Invalid URL : Page Not Found
    app.all('*', unSupportedAPIProc);

};


/**
 * 각 API의 접속 URL을 global에 설정함
 * Network interface가 enable 된 후에 결정이 되는 사항임
 */
exports.setURL = function() {
    RESTAPI.serverInfo.serverurl = 'http://'+RESTAPI.serverInfo.address+':'+RESTAPI.serverInfo.httpPort;
    RESTAPI.serverInfo.apirooturl = 'http://'+RESTAPI.serverInfo.address+':'+RESTAPI.serverInfo.httpPort+API_ROOT_PATH;
    RESTAPI.serverInfo.apiv1url = 'http://'+RESTAPI.serverInfo.address+':'+RESTAPI.serverInfo.httpPort+API_V1_PATH;

    if (!RESTAPI.v1) {
        return;
    }

    // 각 category에 등록된 API들의 full URL을 만듦
    for (var category in RESTAPI.v1) {
        if (RESTAPI.v1.hasOwnProperty(category)) {
            var api_category = RESTAPI.v1[category];

            for (var i in api_category) {
                if (api_category.hasOwnProperty(i)) {
                    api_category[i].url = RESTAPI.serverInfo.serverurl + api_category[i].api;
                }
            }
        }
    }

};

// domain의 root 요청시 API_V1_PATH로 redirect
function domainRootProc(request, response) {
    response.redirect(API_V1_PATH);
}

// api Root 요청시에는 테스트용 HTML 응답
function apiRootProc(request, response) {
    var apiset = {};

    apiset.capability = Object.keys(RESTAPI.v1);
    apiset.restAPI = RESTAPI.v1;
    apiset.restError = RESTAPI.restError;
    apiset.error = RESTAPI.restError.ERR_OK.error;
    response.send(apiset);
}

// options method에 대한 응답
function optionsMethodProc(request, response) {
    //response.header('Access-Control-Allow-Methods', 'POST, GET, PUT, DELETE, OPTIONS');
    response.header('Access-Control-Allow-Methods', 'POST, GET, PUT, DELETE');
    response.header('Access-Control-Allow-Credentials', false);
    response.header('Access-Control-Max-Age', '86400');
    response.header('Access-Control-Allow-Headers', 'X-Requested-With, X-HTTP-Method-Override, Content-Type, Accept');
    RESTAPI.sendError(response, 'ERR_OK');
}

// 정의되지 않은 요청은 page not found
function unSupportedAPIProc(request, response) {
    RESTAPI.elog('Invalid Request!! ++++++++++++++++++++++++++');
    RESTAPI.elog('Request : ' + request.path);
    RESTAPI.elog('++++++++++++++++++++++++++++++++++++++++++++');
    RESTAPI.sendError(response, 'ERR_UNSUPPORTED_API');
}

