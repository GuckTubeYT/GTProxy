#ifndef PROXYSTRUCT_H
#define PROXYSTRUCT_H

struct {
    char* meta;
    char* wk;
    char* rid;
    char* mac;
} currentInfo;

struct {
    char OnConsoleMessage;
    char OnSendToServer;
} OnPacket;

struct {
    char* message;
} OnConsoleMessage;

struct {
    char* serverAddress;
    int port;
    int token;
    int userID;
    char* UUIDToken;
} OnSendToServer;

#endif // PROXYSTRUCT_H
