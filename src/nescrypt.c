#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <sodium.h>
#include <stdbool.h>

void nes_encrypt(const char *filename, const char *password);
void nes_decrypt(const char *filename, const char *password);

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
    
    //create a random 24-byte nonce value and write it at the beginning of destination file
    unsigned char nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];
    randombytes_buf(nonce, sizeof nonce); /* 192-bits nonce */
    fwrite(nonce, 1, crypto_aead_xchacha20poly1305_ietf_NPUBBYTES, fp_d);
    
    uint32_t counter = 0;
    int rlen;
    unsigned long long clen;
    
    do {
        rlen = fread(read_buf, 1, sizeof read_buf, fp_s);
        unsigned long long mlen = rlen;
        crypto_aead_xchacha20poly1305_ietf_encrypt(cipher, &clen, (const unsigned char *)read_buf, rlen,
                                                   NULL, 0, NULL, nonce, (const unsigned char *)password);
        if (clen != mlen + crypto_aead_xchacha20poly1305_ietf_abytes()) {
            printf("\nEncryption Error!");
            fclose(fp_s);
            fclose(fp_d);
            return;
        } else {
            fwrite(cipher, 1, (size_t) clen, fp_d);
        }
    } while(!feof(fp_s));
    
    printf("\n%s encrypted and saved in %s", source, target);
    
    return;
}

void nes_decrypt(const char *source, const char *password) {
    
    return;
}

