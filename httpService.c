#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#ifdef _WIN32
    #include <winsock2.h>
    #define socklen_t int
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
#endif
#include <string.h>
#include <unistd.h>

#include "httpService.h"
#include "tlse/tlse.h"
#include "utils/utils.h"
#include "proxyStruct.h"

const unsigned char* certPem = "-----BEGIN CERTIFICATE-----\n\
MIIDeDCCAmCgAwIBAgIULbUEh/rroH5AIbcdBbMNOGt3uiQwDQYJKoZIhvcNAQEL\n\
BQAwEDEOMAwGA1UEAwwFZ2l0dT8wHhcNMjMwMTE3MDI1MjAzWhcNMjQwMTE3MDI1\n\
MjAzWjAQMQ4wDAYDVQQDDAVnaXR1PzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\n\
AQoCggEBAMrQhMRtb1v9k7TQ9UkYRMFnvrjcDsfSxPqqNEGmK4lFX2X19o38vMUo\n\
+68dmBx1j6AAij0grg1sC7K1bZgRkbiOM8DRex3XhT6YO/SsedKOd88QueWbZUnp\n\
qIOVHiCipMo27AlBR1psUOIhsMiD2MxCdpOeg738NYEii2hhKDW/UXa21nl2DCTQ\n\
iqgn+6AXM9gVtjsih2Ms2JXJMpGG1Upx3MPjhl7Us8p2K2oVQ3mnYpjDB63mG6Y8\n\
2IzDX2vLopkP64rMXcSkGMeKMIXRdOeVCK30HqsfLmPSfyA2ye/x/YqfoIS6B/HR\n\
AKKlwtO1FsduSq2p9G8d2FyaIqcD5tcCAwEAAaOByTCBxjAdBgNVHQ4EFgQUZ1Vl\n\
461lWuwmgY+Z0/YdGUsDviowHwYDVR0jBBgwFoAUZ1Vl461lWuwmgY+Z0/YdGUsD\n\
viowDwYDVR0TAQH/BAUwAwEB/zBRBgNVHREESjBIgg5ncm93dG9waWExLmNvbYIS\n\
d3d3Lmdyb3d0b3BpYTEuY29tgg5ncm93dG9waWEyLmNvbYISd3d3Lmdyb3d0b3Bp\n\
YTIuY29tMAsGA1UdDwQEAwIHgDATBgNVHSUEDDAKBggrBgEFBQcDATANBgkqhkiG\n\
9w0BAQsFAAOCAQEAO9qTutDwFG6IXKWfh2lzZvIoMK1A0VPZ+x5AP3vBMZJUa+Ae\n\
LGhvL4i2YaxlzfNndFloymJ5ubIZ1n2NfkeJ+NpcI40Mf02J5H+ltXBlbKsEaA2K\n\
HOKS6xRtbogJH0JiIG300Nx1T4dXtUSZYMS4Ti5TTrcztTmGK3o5ZLtt8gucpk+G\n\
O0aIrdPlAyiXXkLF8HwwLTEJ9P4jCdqAacRCW0Pp8op0i1CeBuMVHTGKW4jLav4c\n\
knSSDgPZplM3Rc5yOMrN0Ff+TgyIKnjxRUlj+SmraCIX9qGfSrd7LMVRAWvaPkGL\n\
b5uJ4NVYY27+jutHB4TZlB4belxlqNrDECCQCA==\n\
-----END CERTIFICATE-----";

