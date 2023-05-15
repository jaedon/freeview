/**
 * Created by ssyun on 14. 3. 12.
 */
'use strict';

// 내부 모듈 import


// 외부 모듈 import
var ffi = require('node-ffi');

// 서브 모듈 import


// Load할 Library의 이름 :  확장자( .so )를 제외한 name
var LIBRARY_NAME = 'libnodebinder';


// 사용할 Library와 사용할 API를 재정의 - 각 함수명.async 를 사용하면 async 함수 이용 가능
// /node/binding/local_include/rest_bind_api.h  에 선언된 API를 JS로 바인딩 함
var binder = new ffi.Library(LIBRARY_NAME, {
    // Channel 관련 Bind API
    'REST_BIND_CHAN_RegisterNotifier': ['int', ['pointer']],
    'REST_BIND_CHAN_UnregisterNotifier': ['int', ['pointer']],
    'REST_BIND_CHAN_GetChannels': ['string', ['string', 'string']],
    'REST_BIND_CHAN_GetChannelOne': ['string', ['string', 'int', 'string']],
    'REST_BIND_CHAN_GetTransponders': ['string', ['string', 'string']],
    'REST_BIND_CHAN_GetAntennas': ['string', ['string', 'string']],

    // Player 관련 Bind API
    'REST_BIND_PLAYER_RegisterNotifier': ['int', ['pointer']],
    'REST_BIND_PLAYER_UnregisterNotifier': ['int', ['pointer']],
    'REST_BIND_PLAYER_Get': ['string', ['string', 'string']],
    'REST_BIND_PLAYER_GetOne': ['string', ['string', 'int', 'string']],
    'REST_BIND_PLAYER_PutOne': ['string', ['string', 'int', 'string']],

    // channel scan 관련 Bind API
    'REST_BIND_SCAN_RegisterNotifier': ['int', ['pointer']],
    'REST_BIND_SCAN_UnregisterNotifier': ['int', ['pointer']],
    'REST_BIND_SCAN_Get': ['string', ['string', 'string']],
    'REST_BIND_SCAN_Put': ['string', ['string', 'string']],

    // EPG 관련 Bind API
    'REST_BIND_EPG_RegisterNotifier': ['int', ['pointer']],
    'REST_BIND_EPG_UnregisterNotifier': ['int', ['pointer']],
    'REST_BIND_EPG_Get': ['string', ['string', 'string']],

    // Schedule 관련 Bind API
    'REST_BIND_SCHEDULE_RegisterNotifier': ['int', ['pointer']],
    'REST_BIND_SCHEDULE_UnregisterNotifier': ['int', ['pointer']],
    'REST_BIND_SCHEDULE_Post': ['string', ['string', 'string']],
    'REST_BIND_SCHEDULE_PutOne': ['string', ['string', 'int', 'string']],
    'REST_BIND_SCHEDULE_Get': ['string', ['string', 'string']],
    'REST_BIND_SCHEDULE_GetOne': ['string', ['string', 'int', 'string']],
    'REST_BIND_SCHEDULE_DeleteOne': ['string', ['string', 'int']],

    // Record 관련 Bind API
    'REST_BIND_RECORD_RegisterNotifier': ['int', ['pointer']],
    'REST_BIND_RECORD_UnregisterNotifier': ['int', ['pointer']],
    'REST_BIND_RECORD_Post': ['string', ['string', 'string']],
    'REST_BIND_RECORD_Get': ['string', ['string', 'string']],
    'REST_BIND_RECORD_GetOne': ['string', ['string', 'int', 'string']],
    'REST_BIND_RECORD_DeleteOne': ['string', ['string', 'int']],

    // Settings 관련 Bind API
    'REST_BIND_SETTINGS_RegisterNotifier': ['int', ['pointer']],
    'REST_BIND_SETTINGS_UnregisterNotifier': ['int', ['pointer']],
    'REST_BIND_SETTINGS_Get': ['string', ['string', 'string']],
    'REST_BIND_SETTINGS_Put': ['string', ['string', 'string']],

    // Pincode 관련 Bind API
    'REST_BIND_PINCODE_VerifyPincode': ['int', ['string']],

    // Tuner 관련 Bind API
    'REST_BIND_TUNER_RegisterNotifier': ['int', ['pointer']],
    'REST_BIND_TUNER_UnregisterNotifier': ['int', ['pointer']],
    'REST_BIND_TUNER_Get': ['string', ['string', 'string']],
    'REST_BIND_TUNER_GetOne': ['string', ['string', 'int', 'string']],

    // Recorder 관련 Bind AIP
    'REST_BIND_RECORDER_RegisterNotifier': ['int', ['pointer']],
    'REST_BIND_RECORDER_UnregisterNotifier': ['int', ['pointer']],
    'REST_BIND_RECORDER_Get': ['string', ['string', 'string']],
    'REST_BIND_RECORDER_GetOne': ['string', ['string', 'int', 'string']],
    'REST_BIND_RECORDER_PutOne': ['string', ['string', 'int', 'string']],


    // 기타 Common API
    'REST_BIND_COMMON_TIME_Get': ['string', ['string', 'string']],

    // Init Binder
    'REST_BIND_Init': ['int', []]
});

