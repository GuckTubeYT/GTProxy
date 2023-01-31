/** 
 @file  protocol.h
 @brief ENet protocol
*/
#ifndef __ENET_PROTOCOL_H__
#define __ENET_PROTOCOL_H__

#include "types.h"

enum
{
   ENET_PROTOCOL_MINIMUM_MTU             = 576,
   ENET_PROTOCOL_MAXIMUM_MTU             = 4096,
   ENET_PROTOCOL_MAXIMUM_PACKET_COMMANDS = 32,
   ENET_PROTOCOL_MINIMUM_WINDOW_SIZE     = 4096,
   ENET_PROTOCOL_MAXIMUM_WINDOW_SIZE     = 65536,
   ENET_PROTOCOL_MINIMUM_CHANNEL_COUNT   = 1,
   ENET_PROTOCOL_MAXIMUM_CHANNEL_COUNT   = 255,
   ENET_PROTOCOL_MAXIMUM_PEER_ID         = 0xFFF,
   ENET_PROTOCOL_MAXIMUM_FRAGMENT_COUNT  = 1024 * 1024
};

typedef enum _ENetProtocolCommand
{
   ENET_PROTOCOL_COMMAND_NONE               = 0,
   ENET_PROTOCOL_COMMAND_ACKNOWLEDGE        = 1,
   ENET_PROTOCOL_COMMAND_CONNECT            = 2,
   ENET_PROTOCOL_COMMAND_VERIFY_CONNECT     = 3,
   ENET_PROTOCOL_COMMAND_DISCONNECT         = 4,
   ENET_PROTOCOL_COMMAND_PING               = 5,
   ENET_PROTOCOL_COMMAND_SEND_RELIABLE      = 6,
   ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE    = 7,
   ENET_PROTOCOL_COMMAND_SEND_FRAGMENT      = 8,
   ENET_PROTOCOL_COMMAND_SEND_UNSEQUENCED   = 9,
   ENET_PROTOCOL_COMMAND_BANDWIDTH_LIMIT    = 10,
   ENET_PROTOCOL_COMMAND_THROTTLE_CONFIGURE = 11,
   ENET_PROTOCOL_COMMAND_SEND_UNRELIABLE_FRAGMENT = 12,
   ENET_PROTOCOL_COMMAND_COUNT              = 13,

   ENET_PROTOCOL_COMMAND_MASK               = 0x0F
} ENetProtocolCommand;

typedef enum _ENetProtocolFlag
{
   ENET_PROTOCOL_COMMAND_FLAG_ACKNOWLEDGE = (1 << 7),
   ENET_PROTOCOL_COMMAND_FLAG_UNSEQUENCED = (1 << 6),

   ENET_PROTOCOL_HEADER_FLAG_COMPRESSED = (1 << 14),
   ENET_PROTOCOL_HEADER_FLAG_SENT_TIME  = (1 << 15),
   ENET_PROTOCOL_HEADER_FLAG_MASK       = ENET_PROTOCOL_HEADER_FLAG_COMPRESSED | ENET_PROTOCOL_HEADER_FLAG_SENT_TIME,

   ENET_PROTOCOL_HEADER_SESSION_MASK    = (3 << 12),
   ENET_PROTOCOL_HEADER_SESSION_SHIFT   = 12
} ENetProtocolFlag;

#ifdef _MSC_VER
#pragma pack(push, 1)
#define ENET_PACKED
#elif defined(__GNUC__) || defined(__clang__)
#define ENET_PACKED __attribute__ ((packed))
#else
#define ENET_PACKED
#endif

typedef struct _ENetProtocolHeader
{
   enet_uint16 peerID;
   enet_uint16 sentTime;
} ENET_PACKED ENetProtocolHeader;

typedef struct _ENetProtocolHeaderUbisoft
{
   enet_uint16 integrity[3];
   enet_uint16 peerID;
   enet_uint16 sentTime;
} ENET_PACKED ENetProtocolHeaderUbisoft;

