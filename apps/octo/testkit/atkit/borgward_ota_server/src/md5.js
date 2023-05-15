// src/md5.js

var fs     = require('fs');
var crypto = require('crypto');

function md5sum(filename) {
  var genChecksum = null;
  var checksum = crypto.createHash('md5');
  var bytesRead = 1;
  var pos = 0
  var buffer = new Buffer(1024*64); // 64Kbyte
  var fd = fs.openSync(filename, 'r');
  var data = null;
  while (bytesRead > 0) {
    bytesRead = fs.readSync(fd, buffer, 0, buffer.length, pos);
    pos += bytesRead;
    if (bytesRead === buffer.length) {
      checksum.update(buffer);
    } else {
      data = buffer.slice(0, bytesRead);
      checksum.update(data);
    }
  }
  fs.closeSync(fd);
  genChecksum = checksum.digest('hex');
  return genChecksum;
};

exports.getCheckSum = md5sum;
