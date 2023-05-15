The following is a brief description of each file contained in this package:
----------------------------------------------------------------------------

DeviceTestCert.crt: This file is the device test certificate

PrivateKey-bsafe.bin: This file contains the private key in the format used by the bsafe api.

PrivateKey-KeyParts.txt: This file contains all parts of the public and private keys, in network byte order.

Vector.txt: This file contains a simple buffer encrypted with the public key and the expected results after decrypting it with the private key.

PublicKey.txt: This file contains the public key.

RegistrationRequestMessage.bin: This file contains the dump of a sample registration request message.

RegistrationResponseMessage.bin: This file contains the dump of a response to the above message.



The following is a description of the RSA key parts:
----------------------------------------------------

P: the first (large) prime number used in the RSA algorithm

Q: the second (large) prime number used in the RSA algorithm

Modulus (also known as: n, public modulus): The public modulus of the RSA algorithm. Its value is "P * Q". The modulus size corresponds to the key length of the algorithm

Exponent (also known as: e, public exponent): The public exponent of the RSA algorithm. Constant value of 65537 (0x10001) in WMDRM-ND.

DP (also known as: exp1, exponent 1): the exponent corresponding to the first prime. Its value is "D mod (P - 1)"

DQ (also known as: exp2, exponent 2): the exponent corresponding to the second prime. Its value is "D mod (Q - 1)"

InverseQ (also known as: coef, coefficient): it's the inverse of Q, modulus P, i.e., a value such as "(Q * InverseQ) mod P == 1".

D (also known as: prvexp, private exponent): the private exponent of the RSA algorithm
 

NOTE: the byte arrays presented in the sample private key are in network byte order (big endian).