/* Audio 관련 */
'use strict';

// 내부 모듈 import


// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
// TODO: 구현 및 Binding 완료 되면 아래 주석을 풀어서 global 객체에 삽입할 것
//var audio_api = {
//    category: 'audio',
//    api_set: [
//        {method: 'GET',
//            api: RESTAPI.serverInfo.apiv1path+'/audios',
//            url: undefined,
//            parameters: []},
//        {method: 'GET',
//            api: RESTAPI.serverInfo.apiv1path+'/audios/[audioUid]',
//            url: undefined,
//            parameters: []}
//    ]
//};
//if (!RESTAPI.v1.audio_api) {
//    RESTAPI.v1.audio_api = audio_api.api_set;
//}

/**
 *  Method : GET
 *  API : /stb-api/v1/audios
 *  Description : 모든 Audio 대한 요청
 *  */
exports.getAudios = function (request, response) {
    response.send('Return All Audio info');
};


/**
 *  Method : GET
 *  API : /stb-api/v1/audios/:audioUid
 *  Description : Audio Inform 요청
 *  */
exports.getAudioByUid = function (request, response) {
    var uid = request.param('audioUid');

    response.send('Return ' + uid + ' Audio info');
};