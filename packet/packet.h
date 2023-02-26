#ifndef PACKET_H
#define PACKET_H

#include "../enet/include/enet.h"

char* GetTextPointerFromPacket(ENetPacket* packet);
int GetMessageTypeFromPacket(ENetPacket* packet);
void SendPacketPacket(ENetPacket* oldPacket, ENetPeer* peer);
void sendPacket(int val, char* packetText, ENetPeer* peer);
unsigned char* GetExtendedDataPointerFromTankPacket(unsigned char* a1);
void enet_peerSend(ENetPacket* packet, ENetPeer* peer);
ENetPacket* onPacketCreate(char* format, ...);

#endif // PACKET_H
