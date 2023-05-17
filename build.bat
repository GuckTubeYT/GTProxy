@echo off
gcc *.c utils/*.c packet/*.c events/*.c enet/*.c tlse/tlse.c sha256/sha256.c -DTLS_AMALGAMATION -o proxy -lws2_32 -lwinmm -lpthread
proxy
