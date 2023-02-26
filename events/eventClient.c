#include <stdio.h>
#include <string.h>

#include "eventClient.h"
#include "eventServer.h"
#include "../enet/include/enet.h"
#include "../httpService.h"
#include "../utils/utils.h"
#include "../mainVar.h"
#include "../packet/packet.h"
#include "../proxyStruct.h"

void clientConnect() {
    if (isSendToServer) {
        printf("[Client] Client connected into proxy\n[Client] Connecting to subserver...\n");
        enet_host_destroy(realServer);

        realServer = enet_host_create(NULL, 1, 2, 0, 0);
        realServer->checksum = enet_crc32;
        realServer->usingNewPacket = 1;
        enet_host_compress_with_range_coder(realServer);
        enet_address_set_host(&realAddress, OnSendToServer.serverAddress);
        realAddress.port = OnSendToServer.port;
        realPeer = enet_host_connect(realServer, &realAddress, 2, 0);

        free(OnSendToServer.serverAddress);
        free(OnSendToServer.UUIDToken);

        isSendToServer = 0;
    } else {
        printf("[Client] Client connected into proxy\n[Client] Connecting to Growtopia Server...\n");

        info = HTTPSClient("2.17.198.162");

        char** arr = strsplit(info.buffer + (findStr(info.buffer, "server|") - 7), "\n", 0);
        char** server = strsplit(arr[0], "|", 0);
        char** port = strsplit(arr[1], "|", 0);
        char** meta = strsplit(arr[14], "|", 0);

        memset(&realAddress, 0, sizeof(ENetAddress));
        enet_address_set_host(&realAddress, server[1]);
        realAddress.port = atoi(port[1]);
        realPeer = enet_host_connect(realServer, &realAddress, 2, 0);
        if (currentInfo.meta) free(currentInfo.meta);
        asprintf(&currentInfo.meta, "%s", meta[1]);

        free(arr);
        free(server);
        free(port);
        free(meta);
    }
}

void clientReceive(ENetEvent event, ENetPeer* clientPeer, ENetPeer* serverPeer) {
    switch(GetMessageTypeFromPacket(event.packet)) {
        case 2: {
            char* packetText = GetTextPointerFromPacket(event.packet);

            if (includeStr(packetText, "requestedName|", event.packet->dataLength - 5)) {
                char** loginInfo = strsplit(packetText, "\n", 0);
                printf("[GTProxy] Spoofing Login info...\n");
                loginInfo[findArray(loginInfo, "meta|")] = CatchMessage("meta|%s", currentInfo.meta);
                loginInfo[findArray(loginInfo, "wk|")] = CatchMessage("wk|%s", currentInfo.wk);
                loginInfo[findArray(loginInfo, "rid|")] = CatchMessage("rid|%s", currentInfo.rid);
                loginInfo[findArray(loginInfo, "mac|")] = CatchMessage("mac|%s", currentInfo.mac);
                char* resultSpoofed = arrayJoin(loginInfo, "\n");
                sendPacket(2, resultSpoofed, serverPeer);
                printf("[Client] Spoofed Login info: %s\n", resultSpoofed);
                free(loginInfo);
            } else {
                printf("[Client] Packet 2: received packet text: %s\n", packetText);
                enet_peerSend(event.packet, serverPeer);
            }
            break;
        }
        case 3: {
            char* packetText = GetTextPointerFromPacket(event.packet);
            printf("[Client] Packet 3: received packet text: %s\n", packetText);
            if (isStr(packetText, "action|quit")) {
                isLoop = 0;
                doLoop = 1;
            }
            enet_peerSend(event.packet, serverPeer);
            break;
        }
        case 4: {
            switch(event.packet->data[4]) {
                case 26: {
                    enet_peerSend(event.packet, serverPeer);
                    enet_peer_disconnect_now(clientPeer, 0);
                    enet_peer_disconnect_now(serverPeer, 0);
                    break;
                }
                default: {
                    printf("[Client] TankUpdatePacket: Unknown packet tank type: %d\n", event.packet->data[4]);
                    enet_peerSend(event.packet, serverPeer);
                    break;
                }
            }
            break;
        }
        default: {
            printf("[Client] Unknown message type: %d\n", GetMessageTypeFromPacket(event.packet));
            enet_peerSend(event.packet, serverPeer);
            break;
        }
    }
}

void clientDisconnect() {
    printf("[Client] Client just disconnected from Proxy\n");
    isLoop = 0;
    doLoop = 1;
}
