#ifndef EVENTCLIENT_H
#define EVENTCLIENT_H

#include "../enet/include/enet.h"

void clientConnect();
void clientReceive(ENetEvent event, ENetPeer* clientPeer, ENetPeer* serverPeer);
void clientDisconnect();

#endif // EVENTCLIENT_H
