/**
 * Created by ssyun on 2014-06-12.
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
var settings_api = {
    category: 'settings',
    api_set: [
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/settings',
            url: undefined,
            parameters: ['member']},
        {method: 'PUT',
            api: RESTAPI.serverInfo.apiv1path+'/settings',
            url: undefined,
            parameters: []},
        {method: 'GET',
            api: RESTAPI.serverInfo.apiv1path+'/access-settings',
            url: undefined,
            parameters: []},
        {method: 'PUT',
            api: RESTAPI.serverInfo.apiv1path+'/access-settings',
            url: undefined,
            parameters: []}
    ]
};
if (!RESTAPI.v1.settings_api) {
    RESTAPI.v1.settings_api = settings_api.api_set;
}

var GET_SETTINGS_ARGS = settings_api.api_set[0].parameters;

// settings의 property들 중, admin 권한이 필요한 것들을 local 변수로 저장해 두기 위한 변수. 초기값 주지 말것!!
var adminItems;

/**
 *  Method : GET
 *  API : /stb-api/v1/settings
 *  Description : settings 에 대한 get 요청
 *  */
exports.getSettings = function (request, response) {
    var parsedObject = url.parse(request.url);
    var query = querystring.parse(parsedObject.query);

    RESTAPI.log(query);

    // Query문의 유효성 검사.
    if (restUtil.isValidQuery(query, GET_SETTINGS_ARGS) === false) {
        RESTAPI.sendError(response, 'ERR_INVALID_QUERY');
        return;
    }

    var arg1 = request.sessionID;
    var arg2 = JSON.stringify(query);
    binder.REST_BIND_SETTINGS_Get.async(arg1, arg2, function (err, res) {
        if (err) {
            throw err;
        }
        getAdminItems(res);
        RESTAPI.log('REST_BIND_SETTINGS_Get() Return OK...');
        RESTAPI.sendResult(response, res);
    });
};


/**
 *  Method : PUT
 *  API : /stb-api/v1/settings
 *  Description :
 *  */
exports.putSettings = function (request, response) {
    var bRequirePIN = false;

    // adminItems를 모르는 상태이면 한번은 get 해와야 한다.
    if (adminItems === undefined) {
        var res = binder.REST_BIND_SETTINGS_Get(request.sessionID, "");
        getAdminItems(res);
    }

    // put 요청된 값들 중 admin 권한이 필요한 값이 있는지 검사.
    for (var item in request.body) {
        if (request.body.hasOwnProperty(item)) {
            RESTAPI.log( 'request.body : ' + item + '  index : ' + adminItems.indexOf(item) );
            if (adminItems.indexOf(item) >= 0) {
                RESTAPI.log(item + ' - This item requires PIN code!');
                bRequirePIN = true;
                break;
            }
        }
    }

    if (bRequirePIN) {
        // admin 권한이 필요한 property를 수정하는 경우는 pinCode를 먼저 검사 한다.
        if (request.body.pinCode) {
            binder.REST_BIND_PINCODE_VerifyPincode.async(request.body.pinCode, function (err, res) {
                if (err) {
                    throw err;
                }

                if (res) {
                    // Valid PIN code
                    var arg1 = request.sessionID;
                    var arg2 = JSON.stringify(request.body);
                    binder.REST_BIND_SETTINGS_Put.async(arg1, arg2, function (err, res) {
                        if (err) {
                            throw err;
                        }
                        RESTAPI.log('REST_BIND_SETTINGS_Put() Return OK...');
                        RESTAPI.sendResult(response, res);
                    });

                } else {
                    // Invalid PIN code
                    RESTAPI.elog('Invalid PIN Code!!!');
                    RESTAPI.sendError(response, 'ERR_INVALID_PIN');
                }
            });
        } else {
            RESTAPI.elog('Undefined PIN Code!!!');
            RESTAPI.sendError(response, 'ERR_REQUIRE_PIN');
        }
    } else {
        // admin 권한이 필요치 않은 property를 수정할 경우에는 pinCode 확인 없이 요청
        var arg1 = request.sessionID;
        var arg2 = JSON.stringify(request.body);
        binder.REST_BIND_SETTINGS_Put.async(arg1, arg2, function (err, res) {
            if (err) {
                throw err;
            }
            RESTAPI.log('REST_BIND_SETTINGS_Put() Return OK...');
            RESTAPI.sendResult(response, res);
        });
    }

};

/**
 * jsonString으로 부터 adminItems 값이 있는지 검사 후, 해당 값이 있으면 local의 adminItems로 설정해 둠
 * @param jsonString - JSON String 전문
 */
function getAdminItems (jsonString) {
    if (adminItems === undefined) {
        var object = JSON.parse(jsonString);
        if (object.adminItems) {
            adminItems = object.adminItems;
        } else {
            adminItems = {};
        }
    }
}


/**
 *  Method : GET
 *  API : /stb-api/v1/access-settings
 *  Description : REST Service의 on/off 현황
 *  */
exports.getAccessSettings = function (request, response) {
    var ret = {};
    var setting = {};

    setting.remote = RESTAPI.config.remoteService;
    ret.setting = setting;
    ret.error = RESTAPI.restError.ERR_OK.error;
    response.send(ret);
};

/**
 *  Method : PUT
 *  API : /stb-api/v1/access-settings
 *  Description : REST Service의 on/off 설정
 *  */
exports.putAccessSettings = function (request, response) {
    RESTAPI.log('PIN : ' + request.body.pinCode);
    if (request.body.pinCode) {
        RESTAPI.binder.REST_BIND_PINCODE_VerifyPincode.async(request.body.pinCode, function (err, res) {
            if (err) {
                throw err;
            }

            if (res) {
                // Valid PIN code
                var setValue = request.body.remote;
                if (setValue !== undefined) {
                    var ret = {};
                    RESTAPI.log(setValue);

                    if (setValue === 1 || setValue === 0) {
                        var setting = {};
                        // config file에 save
                        RESTAPI.setRemoteService(setValue);

                        setting.remote = setValue;
                        ret.setting = setting;
                        ret.error = RESTAPI.restError.ERR_OK.error;
                        response.send(ret);
                    } else {
                        RESTAPI.sendError(response, 'ERR_INVALID_VALUE');
                    }
                } else {
                    RESTAPI.sendError(response, 'ERR_REQUIRE_MANDATORYKEY');
                }

            } else {
                // Invalid PIN code
                RESTAPI.elog('Invalid PIN Code!!!');
                RESTAPI.sendError(response, 'ERR_INVALID_PIN');
            }
        });
    } else {
        RESTAPI.elog('Undefined PIN Code!!!');
        RESTAPI.sendError(response, 'ERR_REQUIRE_PIN');
    }
};