// Event Notifier Callback 객체 생성
var cbEventNotifier = ffi.Callback('void', ['string', 'string'], eventNotifier);

var bRegistNotifiers = false;


/**
 * Event Notifier 등록
 */
exports.registNotifiers = function () {
    var ret;
    RESTAPI.log('=================== Regist Event Notifier =======================');

    ret = binder.REST_BIND_CHAN_RegisterNotifier(cbEventNotifier);
    RESTAPI.log('Regist channel notifier.......(%d)\n', ret);

    ret = binder.REST_BIND_PLAYER_RegisterNotifier(cbEventNotifier);
    RESTAPI.log('Regist player notifier.......(%d)\n', ret);

    ret = binder.REST_BIND_SCAN_RegisterNotifier(cbEventNotifier);
    RESTAPI.log('Regist scanner notifier.......(%d)\n', ret);

    ret = binder.REST_BIND_EPG_RegisterNotifier(cbEventNotifier);
    RESTAPI.log('Regist epg notifier.......(%d)\n', ret);

    ret = binder.REST_BIND_SCHEDULE_RegisterNotifier(cbEventNotifier);
    RESTAPI.log('Regist schedule notifier.......(%d)\n', ret);

    ret = binder.REST_BIND_RECORD_RegisterNotifier(cbEventNotifier);
    RESTAPI.log('Regist record notifier.......(%d)\n', ret);

    ret = binder.REST_BIND_SETTINGS_RegisterNotifier(cbEventNotifier);
    RESTAPI.log('Regist settings notifier.......(%d)\n', ret);

    ret = binder.REST_BIND_TUNER_RegisterNotifier(cbEventNotifier);
    RESTAPI.log('Regist tuner notifier.......(%d)\n', ret);

    ret = binder.REST_BIND_RECORDER_RegisterNotifier(cbEventNotifier);
    RESTAPI.log('Regist recorder notifier.......(%d)\n', ret);

    RESTAPI.log('=================================================================');
    bRegistNotifiers = true;
};


/**
 * Event Notifier 해지
 */
exports.unregistNotifiers = function () {
    if (bRegistNotifiers) {
        RESTAPI.log('=================== Unregist Event Notifier =======================');
        var ret;
        ret = binder.REST_BIND_CHAN_UnregisterNotifier(cbEventNotifier);
        RESTAPI.log('Unregist channel notifier.......(%d)\n', ret);

        ret = binder.REST_BIND_PLAYER_UnregisterNotifier(cbEventNotifier);
        RESTAPI.log('Unregist player notifier.......(%d)\n', ret);

        ret = binder.REST_BIND_SCAN_UnregisterNotifier(cbEventNotifier);
        RESTAPI.log('Unregist scanner notifier.......(%d)\n', ret);

        ret = binder.REST_BIND_EPG_UnregisterNotifier(cbEventNotifier);
        RESTAPI.log('Unregist epg notifier.......(%d)\n', ret);

        ret = binder.REST_BIND_SCHEDULE_UnregisterNotifier(cbEventNotifier);
        RESTAPI.log('Unregist schedule notifier.......(%d)\n', ret);

        ret = binder.REST_BIND_RECORD_UnregisterNotifier(cbEventNotifier);
        RESTAPI.log('Unregist record notifier.......(%d)\n', ret);

        ret = binder.REST_BIND_SETTINGS_UnregisterNotifier(cbEventNotifier);
        RESTAPI.log('Unregist settings notifier.......(%d)\n', ret);

        ret = binder.REST_BIND_TUNER_UnregisterNotifier(cbEventNotifier);
        RESTAPI.log('Unregist tuner notifier.......(%d)\n', ret);

        ret = binder.REST_BIND_RECORDER_UnregisterNotifier(cbEventNotifier);
        RESTAPI.log('Unregist recorder notifier.......(%d)\n', ret);

        RESTAPI.log('=================================================================');

        bRegistNotifiers = false;
    } else {
        RESTAPI.log('Notifiers Not Registed...');
    }
};


/**
 * STB의 Event를 전달 받기 위한 Notifier
 * typedef void (*RestBindPlayer_Notifier_t) (char *szEventType, char *szPayload);
 * @param eventType - 발생한 event의 종류
 * @param payload - 발생한 event의 내용
 */
function eventNotifier(eventType, payload) {
    var eventObj;

    // 전달받은 payload를 JSON Object화 하여 전달
    try {
        eventObj = JSON.parse(payload);
    } catch (e) {
        RESTAPI.elog(e);
        eventObj = {};
    }

    RESTAPI.log('#### NODE EVENT RECEIVE  #### module - ' + eventType + ', event - ' + eventObj.event);
    RESTAPI.emitEvent(eventType, eventObj);
}
exports.binder = binder;
global.RESTAPI.binder = binder;
