/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

nrdp.network = {
    _path: "network",
    addEventListener: function(evt, listener) { nrdp._addEventListener(this, evt, listener); },
    removeEventListener: function(evt, listener) { nrdp._removeEventListener(this, evt, listener); },


    _urlIndex:0,
    _dnsIndex:0,
    _urlList: [],
    _datas: {},
    _errorStack: [],
    _googleDNS: "8.8.8.8",
    _netflixDomain : "netflix.com",
    _isDiagnoseRunning: false,
    _abortDiagnosis: false,
    _abortDiagnosisCb:0,

    CONNTIMEOUT: 6000,
    DATATIMEOUT: 4000,

    CONNECTIONSTATE : {
        CONNECTED : 0,
        DISCONNECTED : 1,
        UNKNOWN : 2
    },

    get ERRORGROUP() { return this._syncData.ERRORGROUP; },
    get ERRORCODE() { return this._syncData.ERRORCODE; },
    

    RECOMMENDEDACTION : {
        RETRY : 0,
        RESTART  : 1,
        REACTIVATE  : 2,
        MODIFY_DNS : 3,
        REDIRECT_SETTINGS : 4,
        REINSTALL : 5,
        RUNDIAGTOOL : 6,
        LOG_ERROR  : 7
    },
    DNSERRORS : {
        SUCCESS : 0,
        NODATA  : 1,
        EFORMERR  : 2,
        SERVFAIL  : 3,
        NOTFOUND : 4,
        NOTIMP : 5,
        REFUSED : 6,
        BADQUERY : 7,
        BADNAME  : 8,
        BADFAMILY : 9,
        BADRESP : 10,
        CONNREFUSED : 11,
        TIMEOUT : 12,
        EOF : 13,
        EFILE : 14,
        NOMEM : 15,
        NODNSSERVER : 16,
        OTHER : 17
    },

    _sendCompletedEvent: function(ra) {
        this._urlIndex = 0;
        this._dnsIndex = 0;
        this._urlList = [];
        var endEvent = {
            type: "diagnosisResult",
            resultArray: this._errorStack,
            recommendedAction: ra
        };
        this._errorStack = [];
        this._isDiagnoseRunning = false;
        nrdp._callEventListeners(this, endEvent);
    },

    _sendAbortCb: function() {
        this._urlIndex = 0;
        this._dnsIndex = 0;
        this._urlList = [];
        this._errorStack = [];
        this._isDiagnoseRunning = false;
        this._abortDiagnosis = false;
        if (nrdp.network._abortDiagnosisCb) {
            nrdp.network._abortDiagnosisCb(true);
        }
    },
    _getDomain: function(url) {
        var domain = url.match(/:\/\/(www[0-9]?\.)?(.[^/:]+)/);
        if (domain) {
            return domain[2];
        }
        return "";
    },
    _dnsCallback: function (data, dns) {
        if (this._abortDiagnosis) {
            this._sendAbortCb();
            return;
       }
        var event = {
            URL: data.url,
            errorgroup: this.ERRORGROUP.DNS_CHECK,
            errorcode: (data.result > this.DNSERRORS.NOMEM) ?
                 this.DNSERRORS.OTHER :
                data.result,
            // recommendedAction: data.result,
            dnsip: dns,
            type: "diagnosisStatus",
            success: !data.result
        };
        nrdp._callEventListeners(this, event);
        this._errorStack.push(event);

        if ((dns == this._googleDNS) && (nrdp.device.dnslist.length < this._dnsIndex)) {
            var myurl = nrdp.options.appboot_url;
            this._dnsIndex = 0;
            this._continueDiag(myurl);
            return;
        }
        if (nrdp.device.dnslist.length > this._dnsIndex) {
            var nextdns = nrdp.device.dnslist[this._dnsIndex++];
            this.checkDNS(nextdns, event.URL, function(evt) {
                nrdp.network._dnsCallback(evt, nextdns);
            });
        } else {
            // now check google DNS
            this._dnsIndex++;
            this.checkDNS(this._googleDNS, event.URL, function(evt) {
                nrdp.network._dnsCallback(evt, nrdp.network._googleDNS);
            });
        }
    },
    _continueDiag: function(url) {
       if (this._abortDiagnosis) {
            this._sendAbortCb();
            return;
       }

        if (this._urlList.length > this._urlIndex) {
            this.getOperation(this._urlList[this._urlIndex++],
                              this.CONNTIMEOUT,
                              this.DATATIMEOUT,
                              function(evt) {
                                  nrdp.network._getCallback(evt);
                              });
        } else {
            // completed. send complete event
            var reco = this.RECOMMENDEDACTION.RETRY;
            for (var index in this._errorStack) {
                if ( (this._errorStack[index].errorcode == this.ERRORCODE.DNS_ERROR) &&  (reco == this.RECOMMENDEDACTION.RETRY) ){
                    reco = this.RECOMMENDEDACTION.REDIRECT_SETTINGS;
                }else if ( (this._errorStack[index].errorcode == this.ERRORCODE.CERT_STATUS_PEW_REVOKED) &&  (reco == this.RECOMMENDEDACTION.RETRY) ){
                    reco = this.RECOMMENDEDACTION.REACTIVATE;
                }
            }
            this._sendCompletedEvent(reco);
        }
    },
    _nflxUrlCallback: function(data, url) {
        if (!this._isDiagnoseRunning) {
            return;
        }
        if (this._abortDiagnosis) {
            this._sendAbortCb();
            return;
        }
        var event = {
            URL: data.url,
            errorgroup: data.errorgroup,
            errorcode: data.errorcode,
            nativeerrorcode: data.nativeerrorcode,
            // recommendedAction: data.result,
            type: "diagnosisStatus",
            success: !data.result
        };

        if (event.errorcode == this.ERRORCODE.DNS_ERROR) {
            //do dnscheck;
            if (nrdp.device.dnslist.length <= 0) {
                event.errorcode = this.ERRORCODE.NO_DNS_SERVER;
                nrdp._callEventListeners(this, event);
                this._errorStack.push(event);
                this._sendCompletedEvent(this.RECOMMENDEDACTION.REDIRECT_SETTINGS);
            } else {
                nrdp._callEventListeners(this, event);
                this._errorStack.push(event);
                var dns = nrdp.device.dnslist[this._dnsIndex++];
                // XXX any reason not to check using all DNS servers at same time?
                if (nrdp.network._getDomain(event.URL) != "") {
                    this.checkDNS(dns,
                                  nrdp.network._getDomain(event.URL),
                                  function(evt) {
                                      nrdp.network._dnsCallback(evt, dns);
                                  });
                } else {
                    this._continueDiag(url);
                }
            }
            return;
        }

        nrdp._callEventListeners(this, event);
        this._errorStack.push(event);
        this._continueDiag(url);

    },

    _getCallback: function(data) {
        if (!this._isDiagnoseRunning) {
            return;
        }
        if (this._abortDiagnosis) {
            this._sendAbortCb();
            return;
        }
        var event = {
            URL: data.url,
            errorgroup: data.errorgroup,
            errorcode: data.errorcode,
            nativeerrorcode: data.nativeerrorcode,
            // recommendedAction: data.result;
            type: "diagnosisStatus",
            success: !data.result
        };

        // XXX take same action if !event.success as _nflxUrlCallback?
        // recommendation to REACTIVATE if PEW revoked and recommendation to REDIRECT  for DNS error
        nrdp._callEventListeners(this, event);
        this._errorStack.push(event);
        if (this._urlList.length > this._urlIndex) {
            this.getOperation(this._urlList[this._urlIndex++],
                              this.CONNTIMEOUT,
                              this.DATATIMEOUT,
                              function(evt) {
                                  nrdp.network._getCallback(evt);
                              });
        } else {
            var reco = this.RECOMMENDEDACTION.RETRY;
            for (var index in this._errorStack) {
                if ( (this._errorStack[index].errorcode == this.ERRORCODE.DNS_ERROR) &&  (reco == this.RECOMMENDEDACTION.RETRY) ){
                    reco = this.RECOMMENDEDACTION.REDIRECT_SETTINGS;
                }else if ( (this._errorStack[index].errorcode == this.ERRORCODE.CERT_STATUS_PEW_REVOKED) &&  (reco == this.RECOMMENDEDACTION.RETRY) ){
                    reco = this.RECOMMENDEDACTION.REACTIVATE;
                }
            }
            this._sendCompletedEvent(reco);
        }
    },

    _isValidIpAvailable: function() {
        var ifList = nrdp.device.iflist;
        var found = false;
        for (var index in ifList) {
            if (ifList[index].ipAddress) {
                if ( (ifList[index].ipAddress.indexOf("127", 0) != 0) &&
                   (ifList[index].ipAddress != "0.0.0.0") &&
                   (ifList[index].ipAddress != "") &&
                   (ifList[index].ipAddress.indexOf("169.254", 0) != 0) ) {
                    found = true;
                    NTRACE("IP address is " + ifList[index].ipAddress);
                    break;
                }   
            } else if (ifList[index].ipv6Addresses && ifList[index].ipv6Addresses.length > 0){
                    found = true;
                    NTRACE("IPV6 address is " + JSON.stringify(ifList[index].ipv6Addresses));
                    break;                
            }
        }
        return found;
    },
    isValidIpAvailable: function() {
        return this._isValidIpAvailable();
    },
    diagnose: function(urls) {
        nrdp.network._datas = {};
        if (!nrdp.network._isValidIpAvailable()) {
            var event = {
                URL: "",
                errorgroup: nrdp.network.ERRORGROUP.NO_IP_ADDRESS,
                errorcode: nrdp.network.ERRORCODE.UNKNOWN_ERROR,
                nativeerrorcode: -1, // duplicated from NativeerrorCodes.h
                type: "diagnosisStatus",
                success: false
            };
            nrdp.log.debug("diagnosis status event " + JSON.stringify(event) );
            nrdp._callEventListeners(nrdp.network, event);
            nrdp.network._errorStack.push(event);
            nrdp.network._sendCompletedEvent(nrdp.network.RECOMMENDEDACTION.REDIRECT_SETTINGS);
            return;
        }
        if (!nrdp.network._abortDiagnosis) {
            nrdp.network._isDiagnoseRunning = true;
            // XXX need to handle multiple calls to diagnose in parallel?
            nrdp.network._urlList = urls;
            // XXX any reason not to check all urls at the same time?
            nrdp.network.getOperation(nrdp.options.appboot_url,
                                      nrdp.network.CONNTIMEOUT,
                                      nrdp.network.DATATIMEOUT,
                                      function(evt) {
                                          nrdp.network._nflxUrlCallback(evt, nrdp.options.appboot_url);
                                      });
        } else {
            nrdp.network._abortDiagnosis = false;
            nrdp.network._abortDiagnosisCb(true);
        }
    },

    abortDiagnosis: function(abortCb) {
        if (nrdp.network._isDiagnoseRunning) {
            nrdp.network._abortDiagnosis = true;
            nrdp.network._abortDiagnosisCb = abortCb;
        }
    },

    getOperation: function(url, connectiontimeout, datatimeout, callback) {
        // XXX what if there's two get operations for the same url at the same time?
        nrdp.network._datas[url] = {cb: callback, results: {}};
        nrdp._invoke("network", "get",
                     {url: url, connectiontimeout: connectiontimeout, datatimeout: datatimeout});
    },

    checkDNS: function(dnsip, url, callback) {
        // XXX what if there's two checks for the same url at the same time?
        nrdp.network._datas[url] = {cb: callback, results: {}};
        nrdp._invoke("network", "checkdns", {dnsip: dnsip, url: url});
    },
    useFallbackDnsOnly: function(fallback) {
        nrdp._invoke("network", "useFallbackDnsOnly", {fallback: fallback});
    },
    _handleEvent: function(event) {
        if (event.name != "INetwork") {
            return false;
        }
        if ( (event.data.type !== "dnsresult") && (event.data.type !== "getresult") ){
            return false;
        }

        var url = event.data.url;
        var data = this._datas[url];
        if (data) {
            data.cb(event.data);
        }

        return true;
    }
};
