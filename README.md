# file_encryptor by Neslihan Guler
A simple command line program to encrypt/decrypt files

# How to build:<br />
```
git clone https://github.com/neslihanguler/file_encryptor<br />
cd file_encryptor/src<br />
make<br />
```

# Usage:<br />
`./nescrypt -c <file_name> <password>`  -> encrypt file  <br />
`./nescrypt <file_name> <password>` -> decrypt file<br />

# Sample Usage:<br />
`./nescrypt -c sample.txt "cryptography is great" `-> encrypts file.txt and writes in sample.txt.nes<br />
`./nescrypt sample.txt.nes "cryptography is great"`->decrypts sample.txt.nes and writes in sample.txt<br />
This command line program (nescrypt) gets file_name and password as input parameters and encrypts/decrypts given file. <br />

# Crypto Library: <br />
nescrypt uses cryptographic primitives of Libsodium crypto library. 
It is a portable, cross-compilable, installable, packageable fork of NaCl, with a compatible API, and an extended API to improve usability even further. I preferred to use NaCl since I've used it before and I experienced that it has very easy to use APIs and secure_by_default constructions. I also used openSSL in the past for different purposes. To compare: implementing crypto is hard and error prone, especially while using complex to use libraries like openSSL. openSSL is more open to user errors. NaCl advances the state of the art by improving security, usability, and speed.<br />

# Key Derivation Method:<br />
Secret keys used to encrypt (or sign) confidential data have to be chosen from a very large keyspace. However, passwords are usually short,  human-generated strings, making dictionary attacks practical. I decided to use Argon2 function of libsodium which provides a password hashing scheme through the use of an unpredictable salt. It is resistant to GPU cracking attacks and side channels.<br />
https://libsodium.gitbook.io/doc/password_hashing/the_argon2i_function<br />
https://en.wikipedia.org/wiki/Argon2<br />

The salt should be unpredictable (to avoid rainbow table attack, but it is not secret) so using a random value would be the best method to generate a salt. Before encryption I create a random salt and write it to the beginning of ciphertext file. While decrypting the file this salt value is read from file first and the encryption key is calculated by using salt and pasword. <br />

# Authenticated Encryption: <br />
Authenticated encryption is known as AEAD (Authenticated Encryption with Associated Data) where a single algorithm protects the confidentiality and integrity of data simultaneously. They also support providing integrity for associated data which is not encrypted.<br />

The XChaCha20-Poly1305 AEAD construction can safely encrypt a practically unlimited number of messages with the same key, without any practical limit to the size of a plain text. It uses a large nonce size (192-bit) as an alternative to counters, so random nonces can be used safely for large file encryption. <br />

nescrypt reads a file chunk by chunk (chunk size is 4096), each chunk is encrypted and an authentication tag (16-bytes) is binded to calculated cipher text. Both ciphertext and auth tag are written in encrypted file. In decryption phase, the tag is verified to catch if a modification made on ciphertext file. There is a combined mode of AEAD algorithm implemented in Libsodium where the authentication tag and the encrypted message are stored together. This approach makes thing easier and solves mac-then-encrypt vs encrypt-then-mac problem.<br /> 

Cryptographic security of xchacha20poly1305 AEAD algorithm depends on use of unique nonce values for distinct messages. Same nonce should never be reused with the same key. That means we need a unique nonce for each chunk. I decided to create a random nonce value for encrypting first block and then increment nonce by a seq_number for each subsequent data block. For decryption I need to know the initial nonce value then. The initial value is being appended to the beginning of encrypted text, right after salt.





 
