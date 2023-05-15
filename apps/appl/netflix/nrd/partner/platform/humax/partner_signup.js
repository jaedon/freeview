nrdp.signup = {
    _syncData: {},

    getExternalPaymentServices: function() {
        return this._syncData.externalPaymentServices;
    },

    getExternalSignUpServices: function() {
        return this._syncData.externalSignUpServices;
    },

    getUserConfirmationType: function(service) {
        return this._syncData.userConfirmationType;
    },

    getExternalPaymentPartnerData: function(service, locale, cbo) {
        nrdp.signup._fn("getPaymentPartnerData", {}, cbo);
    },

    getExternalUserData: function(service, cbo) {
        nrdp.signup._fn("getUserData", {}, cbo);
    },

    requestExternalPaymentData: function(service, cbo) {
        nrdp.signup._fn("getPaymentData", {}, cbo);
    },

    authorizeCredentials: function(service, credentials, cbo) {
        nrdp.signup._fn("authorizeCredentials", credentials, cbo);
    },

    resetCredentials: function() {
        nrdp.signup._fn("resetCredentials", {}, {});
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
                        var index = event.data.idx;
                        var userData = event.data;
                        delete userData.idx;
                        this._cbs[index][event.name](userData);
                        break;
                    case "onFailure":
                        var error = event.data.error;
                        this._cbs[event.data.idx][event.name](error);
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
    }
};

