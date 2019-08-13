#ifndef ASYNC_SOCKET_CLIENT_H_
#define ASYNC_SOCKET_CLIENT_H_

#ifdef _cplusplus
extern "C" {
#endif

#define RCVBUFSIZE 200

int setupSocket(int port, void (*signal_handler) (int));
void readPacket(char *s);
void sendPacket(char *packet);

#ifdef _cplusplus
}
#endif

#endif
