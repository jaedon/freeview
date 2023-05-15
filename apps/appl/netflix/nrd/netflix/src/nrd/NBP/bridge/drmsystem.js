/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/* global nrdp */

nrdp.drmsystem = {
    // properties
    _path : "drmsystem",

    get INVALID_LICENSE() { return 0;},
    get LIMITED_DURATION_LICENSE() { return 1;},
    get STANDARD_LICENSE() { return 2;},

    get drmType() { return this._syncData.drmType; },
    get supportSecureStop() { return this._syncData.supportSecureStop; },
    get supportStorageDeletion() { return this._syncData.supportStorageDeletion; },

    get isOpen() { return this._syncData.isOpen; },

    /*
     * drm session state - defined at IDrmSession.h
     * enum SessionState {
     *     LicenseAcquisitionState = 0,
     *     InactiveDecryptionState,
     *     ActiveDecryptionState,
     *     InvalidState }
     */
    get DRMSESSION_LICENSE_ACQUISITION_STATE() { return 0;},
    get DRMSESSION_INACTIVE_DECRYPTION_STATE() { return 1;},
    get DRMSESSION_ACTIVE_DECRYPTION_STATE() {return 2;},
    get DRMSESSION_INVALID_STATE() { return 3;},

    /*
     * drm system open/close
     */
    openDrmSystem: function(drmType, cb)
    {
        nrdp.drmsystem._fn("openDrmSystem", {drmType:drmType}, cb);
    },

    closeDrmSystem: function(drmType, cb)
    {
        nrdp.drmsystem._fn("closeDrmSystem", {drmType:drmType}, cb);
    },

    /*
     * secure stop JS APIs
     */
    getSecureStopIds: function(cb)
    {
        nrdp.drmsystem._fn("getSecureStopIds", {}, cb);
    },

    getSecureStop: function(secureStopId, cb)
    {
        var args = {'secureStopId': secureStopId};
        nrdp.drmsystem._fn("getSecureStop", args, cb);
    },

    commitSecureStop: function(secureStopId, serverResponse)
    {
        var args = {'secureStopId': secureStopId, 'serverResponse': serverResponse};
        nrdp.drmsystem._fn("commitSecureStop", args);
    },

    resetSecureStops: function()
    {
        nrdp.drmsystem._fn("resetSecureStops");
    },

    useSecureStop: function(enable)
    {
        var args = {'enable': enable};
        nrdp.drmsystem._fn("enableSecureStop", args);
    },

    isSecureStopEnabled: function isSecureStopEnabled()
    {
        return nrdp._invoke("drmsystem", "isSecureStopEnabled");
    },

    /*
     * delete drm license store / key store JS APIs
     */
    deleteDrmStore: function(cb)
    {
      nrdp.drmsystem._fn("deleteDrmStore", {}, cb);
    },

    deleteKeyStore: function(cb)
    {
      nrdp.drmsystem._fn("deleteKeyStore", {}, cb);
    },

    getDrmStoreHash: function(cb)
    {
        nrdp.drmsystem._fn("getDrmStoreHash", {}, cb);
    },

    getKeyStoreHash: function(cb)
    {
        nrdp.drmsystem._fn("getKeyStoreHash", {}, cb);
    },

    getDrmTime: function getDrmTime()
    {
        return nrdp._invoke("drmsystem", "getDrmTime");
    },

    /*
     * License handling JS APIs
     */
    generateChallenge: function(drmType, contentId, licenseType, drmHeader, cb)
    {
        var args = {
            'drmType':drmType,
            'contentId':contentId,
            'licenseType':licenseType,
            'drmHeader':drmHeader
        };
        nrdp.drmsystem._fn("generateChallenge", args, cb);
    },

    provideLicense: function(sessionId, license, cb)
    {
        var args = {
            'sessionId':sessionId,
            'license':license
        };
        nrdp.drmsystem._fn("provideLicense", args, cb);
    },

    getDrmSessionIds: function(cb)
    {
        nrdp.drmsystem._fn("getDrmSessionIds", {}, cb);
    },

    deleteDrmSession: function(sessionId, cb)
    {
        var args = {
            'sessionId':sessionId
        };
        nrdp.drmsystem._fn("deleteDrmSession", args, cb);
    },

    flushDrmSessions: function(cb)
    {
        nrdp.drmsystem._fn("flushDrmSessions", {}, cb);
    },

    getLdlSessionsLimit: function(cb)
    {
        nrdp.drmsystem._fn("getLdlSessionsLimit", {}, cb);
    },

    _fn: function(name, params, cb)
    {
        if (!params) params = {};
        params.idx = this._nextIdx++;
        if (cb)
            this._cbs[params.idx] = cb;
        nrdp._invoke("drmsystem", name, params);
        return params.idx;
    },

    _handleEvent: function(event)
    {
        var cb, idx;

        if (event.name != "result"){
            return false;
        }

        idx = event.data.idx;
        if (typeof this._cbs[idx] == "function")
        {
            cb = this._cbs[idx];
            delete event.data.idx;
            cb(event.data);
        }
        delete this._cbs[idx];

        return true;
    },

    _nextIdx: 1,
    _cbs: {}
};
