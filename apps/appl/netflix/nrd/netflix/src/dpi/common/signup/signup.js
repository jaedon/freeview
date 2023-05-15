/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

nrdp.signup = {

    getExternalSignUpServices: function() {
        return this._syncData.externalSignUpServices;
    },

    getExternalUserData: function(service, userid, cbo) {
        nrdp.signup._fn("getExternalUserData", {service: service, uid:userid}, cbo);
    },

    requestExternalUserConfirmation: function(service, userid, cbo) {
        nrdp.signup._fn("requestExternalUserConfirmation", {service: service, uid:userid}, cbo);
    },

    _nextIdx: 1,
    _cbs: {},
    _fn: function(name, args, cb) {
        if (!args) args = {};
        args.idx = this._nextIdx++;
        this._cbs[args.idx] = cb;
        nrdp._invoke("signup", name, args);
    },

    _handleEvent: function(event) {

        if (event.data && event.data.idx) {
            if (typeof this._cbs[event.data.idx] == "object") {
                if ((this._cbs[event.data.idx] != null) &&
                    (typeof this._cbs[event.data.idx][event.name] == "function")) {
                    switch (event.name) {
                    case "onSuccess":
                        var userid = event.data.uid;
                        var token = event.data;
                        var index = event.data.idx;
                        delete token.idx;
                        delete token.uid;
                        this._cbs[index][event.name](userid, token);
                        break;
                    case "onFailure":
                        this._cbs[event.data.idx][event.name](event.data.uid, event.data.errorcode, event.data.message);
                        break;
                    case "onComplete":
                        this._cbs[event.data.idx][event.name]();
                        delete this._cbs[event.data.idx];
                        break;
                    default:
                        break;
                    }
                }
            }
        } else {
            return false;
        }
        return true;
    },

    _syncData: {}

};
