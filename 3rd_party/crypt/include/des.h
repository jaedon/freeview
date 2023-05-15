#ifndef _DES_H_
#define _DES_H_

/*****************************************************************************
* Definitions, typedef & const data
*****************************************************************************/

/* definitions, typedef & const data */


/*****************************************************************************
*   Global function prototypes
*****************************************************************************/

/* global function prototypes */
void encrypt (char *key, char *plaintext, char *ciphertext);
void decrypt (char *key, char *ciphertext, char *plaintext);

void encrypt3 (char *key, char *plaintext, char *ciphertext);
void decrypt3 (char *key, char *ciphertext, char *plaintext);





void des_init(char *key);
void des3_init(char *key);

void encrypt_process(char *plaintext, char *ciphertext);
void decrypt_process(char *ciphertext, char *plaintext);

void encrypt3_process(char *plaintext, char *ciphertext);
void decrypt3_process(char *ciphertext, char *plaintext);





#endif /* _DES_H_ */
