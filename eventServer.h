#ifndef EVENTSERVER_H
#define EVENTSERVER_H

#include "../enet/include/enet.h"

void serverConnect();
void serverReceive(ENetEvent event, ENetPeer* clientPeer, ENetPeer* serverPeer);
void serverDisconnect();

#endif // EVENTSERVER_H
