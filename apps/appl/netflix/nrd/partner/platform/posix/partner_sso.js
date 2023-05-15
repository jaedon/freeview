nrdp.sso = {
    _syncData: {},

    getExternalSsoServices: function() {
        return this._syncData.externalSsoServices || [];
    },

    getExternalUserToken: function(service, cbo) {
        nrdp.sso._fn("getExternalUserToken", {service: service}, cbo);
    },

    _nextIdx: 1,
    _cbs: {},
    _fn: function(name, args, cb) {
        if (!args) args = {};
        args.idx = this._nextIdx++;
        this._cbs[args.idx] = cb;
        nrdp._invoke("sso", name, args);
    },

    _handleEvent: function(event) {
         if (event.data && event.data.idx) {
            if (typeof this._cbs[event.data.idx] == "object") {
                if ((this._cbs[event.data.idx] != null) &&
                    (typeof this._cbs[event.data.idx][event.name] == "function")) {
                    switch (event.name) {
                    case "onSuccess":
                        var data = event.data;
                        var index = event.data.idx;
                        this._cbs[index][event.name](data);
                        break;
                    case "onFailure":
                        var error = event.data.error;
                        this._cbs[event.data.idx][event.name](error);
                        break;
                    case "noUser":
                        this._cbs[event.data.idx][event.name]();
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


