/* EPG 관련 */
'use strict';

// 내부 모듈 import
var url = require('url');
var querystring = require('querystring');


// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
var epg_api = {
    category: 'epg',
    api_set: [
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/epg',
            url: undefined,
            parameters: ['member', 'type', 'svcuid', 'starttime', 'duration']}
    ]
};
if (!RESTAPI.v1.epg_api) {
    RESTAPI.v1.epg_api = epg_api.api_set;
}

var GET_EPG_ARGS = epg_api.api_set[0].parameters;

/**
 *  Method : GET
 *  API : /stb-api/v1/epg
 *  Description : 모든 EPG 대한 요청
 *  */
exports.getEpg = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_EPG_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_EPG_Get.async(arg1, arg2, function (err, res) {
        if (err) {
            RESTAPI.log(err);
            throw err;
        }
        RESTAPI.log('REST_BIND_EPG_Get() Return OK...');

        // TODO: 향후 URL 자체는 아랫단에서 완성되어 올라올 것이기 때문에 아래 삭제 예정.
        // IBC 데모용 Thumbnail URL을 완성하기 위한 임시 코드..
        var resObj = JSON.parse(res);
        for (var program in resObj.EpgPrograms) {
            if (resObj.EpgPrograms.hasOwnProperty(program)) {
                if (resObj.EpgPrograms[program].thumbnail) {
                    resObj.EpgPrograms[program].thumbnail.url = RESTAPI.serverInfo.serverurl + resObj.EpgPrograms[program].thumbnail.url;
                }
            }
        }

        RESTAPI.sendResult(response, JSON.stringify(resObj));

        //RESTAPI.sendResult(response, res);
    });
};