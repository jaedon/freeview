/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

/* global nrdp */

nrdp.test = {

	aesDecrypt: function aesDecrypt(key, init, data)
	{
		nrdp.webcrypto.aesDecrypt(
			key, init, data,
			function (val)
			{
				var output = val.data;
				nrdp.log.console("aesDecrypt callback: " + output);
			});
	},

	aesEncrypt: function aesEncrypt(key)
	{
		var init = "aW5pdHZlY3RvcmFiY2RlCg=="; // "initvectorabcde"
		var data = "dGVzdGRhdGEK"; // "testdata"

		nrdp.webcrypto.aesEncrypt(
			key, init, data,
			function (val)
			{
				var output = val.data;
				nrdp.log.console("aesEncrypt callback: " + output);

				nrdp.test.aesDecrypt(key, init, output);
			});
	},

	importKey: function importKey()
	{
		var testkey = "dGVzdGtleTg5MGFiY2RlCg=="; // "testkey890abcde"
		var algorithm = {
			name: "AES-GCM",
			params:	{
				hash: {
					name: "SHA256"
				},
			}
		};
		return nrdp.webcrypto.importKey(
			testkey, nrdp.webcrypto.RAW, algorithm, true,
            [nrdp.webcrypto.ENCRYPT, nrdp.webcrypto.DECRYPT],
			function (val)
			{
				var key = val.key.handle;
				nrdp.log.console("importKey key: " + key);

				nrdp.test.aesEncrypt(key);
			});
	},

	rsaDecrypt: function rsaDecrypt(privKey, data)
	{
		nrdp.webcrypto.rsaDecrypt(
			privKey, data,
			function (val)
			{
				var output = val.data;
				nrdp.log.console("rsaDecrypt callback: " + output);
			});
	},

	rsaEncrypt: function rsaEncrypt(pubKey, privKey)
	{
		var data = "dGVzdGRhdGEK"; // "testdata"

		nrdp.webcrypto.rsaEncrypt(
			pubKey, data,
			function (val)
			{
				var output = val.data;
				nrdp.log.console("rsaEncrypt callback: " + output);

				nrdp.test.rsaDecrypt(privKey, output);
			});
	},

	rsaKeyGen: function rsaKeyGen()
	{
		var algorithm = {
			name: "RSAES-PKCS1-v1_5",
			params: {
				modulusLength: 1024,
				publicExponent: "cHVibGljZXhwCg==" // "publicexp"
			},
		}
		return nrdp.webcrypto.rsaKeyGen(
			algorithm, true, [nrdp.webcrypto.ENCRYPT, nrdp.webcrypto.DECRYPT],
			function (val)
			{
				var pubKey = val.publickey.handle;
				var privKey = val.privatekey.handle;
				nrdp.log.console(
					"rsaKeyGen callback: pub=" + pubKey + ", priv=" + privKey);

				nrdp.test.rsaEncrypt(pubKey, privKey);
			});
	},

	symKeyGen: function symKeyGen(algorithm)
	{
		return nrdp.webcrypto.symKeyGen(
			algorithm, true, [nrdp.webcrypto.ENCRYPT, nrdp.webcrypto.DECRYPT],
			function (val)
			{
				var key = val.key.handle;
				nrdp.log.console(
					"symKeyGen callback: key=" + key);

				nrdp.test.aesEncrypt(key);
			});
	},

	testSymKey: function testSymKey()
	{
		var hashalgorithm = {
			name: "HMAC",
			params: {
				hash: {
					name: "SHA-256"
				}
			}
		};
		var aesalgorithm = {
			name: "AES-GCM",
			params: {
				length: 256
			}
		}

		nrdp.test.symKeyGen(hashalgorithm);

		nrdp.test.symKeyGen(aesalgorithm);
	},

	testAes: function testAes(key)
	{
		nrdp.test.importKey();
	},

	testRsa: function testRsa(key)
	{
		nrdp.test.rsaKeyGen();
	},

	testDigest: function testDigest()
	{
		var data = "dGVzdGRhdGEK"; // "testdata"

		nrdp.webcrypto.digest(
			data, "SHA-1",
			function (val)
			{
				nrdp.log.console("digest SHA-1 callback: " + val.success);
			});

		nrdp.webcrypto.digest(
			data, { name: "SHA-256" },
			function (val)
			{
				nrdp.log.console("digest SHA-256 callback: " + val.success);
			});
	},

	go: function go()
	{
//		nrdp.test.testRsa();
//		nrdp.test.testAes();
	}
};
