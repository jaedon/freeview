/* ETC( API ver, etc.. ) 관련 */
'use strict';

// 내부 모듈 import
var url = require('url');
var querystring = require('querystring');
var os = require('os');


// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
var etc_api = {
    category: 'etc',
    api_set: [
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/info',
            url: undefined,
            parameters: []},
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/time',
            url: undefined,
            parameters: []}
    ]
};
if (!RESTAPI.v1.etc_api) {
    RESTAPI.v1.etc_api = etc_api.api_set;
}


/**
 *  Method : GET
 *  API : /stb-api/v1/info
 *  Description : STB Information 요청
 *  */
exports.getInfo = function (request, response) {
    response.send(RESTAPI);
};

/**
 *  Method : GET
 *  API : /stb-api/v1/time
 *  Description : System의 Current Time 정보를 요청
 *  */
exports.getTime = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_COMMON_TIME_Get.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_COMMON_TIME_Get() Return OK...');

        var resObj = JSON.parse(res);
        // OS, Process의 up time을 덧붙여 줌.
        if (resObj.systemTime) {
            resObj.systemTime.osUpTime = os.uptime();
            resObj.systemTime.serverUpTime = process.uptime();
        }

        RESTAPI.sendResult(response, JSON.stringify(resObj));

    });
};
