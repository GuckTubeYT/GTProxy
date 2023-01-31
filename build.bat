@echo off
gcc *.c enet/*.c tlse/tlse.c -DTLS_AMALGAMATION -o proxy -lws2_32 -lwinmm -lpthread
proxy