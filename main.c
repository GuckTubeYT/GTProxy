#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif // __WIN32
#include <time.h>
#include <stdint.h>

#include "enet/include/enet.h"
#include "httpService.h"
#include "utils/utils.h"
#include "packet/packet.h"
#include "proxyStruct.h"
#include "events/eventClient.h"
#include "events/eventServer.h"
#include "mainVar.h"
#include "proxyStruct.h"

int main() {
    isLoop = 1;
    doLoop = 0;
    srand(time(NULL));

    memset(&currentInfo, 0, sizeof(currentInfo));
    memset(&OnPacket, 0, sizeof(OnPacket));

    currentInfo.wk = generateHex(32);
    currentInfo.rid = generateHex(32);
    currentInfo.mac = generateHex(0);
    currentInfo.gid = generateGID();

    enet_initialize();

    memset(&proxyAddress, 0, sizeof(ENetAddress));
    proxyAddress.host = 0; // 0.0.0.0
    proxyAddress.port = 17091;
    proxyServer = enet_host_create(&proxyAddress, 1024, 10, 0, 0);
    proxyServer->checksum = enet_crc32;
    enet_host_compress_with_range_coder(proxyServer);

    realServer = enet_host_create(NULL, 1, 2, 0, 0);
    realServer->checksum = enet_crc32;
    realServer->usingNewPacket = 1;
    enet_host_compress_with_range_coder(realServer);

    if (!HTTPAlreadyOn) {
        #ifdef __WIN32
        HTTPThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HTTPSServer, NULL, 0, NULL);
        #else
        pthread_create(&HTTPThread, NULL, HTTPSServer, NULL);
        #endif // __WIN32
        HTTPAlreadyOn = 1;
    }

    while(isLoop) {
        ENetEvent proxyEvent;
        ENetEvent realEvent;
        while(enet_host_service(proxyServer, &proxyEvent, 5) > 0) {
            proxyPeer = proxyEvent.peer;
            switch(proxyEvent.type) {
                case ENET_EVENT_TYPE_CONNECT: {
                    clientConnect();
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    clientReceive(proxyEvent, proxyPeer, realPeer);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    clientDisconnect();
                    break;
                }
                case ENET_EVENT_TYPE_NONE: break;
            }
        }

        while(enet_host_service(realServer, &realEvent, 5) > 0) {
            switch(realEvent.type) {
                case ENET_EVENT_TYPE_CONNECT: {
                    serverConnect();
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    serverReceive(realEvent, proxyPeer, realPeer);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    serverDisconnect();
                    break;
                }
                case ENET_EVENT_TYPE_NONE: break;
            }
        }
    }

    if (realPeer) enet_peer_disconnect_now(realPeer, 0);
    enet_peer_disconnect_now(proxyPeer, 0);

    enet_host_destroy(realServer);
    enet_host_destroy(proxyServer);

    enet_deinitialize();

    free(currentInfo.wk);
    free(currentInfo.rid);
    free(currentInfo.mac);
    free(currentInfo.gid);
    if (currentInfo.meta) free(currentInfo.meta);

    if (doLoop) {
        isLoop = 1;
        doLoop = 0;
        main();
    }
    return 0;
}
