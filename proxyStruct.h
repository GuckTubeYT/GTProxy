#ifndef PROXYSTRUCT_H
#define PROXYSTRUCT_H

struct currentInfo_ {
    char* meta;
    char* wk;
    char* rid;
    char* mac;
};

struct OnPacket_ {
    char OnConsoleMessage;
    char OnSendToServer;
};

struct OnConsoleMessage_ {
    char* message;
};

struct OnSendToServer_ {
    char* serverAddress;
    int port;
    int token;
    int userID;
    char* UUIDToken;
    int unkInt;
    char* rawSplit;
};

struct OnSpawn_ {
    int LocalNetid;
};

extern struct currentInfo_ currentInfo;
extern struct OnPacket_ OnPacket;
extern struct OnConsoleMessage_ OnConsoleMessage;
extern struct OnSendToServer_ OnSendToServer;
extern struct OnSpawn_ OnSpawn;

extern char isSendToServer;

#endif // PROXYSTRUCT_H