typedef struct _ENetProtocolCommandHeader
{
   enet_uint8 command;
   enet_uint8 channelID;
   enet_uint16 reliableSequenceNumber;
} ENET_PACKED ENetProtocolCommandHeader;

typedef struct _ENetProtocolAcknowledge
{
   ENetProtocolCommandHeader header;
   enet_uint16 receivedReliableSequenceNumber;
   enet_uint16 receivedSentTime;
} ENET_PACKED ENetProtocolAcknowledge;

typedef struct _ENetProtocolConnect
{
   ENetProtocolCommandHeader header;
   enet_uint16 outgoingPeerID;
   enet_uint8  incomingSessionID;
   enet_uint8  outgoingSessionID;
   enet_uint32 mtu;
   enet_uint32 windowSize;
   enet_uint32 channelCount;
   enet_uint32 incomingBandwidth;
   enet_uint32 outgoingBandwidth;
   enet_uint32 packetThrottleInterval;
   enet_uint32 packetThrottleAcceleration;
   enet_uint32 packetThrottleDeceleration;
   enet_uint32 connectID;
   enet_uint32 data;
} ENET_PACKED ENetProtocolConnect;

typedef struct _ENetProtocolVerifyConnect
{
   ENetProtocolCommandHeader header;
   enet_uint16 outgoingPeerID;
   enet_uint8  incomingSessionID;
   enet_uint8  outgoingSessionID;
   enet_uint32 mtu;
   enet_uint32 windowSize;
   enet_uint32 channelCount;
   enet_uint32 incomingBandwidth;
   enet_uint32 outgoingBandwidth;
   enet_uint32 packetThrottleInterval;
   enet_uint32 packetThrottleAcceleration;
   enet_uint32 packetThrottleDeceleration;
   enet_uint32 connectID;
} ENET_PACKED ENetProtocolVerifyConnect;

typedef struct _ENetProtocolBandwidthLimit
{
   ENetProtocolCommandHeader header;
   enet_uint32 incomingBandwidth;
   enet_uint32 outgoingBandwidth;
} ENET_PACKED ENetProtocolBandwidthLimit;

typedef struct _ENetProtocolThrottleConfigure
{
   ENetProtocolCommandHeader header;
   enet_uint32 packetThrottleInterval;
   enet_uint32 packetThrottleAcceleration;
   enet_uint32 packetThrottleDeceleration;
} ENET_PACKED ENetProtocolThrottleConfigure;

typedef struct _ENetProtocolDisconnect
{
   ENetProtocolCommandHeader header;
   enet_uint32 data;
} ENET_PACKED ENetProtocolDisconnect;

typedef struct _ENetProtocolPing
{
   ENetProtocolCommandHeader header;
} ENET_PACKED ENetProtocolPing;

typedef struct _ENetProtocolSendReliable
{
   ENetProtocolCommandHeader header;
   enet_uint16 dataLength;
} ENET_PACKED ENetProtocolSendReliable;

typedef struct _ENetProtocolSendUnreliable
{
   ENetProtocolCommandHeader header;
   enet_uint16 unreliableSequenceNumber;
   enet_uint16 dataLength;
} ENET_PACKED ENetProtocolSendUnreliable;

typedef struct _ENetProtocolSendUnsequenced
{
   ENetProtocolCommandHeader header;
   enet_uint16 unsequencedGroup;
   enet_uint16 dataLength;
} ENET_PACKED ENetProtocolSendUnsequenced;

typedef struct _ENetProtocolSendFragment
{
   ENetProtocolCommandHeader header;
   enet_uint16 startSequenceNumber;
   enet_uint16 dataLength;
   enet_uint32 fragmentCount;
   enet_uint32 fragmentNumber;
   enet_uint32 totalLength;
   enet_uint32 fragmentOffset;
} ENET_PACKED ENetProtocolSendFragment;

