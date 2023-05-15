/**
 * Created by ssyun on 14. 3. 14.
 * Channel 관련 요청을 처리하는 모듈
 */
'use strict';

// 내부 모듈 import
var url = require('url');
var fs = require('fs');

// 외부 모듈 import

// 서브 모듈 import

// Local Variable

var USB_HTTP_PAGE_ROOT = '//media/drive1/';
var USB_HTTP_PAGE_ROOT2 = '//media/drive';
/**
 *  Method : GET
 *  API : /jstbtest/:page
 *  Description : Session Test를 위한 API.
 *  */

var openUsbFunc = function (request, response, pathname, count, error, data) {
    if (count >= 10)
    {
        RESTAPI.log(error);
        RESTAPI.sendError(response, 'ERR_PAGE_NOTFOUND');
    }
    else {
        if (error) {
            RESTAPI.log(error);
            fs.readFile(USB_HTTP_PAGE_ROOT2 + count + '/' + pathname, function (rerror, rdata) {
                openUsbFunc(request, response, pathname, count + 1, rerror, rdata);
            });
        }
        else {
            response.writeHead(200, { 'Content-Type': mimeType });
            response.end(data);
        }
    }
}

 exports.usbHttp = function (request, response) {
    var parsedObject = url.parse(request.url);
    var ext = RESTAPI.extractExt(parsedObject.pathname);
    var mimeType = RESTAPI.getMineTypeByExt(ext);

    RESTAPI.log('ext = %s, mimetype = %s', ext, mimeType);

    fs.readFile(USB_HTTP_PAGE_ROOT + parsedObject.pathname, function (error, data) {
        if (error) {
            openUsbFunc(request, response, parsedObject.pathname, 2, error, data);
           // RESTAPI.log(error);
           // RESTAPI.sendError(response, 'ERR_PAGE_NOTFOUND');
        } else {
            response.writeHead(200, { 'Content-Type': mimeType });
            response.end(data);
            //response.send(data);
        }
    });
};