const unsigned char* keyPem = "-----BEGIN PRIVATE KEY-----\n\
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDK0ITEbW9b/ZO0\n\
0PVJGETBZ7643A7H0sT6qjRBpiuJRV9l9faN/LzFKPuvHZgcdY+gAIo9IK4NbAuy\n\
tW2YEZG4jjPA0Xsd14U+mDv0rHnSjnfPELnlm2VJ6aiDlR4goqTKNuwJQUdabFDi\n\
IbDIg9jMQnaTnoO9/DWBIotoYSg1v1F2ttZ5dgwk0IqoJ/ugFzPYFbY7IodjLNiV\n\
yTKRhtVKcdzD44Ze1LPKditqFUN5p2KYwwet5humPNiMw19ry6KZD+uKzF3EpBjH\n\
ijCF0XTnlQit9B6rHy5j0n8gNsnv8f2Kn6CEugfx0QCipcLTtRbHbkqtqfRvHdhc\n\
miKnA+bXAgMBAAECggEBAKUAO+OqvLx0cuTaU5QiIF3Qz4OJ1KSRPxxHuLfoPucw\n\
/0nub6ZYhiNJEmoHg6czpaOgjNbqbXASBPphTEY5lPo3BrfKNYWzv32LUMvgPkQl\n\
ECfwDa1VXXRimmxitt4KNFMnl6R37VsNYEh47GBVk92p/NpgTgIU3FqxBgXndVUz\n\
WpG0KxMcQ32ahIMvoVRZ3jp+DAMTo2h2HEOFC8lciS7bKo5YfXEr3fMUIa57SK0p\n\
GI81hFBJ1kbHS/RVLZpRxc4CGzUTR3w8eTDzWzHqooV6t31P6t7mskY40cuK3MCV\n\
2fpYUmxZ8KDYG1hCy/jeQeLN0jvT5CWSkn1leTWYszECgYEA/Jv7wxGi2F6gQvx4\n\
/NKmrTZxXxJ7+OXWDV9/b0YkW6L83LL6OP+TpsbdMhM7J3nTrhdAfqxvOXVmHeom\n\
hZK5a94tpDvlq5+1HyGzN1+n3S53JJAQFZdyzj+eOtq4j7xO7JBC4Z9NWz6pqtsD\n\
xqz05ooxnF0+R+U6EUAm614RfbsCgYEAzYluJPQJwK0WQYRF7dWicqfrvAyy2vwW\n\
k5+a3Iw7BkK7ZC1b+rt7em7XPoYy1VSZ6sKReEJqr3Jjp9gpb4nGSL7vyxwq3PmT\n\
xVqb7NeBz6Ynq/DCuPxjg4XvkYYgSDlxmYwo2UMbeA3AjkpQ6GGCSREdHwrqS82R\n\
CkNthzkJG5UCgYEA+EPtacg5feFyLKOp3QDDI1Vwkvm9sd9hQn9q5VfQf+duERIt\n\
7jrxSGYGGyLfbmrDakQb9ONP5O7Gz/vCpGCRr7wKq3DaxpQOFGywkGmIt07ldZ/I\n\
zw+rm5zWzA0OeuWdw838jXZdNqIdangZ+/ccMj/7hiv1+/8D1NRx67zl/+UCgYAU\n\
Nasq3wQGef5yOrtVg2gRtgkr2GPNvHPL2Cm1/ePf3bfnRb6BjOW7SCLJ9xAQgd0Q\n\
hEkhdgZfcfPykQKiAP4KMRwK8J4WVEP/Z8VlKFuPO1szQN0kq0xtpkHEIVgTQfK1\n\
kl6B3mNZDqEcXJpP/yZ8YL5oSv52raPRSof+f9ExcQKBgCqVb/IS4nsvmgNzqfqg\n\
VUdxsR9RmUSmrPeVJUOu0XzYR/RLBXM19XiOT78YRzAUWJjFZm7SoaZQcwLgGSw6\n\
T0DtIPCMA4ld9UYknjMRSLBq1qt3u/DhkgWPvdARN9S4sqF+cQcNbN4CofpI5bdq\n\
dYyMOVQz/HrNS/RTfTU54JbU\n\
-----END PRIVATE KEY-----";

int send_pending(int client_sock, struct TLSContext *context) {
    unsigned int out_buffer_len = 0;
    const unsigned char *out_buffer = tls_get_write_buffer(context, &out_buffer_len);
    unsigned int out_buffer_index = 0;
    int send_res = 0;
    while ((out_buffer) && (out_buffer_len > 0)) {
        int res = send(client_sock, (char *)&out_buffer[out_buffer_index], out_buffer_len, 0);
        if (res <= 0) {
            send_res = res;
            break;
        }
        out_buffer_len -= res;
        out_buffer_index += res;
    }
    tls_buffer_clear(context);
    return send_res;
}

struct HTTPInfo HTTPSClient(const char* website) {
    unsigned char read_buffer[0xFFFF];
    unsigned char client_message[0xFFFF];

