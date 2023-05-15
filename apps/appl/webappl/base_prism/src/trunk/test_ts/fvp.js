var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
/// <reference path="typings/mocha/mocha.d.ts" />
/// <reference path="typings/jquery/jquery.d.ts" />
/**
 * Created by bglee on 2015-03-16.
 */
var CXMLParser = (function () {
    function CXMLParser() {
        this._parser = new DOMParser();
    }
    CXMLParser.prototype.construcutor = function () {
    };
    CXMLParser.prototype.parse = function (xml) {
        return this._parser.parseFromString(xml, 'application/xml');
    };
    return CXMLParser;
})();
var CFVPParser = (function (_super) {
    __extends(CFVPParser, _super);
    function CFVPParser() {
        _super.call(this);
    }
    return CFVPParser;
})(CXMLParser);
var CExpireManager = (function () {
    function CExpireManager() {
    }
    return CExpireManager;
})();
!function () {
    describe('FVP parser test', function () {
        var fvp = new CFVPParser();
        var proxy = 'http://10.0.12.51';
        var url = function (query) {
            return proxy + query;
        };
        var listQuery = [];
        it('get service of london', function (done) {
            $.get(url('/services?nid=12339'), function (xml) {
                console.log('success');
                console.log(xml);
                debugger;
            }, function (xml) {
                console.log('fail');
                console.log(xml);
                debugger;
            });
        });
    });
}();
