/**
 * Created by bglee on 2015-03-02.
 */

var https = require('https');
var fs = require('fs');
var path = require('path');

var host = 'auth-ctv.cp-test.digitaluk.co.uk';
var cert = fs.readFileSync(path.join('certs', 'client', 'hdr4000t_fvc_mds_20141204.pem'));
var key = fs.readFileSync(path.join('certs', 'client', 'hdr4000t_fvc_mds_20141204.key.pem'));
var agent = new https.Agent({
    host: host,
    cert: cert,
    key: key
});
var options = {
    host: host,
    method: 'HEAD',
    //headers: {
    //    Accept: 'application/vnd.fvc.v0+xml'
    //},
    agent: agent
};
https
    .request(options, function(res) {
        console.log(res.statusCode);
        console.log(res.headers);
        if (res.statusCode === 200) {
            console.log(res.headers['x-auth-timestamp']);
            console.log(new Date(parseInt(res.headers['x-auth-timestamp']) * 1000));
        }
    })
    .end();