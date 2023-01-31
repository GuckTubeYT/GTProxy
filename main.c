#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>

#include "enet/include/enet.h"
#include "httpService.h"
#include "utils.h"
#include "packet.h"
#include "proxyStruct.h"

pthread_t HTTPThread;
struct HTTPInfo info;

ENetHost* proxyServer;
ENetHost* realServer;

ENetPeer* realPeer;
ENetPeer* proxyPeer;

char isLoop = 1;
char doLoop = 0;
char HTTPAlreadyOn = 0;

int main() {
    srand(time(NULL));

    memset(&currentInfo, 0, sizeof(currentInfo));

    currentInfo.wk = generateHex(32);
    currentInfo.rid = generateHex(32);
    currentInfo.mac = generateHex(0);

    enet_initialize();

    ENetAddress proxyAddress;
    proxyAddress.host = 0; // 0.0.0.0
    proxyAddress.port = 17091;
    proxyServer = enet_host_create(&proxyAddress, 1024, 10, 0, 0);
    proxyServer->checksum = enet_crc32;
    enet_host_compress_with_range_coder(proxyServer);

    ENetAddress realAddress;
    realServer = enet_host_create(NULL, 1, 2, 0, 0);
    realServer->checksum = enet_crc32;
    realServer->usingNewPacket = 1;
    enet_host_compress_with_range_coder(realServer);

    if (!HTTPAlreadyOn) {
        pthread_create(&HTTPThread, NULL, HTTPSServer, NULL);
        HTTPAlreadyOn = 1;
    }

    while(isLoop) {
        ENetEvent proxyEvent;
        ENetEvent realEvent;

        while(enet_host_service(proxyServer, &proxyEvent, 10) > 0) {
            proxyPeer = proxyEvent.peer;
            switch(proxyEvent.type) {
                case ENET_EVENT_TYPE_CONNECT: {
                    if (OnPacket.OnSendToServer) {
                        enet_peer_disconnect_now(realPeer, 0);
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

                        OnPacket.OnSendToServer = 0;
                    }
                    else {
                        info = HTTPSClient("2.17.198.162");

                        char** arr = strsplit(info.buffer, "\n", 0);
                        char** server = strsplit(arr[0], "|", 0);
                        char** port = strsplit(arr[1], "|", 0);
                        char** meta = strsplit(arr[14], "|", 0);

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

                    printf("[Client] Client connected to Proxy!\n");
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    switch(GetMessageTypeFromPacket(proxyEvent.packet)) {
                        case 2: {
                            char* packetText = GetTextPointerFromPacket(proxyEvent.packet);
                            printf("[Client] Packet 2: received text: %s\n", packetText);
                            if (strstr(packetText, "requestedName|")) {
                                char** loginInfo = strsplit(packetText, "\n", 0);
                                loginInfo[findArray(loginInfo, "meta|")] = CatchMessage("meta|%s", currentInfo.meta);
                                loginInfo[findArray(loginInfo, "wk|")] = CatchMessage("wk|%s", currentInfo.wk);
                                loginInfo[findArray(loginInfo, "rid|")] = CatchMessage("rid|%s", currentInfo.rid);
                                loginInfo[findArray(loginInfo, "mac|")] = CatchMessage("mac|%s", currentInfo.mac);
                                sendPacket(2, arrayJoin(loginInfo, "\n"), realPeer);
                                free(loginInfo);
                            } else SendPacketPacket(proxyEvent.packet, realPeer);
                            break;
                        }
                        case 3: {
                            char* packetText = GetTextPointerFromPacket(proxyEvent.packet);
                            if (strstr(packetText, "action|quit")) {
                                isLoop = 0;
                                doLoop = 1;
                            }
                            SendPacketPacket(proxyEvent.packet, realPeer);
                            printf("[Client] Packet 3: received text: %s\n", packetText);
                            break;
                        }
                        case 4: {
                            switch(proxyEvent.packet->data[4]) {
                                case 26: {
                                    SendPacketPacket(proxyEvent.packet, realPeer);
                                    enet_peer_disconnect_now(proxyPeer, 0);
                                    enet_peer_disconnect_now(realPeer, 0);
                                    break;
                                }
                                default: {
                                    printf("[Client] Tank Packet: Unknown packet tank type: %d\n", proxyEvent.packet->data[4]);
                                    SendPacketPacket(proxyEvent.packet, realPeer);
                                    break;
                                }
                            }
                            break;
                        }
                        default: {
                            printf("[Client] Unknown message type: %d\n", GetMessageTypeFromPacket(proxyEvent.packet));
                            SendPacketPacket(proxyEvent.packet, realPeer);
                            break;
                        }
                    }
                    enet_packet_destroy(proxyEvent.packet);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    printf("[Client] disconnected!\n");
                    isLoop = 0;
                    doLoop = 1;
                    break;
                }
                case ENET_EVENT_TYPE_NONE: break;
            }
        }

        while(enet_host_service(realServer, &realEvent, 10) > 0) {
            switch(realEvent.type) {
                case ENET_EVENT_TYPE_CONNECT: {
                    printf("[Server] Proxy Connected to Server!\n");
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE: {
                    switch(GetMessageTypeFromPacket(realEvent.packet)) {
                        case 1: {
                            printf("[Server] Server just send Hello Packet\n");
                            SendPacketPacket(realEvent.packet, proxyPeer);
                            break;
                        }
                        case 3: {
                            char* packetText = GetTextPointerFromPacket(realEvent.packet);
                            printf("[Server] Packet 3: received text: %s\n", packetText);
                            SendPacketPacket(realEvent.packet, proxyPeer);
                            break;
                        }
                        case 4: {
                            switch(realEvent.packet->data[4]) {
                                case 1: {
                                    unsigned char* packetTank = GetExtendedDataPointerFromTankPacket(realEvent.packet->data + 4);
		                            unsigned char count = packetTank[0]; packetTank++;

                                    for (unsigned char a = 0; a < count; a++) {
                                        unsigned char index = packetTank[0]; packetTank++;
			                            unsigned char type = packetTank[0]; packetTank++;

                                        switch(type) {
                                            case 1: {
                                                float value;
                                                memcpy(&value, packetTank, 4); packetTank += 4;
                                                printf("[Server] PacketTank Variable: float found at %d\n %f\n", index, value);
                                                break;
                                            }
                                            case 2: {
                                                int strLen;
                                                memcpy(&strLen, packetTank, 4); packetTank += 4;

                                                static char* value;
                                                value = malloc(strLen);

                                                memcpy(value, packetTank, strLen); packetTank += strLen;
                                                value[strLen] = '\0';

                                                if (strstr(value, "OnSendToServer") && index == 0) {
                                                    memset(&OnSendToServer, 0, sizeof(OnSendToServer));
                                                    OnPacket.OnSendToServer = 1;
                                                }
                                                else if (strstr(value, "OnConsoleMessage") && index == 0) {
                                                    memset(&OnConsoleMessage, 0, sizeof(OnConsoleMessage));
                                                    OnPacket.OnConsoleMessage = 1;
                                                }
                                                else if (OnPacket.OnSendToServer && index == 4) {
                                                    char** toSplit = strsplit(value, "|", 0);
                                                    asprintf(&OnSendToServer.serverAddress, "%s", toSplit[0]);
                                                    asprintf(&OnSendToServer.UUIDToken, "%s", toSplit[2]);
                                                    free(toSplit);
                                                }

                                                printf("[Server] PacketTank Variable: String found at %d: %s\n", index, value);
                                                break;
                                            }
                                            case 3: {
                                                float value1;
                                                float value2;
                                                memcpy(&value1, packetTank, 4); packetTank += 4;
                                                memcpy(&value2, packetTank, 4); packetTank += 4;

                                                printf("[Server] PacketTank Variable: Vector found at %d: %f, %f\n", index, value1, value2);
                                                break;
                                            }
                                            case 5: {
                                                int value;
                                                memcpy(&value, packetTank, 4); packetTank += 4;

                                                printf("[Server] PacketTank Variable: Integer X found at %d: %d\n", index, value);
                                                break;
                                            }
                                            case 9: {
                                                int value;
                                                memcpy(&value, packetTank, 4); packetTank += 4;

                                                if (OnPacket.OnSendToServer && index == 1) OnSendToServer.port = value;
                                                else if (OnPacket.OnSendToServer && index == 2) OnSendToServer.token = value;
                                                else if (OnPacket.OnSendToServer && index == 3) OnSendToServer.userID = value;
                                                printf("[Server] PacketTank Variable: Integer found at %d: %d\n", index, value);
                                                break;
                                            }
                                            default: {
                                                printf("[Server] PacketTank Variable: Unknown variable type: %d\n", type);
                                                break;
                                            }
                                        }
                                    }
                                    if (OnPacket.OnSendToServer) {
                                        char* tempString;
                                        asprintf(&tempString, "127.0.0.1|0|%s", OnSendToServer.UUIDToken);
                                        SendPacketPacket(onPacketCreate("sdddsd", "OnSendToServer", 17091, OnSendToServer.token, OnSendToServer.userID, tempString, 1), proxyPeer);
                                        free(tempString);
                                    } else SendPacketPacket(realEvent.packet, proxyPeer);
                                    break;
                                }

                                default: {
                                    printf("[Server] Packet 4: Unknown packet tank type: %d\n", realEvent.packet->data[4]);
                                    SendPacketPacket(realEvent.packet, proxyPeer);
                                    break;
                                }
                            }
                            break;
                        }
                        case 6: {
                            char* packetText = GetTextPointerFromPacket(realEvent.packet);
                            printf("[Server] Packet 6: received text: %s\n", packetText);
                            SendPacketPacket(realEvent.packet, proxyPeer);
                            break;
                        }
                        default: {
                            printf("[Server] Unknown message type: %d\n", GetMessageTypeFromPacket(realEvent.packet));
                            SendPacketPacket(realEvent.packet, proxyPeer);
                            break;
                        }
                    }
                    enet_packet_destroy(realEvent.packet);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    printf("[Server] Disconnected!\n");
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

    if (doLoop) {
        isLoop = 1;
        doLoop = 0;
        main();
    }
    return 0;
}
