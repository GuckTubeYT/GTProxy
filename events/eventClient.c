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
                loginInfo[findArray(loginInfo, "platformID|")] = "platformID|4";
                loginInfo[findArray(loginInfo, "category|")] = "category|_0";
                loginInfo[findArray(loginInfo, "game_version|")] = "game_version|4.23";
                loginInfo[findArray(loginInfo, "cbits|")] = "cbits|1040";
                if (findArray(loginInfo, "fz|") != -1) loginInfo[findArray(loginInfo, "fz|")] = "";
                if (findArray(loginInfo, "klv|") != -1) loginInfo[findArray(loginInfo, "klv|")] = "";
                if (findArray(loginInfo, "gid|") != -1) loginInfo[findArray(loginInfo, "gid|")] = "";
                if (findArray(loginInfo, "tr|") != -1) loginInfo[findArray(loginInfo, "tr|")] = "";
                char* resultSpoofed;
                asprintf(&resultSpoofed, "%sgid|%s\ntr|4322", arrayJoin(loginInfo, "\n", 1), currentInfo.gid);
                sendPacket(2, resultSpoofed, serverPeer);
                printf("[Client] Spoofed Login info: %s\n", resultSpoofed);
                free(loginInfo);
                free(resultSpoofed);
                break;
            }

            printf("[Client] Packet 2: received packet text: %s\n", packetText);

            if ((packetText + 19)[0] == '/') {
                char** command = strsplit(packetText + 19, " ", 0);
                if (isStr(command[0], "/proxyhelp")) {
                    sendPacket(3, "action|log\nmsg|>> Commands: /helloworld /testarg <your arg> /testdialog /warp <name world> /netid", clientPeer);
                }
                else if (isStr(command[0], "/helloworld")) {
                    sendPacket(3, "action|log\nmsg|`2Hello World", clientPeer);
                }
                else if (isStr(command[0], "/netid")) {
                    enet_peerSend(onPacketCreate("ss", "OnConsoleMessage", CatchMessage("My netID is %s", OnSpawn.LocalNetid)), clientPeer);
                }
                else if (isStr(command[0], "/testarg")) {
                    if (!command[1]) {
                        sendPacket(3, "action|log\nmsg|Please input argument", clientPeer);
                        free(command); // prevent memleak
                        break;
                    }
                    sendPacket(3, CatchMessage("action|log\nmsg|%s", command[1]), clientPeer);
                }
                else if (isStr(command[0], "/testdialog")) {

                    enet_peerSend(onPacketCreate("ss", "OnDialogRequest","set_default_color|`o\nadd_label_with_icon|big|`wTest Dialog!``|left|758|\nadd_textbox|Is It Working?|left|\nadd_text_input|yesno||yes|5|\nembed_data|testembed|4\nadd_textbox|`4Warning:``Dont Forget To Star Repo!|left|\nend_dialog|test_dialog|Cancel|OK|"), clientPeer);
                }
                else if (isStr(command[0], "/warp")) {
                    if (!command[1]) {
                        sendPacket(3, "action|log\nmsg|Please input world name", clientPeer);
                        free(command); // prevent memleak
                        break;
                    }
                    sendPacket(3, CatchMessage("action|join_request\nname|%s\ninvitedWorld|0", command[1]), serverPeer);
                }
                else enet_peerSend(event.packet, serverPeer);

                free(command); // prevent memleak
                break;
            }

            enet_peerSend(event.packet, serverPeer);
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
