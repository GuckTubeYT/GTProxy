#include "enet/include/enet.h"

char* GetTextPointerFromPacket(ENetPacket* packet);
int GetMessageTypeFromPacket(ENetPacket* packet);
void SendPacketPacket(ENetPacket* oldPacket, ENetPeer* peer);
void sendPacket(int val, char* packetText, ENetPeer* peer);
unsigned char* GetExtendedDataPointerFromTankPacket(unsigned char* a1);
ENetPacket* onPacketCreate(char* format, ...);