    int sockfd, portno = 443;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    struct HTTPInfo info;

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("[HTTPService Client] Error: opening socket\n");
        exit(0);
    }

    server = gethostbyname(website);
    if (server == NULL) {
        printf("[HTTPService Client] Error: no such host\n");
        exit(0);
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        printf("[HTTPService Client] Error: connecting\n");
        exit(0);
    }

    struct TLSContext *context = tls_create_context(0, TLS_V12);

    tls_make_exportable(context, 1);
    tls_client_connect(context);
    send_pending(sockfd, context);

    int read_size;
    while ((read_size = recv(sockfd, client_message, sizeof(client_message), 0)) > 0) {
        tls_consume_stream(context, client_message, read_size, NULL);
        send_pending(sockfd, context);
        if (tls_established(context)) {
            const char *request = "POST /growtopia/server_data.php HTTP/1.1\r\nUser-Agent: UbiServices_SDK_2019.Release.27_PC64_unicode_static\r\nHost: www.growtopia1.com\r\nAccept: */*\r\nConnection: close\r\ncontent-length: 0\r\n\r\n";
            if (!tls_make_ktls(context, sockfd)) send(sockfd, request, strlen(request), 0);
            else {
                tls_write(context, (unsigned char *)request, strlen(request));
                send_pending(sockfd, context);
            }
            int tempLen = tls_read(context, read_buffer, 0xFFFF - 1);
            if (tempLen != 0) info.bufferLen = tempLen;
        }
    }
    read_buffer[info.bufferLen] = '\0';
    info.buffer = read_buffer;
    SSL_CTX_free(context);
    return info;
}

void* HTTPSServer(void* unused) {
    int socket_desc, client_sock;
    socklen_t c;
    struct sockaddr_in server, client;
    const char msg[] = "HTTP/1.1 200 OK\r\nContent-length: 279\r\n\r\nserver|127.0.0.1\nport|17091\ntype|1\n#maint|maintenance\nbeta_server|beta.growtopiagame.com\nbeta_port|26999\nbeta_type|1\nbeta2_server|beta2.growtopiagame.com\nbeta2_port|26999\nbeta2_type|1\nbeta3_server|34.202.7.77\nbeta3_port|26999\nbeta3_type|1\ntype2|0\nmeta|localhost\nRTENDMARKERBS1001";

    #ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    #endif

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("[HTTPService Server] Error: Could not create socket\n");
        exit(0);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(userConfig.httpsPort);

    int enable = 1;
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, 4);

    if (bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("[HTTPService Server] Error: bind failed! If you are not a root user, please change it to 8080\n");
        exit(1);
    }

    listen(socket_desc, 3);

    c = sizeof(struct sockaddr_in);

    SSL* server_ctx = SSL_CTX_new(SSLv3_server_method());

    if (!server_ctx) {
        printf("[HTTPService Server] Error: creating server context");
        exit(-1);
    }

    tls_load_certificates(server_ctx, certPem, strlen(certPem));
    tls_load_private_key(server_ctx, keyPem, strlen(keyPem));

    if (!SSL_CTX_check_private_key(server_ctx)) {
        printf("[HTTPService Server] Error: Private key not loaded\n");
        exit(-2);
    }

    printf("[HTTPService Server] Log: HTTPS Server is enabled\n");

    while(1) {
        client_sock = accept(socket_desc, (struct sockaddr*)&client, &c);

        if (client_sock < 0) {
            printf("[HTTPService Server] Error: Accept failed\n");
            exit(-3);
        }

        SSL* client = SSL_new(server_ctx);
        if (!client) {
            printf("[HTTPService Server] Error: Error creating SSL Client");
            exit(-4);
        }

        SSL_set_fd(client, client_sock);

        if (SSL_accept(client)) {
            if (SSL_write(client, msg, strlen(msg)) < 0) printf("[HTTPService Server] Error: in SSL Write\n");
        } else printf("[HTTPService Server] Error: in handshake\n");
        SSL_shutdown(client);
        #ifdef _WIN32
        Sleep(500);
        #else
        usleep(500);
        #endif
#ifdef __WIN32
        shutdown(client_sock, SD_BOTH);
        closesocket(client_sock);
#else
        shutdown(client_sock, SHUT_RDWR);
        close(client_sock);
#endif
        SSL_free(client);
    }
    SSL_CTX_free(server_ctx);
}
