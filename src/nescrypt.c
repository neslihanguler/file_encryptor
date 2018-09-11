#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <sodium.h>
#include <stdbool.h>

void nes_encrypt(const char *filename, const char *password);
void nes_decrypt(const char *filename, const char *password);

void calc_nonce (unsigned char *nonce, unsigned char *seed, uint64_t seq_number);

#define CHUNK_SIZE 4096

int main(int argc , char *argv[]) 
{
    bool encryptor = false;
    
    char filename[30];
    char password[30];
    
    /* Parse cmd line args */
    int option;
    while ((option = getopt(argc, argv, "c")) != -1) {
        encryptor = true;
    }
    
    bzero(filename, 30);
    
    int position = 0;
    while (optind < argc) {
        const char *arg = argv[optind];
        switch (position) {
            case 0:
                strncpy(filename, arg, strlen(arg));
                break;
            case 1:
                strncpy(password, arg, strlen(arg));
            default:
                perror("unknown argument");
                break;
        }
        optind++;
        position++;
    }

    printf("filename: %s\npassword: %s", filename, password);
    
    if (encryptor) {
        nes_encrypt(filename, password);
    }
    else {
        nes_decrypt(filename, password);
    }
    
    
    return 0;

}

void nes_encrypt(const char *source, const char *password) {
    
    unsigned char read_buf[CHUNK_SIZE];
    unsigned char cipher[CHUNK_SIZE + crypto_aead_xchacha20poly1305_ietf_ABYTES]; //cipher text + mac
    FILE *fp_s, *fp_d;
    
    char target[33];
    bzero(target, 33);
    sprintf(target, "%s.nes", source);
    
    fp_s = fopen(source, "rb");
    fp_d = fopen(target, "wb");
    
    //create a random 24-byte nonce (seed) value and write it at the beginning of destination file
    unsigned char seed[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];
    randombytes_buf(seed, sizeof seed); /* 192-bits nonce */
    fwrite(seed, 1, crypto_aead_xchacha20poly1305_ietf_NPUBBYTES, fp_d);
    
    uint64_t seq_number = 0;
    int rlen;
    unsigned long long clen;
    
    do {
        rlen = fread(read_buf, 1, sizeof read_buf, fp_s);
        unsigned long long mlen = rlen;
        unsigned char nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];
        calc_nonce(nonce, seed, seq_number);
        crypto_aead_xchacha20poly1305_ietf_encrypt(cipher, &clen, (const unsigned char *)read_buf, rlen,
                                                   NULL, 0, NULL, nonce, (const unsigned char *)password);
        if (clen != mlen + crypto_aead_xchacha20poly1305_ietf_abytes()) {
            printf("\nEncryption Error!");
            fclose(fp_s);
            fclose(fp_d);
            return;
        } else {
            fwrite(cipher, 1, (size_t) clen, fp_d);
            seq_number++;
        }
    } while(!feof(fp_s));
    
    fclose(fp_s);
    fclose(fp_d);
    
    printf("\n%s encrypted and saved in %s", source, target);
    
    return;
}

void nes_decrypt(const char *source, const char *password) {
    
    return;
}


/* Cryptographic security of xchacha20poly1305 AEAD algorithm depends on use of unique nonce
   values for distinct messages. Same nonce should never be reused with the same key.
   I decided to create a random nonce value for encrypting first block and then increment nonce
   for each subsequent data block.
*/
void calc_nonce (unsigned char *nonce, unsigned char *seed, uint64_t seq_number) {
    
    memcpy(nonce, seed, crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
    unsigned char sequence[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES] = {'\0'};
    int position = crypto_aead_xchacha20poly1305_ietf_NPUBBYTES - sizeof(seq_number);
    memcpy(sequence + position, &seq_number, sizeof(seq_number)); //copy sequence number to last 8 bytes
    sodium_add(nonce, (const unsigned char *)sequence, sizeof(sequence)); //add two little endian big numbers
}