typedef union _ENetProtocol
{
   ENetProtocolCommandHeader header;
   ENetProtocolAcknowledge acknowledge;
   ENetProtocolConnect connect;
   ENetProtocolVerifyConnect verifyConnect;
   ENetProtocolDisconnect disconnect;
   ENetProtocolPing ping;
   ENetProtocolSendReliable sendReliable;
   ENetProtocolSendUnreliable sendUnreliable;
   ENetProtocolSendUnsequenced sendUnsequenced;
   ENetProtocolSendFragment sendFragment;
   ENetProtocolBandwidthLimit bandwidthLimit;
   ENetProtocolThrottleConfigure throttleConfigure;
} ENET_PACKED ENetProtocol;

enum
{
   ENET_SOCKS5_VERSION = 5,
   ENET_SOCKS5_AUTH_VERSION = 1,
   ENET_SOCKS5_AUTH_SUCCESS = 0,
};

typedef enum _ENetSocks5MethodType
{
   ENET_SOCKS5_METHOD_NOAUTH,
   ENET_SOCKS5_METHOD_GSSAPI,
   ENET_SOCKS5_METHOD_USERPASS,
   ENET_SOCKS5_METHOD_INVALID = 0xFF,
} ENetSocks5MethodType;

typedef enum _ENetSocks5CommandCode
{
   ENET_SOCKS5_COMMAND_CONNECT = 1,
   ENET_SOCKS5_COMMAND_BIND,
   ENET_SOCKS5_COMMAND_UDP_ASSOCIATE,
} ENetSocks5CommandCode;

typedef enum _ENetsocks5ConfigType
{
   ENET_SOCKS5_ADDRESS_IPV4 = 1,
   ENET_SOCKS5_ADDRESS_DOMAIN = 3,
   ENET_SOCKS5_ADDRESS_IPV6 = 4,
} ENetsocks5ConfigType;

typedef enum _ENetSocks5ReplyStatus
{
   ENET_SOCKS5_REPLY_SUCCEED,
   ENET_SOCKS5_REPLY_SOCKS_SERVER_FAILURE,
   ENET_SOCKS5_REPLY_CONNECTION_NOT_ALLOWED,
   ENET_SOCKS5_REPLY_NETWORK_UNREACHABLE,
   ENET_SOCKS5_REPLY_CONNECTION_REFUSED,
   ENET_SOCKS5_REPLY_TTL_EXPIRED,
   ENET_SOCKS5_REPLY_COMMAND_NOT_SUPPORTED,
   ENET_SOCKS5_REPLY_addressType_NOT_SUPPORTED,
} ENetSocks5ReplyStatus;

typedef struct _ENetSocks5MethodRequest
{
   enet_uint8 version;
   enet_uint8 methodCount;
   enet_uint8 methods[255];
} ENET_PACKED ENetSocks5MethodRequest;

typedef struct _ENetSocks5MethodResponse
{
   enet_uint8 version;
   enet_uint8 method;
} ENET_PACKED ENetSocks5MethodResponse;

typedef struct _ENetSocks5AuthResponse
{
   enet_uint8 version;
   enet_uint8 status;
} ENET_PACKED ENetSocks5AuthResponse;

typedef struct _ENetSocks5Connection
{
   enet_uint8 version;
   union {
      enet_uint8 command;
      enet_uint8 status;
   };
   enet_uint8 reserved;
   enet_uint8 addressType;
   enet_uint32 addressHost; // since we will use ipv4 anyways
   enet_uint16 addressPort;
} ENET_PACKED ENetSocks5Connection;

typedef struct _ENetSocks5UDP
{
   enet_uint16 reserved;
   enet_uint8 fragment;
   enet_uint8 addressType;
   enet_uint32 addressHost; // since we will use ipv4 anyways
   enet_uint16 addressPort;
} ENET_PACKED ENetSocks5UDP;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif /* __ENET_PROTOCOL_H__ */

