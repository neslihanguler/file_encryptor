# file_encryptor
A simple command line program to encrypt/decrypt files

How to build:
git clone https://github.com/neslihanguler/file_encryptor
cd file_encryptor/src
make

Usage:
./nescrypt -c <file_name> <password>  -> encrypt file
./nescrypt <file_name> <password> -> decrypt file

Sample Usage:
./nescrypt -c sample.txt "cryptography is great" -> encrypts file.txt and writes in sample.txt.nes
./nescrypt sample.txt.nes "cryptography is great" ->decrypts sample.txt.nes and writes in sample.txt
