#ifndef ASYNC_SOCKET_CLIENT_H_
#define ASYNC_SOCKET_CLIENT_H_

#ifdef _cplusplus
extern "C" {
#endif

#define RCVBUFSIZE 200

int setupSocket(int port, void (*signal_handler) (int));
void readPacket(char *s);
void sendPacket(char *packet);
// void readPacket(unsigned char *s, int numBytes);
// void sendPacket(const unsigned char *packet, int numBytes);

#ifdef _cplusplus
}
#endif

#endif
