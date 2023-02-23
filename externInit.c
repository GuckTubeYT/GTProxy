#include "proxyStruct.h"
#include "enet/include/enet.h"
#include "httpService.h"

#ifdef __WIN32
#include <windows.h>
HANDLE HTTPThread;
#else
#include <pthread.h>
pthread_t HTTPThread;
#endif // __WIN32

char isLoop;
char doLoop;
char isSendToServer;
char HTTPAlreadyOn;

struct currentInfo_ currentInfo;
struct OnPacket_ OnPacket;
struct OnConsoleMessage_ OnConsoleMessage;
struct OnSendToServer_ OnSendToServer;

struct HTTPInfo info;
ENetHost* realServer;
ENetPeer* realPeer;
ENetAddress realAddress;
ENetHost* proxyServer;
ENetPeer* proxyPeer;
ENetAddress proxyAddress;
