#ifndef MAINVAR_H
#define MAINVAR_H

#include "enet/include/enet.h"

#ifdef __WIN32
#include <windows.h>
HANDLE HTTPThread;
#else
#include <pthread.h>
pthread_t HTTPThread;
#endif // __WIN32

struct HTTPInfo info;

ENetHost* realServer;
ENetPeer* realPeer;
ENetAddress realAddress;

ENetHost* proxyServer;
ENetPeer* proxyPeer;
ENetAddress proxyAddress;

char isLoop;
char doLoop;
char HTTPAlreadyOn;

#endif // MAINVAR_H