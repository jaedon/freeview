/* Video 관련 */
'use strict';

/* 내부 모듈 import */


/* 외부 모듈 import */


/* 서브 모듈 import */
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
// TODO: 구현 및 Binding 완료 되면 아래 주석을 풀어서 global 객체에 삽입할 것
//var video_api = {
//    category: 'video',
//    api_set: [
//        {method: 'GET',
//            api: RESTAPI.serverInfo.apiv1path+'/videos',
//            url: undefined,
//            parameters: []},
//        {method: 'GET',
//            api: RESTAPI.serverInfo.apiv1path+'/videos/[videoUid]',
//            url: undefined,
//            parameters: []}
//    ]
//};
//if (!RESTAPI.v1.video_api) {
//    RESTAPI.v1.video_api = video_api.api_set;
//}


/**
 *  Method : GET
 *  API : /stb-api/v1/videos
 *  Description : 모든 Video 대한 요청
 *  */
exports.getVideos = function (request, response) {
    response.send('Return All Video info');
};


/**
 *  Method : GET
 *  API : /stb-api/v1/videos/:videoUid
 *  Description : Video Inform 요청
 *  */
exports.getVideoByUid = function (request, response) {
    var uid = request.param('videoUid');

    response.send('Return ' + uid + ' Video info');
};