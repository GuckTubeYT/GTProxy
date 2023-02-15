#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include "enet/include/enet.h"
#include "packet.h"
#include "proxyStruct.h"

char* GetTextPointerFromPacket(ENetPacket* packet) {
    char zero = 0;
	memcpy(packet->data + packet->dataLength - 1, &zero, 1);
	return (char*)(packet->data + 4);
}

int GetMessageTypeFromPacket(ENetPacket* packet) {
	if (packet->dataLength > 3u) return *(packet->data);
    return 0;
}

void SendPacketPacket(ENetPacket* oldPacket, ENetPeer* peer) {
    ENetPacket* packet = enet_packet_create(NULL, oldPacket->dataLength, oldPacket->flags);
    memcpy(packet->data, oldPacket->data, oldPacket->dataLength);
    enet_peer_send(peer, 0, packet);
}

void sendPacket(int val, char* packetText, ENetPeer* peer) {
    ENetPacket* packet = enet_packet_create(NULL, strlen(packetText) + 5, ENET_PACKET_FLAG_RELIABLE);
    memcpy(packet->data, &val, 4);
    memcpy(packet->data + 4, packetText, strlen(packetText));
    if (enet_peer_send(peer, 0, packet)) enet_packet_destroy(packet);
}

unsigned char* GetExtendedDataPointerFromTankPacket(unsigned char* a1) {
	return a1 + 56;
}

void enet_peerSend(ENetPacket* packet, ENetPeer* peer) {
    if (enet_peer_send(peer, 0, packet)) enet_packet_destroy(packet);
}

ENetPacket* onPacketCreate(char* format, ...) {
    va_list args;
    va_start(args, format);
    int totalData = 62;
    int memPos = 60;
    for (int a = 0; a < strlen(format); a++) {
        switch(format[a]) {
            case 's': {
                totalData += 4 + strlen(va_arg(args, char*));
                break;
            }
            case 'd': {
                va_arg(args, int);
                totalData += 4;
                break;
            }
            case 'D': {
                va_arg(args, int);
                totalData += 4;
                break;
            }
            case 'f': {
                va_arg(args, int);
                totalData += 4;
                break;
            }
        }
        totalData += 2;
    }

    va_end(args);

    unsigned char* resultData = malloc(totalData + 1);
    memset(resultData, 0, totalData);

    int four = 4;
    int one = 1;
    int negativeOne = -1;
    int eight = 8;

    memcpy(resultData, &four, 4);
    memcpy(resultData + 4, &one, 4);
    memcpy(resultData + 8, &negativeOne, 4);
    memcpy(resultData + 16, &eight, 4);
    totalData -= 61;
    memcpy(resultData + 56, &totalData, 4);
    totalData += 61;
    resultData[memPos++] = strlen(format);
    
    va_start(args, format);

    for (int a = 0; a < strlen(format); a++) {
        resultData[memPos++] = a;
        if (format[a] == 's') {
            resultData[memPos++] = 2;
            char* val = va_arg(args, char*);
            int strLen = strlen(val);
            memcpy(resultData + memPos, &strLen, 4);
            memPos += 4;
            memcpy(resultData + memPos, val, strLen);
            memPos += strLen;
        }
        else if (format[a] == 'd') {
            resultData[memPos++] = 9;
            int val = va_arg(args, int);
            memcpy(resultData + memPos, &val, 4);
            memPos += 4;
        }
        else if (format[a] == 'D') {
            resultData[memPos++] = 5;
            int val = va_arg(args, int);
            memcpy(resultData + memPos, &val, 4);
            memPos += 4;
        }
        else if (format[a] == 'f') {
            resultData[memPos++] = 1;
            float val = va_arg(args, double);
            memcpy(resultData + memPos, &val, 4);
            memPos += 4;
        }
    }

    va_end(args);
    
    resultData[totalData] = '\0';
    ENetPacket* packet = enet_packet_create(resultData, totalData, ENET_PACKET_FLAG_RELIABLE);
    free(resultData);
    
    return packet;
}