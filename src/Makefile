PLAT = linux
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    PLAT = linux
endif
ifeq ($(UNAME_S),Darwin)
    PLAT = macosx
endif

CC=gcc
LDFLAGS=-I../libsodium/include
ifeq ($(PLAT), linux)
    LDFLAGS += -Wl,-rpath=../libsodium/libs/linux
endif

#Despite the linker throwing up the following error:
# ld: warning: option -s is obsolete and being ignored
# the presence of -s option affects the code size!! 
CFLAGS=-Wall -Os -s
#CFLAGS=-Wall -g

all: nescrypt

nescrypt: nescrypt.c


nescrypt:
	$(CC) $(CFLAGS)  $(LDFLAGS) -o nescrypt nescrypt.c ../libsodium/libs/$(PLAT)/libsodium.a

.PHONY: clean
clean :
	rm client server 

