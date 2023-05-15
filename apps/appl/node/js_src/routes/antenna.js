/* Antenna 관련 */
'use strict';

// 내부 모듈 import
var url = require('url');
var querystring = require('querystring');

// 외부 모듈 import


// 서브 모듈 import
var binder = RESTAPI.binder;
var restUtil = RESTAPI.restUtil;


// Local Variable
var antenna_api = {
    category: 'antenna',
    api_set: [
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/antennas',
            url: undefined,
            parameters: ['member', 'startidx', 'cnt']}
    ]
};
if (!RESTAPI.v1.antenna_api) {
    RESTAPI.v1.antenna_api = antenna_api.api_set;
}

var GET_ANTENNAS_ARGS = antenna_api.api_set[0].parameters;

/**
 *  Method : GET
 *  API : /stb-api/v1/antennas
 *  Description : 모든 Antenna 에 대한 요청
 *  */
exports.getAntennas = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_ANTENNAS_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_KEY');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_CHAN_GetAntennas.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        RESTAPI.log('REST_BIND_CHAN_GetAntennas() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : GET
 *  API : /stb-api/v1/antennas/:antennaId
 *  Description : Antenna Inform 요청
 *  */
exports.getAntennaById = function (request, response) {
    var antennaId = request.param('antennaId');

    response.send('Return ' + antennaId + ' antenna info');
};