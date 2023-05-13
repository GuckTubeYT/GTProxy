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

void loadConfig() {
    FILE* fp = fopen("config.conf", "rb");
    if (!fp) {
        printf("[GTProxy] config.conf not found! Creating...\n");
        fp = fopen("config.conf", "wb");

        userConfig.usingServerData = 1;
        userConfig.serverDataIP = "2.17.198.162";
        userConfig.manualIP = "127.0.0.1";
        userConfig.manualPort = 17091;
        asprintf(&userConfig.manualMeta, "localhost");
        userConfig.usingNewPacket = 1;
        userConfig.httpsPort = 443;
        userConfig.skipGazette = 1;
        userConfig.isSpoofed = 0;

        fprintf(fp, "usingServerData=1\nserverDataIP=2.17.198.162\nmanualIP=127.0.0.1\nmanualPort=17091\nmanualMeta=localhost\nusingNewPacket=1\nhttpsPort=443\nskipGazette=1\nisSpoofed=0");
        fclose(fp);
        printf("[GTProxy] config.conf has been created!\n");
    } else {
        fseek(fp, 0, SEEK_END);
        int fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char* data = malloc(fsize + 1);
        fread(data, fsize, 1, fp);
        fclose(fp);
        data[fsize] = '\0';

        char** split = strsplit(data, "\n", 0);
        free(data);

        int a = 0;
        while(split[a]) {
            if (split[a][0] == '#') continue;

            char** split2 = strsplit(split[a++], "=", 0);
            if (isStr(split2[0], "usingServerData", 1)) {
                if (split2[1][0] == '1') userConfig.usingServerData = 1;
                else userConfig.usingServerData = 0;
            }

            if (isStr(split2[0], "serverDataIP", 1)) asprintf(&userConfig.serverDataIP, "%s", split2[1]);
            if (isStr(split2[0], "manualIP", 1)) asprintf(&userConfig.manualIP, "%s", split2[1]);
            if (isStr(split2[0], "manualPort", 1)) userConfig.manualPort = atoi(split2[1]);
            if (isStr(split2[0], "manualMeta", 1)) asprintf(&userConfig.manualMeta, "%s", split2[1]);

            if (isStr(split2[0], "usingNewPacket", 1)) {
                if (split2[1][0] == '1') userConfig.usingNewPacket = 1;
                else userConfig.usingNewPacket = 0;
            }
            if (isStr(split2[0], "httpsPort", 1)) userConfig.httpsPort = atoi(split2[1]);
            if (isStr(split2[0], "skipGazette", 1)) {
                if (split2[1][0] == '1') userConfig.skipGazette = 1;
                else userConfig.skipGazette = 0;
            }
            if (isStr(split2[0], "isSpoofed", 1)) {
                if (split2[1][0] == '1') userConfig.isSpoofed = 1;
                else userConfig.isSpoofed = 0;
            }

            free(split2);
        }

        free(split);
    }

}

int main() {
    if (!isLoop) {
        loadConfig();
    }

    isLoop = 1;
    doLoop = 0;
    srand(time(NULL));

    memset(&currentInfo, 0, sizeof(currentInfo));

    currentInfo.wk = generateHex(16);
    currentInfo.rid = generateHex(16);
    currentInfo.deviceID = generateHex(16);
    currentInfo.mac = generateHex(0);

    memset(&OnPacket, 0, sizeof(OnPacket));

    enet_initialize();

    memset(&proxyAddress, 0, sizeof(ENetAddress));
    proxyAddress.host = 0; // 0.0.0.0
    proxyAddress.port = 17091;
    proxyServer = enet_host_create(&proxyAddress, 1024, 10, 0, 0);
    proxyServer->checksum = enet_crc32;
    enet_host_compress_with_range_coder(proxyServer);

    realServer = enet_host_create(NULL, 1, 2, 0, 0);
    realServer->checksum = enet_crc32;
    realServer->usingNewPacket = userConfig.usingNewPacket;
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

    if (currentInfo.isMetaMalloc) {
        free(currentInfo.meta);
        currentInfo.isMetaMalloc = 0;
    }

    free(currentInfo.wk);
    free(currentInfo.rid);
    free(currentInfo.deviceID);
    free(currentInfo.mac);

    if (doLoop) {
        currentInfo.isLogin = 0;
        isLoop = 1;
        doLoop = 0;
        main();
    }
    return 0;
}
