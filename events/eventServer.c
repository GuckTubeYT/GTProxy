#include <stdio.h>
#include <string.h>

#include "eventServer.h"
#include "../enet/include/enet.h"
#include "../packet/packet.h"
#include "../utils/utils.h"
#include "../proxyStruct.h"

void serverConnect() {
    printf("[Server] Proxy connected into Server\n");
}

void serverReceive(ENetEvent event, ENetPeer* clientPeer, ENetPeer* serverPeer) {
    switch(GetMessageTypeFromPacket(event.packet)) {
        case 1: {
            printf("[Server] Server just send Hello Packet\n[Client] Sending login info\n");
            SendPacketPacket(event.packet, clientPeer);
            break;
        }
        case 2: {
            char* packetText = GetTextPointerFromPacket(event.packet);
            printf("[Server] Packet 2: received packet text: %s\n", packetText);
            enet_peerSend(event.packet, clientPeer);
            break;
        }
        case 3: {
            char* packetText = GetTextPointerFromPacket(event.packet);
            printf("[Server] Packet 3: received packet text: %s\n", packetText);
            enet_peerSend(event.packet, clientPeer);
            break;
        }
        case 4: {
            printf("[Server] Packet 4: Received packet tank: %d\n", event.packet->data[4]);
            switch(event.packet->data[4]) {
                case 1: {
                    unsigned char* packetTank = GetExtendedDataPointerFromTankPacket(event.packet->data + 4);
                    unsigned char count = packetTank++[0];

                    for (unsigned char a = 0; a < count; a++) {
                        unsigned char index = packetTank++[0];
                        unsigned char type = packetTank++[0];

                        switch(type) {
                            case 1: {
                                float value;
                                memcpy(&value, packetTank, 4); packetTank += 4;
                                printf("[Server] TankUpdatePacket Variable: float found at %d: %f\n", index, value);
                                break;
                            }
                            case 2: {
                                int strLen;
                                memcpy(&strLen, packetTank, 4); packetTank += 4;

                                char* value = malloc(strLen + 1);
                                memcpy(value, packetTank, strLen); packetTank += strLen;
                                value[strLen] = '\0';

                                switch(index) {
                                    case 0: {
                                        if (isStr(value, "OnSendToServer")) {
                                            memset(&OnSendToServer, 0, sizeof(OnSendToServer));
                                            OnPacket.OnSendToServer = 1;
                                        }
                                        else if (isStr(value, "OnConsoleMessage")) {
                                            memset(&OnConsoleMessage, 0, sizeof(OnConsoleMessage));
                                            OnPacket.OnConsoleMessage = 1;
                                        }
                                        break;
                                    }
                                    case 4: {
                                        if (OnPacket.OnSendToServer) {
                                            asprintf(&OnSendToServer.rawSplit, "%s", value);
                                            char** toSplit = strsplit(value, "|", 0);
                                            asprintf(&OnSendToServer.serverAddress, "%s", toSplit[0]);
                                            asprintf(&OnSendToServer.UUIDToken, "%s", toSplit[2]);
                                            free(toSplit);
                                        }
                                        break;
                                    }
                                }

                                printf("[Server] TankUpdatePacket Variable: string found at %d: %s\n", index, value);
                                free(value);
                                break;
                            }
                            case 3: {
                                float value1;
                                float value2;

                                memcpy(&value1, packetTank, 4); packetTank += 4;
                                memcpy(&value2, packetTank, 4); packetTank += 4;

                                printf("[Server] TankUpdatePacket Variable: vector found at %d: %f, %f\n", index, value1, value2);
                                break;
                            }
                            case 5: {
                                int value;
                                memcpy(&value, packetTank, 4); packetTank += 4;

                                printf("[Server] TankUpdatePacket Variable: integer X found at %d: %d\n", index, value);
                                break;
                            }
                            case 9: {
                                int value;
                                memcpy(&value, packetTank, 4); packetTank += 4;

                                switch(index) {
                                    case 1: {
                                        if (OnPacket.OnSendToServer) OnSendToServer.port = value;
                                        break;
                                    }
                                    case 2: {
                                        if (OnPacket.OnSendToServer) OnSendToServer.token = value;
                                        break;
                                    }
                                    case 3: {
                                        if (OnPacket.OnSendToServer) OnSendToServer.userID = value;
                                        break;
                                    }
                                    case 5: {
                                        if (OnPacket.OnSendToServer) OnSendToServer.unkInt = value;
                                        break;
                                    }
                                }

                                printf("[Server] TankUpdatePacket Variable: integer found at %d: %d\n", index, value);
                                break;
                            }
                            default: {
                                printf("[Server] TankUpdatePacket Variable: unknown variable type: %d\n", type);
                                break;
                            }
                        }
                    }
                    if (OnPacket.OnSendToServer) {
                        char** splitString = strsplit(OnSendToServer.rawSplit, "|", 0);
                        splitString[0] = "127.0.0.1";
                        enet_peerSend(onPacketCreate("sdddsd", "OnSendToServer", 17091, OnSendToServer.token, OnSendToServer.userID, arrayJoin(splitString, "|"), OnSendToServer.unkInt), clientPeer);
                        free(splitString);
                        free(OnSendToServer.rawSplit);
                        OnPacket.OnSendToServer = 0;
                        isSendToServer = 1;
                    } else enet_peerSend(event.packet, clientPeer);
                    break;
                }
                default: {
                    enet_peerSend(event.packet, clientPeer);
                    break;
                }
            }
            break;
        }
        default: {
            printf("[Server] Unknown message type: %d\n", GetMessageTypeFromPacket(event.packet));
            enet_peerSend(event.packet, clientPeer);
            break;
        }
    }
}

void serverDisconnect() {
    printf("[Server] Proxy just disconnected from Server\n");
}
