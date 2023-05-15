/**
 * Created by bglee@humaxdigital.com on 2015-03-19.
 * freeview play authentication test script
 */

!(function() {
    this.fvp = this.fvp || {};
    var __domParser = new DOMParser();
    var __baseUrl;
    var __headers = { Accept: 'application/vnd.fvc.v1+xml' };
    var fr = new FileReader();
    var authenticator = function(callback, caller, query) {
        var url = 'https://auth-ctv.cp-test.digitaluk.co.uk';
        $.ajax({
            type: 'head',
            url: url,
            headers: __headers,
            success: function(str, num, xhr) {
                console.log('success: auth ');
                callback(
                    xhr.getResponseHeader('x-baseurl'),
                    xhr.getResponseHeader('x-auth-hash'),
                    xhr.getResponseHeader('x-auth-timestamp'),
                    caller,
                    query
                );
            },
            error: function(error) {
                console.log('fail: auth');
                callback(null);
            }
        });
    };
    var setAuthInfo = function(baseurl, hash, timestamp, callback, query) {
        console.log('set auth info');
        if (baseurl && hash && timestamp) {
            __baseUrl = baseurl;
            __headers['x-auth-hash'] = hash;
            __headers['x-auth-timestamp'] = timestamp;
            callback(query);
        }
    };
    var callAPI = function(query) {
        console.log('callAPI: ' + query);
        if (!query) return;
        var callee = arguments.callee;
        if (!__baseUrl) {
            authenticator(setAuthInfo, callee, query);
            return;
        }
        $.ajax({
            type: 'get',
            url: __baseUrl + query,
            headers: __headers,
            success: function(xml) {
                console.log('success: get xml');
                var dom = __domParser.parseFromString(xml, 'application/xml');
                console.log(dom.toString());
            },
            error: function(str, status, xhr) {
                if (xhr.status === 401) {
                    authenticator(setAuthInfo, callee, query);
                }
                console.log('fail: get xml');
            }
        });
    };
    var getImage = function(query) {
        console.log('getImage: ' + query);
        if (!query) return;
        var callee = arguments.callee;
        if (!__baseUrl) {
            authenticator(setAuthInfo, callee, query);
            return;
        }
        var xhr = new XMLHttpRequest();
        xhr.onload = function(e) {
            if (this.readyState === 4 && this.status === 200) {
                fr.onload = function() {
                    console.log('success: get image');
                    window.bgleeImage = (function(url) {
                        return function() {
                            console.log('url: ' + url);
                            if ($('#focus img').length) {
                                $('#focus img').attr('src', url);
                            }
                        }
                    })(fr.result);
                };
                fr.readAsDataURL(this.response);
            }
        };
        xhr.open('GET', __baseUrl.replace('http://api', 'http://img') + query);
        for (var key in __headers) {
            xhr.setRequestHeader(key, __headers[key]);
        }
        xhr.responseType = 'blob';
        xhr.send();
    };
    fr.onload = function() {
        console.log('success: get image');
        debugger;
        window.bgleeImage = (function(url) {
            return function() {
                console.log('url: ' + url);
                if ($('#focus img').length) {
                    $('#focus img').attr('src', url);
                }
            }
        })(fr.result);
    };
    fr.onerror = function() {
        console.log('fail: get image');
    };
    this.fvp.call = callAPI;
    this.fvp.image = getImage;
}).call(this);
//fvp.call('/services?nid=12339');
//fvp.call('/services?sids[]=17541');
fvp.image('/apps/itv-player.png?w=192');
//fvp.image('/placeholder/192_108/Freeview_Test_Placeholder_192_108.jpg?w=192');
