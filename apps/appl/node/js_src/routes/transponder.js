/**
 * Created by ssyun on 14. 3. 31.
 */
'use strict';

// 내부 모듈 import
var url = require('url');
var querystring = require('querystring');


// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
var transponder_api = {
    category: 'transponder',
    api_set: [
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/transponders',
            url: undefined,
            parameters: ['member', 'startidx', 'cnt', 'mode']}
    ]
};
if (!RESTAPI.v1.transponder_api) {
    RESTAPI.v1.transponder_api = transponder_api.api_set;
}

var GET_TRANSPONDERS_ARGS = transponder_api.api_set[0].parameters;

/**
 *  Method : GET
 *  API : /stb-api/v1/transponders
 *  Description : TV broadcast channel이 실려 있는 transponder를 다루는 URL
 *  */
exports.getTransponders = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_TRANSPONDERS_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_CHAN_GetTransponders.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_CHAN_GetTransponders() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};
