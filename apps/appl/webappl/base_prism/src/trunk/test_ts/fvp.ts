/// <reference path="typings/mocha/mocha.d.ts" />
/// <reference path="typings/jquery/jquery.d.ts" />
/**
 * Created by bglee on 2015-03-16.
 */
class CXMLParser {
    private _parser = new DOMParser();
    construcutor() {

    }
    parse(xml: string) {
        return this._parser.parseFromString(xml, 'application/xml');
    }
}
class CFVPParser extends CXMLParser {
    constructor() {
        super();
    }
}
class CExpireManager {

}
!function() {
    describe('FVP parser test', function() {
        var fvp = new CFVPParser();
        var proxy = 'http://10.0.12.51';
        var url = function(query) {
            return proxy + query;
        };
        var listQuery = [];
        it('get service of london', function(done) {
            $.get(url('/services?nid=12339'),
                function(xml) {
                    console.log('success');
                    console.log(xml);
                    debugger;
                },
                function(xml) {
                    console.log('fail');
                    console.log(xml);
                    debugger;
                }
            );
        })
    });
}();
