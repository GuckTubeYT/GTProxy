#ifndef MAINVAR_H
#define MAINVAR_H

#include "enet/include/enet.h"

#ifdef __WIN32
#include <windows.h>
extern HANDLE HTTPThread;
#else
#include <pthread.h>
extern pthread_t HTTPThread;
#endif // __WIN32

extern struct HTTPInfo info;

extern ENetHost* realServer;
extern ENetPeer* realPeer;
extern ENetAddress realAddress;

extern ENetHost* proxyServer;
extern ENetPeer* proxyPeer;
extern ENetAddress proxyAddress;

extern char isLoop;
extern char doLoop;
extern char HTTPAlreadyOn;

#endif // MAINVAR_H
