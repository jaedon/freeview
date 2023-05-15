/**
 * Created by bglee on 2015-01-27.
 */
/// <reference path="../def/require.d.ts" />
/// <reference path="../def/jquery.d.ts" />
/// <reference path="../def/lodash.d.ts" />
/// <reference path="../def/sugar.d.ts" />

import _model_ = require("model/model");
function fnMakeOIPFObject(): any {
    //XXX: HMXConnect.js
    var ret: any = {};

    hx.log("warn", "[fnMakeOIPF] make OIPF object");
    ret.videoBroadcast = window.oipfObjectFactory.createHumaxVideoBroadcastObject();
    ret.channelConfig = ret.videoBroadcast.getChannelConfig();
    ret.configuration = window.oipfObjectFactory.createHumaxConfigurationObject();
    ret.avController = window.oipfObjectFactory.createHumaxVideoOnDemandObject();
    ret.applicationManager = window.oipfObjectFactory.createApplicationManagerObject();
    ret.parentalControlManager = window.oipfObjectFactory.createParentalControlManagerObject();
    ret.searchManager = window.oipfObjectFactory.createSearchManagerObject();
    ret.recordingScheduler = window.oipfObjectFactory.createRecordingSchedulerObject();
    ret.casUtil = window.oipfObjectFactory.createHumaxCasUtil();
    ret.storageUtil = window.oipfObjectFactory.createHumaxStorageUtilObject();
    ret.hDlna = window.oipfObjectFactory.createDlnaManagerObject();
    ret.hSamba = window.oipfObjectFactory.createSambaManagerObject();
    ret.hFtp = window.oipfObjectFactory.createFtpManagerObject();
    ret.hContentManager = window.oipfObjectFactory.createContentManagerObject();
    ret.hWoon = window.oipfObjectFactory.createWoonManagerObject();
    ret.hBluetoothManager = window.oipfObjectFactory.createBluetoothManagerObject();
    ret.hDial = window.oipfObjectFactory.createDialManagerObject();
    ret.hSatip = window.oipfObjectFactory.createSatipManagerObject();
    ret.hAirPlay = window.oipfObjectFactory.createAirplayManagerObject();
    ret.operatorFeature = window.oipfObjectFactory.createHumaxOperatorFeatureObject();
    ret.hAlexa = window.oipfObjectFactory.createAlexaManagerObject();
    hx.log("warn", "[fnMakeOIPF] make OIPF object");

    return ret;
}
var hx: ifSugar;
var ENV: ifENV = <ifENV>{};
var OIPF;
var fn_print_lcn_present = function () {
    var currentCH = OIPF.videoBroadcast.currentChannel;
    var present = OIPF.videoBroadcast.programmes[0];
    var log = [
        "LCN: " + currentCH.majorChannel,
        "Present: " + present.title
    ];
    hx.log("debug", log.join("\n"));
};
var eventReceiver = {
    onKey: (code, repeat, type) => {
        if (type === "keyup") {
            return;
        }
    },
    onProgrammesChanged: function(programme) {
        var item = new _model_.cPdeProgrammeItem(programme);
        hx.log("warn", "^[onProgrammesChanged]");
        fn_print_lcn_present();
        hx.log("debug", item.toString());
        hx.log("warn", "$[onProgrammesChanged]");
    },
    onChannelChangeSucceeded: function() {
        hx.log("warn", "^[onChannelChangeSucceeded]");
        fn_print_lcn_present();
        hx.log("warn", "$[onChannelChangeSucceeded]");
    },
    onChannelChangeError: function() {
        hx.log("warn", "^[onChannelChangeError]");
        fn_print_lcn_present();
        hx.log("warn", "$[onChannelChangeError]");
    }
};
require([
    "lib/ext/jquery-1.7.1",
    "lib/ext/lodash.min",
    "lib/local/logger",
    "env/keytable",
    "layer_domain/service/dl_key_event_binder"
], function($, _, _logger_, _key_table_,  _key_event_binder_) {
    hx = _logger_;
    hx.key = _key_table_;
    var keyBinder = new _key_event_binder_("binder");
    keyBinder.bind(document, eventReceiver);

    OIPF = fnMakeOIPFObject();
    OIPF.videoBroadcast.addEventListener("ProgrammesChanged", eventReceiver.onProgrammesChanged);
});
